# loadtexture.py

from OpenGL.GL import *
from OpenGL.GLU import *
from PIL import Image
from zipfile import ZipFile
from cStringIO import StringIO
import os


def texture_image_2d(image, mipmap):
	if image.mode == 'RGB':
		data = image.tostring("raw","RGBX",0,-1)
	elif image.mode == 'RGBA':
		data = image.tostring("raw","RGBA",0,-1)
	else:
		assert False, image.mode
	width, height = image.size
	if mipmap:
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height,
				  GL_RGBA, GL_UNSIGNED_BYTE, data)
	else:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
			     0, GL_RGBA, GL_UNSIGNED_BYTE, data)


def texture_image_1d(image, mipmap):
	if image.mode == 'RGB':
		data = image.tostring("raw","RGBX",0,-1)
	elif image.mode == 'RGBA':
		data = image.tostring("raw","RGBA",0,-1)
	else:
		assert False, image.mode
	length = max(image.size)
	if mipmap:
		gluBuild1DMipmaps(GL_TEXTURE_1D, GL_RGBA, length,
				  GL_RGBA, GL_UNSIGNED_BYTE, data)
	else:
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, length,
			     0, GL_RGBA, GL_UNSIGNED_BYTE, data)


def _up_power_of_2(n):
	assert n > 0
	i = 1
	while i < n:
		i *= 2
	return i


def scale_to_power_of_2(image):
	width, height = image.size
	pw = _up_power_of_2(width)
	ph = _up_power_of_2(height)
	if (width,height) != (pw,ph):
		return image.resize((pw,ph),Image.BICUBIC)
	return image


def create_texture_from_image(image, wrap, magfilter, minfilter):
	if image.mode not in('RGB', 'RGBA'):
		image = image.convert("RGB")

	width,height = image.size
	image = scale_to_power_of_2(image)

	mipmap_filters = (GL_NEAREST_MIPMAP_NEAREST,
			  GL_NEAREST_MIPMAP_LINEAR,
			  GL_LINEAR_MIPMAP_NEAREST,
			  GL_LINEAR_MIPMAP_LINEAR)
	mipmap = minfilter in mipmap_filters

	ti = glGenTextures(1)

	if width == 1 or height == 1:
		glBindTexture(GL_TEXTURE_1D,ti)
		glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,wrap)
		glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,magfilter)
		glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,minfilter)

		texture_image_1d(image,mipmap)

		dim = GL_TEXTURE_1D

	else:
		glBindTexture(GL_TEXTURE_2D,ti)
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,wrap)
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,wrap)
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,magfilter)
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,minfilter)

		texture_image_2d(image,mipmap)

		dim = GL_TEXTURE_2D

	return ti, dim, (width,height)


class Texture(object):	

	def real(self):
		return True

	def enable(self):
		glEnable(self.dim)

	def disable(self):
		glDisable(self.dim)

	def bind(self):
		glBindTexture(self.dim, self.index)

	def enable_gen_s(self,coeff):
		glEnable(GL_TEXTURE_GEN_S)
		glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR)
		glTexGenfv(GL_S,GL_OBJECT_PLANE,coeff)

	def enable_gen_t(self,coeff):
		if self.dim == GL_TEXTURE_2D:
			glEnable(GL_TEXTURE_GEN_T)
			glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR)
			glTexGenfv(GL_T,GL_OBJECT_PLANE,coeff)

	def disable_gen_s(self):
		glDisable(GL_TEXTURE_GEN_S)

	def disable_gen_t(self):
		if self.dim == GL_TEXTURE_2D:
			glDisable(GL_TEXTURE_GEN_T)
		

_texture_cache = {}


class FilesystemTexture(Texture):
	def __init__(self, filename, wrap=GL_REPEAT,
		     magfilter=GL_LINEAR,minfilter=GL_LINEAR,proxy=False):
		self.filename = filename
		self.hashvalue = (filename,wrap,magfilter,minfilter)

		if proxy:
			return
		
		if _texture_cache.has_key(self.hashvalue):
			tp = _texture_cache[self.hashvalue]
		else:
			image = Image.open(filename)
			tp = create_texture_from_image(
				image,wrap,magfilter,minfilter)
			_texture_cache[self.hashvalue] = tp

		self.index, self.dim, self.size = tp

	def __getstate__(self):
		return self.hashvalue

	def __setstate__(self,hashvalue):
		self.__init__(*hashvalue)

	def basename(self):
		return os.path.basename(self.filename)

	def imagedata(self):
		flo = open(self.filename,"rb")
		try: return flo.read()
		finally: flo.close()


class ZipfileTexture(Texture):
	def __init__(self, zfo, zipname, arcname, wrap=GL_REPEAT,
		     magfilter=GL_LINEAR,minfilter=GL_LINEAR,proxy=False):
		self.zipname = zipname
		self.arcname = arcname
		self.hashvalue = (zipname,arcname,wrap,magfilter,minfilter)

		if proxy:
			return
		
		if _texture_cache.has_key(self.hashvalue):
			tp = _texture_cache[self.hashvalue]
		else:
			image = Image.open(StringIO(zfo.read(arcname)))
			tp = create_texture_from_image(
				image,wrap,magfilter,minfilter)
			_texture_cache[self.hashvalue] = tp

		self.index, self.dim, self.size = tp

	def __getstate__(self):
		return self.hashvalue

	def __setstate__(self, hashvalue):
		zfo = ZipFile(hashvalue[0],"r")
		try: self.__init__(zfo,*hashvalue)
		finally: zfo.close()

	def basename(self):
		return os.path.basename(self.arcname)

	def imagedata(self):
		zfo = ZipFile(self.zipname,"r")
		try: return zfo.read(self.arcname)
		finally: zfo.close()


class NoTexture(object):
	def __init__(self):
		self.dim = 0
	def real(self):
		return False
	def enable(self):
		pass
	def disable(self):
		pass
	def bind(self):
		pass
	def enable_gen_s(self,coeff):
		pass
	def enable_gen_t(self,coeff):
		pass
	def disable_gen_s(self):
		pass
	def disable_gen_t(self):
		pass
	def basename(self):
		return "None"


