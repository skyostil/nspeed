#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import zlib
import Image
import ImageChops
import struct
import sys

offset = (80, 150)

if not len(sys.argv)>1:
	print "Rata puuttuu, hölö!"
	sys.exit(1)
	
i = Image.open(sys.argv[1])
o = open(sys.argv[1].replace(".png",".trk"),"wb")

i = ImageChops.offset(i, offset[0], offset[1])

"""
last = None
runlen = 0
bytes = 0
compressed = ""
for y in range(0,i.size[1]):
    for x in range(0,i.size[0]):
        p = i.getpixel((x,y))
        if p == last and runlen<255:
            runlen += 1
        else:
            if runlen:
                compressed += chr(p | 0x80) + chr(runlen)
            else:
                compressed += chr(p)
            last = p
            runlen = 0

print dir(i)

print len(compressed)
print len(zlib.compress(compressed, 9))
print len(zlib.compress(i.tostring(), 9))
"""

uncompressed = i.tostring()
compressed = zlib.compress(uncompressed, 9)
o.write(struct.pack('HH', i.size[0], i.size[1]))
o.write(compressed)
o.close()

print "%dx%d image, compressed from %d to %d bytes." % (i.size[0], i.size[1], len(uncompressed), len(compressed))
