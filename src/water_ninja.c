/*
 * water_ninja.c
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

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>
#include "gettext.h"
#define _(string) gettext (string)

#include "path.h"
#include "snow.h"

#include "water_ninja.h"

/* Listar las imágenes del Ninja de Agua */
enum {
	WATER_NINJA_IDLE,
	WATER_NINJA_ATTACK,
	WATER_NINJA_CELEBRATE,
	WATER_NINJA_MOVE,
	WATER_NINJA_GHOST,
	WATER_NINJA_HIT,
	WATER_NINJA_KO_INTRO,
	WATER_NINJA_KO_LOOP,
	WATER_NINJA_POWERCARD_FISHDROP,
	WATER_NINJA_POWERCARD_SUMMON,
	WATER_NINJA_POWERCARD_WATER,
	WATER_NINJA_REVIVED,
	WATER_NINJA_REVIVE_OTHER,
	WATER_NINJA_REVIVE_OTHER_LOOP,
	
	NUM_WATER_NINJA_IMAGES
};

static const char *water_ninja_images_names[NUM_WATER_NINJA_IMAGES] = {
	"images/water_ninja/idle.png",
	"images/water_ninja/attack.png",
	"images/water_ninja/celebrate.png",
	"images/water_ninja/move.png",
	"images/water_ninja/ghost.png",
	"images/water_ninja/hit.png",
	"images/water_ninja/knockout_intro.png",
	"images/water_ninja/knockout_loop.png",
	"images/water_ninja/powercard_fishdrop.png",
	"images/water_ninja/powercard_summon.png",
	"images/water_ninja/powercard_water_loop.png",
	"images/water_ninja/revived.png",
	"images/water_ninja/revive_other.png",
	"images/water_ninja/revive_other_loop.png",
};

static SnowSprite sprite_idle[] = {
	{2,694,90,83,7,15,FALSE},
	{2,609,90,83,7,15,FALSE},
	{2,437,90,85,7,13,FALSE},
	{2,350,90,85,7,13,FALSE},
	{2,263,90,85,7,13,FALSE},
	{2,868,88,87,9,11,FALSE},
	{2,779,88,87,9,11,FALSE},
	{2,176,90,85,7,13,FALSE},
	{2,89,90,85,7,13,FALSE},
	{2,2,90,85,7,13,FALSE},
	{2,524,90,83,7,15,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_attack[] = {
	{266,336,93,86,38,12,FALSE},
	{2,262,69,72,63,26,FALSE},
	{158,336,65,76,60,22,TRUE},
	{151,250,65,76,60,22,FALSE},
	{73,262,65,76,60,22,TRUE},
	{80,336,65,76,60,22,TRUE},
	{2,336,65,76,60,22,TRUE},
	{447,94,133,54,3,44,TRUE},
	{278,182,111,64,20,47,FALSE},
	{155,2,151,74,21,26,FALSE},
	{2,178,125,82,53,16,FALSE},
	{155,78,147,80,31,18,FALSE},
	{147,160,129,88,47,10,FALSE},
	{304,94,141,86,39,12,FALSE},
	{2,90,143,86,39,12,FALSE},
	{308,2,147,90,37,8,FALSE},
	{2,2,151,86,36,12,FALSE},
	{391,229,109,86,39,12,FALSE},
	{178,405,93,86,38,12,TRUE},
	{90,405,93,86,38,12,TRUE},
	{2,405,93,86,38,12,TRUE},
	{373,317,93,86,38,12,FALSE},
	{278,248,93,86,38,12,FALSE},
	{266,336,93,86,38,12,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_celebrate[] = {
	{2,88,86,94,29,24,TRUE},
	{152,380,62,98,44,20,FALSE},
	{448,402,62,98,44,20,FALSE},
	{384,402,62,98,44,20,FALSE},
	{284,364,62,98,44,20,TRUE},
	{2,392,62,98,44,20,FALSE},
	{448,104,62,98,44,20,FALSE},
	{448,2,62,100,44,18,FALSE},
	{68,370,82,80,26,38,FALSE},
	{206,164,88,82,29,52,FALSE},
	{2,2,114,84,37,54,FALSE},
	{350,2,96,84,37,60,FALSE},
	{236,2,112,82,43,60,FALSE},
	{340,88,106,76,49,62,FALSE},
	{232,86,106,76,49,62,FALSE},
	{98,162,106,74,49,62,FALSE},
	{2,176,106,72,50,62,TRUE},
	{118,84,112,76,49,61,FALSE},
	{118,2,116,80,49,61,FALSE},
	{176,314,106,64,49,62,FALSE},
	{68,304,106,64,49,62,FALSE},
	{400,336,106,64,49,62,FALSE},
	{292,298,106,64,49,62,FALSE},
	{184,248,106,64,49,62,FALSE},
	{404,270,106,64,49,62,FALSE},
	{296,232,106,64,49,62,FALSE},
	{404,204,106,64,49,62,FALSE},
	{296,166,106,64,49,62,FALSE},
	{2,284,106,64,49,62,TRUE},
	{76,238,106,64,49,62,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_move[] = {
	{101,204,72,95,16,22,TRUE},
	{103,66,70,97,17,20,TRUE},
	{2,130,66,97,19,20,TRUE},
	{2,198,64,97,19,20,TRUE},
	{2,66,62,99,21,18,TRUE},
	{103,2,62,99,21,19,TRUE},
	{2,2,62,99,21,20,TRUE},
	{101,138,64,97,19,21,TRUE},
	{99,278,68,95,19,22,TRUE},
	{2,264,70,95,18,22,TRUE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_ghost[] = {
	{0,0,66,93,0,5,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_hit[] = {
	{119,160,105,80,15,19,FALSE},
	{2,86,115,76,11,23,FALSE},
	{2,2,125,82,11,17,FALSE},
	{129,2,125,78,10,22,FALSE},
	{129,82,125,76,12,23,FALSE},
	{2,409,79,84,53,15,TRUE},
	{2,332,75,86,50,13,TRUE},
	{93,326,89,82,36,17,FALSE},
	{2,248,89,82,36,17,FALSE},
	{93,242,89,82,36,17,FALSE},
	{2,164,89,82,36,17,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_ko_intro[] = {
	{182,266,83,64,29,25,TRUE},
	{2,160,91,70,27,19,FALSE},
	{103,2,97,82,27,7,FALSE},
	{2,80,97,78,28,13,FALSE},
	{2,2,99,76,29,13,FALSE},
	{101,144,95,76,28,13,FALSE},
	{2,434,81,76,29,13,FALSE},
	{176,351,81,76,29,18,TRUE},
	{89,338,85,54,29,36,FALSE},
	{200,86,91,54,29,36,TRUE},
	{101,86,97,56,28,35,FALSE},
	{2,290,85,56,28,35,FALSE},
	{89,280,85,56,28,35,FALSE},
	{2,232,85,56,28,35,FALSE},
	{95,222,85,56,28,35,FALSE},
	{198,179,85,56,28,35,TRUE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_ko_loop[] = {
	{2,350,85,56,28,35,TRUE},
	{176,263,85,56,28,35,TRUE},
	{89,234,85,56,28,35,FALSE},
	{2,234,85,56,28,35,FALSE},
	{176,176,85,56,28,35,TRUE},
	{89,176,85,56,28,35,FALSE},
	{2,176,85,56,28,35,FALSE},
	{89,118,85,56,28,35,FALSE},
	{2,118,85,56,28,35,FALSE},
	{176,89,85,56,28,35,TRUE},
	{89,60,85,56,28,35,FALSE},
	{2,60,85,56,28,35,FALSE},
	{176,2,85,56,28,35,TRUE},
	{89,2,85,56,28,35,FALSE},
	{2,2,85,56,28,35,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_revived[] = {
	{158,391,85,55,28,122,FALSE},
	{93,451,85,55,28,122,FALSE},
	{188,218,87,67,29,113,FALSE},
	{107,2,87,123,35,55,FALSE},
	{103,155,83,133,36,43,FALSE},
	{2,308,89,151,34,24,FALSE},
	{2,155,99,151,28,24,FALSE},
	{2,2,103,151,26,24,FALSE},
	{93,308,63,141,49,34,FALSE},
	{158,290,93,99,22,76,FALSE},
	{362,2,87,79,25,96,TRUE},
	{328,378,87,77,25,98,TRUE},
	{354,93,89,81,23,94,TRUE},
	{277,184,89,81,23,94,TRUE},
	{279,2,89,81,23,94,TRUE},
	{271,93,89,81,23,94,TRUE},
	{196,2,89,81,23,94,TRUE},
	{253,287,89,81,23,94,TRUE},
	{188,127,89,81,23,94,TRUE},
	{245,391,89,81,23,94,TRUE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

SnowSprite *water_animations[NUM_WATER_NINJA_IMAGES] = {
	sprite_idle,
	sprite_attack,
	sprite_celebrate,
	sprite_move,
	sprite_ghost,
	sprite_hit,
	sprite_ko_intro,
	sprite_ko_loop,
	NULL,
	NULL,
	NULL,
	sprite_revived,
};

static float water_ninja_offsets[NUM_WATER_NINJA_IMAGES][4] = {
	{0.59375, 0.9375, 112.0, 111.0},
	{0.52307, 0.92857, 195.0, 112.0},
	{0.5, 0.75, 166.0, 166.0},
	{0.5625, 0.90625, 112.0, 139.0},
	{0.65625, 1.0625, 78.0, 99.0},
	{0.57485, 0.92857, 167.0, 112.0},
	{0.58992, 0.84821, 139.0, 112.0},
	{0.58992, 0.84821, 139.0, 112.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.58992, 0.81614, 139.0, 223.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0}
};

static int water_ninja_offsets_int[NUM_WATER_NINJA_IMAGES][2];

struct _WaterNinja {
	int frame;
	int x, y;
	int next_x, next_y;
	int estado;
	int x_real, y_real;
};

static SDL_Texture *water_ninja_images[NUM_WATER_NINJA_IMAGES];

WaterNinja *create_water_ninja (int x, int y) {
	WaterNinja *obj;
	
	obj = (WaterNinja *) malloc (sizeof (WaterNinja));
	
	obj->x = x;
	obj->y = y;
	obj->x_real = MAP_X + (x * 70) + 35;
	obj->y_real = MAP_Y + (y * 70) + 70;
	
	obj->next_x = obj->x;
	obj->next_y = obj->y;
	obj->estado = WATER_NINJA_IDLE;
	obj->frame = 0;
	
	return obj;
}

void ask_water_coords (WaterNinja *ninja, int *x, int *y) {
	if (x != NULL) {
		*x = ninja->x;
	}
	
	if (y != NULL) {
		*y = ninja->y;
	}
}

void put_idle_water (WaterNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = WATER_NINJA_IDLE;
}

void attack_water (WaterNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = WATER_NINJA_ATTACK;
}

void celebrate_water (WaterNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = WATER_NINJA_CELEBRATE;
}

void move_water (WaterNinja *ninja, int x, int y) {
	ninja->next_x = x;
	ninja->next_y = y;
}

void prev_move_water (WaterNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = WATER_NINJA_MOVE;
}

void ko_water (WaterNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = WATER_NINJA_KO_INTRO;
}

void hit_water (WaterNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = WATER_NINJA_HIT;
}

void revive_water (WaterNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = WATER_NINJA_REVIVED;
}

void draw_water_ninja (WaterNinja *ninja) {
	SDL_Rect rect, rect2, rect3;
	int calc;
	int temp;
	int est;
	int sx, sy;
	
	est = ninja->estado;
	
	calc = ninja->frame / 2;
	
	rect2.x = water_animations[est][calc].orig_x;
	rect2.y = water_animations[est][calc].orig_y;
	rect.w = rect2.w = water_animations[est][calc].w;
	rect.h = rect2.h = water_animations[est][calc].h;
	
	rect.x = ninja->x_real - water_ninja_offsets_int[est][0] + water_animations[est][calc].dest_x;
	rect.y = ninja->y_real - water_ninja_offsets_int[est][1] + water_animations[est][calc].dest_y;
	
	if (water_animations[est][calc].rot) {
		rect2.w = rect.h;
		rect2.h = rect.w;
		
		rect.h = rect2.h;
		rect.w = rect2.w;
		SDL_Point p;
		p.y = 0;
		p.x = rect.w;
		rect.x = ninja->x_real - water_ninja_offsets_int[est][0] - rect.w + water_animations[est][calc].dest_x;
		
		SDL_RenderCopyEx (renderer, water_ninja_images[est], &rect2, &rect, 270.0, &p, SDL_FLIP_NONE);
	} else {
		SDL_RenderCopy (renderer, water_ninja_images[est], &rect2, &rect);
	}
	
	ninja->frame++;
	calc = ninja->frame / 2;
	if (water_animations[est][calc].w == -1 && water_animations[est][calc].h == -1) {
		ninja->frame = 0;
		if (ninja->estado == WATER_NINJA_ATTACK) {
			ninja->estado = WATER_NINJA_IDLE;
		} else if (ninja->estado == WATER_NINJA_CELEBRATE) {
			ninja->estado = WATER_NINJA_CELEBRATE;
		} else if (ninja->estado == WATER_NINJA_KO_INTRO) {
			ninja->estado = WATER_NINJA_KO_LOOP;
		} else if (ninja->estado == WATER_NINJA_HIT) {
			ninja->estado = WATER_NINJA_IDLE;
		} else if (ninja->estado == WATER_NINJA_REVIVED) {
			ninja->estado = WATER_NINJA_IDLE;
		}
	}
}

void draw_ghost_water_ninja (WaterNinja *ninja) {
	SDL_Rect rect;
	if (ninja->x != ninja->next_x || ninja->y != ninja->next_y) {
		/* Dibujar el ninja fantasma */
		rect.x = MAP_X + (ninja->next_x * 70) + 35 - water_ninja_offsets_int[WATER_NINJA_GHOST][0] + water_animations[WATER_NINJA_GHOST][0].dest_x;
		rect.y = MAP_Y + (ninja->next_y * 70) + 70 - water_ninja_offsets_int[WATER_NINJA_GHOST][1] + water_animations[WATER_NINJA_GHOST][0].dest_y;
		
		SDL_QueryTexture (water_ninja_images[WATER_NINJA_GHOST], NULL, NULL, &rect.w, &rect.h);
		SDL_RenderCopy (renderer, water_ninja_images[WATER_NINJA_GHOST], NULL, &rect);
	}
}

void ask_water_actions (WaterNinja *ninja, int escenario[5][9], int acciones[5][9]) {
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

void setup_water_ninja (void) {
	SDL_Surface * image;
	SDL_Texture * texture;
	int g;
	
	char buffer_file[8192];
	
	for (g = 0; g < NUM_WATER_NINJA_IMAGES; g++) {
		sprintf (buffer_file, "%s%s", systemdata_path, water_ninja_images_names[g]);
		image = IMG_Load (buffer_file);
		
		if (image == NULL) {
			fprintf (stderr,
				_("Failed to load data file:\n"
				"%s\n"
				"The error returned by SDL is:\n"
				"%s\n"), buffer_file, SDL_GetError());
			SDL_Quit ();
			exit (1);
		}
		
		texture = SDL_CreateTextureFromSurface (renderer, image);
		water_ninja_images[g] = texture;
		SDL_FreeSurface (image);
		/* TODO: Mostrar la carga de porcentaje */
		
		/* Calcular los centros */
		water_ninja_offsets_int[g][0] = (int) roundf (water_ninja_offsets[g][0] * water_ninja_offsets[g][2]);
		water_ninja_offsets_int[g][1] = (int) roundf (water_ninja_offsets[g][1] * water_ninja_offsets[g][3]);
	}
}
