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

#include "MeshSet.h"
#include "Mesh.h"
#include "World.h"
#include "Environment.h"
#include <stdlib.h>
 
MeshSet::MeshSet():
        meshes(MAX_MESHES, false)
{
}

void MeshSet::render(class World *_world)
{
        int i;
        int count = 0;
        const scalar viewLimit = FPInt(4);

        world = _world;
        for(i=0; i<meshes.getCount(); i++)
        {
            Mesh *m = meshes.getItem(i);

            // don't render meshes that are too far
            if ((m->getOrigin() - world->getEnvironment()->getView()->camera.origin).manhattanNorm() < viewLimit)
            {
                sortList[count].self = this;
                sortList[count].mesh = m;
                count++;
            }
        }

        qsort(sortList, count, sizeof(SortItem), sortComparator);

        for(i=0; i<count; i++)
                sortList[i].mesh->render(world);
}

int MeshSet::sortComparator(const void *_a, const void *_b)
{
        SortItem *a = (SortItem*)_a;
        SortItem *b = (SortItem*)_b;
        Vector distA = a->mesh->getOrigin() - a->self->world->getEnvironment()->getView()->camera.origin;
        Vector distB = b->mesh->getOrigin() - a->self->world->getEnvironment()->getView()->camera.origin;
        
        if (
                distA.lengthSquared() >
                distB.lengthSquared())
                return -1;
        return 1;
}

int MeshSet::add(Mesh *o)
{
        return meshes.add(o);
}

void MeshSet::remove(Mesh *o)
{
        meshes.remove(o);
}
