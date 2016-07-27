/*
 * ninja.h
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

#ifndef __NINJA_H__
#define __NINJA_H__

typedef struct _Ninja Ninja;

void setup_ninja (void);
Ninja *create_ninja (int tipo, int x, int y);
void ninja_ask_coords (Ninja *ninja, int *x, int *y);
int ninja_is_done (Ninja *ninja);
void ninja_attack (Ninja *ninja);
void ninja_move (Ninja *ninja, int x, int y);
void ninja_move_ghost (Ninja *ninja, int x, int y);
void ninja_draw (Ninja *ninja);
void ninja_draw_ghost (Ninja *ninja);
void ninja_ask_actions (Ninja *ninja, int escenario[5][9], int acciones[5][9]);
int ninja_get_hit_delay (Ninja *ninja);
int ninja_get_attack (Ninja *ninja);

#endif /* __NINJA_H__ */

