/***************************************************************************
 *   Copyright (C) 2004 by Sami Ky�stil�                                   *
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

#ifndef SET_H
#define SET_H

// gee, thanks for not supporting stl, symbian...

template<typename T>
class Set
{
public:
	class Iterator
	{
	public:
		Iterator(): set(0), index(0)
		{
		}

		Iterator(const Set<T> *_set):	set(_set), index(0)
		{
		}
				
		T operator*() const
		{
			return set->getItem(index);
		}

		void operator++()
		{
			index++;
		}

		void operator++(int)
		{
			index++;
		}
		
		void operator--()
		{
			index--;
		}
				
		bool operator==(int _index) const
		{
			return index == _index;
		}

		bool operator!=(int _index) const
		{
			return index != _index;
		}
				
		void operator=(int _index) const
		{
			index = _index;
		}
				
	private:
		int index;
		const Set<T> *set;
	};

	Set(int _size = 0, bool _autoDelete = false):
	  size(_size),
	  autoDelete(_autoDelete),
	  count(0),
	  item(0)
	{
		grow(size);
	}

	Set(Set<T> &s):
	  size(0),
	  autoDelete(0),
	  count(0),
	  item(0)
	{
		grow(s.count);

		int i;
		for(i=0; i<s.count; i++)
			item[i] = s.item[i];
		count = s.count;
	}
	  
	void grow(int items)
	{
		int i;
		
		if (items < 1)
			items = 1;
	
		if (items < size)
			return;
	
		T *newItem = new T[items];
		
		if (item)
		{
			for(i=0; i<size; i++)
				newItem[i] = item[i];
			for(; i<items; i++)
				newItem[i] = 0;
			delete[] item;
		} else
		{
			for(i=0; i<items; i++)
				newItem[i] = 0;
		}
		size = items;
		item = newItem;
	}

	virtual ~Set()
	{
		if (autoDelete)
		{
			int i;
			for(i=0; i<count; i++)
				delete item[i];
		}
		delete[] item;
	}
	
	inline const Iterator begin() const
	{
		return Iterator(this);
	}
	
	inline int end() const
	{
		return count;
	}
	
	int add(T t)
	{
		if (count < size-1)
		{
			item[count] = t;
			return count++;
		} else
		{
			grow(size * 2);
			item[count] = t;
			return count++;
		}
		return -1;
	}

	void clear()
	{
		if (autoDelete)
			deleteItems();
		count = 0;
	}
	
	void deleteItems()
	{
		int i;
		for(i=0; i<count; i++)
			delete item[i];
		count = 0;
	}

	void remove(T t)
	{
		int i;

		for(i=0; i<count; i++)
			if (item[i] == t)
			{
				count--;
				for(; i<count; i++)
					item[i] = item[i+1];
				return;
			}
	}

	inline T getItem(int i) const
	{
#if 0
		if (i>=0 && i<size-1)
			return item[i];
		return 0;
#else // unsafe and proud of it
		return item[i];
#endif
	}

	inline int getCount() const
	{
		return count;
	}
	
	void setAutoDelete(bool a)
	{
		autoDelete = a;
	}

private:
	bool autoDelete;
	int count, size;
	T *item;
};

#endif
