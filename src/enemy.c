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
	
	SCRAP_IDLE,
	SCRAP_ATTACK,
	SCRAP_DAZE,
	SCRAP_HIT,
	SCRAP_MOVE,
	SCRAP_KO,
	
	TANK_IDLE,
	TANK_ATTACK,
	TANK_DAZE,
	TANK_HIT,
	TANK_MOVE,
	TANK_KO,
	
	NUM_ENEMY_IMAGES
};

static const char *enemy_images_names[NUM_ENEMY_IMAGES] = {
	"images/enemy/snowman_spawn.png",
	
	"images/enemy/sly_idle.png",
	"images/enemy/sly_attack.png",
	"images/enemy/sly_daze.png", // Falta ajuste
	"images/enemy/sly_hit.png",
	"images/enemy/sly_move.png",
	"images/enemy/sly_ko.png",
	
	"images/enemy/scrap_idle.png",
	"images/enemy/scrap_attack.png",
	"images/enemy/scrap_daze.png", // Falta ajuste
	"images/enemy/scrap_hit.png",
	"images/enemy/scrap_move.png",
	"images/enemy/scrap_ko.png",
	
	"images/enemy/tank_idle.png",
	"images/enemy/tank_attack.png",
	"images/enemy/tank_daze.png",
	"images/enemy/tank_hit.png",
	"images/enemy/tank_move.png",
	"images/enemy/tank_ko.png"
};

/* Los estados de los enemigos */
enum {
	ENEMY_SPAWN,
	
	ENEMY_IDLE,
	ENEMY_ATTACK,
	ENEMY_DAZE,
	ENEMY_HIT,
	ENEMY_MOVE,
	ENEMY_KO,
	
	NUM_ENEMY_STATES
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

static SnowSprite sprite_scrap_idle[] = {
	{80,284,76,69,22,22,FALSE},
	{236,558,76,67,22,24,FALSE},
	{158,493,76,67,21,24,FALSE},
	{80,493,76,67,21,24,FALSE},
	{2,493,76,67,21,24,FALSE},
	{392,556,76,67,20,24,FALSE},
	{2,284,76,69,20,22,FALSE},
	{314,556,76,67,21,24,FALSE},
	{236,489,76,67,21,24,FALSE},
	{158,215,76,69,22,22,FALSE},
	{158,424,76,67,22,24,FALSE},
	{80,424,76,67,22,24,FALSE},
	{240,211,76,69,22,22,FALSE},
	{2,424,76,67,21,24,FALSE},
	{392,487,76,67,21,24,FALSE},
	{314,487,76,67,21,24,FALSE},
	{236,420,76,67,21,24,FALSE},
	{158,355,76,67,21,24,FALSE},
	{80,213,76,69,22,22,FALSE},
	{80,355,76,67,22,24,FALSE},
	{2,355,76,67,21,24,FALSE},
	{392,418,76,67,21,24,FALSE},
	{314,418,76,67,21,24,FALSE},
	{392,349,76,67,21,24,FALSE},
	{162,73,78,69,20,22,FALSE},
	{236,351,76,67,22,24,FALSE},
	{314,349,76,67,22,24,FALSE},
	{2,213,76,69,22,22,FALSE},
	{396,280,76,67,22,24,FALSE},
	{158,286,76,67,22,24,FALSE},
	{82,73,78,69,20,22,FALSE},
	{236,282,76,67,21,24,FALSE},
	{318,280,76,67,21,24,FALSE},
	{396,211,76,67,21,24,FALSE},
	{82,144,78,67,19,24,FALSE},
	{2,144,78,67,19,24,FALSE},
	{2,73,78,69,20,22,FALSE},
	{402,142,78,67,18,24,FALSE},
	{390,625,74,67,20,24,FALSE},
	{314,625,74,67,19,24,FALSE},
	{318,211,76,67,17,24,FALSE},
	{322,142,78,67,17,24,FALSE},
	{242,142,78,67,18,24,FALSE},
	{402,73,78,67,18,24,FALSE},
	{322,73,78,67,19,24,FALSE},
	{242,73,78,67,19,24,FALSE},
	{330,2,80,69,17,22,FALSE},
	{248,2,80,69,17,22,FALSE},
	{166,2,80,69,17,22,FALSE},
	{84,2,80,69,17,22,FALSE},
	{2,2,80,69,17,22,FALSE},
	{412,2,78,69,19,22,FALSE},
	{162,144,76,69,22,22,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_scrap_attack[] = {
	{2,644,76,68,78,124,FALSE},
	{2,714,74,68,78,124,FALSE},
	{306,632,64,68,77,124,FALSE},
	{446,726,52,68,76,124,FALSE},
	{450,656,52,68,76,124,FALSE},
	{132,692,70,68,58,124,FALSE},
	{312,422,60,68,68,124,FALSE},
	{316,352,54,68,74,124,FALSE},
	{276,772,48,70,80,122,TRUE},
	{306,492,66,72,90,120,FALSE},
	{374,434,90,96,91,96,TRUE},
	{374,342,90,96,91,96,TRUE},
	{374,250,90,118,91,74,TRUE},
	{374,158,90,136,91,56,TRUE},
	{2,552,90,144,91,48,TRUE},
	{2,460,90,152,91,40,TRUE},
	{158,384,90,152,91,40,TRUE},
	{2,368,90,154,90,38,TRUE},
	{2,264,102,156,91,36,TRUE},
	{160,292,90,154,91,38,TRUE},
	{156,476,82,148,91,44,TRUE},
	{380,2,120,154,46,38,FALSE},
	{2,154,108,166,29,26,TRUE},
	{196,2,122,182,22,10,TRUE},
	{2,2,150,192,7,0,TRUE},
	{196,126,164,176,0,16,TRUE},
	{2,836,70,50,58,142,FALSE},
	{74,790,70,50,58,142,TRUE},
	{130,762,70,50,58,142,FALSE},
	{204,734,70,50,58,142,FALSE},
	{2,784,70,50,58,142,FALSE},
	{78,718,70,50,58,142,TRUE},
	{78,718,70,50,58,142,TRUE},
	{78,718,70,50,58,142,TRUE},
	{78,718,70,50,58,142,TRUE},
	{78,718,70,50,58,142,TRUE},
	{78,718,70,50,58,142,TRUE},
	{78,718,70,50,58,142,TRUE},
	{80,644,72,50,56,142,TRUE},
	{316,292,56,58,72,134,FALSE},
	{240,652,80,62,49,130,TRUE},
	{374,590,90,64,39,128,FALSE},
	{148,626,90,64,39,128,FALSE},
	{240,560,90,64,39,128,TRUE},
	{148,560,90,64,39,128,FALSE},
	{148,560,90,64,39,128,FALSE},
	{148,560,90,64,39,128,FALSE},
	{148,560,90,64,39,128,FALSE},
	{148,560,90,64,39,128,FALSE},
	{148,560,90,64,39,128,FALSE},
	{148,560,90,64,39,128,FALSE},
	{148,560,90,64,39,128,FALSE},
	{148,560,90,64,39,128,FALSE},
	{374,526,92,62,37,130,FALSE},
	{306,566,66,64,62,128,FALSE},
	{304,702,62,68,78,124,FALSE},
	{368,726,76,68,78,124,FALSE},
	{372,656,76,68,78,124,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_scrap_daze[] = {
	{2,276,67,85,17,32,TRUE},
	{91,213,67,85,16,33,TRUE},
	{184,91,67,87,15,32,FALSE},
	{184,2,67,87,14,33,FALSE},
	{93,2,67,89,14,32,TRUE},
	{2,138,63,89,19,31,TRUE},
	{93,71,65,89,17,31,TRUE},
	{2,2,67,89,17,30,TRUE},
	{2,71,65,89,20,29,TRUE},
	{2,203,71,87,15,30,TRUE},
	{93,138,73,87,13,30,TRUE},
	{182,180,71,87,14,30,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_scrap_hit[] = {
	{69,397,67,60,21,57,TRUE},
	{196,266,65,70,22,47,TRUE},
	{117,118,93,80,3,42,FALSE},
	{425,327,71,68,16,49,FALSE},
	{425,257,73,68,14,49,FALSE},
	{357,2,111,80,13,37,FALSE},
	{242,2,113,92,6,25,FALSE},
	{123,2,117,114,3,7,FALSE},
	{2,2,119,104,2,13,FALSE},
	{2,108,113,108,8,9,FALSE},
	{357,84,101,104,18,13,TRUE},
	{242,96,97,92,17,25,FALSE},
	{212,190,83,74,18,43,FALSE},
	{341,187,85,66,17,51,FALSE},
	{297,255,73,72,16,55,FALSE},
	{372,255,51,72,26,50,FALSE},
	{81,270,57,70,24,50,TRUE},
	{2,397,65,68,21,49,FALSE},
	{2,288,69,68,22,49,FALSE},
	{2,218,77,68,21,49,FALSE},
	{117,200,77,68,21,49,FALSE},
	{428,187,77,68,21,49,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_scrap_move[] = {
	{2,312,68,70,19,24,TRUE},
	{76,186,66,72,20,25,TRUE},
	{172,2,66,80,19,23,TRUE},
	{88,2,56,82,27,23,TRUE},
	{2,2,48,84,36,23,TRUE},
	{2,52,60,80,27,23,TRUE},
	{84,60,52,78,20,24,TRUE},
	{164,70,62,76,15,24,TRUE},
	{2,114,66,74,14,26,TRUE},
	{2,182,70,72,11,24,TRUE},
	{78,114,70,72,12,23,TRUE},
	{152,134,66,72,17,23,TRUE},
	{150,202,60,72,24,23,TRUE},
	{2,254,56,72,31,24,TRUE},
	{74,324,56,68,26,26,TRUE},
	{148,264,66,68,20,26,TRUE},
	{76,254,68,70,19,24,TRUE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_scrap_ko[] = {
	{291,376,77,69,79,54,TRUE},
	{166,347,123,91,60,39,FALSE},
	{887,2,133,101,50,29,FALSE},
	{2,413,111,107,41,23,FALSE},
	{164,440,121,113,38,22,FALSE},
	{887,105,131,105,34,25,FALSE},
	{363,300,125,103,42,27,FALSE},
	{2,308,103,125,67,5,TRUE},
	{2,522,101,99,63,31,FALSE},
	{547,371,91,95,59,35,TRUE},
	{362,405,101,93,57,37,FALSE},
	{287,500,93,91,63,39,FALSE},
	{2,623,75,87,81,43,TRUE},
	{160,555,75,87,81,43,TRUE},
	{644,373,75,85,81,45,TRUE},
	{465,444,75,87,81,43,FALSE},
	{382,500,81,87,75,43,FALSE},
	{883,212,139,129,36,15,FALSE},
	{722,121,159,139,27,11,FALSE},
	{557,2,163,147,24,7,FALSE},
	{722,2,163,117,17,29,FALSE},
	{370,161,141,137,27,13,FALSE},
	{557,151,157,155,16,12,FALSE},
	{2,149,165,157,15,13,FALSE},
	{189,127,179,157,8,14,FALSE},
	{374,2,181,157,7,13,FALSE},
	{2,2,185,145,5,25,FALSE},
	{189,2,183,123,5,36,FALSE},
	{716,262,149,109,35,51,FALSE},
	{867,343,145,83,35,62,FALSE},
	{169,286,131,59,49,85,FALSE},
	{556,308,137,61,43,84,FALSE},
	{490,319,123,55,40,85,TRUE},
	{513,161,93,41,38,84,TRUE},
	{115,503,95,43,36,85,TRUE},
	{330,455,25,39,106,85,FALSE},
	{287,455,41,41,95,84,FALSE},
	{115,448,53,45,88,85,TRUE},
	{302,286,59,43,83,85,FALSE},
	{302,331,57,43,85,84,FALSE},
	{513,256,61,41,82,84,TRUE},
	{129,308,67,35,79,83,TRUE},
	{129,377,69,33,79,80,TRUE},
	{370,127,1,1,0,0,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

SnowSprite *enemy_scrap_animations[6] = {
	sprite_scrap_idle,
	sprite_scrap_attack,
	sprite_scrap_daze,
	sprite_scrap_hit,
	sprite_scrap_move,
	sprite_scrap_ko
};

static SnowSprite sprite_tank_idle[] = {
	{81,2,77,65,4,3,FALSE},
	{2,136,75,65,6,3,FALSE},
	{156,69,75,65,6,3,FALSE},
	{79,69,75,65,6,3,FALSE},
	{2,203,75,63,6,5,FALSE},
	{79,201,75,63,6,5,FALSE},
	{77,331,73,63,8,5,FALSE},
	{154,266,73,63,8,5,FALSE},
	{2,268,73,63,8,5,FALSE},
	{79,266,73,63,8,5,FALSE},
	{156,201,73,63,8,5,FALSE},
	{156,136,75,63,6,5,FALSE},
	{79,136,75,63,6,5,FALSE},
	{2,69,75,65,6,3,FALSE},
	{160,2,75,65,6,3,FALSE},
	{2,2,77,65,4,3,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_tank_attack[] = {
	{103,304,99,79,36,34,FALSE},
	{103,304,99,79,36,34,FALSE},
	{103,304,99,79,36,34,FALSE},
	{204,306,109,79,36,34,FALSE},
	{204,306,109,79,36,34,FALSE},
	{2,144,133,77,11,36,FALSE},
	{2,144,133,77,11,36,FALSE},
	{311,69,133,77,8,37,FALSE},
	{337,395,133,81,8,32,FALSE},
	{337,395,133,81,8,32,FALSE},
	{204,387,131,79,7,34,FALSE},
	{204,387,131,79,7,34,FALSE},
	{137,146,129,77,9,36,FALSE},
	{180,69,129,75,8,38,FALSE},
	{2,223,127,77,9,36,FALSE},
	{268,148,127,77,9,36,FALSE},
	{87,385,115,79,20,34,FALSE},
	{79,69,99,73,35,40,FALSE},
	{158,2,93,65,35,48,FALSE},
	{2,65,75,65,35,48,FALSE},
	{407,2,75,65,35,48,FALSE},
	{330,2,75,65,35,48,FALSE},
	{2,2,75,61,35,52,FALSE},
	{404,314,79,87,31,26,TRUE},
	{2,302,77,99,33,14,TRUE},
	{331,227,77,97,33,16,TRUE},
	{331,227,77,97,33,16,TRUE},
	{430,227,79,85,31,28,FALSE},
	{315,306,79,87,31,26,TRUE},
	{230,227,77,99,33,14,TRUE},
	{131,225,77,97,33,16,TRUE},
	{131,225,77,97,33,16,TRUE},
	{131,225,77,97,33,16,TRUE},
	{397,148,77,99,33,14,TRUE},
	{2,381,83,81,27,32,FALSE},
	{79,2,77,63,34,50,FALSE},
	{253,2,75,65,35,48,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_tank_daze[] = {
	{82,226,78,72,6,3,FALSE},
	{162,226,78,70,6,5,FALSE},
	{2,226,78,72,6,3,FALSE},
	{2,2,80,72,5,3,FALSE},
	{162,152,78,72,6,3,FALSE},
	{82,152,78,72,6,3,FALSE},
	{2,152,78,72,6,3,FALSE},
	{162,78,78,72,6,3,FALSE},
	{164,2,78,74,6,1,FALSE},
	{2,76,78,74,6,1,FALSE},
	{84,2,78,74,6,1,FALSE},
	{82,78,78,72,6,3,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_tank_hit[] = {
	{422,204,76,66,18,22,FALSE},
	{370,138,78,64,16,24,FALSE},
	{2,2,100,82,0,6,FALSE},
	{290,2,86,84,11,4,FALSE},
	{200,2,88,88,6,0,FALSE},
	{2,86,92,88,2,0,FALSE},
	{104,2,94,86,0,2,FALSE},
	{96,90,88,84,6,4,FALSE},
	{416,340,72,66,22,22,FALSE},
	{416,272,72,66,22,22,FALSE},
	{342,272,72,68,22,20,FALSE},
	{264,224,76,66,18,22,FALSE},
	{186,92,78,66,16,22,FALSE},
	{378,70,80,66,14,22,FALSE},
	{290,88,78,66,16,22,FALSE},
	{378,2,80,66,14,22,FALSE},
	{344,204,76,66,18,22,FALSE},
	{186,160,76,66,18,22,FALSE},
	{266,156,76,66,18,22,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_tank_move[] = {
	{160,69,77,65,4,3,FALSE},
	{81,69,77,65,5,3,FALSE},
	{2,69,77,65,6,3,FALSE},
	{85,2,79,65,7,3,FALSE},
	{152,268,73,63,8,5,FALSE},
	{77,266,73,63,9,5,FALSE},
	{79,201,75,63,8,5,FALSE},
	{81,136,77,63,9,5,FALSE},
	{2,331,71,63,10,5,FALSE},
	{2,266,73,63,9,5,FALSE},
	{2,201,75,63,8,5,FALSE},
	{2,136,77,63,9,5,FALSE},
	{156,203,73,63,8,5,FALSE},
	{160,136,75,65,7,3,FALSE},
	{166,2,77,65,6,3,FALSE},
	{2,2,81,65,5,3,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_tank_ko[] = {
	{399,540,77,67,32,46,FALSE},
	{437,119,79,67,30,46,TRUE},
	{259,515,95,87,24,26,FALSE},
	{399,453,89,85,25,28,FALSE},
	{259,424,97,89,20,24,FALSE},
	{2,462,101,93,16,22,FALSE},
	{119,412,101,95,14,25,FALSE},
	{411,200,99,97,21,27,FALSE},
	{409,299,109,101,25,20,TRUE},
	{135,256,123,109,18,16,FALSE},
	{282,127,127,115,16,13,FALSE},
	{282,244,125,91,11,31,FALSE},
	{2,349,115,111,14,15,FALSE},
	{2,218,131,129,4,14,FALSE},
	{147,127,133,127,6,16,FALSE},
	{294,2,141,123,1,18,FALSE},
	{149,2,143,123,1,18,FALSE},
	{2,2,145,115,1,26,FALSE},
	{2,119,143,97,1,36,FALSE},
	{260,337,115,85,25,48,FALSE},
	{437,2,115,67,23,55,TRUE},
	{406,410,103,41,34,80,FALSE},
	{119,367,107,43,30,79,FALSE},
	{358,449,95,39,29,79,TRUE},
	{391,609,75,31,26,80,FALSE},
	{478,540,77,31,24,80,TRUE},
	{356,595,47,33,54,78,TRUE},
	{222,508,43,35,59,75,TRUE},
	{222,412,47,35,56,74,TRUE},
	{222,461,45,35,57,73,TRUE},
	{356,546,47,33,56,74,TRUE},
	{377,394,53,27,53,73,TRUE},
	{377,337,55,27,52,71,TRUE},
	{506,2,1,1,0,0,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

SnowSprite *enemy_tank_animations[6] = {
	sprite_tank_idle,
	sprite_tank_attack,
	sprite_tank_daze,
	sprite_tank_hit,
	sprite_tank_move,
	sprite_tank_ko
};

static float enemy_offsets[NUM_ENEMY_IMAGES][4] = {
	{0.46875, 0.90625, 185.0, 133.0},
	
	/* Sly */
	{0.53125, 1.0625, 63.0, 60.0},
	{0.46875, 1.0, 152.0, 106.0},
	{0.625, 1.0625, 74.0, 83.0},
	{0.53125, 0.875, 115.0, 112.0},
	{0.53125, 1.03125, 68.0, 64.0},
	{0.5, 0.65625, 195.0, 167.0},
	
	/* Scrap */
	{0.5, 1.03125, 112.0, 93.0},
	{1.47368, 2.89705, 76.0, 68.0},
	{0.40625, 0.9375, 67.0, 85.0},
	{0.83582, 2.03333, 67.0, 60.0},
	{0.83823, 1.38571, 68.0, 70.0},
	{1.46753, 1.84057, 77.0, 69.0},
	
	/* Tank */
	{0.63636, 1.07692, 77.0, 65.0},
	{0.78787, 1.45569, 99.0, 79.0},
	{0.59375, 1.0, 78.0, 72.0},
	{0.7922, 1.36363, 76.0, 66.0},
	{0.64935, 1.07692, 77.0, 65.0},
	{1.0, 1.7015, 77.0, 67.0}
};

struct _Enemy {
	int tipo;
	int frame, image;
	int x, y;
	int estado;
	int x_real, y_real;
	int next_x_real, next_y_real;
	int sum_x, sum_y;
	int vida;
	int max_life;
	
	int ref; /* Contador de referencias para saber si un enemigo se murió */
	int delay;
	int damage;
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
	obj->estado = ENEMY_SPAWN;
	obj->image = SNOWMAN_SPAWN;
	obj->frame = 0;
	
	if (obj->tipo == ENEMY_SLY) {
		obj->vida = obj->max_life = 30;
	} else if (obj->tipo == ENEMY_SCRAP) {
		obj->vida = obj->max_life = 45;
	} else if (obj->tipo == ENEMY_TANK) {
		obj->vida = obj->max_life = 60;
	}
	
	obj->ref = 0;
	obj->damage = 0;
	obj->delay = 0;
	return obj;
}

void enemy_move (Enemy *enemy, int x, int y) {
	enemy->frame = 0;
	enemy->estado = ENEMY_MOVE;
	
	if (enemy->tipo == ENEMY_SLY) {
		enemy->image = SLY_MOVE;
	} else if (enemy->tipo == ENEMY_SCRAP) {
		enemy->image = SCRAP_MOVE;
	} else if (enemy->tipo == ENEMY_TANK) {
		enemy->image = TANK_MOVE;
	}
	
	enemy->x = x;
	enemy->y = y;
	
	/* Calcular las siguientes coordenadas reales */
	enemy->next_x_real = MAP_X + (x * 70) + 35;
	enemy->next_y_real = MAP_Y + (y * 70) + 70;
	
	enemy->sum_x = (enemy->next_x_real - enemy->x_real) / 35;
	enemy->sum_y = (enemy->next_y_real - enemy->y_real) / 35;
}

void draw_enemy (Enemy *enemy) {
	SDL_Rect rect, rect2;
	int calc;
	int temp;
	int est;
	SnowSprite *animation;
	
	/* Si estamos esperando un golpe, calcular si ya llegó */
	if (enemy->damage != 0 && enemy->delay > 0) enemy->delay--;
	
	if (enemy->damage != 0 && enemy->delay == 0) {
		/* Delay de golpe */
		enemy->vida = enemy->vida - enemy->damage;
		
		if (enemy->vida < 0) {
			enemy->vida = 0;
		}
		
		if (enemy->ref == 0 && enemy->vida == 0) {
			enemy->estado = ENEMY_KO;
		} else {
			enemy->estado = ENEMY_HIT;
		}
		
		if (enemy->tipo == ENEMY_SLY) {
			enemy->image = (enemy->estado == ENEMY_KO) ? SLY_KO : SLY_HIT;
		} else if (enemy->tipo == ENEMY_SCRAP) {
			enemy->image = (enemy->estado == ENEMY_KO) ? SCRAP_KO : SCRAP_HIT;
		} else if (enemy->tipo == ENEMY_TANK) {
			enemy->image = (enemy->estado == ENEMY_KO) ? TANK_KO : TANK_HIT;
		}
		
		enemy->frame = 0;
		enemy->damage = 0;
		enemy->delay = 0;
	}
	
	est = enemy->estado;
	calc = enemy->frame / 2;
	
	if (est == ENEMY_SPAWN) {
		animation = sprite_spawn;
	} else if (enemy->tipo == ENEMY_SLY) {
		animation = enemy_sly_animations[est - ENEMY_IDLE];
	} else if (enemy->tipo == ENEMY_SCRAP) {
		animation = enemy_scrap_animations[est - ENEMY_IDLE];
	} else if (enemy->tipo == ENEMY_TANK) {
		animation = enemy_tank_animations[est - ENEMY_IDLE];
	}
	
	rect2.x = animation[calc].orig_x;
	rect2.y = animation[calc].orig_y;
	rect.w = rect2.w = animation[calc].w;
	rect.h = rect2.h = animation[calc].h;
	
	if (enemy->estado == ENEMY_MOVE) {
		enemy->x_real += enemy->sum_x;
		enemy->y_real += enemy->sum_y;
	}
	
	rect.x = enemy->x_real - enemy_offsets_int[enemy->image][0] + animation[calc].dest_x;
	rect.y = enemy->y_real - enemy_offsets_int[enemy->image][1] + animation[calc].dest_y;
	
	if (animation[calc].rot) {
		rect2.w = rect.h;
		rect2.h = rect.w;
		
		rect.h = rect2.h;
		rect.w = rect2.w;
		SDL_Point p;
		p.y = 0;
		p.x = rect.w;
		rect.x -= rect.w;
		
		SDL_RenderCopyEx (renderer, enemy_images[enemy->image], &rect2, &rect, 270.0, &p, SDL_FLIP_NONE);
	} else {
		SDL_RenderCopy (renderer, enemy_images[enemy->image], &rect2, &rect);
	}
	
	if (est != ENEMY_SPAWN) {
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
		if (est == ENEMY_SPAWN) {
			enemy->estado = ENEMY_IDLE;
			if (enemy->tipo == ENEMY_SLY) {
				enemy->image = SLY_IDLE;
			} else if (enemy->tipo == ENEMY_SCRAP) {
				enemy->image = SCRAP_IDLE;
			} else if (enemy->tipo == ENEMY_TANK) {
				enemy->image = TANK_IDLE;
			}
		} else if (est == ENEMY_HIT || est == ENEMY_ATTACK) {
			enemy->estado = ENEMY_IDLE;
			if (enemy->tipo == ENEMY_SLY) {
				enemy->image = SLY_IDLE;
			} else if (enemy->tipo == ENEMY_SCRAP) {
				enemy->image = SCRAP_IDLE;
			} else if (enemy->tipo == ENEMY_TANK) {
				enemy->image = TANK_IDLE;
			}
		}
	}
	
	if (enemy->estado == ENEMY_MOVE) {
		if (enemy->x_real == enemy->next_x_real && enemy->y_real == enemy->next_y_real) {
			/* Llegamos al destino */
			enemy->estado = ENEMY_IDLE;
			if (enemy->tipo == ENEMY_SLY) {
				enemy->image = SLY_IDLE;
			} else if (enemy->tipo == ENEMY_SCRAP) {
				enemy->image = SCRAP_IDLE;
			} else if (enemy->tipo == ENEMY_TANK) {
				enemy->image = TANK_IDLE;
			}
			enemy->frame = 0;
		}
	}
}

int is_enemy_ready (Enemy *enemy) {
	if (enemy->estado == ENEMY_IDLE) {
		return TRUE;
	}
	
	return FALSE;
}

int is_enemy_dead (Enemy *enemy) {
	int calc;
	calc = enemy->frame / 2;
	if (enemy->estado == ENEMY_KO && enemy->tipo == ENEMY_SLY) {
		if (enemy->frame == 83) {
			return TRUE;
		}
	}
	
	if (enemy->estado == ENEMY_KO && enemy->tipo == ENEMY_SCRAP) {
		if (enemy->frame == 87) {
			return TRUE;
		}
	}
	
	if (enemy->estado == ENEMY_KO && enemy->tipo == ENEMY_TANK) {
		if (enemy->frame == 67) {
			return TRUE;
		}
	}
	
	return FALSE;
}

void enemy_ask_coords (Enemy *enemy, int *x, int *y) {
	if (x != NULL) {
		*x = enemy->x;
	}
	
	if (y != NULL) {
		*y = enemy->y;
	}
}

void add_enemy_ref (Enemy *enemy) {
	enemy->ref++;
}

void enemy_hit_delayed (Enemy *enemy, int damage, int delay) {
	enemy->ref--;
	enemy->delay = delay;
	enemy->damage = damage;
}

int enemy_get_hit_zone (Enemy *enemy, EnemyHitZone *hitzone, int x, int y) {
	int g, h, i;
	int max_h, min_h, max_g, min_g;
	
	if (enemy->tipo == ENEMY_SLY) {
		hitzone[0].x = x;
		hitzone[0].y = y;
		g = enemy->x - x;
		h = enemy->y - y;
		if (g < 0) g = -g;
		if (h < 0) h = -h;
		
		hitzone[0].damage = g + h + 2;
		hitzone[0].delay = 50;
		
		return 1;
	} else if (enemy->tipo == ENEMY_SCRAP) {
		i = 0;
		for (g = x - 1; g <= x + 1; g++) {
			for (h = y - 1; h <= y + 1; h++) {
				if (g < 0 || h < 0 || g > 8 || h > 4) continue;
				hitzone[i].x = g;
				hitzone[i].y = h;
				if (g == x && h == y) {
					hitzone[i].delay = 54;
					hitzone[i].damage = 8;
				} else {
					hitzone[i].delay = 64;
					hitzone[i].damage = 4;
				}
				i++;
			}
		}
		
		return i;
	} else if (enemy->tipo == ENEMY_TANK) {
		printf ("Tank busca su zona de hit. (%i, %i). Ataque: %i, %i\n", enemy->x, enemy->y, x, y);
		if (x == enemy->x) { /* Golpe hacia abajo o hacia arriba */
			min_g = (x - 1 < 0) ? x : x - 1;
			max_g = (x + 1 > 8) ? x : x + 1;
			min_h = max_h = y;
		} else if (y == enemy->y) { /* Golpe hacia la izquierda o derecha */
			min_h = (y - 1 < 0) ? y : y - 1;
			max_h = (y + 1 > 4) ? y : y + 1;
			min_g = max_g = x;
		}
		
		i = 0;
		for (g = min_g; g <= max_g; g++) {
			for (h = min_h; h <= max_h; h++) {
				if (g < 0 || h < 0 || g > 8 || h > 4) continue;
				printf ("Hitzone: %i, %i\n", g, h);
				hitzone[i].x = g;
				hitzone[i].y = h;
				hitzone[i].delay = 20;
				hitzone[i].damage = 10;
				
				i++;
			}
		}
		
		return i;
	}
	
	return 0;
}

int is_enemy_done (Enemy *enemy) {
	if (enemy->estado == ENEMY_IDLE) return TRUE;
	
	return FALSE;
}

void enemy_attack (Enemy *enemy) {
	enemy->frame = 0;
	enemy->estado = ENEMY_ATTACK;
	
	if (enemy->tipo == ENEMY_SLY) {
		enemy->image = SLY_ATTACK;
	} else if (enemy->tipo == ENEMY_SCRAP) {
		enemy->image = SCRAP_ATTACK;
	} else if (enemy->tipo == ENEMY_TANK) {
		enemy->image = TANK_ATTACK;
	}
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

void adjust_enemy (int x, int y) {
	int g = TANK_KO;
	enemy_offsets_int[g][0] += x;
	enemy_offsets_int[g][1] += y;
	printf ("Offset actual: %i, %i\n", enemy_offsets_int[g][0], enemy_offsets_int[g][1]);
}

