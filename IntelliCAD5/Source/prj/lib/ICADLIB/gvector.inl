// File  :
// Author: Alexey Malov
#include <cassert>
#include <cmath>
#include "values.h"
namespace icl
{

inline
gvector::gvector()
{ 
	m_coord[0] = m_coord[1] = m_coord[2] = 0.0;
}

inline
gvector::gvector(const gvector& p)
{
    *this = p;
}

inline
gvector::gvector(const double p[3])
{
    m_coord[0] = p[0];
    m_coord[1] = p[1];
    m_coord[2] = p[2];
}

inline
gvector::gvector(double x, double y, double z)
{
    m_coord[0] = x; 
    m_coord[1] = y; 
    m_coord[2] = z;
}

inline
const gvector& gvector::operator = (const gvector& p)
{
    m_coord[0] = p[0];
    m_coord[1] = p[1];
    m_coord[2] = p[2];
    return *this;
}

inline
const gvector& gvector::operator = (double p[3])
{
    m_coord[0] = p[0];
    m_coord[1] = p[1];
    m_coord[2] = p[2];
    return *this;
}

inline
double gvector::operator [] (int i) const
{
    assert(i>=0 || i<3);
    return m_coord[i];
}

inline
double& gvector::operator [] (int i)
{
    assert(i>=0 || i<3);
    return m_coord[i];
}

inline
double gvector::x() const
{
    return m_coord[0];
}

inline 
double gvector::y() const
{
    return m_coord[1];
}

inline 
double gvector::z() const
{
    return m_coord[2];
}

inline
double& gvector::x()
{
    return m_coord[0];
}

inline 
double& gvector::y()
{
    return m_coord[1];
}

inline 
double& gvector::z()
{
    return m_coord[2];
}

inline
const gvector& gvector::set(double x, double y, double z)
{
    m_coord[0] = x; m_coord[1] = y; m_coord[2] = z;
    return *this;
}

inline
gvector::operator double* ()
{
    return &m_coord[0];
}

inline
gvector::operator const double* () const
{
    return &m_coord[0];
}

inline
double gvector::norm() const
{
    return sqrt(m_coord[0]*m_coord[0]+m_coord[1]*m_coord[1]+m_coord[2]*m_coord[2]);
}

inline
double gvector::norm2() const
{
    return m_coord[0]*m_coord[0]+m_coord[1]*m_coord[1]+m_coord[2]*m_coord[2];
}

inline
gvector gvector::normalized() const
{
	return *this/norm();
}
inline
const gvector& gvector::normalize()
{
	return *this /= norm();
}

inline
const gvector& gvector::operator *= (double d)
{
    m_coord[0] *= d;
    m_coord[1] *= d;
    m_coord[2] *= d;
    return *this;
}

inline
gvector gvector::operator / (double d) const
{
    return (1./d)**this;
}

inline
const gvector& gvector::operator /= (double d)
{
    double r_d = 1./d;
    m_coord[0] *= r_d;
    m_coord[1] *= r_d;
    m_coord[2] *= r_d;
    return *this;
}

inline
const gvector& gvector::operator *= (const gvector& v)
{

    return *this = *this * v;
}

inline
double gvector::operator % (const gvector& v) const
{
    return (m_coord[0]*v.m_coord[0] + 
            m_coord[1]*v.m_coord[1] + 
            m_coord[2]*v.m_coord[2]);
}

inline
gvector gvector::operator - (const gvector& v) const
{
    return gvector(m_coord[0] - v.m_coord[0],
                   m_coord[1] - v.m_coord[1],
                   m_coord[2] - v.m_coord[2]);
}

inline
gvector gvector::operator - () const
{
    return gvector(-m_coord[0], -m_coord[1], -m_coord[2]);
}

inline
const gvector& gvector::operator -= (const gvector& v)
{
    m_coord[0] -= v[0];
    m_coord[1] -= v[1];
    m_coord[2] -= v[2];
    return *this;
}

inline
gvector gvector::operator + (const gvector& v) const
{
    return gvector(m_coord[0] + v.m_coord[0],
                   m_coord[1] + v.m_coord[1],
                   m_coord[2] + v.m_coord[2]);
}

inline
const gvector& gvector::operator += (const gvector& v)
{
    m_coord[0] += v[0];
    m_coord[1] += v[1];
    m_coord[2] += v[2];
    return *this;
}

inline
bool gvector::operator < (double eps) const
{
    return eq(m_coord[0], 0., eps) &&
           eq(m_coord[1], 0., eps) &&
           eq(m_coord[2], 0., eps);
}

inline
bool gvector::operator > (double eps) const
{
    return !eq(m_coord[0], 0., eps) ||
           !eq(m_coord[1], 0., eps) ||
           !eq(m_coord[2], 0., eps);
}

inline
bool gvector::operator != (const gvector& v) const
{
    return !(*this == v);
}

inline
gvector gvector::operator * (const gvector& v) const
{
    return gvector(m_coord[1]*v.m_coord[2]-m_coord[2]*v.m_coord[1],
                  -m_coord[0]*v.m_coord[2]+m_coord[2]*v.m_coord[0],
                   m_coord[0]*v.m_coord[1]-m_coord[1]*v.m_coord[0]);
}

inline
gvector operator * (const gvector& v, double d)
{
    return gvector(d*v[0], d*v[1], d*v[2]);
}

inline
gvector operator * (double d, const gvector& v)
{
    return v*d;
}

inline
point::point()
{
	m_coord[0] = m_coord[1] = m_coord[2] = 0.0;
}

inline
point::point(const point& p)
{
    *this = p;
}

inline
point::point(const double p[3])
{
    m_coord[0] = p[0];
    m_coord[1] = p[1];
    m_coord[2] = p[2];
}

inline
point::point(double x, double y, double z)
{
    m_coord[0] = x; 
    m_coord[1] = y; 
    m_coord[2] = z;
}

inline
const point& point::operator = (const point& p)
{
    m_coord[0] = p[0];
    m_coord[1] = p[1];
    m_coord[2] = p[2];
    return *this;
}

inline
const point& point::operator = (double p[3])
{
    m_coord[0] = p[0];
    m_coord[1] = p[1];
    m_coord[2] = p[2];
    return *this;
}

inline
double point::operator [] (int i) const
{
    assert(i>=0 || i<3);
    return m_coord[i];
}

inline
double& point::operator [] (int i)
{
    assert(i>=0 || i<3);
    return m_coord[i];
}

inline
double point::x() const
{
    return m_coord[0];
}

inline 
double point::y() const
{
    return m_coord[1];
}

inline 
double point::z() const
{
    return m_coord[2];
}

inline
double& point::x()
{
    return m_coord[0];
}

inline 
double& point::y()
{
    return m_coord[1];
}

inline 
double& point::z()
{
    return m_coord[2];
}

inline
const point& point::set(double x, double y, double z)
{
    m_coord[0] = x; m_coord[1] = y; m_coord[2] = z;
    return *this;
}

inline
const point& point::operator += (const gvector& v)
{
    m_coord[0] += v[0];
    m_coord[1] += v[1];
    m_coord[2] += v[2];
    return *this;
}

inline
const point& point::operator -= (const gvector& v)
{
    m_coord[0] -= v[0];
    m_coord[1] -= v[1];
    m_coord[2] -= v[2];
    return *this;
}

inline
gvector point::operator - (const point& p) const
{
    return gvector(m_coord[0]-p[0], m_coord[1]-p[1], m_coord[2]-p[2]);
}

inline
point point::operator - (const gvector& v) const
{
    return point(m_coord[0] - v[0], m_coord[1] - v[1], m_coord[2] - v[2]);
}

inline
point::operator double* ()
{
    return &m_coord[0];
}

inline
point::operator const double* () const
{
    return &m_coord[0];
}

inline
bool point::operator != (const point& p) const
{
    return !(*this == p);
}

inline
point operator + (const point& p, const gvector& v)
{
    return point(p[0] + v[0], p[1] + v[1], p[2] + v[2]);
}

inline
point operator + (const gvector& v, const point& p)
{
    return p + v;
}

}
