/*
 * ninja.c
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

#include <SDL.h>
#include <SDL_image.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>
#include "gettext.h"
#define _(string) gettext (string)

#include "path.h"

#include "ninja.h"
#include "snow.h"
#include "enemy.h"

/* Listar todas las imágenes */
enum {
	IMG_FIRE_NINJA_IDLE,
	IMG_FIRE_NINJA_ATTACK,
	IMG_FIRE_NINJA_CELEBRATE_INTRO,
	IMG_FIRE_NINJA_CELEBRATE,
	IMG_FIRE_NINJA_MOVE,
	IMG_FIRE_NINJA_GHOST,
	IMG_FIRE_NINJA_HIT,
	IMG_FIRE_NINJA_KO_INTRO,
	IMG_FIRE_NINJA_KO_LOOP,
	IMG_FIRE_NINJA_POWERCARD_BOTTLEDROP,
	IMG_FIRE_NINJA_POWERCARD_SUMMON,
	IMG_FIRE_NINJA_POWERCARD_FIRE,
	IMG_FIRE_NINJA_REVIVED,
	IMG_FIRE_NINJA_REVIVE_OTHER,
	IMG_FIRE_NINJA_REVIVE_OTHER_LOOP,
	
	IMG_SNOW_NINJA_IDLE,
	IMG_SNOW_NINJA_ATTACK,
	IMG_SNOW_NINJA_HEAL,
	IMG_SNOW_NINJA_CELEBRATE,
	IMG_SNOW_NINJA_MOVE,
	IMG_SNOW_NINJA_GHOST,
	IMG_SNOW_NINJA_HIT,
	IMG_SNOW_NINJA_KO_INTRO,
	IMG_SNOW_NINJA_KO_LOOP,
	IMG_SNOW_NINJA_POWERCARD_IGLOO_1,
	IMG_SNOW_NINJA_POWERCARD_IGLOO_2,
	IMG_SNOW_NINJA_POWERCARD_SUMMON,
	IMG_SNOW_NINJA_POWERCARD_SNOW,
	IMG_SNOW_NINJA_REVIVED,
	IMG_SNOW_NINJA_REVIVE_OTHER,
	IMG_SNOW_NINJA_REVIVE_OTHER_LOOP,
	
	IMG_WATER_NINJA_IDLE,
	IMG_WATER_NINJA_ATTACK,
	IMG_WATER_NINJA_CELEBRATE,
	IMG_WATER_NINJA_MOVE,
	IMG_WATER_NINJA_GHOST,
	IMG_WATER_NINJA_HIT,
	IMG_WATER_NINJA_KO_INTRO,
	IMG_WATER_NINJA_KO_LOOP,
	IMG_WATER_NINJA_POWERCARD_FISHDROP,
	IMG_WATER_NINJA_POWERCARD_SUMMON,
	IMG_WATER_NINJA_POWERCARD_WATER,
	IMG_WATER_NINJA_REVIVED,
	IMG_WATER_NINJA_REVIVE_OTHER,
	IMG_WATER_NINJA_REVIVE_OTHER_LOOP,
	
	NUM_NINJA_IMAGES
};

static const char *ninja_images_names[NUM_NINJA_IMAGES] = {
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
	"images/water_ninja/revive_other_loop.png"
};

/* Los estados de los ninjas */
enum {
	NINJA_IDLE,
	NINJA_ATTACK,
	NINJA_HEAL,
	NINJA_REVIVE_INTRO,
	NINJA_REVIVE_LOOP,
	NINJA_CELEBRATE_INTRO,
	NINJA_CELEBRATE_LOOP,
	NINJA_MOVE,
	NINJA_HIT,
	NINJA_KO_INTRO,
	NINJA_KO_LOOP,
	NINJA_REVIVED,
	
	NUM_NINJA_STATES
};

static SnowSprite fire_sprite_idle[] = {
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

static SnowSprite fire_sprite_attack[] = {
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

static SnowSprite fire_sprite_celebrate_intro[] = {
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

static SnowSprite fire_sprite_celebrate[] = {
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

static SnowSprite fire_sprite_move[] = {
	{142,2,64,92,23,19,FALSE},
	{348,2,62,86,25,25,FALSE},
	{288,2,58,88,25,23,FALSE},
	{74,2,66,92,19,19,FALSE},
	{208,2,78,90,12,21,FALSE},
	{412,2,78,94,13,17,TRUE},
	{2,2,70,92,19,19,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite fire_sprite_hit[] = {
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

static SnowSprite fire_sprite_ko_intro[] = {
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

static SnowSprite fire_sprite_ko_loop[] = {
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

static SnowSprite fire_sprite_revived[] = {
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

SnowSprite *fire_animations[NUM_NINJA_STATES] = {
	fire_sprite_idle,
	fire_sprite_attack,
	NULL, /* Los ninjas de fuego no curan */
	NULL,
	NULL,
	fire_sprite_celebrate_intro,
	fire_sprite_celebrate,
	fire_sprite_move,
	fire_sprite_hit,
	fire_sprite_ko_intro,
	fire_sprite_ko_loop,
	fire_sprite_revived
};

static SnowSprite snow_sprite_idle[] = {
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

static SnowSprite snow_sprite_attack[] = {
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

static SnowSprite snow_sprite_heal [] = {
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

static SnowSprite snow_sprite_celebrate[] = {
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

static SnowSprite snow_sprite_move[] = {
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

static SnowSprite snow_sprite_hit[] = {
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

static SnowSprite snow_sprite_ko_intro[] = {
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

static SnowSprite snow_sprite_ko_loop[] = {
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

static SnowSprite snow_sprite_revived[] = {
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

SnowSprite *snow_animations[NUM_NINJA_STATES] = {
	snow_sprite_idle,
	snow_sprite_attack,
	snow_sprite_heal,
	NULL,
	NULL,
	NULL, /* Celebrate intro */
	snow_sprite_celebrate,
	snow_sprite_move,
	snow_sprite_hit,
	snow_sprite_ko_intro,
	snow_sprite_ko_loop,
	snow_sprite_revived,
};

static SnowSprite water_sprite_idle[] = {
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

static SnowSprite water_sprite_attack[] = {
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

static SnowSprite water_sprite_celebrate[] = {
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

static SnowSprite water_sprite_move[] = {
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

static SnowSprite water_sprite_hit[] = {
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

static SnowSprite water_sprite_ko_intro[] = {
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

static SnowSprite water_sprite_ko_loop[] = {
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

static SnowSprite water_sprite_revived[] = {
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

SnowSprite *water_animations[NUM_NINJA_STATES] = {
	water_sprite_idle,
	water_sprite_attack,
	NULL, /* Los ninjas de agua no curan */
	NULL,
	NULL,
	NULL, /* Celebrate intro */
	water_sprite_celebrate,
	water_sprite_move,
	water_sprite_hit,
	water_sprite_ko_intro,
	water_sprite_ko_loop,
	water_sprite_revived,
};

static float ninja_offsets[NUM_NINJA_IMAGES][4] = {
	/* Primero fuego */
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
	{0.0, 0.0, 0.0, 0.0},
	
	/* Luego nieve */
	{0.5, 1.09375, 112.0, 56.0},
	{0.5, 0.84375, 111.0, 139.0},
	{0.46875, 1.0, 111.0, 111.0},
	{0.5625, 0.875, 112.0, 138.0},
	{0.5, 1.0625, 84.0, 83.0},
	{0.46875, 1.1363, 70.0, 66.0},
	{0.5, 0.96875, 139.0, 89.0},
	{0.46875, 0.875, 115.0, 112.0},
	{0.46875, 0.875, 112.0, 111.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.46875, 0.96875, 139.0, 139.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0},
	
	/* Al final agua */
	{0.59375, 0.9375, 112.0, 111.0},
	{0.52307, 0.92857, 195.0, 112.0},
	{0.5, 0.75, 166.0, 166.0},
	{0.5625, 0.90625, 112.0, 139.0},
	{0.65625, 1.1111, 78.0, 99.0},
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

static int ninja_offsets_int[NUM_NINJA_IMAGES][2];

static SDL_Texture *ninja_images[NUM_NINJA_IMAGES];

struct _Ninja {
	int tipo;
	int frame, image;
	int x, y;
	int next_x, next_y;
	int estado;
	int x_real, y_real;
	int next_x_real, next_y_real;
	int sum_x, sum_y;
	
	Enemy *dest_attack;
};

Ninja *create_ninja (int tipo, int x, int y) {
	Ninja *obj;
	
	if (tipo < NINJA_FIRE || tipo > NINJA_WATER) {
		/* Tipo incorrecto */
		return NULL;
	}
	
	obj = (Ninja *) malloc (sizeof (Ninja));
	
	obj->tipo = tipo;
	
	obj->x = x;
	obj->y = y;
	obj->x_real = MAP_X + (x * 70) + 35;
	obj->y_real = MAP_Y + (y * 70) + 70;
	
	obj->next_x = obj->x;
	obj->next_y = obj->y;
	obj->estado = NINJA_IDLE;
	obj->frame = 0;
	
	if (tipo == NINJA_FIRE) {
		obj->image = IMG_FIRE_NINJA_IDLE;
	} else if (tipo == NINJA_SNOW) {
		obj->image = IMG_SNOW_NINJA_IDLE;
	} else if (tipo == NINJA_WATER) {
		obj->image = IMG_WATER_NINJA_IDLE;
	}
	
	return obj;
}

void ninja_ask_coords (Ninja *ninja, int *x, int *y) {
	if (x != NULL) {
		*x = ninja->x;
	}
	
	if (y != NULL) {
		*y = ninja->y;
	}
}

int ninja_is_done (Ninja *ninja) {
	if (ninja->estado == NINJA_IDLE) return TRUE;
	
	return FALSE;
}

void ninja_attack (Ninja *ninja, Enemy *enemy) {
	ninja->frame = 0;
	ninja->estado = NINJA_ATTACK;
	
	if (ninja->tipo == NINJA_FIRE) {
		ninja->image = IMG_FIRE_NINJA_ATTACK;
	} else if (ninja->tipo == NINJA_SNOW) {
		ninja->image = IMG_SNOW_NINJA_ATTACK;
	} else if (ninja->tipo == NINJA_WATER) {
		ninja->image = IMG_WATER_NINJA_ATTACK;
	}
	
	ninja->dest_attack = enemy;
}

void ninja_move (Ninja *ninja, int x, int y) {
	ninja->frame = 0;
	ninja->estado = NINJA_MOVE;
	
	if (ninja->tipo == NINJA_FIRE) {
		ninja->image = IMG_FIRE_NINJA_MOVE;
	} else if (ninja->tipo == NINJA_SNOW) {
		ninja->image = IMG_SNOW_NINJA_MOVE;
	} else if (ninja->tipo == NINJA_WATER) {
		ninja->image = IMG_WATER_NINJA_MOVE;
	}
	
	ninja->x = ninja->next_x = x;
	ninja->y = ninja->next_y = y;
	
	/* Calcular las siguientes coordenadas reales */
	ninja->next_x_real = MAP_X + (x * 70) + 35;
	ninja->next_y_real = MAP_Y + (y * 70) + 70;
	
	ninja->sum_x = (ninja->next_x_real - ninja->x_real) / 35;
	ninja->sum_y = (ninja->next_y_real - ninja->y_real) / 35;
}

void ninja_move_ghost (Ninja *ninja, int x, int y) {
	ninja->next_x = x;
	ninja->next_y = y;
}

void ninja_draw (Ninja *ninja) {
	SDL_Rect rect, rect2;
	int calc;
	int temp;
	SnowSprite *animation;
	
	calc = ninja->frame / 2;
	
	if (ninja->tipo == NINJA_FIRE) {
		animation = fire_animations[ninja->estado];
	} else if (ninja->tipo == NINJA_SNOW) {
		animation = snow_animations[ninja->estado];
	} else if (ninja->tipo == NINJA_WATER) {
		animation = water_animations[ninja->estado];
	}
	
	rect2.x = animation[calc].orig_x;
	rect2.y = animation[calc].orig_y;
	rect.w = rect2.w = animation[calc].w;
	rect.h = rect2.h = animation[calc].h;
	
	if (ninja->estado == NINJA_MOVE) {
		ninja->x_real += ninja->sum_x;
		ninja->y_real += ninja->sum_y;
	}
	
	rect.x = ninja->x_real - ninja_offsets_int[ninja->image][0] + animation[calc].dest_x;
	rect.y = ninja->y_real - ninja_offsets_int[ninja->image][1] + animation[calc].dest_y;
	
	if (animation[calc].rot) {
		rect2.w = rect.h;
		rect2.h = rect.w;
		
		rect.h = rect2.h;
		rect.w = rect2.w;
		SDL_Point p;
		p.y = 0;
		p.x = rect.w;
		rect.x -= rect.w;
		
		SDL_RenderCopyEx (renderer, ninja_images[ninja->image], &rect2, &rect, 270.0, &p, SDL_FLIP_NONE);
	} else {
		SDL_RenderCopy (renderer, ninja_images[ninja->image], &rect2, &rect);
	}
	
	if (ninja->tipo == NINJA_SNOW && ninja->estado == NINJA_ATTACK && ninja->frame == 26) {
		/* Golpear al enemigo */
		enemy_hit (ninja->dest_attack, 6);
	}
	
	ninja->frame++;
	calc = ninja->frame / 2;
	if (animation[calc].w == -1 && animation[calc].h == -1) {
		ninja->frame = 0;
		if (ninja->estado == NINJA_ATTACK) {
			ninja->estado = NINJA_IDLE;
		} else if (ninja->estado == NINJA_KO_INTRO) {
			ninja->estado = NINJA_KO_LOOP;
		} else if (ninja->estado == NINJA_HIT) {
			ninja->estado = NINJA_IDLE;
		} else if (ninja->estado == NINJA_REVIVED) {
			ninja->estado = NINJA_IDLE;
		} else if (ninja->estado == NINJA_HEAL) {
			ninja->estado = NINJA_IDLE;
		}
		
		if (ninja->estado == NINJA_IDLE) {
			if (ninja->tipo == NINJA_FIRE) {
				ninja->image = IMG_FIRE_NINJA_IDLE;
			} else if (ninja->tipo == NINJA_SNOW) {
				ninja->image = IMG_SNOW_NINJA_IDLE;
			} else if (ninja->tipo == NINJA_WATER) {
				ninja->image = IMG_WATER_NINJA_IDLE;
			}
		} else if (ninja->estado == NINJA_KO_LOOP) {
			if (ninja->tipo == NINJA_FIRE) {
				ninja->image = IMG_FIRE_NINJA_KO_LOOP;
			} else if (ninja->tipo == NINJA_SNOW) {
				ninja->image = IMG_SNOW_NINJA_KO_LOOP;
			} else if (ninja->tipo == NINJA_WATER) {
				ninja->image = IMG_WATER_NINJA_KO_LOOP;
			}
		}
	}
	if (ninja->estado == NINJA_MOVE) {
		if (ninja->x_real == ninja->next_x_real && ninja->y_real == ninja->next_y_real) {
			/* Llegamos al destino */
			ninja->estado = NINJA_IDLE;
			ninja->frame = 0;
			
			if (ninja->tipo == NINJA_FIRE) {
				ninja->image = IMG_FIRE_NINJA_IDLE;
			} else if (ninja->tipo == NINJA_SNOW) {
				ninja->image = IMG_SNOW_NINJA_IDLE;
			} else if (ninja->tipo == NINJA_WATER) {
				ninja->image = IMG_WATER_NINJA_IDLE;
			}
		}
	}
}

void ninja_draw_ghost (Ninja *ninja) {
	SDL_Rect rect;
	int image;
	if (ninja->x != ninja->next_x || ninja->y != ninja->next_y) {
		if (ninja->tipo == NINJA_FIRE) {
			image = IMG_FIRE_NINJA_GHOST;
		} else if (ninja->tipo == NINJA_SNOW) {
			image = IMG_SNOW_NINJA_GHOST;
		} else if (ninja->tipo == NINJA_WATER) {
			image = IMG_WATER_NINJA_GHOST;
		}
		
		/* Dibujar el ninja fantasma */
		rect.x = MAP_X + (ninja->next_x * 70) + 35 - ninja_offsets_int[image][0];
		rect.y = MAP_Y + (ninja->next_y * 70) + 70 - ninja_offsets_int[image][1];
		
		SDL_QueryTexture (ninja_images[image], NULL, NULL, &rect.w, &rect.h);
		SDL_RenderCopy (renderer, ninja_images[image], NULL, &rect);
	}
}

void ninja_ask_actions (Ninja *ninja, int escenario[5][9], int acciones[5][9]) {
	int g, h;
	int obj;
	int s;
	
	int max;
	
	if (ninja->tipo == NINJA_FIRE || ninja->tipo == NINJA_WATER) {
		max = 2;
	} else if (ninja->tipo == NINJA_SNOW) {
		max = 3;
	}
	
	for (g = -3; g <= 3; g++) {
		for (h = -3; h <= 3; h++) {
			s = ((g < 0) ? -g : g) + ((h < 0) ? -h : h);
			if (s > max) continue;
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
	
	if (ninja->tipo == NINJA_FIRE) {
		max = 2;
	} else if (ninja->tipo == NINJA_SNOW) {
		max = 3;
	} else if (ninja->tipo == NINJA_WATER) {
		max = 1;
	}
	
	for (g = -3; g <= 3; g++) {
		for (h = -3; h <= 3; h++) {
			s = ((g < 0) ? -g : g) + ((h < 0) ? -h : h);
			
			if (s > max) continue;
			if (ninja->next_x + g >= 0 && ninja->next_x + g < 9 && ninja->next_y + h >= 0 && ninja->next_y + h < 5) {
				obj = escenario[ninja->next_y + h][ninja->next_x + g];
				if (obj >= ENEMY_1 && obj <= ENEMY_4) {
					acciones[ninja->next_y + h][ninja->next_x + g] |= ACTION_ATTACK;
				}
			}
		}
	}
}

void setup_ninja (void) {
	SDL_Surface * image;
	SDL_Texture * texture;
	int g;
	
	char buffer_file[8192];
	
	for (g = 0; g < NUM_NINJA_IMAGES; g++) {
		sprintf (buffer_file, "%s%s", systemdata_path, ninja_images_names[g]);
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
		ninja_images[g] = texture;
		SDL_FreeSurface (image);
		/* TODO: Mostrar la carga de porcentaje */
		
		/* Calcular los centros */
		ninja_offsets_int[g][0] = (int) roundf (ninja_offsets[g][0] * ninja_offsets[g][2]);
		ninja_offsets_int[g][1] = (int) roundf (ninja_offsets[g][1] * ninja_offsets[g][3]);
	}
}


