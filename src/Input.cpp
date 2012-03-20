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
#include <iostream>

//Defined in Smart.cpp
extern JNIEnv* jre;
extern jobject smart;
extern JCLIENT _client;
extern JPOINT _point;

//Sends a String to the client in a human way
void sendKeys(char* text) {
    if (jre) {
        jobject string_ = jre->NewStringUTF(text);
        jre->CallVoidMethod(smart, _client.sendkeys, string_);
        jre->DeleteLocalRef(string_);
		checkexception(jre);
	}
}

//Tests if a key (by keycode) is being held
bool isKeyDown(long code) {
    return (jre) ? jre->CallBooleanMethod(smart, _client.iskeydown, code) : false;
}

//Holds a key (by keycode)
void holdKey(long code) {
    if (jre) {
		jre->CallVoidMethod(smart, _client.holdkey, code);
		checkexception(jre);
	}
}

//Releases a key (by keycode)
void releaseKey(long code) {
    if (jre) {
    	jre->CallVoidMethod(smart, _client.releasekey, code);
		checkexception(jre);
	}
}

//Fills the argumnet with the current mouse position
void getMousePos(long& x, long& y) {
    if (jre) {
        jobject pt = jre->CallObjectMethod(smart, _client.getmousepos);
        x = jre->GetIntField(pt, _point.x);
        y = jre->GetIntField(pt, _point.y);
        jre->DeleteLocalRef(pt);
		checkexception(jre);
    } else {
        x = -1;
        y = -1;
    }
}

//Moves the mouse instantly to the given coords
void moveMouse(long x, long y) {
    if (jre) {
    	jre->CallVoidMethod(smart, _client.movemouse, x, y);
		checkexception(jre);
	}
}

//Moves the mouse humanly to the given coords
void windMouse(long x, long y) {
    if (jre) {
    	jre->CallVoidMethod(smart, _client.windmouse, x, y);
		checkexception(jre);
	}
}

//Holds the mouse (left if true, right otherwise) at a position 
void holdMouse(long x, long y, bool left) {
    if (jre) {
    	jre->CallVoidMethod(smart, _client.holdmouse, x, y, left ? 1 : 3);
		checkexception(jre);
	}
}

//Holds the mouse (left if true, right otherwise) at a position 
void releaseMouse(long x, long y, bool left) {
    if (jre) {
    	jre->CallVoidMethod(smart, _client.releasemouse, x, y, left ? 1 : 3);
		checkexception(jre);
	}
}

//Clicks the mouse humanly at a position
void clickMouse(long x, long y, bool left) {
    if (jre) {
    	jre->CallVoidMethod(smart, _client.clickmouse, x, y, left ? 1 : 3);
		checkexception(jre);
	}
}

//Holds the mouse (left=1, mid=2, right=3) at a position 
void holdMousePlus(long x, long y, long button) {
    if (jre) {
    	jre->CallVoidMethod(smart, _client.holdmouse, x, y, button);
		checkexception(jre);
	}
}

//Holds the mouse (left=1, mid=2, right=3e) at a position 
void releaseMousePlus(long x, long y, long button) {
    if (jre) {
    	jre->CallVoidMethod(smart, _client.releasemouse, x, y, button);
		checkexception(jre);
	}
}

bool isMouseButtonHeld(long button) {
    if (jre) {
    	bool held = jre->CallBooleanMethod(smart, _client.ismousebuttonheld, button);
		checkexception(jre);
		return held;
	}
	return false;
}

//Clicks the mouse humanly at a position
void clickMousePlus(long x, long y, long button) {
    if (jre) {
    	jre->CallVoidMethod(smart, _client.clickmouse, x, y, button);
		checkexception(jre);
	}
}
