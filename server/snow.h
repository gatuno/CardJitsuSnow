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

#include <stdlib.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE !FALSE
#endif

/* Acciones */
#define ACTION_CLEAR 0x00
#define ACTION_MOVE 0x01
#define ACTION_CANT_MOVE 0x02

#define ACTION_ATTACK 0x04
#define ACTION_HEAL 0x08

#define RANDOM(x) ((int) (x ## .0 * rand () / (RAND_MAX + 1.0)))

/* Objetos */
enum {
	NONE = 0,
	
	NINJA_FIRE,
	NINJA_SNOW,
	NINJA_WATER,
	
	NINJA, /* Generico */
	
	ROCK,
	
	ENEMY_SLY,
	ENEMY_SCRAP,
	ENEMY_TANK,
	
	ENEMY_1,
	ENEMY_2,
	ENEMY_3,
	ENEMY_4,
	
	NUM_OBJECTS
};

#endif /* __SNOW_H__ */

