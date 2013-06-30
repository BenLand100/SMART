WinGPP = g++
LinGPP = g++
Javac = javac
JavacPath = "C:\Program Files\Java\jdk1.7.0_21\bin"

BinDIR = bin
ObjDIR = obj
SrcDIR = src
IcoDIR = icons
RmDIR  = remote
BitFLG = -m32

JavaOUT = smart.jar
RemoteOUT = libsmartremote$(subst -m,,$(BitFLG))
JniOUT = libsmartjni$(subst -m,,$(BitFLG))

WinDEP = -static -static-libgcc -static-libstdc++ -shared -s -o
LinDEP = -static -static-libgcc -static-libstdc++ -shared -s -o
WinARGS = -std=c++11 -O3 -DWINDOWS -s -c $(BitFLG)
LinARGS = -FPIC -std=c++11 -O3 -DLINUX -s -c $(BitFLG)


REMOUT = NONE
GPP = NONE
ARGS = NONE
DEPS = NONE
JNI = NONE
OBJ = NONE
LINKER = NONE
	
CppFiles = \
	$(wildcard $(SrcDIR)/$(RmDIR)/*.cpp)
	
ObjFiles = \
	$(patsubst $(SrcDIR)/%.cpp,$(OBJ)/%.o,$(CppFiles))
	
JniFiles = \
	$(wildcard $(SrcDIR)/libsmartjni/*.cpp)
	
JniObjFiles = \
	$(patsubst $(SrcDIR)/%.cpp,$(OBJ)/%.o,$(JniFiles))
	
JavaFiles = \
	$(wildcard $(SrcDIR)/smart/*.java) \
	$(wildcard $(SrcDIR)/java.awt/*.java)
	
JavaObjs = \
	$(wildcard $(OBJ)/smart/*.class) \
	$(wildcard $(OBJ)/java.awt/*.class) \
	$(wildcard $(OBJ)/$(IcoDIR)/*)
	
	
all:
	@echo
	@echo "  Instructions For Making Smart:"
	@echo
	@echo "    For Windows:     make windows"
	@echo "    For Linux:       make linux"
	@echo "    For Both:        make both"
	@echo "    For x64 Builds:  make windows BitFLG=-m64"
	@echo "                     make linux BitFLG=-m64"
	@echo "    To Clean Files:  make clean"
    
everything: linux windows

linux:
GPP = $(LinGPP)
ARGS = $(LinARGS)
DEPS = $(LinDEP)
OBJ = $(ObjDIR)/Linux$(subst -m,,$(BitFLG))
ICONIN = $(SrcDIR)/$(IcoDIR)/*
ICONOUT = $(OBJ)/$(IcoDIR)
REMOUT = $(RemoteOUT).so
JNI = $(JniOUT).so
LINKER = 

linux: $(BinDIR)/Java $(BinDIR)/$(OUT) $(BinDIR)/$(JNI)
	@echo
	@echo "Finished Building Smart Linux Distribution."


windows:
GPP = $(WinGPP)
ARGS = $(WinARGS)
DEPS = $(WinDEP)
OBJ = $(ObjDIR)/Windows$(subst -m,,$(BitFLG))
ICONIN = $(SrcDIR)/$(IcoDIR)/*
ICONOUT = $(OBJ)/$(IcoDIR)
REMOUT = $(RemoteOUT).dll
JNI = $(JniOUT).dll
LINKER = -lws2_32 -lole32 -lshell32

windows: $(BinDIR)/Java $(BinDIR)/$(REMOUT) $(BinDIR)/$(JNI)
	@echo
	@echo "Finished Building Smart Windows Distribution."
	
clean:
	@echo "    Cleaning Build Files."
	@rm -rf $(ObjFiles) $(BinDIR) $(OBJ)

    


$(BinDIR)/$(REMOUT): $(ObjFiles)
	@echo
	@echo "Linking Remote Object Files.."
	@mkdir -p $(BinDIR)
	@mkdir -p $(OBJ)/libsmartjni
	@$(GPP) -Wl,$(SrcDIR)/$(RmDIR)/libsmartremote.def -Wl,--enable-stdcall-fixup $(DEPS) $(BinDIR)/$(REMOUT) $(ObjFiles) $(BitFLG) $(LINKER)
	
$(BinDIR)/$(JNI): $(JniObjFiles)
	@echo
	@echo "Compiling JNI Object Files.."
	@mkdir -p $(BinDIR)
	@$(GPP) $(DEPS) $(BinDIR)/$(JNI) $(JniObjFiles) $(BitFLG) $(LINKER)

	
$(OBJ)/%.o: $(SrcDIR)/%.cpp
	@echo "    Compiling: " $<
	@mkdir -p $(OBJ)
	@mkdir -p $(OBJ)/$(RmDIR)
	@$(GPP) $(ARGS) -o $@ $< $(LINKER)
	
$(BinDIR)/Java:
	@echo "Compiling Java Files.."
	@mkdir -p $(OBJ)
	@mkdir -p $(OBJ)/$(IcoDIR)
	@cp -r $(ICONIN) $(ICONOUT)
	@$(JavacPath)/$(Javac) -d $(OBJ) $(JavaFiles) -sourcepath $(SrcDIR) -classpath $(OBJ)
	@mkdir -p $(BinDIR)
	@$(JavacPath)/jar cfe $(BinDIR)/$(JavaOUT) smart.Main -C $(OBJ) smart -C $(OBJ) java -C $(OBJ) JNI -C $(OBJ) icons