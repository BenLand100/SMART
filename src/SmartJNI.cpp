/**
 *  Copyright 2012 by Benjamin J. Land (a.k.a. BenLand100)
 *
 *  This file is part of the SMART
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
 
#include <iostream>
#include "jni.h"

#ifndef _WIN32
    #include <sys/syscall.h>
    #include <sys/mman.h>
    #include <fcntl.h>
    #include <dirent.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/types.h>
    #include <dlfcn.h>
#else
    #if __SIZEOF_POINTER__ == 4
       #define _WIN32_WINNT 0x0501
    #endif
    #include <windows.h>
#endif

extern "C" JNIEXPORT jboolean JNICALL Java_smart_Main_checkAlive(JNIEnv *env, jclass cls, jint tid) {    
    #ifndef _WIN32
        return syscall(SYS_tkill,tid,0) ? JNI_FALSE : JNI_TRUE;
    #else
        HANDLE thread = OpenThread(SYNCHRONIZE,FALSE,tid);
        if (!thread) return JNI_FALSE;
        jboolean res = WaitForSingleObject(thread, 0) == WAIT_TIMEOUT ? JNI_TRUE : JNI_FALSE;
        CloseHandle(thread);
        return res;
    #endif
}

extern "C" JNIEXPORT jint JNICALL Java_smart_Main_getPID(JNIEnv *env, jclass cls) {
    #ifndef _WIN32
        return getpid();
    #else
        return GetCurrentProcessId();
    #endif
}

extern "C" JNIEXPORT jobject JNICALL Java_smart_Main_getGlobalRef(JNIEnv *env, jclass cls, jobject org) {
    return *((jobject*)env->GetDirectBufferAddress(org));
}

extern "C" JNIEXPORT void JNICALL Java_smart_Main_putGlobalRef(JNIEnv *env, jclass cls, jobject obj, jobject org) {
    *((jobject*)env->GetDirectBufferAddress(org)) = env->NewGlobalRef(obj);
}

extern "C" JNIEXPORT void JNICALL Java_smart_Main_freeGlobalRef(JNIEnv *env, jclass cls, jobject org) {
    env->DeleteGlobalRef(*((jobject*)env->GetDirectBufferAddress(org)));
}

extern "C" JNIEXPORT jstring JNICALL Java_smart_Main_pathFromAddress(JNIEnv *env, jclass cls, jobject org) {
    char *path = ((char*)env->GetDirectBufferAddress(org));
    path += sizeof(jobject); //skip the object reference
    return env->NewStringUTF(path);
}

extern "C" JNIEXPORT jint JNICALL Java_smart_Main_indexFromAddress(JNIEnv *env, jclass cls, jobject org, int idx) {
    char *data = ((char*)env->GetDirectBufferAddress(org));
    data += sizeof(jobject); //skip the object reference
    data += strlen(data)+1; //skip the path
    return ((int*)data)[idx];
}

#include "SMARTPlugin.h"

SMARTInfo info;
void **plugins;
int plugin_c,plugin_i;
JavaVM *vm;
jobject client;
jmethodID _client_getMousePos;
jmethodID _client_setCapture;
jmethodID _client_defineNativeButton;
jmethodID _client_replaceCaptureButtons;
jfieldID _point_x;
jfieldID _point_y;
#ifdef _WIN32
HMODULE dllinst;

extern "C" bool DllMain(HINSTANCE, int, void*) __attribute__((stdcall));
bool DllMain(HINSTANCE instance, int reason, void* checks) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            dllinst = instance;
            return true;
        case DLL_THREAD_ATTACH:
            return true;
        case DLL_PROCESS_DETACH:
            return true;
        case DLL_THREAD_DETACH:
            return true;
    }
    return false;
}
#endif

#define AttachVM(env) JNIEnv *env; vm->AttachCurrentThreadAsDaemon((void**)&env,0);

extern "C" void getMousePos(int &x, int &y) {
    AttachVM(env);
    jobject pt = env->CallObjectMethod(client, _client_getMousePos);
    x = env->GetIntField(pt, _point_x);
    y = env->GetIntField(pt, _point_y);
    env->DeleteLocalRef(pt);
}

extern "C" void setCapture(bool enabled) {
    AttachVM(env);
    env->CallVoidMethod(client, _client_setCapture, enabled);
}

extern "C" JNIEXPORT void JNICALL Java_smart_Main_setupPlugins(JNIEnv *env, jclass cls, jint num) {
    plugins = new void*[num];
    for (int i = 0; i < num; i++) plugins[i] = 0;
    plugin_c = num;
    plugin_i = 0;
}
    
extern "C" JNIEXPORT jboolean JNICALL Java_smart_Main_loadPlugin(JNIEnv *env, jclass cls, jstring jpath) {
    const char *path = env->GetStringUTFChars(jpath, 0);
    #ifdef _WIN32
    plugins[plugin_i] = LoadLibrary(path);
    #else
    plugins[plugin_i] = dlopen(path, RTLD_LAZY);
    #endif
    env->ReleaseStringUTFChars(jpath, path);
    return plugins[plugin_i++] != NULL;
}
    
#ifdef _WIN32
#define LDPROCADDR(name) GetProcAddress(dllinst,name)
#else
#define LDPROCADDR(name) dlsym(RTLD_DEFAULT,name)
#endif
extern "C" JNIEXPORT void JNICALL Java_smart_Main_setNatives(JNIEnv *env, jclass cls, jobject _client, jobject img, jobject dbg, jint width, jint height) {
    info.version = 0x08010000;
    info.img = env->GetDirectBufferAddress(img);
    info.dbg = env->GetDirectBufferAddress(dbg);
    info.width = width;
    info.height = height;
    info.getMousePos = (_SMARTGetMousePos) LDPROCADDR("getMousePos");
    info.setCapture = (_SMARTSetCapture) LDPROCADDR("setCapture");
    
    env->GetJavaVM(&vm);
    client = env->NewGlobalRef(_client);
    jclass client_class = env->FindClass("smart/Client");
    _client_getMousePos = env->GetMethodID(client_class, "getMousePos", "()Ljava/awt/Point;");
    _client_setCapture = env->GetMethodID(client_class, "setCapture", "(Z)V");
    _client_defineNativeButton = env->GetMethodID(client_class, "defineNativeButton", "(Ljava/lang/String;ILjava/nio/ByteBuffer;)V");
    _client_replaceCaptureButtons = env->GetMethodID(client_class, "replaceCaptureButtons", "()V");
    jclass point_class = env->FindClass("java/awt/Point");
    _point_x = env->GetFieldID(point_class, "x", "I");
    _point_y = env->GetFieldID(point_class, "y", "I");
}
    
extern "C" JNIEXPORT void JNICALL Java_smart_Main_nativeButton(JNIEnv *env, jclass cls, jobject callback, jint id, jboolean state) {
    _SMARTButtonPressed buttonproc = *((_SMARTButtonPressed*)env->GetDirectBufferAddress(callback));
    buttonproc(id,state);
}
    
extern "C" JNIEXPORT jboolean JNICALL Java_smart_Main_initPlugin(JNIEnv *env, jclass cls, jint i) {
    if (plugins[i]) {
        #ifdef _WIN32
        _SMARTPluginInit init = (_SMARTPluginInit) GetProcAddress((HMODULE)plugins[i],"SMARTPluginInit");
        #else
        _SMARTPluginInit init = (_SMARTPluginInit) dlsym(plugins[i],"SMARTPluginInit");
        #endif 
        if (init) {
            bool replace = false;
            int buttonc = 0;
            char **buttonv = NULL;
            int *buttonid = NULL;
            _SMARTButtonPressed *proc = new _SMARTButtonPressed;
            *proc = NULL;
            init(&info,&replace,&buttonc,&buttonv,&buttonid,proc);
            if (replace) {
                env->CallVoidMethod(client,_client_replaceCaptureButtons);
            }
            jobject callback = env->NewDirectByteBuffer(proc,sizeof(_SMARTButtonPressed));
            for (int i = 0; i < buttonc; i++) {
                std::cout << buttonv[i] << '\n';
                jstring cap = env->NewStringUTF(buttonv[i]);
                env->CallVoidMethod(client,_client_defineNativeButton,cap,buttonid[i],callback);
            }
            return true;
        }
    }
    return false;
}

