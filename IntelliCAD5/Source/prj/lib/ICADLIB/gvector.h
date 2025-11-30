// File  :
// Author: Alexey Malov
#pragma once
#ifndef _GVECTOR_H_
#define _GVECTOR_H_

#include "values.h"

namespace icl
{
//forward declarations
class matrix;
class transf;

class gvector
{
    //NOTE: don't add data to the structure or virtual functions
    //vector' coordinates
    double m_coord[3];

public:
    // ctors
    gvector();
    gvector(const gvector&);
    gvector(const double[3]);
    gvector(double, double, double);

    // assignment
    const gvector& operator = (const gvector&);
    const gvector& operator = (double[3]);
    const gvector& set(double, double, double);

    // accessors
    double operator [] (int) const;
    double& operator [] (int);
    double x() const;
    double& x();
    double y() const;
    double& y();
    double z() const;
    double& z();

    // casting to array
    operator double* ();
    operator const double* () const;

    // calculates length
    double norm() const;
    // calculates squared length
    double norm2() const;

	//return normalized vector
	gvector normalized() const;
	//normalize this vector
	const gvector& normalize();

    // multiplication with assign by double
    const gvector& operator *= (double);

    // dividing by double (without any check of divider)
    gvector operator / (double) const;
    //dividing with assign (without any check of divider)
    const gvector& operator /= (double);

    // cross-product
    gvector operator * (const gvector&) const;
    // cross-product withh assign
    const gvector& operator *= (const gvector&);

    // dot-product
    double operator % (const gvector&) const;

    // subtraction
    gvector operator - (const gvector&) const;
    // unary minus
    gvector operator - () const;
    // subtraction with assign
    const gvector& operator -= (const gvector&);

    // addition
    gvector operator + (const gvector&) const;
    // addition with assign
    const gvector& operator += (const gvector&);

    // transformation by transf, with assign
    const gvector& operator *= (const transf&);
    // transformation by matrix(3x3), with assign
    const gvector& operator *= (const matrix&);

    // comparision of absolute values of vector components
    bool operator < (double eps) const;
	bool operator > (double eps) const;

    // comparing for equality
    bool    operator == (const gvector&) const;
    // comparing for inequality
    bool operator != (const gvector&) const;
};

// multiplication by double
gvector operator * (const gvector&, double);
gvector operator * (double, const gvector&);

// transformation by transf
gvector operator * (const gvector&, const transf&);
// transformation by matrix(3x3)
gvector operator * (const gvector&, const matrix&);

class point
{
    //NOTE: don't add data to the structure or virtual functions
    // point's coordinate
    double m_coord[3];

public:
    // ctors
    point();
    point(const point&);
    point(const double[3]);
    point(double, double, double);

    // assignments
    const point& operator = (const point&);
    const point& operator = (double[3]);
    const point& set(double, double, double);

    // accessors
    double operator [] (int) const;
    double& operator [] (int);
    double x() const;
    double& x();
    double y() const;
    double& y();
    double z() const;
    double& z();

    // casting to array
    operator double* ();
    operator const double* () const;

    // comparing
    bool operator == (const point&) const;
    bool operator != (const point&) const;

    // addition of vector with assignment
    const point& operator += (const gvector&);
    // subtraction of vector with assignment
    const point& operator -= (const gvector&);

    // point from point subtraction
    gvector operator - (const point&) const;
    // vector from point subtraction
    point operator - (const gvector&) const;

    // transformation by transf, with assign
    const point& operator *= (const transf&);
    // transformation by matrix(3x3), with assign
    const point& operator *= (const matrix&);
};

// commutative addition of vector and point
point operator + (const point&, const gvector&);
point operator + (const gvector&, const point&);

// transformation by transf
// multiplication implemented only for point*transf case;
// we consider, transf*point is meaningless
point operator * (const point&, const transf&);
// transformation by matrix(3x3)
// multiplication with matrix is implemented in both cases, 
// from left and from right, but "constructors" (matrix::scaling(), matrix::translation(),...)
// create matrices, which work properly, when used from right: point * matrix;
// multiplication from left (matrix*point) is usefull, when matrix is orthogonal.
point operator * (const point&, const matrix&);
point operator * (const matrix&, const point&);


bool parallel(const gvector&, const gvector&, double = resnorm());
bool antiparallel(const gvector&, const gvector&, double = resnorm());
bool biparallel(const gvector&, const gvector&, double = resnorm());
bool perpendicular(const gvector&, const gvector&, double = resnorm());

}//namespace icl


#ifdef NDEBUG
#include "gvector.inl"
#endif

#endif
