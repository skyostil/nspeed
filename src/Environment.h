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

class Track;
class BitmapFont;
class Mixer;
class ModPlayer;
class Car;

#define MAX_TEXTURES	64
#define MAX_CARS		4

//! This class contains all the global game related data
class Environment
{
public:
	Environment();
	~Environment();

	Track		*track;
	BitmapFont	*font;

	Mixer		*mixer;
	ModPlayer	*modplayer;

	ObjectSet			objectPool;
	Set<Game::Surface*>	texturePool;
	Set<Car*>			carPool;
};

#endif
