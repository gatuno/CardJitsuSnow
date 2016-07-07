/*
 * server_ninja.h
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

#ifndef __SERVER_NINJA_H__
#define __SERVER_NINJA_H__

typedef struct {
	int x, y;
	int tipo;
	int next_x, next_y;
} ServerNinja;

ServerNinja *create_server_ninja (int x, int y, int tipo);

void ask_fire_actions (ServerNinja *ninja, int escenario[5][9], int acciones[5][9]);
void ask_water_actions (ServerNinja *ninja, int escenario[5][9], int acciones[5][9]);
void ask_snow_actions (ServerNinja *ninja, int escenario[5][9], int acciones[5][9]);

void move_next (ServerNinja *ninja, int x, int y);

typedef struct {
	int x, y;
	int tipo;
} ServerEnemy;

ServerEnemy *create_server_enemy (int x, int y, int tipo);

#endif /* __SERVER_NINJA_H__ */

