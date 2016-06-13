/*
 * path.h
 * This file is part of Paddle Puffle
 *
 * Copyright (C) 2015 - Félix Arreola Rodríguez
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
 * along with Paddle Puffle. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __PATH_H__
#define __PATH_H__

extern char *systemdata_path;
extern char *l10n_path;
extern char *userdata_path;

void initSystemPaths (const char *argv_0);
int folder_exists (const char *fname);
int file_exists (const char *fname);
int folder_create (const char *fname);

#endif /* __PATH_H__ */

