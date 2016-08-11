/*
 * select_server.h
 * This file is part of Card-Jitsu Snow
 *
 * Copyright (C) 2016 - Félix Arreola Rodríguez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SELECT_SERVER_H__
#define __SELECT_SERVER_H__

#include <SDL.h>

#if SDL_MAJOR_VERSION == 1
#define SDL_Renderer SDL_Surface
#endif

typedef struct {
	char name[60];
	char host[256];
	
	int population;
	int max_population;
} ServerInfo;

ServerInfo * select_server (SDL_Renderer *screen, ServerInfo *server_list, int servers, int recommended[5]);
void setup_select_server (SDL_Renderer *renderer);

#endif /* __SELECT_SERVER_H__ */

