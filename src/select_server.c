/*
 * select_server.c
 * This file is part of Card-Jitsu Snow
 *
 * Copyright (C) 2016 - Félix Arreola Rodríguez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <SDL.h>

#include <SDL_image.h>
#include <SDL_ttf.h>

/* Agrega algo de compatibilidad entre SDL 1 y SDL 2 */
#if SDL_MAJOR_VERSION == 1
#define SDL_GET_SIZE(I, W, H) do {\
	W = I->w; \
	H = I->h; \
	} while (0)
#define SDL_COMP_BLIT(IMG, SRCRECT, DEST, DSTRECT) SDL_BlitSurface (IMG, SRCRECT, DEST, DSTRECT);
#define SDL_Renderer SDL_Surface
#define SDL_Texture SDL_Surface
#define SDL_RendererPresent SDL_UpdateSurface
#else
#define SDL_GET_SIZE(I, W, H) SDL_QueryTexture (I, NULL, NULL, &W, &H)
#define SDL_COMP_BLIT(IMG, SRCRECT, DEST, DSTRECT) SDL_RenderCopy (DEST, IMG, SRCRECT, DSTRECT);
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>
#include "gettext.h"
#define _(string) gettext (string)

#include "select_server.h"
#include "path.h"

enum {
	IMG_SERVER_BACKGROUND,
	
	IMG_WORLD_BIG,
	IMG_WORLD_BIG_HOVER,
	IMG_WORLD_BIG_STAT_ON,
	IMG_WORLD_BIG_STAT_OFF,
	
	IMG_STAT_MINI_ONLINE,
	
	NUM_SERVER_IMAGES
};

static const char *images_server_names[NUM_SERVER_IMAGES] = {
	"images/server/background.png",
	
	"images/server/world_big.png",
	"images/server/world_big_hover.png",
	"images/server/world_big_pop_on.png",
	"images/server/world_big_pop_off.png",
	
	"images/server/online_mini.png",
};

enum {
	TEXT_SERVER_RECOMMEND,
	
	TEXT_MORE_SERVER,
	TEXT_CLICK_HERE,
	
	TEXT_AMOUNT,
	
	NUM_SERVER_TEXTS
};

static const char *text_server_strings [NUM_SERVER_TEXTS] = {
	gettext_noop ("YOUR SUGGESTED SERVERS"),
	gettext_noop ("MORE\nSERVERS"),
	gettext_noop ("CLICK HERE"),
	
	gettext_noop ("Amount of penguins online")
};

/* Variables Locales globales */
static SDL_Texture * images_server [NUM_SERVER_IMAGES];
static SDL_Texture * text_images_server [NUM_SERVER_TEXTS];
static TTF_Font *ttf16;

/* Nuestra función principal */
ServerInfo * select_server (SDL_Renderer *screen, ServerInfo *server_list, int servers, int recommended[5]) {
	int done = 0;
	SDL_Event event;
	Uint32 last_time, now_time;
	SDL_Rect rect;
	SDL_Rect update_rects[6];
	int num_rects;
	int tot_rec, g;
	int world_hover;
	
	SDL_Surface *image;
	SDL_Texture *rec_names[5];
	SDL_Color blanco = {0xFF, 0xFF, 0xFF};
	
	world_hover = -1;
	
	/* Ver cuántos servidores recomendados hay */
	g = 0;
	tot_rec = 0;
	while (g < 5 && recommended[g] != -1) {
		g++;
		tot_rec++;
	}
	
	/* Prerenderizar los nombres de los servidores */
	for (g = 0; g < tot_rec; g++) {
		image = TTF_RenderUTF8_Blended (ttf16, server_list[recommended[g]].name, blanco);
		
#if SDL_MAJOR_VERSION == 2
		rec_names[g] = SDL_CreateTextureFromSurface (screen, images[g]);
		SDL_FreeSurface (image);
#else
		rec_names[g] = image;
#endif
	}
	
	rect.x = 0;
	rect.y = 0;
	SDL_GET_SIZE (images[IMG_SERVER_BACKGROUND], rect.w, rect.h);
	SDL_COMP_BLIT (images[IMG_SERVER_BACKGROUND], NULL, screen, &rect);
	
	/* Copiar el texto de "servidores recomendados" */
	SDL_GET_SIZE (text_images_server[TEXT_SERVER_RECOMMEND], rect.w, rect.h);
	rect.x = 92 + (632 - rect.w) / 2;
	rect.y = 30;
	SDL_COMP_BLIT (text_images_server[TEXT_SERVER_RECOMMEND], NULL, screen, &rect);
	
	for (g = 0; g < tot_rec; g++) {
		rect.x = 187;
		rect.y = 91 + (51 * g);
		SDL_GET_SIZE (images[IMG_WORLD_BIG], rect.w, rect.h);
		SDL_COMP_BLIT (images[IMG_WORLD_BIG], NULL, screen, &rect);
		
		/* Escribir los nombres en pantalla */
		rect.x = 29;
		rect.y = 94 + (51 * g);
		SDL_GET_SIZE (rec_names[g], rect.w, rect.h);
		SDL_COMP_BLIT (rec_names[g], NULL, screen, &rect);
	}
	
}

void setup_select_server (void) {
	SDL_Surface * image;
	SDL_Texture * texture;
	
	int g, h;
	char buffer_file[8192];
	TTF_Font *ttf20, *ttf14;
	
	SDL_Color azul = {0, 0x33, 0x66}, blanco = {0xFF, 0xFF, 0xFF};
	
	for (g = 0; g < NUM_SERVER_IMAGES; g++) {
		sprintf (buffer_file, "%s%s", systemdata_path, images_server_names[g]);
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
#if SDL_MAJOR_VERSION == 2
		texture = SDL_CreateTextureFromSurface (renderer, image);
#else
		texture = image;
#endif
		images[g] = texture;
	}
	
	if (TTF_Init () < 0) {
		fprintf (stderr,
			_("Error: Can't initialize the SDL TTF library\n"
			"%s\n"), TTF_GetError ());
		SDL_Quit ();
		exit (1);
	}
	
	sprintf (buffer_file, "%s%s", systemdata_path, "comic crazy.ttf");
	ttf20 = TTF_OpenFont (buffer_file, 20);
	
	if (!ttf20) {
		fprintf (stderr,
			_("Failed to load font file 'Comic Crazy'\n"
			"The error returned by SDL is:\n"
			"%s\n"), TTF_GetError ());
		SDL_Quit ();
		exit (1);
	}
	
	/* Pre-renderizar los textos */
	
	TTF_CloseFont (ttf20);
	
	sprintf (buffer_file, "%s%s", systemdata_path, "burbanksb.ttf");
	ttf16 = TTF_OpenFont (buffer_file, 16);
	
	if (!ttf16) {
		fprintf (stderr,
			_("Failed to load font file 'Burbank Small Bold'\n"
			"The error returned by SDL is:\n"
			"%s\n"), TTF_GetError ());
		SDL_Quit ();
		exit (1);
	}
	
	
}
