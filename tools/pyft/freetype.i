%title "PyFT -- Python's FreeType Wrapper", pre, noinfo
%module freetype

%{
#include "freetype.h"
#include "ftxerr18.h"
#include "ftxkern.h"
#include "ftxcmap.h"
#include "ftxgasp.h"
#include "ftxpost.h"
#include "ftxwidth.h"

static PyObject *FreeTypeError;
%}

typedef unsigned short TT_Bool;
typedef signed long TT_Fixed;
typedef signed short TT_FWord;
typedef unsigned short TT_UFWord;
typedef char TT_Text;
typedef char TT_String;
typedef signed char TT_Char;
typedef unsigned char TT_Byte;
typedef signed short TT_Short;
typedef unsigned short TT_UShort;
typedef int TT_Int;
typedef unsigned int TT_UInt;
typedef signed long TT_Long;
typedef unsigned long TT_ULong;
typedef signed short TT_F2Dot14;
typedef signed long TT_F26Dot6;
typedef signed long TT_Pos;

%typemap(python,out) TT_Error {
  if (*$source) {
    char *errstring;

    errstring = TT_ErrToString18(*$source);

    PyErr_SetString(FreeTypeError, errstring);
    return NULL;
  }
  else {
    $target = Py_None;
    Py_INCREF(Py_None);
  }
}

%disabledoc
%include typemaps.i

PyObject *FreeTypeError;

%enabledoc

%section "Introduction"
%text %{
Welcome to PyFT, a hopefully complete wrapping of the FreeType 1.2 rendering
engine for TrueType fonts.  Most of this is a direct and nearly exhaustive
translation of the FreeType API.  A few of the functions have been given
more "Pythonic" wrappings for convenience.  There are also a couple of 
_de_novo_ convenience functions to access certain bits of data.

Documenting FreeType itself is not my goal here.  The following documentation
was gleaned almost entirely from the comments in the C headers.  To learn
how to use the API, I refer you to the documentation distributed with the 
FreeType source code.  Specifically, you will want to read the User Guide
<URL: http://www.freetype.org/docs/user.txt>, skim the API Reference
<URL: http://www.freetype.org/docs/apiref.txt>, and read the Introduction to Glyphs
<URL: http://www.freetype.org/glyphs.htm>.  All of these documents
are also included in the "docs" subdirectory of the binary PyFT distribution with 
some other useful documents.
%}

%subsection "Python Notes"
%text %{
To make using FreeType from Python easier, I took some liberties with the API.

 * In C, most of the API functions return an error code; in Python, they either
   return None or the values that would be placed in their "output arguments" in C.
   If an error occurs, the function will raise a "FreeTypeError" and display an
   appropriate error message.

 * Some of the API functions take a reference to an uninitialized structure and 
   initialize that structure.  E.g.

     TT_Engine engine;
     TT_Face face;

     TT_Init_FreeType(&engine);
     TT_Open_Face(engine, "arial.ttf", &face);
     ...

   In Python, such functions do not take this argument and will return a suitably 
   initialized structure.  The equivalent Python code is as follows:

     engine = TT_Init_FreeType()
     face = TT_Open_Face(engine, "arial.ttf")

   The exception to this exception is TT_Raster_Map, the structure that holds the 
   bitmap and pixmap data.  Since they need extra parameters to specify them (say, 
   height and width), they must be created by calling TT_Raster_Map(...) with the 
   appropriate arguments.  They are then passed to the rendering functions normally.

 * Similarly, for "output arguments" that are readily mapped to Python types
   (e.g. TT_Long becomes a PyInt), the function will return them in a tuple.  E.g.
   the C code:

     ...
     TT_UShort platformID;
     TT_UShort encodingID;

     TT_Get_CharMap_ID(face, charmapIndex, &platformID, &encodingID);
     ...

   is replaced with 

     platformID, encodingID = TT_Get_CharMap_ID(face, charmapIndex)

 * Due to the limitations of SWIG and/or my laziness to work around them, some
   members of structures can only be meaningfully accessed from helper functions.
   See the section "Structure Helper Functions".

 * Some of the structures have TT_Done_<Foo> functions that clean them up.
   Getting the order of these right is somewhat tricky.  USE WITH EXTREME CAUTION.
   be sure not to access their pointers after using these functions.

 * The Python function 'toImage' will take a TT_Raster_Map and convert it
   to a PIL Image.

     Image = toImage(rasterMap, type='bit')

   The keyword argument can be either 'bit' for a bitmap or 'pix' for a pixmap.

 * Convenience functions 'FLOOR' and 'CEIL' are for the round integers to a 
   multiple of 64.

 * Functions 'rotation' and 'skew' will return TT_Matrix's for rotating by 
   an angle and skewing.

 * 'transparentPalette' is a constant for use with PIL.  You can use it as a PIL
   palette to convert bitmaps and pixmaps from their own modes to the RGBA mode.
   The image will then have all of the white space transparent.

 * 'drawString' is an example of rendering text onto a bitmap.  Its signature is

     drawString(text, font='/Windows/Fonts/Times.ttf', size=24) --> TT_Raster_Map

   Its method does not translate directly to pixmaps as the FreeType renderer does not
   implement transparency, so glyphs will sometimes 'blank out' portions of preceding 
   glyphs.  A correct pixmap implementation will have to create pixmaps for individual
   glyphs and then place them with PIL.

 * Memory and pointer manipulation functions from SWIG are also exposed.  They are 
   documented in later sections.
%}

%subsection "Extensions"
%text %{
There are several extensions to the core FreeType renderer.  The following
are exposed in PyFT:

  ftxkern:  Access to the kerning tables given by each font.
            The wrapping is somewhat non-functional at the moment. I need
            to figure out what one is supposed to do with the kerning 
            structures before I can wrap the intelligently.  I have included
	    them here mostly because I like seeing my name littered through
	    the source code.  :-)
            Be sure to call TT_Init_Kerning_Extension(engine) first!

  ftxcmap:  Iteration over the 'cmap' table.
	    You probably won't need this extension.  Python offers better
            ways to get the information you need from the standard character
            map functions.  Nonetheless, it's here for your enjoyment.

  ftxgasp:  Access to the 'gasp' table.
	    If you know why you would need this information, this is where
	    to get it from.

  ftxpost:  PostScript glyph names.
	    You can obtain the PostScript names of each glyph from the functions
	    from this extension.
	    Be sure to call TT_Init_Post_Extension(engine) first!

  ftxwidth: Glyph widths.
	    Get the widths for a range of glyphs.

The ftxerr18 extension converts error codes into error messages.  The error handler
uses this extension internally, so that you will never see a bare error code.  Thus
its API does not need to be exposed here.

The other FreeType extensions are (as of version 1.2) still experimental, so I won't
bother wrapping them just yet.

When compiling PyFT, your FreeType library must include the following extensions:

  ftxkern
  ftxcmap
  ftxgasp
  ftxpost
  ftxwidth
  ftxerr18

%}

%subsection "To Do"
%text %{

 * Add a good OO layer on top of the shadow classes.  I am thinking of having a 
   class that automatically starts an engine on initialization and will keep 
   track of what fonts are open, their attributes, and so on.  Subclasses can
   handle the rendering onto different substrates.

 * Do simple text rendering from Python onto a PIL Image.

 * Do simple text rendering from C onto a bitmap that is converted to a PIL Image.

 * Write a converter from a TrueType font to a PIL font.

 * Make the PyFT-PIL connectively suitable for using PyFT to draw strings for
   PIDDLE.

 * Figure out how to do rotated strings on both bitmaps and pixmaps cleanly.

 * *** Fix Memory Leaks! ***  They are probably rampant here.  There is a reason
   why I like Python; I do not have to think about such things.
%}

%section "Structures"

%pragma make_default

typedef struct
  {
    TT_F2Dot14  x;
    TT_F2Dot14  y;
  } TT_UnitVector;

typedef  struct
  {
    TT_F26Dot6  x;
    TT_F26Dot6  y;
  } TT_Vector;

typedef  struct  
  {/*  A simple 2x2 matrix used for transformations.
    You should use 16.16 fixed floats.           
                                               
      x' = xx*x + xy*y                            
      y' = yx*x + yy*y */
    TT_Fixed  xx, xy;
    TT_Fixed  yx, yy;
  } TT_Matrix;

%pragma no_default
typedef  struct  
  {/* A structure used to describe the source glyph to the renderer. */

    TT_Short         n_contours; /* number of contours in glyph */
    TT_UShort        n_points; /* number of points in the glyph */

    TT_Vector*       points; /* the outline's points */
    TT_Byte* flags; /* the points flags */
    TT_UShort* contours; /* the contour end points */

    TT_Bool          owner; 
/* The owner flag indicates that the outline owns the arrays it 
   refers to.  Typically, this is true of outlines created from the
   TT_New_Outline() API, while it isn't for those returned by 
   TT_Get_Glyph_Outline(). */

    TT_Bool          high_precision;
/* When true, the scan-line converter will use
   a higher precision to render bitmaps (i.e. a
   1/1024 pixel precision).  This is important for
   small ppem sizes. */

    TT_Bool          second_pass;
/* When true, the scan-line converter performs    
   a second sweep phase dedicated to find         
   vertical drop-outs.  If false, only horizontal 
   drop-outs will be checked during the first     
   vertical sweep (yes, this is a bit confusing   
   but it's really the way it should work).       
   This is important for small ppems too.   */

    TT_Char          dropout_mode; 
/* Specifies the TrueType drop-out mode to
   use for continuity checking. valid values      
   are 0 (no check), 1, 2, 4, and 5. */

} TT_Outline;

/* %disabledoc */
/* %addmethods TT_Outline { */
/*   ~TT_Outline() { */
/*     TT_Done_Outline(self); */
/* printf("Done Outline\n"); */
/*     free(self); */
/*   }   */
/* } */
/* %enabledoc */

typedef  struct 
  {/* A structure used to describe a simple bounding box */

    TT_Pos  xMin;
    TT_Pos  yMin;
    TT_Pos  xMax;
    TT_Pos  yMax;
  } TT_BBox;

typedef  struct  
  {/* A structure used to return glyph metrics.                         
                                                                   
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
   certainly be introduced in later releases. */

    TT_BBox  bbox; /* glyph bounding box */

    TT_Pos   bearingX; /* left-side bearing */
    TT_Pos   bearingY; /* top-side bearing, per se the TT spec */

    TT_Pos   advance; /* advance width (or height) */
  } TT_Glyph_Metrics;

typedef  struct 
  {/* A structure used to return horizontal _and_ vertical glyph        
   metrics.                                                          
                                                                  
   A glyph can be used either in a horizontal or vertical layout.    
   Its glyph metrics vary with orientation.  The Big_Glyph_Metrics   
   structure is used to return _all_ metrics in one call.            

   This structure is currently unused. */
    TT_BBox  bbox; /* glyph bounding box */

    TT_Pos   horiBearingX; /* left side bearing in horizontal layouts */
    TT_Pos   horiBearingY; /* top side bearing in horizontal layouts */

    TT_Pos   vertBearingX; /* left side bearing in vertical layouts */
    TT_Pos   vertBearingY; /* top side bearing in vertical layouts */

    TT_Pos   horiAdvance; /* advance width for horizontal layout */
    TT_Pos   vertAdvance; /* advance height for vertical layout */

    TT_Pos   linearHoriBearingX; /* linearly scaled horizontal lsb */
    TT_Pos   linearHoriAdvance; /* linearly scaled horizontal advance */

    TT_Pos   linearVertBearingY; /* linearly scaled vertical tsb */
    TT_Pos   linearVertAdvance; /* linearly scaled vertical advance */
  } TT_Big_Glyph_Metrics;

typedef  struct  
  {/* A structure used to return instance metrics. */
    TT_F26Dot6  pointSize; /* char. size in points (1pt = 1/72 inch) */

    TT_UShort   x_ppem; /* horizontal pixels per EM square */
    TT_UShort   y_ppem; /* vertical pixels per EM square */

    TT_Fixed    x_scale; /* 16.16 to convert from EM units to 26.6 pix */
    TT_Fixed    y_scale; /* 16.16 to convert from EM units to 26.6 pix */

    TT_UShort   x_resolution; /* device horizontal resolution in dpi */
    TT_UShort   y_resolution; /* device vertical resolution in dpi */
  } TT_Instance_Metrics;

typedef  struct  
  {/* A structure used to describe the target bitmap or pixmap to the  
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
         flow  = your_choice */

    int    rows;    /* number of rows                    */
    int    cols;    /* number of columns (bytes) per row */
    int    width;   /* number of pixels per line         */
    int    flow;    /* bitmap orientation                */

    void*  bitmap;  /* bit/pixmap buffer                 */
    long   size;    /* bit/pixmap size in bytes          */

  } TT_Raster_Map;

%addmethods TT_Raster_Map {
  PyObject *tostring() {
    PyObject *pystring;
    unsigned char *string;
    int length;
    int i;

    length = self->rows * self->cols;
    
    string = (unsigned char *)calloc(length, sizeof(unsigned char));
    
    for (i=0; i<length; i++) {
      string[i] = ((unsigned char *)(self->bitmap))[i];
    }

    pystring = PyString_FromStringAndSize(string, length);

    return pystring;
  }
/* This method is a convenience method to get the TT_Raster_Map's raw data as
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
   and pixmaps. */

  PyObject *tobuffer() {
    return PyBuffer_FromReadWriteMemory(self->bitmap, self->rows*self->cols);
  }
/* This method gets the TT_Raster_Map's raw data as a read-write buffer object.
   PIL's 'fromstring' method will take a buffer object as readily as it will a 
   string object.  The advantage of a buffer is that it will not copy the memory.
 */

  TT_Raster_Map(int width, int height, char *type="bit") {
    TT_Raster_Map *retMap;

    retMap = (TT_Raster_Map *)calloc(1, sizeof(TT_Raster_Map));

    if (strcmp(type, "bit")==0) {
      retMap->width = width;
      retMap->cols = (width+7) / 8;
      retMap->rows = height;
      retMap->flow = TT_Flow_Down;
      retMap->size = retMap->cols * retMap->rows;
      retMap->bitmap = malloc(retMap->size);

      memset(retMap->bitmap, 0, retMap->size);
      /* clear the buffer */

      return retMap;
    }
    else if (strcmp(type, "pix")==0) {
      retMap->width = width;
      retMap->cols = (width+3) & ~3;
      retMap->rows = height;
      retMap->flow = TT_Flow_Down;
      retMap->size = retMap->cols * retMap->rows;
      retMap->bitmap = malloc(retMap->size);

      memset(retMap->bitmap, 0, retMap->size);
      /* clear the buffer */

      return retMap;
    }
    else {
      free(retMap);
      PyErr_SetString(PyExc_ValueError, "final argument must be either 'bit' or 'pix'");
      return NULL;
    }
  }
/* Create a new TT_Raster_Map.  'width' and 'height' are in pixels.  'type' 
   is "bit" for a bitmap or "pix" for a pixmap.
*/

}

%typemap(python,out) TT_Long [2] {
  PyObject *tmpTuple;
  PyObject *x;

  tmpTuple = PyTuple_New(2);

  x = PyInt_FromLong($source[0]);
  PyTuple_SetItem(tmpTuple, 0, x);

  x = PyInt_FromLong($source[1]);
  PyTuple_SetItem(tmpTuple, 1, x);

  $target = tmpTuple;
}

%typemap(python,memberout) TT_Long [2] {
  $target[0] = $source[0];
  $target[1] = $source[1];
}

typedef  struct  
  {/* The font header TrueType table structure*/

    TT_Fixed   Table_Version;
    TT_Fixed   Font_Revision;

    TT_Long    CheckSum_Adjust;
    TT_Long    Magic_Number;

    TT_UShort  Flags;
    TT_UShort  Units_Per_EM;

    TT_Long    Created [2];
    TT_Long    Modified[2];

    TT_FWord   xMin;
    TT_FWord   yMin;
    TT_FWord   xMax;
    TT_FWord   yMax;

    TT_UShort  Mac_Style;
    TT_UShort  Lowest_Rec_PPEM;

    TT_Short   Font_Direction;
    TT_Short   Index_To_Loc_Format;
    TT_Short   Glyph_Data_Format;
  } TT_Header;

typedef  struct  
  {/* The horizontal header TrueType table structure

   This structure is the one defined by the TrueType 
   specification, plus two fields used to link the   
   font-units metrics to the header. */

    TT_Fixed   Version;
    TT_FWord   Ascender;
    TT_FWord   Descender;
    TT_FWord   Line_Gap;

    TT_UFWord  advance_Width_Max;      /* advance width maximum */

    TT_FWord   min_Left_Side_Bearing;  /* minimum left-sb       */
    TT_FWord   min_Right_Side_Bearing; /* minimum right-sb      */
    TT_FWord   xMax_Extent;            /* xmax extents          */
    TT_FWord   caret_Slope_Rise;
    TT_FWord   caret_Slope_Run;

    TT_Short   Reserved0,
               Reserved1,
               Reserved2,
               Reserved3,
               Reserved4;

    TT_Short   metric_Data_Format;
    TT_UShort  number_Of_HMetrics;

    void*      long_metrics;
    void*      short_metrics;
  } TT_Horizontal_Header;

typedef  struct  
  {/* The vertical header TrueType table structure

   This structure is the one defined by the TrueType 
   specification.  Note that it has exactly the same 
   layout as the horizontal header (both are loaded  
   by the same function). */

    TT_Fixed   Version;
    TT_FWord   Ascender;
    TT_FWord   Descender;
    TT_FWord   Line_Gap;

    TT_UFWord  advance_Height_Max;      /* advance height maximum */

    TT_FWord   min_Top_Side_Bearing;    /* minimum left-sb or top-sb       */
    TT_FWord   min_Bottom_Side_Bearing; /* minimum right-sb or bottom-sb   */
    TT_FWord   yMax_Extent;             /* xmax or ymax extents            */
    TT_FWord   caret_Slope_Rise;
    TT_FWord   caret_Slope_Run;
    TT_FWord   caret_Offset;

    TT_Short   Reserved1,
               Reserved2,
               Reserved3,
               Reserved4;

    TT_Short   metric_Data_Format;
    TT_UShort  number_Of_VMetrics;

    void*      long_metrics;
    void*      short_metrics;
  } TT_Vertical_Header;

%typemap(python,out) TT_Byte panose[10] {
  PyObject *string;

  string = PyString_FromStringAndSize($source, 10);

  $target = string;
}

%typemap(python,memberout) TT_Byte panose[10] {
  strncpy($target, $source, 10);
}
  
%typemap(python,out) TT_Char achVendID[4] {
  PyObject *string;

  string = PyString_FromStringAndSize($source, 4);

  $target = string;
}

%typemap(python,memberout) TT_Char achVendID[4] {
  strncpy($target, $source, 4);
}

typedef  struct  
  {/* OS/2 Table */

    TT_UShort  version;                /* 0x0001 */
    TT_FWord   xAvgCharWidth;
    TT_UShort  usWeightClass;
    TT_UShort  usWidthClass;
    TT_Short   fsType;
    TT_FWord   ySubscriptXSize;
    TT_FWord   ySubscriptYSize;
    TT_FWord   ySubscriptXOffset;
    TT_FWord   ySubscriptYOffset;
    TT_FWord   ySuperscriptXSize;
    TT_FWord   ySuperscriptYSize;
    TT_FWord   ySuperscriptXOffset;
    TT_FWord   ySuperscriptYOffset;
    TT_FWord   yStrikeoutSize;
    TT_FWord   yStrikeoutPosition;
    TT_Short   sFamilyClass;

    TT_Byte    panose[10];

    TT_ULong   ulUnicodeRange1;        /* Bits 0-31   */
    TT_ULong   ulUnicodeRange2;        /* Bits 32-63  */
    TT_ULong   ulUnicodeRange3;        /* Bits 64-95  */
    TT_ULong   ulUnicodeRange4;        /* Bits 96-127 */

    TT_Char    achVendID[4];

    TT_UShort  fsSelection;
    TT_UShort  usFirstCharIndex;
    TT_UShort  usLastCharIndex;
    TT_Short   sTypoAscender;
    TT_Short   sTypoDescender;
    TT_Short   sTypoLineGap;
    TT_UShort  usWinAscent;
    TT_UShort  usWinDescent;

    TT_ULong   ulCodePageRange1;       /* Bits 0-31   */
    TT_ULong   ulCodePageRange2;       /* Bits 32-63  */
  } TT_OS2;

typedef  struct  
  {/* Postscript table

   Glyph names follow in the file, but we don't
   load them by default.  See the ftxpost.c extension. */

    TT_Fixed  FormatType;
    TT_Fixed  italicAngle;
    TT_FWord  underlinePosition;
    TT_FWord  underlineThickness;
    TT_ULong  isFixedPitch;
    TT_ULong  minMemType42;
    TT_ULong  maxMemType42;
    TT_ULong  minMemType1;
    TT_ULong  maxMemType1;

  } TT_Postscript;

typedef  struct  
  {/* horizontal device metrics "hdmx" */

    TT_Byte   ppem;
    TT_Byte   max_width;
    TT_Byte*  widths;
  } TT_Hdmx_Record;

typedef  struct  
  {/* horizontal device metrics "hdmx" */

    TT_UShort        version;
    TT_Short         num_records;
    TT_Hdmx_Record*  records;
  } TT_Hdmx;

typedef  struct  
  {/* A structure used to describe face properties. */

    TT_UShort  num_Glyphs;      /* number of glyphs in face              */
    TT_UShort  max_Points;      /* maximum number of points in a glyph   */
    TT_UShort  max_Contours;    /* maximum number of contours in a glyph */

    TT_UShort  num_CharMaps;    /* number of charmaps in the face     */
    TT_UShort  num_Names;       /* number of name records in the face */

    TT_ULong   num_Faces;  
/* 1 for normal TrueType files, and the number 
   of embedded faces for TrueType collections */

    TT_Header*             header;        /* TrueType header table          */
    TT_Horizontal_Header*  horizontal;    /* TrueType horizontal header     */
    TT_OS2*                os2;           /* TrueType OS/2 table            */
    TT_Postscript*         postscript;    /* TrueType Postscript table      */
    TT_Hdmx*               hdmx;
    TT_Vertical_Header*    vertical;      /* TT Vertical header, if present */
  } TT_Face_Properties;

typedef  struct {/* engine instance           */
  void*  z; } TT_Engine;   

typedef  struct { /* instance handle type */
  void*  z; } TT_Instance; 

typedef  struct { /* glyph handle type */
  void*  z; } TT_Glyph;    

%disabledoc
%addmethods TT_Engine {

  ~TT_Engine() {
    TT_Done_FreeType(*self);
    free(self);
  }
}

/* %addmethods TT_Instance { */
/*   ~TT_Instance() { */
/*     TT_Done_Instance(*self); */
/* printf("Done Instance\n"); */
/*     free(self); */
/*   } */
/* } */

/* %addmethods TT_Glyph { */
/*   ~TT_Glyph() { */
/*     TT_Done_Glyph(*self); */
/* printf("Done Glyph\n"); */
/*     free(self); */
/*   } */
/* } */
%enabledoc

typedef  struct { /* face handle type */
                  void*  z; } TT_Face;     

/* %addmethods TT_Face { */
/*   ~TT_Face() { */
/*     TT_Close_Face(*self); */
/* printf("Close Face\n"); */
/*     free(self); */
/*   } */
/* } */

typedef  struct { /* character map handle type */
                  void*  z; } TT_CharMap;  

%subsection "Structure Helper Functions"

%text 
%{ Since I cannot get SWIG to meaningfully handle some of the structure
members, I have the following functions to return appropriate values for 
these members.  See the documentation under the various structures for 
the meanings of these members. %}

%subsubsection "TT_Outline Helpers"

%{
  PyObject *getOutlinePoints(TT_Outline *outline) {
    int i;
    PyObject *list;
    PyObject *tmp;

    list = PyList_New((int)(outline->n_points));

    for (i=0; i<outline->n_points; i++) {
      tmp = Py_BuildValue("ll", outline->points[i].x, outline->points[i].y);
      PyList_SetItem(list, i, tmp);
    }

    return list;
  }

  PyObject *getOutlineFlags(TT_Outline *outline) {
    int i;
    PyObject *tmp;
    PyObject *tuple;

    tuple = PyTuple_New((int)(outline->n_points));

    for (i=0; i<(outline->n_points); i++) {
      tmp = PyInt_FromLong((long)(outline->flags[i] % 2));
      PyTuple_SetItem(tuple, i, tmp);
    }

    return tuple;
  }

  PyObject *getOutlineContours(TT_Outline *outline) {
    int i;
    PyObject *tmp;
    PyObject *tuple;

    tuple = PyTuple_New((int)(outline->n_contours));

    for (i=0; i<outline->n_contours; i++) {
      tmp = PyInt_FromLong((long)(outline->contours[i]));
      PyTuple_SetItem(tuple, i, tmp);
    }
    
    return tuple;
  }
%}

PyObject *getOutlinePoints(TT_Outline *outline);
/* Returns the 'points' member as a list of 2-tuples.  Note that this 
   is not a list of TT_Vector instances. */

PyObject *getOutlineFlags(TT_Outline *outline);
/* Returns the 'flags' member as a tuple of length 'n_points'.  Rather
   than representing the actual value that is stored, each element
   of the tuple is either 0 or 1 depending on whether the corresponding
   point is off or on the curve, respectively. */

PyObject *getOutlineContours(TT_Outline *outline);
/* Returns the 'contours' member as a tuple of length 'n_contours'.
   The returned tuple is a direct mapping from the internal C 
   representation to the Python representation. */

%subsubsection "TT_Hdmx Helpers"

%{

PyObject *getHdmxRecords(TT_Hdmx *hdmx) {
  PyObject *retTuple;
  char *str;
  PyObject *string;
  int i;

  str = (char *)calloc(64, sizeof(char));

  retTuple = PyTuple_New(hdmx->num_records);

  for (i=0; i<hdmx->num_records; i++) {
    SWIG_MakePtr(str, (void *)(&hdmx->records[i]), "_TT_Hdmx_Record_p");
    string = PyString_FromString(str);
    PyTuple_SetItem(retTuple, i, string);
  }

  return retTuple;
}

%}

PyObject *getHdmxRecords(TT_Hdmx *hdmx);
/* Returns the 'records' member as a list of TT_Hdmx_Record's. */

%subsection "Raster Map Generation"

%{
  TT_Raster_Map newBitmap(int width, int height) {
    TT_Raster_Map bitmap;

    bitmap.width = width;
    bitmap.cols = (width+7) / 8;
    bitmap.rows = height;
    bitmap.flow = TT_Flow_Down;
    bitmap.size = bitmap.cols * bitmap.rows;
    bitmap.bitmap = malloc(bitmap.size);

    memset(bitmap.bitmap, 0, bitmap.size);
    /* clear the buffer */

    return bitmap;
  }

  TT_Raster_Map newPixmap(int width, int height) {
    TT_Raster_Map pixmap;

    pixmap.width = width;
    pixmap.cols = (width+3) & ~3;
    pixmap.rows = height;
    pixmap.flow = TT_Flow_Down;
    pixmap.size = pixmap.cols * pixmap.rows;
    pixmap.bitmap = malloc(pixmap.size);

    memset(pixmap.bitmap, 0, pixmap.size);
    /* clear the buffer */

    return pixmap;
  }

%}
TT_Raster_Map newBitmap(int width, int height);
/* Create a TT_Raster_Map bitmap structure from the width and height. */

TT_Raster_Map newPixmap(int width, int height);
/* Create a TT_Raster_Map pixmap structure from the width and height. */


%subsection "Flow Constants"
%text %{The flow of a bitmap refers to the way lines are oriented 
within the bitmap data, i.e., the orientation of the Y     
coordinate axis.                                           

For example, if the first bytes of the bitmap pertain to   
its top-most line, then the flow is 'down'.  If these bytes
pertain to its lowest line, the the flow is 'up'. %}

#define TT_Flow_Down  -1  /* bitmap is oriented from top to bottom */
#define TT_Flow_Up     1  /* bitmap is oriented from bottom to top */
#define TT_Flow_Error  0  /* an error occurred during rendering    */

%section "Initialization"

%{
  TT_Engine *PyTT_Init_FreeType() {
    TT_Engine *engine;
    TT_Error error;

    engine = (TT_Engine *)calloc(1, sizeof(TT_Engine));

    error = TT_Init_FreeType(engine);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);

      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    return engine;
  }

  TT_Face *PyTT_Open_Face(TT_Engine      engine,
			  const TT_Text* fontPathName) {
    TT_Face *face;
    TT_Error error;

    face = (TT_Face *)calloc(1, sizeof(TT_Face));

    error = TT_Open_Face(engine, fontPathName, face);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);
	
      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    return face;
  }

  TT_Face *PyTT_Open_Collection(TT_Engine engine,
				const TT_Text* collectionPathName,
				TT_ULong fontIndex) {
    TT_Face *face;
    TT_Error error;

    face = (TT_Face *)calloc(1, sizeof(TT_Face));
    
    error = TT_Open_Collection(engine, collectionPathName, fontIndex, face);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);

      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    return face;
  }

  TT_Face_Properties *PyTT_Get_Face_Properties( TT_Face              face) {
    TT_Face_Properties *properties;
    TT_Error error;

    properties = (TT_Face_Properties *)calloc(1, sizeof(TT_Face_Properties));

    error = TT_Get_Face_Properties(face, properties);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);
	
      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    return properties;
  }

  TT_Instance *PyTT_New_Instance(TT_Face face) {
    TT_Instance *instance;
    TT_Error error;

    instance = (TT_Instance *)calloc(1, sizeof(TT_Instance));

    error = TT_New_Instance(face, instance);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);
	
      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    return instance;
  }

  TT_Instance_Metrics *PyTT_Get_Instance_Metrics(TT_Instance instance) {
    TT_Instance_Metrics *metrics;
    TT_Error error;

    metrics = (TT_Instance_Metrics *)calloc(1, sizeof(TT_Instance_Metrics));

    error = TT_Get_Instance_Metrics(instance, metrics);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);
	
      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    return metrics;
  }

  TT_Glyph *PyTT_New_Glyph(TT_Face face) {
    TT_Glyph *glyph;
    TT_Error error;

    glyph = (TT_Glyph *)calloc(1, sizeof(TT_Glyph));

    error = TT_New_Glyph(face, glyph);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);
	
      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    return glyph;
  }

  TT_Outline *PyTT_Get_Glyph_Outline(TT_Glyph glyph) {
    TT_Outline *outline;
    TT_Error error;

    outline = (TT_Outline *)calloc(1, sizeof(TT_Outline));

    error = TT_Get_Glyph_Outline(glyph, outline);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);
	
      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    return outline;
  }

  TT_Glyph_Metrics *PyTT_Get_Glyph_Metrics(TT_Glyph glyph) {
    TT_Glyph_Metrics *metrics;
    TT_Error error;

    metrics = (TT_Glyph_Metrics *)calloc(1, sizeof(TT_Glyph_Metrics));

    error = TT_Get_Glyph_Metrics(glyph, metrics);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);
      
      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    return metrics;
  }

  TT_Big_Glyph_Metrics *PyTT_Get_Glyph_Big_Metrics(TT_Glyph glyph) {
    TT_Big_Glyph_Metrics *metrics;
    TT_Error error;

    metrics = (TT_Big_Glyph_Metrics *)calloc(1, sizeof(TT_Big_Glyph_Metrics));

    error = TT_Get_Glyph_Big_Metrics(glyph, metrics);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);

      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    return metrics;
  }

  TT_Outline *PyTT_New_Outline( TT_UShort    numPoints,
				TT_Short     numContours) {
    TT_Outline *outline;
    TT_Error error;

    outline = (TT_Outline *)calloc(1, sizeof(TT_Outline));

    error = TT_New_Outline(numPoints, numContours, outline);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);

      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    return outline;
  }

  TT_Outline *PyTT_Copy_Outline(TT_Outline *source) {
    TT_Outline *target;
    TT_Error error;

    target = (TT_Outline *)calloc(1, sizeof(TT_Outline));

    error = TT_New_Outline(source->n_points, source->n_contours, target);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);

      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    error = TT_Copy_Outline(source, target);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);

      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    return target;
  }

  TT_BBox *PyTT_Get_Outline_BBox(TT_Outline *outline) {
    TT_BBox *bbox;
    TT_Error error;

    bbox = (TT_BBox *)calloc(1, sizeof(TT_BBox));

    error = TT_Get_Outline_BBox(outline, bbox);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);

      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    return bbox;
  }

  TT_CharMap *PyTT_Get_CharMap(TT_Face face, TT_UShort charmapIndex) {
    TT_CharMap *charMap;
    TT_Error error;

    charMap = (TT_CharMap *)calloc(1, sizeof(TT_CharMap));

    error = TT_Get_CharMap(face, charmapIndex, charMap);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);

      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    return charMap;
  }

  TT_Kerning *PyTT_Get_Kerning_Directory(TT_Face face) {
    TT_Kerning *directory;
    TT_Error error;
      
    directory = (TT_Kerning *)calloc(1, sizeof(TT_Kerning));

    error = TT_Get_Kerning_Directory(face, directory);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);

      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    return directory;
  }

  TT_Post *PyTT_Load_PS_Names(TT_Face face) {
    TT_Post *post;
    TT_Error error;

    post = (TT_Post *)calloc(1, sizeof(TT_Post));

    error = TT_Load_PS_Names(face, post);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);

      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    return post;
  }

%}

%new %name(TT_Init_FreeType) TT_Engine *PyTT_Init_FreeType();
  /* Initialize the engine. 

     Python Note:
       Returns a TT_Engine.  You do not need to pass this function
       a pointer like you do in C. */
  
TT_Error TT_Done_FreeType(TT_Engine engine);

%typemap(python,in) TT_Byte*   palette {
  if (!PySequence_Check($source)) {
    PyErr_SetString(PyExc_TypeError, "palette must be a sequence of integers");
    return NULL;
  }
  else if (PyObject_Length($source) != 5) {
    PyErr_SetString(PyExc_ValueError, "palette must have five values");
    return NULL;
  }
  else {
    int i;
    TT_Byte *tmp_array;
    PyObject *tmp;

    tmp_array = (TT_Byte *)calloc(5, sizeof(TT_Byte));

    for (i=0; i<5; i++) {
      tmp = PySequence_GetItem($source, i);
      if (!PyInt_Check(tmp)) {
	PyErr_SetString(PyExc_ValueError, "palette must be a sequence of integers");
	return NULL;
      }
      tmp_array[i] = (unsigned char)(PyInt_AsLong(tmp));
      Py_DECREF(tmp);
    }
    $target = tmp_array;
  }
}
   
  TT_Error  TT_Set_Raster_Gray_Palette( TT_Engine  engine,
                                        TT_Byte*   palette );
/* Set the gray level palette.  This is an array of 5 bytes used
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
*/

%section "Face Management"

%name(TT_Open_Face) TT_Face *PyTT_Open_Face(TT_Engine      engine,
					    const TT_Text* fontPathName);
/* Open a new TrueType font file, and returns a handle for
   it.

   Note: The file can be either a TrueType file (*.ttf) or 
         a TrueType collection (*.ttc, in this case, only  
         the first face is opened).  The number of faces in
         the same collection can be obtained in the face's 
         properties, using TT_Get_Face_Properties() and the
         'max_Faces' field. 
     
   Python Note:
     Returns a TT_Face.  You do not need to pass this function
     a pointer like you do in C.*/

%name(TT_Open_Collection) TT_Face *PyTT_Open_Collection(TT_Engine engine, const TT_Text*  collectionPathName, TT_ULong fontIndex);
/* Open a TrueType font file located inside a collection.
   The font is assigned by its index in 'fontIndex'. 

   Python Note:
     Returns a TT_Face.  You do not need to pass this function
     a pointer like you do in C.*/

%name(TT_Get_Face_Properties) TT_Face_Properties *PyTT_Get_Face_Properties( TT_Face face);
/* Return face properties.

   Python Note:
     Returns a TT_Face_Properties.  You do not need to pass this function
     a pointer like you do in C.
*/

  TT_Error  TT_Flush_Face( TT_Face  face );
/* Close a face's file handle to save system resources.  The file
   will be re-opened automatically on the next disk access. */


%{
  PyObject *PyTT_Get_Face_Metrics(TT_Face face, 
		TT_UShort   firstGlyph,
		TT_UShort   lastGlyph) {
    TT_Face_Properties props;
    TT_Short *leftBearings;
    TT_UShort *widths;
    TT_Short *topBearings;
    TT_UShort *heights;
    TT_Error error;
    int i;
    int length;
    PyObject *retTuple;
    PyObject *leftBTuple;
    PyObject *widthsTuple;
    PyObject *topBTuple;
    PyObject *heightsTuple;
    PyObject *tmp;

    length = (int)(lastGlyph - firstGlyph + 1);

    error = TT_Get_Face_Properties(face, &props);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);

      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    leftBearings = (TT_Short *)calloc(length, sizeof(TT_Short));
    widths = (TT_UShort *)calloc(length, sizeof(TT_UShort));

    if (props.vertical != NULL) {
      topBearings = (TT_Short *)calloc(length, sizeof(TT_Short));
      heights = (TT_UShort *)calloc(length, sizeof(TT_UShort));
    }
    else {
      topBearings = NULL;
      heights = NULL;
    }

    error = TT_Get_Face_Metrics(face, firstGlyph, lastGlyph, leftBearings, widths, topBearings, heights);
    if (error) {
      char *errstring;
   
      errstring = TT_ErrToString18(error);

      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    leftBTuple = PyTuple_New(length);
    for (i=0; i<length; i++) {
      tmp = PyInt_FromLong((long)leftBearings[i]);
      PyTuple_SetItem(leftBTuple, i, tmp);
    }

    free(leftBearings);

    widthsTuple = PyTuple_New(length);
    for (i=0; i<length; i++) {
      tmp = PyInt_FromLong((long)widths[i]);
      PyTuple_SetItem(widthsTuple, i, tmp);
    }

    free(widths);

    if (props.vertical != NULL) {
      for (i=0; i<length; i++) {
        tmp = PyInt_FromLong((long)topBearings[i]);
        PyTuple_SetItem(topBTuple, i, tmp);
      }

      free(topBearings);

      for (i=0; i<length; i++) {
        tmp = PyInt_FromLong((long)heights[i]);
        PyTuple_SetItem(heightsTuple, i, tmp);
      }

      free(heights);

    }
    else {
      topBTuple = Py_None;
      Py_INCREF(Py_None);

      heightsTuple = Py_None;
      Py_INCREF(Py_None);
    }

    retTuple = Py_BuildValue("OOOO", leftBTuple, widthsTuple, topBTuple, heightsTuple);

    return retTuple;
}

%}

%name(TT_Get_Face_Metrics) PyObject *PyTT_Get_Face_Metrics(TT_Face face, TT_UShort firstGlyph, TT_UShort lastGlyph);
/* Get a face's glyph metrics expressed in font units.

   Python Note:
     Returns the arrays as a tuple:

       leftBearings, widths, topBearings, heights = TT_Get_Face_Metrics(face, 
                                                                        firstGlyph, 
									lastGlyph)

     Values of 'None' mean that there are no vertical metrics in the face. */

  TT_Error  TT_Close_Face( TT_Face  face );
/* Close a given font object, destroying all associated instances. */

%{
  PyObject *PyTT_Get_Font_Data( TT_Face face,
				char table[4],
				TT_Long offset) {
    char *buffer;
    TT_Long length;
    TT_ULong tag;

    PyObject *string;
    TT_Error error;

    tag = MAKE_TT_TAG(table[0], table[1], table[2], table[3]);

    error = TT_Get_Font_Data(face, tag, offset, (void *)buffer, &length);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);

      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }
    
    string = PyString_FromStringAndSize(buffer, length);

    return string;
  }   
%}

%name(TT_Get_Font_Data) PyObject *PyTT_Get_Font_Data( TT_Face   face,
						      char table[4],
						      TT_Long   offset);
/* Get font or table data. 

   Python Note:
     The Python function differs from the C function in the following ways:

       1. Instead of a numerical 'tag', the Python function takes the table
          name as a string.  I.e. to get the data from the 'eblc' table, 
	  you simply pass the string 'eblc' as the second argument rather
	  than MAKE_TT_TAG('e', 'b', 'l', 'c').

       2. The buffer is returned to you as a string.  What you can do with
          it, I have no idea, but I am sure that someone will find a clever
	  trick. */

%section "Instance Management"

%name(TT_New_Instance) TT_Instance *PyTT_New_Instance(TT_Face face);
/* Open a new font instance and returns an instance handle
   for it.

   Python Note:
     Returns a TT_Instance.  You do not need to pass this function
     a pointer like you do in C.
*/

  TT_Error  TT_Set_Instance_Resolutions( TT_Instance  instance,
                                         TT_UShort    xResolution,
                                         TT_UShort    yResolution );
/* Set device resolution for a given instance.  The values are
   given in dpi (Dots Per Inch).  Default is 96 in both directions. */

  TT_Error  TT_Set_Instance_CharSize( TT_Instance  instance,
                                      TT_F26Dot6   charSize );
/* Set the pointsize (in 1/64 points) for a given instance.  Default is 10pt. 

   charSize = pointSize * 64
   e.g. for a 12pt fontsize, charSize = 12 * 64 = 768
*/
  
  TT_Error  TT_Set_Instance_CharSizes( TT_Instance  instance,
                                       TT_F26Dot6   charWidth,
                                       TT_F26Dot6   charHeight );

%{
#undef TT_Set_Instance_PointSize
  TT_Error TT_Set_Instance_PointSize( TT_Instance instance, 
				      TT_F26Dot6 pointSize ) {
    return TT_Set_Instance_CharSize(instance, (TT_F26Dot6)(pointSize*64));
  }
%}
  TT_Error TT_Set_Instance_PointSize( TT_Instance instance, 
				      TT_F26Dot6 ptsize );
/* Set the pointsize (in points) for a given instance.  Default is 10pt. */

  TT_Error  TT_Set_Instance_PixelSizes( TT_Instance  instance,
                                        TT_UShort    pixelWidth,
                                        TT_UShort    pixelHeight,
                                        TT_F26Dot6   pointSize );

%name(TT_Get_Instance_Metrics) TT_Instance_Metrics *PyTT_Get_Instance_Metrics(TT_Instance instance);
/* Return instance metrics. 

   Python Note:
     Returns a TT_Instance_Metrics.  You do not need to pass this function
     a pointer like you do in C. */

  TT_Error  TT_Done_Instance( TT_Instance  instance );

%section "Glyph Management"

%name(TT_New_Glyph) TT_Glyph *PyTT_New_Glyph(TT_Face face);
/* Create a new glyph object related to the given 'face'. 

   Python Note:
     Returns a TT_Glyph.  You do not need to pass this function
     a pointer like you do in C. */

  TT_Error  TT_Done_Glyph( TT_Glyph  glyph );

#define TTLOAD_SCALE_GLYPH  1
#define TTLOAD_HINT_GLYPH   2

#define TTLOAD_DEFAULT  3

  TT_Error  TT_Load_Glyph( TT_Instance  instance,
                           TT_Glyph     glyph,
                           TT_UShort    glyphIndex,
                           TT_UShort    loadFlags );
/* Load and process (scale/transform and hint) a glyph from the
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
          the font file (given in the face properties). */

%name(TT_Get_Glyph_Outline) TT_Outline *PyTT_Get_Glyph_Outline(TT_Glyph glyph);
/* Return glyph outline pointers in 'outline'.  Note that the returned
   pointers are owned by the glyph object, and will be destroyed with 
   it.  The client application should _not_ change the pointers. 

   Python Note:
     Returns a TT_Outline.  You do not need to pass this function
     a pointer like you do in C. */

%name(TT_Get_Glyph_Metrics) TT_Glyph_Metrics *PyTT_Get_Glyph_Metrics(TT_Glyph glyph);
/* Copy the glyph metrics into 'metrics'. 

   Python Note:
     Returns a TT_Glyph_Metrics.  You do not need to pass this function
     a pointer like you do in C. */

%name(TT_Get_Glyph_Big_Metrics) TT_Big_Glyph_Metrics *PyTT_Get_Glyph_Big_Metrics(TT_Glyph glyph);
/* Copy the glyph's big metrics into 'metrics'.
   Necessary to obtain vertical metrics. 

   Python Note:
     Returns a TT_Big_Glyph_Metrics.  You do not need to pass this function
     a pointer like you do in C. */

  TT_Error  TT_Get_Glyph_Bitmap( TT_Glyph        glyph,
                                 TT_Raster_Map*  map,
                                 TT_F26Dot6      xOffset,
                                 TT_F26Dot6      yOffset );
/* Render the glyph into a bitmap, with given position offsets.    

   Note:  Only use integer pixel offsets to preserve the fine     
          hinting of the glyph and the 'correct' anti-aliasing    
          (where vertical and horizontal stems aren't grayed).    
          This means that xOffset and yOffset must be multiples   
          of 64! */

  TT_Error  TT_Get_Glyph_Pixmap( TT_Glyph        glyph,
                                 TT_Raster_Map*  map,
                                 TT_F26Dot6      xOffset,
                                 TT_F26Dot6      yOffset );
/* Render the glyph into a pixmap, with given position offsets.    

   Note: Only use integer pixel offsets to preserve the fine     
         hinting of the glyph and the 'correct' anti-aliasing    
         (where vertical and horizontal stems aren't grayed).    
         This means that xOffset and yOffset must be multiples   
         of 64! */

%section "Outline Support"

%name(TT_New_Outline) TT_Outline *PyTT_New_Outline(TT_UShort    numPoints,
						   TT_Short     numContours);
/* Allocate a new outline.  Reserve space for 'numPoints' and 'numContours'. 

   Python Note:
     Returns a TT_Outline.  You do not need to pass this function
     a pointer like you do in C. */

  TT_Error  TT_Done_Outline( TT_Outline*  outline );

%name(TT_Copy_Outline) TT_Outline *PyTT_Copy_Outline(TT_Outline *source);
/* Copy an outline to another one. 

   Python Note:
     Returns a TT_Outline.  You do not need to pass this function
     a pointer like you do in C. */

  TT_Error  TT_Get_Outline_Bitmap( TT_Engine       engine,
                                   TT_Outline*     outline,
                                   TT_Raster_Map*  map );
/* Render an outline into a bitmap. */

  TT_Error  TT_Get_Outline_Pixmap( TT_Engine       engine,
                                   TT_Outline*     outline,
                                   TT_Raster_Map*  map );
/* Render an outline into a pixmap -- note that this function uses
   a different pixel scale, where 1.0 pixels = 128 */

%name(TT_Get_Outline_BBox) TT_BBox *PyTT_Get_Outline_BBox(TT_Outline *outline);
/* Return an outline's bounding box -- this function is slow as it
   performs a complete scan-line process, without drawing, to get  
   the most accurate values. 

   Python Note:
     Returns a TT_BBox.  You do not need to pass this function
     a pointer like you do in C. */
  
  void      TT_Transform_Outline( TT_Outline*  outline,
                                  TT_Matrix*   matrix );
/* Apply a transformation to a glyph outline. */

  void      TT_Translate_Outline( TT_Outline*  outline,
                                  TT_F26Dot6   xOffset,
                                  TT_F26Dot6   yOffset );
/* Apply a translation to a glyph outline. */

%apply long *IN_OUT {TT_F26Dot6* x, TT_F26Dot6* y};
  
  void      TT_Transform_Vector( TT_F26Dot6*  x,
                                 TT_F26Dot6*  y,
                                 TT_Matrix*   matrix );
/* Apply a transformation to a vector. 

   Python Note:
     
     x, y = TT_Transform_Vector(x, y, matrix)
*/
  
  TT_Long   TT_MulDiv( TT_Long A, TT_Long B, TT_Long C );
/* Compute A*B/C with 64 bits intermediate precision. */

  TT_Long   TT_MulFix( TT_Long A, TT_Long B );
/* Compute A*B/0x10000 with 64 bits intermediate precision. 
   Useful to multiply by a 16.16 fixed float value. */

%section "Character Mapping Support"

%apply unsigned short *OUTPUT {TT_UShort* platformID, TT_UShort* encodingID};
  
  TT_Error  TT_Get_CharMap_ID( TT_Face     face,
                               TT_UShort   charmapIndex,
                               TT_UShort*  platformID,
                               TT_UShort*  encodingID );
/* Return the ID of charmap number 'charmapIndex' of a given face
   used to enumerate the charmaps present in a TrueType file. 

   Common character mappings:
     platformID encodingID : Mapping Name
     0          0          : Apple Unicode
     1          0          : Apple Roman
     3          0          : Windows Symbol
     3          1          : Windows Unicode

   Python Note:
   
     platformID, encodingID = TT_Get_CharMap_ID(face, charmapIndex)
*/

%name(TT_Get_CharMap) TT_CharMap *PyTT_Get_CharMap(TT_Face face, TT_UShort charmapIndex);
/* Look up the character maps found in 'face' and return a handle
   for the one matching 'platformID' and 'platformEncodingID'    
   (see the TrueType specs relating to the 'cmap' table for      
   information on these ID numbers). 

   Python Note:
     Returns a TT_CharMap.  You do not need to pass this function
     a pointer like you do in C. */

%typemap(python,out) PyObject * {
  $target = $source;
}

%typemap(python,in) PyObject * {
  $target = $source;
}

%{
  PyObject *PyTT_String_Indices( TT_CharMap charMap,
		                 PyObject *inputString) {
    PyObject *retTuple;
    PyObject *tmp;
    char *string;
    int length;
    int i;

    if (!PyString_Check(inputString)) {
      PyErr_SetString(PyExc_TypeError, "need a string");
      return NULL;
    }
    
    string = PyString_AsString(inputString);
    length = PyObject_Length(inputString);

    retTuple = PyTuple_New(length);

    for (i=0; i<length; i++) {
      tmp = PyInt_FromLong((long)TT_Char_Index(charMap, (TT_UShort)(string[i])));
      PyTuple_SetItem(retTuple, i, tmp);
    }

    return retTuple;
  }

  PyObject *PyTT_Char_Index( TT_CharMap charMap,
			     PyObject *inputChar) {
    PyObject *retInt;
    char *string;
    
    if (!PyString_Check(inputChar) || PyObject_Length(inputChar) != 1) {
      PyErr_SetString(PyExc_ValueError, "char expected");
      return NULL;
    }

    string = PyString_AsString(inputChar);

    return PyInt_FromLong((long)TT_Char_Index(charMap, (TT_UShort)(string[0])));
  }
%}
PyObject *PyTT_Char_Index( TT_CharMap charMap,
			   PyObject *inputChar);
/* Return the index of the character inputChar in the character map. */

PyObject *PyTT_String_Indices( TT_CharMap charMap,
			       PyObject *inputString);
/* Return the indices of the characters in inputString as a tuple.

   E.g.

     a, b, c = PyTT_String_Indices(charMap, 'abc')
*/

%typemap(python,out) TT_UShort {
  if (!$source) {
    PyErr_SetString(FreeTypeError, "cannot load character index");
    return NULL;
  }
  else {
    $target = Py_None;
    Py_INCREF(Py_None);
  }
}

  TT_UShort  TT_Char_Index( TT_CharMap  charMap,
                            TT_UShort   charCode );
/* Translate a character code through a given character map
   and return the corresponding glyph index to be used in     
   a TT_Load_Glyph call.

   Python Note:
     For the Windows Unicode mapping (3,1), 

       charCode = ord(character)

       (I think)
*/

%typemap(python,out) TT_UShort;

%section "Names Table Support"

%apply unsigned short *OUTPUT {TT_UShort*  platformID, TT_UShort*  encodingID, TT_UShort*  languageID, TT_UShort*  nameID};

  TT_Error  TT_Get_Name_ID( TT_Face     face,
                            TT_UShort   nameIndex,
                            TT_UShort*  platformID,
                            TT_UShort*  encodingID,
                            TT_UShort*  languageID,
                            TT_UShort*  nameID );
/* Return the ID of the name number 'nameIndex' of a given face 
   used to enumerate the charmaps present in a TrueType file. 

   Python Note:
   
     platformID, encodingID, languageID, nameID = TT_Get_Name_ID(face, nameIndex)
*/

%typemap(python,argout) char** stringPtr {
  PyObject *o;

  if ($source == NULL) {
    PyErr_SetString(FreeTypeError, "cannot get name");
    return NULL;
  }

  o = PyString_FromString(*$source);
  $target = t_output_helper($target, o);
}

%typemap(python,ignore) char** stringPtr {
  char *temp;
  $target = &temp;
}

%apply unsigned short *OUTPUT {TT_UShort* length};
  
  TT_Error  TT_Get_Name_String( TT_Face      face,
                                TT_UShort    nameIndex,
                                char**  stringPtr,
                                TT_UShort*   length );
/* Return the address and length of the name number 'nameIndex'
   of a given face.  The string is part of the face object and  
   shouldn't be written to or released.                         

   Note that if for an invalid platformID a null pointer will   
   be returned. */

%section "FreeType Extensions"
%subsection "Kerning"
%subsubsection "Structures"

typedef struct
  {/* format 0 kerning pair */

    TT_UShort  left;   /* index of left  glyph in pair */
    TT_UShort  right;  /* index of right glyph in pair */
    TT_FWord   value;  /* kerning value                */
  } TT_Kern_0_Pair;

typedef struct 
  {/* format 0 kerning subtable */

    TT_UShort  nPairs;          /* number of kerning pairs */

    TT_UShort  searchRange;     
    TT_UShort  entrySelector;  
    TT_UShort  rangeShift;

    TT_Kern_0_Pair*  pairs;     /* a table of nPairs `pairs' */
  } TT_Kern_0;

typedef struct
  {/* format 2 kerning glyph class */

    TT_UShort   firstGlyph;   /* first glyph in range                    */
    TT_UShort   nGlyphs;      /* number of glyphs in range               */
    TT_UShort*  classes;      
/* a table giving for each ranged glyph its class offset 
   in the subtable pairs two-dimensional array */
  } TT_Kern_2_Class;

typedef struct 
  {/* format 2 kerning subtable */

    TT_UShort        rowWidth;   /* length of one row in bytes         */
    TT_Kern_2_Class  leftClass;  /* left class table                   */
    TT_Kern_2_Class  rightClass; /* right class table                  */
    TT_FWord*        array;      /* 2-dimensional kerning values array */
  } TT_Kern_2;

typedef struct 
  {/* kerning subtable */

    TT_Bool    loaded;   /* boolean; indicates whether the table is   
                            loaded                                    */
    TT_UShort  version;  /* table version number                      */
    TT_Long    offset;   /* file offset of table                      */
    TT_UShort  length;   /* length of table, _excluding_ header       */
    TT_Byte    coverage; /* lower 8 bit of the coverage table entry   */
    TT_Byte    format;   
   /* the subtable format, as found in the higher 8 bits of the coverage table entry */
    union
    {
      TT_Kern_0  kern0;
      TT_Kern_2  kern2;
    } t;
  } TT_Kern_Subtable;

typedef struct 
  {
    TT_UShort          version;  /* kern table version number. starts at 0 */
    TT_UShort          nTables;  /* number of tables                       */

    TT_Kern_Subtable*  tables;   /* the kerning sub-tables                 */
  } TT_Kerning;

%subsubsection "Functions"

  TT_Error  TT_Init_Kerning_Extension( TT_Engine  engine );
/* Initialize Kerning extension, must be called after                
   TT_Init_FreeType(). There is no need for a finalizer.

   Note on the implemented mechanism:                                

     The kerning table directory is loaded with the face through the    
     extension constructor.  However, the tables will only be loaded    
     on demand, as they may represent a lot of data, unnecessary to     
     most applications. */                     

%name(TT_Get_Kerning_Directory) TT_Kerning *PyTT_Get_Kerning_Directory(TT_Face face);
/* Queries a pointer to the kerning directory for the face object 

   Python Note:
     Returns a TT_Kerning.  You do not need to pass this function
     a pointer like you do in C. */

  TT_Error  TT_Load_Kerning_Table( TT_Face    face,
                                   TT_UShort  kern_index );
/* Load the kerning table number `kern_index' in the kerning         
   directory.  The table will stay in memory until the `face'         
   face is destroyed. */


%subsection "cmap Iteration"

%typemap(python,out) TT_Long {
  if ($source == -1) {
    PyErr_SetString(FreeTypeError, "cannot get character code");
    return NULL;
  }
  else {
    $target = Py_BuildValue("l",$source);
  }
}

%apply unsigned short *OUTPUT {TT_UShort*  id};

  TT_Long  TT_CharMap_First( TT_CharMap  charMap,
                             TT_UShort*  id );
/* Find the first entry of a Cmap.

   Python Note:
     The return value is a 2-tuple of the first valid character
     code in charMap and the first glyph index.
 */

  TT_Long  TT_CharMap_Next( TT_CharMap  charMap,
                            TT_UShort   startId,
                            TT_UShort*  id );
/* Find the next entry of Cmap.  Same return conventions. 

   Python Note:
     The return value is a 2-tuple of the next valid character
     code in charMap and its corresponding glyph index.
*/

  TT_Long  TT_CharMap_Last( TT_CharMap  charMap,
                            TT_UShort*  id );
/* Find the last entry of a Cmap.

   Python Note:
     The return value is a 2-tuple of the last valid character
     code in charMap and the last glyph index.
*/

%typemap(python,out) TT_Long;

%subsection "gasp Extension"

%typemap(python,ignore) TT_Bool *OUTPUT = unsigned short *OUTPUT;
%typemap(python,argout) TT_Bool *OUTPUT = unsigned short *OUTPUT;

%apply TT_Bool *OUTPUT {TT_Bool*  grid_fit, TT_Bool*  smooth_font};

  TT_Error  TT_Get_Face_Gasp_Flags( TT_Face    face,
                                    TT_UShort  point_size,
                                    TT_Bool*   grid_fit,
                                    TT_Bool*   smooth_font );
/* This function returns for a given 'point_size' the values of the 
   gasp flags 'grid_fit' and 'smooth_font'.  The returned values    
   are booleans (where 0 = NO, and 1 = YES).                        

   Note that this function will return TT_Err_Table_Missing if      
   the font file doesn't contain any gasp table. 

   Python Note:

     grid_fit, smooth_font = TT_Get_Face_Gasp_Flags(face, point_size)

*/

%subsection "PostScript Glyph Names"
%subsubsection "Structures"

typedef struct
  {/* format 2.0 table */

    TT_UShort   numGlyphs;
    TT_UShort*  glyphNameIndex;
    TT_Char**   glyphNames;
  } TT_Post_20;

typedef struct
  {
    TT_UShort  numGlyphs;
    TT_Char*   offset;
  } TT_Post_25;

typedef struct
  {
/* This structure holds the information needed by TT_Get_PS_Name.
   Although, it is not passed as an argument in that function and 
   one should access it's contents directly, I believe that 
   it is necessary to have it available in memory when TT_Get_PS_Name
   is called.  Therefore, be sure to create one with TT_Load_PS_Names
   and don't let it be deleted until you are finished. */

    TT_Long  offset;
    TT_Long  length;
    TT_Bool  loaded;

    union
    {
      TT_Post_20  post20;
      TT_Post_25  post25;
    } p;
  } TT_Post;

%subsubsection "Functions"

  TT_Error TT_Init_Post_Extension( TT_Engine  engine );

%name(TT_Load_PS_Names) TT_Post *PyTT_Load_PS_Names(TT_Face face);
/* Python Note:
     Returns a TT_Post.  You do not need to pass this function
     a pointer like you do in C. */

%apply char** stringPtr {TT_String**  PSname};

  TT_Error TT_Get_PS_Name( TT_Face      face,
                           TT_UShort    index,
                           TT_String**  PSname );

%subsection "Glyph Widths"

%{
  PyObject *PyTT_Get_Face_Widths(TT_Face face, TT_UShort first_glyph, TT_UShort last_glyph) {
    TT_Face_Properties props;
    TT_Error error;
    TT_UShort *widths;
    TT_UShort *heights;
    int i;
    int length;
    PyObject *retTuple;
    PyObject *widthsObj;
    PyObject *heightsObj;
    PyObject *tmp;

    error = TT_Get_Face_Properties(face, &props);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);

      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    length = (int)(last_glyph - first_glyph + 1);

    widths = (TT_UShort *)calloc(length, sizeof(TT_UShort));

    if (props.vertical != NULL) {
      heights = (TT_UShort *)calloc(length, sizeof(TT_UShort));
    }
    else {
      heights = NULL;
    }

    error = TT_Get_Face_Widths(face, first_glyph, last_glyph, widths, heights);
    if (error) {
      char *errstring;

      errstring = TT_ErrToString18(error);

      PyErr_SetString(FreeTypeError, errstring);
      return NULL;
    }

    widthsObj = PyTuple_New(length);
    for (i=0; i<length; i++) {
      tmp = PyInt_FromLong((long)widths[i]);
      PyTuple_SetItem(widthsObj, i, tmp);
    }

    free(widths);

    if (props.vertical != NULL) {
      heightsObj = PyTuple_New(length);
      for (i=0; i<length; i++) {
        tmp = PyInt_FromLong((long)heights[i]);
        PyTuple_SetItem(heightsObj, i, tmp);
      }

      free(heights);
    
    }
    else {
      heightsObj = Py_None;
      Py_INCREF(Py_None);
    }

    retTuple = Py_BuildValue("OO", widthsObj, heightsObj);

    return retTuple;
  }
%}

%name(TT_Get_Face_Widths) PyObject *PyTT_Get_Face_Widths(TT_Face face, TT_UShort first_glyph, TT_UShort last_glyph);
/* Description: Returns the widths and/or heights of a given  
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
     in the font file. */

%include malloc.i
%include memory.i
%include pointer.i

%section "Credits"
%text %{
I would like to thank the FreeType project, especially its principal authors,
David Turner, Robert Wilhelm, and Werner Lemberg, for writing such a wonderful 
library to wrap.

Other notable persons/entities include

  * Fred L. Drake, Jr. for letting me peek at (read: steal from) his t1python code
    for the bitmap-to-PIL conversion.

  * Dave Beazley for SWIG, without which I would never had had the activation
    energy to start this project.

  * Guido and the rest of the Python gang for making a damn fine programming language.
%}

%section "Legal"
%text %{
Copyright 1999 by Robert Kern, Frederick MD, USA.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of Robert Kern not be used in advertising 
or publicity pertaining to distribution of the software without specific, 
written prior permission. 

ROBERT KERN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING 
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL 
ROBERT KERN BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES 
OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
%}

%init %{
  FreeTypeError = PyErr_NewException("freetype.error", NULL, NULL);
%}

%pragma(python) 
include="freetype_inc.py"
