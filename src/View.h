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

#ifndef VIEW_H
#define VIEW_H

#include "engine/Engine.h"
#include "Rasterizer.h"
#include "FixedPointMath.h"
#include "FixedPointVector.h"
#include "FixedPointMatrix.h"

#define MAX_VERTICES		64
#define MAX_CLIPPING_PLANES	5

// how much to shrink the frustum to avoid drawing outside the screen
#define VIEW_FRUSTUM_TWEAK	1

// improves clipping accuracy
#define VIEW_64BIT_CLIPPING


/*
class Object
{
public:
	Object(): origin(0,0,0)
	{
	}
	
	Vector origin;
};
*/
	
typedef struct
{
	Vector pos;
	scalar u, v;
	
} Vertex;

class Plane
{
public:
	//! \returns the shortest distance from the plane to the given point.
	scalar	pointDistance(Vector &point);
	
	//! ray is in object-space
	scalar intersectRay(Vector &origin, Vector &direction);

	Vector	normal;
	scalar	dist;
};

class Camera
{
public:
	Camera(Rasterizer *_rasterizer);
	~Camera();
	
	//! Configures the view frustum.
	void configure();
	
	//! Updates the transformation matrix.
	void update();
	
	//! Clips given polygon. \returns outCount.
	int clipPolygon(Vertex *in, int inCount, Vertex *out);

	//! Projects the given camera-space coordinates (v) to screen-space (sx, sy, uz, vz, invz).
	void project(Vertex *v, scalar *sx, scalar *sy, scalar *uz, scalar *vz, scalar *invz);

	//! Projects the given screen-space (sx, sy, z) coordinates to camera-space (v).
	void unproject(scalar sx, scalar sy, scalar z, Vertex *v);
		
	//! Transforms the given world-space vector into camera-space.
	Vector	transform(Vector &v);

	//! Transforms the given world-space direction vector into camera-space.
	Vector	transformDirection(Vector &v);

	//! Transforms the given camera-space vector into world-space.
	Vector	inverseTransform(Vector &v);

	//! Transforms the given camera-space direction vector into world-space.
	Vector	inverseTransformDirection(Vector &v);
				
	Vector	target;
	Vector	origin;

protected:
	class ClippingPlane: public Plane
	{
	};

	//! \returns outCount.
	int clipToPlane(ClippingPlane *plane, Vertex *in, int inCount, Vertex *out);
	
	//! A list of clipping plane normals
	ClippingPlane	clipStack[MAX_CLIPPING_PLANES];
	int		clipStackCount;
	
	scalar		aspectRatio, fov;
	scalar		perspective;
	Matrix		transformation, rotation;
	Matrix		invTransformation, invRotation;

	Rasterizer	*rasterizer;
};

class View
{
public:
	View(Rasterizer *_rasterizer);
	~View();
	
	Camera		camera;
	
	// the polygon MUST be convex. x, y and z are in world-space.
	void beginPolygon();
	void setTexCoord(scalar u, scalar v);
	void addVertex(Vector &pos);
	//! \param v must be in camera-space.
	void addTransformedVertex(Vertex &v);
	void endPolygon();
	
	Rasterizer	*rasterizer;
protected:
	Vertex		vertex[MAX_VERTICES];
	int		vertexCount;
};

#endif
