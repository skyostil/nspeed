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

#include "Config.h"
#include "Track.h"

#ifdef EPOC
#include <ezlib.h>
#else
#include <zlib.h>
#endif

#include <stdio.h>

Track::Track(Game::Framework *_framework, Game::Surface *_screen):
	framework(_framework),
	screen(_screen),
	texture(0),
	groundTexture(0),
	skyTexture(0),
	land(0),
	ground(0)
{
	int i;

	for(i=0; i<sizeof(textureTileList)/sizeof(textureTileList[0]); i++)
	{
		textureTileList[i] = 0;
	}
}

Track::~Track()
{
	unload();
}

bool Track::load(const char *name)
{
	FILE *f = fopen(name, "rb");
	unsigned short w, h;
	unsigned char *cdata;
	unsigned long len;
	Game::PixelFormat pf(8);

	unload();

	if (!f)
		return false;

	fread(&w, sizeof(w), 1, f);
	fread(&h, sizeof(h), 1, f);

	len = w * h;

	texture = new Game::Surface(&pf, w, h);
	land = new Land(texture);
	cdata = new unsigned char[w * h];

	fread(cdata, w * h, 1, f);
	if (uncompress((Bytef*)texture->pixels, &len, cdata, len) != Z_OK)
	{
		delete[] cdata;
		unload();
		return false;
	}
	delete[] cdata;

	textureTileList[1] = framework->loadImage(framework->findResource("test.png"), &screen->format);
	textureTileList[2] = framework->loadImage(framework->findResource("test2.png"), &screen->format);

	groundTexture = framework->loadImage(framework->findResource("ground.png"), &screen->format);
	skyTexture = framework->loadImage(framework->findResource("sky.png"), &screen->format);
	ground = new Land(groundTexture, skyTexture, Rasterizer::FlagPerspectiveCorrection, 0 /* textureScale */, FPInt(6));

	fclose(f);
	return true;
}

void Track::unload()
{
	int i;

	for(i=0; i<sizeof(textureTileList)/sizeof(textureTileList[0]); i++)
	{
		delete textureTileList[i];
		textureTileList[i] = 0;
	}

	delete texture;
	texture = 0;

	delete land;
	land = 0;

	delete ground;
	ground = 0;

	delete skyTexture;
	skyTexture = 0;

	delete groundTexture;
	groundTexture = 0;
}

void Track::render(World *world)
{
	if (ground)
	{
		ground->render(world);
	}

	if (land)
	{
		world->getView()->rasterizer->setTextureTileList(textureTileList);
		land->render(world);
	}

	int x, y, scale = 6;

	for(y=0; y<texture->height; y+=scale)
	for(x=0; x<texture->width; x+=scale)
	{
		int px = world->getScreen()->width - texture->width/scale + x/scale;
		int py = world->getScreen()->height - texture->height/scale + y/scale;
		if (texture->getPixel((x+128)&0xff,(y+128)&0xff))
			world->getScreen()->setPixel(px, py, -1);
		else if (
			texture->getPixel((x+128)&0xff,(y+128+scale)&0xff) ||
			texture->getPixel((x+128)&0xff,(y+128-scale)&0xff) ||
			texture->getPixel((x+128+scale)&0xff,(y+128)&0xff) ||
			texture->getPixel((x+128-scale)&0xff,(y+128)&0xff)
			)
			world->getScreen()->setPixel(px, py, 0);
	}
}


int Track::getCell(Vector &pos)
{
	unsigned char x, y;
	
	project(pos, x, y);
	
	return lookup(x,y);
}

void Track::setCell(Vector &pos)
{
	unsigned char x, y;

	project(pos, x, y);
		
	((Game::Pixel8*)(texture->pixels))[x + (y<<8)] = 1;
}

Vector &Track::getNormal(Vector &pos)
{
	const int size = 4; // sample square size / 2
	unsigned char cx, cy;
	int px, py;
	Vector normal(0,0,0);

	project(pos, cx, cy);
	
	for(py=cy-size; py<cy+size; py++)
	for(px=cx-size; px<cx+size; px++)
	{
		Game::Pixel8 p = lookup((unsigned char)px, (unsigned char)py);
		
		if (p)
		{
			// XXX - possible sign issues here
			normal.x += FPInt(px - cx);
			normal.z += FPInt(py - cy);
		}
	}
	
	normal /= FPInt(size*size);
	
	return normal.normalize();
}

void Track::project(Vector &pos, unsigned char &x, unsigned char &y)
{
	x = (pos.x << 3) >> FP;
	y = (pos.z << 3) >> FP;
}
