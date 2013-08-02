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

#include "SMARTPlugin.h"
#include <iostream>

void button_callback(int id, bool state) {
    std::cout << "Button Pressed! " << id << ' ' << state << '\n';
}

extern "C" void SMARTPluginInit(SMARTInfo *ptr, bool *replace, int *buttonc, char ***buttonv, int **buttonid, _SMARTButtonPressed *buttonproc) {
    std::cout << "HELLO WORLD! The image is at " << ptr->img << " and the debug is at " << ptr->dbg << "\n";
    
    char **caps = new char*[2];
    caps[0] = (char*)"Disable IDs_Enable IDs";
    caps[1] = (char*)"Disable Debug_Enable Debug";
    int *ids = new int[2];
    ids[0] = 100;
    ids[1] = 101;
    
    *replace = false;
    *buttonc = 2;
    *buttonv = caps;
    *buttonid = ids;
    *buttonproc = &button_callback;
}
