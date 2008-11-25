#!/usr/bin/python

import sys
import Image
import ImageChops

# Requires PyFT freetype wrapper
import freetype

characters = "".join([chr(x) for x in range(33,127)])

def drawString(text, font, size=24):
	e = freetype.TT_Init_FreeType()
	
	face = freetype.TT_Open_Face(e, font)
	
	props = freetype.TT_Get_Face_Properties(face)
	
	ins = freetype.TT_New_Instance(face)
	freetype.TT_Set_Instance_Resolutions(ins, 96, 96)
	freetype.TT_Set_Instance_PointSize(ins, size)
	
	for id in range(props.num_CharMaps):
		tup = freetype.TT_Get_CharMap_ID(face, id)
		if tup == (3,1):
			# Windows Unicode mapping
			cm = freetype.TT_Get_CharMap(face, id)
			break
	
	glyphIndices = freetype.PyTT_String_Indices(cm, text)
	
	glyphCache = {}
	metricCache = {}
	minyMin = 0
	maxyMax = 0
	xSpacing = 1<<6
	ySpacing = 1<<6
	
	for i in range(len(text)):
		if not glyphCache.has_key(text[i]):
			glyphCache[text[i]] = freetype.TT_New_Glyph(face)
			freetype.TT_Load_Glyph(ins, glyphCache[text[i]], glyphIndices[i], freetype.TTLOAD_DEFAULT)
			metricCache[text[i]] = freetype.TT_Get_Glyph_Metrics(glyphCache[text[i]])
			maxyMax = max(maxyMax, metricCache[text[i]].bbox.yMax)
			if i != 0:
				minyMin = min(minyMin, metricCache[text[i]].bbox.yMin)
			else:
				minyMin = metricCache[text[i]].bbox.yMin
	
	width = 0
	for i in text:
		width = width + metricCache[i].advance + xSpacing
	width = width / 64
	
	height = (maxyMax - minyMin) / 64
	raster = freetype.TT_Raster_Map(width, height + (ySpacing>>6), 'bit')

	curX = 0

	for char in text:
		freetype.TT_Get_Glyph_Bitmap(glyphCache[char], raster, curX - metricCache[char].bbox.xMin + xSpacing/2, -minyMin)
		curX = curX + metricCache[char].advance + xSpacing

	img = freetype.toImage(raster)
	
	curX = 0
	for i in text:
		img.putpixel((curX>>6, 0), 0)
		curX += metricCache[i].advance + xSpacing

	img = ImageChops.invert(img)
	img = img.convert("RGB") # remove alpha channel
			
	return img

def ttfToBitmapFont(fontFile, outputFile, pointSize):
	img = drawString(characters, font=fontFile, size=pointSize)
	img.save(outputFile)

if __name__=="__main__":
	if len(sys.argv)<3:
		print "Usage: makefont.py font.ttf output [point size]"
		sys.exit(1)
	
	if len(sys.argv)==4:
		pointSize = int(sys.argv[3])
	else:
		pointSize = 10
		
	ttfToBitmapFont(sys.argv[1], sys.argv[2], pointSize)
