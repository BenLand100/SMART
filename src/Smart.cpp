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

//Core setup routines and most of the static data here

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
//Windows HDC magic
HDC imageHDC = 0;
HBITMAP imageHBITMAP = 0;
HDC debugHDC = 0;
HBITMAP debugHBITMAP = 0;
#endif

//References to the JVM
JNIEnv* jre = 0;
JavaVM* vm = 0;
void* jvmdll = 0;

//References to the SMART java instance
jobject smart = 0;
JCLIENT _client;
JPOINT _point;
JSTRING _string;

//Info related to the current SMART instance
char* curserver = 0;
char* curparams = 0;
PRGB image = 0;
PRGB debug = 0;
int client_width = 0;
int client_height = 0;

#ifdef WINDOWS

//Returns the HDC of the surface where the Client's image is rendered
long getImageHDC() {
    return (long) imageHDC;
}

//Returns the HDC of the debug space that can be rendered over the client
long getDebugHDC() {
    return (long) debugHDC;
}

#endif

//Returns the array where the client's image is rendered
void* getImageArray() {
    return (void*) image;
}

//Returns the array of the debug space that can be rendered over the client
void* getDebugArray() {
    return (void*) debug;
}

//Sets the color on the debug space that is considered transparent, default 0
void setTransparentColor(jint color) {
    if (jre) jre->SetIntField(smart, _client.transcolor, color);
}

//Turns debug rendering on or off
void setDebug(bool on) {
    if (jre) jre->CallVoidMethod(smart, _client.setdebug, on ? 1 : 0);
}

//Turns graphics rendering on or off, current data is in the ImageArray regardless
void setGraphics(bool on) {
    if (jre) jre->CallVoidMethod(smart, _client.setgraphics, on ? 1 : 0);
}

//Returns true if the java instance is active and ready for commands
bool isActive() {
	//This is kind of dirty, since now a thread that didn't call `setup` now owns
	//the JVM, however I'm assuming that shortly thereafter people would either 
	//A) Terminate the thread, or B) Call `setup`; so it won't likely cause issues
	if (jre) vm->AttachCurrentThreadAsDaemon((void**)&jre, 0);
    return jre ? jre->GetBooleanField(smart, _client.active) : false;
}

//Returns true if the client is currently blocking events and accepting fake events
bool isBlocking() {
    return jre ? jre->GetBooleanField(smart, _client.blocking) : false;
}

//Returns the 'refresh rate' of the client, 0-100, 100 being fullspeed, 0 being very slow
long getRefresh() {
    return jre ? jre->CallIntMethod(smart, _client.getrefresh) : -1;
}

//Sets the 'refresh rate' of the client, 0-100, 100 being fullspeed, 0 being very slow
void setRefresh(long x) {
    if (jre) jre->CallVoidMethod(smart, _client.setrefresh, x);
}

//Erases the previous SMART state, but leaves the JVM intact
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

//Caches all needed refrences to java classes and methods
void findClasses() {
    loadClasses();
    _client.clazz = (jclass) jre->NewGlobalRef(jre->FindClass("smart/Client"));
    _client.init = jre->GetMethodID(_client.clazz, "<init>", "(Ljava/nio/ByteBuffer;Ljava/nio/ByteBuffer;IILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    _client.clickmouse = jre->GetMethodID(_client.clazz, "clickMouse", "(III)V");
    _client.getmousepos = jre->GetMethodID(_client.clazz, "getMousePos", "()Ljava/awt/Point;");
    _client.holdkey = jre->GetMethodID(_client.clazz, "holdKey", "(I)V");
    _client.iskeydown = jre->GetMethodID(_client.clazz, "isKeyDown", "(I)Z");
    _client.holdmouse = jre->GetMethodID(_client.clazz, "holdMouse", "(III)V");
    _client.movemouse = jre->GetMethodID(_client.clazz, "moveMouse", "(II)V");
    _client.windmouse = jre->GetMethodID(_client.clazz, "windMouse", "(II)V");
    _client.releasekey = jre->GetMethodID(_client.clazz, "releaseKey", "(I)V");
    _client.releasemouse = jre->GetMethodID(_client.clazz, "releaseMouse", "(III)V");
    _client.sendkeys = jre->GetMethodID(_client.clazz, "sendKeys", "(Ljava/lang/String;)V");
    _client.destroy = jre->GetMethodID(_client.clazz, "destroy", "()V");
    _client.blocking = jre->GetFieldID(_client.clazz, "blocking", "Z");
    _client.active = jre->GetFieldID(_client.clazz, "active", "Z");
    _client.getrefresh = jre->GetMethodID(_client.clazz, "getRefresh", "()I");
    _client.setrefresh = jre->GetMethodID(_client.clazz, "setRefresh", "(I)V");
    _client.getfieldobject = jre->GetMethodID(_client.clazz, "getFieldObject", "(Ljava/lang/Object;Ljava/lang/String;)Ljava/lang/Object;");
    _client.ispathvalid = jre->GetMethodID(_client.clazz, "isPathValid", "(Ljava/lang/Object;Ljava/lang/String;)Z");
    _client.getfieldint = jre->GetMethodID(_client.clazz, "getFieldInt", "(Ljava/lang/Object;Ljava/lang/String;)I");
    _client.getfieldshort = jre->GetMethodID(_client.clazz, "getFieldShort", "(Ljava/lang/Object;Ljava/lang/String;)I");
    _client.getfieldbyte = jre->GetMethodID(_client.clazz, "getFieldByte", "(Ljava/lang/Object;Ljava/lang/String;)I");
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
    _client.getfieldarraysize = jre->GetMethodID(_client.clazz, "getFieldArraySize", "(Ljava/lang/Object;Ljava/lang/String;I)I");
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

//Frees all cached class refrences
void freeClasses() {
    jre->DeleteGlobalRef(_client.clazz);
    jre->DeleteGlobalRef(_point.clazz);
    jre->DeleteGlobalRef(_string.clazz);
    unloadClasses();
}

//Entrypoint for SMART operation --- sets a new state or reuses an old one if compatible
//loads the JVM if necessary, and initilizes anything that might need it
void setup(char* root, char* params, long width, long height, char* initseq) {
    cout << "SmartSetup Entered\n";
    if (jre) vm->AttachCurrentThreadAsDaemon((void**)&jre, 0);
    if (strcmp(root, curserver) || strcmp(params, curparams) || width != client_width || height != client_height) {
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
        free(curparams);
        curparams = (char*) malloc(strlen(params) + 1);
        strcpy(curparams, params);
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
        jobject initseqstr = jre->NewStringUTF(initseq);
        jobject temp = jre->NewObject(_client.clazz, _client.init, imgBuffer, debugBuffer, client_width, client_height, rootstr, paramsstr,initseqstr);
        smart = jre->NewGlobalRef(temp);
        jre->DeleteLocalRef(temp);
        jre->DeleteLocalRef(imgBuffer);
        jre->DeleteLocalRef(debugBuffer);
        jre->DeleteLocalRef(rootstr);
        jre->DeleteLocalRef(paramsstr);
        jre->DeleteLocalRef(initseqstr);
    } else {
        if (jre && (jre->GetBooleanField(smart, _client.active) == JNI_FALSE)) {
            jre->CallVoidMethod(smart, _client.destroy);
            jre->DeleteGlobalRef(smart);
            jobject imgBuffer = jre->NewDirectByteBuffer((void*)((char*)image), client_height*client_width*4);
            jobject debugBuffer = jre->NewDirectByteBuffer((void*)((char*)debug), client_height*client_width*4);
            jobject rootstr = jre->NewStringUTF(curserver);
            jobject paramsstr = jre->NewStringUTF(params);
            jobject initseqstr = jre->NewStringUTF(initseq);
            jobject temp = jre->NewObject(_client.clazz, _client.init, imgBuffer, debugBuffer, client_width, client_height, rootstr, paramsstr,initseqstr);
            smart = jre->NewGlobalRef(temp);
            jre->DeleteLocalRef(temp);
            jre->DeleteLocalRef(imgBuffer);
            jre->DeleteLocalRef(debugBuffer);
            jre->DeleteLocalRef(rootstr);
            jre->DeleteLocalRef(paramsstr);
            jre->DeleteLocalRef(initseqstr);
            return;
        }
    }
}

//Will completely kill the JVM and as a consequence, the RS client.
//Note, resets the state to as if the JVM hasn't been loaded yet.
void hardReset() {
    vm->AttachCurrentThreadAsDaemon((void**)&jre, 0);
    clearOld();
    free(curserver);
    free(curparams);
    curserver = (char*) malloc(1);
    *curserver = 0;
    curparams = (char*) malloc(1);
    *curparams = 0;
    jre = 0;
    vm = 0;
}

//Called at Library loading to ensure a 'neat' state since initilizers might not always get run
void internalConstructor() {
    curserver = (char*) malloc(1);
    *curserver = 0;
    curparams = (char*) malloc(1);
    *curparams = 0;
    jvmpath = 0;
    maxmem = 256;
}

//Called at Library unloading to ensure all memory is nicely released (or so we hope)
void internalDestructor() {
	if (jvmpath) free(jvmpath);
    if (jre) vm->AttachCurrentThreadAsDaemon((void**)&jre, 0);
    clearOld();
    free(curserver);
    free(curparams);
    if (jre) {
        freeClasses();
    }
    cout << "Releasing SMART library...\n";
    #ifdef WINDOWS
    if (jvmdll) FreeLibrary((HMODULE)jvmdll);
    #endif
    #ifdef LINUX
    if (jvmdll) dlclose(jvmdll);
    #endif
}

