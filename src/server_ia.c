/*
 * server_ia.c
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
 * along with Card Jitsu Snow; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "snow.h"
#include "server_ninja.h"
#include "server_ia.h"

void calcular_ia (int escenario[5][9], ServerNinja *ninja[3], ServerEnemy *enemy[4]) {
	int g, s;
	int done;
	int dir;
	
	/* Generar un movimiento aleatorio para todos los enemigos */
	for (g = 0; g < 4; g++) {
		if (enemy[g] == NULL) return;
		done = 0;
		
		dir = RANDOM (4);
		if (dir == 0) {
			/* Izq */
			if (enemy[g]->x > 0 && escenario[enemy[g]->y][enemy[g]->x - 1] == NONE) {
				enemy[g]->x = enemy[g]->x - 1;
				done = 1;
			}
		} else if (dir == 1) {
			/* Der */
			if (enemy[g]->x < 8 && escenario[enemy[g]->y][enemy[g]->x + 1] == NONE) {
				enemy[g]->x = enemy[g]->x + 1;
				done = 1;
			}
		} else if (dir == 2) {
			/* Arriba */
			if (enemy[g]->y > 0 && escenario[enemy[g]->y - 1][enemy[g]->x] == NONE) {
				enemy[g]->y = enemy[g]->y - 1;
				done = 1;
			}
		} else if (dir == 3) {
			/* Abajo */
			if (enemy[g]->y < 4 && escenario[enemy[g]->y + 1][enemy[g]->x] == NONE) {
				enemy[g]->y = enemy[g]->y + 1;
				done = 1;
			}
		}
		
		if (done == 0) {
			g--; /* Repetir el ciclo, aún no hace un movimiento */
			continue;
		}
		
		/* Si el enemigo hizo un movimiento, moverlo */
		if (enemy[g]->old_x != enemy[g]->x || enemy[g]->old_y != enemy[g]->y) {
			/* Mover al enemigo */
			s = escenario[enemy[g]->old_y][enemy[g]->old_x];
			escenario[enemy[g]->old_y][enemy[g]->old_x] = NONE;
			escenario[enemy[g]->y][enemy[g]->x] = s;
		}
	}
}
