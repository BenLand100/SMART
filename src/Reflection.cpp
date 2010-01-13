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

#include "Reflection.h"
#include "Smart.h"
#include <cstring>

//Defined in Smart.cpp
extern JNIEnv* jre;
extern jobject smart;
extern JCLIENT _client;
extern JSTRING _string;

int stringFromBytes(jobject object, char* delphistr) {
    if (jre && object != 0 && delphistr != 0) {
        jstring str = (jstring) jre->NewObject(_string.clazz, _string.byteInit, object);
        const char* chars = jre->GetStringUTFChars(str, 0);
        int len = strlen(chars);
        strcpy(delphistr, chars);
        jre->ReleaseStringUTFChars(str, chars);
        jre->DeleteLocalRef(str);
        return len;
    }
    return 0;
}

int stringFromChars(jobject object, char* delphistr) {
    if (jre && object != 0 && delphistr != 0) {
        jstring str = (jstring) jre->NewObject(_string.clazz, _string.charInit, object);
        const char* chars = jre->GetStringUTFChars(str, 0);
        int len = strlen(chars);
        strcpy(delphistr, chars);
        jre->ReleaseStringUTFChars(str, chars);
        jre->DeleteLocalRef(str);
        return len;
    }
    return 0;
}

int stringFromString(jobject str, char* delphistr) {
    if (jre && str != 0 && delphistr != 0) {
        const char* chars = jre->GetStringUTFChars((jstring)str, 0);
        int len = strlen(chars);
        strcpy(delphistr, chars);
        jre->ReleaseStringUTFChars((jstring)str, chars);
        return len;
    }
    return 0;
}


void freeObject(jobject object) {
    try {
        if (jre) jre->DeleteGlobalRef(object);
    } catch (...) { }
}

long getFieldObject(jobject object, char* path) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            jobject o = jre->CallObjectMethod(smart, _client.getfieldobject, object == 0 ? NULL : object, jpath);
            jre->DeleteLocalRef(jpath);
            if (o != NULL) {
                jobject ref = jre->NewGlobalRef(o);
                jre->DeleteLocalRef(o);
                return (long) ref;
            }
        }
    } catch (...) { }
    return 0;
}

long getFieldInt(jobject object, char* path) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldint, object == 0 ? NULL : object, jpath);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

float getFieldFloat(jobject object, char* path) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            float i = jre->CallFloatMethod(smart, _client.getfieldfloat, object == 0 ? NULL : object, jpath);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1.0;
}

float getFieldDouble(jobject object, char* path) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            float i = jre->CallDoubleMethod(smart, _client.getfielddouble, object == 0 ? NULL : object, jpath);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1.0;
}

bool getFieldBool(jobject object, char* path) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            bool i = jre->CallBooleanMethod(smart, _client.getfieldboolean, object == 0 ? NULL : object, jpath);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

long getFieldLongH(jobject object, char* path) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldlongh, object == 0 ? NULL : object, jpath);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

long getFieldLongL(jobject object, char* path) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldlongl, object == 0 ? NULL : object, jpath);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

long getFieldArrayObject(jobject object, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            jobject o = jre->CallObjectMethod(smart, _client.getfieldarrayobject, object == 0 ? NULL : object, jpath, index);
            jre->DeleteLocalRef(jpath);
            if (o != NULL) {
                jobject ref = jre->NewGlobalRef(o);
                jre->DeleteLocalRef(o);
                return (long) ref;
            }
        }
    } catch (...) { }
    return 0;
}

long getFieldArrayInt(jobject object, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldarrayint, object == 0 ? NULL : object, jpath, index);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

float getFieldArrayFloat(jobject object, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            float i = jre->CallFloatMethod(smart, _client.getfieldarrayfloat, object == 0 ? NULL : object, jpath, index);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1.0;
}

float getFieldArrayDouble(jobject object, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            float i = jre->CallDoubleMethod(smart, _client.getfieldarraydouble, object == 0 ? NULL : object, jpath, index);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1.0;
}

bool getFieldArrayBool(jobject object, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            bool i = jre->CallBooleanMethod(smart, _client.getfieldarrayboolean, object == 0 ? NULL : object, jpath, index);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

long getFieldArrayLongH(jobject object, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldarraylongh, object == 0 ? NULL : object, jpath, index);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

long getFieldArrayLongL(jobject object, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldarraylongl, object == 0 ? NULL : object, jpath, index);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

long getFieldArrayByte(jobject object, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldarraybyte, object == 0 ? NULL : object, jpath, index);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}


long getFieldArrayShort(jobject object, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldarrayshort, object == 0 ? NULL : object, jpath, index);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}


long getFieldArrayChar(jobject object, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldarraychar, object == 0 ? NULL : object, jpath, index);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

long getFieldArray3DObject(jobject object, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            jobject o = jre->CallObjectMethod(smart, _client.getfieldarray3dobject, object == 0 ? NULL : object, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            if (o != NULL) {
                jobject ref = jre->NewGlobalRef(o);
                jre->DeleteLocalRef(o);
                return (long) ref;
            }
        }
    } catch (...) { }
    return 0;
}

long getFieldArray3DInt(jobject object, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldarray3dint, object == 0 ? NULL : object, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

float getFieldArray3DFloat(jobject object, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            float i = jre->CallFloatMethod(smart, _client.getfieldarray3dfloat, object == 0 ? NULL : object, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1.0;
}

float getFieldArray3DDouble(jobject object, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            float i = jre->CallDoubleMethod(smart, _client.getfieldarray3ddouble, object == 0 ? NULL : object, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1.0;
}

bool getFieldArray3DBool(jobject object, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            bool i = jre->CallBooleanMethod(smart, _client.getfieldarray3dboolean, object == 0 ? NULL : object, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

long getFieldArray3DLongH(jobject object, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldarray3dlongh, object == 0 ? NULL : object, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

long getFieldArray3DLongL(jobject object, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldarray3dlongl, object == 0 ? NULL : object, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

long getFieldArray3DByte(jobject object, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldarray3dbyte, object == 0 ? NULL : object, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

long getFieldArray3DShort(jobject object, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldarray3dshort, object == 0 ? NULL : object, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

long getFieldArray3DChar(jobject object, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldarray3dchar, object == 0 ? NULL : object, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

long getFieldArray2DObject(jobject object, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            jobject o = jre->CallObjectMethod(smart, _client.getfieldarray2dobject, object == 0 ? NULL : object, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            if (o != NULL) {
                jobject ref = jre->NewGlobalRef(o);
                jre->DeleteLocalRef(o);
                return (long) ref;
            }
        }
    } catch (...) { }
    return 0;
}

long getFieldArray2DInt(jobject object, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldarray2dint, object == 0 ? NULL : object, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}


float getFieldArray2DDouble(jobject object, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            float i = jre->CallDoubleMethod(smart, _client.getfieldarray2ddouble, object == 0 ? NULL : object, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1.0;
}


float getFieldArray2DFloat(jobject object, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            float i = jre->CallFloatMethod(smart, _client.getfieldarray2dfloat, object == 0 ? NULL : object, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1.0;
}

bool getFieldArray2DBool(jobject object, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            bool i = jre->CallBooleanMethod(smart, _client.getfieldarray2dboolean, object == 0 ? NULL : object, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

long getFieldArray2DLongH(jobject object, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldarray2dlongh, object == 0 ? NULL : object, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

long getFieldArray2DLongL(jobject object, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldarray2dlongl, object == 0 ? NULL : object, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

long getFieldArray2DByte(jobject object, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldarray2dbyte, object == 0 ? NULL : object, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

long getFieldArray2DChar(jobject object, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldarray2dchar, object == 0 ? NULL : object, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

long getFieldArray2DShort(jobject object, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldarray2dshort, object == 0 ? NULL : object, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

long getFieldArraySize(jobject object, char* path) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            long i = jre->CallIntMethod(smart, _client.getfieldarraysize, object == 0 ? NULL : object, jpath);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}
