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

	scalar	cell(int row, int column) const;
	void	setCell(int i, scalar v);
	Vector	getTranslation() const;

	Matrix	operator*(const Matrix &b);
	Vector	operator*(const Vector &v);
	Matrix	operator*(scalar s);
	scalar	operator[](unsigned i) const;
	Matrix	operator*=(const Matrix &b);

	void	setColumn(int column, const Vector &v);
	Vector	getColumn(int column) const;
	void	setRow(int row, const Vector &v);
	Vector	getRow(int row) const;

	Matrix	inverse3x3();
	Matrix	inverse();
	Matrix	transpose3x3();
	Vector	mul3x3(const Vector &v);

	Matrix makeIdentity();
	static Matrix makeTranslation(const Vector &direction);
	static Matrix makeRotation(const Vector &axis, scalar angle);
	static Matrix makeRotation(const Vector &origin, const Vector &axis, scalar angle);
	static Matrix makeScaling(const Vector &scale);
	static Matrix makeLookAt(const Vector &zdirection, scalar angle);
protected:
	scalar	m[4*4];
};

#endif
