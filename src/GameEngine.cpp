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
#define KEY_ROTATE	EStdKeyDevice1
#else
#include <SDL/SDL.h>
#define KEY_LEFT	SDLK_LEFT
#define KEY_RIGHT	SDLK_RIGHT
#define KEY_UP		SDLK_UP
#define KEY_THRUST	'a'
#define KEY_BRAKE	'z'
#define KEY_DOWN	SDLK_DOWN
#define KEY_EXIT	SDLK_ESCAPE
#define KEY_ROTATE	'r'
#endif

GameEngine::GameEngine(Game::Framework* _framework):
	Object(0),
	Game::Engine(_framework),
	framework(_framework),
	env(0),
	time(0),
	lastTime(0),
	state(IdleState),
	fpsCountStart(0),
	frameCount(0),
	rotateCamera(false)
{
	debugMessage[0] = 0;
}

GameEngine::~GameEngine()
{
}

void GameEngine::configureVideo(Game::Surface* screen)
{
	Game::Surface *img;

	env = new Environment(this, framework, screen);
}

void GameEngine::configureAudio(Game::SampleChunk* sample)
{
	env->initializeSound(sample);
}

void GameEngine::lookAtCarFromBehind(Car *car)
{
	scalar angle = rotateCamera?FPMod(car->getAngle() + (time>>1), 2*PI):car->getAngle();
	scalar x = FPMul(FPCos(angle), FPInt(3)>>1);
	scalar z = FPMul(FPSin(angle), FPInt(3)>>1);
	scalar y = FPInt(2)>>2;

	env->getView()->camera.target = car->getOrigin() + Vector(x,0,z);
	env->getView()->camera.origin = car->getOrigin() + Vector(-x>>1,y,-z>>1);
	env->getView()->camera.update();
}

void GameEngine::renderVideo(Game::Surface* screen)
{
	if (state != RaceState)
		setState(RaceState);

	step();

	switch(state)
	{
	case RaceState:
	{
		Car *playerCar = env->carPool.getItem(0);
		
		// render the world
		lookAtCarFromBehind(playerCar);
		env->getWorld()->render();
		
		// render the map & OSD
		Game::Surface *map = env->track->getMap();

		if (map != NULL)
		{
			int mapX = env->getScreen()->width - env->track->getMap()->width - 2;
			int mapY = env->getScreen()->height - env->track->getMap()->height - 2;
			env->getScreen()->renderTransparentSurface(env->track->getMap(), mapX, mapY);
		}
		
		char speedText[16];
		int speedX = 4;
		int speedY = env->getScreen()->height - env->bigFont->getHeight() - 2;
		scalar speed = playerCar->getSpeed();
		
		if (speed <= (32<<2))
			speed = 0;
		
		sprintf(speedText, "%3d kph", speed >> 2);
		env->bigFont->renderText(env->getScreen(), speedText, speedX, speedY);
	}
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

		env->getWorld()->getRenderableSet().remove(env->track);
		env->getWorld()->getRenderableSet().remove(&env->meshPool);
	break;
	}

	switch(newState)
	{
	case RaceState:
		if (!env->track->load(framework->findResource("track3.trk")))
		{
			setState(IdleState);
			return;
		}

		env->carPool.add(new Car(env->getWorld(), "default"));

		if (env->modplayer)
		{
			env->modplayer->load(framework->findResource("dallas.mod"));
			env->modplayer->play();
		}

		env->getWorld()->getRenderableSet().add(env->track);
		env->getWorld()->getRenderableSet().add(&env->meshPool);
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
			case KEY_ROTATE:
				rotateCamera = !rotateCamera;
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
	
	if (framework->getTickCount() - fpsCountStart > framework->getTicksPerSecond() / 4)
	{
		sprintf(debugMessage, "%d fps", frameCount*4);
		fpsCountStart = framework->getTickCount();
		frameCount = 0;
	}
	else
		frameCount++;
	
	if ((time - lastTime) >= timestep)
	{
		while((time - lastTime) >= timestep)
		{
			atomicStep();
			lastTime+=timestep;
		}
		lastTime = time;
	}
}

void GameEngine::atomicStep()
{
	Set<Car*>::Iterator i;

	for(i=env->carPool.begin(); i!=env->carPool.end(); i++)
		(*i)->update(env->track);
}

// bootstrap
extern "C"
{

Game::Engine* CreateEngine(Game::Framework* framework)
{
        return new GameEngine(framework);
}

}

