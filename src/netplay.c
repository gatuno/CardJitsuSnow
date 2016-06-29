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

#include <fcntl.h>

#include <SDL.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>
#include "gettext.h"
#define _(string) gettext (string)

#include "netplay.h"

static int server_fd;

/* Constante de los eventos */
int NETWORK_EVENT;

int setup_netplay (char *server, int puerto) {
	int n;
	struct addrinfo hints, *res, *ressave;
	char buffer_port[20];
	
	memset (&hints, 0, sizeof (hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	sprintf (buffer_port, "%i", puerto);
	
	n = getaddrinfo (server, buffer_port, &hints, &res);
	
	if (n < 0) {
		/* Falló la resolución de nombres */
		return -1;
	}
	
	ressave = res;
	char buffer_ip[256];
	char buffer_serv[256];
	
	server_fd = -1;
	
	while (res != NULL) {
		/*getnameinfo (res->ai_addr, res->ai_addrlen, buffer_ip, sizeof (buffer_ip), buffer_serv, sizeof (buffer_serv), NI_NUMERICHOST | NI_NUMERICSERV);
		
		if (res->ai_family == AF_INET) {
			printf ("Me intento conectar por IPv4 a %s:%s\n", buffer_ip, buffer_serv);
		} else if (res->ai_family == AF_INET6) {
			printf ("Me intento conectar por IPv6 a %s:%s\n", buffer_ip, buffer_serv);
		}*/
		server_fd = socket (res->ai_family, res->ai_socktype, 0);
		
		if (server_fd >= 0) {
			n = connect (server_fd, res->ai_addr, res->ai_addrlen);
			
			if (n == 0) {
				break; // Cierra el while de conexión
			}
			perror ("Falló la conexión");
			
			close (server_fd);
			server_fd = -1;
		}
		res = res->ai_next;
	}
	freeaddrinfo (ressave);
	if (server_fd < 0) {
		/* No se pudo hacer la conexión a cualquier */
		printf ("Ninguna conexión fué posible\n");
		return -1;
	}
	
	/* No utilizaré poll, sino llamadas no-bloqueantes */
	int flags;
	flags = fcntl (server_fd, F_GETFL);
	flags = flags | O_NONBLOCK;
	fcntl (server_fd, F_SETFL, flags);
	
	/* Registrar los eventos de la red */
	NETWORK_EVENT = SDL_RegisterEvents (1);
	
	if (NETWORK_EVENT == ((Uint32) -1)) {
		fprintf (stderr, _("Failed to register custom user events to SDL\n"
			"The error returned by SDL is:\n"
			"%s\n"), SDL_GetError());
		SDL_Quit ();
		exit (1);
	}
	
	return 0;
}

void send_join (int ninja, char *nick) {
	char buffer_send[128];
	uint16_t temp;
	
	/* Rellenar con la firma del protocolo FF */
	buffer_send[0] = 'S';
	buffer_send[1] = 'N';
	
	/* Poner el campo de la versión */
	buffer_send[2] = 0;
	
	/* El campo de tipo */
	buffer_send[3] = NET_TYPE_JOIN;
	
	buffer_send[4] = ninja;
	buffer_send[5] = 0; /* Sin uso */
	memcpy (&buffer_send[6], nick, NICK_SIZE);
	
	buffer_send[6 + NICK_SIZE - 1] = 0; /* Asegurar que tenga el caracter de terminación de cadena */
	
	send (server_fd, buffer_send, 6 + NICK_SIZE, 0);
}

int unpack (NetworkMessage *msg, unsigned char *buffer, int len) {
	int g, h, e;
	
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
		if (len < (6 + NICK_SIZE)) return -1; /* Oops, tamaño incorrecto */
		
		msg->element = buffer[4];
		memcpy (msg->nick, &buffer[6], NICK_SIZE);
	} else if (msg->type == NET_TYPE_ACCEPT) {
		if (len < 5) return -1; /* Oops, tamaño incorrecto */
		
		g = buffer[4];
		if (g > 2) return -1;
		if (len < 5 + (NICK_SIZE + 1) * g) return -1;
		
		msg->others = g;
		h = 5;
		for (g = 0; g < msg->others; g++) {
			e = buffer[h];
			
			if (e < 0 || e > 2) {
				printf ("Elemento inválido\n");
				return -1;
			}
			msg->others_color[g] = e;
			memcpy (msg->others_nicks[g], &buffer[h + 1], NICK_SIZE);
			msg->others_nicks[g][NICK_SIZE - 1] = 0;
			
			h += NICK_SIZE + 1;
		}
	} else if (msg->type == NET_TYPE_OTHER_JOIN) {
		if (len < (4 + NICK_SIZE)) return -1;
		
		msg->element = buffer[4];
		memcpy (msg->nick, &buffer[5], NICK_SIZE);
	}
	
	return 0;
}

void process_network_events (void) {
	int len, g;
	unsigned char buffer [256];
	SDL_Event evento;
	NetworkMessage msg;
	NinjaInfo *otros_info;
	
	do {
		len = recv (server_fd, buffer, 256, 0);
		
		if (len < 0 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
			break;
		}
		
		if (len == 0) {
			/* Conexión cerrada */
			SDL_zero (evento);
			evento.type = NETWORK_EVENT;
			evento.user.code = NETWORK_EVENT_CLOSE;
		
			SDL_PushEvent (&evento);
			break;
		}
		
		if (unpack (&msg, buffer, len) < 0) {
			printf ("Recibí un paquete mal estructurado\n");
			continue;
		}
		
		if (msg.type == NET_TYPE_ACCEPT) {
			/* Información del servidor y posibles otros ninjas */
			SDL_zero (evento);
			
			evento.type = NETWORK_EVENT;
			evento.user.code = NETWORK_EVENT_ACCEPT;
			
			evento.user.data1 = GINT_TO_POINTER (msg.others);
			evento.user.data2 = NULL;
			
			if (msg.others > 0) {
				otros_info = (NinjaInfo *) malloc (sizeof (NinjaInfo) * msg.others);
				
				for (g = 0; g < msg.others; g++) {
					otros_info[g].elemento = msg.others_color[g];
					memcpy (otros_info[g].nick, msg.others_nicks[g], NICK_SIZE);
				}
				
				evento.user.data2 = otros_info;
			}
			
			SDL_PushEvent (&evento);
		} else if (msg.type == NET_TYPE_OTHER_JOIN) {
			/* Se conectó un nuevo jugador */
			SDL_zero (evento);
			evento.type = NETWORK_EVENT;
			evento.user.code = NETWORK_EVENT_JOIN_NINJA;
			
			otros_info = (NinjaInfo *) malloc (sizeof (NinjaInfo));
			
			otros_info->elemento = msg.element;
			memcpy (otros_info->nick, msg.nick, NICK_SIZE);
			
			evento.user.data1 = otros_info;
			
			SDL_PushEvent (&evento);
		}
	} while (1);
}

