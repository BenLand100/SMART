/**
 *  Copyright 2006-2013 by Benjamin J. Land (a.k.a. BenLand100)
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

#include "SmartRemote.h"
#include <time.h>
#include <stdio.h>
#include <cstring>
#include <string>
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
static clients_dat clients;

/**
 * Cleans up memory associated with the client if there are no more references
 * to that client.
 */
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
    struct sockaddr_in sockaddr;
    memset((char *) &sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    memcpy((char *)&sockaddr.sin_addr.s_addr,(char *)localhost->h_addr, localhost->h_length);
    sockaddr.sin_port = htons(client->data->port);
    debug << "Attempting to connect to localhost:" << client->data->port << "\n";
    int res;
    if ((res=connect(client->socket,(struct sockaddr *) &sockaddr,sizeof(sockaddr))) < 0) {
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
 * Get all the stale SHM files and killem.
 */
void cleanupSHM() {    
    char buff[512];
    int count = exp_getClients(false);
    for (int i = 0; i < count; ++i) {
        sprintf(buff,"SMART.%i",exp_getAvailablePID(i));
        remove(buff);
    }
}

/**
 * Pairing logic: 
 * 1) Try to open the file SMART.[pid]
 * 2) If file does not exist, abort
 * 3) Read the shm_data structure at the beginning of the file
 * 5) If data->paired is nonzero, assume paired and abort
 * 6) Remap file with proper size of image and debug included
 * 7) Set data->paired to our TID
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
            return NULL;
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
SMARTClient* spawnClient(char *java_exec, char* remote_path, char *root, char *params, int width, int height, char *initseq, char *useragent, char* javaargs, char* plugins) {
    cleanupSHM();
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
    #ifdef _WIN32
    char bootclasspath[512];
    sprintf(bootclasspath,"-Xbootclasspath/p:\"%s/%s\"",remote_path,"smart.jar");
    char library[512];
    sprintf(library,"%s/libsmartjni%s.dll",remote_path,bits);
    int len = strlen(javaargs)+strlen(bootclasspath)+strlen(library)+strlen(root)+strlen(params)+strlen(_width)+strlen(_height)+strlen(initseq)+strlen(useragent)+strlen(remote_path)+strlen(plugins)+7*3+50; //A little extra
    char *args = new char[len];
    sprintf(args,"%s %s smart.Main \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"",javaargs,bootclasspath,library,root,params,_width,_height,initseq,useragent, remote_path, plugins);
	SHELLEXECUTEINFO info;
    memset(&info, 0, sizeof(SHELLEXECUTEINFO));
    info.cbSize = sizeof(SHELLEXECUTEINFO); 
    info.fMask = SEE_MASK_NOCLOSEPROCESS;
    info.lpFile = java_exec;
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
    char bootclasspath[512];
    sprintf(bootclasspath,"-Xbootclasspath/p:%s/%s",remote_path,"smart.jar"); //linux supports spaces in path fine, fails with quotes
    char library[512];
    #ifdef __APPLE__
    sprintf(library,"%s/libsmartjni%s.dylib",remote_path,bits);
    #else
    sprintf(library,"%s/libsmartjni%s.so",remote_path,bits);
    #endif
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
        execlp(java_exec,java_exec,bootclasspath,"smart.Main",library,root,params,_width,_height,initseq,useragent, remote_path, plugins, NULL);
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
        int pid = atoi(name+6);
        if ((only_unpaired && temp->paired) || temp->id != pid) {
            UnmapViewOfFile(temp);
            CloseHandle(memmap);
            CloseHandle(file);
            if (!FindNextFile(hfind,&find)) {
                FindClose(hfind);
                break;
            }
            continue;
        }
        count++;
        if (_clients) {
            *_clients = (int*)realloc(*_clients,count*sizeof(int));
            (*_clients)[count-1] = pid;
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
    debug << "Located " << count << " clients\n";
    return count;
}

/**
 * Searches for all clients avaliable or only_unpaired clients, and returns the
 * number found.
 */
int exp_getClients(bool only_unpaired) {
    if (clients.ids) delete clients.ids;
    clients.ids = 0;
    return clients.count = getClients(only_unpaired,&clients.ids);
}

/**
 * Returns the PID of the client at index idx (after calling exp_getClients())
 */
int exp_getAvailablePID(int idx) {
    if (idx < clients.count && idx >= 0) {
        debug << "Returning client index " << idx << " with PID=" << clients.ids[idx] << '\n';
        return clients.ids[idx];
    }
    return 0;
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
 * Creates a remote SMART client and pairs to it. Returns the target to be used
 * in target-specific methods, or NULL on failure.
 */
Target exp_spawnClient(char* java_exec, char* remote_path, char *root, char *params, int width, int height, char *initseq, char *useragent, char* javaargs, char* plugins) {
    return spawnClient(java_exec, remote_path,root,params,width,height,initseq,useragent,javaargs,plugins);
}

/**
 * Pairs to an existing client of the given PID. Returns the target to be used
 * in target-specific methods, or NULL on failure.
 */
Target exp_pairClient(int pid) {
    return pairClient(pid);
}

/**
 * Releases this reference to the target.
 */
int exp_getClientPID(Target t) {
    return t ? t->data->id : 0;
}

/**
 * Releases this reference to the target.
 */
void exp_freeClient(Target t) {
    freeClient(t);
}

//Returns a pointer into shared memory where the image resides
void* exp_getImageArray(Target t) {
    return t ? (char*)t->data + t->data->imgoff : 0;
}

//Returns a pointer into shared memory where the debug image resides
void* exp_getDebugArray(Target t) {
    return t ? (char*)t->data + t->data->dbgoff : 0;
}

int exp_getRefresh(Target t) {
    if (t) {
        callClient(t,getRefresh);
        return *(int*)(t->data->args);
    } return -1;
}

int exp_getManifestHash(Target t) {
    if (t) {
        callClient(t,getManifestHash);
        return *(int*)(t->data->args);
    } return -1;
}

void exp_setRefresh(Target t, int x) {
    if (t) {
        *(int*)(t->data->args) = x;
        callClient(t,setRefresh);
    }
}

void exp_setTransparentColor(Target t, int color) {
    if (t) {
        *(int*)(t->data->args) = color;
        callClient(t,setTransparentColor);
    }
}

void exp_setDebug(Target t, bool enabled) {
    if (t) {
        *(bool*)(t->data->args) = enabled;
        callClient(t,setDebug);
    }
}

void exp_setCapture(Target t, bool enabled) {
    if (t) {
        *(bool*)(t->data->args) = enabled;
        callClient(t,setCapture);
    }
}

void exp_setNativeBtn(Target t, int plugid, int btnid, bool state) {
    if (t) {
        ((int*)(t->data->args))[0] = plugid;
        ((int*)(t->data->args))[1] = btnid;
        ((int*)(t->data->args))[2] = state;
        callClient(t,setNativeBtn);
    }
}

void exp_setGraphics(Target t, bool enabled) {
    if (t) {
        *(bool*)(t->data->args) = enabled;
        callClient(t,setGraphics);
    }
}

void exp_setEnabled(Target t, bool enabled) {
    if (t) {
        *(bool*)(t->data->args) = enabled;
        callClient(t,setEnabled);
    }
}

bool exp_isActive(Target t) {
    if (t) {
        callClient(t,isActive);
        return *(bool*)(t->data->args);
    } else return false;
}

bool exp_isBlocking(Target t) {
    if (t) {
        callClient(t,isBlocking);
        return *(bool*)(t->data->args);
    } else return false;
}

void exp_getMousePos(Target t, int &x, int &y) {
    if (t) {
        callClient(t,getMousePos);
        x = ((int*)(t->data->args))[0];
        y = ((int*)(t->data->args))[1];
    }
}

void exp_holdMouse(Target t, int x, int y, bool left) {
    if (t) {
        ((int*)(t->data->args))[0] = x;
        ((int*)(t->data->args))[1] = y;
        ((int*)(t->data->args))[2] = left; //not a mistake
        callClient(t,holdMouse);
    }
}

void exp_releaseMouse(Target t, int x, int y, bool left) {
    if (t) {
        ((int*)(t->data->args))[0] = x;
        ((int*)(t->data->args))[1] = y;
        ((int*)(t->data->args))[2] = left; //not a mistake
        callClient(t,releaseMouse);
    }
}

void exp_holdMousePlus(Target t, int x, int y, int button) {
    if (t) {
        ((int*)(t->data->args))[0] = x;
        ((int*)(t->data->args))[1] = y;
        ((int*)(t->data->args))[2] = button;
        callClient(t,holdMousePlus);
    }
}

void exp_releaseMousePlus(Target t, int x, int y, int button) {
    if (t) {
        ((int*)(t->data->args))[0] = x;
        ((int*)(t->data->args))[1] = y;
        ((int*)(t->data->args))[2] = button;
        callClient(t,releaseMousePlus);
    }
}

void exp_moveMouse(Target t, int x, int y) {
    if (t) {
        ((int*)(t->data->args))[0] = x;
        ((int*)(t->data->args))[1] = y;
        callClient(t,moveMouse);
    }
}

void exp_windMouse(Target t, int x, int y) {
    if (t) {
        ((int*)(t->data->args))[0] = x;
        ((int*)(t->data->args))[1] = y;
        callClient(t,windMouse);
    }
}

void exp_clickMouse(Target t, int x, int y, bool left) {
    if (t) {
        ((int*)(t->data->args))[0] = x;
        ((int*)(t->data->args))[1] = y;
        ((int*)(t->data->args))[2] = left; //not a mistake
        callClient(t,clickMouse);
    }
}

void exp_clickMousePlus(Target t, int x, int y, int button) {
    if (t) {
        ((int*)(t->data->args))[0] = x;
        ((int*)(t->data->args))[1] = y;
        ((int*)(t->data->args))[2] = button;
        callClient(t,clickMousePlus);
    }
}

bool exp_isMouseButtonHeld(Target t, int button) {
    if (t) {
        *(int*)(t->data->args) = button;
        callClient(t,isMouseButtonHeld);
        return *(bool*)(t->data->args);
    } else return false;
}

void exp_scrollMouse(Target t, int x, int y, int lines) {
        if (t) {
        ((int*)(t->data->args))[0] = x;
        ((int*)(t->data->args))[1] = y;
        ((int*)(t->data->args))[2] = lines;
        callClient(t,scrollMouse);
    }
}

void exp_sendKeys(Target t, char *str, int keywait, int keymodwait) {
    if (t) {
        ((int*)t->data->args)[0] = keywait;
        ((int*)t->data->args)[1] = keymodwait;
        strcpy((char*)t->data->args+sizeof(int)*2,str);
        callClient(t,sendKeys);
    }
}

void exp_holdKey(Target t, int code) {
    if (t) {
        *(int*)(t->data->args) = code;
        callClient(t,holdKey);
    }
}

void exp_releaseKey(Target t, int code) {
    if (t) {
        *(int*)(t->data->args) = code;
        callClient(t,releaseKey);
    }
}

bool exp_isKeyDown(Target t, int code) {
    if (t) {
        *(int*)(t->data->args) = code;
        callClient(t,isKeyDown);
        return *(bool*)(t->data->args);
    } else return false;
}

void* exp_getFieldObject(Target t, void* obj, char* path) {
    if (t) {
        ((void**)t->data->args)[0] = obj;
        strcpy((char*)t->data->args+sizeof(void*),path);
        callClient(t,getFieldObject);
        return ((void**)t->data->args)[0];
    } else return 0;
}

bool exp_isPathValid(Target t, void* obj, char* path) {
    if (t) {
        ((void**)t->data->args)[0] = obj;
        strcpy((char*)t->data->args+sizeof(void*),path);
        callClient(t,isPathValid);
        return ((bool*)t->data->args)[0];
    } else return false;
}

bool exp_getFieldBoolean(Target t, void* obj, char* path) {
    if (t) {
        ((void**)t->data->args)[0] = obj;
        strcpy((char*)t->data->args+sizeof(void*),path);
        callClient(t,getFieldBoolean);
        return ((bool*)t->data->args)[0];
    } else return false;
}

unsigned int exp_getFieldLongH(Target t, void* obj, char* path) {
    if (t) {
        ((void**)t->data->args)[0] = obj;
        strcpy((char*)t->data->args+sizeof(void*),path);
        callClient(t,getFieldLongH);
        return ((unsigned int*)t->data->args)[0];
    } else return -1;
}

unsigned int exp_getFieldLongL(Target t, void* obj, char* path) {
    if (t) {
        ((void**)t->data->args)[0] = obj;
        strcpy((char*)t->data->args+sizeof(void*),path);
        callClient(t,getFieldLongL);
        return ((unsigned int*)t->data->args)[0];
    } else return -1;
}

int exp_getFieldInt(Target t, void* obj, char* path) {
    if (t) {
        ((void**)t->data->args)[0] = obj;
        strcpy((char*)t->data->args+sizeof(void*),path);
        callClient(t,getFieldInt);
        return ((int*)t->data->args)[0];
    } else return -1;
}

int exp_getFieldShort(Target t, void* obj, char* path) {
    if (t) {
        ((void**)t->data->args)[0] = obj;
        strcpy((char*)t->data->args+sizeof(void*),path);
        callClient(t,getFieldShort);
        return ((int*)t->data->args)[0];
    } else return -1;
}

double exp_getFieldFloat(Target t, void* obj, char* path) {
    if (t) {
        ((void**)t->data->args)[0] = obj;
        strcpy((char*)t->data->args+sizeof(void*),path);
        callClient(t,getFieldFloat);
        return ((float*)t->data->args)[0];
    } else return -1;
}

double exp_getFieldDouble(Target t, void* obj, char* path) {
    if (t) {
        ((void**)t->data->args)[0] = obj;
        strcpy((char*)t->data->args+sizeof(void*),path);
        callClient(t,getFieldDouble);
        return ((double*)t->data->args)[0];
    } else return -1;
}

int exp_getFieldByte(Target t, void* obj, char* path) {
    if (t) {
        ((void**)t->data->args)[0] = obj;
        strcpy((char*)t->data->args+sizeof(void*),path);
        callClient(t,getFieldByte);
        return ((char*)t->data->args)[0];
    } else return -1;
}

void* exp_getFieldArray3DObject(Target t, void* obj, char* path, int x, int y, int z) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		idx[2] = z;
		callClient(t, getFieldArray3DObject);
		return ((void**)t->data->args)[0];
	}  else return 0;
}

int exp_getFieldArray3DByte(Target t, void* obj, char* path, int x, int y, int z) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		idx[2] = z;
		callClient(t, getFieldArray3DByte);
		return ((int*)t->data->args)[0];
	} else return -1;
}

int exp_getFieldArray3DChar(Target t, void* obj, char* path, int x, int y, int z) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		idx[2] = z;
		callClient(t, getFieldArray3DChar);
		return ((int*)t->data->args)[0];
	} else return -1;
}

int exp_getFieldArray3DShort(Target t, void* obj, char* path, int x, int y, int z) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		idx[2] = z;
		callClient(t, getFieldArray3DShort);
		return ((int*)t->data->args)[0];
	} else return -1;
}

int exp_getFieldArray3DInt(Target t, void* obj, char* path, int x, int y, int z) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		idx[2] = z;
		callClient(t, getFieldArray3DInt);
		return ((int*)t->data->args)[0];
	} else return -1;
}

float exp_getFieldArray3DFloat(Target t, void* obj, char* path, int x, int y, int z) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		idx[2] = z;
		callClient(t, getFieldArray3DFloat);
		return ((float*)t->data->args)[0];
	} else return -1;
}

float exp_getFieldArray3DDouble(Target t, void* obj, char* path, int x, int y, int z) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		idx[2] = z;
		callClient(t, getFieldArray3DDouble);
		return ((float*)t->data->args)[0];
	} else return -1;
}

bool exp_getFieldArray3DBool(Target t, void* obj, char* path, int x, int y, int z) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		idx[2] = z;
		callClient(t, getFieldArray3DBoolean);
		return ((bool*)t->data->args)[0];
	} else return false;
}

int exp_getFieldArray3DLongH(Target t, void* obj, char* path, int x, int y, int z) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		idx[2] = z;
		callClient(t, getFieldArray3DLongH);
		return ((int*)t->data->args)[0];
	} else return -1;
}

int exp_getFieldArray3DLongL(Target t, void* obj, char* path, int x, int y, int z) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		idx[2] = z;
		callClient(t, getFieldArray3DLongL);
		return ((int*)t->data->args)[0];
	} else return -1;
}

void* exp_getFieldArray2DObject(Target t, void* obj, char* path, int x, int y) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		callClient(t, getFieldArray2DObject);
		return ((void**)t->data->args)[0];
	} else return 0;
}

int exp_getFieldArray2DInt(Target t, void* obj, char* path, int x, int y) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		callClient(t, getFieldArray2DInt);
		return ((int*)t->data->args)[0];
	} else return -1;
}

float exp_getFieldArray2DDouble(Target t, void* obj, char* path, int x, int y) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		callClient(t, getFieldArray2DDouble);
		return ((float*)t->data->args)[0];
	} else return -1;
}

float exp_getFieldArray2DFloat(Target t, void* obj, char* path, int x, int y) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		callClient(t, getFieldArray2DFloat);
		return ((float*)t->data->args)[0];
	} else return -1;
}

bool exp_getFieldArray2DBool(Target t, void* obj, char* path, int x, int y) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		callClient(t, getFieldArray2DBoolean);
		return ((bool*)t->data->args)[0];
	} else return false;
}

int exp_getFieldArray2DLongH(Target t, void* obj, char* path, int x, int y) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		callClient(t, getFieldArray2DLongH);
		return ((int*)t->data->args)[0];
	} else return -1;
}

int exp_getFieldArray2DLongL(Target t, void* obj, char* path, int x, int y) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		callClient(t, getFieldArray2DLongL);
		return ((int*)t->data->args)[0];
	} else return -1;
}

int exp_getFieldArray2DByte(Target t, void* obj, char* path, int x, int y) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		callClient(t, getFieldArray2DByte);
		return ((int*)t->data->args)[0];
	} else return -1;
}

int exp_getFieldArray2DChar(Target t, void* obj, char* path, int x, int y) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		callClient(t, getFieldArray2DChar);
		return ((int*)t->data->args)[0];
	} else return -1;
}

int exp_getFieldArray2DShort(Target t, void* obj, char* path, int x, int y) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		int* idx = (int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1);
		idx[0] = x;
		idx[1] = y;
		callClient(t, getFieldArray2DShort);
		return ((int*)t->data->args)[0];
	} else return -1;
}

void* exp_getFieldArrayObject(Target t, void* obj, char* path, int index) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		((int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1))[0] = index;
		callClient(t, getFieldArray1DObject);
		return ((void**)t->data->args)[0];
	} else return 0;
}

int exp_getFieldArrayInt(Target t, void* obj, char* path, int index) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		((int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1))[0] = index;
		callClient(t, getFieldArray1DInt);
		return ((int*)t->data->args)[0];
	} else return -1;
}

float exp_getFieldArrayFloat(Target t, void* obj, char* path, int index) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		((int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1))[0] = index;
		callClient(t, getFieldArray1DFloat);
		return ((float*)t->data->args)[0];
	} else return -1;
}

float exp_getFieldArrayDouble(Target t, void* obj, char* path, int index) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		((int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1))[0] = index;
		callClient(t, getFieldArray1DDouble);
		return ((float*)t->data->args)[0];
	} else return -1;
}

bool exp_getFieldArrayBool(Target t, void* obj, char* path, int index) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		((int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1))[0] = index;
		callClient(t, getFieldArray1DBoolean);
		return ((bool*)t->data->args)[0];
	} else return false;
}

int exp_getFieldArrayLongH(Target t, void* obj, char* path, int index) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		((int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1))[0] = index;
		callClient(t, getFieldArray1DLongH);
		return ((int*)t->data->args)[0];
	} else return -1;
}

int exp_getFieldArrayLongL(Target t, void* obj, char* path, int index) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		((int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1))[0] = index;
		callClient(t, getFieldArray1DLongL);
		return ((int*)t->data->args)[0];
	} else return -1;
}

int exp_getFieldArrayByte(Target t, void* obj, char* path, int index) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		((int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1))[0] = index;
		callClient(t, getFieldArray1DByte);
		return ((int*)t->data->args)[0];
	} else return -1;
}

int exp_getFieldArrayShort(Target t, void* obj, char* path, int index) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		((int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1))[0] = index;
		callClient(t, getFieldArray1DShort);
		return ((int*)t->data->args)[0];
	} else return -1;
}

int exp_getFieldArrayChar(Target t, void* obj, char* path, int index) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		((int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1))[0] = index;
		callClient(t, getFieldArray1DChar);
		return ((int*)t->data->args)[0];
	} else return -1;
}

int exp_getFieldArraySize(Target t, void* obj, char* path, int dim) {
	if (t) {
		((void**)t->data->args)[0] = obj;
		strcpy((char*)t->data->args+sizeof(void*),path);
		((int*)(((char*)t->data->args) + sizeof(void*) + strlen(path) + 1))[0] = dim;
		callClient(t, getFieldArraySize);
		return ((int*)t->data->args)[0];
	} else return -1;
}

void exp_freeObject(Target t, void* obj) {
    if (t) {
        ((void**)t->data->args)[0] = obj;
        callClient(t,freeObject);
    }
}

bool exp_isNull(Target t, void* obj) {
    if (t) {
        ((void**)t->data->args)[0] = obj;
        callClient(t,isNull);
        return ((int*)t->data->args)[0] ? true : false;
    } else return false;
}

bool exp_isEqual(Target t, void* a, void* b) {
    if (t) {
        ((void**)t->data->args)[0] = a;
        ((void**)t->data->args)[1] = b;
        callClient(t,isEqual);
        return ((int*)t->data->args)[0] ? true : false;
    } else return false;
}

int exp_stringFromString(Target t, void* obj, char* delphistr) {
    if (t) {
        ((void**)t->data->args)[0] = obj;
        callClient(t,stringFromString);
        if (delphistr) strcpy(delphistr, (char*)t->data->args);
        return strlen((char*)t->data->args);
    } else return -1;
}

int exp_stringFromChars(Target t, void* obj, char* delphistr) {
    if (t) {
        ((void**)t->data->args)[0] = obj;
        callClient(t,stringFromChars);
        if (delphistr) strcpy(delphistr, (char*)t->data->args);
        return strlen((char*)t->data->args);
    } else return -1;
}

int exp_stringFromBytes(Target t, void* obj, char* delphistr) {
    if (t) {
        ((void**)t->data->args)[0] = obj;
        callClient(t,stringFromBytes);
        if (delphistr) strcpy(delphistr, (char*)t->data->args);
        return strlen((char*)t->data->args);
    } else return -1;
}

Target EIOS_RequestTarget(char *initargs) {
    debug << "EIOS Requesting Target\n";
    if (initargs != 0 && strlen(initargs) > 0) {
        SMARTClient *client = pairClient(atoi(initargs));
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

void EIOS_ScrollMouse(Target t, int x, int y, int lines) {
	if (t) {
		((int*)(t->data->args))[0] = x;
		((int*)(t->data->args))[1] = y;
		((int*)(t->data->args))[2] = lines;
		callClient(t,scrollMouse);
	}
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
    cleanupSHM();
    clients.count = exp_getClients(true);
    pairedClients = new map<int,SMARTClient*>();
}

void internalDestructor() {
    cleanupSHM();
    if (clients.ids) delete clients.ids;
    delete pairedClients;
}


int GetFunctionCount() {
    return NumExports;
}

int GetPluginABIVersion() {
	return 2; //cdecl everything
}

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
