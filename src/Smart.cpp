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

#include "Smart.h"
#include "JVM.h"
#include "ClassLoader.h"
#include "Color.h"
#include <cstring>
#include <cstdlib>
#include <iostream>

using namespace std;

#ifdef LINUX
#include <dlfcn.h>
#endif

#ifdef WINDOWS
#include <windows.h>
HDC imageHDC = 0;
HBITMAP imageHBITMAP = 0;
HDC debugHDC = 0;
HBITMAP debugHBITMAP = 0;
#endif

JNIEnv* jre = 0;
JavaVM* vm = 0;
void* jvmdll = 0;

jobject smart = 0;
JCLIENT _client;
JPOINT _point;
JSTRING _string;

char* curserver = 0;
PRGB image = 0;
PRGB debug = 0;

int client_width = 0;
int client_height = 0;

#ifdef WINDOWS

long getImageHDC() {
    return (long) imageHDC;
}

long getDebugHDC() {
    return (long) debugHDC;
}

#endif

long getImageArray() {
    return (long) image;
}

long getDebugArray() {
    return (long) debug;
}

void setDebugColor(jint color) {
    if (jre) jre->SetIntField(smart, _client.transcolor, color);
}

void setDebug(bool on) {
    if (jre) jre->CallVoidMethod(smart, _client.setdebug, on ? 1 : 0);
}

void setGraphics(bool on) {
    if (jre) jre->CallVoidMethod(smart, _client.setgraphics, on ? 1 : 0);
}

bool isActive() {
    return jre ? jre->GetBooleanField(smart, _client.active) : false;
}

bool isBlocking() {
    return jre ? jre->GetBooleanField(smart, _client.blocking) : false;
}

long getRefresh() {
    return jre ? jre->CallIntMethod(smart, _client.getrefresh) : -1;
}

void setRefresh(long x) {
    if (jre) jre->CallVoidMethod(smart, _client.setrefresh, x);
}

void clearOld(){
    if (jre && smart) {
        jre->CallVoidMethod(smart, _client.destroy);
        jre->DeleteGlobalRef(smart);
        smart = 0;
    }
    #ifdef WINDOWS
        if (imageHDC) DeleteDC(imageHDC);
        if (imageHBITMAP) DeleteObject(imageHBITMAP);
        if (debugHDC) DeleteDC(debugHDC);
        if (debugHBITMAP) DeleteObject(debugHBITMAP);
        imageHDC = 0;
        imageHBITMAP = 0;
        debugHDC = 0;
        debugHBITMAP = 0;
    #endif
    #ifdef LINUX
        if (debug) free(debug);
        if (image) free(image);
    #endif
    debug = 0;
    image = 0;
}

void findClasses() {
    loadClasses();
    _client.clazz = (jclass) jre->NewGlobalRef(jre->FindClass("smart/Client"));
    _client.init = jre->GetMethodID(_client.clazz, "<init>", "(Ljava/nio/ByteBuffer;Ljava/nio/ByteBuffer;IILjava/lang/String;Ljava/lang/String;)V");
    _client.clickmouse = jre->GetMethodID(_client.clazz, "clickMouse", "(IIZ)V");
    _client.dragmouse = jre->GetMethodID(_client.clazz, "dragMouse", "(II)V");
    _client.getmousepos = jre->GetMethodID(_client.clazz, "getMousePos", "()Ljava/awt/Point;");
    _client.holdkey = jre->GetMethodID(_client.clazz, "holdKey", "(I)V");
    _client.iskeydown = jre->GetMethodID(_client.clazz, "isKeyDown", "(I)Z");
    _client.holdmouse = jre->GetMethodID(_client.clazz, "holdMouse", "(IIZ)V");
    _client.movemouse = jre->GetMethodID(_client.clazz, "moveMouse", "(II)V");
    _client.windmouse = jre->GetMethodID(_client.clazz, "windMouse", "(II)V");
    _client.releasekey = jre->GetMethodID(_client.clazz, "releaseKey", "(I)V");
    _client.releasemouse = jre->GetMethodID(_client.clazz, "releaseMouse", "(IIZ)V");
    _client.sendkeys = jre->GetMethodID(_client.clazz, "sendKeys", "(Ljava/lang/String;)V");
    _client.destroy = jre->GetMethodID(_client.clazz, "destroy", "()V");
    _client.blocking = jre->GetFieldID(_client.clazz, "blocking", "Z");
    _client.active = jre->GetFieldID(_client.clazz, "active", "Z");
    _client.getrefresh = jre->GetMethodID(_client.clazz, "getRefresh", "()I");
    _client.setrefresh = jre->GetMethodID(_client.clazz, "setRefresh", "(I)V");
    _client.getfieldobject = jre->GetMethodID(_client.clazz, "getFieldObject", "(Ljava/lang/Object;Ljava/lang/String;)Ljava/lang/Object;");
    _client.getfieldint = jre->GetMethodID(_client.clazz, "getFieldInt", "(Ljava/lang/Object;Ljava/lang/String;)I");
    _client.getfieldfloat = jre->GetMethodID(_client.clazz, "getFieldFloat", "(Ljava/lang/Object;Ljava/lang/String;)F");
    _client.getfielddouble = jre->GetMethodID(_client.clazz, "getFieldDouble", "(Ljava/lang/Object;Ljava/lang/String;)D");
    _client.getfieldboolean = jre->GetMethodID(_client.clazz, "getFieldBoolean", "(Ljava/lang/Object;Ljava/lang/String;)Z");
    _client.getfieldlongh = jre->GetMethodID(_client.clazz, "getFieldLongH", "(Ljava/lang/Object;Ljava/lang/String;)I");
    _client.getfieldlongl = jre->GetMethodID(_client.clazz, "getFieldLongL", "(Ljava/lang/Object;Ljava/lang/String;)I");
    _client.getfieldarray3dint = jre->GetMethodID(_client.clazz, "getFieldArray3DInt", "(Ljava/lang/Object;Ljava/lang/String;III)I");
    _client.getfieldarray3dfloat = jre->GetMethodID(_client.clazz, "getFieldArray3DFloat", "(Ljava/lang/Object;Ljava/lang/String;III)F");
    _client.getfieldarray3ddouble = jre->GetMethodID(_client.clazz, "getFieldArray3DDouble", "(Ljava/lang/Object;Ljava/lang/String;III)D");
    _client.getfieldarray3dboolean = jre->GetMethodID(_client.clazz, "getFieldArray3DBoolean", "(Ljava/lang/Object;Ljava/lang/String;III)Z");
    _client.getfieldarray3dlongh = jre->GetMethodID(_client.clazz, "getFieldArray3DLongH", "(Ljava/lang/Object;Ljava/lang/String;III)I");
    _client.getfieldarray3dlongl = jre->GetMethodID(_client.clazz, "getFieldArray3DLongL", "(Ljava/lang/Object;Ljava/lang/String;III)I");
    _client.getfieldarray3dbyte = jre->GetMethodID(_client.clazz, "getFieldArray3DByte", "(Ljava/lang/Object;Ljava/lang/String;III)I");
    _client.getfieldarray3dshort = jre->GetMethodID(_client.clazz, "getFieldArray3DShort", "(Ljava/lang/Object;Ljava/lang/String;III)I");
    _client.getfieldarray3dchar = jre->GetMethodID(_client.clazz, "getFieldArray3DChar", "(Ljava/lang/Object;Ljava/lang/String;III)I");
    _client.getfieldarray3dobject = jre->GetMethodID(_client.clazz, "getFieldArray3DObject", "(Ljava/lang/Object;Ljava/lang/String;III)Ljava/lang/Object;");
    _client.getfieldarray2dint = jre->GetMethodID(_client.clazz, "getFieldArray2DInt", "(Ljava/lang/Object;Ljava/lang/String;II)I");
    _client.getfieldarray2dfloat = jre->GetMethodID(_client.clazz, "getFieldArray2DFloat", "(Ljava/lang/Object;Ljava/lang/String;II)F");
    _client.getfieldarray2ddouble = jre->GetMethodID(_client.clazz, "getFieldArray2DDouble", "(Ljava/lang/Object;Ljava/lang/String;II)D");
    _client.getfieldarray2dboolean = jre->GetMethodID(_client.clazz, "getFieldArray2DBoolean", "(Ljava/lang/Object;Ljava/lang/String;II)Z");
    _client.getfieldarray2dlongh = jre->GetMethodID(_client.clazz, "getFieldArray2DLongH", "(Ljava/lang/Object;Ljava/lang/String;II)I");
    _client.getfieldarray2dlongl = jre->GetMethodID(_client.clazz, "getFieldArray2DLongL", "(Ljava/lang/Object;Ljava/lang/String;II)I");
    _client.getfieldarray2dbyte = jre->GetMethodID(_client.clazz, "getFieldArray2DByte", "(Ljava/lang/Object;Ljava/lang/String;II)I");
    _client.getfieldarray2dshort = jre->GetMethodID(_client.clazz, "getFieldArray2DShort", "(Ljava/lang/Object;Ljava/lang/String;II)I");
    _client.getfieldarray2dchar = jre->GetMethodID(_client.clazz, "getFieldArray2DChar", "(Ljava/lang/Object;Ljava/lang/String;II)I");
    _client.getfieldarray2dobject = jre->GetMethodID(_client.clazz, "getFieldArray2DObject", "(Ljava/lang/Object;Ljava/lang/String;II)Ljava/lang/Object;");
    _client.getfieldarrayint = jre->GetMethodID(_client.clazz, "getFieldArrayInt", "(Ljava/lang/Object;Ljava/lang/String;I)I");
    _client.getfieldarrayfloat = jre->GetMethodID(_client.clazz, "getFieldArrayFloat", "(Ljava/lang/Object;Ljava/lang/String;I)F");
    _client.getfieldarraydouble = jre->GetMethodID(_client.clazz, "getFieldArrayDouble", "(Ljava/lang/Object;Ljava/lang/String;I)D");
    _client.getfieldarrayboolean = jre->GetMethodID(_client.clazz, "getFieldArrayBoolean", "(Ljava/lang/Object;Ljava/lang/String;I)Z");
    _client.getfieldarraylongh = jre->GetMethodID(_client.clazz, "getFieldArrayLongH", "(Ljava/lang/Object;Ljava/lang/String;I)I");
    _client.getfieldarraylongl = jre->GetMethodID(_client.clazz, "getFieldArrayLongL", "(Ljava/lang/Object;Ljava/lang/String;I)I");
    _client.getfieldarraybyte = jre->GetMethodID(_client.clazz, "getFieldArrayByte", "(Ljava/lang/Object;Ljava/lang/String;I)I");
    _client.getfieldarrayshort = jre->GetMethodID(_client.clazz, "getFieldArrayShort", "(Ljava/lang/Object;Ljava/lang/String;I)I");
    _client.getfieldarraychar = jre->GetMethodID(_client.clazz, "getFieldArrayChar", "(Ljava/lang/Object;Ljava/lang/String;I)I");
    _client.getfieldarrayobject = jre->GetMethodID(_client.clazz, "getFieldArrayObject", "(Ljava/lang/Object;Ljava/lang/String;I)Ljava/lang/Object;");
    _client.getfieldarraysize = jre->GetMethodID(_client.clazz, "getFieldArraySize", "(Ljava/lang/Object;Ljava/lang/String;)I");
    _client.setgraphics = jre->GetMethodID(_client.clazz, "setGraphics", "(Z)V");
    _client.setdebug = jre->GetMethodID(_client.clazz, "setDebug", "(Z)V");
    _client.transcolor = jre->GetFieldID(_client.clazz, "transColor", "I");
    _point.clazz = (jclass) jre->NewGlobalRef(jre->FindClass("java/awt/Point"));
    _point.x = jre->GetFieldID(_point.clazz, "x", "I");
    _point.y = jre->GetFieldID(_point.clazz, "y", "I");
    _string.clazz = (jclass) jre->NewGlobalRef(jre->FindClass("java/lang/String"));
    _string.byteInit = jre->GetMethodID(_string.clazz, "<init>", "([B)V");
    _string.charInit = jre->GetMethodID(_string.clazz, "<init>", "([C)V");

}

void freeClasses() {
    jre->DeleteGlobalRef(_client.clazz);
    jre->DeleteGlobalRef(_point.clazz);
    jre->DeleteGlobalRef(_string.clazz);
    unloadClasses();
}

void setup(char* root, char* params, long width, long height) {
    cout << "SmartSetup Entered\n";
    if (jre) vm->AttachCurrentThreadAsDaemon((void**)&jre, 0);
    if (strcmp(root, curserver) || width != client_width || height != client_height) {
        if (!jre) {
            if (!jvmdll) jvmdll = findJVM();
            if (!jvmdll) {
                cout << "Could not locate the Java Virtual Machine library\n";
                return;
            }
            initJVM(&jre, &vm, jvmdll);
            cout << "Created a Java Runtime Environment object\n";
            findClasses();
            cout << "Classes unpacked, loaded, and reflected\n";
        }
        clearOld();
        free(curserver);
        curserver = (char*) malloc(strlen(root) + 1);
        strcpy(curserver, root);
        client_height = height;
        client_width = width;
        #ifdef WINDOWS
        cout << "Allocating image HDC(s)\n";
        BITMAPINFO info;
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = client_width;
        info.bmiHeader.biHeight = -client_height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        info.bmiHeader.biSizeImage = 0;
        info.bmiHeader.biClrUsed = 0;
        imageHDC = CreateCompatibleDC(0);
        imageHBITMAP = CreateDIBSection(imageHDC, &info, DIB_RGB_COLORS, (void**)&image, 0, 0);
        SelectObject(imageHDC, imageHBITMAP);
        debugHDC = CreateCompatibleDC(0);
        debugHBITMAP = CreateDIBSection(debugHDC, &info, DIB_RGB_COLORS, (void**)&debug, 0, 0);
        SelectObject(debugHDC, debugHBITMAP);
        cout << "Image: " << (void*)image << " Debug: " << (void*)debug << '\n';
        #endif
        #ifdef LINUX
        cout << "Allocating image swap area(s)\n";
        image = (PRGB) malloc(client_width*client_height*4);
        debug = (PRGB) malloc(client_width*client_height*4);
        #endif
        setTarget(0,client_width,client_height);
        jobject imgBuffer = jre->NewDirectByteBuffer((void*)image, client_height*client_width*4);
        jobject debugBuffer = jre->NewDirectByteBuffer((void*)debug, client_height*client_width*4);
        jobject rootstr = jre->NewStringUTF(curserver);
        jobject paramsstr = jre->NewStringUTF(params);
        jobject temp = jre->NewObject(_client.clazz, _client.init, imgBuffer, debugBuffer, client_width, client_height, rootstr, paramsstr);
        smart = jre->NewGlobalRef(temp);
        jre->DeleteLocalRef(temp);
        jre->DeleteLocalRef(imgBuffer);
        jre->DeleteLocalRef(debugBuffer);
        jre->DeleteLocalRef(rootstr);
        jre->DeleteLocalRef(paramsstr);
    } else {
        if (jre && !jre->GetBooleanField(smart, _client.active)) {
            jre->CallVoidMethod(smart, _client.destroy);
            jre->DeleteGlobalRef(smart);
            jobject imgBuffer = jre->NewDirectByteBuffer((void*)((char*)image), client_height*client_width*4);
            jobject debugBuffer = jre->NewDirectByteBuffer((void*)((char*)debug), client_height*client_width*4);
            jobject rootstr = jre->NewStringUTF(curserver);
            jobject paramsstr = jre->NewStringUTF(params);
            jobject temp = jre->NewObject(_client.clazz, _client.init, imgBuffer, debugBuffer, client_width, client_height, rootstr, paramsstr);
            smart = jre->NewGlobalRef(temp);
            jre->DeleteLocalRef(temp);
            jre->DeleteLocalRef(imgBuffer);
            jre->DeleteLocalRef(debugBuffer);
            jre->DeleteLocalRef(rootstr);
            jre->DeleteLocalRef(paramsstr);
            return;
        }
    }
}

void internalConstructor() {
    curserver = (char*) malloc(1);
    *curserver = 0;
}

void internalDestructor() {
    if (jre) vm->AttachCurrentThreadAsDaemon((void**)&jre, 0);
    clearOld();
    free(curserver);
    if (jre) {
        freeClasses();
        //Would be nice... But this causes a MASSIVE hang.
        //vm->DestroyJavaVM();
    }
    cout << "Releasing SMART library...\n";
    #ifdef WINDOWS
    if (jvmdll) FreeLibrary((HMODULE)jvmdll);
    #endif
    #ifdef LINUX
    if (jvmdll) dlclose(jvmdll);
    #endif
}

