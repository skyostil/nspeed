#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import struct
import sys
from tagfile import WriteTagFile

size = (256, 256)
offset = (128, 128)

gates = [
        (0,0), (0,0),
        (0,0), (0,0),
        (0,0), (0,0),
        (0,0), (0,0),
]
gateStart = 64
gateEnd = 71

aiPath = [(0,0)] * 32
aiStart = 96
aiEnd = 127
aiLen = 0
tileLimit = 63

def findSpecialTiles(data):
    global aiPath, aiLen, gates
    for y in xrange(0, size[1]):
        for x in xrange(0, size[0]):
            d = ord(data[y*size[1]+x])
            if d >= gateStart and d <= gateEnd:
                gates[d - gateStart] = (x,y)
            elif d >= aiStart and d <= aiEnd:
                aiPath[d - aiStart] = (x,y)
                if d - aiStart + 1 > aiLen:
                    aiLen = d - aiStart + 1

def lookup(data, x, y):
	return data[((y+offset[1]) % size[1]) * size[1] + ((x+offset[0]) % size[0])]

def transform(data):
    offsetData = ""
    
    for y in xrange(0, size[1]):
    	for x in xrange(0, size[0]):
                tile = lookup(data,x,y)
                if ord(tile)>tileLimit:
        		offsetData += offsetData[-1]
        	else:
        		offsetData += tile
    return offsetData


if not len(sys.argv)>1:
	print "Rata puuttuu, hölö! [file.map]"
	sys.exit(1)
	
data = open(sys.argv[1]).read()
output = WriteTagFile(sys.argv[1].replace(".map",".trk"))

findSpecialTiles(data)

print "AI Path:"
print aiPath[:aiLen]
print "Gates:"
print gates

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
output.writeTag(1, transform(data))

d = ""
for pos in aiPath[:aiLen]:
    d+=struct.pack("hh", pos[0], pos[1])
output.writeTag(2, d)

    

