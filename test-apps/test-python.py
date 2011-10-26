#!/usr/bin/python

"""
 *  Copyright 2010 by Benjamin J. Land (a.k.a. BenLand100)
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
"""

from ctypes import *
import platform

class point:
	"""Dummy class for a generic point."""
	def __init__(self, x = 0, y = 0):
		self.x = x
		self.y = y
	def __str__(self):
		return '('+str(self.x)+','+str(self.y)+')'
		
class jobject:
	"""Represents Java objects returned by SMART."""
	def __init__(self, data = c_void_p(None)):
		self._as_parameter_ = data if data != None else c_void_p(None)
	def __str__(self):
		return '<jobject@'+str(self._as_parameter_.value)+'>'
		

class Smart:
	"""General SMART wrapper for python, requires v6.5 or later"""
	def __init__(self, root, args, width, height, initseq, jvmpath = None):
		"""Sets up the python bindings for SMART, sets the JVM if specified, 
		   and invokes the setup routine, assuming the libsmart library is
		   in the current path."""
		if platform.system() == 'Windows':
			self._dll = CDLL('./libsmart.dll')
		else:
			self._dll = CDLL('./libsmart.so')
		#JVM.h
		self._dll.setJVMPath.argtypes = [c_char_p]
		self._dll.setJVMPath.restype = None
		#Smart.h
		self._dll.getImageArray.argtypes = []
		self._dll.getImageArray.restype = c_long
		self._dll.setTransparentColor.argtypes = [c_long]
		self._dll.setTransparentColor.restype = None
		self._dll.setDebug.argtypes = [c_bool]
		self._dll.setDebug.restype = None
		self._dll.setGraphics.argtypes = [c_bool]
		self._dll.setGraphics.restype = None
		self._dll.getDebugArray.argtypes = []
		self._dll.getDebugArray.restype = c_long
		self._dll.isActive.argtypes = []
		self._dll.isActive.restype = c_bool
		self._dll.isBlocking.argtypes = []
		self._dll.isBlocking.restype = c_bool
		self._dll.getRefresh.argtypes = []
		self._dll.getRefresh.restype = c_long
		self._dll.setRefresh.argtypes = [c_long]
		self._dll.setRefresh.restype = None
		self._dll.setup.argtypes = [c_char_p,c_char_p,c_long,c_long,c_char_p]
		self._dll.setup.restype = None
		#Reflection.h
		self._dll.stringFromBytes.argtypes = [c_void_p, c_char_p]
		self._dll.stringFromBytes.restype = c_long
		self._dll.stringFromChars.argtypes = [c_void_p, c_char_p]
		self._dll.stringFromChars.restype = c_long
		self._dll.stringFromString.argtypes = [c_void_p, c_char_p]
		self._dll.stringFromString.restype = c_long
		self._dll.invokeIntMethod.argtypes = [c_void_p, c_char_p, c_char_p, c_long, c_long]
		self._dll.invokeIntMethod.restype = c_long
		self._dll.freeObject.argtypes = [c_void_p]
		self._dll.freeObject.restype = None
		self._dll.getFieldObject.argtypes = [c_void_p, c_char_p]
		self._dll.getFieldObject.restype = c_void_p
		self._dll.getFieldInt.argtypes = [c_void_p, c_char_p]
		self._dll.getFieldInt.restype = c_long
		self._dll.getFieldShort.argtypes = [c_void_p, c_char_p]
		self._dll.getFieldShort.restype = c_long
		self._dll.getFieldByte.argtypes = [c_void_p, c_char_p]
		self._dll.getFieldByte.restype = c_long
		self._dll.getFieldBool.argtypes = [c_void_p, c_char_p]
		self._dll.getFieldBool.restype = c_bool
		self._dll.getFieldLongH.argtypes = [c_void_p, c_char_p]
		self._dll.getFieldLongH.restype = c_long
		self._dll.getFieldLongL.argtypes = [c_void_p, c_char_p]
		self._dll.getFieldLongL.restype = c_long
		self._dll.getFieldFloat.argtypes = [c_void_p, c_char_p]
		self._dll.getFieldFloat.restype = c_float
		self._dll.getFieldDouble.argtypes = [c_void_p, c_char_p]
		self._dll.getFieldDouble.restype = c_float
		self._dll.getFieldArrayObject.argtypes = [c_void_p, c_char_p, c_long]
		self._dll.getFieldArrayObject.restype = c_void_p
		self._dll.getFieldArrayInt.argtypes = [c_void_p, c_char_p, c_long]
		self._dll.getFieldArrayInt.restype = c_long
		self._dll.getFieldArrayFloat.argtypes = [c_void_p, c_char_p, c_long]
		self._dll.getFieldArrayFloat.restype = c_float
		self._dll.getFieldArrayDouble.argtypes = [c_void_p, c_char_p, c_long]
		self._dll.getFieldArrayDouble.restype = c_float
		self._dll.getFieldArrayBool.argtypes = [c_void_p, c_char_p, c_long]
		self._dll.getFieldArrayBool.restype = c_bool
		self._dll.getFieldArrayLongH.argtypes = [c_void_p, c_char_p, c_long]
		self._dll.getFieldArrayLongH.restype = c_long
		self._dll.getFieldArrayLongL.argtypes = [c_void_p, c_char_p, c_long]
		self._dll.getFieldArrayLongL.restype = c_long
		self._dll.getFieldArrayByte.argtypes = [c_void_p, c_char_p, c_long]
		self._dll.getFieldArrayByte.restype = c_long
		self._dll.getFieldArrayShort.argtypes = [c_void_p, c_char_p, c_long]
		self._dll.getFieldArrayShort.restype = c_long
		self._dll.getFieldArrayChar.argtypes = [c_void_p, c_char_p, c_long]
		self._dll.getFieldArrayChar.restype = c_long
		self._dll.getFieldArray3DObject.argtypes = [c_void_p, c_char_p, c_long, c_long, c_long]
		self._dll.getFieldArray3DObject.restype = c_void_p
		self._dll.getFieldArray3DInt.argtypes = [c_void_p, c_char_p, c_long, c_long, c_long]
		self._dll.getFieldArray3DInt.restype = c_long
		self._dll.getFieldArray3DFloat.argtypes = [c_void_p, c_char_p, c_long, c_long, c_long]
		self._dll.getFieldArray3DFloat.restype = c_float
		self._dll.getFieldArray3DDouble.argtypes = [c_void_p, c_char_p, c_long, c_long, c_long]
		self._dll.getFieldArray3DDouble.restype = c_float
		self._dll.getFieldArray3DBool.argtypes = [c_void_p, c_char_p, c_long, c_long, c_long]
		self._dll.getFieldArray3DBool.restype = c_bool
		self._dll.getFieldArray3DLongH.argtypes = [c_void_p, c_char_p, c_long, c_long, c_long]
		self._dll.getFieldArray3DLongH.restype = c_long
		self._dll.getFieldArray3DLongL.argtypes = [c_void_p, c_char_p, c_long, c_long, c_long]
		self._dll.getFieldArray3DLongL.restype = c_long
		self._dll.getFieldArray3DByte.argtypes = [c_void_p, c_char_p, c_long, c_long, c_long]
		self._dll.getFieldArray3DByte.restype = c_long
		self._dll.getFieldArray3DShort.argtypes = [c_void_p, c_char_p, c_long, c_long, c_long]
		self._dll.getFieldArray3DShort.restype = c_long
		self._dll.getFieldArray3DChar.argtypes = [c_void_p, c_char_p, c_long, c_long, c_long]
		self._dll.getFieldArray3DChar.restype = c_long
		self._dll.getFieldArray2DObject.argtypes = [c_void_p, c_char_p, c_long, c_long]
		self._dll.getFieldArray2DObject.restype = c_void_p
		self._dll.getFieldArray2DInt.argtypes = [c_void_p, c_char_p, c_long, c_long]
		self._dll.getFieldArray2DInt.restype = c_long
		self._dll.getFieldArray2DFloat.argtypes = [c_void_p, c_char_p, c_long, c_long]
		self._dll.getFieldArray2DFloat.restype = c_float
		self._dll.getFieldArray2DDouble.argtypes = [c_void_p, c_char_p, c_long, c_long]
		self._dll.getFieldArray2DDouble.restype = c_float
		self._dll.getFieldArray2DBool.argtypes = [c_void_p, c_char_p, c_long, c_long]
		self._dll.getFieldArray2DBool.restype = c_bool
		self._dll.getFieldArray2DLongH.argtypes = [c_void_p, c_char_p, c_long, c_long]
		self._dll.getFieldArray2DLongH.restype = c_long
		self._dll.getFieldArray2DLongL.argtypes = [c_void_p, c_char_p, c_long, c_long]
		self._dll.getFieldArray2DLongL.restype = c_long
		self._dll.getFieldArray2DByte.argtypes = [c_void_p, c_char_p, c_long, c_long]
		self._dll.getFieldArray2DByte.restype = c_long
		self._dll.getFieldArray2DShort.argtypes = [c_void_p, c_char_p, c_long, c_long]
		self._dll.getFieldArray2DShort.restype = c_long
		self._dll.getFieldArray2DChar.argtypes = [c_void_p, c_char_p, c_long, c_long]
		self._dll.getFieldArray2DChar.restype = c_long
		self._dll.getFieldArraySize.argtypes = [c_void_p, c_char_p, c_long]
		self._dll.getFieldArraySize.restype = c_long
		self._dll.isEqual.argtypes = [c_void_p, c_void_p]
		self._dll.isEqual.restype = c_bool
		self._dll.isNull.argtypes = [c_void_p]
		self._dll.isNull.restype = c_bool
		self._dll.isPathValid.argtypes = [c_void_p, c_char_p]
		self._dll.isPathValid.restype = c_bool
		#Input.h
		self._dll.sendKeys.argtypes = [c_char_p]
		self._dll.sendKeys.restype = None
		self._dll.isKeyDown.argtypes = [c_long]
		self._dll.isKeyDown.restype = c_bool
		self._dll.holdKey.argtypes = [c_long]
		self._dll.holdKey.restype = None
		self._dll.releaseKey.argtypes = [c_long]
		self._dll.releaseKey.restype = None
		self._dll.getMousePos.argtypes = [POINTER(c_long), POINTER(c_long)]
		self._dll.getMousePos.restype = None
		self._dll.moveMouse.argtypes = [c_long, c_long]
		self._dll.moveMouse.restype = None
		self._dll.windMouse.argtypes = [c_long, c_long]
		self._dll.windMouse.restype = None
		self._dll.dragMouse.argtypes = [c_long, c_long]
		self._dll.dragMouse.restype = None
		self._dll.holdMouse.argtypes = [c_long, c_long, c_bool]
		self._dll.holdMouse.restype = None
		self._dll.releaseMouse.argtypes = [c_long, c_long, c_bool]
		self._dll.releaseMouse.restype = None
		self._dll.clickMouse.argtypes = [c_long, c_long, c_bool]
		self._dll.clickMouse.restype = None
		#Color.h
		self._dll.getColor.argtypes = [c_long, c_long]
		self._dll.getColor.restype = c_long
		self._dll.findColor.argtypes = [POINTER(c_long), POINTER(c_long), c_long, c_long, c_long, c_long, c_long]
		self._dll.findColor.restype = c_bool
		self._dll.findColorTol.argtypes = [POINTER(c_long), POINTER(c_long), c_long, c_long, c_long, c_long, c_long, c_long]
		self._dll.findColorTol.restype = c_bool
		self._dll.findColorSpiral.argtypes = [POINTER(c_long), POINTER(c_long), c_long, c_long, c_long, c_long, c_long]
		self._dll.findColorSpiral.restype = c_bool
		self._dll.findColorSpiralTol.argtypes = [POINTER(c_long), POINTER(c_long), c_long, c_long, c_long, c_long, c_long, c_long]
		self._dll.findColorSpiralTol.restype = c_bool
		#set jvm path and invoke setup
		if jvmpath != None:
			self._dll.setJVMPath(jvmpath);
		self._dll.setup(root,args,width,height,initseq)

	#from JVM.cpp

	def setJVMPath(self, path):
		"""Specify a JVM for SMART to use. May be called at any time, but only 
		   affects the next full loading cycle of SMART."""
		setJVMPath(path)

	#from Smart.cpp

	def getImageArray(self):
		"""Returns a c_void_p to SMART's image buffer array."""
		res = self._dll.getImageArray()
		return res.value

	def getDebugArray(self):
		"""Returns a c_void_p to SMART's debug drawing array."""
		res = self._dll.getDebugArray()
		return res.value

	def setTransparentColor(self, color):
		"""Sets the color (int value) that is transparent on the debug image."""
		self._dll.setTransparentColor(color)

	def setDebug(self, on):
		"""Enables or disables the debug drawing."""
		self._dll.setDebug(on)

	def setGraphics(self, on):
		"""Enables or disables renderig graphics on the screen."""
		self._dll.setGraphics(on)
		
	def isActive(self):
		"""Specifies whether or not SMART has loaded."""
		res = self._dll.isActive()
		return res.value

	def isBlocking(self):
		"""Specifies whether or not SMART is blocking events. (i.e. whether 
		   SMART is enabled or disabled.)"""
		res = self._dll.isBlocking()
		return res.value

	def getRefresh(self):
		"""Returns the value of the FPS slider."""
		res = self._dll.getRefresh()
		return res.value

	def setRefresh(self, x):
		"""Sets the value of the FPS slider."""
		self._dll.setRefresh(x)

	def setup(self, root, args, w, h, initseq):
		"""(Re)Initilizes SMART with the given arguments."""
		self._dll.setup(root, args, w, h, initseq)

	#from Reflection.cpp

	def stringFromBytes(self, parent, path):
		"""Returns a string from a byte array."""
		res = self._dll.stringFromBytes(parent, path)
		return res.value

	def stringFromChars(self, parent, path):
		"""Returns a string from a char array."""
		res = self._dll.stringFromChars(parent, path)
		return res.value

	def stringFromString(self, parent, path):
		"""Returns a string from a string object."""
		res = self._dll.stringFromString(parent, path)
		return res.value

	def invokeIntMethod(self, obj, classname, methname, a, b):
		"""Can invoke an int method with two int arguments - not recommended"""
		res = self._dll.invokeIntMethod(obj, classname, methname, a, b)
		return res.value

	def freeObject(self, parent):
		"""Frees Object values. Must be called exactly once for every object."""
		self._dll.freeObject(parent)

	def getFieldObject(self, parent, path):
		"""Gets an Object from a parent/path combo."""
		res = self._dll.getFieldObject(parent, path)
		return jobject(res)

	def getFieldInt(self, parent, path):
		"""Gets an int from a parent/path combo."""
		res = self._dll.getFieldInt(parent, path)
		return res.value

	def getFieldShort(self, parent, path):
		"""Gets a short from a parent/path combo."""
		res = self._dll.getFieldShort(parent, path)
		return res.value

	def getFieldByte(self, parent, path):
		"""Gets a byte from a parent/path combo."""
		res = self._dll.getFieldByte(parent, path)
		return res.value

	def getFieldBool(self, parent, path):
		"""Gets a boolean from a parent/path combo."""
		res = self._dll.getFieldBool(parent, path)
		return res.value

	def getFieldLong(self, parent, path):
		"""Gets a long from a parent/path combo."""
		h = self._dll.getFieldLongH(parent, path).value
		l = self._dll.getFieldLongL(parent, path).value
		return (h << 32) | l

	def getFieldFloat(self, parent, path):
		"""Gets a float from a parent/path combo."""
		res = self._dll.getFieldFloat(parent, path)
		return res.value

	def getFieldDouble(self, parent, path):
		"""Gets a double from a parent/path combo."""
		res = self._dll.getFieldDouble(parent, path)
		return res.value

	def getFieldArrayObject(self, parent, path, i):
		"""Gets an Object from an index in an array at a parent/path combo."""
		res = self._dll.getFieldArrayObject(parent, path, i)
		return jobject(res)

	def getFieldArrayInt(self, parent, path, i):
		"""Gets an int from an index in an array at a parent/path combo."""
		res = self._dll.getFieldArrayInt(parent, path, i)
		return res.value

	def getFieldArrayFloat(self, parent, path, i):
		"""Gets a float from an index in an array at a parent/path combo."""
		res = self._dll.getFieldArrayFloat(parent, path, i)
		return res.value

	def getFieldArrayDouble(self, parent, path, i):
		"""Gets a double from an index in an array at a parent/path combo."""
		res = self._dll.getFieldArrayDouble(parent, path, i)
		return res.value

	def getFieldArrayBool(self, parent, path, i):
		"""Gets a boolean from an index in an array at a parent/path combo."""
		res = self._dll.getFieldArrayBool(parent, path, i)
		return res.value

	def getFieldArrayLong(self, parent, path, i):
		"""Gets a long from an index in an array at a parent/path combo."""
		h = self._dll.getFieldArrayLongH(parent, path, i).value
		l = self._dll.getFieldArrayLongL(parent, path, i).value
		return (h << 32) | l

	def getFieldArrayByte(self, parent, path, i):
		"""Gets a byte from an index in an array at a parent/path combo."""
		res = self._dll.getFieldArrayByte(parent, path, i)
		return res.value

	def getFieldArrayShort(self, parent, path, i):
		"""Gets a short from an index in an array at a parent/path combo."""
		res = self._dll.getFieldArrayShort(parent, path, i)
		return res.value

	def getFieldArrayChar(self, parent, path, i):
		"""Gets a char from an index in an array at a parent/path combo."""
		res = self._dll.getFieldArrayChar(parent, path, i)
		return res.value

	def getFieldArray3DObject(self, parent, path, i, j, k):
		"""Gets an Object from an index in a 3D array at a parent/path combo."""
		res = self._dll.getFieldArray3DObject(parent, path, i, j, k)
		return jobject(res)

	def getFieldArray3DInt(self, parent, path, i, j, k):
		"""Gets an int from an index in a 3D array at a parent/path combo."""
		res = self._dll.getFieldArray3DInt(parent, path, i, j, k)
		return res.value

	def getFieldArray3DFloat(self, parent, path, i, j, k):
		"""Gets a float from an index in a 3D array at a parent/path combo."""
		res = self._dll.getFieldArray3DFloat(parent, path, i, j, k)
		return res.value

	def getFieldArray3DDouble(self, parent, path, i, j, k):
		"""Gets a double from an index in a 3D array at a parent/path combo."""
		res = self._dll.getFieldArray3DDouble(parent, path, i, j, k)
		return res.value

	def getFieldArray3DBool(self, parent, path, i, j, k):
		"""Gets a boolean from an index in a 3D array at a parent/path combo."""
		res = self._dll.getFieldArray3DBool(parent, path, i, j, k)
		return res.value

	def getFieldArray3DLong(self, parent, path, i, j, k):
		"""Gets a long from an index in a 3D array at a parent/path combo."""
		h = self._dll.getFieldArray3DLongH(parent, path, i, j, k).value
		l = self._dll.getFieldArray3DLongL(parent, path, i, j, k).value
		return (h << 32) | l

	def getFieldArray3DByte(self, parent, path, i, j, k):
		"""Gets a byte from an index in a 3D array at a parent/path combo."""
		res = self._dll.getFieldArray3DByte(parent, path, i, j, k)
		return res.value

	def getFieldArray3DShort(self, parent, path, i, j, k):
		"""Gets a short from an index in a 3D array at a parent/path combo."""
		res = self._dll.getFieldArray3DShort(parent, path, i, j, k)
		return res.value

	def getFieldArray3DChar(self, parent, path, i, j, k):
		"""Gets a char from an index in a 3D array at a parent/path combo."""
		res = self._dll.getFieldArray3DChar(parent, path, i, j, k)
		return res.value

	def getFieldArray2DObject(self, parent, path, i, j):
		"""Gets an Object from an index in a 2D array at a parent/path combo."""
		res = self._dll.getFieldArray2DObject(parent, path, i, j)
		return jobject(res)

	def getFieldArray2DInt(self, parent, path, i, j):
		"""Gets an int from an index in a 2D array at a parent/path combo."""
		res = self._dll.getFieldArray2DInt(parent, path, i, j)
		return res.value

	def getFieldArray2DFloat(self, parent, path, i, j):
		"""Gets a float from an index in a 2D array at a parent/path combo."""
		res = self._dll.getFieldArray2DFloat(parent, path, i, j)
		return res.value

	def getFieldArray2DDouble(self, parent, path, i, j):
		"""Gets a double from an index in a 2D array at a parent/path combo."""
		res = self._dll.getFieldArray2DDouble(parent, path, i, j)
		return res.value

	def getFieldArray2DBool(self, parent, path, i, j):
		"""Gets a boolean from an index in a 2D array at a parent/path combo."""
		res = self._dll.getFieldArray2DBool(parent, path, i, j)
		return res.value

	def getFieldArray2DLong(self, parent, path, i, j):
		"""Gets a long from an index in a 2D array at a parent/path combo."""
		h = self._dll.getFieldArray2DLongH(parent, path, i, j).value
		l = self._dll.getFieldArray2DLongL(parent, path, i, j).value
		return (h << 32) | l

	def getFieldArray2DByte(self, parent, path, i, j):
		"""Gets a byte from an index in a 2D array at a parent/path combo."""
		res = self._dll.getFieldArray2DByte(parent, path, i, j)
		return res.value

	def getFieldArray2DShort(self, parent, path, i, j):
		"""Gets a short from an index in a 2D array at a parent/path combo."""
		res = self._dll.getFieldArray2DShort(parent, path, i, j)
		return res.value

	def getFieldArray2DChar(self, parent, path, i, j):
		"""Gets a char from an index in a 2D array at a parent/path combo."""
		res = self._dll.getFieldArray2DChar(parent, path, i, j)
		return res.value

	def getFieldArraySize(self, parent, path, dim):
		"""Returns the size of a dimension of an array at a parent/path combo."""
		res = self._dll.getFieldArraySize(parent, path, dim)
		return res.value

	def isEqual(self, a, b):
		"""Tests the equality of two Objects."""
		res = self._dll.isEqual(a, b)
		return res.value

	def isNull(self, obj):
		"""Tests if an Object is really NULL or just pretending."""
		res = self._dll.isNull(obj)
		return res.value

	def isPathValid(self, parent, path):
		"""Tests the validity of a parent/path combo."""
		res = self._dll.isPathValid(parent, path)
		return res.value

	#from Input.cpp

	def sendKeys(self, string):
		"""Sends a string of characters to the client in a human way."""
		self._dll.sendKeys(string)

	def isKeyDown(self, c):
		"""Tests if a key code is down."""
		res = self._dll.isKeyDown(c)
		return res.value

	def holdKey(self, c):
		"""Holds the specified key code."""
		self._dll.holdKey(c)

	def releaseKey(self, c):
		"""Releases the specified key code."""
		self._dll.releaseKey(c)

	def getMousePos(self):
		"""Returns the current mouse position."""
		x = c_long()
		y = c_long()
		self._dll.getMousePos(byref(x), byref(y))
		return point(x.value,y.value)

	def moveMouse(self, x, y):
		"""Sets the mouse to the specified position."""
		self._dll.moveMouse(x, y)
		
	def holdMouse(self, x, y, left):
		"""Holds the mouse (left or right) at the spcecified position."""
		self._dll.holdMouse(x, y, left)

	def releaseMouse(self, x, y, left):
		"""Releases the mouse (left or right) at the specified position."""
		self._dll.releaseMouse(x, y, left)

	def clickMouse(self, x, y, left):
		"""Clicks the mouse (left or right) humanly at the specified position."""
		self._dll.clickMouse(x, y, left)

	def windMouse(self, x, y):
		"""Moves the mouse humanly to a specified position."""
		self._dll.windMouse(x, y)

	def dragMouse(self, x, y):
		"""Drags the mouse humanly to a specified position."""
		self._dll.dragMouse(x, y)

	#from Color.cpp

	def getColor(self, x, y):
		"""Quickly gets the color from the specified point."""
		res = self._dll.getColor(x, y)
		return res.value

	def findColor(self, x, y, color, xs, ys, xe, ye):
		"""Quickly finds the color from the specified point in the given box."""
		xx = c_long(x)
		yy = c_long(y)
		res = self._dll.findColor(byref(xx), byref(yy), color, xs, ys, xe, ye)
		if res.value:
			return point(xx.value,yy.value)
		return None

	def findColorTol(self, x, y, color, xs, ys, xe, ye, tol):
		"""Quickly finds the color with tolerance from the specified point in the given box."""
		xx = c_long(x)
		yy = c_long(y)
		res = self._dll.findColorTol(byref(xx), byref(yy), color, xs, ys, xe, ye, tol)
		if res.value:
			return point(xx.value,yy.value)
		return None

	def findColorSpiral(self, x, y, color, xs, ys, xe, ye):
		"""Quickly finds the color from the specified point in the given box using a spiral."""
		xx = c_long(x)
		yy = c_long(y)
		res = self._dll.findColorSpiral(byref(xx), byref(yy), color, xs, ys, xe, ye)
		if res.value:
			return point(xx.value,yy.value)
		return None

	def findColorSpiralTol(self, x, y, color, xs, ys, xe, ye, tol):
		"""Quickly finds the color with tolerance from the specified point in the given box using a spiral."""
		xx = c_long(x)
		yy = c_long(y)
		res = self._dll.findColorSpiralTol(byref(xx), byref(yy), color, xs, ys, xe, ye, tol)
		if res.value:
			return point(xx.value,yy.value)
		return None

if __name__ == "__main__":
	smart = Smart('http://world19.runescape.com/', 'plugin.js?param=o0,a2,m0', 765, 503, '')
	import time
	while True:
	    time.sleep(1)

