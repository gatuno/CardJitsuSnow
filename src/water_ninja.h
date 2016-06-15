/*
 * water_ninja.h
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

#ifndef __WATER_NINJA_H__
#define __WATER_NINJA_H__

typedef struct _WaterNinja WaterNinja;

void setup_water_ninja (void);
void dibujar_water (WaterNinja *);
WaterNinja *crear_water_ninja (void);
void attack_water (WaterNinja *water);
void celebrate_water (WaterNinja *ninja);
void move_water (WaterNinja *ninja);
void ko_water (WaterNinja *ninja);
void hit_water (WaterNinja *ninja);


#endif /* __WATER_NINJA_H__ */

