// File  :
// Author: Alexey Malov
#pragma once
#ifndef _MATRIX_H_
#define _MATRIX_H_

namespace icl
{

// forward declarations
class gvector;
class point;

class matrix
{
public:

    // ctors
    matrix() { identity(); };
    matrix(const matrix&);
    matrix
    (
    const gvector&,
    const gvector&,
    const gvector&
    );

    // assignment
    const matrix& operator = (const matrix&);

    // accessors
    double& operator [](int i);
    double operator [](int i) const;
    double& operator ()(int i, int j);
    double operator ()(int i, int j) const;

    // math operations
	// Return the transpose of the matrix.
	matrix transposition() const;
	// Transpose the matrix.
	const matrix& transpose();

	// return the determinant
	double determinant() const;

    // return the inverse of a matrix
	int inverse(matrix&) const;

	// Multiply two matrices.

	matrix operator * (const matrix&) const;
	const matrix& operator *= (const matrix&);

	// Multiply a matrix by a double
	const matrix& operator *= (double);

    // "Constructors" for particular types of transformation. 
    // These are not actually constructors because there is 
    // insufficient information in their argument types to 
    // identify the different cases.

    const matrix& identity();
    // Form matrix for equal scaling.
    const matrix& scaling(double);
    // Form matrix for differential scaling.
    const matrix& scaling(double, double, double);
    // Form matrix for differential scaling.
    const matrix& scaling(const gvector&);
    // Form matrix for rotation by angle about vector.
    const matrix& rotation(double, const gvector&);
    // Form matrix for reflection about a plane through the origin 
    // with the given vector as normal.
    const matrix& reflection(const gvector&);

    const matrix& coordinate
    (
    const gvector&,
    const gvector&,
    const gvector&
    );

    bool operator == (const matrix&) const;
    bool operator != (const matrix&) const;

    enum
    {
        ndim = 3,
        mdim = ndim,
        dim = ndim,
        nelem = ndim*mdim
    };

protected:
    void zero();

protected:

#ifdef _DEBUG
	struct raw
	{
		double m_c[mdim];
	};
#endif

    union
    {
        double m_v[nelem];
        double m_m[ndim][mdim];
#ifdef _DEBUG
		raw m_g[ndim];
#endif
    };
};

matrix operator * (double, const matrix&);
matrix operator * (const matrix&, double);

gvector operator * (const gvector&, const matrix&);
point operator *(const point&, const matrix&);
}; //namespace icl (icadlib)

#ifdef NDEBUG
#include "matrix.inl"
#endif

#endif
