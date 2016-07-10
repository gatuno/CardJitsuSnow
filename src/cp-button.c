/*
 * cp-button.c
 * This file is part of Thin Ice
 *
 * Copyright (C) 2013 - Felix Arreola Rodriguez
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

#include <stdlib.h>
#include <string.h>

int *cp_button_frames;
int *cp_button_refresh;
int cp_old_map, cp_last_button;

void cp_registrar_botones (int n) {
	cp_button_frames = malloc (sizeof (int) * n);
	cp_button_refresh = malloc (sizeof (int) * n);
	memset (cp_button_refresh, 0, n * sizeof (int));
}

void cp_registrar_boton (int n, int frame) {
	cp_button_frames[n] = frame;
}

void cp_button_start (void) {
	cp_old_map = cp_last_button = 0;
}

void cp_button_motion (int map) {
	/* Motor de botones */
	if (cp_old_map == 0 && map != 0) {
		if (cp_last_button == 0) {
			cp_button_frames [map]++;
		} else if (cp_last_button == map) {
			cp_button_frames [map]++;
		}
		cp_button_refresh [map] = 1;
	} else if (cp_old_map != 0 && map == 0) {
		if (cp_last_button == 0) {
			cp_button_frames [cp_old_map]--;
			cp_button_refresh [cp_old_map] = 1;
		} else if (cp_last_button == cp_old_map) {
			cp_button_frames [cp_last_button]--;
			cp_button_refresh [cp_last_button] = 1;
		}
	} else if (cp_old_map != map) {
		if (cp_last_button == 0) {
			cp_button_frames [map]++;
			cp_button_refresh [map] = 1;
			if (cp_old_map != 0) {
				cp_button_frames [cp_old_map]--;
				cp_button_refresh [cp_old_map] = 1;
			}
		} else if (cp_last_button == cp_old_map) {
			cp_button_frames [cp_old_map]--;
			cp_button_refresh [cp_old_map] = 1;
		} else if (cp_last_button == map) {
			cp_button_frames [map]++;
			cp_button_refresh [map] = 1;
		}
	}
	cp_old_map = map;
}

void cp_button_down (int map) {
	cp_last_button = map;
	
	if (cp_last_button != 0) {
		cp_button_frames [cp_last_button]++;
		cp_button_refresh [cp_last_button] = 1;
	}
}

int cp_button_up (int map) {
	if (cp_last_button != 0) {
		cp_button_frames [cp_last_button]--;
		cp_button_refresh [cp_last_button] = 1;
		if (map == cp_last_button) {
			/* Switch del boton */
			cp_last_button = 0;
			return map;
		} else if (map != 0) {
			cp_button_frames [map]++;
			cp_button_refresh [map] = 1;
		}
		
		cp_last_button = 0;
	}
	
	return 0;
}
