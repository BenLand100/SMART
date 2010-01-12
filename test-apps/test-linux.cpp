/**
 *  Copyright 2010 by Benjamin J. Land (a.k.a. BenLand100)
 *
 *  This file is part of the Scar Minimizing Autoing Resource Thing (SMART)
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
#include <dlfcn.h>

using namespace std;

typedef long (*GetFunctionCount) (void) __attribute__((stdcall));
typedef long (*GetFunctionInfo)(int, void*&, char*&) __attribute__((stdcall));
typedef void (*Setup)(char*,char*,long,long) __attribute__((stdcall));

int main(int argc, char** argv) {
    void* libsmart = dlopen("./libsmart.so",RTLD_LAZY);
    cout << "Library: " << libsmart << '\n';
    char* error = dlerror();
    if (error) cout << error << '\n';
    cout << "GetFunctionCount: " << dlsym(libsmart, "GetFunctionCount") << '\n';
    cout << "GetFunctionInfo: " << dlsym(libsmart, "GetFunctionInfo") << '\n';
    GetFunctionCount count = (GetFunctionCount) dlsym(libsmart, "GetFunctionCount");
    GetFunctionInfo info = (GetFunctionInfo) dlsym(libsmart, "GetFunctionInfo");

    int exports = count();
    cout << "#Exports = " << count() << '\n';
    for (int i = 0; i < exports; i++) {
        char* def = new char[1024];
        void* addr;
        info(i,addr,def);
        cout << '\t' << addr << " = " << def  << '\n';
        delete def;
    }

    Setup setup = (Setup) dlsym(libsmart, "setup");
    setup((char*) "http://world142.runescape.com/", (char*) "plugin.js?param=o0,a1,m0",765,503);

    time_t timer = time(0) + 500;
    while(timer >= time(0));

    return 0;
}

