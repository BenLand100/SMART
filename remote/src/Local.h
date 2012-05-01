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
 
#ifndef _LOCAL_H
#define	_LOCAL_H

#include "Bridge.h"

#ifndef _WIN32
#include <dlfcn.h>
void load() __attribute__((constructor));
void unload() __attribute__((destructor));
#else
#include <windows.h>
extern "C" bool DllMain(HINSTANCE, int, void*) __attribute__((stdcall));
#endif

#if __SIZEOF_POINTER__ == 4
    #define bits "32"
#else
    #define bits "64"
#endif
#define TIMEOUT 5

typedef struct {
    int width,height;
    #ifndef _WIN32
    int fd;
    void* memmap;
    #else
    HANDLE file;
    HANDLE memmap;
    #endif
    shm_data *data;
} SMARTClient;

typedef struct {
    int count;
    int *ids;
} clients_dat;

typedef SMARTClient* Target;
typedef union { 
	struct { char b, g, r, a; }; 
	unsigned int color; 
} rgb; 

void call(int funid);
 
extern "C" Target EIOS_RequestTarget(char *initargs) __attribute__((stdcall)); 
extern "C" void EIOS_ReleaseTarget(Target t) __attribute__((stdcall)); 
extern "C" void EIOS_GetTargetDimensions(Target t, int* width, int* height) __attribute__((stdcall)); 
extern "C" rgb* EIOS_GetImageBuffer(Target t) __attribute__((stdcall)); 
extern "C" void EIOS_UpdateImageBuffer(Target t) __attribute__((stdcall)); 
extern "C" void EIOS_GetMousePosition(Target t, int* x, int* y) __attribute__((stdcall)); 
extern "C" void EIOS_MoveMouse(Target t, int x, int y) __attribute__((stdcall)); 
extern "C" void EIOS_HoldMouse(Target t, int x, int y, int button) __attribute__((stdcall)); 
extern "C" void EIOS_ReleaseMouse(Target t, int x, int y, int button) __attribute__((stdcall)); 
extern "C" bool EIOS_IsMouseHeld(Target t, int button) __attribute__((stdcall)); 
extern "C" void EIOS_SendString(Target t, char* str) __attribute__((stdcall)); 
extern "C" void EIOS_HoldKey(Target t, int key) __attribute__((stdcall)); 
extern "C" void EIOS_ReleaseKey(Target t, int key) __attribute__((stdcall)); 
extern "C" bool EIOS_IsKeyHeld(Target t, int key) __attribute__((stdcall)); 


extern "C" int std_clientID(int idx) __attribute__((stdcall));
extern "C" int std_getClients(bool only_unpaired) __attribute__((stdcall));
extern "C" int std_spawnClient(char* remote_path, char *root, char *params, int width, int height, char *initseq, char *useragent, char *jvmpath, int maxmem) __attribute__((stdcall));
extern "C" bool std_pairClient(int pid) __attribute__((stdcall));
extern "C" int std_getCurrent() __attribute__((stdcall));
extern "C" bool std_killClient(int id) __attribute__((stdcall));
extern "C" void* std_getImageArray() __attribute__((stdcall));
extern "C" void* std_getDebugArray() __attribute__((stdcall));
extern "C" int std_getRefresh() __attribute__((stdcall));
extern "C" void std_setRefresh(int x) __attribute__((stdcall));
extern "C" void std_setTransparentColor(int color) __attribute__((stdcall));
extern "C" void std_setDebug(bool enabled) __attribute__((stdcall));
extern "C" void std_setGraphics(bool enabled) __attribute__((stdcall));
extern "C" void std_setEnabled(bool enabled) __attribute__((stdcall));
extern "C" bool std_isActive() __attribute__((stdcall));
extern "C" bool std_isBlocking() __attribute__((stdcall));
extern "C" void std_getMousePos(int &x, int &y) __attribute__((stdcall));
extern "C" void std_holdMouse(int x, int y, bool left) __attribute__((stdcall));
extern "C" void std_releaseMouse(int x, int y, bool left) __attribute__((stdcall));
extern "C" void std_holdMousePlus(int x, int y, int button) __attribute__((stdcall));
extern "C" void std_releaseMousePlus(int x, int y, int button) __attribute__((stdcall));
extern "C" void std_moveMouse(int x, int y) __attribute__((stdcall));
extern "C" void std_windMouse(int x, int y) __attribute__((stdcall));
extern "C" void std_clickMouse(int x, int y, bool left) __attribute__((stdcall));
extern "C" void std_clickMousePlus(int x, int y, int button) __attribute__((stdcall));
extern "C" bool std_isMouseButtonHeld(int button) __attribute__((stdcall));
extern "C" void std_sendKeys(char *text) __attribute__((stdcall));
extern "C" void std_holdKey(int code) __attribute__((stdcall));
extern "C" void std_releaseKey(int code) __attribute__((stdcall));
extern "C" bool std_isKeyDown(int code) __attribute__((stdcall));
extern "C" int std_getColor(int x, int y) __attribute__((stdcall));
extern "C" bool std_findColor(int &x, int& y, int color, int sx, int sy, int ex, int ey) __attribute__((stdcall));
extern "C" bool std_findColorTol(int &x, int& y, int color, int sx, int sy, int ex, int ey, int tol) __attribute__((stdcall));
extern "C" bool std_findColorSpiral(int &x, int& y, int color, int sx, int sy, int ex, int ey) __attribute__((stdcall));
extern "C" bool std_findColorSpiralTol(int &x, int& y, int color, int sx, int sy, int ex, int ey, int tol) __attribute__((stdcall));

//Exports for Local
#define NumExports 34
static char* exports[] = {
    (char*)"std_clientID", (char*)"function SmartClientID(idx: integer): integer;",
    (char*)"std_getClients", (char*)"function SmartGetClients(only_unpaired: boolean): integer;",
    (char*)"std_spawnClient",(char*)"function SmartSpawnClient(remote_path, root, params: string; width, height: integer; initseq, useragent, jvmpath: string; maxmem: integer): integer;",
    (char*)"std_pairClient", (char*)"function SmartPairClient(pid: integer): boolean;",
    (char*)"std_killClient", (char*)"function SmartKillClient(pid: integer): boolean;",
    (char*)"std_getCurrent", (char*)"function SmartCurrentClient(): integer;",
    (char*)"std_getImageArray", (char*)"function SmartImageArray(): integer;",
    (char*)"std_getDebugArray", (char*)"function SmartDebugArray(): integer;",
    (char*)"std_getRefresh", (char*)"function SmartGetRefresh: integer;",
    (char*)"std_setRefresh", (char*)"procedure SmartSetRefresh(x: integer);",
    (char*)"std_setTransparentColor", (char*)"procedure SmartSetTransparentColor(color: integer);",
    (char*)"std_setDebug", (char*)"procedure SmartSetDebug(enabled: boolean);",
    (char*)"std_setGraphics", (char*)"procedure SmartSetGraphics(enabled: boolean);",
    (char*)"std_setEnabled", (char*)"procedure SmartSetEnabled(enabled: boolean);",
    (char*)"std_isActive", (char*)"function SmartActive: boolean;",
    (char*)"std_isBlocking", (char*)"function SmartEnabled: boolean;",
    (char*)"std_getMousePos", (char*)"procedure SmartGetMousePos(var x, y: integer);",
    (char*)"std_holdMouse", (char*)"procedure SmartHoldMouse(x, y: integer; left: boolean);",
    (char*)"std_releaseMouse", (char*)"procedure SmartReleaseMouse(x, y: integer; left: boolean);",
    (char*)"std_holdMousePlus", (char*)"procedure SmartHoldMousePlus(x, y, button: integer);",
    (char*)"std_releaseMousePlus", (char*)"procedure SmartReleaseMousePlus(x, y, button: integer);",
    (char*)"std_moveMouse", (char*)"procedure SmartMoveMouse(x, y: integer);",
    (char*)"std_windMouse", (char*)"procedure SmartWindMouse(x, y: integer);",
    (char*)"std_clickMouse", (char*)"procedure SmartClickMouse(x, y: integer; left: boolean);",
    (char*)"std_clickMousePlus", (char*)"procedure SmartClickMousePlus(x, y, button: integer);",
    (char*)"std_isMouseButtonHeld", (char*)"function SmartIsMouseButtonHeld(button: integer): boolean;",
    (char*)"std_sendKeys", (char*)"procedure SmartSendKeys(Text: String);",
    (char*)"std_holdKey", (char*)"procedure SmartHoldKey(Code: Integer);",
    (char*)"std_releaseKey", (char*)"procedure SmartReleaseKey(Code: Integer);",
    (char*)"std_isKeyDown", (char*)"function SmartIsKeyDown(Code: Integer): Boolean;",
    (char*)"std_getColor", (char*)"function SmartGetColor(x, y: integer): integer;",
    (char*)"std_findColor", (char*)"function SmartFindColor(var x, y: integer; color, sx, sy, ex, ey: integer): boolean;",
    (char*)"std_findColorTol", (char*)"function SmartFindColorTolerance(var x, y: integer; color, sx, sy, ex, ey, tol: integer): boolean;",
    (char*)"std_findColorSpiral", (char*)"function SmartFindColorSpiral(var x, y: integer; color, sx, sy, ex, ey: integer): boolean;",
    (char*)"std_findColorSpiralTol", (char*)"function SmartFindColorSpiralTolerance(var x, y: integer; color, sx, sy, ex, ey, tol: integer): boolean;",
};

extern "C" int GetFunctionCount() __attribute__((stdcall));
extern "C" int GetFunctionInfo(int, void*&, char*&) __attribute__((stdcall));

#endif	/* _LOCAL_H */

