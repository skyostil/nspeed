/***************************************************************************
 *   Copyright (C) 2004 by Sami Kyöstilä                                   *
 *   skyostil@kempele.fi                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
#include "BitmapFont.h"

BitmapFont::BitmapFont(Game::Surface *t):
	glyph(0),
	minGlyph(32),
	maxGlyph(128),
	texture(t)
{
	build(t);
}

BitmapFont::~BitmapFont()
{
	clear();
}

void BitmapFont::build(Game::Surface *t)
{
	int i, x = 0, maxWidth = 0;
	
	clear();
	
	glyph = new Glyph[maxGlyph-minGlyph];
	
	for(i=1; i<maxGlyph-minGlyph; i++)
	{
		int w;
		
		for(w=1; x+w < t->width; w++)
		{
			if (t->getPixel(x+w, 0))
				break;
		}
		
		glyph[i].width = w;
		glyph[i].height = t->height;
		
		if (w > maxWidth)
			maxWidth = w;
		
		switch(t->format.bytesPerPixel)
		{
		case 1:
			glyph[i].pixels = (Game::Pixel*)(((Game::Pixel8*)t->pixels) + x);
		break;
		case 2:
			glyph[i].pixels = (Game::Pixel*)(((Game::Pixel16*)t->pixels) + x);
		break;
		case 4:
			glyph[i].pixels = (Game::Pixel*)(((Game::Pixel32*)t->pixels) + x);
		break;
		}
		x += w+1;
	}
	
	// set the space character
	glyph[0].pixels = 0;
	glyph[0].width = maxWidth;
	glyph[0].height = t->height;
}

void BitmapFont::clear()
{
	if (glyph)
		delete[] glyph;
}

template<typename Pixel>
void BitmapFont::renderGlyph(Game::Surface *screen, Glyph *g, int x, int y, Game::Pixel colorMask)
{
	int px, py;
	int w = g->width;
	int h = g->height;
	
	Pixel *src = ((Pixel*)g->pixels);
	Pixel *dest = ((Pixel*)screen->pixels) +  y*screen->width + x;

	// clip the rectangle	
	if (y < 0)
	{
		src -= y*texture->width;
		dest -= y*screen->width;
		h += y;
		y = 0;
	}
	
	if (y + h > screen->height - 1)
	{
		h = screen->height - y;
	}
	
	if (x < 0)
	{
		src -= x;
		dest -= x;
		w += x;
		x = 0;
	}
	
	if (x + w > screen->width - 1)
	{
		w = screen->width - x;
	}
	
	if (w > 0 && h > 0)
	{
		for(py=0; py<h; py++)
		{
			for(px=0; px<w; px++)
			{
				if (*src)
				{
					*dest = (*src) & colorMask;
				}
				src++;
				dest++;
			}
			src += texture->width - w;
			dest += screen->width - w;
		}
	}
}

void BitmapFont::renderText(Game::Surface *screen, const char *text, int x, int y, Game::Pixel colorMask)
{
	while(*text)
	{
		if (*text >= minGlyph && *text <= maxGlyph)
		{
			Glyph *g = &glyph[*text - minGlyph];
			
			if (g->pixels)
			{
				switch(screen->format.bytesPerPixel)
				{
				case 1:
					renderGlyph<Game::Pixel8>(screen, g, x, y, colorMask);
				break;
				case 2:
					renderGlyph<Game::Pixel16>(screen, g, x, y, colorMask);
				break;
				case 4:
					renderGlyph<Game::Pixel32>(screen, g, x, y, colorMask);
				break;
				}
			}
			x += g->width;
		}
		text++;
	}
}
