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
#include "Object.h"
#include <stdlib.h> // for qsort
 
Object::Object(int _vertexCount, int _faceCount, int _flags):
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

Object::~Object()
{
	int fc = faceCount;
	Face *f = &face[0];
	
	while(fc--)
	{
		delete[] (f++)->vertex;
	}

	delete[] vertex;
	delete[] transformedVertex;
	delete[] face;
}

int Object::sortComparator(const void *_a, const void *_b)
{
	Face *a = (Face*)_a;
	Face *b = (Face*)_b;
	
	if (
		a->object->transformedVertex[a->vertex[0]].pos.z >
		a->object->transformedVertex[b->vertex[0]].pos.z)
		return -1;
	return 1;
}

void Object::render(World *world)
{
	int fc, vc;
	Face *f = &face[0];
	Vertex *v = &vertex[0];
	Vertex *vt = &transformedVertex[0];
	
	vc = vertexCount;
	while(vc--)
	{
		*vt = *v;
		vt->pos = transformation * vt->pos;
		vt->pos = world->getView()->camera.transform(vt->pos);
		
		vt++;
		v++;
	}
	
	qsort(face, faceCount, sizeof(Face), sortComparator);

	world->getView()->rasterizer->flags = flags;
	
	fc = faceCount;
	while(fc--)
	{
		Vector normal = transformation.mul3x3(f->normal);
		Vector *eye = &transformedVertex[f->vertex[0]].pos;
		
		normal = world->getView()->camera.transformDirection(normal);
		
		if (normal.dot(*eye) < 0)
		{
			world->getView()->beginPolygon();
			world->getView()->rasterizer->setTexture(f->texture);
			world->getView()->rasterizer->setColor(f->color);
			for(vc=0; vc<f->vertexCount; vc++)
			{
				v = &transformedVertex[f->vertex[vc]];
	//			printf("%d, %d, %d\n", v->pos.x>>FP, v->pos.y>>FP, v->pos.z>>FP);
				world->getView()->addTransformedVertex(*v);
			}
			world->getView()->endPolygon();
		}
		f++;
	}
}

void Object::beginMesh()
{
	currentVertex = 0;
	currentFace = 0;
}

void Object::setTexCoord(scalar u, scalar v)
{
	vertex[currentVertex].u = u;
	vertex[currentVertex].v = v;
}

int Object::addVertex(Vector &pos)
{
	vertex[currentVertex].pos = pos;
	if (currentVertex < vertexCount-1)
		return currentVertex++;
	return currentVertex;
}

void Object::beginFace(int vertexCount)
{
	face[currentFace].object = this;
	face[currentFace].vertex = new short[vertexCount];
	face[currentFace].vertexCount = 0;
	face[currentFace].texture = 0;
	face[currentFace].color = (Game::Pixel)-1;
}

void Object::setTexture(Game::Surface *t)
{
	face[currentFace].texture = t;
}

void Object::setColor(Game::Pixel c)
{
	face[currentFace].color = c;
}

void Object::addFaceVertex(short n)
{
	face[currentFace].vertex[face[currentFace].vertexCount++] = n;
}

void Object::endFace()
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

void Object::setFlags(int _flags)
{
	flags = _flags;
}

void Object::endMesh()
{
	vertexCount = currentVertex+1;
	faceCount = currentFace+1;
}

Vector &Object::getOrigin()
{
	return transformation.getColumn(3);
}

ObjectSet::ObjectSet():
	objects(MAX_OBJECTS, true)
{
}

void ObjectSet::render(class World *_world)
{
	int i;

	world = _world;
	for(i=0; i<objects.getCount(); i++)
	{
		sortList[i].self = this;
		sortList[i].object = objects.getItem(i);
	}

	qsort(sortList, objects.getCount(), sizeof(SortItem), sortComparator);

	for(i=0; i<objects.getCount(); i++)
		sortList[i].object->render(world);
}

int ObjectSet::sortComparator(const void *_a, const void *_b)
{
	SortItem *a = (SortItem*)_a;
	SortItem *b = (SortItem*)_b;
	Vector distA = a->object->getOrigin() - a->self->world->getView()->camera.origin;
	Vector distB = b->object->getOrigin() - a->self->world->getView()->camera.origin;
	
	if (
		distA.lengthSquared() >
		distB.lengthSquared())
		return -1;
	return 1;
}

int ObjectSet::add(Object *o)
{
	return objects.add(o);
}

void ObjectSet::remove(Object *o)
{
	objects.remove(o);
}
