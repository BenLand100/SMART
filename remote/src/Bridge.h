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
 
typedef struct {
    int id;             //PID of the client
    int paired;         //TID of the controller or ZERO if none
    int width,height;   //Size of the images that follow this structure in the SHM
    int time;           //Last time client active (zero if SMART hasn't initilized yet)
    int die;            //Set nonzero to have the client terminate
    int imgstart;       //Offset in the SHM where the image is stored
    int dbgstart;       //Offset in the SHM where the debug image is stored
    
    /**
     * Comm protocol - Controller sets arguments then sets funid to the ID 
     * (nonzero) of the requested function. The Client executes the function
     * then stores the results in args. Finally, the client sets funid back 
     * to zero to signify the function is completed.
     */
    unsigned char funid, args[4096];
} shm_data;


//Remote uses these typedefs to invoke SMART exports
typedef void* (*type_getImageArray)() __attribute__((cdecl));
typedef void* (*type_getDebugArray)() __attribute__((cdecl));
typedef int (*type_getRefresh)() __attribute__((cdecl));
typedef void (*type_setRefresh)(int x) __attribute__((cdecl));
typedef void (*type_setTransparentColor)(int color) __attribute__((cdecl));
typedef void (*type_setDebug)(bool enabled) __attribute__((cdecl));
typedef void (*type_setGraphics)(bool enabled) __attribute__((cdecl));
typedef void (*type_setEnabled)(bool enabled) __attribute__((cdecl));
typedef bool (*type_isActive)() __attribute__((cdecl));
typedef bool (*type_isBlocking)() __attribute__((cdecl));
typedef void (*type_getMousePos)(int &x, int &y) __attribute__((cdecl));
typedef void (*type_holdMouse)(int x, int y, bool left) __attribute__((cdecl));
typedef void (*type_releaseMouse)(int x, int y, bool left) __attribute__((cdecl));
typedef void (*type_holdMousePlus)(int x, int y, int button) __attribute__((cdecl));
typedef void (*type_releaseMousePlus)(int x, int y, int button) __attribute__((cdecl));
typedef void (*type_moveMouse)(int x, int y) __attribute__((cdecl));
typedef void (*type_windMouse)(int x, int y) __attribute__((cdecl));
typedef void (*type_clickMouse)(int x, int y, bool left) __attribute__((cdecl));
typedef void (*type_clickMousePlus)(int x, int y, int button) __attribute__((cdecl));
typedef bool (*type_isMouseButtonHeld)(int button) __attribute__((cdecl));
typedef void (*type_sendKeys)(char *text) __attribute__((cdecl));
typedef void (*type_holdKey)(int code) __attribute__((cdecl));
typedef void (*type_releaseKey)(int code) __attribute__((cdecl));
typedef bool (*type_isKeyDown)(int code) __attribute__((cdecl));
typedef int (*type_getColor)(int x, int y) __attribute__((cdecl));
typedef bool (*type_findColor)(int &x, int& y, int color, int sx, int sy, int ex, int ey) __attribute__((cdecl));
typedef bool (*type_findColorTol)(int &x, int& y, int color, int sx, int sy, int ex, int ey, int tol) __attribute__((cdecl));
typedef bool (*type_findColorSpiral)(int &x, int& y, int color, int sx, int sy, int ex, int ey) __attribute__((cdecl));
typedef bool (*type_findColorSpiralTol)(int &x, int& y, int color, int sx, int sy, int ex, int ey, int tol) __attribute__((cdecl));

//These methods are exported by Local
extern "C" int std_clientID(int idx) __attribute__((stdcall));
extern "C" int std_getClients(bool only_unpaired) __attribute__((stdcall));
extern "C" int std_spawnClient(char* remote_path, char *root, char *params, int width, int height, char *initseq, char *useragent, char *jvmpath, int maxmem) __attribute__((stdcall));
extern "C" bool std_pairClient(int pid) __attribute__((stdcall));
extern "C" int std_getCurrent() __attribute__((stdcall));
extern "C" void std_killClient(int id) __attribute__((stdcall));
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
    (char*)"std_killClient", (char*)"procedure SmartKillClient(pid: integer);",
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

/**
 * Comm protocol: use these numbers as funid, everything less or equal to 
 * NumImports is a direct export from SMART and must correspond to the name in
 * imports[def-FirstFunc]
 */

#define FirstFunc           1

#define getRefresh          FirstFunc+0
#define setRefresh          FirstFunc+1
#define setTransparentColor FirstFunc+2
#define setDebug            FirstFunc+3
#define setGraphics         FirstFunc+4 
#define setEnabled          FirstFunc+5
#define isActive            FirstFunc+6
#define isBlocking          FirstFunc+7
#define getMousePos         FirstFunc+8
#define holdMouse           FirstFunc+9
#define releaseMouse        FirstFunc+10
#define holdMousePlus       FirstFunc+11
#define releaseMousePlus    FirstFunc+12
#define moveMouse           FirstFunc+13
#define windMouse           FirstFunc+14
#define clickMouse          FirstFunc+15
#define clickMousePlus      FirstFunc+16
#define isMouseButtonHeld   FirstFunc+17
#define sendKeys            FirstFunc+18
#define holdKey             FirstFunc+19
#define releaseKey          FirstFunc+20
#define isKeyDown           FirstFunc+21
#define getColor            FirstFunc+22
#define findColor           FirstFunc+23
#define findColorTol        FirstFunc+24
#define findColorSpiral     FirstFunc+25
#define findColorSpiralTol  FirstFunc+26

#define ExtraFuncs          FirstFunc+27

#define Ping                ExtraFuncs+0

//Imports for Remote
#define NumImports 27
static char* imports[] = {
    (char*)"getRefresh",
    (char*)"setRefresh", 
    (char*)"setTransparentColor", 
    (char*)"setDebug",
    (char*)"setGraphics",
    (char*)"setEnabled", 
    (char*)"isActive", 
    (char*)"isBlocking", 
    (char*)"getMousePos",
    (char*)"holdMouse",
    (char*)"releaseMouse",
    (char*)"holdMousePlus",
    (char*)"releaseMousePlus",
    (char*)"moveMouse", 
    (char*)"windMouse", 
    (char*)"clickMouse",
    (char*)"clickMousePlus",
    (char*)"isMouseButtonHeld",
    (char*)"sendKeys",
    (char*)"holdKey", 
    (char*)"releaseKey", 
    (char*)"isKeyDown", 
    (char*)"getColor", 
    (char*)"findColor", 
    (char*)"findColorTol", 
    (char*)"findColorSpiral",
    (char*)"findColorSpiralTol",
};

