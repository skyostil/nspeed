# loadmodel.py

from Dice3DS.GL import *
from zipfile import ZipFile
import os

import basicmodel
import loadtexture
import dom3ds


class RuntimeMaterial(basicmodel.BasicMaterial):

	def __init__(self,*args):
		super(RuntimeMaterial,self).__init__(*args)

	def set_material(self,has_tverts):
		if self.twosided:
			side = GL_FRONT_AND_BACK
		else:
			side = GL_FRONT
		glMaterialfv(side,GL_AMBIENT,self.ambient)
		glMaterialfv(side,GL_DIFFUSE,self.diffuse)
		glMaterialfv(side,GL_SPECULAR,self.specular)
		glMaterialf(side,GL_SHININESS,self.shininess*128.0)
		if self.texture and has_tverts:
			self.texture.enable()
			self.texture.bind()

	def unset_material(self,has_tverts):
		if self.texture and has_tverts:
			self.texture.disable()


class RuntimeMesh(basicmodel.BasicMesh):

	def __init__(self,*args):
		super(RuntimeMesh,self).__init__(*args)

	def render_normals(self):
		glPushAttrib(GL_LIGHTING_BIT)
		glDisable(GL_LIGHTING)
		glColor3f(1.0,1.0,0.0)
		glBegin(GL_LINES)
		for i in xrange(len(self.points)):
			glVertex3fv(self.points[i])
			glVertex3fv(self.points[i] + 0.1*self.norms[i])
		glEnd()
		glPopAttrib()

	def render_nomaterials(self):
		glMaterialfv(GL_FRONT,GL_AMBIENT,(0.2,0.2,0.2,1.0))
		glMaterialfv(GL_FRONT,GL_DIFFUSE,(0.8,0.8,0.8,1.0))
		glMaterialfv(GL_FRONT,GL_SPECULAR,(0.0,0.0,0.0,1.0))
		glMaterialf(GL_FRONT,GL_SHININESS,0.0)
		glBegin(GL_TRIANGLES)
		for i in xrange(len(self.points)):
			glNormal3fv(self.norms[i])
			glVertex3fv(self.points[i])
		glEnd()

	def render_materials(self):
		has_tverts = bool(self.tverts)
		for material,faces in self.matarrays:
			if (not material.texture or not has_tverts
			    or material.texture.dim == 0):
				def tcfunc(i):
					pass
			elif material.texture.dim == GL_TEXTURE_2D:
				def tcfunc(i):
					glTexCoord2fv(self.tverts[i])
			elif material.texture.dim == GL_TEXTURE_1D:
				def tcfunc(i):
					glTexCoord1f(self.tverts[i,0])
			else:
				assert False
			material.set_material(has_tverts)
			glBegin(GL_TRIANGLES)
			for f in faces:
				tcfunc(f*3)
				glNormal3fv(self.norms[f*3])
				glVertex3fv(self.points[f*3])
				tcfunc(f*3+1)
				glNormal3fv(self.norms[f*3+1])
				glVertex3fv(self.points[f*3+1])
				tcfunc(f*3+2)
				glNormal3fv(self.norms[f*3+2])
				glVertex3fv(self.points[f*3+2])
			glEnd()
			material.unset_material(has_tverts)

	def render(self):
		if self.matarrays:
			self.render_materials()
		else:
			self.render_nomaterials()


class RuntimeModel(basicmodel.BasicModel):
	meshclass = RuntimeMesh
	matclass = RuntimeMaterial

	def __init__(self,*args):
		super(RuntimeModel,self).__init__(*args)

	def create_dl(self):
		dl = glGenLists(1)
		if dl == 0:
			raise RuntimeError, "cannot allocate display list"
		glNewList(dl,GL_COMPILE)
		for m in self.meshes:
			m.render()
		glEndList()
		return dl


def load_model_from_filesystem(filename,texargs=()):

	dirname = os.path.dirname(filename)
	def load_texture(texfilename):
		return loadtexture.FilesystemTexture(
			os.path.join(dirname,texfilename),*texargs)
	
	dom = dom3ds.read_3ds_file(filename)
	return RuntimeModel(dom,load_texture)


def load_model_from_zipfile(zipname,arcname,texargs=()):

	def load_texture(texfilename):
		return loadtexture.ZipfileTexture(
			zfo,zipname,texfilename,*texargs)
	
	zfo = ZipFile(zipname,"r")
	try:
		dom = dom3ds.read_3ds_mem(zfo.read(arcname))
		return RuntimeModel(dom,load_texture)
	finally:
		zfo.close()


def create_model_dl_from_nearby_zipfile(modfile,arcname):
	model = load_model_from_zipfile(
		os.path.splitext(modfile)[0] + ".zip", arcname)	
	return model.create_dl()


