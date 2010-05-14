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

JAVAC=javac
JAVA=java

DIST=dist
BUILD=build

SRC_DIR=src
	
SMARTSOURCES= \
    $(SRC_DIR)/smart/updater/RSClass.java\
    $(SRC_DIR)/smart/updater/RSClient.java\
    $(SRC_DIR)/smart/updater/RSField.java\
    $(SRC_DIR)/smart/updater/Search.java\
    $(SRC_DIR)/smart/updater/Updater.java\
    $(SRC_DIR)/smart/updater/searches/BaseAnimableXY.java\
    $(SRC_DIR)/smart/updater/searches/GroundPlane.java\
    $(SRC_DIR)/smart/updater/searches/MinimapAngle.java\
    $(SRC_DIR)/smart/updater/searches/StandardDetail.java\
    $(SRC_DIR)/smart/updater/searches/Toolkit.java\
    $(SRC_DIR)/smart/updater/searches/Viewport.java
    
SMARTCLASSES= \
    $(BUILD)/smart/updater/RSClass.class\
    $(BUILD)/smart/updater/RSClient.class\
    $(BUILD)/smart/updater/RSField.class\
    $(BUILD)/smart/updater/Search.class\
    $(BUILD)/smart/updater/Updater.class\
    $(BUILD)/smart/updater/searches/BaseAnimableXY.class\
    $(BUILD)/smart/updater/searches/GroundPlane.class\
    $(BUILD)/smart/updater/searches/MinimapAngle.class\
    $(BUILD)/smart/updater/searches/StandardDetail.class\
    $(BUILD)/smart/updater/searches/Toolkit.class\
    $(BUILD)/smart/updater/searches/Viewport.class

all: $(SMARTCLASSES)
	@echo "Updating hooks..."
	@rm -rf $(DIST)
	@mkdir -p $(DIST)
	@$(JAVA) -classpath $(BUILD) smart.updater.Updater $(DIST)/hooks.pas
	@echo "Copying include files..."
	@cp $(SRC_DIR)/include/* $(DIST)/
	
clean: 
	@echo "Cleaning build files..."
	@rm -rf $(BUILD) $(DIST)
	
#### JAVA/Cypher BUILDING DIRECTIVES ####

$(SMARTCLASSES): $(SMARTSOURCES)
	@echo "Compiling SMART Updater..."
	@mkdir -p $(BUILD)
	@$(JAVAC) -classpath $(BUILD) -sourcepath $(SRC_DIR) -d $(BUILD) $(SMARTSOURCES)
	@echo "Don't worry about the warnings, just ignore them ;)"

