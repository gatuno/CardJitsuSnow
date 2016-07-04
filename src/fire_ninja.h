/*
 * fire_ninja.h
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

#ifndef __FIRE_NINJA_H__
#define __FIRE_NINJA_H__

typedef struct _FireNinja FireNinja;

void setup_fire_ninja (void);
void draw_fire_ninja (FireNinja *);
void draw_ghost_fire_ninja (FireNinja *ninja);
FireNinja *create_fire_ninja (int x, int y);
void attack_fire (FireNinja *ninja);
void celebrate_fire (FireNinja *ninja);
void move_fire (FireNinja *ninja, int x, int y);
void ko_fire (FireNinja *ninja);
void hit_fire (FireNinja *ninja);
void revive_fire (FireNinja *ninja);

void ask_fire_actions (FireNinja *ninja, int escenario[5][9], int acciones[5][9]);

void prev_move_fire (FireNinja *ninja);
void put_idle_fire (FireNinja *Ninja);
#endif /* __FIRE_NINJA_H__ */

