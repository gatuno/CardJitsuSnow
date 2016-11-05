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

typedef struct _Enemy Enemy;

typedef struct {
	int x, y;
	int delay;
	int damage;
} EnemyHitZone;

Enemy *create_enemy (int x, int y, int tipo);
void enemy_ask_coords (Enemy *enemy, int *x, int *y);
void draw_enemy (Enemy *enemy);
void setup_enemy (void);
int is_enemy_ready (Enemy *enemy);
int is_enemy_dead (Enemy *enemy);
int is_enemy_done (Enemy *enemy);
void enemy_attack (Enemy *enemy);
void add_enemy_ref (Enemy *enemy);
void enemy_hit_delayed (Enemy *enemy, int damage, int delay);
int enemy_get_hit_zone (Enemy *enemy, EnemyHitZone *hitzone, int x, int y);
void enemy_move (Enemy *enemy, int x, int y);

#endif /* __ENEMY_H__ */

