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

#include "engine/Engine.h"
#include "FixedPointMath.h"
#include "FixedPointVector.h"
#include "Mesh.h"
#include "World.h"
#include "Object.h"

#define CAR_COORDINATE_SCALE 4

class Track;

class Car: public Object
{
public:
	Car(World *_world, const char *name);
	~Car();
	
	void update(Track *track);
	void setThrust(bool _thrust);
	void setBrake(bool _brake);
	void setSteering(int _steering);
	void setAiState(bool enabled);
	
	int getSteering() { return steering; }
	scalar getSpeed() { return FPSqrt(speed); }
	int getGateIndex() { return gateIndex; }
	void setGateIndex(int i) { gateIndex = i; }
	int getLapCount() { return lapCount; }
	
	//! in the range [0, 100]
	int getEnergy() { return energy>>FP; }
	
	scalar getAngle() { return angle; }
	Vector getOrigin() { return origin * (FP_ONE>>CAR_COORDINATE_SCALE); }
	void setOrigin(const Vector &o);
	
	void prepareForRace(int position);
	
	scalar	getAcceleration(scalar speed) const;
	scalar	getAngleAcceleration(scalar speed) const;
	
private:
	void	updateSound();
	void	updateGate();
	void	updateAi();
	void	updateCollisions();
	void	updateTileEffects(Track *track);
	void	checkCollision(Car *other);
	void	explode();

	Vector	origin, velocity, acceleration;
	scalar	angle, angleSpeed, speed; // note: the speed is squared!

	typedef struct
	{
		scalar	acc;
		scalar	angleAcc;
		scalar	threshold;
	} AccelerationSegment;
	
	AccelerationSegment	accProfile[8];
	
	bool	thrust, brake;
	int		steering, steeringWheelPos, thrustPos, engineCycle;
	
	Mesh			*mesh;
	Game::Surface	*texture;
	World			*world;
	Game::SampleChunk	*engineSound;
	int				sfxChannel, gateIndex, lapCount, carNumber;
	scalar			energy;
	bool			aiEnabled;
};

#endif
