// File: GeLineSegment2d.cpp
//

#include "GeLineSegment2d.h"

//////////////////////////////////////////////////
// CGeLineSegment2d class implementation
//////////////////////////////////////////////////

CGeLineSegment2d::CGeLineSegment2d()
{
}

CGeLineSegment2d::CGeLineSegment2d(const CGeLineSegment2d& source)
{
}

CGeLineSegment2d::CGeLineSegment2d(const CGePoint2d& point, const CGeVector2d& vector)
{
}

CGeLineSegment2d::CGeLineSegment2d(const CGePoint2d& point1, const CGePoint2d& point2)
{
}

CGePoint2d
CGeLineSegment2d::baryComb(double blendCoeff) const
{
	return CGePoint2d();
}

CGePoint2d
CGeLineSegment2d::startPoint() const
{
	return CGePoint2d();
}

CGePoint2d
CGeLineSegment2d::midPoint() const
{
	return CGePoint2d();
}

CGePoint2d
CGeLineSegment2d::endPoint() const
{
	return CGePoint2d();
}

double
CGeLineSegment2d::length() const
{
	return 0.0;
}

double
CGeLineSegment2d::length(double fromParam, double toParam, double tol /* = CGeContext::s_defTol.equalPoint()*/) const
{
	return 0.0;
}

CGeLineSegment2d&
CGeLineSegment2d::set(const CGePoint2d& point, const CGeVector2d& vector)
{
	return *this;
}

CGeLineSegment2d&
CGeLineSegment2d::set(const CGePoint2d& point1, const CGePoint2d& point2)
{
	return *this;
}

CGeLineSegment2d&
CGeLineSegment2d::set(const CGeCurve2d& curve1, const CGeCurve2d& curve2, double& param1, double& param2, bool& bSuccess)
{
	return *this;
}

CGeLineSegment2d&
CGeLineSegment2d::set(const CGeCurve2d& curve, const CGePoint2d& point, double& param, bool& bSuccess)
{
	return *this;
}

void
CGeLineSegment2d::getBisector(CGeLine2d& line) const
{
}

CGeLineSegment2d&
CGeLineSegment2d::operator = (const CGeLineSegment2d& source)
{
	return *this;
}
