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
 
//This file loads and initializes the JVM
 
#include "JVM.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

//initilized and free'd in Smart.cpp as well
char *jvmpath = 0;
long maxmem = 256;

//Sets the location of the library that SMART should load for the JVM
void setJVMPath(char* path) {
	if (path && strlen(path) > 0) {
		if (jvmpath) free(jvmpath);
		jvmpath = (char*)malloc(strlen(path)+1);
		strcpy(jvmpath,path);
		cout << "JVMPath set to " << jvmpath << '\n';
	} else {
		if (jvmpath) free(jvmpath);
		jvmpath = 0;
		cout << "JVMPath set to platform default\n";
	}
}

void setMaxJVMMem(long _maxmem) {
    maxmem = _maxmem;
}

#ifdef WINDOWS

#include <windows.h>

//Loads the JVM from a given library handle
bool initJVM(JNIEnv** env, JavaVM** vm, void* jvmdll) {
    GetJVMs getJVMs = (GetJVMs) GetProcAddress((HMODULE)jvmdll, "JNI_GetCreatedJavaVMs");
    jsize number = 0;
    getJVMs(vm, 1, &number);
    if (number > 0) {
        (*vm)->GetEnv((void**)env, JNI_VERSION_1_6);
        return true;
    } else {
        char *maxarg = (char*)malloc(snprintf(0,0,"-Xmx%im",(int)maxmem)+1);
        sprintf(maxarg,"-Xmx%im",(int)maxmem);
        CreateJVM create = (CreateJVM) GetProcAddress((HMODULE)jvmdll, "JNI_CreateJavaVM");
        JavaVMInitArgs vm_args;
        JavaVMOption options[3];
        options[0].optionString = (char*)"-Dsun.java2d.noddraw";
        options[1].optionString = maxarg;
        vm_args.options = options;
        vm_args.nOptions = 2;
        vm_args.version = JNI_VERSION_1_6;
        vm_args.ignoreUnrecognized = true;
        if (JNI_OK == create(vm, (void**)env, &vm_args)) {
            free(maxarg);
            return true;
        }
        free(maxarg);
    }
    return false;
}

//Attempts to put Java on the user's path --- historical
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

//Returns the set JVM path's handle, or locates the JVM in the windows registry
void* findJVM() {
    HMODULE jvmdll;
	if (jvmpath) {
    	cout << "Attempting to load custom JVM\n";
    	jvmdll = LoadLibrary(jvmpath);
		if (jvmdll) return jvmdll;
	}
    HKEY jrekey;
    HKEY curjrekey;
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
    cout << "Attempting to load default server JVM\n";
    if (!(jvmdll = LoadLibrary(path))) {
        memset(path, 0, 512);
        sprintf(path, "%s\\bin\\client\\jvm.dll", value);
    	cout << "Attempting to load default client JVM\n";
        if (!(jvmdll = LoadLibrary(path))) {
            return 0;
        }
    }
    return (void*) jvmdll;
}

#endif

#ifdef LINUX

#include <dlfcn.h>

//Loads the JVM from a given library handle
bool initJVM(JNIEnv** env, JavaVM** vm, void* jvmdll) {
    GetJVMs getJVMs = (GetJVMs) dlsym(jvmdll, "JNI_GetCreatedJavaVMs");
    jsize number = 0;
    getJVMs(vm, 1, &number);
    if (number > 0) {
        (*vm)->GetEnv((void**)env, JNI_VERSION_1_6);
        return true;
    } else {
        char *maxarg = (char*)malloc(snprintf(0,0,"-Xmx%im",(int)maxmem)+1);
        sprintf(maxarg,"-Xmx%im",(int)maxmem);
        CreateJVM create = (CreateJVM) dlsym(jvmdll, "JNI_CreateJavaVM");
        JavaVMInitArgs vm_args;
        JavaVMOption options[3];
        options[0].optionString = (char*)"-Dsun.java2d.noddraw";
        options[1].optionString = maxarg;
        vm_args.options = options;
        vm_args.nOptions = 2;
        vm_args.version = JNI_VERSION_1_6;
        vm_args.ignoreUnrecognized = true;
        if (JNI_OK == create(vm, (void**)env, &vm_args)) {
            free(maxarg);
            return true;
        }
        free(maxarg);
    }
    return false;
}

//Returns the set JVM path's handle, or tries the default Ubuntu locaton for the jvm
void* findJVM() {
	void* libjvm;
    if (jvmpath) {
    	cout << "Attempting to load custom JVM\n";
    	libjvm = dlopen(jvmpath, RTLD_LAZY);
		if (libjvm) return libjvm;
	}
    libjvm = dlopen("/usr/lib/jvm/java-6-sun/jre/lib/i386/server/libjvm.so", RTLD_LAZY);
    cout << "Attempting to load default server JVM\n";
    if (!libjvm) {
    	libjvm = dlopen("/usr/lib/jvm/java-6-sun/jre/lib/i386/client/libjvm.so", RTLD_LAZY);
    	cout << "Attempting to load default client JVM\n";
    }
    return libjvm;
}

#endif

