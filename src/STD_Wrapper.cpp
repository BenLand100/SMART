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

//This file contains the stdcall wrappers, prefixed with std_ 
//See repsective files for documentation

#include "STD_Wrapper.h"
#include "JVM.h"
#include "Smart.h"
#include "Reflection.h"
#include "Input.h"
#include "Color.h"

//from JVM.cpp

void std_setJVMPath(char* path) {
	setJVMPath(path);
}

void std_setMaxJVMMem(long mem) {
    setMaxJVMMem(mem);
}

//from Smart.cpp

#ifdef WINDOWS

void* std_getImageHDC() {
    return getImageHDC();
}

void* std_getDebugHDC() {
    return getDebugHDC();
}

#endif

void* std_getImageArray() {
    return getImageArray();
}

void std_setTransparentColor(long color) {
    setTransparentColor(color);
}

void std_setDebug(bool on) {
    setDebug(on);
}

void std_setGraphics(bool on) {
    setGraphics(on);
}

void std_setEnabled(bool on) {
    setEnabled(on);
}

void* std_getDebugArray() {
    return getDebugArray();
}

bool std_isActive() {
    return isActive();
}

bool std_isBlocking() {
    return isBlocking();
}

long std_getRefresh() {
    return getRefresh();
}

void std_setRefresh(long x) {
    setRefresh(x);
}

void std_setup(char* root, char* args, long w, long h, char* initseq) {
    setup(root, args, w, h, initseq);
}

void std_hardReset() {
    hardReset();
}

void std_setUserAgent(char* useragent) {
    setUserAgent(useragent);
}

//from Reflection.cpp

int std_stringFromBytes(void* parent, char* path) {
    return stringFromBytes(parent, path);
}

int std_stringFromChars(void* parent, char* path) {
    return stringFromChars(parent, path);
}

int std_stringFromString(void* parent, char* path) {
    return stringFromString(parent, path);
}

int std_invokeIntMethod(void* object, char* classname, char* methname, int a, int b) {
    return invokeIntMethod(object, classname, methname, a, b);
}

void std_freeObject(void* parent) {
    freeObject(parent);
}

void* std_getFieldObject(void* parent, char* path) {
    return getFieldObject(parent, path);
}

long std_getFieldInt(void* parent, char* path) {
    return getFieldInt(parent, path);
}

long std_getFieldShort(void* parent, char* path) {
    return getFieldShort(parent, path);
}

long std_getFieldByte(void* parent, char* path) {
    return getFieldByte(parent, path);
}

bool std_getFieldBool(void* parent, char* path) {
    return getFieldBool(parent, path);
}

long std_getFieldLongH(void* parent, char* path) {
    return getFieldLongH(parent, path);
}

long std_getFieldLongL(void* parent, char* path) {
    return getFieldLongL(parent, path);
}

float std_getFieldFloat(void* parent, char* path) {
    return getFieldFloat(parent, path);
}

float std_getFieldDouble(void* parent, char* path) {
    return getFieldDouble(parent, path);
}

void* std_getFieldArrayObject(void* parent, char* path, int i) {
    return getFieldArrayObject(parent, path, i);
}

long std_getFieldArrayInt(void* parent, char* path, int i) {
    return getFieldArrayInt(parent, path, i);
}

float std_getFieldArrayFloat(void* parent, char* path, int i) {
    return getFieldArrayFloat(parent, path, i);
}

float std_getFieldArrayDouble(void* parent, char* path, int i) {
    return getFieldArrayDouble(parent, path, i);
}

bool std_getFieldArrayBool(void* parent, char* path, int i) {
    return getFieldArrayBool(parent, path, i);
}

long std_getFieldArrayLongH(void* parent, char* path, int i) {
    return getFieldArrayLongH(parent, path, i);
}

long std_getFieldArrayLongL(void* parent, char* path, int i) {
    return getFieldArrayLongL(parent, path, i);
}

long std_getFieldArrayByte(void* parent, char* path, int i) {
    return getFieldArrayByte(parent, path, i);
}

long std_getFieldArrayShort(void* parent, char* path, int i) {
    return getFieldArrayShort(parent, path, i);
}

long std_getFieldArrayChar(void* parent, char* path, int i) {
    return getFieldArrayChar(parent, path, i);
}

void* std_getFieldArray3DObject(void* parent, char* path, int i, int j, int k) {
    return getFieldArray3DObject(parent, path, i, j, k);
}

long std_getFieldArray3DInt(void* parent, char* path, int i, int j, int k) {
    return getFieldArray3DInt(parent, path, i, j, k);
}

float std_getFieldArray3DFloat(void* parent, char* path, int i, int j, int k) {
    return getFieldArray3DFloat(parent, path, i, j, k);
}

float std_getFieldArray3DDouble(void* parent, char* path, int i, int j, int k) {
    return getFieldArray3DDouble(parent, path, i, j, k);
}

bool std_getFieldArray3DBool(void* parent, char* path, int i, int j, int k) {
    return getFieldArray3DBool(parent, path, i, j, k);
}

long std_getFieldArray3DLongH(void* parent, char* path, int i, int j, int k) {
    return getFieldArray3DLongH(parent, path, i, j, k);
}

long std_getFieldArray3DLongL(void* parent, char* path, int i, int j, int k) {
    return getFieldArray3DLongL(parent, path, i, j, k);
}

long std_getFieldArray3DByte(void* parent, char* path, int i, int j, int k) {
    return getFieldArray3DByte(parent, path, i, j, k);
}

long std_getFieldArray3DShort(void* parent, char* path, int i, int j, int k) {
    return getFieldArray3DShort(parent, path, i, j, k);
}

long std_getFieldArray3DChar(void* parent, char* path, int i, int j, int k) {
    return getFieldArray3DChar(parent, path, i, j, k);
}

void* std_getFieldArray2DObject(void* parent, char* path, int i, int j) {
    return getFieldArray2DObject(parent, path, i, j);
}

long std_getFieldArray2DInt(void* parent, char* path, int i, int j) {
    return getFieldArray2DInt(parent, path, i, j);
}

float std_getFieldArray2DFloat(void* parent, char* path, int i, int j) {
    return getFieldArray2DFloat(parent, path, i, j);
}

float std_getFieldArray2DDouble(void* parent, char* path, int i, int j) {
    return getFieldArray2DDouble(parent, path, i, j);
}

bool std_getFieldArray2DBool(void* parent, char* path, int i, int j) {
    return getFieldArray2DBool(parent, path, i, j);
}

long std_getFieldArray2DLongH(void* parent, char* path, int i, int j) {
    return getFieldArray2DLongH(parent, path, i, j);
}

long std_getFieldArray2DLongL(void* parent, char* path, int i, int j) {
    return getFieldArray2DLongL(parent, path, i, j);
}

long std_getFieldArray2DByte(void* parent, char* path, int i, int j) {
    return getFieldArray2DByte(parent, path, i, j);
}

long std_getFieldArray2DShort(void* parent, char* path, int i, int j) {
    return getFieldArray2DShort(parent, path, i, j);
}

long std_getFieldArray2DChar(void* parent, char* path, int i, int j) {
    return getFieldArray2DChar(parent, path, i, j);
}

long std_getFieldArraySize(void* parent, char* path, int dim) {
    return getFieldArraySize(parent, path, dim);
}

bool std_isEqual(void* a, void* b) {
    return isEqual(a, b);
}

bool std_isNull(void* obj) {
    return isNull(obj);
}

bool std_isPathValid(void* parent, char* path) {
    return isPathValid(parent, path);
}

//from Input.cpp

void std_sendKeys(char* str) {
    sendKeys(str);
}

bool std_isKeyDown(long c) {
    return isKeyDown(c);
}

void std_holdKey(long c) {
    holdKey(c);
}

void std_releaseKey(long c) {
    releaseKey(c);
}

void std_getMousePos(long& x, long& y) {
    getMousePos(x, y);
}

void std_moveMouse(long x, long y) {
    moveMouse(x, y);
}

void std_windMouse(long x, long y) {
    windMouse(x, y);
}

void std_holdMouse(long x, long y, bool left) {
    holdMouse(x, y, left);
}

void std_releaseMouse(long x, long y, bool left) {
    releaseMouse(x, y, left);
}

void std_clickMouse(long x, long y, bool left) {
    clickMouse(x, y, left);
}

void std_holdMousePlus(long x, long y, long button) {
    holdMousePlus(x, y, button);
}

void std_releaseMousePlus(long x, long y, long button) {
    releaseMousePlus(x, y, button);
}

void std_clickMousePlus(long x, long y, long button) {
    clickMousePlus(x, y, button);
}

bool std_isMouseButtonHeld(long button) {
    return isMouseButtonHeld( button);
}

//from Color.cpp

int std_getColor(int x, int y) {
    return getColor(x, y);
}

bool std_findColor(int& x, int& y, int color, int xs, int ys, int xe, int ye) {
    return findColor(x, y, color, xs, ys, xe, ye);
}

bool std_findColorTol(int& x, int& y, int color, int xs, int ys, int xe, int ye, int tol) {
    return findColorTol(x, y, color, xs, ys, xe, ye, tol);
}

bool std_findColorSpiral(int& x, int& y, int color, int xs, int ys, int xe, int ye) {
    return findColorSpiral(x, y, color, xs, ys, xe, ye);
}

bool std_findColorSpiralTol(int& x, int& y, int color, int xs, int ys, int xe, int ye, int tol) {
    return findColorSpiralTol(x, y, color, xs, ys, xe, ye, tol);
}

/*
Unused and thus no reason to make cross compatible --- but still faster than anything else so leave in
bool std_findBitmapTolIn(int hdc, int& x, int& y, int, int, int, int, int) {
    return findBitmapTolIn(hdc, int&, int&, int, int, int, int, int);
}

bool std_findBitmapTol(int hdc, int& x, int& y, int, int, int, int) {
    return findBitmapTol(hdc, int&, int&, int, int, int, int);
}

bool std_findBitmap(int hdc, int&, int&) {
    return findBitmap(hdc, int&, int&);
}
 */
