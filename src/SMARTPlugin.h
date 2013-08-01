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
 
#ifndef _SMART_PLUGIN
#define _SMART_PLUGIN

typedef void (*_SMARTGetMousePos)(int &x, int &y);
typedef void (*_SMARTSetCapture)(bool enabled);
typedef void (*_SMARTButtonPressed)(int id, bool state);
 
typedef struct {
        int version; //Version of SMART to ensure binary compatibility
        void *img,*dbg; //Pointer to the memory location of the img (to simba) and dbg (from simba) colors
        int width, height; //Dimensions of client
        _SMARTGetMousePos getMousePos; //Callback to get the current fake mouse position
        _SMARTSetCapture setCapture; //Callback to turn set SMART's capture mode
} SMARTInfo;
 
/**
 * The plugin must export this method to handshake with SMART (unless the plugin
 * does not to interact with SMART at all, in which case the plugin binary is
 * simply loaded before the client starts).
 *
 * ptr - [in] pointer to the above structure defined filled with proper values.
 * replace - [out] should be set to true to clear the buttons on SMART related to graphics capture.
 * buttonc - [out] should be set to the number of buttons defined by this plugin.
 * buttonv - [out] toggle style names for defined buttons (true value should be first, false value second seperated by underscore for each button)
 * buttonid - [out] id to be returned to the button callback along with the state
 * buttonproc - [out] method to invoke for when the buttons defined here change state
 */
typedef void (*_SMARTPluginInit)(SMARTInfo *ptr, bool *replace, int *buttonc, char ***buttonv, int **buttonid, _SMARTButtonPressed *buttonproc);


#endif
