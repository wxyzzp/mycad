// File: GePointOnCurve2d.cpp
//

#include "GePointOnCurve2d.h"
#include "GeVector2d.h"

//////////////////////////////////////////////////
// CGePointOnCurve2d class implementation
//////////////////////////////////////////////////

CGePointOnCurve2d::CGePointOnCurve2d()
{
}

CGePointOnCurve2d::CGePointOnCurve2d(const CGeCurve2d& curve)
{
}

CGePointOnCurve2d::CGePointOnCurve2d(const CGeCurve2d& curve, double param)
{
}

CGePointOnCurve2d::CGePointOnCurve2d(const CGePointOnCurve2d& source)
{
}

const CGeCurve2d*
CGePointOnCurve2d::curve() const
{
	return m_pCurve;
}

double
CGePointOnCurve2d::parameter() const
{
	return m_parameter;
}

CGePoint2d
CGePointOnCurve2d::point() const
{
	return m_point;
}

CGePoint2d
CGePointOnCurve2d::point(double param)
{
	return m_point;
}

CGePoint2d
CGePointOnCurve2d::point(const CGeCurve2d& curve, double param)
{
	return m_point;
}

CGeVector2d
CGePointOnCurve2d::deriv(int order) const
{
	return CGeVector2d();
}

CGeVector2d
CGePointOnCurve2d::deriv(int order, double param)
{
	return CGeVector2d();
}

CGeVector2d
CGePointOnCurve2d::deriv(int order, const CGeCurve2d& curve, double param)
{
	return CGeVector2d();
}

bool
CGePointOnCurve2d::isSingular(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGePointOnCurve2d::curvature(double& result)
{
	return false;
}

bool
CGePointOnCurve2d::curvature(double param, double& result)
{
	return false;
}

CGePointOnCurve2d&
CGePointOnCurve2d::setCurve(const CGeCurve2d& curve)
{
	return *this;
}

CGePointOnCurve2d&
CGePointOnCurve2d::setParameter(double param)
{
	m_parameter = param;
	return *this;
}

CGePointOnCurve2d&
CGePointOnCurve2d::operator = (const CGePointOnCurve2d& source)
{
	return *this;
}
