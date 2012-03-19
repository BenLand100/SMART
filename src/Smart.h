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

#ifndef _SMART_H
#define	_SMART_H

#include "jni.h"

typedef struct {
	jclass clazz;
	jfieldID x;
	jfieldID y;
} JPOINT;

typedef struct {
	jclass clazz;
	jmethodID byteInit;
	jmethodID charInit;
} JSTRING;

typedef struct {
	jclass clazz;
	jmethodID init;
	jmethodID movemouse;
	jmethodID windmouse;
	jmethodID holdmouse;
	jmethodID releasemouse;
	jmethodID clickmouse;
	jmethodID getmousepos;
	jmethodID holdkey;
	jmethodID iskeydown;
	jmethodID releasekey;
	jmethodID sendkeys;
	jmethodID destroy;
	jmethodID getrefresh;
	jmethodID setrefresh;
	jmethodID getfieldobject;
	jmethodID ispathvalid;
	jmethodID getfieldint;
	jmethodID getfieldshort;
	jmethodID getfieldbyte;
	jmethodID getfieldfloat;
	jmethodID getfielddouble;
	jmethodID getfieldboolean;
	jmethodID getfieldlongh;
	jmethodID getfieldlongl;
	jmethodID getfieldarrayint;
	jmethodID getfieldarrayfloat;
	jmethodID getfieldarraydouble;
	jmethodID getfieldarrayboolean;
	jmethodID getfieldarraylongh;
	jmethodID getfieldarraylongl;
	jmethodID getfieldarraybyte;
	jmethodID getfieldarraychar;
	jmethodID getfieldarrayshort;
	jmethodID getfieldarrayobject;
	jmethodID getfieldarraysize;
	jmethodID getfieldarray3dint;
	jmethodID getfieldarray3dfloat;
	jmethodID getfieldarray3ddouble;
	jmethodID getfieldarray3dboolean;
	jmethodID getfieldarray3dlongh;
	jmethodID getfieldarray3dlongl;
	jmethodID getfieldarray3dbyte;
	jmethodID getfieldarray3dshort;
	jmethodID getfieldarray3dchar;
	jmethodID getfieldarray3dobject;
	jmethodID getfieldarray2dint;
	jmethodID getfieldarray2ddouble;
	jmethodID getfieldarray2dfloat;
	jmethodID getfieldarray2dboolean;
	jmethodID getfieldarray2dlongh;
	jmethodID getfieldarray2dlongl;
	jmethodID getfieldarray2dbyte;
	jmethodID getfieldarray2dshort;
	jmethodID getfieldarray2dchar;
	jmethodID getfieldarray2dobject;
	jmethodID setgraphics;
	jmethodID setdebug;
	jmethodID setenabled;
	jfieldID blocking;
	jfieldID active;
	jfieldID transcolor;
	jfieldID death;
	jmethodID getglobaltime;
} JCLIENT; 

void internalConstructor();
void internalDestructor();

void getDimensions(int *w, int *h);

#ifdef WINDOWS
extern "C" long getImageHDC() __attribute__((cdecl));
extern "C" long getDebugHDC() __attribute__((cdecl));
#endif
extern "C" void* getImageArray() __attribute__((cdecl));
extern "C" void setTransparentColor(jint color) __attribute__((cdecl));
extern "C" void setDebug(bool on) __attribute__((cdecl));
extern "C" void setGraphics(bool on) __attribute__((cdecl));
extern "C" void setEnabled(bool on) __attribute__((cdecl));
extern "C" void* getDebugArray() __attribute__((cdecl));
extern "C" bool isActive() __attribute__((cdecl));
extern "C" bool isBlocking() __attribute__((cdecl));
extern "C" long getRefresh() __attribute__((cdecl));
extern "C" void setRefresh(long x) __attribute__((cdecl));
extern "C" void setup(char*,char*,long,long,char*) __attribute__((cdecl));
extern "C" void hardReset() __attribute__((cdecl));
extern "C" void setUserAgent(char*) __attribute__((cdecl));


#endif	/* _SMART_H */

