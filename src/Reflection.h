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

extern "C" int stringFromBytes(jobject, char*) __attribute__((stdcall));
extern "C" int stringFromChars(jobject, char*) __attribute__((stdcall));
extern "C" int stringFromString(jobject, char*) __attribute__((stdcall));
extern "C" int invokeIntMethod(jobject object, char* classname, char* methname, int a, int b) __attribute__((stdcall));
extern "C" void freeObject(jobject) __attribute__((stdcall));
extern "C" long getFieldObject(jobject, char*) __attribute__((stdcall));
extern "C" long getFieldInt(jobject, char*) __attribute__((stdcall));
extern "C" bool getFieldBool(jobject, char*) __attribute__((stdcall));
extern "C" long getFieldLongH(jobject, char*) __attribute__((stdcall));
extern "C" long getFieldLongL(jobject, char*) __attribute__((stdcall));
extern "C" float getFieldFloat(jobject, char*) __attribute__((stdcall));
extern "C" float getFieldDouble(jobject, char*) __attribute__((stdcall));
extern "C" long getFieldArrayObject(jobject, char*, int) __attribute__((stdcall));
extern "C" long getFieldArrayInt(jobject, char*, int) __attribute__((stdcall));
extern "C" float getFieldArrayFloat(jobject, char*, int) __attribute__((stdcall));
extern "C" float getFieldArrayDouble(jobject, char*, int) __attribute__((stdcall));
extern "C" bool getFieldArrayBool(jobject, char*, int) __attribute__((stdcall));
extern "C" long getFieldArrayLongH(jobject, char*, int) __attribute__((stdcall));
extern "C" long getFieldArrayLongL(jobject, char*, int) __attribute__((stdcall));
extern "C" long getFieldArrayByte(jobject, char*, int) __attribute__((stdcall));
extern "C" long getFieldArrayShort(jobject, char*, int) __attribute__((stdcall));
extern "C" long getFieldArrayChar(jobject, char*, int) __attribute__((stdcall));
extern "C" long getFieldArray3DObject(jobject, char*, int, int, int) __attribute__((stdcall));
extern "C" long getFieldArray3DInt(jobject, char*, int, int, int) __attribute__((stdcall));
extern "C" float getFieldArray3DFloat(jobject, char*, int, int, int) __attribute__((stdcall));
extern "C" float getFieldArray3DDouble(jobject, char*, int, int, int) __attribute__((stdcall));
extern "C" bool getFieldArray3DBool(jobject, char*, int, int, int) __attribute__((stdcall));
extern "C" long getFieldArray3DLongH(jobject, char*, int, int, int) __attribute__((stdcall));
extern "C" long getFieldArray3DLongL(jobject, char*, int, int, int) __attribute__((stdcall));
extern "C" long getFieldArray3DByte(jobject, char*, int, int, int) __attribute__((stdcall));
extern "C" long getFieldArray3DShort(jobject, char*, int, int, int) __attribute__((stdcall));
extern "C" long getFieldArray3DChar(jobject, char*, int, int, int) __attribute__((stdcall));
extern "C" long getFieldArray2DObject(jobject, char*, int, int) __attribute__((stdcall));
extern "C" long getFieldArray2DInt(jobject, char*, int, int) __attribute__((stdcall));
extern "C" float getFieldArray2DFloat(jobject, char*, int, int) __attribute__((stdcall));
extern "C" float getFieldArray2DDouble(jobject, char*, int, int) __attribute__((stdcall));
extern "C" bool getFieldArray2DBool(jobject, char*, int, int) __attribute__((stdcall));
extern "C" long getFieldArray2DLongH(jobject, char*, int, int) __attribute__((stdcall));
extern "C" long getFieldArray2DLongL(jobject, char*, int, int) __attribute__((stdcall));
extern "C" long getFieldArray2DByte(jobject, char*, int, int) __attribute__((stdcall));
extern "C" long getFieldArray2DShort(jobject, char*, int, int) __attribute__((stdcall));
extern "C" long getFieldArray2DChar(jobject, char*, int, int) __attribute__((stdcall));
extern "C" long getFieldArraySize(jobject, char*) __attribute__((stdcall));

#endif	/* _REFLECTION_H */

