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
#include "Mesh.h"
#include "TagFile.h"
#include "World.h"
#include "Environment.h"
#include "Config.h"
#include <stdlib.h> // for qsort
 
Mesh::Mesh(Object *parent, int _vertexCount, int _faceCount, int _flags):
	Object(parent),
	vertexCount(_vertexCount),
	faceCount(_faceCount),
	currentFace(0),
	currentVertex(0),
	flags(_flags)
{
	transformation.makeIdentity();
	vertex = new Vertex[vertexCount];
	transformedVertex = new Vertex[vertexCount];
	face = new Face[faceCount];
}

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
typedef struct
{
	scalar x, y, z;
	scalar u, v;
} PACKED SerializedVertex;

typedef struct
{
	signed short a, b, c;
} PACKED SerializedTriangle;
#ifdef _MSC_VER
#pragma pack(pop)
#endif

Mesh::Mesh(Object *parent, const char *fileName, Game::Surface *texture, int _flags):
	Object(parent),
	vertexCount(0),
	faceCount(0),
	currentFace(0),
	currentVertex(0),
	vertex(0),
	transformedVertex(0),
	face(0),
	flags(_flags)
{
	transformation.makeIdentity();
	TagFile file(fileName);
	int i;

	beginMesh();
	
	while(1) switch(file.readTag())
	{
	case 0: // list of vertices
	{
		vertexCount = file.getDataSize() / sizeof(SerializedVertex);
		SerializedVertex *v = new SerializedVertex[vertexCount];
		
		vertex = new Vertex[vertexCount];
		transformedVertex = new Vertex[vertexCount];
		
		file.readData((unsigned char*)v, sizeof(SerializedVertex)*vertexCount);
		
		for(i=0; i<vertexCount; i++)
		{
			setTexCoord(v[i].u, v[i].v);
//			addVertex(Vector(v[i].x >> 1, v[i].y >> 1, v[i].z >> 1));
			addVertex(Vector(v[i].x, v[i].y, v[i].z));
		}
		
		delete[] v;
	}
	break;
	case 1: // list of faces
	{
		faceCount = file.getDataSize() / sizeof(SerializedTriangle);		
		SerializedTriangle *t = new SerializedTriangle[faceCount];
				
		face = new Face[faceCount];
		
		file.readData((unsigned char*)t, sizeof(SerializedTriangle)*faceCount);

		for(i=0; i<faceCount; i++)
		{
			beginFace(3);
			setTexture(texture);
			addFaceVertex(t[i].a);
			addFaceVertex(t[i].b);
			addFaceVertex(t[i].c);
			endFace();
		}
				
		delete[] t;
	}
	break;
	default:
		endMesh();
		return;
	}
}

Mesh::~Mesh()
{
	int fc = faceCount;
	Face *f = &face[0];
	
	if (f)
		while(fc--)
		{
			delete[] (f++)->vertex;
		}

	delete[] vertex;
	delete[] transformedVertex;
	delete[] face;
}

int Mesh::sortComparator(const void *_a, const void *_b)
{
	Face *a = (Face*)_a;
	Face *b = (Face*)_b;
	
	if (
		a->mesh->transformedVertex[a->vertex[0]].pos.z >
		a->mesh->transformedVertex[b->vertex[0]].pos.z)
		return -1;
	return 1;
}

void Mesh::render(World *world)
{
	int fc, vc;
	Face *f = &face[0];
	Vertex *v = &vertex[0];
	Vertex *vt = &transformedVertex[0];
	
	if (!v)
		return;
	
	vc = vertexCount;
	while(vc--)
	{
		*vt = *v;
		vt->pos = transformation * vt->pos;
		vt->pos = world->getEnvironment()->getView()->camera.transform(vt->pos);
		
		vt++;
		v++;
	}
	
	qsort(face, faceCount, sizeof(Face), sortComparator);

	world->getEnvironment()->getView()->rasterizer->flags = flags;
	
	fc = faceCount;
	while(fc--)
	{
		Vector normal = transformation.mul3x3(f->normal);
		Vector *eye = &transformedVertex[f->vertex[0]].pos;
		
		normal = world->getEnvironment()->getView()->camera.transformDirection(normal);
		
		if (normal.dot(*eye) < 0)
		{
			world->getEnvironment()->getView()->beginPolygon();
			world->getEnvironment()->getView()->rasterizer->setTexture(f->texture);
			world->getEnvironment()->getView()->rasterizer->setColor(f->color);
			for(vc=0; vc<f->vertexCount; vc++)
			{
				v = &transformedVertex[f->vertex[vc]];
	//			printf("%d, %d, %d\n", v->pos.x>>FP, v->pos.y>>FP, v->pos.z>>FP);
				world->getEnvironment()->getView()->addTransformedVertex(*v);
			}
			world->getEnvironment()->getView()->endPolygon();
		}
		f++;
	}
}

void Mesh::beginMesh()
{
	currentVertex = 0;
	currentFace = 0;
}

void Mesh::setTexCoord(scalar u, scalar v)
{
	vertex[currentVertex].u = u;
	vertex[currentVertex].v = v;
}

int Mesh::addVertex(const Vector &pos)
{
	vertex[currentVertex].pos = pos;
	if (currentVertex < vertexCount-1)
		return currentVertex++;
	return currentVertex;
}

void Mesh::beginFace(int vertexCount)
{
	face[currentFace].mesh = this;
	face[currentFace].vertex = new short[vertexCount];
	face[currentFace].vertexCount = 0;
	face[currentFace].texture = 0;
	face[currentFace].color = (Game::Pixel)-1;
}

void Mesh::setTexture(Game::Surface *t)
{
	face[currentFace].texture = t;
}

void Mesh::setColor(Game::Pixel c)
{
	face[currentFace].color = c;
}

void Mesh::addFaceVertex(short n)
{
	face[currentFace].vertex[face[currentFace].vertexCount++] = n;
}

void Mesh::endFace()
{
	if (face[currentFace].vertexCount>=3)
	{
		Vertex *v0 = &vertex[face[currentFace].vertex[0]];
		Vertex *v1 = &vertex[face[currentFace].vertex[1]];
		Vertex *v2 = &vertex[face[currentFace].vertex[2]];
		face[currentFace].normal = ((v1->pos - v0->pos).cross(v2->pos - v0->pos)).normalize();
	}

	if (currentFace < faceCount-1)
		currentFace++;
}

void Mesh::setFlags(int _flags)
{
	flags = _flags;
}

void Mesh::endMesh()
{
	vertexCount = currentVertex+1;
	faceCount = currentFace+1;
}

Vector Mesh::getOrigin()
{
	return transformation.getColumn(3);
}
