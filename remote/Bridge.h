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
    int id,paired,width,height,ping,pong,die,imgstart,dbgstart;
    unsigned char funid,scratch[4096];
} shm_data;

#define NumExports 29
static char* exports[] = {
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
