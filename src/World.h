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

#ifndef WORLD_H
#define WORLD_H

#include "View.h"
#include "FixedPointMatrix.h"

typedef struct
{
	short		*vertex;
	char		vertexCount;
	Game::Pixel	color;
	Game::Surface	*texture;
	Vector		normal;
	class Object	*object;	// required for qsort
} Face;

class Object
{
public:
	Object(int _vertexCount, int _faceCount);
	~Object();
	
	void	render(View *view);
	
	void	beginMesh();
	void	setTexCoord(scalar u, scalar v);
	int	addVertex(Vector &pos);
	void	beginFace(int vertexCount);
	void	setTexture(Game::Surface *t);
	void	setColor(Game::Pixel c);
	void	addFaceVertex(short n);
	void	endFace();
	void	endMesh();
	
	Matrix		transformation;
	
	Vertex		*vertex;
	Vertex		*transformedVertex;
	int		vertexCount;
	Face		*face;
	int		faceCount;
protected:
	static int	sortComparator(const void *_a, const void *_b);

	int		currentFace;
	int		currentVertex;
};

class World
{
public:
	World();
	~World();
	
	void	render(View *view);
};

#endif
