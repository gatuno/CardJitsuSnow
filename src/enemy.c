/*
 * enemy.c
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

#include "enemy.h"

/* Listar las imágenes de los enemigos */
enum {
	SNOWMAN_SPAWN,
	
	SLY_IDLE,
	SLY_ATTACK,
	SLY_DAZE,
	SLY_HIT,
	SLY_MOVE,
	SLY_KO,
	
	NUM_ENEMY_IMAGES
};

static const char *enemy_images_names[NUM_ENEMY_IMAGES] = {
	"images/enemy/snowman_spawn.png",
	
	"images/enemy/sly_idle.png",
	"images/enemy/sly_attack.png",
	"images/enemy/sly_daze.png",
	"images/enemy/sly_hit.png",
	"images/enemy/sly_move.png",
	"images/enemy/sly_ko.png"
};

static SnowSprite sprite_spawn[] = {
	{189,79,63,39,57,73,FALSE},
	{189,120,59,37,59,75,FALSE},
	{172,266,57,35,59,74,FALSE},
	{74,458,49,39,63,68,TRUE},
	{115,458,41,55,68,50,TRUE},
	{74,385,45,71,65,31,FALSE},
	{189,2,63,75,53,27,FALSE},
	{2,266,117,105,29,19,TRUE},
	{2,135,155,129,11,3,FALSE},
	{2,2,185,131,0,0,FALSE},
	{159,159,105,83,39,27,TRUE},
	{37,385,105,35,42,29,TRUE},
	{2,385,109,33,40,25,TRUE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_sly_idle[] = {
	{2,63,57,60,0,0,TRUE},
	{2,63,57,60,0,0,TRUE},
	{2,63,57,60,0,0,TRUE},
	{2,63,57,60,0,0,TRUE},
	{2,63,57,60,0,0,TRUE},
	{2,63,57,60,0,0,TRUE},
	{2,63,57,60,0,0,TRUE},
	{2,63,57,60,0,0,TRUE},
	{2,63,57,60,0,0,TRUE},
	{2,63,57,60,0,0,TRUE},
	{2,122,55,60,2,0,TRUE},
	{2,179,55,58,2,1,TRUE},
	{64,61,57,60,1,0,TRUE},
	{2,2,59,60,0,0,TRUE},
	{2,2,59,60,0,0,TRUE},
	{2,2,59,60,0,0,TRUE},
	{2,2,59,60,0,0,TRUE},
	{2,2,59,60,0,0,TRUE},
	{2,2,59,60,0,0,TRUE},
	{2,2,59,60,0,0,TRUE},
	{2,2,59,60,0,0,TRUE},
	{2,2,59,60,0,0,TRUE},
	{64,2,57,60,1,0,TRUE},
	{64,177,55,58,2,1,TRUE},
	{64,120,55,60,2,0,TRUE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_sly_attack[] = {
	{170,396,54,58,43,40,TRUE},
	{118,348,50,60,47,38,FALSE},
	{118,348,50,60,47,38,FALSE},
	{64,356,52,60,48,38,FALSE},
	{64,356,52,60,48,38,FALSE},
	{110,418,50,60,53,38,FALSE},
	{110,418,50,60,53,38,FALSE},
	{56,420,52,60,54,38,FALSE},
	{56,420,52,60,54,38,FALSE},
	{2,356,60,62,46,36,FALSE},
	{186,286,60,62,46,36,TRUE},
	{186,286,60,62,46,36,TRUE},
	{186,286,60,62,46,36,TRUE},
	{186,286,60,62,46,36,TRUE},
	{186,286,60,62,46,36,TRUE},
	{186,286,60,62,46,36,TRUE},
	{186,286,60,62,46,36,TRUE},
	{186,286,60,62,46,36,TRUE},
	{142,2,92,96,55,2,TRUE},
	{100,96,74,96,60,2,TRUE},
	{2,92,74,96,60,2,TRUE},
	{2,92,74,96,60,2,TRUE},
	{2,92,74,96,60,2,TRUE},
	{2,2,138,88,13,12,FALSE},
	{2,168,90,66,17,32,FALSE},
	{2,236,90,58,8,40,FALSE},
	{94,232,90,58,3,40,FALSE},
	{94,172,90,58,2,40,FALSE},
	{94,292,82,54,10,44,FALSE},
	{94,292,82,54,10,44,FALSE},
	{94,292,82,54,10,44,FALSE},
	{94,292,82,54,10,44,FALSE},
	{94,292,82,54,10,44,FALSE},
	{94,292,82,54,10,44,FALSE},
	{2,296,70,58,22,40,FALSE},
	{186,222,64,62,28,36,FALSE},
	{186,222,64,62,28,36,FALSE},
	{162,452,54,56,40,42,FALSE},
	{162,452,54,56,40,42,FALSE},
	{162,452,54,56,40,42,FALSE},
	{162,452,54,56,40,42,FALSE},
	{162,452,54,56,40,42,FALSE},
	{198,160,56,60,38,38,FALSE},
	{198,160,56,60,38,38,FALSE},
	{198,160,56,60,38,38,FALSE},
	{198,160,56,60,38,38,FALSE},
	{2,420,52,60,41,41,FALSE},
	{170,348,46,60,47,44,TRUE},
	{170,348,46,60,47,44,TRUE},
	{198,96,56,62,43,39,FALSE},
	{170,396,54,58,43,40,TRUE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_sly_daze[] = {
	{81,202,62,77,5,5,TRUE},
	{2,206,64,75,4,7,TRUE},
	{162,194,64,77,3,5,TRUE},
	{2,140,64,77,3,5,TRUE},
	{2,70,68,79,1,3,TRUE},
	{83,136,64,77,6,5,TRUE},
	{83,72,62,79,5,3,TRUE},
	{166,68,64,79,4,3,TRUE},
	{164,134,58,79,9,3,TRUE},
	{2,2,66,81,3,1,TRUE},
	{85,2,68,79,2,3,TRUE},
	{166,2,64,79,3,3,TRUE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_sly_move[] = {
	{2,126,54,64,11,0,TRUE},
	{68,136,52,58,12,6,TRUE},
	{68,136,52,58,12,6,TRUE},
	{66,190,56,62,9,2,FALSE},
	{68,70,58,64,8,0,FALSE},
	{68,70,58,64,8,0,FALSE},
	{2,2,66,62,0,1,FALSE},
	{70,2,66,56,0,7,TRUE},
	{70,2,66,56,0,7,TRUE},
	{2,182,56,62,9,2,TRUE},
	{2,66,58,64,8,0,TRUE},
	{2,66,58,64,8,0,TRUE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_sly_hit[] = {
	{87,217,57,60,27,35,TRUE},
	{2,84,85,70,19,25,FALSE},
	{2,2,105,80,0,22,FALSE},
	{109,2,95,70,14,25,FALSE},
	{109,74,93,80,12,19,FALSE},
	{2,156,83,76,22,19,FALSE},
	{87,156,59,68,28,27,TRUE},
	{206,2,47,68,38,27,FALSE},
	{157,156,67,58,17,37,FALSE},
	{2,234,59,58,25,37,FALSE},
	{157,216,57,60,27,35,TRUE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_sly_ko[] = {
	{362,677,55,61,66,44,FALSE},
	{429,462,79,75,67,30,FALSE},
	{2,654,107,87,47,20,FALSE},
	{259,625,101,101,61,15,FALSE},
	{119,613,107,107,58,14,FALSE},
	{266,514,115,109,53,16,FALSE},
	{2,541,115,111,46,19,FALSE},
	{424,675,89,85,40,38,TRUE},
	{259,728,85,87,39,37,TRUE},
	{426,539,83,67,38,38,FALSE},
	{426,608,77,65,44,40,FALSE},
	{411,815,47,61,74,44,TRUE},
	{348,803,47,61,74,44,TRUE},
	{348,803,47,61,74,44,TRUE},
	{411,815,47,61,74,44,TRUE},
	{417,766,47,61,74,44,TRUE},
	{348,740,67,61,55,44,FALSE},
	{137,496,127,115,30,12,FALSE},
	{153,290,141,127,22,6,FALSE},
	{324,143,147,133,19,3,FALSE},
	{324,278,143,103,28,25,FALSE},
	{296,383,131,129,36,5,FALSE},
	{2,248,149,149,29,3,FALSE},
	{169,143,153,145,25,7,FALSE},
	{338,2,161,139,21,11,FALSE},
	{171,2,165,139,18,11,FALSE},
	{2,2,167,131,17,19,FALSE},
	{2,135,165,111,19,29,FALSE},
	{2,399,133,99,24,42,FALSE},
	{137,419,131,75,26,52,FALSE},
	{469,341,119,41,27,85,TRUE},
	{2,500,123,39,27,88,FALSE},
	{473,143,109,37,42,87,TRUE},
	{228,613,85,29,70,82,TRUE},
	{473,254,85,37,71,75,TRUE},
	{383,569,49,41,76,70,TRUE},
	{383,514,53,41,75,69,TRUE},
	{111,722,51,39,75,70,FALSE},
	{383,620,55,39,74,68,TRUE},
	{429,383,61,33,71,67,TRUE},
	{228,700,63,29,70,67,TRUE},
	{501,2,1,1,0,0,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

SnowSprite *enemy_sly_animations[6] = {
	sprite_sly_idle,
	sprite_sly_attack,
	sprite_sly_daze,
	sprite_sly_hit,
	sprite_sly_move,
	sprite_sly_ko
};

static float enemy_offsets[NUM_ENEMY_IMAGES][4] = {
	{0.46875, 0.90625, 185.0, 133.0},
	
	{0.53125, 1.0625, 63.0, 60.0},
	{0.46875, 1.0, 152.0, 106.0},
	{0.625, 1.0625, 74.0, 83.0},
	{0.53125, 0.875, 115.0, 112.0},
	{0.53125, 1.03125, 68.0, 64.0},
	{0.5, 0.65625, 195.0, 167.0}
};

typedef struct _Enemy {
	int frame;
	int x, y;
	int tipo;
	int estado;
	int x_real, y_real;
	int vida;
	int max_life;
	
	int ref; /* Contador de referencias para saber si un enemigo se murió */
};

static int enemy_offsets_int[NUM_ENEMY_IMAGES][2];

static SDL_Texture *enemy_images[NUM_ENEMY_IMAGES];

Enemy *create_enemy (int x, int y, int tipo) {
	Enemy *obj;
	
	obj = (Enemy *) malloc (sizeof (Enemy));
	
	obj->x = x;
	obj->y = y;
	obj->x_real = MAP_X + (x * 70) + 35;
	obj->y_real = MAP_Y + (y * 70) + 70;
	
	obj->tipo = tipo;
	obj->estado = SNOWMAN_SPAWN;
	obj->frame = 0;
	
	if (obj->tipo == ENEMY_SLY) {
		obj->vida = obj->max_life = 30;
	}
	
	obj->ref = 0;
	return obj;
}

void draw_enemy (Enemy *enemy) {
	SDL_Rect rect, rect2;
	int calc;
	int temp;
	int est;
	SnowSprite *animation;
	
	est = enemy->estado;
	calc = enemy->frame / 2;
	
	if (est == SNOWMAN_SPAWN) {
		animation = sprite_spawn;
	} else if (enemy->tipo == ENEMY_SLY) {
		animation = enemy_sly_animations[est - SLY_IDLE];
	}
	
	rect2.x = animation[calc].orig_x;
	rect2.y = animation[calc].orig_y;
	rect.w = rect2.w = animation[calc].w;
	rect.h = rect2.h = animation[calc].h;
	
	rect.x = enemy->x_real - enemy_offsets_int[est][0] + animation[calc].dest_x;
	rect.y = enemy->y_real - enemy_offsets_int[est][1] + animation[calc].dest_y;
	
	if (animation[calc].rot) {
		rect2.w = rect.h;
		rect2.h = rect.w;
		
		rect.h = rect2.h;
		rect.w = rect2.w;
		SDL_Point p;
		p.y = 0;
		p.x = rect.w;
		rect.x -= rect.w;
		
		SDL_RenderCopyEx (renderer, enemy_images[est], &rect2, &rect, 270.0, &p, SDL_FLIP_NONE);
	} else {
		SDL_RenderCopy (renderer, enemy_images[est], &rect2, &rect);
	}
	
	if (est != SNOWMAN_SPAWN) {
		/* Dibujar la barra de vida */
		temp = 59 - (59 * enemy->vida) / enemy->max_life;
		
		rect2.x = shared_sprites[SHARED_IMG_HEALTHBAR][temp].orig_x;
		rect2.y = shared_sprites[SHARED_IMG_HEALTHBAR][temp].orig_y;
		rect.w = rect2.w = shared_sprites[SHARED_IMG_HEALTHBAR][temp].w;
		rect.h = rect2.h = shared_sprites[SHARED_IMG_HEALTHBAR][temp].h;
		
		rect.x = enemy->x_real + shared_sprites[SHARED_IMG_HEALTHBAR][temp].dest_x - 29;
		rect.y = enemy->y_real + shared_sprites[SHARED_IMG_HEALTHBAR][temp].dest_y - 8;
		
		if (shared_sprites[SHARED_IMG_HEALTHBAR][temp].rot) {
			rect2.w = rect.h;
			rect2.h = rect.w;
		
			rect.h = rect2.h;
			rect.w = rect2.w;
			SDL_Point p;
			p.y = 0;
			p.x = rect.w;
			rect.x -= rect.w;
		
			SDL_RenderCopyEx (renderer, shared_images[SHARED_IMG_HEALTHBAR], &rect2, &rect, 270.0, &p, SDL_FLIP_NONE);
		} else {
			SDL_RenderCopy (renderer, shared_images[SHARED_IMG_HEALTHBAR], &rect2, &rect);
		}
	}
	
	enemy->frame++;
	calc = enemy->frame / 2;
	if (animation[calc].w == -1 && animation[calc].h == -1) {
		enemy->frame = 0;
		if (est == SNOWMAN_SPAWN) {
			if (enemy->tipo == ENEMY_SLY) {
				enemy->estado = SLY_IDLE;
			} /* TODO: Los otros enemigos aquí */
		} else if (est == SLY_HIT) {
			enemy->estado = SLY_IDLE;
		}
	}
}

int is_enemy_ready (Enemy *enemy) {
	if (enemy->estado == SLY_IDLE /* || TODO: Los otros */) {
		return TRUE;
	}
	
	return FALSE;
}

void add_enemy_ref (Enemy *enemy) {
	enemy->ref++;
}

void enemy_hit (Enemy *enemy, int damage) {
	enemy->vida = enemy->vida - damage;
	
	if (enemy->vida < 0) {
		enemy->vida = 0;
	}
	
	enemy->ref--;
	
	if (enemy->ref == 0 && enemy->vida == 0) {
		enemy->estado = SLY_KO;
	} else {
		enemy->estado = SLY_HIT;
	}
	
	enemy->frame = 0;
}

void setup_enemy (void) {
	SDL_Surface * image;
	SDL_Texture * texture;
	int g;
	
	char buffer_file[8192];
	
	for (g = 0; g < NUM_ENEMY_IMAGES; g++) {
		sprintf (buffer_file, "%s%s", systemdata_path, enemy_images_names[g]);
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
		enemy_images[g] = texture;
		SDL_FreeSurface (image);
		/* TODO: Mostrar la carga de porcentaje */
		
		/* Calcular los centros */
		enemy_offsets_int[g][0] = (int) roundf (enemy_offsets[g][0] * enemy_offsets[g][2]);
		enemy_offsets_int[g][1] = (int) roundf (enemy_offsets[g][1] * enemy_offsets[g][3]);
	}
}
