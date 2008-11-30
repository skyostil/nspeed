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

#ifndef LAND_H
#define LAND_H

#include "engine/Engine.h"
#include "Rasterizer.h"
#include "View.h"
#include "World.h"
#include "Renderable.h"
#include "FixedPointMath.h"

class Land: public Renderable
{
public:
	//! Depth must be > (1<<FP)
	Land(Game::Surface *_texture,
	     Game::Surface *_skyTexture = 0,
	     int _flags = Rasterizer::FlagTileTexture | Rasterizer::FlagPerspectiveCorrection,
	     int _textureScale = 4,
	     scalar _depth = FPInt(1));
	
	void render(World *world);
	
private:
	Game::Surface	*texture, *skyTexture;
	int		flags, textureScale;
	scalar		invDepth;
};

#endif
