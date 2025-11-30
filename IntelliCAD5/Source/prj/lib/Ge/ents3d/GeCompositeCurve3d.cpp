// File: GeCompositeCurve3d.cpp
//

#include "GeCompositeCurve3d.h"

//////////////////////////////////////////////////
// CGeCompositeCurve3d class implementation
//////////////////////////////////////////////////

CGeCompositeCurve3d::CGeCompositeCurve3d()
{
}

CGeCompositeCurve3d::CGeCompositeCurve3d(const CGeVoidPointerArray& curveList)
{
}

CGeCompositeCurve3d::CGeCompositeCurve3d(const CGeVoidPointerArray& curveList, const CGeIntArray& isOwnerOfCurves)
{
}

CGeCompositeCurve3d::CGeCompositeCurve3d(const CGeCompositeCurve3d& source)
{
}

void
CGeCompositeCurve3d::getCurveList(CGeVoidPointerArray& curveList) const
{
}

CGeCompositeCurve3d&
CGeCompositeCurve3d::setCurveList(const CGeVoidPointerArray& curveList)
{
	return *this;
}

CGeCompositeCurve3d&
CGeCompositeCurve3d::setCurveList(const CGeVoidPointerArray& curveList, const CGeIntArray& isOwnerOfCurves)
{
	return *this;
}

double
CGeCompositeCurve3d::globalToLocalParam(double param, int& segNum) const
{
	return 0.0;
}

double
CGeCompositeCurve3d::localToGlobalParam(double param, int segNum) const
{
	return 0.0;
}

CGeCompositeCurve3d&
CGeCompositeCurve3d::operator = (const CGeCompositeCurve3d& source)
{
	return *this;
}
