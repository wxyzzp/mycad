// File  :
// Author: Alexey Malov
#pragma once 
#ifndef _TRANSF_H_
#define _TRANSF_H_

#include "matrix.h"
#include "gvector.h"

namespace icl
{

class transf
{
	matrix m_matrix;			// always normalised: det == + or - 1
	gvector m_trans;
	bool m_identity;

    transf(const matrix&, const gvector&);

public:
    // ctors
    // default: creates identity transformation
	transf();
    // copy ctor
	transf(const transf&);
    // initialize from sds_matrix; 
    // i don't want to include "sds.h" here, so use that
    transf(const double[4][4]);

    // assignment
    const transf& operator = (const transf&);
    const transf& operator = (const double[4][4]);

    // returns rotation/reflection of the transformation
    const matrix& affine() const { return m_matrix; }
    // returns translation of the transformation
    const gvector& translation() const { return m_trans; }
    // is this transformation identity?
	bool identity() const {return m_identity;}

    // sets the transformation in scaling
	const transf& scale(double);
    // sets the transformation in scaling
	const transf& scale(double, double, double);
	// sets the transformation in scaling
	const transf& scale(const gvector&);

    // inverses the transformation
    const transf& inverse();
    // sets the transformation in rotation
    const transf& rotate(double, const gvector&);
    // sets the transformation in rotation
    const transf& rotate(double, const point&, const gvector&);
    // sets the transformation in reflection
	const transf& reflect(const gvector&);
    // sets the transformation in translation
	const transf& translate(const gvector&);
    // sets the transformation in arbitrary orthogonal transformation,
    // defined by right-hand coordinate system
	const transf& coordinate
    (
    const point&,
    const gvector&,
    const gvector&
    );

	const transf& projection
	(
	const point& root, 
	const gvector& normal
	);

    // comparing for (in)equality
	bool operator == (const transf&) const;
	bool operator != (const transf&) const;

    // multiplication of transformations
	transf operator * (const transf&) const;
    // multiplication, with assignment
	const transf& operator *= (const transf&);

    // friends
    friend transf scaling(double);
	friend transf scaling(double, double, double);
	friend transf scaling(const gvector&);

    friend transf rotation(double, const gvector&);
	friend transf reflection(const gvector&);
	friend transf translation(const gvector&);

	friend transf coordinate(const point&, const gvector&);

#if 0
	friend void projection
	(
	const point& root, 
	const gvector& normal,
	const gvector& view,
	transf& forw,
	transf& pr,
	transf back
	);
#endif
};

gvector operator * (const gvector&, const transf&);
point operator * (const point&, const transf&);

transf rotation
(
double angle, 
const point& root, 
const gvector& axis
);
transf coordinate
(
const point& origin,
const gvector& xaxis,
const gvector& yaxis
);
transf projection
(
const point& root, 
const gvector& normal
);
point projection
(
const point& pnt, 
const point& root, 
const gvector& normal, 
const gvector* view = 0
);

}

#endif
