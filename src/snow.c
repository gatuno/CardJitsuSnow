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

#include "ninja.h"
#include "enemy.h"
#include "uitimer.h"
#include "netplay.h"
#include "cp-button.h"

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
	
	IMG_ROUND_1,
	IMG_ROUND_2,
	IMG_ROUND_3,
	
	IMG_CHECKMARK,
	
	IMG_HEALTHBAR,
	
	IMG_TARGET_RED_ATTACK_INTRO,
	IMG_TARGET_RED_ATTACK_IDLE,
	
	IMG_TARGET_GREEN_ATTACK_INTRO,
	IMG_TARGET_GREEN_ATTACK_IDLE,
	
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
	"images/tile_no_move.png",
	
	"images/round_1.png",
	"images/round_2.png",
	"images/round_3.png",
	
	"images/checkmark.png",
	
	"images/healthbar.png",
	
	"images/target_red_attack_intro.png",
	"images/target_red_attack_idle.png",
	"images/target_green_attack_selected_intro.png",
	"images/target_green_attack_selected.png",
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
	UI_WAITING_READY,
	UI_WAIT_INPUT,
	UI_WAITING_SERVER,
	UI_HIDE_TIMER,
	UI_ANIMATE,
	
	NUM_UI
};

/* Lista de animaciones */
enum {
	UI_ANIM_ROUND,
	UI_SHOW_ENEMYS,
	
	ANIM_MOVE_NINJAS,
	ANIM_ATTACK,
	
	NUM_UI_ANIM
};

typedef struct {
	int local_ui;
	
	int background;
	
	int escenario[5][9];
	int acciones[5][9];
	int acciones_anim[5][9];
	int acciones_frame[5][9];
	
	ObjectPos next_enemys[4];
	int count_next_enemys;
	
	union {
		struct {
			Ninja *fire;
			Ninja *snow;
			Ninja *water;
		};
		Ninja *ninjas[3];
	};
	Ninja *local_ninja;
	
	Enemy *enemigos[4];
	int attack_x, attack_y;
} SnowStage;

typedef struct {
	int tipo;
	
	union {
		int round;
		struct {
			Ninja *ninja;
			Enemy *enemy;
		} attack;
	};
} Animaten;

/* Prototipos de función */
int game_intro (SnowStage *stage);
int game_loop (SnowStage *stage);
int game_finish (void);
void setup (void);

void do_moves_ninjas (SnowStage *stage, ServerActions *server);
void do_ninja_attacks (SnowStage *stage, ServerActions *server, Animaten *animaciones, int *pos);
void reverse_animations (Animaten *animaciones, int total);
void update_target_actions (SnowStage *stage);
int mouse_intro_penguin (int x, int y);
int map_button_in_game (int x, int y, UITimer *timer);

/* Variables globales */
SDL_Window *ventana;
SDL_Renderer *renderer;
int full_screen = 0;
SDL_Texture * images[NUM_IMAGES];
SDL_Texture * shared_images[NUM_SHARED_IMGS];
int use_sound;

SDL_Surface *penguin[3];

Mix_Chunk * sounds[NUM_SOUNDS];
Mix_Music * mus_carnie;

char nick_global[NICK_SIZE];
static int nick_default;

/* Los Snow Sprites compartidos */
static SnowSprite health_bar[] = {
	{57,155,53,7,0,0,FALSE},
	{2,155,53,7,0,0,FALSE},
	{57,146,53,7,0,0,FALSE},
	{2,146,53,7,0,0,FALSE},
	{57,137,53,7,0,0,FALSE},
	{2,137,53,7,0,0,FALSE},
	{57,128,53,7,0,0,FALSE},
	{2,128,53,7,0,0,FALSE},
	{57,119,53,7,0,0,FALSE},
	{2,119,53,7,0,0,FALSE},
	{2,240,53,7,0,0,FALSE},
	{2,231,53,7,0,0,FALSE},
	{119,194,53,7,0,0,TRUE},
	{110,194,53,7,0,0,TRUE},
	{101,194,53,7,0,0,TRUE},
	{92,194,53,7,0,0,TRUE},
	{83,194,53,7,0,0,TRUE},
	{74,194,53,7,0,0,TRUE},
	{65,194,53,7,0,0,TRUE},
	{65,185,53,7,0,0,FALSE},
	{65,185,53,7,0,0,FALSE},
	{65,176,53,7,0,0,FALSE},
	{65,167,53,7,0,0,FALSE},
	{56,167,53,7,0,0,TRUE},
	{47,167,53,7,0,0,TRUE},
	{38,167,53,7,0,0,TRUE},
	{29,167,53,7,0,0,TRUE},
	{20,167,53,7,0,0,TRUE},
	{11,167,53,7,0,0,TRUE},
	{2,222,53,7,0,0,FALSE},
	{2,167,53,7,0,0,TRUE},
	{112,112,53,7,0,0,TRUE},
	{57,110,53,7,0,0,FALSE},
	{2,110,53,7,0,0,FALSE},
	{57,101,53,7,0,0,FALSE},
	{2,101,53,7,0,0,FALSE},
	{57,92,53,7,0,0,FALSE},
	{2,92,53,7,0,0,FALSE},
	{57,83,53,7,0,0,FALSE},
	{2,83,53,7,0,0,FALSE},
	{57,74,53,7,0,0,FALSE},
	{2,74,53,7,0,0,FALSE},
	{57,65,53,7,0,0,FALSE},
	{2,65,53,7,0,0,FALSE},
	{112,57,53,7,0,0,TRUE},
	{57,56,53,7,0,0,FALSE},
	{2,56,53,7,0,0,FALSE},
	{57,47,53,7,0,0,FALSE},
	{2,47,53,7,0,0,FALSE},
	{57,38,53,7,0,0,FALSE},
	{2,38,53,7,0,0,FALSE},
	{57,29,53,7,0,0,FALSE},
	{2,29,53,7,0,0,FALSE},
	{57,20,53,7,0,0,FALSE},
	{2,20,53,7,0,0,FALSE},
	{57,11,53,7,0,0,FALSE},
	{2,11,53,7,0,0,FALSE},
	{112,2,53,7,0,0,TRUE},
	{57,2,53,7,0,0,FALSE},
	{2,2,53,7,0,0,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

SnowSprite *shared_sprites[NUM_SHARED_IMGS] = {
	health_bar
};

/* Los targets */
static SnowSprite sprite_target_red_intro[] = {
	{65,65,59,59,0,0,FALSE},
	{2,65,61,61,0,0,FALSE},
	{65,2,61,61,0,0,FALSE},
	{2,2,61,61,0,0,FALSE},
	{65,126,47,47,6,6,FALSE},
	{2,128,39,39,10,10,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_target_red_idle[] = {
	{44,86,40,40,6,6,FALSE},
	{2,86,40,40,6,6,FALSE},
	{212,44,40,40,6,6,FALSE},
	{170,44,40,40,6,6,FALSE},
	{128,44,40,40,6,6,FALSE},
	{86,44,40,40,6,6,FALSE},
	{44,44,40,40,6,6,FALSE},
	{86,126,40,38,6,8,FALSE},
	{212,86,40,38,6,8,FALSE},
	{202,166,38,38,8,8,FALSE},
	{162,166,38,38,8,8,FALSE},
	{122,166,38,38,8,7,FALSE},
	{42,168,38,38,7,7,FALSE},
	{2,168,38,38,7,7,FALSE},
	{82,166,38,38,7,7,FALSE},
	{40,398,36,36,9,9,FALSE},
	{40,360,36,36,9,9,FALSE},
	{2,474,36,36,8,8,FALSE},
	{2,436,36,36,8,8,FALSE},
	{2,398,36,36,8,8,FALSE},
	{2,360,36,36,8,8,FALSE},
	{192,320,36,36,8,8,FALSE},
	{154,320,36,36,8,8,FALSE},
	{116,320,36,36,8,8,FALSE},
	{40,322,36,36,8,8,FALSE},
	{2,322,36,36,8,8,FALSE},
	{78,320,36,36,8,8,FALSE},
	{192,282,36,36,8,8,FALSE},
	{154,282,36,36,8,8,FALSE},
	{116,282,36,36,8,8,FALSE},
	{40,284,36,36,8,8,FALSE},
	{2,284,36,36,8,8,FALSE},
	{78,282,36,36,8,8,FALSE},
	{192,244,36,36,8,8,FALSE},
	{154,244,36,36,8,8,FALSE},
	{116,244,36,36,8,8,FALSE},
	{40,246,36,36,8,8,FALSE},
	{2,246,36,36,8,8,FALSE},
	{78,244,36,36,8,8,FALSE},
	{198,206,36,36,8,8,FALSE},
	{160,206,36,36,8,8,FALSE},
	{122,206,36,36,8,8,FALSE},
	{40,208,36,36,8,8,FALSE},
	{2,208,36,36,9,8,FALSE},
	{82,206,38,36,7,9,FALSE},
	{208,126,38,38,7,7,FALSE},
	{168,126,38,38,7,7,FALSE},
	{128,126,38,38,7,7,FALSE},
	{42,128,38,38,7,7,FALSE},
	{2,128,38,38,8,7,FALSE},
	{170,86,40,38,6,8,FALSE},
	{128,86,40,38,6,8,FALSE},
	{86,86,40,38,6,8,FALSE},
	{2,44,40,40,6,6,FALSE},
	{212,2,40,40,6,6,FALSE},
	{170,2,40,40,6,6,FALSE},
	{128,2,40,40,6,6,FALSE},
	{86,2,40,40,6,6,FALSE},
	{44,2,40,40,6,6,FALSE},
	{2,2,40,40,6,6,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_target_green_intro[] = {
	{2,110,39,39,11,11,FALSE},
	{65,2,57,57,2,2,FALSE},
	{2,2,61,61,0,0,FALSE},
	{65,61,51,51,5,5,FALSE},
	{2,65,43,43,8,8,FALSE},
	{2,110,39,39,11,11,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

static SnowSprite sprite_target_green_idle[] = {
	{0,0,39,39,11,11,FALSE},
	{-1,-1,-1,-1,-1,-1,FALSE}
};

SnowSprite *sprite_target_red[4] = {
	sprite_target_red_intro,
	sprite_target_red_idle,
	sprite_target_green_intro,
	sprite_target_green_idle
};

int main (int argc, char *argv[]) {
	int r;
	SnowStage stage;
	
	/* Recuperar las rutas del sistema */
	initSystemPaths (argv[0]);
	
	/* Inicializar l18n */
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, l10n_path);
	
	textdomain (PACKAGE);
	
	cp_registrar_botones (NUM_BUTTONS);
	setup ();
	
	cp_button_start ();
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
	int anim[4];
	int others[4] = {-1, -1, -1, -1};
	NinjaInfo *info;
	ObjectPos *objs;
	StartInfo *start_info;
	
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
					close_netplay ();
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
						if (selected != -1) {
							anim[selected] = 0;
						
							/* Enviar la selección al servidor */
							send_join (selected, nick_global);
						
							stage->local_ui = selected;
						}
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
								close_netplay ();
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
							case NETWORK_EVENT_REMOVE_PLAYER:
								/* TODO: Borrar el nick de este jugador */
								h = GPOINTER_TO_INT (event.user.data1);
								
								others[h] = -1;
								anim[h] = 0;
								break;
							case NETWORK_EVENT_START:
								start_info = (StartInfo *) event.user.data1;
								h = start_info->num_objects;
								objs = (ObjectPos *) event.user.data2;
								stage->count_next_enemys = 0;
								for (g = 0; g < h; g++) {
									if (objs[g].object >= ENEMY_SLY && objs[g].object <= ENEMY_TANK) {
										/* Agregar a la lista de enemigos próximos */
										stage->next_enemys[stage->count_next_enemys].object = objs[g].object;
										stage->next_enemys[stage->count_next_enemys].x = objs[g].x;
										stage->next_enemys[stage->count_next_enemys].y = objs[g].y;
										stage->count_next_enemys++;
									} else {
										stage->escenario[objs[g].y][objs[g].x] = objs[g].object;
									}
								}
								
								stage->background = start_info->background;
								free (objs);
								free (start_info);
								
								ignore_network = TRUE; /* Ya no procesar eventos de red */
								f_anim = 0;
								break;
						}
					}
			}
		}
		
		/* Borrar con el fondo */
		SDL_RenderCopy (renderer, images[IMG_INTRO_BACKGROUND], NULL, NULL);
		
		if (selected == NINJA_WATER || others[NINJA_WATER] != -1) {
			/* Correr la animación */
			rect.x = 0;
			rect.y = 0;
			g = anim[NINJA_WATER] + IMG_INTRO_WATER_SELECT_1;
			SDL_QueryTexture (images[g], NULL, NULL, &rect.w, &rect.h);
			SDL_RenderCopy (renderer, images[g], NULL, &rect);
			
			if (anim[NINJA_WATER] < 15) anim[NINJA_WATER]++;
		} else {
			g = (over == NINJA_WATER) ? IMG_INTRO_WATER_OVER : (selected == -1) ? IMG_INTRO_WATER_NORMAL : IMG_INTRO_WATER_SHADOW;
			rect.x = -28;
			rect.y = 27;
			
			SDL_QueryTexture (images[g], NULL, NULL, &rect.w, &rect.h);
			SDL_RenderCopy (renderer, images[g], NULL, &rect);
		}
		
		if (selected == NINJA_FIRE || others[NINJA_FIRE] != -1) {
			rect.x = 465;
			rect.y = 0;
			g = anim[NINJA_FIRE] + IMG_INTRO_FIRE_SELECT_1;
			SDL_QueryTexture (images[g], NULL, NULL, &rect.w, &rect.h);
			SDL_RenderCopy (renderer, images[g], NULL, &rect);
			
			if (anim[NINJA_FIRE] < 15) anim[NINJA_FIRE]++;
		} else {
			g = (over == NINJA_FIRE) ? IMG_INTRO_FIRE_OVER : (selected == -1) ? IMG_INTRO_FIRE_NORMAL : IMG_INTRO_FIRE_SHADOW;
			rect.x = 484;
			rect.y = 87;
			
			SDL_QueryTexture (images[g], NULL, NULL, &rect.w, &rect.h);
			SDL_RenderCopy (renderer, images[g], NULL, &rect);
		}
		
		if (selected == NINJA_SNOW || others[NINJA_SNOW] != -1) {
			rect.x = 231;
			rect.y = 0;
			g = anim[NINJA_SNOW] + IMG_INTRO_SNOW_SELECT_1;
			SDL_QueryTexture (images[g], NULL, NULL, &rect.w, &rect.h);
			SDL_RenderCopy (renderer, images[g], NULL, &rect);
			
			if (anim[NINJA_SNOW] < 15) anim[NINJA_SNOW]++;
		} else {
			g = (over == NINJA_SNOW) ? IMG_INTRO_SNOW_OVER : (selected == -1) ? IMG_INTRO_SNOW_NORMAL : IMG_INTRO_SNOW_SHADOW;
			rect.x = 241;
			rect.y = 22;
			
			SDL_QueryTexture (images[g], NULL, NULL, &rect.w, &rect.h);
			SDL_RenderCopy (renderer, images[g], NULL, &rect);
		}
		
		if (ignore_network) {
			/* Presentar la animación final */
			f_anim++;
			if (f_anim > 30 && done != GAME_QUIT) {
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
	
	int g, h, i, j;
	UITimer *timer;
	int map;
	
	int ui_estatus = UI_ANIMATE;
	
	int fondo = stage->background;
	Animaten animaciones[6];
	int anims, cont_a;
	Action *accion;
	ServerActions *server;
	int readys[4];
	int attack_started = 0;
	fondo = 2;
	
	anims = 1;
	animaciones[0].tipo = UI_ANIM_ROUND;
	animaciones[0].round = 0;
	cont_a = 0;
	
	stage->attack_x = stage->attack_y = -1;
	readys[1] = readys[2] = readys[3] = FALSE;
	
	/* Crear los 3 ninjas */
	if (stage->escenario[0][0] >= NINJA_FIRE && stage->escenario[0][0] <= NINJA_WATER) {
		stage->ninjas[stage->escenario[0][0] - NINJA_FIRE] = create_ninja (stage->escenario[0][0], 0, 0);
	}
	
	if (stage->escenario[2][0] >= NINJA_FIRE && stage->escenario[2][0] <= NINJA_WATER) {
		stage->ninjas[stage->escenario[2][0] - NINJA_FIRE] = create_ninja (stage->escenario[2][0], 0, 2);
	}
	
	if (stage->escenario[4][0] >= NINJA_FIRE && stage->escenario[4][0] <= NINJA_WATER) {
		stage->ninjas[stage->escenario[4][0] - NINJA_FIRE] = create_ninja (stage->escenario[4][0], 0, 4);
	}
	
	stage->local_ninja = stage->ninjas[stage->local_ui - NINJA_FIRE];
	
	timer = crear_timer (stage->local_ui);
	
	do {
		last_time = SDL_GetTicks ();
		
		process_network_events ();
		
		while (SDL_PollEvent(&event) > 0) {
			switch (event.type) {
				case SDL_QUIT:
					/* Vamos a cerrar la aplicación */
					done = GAME_QUIT;
					break;
				case SDL_MOUSEMOTION:
					map = map_button_in_game (event.motion.x, event.motion.y, timer);
					cp_button_motion (map);
					break;
				case SDL_MOUSEBUTTONDOWN:
					/* Tengo un Mouse Down */
					map = map_button_in_game (event.motion.x, event.motion.y, timer);
					cp_button_down (map);
					
					if (ui_estatus == UI_WAIT_INPUT &&
					    event.button.x >= MAP_X && event.button.x < (MAP_X + 70 * 9) &&
					    event.button.y >= MAP_Y && event.button.y < (MAP_Y + 70 * 5)) {
						g = (event.button.x - MAP_X) / 70;
						h = (event.button.y - MAP_Y) / 70;
						
						// Enviar el evento al servidor y esperar a que llegue para mostrarlo */
						if (stage->acciones[h][g] == ACTION_MOVE) {
							send_action (stage->local_ui, ACTION_MOVE, g, h);
						} else if (stage->acciones[h][g] == ACTION_ATTACK) {
							send_action (stage->local_ui, ACTION_ATTACK, g, h);
						}
					}
					break;
				case SDL_MOUSEBUTTONUP:
					/* Tengo un mouse Up */
					map = map_button_in_game (event.motion.x, event.motion.y, timer);
					map = cp_button_up (map);
					
					switch (map) {
						case BUTTON_TIMER_DONE:
							/* Enviar nuestro ready al servidor */
							send_actions_done ();
							timer_button_selected (timer);
							ui_estatus = UI_WAITING_SERVER;
							readys[stage->local_ui] = TRUE;
							break;
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
					if (event.type == UI_TIMER_EVENT) {
						switch (event.user.code) {
							case UI_TIMER_EVENT_SHOW:
								/* Como el reloj ya se mostró, enviar el evento al servidor de que estamos listos */
								send_ready ();
								ui_estatus = UI_WAITING_READY;
								break;
							case UI_TIMER_EVENT_DONE_TICKS:
								timer_no_more_actions (timer);
								ui_estatus = UI_WAITING_SERVER;
								break;
							case UI_TIMER_EVENT_HIDE:
								/* Como el reloj ya se ocultó, continuar con las animaciones */
								ui_estatus = UI_ANIMATE;
								break;
						}
					} else if (event.type == NETWORK_EVENT) {
						switch (event.user.code) {
							case NETWORK_EVENT_SERVER_ASK_ACTIONS:
								if (ui_estatus != UI_WAITING_READY) {
									printf ("Error de sincronización, recibí un ask actions cuando aún no envio ready\n");
									/* FIXME: Hacer algo */
								} else {
									ui_estatus = UI_WAIT_INPUT;
									start_ticking (timer);
									readys[1] = readys[2] = readys[3] = FALSE;
									stage->attack_x = stage->attack_y = -1;
									memset (stage->acciones, 0, sizeof (stage->acciones));
									ninja_ask_actions (stage->local_ninja, stage->escenario, stage->acciones);
									
									update_target_actions (stage);
								}
								break;
							case NETOWRK_EVENT_ACTION:
								accion = (Action *) event.user.data1;
								
								if (accion->type == ACTION_MOVE) {
									if (accion->object == NINJA_FIRE) {
										ninja_move_ghost (stage->fire, accion->x, accion->y);
										ninja_ask_coords (stage->fire, &g, &h);
									} else if (accion->object == NINJA_WATER) {
										ninja_move_ghost (stage->water, accion->x, accion->y);
										ninja_ask_coords (stage->water, &g, &h);
									} else if (accion->object == NINJA_SNOW) {
										ninja_move_ghost (stage->snow, accion->x, accion->y);
										ninja_ask_coords (stage->snow, &g, &h);
									}
									
									/* Si las coordenadas de nuestro movimiento son diferentes, resetear el ataque */
									if (accion->x != g || accion->y != h) {
										stage->attack_x = stage->attack_y = -1;
									}
									if (accion->object == stage->local_ui) {
										/* Si nuestra posición cambió, actualizar las acciones */
										memset (stage->acciones, 0, sizeof (stage->acciones));
										ninja_ask_actions (stage->local_ninja, stage->escenario, stage->acciones);
										update_target_actions (stage);
									}
								} else if (accion->type == ACTION_ATTACK) {
									if (accion->object == stage->local_ui) {
										stage->attack_x = accion->x;
										stage->attack_y = accion->y;
										/* Solicitar nuevas acciones */
										memset (stage->acciones, 0, sizeof (stage->acciones));
										ninja_ask_actions (stage->local_ninja, stage->escenario, stage->acciones);
										update_target_actions (stage);
									}
								}
								
								free (accion);
								break;
							case NETWORK_EVENT_REMOVE_PLAYER:
								/* Eliminar a otro jugador */
								g = GPOINTER_TO_INT (event.user.data1);
								
								if (g == NINJA_FIRE) {
									ninja_ask_coords (stage->fire, &h, &i);
									stage->escenario[i][h] = NONE;
									free (stage->fire);
									stage->fire = NULL;
								} else if (g == NINJA_SNOW) {
									ninja_ask_coords (stage->snow, &h, &i);
									stage->escenario[i][h] = NONE;
									free (stage->snow);
									stage->snow = NULL;
								} else if (g == NINJA_WATER) {
									ninja_ask_coords (stage->water, &h, &i);
									stage->escenario[i][h] = NONE;
									free (stage->water);
									stage->water = NULL;
								}
								break;
							case NETWORK_EVENT_PLAYER_DONE_ACTIONS:
								g = GPOINTER_TO_INT (event.user.data1);
								readys[g] = TRUE;
								break;
							case NETWORK_EVENT_SERVER_ACTIONS:
								server = (ServerActions *) event.user.data1;
								
								/* Ocultar el reloj */
								hide_timer (timer);
								
								anims = 0;
								/* Anexar todas las animaciones */
								if (server->movs != 0) {
									animaciones[anims].tipo = ANIM_MOVE_NINJAS;
									
									do_moves_ninjas (stage, server);
									anims++;
								}
								
								if (server->attacks != 0) {
									do_ninja_attacks (stage, server, animaciones, &anims);
								}
								
								reverse_animations (animaciones, anims);
								ui_estatus = UI_HIDE_TIMER;
								free (server);
								break;
							//default:
								//printf ("Recibí un evento de red aún desconocido\n");
						}
					}
					break;
			}
		}
		/* Borrar con el fondo */
		SDL_RenderCopy (renderer, images[IMG_BACKGROUND_1 + fondo], NULL, NULL);
		
		if (ui_estatus == UI_WAIT_INPUT || ui_estatus == UI_WAITING_SERVER) {
			rect.x = MAP_X + 2;
			rect.y = MAP_Y;
			SDL_QueryTexture (images[IMG_UI_FRAME], NULL, NULL, &rect.w, &rect.h);
			SDL_RenderCopy (renderer, images[IMG_UI_FRAME], NULL, &rect);
		}
		
		if (ui_estatus == UI_WAIT_INPUT) {
			/* Dibujar las posibles acciones */
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
		
		/* Dibujar todos los objetos */
		for (g = 0; g < 5; g++) {
			for (h = 0; h < 9; h++) {
				if (stage->escenario[g][h] == NONE) continue;
				
				if (stage->escenario[g][h] == ROCK) {
					/* Dibujar la piedra */
					rect.x = MAP_X + (h * 70);
					rect.y = MAP_Y + (g * 70);
					i = (fondo == 0) ? IMG_CRAG_ROCK : IMG_ROCK;
					SDL_QueryTexture (images[i], NULL, NULL, &rect.w, &rect.h);
					
					SDL_RenderCopy (renderer, images[i], NULL, &rect);
				} else if (stage->escenario[g][h] >= NINJA_FIRE && stage->escenario[g][h] <= NINJA_WATER) {
					ninja_draw (stage->ninjas[stage->escenario[g][h] - NINJA_FIRE]);
					if (readys[stage->escenario[g][h]] && (ui_estatus == UI_WAIT_INPUT || ui_estatus == UI_WAITING_SERVER)) {
						/* Dibuja la palomita de listo */
						rect.x = MAP_X + (h * 70) + 19;
						rect.y = MAP_Y + (g * 70) + 20;
						SDL_QueryTexture (images[IMG_CHECKMARK], NULL, NULL, &rect.w, &rect.h);
						
						SDL_RenderCopy (renderer, images[IMG_CHECKMARK], NULL, &rect);
					}
				} else if (stage->escenario[g][h] >= ENEMY_1 || stage->escenario[g][h] <= ENEMY_4) {
					draw_enemy (stage->enemigos[stage->escenario[g][h] - ENEMY_1]);
				}
				
				if (ui_estatus == UI_WAIT_INPUT && (stage->acciones[g][h] & ACTION_ATTACK)) {
					/* Dibujar el target */
					i = stage->acciones_frame[g][h];
					j = stage->acciones_anim[g][h] - IMG_TARGET_RED_ATTACK_INTRO;
					
					rect2.x = sprite_target_red[j][i].orig_x;
					rect2.y = sprite_target_red[j][i].orig_y;
					rect.w = rect2.w = sprite_target_red[j][i].w;
					rect.h = rect2.h = sprite_target_red[j][i].h;
					
					rect.x = MAP_X + (h * 70) + 6 + sprite_target_red[j][i].dest_x;
					rect.y = MAP_Y + (g * 70) + 6 + sprite_target_red[j][i].dest_y;
					
					SDL_RenderCopy (renderer, images[stage->acciones_anim[g][h]], &rect2, &rect);
					i = ++stage->acciones_frame[g][h];
					
					/* Avanzar la animación */
					if (sprite_target_red[j][i].w == -1 && sprite_target_red[j][i].h == -1) {
						stage->acciones_frame[g][h] = 0;
						if (stage->acciones_anim[g][h] == IMG_TARGET_RED_ATTACK_INTRO) {
							stage->acciones_anim[g][h] = IMG_TARGET_RED_ATTACK_IDLE;
						} else if (stage->acciones_anim[g][h] == IMG_TARGET_GREEN_ATTACK_INTRO) {
							stage->acciones_anim[g][h] = IMG_TARGET_GREEN_ATTACK_IDLE;
						}
					}
				}
			}
		}
		
		if (ui_estatus == UI_WAIT_INPUT || ui_estatus == UI_WAITING_SERVER) {
			if (stage->water != NULL) ninja_draw_ghost (stage->water);
			if (stage->fire != NULL) ninja_draw_ghost (stage->fire);
			if (stage->snow != NULL) ninja_draw_ghost (stage->snow);
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
		
		if (ui_estatus == UI_ANIMATE) {
			if (anims == 0) {
				/* Nada que animar, que raro */
				
			} else if (animaciones[anims - 1].tipo == UI_ANIM_ROUND) {
				/* Desplegar el round en pantalla */
				SDL_QueryTexture (images[IMG_ROUND_1 + animaciones[anims - 1].round], NULL, NULL, &rect.w, &rect.h);
				rect.x = (800 - rect.w) / 2;
				rect.y = (500 - rect.h) / 2;
				
				SDL_RenderCopy (renderer, images[IMG_ROUND_1 + animaciones[anims - 1].round], NULL, &rect);
				cont_a++;
				
				if (cont_a >= 60) {
					/* Como ya terminó la animación del round, apilar las animaciones para aparecer a los enemigos */
					animaciones[anims - 1].tipo = UI_SHOW_ENEMYS;
					for (g = 0; g < stage->count_next_enemys; g++) {
						stage->escenario[stage->next_enemys[g].y][stage->next_enemys[g].x] = ENEMY_1 + g;
						
						/* Crear los objetos */
						stage->enemigos[g] = create_enemy (stage->next_enemys[g].x, stage->next_enemys[g].y, stage->next_enemys[g].object);
					}
				}
			} else if (animaciones[anims - 1].tipo == UI_SHOW_ENEMYS) {
				/* Preguntarle al primer enemigo si está listo */
				if (is_enemy_ready (stage->enemigos[0])) {
					anims--;
				}
			} else if (animaciones[anims - 1].tipo == ANIM_MOVE_NINJAS) {
				/* Preguntarle a todos los ninjas si ya terminaron */
				g = 0;
				h = 0;
				for (i = 0; i < 3; i++) {
					if (stage->ninjas[i] != NULL) {
						g++;
						if (ninja_is_done (stage->ninjas[i])) {
							h++;
						}
					}
				}
				
				if (h == g) {
					anims--;
				}
			} else if (animaciones[anims - 1].tipo == ANIM_ATTACK) {
				if (attack_started == 0) {
					/* Solicitarle al ninja que ejecute el ataque */
					ninja_attack (animaciones[anims - 1].attack.ninja);
					/* Golpear al enemigo */
					g = ninja_get_hit_delay (animaciones[anims - 1].attack.ninja);
					h = ninja_get_attack (animaciones[anims - 1].attack.ninja);
					
					enemy_hit_delayed (animaciones[anims - 1].attack.enemy, h, g);
					attack_started = 1;
				} else {
					/* Preguntarle al ninja si ya atacó */
					if (ninja_is_done (animaciones[anims - 1].attack.ninja)) {
						attack_started = 0;
						anims--;
					}
				}
			}
			if (anims == 0) {
				/* Enviar el mensaje de listo y mostrar el reloj */
				ui_estatus = UI_SHOW_TIMER;
				show_timer (timer);
			}
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
	
	setup_ninja ();
	setup_enemy ();
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
	
	shared_images [SHARED_IMG_HEALTHBAR] = images [IMG_HEALTHBAR];
	
	/* Generador de números aleatorios */
	srand (SDL_GetTicks ());
}

void do_moves_ninjas (SnowStage *stage, ServerActions *server) {
	int g, h, i, s;
	/* Quitar del escenario en la viejas posiciones */
	/* Solicitar los movimientos a los ninjas */
	for (i = 0; i < server->movs; i++) {
		s = server->movs_coords[i].object - NINJA_FIRE;
		ninja_ask_coords (stage->ninjas[s], &g, &h);
		stage->escenario[h][g] = NONE;
		g = server->movs_coords[i].x;
		h = server->movs_coords[i].y;
		ninja_move (stage->ninjas[s], g, h);
		stage->escenario[h][g] = s + NINJA_FIRE;
	}
}

void do_ninja_attacks (SnowStage *stage, ServerActions *server, Animaten *animaciones, int *pos) {
	int g, s, n;
	int x, y;
	
	for (g = 0; g < server->attacks; g++) {
		s = stage->escenario[server->attack_coords[g].y][server->attack_coords[g].x] - ENEMY_1;
		
		add_enemy_ref (stage->enemigos[s]);
		
		n = server->attack_coords[g].object - NINJA_FIRE;
		animaciones[*pos].tipo = ANIM_ATTACK;
		animaciones[*pos].attack.enemy = stage->enemigos[s];
		animaciones[*pos].attack.ninja = stage->ninjas[n];
		
		(*pos)++;
	}
}

void reverse_animations (Animaten *animaciones, int total) {
	int g;
	Animaten temp;
	
	for (g = 0; g < total / 2; g++) {
		temp = animaciones[g];
		animaciones[g] = animaciones[total - g - 1];
		animaciones[total - g - 1] = temp;
	}
}

void update_target_actions (SnowStage *stage) {
	int g, h;
	
	memset (stage->acciones_anim, 0, sizeof (stage->acciones_anim));
	memset (stage->acciones_frame, 0, sizeof (stage->acciones_frame));
	for (g = 0; g < 5; g++) {
		for (h = 0; h < 9; h++) {
			if (stage->acciones[g][h] & ACTION_ATTACK) {
				stage->acciones_anim[g][h] = IMG_TARGET_RED_ATTACK_INTRO;
			}
		}
	}
	
	if (stage->attack_x != -1 && stage->attack_y != -1) {
		stage->acciones_anim[stage->attack_y][stage->attack_x] = IMG_TARGET_GREEN_ATTACK_INTRO;
	}
}

int mouse_intro_penguin (int x, int y) {
	Uint32 pixel;
	int bpp, pos;
	Uint8 r, g, b, a;
	
	bpp = 4;
	
	/* Revisar si el mouse está en el ninja de nieve */
	if (x >= 241 && x < 241 + penguin[NINJA_SNOW - 1]->w &&
	    y >= 22 && y < 22 + penguin[NINJA_SNOW - 1]->h) {
		pos = ((y - 22) * penguin[NINJA_SNOW - 1]->pitch) / bpp + (x - 241);
		pixel = ((Uint32 *) penguin[NINJA_SNOW - 1]->pixels)[pos];
		
		SDL_GetRGBA (pixel, penguin[NINJA_SNOW - 1]->format, &r, &g, &b, &a);
		
		if (a != 0) {
			return NINJA_SNOW;
		}
	}
	
	if (x >= 484 && x < 484 + penguin[NINJA_FIRE - 1]->w &&
	    y >= 87 && y < 87 + penguin[NINJA_FIRE - 1]->h) {
		pos = ((y - 87) * penguin[NINJA_FIRE - 1]->pitch) / bpp + (x - 484);
		pixel = ((Uint32 *) penguin[NINJA_FIRE - 1]->pixels)[pos];
		
		SDL_GetRGBA (pixel, penguin[NINJA_FIRE - 1]->format, &r, &g, &b, &a);
		
		if (a != 0) {
			return NINJA_FIRE;
		}
	}
	
	if (x >= 0 && x < -28 + penguin[NINJA_WATER - 1]->w &&
	    y >= 27 && y < 27 + penguin[NINJA_WATER - 1]->h) {
		pos = ((y - 27) * penguin[NINJA_WATER - 1]->pitch) / bpp + (x + 28);
		pixel = ((Uint32 *) penguin[NINJA_WATER - 1]->pixels)[pos];
		
		SDL_GetRGBA (pixel, penguin[NINJA_WATER - 1]->format, &r, &g, &b, &a);
		
		if (a != 0) {
			return NINJA_WATER;
		}
	}
	
	return -1;
}

int map_button_in_game (int x, int y, UITimer *timer) {
	if (timer_accepts_input (timer)) {
		if (x >= TIMER_X && x < TIMER_X + 36 && y >= 29 && y < 66) return BUTTON_TIMER_DONE;
	}
	return BUTTON_NONE;
}
