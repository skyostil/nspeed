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

#include "World.h"

World::World(Game::Framework *_framework, Game::Surface *_screen, Rasterizer *_rasterizer, View *_view, Environment *_env):
	framework(_framework),
	screen(_screen),
	rasterizer(_rasterizer),
	view(_view),
	env(_env),
	renderables(MAX_RENDERABLES)
{
}

World::~World()
{
}

void World::render()
{
	int i;

	for(i=0; i<renderables.getCount(); i++)
		renderables.getItem(i)->render(this);
}
