#include "engine/Engine.h"
#include <stdio.h>

using namespace Game;

Engine::Engine(Framework* _framework):
	framework(_framework)
{
}

Engine::~Engine()
{
}

Framework::Framework()
{
}

Framework::~Framework()
{
}

Surface	*Framework::loadImage(const char *name, PixelFormat *pf)
{
	return 0;
}

SampleChunk *Framework::loadSample(const char *name, SampleFormat *sf)
{
	return 0;
}
