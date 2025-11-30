// File: GeLineSegment3d.cpp
//

#include "GeLineSegment3d.h"

//////////////////////////////////////////////////
// CGeLineSegment3d class implementation
//////////////////////////////////////////////////

CGeLineSegment3d::CGeLineSegment3d()
{
}

CGeLineSegment3d::CGeLineSegment3d(const CGeLineSegment3d& source)
{
}

CGeLineSegment3d::CGeLineSegment3d(const CGePoint3d& point, const CGeVector3d& vector)
{
}

CGeLineSegment3d::CGeLineSegment3d(const CGePoint3d& point1, const CGePoint3d& point2)
{
}

CGePoint3d
CGeLineSegment3d::baryComb(double blendCoeff) const
{
	return CGePoint3d();
}

CGePoint3d
CGeLineSegment3d::startPoint() const
{
	return CGePoint3d();
}

CGePoint3d
CGeLineSegment3d::midPoint() const
{
	return CGePoint3d();
}

CGePoint3d
CGeLineSegment3d::endPoint() const
{
	return CGePoint3d();
}

double
CGeLineSegment3d::length() const
{
	return 0.0;
}

double
CGeLineSegment3d::length(double fromParam, double toParam, double tol /* = CGeContext::s_defTol.equalPoint()*/) const
{
	return 0.0;
}

CGeLineSegment3d&
CGeLineSegment3d::set(const CGePoint3d& point, const CGeVector3d& vector)
{
	return *this;
}

CGeLineSegment3d&
CGeLineSegment3d::set(const CGePoint3d& point1, const CGePoint3d& point2)
{
	return *this;
}

CGeLineSegment3d&
CGeLineSegment3d::set(const CGeCurve3d& curve1, const CGeCurve3d& curve2, double& param1, double& param2, bool& bSuccess)
{
	return *this;
}

CGeLineSegment3d&
CGeLineSegment3d::set(const CGeCurve3d& curve, const CGePoint3d& point, double& param, bool& bSuccess)
{
	return *this;
}

void
CGeLineSegment3d::getBisector(CGePlane& plane) const
{
}

CGeLineSegment3d&
CGeLineSegment3d::operator = (const CGeLineSegment3d& source)
{
	return *this;
}
