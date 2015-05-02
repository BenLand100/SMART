#   Copyright 2006-2013 by Benjamin J. Land (a.k.a. BenLand100)
# 
#   This file is part of the SMART Minimizing Autoing Resource Thing (SMART)
# 
#   SMART is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
# 
#   SMART is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#   GNU General Public License for more details.
# 
#   You should have received a copy of the GNU General Public License
#   along with SMART. If not, see <http://www.gnu.org/licenses/>.

#these compilers work with crossdev on gentoo
WIN_GPP=i686-pc-mingw32-g++
WIN64_GPP=x86_64-pc-mingw32-g++
LIN_GPP=i686-pc-linux-gnu-g++
LIN64_GPP=x86_64-pc-linux-gnu-g++
JAVAC=javac
JAR=jar

DIST=dist
BUILD=build
WIN_NAME=libsmartremote32.dll
WIN64_NAME=libsmartremote64.dll
LIN_NAME=libsmartremote32.so
LIN64_NAME=libsmartremote64.so

JNI_WIN_NAME=libsmartjni32.dll
JNI_WIN64_NAME=libsmartjni64.dll
JNI_LIN_NAME=libsmartjni32.so
JNI_LIN64_NAME=libsmartjni64.so

JAVA_NAME=smart.jar

WIN_COMPILE_ARGS=-DWINDOWS -O3 -s -c
WIN64_COMPILE_ARGS=-DWINDOWS -O3 -s -c
LIN_COMPILE_ARGS=-fPIC -DLINUX -O3 -s -c
LIN64_COMPILE_ARGS=-fPIC -DLINUX -O3 -s -c

SRC_DIR=src
LIN_BUILD_DIR=$(BUILD)/linux32
LIN64_BUILD_DIR=$(BUILD)/linux64
WIN_BUILD_DIR=$(BUILD)/windows32
WIN64_BUILD_DIR=$(BUILD)/windows64
JAVA_BUILD_DIR=$(BUILD)/java

CPPSOURCEFILES= \
	$(SRC_DIR)/SmartRemote.cpp
	
JNI_CPPSOURCEFILES= \
	$(SRC_DIR)/SmartJNI.cpp
	
CPPHEADERFILES= \
	$(SRC_DIR)/SmartRemote.h \
	$(SRC_DIR)/libsmartremote.def

JNI_CPPHEADERFILES= \
    $(SRC_DIR)/jni.h \
	
WINOBJFILES= \
	$(WIN_BUILD_DIR)/SmartRemote.o
	 
JNI_WINOBJFILES= \
	$(WIN_BUILD_DIR)/SmartJNI.o 
	
WIN64OBJFILES= \
	$(WIN64_BUILD_DIR)/SmartRemote.o 
	
JNI_WIN64OBJFILES= \
	$(WIN64_BUILD_DIR)/SmartJNI.o 
	
LINOBJFILES= \
	$(LIN_BUILD_DIR)/SmartRemote.o 
	
JNI_LINOBJFILES= \
	$(LIN_BUILD_DIR)/SmartJNI.o 
	
LIN64OBJFILES= \
	$(LIN64_BUILD_DIR)/SmartRemote.o
	
JNI_LIN64OBJFILES= \
	$(LIN64_BUILD_DIR)/SmartJNI.o 
	
JAVASOURCES= \
    $(SRC_DIR)/Canvas.java \
    $(SRC_DIR)/Main.java \
    $(SRC_DIR)/BlockingEventQueue.java \
    $(SRC_DIR)/Client.java \
    $(SRC_DIR)/ClientStub.java \
    $(SRC_DIR)/EventNazi.java \
    $(SRC_DIR)/EventRedirect.java \
    $(SRC_DIR)/UnblockedEvent.java
    
JAVACLASSES= \
    $(JAVA_BUILD_DIR)/java/awt/Canvas.class \
    $(JAVA_BUILD_DIR)/smart/Main.class \
    $(JAVA_BUILD_DIR)/smart/BlockingEventQueue.class \
    $(JAVA_BUILD_DIR)/smart/Client.class \
    $(JAVA_BUILD_DIR)/smart/ClientStub.class \
    $(JAVA_BUILD_DIR)/smart/EventNazi.class \
    $(JAVA_BUILD_DIR)/smart/EventRedirect.class \
    $(JAVA_BUILD_DIR)/smart/UnblockedEvent.class

all:
	@echo "Syntax for the SMART makefile:"
	@echo "    For Windows distributions: make windows windows64"
	@echo "    For Linux distributions: make linux linux64"
	@echo "    For All distributions: make everything"
	@echo "    For test apps: make test-python test-linux test-linux64 test-windows test-windows64"
	@echo "    For all test apps: make test-all"
	@echo "    To clean build files: make clean"
	
everything: linux linux64 windows windows64

test-all: test-python test-windows test-windows64 test-linux test-linux64

linux: $(JAVACLASSES) $(DIST)/$(LIN_NAME) $(DIST)/$(JNI_LIN_NAME)
	@echo "Finished Building the Linux 32bit SMART distribution"
	
linux64: $(JAVACLASSES) $(DIST)/$(LIN64_NAME) $(DIST)/$(JNI_LIN64_NAME)
	@echo "Finished Building the Linux 64bit SMART distribution"

windows: $(JAVACLASSES) $(DIST)/$(WIN_NAME) $(DIST)/$(JNI_WIN_NAME)
	@echo "Finished Building the Windows 32bit SMART distribution"

windows64: $(JAVACLASSES) $(DIST)/$(WIN64_NAME) $(DIST)/$(JNI_WIN64_NAME)
	@echo "Finished Building the Windows 64bit SMART distribution"
	
test-linux:
	@mkdir -p $(DIST)
	@$(LIN_GPP) -I $(SRC_DIR) -fPIC -shared -s -o $(DIST)/example-plugin32.so test-apps/example-plugin.cpp
	@$(LIN_GPP) -ldl -o $(DIST)/test-plugin-load32 test-apps/test-plugin-load.cpp
	@$(LIN_GPP) -ldl -o $(DIST)/test-spawn32 test-apps/test-spawn.cpp
	@$(LIN_GPP) -ldl -o $(DIST)/test-eios32 test-apps/test-eios.cpp
	@$(LIN_GPP) -ldl -o $(DIST)/test-exports32 test-apps/test-exports.cpp
	@echo "Finished building Linux 32bit test files"
	
test-linux64:
	@mkdir -p $(DIST)
	@$(LIN64_GPP) -I $(SRC_DIR) -fPIC -shared -s -o $(DIST)/example-plugin64.so test-apps/example-plugin.cpp
	@$(LIN64_GPP) -ldl -o $(DIST)/test-plugin-load64 test-apps/test-plugin-load.cpp
	@$(LIN64_GPP) -ldl -o $(DIST)/test-spawn64 test-apps/test-spawn.cpp
	@$(LIN64_GPP) -ldl -o $(DIST)/test-eios64 test-apps/test-eios.cpp
	@$(LIN64_GPP) -ldl -o $(DIST)/test-exports64 test-apps/test-exports.cpp
	@echo "Finished building Linux 64bit test files"
	
test-windows: 
	@mkdir -p $(DIST)
	@$(WIN_GPP) -I $(SRC_DIR) -shared -static-libgcc -static-libstdc++ -s -o $(DIST)/example-plugin32.dll test-apps/example-plugin.cpp
	@$(WIN_GPP) -static -static-libgcc -static-libstdc++ -o $(DIST)/test-plugin-load32.exe test-apps/test-plugin-load.cpp
	@$(WIN_GPP) -static -static-libgcc -static-libstdc++ -o $(DIST)/test-spawn32.exe test-apps/test-spawn.cpp
	@$(WIN_GPP) -static -static-libgcc -static-libstdc++ -o $(DIST)/test-eios32.exe test-apps/test-eios.cpp
	@$(WIN_GPP) -static -static-libgcc -static-libstdc++ -o $(DIST)/test-exports32.exe test-apps/test-exports.cpp
	@echo "Finished building Windows 32bit test files"
	
test-windows64:
	@mkdir -p $(DIST)
	@$(WIN64_GPP) -I $(SRC_DIR) -shared -static-libgcc -static-libstdc++ -s -o $(DIST)/example-plugin64.dll test-apps/example-plugin.cpp
	@$(WIN64_GPP) -static -static-libgcc -static-libstdc++ -o $(DIST)/test-plugin-load64.exe test-apps/test-plugin-load.cpp
	@$(WIN64_GPP) -static -static-libgcc -static-libstdc++ -o $(DIST)/test-spawn64.exe test-apps/test-spawn.cpp
	@$(WIN64_GPP) -static -static-libgcc -static-libstdc++ -o $(DIST)/test-eios64.exe test-apps/test-eios.cpp
	@$(WIN64_GPP) -static -static-libgcc -static-libstdc++ -o $(DIST)/test-exports64.exe test-apps/test-exports.cpp
	@echo "Finished building Windows 64bit test files"
	
test-python:
	@mkdir -p $(DIST)
	@cp test-apps/test-python.py $(DIST)/test-python
	@echo "Finished building Python test files"
	
clean: 
	@echo "Cleaning build files..."
	@rm -rf $(BUILD) $(DIST)  
	
#### LINUX BUILDING DIRECTIVES ####

$(DIST)/$(LIN_NAME): $(LINOBJFILES)
	@echo "Linking Linux Remote object files..."
	@mkdir -p $(DIST)
	@$(LIN_GPP) -fPIC -shared -s -o $(DIST)/$(LIN_NAME) $(LINOBJFILES)
	
$(DIST)/$(JNI_LIN_NAME): $(JNI_LINOBJFILES)
	@echo "Linking Linux JNI object files..."
	@mkdir -p $(DIST)
	@$(LIN_GPP) -fPIC -shared -s -o $(DIST)/$(JNI_LIN_NAME) $(JNI_LINOBJFILES)

$(LIN_BUILD_DIR)/SmartRemote.o: $(SRC_DIR)/SmartRemote.cpp $(CPPHEADERFILES)
	@echo "Compiling SmartRemote.cpp"
	@mkdir -p $(LIN_BUILD_DIR)
	@$(LIN_GPP) $(LIN_COMPILE_ARGS) -o $(LIN_BUILD_DIR)/SmartRemote.o $(SRC_DIR)/SmartRemote.cpp
	
$(LIN_BUILD_DIR)/SmartJNI.o: $(SRC_DIR)/SmartJNI.cpp $(JNI_CPPHEADERFILES)
	@echo "Compiling SmartJNI.cpp"
	@mkdir -p $(LIN_BUILD_DIR)
	@$(LIN_GPP) $(LIN_COMPILE_ARGS) -o $(LIN_BUILD_DIR)/SmartJNI.o $(SRC_DIR)/SmartJNI.cpp

	
#### LINUX64 BUILDING DIRECTIVES ####

$(DIST)/$(LIN64_NAME): $(LIN64OBJFILES)
	@echo "Linking Linux64 Remote object files..."
	@mkdir -p $(DIST)
	@$(LIN64_GPP) -fPIC -shared -s -o $(DIST)/$(LIN64_NAME) $(LIN64OBJFILES)
	
$(DIST)/$(JNI_LIN64_NAME): $(JNI_LIN64OBJFILES)
	@echo "Linking Linux64 JNI object files..."
	@mkdir -p $(DIST)
	@$(LIN64_GPP) -fPIC -shared -s -o $(DIST)/$(JNI_LIN64_NAME) $(JNI_LIN64OBJFILES)

$(LIN64_BUILD_DIR)/SmartRemote.o: $(SRC_DIR)/SmartRemote.cpp $(CPPHEADERFILES)
	@echo "Compiling SmartRemote.cpp"
	@mkdir -p $(LIN64_BUILD_DIR)
	@$(LIN64_GPP) $(LIN64_COMPILE_ARGS) -o $(LIN64_BUILD_DIR)/SmartRemote.o $(SRC_DIR)/SmartRemote.cpp
	
$(LIN64_BUILD_DIR)/SmartJNI.o: $(SRC_DIR)/SmartJNI.cpp $(JNI_CPPHEADERFILES)
	@echo "Compiling SmartJNI.cpp"
	@mkdir -p $(LIN64_BUILD_DIR)
	@$(LIN64_GPP) $(LIN64_COMPILE_ARGS) -o $(LIN64_BUILD_DIR)/SmartJNI.o $(SRC_DIR)/SmartJNI.cpp


#### WINDOWS BUILDING DIRECTIVES ####

$(DIST)/$(WIN_NAME): $(WINOBJFILES)
	@echo "Linking Windows Remote object files..."
	@mkdir -p $(DIST)
	@$(WIN_GPP) -Wl,$(SRC_DIR)/libsmartremote.def -static -static-libgcc -static-libstdc++ -mwindows -shared -s -o $(DIST)/$(WIN_NAME) $(WINOBJFILES) -lws2_32
	
$(DIST)/$(JNI_WIN_NAME): $(JNI_WINOBJFILES)
	@echo "Linking Windows JNI object files..."
	@mkdir -p $(DIST)
	@$(WIN_GPP) -Wl,$(SRC_DIR)/libsmartjni.def -static -static-libgcc -static-libstdc++ -mwindows -shared -s -o $(DIST)/$(JNI_WIN_NAME) $(JNI_WINOBJFILES)

$(WIN_BUILD_DIR)/SmartRemote.o: $(SRC_DIR)/SmartRemote.cpp $(CPPHEADERFILES)
	@echo "Compiling SmartRemote.cpp"
	@mkdir -p $(WIN_BUILD_DIR)
	@$(WIN_GPP) $(WIN_COMPILE_ARGS) -o $(WIN_BUILD_DIR)/SmartRemote.o $(SRC_DIR)/SmartRemote.cpp -lws2_32
	
$(WIN_BUILD_DIR)/SmartJNI.o: $(SRC_DIR)/SmartJNI.cpp $(JNI_CPPHEADERFILES)
	@echo "Compiling SmartJNI.cpp"
	@mkdir -p $(WIN_BUILD_DIR)
	@$(WIN_GPP) $(WIN_COMPILE_ARGS) -o $(WIN_BUILD_DIR)/SmartJNI.o $(SRC_DIR)/SmartJNI.cpp

#### WINDOWS64 BUILDING DIRECTIVES ####

$(DIST)/$(WIN64_NAME): $(WIN64OBJFILES)
	@echo "Linking Windows64 Remote object files..."
	@mkdir -p $(DIST)
	@$(WIN64_GPP) -Wl,$(SRC_DIR)/libsmartremote.def -static -static-libgcc -static-libstdc++ -mwindows -shared -s -o $(DIST)/$(WIN64_NAME) $(WIN64OBJFILES) -lws2_32

$(DIST)/$(JNI_WIN64_NAME): $(JNI_WIN64OBJFILES)
	@echo "Linking Windows64 JNI object files..."
	@mkdir -p $(DIST)
	@$(WIN64_GPP) -Wl,$(SRC_DIR)/libsmartjni.def -static -static-libgcc -static-libstdc++ -mwindows -shared -s -o $(DIST)/$(JNI_WIN64_NAME) $(JNI_WIN64OBJFILES)

$(WIN64_BUILD_DIR)/SmartRemote.o: $(SRC_DIR)/SmartRemote.cpp $(CPPHEADERFILES)
	@echo "Compiling SmartRemote.cpp"
	@mkdir -p $(WIN64_BUILD_DIR)
	@$(WIN64_GPP) $(WIN64_COMPILE_ARGS) -o $(WIN64_BUILD_DIR)/SmartRemote.o $(SRC_DIR)/SmartRemote.cpp -lws2_32
	
$(WIN64_BUILD_DIR)/SmartJNI.o: $(SRC_DIR)/SmartJNI.cpp $(JNI_CPPHEADERFILES)
	@echo "Compiling SmartJNI.cpp"
	@mkdir -p $(WIN64_BUILD_DIR)
	@$(WIN64_GPP) $(WIN64_COMPILE_ARGS) -o $(WIN64_BUILD_DIR)/SmartJNI.o $(SRC_DIR)/SmartJNI.cpp


#### JAVA BUILDING DIRECTIVES ####

 $(JAVACLASSES): $(JAVASOURCES)
	@echo "Compiling Java Classes..."
	@mkdir -p $(JAVA_BUILD_DIR)
	@$(JAVAC) -classpath $(JAVA_BUILD_DIR) -sourcepath $(SRC_DIR) -d $(JAVA_BUILD_DIR) $(JAVASOURCES)
	@echo "Creating JAR Archive..."
	@mkdir -p $(DIST)
	@$(JAR) cfe $(DIST)/$(JAVA_NAME) smart.Main -C $(JAVA_BUILD_DIR) .


