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
#include "Config.h"
#include "Car.h"
#include "Object.h"
#include "Menu.h"

class GameEngine: public Game::Engine, public Object
{
public:
	enum State
	{
		IdleState,
		MainMenuState,
		ChooseCarState,
		ChooseTrackState,
		RaceIntroState,
		RaceCountDownState,
		RaceMenuState,
		RaceState,
		QuitState
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
	void			handleMenuAction(Menu::Action action);
	void			lookAtCarFromBehind(Car *car);
	void 			rotateAroundCar(Car *car);
	void 			rotateAroundGoal(Track *track);
	void			handleRaceEvent(Game::Event* event);
	void			step();
	void			atomicStep();
	void			fillMenuWithDirectories(Menu *menu, const char *path);
	void			copySelectedMenuItem(Menu *menu, char *out, unsigned int outSize);
	void			renderTitle(Game::Surface *s, const char *title);

	Game::Framework	*framework;
	State			state;
	Environment		*env;
	scalar			time, lastTime, fpsCountStart;
	unsigned int	frameCount;
	bool			rotateCamera;

	// main menu
	MenuItem		menuItemPractice;
	MenuItem		menuItemQuit;
	Game::Surface	*logo;
	
	// choose car menu
	Car				*demoCar;

	// choose track menu
	Track			*demoTrack;
	
	// race
	int				raceCountDown;
		
	// race menu
	MenuItem		menuItemRestart;
	MenuItem		menuItemMainMenu;
	
	Set<MenuItem*>	menuItemList;
	
	char			debugMessage[256];
	char			selectedCar[64], selectedTrack[64];
};

#endif
