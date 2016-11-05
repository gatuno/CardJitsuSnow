/*
 * server_timer.c
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
#include <time.h>

#include "server_timer.h"

typedef struct TimerData {
	SnowFight *tabla;
	struct timeval inicio;
	int interval;
	Callback func;
	void * user_data;
	
	struct TimerData *next;
} TimerData;

/* Para el manejo de timers */
static TimerData *timers_funcs = NULL;

/* Manejo de timers */
long timevaldiff (struct timeval *starttime, struct timeval *finishtime) {
	long msec;
	msec=(finishtime->tv_sec-starttime->tv_sec)*1000;
	msec+=(finishtime->tv_usec-starttime->tv_usec)/1000;
	return msec;
}

void agregar_timer (SnowFight *tabla, Callback func, int interval, void *data) {
	TimerData **pos, *new;
	
	pos = &timers_funcs;
	
	while (*pos != NULL) {
		pos = &((*pos)->next);
	}
	
	new = (TimerData *) malloc (sizeof (TimerData));
	new->tabla = tabla;
	new->func = func;
	new->user_data = data;
	new->interval = interval;
	gettimeofday (&new->inicio, NULL);
	new->next = NULL;
	
	*pos = new;
}

void cancel_timer (SnowFight *tabla) {
	TimerData *pos, **prev;
	
	prev = &timers_funcs;
	pos = timers_funcs;
	
	while (pos != NULL) {
		if (pos->tabla == tabla) {
			(*prev) = pos->next;
			free (pos);
			return;
		}
		
		prev = &(pos->next);
		pos = pos->next;
	}
}

void check_timers (void) {
	TimerData *pos, **prev, *n;
	struct timeval now;
	long int dif;
	
	prev = &timers_funcs;
	pos = timers_funcs;
	
	gettimeofday (&now, NULL);
	while (pos != NULL) {
		dif = timevaldiff (&pos->inicio, &now);
		if (dif >= (pos->interval * 1000)) {
			/* Llamar esta función */
			pos->func (pos->tabla, pos->user_data);
			
			/* Eliminar la función */
			(*prev) = pos->next;
			n = pos->next;
			
			free (pos);
			pos = n;
		} else {
			prev = &(pos->next);
			pos = pos->next;
		}
	}
}

