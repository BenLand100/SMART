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
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0501
#endif
#include <windows.h>
#include <shobjidl.h>
#endif

extern "C" JNIEXPORT jboolean JNICALL Java_smart_Main_checkAlive(JNIEnv *env, jclass cls, jint tid)
{
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

extern "C" JNIEXPORT jint JNICALL Java_smart_Main_getPID(JNIEnv *env, jclass cls)
{
    #ifndef _WIN32
        return getpid();
    #else
        return GetCurrentProcessId();
    #endif
}

extern "C" JNIEXPORT jlong JNICALL Java_JNI_Natives_createFileMapping(JNIEnv *env, jclass, jlong hFile, jint lpProtect, jint dwMaximumSizeHigh, jint dwMaximumSizeLow, jstring lpName)
{
    const char* Name = env->GetStringUTFChars(lpName, nullptr);

    #if defined _WIN32 || defined _WIN64
    HANDLE hFileMap = nullptr;
    if ((hFileMap = CreateFileMapping(hFile == -1 ? INVALID_HANDLE_VALUE : reinterpret_cast<HANDLE>(hFile), nullptr, lpProtect, dwMaximumSizeHigh, dwMaximumSizeLow, Name)) != nullptr)
    {
        if (errno ==  ERROR_ALREADY_EXISTS)
        {
            CloseHandle(hFileMap);
            hFileMap = nullptr;
        }
    }
    #else
    int hFileMap = nullptr;
    if ((hFileMap = open(Name, O_RDWR | O_CREAT, 438)) == -1)
    {
        close(hFileMap);
        hFileMap = 0;
    }
    #endif

    env->ReleaseStringUTFChars(lpName, Name);
    return reinterpret_cast<jlong>(hFileMap);
}

extern "C" JNIEXPORT jlong JNICALL Java_JNI_Natives_openFileMapping(JNIEnv *env, jclass, jint dwDesiredAccess, jboolean bInheritHandle, jstring lpName)
{
    const char* Name = env->GetStringUTFChars(lpName, nullptr);
    #if defined _WIN32 || defined _WIN64
    HANDLE hFileMap = OpenFileMapping(dwDesiredAccess, static_cast<bool>(bInheritHandle), Name);
    #else
    int hFileMap = open(MapName.c_str(), O_RDWR | O_CREAT, 438);
    if (hFileMap == -1)
    {
        hFileMap = 0;
    }
    #endif
    env->ReleaseStringUTFChars(lpName, Name);
    return reinterpret_cast<jlong>(hFileMap);
}

extern "C" JNIEXPORT jlong JNICALL Java_JNI_Natives_mapViewOfFile(JNIEnv *, jclass, jlong hFileMap, jint dwDesiredAccess, jint dwFileOffsetHigh, jint dwFileOffsetLow, jint dwNumberOfBytesToMap)
{
    #if defined _WIN32 || defined _WIN64
    void* pData = MapViewOfFile(reinterpret_cast<HANDLE>(hFileMap), dwDesiredAccess, dwFileOffsetHigh, dwFileOffsetLow, dwNumberOfBytesToMap);
    #else
    void* pData = nullptr;
    if ((pData = mmap(nullptr, Size, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, hFileMap, 0)) == MAP_FAILED)
    {
        pData = nullptr;
    }
    #endif
    return reinterpret_cast<jlong>(pData);
}

extern "C" JNIEXPORT jlong JNICALL Java_JNI_Natives_unmapViewOfFile(JNIEnv *, jclass, jlong hFileMap, jlong Size)
{
    #if defined _WIN32 || defined _WIN64
    return UnmapViewOfFile(reinterpret_cast<void*>(hFileMap));
    #else
    return !munmap(reinterpret_cast<void*>(hFileMap), Size) ? 1 : 0;
    #endif
}

extern "C" JNIEXPORT jlong JNICALL Java_JNI_Natives_closeHandle(JNIEnv *, jclass, jlong hFile)
{
    #if defined _WIN32 || defined _WIN64
    return CloseHandle(reinterpret_cast<HANDLE>(hFile));
    #else
    return !close(hFile) ? 1 : 0;
    #endif
}



#if defined _WIN32 || defined _WIN64
extern "C"
{
    const GUID CLSID_TaskbarList = {0x56FDF344, 0xFD6D, 0x11D0, {0x95, 0x8A, 0x00, 0x60, 0x97, 0xC9, 0xA0, 0x90}};

    const GUID IID_ITaskbarList = {0x56FDF342, 0xFD6D, 0x11D0, {0x95, 0x8A, 0x00, 0x60, 0x97, 0xC9, 0xA0, 0x90}};

    const GUID IID_ITaskbarList2 = {0x602D4995, 0xB13A, 0x429b, {0xA6, 0x6E, 0x19, 0x35, 0xE4, 0x4F, 0x43, 0x17}};

    const GUID IID_ITaskList3 = {0xEA1AFB91, 0x9E28, 0x4B86, {0x90, 0xE9, 0x9E, 0x9F, 0x8A, 0x5E, 0xEF, 0xAF}};
}
#endif

extern "C" JNIEXPORT void JNICALL Java_JNI_Natives_addToTaskBar(JNIEnv *, jclass, jlong WindowHandle)
{
    #if defined _WIN32 || defined _WIN64
    ITaskbarList* TaskListPtr;
    CoInitialize(nullptr);
    long Result = !CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_SERVER, IID_ITaskbarList, reinterpret_cast<void**>(&TaskListPtr));
    if (Result) TaskListPtr->AddTab(reinterpret_cast<HWND>(WindowHandle));
    TaskListPtr->Release();
    CoUninitialize();
    #endif
}

extern "C" JNIEXPORT void JNICALL Java_JNI_Natives_removeFromTaskBar(JNIEnv *, jclass, jlong WindowHandle)
{
    #if defined _WIN32 || defined _WIN64
    ITaskbarList* TaskListPtr;
    CoInitialize(nullptr);
    long Result = !CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_SERVER, IID_ITaskbarList, reinterpret_cast<void**>(&TaskListPtr));
    if (Result) TaskListPtr->DeleteTab(reinterpret_cast<HWND>(WindowHandle));
    TaskListPtr->Release();
    CoUninitialize();
    #endif
}