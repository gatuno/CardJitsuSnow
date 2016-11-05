/*
 * server_timer.h
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

#ifndef __SERVER_TIMER_H__
#define __SERVER_TIMER_H__

/* Typedef vacio */
typedef struct SnowFight SnowFight;

typedef void (*Callback)(SnowFight *, void *);

void agregar_timer (SnowFight *tabla, Callback func, int interval, void *data);
void cancel_timer (SnowFight *tabla);
void check_timers (void);

#endif /* __SERVER_TIMER_H__ */

