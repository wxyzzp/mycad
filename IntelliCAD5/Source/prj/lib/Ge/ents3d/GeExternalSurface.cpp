// File: GeExternalSurface.cpp
//

#include "GeExternalSurface.h"

//////////////////////////////////////////////////
// CGeExternalSurface class implementation
//////////////////////////////////////////////////

CGeExternalSurface::CGeExternalSurface()
{
}

CGeExternalSurface::CGeExternalSurface(void* pSurfaceDef, CGe::EExternalEntityKind surfaceKind, bool bMakeCopy /* = true*/)
{
}

CGeExternalSurface::CGeExternalSurface(const CGeExternalSurface& source)
{
}

void
CGeExternalSurface::getExternalSurface(void*& pSurfaceDef) const
{
}

CGe::EExternalEntityKind
CGeExternalSurface::externalSurfaceKind() const
{
	return CGe::eExternalEntityUndefined;
}

bool
CGeExternalSurface::isPlane() const
{
	return false;
}

bool
CGeExternalSurface::isSphere() const
{
	return false;
}

bool
CGeExternalSurface::isCylinder() const
{
	return false;
}

bool
CGeExternalSurface::isCone() const
{
	return false;
}

bool
CGeExternalSurface::isTorus() const
{
	return false;
}

bool
CGeExternalSurface::isNurbSurface() const
{
	return false;
}

bool
CGeExternalSurface::isDefined() const
{
	return false;
}

bool
CGeExternalSurface::isNativeSurface(CGeSurface*& pNativeSurface) const
{
	return false;
}

CGeExternalSurface&
CGeExternalSurface::set(void* pSurfaceDef, CGe::EExternalEntityKind surfaceKind, bool bMakeCopy /* = true*/)
{
	return *this;
}

bool
CGeExternalSurface::isOwnerOfSurface() const
{
	return false;
}

CGeExternalSurface&
CGeExternalSurface::setToOwnSurface()
{
	return *this;
}

CGeExternalSurface&
CGeExternalSurface::operator = (const CGeExternalSurface& source)
{
	return *this;
}
