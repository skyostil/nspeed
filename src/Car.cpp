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

Car::Car(World *_world, const char *name):
        Object(_world),
        angle(PI/2),
        speed(0),
        texture(0),
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
        world(_world)
{
        char fileName[256];
		FILE *f;

        sprintf(fileName, "cars/%.64s/engine.wav", name);
        engineSound = world->getEnvironment()->getFramework()->loadSample(world->getEnvironment()->getFramework()->findResource(fileName));

        sprintf(fileName, "cars/%.64s/texture.png", name);
        texture = world->getEnvironment()->loadImage(fileName);

        sprintf(fileName, "cars/%.64s/mesh.mesh", name);
        mesh = new Mesh(this, world->getEnvironment()->getFramework()->findResource(fileName), texture);

        world->getEnvironment()->meshPool.add(mesh);

        // build an acceleration profile
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
		f = fopen(fileName, "r");
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
}

Car::~Car()
{
        world->getEnvironment()->meshPool.remove(mesh);
        delete texture;
        delete engineSound;
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
                int backtrackLimit = 16;

//              vel.normalize();

//              printf("%d\n", speed);

                // if we were going too slow, use the normal as the new velocity
                if (speed < 256)
                {
                        velocity = normal * -512;
//                      printf("Crash %6d, %6d\n", normal.x, normal.z);
                }
                
                // backtrack to avoid collision
                while(track->getCell(origin * (FP_ONE>>CAR_COORDINATE_SCALE))==0 && --backtrackLimit)
                        origin -= velocity;
/*              
                // rotate the normal 90 degress
                normal.y = normal.x;
                normal.x = -normal.z;
                normal.z = normal.y;
                normal.y = 0;
*/              
//              scalar p = velocity.dot(normal);
//              scalar p = vel.dot(normal);
/*
                // make sure we bounce off the wall
                if (p > -(FP_ONE) && p < 0)
                        p = -(FP_ONE);
                else if (p < (FP_ONE) && p > 0)
                        p = (FP_ONE);
*/
//              if (p < 0) p = -p;
                
//              velocity = normal * p - velocity * (FPInt(1)>>2);
//              acceleration = normal * p - velocity * (FPInt(1)>>5);
//              acceleration = normal * p - velocity * (FP_ONE + 3*FP_ONE/4);
//              acceleration = normal * p - velocity * (FP_ONE<<1);
//              acceleration = -velocity + (vel - normal * FPMul(FPInt(2), p)) * speed;
//              acceleration = -velocity + (vel - normal * FPMul(FPInt(2), p)) * speed + normal * 1000;

                if (normal.dot(velocity) < 0)
                        acceleration = normal * FPMul(normal.dot(velocity), FPInt(-2));
                else
                        acceleration = normal * 512;

                if (acceleration.lengthSquared() < 128)
                        acceleration += normal * 256;

//              printf("%d\n", acceleration.lengthSquared());
//              printf("%d\n", normal.dot(velocity));
                
//              speed = 0;
//              speed = FPMul(speed, FPInt(-1));
//              origin += normal * 10000;
        } else
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
        //                      Vector dir(FPCos(angle), 0, FPSin(angle));
                                Vector dir(FPSin(angle), 0, -FPCos(angle));
        //                      Vector dir(FPMul(FPCos(angle), speed), 0, FPMul(FPSin(angle), speed));
        //                      Vector dir(FPCos(angle + (PI>>1)), 0, FPSin(angle + (PI>>1)));
        //                      Vector acc(FPCos(angle), 0, FPSin(angle));

//                              vel.normalize();

                                scalar acc = vel.dot(dir);

        //                      if (acc < 0)
        //                              acc = -acc;

                                acc = FPDiv(acc, FPInt(speed)>>7);
//                              printf("Angle acc: %6d\n", acc);


                                {
//                                      acc>>=7;
                                //      acc>>=8;
        //                              Vector dir(FPCos((angle + (PI>>1)) % 2*PI), 0, FPSin((angle + (PI>>1))) % 2*PI);
        //                              Vector dir(FPSin(angle), 0, -FPCos(angle));

                                        acceleration -= dir * acc;

//                                      scalar slip = FPMul(steeringWheelPos<<8, speed * thrustPos);
                                        scalar slip = FPMul(steeringWheelPos<<6, FPSqrt(speed));
                                        scalar maxSlip = 4096;
//                                      printf("slip: %6d\n", slip);

                                        if (slip > maxSlip)
                                                slip = maxSlip;
                                        else if (slip < -maxSlip)
                                                slip = -maxSlip;

                                        // slip
//                                      acceleration += dir * slip;
        /*
                                        if (vel.cross(dir).y > 0)
                                                acceleration += dir * acc;
                                        else
                                                acceleration += dir * -acc;
        */
        //                              printf("Turn: %6d\n", acc);
                                }
        //                              printf("No Turn: %6d\n", acc);

        /*
                                scalar acc = ((FP_ONE - vel.dot(dir))) >> 9;

                                if (vel.cross(dir).y > 0)
                                        acc = -acc;

                                acceleration.x += FPMul(FPCos(angle + (PI>>1)), acc);
                                acceleration.z += FPMul(FPSin(angle + (PI>>1)), acc);
                                printf("Turn: %6d, %d\n", acc, vel.cross(dir).y);
        */
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

        DAMPEN(velocity.x, brake?64:8);
        DAMPEN(velocity.z, brake?64:8);
        DAMPEN(angleSpeed, 1);

        // update the model position
        Vector verticalAxis(0,FP_ONE,0);
        Vector rollAxis(FPCos(angle),0,FPSin(angle));
        Matrix translation = Matrix::makeTranslation(origin * (FP_ONE>>CAR_COORDINATE_SCALE) + Vector(0, thrustPos<<(FP-10), 0));

        mesh->transformation = Matrix::makeRotation(verticalAxis, angle + (steeringWheelPos<<(FP-8)));
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

    takingDamage = false;
	
	if (tile == 0)
	{
        takingDamage = true;
		energy = 0;
	}
	else if (track->tileGivesEnergy(tile))
	{
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
        if (!aiEnabled)
        {
		    energy -= 6000;
		    if (energy < 0)
			    energy = 0;
            takingDamage = true;
        }
        acceleration -= (velocity * (speed>>3));
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
                freq = (FPSqrt(speed) << 2) + 16000;
        else
                freq = (FPSqrt(speed) << 2) + 8000;

        if (world->getEnvironment()->mixer)
                world->getEnvironment()->getEngineSoundChannel()->setFrequency(freq);
}

void Car::updateGate()
{
        int nextGateIndex = (gateIndex+1) % world->getEnvironment()->track->getGateCount();
        LineSegment *nextGate = world->getEnvironment()->track->getGate(nextGateIndex);
        
        if (nextGate && nextGate->isInside(getOrigin()))
        {
                if (nextGateIndex < gateIndex)
                {
                        int t = getTime();

                        if (t - lapStart < bestLapTime)
                        {
                            bestLapTime = t - lapStart;
                        }

                        lapStart = t;
                        lapCount++;

                        // we finished the race
                        if (lapCount >= world->getEnvironment()->track->getLapCount())
                        {
                            raceTime = t - raceStart;
                            setAiState(true);
                        }
                }
                gateIndex = nextGateIndex;
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

        // this peculiar double-test is needed because of numerical overflow
        if (FPAbs(dist.x) <= radius && FPAbs(dist.z) <= radius)
        {
                if (dist.lengthSquared() < radius)
                {
                        Vector normal = (other->origin - origin).normalize();
                        Vector relativeVelocity = velocity - other->velocity;

                        // backtrack to avoid collision
                        origin -= velocity * FPInt(2);

                        scalar p = relativeVelocity.dot(normal) >> 1;

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
        
        setOrigin(world->getEnvironment()->track->getStartingPosition(position));
        angle = world->getEnvironment()->track->getStartingAngle();
        
        if (world->getEnvironment()->mixer && engineSound && carNumber == 0)
        {
                world->getEnvironment()->getEngineSoundChannel()->playSample(engineSound, 1, true);
        }
        
        update(world->getEnvironment()->track);
}

int Car::getTime() const
{
        return 1000 * world->getEnvironment()->getFramework()->getTickCount() / world->getEnvironment()->getFramework()->getTicksPerSecond();
}

void Car::startRace()
{
        bestLapTime = -1;
        raceTime = -1;
        lapStart = raceStart = getTime();
}

void Car::updateAi()
{
        Track *track = world->getEnvironment()->track;
        Vector o = getOrigin();
        Vector probe;
        Vector target;

        if (speed < 128)
        {
                // probe ahead
                probe = o + (Vector(FPCos(angle), 0, FPSin(angle)) * FPInt(1));
        } else
        {
                // probe according to current velocity
                probe = o + (velocity * FPInt(8));
        }

        // find out the nearest point on the AI path
        if (track->getNearestPointOnAiPath(probe, target))
        {
//              track->setCell(probe);
//              if (carNumber == 0)
//                      track->setCell(target);

                unsigned char tile = track->getCell(probe);

                // avoid emptyness and edges
                if (track->shouldAiAvoidTile(tile))
                {
                        // something's ahead -> slow down and turn toward the path
                        setThrust(false);

                        if (track->shouldAiAvoidTile(track->getCell(o + (velocity * FPInt(3)))))
                                setBrake(true);

                        if (((probe - o).cross(target - o)).y < 0)
                                setSteering(1);
                        else
                                setSteering(-1);

                } else
                {
                        // no obstacles -> pedal to the metal
                        setBrake(false);
                        setThrust(true);
                        setSteering(0);
                }

                // don't get stuck
                if (speed < 64)
                {
                        setThrust(true);
                        setBrake(false);
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
    return getTime() - lapStart;
}

int Car::getBestLapTime() const
{
    return bestLapTime;
}

int Car::getRaceTime() const
{
    if (hasFinished())
        return raceTime;
    else
        return getTime() - raceStart;
}

bool Car::isTakingDamage() const
{
    return takingDamage;
}

