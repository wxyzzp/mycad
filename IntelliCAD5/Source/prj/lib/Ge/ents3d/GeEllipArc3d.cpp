// File: GeEllipArc3d.cpp
//

#include "GeEllipArc3d.h"
#include "GeVector3d.h"

//////////////////////////////////////////////////
// CGeEllipArc3d class implementation
//////////////////////////////////////////////////

CGeEllipArc3d::CGeEllipArc3d()
{
}

CGeEllipArc3d::CGeEllipArc3d(const CGeEllipArc3d& source)
{
}

CGeEllipArc3d::CGeEllipArc3d(const CGeCircArc3d& sourceCircArc)
{
}

CGeEllipArc3d::CGeEllipArc3d(const CGePoint3d& center, const CGeVector3d& majorAxis, const CGeVector3d& minorAxis, double majorRadius, double minorRadius)
{
}

CGeEllipArc3d::CGeEllipArc3d(const CGePoint3d& center, const CGeVector3d& majorAxis, const CGeVector3d& minorAxis, double majorRadius, double minorRadius, double startAngle, double endAngle)
{
}

bool
CGeEllipArc3d::intersectWith(const CGeLinearCurve3d& linearCurve, int& intNum, CGePoint3d& point1, CGePoint3d& point2, const CGeTolerance& /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeEllipArc3d::intersectWith(const CGePlanarSurface& planarSurface, int& intNum, CGePoint3d& point1, CGePoint3d& point2, const CGeTolerance& /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeEllipArc3d::projIntersectWith(const CGeLinearCurve3d& linearCurve, const CGeVector3d& projDir, int &numInt, CGePoint3d& pointOnArc1, CGePoint3d& pointOnArc2, CGePoint3d& pointOnLine1, CGePoint3d& pointOnLine2, const CGeTolerance& /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGePoint3d
CGeEllipArc3d::closestPointToPlane(const CGePlanarSurface& planarSurface, CGePoint3d& pointOnPlane, const CGeTolerance& /* = CGeContext::s_defTol*/) const
{
	return CGePoint3d();
}

bool
CGeEllipArc3d::isCircular(const CGeTolerance& /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeEllipArc3d::isInside(const CGePoint3d& point, const CGeTolerance& /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGePoint3d
CGeEllipArc3d::center() const
{
	return CGePoint3d();
}

double
CGeEllipArc3d::minorRadius() const
{
	return 0.0;
}

double
CGeEllipArc3d::majorRadius() const
{
	return 0.0;
}

CGeVector3d
CGeEllipArc3d::minorAxis() const
{
	return CGeVector3d();
}

CGeVector3d
CGeEllipArc3d::majorAxis() const
{
	return CGeVector3d();
}

CGeVector3d
CGeEllipArc3d::normal() const
{
	return CGeVector3d();
}

void
CGeEllipArc3d::getPlane(CGePlane& plane) const
{
}

double
CGeEllipArc3d::startAng() const
{
	return 0.0;
}

double
CGeEllipArc3d::endAng() const
{
	return 0.0;
}

CGePoint3d
CGeEllipArc3d::startPoint() const
{
	return CGePoint3d();
}

CGePoint3d
CGeEllipArc3d::endPoint() const
{
	return CGePoint3d();
}

CGeEllipArc3d&
CGeEllipArc3d::setCenter(const CGePoint3d& center)
{
	return *this;
}

CGeEllipArc3d&
CGeEllipArc3d::setMinorRadius(double radius)
{
	return *this;
}

CGeEllipArc3d&
CGeEllipArc3d::setMajorRadius(double radius)
{
	return *this;
}

CGeEllipArc3d&
CGeEllipArc3d::setAxes(const CGeVector3d& majorAxis, const CGeVector3d& minorAxis)
{
	return *this;
}

CGeEllipArc3d&
CGeEllipArc3d::setAngles(double startAngle, double endAngle)
{
	return *this;
}

CGeEllipArc3d&
CGeEllipArc3d::set(const CGePoint3d& center, const CGeVector3d& majorAxis, const CGeVector3d& minorAxis, double majorRadius, double minorRadius)
{
	return *this;
}

CGeEllipArc3d&
CGeEllipArc3d::set(const CGePoint3d& center, const CGeVector3d& majorAxis, const CGeVector3d& minorAxis, double majorRadius, double minorRadius, double startAngle, double endAngle)
{
	return *this;
}

CGeEllipArc3d&
CGeEllipArc3d::set(const CGeCircArc3d& sourceCircArc)
{
	return *this;
}

CGeEllipArc3d&
CGeEllipArc3d::operator = (const CGeEllipArc3d& source)
{
	return *this;
}
