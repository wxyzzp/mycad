// File: GeSurface.cpp
//

#include "GeSurface.h"
#include "GePoint2d.h"

//////////////////////////////////////////////////
// CGeSurface class implementation
//////////////////////////////////////////////////

CGePoint2d
CGeSurface::paramOf(const CGePoint3d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return CGePoint2d();
}

bool
CGeSurface::isOn(const CGePoint3d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeSurface::isOn(const CGePoint3d& point, CGePoint2d& paramPoint, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGePoint3d
CGeSurface::closestPointTo(const CGePoint3d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return CGePoint3d();
}

void
CGeSurface::getClosestPointTo(const CGePoint3d& point, CGePointOnSurface& result, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
}

double
CGeSurface::distanceTo(const CGePoint3d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return 0.0;
}

bool
CGeSurface::isNormalReversed() const
{
	return false;
}

CGeSurface&
CGeSurface::reverseNormal()
{
	return *this;
}

void
CGeSurface::getEnvelope(CGeInterval& intervalX, CGeInterval& intervalY) const
{
}

bool
CGeSurface::isClosedInU(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeSurface::isClosedInV(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGePoint3d
CGeSurface::evalPoint(const CGePoint2d& paramPoint) const
{
	return CGePoint3d();
}

CGePoint3d
CGeSurface::evalPoint(const CGePoint2d& paramPoint, int derivOrder, CGeVector3dArray& derivatives) const
{
	return CGePoint3d();
}

CGePoint3d
CGeSurface::evalPoint(const CGePoint2d& paramPoint, int derivOrder, CGeVector3dArray& derivatives, CGeVector3d& normal) const
{
	return CGePoint3d();
}

CGeSurface&
CGeSurface::operator = (const CGeSurface& source)
{
	return *this;
}

CGeSurface::CGeSurface()
{
}

CGeSurface::CGeSurface(const CGeSurface& source)
{
}
