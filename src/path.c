/*
 * path.c
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef MACOSX
// for search paths
#include "NSSystemDirectories.h"
#include <Carbon/Carbon.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#endif

#ifdef __MINGW32__
#include <windows.h>
#include <shellapi.h>
#endif

#include "path.h"

char *systemdata_path;
char *l10n_path;
char *userdata_path;

//#ifdef __MINGW32__
//const char *PathSeparator = "\\";      // for path assembly
//#else
//const char *PathSeparator = "/";       // for path assembly
//#endif
//const char *PathSeparators = "/\\";    // for path splits

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE !FALSE
#endif

#ifndef MAX_PATH
#	define MAX_PATH 2048
#endif

int folder_exists (const char *fname) {
	struct stat s;
	return (stat(fname, &s) == 0 && S_ISDIR(s.st_mode));
}

int file_exists (const char *fname) {
	struct stat s;
	return (stat(fname, &s) == 0 && S_ISREG(s.st_mode));
}

static int split_path (const char *path, char * dir_part, char * filename_part) {
	int lslash, lnslash;
	int g;
	char *dup;
	
	lslash = -1;
	for (g = strlen (path) - 1; g >= 0; g--) {
		if (path[g] == '/' || path[g] == '\\') {
			lslash = g;
			break;
		}
	}
	
	if (
	#ifdef __MINGW32__
		lslash == 2 && path[1] == ':' && (path[0] >= 'A' && path[0] <= 'Z')
	#else
		lslash == 0 
	#endif
	) {
		return FALSE; // we cannot split the root directory apart
	}

	if (lslash == strlen (path) - 1) {
		// trailing slash
		dup = strdup (path);
		dup[lslash] = 0;
		g = split_path (dup, dir_part, filename_part);
		
		free (dup);
		return g;
	}
	
	if (lslash == -1) {
		return FALSE;
	}
	
	for (g = lslash; g >= 0; g--) {
		if (path[g] != '/' && path[g] != '\\') {
			lnslash = g;
			break;
		}
	}
	
	if (dir_part) {
		strncpy (dir_part, path, lnslash + 1);
		dir_part[lnslash + 1] = 0;
	}
	
	if (filename_part) {
		strcpy (filename_part, &path[lslash + 1]);
	}
	
	return TRUE;
}

int folder_create (const char *fname) {
	char *parent_folder;
	char *sub_folder;
	int ok = TRUE;
	
	if (folder_exists (fname)) return TRUE;
	
	parent_folder = strdup (fname);
	sub_folder = strdup (fname);
	
	if (split_path (fname, parent_folder, sub_folder)) {
		if (!folder_exists (parent_folder)) {
			ok = folder_create(parent_folder);
		}
	}
	
	if (ok) {
		#ifdef __MINGW32__
		ok = mkdir(fname) == 0;
		#else
		ok = mkdir(fname, 0775) == 0;
		#endif
	}
	
	free (parent_folder);
	free (sub_folder);
	return ok;
}

#ifdef __MINGW32__
// should be ecl_system_windows.cc ?
static void ApplicationDataPath (char * buffer) {
	typedef HRESULT (WINAPI *SHGETFOLDERPATH)( HWND, int, HANDLE, DWORD, LPTSTR );
	#   define CSIDL_FLAG_CREATE 0x8000
	#   define CSIDL_APPDATA 0x1A
	#   define SHGFP_TYPE_CURRENT 0

	HINSTANCE shfolder_dll;
	SHGETFOLDERPATH SHGetFolderPath ;
	
	/* load the shfolder.dll to retreive SHGetFolderPath */
	if ((shfolder_dll = LoadLibrary("shfolder.dll")) != NULL) {
		SHGetFolderPath = (SHGETFOLDERPATH)GetProcAddress(shfolder_dll, "SHGetFolderPathA");
		if (SHGetFolderPath != NULL) {
			TCHAR szPath[MAX_PATH] = "";
			
			/* get the "Application Data" folder for the current user */
			if (S_OK == SHGetFolderPath (NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, szPath)) {
				strcpy (buffer, szPath);
			}
		} else {
			buffer[0] = '\0';
		}
		FreeLibrary (shfolder_dll);
	} else {
		buffer[0] = '\0';
	}
}
#endif

void initSystemPaths (const char *argv_0) {
	char *progCallPath;
	int progdirexists;
	char *progdir;
	char *pref_path;
#ifdef __MINGW32__
	char winappdata_path[MAX_PATH];
	
	ApplicationDataPath (winappdata_path);
#endif
	
	progCallPath = strdup (argv_0);
#if MACOSX
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef cfurlmain = CFBundleCopyExecutableURL(mainBundle);
	CFStringRef cffileStr = CFURLCopyFileSystemPath(cfurlmain, kCFURLPOSIXPathStyle);
	CFIndex cfmax = CFStringGetMaximumSizeOfFileSystemRepresentation(cffileStr);
	char *localbuffer;
	localbuffer = (char *) malloc (sizeof (char) * cfmax);
	if (CFStringGetFileSystemRepresentation(cffileStr, localbuffer, cfmax)) {
    	free (progCallPath);
		progCallPath = localbuffer; // error skips this and defaults to argv[0] which works for most purposes
	}
	CFRelease(mainBundle);
	CFRelease(cfurlmain);
	CFRelease(cffileStr);
#endif
	progdir = strdup (progCallPath);
	progdirexists = split_path (progCallPath, progdir, NULL);

	/* Primero conseguir el system path */
#ifdef __MINGW32__
	if (!progdirexists) {
		systemdata_path = "./data/";
	} else {
		systemdata_path = (char *) malloc (strlen (progdir) + 50);
		sprintf (systemdata_path, "%s/data/", progdir);
	}
#elif MACOSX
    // Mac OS X applications are self-contained bundles,
    // i.e., directories like "Enigma.app".  Resources are
    // placed in those bundles under "Enigma.app/Contents/Resources",
    // the main executable would be "Enigma.app/Contents/MacOS/enigma".
    // Here, we get the executable name, clip off the last bit, chdir into it,
    // then chdir to ../Resources. The original SDL implementation chdirs to
    // "../../..", i.e. the directory the bundle is placed in. This breaks
    // the self-containedness.
	systemdata_path = (char *) malloc (sizeof (char) * (strlen (progdir) + 30));
	sprintf (systemdata_path, "%s/../Resources/data/", progdir);
#else
	/* Para Linux */
	systemdata_path = GAMEDATA_DIR;
#endif
	
	/* Ahora, conseguir el L10n */
	l10n_path = LOCALEDIR;
#ifdef __MINGW32__
	if (progdirexists) {
		l10n_path = (char *) malloc (strlen (progdir) + strlen (l10n_path) + 10);
		if (strncmp (LOCALEDIR, "/", 1) == 0 || strncmp (LOCALEDIR, "\\", 1) == 0) {
			/* No necesita slash final */
			sprintf (l10n_path, "%s%s", progdir, LOCALEDIR);
		} else {
			sprintf (l10n_path, "%s/%s", progdir, LOCALEDIR);
		}
	}
#elif MACOSX
	l10n_path = (char *) malloc (sizeof (char) * (strlen (progdir) + 30));
	sprintf (l10n_path, "%s/../Resources/locale", progdir);
#endif
	
	/* Ahora conseguir el user path */
	if (getenv ("HOME") != 0) {
		pref_path = getenv ("HOME");
		
		if (!folder_exists (pref_path)) {
			if (!folder_create (pref_path)) {
				fprintf (stderr, "Error Home directory does not exist.\n");
				userdata_path = NULL;
			}
		}
#ifdef MACOSX
		userdata_path = (char *) malloc (strlen (pref_path) + 40);
		sprintf (userdata_path, "%s/Library/Application Support", pref_path);
#else
		userdata_path = strdup (pref_path);
#endif
#ifdef __MINGW32__
	} else if (winappdata_path[0] != 0) {
		userdata_path = strdup (winappdata_path);
#endif
	} else {
		userdata_path = NULL;
	}
	
	/* Liberar las cadenas temporales */
	free (progdir);
	free (progCallPath);
}

