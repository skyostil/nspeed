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

#include "View.h"
#include <stdio.h>
#include <string.h>

View::View(Rasterizer *_rasterizer):
	camera(_rasterizer),
	rasterizer(_rasterizer)
{
}

View::~View()
{
	delete rasterizer;
}

void View::beginPolygon()
{
	vertexCount = 0;
}

void View::setTexCoord(scalar u, scalar v)
{
	vertex[vertexCount].u = u;
	vertex[vertexCount].v = v;
}

void View::addVertex(Vector &pos)
{
	Vector p = camera.transform(pos);
	vertex[vertexCount].pos = p;
	vertexCount++;
}

void View::addTransformedVertex(Vertex &v)
{
	vertex[vertexCount] = v;
	vertexCount++;
}

void View::endPolygon()
{
	int i;
	Vertex clippedVertex[MAX_VERTICES];
	ScreenVertex sv;
	
	vertexCount = camera.clipPolygon(&vertex[0], vertexCount, &clippedVertex[0]);
	
//	printf("Clipped polygon has %d vertices.\n", vertexCount);
	
	rasterizer->beginPolygon();
	for(i=0; i<vertexCount; i++)
	{
		camera.project(&clippedVertex[i], &sv.sx, &sv.sy, &sv.uz, &sv.vz, &sv.invz);
		rasterizer->addVertex(&sv);
	}
	rasterizer->endPolygon();
}


Camera::Camera(Rasterizer *_rasterizer):
	rasterizer(_rasterizer),
	clipStackCount(0),
	origin(FPInt(1),FPInt(0),FPInt(1)),
	target(FPInt(0),FPInt(0),FPInt(0)),
	fov(FPInt(60))
{
	aspectRatio = FPDiv(FPInt(rasterizer->screen->height), FPInt(rasterizer->screen->width));
//	aspectRatio = FPDiv(FPInt(3), FPInt(4));
//	aspectRatio = FPDiv(FPInt(screen->width), FPInt(screen->height));
	configure();
	update();
}

Camera::~Camera()
{
}

void Camera::configure()
{
	scalar halfFOV = FPMul(FPDiv(fov,FPInt(2)), (PI/180));

	scalar s = FPSin(halfFOV - (1<<FP)/128);
	scalar c = FPCos(halfFOV - (1<<FP)/128);
	
	// left
	clipStack[0].normal.x = c;
	clipStack[0].normal.y = 0;
	clipStack[0].normal.z = s;
	clipStack[0].dist = FPInt(0);

	// right
	clipStack[1].normal.x = -c;
	clipStack[1].normal.y = 0;
	clipStack[1].normal.z = s;
	clipStack[1].dist = FPInt(0);
	
	// top
	clipStack[2].normal.x = 0;
	clipStack[2].normal.y = c;
	clipStack[2].normal.z = s;
	clipStack[2].dist = FPInt(0);
	
	// bottom
	clipStack[3].normal.x = 0;
	clipStack[3].normal.y = -c;
	clipStack[3].normal.z = s;
	clipStack[3].dist = FPInt(0);
	
	// near
	clipStack[4].normal.x = 0;
	clipStack[4].normal.y = 0;
	clipStack[4].normal.z = FPInt(1);
	clipStack[4].dist = FPInt(1)>>2;
	
	clipStackCount = 5;
	
	perspective = FPMul(FPDiv(FPCos(halfFOV),FPSin(halfFOV)), FPInt(rasterizer->screen->width/2));
}

void Camera::update()
{
	Vector dir(target - origin);
	Vector o(-origin);
	Matrix tr(Matrix::makeTranslation(o));
	
	rotation = Matrix::makeLookAt(dir, 0);
	rotation = rotation.transpose3x3();
	
	transformation = tr * rotation;
}

int Camera::clipPolygon(Vertex *in, int inCount, Vertex *out)
{
	int i;
	Vertex *vIn = in, *vOut = out;
	
	if (!clipStackCount)
	{
		memcpy(out, in, sizeof(Vertex)*inCount);
		return inCount;
	}
	
	for(i=0; (i<clipStackCount) && (inCount>0); i++)
	{
		if (i&1)
		{
			vIn = out;
			vOut = in;
		} else
		{
			vIn = in;
			vOut = out;
		}
		inCount = clipToPlane(&clipStack[i], vIn, inCount, vOut);
	}
	
	if (vOut != out)
	{
		memcpy(out, vOut, sizeof(Vertex)*inCount);
	}
	
	return inCount;
}

void Camera::project(Vertex *v, scalar *sx, scalar *sy, scalar *uz, scalar *vz, scalar *invz)
{
	*invz = FPDiv(FPInt(1), v->pos.z);
	
	if (rasterizer->flags & Rasterizer::FlagPerspectiveCorrection)
	{
		*uz = FPMul(v->u, *invz);
		*vz = FPMul(v->v, *invz);
	} else
	{
		*uz = v->u;
		*vz = v->v;
	}
	
	scalar p = FPMul(perspective, *invz);
	
	*sx = FPInt(rasterizer->screen->width>>1) + FPMul(v->pos.x, p);
	*sy = FPInt(rasterizer->screen->height>>1) + FPMul(v->pos.y, FPMul(p,aspectRatio));
	
	*invz = (1<<31) / (v->pos.z>>INVZ_SCALE);
	
//	printf("%d\n", *invz);

//	printf("screen: %d, %d\n", (*sx)>>FP, (*sy)>>FP);
}

int Camera::clipToPlane(ClippingPlane *plane, Vertex *in, int inCount, Vertex *out)
{
	Vertex *v1 = in, *v2;
	Vertex *first = in;
	int n = inCount;
	int outCount = 0;

#if 0	// disable clipper
	memcpy(out, in, sizeof(Vertex)*inCount);
	return inCount;
#endif	
	
	while(n-- && outCount<MAX_VERTICES)
	{
		if (n)
			v2 = ++in;
		else
			v2 = first;
			
		scalar d1 = plane->pointDistance(v1->pos);
		scalar d2 = plane->pointDistance(v2->pos);
		
//		printf("%d, %d\n", d1>>FP, d2>>FP);
		if (d1>0 && d2>0)
		{
			*out++ = *v1;
			outCount++;
		}
		else if (d1<=0 && d2<=0)
		{
		}
		else
		{
			scalar d = d1 - d2;
			if (d < FP_MIN_DIVISOR && d > -FP_MIN_DIVISOR)
			{
				continue;
			}
		
			// XXX - custom division here!
//			scalar t = FPDiv(d1, d);
			scalar t = ((d1<<10) / (d>>4))<<2;
			
			if (d1>0)
			{
				*out++ = *v1;
				outCount++;
			}
			
			out->pos = v1->pos + (v2->pos - v1->pos)*t;
			out->u = v1->u + FPMul(v2->u - v1->u, t);
			out->v = v1->v + FPMul(v2->v - v1->v, t);
			out++;
			outCount++;
		
			if (d2>0)
			{
				*out++ = *v2;
				outCount++;
			}
		}
		v1 = v2;
	}
	
//	printf("Clip to plane: out %d vertices.\n", outCount);
	return outCount;
}

scalar Camera::ClippingPlane::pointDistance(Vector &point)
{
	return point.dot(normal) - dist;
}

Vector Camera::transform(Vector &v)
{
	return transformation * v;
}

Vector Camera::transformDirection(Vector &v)
{
	return rotation * v;
}
