#include "engine/Engine.h"

using namespace Game;

PixelFormat::PixelFormat(int _bitsPerPixel):
	bitsPerPixel(_bitsPerPixel),
	rmask(0),
	rshift(0),
	gmask(0),
	gshift(0),
	bmask(0),
	bshift(0)
{
	bytesPerPixel = ((_bitsPerPixel-1)>>3)+1;

	switch(bitsPerPixel)
	{
	case 32:
		rmask = 0xff0000;
		gmask = 0x00ff00;
		bmask = 0x0000ff;
		rshift = 16;
		gshift = 8;
		bshift = 0;
		rsize = 8;
		gsize = 8;
		bsize = 8;
	break;
	case 16:
		rmask = 0xf00;
		gmask = 0x0f0;
		bmask = 0x00f;
		rshift = 8;
		gshift = 4;
		bshift = 0;
		rsize = 4;
		gsize = 4;
		bsize = 4;
	break;
	case 12:
		rmask = 0xf00;
		gmask = 0x0f0;
		bmask = 0x00f;
		rshift = 8;
		gshift = 4;
		bshift = 0;
		rsize = 4;
		gsize = 4;
		bsize = 4;
	break;
	}
}

#define BITCOUNT(x) ((x)==0)?0:((x)==1)?1:((x)==3)?2:((x)==7)?3:((x)==15)?4:((x)==31)?5:((x)==63)?6:((x)==127)?7:8

PixelFormat::PixelFormat(int _bitsPerPixel, int rm, int rs, int gm, int gs, int bm, int bs):
	bitsPerPixel(_bitsPerPixel),
	bytesPerPixel(((_bitsPerPixel-1)>>3)+1),
	rmask(rm),
	rshift(rs),
	gmask(gm),
	gshift(gs),
	bmask(bm),
	bshift(bs)
{
	rsize = BITCOUNT(rmask>>rshift);
	gsize = BITCOUNT(gmask>>gshift);
	bsize = BITCOUNT(bmask>>bshift);
}

Surface::Surface(PixelFormat* _format, Pixel* _pixels, int _width, int _height):
	format(*_format),
	pixels(_pixels),
	width(_width),
	height(_height),
	autoDelete(false)
{
	bytes = width * height * format.bytesPerPixel;
}

Surface::Surface(PixelFormat* _format, int _width, int _height):
	format(*_format),
	pixels(0),
	width(_width),
	height(_height),
	autoDelete(true)
{
	bytes = width * height * format.bytesPerPixel;
	pixels = new Pixel[bytes / sizeof(Pixel)];
	clear();
}

Surface::Surface(PixelFormat* _format, Surface* s):
	format(*_format),
	pixels(0),
	width(s->width),
	height(s->height),
	autoDelete(true)
{
	int x, y;
	bytes = width * height * format.bytesPerPixel;
	pixels = new Pixel[bytes / sizeof(Pixel)];
	
	for(y=0; y<height; y++)
	for(x=0; x<width; x++)
	{
		Pixel8 r,g,b;
		s->format.makeRGB(s->getPixel(x,y),&r,&g,&b);
		setPixel(x,y,format.makePixel(r,g,b));
	}
}

Surface::~Surface()
{
	if (autoDelete)
	{
		delete[] pixels;
	}
	pixels = 0;
}

void Surface::clear(Pixel color)
{
	switch(format.bytesPerPixel)
	{
	case 1:
	{
		Pixel8 *p = (Pixel8*)pixels;
		int l = bytes;
		
		while(l--)
			*p++ = color;
	}
	break;
	case 2:
	{
		Pixel16 *p = (Pixel16*)pixels;
		int l = bytes>>1;
		
		while(l--)
			*p++ = color;
	}
	break;
	case 4:
	{
		Pixel32 *p = (Pixel32*)pixels;
		int l = bytes>>2;
		
		while(l--)
			*p++ = color;
	}
	break;
	}
}

Pixel Surface::getPixel(int x, int y)
{
	switch(format.bytesPerPixel)
	{
	case 1:
	{
		Pixel8 *p = (Pixel8*)pixels;
		return p[x + y*width];
	}
	break;
	case 2:
	{
		Pixel16 *p = (Pixel16*)pixels;
		return p[x + y*width];
	}
	break;
	case 4:
	{
		Pixel32 *p = (Pixel32*)pixels;
		return p[x + y*width];
	}
	break;
	}
	return 0;
}

void Surface::setPixel(int x, int y, Pixel color)
{
	switch(format.bytesPerPixel)
	{
	case 1:
	{
		Pixel8 *p = (Pixel8*)pixels;
		p[x + y*width] = color;
	}
	break;
	case 2:
	{
		Pixel16 *p = (Pixel16*)pixels;
		p[x + y*width] = color;
	}
	break;
	case 4:
	{
		Pixel32 *p = (Pixel32*)pixels;
		p[x + y*width] = color;
	}
	break;
	}
}

