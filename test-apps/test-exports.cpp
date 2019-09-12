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

typedef void* Target; 
typedef union { 
	struct { char b, g, r, a; }; 
	unsigned int color; 
} rgb;

typedef int (*GetPluginABIVersion)();
typedef int (*GetFunctionCount)();
typedef int (*GetFunctionInfo)(int index, void*& address, char*& def);
 
void* link(void *module, char *method) {
    #ifndef _WIN32
        return dlsym(module, method);
    #else
        return (void*)GetProcAddress((HMODULE)module, method);
    #endif
}
 
int main(int argc, char** argv) {
    #ifndef _WIN32
        #if __SIZEOF_POINTER__ == 4
            void* libsmart = dlopen((char*)"./libsmartremote32.so",RTLD_LAZY);
        #else
            void* libsmart = dlopen((char*)"./libsmartremote64.so",RTLD_LAZY);
        #endif
    #else
        #if __SIZEOF_POINTER__ == 4
            HMODULE libsmart = LoadLibrary((char*)".\\libsmartremote32.dll");
        #else
            HMODULE libsmart = LoadLibrary((char*)".\\libsmartremote64.dll");
        #endif
    #endif
    cout << "Library: " << libsmart << '\n';
    #ifndef _WIN32
        char* error = dlerror();
        if (error) cout << error << '\n';
    #endif
    
    GetPluginABIVersion version = (GetPluginABIVersion)link(libsmart,(char*)"GetPluginABIVersion");
    GetFunctionCount count = (GetFunctionCount)link(libsmart,(char*)"GetFunctionCount");
    GetFunctionInfo info = (GetFunctionInfo)link(libsmart,(char*)"GetFunctionInfo");
    
    cout << (void*)version << ' ' << (void*)count << ' ' << (void*)info << '\n';
    if (!version || !count || !info) {
        cout << "Error loading core import methods\n";
        return 1;
    }
    int n = 0;
    cout << "ABI: " << version() << " Count: " << (n = count()) << '\n';
    char *def = new char[1024];
    void *addr;
    bool bad = false;
    for (int i = 0; i < n; i++) {
        info(i,addr,def);
        if (!addr) bad = true;
        cout << i << ' ' << addr << ' ' << def << '\n';
    }
    delete[] def;
    cout << (bad ? "There were problems importing methods" : "Methods imported successfully") << '\n'; 
    return 0;
}

