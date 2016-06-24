/*
 * uitimer.c
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

#include <stdio.h>
#include <stdlib.h>
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
#include "snow.h"

#include "uitimer.h"

#define TIMER_X 320

enum {
	IMG_UI_TIMER_WATER_BASE,
	IMG_UI_TIMER_WATER_BUTTON_DEACTIVE,
	IMG_UI_TIMER_WATER_BUTTON_UP,
	IMG_UI_TIMER_WATER_BUTTON_OVER,
	IMG_UI_TIMER_WATER_BUTTON_DOWN,
	
	IMG_UI_TIMER_WATER_TICK_0,
	IMG_UI_TIMER_WATER_TICK_1,
	IMG_UI_TIMER_WATER_TICK_2,
	IMG_UI_TIMER_WATER_TICK_3,
	IMG_UI_TIMER_WATER_TICK_4,
	IMG_UI_TIMER_WATER_TICK_5,
	IMG_UI_TIMER_WATER_TICK_6,
	IMG_UI_TIMER_WATER_TICK_7,
	IMG_UI_TIMER_WATER_TICK_8,
	IMG_UI_TIMER_WATER_TICK_9,
	
	NUM_UI_TIMER
};

static const char *ui_timer_images_names[NUM_UI_TIMER] = {
	"images/water_timer/base.png",
	"images/water_timer/button_deactive.png",
	"images/water_timer/button_up.png",
	"images/water_timer/button_over.png",
	"images/water_timer/button_down.png",
	
	"images/water_timer/tick0.png",
	"images/water_timer/tick1.png",
	"images/water_timer/tick2.png",
	"images/water_timer/tick3.png",
	"images/water_timer/tick4.png",
	"images/water_timer/tick5.png",
	"images/water_timer/tick6.png",
	"images/water_timer/tick7.png",
	"images/water_timer/tick8.png",
	"images/water_timer/tick9.png"
};

struct _UITimer {
	int color;
	int anim;
	
	int img_base, img_ring, img_button;
};

static SDL_Texture *ui_timer_images[NUM_UI_TIMER];

UITimer *crear_timer (int ui) {
	UITimer *nuevo;
	
	nuevo = (UITimer *) malloc (sizeof (UITimer));
	
	nuevo->color = ui;
	nuevo->anim = 0;
	
	if (ui == UI_FIRE) {
		//nuevo->img_base = IMG_UI_TIMER_FIRE_BASE;
		//nuevo->img_ring = IMG_UI_TIMER_FIRE_TICK_0;
		//nuevo->img_button = IMG_UI_TIMER_FIRE_BUTTON_DEACTIVE;
	} else if (ui == UI_SNOW) {
		//nuevo->img_base = IMG_UI_TIMER_SNOW_BASE;
		//nuevo->img_ring = IMG_UI_TIMER_SNOW_TICK_0;
		//nuevo->img_button = IMG_UI_TIMER_SNOW_BUTTON_DEACTIVE;
	} else if (ui == UI_WATER) {
		nuevo->img_base = IMG_UI_TIMER_WATER_BASE;
		nuevo->img_ring = IMG_UI_TIMER_WATER_TICK_0;
		nuevo->img_button = IMG_UI_TIMER_WATER_BUTTON_DEACTIVE;
	}
	
	return nuevo;
};

void dibujar_timer (UITimer *timer) {
	SDL_Rect rect;
	int g;
	
	g = timer->img_base;
	/* Dibujar el reloj base */
	rect.x = TIMER_X;
	rect.y = -70 + timer->anim;
	SDL_QueryTexture (ui_timer_images[g], NULL, NULL, &rect.w, &rect.h);
	SDL_RenderCopy (renderer, ui_timer_images[g], NULL, &rect);
	
	g = timer->img_ring;
	if (timer->color == UI_WATER) {
		rect.x = TIMER_X + 26;
		rect.y = -70 + timer->anim + 13;
	} else if (timer->color == UI_SNOW) {
		//i = IMG_UI_SNOW_TIMER_BASE;
	} else if (timer->color == UI_FIRE) {
		//i = IMG_UI_FIRE_TIMER_BASE;
	}
	SDL_QueryTexture (ui_timer_images[g], NULL, NULL, &rect.w, &rect.h);
	SDL_RenderCopy (renderer, ui_timer_images[g], NULL, &rect);
	
	g = timer->img_button;
	if (timer->color == UI_WATER) {
		rect.x = TIMER_X + 106;
		rect.y = -70 + timer->anim + 36;
	}
	
	SDL_QueryTexture (ui_timer_images[g], NULL, NULL, &rect.w, &rect.h);
	SDL_RenderCopy (renderer, ui_timer_images[g], NULL, &rect);
	
	timer->anim++;
	if (timer->anim >= 70) {
		timer->anim = 70;
		//ui_estatus = UI_WAIT_INPUT;
	}
}

void setup_timer (void) {
	SDL_Surface * image;
	SDL_Texture * texture;
	int g;
	
	char buffer_file[8192];
	
	for (g = 0; g < NUM_UI_TIMER; g++) {
		sprintf (buffer_file, "%s%s", systemdata_path, ui_timer_images_names[g]);
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
		ui_timer_images[g] = texture;
		SDL_FreeSurface (image);
		/* TODO: Mostrar la carga de porcentaje */
	}
}
