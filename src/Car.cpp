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
#include "models/CarModel.h"
#include "Track.h"
#include <stdio.h>

Car::Car(World *_world):
	angle(0),
	speed(0),
	angleSpeed(0),
	velocity(0,0,0),
	origin(0,0,0),
	thrust(false),
	brake(false),
	steering(0),
	steeringWheelPos(0)
{
	object = new CarModel(FPInt(1)>>8, _world->getFramework()->loadImage(_world->getFramework()->findResource("car.png"), &_world->getScreen()->format));

	// build an acceleration profile
	accProfile[0].acc = 1000;
	accProfile[0].angleAcc = 200;
	accProfile[0].threshold = 1000;
	accProfile[1].acc = 300;
	accProfile[1].angleAcc = 180;
	accProfile[1].threshold = 2000;
	accProfile[2].acc = 100;
	accProfile[2].angleAcc = 150;
	accProfile[2].threshold = 6000;
	accProfile[3].acc = 0;
	accProfile[3].angleAcc = 90;
	accProfile[3].threshold = 0x7fffffff;
}

Car::~Car()
{
	delete object;
}

#define DAMPEN(x,amount) if (x) x += (x>(amount))?(-(amount)):(amount)

void Car::update(Track *track)
{
	Vector acceleration(0,0,0);
	
//	velocity.x = FPMul(FPCos(angle), speed);
//	velocity.z = FPMul(FPSin(angle), speed);
	
	speed = velocity.length();

//	printf("Speed: %d\n", speed);
//	printf("Vel: %6d, %6d\n", velocity.x, velocity.z);
	
	// collision detection & response
	if (0 && track->getCell(origin) == 0)
	{
		Vector normal = track->getNormal(origin);
		
		// backtrack to avoid collision
//		origin -= velocity * FPInt(1);
		
		// rotate the normal 90 degress
		normal.y = normal.x;
		normal.x = -normal.z;
		normal.z = normal.y;
		normal.y = 0;
		
		scalar p = velocity.dot(normal);
		
//		if (p < 0) p = -p;
		
		velocity = normal * p - velocity * (FPInt(1)>>2);
		printf("Crash %d\n", p);
		
//		speed = 0;
//		speed = FPMul(speed, FPInt(-1));
//		origin += normal * 10000;
	}
		
	if (thrust)
	{
//		speed += getAcceleration();
		scalar acc = getAcceleration();
		acceleration.x += FPMul(FPCos(angle), acc);
		acceleration.z += FPMul(FPSin(angle), acc);
		printf("Acc: %d\n", acc);
	} else
	{
		scalar acc = speed >> 4;
		acceleration.x += FPMul(FPCos(angle), acc);
		acceleration.z += FPMul(FPSin(angle), acc);
		printf("Spd: %d\n", acc);
	}
	
	switch(steering)
	{
	case -1:
		if (steeringWheelPos > -8) steeringWheelPos--;
	break;
	case 0:
		if (steeringWheelPos > 0) steeringWheelPos--;
		else if (steeringWheelPos < 0) steeringWheelPos++;
	break;
	case 1:
		if (steeringWheelPos < 8) steeringWheelPos++;
	break;
	}
		
//	printf("%d\n", steeringWheelPos);
		
//	angleSpeed += (steeringWheelPos) * getAngleAcceleration();

	if (speed > 200 || speed < -200)
		angle += steeringWheelPos * getAngleAcceleration();

//	printf("Acc: %6d, %6d\n", acceleration.x, acceleration.z);
		
	// integration
//	acceleration *= (FPInt(1)>>5);
	velocity += acceleration;
	origin += velocity;
	angle += angleSpeed;
		
	// bring angle back in range
	while(angle > 2*PI)
		angle -= 2*PI;

	while(angle < 0)
		angle += 2*PI;
		
	DAMPEN(speed, brake?32:4);
	DAMPEN(angleSpeed, 1);
}

scalar Car::getAcceleration()
{
	int i = 0;
	
	while(speed > accProfile[i].threshold && (i < sizeof(accProfile)/sizeof(accProfile[0])))
		i++;
		
//	printf("%6d -> acceleration segment %d: %d\n", speed, i, accProfile[i].acc);
		
	return accProfile[i].acc;
}

scalar Car::getAngleAcceleration()
{
	int i = 0;
	
	while(speed > accProfile[i].threshold && (i < sizeof(accProfile)/sizeof(accProfile[0])))
		i++;
		
//	printf("%6d -> acceleration segment %d: %d\n", speed, i, accProfile[i].acc);
	return accProfile[i].angleAcc;
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

scalar Car::getAngle()
{
	return angle;
}

void Car::render(World *world)
{
	Vector axis(0,FP_ONE,0);
	Matrix translation = Matrix::makeTranslation(origin);

	object->transformation = Matrix::makeRotation(axis, angle + (steeringWheelPos<<(FP-6)));
	object->transformation *= translation;
	
	world->getView()->rasterizer->flags &= ~Rasterizer::FlagPerspectiveCorrection;
	object->render(world);
	world->getView()->rasterizer->flags |= Rasterizer::FlagPerspectiveCorrection;
}
