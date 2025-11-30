// File: GeCompositeCurve2d.cpp
//

#include "GeCompositeCurve2d.h"

//////////////////////////////////////////////////
// CGeCompositeCurve2d class implementation
//////////////////////////////////////////////////

CGeCompositeCurve2d::CGeCompositeCurve2d()
{
}

CGeCompositeCurve2d::CGeCompositeCurve2d(const CGeVoidPointerArray& curveList)
{
}

CGeCompositeCurve2d::CGeCompositeCurve2d(const CGeVoidPointerArray& curveList, const CGeIntArray& isOwnerOfCurves)
{
}

CGeCompositeCurve2d::CGeCompositeCurve2d(const CGeCompositeCurve2d& source)
{
}

void
CGeCompositeCurve2d::getCurveList(CGeVoidPointerArray& curveList) const
{
}

CGeCompositeCurve2d&
CGeCompositeCurve2d::setCurveList(const CGeVoidPointerArray& curveList)
{
	return *this;
}

CGeCompositeCurve2d&
CGeCompositeCurve2d::setCurveList(const CGeVoidPointerArray& curveList, const CGeIntArray& isOwnerOfCurves)
{
	return *this;
}

double
CGeCompositeCurve2d::globalToLocalParam(double param, int& segNum) const
{
	return 0.0;
}

double
CGeCompositeCurve2d::localToGlobalParam(double param, int segNum) const
{
	return 0.0;
}

CGeCompositeCurve2d&
CGeCompositeCurve2d::operator = (const CGeCompositeCurve2d& source)
{
	return *this;
}
