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

#include "Environment.h"
#include "Track.h"
#include "BitmapFont.h"
#include "Mixer.h"
#include "ModPlayer.h"
#include "Car.h"
#include "Menu.h"

//Environment::Environment(Object *parent, Game::Framework *_framework, Game::Surface *_screen, View *_view):
Environment::Environment(Object *parent, Game::Framework *_framework, Game::Surface *_screen):
        Object(parent),
        framework(_framework),
        screen(_screen),
        rasterizer(0),
        view(0),
        track(0),
        font(0),
        mixer(0),
        modplayer(0),
        menu(0),
        texturePool(0, true)
{
        Game::Surface *img;
        
        // init the screen
        rasterizer = new Rasterizer(screen);
        view = new View(rasterizer);
        
        // load the fonts
        img = loadImage("fonts/default.png");
        texturePool.add(img);
        font = new BitmapFont(img);

        img = loadImage("fonts/big.png");
        texturePool.add(img);
        bigFont = new BitmapFont(img);
                
        // create the world
        world = new World(this, this);
        world->getRenderableSet().add(&meshPool);

        // create the track
        track = new Track(this, this);
        
        // create the menu
        menu = new Menu(this, this);
}

void Environment::initializeSound(Game::SampleChunk *sample)
{
        mixer = new Mixer(sample->rate, 8);
        modplayer = new ModPlayer(mixer);
}

Environment::~Environment()
{
        delete view;
        delete rasterizer;
        delete world;

        delete mixer;
        delete modplayer;
        
        delete font;
        delete bigFont;
        
        delete track;
        delete menu;
}

Game::Surface *Environment::loadImage(const char *name)
{
        Game::Surface *img = framework->loadImage(framework->findResource(name), &screen->format);
        
        return img;
}

