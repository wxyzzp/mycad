// File: GeSurfaceSurfaceInt.cpp
//

#include "GeSurfaceSurfaceInt.h"
#include "GeCurve2d.h"
#include "GeCurve3d.h"

//////////////////////////////////////////////////
// CGeSurfaceSurfaceInt class implementation
//////////////////////////////////////////////////

CGeSurfaceSurfaceInt::CGeSurfaceSurfaceInt()
{
}

CGeSurfaceSurfaceInt::CGeSurfaceSurfaceInt(const CGeSurface& surf1, const CGeSurface& surf2, const CGeTolerance& tol /* = CGeContext::s_defTol*/)
{
}

CGeSurfaceSurfaceInt::CGeSurfaceSurfaceInt(const CGeSurfaceSurfaceInt& source)
{
}

const CGeSurface*
CGeSurfaceSurfaceInt::surface1() const
{
	return m_pSurface1;
}

const CGeSurface*
CGeSurfaceSurfaceInt::surface2() const
{
	return m_pSurface1;
}

CGeTolerance
CGeSurfaceSurfaceInt::tolerance() const
{
	return m_tolerance;
}

int
CGeSurfaceSurfaceInt::numResults(CGe::EGeIntersectError& error) const
{
	return 0;
}

CGeCurve3d*
CGeSurfaceSurfaceInt::intCurve(int intNum, bool bIsExternal, CGe::EGeIntersectError& error) const
{
	return 0;
}

CGeCurve2d*
CGeSurfaceSurfaceInt::intParamCurve(int num, bool bIsExternall, bool bIsFirst, CGe::EGeIntersectError& error) const
{
	return 0;
}

CGePoint3d
CGeSurfaceSurfaceInt::intPoint(int intNum, CGe::EGeIntersectError& error) const
{
	return CGePoint3d();
}

void
CGeSurfaceSurfaceInt::getIntPointParams(int intNum, CGePoint2d& paramPoint1, CGePoint2d& paramPoint2, CGe::EGeIntersectError& error) const
{
}

void
CGeSurfaceSurfaceInt::getIntConfigs(int intNum, CGe::EssiConfig& surf1Left, CGe::EssiConfig& surf1Right, CGe::EssiConfig& surf2Left, CGe::EssiConfig& surf2Right, CGe::EssiType& intType, int& dim, CGe::EGeIntersectError& error) const
{
}

int
CGeSurfaceSurfaceInt::getDimension(int intNum, CGe::EGeIntersectError& error) const
{
	return 0;
}

CGe::EssiType
CGeSurfaceSurfaceInt::getType(int intNum, CGe::EGeIntersectError& error) const
{
	return CGe::eSSITransverse;
}

CGeSurfaceSurfaceInt&
CGeSurfaceSurfaceInt::set(const CGeSurface& surf1, const CGeSurface& surf2, const CGeTolerance& tol /* = CGeContext::s_defTol*/)
{
	return *this;
}

CGeSurfaceSurfaceInt&
CGeSurfaceSurfaceInt::operator = (const CGeSurfaceSurfaceInt& source)
{
	return *this;
}
