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
//  *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
#ifndef RASTERIZER_H
#define RASTERIZER_H
 
#include "engine/Engine.h"
#include "FixedPointMath.h"

//! Number of extra fraction bits used for inverse z (for perspective correction)
#define INVZ_SCALE		6

// This slows things down a bit
//#define RASTERIZER_2D_CLIPPING

//! Dimensions of a texture tile.
#define TILE_TEXTURE_BITS	5
#define TILE_TEXTURE_SIZE	(1<<TILE_TEXTURE_BITS)
#define TILE_TEXTURE_MASK	((1<<TILE_TEXTURE_BITS)-1)


typedef struct
{
	scalar x1, x2, invz1, invz2;
	// u and v are divided by z if perspective correct rendering is enabled.
	scalar u1z, v1z, u2z, v2z;
	Game::Pixel *offset;
} Scanline;

typedef struct
{
	scalar sx, sy, invz;
	scalar uz, vz;
	
} ScreenVertex;

class Rasterizer
{
public:
	enum Flags
	{
		FlagPerspectiveCorrection = 0x1,
		FlagTileTexture = 0x2,			// requires perspective correction
	};
	
	Rasterizer(Game::Surface *_screen);
	~Rasterizer();
	
	void	setTexture(Game::Surface *_texture);
	void	setTextureTileList(Game::Surface **tiles);
	void	setColor(unsigned short _color);
	
	//! Renders a polygon.
	//! The polygon MUST be convex and clipped to screen-space.
	//! x and y are in screen-space, z in camera-space.
	void	beginPolygon();
	//! invz must be of proper format, \see INVZ_SCALE
	void	setInvZ(scalar invz);
	//! u and v must be divided by z.
	void	setTexCoord(scalar uz, scalar vz);
	void	addVertex(scalar sx, scalar sy);
	void	addVertex(ScreenVertex *v);
	void	endPolygon();
	
	Game::Surface	*screen, *texture;
	Game::Surface	**textureTileList;		// an array of textures
	unsigned short	color;
	
	int		flags;
protected:
	void	finalizeVertex();
	void	addEdge(ScreenVertex *v1, ScreenVertex *v2);
	
	// scanline rasterizers
	inline void	flatRasterizer();
	inline void	textureRasterizer();
	inline void	perspectiveTextureRasterizer();
	inline void	tileTextureRasterizer();
	
	Scanline	*edge;
	int		minY, maxY;
	ScreenVertex	vertex[3];
	int		currentVertex;
	bool		initialEdgeFlag;
};

#endif
