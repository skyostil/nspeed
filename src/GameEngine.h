/***************************************************************************
 *   Copyright (C) 2004 by Sami Ky�stil�                                   *
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

#ifndef ENGINE_H
#define ENGINE_H

#include "engine/Engine.h"
#include "Config.h"
#include "Car.h"
#include "Object.h"

class GameEngine: public Game::Engine, public Object
{
public:
	enum State
	{
		IdleState,
		RaceState
	};

	GameEngine(Game::Framework* _framework);
	~GameEngine();

	void configureVideo(Game::Surface* screen);
	void configureAudio(Game::SampleChunk* sample);
	void renderVideo(Game::Surface* screen);
	void renderAudio(Game::SampleChunk* sample);
	void handleEvent(Game::Event* event);

private:
	void			setState(State newState);
	void			lookAtCarFromBehind(Car *car);
	void			handleRaceEvent(Game::Event* event);
	void			step();
	void			atomicStep();

	Game::Framework	*framework;
	State			state;
	Rasterizer      *rasterizer;
	View            *view;
	World			*world;
	Environment		*env;
	scalar			time, lastTime;

	char			debugMessage[256];
};

#endif