// File: GePolyline3d.cpp
//

#include "GePolyline3d.h"

//////////////////////////////////////////////////
// CGePolyline3d class implementation
//////////////////////////////////////////////////

CGePolyline3d::CGePolyline3d()
{
}

CGePolyline3d::CGePolyline3d(const CGePolyline3d& source)
{
}

CGePolyline3d::CGePolyline3d(const CGePoint3dArray& points)
{
}

CGePolyline3d::CGePolyline3d(const CGeKnotVector& knots, const CGePoint3dArray& controlPoints)
{
}

CGePolyline3d::CGePolyline3d(const CGeCurve3d& curve, double approxEps)
{
}

int
CGePolyline3d::numFitPoints() const
{
	return 0;
}

CGePoint3d
CGePolyline3d::fitPointAt(int i) const
{
	return CGePoint3d();
}

CGeSplineCurve3d&
CGePolyline3d::setFitPointAt(int i, const CGePoint3d& point)
{
	return *this;
}

CGePolyline3d&
CGePolyline3d::operator = (const CGePolyline3d& source)
{
	return *this;
}
