/*
 * server.c
 * This file is part of Card Jitsu Snow
 *
 * Copyright (C) 2016 - Félix Arreola Rodríguez
 *
 * Card Jitsu Snow is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Card Jitsu Snow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Card Jitsu Snow. If not, see <http://www.gnu.org/licenses/>.
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
#include "server_timer.h"

#include "server_ia.h"

#define NICK_SIZE 16

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
	
	int clientes;
	
	/* Otra info aquí */
	int escenario[5][9];
	
	int ready[3];
	int round;
	
	union {
		struct {
			ServerNinja *fire;
			ServerNinja *snow;
			ServerNinja *water;
		};
		ServerNinja *ninjas[3];
	};
	
	int enemys;
	ServerEnemy *enemigos[4];
	
	struct SnowFight *next;
} SnowFight;

/* Prototipos de función */
void start_tabla (SnowFight *tabla);
void calculate_actions (SnowFight *tabla);
void remove_table (SnowFight *tabla);

static SnowFight *lista_partidas;

static int server_fd;

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
		if (n->fds[element - 1] == -1 && !n->running) {
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
	
	if (element < NINJA_FIRE || element > NINJA_WATER) {
		printf ("Error. El elemento que quiere hacer join es inválido\n");
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
	new->round = 1;
	
	/* Rellenar los FD's */
	new->fds[1] = new->fds[2] = new->fds[0] = -1;
	
	new->fds[element - 1] = fd;
	
	/* Copiar el nick */
	strncpy (new->nicks[element - 1], msg->nick, NICK_SIZE - 1);
	new->nicks[element - 1][NICK_SIZE - 1] = 0;
	
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
	
	//printf ("Se creó una tabla «%i» y se le envió ACCEPTED al jugador [%i]\n", new->id, fd);
	
#ifdef STAND_ALONE
	start_tabla (new);
#endif
}

void join_tabla (SnowFight *tabla, int fd, NetworkMessage *msg) {
	int element;
	unsigned char buffer_send[128];
	int g, otros, h;
	
	element = msg->element;
	
	if (element < NINJA_FIRE || element > NINJA_WATER) {
		printf ("Intentando hacer JOIN a una tabla con un elemento inválido\n");
		return;
	}
	
	if (tabla->fds[element - 1] != -1) {
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
			
			buffer_send[h] = NINJA_FIRE + g;
			memcpy (&buffer_send[h + 1], tabla->nicks[g], NICK_SIZE);
			h += NICK_SIZE + 1;
		}
	}
	
	buffer_send[4] = otros;
	
	agregar_write (fd, buffer_send, h, 0);
	
	/* Meter el cliente en la tabla */
	tabla->fds[element - 1] = fd;
	memcpy (tabla->nicks[element - 1], msg->nick, NICK_SIZE);
	tabla->nicks[element - 1][NICK_SIZE - 1] = 0;
	
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
	
	//printf ("El cliente [%i] se unió a la tabla «%i» y se le envió ACCEPTED.\n", fd, tabla->id);
	
	if (tabla->fds[0] != -1 && tabla->fds[1] != -1 && tabla->fds[2] != -1) {
		start_tabla (tabla);
	}
}

void generar_enemigos (SnowFight *tabla, int bonus) {
	int g, h, i, j;
	tabla->enemys = 1 + RANDOM(3);
	
	if (bonus) {
		tabla->enemys = 4;
	}
	// FIXME: tabla->enemys = 3;
	//printf ("Generando enemigos para la tabla «%i». Total = %i\n", tabla->id, tabla->enemys);
	for (g = 0; g < tabla->enemys; g++) {
		/* I determina el tipo de enemigo */
		i = RANDOM(3);
		// FIXME: i = 2;
		do {
			h = RANDOM(5);
			j = 8;
			
			if (tabla->round != 1 && tabla->escenario[h][j] != NONE) {
				j = 7;
			}
		} while (tabla->escenario[h][j] != NONE);
		
		//printf ("Coordenadas para un enemigo: %i, %i\n");
		tabla->escenario[h][j] = ENEMY_1 + g;
		
		tabla->enemigos[g] = create_server_enemy (j, h, i + ENEMY_SLY);
	}
}

void start_tabla (SnowFight *tabla) {
	int r, g, h, i, ninjas;
	char buffer_send[128];
	int tipo, good;
	
	/* Vaciar el escenario */
	memset (tabla->escenario, 0, sizeof (tabla->escenario));
	
	tabla->escenario[0][2] = tabla->escenario[0][6] = tabla->escenario[4][2] = tabla->escenario[4][6] = ROCK;
	
	/* Colocar a los ninjas en posiciones aleatorias */
	ninjas = 0;
	int random_pos[3] = {-1, -1, -1};
	for (g = 0; g < 3; g++) {
		if (tabla->fds[g] == -1) continue;
		/* Generar una posición */
		do {
			good = 1;
			r = RANDOM(3);
			
			for (h = 0; h < 3; h++) {
				if (random_pos[h] == r) good = 0;
			}
		} while (!good);
		random_pos[g] = r;
		ninjas++;
	}
	
	for (g = 0; g < 3; g++) {
		if (tabla->fds[g] == -1) continue;
		
		tabla->ninjas[g] = create_server_ninja (0, random_pos[g] * 2, NINJA_FIRE + g);
		
		tabla->escenario[random_pos[g] * 2][0] = NINJA_FIRE + g;
	}
	
	generar_enemigos (tabla, 0);
	
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
	buffer_send[6] = 4 + ninjas + tabla->enemys; /* 4 rocas + 3 ninjas + 3 enemigos */
	
	r = 7;
	/* Colocar coordenadas iniciales de los objetos, excepto los enemigos iniciales */
	for (g = 0; g < 5; g++) {
		for (h = 0; h < 9; h++) {
			if (tabla->escenario[g][h] != NONE && tabla->escenario[g][h] < ENEMY_SLY) {
				buffer_send[r] = tabla->escenario[g][h];
				buffer_send[r + 1] = h;
				buffer_send[r + 2] = g;
				
				r = r + 3;
			}
		}
	}
	
	/* Mandar los enemigos en el mismo orden que están en el arreglo */
	for (g = 0; g < 4; g++) {
		if (tabla->enemigos[g] == NULL) continue;
		buffer_send[r] = tabla->enemigos[g]->tipo;
		buffer_send[r + 1] = tabla->enemigos[g]->x;
		buffer_send[r + 2] = tabla->enemigos[g]->y;
		
		r = r + 3;
	}
	
	/* Enviar el mensaje de start */
	for (g = 0; g < 3; g++) {
		if (tabla->fds[g] != -1) {
			agregar_write (tabla->fds[g], buffer_send, r, 0);
		}
	}
	
	tabla->running = TRUE;
	tabla->estado = WAITING_CLIENTS;
	tabla->clientes = ninjas;
	tabla->ready[0] = tabla->ready[1] = tabla->ready[2] = 0;
}

void send_ask_actions (SnowFight *tabla) {
	int g;
	char buffer_send[128];
	
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
	
	/* Los ninjas muertos no tienen derecho a realizar acciones */
	for (g = 0; g < 3; g++) {
		if (tabla->fds[g] != -1) {
			if (tabla->ninjas[g]->vida == 0) {
				tabla->ready[g] = 1;
			}
		}
	}
	
	/* Instalar un timer para forzar a los clientes a contestar en 10 segundos */
	agregar_timer (tabla, (Callback) calculate_actions, 11, NULL);
	//printf ("La tabla «%i» instaló un timer para ser llamado en 11 segundos\n", tabla->id);
}

void calculate_actions (SnowFight *tabla) {
	char buffer_send[128];
	int pos, save_pos;
	int g, s, h;
	int i, j;
	int gameover = 0;
	int enemigos_muertos;
	
	printf ("Calculando tabla «%i»\n", tabla->id);
	/* Ejecutar todas las acciones */
	buffer_send[0] = 'S';
	buffer_send[1] = 'N';

	/* Poner el campo de la versión */
	buffer_send[2] = 0;

	/* El campo de tipo */
	buffer_send[3] = NET_TYPE_SERVER_ACTIONS;
	
	pos = 5;
	s = 0;
	
	/* Primero los movimientos de los ninjas */
	for (g = 0; g < 3; g++) {
		if (tabla->ninjas[g] == NULL) continue;
		
		if (tabla->ninjas[g]->next_x != tabla->ninjas[g]->x || tabla->ninjas[g]->next_y != tabla->ninjas[g]->y) {
			s++;
			buffer_send[pos] = NINJA_FIRE + g;
			buffer_send[pos + 1] = tabla->ninjas[g]->next_x;
			buffer_send[pos + 2] = tabla->ninjas[g]->next_y;
			
			tabla->escenario[tabla->ninjas[g]->y][tabla->ninjas[g]->x] = NONE;
			pos = pos + 3;
			/* Mover realmente al ninja */
			tabla->ninjas[g]->x = tabla->ninjas[g]->next_x;
			tabla->ninjas[g]->y = tabla->ninjas[g]->next_y;
			
			tabla->escenario[tabla->ninjas[g]->y][tabla->ninjas[g]->x] = NINJA_FIRE + g;
		}
	}
	
	buffer_send[4] = s;
	
	save_pos = pos++;
	s = 0;
	
	/* Enviar los ataques */
	for (g = 0; g < 3; g++) {
		if (tabla->ninjas[g] == NULL) continue;
		
		/* Este ninja tiene un ataque por hacer */
		if (tabla->ninjas[g]->attack_x != -1 && tabla->ninjas[g]->attack_y != -1) {
			i = tabla->ninjas[g]->attack_x;
			j = tabla->ninjas[g]->attack_y;
			h = tabla->escenario[j][i] - ENEMY_1;
			
			s++;
			buffer_send[pos] = NINJA_FIRE + g;
			buffer_send[pos + 1] = tabla->escenario[j][i];
			
			pos = pos + 2;
			
			/* Ejecutar el daño */
			if (g == 0) { /* Ninja de fuego */
				tabla->enemigos[h]->vida -= 8;
			} else if (g == 1) { /* Ninja de nieve */
				tabla->enemigos[h]->vida -= 6;
			} else if (g == 2) { /* Ninja de agua */
				tabla->enemigos[h]->vida -= 10;
			}
			
			tabla->ninjas[g]->attack_x = tabla->ninjas[g]->attack_y = -1;
		}
	}
	
	buffer_send[save_pos] = s;
	
	/* Revisar que enemigos murieron */
	for (g = 0; g < 4; g++) {
		if (tabla->enemigos[g] != NULL) {
			/* Revisar si el enemigo ya está muerto */
			if (tabla->enemigos[g]->vida <= 0) {
				//printf ("«%i» Un enemigo se murió\n", tabla->id);
				i = tabla->enemigos[g]->x;
				j = tabla->enemigos[g]->y;
				tabla->escenario[j][i] = NONE;
				free (tabla->enemigos[g]);
				tabla->enemigos[g] = NULL;
			}
		}
	}
	
	/* Revisar si todos los enemigos mueriron, en ese caso, avanzar el round y mandar los siguiente enemigos */
	/* Significado de los bytes:
	 * 0 = Continua el round
	 * 2 = Vas hacia el round 2
	 * 3 = Vas hacia el round 3
	 * 4 = Vas hacia el bonus round
	 * 64 = Fin del juego (Ganaste)
	 * 65 = Fin del juego (Todos murieron)
	 */
	
	enemigos_muertos = 0;
	for (g = 0; g < 4; g++) {
		if (tabla->enemigos[g] != NULL) {
			enemigos_muertos++;
		}
	}
	
	if (enemigos_muertos != 0) {
		/* Vaciar movimientos y ataques previos */
		for (g = 0; g < 4; g++) {
			if (tabla->enemigos[g] == NULL) continue;
			tabla->enemigos[g]->old_x = tabla->enemigos[g]->x;
			tabla->enemigos[g]->old_y = tabla->enemigos[g]->y;
			tabla->enemigos[g]->attack_x = tabla->enemigos[g]->attack_y = -1;
		}
		
		/* Ejecutar el cálculo de la IA */
		calcular_ia (tabla->escenario, tabla->ninjas, tabla->enemigos);
		
		save_pos = pos++;
		s = 0;
		/* Por cada movimiento, mandar los bytes correspondientes */
		for (g = 0; g < 4; g++) {
			if (tabla->enemigos[g] == NULL) continue;
			if ((tabla->enemigos[g]->old_x != tabla->enemigos[g]->x ||
			    tabla->enemigos[g]->old_y != tabla->enemigos[g]->y) ||
			    (tabla->enemigos[g]->attack_x != -1 && tabla->enemigos[g]->attack_y != -1)) {
				
				/* El enemigo se movió o va a atacar */
				s++;
				
				/* Este es el movimiento */
				buffer_send[pos] = ENEMY_1 + g;
				buffer_send[pos + 1] = tabla->enemigos[g]->x;
				buffer_send[pos + 2] = tabla->enemigos[g]->y;
				
				/* Enviar el ataque si es que existe */
				if (tabla->enemigos[g]->attack_x != -1 && tabla->enemigos[g]->attack_y != -1) {
					i = tabla->enemigos[g]->attack_x;
					j = tabla->enemigos[g]->attack_y;
					
					buffer_send[pos + 3] = tabla->escenario[j][i];
				} else {
					buffer_send[pos + 3] = NONE;
				}
				
				pos = pos + 4;
			}
		}
		
		buffer_send[save_pos] = s;
	} else {
		/* No hay enemigos, mandar 0 en sus movimientos y ataques */
		buffer_send[pos++] = 0;
	}
	
	save_pos = pos++;
	
	if (enemigos_muertos == 0) {
		/* Generar más enemigos */
		tabla->round++;
		if ((0 && tabla->round == 4) || (tabla->round < 4)) {
			/* Si cumple la condición del bonus round, meter al bonus round */
			generar_enemigos (tabla, (tabla->round == 4) ? 1 : 0);
			buffer_send[save_pos] = tabla->round;
			
			buffer_send[pos] = tabla->enemys;
			pos++;
			
			/* Mandar las coordenadas de los nuevos enemigos */
			for (g = 0; g < tabla->enemys; g++) {
				buffer_send[pos] = tabla->enemigos[g]->tipo;
				buffer_send[pos + 1] = tabla->enemigos[g]->x;
				buffer_send[pos + 2] = tabla->enemigos[g]->y;
				
				pos = pos + 3;
			}
		} else if (tabla->round >= 4) {
			/* Fin del juego */
			buffer_send[save_pos] = 64;
			gameover = 1;
		}
	} else {
		/* Revisar si todos los ninjas murieron, para finalizar la partida */
		i = 0;
		for (g = 0; g < 3; g++) {
			if (tabla->ninjas[g] != NULL) {
				if (tabla->ninjas[g]->vida > 0) {
					i++;
				}
			}
		}
		if (i == 0) {
			/* Todos murieron */
			buffer_send[save_pos] = 65;
			gameover = 1;
		} else {
			buffer_send[save_pos] = 0;
		}
	}
	
	/* Enviar todo */
	for (g = 0; g < 3; g++) {
		if (tabla->fds[g] != -1) {
			agregar_write (tabla->fds[g], buffer_send, pos, 0);
		}
	}
	
	if (gameover) {
		/* Cerrar la mesa, pero no los clientes */
		remove_table (tabla);
		return;
	}
	tabla->estado = WAITING_CLIENTS;
	tabla->ready[0] = tabla->ready[1] = tabla->ready[2] = 0;
}

void manage_client_ready (SnowFight *tabla, int fd) {
	int g;
	if (!tabla->running || tabla->estado != WAITING_CLIENTS) {
		printf ("Error en la tabla. El cliente [%i] envió un ready cuando la tabla no está lista\n", fd);
		return;
	}
	
	for (g = 0; g < 3; g++) {
		if (tabla->fds[g] == fd) break;
	}
	
	/* Poner ready a ese cliente */
	tabla->ready[g] = 1;
	
	g = tabla->ready[0] + tabla->ready[1] + tabla->ready[2];
	
	if (g >= tabla->clientes) {
		send_ask_actions (tabla);
	}
}

void manage_client_done_actions (SnowFight *tabla, int fd) {
	int g, slot;
	char buffer_send[128];
	
	if (!tabla->running || tabla->estado != WAITING_ACTIONS) {
		printf ("Error en la tabla. El cliente [%i] envió un done actions cuando la tabla no está lista\n", fd);
		return;
	}
	
	for (g = 0; g < 3; g++) {
		if (tabla->fds[g] == fd) {
			slot = g;
			break;
		}
	}
	
	/* Poner ready a ese cliente */
	tabla->ready[slot] = 1;
	
	/* Rellenar con la firma del protocolo SN */
	buffer_send[0] = 'S';
	buffer_send[1] = 'N';

	/* Poner el campo de la versión */
	buffer_send[2] = 0;

	/* El campo de tipo */
	buffer_send[3] = NET_TYPE_PLAYER_DONE_ACTIONS;

	/* El ninja */
	buffer_send[4] = NINJA_FIRE + slot;
	
	/* Enviar el mensaje a los otros clientes */
	for (g = 0; g < 3; g++) {
		if (tabla->fds[g] == -1 || g == slot) continue;
		agregar_write (tabla->fds[g], buffer_send, 5, 0);
	}
	
	g = tabla->ready[0] + tabla->ready[1] + tabla->ready[2];
	
	if (g >= tabla->clientes) {
		/* Cancelar el timer */
		printf ("Tabla «%i» Done actions, ejecutando calculate\n", tabla->id);
		cancel_timer (tabla);
		calculate_actions (tabla);
	}
}

void manage_client_actions (SnowFight *tabla, int fd, NetworkMessage *msg) {
	int slot, g;
	int acciones[5][9];
	char buffer_send[128];
	ServerNinja *ninja;
	
	if (!tabla->running || tabla->estado != WAITING_ACTIONS) {
		printf ("Warning. El cliente [%i] envió una acción cuando la tabla no está recibiendo acciones\n", fd);
		return;
	}
	
	for (g = 0; g < 3; g++) {
		if (tabla->fds[g] == fd) {
			slot = g;
			break;
		}
	}
	
	if (tabla->ready[slot] == 1) {
		/* El cliente ya está listo, ignorar su acción */
		printf ("Warning. El cliente [%i] envió una acción cuando ya estaba listo, ignorando\n", fd);
		return;
	}
	
	/* Ya localizado el cliente, validar que el movimiento sea del ninja al que pertenece */
	if (msg->action.object != NINJA_FIRE + slot) {
		printf ("Error en la tabla «%i». El cliente [%i] trata de mover un ninja que no es él\n", tabla->id, fd);
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
	
	/* Validar la acción */
	if (msg->action.type == ACTION_MOVE) {
		if (msg->action.x < 0 || msg->action.x > 8 || msg->action.y < 0 || msg->action.y > 4) {
			printf ("Error en la tabla «%i». El cliente [%i] envió coordenadas inválidas para un movimiento\n", tabla->id, fd);
			return;
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
	} else if (msg->action.type == ACTION_ATTACK) {
		if (msg->action.x < 0 || msg->action.x > 8 || msg->action.y < 0 || msg->action.y > 4) {
			printf ("Error en la tabla «%i». El cliente [%i] envió coordenadas inválidas para un movimiento\n", tabla->id, fd);
			return;
		}
		
		if (acciones[msg->action.y][msg->action.x] & ACTION_ATTACK) {
			attack_next (ninja, msg->action.x, msg->action.y);
			
			/* Enviar el movimiento de regreso al cliente */
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
			
			agregar_write (tabla->fds[slot], buffer_send, 8, 0);
		}
	}
}

void remove_table (SnowFight *tabla) {
	SnowFight *prev;
	int g;
	
	/* Localizar la tabla en la lista ligada y eliminarla */
	if (lista_partidas == tabla) {
		/* Primera de la lista */
		lista_partidas = lista_partidas->next;
	} else {
		prev = lista_partidas;
		
		while (prev->next != tabla) {
			prev = prev->next;
		}
		
		prev->next = tabla->next;
	}
	
	/* Antes de hacerle free, revisar si dejó objetos creados para eliminarlos */
	for (g = 0; g < 3; g++) {
		if (tabla->ninjas[g] != NULL) free (tabla->ninjas[g]);
	}
	
	for (g = 0; g < 4; g++) {
		if (tabla->enemigos[g] != NULL) free (tabla->enemigos[g]);
	}
	
	free (tabla);
}

void leave_table_by_close (SnowFight *tabla, int fd) {
	int slot;
	unsigned char buffer_send[128];
	int g, h;
	ServerNinja *ninja;
	int elemento;
	
	if (tabla == NULL) return;
	
	/* Localizar el jugador que se cerró */
	for (g = 0; g < 3; g++) {
		if (tabla->fds[g] == fd) {
			slot = g;
			break;
		}
	}
	
	//printf ("Un cliente [%i] abandonó la tabla «%i».\n", fd, tabla->id);
	if (tabla->running == FALSE) {
		/* Como la tabla aún no está completa, puedo eliminar un jugador */
		/* Rellenar con la firma del protocolo SN */
		buffer_send[0] = 'S';
		buffer_send[1] = 'N';
	
		/* Poner el campo de la versión */
		buffer_send[2] = 0;
	
		/* El campo de tipo */
		buffer_send[3] = NET_TYPE_REMOVE_PLAYER;
	
		/* El ninja */
		buffer_send[4] = NINJA_FIRE + slot;
		
		/* Enviar el remove player a los otros jugadores */
		for (g = 0; g < 3; g++) {
			if (g == slot) continue;
			agregar_write (tabla->fds[g], buffer_send, 5, 0);
		}
		
		tabla->fds[slot] = -1;
		
		if (tabla->fds[0] == -1 && tabla->fds[1] == -1 && tabla->fds[2] == -1) {
			//printf ("Eliminado tabla «%i» por falta de jugadores\n", tabla->id);
			remove_table (tabla);
		}
	} else {
		/* Un ninja se perdió en media partida, mandar el mensaje de eliminar */
		if (tabla->clientes == 1) {
			/* Último cliente, nada que mandar */
			//printf ("Eliminando tabla «%i»\n", tabla->id);
			cancel_timer (tabla);
			remove_table (tabla);
			return;
		}
		
		/* Rellenar con la firma del protocolo SN */
		buffer_send[0] = 'S';
		buffer_send[1] = 'N';
	
		/* Poner el campo de la versión */
		buffer_send[2] = 0;
	
		/* El campo de tipo */
		buffer_send[3] = NET_TYPE_REMOVE_PLAYER;
	
		/* El ninja */
		buffer_send[4] = NINJA_FIRE + slot;
		
		/* Enviar el remove player a los otros jugadores */
		for (g = 0; g < 3; g++) {
			if (g == slot) continue;
			agregar_write (tabla->fds[g], buffer_send, 5, 0);
		}
		
		tabla->fds[slot] = -1;
		tabla->clientes--;
		
		/* Quitar el jugador del mapa */
		elemento = NINJA_FIRE + slot;
		
		if (elemento == NINJA_FIRE) {
			ninja = tabla->fire;
			tabla->fire = NULL;
		} else if (elemento == NINJA_SNOW) {
			ninja = tabla->snow;
			tabla->snow = NULL;
		} else if (elemento == NINJA_WATER) {
			ninja = tabla->water;
			tabla->water = NULL;
		}
		
		tabla->escenario[ninja->y][ninja->x] = NONE;
		
		free (ninja);
		
		if (tabla->estado == WAITING_CLIENTS) {
			/* Si estabamos esperando por un cliente, revisar si después de su salida ya podemos continuar */
			g = tabla->ready[0] + tabla->ready[1] + tabla->ready[2];
		
			if (g >= tabla->clientes) {
				send_ask_actions (tabla);
			}
		} else if (tabla->estado == WAITING_ACTIONS) {
			/* Si estabamos esperando por un cliente y se cerró, ya podemos continuar sin él */
			g = tabla->ready[0] + tabla->ready[1] + tabla->ready[2];
		
			if (g >= tabla->clientes) {
				printf ("«%i» Done actions\n", tabla->id);
				cancel_timer (tabla);
				calculate_actions (tabla);
			}
		}
	}
}

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
		
		if (res == 0) {
			goto continue_mainloop; /* Nada que vigilar */
		}
		
		if (vigilar[0].revents & POLLIN) {
			/* Evento en el servidor */
			cliente_size = sizeof (cliente);
			nuevo = accept (server_fd, (struct sockaddr *) &cliente, &cliente_size);
			
			getnameinfo ((struct sockaddr *) &cliente, cliente_size, buffer_ip, sizeof (buffer_ip), buffer_port, sizeof (buffer_port), NI_NUMERICHOST | NI_NUMERICSERV);
			
			//printf ("Nueva conexión entrante desde la IP: %s:%s => [%i]\n", buffer_ip, buffer_port, nuevo);
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
				tabla = find_table_by_fd (fd);
				
				if (tabla != NULL) {
					leave_table_by_close (tabla, fd);
				}
				eliminar_a_fd (vigilar[g].fd);
			} else if ((vigilar[g].revents & POLLIN) || (vigilar[g].revents & POLLHUP)) {
				fd = vigilar[g].fd;
				
				res = read (fd, buffer_data, sizeof (buffer_data));
				
				if (res == 0) {
					/* Cierre de conexión */
					eliminar_a_fd (fd);
					printf ("[%i] Cierre normal\n", vigilar[g].fd);
					/* Buscar si está sentado en una mesa */
					tabla = find_table_by_fd (fd);
					
					if (tabla != NULL) {
						leave_table_by_close (tabla, fd);
					}
					continue;
				} else if (res < 0) {
					perror ("Error en read");
					eliminar_a_fd (fd);
					
					tabla = find_table_by_fd (fd);
					if (tabla != NULL) {
						leave_table_by_close (tabla, fd);
					}
					
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
				} else if (message.type == NET_TYPE_DONE_ACTIONS) {
					/* Debe estar sentado en una tabla */
					if (tabla == NULL) {
						printf ("[%i] Error, este cliente envia un done actions cuando no está sentado en una tabla\n", fd);
					} else {
						manage_client_done_actions (tabla, fd);
					}
				} else {
					printf ("[%i] Mensaje desconocido para el servidor\n", fd);
				}
				
				
			}
		}
		
		continue_mainloop:
		eliminar_poll ();
		
		agregar_poll ();
		
		check_timers ();
	} while (1);
	
	/* Ejecutar cierres y limpieza aquí */
	
	return 0;
}
