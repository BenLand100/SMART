/**
 *  Copyright 2010 by Benjamin J. Land (a.k.a. BenLand100)
 *
 *  This file is part of the SMART Minimizing Autoing Resource Thing (SMART)
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
 
#ifndef _INPUT_H
#define	_INPUT_H

#include "Smart.h"

extern "C" void sendKeys(char*) __attribute__((cdecl));
extern "C" bool isKeyDown(int) __attribute__((cdecl));
extern "C" void holdKey(int) __attribute__((cdecl));
extern "C" void releaseKey(int) __attribute__((cdecl));
extern "C" void getMousePos(int&, int&) __attribute__((cdecl));
extern "C" void moveMouse(int, int) __attribute__((cdecl));
extern "C" void windMouse(int, int) __attribute__((cdecl));
extern "C" void holdMouse(int, int, bool) __attribute__((cdecl));
extern "C" void releaseMouse(int, int, bool) __attribute__((cdecl));
extern "C" void clickMouse(int, int, bool) __attribute__((cdecl));
extern "C" void holdMousePlus(int, int, int) __attribute__((cdecl));
extern "C" void releaseMousePlus(int, int, int) __attribute__((cdecl));
extern "C" void clickMousePlus(int, int, int) __attribute__((cdecl));
extern "C" bool isMouseButtonHeld(int) __attribute__((cdecl));

#endif	/* _INPUT_H */

