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

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "engine/Engine.h"
#include "Set.h"
#include "Object.h"
#include "MeshSet.h"

class Track;
class BitmapFont;
class Mixer;
class ModPlayer;
class Car;
class View;
class Rasterizer;

//! This class contains all the global game related data
class Environment: public Object
{
public:
//	Environment(Object *parent, Game::Framework *_framework, Game::Surface *_screen, View *_view);
	Environment(Object *parent, Game::Framework *_framework, Game::Surface *_screen);
	~Environment();

	//! Loads an image. Don't forget to delete it!
	Game::Surface	*loadImage(const char *name);
	
	Game::Surface	*getScreen() { return screen; }
	Game::Framework	*getFramework() { return framework; }
	Rasterizer		*getRasterizer() { return rasterizer; }
	View			*getView() { return view; }
	World			*getWorld() { return world; }
	
	void			initializeSound(Game::SampleChunk *sample);
	
	Track				*track;
	BitmapFont			*font, *bigFont;

	Mixer				*mixer;
	ModPlayer			*modplayer;

	MeshSet				meshPool;
	Set<Game::Surface*>	texturePool;
	Set<Car*>			carPool;
	
private:
	Game::Surface		*fontImage;

	Game::Surface		*screen;
	Game::Framework		*framework;
	Rasterizer			*rasterizer;
	View				*view;
	World				*world;
};

#endif
