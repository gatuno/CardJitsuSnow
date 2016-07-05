/*
 * netplay.h
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

#ifndef __NETPLAY_H__
#define __NETPLAY_H__

#include <stdint.h>

#define NICK_SIZE 16

#define GPOINTER_TO_INT(p) ((int)  (long) (p))
#define GINT_TO_POINTER(i) ((void *) (long) (i))

typedef struct {
	int object, x, y;
} ObjectPos;

typedef struct {
	int num_objects;
	int background;
	int bonus_type;
} StartInfo;

typedef struct {
	uint8_t version;
	uint8_t type;
	union {
		struct {
			int others;
			int others_color[2];
			char others_nicks[2][NICK_SIZE];
		};
		struct {
			int element;
			char nick[NICK_SIZE];
		};
		struct {
			StartInfo info;
			ObjectPos *objects;
		};
	};
} NetworkMessage;

enum {
	NET_TYPE_JOIN = 1,
	NET_TYPE_ACCEPT,
	NET_TYPE_OTHER_JOIN,
	
	NET_TYPE_START_INFO = 8,
	NET_TYPE_READY
};

enum {
	NETWORK_EVENT_CLOSE = 0,
	NETWORK_EVENT_ACCEPT,
	NETWORK_EVENT_JOIN_NINJA,
	
	NETWORK_EVENT_START
};

typedef struct {
	int elemento;
	char nick[NICK_SIZE];
} NinjaInfo;

int setup_netplay (char *server, int puerto);
void send_join (int ninja, char *nick);
void send_ready (void);
void process_network_events (void);

extern int NETWORK_EVENT;

#endif /* __NETPLAY_H__ */
