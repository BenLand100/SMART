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

extern "C" int stringFromBytes(void*, char*);
extern "C" int stringFromChars(void*, char*);
extern "C" int stringFromString(void*, char*);
extern "C" int invokeIntMethod(void* object, char* classname, char* methname, int a, int b);
extern "C" void freeObject(void*);
extern "C" void* getFieldObject(void*, char*);
extern "C" int getFieldInt(void*, char*);
extern "C" int getFieldShort(void*, char*);
extern "C" int getFieldByte(void*, char*);
extern "C" bool getFieldBool(void*, char*);
extern "C" int getFieldLongH(void*, char*);
extern "C" int getFieldLongL(void*, char*);
extern "C" float getFieldFloat(void*, char*);
extern "C" float getFieldDouble(void*, char*);
extern "C" void* getFieldArrayObject(void*, char*, int);
extern "C" int getFieldArrayInt(void*, char*, int);
extern "C" float getFieldArrayFloat(void*, char*, int);
extern "C" float getFieldArrayDouble(void*, char*, int);
extern "C" bool getFieldArrayBool(void*, char*, int);
extern "C" int getFieldArrayLongH(void*, char*, int);
extern "C" int getFieldArrayLongL(void*, char*, int);
extern "C" int getFieldArrayByte(void*, char*, int);
extern "C" int getFieldArrayShort(void*, char*, int);
extern "C" int getFieldArrayChar(void*, char*, int);
extern "C" void* getFieldArray3DObject(void*, char*, int, int, int);
extern "C" int getFieldArray3DInt(void*, char*, int, int, int);
extern "C" float getFieldArray3DFloat(void*, char*, int, int, int);
extern "C" float getFieldArray3DDouble(void*, char*, int, int, int);
extern "C" bool getFieldArray3DBool(void*, char*, int, int, int);
extern "C" int getFieldArray3DLongH(void*, char*, int, int, int);
extern "C" int getFieldArray3DLongL(void*, char*, int, int, int);
extern "C" int getFieldArray3DByte(void*, char*, int, int, int);
extern "C" int getFieldArray3DShort(void*, char*, int, int, int);
extern "C" int getFieldArray3DChar(void*, char*, int, int, int);
extern "C" void* getFieldArray2DObject(void*, char*, int, int);
extern "C" int getFieldArray2DInt(void*, char*, int, int);
extern "C" float getFieldArray2DFloat(void*, char*, int, int);
extern "C" float getFieldArray2DDouble(void*, char*, int, int);
extern "C" bool getFieldArray2DBool(void*, char*, int, int);
extern "C" int getFieldArray2DLongH(void*, char*, int, int);
extern "C" int getFieldArray2DLongL(void*, char*, int, int);
extern "C" int getFieldArray2DByte(void*, char*, int, int);
extern "C" int getFieldArray2DShort(void*, char*, int, int);
extern "C" int getFieldArray2DChar(void*, char*, int, int);
extern "C" int getFieldArraySize(void*, char*, int);
extern "C" bool isEqual(void*, void*);
extern "C" bool isNull(void*);
extern "C" bool isPathValid(void*, char*);

#endif	/* _REFLECTION_H */

