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

#ifndef MENU_H
#define MENU_H

#include "engine/Engine.h"
#include "Renderable.h"
#include "Set.h"
#include "Object.h"

class Environment;
class World;

class MenuItem
{
public:
	MenuItem(const char *_text = "");
	~MenuItem();
	
	void clear();
	void setText(const char *_text);
	const char *getText() const;
	
private:
	char *text;
};

class Menu: public Renderable, public Object
{
public:
	Menu(Object *parent, Environment *_env);
	~Menu();
	
	enum Action
	{
		NoAction,
		Select,
		GoUp,
		GoDown,
		GoBack,
	};
	
	void clear();
	void addItem(MenuItem *item);
	void render(World *world);
	Action handleEvent(Game::Event* event);
	MenuItem *getSelection() const;
	
	void setTopClipping(int _clipMinY);
	void setTopLevelMenu(bool topLevel);
	
	int getSwooshAmount() const;
	int getSelectionRectangleError() const;
	
	//! Returns the menu action deferred by e.g. animation
	Action getAction();

	void dimScreen(Game::Surface *s, int minY, int maxY) const;
private:
	unsigned int getWidth(MenuItem *item) const;
	unsigned int getHeight(MenuItem *item) const;
	void renderItem(MenuItem *item, int x, int y, int w, int h, bool selected) const;
	void renderSelectionRectangle(int x, int y, int w, int h) const;
	void animate();
	void atomicStep();

	int				selected, selectionBarY, selectionBarTargetY, selectionBarHeight;
	int				lastTime;
	int				swooshDirection, swooshAmount;
	int				jiggleAmount, jiggleSpeed, jiggleDirection;
	int				minY, maxY, offsetY;
	int				clipMinY;
	bool			swooshDone, topLevelMenu;
	Action			deferredAction;
	Environment		*env;
	Set<MenuItem*>	items;
};

#endif
