// File: GePointOnSurface.cpp
//

#include "GePointOnSurface.h"
#include "GeVector3d.h"
#include "GeVector2d.h"

//////////////////////////////////////////////////
// CGePointOnSurface class implementation
//////////////////////////////////////////////////

CGePointOnSurface::CGePointOnSurface()
{
}

CGePointOnSurface::CGePointOnSurface(const CGeSurface& surf)
{
}

CGePointOnSurface::CGePointOnSurface(const CGeSurface& surf, const CGePoint2d& paramPoint)
{
}

CGePointOnSurface::CGePointOnSurface(const CGePointOnSurface& source)
{
}

const CGeSurface*
CGePointOnSurface::surface() const
{
	return m_pSurface;
}

CGePoint2d
CGePointOnSurface::parameter() const
{
	return m_parameter;
}

CGePoint3d
CGePointOnSurface::point() const
{
	return m_point;
}

CGePoint3d
CGePointOnSurface::point(const CGePoint2d& paramPoint)
{
	return CGePoint3d();
}

CGePoint3d
CGePointOnSurface::point(const CGeSurface& surf, const CGePoint2d& paramPoint)
{
	return CGePoint3d();
}

CGeVector3d
CGePointOnSurface::normal() const
{
	return CGeVector3d();
}

CGeVector3d
CGePointOnSurface::normal(const CGePoint2d& paramPoint)
{
	return CGeVector3d();
}

CGeVector3d
CGePointOnSurface::normal(const CGeSurface& surf, const CGePoint2d& paramPoint)
{
	return CGeVector3d();
}

CGeVector3d
CGePointOnSurface::uDeriv(int order) const
{
	return CGeVector3d();
}

CGeVector3d
CGePointOnSurface::uDeriv(int order, const CGePoint2d& paramPoint)
{
	return CGeVector3d();
}

CGeVector3d
CGePointOnSurface::uDeriv(int order, const CGeSurface& surf, const CGePoint2d& paramPoint)
{
	return CGeVector3d();
}

CGeVector3d
CGePointOnSurface::vDeriv(int order) const
{
	return CGeVector3d();
}

CGeVector3d
CGePointOnSurface::vDeriv(int order, const CGePoint2d& paramPoint)
{
	return CGeVector3d();
}

CGeVector3d
CGePointOnSurface::vDeriv(int order, const CGeSurface& surf, const CGePoint2d& paramPoint)
{
	return CGeVector3d();
}

CGeVector3d
CGePointOnSurface::mixedPartial() const
{
	return CGeVector3d();
}

CGeVector3d
CGePointOnSurface::mixedPartial(const CGePoint2d& paramPoint)
{
	return CGeVector3d();
}

CGeVector3d
CGePointOnSurface::mixedPartial(const CGeSurface& surf, const CGePoint2d& paramPoint)
{
	return CGeVector3d();
}

CGeVector3d
CGePointOnSurface::tangentVector(const CGeVector2d& vector) const
{
	return CGeVector3d();
}

CGeVector3d
CGePointOnSurface::tangentVector(const CGeVector2d& vector, const CGePoint2d& paramPoint)
{
	return CGeVector3d();
}

CGeVector3d
CGePointOnSurface::tangentVector(const CGeVector2d& vector, const CGeSurface& surf, const CGePoint2d& paramPoint)
{
	return CGeVector3d();
}

CGeVector2d
CGePointOnSurface::inverseTangentVector(const CGeVector3d& vector) const
{
	return CGeVector2d();
}

CGeVector2d
CGePointOnSurface::inverseTangentVector(const CGeVector3d& vector, const CGePoint2d& paramPoint)
{
	return CGeVector2d();
}

CGeVector2d
CGePointOnSurface::inverseTangentVector(const CGeVector3d& vector, const CGeSurface& surf, const CGePoint2d& paramPoint)
{
	return CGeVector2d();
}

CGePointOnSurface&
CGePointOnSurface::setSurface(const CGeSurface& surf)
{
	return *this;
}

CGePointOnSurface&
CGePointOnSurface::setParameter(const CGePoint2d& paramPoint)
{
	m_parameter = paramPoint;
	return *this;
}

CGePointOnSurface&
CGePointOnSurface::operator = (const CGePointOnSurface& source)
{
	return *this;
}
