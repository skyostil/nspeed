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
 
Land::Land(Game::Surface *_texture,
           int _flags,
           scalar _scale,
           scalar _depth,
           scalar _textureScale,
           scalar _textureWindowSize):
	texture(_texture),
	scale(_scale),
	depth(_depth),
	textureScale(_textureScale),
	flags(_flags),
	textureWindowSize(_textureWindowSize)
{
}

void Land::render(View *view)
{
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
}
