/*
 * server.c
 * This file is part of Card Jitsu Snow
 *
 * Copyright (C) 2016 - Félix Arreola Rodríguez
 *
 * Blokus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Blokus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Blokus. If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Para el manejo de red */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <sys/types.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/poll.h>

/* Para pipe */
#include <unistd.h>
#include <signal.h>

#include <errno.h>

#include "snow.h"
#include "server_ninja.h"

#include "netplay.h"

#define NICK_SIZE 16
#define SERV_PORT 3301

#define MAX_FDS 50

enum {
	WAITING_CLIENTS = 0,
	WAITING_ACTIONS
};

typedef struct SnowFight {
	int id;
	int fds[3];
	char nicks[3][NICK_SIZE];
	
	int running;
	
	int estado;
	
	/* Otra info aquí */
	int escenario[5][9];
	int acciones[5][9];
	
	int ready[3];
	
	ServerNinja *water;
	ServerNinja *fire;
	ServerNinja *snow;
	
	int enemys;
	ServerEnemy *enemigos[4];
	
	struct SnowFight *next;
} SnowFight;

typedef struct PendingWrite {
	int fd;
	char buffer[128];
	int size;
	
	int close;
	
	struct PendingWrite *next;
} PendingWrite;

/* Prototipos de función */
void cancel_writes (int fd);
void start_tabla (SnowFight *tabla);

static SnowFight *lista_partidas;
static PendingWrite *writes;

static int server_fd;
static struct pollfd vigilar[MAX_FDS];
static int fds;
int agregar_fd[MAX_FDS];
int agregar_count = 0;
int eliminar_fd[MAX_FDS];
int eliminar_count = 0;

int ids_tablas = 1;

static int sigterm_pipe_fds[2] = { -1, -1 };

static void sigterm_handler (int signum) {
	fprintf (stderr, "SIGTERM SIGINT Handler\n");
	if (sigterm_pipe_fds[1] >= 0) {
		if (write (sigterm_pipe_fds[1], "", 1) == -1 ) {
			fprintf (stderr, "Write to sigterm_pipe failed.\n");
		}
		close (sigterm_pipe_fds[1]);
		sigterm_pipe_fds[1] = -1;
	}
}

int inicializar_socket (void) {
	int server, res;
	struct sockaddr_in6 serv_bind;
	
	server = socket (AF_INET6, SOCK_STREAM, 0);
	
	if (server < 0) {
		perror ("Error al crear el socket");
		
		return -1;
	}
	
	memset (&serv_bind, 0, sizeof (serv_bind));
	
	serv_bind.sin6_family = AF_INET6;
	serv_bind.sin6_port = htons (SERV_PORT);
	memcpy (&serv_bind.sin6_addr.s6_addr, &in6addr_any, sizeof (in6addr_any));
	
	if (bind (server, (struct sockaddr *) &serv_bind, sizeof (serv_bind)) < 0) {
		perror ("Error en el bind");
		
		return -1;
	}
	
	res = listen (server, 6);
	
	if (res < 0) {
		perror ("Error en el listen");
		
		return -1;
	}
	
	return server;
}

void agregar_a_fd (int fd) {
	agregar_fd[agregar_count] = fd;
	
	agregar_count++;
}

void eliminar_a_fd (int fd) {
	eliminar_fd[eliminar_count] = fd;
	
	eliminar_count++;
	
	close (fd);
}

void agregar_poll (void) {
	int g;
	for (g = 0; g < agregar_count; g++) {
		vigilar[fds].fd = agregar_fd[g];
		vigilar[fds].events = POLLIN;
		vigilar[fds].revents = 0;
		
		fds++;
	}
	
	agregar_count = 0;
}

void eliminar_poll (void) {
	int g, h;
	
	/* Recorrer la lista para eliminar los fds viejos */
	for (g = 0; g < eliminar_count; g++) {
		for (h = 1; h < fds; h++) {
			if (eliminar_fd[g] == vigilar[h].fd) {
				cancel_writes (vigilar[h].fd);
				/* Sustituir la posición h con la fds - 1 */
				vigilar[h] = vigilar[fds - 1];
				
				fds--;
				break;
			}
		}
	}
	
	eliminar_count = 0;
}

void agregar_write (int fd, char *buffer, int size, int close) {
	PendingWrite **pos, *new;
	int g;
	
	pos = &writes;
	
	while (*pos != NULL) {
		pos = &((*pos)->next);
	}
	
	new = (PendingWrite *) malloc (sizeof (PendingWrite));
	
	memcpy (new->buffer, buffer, size);
	new->size = size;
	new->fd = fd;
	new->close = close;
	new->next = NULL;
	
	*pos = new;
	
	for (g = 1; g < fds; g++) {
		if (vigilar[g].fd == fd) {
			vigilar[g].events |= POLLOUT;
			break;
		}
	}
}

void do_writes (int fd) {
	PendingWrite *pos, **prev;
	int g;
	
	pos = writes;
	prev = &writes;
	while (pos != NULL && pos->fd != fd) {
		prev = &((*prev)->next);
		pos = pos->next;
	}
	
	if (pos != NULL) {
		printf ("Se envia una escritura pendiente para el fd [%i]. Size: %i\n", fd, pos->size);
		/* Ejecutar el write */
		write (pos->fd, pos->buffer, pos->size);
		
		if (pos->close) {
			printf ("Y después se solicitó cerrar\n");
			eliminar_a_fd (pos->fd);
		}
		*prev = pos->next;
		free (pos);
	} else {
		/* Desactivar su bandera de POLLOUT */
		for (g = 1; g < fds; g++) {
			if (vigilar[g].fd == fd) {
				vigilar[g].events = POLLIN;
				break;
			}
		}
	}
}

void cancel_writes (int fd) {
	PendingWrite *pos, **prev, *n;
	
	prev = &writes;
	pos = writes;
	while (pos != NULL) {
		if (pos->fd == fd) {
			/* Eliminar este write pendiente */
			(*prev)->next = pos->next;
			n = pos->next;
			
			free (pos);
			pos = n;
			printf ("Se canceló una escritura pendiente para el fd [%i]\n", fd);
		} else {
			pos = pos->next;
			prev = &((*prev)->next);
		}
	}
}

/* Manejo de tablas */
SnowFight *find_table_by_fd (int fd) {
	SnowFight *n;
	
	n = lista_partidas;
	
	while (n != NULL) {
		if (n->fds[0] == fd || n->fds[1] == fd || n->fds[2] == fd) {
			return n;
		}
		n = n->next;
	}
	
	return NULL;
}

SnowFight *find_by_empty_element (int element) {
	SnowFight *n;
	
	n = lista_partidas;
	
	/* Localizar una partida a la que le falte un ninja del elemento solicitado */
	while (n != NULL) {
		if (n->fds[element] == -1 && !n->running) {
			return n;
		}
		n = n->next;
	}
	
	return NULL;
}

SnowFight *crear_tabla (int fd, NetworkMessage *msg) {
	SnowFight *new, *n;
	unsigned char buffer_send[128];
	int element;
	
	element = msg->element;
	
	if (element < 0 || element > 2) {
		printf ("El elemento que quiere hacer join es inválido\n");
		return NULL;
	}
	
	new = (SnowFight *) malloc (sizeof (SnowFight));
	
	memset (new, 0, sizeof (SnowFight));
	
	new->next = NULL;
	new->running = FALSE;
	if (lista_partidas == NULL) {
		lista_partidas = new;
	} else {
		n = lista_partidas;
		while (n->next != NULL) n = n->next;
		n->next = new;
	}
	
	new->id = ids_tablas++;
	
	/* Rellenar los FD's */
	new->fds[1] = new->fds[2] = new->fds[0] = -1;
	
	new->fds[element] = fd;
	
	/* Copiar el nick */
	strncpy (new->nicks[element], msg->nick, NICK_SIZE - 1);
	new->nicks[element][NICK_SIZE - 1] = 0;
	
	/* Rellenar con la firma del protocolo SN */
	buffer_send[0] = 'S';
	buffer_send[1] = 'N';
	
	/* Poner el campo de la versión */
	buffer_send[2] = 0;
	
	/* El campo de tipo */
	buffer_send[3] = NET_TYPE_ACCEPT;
	
	/* Indicar cuandos clientes más hay conectados */
	buffer_send[4] = 0;
	
	agregar_write (fd, buffer_send, 5, 0);
	
	printf ("Se creó una tabla [%i] y se le envió ACCEPTED al jugador [%i]\n", new->id, fd);
}

void join_tabla (SnowFight *tabla, int fd, NetworkMessage *msg) {
	int element;
	unsigned char buffer_send[128];
	int g, otros, h;
	
	element = msg->element;
	
	if (element < 0 || element > 2) {
		printf ("Intentando hacer JOIN a una tabla con un elemento inválido\n");
		return;
	}
	
	if (tabla->fds[element] != -1) {
		printf ("Error, JOIN a una mesa con un elemento ya ocupado\n");
		return;
	}
	
	/* Preparar el mensaje de accepted */
	buffer_send[0] = 'S';
	buffer_send[1] = 'N';
	
	/* Poner el campo de la versión */
	buffer_send[2] = 0;
	
	/* El campo de tipo */
	buffer_send[3] = NET_TYPE_ACCEPT;
	
	otros = 0;
	h = 5;
	for (g = 0; g < 3; g++) {
		if (tabla->fds[g] != -1) {
			otros++;
			
			buffer_send[h] = g;
			memcpy (&buffer_send[h + 1], tabla->nicks[g], NICK_SIZE);
			h += NICK_SIZE + 1;
		}
	}
	
	buffer_send[4] = otros;
	
	agregar_write (fd, buffer_send, h, 0);
	
	/* Meter el cliente en la tabla */
	tabla->fds[element] = fd;
	memcpy (tabla->nicks[element], msg->nick, NICK_SIZE);
	tabla->nicks[element][NICK_SIZE - 1] = 0;
	
	/* Enviar join a los otros clientes de la tabla */
	/* Preparar el mensaje de accepted */
	buffer_send[0] = 'S';
	buffer_send[1] = 'N';
	
	/* Poner el campo de la versión */
	buffer_send[2] = 0;
	
	/* El campo de tipo */
	buffer_send[3] = NET_TYPE_OTHER_JOIN;
	
	buffer_send[4] = element;
	memcpy (&buffer_send[5], msg->nick, NICK_SIZE);
	buffer_send[5 + NICK_SIZE - 1] = 0;
	
	for (g = 0; g < 3; g++) {
		if (tabla->fds[g] != fd && tabla->fds[g] != -1) {
			agregar_write (tabla->fds[g], buffer_send, 5 + NICK_SIZE, 0);
		}
	}
	
	printf ("El cliente [%i] se unió a la tabla [%i] y se le envió ACCEPTED.\n", fd, tabla->id);
	
	if (tabla->fds[0] != -1 && tabla->fds[1] != -1 && tabla->fds[2] != -1) {
		start_tabla (tabla);
	}
}

void start_tabla (SnowFight *tabla) {
	int r, g, h, i;
	char buffer_send[128];
	int tipo;
	
	/* Vaciar el escenario */
	memset (tabla->escenario, 0, sizeof (tabla->escenario));
	memset (tabla->acciones, 0, sizeof (tabla->acciones));
	
	tabla->escenario[0][2] = tabla->escenario[0][6] = tabla->escenario[4][2] = tabla->escenario[4][6] = ROCK;
	
	/* Colocar a los ninjas en posiciones aleatorias */
	
	/* Sacar uno aleatorio para la primera posición */
	r = NINJA_FIRE + RANDOM (3);
	
	tabla->escenario[0][0] = r;
	
	/* Crear el objeto correspondiente */
	if (r == NINJA_FIRE) {
		tabla->fire = create_server_ninja (0, 0, r);
	} else if (r == NINJA_WATER) {
		tabla->water = create_server_ninja (0, 0, r);
	} else if (r == NINJA_SNOW) {
		tabla->snow = create_server_ninja (0, 0, r);
	}
	
	/* Sacar otro que no sea el primero para la posición de en medio */
	do {
		r = NINJA_FIRE + RANDOM (3);
	} while (r == tabla->escenario[0][0]);
	
	tabla->escenario[2][0] = r;
	
	/* Crear el objeto correspondiente */
	if (r == NINJA_FIRE) {
		tabla->fire = create_server_ninja (0, 2, r);
	} else if (r == NINJA_WATER) {
		tabla->water = create_server_ninja (0, 2, r);
	} else if (r == NINJA_SNOW) {
		tabla->snow = create_server_ninja (0, 2, r);
	}
	
	/* Determinar el último ninja */
	r = NINJA_FIRE;
	if (tabla->escenario[0][0] == r || tabla->escenario[2][0] == r) r++;
	if (tabla->escenario[0][0] == r || tabla->escenario[2][0] == r) r++;
	
	tabla->escenario[4][0] = r;
	
	/* Crear el objeto correspondiente */
	if (r == NINJA_FIRE) {
		tabla->fire = create_server_ninja (0, 4, r);
	} else if (r == NINJA_WATER) {
		tabla->water = create_server_ninja (0, 4, r);
	} else if (r == NINJA_SNOW) {
		tabla->snow = create_server_ninja (0, 4, r);
	}
	
	tabla->enemys = 1 + RANDOM(3);
	for (g = 0; g < tabla->enemys; g++) {
		i = RANDOM(3);
		i = 0; /* FIXME */
		/* Como es la primera ronda, las posiciones son seguras */
		do {
			h = RANDOM(5);
		} while (tabla->escenario[h][8] != NONE);
		
		tabla->escenario[h][8] = i + ENEMY_SLY;
		
		tabla->enemigos[g] = create_server_enemy (8, h, i + ENEMY_SLY);
	}
	
	/* Preparar el mensaje de start + info de inicio */
	buffer_send[0] = 'S';
	buffer_send[1] = 'N';
	
	/* Poner el campo de la versión */
	buffer_send[2] = 0;
	
	/* El campo de tipo */
	buffer_send[3] = NET_TYPE_START_INFO;
	
	/* El fondo a usar */
	buffer_send[4] = RANDOM(3);
	
	/* El tipo de logro para el bonus round */
	buffer_send[5] = RANDOM(3);
	
	/* Cantidad de objetos que vamos a enviar */
	buffer_send[6] = 4 + 3 + tabla->enemys; /* 4 rocas + 3 ninjas + 3 enemigos */
	
	r = 7;
	/* Colocar coordenadas iniciales de los objetos */
	for (g = 0; g < 5; g++) {
		for (h = 0; h < 9; h++) {
			if (tabla->escenario[g][h] != NONE) {
				buffer_send[r] = tabla->escenario[g][h];
				buffer_send[r + 1] = h;
				buffer_send[r + 2] = g;
				
				r = r + 3;
			}
		}
	}
	
	/* Enviar el mensaje de start */
	for (g = 0; g < 3; g++) {
		if (tabla->fds[g] != -1) {
			agregar_write (tabla->fds[g], buffer_send, r, 0);
		}
	}
	tabla->running = TRUE;
	tabla->estado = WAITING_CLIENTS;
	tabla->ready[0] = tabla->ready[1] = tabla->ready[2] = 0;
	
	/* Cambiar los ENEMY_SLY por ENEMY_1 */
	for (g = 0; g < tabla->enemys; g++) {
		tabla->escenario[tabla->enemigos[g]->y][tabla->enemigos[g]->x] = ENEMY_1 + g;
	}
}

void manage_client_ready (SnowFight *tabla, int fd) {
	int g;
	char buffer_send[128];
	if (!tabla->running || tabla->estado != WAITING_CLIENTS) {
		printf ("Error en la tabla. El cliente [%i] envió un ready cuando la tabla no está lista\n", fd);
	}
	
	for (g = 0; g < 3; g++) {
		if (tabla->fds[g] == fd) break;
	}
	
	/* Poner ready a ese cliente */
	tabla->ready[g] = 1;
	
	g = tabla->ready[0] + tabla->ready[1] + tabla->ready[2];
	
	if (g == 3) { /* FIXME: Debe ser igual a la cantidad de clientes conectados en la tabla */
		/* Enviar el "ASK actions" a los clientes */
		buffer_send[0] = 'S';
		buffer_send[1] = 'N';
	
		/* Poner el campo de la versión */
		buffer_send[2] = 0;
	
		/* El campo de tipo */
		buffer_send[3] = NET_TYPE_ASK_ACTIONS;
		
		for (g = 0; g < 3; g++) {
			if (tabla->fds[g] != -1) {
				agregar_write (tabla->fds[g], buffer_send, 4, 0);
			}
		}
		
		tabla->estado = WAITING_ACTIONS;
		tabla->ready[0] = tabla->ready[1] = tabla->ready[2] = 0;
	}
}

void manage_client_actions (SnowFight *tabla, int fd, NetworkMessage *msg) {
	int element, g;
	int acciones[5][9];
	char buffer_send[128];
	ServerNinja *ninja;
	
	if (!tabla->running || tabla->estado != WAITING_ACTIONS) {
		printf ("Error en la tabla. El cliente [%i] envió una acción cuando la tabla no está recibiendo acciones\n", fd);
	}
	
	/* Validar la acción */
	if (msg->action.type == ACTION_MOVE) {
		for (g = 0; g < 3; g++) {
			if (tabla->fds[g] == fd) {
				element = g;
				break;
			}
		}
		
		/* Ya localizado el cliente, validar que el movimiento sea del ninja al que pertenece */
		if (msg->action.object != NINJA_FIRE + element) {
			printf ("Error en la tabla. El cliente [%i] trata de mover un ninja que no es él\n", fd);
			return;
		}
		
		if (msg->action.x < 0 || msg->action.x > 8 || msg->action.y < 0 || msg->action.y > 4) {
			printf ("Error en la tabla. El cliente [%i] envió coordenadas inválidas para un movimiento\n", fd);
			return;
		}
		memset (acciones, 0, sizeof (acciones));
		
		/* Preguntar las acciones locales */
		if (msg->action.object == NINJA_FIRE) {
			ninja = tabla->fire;
			ask_fire_actions (tabla->fire, tabla->escenario, acciones);
		} else if (msg->action.object == NINJA_WATER) {
			ninja = tabla->water;
			ask_water_actions (tabla->water, tabla->escenario, acciones);
		} else if (msg->action.object == NINJA_SNOW) {
			ninja = tabla->snow;
			ask_snow_actions (tabla->snow, tabla->escenario, acciones);
		}
		
		/* Si un movimiento choca contra el siguiente de otro ninja no es válido */
		if (tabla->water != NULL) {
			if (msg->action.y == tabla->water->next_y && msg->action.x == tabla->water->next_x) {
				return;
			}
		}
		if (tabla->fire != NULL) {
			if (msg->action.y == tabla->fire->next_y && msg->action.x == tabla->fire->next_x) {
				return;
			}
		}
		if (tabla->snow != NULL) {
			if (msg->action.y == tabla->snow->next_y && msg->action.x == tabla->snow->next_x) {
				return;
			}
		}
		
		/* Rellenar con la firma del protocolo SN */
		buffer_send[0] = 'S';
		buffer_send[1] = 'N';
	
		/* Poner el campo de la versión */
		buffer_send[2] = 0;
	
		/* El campo de tipo */
		buffer_send[3] = NET_TYPE_ACTION;
	
		/* El ninja */
		buffer_send[4] = msg->action.object;
	
		/* Tipo de accion */
		buffer_send[5] = msg->action.type;
	
		/* Coordenadas x, y */
		buffer_send[6] = msg->action.x;
		buffer_send[7] = msg->action.y;
		if (acciones[msg->action.y][msg->action.x] & ACTION_MOVE) {
			move_next (ninja, msg->action.x, msg->action.y);
			/* Es válido, enviar el movimiento a los otros  */
			for (g = 0; g < 3; g++) {
				if (tabla->fds[g] != -1) {
					agregar_write (tabla->fds[g], buffer_send, 8, 0);
				}
			}
		}
	}
}

#if 0
void remove_table (GameBoard *tabla) {
	GameBoard *prev;
	/* Localizar la tabla en la lista ligada y eliminarla */
	if (lista_tablas == tabla) {
		/* Primera de la lista */
		lista_tablas = lista_tablas->next;
	} else {
		prev = lista_tablas;
		
		while (prev->next != tabla) {
			prev = prev->next;
		}
		
		prev->next = tabla->next;
	}
	
	free (tabla);
}

void leave_table_by_close (int fd) {
	int slot;
	unsigned char buffer_send[128];
	int g, h;
	GameBoard *tabla;
	
	tabla = find_table_by_fd (fd);
	if (tabla == NULL) return;
	
	/* Localizar el jugador que se cerró */
	for (g = 0; g < 4; g++) {
		if (tabla->fds[g] == fd) {
			slot = g;
			break;
		}
	}
	
	printf ("Un cliente abandonó la tabla [%i].\n", tabla->id);
	/* Si la mesa ya tiene a los 4 jugadores completos, cerrar toda la mesa */
	if (tabla->clientes == 4) {
		/* TODO: Cerrar esta tabla y enviar a todos los clientes un fin */
		buffer_send[0] = 'B';
		buffer_send[1] = 'K';

		/* Poner el campo de la versión */
		buffer_send[2] = 0;

		/* El campo de tipo */
		buffer_send[3] = TYPE_FIN;
		
		buffer_send[4] = NET_DISCONNECT_NETERROR;
		
		for (g = 0; g < 4; g++) {
			if (slot == g) continue;
			
			agregar_write (tabla->fds[g], buffer_send, 5, 1);
		}
		
		printf ("La tabla [%i] será cerrada porque está a medias de un juego\n", tabla->id);
		remove_table (tabla);
	} else {
		/* Enviar un mensaje de que un cliente se salió, para que otro cliente se pueda unir */
		buffer_send[0] = 'B';
		buffer_send[1] = 'K';

		/* Poner el campo de la versión */
		buffer_send[2] = 0;

		/* El campo de tipo */
		buffer_send[3] = TYPE_LEAVE_PLAYER;

		/* Color del nuevo jugador */
		buffer_send[4] = COLOR_AZUL + slot;
		
		for (g = 0; g < 4; g++) {
			if (slot == g || tabla->fds[g] == -1) continue;
			
			agregar_write (tabla->fds[g], buffer_send, 5, 0);
		}
		
		/* Eliminar su fd */
		tabla->fds[slot] = -1;
		memset (tabla->nicks[slot], 0, NICK_SIZE);
		
		tabla->clientes--;
		
		printf ("La tabla [%i] se removió un jugador a la espera\n", tabla->id);
		if (tabla->clientes == 0) {
			printf ("La tabla [%i] se cerró porque ya no hay más jugadores a la espera\n", tabla->id);
			
			remove_table (tabla);
		}
	}
}
#endif
int main (int argc, char *argv[]) {
	struct sigaction act;
	sigset_t empty_mask;
	int res, g;
	char buffer_ip[100], buffer_port[20];
	char buffer_data[2000];
	struct sockaddr_in6 cliente;
	socklen_t cliente_size;
	int nuevo, fd;
	
	int inicio;
	
	NetworkMessage message;
	SnowFight *tabla;
	
	srand ((unsigned int) time (NULL));
	server_fd = inicializar_socket ();
	
	if (server_fd < 0) {
		printf ("Falló al inicializar el Socket del lado del servidor\n");
		return EXIT_FAILURE;
	}
	
	lista_partidas = NULL;
	writes = NULL;
	
	if (pipe (sigterm_pipe_fds) != 0) {
		perror ("Failed to create SIGTERM pipe");
		sigterm_pipe_fds[0] = -1;
	}
	
	/* Instalar un manejador de señales para SIGTERM */
	sigemptyset (&empty_mask);
	act.sa_mask    = empty_mask;
	act.sa_flags   = 0;
	act.sa_handler = &sigterm_handler;
	if (sigaction (SIGTERM, &act, NULL) < 0) {
		perror ("Failed to register SIGTERM handler");
	}
	
	if (sigaction (SIGINT, &act, NULL) < 0) {
		perror ("Failed to register SIGINT handler");
	}
	
	/* Preparar el Poll */
	vigilar[0].fd = server_fd;
	vigilar[0].events = POLLIN;
	
	fds = 1;
	inicio = 1;
	
	if (sigterm_pipe_fds[0] != -1) {
		vigilar[1].fd = sigterm_pipe_fds[0];
		vigilar[1].events = POLLIN;
		
		fds = 2;
		inicio = 2;
	}
	
	do {
		/* A la espera de mensajes de red */
		res = poll (vigilar, fds, 30);
		
		if (res < 0) {
			/* Error en poll */
			if (errno == EINTR) {
				/* El poll fué detenido por manejo de señales */
				continue;
			}
			perror ("Error en poll");
			break;
		}
		
		if (res == 0) continue; /* Nada que vigilar */
		
		if (vigilar[0].revents & POLLIN) {
			/* Evento en el servidor */
			cliente_size = sizeof (cliente);
			nuevo = accept (server_fd, (struct sockaddr *) &cliente, &cliente_size);
			
			getnameinfo ((struct sockaddr *) &cliente, cliente_size, buffer_ip, sizeof (buffer_ip), buffer_port, sizeof (buffer_port), NI_NUMERICHOST | NI_NUMERICSERV);
			
			printf ("Nueva conexión entrante desde la IP: %s:%s => [%i]\n", buffer_ip, buffer_port, nuevo);
			agregar_a_fd (nuevo);
		}
		
		if (inicio == 2 && (vigilar[1].revents & POLLIN)) {
			/* Leer el pipe de cierre */
			res = read (sigterm_pipe_fds[0], buffer_data, sizeof (buffer_data));
			
			if (res > 0) {
				/* Debemos cerrar la aplicación */
				break;
			}
		}
		/* Hacer los write antes de los reads */
		for (g = 1; g < fds; g++) {
			if (vigilar[g].revents & POLLOUT) {
				/* Puedo hacer un write limpio en este socket */
				do_writes (vigilar[g].fd);
			}
		}
		
		eliminar_poll ();
		
		for (g = 1; g < fds; g++) {
			if (vigilar[g].revents & POLLERR) {
				/* Error en este socket, marcarlo para eliminación y eliminarlo de la tabla */
				printf ("[%i] Error en socket por POLLERR\n", vigilar[g].fd);
				// FIXME: leave_table_by_close (vigilar[g].fd);
				eliminar_a_fd (vigilar[g].fd);
			} else if ((vigilar[g].revents & POLLIN) || (vigilar[g].revents & POLLHUP)) {
				fd = vigilar[g].fd;
				
				res = read (fd, buffer_data, sizeof (buffer_data));
				
				if (res == 0) {
					/* Cierre de conexión */
					eliminar_a_fd (fd);
					printf ("[%i] Cierre normal\n", vigilar[g].fd);
					// FIXME: leave_table_by_close (fd);
					continue;
				} else if (res < 0) {
					perror ("Error en read");
					eliminar_a_fd (fd);
					
					// FIXME: leave_table_by_close (fd);
					
					continue;
				}
				
				/* Procesar los datos */
				if (unpack (&message, buffer_data, res) < 0) {
					printf ("[%i] Recibí un paquete mal estructurado\n", vigilar[g].fd);
					continue;
				}
				
				tabla = find_table_by_fd (fd);
				
				if (message.type == NET_TYPE_JOIN) {
					/* Si es una conexión inicial, verificar que no esté sentado en una tabla */
					if (tabla != NULL) {
						printf ("[%i] Error, este cliente envió un mensaje JOIN cuando ya está sentado en una tabla\n", fd);
					} else {
						/* Buscar una tabla para sentar al cliente */
						tabla = find_by_empty_element (message.element);
						
						if (tabla == NULL) {
							/* Crear una nueva tabla */
							tabla = crear_tabla (fd, &message);
						} else {
							/* Hacer join a esta tabla */
							join_tabla (tabla, fd, &message);
						}
					}
				} else if (message.type == NET_TYPE_CLIENT_READY) {
					/* Debe estar sentado en una tabla */
					if (tabla == NULL) {
						printf ("[%i] Error, este cliente envia un ready cuando no está sentado en una tabla\n", fd);
					} else {
						manage_client_ready (tabla, fd);
					}
				} else if (message.type == NET_TYPE_ACTION) {
					/* Debe estar sentado en una tabla */
					if (tabla == NULL) {
						printf ("[%i] Error, este cliente envia un movimiento cuando no está sentado en una tabla\n", fd);
					} else {
						manage_client_actions (tabla, fd, &message);
					}
				} else {
					printf ("[%i] Mensaje desconocido para el servidor\n", fd);
				}
				
				
			}
		}
		
		eliminar_poll ();
		
		agregar_poll ();
	} while (1);
	
	/* Ejecutar cierres y limpieza aquí */
	
	return 0;
}
