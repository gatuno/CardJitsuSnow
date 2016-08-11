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
#define SDL_Texture SDL_Surface
#define SDL_RendererPresent SDL_UpdateSurface
#define SDL_DestroyTexture SDL_FreeSurface
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
#include "draw-text.h"
#include "path.h"
#include "rotar.h"

#define FPS (1000/24)

enum {
	IMG_SERVER_BACKGROUND,
	
	IMG_WORLD_BIG,
	IMG_WORLD_BIG_HOVER,
	IMG_WORLD_BIG_STAT_ON,
	IMG_WORLD_BIG_STAT_OFF,
	
	IMG_MORE_SERVER,
	IMG_STAT_MINI_ONLINE,
	
	NUM_SERVER_IMAGES
};

static const char *images_server_names[NUM_SERVER_IMAGES] = {
	"server/images/background.png",
	
	"server/images/world_big.png",
	"server/images/world_big_hover.png",
	"server/images/world_big_pop_on.png",
	"server/images/world_big_pop_off.png",
	
	"server/images/more_server.png",
	"server/images/online_mini.png"
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

/* Prototipos locales */
static int mouse_motion_world_big (int x, int y);

static void draw_world_big (SDL_Renderer *screen, SDL_Texture *name, ServerInfo *world, int pos, int hover) {
	SDL_Rect rect;
	int g, h, i;
	
	if (hover) {
		i = IMG_WORLD_BIG_HOVER;
	} else {
		i = IMG_WORLD_BIG;
	}
	rect.x = 207;
	rect.y = 91 + (51 * pos);
	SDL_GET_SIZE (images_server[i], rect.w, rect.h);
	SDL_COMP_BLIT (images_server[i], NULL, screen, &rect);

	/* Escribir los nombres en pantalla */
	rect.x = 207 + 29;
	rect.y = 104 + (51 * pos);
	SDL_GET_SIZE (name, rect.w, rect.h);
	SDL_COMP_BLIT (name, NULL, screen, &rect);

	/* El primer stat siempre prendido */
	rect.x = 207 + 247;
	rect.y = 101 + (51 * pos);
	SDL_GET_SIZE (images_server[IMG_WORLD_BIG_STAT_ON], rect.w, rect.h);
	SDL_COMP_BLIT (images_server[IMG_WORLD_BIG_STAT_ON], NULL, screen, &rect);

	h = world->population * 100 / world->max_population;
	h = (h + 5) / 20;
	for (g = 0; g < 4; g++) {
		if (g + 2 <= h) {
			i = IMG_WORLD_BIG_STAT_ON;
		} else {
			i = IMG_WORLD_BIG_STAT_OFF;
		}
	
		rect.x = 207 + 247 + 24 + (24 * g);
		rect.y = 101 + (51 * pos);
		SDL_GET_SIZE (images_server[i], rect.w, rect.h);
		SDL_COMP_BLIT (images_server[i], NULL, screen, &rect);
	}
}

/* Nuestra función principal */
ServerInfo * select_server (SDL_Renderer *screen, ServerInfo *server_list, int servers, int recommended[5]) {
	int done = 0;
	SDL_Event event;
	Uint32 last_time, now_time;
	SDL_Rect rect;
	SDL_Rect update_rects[6];
	int num_rects;
	int tot_rec, g, h, i, j;
	int world_hover, map;
	ServerInfo *selected_server = NULL;
	
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
		rec_names[g] = SDL_CreateTextureFromSurface (screen, image);
		SDL_FreeSurface (image);
#else
		rec_names[g] = image;
#endif
	}
	
	rect.x = 0;
	rect.y = 0;
	SDL_GET_SIZE (images_server[IMG_SERVER_BACKGROUND], rect.w, rect.h);
	SDL_COMP_BLIT (images_server[IMG_SERVER_BACKGROUND], NULL, screen, &rect);
	
	/* Copiar el texto de "servidores recomendados" */
	SDL_GET_SIZE (text_images_server[TEXT_SERVER_RECOMMEND], rect.w, rect.h);
	rect.x = (800 - rect.w) / 2;
	rect.y = 30;
	SDL_COMP_BLIT (text_images_server[TEXT_SERVER_RECOMMEND], NULL, screen, &rect);
	
	for (g = 0; g < tot_rec; g++) {
		draw_world_big (screen, rec_names[g], &server_list[recommended[g]], g, 0);
	}
	
	/* Dibujar la nota */
	rect.x = 610;
	rect.y = 253;
	SDL_GET_SIZE (images_server[IMG_MORE_SERVER], rect.w, rect.h);
	SDL_COMP_BLIT (images_server[IMG_MORE_SERVER], NULL, screen, &rect);
	
	SDL_GET_SIZE (text_images_server[TEXT_MORE_SERVER], rect.w, rect.h);
	rect.x = 616 + (130 - rect.w) / 2;
	rect.y = 268;
	SDL_COMP_BLIT (text_images_server[TEXT_MORE_SERVER], NULL, screen, &rect);
	
	SDL_GET_SIZE (text_images_server[TEXT_CLICK_HERE], rect.w, rect.h);
	rect.x = 620 + (140 - rect.w) / 2;
	rect.y = 320;
	SDL_COMP_BLIT (text_images_server[TEXT_CLICK_HERE], NULL, screen, &rect);
	
	/* Dibujar la otra info */
	rect.x = 276 + 20;
	rect.y = 448;
	SDL_GET_SIZE (images_server[IMG_STAT_MINI_ONLINE], rect.w, rect.h);
	SDL_COMP_BLIT (images_server[IMG_STAT_MINI_ONLINE], NULL, screen, &rect);
	
	rect.x = 292 + 20;
	rect.y = 450;
	SDL_GET_SIZE (text_images_server[TEXT_AMOUNT], rect.w, rect.h);
	SDL_COMP_BLIT (text_images_server[TEXT_AMOUNT], NULL, screen, &rect);
	
	world_hover = -1;
	
	SDL_RenderPresent (screen);
	
	do {
		last_time = SDL_GetTicks ();
		
		while (SDL_PollEvent(&event) > 0) {
			switch (event.type) {
				case SDL_QUIT:
					/* Vamos a cerrar la aplicación */
					done = 1;
					break;
				case SDL_MOUSEMOTION:
					map = mouse_motion_world_big (event.motion.x, event.motion.y);
					
					if (world_hover != map && map < tot_rec) {
						if (world_hover != -1) draw_world_big (screen, rec_names[world_hover], &server_list[recommended[world_hover]], world_hover, 0);
						if (map != -1) draw_world_big (screen, rec_names[map], &server_list[recommended[map]], map, 1);
						world_hover = map;
					}
					break;
				case SDL_MOUSEBUTTONUP:
					if (event.button.button != SDL_BUTTON_LEFT) break;
					
					map = mouse_motion_world_big (event.button.x, event.button.y);
					
					if (map != -1 && map < tot_rec) {
						/* Eligió un servidor, retornar la información */
						selected_server = &server_list[recommended[map]];
						done = 1;
					}
					break;
			}
		}
		
		SDL_RenderPresent (screen);
		
		now_time = SDL_GetTicks ();
		if (now_time < last_time + FPS) SDL_Delay(last_time + FPS - now_time);
	} while (!done);
	
	for (g = 0; g < tot_rec; g++) {
		SDL_DestroyTexture (rec_names[g]);
	}
	
	/* Cerrar la tipografía, ya no la vamos a usar */
	TTF_CloseFont (ttf16);
	
	return selected_server;
}

void setup_select_server (SDL_Renderer *renderer) {
	SDL_Surface * image, *image2;
	SDL_Texture * texture;
	
	int g, h;
	char buffer_file[8192];
	TTF_Font *ttf20, *ttf14;
	
	SDL_Color azul = {0, 0x33, 0x66}, blanco = {0xFF, 0xFF, 0xFF}, cafe = {0x54, 0x3A, 0x01};
	
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
		SDL_FreeSurface (image);
#else
		texture = image;
#endif
		images_server[g] = texture;
	}
	
	if (TTF_Init () < 0) {
		fprintf (stderr,
			_("Error: Can't initialize the SDL TTF library\n"
			"%s\n"), TTF_GetError ());
		SDL_Quit ();
		exit (1);
	}
	
	sprintf (buffer_file, "%s%s", systemdata_path, "server/comicrazy.ttf");
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
	TTF_SetFontOutline (ttf20, 3);
	image = TTF_RenderUTF8_Blended (ttf20, gettext (text_server_strings [TEXT_SERVER_RECOMMEND]), azul);
	
	TTF_SetFontOutline (ttf20, 0);
	image2 = TTF_RenderUTF8_Blended (ttf20, gettext (text_server_strings [TEXT_SERVER_RECOMMEND]), blanco);
	
	SDL_Rect rect;
	rect.x = 3;
	rect.y = 3;
	rect.w = image2->w;
	rect.h = image2->h;
	SDL_BlitSurface (image2, NULL, image, &rect);
	
	SDL_FreeSurface (image2);
#if SDL_MAJOR_VERSION == 2
	texture = SDL_CreateTextureFromSurface (renderer, image);
	SDL_FreeSurface (image);
#else
	texture = image;
#endif
	
	text_images_server[TEXT_SERVER_RECOMMEND] = texture;
	
	/* El texto de "Más servidores" */
	image = draw_text (ttf20, gettext (text_server_strings [TEXT_MORE_SERVER]), cafe, ALIGN_CENTER, 0);
	
	image2 = rotozoomSurface (image, 9.0, 1);
	SDL_FreeSurface (image);
#if SDL_MAJOR_VERSION == 2
	texture = SDL_CreateTextureFromSurface (renderer, image2);
	SDL_FreeSurface (image2);
#else
	texture = image2;
#endif
	text_images_server[TEXT_MORE_SERVER] = texture;
	
	TTF_CloseFont (ttf20);
	
	/* Abrir la tipografía en tamaño 14 */
	sprintf (buffer_file, "%s%s", systemdata_path, "server/burbanksb.ttf");
	ttf14 = TTF_OpenFont (buffer_file, 14);
	if (!ttf14) {
		fprintf (stderr,
			_("Failed to load font file 'Burbank Small Bold'\n"
			"The error returned by SDL is:\n"
			"%s\n"), TTF_GetError ());
		SDL_Quit ();
		exit (1);
	}
	
	/* Pre renderizar los textos */
	image = TTF_RenderUTF8_Blended (ttf14, gettext (text_server_strings [TEXT_AMOUNT]), blanco);
#if SDL_MAJOR_VERSION == 2
	texture = SDL_CreateTextureFromSurface (renderer, image);
	SDL_FreeSurface (image);
#else
	texture = image;
#endif
	text_images_server[TEXT_AMOUNT] = texture;
	TTF_CloseFont (ttf14);
	
	sprintf (buffer_file, "%s%s", systemdata_path, "server/comicrazy.ttf");
	ttf14 = TTF_OpenFont (buffer_file, 14);
	
	if (!ttf14) {
		fprintf (stderr,
			_("Failed to load font file 'Comic Crazy'\n"
			"The error returned by SDL is:\n"
			"%s\n"), TTF_GetError ());
		SDL_Quit ();
		exit (1);
	}
	
	/* El texto de "Click Here" */
	image = TTF_RenderUTF8_Blended (ttf14, gettext (text_server_strings [TEXT_CLICK_HERE]), cafe);
	image2 = rotozoomSurface (image, 9.0, 1);
	SDL_FreeSurface (image);
#if SDL_MAJOR_VERSION == 2
	texture = SDL_CreateTextureFromSurface (renderer, image2);
	SDL_FreeSurface (image2);
#else
	texture = image2;
#endif
	text_images_server[TEXT_CLICK_HERE] = texture;
	
	TTF_CloseFont (ttf14);
	
	sprintf (buffer_file, "%s%s", systemdata_path, "server/burbanksb.ttf");
	ttf16 = TTF_OpenFont (buffer_file, 24);
	
	if (!ttf16) {
		fprintf (stderr,
			_("Failed to load font file 'Burbank Small Bold'\n"
			"The error returned by SDL is:\n"
			"%s\n"), TTF_GetError ());
		SDL_Quit ();
		exit (1);
	}
}

void unload_select_server (void) {
	int g;
	
	/* Liberar todos los recursos usados */
	for (g = 0; g < NUM_SERVER_IMAGES; g++) {
		SDL_DestroyTexture (images_server[g]);
	}
	
	for (g = 0; g < NUM_SERVER_TEXTS; g++) {
		SDL_DestroyTexture (text_images_server[g]);
	}
}

static int mouse_motion_world_big (int x, int y) {
	if (x >= 207 && x < 207 + 388 &&
	    y >= 91 && y < 91 + 255) {
		if ((y - 91) % 51 < 47) {
			return ((y - 91) / 51);
		}
	}
	
	return -1;
}

