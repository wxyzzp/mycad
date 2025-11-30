// File: GePointOnCurve3d.cpp
//

#include "GePointOnCurve3d.h"
#include "GeVector3d.h"

//////////////////////////////////////////////////
// CGePointOnCurve3d class implementation
//////////////////////////////////////////////////

CGePointOnCurve3d::CGePointOnCurve3d()
{
}

CGePointOnCurve3d::CGePointOnCurve3d(const CGeCurve3d& curve)
{
}

CGePointOnCurve3d::CGePointOnCurve3d(const CGeCurve3d& curve, double param)
{
}

CGePointOnCurve3d::CGePointOnCurve3d(const CGePointOnCurve3d& source)
{
}

const CGeCurve3d*
CGePointOnCurve3d::curve() const
{
	return m_pCurve;
}

double
CGePointOnCurve3d::parameter() const
{
	return m_parameter;
}

CGePoint3d
CGePointOnCurve3d::point() const
{
	return m_point;
}

CGePoint3d
CGePointOnCurve3d::point(double param)
{
	return CGePoint3d();
}

CGePoint3d
CGePointOnCurve3d::point(const CGeCurve3d& curve, double param)
{
	return CGePoint3d();
}

CGeVector3d
CGePointOnCurve3d::deriv(int order) const
{
	return CGeVector3d();
}

CGeVector3d
CGePointOnCurve3d::deriv(int order, double param)
{
	return CGeVector3d();
}

CGeVector3d
CGePointOnCurve3d::deriv(int order, const CGeCurve3d& curve, double param)
{
	return CGeVector3d();
}

bool
CGePointOnCurve3d::isSingular(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGePointOnCurve3d::curvature(double& result)
{
	return false;
}

bool
CGePointOnCurve3d::curvature(double param, double& result)
{
	return false;
}

CGePointOnCurve3d&
CGePointOnCurve3d::setCurve(const CGeCurve3d& curve)
{
	return *this;
}

CGePointOnCurve3d&
CGePointOnCurve3d::setParameter(double param)
{
	m_parameter = param;
	return *this;
}

CGePointOnCurve3d&
CGePointOnCurve3d::operator = (const CGePointOnCurve3d& source)
{
	return *this;
}
