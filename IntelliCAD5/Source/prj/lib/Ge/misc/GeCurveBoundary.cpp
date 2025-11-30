// File: GeCurveBoundary.cpp
//

#include "GeCurveBoundary.h"

//////////////////////////////////////////////////
// CGeCurveBoundary class implementation
//////////////////////////////////////////////////

CGeCurveBoundary::CGeCurveBoundary()
{
}

CGeCurveBoundary::CGeCurveBoundary(int numberOfCurves, const CGeEntity3d* const* curve3d, const CGeCurve2d* const* curve2d, bool* pbOrientation3d, bool* pbOrientation2d, bool bMakeCopy /* = true*/)
{
}

CGeCurveBoundary::CGeCurveBoundary(const CGeCurveBoundary& source)
{
}

CGeCurveBoundary::~CGeCurveBoundary()
{
}

bool
CGeCurveBoundary::isDegenerate() const
{
	return false;
}

bool
CGeCurveBoundary::isDegenerate(CGePosition3d& degenPoint, CGeCurve2d** paramCurve) const
{
	return false;
}

int
CGeCurveBoundary::numElements() const
{
	return m_numElements;
}

void
CGeCurveBoundary::getContour(int& n, CGeEntity3d*** curve3d, CGeCurve2d*** paramGeometry, bool** ppbOrientation3d, bool** ppbOrientation2d) const
{
}

CGeCurveBoundary&
CGeCurveBoundary::set(int numElements, const CGeEntity3d* const* curve3d, const CGeCurve2d* const* curve2d, bool* pbOrientation3d, bool* pbOrientation2d, bool bMakeCopy /* = true*/)
{
	return *this;
}

bool
CGeCurveBoundary::isOwnerOfCurves() const
{
	return false;
}

CGeCurveBoundary&
CGeCurveBoundary::setToOwnCurves()
{
	return *this;
}

CGeCurveBoundary&
CGeCurveBoundary::operator = (const CGeCurveBoundary& source)
{
	return *this;
}
