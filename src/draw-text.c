/*
 * draw-text.c
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

/* Draw text: General wrapper to draw text with line breaks */

#include <stdlib.h>
#include <string.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "draw-text.h"

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define RMASK 0xff000000
#define GMASK 0x00ff0000
#define BMASK 0x0000ff00
#define AMASK 0x000000ff
#else
#define RMASK 0x000000ff
#define GMASK 0x0000ff00
#define BMASK 0x00ff0000
#define AMASK 0xff000000
#endif

#ifdef WIN32
char *strsep(char **from, const char *delim) {
    char *s, *dp, *ret;

    if ((s = *from) == NULL)
        return NULL;

    ret = s;
    while (*s != '\0') {
        /* loop until the end of s, checking against each delimiting character,
         * if we find a delimiter set **s to '\0' and return our previous token
         * to the user. */
        dp = (char *)delim;
        while (*dp != '\0') {
            if (*s == *dp) {
                *s = '\0';
                *from = s + 1;
                return ret;
            }
            dp++;
        }
        s++;
    }
    /* end of string case */
    *from = NULL;
    return ret;
}
#endif

SDL_Surface *draw_text (TTF_Font *font, const char *cadena, SDL_Color color, int align, int height_line) {
	SDL_Surface *final, **text;
	SDL_Rect dest_rect;
	int n_tokens;
	int g, len, maxw;
	Uint32 pixel;
	char *dup, *str_token, *original;
	
	/* Si no contiene saltos de linea, llamar a la otra función */
	if (strchr (cadena, '\n') != NULL) {
		len = strlen (cadena);
		for (g = 0, n_tokens = 1; g < len; g++) {
			if (cadena[g] == '\n') n_tokens++;
		}
		len =  (TTF_FontLineSkip (font) + height_line) * n_tokens;
		
		text = (SDL_Surface **) malloc (sizeof (SDL_Surface *) * n_tokens);
		original = dup = strdup (cadena);
		
		str_token = strsep (&dup, "\n");
		g = 0; maxw = 0;
		while (str_token != NULL) {
			if (str_token[0] != '\0') {
				text[g] = TTF_RenderUTF8_Blended (font, str_token, color);
				if (text[g]->w > maxw) maxw = text[g]->w;
			} else {
				text[g] = NULL;
			}
			g++;
			str_token = strsep (&dup, "\n");
		};
		final = SDL_CreateRGBSurface (SDL_SWSURFACE, maxw, len, 32, RMASK, GMASK, BMASK, AMASK);
		
		SDL_FillRect (final, NULL, SDL_MapRGBA (final->format, 0, 0, 0, 0));
		/*SDL_SetAlpha(final, 0, SDL_ALPHA_OPAQUE);*/
		len = (TTF_FontLineSkip (font) + height_line);
		for (g = 0; g < n_tokens; g++) {
			if (text[g] == NULL) continue;
			//SDL_SetAlpha (text[g], 0, SDL_ALPHA_OPAQUE);
			
			if (align == ALIGN_LEFT) {
				dest_rect.x = 0;
			} else if (align == ALIGN_CENTER) {
				dest_rect.x = (maxw - text[g]->w) / 2;
			} else if (align == ALIGN_RIGHT) {
				dest_rect.x = (maxw - text[g]->w);
			}
			dest_rect.y = len * g;
			dest_rect.w = text[g]->w;
			dest_rect.h = text[g]->h;
			/*fprintf (stdout, "UMASK, R: %i, G: %i, B: %i, A: %i\n", text->format->Rmask, text->format->Gmask, text->format->Bmask, text->format->Amask);*/
			SDL_BlitSurface (text[g], NULL, final, &dest_rect);
			SDL_FreeSurface (text[g]);
		}
		//SDL_SetAlpha(final, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);
		free (text);
		free (original);
		return final;	
	} else {
		/* En caso contrario, renderizarla nosotros mismos */
		return TTF_RenderUTF8_Blended (font, cadena, color);
	}
}

