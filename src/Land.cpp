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

#include "Land.h"
#include <stdio.h>
 
Land::Land(Game::Surface *_texture,
           int _flags,
	   int _textureScale,
           scalar _depth):
	texture(_texture),
	textureScale(_textureScale),
	invDepth(FPDiv(FP_ONE, _depth)),
	flags(_flags)
{
}

void Land::render(View *view)
{
#if 0  // we used to draw a textured quad
	Vector center = view->camera.origin;
	Vector v0, v1, v2, v3;
	
	v0.set(-scale + center.x, depth, -scale + center.z);
	v1.set( scale + center.x, depth, -scale + center.z);
	v2.set( scale + center.x, depth,  scale + center.z);
	v3.set(-scale + center.x, depth,  scale + center.z);
	
	center *= textureScale;
	center+=Vector(FPInt(64),0,FPInt(64));

	view->rasterizer->flags |= flags;
	view->rasterizer->setTexture(texture);

	view->beginPolygon();
	view->setTexCoord(center.x-textureWindowSize, center.z-textureWindowSize);
	view->addVertex(v0);
	view->setTexCoord(center.x+textureWindowSize, center.z-textureWindowSize);
	view->addVertex(v1);
	view->setTexCoord(center.x+textureWindowSize, center.z+textureWindowSize);
	view->addVertex(v2);
	view->setTexCoord(center.x-textureWindowSize, center.z+textureWindowSize);
	view->addVertex(v3);
	view->endPolygon();
	
	view->rasterizer->flags &= ~flags;
#endif

	scalar w(FPInt(view->rasterizer->screen->width-1)), h(FPInt(view->rasterizer->screen->height-1));
	
	// find out where the horizon is
	Vertex center;
	view->camera.unproject(w>>1, h>>1, FP_ONE, &center);
	center.pos = view->camera.inverseTransformDirection(center.pos);
	center.pos.y = 0;
	scalar horizon, dummy0, dummy1, dummy2, dummy3;
	center.pos = view->camera.transformDirection(center.pos);
	view->camera.project(&center, &dummy0, &horizon, &dummy1, &dummy2, &dummy3);

	// add magic number to ensure we don't see too far
	horizon+=13*FP_ONE;
	
	// clip the horizon
	if (horizon < 0)
		horizon = 0;
	else if (horizon > h)
		horizon = h;
		
	Vertex v0, v1, v2, v3;
	view->camera.unproject(0, horizon, FP_ONE, &v0);
	view->camera.unproject(w, horizon, FP_ONE, &v1);
	view->camera.unproject(w, h,       FP_ONE, &v2);
	view->camera.unproject(0, h,       FP_ONE, &v3);
	
	v0.pos = view->camera.inverseTransformDirection(v0.pos);
	v1.pos = view->camera.inverseTransformDirection(v1.pos);
	v2.pos = view->camera.inverseTransformDirection(v2.pos);
	v3.pos = view->camera.inverseTransformDirection(v3.pos);
	
	Vector dir0, dir1, dir2, dir3;
	dir0 = (v0.pos).normalize();
	dir1 = (v1.pos).normalize();
	dir2 = (v2.pos).normalize();
	dir3 = (v3.pos).normalize();
	
	// calculate plane intersection
	Plane plane;
	
	plane.dist = 0;
	plane.normal.set(0,FP_ONE,0);
	
	scalar z0 = plane.intersectRay(view->camera.origin, dir0);
	scalar z1 = plane.intersectRay(view->camera.origin, dir1);
	scalar z2 = plane.intersectRay(view->camera.origin, dir2);
	scalar z3 = plane.intersectRay(view->camera.origin, dir3);

	// calculate intersection in world-space
	v0.pos = (view->camera.origin * invDepth) + (dir0 * z0);
	v1.pos = (view->camera.origin * invDepth) + (dir1 * z1);
	v2.pos = (view->camera.origin * invDepth) + (dir2 * z2);
	v3.pos = (view->camera.origin * invDepth) + (dir3 * z3);
	
	scalar invz0 = (1<<31) / (z0>>INVZ_SCALE);
	scalar invz1 = (1<<31) / (z1>>INVZ_SCALE);
	scalar invz2 = (1<<31) / (z2>>INVZ_SCALE);
	scalar invz3 = (1<<31) / (z3>>INVZ_SCALE);

	// prepare texture coordinates
	v0.pos *= invz0;
	v1.pos *= invz1;
	v2.pos *= invz2;
	v3.pos *= invz3;	
		
	view->rasterizer->flags |= flags;
	view->rasterizer->setTexture(texture);
	
	view->rasterizer->beginPolygon();
	view->rasterizer->setInvZ(invz0);
	view->rasterizer->setTexCoord(v0.pos.x >> textureScale, v0.pos.z >> textureScale);
	view->rasterizer->addVertex(0, horizon);
	view->rasterizer->setInvZ(invz1);
	view->rasterizer->setTexCoord(v1.pos.x >> textureScale, v1.pos.z >> textureScale);
	view->rasterizer->addVertex(w, horizon);
	view->rasterizer->setInvZ(invz2);
	view->rasterizer->setTexCoord(v2.pos.x >> textureScale, v2.pos.z >> textureScale);
	view->rasterizer->addVertex(w, h);
	view->rasterizer->setInvZ(invz3);
	view->rasterizer->setTexCoord(v3.pos.x >> textureScale, v3.pos.z >> textureScale);
	view->rasterizer->addVertex(0, h);
	view->rasterizer->endPolygon();
	
	view->rasterizer->flags &= ~flags;
}
