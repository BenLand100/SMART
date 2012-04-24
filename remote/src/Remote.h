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

typedef void (*SetupRemote)(char*,char*,long,long,void*,void*,char*) __attribute__((cdecl));
typedef void (*SetUserAgent)(char*) __attribute__((cdecl));
typedef void (*SetJVMPath)(char*) __attribute__((cdecl));
typedef void (*SetMaxJVMMem)(int) __attribute__((cdecl));

#ifndef _WIN32
#include <dlfcn.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#endif	/* _REMOTE_H */

