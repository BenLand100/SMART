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

#include "Main.h"
#include "Smart.h"
#include <cstring>
#include <iostream>

using namespace std;

#ifdef LINUX

#include <dlfcn.h>

#define NumExports 68

static char* exports[] = {
    (char*)"setup", (char*)"procedure SmartSetup(root, params: String; width, height: Integer);",
    (char*)"getImageArray", (char*)"function SmartImageArray(): Pointer;",
    (char*)"getDebugArray", (char*)"function SmartDebugArray(): Pointer;",
    (char*)"getRefresh", (char*)"function SmartGetRefresh: integer;",
    (char*)"setRefresh", (char*)"procedure SmartSetRefresh(x: integer);",
    (char*)"setDebugColor", (char*)"procedure SmartSetTrasparentColor(color: integer);",
    (char*)"setDebug", (char*)"procedure SmartSetDebug(enabled: boolean);",
    (char*)"setGraphics", (char*)"procedure SmartSetGraphics(enabled: boolean);",
    (char*)"isActive", (char*)"function SmartActive: boolean;",
    (char*)"isBlocking", (char*)"function SmartEnabled: boolean;",
    (char*)"getMousePos", (char*)"procedure SmartGetMousePos(var x, y: integer);",
    (char*)"holdMouse", (char*)"procedure SmartHoldMouse(x, y: integer; left: boolean);",
    (char*)"releaseMouse", (char*)"procedure SmartReleaseMouse(x, y: integer; left: boolean);",
    (char*)"moveMouse", (char*)"procedure SmartMoveMouse(x, y: Integer);",
    (char*)"windMouse", (char*)"procedure SmartWindMouse(x, y: Integer);",
    (char*)"dragMouse", (char*)"procedure SmartDragMouse(x, y: Integer);",
    (char*)"clickMouse", (char*)"procedure SmartClickMouse(x, y: Integer; left: Boolean);",
    (char*)"sendKeys", (char*)"procedure SmartSendKeys(Text: String);",
    (char*)"holdKey", (char*)"procedure SmartHoldKey(Code: Integer);",
    (char*)"releaseKey", (char*)"procedure SmartReleaseKey(Code: Integer);",
    (char*)"isKeyDown", (char*)"function SmartIsKeyDown(Code: Integer): Boolean;",
    (char*)"getColor", (char*)"function SmartGetColor(x, y: integer): integer;",
    (char*)"findColor", (char*)"function SmartFindColor(var x, y: integer; color, sx, sy, ex, ey: integer): boolean;",
    (char*)"findColorTol", (char*)"function SmartFindColorTolerance(var x, y: integer; color, sx, sy, ex, ey, tol: integer): boolean;",
    (char*)"findColorSpiral", (char*)"function SmartFindColorSpiral(var x, y: integer; color, sx, sy, ex, ey: integer): boolean;",
    (char*)"findColorSpiralTol", (char*)"function SmartFindColorSpiralTolerance(var x, y: integer; color, sx, sy, ex, ey, tol: integer): boolean;",
    (char*)"stringFromBytes", (char*)"function SmartStringFromBytes(bytes: integer; str: String): integer;",
    (char*)"stringFromChars", (char*)"function SmartStringFromChars(chars: integer; str: String): integer;",
    (char*)"stringFromString", (char*)"function SmartStringFromString(jstr: integer; str: String): integer;",
    (char*)"invokeIntMethod", (char*)"function SmartInvokeIntMethod(obj: integer; clazz, meth: string; a, b: integer): integer;",
    (char*)"freeObject", (char*)"procedure SmartFreeObject(obj: integer);",
    (char*)"getFieldObject", (char*)"function SmartGetFieldObject(parent: integer; path: String): integer;",
    (char*)"getFieldInt", (char*)"function SmartGetFieldInt(parent: integer; path: String): integer;",
    (char*)"getFieldFloat", (char*)"function SmartGetFieldFloat(parent: integer; path: String): extended;",
    (char*)"getFieldDouble", (char*)"function SmartGetFieldDouble(parent: integer; path: String): extended;",
    (char*)"getFieldBool", (char*)"function SmartGetFieldBoolean(parent: integer; path: String): boolean;",
    (char*)"getFieldLongH", (char*)"function SmartGetFieldLongH(parent: integer; path: String): integer;",
    (char*)"getFieldLongL", (char*)"function SmartGetFieldLongL(parent: integer; path: String): integer;",
    (char*)"getFieldArrayInt", (char*)"function SmartGetFieldArrayInt(parent: integer; path: String; index: integer): integer;",
    (char*)"getFieldArrayFloat", (char*)"function SmartGetFieldArrayFloat(parent: integer; path: String; index: integer): extended;",
    (char*)"getFieldArrayDouble", (char*)"function SmartGetFieldArrayDouble(parent: integer; path: String; index: integer): extended;",
    (char*)"getFieldArrayLongH", (char*)"function SmartGetFieldArrayLongH(parent: integer; path: String; index: integer): integer;",
    (char*)"getFieldArrayLongL", (char*)"function SmartGetFieldArrayLongL(parent: integer; path: String; index: integer): integer;",
    (char*)"getFieldArrayBool", (char*)"function SmartGetFieldArrayBoolean(parent: integer; path: String; index: integer): boolean;",
    (char*)"getFieldArrayByte", (char*)"function SmartGetFieldArrayByte(parent: integer; path: String; index: integer): integer;",
    (char*)"getFieldArrayShort", (char*)"function SmartGetFieldArrayShort(parent: integer; path: String; index: integer): integer;",
    (char*)"getFieldArrayChar", (char*)"function SmartGetFieldArrayChar(parent: integer; path: String; index: integer): integer;",
    (char*)"getFieldArray3DInt", (char*)"function SmartGetFieldArray3DInt(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"getFieldArray3DFloat", (char*)"function SmartGetFieldArray3DFloat(parent: integer; path: String; x ,y, z: integer): extended;",
    (char*)"getFieldArray3DDouble", (char*)"function SmartGetFieldArray3DDouble(parent: integer; path: String; x ,y, z: integer): extended;",
    (char*)"getFieldArray3DBool", (char*)"function SmartGetFieldArray3DBoolean(parent: integer; path: String; x ,y, z: integer): boolean;",
    (char*)"getFieldArray3DLongH", (char*)"function SmartGetFieldArray3DLongH(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"getFieldArray3DLongL", (char*)"function SmartGetFieldArray3DLongL(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"getFieldArray3DByte", (char*)"function SmartGetFieldArray3DByte(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"getFieldArray3DShort", (char*)"function SmartGetFieldArray3DShort(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"getFieldArray3DChar", (char*)"function SmartGetFieldArray3DChar(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"getFieldArray3DObject", (char*)"function SmartGetFieldArray3DObject(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"getFieldArray2DInt", (char*)"function SmartGetFieldArray2DInt(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"getFieldArray2DFloat", (char*)"function SmartGetFieldArray2DFloat(parent: integer; path: String; x ,y: integer): extended;",
    (char*)"getFieldArray2DDouble", (char*)"function SmartGetFieldArray2DDouble(parent: integer; path: String; x ,y: integer): extended;",
    (char*)"getFieldArray2DBool", (char*)"function SmartGetFieldArray2DBoolean(parent: integer; path: String; x ,yz: integer): boolean;",
    (char*)"getFieldArray2DLongH", (char*)"function SmartGetFieldArray2DLongH(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"getFieldArray2DLongL", (char*)"function SmartGetFieldArray2DLongL(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"getFieldArray2DByte", (char*)"function SmartGetFieldArray2DByte(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"getFieldArray2DShort", (char*)"function SmartGetFieldArray2DShort(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"getFieldArray2DChar", (char*)"function SmartGetFieldArray2DChar(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"getFieldArray2DObject", (char*)"function SmartGetFieldArray2DObject(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"getFieldArrayObject", (char*)"function SmartGetFieldArrayObject(parent: integer; path: String; index: integer): integer;",
    (char*)"getFieldArraySize", (char*)"function SmartGetFieldArraySize(parent: integer; path: String): integer;"
};


long GetFunctionCount() {
    return NumExports;
}

long GetFunctionInfo(int index, void*& address, char*& def) {
    if (index < NumExports) {
        address = dlsym(RTLD_DEFAULT,exports[index*2]);
        strcpy(def, exports[index*2+1]);
        return index;
    }
    return -1;
}


void load(void) {
    cout << "Running SMART Constructor\n";
    internalConstructor();
    cout << "SMART Initialized!\n";
}


void unload(void) {
    cout << "Running SMART Destructor\n";
    internalDestructor();
    cout << "SMART Finalized!\n";
}

#endif

#ifdef WINDOWS

#include <windows.h>

#define NumExports 70

static char* exports[] = {
    (char*)"setup@16", (char*)"procedure SmartSetup(root, params: String; width, height: Integer);",
    (char*)"getImageHDC@0", (char*)"function SmartGetDC: integer;",
    (char*)"getDebugHDC@0", (char*)"function SmartGetDebugDC: integer;",
    (char*)"getImageArray@0", (char*)"function SmartImageArray(): integer;",
    (char*)"getDebugArray@0", (char*)"function SmartDebugArray(): integer;",
    (char*)"getRefresh@0", (char*)"function SmartGetRefresh: integer;",
    (char*)"setRefresh@4", (char*)"procedure SmartSetRefresh(x: integer);",
    (char*)"setDebugColor@4", (char*)"procedure SmartSetTrasparentColor(color: integer);",
    (char*)"setDebug@4", (char*)"procedure SmartSetDebug(enabled: boolean);",
    (char*)"setGraphics@4", (char*)"procedure SmartSetGraphics(enabled: boolean);",
    (char*)"isBlocking@0", (char*)"function SmartEnabled: boolean;",
    (char*)"isActive@0", (char*)"function SmartActive: boolean;",
    (char*)"getMousePos@8", (char*)"procedure SmartGetMousePos(var x, y: integer);",
    (char*)"holdMouse@12", (char*)"procedure SmartHoldMouse(x, y: integer; left: boolean);",
    (char*)"releaseMouse@12", (char*)"procedure SmartReleaseMouse(x, y: integer; left: boolean);",
    (char*)"moveMouse@8", (char*)"procedure SmartMoveMouse(x, y: Integer);",
    (char*)"windMouse@8", (char*)"procedure SmartWindMouse(x, y: Integer);",
    (char*)"dragMouse@8", (char*)"procedure SmartDragMouse(x, y: Integer);",
    (char*)"clickMouse@12", (char*)"procedure SmartClickMouse(x, y: Integer; left: Boolean);",
    (char*)"sendKeys@4", (char*)"procedure SmartSendKeys(Text: String);",
    (char*)"holdKey@4", (char*)"procedure SmartHoldKey(Code: Integer);",
    (char*)"isKeyDown@4", (char*)"function SmartIsKeyDown(Code: Integer): Boolean;",
    (char*)"releaseKey@4", (char*)"procedure SmartReleaseKey(Code: Integer);",
    (char*)"getColor@8", (char*)"function SmartGetColor(x, y: integer): integer;",
    (char*)"findColor@28", (char*)"function SmartFindColor(var x, y: integer; color, sx, sy, ex, ey: integer): boolean;",
    (char*)"findColorTol@32", (char*)"function SmartFindColorTolerance(var x, y: integer; color, sx, sy, ex, ey, tol: integer): boolean;",
    (char*)"findColorSpiral@28", (char*)"function SmartFindColorSpiral(var x, y: integer; color, sx, sy, ex, ey: integer): boolean;",
    (char*)"findColorSpiralTol@32", (char*)"function SmartFindColorSpiralTolerance(var x, y: integer; color, sx, sy, ex, ey, tol: integer): boolean;",
    (char*)"stringFromBytes@8", (char*)"function SmartStringFromBytes(bytes: integer; str: String): integer;",
    (char*)"stringFromChars@8", (char*)"function SmartStringFromChars(chars: integer; str: String): integer;",
    (char*)"stringFromString@8", (char*)"function SmartStringFromString(str: integer; str: String): integer;",
    (char*)"invokeIntMethod@20", (char*)"function SmartInvokeIntMethod(obj: integer; clazz, meth: string; a, b: integer): integer;",
    (char*)"freeObject@4", (char*)"procedure SmartFreeObject(object: integer);",
    (char*)"getFieldObject@8", (char*)"function SmartGetFieldObject(parent: integer; path: String): integer;",
    (char*)"getFieldInt@8", (char*)"function SmartGetFieldInt(parent: integer; path: String): integer;",
    (char*)"getFieldFloat@8", (char*)"function SmartGetFieldFloat(parent: integer; path: String): extended;",
    (char*)"getFieldDouble@8", (char*)"function SmartGetFieldDouble(parent: integer; path: String): extended;",
    (char*)"getFieldBool@8", (char*)"function SmartGetFieldBoolean(parent: integer; path: String): boolean;",
    (char*)"getFieldLongH@8", (char*)"function SmartGetFieldLongH(parent: integer; path: String): integer;",
    (char*)"getFieldLongL@8", (char*)"function SmartGetFieldLongL(parent: integer; path: String): integer;",
    (char*)"getFieldArrayInt@12", (char*)"function SmartGetFieldArrayInt(parent: integer; path: String; index: integer): integer;",
    (char*)"getFieldArrayFloat@12", (char*)"function SmartGetFieldArrayFloat(parent: integer; path: String; index: integer): extended;",
    (char*)"getFieldArrayDouble@12", (char*)"function SmartGetFieldArrayDouble(parent: integer; path: String; index: integer): extended;",
    (char*)"getFieldArrayLongH@12", (char*)"function SmartGetFieldArrayLongH(parent: integer; path: String; index: integer): integer;",
    (char*)"getFieldArrayLongL@12", (char*)"function SmartGetFieldArrayLongL(parent: integer; path: String; index: integer): integer;",
    (char*)"getFieldArrayBool@12", (char*)"function SmartGetFieldArrayBoolean(parent: integer; path: String; index: integer): boolean;",
    (char*)"getFieldArrayByte@12", (char*)"function SmartGetFieldArrayByte(parent: integer; path: String; index: integer): integer;",
    (char*)"getFieldArrayShort@12", (char*)"function SmartGetFieldArrayShort(parent: integer; path: String; index: integer): integer;",
    (char*)"getFieldArrayChar@12", (char*)"function SmartGetFieldArrayChar(parent: integer; path: String; index: integer): integer;",
    (char*)"getFieldArray3DInt@20", (char*)"function SmartGetFieldArray3DInt(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"getFieldArray3DFloat@20", (char*)"function SmartGetFieldArray3DFloat(parent: integer; path: String; x ,y, z: integer): extended;",
    (char*)"getFieldArray3DDouble@20", (char*)"function SmartGetFieldArray3DDouble(parent: integer; path: String; x ,y, z: integer): extended;",
    (char*)"getFieldArray3DBool@20", (char*)"function SmartGetFieldArray3DBoolean(parent: integer; path: String; x ,y, z: integer): boolean;",
    (char*)"getFieldArray3DLongH@20", (char*)"function SmartGetFieldArray3DLongH(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"getFieldArray3DLongL@20", (char*)"function SmartGetFieldArray3DLongL(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"getFieldArray3DByte@20", (char*)"function SmartGetFieldArray3DByte(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"getFieldArray3DShort@20", (char*)"function SmartGetFieldArray3DShort(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"getFieldArray3DChar@20", (char*)"function SmartGetFieldArray3DChar(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"getFieldArray3DObject@20", (char*)"function SmartGetFieldArray3DObject(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"getFieldArray2DInt@16", (char*)"function SmartGetFieldArray2DInt(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"getFieldArray2DFloat@16", (char*)"function SmartGetFieldArray2DFloat(parent: integer; path: String; x ,y: integer): extended;",
    (char*)"getFieldArray2DDouble@16", (char*)"function SmartGetFieldArray2DDouble(parent: integer; path: String; x ,y: integer): extended;",
    (char*)"getFieldArray2DBool@16", (char*)"function SmartGetFieldArray2DBoolean(parent: integer; path: String; x ,yz: integer): boolean;",
    (char*)"getFieldArray2DLongH@16", (char*)"function SmartGetFieldArray2DLongH(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"getFieldArray2DLongL@16", (char*)"function SmartGetFieldArray2DLongL(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"getFieldArray2DByte@16", (char*)"function SmartGetFieldArray2DByte(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"getFieldArray2DShort@16", (char*)"function SmartGetFieldArray2DShort(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"getFieldArray2DChar@16", (char*)"function SmartGetFieldArray2DChar(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"getFieldArray2DObject@16", (char*)"function SmartGetFieldArray2DObject(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"getFieldArrayObject@12", (char*)"function SmartGetFieldArrayObject(parent: integer; path: String; index: integer): integer;",
    (char*)"getFieldArraySize@8", (char*)"function SmartGetFieldArraySize(parent: integer; path: String): integer;"
};

HMODULE dllinst;
void* functionInfoAddr;
void* functionCountAddr;

long functionCount() {
    return NumExports;
}

void GetFunctionCount() {
    asm ("popl %%ebp \n jmp *%0" : : "a" (functionCountAddr) : );
}

long functionInfo(int index, void*& address, char*& def) {
    if (index < NumExports) {
        address = (void*)GetProcAddress(dllinst, exports[index*2]);
        strcpy(def, exports[index*2+1]);
        return index;
    }
    return -1;
}

void GetFunctionInfo() {
    asm ("popl %%ebp \n jmp *%0" : : "a" (functionInfoAddr) : );
}


bool DllMain(HINSTANCE instance, int reason, void* checks) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            dllinst = instance;
            functionInfoAddr = (void*) GetProcAddress(dllinst,"functionInfo@12");
            functionCountAddr = (void*) GetProcAddress(dllinst,"functionCount@0");
            cout << "Running SMART Constructor\n";
            internalConstructor();
            cout << "SMART Initialized!\n";
            return true;
        case DLL_THREAD_ATTACH:
            return true;
        case DLL_PROCESS_DETACH:
            cout << "Running SMART Destructor\n";
            internalDestructor();
            cout << "SMART Finalized!\n";
            return true;
        case DLL_THREAD_DETACH:
            return true;
    }
    return false;
}

#endif
