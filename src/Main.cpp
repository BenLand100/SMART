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

//Handles initial loading of the plugin / exporting of functions

#include "Main.h"
#include "Smart.h"
#include <cstring>
#include <iostream>

using namespace std;

//Export list for the SCAR/Simba plugin loaders
static char* exports[] = {
    (char*)"std_setup", (char*)"procedure SmartSetup(root, params: String; width, height: Integer; initseq: String);",
    (char*)"std_setJVMPath", (char*)"procedure SmartSetJVMPath(path: String);",
#ifdef WINDOWS
    (char*)"std_getImageHDC", (char*)"function SmartGetDC: integer;",
    (char*)"std_getDebugHDC", (char*)"function SmartGetDebugDC: integer;",
#endif
    (char*)"std_getImageArray", (char*)"function SmartImageArray(): integer;",
    (char*)"std_getDebugArray", (char*)"function SmartDebugArray(): integer;",
    (char*)"std_getRefresh", (char*)"function SmartGetRefresh: integer;",
    (char*)"std_setRefresh", (char*)"procedure SmartSetRefresh(x: integer);",
    (char*)"std_setTransparentColor", (char*)"procedure SmartSetTransparentColor(color: integer);",
    (char*)"std_setDebug", (char*)"procedure SmartSetDebug(enabled: boolean);",
    (char*)"std_setGraphics", (char*)"procedure SmartSetGraphics(enabled: boolean);",
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
    (char*)"std_sendKeys", (char*)"procedure SmartSendKeys(Text: String);",
    (char*)"std_holdKey", (char*)"procedure SmartHoldKey(Code: Integer);",
    (char*)"std_releaseKey", (char*)"procedure SmartReleaseKey(Code: Integer);",
    (char*)"std_isKeyDown", (char*)"function SmartIsKeyDown(Code: Integer): Boolean;",
    (char*)"std_getColor", (char*)"function SmartGetColor(x, y: integer): integer;",
    (char*)"std_findColor", (char*)"function SmartFindColor(var x, y: integer; color, sx, sy, ex, ey: integer): boolean;",
    (char*)"std_findColorTol", (char*)"function SmartFindColorTolerance(var x, y: integer; color, sx, sy, ex, ey, tol: integer): boolean;",
    (char*)"std_findColorSpiral", (char*)"function SmartFindColorSpiral(var x, y: integer; color, sx, sy, ex, ey: integer): boolean;",
    (char*)"std_findColorSpiralTol", (char*)"function SmartFindColorSpiralTolerance(var x, y: integer; color, sx, sy, ex, ey, tol: integer): boolean;",
    (char*)"std_stringFromBytes", (char*)"function SmartStringFromBytes(bytes: integer; str: String): integer;",
    (char*)"std_stringFromChars", (char*)"function SmartStringFromChars(chars: integer; str: String): integer;",
    (char*)"std_stringFromString", (char*)"function SmartStringFromString(jstr: integer; str: String): integer;",
    (char*)"std_invokeIntMethod", (char*)"function SmartInvokeIntMethod(obj: integer; clazz, meth: String; a, b: integer): integer;",
    (char*)"std_getFieldObject", (char*)"function SmartGetFieldObject(parent: integer; path: String): integer;",
    (char*)"std_isPathValid", (char*)"function SmartIsPathValid(parent: integer; path: String): boolean;",
    (char*)"std_getFieldInt", (char*)"function SmartGetFieldInt(parent: integer; path: String): integer;",
    (char*)"std_getFieldShort", (char*)"function SmartGetFieldShort(parent: integer; path: String): integer;",
    (char*)"std_getFieldByte", (char*)"function SmartGetFieldByte(parent: integer; path: String): integer;",
    (char*)"std_getFieldFloat", (char*)"function SmartGetFieldFloat(parent: integer; path: String): extended;",
    (char*)"std_getFieldDouble", (char*)"function SmartGetFieldDouble(parent: integer; path: String): extended;",
    (char*)"std_getFieldBool", (char*)"function SmartGetFieldBoolean(parent: integer; path: String): boolean;",
    (char*)"std_getFieldLongH", (char*)"function SmartGetFieldLongH(parent: integer; path: String): integer;",
    (char*)"std_getFieldLongL", (char*)"function SmartGetFieldLongL(parent: integer; path: String): integer;",
    (char*)"std_getFieldArrayInt", (char*)"function SmartGetFieldArrayInt(parent: integer; path: String; index: integer): integer;",
    (char*)"std_getFieldArrayFloat", (char*)"function SmartGetFieldArrayFloat(parent: integer; path: String; index: integer): extended;",
    (char*)"std_getFieldArrayDouble", (char*)"function SmartGetFieldArrayDouble(parent: integer; path: String; index: integer): extended;",
    (char*)"std_getFieldArrayLongH", (char*)"function SmartGetFieldArrayLongH(parent: integer; path: String; index: integer): integer;",
    (char*)"std_getFieldArrayLongL", (char*)"function SmartGetFieldArrayLongL(parent: integer; path: String; index: integer): integer;",
    (char*)"std_getFieldArrayBool", (char*)"function SmartGetFieldArrayBoolean(parent: integer; path: String; index: integer): boolean;",
    (char*)"std_getFieldArrayByte", (char*)"function SmartGetFieldArrayByte(parent: integer; path: String; index: integer): integer;",
    (char*)"std_getFieldArrayShort", (char*)"function SmartGetFieldArrayShort(parent: integer; path: String; index: integer): integer;",
    (char*)"std_getFieldArrayChar", (char*)"function SmartGetFieldArrayChar(parent: integer; path: String; index: integer): integer;",
    (char*)"std_getFieldArray3DInt", (char*)"function SmartGetFieldArray3DInt(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"std_getFieldArray3DFloat", (char*)"function SmartGetFieldArray3DFloat(parent: integer; path: String; x ,y, z: integer): extended;",
    (char*)"std_getFieldArray3DDouble", (char*)"function SmartGetFieldArray3DDouble(parent: integer; path: String; x ,y, z: integer): extended;",
    (char*)"std_getFieldArray3DBool", (char*)"function SmartGetFieldArray3DBoolean(parent: integer; path: String; x ,y, z: integer): boolean;",
    (char*)"std_getFieldArray3DLongH", (char*)"function SmartGetFieldArray3DLongH(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"std_getFieldArray3DLongL", (char*)"function SmartGetFieldArray3DLongL(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"std_getFieldArray3DByte", (char*)"function SmartGetFieldArray3DByte(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"std_getFieldArray3DShort", (char*)"function SmartGetFieldArray3DShort(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"std_getFieldArray3DChar", (char*)"function SmartGetFieldArray3DChar(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"std_getFieldArray3DObject", (char*)"function SmartGetFieldArray3DObject(parent: integer; path: String; x ,y, z: integer): integer;",
    (char*)"std_getFieldArray2DInt", (char*)"function SmartGetFieldArray2DInt(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"std_getFieldArray2DFloat", (char*)"function SmartGetFieldArray2DFloat(parent: integer; path: String; x ,y: integer): extended;",
    (char*)"std_getFieldArray2DDouble", (char*)"function SmartGetFieldArray2DDouble(parent: integer; path: String; x ,y: integer): extended;",
    (char*)"std_getFieldArray2DBool", (char*)"function SmartGetFieldArray2DBoolean(parent: integer; path: String; x ,yz: integer): boolean;",
    (char*)"std_getFieldArray2DLongH", (char*)"function SmartGetFieldArray2DLongH(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"std_getFieldArray2DLongL", (char*)"function SmartGetFieldArray2DLongL(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"std_getFieldArray2DByte", (char*)"function SmartGetFieldArray2DByte(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"std_getFieldArray2DShort", (char*)"function SmartGetFieldArray2DShort(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"std_getFieldArray2DChar", (char*)"function SmartGetFieldArray2DChar(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"std_getFieldArray2DObject", (char*)"function SmartGetFieldArray2DObject(parent: integer; path: String; x ,y: integer): integer;",
    (char*)"std_getFieldArrayObject", (char*)"function SmartGetFieldArrayObject(parent: integer; path: String; index: integer): integer;",
    (char*)"std_getFieldArraySize", (char*)"function SmartGetFieldArraySize(parent: integer; path: String; dim: integer): integer;",
    (char*)"std_isEqual", (char*)"function SmartIsEqual(obja,objb: integer): boolean;",
    (char*)"std_isNull", (char*)"function SmartIsNull(obj: integer): boolean;",
    (char*)"std_freeObject", (char*)"procedure SmartFreeObject(obj: integer);"
};

#ifdef LINUX

#define NumExports 77

//STDCALL - SCAR/Simba
long GetFunctionCount() {
    return NumExports;
}

//STDCALL - SCAR/Simba
long GetFunctionInfo(int index, void*& address, char*& def) {
    if (index < NumExports) {
        address = dlsym(RTLD_DEFAULT,exports[index*2]);
        strcpy(def, exports[index*2+1]);
        return index;
    }
    return -1;
}

//Linux lib entrypoint
void load(void) {
    cout << "Running SMART Constructor\n";
    internalConstructor();
    cout << "SMART Initialized!\n";
}

//Linux lib exitpoint
void unload(void) {
    cout << "Running SMART Destructor\n";
    internalDestructor();
    cout << "SMART Finalized!\n";
}

#endif

#ifdef WINDOWS

#define NumExports 79

HMODULE dllinst;

//STDCALL - SCAR/Simba
long GetFunctionCount() {
    return NumExports;
}

//STDCALL - SCAR/Simba
long GetFunctionInfo(int index, void*& address, char*& def) {
    if (index < NumExports) {
        address = (void*)GetProcAddress(dllinst, exports[index*2]);
        strcpy(def, exports[index*2+1]);
        return index;
    }
    return -1;
}

//Windows DLL entrypoint/exitpoint
bool DllMain(HINSTANCE instance, int reason, void* checks) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            dllinst = instance;
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
