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
#include <stdio.h>

Object::Object(Object *_parent):
	parent(_parent)
{
	if (parent)
		parent->addChild(this);
}

Object::~Object()
{
	Set<Object*>::Iterator i;
	Set<Object*> copyChildren(children);
	
	if (parent)
		parent->removeChild(this);

//	printf("%p: Deleting %d children.\n", this, children.getCount());
			
	for(i=copyChildren.begin(); i!=copyChildren.end(); i++)
	{
//		printf("%p: Delete %p\n", this, *i);
		delete *i;
	}
//	printf("%p: Delete done.\n", this);
	children.clear();
}

void Object::addChild(Object *c)
{
//	printf("%p: Add child %p\n", this, c);
	children.add(c);
}

void Object::removeChild(Object *c)
{
//	printf("%p: Remove child %p\n", this, c);
	children.remove(c);
}

