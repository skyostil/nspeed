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
#include "Environment.h"
#include <stdio.h>

Car::Car(World *_world, const char *name):
	angle(0),
	speed(0),
	angleSpeed(0),
	velocity(0,0,0),
	origin(0,0,0),
	thrust(false),
	brake(false),
	steering(0),
	steeringWheelPos(0),
	thrustPos(0),
	world(_world)
{
	object = new CarModel(FPInt(1)>>8, world->getFramework()->loadImage(world->getFramework()->findResource("car.png"), &world->getScreen()->format));
	world->getEnvironment()->objectPool.add(object);

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
	accProfile[3].angleAcc = 100;
	accProfile[3].threshold = 0x7fffffff;
}

Car::~Car()
{
	world->getEnvironment()->objectPool.remove(object);
	delete object;
}

#define DAMPEN(x,amount) if (x) x += (x>(amount))?(-(amount)):(amount)

void Car::update(Track *track)
{
	Vector acceleration(0,0,0);

	// update the speedometer
	speed = velocity.length();

//	printf("Speed: %d\n", speed);
//	printf("Vel: %6d, %6d\n", velocity.x, velocity.z);
	
	// collision detection & response
	if (track->getCell(origin) == 0)
	{
		Vector normal = track->getNormal(origin);
		Vector vel = velocity;

		vel.normalize();

//		printf("%d\n", speed);

		// if we were going too slow, use the normal as the new velocity
		if (speed < 256)
		{
			velocity = normal * -1000;
//			printf("Crash %6d, %6d\n", normal.x, normal.z);
		}
		
		// backtrack to avoid collision
		while(track->getCell(origin)==0)
			origin -= velocity;
/*		
		// rotate the normal 90 degress
		normal.y = normal.x;
		normal.x = -normal.z;
		normal.z = normal.y;
		normal.y = 0;
*/		
//		scalar p = velocity.dot(normal);
		scalar p = vel.dot(normal);
/*
		// make sure we bounce off the wall
		if (p > -(FP_ONE) && p < 0)
			p = -(FP_ONE);
		else if (p < (FP_ONE) && p > 0)
			p = (FP_ONE);
*/
//		if (p < 0) p = -p;
		
//		velocity = normal * p - velocity * (FPInt(1)>>2);
//		acceleration = normal * p - velocity * (FPInt(1)>>5);
//		acceleration = normal * p - velocity * (FP_ONE + 3*FP_ONE/4);
//		acceleration = normal * p - velocity * (FP_ONE<<1);
//		acceleration = -velocity + (vel - normal * FPMul(FPInt(2), p)) * speed;
//		acceleration = -velocity + (vel - normal * FPMul(FPInt(2), p)) * speed + normal * 1000;

		if (normal.dot(velocity) < 0)
			acceleration = normal * FPMul(normal.dot(velocity), FPInt(-2));
		else
			acceleration = normal * 1024;

		if (acceleration.lengthSquared() < 128)
			acceleration += normal * 2048;
//		printf("%d\n", acceleration.lengthSquared());
//		printf("%d\n", normal.dot(velocity));
		
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
					acc>>=6;
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

	if (thrust && thrustPos < 8)
		thrustPos++;
	else if (!thrust && thrustPos > 0)
		thrustPos--;
		
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

	// update the model position
	Vector verticalAxis(0,FP_ONE,0);
	Vector rollAxis(FPCos(angle),0,FPSin(angle));
	Matrix translation = Matrix::makeTranslation(origin + Vector(0, thrustPos<<(FP-10), 0));

	object->transformation = Matrix::makeRotation(verticalAxis, angle + (steeringWheelPos<<(FP-6)));
	object->transformation *= Matrix::makeRotation(rollAxis, -(steeringWheelPos<<(FP-6)));
	object->transformation *= translation;
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

void Car::render(World *world)
{
/*
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
*/
}
