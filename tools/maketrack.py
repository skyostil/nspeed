#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import struct
import sys
from tagfile import WriteTagFile

size = (256, 256)
offset = (128, 128)

def FP(i):
	return i

gates = [
	(FP(120), FP(128)), (FP(136), FP(128)), 
	(FP(120), FP(138)), (FP(136), FP(138)), 
	(FP(0),   FP(0)),   (FP(0),   FP(0)), 
	(FP(0),   FP(0)),   (FP(0),   FP(0)), 
]

def lookup(data, x, y):
	return data[((y+offset[1]) % size[1]) * size[1] + ((x+offset[0]) % size[0])]

if not len(sys.argv)>1:
	print "Rata puuttuu, hölö! [file.map]"
	sys.exit(1)
	
data = open(sys.argv[1]).read()
output = WriteTagFile(sys.argv[1].replace(".map",".trk"))

offsetData = ""

for y in xrange(0, size[1]):
	for x in xrange(0, size[0]):
		offsetData += lookup(data,x,y)

output.writeTag(0,
	struct.pack(
		"HHhhhhhhhhhhhhhhhh",
		size[0], size[1],
		gates[0][0], gates[0][1], gates[1][0], gates[1][1],
		gates[2][0], gates[2][1], gates[3][0], gates[3][1],
		gates[4][0], gates[4][1], gates[5][0], gates[5][1],
		gates[6][0], gates[6][1], gates[7][0], gates[7][1]
	)
)
output.writeTag(1, offsetData)
