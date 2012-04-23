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
#include <sys/mman.h>
#include <time.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>

using namespace std;

static char *root,*params,*initseq,*useragent,*jvmpath,*maxmem;
static int width,height;
static void *img,*dbg;
static void *memmap;

typedef void (*SetupRemote)(char*,char*,long,long,void*,void*,char*) __attribute__((cdecl));
typedef void (*SetUserAgent)(char*) __attribute__((cdecl));
typedef void (*SetJVMPath)(char*) __attribute__((cdecl));
typedef void (*SetMaxJVMMem)(int) __attribute__((cdecl));

shm_data *data;

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
    SetupRemote setup;
    SetUserAgent setAgent;
    SetJVMPath setPath;
    SetMaxJVMMem setMem;
    #ifndef _WIN32
        setup = (SetupRemote)dlsym(libsmart, "setupRemote");
        setAgent = (SetUserAgent)dlsym(libsmart, "setUserAgent");
        setPath = (SetJVMPath)dlsym(libsmart, "setJVMPath");
        setMem = (SetMaxJVMMem)dlsym(libsmart, "setMaxJVMMem");
    #else
        setup = (SetupRemote)GetProcAddress(libsmart, "setupRemote");
        setAgent = (SetUserAgent)GetProcAddress(libsmart, "setUserAgent");
        setPath = (SetJVMPath)GetProcAddress(libsmart, "setJVMPath");
        setMem = (SetMaxJVMMem)GetProcAddress(libsmart, "setMaxJVMMem");
    #endif
    cout << "Found my functions!\n";
    if (useragent) setAgent(useragent);
    if (jvmpath) setPath(jvmpath);
    if (maxmem) setMem(atoi(maxmem));
    setup(root,params,width,height,img,dbg,initseq);
}

void execfun() {
    switch (data->funid) {
        case 1: cout << (char*)data->scratch << '\n';
    }
    data->funid = 0;
}

int main(int argc, char** argv) {
    cout << "Can has starting!\n";
    
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
    
    cout << "Can has arguments\n";
   
    char shmfile[256];
    sprintf(shmfile,"SMART.%i",getpid());
    int fd = open(shmfile,O_CREAT|O_RDWR,S_IRWXU|S_IRWXG|S_IRWXO); 
    lseek(fd,width*height*2+sizeof(shm_data),0);
    write(fd,"\0",1);
    fsync(fd);
    memmap = mmap(NULL, width*height*2+sizeof(shm_data), PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, fd, 0);
    
    cout << "Can has shared memory @ " << memmap << "\n";
    
    
    data = (shm_data*)memmap;
    data->id = getpid();
    data->paired = 0;
    data->width = width;
    data->height = height;
    data->ping = data->pong = time(0);
    data->die = 0;
    data->funid = 0;
    
    fsync(fd);
    
    cout << "Can has data\n";
    
    data->imgstart = sizeof(shm_data);
    data->dbgstart = sizeof(shm_data)+width*height;

    img = memmap + data->imgstart;
    dbg = memmap + data->dbgstart;
    
    initSMART();
    
    cout << "Can has waiting...\n";

    while (!data->die) {
        data->ping = time(0);
        //if (data->ping - data->pong > 5000) data->paired = 0;
        if (data->funid != 0) execfun();
    }
    
    cout << "Can has unwanted :<\n";
    
    munmap(memmap,width*height*2+sizeof(shm_data));
    close(fd);
    
    cout << "Can has dead.\n";
    
    return 1;
}
