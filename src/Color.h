/**
 *  Copyright 2010 by Benjamin J. Land (a.k.a. BenLand100)
 *
 *  This file is part of the Scar Minimizing Autoing Resource Thing (SMART)
 *
 *  SMART is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SMART is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SMART. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _COLOR_H
#define	_COLOR_H

#define OutOfBoundsCheck(x,y,sx,sy,ex,ey) if (x < sx || x > ex || y < sy || y > ey) { x = 0; y = 0; }

typedef struct {
	double hue, sat, lum;
} HSL, *PHSL;

typedef union {
	struct { char b, g, r, a; };
	int color;
} RGB, *PRGB;

inline HSL toHSL(RGB&);
inline bool cmpColors(RGB&, RGB&, int);

extern "C" int  setTarget(PRGB, int, int) __attribute__((stdcall));
extern "C" int  getColor(int, int) __attribute__((stdcall));
extern "C" bool findColor(int&, int&, int, int, int, int, int) __attribute__((stdcall));
extern "C" bool findColorTol(int&, int&, int, int, int, int, int, int) __attribute__((stdcall));
extern "C" bool findColorSpiral(int&, int&, int, int, int, int, int) __attribute__((stdcall));
extern "C" bool findColorSpiralTol(int&, int&, int, int, int, int, int, int) __attribute__((stdcall));
extern "C" bool findBitmapTolIn(int, int&, int&, int, int, int, int, int) __attribute__((stdcall));
extern "C" bool findBitmapTol(int, int&, int&, int, int, int, int) __attribute__((stdcall));
extern "C" bool findBitmap(int, int&, int&) __attribute__((stdcall));

#endif	/* _COLOR_H */

