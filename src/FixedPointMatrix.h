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
 
#ifndef FIXEDPOINTMATRIX_H
#define FIXEDPOINTMATRIX_H
 
#include "FixedPointVector.h"

class Matrix	// 4x4
{
public:
	Matrix(Matrix *b);
	Matrix();

	scalar	cell(int row, int column);
	void	setCell(int i, scalar v);
	Vector	getTranslation();

	Matrix	operator*(Matrix &b);
	Vector	operator*(Vector &v);
	Matrix	operator*(scalar s);
	scalar	operator[](unsigned i);
	Matrix	operator*=(Matrix &b);

	void	setColumn(int column, Vector &v);
	Vector	getColumn(int column);
	void	setRow(int row, Vector &v);
	Vector	getRow(int row);

	Matrix	inverse3x3();
	Matrix	inverse();
	Matrix	transpose3x3();
	Vector	mul3x3(Vector &v);

	Matrix makeIdentity();
	static Matrix makeTranslation(Vector &direction);
	static Matrix makeRotation(Vector &axis, scalar angle);
	static Matrix makeRotation(Vector &origin, Vector &axis, scalar angle);
	static Matrix makeScaling(Vector &scale);
	static Matrix makeLookAt(Vector &zdirection, scalar angle);
protected:
	scalar	m[4*4];
};

#endif
