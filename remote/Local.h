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
 
#ifndef _LOCAL_H
#define	_LOCAL_H

#ifndef _WIN32

#include <dlfcn.h>

void load() __attribute__((constructor));
void unload() __attribute__((destructor));

#endif

#ifdef WINDOWS

#include <windows.h>

extern "C" bool DllMain(HINSTANCE, int, void*) __attribute__((stdcall));

#endif

//These are intended for use with the SCAR/Simba plugin loaders, and nothing else.
extern "C" int GetFunctionCount() __attribute__((stdcall));
extern "C" int GetFunctionInfo(int, void*&, char*&) __attribute__((stdcall));

#endif	/* _LOCAL_H */

