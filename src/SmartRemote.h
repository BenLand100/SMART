/**
 *  Copyright 2012 by Benjamin J. Land (a.k.a. BenLand100)
 *
 *  This file is part of the SMART
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
 
#ifndef _SMARTREMOTE_H
#define	_SMARTREMOTE_H

typedef struct {
    int port;           //The port the client listens on
    int id;             //ID of the client: SMART.id
    int width,height;   //Size of the images that follow this structure in the SHM
    int paired;         //TID of the controller or ZERO if none
    int imgoff;         //Offset in the SHM where the image is stored
    int dbgoff;         //Offset in the SHM where the debug image is stored
    
    /**
     * Comm protocol - Controller sets arguments then writes funid to the client's
     * socket to notify it of the new call. The Client executes the function
     * then stores the results in args. Finally, the client echos the funid to
     * the controller
     */
    unsigned char args[4096];
} shm_data;


#ifndef _WIN32
    #include <unistd.h>
    #include <string.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h> 
#else
    #if __SIZEOF_POINTER__ == 4
        //some versions of mingw are missing this
        #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0501
        #endif
    #endif
    #include <windows.h>
#endif

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
#define scrollMouse         FirstFunc+18
#define sendKeys            FirstFunc+19
#define holdKey             FirstFunc+20
#define releaseKey          FirstFunc+21
#define isKeyDown           FirstFunc+22
#define setCapture          FirstFunc+23
#define setNativeBtn        FirstFunc+24

#define ReflectionFuncs			 FirstFunc+25
#define getFieldObject           ReflectionFuncs+0
#define isPathValid              ReflectionFuncs+1
#define getFieldBoolean          ReflectionFuncs+2
#define getFieldLongH            ReflectionFuncs+3
#define getFieldLongL            ReflectionFuncs+4
#define getFieldInt              ReflectionFuncs+5
#define getFieldShort            ReflectionFuncs+6
#define getFieldFloat            ReflectionFuncs+7
#define getFieldDouble           ReflectionFuncs+8
#define getFieldByte             ReflectionFuncs+9
#define getFieldArray3DObject    ReflectionFuncs+10
#define getFieldArray3DByte      ReflectionFuncs+11
#define getFieldArray3DChar      ReflectionFuncs+12
#define getFieldArray3DShort     ReflectionFuncs+13
#define getFieldArray3DInt       ReflectionFuncs+14
#define getFieldArray3DFloat     ReflectionFuncs+15
#define getFieldArray3DDouble    ReflectionFuncs+16
#define getFieldArray3DBoolean   ReflectionFuncs+17
#define getFieldArray3DLongL     ReflectionFuncs+18
#define getFieldArray3DLongH     ReflectionFuncs+19
#define getFieldArray2DObject    ReflectionFuncs+20
#define getFieldArray2DByte      ReflectionFuncs+21
#define getFieldArray2DChar      ReflectionFuncs+22
#define getFieldArray2DShort     ReflectionFuncs+23
#define getFieldArray2DInt       ReflectionFuncs+24
#define getFieldArray2DFloat     ReflectionFuncs+25
#define getFieldArray2DDouble    ReflectionFuncs+26
#define getFieldArray2DBoolean   ReflectionFuncs+27
#define getFieldArray2DLongL     ReflectionFuncs+28
#define getFieldArray2DLongH     ReflectionFuncs+29
#define getFieldArray1DObject    ReflectionFuncs+30
#define getFieldArray1DByte      ReflectionFuncs+31
#define getFieldArray1DChar      ReflectionFuncs+32
#define getFieldArray1DShort     ReflectionFuncs+33
#define getFieldArray1DInt       ReflectionFuncs+34
#define getFieldArray1DFloat     ReflectionFuncs+35
#define getFieldArray1DDouble    ReflectionFuncs+36
#define getFieldArray1DBoolean   ReflectionFuncs+37
#define getFieldArray1DLongL     ReflectionFuncs+38
#define getFieldArray1DLongH     ReflectionFuncs+39
#define getFieldArraySize        ReflectionFuncs+40
#define freeObject               ReflectionFuncs+41
#define stringFromString         ReflectionFuncs+42
#define stringFromChars          ReflectionFuncs+43
#define stringFromBytes          ReflectionFuncs+44
#define isNull                   ReflectionFuncs+45
#define isEqual                  ReflectionFuncs+46

#define ExtraFuncs          ReflectionFuncs+47
#define Ping                ExtraFuncs+0
#define Die                 ExtraFuncs+1



#ifndef _WIN32
#include <dlfcn.h>
void load() __attribute__((constructor));
void unload() __attribute__((destructor));
#else
extern "C" bool DllMain(HINSTANCE, int, void*) __attribute__((stdcall));
#endif

#if __SIZEOF_POINTER__ == 4
    #define bits "32"
#else
    #define bits "64"
#endif
#define TIMEOUT 5

extern "C" int GetPluginABIVersion();
extern "C" int GetFunctionCount();
extern "C" int GetFunctionInfo(int, void*&, char*&);

typedef struct {
    int width,height;
    int refcount;
    #ifndef _WIN32
    int socket;
    int fd;
    void* memmap;
    #else
    SOCKET socket;
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
extern "C" void EIOS_ScrollMouse(Target t, int x, int y, int lines) __attribute__((stdcall));
extern "C" void EIOS_SendString(Target t, char* str, int keywait, int keymodwait) __attribute__((stdcall)); 
extern "C" void EIOS_HoldKey(Target t, int key) __attribute__((stdcall)); 
extern "C" void EIOS_ReleaseKey(Target t, int key) __attribute__((stdcall)); 
extern "C" bool EIOS_IsKeyHeld(Target t, int key) __attribute__((stdcall)); 


extern "C" int exp_clientID(int idx);
extern "C" int exp_getClients(bool only_unpaired);
extern "C" int exp_spawnClient(char* remote_path, char *root, char *params, int width, int height, char *initseq, char *useragent, char* javaargs, char* Plugins);
extern "C" bool exp_pairClient(int pid);
extern "C" int exp_getCurrent();
extern "C" bool exp_killClient(int pid);
extern "C" void* exp_getImageArray();
extern "C" void* exp_getDebugArray();
extern "C" int exp_getRefresh();
extern "C" void exp_setRefresh(int x);
extern "C" void exp_setTransparentColor(int color);
extern "C" void exp_setDebug(bool enabled);
extern "C" void exp_setGraphics(bool enabled);
extern "C" void exp_setEnabled(bool enabled);
extern "C" bool exp_isActive();
extern "C" bool exp_isBlocking();
extern "C" void exp_getMousePos(int &x, int &y);
extern "C" void exp_holdMouse(int x, int y, bool left);
extern "C" void exp_releaseMouse(int x, int y, bool left);
extern "C" void exp_holdMousePlus(int x, int y, int button);
extern "C" void exp_releaseMousePlus(int x, int y, int button);
extern "C" void exp_moveMouse(int x, int y);
extern "C" void exp_windMouse(int x, int y);
extern "C" void exp_clickMouse(int x, int y, bool left);
extern "C" void exp_clickMousePlus(int x, int y, int button);
extern "C" bool exp_isMouseButtonHeld(int button);
extern "C" void exp_scrollMouse(int x, int y, int lines);
extern "C" void exp_sendKeys(char *text, int keywait, int keymodwait);
extern "C" void exp_holdKey(int code);
extern "C" void exp_releaseKey(int code);
extern "C" bool exp_isKeyDown(int code);
extern "C" void exp_setCapture(bool enabled);
extern "C" void exp_setNativeBtn(int plugid, int btnid, bool state);

extern "C" void* exp_getFieldObject(void* obj, char* path);
extern "C" bool exp_isPathValid(void* obj, char* path);
extern "C" bool exp_getFieldBoolean(void* obj, char* path);
extern "C" unsigned int exp_getFieldLongH(void* obj, char* path);
extern "C" unsigned int exp_getFieldLongL(void* obj, char* path);
extern "C" int exp_getFieldInt(void* obj, char* path);
extern "C" int exp_getFieldShort(void* obj, char* path);
extern "C" double exp_getFieldFloat(void* obj, char* path);
extern "C" double exp_getFieldDouble(void* obj, char* path);
extern "C" int exp_getFieldByte(void* obj, char* path);

extern "C" void* exp_getFieldArray3DObject(void* obj, char* path, int x, int y, int z);
extern "C" int exp_getFieldArray3DByte(void* obj, char* path, int x, int y, int z);
extern "C" int exp_getFieldArray3DChar(void* obj, char* path, int x, int y, int z);
extern "C" int exp_getFieldArray3DShort(void* obj, char* path, int x, int y, int z);
extern "C" int exp_getFieldArray3DInt(void* obj, char* path, int x, int y, int z);
extern "C" float exp_getFieldArray3DFloat(void* obj, char* path, int x, int y, int z);
extern "C" float exp_getFieldArray3DDouble(void* obj, char* path, int x, int y, int z);
extern "C" bool exp_getFieldArray3DBool(void* obj, char* path, int x, int y, int z);
extern "C" int exp_getFieldArray3DLongH(void* obj, char* path, int x, int y, int z); 
extern "C" int exp_getFieldArray3DLongL(void* obj, char* path, int x, int y, int z);

extern "C" void* exp_getFieldArray2DObject(void* obj, char* path, int x, int y);
extern "C" int exp_getFieldArray2DInt(void* obj, char* path, int x, int y);
extern "C" float exp_getFieldArray2DDouble(void* obj, char* path, int x, int y);
extern "C" float exp_getFieldArray2DFloat(void* obj, char* path, int x, int y);
extern "C" bool exp_getFieldArray2DBool(void* obj, char* path, int x, int y);
extern "C" int exp_getFieldArray2DLongH(void* obj, char* path, int x, int y);
extern "C" int exp_getFieldArray2DLongL(void* obj, char* path, int x, int y);
extern "C" int exp_getFieldArray2DByte(void* obj, char* path, int x, int y);
extern "C" int exp_getFieldArray2DChar(void* obj, char* path, int x, int y);
extern "C" int exp_getFieldArray2DShort(void* obj, char* path, int x, int y);

extern "C" int exp_getFieldArraySize(void* obj, char* path, int dim);
extern "C" void* exp_getFieldArrayObject(void* obj, char* path, int index);
extern "C" int exp_getFieldArrayInt(void* obj, char* path, int index);
extern "C" float exp_getFieldArrayFloat(void* obj, char* path, int index);
extern "C" float exp_getFieldArrayDouble(void* obj, char* path, int index);
extern "C" bool exp_getFieldArrayBool(void* obj, char* path, int index);
extern "C" int exp_getFieldArrayLongH(void* obj, char* path, int index);
extern "C" int exp_getFieldArrayLongL(void* obj, char* path, int index);
extern "C" int exp_getFieldArrayByte(void* obj, char* path, int index);
extern "C" int exp_getFieldArrayShort(void* obj, char* path, int index);
extern "C" int exp_getFieldArrayChar(void* obj, char* path, int index);

extern "C" void exp_freeObject(void* obj);
extern "C" int exp_stringFromString(void* obj, char* delphistr);
extern "C" int exp_stringFromChars(void* obj, char* delphistr);
extern "C" int exp_stringFromBytes(void* obj, char* delphistr);
extern "C" bool exp_isNull(void* obj);
extern "C" bool exp_isEqual(void* a, void* b);

#if __SIZEOF_POINTER__ == 4
    #define PTR "integer"
#else
    #define PTR "int64"
#endif

//Exports for Local
#define NumExports 80
static char* exports[] = {
    (char*)"exp_clientID", (char*)"function SmartClientID(idx: integer): integer;",
    (char*)"exp_getClients", (char*)"function SmartGetClients(only_unpaired: boolean): integer;",
    (char*)"exp_spawnClient",(char*)"function SmartSpawnClient(remote_path, root, params: string; width, height: integer; initseq, useragent, javaargs, Plugins: string): integer;",
    (char*)"exp_pairClient", (char*)"function SmartPairClient(pid: integer): boolean;",
    (char*)"exp_killClient", (char*)"function SmartKillClient(pid: integer): boolean;",
    (char*)"exp_getCurrent", (char*)"function SmartCurrentClient(): integer;",
    (char*)"exp_getImageArray", (char*)"function SmartImageArray(): "PTR";",
    (char*)"exp_getDebugArray", (char*)"function SmartDebugArray(): "PTR";",
    (char*)"exp_getRefresh", (char*)"function SmartGetRefresh: integer;",
    (char*)"exp_setRefresh", (char*)"procedure SmartSetRefresh(x: integer);",
    (char*)"exp_setTransparentColor", (char*)"procedure SmartSetTransparentColor(color: integer);",
    (char*)"exp_setDebug", (char*)"procedure SmartSetDebug(enabled: boolean);",
    (char*)"exp_setGraphics", (char*)"procedure SmartSetGraphics(enabled: boolean);",
    (char*)"exp_setEnabled", (char*)"procedure SmartSetEnabled(enabled: boolean);",
    (char*)"exp_isActive", (char*)"function SmartActive: boolean;",
    (char*)"exp_isBlocking", (char*)"function SmartEnabled: boolean;",
    (char*)"exp_getMousePos", (char*)"procedure SmartGetMousePos(var x, y: integer);",
    (char*)"exp_holdMouse", (char*)"procedure SmartHoldMouse(x, y: integer; left: boolean);",
    (char*)"exp_releaseMouse", (char*)"procedure SmartReleaseMouse(x, y: integer; left: boolean);",
    (char*)"exp_holdMousePlus", (char*)"procedure SmartHoldMousePlus(x, y, button: integer);",
    (char*)"exp_releaseMousePlus", (char*)"procedure SmartReleaseMousePlus(x, y, button: integer);",
    (char*)"exp_moveMouse", (char*)"procedure SmartMoveMouse(x, y: integer);",
    (char*)"exp_windMouse", (char*)"procedure SmartWindMouse(x, y: integer);",
    (char*)"exp_clickMouse", (char*)"procedure SmartClickMouse(x, y: integer; left: boolean);",
    (char*)"exp_clickMousePlus", (char*)"procedure SmartClickMousePlus(x, y, button: integer);",
    (char*)"exp_isMouseButtonHeld", (char*)"function SmartIsMouseButtonHeld(button: integer): boolean;",
    (char*)"exp_scrollMouse", (char*)"procedure SmartScrollMouse(x, y, lines: integer);",
    (char*)"exp_sendKeys", (char*)"procedure SmartSendKeys(Text: String; keywait, keymodwait: integer);",
    (char*)"exp_holdKey", (char*)"procedure SmartHoldKey(Code: Integer);",
    (char*)"exp_releaseKey", (char*)"procedure SmartReleaseKey(Code: Integer);",
    (char*)"exp_isKeyDown", (char*)"function SmartIsKeyDown(Code: Integer): Boolean;",
    (char*)"exp_setCapture", (char*)"procedure SmartSetCapture(enabled: boolean);",
    (char*)"exp_setNativeBtn", (char*)"procedure SmartSetNativeButton(plugid,btnid: integer; state: boolean);",
    (char*)"exp_getFieldObject", (char*)"function SmartGetFieldObject(objref: "PTR"; path: string): "PTR";",
    (char*)"exp_isPathValid", (char*)"function SmartIsPathValid(objref: "PTR"; path: string): boolean;",
    (char*)"exp_getFieldBoolean", (char*)"function SmartGetFieldBoolean(objref: "PTR"; path: string): boolean;",
    (char*)"exp_getFieldLongH", (char*)"function SmartGetFieldLongH(objref: "PTR"; path: string): integer;",
    (char*)"exp_getFieldLongL", (char*)"function SmartGetFieldLongL(objref: "PTR"; path: string): integer;",
    (char*)"exp_getFieldInt", (char*)"function SmartGetFieldInt(objref: "PTR"; path: string): integer;",
    (char*)"exp_getFieldShort", (char*)"function SmartGetFieldShort(objref: "PTR"; path: string): integer;",
    (char*)"exp_getFieldFloat", (char*)"function SmartGetFieldFloat(objref: "PTR"; path: string): extended;",
    (char*)"exp_getFieldDouble", (char*)"function SmartGetFieldDouble(objref: "PTR"; path: string): extended;",
    (char*)"exp_getFieldByte", (char*)"function SmartGetFieldByte(objref: "PTR"; path: string): integer;",
    (char*)"exp_getFieldArray3DObject", (char*)"function SmartGetFieldArray3DObject(objref: " PTR"; path: string; x, y, z: integer): " PTR";",
    (char*)"exp_getFieldArray3DByte", (char*)"function SmartGetFieldArray3DByte(objref: " PTR"; path: string; x, y, z: integer): integer;",
    (char*)"exp_getFieldArray3DChar", (char*)"function SmartGetFieldArray3DChar(objref: " PTR"; path: string; x, y, z: integer): integer;",
    (char*)"exp_getFieldArray3DShort", (char*)"function SmartGetFieldArray3DShort(objref: " PTR"; path: string; x, y, z: integer): integer;",
    (char*)"exp_getFieldArray3DInt", (char*)"function SmartGetFieldArray3DInt(objref: " PTR"; path: string; x, y, z: integer): integer;",
    (char*)"exp_getFieldArray3DFloat", (char*)"function SmartGetFieldArray3DFloat(objref: " PTR"; path: string; x, y, z: integer): extended;",
    (char*)"exp_getFieldArray3DDouble", (char*)"function SmartGetFieldArray3DDouble(objref: " PTR"; path: string; x, y, z: integer): extended;",
    (char*)"exp_getFieldArray3DBool", (char*)"function SmartGetFieldArray3DBoolean(objref: " PTR"; path: string; x, y, z: integer): boolean;",
    (char*)"exp_getFieldArray3DLongH", (char*)"function SmartGetFieldArray3DLongH(objref: " PTR"; path: string; x, y, z: integer): integer;",
    (char*)"exp_getFieldArray3DLongL", (char*)"function SmartGetFieldArray3DLongL(objref: " PTR"; path: string; x, y, z: integer): integer;",
    (char*)"exp_getFieldArray2DObject", (char*)"function SmartGetFieldArray2DObject(objref: " PTR"; path: string; x, y: integer): " PTR";",
    (char*)"exp_getFieldArray2DInt", (char*)"function SmartGetFieldArray2DInt(objref: " PTR"; path: string; x, y: integer): integer;",
    (char*)"exp_getFieldArray2DDouble", (char*)"function SmartGetFieldArray2DDouble(objref: " PTR"; path: string; x, y: integer): extended;",
    (char*)"exp_getFieldArray2DFloat", (char*)"function SmartGetFieldArray2DFloat(objref: " PTR"; path: string; x, y: integer): extended;",
    (char*)"exp_getFieldArray2DBool", (char*)"function SmartGetFieldArray2DBoolean(objref: " PTR"; path: string; x, y: integer): boolean;",
    (char*)"exp_getFieldArray2DLongH", (char*)"function SmartGetFieldArray2DLongH(objref: " PTR"; path: string; x, y: integer): integer;",
    (char*)"exp_getFieldArray2DLongL", (char*)"function SmartGetFieldArray2DLongL(objref: " PTR"; path: string; x, y: integer): integer;",
    (char*)"exp_getFieldArray2DByte", (char*)"function SmartGetFieldArray2DByte(objref: " PTR"; path: string; x, y: integer): integer;",
    (char*)"exp_getFieldArray2DChar", (char*)"function SmartGetFieldArray2DChar(objref: " PTR"; path: string; x, y: integer): integer;",
    (char*)"exp_getFieldArray2DShort", (char*)"function SmartGetFieldArray2DShort(objref: " PTR"; path: string; x, y: integer): integer;",
    (char*)"exp_getFieldArraySize", (char*)"function SmartGetFieldArraySize(objref: " PTR"; path: string; dim: integer): integer;",
    (char*)"exp_getFieldArrayObject", (char*)"function SmartGetFieldArrayObject(objref: " PTR"; path: string; index: integer): " PTR";",
    (char*)"exp_getFieldArrayInt", (char*)"function SmartGetFieldArrayInt(objref: " PTR"; path: string; index: integer): integer;",
    (char*)"exp_getFieldArrayFloat", (char*)"function SmartGetFieldArrayFloat(objref: " PTR"; path: string; index: integer): extended;",
    (char*)"exp_getFieldArrayDouble", (char*)"function SmartGetFieldArrayDouble(objref: " PTR"; path: string; index: integer): extended;",
    (char*)"exp_getFieldArrayBool", (char*)"function SmartGetFieldArrayBool(objref: " PTR"; path: string; index: integer): boolean;",
    (char*)"exp_getFieldArrayLongH", (char*)"function SmartGetFieldArrayLongH(objref: " PTR"; path: string; index: integer): integer;",
    (char*)"exp_getFieldArrayLongL", (char*)"function SmartGetFieldArrayLongL(objref: " PTR"; path: string; index: integer): integer;",
    (char*)"exp_getFieldArrayByte", (char*)"function SmartGetFieldArrayByte(objref: " PTR"; path: string; index: integer): integer;",
    (char*)"exp_getFieldArrayShort", (char*)"function SmartGetFieldArrayShort(objref: " PTR"; path: string; index: integer): integer;",
    (char*)"exp_getFieldArrayChar", (char*)"function SmartGetFieldArrayChar(objref: " PTR"; path: string; index: integer): integer;",
    (char*)"exp_freeObject", (char*)"procedure SmartFreeObject(obj: "PTR");",
    (char*)"exp_stringFromString", (char*)"function SmartStringFromString(jstr: "PTR"; str: String): integer;",
    (char*)"exp_stringFromBytes", (char*)"function SmartStringFromBytes(bytes: "PTR"; str: String): integer;",
    (char*)"exp_stringFromChars", (char*)"function SmartStringFromChars(chars: "PTR"; str: String): integer;",
    (char*)"exp_isEqual", (char*)"function SmartIsEqual(obja, objb: "PTR"): boolean;",
    (char*)"exp_isNull", (char*)"function SmartIsNull(obj: "PTR"): boolean;",
};

#endif	/* _LOCAL_H */

