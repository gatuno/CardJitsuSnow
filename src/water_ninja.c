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
	
	/*WATER_PIXEL,
	WATER_PIXEL2,
	WATER_PIXEL3,*/
	
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
	
	/*"images/water_ninja/pixel.png",
	"images/water_ninja/pixel2.png",
	"images/water_ninja/pixel3.png"*/
};

static SnowSprite sprite_idle[] = {
	{2,694,90,83,-23,-15,FALSE},
	{2,609,90,83,-23,-15,FALSE},
	{2,437,90,85,-23,-17,FALSE},
	{2,350,90,85,-23,-17,FALSE},
	{2,263,90,85,-23,-17,FALSE},
	{2,868,88,87,-21,-19,FALSE},
	{2,779,88,87,-21,-19,FALSE},
	{2,176,90,85,-23,-17,FALSE},
	{2,89,90,85,-23,-17,FALSE},
	{2,2,90,85,-23,-17,FALSE},
	{2,524,90,83,-23,-15,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_attack[] = {
	{266,336,93,86,-26,-18,FALSE},
	{2,262,69,72,-1,-4,FALSE},
	{158,336,65,76,-4,-8,TRUE},
	{151,250,65,76,-4,-8,FALSE},
	{73,262,65,76,-4,-8,TRUE},
	{80,336,65,76,-4,-8,TRUE},
	{2,336,65,76,-4,-8,TRUE},
	{447,94,133,54,-61,14,TRUE},
	{278,182,111,64,-44,17,FALSE},
	{155,2,151,74,-43,-4,FALSE},
	{2,178,125,82,-11,-14,FALSE},
	{155,78,147,80,-33,-12,FALSE},
	{147,160,129,88,-17,-20,FALSE},
	{304,94,141,86,-25,-18,FALSE},
	{2,90,143,86,-25,-18,FALSE},
	{308,2,147,90,-27,-22,FALSE},
	{2,2,151,86,-28,-18,FALSE},
	{391,229,109,86,-25,-18,FALSE},
	{178,405,93,86,-26,-18,TRUE},
	{90,405,93,86,-26,-18,TRUE},
	{2,405,93,86,-26,-18,TRUE},
	{373,317,93,86,-26,-18,FALSE},
	{278,248,93,86,-26,-18,FALSE},
	{266,336,93,86,-26,-18,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_celebrate[] = {
	{2,88,86,94,-15,-28,TRUE},
	{152,380,62,98,0,-32,FALSE},
	{448,402,62,98,0,-32,FALSE},
	{384,402,62,98,0,-32,FALSE},
	{284,364,62,98,0,-32,TRUE},
	{2,392,62,98,0,-32,FALSE},
	{448,104,62,98,0,-32,FALSE},
	{448,2,62,100,0,-34,FALSE},
	{68,370,82,80,-18,-14,FALSE},
	{206,164,88,82,-15,0,FALSE},
	{2,2,114,84,-7,2,FALSE},
	{350,2,96,84,-7,8,FALSE},
	{236,2,112,82,-1,8,FALSE},
	{340,88,106,76,5,10,FALSE},
	{232,86,106,76,5,10,FALSE},
	{98,162,106,74,5,10,FALSE},
	{2,176,106,72,6,10,TRUE},
	{118,84,112,76,5,9,FALSE},
	{118,2,116,80,5,9,FALSE},
	{176,314,106,64,5,10,FALSE},
	{68,304,106,64,5,10,FALSE},
	{400,336,106,64,5,10,FALSE},
	{292,298,106,64,5,10,FALSE},
	{184,248,106,64,5,10,FALSE},
	{404,270,106,64,5,10,FALSE},
	{296,232,106,64,5,10,FALSE},
	{404,204,106,64,5,10,FALSE},
	{296,166,106,64,5,10,FALSE},
	{2,284,106,64,5,10,TRUE},
	{76,238,106,64,5,10,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_move[] = {
	{101,204,72,95,-6,-27,TRUE},
	{103,66,70,97,-5,-29,TRUE},
	{2,130,66,97,-3,-29,TRUE},
	{2,198,64,97,-3,-29,TRUE},
	{2,66,62,99,-1,-31,TRUE},
	{103,2,62,99,-1,-30,TRUE},
	{2,2,62,99,-1,-29,TRUE},
	{101,138,64,97,-3,-28,TRUE},
	{99,278,68,95,-3,-27,TRUE},
	{2,264,70,95,-4,-27,TRUE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_ghost[] = {
	{0,0,66,93,-15,-27,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_hit[] = {
	{119,160,105,80,-44,-11,FALSE},
	{2,86,115,76,-48,-7,FALSE},
	{2,2,125,82,-48,-13,FALSE},
	{129,2,125,78,-49,-8,FALSE},
	{129,82,125,76,-47,-7,FALSE},
	{2,409,79,84,-6,-15,TRUE},
	{2,332,75,86,-9,-17,TRUE},
	{93,326,89,82,-23,-13,FALSE},
	{2,248,89,82,-23,-13,FALSE},
	{93,242,89,82,-23,-13,FALSE},
	{2,164,89,82,-23,-13,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_ko_intro[] = {
	{182,266,83,64,-16,4,TRUE},
	{2,160,91,70,-18,-2,FALSE},
	{103,2,97,82,-18,-14,FALSE},
	{2,80,97,78,-17,-8,FALSE},
	{2,2,99,76,-16,-8,FALSE},
	{101,144,95,76,-17,-8,FALSE},
	{2,434,81,76,-16,-8,FALSE},
	{176,351,81,76,-16,-3,TRUE},
	{89,338,85,54,-16,15,FALSE},
	{200,86,91,54,-16,15,TRUE},
	{101,86,97,56,-17,14,FALSE},
	{2,290,85,56,-17,14,FALSE},
	{89,280,85,56,-17,14,FALSE},
	{2,232,85,56,-17,14,FALSE},
	{95,222,85,56,-17,14,FALSE},
	{198,179,85,56,-17,14,TRUE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_ko_loop[] = {
	{2,350,85,56,-17,14,TRUE},
	{176,263,85,56,-17,14,TRUE},
	{89,234,85,56,-17,14,FALSE},
	{2,234,85,56,-17,14,FALSE},
	{176,176,85,56,-17,14,TRUE},
	{89,176,85,56,-17,14,FALSE},
	{2,176,85,56,-17,14,FALSE},
	{89,118,85,56,-17,14,FALSE},
	{2,118,85,56,-17,14,FALSE},
	{176,89,85,56,-17,14,TRUE},
	{89,60,85,56,-17,14,FALSE},
	{2,60,85,56,-17,14,FALSE},
	{176,2,85,56,-17,14,TRUE},
	{89,2,85,56,-17,14,FALSE},
	{2,2,85,56,-17,14,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_revived[] = {
	{158,391,85,55,-17,14,FALSE},
	{93,451,85,55,-17,14,FALSE},
	{188,218,87,67,-16,5,FALSE},
	{107,2,87,123,-10,-53,FALSE},
	{103,155,83,133,-9,-65,FALSE},
	{2,308,89,151,-11,-84,FALSE},
	{2,155,99,151,-17,-84,FALSE},
	{2,2,103,151,-19,-84,FALSE},
	{93,308,63,141,4,-74,FALSE},
	{158,290,93,99,-23,-32,FALSE},
	{362,2,87,79,-20,-12,TRUE},
	{328,378,87,77,-20,-10,TRUE},
	{354,93,89,81,-22,-14,TRUE},
	{277,184,89,81,-22,-14,TRUE},
	{279,2,89,81,-22,-14,TRUE},
	{271,93,89,81,-22,-14,TRUE},
	{196,2,89,81,-22,-14,TRUE},
	{253,287,89,81,-22,-14,TRUE},
	{188,127,89,81,-22,-14,TRUE},
	{245,391,89,81,-22,-14,TRUE},
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

struct _WaterNinja {
	int frame;
	int x, y;
	int next_x, next_y;
	int estado;
	int x_real, y_real;
	int offset_x, offset_y;
};

static SDL_Texture *water_ninja_images[NUM_WATER_NINJA_IMAGES];

WaterNinja *crear_water_ninja (int x, int y) {
	WaterNinja *obj;
	
	obj = (WaterNinja *) malloc (sizeof (WaterNinja));
	
	obj->x = x;
	obj->y = y;
	obj->x_real = MAP_X + (x * 70);
	obj->y_real = MAP_Y + (y * 70);
	
	obj->next_x = obj->x;
	obj->next_y = obj->y;
	obj->estado = WATER_NINJA_IDLE;
	obj->frame = 0;
	
	obj->offset_x = obj->offset_y = 0;
	return obj;
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

void add_water_offset (WaterNinja *ninja, int x, int y) {
	ninja->offset_x += x;
	ninja->offset_y += y;
	
	printf ("Water Ninja Offsets: %i, %i\n", ninja->offset_x, ninja->offset_y);
}

void dibujar_water (WaterNinja *ninja) {
	SDL_Rect rect, rect2, rect3;
	int calc;
	int temp;
	int est;
	int sx, sy;
	
	est = ninja->estado;
	
	sx = ninja->offset_x;
	sy = ninja->offset_y;
	if (est == WATER_NINJA_IDLE || est == WATER_NINJA_ATTACK || est == WATER_NINJA_CELEBRATE || est == WATER_NINJA_MOVE || est == WATER_NINJA_HIT || est == WATER_NINJA_REVIVED) {
		ninja->offset_x = 0;
		ninja->offset_y = 0;
	}
	calc = ninja->frame / 2;
	
	rect2.x = water_animations[est][calc].orig_x;
	rect2.y = water_animations[est][calc].orig_y;
	rect.w = rect2.w = water_animations[est][calc].w;
	rect.h = rect2.h = water_animations[est][calc].h;
	
	rect.x = ninja->x_real + ninja->offset_x + water_animations[est][calc].dest_x;
	rect.y = ninja->y_real + ninja->offset_y + water_animations[est][calc].dest_y;
	
	/*printf ("Ninja = [%3i, %3i]. Dest = [%3i, %3i]. Rect = [%3i, %3i]\n", ninja->x, ninja->y, water_animations[est][calc].dest_x, water_animations[est][calc].dest_y, rect.x, rect.y);
	rect3.x = ninja->x - 1;
	rect3.y = ninja->y - 1;
	rect3.w = 3;
	rect3.h = 3;
	
	SDL_RenderCopy (renderer, water_ninja_images[WATER_PIXEL], NULL, &rect3);
	
	rect3.x = rect.x - 1;
	rect3.y = rect.y - 1;
	rect3.w = 3;
	rect3.h = 3;
	
	SDL_RenderCopy (renderer, water_ninja_images[WATER_PIXEL2], NULL, &rect3);*/
	
	if (water_animations[est][calc].rot) {
		rect2.w = rect.h;
		rect2.h = rect.w;
		
		rect.h = rect2.h;
		rect.w = rect2.w;
		SDL_Point p;
		p.y = 0;
		p.x = rect.w;
		rect.x = ninja->x_real + ninja->offset_x - rect.w + water_animations[est][calc].dest_x;
		
		/*printf ("Rotado, por lo tanto, sus coordenadas son: [%3i, %3i]\n", rect.x, rect.y);
		
		rect3.x = rect.x - 1;
		rect3.y = rect.y - 1;
		rect3.w = rect3.h = 3;
		
		SDL_RenderCopy (renderer, water_ninja_images[WATER_PIXEL3], NULL, &rect3);*/
		
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
	
	ninja->offset_x = sx;
	ninja->offset_y = sy;
}

void dibujar_ghost_water (WaterNinja *ninja) {
	SDL_Rect rect;
	if (ninja->x != ninja->next_x || ninja->y != ninja->next_y) {
		/* Dibujar el ninja fantasma */
		rect.x = MAP_X + (ninja->next_x * 70) + water_animations[WATER_NINJA_GHOST][0].dest_x;
		rect.y = MAP_Y + (ninja->next_y * 70) + water_animations[WATER_NINJA_GHOST][0].dest_y;
		
		SDL_QueryTexture (water_ninja_images[WATER_NINJA_GHOST], NULL, NULL, &rect.w, &rect.h);
		SDL_RenderCopy (renderer, water_ninja_images[WATER_NINJA_GHOST], NULL, &rect);
	}
}

void ask_water_actions (WaterNinja *ninja, int escenario[5][9], int acciones[5][9]) {
	int g, h;
	int obj;
	
	for (g = -2; g < 2; g++) {
		if (ninja->x + g >= 0 && ninja->x + g < 9) {
			obj = escenario[ninja->x + g][ninja->y];
			if (obj == ROCK || obj == NINJA_WATER || obj == NINJA_FIRE || obj == NINJA_SNOW) {
				acciones[ninja->x + g][ninja->y] |= ACTION_CANT_MOVE;
			} else if (obj == NONE) {
				acciones[ninja->x + g][ninja->y] = ACTION_MOVE;
			}
		}
	}
	
	for (h = -2; h < 2; h++) {
		if (ninja->y + h >= 0 && ninja->y + h < 5) {
			obj = escenario[ninja->x][ninja->y + h];
			if (obj == ROCK || obj == NINJA_WATER || obj == NINJA_FIRE || obj == NINJA_SNOW) {
				acciones[ninja->x][ninja->y + h] |= ACTION_CANT_MOVE;
			} else if (obj == NONE) {
				acciones[ninja->x][ninja->y + h] = ACTION_MOVE;
			}
		}
	}
	
	if (ninja->x - 1 >= 0 && ninja->y - 1 >= 0) {
		obj = escenario[ninja->x - 1][ninja->y - 1];
		if (obj == ROCK || obj == NINJA_WATER || obj == NINJA_FIRE || obj == NINJA_SNOW) {
			acciones[ninja->x - 1][ninja->y - 1] |= ACTION_CANT_MOVE;
		} else if (obj == NONE) {
			acciones[ninja->x - 1][ninja->y - 1] = ACTION_MOVE;
		}
	}
	
	acciones[ninja->x][ninja->y] = ACTION_MOVE;
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
	}
}
