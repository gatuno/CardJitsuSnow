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

#define TIMER_X 420

enum {
	IMG_UI_TIMER_FIRE_BASE,
	IMG_UI_TIMER_FIRE_BUTTON_DEACTIVE,
	IMG_UI_TIMER_FIRE_BUTTON_UP,
	IMG_UI_TIMER_FIRE_BUTTON_OVER,
	IMG_UI_TIMER_FIRE_BUTTON_DOWN,
	
	IMG_UI_TIMER_FIRE_TICK_0,
	IMG_UI_TIMER_FIRE_TICK_1,
	IMG_UI_TIMER_FIRE_TICK_2,
	IMG_UI_TIMER_FIRE_TICK_3,
	IMG_UI_TIMER_FIRE_TICK_4,
	IMG_UI_TIMER_FIRE_TICK_5,
	IMG_UI_TIMER_FIRE_TICK_6,
	IMG_UI_TIMER_FIRE_TICK_7,
	IMG_UI_TIMER_FIRE_TICK_8,
	IMG_UI_TIMER_FIRE_TICK_9,
	
	IMG_UI_TIMER_SNOW_BASE,
	IMG_UI_TIMER_SNOW_BUTTON_DEACTIVE,
	IMG_UI_TIMER_SNOW_BUTTON_UP,
	IMG_UI_TIMER_SNOW_BUTTON_OVER,
	IMG_UI_TIMER_SNOW_BUTTON_DOWN,
	
	IMG_UI_TIMER_SNOW_TICK_0,
	IMG_UI_TIMER_SNOW_TICK_1,
	IMG_UI_TIMER_SNOW_TICK_2,
	IMG_UI_TIMER_SNOW_TICK_3,
	IMG_UI_TIMER_SNOW_TICK_4,
	IMG_UI_TIMER_SNOW_TICK_5,
	IMG_UI_TIMER_SNOW_TICK_6,
	IMG_UI_TIMER_SNOW_TICK_7,
	IMG_UI_TIMER_SNOW_TICK_8,
	IMG_UI_TIMER_SNOW_TICK_9,
	
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
	"images/fire_timer/base.png",
	"images/fire_timer/button_deactive.png",
	"images/fire_timer/button_up.png",
	"images/fire_timer/button_over.png",
	"images/fire_timer/button_down.png",
	
	"images/fire_timer/tick0.png",
	"images/fire_timer/tick1.png",
	"images/fire_timer/tick2.png",
	"images/fire_timer/tick3.png",
	"images/fire_timer/tick4.png",
	"images/fire_timer/tick5.png",
	"images/fire_timer/tick6.png",
	"images/fire_timer/tick7.png",
	"images/fire_timer/tick8.png",
	"images/fire_timer/tick9.png",
	
	"images/snow_timer/base.png",
	"images/snow_timer/button_deactive.png",
	"images/snow_timer/button_up.png",
	"images/snow_timer/button_over.png",
	"images/snow_timer/button_down.png",
	
	"images/snow_timer/tick0.png",
	"images/snow_timer/tick1.png",
	"images/snow_timer/tick2.png",
	"images/snow_timer/tick3.png",
	"images/snow_timer/tick4.png",
	"images/snow_timer/tick5.png",
	"images/snow_timer/tick6.png",
	"images/snow_timer/tick7.png",
	"images/snow_timer/tick8.png",
	"images/snow_timer/tick9.png",
	
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

enum {
	TIMER_SHOW,
	TIMER_TICKING,
};

struct _UITimer {
	int color;
	int anim;
	int estado;
	
	Uint32 timestamp;
	int img_base, img_ring, img_button;
};

static SDL_Texture *ui_timer_images[NUM_UI_TIMER];

/* Constante de los eventos */
int UI_TIMER_EVENT;

UITimer *crear_timer (int ui) {
	UITimer *nuevo;
	
	nuevo = (UITimer *) malloc (sizeof (UITimer));
	
	nuevo->color = ui;
	nuevo->anim = 0;
	nuevo->estado = TIMER_SHOW;
	
	if (ui == UI_FIRE) {
		nuevo->img_base = IMG_UI_TIMER_FIRE_BASE;
		nuevo->img_ring = IMG_UI_TIMER_FIRE_TICK_0;
		nuevo->img_button = IMG_UI_TIMER_FIRE_BUTTON_DEACTIVE;
	} else if (ui == UI_SNOW) {
		nuevo->img_base = IMG_UI_TIMER_SNOW_BASE;
		nuevo->img_ring = IMG_UI_TIMER_SNOW_TICK_0;
		nuevo->img_button = IMG_UI_TIMER_SNOW_BUTTON_DEACTIVE;
	} else if (ui == UI_WATER) {
		nuevo->img_base = IMG_UI_TIMER_WATER_BASE;
		nuevo->img_ring = IMG_UI_TIMER_WATER_TICK_0;
		nuevo->img_button = IMG_UI_TIMER_WATER_BUTTON_DEACTIVE;
	}
	
	return nuevo;
};

void start_ticking (UITimer *timer) {
	timer->anim = 0;
	timer->estado = TIMER_TICKING;
}

void dibujar_timer (UITimer *timer) {
	SDL_Rect rect;
	int g;
	SDL_Event evento;
	int ring;
	Uint32 curtime;
	
	if (timer->estado == TIMER_SHOW) {
		g = timer->img_base;
		/* Dibujar el reloj base */
		if (timer->color == UI_WATER) {
			rect.x = TIMER_X - 106;
			rect.y = -70 + timer->anim - 7;
		} else if (timer->color == UI_SNOW) {
			rect.x = TIMER_X - 95;
			rect.y = -70 + timer->anim - 9;
		} else if (timer->color == UI_FIRE) {
			rect.x = TIMER_X - 102;
			rect.y = -70 + timer->anim;
		}
	
		SDL_QueryTexture (ui_timer_images[g], NULL, NULL, &rect.w, &rect.h);
		SDL_RenderCopy (renderer, ui_timer_images[g], NULL, &rect);
	
		g = timer->img_ring;
		if (timer->color == UI_WATER) {
			rect.x = TIMER_X - 76;
			rect.y = -70 + timer->anim + 6;
		} else if (timer->color == UI_SNOW) {
			rect.x = TIMER_X - 77;
			rect.y = -70 + timer->anim + 5;
		} else if (timer->color == UI_FIRE) {
			rect.x = TIMER_X - 70;
			rect.y = -70 + timer->anim + 10;
		}
	
		SDL_QueryTexture (ui_timer_images[g], NULL, NULL, &rect.w, &rect.h);
		SDL_RenderCopy (renderer, ui_timer_images[g], NULL, &rect);
	
		g = timer->img_button;
		rect.x = TIMER_X;
		rect.y = -70 + timer->anim + 29;
	
		SDL_QueryTexture (ui_timer_images[g], NULL, NULL, &rect.w, &rect.h);
		SDL_RenderCopy (renderer, ui_timer_images[g], NULL, &rect);
	
		timer->anim++;
		if (timer->anim >= 70) {
			timer->anim = 70;
		
			SDL_zero (evento);
			evento.type = UI_TIMER_EVENT;
			evento.user.code = UI_TIMER_EVENT_SHOW;
		
			SDL_PushEvent (&evento);
		}
	} else if (timer->estado == TIMER_TICKING) {
		if (timer->anim < 8) {
			/* Cuando el timer se rellena, reservar los cuadros */
			ring = timer->img_ring;
			timer->anim++;
			if (timer->anim == 8) {
				/* Tomar la hora actual */
				timer->timestamp = SDL_GetTicks ();
			}
		} else {
			curtime = SDL_GetTicks ();
			
			g = (curtime - timer->timestamp) / 3000;
			if (g >= 9) g = 9;
			ring = timer->img_ring + 9 - g;
			if (g == 9) {
				SDL_zero (evento);
				evento.type = UI_TIMER_EVENT;
				evento.user.code = UI_TIMER_EVENT_DONE_TICKS;
				
				SDL_PushEvent (&evento);
			}
		}
		
		g = timer->img_base;
		/* Dibujar el reloj base */
		if (timer->color == UI_WATER) {
			rect.x = TIMER_X - 106;
			rect.y = -7;
		} else if (timer->color == UI_SNOW) {
			rect.x = TIMER_X - 95;
			rect.y = -9;
		} else if (timer->color == UI_FIRE) {
			rect.x = TIMER_X - 102;
			rect.y = 0;
		}

		SDL_QueryTexture (ui_timer_images[g], NULL, NULL, &rect.w, &rect.h);
		SDL_RenderCopy (renderer, ui_timer_images[g], NULL, &rect);

		if (timer->color == UI_WATER) {
			rect.x = TIMER_X - 76;
			rect.y = 6;
		} else if (timer->color == UI_SNOW) {
			rect.x = TIMER_X - 77;
			rect.y = 5;
		} else if (timer->color == UI_FIRE) {
			rect.x = TIMER_X - 70;
			rect.y = 10;
		}
		
		SDL_QueryTexture (ui_timer_images[ring], NULL, NULL, &rect.w, &rect.h);
		SDL_RenderCopy (renderer, ui_timer_images[ring], NULL, &rect);

		g = timer->img_button;
		rect.x = TIMER_X;
		rect.y = 29;

		SDL_QueryTexture (ui_timer_images[g], NULL, NULL, &rect.w, &rect.h);
		SDL_RenderCopy (renderer, ui_timer_images[g], NULL, &rect);
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
	
	UI_TIMER_EVENT = SDL_RegisterEvents (1);
	
	if (UI_TIMER_EVENT == ((Uint32) -1)) {
		fprintf (stderr, _("Failed to register custom user events to SDL\n"
			"The error returned by SDL is:\n"
				"%s\n"), SDL_GetError());
		SDL_Quit ();
		exit (1);
	}
}

