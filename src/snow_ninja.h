/*
 * snow_ninja.h
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

#ifndef __SNOW_NINJA_H__
#define __SNOW_NINJA_H__

#include "enemy.h"

typedef struct _SnowNinja SnowNinja;

void setup_snow_ninja (void);
void draw_snow_ninja (SnowNinja *);
void draw_ghost_snow_ninja (SnowNinja *ninja);
SnowNinja *create_snow_ninja (int x, int y);
void heal_snow (SnowNinja *ninja);
void celebrate_snow (SnowNinja *ninja);
void move_snow (SnowNinja *ninja, int x, int y);
void ghost_move_snow (SnowNinja *ninja, int x, int y);
void ko_snow (SnowNinja *ninja);
void hit_snow (SnowNinja *ninja);
void revive_snow (SnowNinja *ninja);

void ask_snow_actions (SnowNinja *ninja, int escenario[5][9], int acciones[5][9]);

void put_idle_snow (SnowNinja *ninja);
void ask_snow_coords (SnowNinja *ninja, int *x, int *y);
int is_snow_done (SnowNinja *ninja);
void attack_snow (SnowNinja *ninja, Enemy *enemy);

#endif /* __SNOW_NINJA_H__ */

