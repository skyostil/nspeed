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

#include "Car.h"
#include "Track.h"
#include "Environment.h"
#include "World.h"
#include "Mixer.h"
#include <stdio.h>

Car::Car(World *_world):
        Object(_world),
        angle(PI/2),
        speed(0),
        texture(0),
        mesh(0),
        engineSound(0),
        angleSpeed(0),
        velocity(0,0,0),
        origin(0,0,0),
        takingDamage(false),
        thrust(false),
        brake(false),
        steering(0),
        steeringWheelPos(0),
        thrustPos(0),
        engineCycle(0),
        gateIndex(0),
        lapCount(0),
        aiEnabled(false),
        carNumber(-1),
        energy(100),
        bestLapTime(-1),
        raceStart(0),
        lapStart(0),
        raceTime(-1),
        finishingRank(1),
        world(_world)
{
    // build a default acceleration profile
    accProfile[0].acc = 96;
    accProfile[0].angleAcc = 96;
    accProfile[0].threshold = 1200*2;
    accProfile[1].acc = 64;
    accProfile[1].angleAcc = 72;
    accProfile[1].threshold = 3000*2;
    accProfile[2].acc = 16;
    accProfile[2].angleAcc = 64;
    accProfile[2].threshold = 4000*2;
    accProfile[3].acc = 12;
    accProfile[3].angleAcc = 64;
    accProfile[3].threshold = 0x7fffffff;
}

bool Car::load(const char *name)
{
    char fileName[256];
    FILE *f;

    unload();

    sprintf(fileName, "cars/%.64s/engine.wav", name);
    engineSound = world->getEnvironment()->getFramework()->loadSample(world->getEnvironment()->getFramework()->findResource(fileName));

    sprintf(fileName, "cars/%.64s/texture.png", name);
    texture = world->getEnvironment()->loadImage(fileName);

    sprintf(fileName, "cars/%.64s/mesh.mesh", name);
    mesh = new Mesh(this, world->getEnvironment()->getFramework()->findResource(fileName), texture);

    // build the default acceleration profile
    accProfile[0].acc = 96;
    accProfile[0].angleAcc = 96;
    accProfile[0].threshold = 1200*2;
    accProfile[1].acc = 64;
    accProfile[1].angleAcc = 72;
    accProfile[1].threshold = 3000*2;
    accProfile[2].acc = 16;
    accProfile[2].angleAcc = 64;
    accProfile[2].threshold = 4000*2;
    accProfile[3].acc = 12;
    accProfile[3].angleAcc = 64;
    accProfile[3].threshold = 0x7fffffff;

    // load the acceleration profile
    sprintf(fileName, "cars/%.64s/acceleration.dat", name);
    f = fopen(world->getEnvironment()->getFramework()->findResource(fileName), "r");

    if (f)
    {
        int i = 0;
        for(i=0; i<sizeof(accProfile)/sizeof(accProfile[0]); i++)
        {
            if (feof(f))
                break;
            fscanf(f, "%d %d %d", &accProfile[i].threshold, &accProfile[i].acc, &accProfile[i].angleAcc);
        }
        fclose(f);
    }
    return true;
}

void Car::unload()
{
    hide();
    world->getEnvironment()->scheduleSampleDeletion(engineSound);

    delete texture;
    texture = 0;

    delete mesh;
    mesh = 0;
}

Car::~Car()
{
    unload();
}

#define DAMPEN(x,amount) if (x) x += (x>(amount))?(-(amount)):(amount)

void Car::update(Track *track)
{
    Vector worldOrigin = origin * (FP_ONE>>CAR_COORDINATE_SCALE);
    bool airborne = (origin.y > 0);
    const scalar gravity = FPInt(1)>>8;

    if (carNumber <= 0 && energy == 0)
    {
        explode();
        return;
    }

    // reset the acceleration
    acceleration.set(0,0,0);

    // update the speedometer
    speed = velocity.lengthSquared();

    //      printf("Vel: %6d, %6d\n", velocity.x, velocity.z);

    // collision detection & response
    if (!airborne && track->getCell(worldOrigin) == 0)
    {
        Vector normal = track->getNormal(worldOrigin);
        //              Vector vel = velocity;
        int backtrackLimit = 32;

        //              vel.normalize();

        //              printf("%d\n", speed);

        // if we were going too slow, use the normal as the new velocity
        if (speed < 2048)
        {
            velocity = normal * -512;
            //                      printf("Crash %6d, %6d\n", normal.x, normal.z);
        }

        // backtrack to avoid collision
        while(track->getCell(origin * (FP_ONE>>CAR_COORDINATE_SCALE))==0 && --backtrackLimit)
            origin -= velocity;

        if (normal.dot(velocity) < 0)
            acceleration = normal * FPMul(normal.dot(velocity), FPInt(-2));
        else
            acceleration = normal * 512;

        if (acceleration.lengthSquared() < 32)
            acceleration += normal * (FP_ONE>>3);

        // printf("%d, %d\n", acceleration.lengthSquared(), normal.lengthSquared());
        // printf("%d\n", normal.dot(velocity));
    }
    else
    {
        if (thrust)
        {
            //              speed += getAcceleration();
            scalar acc = getAcceleration(speed);
            acceleration.x += (FPCos(angle) * acc) >> FP;
            acceleration.z += (FPSin(angle) * acc) >> FP;

            //              printf("Acc: %d\n", acc);
        }

        {
            //              scalar acc = speed >> 5;
            //                      if (speed > 1024)
            if (speed > 16)
            {
                Vector vel = velocity;
                Vector dir(FPSin(angle), 0, -FPCos(angle));

                scalar acc = vel.dot(dir);

                acc = FPDiv(acc, FPInt(speed)>>7);
                    
                acceleration -= dir * acc;
#if 0
                //                                      scalar slip = FPMul(steeringWheelPos<<8, speed * thrustPos);
                scalar slip = FPMul(steeringWheelPos<<6, FPSqrt(speed));
                scalar maxSlip = 4096;
                //                                      printf("slip: %6d\n", slip);

                if (slip > maxSlip)
                    slip = maxSlip;
                else if (slip < -maxSlip)
                    slip = -maxSlip;
#endif                    
            }
        }
    }

    if (!airborne)
    {
        updateCollisions();
        updateTileEffects(track);
    }

    switch(steering)
    {
    case -1:
        if (steeringWheelPos > -32) steeringWheelPos--;
        break;
    case 0:
        if (steeringWheelPos > 0) steeringWheelPos--;
        else if (steeringWheelPos < 0) steeringWheelPos++;
        break;
    case 1:
        if (steeringWheelPos < 32) steeringWheelPos++;
        break;
    }

    if (thrust && thrustPos < 8)
        thrustPos++;
    else if (!thrust && thrustPos > 0)
        thrustPos--;

    //      printf("%d\n", steeringWheelPos);

    //      angleSpeed += (steeringWheelPos) * getAngleAcceleration();

    if (FPAbs(velocity.x) > 32 || FPAbs(velocity.z) > 32)
        angle += (steeringWheelPos>>2) * getAngleAcceleration(speed);

    //      printf("Acc: %6d, %6d\n", acceleration.x, acceleration.z);

    // fall down if we're flying
    if (origin.y > 0)
        acceleration.y = -gravity;

    // integration
    //      acceleration *= (FPInt(1)>>5);
    velocity += acceleration;
    origin += velocity;
    angle += angleSpeed;

    if (origin.y < 0)
    {
        origin.y = 0;
        velocity.y = 0;
        acceleration.y = 0;
    }

    // bring angle back in range
    while(angle > 2*PI)
        angle -= 2*PI;

    while(angle < 0)
        angle += 2*PI;

    //      DAMPEN(speed, brake?32:4);

    if (airborne)
    {
        DAMPEN(velocity.x, 8);
        DAMPEN(velocity.z, 8);
    }
    else
    {
        DAMPEN(velocity.x, brake?64:8);
        DAMPEN(velocity.z, brake?64:8);
    }

    DAMPEN(angleSpeed, 1);

    // update the model position
    Vector verticalAxis(0,FP_ONE,0);
    Vector rollAxis(FPCos(angle),0,FPSin(angle));
    Matrix translation = Matrix::makeTranslation(origin * (FP_ONE>>CAR_COORDINATE_SCALE) + Vector(0, thrustPos<<(FP-10), 0));

    mesh->transformation = Matrix::makeRotation(verticalAxis, angle + (steeringWheelPos<<(FP-7)));
    mesh->transformation *= Matrix::makeRotation(rollAxis, -(steeringWheelPos<<(FP-8)));
    mesh->transformation *= translation;

    updateGate();

    if (carNumber == 0)
        updateSound();

    if (aiEnabled)
        updateAi();
}

void Car::updateTileEffects(Track *track)
{
    unsigned char tile = track->getCell(getOrigin());
	Environment *env = world->getEnvironment();

    takingDamage = false;

    if (tile == 0)
    {
        takingDamage = true;
        energy = 0;
    }
    else if (track->tileGivesEnergy(tile))
    {
		if (carNumber == 0)
		{
			if (env->getSfxChannel() && env->rechargeSound)
			{
				env->getSfxChannel()->playSample(env->rechargeSound, 11025);
			}
		}
        energy += 32768;
        if (energy > FPInt(100))
            energy = FPInt(100);
    }
    else if (track->tileBounces(tile))
    {
        const scalar maxJump = FPInt(2)>>3;
        acceleration.y = (speed << 3);
        if (acceleration.y > maxJump)
            acceleration.y = maxJump;
    }
    else if (track->tileIsDamaging(tile))
    {
		if (carNumber == 0)
		{
			if (env->getSfxChannel() && env->damageSound)
			{
				env->getSfxChannel()->playSample(env->damageSound, 8192);
			}
		}

        if (!aiEnabled)
        {
            energy -= 6000;
            if (energy < 0)
                energy = 0;
            takingDamage = true;
        }
        acceleration -= (velocity * (speed>>2));
    }
    else if (track->tileIsTurbo(tile))
    {
        const scalar turbo = FPInt(1)>>4;
        acceleration += (velocity * turbo);
    }
    else if (track->tileIsDirt(tile))
    {
        acceleration -= (velocity * (speed>>1));
    }
}

scalar Car::getAcceleration(scalar speed) const
{
    int i = 0;

    while(speed > accProfile[i].threshold && (i < sizeof(accProfile)/sizeof(accProfile[0])))
        i++;

    //      printf("%6d -> acceleration segment %d: %d\n", speed, i, accProfile[i].acc);

    return accProfile[i].acc;
}

void Car::updateSound()
{
    int freq;

    if (thrust)
        freq = (FPSqrt(speed) >> 2) + 2000;
    else
        freq = (FPSqrt(speed) >> 2) + 1000;

    if (world->getEnvironment()->mixer)
        world->getEnvironment()->getEngineSoundChannel()->setFrequency(freq);
}

void Car::updateGate()
{
    int nextGateIndex = (gateIndex+1) % world->getEnvironment()->track->getGateCount();
    LineSegment *nextGate = world->getEnvironment()->track->getGate(nextGateIndex);

    if (nextGate && nextGate->isInside(getOrigin()))
    {
        bool newLap = nextGateIndex < gateIndex;

        gateIndex = nextGateIndex;
                
        if (newLap)
        {
            int t = world->getEnvironment()->getTimeInMs();
   
            if (bestLapTime == -1 || t - lapStart < bestLapTime)
            {
                bestLapTime = t - lapStart;
            }

            lapStart = t;
            lapCount++;

            // we finished the race
            if (!hasFinished() && lapCount == world->getEnvironment()->track->getLapCount())
            {
                finishingRank = getRank();
                raceTime = t - raceStart;
                setAiState(true);
            }
        }
    }
}

scalar Car::getAngleAcceleration(scalar speed) const
{
    int i = 0;

    while(speed > accProfile[i].threshold && (i < sizeof(accProfile)/sizeof(accProfile[0])))
        i++;

    //      printf("%6d -> acceleration segment %d: %d\n", speed, i, accProfile[i].acc);
    return accProfile[i].angleAcc;
}

void Car::updateCollisions()
{
    int i;

    for(i=0; i<world->getEnvironment()->carPool.getCount(); i++)
    {
        Car *other = world->getEnvironment()->carPool.getItem(i);

        // check each car pair only once
        if (other > this)
        {
            checkCollision(other);
        }
    }
}

void Car::checkCollision(Car *other)
{
    scalar radius = FP_ONE * 2;
    Vector dist = other->origin - origin;
    const int collisionDampening = 8; // logarithmic

    // this peculiar double-test is needed because of numerical overflow
    if (FPAbs(dist.x) <= radius && FPAbs(dist.z) <= radius)
    {
        if (dist.lengthSquared() < radius)
        {
            Vector normal = (other->origin - origin).normalize();
            Vector relativeVelocity = velocity - other->velocity;

            // backtrack to avoid collision
            origin -= velocity * FPInt(2);

            scalar p = relativeVelocity.dot(normal) >> collisionDampening;

            velocity -= normal * p + relativeVelocity;
            other->velocity += normal * p + relativeVelocity;
        }
    }
}

void Car::setThrust(bool _thrust)
{
    thrust = _thrust;
}

void Car::setBrake(bool _brake)
{
    brake = _brake;
}

void Car::setSteering(int _steering)
{
    steering = _steering;
}

void Car::setOrigin(const Vector &o)
{
    origin = o * (FP_ONE<<CAR_COORDINATE_SCALE);
}

void Car::prepareForRace(int position)
{
    velocity.set(0,0,0);
    steeringWheelPos = 0;
    thrustPos = 0;
    lapCount = 0;
    gateIndex = 0;
    angleSpeed = 0;
    carNumber = position;
    energy = FPInt(100);

    bestLapTime = -1;
    raceTime = -1;
    lapStart = 0;
    raceStart = 0;

    setAiState(position > 0);

    // make sure the player starts last
    setOrigin(world->getEnvironment()->track->getStartingPosition(world->getEnvironment()->carPool.getCount() - position - 1));
    angle = world->getEnvironment()->track->getStartingAngle();

    if (world->getEnvironment()->mixer && engineSound && carNumber == 0)
    {
        world->getEnvironment()->getEngineSoundChannel()->playSample(engineSound, 1, true);
    }

    update(world->getEnvironment()->track);
}

void Car::startRace()
{
    bestLapTime = -1;
    raceTime = -1;
    lapStart = raceStart = world->getEnvironment()->getTimeInMs();
}

void Car::updateAi()
{
    Track *track = world->getEnvironment()->track;
    Vector o = getOrigin();
    Vector target, steerTarget, previousSteerTarget;
    const int probeCount = 4;
    int i;
    bool onDirt = track->tileIsDirt(track->getCell(o));
    Vector probe[probeCount];
    
//    if (carNumber == 0)
//        printf("%d\n", speed);

    if (speed < 1024)
    {
        // probe ahead
        probe[0] = o + (Vector(FPCos(angle), 0, FPSin(angle)) * (FPInt(1)>>3));
        probe[1] = o + (Vector(FPCos(angle), 0, FPSin(angle)) * (FPInt(1)>>2));
        probe[2] = o + (Vector(FPCos(angle), 0, FPSin(angle)) * (FPInt(1)>>1));
        probe[3] = o + (Vector(FPCos(angle), 0, FPSin(angle)) * (FPInt(1)));
    }
    else
    {
        // probe according to current velocity
        probe[0] = o + (velocity * FPInt(2));
        probe[1] = o + (velocity * FPInt(5));
        probe[2] = o + (velocity * FPInt(7));
        probe[3] = o + (velocity * FPInt(8));
    }

    // find out the nearest point on the AI path
    //    if (track->getNearestPointOnAiPath(probe, target))
    /*
        if (
            track->getNearestPointOnAiPath(probe[0], target) ||
            track->getNearestPointOnAiPath(probe[1], target) ||
            track->getNearestPointOnAiPath(probe[2], target) ||
            track->getNearestPointOnAiPath(probe[3], target)
        )
    */
    for(i=0; i<probeCount; i++)
    {
        if (track->getNearestPointOnAiPath(probe[i], target))
        {
/*
            if (carNumber == 0)
            {
                if (track->getCell(probe[i]))
                    track->setCell(probe[i]);
//                track->setCell(target);
            }
*/
            unsigned char tile = track->getCell(probe[i]);

            // don't get stuck
            if (speed < 128)
            {
                setThrust(true);
                setBrake(false);
                break;
            }
            
            // avoid emptyness and edges
            if (track->shouldAiAvoidTile(tile))
            {
                scalar distance = speed<<4;
                
                if (distance < (FPInt(1)))
                    distance = (FPInt(1));
                
                if (!track->getNextPointOnAiPath(o, steerTarget, distance))
                    continue;
//                steerTarget = target;

//                if (carNumber == 0)
//                    track->setCell(steerTarget, (world->getEnvironment()->getFramework()->getTickCount()&1)?1:2);

//                if (track->shouldAiAvoidTile(track->getCell(o + (velocity * FPInt(3)))))
                if (!onDirt && i < 2)
                    setBrake(true);
                
                if (i < probeCount-1)
                {
//                    scalar error = (probe[i] - o).normalize().dot((steerTarget - o).normalize());
                    scalar error = (probe[i] - o).normalize().dot((steerTarget - o).normalize());
                    
//                    if (carNumber == 0)
//                        printf("%d\t%d\n", error, speed);
                    
                    // something's ahead -> slow down and turn toward the path
                    if (!onDirt)
                        setThrust(false);

                    if (error > (FPInt(7)>>8) && speed < 800)
                    {
//                        steerTarget = previousSteerTarget;
                        track->getNextPointOnAiPath(o, steerTarget, FP_ONE>>1);
                    }
                                            
                    if (((probe[i] - o).cross(steerTarget - o)).y < 0)
                        setSteering(1);
                    else
                        setSteering(-1);
                    break;
                }
            }
            else
            {
                // no obstacles -> pedal to the metal
                setBrake(false);
                setThrust(true);
                setSteering(0);
            }
        }
    }
}

void Car::setAiState(bool enabled)
{
    aiEnabled = enabled;
    if (!aiEnabled)
    {
        setBrake(false);
        setThrust(false);
        setSteering(0);
    }
}

void Car::explode()
{
    origin.y = 0;
}

bool Car::hasFinished() const
{
    return raceTime != -1;
}

int Car::getLapTime() const
{
    return world->getEnvironment()->getTimeInMs() - lapStart;
}

int Car::getBestLapTime() const
{
    return bestLapTime;
}

int Car::getRaceTime() const
{
    if (hasFinished())
    {
        return raceTime;
    }
    else
    {
        return world->getEnvironment()->getTimeInMs() - raceStart;
    }
}

bool Car::isTakingDamage() const
{
    return takingDamage;
}

int Car::getRank() const
{
    int i;
    Track *track = world->getEnvironment()->track;
    int index = lapCount * track->getGateCount() + gateIndex;
    int rank = 1;
    int nextGateIndex = (gateIndex+1) % world->getEnvironment()->track->getGateCount();

    if (hasFinished())
    {
        return finishingRank;
    }

    for(i=0; i<world->getEnvironment()->carPool.getCount(); i++)
    {
        Car *other = world->getEnvironment()->carPool.getItem(i);
        int otherIndex = other->lapCount * track->getGateCount() + other->gateIndex;

        if (this == other)
            continue;

        if (index == otherIndex)
        {
            scalar dist, otherDist;

            dist = (getOrigin() - track->getGate(nextGateIndex)->getCenter()).manhattanNorm();
            otherDist = (other->getOrigin() - track->getGate(nextGateIndex)->getCenter()).manhattanNorm();

            if (otherDist < dist)
            {
                rank++;
            }
        }
        else if (index < otherIndex)
        {
            rank++;
        }
    }
    return rank;
}

void Car::adjustTimes(int delta)
{
    lapStart += delta;
    raceStart += delta;
}

void Car::show()
{
    world->getEnvironment()->meshPool.add(mesh);
}

void Car::hide()
{
    world->getEnvironment()->meshPool.remove(mesh);
}

