#!/usr/bin/python

"""
Tag header format:

(name:size in bits)

[flags:4] [id:4] [size:24] (if compressed: [uncompressed size:32])

flags: [reserved:3] [compressed:1]

"""

import struct
import zlib
import sys

FLAG_COMPRESSED = 0x1
MAGIC = "TAG1"

class WriteTagFile:
	def __init__(self, name):
		self.file = open(name, "wb")
		self.file.write(MAGIC)

	def getHeader(self, id, data, flags):
		return (flags << 28) + (id << 24) + len(data)

	def writeTag(self, id, data, flags=0):
		origsize = len(data)
		cdata = zlib.compress(data, 9)
		
#		if flags & FLAG_COMPRESSED:
		if len(cdata) < origsize:
			data = cdata
			flags |= FLAG_COMPRESSED
			
		header = self.getHeader(id, data, flags)
		self.file.write(struct.pack("l", header))
		
		if flags & FLAG_COMPRESSED:
			self.file.write(struct.pack("l", origsize))
			print "Wrote tag %2d: %d bytes, %d bytes compressed." % (id, origsize,len(data))
		else:
			print "Wrote tag %2d: %d bytes." % (id, len(data))
		
		self.file.write(data)

class ReadTagFile:
	def __init__(self, name):
		self.file = open(name, "rb")
		if not self.file.read(4) == MAGIC:
			raise UserWarning("Not a tag file.")
				
	def readTag(self):
		header = self.file.read(4)
		if not header:
			return -1
		
		(header,) = struct.unpack("l", header)
		
		self.tag_flags = (header>>28);
		self.tag_id = (header>>24) & 0xf;
		self.tag_size = header & 0x00ffffff;
		
		print "flags:", self.tag_flags
		print "id:", self.tag_id
		print "size:", self.tag_size
		
		if self.tag_flags & FLAG_COMPRESSED:
			self.tag_uncompressed_size = struct.unpack("l", self.file.read(4))
	
	def getData(self):
		data = self.file.read(self.tag_size)
		if self.tag_flags & FLAG_COMPRESSED:
			return zlib.decompress(data)
		return data
		
if __name__=="__main__":
#	f = WriteTagFile("test.tag")
#	f.writeTag(0, "abcd\0")
#	f.writeTag(1, "abcdefgh\0")

#	f = ReadTagFile("car.mesh")
	import binascii
	f = ReadTagFile(sys.argv[1])
	while f.readTag() != -1:
		data = f.getData()
		print len(data), binascii.hexlify(data)[:40]
			
