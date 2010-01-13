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
 
#include "JVM.h"

#ifdef WINDOWS

#include <windows.h>

bool initJVM(JNIEnv** env, JavaVM** vm, void* jvmdll) {
    GetJVMs getJVMs = (GetJVMs) GetProcAddress((HMODULE)jvmdll, "JNI_GetCreatedJavaVMs");
    jsize number = 0;
    getJVMs(vm, 1, &number);
    if (number > 0) {
        (*vm)->GetEnv((void**)env, JNI_VERSION_1_6);
        return true;
    } else {
        CreateJVM create = (CreateJVM) GetProcAddress((HMODULE)jvmdll, "JNI_CreateJavaVM");
        JavaVMInitArgs vm_args;
        JavaVMOption options[2];
        options[0].optionString = (char*)"-Dsun.java2d.noddraw";
        options[1].optionString = (char*)"-Xcheck:jni";
        vm_args.options = options;
        vm_args.nOptions = 2;
        vm_args.version = JNI_VERSION_1_6;
        vm_args.ignoreUnrecognized = true;
        if (JNI_OK == create(vm, (void**)env, &vm_args))
            return true;
    }
    return false;
}

void checkPath(char* javaBin) {
    HMODULE java = LoadLibrary("java.exe");
    if (!java) {
        char* path = getenv("PATH");
        if (!strstr(path, javaBin)) {
            char* newPath = (char*) malloc(strlen(path)+strlen(javaBin)+2);
            sprintf(newPath, "PATH=%s;%s", path, javaBin);
            putenv(newPath);
        }
    }
    FreeLibrary(java);
}

void* findJVM() {
    HKEY jrekey;
    HKEY curjrekey;
    HMODULE jvmdll;
    char value[512], path[512];
    unsigned long size;
    RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\JavaSoft\\Java Runtime Environment", 0, KEY_QUERY_VALUE, &jrekey);
    size = 512;
    memset(value, 0, 512);
    RegQueryValueEx(jrekey, "CurrentVersion", 0, 0, (unsigned char*)value, &size);
    RegOpenKeyEx(jrekey, value, 0, KEY_QUERY_VALUE, &curjrekey);
    size = 512;
    RegQueryValueEx(curjrekey, "JavaHome", 0, 0, (unsigned char*)value, &size);
    memset(path, 0, 512);
    sprintf(path, "%s\\bin", value);
    //checkPath(path);
    memset(path, 0, 512);
    sprintf(path, "%s\\bin\\server\\jvm.dll", value);
    if (!(jvmdll = LoadLibrary(path))) {
        memset(path, 0, 512);
        sprintf(path, "%s\\bin\\client\\jvm.dll", value);
        if (!(jvmdll = LoadLibrary(path))) {
            return 0;
        }
    }
    return (void*) jvmdll;
}

#endif

#ifdef LINUX

#include <dlfcn.h>

bool initJVM(JNIEnv** env, JavaVM** vm, void* jvmdll) {
    GetJVMs getJVMs = (GetJVMs) dlsym(jvmdll, "JNI_GetCreatedJavaVMs");
    jsize number = 0;
    getJVMs(vm, 1, &number);
    if (number > 0) {
        (*vm)->GetEnv((void**)env, JNI_VERSION_1_6);
        return true;
    } else {
        CreateJVM create = (CreateJVM) dlsym(jvmdll, "JNI_CreateJavaVM");
        JavaVMInitArgs vm_args;
        JavaVMOption options[2];
        options[0].optionString = (char*)"-Dsun.java2d.noddraw";
        options[1].optionString = (char*)"-Xcheck:jni";
        vm_args.options = options;
        vm_args.nOptions = 2;
        vm_args.version = JNI_VERSION_1_6;
        vm_args.ignoreUnrecognized = true;
        if (JNI_OK == create(vm, (void**)env, &vm_args))
            return true;
    }
    return false;
}

void* findJVM() {
    void* libjvm = dlopen("/usr/lib/jvm/java-6-sun/jre/lib/i386/server/libjvm.so", RTLD_LAZY);
    if (!libjvm) libjvm = dlopen("/usr/lib/jvm/java-6-sun/jre/lib/i386/client/libjvm.so", RTLD_LAZY);
    return libjvm;
}

#endif

