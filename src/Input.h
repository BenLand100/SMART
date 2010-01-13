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

extern "C" void sendKeys(char*) __attribute__((stdcall));
extern "C" bool isKeyDown(long) __attribute__((stdcall));
extern "C" void holdKey(long) __attribute__((stdcall));
extern "C" void releaseKey(long) __attribute__((stdcall));
extern "C" void getMousePos(long&, long&) __attribute__((stdcall));
extern "C" void moveMouse(long, long) __attribute__((stdcall));
extern "C" void windMouse(long, long) __attribute__((stdcall));
extern "C" void dragMouse(long, long) __attribute__((stdcall));
extern "C" void holdMouse(long, long, bool) __attribute__((stdcall));
extern "C" void releaseMouse(long, long, bool) __attribute__((stdcall));
extern "C" void clickMouse(long, long, bool) __attribute__((stdcall));

#endif	/* _INPUT_H */

