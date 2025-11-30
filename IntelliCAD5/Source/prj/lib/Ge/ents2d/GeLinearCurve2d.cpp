// File: GeLinearCurve2d.cpp
//

#include "GeLinearCurve2d.h"
#include "GeVector2d.h"

//////////////////////////////////////////////////
// CGeLinearCurve2d class implementation
//////////////////////////////////////////////////

CGeLinearCurve2d::~CGeLinearCurve2d()
{
}

bool
CGeLinearCurve2d::intersectWith(const CGeLinearCurve2d& linearCurve, CGePoint2d& intPoint, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeLinearCurve2d::overlap(const CGeLinearCurve2d& linearCurve, CGeLinearCurve2d*& overlapCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeLinearCurve2d::isParallelTo(const CGeLinearCurve2d& linearCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeLinearCurve2d::isPerpendicularTo(const CGeLinearCurve2d& linearCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeLinearCurve2d::isColinearTo(const CGeLinearCurve2d& linearCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGePoint2d
CGeLinearCurve2d::pointOnLine() const
{
	return CGePoint2d();
}

CGeVector2d
CGeLinearCurve2d::direction() const
{
	return CGeVector2d();
}

void
CGeLinearCurve2d::getLine(CGeLine2d& line) const
{
}

void
CGeLinearCurve2d::getPerpLine(const CGePoint2d& point, CGeLine2d& line) const
{
}

CGeLinearCurve2d&
CGeLinearCurve2d::operator = (const CGeLinearCurve2d& source)
{
	return *this;
}

CGeLinearCurve2d::CGeLinearCurve2d()
{
}

CGeLinearCurve2d::CGeLinearCurve2d(const CGeLinearCurve2d& source)
{
}
