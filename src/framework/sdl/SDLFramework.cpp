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

#include "SDLFramework.h"
#include "engine/Engine.h"
#include "Config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

//extern "C"
//{
int main(int argc, char **argv)
{
    SDLFramework fw;

    return fw.run(argc, argv);
}
//}

SDLFramework::SDLFramework():
        screen(0),
        gameScreen(0),
        gameAudio(0),
        engine(0)
{}

SDLFramework::~SDLFramework()
{
    if (screen)
        SDL_FreeSurface(screen);

    delete gameScreen;
    delete gameAudio;
    delete engine;

    SDL_Quit();
}

void SDLFramework::printUsage()
{
    printf("Usage:\n"
           "-ns, --nosound          Disable audio\n"
           "-nv, --novideo          Disable video\n"
           "--xres n                Set horizontal resolution\n"
           "--yres n                Set vertical resolution\n"
           "--rate n                Set sound sampling rate\n"
           "--joynum n              Use joystick n\n"
           "--datadir path          Override path to data files [default: %s]\n"
           "-fs, --fullscreen       Use fullscreen mode\n"
           "--scale n               Render at 1/n resolution\n",
          dataDir);
}

int SDLFramework::run(int argc, char **argv)
{
    int i;
    bool useSound = true, useVideo = true, useFullscreen = false;
    int scaleFactor = 1;
    int xres = 320, yres = 240, rate = 22050;
    int joynum = 0;

    done = false;

#if defined(PREFIX) && defined(APPNAME)
    strncpy(dataDir, PREFIX "/share/games/" APPNAME, sizeof(dataDir));
#else
    strncpy(dataDir, "../data", sizeof(dataDir));
#endif

    for(i=1; i<argc; i++)
    {
        if (argv[i][0] != '-')
        {
            continue;
        }

        if (!strcmp(argv[i], "-ns") || !strcmp(argv[i], "--nosound"))
        {
            useSound = false;
        }
        else if (!strcmp(argv[i], "-nv") || !strcmp(argv[i], "--novideo"))
        {
            useVideo = false;
        }
        else if (!strcmp(argv[i], "-fs") || !strcmp(argv[i], "--fullscreen"))
        {
            useFullscreen = true;
        }
        else if (!strcmp(argv[i], "--scale"))
        {
            scaleFactor = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
        {
            printUsage();
            return 0;
        }
        else if (!strcmp(argv[i], "--xres"))
        {
            xres = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "--rate"))
        {
            rate = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "--yres"))
        {
            yres = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "--joynum"))
        {
            joynum = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "--datadir"))
        {
            strncpy(dataDir, argv[++i], sizeof(dataDir));
        }
        else
        {
            printf("Unknown argument: %s\n", argv[i]);
            printUsage();
            return 1;
        }
    }

    engine = Game::CreateEngine(this, argc, argv);

    if (!engine)
        return 1;

    if (useVideo)
    {
        if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE))
        {
            fprintf(stderr, "SDLFramework: Unable to init video.\n");
        }
        else
        {
            xres -= xres % scaleFactor;
            yres -= yres % scaleFactor;

            screen = SDL_SetVideoMode(xres, yres, 16, (useFullscreen ? SDL_FULLSCREEN : 0));

            if (!screen)
            {
                fprintf(stderr, "SDLFramework: Unable to set video mode.\n");
            }
            else
            {
                Game::PixelFormat pf(
                    screen->format->BitsPerPixel,
                    screen->format->Rmask, screen->format->Rshift,
                    screen->format->Gmask, screen->format->Gshift,
                    screen->format->Bmask, screen->format->Bshift
                );

                printf("SDLFramework: %dx%d framebuffer, %d bits per pixel, scale %dx.\n",
                       screen->w, screen->h, screen->format->BitsPerPixel, scaleFactor);

                if (scaleFactor == 1)
                {
                    gameScreen = new Game::Surface(&pf, (Game::Pixel*)screen->pixels, screen->w, screen->h);
                }
                else
                {
                    gameScreen = new Game::Surface(&pf, screen->w / scaleFactor, screen->h / scaleFactor);
                }
                engine->configureVideo(gameScreen);
            }
        }
    }

    if (useSound)
    {
        if (SDL_Init(SDL_INIT_AUDIO))
        {
            fprintf(stderr, "SDLFramework: Unable to init audio.\n");
        }
        else
        {
            SDL_AudioSpec audio, audioResult;

            memset(&audio, 0, sizeof(SDL_AudioSpec));
            audio.freq = rate;
            audio.format = AUDIO_S16;
            audio.channels = 1;
            audio.samples = 2048;
            audio.callback = audioCallback;
            audio.userdata = this;

            if (SDL_OpenAudio(&audio, &audioResult))
            {
                fprintf(stderr, "SDLFramework: Unable to open audio device.\n");
            }
            else
            {
                printf("SDLFramework: %d sample audio buffer at %d Hz, %d channels, 16 bits.\n", audioResult.samples, audioResult.freq, audioResult.channels);

                Game::SampleFormat sf(16, audioResult.channels);
                gameAudio = new Game::SampleChunk(&sf, 0, audioResult.samples, audioResult.freq);
                gameAudio->autoDelete = false;
                engine->configureAudio(gameAudio);
                SDL_PauseAudio(0);
            }
        }
    }
    
    if (SDL_Init(SDL_INIT_JOYSTICK) == 0)
    {
        SDL_JoystickEventState(SDL_ENABLE);
        if (SDL_JoystickOpen(joynum))
        {
            printf("SDLFramework: Using joystick '%s'\n", SDL_JoystickName(joynum));
        } else
        {
            fprintf(stderr, "SDLFramework: Unable to open joystick %d.\n", joynum);
        }
    }

    while(!done)
    {
        SDL_Event event;
        Game::Event gameEvent;

        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_KEYDOWN:
                gameEvent.type = Game::Event::KeyPressEvent;
                gameEvent.key.code = event.key.keysym.sym;
                engine->handleEvent(&gameEvent);
                break;
            case SDL_KEYUP:
                gameEvent.type = Game::Event::KeyReleaseEvent;
                gameEvent.key.code = event.key.keysym.sym;
                engine->handleEvent(&gameEvent);
                break;
            case SDL_QUIT:
                gameEvent.type = Game::Event::ExitEvent;
                engine->handleEvent(&gameEvent);
                exit();
                return 0;
                break;
            case SDL_JOYAXISMOTION:
                gameEvent.type = Game::Event::JoystickAxisEvent;
                gameEvent.joyAxis.which = event.jaxis.which;
                gameEvent.joyAxis.axis = event.jaxis.axis;
                gameEvent.joyAxis.value = event.jaxis.value;
                engine->handleEvent(&gameEvent);
                break;
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
                gameEvent.type = Game::Event::JoystickAxisEvent;
                gameEvent.joyButton.which = event.jbutton.which;
                gameEvent.joyButton.button = event.jbutton.button;
                gameEvent.joyButton.state = event.jbutton.state;
                engine->handleEvent(&gameEvent);
                break;
            case SDL_VIDEORESIZE:
                SDL_FreeSurface(screen);

                screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 16, SDL_RESIZABLE);

                if (!screen)
                {
                    fprintf(stderr, "SDLFramework: Unable to set video mode.\n");
                    exit();
                    return 1;
                }
                else
                {
                    delete gameScreen;
                    Game::PixelFormat pf(
                        screen->format->BitsPerPixel,
                        screen->format->Rmask, screen->format->Rshift,
                        screen->format->Gmask, screen->format->Gshift,
                        screen->format->Bmask, screen->format->Bshift
                    );
                    gameScreen = new Game::Surface(&pf, (Game::Pixel*)screen->pixels, screen->w, screen->h);
                    engine->configureVideo(gameScreen);
                }

                break;
            }
        }

        if (screen)
        {
            engine->renderVideo(gameScreen);

            if (scaleFactor > 1)
            {
                upscale(screen, gameScreen, scaleFactor);
            }
            SDL_Flip(screen);
        }
    }
    return 0;
}

template <typename PIXEL, int FACTOR>
void genericUpscale(PIXEL* RESTRICT dest, const PIXEL* RESTRICT src, int srcW, int srcH)
{
    PIXEL* RESTRICT d = dest;
    const PIXEL* RESTRICT s = src;
    int x, y, i, j;

    for (y = 0; y < srcH; y++)
    {
        for (x = 0; x < srcW; x++)
        {
            for (i = 0; i < FACTOR; i++)
            {
                for (j = 0; j < FACTOR; j++)
                {
                d[i + j * (srcW * FACTOR)] = *s;
                }
            }
            PREFETCH(&s[1], 0, 0);
            PREFETCH(&d[FACTOR], 1, 0);
            s++;
            d += FACTOR;
        }
        d += (srcW * FACTOR) * (FACTOR - 1);
    }
}

void SDLFramework::upscale(SDL_Surface* target, const Game::Surface* source, int scaleFactor)
{
    if (SDL_MUSTLOCK(target))
    {
        SDL_LockSurface(target);
    }

    switch (target->format->BitsPerPixel)
    {
    case 16:
        {
            const Uint16 *s = (const Uint16*)source->pixels;
            Uint16 *d = (Uint16*)target->pixels;
            switch (scaleFactor)
            {
            case 2:
                genericUpscale<Uint16, 2>(d, s, source->width, source->height);
                break;
            case 3:
                genericUpscale<Uint16, 3>(d, s, source->width, source->height);
                break;
            case 4:
                genericUpscale<Uint16, 4>(d, s, source->width, source->height);
                break;
            case 5:
                genericUpscale<Uint16, 5>(d, s, source->width, source->height);
                break;
            case 6:
                genericUpscale<Uint16, 6>(d, s, source->width, source->height);
                break;
            case 7:
                genericUpscale<Uint16, 7>(d, s, source->width, source->height);
                break;
            case 8:
                genericUpscale<Uint16, 8>(d, s, source->width, source->height);
                break;
            default:
                assert(!"Unsupported scaling factor");
                break;
            }
            break;
        }
    case 32:
        {
            const Uint32 *s = (const Uint32*)source->pixels;
            Uint32 *d = (Uint32*)target->pixels;
            switch (scaleFactor)
            {
            case 2:
                genericUpscale<Uint32, 2>(d, s, source->width, source->height);
                break;
            case 3:
                genericUpscale<Uint32, 3>(d, s, source->width, source->height);
                break;
            case 4:
                genericUpscale<Uint32, 4>(d, s, source->width, source->height);
                break;
            case 5:
                genericUpscale<Uint32, 5>(d, s, source->width, source->height);
                break;
            case 6:
                genericUpscale<Uint32, 6>(d, s, source->width, source->height);
                break;
            case 7:
                genericUpscale<Uint32, 7>(d, s, source->width, source->height);
                break;
            case 8:
                genericUpscale<Uint32, 8>(d, s, source->width, source->height);
                break;
            default:
                assert(!"Unsupported scaling factor");
                break;
            }
            break;
        }
        break;
    default:
        assert(!"Unsupported display bit depth for scaling");
        break;
    }
    if (SDL_MUSTLOCK(target))
    {
        SDL_UnlockSurface(target);
    }
}

void SDLFramework::exit()
{
    SDL_PauseAudio(1);
    done = true;
}

unsigned int SDLFramework::getTickCount()
{
    return SDL_GetTicks();
}

unsigned int SDLFramework::getTicksPerSecond()
{
    return 1000;
}

void SDLFramework::audioCallback(void *userdata, Uint8 *stream, int len)
{
    SDLFramework *fw = (SDLFramework*)userdata;

    fw->gameAudio->data = (Game::Sample8*)stream;
    fw->gameAudio->length = len / (fw->gameAudio->format.bytesPerSample * fw->gameAudio->format.channels);
    fw->engine->renderAudio(fw->gameAudio);
}

#ifdef USE_SDL_IMAGE
Game::Surface *SDLFramework::loadImage(const char *name, Game::PixelFormat *pf)
{
    SDL_Surface *img = IMG_Load(name);
    Game::Surface *surface = 0;
    int x, y;

    if (img)
    {
        Game::PixelFormat imgpf(
            img->format->BitsPerPixel,
            img->format->Rmask, img->format->Rshift,
            img->format->Gmask, img->format->Gshift,
            img->format->Bmask, img->format->Bshift
        );

        if (pf)
            surface = new Game::Surface(pf, img->w, img->h);
        else
            surface = new Game::Surface(&imgpf, img->w, img->h);

        if (img->format->palette)
        {
            if (surface->format.bitsPerPixel == 8)
            {
                // memcpy hack for 8-bit palette images
                memcpy(surface->pixels, img->pixels, img->h * img->pitch);
            }
            else
                for(y=0; y<img->h; y++)
                    for(x=0; x<img->w; x++)
                    {
                        Uint8 r,g,b;
                        Uint8 index = *(((Uint8*)img->pixels) + y*img->pitch + x*img->format->BytesPerPixel);

                        r = img->format->palette->colors[index].r;
                        g = img->format->palette->colors[index].g;
                        b = img->format->palette->colors[index].b;
                        surface->setPixel(x, y, surface->format.makePixel(r,g,b));
                    }
        }
        else
        {
            for(y=0; y<img->h; y++)
                for(x=0; x<img->w; x++)
                {
                    // here we might go beyond the image
                    Uint32 srcpixel = *(Uint32*)(((Uint8*)img->pixels) + y*img->pitch + x*img->format->BytesPerPixel);
                    Uint8 r,g,b;

                    SDL_GetRGB(srcpixel, img->format, &r, &g, &b);
                    surface->setPixel(x, y, surface->format.makePixel(r,g,b));
                }
        }
        SDL_FreeSurface(img);

        // perform pixel format conversion
        if (pf && surface->format.bitsPerPixel!=8)
        {
            Game::Surface *converted = new Game::Surface(pf, surface);
            delete surface;
            return converted;
        }
    }
    return surface;
}
#endif

#ifdef USE_WAVE_LOADER
Game::SampleChunk *SDLFramework::loadSample(const char *name, Game::SampleFormat *sf)
{
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
    typedef struct
    {
        unsigned int chunkID;
        unsigned int chunkSize;
        unsigned int format;
        unsigned int subChunk1ID;
        unsigned int subChunk1Size;
        unsigned short audioFormat;
        unsigned short numChannels;
        unsigned int sampleRate;
        unsigned int byteRate;
        unsigned short blockAlign;
        unsigned short bitsPerSample;
        unsigned int subChunk2ID;
        unsigned int subChunk2Size;
    }
    PACKED WaveHeader;
#ifdef _MSC_VER
#pragma pack(pop)
#endif

    FILE *f = fopen(name, "rb");
    Game::SampleChunk *sample = 0;

    if (f)
    {
        WaveHeader header;

        fread(&header, sizeof(WaveHeader), 1, f);

        if (header.audioFormat == 1)
        {
            Game::SampleFormat wavsf(header.bitsPerSample, header.numChannels);
            int samples = header.subChunk2Size / (header.numChannels * (header.bitsPerSample/8));
            int n, ch;

            if (sf)
                sample = new Game::SampleChunk(sf, samples, header.sampleRate);
            else
                sample = new Game::SampleChunk(&wavsf, samples, header.sampleRate);

            for(n=0; n<samples; n++)
                for(ch=0; ch<header.numChannels; ch++)
                {
                    switch(header.bitsPerSample)
                    {
                    case 8:
                        {
                            Game::Sample8 s;
                            fread(&s, sizeof(s), 1, f);
                            s+=0x80;
                            sample->setSample(n,ch,sample->format.makeSample(s<<8));
                        }
                        break;
                    case 16:
                        {
                            Game::Sample16 s;
                            fread(&s, sizeof(s), 1, f);
                            s+=0x8000;
                            sample->setSample(n,ch,sample->format.makeSample(s));
                        }
                        break;
                    }
                }
        }

        fclose(f);
    }
    return sample;
}
#endif

static bool makedirs(const char* path, int mode = 0700)
{
    char tmpPath[PATH_MAX];
    int i;

    for (i = 0; path[i]; i++)
    {
        if (path[i] == '/' && i > 1)
        {
            struct stat sb;

            tmpPath[i] = 0;
            if (stat(tmpPath, &sb) != 0)
            {
                if (mkdir(tmpPath, mode) != 0)
                {
                    perror("mkdir");
                    return false;
                }
            }
        }
        tmpPath[i] = path[i];
    }
    return true;
}

const char *SDLFramework::findResource(const char *name, bool mustExist, bool writable)
{
    FILE* f;

    if (!writable)
    {
        snprintf(resourcePath, sizeof(resourcePath), "%s/%s", dataDir, name);
    }
    else
    {
#if !defined(APPNAME)
        return name;
#else
        const char* home = getenv("HOME");
        if (home)
        {
            int i;
            snprintf(resourcePath, sizeof(resourcePath), "%s/.%s/%s", home, APPNAME, name);
            for (i = strlen(resourcePath) - 1; i >= 0; i--)
            {
                if (i == '/')
                {
                    resourcePath[i] = 0;
                    if (!makedirs(resourcePath))
                    {
                        return NULL;
                    }
                    break;
                }
            }
            snprintf(resourcePath, sizeof(resourcePath), "%s/.%s/%s", home, APPNAME, name);
        }
        else
        {
            return name;
        }
#endif
    }

    f = fopen(resourcePath, "r");
    fprintf(stdout, "SDLFramework::findResource(): %s%s\n", resourcePath, (f == NULL) ? " (not found)" : "");
    if (f)
        {
        fclose(f);
        }
    return resourcePath;
}

