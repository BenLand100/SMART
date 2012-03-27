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


#include <iostream>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

using namespace std;

typedef void* Target; 
typedef union { 
	struct { char b, g, r, a; }; 
	unsigned int color; 
} rgb;
 
typedef Target (*EIOS_RequestTarget)(char *initargs) __attribute__((stdcall)); 
typedef void (*EIOS_ReleaseTarget)(Target t) __attribute__((stdcall)); 
typedef void (*EIOS_GetTargetDimensions)(Target t, int* width, int* height) __attribute__((stdcall)); 
typedef rgb* (*EIOS_GetImageBuffer)(Target t) __attribute__((stdcall)); 
typedef void (*EIOS_UpdateImageBuffer)(Target t) __attribute__((stdcall)); 
typedef void (*EIOS_GetMousePosition)(Target t, int* x, int* y) __attribute__((stdcall)); 
typedef void (*EIOS_MoveMouse)(Target t, int x, int y) __attribute__((stdcall)); 
typedef void (*EIOS_HoldMouse)(Target t, int x, int y, int button) __attribute__((stdcall)); 
typedef void (*EIOS_ReleaseMouse)(Target t, int x, int y, int button) __attribute__((stdcall)); 
typedef bool (*EIOS_IsMouseHeld)(Target t, int button) __attribute__((stdcall)); 
typedef void (*EIOS_SendString)(Target t, char* str) __attribute__((stdcall)); 
typedef void (*EIOS_HoldKey)(Target t, int key) __attribute__((stdcall)); 
typedef void (*EIOS_ReleaseKey)(Target t, int key) __attribute__((stdcall)); 
typedef bool (*EIOS_IsKeyHeld)(Target t, int key) __attribute__((stdcall)); 
 
int main(int argc, char** argv) {
    #ifndef _WIN32
        #if __SIZEOF_POINTER__ == 4
            void* libsmart = dlopen("./libsmart32.so",RTLD_LAZY);
        #else
            void* libsmart = dlopen("./libsmart64.so",RTLD_LAZY);
        #endif
    #else
        #if __SIZEOF_POINTER__ == 4
            HMODULE libsmart = LoadLibrary("./libsmart32.dll");
        #else
            HMODULE libsmart = LoadLibrary("./libsmart64.dll");
        #endif
    #endif
    cout << "Library: " << libsmart << '\n';
    #ifndef _WIN32
        char* error = dlerror();
        if (error) cout << error << '\n';
    #endif
    cout << "Checking for EIOS methods\n";
    
    #define NumMethods 14
    static char* methods[] = {
        (char*)"EIOS_RequestTarget",
        (char*)"EIOS_ReleaseTarget",
        (char*)"EIOS_GetTargetDimensions",
        (char*)"EIOS_GetImageBuffer",
        (char*)"EIOS_UpdateImageBuffer",
        (char*)"EIOS_GetMousePosition",
        (char*)"EIOS_MoveMouse",
        (char*)"EIOS_HoldMouse",
        (char*)"EIOS_ReleaseMouse",
        (char*)"EIOS_IsMouseHeld",
        (char*)"EIOS_SendString",
        (char*)"EIOS_HoldKey",
        (char*)"EIOS_ReleaseKey",
        (char*)"EIOS_IsKeyHeld"
    };
    
    bool bad = false;
    for (int i = 0; i < NumMethods; i++) {
        #ifndef _WIN32
            void* addr = dlsym(libsmart, methods[i]);
        #else
            void* addr = (void*)GetProcAddress(libsmart, methods[i]);
        #endif
        if (!addr) {
            cout << "Failed to import " << methods[i] << '\n';
            bad = true;
        }
    }
    
    cout << (bad ? "There were problems importing EIOS methods" : "EIOS methods imported successfully") << '\n'; 

    return 0;
}

