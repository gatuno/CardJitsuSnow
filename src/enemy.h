/*
 * enemy.h
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

#ifndef __ENEMY_H__
#define __ENEMY_H__

typedef struct {
	int frame;
	int x, y;
	int tipo;
	int estado;
	int x_real, y_real;
} Enemy;

Enemy *create_enemy (int x, int y, int tipo);
void draw_enemy (Enemy *enemy);
void setup_enemy (void);
int is_enemy_ready (Enemy *enemy);

#endif /* __ENEMY_H__ */

