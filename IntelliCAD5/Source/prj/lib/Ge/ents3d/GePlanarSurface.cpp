// File: GePlanarSurface.cpp
//

#include "GePlanarSurface.h"

//////////////////////////////////////////////////
// CGePlanarSurface class implementation
//////////////////////////////////////////////////

bool
CGePlanarSurface::intersectWith(const CGeLinearCurve3d& linearCurve, CGePoint3d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGePoint3d
CGePlanarSurface::closestPointToLinearEnt(const CGeLinearCurve3d& linearCurve, CGePoint3d& pointOnCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return CGePoint3d();
}

CGePoint3d
CGePlanarSurface::closestPointToPlanarEnt(const CGePlanarSurface& planarSurf, CGePoint3d& pointOnOtherPlanarSurf, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return CGePoint3d();
}

bool
CGePlanarSurface::isParallelTo(const CGeLinearCurve3d& linearCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGePlanarSurface::isParallelTo(const CGePlanarSurface& planarSurf, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGePlanarSurface::isPerpendicularTo(const CGeLinearCurve3d& linearCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGePlanarSurface::isPerpendicularTo(const CGePlanarSurface& planarSurf, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGePlanarSurface::isCoplanarTo(const CGePlanarSurface& planarSurf, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

void
CGePlanarSurface::get(CGePoint3d& origin, CGeVector3d& uVector, CGeVector3d& vVector) const
{
}

void
CGePlanarSurface::get(CGePoint3d& point1, CGePoint3d& origin, CGePoint3d& point2) const
{
}

CGePoint3d
CGePlanarSurface::pointOnPlane() const
{
	return CGePoint3d();
}

CGeVector3d
CGePlanarSurface::normal() const
{
	return m_normal;
}

void
CGePlanarSurface::getCoefficients(double& a, double& b, double& c, double& d) const
{
}

void
CGePlanarSurface::getCoordSystem(CGePoint3d& origin, CGeVector3d& axis1, CGeVector3d& axis2) const
{
}

CGePlanarSurface&
CGePlanarSurface::operator = (const CGePlanarSurface& source)
{
	return *this;
}

CGePlanarSurface::CGePlanarSurface()
{
}

CGePlanarSurface::CGePlanarSurface(const CGePlanarSurface& source)
{
}
