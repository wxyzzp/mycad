// File: GePolyline2d.cpp
//

#include "GePolyline2d.h"

//////////////////////////////////////////////////
// CGePolyline2d class implementation
//////////////////////////////////////////////////

CGePolyline2d::CGePolyline2d()
{
}

CGePolyline2d::CGePolyline2d(const CGePolyline2d& source)
{
}

CGePolyline2d::CGePolyline2d(const CGePoint2dArray& points)
{
}

CGePolyline2d::CGePolyline2d(const CGeKnotVector& knots, const CGePoint2dArray& points)
{
}

CGePolyline2d::CGePolyline2d(const CGeCurve2d& curve, double approxEps)
{
}

int
CGePolyline2d::numFitPoints() const
{
	return 0;
}

CGePoint2d
CGePolyline2d::fitPointAt(int i) const
{
	return CGePoint2d();
}

CGeSplineCurve2d&
CGePolyline2d::setFitPointAt(int i, const CGePoint2d& point)
{
	return *static_cast<CGeSplineCurve2d*>(this);
}

CGePolyline2d&
CGePolyline2d::operator = (const CGePolyline2d& source)
{
	return *this;
}
