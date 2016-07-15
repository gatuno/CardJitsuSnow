/*
 * fire_ninja.c
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
#include <math.h>

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

#include "fire_ninja.h"

/* Listar las imágenes del Ninja de fuego */
enum {
	FIRE_NINJA_IDLE,
	FIRE_NINJA_ATTACK,
	FIRE_NINJA_CELEBRATE_INTRO,
	FIRE_NINJA_CELEBRATE,
	FIRE_NINJA_MOVE,
	FIRE_NINJA_GHOST,
	FIRE_NINJA_HIT,
	FIRE_NINJA_KO_INTRO,
	FIRE_NINJA_KO_LOOP,
	FIRE_NINJA_POWERCARD_BOTTLEDROP,
	FIRE_NINJA_POWERCARD_SUMMON,
	FIRE_NINJA_POWERCARD_FIRE,
	FIRE_NINJA_REVIVED,
	FIRE_NINJA_REVIVE_OTHER,
	FIRE_NINJA_REVIVE_OTHER_LOOP,
	
	NUM_FIRE_NINJA_IMAGES
};

static const char *fire_ninja_images_names[NUM_FIRE_NINJA_IMAGES] = {
	"images/fire_ninja/idle.png",
	"images/fire_ninja/attack.png",
	"images/fire_ninja/celebrate_intro.png",
	"images/fire_ninja/celebrate.png",
	"images/fire_ninja/move.png",
	"images/fire_ninja/ghost.png",
	"images/fire_ninja/hit.png",
	"images/fire_ninja/knockout_intro.png",
	"images/fire_ninja/knockout_loop.png",
	"images/fire_ninja/powercard_bottledrop.png",
	"images/fire_ninja/powercard_summon.png",
	"images/fire_ninja/powercard_fire_loop.png",
	"images/fire_ninja/revived.png",
	"images/fire_ninja/revive_other.png",
	"images/fire_ninja/revive_other_loop.png",
};

static SnowSprite sprite_idle[] = {
	{72,138,66,50,5,25,TRUE},
	{72,138,66,50,5,25,TRUE},
	{72,70,66,50,6,25,TRUE},
	{72,70,66,50,6,25,TRUE},
	{2,102,68,48,6,27,FALSE},
	{2,102,68,48,6,27,FALSE},
	{2,52,68,48,6,27,FALSE},
	{2,52,68,48,6,27,FALSE},
	{2,2,68,48,6,27,FALSE},
	{2,2,68,48,6,27,FALSE},
	{72,2,66,50,6,25,TRUE},
	{72,2,66,50,6,25,TRUE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_attack[] = {
	{2,340,65,48,36,55,FALSE},
	{272,295,67,48,36,55,FALSE},
	{196,256,69,74,36,28,TRUE},
	{298,90,69,82,35,20,TRUE},
	{382,160,69,80,36,22,TRUE},
	{196,185,69,76,35,26,TRUE},
	{196,256,69,74,36,28,TRUE},
	{298,90,69,82,35,20,TRUE},
	{274,228,65,76,35,26,TRUE},
	{352,231,65,72,35,30,TRUE},
	{97,266,65,72,35,30,TRUE},
	{298,161,65,78,35,24,TRUE},
	{274,228,65,76,35,26,TRUE},
	{352,231,65,72,35,30,TRUE},
	{2,78,99,82,1,20,FALSE},
	{305,2,97,86,3,16,FALSE},
	{2,162,97,86,3,16,FALSE},
	{105,2,99,82,1,20,FALSE},
	{103,86,97,86,3,16,FALSE},
	{206,2,97,86,3,16,FALSE},
	{2,78,99,82,1,20,FALSE},
	{305,2,97,86,3,16,FALSE},
	{2,2,101,74,11,28,FALSE},
	{404,2,97,74,24,35,FALSE},
	{404,78,95,80,30,25,FALSE},
	{2,250,93,88,35,14,FALSE},
	{202,90,93,94,35,8,TRUE},
	{101,174,93,90,35,12,FALSE},
	{97,333,65,52,35,51,FALSE},
	{171,327,65,60,36,43,FALSE},
	{426,231,65,66,36,37,TRUE},
	{2,340,65,48,36,55,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_celebrate_intro[] = {
	{76,204,66,68,6,34,TRUE},
	{2,142,66,72,6,29,TRUE},
	{152,142,64,72,6,29,TRUE},
	{106,392,64,50,6,51,TRUE},
	{2,342,64,52,6,49,FALSE},
	{54,396,64,50,6,51,TRUE},
	{88,68,64,76,6,25,TRUE},
	{78,134,68,72,2,28,TRUE},
	{88,2,64,78,6,22,TRUE},
	{2,210,64,68,6,32,TRUE},
	{2,396,64,50,6,50,TRUE},
	{70,338,64,52,6,48,FALSE},
	{166,70,70,72,0,28,TRUE},
	{2,74,66,74,5,27,TRUE},
	{140,274,64,64,5,37,FALSE},
	{146,208,66,64,3,37,FALSE},
	{2,276,64,66,5,35,TRUE},
	{72,272,64,66,5,35,TRUE},
	{168,2,66,76,5,25,TRUE},
	{2,2,70,84,9,16,TRUE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_celebrate[] = {
	{152,200,64,72,15,28,TRUE},
	{84,134,64,80,15,20,TRUE},
	{88,68,64,82,15,18,TRUE},
	{2,68,64,84,15,16,TRUE},
	{2,2,64,88,15,12,TRUE},
	{92,2,64,84,15,16,TRUE},
	{2,200,64,74,15,26,TRUE},
	{2,134,64,80,15,20,TRUE},
	{78,200,64,72,15,28,TRUE},
	{178,2,64,76,15,24,TRUE},
	{172,68,64,80,15,20,TRUE},
	{166,134,64,78,15,22,TRUE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_move[] = {
	{142,2,64,92,23,19,FALSE},
	{348,2,62,86,25,25,FALSE},
	{288,2,58,88,25,23,FALSE},
	{74,2,66,92,19,19,FALSE},
	{208,2,78,90,12,21,FALSE},
	{412,2,78,94,13,17,TRUE},
	{2,2,70,92,19,19,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_ghost[] = {
	{0,0,69,70,0,0,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_hit[] = {
	{196,79,59,51,33,65,TRUE},
	{2,223,65,61,34,59,FALSE},
	{2,73,95,83,25,44,FALSE},
	{99,79,73,95,33,35,TRUE},
	{111,2,75,97,34,25,TRUE},
	{2,2,69,107,34,15,TRUE},
	{134,260,51,51,34,65,FALSE},
	{196,140,51,51,34,65,FALSE},
	{196,140,51,51,34,65,FALSE},
	{196,140,51,51,34,65,FALSE},
	{196,140,51,51,34,65,FALSE},
	{196,140,51,51,34,65,FALSE},
	{69,274,51,49,34,67,FALSE},
	{176,193,73,65,18,51,FALSE},
	{99,154,69,75,28,41,TRUE},
	{2,158,63,73,34,43,TRUE},
	{69,225,63,47,34,69,FALSE},
	{69,225,63,47,34,69,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_ko_intro[] = {
	{75,296,65,48,33,26,FALSE},
	{179,82,75,58,20,19,FALSE},
	{2,2,93,80,23,4,FALSE},
	{186,2,67,78,35,4,FALSE},
	{97,56,69,80,34,3,TRUE},
	{2,151,65,80,28,3,TRUE},
	{84,127,65,80,28,3,TRUE},
	{2,84,65,80,28,3,TRUE},
	{166,186,59,76,28,7,TRUE},
	{84,194,73,46,27,37,FALSE},
	{166,142,79,42,26,41,FALSE},
	{97,2,87,52,23,31,FALSE},
	{75,242,71,52,28,31,FALSE},
	{2,218,71,52,28,31,FALSE},
	{2,272,71,50,28,33,FALSE},
	{148,247,71,48,28,35,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_ko_loop[] = {
	{902,2,73,48,27,35,FALSE},
	{1127,2,73,46,27,37,FALSE},
	{1052,2,73,46,27,37,FALSE},
	{977,2,73,46,27,37,FALSE},
	{827,2,73,48,27,35,FALSE},
	{752,2,73,48,27,35,FALSE},
	{602,2,73,50,27,33,FALSE},
	{452,2,73,52,27,31,FALSE},
	{377,2,73,52,27,31,FALSE},
	{302,2,73,52,27,31,FALSE},
	{227,2,73,52,27,31,FALSE},
	{152,2,73,52,27,31,FALSE},
	{77,2,73,52,27,31,FALSE},
	{2,2,73,52,27,31,FALSE},
	{527,2,73,50,27,33,FALSE},
	{677,2,73,48,27,35,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_revived[] = {
	{78,308,72,40,28,126,TRUE},
	{148,74,60,98,34,60,TRUE},
	{148,2,70,104,25,54,TRUE},
	{2,168,70,130,26,28,TRUE},
	{2,84,82,140,15,18,TRUE},
	{2,2,80,144,18,14,TRUE},
	{134,218,84,124,14,34,FALSE},
	{144,136,80,108,18,50,TRUE},
	{2,308,74,74,24,84,FALSE},
	{2,240,66,78,32,80,TRUE},
	{2,384,66,72,33,86,FALSE},
	{82,240,66,48,33,110,TRUE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

SnowSprite *fire_animations[NUM_FIRE_NINJA_IMAGES] = {
	sprite_idle,
	sprite_attack,
	sprite_celebrate_intro,
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

static float fire_ninja_offsets[NUM_FIRE_NINJA_IMAGES][4] = {
	{0.5, 1.0, 84.0, 84.0},
	{0.51798, 1.0089, 139.0, 112.0},
	{0.5, 1.0, 84.0, 112.0}, // 0.4375,
	{0.5, 1.0, 84.0, 112.0}, // , 0.9375
	{0.625, 1.09375, 112.0, 112.0},
	{0.65625, 1.15625, 69.0, 70.0},
	{0.5609, 0.8992, 123.0, 139.0},
	{0.50359, 0.94318, 139.0, 88.0},
	{0.50359, 1.0, 139.0, 84.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.625, 1.006, 112.0, 166.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0}
};

static int fire_ninja_offsets_int[NUM_FIRE_NINJA_IMAGES][2];

struct _FireNinja {
	int frame;
	int x, y;
	int next_x, next_y;
	int estado;
	int x_real, y_real;
	int next_x_real, next_y_real;
	int sum_x, sum_y;
};

static SDL_Texture *fire_ninja_images[NUM_FIRE_NINJA_IMAGES];

FireNinja *create_fire_ninja (int x, int y) {
	FireNinja *obj;
	
	obj = (FireNinja *) malloc (sizeof (FireNinja));
	
	obj->x = x;
	obj->y = y;
	obj->x_real = MAP_X + (x * 70) + 35;
	obj->y_real = MAP_Y + (y * 70) + 70;
	
	obj->next_x = obj->x;
	obj->next_y = obj->y;
	obj->estado = FIRE_NINJA_IDLE;
	obj->frame = 0;
	
	return obj;
}

void ask_fire_coords (FireNinja *ninja, int *x, int *y) {
	if (x != NULL) {
		*x = ninja->x;
	}
	
	if (y != NULL) {
		*y = ninja->y;
	}
}

int is_fire_done (FireNinja *ninja) {
	if (ninja->estado == FIRE_NINJA_IDLE) return TRUE;
	
	return FALSE;
}

void put_idle_fire (FireNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = FIRE_NINJA_IDLE;
}

void attack_fire (FireNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = FIRE_NINJA_ATTACK;
}

void celebrate_fire (FireNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = FIRE_NINJA_CELEBRATE_INTRO;
}

void move_fire (FireNinja *ninja, int x, int y) {
	ninja->frame = 0;
	ninja->estado = FIRE_NINJA_MOVE;
	
	ninja->x = ninja->next_x = x;
	ninja->y = ninja->next_y = y;
	
	/* Calcular las siguientes coordenadas reales */
	ninja->next_x_real = MAP_X + (x * 70) + 35;
	ninja->next_y_real = MAP_Y + (y * 70) + 70;
	
	ninja->sum_x = (ninja->next_x_real - ninja->x_real) / 35;
	ninja->sum_y = (ninja->next_y_real - ninja->y_real) / 35;
}

void ghost_move_fire (FireNinja *ninja, int x, int y) {
	ninja->next_x = x;
	ninja->next_y = y;
}

void ko_fire (FireNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = FIRE_NINJA_KO_INTRO;
}

void hit_fire (FireNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = FIRE_NINJA_HIT;
}

void revive_fire (FireNinja *ninja) {
	ninja->frame = 0;
	ninja->estado = FIRE_NINJA_REVIVED;
}

void draw_fire_ninja (FireNinja *ninja) {
	SDL_Rect rect, rect2, rect3;
	int calc;
	int temp;
	int est;
	int sx, sy;
	
	est = ninja->estado;
	
	calc = ninja->frame / 2;
	
	rect2.x = fire_animations[est][calc].orig_x;
	rect2.y = fire_animations[est][calc].orig_y;
	rect.w = rect2.w = fire_animations[est][calc].w;
	rect.h = rect2.h = fire_animations[est][calc].h;
	
	if (ninja->estado == FIRE_NINJA_MOVE) {
		ninja->x_real += ninja->sum_x;
		ninja->y_real += ninja->sum_y;
	}
	
	rect.x = ninja->x_real - fire_ninja_offsets_int[est][0] + fire_animations[est][calc].dest_x;
	rect.y = ninja->y_real - fire_ninja_offsets_int[est][1] + fire_animations[est][calc].dest_y;
	
	if (fire_animations[est][calc].rot) {
		rect2.w = rect.h;
		rect2.h = rect.w;
		
		rect.h = rect2.h;
		rect.w = rect2.w;
		SDL_Point p;
		p.y = 0;
		p.x = rect.w;
		rect.x = ninja->x_real - fire_ninja_offsets_int[est][0] - rect.w + fire_animations[est][calc].dest_x;
		
		SDL_RenderCopyEx (renderer, fire_ninja_images[est], &rect2, &rect, 270.0, &p, SDL_FLIP_NONE);
	} else {
		SDL_RenderCopy (renderer, fire_ninja_images[est], &rect2, &rect);
	}
	
	ninja->frame++;
	calc = ninja->frame / 2;
	if (fire_animations[est][calc].w == -1 && fire_animations[est][calc].h == -1) {
		ninja->frame = 0;
		if (ninja->estado == FIRE_NINJA_ATTACK) {
			ninja->estado = FIRE_NINJA_IDLE;
		} else if (ninja->estado == FIRE_NINJA_CELEBRATE_INTRO) {
			ninja->estado = FIRE_NINJA_CELEBRATE;
		} else if (ninja->estado == FIRE_NINJA_KO_INTRO) {
			ninja->estado = FIRE_NINJA_KO_LOOP;
		} else if (ninja->estado == FIRE_NINJA_HIT) {
			ninja->estado = FIRE_NINJA_IDLE;
		} else if (ninja->estado == FIRE_NINJA_REVIVED) {
			ninja->estado = FIRE_NINJA_IDLE;
		} else if (ninja->estado == FIRE_NINJA_MOVE) {
			ninja->estado = FIRE_NINJA_MOVE;
		}
	}
	
	if (ninja->estado == FIRE_NINJA_MOVE) {
		if (ninja->x_real == ninja->next_x_real && ninja->y_real == ninja->next_y_real) {
			/* Llegamos al destino */
			ninja->estado = FIRE_NINJA_IDLE;
			ninja->frame = 0;
		}
	}
}

void draw_ghost_fire_ninja (FireNinja *ninja) {
	SDL_Rect rect;
	if (ninja->x != ninja->next_x || ninja->y != ninja->next_y) {
		/* Dibujar el ninja fantasma */
		rect.x = MAP_X + (ninja->next_x * 70) + 35 - fire_ninja_offsets_int[FIRE_NINJA_GHOST][0] + fire_animations[FIRE_NINJA_GHOST][0].dest_x;
		rect.y = MAP_Y + (ninja->next_y * 70) + 70 - fire_ninja_offsets_int[FIRE_NINJA_GHOST][1] + fire_animations[FIRE_NINJA_GHOST][0].dest_y;
		
		SDL_QueryTexture (fire_ninja_images[FIRE_NINJA_GHOST], NULL, NULL, &rect.w, &rect.h);
		SDL_RenderCopy (renderer, fire_ninja_images[FIRE_NINJA_GHOST], NULL, &rect);
	}
}

void ask_fire_actions (FireNinja *ninja, int escenario[5][9], int acciones[5][9]) {
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

void setup_fire_ninja (void) {
	SDL_Surface * image;
	SDL_Texture * texture;
	int g;
	
	char buffer_file[8192];
	
	for (g = 0; g < NUM_FIRE_NINJA_IMAGES; g++) {
		sprintf (buffer_file, "%s%s", systemdata_path, fire_ninja_images_names[g]);
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
		fire_ninja_images[g] = texture;
		SDL_FreeSurface (image);
		/* TODO: Mostrar la carga de porcentaje */
		
		/* Calcular los centros */
		fire_ninja_offsets_int[g][0] = (int) roundf (fire_ninja_offsets[g][0] * fire_ninja_offsets[g][2]);
		fire_ninja_offsets_int[g][1] = (int) roundf (fire_ninja_offsets[g][1] * fire_ninja_offsets[g][3]);
	}
}
