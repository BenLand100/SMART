/** 
 *  Copyright 2010 by Benjamin J. Land (a.k.a. BenLand100) 
 * 
 *  This file is part of the Extensible Input/Output System (EIOS) 
 * 
 *  EIOS is free software: you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation, either version 3 of the License, or 
 *  (at your option) any later version. 
 * 
 *  EIOS is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with EIOS. If not, see <http://www.gnu.org/licenses/>. 
 */ 
  
#include "EIOS.h"
#include "Smart.h"
#include "Input.h"
#include "JVM.h"
#include <string.h>
#include <iostream>
#include <cstdlib>

typedef struct {
    int version;
    char *root, *params;
    int width, height;
    char *initseq; //deprecated?
    int maxmem; //in MB
    char *jvmpath, *useragent;
} initargs;
  
 
Target EIOS_RequestTarget(char *initarg) {
    int len;
    if (initarg != 0 && (len=strlen(initarg)) > 0) {
        initargs args;
        char *buffer = new char[len+1];
        int ida=0,idb=0,idx;
        while (ida < len && (buffer[idb++]=initarg[ida++]) != ','); buffer[idb-1]=0;
        if (initarg[ida++] != '"') return NULL;
        args.version = atoi(&buffer[0]);
        if (args.version == 0) {
            args.root = &buffer[idb];
            while (ida < len && (buffer[idb++]=initarg[ida++]) != '"'); buffer[idb-1]=0;
            if (initarg[ida++] != ',') return NULL;
            if (initarg[ida++] != '"') return NULL;
            args.params = &buffer[idb];
            while (ida < len && (buffer[idb++]=initarg[ida++]) != '"'); buffer[idb-1]=0;
            if (initarg[ida++] != ',') return NULL;
            idx = idb;
            while (ida < len && (buffer[idb++]=initarg[ida++]) != ','); buffer[idb-1]=0;
            if (initarg[ida-1] != ',') return NULL;
            args.width = atoi(&buffer[idx]);
            idx = idb;
            while (ida < len && (buffer[idb++]=initarg[ida++]) != ','); buffer[idb-1]=0;
            if (initarg[ida-1] != ',') return NULL;
            args.height = atoi(&buffer[idx]);
            if (initarg[ida++] != '"') return NULL;
            args.initseq = &buffer[idb];
            while (ida < len && (buffer[idb++]=initarg[ida++]) != '"'); buffer[idb-1]=0;
            if (initarg[ida++] != ',') return NULL;
            idx = idb;
            while (ida < len && (buffer[idb++]=initarg[ida++]) != ','); buffer[idb-1]=0;
            if (initarg[ida-1] != ',') return NULL;
            args.maxmem = atoi(&buffer[idx]);
            if (initarg[ida++] != '"') return NULL;
            args.jvmpath = &buffer[idb];
            while (ida < len && (buffer[idb++]=initarg[ida++]) != '"'); buffer[idb-1]=0;
            if (initarg[ida++] != ',') return NULL;if (initarg[ida++] != '"') return NULL;
            args.useragent = &buffer[idb];
            while (ida < len && (buffer[idb++]=initarg[ida++]) != '"'); buffer[idb-1]=0;
            
            if (args.maxmem != -1) setMaxJVMMem(args.maxmem);
            if (strlen(args.jvmpath) > 0) setJVMPath(args.jvmpath);
            if (strlen(args.useragent) > 0) setUserAgent(args.useragent);
            setup(args.root,args.params,args.width,args.height,args.initseq);
            return (void*)1; //Smart only supports one target at a time, this is just a success response.
        }
    }
    return NULL; //This result signifies a failure
}

void EIOS_ReleaseTarget(Target t) {
    //hardReset();
}
 
void EIOS_GetTargetDimensions(Target t, long* width, long* height) {
    getDimensions((int*)width,(int*)height); //Hope these are the same size lol
}
 
rgb* EIOS_GetImageBuffer(Target t) {
    return (rgb*)getImageArray();
}

void EIOS_UpdateImageBuffer(Target t) {
    //SMART constantly updates the buffer, so ignore
}
  
void EIOS_GetMousePosition(Target t, long* x, long* y) {
    getMousePos(*x,*y);
}

void EIOS_MoveMouse(Target t, long x, long y) {
    moveMouse(x,y);
}

void EIOS_HoldMouse(Target t, long x, long y, int button) {
    holdMousePlus(x,y,button);
}

void EIOS_ReleaseMouse(Target t, long x, long y, int button) {
    releaseMousePlus(x,y,button);
}

bool EIOS_IsMouseHeld(Target t, int button) {
    return isMouseButtonHeld(button);
}
  
void EIOS_SendString(Target t, char* str) {
    sendKeys(str);
}

void EIOS_HoldKey(Target t, long key) {
    holdKey(key);
}
 
void EIOS_ReleaseKey(Target t, long key) {
    releaseKey(key);
}
 
bool EIOS_IsKeyHeld(Target t, long key) {
    return isKeyDown(key);
}
