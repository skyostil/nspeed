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

#ifndef TRACK_H
#define TRACK_H

#include "engine/Engine.h"
#include "Land.h"
#include "FixedPointVector.h"

class View;

class Track: public Renderable
{
public:
	Track(Game::Framework *_framework, Game::Surface *_screen);
	~Track();

	bool		load(const char *name);
	void		unload();
	void		render(World *world);
	
	int			getCell(const Vector &pos) const;
	void		setCell(const Vector &pos);
	
	//! Returns approximate the 2D normal (x,z) of the track at the given position
	Vector		getNormal(const Vector &pos) const;

protected:
	void		project(const Vector &pos, unsigned char &x, unsigned char &y) const;

	inline Game::Pixel8 lookup(unsigned char x, unsigned char y) const
	{
		return ((Game::Pixel8*)(texture->pixels))[x + (y<<8)];
	}

	Land			*land, *ground;
	Game::Surface	*texture, *groundTexture, *skyTexture;
	Game::Surface   *textureTileList[256];
	Game::Surface	*screen;
	Game::Framework	*framework;
};

#endif
