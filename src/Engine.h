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

#ifndef ENGINE_H
#define ENGINE_H

#include "engine/Engine.h"
#include "Car.h"

#ifdef EPOC
#define KEY_LEFT	EStdKeyLeftArrow
#define KEY_RIGHT	EStdKeyRightArrow
#define KEY_UP		EStdKeyUpArrow
#define KEY_DOWN	EStdKeyDownArrow
#define KEY_EXIT	EStdKeyDevice0
#else
#define KEY_LEFT	SDLK_LEFT
#define KEY_RIGHT	SDLK_RIGHT
#define KEY_UP		SDLK_UP
#define KEY_DOWN	SDLK_DOWN
#define KEY_EXIT	SDLK_ESCAPE
#endif

class Engine: public Game::Engine
{
public:
	enum State
	{
		IdleState,
		RaceState
	};

	Engine(Game::Framework* _framework);
	~Engine();

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
};

#endif