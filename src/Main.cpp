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
    (char*)"setup", (char*)"procedure SmartSetup(root, params: String; width, height: Integer; initseq: String);",
    (char*)"hardReset", (char*)"procedure SmartHardReset();",
    (char*)"setJVMPath", (char*)"procedure SmartSetJVMPath(path: String);",
    (char*)"setMaxJVMMem", (char*)"procedure SmartSetMaxJVMMem(mb: integer);",
    (char*)"setUserAgent", (char*)"procedure SmartSetUserAgent(useragent: String);",
#ifdef WINDOWS
    (char*)"getImageHDC", (char*)"function SmartGetDC: integer;",
    (char*)"getDebugHDC", (char*)"function SmartGetDebugDC: integer;",
#endif
    (char*)"exp_getImageArray", (char*)"function SmartImageArray(): int" 
    #if __SIZEOF_POINTER__ == 4
        "eger;",
    #else
        "64;",
    #endif
    (char*)"exp_getDebugArray", (char*)"function SmartDebugArray(): int" 
    #if __SIZEOF_POINTER__ == 4
        "eger;",
    #else
        "64;",
    #endif
    (char*)"getRefresh", (char*)"function SmartGetRefresh: integer;",
    (char*)"setRefresh", (char*)"procedure SmartSetRefresh(x: integer);",
    (char*)"setTransparentColor", (char*)"procedure SmartSetTransparentColor(color: integer);",
    (char*)"setDebug", (char*)"procedure SmartSetDebug(enabled: boolean);",
    (char*)"setGraphics", (char*)"procedure SmartSetGraphics(enabled: boolean);",
    (char*)"setEnabled", (char*)"procedure SmartSetEnabled(enabled: boolean);",
    (char*)"isActive", (char*)"function SmartActive: boolean;",
    (char*)"isBlocking", (char*)"function SmartEnabled: boolean;",
    (char*)"getMousePos", (char*)"procedure SmartGetMousePos(var x, y: integer);",
    (char*)"holdMouse", (char*)"procedure SmartHoldMouse(x, y: integer; left: boolean);",
    (char*)"releaseMouse", (char*)"procedure SmartReleaseMouse(x, y: integer; left: boolean);",
    (char*)"holdMousePlus", (char*)"procedure SmartHoldMousePlus(x, y, button: integer);",
    (char*)"releaseMousePlus", (char*)"procedure SmartReleaseMousePlus(x, y, button: integer);",
    (char*)"moveMouse", (char*)"procedure SmartMoveMouse(x, y: integer);",
    (char*)"windMouse", (char*)"procedure SmartWindMouse(x, y: integer);",
    (char*)"clickMouse", (char*)"procedure SmartClickMouse(x, y: integer; left: boolean);",
    (char*)"clickMousePlus", (char*)"procedure SmartClickMousePlus(x, y, button: integer);",
    (char*)"isMouseButtonHeld", (char*)"function SmartIsMouseButtonHeld(button: integer): boolean;",
    (char*)"sendKeys", (char*)"procedure SmartSendKeys(Text: String; keywait, keymodwait: integer);",
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
    (char*)"invokeIntMethod", (char*)"function SmartInvokeIntMethod(obj: integer; clazz, meth: String; a, b: integer): integer;",
    (char*)"getFieldObject", (char*)"function SmartGetFieldObject(parent: integer; path: String): integer;",
    (char*)"isPathValid", (char*)"function SmartIsPathValid(parent: integer; path: String): boolean;",
    (char*)"getFieldInt", (char*)"function SmartGetFieldInt(parent: integer; path: String): integer;",
    (char*)"getFieldShort", (char*)"function SmartGetFieldShort(parent: integer; path: String): integer;",
    (char*)"getFieldByte", (char*)"function SmartGetFieldByte(parent: integer; path: String): integer;",
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
    (char*)"getFieldArraySize", (char*)"function SmartGetFieldArraySize(parent: integer; path: String; dim: integer): integer;",
    (char*)"isEqual", (char*)"function SmartIsEqual(obja,objb: integer): boolean;",
    (char*)"isNull", (char*)"function SmartIsNull(obj: integer): boolean;",
    (char*)"freeObject", (char*)"procedure SmartFreeObject(obj: integer);"
};

#ifdef LINUX

#define NumExports 82

int GetPluginABIVersion() {
	return 2; //cdecl everything
}

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

#define NumExports 84

HMODULE dllinst;

int GetPluginABIVersion() {
	return 2; //cdecl everything
}

int GetFunctionCount() {
	cout << "numexports\n";
    return NumExports;
}

int GetFunctionInfo(int index, void*& address, char*& def) {
	cout << "funcinfo " << index << "\n";
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
