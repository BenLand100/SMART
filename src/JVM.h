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
 
#ifndef _JVM_H
#define	_JVM_H

#include "jni.h"

extern char* jvmpath;
extern int maxmem;
extern "C" void setJVMPath(char* path);
extern "C" void setMaxJVMMem(int maxmem);

typedef jint (JNICALL *CreateJVM)(JavaVM**, void**, void*);
typedef jint (JNICALL *GetJVMs)(JavaVM**,jsize,jsize*);

bool initJVM(JNIEnv**, JavaVM**, void*);
void* findJVM();

#endif	/* _JVM_H */

