// File: GeSplineCurve3d.cpp
//

#include "GeSplineCurve3d.h"

//////////////////////////////////////////////////
// CGeSplineCurve3d class implementation
//////////////////////////////////////////////////

CGeSplineCurve3d::~CGeSplineCurve3d()
{
}

CGePoint3d
CGeSplineCurve3d::startPoint() const
{
	return CGePoint3d();
}

CGePoint3d
CGeSplineCurve3d::endPoint() const
{
	return CGePoint3d();
}

CGeSplineCurve3d&
CGeSplineCurve3d::setKnotAt(int i, double value)
{
	return *this;
}

CGePoint3d
CGeSplineCurve3d::controlPointAt(int i) const
{
	return CGePoint3d();
}

CGeSplineCurve3d&
CGeSplineCurve3d::setControlPointAt(int i, const CGePoint3d& point)
{
	return *this;
}

CGeSplineCurve3d&
CGeSplineCurve3d::operator = (const CGeSplineCurve3d& source)
{
	return *this;
}

CGeSplineCurve3d::CGeSplineCurve3d()
{
}

CGeSplineCurve3d::CGeSplineCurve3d(const CGeSplineCurve3d& source)
{
}
