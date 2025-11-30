// File: GePlane.cpp
//

#include "GePlane.h"

//////////////////////////////////////////////////
// CGePlane class implementation
//////////////////////////////////////////////////

const CGePlane	CGePlane::s_xyPlane;
const CGePlane	CGePlane::s_yzPlane;
const CGePlane	CGePlane::s_zxPlane;


CGePlane::CGePlane()
{
}

CGePlane::CGePlane(const CGePlane& source)
{
}

CGePlane::CGePlane(const CGePoint3d& origin, const CGeVector3d& normal)
{
}

CGePlane::CGePlane(const CGePoint3d& pointU, const CGePoint3d& origin, const CGePoint3d& pointV)
{
}

CGePlane::CGePlane(const CGePoint3d& origin, const CGeVector3d& uAxis, const CGeVector3d& vAxis)
{
}

CGePlane::CGePlane(double a, double b, double c, double d)
{
}

double
CGePlane::signedDistanceTo(const CGePoint3d& point) const
{
	return 0.0;
}

bool
CGePlane::intersectWith(const CGeLinearCurve3d& linearCurve, CGePoint3d& resultPoint, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGePlane::intersectWith(const CGePlane& plane, CGeLine3d& resultLine, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGePlane::intersectWith(const CGeBoundedPlane& boundedPlane, CGeLineSegment3d& resultLineSegment, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGePlane&
CGePlane::set(const CGePoint3d& origin, const CGeVector3d& normal)
{
	return *this;
}

CGePlane&
CGePlane::set(const CGePoint3d& pointU, const CGePoint3d& origin, const CGePoint3d& pointV)
{
	return *this;
}

CGePlane&
CGePlane::set(double a, double b, double c, double d)
{
	return *this;
}

CGePlane&
CGePlane::set(const CGePoint3d& origin, const CGeVector3d& uAxis, const CGeVector3d& vAxis)
{
	return *this;
}

CGePlane&
CGePlane::operator = (const CGePlane& source)
{
	return *this;
}
