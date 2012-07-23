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
    //#include <GL/gl.h>
    #include <sys/syscall.h>
    #include <sys/mman.h>
    #include <fcntl.h>
    #include <dirent.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/types.h>
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

extern "C" JNIEXPORT void JNICALL Java_smart_Main_copyGLBuffer(JNIEnv *env, jclass cls, jint x, jint y, jint w, jint h, jobject dest) {
    void *data = env->GetDirectBufferAddress(dest);
    #ifndef _WIN32
        //glReadPixels(x,y,w,h,GL_BGR,GL_UNSIGNED_INT_8_8_8_8,data);
    #else
    #endif
}

