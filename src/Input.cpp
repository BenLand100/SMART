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
 
//Handles Key/Mouse interaction, mostly through JNI to Java
 
#include "Input.h"

extern JNIEnv* jre;
extern jobject smart;
extern JCLIENT _client;
extern JPOINT _point;

void sendKeys(char* text) {
    if (jre) {
        jobject string_ = jre->NewStringUTF(text);
        jre->CallVoidMethod(smart, _client.sendkeys, string_);
        jre->DeleteLocalRef(string_);
    }
}

bool isKeyDown(long code) {
    return (jre) ? jre->CallBooleanMethod(smart, _client.iskeydown, code) : false;
}

void holdKey(long code) {
    if (jre) jre->CallVoidMethod(smart, _client.holdkey, code);
}

void releaseKey(long code) {
    if (jre) jre->CallVoidMethod(smart, _client.releasekey, code);
}

void getMousePos(long& x, long& y) {
    if (jre) {
        jobject pt = jre->CallObjectMethod(smart, _client.getmousepos);
        x = jre->GetIntField(pt, _point.x);
        y = jre->GetIntField(pt, _point.y);
        jre->DeleteLocalRef(pt);
    } else {
        x = -1;
        y = -1;
    }
}

void moveMouse(long x, long y) {
    if (jre) jre->CallVoidMethod(smart, _client.movemouse, x, y);
}

void windMouse(long x, long y) {
    if (jre) jre->CallVoidMethod(smart, _client.windmouse, x, y);
}

void dragMouse(long x, long y) {
    if (jre) jre->CallVoidMethod(smart, _client.dragmouse, x, y);
}

void holdMouse(long x, long y, bool left) {
    if (jre) jre->CallVoidMethod(smart, _client.holdmouse, x, y, left ? 1 : 0);
}

void releaseMouse(long x, long y, bool left) {
    if (jre) jre->CallVoidMethod(smart, _client.releasemouse, x, y, left ? 1 : 0);
}

void clickMouse(long x, long y, bool left) {
    if (jre) jre->CallVoidMethod(smart, _client.clickmouse, x, y, left ? 1 : 0);
}
