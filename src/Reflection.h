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

#ifndef _REFLECTION_H
#define	_REFLECTION_H

#include "jni.h"

extern "C" int stringFromBytes(void*, char*) __attribute__((cdecl));
extern "C" int stringFromChars(void*, char*) __attribute__((cdecl));
extern "C" int stringFromString(void*, char*) __attribute__((cdecl));
extern "C" int invokeIntMethod(void* object, char* classname, char* methname, int a, int b) __attribute__((cdecl));
extern "C" void freeObject(void*) __attribute__((cdecl));
extern "C" void* getFieldObject(void*, char*) __attribute__((cdecl));
extern "C" long getFieldInt(void*, char*) __attribute__((cdecl));
extern "C" long getFieldShort(void*, char*) __attribute__((cdecl));
extern "C" long getFieldByte(void*, char*) __attribute__((cdecl));
extern "C" bool getFieldBool(void*, char*) __attribute__((cdecl));
extern "C" long getFieldLongH(void*, char*) __attribute__((cdecl));
extern "C" long getFieldLongL(void*, char*) __attribute__((cdecl));
extern "C" float getFieldFloat(void*, char*) __attribute__((cdecl));
extern "C" float getFieldDouble(void*, char*) __attribute__((cdecl));
extern "C" void* getFieldArrayObject(void*, char*, int) __attribute__((cdecl));
extern "C" long getFieldArrayInt(void*, char*, int) __attribute__((cdecl));
extern "C" float getFieldArrayFloat(void*, char*, int) __attribute__((cdecl));
extern "C" float getFieldArrayDouble(void*, char*, int) __attribute__((cdecl));
extern "C" bool getFieldArrayBool(void*, char*, int) __attribute__((cdecl));
extern "C" long getFieldArrayLongH(void*, char*, int) __attribute__((cdecl));
extern "C" long getFieldArrayLongL(void*, char*, int) __attribute__((cdecl));
extern "C" long getFieldArrayByte(void*, char*, int) __attribute__((cdecl));
extern "C" long getFieldArrayShort(void*, char*, int) __attribute__((cdecl));
extern "C" long getFieldArrayChar(void*, char*, int) __attribute__((cdecl));
extern "C" void* getFieldArray3DObject(void*, char*, int, int, int) __attribute__((cdecl));
extern "C" long getFieldArray3DInt(void*, char*, int, int, int) __attribute__((cdecl));
extern "C" float getFieldArray3DFloat(void*, char*, int, int, int) __attribute__((cdecl));
extern "C" float getFieldArray3DDouble(void*, char*, int, int, int) __attribute__((cdecl));
extern "C" bool getFieldArray3DBool(void*, char*, int, int, int) __attribute__((cdecl));
extern "C" long getFieldArray3DLongH(void*, char*, int, int, int) __attribute__((cdecl));
extern "C" long getFieldArray3DLongL(void*, char*, int, int, int) __attribute__((cdecl));
extern "C" long getFieldArray3DByte(void*, char*, int, int, int) __attribute__((cdecl));
extern "C" long getFieldArray3DShort(void*, char*, int, int, int) __attribute__((cdecl));
extern "C" long getFieldArray3DChar(void*, char*, int, int, int) __attribute__((cdecl));
extern "C" void* getFieldArray2DObject(void*, char*, int, int) __attribute__((cdecl));
extern "C" long getFieldArray2DInt(void*, char*, int, int) __attribute__((cdecl));
extern "C" float getFieldArray2DFloat(void*, char*, int, int) __attribute__((cdecl));
extern "C" float getFieldArray2DDouble(void*, char*, int, int) __attribute__((cdecl));
extern "C" bool getFieldArray2DBool(void*, char*, int, int) __attribute__((cdecl));
extern "C" long getFieldArray2DLongH(void*, char*, int, int) __attribute__((cdecl));
extern "C" long getFieldArray2DLongL(void*, char*, int, int) __attribute__((cdecl));
extern "C" long getFieldArray2DByte(void*, char*, int, int) __attribute__((cdecl));
extern "C" long getFieldArray2DShort(void*, char*, int, int) __attribute__((cdecl));
extern "C" long getFieldArray2DChar(void*, char*, int, int) __attribute__((cdecl));
extern "C" long getFieldArraySize(void*, char*, int) __attribute__((cdecl));
extern "C" bool isEqual(void*, void*) __attribute__((cdecl));
extern "C" bool isNull(void*) __attribute__((cdecl));
extern "C" bool isPathValid(void*, char*) __attribute__((cdecl));

#endif	/* _REFLECTION_H */

