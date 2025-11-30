// File  :
// Author: Alexey Malov
#include "gvector.h"
#include "transf.h"
#include "matrix.h"
#include "values.h"
#include <cassert>
#include <cmath>

#ifdef _DEBUG
#define inline
#include "gvector.inl"
#undef inline
#endif

namespace icl
{

bool gvector::operator == (const gvector& v) const
{
    return eq(m_coord[0], v[0]) &&
           eq(m_coord[1], v[1]) &&
           eq(m_coord[2], v[2]);
}

const gvector& gvector::operator *= (const transf& t)
{
    if (!t.identity())
        *this *= t.affine();
    
    return *this;
}

const gvector& gvector::operator *= (const matrix& m)
{
    return set(x()*m(0,0) + y()*m(1,0) + z()*m(2,0),
               x()*m(0,1) + y()*m(1,1) + z()*m(2,1),
               x()*m(0,2) + y()*m(1,2) + z()*m(2,2));
}

bool point::operator == (const point& p) const
{
    return eq(m_coord[0], p[0]) &&
           eq(m_coord[1], p[1]) &&
           eq(m_coord[2], p[2]);
}

const point& point::operator *= (const transf& t)
{
    if (!t.identity())
    {
        *this *= t.affine();
        *this += t.translation();
    }
    return *this;
}

const point& point::operator *= (const matrix& m)
{
    return set(x()*m(0,0) + y()*m(1,0) + z()*m(2,0),
               x()*m(0,1) + y()*m(1,1) + z()*m(2,1),
               x()*m(0,2) + y()*m(1,2) + z()*m(2,2));
}

bool parallel(const gvector& v1, const gvector& v2, double res)
{
	if (v1*v2 < res)
	{
		return v1%v2 > 0.;
	}
	return false;
}

bool antiparallel(const gvector& v1, const gvector& v2, double res)
{
	if (v1*v2 < res)
	{
		return v1%v2 < 0.;
	}
	return false;
}

bool biparallel(const gvector& v1, const gvector& v2, double res)
{
	return v1*v2 < res;
}

bool perpendicular(const gvector& v1, const gvector& v2, double res)
{
	return v1%v2 < res;
}

} //namespace icl
