/**
 *  Copyright 2006-2013 by Benjamin J. Land (a.k.a. BenLand100)
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

#include <iostream>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif


using namespace std;

typedef void (*SpawnClient)(char* java_exec, char* remote_path, char *root, char *params, int width, int height, char *initseq, char *useragent, char* javaargs, char* plugins);

int main(int argc, char** argv) {
    #ifndef _WIN32
        #define JAVA_EXEC "java"
        #if __SIZEOF_POINTER__ == 4
            #define PLUGIN "./example-plugin32.so"
            void* libsmart = dlopen("./libsmartremote32.so",RTLD_LAZY);
        #else
            #define PLUGIN "./example-plugin64.so"
            void* libsmart = dlopen("./libsmartremote64.so",RTLD_LAZY);
        #endif
    #else
        #define JAVA_EXEC "javaw.exe"
        #if __SIZEOF_POINTER__ == 4
            #define PLUGIN "./example-plugin32.dll"
            HMODULE libsmart = LoadLibrary("./libsmartremote32.dll");
        #else
            #define PLUGIN "./example-plugin64.dll"
            HMODULE libsmart = LoadLibrary("./libsmartremote64.dll");
        #endif
    #endif
    cout << "Library: " << libsmart << '\n';
    #ifndef _WIN32
        char* error = dlerror();
        if (error) cout << error << '\n';
    #endif

    #ifndef _WIN32
        SpawnClient spawn = (SpawnClient)dlsym(libsmart, "exp_spawnClient");
    #else
        SpawnClient spawn = (SpawnClient)GetProcAddress(libsmart, "exp_spawnClient");
    #endif
    
    spawn((char*)JAVA_EXEC,(char*)".",(char*)"http://world37.runescape.com/",(char*)",f681985954784915908",765,553,(char*)"s",NULL,NULL,(char*)PLUGIN);

    return 0;
}

