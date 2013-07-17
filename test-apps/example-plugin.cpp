/**
 *  Copyright 2012 by Benjamin J. Land (a.k.a. BenLand100)
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

//This example does nothing but turn SMART's internal capture off and write out
//the memory locations of the dbg and img arrays. This method is the entry point
//that SMART will call when your plugin is loaded.
extern "C" void SMARTPluginInit(SMARTInfo *ptr) {
    ptr->setCapture(false);
    std::cout << "HELLO WORLD! The image is at " << ptr->img << " and the debug is at " << ptr->dbg << "\n";
}
