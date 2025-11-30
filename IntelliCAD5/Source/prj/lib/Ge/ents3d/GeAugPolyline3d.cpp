// File: GeAugPolyline3d.cpp
//

#include "GeAugPolyline3d.h"
#include "GeVector3d.h"

//////////////////////////////////////////////////
// CGeAugPolyline3d class implementation
//////////////////////////////////////////////////

CGeAugPolyline3d::CGeAugPolyline3d()
{
}

CGeAugPolyline3d::CGeAugPolyline3d(const CGeAugPolyline3d& source)
{
}

CGeAugPolyline3d::CGeAugPolyline3d(const CGeKnotVector& knots, const CGePoint3dArray& controlPoints, const CGeVector3dArray& vectors)
{
}

CGeAugPolyline3d::CGeAugPolyline3d(const CGePoint3dArray& controlPoints, const CGeVector3dArray& vectors)
{
}

CGeAugPolyline3d::CGeAugPolyline3d(const CGeCurve3d& curve, double fromParam, double toParam, double approxEps)
{
}

CGePoint3d
CGeAugPolyline3d::getPoint(int i) const
{
	return CGePoint3d();
}

CGeAugPolyline3d&
CGeAugPolyline3d::setPoint(int i, CGePoint3d point)
{
	return *this;
}

void
CGeAugPolyline3d::getPoints(CGePoint3dArray& points) const
{
}

CGeVector3d
CGeAugPolyline3d::getVector(int i) const
{
	return CGeVector3d();
}

CGeAugPolyline3d&
CGeAugPolyline3d::setVector(int i, CGeVector3d vector)
{
	return *this;
}

void
CGeAugPolyline3d::getD1Vectors(CGeVector3dArray& d1vectors) const
{
}

CGeVector3d
CGeAugPolyline3d::getD2Vector(int i) const
{
	return CGeVector3d();
}

CGeAugPolyline3d&
CGeAugPolyline3d::setD2Vector(int i, CGeVector3d d2vector)
{
	return *this;
}

void
CGeAugPolyline3d::getD2Vectors(CGeVector3dArray& d2vectors) const
{
}

double
CGeAugPolyline3d::approxTol() const
{
	return 0.0;
}

CGeAugPolyline3d&
CGeAugPolyline3d::setApproxTol(double approxTol)
{
	return *this;
}

CGeAugPolyline3d&
CGeAugPolyline3d::operator = (const CGeAugPolyline3d& source)
{
	return *this;
}
