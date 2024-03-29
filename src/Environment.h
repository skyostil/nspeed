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

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "engine/Engine.h"
#include "Car.h"
#include "Set.h"
#include "Object.h"
#include "MeshSet.h"
#include "Config.h"

#ifdef EPOC
#include <e32keys.h>
#define KEY_LEFT        EStdKeyLeftArrow
#define KEY_RIGHT       EStdKeyRightArrow
#define KEY_UP          EStdKeyUpArrow
#define KEY_SELECT      EStdKeyDevice3
#define KEY_THRUST      '5'
#define KEY_BRAKE       '7'
#define KEY_DOWN        EStdKeyDownArrow
#define KEY_EXIT        EStdKeyDevice0
#define KEY_ROTATE      EStdKeyDevice1
#else
#include <SDL/SDL.h>
#define KEY_LEFT        SDLK_LEFT
#define KEY_RIGHT       SDLK_RIGHT
#define KEY_UP          SDLK_UP
#define KEY_THRUST      'a'
#define KEY_BRAKE       'z'
#define KEY_THRUST2     SDLK_LSHIFT     /* both shift keys are identical on N810 */
#define KEY_BRAKE2      SDLK_RCTRL
#if (MAEMO_VERSION <= 4)
#define KEY_THRUST3     SDLK_F7
#define KEY_BRAKE3      SDLK_F8
#define KEY_SELECT      SDLK_RETURN
#else
#define KEY_SELECT      SDLK_KP_ENTER
#define KEY_THRUST3     KEY_THRUST2
#define KEY_BRAKE3      KEY_BRAKE2
#endif
#define KEY_DOWN        SDLK_DOWN
#define KEY_EXIT        SDLK_ESCAPE
#define KEY_ROTATE      'r'
#endif

#define BUTTON_WIDTH    35
#define BUTTON_HEIGHT   18

class Track;
class BitmapFont;
class Mixer;
class ModPlayer;
class View;
class Rasterizer;
class Menu;
class Channel;

//! This class contains all the global game related data
class Environment: public Object
{
public:
    Environment(Object *parent, Game::Framework *_framework, Game::Surface *_screen);
    ~Environment();

    //! Loads an image. Don't forget to delete it!
    Game::Surface          *loadImage(const char *name);

    Game::Surface           *getScreen() { return screen; }
    Game::Framework         *getFramework() { return framework; }
    Rasterizer              *getRasterizer() { return rasterizer; }
    View                    *getView() { return view; }
    World                   *getWorld() { return world; }
    Menu                    *getMenu() { return menu; }

    void                    initializeSound(Game::SampleChunk *sample);
    void                    stopSoundEffects();
    void                    muteSoundEffects(bool mute);
    void                    scheduleMusicChange(const char *name);
    void                    scheduleSampleDeletion(Game::SampleChunk *sample);
    void                    stopMusic();
    void                    renderAudio(Game::SampleChunk* sample);

    int                     getSfxVolume() const;
    int                     getMusicVolume() const;
    void                    setSfxVolume(int v);
    void                    setMusicVolume(int v);
    Channel                 *getEngineSoundChannel() const;
    Channel                 *getSfxChannel() const;
    int                     getAiCount() const;
    void                    setAiCount(int c);

    Track                   *track;
    BitmapFont              *font, *bigFont;
    Game::Surface           *carDot, *enemyCarDot;
    Game::Surface           *menuButton, *taskSwitcherButton, *backButton;
	Game::SampleChunk		*damageSound, *rechargeSound;

    Mixer                   *mixer;
    ModPlayer               *modplayer;

    MeshSet                 meshPool;
    Set<Game::Surface*>     texturePool;
    Set<Car*>               carPool;
    
    char                    playerName[12];
    
    int                     getTimeInMs() const;

private:
    void                    loadSettings();
    void                    saveSettings();
    void                    doScheduledAudioEvents();
    void                    setVolumes();

    Game::Surface           *fontImage;
    Menu                    *menu;

    Game::Surface           *screen;
    Game::Framework         *framework;
    Rasterizer              *rasterizer;
    View                    *view;
    World                   *world;

    Set<Game::SampleChunk*> sampleDeletionQueue;
    char                    scheduledMusicName[256];
    bool                    musicChangeScheduled; // use a mutex for these
    bool                    stopSfxScheduled;
    
    int                     sfxVolume, musicVolume, aiCount;
};

#endif
