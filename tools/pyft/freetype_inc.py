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
