/**
 *  Copyright 2012 by Benjamin J. Land (a.k.a. BenLand100)
 *
 *  This file is part of the SMART
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

#include "SmartRemote.h"
#include <time.h>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <map>
#ifndef _WIN32
    #include <sys/syscall.h>
    #include <sys/mman.h>
    #include <fcntl.h>
    #include <dirent.h>
    #include <errno.h>
#endif

#define debug cout << "SMART: "

using namespace std;

static map<int,SMARTClient*> *pairedClients;
static SMARTClient *local;
static clients_dat clients;

void freeClient(SMARTClient *client) {
    if (!client) return;
    if (client->memmap) {
        debug << "Decrementing refcount [" << client->data->id << "]\n";
        if (--client->refcount == 0) {
            debug << "Freeing client data [" << client->data->id << "]\n";
            if (client->socket) {
                #ifndef _WIN32
                    close(client->socket);
                #else
                    closesocket(client->socket);
                #endif
                client->socket = 0;
            }
            pairedClients->erase(client->data->id);
            client->data->paired = 0;
            #ifndef _WIN32
            munmap(client->memmap,client->width*client->height*2*4+sizeof(shm_data));
            close(client->fd);
            #else
            UnmapViewOfFile(client->data);
            CloseHandle(client->memmap);
            CloseHandle(client->file);
            #endif
            client->memmap = NULL;
            client->data = NULL;
            delete client;
       }
    }
}

bool resock(SMARTClient *client) {
    if (client->socket) 
        #ifndef _WIN32
            close(client->socket);
        #else
            closesocket(client->socket);
        #endif
    client->socket = 0;
    struct hostent *localhost = gethostbyname("localhost");
    if (!localhost) {
        debug << "Could not resolve localhost, check your network settings\n";
        return false;
    }
    client->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in local;
    memset((char *) &local, 0, sizeof(local));
    local.sin_family = AF_INET;
    memcpy((char *)&local.sin_addr.s_addr,(char *)localhost->h_addr, localhost->h_length);
    local.sin_port = htons(client->data->port);
    debug << "Attempting to connect to localhost:" << client->data->port << "\n";
    int res;
    if ((res=connect(client->socket,(struct sockaddr *) &local,sizeof(local))) < 0) {
        debug << "Could not connect socket: ";
        #ifndef _WIN32
            debug << errno << '\n';
            close(client->socket);
        #else
            debug << WSAGetLastError() << '\n';
            closesocket(client->socket);
        #endif
        client->socket = 0;
        return false;
    }
    return true;
}

/**
 * Invokes a remote method. Might free your client if the client died.
 * Assumes arguments are already set and results are set according to the comm protocol
 */
void callClient(SMARTClient *client, char funid) {
    if (send(client->socket,(const char*)&funid,sizeof(char),0)!=sizeof(char)) {
        debug << "Could Not Call\n";
        return;
    }
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(client->socket, &rfds);
    for (int i = 0; i < 600; i++) {
        if (select(client->socket+1, &rfds, &rfds, NULL, &tv)) {
            if (recv(client->socket,(char*)&funid,sizeof(char),0)!=sizeof(char)) {
                debug << "Call appears to have failed, or client successfully killed.\n";
            }
            return;
        }
        tv.tv_sec = 0;
        tv.tv_usec = 100000;
    }
    debug << "Client timed out\n";
}

/**
 * Kills the client 
 */
void killClient(SMARTClient *client) {
    callClient(client,Die);
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
SMARTClient* pairClient(int id) {
    #ifndef _WIN32
    int tid = syscall(SYS_gettid);
    #else
    int tid = GetCurrentThreadId();
    #endif
    map<int,SMARTClient*>::iterator it = pairedClients->find(id);
    if (it != pairedClients->end()) {
        debug << "Client possibly paired to us\n";
        if (it->second->data->paired == tid) {
            debug << "Already paired: Incrementing refcount\n";
            it->second->refcount++;
            return it->second;
        }
        if (it->second->data->paired == 0) {
            debug << "Repairing: Incrementing refcount\n";
            it->second->refcount++;
            it->second->data->paired = tid; 
            if (resock(it->second)) {  
                return it->second;
            } else {
                debug << "Zombie detected (no socket response)\n";
                // ***FIXME*** Issue process kill here
                return NULL;
            }
        } else {
            debug << "Client is otherwise paired\n";
            return NULL;
        }
    }
    SMARTClient *client = new SMARTClient;
    char shmfile[256];
    sprintf(shmfile,"SMART.%i",id);
    #ifndef _WIN32
    client->fd = open(shmfile,O_RDWR);
    if (client->fd != -1) {
    #else
    client->file = CreateFile(
        shmfile,
        GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_RANDOM_ACCESS|FILE_FLAG_OVERLAPPED,
        NULL);
    if (client->file != INVALID_HANDLE_VALUE) {
    #endif
        #ifndef _WIN32
        client->memmap = mmap(NULL,sizeof(shm_data),PROT_READ|PROT_WRITE, MAP_SHARED, client->fd, 0);
        client->data = (shm_data*)client->memmap;
        if (!client->data) {
            debug << "Could not map shared memory (pre)\n";
            return NULL;   
        }
        #else
        client->memmap = CreateFileMapping(client->file,NULL,PAGE_READWRITE,0,sizeof(shm_data),shmfile);
        if (client->memmap == INVALID_HANDLE_VALUE) {
            debug << "Could not map shared file (pre)\n";
            return NULL;   
        }
        client->data = (shm_data*)MapViewOfFile(client->memmap,FILE_MAP_ALL_ACCESS,0,0,sizeof(shm_data));
        if (!client->data) {
            debug << "Could not map shared memory (pre)\n";
            return NULL;   
        }
        #endif
        int client_paired = client->data->paired;
        int client_width = client->data->width;
        int client_height = client->data->height;
        #ifndef _WIN32
        munmap(client->memmap,+sizeof(shm_data));
        #else
        UnmapViewOfFile(client->data);
        CloseHandle(client->memmap);
        #endif
        if (client_paired && client_paired != tid) { 
            debug << "Failed to pair - Client appears to be paired\n";
            #ifndef _WIN32
            close(client->fd);
            #else
            CloseHandle(client->file);
            #endif
            delete client;
            return NULL;
        }
        #ifndef _WIN32
        client->memmap = mmap(NULL,2*client_width*client_height*4+sizeof(shm_data),PROT_READ|PROT_WRITE, MAP_SHARED, client->fd, 0);
        client->data = (shm_data*)client->memmap;
        if (!client->data) {
            debug << "Could not map shared memory (post)\n";
            return NULL;   
        }
        #else
        client->memmap = CreateFileMapping(client->file,NULL,PAGE_READWRITE,0,sizeof(shm_data)+2*client_width*client_height*4,shmfile);
        if (client->memmap == INVALID_HANDLE_VALUE) {
            debug << "Could not map shared file (post)\n";
            return NULL;   
        }
        client->data = (shm_data*)MapViewOfFile(client->memmap,FILE_MAP_ALL_ACCESS,0,0,sizeof(shm_data)+2*client_width*client_height*4);
        if (!client->data) {
            debug << "Could not map shared memory (post)\n";
            return NULL;   
        }
        #endif
        
        client->data->paired = tid;
        debug << "Setting the client's controller to our TID\n";
        client->socket = 0;
        if (!resock(client)) {
            freeClient(client);
            return false;
        } 
        client->refcount = 1;
        #ifdef _WIN32
        FlushFileBuffers(client->file);
        #endif
        (*pairedClients)[id] = client;
        return client;
    } else {
        debug << "Failed to pair - No client by that ID\n";
        delete client;
        return NULL;
    }
}

/**
 * Creates a remote SMART client and pairs to it
 * FIXME javaargs not handled for linux
 */
SMARTClient* spawnClient(char* remote_path, char *root, char *params, int width, int height, char *initseq, char *useragent, char* javaargs, char* plugins) {
    SMARTClient *client;
    if (!remote_path || !root || !params) return 0;
    char _width[256],_height[256];
    sprintf(_width,"%i",width);
    sprintf(_height,"%i",height);
    char empty = '\0';
    if (!initseq) initseq = &empty;
    if (!useragent) useragent = &empty;
    if (!javaargs) javaargs = &empty;
	if (!plugins) plugins = &empty;
    char bootclasspath[512];
    sprintf(bootclasspath,"-Xbootclasspath/p:%s/%s",remote_path,"smart.jar");
    char library[512];
    #ifdef _WIN32
    sprintf(library,"%s/libsmartjni%s.dll",remote_path,bits);
    int len = strlen(javaargs)+strlen(bootclasspath)+strlen(library)+strlen(root)+strlen(params)+strlen(_width)+strlen(_height)+strlen(initseq)+strlen(useragent)+strlen(remote_path)+strlen(plugins)+7*3+50; //A little extra
    char *args = new char[len];
    sprintf(args,"%s %s smart.Main \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"",javaargs,bootclasspath,library,root,params,_width,_height,initseq,useragent, remote_path, plugins);
	SHELLEXECUTEINFO info;
    memset(&info, 0, sizeof(SHELLEXECUTEINFO));
    info.cbSize = sizeof(SHELLEXECUTEINFO); 
    info.fMask = SEE_MASK_NOCLOSEPROCESS;
    info.lpFile = "java.exe";
    info.lpParameters = args;
    info.nShow = SW_SHOWNORMAL;
    ShellExecuteEx(&info);
    delete args;
    if (!info.hProcess) {
        debug << "Failed to spawn process. Make sure java.exe is on your path and that SMART is installed correctly.\n";
        return NULL;
    }
    int pid = GetProcessId(info.hProcess);
    CloseHandle(info.hProcess);
    int count = 0;
    do {
        Sleep(1000);
        count++;
    } while  (!(client=pairClient(pid))&&count<10);
    if (count >= 10) return NULL;
    callClient(client,Ping);
    return client;
    #else
    sprintf(library,"%s/libsmartjni%s.so",remote_path,bits);
    int v = fork();
    if (v) {
        int count = 0;
        do {
            sleep(1);
            count++;
        } while  (!(client=pairClient(v)) && count<10);
        if (count >= 10) return NULL;
        callClient(client,Ping);
        return client;
    } else {
        execlp("java","java",bootclasspath,"smart.Main",library,root,params,_width,_height,initseq,useragent, remote_path, plugins, NULL);
        debug << "Process terminating. If nothing happened, make sure java is on your path and that SMART is installed correctly.\n";
        exit(1);
    }
    #endif
}

/**
 * Finds the accessible clients, either all or only those unpaired.
 * If **clients is non-null it will be filled with an array of the IDs.
 * The array MUST BE FREED by the caller
 *
 * This will also clean up any zombies left hanging.
 */
int getClients(bool only_unpaired, int **_clients) {
    int count = 0;
    if (_clients) *_clients = NULL;
    #ifndef _WIN32
    DIR *dir = opendir(".");
    struct dirent *dp;
	while ((dp=readdir(dir)) != NULL) {
		char *name = strstr(dp->d_name,"SMART.");
		if (name == dp->d_name) {
	        int fd = open(name,O_RDWR);
            shm_data *temp = (shm_data*)mmap(NULL,sizeof(shm_data),PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
		    if (only_unpaired && temp->paired) {
	            munmap(temp,sizeof(shm_data));
	            close(fd);
	            continue;
		    }
		    name += 6;
		    count++;
		    if (_clients) {
		        *_clients = (int*)realloc(*_clients,count*sizeof(int));
		        (*_clients)[count-1] = atoi(name);
		    }
            munmap(temp,sizeof(shm_data));
            close(fd);
		}
	}
	closedir(dir);
	#else
	WIN32_FIND_DATA find;
	HANDLE hfind = FindFirstFile("SMART.*",&find);
	while (hfind != INVALID_HANDLE_VALUE) {
        char *name = find.cFileName;
	    HANDLE file = CreateFile(
            name,
            GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_RANDOM_ACCESS|FILE_FLAG_OVERLAPPED,
            NULL);
        HANDLE memmap = CreateFileMapping(file,NULL,PAGE_READWRITE,0,sizeof(shm_data),name);
        shm_data *temp = (shm_data*)MapViewOfFile(memmap,FILE_MAP_ALL_ACCESS,0,0,sizeof(shm_data));
        if (only_unpaired && temp->paired) {
            UnmapViewOfFile(temp);
            CloseHandle(memmap);
            CloseHandle(file);
            if (!FindNextFile(hfind,&find)) {
                FindClose(hfind);
                break;
            }
            continue;
        }
        name += 6;
        count++;
        if (_clients) {
            *_clients = (int*)realloc(*_clients,count*sizeof(int));
            (*_clients)[count-1] = atoi(name);
        }
        UnmapViewOfFile(temp);
        CloseHandle(memmap);
        CloseHandle(file);
        if (!FindNextFile(hfind,&find)) {
            FindClose(hfind);
            break;
        }
    }
    #endif
    return count;
}

/**
 * Returns the number of clients in the structure
 */
int exp_clientID(int idx) {
    debug << "Returning index " << idx << '\n';
    if (idx < clients.count && idx >= 0) {
        return clients.ids[idx];
    }
    return 0;
}

/**
 * Returns a pointer to a structure containing the found clients
 */
int exp_getClients(bool only_unpaired) {
    if (clients.ids) delete clients.ids;
    clients.ids = 0;
    return clients.count = getClients(only_unpaired,&clients.ids);
}

/**
 * Returns the current paired client's ID 
 */
int exp_getCurrent() {
    return local ? local->data->id : 0;
}

/**
 * Kills the client with the given ID
 * !!!Fails if the client is paired with ANOTHER controller!!!
 */
bool exp_killClient(int pid) {
    debug << "Killing client " << pid << '\n';
    SMARTClient *client = pairClient(pid);
    if (!client) return false;
    killClient(client);
    freeClient(client);
    return true;
}

/**
 * Creates a remote SMART client and pairs to it, returning its ID or 0 if failed
 */
int exp_spawnClient(char* remote_path, char *root, char *params, int width, int height, char *initseq, char *useragent, char* javaargs, char* plugins) {
    freeClient(local);
    local = spawnClient(remote_path,root,params,width,height,initseq,useragent,javaargs,plugins);
    return local ? local->data->id : 0;
}

/**
 * Pairs to an existing client, returning its ID or 0 if failed
 */
bool exp_pairClient(int id) {
    SMARTClient *next = pairClient(id);
    if (next != local) freeClient(local);
    local = next;
    return local ? local->data->id : 0;
}
 
//Returns a pointer into shared memory where the image resides
void* exp_getImageArray() {
    return local ? (char*)local->data + local->data->imgoff : 0;
}

//Returns a pointer into shared memory where the debug image resides
void* exp_getDebugArray() {
    return local ? (char*)local->data + local->data->dbgoff : 0;
}

int exp_getRefresh() {
    if (local) {
        callClient(local,getRefresh);
        return *(int*)(local->data->args);
    } return -1;
}

void exp_setRefresh(int x) {
    if (local) {
        *(int*)(local->data->args) = x;
        callClient(local,setRefresh);
    }
}

void exp_setTransparentColor(int color) {
    if (local) {
        *(int*)(local->data->args) = color;
        callClient(local,setTransparentColor);
    }
}

void exp_setDebug(bool enabled) {
    if (local) {
        *(bool*)(local->data->args) = enabled;
        callClient(local,setDebug);
    }
}


void exp_setGraphics(bool enabled) {
    if (local) {
        *(bool*)(local->data->args) = enabled;
        callClient(local,setGraphics);
    }
}

void exp_setEnabled(bool enabled) {
    if (local) {
        *(bool*)(local->data->args) = enabled;
        callClient(local,setEnabled);
    }
}

bool exp_isActive() {
    if (local) {
        callClient(local,isActive);
        return *(bool*)(local->data->args);
    } else return false;
}

bool exp_isBlocking() {
    if (local) {
        callClient(local,isBlocking);
        return *(bool*)(local->data->args);
    } else return false;
}

void exp_getMousePos(int &x, int &y) {
    if (local) {
        callClient(local,getMousePos);
        x = ((int*)(local->data->args))[0];
        y = ((int*)(local->data->args))[1];
    }
}

void exp_holdMouse(int x, int y, bool left) {
    if (local) {
        ((int*)(local->data->args))[0] = x;
        ((int*)(local->data->args))[1] = y;
        ((int*)(local->data->args))[2] = left; //not a mistake
        callClient(local,holdMouse);
    }
}

void exp_releaseMouse(int x, int y, bool left) {
    if (local) {
        ((int*)(local->data->args))[0] = x;
        ((int*)(local->data->args))[1] = y;
        ((int*)(local->data->args))[2] = left; //not a mistake
        callClient(local,releaseMouse);
    }
}

void exp_holdMousePlus(int x, int y, int button) {
    if (local) {
        ((int*)(local->data->args))[0] = x;
        ((int*)(local->data->args))[1] = y;
        ((int*)(local->data->args))[2] = button;
        callClient(local,holdMousePlus);
    }
}

void exp_releaseMousePlus(int x, int y, int button) {
    if (local) {
        ((int*)(local->data->args))[0] = x;
        ((int*)(local->data->args))[1] = y;
        ((int*)(local->data->args))[2] = button;
        callClient(local,releaseMousePlus);
    }
}

void exp_moveMouse(int x, int y) {
    if (local) {
        ((int*)(local->data->args))[0] = x;
        ((int*)(local->data->args))[1] = y;
        callClient(local,moveMouse);
    }
}

void exp_windMouse(int x, int y) {
    if (local) {
        ((int*)(local->data->args))[0] = x;
        ((int*)(local->data->args))[1] = y;
        callClient(local,windMouse);
    }
}

void exp_clickMouse(int x, int y, bool left) {
    if (local) {
        ((int*)(local->data->args))[0] = x;
        ((int*)(local->data->args))[1] = y;
        ((int*)(local->data->args))[2] = left; //not a mistake
        callClient(local,clickMouse);
    }
}

void exp_clickMousePlus(int x, int y, int button) {
    if (local) {
        ((int*)(local->data->args))[0] = x;
        ((int*)(local->data->args))[1] = y;
        ((int*)(local->data->args))[2] = button;
        callClient(local,clickMousePlus);
    }
}

bool exp_isMouseButtonHeld(int button) {
    if (local) {
        *(int*)(local->data->args) = button;
        callClient(local,isMouseButtonHeld);
        return *(bool*)(local->data->args);
    } else return false;
}

void exp_sendKeys(char *str, int keywait, int keymodwait) {
    if (local) {
        ((int*)local->data->args)[0] = keywait;
        ((int*)local->data->args)[1] = keymodwait;
        strcpy((char*)local->data->args+sizeof(int)*2,str);
        callClient(local,sendKeys);
    }
}

void exp_holdKey(int code) {
    if (local) {
        *(int*)(local->data->args) = code;
        callClient(local,holdKey);
    }
}

void exp_releaseKey(int code) {
    if (local) {
        *(int*)(local->data->args) = code;
        callClient(local,releaseKey);
    }
}

bool exp_isKeyDown(int code) {
    if (local) {
        *(int*)(local->data->args) = code;
        callClient(local,isKeyDown);
        return *(bool*)(local->data->args);
    } else return false;
}

void* exp_getFieldObject(void* obj, char* path) {
    if (local) {
        ((void**)local->data->args)[0] = obj;
        strcpy((char*)local->data->args+sizeof(void*),path);
        callClient(local,getFieldObject);
        return ((void**)local->data->args)[0];
    } else return 0;
}

bool exp_isPathValid(void* obj, char* path) {
    if (local) {
        ((void**)local->data->args)[0] = obj;
        strcpy((char*)local->data->args+sizeof(void*),path);
        callClient(local,isPathValid);
        return ((bool*)local->data->args)[0];
    } else return false;
}

bool exp_getFieldBoolean(void* obj, char* path) {
    if (local) {
        ((void**)local->data->args)[0] = obj;
        strcpy((char*)local->data->args+sizeof(void*),path);
        callClient(local,getFieldBoolean);
        return ((bool*)local->data->args)[0];
    } else return false;
}

unsigned int exp_getFieldLongH(void* obj, char* path) {
    if (local) {
        ((void**)local->data->args)[0] = obj;
        strcpy((char*)local->data->args+sizeof(void*),path);
        callClient(local,getFieldLongH);
        return ((unsigned int*)local->data->args)[0];
    } else return -1;
}

unsigned int exp_getFieldLongL(void* obj, char* path) {
    if (local) {
        ((void**)local->data->args)[0] = obj;
        strcpy((char*)local->data->args+sizeof(void*),path);
        callClient(local,getFieldLongL);
        return ((unsigned int*)local->data->args)[0];
    } else return -1;
}

int exp_getFieldInt(void* obj, char* path) {
    if (local) {
        ((void**)local->data->args)[0] = obj;
        strcpy((char*)local->data->args+sizeof(void*),path);
        callClient(local,getFieldInt);
        return ((int*)local->data->args)[0];
    } else return -1;
}

int exp_getFieldShort(void* obj, char* path) {
    if (local) {
        ((void**)local->data->args)[0] = obj;
        strcpy((char*)local->data->args+sizeof(void*),path);
        callClient(local,getFieldShort);
        return ((int*)local->data->args)[0];
    } else return -1;
}

double exp_getFieldFloat(void* obj, char* path) {
    if (local) {
        ((void**)local->data->args)[0] = obj;
        strcpy((char*)local->data->args+sizeof(void*),path);
        callClient(local,getFieldFloat);
        return ((float*)local->data->args)[0];
    } else return -1;
}

double exp_getFieldDouble(void* obj, char* path) {
    if (local) {
        ((void**)local->data->args)[0] = obj;
        strcpy((char*)local->data->args+sizeof(void*),path);
        callClient(local,getFieldDouble);
        return ((double*)local->data->args)[0];
    } else return -1;
}

char exp_getFieldByte(void* obj, char* path) {
    if (local) {
        ((void**)local->data->args)[0] = obj;
        strcpy((char*)local->data->args+sizeof(void*),path);
        callClient(local,getFieldByte);
        return ((char*)local->data->args)[0];
    } else return -1;
}


SMARTClient* spawnFromString(char* initarg) {
    int len = strlen(initarg);
    char *path,*root,*params,*initseq,*useragent,*javaargs,*plugins;
    int width,height;
    char *buffer = new char[len+1];
    int ida=0,idb=0,idx;
    while (ida < len && (buffer[idb++]=initarg[ida++]) != ','); buffer[idb-1]=0;
    if (initarg[ida++] != '"') return NULL;
    int version = atoi(&buffer[0]);
    if (version == 0) {
        path = &buffer[idb];
        while (ida < len && (buffer[idb++]=initarg[ida++]) != '"'); buffer[idb-1]=0;
        if (initarg[ida++] != ',') return NULL;
        if (initarg[ida++] != '"') return NULL;
        root = &buffer[idb];
        while (ida < len && (buffer[idb++]=initarg[ida++]) != '"'); buffer[idb-1]=0;
        if (initarg[ida++] != ',') return NULL;
        if (initarg[ida++] != '"') return NULL;
        params = &buffer[idb];
        while (ida < len && (buffer[idb++]=initarg[ida++]) != '"'); buffer[idb-1]=0;
        if (initarg[ida++] != ',') return NULL;
        idx = idb;
        while (ida < len && (buffer[idb++]=initarg[ida++]) != ','); buffer[idb-1]=0;
        if (initarg[ida-1] != ',') return NULL;
        width = atoi(&buffer[idx]);
        idx = idb;
        while (ida < len && (buffer[idb++]=initarg[ida++]) != ','); buffer[idb-1]=0;
        if (initarg[ida-1] != ',') return NULL;
        height = atoi(&buffer[idx]);
        if (initarg[ida++] != '"') return NULL;
        initseq = &buffer[idb];
        while (ida < len && (buffer[idb++]=initarg[ida++]) != '"'); buffer[idb-1]=0;
        if (initarg[ida++] != ',') return NULL;
        if (initarg[ida++] != '"') return NULL;
        useragent = &buffer[idb];
        while (ida < len && (buffer[idb++]=initarg[ida++]) != '"'); buffer[idb-1]=0;
        if (initarg[ida++] != ',') return NULL;
        if (initarg[ida++] != '"') return NULL;
        javaargs = &buffer[idb];
        while (ida < len && (buffer[idb++]=initarg[ida++]) != '"'); buffer[idb-1]=0;
        if (initarg[ida++] != ',') return NULL;
        if (initarg[ida++] != '"') return NULL;
        plugins = &buffer[idb];
        while (ida < len && (buffer[idb++]=initarg[ida++]) != 0); buffer[idb-1]=0;
        if (initarg[ida++] != 0) return NULL;
        return spawnClient(path,root,params,width,height,initseq,useragent,javaargs,plugins);
    }
    return NULL;
}

Target EIOS_RequestTarget(char *initargs) {
    debug << "EIOS Requesting Target\n";
    if (initargs != 0 && strlen(initargs) > 0) {
        SMARTClient *client = spawnFromString(initargs); //This seems silly, could use exp_SpawnClient instead
        if (!client) client = pairClient(atoi(initargs));
        debug << "Target Identifier: " << client << '\n';
        return client;
    }
    return NULL; //This result signifies a failure
} 

void EIOS_ReleaseTarget(Target t) {
    debug << "EIOS Releasing Target\n";
    freeClient(t);
} 

void EIOS_GetTargetDimensions(Target t, int* width, int* height) {
    if (t) {
        *width = t->data->width;
        *height = t->data->height;
    }
} 

rgb* EIOS_GetImageBuffer(Target t) {
    debug << "EIOS requested image buffer\n";
    if (t) {
        debug << "Base: " << t->data << " Off: " << t->data->imgoff << '\n';
    }
    return t ? (rgb*)(((char*)t->data) + t->data->imgoff) : 0;
} 

void EIOS_UpdateImageBuffer(Target t) {
    return; //Do nothing, buffer is always updated
} 

void EIOS_GetMousePosition(Target t, int* x, int* y) {
    if (t) {
        callClient(t,getMousePos);
        *x = ((int*)(t->data->args))[0];
        *y = ((int*)(t->data->args))[1];
    }
} 

void EIOS_MoveMouse(Target t, int x, int y) {
    if (t) {
        ((int*)(t->data->args))[0] = x;
        ((int*)(t->data->args))[1] = y;
        callClient(t,moveMouse);
    }
} 

void EIOS_HoldMouse(Target t, int x, int y, int button) {
    if (t) {
        ((int*)(t->data->args))[0] = x;
        ((int*)(t->data->args))[1] = y;
        ((int*)(t->data->args))[2] = button;
        callClient(t,holdMousePlus);
    }
}

void EIOS_ReleaseMouse(Target t, int x, int y, int button) {
    if (t) {
        ((int*)(t->data->args))[0] = x;
        ((int*)(t->data->args))[1] = y;
        ((int*)(t->data->args))[2] = button;
        callClient(t,releaseMousePlus);
    }
}

bool EIOS_IsMouseHeld(Target t, int button) {
    if (t) {
        *(int*)(t->data->args) = button;
        callClient(t,isMouseButtonHeld);
        return *(bool*)(t->data->args);
    } else return false;
} 

void EIOS_SendString(Target t, char* str, int keywait, int keymodwait) {    
    if (t) {
        ((int*)t->data->args)[0] = keywait;
        ((int*)t->data->args)[1] = keymodwait;
        strcpy((char*)t->data->args+sizeof(int)*2,str);
        callClient(t,sendKeys);
    }
}

void EIOS_HoldKey(Target t, int key) {
    if (t) {
        *(int*)(t->data->args) = key;
        callClient(t,holdKey);
    }
}

void EIOS_ReleaseKey(Target t, int key) {
    if (t) {
        *(int*)(t->data->args) = key;
        callClient(t,releaseKey);
    }
}

bool EIOS_IsKeyHeld(Target t, int key) {
    if (t) {
        *(int*)(t->data->args) = key;
        callClient(t,isKeyDown);
        return *(bool*)(t->data->args);
    } else return false;
}

void internalConstructor() {
    clients.ids = 0;
    clients.count = exp_getClients(true);
    local = NULL;
    pairedClients = new map<int,SMARTClient*>();
}

void internalDestructor() {
    freeClient(local);
    if (clients.ids) delete clients.ids;
    delete pairedClients;
}


int GetFunctionCount() {
    return NumExports;
}

int GetPluginABIVersion() {
	return 2; //cdecl everything
}

/*int main(int argc, char **argv) {
    internalConstructor();
    
    debug << "Client Count: " << exp_getClients(false) << '\n';
    debug << "Unpaired Clients: " << exp_getClients(true) << '\n';
    
    debug << "Attempting to spawn...\n";
    int id = exp_spawnClient(".", "http://world37.runescape.com/", ",f68198595478491590", 765, 553, NULL, NULL, NULL);
    debug << "Spawned client " << id << " now sleeping...\n";
    sleep(5);
    debug << "Kill it with fire!\n";
    exp_killClient(id);
    
    internalDestructor();
}*/

#ifndef _WIN32

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

#else

HMODULE dllinst;

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
        case DLL_PROCESS_ATTACH: {
            dllinst = instance;
            WORD wVersionRequested;
            WSADATA wsaData;
            wVersionRequested = MAKEWORD(2, 2);
            WSAStartup(wVersionRequested, &wsaData);
            internalConstructor();
            return true;
        }
        case DLL_THREAD_ATTACH:
            return true;
        case DLL_PROCESS_DETACH:
            WSACleanup();
            internalDestructor();
            return true;
        case DLL_THREAD_DETACH:
            return true;
    }
    return false;
}

#endif
