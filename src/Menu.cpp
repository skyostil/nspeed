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

#include "Menu.h"
#include "Environment.h"
#include "World.h"
#include "BitmapFont.h"
#include <string.h>
#include <stdio.h>

MenuItem::MenuItem(const char *_text):
	text(0)
{
	setText(_text);
}

MenuItem::~MenuItem()
{
	if (text)
		delete[] text;
	text = 0;
}

void MenuItem::clear()
{
	setText("");
}

void MenuItem::setText(const char *_text)
{
	unsigned int len = strlen(_text), i;
	
	if (text)
		delete[] text;
	text = new char[len+1];
	
	if (text)
		for(i=0; i<len+1; i++)
			text[i] = _text[i];
}

const char *MenuItem::getText() const
{
	return text;
}

Menu::Menu(Object *parent, Environment *_env):
	Object(parent),
	env(_env),
	selected(0),
	selectionBarY(0),
	selectionBarTargetY(0),
	selectionBarHeight(0),
	lastTime(0),
	swooshDirection(0),
	swooshAmount(0),
	deferredAction(NoAction),
	clipMinY(0),
	minY(0),
	maxY(0),
	offsetY(0),
	jiggleAmount(0),
	jiggleDirection(1),
	jiggleSpeed(0),
	topLevelMenu(false)
{
}

Menu::~Menu()
{
}

void Menu::clear()
{
	swooshDirection = 0;
	swooshAmount = 0;
	selected = 0;
	clipMinY = 0;
	offsetY = 0;
	topLevelMenu = false;
	items.clear();
	deferredAction = GoUp;
}

void Menu::addItem(MenuItem *item)
{
	items.add(item);
}

void Menu::render(World *world)
{
	Game::Surface *screen = env->getScreen();
	int i, totalHeight = 0, y, x, w, h;
	const int verticalSpacing = 2;
	
	animate();
	
	for(i=0; i<items.getCount(); i++)
		totalHeight += getHeight(items.getItem(i)) + verticalSpacing;
		
	y = (screen->height - clipMinY) / 2 - totalHeight / 2;
	minY = y - 4 + offsetY;
	maxY = minY + totalHeight + 8;
	
	if (minY < clipMinY) minY = clipMinY;
	if (maxY > screen->height - 1) maxY = screen->height - 1;

	dimScreen(screen, minY, maxY);
	
	if (selectionBarY + offsetY < clipMinY)
		offsetY = clipMinY - selectionBarY;
	if (selectionBarY + offsetY + selectionBarHeight > screen->height - 1)
		offsetY = (screen->height - 1) - selectionBarY - selectionBarHeight;
		
	renderSelectionRectangle(0, selectionBarY + offsetY, screen->width, selectionBarHeight);
	
	for(i=0; i<items.getCount(); i++)
	{
		MenuItem *item = items.getItem(i);
		w = getWidth(item);
		h = getHeight(item);

		x = world->getEnvironment()->getScreen()->width / 2 - w/2 + swooshAmount + ((swooshDirection*i)<<2);
				
		if (i == selected)
		{
			selectionBarTargetY = y;
			selectionBarHeight = h;
			x += (jiggleAmount>>4) - 8;
		}
		
		renderItem(item, x, y + offsetY, w, h, i == selected);
		y += h + verticalSpacing;
	}
}

unsigned int Menu::getWidth(MenuItem *item) const
{
	return env->font->getTextWidth(item->getText());
}

unsigned int Menu::getHeight(MenuItem *item) const
{
	return env->font->getHeight();
}

void Menu::renderItem(MenuItem *item, int x, int y, int w, int h, bool selected) const
{
	if (selected)
	{
	}
	
	if (y < clipMinY)
		return;
	
	env->font->renderText(env->getScreen(), item->getText(), x, y);
}

void Menu::renderSelectionRectangle(int x, int y, int w, int h) const
{
	Game::Surface *screen = env->getScreen();
	Game::Pixel16 *p = (Game::Pixel16*)screen->pixels;
	Game::Pixel16 mask = 
		(screen->format.rmask & (screen->format.rmask>>1)) +
		(screen->format.gmask & (screen->format.gmask>>1)) +
		(screen->format.bmask & (screen->format.bmask>>1));
	const int scale = 1;
	int c = (1<<(screen->format.rsize+1))-1;
	const int r = 1, g = 0, b = 0;

	p+=y * screen->width;

	int maxr = ((1<<screen->format.rsize)-1) << screen->format.rshift;
	int maxg = ((1<<screen->format.gsize)-1) << screen->format.gshift;
	int maxb = ((1<<screen->format.bsize)-1) << screen->format.bshift;
	
	while(h--)
	{
		int gradr = ((r*c)>>scale) << screen->format.rshift;
		int gradg = ((g*c)>>scale) << screen->format.gshift;
		int gradb = ((b*c)>>scale) << screen->format.bshift;
		
		c-=(1<<(screen->format.rsize-3));
		
		for(x=0; x<screen->width; x++)
		{
			int r = (*p) & screen->format.rmask;
			int g = (*p) & screen->format.gmask;
			int b = (*p) & screen->format.bmask;

			r += gradr;
			g += gradg;
			b += gradb;

			if (r > maxr) r = maxr;
			if (g > maxg) g = maxg;
			if (b > maxb) b = maxb;

			*p++ = (r|g|b);
		}
	}
}

void Menu::dimScreen(Game::Surface *s, int minY, int maxY) const
{
	Game::Surface *screen = env->getScreen();
	Game::Pixel16 *p = (Game::Pixel16*)screen->pixels;
	int i = screen->width * (maxY - minY);
	Game::Pixel16 mask = 
		(((1<<screen->format.rsize-1)-1) << screen->format.rshift) |
		(((1<<screen->format.gsize-1)-1) << screen->format.gshift) |
		(((1<<screen->format.bsize-1)-1) << screen->format.bshift);
		
	if (minY > maxY)
		return;

	p += screen->width * minY;
		
	while(i--)
	{
		*p++ = ((*p) >> 1) & mask;
	}
}

void Menu::animate()
{
	const int timestep = 1024;
	int time = (256*env->getFramework()->getTickCount() / env->getFramework()->getTicksPerSecond()) << (FP-8);
	
	if (lastTime == 0)
	{
		lastTime = time;
		return;
	}
	
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

void Menu::atomicStep()
{
	int dist = getSelectionRectangleError() >> 1;
	const int jiggleLimit = 16;
	
	if (dist == 0)
		dist = 1;
	
	if (selectionBarY < selectionBarTargetY)
		selectionBarY+=dist;
	else if (selectionBarY > selectionBarTargetY)
		selectionBarY-=dist;
		
	swooshAmount += swooshDirection;
	swooshDirection += swooshDirection;
		
	jiggleAmount += jiggleSpeed;
	jiggleSpeed += jiggleDirection;
	if (jiggleSpeed == -jiggleLimit || jiggleSpeed == jiggleLimit)
		jiggleDirection = -jiggleDirection;
}

Menu::Action Menu::handleEvent(Game::Event* event)
{
	if (swooshDirection)
		return NoAction;

    if (deferredAction != NoAction)
        return NoAction;

    if (!items.getCount())
        return NoAction;

	switch(event->type)
	{
		case Game::Event::ExitEvent:
		break;
		case Game::Event::KeyPressEvent:
		{
			switch(event->key.code)
			{
			case KEY_UP:
				selected--;
				if (selected < 0)
					selected = items.getCount() - 1;
				if (selected < 0)
					selected = 0;
				deferredAction = GoUp;
				return deferredAction;
			break;
			case KEY_DOWN:
				selected++;
				if (selected > items.getCount() - 1)
					selected = 0;
				deferredAction = GoDown;
				return deferredAction;
			break;
            case KEY_LEFT:
				deferredAction = ToggleLeft;
				return deferredAction;
                break;
            case KEY_RIGHT:
				deferredAction = ToggleRight;
				return deferredAction;
                break;
            case KEY_EXIT:
			case KEY_BRAKE:
				if (!topLevelMenu)
				{
					swooshDirection = 1;
					deferredAction = GoBack;
					return deferredAction;
				}
			break;
			case KEY_SELECT:
			case KEY_THRUST:
				swooshDirection = -1;
				deferredAction = Select;
				return deferredAction;
			break;
			}
		}
		break;
		case Game::Event::KeyReleaseEvent:
		break;
	}
	return NoAction;
}

MenuItem *Menu::getSelection() const
{
	if (selected <= items.getCount() - 1)
		return items.getItem(selected);
	return NULL;
}

Menu::Action Menu::getAction()
{
	const int swooshLimit = 512;
	if (!swooshDirection || (swooshAmount < -swooshLimit || swooshAmount > swooshLimit))
	{
		swooshDirection = 0;
		Action a = deferredAction;
		deferredAction = NoAction;
		return a;
	}
	return NoAction;
}

void Menu::setTopClipping(int _clipMinY)
{
	clipMinY = _clipMinY;
}

void Menu::setTopLevelMenu(bool topLevel)
{
	topLevelMenu = topLevel;
}

int Menu::getSwooshAmount() const
{
	return swooshAmount;
}

int Menu::getSelectionRectangleError() const
{
	int dist = selectionBarY - selectionBarTargetY;
	
	if (dist < 0)
		dist = -dist;
		
	return dist;
}

MenuItem *Menu::getItem(unsigned int index) const
{
	if (index <= items.getCount() - 1)
		return items.getItem(index);
	return NULL;
}

int Menu::getItemCount() const
{
	return items.getCount();
}

int Menu::getSelectionIndex() const
{
	return selected;
}


