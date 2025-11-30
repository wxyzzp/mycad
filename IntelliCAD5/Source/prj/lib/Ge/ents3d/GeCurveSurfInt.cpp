// File: GeCurveSurfInt.cpp
//

#include "GeCurveSurfInt.h"

//////////////////////////////////////////////////
// CGeCurveSurfInt class implementation
//////////////////////////////////////////////////

CGeCurveSurfInt::CGeCurveSurfInt()
{
}

CGeCurveSurfInt::CGeCurveSurfInt(const CGeCurve3d& curve, const CGeSurface& surf, const CGeTolerance& tol /* = CGeContext::s_defTol*/)
{
}

CGeCurveSurfInt::CGeCurveSurfInt(const CGeCurveSurfInt& source)
{
}

const CGeCurve3d*
CGeCurveSurfInt::curve() const
{
	return m_pCurve;
}

const CGeSurface*
CGeCurveSurfInt::surface() const
{
	return m_pSurface;
}

CGeTolerance
CGeCurveSurfInt::tolerance() const
{
	return m_tolerance;
}

int
CGeCurveSurfInt::numIntPoints(CGe::EGeIntersectError& error) const
{
	return 0;
}

CGePoint3d
CGeCurveSurfInt::intPoint(int intNum, CGe::EGeIntersectError& error) const
{
	return CGePoint3d();
}

void
CGeCurveSurfInt::getIntParams(int intNum, double& param1, CGePoint2d& param2, CGe::EGeIntersectError& error) const
{
}

void
CGeCurveSurfInt::getPointOnCurve(int intNum, CGePointOnCurve3d&, CGe::EGeIntersectError& error) const
{
}

void
CGeCurveSurfInt::getPointOnSurface(int intNum, CGePointOnSurface&, CGe::EGeIntersectError& error) const
{
}

void
CGeCurveSurfInt::getIntConfigs(int intNum, CGe::EcsiConfig& lower, CGe::EcsiConfig& higher, bool& bSmallAngle, CGe::EGeIntersectError& error) const
{
}

CGeCurveSurfInt&
CGeCurveSurfInt::set(const CGeCurve3d& curve, const CGeSurface& surf, const CGeTolerance& tol /* = CGeContext::s_defTol*/)
{
	return *this;
}

CGeCurveSurfInt&
CGeCurveSurfInt::operator = (const CGeCurveSurfInt& source)
{
	return *this;
}
