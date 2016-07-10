/*
 * uitimer.h
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

#ifndef __UITIMER_H__
#define __UITIMER_H__

#define TIMER_X 420

typedef struct _UITimer UITimer;

/* Para los eventos */
extern int UI_TIMER_EVENT;

enum {
	UI_TIMER_EVENT_SHOW = 0,
	UI_TIMER_EVENT_DONE_TICKS
};

UITimer *crear_timer (int ui);
void start_ticking (UITimer *timer);
void dibujar_timer (UITimer *timer);
void show_timer (UITimer *timer);
int timer_accepts_input (UITimer *timer);
void timer_button_selected (UITimer *timer);
void setup_timer (void);

#endif /* __UITIMER_H__ */

