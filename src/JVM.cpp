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
#include <sys/stat.h>
#include <libgen.h>

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

#define JAVA_EXECUTABLE "/usr/bin/java"
#define JAVA_DEFAULT_HOME "/usr/lib/jvm/default-java"
#define HARDCODED_JAVA_HOME "/usr/lib/jvm/java-6-sun"

std::string findJVMFolder() {
    //Try using the JAVA_HOME environmental variable
    char * jvmhome=getenv("JAVA_HOME");
    if(jvmhome) {
       return std::string(jvmhome);
    }
    struct stat sts;
    //Try to trace back the symbolic link from the java executable
    if ((lstat (JAVA_EXECUTABLE, &sts)) == 0) {
	if((sts.st_mode & S_IFMT)==S_IFLNK){
	    //If /usr/bin/java exists and is a symbolic link, then...
            char BUFF1[255];
            char BUFF2[255];
            strcpy(BUFF1,JAVA_EXECUTABLE);
            int sz;
            //Loop to unwind symbolic links.
            do {
                sz=readlink(BUFF1,BUFF2,255);
		if(sz<0) {
                   break;
                }
                BUFF2[sz]=0;
                strcpy(BUFF1,BUFF2);
                lstat(BUFF1,&sts);
            } while((sts.st_mode & S_IFMT) ==S_IFLNK);
            if(sz>0) {
                const char* parentdir=dirname(BUFF1);
                //basename could overwrite parentdir, so we copy it for safe keeping.
                strcpy(BUFF1,parentdir);
                const char* parentbase=basename(BUFF1);
                //If our executable is in a "bin" directory, then we really want the directory above it.
                if(!strcmp(parentbase,"bin")) {
                    parentdir=dirname(BUFF1);
                } else {
                    parentdir=BUFF1;
                }
                return std::string(parentdir);
            }
        }
    }
    //Try the symbolic link to the default JRE on Debian based systems
    if ((stat (JAVA_DEFAULT_HOME, &sts)) == 0) {
        return std::string(HARDCODED_JAVA_HOME);
    }
    //Try the hardcoded Java 6 path (for backwards compatibility)
    if ((stat (HARDCODED_JAVA_HOME, &sts)) == 0) {
        return std::string(HARDCODED_JAVA_HOME);
    }
    cout <<"Could not find any trace of a JVM on your system.\n";
    return std::string("/usr/lib");
}

//Mostly taken from the debian listing of packages with the file libjvm.so in them.
static const char* architectures[]= {"/i386","/amd64","/arm","/alpha","/mips","/mipsel","/ppc","/s390",NULL};
static const char* vmtypes[]= {"/server","/client","/zero","/shark","/cacao",NULL};

std::string findLibJVMso(std::string jvmfolder) {
     struct stat sts;
      if(stat(jvmfolder.c_str(),&sts)!=0) {
         return std::string();
     }
     std::string test=jvmfolder + "/libjvm.so";

     if(stat(test.c_str(),&sts)==0) {
         return test;
     }
     jvmfolder += "/lib";
     test=jvmfolder + "/libjvm.so";
     if(stat(test.c_str(),&sts)==0) {
         return test;
     }
     //Try every combination of architecture and VM type to find libjvm.so
     for(const char** arch=architectures; *arch; arch++) {
         std::string archfolder=jvmfolder + *arch;
         if(stat(archfolder.c_str(),&sts)==0) {
             for(const char** vm=vmtypes; *vm; vm++) {
                 test=archfolder + *vm + "/libjvm.so";
                 if(stat(test.c_str(),&sts)==0) {
                     return test;
                 }
             }
         }
     }

     return std::string();
}

//Returns the set JVM path's handle, or tries the default Ubuntu locaton for the jvm
void* findJVM() {
	void* libjvm;
    if (jvmpath) {
    	cout << "Attempting to load custom JVM\n";
    	libjvm = dlopen(jvmpath, RTLD_LAZY);
		if (libjvm) return libjvm;
    }
    std::string jvmfolder=findJVMFolder();
    cout << "Found jvm folder: " << jvmfolder <<"\n";
    std::string path=findLibJVMso(jvmfolder);
    if(path.size()==0) {
        path=findLibJVMso(jvmfolder + "/jre");
    }
    if(path.size()==0) {
         cout <<"Could not find libjvm.so anywhere. If you think you know where a JVM is installed, try setting the JAVA_HOME environmental variable appropriately.\n";
    }
    libjvm = dlopen(path.c_str(), RTLD_LAZY);
    cout << "Attempting to load JVM discovered in " << path << "\n";

    return libjvm;
}

#endif

