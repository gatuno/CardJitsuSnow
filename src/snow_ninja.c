/*
 * snow_ninja.c
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

#include "snow_ninja.h"

/* Listar las imágenes del Ninja de nieve */
enum {
	SNOW_NINJA_IDLE,
	SNOW_NINJA_ATTACK,
	SNOW_NINJA_HEAL,
	SNOW_NINJA_CELEBRATE,
	SNOW_NINJA_MOVE,
	SNOW_NINJA_GHOST,
	SNOW_NINJA_HIT,
	SNOW_NINJA_KO_INTRO,
	SNOW_NINJA_KO_LOOP,
	SNOW_NINJA_POWERCARD_IGLOO_1,
	SNOW_NINJA_POWERCARD_IGLOO_2,
	SNOW_NINJA_POWERCARD_SUMMON,
	SNOW_NINJA_POWERCARD_SNOW,
	SNOW_NINJA_REVIVED,
	SNOW_NINJA_REVIVE_OTHER,
	SNOW_NINJA_REVIVE_OTHER_LOOP,
	
	NUM_SNOW_NINJA_IMAGES
};

static const char *snow_ninja_images_names[NUM_SNOW_NINJA_IMAGES] = {
	"images/snow_ninja/idle.png",
	"images/snow_ninja/attack.png",
	"images/snow_ninja/heal.png",
	"images/snow_ninja/celebrate.png",
	"images/snow_ninja/move.png",
	"images/snow_ninja/ghost.png",
	"images/snow_ninja/hit.png",
	"images/snow_ninja/knockout_intro.png",
	"images/snow_ninja/knockout_loop.png",
	"images/snow_ninja/powercard_igloo_1.png",
	"images/snow_ninja/powercard_igloo_2.png",
	"images/snow_ninja/powercard_summon.png",
	"images/snow_ninja/powercard_snow.png",
	"images/snow_ninja/revived.png",
	"images/snow_ninja/revive_other.png",
	"images/snow_ninja/revive_other_loop.png",
};

static SnowSprite sprite_idle[] = {
	{2,454,72,46,21,9,FALSE},
	{2,406,72,46,21,9,FALSE},
	{2,308,72,48,21,7,FALSE},
	{76,298,72,48,21,7,TRUE},
	{2,258,72,48,21,7,FALSE},
	{2,106,72,50,21,5,FALSE},
	{76,76,72,50,21,5,TRUE},
	{2,54,72,50,21,5,FALSE},
	{76,2,72,50,21,5,TRUE},
	{2,2,72,50,21,5,FALSE},
	{76,224,72,48,21,7,TRUE},
	{2,208,72,48,21,7,FALSE},
	{2,158,72,48,21,7,FALSE},
	{76,150,72,48,21,7,TRUE},
	{76,372,72,46,21,9,TRUE},
	{2,358,72,46,21,9,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_attack[] = {
	{178,263,71,59,22,50,FALSE},
	{95,168,89,83,4,26,FALSE},
	{2,77,91,85,2,24,FALSE},
	{101,2,93,81,0,28,FALSE},
	{95,85,91,81,2,28,FALSE},
	{2,225,89,81,4,28,FALSE},
	{2,2,97,73,6,36,FALSE},
	{2,164,91,59,14,50,FALSE},
	{196,2,89,55,18,54,TRUE},
	{166,381,79,53,21,56,FALSE},
	{2,365,81,55,22,54,FALSE},
	{87,310,81,55,22,54,FALSE},
	{2,308,83,55,21,54,FALSE},
	{93,253,83,55,21,54,FALSE},
	{170,324,79,55,21,54,FALSE},
	{85,367,79,53,21,56,FALSE},
	{186,180,81,67,21,42,TRUE},
	{188,93,85,59,21,63,TRUE},
	{51,436,71,47,22,63,TRUE},
	{2,436,71,47,22,64,TRUE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_heal [] = {
	{180,187,71,45,17,58,FALSE},
	{89,239,61,55,18,47,FALSE},
	{89,296,59,51,12,51,FALSE},
	{152,295,61,53,16,49,FALSE},
	{180,234,67,59,16,43,FALSE},
	{180,2,71,87,17,15,FALSE},
	{91,162,75,87,16,15,TRUE},
	{91,83,77,87,17,15,TRUE},
	{91,2,79,87,17,15,TRUE},
	{2,162,75,87,16,15,TRUE},
	{2,83,77,87,17,15,TRUE},
	{2,2,79,87,17,15,TRUE},
	{2,162,75,87,16,15,TRUE},
	{2,83,77,87,17,15,TRUE},
	{2,2,79,87,17,15,TRUE},
	{2,239,85,63,17,43,FALSE},
	{180,91,71,47,17,56,FALSE},
	{180,140,71,45,17,58,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_celebrate[] = {
	{152,276,72,46,32,70,FALSE},
	{82,422,64,58,33,56,FALSE},
	{148,456,64,50,26,64,FALSE},
	{148,390,58,64,33,50,FALSE},
	{148,324,58,64,33,50,FALSE},
	{148,324,58,64,33,50,FALSE},
	{148,324,58,64,33,50,FALSE},
	{148,324,58,64,33,50,FALSE},
	{152,210,58,64,32,50,FALSE},
	{2,422,78,78,9,36,FALSE},
	{80,214,70,102,15,12,FALSE},
	{2,2,76,106,18,8,FALSE},
	{162,2,80,102,14,12,FALSE},
	{80,2,80,102,13,12,FALSE},
	{2,110,78,102,14,12,FALSE},
	{2,214,76,102,13,12,FALSE},
	{82,106,72,102,12,12,FALSE},
	{2,318,72,102,12,12,FALSE},
	{76,318,70,102,14,12,FALSE},
	{156,106,70,102,14,12,FALSE},
	{156,106,70,102,14,12,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_move[] = {
	{150,71,72,67,5,14,FALSE},
	{76,71,72,67,5,14,FALSE},
	{2,71,72,67,5,14,FALSE},
	{76,140,72,65,5,16,FALSE},
	{150,205,72,63,5,18,FALSE},
	{150,2,72,67,5,14,FALSE},
	{76,2,72,67,5,14,FALSE},
	{2,2,72,67,5,14,FALSE},
	{2,140,72,65,5,16,FALSE},
	{150,140,72,63,5,18,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_ghost[] = {
	{0,0,70,64,0,1,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_hit[] = {
	{75,319,71,45,35,33,FALSE},
	{2,164,93,69,16,8,FALSE},
	{2,81,111,81,8,0,FALSE},
	{119,75,113,71,8,6,FALSE},
	{2,2,115,77,8,5,FALSE},
	{119,2,113,71,13,6,FALSE},
	{115,148,93,73,27,8,FALSE},
	{170,223,71,47,35,30,FALSE},
	{97,223,71,47,35,32,FALSE},
	{148,272,71,45,35,34,FALSE},
	{2,282,71,45,35,34,FALSE},
	{75,272,71,45,35,34,FALSE},
	{2,235,71,45,35,33,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_ko_intro[] = {
	{186,142,85,68,4,21,TRUE},
	{170,229,77,76,19,13,FALSE},
	{2,2,103,86,0,3,FALSE},
	{2,90,91,86,14,6,FALSE},
	{107,2,97,82,11,7,FALSE},
	{2,178,79,82,23,7,TRUE},
	{86,210,61,82,23,7,TRUE},
	{206,2,47,82,31,12,FALSE},
	{2,259,81,64,12,26,FALSE},
	{95,142,89,66,9,24,FALSE},
	{107,86,95,54,8,37,FALSE},
	{158,307,71,54,20,37,FALSE},
	{75,329,71,52,20,39,FALSE},
	{2,325,71,54,20,37,FALSE},
	{85,273,71,54,20,37,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_ko_loop[] = {
	{2,228,70,53,18,38,FALSE},
	{74,226,70,53,18,38,FALSE},
	{146,171,70,53,18,38,FALSE},
	{2,173,70,53,18,38,FALSE},
	{146,279,70,51,18,40,FALSE},
	{146,226,70,51,18,40,FALSE},
	{74,171,70,53,18,38,FALSE},
	{146,116,70,53,18,38,FALSE},
	{2,116,70,55,18,36,FALSE},
	{146,59,70,55,18,36,FALSE},
	{74,59,70,55,18,36,FALSE},
	{2,59,70,55,18,36,FALSE},
	{146,2,70,55,18,36,FALSE},
	{74,116,70,53,18,38,FALSE},
	{74,2,70,55,18,36,FALSE},
	{2,2,70,55,18,36,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_revived[] = {
	{81,334,61,43,39,85,TRUE},
	{2,346,65,53,35,75,FALSE},
	{2,281,77,63,26,67,FALSE},
	{117,170,83,111,25,17,TRUE},
	{119,89,79,115,26,12,TRUE},
	{119,2,85,115,24,12,TRUE},
	{2,2,93,115,20,12,TRUE},
	{2,97,97,113,18,13,TRUE},
	{2,196,83,109,37,17,TRUE},
	{113,255,91,77,28,50,FALSE},
	{126,334,71,45,29,83,FALSE},
	{49,401,71,45,29,83,TRUE},
	{2,401,71,45,29,82,TRUE},
	{206,328,71,45,29,83,TRUE},
	{206,255,71,45,29,82,TRUE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

SnowSprite *snow_animations[NUM_SNOW_NINJA_IMAGES] = {
	sprite_idle,
	sprite_attack,
	sprite_heal,
	sprite_celebrate,
	sprite_move,
	sprite_ghost,
	sprite_hit,
	sprite_ko_intro,
	sprite_ko_loop,
	NULL,
	NULL,
	NULL,
	NULL,
	sprite_revived,
};

static float snow_ninja_offsets[NUM_SNOW_NINJA_IMAGES][4] = {
	{0.5, 1.09375, 112.0, 56.0},
	{0.5, 0.84375, 111.0, 139.0},
	{0.46875, 1.0, 111.0, 111.0},
	{0.5625, 0.875, 112.0, 138.0},
	{0.5, 1.0625, 84.0, 83.0},
	{0.46875, 1.125, 70.0, 66.0},
	{0.5, 0.96875, 139.0, 89.0},
	{0.46875, 0.875, 115.0, 112.0},
	{0.46875, 0.875, 112.0, 111.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.46875, 0.96875, 139.0, 139.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0}
};

static int snow_ninja_offsets_int[NUM_SNOW_NINJA_IMAGES][2];

struct _SnowNinja {
	int frame;
	int x, y;
	int next_x, next_y;
	int estado;
	int x_real, y_real;
	int next_x_real, next_y_real;
	int sum_x, sum_y;
};

static SDL_Texture *snow_ninja_images[NUM_SNOW_NINJA_IMAGES];

SnowNinja *create_snow_ninja (int x, int y) {
	SnowNinja *obj;
	
	obj = (SnowNinja *) malloc (sizeof (SnowNinja));
	
	obj->x = x;
	obj->y = y;
	obj->x_real = MAP_X + (x * 70) + 35;
	obj->y_real = MAP_Y + (y * 70) + 70;
	
	obj->next_x = obj->x;
	obj->next_y = obj->y;
	obj->estado = SNOW_NINJA_IDLE;
	obj->frame = 0;
	
	return obj;
}

void ask_snow_coords (SnowNinja *ninja, int *x, int *y) {
	if (x != NULL) {
		*x = ninja->x;
	}
	
	if (y != NULL) {
		*y = ninja->y;
	}
}

int is_snow_done (SnowNinja *ninja) {
	if (ninja->estado == SNOW_NINJA_IDLE) return TRUE;
	
	return FALSE;
}

void put_idle_snow (SnowNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = SNOW_NINJA_IDLE;
}

void attack_snow (SnowNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = SNOW_NINJA_ATTACK;
}

void heal_snow (SnowNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = SNOW_NINJA_HEAL;
}

void celebrate_snow (SnowNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = SNOW_NINJA_CELEBRATE;
}

void move_snow (SnowNinja *ninja, int x, int y) {
	ninja->frame = 0;
	ninja->estado = SNOW_NINJA_MOVE;
	
	ninja->x = ninja->next_x = x;
	ninja->y = ninja->next_y = y;
	
	/* Calcular las siguientes coordenadas reales */
	ninja->next_x_real = MAP_X + (x * 70) + 35;
	ninja->next_y_real = MAP_Y + (y * 70) + 70;
	
	ninja->sum_x = (ninja->next_x_real - ninja->x_real) / 35;
	ninja->sum_y = (ninja->next_y_real - ninja->y_real) / 35;
}

void ghost_move_snow (SnowNinja *ninja, int x, int y) {
	ninja->next_x = x;
	ninja->next_y = y;
}

void ko_snow (SnowNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = SNOW_NINJA_KO_INTRO;
}

void hit_snow (SnowNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = SNOW_NINJA_HIT;
}

void revive_snow (SnowNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = SNOW_NINJA_REVIVED;
}

void draw_snow_ninja (SnowNinja *ninja) {
	SDL_Rect rect, rect2, rect3;
	int calc;
	int temp;
	int est;
	int sx, sy;
	
	est = ninja->estado;
	
	calc = ninja->frame / 2;
	
	rect2.x = snow_animations[est][calc].orig_x;
	rect2.y = snow_animations[est][calc].orig_y;
	rect.w = rect2.w = snow_animations[est][calc].w;
	rect.h = rect2.h = snow_animations[est][calc].h;
	
	if (ninja->estado == SNOW_NINJA_MOVE) {
		ninja->x_real += ninja->sum_x;
		ninja->y_real += ninja->sum_y;
	}
	
	rect.x = ninja->x_real - snow_ninja_offsets_int[est][0] + snow_animations[est][calc].dest_x;
	rect.y = ninja->y_real - snow_ninja_offsets_int[est][1] + snow_animations[est][calc].dest_y;
	
	if (snow_animations[est][calc].rot) {
		rect2.w = rect.h;
		rect2.h = rect.w;
		
		rect.h = rect2.h;
		rect.w = rect2.w;
		SDL_Point p;
		p.y = 0;
		p.x = rect.w;
		rect.x = ninja->x_real - snow_ninja_offsets_int[est][0] - rect.w + snow_animations[est][calc].dest_x;
		
		SDL_RenderCopyEx (renderer, snow_ninja_images[est], &rect2, &rect, 270.0, &p, SDL_FLIP_NONE);
	} else {
		SDL_RenderCopy (renderer, snow_ninja_images[est], &rect2, &rect);
	}
	
	ninja->frame++;
	calc = ninja->frame / 2;
	if (snow_animations[est][calc].w == -1 && snow_animations[est][calc].h == -1) {
		ninja->frame = 0;
		if (ninja->estado == SNOW_NINJA_ATTACK) {
			ninja->estado = SNOW_NINJA_IDLE;
		} else if (ninja->estado == SNOW_NINJA_CELEBRATE) {
			ninja->estado = SNOW_NINJA_CELEBRATE;
		} else if (ninja->estado == SNOW_NINJA_KO_INTRO) {
			ninja->estado = SNOW_NINJA_KO_LOOP;
		} else if (ninja->estado == SNOW_NINJA_HIT) {
			ninja->estado = SNOW_NINJA_IDLE;
		} else if (ninja->estado == SNOW_NINJA_REVIVED) {
			ninja->estado = SNOW_NINJA_IDLE;
		} else if (ninja->estado == SNOW_NINJA_HEAL) {
			ninja->estado = SNOW_NINJA_IDLE;
		} else if (ninja->estado == SNOW_NINJA_MOVE) {
			ninja->estado = SNOW_NINJA_MOVE;
		}
	}
	if (ninja->estado == SNOW_NINJA_MOVE) {
		if (ninja->x_real == ninja->next_x_real && ninja->y_real == ninja->next_y_real) {
			/* Llegamos al destino */
			ninja->estado = SNOW_NINJA_IDLE;
			ninja->frame = 0;
		}
	}
}

void draw_ghost_snow_ninja (SnowNinja *ninja) {
	SDL_Rect rect;
	if (ninja->x != ninja->next_x || ninja->y != ninja->next_y) {
		/* Dibujar el ninja fantasma */
		rect.x = MAP_X + (ninja->next_x * 70) + 35 - snow_ninja_offsets_int[SNOW_NINJA_GHOST][0] + snow_animations[SNOW_NINJA_GHOST][0].dest_x;
		rect.y = MAP_Y + (ninja->next_y * 70) + 70 - snow_ninja_offsets_int[SNOW_NINJA_GHOST][1] + snow_animations[SNOW_NINJA_GHOST][0].dest_y;
		
		SDL_QueryTexture (snow_ninja_images[SNOW_NINJA_GHOST], NULL, NULL, &rect.w, &rect.h);
		SDL_RenderCopy (renderer, snow_ninja_images[SNOW_NINJA_GHOST], NULL, &rect);
	}
}

void ask_snow_actions (SnowNinja *ninja, int escenario[5][9], int acciones[5][9]) {
	int g, h;
	int obj;
	int s;
	for (g = -3; g <= 3; g++) {
		for (h = -3; h <= 3; h++) {
			s = ((g < 0) ? -g : g) + ((h < 0) ? -h : h);
			if (s > 3) continue;
			if (ninja->x + g >= 0 && ninja->x + g < 9 && ninja->y + h >= 0 && ninja->y + h < 5) {
				obj = escenario[ninja->y + h][ninja->x + g];
				if (obj == ROCK || obj == NINJA_WATER || obj == NINJA_FIRE || obj == NINJA_SNOW) {
					acciones[ninja->y + h][ninja->x + g] = ACTION_CANT_MOVE;
				} else if (obj == NONE) {
					acciones[ninja->y + h][ninja->x + g] = ACTION_MOVE;
				}
			}
		}
	}
	
	acciones[ninja->y][ninja->x] = ACTION_MOVE;
	
	for (g = -3; g <= 3; g++) {
		for (h = -3; h <= 3; h++) {
			s = ((g < 0) ? -g : g) + ((h < 0) ? -h : h);
			
			if (s > 3) continue;
			if (ninja->next_x + g >= 0 && ninja->next_x + g < 9 && ninja->next_y + h >= 0 && ninja->next_y + h < 5) {
				obj = escenario[ninja->next_y + h][ninja->next_x + g];
				if (obj >= ENEMY_1 && obj <= ENEMY_4) {
					acciones[ninja->next_y + h][ninja->next_x + g] |= ACTION_ATTACK;
				}
			}
		}
	}
}

void setup_snow_ninja (void) {
	SDL_Surface * image;
	SDL_Texture * texture;
	int g;
	
	char buffer_file[8192];
	
	for (g = 0; g < NUM_SNOW_NINJA_IMAGES; g++) {
		sprintf (buffer_file, "%s%s", systemdata_path, snow_ninja_images_names[g]);
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
		snow_ninja_images[g] = texture;
		SDL_FreeSurface (image);
		/* TODO: Mostrar la carga de porcentaje */
		
		/* Calcular los centros */
		snow_ninja_offsets_int[g][0] = (int) roundf (snow_ninja_offsets[g][0] * snow_ninja_offsets[g][2]);
		snow_ninja_offsets_int[g][1] = (int) roundf (snow_ninja_offsets[g][1] * snow_ninja_offsets[g][3]);
	}
}
