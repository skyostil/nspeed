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

#include "Engine.h"

#include <SDL.h>
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

Engine::Engine(Game::Framework* _framework):
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
}

Engine::~Engine()
{
	delete world;
	delete view;
	delete rasterizer;
	delete env;
}

void Engine::configureVideo(Game::Surface* screen)
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

void Engine::configureAudio(Game::SampleChunk* sample)
{
	env->mixer = new Mixer(sample->rate, 6);
	env->modplayer = new ModPlayer(env->mixer);
	setState(RaceState);
}

void Engine::lookAtCarFromBehind(Car *car)
{
	scalar x = FPMul(FPCos(car->getAngle()), FPInt(1));
	scalar z = FPMul(FPSin(car->getAngle()), FPInt(1));
	scalar y = FPInt(3)>>3;

	view->camera.target = car->getOrigin();
	view->camera.origin = car->getOrigin() + Vector(-x,y,-z);
	view->camera.update();
}

void Engine::renderVideo(Game::Surface* screen)
{
	step();

	switch(state)
	{
	case RaceState:
		lookAtCarFromBehind(env->carPool.getItem(0));
		world->render();
	break;
	}
}

void Engine::renderAudio(Game::SampleChunk* sample)
{
	env->mixer->render(sample);
}

void Engine::setState(State newState)
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

	state = newState;

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
}

void Engine::handleEvent(Game::Event* event)
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

void Engine::handleRaceEvent(Game::Event* event)
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
			case KEY_UP:
				car->setThrust(true);
				car->setBrake(false);
			break;
			case KEY_DOWN:
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
			case KEY_UP:
				car->setThrust(false);
			break;
			case KEY_DOWN:
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

void Engine::step()
{
	const int timestep = 256;

	time = (100*framework->getTickCount() / framework->getTicksPerSecond()) << (FP-8);
	
	if (time - lastTime > timestep)
	{
		while(time - lastTime > timestep)
		{
			atomicStep();
			lastTime+=timestep;
		}
		lastTime = time;
	}
}

void Engine::atomicStep()
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
        return new Engine(framework);
}

}

