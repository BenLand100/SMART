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

#ifndef _MAIN_H
#define	_MAIN_H

#ifdef LINUX

void load() __attribute__ ((constructor));
void unload() __attribute__ ((destructor));
extern "C" long GetFunctionCount() __attribute__((stdcall));
extern "C" long GetFunctionInfo(int, void*&, char*&) __attribute__((stdcall));

#endif

#ifdef WINDOWS

#include <windows.h>

extern "C" bool DllMain(HINSTANCE, int, void*) __attribute__((stdcall));
extern "C" void GetFunctionCount() __attribute__((cdecl));
extern "C" void GetFunctionInfo() __attribute__((cdecl));
extern "C" long functionCount() __attribute__((stdcall));
extern "C" long functionInfo(int, void*&, char*&) __attribute__((stdcall));

#endif

#endif	/* _MAIN_H */

