# This file was created automatically by SWIG.
import freetypec
class TT_UnitVectorPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,freetypec=freetypec):
        if self.thisown == 1 :
            freetypec.delete_TT_UnitVector(self)
    def __setattr__(self,name,value):
        if name == "x" :
            freetypec.TT_UnitVector_x_set(self,value)
            return
        if name == "y" :
            freetypec.TT_UnitVector_y_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "x" : 
            return freetypec.TT_UnitVector_x_get(self)
        if name == "y" : 
            return freetypec.TT_UnitVector_y_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_UnitVector instance at %s>" % (self.this,)
class TT_UnitVector(TT_UnitVectorPtr):
    """"""
    def __init__(self,*_args,**_kwargs):
        """"""
        self.this = apply(freetypec.new_TT_UnitVector,_args,_kwargs)
        self.thisown = 1




class TT_VectorPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,freetypec=freetypec):
        if self.thisown == 1 :
            freetypec.delete_TT_Vector(self)
    def __setattr__(self,name,value):
        if name == "x" :
            freetypec.TT_Vector_x_set(self,value)
            return
        if name == "y" :
            freetypec.TT_Vector_y_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "x" : 
            return freetypec.TT_Vector_x_get(self)
        if name == "y" : 
            return freetypec.TT_Vector_y_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Vector instance at %s>" % (self.this,)
class TT_Vector(TT_VectorPtr):
    """"""
    def __init__(self,*_args,**_kwargs):
        """"""
        self.this = apply(freetypec.new_TT_Vector,_args,_kwargs)
        self.thisown = 1




class TT_MatrixPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,freetypec=freetypec):
        if self.thisown == 1 :
            freetypec.delete_TT_Matrix(self)
    def __setattr__(self,name,value):
        if name == "xx" :
            freetypec.TT_Matrix_xx_set(self,value)
            return
        if name == "xy" :
            freetypec.TT_Matrix_xy_set(self,value)
            return
        if name == "yx" :
            freetypec.TT_Matrix_yx_set(self,value)
            return
        if name == "yy" :
            freetypec.TT_Matrix_yy_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "xx" : 
            return freetypec.TT_Matrix_xx_get(self)
        if name == "xy" : 
            return freetypec.TT_Matrix_xy_get(self)
        if name == "yx" : 
            return freetypec.TT_Matrix_yx_get(self)
        if name == "yy" : 
            return freetypec.TT_Matrix_yy_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Matrix instance at %s>" % (self.this,)
class TT_Matrix(TT_MatrixPtr):
    """ A simple 2x2 matrix used for transformations.
 You should use 16.16 fixed floats.           
                                            
   x' = xx*x + xy*y                            
   y' = yx*x + yy*y   
"""
    def __init__(self,*_args,**_kwargs):
        """"""
        self.this = apply(freetypec.new_TT_Matrix,_args,_kwargs)
        self.thisown = 1




class TT_OutlinePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "n_contours" :
            freetypec.TT_Outline_n_contours_set(self,value)
            return
        if name == "n_points" :
            freetypec.TT_Outline_n_points_set(self,value)
            return
        if name == "points" :
            freetypec.TT_Outline_points_set(self,value.this)
            return
        if name == "flags" :
            freetypec.TT_Outline_flags_set(self,value)
            return
        if name == "contours" :
            freetypec.TT_Outline_contours_set(self,value)
            return
        if name == "owner" :
            freetypec.TT_Outline_owner_set(self,value)
            return
        if name == "high_precision" :
            freetypec.TT_Outline_high_precision_set(self,value)
            return
        if name == "second_pass" :
            freetypec.TT_Outline_second_pass_set(self,value)
            return
        if name == "dropout_mode" :
            freetypec.TT_Outline_dropout_mode_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "n_contours" : 
            return freetypec.TT_Outline_n_contours_get(self)
        if name == "n_points" : 
            return freetypec.TT_Outline_n_points_get(self)
        if name == "points" : 
            return TT_VectorPtr(freetypec.TT_Outline_points_get(self))
        if name == "flags" : 
            return freetypec.TT_Outline_flags_get(self)
        if name == "contours" : 
            return freetypec.TT_Outline_contours_get(self)
        if name == "owner" : 
            return freetypec.TT_Outline_owner_get(self)
        if name == "high_precision" : 
            return freetypec.TT_Outline_high_precision_get(self)
        if name == "second_pass" : 
            return freetypec.TT_Outline_second_pass_get(self)
        if name == "dropout_mode" : 
            return freetypec.TT_Outline_dropout_mode_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Outline instance at %s>" % (self.this,)
class TT_Outline(TT_OutlinePtr):
    """A structure used to describe the source glyph to the renderer.   
"""
    def __init__(self,this):
        self.this = this




class TT_BBoxPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "xMin" :
            freetypec.TT_BBox_xMin_set(self,value)
            return
        if name == "yMin" :
            freetypec.TT_BBox_yMin_set(self,value)
            return
        if name == "xMax" :
            freetypec.TT_BBox_xMax_set(self,value)
            return
        if name == "yMax" :
            freetypec.TT_BBox_yMax_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "xMin" : 
            return freetypec.TT_BBox_xMin_get(self)
        if name == "yMin" : 
            return freetypec.TT_BBox_yMin_get(self)
        if name == "xMax" : 
            return freetypec.TT_BBox_xMax_get(self)
        if name == "yMax" : 
            return freetypec.TT_BBox_yMax_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_BBox instance at %s>" % (self.this,)
class TT_BBox(TT_BBoxPtr):
    """A structure used to describe a simple bounding box   
"""
    def __init__(self,this):
        self.this = this




class TT_Glyph_MetricsPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "bbox" :
            freetypec.TT_Glyph_Metrics_bbox_set(self,value.this)
            return
        if name == "bearingX" :
            freetypec.TT_Glyph_Metrics_bearingX_set(self,value)
            return
        if name == "bearingY" :
            freetypec.TT_Glyph_Metrics_bearingY_set(self,value)
            return
        if name == "advance" :
            freetypec.TT_Glyph_Metrics_advance_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "bbox" : 
            return TT_BBoxPtr(freetypec.TT_Glyph_Metrics_bbox_get(self))
        if name == "bearingX" : 
            return freetypec.TT_Glyph_Metrics_bearingX_get(self)
        if name == "bearingY" : 
            return freetypec.TT_Glyph_Metrics_bearingY_get(self)
        if name == "advance" : 
            return freetypec.TT_Glyph_Metrics_advance_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Glyph_Metrics instance at %s>" % (self.this,)
class TT_Glyph_Metrics(TT_Glyph_MetricsPtr):
    """A structure used to return glyph metrics.                         
                                                                
The "bearingX" isn't called "left-side bearing" anymore because   
it has different meanings depending on the glyph's orientation.   
                                                                
The same is true for "bearingY", which is the top-side bearing    
defined by the TT_Spec, i.e., the distance from the baseline to   
the top of the glyph's bbox.  According to our current convention,
this is always the same as "bbox.yMax" but we make it appear for  
consistency in its proper field.                                  
                                                                
The "advance" width is the advance width for horizontal layout,   
and advance height for vertical layouts.                          
                                                                
Finally, the library (ver. 1.1) doesn't support vertical text yet but
these changes were introduced to accomodate it, as it will most
certainly be introduced in later releases.   
"""
    def __init__(self,this):
        self.this = this




class TT_Big_Glyph_MetricsPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "bbox" :
            freetypec.TT_Big_Glyph_Metrics_bbox_set(self,value.this)
            return
        if name == "horiBearingX" :
            freetypec.TT_Big_Glyph_Metrics_horiBearingX_set(self,value)
            return
        if name == "horiBearingY" :
            freetypec.TT_Big_Glyph_Metrics_horiBearingY_set(self,value)
            return
        if name == "vertBearingX" :
            freetypec.TT_Big_Glyph_Metrics_vertBearingX_set(self,value)
            return
        if name == "vertBearingY" :
            freetypec.TT_Big_Glyph_Metrics_vertBearingY_set(self,value)
            return
        if name == "horiAdvance" :
            freetypec.TT_Big_Glyph_Metrics_horiAdvance_set(self,value)
            return
        if name == "vertAdvance" :
            freetypec.TT_Big_Glyph_Metrics_vertAdvance_set(self,value)
            return
        if name == "linearHoriBearingX" :
            freetypec.TT_Big_Glyph_Metrics_linearHoriBearingX_set(self,value)
            return
        if name == "linearHoriAdvance" :
            freetypec.TT_Big_Glyph_Metrics_linearHoriAdvance_set(self,value)
            return
        if name == "linearVertBearingY" :
            freetypec.TT_Big_Glyph_Metrics_linearVertBearingY_set(self,value)
            return
        if name == "linearVertAdvance" :
            freetypec.TT_Big_Glyph_Metrics_linearVertAdvance_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "bbox" : 
            return TT_BBoxPtr(freetypec.TT_Big_Glyph_Metrics_bbox_get(self))
        if name == "horiBearingX" : 
            return freetypec.TT_Big_Glyph_Metrics_horiBearingX_get(self)
        if name == "horiBearingY" : 
            return freetypec.TT_Big_Glyph_Metrics_horiBearingY_get(self)
        if name == "vertBearingX" : 
            return freetypec.TT_Big_Glyph_Metrics_vertBearingX_get(self)
        if name == "vertBearingY" : 
            return freetypec.TT_Big_Glyph_Metrics_vertBearingY_get(self)
        if name == "horiAdvance" : 
            return freetypec.TT_Big_Glyph_Metrics_horiAdvance_get(self)
        if name == "vertAdvance" : 
            return freetypec.TT_Big_Glyph_Metrics_vertAdvance_get(self)
        if name == "linearHoriBearingX" : 
            return freetypec.TT_Big_Glyph_Metrics_linearHoriBearingX_get(self)
        if name == "linearHoriAdvance" : 
            return freetypec.TT_Big_Glyph_Metrics_linearHoriAdvance_get(self)
        if name == "linearVertBearingY" : 
            return freetypec.TT_Big_Glyph_Metrics_linearVertBearingY_get(self)
        if name == "linearVertAdvance" : 
            return freetypec.TT_Big_Glyph_Metrics_linearVertAdvance_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Big_Glyph_Metrics instance at %s>" % (self.this,)
class TT_Big_Glyph_Metrics(TT_Big_Glyph_MetricsPtr):
    """A structure used to return horizontal _and_ vertical glyph        
metrics.                                                          
                                                               
A glyph can be used either in a horizontal or vertical layout.    
Its glyph metrics vary with orientation.  The Big_Glyph_Metrics   
structure is used to return _all_ metrics in one call.            

This structure is currently unused.   
"""
    def __init__(self,this):
        self.this = this




class TT_Instance_MetricsPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "pointSize" :
            freetypec.TT_Instance_Metrics_pointSize_set(self,value)
            return
        if name == "x_ppem" :
            freetypec.TT_Instance_Metrics_x_ppem_set(self,value)
            return
        if name == "y_ppem" :
            freetypec.TT_Instance_Metrics_y_ppem_set(self,value)
            return
        if name == "x_scale" :
            freetypec.TT_Instance_Metrics_x_scale_set(self,value)
            return
        if name == "y_scale" :
            freetypec.TT_Instance_Metrics_y_scale_set(self,value)
            return
        if name == "x_resolution" :
            freetypec.TT_Instance_Metrics_x_resolution_set(self,value)
            return
        if name == "y_resolution" :
            freetypec.TT_Instance_Metrics_y_resolution_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "pointSize" : 
            return freetypec.TT_Instance_Metrics_pointSize_get(self)
        if name == "x_ppem" : 
            return freetypec.TT_Instance_Metrics_x_ppem_get(self)
        if name == "y_ppem" : 
            return freetypec.TT_Instance_Metrics_y_ppem_get(self)
        if name == "x_scale" : 
            return freetypec.TT_Instance_Metrics_x_scale_get(self)
        if name == "y_scale" : 
            return freetypec.TT_Instance_Metrics_y_scale_get(self)
        if name == "x_resolution" : 
            return freetypec.TT_Instance_Metrics_x_resolution_get(self)
        if name == "y_resolution" : 
            return freetypec.TT_Instance_Metrics_y_resolution_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Instance_Metrics instance at %s>" % (self.this,)
class TT_Instance_Metrics(TT_Instance_MetricsPtr):
    """A structure used to return instance metrics.   
"""
    def __init__(self,this):
        self.this = this




class TT_Raster_MapPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def tostring(self, *_args, **_kwargs):
        """This method is a convenience method to get the TT_Raster_Map's raw data as
a Python string.  This string can be imported into a PIL Image using Image.py's
'fromstring' function.  E.g.

  import Image

  bitdata = bitmap.tostring()  # bitmap = TT_Raster_Map(..., type='bit')
  pixdata = pixmap.tostring()  # pixmap = TT_Raster_Map(..., type='pix')

  bitImage = Image.fromstring('1', (bitmap.width, bitmap.rows), 
                              bitdata, 'raw', '1;I')
  pixImage = Image.fromstring('L', (pixmap.cols, pixmap.rows),
                              pixdata, 'raw', 'L;I')

The ';I' in the final arguments causes 0 to be the white and
non-zero to be the non-white (black for bitmaps, some gray 
for pixmaps).  Note the difference in dimensions between bitmaps 
and pixmaps.   
"""
        val = apply(freetypec.TT_Raster_Map_tostring,(self,) + _args, _kwargs)
        return val
    def tobuffer(self, *_args, **_kwargs):
        """This method gets the TT_Raster_Map's raw data as a read-write buffer object.
PIL's 'fromstring' method will take a buffer object as readily as it will a 
string object.  The advantage of a buffer is that it will not copy the memory.

"""
        val = apply(freetypec.TT_Raster_Map_tobuffer,(self,) + _args, _kwargs)
        return val
    def __setattr__(self,name,value):
        if name == "rows" :
            freetypec.TT_Raster_Map_rows_set(self,value)
            return
        if name == "cols" :
            freetypec.TT_Raster_Map_cols_set(self,value)
            return
        if name == "width" :
            freetypec.TT_Raster_Map_width_set(self,value)
            return
        if name == "flow" :
            freetypec.TT_Raster_Map_flow_set(self,value)
            return
        if name == "bitmap" :
            freetypec.TT_Raster_Map_bitmap_set(self,value)
            return
        if name == "size" :
            freetypec.TT_Raster_Map_size_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "rows" : 
            return freetypec.TT_Raster_Map_rows_get(self)
        if name == "cols" : 
            return freetypec.TT_Raster_Map_cols_get(self)
        if name == "width" : 
            return freetypec.TT_Raster_Map_width_get(self)
        if name == "flow" : 
            return freetypec.TT_Raster_Map_flow_get(self)
        if name == "bitmap" : 
            return freetypec.TT_Raster_Map_bitmap_get(self)
        if name == "size" : 
            return freetypec.TT_Raster_Map_size_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Raster_Map instance at %s>" % (self.this,)
class TT_Raster_Map(TT_Raster_MapPtr):
    """A structure used to describe the target bitmap or pixmap to the  
renderer.  Note that there is nothing in this structure that     
gives the nature of the buffer.                                  

IMPORTANT NOTE:                                                  
                                                              
  In the case of a pixmap, the 'width' and 'cols' fields must    
  have the _same_ values, and _must_ be padded to 32-bits, i.e., 
  be a multiple of 4.  Clipping problems will arise otherwise,   
  if not even page faults!                                       
                                                              
  The typical settings are:                                      
                                                              
  - for an WxH bitmap:                                           
                                                              
      rows  = H                                                  
      cols  = (W+7)/8                                            
      width = W                                                  
      flow  = your_choice                                        
                                                              
  - for an WxH pixmap:                                           
                                                              
      rows  = H                                                  
      cols  = (W+3) & ~3                                         
      width = cols                                               
      flow  = your_choice   
"""
    def __init__(self,*_args,**_kwargs):
        """Create a new TT_Raster_Map.  'width' and 'height' are in pixels.  'type' 
is "bit" for a bitmap or "pix" for a pixmap.

"""
        self.this = apply(freetypec.new_TT_Raster_Map,_args,_kwargs)
        self.thisown = 1




class TT_HeaderPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "Table_Version" :
            freetypec.TT_Header_Table_Version_set(self,value)
            return
        if name == "Font_Revision" :
            freetypec.TT_Header_Font_Revision_set(self,value)
            return
        if name == "CheckSum_Adjust" :
            freetypec.TT_Header_CheckSum_Adjust_set(self,value)
            return
        if name == "Magic_Number" :
            freetypec.TT_Header_Magic_Number_set(self,value)
            return
        if name == "Flags" :
            freetypec.TT_Header_Flags_set(self,value)
            return
        if name == "Units_Per_EM" :
            freetypec.TT_Header_Units_Per_EM_set(self,value)
            return
        if name == "Created" :
            freetypec.TT_Header_Created_set(self,value)
            return
        if name == "Modified" :
            freetypec.TT_Header_Modified_set(self,value)
            return
        if name == "xMin" :
            freetypec.TT_Header_xMin_set(self,value)
            return
        if name == "yMin" :
            freetypec.TT_Header_yMin_set(self,value)
            return
        if name == "xMax" :
            freetypec.TT_Header_xMax_set(self,value)
            return
        if name == "yMax" :
            freetypec.TT_Header_yMax_set(self,value)
            return
        if name == "Mac_Style" :
            freetypec.TT_Header_Mac_Style_set(self,value)
            return
        if name == "Lowest_Rec_PPEM" :
            freetypec.TT_Header_Lowest_Rec_PPEM_set(self,value)
            return
        if name == "Font_Direction" :
            freetypec.TT_Header_Font_Direction_set(self,value)
            return
        if name == "Index_To_Loc_Format" :
            freetypec.TT_Header_Index_To_Loc_Format_set(self,value)
            return
        if name == "Glyph_Data_Format" :
            freetypec.TT_Header_Glyph_Data_Format_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "Table_Version" : 
            return freetypec.TT_Header_Table_Version_get(self)
        if name == "Font_Revision" : 
            return freetypec.TT_Header_Font_Revision_get(self)
        if name == "CheckSum_Adjust" : 
            return freetypec.TT_Header_CheckSum_Adjust_get(self)
        if name == "Magic_Number" : 
            return freetypec.TT_Header_Magic_Number_get(self)
        if name == "Flags" : 
            return freetypec.TT_Header_Flags_get(self)
        if name == "Units_Per_EM" : 
            return freetypec.TT_Header_Units_Per_EM_get(self)
        if name == "Created" : 
            return freetypec.TT_Header_Created_get(self)
        if name == "Modified" : 
            return freetypec.TT_Header_Modified_get(self)
        if name == "xMin" : 
            return freetypec.TT_Header_xMin_get(self)
        if name == "yMin" : 
            return freetypec.TT_Header_yMin_get(self)
        if name == "xMax" : 
            return freetypec.TT_Header_xMax_get(self)
        if name == "yMax" : 
            return freetypec.TT_Header_yMax_get(self)
        if name == "Mac_Style" : 
            return freetypec.TT_Header_Mac_Style_get(self)
        if name == "Lowest_Rec_PPEM" : 
            return freetypec.TT_Header_Lowest_Rec_PPEM_get(self)
        if name == "Font_Direction" : 
            return freetypec.TT_Header_Font_Direction_get(self)
        if name == "Index_To_Loc_Format" : 
            return freetypec.TT_Header_Index_To_Loc_Format_get(self)
        if name == "Glyph_Data_Format" : 
            return freetypec.TT_Header_Glyph_Data_Format_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Header instance at %s>" % (self.this,)
class TT_Header(TT_HeaderPtr):
    """The font header TrueType table structure  
"""
    def __init__(self,this):
        self.this = this




class TT_Horizontal_HeaderPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "Version" :
            freetypec.TT_Horizontal_Header_Version_set(self,value)
            return
        if name == "Ascender" :
            freetypec.TT_Horizontal_Header_Ascender_set(self,value)
            return
        if name == "Descender" :
            freetypec.TT_Horizontal_Header_Descender_set(self,value)
            return
        if name == "Line_Gap" :
            freetypec.TT_Horizontal_Header_Line_Gap_set(self,value)
            return
        if name == "advance_Width_Max" :
            freetypec.TT_Horizontal_Header_advance_Width_Max_set(self,value)
            return
        if name == "min_Left_Side_Bearing" :
            freetypec.TT_Horizontal_Header_min_Left_Side_Bearing_set(self,value)
            return
        if name == "min_Right_Side_Bearing" :
            freetypec.TT_Horizontal_Header_min_Right_Side_Bearing_set(self,value)
            return
        if name == "xMax_Extent" :
            freetypec.TT_Horizontal_Header_xMax_Extent_set(self,value)
            return
        if name == "caret_Slope_Rise" :
            freetypec.TT_Horizontal_Header_caret_Slope_Rise_set(self,value)
            return
        if name == "caret_Slope_Run" :
            freetypec.TT_Horizontal_Header_caret_Slope_Run_set(self,value)
            return
        if name == "Reserved0" :
            freetypec.TT_Horizontal_Header_Reserved0_set(self,value)
            return
        if name == "Reserved1" :
            freetypec.TT_Horizontal_Header_Reserved1_set(self,value)
            return
        if name == "Reserved2" :
            freetypec.TT_Horizontal_Header_Reserved2_set(self,value)
            return
        if name == "Reserved3" :
            freetypec.TT_Horizontal_Header_Reserved3_set(self,value)
            return
        if name == "Reserved4" :
            freetypec.TT_Horizontal_Header_Reserved4_set(self,value)
            return
        if name == "metric_Data_Format" :
            freetypec.TT_Horizontal_Header_metric_Data_Format_set(self,value)
            return
        if name == "number_Of_HMetrics" :
            freetypec.TT_Horizontal_Header_number_Of_HMetrics_set(self,value)
            return
        if name == "long_metrics" :
            freetypec.TT_Horizontal_Header_long_metrics_set(self,value)
            return
        if name == "short_metrics" :
            freetypec.TT_Horizontal_Header_short_metrics_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "Version" : 
            return freetypec.TT_Horizontal_Header_Version_get(self)
        if name == "Ascender" : 
            return freetypec.TT_Horizontal_Header_Ascender_get(self)
        if name == "Descender" : 
            return freetypec.TT_Horizontal_Header_Descender_get(self)
        if name == "Line_Gap" : 
            return freetypec.TT_Horizontal_Header_Line_Gap_get(self)
        if name == "advance_Width_Max" : 
            return freetypec.TT_Horizontal_Header_advance_Width_Max_get(self)
        if name == "min_Left_Side_Bearing" : 
            return freetypec.TT_Horizontal_Header_min_Left_Side_Bearing_get(self)
        if name == "min_Right_Side_Bearing" : 
            return freetypec.TT_Horizontal_Header_min_Right_Side_Bearing_get(self)
        if name == "xMax_Extent" : 
            return freetypec.TT_Horizontal_Header_xMax_Extent_get(self)
        if name == "caret_Slope_Rise" : 
            return freetypec.TT_Horizontal_Header_caret_Slope_Rise_get(self)
        if name == "caret_Slope_Run" : 
            return freetypec.TT_Horizontal_Header_caret_Slope_Run_get(self)
        if name == "Reserved0" : 
            return freetypec.TT_Horizontal_Header_Reserved0_get(self)
        if name == "Reserved1" : 
            return freetypec.TT_Horizontal_Header_Reserved1_get(self)
        if name == "Reserved2" : 
            return freetypec.TT_Horizontal_Header_Reserved2_get(self)
        if name == "Reserved3" : 
            return freetypec.TT_Horizontal_Header_Reserved3_get(self)
        if name == "Reserved4" : 
            return freetypec.TT_Horizontal_Header_Reserved4_get(self)
        if name == "metric_Data_Format" : 
            return freetypec.TT_Horizontal_Header_metric_Data_Format_get(self)
        if name == "number_Of_HMetrics" : 
            return freetypec.TT_Horizontal_Header_number_Of_HMetrics_get(self)
        if name == "long_metrics" : 
            return freetypec.TT_Horizontal_Header_long_metrics_get(self)
        if name == "short_metrics" : 
            return freetypec.TT_Horizontal_Header_short_metrics_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Horizontal_Header instance at %s>" % (self.this,)
class TT_Horizontal_Header(TT_Horizontal_HeaderPtr):
    """The horizontal header TrueType table structure

This structure is the one defined by the TrueType 
specification, plus two fields used to link the   
font-units metrics to the header.   
"""
    def __init__(self,this):
        self.this = this




class TT_Vertical_HeaderPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "Version" :
            freetypec.TT_Vertical_Header_Version_set(self,value)
            return
        if name == "Ascender" :
            freetypec.TT_Vertical_Header_Ascender_set(self,value)
            return
        if name == "Descender" :
            freetypec.TT_Vertical_Header_Descender_set(self,value)
            return
        if name == "Line_Gap" :
            freetypec.TT_Vertical_Header_Line_Gap_set(self,value)
            return
        if name == "advance_Height_Max" :
            freetypec.TT_Vertical_Header_advance_Height_Max_set(self,value)
            return
        if name == "min_Top_Side_Bearing" :
            freetypec.TT_Vertical_Header_min_Top_Side_Bearing_set(self,value)
            return
        if name == "min_Bottom_Side_Bearing" :
            freetypec.TT_Vertical_Header_min_Bottom_Side_Bearing_set(self,value)
            return
        if name == "yMax_Extent" :
            freetypec.TT_Vertical_Header_yMax_Extent_set(self,value)
            return
        if name == "caret_Slope_Rise" :
            freetypec.TT_Vertical_Header_caret_Slope_Rise_set(self,value)
            return
        if name == "caret_Slope_Run" :
            freetypec.TT_Vertical_Header_caret_Slope_Run_set(self,value)
            return
        if name == "caret_Offset" :
            freetypec.TT_Vertical_Header_caret_Offset_set(self,value)
            return
        if name == "Reserved1" :
            freetypec.TT_Vertical_Header_Reserved1_set(self,value)
            return
        if name == "Reserved2" :
            freetypec.TT_Vertical_Header_Reserved2_set(self,value)
            return
        if name == "Reserved3" :
            freetypec.TT_Vertical_Header_Reserved3_set(self,value)
            return
        if name == "Reserved4" :
            freetypec.TT_Vertical_Header_Reserved4_set(self,value)
            return
        if name == "metric_Data_Format" :
            freetypec.TT_Vertical_Header_metric_Data_Format_set(self,value)
            return
        if name == "number_Of_VMetrics" :
            freetypec.TT_Vertical_Header_number_Of_VMetrics_set(self,value)
            return
        if name == "long_metrics" :
            freetypec.TT_Vertical_Header_long_metrics_set(self,value)
            return
        if name == "short_metrics" :
            freetypec.TT_Vertical_Header_short_metrics_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "Version" : 
            return freetypec.TT_Vertical_Header_Version_get(self)
        if name == "Ascender" : 
            return freetypec.TT_Vertical_Header_Ascender_get(self)
        if name == "Descender" : 
            return freetypec.TT_Vertical_Header_Descender_get(self)
        if name == "Line_Gap" : 
            return freetypec.TT_Vertical_Header_Line_Gap_get(self)
        if name == "advance_Height_Max" : 
            return freetypec.TT_Vertical_Header_advance_Height_Max_get(self)
        if name == "min_Top_Side_Bearing" : 
            return freetypec.TT_Vertical_Header_min_Top_Side_Bearing_get(self)
        if name == "min_Bottom_Side_Bearing" : 
            return freetypec.TT_Vertical_Header_min_Bottom_Side_Bearing_get(self)
        if name == "yMax_Extent" : 
            return freetypec.TT_Vertical_Header_yMax_Extent_get(self)
        if name == "caret_Slope_Rise" : 
            return freetypec.TT_Vertical_Header_caret_Slope_Rise_get(self)
        if name == "caret_Slope_Run" : 
            return freetypec.TT_Vertical_Header_caret_Slope_Run_get(self)
        if name == "caret_Offset" : 
            return freetypec.TT_Vertical_Header_caret_Offset_get(self)
        if name == "Reserved1" : 
            return freetypec.TT_Vertical_Header_Reserved1_get(self)
        if name == "Reserved2" : 
            return freetypec.TT_Vertical_Header_Reserved2_get(self)
        if name == "Reserved3" : 
            return freetypec.TT_Vertical_Header_Reserved3_get(self)
        if name == "Reserved4" : 
            return freetypec.TT_Vertical_Header_Reserved4_get(self)
        if name == "metric_Data_Format" : 
            return freetypec.TT_Vertical_Header_metric_Data_Format_get(self)
        if name == "number_Of_VMetrics" : 
            return freetypec.TT_Vertical_Header_number_Of_VMetrics_get(self)
        if name == "long_metrics" : 
            return freetypec.TT_Vertical_Header_long_metrics_get(self)
        if name == "short_metrics" : 
            return freetypec.TT_Vertical_Header_short_metrics_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Vertical_Header instance at %s>" % (self.this,)
class TT_Vertical_Header(TT_Vertical_HeaderPtr):
    """The vertical header TrueType table structure

This structure is the one defined by the TrueType 
specification.  Note that it has exactly the same 
layout as the horizontal header (both are loaded  
by the same function).   
"""
    def __init__(self,this):
        self.this = this




class TT_OS2Ptr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "version" :
            freetypec.TT_OS2_version_set(self,value)
            return
        if name == "xAvgCharWidth" :
            freetypec.TT_OS2_xAvgCharWidth_set(self,value)
            return
        if name == "usWeightClass" :
            freetypec.TT_OS2_usWeightClass_set(self,value)
            return
        if name == "usWidthClass" :
            freetypec.TT_OS2_usWidthClass_set(self,value)
            return
        if name == "fsType" :
            freetypec.TT_OS2_fsType_set(self,value)
            return
        if name == "ySubscriptXSize" :
            freetypec.TT_OS2_ySubscriptXSize_set(self,value)
            return
        if name == "ySubscriptYSize" :
            freetypec.TT_OS2_ySubscriptYSize_set(self,value)
            return
        if name == "ySubscriptXOffset" :
            freetypec.TT_OS2_ySubscriptXOffset_set(self,value)
            return
        if name == "ySubscriptYOffset" :
            freetypec.TT_OS2_ySubscriptYOffset_set(self,value)
            return
        if name == "ySuperscriptXSize" :
            freetypec.TT_OS2_ySuperscriptXSize_set(self,value)
            return
        if name == "ySuperscriptYSize" :
            freetypec.TT_OS2_ySuperscriptYSize_set(self,value)
            return
        if name == "ySuperscriptXOffset" :
            freetypec.TT_OS2_ySuperscriptXOffset_set(self,value)
            return
        if name == "ySuperscriptYOffset" :
            freetypec.TT_OS2_ySuperscriptYOffset_set(self,value)
            return
        if name == "yStrikeoutSize" :
            freetypec.TT_OS2_yStrikeoutSize_set(self,value)
            return
        if name == "yStrikeoutPosition" :
            freetypec.TT_OS2_yStrikeoutPosition_set(self,value)
            return
        if name == "sFamilyClass" :
            freetypec.TT_OS2_sFamilyClass_set(self,value)
            return
        if name == "panose" :
            freetypec.TT_OS2_panose_set(self,value)
            return
        if name == "ulUnicodeRange1" :
            freetypec.TT_OS2_ulUnicodeRange1_set(self,value)
            return
        if name == "ulUnicodeRange2" :
            freetypec.TT_OS2_ulUnicodeRange2_set(self,value)
            return
        if name == "ulUnicodeRange3" :
            freetypec.TT_OS2_ulUnicodeRange3_set(self,value)
            return
        if name == "ulUnicodeRange4" :
            freetypec.TT_OS2_ulUnicodeRange4_set(self,value)
            return
        if name == "achVendID" :
            freetypec.TT_OS2_achVendID_set(self,value)
            return
        if name == "fsSelection" :
            freetypec.TT_OS2_fsSelection_set(self,value)
            return
        if name == "usFirstCharIndex" :
            freetypec.TT_OS2_usFirstCharIndex_set(self,value)
            return
        if name == "usLastCharIndex" :
            freetypec.TT_OS2_usLastCharIndex_set(self,value)
            return
        if name == "sTypoAscender" :
            freetypec.TT_OS2_sTypoAscender_set(self,value)
            return
        if name == "sTypoDescender" :
            freetypec.TT_OS2_sTypoDescender_set(self,value)
            return
        if name == "sTypoLineGap" :
            freetypec.TT_OS2_sTypoLineGap_set(self,value)
            return
        if name == "usWinAscent" :
            freetypec.TT_OS2_usWinAscent_set(self,value)
            return
        if name == "usWinDescent" :
            freetypec.TT_OS2_usWinDescent_set(self,value)
            return
        if name == "ulCodePageRange1" :
            freetypec.TT_OS2_ulCodePageRange1_set(self,value)
            return
        if name == "ulCodePageRange2" :
            freetypec.TT_OS2_ulCodePageRange2_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "version" : 
            return freetypec.TT_OS2_version_get(self)
        if name == "xAvgCharWidth" : 
            return freetypec.TT_OS2_xAvgCharWidth_get(self)
        if name == "usWeightClass" : 
            return freetypec.TT_OS2_usWeightClass_get(self)
        if name == "usWidthClass" : 
            return freetypec.TT_OS2_usWidthClass_get(self)
        if name == "fsType" : 
            return freetypec.TT_OS2_fsType_get(self)
        if name == "ySubscriptXSize" : 
            return freetypec.TT_OS2_ySubscriptXSize_get(self)
        if name == "ySubscriptYSize" : 
            return freetypec.TT_OS2_ySubscriptYSize_get(self)
        if name == "ySubscriptXOffset" : 
            return freetypec.TT_OS2_ySubscriptXOffset_get(self)
        if name == "ySubscriptYOffset" : 
            return freetypec.TT_OS2_ySubscriptYOffset_get(self)
        if name == "ySuperscriptXSize" : 
            return freetypec.TT_OS2_ySuperscriptXSize_get(self)
        if name == "ySuperscriptYSize" : 
            return freetypec.TT_OS2_ySuperscriptYSize_get(self)
        if name == "ySuperscriptXOffset" : 
            return freetypec.TT_OS2_ySuperscriptXOffset_get(self)
        if name == "ySuperscriptYOffset" : 
            return freetypec.TT_OS2_ySuperscriptYOffset_get(self)
        if name == "yStrikeoutSize" : 
            return freetypec.TT_OS2_yStrikeoutSize_get(self)
        if name == "yStrikeoutPosition" : 
            return freetypec.TT_OS2_yStrikeoutPosition_get(self)
        if name == "sFamilyClass" : 
            return freetypec.TT_OS2_sFamilyClass_get(self)
        if name == "panose" : 
            return freetypec.TT_OS2_panose_get(self)
        if name == "ulUnicodeRange1" : 
            return freetypec.TT_OS2_ulUnicodeRange1_get(self)
        if name == "ulUnicodeRange2" : 
            return freetypec.TT_OS2_ulUnicodeRange2_get(self)
        if name == "ulUnicodeRange3" : 
            return freetypec.TT_OS2_ulUnicodeRange3_get(self)
        if name == "ulUnicodeRange4" : 
            return freetypec.TT_OS2_ulUnicodeRange4_get(self)
        if name == "achVendID" : 
            return freetypec.TT_OS2_achVendID_get(self)
        if name == "fsSelection" : 
            return freetypec.TT_OS2_fsSelection_get(self)
        if name == "usFirstCharIndex" : 
            return freetypec.TT_OS2_usFirstCharIndex_get(self)
        if name == "usLastCharIndex" : 
            return freetypec.TT_OS2_usLastCharIndex_get(self)
        if name == "sTypoAscender" : 
            return freetypec.TT_OS2_sTypoAscender_get(self)
        if name == "sTypoDescender" : 
            return freetypec.TT_OS2_sTypoDescender_get(self)
        if name == "sTypoLineGap" : 
            return freetypec.TT_OS2_sTypoLineGap_get(self)
        if name == "usWinAscent" : 
            return freetypec.TT_OS2_usWinAscent_get(self)
        if name == "usWinDescent" : 
            return freetypec.TT_OS2_usWinDescent_get(self)
        if name == "ulCodePageRange1" : 
            return freetypec.TT_OS2_ulCodePageRange1_get(self)
        if name == "ulCodePageRange2" : 
            return freetypec.TT_OS2_ulCodePageRange2_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_OS2 instance at %s>" % (self.this,)
class TT_OS2(TT_OS2Ptr):
    """OS/2 Table   
"""
    def __init__(self,this):
        self.this = this




class TT_PostscriptPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "FormatType" :
            freetypec.TT_Postscript_FormatType_set(self,value)
            return
        if name == "italicAngle" :
            freetypec.TT_Postscript_italicAngle_set(self,value)
            return
        if name == "underlinePosition" :
            freetypec.TT_Postscript_underlinePosition_set(self,value)
            return
        if name == "underlineThickness" :
            freetypec.TT_Postscript_underlineThickness_set(self,value)
            return
        if name == "isFixedPitch" :
            freetypec.TT_Postscript_isFixedPitch_set(self,value)
            return
        if name == "minMemType42" :
            freetypec.TT_Postscript_minMemType42_set(self,value)
            return
        if name == "maxMemType42" :
            freetypec.TT_Postscript_maxMemType42_set(self,value)
            return
        if name == "minMemType1" :
            freetypec.TT_Postscript_minMemType1_set(self,value)
            return
        if name == "maxMemType1" :
            freetypec.TT_Postscript_maxMemType1_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "FormatType" : 
            return freetypec.TT_Postscript_FormatType_get(self)
        if name == "italicAngle" : 
            return freetypec.TT_Postscript_italicAngle_get(self)
        if name == "underlinePosition" : 
            return freetypec.TT_Postscript_underlinePosition_get(self)
        if name == "underlineThickness" : 
            return freetypec.TT_Postscript_underlineThickness_get(self)
        if name == "isFixedPitch" : 
            return freetypec.TT_Postscript_isFixedPitch_get(self)
        if name == "minMemType42" : 
            return freetypec.TT_Postscript_minMemType42_get(self)
        if name == "maxMemType42" : 
            return freetypec.TT_Postscript_maxMemType42_get(self)
        if name == "minMemType1" : 
            return freetypec.TT_Postscript_minMemType1_get(self)
        if name == "maxMemType1" : 
            return freetypec.TT_Postscript_maxMemType1_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Postscript instance at %s>" % (self.this,)
class TT_Postscript(TT_PostscriptPtr):
    """Postscript table

Glyph names follow in the file, but we don't
load them by default.  See the ftxpost.c extension.   
"""
    def __init__(self,this):
        self.this = this




class TT_Hdmx_RecordPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "ppem" :
            freetypec.TT_Hdmx_Record_ppem_set(self,value)
            return
        if name == "max_width" :
            freetypec.TT_Hdmx_Record_max_width_set(self,value)
            return
        if name == "widths" :
            freetypec.TT_Hdmx_Record_widths_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "ppem" : 
            return freetypec.TT_Hdmx_Record_ppem_get(self)
        if name == "max_width" : 
            return freetypec.TT_Hdmx_Record_max_width_get(self)
        if name == "widths" : 
            return freetypec.TT_Hdmx_Record_widths_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Hdmx_Record instance at %s>" % (self.this,)
class TT_Hdmx_Record(TT_Hdmx_RecordPtr):
    """horizontal device metrics "hdmx"   
"""
    def __init__(self,this):
        self.this = this




class TT_HdmxPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "version" :
            freetypec.TT_Hdmx_version_set(self,value)
            return
        if name == "num_records" :
            freetypec.TT_Hdmx_num_records_set(self,value)
            return
        if name == "records" :
            freetypec.TT_Hdmx_records_set(self,value.this)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "version" : 
            return freetypec.TT_Hdmx_version_get(self)
        if name == "num_records" : 
            return freetypec.TT_Hdmx_num_records_get(self)
        if name == "records" : 
            return TT_Hdmx_RecordPtr(freetypec.TT_Hdmx_records_get(self))
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Hdmx instance at %s>" % (self.this,)
class TT_Hdmx(TT_HdmxPtr):
    """horizontal device metrics "hdmx"   
"""
    def __init__(self,this):
        self.this = this




class TT_Face_PropertiesPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "num_Glyphs" :
            freetypec.TT_Face_Properties_num_Glyphs_set(self,value)
            return
        if name == "max_Points" :
            freetypec.TT_Face_Properties_max_Points_set(self,value)
            return
        if name == "max_Contours" :
            freetypec.TT_Face_Properties_max_Contours_set(self,value)
            return
        if name == "num_CharMaps" :
            freetypec.TT_Face_Properties_num_CharMaps_set(self,value)
            return
        if name == "num_Names" :
            freetypec.TT_Face_Properties_num_Names_set(self,value)
            return
        if name == "num_Faces" :
            freetypec.TT_Face_Properties_num_Faces_set(self,value)
            return
        if name == "header" :
            freetypec.TT_Face_Properties_header_set(self,value.this)
            return
        if name == "horizontal" :
            freetypec.TT_Face_Properties_horizontal_set(self,value.this)
            return
        if name == "os2" :
            freetypec.TT_Face_Properties_os2_set(self,value.this)
            return
        if name == "postscript" :
            freetypec.TT_Face_Properties_postscript_set(self,value.this)
            return
        if name == "hdmx" :
            freetypec.TT_Face_Properties_hdmx_set(self,value.this)
            return
        if name == "vertical" :
            freetypec.TT_Face_Properties_vertical_set(self,value.this)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "num_Glyphs" : 
            return freetypec.TT_Face_Properties_num_Glyphs_get(self)
        if name == "max_Points" : 
            return freetypec.TT_Face_Properties_max_Points_get(self)
        if name == "max_Contours" : 
            return freetypec.TT_Face_Properties_max_Contours_get(self)
        if name == "num_CharMaps" : 
            return freetypec.TT_Face_Properties_num_CharMaps_get(self)
        if name == "num_Names" : 
            return freetypec.TT_Face_Properties_num_Names_get(self)
        if name == "num_Faces" : 
            return freetypec.TT_Face_Properties_num_Faces_get(self)
        if name == "header" : 
            return TT_HeaderPtr(freetypec.TT_Face_Properties_header_get(self))
        if name == "horizontal" : 
            return TT_Horizontal_HeaderPtr(freetypec.TT_Face_Properties_horizontal_get(self))
        if name == "os2" : 
            return TT_OS2Ptr(freetypec.TT_Face_Properties_os2_get(self))
        if name == "postscript" : 
            return TT_PostscriptPtr(freetypec.TT_Face_Properties_postscript_get(self))
        if name == "hdmx" : 
            return TT_HdmxPtr(freetypec.TT_Face_Properties_hdmx_get(self))
        if name == "vertical" : 
            return TT_Vertical_HeaderPtr(freetypec.TT_Face_Properties_vertical_get(self))
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Face_Properties instance at %s>" % (self.this,)
class TT_Face_Properties(TT_Face_PropertiesPtr):
    """A structure used to describe face properties.   
"""
    def __init__(self,this):
        self.this = this




class TT_EnginePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,freetypec=freetypec):
        if self.thisown == 1 :
            freetypec.delete_TT_Engine(self)
    def __setattr__(self,name,value):
        if name == "z" :
            freetypec.TT_Engine_z_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "z" : 
            return freetypec.TT_Engine_z_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Engine instance at %s>" % (self.this,)
class TT_Engine(TT_EnginePtr):
    """engine instance             
"""
    def __init__(self,this):
        self.this = this




class TT_InstancePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "z" :
            freetypec.TT_Instance_z_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "z" : 
            return freetypec.TT_Instance_z_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Instance instance at %s>" % (self.this,)
class TT_Instance(TT_InstancePtr):
    """instance handle type   
"""
    def __init__(self,this):
        self.this = this




class TT_GlyphPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "z" :
            freetypec.TT_Glyph_z_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "z" : 
            return freetypec.TT_Glyph_z_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Glyph instance at %s>" % (self.this,)
class TT_Glyph(TT_GlyphPtr):
    """glyph handle type   
"""
    def __init__(self,this):
        self.this = this




class TT_FacePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "z" :
            freetypec.TT_Face_z_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "z" : 
            return freetypec.TT_Face_z_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Face instance at %s>" % (self.this,)
class TT_Face(TT_FacePtr):
    """face handle type   
"""
    def __init__(self,this):
        self.this = this




class TT_CharMapPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "z" :
            freetypec.TT_CharMap_z_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "z" : 
            return freetypec.TT_CharMap_z_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_CharMap instance at %s>" % (self.this,)
class TT_CharMap(TT_CharMapPtr):
    """character map handle type   
"""
    def __init__(self,this):
        self.this = this




class TT_Kern_0_PairPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "left" :
            freetypec.TT_Kern_0_Pair_left_set(self,value)
            return
        if name == "right" :
            freetypec.TT_Kern_0_Pair_right_set(self,value)
            return
        if name == "value" :
            freetypec.TT_Kern_0_Pair_value_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "left" : 
            return freetypec.TT_Kern_0_Pair_left_get(self)
        if name == "right" : 
            return freetypec.TT_Kern_0_Pair_right_get(self)
        if name == "value" : 
            return freetypec.TT_Kern_0_Pair_value_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Kern_0_Pair instance at %s>" % (self.this,)
class TT_Kern_0_Pair(TT_Kern_0_PairPtr):
    """format 0 kerning pair   
"""
    def __init__(self,this):
        self.this = this




class TT_Kern_0Ptr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "nPairs" :
            freetypec.TT_Kern_0_nPairs_set(self,value)
            return
        if name == "searchRange" :
            freetypec.TT_Kern_0_searchRange_set(self,value)
            return
        if name == "entrySelector" :
            freetypec.TT_Kern_0_entrySelector_set(self,value)
            return
        if name == "rangeShift" :
            freetypec.TT_Kern_0_rangeShift_set(self,value)
            return
        if name == "pairs" :
            freetypec.TT_Kern_0_pairs_set(self,value.this)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "nPairs" : 
            return freetypec.TT_Kern_0_nPairs_get(self)
        if name == "searchRange" : 
            return freetypec.TT_Kern_0_searchRange_get(self)
        if name == "entrySelector" : 
            return freetypec.TT_Kern_0_entrySelector_get(self)
        if name == "rangeShift" : 
            return freetypec.TT_Kern_0_rangeShift_get(self)
        if name == "pairs" : 
            return TT_Kern_0_PairPtr(freetypec.TT_Kern_0_pairs_get(self))
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Kern_0 instance at %s>" % (self.this,)
class TT_Kern_0(TT_Kern_0Ptr):
    """format 0 kerning subtable   
"""
    def __init__(self,this):
        self.this = this




class TT_Kern_2_ClassPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "firstGlyph" :
            freetypec.TT_Kern_2_Class_firstGlyph_set(self,value)
            return
        if name == "nGlyphs" :
            freetypec.TT_Kern_2_Class_nGlyphs_set(self,value)
            return
        if name == "classes" :
            freetypec.TT_Kern_2_Class_classes_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "firstGlyph" : 
            return freetypec.TT_Kern_2_Class_firstGlyph_get(self)
        if name == "nGlyphs" : 
            return freetypec.TT_Kern_2_Class_nGlyphs_get(self)
        if name == "classes" : 
            return freetypec.TT_Kern_2_Class_classes_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Kern_2_Class instance at %s>" % (self.this,)
class TT_Kern_2_Class(TT_Kern_2_ClassPtr):
    """format 2 kerning glyph class   
"""
    def __init__(self,this):
        self.this = this




class TT_Kern_2Ptr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "rowWidth" :
            freetypec.TT_Kern_2_rowWidth_set(self,value)
            return
        if name == "leftClass" :
            freetypec.TT_Kern_2_leftClass_set(self,value.this)
            return
        if name == "rightClass" :
            freetypec.TT_Kern_2_rightClass_set(self,value.this)
            return
        if name == "array" :
            freetypec.TT_Kern_2_array_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "rowWidth" : 
            return freetypec.TT_Kern_2_rowWidth_get(self)
        if name == "leftClass" : 
            return TT_Kern_2_ClassPtr(freetypec.TT_Kern_2_leftClass_get(self))
        if name == "rightClass" : 
            return TT_Kern_2_ClassPtr(freetypec.TT_Kern_2_rightClass_get(self))
        if name == "array" : 
            return freetypec.TT_Kern_2_array_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Kern_2 instance at %s>" % (self.this,)
class TT_Kern_2(TT_Kern_2Ptr):
    """format 2 kerning subtable   
"""
    def __init__(self,this):
        self.this = this




class TT_Kern_SubtablePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "loaded" :
            freetypec.TT_Kern_Subtable_loaded_set(self,value)
            return
        if name == "version" :
            freetypec.TT_Kern_Subtable_version_set(self,value)
            return
        if name == "offset" :
            freetypec.TT_Kern_Subtable_offset_set(self,value)
            return
        if name == "length" :
            freetypec.TT_Kern_Subtable_length_set(self,value)
            return
        if name == "coverage" :
            freetypec.TT_Kern_Subtable_coverage_set(self,value)
            return
        if name == "format" :
            freetypec.TT_Kern_Subtable_format_set(self,value)
            return
        if name == "t" :
            freetypec.TT_Kern_Subtable_t_set(self,value.this)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "loaded" : 
            return freetypec.TT_Kern_Subtable_loaded_get(self)
        if name == "version" : 
            return freetypec.TT_Kern_Subtable_version_get(self)
        if name == "offset" : 
            return freetypec.TT_Kern_Subtable_offset_get(self)
        if name == "length" : 
            return freetypec.TT_Kern_Subtable_length_get(self)
        if name == "coverage" : 
            return freetypec.TT_Kern_Subtable_coverage_get(self)
        if name == "format" : 
            return freetypec.TT_Kern_Subtable_format_get(self)
        if name == "t" : 
            return TT_Kern_Subtable_tPtr(freetypec.TT_Kern_Subtable_t_get(self))
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Kern_Subtable instance at %s>" % (self.this,)
class TT_Kern_Subtable(TT_Kern_SubtablePtr):
    """kerning subtable   
"""
    def __init__(self,this):
        self.this = this




class TT_Kern_Subtable_tPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "kern0" :
            freetypec.TT_Kern_Subtable_t_kern0_set(self,value.this)
            return
        if name == "kern2" :
            freetypec.TT_Kern_Subtable_t_kern2_set(self,value.this)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "kern0" : 
            return TT_Kern_0Ptr(freetypec.TT_Kern_Subtable_t_kern0_get(self))
        if name == "kern2" : 
            return TT_Kern_2Ptr(freetypec.TT_Kern_Subtable_t_kern2_get(self))
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Kern_Subtable_t instance at %s>" % (self.this,)
class TT_Kern_Subtable_t(TT_Kern_Subtable_tPtr):
    """"""
    def __init__(self,this):
        self.this = this




class TT_KerningPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "version" :
            freetypec.TT_Kerning_version_set(self,value)
            return
        if name == "nTables" :
            freetypec.TT_Kerning_nTables_set(self,value)
            return
        if name == "tables" :
            freetypec.TT_Kerning_tables_set(self,value.this)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "version" : 
            return freetypec.TT_Kerning_version_get(self)
        if name == "nTables" : 
            return freetypec.TT_Kerning_nTables_get(self)
        if name == "tables" : 
            return TT_Kern_SubtablePtr(freetypec.TT_Kerning_tables_get(self))
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Kerning instance at %s>" % (self.this,)
class TT_Kerning(TT_KerningPtr):
    """"""
    def __init__(self,this):
        self.this = this




class TT_Post_20Ptr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "numGlyphs" :
            freetypec.TT_Post_20_numGlyphs_set(self,value)
            return
        if name == "glyphNameIndex" :
            freetypec.TT_Post_20_glyphNameIndex_set(self,value)
            return
        if name == "glyphNames" :
            freetypec.TT_Post_20_glyphNames_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "numGlyphs" : 
            return freetypec.TT_Post_20_numGlyphs_get(self)
        if name == "glyphNameIndex" : 
            return freetypec.TT_Post_20_glyphNameIndex_get(self)
        if name == "glyphNames" : 
            return freetypec.TT_Post_20_glyphNames_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Post_20 instance at %s>" % (self.this,)
class TT_Post_20(TT_Post_20Ptr):
    """format 2.0 table   
"""
    def __init__(self,this):
        self.this = this




class TT_Post_25Ptr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "numGlyphs" :
            freetypec.TT_Post_25_numGlyphs_set(self,value)
            return
        if name == "offset" :
            freetypec.TT_Post_25_offset_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "numGlyphs" : 
            return freetypec.TT_Post_25_numGlyphs_get(self)
        if name == "offset" : 
            return freetypec.TT_Post_25_offset_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Post_25 instance at %s>" % (self.this,)
class TT_Post_25(TT_Post_25Ptr):
    """"""
    def __init__(self,this):
        self.this = this




class TT_PostPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "offset" :
            freetypec.TT_Post_offset_set(self,value)
            return
        if name == "length" :
            freetypec.TT_Post_length_set(self,value)
            return
        if name == "loaded" :
            freetypec.TT_Post_loaded_set(self,value)
            return
        if name == "p" :
            freetypec.TT_Post_p_set(self,value.this)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "offset" : 
            return freetypec.TT_Post_offset_get(self)
        if name == "length" : 
            return freetypec.TT_Post_length_get(self)
        if name == "loaded" : 
            return freetypec.TT_Post_loaded_get(self)
        if name == "p" : 
            return TT_Post_pPtr(freetypec.TT_Post_p_get(self))
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Post instance at %s>" % (self.this,)
class TT_Post(TT_PostPtr):
    """This structure holds the information needed by TT_Get_PS_Name.
Although, it is not passed as an argument in that function and 
one should access it's contents directly, I believe that 
it is necessary to have it available in memory when TT_Get_PS_Name
is called.  Therefore, be sure to create one with TT_Load_PS_Names
and don't let it be deleted until you are finished.   
"""
    def __init__(self,this):
        self.this = this




class TT_Post_pPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "post20" :
            freetypec.TT_Post_p_post20_set(self,value.this)
            return
        if name == "post25" :
            freetypec.TT_Post_p_post25_set(self,value.this)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "post20" : 
            return TT_Post_20Ptr(freetypec.TT_Post_p_post20_get(self))
        if name == "post25" : 
            return TT_Post_25Ptr(freetypec.TT_Post_p_post25_get(self))
        raise AttributeError,name
    def __repr__(self):
        return "<C TT_Post_p instance at %s>" % (self.this,)
class TT_Post_p(TT_Post_pPtr):
    """"""
    def __init__(self,this):
        self.this = this






#-------------- FUNCTION WRAPPERS ------------------

def getOutlinePoints(*_args, **_kwargs):
    """Returns the 'points' member as a list of 2-tuples.  Note that this 
is not a list of TT_Vector instances.   
"""
    val = apply(freetypec.getOutlinePoints,_args,_kwargs)
    return val

def getOutlineFlags(*_args, **_kwargs):
    """Returns the 'flags' member as a tuple of length 'n_points'.  Rather
than representing the actual value that is stored, each element
of the tuple is either 0 or 1 depending on whether the corresponding
point is off or on the curve, respectively.   
"""
    val = apply(freetypec.getOutlineFlags,_args,_kwargs)
    return val

def getOutlineContours(*_args, **_kwargs):
    """Returns the 'contours' member as a tuple of length 'n_contours'.
The returned tuple is a direct mapping from the internal C 
representation to the Python representation.   
"""
    val = apply(freetypec.getOutlineContours,_args,_kwargs)
    return val

def getHdmxRecords(*_args, **_kwargs):
    """Returns the 'records' member as a list of TT_Hdmx_Record's.   
"""
    val = apply(freetypec.getHdmxRecords,_args,_kwargs)
    return val

def newBitmap(*_args, **_kwargs):
    """Create a TT_Raster_Map bitmap structure from the width and height.   
"""
    val = apply(freetypec.newBitmap,_args,_kwargs)
    if val: val = TT_Raster_MapPtr(val); val.thisown = 1
    return val

def newPixmap(*_args, **_kwargs):
    """Create a TT_Raster_Map pixmap structure from the width and height.   
"""
    val = apply(freetypec.newPixmap,_args,_kwargs)
    if val: val = TT_Raster_MapPtr(val); val.thisown = 1
    return val

def TT_Init_FreeType(*_args, **_kwargs):
    """Initialize the engine. 

  Python Note:
    Returns a TT_Engine.  You do not need to pass this function
    a pointer like you do in C.   
"""
    val = apply(freetypec.TT_Init_FreeType,_args,_kwargs)
    if val: val = TT_EnginePtr(val)
    return val

def TT_Done_FreeType(*_args, **_kwargs):
    """"""
    val = apply(freetypec.TT_Done_FreeType,_args,_kwargs)
    return val

def TT_Set_Raster_Gray_Palette(*_args, **_kwargs):
    """Set the gray level palette.  This is an array of 5 bytes used
to produce the font smoothed pixmaps.  By convention:        
                                                               
   palette[0] = background (white)                             
   palette[1] = light                                          
   palette[2] = medium                                         
   palette[3] = dark                                           
   palette[4] = foreground (black) 

Python Note:
   Use a sequence of five integers like so:

     TT_Set_Raster_Gray_Palette(engine, (0, 64, 128, 192, 255))

   'newPixmap' automatically sets the pixmap to 0's.  The rendering 
   engine will sometimes leave a a group of pixels unaltered if they
   all should be set to palette[0], so do not rely on palette[0]'s 
   being something other than 0 to paint that color in.

"""
    val = apply(freetypec.TT_Set_Raster_Gray_Palette,_args,_kwargs)
    return val

def TT_Open_Face(*_args, **_kwargs):
    """Open a new TrueType font file, and returns a handle for
it.

Note: The file can be either a TrueType file (*.ttf) or 
      a TrueType collection (*.ttc, in this case, only  
      the first face is opened).  The number of faces in
      the same collection can be obtained in the face's 
      properties, using TT_Get_Face_Properties() and the
      'max_Faces' field. 
  
Python Note:
  Returns a TT_Face.  You do not need to pass this function
  a pointer like you do in C.  
"""
    val = apply(freetypec.TT_Open_Face,_args,_kwargs)
    if val: val = TT_FacePtr(val)
    return val

def TT_Open_Collection(*_args, **_kwargs):
    """Open a TrueType font file located inside a collection.
The font is assigned by its index in 'fontIndex'. 

Python Note:
  Returns a TT_Face.  You do not need to pass this function
  a pointer like you do in C.  
"""
    val = apply(freetypec.TT_Open_Collection,_args,_kwargs)
    if val: val = TT_FacePtr(val)
    return val

def TT_Get_Face_Properties(*_args, **_kwargs):
    """Return face properties.

Python Note:
  Returns a TT_Face_Properties.  You do not need to pass this function
  a pointer like you do in C.

"""
    val = apply(freetypec.TT_Get_Face_Properties,_args,_kwargs)
    if val: val = TT_Face_PropertiesPtr(val)
    return val

def TT_Flush_Face(*_args, **_kwargs):
    """Close a face's file handle to save system resources.  The file
will be re-opened automatically on the next disk access.   
"""
    val = apply(freetypec.TT_Flush_Face,_args,_kwargs)
    return val

def TT_Get_Face_Metrics(*_args, **_kwargs):
    """Get a face's glyph metrics expressed in font units.

Python Note:
  Returns the arrays as a tuple:

    leftBearings, widths, topBearings, heights = TT_Get_Face_Metrics(face, 
                                                                     firstGlyph, 
                                                                     lastGlyph)

  Values of 'None' mean that there are no vertical metrics in the face.   
"""
    val = apply(freetypec.TT_Get_Face_Metrics,_args,_kwargs)
    return val

def TT_Close_Face(*_args, **_kwargs):
    """Close a given font object, destroying all associated instances.   
"""
    val = apply(freetypec.TT_Close_Face,_args,_kwargs)
    return val

def TT_Get_Font_Data(*_args, **_kwargs):
    """Get font or table data. 

Python Note:
  The Python function differs from the C function in the following ways:

    1. Instead of a numerical 'tag', the Python function takes the table
       name as a string.  I.e. to get the data from the 'eblc' table, 
       you simply pass the string 'eblc' as the second argument rather
       than MAKE_TT_TAG('e', 'b', 'l', 'c').

    2. The buffer is returned to you as a string.  What you can do with
       it, I have no idea, but I am sure that someone will find a clever
       trick.   
"""
    val = apply(freetypec.TT_Get_Font_Data,_args,_kwargs)
    return val

def TT_New_Instance(*_args, **_kwargs):
    """Open a new font instance and returns an instance handle
for it.

Python Note:
  Returns a TT_Instance.  You do not need to pass this function
  a pointer like you do in C.

"""
    val = apply(freetypec.TT_New_Instance,_args,_kwargs)
    if val: val = TT_InstancePtr(val)
    return val

def TT_Set_Instance_Resolutions(*_args, **_kwargs):
    """Set device resolution for a given instance.  The values are
given in dpi (Dots Per Inch).  Default is 96 in both directions.   
"""
    val = apply(freetypec.TT_Set_Instance_Resolutions,_args,_kwargs)
    return val

def TT_Set_Instance_CharSize(*_args, **_kwargs):
    """Set the pointsize (in 1/64 points) for a given instance.  Default is 10pt. 

charSize = pointSize * 64
e.g. for a 12pt fontsize, charSize = 12 * 64 = 768

"""
    val = apply(freetypec.TT_Set_Instance_CharSize,_args,_kwargs)
    return val

def TT_Set_Instance_CharSizes(*_args, **_kwargs):
    """"""
    val = apply(freetypec.TT_Set_Instance_CharSizes,_args,_kwargs)
    return val

def TT_Set_Instance_PointSize(*_args, **_kwargs):
    """Set the pointsize (in points) for a given instance.  Default is 10pt.   
"""
    val = apply(freetypec.TT_Set_Instance_PointSize,_args,_kwargs)
    return val

def TT_Set_Instance_PixelSizes(*_args, **_kwargs):
    """"""
    val = apply(freetypec.TT_Set_Instance_PixelSizes,_args,_kwargs)
    return val

def TT_Get_Instance_Metrics(*_args, **_kwargs):
    """Return instance metrics. 

Python Note:
  Returns a TT_Instance_Metrics.  You do not need to pass this function
  a pointer like you do in C.   
"""
    val = apply(freetypec.TT_Get_Instance_Metrics,_args,_kwargs)
    if val: val = TT_Instance_MetricsPtr(val)
    return val

def TT_Done_Instance(*_args, **_kwargs):
    """"""
    val = apply(freetypec.TT_Done_Instance,_args,_kwargs)
    return val

def TT_New_Glyph(*_args, **_kwargs):
    """Create a new glyph object related to the given 'face'. 

Python Note:
  Returns a TT_Glyph.  You do not need to pass this function
  a pointer like you do in C.   
"""
    val = apply(freetypec.TT_New_Glyph,_args,_kwargs)
    if val: val = TT_GlyphPtr(val)
    return val

def TT_Done_Glyph(*_args, **_kwargs):
    """"""
    val = apply(freetypec.TT_Done_Glyph,_args,_kwargs)
    return val

def TT_Load_Glyph(*_args, **_kwargs):
    """Load and process (scale/transform and hint) a glyph from the
given 'instance'.  The glyph and instance handles must be   
related to the same face object.  The glyph index can be    
computed with a call to TT_Char_Index().                    

The 'load_flags' argument is a combination of the macros    
TTLOAD_SCALE_GLYPH and TTLOAD_HINT_GLYPH.  Hinting will be  
applied only if the scaling is selected.                    

When scaling is off (i.e., load_flags = 0), the returned    
outlines are in EM square coordinates (also called FUnits), 
extracted directly from the font with no hinting.           
Other glyph metrics are also in FUnits.                     

When scaling is on, the returned outlines are in fractional 
pixel units (i.e. signed long = 26.6 fixed floats).          

NOTE:  The glyph index must be in the range 0..num_glyphs-1 
       where 'num_glyphs' is the total number of glyphs in  
       the font file (given in the face properties).   
"""
    val = apply(freetypec.TT_Load_Glyph,_args,_kwargs)
    return val

def TT_Get_Glyph_Outline(*_args, **_kwargs):
    """Return glyph outline pointers in 'outline'.  Note that the returned
pointers are owned by the glyph object, and will be destroyed with 
it.  The client application should _not_ change the pointers. 

Python Note:
  Returns a TT_Outline.  You do not need to pass this function
  a pointer like you do in C.   
"""
    val = apply(freetypec.TT_Get_Glyph_Outline,_args,_kwargs)
    if val: val = TT_OutlinePtr(val)
    return val

def TT_Get_Glyph_Metrics(*_args, **_kwargs):
    """Copy the glyph metrics into 'metrics'. 

Python Note:
  Returns a TT_Glyph_Metrics.  You do not need to pass this function
  a pointer like you do in C.   
"""
    val = apply(freetypec.TT_Get_Glyph_Metrics,_args,_kwargs)
    if val: val = TT_Glyph_MetricsPtr(val)
    return val

def TT_Get_Glyph_Big_Metrics(*_args, **_kwargs):
    """Copy the glyph's big metrics into 'metrics'.
Necessary to obtain vertical metrics. 

Python Note:
  Returns a TT_Big_Glyph_Metrics.  You do not need to pass this function
  a pointer like you do in C.   
"""
    val = apply(freetypec.TT_Get_Glyph_Big_Metrics,_args,_kwargs)
    if val: val = TT_Big_Glyph_MetricsPtr(val)
    return val

def TT_Get_Glyph_Bitmap(*_args, **_kwargs):
    """Render the glyph into a bitmap, with given position offsets.    

Note:  Only use integer pixel offsets to preserve the fine     
       hinting of the glyph and the 'correct' anti-aliasing    
       (where vertical and horizontal stems aren't grayed).    
       This means that xOffset and yOffset must be multiples   
       of 64!   
"""
    val = apply(freetypec.TT_Get_Glyph_Bitmap,_args,_kwargs)
    return val

def TT_Get_Glyph_Pixmap(*_args, **_kwargs):
    """Render the glyph into a pixmap, with given position offsets.    

Note: Only use integer pixel offsets to preserve the fine     
      hinting of the glyph and the 'correct' anti-aliasing    
      (where vertical and horizontal stems aren't grayed).    
      This means that xOffset and yOffset must be multiples   
      of 64!   
"""
    val = apply(freetypec.TT_Get_Glyph_Pixmap,_args,_kwargs)
    return val

def TT_New_Outline(*_args, **_kwargs):
    """Allocate a new outline.  Reserve space for 'numPoints' and 'numContours'. 

Python Note:
  Returns a TT_Outline.  You do not need to pass this function
  a pointer like you do in C.   
"""
    val = apply(freetypec.TT_New_Outline,_args,_kwargs)
    if val: val = TT_OutlinePtr(val)
    return val

def TT_Done_Outline(*_args, **_kwargs):
    """"""
    val = apply(freetypec.TT_Done_Outline,_args,_kwargs)
    return val

def TT_Copy_Outline(*_args, **_kwargs):
    """Copy an outline to another one. 

Python Note:
  Returns a TT_Outline.  You do not need to pass this function
  a pointer like you do in C.   
"""
    val = apply(freetypec.TT_Copy_Outline,_args,_kwargs)
    if val: val = TT_OutlinePtr(val)
    return val

def TT_Get_Outline_Bitmap(*_args, **_kwargs):
    """Render an outline into a bitmap.   
"""
    val = apply(freetypec.TT_Get_Outline_Bitmap,_args,_kwargs)
    return val

def TT_Get_Outline_Pixmap(*_args, **_kwargs):
    """Render an outline into a pixmap -- note that this function uses
a different pixel scale, where 1.0 pixels = 128   
"""
    val = apply(freetypec.TT_Get_Outline_Pixmap,_args,_kwargs)
    return val

def TT_Get_Outline_BBox(*_args, **_kwargs):
    """Return an outline's bounding box -- this function is slow as it
performs a complete scan-line process, without drawing, to get  
the most accurate values. 

Python Note:
  Returns a TT_BBox.  You do not need to pass this function
  a pointer like you do in C.   
"""
    val = apply(freetypec.TT_Get_Outline_BBox,_args,_kwargs)
    if val: val = TT_BBoxPtr(val)
    return val

def TT_Transform_Outline(*_args, **_kwargs):
    """Apply a transformation to a glyph outline.   
"""
    val = apply(freetypec.TT_Transform_Outline,_args,_kwargs)
    return val

def TT_Translate_Outline(*_args, **_kwargs):
    """Apply a translation to a glyph outline.   
"""
    val = apply(freetypec.TT_Translate_Outline,_args,_kwargs)
    return val

def TT_Transform_Vector(*_args, **_kwargs):
    """Apply a transformation to a vector. 

Python Note:
  
  x, y = TT_Transform_Vector(x, y, matrix)

"""
    val = apply(freetypec.TT_Transform_Vector,_args,_kwargs)
    return val

def TT_MulDiv(*_args, **_kwargs):
    """Compute A*B/C with 64 bits intermediate precision.   
"""
    val = apply(freetypec.TT_MulDiv,_args,_kwargs)
    return val

def TT_MulFix(*_args, **_kwargs):
    """Compute A*B/0x10000 with 64 bits intermediate precision. 
Useful to multiply by a 16.16 fixed float value.   
"""
    val = apply(freetypec.TT_MulFix,_args,_kwargs)
    return val

def TT_Get_CharMap_ID(*_args, **_kwargs):
    """Return the ID of charmap number 'charmapIndex' of a given face
used to enumerate the charmaps present in a TrueType file. 

Common character mappings:
  platformID encodingID : Mapping Name
  0          0          : Apple Unicode
  1          0          : Apple Roman
  3          0          : Windows Symbol
  3          1          : Windows Unicode

Python Note:

  platformID, encodingID = TT_Get_CharMap_ID(face, charmapIndex)

"""
    val = apply(freetypec.TT_Get_CharMap_ID,_args,_kwargs)
    return val

def TT_Get_CharMap(*_args, **_kwargs):
    """Look up the character maps found in 'face' and return a handle
for the one matching 'platformID' and 'platformEncodingID'    
(see the TrueType specs relating to the 'cmap' table for      
information on these ID numbers). 

Python Note:
  Returns a TT_CharMap.  You do not need to pass this function
  a pointer like you do in C.   
"""
    val = apply(freetypec.TT_Get_CharMap,_args,_kwargs)
    if val: val = TT_CharMapPtr(val)
    return val

def PyTT_Char_Index(*_args, **_kwargs):
    """Return the index of the character inputChar in the character map.   
"""
    val = apply(freetypec.PyTT_Char_Index,_args,_kwargs)
    return val

def PyTT_String_Indices(*_args, **_kwargs):
    """Return the indices of the characters in inputString as a tuple.

E.g.

  a, b, c = PyTT_String_Indices(charMap, 'abc')

"""
    val = apply(freetypec.PyTT_String_Indices,_args,_kwargs)
    return val

def TT_Char_Index(*_args, **_kwargs):
    """Translate a character code through a given character map
and return the corresponding glyph index to be used in     
a TT_Load_Glyph call.

Python Note:
  For the Windows Unicode mapping (3,1), 

    charCode = ord(character)

    (I think)

"""
    val = apply(freetypec.TT_Char_Index,_args,_kwargs)
    return val

def TT_Get_Name_ID(*_args, **_kwargs):
    """Return the ID of the name number 'nameIndex' of a given face 
used to enumerate the charmaps present in a TrueType file. 

Python Note:

  platformID, encodingID, languageID, nameID = TT_Get_Name_ID(face, nameIndex)

"""
    val = apply(freetypec.TT_Get_Name_ID,_args,_kwargs)
    return val

def TT_Get_Name_String(*_args, **_kwargs):
    """Return the address and length of the name number 'nameIndex'
of a given face.  The string is part of the face object and  
shouldn't be written to or released.                         

Note that if for an invalid platformID a null pointer will   
be returned.   
"""
    val = apply(freetypec.TT_Get_Name_String,_args,_kwargs)
    return val

def TT_Init_Kerning_Extension(*_args, **_kwargs):
    """Initialize Kerning extension, must be called after                
TT_Init_FreeType(). There is no need for a finalizer.

Note on the implemented mechanism:                                

  The kerning table directory is loaded with the face through the    
  extension constructor.  However, the tables will only be loaded    
  on demand, as they may represent a lot of data, unnecessary to     
  most applications.   
"""
    val = apply(freetypec.TT_Init_Kerning_Extension,_args,_kwargs)
    return val

def TT_Get_Kerning_Directory(*_args, **_kwargs):
    """Queries a pointer to the kerning directory for the face object 

Python Note:
  Returns a TT_Kerning.  You do not need to pass this function
  a pointer like you do in C.   
"""
    val = apply(freetypec.TT_Get_Kerning_Directory,_args,_kwargs)
    if val: val = TT_KerningPtr(val)
    return val

def TT_Load_Kerning_Table(*_args, **_kwargs):
    """Load the kerning table number `kern_index' in the kerning         
directory.  The table will stay in memory until the `face'         
face is destroyed.   
"""
    val = apply(freetypec.TT_Load_Kerning_Table,_args,_kwargs)
    return val

def TT_CharMap_First(*_args, **_kwargs):
    """Find the first entry of a Cmap.

Python Note:
  The return value is a 2-tuple of the first valid character
  code in charMap and the first glyph index.

"""
    val = apply(freetypec.TT_CharMap_First,_args,_kwargs)
    return val

def TT_CharMap_Next(*_args, **_kwargs):
    """Find the next entry of Cmap.  Same return conventions. 

Python Note:
  The return value is a 2-tuple of the next valid character
  code in charMap and its corresponding glyph index.

"""
    val = apply(freetypec.TT_CharMap_Next,_args,_kwargs)
    return val

def TT_CharMap_Last(*_args, **_kwargs):
    """Find the last entry of a Cmap.

Python Note:
  The return value is a 2-tuple of the last valid character
  code in charMap and the last glyph index.

"""
    val = apply(freetypec.TT_CharMap_Last,_args,_kwargs)
    return val

def TT_Get_Face_Gasp_Flags(*_args, **_kwargs):
    """This function returns for a given 'point_size' the values of the 
gasp flags 'grid_fit' and 'smooth_font'.  The returned values    
are booleans (where 0 = NO, and 1 = YES).                        

Note that this function will return TT_Err_Table_Missing if      
the font file doesn't contain any gasp table. 

Python Note:

  grid_fit, smooth_font = TT_Get_Face_Gasp_Flags(face, point_size)


"""
    val = apply(freetypec.TT_Get_Face_Gasp_Flags,_args,_kwargs)
    return val

def TT_Init_Post_Extension(*_args, **_kwargs):
    """"""
    val = apply(freetypec.TT_Init_Post_Extension,_args,_kwargs)
    return val

def TT_Load_PS_Names(*_args, **_kwargs):
    """Python Note:
  Returns a TT_Post.  You do not need to pass this function
  a pointer like you do in C.   
"""
    val = apply(freetypec.TT_Load_PS_Names,_args,_kwargs)
    if val: val = TT_PostPtr(val)
    return val

def TT_Get_PS_Name(*_args, **_kwargs):
    """"""
    val = apply(freetypec.TT_Get_PS_Name,_args,_kwargs)
    return val

def TT_Get_Face_Widths(*_args, **_kwargs):
    """Description: Returns the widths and/or heights of a given  
             range of glyphs for a face.                      
                                                             
Input:                                                        
  face        ::  face handle                                
                                                            
  first_glyph ::  first glyph in range                       
                                                             
  last_glyph  ::  last glyph in range                        


Returns:                                                       
  widths      ::  widths expressed in font units (ushorts).
     
  heights     ::  heights expressed in font units (ushorts).
                  
Python Note:
  Returns the 2-tuple of tuples (widths, heights).

  A value of 'None' in 'heights' means that there are no vertical metrics
  in the font file.   
"""
    val = apply(freetypec.TT_Get_Face_Widths,_args,_kwargs)
    return val

def calloc(*_args, **_kwargs):
    """Returns a pointer to a space for an array of nobj objects, each with
size bytes.   Returns NULL if the request can't be satisfied. 
Initializes the space to zero bytes.   
"""
    val = apply(freetypec.calloc,_args,_kwargs)
    return val

def malloc(*_args, **_kwargs):
    """Returns a pointer to space for an object of size bytes.  Returns NULL
upon failure.   
"""
    val = apply(freetypec.malloc,_args,_kwargs)
    return val

def realloc(*_args, **_kwargs):
    """Changes the size of the object pointed to by ptr to size bytes. 
The contents will be unchanged up the minimum of the old and new
sizes.  Returns a pointer to the new space of NULL upon failure,
in which case *ptr is unchanged.   
"""
    val = apply(freetypec.realloc,_args,_kwargs)
    return val

def free(*_args, **_kwargs):
    """Deallocates the space pointed to by ptr.  Does nothing if ptr is NULL.
ptr must be a space previously allocated by calloc, malloc, or realloc.   
"""
    val = apply(freetypec.free,_args,_kwargs)
    return val

def memcpy(*_args, **_kwargs):
    """Copy n characters from ct to s, and return s   
"""
    val = apply(freetypec.memcpy,_args,_kwargs)
    return val

def memmove(*_args, **_kwargs):
    """Same as memcpy except that it works even if the objects overlap.   
"""
    val = apply(freetypec.memmove,_args,_kwargs)
    return val

def memcmp(*_args, **_kwargs):
    """Compare the first n characters of cs with ct.  Returns 0 if
they are equal, <0 if cs < ct, and >0 if cs > ct.   
"""
    val = apply(freetypec.memcmp,_args,_kwargs)
    return val

def memchr(*_args, **_kwargs):
    """Returns pointer to the first occurrence of character c in cs.   
"""
    val = apply(freetypec.memchr,_args,_kwargs)
    return val

def memset(*_args, **_kwargs):
    """Place character c into first n characters of s, return s   
"""
    val = apply(freetypec.memset,_args,_kwargs)
    return val

def ptrcast(*_args, **_kwargs):
    """Casts a pointer ptr to a new datatype given by the string type.
type may be either the SWIG generated representation of a datatype
or the C representation.  For example :

   ptrcast(ptr,"double_p");   # Python representation
   ptrcast(ptr,"double *");    # C representation

A new pointer value is returned.   ptr may also be an integer
value in which case the value will be used to set the pointer
value.  For example :

   a = ptrcast(0,"Vector_p");

Will create a NULL pointer of type "Vector_p"

The casting operation is sensitive to formatting.  As a result,
"double *" is different than "double*".  As a result of thumb,
there should always be exactly one space between the C datatype
and any pointer specifiers (*).
"""
    val = apply(freetypec.ptrcast,_args,_kwargs)
    return val

def ptrvalue(*_args, **_kwargs):
    """Returns the value that a pointer is pointing to (ie. dereferencing).
The type is automatically inferred by the pointer type--thus, an
integer pointer will return an integer, a double will return a double,
and so on.   The index and type fields are optional parameters.  When
an index is specified, this function returns the value of ptr[index].
This allows array access.   When a type is specified, it overrides
the given pointer type.   Examples :

   ptrvalue(a)             #  Returns the value *a
   ptrvalue(a,10)          #  Returns the value a[10]
   ptrvalue(a,10,"double") #  Returns a[10] assuming a is a double *
"""
    val = apply(freetypec.ptrvalue,_args,_kwargs)
    return val

def ptrset(*_args, **_kwargs):
    """Sets the value pointed to by a pointer.  The type is automatically
inferred from the pointer type so this function will work for
integers, floats, doubles, etc...  The index and type fields are
optional.  When an index is given, it provides array access.  When
type is specified, it overrides the given pointer type.  Examples :

  ptrset(a,3)            # Sets the value *a = 3
  ptrset(a,3,10)         # Sets a[10] = 3
  ptrset(a,3,10,"int")   # Sets a[10] = 3 assuming a is a int *
"""
    val = apply(freetypec.ptrset,_args,_kwargs)
    return val

def ptrcreate(*_args, **_kwargs):
    """Creates a new object and returns a pointer to it.  This function 
can be used to create various kinds of objects for use in C functions.
type specifies the basic C datatype to create and value is an
optional parameter that can be used to set the initial value of the
object.  nitems is an optional parameter that can be used to create
an array.  This function results in a memory allocation using
malloc().  Examples :

  a = ptrcreate("double")     # Create a new double, return pointer
  a = ptrcreate("int",7)      # Create an integer, set value to 7
  a = ptrcreate("int",0,1000) # Create an integer array with initial
                              # values all set to zero

This function only recognizes a few common C datatypes as listed below :

       int, short, long, float, double, char, char *, void

All other datatypes will result in an error.  However, other
datatypes can be created by using the ptrcast function.  For
example:

 a = ptrcast(ptrcreate("int",0,100),"unsigned int *")
"""
    val = apply(freetypec.ptrcreate,_args,_kwargs)
    return val

def ptrfree(*_args, **_kwargs):
    """Destroys the memory pointed to by ptr.  This function calls free()
and should only be used with objects created by ptrcreate().  Since
this function calls free, it may work with other objects, but this
is generally discouraged unless you absolutely know what you're
doing.
"""
    val = apply(freetypec.ptrfree,_args,_kwargs)
    return val

def ptradd(*_args, **_kwargs):
    """Adds a value to the current pointer value.  For the C datatypes of
int, short, long, float, double, and char, the offset value is the
number of objects and works in exactly the same manner as in C.  For
example, the following code steps through the elements of an array

 a = ptrcreate("double",0,100);    # Create an array double a[100]
 b = a;
 for i in range(0,100):
     ptrset(b,0.0025*i);           # set *b = 0.0025*i
     b = ptradd(b,1);              # b++ (go to next double)

In this case, adding one to b goes to the next double.

For all other datatypes (including all complex datatypes), the
offset corresponds to bytes.  This function does not perform any
bounds checking and negative offsets are perfectly legal.  
"""
    val = apply(freetypec.ptradd,_args,_kwargs)
    return val

def ptrmap(*_args, **_kwargs):
    """This is a rarely used function that performs essentially the same
operation as a C typedef.  To manage datatypes at run-time, SWIG
modules manage an internal symbol table of type mappings.  This
table keeps track of which types are equivalent to each other.  The
ptrmap() function provides a mechanism for scripts to add symbols
to this table.  For example :

   ptrmap("double_p","Real_p");

would make the types "doublePtr" and "RealPtr" equivalent to each
other.  Pointers of either type could now be used interchangably.

Normally this function is not needed, but it can be used to
circumvent SWIG's normal type-checking behavior or to work around
weird type-handling problems.
"""
    val = apply(freetypec.ptrmap,_args,_kwargs)
    return val



#-------------- VARIABLE WRAPPERS ------------------

cvar = freetypec.cvar
TT_Flow_Down = freetypec.TT_Flow_Down
TT_Flow_Up = freetypec.TT_Flow_Up
TT_Flow_Error = freetypec.TT_Flow_Error
TTLOAD_SCALE_GLYPH = freetypec.TTLOAD_SCALE_GLYPH
TTLOAD_HINT_GLYPH = freetypec.TTLOAD_HINT_GLYPH
TTLOAD_DEFAULT = freetypec.TTLOAD_DEFAULT


#-------------- USER INCLUDE -----------------------

import math
import string

error = cvar.FreeTypeError

def getHdmxRecords(*_args, **_kwargs):
    """Returns the 'records' member as a list of TT_Hdmx_Record's. 
"""
    val = apply(freetypec.getHdmxRecords,_args,_kwargs)
    return map(TT_Hdmx_RecordPtr, val)

def TT_Init_FreeType():
    """Initialize the engine. 

  Python Note:
    Returns a TT_Engine.  You do not need to pass this function
    a pointer like you do in C.   
"""
    val = freetypec.TT_Init_FreeType()
    if val:
        val = TT_EnginePtr(val)
        val.thisown = 1
    return val

def FLOOR(val):
    """Round to the next lower multiple of 64."""
    return val & -64

def CEIL(val):
    """Round to the next highest multiple of 64."""
    return (val + 63) & -64

def rotation(theta):
    """Return a TT_Matrix that represents a counter-clockwise rotation
    of 'theta' degrees.

    [[ cos(theta)  -sin(theta) ]
     [ sin(theta)   cos(theta) ]]

    rotation(theta) --> TT_Matrix"""

    matrix = TT_Matrix()

    angle = theta * math.pi / 180.

    matrix.xx = math.cos(angle) * 2**16
    matrix.xy = math.sin(angle) * 2**16
    matrix.yx = - matrix.xy
    matrix.yy = matrix.xx

    return matrix

def skew(alpha, beta):
    """Return a TT_Matrix that represents a skew transformation which skews
    the x-axis by the angle 'alpha' and the y-axis by the angle 'beta'.

    [[ 1         tan(alpha) ]
     [ tan(beta) 1          ]]

    |      /
    |beta /
    |    /    
    |-->/        _.
    |  /     _.--
    | /  _.-- ^
    |/.--     | alpha
    +----------------

    skew(alpha, beta) --> TT_Matrix"""

    alpha = alpha * math.pi / 180.
    beta  = beta  * math.pi / 180.

    matrix.xx = 2**16
    matrix.xy = math.tan(beta)  * 2**16
    matrix.yx = math.tan(alpha) * 2**16
    matrix.yy = 2 ** 16

    return matrix

transparentPalette = "\000\000\000\000\001\001\001\000\002\002\002\000\003\003\003\000\004\004\004\000\005\005\005\000\006\006\006\000\007\007\007\000\010\010\010\000\011\011\011\000\012\012\012\000\013\013\013\000\014\014\014\000\015\015\015\000\016\016\016\000\017\017\017\000\020\020\020\000\021\021\021\000\022\022\022\000\023\023\023\000\024\024\024\000\025\025\025\000\026\026\026\000\027\027\027\000\030\030\030\000\031\031\031\000\032\032\032\000\033\033\033\000\034\034\034\000\035\035\035\000\036\036\036\000\037\037\037\000\040\040\040\000\041\041\041\000\042\042\042\000\043\043\043\000\044\044\044\000\045\045\045\000\046\046\046\000\047\047\047\000\050\050\050\000\051\051\051\000\052\052\052\000\053\053\053\000\054\054\054\000\055\055\055\000\056\056\056\000\057\057\057\000\060\060\060\000\061\061\061\000\062\062\062\000\063\063\063\000\064\064\064\000\065\065\065\000\066\066\066\000\067\067\067\000\070\070\070\000\071\071\071\000\072\072\072\000\073\073\073\000\074\074\074\000\075\075\075\000\076\076\076\000\077\077\077\000\100\100\100\000\101\101\101\000\102\102\102\000\103\103\103\000\104\104\104\000\105\105\105\000\106\106\106\000\107\107\107\000\110\110\110\000\111\111\111\000\112\112\112\000\113\113\113\000\114\114\114\000\115\115\115\000\116\116\116\000\117\117\117\000\120\120\120\000\121\121\121\000\122\122\122\000\123\123\123\000\124\124\124\000\125\125\125\000\126\126\126\000\127\127\127\000\130\130\130\000\131\131\131\000\132\132\132\000\133\133\133\000\134\134\134\000\135\135\135\000\136\136\136\000\137\137\137\000\140\140\140\000\141\141\141\000\142\142\142\000\143\143\143\000\144\144\144\000\145\145\145\000\146\146\146\000\147\147\147\000\150\150\150\000\151\151\151\000\152\152\152\000\153\153\153\000\154\154\154\000\155\155\155\000\156\156\156\000\157\157\157\000\160\160\160\000\161\161\161\000\162\162\162\000\163\163\163\000\164\164\164\000\165\165\165\000\166\166\166\000\167\167\167\000\170\170\170\000\171\171\171\000\172\172\172\000\173\173\173\000\174\174\174\000\175\175\175\000\176\176\176\000\177\177\177\000\200\200\200\000\201\201\201\000\202\202\202\000\203\203\203\000\204\204\204\000\205\205\205\000\206\206\206\000\207\207\207\000\210\210\210\000\211\211\211\000\212\212\212\000\213\213\213\000\214\214\214\000\215\215\215\000\216\216\216\000\217\217\217\000\220\220\220\000\221\221\221\000\222\222\222\000\223\223\223\000\224\224\224\000\225\225\225\000\226\226\226\000\227\227\227\000\230\230\230\000\231\231\231\000\232\232\232\000\233\233\233\000\234\234\234\000\235\235\235\000\236\236\236\000\237\237\237\000\240\240\240\000\241\241\241\000\242\242\242\000\243\243\243\000\244\244\244\000\245\245\245\000\246\246\246\000\247\247\247\000\250\250\250\000\251\251\251\000\252\252\252\000\253\253\253\000\254\254\254\000\255\255\255\000\256\256\256\000\257\257\257\000\260\260\260\000\261\261\261\000\262\262\262\000\263\263\263\000\264\264\264\000\265\265\265\000\266\266\266\000\267\267\267\000\270\270\270\000\271\271\271\000\272\272\272\000\273\273\273\000\274\274\274\000\275\275\275\000\276\276\276\000\277\277\277\000\300\300\300\000\301\301\301\000\302\302\302\000\303\303\303\000\304\304\304\000\305\305\305\000\306\306\306\000\307\307\307\000\310\310\310\000\311\311\311\000\312\312\312\000\313\313\313\000\314\314\314\000\315\315\315\000\316\316\316\000\317\317\317\000\320\320\320\000\321\321\321\000\322\322\322\000\323\323\323\000\324\324\324\000\325\325\325\000\326\326\326\000\327\327\327\000\330\330\330\000\331\331\331\000\332\332\332\000\333\333\333\000\334\334\334\000\335\335\335\000\336\336\336\000\337\337\337\000\340\340\340\000\341\341\341\000\342\342\342\000\343\343\343\000\344\344\344\000\345\345\345\000\346\346\346\000\347\347\347\000\350\350\350\000\351\351\351\000\352\352\352\000\353\353\353\000\354\354\354\000\355\355\355\000\356\356\356\000\357\357\357\000\360\360\360\000\361\361\361\000\362\362\362\000\363\363\363\000\364\364\364\000\365\365\365\000\366\366\366\000\367\367\367\000\370\370\370\000\371\371\371\000\372\372\372\000\373\373\373\000\374\374\374\000\375\375\375\000\376\376\376\000\377\377\377\377"

def drawString(text, font='/Windows/Fonts/Times.ttf', size=24):
    text = string.expandtabs(text)
    
    e = TT_Init_FreeType()

    face = TT_Open_Face(e, font)

    props = TT_Get_Face_Properties(face)

    ins = TT_New_Instance(face)
    TT_Set_Instance_Resolutions(ins, 96, 96)
    TT_Set_Instance_PointSize(ins, size)

    for id in range(props.num_CharMaps):
        tup = TT_Get_CharMap_ID(face, id)
        if tup == (3,1):
            # Windows Unicode mapping
            cm = TT_Get_CharMap(face, id)
            break
        
    glyphIndices = PyTT_String_Indices(cm, text)

    glyphCache = {}
    metricCache = {}
    minyMin = 0
    maxyMax = 0

    for i in range(len(text)):
        if not glyphCache.has_key(text[i]):
            glyphCache[text[i]] = TT_New_Glyph(face)
            TT_Load_Glyph(ins, glyphCache[text[i]], glyphIndices[i], TTLOAD_DEFAULT)
            metricCache[text[i]] = TT_Get_Glyph_Metrics(glyphCache[text[i]])
            maxyMax = max(maxyMax, metricCache[text[i]].bbox.yMax)
            if i != 0:
                minyMin = min(minyMin, metricCache[text[i]].bbox.yMin)
            else:
                minyMin = metricCache[text[i]].bbox.yMin

    width = 0
    for i in text:
        width = width + metricCache[i].advance
    width = width / 64

    height = (maxyMax - minyMin) / 64

    raster = TT_Raster_Map(width, height, 'bit')

    curX = 0

    for char in text:
        TT_Get_Glyph_Bitmap(glyphCache[char], raster, curX - metricCache[char].bbox.xMin, -minyMin)
        curX = curX + metricCache[char].advance

    return raster

def toImage(rasterMap, type='bit'):
    import Image

    if type == 'bit':
        im = Image.fromstring("1", (rasterMap.width, rasterMap.rows),
                              rasterMap.tostring(), "raw", "1;I")
    elif type == 'pix':
        im = Image.fromstring("L", (rasterMap.cols, rasterMap.rows),
                              rasterMap.tostring(), "raw", "L;I")
    
    return im
