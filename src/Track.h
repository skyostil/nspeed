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
#include "Renderable.h"

class View;

class LineSegment
{
public:
	LineSegment();

	void set(const Vector &_left, const Vector &_right);

	bool isInside(const Vector &pos) const;
	bool isValid() const { return valid; }
	const Vector &getCenter() const { return center; }
	const Vector &getNormal() const { return normal; }
	const Vector &getLeft() const { return left; }
	const Vector &getLeftToRight() const { return leftToRight; }
	const Vector &getRight() const { return right; }
	scalar getAngle() const { return angle; }
	bool getNearestPoint(const Vector &pos, Vector &out) const;

private:
	Vector left, right, leftToRight, center, normal;
	scalar angle;
	scalar lengthSquared;
	bool valid;
};

class Track: public Renderable, public Object
{
public:
	enum TileRanges
	{
		EdgeStart = 2, // XXX fixme
		EdgeEnd = 2,
		AIPathStart = 96,
		AIPathEnd = 128,
		BorderStart = 32,
		BorderEnd = 63,
		BounceStart = 24,
		BounceEnd = 32,
		RechargeStart = 16,
		RechargeEnd = 24,
		TurboStart = 8,
		TurboEnd = 16,
	};

	Track(Object *parent, Environment *_env);
	~Track();

	bool		load(const char *name, int mapScale = 6);
	bool		saveTimes(const char *name);
	void		unload();
	void		render(World *world);
	
	int			getCell(const Vector &pos) const;
	void		setCell(const Vector &pos);
	
	Vector		getStartingPosition(int carNumber) const;
	scalar		getStartingAngle() const;
	bool		getNearestPointOnAiPath(const Vector &pos, Vector &out) const;
	bool		shouldAiAvoidTile(unsigned char tile) const;
	bool		tileIsDamaging(unsigned char tile) const;
	bool		tileGivesEnergy(unsigned char tile) const;
	bool		tileBounces(unsigned char tile) const;
	
	int			getLapCount() const { return 5; }
	
	// these times are in milliseconds
	int			getBestLapTime(char *name, unsigned int nameSize);
	int			getBestTotalTime(char *name, unsigned int nameSize);
	
	//! Returns approximate the 2D normal (x,z) of the track at the given position
	Vector		getNormal(const Vector &pos) const;
	
	LineSegment	*getGate(unsigned int index);
	int			getGateCount() { return sizeof(gate)/sizeof(gate[0]); }
	
	Game::Surface	*getMap() { return map; }
protected:
	//! Project a world-vector to a pixel
	void		project(const Vector &pos, unsigned char &x, unsigned char &y) const;
	
	void		setBestLapTime(int t, const char *name);
	void		setBestTotalTime(int t, const char *name);
	
	//! Project a pixel to a world vector
	Vector		unproject(unsigned char x, unsigned char y) const;
	
	void		initializeMap(int scale = 6);

	inline Game::Pixel8 lookup(unsigned char x, unsigned char y) const
	{
		return ((Game::Pixel8*)(texture->pixels))[x + (y<<8)];
	}

	Land			*land, *ground;
	Game::Surface	*texture, *groundTexture, *skyTexture;
	Game::Surface   *textureTileList[256];
	Game::Surface	*map;
	Environment		*env;
	LineSegment		gate[4];
	LineSegment		*aiPath;
	unsigned int	aiPathLength;
	
	int				bestLapTime, bestTotalTime;
	char			bestLapName[16], bestTotalName[16], playerName[16];
};

#endif
