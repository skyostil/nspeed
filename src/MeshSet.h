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
#ifndef MESHSET_H
#define MESHSET_H

#include "Mesh.h"
#include "Renderable.h"

#define MAX_MESHES	32

class MeshSet: public Renderable
{
public:
	MeshSet();
	void	render(World *world);
	int		add(Mesh *o);
	void	remove(Mesh *o);

protected:
	static int	sortComparator(const void *_a, const void *_b);

	typedef struct
	{
		class MeshSet	*self;
		Mesh			*mesh;
	} SortItem;
		
	World	*world;
	SortItem sortList[MAX_MESHES];
	Set<Mesh*> meshes;
};

#endif
