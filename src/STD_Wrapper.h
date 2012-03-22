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

//All of the stdcall wrappers defined here

#ifndef _STD_WRAPPER_H
#define	_STD_WRAPPER_H

//from JVM.h

extern "C" void std_setJVMPath(char* path) __attribute__((stdcall));
extern "C" void std_setMaxJVMMem(int mem) __attribute__((stdcall));

//from Smart.h

#ifdef WINDOWS
extern "C" void* std_getImageHDC() __attribute__((stdcall));
extern "C" void* std_getDebugHDC() __attribute__((stdcall));
#endif
extern "C" void* std_getImageArray() __attribute__((stdcall));
extern "C" void std_setTransparentColor(int color) __attribute__((stdcall));
extern "C" void std_setDebug(bool on) __attribute__((stdcall));
extern "C" void std_setGraphics(bool on) __attribute__((stdcall));
extern "C" void std_setEnabled(bool on) __attribute__((stdcall));
extern "C" void* std_getDebugArray() __attribute__((stdcall));
extern "C" bool std_isActive() __attribute__((stdcall));
extern "C" bool std_isBlocking() __attribute__((stdcall));
extern "C" int std_getRefresh() __attribute__((stdcall));
extern "C" void std_setRefresh(int x) __attribute__((stdcall));
extern "C" void std_setup(char*,char*,int,int,char*) __attribute__((stdcall));
extern "C" void std_hardReset() __attribute__((stdcall));
extern "C" void std_setUserAgent(char*) __attribute__((stdcall));

//from Reflection.h

extern "C" int std_stringFromBytes(void*, char*) __attribute__((stdcall));
extern "C" int std_stringFromChars(void*, char*) __attribute__((stdcall));
extern "C" int std_stringFromString(void*, char*) __attribute__((stdcall));
extern "C" int std_invokeIntMethod(void* object, char* classname, char* methname, int a, int b) __attribute__((stdcall));
extern "C" void std_freeObject(void*) __attribute__((stdcall));
extern "C" void* std_getFieldObject(void*, char*) __attribute__((stdcall));
extern "C" int std_getFieldInt(void*, char*) __attribute__((stdcall));
extern "C" int std_getFieldShort(void*, char*) __attribute__((stdcall));
extern "C" int std_getFieldByte(void*, char*) __attribute__((stdcall));
extern "C" bool std_getFieldBool(void*, char*) __attribute__((stdcall));
extern "C" int std_getFieldLongH(void*, char*) __attribute__((stdcall));
extern "C" int std_getFieldLongL(void*, char*) __attribute__((stdcall));
extern "C" float std_getFieldFloat(void*, char*) __attribute__((stdcall));
extern "C" float std_getFieldDouble(void*, char*) __attribute__((stdcall));
extern "C" void* std_getFieldArrayObject(void*, char*, int) __attribute__((stdcall));
extern "C" int std_getFieldArrayInt(void*, char*, int) __attribute__((stdcall));
extern "C" float std_getFieldArrayFloat(void*, char*, int) __attribute__((stdcall));
extern "C" float std_getFieldArrayDouble(void*, char*, int) __attribute__((stdcall));
extern "C" bool std_getFieldArrayBool(void*, char*, int) __attribute__((stdcall));
extern "C" int std_getFieldArrayLongH(void*, char*, int) __attribute__((stdcall));
extern "C" int std_getFieldArrayLongL(void*, char*, int) __attribute__((stdcall));
extern "C" int std_getFieldArrayByte(void*, char*, int) __attribute__((stdcall));
extern "C" int std_getFieldArrayShort(void*, char*, int) __attribute__((stdcall));
extern "C" int std_getFieldArrayChar(void*, char*, int) __attribute__((stdcall));
extern "C" void* std_getFieldArray3DObject(void*, char*, int, int, int) __attribute__((stdcall));
extern "C" int std_getFieldArray3DInt(void*, char*, int, int, int) __attribute__((stdcall));
extern "C" float std_getFieldArray3DFloat(void*, char*, int, int, int) __attribute__((stdcall));
extern "C" float std_getFieldArray3DDouble(void*, char*, int, int, int) __attribute__((stdcall));
extern "C" bool std_getFieldArray3DBool(void*, char*, int, int, int) __attribute__((stdcall));
extern "C" int std_getFieldArray3DLongH(void*, char*, int, int, int) __attribute__((stdcall));
extern "C" int std_getFieldArray3DLongL(void*, char*, int, int, int) __attribute__((stdcall));
extern "C" int std_getFieldArray3DByte(void*, char*, int, int, int) __attribute__((stdcall));
extern "C" int std_getFieldArray3DShort(void*, char*, int, int, int) __attribute__((stdcall));
extern "C" int std_getFieldArray3DChar(void*, char*, int, int, int) __attribute__((stdcall));
extern "C" void* std_getFieldArray2DObject(void*, char*, int, int) __attribute__((stdcall));
extern "C" int std_getFieldArray2DInt(void*, char*, int, int) __attribute__((stdcall));
extern "C" float std_getFieldArray2DFloat(void*, char*, int, int) __attribute__((stdcall));
extern "C" float std_getFieldArray2DDouble(void*, char*, int, int) __attribute__((stdcall));
extern "C" bool std_getFieldArray2DBool(void*, char*, int, int) __attribute__((stdcall));
extern "C" int std_getFieldArray2DLongH(void*, char*, int, int) __attribute__((stdcall));
extern "C" int std_getFieldArray2DLongL(void*, char*, int, int) __attribute__((stdcall));
extern "C" int std_getFieldArray2DByte(void*, char*, int, int) __attribute__((stdcall));
extern "C" int std_getFieldArray2DShort(void*, char*, int, int) __attribute__((stdcall));
extern "C" int std_getFieldArray2DChar(void*, char*, int, int) __attribute__((stdcall));
extern "C" int std_getFieldArraySize(void*, char*, int) __attribute__((stdcall));
extern "C" bool std_isEqual(void*, void*) __attribute__((stdcall));
extern "C" bool std_isNull(void*) __attribute__((stdcall));
extern "C" bool std_isPathValid(void*, char*) __attribute__((stdcall));

//from Input.h

#include "Smart.h"

extern "C" void std_sendKeys(char*) __attribute__((stdcall));
extern "C" bool std_isKeyDown(int) __attribute__((stdcall));
extern "C" void std_holdKey(int) __attribute__((stdcall));
extern "C" void std_releaseKey(int) __attribute__((stdcall));
extern "C" void std_getMousePos(int&, int&) __attribute__((stdcall));
extern "C" void std_moveMouse(int, int) __attribute__((stdcall));
extern "C" void std_windMouse(int, int) __attribute__((stdcall));
extern "C" void std_holdMouse(int, int, bool) __attribute__((stdcall));
extern "C" void std_releaseMouse(int, int, bool) __attribute__((stdcall));
extern "C" void std_clickMouse(int, int, bool) __attribute__((stdcall));
extern "C" void std_holdMousePlus(int, int, int) __attribute__((stdcall));
extern "C" void std_releaseMousePlus(int, int, int) __attribute__((stdcall));
extern "C" void std_clickMousePlus(int, int, int) __attribute__((stdcall));
extern "C" bool std_isMouseButtonHeld( int) __attribute__((stdcall));

//from Color.h
extern "C" int  std_getColor(int, int) __attribute__((stdcall));
extern "C" bool std_findColor(int&, int&, int, int, int, int, int) __attribute__((stdcall));
extern "C" bool std_findColorTol(int&, int&, int, int, int, int, int, int) __attribute__((stdcall));
extern "C" bool std_findColorSpiral(int&, int&, int, int, int, int, int) __attribute__((stdcall));
extern "C" bool std_findColorSpiralTol(int&, int&, int, int, int, int, int, int) __attribute__((stdcall));
/*
Unused and thus no reason to make cross compatible --- but still faster than anything else so leave in
extern "C" bool std_findBitmapTolIn(int, int&, int&, int, int, int, int, int) __attribute__((stdcall));
extern "C" bool std_findBitmapTol(int, int&, int&, int, int, int, int) __attribute__((stdcall));
extern "C" bool std_findBitmap(int, int&, int&) __attribute__((stdcall));
*/

#endif

