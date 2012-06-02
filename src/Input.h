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

extern "C" void sendKeys(char*,int,int);
extern "C" bool isKeyDown(int);
extern "C" void holdKey(int);
extern "C" void releaseKey(int);
extern "C" void getMousePos(int&, int&);
extern "C" void moveMouse(int, int);
extern "C" void windMouse(int, int);
extern "C" void holdMouse(int, int, bool);
extern "C" void releaseMouse(int, int, bool);
extern "C" void clickMouse(int, int, bool);
extern "C" void holdMousePlus(int, int, int);
extern "C" void releaseMousePlus(int, int, int);
extern "C" void clickMousePlus(int, int, int);
extern "C" bool isMouseButtonHeld(int);

#endif	/* _INPUT_H */

