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
#include "Environment.h"
#include <math.h>
#include <stdio.h>
 
Land::Land(Game::Surface *_texture,
		   Game::Surface *_skyTexture,
           int _flags,
		   int _textureScale,
           scalar _depth):
	texture(_texture),
	textureScale(_textureScale),
	invDepth(FPDiv(FP_ONE, _depth)),
	flags(_flags),
	skyTexture(_skyTexture)
{
}

void Land::render(World *world)
{
	View *view = world->getEnvironment()->getView();
	
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
	scalar z1 = z0; 	// exploit the planar quality of the plane :)
//	scalar z1 = plane.intersectRay(view->camera.origin, dir1);
	scalar z2 = plane.intersectRay(view->camera.origin, dir2);
//	scalar z3 = plane.intersectRay(view->camera.origin, dir3);
	scalar z3 = z2;

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

	// draw the sky rectangle
	if (skyTexture)
	{
		Vector v = view->camera.target - view->camera.origin;
		scalar angle;
		scalar skyWidth = FPDiv(FPInt(256),PI);
		scalar skyHeight = FPInt(64);

		if (FPAbs(v.z) > FPAbs(v.x))
		{
			angle = FPArcTan2(v.z, v.x);
		}
		else
		{
			angle = -FPArcTan2(v.x, v.z) + PI/2;
//			angle = -FPArcTan2(v.x, v.z);
//			vOffset = FPInt(64);
		}

		scalar x = FPMul(angle, -skyWidth) + skyWidth*4;

		scalar u0 = x;
		scalar v0 = skyHeight - (horizon>>1);
		scalar u1 = x + skyWidth;
		scalar v1 = skyHeight - (horizon>>1);
		scalar u2 = x + skyWidth;
		scalar v2 = skyHeight;
		scalar u3 = x;
		scalar v3 = skyHeight;

		view->rasterizer->flags = 0;
		view->rasterizer->setTexture(skyTexture);

		view->rasterizer->beginPolygon();
		view->rasterizer->setInvZ(1);
		view->rasterizer->setTexCoord(u0, v0);
		view->rasterizer->addVertex(0, 0);
		view->rasterizer->setInvZ(1);
		view->rasterizer->setTexCoord(u1, v1);
		view->rasterizer->addVertex(w, 0);
		view->rasterizer->setInvZ(1);
		view->rasterizer->setTexCoord(u2, v2);
		view->rasterizer->addVertex(w, horizon);
		view->rasterizer->setInvZ(1);
		view->rasterizer->setTexCoord(u3, v3);
		view->rasterizer->addVertex(0, horizon);
		view->rasterizer->endPolygon();
	}

	view->rasterizer->flags |= flags;
	view->rasterizer->setTexture(texture);
	
	// draw the ground rectangle
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

	// draw the horizon gradient
	if (!skyTexture)
	{
		unsigned int x;
		Game::Pixel16 *p = (Game::Pixel16*)view->rasterizer->screen->pixels;
		Game::Pixel16 mask = 
			(view->rasterizer->screen->format.rmask & (view->rasterizer->screen->format.rmask>>1)) +
			(view->rasterizer->screen->format.gmask & (view->rasterizer->screen->format.gmask>>1)) +
			(view->rasterizer->screen->format.bmask & (view->rasterizer->screen->format.bmask>>1));
		const int scale = 2;
		int c = 16 << scale;

		p+=(horizon>>FP) * view->rasterizer->screen->width;

		if (c > 0)
		{
			int maxr = ((1<<view->rasterizer->screen->format.rsize)-1) <<view->rasterizer->screen->format.rshift;
			int maxg = ((1<<view->rasterizer->screen->format.gsize)-1) <<view->rasterizer->screen->format.gshift;
			int maxb = ((1<<view->rasterizer->screen->format.bsize)-1) <<view->rasterizer->screen->format.bshift;
			while(c--)
			{
				int gradr = (c>>scale) << view->rasterizer->screen->format.rshift;
				int gradg = (c>>scale) << view->rasterizer->screen->format.gshift;
				int gradb = (c>>scale) << view->rasterizer->screen->format.bshift;

				for(x=0; x<view->rasterizer->screen->width; x++)
				{
					int r = (*p) & view->rasterizer->screen->format.rmask;
					int g = (*p) & view->rasterizer->screen->format.gmask;
					int b = (*p) & view->rasterizer->screen->format.bmask;

					r += gradr;
					g += gradg;
					b += gradb;

					if (r > maxr) r = maxr;
					if (g > maxg) g = maxg;
					if (b > maxb) b = maxb;

					*p++ = (r|g|b);
				}
			}
		}
		else
		{
			int minr = (1<<view->rasterizer->screen->format.rshift);
			int ming = (1<<view->rasterizer->screen->format.gshift);
			int minb = (1<<view->rasterizer->screen->format.bshift);
			while(c++)
			{
				int gradr = (c>>scale) << view->rasterizer->screen->format.rshift;
				int gradg = (c>>scale) << view->rasterizer->screen->format.gshift;
				int gradb = (c>>scale) << view->rasterizer->screen->format.bshift;

				for(x=0; x<view->rasterizer->screen->width; x++)
				{
					int r = (*p) & view->rasterizer->screen->format.rmask;
					int g = (*p) & view->rasterizer->screen->format.gmask;
					int b = (*p) & view->rasterizer->screen->format.bmask;

					r += gradr;
					g += gradg;
					b += gradb;

					if (r < minr) r = minr;
					if (g < ming) g = ming;
					if (b < minb) b = minb;

					*p++ = (r|g|b);
				}
			}
		}
	}
}
