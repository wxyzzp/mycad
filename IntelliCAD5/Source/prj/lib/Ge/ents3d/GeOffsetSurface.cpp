// File: GeOffsetSurface.cpp
//

#include "GeOffsetSurface.h"

//////////////////////////////////////////////////
// CGeOffsetSurface class implementation
//////////////////////////////////////////////////

CGeOffsetSurface::CGeOffsetSurface()
{
}

CGeOffsetSurface::CGeOffsetSurface(CGeSurface* pBaseSurface, double offsetDist, bool bMakeCopy /* = true*/)
{
}

CGeOffsetSurface::CGeOffsetSurface(const CGeOffsetSurface& source)
{
}

bool
CGeOffsetSurface::isPlane() const
{
	return false;
}

bool
CGeOffsetSurface::isBoundedPlane() const
{
	return false;
}

bool
CGeOffsetSurface::isSphere() const
{
	return false;
}

bool
CGeOffsetSurface::isCylinder() const
{
	return false;
}

bool
CGeOffsetSurface::isCone() const
{
	return false;
}

bool
CGeOffsetSurface::isTorus() const
{
	return false;
}

bool
CGeOffsetSurface::getSurface(CGeSurface*& pSurf) const
{
	return false;
}

void
CGeOffsetSurface::getConstructionSurface(CGeSurface*& pBaseSurf) const
{
	pBaseSurf = m_pSurface;
}

double
CGeOffsetSurface::offsetDist() const
{
	return m_offset;
}

CGeOffsetSurface&
CGeOffsetSurface::set(CGeSurface* pBaseSurf, double offsetDist, bool bMakeCopy /* = true*/)
{
	return *this;
}

CGeOffsetSurface&
CGeOffsetSurface::operator = (const CGeOffsetSurface& offset)
{
	return *this;
}
