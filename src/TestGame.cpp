#include "Config.h"
#include "engine/Engine.h"

#ifdef EPOC
#include <e32keys.h>
#include <stdlib.h>
#include <stdio.h>
#else
#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#endif

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

#ifdef EPOC
#define KEY_LEFT	EStdKeyLeftArrow
#define KEY_RIGHT	EStdKeyRightArrow
#define KEY_UP		EStdKeyUpArrow
#define KEY_DOWN	EStdKeyDownArrow
#define KEY_EXIT	EStdKeyDevice0
#else
#define KEY_LEFT	SDLK_LEFT
#define KEY_RIGHT	SDLK_RIGHT
#define KEY_UP		SDLK_UP
#define KEY_DOWN	SDLK_DOWN
#define KEY_EXIT	SDLK_ESCAPE
#endif


//#include "models/Duck.h"
//#include "models/Loop.h"
//#include "models/Alus.h"
#include "models/CarModel.h"

class MyEngine: public Game::Engine
{
public:
        Rasterizer      *rasterizer;
        View            *view;
        Game::Surface   *texture, *texture2, *font;
//        Object          *object;
        Land            *ground, *sky;
	Track		*track;
	Car		*car;
        scalar          t, lastTime;
	BitmapFont	*bitmapFont;
	World		*world;
        
        Mixer           *mixer;
        ModPlayer       *modplayer;
	Game::SampleChunk	*audioBuffer;

        MyEngine(Game::Framework* _framework):
                Game::Engine(_framework),
                rasterizer(NULL),
                view(NULL),
		t(0),
		world(0),
		lastTime(0),
		car(0),
		audioBuffer(0)
        {
        }

        ~MyEngine()
        {
		delete car;
		delete world;
//                delete object;
                delete rasterizer;
                delete texture;
                delete texture2;
                delete track;
//                delete sky;
                delete ground;
                delete view;
                delete mixer;
                delete modplayer;
		delete font;
		delete bitmapFont;
        }
        
        void configureVideo(Game::Surface* screen)
        {
                int x, y;
                Game::Pixel *pixels;
                
                rasterizer = new Rasterizer(screen);
                view = new View(rasterizer);
		world = new World(framework, screen, rasterizer, view);
                
                texture = new Game::Surface(&screen->format, 256, 256);
                pixels = texture->pixels;
                for(y=0; y<texture->height; y++)
                {
                        for(x=0; x<texture->width; x++)
                        {
//                              *pixels++ = (x<<texture->format.rshift) + (y<<texture->format.bshift);
//                              int i = x^y;
//                              *pixels++ = (x^y) & texture->format.bmask;
                                
                                if ((x+y)&1)
                                        *pixels++ = 0xf00;
                                else
                                        *pixels++ = 0x0f0;
                                
/*                              
                                *pixels++ =
                                        ((i<<texture->format.rshift) & texture->format.rmask) +
                                        ((i<<texture->format.gshift) & texture->format.gmask) +
                                        ((i<<texture->format.bshift) & texture->format.bmask);
*/                                      
        //                      *pixels++ = (i<<8) + (i<<4) + (i<<0);
        //                      *pixels++ = rand();
                        }
                }
                
                texture2 = new Game::Surface(&screen->format, 256, 256);
                pixels = texture2->pixels;
                for(y=0; y<texture2->height; y++)
                {
                        for(x=0; x<texture2->width; x++)
                        {
                                int s, p;
        
                                if (x<texture2->width/2)
                                        if (y<texture2->height/2)
                                        {
                                                p = texture2->format.rmask;
                                                s = texture2->format.rshift;
                                        }
                                        else
                                        {
                                                p = texture2->format.gmask;
                                                s = texture2->format.gshift;
                                        }
                                else
                                        if (y<texture2->height/2)
                                        {
                                                p = texture2->format.bmask;
                                                s = texture2->format.bshift;
                                        }
                                        else
                                        {
                                                p = texture2->format.rmask;
                                                s = texture2->format.rshift;
                                        }
                                *pixels++ = ((x^y)<<s) & p;
                        }
                }
                
#if 0
                textureTileList[0] = new Game::Surface(&screen->format, TILE_TEXTURE_SIZE, TILE_TEXTURE_SIZE);
                pixels = textureTileList[0]->pixels;
                for(y=0; y<textureTileList[0]->height; y++)
                {
                        for(x=0; x<textureTileList[0]->width; x++)
                        {
                                if ((TILE_TEXTURE_SIZE/2-x)*(TILE_TEXTURE_SIZE/2-x) + (TILE_TEXTURE_SIZE/2-y)*(TILE_TEXTURE_SIZE/2-y) >= (TILE_TEXTURE_SIZE*TILE_TEXTURE_SIZE/8))
                                        *pixels++ = 0;
                                else
                                        *pixels++ = (x^y)<<1;
                        }
                }
#else
//                view->rasterizer->setTextureTileList(textureTileList);
//                textureTileList[1] = framework->loadImage(findResource("test.png"), &screen->format);
//                textureTileList[2] = framework->loadImage(findResource("test2.png"), &screen->format);
#endif          
                
                track = new Track(framework, screen);
		track->load(framework->findResource("track.trk"));

                ground = new Land(framework->loadImage(framework->findResource("ground.png"), &screen->format), 0 /*flags*/, 0 /* textureScale */, FPInt(6));
//                sky = new Land(framework->loadImage(framework->findResource("sky.png"), &screen->format), 0, FPInt(6), FPInt(1), FPInt(0), FPInt(64));
                
#if 1
//              object = new Duck(FPInt(1)>>9);
//              object = new Loop(FPInt(1)>>3, texture2);
//                object = new Alus(FPInt(1)>>8);
//                object = new CarModel(FPInt(1)>>8, framework->loadImage(framework->findResource("car.png"), &screen->format));
//		Vector axis(FP_ONE,0,0);
//		object->transformation = Matrix::makeRotation(axis, PI>>1);
#else
                object = new Object(8, 6);
                object->beginMesh();
                
                scalar scale = FPInt(2)>>3;
                Vector v0(-scale, 0,-scale);
                Vector v1( scale, 0,-scale);
                Vector v2( scale, 0, scale);
                Vector v3(-scale, 0, scale);
                Vector v4(-scale, scale<<1,-scale);
                Vector v5( scale, scale<<1,-scale);
                Vector v6( scale, scale<<1, scale);
                Vector v7(-scale, scale<<1, scale);

                object->setTexCoord(FPInt(0), FPInt(0));
                object->addVertex(v0);
                object->setTexCoord(FPInt(0xff), FPInt(0));
                object->addVertex(v1);
                object->setTexCoord(FPInt(0xff), FPInt(0xff));
                object->addVertex(v2);
                object->setTexCoord(FPInt(0), FPInt(0xff));
                object->addVertex(v3);

                object->setTexCoord(FPInt(0), FPInt(0));
                object->addVertex(v4);
                object->setTexCoord(FPInt(0xff), FPInt(0));
                object->addVertex(v5);
                object->setTexCoord(FPInt(0xff), FPInt(0xff));
                object->addVertex(v6);
                object->setTexCoord(FPInt(0), FPInt(0xff));
                object->addVertex(v7);
                                
                object->beginFace(4);
//              object->setTexture(texture2);
                object->addFaceVertex(0);
                object->addFaceVertex(1);
                object->addFaceVertex(2);
                object->addFaceVertex(3);
                object->endFace();

                object->beginFace(4);
                object->setTexture(texture2);
                object->addFaceVertex(7);
                object->addFaceVertex(6);
                object->addFaceVertex(5);
                object->addFaceVertex(4);
                object->endFace();

                object->beginFace(4);
//              object->setTexture(texture2);
                object->addFaceVertex(4);
                object->addFaceVertex(5);
                object->addFaceVertex(1);
                object->addFaceVertex(0);
                object->endFace();

                object->beginFace(4);
//              object->setTexture(texture2);
                object->addFaceVertex(5);
                object->addFaceVertex(6);
                object->addFaceVertex(2);
                object->addFaceVertex(1);
                object->endFace();
                
                object->beginFace(4);
                object->setTexture(texture2);
                object->addFaceVertex(6);
                object->addFaceVertex(7);
                object->addFaceVertex(3);
                object->addFaceVertex(2);
                object->endFace();

                object->beginFace(4);
                object->setTexture(texture2);
                object->addFaceVertex(7);
                object->addFaceVertex(4);
                object->addFaceVertex(0);
                object->addFaceVertex(3);
                object->endFace();
                                
                object->endMesh();
#endif          
                font = framework->loadImage(framework->findResource("font.png"), &screen->format);
		bitmapFont = new BitmapFont(font);
		car = new Car(world);
        }

        void setupView()
        {
#if 0	
                scalar angle = (t>>2) % (2*PI);
		int r = 2;
                scalar x = 0*FPMul(FPSin(angle), FPInt(8)) + FPInt(0);
                scalar z = FPMul(FPCos(angle), FPInt(8)) + FPInt(0);
                scalar y = 0*FPMul(FPCos(angle)+(FP_ONE>>1), FPInt(2)) + FPInt(3)>>3;
//                scalar y = FPInt(3)>>4;
                
//              printf("%d, %d, %d\n", t, framework->getTickCount()/10, framework->getTicksPerSecond());

                view->rasterizer->setTexture(texture);

                view->camera.target = Vector(FPInt(0),FPInt(0),FPInt(0));
//                view->camera.origin = Vector(x,FPInt(3)>>4,z);
                view->camera.origin = Vector(x,y,z);

                view->camera.target = Vector(x+FPInt(0),FPInt(0),z+FPInt(1));
                view->camera.origin = Vector(x,y,z);
		
//                view->camera.origin = Vector(FPInt(0),FPInt(3)>>4,FPInt(0));
//                view->camera.target = Vector(x,0,z);
#endif
                scalar x = FPMul(FPCos(car->getAngle()), FPInt(1));
                scalar z = FPMul(FPSin(car->getAngle()), FPInt(1));
                scalar y = FPInt(3)>>3;

                view->camera.target = car->origin;
                view->camera.origin = Vector(car->origin.x-x,car->origin.y+y,car->origin.z-z);
		
                view->camera.update();
        }
#if 1
        void renderRasterizerTest()
        {
                scalar angle = (FPInt(framework->getTickCount()) / 1000);
                scalar angle_l = FPMod(angle - FPInt(1), 2*PI);
                scalar angle_r = FPMod(angle + FPInt(1), 2*PI);
                
                angle = FPMod(angle,2*PI);
                
                scalar r = FPInt(120);
                scalar rs = FPInt(28);
                scalar cx = FPInt(rasterizer->screen->width >> 1);
                scalar cy = FPInt(rasterizer->screen->height >> 1);
                scalar x = FPMul(FPSin(angle), r);
                scalar y = FPMul(FPCos(angle), r);
                scalar lx = FPMul(FPSin(angle_l), rs);
                scalar ly = FPMul(FPCos(angle_l), rs);
                scalar rx = FPMul(FPSin(angle_r), rs);
                scalar ry = FPMul(FPCos(angle_r), rs);
                
                rasterizer->setTexture(0);
		
                rasterizer->beginPolygon();
                rasterizer->setInvZ(FPInt(1));
                rasterizer->setTexCoord(FPInt(0x7f), FPInt(0));
                rasterizer->addVertex(cx, cy);
                rasterizer->setInvZ(FPInt(1));
                rasterizer->setTexCoord(FPInt(0), FPInt(0x7f));
                rasterizer->addVertex(cx+lx, cy+ly);
                rasterizer->setInvZ(FPInt(1));
                rasterizer->setTexCoord(FPInt(0), FPInt(0xff));
                rasterizer->addVertex(cx+x, cy+y);
                rasterizer->setInvZ(FPInt(1));
                rasterizer->setTexCoord(FPInt(0xff), FPInt(0x7f));
                rasterizer->addVertex(cx+rx, cy+ry);
                rasterizer->endPolygon();
        }
#endif        

#if 0
        void renderBackground(Game::Surface *screen)
        {
                Game::Pixel *pixels = screen->pixels;
                int x, y;
                int j = framework->getTickCount() >> 4;

                for(y=0;y<screen->height;y++)
                for(x=0;x<screen->width;x++)
                {
                        int s, p;

                        if (x<screen->width/2)
                                if (y<screen->height/2)
                                {
                                        p = screen->format.rmask;
                                        s = screen->format.rshift;
                                }
                                else
                                {
                                        p = screen->format.gmask;
                                        s = screen->format.gshift;
                                }
                        else
                                if (y<screen->height/2)
                                {
                                        p = screen->format.bmask;
                                        s = screen->format.bshift;
                                }
                                else
                                {
                                        p = screen->format.rmask;
                                        s = screen->format.rshift;
                                }

                        *pixels++ = ((x^y + j)<<s) & p;
                }
        }
#endif

	void renderAudioBuffer(Game::Surface *screen)
	{
		if (!audioBuffer)
			return;

		int x, y;
		for(x=0; x<screen->width; x++)
		{
			y = ((Game::Sample*)audioBuffer->data)[x];
			y >>= 7;
			y += screen->height/2;

			if (y<0) y=0;
			if (y>screen->height-1) y=screen->height-1;

			screen->setPixel(x, y, screen->getPixel(x,y)^0xffff);
		}
	}
	
	void step()
	{
		const int timestep = 256;
	
                t = (100*framework->getTickCount() / framework->getTicksPerSecond()) << (FP-8);
		
		if (t - lastTime > timestep)
		{
			while(t - lastTime > timestep)
			{
				lastTime+=timestep;
				car->update(track);
			}
			lastTime = t;
		}
	}
        
        void renderVideo(Game::Surface* screen)
        {
#if 1
		step();

                screen->clear();
		setupView();

//              renderBackground(screen);
//              renderLand();
//		sky->render(view);
		ground->render(world);
		track->render(world);
		car->render(world);
		
/*
		rasterizer->flags &= ~Rasterizer::FlagPerspectiveCorrection;
		Vector axis(0,FP_ONE,0);
		Matrix translation = Matrix::makeTranslation(car.origin);
		object->transformation = Matrix::makeRotation(axis, car.getAngle());
		object->transformation *= translation;
		object->render(view);
		rasterizer->flags |= Rasterizer::FlagPerspectiveCorrection;
*/
//		track->setCell(car.origin);
		
//                renderRasterizerTest();
		
//                renderObjectTest(view);
#endif
//		renderAudioBuffer(screen);
		
		char hud[32];
//		sprintf(hud, "%d", t);
		sprintf(hud, "%d", car->speed);
		bitmapFont->renderText(screen, hud, 1, 1);
        }

        void configureAudio(Game::SampleChunk* sample)
        {
                mixer = new Mixer(sample->rate, 4);
                modplayer = new ModPlayer(mixer);
                modplayer->load(framework->findResource("dallas.mod"));
        }

        void renderAudio(Game::SampleChunk* sample)
        {
                mixer->render(sample);
		audioBuffer = sample;
        }

        void handleEvent(Game::Event* event)
        {
			switch(event->type)
			{
			case Game::Event::ExitEvent:
			break;
			case Game::Event::KeyPressEvent:
			{
					switch(event->key.code)
					{
					case KEY_EXIT:
							framework->exit();
					break;
					case KEY_UP:
						car->setThrust(true);
						car->setBrake(false);
					break;
					case KEY_DOWN:
						car->setThrust(false);
						car->setBrake(true);
					break;
					case KEY_LEFT:
						car->setSteering(-1);
					break;
					case KEY_RIGHT:
						car->setSteering(1);
					break;
				}
		}
		break;
		case Game::Event::KeyReleaseEvent:
		{
				switch(event->key.code)
				{
					case KEY_UP:
						car->setThrust(false);
					break;
					case KEY_DOWN:
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
		break;
		}
        }
};


extern "C"
{

Game::Engine* CreateEngine(Game::Framework* framework)
{
        return new MyEngine(framework);
}


};
