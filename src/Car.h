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

#ifndef CAR_H
#define CAR_H

#include "FixedPointMath.h"
#include "FixedPointVector.h"
#include "Object.h"
#include "World.h"

class Track;

class Car: public Renderable
{
public:
	Car(World *_world, const char *name);
	~Car();
	
	void update(Track *track);
	void setThrust(bool _thrust);
	void setBrake(bool _brake);
	void setSteering(int _steering);
	void render(World *world);

	int getSteering() { return steering; }
	scalar getSpeed() { return speed; }
	
	scalar getAngle() { return angle; }
	Vector &getOrigin() { return origin; }
	
private:
	Vector	origin, velocity;
	scalar	angle, angleSpeed, speed;

	scalar	getAcceleration(scalar speed);
	scalar	getAngleAcceleration(scalar speed);

	typedef struct
	{
		scalar	acc;
		scalar	angleAcc;
		scalar	threshold;
	} AccelerationSegment;
	
	AccelerationSegment	accProfile[4];
	
	bool	thrust, brake;
	int		steering, steeringWheelPos, thrustPos;
	
	Object	*object;
	World	*world;
};

#endif