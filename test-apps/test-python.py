#!/usr/bin/python

"""
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
"""

from ctypes import *
import platform, sys

class point:
	"""Dummy class for a generic point."""
	def __init__(self, x = 0, y = 0):
		self.x = x
		self.y = y
	def __str__(self):
		return '('+str(self.x)+','+str(self.y)+')'

class Smart:
	"""General SMART wrapper for python, requires v8.0 or later"""
	def __init__(self, root, args, width, height, initseq, jvmpath = None):
		"""Sets up the python bindings for SMART, sets the JVM if specified, 
		   and invokes the setup routine, assuming the libsmart library is
		   in the current path."""
		bitstr = '64' if sys.maxsize > 2**32 else '32'
		if platform.system() == 'Windows':
			self._dll = CDLL('./libsmart'+bitstr+'.dll')
		else:
			self._dll = CDLL('./libsmart'+bitstr+'.so')
	    #Remote
	    self._dll.exp_clientID.argtypes = [c_long];
	    self._dll.exp_clientID.restype = c_long;
        self._dll.exp_getClients.argtypes = [c_bool];
        self._dll.exp_getClients.restype = c_long;
        self._dll.exp_pairClient.argtypes = [c_long];
        self._dll.exp_pairClient.restype = c_bool;
        self._dll.exp_getCurrent.argtypes = [];
        self._dll.exp_getCurrent.restype = c_long;
        self._dll.exp_killClient.argtypes = [c_long];
        self._dll.exp_killClient.restype = c_bool;
		self._dll.spawnClient.argtypes = [c_char_p,c_char_p,c_char_p,c_long,c_long,c_char_p,c_char_p,c_char_p]
		self._dll.spawnClient.restype = c_long
		#Smart
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
		#Input
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
		self._dll.holdMouse.argtypes = [c_long, c_long, c_bool]
		self._dll.holdMouse.restype = None
		self._dll.releaseMouse.argtypes = [c_long, c_long, c_bool]
		self._dll.releaseMouse.restype = None
		self._dll.clickMouse.argtypes = [c_long, c_long, c_bool]
		self._dll.clickMouse.restype = None
		self._dll.holdMousePlus.argtypes = [c_long, c_long, c_long]
		self._dll.holdMousePlus.restype = None
		self._dll.releaseMousePlus.argtypes = [c_long, c_long, c_long]
		self._dll.releaseMousePlus.restype = None
		self._dll.clickMousePlus.argtypes = [c_long, c_long, c_long]
		self._dll.clickMousePlus.restype = None

    #Remote

	def getClientID(self,idx):
		"""Returns the ID of the client at internal index idx."""
		res = self._dll.exp_clientID(idx)
		return res.value

	def getClients(self,only_unpaired=True):
		"""Populates the internal client list with unpaired or all clients."""
		res = self._dll.exp_getClients(only_unpaired)
		return res.value
    
	def pairClient(self,pid):
		"""Attempts to pair to the client SMART.PID."""
		res = self._dll.exp_pairClient(pid)
		return res.value
    
	def getCurrent(self):
		"""Returns the PID of the currently paired client."""
		res = self._dll.exp_getCurrent()
		return res.value
    
	def killClient(self,pid):
		"""Attempts to kill to the client SMART.PID."""
		res = self._dll.exp_killClient(pid)
		return res.value
		
    
	def spawnClient(self,remote_path,root,params,width,height,initseq,useragent,javaargs):
		"""Attempts to kill to the client SMART.PID."""
		res = self._dll.exp_killClient(pid)
		return res.value
		
		self._dll.spawnClient.argtypes = [c_char_p,c_char_p,c_char_p,c_long,c_long,c_char_p,c_char_p,c_char_p]
		self._dll.spawnClient.restype = c_long

	#from Smart

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

	#from Input

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
		
	def holdMousePlus(self, x, y, button):
		"""Holds the mouse (left or right) at the spcecified position."""
		self._dll.holdMousePlus(x, y, button)

	def releaseMousePlus(self, x, y, button):
		"""Releases the mouse (left or right) at the specified position."""
		self._dll.releaseMousePlus(x, y, button)

	def clickMousePlus(self, x, y, button):
		"""Clicks the mouse (left or right) humanly at the specified position."""
		self._dll.clickMousePlus(x, y, button)

	def windMouse(self, x, y):
		"""Moves the mouse humanly to a specified position."""
		self._dll.windMouse(x, y)

if __name__ == "__main__":
        smart = Smart('http://world19.runescape.com/', ',f5', 765, 503, '')
        import time
        from threading import *
        t = Timer(10.0, smart.hardReset)
        t.start()
        time.sleep(50)
        
        
        
        #import random
        #while True:
        #    time.sleep(1)
        #    smart.windMouse(int(random.random()*700),int(random.random()*500))
