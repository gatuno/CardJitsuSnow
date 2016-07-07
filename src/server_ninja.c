/*
 * server_ninja.c
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "server_ninja.h"
#include "snow.h"

ServerNinja *create_server_ninja (int x, int y, int tipo) {
	ServerNinja *obj;
	
	obj = (ServerNinja *) malloc (sizeof (ServerNinja));
	
	obj->x = x;
	obj->y = y;
	
	obj->next_x = obj->x;
	obj->next_y = obj->y;
	
	obj->tipo = tipo;
	return obj;
}

void move_next (ServerNinja *ninja, int x, int y) {
	ninja->next_x = x;
	ninja->next_y = y;
}

void ask_fire_actions (ServerNinja *ninja, int escenario[5][9], int acciones[5][9]) {
	int g, h;
	int obj;
	
	for (g = -2; g <= 2; g++) {
		if (ninja->x + g >= 0 && ninja->x + g < 9) {
			obj = escenario[ninja->y][ninja->x + g];
			if (obj == ROCK || obj == NINJA_WATER || obj == NINJA_FIRE || obj == NINJA_SNOW) {
				acciones[ninja->y][ninja->x + g] = ACTION_CANT_MOVE;
			} else if (obj == NONE) {
				acciones[ninja->y][ninja->x + g] = ACTION_MOVE;
			}
		}
	}
	
	for (h = -2; h <= 2; h++) {
		if (ninja->y + h >= 0 && ninja->y + h < 5) {
			obj = escenario[ninja->y + h][ninja->x];
			if (obj == ROCK || obj == NINJA_WATER || obj == NINJA_FIRE || obj == NINJA_SNOW) {
				acciones[ninja->y + h][ninja->x] = ACTION_CANT_MOVE;
			} else if (obj == NONE) {
				acciones[ninja->y + h][ninja->x] = ACTION_MOVE;
			}
		}
	}
	
	if (ninja->x - 1 >= 0 && ninja->y - 1 >= 0) {
		obj = escenario[ninja->y - 1][ninja->x - 1];
		if (obj == ROCK || obj == NINJA_WATER || obj == NINJA_FIRE || obj == NINJA_SNOW) {
			acciones[ninja->y - 1][ninja->x - 1] = ACTION_CANT_MOVE;
		} else if (obj == NONE) {
			acciones[ninja->y - 1][ninja->x - 1] = ACTION_MOVE;
		}
	}
	
	if (ninja->x - 1 >= 0 && ninja->y + 1 < 5) {
		obj = escenario[ninja->y + 1][ninja->x - 1];
		if (obj == ROCK || obj == NINJA_WATER || obj == NINJA_FIRE || obj == NINJA_SNOW) {
			acciones[ninja->y + 1][ninja->x - 1] = ACTION_CANT_MOVE;
		} else if (obj == NONE) {
			acciones[ninja->y + 1][ninja->x - 1] = ACTION_MOVE;
		}
	}
	
	if (ninja->x + 1 < 9 && ninja->y - 1 >= 0) {
		obj = escenario[ninja->y - 1][ninja->x + 1];
		if (obj == ROCK || obj == NINJA_WATER || obj == NINJA_FIRE || obj == NINJA_SNOW) {
			acciones[ninja->y - 1][ninja->x + 1] = ACTION_CANT_MOVE;
		} else if (obj == NONE) {
			acciones[ninja->y - 1][ninja->x + 1] = ACTION_MOVE;
		}
	}
	
	if (ninja->x + 1 < 9 && ninja->y + 1 < 5) {
		obj = escenario[ninja->y + 1][ninja->x + 1];
		if (obj == ROCK || obj == NINJA_WATER || obj == NINJA_FIRE || obj == NINJA_SNOW) {
			acciones[ninja->y + 1][ninja->x + 1] = ACTION_CANT_MOVE;
		} else if (obj == NONE) {
			acciones[ninja->y + 1][ninja->x + 1] = ACTION_MOVE;
		}
	}
	
	acciones[ninja->y][ninja->x] = ACTION_MOVE;
}

void ask_snow_actions (ServerNinja *ninja, int escenario[5][9], int acciones[5][9]) {
	int g, h;
	int obj;
	int s;
	for (g = -3; g <= 3; g++) {
		for (h = -3; h <= 3; h++) {
			s = ((g < 0) ? -g : g) + ((h < 0) ? -h : h);
			if (s > 3) continue;
			if (ninja->x + g >= 0 && ninja->x + g < 9 && ninja->y + h >= 0 && ninja->y + h < 5) {
				obj = escenario[ninja->y + h][ninja->x + g];
				if (obj == ROCK || obj == NINJA_SNOW || obj == NINJA_FIRE || obj == NINJA_SNOW) {
					acciones[ninja->y + h][ninja->x + g] = ACTION_CANT_MOVE;
				} else if (obj == NONE) {
					acciones[ninja->y + h][ninja->x + g] = ACTION_MOVE;
				}
			}
		}
	}
	
	acciones[ninja->y][ninja->x] = ACTION_MOVE;
}

void ask_water_actions (ServerNinja *ninja, int escenario[5][9], int acciones[5][9]) {
	int g, h;
	int obj;
	
	for (g = -2; g <= 2; g++) {
		if (ninja->x + g >= 0 && ninja->x + g < 9) {
			obj = escenario[ninja->y][ninja->x + g];
			if (obj == ROCK || obj == NINJA_WATER || obj == NINJA_FIRE || obj == NINJA_SNOW) {
				acciones[ninja->y][ninja->x + g] = ACTION_CANT_MOVE;
			} else if (obj == NONE) {
				acciones[ninja->y][ninja->x + g] = ACTION_MOVE;
			}
		}
	}
	
	for (h = -2; h <= 2; h++) {
		if (ninja->y + h >= 0 && ninja->y + h < 5) {
			obj = escenario[ninja->y + h][ninja->x];
			if (obj == ROCK || obj == NINJA_WATER || obj == NINJA_FIRE || obj == NINJA_SNOW) {
				acciones[ninja->y + h][ninja->x] = ACTION_CANT_MOVE;
			} else if (obj == NONE) {
				acciones[ninja->y + h][ninja->x] = ACTION_MOVE;
			}
		}
	}
	
	if (ninja->x - 1 >= 0 && ninja->y - 1 >= 0) {
		obj = escenario[ninja->y - 1][ninja->x - 1];
		if (obj == ROCK || obj == NINJA_WATER || obj == NINJA_FIRE || obj == NINJA_SNOW) {
			acciones[ninja->y - 1][ninja->x - 1] = ACTION_CANT_MOVE;
		} else if (obj == NONE) {
			acciones[ninja->y - 1][ninja->x - 1] = ACTION_MOVE;
		}
	}
	
	if (ninja->x - 1 >= 0 && ninja->y + 1 < 5) {
		obj = escenario[ninja->y + 1][ninja->x - 1];
		if (obj == ROCK || obj == NINJA_WATER || obj == NINJA_FIRE || obj == NINJA_SNOW) {
			acciones[ninja->y + 1][ninja->x - 1] = ACTION_CANT_MOVE;
		} else if (obj == NONE) {
			acciones[ninja->y + 1][ninja->x - 1] = ACTION_MOVE;
		}
	}
	
	if (ninja->x + 1 < 9 && ninja->y - 1 >= 0) {
		obj = escenario[ninja->y - 1][ninja->x + 1];
		if (obj == ROCK || obj == NINJA_WATER || obj == NINJA_FIRE || obj == NINJA_SNOW) {
			acciones[ninja->y - 1][ninja->x + 1] = ACTION_CANT_MOVE;
		} else if (obj == NONE) {
			acciones[ninja->y - 1][ninja->x + 1] = ACTION_MOVE;
		}
	}
	
	if (ninja->x + 1 < 9 && ninja->y + 1 < 5) {
		obj = escenario[ninja->y + 1][ninja->x + 1];
		if (obj == ROCK || obj == NINJA_WATER || obj == NINJA_FIRE || obj == NINJA_SNOW) {
			acciones[ninja->y + 1][ninja->x + 1] = ACTION_CANT_MOVE;
		} else if (obj == NONE) {
			acciones[ninja->y + 1][ninja->x + 1] = ACTION_MOVE;
		}
	}
	
	acciones[ninja->y][ninja->x] = ACTION_MOVE;
}

ServerEnemy *create_server_enemy (int x, int y, int tipo) {
	ServerEnemy *obj;
	
	obj = (ServerEnemy *) malloc (sizeof (ServerEnemy));
	
	obj->x = x;
	obj->y = y;
	
	obj->tipo = tipo;
	return obj;
}

