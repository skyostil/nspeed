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

#include "GameEngine.h"

#include <stdio.h>
#include "Config.h"
#include "FixedPointMath.h"
#include "FixedPointVector.h"
#include "FixedPointMatrix.h"
#include "Rasterizer.h"
#include "View.h"
#include "World.h"
#include "Land.h"
#include "Mixer.h"
#include "ModPlayer.h"
#include "Track.h"
#include "Car.h"
#include "BitmapFont.h"
#include "Environment.h"

#ifdef EPOC
#include <e32keys.h>
#define KEY_LEFT	EStdKeyLeftArrow
#define KEY_RIGHT	EStdKeyRightArrow
#define KEY_UP		EStdKeyUpArrow
#define KEY_THRUST	'3'
#define KEY_BRAKE	EStdKeyDownArrow
#define KEY_DOWN	EStdKeyDownArrow
#define KEY_EXIT	EStdKeyDevice0
#else
#include <SDL/SDL.h>
#define KEY_LEFT	SDLK_LEFT
#define KEY_RIGHT	SDLK_RIGHT
#define KEY_UP		SDLK_UP
#define KEY_THRUST	'a'
#define KEY_BRAKE	'z'
#define KEY_DOWN	SDLK_DOWN
#define KEY_EXIT	SDLK_ESCAPE
#endif

GameEngine::GameEngine(Game::Framework* _framework):
	Game::Engine(_framework),
	framework(_framework),
	world(0),
	env(0),
	view(0),
	rasterizer(0),
	time(0),
	lastTime(0),
	state(IdleState)
{
	env = new Environment();
	debugMessage[0] = 0;
}

GameEngine::~GameEngine()
{
	delete world;
	delete view;
	delete rasterizer;
	delete env;
}

void GameEngine::configureVideo(Game::Surface* screen)
{
	Game::Surface *img;

	// init the screen
	rasterizer = new Rasterizer(screen);
	view = new View(rasterizer);

	// setup the environment
	img = framework->loadImage(framework->findResource("font.png"), &screen->format);
	env->texturePool.add(img);
	env->font = new BitmapFont(img);
	env->track = new Track(framework, rasterizer->screen);

	// create the world
	world = new World(framework, screen, rasterizer, view, env);
	world->getRenderableSet().add(&env->objectPool);
}

void GameEngine::configureAudio(Game::SampleChunk* sample)
{
	env->mixer = new Mixer(sample->rate, 6);
	env->modplayer = new ModPlayer(env->mixer);
}

void GameEngine::lookAtCarFromBehind(Car *car)
{
	scalar x = FPMul(FPCos(car->getAngle()), FPInt(3)>>1);
	scalar z = FPMul(FPSin(car->getAngle()), FPInt(3)>>1);
	scalar y = FPInt(4)>>3;

	view->camera.target = car->getOrigin() + Vector(x,0,z);
	view->camera.origin = car->getOrigin() + Vector(-x>>1,y,-z>>1);
	view->camera.update();
}

void GameEngine::renderVideo(Game::Surface* screen)
{
	if (state != RaceState)
		setState(RaceState);

	step();

	switch(state)
	{
	case RaceState:
		lookAtCarFromBehind(env->carPool.getItem(0));
		world->render();
		env->carPool.getItem(0)->render(world);
	break;
	}

	env->font->renderText(screen, debugMessage, 1, 1);
}

void GameEngine::renderAudio(Game::SampleChunk* sample)
{
	env->mixer->render(sample);
}

void GameEngine::setState(State newState)
{
	int i;

	if (state == newState) return;

	switch(state)
	{
	case IdleState:
	break;
	case RaceState:
		env->track->unload();

		if (env->modplayer)
			env->modplayer->unload();

		for(i=0; i<env->carPool.getCount(); i++)
			delete env->carPool.getItem(i);
		env->carPool.clear();

		world->getRenderableSet().remove(env->track);
		world->getRenderableSet().remove(&env->objectPool);
	break;
	}

	switch(newState)
	{
	case RaceState:
		env->track->load(framework->findResource("track.trk"));
		env->carPool.add(new Car(world, "car.car"));

		if (env->modplayer)
			env->modplayer->load(framework->findResource("dallas.mod"));

		world->getRenderableSet().add(env->track);
		world->getRenderableSet().add(&env->objectPool);
	break;
	}
	state = newState;
}

void GameEngine::handleEvent(Game::Event* event)
{
	switch(state)
	{
	case IdleState:
		if (event->type == Game::Event::KeyPressEvent && event->key.code == KEY_EXIT)
			framework->exit();
	break;
	case RaceState:
		handleRaceEvent(event);
	break;
	}
}

void GameEngine::handleRaceEvent(Game::Event* event)
{
	Car *car = env->carPool.getItem(0);

	switch(event->type)
	{
		case Game::Event::ExitEvent:
		break;
		case Game::Event::KeyPressEvent:
		{
			switch(event->key.code)
			{
			case KEY_EXIT:
				framework->exit();
			break;
			case KEY_THRUST:
				car->setThrust(true);
				car->setBrake(false);
			break;
			case KEY_BRAKE:
				car->setThrust(false);
				car->setBrake(true);
			break;
			case KEY_LEFT:
				car->setSteering(-1);
			break;
			case KEY_RIGHT:
				car->setSteering(1);
			break;
			}
		}
		break;
		case Game::Event::KeyReleaseEvent:
		{
			switch(event->key.code)
			{
			case KEY_THRUST:
				car->setThrust(false);
			break;
			case KEY_BRAKE:
				car->setBrake(false);
			break;
			case KEY_LEFT:
				if (car->getSteering() == -1)
						car->setSteering(0);
			break;
			case KEY_RIGHT:
				if (car->getSteering() == 1)
					car->setSteering(0);
			break;
			}
		}
	}
}

void GameEngine::step()
{
	const int timestep = 512;

	time = (256*framework->getTickCount() / framework->getTicksPerSecond()) << (FP-8);

	if (lastTime == 0)
	{
		lastTime = time;
		return;
	}
	
	sprintf(debugMessage, "%d, %d", time - lastTime, (time-lastTime)/timestep);

	if ((time - lastTime) > timestep)
	{
		while((time - lastTime) > timestep)
		{
			atomicStep();
			lastTime+=timestep;
		}
		lastTime = time;
	}
}

void GameEngine::atomicStep()
{
	int i;

	for(i=0; i<env->carPool.getCount(); i++)
		env->carPool.getItem(i)->update(env->track);
}

// bootstrap
extern "C"
{

Game::Engine* CreateEngine(Game::Framework* framework)
{
        return new GameEngine(framework);
}

}

