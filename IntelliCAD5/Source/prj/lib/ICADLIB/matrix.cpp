// File  :
// Author: Alexey Malov
#include "matrix.h"
#include "gvector.h"
#include "values.h"
#include <math.h>

#ifdef _DEBUG
#define inline
#include "matrix.inl"
#undef inline
#endif

namespace icl
{

matrix::matrix(const matrix& m)
{
    for (int i = 0; i < nelem; ++i)
        m_v[i] = m[i];
}

matrix::matrix
(
const gvector& x,
const gvector& y,
const gvector& z
)
{
    for (int i = 0; i < dim; ++i)
    {
        m_m[i][0] = x[i];
        m_m[i][1] = y[i];
        m_m[i][2] = z[i];
    }
}

const matrix& matrix::operator = (const matrix& m)
{
    for (int i = 0; i < nelem; ++i)
        m_v[i] = m[i];
    return *this;
}

matrix matrix::operator *(const matrix& m2) const
{
    matrix m;
    const matrix& m1 = *this;
	for (int i=0; i < ndim; i++)
    {
	    for (int j=0; j < mdim; j++)
        {
            m(i,j) = m1(i,0)*m2(0,j)+
                     m1(i,1)*m2(1,j)+
                     m1(i,2)*m2(2,j);
	    }
	}
    return m;
}

const matrix& matrix::operator *= (const matrix &m2)
{
    matrix& m1 = *this;
	for (int i = 0 ; i < ndim; i++) 
    {
		double c[mdim];
		for (int j = 0; j < mdim; j++)
        {
			c[j] = m1(i,0)*m2(0,j)+
				   m1(i,1)*m2(1,j)+
				   m1(i,2)*m2(2,j);
		}
        m1(i,0) = c[0];
        m1(i,1) = c[1];
        m1(i,2) = c[2];
	}

	return m1;
}

double matrix::determinant() const
{
	double det1 = m_m[1][1] * m_m[2][2] - m_m[2][1] * m_m[1][2];
	double det2 = m_m[0][1] * m_m[2][2] - m_m[2][1] * m_m[0][2];
	double det3 = m_m[0][1] * m_m[1][2] - m_m[1][1] * m_m[0][2];

	return	m_m[0][0]*det1 -
            m_m[1][0]*det2 +
			m_m[2][0]*det3;
}

int matrix::inverse(matrix& m) const
{
	double det = determinant();

    double resnorm = 1.e-10;
    if (det < resnorm)
        return 0;
#if 0
	result.m_m[0][0] = m_m[1][1] * m_m[2][2] - m_m[2][1] * m_m[1][2];
	result.m_m[0][1] = -(m_m[0][1] * m_m[2][2] - m_m[2][1] * m_m[0][2]);
	result.m_m[0][2] = m_m[0][1] * m_m[1][2] - m_m[1][1] * m_m[0][2];

	result.m_m[1][0] = -(m_m[1][0] * m_m[2][2] - m_m[2][0] * m_m[1][2]);
	result.m_m[1][1] = m_m[0][0] * m_m[2][2] - m_m[2][0] * m_m[0][2];
	result.m_m[1][2] = -(m_m[0][0] * m_m[1][2] - m_m[1][0] * m_m[0][2]);

	result.m_m[2][0] = m_m[1][0] * m_m[2][1] - m_m[2][0] * m_m[1][1];
	result.m_m[2][1] = -(m_m[0][0] * m_m[2][1] - m_m[2][0] * m_m[0][1]);
	result.m_m[2][2] = m_m[0][0] * m_m[1][1] - m_m[1][0] * m_m[0][1];
#endif

	double rdet = 1.0 / det;

	if (this == &m)
	{
		matrix result;

		result.m_v[0] =  (m_v[4] * m_v[8] - m_v[7] * m_v[5])*rdet;
		result.m_v[1] = -(m_v[1] * m_v[8] - m_v[7] * m_v[2])*rdet;
		result.m_v[2] =  (m_v[1] * m_v[5] - m_v[4] * m_v[2])*rdet;

		result.m_v[3] = -(m_v[3] * m_v[8] - m_v[6] * m_v[5])*rdet;
		result.m_v[4] =  (m_v[0] * m_v[8] - m_v[6] * m_v[2])*rdet;
		result.m_v[5] = -(m_v[0] * m_v[5] - m_v[3] * m_v[2])*rdet;

		result.m_v[6] =  (m_v[3] * m_v[7] - m_v[6] * m_v[4])*rdet;
		result.m_v[7] = -(m_v[0] * m_v[7] - m_v[6] * m_v[1])*rdet;
		result.m_v[8] =  (m_v[0] * m_v[4] - m_v[3] * m_v[1])*rdet;

	    m = result;
	}
	else
	{
		m.m_v[0] =  (m_v[4] * m_v[8] - m_v[7] * m_v[5])*rdet;
		m.m_v[1] = -(m_v[1] * m_v[8] - m_v[7] * m_v[2])*rdet;
		m.m_v[2] =  (m_v[1] * m_v[5] - m_v[4] * m_v[2])*rdet;

		m.m_v[3] = -(m_v[3] * m_v[8] - m_v[6] * m_v[5])*rdet;
		m.m_v[4] =  (m_v[0] * m_v[8] - m_v[6] * m_v[2])*rdet;
		m.m_v[5] = -(m_v[0] * m_v[5] - m_v[3] * m_v[2])*rdet;

		m.m_v[6] =  (m_v[3] * m_v[7] - m_v[6] * m_v[4])*rdet;
		m.m_v[7] = -(m_v[0] * m_v[7] - m_v[6] * m_v[1])*rdet;
		m.m_v[8] =  (m_v[0] * m_v[4] - m_v[3] * m_v[1])*rdet;
	}

	return 1;
}

const matrix& matrix::operator *= (double d)
{
    matrix& m = *this;
	for (int i = 0; i < 3; ++i)
    {
        m(i,0) *= d;
        m(i,1) *= d;
        m(i,2) *= d;
    }

	return m;
}

const matrix& matrix::identity()
{
    zero();
    m_m[0][0] = m_m[1][1] = m_m[2][2] = 1.;

    return *this;
}

const matrix& matrix::scaling(double factor)
{
	zero();
	m_m[0][0] = m_m[1][1] = m_m[2][2] = factor;
	return *this;
}

const matrix& matrix::scaling
(
double xfactor,
double yfactor,
double zfactor
)
{
	zero();

	m_m[0][0] = xfactor;
	m_m[1][1] = yfactor;
	m_m[2][2] = zfactor;

	return *this;
}

const matrix& matrix::scaling(const gvector& factor)
{
 	zero();

    m_m[0][0] = factor[0];
	m_m[1][1] = factor[1];
	m_m[2][2] = factor[2];

	return *this;
}

const matrix& matrix::rotation
(
double angle,
const gvector& axis
)
{
	gvector uaxis = axis; uaxis.normalize();
	double sin_angle = sin(angle);
	double cos_angle = cos(angle);
	double one_minus_cos = 1. - cos_angle;

	// The 3 by 3 rotation matrix has three summed components.
	//
	// First the outer product of the axis vector with itself,
	// conditioned by (1 - cos), second the identity, scaled by
	// cos angle, and finally the antisymmetric form of the axis
	// vector, multiplied by sin angle.
	//
	// We set up the first two components in the loops, then do
	// the last component separately.

    matrix& m = *this;
    for (int i = 0; i < matrix::dim; ++i)
    {
		// Outer product
        for (int j = 0; j < matrix::dim; ++j)
        {
			m(i,j) = one_minus_cos*uaxis[i]*uaxis[j];
		}

		// Main diagonal
		m(i,i) += cos_angle;
	}

	// Antisymmetric part

	gvector anti = uaxis*sin_angle;
	m(1,2) += anti.x();
	m(2,1) -= anti.x();
	m(2,0) += anti.y();
	m(0,2) -= anti.y();
	m(0,1) += anti.z();
	m(1,0) -= anti.z();

	return m;
}

const matrix& matrix::reflection(const gvector& axis)
{
	gvector	uaxis = axis; uaxis.normalize();

	// The 3 by 3 reflection matrix is the identity minus twice
	// the outer product of the normalised axis with itself.

    matrix& m = *this;
	for (int i = 0; i < ndim; ++i)
    {
		// Outer product
		for (int j = 0; j < mdim; ++j)
			m(i,j) = -2.*uaxis[i]*uaxis[j];

		// Main diagonal
		m(i,i) += 1.;
	}

	return m;
}

const matrix& matrix::coordinate
(
const gvector& x,
const gvector& y,
const gvector& z
)
{
    for (int i = 0; i < dim; ++i)
    {
        m_m[i][0] = x[i];
        m_m[i][1] = y[i];
        m_m[i][2] = z[i];
    }
    return *this;
}

bool matrix::operator == (const matrix& m) const
{
    for (int i = 0; i < nelem; ++i)
    {
        if (!eq(m_v[i], m[i]))
            return false;
    }
    return true;
}

// Initialise a matrix to zero

void matrix::zero()
{
	for (int i = 0; i < nelem; ++i)
        m_v[i] = 0.0;
}

// Transpose the matrix
matrix matrix::transposition() const
{
	return matrix(gvector(m_m[0][0], m_m[0][1], m_m[0][2]),
				  gvector(m_m[1][0], m_m[1][1], m_m[1][2]),
				  gvector(m_m[2][0], m_m[2][1], m_m[2][2]));
}

const matrix& matrix::transpose()
{
	for (int i = 0; i < dim-1; i++)
		for (int j = 1; j < dim; j++)
		{
			double e = m_m[i][j];
			m_m[i][j] = m_m[j][i];
			m_m[j][i] = e;
		}
	return *this;
}

// Transform vector
#if 0
gvector operator * (const matrix& m, const gvector& v)
{
#if 0
	gvector rv;
	for (int i = 0; i < 3; ++i)
    {
		rv[i] = m(i,0)*v[0] +
				m(i,1)*v[1] +
				m(i,2)*v[2];
    }
	return rv;
#else
    return point(v.x()*m(0,0) + v.y()*m(0,1) + v.z()*m(0,2),
                 v.x()*m(1,0) + v.y()*m(1,1) + v.z()*m(1,2),
                 v.x()*m(2,0) + v.y()*m(2,1) + v.z()*m(2,2));
#endif
}
#endif
gvector operator * (const gvector& v, const matrix &m)
{
#if 0
	gvector rv;
	for (int i = 0; i < 3; ++i)
    {
		rv[i] = v[0]*m(0,i)+
				v[1]*m(1,i)+
				v[2]*m(2,i);
    }
	return rv;
#else
    return point(v.x()*m(0,0) + v.y()*m(1,0) + v.z()*m(2,0),
                 v.x()*m(0,1) + v.y()*m(1,1) + v.z()*m(2,1),
                 v.x()*m(0,2) + v.y()*m(1,2) + v.z()*m(2,2));
#endif
}

// Transform position
point operator * (const point& p, const matrix &m)
{
#if 0
	point rp;
	for (int i = 0; i < 3; ++i)
    {
		rp[i] = p[0]* m(0,i)+
				p[1]* m(1,i)+
				p[2]* m(2,i);
    }
	return rp;
#else
    return point(p.x()*m(0,0) + p.y()*m(1,0) + p.z()*m(2,0),
                 p.x()*m(0,1) + p.y()*m(1,1) + p.z()*m(2,1),
                 p.x()*m(0,2) + p.y()*m(1,2) + p.z()*m(2,2));
#endif
}

matrix operator * (const matrix& m1, double d)
{
	matrix m;
                            
    for (int i = 0; i < 3; ++i)
    {
        m(i,0) = m1(i,0)*d;
        m(i,1) = m1(i,1)*d;
        m(i,2) = m1(i,2)*d;
    }
	return m;
}

matrix operator * (double d, const matrix& m1)
{
	matrix m;
                            
    for (int i = 0; i < 3; ++i)
    {
        m(i,0) = m1(i,0)*d;
        m(i,1) = m1(i,1)*d;
        m(i,2) = m1(i,2)*d;
    }
	return m;
}

}//namespace icl
