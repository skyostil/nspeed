# dom3ds.py

"""Interpret 3ds files."""

from struct import pack, unpack, calcsize
import operator
import Numeric
import os


# File format exception

class File3dsFormatError(StandardError):
	pass


# Icky, yucky global flags.  The DOM-like design I used made passing
# parameters unwieldy.  A functional form wraps each flag; this can be
# forcibly overridden to allow (for example) thread-safe behavior.

The_Dump_Array_Data_Flag = True

def i_may_dump_array_data():
	return The_Dump_Array_Data_Flag


# Quasi-file-like object, representing a certain range of a string.
# It has a "stream pointer," and some specialized functions.

class FileLikeBuffer(object):
	def __init__(self,s,start=0,end=None):
		self.buf = s
		self.start = start
		if end is None:
			self.end = len(s)
		else:
			if end > len(s):
				raise IndexError("End of fbuf object set "
						 "past end of buffer")
			self.end = end
		assert 0 <= start <= self.end, (start,self.end)
		self.index = start
	def advance(self,n):
		self.index += n
		if self.index > self.end:
			raise IndexError("End of fbuf reached")
	def read(self,length):
		r = self.buf[self.index:self.index+length]
		self.advance(length)
		return r
	def read_fbuf(self,length):
		r = FileLikeBuffer(self.buf,self.index,self.index+length)
		self.advance(length)
		return r
	def read_to_nul(self):
		try:
			i = self.buf.index('\0',self.index,self.end)
		except IndexError:
			raise IndexError("End of fbuf reached in string")
		r = self.buf[self.index:i]
		self.index = i+1
		return r
	def read_rest(self):
		r = self.buf[self.index:self.end]
		self.index = self.end
		return r
	def room_for_chunks(self):
		return self.index+6 <= self.end


# These changes a set of C-like type declarations into a tuple of
# tuples or a dict.  Basically, "declarations" are comma-separated;
# a "declaration" is a "type" and a "variable" separated by
# whitespace.

def decls_to_list(decls):
	decllist = map(str.strip, decls.split(','))
	decllist = [ tuple(x.split()) for x in decllist ]
	return decllist

def decls_to_vars(decls):
	if not decls: return ()
	vars = map(str.strip, decls.split(','))
	vars = [ x.split()[1] for x in vars ]
	return vars


# Metaclass of chunk types.  This takes some static data defines how
# to read in the chunks, and preprocesses it.  It also creates a
# dictionary of chunk classes keyed by tag.

class ChunkMetaclass(type):

	chunk_taghash = {}
	chunk_labelhash = {}

	def __new__(metatype,name,bases,clsdict):
		vars = []
		vars.extend(decls_to_vars(clsdict.get('struct','')))
		vars.extend(decls_to_vars(clsdict.get('single','')))
		vars.extend(decls_to_vars(clsdict.get('multiple','')))
		if clsdict.get('freechunks',False):
			vars.append('subchunks')
		clsdict.setdefault('__slots__',[]).extend(vars)

		return type.__new__(metatype,name,bases,clsdict)

	def __init__(self,name,bases,clsdict):
		if hasattr(self,'struct'):
			self.struct_fields = decls_to_list(self.struct)
		else:
			self.struct_fields = []

		self.single_types = {}
		self.single_order = []
		if hasattr(self,'single'):
			attrs = {}
			for typ,attr in decls_to_list(self.single):
				self.single_types[typ] = attr
				if not attrs.has_key(attr):
					attrs[attr] = 1
					self.single_order.append(attr)

		self.multiple_types = {}
		self.multiple_order = []
		if hasattr(self,'multiple'):
			attrs = {}
			for typ,attr in decls_to_list(self.multiple):
				self.multiple_types[typ] = attr
				if not attrs.has_key(attr):
					attrs[attr] = 1
					self.multiple_order.append(attr)

		if hasattr(self,'tag'):
			self.chunk_taghash[self.tag] = self
			self.chunk_labelhash[name] = self
			self.label = name

		type.__init__(self,name,bases,clsdict)


# Base class of chunks.

class ChunkBase(object):
	__metaclass__ = ChunkMetaclass

	# Defaults for some parameters

	freechunks = True
	swallow = False

	# Check if a "field" is legal

	def isfield(name):
		return (self.struct_types.has_key(name)
			or self.single_types.has_key(name)
			or self.multiple_types.has_key(name)
			or (name == 'subchunks' and self.freetype))

	# Define constructor to accept keyword arguments, as a convenience
	# Init chunk arguments to nothing

	def __init__(self,**kwargs):
		if self.freechunks:
			self.subchunks = []
		for v in self.single_order:
			setattr(self,v,None)
		for v in self.multiple_order:
			setattr(self,v,[])
		for attr,value in kwargs.items():
			setattr(self,attr,value)

	# -------- SECTION 1: INITIALIZATION FROM CHUNK DATA --------

	# These get a specific object from the fbuf

	def get_byte(self,fbuf):
		return ord(fbuf.read(1))

	def get_short(self,fbuf):
	        return unpack("<H",fbuf.read(2))[0]

	def get_long(self,fbuf):
		return unpack("<L",fbuf.read(4))[0]

	def get_float(self,fbuf):
		return unpack("<f",fbuf.read(4))[0]

	def get_string11(self,fbuf):
		s = fbuf.read(11)
		if '\0' in s:
			s = s[:s.index('\0')]
		return s

	def get_string(self,fbuf):
		return fbuf.read_to_nul()

	def get_chunk(self,fbuf):
		tag,length = unpack("<HL",fbuf.read(6))
		taghash = type(self).chunk_taghash
		if taghash.has_key(tag):
			ch = taghash[tag]()
		else:
			ch = UnknownChunk(tag)
		ch.read(fbuf.read_fbuf(length-6))
		return ch

	# These read sections of the chunk

	def read_struct(self,fbuf):
		for typ,attr in self.struct_fields:
			setattr(self,attr,getattr(self,"get_"+typ)(fbuf))

	def read_array(self,fbuf):
		# Expect subclass to override this
		pass

	def read_chunks(self,fbuf):
		if self.swallow:
			return

		while fbuf.room_for_chunks():
			ch = self.get_chunk(fbuf)

			if self.single_types.has_key(ch.label):
				attr = self.single_types[ch.label]
				if getattr(self,attr) is not None:
					raise File3dsFormatError(
						"Chunk %s appeared twice"
						% ch.label)
				setattr(self,attr,ch)
			elif self.multiple_types.has_key(ch.label):
				attr = self.multiple_types[ch.label]
				getattr(self,attr).append(ch)
			elif ch.label == 'DUMMY':
				pass
			elif self.freechunks:
				self.subchunks.append(ch)
			else:
				raise File3dsFormatError(
					"Illegal context for chunk %s"
					% ch.label)

	# Initialize self by scanning a chunk

	def read(self,fbuf):
		self.read_struct(fbuf)
		self.read_array(fbuf)
		self.read_chunks(fbuf)


	# -------- SECTION 2: DUMPING TO OUTPUT --------

	# Dump individual attributes

	def out_attr(self,attr,flo,indent):
		flo.write("%s%s = %r\n" % (indent,attr,getattr(self,attr)))

	def out_chunk(self,attr,flo,indent):
		g = getattr(self,attr)
		if g is not None:
			flo.write("%s%s = " % (indent,attr))
			g.dump(flo,indent)

	def out_chunk_index(self,attr,i,flo,indent):
		g = getattr(self,attr)[i]
		if g is not None:
			flo.write("%s%s[%d] = " % (indent,attr,i))
			g.dump(flo,indent)


	# Dump sections of the file

	def dump_header(self,flo):
		flo.write("%s (0x%04X)\n"
			% (self.label,self.tag))

	def dump_struct(self,flo,indent):
		for typ,attr in self.struct_fields:
			self.out_attr(attr,flo,indent)

	def dump_array(self,flo,indent):
		# Expect subclass to override this
		pass

	def dump_chunks(self,flo,indent):
		if self.swallow:
			return
		for attr in self.single_order:
			self.out_chunk(attr,flo,indent)
		for attr in self.multiple_order:
			for i in xrange(len(getattr(self,attr))):
				self.out_chunk_index(attr,i,flo,indent)
		if self.freechunks:
			attr = "subchunks"
			for i in xrange(len(self.subchunks)):
				self.out_chunk_index(attr,i,flo,indent)

	# Dump self to stream flo at a certain indentation

	def dump(self,flo,indent):
		indent += '    '
		self.dump_header(flo)
		self.dump_struct(flo,indent)
		self.dump_array(flo,indent)
		self.dump_chunks(flo,indent)


	# -------- SECTION #3: WRITING TO OUTPUT --------

	# These set a specific object from the fbuf

	def set_byte(self,value):
		return chr(value)

	def set_short(self,value):
	        return pack("<H",value)

	def set_long(self,value):
		return pack("<L",value)

	def set_float(self,value):
		return pack("<f",value)

	def set_string11(self,value):
		s = value[:10]
		s += "\0"*(11-len(s))
		return s

	def set_string(self,value):
		return value + "\0"

	def set_chunk(self,value):		
		s = value.write()
		length = len(s) + 6
		return pack("<HL",value.tag,length) + s

	# These write sections of the chunk

	def write_struct(self):
		s = []
		for typ,attr in self.struct_fields:
			s.append(getattr(self,"set_"+typ)(getattr(self,attr)))
		return ''.join(s)

	def write_array(self):
		# Expect subclass to override this
		return ''

	def write_chunks(self):
		if self.swallow:
			return ''
		s = []
		for attr in self.single_order:
			v = getattr(self,attr)
			if v is not None:
				s.append(self.set_chunk(v))
		for attr in self.multiple_order:
			for v in getattr(self,attr):
				s.append(self.set_chunk(v))
		if self.freechunks:
			for v in self.subchunks:
				s.append(self.set_chunk(v))
		return ''.join(s)

	# Write out self to chunk

	def write(self):
		s = []
		s.append(self.write_struct())
		s.append(self.write_array())
		s.append(self.write_chunks())
		return ''.join(s)


	# -------- SECTION #4: OUTPUT DOCUMENTATION --------

	def document_html(cls,flo,done):
		if done.has_key(cls.label):
			return
		flo.write('<dt>\n<a name="%s"></a><b>'
			  '<tt>%s (0x%04X)</tt></b></dt>\n'
			  % (cls.label,cls.label,cls.tag))
		flo.write('<dd>\n')

		i = 0
		
		for typ,attr in cls.struct_fields:
			flo.write('<tt>%s %s</tt><br>\n' % (typ,attr))
			i += 1

		tagorder = []

		d = {}
		for typ,attr in cls.single_types.items():
			if not d.has_key(attr):
				d[attr] = []
			d[attr].append(typ)
		for attr in cls.single_order:
			typs = d[attr]
			for typ in typs:
				tagorder.append(typ)
				flo.write('<tt><a href="#%s">%s</a> '
					  '%s</tt><br>\n' % (typ,typ,attr))
				i += 1

		d = {}
		for typ,attr in cls.multiple_types.items():
			if not d.has_key(attr):
				d[attr] = []
			d[attr].append(typ)
		for attr in cls.multiple_order:
			typs = d[attr]
			for typ in typs:
				tagorder.append(typ)
				flo.write('<tt><a href="#%s">%s[]</a> '
					  '%s</tt><br>\n' % (typ,typ,attr))
				i += 1

		if not i:
			flo.write('No fields<br>\n')

		flo.write('&nbsp;</dd>\n')
		done[cls.label] = 1

		for typ in tagorder:
			type(cls).chunk_labelhash[typ].document_html(flo,done)

	document_html = classmethod(document_html)


# Chunks we don't know anything about, maybe not even the tag

class UndefinedChunk(ChunkBase):
	def read(self,fbuf):
		pass
#		self.data = fbuf.read_rest()
	def dump(self,flo,indent):
		pass
#		self.dump_header(flo)
	def write(self):
		pass
#		return self.data


class UnknownChunk(UndefinedChunk):
	label = "UNKNOWN"
	def __init__(self,tag):
		self.tag = tag


class ErrorChunk(UndefinedChunk):
	label = "ERROR"
	def __init__(self):
		self.tag = 0xEEEE
	def write(self):
		raise TypeError("Attempt to write an Error Chunk")


# Chunks with common sets of attributes

class OneColorChunk(ChunkBase):
	single = ("COLOR_F color, COLOR_24 color,"
		  "LIN_COLOR_F lincolor, LIN_COLOR_24 lincolor")

class OnePercentageChunk(ChunkBase):
	single = ("INT_PERCENTAGE pct, FLOAT_PERCENTAGE pct")

class OneShortValueChunk(ChunkBase):
	struct = "short value"

class OneFloatValueChunk(ChunkBase):
	struct = "float value"

class Color24Chunk(ChunkBase):
	struct = "byte red, byte green, byte blue"

class TextureChunk(ChunkBase):
	single = ("INT_PERCENTAGE pct,"
		  "MAT_MAPNAME filename,"
		  "MAT_MAP_TILING tiling,"
		  "MAT_MAP_TEXBLUR blur,"
		  "MAT_MAP_USCALE uscale,"
		  "MAT_MAP_VSCALE vscale,"
		  "MAT_MAP_UOFFSET uoffset,"
		  "MAT_MAP_VOFFSET voffset,"
		  "MAT_MAP_ANG angle,"
		  "MAT_MAP_COL1 color1,"
		  "MAT_MAP_COL2 color2,"
		  "MAT_MAP_RCOL rtint,"
		  "MAT_MAP_GCOL gtint,"
		  "MAT_MAP_BCOL btint")

class TextureMaskChunk(ChunkBase):
	single = ("INT_PERCENTAGE pct,"
		  "MAT_MAPNAME filename,"
		  "MAT_MAP_TILING tiling,"
		  "MAT_MAP_TEXBLUR blur,"
		  "MAT_MAP_USCALE uscale,"
		  "MAT_MAP_VSCALE vscale,"
		  "MAT_MAP_UOFFSET uoffset,"
		  "MAT_MAP_VOFFSET voffset,"
		  "MAT_MAP_ANG angle")

class ArrayChunk(ChunkBase):
	__slots__ = ['array']
	def dump_array(self,flo,indent):
		flo.write("%s%s = <array data %s>\n"
			  % (indent,"array",repr(self.array.shape)))

class MatrixChunk(ArrayChunk):
	def read_array(self,fbuf):
		size = 48
		a = Numeric.fromstring(fbuf.read(size),Numeric.Float32)
		a = Numeric.reshape(a,(4,3))
		m = Numeric.zeros((4,4),Numeric.Float32)
		m[:,0:3] = a
		m[3,3] = 1.0
		self.array = Numeric.array(Numeric.transpose(m))
	def dump_array(self,flo,indent):
		ArrayChunk.dump_array(self,flo,indent)
		for i in xrange(4):
			flo.write("%s    %12.4g%12.4g%12.4g%12.4g\n"
				  % (indent,self.array[i,0],self.array[i,1],
				     self.array[i,2],self.array[i,3]))
	def write_array(self):
		a = self.array[0:3,:]
		a = Numeric.array(Numeric.transpose(a)).astype(Numeric.Float32)
		return a.tostring()


#
# Magic and Version Chunks
#

class M3DMAGIC(ChunkBase):
	tag = 0x4D4D
	single = "M3D_VERSION version, MDATA mdata, KFDATA kfdata"

class M3D_VERSION(ChunkBase):
	tag = 0x0002
	struct = "long number"

class MDATA(ChunkBase):
	tag = 0x3D3D
	single = ("MESH_VERSION version,"
		  "MASTER_SCALE scale,"
		  "VIEWPORT_LAYOUT layout,"
		  "LO_SHADOW_BIAS lo_shadow_bias,"
		  "HI_SHADOW_BIAS hi_shadow_bias,"
		  "SHADOW_MAP_SIZE shadow_map_size,"
		  "SHADOW_SAMPLES shadow_samples,"
		  "SHADOW_RANGE shadow_range,"
		  "SHADOW_FILTER shadow_filter,"
		  #"RAY_BIAS ray_bias,"
		  "O_CONSTS o_consts,"
		  "AMBIENT_LIGHT ambient_light,"
		  "SOLID_BGND solid_background,"
		  "BIT_MAP bitmap,"
		  "V_GRADIENT v_gradient,"
		  "USE_BIT_MAP use_bitmap,"
		  "USE_SOLID_BGND use_solid_background,"
		  "USE_V_GRADIENT use_v_gradient,"
		  "FOG fog,"
		  "LAYER_FOG layer_fog,"
		  "DISTANCE_CUE distance_cue,"
		  "USE_FOG use_fog,"
		  "USE_LAYER_FOG use_layer_fog,"
		  "USE_DISTANCE_CUE use_distance_cue,"
		  "DEFAULT_VIEW default_view,"
		  "MARKER marker")
	multiple = "MAT_ENTRY materials, NAMED_OBJECT objects"

class MESH_VERSION(ChunkBase):
	tag = 0x3D3E
	struct = "long number"

class SMAGIC(UndefinedChunk): tag = 0x2D2D
class LMAGIC(UndefinedChunk): tag = 0x2D3D
class MLIBMAGIC(UndefinedChunk): tag = 0x2DAA
class PRJMAGIC(UndefinedChunk): tag = 0x3D2C
class MATMAGIC(UndefinedChunk): tag = 0x3DFF

#
# Miscellaneous, Global data
#

class MARKER(ChunkBase):
	tag = 0x0001
	struct = "long sum"

class COLOR_F(ChunkBase):
	tag = 0x0010
	struct = "float red, float green, float blue"

class COLOR_24(Color24Chunk):
	tag = 0x0011

class LIN_COLOR_24(Color24Chunk):
	tag = 0x0012

class LIN_COLOR_F(ChunkBase):
	tag = 0x0013
	struct = "float red, float green, float blue"

class INT_PERCENTAGE(OneShortValueChunk):
	tag = 0x0030

class FLOAT_PERCENTAGE(OneFloatValueChunk):
	tag = 0x0031

class MASTER_SCALE(OneFloatValueChunk):
	tag = 0x0100


#
# Coloring, Lighting, Atmosphers Stuff
#

class BIT_MAP(ChunkBase):
	tag = 0x1100
	struct = 'string filename'
	swallow = True

class USE_BIT_MAP(ChunkBase):
	tag = 0x1101

class SOLID_BGND(OneColorChunk):
	tag = 0x1200

class USE_SOLID_BGND(ChunkBase):
	tag = 0x1201

class V_GRADIENT(ChunkBase):
	tag = 0x1300
	struct = "float midpoint"
	multiple = ("COLOR_F color, LIN_COLOR_F lincolor,"
		    "COLOR_24 color, LIN_COLOR_24 lincolor")

class USE_V_GRADIENT(ChunkBase):
	tag = 0x1301

class LO_SHADOW_BIAS(OneFloatValueChunk):
	tag = 0x1400

class HI_SHADOW_BIAS(OneFloatValueChunk):
	tag = 0x1410

class SHADOW_MAP_SIZE(OneShortValueChunk):
	tag = 0x1420

class SHADOW_SAMPLES(OneShortValueChunk):
	tag = 0x1430

class SHADOW_RANGE(OneShortValueChunk):
	tag = 0x1440

class SHADOW_FILTER(OneFloatValueChunk):
	tag = 0x1450

class RAW_BIAS(OneFloatValueChunk):
	tag = 0x1460

class O_CONSTS(ChunkBase):
	tag = 0x1500
	struct = "float plane_x, float plane_y, float plane_z"

class AMBIENT_LIGHT(OneColorChunk):
	tag = 0x2100


class FOG(ChunkBase):
	tag = 0x2200
	struct = ("float near_plane, float near_density,"
		  "float far_plane, float far_density")
	single = "COLOR_F color, FOG_BGND fog_background"

class USE_FOG(ChunkBase):
	tag = 0x2201

class FOG_BGND(ChunkBase):
	tag = 0x2210

class DISTANCE_CUE(ChunkBase):
	tag = 0x2300
	struct = ("float near_plane, float near_density,"
		  "float far_plane, float far_density")
	single = "DCUE_BGND dcue_background"

class USE_DISTANCE_CUE(ChunkBase):
	tag = 0x2301

class LAYER_FOG(ChunkBase):
	tag = 0x2302
	struct = ("float fog_z_from, float fog_z_to,"
		  "float fog_density, long fog_type")
	single = "COLOR_F color"

class USE_LAYER_FOG(ChunkBase):
	tag = 0x2303

class DCUE_BGND(ChunkBase):
	tag = 0x2310

#
# View Data
#

class DEFAULT_VIEW(ChunkBase):
	tag = 0x3000
	single = ('VIEW_TOP view, VIEW_BOTTOM view, VIEW_LEFT view,'
		  'VIEW_RIGHT view, VIEW_FRONT view, VIEW_BACK view,'
		  'VIEW_USER view, VIEW_CAMERA camera')

class ViewChunk(ChunkBase):
	struct = ("float target_x, float target_y,"
		  "float target_z, float view_width")

class VIEW_TOP(ViewChunk):
	tag = 0x3010

class VIEW_BOTTOM(ViewChunk):
	tag = 0x3020

class VIEW_LEFT(ViewChunk):
	tag = 0x3030

class VIEW_RIGHT(ViewChunk):
	tag = 0x3040

class VIEW_FRONT(ViewChunk):
	tag = 0x3050

class VIEW_BACK(ViewChunk):
	tag = 0x3060

class VIEW_USER(ChunkBase):
	tag = 0x3070
	struct = ("float target_x, float target_y,"
		  "float target_z, float view_width,"
		  "float horiz_angle, float vert_angle,"
		  "float back_angle")

class VIEW_CAMERA(ChunkBase):
	tag = 0x3080
	struct = "string11 name"

class VIEW_WINDOW(UndefinedChunk):
	tag = 0x3090


#
# Objects
#

class NAMED_OBJECT(ChunkBase):
	tag = 0x4000
	struct = "string name"
	single = ("N_TRI_OBJECT obj,"
		  "N_DIRECT_LIGHT obj,"
		  "N_CAMERA obj,"
		  "OBJ_HIDDEN hidden,"
		  "OBJ_VIS_LOFTER vis_lofter,"
		  "OBJ_DOESNT_CAST doesnt_cast,"
		  "OBJ_MATTE matte,"
		  "OBJ_DONT_RCVSHADOW dont_rcvshadow,"
		  "OBJ_FAST fast,"
		  "OBJ_PROCEDURAL procedural,"
		  "OBJ_FROZEN frozen")

class OBJ_HIDDEN(ChunkBase):
	tag = 0x4010 

class OBJ_VIS_LOFTER(ChunkBase):
	tag = 0x4011 

class OBJ_DOESNT_CAST(ChunkBase):
	tag = 0x4012 

class OBJ_MATTE(ChunkBase):
	tag = 0x4013
	
class OBJ_FAST(ChunkBase):
	tag = 0x4014
	
class OBJ_PROCEDURAL(ChunkBase):
	tag = 0x4015
	
class OBJ_FROZEN(ChunkBase):
	tag = 0x4016 

class OBJ_DONT_RCVSHADOW(ChunkBase):
	tag = 0x4017 

class N_TRI_OBJECT(ChunkBase):
	tag = 0x4100
	single = ("POINT_ARRAY points,"
		  "POINT_FLAG_ARRAY flags,"
		  "FACE_ARRAY faces,"
		  "TEX_VERTS texverts,"
		  "MESH_MATRIX matrix,"
		  "MESH_COLOR color,"
		  "MESH_TEXTURE_INFO texinfo,"
		  "PROC_NAME proc_name,"
		  "PROC_DATA proc_data")
	multiple = "MSH_MAT_GROUP matlist"

class POINT_ARRAY(ArrayChunk):
	tag = 0x4110
	struct = "short npoints"
	def read_array(self,fbuf):
		size = 12*self.npoints
		a = Numeric.fromstring(fbuf.read(size),Numeric.Float32)
		a = Numeric.reshape(a,(self.npoints,3))
		self.array = Numeric.array(a)
	def dump_array(self,flo,indent):
		ArrayChunk.dump_array(self,flo,indent)
		if not i_may_dump_array_data():
			return
		for i in xrange(self.npoints):
			flo.write("%s    %12.4g%12.4g%12.4g\n"
				  % (indent,self.array[i,0],self.array[i,1],
				     self.array[i,2]))
	def write_array(self):
		s = Numeric.array(self.array).astype(Numeric.Float32)
		return s.tostring()
	swallow = True

class POINT_FLAG_ARRAY(ArrayChunk):
	tag = 0x4111
	struct = "short npoints"
	def read_array(self,fbuf):
		size = 2*self.npoints
		self.array = Numeric.fromstring(
			fbuf.read(size),Numeric.UnsignedInt16)
	def write_array(self):
		s = Numeric.array(self.array).astype(Numeric.UnsignedInt16)
		return s.tostring()
	swallow = True

class FACE_ARRAY(ArrayChunk):
	tag = 0x4120
	struct = "short nfaces"
	multiple = 'MSH_MAT_GROUP materials'
	single = 'SMOOTH_GROUP smoothing, MSH_BOXMAP box'
	def read_array(self,fbuf):
		size = 8*self.nfaces
		a = Numeric.fromstring(fbuf.read(size),Numeric.UnsignedInt16)
		a = Numeric.reshape(a,(self.nfaces,4))
		self.array = Numeric.array(a)
	def dump_array(self,flo,indent):
		ArrayChunk.dump_array(self,flo,indent)
		if not i_may_dump_array_data():
			return
		for i in xrange(self.nfaces):
			flo.write("%s    %10d%10d%10d%10d\n"
				  % (indent,self.array[i,0],self.array[i,1],
				     self.array[i,2],self.array[i,3]))
	def write_array(self):
		s = Numeric.array(self.array).astype(Numeric.UnsignedInt16)
		return s.tostring()

class MSH_MAT_GROUP(ArrayChunk):
	tag = 0x4130
	struct = "string name, short mfaces"
	def read_array(self,fbuf):
		size = 2*self.mfaces
		self.array = Numeric.fromstring(
			fbuf.read(size),Numeric.UnsignedInt16)
	def dump_array(self,flo,indent):
		ArrayChunk.dump_array(self,flo,indent)
		if not i_may_dump_array_data():
			return
		for i in xrange(self.mfaces):
			flo.write("%s    %10d\n" % (indent,self.array[i]))
	def write_array(self):
		s = Numeric.array(self.array).astype(Numeric.UnsignedInt16)
		return s.tostring()
	swallow = True

class TEX_VERTS(ArrayChunk):
	tag = 0x4140
	struct = "short npoints"
	def read_array(self,fbuf):
		size = 8*self.npoints
		a = Numeric.fromstring(fbuf.read(size),Numeric.Float32)
		a = Numeric.reshape(a,(self.npoints,2))
		self.array = Numeric.array(a)
	def dump_array(self,flo,indent):
		ArrayChunk.dump_array(self,flo,indent)
		if not i_may_dump_array_data():
			return
		for i in xrange(self.npoints):
			flo.write("%s    %12.4g%12.4g\n"
				  % (indent,self.array[i,0],self.array[i,1]))
	def write_array(self):
		s = Numeric.array(self.array).astype(Numeric.Float32)
		return s.tostring()
	swallow = True

class SMOOTH_GROUP(ArrayChunk):
	tag = 0x4150
	def read_array(self,fbuf):
		self.array = Numeric.fromstring(
			fbuf.read_rest(),Numeric.UnsignedInt32)
	def dump_array(self,flo,indent):
		ArrayChunk.dump_array(self,flo,indent)
		if not i_may_dump_array_data():
			return
		for i in xrange(len(self.array)):
			flo.write("%s    %10d\n" % (indent,self.array[i]))
	def write_array(self):
		s = Numeric.array(self.array).astype(Numeric.UnsignedInt32)
		return s.tostring()
	swallow = True

class MESH_MATRIX(MatrixChunk):
	tag = 0x4160

class MESH_COLOR(ChunkBase):
	tag = 0x4165
	struct = "byte color_index"

class MESH_TEXTURE_INFO(MatrixChunk):
	tag = 0x4170
	struct = ("float x_tiling, float y_tiling, float icon_x,"
		  "float icon_y, float icon_z, float scaling")
	def read(self,fbuf):
		self.read_struct(fbuf)
		self.read_array(fbuf)
		self.icon_width = self.get_float(fbuf)
		self.icon_height = self.get_float(fbuf)
		self.cyl_height = self.get_float(fbuf)
		self.read_chunks(fbuf)
	def dump(self,flo,indent):
		indent += '    '
		self.dump_header(flo)
		self.dump_struct(flo,indent)
		self.dump_array(flo,indent)
		self.out_attr('icon_width',flo,indent)
		self.out_attr('icon_height',flo,indent)
		self.out_attr('cyl_height',flo,indent)
		self.dump_chunks(flo,indent)
	def write(self):
		s = []
		s.append(self.write_struct())
		s.append(self.write_array())
		s.append(self.out_float(self.icon_width))
		s.append(self.out_float(self.icon_height))
		s.append(self.out_float(self.cyl_height))
		s.append(self.write_chunks())
		return ''.join(s)

class PROC_NAME(ChunkBase):
	tag = 0x4181
	struct = "string value"
	swallow = True

class PROC_DATA(UndefinedChunk):
	tag = 0x4181
	struct = "string value"
	swallow = True

class MSH_BOXMAP(ChunkBase):
	tag = 0x4190
	struct = ("string front, string back, string left,"
		  "string right, string top, string bottom")
	swallow = True


class N_DIRECT_LIGHT(ChunkBase):
	tag = 0x4600
	struct = "float light_x, float light_y, float light_z"
	single = ("COLOR_F color, COLOR_24 color,"
		  "DL_OFF switch,"
		  "DL_OUTER_RANGE outer_range,"
		  "DL_INNER_RANGE inner_range,"
		  "DL_MULTIPLIER multiplier,"
		  "DL_SPOTLIGHT spotlight,"
		  "DL_ATTENUATE attenuate")
	multiple = "DL_EXCLUDE excludes"

class DL_SPOTLIGHT(ChunkBase):
	tag = 0x4610
	struct = ("float spot_x, float spot_y, float spot_z,"
		  "float hotspot_angle, float falloff_angle")
	single = ("DL_SPOT_ROLL roll_angle,"
		  "DL_SHADOWED shadowed,"
		  "DL_LOCAL_SHADOW2 local_shadow,"
		  "DL_SEE_CONE see_cone,"
		  "DL_SPOT_RECTANGULAR rectangular,"
		  "DL_SPOT_ASPECT aspect,"
		  "DL_SPOT_PROJECTOR projector,"
		  "DL_SPOT_OVERSHOOT overshoot,"
		  "DL_RAY_BIAS bias,"
		  "DL_RAYSHADE rayshade")

class DL_OFF(ChunkBase):
	tag = 0x4620

class DL_ATTENUATE(ChunkBase):
	tag = 0x4625

class DL_RAYSHADE(ChunkBase):
	tag = 0x4627

class DL_SHADOWED(ChunkBase):
	tag = 0x4630

class DL_LOCAL_SHADOW2(ChunkBase):
	tag = 0x4630
	struct = "float low_bias, float filter, short mapsize"

class DL_SEE_CONE(ChunkBase):
	tag = 0x4650

class DL_SPOT_RECTANGULAR(ChunkBase):
	tag = 0x4651

class DL_SPOT_OVERSHOOT(ChunkBase):
	tag = 0x4652

class DL_SPOT_PROJECTOR(ChunkBase):
	tag = 0x4653
	struct = "string filename"
	swallow = True

class DL_EXCLUDE(ChunkBase):
	tag = 0x4654
	struct = "string value"
	swallow = True

class DL_SPOT_ROLL(OneFloatValueChunk):
	tag = 0x4656

class DL_SPOT_ASPECT(OneFloatValueChunk):
	tag = 0x4657

class DL_RAY_BIAS(OneFloatValueChunk):
	tag = 0x4658

class DL_INNER_RANGE(OneFloatValueChunk):
	tag = 0x4659

class DL_OUTER_RANGE(OneFloatValueChunk):
	tag = 0x465A

class DL_MULTIPLIER(OneFloatValueChunk):
	tag = 0x465B


class N_CAMERA(ChunkBase):
	tag = 0x4700
	struct = ("float camera_x, float camera_y, float camera_z,"
		  "float target_x, float target_y, float target_z,"
		  "float bank_angle, float focal_length")
	single = "CAM_SEE_CONE see_cone, CAM_RANGES ranges"

class CAM_SEE_CONE(ChunkBase):
	tag = 0x4710

class CAM_RANGES(ChunkBase):
	tag = 0x4720
	struct = "float near, float far"

#
# Viewport Data
#

class VIEWPORT_LAYOUT(ChunkBase):
	tag = 0x7001
	struct = ('short form, short top, short ready,'
		  'short wstate, short swapws, short swapport,'
		  'short swapcur')
	single = 'VIEWPORT_SIZE size'
	multiple = 'VIEWPORT_DATA data, VIEWPORT_DATA_3 data'

class ViewportDataChunk(ChunkBase):
	struct = ('short flags, short axis_lockout, short win_x,'
		  'short win_y, short win_w, short win_h,'
		  'short win_view, float zoom, float worldcenter_x,'
		  'float worldcenter_y, float worldcenter_z,'
		  'float horiz_ang, float vert_ang,'
		  'string11 cameraname')

class VIEWPORT_DATA(ViewportDataChunk):
	tag = 0x7011

class VIEWPORT_DATA_3(ViewportDataChunk):
	tag = 0x7012

class VIEWPORT_SIZE(ChunkBase):
	tag = 0x7020
	struct = 'short x, short y, short w, short h'

class NETWORK_VIEW(UndefinedChunk): tag = 0x7030

#
# Material Data
#

class MAT_NAME(ChunkBase):
	tag = 0xA000
        struct = 'string value'
	swallow = True

class MAT_AMBIENT(OneColorChunk):
	tag = 0xA010

class MAT_DIFFUSE(OneColorChunk):
	tag = 0xA020

class MAT_SPECULAR(OneColorChunk):
	tag = 0xA030

class MAT_SHININESS(OnePercentageChunk):
	tag = 0xA040

class MAT_SHIN2PCT(OnePercentageChunk):
	tag = 0xA041

class MAT_SHIN3PCT(OnePercentageChunk):
	tag = 0xA042

class MAT_TRANSPARENCY(OnePercentageChunk):
	tag = 0xA050

class MAT_XPFALL(OnePercentageChunk):
	tag = 0xA052

class MAT_REFBLUR(OnePercentageChunk):
	tag = 0xA053

class MAT_SELF_ILLUM(ChunkBase):
	tag = 0xA080

class MAT_TWO_SIDE(ChunkBase):
	tag = 0xA081

class MAT_DECAL(ChunkBase):
	tag = 0xA082

class MAT_ADDITIVE(ChunkBase):
	tag = 0xA083

class MAT_SELF_ILPCT(OnePercentageChunk):
	tag = 0xA084

class MAT_WIRE(ChunkBase):
	tag = 0xA085

class MAT_SUPERSMP(UndefinedChunk): tag = 0xA086

class MAT_WIRESIZE(OneFloatValueChunk):
	tag = 0xA087

class MAT_FACEMAP(ChunkBase):
	tag = 0xA088

class MAT_XPFALLIN(UndefinedChunk): tag = 0xA08A

class MAT_PHONGSOFT(ChunkBase):
	tag = 0xA08C

class MAT_WIREABS(ChunkBase):
	tag = 0xA08E

class MAT_SHADING(OneShortValueChunk):
	tag = 0xA100

class MAT_TEXMAP(TextureChunk):
	tag = 0xA200

class MAT_SPECMAP(TextureChunk):
	tag = 0xA204

class MAT_OPACMAP(TextureMaskChunk):
	tag = 0xA210

class MAT_REFLMAP(ChunkBase):
	tag = 0xA220
	single = "INT_PERCENTAGE pct, MAT_MAPNAME filename"

class MAT_BUMPMAP(TextureMaskChunk):
	tag = 0xA230

class MAT_USE_XPFALL(ChunkBase):
	tag = 0xA240

class MAT_USE_REFBLUR(ChunkBase):
	tag = 0xA250

class MAT_BUMP_PERCENT(ChunkBase):
	tag = 0xA252
	struct = "short value"

class MAT_MAPNAME(ChunkBase):
	tag = 0xA300
	struct = "string value"
	swallow = True

class MAT_ACUBIC(ChunkBase):
	tag = 0xA310
	struct = ("byte shade, byte antialias, short rflags,"
		  "long mapsize, long frame")

class MAT_TEX2MAP(TextureChunk):
	tag = 0xA33a 

class MAT_SHINMAP(TextureMaskChunk):
	tag = 0xA33c

class MAT_SELFIMAP(TextureChunk):
	tag = 0xA33d 

class MAT_TEXMASK(TextureMaskChunk):
	tag = 0xA33e

class MAT_TEXT2MASK(TextureMaskChunk):
	tag = 0xA340 

class MAT_OPACMASK(TextureMaskChunk):
	tag = 0xA342 

class MAT_BUMPMASK(TextureMaskChunk):
	tag = 0xA344 

class MAT_SHINMASK(TextureMaskChunk):
	tag = 0xA346

class MAT_SPECMASK(TextureMaskChunk):
	tag = 0xA348

class MAT_SELFIMASK(TextureChunk):
	tag = 0xA34a 

class MAT_REFLMASK(TextureChunk):
	tag = 0xA34c

class MAT_MAP_TILING(OneShortValueChunk):
	tag = 0xA351

class MAT_MAP_TEXBLUR(OneFloatValueChunk):
	tag = 0xA353

class MAT_MAP_USCALE(OneFloatValueChunk):
	tag = 0xA354

class MAT_MAP_VSCALE(OneFloatValueChunk):
	tag = 0xA356 

class MAT_MAP_UOFFSET(OneFloatValueChunk):
	tag = 0xA358 

class MAT_MAP_VOFFSET(OneFloatValueChunk):
	tag = 0xA35a 

class MAT_MAP_ANG(OneFloatValueChunk):
	tag = 0xA35c 

class MAT_MAP_COL1(Color24Chunk):
	tag = 0xA360 

class MAT_MAP_COL2(Color24Chunk):
	tag = 0xA362 

class MAT_MAP_RCOL(Color24Chunk):
	tag = 0xA364 

class MAT_MAP_GCOL(Color24Chunk):
	tag = 0xA366 

class MAT_MAP_BCOL(Color24Chunk):
	tag = 0xA368 

class MAT_ENTRY(ChunkBase):
	tag = 0xAFFF
	single = ("MAT_NAME name,"
		  "MAT_AMBIENT ambient,"
		  "MAT_DIFFUSE diffuse,"
		  "MAT_SPECULAR specular,"
		  "MAT_SHININESS shininess,"
		  "MAT_SHIN2PCT shin2pct,"
		  "MAT_TRANSPARENCY transparency,"
		  "MAT_XPFALL xpfall,"
		  "MAT_REFBLUR refblur,"
		  "MAT_SELF_ILLUM self_illum,"
		  "MAT_SHADING shading,"
		  "MAT_SELF_ILPCT self_ilpct,"
		  "MAT_USE_XPFALL use_xpfall,"
		  "MAT_USE_REFBLUR use_refblur,"
		  "MAT_TWO_SIDE two_side,"
		  "MAT_ADDITIVE additive,"
		  "MAT_WIRE wire,"
		  "MAT_FACEMAP facemap,"
		  "MAT_PHONGSOFT phongsoft,"
		  "MAT_WIRESIZE wiresize,"
		  "MAT_DECAL decal,"
		  "MAT_TEXMAP texmap,"
		  "MAT_SXP_TEXT_DATA sxp_text_data,"
		  "MAT_TEXMASK texmask,"
		  "MAT_SXP_TEXT_MASKDATA sxp_text_maskdata,"
		  "MAT_TEX2MAP tex2map,"
		  "MAT_SXP_TEXT2_DATA sxp_text2_data,"
		  "MAT_TEXT2MASK text2mask,"
		  "MAT_SXP_TEXT2_MASKDATA sxp_text2_maskdata,"
		  "MAT_OPACMAP opacmap,"
		  "MAT_SXP_OPAC_DATA sxp_opac_data,"
		  "MAT_OPACMASK opac_mask,"
		  "MAT_SXP_OPAC_MASKDATA sxp_opac_maskdata,"
		  "MAT_BUMPMAP bumpmap,"
		  "MAT_SXP_BUMP_DATA sxp_bump_data,"
		  "MAT_BUMPMASK bumpmask,"
		  "MAT_SXP_BUMP_MASKDATA sxp_bump_maskdata,"
		  "MAT_SPECMAP specmap,"
		  "MAT_SXP_SPEC_DATA sxp_spec_data,"
		  "MAT_SPECMASK specmask,"
		  "MAT_SXP_SPEC_MASKDATA sxp_spec_maskdata,"
		  "MAT_SHINMAP shinmap,"
		  "MAT_SXP_SHIN_DATA sxp_shin_data,"
		  "MAT_SHINMASK shinmask,"
		  "MAT_SXP_SHIN_MASKDATA sxp_shin_maskdata,"
		  "MAT_SELFIMAP selfimap,"
		  "MAT_SXP_SELFI_DATA sxp_selfi_data,"
		  "MAT_SELFIMASK selfimask,"
		  "MAT_SXP_SELFI_MASKDATA sxp_selfi_maskdata,"
		  "MAT_REFLMAP reflmap,"
		  "MAT_REFLMASK reflmask,"
		  "MAT_SXP_REFL_MASKDATA sxp_refl_maskdata,"
		  "MAT_ACUBIC acubic")

class MAT_SXP_TEXT_DATA(UndefinedChunk): tag = 0xA320
class MAT_SXP_TEXT2_DATA(UndefinedChunk): tag = 0xA321
class MAT_SXP_OPAC_DATA(UndefinedChunk): tag = 0xA322
class MAT_SXP_BUMP_DATA(UndefinedChunk): tag = 0xA324
class MAT_SXP_SPEC_DATA(UndefinedChunk): tag = 0xA325 
class MAT_SXP_SHIN_DATA(UndefinedChunk): tag = 0xA326 
class MAT_SXP_SELFI_DATA(UndefinedChunk): tag = 0xA328 
class MAT_SXP_TEXT_MASKDATA(UndefinedChunk): tag = 0xA32a 
class MAT_SXP_TEXT2_MASKDATA(UndefinedChunk): tag = 0xA32c 
class MAT_SXP_OPAC_MASKDATA(UndefinedChunk): tag = 0xA32e 
class MAT_SXP_BUMP_MASKDATA(UndefinedChunk): tag = 0xA330 
class MAT_SXP_SPEC_MASKDATA(UndefinedChunk): tag = 0xA332 
class MAT_SXP_SHIN_MASKDATA(UndefinedChunk): tag = 0xA334 
class MAT_SXP_SELFI_MASKDATA(UndefinedChunk): tag = 0xA336 
class MAT_SXP_REFL_MASKDATA(UndefinedChunk): tag = 0xA338 

#
# Keyframe Section
#

class KFDATA(UndefinedChunk): tag = 0xB000

#
# Misc
#

class DUMMY(ChunkBase): tag = 0xFFFF

#
# Obsolute Chunks
#

class VIEWPORT_LAYOUT_OLD(UndefinedChunk): tag = 0x7000
class VIEWPORT_DATA_OLD(UndefinedChunk): tag = 0x7010
class OLD_MAT_GROUP(UndefinedChunk): tag = 0x4131
class MAT_MAP_TILING_OLD(UndefinedChunk): tag = 0xA350 
class MAT_MAP_TEXBLUR_OLD(UndefinedChunk): tag = 0xA352 


#
# Functions to operate on chunks
#

def read_3ds_mem(membuf):
	tag,length = unpack("<HL",membuf[:6])
	if tag != 0x4D4D:
		raise File3dsFormatError("Not a 3D Studio file.")
	fbuf = FileLikeBuffer(membuf,6,length)
	dom = M3DMAGIC()
	dom.read(fbuf)
	return dom

def read_3ds_file(filename):
	flo = open(filename,'rb')
	try:	
		tag,length = unpack("<HL",flo.read(6))
		if tag != 0x4D4D:
			raise File3dsFormatError("Not a 3D Studio file.")
		fbuf = FileLikeBuffer(flo.read(length-6))
	finally:
		flo.close()
	dom = M3DMAGIC()
	dom.read(fbuf)
	return dom

def write_3ds_mem(dom):
	s = dom.write()
	length = len(s) + 6
	return pack("<HL",0x4D4D,length) + s

def write_3ds_file(filename,dom):
	if dom.tag != 0x4D4D:
		raise File3dsFormatError("Not a M3DMAGIC chunk.")
	flo = open(filename,'wb')
	try:
		s = dom.write()
		length = len(s) + 6
		flo.write(pack("<HL",0x4D4D,length))
		flo.write(s)
	finally:
		flo.close()

def dump_3ds_chunk(chunk,flo,indent=''):
	chunk.dump(flo,indent)

def dump_3ds_file(filename,flo):
	dom = read_3ds_file(filename)
	dump_3ds_chunk(dom,flo)

def remove_errant_chunks(chunk):
	for attr in chunk.single_order:
		c = getattr(chunk,attr)
		if isinstance(c,ErrorChunk):
			setattr(chunk,attr,None)
		elif c:
			remove_errant_chunks(c)
	mchunks = chunk.multiple_order[:]
	if chunk.freechunks: mchunks.append('subchunks')
	for attr in mchunks:
		cl = [ x for x in getattr(chunk,attr)
		       if not isinstance(x,ErrorChunk) ]
		setattr(chunk,attr,cl)
		for c in cl:
			remove_errant_chunks(c)


