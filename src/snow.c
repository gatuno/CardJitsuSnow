/*
 * snow.c
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
 * along with Card-Jitsu Snow; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
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
#include "uitimer.h"

#define FPS (1000/30)

#define RANDOM(x) ((int) (x ## .0 * rand () / (RAND_MAX + 1.0)))

/* Enumerar las imágenes */
enum {
	IMG_BACKGROUND_1,
	IMG_BACKGROUND_2,
	IMG_BACKGROUND_3,
	
	IMG_FOREGROUND_1,
	IMG_FOREGROUND_2,
	
	IMG_CRAG_ROCK,
	IMG_ROCK,
	
	IMG_UI_FIRE_CARDS,
	IMG_UI_SNOW_CARDS,
	IMG_UI_WATER_CARDS,
	
	IMG_UI_FRAME,
	IMG_UI_TILE_ATTACK,
	IMG_UI_TILE_HEAL,
	IMG_UI_TILE_MOVE,
	IMG_UI_TILE_NO_MOVE,
	
	NUM_IMAGES
};

/* Los nombres de archivos */
const char *images_names[NUM_IMAGES] = {
	"images/background_1.png",
	"images/background_2.png",
	"images/background_3.png",
	
	"images/foreground_1.png",
	"images/foreground_2.png",
	
	"images/crag_rock.png",
	"images/rock.png",
	
	"images/ui_fire_cards.png",
	"images/ui_snow_cards.png",
	"images/ui_water_cards.png",
	
	"images/frame.png",
	"images/tile_attack.png",
	"images/tile_heal.png",
	"images/tile_move.png",
	"images/tile_no_move.png"
};

enum {
	//SND_NONE,
	
	NUM_SOUNDS
};

const char *sound_names[NUM_SOUNDS] = {
	//"sounds/none.wav",
};

/* Codigos de salida */
enum {
	GAME_NONE = 0, /* No usado */
	GAME_CONTINUE,
	GAME_QUIT
};

/* Objetos */
enum {
	NONE = 0,
	
	NINJA_FIRE,
	NINJA_SNOW,
	NINJA_WATER,
	
	ROCK,
	
	NUM_OBJECTS
};

/* Acciones */
enum {
	NO_ACTION = 0,
	MOVE,
	CANT_MOVE,
	
	NINJA_FIRE_MOVE,
	NINJA_SNOW_MOVE,
	NINJA_WATER_MOVE,
	
	NUM_ACTIONS
};

/* Estatus de la interfaz */
enum {
	UI_SHOW_TIMER,
	UI_WAIT_INPUT,
	UI_HIDE_TIMER,
	UI_WAITING_SERVER,
	UI_ANIMATE,
	
	NUM_UI
};

/* Prototipos de función */
int game_intro (void);
int game_loop (void);
int game_finish (void);
void setup (void);

/* Variables globales */
SDL_Window *ventana;
SDL_Renderer *renderer;
int full_screen = 0;
SDL_Texture * images[NUM_IMAGES];
int use_sound;

Mix_Chunk * sounds[NUM_SOUNDS];
Mix_Music * mus_carnie;

int main (int argc, char *argv[]) {
	/* Recuperar las rutas del sistema */
	initSystemPaths (argv[0]);
	
	/* Inicializar l18n */
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, l10n_path);
	
	textdomain (PACKAGE);
	
	setup ();
	do {
		//if (game_intro () == GAME_QUIT) break;
		if (game_loop () == GAME_QUIT) break;
		//if (game_finish () == GAME_QUIT) break;
	} while (1 == 0);
	
	SDL_Quit ();
	return EXIT_SUCCESS;
}

#if 0
int game_intro (void) {
	int done = 0;
	SDL_Event event;
	SDL_Keycode key;
	SDL_Rect rect;
	Uint32 last_time, now_time;
	
	/* Predibujar todo */
	SDL_RenderClear (renderer);
	SDL_RenderPresent (renderer);
	
	do {
		last_time = SDL_GetTicks ();
		
		while (SDL_PollEvent(&event) > 0) {
			switch (event.type) {
				case SDL_QUIT:
					/* Vamos a cerrar la aplicación */
					done = GAME_QUIT;
					break;
				case SDL_MOUSEMOTION:
					break;
				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_MOUSEBUTTONUP:
					break;
				case SDL_KEYDOWN:
					/* Tengo una tecla presionada */
					key = event.key.keysym.sym;
					
					if (key == SDLK_F11 || (key == SDLK_RETURN && (event.key.keysym.mod & KMOD_ALT))) {
						if (full_screen == 0) {
							full_screen = SDL_WINDOW_FULLSCREEN;
						} else {
							full_screen = 0;
						}
						SDL_SetWindowFullscreen (ventana, full_screen);
					}
					if (key == SDLK_ESCAPE) {
						done = GAME_QUIT;
					}
					break;
			}
		}
		
		SDL_RenderPresent (renderer);
		
		now_time = SDL_GetTicks ();
		if (now_time < last_time + FPS) SDL_Delay(last_time + FPS - now_time);
		
	} while (!done);
	
	return done;
}

int game_finish (void) {
	int done = 0;
	SDL_Event event;
	SDL_Keycode key;
	SDL_Rect rect;
	Uint32 last_time, now_time;
	
	/* Predibujar todo */
	SDL_RenderClear (renderer);
	SDL_RenderPresent (renderer);
	
	do {
		last_time = SDL_GetTicks ();
		
		while (SDL_PollEvent(&event) > 0) {
			/* fprintf (stdout, "Evento: %i\n", event.type);*/
			switch (event.type) {
				case SDL_QUIT:
					/* Vamos a cerrar la aplicación */
					done = GAME_QUIT;
					break;
				case SDL_MOUSEMOTION:
					break;
				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_MOUSEBUTTONUP:
					break;
				case SDL_KEYDOWN:
					/* Tengo una tecla presionada */
					key = event.key.keysym.sym;
					
					if (key == SDLK_F11 || (key == SDLK_RETURN && (event.key.keysym.mod & KMOD_ALT))) {
						if (full_screen == 0) {
							full_screen = SDL_WINDOW_FULLSCREEN;
						} else {
							full_screen = 0;
						}
						SDL_SetWindowFullscreen (ventana, full_screen);
					}
					if (key == SDLK_ESCAPE) {
						done = GAME_QUIT;
					}
					break;
			}
		}
		
		SDL_RenderPresent (renderer);
		
		now_time = SDL_GetTicks ();
		if (now_time < last_time + FPS) SDL_Delay(last_time + FPS - now_time);
		
	} while (!done);
	
	return done;
}
#endif

int game_loop (void) {
	int done = 0;
	SDL_Event event;
	SDL_Keycode key;
	Uint32 last_time, now_time;
	SDL_Rect rect, rect2;
	Uint32 input_time;
	
	int g, h, i;
	UITimer *timer, *timer2, *timer3;
	
	int fondo;
	WaterNinja *water;
	int escenario[5][9];
	int acciones[5][9];
	
	int local_ninja = NINJA_WATER;
	int ui_estatus = UI_SHOW_TIMER;
	memset (escenario, 0, sizeof (escenario));
	memset (acciones, 0, sizeof (acciones));
	
	escenario[0][2] = escenario[0][6] = escenario[4][2] = escenario[4][6] = ROCK;
	
	fondo = RANDOM(3);
	water = crear_water_ninja (0, 0);
	escenario[0][0] = NINJA_WATER;
	timer = crear_timer (UI_WATER);
	timer2 = crear_timer (UI_SNOW);
	timer3 = crear_timer (UI_FIRE);
	
	/* Predibujar todo */
	SDL_RenderClear (renderer);
	SDL_RenderPresent (renderer);
	
	do {
		last_time = SDL_GetTicks ();
		
		while (SDL_PollEvent(&event) > 0) {
			switch (event.type) {
				case SDL_QUIT:
					/* Vamos a cerrar la aplicación */
					done = GAME_QUIT;
					break;
				case SDL_MOUSEBUTTONDOWN:
					/* Tengo un Mouse Down */
					break;
				case SDL_MOUSEBUTTONUP:
					/* Tengo un mouse Up */
					break;
				case SDL_KEYDOWN:
					/* Tengo una tecla presionada */
					key = event.key.keysym.sym;
					
					if (key == SDLK_F11 || (key == SDLK_RETURN && (event.key.keysym.mod & KMOD_ALT))) {
						if (full_screen == 0) {
							full_screen = SDL_WINDOW_FULLSCREEN;
						} else {
							full_screen = 0;
						}
						SDL_SetWindowFullscreen (ventana, full_screen);
					}
					if (key == SDLK_ESCAPE) {
						done = GAME_QUIT;
					}
					
					if (key == SDLK_a) {
						attack_water (water);
					} else if (key == SDLK_s) {
						celebrate_water (water);
					} else if (key == SDLK_d) {
						move_water (water);
					} else if (key == SDLK_f) {
						ko_water (water);
					} else if (key == SDLK_g) {
						hit_water (water);
					} else if (key == SDLK_h) {
						revive_water (water);
					} else if (key == SDLK_DOWN) {
						add_water_offset (water, 0, 1);
					} else if (key == SDLK_UP) {
						add_water_offset (water, 0, -1);
					} else if (key == SDLK_LEFT) {
						add_water_offset (water, -1, 0);
					} else if (key == SDLK_RIGHT) {
						add_water_offset (water, 1, 0);
					}
					break;
			}
		}
		/* Borrar con el fondo */
		SDL_RenderCopy (renderer, images[IMG_BACKGROUND_1 + fondo], NULL, NULL);
		
		rect.x = MAP_X;
		rect.y = MAP_Y;
		SDL_QueryTexture (images[IMG_UI_FRAME], NULL, NULL, &rect.w, &rect.h);
		SDL_RenderCopy (renderer, images[IMG_UI_FRAME], NULL, &rect);
		
		if (fondo == 0) {
			i = IMG_CRAG_ROCK;
		} else {
			i = IMG_ROCK;
		}
		
		/* Dibujar todos los objetos */
		for (g = 0; g < 5; g++) {
			for (h = 0; h < 9; h++) {
				if (escenario[g][h] == NONE) continue;
				
				if (escenario[g][h] == ROCK) {
					/* Dibujar la piedra */
					rect.x = MAP_X + (h * 70);
					rect.y = MAP_Y + (g * 70);
					SDL_QueryTexture (images[i], NULL, NULL, &rect.w, &rect.h);
					
					SDL_RenderCopy (renderer, images[i], NULL, &rect);
				} else if (escenario[g][h] == NINJA_WATER) {
					dibujar_water (water);
				}
			}
		}
		
		dibujar_timer (timer);
		dibujar_timer (timer2);
		dibujar_timer (timer3);
		
		/* Dibujar el frente */
		if (fondo == 0) {
			rect.x = 0;
			rect.y = 298;
			rect.w = 800;
			rect.h = 202;
			
			SDL_RenderCopy (renderer, images[IMG_FOREGROUND_1], NULL, &rect);
		} else if (fondo == 1) {
			SDL_RenderCopy (renderer, images[IMG_FOREGROUND_2], NULL, NULL);
		}
		SDL_RenderPresent (renderer);
		
		now_time = SDL_GetTicks ();
		if (now_time < last_time + FPS) SDL_Delay(last_time + FPS - now_time);
	} while (!done);
	
	return done;
}

void setup (void) {
	SDL_Surface * image;
	SDL_Texture * texture;
	
	int g;
	char buffer_file[8192];
	
	/* Inicializar el Video SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf (stderr,
			_("Error: Can't initialize the video subsystem\n"
			"The error returned by SDL is:\n"
			"%s\n"), SDL_GetError());
		exit (1);
	}
	
	ventana = SDL_CreateWindow (_("Card-Jitsu Snow"), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 500, 0);
	
	if (ventana == NULL) {
		fprintf (stderr,
			_("Error: Can't setup 760x480 video mode.\n"
			"The error returned by SDL is:\n"
			"%s\n"), SDL_GetError());
		SDL_Quit ();
		exit (1);
	}
	
	sprintf (buffer_file, "%simages/icon.png", systemdata_path);
	image = IMG_Load (buffer_file);
	if (image) {
		SDL_SetWindowIcon (ventana, image);
		SDL_FreeSurface (image);
	}
	
	renderer = SDL_CreateRenderer (ventana, -1, SDL_RENDERER_PRESENTVSYNC);
	
	if (renderer == NULL) {
		renderer = SDL_CreateRenderer (ventana, -1, SDL_RENDERER_SOFTWARE);
		
		if (renderer == NULL) {
			fprintf (stderr,
				_("Error: Can't create SDL_Renderer.\n"
				"The error returned by SDL is:\n"
				"%s\n"), SDL_GetError());
			SDL_Quit ();
			exit (1);
		}
	}
	
	SDL_SetRenderDrawColor (renderer, 0, 0, 0, 255);
	
	use_sound = 1;
	if (SDL_InitSubSystem (SDL_INIT_AUDIO) < 0) {
		fprintf (stdout,
			_("Warning: Can't initialize the audio subsystem\n"
			"Continuing...\n"));
		use_sound = 0;
	}
	
	if (use_sound) {
		/* Inicializar el sonido */
		if (Mix_OpenAudio (22050, AUDIO_S16, 2, 4096) < 0) {
			fprintf (stdout,
				_("Warning: Can't initialize the SDL Mixer library\n"));
			use_sound = 0;
		}
	}
	
	for (g = 0; g < NUM_IMAGES; g++) {
		sprintf (buffer_file, "%s%s", systemdata_path, images_names[g]);
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
		images[g] = texture;
		SDL_FreeSurface (image);
		/* TODO: Mostrar la carga de porcentaje */
	}
	
	setup_water_ninja ();
	setup_timer ();
	
	if (use_sound) {
		for (g = 0; g < NUM_SOUNDS; g++) {
			sprintf (buffer_file, "%s%s", systemdata_path, sound_names[g]);
			sounds[g] = Mix_LoadWAV (buffer_file);
			
			if (sounds[g] == NULL) {
				fprintf (stderr,
					_("Failed to load data file:\n"
					"%s\n"
					"The error returned by SDL is:\n"
					"%s\n"), buffer_file, SDL_GetError ());
				SDL_Quit ();
				exit (1);
			}
			Mix_VolumeChunk (sounds[g], MIX_MAX_VOLUME / 2);
		}
		
		/* Cargar la música */
		//sprintf (buffer_file, "%s%s", systemdata_path, MUS_CARNIE);
		//mus_carnie = Mix_LoadMUS (buffer_file);
		
		/*if (mus_carnie == NULL) {
			fprintf (stderr,
				_("Failed to load data file:\n"
				"%s\n"
				"The error returned by SDL is:\n"
				"%s\n"), buffer_file, SDL_GetError ());
			SDL_Quit ();
			exit (1);
		}*/
	}
	
	if (TTF_Init () < 0) {
		fprintf (stderr,
			_("Error: Can't initialize the SDL TTF library\n"
			"%s\n"), TTF_GetError ());
		SDL_Quit ();
		exit (1);
	}
	
	/* Generador de números aleatorios */
	srand (SDL_GetTicks ());
}

