#!/usr/bin/python

from Dice3DS import *
from tagfile import WriteTagFile
import sys
import struct

#scale = 1.0/1024.0
scale = 1.0/256.0
#scale = 1.0/16.0
#scale = 1.0/4.0
#scale = 1.0

def fixedPoint(f,scale = 1):
	return long(f*65536.0*scale)

def loadTexture(*foo):
	pass

def convert3DSToMesh(srcFile, destFile):
	dom = dom3ds.read_3ds_file(srcFile)
	model = basicmodel.BasicModel(dom,loadTexture)	
	output = WriteTagFile(destFile)
	
	print "%s: %d mesh(es):" % (srcFile, len(model.meshes))
	
	for mesh in model.meshes:
		print "%s: %d vertices, %d faces" % (srcFile, len(mesh.points), len(mesh.facearray))
		data = ""
		for i in xrange(0, len(mesh.points)):
			try:
				(x, y, z) = mesh.points[i]
				
				try:
					(u,v) = mesh.tvertarray[i]
				except TypeError:
					(u,v) = (0,0)
				except IndexError:
					(u,v) = (0,0)
					
				print u, v
				
				x = fixedPoint(x,scale)
				y = fixedPoint(y,scale)
				z = fixedPoint(z,scale)
				u = fixedPoint(u)
				v = fixedPoint(v)
				
				data += struct.pack("lllll", x, y, z, u, v)
			except IndexError:
				pass
		output.writeTag(0, data)
								
		data = ""
		for (a,b,c) in mesh.facearray:
			data += struct.pack("hhh", a, b, c)
		output.writeTag(1, data)

if not len(sys.argv) == 3:	
	print "Usage: makemesh.py src.3ds target.mesh"
else:
	convert3DSToMesh(sys.argv[1], sys.argv[2])
