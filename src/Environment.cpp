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
#include "TagFile.h"
#include <string.h>

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
        aiCount(3),
        sfxVolume(16), musicVolume(16),
        musicChangeScheduled(false),
        stopSfxScheduled(false),
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

    // clear the player name
    memset(playerName, ' ', sizeof(playerName));
    playerName[0] = 0;

    // load graphics
    carDot = loadImage("reddot.png");
    enemyCarDot = loadImage("bluedot.png");

    scheduledMusicName[0] = 0;

    loadSettings();
}

void Environment::initializeSound(Game::SampleChunk *sample)
{
    mixer = new Mixer(sample->rate, 8);
    modplayer = new ModPlayer(mixer);

    setVolumes();
}

void Environment::muteSoundEffects(bool mute)
{
    if (mixer)
    {
        if (mute)
        {
            getEngineSoundChannel()->setVolume(0);
            getSfxChannel()->setVolume(0);
        }
        else
        {
            getEngineSoundChannel()->setVolume(sfxVolume);
            getSfxChannel()->setVolume(sfxVolume);
        }
    }
}

Environment::~Environment()
{
    doScheduledAudioEvents();
    saveSettings();

    delete view;
    delete rasterizer;
    delete world;

    delete modplayer;
    delete mixer;

    delete font;
    delete bigFont;

    delete track;
    delete menu;

    delete carDot;
    delete enemyCarDot;
}

Game::Surface *Environment::loadImage(const char *name)
{
    const char *resName = framework->findResource(name);

    if (resName && strlen(resName))
    {
        Game::Surface *img = framework->loadImage(resName, &screen->format);
        return img;
    }
    return NULL;
}

Channel *Environment::getEngineSoundChannel() const
{
    if (mixer)
        return mixer->getChannel(4);
    return NULL;
}

Channel *Environment::getSfxChannel() const
{
    if (mixer)
        return mixer->getChannel(5);
    return NULL;
}

void Environment::stopSoundEffects()
{
    stopSfxScheduled = true;
}

void Environment::loadSettings()
{
    TagFile file(framework->findResource("settings.tag"));

    while(1) switch(file.readTag())
        {
        case 0: // player name
            file.readData((unsigned char*)playerName, sizeof(playerName));
            break;
        case 1: // sfx volume
            file.readData((unsigned char*)&sfxVolume, sizeof(sfxVolume));
            break;
        case 2: // music volume
            file.readData((unsigned char*)&musicVolume, sizeof(musicVolume));
            break;
        case 3: // ai count
            file.readData((unsigned char*)&aiCount, sizeof(aiCount));
            break;
        default:
            return;
        }

    setVolumes();
}

void Environment::setVolumes()
{
    setSfxVolume(sfxVolume);
    setMusicVolume(musicVolume);
}

void Environment::saveSettings()
{
    WriteTagFile file(framework->findResource("settings.tag", false));

    file.writeTag(0, (unsigned char*)playerName, sizeof(playerName));
    file.writeTag(1, (unsigned char*)&sfxVolume, sizeof(sfxVolume));
    file.writeTag(2, (unsigned char*)&musicVolume, sizeof(musicVolume));
    file.writeTag(3, (unsigned char*)&aiCount, sizeof(aiCount));
}

void Environment::scheduleMusicChange(const char *name)
{
    if (mixer && !musicChangeScheduled && name)
    {
        // don't schedule the music if we're already playing it
        if (strcmp(scheduledMusicName, name))
        {
            strncpy(scheduledMusicName, name, sizeof(scheduledMusicName));
            musicChangeScheduled = true;
        }
    }
}

void Environment::renderAudio(Game::SampleChunk* sample)
{
    doScheduledAudioEvents();
    mixer->render(sample);
}

void Environment::stopMusic()
{
    scheduleMusicChange("");
}

void Environment::scheduleSampleDeletion(Game::SampleChunk *sample)
{
    if (sample)
        sampleDeletionQueue.add(sample);
}

int Environment::getSfxVolume() const
{
    return sfxVolume;
}

int Environment::getMusicVolume() const
{
    return musicVolume;
}

void Environment::setSfxVolume(int v)
{
    if (v < 0) v = 0;
    if (v > 64) v = 64;
    sfxVolume = v;

    if (mixer)
    {
        getEngineSoundChannel()->setVolume(sfxVolume);
        getSfxChannel()->setVolume(sfxVolume);
    }
}

void Environment::setMusicVolume(int v)
{
    if (v < 0) v = 0;
    if (v > 64) v = 64;
    musicVolume = v;

    if (mixer)
    {
        modplayer->setVolume(musicVolume);
    }
}

void Environment::doScheduledAudioEvents()
{
    if (!mixer)
        return;
        
    if (musicChangeScheduled)
    {
        modplayer->stop();
        if (strlen(scheduledMusicName))
        {
            modplayer->load(scheduledMusicName);
            modplayer->play();
        }
        musicChangeScheduled = false;
    }

    if (sampleDeletionQueue.getCount())
    {
        int i;
        for(i=0; i<sampleDeletionQueue.getCount(); i++)
            delete(sampleDeletionQueue.getItem(i));
        sampleDeletionQueue.clear();
    }

    if (stopSfxScheduled)
    {
        getEngineSoundChannel()->stop();
        getSfxChannel()->stop();
        stopSfxScheduled = false;
    }
}

int Environment::getTimeInMs() const
{
    return 1000 * framework->getTickCount() / framework->getTicksPerSecond();
}

int Environment::getAiCount() const
{
    return aiCount;
}

void Environment::setAiCount(int c)
{
    if (c < 0) c = 0;
    if (c > 7) c = 7;
    aiCount = c;
}
