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
	accProfile[0].acc = 60;
	accProfile[0].angleAcc = 250;
	accProfile[0].threshold = 3000;
	accProfile[1].acc = 30;
	accProfile[1].angleAcc = 200;
	accProfile[1].threshold = 4000;
	accProfile[2].acc = 10;
	accProfile[2].angleAcc = 150;
	accProfile[2].threshold = 8000;
	accProfile[3].acc = 3;
	accProfile[3].angleAcc = 150;
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
	if (1 && track->getCell(origin) == 0)
	{
		Vector normal = track->getNormal(origin);
		Vector vel = velocity;

		vel.normalize();
		
		// backtrack to avoid collision
		while(track->getCell(origin)==0)
			origin -= velocity;
		
		// rotate the normal 90 degress
		normal.y = normal.x;
		normal.x = -normal.z;
		normal.z = normal.y;
		normal.y = 0;
		
		scalar p = velocity.dot(normal);

//		if (p < 0) p = -p;
		
//		velocity = normal * p - velocity * (FPInt(1)>>2);
//		acceleration = normal * p - velocity * (FPInt(1)>>5);
//		acceleration = normal * p - velocity * (FP_ONE + 3*FP_ONE/4);
		acceleration = normal * p - velocity * (FP_ONE<<1);
//		acceleration = -velocity - (normal * FPInt(2) * normal.dot(vel) - vel) * speed;
		printf("Crash %d, %6d, %6d\n", p, acceleration.x, acceleration.z);
		
//		speed = 0;
//		speed = FPMul(speed, FPInt(-1));
//		origin += normal * 10000;
	} else
	{
		if (thrust)
		{
	//		speed += getAcceleration();
			scalar acc = getAcceleration(speed);
			acceleration.x += (FPCos(angle) * acc) >> FP;
			acceleration.z += (FPSin(angle) * acc) >> FP;
	//		printf("Acc: %d\n", acc);
		}

		{
	//		scalar acc = speed >> 5;
			if (speed > 2000)
			{
				Vector vel = velocity;
	//			Vector dir(FPCos(angle), 0, FPSin(angle));
				Vector dir(FPSin(angle), 0, -FPCos(angle));
	//			Vector dir(FPMul(FPCos(angle), speed), 0, FPMul(FPSin(angle), speed));
	//			Vector dir(FPCos(angle + (PI>>1)), 0, FPSin(angle + (PI>>1)));
	//			Vector acc(FPCos(angle), 0, FPSin(angle));

				vel.normalize();

	//			scalar acc = (FP_ONE - (vel.dot(dir))) >> 9;
				scalar acc = vel.dot(dir);
	//			scalar acc = FP_ONE - vel.dot(dir);

	//			if (acc < 0)
	//				acc = -acc;

	//			printf("Angle: %6d\n", acc);

				if (1)
				{
					acc>>=5;
	//				Vector dir(FPCos((angle + (PI>>1)) % 2*PI), 0, FPSin((angle + (PI>>1))) % 2*PI);
	//				Vector dir(FPSin(angle), 0, -FPCos(angle));

					acceleration -= dir * acc;
	/*
					if (vel.cross(dir).y > 0)
						acceleration += dir * acc;
					else
						acceleration += dir * -acc;
	*/
	//				printf("Turn: %6d\n", acc);
				}
	//				printf("No Turn: %6d\n", acc);

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
		angle += steeringWheelPos * getAngleAcceleration(speed);

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
		
//	DAMPEN(speed, brake?32:4);
	DAMPEN(velocity.x, brake?32:4);
	DAMPEN(velocity.z, brake?32:4);
	DAMPEN(angleSpeed, 1);
}

scalar Car::getAcceleration(scalar speed)
{
	int i = 0;
	
	while(speed > accProfile[i].threshold && (i < sizeof(accProfile)/sizeof(accProfile[0])))
		i++;
		
//	printf("%6d -> acceleration segment %d: %d\n", speed, i, accProfile[i].acc);
		
	return accProfile[i].acc;
}

scalar Car::getAngleAcceleration(scalar speed)
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

	int x, y, i;
	scalar s = 0;

	for(x=0; x<world->getScreen()->width; x++)
	{
		for(i=0; i<5; i++)
			s += getAcceleration(s);

		y = s>>8;
		if (y > 0 && y < world->getScreen()->height)
		{
			if (s <= speed)
			{
				while(--y > 0)
					world->getScreen()->setPixel(x, world->getScreen()->height - y, 0xf000);
			}
			else
				world->getScreen()->setPixel(x, world->getScreen()->height - y, -1);
		}
	}
}
