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

void calcular_ia (int escenario[5][9], ServerNinja *ninjas[3], ServerEnemy *enemy[4]) {
	int g, s, h, i;
	int max_h, min_h, max_i, min_i;
	int done;
	int dir;
	
	/* Generar un movimiento aleatorio para todos los enemigos */
	for (g = 0; g < 4; g++) {
		if (enemy[g] == NULL) return;
		done = 0;
		
		#if 0
		/* Ejemplo de como causar un movimiento */
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
		#endif
		
		
		/* Si el enemigo hizo un movimiento, moverlo */
		if (enemy[g]->old_x != enemy[g]->x || enemy[g]->old_y != enemy[g]->y) {
			/* Mover al enemigo */
			s = escenario[enemy[g]->old_y][enemy[g]->old_x];
			escenario[enemy[g]->old_y][enemy[g]->old_x] = NONE;
			escenario[enemy[g]->y][enemy[g]->x] = s;
		}
		
		/* Tratar de atacar a alguien que esté a la izquierda */
		ServerNinja *n;
		
		if (enemy[g]->x > 0) {
			s = escenario[enemy[g]->y][enemy[g]->x - 1];
			if (s >= NINJA_FIRE && s <= NINJA_WATER) { /* Es un objeto ninja */
				n = ninjas[s - NINJA_FIRE];
				
				if (n->vida > 0) {
					/* Pegarle, porque aún tiene vida */
					enemy[g]->attack_x = enemy[g]->x - 1;
					enemy[g]->attack_y = enemy[g]->y;
				}
			}
		}
		
		
		/* Si este enemigo ejecutó un ataque bajar vida */
		if (enemy[g]->attack_x != -1 && enemy[g]->attack_y != -1) {
			
			if (enemy[g]->tipo == ENEMY_SLY) {
				/* Calcular la distancia absoluta para saber el daño */
				h = (enemy[g]->x - enemy[g]->attack_x);
				if (h < 0) h = -h;
				
				i = (enemy[g]->y - enemy[g]->attack_y);
				if (i < 0) i = -i;
				
				s = escenario[enemy[g]->attack_y][enemy[g]->attack_x];
				n = ninjas[s - NINJA_FIRE];
				
				n->vida = n->vida - (h + i + 2);
				if (n->vida < 0) n->vida = 0;
			} else if (enemy[g]->tipo == ENEMY_SCRAP) {
				/* Ejecutar daño en esa posición y todas alrededor */
				s = escenario[enemy[g]->attack_y][enemy[g]->attack_x];
				n = ninjas[s - NINJA_FIRE];
				n->vida = n->vida - 4;
				if (n->vida < 0) n->vida = 0;
				
				min_h = (enemy[g]->attack_y - 1 < 0) ? enemy[g]->attack_y : enemy[g]->attack_y - 1;
				max_h = (enemy[g]->attack_y + 1 > 4) ? enemy[g]->attack_y : enemy[g]->attack_y + 1;
				
				min_i = (enemy[g]->attack_x - 1 < 0) ? enemy[g]->attack_x : enemy[g]->attack_x - 1;
				max_i = (enemy[g]->attack_x + 1 > 8) ? enemy[g]->attack_x : enemy[g]->attack_x + 1;
				
				for (h = min_h; h <= max_h; h++) {
					for (i = min_i; i <= min_i; i++) {
						s = escenario[h][i];
						if (s >= NINJA_FIRE && s <= NINJA_WATER) { /* Ninja que recibe daño colateral */
							n = ninjas[s - NINJA_FIRE];
							
							if (n->vida > 0) {
								n->vida = n->vida - 4;
								if (n->vida < 0) n->vida = 0;
							}
						}
					}
				} /* For ninjas daño colateral */
			} else if (enemy[g]->tipo == ENEMY_TANK) {
				/* Ejecutar daño a ese ninja y a los que estén al lado */
				if (enemy[g]->attack_x == enemy[g]->x) { /* Golpe hacia abajo o hacia arriba */
					min_i = (enemy[g]->attack_x - 1 < 0) ? enemy[g]->attack_x : enemy[g]->attack_x - 1;
					max_i = (enemy[g]->attack_x + 1 > 8) ? enemy[g]->attack_x : enemy[g]->attack_x + 1;
					min_h = max_h = enemy[g]->attack_y;
				} else if (enemy[g]->attack_y == enemy[g]->y) { /* Golpe hacia la izquierda o derecha */
					min_h = (enemy[g]->attack_y - 1 < 0) ? enemy[g]->attack_y : enemy[g]->attack_y - 1;
					max_h = (enemy[g]->attack_y + 1 > 4) ? enemy[g]->attack_y : enemy[g]->attack_y + 1;
					min_i = max_i = enemy[g]->attack_x;
				}
				
				for (h = min_h; h <= max_h; h++) {
					for (i = min_i; i <= min_i; i++) {
						s = escenario[h][i];
						if (s >= NINJA_FIRE && s <= NINJA_WATER) { /* Ninja que recibe daño colateral */
							n = ninjas[s - NINJA_FIRE];
							
							if (n->vida > 0) {
								n->vida = n->vida - 10;
								if (n->vida < 0) n->vida = 0;
							}
						}
					}
				} /* For ninjas daño colateral */
				
			} /* Enemigo tipo == TANK */
			
		} /* Fin del if si el enemigo ataca */
	} /* Fin del ciclo recorrer enemigos */
}

