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

#include "Local.h"
#include "Bridge.h"
#include <time.h>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#ifndef _WIN32
#include <sys/syscall.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <dirent.h>
#endif


#if __SIZEOF_POINTER__ == 4
    #define bits "32"
#else
    #define bits "64"
#endif
#define TIMEOUT 5

using namespace std;

static int width,height;
#ifndef _WIN32
static int fd;
static void* memmap;
#else
static HANDLE file;
static HANDLE memmap;
#endif
static shm_data *data;

void cleanup() {
    if (memmap) {
        #ifndef _WIN32
        munmap(memmap,width*height*2+sizeof(shm_data));
        close(fd);
        #else
        UnmapViewOfFile(data);
        CloseHandle(memmap);
        CloseHandle(file);
        #endif
        memmap = NULL;
        data = NULL;
    }
}

/**
 * Finds the accessible clients, either all or only those unpaired.
 * If **clients is non-null it will be filled with an array of the IDs.
 * The array MUST BE FREED by the caller
 *
 * This will also clean up any zombies left hanging.
 */
int getClients(bool only_unpaired, int **clients) {
    int count = 0;
    if (clients) *clients = (int*)realloc(NULL,count*sizeof(int));
    #ifndef _WIN32
    DIR *dir = opendir(".");
    struct dirent *dp;
	while ((dp=readdir(dir)) != NULL) {
		char *name = strstr(dp->d_name,"SMART.");
		if (name == dp->d_name) {
	        FILE *f = fopen(name, "rb");
	        shm_data temp;
	        fread(&temp, sizeof(shm_data), 1, f);
	        fclose(f);
	        if (temp.time && time(0) - temp.time > TIMEOUT) {
                unlink(name);
                continue;
            }
		    if (only_unpaired) {
		        if (temp.paired) continue;
		    }
		    name += 6;
		    count++;
		    if (clients) {
		        *clients = (int*)realloc(*clients,count*sizeof(int));
		        (*clients)[count-1] = atoi(name);
		    }
		}
	}
	closedir(dir);
	#else
	WIN32_FIND_DATA find;
	HANDLE hfind = FindFirstFile("SMART.*",&find);
	while (hfind != INVALID_HANDLE_VALUE) {
        shm_data temp;
        char *name = find.cFileName;
	    HANDLE file = CreateFile(
            name,
            GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_RANDOM_ACCESS|FILE_FLAG_DELETE_ON_CLOSE|FILE_FLAG_OVERLAPPED,
            NULL);
        DWORD sz;
        ReadFile(file,&temp,sizeof(shm_data),&sz,NULL);
        CloseHandle(file);
        if (temp.time && time(0) - temp.time > TIMEOUT) {
            DeleteFile(name);
            if (!FindNextFile(hfind,&find)) {
                FindClose(hfind);
                break;
            }
            continue;
        }
        if (only_unpaired && temp.paired) {
            if (!FindNextFile(hfind,&find)) {
                FindClose(hfind);
                break;
            }
            continue;
        }
        name += 6;
        count++;
        if (clients) {
            *clients = (int*)realloc(*clients,count*sizeof(int));
            (*clients)[count-1] = atoi(name);
        }
        if (!FindNextFile(hfind,&find)) {
            FindClose(hfind);
            break;
        }
    }
    #endif
    return count;
}

/**
 * Returns the number of clients accessible
 * Either the total number or only the number of unpaired
 */
int std_countClients(bool only_unpaired) {
    return getClients(only_unpaired,NULL);
}

/**
 * Returns the IDs of clients accessible
 * Either all clients or only the unpaired clients
 * *clients must be PREALLOCATED to the count to be returned
 */
int std_getClients(bool only_unpaired, int maxc, int *clients) {
    int *found;
    int foundc = getClients(only_unpaired,&found);
    memcpy(clients,found,(foundc < maxc ? foundc : maxc)*sizeof(int));
    return foundc;
}

/**
 * Returns the current paired client's ID 
 */
int std_getCurrent() {
    return data ? data->id : 0;
}

/**
 * Kills the client with the given ID
 * !!!Fails if the client is paired with ANOTHER controller!!!
 */
void std_killClient(int id) {
    int old = data ? data->id : 0;
    if (old != id) std_pairClient(id);
    if (data) {
        data->die = 1;
    }
    std_pairClient(old);
}

/**
 * Creates a remote SMART client and pairs to it, returning its ID
 */
int std_spawnClient(char* remote_path, char *root, char *params, int width, int height, char *initseq, char *useragent, char *jvmpath, int maxmem) {
    if (!remote_path || !root || !params) return 0;
    char _width[256],_height[256];
    sprintf(_width,"%i",width);
    sprintf(_height,"%i",height);
    char empty = '\0';
    if (!initseq) initseq = &empty;
    if (!useragent) useragent = &empty;
    if (!jvmpath) jvmpath = &empty;
    char _maxmem[256];
    if (maxmem<=0) {
        _maxmem[0]='\0';
    } else {
        sprintf(_maxmem,"%i",maxmem);
    }
    #ifdef _WIN32
    int len = 2*strlen(remote_path)+strlen(root)+strlen(params)+strlen(_width)+strlen(_height)+strlen(initseq)+strlen(useragent)+strlen(jvmpath)+strlen(_maxmem);
    char *args = new char[len+10*3+20];
    sprintf(args,"\"%ssmartremote%s.exe\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"",remote_path,bits,remote_path,root,params,_width,_height,initseq,useragent,jvmpath,_maxmem);
    char *exec = new char[strlen(remote_path)+20];
    sprintf(exec,"%ssmartremote%s.exe",remote_path,bits);
    cout << exec << '\n';
    PROCESS_INFORMATION procinfo;
    STARTUPINFO startupinfo;
    memset(&startupinfo, 0, sizeof(STARTUPINFO));
    memset(&procinfo, 0, sizeof(PROCESS_INFORMATION));
    startupinfo.cb = sizeof(STARTUPINFOW); 
    CreateProcess(exec,args,NULL,NULL,FALSE,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&startupinfo,&procinfo);
    CloseHandle(procinfo.hProcess);
    CloseHandle(procinfo.hThread);
    delete exec;
    delete args;
    do {
        Sleep(1000);
    } while  (!std_pairClient(procinfo.dwProcessId));
    call(Ping);
    return procinfo.dwProcessId;
    #else
    int v = fork();
    if (v) {
        do {
            sleep(1);
        } while  (!std_pairClient(v));
        call(Ping);
        return v;
    } else {
        char *exec = new char[strlen(remote_path)+20];
        sprintf(exec,"%s/smartremote%s",remote_path,bits);
        cout << exec << '\n';
        execl(exec,exec,remote_path,root,params,_width,_height,initseq,useragent,jvmpath,_maxmem,NULL);
        delete exec;
        exit(1);
    }
    #endif
}

/**
 * Pairing logic: 
 * 1) Try to open the file SMART.[pid]
 * 2) If file does not exist, abort
 * 3) Read the shm_data structure at the beginning of the file
 * 4) If data->time is older than TIMEOUT, assume zombie, destroy file, and abort
 * 5) If data->paired is nonzero, assume paired and abort
 * 6) Remap file with proper size of image and debug included
 * 7) Set data->paired to our PID
 */
bool std_pairClient(int id) {
    cleanup();
    char shmfile[256];
    sprintf(shmfile,"SMART.%i",id);
    #ifndef _WIN32
    fd = open(shmfile,O_RDWR);
    if (fd != -1) {
    #else
    file = CreateFile(
        shmfile,
        GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_RANDOM_ACCESS|FILE_FLAG_DELETE_ON_CLOSE|FILE_FLAG_OVERLAPPED,
        NULL);
    if (file != INVALID_HANDLE_VALUE) {
    #endif
        #ifndef _WIN32
        memmap = mmap(NULL,sizeof(shm_data),PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        data = (shm_data*)memmap;
        #else
        memmap = CreateFileMapping(file,NULL,PAGE_EXECUTE_READWRITE,0,sizeof(shm_data),shmfile);
        data = (shm_data*) MapViewOfFile(memmap,FILE_MAP_ALL_ACCESS,0,0,sizeof(shm_data));
        #endif
        int client_time = data->time;
        int client_paired = data->paired;
        width = data->width;
        height = data->height;
        #ifndef _WIN32
        munmap(memmap,+sizeof(shm_data));
        #else
        UnmapViewOfFile(data);
        CloseHandle(memmap);
        #endif
        if (client_time != 0 && time(0) - client_time > TIMEOUT) {
            cout << "Failed to pair - Zombie client detected\n";
            #ifndef _WIN32
            close(fd);
            unlink(shmfile);
            #else
            CloseHandle(file);
            DeleteFile(shmfile);
            #endif
            memmap = NULL;
            data = NULL;
            return false;
        }
        if (client_paired) { 
            cout << "Failed to pair - Client appears to be paired\n";
            #ifndef _WIN32
            close(fd);
            #else
            CloseHandle(file);
            #endif
            memmap = NULL;
            data = NULL;
            return false;
        }
        #ifndef _WIN32
        memmap = mmap(NULL,2*width*height+sizeof(shm_data),PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        data = (shm_data*)memmap;
        data->paired = syscall(SYS_gettid);
        #else
        memmap = CreateFileMapping(file,NULL,PAGE_EXECUTE_READWRITE,0,sizeof(shm_data)+2*width*height,shmfile);
        data = (shm_data*)MapViewOfFile(memmap,FILE_MAP_ALL_ACCESS,0,0,sizeof(shm_data)+2*width*height);
        data->paired = GetCurrentThreadId();
        #endif
        return true;
    } else {
        cout << "Failed to pair - No client by that ID\n";
        return false;
    }
}
 
//Returns a pointer into shared memory where the image resides
void* std_getImageArray() {
    return data ? memmap + data->imgstart : 0;
}

//Returns a pointer into shared memory where the debug image resides
void* std_getDebugArray() {
    return data ? memmap + data->dbgstart : 0;
}

//Invokes a remote method
void call(int funid) {
    //assume that anything calling this already checked if data is nonzero
    data->funid = funid;
    #ifndef _WIN32
    //syscall(SYS_tkill,data->listener,SIGALRM); //wake process
    #else
    #endif
    while (data->funid) { 
        #ifndef _WIN32
        sched_yield();
        #else
        Sleep(10); //ms
        #endif
        if (data->time && time(0) - data->time > TIMEOUT) {
            cout << "Client appears to have died: aborting link\n";
            char shmfile[256];
            sprintf(shmfile,"SMART.%i",data->id);
            cleanup();
            #ifndef _WIN32
            unlink(shmfile);
            #else
            DeleteFile(shmfile);
            #endif
        }
    }
}

int std_getRefresh() {
    if (data) {
        call(getRefresh);
        return *(int*)(data->args);
    } return -1;
}

void std_setRefresh(int x) {
    if (data) {
        *(int*)(data->args) = x;
        call(setRefresh);
    }
}

void std_setTransparentColor(int color) {
    if (data) {
        *(int*)(data->args) = color;
        call(setTransparentColor);
    }
}

void std_setDebug(bool enabled) {
    if (data) {
        *(bool*)(data->args) = enabled;
        call(setDebug);
    }
}


void std_setGraphics(bool enabled) {
    if (data) {
        *(bool*)(data->args) = enabled;
        call(setGraphics);
    }
}

void std_setEnabled(bool enabled) {
    if (data) {
        *(bool*)(data->args) = enabled;
        call(setEnabled);
    }
}

bool std_isActive() {
    if (data) {
        call(isActive);
        return *(bool*)(data->args);
    } else return false;
}

bool std_isBlocking() {
    if (data) {
        call(isBlocking);
        return *(bool*)(data->args);
    } else return false;
}

void std_getMousePos(int &x, int &y) {
    if (data) {
        call(getMousePos);
        x = ((int*)(data->args))[0];
        y = ((int*)(data->args))[1];
    }
}

void std_holdMouse(int x, int y, bool left) {
    if (data) {
        ((int*)(data->args))[0] = x;
        ((int*)(data->args))[1] = y;
        ((int*)(data->args))[2] = left; //not a mistake
        call(holdMouse);
    }
}

void std_releaseMouse(int x, int y, bool left) {
    if (data) {
        ((int*)(data->args))[0] = x;
        ((int*)(data->args))[1] = y;
        ((int*)(data->args))[2] = left; //not a mistake
        call(releaseMouse);
    }
}

void std_holdMousePlus(int x, int y, int button) {
    if (data) {
        ((int*)(data->args))[0] = x;
        ((int*)(data->args))[1] = y;
        ((int*)(data->args))[2] = button;
        call(holdMousePlus);
    }
}

void std_releaseMousePlus(int x, int y, int button) {
    if (data) {
        ((int*)(data->args))[0] = x;
        ((int*)(data->args))[1] = y;
        ((int*)(data->args))[2] = button;
        call(releaseMousePlus);
    }
}

void std_moveMouse(int x, int y) {
    if (data) {
        ((int*)(data->args))[0] = x;
        ((int*)(data->args))[1] = y;
        call(moveMouse);
    }
}

void std_windMouse(int x, int y) {
    if (data) {
        ((int*)(data->args))[0] = x;
        ((int*)(data->args))[1] = y;
        call(windMouse);
    }
}

void std_clickMouse(int x, int y, bool left) {
    if (data) {
        ((int*)(data->args))[0] = x;
        ((int*)(data->args))[1] = y;
        ((int*)(data->args))[2] = left; //not a mistake
        call(clickMouse);
    }
}

void std_clickMousePlus(int x, int y, int button) {
    if (data) {
        ((int*)(data->args))[0] = x;
        ((int*)(data->args))[1] = y;
        ((int*)(data->args))[2] = button;
        call(clickMousePlus);
    }
}

bool std_isMouseButtonHeld(int button) {
    if (data) {
        *(int*)(data->args) = button;
        call(isMouseButtonHeld);
        return *(bool*)(data->args);
    } else return false;
}

void std_sendKeys(char *text) {
    if (data) {
        strcpy((char*)data->args,text);
        call(sendKeys);
    }
}

void std_holdKey(int code) {
    if (data) {
        *(int*)(data->args) = code;
        call(holdKey);
    }
}

void std_releaseKey(int code) {
    if (data) {
        *(int*)(data->args) = code;
        call(releaseKey);
    }
}

bool std_isKeyDown(int code) {
    if (data) {
        *(int*)(data->args) = code;
        call(isKeyDown);
        return *(bool*)(data->args);
    } else return false;
}

int std_getColor(int x, int y) {
    if (data) {
        ((int*)(data->args))[0] = x;
        ((int*)(data->args))[1] = y;
        call(getColor);
        return *(int*)(data->args);
    } else return -1;
}

bool std_findColor(int &x, int& y, int color, int sx, int sy, int ex, int ey) {
    if (data) {
        ((int*)(data->args))[0] = x;
        ((int*)(data->args))[1] = y;
        ((int*)(data->args))[2] = color;
        ((int*)(data->args))[3] = sx;
        ((int*)(data->args))[4] = sy;
        ((int*)(data->args))[5] = ex;
        ((int*)(data->args))[6] = ey;
        call(findColor);
        x = ((int*)(data->args))[0];
        y = ((int*)(data->args))[1];
        return (bool)(((int*)(data->args))[2]);
    } else return false;
}

bool std_findColorTol(int &x, int& y, int color, int sx, int sy, int ex, int ey, int tol) {
    if (data) {
        ((int*)(data->args))[0] = x;
        ((int*)(data->args))[1] = y;
        ((int*)(data->args))[2] = color;
        ((int*)(data->args))[3] = sx;
        ((int*)(data->args))[4] = sy;
        ((int*)(data->args))[5] = ex;
        ((int*)(data->args))[6] = ey;
        ((int*)(data->args))[7] = tol;
        call(findColorTol);
        x = ((int*)(data->args))[0];
        y = ((int*)(data->args))[1];
        return (bool)(((int*)(data->args))[2]);
    } else return false;
}

bool std_findColorSpiral(int &x, int& y, int color, int sx, int sy, int ex, int ey) {
    if (data) {
        ((int*)(data->args))[0] = x;
        ((int*)(data->args))[1] = y;
        ((int*)(data->args))[2] = color;
        ((int*)(data->args))[3] = sx;
        ((int*)(data->args))[4] = sy;
        ((int*)(data->args))[5] = ex;
        ((int*)(data->args))[6] = ey;
        call(findColorSpiral);
        x = ((int*)(data->args))[0];
        y = ((int*)(data->args))[1];
        return (bool)(((int*)(data->args))[2]);
    } else return false;
}

bool std_findColorSpiralTol(int &x, int& y, int color, int sx, int sy, int ex, int ey, int tol) {
    if (data){
        ((int*)(data->args))[0] = x;
        ((int*)(data->args))[1] = y;
        ((int*)(data->args))[2] = color;
        ((int*)(data->args))[3] = sx;
        ((int*)(data->args))[4] = sy;
        ((int*)(data->args))[5] = ex;
        ((int*)(data->args))[6] = ey;
        ((int*)(data->args))[7] = tol;
        call(findColorSpiralTol);
        x = ((int*)(data->args))[0];
        y = ((int*)(data->args))[1];
        return (bool)(((int*)(data->args))[2]);
    } else return false;
}

void internalConstructor() {
    memmap = 0;
    data = 0;
}

void internalDestructor() {
    cleanup();
}


#ifndef _WIN32

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

void load(void) {
    internalConstructor();
}

void unload(void) {
    internalDestructor();
}

#endif

#ifdef _WIN32

HMODULE dllinst;

int GetFunctionCount() {
    return NumExports;
}

int GetFunctionInfo(int index, void*& address, char*& def) {
    if (index < NumExports) {
        address = (void*)GetProcAddress(dllinst, exports[index*2]);
        strcpy(def, exports[index*2+1]);
        return index;
    }
    return -1;
}

bool DllMain(HINSTANCE instance, int reason, void* checks) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            dllinst = instance;
            internalConstructor();
            return true;
        case DLL_THREAD_ATTACH:
            return true;
        case DLL_PROCESS_DETACH:
            internalDestructor();
            return true;
        case DLL_THREAD_DETACH:
            return true;
    }
    return false;
}

#endif
