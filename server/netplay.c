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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>

#include "netplay.h"
#include "snow.h"

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

