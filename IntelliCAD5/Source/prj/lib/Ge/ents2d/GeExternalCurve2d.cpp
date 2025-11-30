// File: GeExternalCurve2d.cpp
//

#include "GeExternalCurve2d.h"

//////////////////////////////////////////////////
// CGeExternalCurve2d class implementation
//////////////////////////////////////////////////

CGeExternalCurve2d::CGeExternalCurve2d()
{
}

CGeExternalCurve2d::CGeExternalCurve2d(const CGeExternalCurve2d& source)
{
}

CGeExternalCurve2d::CGeExternalCurve2d(void* pCurveDef, CGe::EExternalEntityKind curveKind, bool bMakeCopy /* = true*/)
{
}

bool
CGeExternalCurve2d::isNurbCurve() const
{
	return false;
}

bool
CGeExternalCurve2d::isNurbCurve(CGeNurbCurve2d& nurbCurve) const
{
	return false;
}

bool
CGeExternalCurve2d::isDefined() const
{
	return false;
}

void
CGeExternalCurve2d::getExternalCurve(void*& pCurveDef) const
{
}

CGe::EExternalEntityKind
CGeExternalCurve2d::externalCurveKind() const
{
	return CGe::eExternalEntityUndefined;
}

CGeExternalCurve2d&
CGeExternalCurve2d::set(void* pCurveDef, CGe::EExternalEntityKind curveKind, bool bMakeCopy /* = true*/)
{
	return *this;
}

bool
CGeExternalCurve2d::isOwnerOfCurve() const
{
	return false;
}

CGeExternalCurve2d&
CGeExternalCurve2d::setToOwnCurve()
{
	return *this;
}

CGeExternalCurve2d&
CGeExternalCurve2d::operator = (const CGeExternalCurve2d& source)
{
	return *this;
}
