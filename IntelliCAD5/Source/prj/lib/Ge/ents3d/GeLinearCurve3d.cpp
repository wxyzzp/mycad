// File: GeLinearCurve3d.cpp
//

#include "GeLinearCurve3d.h"
#include "GeVector3d.h"

//////////////////////////////////////////////////
// CGeLinearCurve3d class implementation
//////////////////////////////////////////////////

CGeLinearCurve3d::~CGeLinearCurve3d()
{
}

bool
CGeLinearCurve3d::intersectWith(const CGeLinearCurve3d& linearCurve, CGePoint3d& intPoint, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeLinearCurve3d::intersectWith(const CGePlanarSurface& planarSurface, CGePoint3d& intPoint, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeLinearCurve3d::projIntersectWith(const CGeLinearCurve3d& linearCurve, const CGeVector3d& projDir, CGePoint3d& pointOnThisCurve, CGePoint3d& pointOnOtherCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeLinearCurve3d::overlap(const CGeLinearCurve3d& linearCurve, CGeLinearCurve3d*& overlapCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeLinearCurve3d::isParallelTo(const CGeLinearCurve3d& linearCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeLinearCurve3d::isParallelTo(const CGePlanarSurface& planarSurface, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeLinearCurve3d::isPerpendicularTo(const CGeLinearCurve3d& linearCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeLinearCurve3d::isPerpendicularTo(const CGePlanarSurface& planarSurface, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeLinearCurve3d::isColinearTo(const CGeLinearCurve3d& linearCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGePoint3d
CGeLinearCurve3d::pointOnLine() const
{
	return CGePoint3d();
}

bool
CGeLinearCurve3d::isOn(const CGePoint3d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeLinearCurve3d::isOn(const CGePoint3d& point, double& param, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeLinearCurve3d::isOn(double param, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeLinearCurve3d::isOn(const CGePlane& plane, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGeVector3d
CGeLinearCurve3d::direction() const
{
	return CGeVector3d();
}

void
CGeLinearCurve3d::getLine(CGeLine3d& line) const
{
}

void
CGeLinearCurve3d::getPerpPlane(const CGePoint3d& point, CGePlane& plane) const
{
}

CGeLinearCurve3d&
CGeLinearCurve3d::operator = (const CGeLinearCurve3d& source)
{
	return *this;
}

CGeLinearCurve3d::CGeLinearCurve3d()
{
}

CGeLinearCurve3d::CGeLinearCurve3d(const CGeLinearCurve3d& source)
{
}
