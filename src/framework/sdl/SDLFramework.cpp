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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	SDLFramework fw;
	
	return fw.run();
}

SDLFramework::SDLFramework():
	screen(NULL),
	gameScreen(NULL),
	gameAudio(NULL),
	engine(NULL)
{
}

SDLFramework::~SDLFramework()
{
	if (screen)
		SDL_FreeSurface(screen);
		
	delete gameScreen;
	delete gameAudio;
	delete engine;
}

int SDLFramework::run()
{
	engine = CreateEngine(this);
	
	if (!engine)
		return 1;

	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE))
	{
		fprintf(stderr, "SDLFramework: Unable to init video.\n");
	} else
	{
//		screen = SDL_SetVideoMode(640, 480, 16, SDL_RESIZABLE*0);
		screen = SDL_SetVideoMode(176, 208, 16, SDL_RESIZABLE*0);
		
		if (!screen)
		{
			fprintf(stderr, "SDLFramework: Unable to set video mode.\n");
		} else
		{
			printf("SDLFramework: %dx%d framebuffer, %d bits per pixel.\n", screen->w, screen->h, screen->format->BitsPerPixel);
		
			Game::PixelFormat pf(
				screen->format->BitsPerPixel,
				screen->format->Rmask, screen->format->Rshift,
				screen->format->Gmask, screen->format->Gshift,
				screen->format->Bmask, screen->format->Bshift
				);
			gameScreen = new Game::Surface(&pf, (Game::Pixel*)screen->pixels, screen->w, screen->h);
			engine->configureVideo(gameScreen);
		}
	}
	
	if (SDL_Init(SDL_INIT_AUDIO))
	{
		fprintf(stderr, "SDLFramework: Unable to init audio.\n");
	} else
	{
		SDL_AudioSpec audio, audioResult;
		
		memset(&audio, 0, sizeof(SDL_AudioSpec));
		audio.freq = 22050;
		audio.format = AUDIO_S16;
		audio.channels = 1;
		audio.samples = 512;
		audio.callback = audioCallback;
		audio.userdata = this;
		
		if (SDL_OpenAudio(&audio, &audioResult))
		{
			fprintf(stderr, "SDLFramework: Unable to open audio device.\n");
		} else
		{
			printf("SDLFramework: %d sample audio buffer at %d Hz, %d channels, 16 bits.\n", audioResult.samples, audioResult.freq, audioResult.channels);
			
			Game::SampleFormat sf(16, audioResult.channels);
			gameAudio = new Game::SampleChunk(&sf, NULL, audioResult.samples, audioResult.freq);
			gameAudio->autoDelete = false;
			engine->configureAudio(gameAudio);
			SDL_PauseAudio(0);
		}
	}
	
	while(1)
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
				gameEvent.key.code = event.key.keysym.scancode;
				engine->handleEvent(&gameEvent);
			break;
			case SDL_QUIT:
				gameEvent.type = Game::Event::ExitEvent;
				engine->handleEvent(&gameEvent);
				exit();
				return 0;
			break;
			case SDL_VIDEORESIZE:
				SDL_FreeSurface(screen);
				
				screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 16, SDL_RESIZABLE);
				
				if (!screen)
				{
					fprintf(stderr, "SDLFramework: Unable to set video mode.\n");
					exit();
					return 1;
				} else
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
		
		engine->renderVideo(gameScreen);
		SDL_Flip(screen);
	}
	return 0;
}

void SDLFramework::exit()
{
	SDL_Quit();
	::exit(0);
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
	
	fw->gameAudio->data = stream;
	fw->gameAudio->length = len / (fw->gameAudio->format.bytesPerSample * fw->gameAudio->format.channels);
	fw->engine->renderAudio(fw->gameAudio);
}

#ifdef USE_SDL_IMAGE
Game::Surface *SDLFramework::loadImage(const char *name, Game::PixelFormat *pf)
{
	SDL_Surface *img = IMG_Load(name);
	Game::Surface *surface = NULL;
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
				
//				for(x=0; x<img->h*img->pitch; x++)
//					printf("%d\n", ((char*)img->pixels)[x]);
//				for(x=0; x<img->h*img->pitch; x++)
//					printf("%d\n", ((char*)surface->pixels)[x]);
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
		} else
		{
			for(y=0; y<img->h; y++)
			for(x=0; x<img->w; x++)
			{
				Uint32 srcpixel = *(Uint32*)(((Uint8*)img->pixels) + y*img->pitch + x*img->format->BytesPerPixel);
				Uint8 r,g,b;
				
				SDL_GetRGB(srcpixel, img->format, &r, &g, &b);
				surface->setPixel(x, y, surface->format.makePixel(r,g,b));
			}
		}
		
		// perform pixel format conversion
		if (pf && surface->format.bitsPerPixel!=8)
		{
			Game::Surface *converted = new Game::Surface(pf, surface);
			delete surface;
			return converted;
		}
		SDL_FreeSurface(img);
	}
	return surface;
}
#endif

#define PACKED __attribute__((packed)) 

#ifdef USE_WAVE_LOADER
Game::SampleChunk *SDLFramework::loadSample(const char *name, Game::SampleFormat *sf)
{
	typedef struct
	{
		unsigned int chunkID PACKED;
		unsigned int chunkSize PACKED;
		unsigned int format PACKED;
		unsigned int subChunk1ID PACKED;
		unsigned int subChunk1Size PACKED;
		unsigned short audioFormat PACKED;
		unsigned short numChannels PACKED;
		unsigned int sampleRate PACKED;
		unsigned int byteRate PACKED;
		unsigned short blockAlign PACKED;
		unsigned short bitsPerSample PACKED;
		unsigned int subChunk2ID PACKED;
		unsigned int subChunk2Size PACKED;
	} WaveHeader;

	FILE *f = fopen(name, "rb");
	Game::SampleChunk *sample = NULL;
	
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
					sample->setSample(n,ch,sample->format.makeSample(s<<8));
				}
				break;
				case 16:
				{
					Game::Sample16 s;
					fread(&s, sizeof(s), 1, f);
					s-=0x8000;
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
