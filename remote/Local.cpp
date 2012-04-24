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
static int fd;
static void* memmap;
static shm_data *data;

void pairClient(int id) {
    if (memmap) {
        munmap(memmap,width*height*2+sizeof(shm_data));
        close(fd);
    }
    char shmfile[256];
    sprintf(shmfile,"SMART.%i",id);
    fd = open(shmfile,O_RDWR);
    memmap = mmap(NULL,sizeof(shm_data),PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    data = (shm_data*)memmap;
    width = data->width;
    height = data->height;
    munmap(memmap,+sizeof(shm_data));
    memmap = mmap(NULL,2*width*height+sizeof(shm_data),PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    data = (shm_data*)memmap;
}

void* std_getImageArray() {
    return data ? memmap + data->imgstart : 0;
}

void* std_getDebugArray() {
    return data ? memmap + data->dbgstart : 0;
}

void call(int funid) {
    data->funid = funid;
    while (data->funid) { /* sleep */ }
}

int std_getRefresh() {
    call(getRefresh);
    return *(int*)(data->args);
}

void std_setRefresh(int x) {
    *(int*)(data->args) = x;
    call(setRefresh);
}

void std_setTransparentColor(int color) {
    *(int*)(data->args) = color;
    call(setTransparentColor);
}

void std_setDebug(bool enabled) {
    *(bool*)(data->args) = enabled;
    call(setDebug);
}


void std_setGraphics(bool enabled) {
    *(bool*)(data->args) = enabled;
    call(setGraphics);
}

void std_setEnabled(bool enabled) {
    *(bool*)(data->args) = enabled;
    call(setEnabled);
}

bool std_isActive() {
    call(isActive);
    return *(bool*)(data->args);
}

bool std_isBlocking() {
    call(isBlocking);
    return *(bool*)(data->args);
}

void std_getMousePos(int &x, int &y) {
    call(getMousePos);
    x = ((int*)(data->args))[0];
    y = ((int*)(data->args))[1];
}

void std_holdMouse(int x, int y, bool left) {
    ((int*)(data->args))[0] = x;
    ((int*)(data->args))[1] = y;
    ((int*)(data->args))[2] = left; //not a mistake
    call(holdMouse);
}

void std_releaseMouse(int x, int y, bool left) {
    ((int*)(data->args))[0] = x;
    ((int*)(data->args))[1] = y;
    ((int*)(data->args))[2] = left; //not a mistake
    call(releaseMouse);
}

void std_holdMousePlus(int x, int y, int button) {
    ((int*)(data->args))[0] = x;
    ((int*)(data->args))[1] = y;
    ((int*)(data->args))[2] = button;
    call(holdMousePlus);
}

void std_releaseMousePlus(int x, int y, int button) {
    ((int*)(data->args))[0] = x;
    ((int*)(data->args))[1] = y;
    ((int*)(data->args))[2] = button;
    call(releaseMousePlus);
}

void std_moveMouse(int x, int y) {
    ((int*)(data->args))[0] = x;
    ((int*)(data->args))[1] = y;
    call(moveMouse);
}

void std_windMouse(int x, int y) {
    ((int*)(data->args))[0] = x;
    ((int*)(data->args))[1] = y;
    call(windMouse);
}

void std_clickMouse(int x, int y, bool left) {
    ((int*)(data->args))[0] = x;
    ((int*)(data->args))[1] = y;
    ((int*)(data->args))[2] = left; //not a mistake
    call(clickMouse);
}

void std_clickMousePlus(int x, int y, int button) {
    ((int*)(data->args))[0] = x;
    ((int*)(data->args))[1] = y;
    ((int*)(data->args))[2] = button;
    call(clickMousePlus);
}

bool std_isMouseButtonHeld(int button) {
    *(int*)(data->args) = button;
    call(isMouseButtonHeld);
    return *(bool*)(data->args);
}

void std_sendKeys(char *text) {
    strcpy((char*)data->args,text);
    call(sendKeys);
}

void std_holdKey(int code) {
    *(int*)(data->args) = code;
    call(holdKey);
}

void std_releaseKey(int code) {
    *(int*)(data->args) = code;
    call(releaseKey);
}

bool std_isKeyDown(int code) {
    *(int*)(data->args) = code;
    call(isKeyDown);
    return *(bool*)(data->args);
}

int std_getColor(int x, int y) {
    ((int*)(data->args))[0] = x;
    ((int*)(data->args))[1] = y;
    call(getColor);
    return *(int*)(data->args);
}

bool std_findColor(int &x, int& y, int color, int sx, int sy, int ex, int ey) {
    ((int*)(data->args))[0] = x;
    ((int*)(data->args))[1] = y;
    ((int*)(data->args))[2] = color;
    ((int*)(data->args))[3] = sx;
    ((int*)(data->args))[4] = sy;
    ((int*)(data->args))[5] = ex;
    ((int*)(data->args))[6] = ey;
    call(findColor);
    x = ((int*)(data->args))[0];
    y = ((int*)(data->args))[1];
}

bool std_findColorTol(int &x, int& y, int color, int sx, int sy, int ex, int ey, int tol) {
    ((int*)(data->args))[0] = x;
    ((int*)(data->args))[1] = y;
    ((int*)(data->args))[2] = color;
    ((int*)(data->args))[3] = sx;
    ((int*)(data->args))[4] = sy;
    ((int*)(data->args))[5] = ex;
    ((int*)(data->args))[6] = ey;
    ((int*)(data->args))[7] = tol;
    call(findColorTol);
    x = ((int*)(data->args))[0];
    y = ((int*)(data->args))[1];
}

bool std_findColorSpiral(int &x, int& y, int color, int sx, int sy, int ex, int ey) {
    ((int*)(data->args))[0] = x;
    ((int*)(data->args))[1] = y;
    ((int*)(data->args))[2] = color;
    ((int*)(data->args))[3] = sx;
    ((int*)(data->args))[4] = sy;
    ((int*)(data->args))[5] = ex;
    ((int*)(data->args))[6] = ey;
    call(findColorSpiral);
    x = ((int*)(data->args))[0];
    y = ((int*)(data->args))[1];
}

bool std_findColorSpiralTol(int &x, int& y, int color, int sx, int sy, int ex, int ey, int tol) {
    ((int*)(data->args))[0] = x;
    ((int*)(data->args))[1] = y;
    ((int*)(data->args))[2] = color;
    ((int*)(data->args))[3] = sx;
    ((int*)(data->args))[4] = sy;
    ((int*)(data->args))[5] = ex;
    ((int*)(data->args))[6] = ey;
    ((int*)(data->args))[7] = tol;
    call(findColorSpiralTol);
    x = ((int*)(data->args))[0];
    y = ((int*)(data->args))[1];
}

void internal_constructor() {
    memmap = 0;
    data = 0;
}

void internal_destructor() {
    if (memmap) {
        munmap(memmap,width*height*2+sizeof(shm_data));
        close(fd);
    }
}

int main(int argc, char** argv) {
    pairClient(atoi(argv[1]));
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
