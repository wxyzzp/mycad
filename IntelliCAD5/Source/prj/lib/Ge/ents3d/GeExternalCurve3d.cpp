// File: GeExternalCurve3d.cpp
//

#include "GeExternalCurve3d.h"

//////////////////////////////////////////////////
// CGeExternalCurve3d class implementation
//////////////////////////////////////////////////

CGeExternalCurve3d::CGeExternalCurve3d()
{
}

CGeExternalCurve3d::CGeExternalCurve3d(const CGeExternalCurve3d& source)
{
}

CGeExternalCurve3d::CGeExternalCurve3d(void* pCurveDef, CGe::EExternalEntityKind curveKind, bool bMakeCopy /* = true*/)
{
}

bool
CGeExternalCurve3d::isLine() const
{
	return false;
}

bool
CGeExternalCurve3d::isRay() const
{
	return false;
}

bool
CGeExternalCurve3d::isLineSeg() const
{
	return false;
}

bool
CGeExternalCurve3d::isCircArc() const
{
	return false;
}

bool
CGeExternalCurve3d::isEllipArc() const
{
	return false;
}

bool
CGeExternalCurve3d::isNativeCurve(CGeCurve3d*& pNativeCurve) const
{
	return false;
}

bool
CGeExternalCurve3d::isNurbCurve() const
{
	return false;
}

bool
CGeExternalCurve3d::isDefined() const
{
	return false;
}

void
CGeExternalCurve3d::getExternalCurve(void*& pCurveDef) const
{
}

CGe::EExternalEntityKind
CGeExternalCurve3d::externalCurveKind() const
{
	return CGe::eExternalEntityUndefined;
}

CGeExternalCurve3d&
CGeExternalCurve3d::set(void* pCurveDef, CGe::EExternalEntityKind curveKind, bool bMakeCopy /* = true*/)
{
	return *this;
}

bool
CGeExternalCurve3d::isOwnerOfCurve() const
{
	return false;
}

CGeExternalCurve3d&
CGeExternalCurve3d::setToOwnCurve()
{
	return *this;
}

CGeExternalCurve3d&
CGeExternalCurve3d::operator = (const CGeExternalCurve3d& source)
{
	return *this;
}
