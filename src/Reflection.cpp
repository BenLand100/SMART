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

//Reflection routines, callbacks through JNI to the Java end

#include "Reflection.h"
#include "Smart.h"
#include <cstring>

//Defined in Smart.cpp
extern JNIEnv* jre;
extern jobject smart;
extern JCLIENT _client;
extern JSTRING _string;

//Fills the given char* with the string represented by the reference to a java byte array in obj
int stringFromBytes(void* obj, char* delphistr) {
    if (jre && obj != 0 && delphistr != 0) {
        jstring str = (jstring) jre->NewObject(_string.clazz, _string.byteInit, (jobject)obj);
        const char* chars = jre->GetStringUTFChars(str, 0);
        int len = strlen(chars);
        strcpy(delphistr, chars);
        jre->ReleaseStringUTFChars(str, chars);
        jre->DeleteLocalRef(str);
        return len;
    }
    return 0;
}

//Fills the given char* with the string represented by the reference to a java char array in obj
int stringFromChars(void* obj, char* delphistr) {
    if (jre && obj != 0 && delphistr != 0) {
        jstring str = (jstring) jre->NewObject(_string.clazz, _string.charInit, (jobject)obj);
        const char* chars = jre->GetStringUTFChars(str, 0);
        int len = strlen(chars);
        strcpy(delphistr, chars);
        jre->ReleaseStringUTFChars(str, chars);
        jre->DeleteLocalRef(str);
        return len;
    }
    return 0;
}

//Fills the given char* with the string represented by the reference to java string
int stringFromString(void* str, char* delphistr) {
    if (jre && str != 0 && delphistr != 0) {
        const char* chars = jre->GetStringUTFChars((jstring)str, 0);
        int len = strlen(chars);
        strcpy(delphistr, chars);
        jre->ReleaseStringUTFChars((jstring)str, chars);
        return len;
    }
    return 0;
}

//Calls a method that can take two ints and return an int --- historical
int invokeIntMethod(void* obj, char* classname, char* methname, int a, int b) {
    try {
      jclass classref = jre->FindClass(classname);
      if (classref) {
        jre->DeleteLocalRef(classref);
        jmethodID meth = jre->GetMethodID(classref, methname, "(II)I");
        if (meth) {
          return jre->CallIntMethod((jobject)obj,meth,a,b);
        }
      }
    } catch (...) { }
    return -1;
}

//Frees a java object refrence returned by the getField[...]Object methods
//ALL must be freed or seriosly bad shit will happen
void freeObject(void* obj) {
    try {
        if (jre) jre->DeleteGlobalRef((jobject)obj);
    } catch (...) { }
}

//***
//The following getField[...] methods take an object refrence which is known
//as the parent, or zero for static scope, and a path that basically amounts to fields
//in objects or classes and static fields (if in the static scope), and return the
//value of the field. The proper return type should ALWAYS be used. Conveniance methods
//are provided for 1, 2, and 3 dimensional arrays
//***

void* getFieldObject(void* obj, char* path) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            jobject o = jre->CallObjectMethod(smart, _client.getfieldobject, obj == 0 ? NULL : (jobject)obj, jpath);
            jre->DeleteLocalRef(jpath);
            if (o != NULL) {
                jobject ref = jre->NewGlobalRef(o);
                jre->DeleteLocalRef(o);
                return (void*) ref;
            }
        }
    } catch (...) { }
    return 0;
}

bool isPathValid(void* obj, char* path) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            jboolean b = jre->CallBooleanMethod(smart, _client.ispathvalid, obj == 0 ? NULL : (jobject)obj, jpath);
            jre->DeleteLocalRef(jpath);
            return b;
        }
    } catch (...) { }
    return 0;
}

int getFieldInt(void* obj, char* path) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldint, obj == 0 ? NULL : (jobject)obj, jpath);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

int getFieldShort(void* obj, char* path) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldshort, obj == 0 ? NULL : (jobject)obj, jpath);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}


int getFieldByte(void* obj, char* path) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallByteMethod(smart, _client.getfieldbyte, obj == 0 ? NULL : (jobject)obj, jpath);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

float getFieldFloat(void* obj, char* path) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            float i = jre->CallFloatMethod(smart, _client.getfieldfloat, obj == 0 ? NULL : (jobject)obj, jpath);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1.0;
}

float getFieldDouble(void* obj, char* path) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            float i = jre->CallDoubleMethod(smart, _client.getfielddouble, obj == 0 ? NULL : (jobject)obj, jpath);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1.0;
}

bool getFieldBool(void* obj, char* path) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            bool i = jre->CallBooleanMethod(smart, _client.getfieldboolean, obj == 0 ? NULL : (jobject)obj, jpath);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

int getFieldLongH(void* obj, char* path) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldlongh, obj == 0 ? NULL : (jobject)obj, jpath);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

int getFieldLongL(void* obj, char* path) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldlongl, obj == 0 ? NULL : (jobject)obj, jpath);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

void* getFieldArrayObject(void* obj, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            jobject o = jre->CallObjectMethod(smart, _client.getfieldarrayobject, obj == 0 ? NULL : (jobject)obj, jpath, index);
            jre->DeleteLocalRef(jpath);
            if (o != NULL) {
                jobject ref = jre->NewGlobalRef(o);
                jre->DeleteLocalRef(o);
                return (void*) ref;
            }
        }
    } catch (...) { }
    return 0;
}

int getFieldArrayInt(void* obj, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldarrayint, obj == 0 ? NULL : (jobject)obj, jpath, index);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

float getFieldArrayFloat(void* obj, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            float i = jre->CallFloatMethod(smart, _client.getfieldarrayfloat, obj == 0 ? NULL : (jobject)obj, jpath, index);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1.0;
}

float getFieldArrayDouble(void* obj, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            float i = jre->CallDoubleMethod(smart, _client.getfieldarraydouble, obj == 0 ? NULL : (jobject)obj, jpath, index);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1.0;
}

bool getFieldArrayBool(void* obj, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            bool i = jre->CallBooleanMethod(smart, _client.getfieldarrayboolean, obj == 0 ? NULL : (jobject)obj, jpath, index);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

int getFieldArrayLongH(void* obj, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldarraylongh, obj == 0 ? NULL : (jobject)obj, jpath, index);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

int getFieldArrayLongL(void* obj, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldarraylongl, obj == 0 ? NULL : (jobject)obj, jpath, index);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

int getFieldArrayByte(void* obj, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldarraybyte, obj == 0 ? NULL : (jobject)obj, jpath, index);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}


int getFieldArrayShort(void* obj, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldarrayshort, obj == 0 ? NULL : (jobject)obj, jpath, index);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}


int getFieldArrayChar(void* obj, char* path, int index) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldarraychar, obj == 0 ? NULL : (jobject)obj, jpath, index);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

void* getFieldArray3DObject(void* obj, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            jobject o = jre->CallObjectMethod(smart, _client.getfieldarray3dobject, obj == 0 ? NULL : (jobject)obj, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            if (o != NULL) {
                jobject ref = jre->NewGlobalRef(o);
                jre->DeleteLocalRef(o);
                return (void*) ref;
            }
        }
    } catch (...) { }
    return 0;
}

int getFieldArray3DInt(void* obj, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldarray3dint, obj == 0 ? NULL : (jobject)obj, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

float getFieldArray3DFloat(void* obj, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            float i = jre->CallFloatMethod(smart, _client.getfieldarray3dfloat, obj == 0 ? NULL : (jobject)obj, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1.0;
}

float getFieldArray3DDouble(void* obj, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            float i = jre->CallDoubleMethod(smart, _client.getfieldarray3ddouble, obj == 0 ? NULL : (jobject)obj, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1.0;
}

bool getFieldArray3DBool(void* obj, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            bool i = jre->CallBooleanMethod(smart, _client.getfieldarray3dboolean, obj == 0 ? NULL : (jobject)obj, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

int getFieldArray3DLongH(void* obj, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldarray3dlongh, obj == 0 ? NULL : (jobject)obj, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

int getFieldArray3DLongL(void* obj, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldarray3dlongl, obj == 0 ? NULL : (jobject)obj, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

int getFieldArray3DByte(void* obj, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldarray3dbyte, obj == 0 ? NULL : (jobject)obj, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

int getFieldArray3DShort(void* obj, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldarray3dshort, obj == 0 ? NULL : (jobject)obj, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

int getFieldArray3DChar(void* obj, char* path, int x, int y, int z) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldarray3dchar, obj == 0 ? NULL : (jobject)obj, jpath, x, y, z);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

void* getFieldArray2DObject(void* obj, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            jobject o = jre->CallObjectMethod(smart, _client.getfieldarray2dobject, obj == 0 ? NULL : (jobject)obj, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            if (o != NULL) {
                jobject ref = jre->NewGlobalRef(o);
                jre->DeleteLocalRef(o);
                return (void*) ref;
            }
        }
    } catch (...) { }
    return 0;
}

int getFieldArray2DInt(void* obj, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldarray2dint, obj == 0 ? NULL : (jobject)obj, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}


float getFieldArray2DDouble(void* obj, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            float i = jre->CallDoubleMethod(smart, _client.getfieldarray2ddouble, obj == 0 ? NULL : (jobject)obj, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1.0;
}


float getFieldArray2DFloat(void* obj, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            float i = jre->CallFloatMethod(smart, _client.getfieldarray2dfloat, obj == 0 ? NULL : (jobject)obj, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1.0;
}

bool getFieldArray2DBool(void* obj, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            bool i = jre->CallBooleanMethod(smart, _client.getfieldarray2dboolean, obj == 0 ? NULL : (jobject)obj, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

int getFieldArray2DLongH(void* obj, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldarray2dlongh, obj == 0 ? NULL : (jobject)obj, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

int getFieldArray2DLongL(void* obj, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldarray2dlongl, obj == 0 ? NULL : (jobject)obj, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

int getFieldArray2DByte(void* obj, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldarray2dbyte, obj == 0 ? NULL : (jobject)obj, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

int getFieldArray2DChar(void* obj, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldarray2dchar, obj == 0 ? NULL : (jobject)obj, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

int getFieldArray2DShort(void* obj, char* path, int x, int y) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldarray2dshort, obj == 0 ? NULL : (jobject)obj, jpath, x, y);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

int getFieldArraySize(void* obj, char* path, int dim) {
    try {
        if (jre) {
            jobject jpath = jre->NewStringUTF(path);
            int i = jre->CallIntMethod(smart, _client.getfieldarraysize, obj == 0 ? NULL : (jobject)obj, jpath, dim);
            jre->DeleteLocalRef(jpath);
            return i;
        }
    } catch (...) { }
    return -1;
}

//Tests if two object refrences (that might not be numerically equal) point
//to the same java object
bool isEqual(void* a, void* b) {
    try {
        if (jre) {
            return jre->IsSameObject((jobject)a,(jobject)b);
        }
    } catch (...) { }
    return 0;
}

//Tests if an object refrence (that might not be numerically 0) is a null refrence
bool isNull(void* a) {
    try {
        if (jre) {
            return jre->IsSameObject((jobject)a,NULL);
        }
    } catch (...) { }
    return 0;
}
