#include "config.h"
#include "engine/Engine.h"

#ifdef EPOC
#include <e32keys.h>
#include <stdlib.h>
#include <stdio.h>
#else
#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
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

#include "models/Duck.h"
#include "models/Loop.h"
#include "models/Alus.h"

class MyEngine: public Game::Engine
{
public:
        Rasterizer      *rasterizer;
        View            *view;
        Game::Surface   *texture, *texture2;
        Object          *object;
        Land            *ground, *sky;
	Track		*track;
        scalar          t;
        
        Mixer           *mixer;
        ModPlayer       *modplayer;
	Game::SampleChunk	*audioBuffer;

        MyEngine(Game::Framework* _framework):
                Game::Engine(_framework),
                rasterizer(NULL),
                view(NULL),
		audioBuffer(0)
        {
        }

        ~MyEngine()
        {
                delete object;
                delete rasterizer;
                delete texture;
                delete texture2;
                delete track;
                delete sky;
                delete ground;
                delete view;
                delete mixer;
                delete modplayer;
        }
        
        void configureVideo(Game::Surface* screen)
        {
                int x, y;
                Game::Pixel *pixels;
                
                rasterizer = new Rasterizer(screen);
                view = new View(rasterizer);
                
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

                ground = new Land(framework->loadImage(framework->findResource("ground.png"), &screen->format), 0, FPInt(4), FPInt(0), FPInt(2), FPInt(256));
                sky = new Land(framework->loadImage(framework->findResource("sky.png"), &screen->format), 0, FPInt(6), FPInt(1), FPInt(0), FPInt(64));
                
#if 1
//              object = new Duck(FPInt(1)>>9);
//              object = new Loop(FPInt(1)>>3, texture2);
                object = new Alus(FPInt(1)>>8);
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
        }

#if 0
        void renderObjectTest(View *view)
        {
                Vector axis(0,FP_ONE,0);
//              Matrix tr = Matrix::makeTranslation(axis);
//              object->transformation = Matrix::makeRotation(axis, ((t<<2) % (2*PI)));
//              object->transformation *= tr;


                rasterizer->flags &= ~Rasterizer::FlagPerspectiveCorrection;
                object->render(view);
                rasterizer->flags |= Rasterizer::FlagPerspectiveCorrection;
        }
#endif

#if 0        
        void renderLand()
        {
                scalar scale = FPInt(4);
                Vector v0(-scale, 0,-scale);
                Vector v1( scale, 0,-scale);
                Vector v2( scale, 0, scale);
                Vector v3(-scale, 0, scale);
                scalar tSize = FPInt(32);
                Vector tCenter = view->camera.origin;
                
                v0.x+=tCenter.x;
                v0.z+=tCenter.z;
                v1.x+=tCenter.x;
                v1.z+=tCenter.z;
                v2.x+=tCenter.x;
                v2.z+=tCenter.z;
                v3.x+=tCenter.x;
                v3.z+=tCenter.z;
                
                tCenter *= FPInt(8);
                tCenter+=Vector(FPInt(64),0,FPInt(64));
                
//              printf("%d, %d, %d\n", tCenter.x>>FP, tCenter.y>>FP, tCenter.z>>FP);
                
//              tCenter /= FPInt(20000);

                view->rasterizer->flags |= Rasterizer::FlagTileTexture;
                view->rasterizer->setTexture(texture3);
                view->rasterizer->setTextureTileList(textureTileList);

                view->beginPolygon();
                view->setTexCoord(tCenter.x-tSize, tCenter.z-tSize);
                view->addVertex(v0);
                view->setTexCoord(tCenter.x+tSize, tCenter.z-tSize);
                view->addVertex(v1);
                view->setTexCoord(tCenter.x+tSize, tCenter.z+tSize);
                view->addVertex(v2);
                view->setTexCoord(tCenter.x-tSize, tCenter.z+tSize);
                view->addVertex(v3);
                view->endPolygon();
                
                view->rasterizer->flags &= ~Rasterizer::FlagTileTexture;
        }
#endif
                
        void setupView()
        {
                scalar angle = t % (2*PI);
                scalar x = FPMul(FPSin(angle), FPInt(3)) + FPInt(0);
                scalar z = FPMul(FPCos(angle), FPInt(1)) + FPInt(0);
                
//              printf("%d, %d, %d\n", t, framework->getTickCount()/10, framework->getTicksPerSecond());

                view->rasterizer->setTexture(texture);

                view->camera.target = Vector(FPInt(0),FPInt(0),FPInt(0));
                view->camera.origin = Vector(x,FPInt(3)>>4,z);

//              view->camera.origin = Vector(FPInt(2),FPInt(3)>>4,FPInt(0));
//              view->camera.target = Vector(x,0,z);
                                
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
        
        void renderVideo(Game::Surface* screen)
        {
                screen->clear();
#if 1
                t = (100*framework->getTickCount() / framework->getTicksPerSecond()) << (FP-8);

		setupView();

//              renderBackground(screen);
//              renderLand();
                sky->render(view);
                ground->render(view);
                track->render(view);

		rasterizer->flags &= ~Rasterizer::FlagPerspectiveCorrection;
		object->render(view);
		rasterizer->flags |= Rasterizer::FlagPerspectiveCorrection;
		
//                renderRasterizerTest();
		
//                renderObjectTest(view);
#endif
		renderAudioBuffer(screen);
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
#ifdef EPOC                     
                        case EStdKeyDevice0:
                                framework->exit();
                        break;
#else
                        case SDLK_ESCAPE:
                                framework->exit();
                        break;
#endif
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
