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
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
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

#define RACECOUNTDOWN	256

GameEngine::GameEngine(Game::Framework* _framework):
	Object(0),
	Game::Engine(_framework),
	framework(_framework),
	env(0),
	time(0),
	logo(0),
	lastTime(0),
	state(IdleState),
	fpsCountStart(0),
	frameCount(0),
	raceCountDown(0),
	rotateCamera(false),
	menuItemPractice("Practice"),
	menuItemQuit("Quit"),
	menuItemRestart("Restart Race"),
	menuItemMainMenu("Main Menu")
{
	debugMessage[0] = 0;
	selectedCar[0] = 0;
	selectedTrack[0] = 0;
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
	scalar x = FPMul(FPCos(angle), FPInt(3)>>3);
	scalar z = FPMul(FPSin(angle), FPInt(3)>>3);
//	scalar y = FPInt(2)>>2;
	scalar y = FPInt(3)>>3;

	env->getView()->camera.target = car->getOrigin() + Vector(x,0,z);
	env->getView()->camera.origin = car->getOrigin() + Vector(-x>>1,y,-z>>1);
	env->getView()->camera.update();
}

void GameEngine::rotateAroundCar(Car *car)
{
	scalar angle = FPMod(car->getAngle() + (time>>1), 2*PI);
	scalar x = FPMul(FPCos(angle), FPInt(1)>>1)>>2;
	scalar z = FPMul(FPSin(angle), FPInt(1)>>1)>>2;
	scalar y = FPInt(3)>>4;

	env->getView()->camera.target = car->getOrigin();
	env->getView()->camera.origin = car->getOrigin() + Vector(x,y,z);
	env->getView()->camera.update();
}

void GameEngine::rotateAroundGoal(Track *track)
{
	scalar angle = FPMod((time>>1), 2*PI);
	scalar angle2 = FPMod((time), 2*PI);
	scalar x = FPMul(FPCos(angle), FPInt(3)>>3);
	scalar z = FPMul(FPSin(angle), FPInt(3)>>3);
	scalar y = FPMul(FPSin(angle2), FPInt(3)>>2) + FPInt(3)>>3;

	env->getView()->camera.target = track->getStartingPosition(0);
	env->getView()->camera.origin = track->getStartingPosition(0) + Vector(x,y,z);
	env->getView()->camera.update();
}

void GameEngine::renderVideo(Game::Surface* screen)
{
	Menu *menu = env->getMenu();
	World *world = env->getWorld();

	if (state == IdleState)
		setState(MainMenuState);

	step();
	handleMenuAction(env->getMenu()->getAction());
	
	switch(state)
	{
	case MainMenuState:
		screen->clear(0);
		if (logo)
			screen->renderTransparentSurface(logo, screen->width/2 - logo->width/2, screen->height/3 - logo->height);
		menu->render(world);
	break;
	case ChooseCarState:
		screen->clear(0);

		if (demoCar)
		{
			scalar speed = 0;
			int x, y, d;
			Game::Pixel8 r = 128, g = 128, b = 128;
			
			for(x=16; x<screen->width-16; x++)
			{
				y = screen->height/2 - ((speed>>7) - (menu->getSelectionRectangleError()<<3));
				
				if (x&1) r++;
				
				if (y > 0)
				{
					for(d=0 ;y < screen->height/2; y++, d+=3)
					{
						int gr = r - d;
						int gg = g - d;
						int gb = b - d;
						
						if (gr<0) gr=0;
						if (gg<0) gg=0;
						if (gb<0) gb=0;
						
						Game::Pixel16 pixel = screen->format.makePixel(gr,gg,gb);
						screen->setPixel(x, y, pixel);
					}
				}
//				printf("%d\n", speed);
				speed += demoCar->getAcceleration(speed);
				speed += demoCar->getAcceleration(speed);
			}
		
			rotateAroundCar(demoCar);
			world->render();
		}
				
		renderTitle(screen, "Choose Car");
		menu->render(world);
	break;
	case ChooseTrackState:
		screen->clear(0);
		
		if (demoTrack)
		{
			rotateAroundGoal(demoTrack);
			demoTrack->render(world);
		
			Game::Surface *m = demoTrack->getMap();
			if (m)
				screen->renderTransparentSurface(m, screen->width/2 - m->width/2 + (menu->getSelectionRectangleError()<<2), screen->height/3 - m->height/2);
		}
		renderTitle(screen, "Choose Track");
		menu->render(world);
	break;
	case RaceIntroState:
	case RaceCountDownState:
	case RaceState:
	case RaceMenuState:
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
		
		BitmapFont *font = env->font;
		char text[16];
		int speedX = 4;
		int speedY = env->getScreen()->height - font->getHeight() - 2;
		scalar speed = playerCar->getSpeed();
		
		if (speed <= (32<<2))
			speed = 0;
		
		sprintf(text, "%3d km/h", speed >> 5);
		font->renderText(env->getScreen(), text, speedX, speedY);
		
		sprintf(text, "lap %d:%d", playerCar->getLapCount()+1, env->track->getLapCount());
		font->renderText(env->getScreen(), text, speedX, speedY - font->getHeight() - 2);
		
		if (state == RaceMenuState)
		{
			menu->render(world);
		}
		else if (raceCountDown > 0)
		{
			if (raceCountDown > 3*(RACECOUNTDOWN/4))
				strncpy(text, "3", sizeof(text));
			else if (raceCountDown > 2*(RACECOUNTDOWN/4))
				strncpy(text, "2", sizeof(text));
			else if (raceCountDown > 1*(RACECOUNTDOWN/4))
				strncpy(text, "1", sizeof(text));
			else
			{
				setState(RaceState);
				strncpy(text, "go!!", sizeof(text));
			}
				
			env->bigFont->renderText(env->getScreen(), text, screen->width/2 - env->bigFont->getTextWidth(text), screen->height/2 - env->bigFont->getHeight());
		}
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
	Menu *menu = env->getMenu();
	Set<Renderable*> *renderableSet = &env->getWorld()->getRenderableSet();
	Game::Surface *screen = env->getScreen();

	if (state == newState) return;

	// clear old state
	switch(state)
	{
	case IdleState:
	break;
	case MainMenuState:
		delete logo;
		logo = 0;
	break;
	case ChooseCarState:
		delete demoCar;
		demoCar = 0;
		renderableSet->remove(&env->meshPool);
	break;
	case ChooseTrackState:
		delete demoTrack;
		demoTrack = 0;
	break;
	case RaceState:
	break;
	}

	// set new state
	state = newState;
	switch(newState)
	{
	case MainMenuState:
		menu->clear();
		menu->addItem(&menuItemPractice);
		menu->addItem(&menuItemQuit);
		menu->setTopLevelMenu(true);
		
		env->track->unload();
		
		if (env->modplayer)
			env->modplayer->unload();
		
		for(i=0; i<env->carPool.getCount(); i++)
			delete env->carPool.getItem(i);
		env->carPool.clear();

		renderableSet->remove(env->track);
		renderableSet->remove(&env->meshPool);
		
		logo = env->loadImage("logo.png");
	break;
	case ChooseCarState:
		menu->clear();
		fillMenuWithDirectories(menu, framework->findResource("cars"));
		menu->setTopClipping(screen->height - 64);
		renderableSet->add(&env->meshPool);
	break;
	case ChooseTrackState:
		menu->clear();
		fillMenuWithDirectories(menu, framework->findResource("tracks"));
		menu->setTopClipping(screen->height - 64);
	break;
	case RaceIntroState:
		if (!env->track->load(selectedTrack))
		{
			setState(MainMenuState);
			return;
		}
		env->carPool.add(new Car(env->getWorld(), selectedCar));
		env->carPool.add(new Car(env->getWorld(), selectedCar));
		env->carPool.getItem(1)->setAiState(true);
		
		if (env->modplayer)
		{
			char song[256];
			sprintf(song, "tracks/%32s/music.mod", selectedTrack);
			if (!env->modplayer->load(framework->findResource(song)))
			{
				env->modplayer->load(framework->findResource("music.mod"));
			}
			env->modplayer->play();
		}
		
		renderableSet->add(env->track);
		renderableSet->add(&env->meshPool);
	
		setState(RaceCountDownState);
	break;
	case RaceCountDownState:
		for(i=0; i<env->carPool.getCount(); i++)
			env->carPool.getItem(i)->prepareForRace(i);
		raceCountDown = RACECOUNTDOWN;
	break;
	case RaceState:
/*	
		if (!env->track->load("test"))
		{
			setState(IdleState);
			return;
		}

		env->carPool.add(new Car(env->getWorld(), "default"));
		env->carPool.add(new Car(env->getWorld(), "default"));
		env->carPool.getItem(0)->prepareForRace(0);
		env->carPool.getItem(1)->prepareForRace(1);
		env->carPool.getItem(1)->setAiState(true);
		
		if (env->modplayer)
		{
			env->modplayer->load(framework->findResource("dallas.mod"));
//			env->modplayer->play();
		}

		env->getWorld()->getRenderableSet().add(env->track);
		env->getWorld()->getRenderableSet().add(&env->meshPool);
*/		
	break;
	case QuitState:
		framework->exit();
	break;
	case RaceMenuState:
		menu->clear();
		menu->addItem(&menuItemRestart);
		menu->addItem(&menuItemMainMenu);
	break;
	}
}

void GameEngine::handleMenuAction(Menu::Action action)
{
	Menu *menu = env->getMenu();

	switch(action)
	{
	case Menu::GoBack:
		switch(state)
		{
		case ChooseCarState:
			setState(MainMenuState);
		break;
		case ChooseTrackState:
			setState(ChooseCarState);
		break;
		case RaceMenuState:
			setState(RaceState);
		break;
		}
	break;
	case Menu::Select:
		switch(state)
		{
		case MainMenuState:
			if (menu->getSelection() == &menuItemPractice)
				setState(ChooseCarState);
			else if (menu->getSelection() == &menuItemQuit)
				setState(QuitState);
		break;
		case ChooseCarState:
			copySelectedMenuItem(menu, selectedCar, sizeof(selectedCar));
			setState(ChooseTrackState);
		break;
		case ChooseTrackState:
			copySelectedMenuItem(menu, selectedTrack, sizeof(selectedTrack));
			setState(RaceIntroState);
		break;
		case RaceMenuState:
			if (menu->getSelection() == &menuItemRestart)
				setState(RaceCountDownState);
			else if (menu->getSelection() == &menuItemMainMenu)
				setState(MainMenuState);
		break;
		}
	break;
	case Menu::GoUp:
	case Menu::GoDown:
		switch(state)
		{
		case ChooseCarState:
			delete demoCar;
			demoCar = new Car(env->getWorld(), menu->getSelection()->getText());
		break;
		case ChooseTrackState:
			delete demoTrack;
			demoTrack = new Track(this, env);
			demoTrack->load(menu->getSelection()->getText(), 2);
		break;
		}
	break;
	}
}

void GameEngine::copySelectedMenuItem(Menu *menu, char *out, unsigned int outSize)
{
	MenuItem *item = menu->getSelection();
	
	if (item)
	{
		strncpy(out, item->getText(), outSize);
	}
	else
		out[0] = 0;
}

void GameEngine::handleEvent(Game::Event* event)
{
	env->getMenu()->handleEvent(event);

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
				setState(RaceMenuState);
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
				car->setAiState(rotateCamera);
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
		Car *car = env->carPool.getItem(0);
//		sprintf(debugMessage, "%d fps, %d s, gate %d, lap %d", frameCount*4, framework->getTickCount() / framework->getTicksPerSecond(), car?car->getGateIndex():-1, car?car->getLapCount()+1:0);
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
	
	switch(state)
	{
	case RaceState:
		for(i=env->carPool.begin(); i!=env->carPool.end(); i++)
			(*i)->update(env->track);
	case RaceCountDownState:
		if (raceCountDown > 0) raceCountDown--;
	break;
	}
}

void GameEngine::fillMenuWithDirectories(Menu *menu, const char *path)
{
	DIR *dir = opendir(path);
	
	menuItemList.deleteItems();
	
	if (dir)
	{
		struct dirent *entry;
		struct stat entryStat;
		
		while(entry = readdir(dir))
		{
			char fullName[256];
			
			snprintf(fullName, sizeof(fullName), "%s/%s", path, entry->d_name);
			stat(fullName, &entryStat);
			
			if (entry->d_name[0] != '.' && S_ISDIR(entryStat.st_mode))
			{
				MenuItem *item = new MenuItem(entry->d_name);
				menuItemList.add(item);
				menu->addItem(item);
			}
		}
		closedir(dir);
	}
}

void GameEngine::renderTitle(Game::Surface *s, const char *title)
{
	env->bigFont->renderText(s, title, 1, 1, -1);
}

// bootstrap
extern "C"
{

Game::Engine* CreateEngine(Game::Framework* framework)
{
        return new GameEngine(framework);
}

}

