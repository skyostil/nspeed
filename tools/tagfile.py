#!/usr/bin/python

"""
Tag header format:

(name:size in bits)

[flags:4] [id:4] [size:24] (if compressed: [uncompressed size:32])

flags: [reserved:3] [compressed:1]

"""

import struct
import zlib

FLAG_COMPRESSED = 0x1

class WriteTagFile:
	def __init__(self, name):
		self.file = open(name, "wb")

	def getHeader(self, id, data, flags):
		return (flags << 28) + (id << 24) + len(data)

	def writeTag(self, id, data, flags=0):
		origsize = len(data)
		
		if flags & FLAG_COMPRESSED:
			data = zlib.compress(data, 9)
			
		header = self.getHeader(id, data, flags)
		self.file.write(struct.pack("l", header))
		
		if flags & FLAG_COMPRESSED:
			self.file.write(struct.pack("l", origsize))
			print "Wrote tag %2d: %d bytes, %d bytes compressed." % (id, origsize,len(data))
		else:
			print "Wrote tag %2d: %d bytes." % (id, len(data))
		
		self.file.write(data)

if __name__=="__main__":
	f = WriteTagFile("test.tag")
	f.writeTag(0, "abcd\0", FLAG_COMPRESSED)
	f.writeTag(0, "abcd\0", FLAG_COMPRESSED)
