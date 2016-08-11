/*
 * rotar.c
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

/*
 * Código tomado de la SDL_gfx
 */

/*  

SDL_rotozoom.c: rotozoomer, zoomer and shrinker for 32bit or 8bit surfaces

Copyright (C) 2001-2011  Andreas Schiffler

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.

Andreas Schiffler -- aschiffler at ferzkopp dot net

*/
#include <stdio.h>

#include <math.h>

#include <SDL.h>

/*!
\brief A 32 bit RGBA pixel.
*/
typedef struct tColorRGBA {
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
} tColorRGBA;

/*!
\brief A 8bit Y/palette pixel.
*/
typedef struct tColorY {
	Uint8 y;
} tColorY;

/*! 
\brief Returns maximum of two numbers a and b.
*/
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))

#define MIN(a, b) ((a) < (b) ? (a) : (b))

/*! 
\brief Number of guard rows added to destination surfaces.

This is a simple but effective workaround for observed issues.
These rows allocate extra memory and are then hidden from the surface.
Rows are added to the end of destination surfaces when they are allocated. 
This catches any potential overflows which seem to happen with 
just the right src image dimensions and scale/rotation and can lead
to a situation where the program can segfault.
*/
#define GUARD_ROWS (2)

/*!
\brief Internal target surface sizing function for rotozooms with trig result return. 

\param width The source surface width.
\param height The source surface height.
\param angle The angle to rotate in degrees.
\param zoomx The horizontal scaling factor.
\param zoomy The vertical scaling factor.
\param dstwidth The calculated width of the destination surface.
\param dstheight The calculated height of the destination surface.
\param canglezoom The sine of the angle adjusted by the zoom factor.
\param sanglezoom The cosine of the angle adjusted by the zoom factor.

*/
void _rotozoomSurfaceSizeTrig(int width, int height, double angle, 
							  int *dstwidth, int *dstheight, 
							  double *canglezoom, double *sanglezoom) {
	double x, y, cx, cy, sx, sy;
	double radangle;
	int dstwidthhalf, dstheighthalf;

	/*
	* Determine destination width and height by rotating a centered source box 
	*/
	radangle = angle * (M_PI / 180.0);
	*sanglezoom = sin(radangle);
	*canglezoom = cos(radangle);
	/**sanglezoom *= zoomx;
	*canglezoom *= zoomx;*/ /* Múltiplicación por 1.0 */
	x = (double)(width / 2);
	y = (double)(height / 2);
	cx = *canglezoom * x;
	cy = *canglezoom * y;
	sx = *sanglezoom * x;
	sy = *sanglezoom * y;

	dstwidthhalf = MAX((int)
		ceil(MAX(MAX(MAX(fabs(cx + sy), fabs(cx - sy)), fabs(-cx + sy)), fabs(-cx - sy))), 1);
	dstheighthalf = MAX((int)
		ceil(MAX(MAX(MAX(fabs(sx + cy), fabs(sx - cy)), fabs(-sx + cy)), fabs(-sx - cy))), 1);
	*dstwidth = 2 * dstwidthhalf;
	*dstheight = 2 * dstheighthalf;
}

void _transformSurfaceRGBA(SDL_Surface * src, SDL_Surface * dst, int cx, int cy, int isin, int icos, int smooth)
{
	int x, y, t1, t2, dx, dy, xd, yd, sdx, sdy, ax, ay, ex, ey, sw, sh;
	tColorRGBA c00, c01, c10, c11, cswap;
	tColorRGBA *pc, *sp;
	int gap;

	/*
	* Variable setup 
	*/
	xd = ((src->w - dst->w) << 15);
	yd = ((src->h - dst->h) << 15);
	ax = (cx << 16) - (icos * cx);
	ay = (cy << 16) - (isin * cx);
	sw = src->w - 1;
	sh = src->h - 1;
	pc = (tColorRGBA*) dst->pixels;
	gap = dst->pitch - dst->w * 4;

	/*
	* Switch between interpolating and non-interpolating code 
	*/
	if (smooth) {
		for (y = 0; y < dst->h; y++) {
			dy = cy - y;
			sdx = (ax + (isin * dy)) + xd;
			sdy = (ay - (icos * dy)) + yd;
			for (x = 0; x < dst->w; x++) {
				dx = (sdx >> 16);
				dy = (sdy >> 16);
				if ((dx > -1) && (dy > -1) && (dx < (src->w-1)) && (dy < (src->h-1))) {
					sp = (tColorRGBA *)src->pixels;;
					sp += ((src->pitch/4) * dy);
					sp += dx;
					c00 = *sp;
					sp += 1;
					c01 = *sp;
					sp += (src->pitch/4);
					c11 = *sp;
					sp -= 1;
					c10 = *sp;
					/*
					* Interpolate colors 
					*/
					ex = (sdx & 0xffff);
					ey = (sdy & 0xffff);
					t1 = ((((c01.r - c00.r) * ex) >> 16) + c00.r) & 0xff;
					t2 = ((((c11.r - c10.r) * ex) >> 16) + c10.r) & 0xff;
					pc->r = (((t2 - t1) * ey) >> 16) + t1;
					t1 = ((((c01.g - c00.g) * ex) >> 16) + c00.g) & 0xff;
					t2 = ((((c11.g - c10.g) * ex) >> 16) + c10.g) & 0xff;
					pc->g = (((t2 - t1) * ey) >> 16) + t1;
					t1 = ((((c01.b - c00.b) * ex) >> 16) + c00.b) & 0xff;
					t2 = ((((c11.b - c10.b) * ex) >> 16) + c10.b) & 0xff;
					pc->b = (((t2 - t1) * ey) >> 16) + t1;
					t1 = ((((c01.a - c00.a) * ex) >> 16) + c00.a) & 0xff;
					t2 = ((((c11.a - c10.a) * ex) >> 16) + c10.a) & 0xff;
					pc->a = (((t2 - t1) * ey) >> 16) + t1;
				}
				sdx += icos;
				sdy += isin;
				pc++;
			}
			pc = (tColorRGBA *) ((Uint8 *) pc + gap);
		}
	} else {
		for (y = 0; y < dst->h; y++) {
			dy = cy - y;
			sdx = (ax + (isin * dy)) + xd;
			sdy = (ay - (icos * dy)) + yd;
			for (x = 0; x < dst->w; x++) {
				dx = (short) (sdx >> 16);
				dy = (short) (sdy >> 16);
				if ((dx >= 0) && (dy >= 0) && (dx < src->w) && (dy < src->h)) {
					sp = (tColorRGBA *) ((Uint8 *) src->pixels + src->pitch * dy);
					sp += dx;
					*pc = *sp;
				}
				sdx += icos;
				sdy += isin;
				pc++;
			}
			pc = (tColorRGBA *) ((Uint8 *) pc + gap);
		}
	}
}

SDL_Surface *rotozoomSurface(SDL_Surface * src, double angle, int smooth) {
	SDL_Surface *rz_src;
	SDL_Surface *rz_dst;
	double zoominv;
	double sanglezoom, canglezoom, sanglezoominv, canglezoominv;
	int dstwidthhalf, dstwidth, dstheighthalf, dstheight;
	int is32bit;
	int i, src_converted;
	Uint8 r,g,b;
	Uint32 colorkey = 0;
	int colorKeyAvailable = 0;

	/*
	* Sanity check 
	*/
	if (src == NULL)
		return (NULL);
	
	if (SDL_GetColorKey (src, &colorkey) == 0) {
		SDL_GetRGB(colorkey, src->format, &r, &g, &b);
		colorKeyAvailable = 1;
	}
	/*
	* Determine if source surface is 32bit or 8bit 
	*/
	is32bit = (src->format->BitsPerPixel == 32);
	if ((is32bit)) {
		/*
		* Use source surface 'as is' 
		*/
		rz_src = src;
		src_converted = 0;
	} else {
		/*
		* New source surface is 32bit with a defined RGBA ordering 
		*/
		rz_src =
			SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, 32, 
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
			0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000
#else
			0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff
#endif
			);
		if(colorKeyAvailable)
			SDL_SetColorKey(src, 0, 0);

		SDL_BlitSurface(src, NULL, rz_src, NULL);

		if(colorKeyAvailable)
			SDL_SetColorKey(src, SDL_TRUE, colorkey);
		src_converted = 1;
		is32bit = 1;
	}

	/*
	* Sanity check zoom factor 
	*/
	zoominv = 65536.0 / (1.0 * 1.0); // 32768

	/*
	* Angle!=0: full rotozoom 
	*/
	/*
	* ----------------------- 
	*/

	/* Determine target size */
	_rotozoomSurfaceSizeTrig(rz_src->w, rz_src->h, angle, &dstwidth, &dstheight, &canglezoom, &sanglezoom);

	/*
	* Calculate target factors from sin/cos and zoom 
	*/
	sanglezoominv = sanglezoom;
	canglezoominv = canglezoom;
	sanglezoominv *= zoominv;
	canglezoominv *= zoominv;
	
	/* Calculate half size */
	dstwidthhalf = dstwidth / 2;
	dstheighthalf = dstheight / 2;
	
	/*
	* Alloc space to completely contain the rotated surface 
	*/
	rz_dst = NULL;
	if (is32bit) {
		/*
		* Target surface is 32bit with source RGBA/ABGR ordering 
		*/
		rz_dst =
			SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight + GUARD_ROWS, 32,
			rz_src->format->Rmask, rz_src->format->Gmask,
			rz_src->format->Bmask, rz_src->format->Amask);
	}

	/* Check target */
	if (rz_dst == NULL)
		return NULL;

	/* Adjust for guard rows */
	rz_dst->h = dstheight;

	if (colorKeyAvailable == 1){
		colorkey = SDL_MapRGB(rz_dst->format, r, g, b);

		SDL_FillRect(rz_dst, NULL, colorkey );
	}

	/*
	* Lock source surface 
	*/
	if (SDL_MUSTLOCK(rz_src)) {
		SDL_LockSurface(rz_src);
	}

	/*
	* Check which kind of surface we have 
	*/
	if (is32bit) {
		/*
		* Call the 32bit transformation routine to do the rotation (using alpha) 
		*/
		_transformSurfaceRGBA(rz_src, rz_dst, dstwidthhalf, dstheighthalf,
			(int) (sanglezoominv), (int) (canglezoominv), 
			smooth);
		/*
		* Turn on source-alpha support 
		*/
		//SDL_SetAlpha(rz_dst, SDL_SRCALPHA, 255);
		Uint32 temp_c;
		if (SDL_GetColorKey (rz_src, &temp_c) == 0) {
			SDL_SetColorKey(rz_dst, SDL_TRUE, temp_c);
		}
	}
	
	/*
	* Unlock source surface 
	*/
	if (SDL_MUSTLOCK(rz_src)) {
		SDL_UnlockSurface(rz_src);
	}
	
	/*
	* Cleanup temp surface 
	*/
	if (src_converted) {
		SDL_FreeSurface(rz_src);
	}

	/*
	* Return destination surface 
	*/
	return (rz_dst);
}

void coordenadas_centro (int centerx, int centery, int w, int h, double angle, int *retx, int *rety) {
	double cangle, sangle, px, py, p1x, p1y, p2x, p2y, p3x, p3y, p4x, p4y;
	double radangle;
	
	radangle = angle * (M_PI / 180.0);
	sangle = sin(radangle);
	cangle = cos(radangle);
	
	sangle = -sangle;
	
	/* Top Left */
    px = -centerx;
    py = -centery;
    p1x = px * cangle - py * sangle + centerx;
    p1y = px * sangle + py * cangle + centery;

    /* Top Right */
    px = w - centerx;
    py = -centery;
    p2x = px * cangle - py * sangle + centerx;
    p2y = px * sangle + py * cangle + centery;

    /* Bottom Left */
    px = -centerx;
    py = h - centery;
    p3x = px * cangle - py * sangle + centerx;
    p3y = px * sangle + py * cangle + centery;

    /* Bottom Right */
    px = w - centerx;
    py = h - centery;
    p4x = px * cangle - py * sangle + centerx;
    p4y = px * sangle + py * cangle + centery;

    *retx = (int)MIN(MIN(p1x, p2x), MIN(p3x, p4x));
    *rety = (int)MIN(MIN(p1y, p2y), MIN(p3y, p4y));
}
