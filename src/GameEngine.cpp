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

#define RACECOUNTDOWN   256
#define AICOUNT         3

GameEngine::GameEngine(Game::Framework* _framework):
        Object(0),
        Game::Engine(_framework),
        framework(_framework),
        env(0),
        time(0),
        logo(0),
        lastTime(0),
        playerNameIndex(0),
        madeRecord(false),
        state(IdleState), oldState(IdleState),
        fpsCountStart(0),
        frameCount(0),
        selectedTrackIndex(0),
        raceCountDown(0),
        playerNameCounter(0),
        rotateCamera(false),
        menuItemPractice("Single Race"),
        menuItemSettings("Settings"),
        menuItemQuit("Quit"),
        menuItemContinue("Continue"),
        menuItemRestart("Restart Race"),
        menuItemMainMenu("Main Menu"),
        menuItemSfxVolume(""),
        menuItemMusicVolume("")
{
    debugMessage[0] = 0;
    selectedCar[0] = 0;
    selectedTrack[0] = 0;
    allCars.setAutoDelete(true);
    allTracks.setAutoDelete(true);
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
    scalar angle = rotateCamera?FPMod((time>>1), 2*PI):car->getAngle();
    scalar x = FPMul(FPCos(angle), FPInt(3)>>3);
    scalar z = FPMul(FPSin(angle), FPInt(3)>>3);
    //      scalar y = FPInt(2)>>2;
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

void GameEngine::rotateAroundPosition(const Vector &pos, scalar scale)
{
    scalar angle = FPMod((time>>1), 2*PI);
    scalar angle2 = FPMod((time), 2*PI);
    scalar x = FPMul(FPCos(angle), scale);
    scalar z = FPMul(FPSin(angle), scale);
    scalar y = FPMul(FPSin(angle2), scale>>2) + scale;

    env->getView()->camera.target = pos;
    env->getView()->camera.origin = pos + Vector(x,y,z);
    env->getView()->camera.update();
}

void GameEngine::renderVideo(Game::Surface* screen)
{
    char text[64];
    Menu *menu = env->getMenu();
    World *world = env->getWorld();
    Game::Pixel16 textMask = ((1<<screen->format.rsize)-1) << screen->format.rshift;
    BitmapFont *font = env->font;
    int x, y, i;

    if (state == IdleState)
    {
        setState(LoadingState);
    }

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
    case SettingsMenuState:
        screen->clear(0);

        sprintf(text, "Effects Volume: %d%%", (100*env->getSfxVolume())/64);
        menuItemSfxVolume.setText(text);
        sprintf(text, "Music Volume: %d%%", (100*env->getMusicVolume())/64);
        menuItemMusicVolume.setText(text);
        menu->render(world);

        renderTitle(screen, "Settings");

        break;
    case ChooseCarState:
        screen->clear(0);

        if (allCars.getCount())
        {
            scalar speed = 0;
            int x, y, d;
            Car *car = allCars.getItem(menu->getSelectionIndex());
            Game::Pixel16 pixel;

            for(x=4; x<screen->width-4; x++)
            {
                y = screen->height/3 - ((speed>>8) - (menu->getSelectionRectangleError()<<3));

                if (y > 0)
                {
                    int r = 0, g = 0, b = 0;

                    if (x % 8 == 0)
                    {
                        r = 63;
                    }

                    for(d=screen->height/3 ;d > y; d--)
                    {
                        r+=4;

                        if (r>255) r=255;
                        if (g>255) g=255;
                        if (b>255) b=255;

                        if (d % 8 == 0)
                        {
                            int rr = r + 63;
                            pixel = screen->format.makePixel(rr<255?rr:255,g,b);
                        }
                        else
                            pixel = screen->format.makePixel(r,g,b);
                        screen->setPixel(x, d, pixel);
                    }
                }

                for(d=0; d<3; d++)
                    speed += car->getAcceleration(speed);
            }

            rotateAroundCar(car);
            world->render();
        }

        renderTitle(screen, "Choose Car");
        menu->render(world);
        break;
    case ChooseTrackState:
        screen->clear(0);

        if (allTracks.getCount())
        {
            rotateAroundPosition(Vector(0,0,0));

            Track *track = allTracks.getItem(menu->getSelectionIndex());
            if (track)
            {
                Game::Surface *map = track->getMap();

                if (map)
                    renderRotatingQuad(env->getView(), map);

                y = 32;

                sprintf(text, "Best lap:");
                font->renderText(env->getScreen(), text, 4, y, textMask);

                i = track->getBestLapTime(text, sizeof(text));

                if (i != -1)
                {
                    font->renderText(env->getScreen(), text, 108, y + font->getHeight() + 2);

                    formatTime(text, i);
                    font->renderText(env->getScreen(), text, 108, y);
                    y += font->getHeight() + 2;
                }

                y += font->getHeight() + 2;

                sprintf(text, "Best race:");
                font->renderText(env->getScreen(), text, 4, y, textMask);

                i = track->getBestTotalTime(text, sizeof(text));

                if (i != -1)
                {
                    font->renderText(env->getScreen(), text, 108, y + font->getHeight() + 2);

                    formatTime(text, i);
                    font->renderText(env->getScreen(), text, 108, y);
                    y += font->getHeight() + 2;
                }
            }
        }
        renderTitle(screen, "Choose Track");
        menu->render(world);
        break;
    case RaceLoadingState:
        screen->clear(0);
        sprintf(text, "Loading...");
        env->bigFont->renderText(
            screen, text,
            screen->width/2 - env->bigFont->getTextWidth(text)/2,
            screen->height/2 - env->bigFont->getHeight());

        if (framework->getTickCount() - stateChangeTime > 1)
            setState(RaceIntroState);
        break;
    case RaceIntroState:
        rotateAroundPosition(env->track->getStartingPosition(0), FPInt(1));
        env->getWorld()->render();
        menu->dimScreen(screen, 0, env->bigFont->getHeight()+2);
        renderTitle(screen, selectedTrack);
        break;
    case RaceCountDownState:
    case RaceState:
    case RaceMenuState:
    case RaceOutroState:
        {
            Car *playerCar = env->carPool.getItem(0);

            // render the world
            if (state == RaceOutroState)
                rotateAroundPosition(playerCar->getOrigin(), FPInt(1)>>1);
            else
                lookAtCarFromBehind(playerCar);

            env->getWorld()->render();

            // render the map & OSD
            if (state == RaceState)
            {
                renderOSD(screen);
            }

            if (playerCar->getEnergy() <= 0)
            {
                renderStatic(screen);
                env->getMenu()->dimScreen(screen, 0, env->bigFont->getHeight()+2);
                renderTitle(screen, "Wrecked!");
            }
            else if (playerCar->isTakingDamage())
            {
                renderDamage(screen);
            }

            if (state == RaceCountDownState || raceCountDown > 0)
            {
                if (raceCountDown > 3*(RACECOUNTDOWN/4))
                    strncpy(text, "3", sizeof(text));
                else if (raceCountDown > 2*(RACECOUNTDOWN/4))
                    strncpy(text, "2", sizeof(text));
                else if (raceCountDown > 1*(RACECOUNTDOWN/4))
                    strncpy(text, "1", sizeof(text));
                else
                {
                    // start the clocks
                    for(i=0; i<env->carPool.getCount(); i++)
                        env->carPool.getItem(i)->startRace();

                    setState(RaceState);
                    strncpy(text, "go!!", sizeof(text));
                }

                env->bigFont->renderText(env->getScreen(), text, screen->width/2 - env->bigFont->getTextWidth(text), screen->height/2 - env->bigFont->getHeight());
            }

            if (state == RaceOutroState)
            {
                renderLapTimes(screen);
                if (madeRecord)
                {
                    renderTextEntryDialog(screen, env->playerName, sizeof(env->playerName), playerNameIndex);
                }
            }

            if (state == RaceMenuState)
            {
                menu->render(world);
            }
        }
        break;
    }

    //        env->font->renderText(screen, debugMessage, 1, 1);
}

void GameEngine::renderAudio(Game::SampleChunk* sample)
{
    env->renderAudio(sample);
}

void GameEngine::setState(State newState)
{
    int i;
    Menu *menu = env->getMenu();
    Set<Renderable*> *renderableSet = &env->getWorld()->getRenderableSet();
    Game::Surface *screen = env->getScreen();

    if (state == newState) return;

    oldState = state;

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
        renderableSet->remove(&env->meshPool);
        break;
    case ChooseTrackState:
        break;
    case RaceState:
        break;
    case RaceMenuState:
        {
            // fix lap and race times
            int delta = framework->getTickCount() - stateChangeTime;
        
            for(i=0; i<env->carPool.getCount(); i++)
                env->carPool.getItem(i)->adjustTimes(delta);

            env->muteSoundEffects(false);
            break;
        }
    }

    stateChangeTime = framework->getTickCount();

    // set new state
    state = newState;
    switch(newState)
    {
    case LoadingState:
        // load all cars
        menu->clear();
        fillMenuWithDirectories(menu, framework->findResource("cars"));

        allCars.clear();
        for(i=0; i<menu->getItemCount(); i++)
        {
            Car *car = new Car(env->getWorld(), menu->getItem(i)->getText());
            if (car)
                allCars.add(car);
        }

        // load all track maps
        menu->clear();
        fillMenuWithDirectories(menu, framework->findResource("tracks"));

        allTracks.clear();
        for(i=0; i<menu->getItemCount(); i++)
        {
            Track *track = new Track(this, env);

            if (track)
            {
                track->load(menu->getItem(i)->getText(), 1, true);
                allTracks.add(track);
            }
            else
                allTracks.add(0);
        }

        setState(MainMenuState);
        break;
    case MainMenuState:
        menu->clear();
        menu->addItem(&menuItemPractice);
        menu->addItem(&menuItemSettings);
        menu->addItem(&menuItemQuit);
        menu->setTopLevelMenu(true);

        env->track->unload();
        env->stopSoundEffects();

        if (oldState != ChooseCarState && oldState != SettingsMenuState)
        {
            env->scheduleMusicChange(framework->findResource("music/menu.mod"));
        }

        for(i=0; i<env->carPool.getCount(); i++)
        {
            delete env->carPool.getItem(i);
        }
        env->carPool.clear();

        renderableSet->remove(env->track);
        renderableSet->remove(&env->meshPool);

        logo = env->loadImage("logo.png");
        break;
    case SettingsMenuState:
        menu->clear();
        menu->addItem(&menuItemSfxVolume);
        menu->addItem(&menuItemMusicVolume);
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
    case RaceLoadingState:
        env->stopMusic();
        env->muteSoundEffects(true);
        //                setState(RaceCountDownState);
        break;
    case RaceIntroState:
        if (!env->track->load(selectedTrack))
        {
            setState(MainMenuState);
            return;
        }

        spawnCars();

        char song[128];
        sprintf(song, "tracks/%.32s/music.mod", selectedTrack);
        env->scheduleMusicChange(framework->findResource(song));

        renderableSet->add(env->track);
        renderableSet->add(&env->meshPool);
        break;
    case RaceCountDownState:
        env->muteSoundEffects(false);
        for(i=0; i<env->carPool.getCount(); i++)
            env->carPool.getItem(i)->prepareForRace(i);
        raceCountDown = RACECOUNTDOWN;
        preventWarping();
        break;
    case RaceState:
        break;
    case RaceOutroState:
        env->muteSoundEffects(true);
        playerNameIndex = 0;
        madeRecord = false;
    break;
    case QuitState:
        framework->exit();
        break;
    case RaceMenuState:
        env->muteSoundEffects(true);
        menu->clear();
        menu->addItem(&menuItemContinue);
        menu->addItem(&menuItemRestart);
        menu->addItem(&menuItemSettings);
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
            setState(oldState);
            break;
        case SettingsMenuState:
            setState(oldState);
            break;
        }
        break;
    case Menu::Select:
        switch(state)
        {
        case MainMenuState:
            if (menu->getSelection() == &menuItemPractice)
                setState(ChooseCarState);
            else if (menu->getSelection() == &menuItemSettings)
                setState(SettingsMenuState);
            else if (menu->getSelection() == &menuItemQuit)
                setState(QuitState);
            break;
        case ChooseCarState:
            copySelectedMenuItem(menu, selectedCar, sizeof(selectedCar));
            setState(ChooseTrackState);
            break;
        case ChooseTrackState:
            copySelectedMenuItem(menu, selectedTrack, sizeof(selectedTrack));
            selectedTrackIndex = menu->getSelectionIndex();
            setState(RaceLoadingState);
            break;
        case RaceMenuState:
            if (menu->getSelection() == &menuItemRestart)
                setState(RaceCountDownState);
            else if (menu->getSelection() == &menuItemContinue)
                setState(RaceState);
            else if (menu->getSelection() == &menuItemSettings)
                setState(SettingsMenuState);
            else if (menu->getSelection() == &menuItemMainMenu)
                setState(MainMenuState);
            break;
        case SettingsMenuState:
            setState(oldState);
            break;
        }
        break;
    case Menu::ToggleLeft:
        switch(state)
        {
        case SettingsMenuState:
            if (menu->getSelection() == &menuItemSfxVolume)
                env->setSfxVolume(env->getSfxVolume() - 8);
            else if (menu->getSelection() == &menuItemMusicVolume)
                env->setMusicVolume(env->getMusicVolume() - 8);
            break;
        }
        break;
    case Menu::ToggleRight:
        switch(state)
        {
        case SettingsMenuState:
            if (menu->getSelection() == &menuItemSfxVolume)
                env->setSfxVolume(env->getSfxVolume() + 8);
            else if (menu->getSelection() == &menuItemMusicVolume)
                env->setMusicVolume(env->getMusicVolume() + 8);
            break;
        }
        break;
    case Menu::GoUp:
    case Menu::GoDown:
        switch(state)
        {
        case ChooseCarState:
            break;
        case ChooseTrackState:
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
    case RaceIntroState:
        if (event->type == Game::Event::KeyPressEvent && 
            (event->key.code == KEY_SELECT || event->key.code == KEY_THRUST)
            )
            setState(RaceCountDownState);
        break;
    case RaceCountDownState:
    case RaceState:
        handleRaceEvent(event);
        break;
    case RaceOutroState:
        handleRaceOutroEvent(event);
        break;
    }
}
                
void GameEngine::handleRaceOutroEvent(Game::Event* event)
{
    Car *car = env->carPool.getItem(0);

    switch(event->type)
    {
    case Game::Event::KeyPressEvent:
        {
            switch(event->key.code)
            {
            case KEY_SELECT:
            case KEY_THRUST:
            case KEY_EXIT:
                {            
                    int l = car->getBestLapTime();
                    int r = car->getRaceTime();
                
                    if (l != -1 && r != -1)
                    {
                        if (l < env->track->getBestLapTime() || env->track->getBestLapTime() == -1)
                        {
                            Track *metaTrack = allTracks.getItem(selectedTrackIndex);
                            env->track->setBestLapTime(l, env->playerName);
                            metaTrack->setBestLapTime(l, env->playerName);
                        }
                        if (r < env->track->getBestTotalTime() || env->track->getBestTotalTime() == -1)
                        {
                            Track *metaTrack = allTracks.getItem(selectedTrackIndex);
                            env->track->setBestTotalTime(r, env->playerName);
                            metaTrack->setBestTotalTime(r, env->playerName);
                        }
                    }
                
                    env->track->saveTimes(selectedTrack);
                    setState(RaceMenuState);
                    break;
                }
            case KEY_LEFT:
                playerNameIndex--;
                if (playerNameIndex < 0)
                    playerNameIndex = sizeof(env->playerName)-1;
                break;
            case KEY_RIGHT:
                playerNameIndex++;
                if (playerNameIndex > sizeof(env->playerName)-1)
                    playerNameIndex = 0;
                break;
            case KEY_UP:
                buttonUp = true;
                break;
            case KEY_DOWN:
                buttonDown = true;
                break;
            }
        }
        break;
   case Game::Event::KeyReleaseEvent:
        {
            switch(event->key.code)
            {
            case KEY_UP:
                buttonUp = false;
                break;
            case KEY_DOWN:
                buttonDown = false;
                break;
            }
        }
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
        /*
                        Car *car = env->carPool.getItem(0);
                        sprintf(debugMessage, "%d fps, %d s, g %d, s %d", frameCount*4, framework->getTickCount() / framework->getTicksPerSecond(), car?car->getGateIndex():-1, state);
        */
        fpsCountStart = framework->getTickCount();
        frameCount = 0;
    }
    else
        frameCount++;

    if ((time - lastTime) >= timestep)
    {
        if ((time - lastTime) < 64 * timestep)
        {
            while((time - lastTime) >= timestep)
            {
                atomicStep();
                lastTime+=timestep;
            }
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
        {
            Car *car = env->carPool.getItem(0);
            if (car && car->hasFinished())
            {
                setState(RaceOutroState);
            }
            // no break here
        }
    case RaceOutroState:
        {
            for(i=env->carPool.begin(); i!=env->carPool.end(); i++)
                (*i)->update(env->track);
                
            if (madeRecord)
            {
                if (++playerNameCounter > 16)
                {
                    char *c = &env->playerName[playerNameIndex];
                    
                    playerNameCounter = 0;
                    
                    if (buttonUp)
                    {
                        (*c)++;
                        
                        if (*c > 126)
                            *c = 46;
                    }
                    
                    if (buttonDown)
                    {
                        (*c)--;
                        
                        if (*c < 46)
                            *c = 126;
                    }
                }
            }
        }
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

            if (entry->d_name[0] == '.')
                continue;

#ifdef EPOC
            // stat() bug -> use opendir
            DIR *tmpDir;
            sprintf(fullName, "%.128s\\%.64s", path, entry->d_name);
            tmpDir = opendir(fullName);
            if (!tmpDir)
                continue;
            else
                closedir(tmpDir);
#else
            sprintf(fullName, "%.128s/%.64s", path, entry->d_name);

            if (stat(fullName, &entryStat) != 0)
                continue;

            if (!S_ISDIR(entryStat.st_mode))
                continue;
#endif

            MenuItem *item = new MenuItem(entry->d_name);
            menuItemList.add(item);
            menu->addItem(item);
        }
        closedir(dir);
    }
}

void GameEngine::preventWarping()
{
    lastTime = time;
}

void GameEngine::renderTitle(Game::Surface *s, const char *title)
{
    env->bigFont->renderText(s, title, 4, 2);
}

void GameEngine::renderRotatingQuad(View *view, Game::Surface *texture)
{
    const scalar scale = FPInt(4);
    const scalar depth = FPInt(-1);
    Vector center = view->camera.target;
    Vector v0, v1, v2, v3;

    v0.set(-scale + center.x, depth, -scale + center.z);
    v1.set( scale + center.x, depth, -scale + center.z);
    v2.set( scale + center.x, depth,  scale + center.z);
    v3.set(-scale + center.x, depth,  scale + center.z);

    view->rasterizer->flags = Rasterizer::FlagPerspectiveCorrection;
    view->rasterizer->setTexture(texture);

    view->beginPolygon();
    view->setTexCoord(FPInt(0), FPInt(0));
    view->addVertex(v0);
    view->setTexCoord(FPInt(128), FPInt(0));
    view->addVertex(v1);
    view->setTexCoord(FPInt(128), FPInt(128));
    view->addVertex(v2);
    view->setTexCoord(FPInt(0), FPInt(128));
    view->addVertex(v3);
    view->endPolygon();
}

void GameEngine::renderEnergyBar(Game::Surface *screen, int energy, int x, int y, int h) const
{
    Game::Pixel16 mask =
        (screen->format.rmask & (screen->format.rmask>>1)) +
        (screen->format.gmask & (screen->format.gmask>>1)) +
        (screen->format.bmask & (screen->format.bmask>>1));
    Game::Pixel16 border = screen->format.makePixel(0,0,0);

    const int scale = 1;
    int c = 31;
    const int r = 1, g = 0, b = 0;
    int w = energy>>scale;
    int fullWidth = 100>>scale;
    int px, py;

    for(py = y+1; py<=y+h-1; py++)
    {
        Game::Pixel16 *p = ((Game::Pixel16*)screen->pixels) + (py * screen->width) + x + 1;
        for(px=x+1; px<=x+w-1; px++)
        {
            (*p) = (((*p)>>1) & mask) + ((1<<(screen->format.rsize-1)) << screen->format.rshift);
            p++;
        }
    }

    for(px = x; px <= x+fullWidth; px++)
    {
        screen->setPixel(px, y, border);
        screen->setPixel(px, y+h, border);
    }

    for(py = y; py < y+h; py++)
    {
        screen->setPixel(x, py, border);
        screen->setPixel(x+fullWidth, py, border);
    }
}

void GameEngine::renderStatic(Game::Surface *screen)
{
    Game::Pixel16 mask =
        (screen->format.rmask & (screen->format.rmask>>1)) +
        (screen->format.gmask & (screen->format.gmask>>1)) +
        (screen->format.bmask & (screen->format.bmask>>1));
    Game::Pixel16 *p = (Game::Pixel16*)screen->pixels;
    int count = screen->width * screen->height;
    int q = framework->getTickCount();

    while(count--)
    {
        (*p) = (((*p)>>1) & mask) +
               ((q & ((1<<(screen->format.rsize-1))-1)) << screen->format.rshift) +
               ((q & ((1<<(screen->format.gsize-1))-1)) << screen->format.gshift) +
               ((q & ((1<<(screen->format.bsize-1))-1)) << screen->format.bshift);
        q ^= 52135123;
        q += (q>>1);
        q += 13421;
        p++;
    }
}

void GameEngine::renderDamage(Game::Surface *screen)
{
    Game::Pixel16 *p = (Game::Pixel16*)screen->pixels;
    Game::Pixel16 mask =
        (screen->format.rmask & (screen->format.rmask>>1)) +
        (screen->format.gmask & (screen->format.gmask>>1)) +
        (screen->format.bmask & (screen->format.bmask>>1));
    int count = screen->width * screen->height;
    int r, q = framework->getTickCount();
    int x, y;

    for(y=0; y<screen->height; y++)
    {
        r = (q & 7) - 4;

        if (r < 0)
        {
            r = -r;
            for(x=screen->width-1; x>=r; x--)
                p[x] = p[x-r];
        }
        else
        {
            if (r)
                for(x=0; x<screen->width-r; x++)
                    p[x] = p[x+r];
        }

        q ^= 13512301;
        q += (q>>5);
        q += 13421;

        p+=screen->width;
    }
}

Game::Pixel16 GameEngine::getEmphasisColor(Game::Surface *screen) const
{
    return ((1<<screen->format.rsize)-1) << screen->format.rshift;
}

void GameEngine::renderLapTimes(Game::Surface *screen)
{
    Car *car = env->carPool.getItem(0);
    BitmapFont *font = env->font;
    int l = car->getBestLapTime();
    int r = car->getRaceTime();
    int y = 64;
    char text[64];
    Game::Pixel16 recordMask = getEmphasisColor(screen);
    Game::Pixel16 normalMask = (Game::Pixel16)-1;
    Game::Pixel16 mask;
    Track *track = env->track;
    
    // blink the record texts
    if (((time>>(FP-6)) & 15) < 8)
        recordMask = normalMask;

    env->getMenu()->dimScreen(screen, y-2, y+2*(font->getHeight()+2)+2);

    sprintf(text, "Best lap:");
    font->renderText(env->getScreen(), text, 4, y);

    if (l != -1)
    {
        formatTime(text, l);
        if (l < track->getBestLapTime() || track->getBestLapTime()==-1)
        {
            mask = recordMask;
            madeRecord = true;
        }
        else
            mask = normalMask;
        font->renderText(env->getScreen(), text, 108, y, mask);
    }

    y += font->getHeight() + 2;

    sprintf(text, "Total:");
    font->renderText(env->getScreen(), text, 4, y);

    if (r != -1)
    {
        formatTime(text, r);
        if (r < track->getBestTotalTime() || track->getBestTotalTime()==-1)
        {
            mask = recordMask;
            madeRecord = true;
        }
        else
            mask = normalMask;
        font->renderText(env->getScreen(), text, 108, y, mask);
    }

    sprintf(text, "Finished %d", car->getRank());
    
    switch(car->getRank())
    {
    case 1:
        strcat(text, "st!");
        break;
    case 2:
        strcat(text, "nd!");
        break;
    case 3:
        strcat(text, "rd!");
        break;
    default:
        strcat(text, "th!");
        break;
    }
    env->getMenu()->dimScreen(screen, 0, env->bigFont->getHeight()+2);
    renderTitle(screen, text);
}

void GameEngine::renderOSD(Game::Surface *screen)
{
    int x, y, i;
    Game::Surface *map = env->track->getMap();
    Car *car = env->carPool.getItem(0);
    BitmapFont *font = env->font;
    char text[64];

    // render map
    if (map)
    {
        int mapX = env->getScreen()->width - env->track->getMap()->width - 2;
        int mapY = env->getScreen()->height - env->track->getMap()->height - 2;
        env->getScreen()->renderTransparentSurface(env->track->getMap(), mapX, mapY);

        if (env->carDot && env->enemyCarDot)
        {
            for(i=0; i<env->carPool.getCount(); i++)
            {
                Vector origin = env->carPool.getItem(i)->getOrigin();

                x = mapX + map->width / 2 +   ((origin.x << 3) >> FP) / 6 - env->carDot->width / 2;
                y = mapY + map->height / 2  + ((origin.z << 3) >> FP) / 6 - env->carDot->height / 2;

                if (i == 0)
                    env->getScreen()->renderTransparentSurface(env->carDot, x, y);
                else
                    env->getScreen()->renderTransparentSurface(env->enemyCarDot, x, y);
            }
        }
    }

    x = 4;
    y = env->getScreen()->height - font->getHeight() - 2;
    scalar speed = car->getSpeed();

    if (speed <= (32<<2))
        speed = 0;

    // render speed
    sprintf(text, "%3d km/h", speed >> 5);
    font->renderText(env->getScreen(), text, x, y);

    // render lap
    y -= font->getHeight() - 2;
    sprintf(text, "lap %d of %d", car->getLapCount()+1, env->track->getLapCount());
    font->renderText(env->getScreen(), text, x, y);

    // render times
    i = car->getRaceTime();
    formatTime(text, i);
    font->renderText(env->getScreen(), text, 4, 4);

    i = car->getLapTime();
    formatTime(text, i);
    font->renderText(env->getScreen(), text, 4, 4 + font->getHeight());

    // render energy
    renderEnergyBar(screen, car->getEnergy(), screen->width - 54, 4, 8);
}

void GameEngine::renderTextEntryDialog(Game::Surface *screen, const char *text, unsigned int len, unsigned int selectedIndex)
{
    BitmapFont *font = env->font;
    int letterWidth = font->getTextWidth("M") + 1;
    int width = len * letterWidth;
    int x = screen->width / 2 - width / 2;
    int y = 2 * screen->height / 3;
    int i;
    char letter[2];
    Game::Pixel16 selectedMask = getEmphasisColor(screen);
    Game::Pixel16 normalMask = (Game::Pixel16)-1;

    env->getMenu()->dimScreen(screen, y-2, y+2*(font->getHeight()+2)+2);
        
    font->renderText(screen, "New Record! Enter name:", 2, y);
    
    y += font->getHeight() + 2;
    
    letter[1] = 0;
    for(i=0; i<len; i++)
    {
        letter[0] = (text[i]>32 && text[i]<126) ? text[i] : '.';
        font->renderText(screen, letter, x, y, i == selectedIndex ? selectedMask : normalMask);
        x += letterWidth;
    }
}

void GameEngine::formatTime(char *out, int milliseconds) const
{
    sprintf(out, "%02d:%02d'%02d", (milliseconds/1000)/60, (milliseconds/1000)%60, (milliseconds/10)%100);
}

void GameEngine::spawnCars()
{
    int i;
    Menu *menu = env->getMenu();
    int q = framework->getTickCount();

    env->carPool.clear();

    // player
    env->carPool.add(new Car(env->getWorld(), selectedCar));

    // get a list of all cars
    menu->clear();
    fillMenuWithDirectories(menu, framework->findResource("cars"));

    for(i=0; i<menu->getItemCount(); i++)
    {
        Car *car = new Car(env->getWorld(), menu->getItem(i)->getText());
        if (car)
            allCars.add(car);
    }

    // randomly select the AI cars
    for(i=0; i<AICOUNT; i++)
    {
        env->carPool.add(
            new Car(
                env->getWorld(), menu->getItem(q % menu->getItemCount())->getText()
                )
            );
        q ^= 52135123;
        q += (q>>1);
        q += 13421;
    }

    // reset all the cars to the starting grid
    for(i=0; i<env->carPool.getCount(); i++)
        env->carPool.getItem(i)->prepareForRace(i);
}

// bootstrap
extern "C"
{

    Game::Engine* CreateEngine(Game::Framework* framework, int argc, char **argv)
    {
        return new GameEngine(framework);
    }

}
