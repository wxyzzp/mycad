// File  :
// Author: Alexey Malov
#include "transf.h"
#include <cmath>

namespace icl
{

// Public constructor for the identity matrix
transf::transf()
{
    m_matrix.identity();
    m_trans.set(0.,0.,0.);
	m_identity = true;
}

transf::transf(const transf& t)
{
	if (t.identity())
		*this = transf();
	else
		*this = t;
}

transf::transf(const double mtx[4][4])
{
    for (int i = 0; i < matrix::dim; ++i)
    {
        m_matrix(i,0) = mtx[0][i];
        m_matrix(i,1) = mtx[1][i];
        m_matrix(i,2) = mtx[2][i];
    }
    m_trans.set(mtx[0][3], mtx[1][3], mtx[2][3]);
    m_identity = false;
}

transf::transf(const matrix& m, const gvector& v)
{
    m_matrix = m;
    m_trans = v;
    m_identity = false;
}

const transf& transf::operator = (const transf& t)
{
    m_matrix = t.affine();
    m_trans = t.translation();
    m_identity = t.identity();

    return *this;
}

const transf& transf::operator = (const double mtx[4][4])
{
    for (int i = 0; i < matrix::dim; ++i)
    {
        m_matrix(i,0) = mtx[0][i];
        m_matrix(i,1) = mtx[1][i];
        m_matrix(i,2) = mtx[2][i];
    }
    m_identity = false;
    return *this;
}

const transf& transf::scale(double factor)
{
	m_matrix.scaling(factor);
	m_trans.set(0.,0.,0.);
	m_identity = false;
    return *this;
}

const transf& transf::scale
(
double xfactor,
double yfactor,
double zfactor
)
{
    m_matrix.scaling(xfactor, yfactor, zfactor);
    m_trans.set(0.,0.,0.);
    m_identity = false;
    return *this;
}

const transf& transf::scale(const gvector& v)
{
	m_matrix.scaling(v);
	m_trans.set(0.,0.,0.);
	m_identity = false;
	return *this;
}

const transf& transf::rotate
(
double angle,
const gvector& axis
)
{
    m_matrix.rotation(angle, axis);
    m_trans.set(0.,0.,0.);
    m_identity = false;
    return *this;
}

const transf& transf::rotate
(
double angle,
const point& root, 
const gvector& axis
)
{
    gvector shift(root - point(0.,0.,0.));
    return *this = icl::translation(-shift)*rotation(angle, axis)*icl::translation(shift);
}

const transf& transf::reflect(const gvector& axis)
{
    m_matrix.reflection(axis);
    m_trans.set(0.,0.,0.);
    m_identity = false;
    return *this;
}

const transf& transf::translate(const gvector& trans)
{
    m_matrix.identity();
    m_trans = trans;
    m_identity = false;
    return *this;
}

const transf& transf::coordinate
(
const point& origin,
const gvector& xaxis,
const gvector& yaxis
)
{
	if (eq(xaxis.x(), 1.) && eq(xaxis.y(), 0.) && eq(xaxis.z(), 0.) &&
		eq(yaxis.x(), 0.) && eq(yaxis.y(), 1.) && eq(yaxis.z(), 0.))
	{
        m_matrix.identity();

        const point zero(0.,0.,0.);
        if (origin == zero)
        {
            m_trans.set(0.,0.,0.);
            m_identity = true;
        }
        else
        {
            m_trans = zero - origin;
            m_identity = false;
        }

        return *this;
	}

    gvector nxaxis = xaxis.normalized();
    gvector nyaxis = (yaxis - (yaxis%nxaxis)*nxaxis).normalized();
	gvector nzaxis = nxaxis*nyaxis;

    m_matrix.coordinate(nxaxis, nyaxis, nzaxis);
	point p = origin*m_matrix;
    m_trans.set(-p.x(), -p.y(), -p.z());
    m_identity = false;

    return *this;
}

const transf& transf::projection
(
const point& root, 
const gvector& normal
)
{
	if (normal < resabs())
		return *this = scaling(0.);

	gvector newz = normal.normalized();
	gvector newx(-newz.y(), newz.x(), 0.);
	if (newx < resabs())
		newx.set(1., 0., 0.);
	else
		newx.normalize();
	gvector newy = newz*newx;

	matrix mplane(newx, newy, newz);

	m_matrix[0] = 1. - mplane[2]*mplane[2];	  m_matrix[1] =	   - mplane[5]*mplane[2]; m_matrix[2] =    - mplane[8]*mplane[2];
	m_matrix[3] =	 - mplane[2]*mplane[5];	  m_matrix[4] = 1. - mplane[5]*mplane[5]; m_matrix[5] =    - mplane[8]*mplane[5];
	m_matrix[6] =	 - mplane[2]*mplane[8];	  m_matrix[7] =    - mplane[5]*mplane[8]; m_matrix[8] = 1. - mplane[8]*mplane[8];

	m_trans.set(0.,0., root.x()*mplane[2] + root.y()*mplane[5] + root.z()*mplane[8]);

	double x = m_trans.x(), y = m_trans.y(), z = m_trans.z();
	m_trans.x() = x*mplane[0] + y*mplane[1] + z*mplane[2];
	m_trans.y() = x*mplane[3] + y*mplane[4] + z*mplane[5];
	m_trans.z() = x*mplane[6] + y*mplane[7] + z*mplane[8];

	m_identity = false;

	return *this;
}

bool transf::operator == (const transf& t) const
{
    return (identity() && t.identity()) ||
           (m_matrix == t.m_matrix && m_trans == t.m_trans);
}

bool transf::operator != (const transf& t) const
{
    return !(operator ==(t));
}

// Multiplication of transforms
transf transf::operator * (const transf& t) const
{
	if (identity())
    {
		if (t.identity())
			return transf();
		else
			return t;
    }
	else
    {
		if (t.identity())
			return *this;
		else
        {
			matrix m = m_matrix*t.affine();
            gvector trans = m_trans*t.affine() + t.translation();
           
            return transf(m, trans);
		}
    }
}

const transf& transf::operator *= (const transf& t)
{
	if (identity())
    {
		if (t.identity())
			return *this;
		else
            return *this = t;
    }
	else
    {
		if (t.identity())
			return *this;
		else
        {
			m_matrix *= t.m_matrix;
            m_trans *= t.m_matrix;
            m_trans += t.m_trans;
            
            return *this;
		}
    }
}

const transf& transf::inverse()
{
	if (identity())
		return *this;

    m_matrix.inverse(m_matrix);
    m_trans = -(m_trans*m_matrix);
    return *this;
}

gvector operator * 
(
const gvector& vec,
const transf& t
)
{
	if (t.identity())
		return vec;
	else
		return vec*t.affine();
}

point operator * 
(
const point& pos,
const transf& t
)
{
	if (t.identity())
		return pos;
	else
		return (pos*t.affine()) + t.translation();
}

transf scaling(double d)
{
    matrix m;
    return transf(m.scaling(d), gvector(0.,0.,0.));
}

transf scaling
(
double dx, 
double dy, 
double dz
)
{
    matrix m;
    return transf(m.scaling(dx, dy, dz), gvector(0.,0.,0.));
}

transf scaling(const gvector& v)
{
	matrix m;
	return transf(m.scaling(v), gvector(0.,0.,0.));
}

transf rotation
(
double angle, 
const gvector& axis
)
{
    matrix m;
    return transf(m.rotation(angle, axis), gvector(0.,0.,0.));
}

transf rotation
(
double angle,
const point& root,
const gvector& axis
)
{
    gvector shift = root - gvector(0.,0.,0.);
    return icl::translation(-shift)*icl::rotation(angle, axis)*icl::translation(shift);
}

transf reflection(const gvector& normal)
{
    matrix m;
    return transf(m.reflection(normal), gvector(0.,0.,0.));
}

transf translation(const gvector& shift)
{
    matrix m;
    return transf(m.identity(), shift);
}

transf coordinate
(
const point& origin,
const gvector& xaxis,
const gvector& yaxis
)
{
    transf t;
    return t.coordinate(origin, xaxis, yaxis);
}

transf coordinate
(
const point& origin,
const gvector& normal
)
{
    double l = normal.norm();
    if (eq(l, 0.))
		//return degenerated transform
        return scaling(0.);

    gvector zaxis = normal/l;

    if (zaxis == gvector(0.,0.,1.))
		//unit transform
        return transf();

	double cap = 1./64.;  /* square polar cap. */

	gvector xaxis;
    if (eq(zaxis.x(), 0., cap) &&
        eq(zaxis.y(), 0., cap))
    {
        // ECSX = WCSY X ECSZ
        xaxis.set(zaxis.z(), 0., -zaxis.x());
    }
    else
    {
        // ECSX = WCSZ X ECSZ
        xaxis.set(-zaxis.y(), zaxis.x(), 0.);
    }

	l = xaxis.norm();
    if (eq(l, 0.))
		//return degenerated trasform
		return scaling(0.);
	xaxis /= l;

	matrix m;
	return transf(m.coordinate(xaxis, zaxis*xaxis, zaxis), (origin - point(0.,0.,0.)));
}

transf projection
(
const point& root, 
const gvector& normal
)
{
	transf t;
	return t.projection(root, normal);
}

point projection
(
const point& pnt, 
const point& root, 
const gvector& normal, 
const gvector* view
)
{
	if (view == 0)
	{
		return pnt + ((root - pnt) % normal) * normal;
	}
	else
	{
		return pnt + ((root - pnt) % normal) / ((*view) % normal) * (*view);
	}
}

#if 0
void projection
(
const point& root, 
const gvector& normal,
const gvector& view,
transf& forw,
transf& pr,
transf back
)
{
	gvector newz = normal; newz.normalize();
	gvector newx(-newz.y(), newz.x(), 0.);
	if (newx.normalize() < resabs())
	{
		newx.set(1., 0., 0.);
	}
	gvector newy = newz*newx;

	forw = coordinate(root, newx, newy);
	{

		matrix p(normal*view.x(),
				  normal*view.y(),
				  normal*view.z());
		p *= 1./(view%newz);
		pr = transf(p, gvector(0., 0., 0.));
	}
	back = forw; back.inverse();
}
#endif
}//namespace icl
