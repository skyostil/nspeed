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
#include <stdio.h>

Car::Car():
	angle(0),
	acceleration(0),
	angleAcceleration(0),
	speed(0),
	angleSpeed(0),
	velocity(0,0,0),
	origin(0,0,0)
{
}

#define DAMPEN(x,amount) if (x) x += (x>0)?(-amount):amount

void Car::update(scalar t)
{
	velocity.x = FPMul(FPCos(angle), speed);
	velocity.z = FPMul(FPSin(angle), speed);
	origin += velocity;
	angle += angleSpeed;
	
	while(angle > 2*PI)
		angle -= 2*PI;

	while(angle < 0)
		angle += 2*PI;
		
	speed += acceleration;
	angleSpeed += angleAcceleration;
			
	DAMPEN(speed, 1);
	DAMPEN(angleSpeed, 1);
}


