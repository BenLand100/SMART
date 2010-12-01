#!/usr/bin/python
from ctypes import CDLL
from time import sleep
d = CDLL('../dist/libsmart.so')
d.setup('http://world19.runescape.com/', 'plugin.js?param=o0,a2,m0', 765,503, '')
sleep(1000000)

