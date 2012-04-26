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
 
#ifndef _BRIDGE_H
#define _BRIDGE_H
 
typedef struct {
    int id;             //PID of the client
    int paired;         //TID of the controller or ZERO if none
    int refcount;       //Number of times the parent has paired
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

#endif
