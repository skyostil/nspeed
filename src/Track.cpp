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

#include "Config.h"
#include "Track.h"
#include "Car.h"
#include "Environment.h"
#include "TagFile.h"

#include <stdio.h>
#include <string.h>

Track::Track(Object *parent, Environment *_env):
        Object(parent),
        env(_env),
        texture(0),
        groundTexture(0),
        skyTexture(0),
        land(0),
        map(0),
        ground(0),
        aiPath(0),
        aiPathLength(0),
        bestLapTime(-1),
        bestTotalTime(-1)
{
    int i;

    // clear the record holders' names
    memset(bestLapName, ' ', sizeof(bestLapName));
    memset(bestTotalName, ' ', sizeof(bestTotalName));
    bestLapName[0] = 0;
    bestTotalName[0] = 0;

    for(i=0; i<sizeof(textureTileList)/sizeof(textureTileList[0]); i++)
    {
        textureTileList[i] = 0;
    }
}

Track::~Track()
{
    unload();
}

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif

typedef struct
{
    unsigned short w, h;
    signed short gate1LeftX, gate1LeftY, gate1RightX, gate1RightY;
    signed short gate2LeftX, gate2LeftY, gate2RightX, gate2RightY;
    signed short gate3LeftX, gate3LeftY, gate3RightX, gate3RightY;
    signed short gate4LeftX, gate4LeftY, gate4RightX, gate4RightY;
}
PACKED MapHeader;

typedef struct
{
    unsigned short x, y;
}
PACKED AiPathNode;

typedef struct
{
    char name[16];
    int time;
}
PACKED TimeEntry;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

bool Track::load(const char *name, int mapScale, bool loadOnlyMetadata)
{
    unsigned short w=0, h=0;
    Game::PixelFormat pf(8);
    char fileName[256];
    int i;

    unload();

    sprintf(fileName, "tracks/%.64s/track.trk", name);
    TagFile file(env->getFramework()->findResource(fileName));

    while(1)
        switch(file.readTag())
        {
        case 0: // map dimensions
            {
                MapHeader header;

                file.readData((unsigned char*)&header, sizeof(header));
                w = header.w;
                h = header.h;

                gate[0].set(unproject(header.gate1LeftX, header.gate1LeftY), unproject(header.gate1RightX, header.gate1RightY));
                gate[1].set(unproject(header.gate2LeftX, header.gate2LeftY), unproject(header.gate2RightX, header.gate2RightY));
                gate[2].set(unproject(header.gate3LeftX, header.gate3LeftY), unproject(header.gate3RightX, header.gate3RightY));
                gate[3].set(unproject(header.gate4LeftX, header.gate4LeftY), unproject(header.gate4RightX, header.gate4RightY));
            }
            break;
        case 1: // map data
            {
                if (w && h)
                {
                    texture = new Game::Surface(&pf, w, h);
                    land = new Land(texture);
                    file.readData((unsigned char*)texture->pixels, texture->bytes);
                }
                else
                {
                    unload();
                    return false;
                }
            }
            break;
        case 2: // ai path
            {
                scalar halfTile = FP_ONE >> 4;
                aiPathLength = file.getDataSize() / sizeof(AiPathNode);
                aiPath = new LineSegment[aiPathLength];
                AiPathNode *aiPathNodes = new AiPathNode[aiPathLength];
                Vector currNode, nextNode;

                file.readData((unsigned char*)aiPathNodes, aiPathLength * sizeof(AiPathNode));

                currNode = unproject(aiPathNodes[0].x, aiPathNodes[0].y);
                for (i=1; i<aiPathLength+1; i++)
                {
                    nextNode = unproject(aiPathNodes[i%aiPathLength].x, aiPathNodes[i%aiPathLength].y);
                    nextNode += Vector(halfTile, 0, halfTile);
                    aiPath[i-1].set(currNode, nextNode);
                    currNode = nextNode;
                }

                delete[] aiPathNodes;
            }
            break;
        default:
            {
                int x, y;

                if (!loadOnlyMetadata)
                {
                    if (!texture)
                        return false;

                    for(y=0; y<texture->height; y++)
                        for(x=0; x<texture->width; x++)
                        {
                            unsigned char tile = ((unsigned char*)texture->pixels)[y * texture->width + x];
                            if (tile > 0 && !textureTileList[tile])
                            {
                                sprintf(fileName, "tracks/%.64s/tile%d.png", name, tile);
                                if ((textureTileList[tile] = env->loadImage(fileName)) == NULL)
                                {
                                    sprintf(fileName, "tracks/tile%d.png", tile);
                                    if ((textureTileList[tile] = env->loadImage(fileName)) == NULL)
                                        ((unsigned char*)texture->pixels)[y * texture->width + x] = 0;
                                }
                            }
                        }

                    sprintf(fileName, "tracks/%.64s/ground.png", name);
                    groundTexture = env->loadImage(fileName);

                    if (!groundTexture)
                        groundTexture = env->loadImage("tracks/ground.png");

                    sprintf(fileName, "tracks/%.64s/sky.png", name);
                    skyTexture = env->loadImage(fileName);

                    if (!skyTexture)
                        skyTexture = env->loadImage("tracks/sky.png");

                    ground = new Land(groundTexture, skyTexture, Rasterizer::FlagPerspectiveCorrection, 2 /* textureScale */, FPInt(6));
                }

                initializeMap(mapScale);

                if (loadOnlyMetadata)
                {
                    delete texture;
                    texture = 0;
                }

                // load best times
                sprintf(fileName, "tracks/%.64s/times.tag", name);
                TagFile timeFile(env->getFramework()->findResource(fileName));
                TimeEntry timeEntry;

                while(1) switch(timeFile.readTag())
                    {
                    case 0: // best lap
                        timeFile.readData((unsigned char*)&timeEntry, sizeof(timeEntry));
                        setBestLapTime(timeEntry.time, timeEntry.name);
                        break;
                    case 1: // best total
                        timeFile.readData((unsigned char*)&timeEntry, sizeof(timeEntry));
                        setBestTotalTime(timeEntry.time, timeEntry.name);
                        break;
                    default:
                        return true;
                    }
            }
        }
}

bool Track::saveTimes(const char *name)
{
    char fileName[256];
    TimeEntry timeEntry;

    sprintf(fileName, "tracks/%.64s/times.tag", name);
    WriteTagFile timeFile(env->getFramework()->findResource(fileName, false));

    timeEntry.time = getBestLapTime(timeEntry.name, sizeof(timeEntry.name));
    strncpy(timeEntry.name, bestLapName, sizeof(timeEntry.name));
    timeFile.writeTag(0, (const unsigned char*)&timeEntry, sizeof(timeEntry));
    timeEntry.time = getBestTotalTime(timeEntry.name, sizeof(timeEntry.name));
    strncpy(timeEntry.name, bestTotalName, sizeof(timeEntry.name));
    timeFile.writeTag(1, (const unsigned char*)&timeEntry, sizeof(timeEntry));

    return true;
}

void Track::unload()
{
    int i;

    for(i=0; i<sizeof(textureTileList)/sizeof(textureTileList[0]); i++)
    {
        delete textureTileList[i];
        textureTileList[i] = 0;
    }

    delete texture;
    texture = 0;

    delete land;
    land = 0;

    delete ground;
    ground = 0;

    delete skyTexture;
    skyTexture = 0;

    delete groundTexture;
    groundTexture = 0;

    delete map;
    map = 0;

    delete[] aiPath;
    aiPath = 0;
    aiPathLength = 0;
}

void Track::render(World *world)
{
    if (ground)
    {
        ground->render(world);
    }

    if (land)
    {
        env->getView()->rasterizer->setTextureTileList(textureTileList);
        land->render(world);
    }
}

void Track::initializeMap(int scale)
{
    unsigned int x, y;

    if (!texture)
        return;

    map = new Game::Surface(&env->getScreen()->format, texture->width/scale, texture->height/scale);

    Game::PixelFormat &format = env->getScreen()->format;
    Game::Pixel road = format.makePixel(255, 255, 255);
    Game::Pixel edge = format.makePixel(16, 16, 16);

    for(y=0; y<texture->height-scale; y+=scale)
        for(x=0; x<texture->width-scale; x+=scale)
        {
            int px = x/scale;
            int py = y/scale;
            if (texture->getPixel((x+128)&0xff,(y+128)&0xff))
                map->setPixel(px, py, road);
            else if (
                texture->getPixel((x+128)&0xff,(y+128+scale)&0xff) ||
                texture->getPixel((x+128)&0xff,(y+128-scale)&0xff) ||
                texture->getPixel((x+128+scale)&0xff,(y+128)&0xff) ||
                texture->getPixel((x+128-scale)&0xff,(y+128)&0xff)
            )
                map->setPixel(px, py, edge);
        }
    /*
            int i;
     
            for(i=0; i<env->carPool.getCount(); i++)
            {
                    Car *car = env->carPool.getItem(i);
                    unsigned char mx, my;
     
                    project(car->getOrigin(), mx, my);
     
                    mx-=128;
                    my-=128;
     
                    x = env->getScreen()->width - 1 - texture->width/scale + (mx)/scale;
                    y = env->getScreen()->height - 1 - texture->height/scale + (my)/scale;
     
                    env->getScreen()->setPixel(x, y, 0xff00);
                    env->getScreen()->setPixel(x-1, y, 0);
                    env->getScreen()->setPixel(x, y-1, 0);
                    env->getScreen()->setPixel(x+1, y, 0);
                    env->getScreen()->setPixel(x, y+1, 0);
            }
    */
}


int Track::getCell(const Vector &pos) const
{
    unsigned char x, y;

    project(pos, x, y);

    return lookup(x,y);
}

void Track::setCell(const Vector &pos, unsigned char c)
{
    unsigned char x, y;

    project(pos, x, y);

    ((Game::Pixel8*)(texture->pixels))[x + (y<<8)] = c;
}

Vector Track::getNormal(const Vector &pos) const
{
    const int size = 4; // sample square size / 2
    unsigned char cx, cy;
    int px, py;
    Vector normal(0,0,0);

    project(pos, cx, cy);

    for(py=cy-size; py<cy+size; py++)
        for(px=cx-size; px<cx+size; px++)
        {
            Game::Pixel8 p = lookup((unsigned char)px, (unsigned char)py);

            if (p)
            {
                // XXX - possible sign issues here
                normal.x += FPInt(px - cx);
                normal.z += FPInt(py - cy);
            }
        }

    normal /= FPInt(size*size);

    return normal.normalize();
}

void Track::project(const Vector &pos, unsigned char &x, unsigned char &y) const
{
    x = (pos.x << 3) >> FP;
    y = (pos.z << 3) >> FP;
}

Vector Track::unproject(unsigned char x, unsigned char y) const
{
    return Vector(((x-128) << FP) >> 3, 0, ((y-128) << FP) >> 3);
}

LineSegment::LineSegment():
        valid(false),
        angle(0)
{}

bool LineSegment::isInside(const Vector &pos) const
{
    if (!valid) return false;

    Vector planarPos(pos.x, 0, pos.z);
    scalar t = FPDiv((planarPos - left).dot(leftToRight), lengthSquared);
    const scalar epsilon = 1<<(FP-8);

    if (t >= 0 && t <= FP_ONE)
    {
        Vector p = left + leftToRight * t;
        if ((planarPos - p).lengthSquared() < epsilon)
            return true;
    }
    return false;
}

void LineSegment::set(const Vector &_left, const Vector &_right)
{
    left = _left;
    right = _right;
    center = (left + right) * (FP_ONE/2);
    leftToRight = right - left;
    lengthSquared = leftToRight.lengthSquared();
    normal.set(-leftToRight.z, 0, leftToRight.x);
    normal.normalize();

    if (lengthSquared == 0)
        valid = false;
    else
    {
        valid = true;

        if (FPAbs(leftToRight.z) > FPAbs(leftToRight.x))
        {
            angle = FPArcTan2(leftToRight.z, leftToRight.x) + PI/2;
        }
        else
        {
            angle = -FPArcTan2(leftToRight.x, leftToRight.z) + PI;
        }
    }
}

LineSegment *Track::getGate(unsigned int index)
{
    if (index < sizeof(gate)/sizeof(gate[0]))
        return gate[index].isValid() ? (&gate[index]) : NULL;
    return NULL;
}

Vector Track::getStartingPosition(int carNumber) const
{
    Vector dir = gate[0].getLeftToRight() * (FP_ONE / 8);

    if (carNumber & 1)
        dir = -dir;

    return gate[0].getCenter() + dir + (gate[0].getNormal() * (FPInt(carNumber/2+1)>>1));
}

scalar Track::getStartingAngle() const
{
    return gate[0].getAngle() + PI/2;
}

bool LineSegment::getNearestPoint(const Vector &pos, Vector &out) const
{
    if (!valid) return false;

    Vector planarPos(pos.x, 0, pos.z);
    scalar t = FPDiv((planarPos - left).dot(leftToRight), lengthSquared);

    if (t >= 0 && t <= FP_ONE)
    {
        out = left + leftToRight * t;
        return true;
    }
    return false;
}

bool Track::getNearestPointOnAiPath(const Vector &pos, Vector &out) const
{
    int i;
    scalar nearest = FPInt(4096), dist;
    Vector point;
    bool foundAny = false;

    for(i=0; i<aiPathLength; i++)
    {
        if (aiPath[i].getNearestPoint(pos, point))
        {
            dist = (point - pos).lengthSquared();

            if (dist < nearest)
            {
                out = point;
                nearest = dist;
                foundAny = true;
            }
        }
    }

    // see if a corner is nearer
    for(i=0; i<aiPathLength; i++)
    {
        dist = (pos - aiPath[i].getLeft()).lengthSquared();

        if (dist < nearest)
        {
            out = aiPath[i].getLeft();
            nearest = dist;
            foundAny = true;
        }
    }

    return foundAny;
}

bool Track::getNextPointOnAiPath(const Vector &pos, Vector &out, scalar distance) const
{
    int i;
    scalar nearest = FPInt(4096), dist;
    Vector point, nearestPoint;
    int nearestIndex = -1;
    
    for(i=0; i<aiPathLength; i++)
    {
        if (aiPath[i].getNearestPoint(pos, point))
        {
            dist = (point - pos).manhattanNorm();

            if (dist < nearest)
            {
                nearest = dist;
                nearestPoint = point;
                nearestIndex = i;
            }
        }
    }
    
    for(i=0; i<aiPathLength; i++)
    {
        dist = (pos - aiPath[i].getLeft()).manhattanNorm();

        if (dist < nearest)
        {
            nearest = dist;
            nearestPoint = aiPath[i].getLeft();
            nearestIndex = i;
        }
    }
    
    
    if (nearestIndex == -1)
        return false;
        
    out = nearestPoint;
    while(distance > 0)
    {
        dist = (aiPath[nearestIndex].getRight() - pos).manhattanNorm();
//        dist = (aiPath[nearestIndex].getRight() - pos).length();
//        printf("%d\t%d\t%d\n", nearestIndex, distance,dist);
        
        if (dist < distance)
        {
            nearestIndex = (nearestIndex + 1) % aiPathLength;
            distance -= dist;
            out = aiPath[nearestIndex].getRight();
        } else
        {
            Vector dir = Vector(aiPath[nearestIndex].getLeftToRight()).normalize();
            out += dir * distance;
            break;
        }
    }
	return true;    
    
#if 0
    for(i=0; i<aiPathLength; i++)
    {
        if (aiPath[i].getNearestPoint(pos, point))
        {
//            dist = (point - pos).lengthSquared();
            dist = (point - pos).manhattanNorm();

            if (dist < nearest)
            {
//                out = point;
                nearest = dist;

                out = aiPath[i].getRight();
                previousOut = aiPath[i].getLeft();
                dist = (out - pos).manhattanNorm();
                                
                if (dist < limit)
                {
                    int next = (i + 1) % aiPathLength;
//                    printf("seuraava\t%d\n", dist);
                    out = aiPath[next].getRight();
                    previousOut = aiPath[next].getLeft();
                }
                else
                {
//                    printf("tämä\t%d\n", dist);
                }
                
//                out = aiPath[i].getRight();
//                nearestSegment = &aiPath[i];
//                nearestSegment = &aiPath[(i + 1) % aiPathLength];
            }
        }
    }

/*
    // if no nearest point was found, find the nearest corner
    if (!nearestSegment)
    {
        for(i=0; i<aiPathLength; i++)
        {
//            dist = (pos - aiPath[i].getLeft()).lengthSquared();
            dist = (pos - aiPath[i].getLeft()).manhattanNorm();

            if (dist < nearest)
            {
                if (dist < limit)
                {
                    printf("seuraava\n");
                    out = aiPath[i].getRight();
                }
                else
                {
                    printf("tämä\n");
                    out = aiPath[i].getLeft();
                }
                    
                nearest = dist;
                nearestSegment = &aiPath[i];
//                nearestSegment = &aiPath[(i + 1) % aiPathLength];
            }
        }
    }
*/    
/*    
    Vector d = (pos - out);
    scalar len =  d.manhattanNorm();
    scalar limit = FPInt(1)>>1;
    
    if (nearestSegment && len < limit)
    {
        printf("seuraava\t%d\n",len);
        out = nearestSegment->getRight();
    } else
        printf("tämä\t%d\n",len);
*/
//    return nearestSegment != NULL;
    return true;
#endif    
}

bool Track::shouldAiAvoidTile(unsigned char tile) const
{
    if (
        tile == 0 || 
        (tile >= Track::BorderStart && tile <= Track::BorderEnd) ||
        (tile >= Track::DirtStart && tile <= Track::DirtEnd)
        )
        return true;
    return false;
}

bool Track::tileIsDamaging(unsigned char tile) const
{
    if (tile == 0 || (tile >= Track::BorderStart && tile <= Track::BorderEnd))
        return true;
    return false;
}

bool Track::tileIsTurbo(unsigned char tile) const
{
    if (tile == 0 || (tile >= Track::TurboStart && tile <= Track::TurboEnd))
        return true;
    return false;
}

bool Track::tileIsDirt(unsigned char tile) const
{
    if (tile == 0 || (tile >= Track::DirtStart && tile <= Track::DirtEnd))
        return true;
    return false;
}

bool Track::tileGivesEnergy(unsigned char tile) const
{
    if (tile == 0 || (tile >= Track::RechargeStart && tile <= Track::RechargeEnd))
        return true;
    return false;
}

bool Track::tileBounces(unsigned char tile) const
{
    if (tile == 0 || (tile >= Track::BounceStart && tile <= Track::BounceEnd))
        return true;
    return false;
}


int Track::getBestLapTime(char *name, unsigned int nameSize)
{
    strncpy(name, bestLapName, nameSize);
    return bestLapTime;
}

int Track::getBestTotalTime(char *name, unsigned int nameSize)
{
    strncpy(name, bestTotalName, nameSize);
    return bestTotalTime;
}

void Track::setBestLapTime(int t, const char *name)
{
    strncpy(bestLapName, name, sizeof(bestLapName));
    bestLapTime = t;
}

void Track::setBestTotalTime(int t, const char *name)
{
    strncpy(bestTotalName, name, sizeof(bestTotalName));
    bestTotalTime = t;
}

int Track::getNearestGateIndex(const Vector &pos) const
{
    scalar nearest = FPInt(4096), dist;
    int i, nearestIndex = 0;

    for(i=0; i<getGateCount(); i++)
    {
        dist = (gate[i].getCenter() - pos).lengthSquared();
        if (dist < nearest)
        {
            nearest = dist;
            nearestIndex = i;
        }
    }
    return nearestIndex;
}
