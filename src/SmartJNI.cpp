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

extern "C" JNIEXPORT jint JNICALL Java_smart_Main_getPID(JNIEnv *env, jclass cls, jint dummy) {
    #ifndef _WIN32
        return getpid();
    #else
        return GetCurrentProcessId();
    #endif
}

//This MIGHT be needed if Java's SHM mapping doesn't work on windows
/*
#ifndef _WIN32
    int fd;
    void *memmap;
#else
    HANDLE file;
    HANDLE memmap;
#endif
int n_size;
void *data;

extern "C" JNIEXPORT jobject JNICALL Java_smart_Main_mapShared(JNIEnv *env, jclass cls, jstring path, jint size) {
    n_size = size; 
    const char* shmfile = env->GetStringUTFChars(path, 0);
    #ifndef _WIN32
        fd = open(shmfile,O_RDWR);
        if (fd != -1) {
            memmap = mmap(NULL,size,PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
            data = (void*)memmap;
        }
    #else
        file = CreateFile(
            shmfile,
            GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
            NULL,
            OPEN_ALWAYS,
            NULL);
        if (file != INVALID_HANDLE_VALUE) {
            memmap = CreateFileMapping(client->file,NULL,PAGE_READWRITE,0,size,shmfile);
            data = (void*)MapViewOfFile(client->memmap,FILE_MAP_ALL_ACCESS,0,0,size);
        }
    #endif
    env->ReleaseStringUTFChars(path, shmfile);
    jobject buffer = env->NewDirectByteBuffer(data, size);
    return buffer;
}

extern "C" JNIEXPORT jint JNICALL Java_smart_Main_unmap(JNIEnv *env, jclass cls) {
    #ifndef _WIN32
        munmap(memmap,n_size);
        close(fd);
    #else
        UnmapViewOfFile(data);
        CloseHandle(memmap);
        CloseHandle(file);
    #endif
}
*/
