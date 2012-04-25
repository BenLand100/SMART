#   Copyright 2010 by Benjamin J. Land (a.k.a. BenLand100)
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
WIN64_GPP=x86_64-w64-mingw32-g++
LIN_GPP=i686-pc-linux-gnu-g++
LIN64_GPP=x86_64-pc-linux-gnu-g++
JAVAC=javac
JAVA=java

DIST=dist
BUILD=build
REMOTE_NAME=smartremote
LOCAL_NAME=libsmartremote
WIN_LIB_EXT=.dll
LIN_LIB_EXT=.so
WIN_EXE_EXT=.exe
LIN_EXE_EXT=

WIN_COMPILE_ARGS=-Wall -O3 -s -c
WIN64_COMPILE_ARGS=-Wall -O3 -s -c
LIN_COMPILE_ARGS=-fPIC -Wall -O3 -s -c
LIN64_COMPILE_ARGS=-fPIC -Wall -O3 -s -c

SRC_DIR=src
LIN_BUILD_DIR=$(BUILD)/linux32
LIN64_BUILD_DIR=$(BUILD)/linux64
WIN_BUILD_DIR=$(BUILD)/windows32
WIN64_BUILD_DIR=$(BUILD)/windows64
	
LOCALHEADERFILES= \
	$(SRC_DIR)/Local.h \
	$(SRC_DIR)/Bridge.h \

LOCALWINOBJFILES= \
	$(WIN_BUILD_DIR)/Local.o \
	$(SRC_DIR)/libsmartremote.def
	
LOCALWIN64OBJFILES= \
	$(WIN64_BUILD_DIR)/Local.o \
	$(SRC_DIR)/libsmartremote.def

LOCALLINOBJFILES= \
	$(LIN_BUILD_DIR)/Local.o
	
LOCALLIN64OBJFILES= \
	$(LIN64_BUILD_DIR)/Local.o
	
REMOTEHEADERFILES= \
	$(SRC_DIR)/Remote.h \
	$(SRC_DIR)/Bridge.h \

REMOTEWINOBJFILES= \
	$(WIN_BUILD_DIR)/Remote.o
	
REMOTEWIN64OBJFILES= \
	$(WIN64_BUILD_DIR)/Remote.o 

REMOTELINOBJFILES= \
	$(LIN_BUILD_DIR)/Remote.o
	
REMOTELIN64OBJFILES= \
	$(LIN64_BUILD_DIR)/Remote.o
	

all:
	@echo "Syntax for SMART makefile:\n    For Windows distributions: make windows windows64\n    For Linux distributions: make linux linux64\n   For All distributions: make everything\n    To clean build files: make clean"
	
everything: linux linux64 windows windows64

linux: $(DIST)/$(LOCAL_NAME)32$(LIN_LIB_EXT) $(DIST)/$(REMOTE_NAME)32$(LIN_EXE_EXT)
	@echo "Finished Building the Linux 32bit distribution"
	
linux64: $(DIST)/$(LOCAL_NAME)64$(LIN_LIB_EXT)  $(DIST)/$(REMOTE_NAME)64$(LIN_EXE_EXT)
	@echo "Finished Building the Linux 64bit distribution"

windows: $(DIST)/$(LOCAL_NAME)32$(WIN_LIB_EXT) $(DIST)/$(REMOTE_NAME)32$(WIN_EXE_EXT)
	@echo "Finished Building the Windows 32bit distribution"

windows64: $(DIST)/$(LOCAL_NAME)64$(WIN_LIB_EXT) $(DIST)/$(REMOTE_NAME)64$(WIN_EXE_EXT)
	@echo "Finished Building the Windows 64bit distribution"
	
clean: 
	@echo "Cleaning build files..."
	@rm -rf $(BUILD) $(DIST)
	
#### LINUX BUILDING DIRECTIVES ####

$(DIST)/$(LOCAL_NAME)32$(LIN_LIB_EXT): $(LOCALLINOBJFILES)
	@echo "Linking object files..."
	@mkdir -p $(DIST)
	@$(LIN_GPP) -fPIC -shared -s -o $(DIST)/$(LOCAL_NAME)32$(LIN_LIB_EXT) $(LOCALLINOBJFILES)

$(LIN_BUILD_DIR)/Local.o: $(SRC_DIR)/Local.cpp $(LOCALHEADERFILES)
	@echo "Compiling Local.cpp"
	@mkdir -p $(LIN_BUILD_DIR)
	@$(LIN_GPP) $(LIN_COMPILE_ARGS) -o $(LIN_BUILD_DIR)/Local.o $(SRC_DIR)/Local.cpp

$(DIST)/$(REMOTE_NAME)32$(LIN_EXE_EXT): $(REMOTELINOBJFILES)
	@echo "Linking object files..."
	@mkdir -p $(DIST)
	@$(LIN_GPP) -fPIC -ldl -s -o $(DIST)/$(REMOTE_NAME)32$(LIN_EXE_EXT) $(REMOTELINOBJFILES)

$(LIN_BUILD_DIR)/Remote.o: $(SRC_DIR)/Remote.cpp $(REMOTEHEADERFILES)
	@echo "Compiling Remote.cpp"
	@mkdir -p $(LIN_BUILD_DIR)
	@$(LIN_GPP) $(LIN_COMPILE_ARGS) -o $(LIN_BUILD_DIR)/Remote.o $(SRC_DIR)/Remote.cpp

#### LINUX64 BUILDING DIRECTIVES ####

$(DIST)/$(LOCAL_NAME)64$(LIN_LIB_EXT): $(LOCALLIN64OBJFILES)
	@echo "Linking object files..."
	@mkdir -p $(DIST)
	@$(LIN64_GPP) -fPIC -shared -s -o $(DIST)/$(LOCAL_NAME)64$(LIN_LIB_EXT) $(LOCALLIN64OBJFILES)

$(LIN64_BUILD_DIR)/Local.o: $(SRC_DIR)/Local.cpp $(LOCALHEADERFILES)
	@echo "Compiling Local.cpp"
	@mkdir -p $(LIN64_BUILD_DIR)
	@$(LIN64_GPP) $(LIN64_COMPILE_ARGS) -o $(LIN64_BUILD_DIR)/Local.o $(SRC_DIR)/Local.cpp

$(DIST)/$(REMOTE_NAME)64$(LIN_EXE_EXT): $(REMOTELIN64OBJFILES)
	@echo "Linking object files..."
	@mkdir -p $(DIST)
	@$(LIN64_GPP) -fPIC -ldl -s -o $(DIST)/$(REMOTE_NAME)64$(LIN_EXE_EXT) $(REMOTELIN64OBJFILES)

$(LIN64_BUILD_DIR)/Remote.o: $(SRC_DIR)/Remote.cpp $(REMOTEHEADERFILES)
	@echo "Compiling Remote.cpp"
	@mkdir -p $(LIN64_BUILD_DIR)
	@$(LIN64_GPP) $(LIN64_COMPILE_ARGS) -o $(LIN64_BUILD_DIR)/Remote.o $(SRC_DIR)/Remote.cpp

#### WINDOWS BUILDING DIRECTIVES ####

$(DIST)/$(LOCAL_NAME)32$(WIN_LIB_EXT): $(LOCALWINOBJFILES)
	@echo "Linking object files..."
	@mkdir -p $(DIST)
	@$(WIN_GPP) -shared -s -o $(DIST)/$(LOCAL_NAME)32$(WIN_LIB_EXT) $(LOCALWINOBJFILES)

$(WIN_BUILD_DIR)/Local.o: $(SRC_DIR)/Local.cpp $(LOCALHEADERFILES)
	@echo "Compiling Local.cpp"
	@mkdir -p $(WIN_BUILD_DIR)
	@$(WIN_GPP) $(WIN_COMPILE_ARGS) -o $(WIN_BUILD_DIR)/Local.o $(SRC_DIR)/Local.cpp

$(DIST)/$(REMOTE_NAME)32$(WIN_EXE_EXT): $(REMOTEWINOBJFILES)
	@echo "Linking object files..."
	@mkdir -p $(DIST)
	@$(WIN_GPP) -s -o $(DIST)/$(REMOTE_NAME)32$(WIN_EXE_EXT) $(REMOTEWINOBJFILES)

$(WIN_BUILD_DIR)/Remote.o: $(SRC_DIR)/Remote.cpp $(REMOTEHEADERFILES)
	@echo "Compiling Remote.cpp"
	@mkdir -p $(WIN_BUILD_DIR)
	@$(WIN_GPP) $(WIN_COMPILE_ARGS) -o $(WIN_BUILD_DIR)/Remote.o $(SRC_DIR)/Remote.cpp
	
#### WINDOWS64 BUILDING DIRECTIVES ####

$(DIST)/$(LOCAL_NAME)64$(WIN_LIB_EXT): $(LOCALWIN64OBJFILES)
	@echo "Linking object files..."
	@mkdir -p $(DIST)
	@$(WIN64_GPP) -shared -s -o $(DIST)/$(LOCAL_NAME)64$(WIN_LIB_EXT) $(LOCALWIN64OBJFILES)

$(WIN64_BUILD_DIR)/Local.o: $(SRC_DIR)/Local.cpp $(LOCALHEADERFILES)
	@echo "Compiling Local.cpp"
	@mkdir -p $(WIN64_BUILD_DIR)
	@$(WIN64_GPP) $(WIN64_COMPILE_ARGS) -o $(WIN64_BUILD_DIR)/Local.o $(SRC_DIR)/Local.cpp

$(DIST)/$(REMOTE_NAME)64$(WIN_EXE_EXT): $(REMOTEWIN64OBJFILES)
	@echo "Linking object files..."
	@mkdir -p $(DIST)
	@$(WIN64_GPP) -s -o $(DIST)/$(REMOTE_NAME)64$(WIN_EXE_EXT) $(REMOTEWIN64OBJFILES)

$(WIN64_BUILD_DIR)/Remote.o: $(SRC_DIR)/Remote.cpp $(REMOTEHEADERFILES)
	@echo "Compiling Remote.cpp"
	@mkdir -p $(WIN64_BUILD_DIR)
	@$(WIN64_GPP) $(WIN64_COMPILE_ARGS) -o $(WIN64_BUILD_DIR)/Remote.o $(SRC_DIR)/Remote.cpp
