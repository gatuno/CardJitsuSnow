/*
 * draw-text.h
 * This file is part of Paddle Puffle
 *
 * Copyright (C) 2012 - Félix Arreola Rodríguez
 *
 * Paddle Puffle is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Paddle Puffle is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Paddle Puffle; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#ifndef __DRAW_TEXT_H__
#define __DRAW_TEXT_H__

#define OUTLINE_TEXT 2

#include <SDL.h>

enum {
	ALIGN_LEFT,
	ALIGN_CENTER,
	ALIGN_RIGHT
};

/* Prototipos de función */
SDL_Surface *draw_text (TTF_Font *font, const char *cadena, SDL_Color color, int align, int height_line);

#endif /* __DRAW_TEXT_H__ */

