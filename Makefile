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
WIN_NAME=libsmart32.dll
WIN64_NAME=libsmart64.dll
LIN_NAME=libsmart32.so
LIN64_NAME=libsmart64.so

WIN_COMPILE_ARGS=-DWINDOWS -Wall -O0 -s -c
WIN64_COMPILE_ARGS=-DWINDOWS -Wall -O0 -s -c
LIN_COMPILE_ARGS=-fPIC -DLINUX -Wall -O3 -s -c
LIN64_COMPILE_ARGS=-fPIC -DLINUX -Wall -O3 -s -c

SRC_DIR=src
LIN_BUILD_DIR=$(BUILD)/linux32
LIN64_BUILD_DIR=$(BUILD)/linux64
WIN_BUILD_DIR=$(BUILD)/windows32
WIN64_BUILD_DIR=$(BUILD)/windows64
JAVA_BUILD_DIR=$(BUILD)/java

CPPSOURCEFILES= \
	$(SRC_DIR)/Main.cpp \
	$(SRC_DIR)/Color.cpp \
	$(SRC_DIR)/ClassLoader.cpp \
	$(SRC_DIR)/Input.cpp \
	$(SRC_DIR)/Reflection.cpp \
	$(SRC_DIR)/JVM.cpp \
	$(SRC_DIR)/Smart.cpp \
	$(SRC_DIR)/EIOS.cpp
	
CPPHEADERFILES= \
	$(SRC_DIR)/jni.h \
	$(SRC_DIR)/jni_md.h \
	$(SRC_DIR)/Main.h \
	$(SRC_DIR)/Color.h \
	$(SRC_DIR)/ClassLoader.h \
	$(SRC_DIR)/Input.h \
	$(SRC_DIR)/Reflection.h \
	$(SRC_DIR)/JVM.h \
	$(SRC_DIR)/Smart.h \
	$(SRC_DIR)/EIOS.h
	
WINOBJFILES= \
	$(WIN_BUILD_DIR)/Main.o \
	$(WIN_BUILD_DIR)/Color.o \
	$(WIN_BUILD_DIR)/ClassLoader.o \
	$(WIN_BUILD_DIR)/Input.o \
	$(WIN_BUILD_DIR)/Reflection.o \
	$(WIN_BUILD_DIR)/JVM.o \
	$(WIN_BUILD_DIR)/Smart.o \
	$(WIN_BUILD_DIR)/EIOS.o
	
WIN64OBJFILES= \
	$(WIN64_BUILD_DIR)/Main.o \
	$(WIN64_BUILD_DIR)/Color.o \
	$(WIN64_BUILD_DIR)/ClassLoader.o \
	$(WIN64_BUILD_DIR)/Input.o \
	$(WIN64_BUILD_DIR)/Reflection.o \
	$(WIN64_BUILD_DIR)/JVM.o \
	$(WIN64_BUILD_DIR)/Smart.o \
	$(WIN64_BUILD_DIR)/EIOS.o
	
LINOBJFILES= \
	$(LIN_BUILD_DIR)/Main.o \
	$(LIN_BUILD_DIR)/Color.o \
	$(LIN_BUILD_DIR)/ClassLoader.o \
	$(LIN_BUILD_DIR)/Input.o \
	$(LIN_BUILD_DIR)/Reflection.o \
	$(LIN_BUILD_DIR)/JVM.o \
	$(LIN_BUILD_DIR)/Smart.o \
	$(LIN_BUILD_DIR)/EIOS.o
	
LIN64OBJFILES= \
	$(LIN64_BUILD_DIR)/Main.o \
	$(LIN64_BUILD_DIR)/Color.o \
	$(LIN64_BUILD_DIR)/ClassLoader.o \
	$(LIN64_BUILD_DIR)/Input.o \
	$(LIN64_BUILD_DIR)/Reflection.o \
	$(LIN64_BUILD_DIR)/JVM.o \
	$(LIN64_BUILD_DIR)/Smart.o \
	$(LIN64_BUILD_DIR)/EIOS.o
	
SMARTSOURCES= \
    $(SRC_DIR)/java/awt/Canvas.java \
    $(SRC_DIR)/smart/BlockingEventQueue.java \
    $(SRC_DIR)/smart/Client.java \
    $(SRC_DIR)/smart/ClientStub.java \
    $(SRC_DIR)/smart/EventNazi.java \
    $(SRC_DIR)/smart/EventRedirect.java \
    $(SRC_DIR)/smart/UnblockedEvent.java
    
SMARTCLASSES= \
    $(JAVA_BUILD_DIR)/java/awt/Canvas.class \
    $(JAVA_BUILD_DIR)/smart/BlockingEventQueue.class \
    $(JAVA_BUILD_DIR)/smart/Client.class \
    $(JAVA_BUILD_DIR)/smart/ClientStub.class \
    $(JAVA_BUILD_DIR)/smart/EventNazi.class \
    $(JAVA_BUILD_DIR)/smart/EventRedirect.class \
    $(JAVA_BUILD_DIR)/smart/UnblockedEvent.class

all:
	@echo "Syntax for SMART makefile:\n    For Windows distributions: make windows windows64\n    For Linux distributions: make linux linux64\n    For SCAR distributions: make scar\n    For All distributions: make everything\n    For test apps: make test\n    To clean build files: make clean"
	
everything: linux linux64 windows windows64

linux: $(DIST)/$(LIN_NAME)
	@echo "Finished Building the Linux 32bit SMART distribution"
	@make -C remote DIST=../dist BUILD=../build linux
	
linux64: $(DIST)/$(LIN64_NAME)
	@echo "Finished Building the Linux 64bit SMART distribution"
	@make -C remote DIST=../dist BUILD=../build linux64

windows: $(DIST)/$(WIN_NAME)
	@echo "Finished Building the Windows 32bit SMART distribution"
	@make -C remote DIST=../dist BUILD=../build windows

windows64: $(DIST)/$(WIN64_NAME)
	@echo "Finished Building the Windows 64bit SMART distribution"
	@make -C remote DIST=../dist BUILD=../build windows64
	
test: test-apps/test-windows.cpp test-apps/test-linux.cpp test-apps/test-scar.cpp
	@mkdir -p $(DIST)
	@$(WIN_GPP) -Wall -o $(DIST)/test-windows32.exe test-apps/test-windows.cpp
	@$(WIN64_GPP) -Wall -o $(DIST)/test-windows64.exe test-apps/test-windows.cpp
	@$(WIN_GPP) -Wall -o $(DIST)/test-scar.exe test-apps/test-scar.cpp
	@$(LIN_GPP) -Wall -fPIC -ldl -o $(DIST)/test-linux32 test-apps/test-linux.cpp
	@$(LIN64_GPP) -Wall -fPIC -ldl -o $(DIST)/test-linux64 test-apps/test-linux.cpp
	@$(WIN_GPP) -Wall -o $(DIST)/test-eios-win32.exe test-apps/test-eios.cpp
	@$(WIN64_GPP) -Wall -o $(DIST)/test-eios-win64.exe test-apps/test-eios.cpp
	@$(LIN_GPP) -Wall -fPIC -ldl -o $(DIST)/test-eios-lin32 test-apps/test-eios.cpp
	@$(LIN64_GPP) -Wall -fPIC -ldl -o $(DIST)/test-eios-lin64 test-apps/test-eios.cpp
	@$(WIN_GPP) -Wall -o $(DIST)/test-remote-win32.exe test-apps/test-remote.cpp
	@$(WIN64_GPP) -Wall -o $(DIST)/test-remote-win64.exe test-apps/test-remote.cpp
	@$(LIN_GPP) -Wall -fPIC -ldl -o $(DIST)/test-remote-lin32 test-apps/test-remote.cpp
	@$(LIN64_GPP) -Wall -fPIC -ldl -o $(DIST)/test-remote-lin64 test-apps/test-remote.cpp
	@cp test-apps/test-python.py $(DIST)/test-python
	@echo "Finished building test programs" 
	
clean: 
	@echo "Cleaning build files..."
	@rm -rf $(BUILD) $(DIST)
	@rm -f $(SRC_DIR)/classes.data    
	
#### LINUX BUILDING DIRECTIVES ####

$(DIST)/$(LIN_NAME): $(LINOBJFILES)
	@echo "Linking SMART object files..."
	@mkdir -p $(DIST)
	@$(LIN_GPP) -fPIC -shared -s -o $(DIST)/$(LIN_NAME) $(LINOBJFILES)

$(LIN_BUILD_DIR)/Main.o: $(SRC_DIR)/Main.cpp $(CPPHEADERFILES)
	@echo "Compiling Main.cpp"
	@mkdir -p $(LIN_BUILD_DIR)
	@$(LIN_GPP) $(LIN_COMPILE_ARGS) -o $(LIN_BUILD_DIR)/Main.o $(SRC_DIR)/Main.cpp

${LIN_BUILD_DIR}/Color.o: $(SRC_DIR)/Color.cpp $(CPPHEADERFILES)
	@echo "Compiling Color.cpp"
	@mkdir -p $(LIN_BUILD_DIR)
	@$(LIN_GPP) $(LIN_COMPILE_ARGS) -o $(LIN_BUILD_DIR)/Color.o $(SRC_DIR)/Color.cpp

${LIN_BUILD_DIR}/ClassLoader.o: $(SRC_DIR)/ClassLoader.cpp $(SRC_DIR)/classes.data $(CPPHEADERFILES)
	@echo "Compiling ClassLoader.cpp"
	@mkdir -p $(LIN_BUILD_DIR)
	@$(LIN_GPP) $(LIN_COMPILE_ARGS) -o $(LIN_BUILD_DIR)/ClassLoader.o $(SRC_DIR)/ClassLoader.cpp

${LIN_BUILD_DIR}/Input.o: $(SRC_DIR)/Input.cpp $(CPPHEADERFILES)
	@echo "Compiling Input.cpp"
	@mkdir -p $(LIN_BUILD_DIR)
	@$(LIN_GPP) $(LIN_COMPILE_ARGS) -o $(LIN_BUILD_DIR)/Input.o $(SRC_DIR)/Input.cpp

${LIN_BUILD_DIR}/Reflection.o: $(SRC_DIR)/Reflection.cpp $(CPPHEADERFILES)
	@echo "Compiling Reflection.cpp"
	@mkdir -p $(LIN_BUILD_DIR)
	@$(LIN_GPP) $(LIN_COMPILE_ARGS) -o $(LIN_BUILD_DIR)/Reflection.o $(SRC_DIR)/Reflection.cpp

${LIN_BUILD_DIR}/JVM.o: $(SRC_DIR)/JVM.cpp $(CPPHEADERFILES)
	@echo "Compiling JVM.cpp"
	@mkdir -p $(LIN_BUILD_DIR)
	@$(LIN_GPP) $(LIN_COMPILE_ARGS) -o $(LIN_BUILD_DIR)/JVM.o $(SRC_DIR)/JVM.cpp

${LIN_BUILD_DIR}/Smart.o: $(SRC_DIR)/Smart.cpp $(CPPHEADERFILES)
	@echo "Compiling Smart.cpp"
	@mkdir -p $(LIN_BUILD_DIR)
	@$(LIN_GPP) $(LIN_COMPILE_ARGS) -o $(LIN_BUILD_DIR)/Smart.o $(SRC_DIR)/Smart.cpp

${LIN_BUILD_DIR}/EIOS.o: $(SRC_DIR)/EIOS.cpp $(CPPHEADERFILES)
	@echo "Compiling EIOS.cpp"
	@mkdir -p $(LIN_BUILD_DIR)
	@$(LIN_GPP) $(LIN_COMPILE_ARGS) -o $(LIN_BUILD_DIR)/EIOS.o $(SRC_DIR)/EIOS.cpp
	
#### LINUX64 BUILDING DIRECTIVES ####

$(DIST)/$(LIN64_NAME): $(LIN64OBJFILES)
	@echo "Linking SMART object files..."
	@mkdir -p $(DIST)
	@$(LIN64_GPP) -fPIC -shared -s -o $(DIST)/$(LIN64_NAME) $(LIN64OBJFILES)

$(LIN64_BUILD_DIR)/Main.o: $(SRC_DIR)/Main.cpp $(CPPHEADERFILES)
	@echo "Compiling Main.cpp"
	@mkdir -p $(LIN64_BUILD_DIR)
	@$(LIN64_GPP) $(LIN64_COMPILE_ARGS) -o $(LIN64_BUILD_DIR)/Main.o $(SRC_DIR)/Main.cpp

${LIN64_BUILD_DIR}/Color.o: $(SRC_DIR)/Color.cpp $(CPPHEADERFILES)
	@echo "Compiling Color.cpp"
	@mkdir -p $(LIN64_BUILD_DIR)
	@$(LIN64_GPP) $(LIN64_COMPILE_ARGS) -o $(LIN64_BUILD_DIR)/Color.o $(SRC_DIR)/Color.cpp

${LIN64_BUILD_DIR}/ClassLoader.o: $(SRC_DIR)/ClassLoader.cpp $(SRC_DIR)/classes.data $(CPPHEADERFILES)
	@echo "Compiling ClassLoader.cpp"
	@mkdir -p $(LIN64_BUILD_DIR)
	@$(LIN64_GPP) $(LIN64_COMPILE_ARGS) -o $(LIN64_BUILD_DIR)/ClassLoader.o $(SRC_DIR)/ClassLoader.cpp

${LIN64_BUILD_DIR}/Input.o: $(SRC_DIR)/Input.cpp $(CPPHEADERFILES)
	@echo "Compiling Input.cpp"
	@mkdir -p $(LIN64_BUILD_DIR)
	@$(LIN64_GPP) $(LIN64_COMPILE_ARGS) -o $(LIN64_BUILD_DIR)/Input.o $(SRC_DIR)/Input.cpp

${LIN64_BUILD_DIR}/Reflection.o: $(SRC_DIR)/Reflection.cpp $(CPPHEADERFILES)
	@echo "Compiling Reflection.cpp"
	@mkdir -p $(LIN64_BUILD_DIR)
	@$(LIN64_GPP) $(LIN64_COMPILE_ARGS) -o $(LIN64_BUILD_DIR)/Reflection.o $(SRC_DIR)/Reflection.cpp

${LIN64_BUILD_DIR}/JVM.o: $(SRC_DIR)/JVM.cpp $(CPPHEADERFILES)
	@echo "Compiling JVM.cpp"
	@mkdir -p $(LIN64_BUILD_DIR)
	@$(LIN64_GPP) $(LIN64_COMPILE_ARGS) -o $(LIN64_BUILD_DIR)/JVM.o $(SRC_DIR)/JVM.cpp

${LIN64_BUILD_DIR}/Smart.o: $(SRC_DIR)/Smart.cpp $(CPPHEADERFILES)
	@echo "Compiling Smart.cpp"
	@mkdir -p $(LIN64_BUILD_DIR)
	@$(LIN64_GPP) $(LIN64_COMPILE_ARGS) -o $(LIN64_BUILD_DIR)/Smart.o $(SRC_DIR)/Smart.cpp

${LIN64_BUILD_DIR}/EIOS.o: $(SRC_DIR)/EIOS.cpp $(CPPHEADERFILES)
	@echo "Compiling EIOS.cpp"
	@mkdir -p $(LIN64_BUILD_DIR)
	@$(LIN64_GPP) $(LIN64_COMPILE_ARGS) -o $(LIN64_BUILD_DIR)/EIOS.o $(SRC_DIR)/EIOS.cpp

#### WINDOWS BUILDING DIRECTIVES ####

$(DIST)/$(WIN_NAME): $(WINOBJFILES)
	@echo "Linking object files..."
	@mkdir -p $(DIST)
	@$(WIN_GPP) -Wl,$(SRC_DIR)/libsmart.def -static-libgcc -static-libstdc++ -mwindows -shared -s -o $(DIST)/$(WIN_NAME) $(WINOBJFILES)

$(WIN_BUILD_DIR)/Main.o: $(SRC_DIR)/Main.cpp $(CPPHEADERFILES)
	@echo "Compiling Main.cpp"
	@mkdir -p $(WIN_BUILD_DIR)
	@$(WIN_GPP) $(WIN_COMPILE_ARGS) -o $(WIN_BUILD_DIR)/Main.o $(SRC_DIR)/Main.cpp

${WIN_BUILD_DIR}/Color.o: $(SRC_DIR)/Color.cpp $(CPPHEADERFILES)
	@echo "Compiling Color.cpp"
	@mkdir -p $(WIN_BUILD_DIR)
	@$(WIN_GPP) $(WIN_COMPILE_ARGS) -o $(WIN_BUILD_DIR)/Color.o $(SRC_DIR)/Color.cpp

${WIN_BUILD_DIR}/ClassLoader.o: $(SRC_DIR)/ClassLoader.cpp $(SRC_DIR)/classes.data $(CPPHEADERFILES)
	@echo "Compiling Classloader.cpp"
	@mkdir -p $(WIN_BUILD_DIR)
	@$(WIN_GPP) $(WIN_COMPILE_ARGS) -o $(WIN_BUILD_DIR)/ClassLoader.o $(SRC_DIR)/ClassLoader.cpp

${WIN_BUILD_DIR}/Input.o: $(SRC_DIR)/Input.cpp $(CPPHEADERFILES)
	@echo "Compiling Input.cpp"
	@mkdir -p $(WIN_BUILD_DIR)
	@$(WIN_GPP) $(WIN_COMPILE_ARGS) -o $(WIN_BUILD_DIR)/Input.o $(SRC_DIR)/Input.cpp

${WIN_BUILD_DIR}/Reflection.o: $(SRC_DIR)/Reflection.cpp $(CPPHEADERFILES)
	@echo "Compiling Reflection.cpp"
	@mkdir -p $(WIN_BUILD_DIR)
	@$(WIN_GPP) $(WIN_COMPILE_ARGS) -o $(WIN_BUILD_DIR)/Reflection.o $(SRC_DIR)/Reflection.cpp

${WIN_BUILD_DIR}/JVM.o: $(SRC_DIR)/JVM.cpp $(CPPHEADERFILES)
	@echo "Compiling JVM.cpp"
	@mkdir -p $(WIN_BUILD_DIR)
	@$(WIN_GPP) $(WIN_COMPILE_ARGS) -o $(WIN_BUILD_DIR)/JVM.o $(SRC_DIR)/JVM.cpp

${WIN_BUILD_DIR}/Smart.o: $(SRC_DIR)/Smart.cpp $(CPPHEADERFILES)
	@echo "Compiling Smart.cpp"
	@mkdir -p $(WIN_BUILD_DIR)
	@$(WIN_GPP) $(WIN_COMPILE_ARGS) -o $(WIN_BUILD_DIR)/Smart.o $(SRC_DIR)/Smart.cpp
	
${WIN_BUILD_DIR}/EIOS.o: $(SRC_DIR)/EIOS.cpp $(CPPHEADERFILES)
	@echo "Compiling EIOS.cpp"
	@mkdir -p $(WIN_BUILD_DIR)
	@$(WIN_GPP) $(WIN_COMPILE_ARGS) -o $(WIN_BUILD_DIR)/EIOS.o $(SRC_DIR)/EIOS.cpp

#### WINDOWS64 BUILDING DIRECTIVES ####

$(DIST)/$(WIN64_NAME): $(WIN64OBJFILES)
	@echo "Linking object files..."
	@mkdir -p $(DIST)
	@$(WIN64_GPP) -Wl,$(SRC_DIR)/libsmart.def -static-libgcc -static-libstdc++ -mwindows -shared -s -o $(DIST)/$(WIN64_NAME) $(WIN64OBJFILES)

$(WIN64_BUILD_DIR)/Main.o: $(SRC_DIR)/Main.cpp $(CPPHEADERFILES)
	@echo "Compiling Main.cpp"
	@mkdir -p $(WIN64_BUILD_DIR)
	@$(WIN64_GPP) $(WIN64_COMPILE_ARGS) -o $(WIN64_BUILD_DIR)/Main.o $(SRC_DIR)/Main.cpp

${WIN64_BUILD_DIR}/Color.o: $(SRC_DIR)/Color.cpp $(CPPHEADERFILES)
	@echo "Compiling Color.cpp"
	@mkdir -p $(WIN64_BUILD_DIR)
	@$(WIN64_GPP) $(WIN64_COMPILE_ARGS) -o $(WIN64_BUILD_DIR)/Color.o $(SRC_DIR)/Color.cpp

${WIN64_BUILD_DIR}/ClassLoader.o: $(SRC_DIR)/ClassLoader.cpp $(SRC_DIR)/classes.data $(CPPHEADERFILES)
	@echo "Compiling Classloader.cpp"
	@mkdir -p $(WIN64_BUILD_DIR)
	@$(WIN64_GPP) $(WIN64_COMPILE_ARGS) -o $(WIN64_BUILD_DIR)/ClassLoader.o $(SRC_DIR)/ClassLoader.cpp

${WIN64_BUILD_DIR}/Input.o: $(SRC_DIR)/Input.cpp $(CPPHEADERFILES)
	@echo "Compiling Input.cpp"
	@mkdir -p $(WIN64_BUILD_DIR)
	@$(WIN64_GPP) $(WIN64_COMPILE_ARGS) -o $(WIN64_BUILD_DIR)/Input.o $(SRC_DIR)/Input.cpp

${WIN64_BUILD_DIR}/Reflection.o: $(SRC_DIR)/Reflection.cpp $(CPPHEADERFILES)
	@echo "Compiling Reflection.cpp"
	@mkdir -p $(WIN64_BUILD_DIR)
	@$(WIN64_GPP) $(WIN64_COMPILE_ARGS) -o $(WIN64_BUILD_DIR)/Reflection.o $(SRC_DIR)/Reflection.cpp

${WIN64_BUILD_DIR}/JVM.o: $(SRC_DIR)/JVM.cpp $(CPPHEADERFILES)
	@echo "Compiling JVM.cpp"
	@mkdir -p $(WIN64_BUILD_DIR)
	@$(WIN64_GPP) $(WIN64_COMPILE_ARGS) -o $(WIN64_BUILD_DIR)/JVM.o $(SRC_DIR)/JVM.cpp

${WIN64_BUILD_DIR}/Smart.o: $(SRC_DIR)/Smart.cpp $(CPPHEADERFILES)
	@echo "Compiling Smart.cpp"
	@mkdir -p $(WIN64_BUILD_DIR)
	@$(WIN64_GPP) $(WIN64_COMPILE_ARGS) -o $(WIN64_BUILD_DIR)/Smart.o $(SRC_DIR)/Smart.cpp
	
${WIN64_BUILD_DIR}/EIOS.o: $(SRC_DIR)/EIOS.cpp $(CPPHEADERFILES)
	@echo "Compiling EIOS.cpp"
	@mkdir -p $(WIN64_BUILD_DIR)
	@$(WIN64_GPP) $(WIN64_COMPILE_ARGS) -o $(WIN64_BUILD_DIR)/EIOS.o $(SRC_DIR)/EIOS.cpp

#### JAVA/Cypher BUILDING DIRECTIVES ####

src/classes.data: $(SMARTCLASSES) $(BUILD)/cypher/Cypher.class 
	@echo "Encoding Java classes..."
	@$(JAVA) -classpath $(BUILD) cypher.Cypher $(JAVA_BUILD_DIR) ./src/classes.data

$(BUILD)/cypher/Cypher.class: $(SRC_DIR)/cypher/Cypher.java
	@echo "Compiling Class Cypher"
	@mkdir -p $(JAVA_BUILD_DIR)
	@$(JAVAC) -sourcepath $(SRC_DIR) -d $(BUILD) $(SRC_DIR)/cypher/Cypher.java

$(SMARTCLASSES): $(SMARTSOURCES)
	@echo "Compiling Java Classes..."
	@mkdir -p $(JAVA_BUILD_DIR)
	@$(JAVAC) -classpath $(JAVA_BUILD_DIR) -sourcepath $(SRC_DIR) -d $(JAVA_BUILD_DIR) $(SMARTSOURCES)

