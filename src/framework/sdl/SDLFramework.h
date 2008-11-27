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

#ifndef SDLFRAMEWORK_H
#define SDLFRAMEWORK_H

#define USE_SDL_IMAGE
#define USE_WAVE_LOADER

#include "engine/Engine.h"
#include <SDL/SDL.h>
#ifdef USE_SDL_IMAGE
#include <SDL/SDL_image.h>
#endif


extern "C"
{
    extern int main(int argc, char **argv);
};

class SDLFramework: public Game::Framework
{
public:
    SDLFramework();
    ~SDLFramework();

    int run(int argc, char **argv);

    // framework services
    void exit();
    unsigned int getTickCount();
    unsigned int getTicksPerSecond();

#ifdef USE_SDL_IMAGE
    Game::Surface *loadImage(const char *name, Game::PixelFormat *pf = NULL);
#endif

#ifdef USE_WAVE_LOADER
    Game::SampleChunk *loadSample(const char *name, Game::SampleFormat *sf = NULL);
#endif

    const char *findResource(const char *name, bool mustExist = false);

protected:
    void printUsage();
    void upscale(SDL_Surface* target, const Game::Surface* source, int scaleFactor);

    static void audioCallback(void *userdata, Uint8 *stream, int len);

    SDL_Surface                 *screen;
    Game::Surface               *gameScreen;
    Game::SampleChunk           *gameAudio;
    Game::Engine		*engine;
    char                        dataDir[512];
    char                        resourcePath[512];
    bool                        done;
};

#endif

