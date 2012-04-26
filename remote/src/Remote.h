/**
 *  Copyright 2012 by Benjamin J. Land (a.k.a. BenLand100)
 *
 *  This file is part of the SMART-Remote
 *
 *  SMART-Remote is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SMART-Remote is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SMART-Remote. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _REMOTE_H
#define	_REMOTE _H

//SMART exports used to setup the client
typedef void (*SetupRemote)(char*,char*,long,long,void*,void*,char*) __attribute__((cdecl));
typedef void (*SetUserAgent)(char*) __attribute__((cdecl));
typedef void (*SetJVMPath)(char*) __attribute__((cdecl));
typedef void (*SetMaxJVMMem)(int) __attribute__((cdecl));

//Remote uses these typedefs to invoke SMART exports
typedef void* (*type_getImageArray)() __attribute__((cdecl));
typedef void* (*type_getDebugArray)() __attribute__((cdecl));
typedef int (*type_getRefresh)() __attribute__((cdecl));
typedef void (*type_setRefresh)(int x) __attribute__((cdecl));
typedef void (*type_setTransparentColor)(int color) __attribute__((cdecl));
typedef void (*type_setDebug)(bool enabled) __attribute__((cdecl));
typedef void (*type_setGraphics)(bool enabled) __attribute__((cdecl));
typedef void (*type_setEnabled)(bool enabled) __attribute__((cdecl));
typedef bool (*type_isActive)() __attribute__((cdecl));
typedef bool (*type_isBlocking)() __attribute__((cdecl));
typedef void (*type_getMousePos)(int &x, int &y) __attribute__((cdecl));
typedef void (*type_holdMouse)(int x, int y, bool left) __attribute__((cdecl));
typedef void (*type_releaseMouse)(int x, int y, bool left) __attribute__((cdecl));
typedef void (*type_holdMousePlus)(int x, int y, int button) __attribute__((cdecl));
typedef void (*type_releaseMousePlus)(int x, int y, int button) __attribute__((cdecl));
typedef void (*type_moveMouse)(int x, int y) __attribute__((cdecl));
typedef void (*type_windMouse)(int x, int y) __attribute__((cdecl));
typedef void (*type_clickMouse)(int x, int y, bool left) __attribute__((cdecl));
typedef void (*type_clickMousePlus)(int x, int y, int button) __attribute__((cdecl));
typedef bool (*type_isMouseButtonHeld)(int button) __attribute__((cdecl));
typedef void (*type_sendKeys)(char *text) __attribute__((cdecl));
typedef void (*type_holdKey)(int code) __attribute__((cdecl));
typedef void (*type_releaseKey)(int code) __attribute__((cdecl));
typedef bool (*type_isKeyDown)(int code) __attribute__((cdecl));
typedef int (*type_getColor)(int x, int y) __attribute__((cdecl));
typedef bool (*type_findColor)(int &x, int& y, int color, int sx, int sy, int ex, int ey) __attribute__((cdecl));
typedef bool (*type_findColorTol)(int &x, int& y, int color, int sx, int sy, int ex, int ey, int tol) __attribute__((cdecl));
typedef bool (*type_findColorSpiral)(int &x, int& y, int color, int sx, int sy, int ex, int ey) __attribute__((cdecl));
typedef bool (*type_findColorSpiralTol)(int &x, int& y, int color, int sx, int sy, int ex, int ey, int tol) __attribute__((cdecl));

//Imports for Remote
#define NumImports 27
static char* imports[] = {
    (char*)"getRefresh",
    (char*)"setRefresh", 
    (char*)"setTransparentColor", 
    (char*)"setDebug",
    (char*)"setGraphics",
    (char*)"setEnabled", 
    (char*)"isActive", 
    (char*)"isBlocking", 
    (char*)"getMousePos",
    (char*)"holdMouse",
    (char*)"releaseMouse",
    (char*)"holdMousePlus",
    (char*)"releaseMousePlus",
    (char*)"moveMouse", 
    (char*)"windMouse", 
    (char*)"clickMouse",
    (char*)"clickMousePlus",
    (char*)"isMouseButtonHeld",
    (char*)"sendKeys",
    (char*)"holdKey", 
    (char*)"releaseKey", 
    (char*)"isKeyDown", 
    (char*)"getColor", 
    (char*)"findColor", 
    (char*)"findColorTol", 
    (char*)"findColorSpiral",
    (char*)"findColorSpiralTol",
};

#ifndef _WIN32
#include <dlfcn.h>
#endif

#ifdef _WIN32
#define _WIN32_WINNT 0x0501
#include <windows.h>
#endif

#endif	/* _REMOTE_H */

