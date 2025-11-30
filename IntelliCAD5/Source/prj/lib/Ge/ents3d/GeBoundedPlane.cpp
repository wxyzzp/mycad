// File: GeBoundedPlane.cpp
//

#include "GeBoundedPlane.h"

//////////////////////////////////////////////////
// CGeBoundedPlane class implementation
//////////////////////////////////////////////////

CGeBoundedPlane::CGeBoundedPlane()
{
}

CGeBoundedPlane::CGeBoundedPlane(const CGeBoundedPlane& source)
{
}

CGeBoundedPlane::CGeBoundedPlane(const CGePoint3d& origin, const CGeVector3d& uVector, const CGeVector3d& vVector)
{
}

CGeBoundedPlane::CGeBoundedPlane(const CGePoint3d& point1, const CGePoint3d& origin, const CGePoint3d& point2)
{
}

bool
CGeBoundedPlane::intersectWith(const CGeLinearCurve3d& linearCurve, CGePoint3d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeBoundedPlane::intersectWith(const CGePlane& plane, CGeLineSegment3d& results, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeBoundedPlane::intersectWith(const CGeBoundedPlane& plane, CGeLineSegment3d& result, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGeBoundedPlane&
CGeBoundedPlane::set(const CGePoint3d& origin, const CGeVector3d& uVector, const CGeVector3d& vVector)
{
	return *this;
}

CGeBoundedPlane&
CGeBoundedPlane::set(const CGePoint3d& point1, const CGePoint3d& origin, const CGePoint3d& point2)
{
	return *this;
}

CGeBoundedPlane&
CGeBoundedPlane::operator = (const CGeBoundedPlane& source)
{
	return *this;
}
