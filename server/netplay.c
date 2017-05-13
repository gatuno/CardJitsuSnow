/*
 * netplay.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <unistd.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>

#include "netplay.h"
#include "snow.h"

#define SERV_PORT 3301

typedef struct PendingWrite {
	int fd;
	char buffer[128];
	int size;
	
	int close;
	
	struct PendingWrite *next;
} PendingWrite;

/* Globales manejador de los sockets */
struct pollfd vigilar[MAX_FDS];
int fds;
int agregar_fd[MAX_FDS];
int agregar_count = 0;
int eliminar_fd[MAX_FDS];
int eliminar_count = 0;
static PendingWrite *writes;

int inicializar_socket (void) {
	int server, res;
	struct sockaddr_in6 serv_bind;
	
	writes = NULL;
	
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

/* Manejo de sockets */
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
		//printf ("Se envia una escritura pendiente para el fd [%i].\n", fd);
		/* Ejecutar el write */
		write (pos->fd, pos->buffer, pos->size);
		
		if (pos->close) {
			//printf ("Y después se solicitó cerrar\n");
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
			(*prev) = pos->next;
			n = pos->next;
			
			free (pos);
			pos = n;
			//printf ("Se canceló una escritura pendiente para el fd [%i]\n", fd);
		} else {
			prev = &(pos->next);
			pos = pos->next;
		}
	}
}

int unpack (NetworkMessage *msg, unsigned char *buffer, int len) {
	int g, h, e, pos;
	int real_len;
	
	/* Vaciar la estructura */
	memset (msg, 0, sizeof (NetworkMessage));
	
	if (len < 4) return -1;
	
	if (buffer[0] != 'S' || buffer[1] != 'N') {
		printf ("Protocol Mismatch!\n");
		
		return -1;
	}
	
	msg->version = buffer[2];
	if (msg->version != 0) {
		printf ("Version mismatch. Expecting 0\n");
		
		return -1;
	}
	
	msg->type = buffer[3];
	
	if (msg->type == NET_TYPE_JOIN) {
		real_len = (6 + NICK_SIZE);
		if (len < real_len) return -1; /* Oops, tamaño incorrecto */
		
		msg->element = buffer[4];
		memcpy (msg->nick, &buffer[6], NICK_SIZE);
	} else if (msg->type == NET_TYPE_CLIENT_READY) {
		real_len = 4;
	} else if (msg->type == NET_TYPE_ACTION) {
		if (len < 8) return -1;
		
		msg->action.object = buffer[4];
		msg->action.type = buffer[5];
		msg->action.x = buffer[6];
		msg->action.y = buffer[7];
		
		real_len = 8;
	} else if (msg->type == NET_TYPE_PLAYER_DONE_ACTIONS) {
		if (len < 5) return -1;
		
		msg->element = buffer[4];
		if (msg->element < NINJA_FIRE || msg->element > NINJA_WATER) {
			printf ("Elemento inválido\n");
			return -1;
		}
		real_len = 5;
	} else if (msg->type == NET_TYPE_DONE_ACTIONS) {
		real_len = 4;
	} else {
		printf ("Mensaje desconocido para el servidor\n");
		return -1;
	}
	
	return real_len;
}

