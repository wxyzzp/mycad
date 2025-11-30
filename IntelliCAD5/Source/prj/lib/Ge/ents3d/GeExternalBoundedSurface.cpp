// File: GeExternalBoundedSurface.cpp
//

#include "GeExternalBoundedSurface.h"

//////////////////////////////////////////////////
// CGeExternalBoundedSurface class implementation
//////////////////////////////////////////////////

CGeExternalBoundedSurface::CGeExternalBoundedSurface()
{
}

CGeExternalBoundedSurface::CGeExternalBoundedSurface(void* pSurfaceDef, CGe::EExternalEntityKind surfaceKind, bool bMakeCopy /* = true*/)
{
}

CGeExternalBoundedSurface::CGeExternalBoundedSurface(const CGeExternalBoundedSurface& source)
{
}

CGe::EExternalEntityKind
CGeExternalBoundedSurface::externalSurfaceKind() const
{
	return CGe::eExternalEntityUndefined;
}

bool
CGeExternalBoundedSurface::isDefined() const
{
	return false;
}

void
CGeExternalBoundedSurface::getExternalSurface(void*& pSurfaceDef) const
{
}

void
CGeExternalBoundedSurface::getBaseSurface(CGeSurface*& pSurfaceDef) const
{
}

void
CGeExternalBoundedSurface::getBaseSurface(CGeExternalSurface& unboundedSurfaceDef) const
{
}

bool
CGeExternalBoundedSurface::isPlane() const
{
	return false;
}

bool
CGeExternalBoundedSurface::isSphere() const
{
	return false;
}

bool
CGeExternalBoundedSurface::isCylinder() const
{
	return false;
}

bool
CGeExternalBoundedSurface::isCone() const
{
	return false;
}

bool
CGeExternalBoundedSurface::isTorus() const
{
	return false;
}

bool
CGeExternalBoundedSurface::isNurbs() const
{
	return false;
}

bool
CGeExternalBoundedSurface::isExternalSurface() const
{
	return false;
}

int
CGeExternalBoundedSurface::numContours() const
{
	return 0;
}

void
CGeExternalBoundedSurface::getContours(int& numContours, CGeCurveBoundary*& pCurveBoundaries) const
{
}

CGeExternalBoundedSurface&
CGeExternalBoundedSurface::set(void* pSurfaceDef, CGe::EExternalEntityKind surfaceKind, bool bMakeCopy /* = true*/)
{
	return *this;
}

bool
CGeExternalBoundedSurface::isOwnerOfSurface() const
{
	return false;
}

CGeExternalBoundedSurface&
CGeExternalBoundedSurface::setToOwnSurface()
{
	return *this;
}

CGeExternalBoundedSurface&
CGeExternalBoundedSurface::operator = (const CGeExternalBoundedSurface& source)
{
	return *this;
}
