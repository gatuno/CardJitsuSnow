/*
 * snow.h
 * This file is part of Card-Jitsu Snow
 *
 * Copyright (C) 2016 - Félix Arreola Rodríguez
 *
 * Card-Jitsu Snow is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Card-Jitsu Snow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Card-Jitsu Snow. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SNOW_H__
#define __SNOW_H__

#include <SDL.h>

typedef struct {
	int orig_x, orig_y;
	int w, h;
	int dest_x, dest_y;
	int rot;
} SnowSprite;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE !FALSE
#endif

#define MAP_X 90
#define MAP_Y 80

extern SDL_Renderer *renderer;

extern int use_sound;

enum {
	UI_FIRE = 0,
	UI_SNOW,
	UI_WATER
};

/* Acciones */
#define ACTION_CLEAR 0x00
#define ACTION_MOVE 0x01
#define ACTION_CANT_MOVE 0x02

#define ACTION_ATTACK 0x04
#define ACTION_HEAL 0x08

#define ACTION_FIRE_MOVE 0x10
#define ACTION_SNOW_MOVE 0x20
#define ACTION_WATER_MOVE 0x40

#endif /* __SNOW_H__ */

