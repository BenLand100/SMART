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

#include "Remote.h"
#include "Bridge.h"
#include <iostream>
#include <time.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#ifndef _WIN32
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#endif

using namespace std;

static char *root,*params,*initseq,*useragent,*jvmpath,*maxmem;
static int width,height;
static void *img,*dbg;
static void *memmap;
static void **functions;

shm_data *data;

/**
 * Loads the SMART library, links the required methods, and starts SMART according
 * to the current arguments
 */
void initSMART() {
    #ifndef _WIN32
        #if __SIZEOF_POINTER__ == 4
            void* libsmart = dlopen("./libsmart32.so",RTLD_LAZY);
        #else
            void* libsmart = dlopen("./libsmart64.so",RTLD_LAZY);
        #endif
    #else
        #if __SIZEOF_POINTER__ == 4
            HMODULE libsmart = LoadLibrary("./libsmart32.dll");
        #else
            HMODULE libsmart = LoadLibrary("./libsmart64.dll");
        #endif
    #endif
    cout << "Library: " << libsmart << '\n';
    functions = (void**)malloc(sizeof(void*)*NumExports);
    SetupRemote setup;
    SetUserAgent setAgent;
    SetJVMPath setPath;
    SetMaxJVMMem setMem;
    #ifndef _WIN32
        setup = (SetupRemote)dlsym(libsmart, "setupRemote");
        setAgent = (SetUserAgent)dlsym(libsmart, "setUserAgent");
        setPath = (SetJVMPath)dlsym(libsmart, "setJVMPath");
        setMem = (SetMaxJVMMem)dlsym(libsmart, "setMaxJVMMem");
        for (int i = 0; i < NumImports; i++) {
            functions[i] = (void*)dlsym(libsmart, imports[i]);
        }
    #else
        setup = (SetupRemote)GetProcAddress(libsmart, "setupRemote");
        setAgent = (SetUserAgent)GetProcAddress(libsmart, "setUserAgent");
        setPath = (SetJVMPath)GetProcAddress(libsmart, "setJVMPath");
        setMem = (SetMaxJVMMem)GetProcAddress(libsmart, "setMaxJVMMem");
        for (int i = 0; i < NumImports; i++) {
            functions[i] = (void*)GetProcAddress(libsmart, imports[i]);
        }
    #endif
    if (useragent) setAgent(useragent);
    if (jvmpath) setPath(jvmpath);
    if (maxmem) setMem(atoi(maxmem));
    setup(root,params,width,height,img,dbg,initseq);
}

/**
 * Executes functions passed from the paired process
 */
void execfun() {
    switch (data->funid) {
        case getRefresh:
            *(int*)(data->args) = ((type_getRefresh)(functions[getRefresh-NoFunc]))();
            break;
        case setRefresh:
            ((type_setRefresh)(functions[setRefresh-NoFunc]))(*(int*)(data->args));
            break;
        case setTransparentColor:
            ((type_setTransparentColor)(functions[setTransparentColor-NoFunc]))(*(int*)(data->args));
            break;
        case setDebug:
            ((type_setDebug)(functions[setDebug-NoFunc]))(*(bool*)(data->args));
            break;
        case setGraphics:
            ((type_setGraphics)(functions[setGraphics-NoFunc]))(*(bool*)(data->args));
            break;
        case setEnabled:
            ((type_setEnabled)(functions[setEnabled-NoFunc]))(*(bool*)(data->args));
            break;
        case isActive:
            *(bool*)(data->args) = ((type_isActive)(functions[isActive-NoFunc]))();
            break;
        case isBlocking:
            *(bool*)(data->args) = ((type_isBlocking)(functions[isBlocking-NoFunc]))();
            break;
        case getMousePos:
            ((type_getMousePos)(functions[getMousePos-NoFunc]))(((int*)(data->args))[0],((int*)(data->args))[1]);
            break;
        case holdMouse:
            ((type_holdMouse)(functions[holdMouse-NoFunc]))(((int*)(data->args))[0],((int*)(data->args))[1],(bool)(((int*)(data->args))[2-NoFunc]));
            break;
        case releaseMouse:
            ((type_releaseMouse)(functions[releaseMouse-NoFunc]))(((int*)(data->args))[0],((int*)(data->args))[1],(bool)(((int*)(data->args))[2-NoFunc]));
            break;
        case holdMousePlus:
            ((type_holdMousePlus)(functions[holdMousePlus-NoFunc]))(((int*)(data->args))[0],((int*)(data->args))[1],((int*)(data->args))[2]);
            break;
        case releaseMousePlus:
            ((type_releaseMousePlus)(functions[releaseMousePlus-NoFunc]))(((int*)(data->args))[0],((int*)(data->args))[1],((int*)(data->args))[2]);
            break;
        case moveMouse:
            ((type_moveMouse)(functions[moveMouse-NoFunc]))(((int*)(data->args))[0],((int*)(data->args))[1]);
            break;
        case windMouse:
            ((type_windMouse)(functions[windMouse-NoFunc]))(((int*)(data->args))[0],((int*)(data->args))[1]);
            break;
        case clickMouse:
            ((type_clickMouse)(functions[clickMouse-NoFunc]))(((int*)(data->args))[0],((int*)(data->args))[1],(bool)(((int*)(data->args))[2-NoFunc]));
            break;
        case clickMousePlus:
            ((type_clickMousePlus)(functions[clickMousePlus-NoFunc]))(((int*)(data->args))[0],((int*)(data->args))[1],((int*)(data->args))[2]);
            break;
        case isMouseButtonHeld:
            *(bool*)(data->args) = ((type_isMouseButtonHeld)(functions[isMouseButtonHeld-NoFunc]))(*(int*)(data->args));
            break;
        case sendKeys:
            ((type_sendKeys)(functions[sendKeys-NoFunc]))((char*)data->args);
            break;
        case holdKey:
            ((type_holdKey)(functions[holdKey-NoFunc]))(*(int*)data->args);
            break;
        case releaseKey:
            ((type_releaseKey)(functions[releaseKey-NoFunc]))(*(int*)data->args);
            break;
        case isKeyDown:
            *(bool*)(data->args) = ((type_isKeyDown)(functions[isKeyDown-NoFunc]))(*(int*)data->args);
            break;
        case getColor:
            *(int*)(data->args) = ((type_getColor)(functions[getColor-NoFunc]))(((int*)(data->args))[0],((int*)(data->args))[1]);
            break;
        case findColor:
            *(bool*)(data->args) = ((type_findColor)(functions[findColor-NoFunc]))(((int*)(data->args))[0],((int*)(data->args))[1],((int*)(data->args))[2],((int*)(data->args))[3],((int*)(data->args))[4],((int*)(data->args))[5],((int*)(data->args))[6]);
            //x&y autoupdated
            break;
        case findColorTol:
            *(bool*)(data->args) = ((type_findColorTol)(functions[findColorTol-NoFunc]))(((int*)(data->args))[0],((int*)(data->args))[1],((int*)(data->args))[2],((int*)(data->args))[3],((int*)(data->args))[4],((int*)(data->args))[5],((int*)(data->args))[6],((int*)(data->args))[7]);
            //x&y autoupdated
            break;
        case findColorSpiral:
            *(bool*)(data->args) = ((type_findColorSpiral)(functions[findColorSpiral-NoFunc]))(((int*)(data->args))[0],((int*)(data->args))[1],((int*)(data->args))[2],((int*)(data->args))[3],((int*)(data->args))[4],((int*)(data->args))[5],((int*)(data->args))[6]);
            //x&y autoupdated
            break;
        case findColorSpiralTol:
            *(bool*)(data->args) = ((type_findColorSpiralTol)(functions[findColorSpiralTol-NoFunc]))(((int*)(data->args))[0],((int*)(data->args))[1],((int*)(data->args))[2],((int*)(data->args))[3],((int*)(data->args))[4],((int*)(data->args))[5],((int*)(data->args))[6],((int*)(data->args))[7]);
            //x&y autoupdated
            break;
        default:
            cout << "Invalid function id: " << data->funid << '\n';
    }
    data->funid = 0;
}

int main(int argc, char** argv) {
    //Read init args from arguments
    if (argc != 9) exit(0);
    
    root = argv[1];
    params = argv[2];
    width = atoi(argv[3]);
    height = atoi(argv[4]);
    initseq = argv[5];
    
    useragent = argv[6]; 
    if (strlen(useragent)<=0) useragent = 0;
    jvmpath = argv[7];
    if (strlen(jvmpath)<=0) jvmpath = 0;
    maxmem = argv[8];
    if (strlen(maxmem)<=0) maxmem = 0;
   
    //Create the shared memory file
    char shmfile[256];
    sprintf(shmfile,"SMART.%i",getpid());
    #ifndef _WIN32
    int fd = open(shmfile,O_CREAT|O_RDWR,S_IRWXU|S_IRWXG|S_IRWXO); 
    lseek(fd,width*height*2+sizeof(shm_data),0);
    write(fd,"\0",1); //ensure proper size
    fsync(fd); //flush to disk
    memmap = mmap(NULL, width*height*2+sizeof(shm_data), PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, fd, 0);
    #else
    //IMPLEMENT
    #endif
    cout << "Shared Memory mapped to " << memmap << "\n";
    
    //Init the shm_data structure
    data = (shm_data*)memmap;
    data->id = getpid();
    data->paired = 0;
    data->width = width;
    data->height = height;
    data->time = 0;
    data->die = 0;
    data->funid = 0;
    data->imgstart = sizeof(shm_data);
    data->dbgstart = sizeof(shm_data)+width*height;
    #ifndef _WIN32
    fsync(fd); //Flush this to disk, probably not necessary
    #else
    //IMPLEMENT
    #endif

    //Let SMART use the shared memory for the images
    img = memmap + data->imgstart;
    dbg = memmap + data->dbgstart;
    
    //Load the smart plugin and link functions
    initSMART();

    //Event loop: updates time, checks for function calls, and unpairs if the paired process dies
    //Terminates when the SMART client closes OR if we recieve a die flag from a paired process
    for (unsigned int i = 0; !data->die && ((type_isActive)functions[isActive-NoFunc])(); i++) {
        data->time = time(0);
        if (data->funid != 0) execfun();
        #ifndef _WIN32
        sleep(0);
        #else
        //IMPLEMENT
        #endif
        if (!(i%1000000)) {
            #ifndef _WIN32
            if (data->paired && kill(data->paired,0)) {
            #else
            if (data->paired && true /*not dead*/) {//IMPLEMENT
            #endif
                cout << "Paired process terminate: reset\n";
                data->paired = 0;
            }
        }
    }
    
    #ifndef _WIN32
    //Free memory and delete SMART.[pid] on terminate
    munmap(memmap,width*height*2+sizeof(shm_data));
    close(fd);
    unlink(shmfile);
    #else
    //IMPLEMENT
    #endif
    
    return 1;
}
