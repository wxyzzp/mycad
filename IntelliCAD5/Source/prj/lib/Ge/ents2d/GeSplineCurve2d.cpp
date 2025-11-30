// File: GeSplineCurve2d.cpp
//

#include "GeSplineCurve2d.h"

//////////////////////////////////////////////////
// CGeSplineCurve2d class implementation
//////////////////////////////////////////////////

CGeSplineCurve2d::~CGeSplineCurve2d()
{
}

CGePoint2d
CGeSplineCurve2d::startPoint() const
{
	return CGePoint2d();
}

CGePoint2d
CGeSplineCurve2d::endPoint() const
{
	return CGePoint2d();
}

CGeSplineCurve2d&
CGeSplineCurve2d::setKnotAt(int i, double value)
{
	return *this;
}

CGePoint2d
CGeSplineCurve2d::controlPointAt(int i) const
{
	return CGePoint2d();
}

CGeSplineCurve2d&
CGeSplineCurve2d::setControlPointAt(int i, const CGePoint2d& point)
{
	return *this;
}

CGeSplineCurve2d&
CGeSplineCurve2d::operator = (const CGeSplineCurve2d& source)
{
	return *this;
}

CGeSplineCurve2d::CGeSplineCurve2d()
{
}

CGeSplineCurve2d::CGeSplineCurve2d(const CGeSplineCurve2d& source)
{
}
