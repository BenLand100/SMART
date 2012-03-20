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

WIN_GPP=i686-pc-mingw32-g++
LIN_GPP=i686-pc-linux-gnu-g++
LIN64_GPP=g++
JAVAC=javac
JAVA=java

DIST=dist
BUILD=build
SCAR_NAME=Embedded_SMART.dll
WIN_NAME=libsmart.dll
LIN_NAME=libsmart32.so
LIN64_NAME=libsmart64.so

WIN_COMPILE_ARGS=-DWINDOWS -Wall -O0 -s -c
SCAR_COMPILE_ARGS=-DWINDOWS -DNEWSCAR -Wall -O0 -s -c
LIN_COMPILE_ARGS=-fPIC -DLINUX -Wall -O3 -s -c
LIN64_COMPILE_ARGS=-fPIC -DLINUX -Wall -O3 -s -c

SRC_DIR=src
LIN_BUILD_DIR=$(BUILD)/linux32
LIN64_BUILD_DIR=$(BUILD)/linux64
WIN_BUILD_DIR=$(BUILD)/windows32
SCAR_BUILD_DIR=$(BUILD)/scar
JAVA_BUILD_DIR=$(BUILD)/java

CPPSOURCEFILES= \
	$(SRC_DIR)/Main.cpp \
	$(SRC_DIR)/Color.cpp \
	$(SRC_DIR)/ClassLoader.cpp \
	$(SRC_DIR)/Input.cpp \
	$(SRC_DIR)/Reflection.cpp \
	$(SRC_DIR)/JVM.cpp \
	$(SRC_DIR)/Smart.cpp \
	$(SRC_DIR)/STD_Wrapper.cpp \
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
	$(SRC_DIR)/STD_Wrapper.h \
	$(SRC_DIR)/EIOS.h
	
WINOBJFILES= \
	$(WIN_BUILD_DIR)/Main.o \
	$(WIN_BUILD_DIR)/Color.o \
	$(WIN_BUILD_DIR)/ClassLoader.o \
	$(WIN_BUILD_DIR)/Input.o \
	$(WIN_BUILD_DIR)/Reflection.o \
	$(WIN_BUILD_DIR)/JVM.o \
	$(WIN_BUILD_DIR)/Smart.o \
	$(WIN_BUILD_DIR)/STD_Wrapper.o \
	$(WIN_BUILD_DIR)/EIOS.o

SCAROBJFILES= \
	$(SCAR_BUILD_DIR)/Main.o \
	$(SCAR_BUILD_DIR)/Color.o \
	$(SCAR_BUILD_DIR)/ClassLoader.o \
	$(SCAR_BUILD_DIR)/Input.o \
	$(SCAR_BUILD_DIR)/Reflection.o \
	$(SCAR_BUILD_DIR)/JVM.o \
	$(SCAR_BUILD_DIR)/Smart.o \
	$(SCAR_BUILD_DIR)/STD_Wrapper.o
	
LINOBJFILES= \
	$(LIN_BUILD_DIR)/Main.o \
	$(LIN_BUILD_DIR)/Color.o \
	$(LIN_BUILD_DIR)/ClassLoader.o \
	$(LIN_BUILD_DIR)/Input.o \
	$(LIN_BUILD_DIR)/Reflection.o \
	$(LIN_BUILD_DIR)/JVM.o \
	$(LIN_BUILD_DIR)/Smart.o \
	$(LIN_BUILD_DIR)/STD_Wrapper.o \
	$(LIN_BUILD_DIR)/EIOS.o
	
LIN64OBJFILES= \
	$(LIN64_BUILD_DIR)/Main.o \
	$(LIN64_BUILD_DIR)/Color.o \
	$(LIN64_BUILD_DIR)/ClassLoader.o \
	$(LIN64_BUILD_DIR)/Input.o \
	$(LIN64_BUILD_DIR)/Reflection.o \
	$(LIN64_BUILD_DIR)/JVM.o \
	$(LIN64_BUILD_DIR)/Smart.o \
	$(LIN64_BUILD_DIR)/STD_Wrapper.o \
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
	@echo "Syntax for SMART makefile:\n    For Windows distributions: make windows\n    For Linux distributions: make linux\n    For SCAR distributions: make scar\n    For All distributions: make everything\n    For test apps: make test\n    To clean build files: make clean"
	
everything: linux linux64 windows scar

linux: $(DIST)/$(LIN_NAME)
	@echo "Finished Building the Linux 32bit SMART distribution"
	
linux64: $(DIST)/$(LIN64_NAME)
	@echo "Finished Building the Linux 64bit SMART distribution"
	
scar: $(DIST)/$(SCAR_NAME)
	@echo "Finished Building the SCAR SMART distribution"

windows: $(DIST)/$(WIN_NAME)
	@echo "Finished Building the Windows SMART distribution"
	
test: test-apps/test-windows.cpp test-apps/test-linux.cpp test-apps/test-scar.cpp
	@$(WIN_GPP) -Wall -o $(DIST)/test-windows.exe test-apps/test-windows.cpp
	@$(WIN_GPP) -Wall -o $(DIST)/test-scar.exe test-apps/test-scar.cpp
	@$(LIN_GPP) -Wall -fPIC -ldl -o $(DIST)/test-linux test-apps/test-linux.cpp
	@cp test-apps/test-python.py $(DIST)/test-python
	@echo "Finished building test programs" 
	
clean: 
	@echo "Cleaning build files..."
	@rm -rf $(BUILD) $(DIST)
	@rm -f $(SRC_DIR)/classes.datai6    
	
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

${LIN_BUILD_DIR}/STD_Wrapper.o: $(SRC_DIR)/STD_Wrapper.cpp $(CPPHEADERFILES)
	@echo "Compiling STD_Wrapper.cpp"
	@mkdir -p $(LIN_BUILD_DIR)
	@$(LIN_GPP) $(LIN_COMPILE_ARGS) -o $(LIN_BUILD_DIR)/STD_Wrapper.o $(SRC_DIR)/STD_Wrapper.cpp

${LIN_BUILD_DIR}/EIOS.o: $(SRC_DIR)/EIOS.cpp $(CPPHEADERFILES)
	@echo "Compiling STD_Wrapper.cpp"
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

${LIN64_BUILD_DIR}/STD_Wrapper.o: $(SRC_DIR)/STD_Wrapper.cpp $(CPPHEADERFILES)
	@echo "Compiling STD_Wrapper.cpp"
	@mkdir -p $(LIN64_BUILD_DIR)
	@$(LIN64_GPP) $(LIN64_COMPILE_ARGS) -o $(LIN64_BUILD_DIR)/STD_Wrapper.o $(SRC_DIR)/STD_Wrapper.cpp

${LIN64_BUILD_DIR}/EIOS.o: $(SRC_DIR)/EIOS.cpp $(CPPHEADERFILES)
	@echo "Compiling STD_Wrapper.cpp"
	@mkdir -p $(LIN64_BUILD_DIR)
	@$(LIN64_GPP) $(LIN64_COMPILE_ARGS) -o $(LIN_BUILD_DIR)/EIOS.o $(SRC_DIR)/EIOS.cpp

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

${WIN_BUILD_DIR}/STD_Wrapper.o: $(SRC_DIR)/STD_Wrapper.cpp $(CPPHEADERFILES)
	@echo "Compiling STD_Wrapper.cpp"
	@mkdir -p $(WIN_BUILD_DIR)
	@$(WIN_GPP) $(WIN_COMPILE_ARGS) -o $(WIN_BUILD_DIR)/STD_Wrapper.o $(SRC_DIR)/STD_Wrapper.cpp
	
${WIN_BUILD_DIR}/EIOS.o: $(SRC_DIR)/EIOS.cpp $(CPPHEADERFILES)
	@echo "Compiling EIOS.cpp"
	@mkdir -p $(WIN_BUILD_DIR)
	@$(WIN_GPP) $(WIN_COMPILE_ARGS) -o $(WIN_BUILD_DIR)/EIOS.o $(SRC_DIR)/EIOS.cpp

#### SCAR BUILDING DIRECTIVES ####

$(DIST)/$(SCAR_NAME): $(SCAROBJFILES)
	@echo "Linking object files..."
	@mkdir -p $(DIST)
	@$(WIN_GPP) -Wl,$(SRC_DIR)/libsmart.def -static-libgcc -static-libstdc++ -mwindows -shared -s -o $(DIST)/$(SCAR_NAME) $(SCAROBJFILES)

$(SCAR_BUILD_DIR)/Main.o: $(SRC_DIR)/Main.cpp $(CPPHEADERFILES)
	@echo "Compiling Main.cpp"
	@mkdir -p $(SCAR_BUILD_DIR)
	@$(WIN_GPP) $(SCAR_COMPILE_ARGS) -o $(SCAR_BUILD_DIR)/Main.o $(SRC_DIR)/Main.cpp

${SCAR_BUILD_DIR}/Color.o: $(SRC_DIR)/Color.cpp $(CPPHEADERFILES)
	@echo "Compiling Color.cpp"
	@mkdir -p $(SCAR_BUILD_DIR)
	@$(WIN_GPP) $(SCAR_COMPILE_ARGS) -o $(SCAR_BUILD_DIR)/Color.o $(SRC_DIR)/Color.cpp

${SCAR_BUILD_DIR}/ClassLoader.o: $(SRC_DIR)/ClassLoader.cpp $(SRC_DIR)/classes.data $(CPPHEADERFILES)
	@echo "Compiling Classloader.cpp"
	@mkdir -p $(SCAR_BUILD_DIR)
	@$(WIN_GPP) $(SCAR_COMPILE_ARGS) -o $(SCAR_BUILD_DIR)/ClassLoader.o $(SRC_DIR)/ClassLoader.cpp

${SCAR_BUILD_DIR}/Input.o: $(SRC_DIR)/Input.cpp $(CPPHEADERFILES)
	@echo "Compiling Input.cpp"
	@mkdir -p $(SCAR_BUILD_DIR)
	@$(WIN_GPP) $(SCAR_COMPILE_ARGS) -o $(SCAR_BUILD_DIR)/Input.o $(SRC_DIR)/Input.cpp

${SCAR_BUILD_DIR}/Reflection.o: $(SRC_DIR)/Reflection.cpp $(CPPHEADERFILES)
	@echo "Compiling Reflection.cpp"
	@mkdir -p $(SCAR_BUILD_DIR)
	@$(WIN_GPP) $(SCAR_COMPILE_ARGS) -o $(SCAR_BUILD_DIR)/Reflection.o $(SRC_DIR)/Reflection.cpp

${SCAR_BUILD_DIR}/JVM.o: $(SRC_DIR)/JVM.cpp $(CPPHEADERFILES)
	@echo "Compiling JVM.cpp"
	@mkdir -p $(SCAR_BUILD_DIR)
	@$(WIN_GPP) $(SCAR_COMPILE_ARGS) -o $(SCAR_BUILD_DIR)/JVM.o $(SRC_DIR)/JVM.cpp

${SCAR_BUILD_DIR}/Smart.o: $(SRC_DIR)/Smart.cpp $(CPPHEADERFILES)
	@echo "Compiling Smart.cpp"
	@mkdir -p $(SCAR_BUILD_DIR)
	@$(WIN_GPP) $(SCAR_COMPILE_ARGS) -o $(SCAR_BUILD_DIR)/Smart.o $(SRC_DIR)/Smart.cpp

${SCAR_BUILD_DIR}/STD_Wrapper.o: $(SRC_DIR)/STD_Wrapper.cpp $(CPPHEADERFILES)
	@echo "Compiling STD_Wrapper.cpp"
	@mkdir -p $(SCAR_BUILD_DIR)
	@$(WIN_GPP) $(SCAR_COMPILE_ARGS) -o $(SCAR_BUILD_DIR)/STD_Wrapper.o $(SRC_DIR)/STD_Wrapper.cpp


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

