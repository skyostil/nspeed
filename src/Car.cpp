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
	acceleration(0),
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
	accProfile[0].acc = 40;
	accProfile[0].angleAcc = 200;
	accProfile[0].threshold = 300;
	accProfile[1].acc = 30;
	accProfile[1].angleAcc = 180;
	accProfile[1].threshold = 1500;
	accProfile[2].acc = 10;
	accProfile[2].angleAcc = 150;
	accProfile[2].threshold = 4000;
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
	// collision detection & response
	if (track->getCell(origin) == 0)
	{
		printf("Crash!\n");
		
		// backtrack to avoid collision
		origin -= velocity;
		speed = FPMul(speed, FPInt(-1));
	}

	// integration
	velocity.x = FPMul(FPCos(angle), speed);
	velocity.z = FPMul(FPSin(angle), speed);
	origin += velocity;
	angle += angleSpeed;
	
	// bring angle back in range
	while(angle > 2*PI)
		angle -= 2*PI;

	while(angle < 0)
		angle += 2*PI;
		
	if (thrust)
		speed += getAcceleration();
		
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
	
	printf("%d\n", steeringWheelPos);
		
//	angleSpeed += (steeringWheelPos) * getAngleAcceleration();

	if (speed > 200 || speed < -200)
		angle += steeringWheelPos * getAngleAcceleration();
			
	DAMPEN(speed, brake?32:4);
	DAMPEN(angleSpeed, 1);
}

scalar Car::getAcceleration()
{
	int i = 0;
	
	while(speed > accProfile[i].threshold && (i < sizeof(accProfile)/sizeof(accProfile[0])))
		i++;
		
	printf("%6d -> acceleration segment %d: %d\n", speed, i, accProfile[i].acc);
		
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
