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
#include "fire_ninja.h"
#include "snow_ninja.h"

#include "uitimer.h"

#include "netplay.h"

#define FPS (1000/30)

/* Enumerar las imágenes */
enum {
	IMG_INTRO_BACKGROUND,
	//IMG_INTRO_BACKGROUND_TUSK,
	
	IMG_INTRO_FIRE_NORMAL,
	IMG_INTRO_FIRE_OVER,
	IMG_INTRO_FIRE_SHADOW,
	
	IMG_INTRO_FIRE_SELECT_1,
	IMG_INTRO_FIRE_SELECT_2,
	IMG_INTRO_FIRE_SELECT_3,
	IMG_INTRO_FIRE_SELECT_4,
	IMG_INTRO_FIRE_SELECT_5,
	IMG_INTRO_FIRE_SELECT_6,
	IMG_INTRO_FIRE_SELECT_7,
	IMG_INTRO_FIRE_SELECT_8,
	IMG_INTRO_FIRE_SELECT_9,
	IMG_INTRO_FIRE_SELECT_10,
	IMG_INTRO_FIRE_SELECT_11,
	IMG_INTRO_FIRE_SELECT_12,
	IMG_INTRO_FIRE_SELECT_13,
	IMG_INTRO_FIRE_SELECT_14,
	IMG_INTRO_FIRE_SELECT_15,
	IMG_INTRO_FIRE_SELECT_16,
	
	IMG_INTRO_SNOW_NORMAL,
	IMG_INTRO_SNOW_OVER,
	IMG_INTRO_SNOW_SHADOW,
	
	IMG_INTRO_SNOW_SELECT_1,
	IMG_INTRO_SNOW_SELECT_2,
	IMG_INTRO_SNOW_SELECT_3,
	IMG_INTRO_SNOW_SELECT_4,
	IMG_INTRO_SNOW_SELECT_5,
	IMG_INTRO_SNOW_SELECT_6,
	IMG_INTRO_SNOW_SELECT_7,
	IMG_INTRO_SNOW_SELECT_8,
	IMG_INTRO_SNOW_SELECT_9,
	IMG_INTRO_SNOW_SELECT_10,
	IMG_INTRO_SNOW_SELECT_11,
	IMG_INTRO_SNOW_SELECT_12,
	IMG_INTRO_SNOW_SELECT_13,
	IMG_INTRO_SNOW_SELECT_14,
	IMG_INTRO_SNOW_SELECT_15,
	IMG_INTRO_SNOW_SELECT_16,
	
	IMG_INTRO_WATER_NORMAL,
	IMG_INTRO_WATER_OVER,
	IMG_INTRO_WATER_SHADOW,
	
	IMG_INTRO_WATER_SELECT_1,
	IMG_INTRO_WATER_SELECT_2,
	IMG_INTRO_WATER_SELECT_3,
	IMG_INTRO_WATER_SELECT_4,
	IMG_INTRO_WATER_SELECT_5,
	IMG_INTRO_WATER_SELECT_6,
	IMG_INTRO_WATER_SELECT_7,
	IMG_INTRO_WATER_SELECT_8,
	IMG_INTRO_WATER_SELECT_9,
	IMG_INTRO_WATER_SELECT_10,
	IMG_INTRO_WATER_SELECT_11,
	IMG_INTRO_WATER_SELECT_12,
	IMG_INTRO_WATER_SELECT_13,
	IMG_INTRO_WATER_SELECT_14,
	IMG_INTRO_WATER_SELECT_15,
	IMG_INTRO_WATER_SELECT_16,
	
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
	"images/intro/background.png",
	//"images/intro/background_tusk.png",
	
	"images/intro/fire_normal.png",
	"images/intro/fire_over.png",
	"images/intro/fire_shadow.png",
	
	"images/intro/fire_selected_01.png",
	"images/intro/fire_selected_02.png",
	"images/intro/fire_selected_03.png",
	"images/intro/fire_selected_04.png",
	"images/intro/fire_selected_05.png",
	"images/intro/fire_selected_06.png",
	"images/intro/fire_selected_07.png",
	"images/intro/fire_selected_08.png",
	"images/intro/fire_selected_09.png",
	"images/intro/fire_selected_10.png",
	"images/intro/fire_selected_11.png",
	"images/intro/fire_selected_12.png",
	"images/intro/fire_selected_13.png",
	"images/intro/fire_selected_14.png",
	"images/intro/fire_selected_15.png",
	"images/intro/fire_selected_16.png",
	
	"images/intro/snow_normal.png",
	"images/intro/snow_over.png",
	"images/intro/snow_shadow.png",
	
	"images/intro/snow_selected_01.png",
	"images/intro/snow_selected_02.png",
	"images/intro/snow_selected_03.png",
	"images/intro/snow_selected_04.png",
	"images/intro/snow_selected_05.png",
	"images/intro/snow_selected_06.png",
	"images/intro/snow_selected_07.png",
	"images/intro/snow_selected_08.png",
	"images/intro/snow_selected_09.png",
	"images/intro/snow_selected_10.png",
	"images/intro/snow_selected_11.png",
	"images/intro/snow_selected_12.png",
	"images/intro/snow_selected_13.png",
	"images/intro/snow_selected_14.png",
	"images/intro/snow_selected_15.png",
	"images/intro/snow_selected_16.png",
	
	"images/intro/water_normal.png",
	"images/intro/water_over.png",
	"images/intro/water_shadow.png",
	
	"images/intro/water_selected_01.png",
	"images/intro/water_selected_02.png",
	"images/intro/water_selected_03.png",
	"images/intro/water_selected_04.png",
	"images/intro/water_selected_05.png",
	"images/intro/water_selected_06.png",
	"images/intro/water_selected_07.png",
	"images/intro/water_selected_08.png",
	"images/intro/water_selected_09.png",
	"images/intro/water_selected_10.png",
	"images/intro/water_selected_11.png",
	"images/intro/water_selected_12.png",
	"images/intro/water_selected_13.png",
	"images/intro/water_selected_14.png",
	"images/intro/water_selected_15.png",
	"images/intro/water_selected_16.png",
	
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

/* Estatus de la interfaz */
enum {
	UI_SHOW_TIMER,
	UI_WAIT_INPUT,
	UI_WAITING_SERVER,
	UI_ANIMATE,
	
	NUM_UI
};

typedef struct {
	int local_ninja;
	
	int escenario[5][9];
	int acciones[5][9];
	int acciones_server[5][9];
	
	/* Poner aquí los ninjas y enemigos */
	FireNinja *fire;
	SnowNinja *snow;
	WaterNinja *water;
} SnowStage;

/* Prototipos de función */
int game_intro (SnowStage *stage);
int game_loop (SnowStage *stage);
int game_finish (void);
void setup (void);

int mouse_intro_penguin (int x, int y);

/* Variables globales */
SDL_Window *ventana;
SDL_Renderer *renderer;
int full_screen = 0;
SDL_Texture * images[NUM_IMAGES];
int use_sound;

SDL_Surface *penguin[3];

Mix_Chunk * sounds[NUM_SOUNDS];
Mix_Music * mus_carnie;

char nick_global[NICK_SIZE];
static int nick_default;

int main (int argc, char *argv[]) {
	int r;
	SnowStage stage;
	
	/* Recuperar las rutas del sistema */
	initSystemPaths (argv[0]);
	
	/* Inicializar l18n */
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, l10n_path);
	
	textdomain (PACKAGE);
	
	setup ();
	
	memset (nick_global, 0, sizeof (nick_global));
	
	/* Generar o leer el nick del archivo de configuración */
	r = RANDOM (65536);
	sprintf (nick_global, "Player%i", r);
	
	nick_default = 1;
	
	if (setup_netplay ("127.0.0.1", 3301) < 0) {
		printf ("Falló la inicializar la red\n");
		SDL_Quit ();
		return EXIT_FAILURE;
	}
	
	memset (&stage, 0, sizeof (stage));
	do {
		if (game_intro (&stage) == GAME_QUIT) break;
		if (game_loop (&stage) == GAME_QUIT) break;
		//if (game_finish () == GAME_QUIT) break;
	} while (1 == 0);
	
	SDL_Quit ();
	return EXIT_SUCCESS;
}

int game_intro (SnowStage *stage) {
	int done = 0;
	SDL_Event event;
	SDL_Keycode key;
	SDL_Rect rect;
	Uint32 last_time, now_time;
	int selected = -1, over = -1, g, h, f_anim;
	int anim[3];
	int others[3] = {-1, -1, -1};
	NinjaInfo *info;
	ObjectPos *objs;
	
	int ignore_network = FALSE;
	
	/* Predibujar todo */
	SDL_RenderCopy (renderer, images[IMG_INTRO_BACKGROUND], NULL, NULL);
	
	rect.x = -28;
	rect.y = 27;
	
	SDL_QueryTexture (images[IMG_INTRO_WATER_NORMAL], NULL, NULL, &rect.w, &rect.h);
	SDL_RenderCopy (renderer, images[IMG_INTRO_WATER_NORMAL], NULL, &rect);
	
	rect.x = 484;
	rect.y = 87;
	
	SDL_QueryTexture (images[IMG_INTRO_FIRE_NORMAL], NULL, NULL, &rect.w, &rect.h);
	SDL_RenderCopy (renderer, images[IMG_INTRO_FIRE_NORMAL], NULL, &rect);
	
	rect.x = 241;
	rect.y = 22;
	
	SDL_QueryTexture (images[IMG_INTRO_SNOW_NORMAL], NULL, NULL, &rect.w, &rect.h);
	SDL_RenderCopy (renderer, images[IMG_INTRO_SNOW_NORMAL], NULL, &rect);
	
	SDL_RenderPresent (renderer);
	
	do {
		last_time = SDL_GetTicks ();
		
		if (!ignore_network) process_network_events ();
		
		while (SDL_PollEvent(&event) > 0) {
			switch (event.type) {
				case SDL_QUIT:
					/* Vamos a cerrar la aplicación */
					done = GAME_QUIT;
					break;
				case SDL_MOUSEMOTION:
					if (selected == -1) {
						/* Calcular si el mouse está encima de algún pingüino */
						over = mouse_intro_penguin (event.motion.x, event.motion.y);
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_MOUSEBUTTONUP:
					if (selected == -1) {
						selected = mouse_intro_penguin (event.button.x, event.button.y);
						anim[selected] = 0;
						
						/* Enviar la selección al servidor */
						send_join (selected, nick_global);
						
						stage->local_ninja = selected;
					}
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
				default:
					if (event.type == NETWORK_EVENT) {
						switch (event.user.code) {
							case NETWORK_EVENT_CLOSE:
								/* TODO: Mostrar mensaje de error */
								done = GAME_QUIT;
								break;
							case NETWORK_EVENT_ACCEPT:
								/* El servidor nos aceptó, y posiblemente haya información de otros jugadores */
								h = GPOINTER_TO_INT (event.user.data1);
								info = (NinjaInfo *) event.user.data2;
								for (g = 0; g < h; g++) {
									others[info[g].elemento] = info[g].elemento;
									anim[info[g].elemento] = 0;
									
									/* TODO: Copiar el nick de los otros jugadores */
								}
								
								if (info != NULL) free (event.user.data2);
								break;
							case NETWORK_EVENT_JOIN_NINJA:
								info = (NinjaInfo *) event.user.data1;
								
								others[info->elemento] = info->elemento;
								anim[info->elemento] = 0;
								
								free (event.user.data1);
								break;
							case NETWORK_EVENT_START:
								h = GPOINTER_TO_INT (event.user.data1);
								objs = (ObjectPos *) event.user.data2;
								for (g = 0; g < h; g++) {
									stage->escenario[objs[g].y][objs[g].x] = objs[g].object;
								}
								
								free (objs);
								
								ignore_network = TRUE; /* Ya no procesar eventos de red */
								f_anim = 0;
								break;
						}
					}
			}
		}
		
		/* Borrar con el fondo */
		SDL_RenderCopy (renderer, images[IMG_INTRO_BACKGROUND], NULL, NULL);
		
		if (selected == UI_WATER || others[UI_WATER] != -1) {
			/* Correr la animación */
			rect.x = 0;
			rect.y = 0;
			g = anim[UI_WATER] + IMG_INTRO_WATER_SELECT_1;
			SDL_QueryTexture (images[g], NULL, NULL, &rect.w, &rect.h);
			SDL_RenderCopy (renderer, images[g], NULL, &rect);
			
			if (anim[UI_WATER] < 15) anim[UI_WATER]++;
		} else {
			g = (over == UI_WATER) ? IMG_INTRO_WATER_OVER : (selected == -1) ? IMG_INTRO_WATER_NORMAL : IMG_INTRO_WATER_SHADOW;
			rect.x = -28;
			rect.y = 27;
			
			SDL_QueryTexture (images[g], NULL, NULL, &rect.w, &rect.h);
			SDL_RenderCopy (renderer, images[g], NULL, &rect);
		}
		
		if (selected == UI_FIRE || others[UI_FIRE] != -1) {
			rect.x = 465;
			rect.y = 0;
			g = anim[UI_FIRE] + IMG_INTRO_FIRE_SELECT_1;
			SDL_QueryTexture (images[g], NULL, NULL, &rect.w, &rect.h);
			SDL_RenderCopy (renderer, images[g], NULL, &rect);
			
			if (anim[UI_FIRE] < 15) anim[UI_FIRE]++;
		} else {
			g = (over == UI_FIRE) ? IMG_INTRO_FIRE_OVER : (selected == -1) ? IMG_INTRO_FIRE_NORMAL : IMG_INTRO_FIRE_SHADOW;
			rect.x = 484;
			rect.y = 87;
			
			SDL_QueryTexture (images[g], NULL, NULL, &rect.w, &rect.h);
			SDL_RenderCopy (renderer, images[g], NULL, &rect);
		}
		
		if (selected == UI_SNOW || others[UI_SNOW] != -1) {
			rect.x = 231;
			rect.y = 0;
			g = anim[UI_SNOW] + IMG_INTRO_SNOW_SELECT_1;
			SDL_QueryTexture (images[g], NULL, NULL, &rect.w, &rect.h);
			SDL_RenderCopy (renderer, images[g], NULL, &rect);
			
			if (anim[UI_SNOW] < 15) anim[UI_SNOW]++;
		} else {
			g = (over == UI_SNOW) ? IMG_INTRO_SNOW_OVER : (selected == -1) ? IMG_INTRO_SNOW_NORMAL : IMG_INTRO_SNOW_SHADOW;
			rect.x = 241;
			rect.y = 22;
			
			SDL_QueryTexture (images[g], NULL, NULL, &rect.w, &rect.h);
			SDL_RenderCopy (renderer, images[g], NULL, &rect);
		}
		
		if (ignore_network) {
			/* Presentar la animación final */
			f_anim++;
			if (f_anim > 30) {
				done = GAME_CONTINUE;
			}
		}
		SDL_RenderPresent (renderer);
		
		now_time = SDL_GetTicks ();
		if (now_time < last_time + FPS) SDL_Delay(last_time + FPS - now_time);
		
	} while (!done);
	
	SDL_FreeSurface (penguin[0]);
	SDL_FreeSurface (penguin[1]);
	SDL_FreeSurface (penguin[2]);
	return done;
}

#if 0
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

int game_loop (SnowStage *stage) {
	int done = 0;
	SDL_Event event;
	SDL_Keycode key;
	Uint32 last_time, now_time;
	SDL_Rect rect, rect2;
	
	int g, h, i;
	UITimer *timer, *timer2, *timer3;
	
	int fondo;
	
	int local_ninja = NINJA_WATER;
	int ui_estatus = UI_SHOW_TIMER;
	
	//fondo = RANDOM(3); /* FIXME: El fondo también debe venir desde el server */
	fondo = 2;
	/* Crear los 3 ninjas */
	if (stage->escenario[0][0] == NINJA_WATER) {
		g = 0;
	} else if (stage->escenario[2][0] == NINJA_WATER) {
		g = 2;
	} else if (stage->escenario[4][0] == NINJA_WATER) {
		g = 4;
	}
	stage->water = create_water_ninja (0, g);
	
	if (stage->escenario[0][0] == NINJA_FIRE) {
		g = 0;
	} else if (stage->escenario[2][0] == NINJA_FIRE) {
		g = 2;
	} else if (stage->escenario[4][0] == NINJA_FIRE) {
		g = 4;
	}
	stage->fire = create_fire_ninja (0, g);
	
	if (stage->escenario[0][0] == NINJA_SNOW) {
		g = 0;
	} else if (stage->escenario[2][0] == NINJA_SNOW) {
		g = 2;
	} else if (stage->escenario[4][0] == NINJA_SNOW) {
		g = 4;
	}
	stage->snow = create_snow_ninja (0, g);
	
	timer = crear_timer (stage->local_ninja);
	
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
					
					/* FIXME: Motor de botones primero */
					if (ui_estatus == UI_WAIT_INPUT &&
					    event.button.x >= MAP_X && event.button.x < (MAP_X + 70 * 9) &&
					    event.button.y >= MAP_Y && event.button.y < (MAP_Y + 70 * 5)) {
						g = (event.button.x - MAP_X) / 70;
						h = (event.button.y - MAP_Y) / 70;
						
						if (stage->acciones[h][g] == ACTION_MOVE) {
							// Enviar el evento al servidor y esperar a que llegue para mostrarlo */
							if (stage->local_ninja == UI_WATER) {
								move_water (stage->water, g, h);
							} else if (stage->local_ninja == UI_FIRE) {
								move_fire (stage->fire, g, h);
							} else if (stage->local_ninja == UI_SNOW) {
								move_snow (stage->snow, g, h);
							}
						}
					}
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
					
					if (key == SDLK_z) {
						put_idle_snow (stage->snow);
					} else if (key == SDLK_a) {
						attack_snow (stage->snow);
					} else if (key == SDLK_s) {
						celebrate_snow (stage->snow);
					} else if (key == SDLK_d) {
						prev_move_snow (stage->snow);
					} else if (key == SDLK_f) {
						ko_snow (stage->snow);
					} else if (key == SDLK_g) {
						hit_snow (stage->snow);
					} else if (key == SDLK_h) {
						revive_snow (stage->snow);
					} else if (key == SDLK_j) {
						heal_snow (stage->snow);
					}
					break;
				default:
					if (event.type == UI_TIMER_EVENT) {
						switch (event.user.code) {
							case UI_TIMER_EVENT_SHOW:
								/* Como el reloj ya se mostró, enviar el evento al servidor de que estamos listos */
								ui_estatus = UI_WAIT_INPUT;
								start_ticking (timer);
								break;
							case UI_TIMER_EVENT_DONE_TICKS:
								ui_estatus = UI_WAITING_SERVER;
								break;
						}
					}
					break;
			}
		}
		/* Borrar con el fondo */
		SDL_RenderCopy (renderer, images[IMG_BACKGROUND_1 + fondo], NULL, NULL);
		
		if (ui_estatus == UI_WAIT_INPUT) {
			rect.x = MAP_X;
			rect.y = MAP_Y;
			SDL_QueryTexture (images[IMG_UI_FRAME], NULL, NULL, &rect.w, &rect.h);
			SDL_RenderCopy (renderer, images[IMG_UI_FRAME], NULL, &rect);
			
			/* Dibujar las posibles acciones */
			memset (stage->acciones, 0, sizeof (stage->acciones));
			if (stage->local_ninja == UI_FIRE) {
				ask_fire_actions (stage->fire, stage->escenario, stage->acciones);
			} else if (stage->local_ninja == UI_WATER) {
				ask_water_actions (stage->water, stage->escenario, stage->acciones);
			} else if (stage->local_ninja == UI_SNOW) {
				ask_snow_actions (stage->snow, stage->escenario, stage->acciones);
			}
			for (g = 0; g < 5; g++) {
				for (h = 0; h < 9; h++) {
					if (stage->acciones[g][h] & ACTION_MOVE) {
						rect.x = MAP_X + (h * 70);
						rect.y = MAP_Y + (g * 70);
						SDL_QueryTexture (images[IMG_UI_TILE_MOVE], NULL, NULL, &rect.w, &rect.h);
						SDL_RenderCopy (renderer, images[IMG_UI_TILE_MOVE], NULL, &rect);
					} else if (stage->acciones[g][h] & ACTION_CANT_MOVE) {
						rect.x = MAP_X + (h * 70);
						rect.y = MAP_Y + (g * 70);
						SDL_QueryTexture (images[IMG_UI_TILE_NO_MOVE], NULL, NULL, &rect.w, &rect.h);
						SDL_RenderCopy (renderer, images[IMG_UI_TILE_NO_MOVE], NULL, &rect);
					}
				}
			}
		}
		
		if (fondo == 0) {
			i = IMG_CRAG_ROCK;
		} else {
			i = IMG_ROCK;
		}
		
		/* Dibujar todos los objetos */
		for (g = 0; g < 5; g++) {
			for (h = 0; h < 9; h++) {
				if (stage->escenario[g][h] == NONE) continue;
				
				if (stage->escenario[g][h] == ROCK) {
					/* Dibujar la piedra */
					rect.x = MAP_X + (h * 70);
					rect.y = MAP_Y + (g * 70);
					SDL_QueryTexture (images[i], NULL, NULL, &rect.w, &rect.h);
					
					SDL_RenderCopy (renderer, images[i], NULL, &rect);
				} else if (stage->escenario[g][h] == NINJA_WATER) {
					draw_water_ninja (stage->water);
				} else if (stage->escenario[g][h] == NINJA_FIRE) {
					draw_fire_ninja (stage->fire);
				} else if (stage->escenario[g][h] == NINJA_SNOW) {
					draw_snow_ninja (stage->snow);
				}
			}
		}
		
		if (ui_estatus == UI_WAIT_INPUT) {
			draw_ghost_water_ninja (stage->water);
			draw_ghost_fire_ninja (stage->fire);
			draw_ghost_snow_ninja (stage->snow);
		}
		
		dibujar_timer (timer);
		
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
	
	int g, h;
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
	SDL_RenderSetLogicalSize (renderer, 800, 500);
	
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
		if (g == IMG_INTRO_FIRE_NORMAL || g == IMG_INTRO_SNOW_NORMAL || g == IMG_INTRO_WATER_NORMAL) {
			h = (g - IMG_INTRO_FIRE_NORMAL) / (IMG_INTRO_SNOW_NORMAL - IMG_INTRO_FIRE_NORMAL);
			penguin[h] = image;
		} else {
			SDL_FreeSurface (image);
		}
		/* TODO: Mostrar la carga de porcentaje */
	}
	
	setup_water_ninja ();
	setup_fire_ninja ();
	setup_snow_ninja ();
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

int mouse_intro_penguin (int x, int y) {
	Uint32 pixel;
	int bpp, pos;
	Uint8 r, g, b, a;
	
	bpp = 4;
	
	/* Revisar si el mouse está en el ninja de nieve */
	if (x >= 241 && x < 241 + penguin[UI_SNOW]->w &&
	    y >= 22 && y < 22 + penguin[UI_SNOW]->h) {
		pos = ((y - 22) * penguin[UI_SNOW]->pitch) / bpp + (x - 241);
		pixel = ((Uint32 *) penguin[UI_SNOW]->pixels)[pos];
		
		SDL_GetRGBA (pixel, penguin[UI_SNOW]->format, &r, &g, &b, &a);
		
		if (a != 0) {
			return UI_SNOW;
		}
	}
	
	if (x >= 484 && x < 484 + penguin[UI_FIRE]->w &&
	    y >= 87 && y < 87 + penguin[UI_FIRE]->h) {
		pos = ((y - 87) * penguin[UI_FIRE]->pitch) / bpp + (x - 484);
		pixel = ((Uint32 *) penguin[UI_FIRE]->pixels)[pos];
		
		SDL_GetRGBA (pixel, penguin[UI_FIRE]->format, &r, &g, &b, &a);
		
		if (a != 0) {
			return UI_FIRE;
		}
	}
	
	if (x >= 0 && x < -28 + penguin[UI_WATER]->w &&
	    y >= 27 && y < 27 + penguin[UI_WATER]->h) {
		pos = ((y - 27) * penguin[UI_WATER]->pitch) / bpp + (x + 28);
		pixel = ((Uint32 *) penguin[UI_WATER]->pixels)[pos];
		
		SDL_GetRGBA (pixel, penguin[UI_WATER]->format, &r, &g, &b, &a);
		
		if (a != 0) {
			return UI_WATER;
		}
	}
	
	return -1;
}

