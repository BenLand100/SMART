/** 
 *  Copyright 2010 by Benjamin J. Land (a.k.a. BenLand100) 
 * 
 *  This file is part of the Extensible Input/Output System (EIOS) 
 * 
 *  EIOS is free software: you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation, either version 3 of the License, or 
 *  (at your option) any later version. 
 * 
 *  EIOS is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with EIOS. If not, see <http://www.gnu.org/licenses/>. 
 */ 
  
/** 
 *  This is the C++ header for the Extensible Input/Output System. 
 *  EIOS is designed as a plugable interface to allow easy routing of 
 *  input/output commands of a computer interface, i.e. Mouse, Keyboard, 
 *  and Graphical components. In the documentation, the Controller is  
 *  program that would be receiving Graphical data from a Client, and  
 *  sending Keyboard and Mouse commands to that same Client. 
 *  
 *  The methods outlined here are a designed in a minimalist approach  
 *  that closely resembles low level communication, e.g. the mouse  
 *  commands only allow for the holding and releasing of the two mouse 
 *  buttons, checking the state of the two buttons, the movement of the  
 *  cursor, and obtaining the current position. No methods are defined 
 *  for more complex motion, dragging, or clicking, because these  
 *  actions consist of the defined actions. Likewise for Keyboard  
 *  control, there exists no method to send a string of characters, due 
 *  to the fact that sending a string of characters is equivalent to 
 *  sending many characters at the lowest level. 
 *  
 *  The Controller should use the defined methods to gather data about  
 *  the Client implementing the EIOS standard, then the Controller may 
 *  initialize and utilize the Client. The Client should implement all  
 *  defined methods with the proper conventions. The methods are written 
 *  to be language and operating system independent so that the EIOS 
 *  standard may be used to abstract all routing of I/O commands. 
 * 
 *  Clients may provide more than one Target via returning a unique  
 *  identifier the address size of the current system, i.e. a void* 
 *  in C-like languages (of any significance, e.g. a pointer to a data  
 *  structure or index into an array) which should be passed as an  
 *  argument to any method requiring it. Although this adds an extra 
 *  argument to each call, it provides a valid and useful service and 
 *  is thus justified. 
 * 
 *  Technical Notes: 
 *  -Controllers should publicize the target argument to scripts 
 *  -Clients that provide only one Target can ignore that argument 
 *  -All methods should use the STDCALL calling convention 
 *  -All methods must be implemented, even if empty 
 *  -Methods should be exported from a binary with no decoration 
 * 
 *  Implementation Notes: 
 *  It is assumed that this implementation will be in its own dynamic 
 *  linked library (shared library), however if the implementation is  
 *  be within a larger program and not used in a plugable way, the  
 *  method names may be modified to whatever end. What is important 
 *  is that the proper pointers are available to the Controller. 
 */ 
 
/** 
 *  EIOS_Initialize and EIOS_Finalize should be called after the 
 *  loading and before the unloading of a plugin, respectively. 
 *  This allows a Client to preform any global setup/cleanup in  
 *  a safe and standardized way. No methods may be used outside  
 *  of code bounded by these methods, and these methods must be  
 *  called in the specified order. 
 */ 
  
void EIOS_Initialize() __attribute__((stdcall)); 
void EIOS_Finalize() __attribute__((stdcall)); 
 
/** 
 *  EIOS_RequestTarget informs a Client that the Controller is ready  
 *  to communicate. The Client should do any Target setup required,  
 *  and may prompt users in an acceptable way for more information.  
 *  Clients should return a valid Target to the Controller to identify 
 *  future transactions. In most cases, after this method is invoked,  
 *  the Target specified should be ready to use, but some Clients may 
 *  require that further initialization be done with other, Client  
 *  specific, methods that are beyond the scope of this specification. 
 * 
 *  EIOS_ReleaseTarget informs a client that the Controller is done 
 *  with communication. The Client should do any Target cleanup  
 *  required, or may choose to do nothing at all, depending on the 
 *  implementation. In the case where a Target may be reused, or there 
 *  is only even one Target to provide, the Client may choose not to  
 *  destroy it, and provide it to a future call to EIOS_RequestTarget, 
 *  however Controllers may not assume that Targets remain valid after 
 *  making this call. 
 */ 
 
typedef Target void*  
 
Target EIOS_RequestTarget() __attribute__((stdcall)); 
void EIOS_ReleaseTarget(Target t) __attribute__((stdcall)); 
 
/** 
 *  The width and height may be obtained by passing pointers, or  
 *  variables by reference, to EIOS_GetTargetDimensions, which will fill 
 *  the pointers with the width and height of the client screen. This  
 *  method applies to the size of the Image Buffer and the valid area for  
 *  Mouse commands, although Clients should gracefully handle Mouse 
 *  positions outside this area. 
 */ 
  
void EIOS_GetTargetDimensions(Target t, long* width, long* height) __attribute__((stdcall)); 
 
/** 
 *  EIOS_GetImageBuffer and EIOS_UpdateImageBuffer both deal with the  
 *  Image acquisition of the Controller from the Client. The image  
 *  should be a static memory segment in the format defined by the  
 *  type RGB, i.e., EIOS_GetImageBuffer should return a pointer to 
 *  a memory segment containing RGB structures representing pixels  
 *  of the Target's image ordered by width first, then height,  
 *  starting in the top left (0,0) and continuing to the bottom right 
 *  (w-1,h-1) where w and h are the width and the height of the Target. 
 * 
 *  The Client is responsible for maintaining the image in this single  
 *  memory segment that is allocated by the client. It must not change 
 *  for a Target, i.e. if (TargetA == TargetB) then (ImageA == ImageB) 
 *  must always hold between calls to EIOS_RequestTarget and  
 *  EIOS_ReleaseTarget. After the final call, however, the Client may 
 *  reclaim the memory associated with the Buffer.  
  
 *  The Client may update the buffer as it pleases, but is only required  
 *  to update the buffer when EIOS_UpdateImageBuffer is called. 
 */ 
  
typedef union { 
	struct { char b, g, r, a; }; 
	unsigned long color; 
} rgb; 
 
*rgb EIOS_GetImageBuffer(Target t) __attribute__((stdcall)); 
void EIOS_UpdateImageBuffer(Target t) __attribute__((stdcall)); 
 
/** 
 *  Mouse control is abstracted into very basic and implementation  
 *  independent methods. A "mouse" is short for a pointing device with 
 *  two buttons that may be held, or released, while moving the cursor, 
 *  or not. The Controller is only aware of these operations, while the 
 *  Client must generate the proper events for its Target, e.g. if the 
 *  mouse is held and a move command is issued, the Client must know  
 *  how to generate the appropriate events for dragging, this is not an 
 *  issue for the Controller. In short, there is no "drag" option for 
 *  your computer mouse, only a composite of the events hold and move. 
 *  The Client interprets these as it will.  
 * 
 *  A Client is allowed to gracefully reject events, i.e. not take  
 *  action, if a Target's state forbids it. A Controller must be aware 
 *  of this and take appropriate action, which may be do nothing or  
 *  repeat. A Client may provide extra information about state to the 
 *  Controller, but that is beyond the scope of this implementation. 
 *  In the same way, a Client must gracefully accept out-of-bounds  
 *  positions from the Controller. It may take any action, including  
 *  no action, but suggested handling would be to simulate the  
 *  theoretical action occurring. 
 * 
 *  EIOS_GetMousePosition fills pointers with the current position 
 *  EIOS_MoveMouse sets the position to a new position, i.e. moving 
 *  EIOS_HoldMouse holds a mouse button at a specified position 
 *  EIOS_ReleaseMouse releases a mouse button at a specified position 
 *  EIOS_IsMouseHeld returns true if the specified button is held 
 * 
 *  For EIOS_HoldMouse and EIOS_ReleaseMouse, true specifies the left 
 *  mouse button and false specifies the right mouse button. There is  
 *  no specification on the order, or combination these methods may be 
 *  used in, Clients must accept all gracefully. Note that hold and 
 *  release methods take a position argument as well. This is both  
 *  for historical and compatibility reasons. A Client should generate 
 *  an event at this position, but may also generate extra events to 
 *  get there, if required. The principle of least surprise applies here, 
 *  a Controller is correct in assuming the cursor position after any 
 *  method with a position argument will be the specified position. 
 */ 
  
void EIOS_GetMousePosition(Target t, long* x, long* y) __attribute__((stdcall)); 
void EIOS_MoveMouse(Target t, long x, long y) __attribute__((stdcall)); 
void EIOS_HoldMouse(Target t, long x, long y, bool left) __attribute__((stdcall)); 
void EIOS_ReleaseMouse(Target t, long x, long y, bool left) __attribute__((stdcall)); 
bool EIOS_IsMouseHeld(Target t, bool left) __attribute__((stdcall)); 
 
/** 
 *  Keyboard control is abstracted to holds and releases of keys. In this 
 *  standard, a "key" is the value assigned to various keys in the Java 
 *  class, java.awt.event.KeyEvent for historical reasons. Since key  
 *  events need to be generated in a time-sensitive manner, and key 
 *  abstraction is presumed to be mostly used for printable characters 
 *  rather than physical keys, a convenience method is provided to send  
 *  a raw, null terminated, string to the Target. For this method, the  
 *  Client should generate appropriate events and take appropriate time, 
 *  whereas other methods in this standard are assumed to be theoretically 
 *  non-blocking. Clients must also maintain a map of which keys are held. 
 * 
 *  EIOS_SendString sends a null terminated string in a proper manner 
 *  EIOS_HoldKey hold the specified key 
 *  EIOS_ReleaseKey releases the specified key 
 *  EIOS_IsKeyHeld return true if the key is held 
 */ 
  
void EIOS_SendString(Target t, char* str) __attribute__((stdcall)); 
void EIOS_HoldKey(Target t, long key) __attribute__((stdcall)); 
void EIOS_ReleaseKey(Target t, long key) __attribute__((stdcall)); 
bool EIOS_IsKeyHeld(Target t, long key) __attribute__((stdcall)); 
  
