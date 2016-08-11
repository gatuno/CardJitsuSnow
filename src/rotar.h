/*
 * rotar.h
 * This file is part of Pizzatron
 *
 * Copyright (C) 2016 - Félix Arreola Rodríguez
 *
 * Pizzatron is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Pizzatron is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Pizzatron. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __ROTAR_H__
#define __ROTAR_H__

#include <SDL.h>

SDL_Surface *rotozoomSurface(SDL_Surface * src, double angle, int smooth);
void coordenadas_centro (int centerx, int centery, int w, int h, double angle, int *retx, int *rety);

#endif /* __ROTAR_H__ */

