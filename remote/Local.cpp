/**
 *  Copyright 2012 by Benjamin J. Land (a.k.a. BenLand100)
 *
 *  This file is part of the SMART-Remote
 *
 *  SMART-Remote is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SMART-Remote is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SMART-Remote. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Local.h"
#include "Bridge.h"
#include <time.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>

using namespace std;

static int width,height;
void* memmap;
shm_data *data;

void* std_getImageArray() {
    return data ? memmap + data->imgstart : 0;
}

void* std_getDebugArray() {
    return data ? memmap + data->dbgstart : 0;
}

void std_pairClient(int id, int _width, int _height) {
    if (memmap) munmap(memmap,width*height*2+sizeof(shm_data));
    width = _width;
    height = _height;
    char shmfile[256];
    sprintf(shmfile,"SMART.%i",id);
    int fd = open(shmfile,O_RDWR);
    memmap = mmap(NULL,width*height*2+sizeof(shm_data),PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    data = (shm_data*)memmap;
}

void internal_constructor() {
    memmap = 0;
    data = 0;
}

void internal_destructor() {
    if (memmap) munmap(memmap,width*height*2+sizeof(shm_data));
}

int main(int argc, char** argv) {
    std_pairClient(atoi(argv[1]),765,503);
    data->die = 1;
}

/*
#ifndef _WIN32

int GetFunctionCount() {
    return NumExports;
}

int GetFunctionInfo(int index, void*& address, char*& def) {
    if (index < NumExports) {
        address = dlsym(RTLD_DEFAULT,exports[index*2]);
        strcpy(def, exports[index*2+1]);
        return index;
    }
    return -1;
}

void load(void) {
    internalConstructor();
}

void unload(void) {
    internalDestructor();
}

#endif

#ifdef _WIN32

HMODULE dllinst;

int GetFunctionCount() {
    return NumExports;
}

int GetFunctionInfo(int index, void*& address, char*& def) {
    if (index < NumExports) {
        address = (void*)GetProcAddress(dllinst, exports[index*2]);
        strcpy(def, exports[index*2+1]);
        return index;
    }
    return -1;
}

bool DllMain(HINSTANCE instance, int reason, void* checks) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            dllinst = instance;
            internalConstructor();
            return true;
        case DLL_THREAD_ATTACH:
            return true;
        case DLL_PROCESS_DETACH:
            internalDestructor();
            return true;
        case DLL_THREAD_DETACH:
            return true;
    }
    return false;
}

#endif
*/
