// File: GeEllipArc2d.cpp
//

#include "GeEllipArc2d.h"
#include "GeVector2d.h"

//////////////////////////////////////////////////
// CGeEllipArc2d class implementation
//////////////////////////////////////////////////

CGeEllipArc2d::CGeEllipArc2d()
{
}

CGeEllipArc2d::CGeEllipArc2d(const CGeEllipArc2d& source)
{
}

CGeEllipArc2d::CGeEllipArc2d(const CGeCircArc2d& sourceCircArc)
{
}

CGeEllipArc2d::CGeEllipArc2d(const CGePoint2d& center, const CGeVector2d& majorAxis, const CGeVector2d& minorAxis, double majorRadius, double minorRadius)
{
}

CGeEllipArc2d::CGeEllipArc2d(const CGePoint2d& center, const CGeVector2d& majorAxis, const CGeVector2d& minorAxis, double majorRadius, double minorRadius, double startAngle, double endAngle)
{
}

bool
CGeEllipArc2d::intersectWith(const CGeLinearCurve2d& linearCurve, int& intNum, CGePoint2d& point1, CGePoint2d& point2, const CGeTolerance& /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeEllipArc2d::isCircular(const CGeTolerance& /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeEllipArc2d::isClockWise() const
{
	return false;
}

bool
CGeEllipArc2d::isInside(const CGePoint2d& point, const CGeTolerance& /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGePoint2d
CGeEllipArc2d::center() const
{
	return CGePoint2d();
}

double
CGeEllipArc2d::minorRadius() const
{
	return 0.0;
}

double
CGeEllipArc2d::majorRadius() const
{
	return 0.0;
}

CGeVector2d
CGeEllipArc2d::minorAxis() const
{
	return CGeVector2d();
}

CGeVector2d
CGeEllipArc2d::majorAxis() const
{
	return CGeVector2d();
}

CGeVector2d
CGeEllipArc2d::normal() const
{
	return CGeVector2d();
}

double
CGeEllipArc2d::startAng() const
{
	return 0.0;
}

double
CGeEllipArc2d::endAng() const
{
	return 0.0;
}

CGePoint2d
CGeEllipArc2d::startPoint() const
{
	return CGePoint2d();
}

CGePoint2d
CGeEllipArc2d::endPoint() const
{
	return CGePoint2d();
}

CGeEllipArc2d&
CGeEllipArc2d::setCenter(const CGePoint2d& center)
{
	return *this;
}

CGeEllipArc2d&
CGeEllipArc2d::setMinorRadius(double radius)
{
	return *this;
}

CGeEllipArc2d&
CGeEllipArc2d::setMajorRadius(double radius)
{
	return *this;
}

CGeEllipArc2d&
CGeEllipArc2d::setAxes(const CGeVector2d& majorAxis, const CGeVector2d& minorAxis)
{
	return *this;
}

CGeEllipArc2d&
CGeEllipArc2d::setAngles(double startAngle, double endAngle)
{
	return *this;
}

CGeEllipArc2d&
CGeEllipArc2d::set(const CGePoint2d& center, const CGeVector2d& majorAxis, const CGeVector2d& minorAxis, double majorRadius, double minorRadius)
{
	return *this;
}

CGeEllipArc2d&
CGeEllipArc2d::set(const CGePoint2d& center, const CGeVector2d& majorAxis, const CGeVector2d& minorAxis, double majorRadius, double minorRadius, double startAngle, double endAngle)
{
	return *this;
}

CGeEllipArc2d&
CGeEllipArc2d::set(const CGeCircArc2d& sourceCircArc)
{
	return *this;
}

CGeEllipArc2d&
CGeEllipArc2d::operator = (const CGeEllipArc2d& source)
{
	return *this;
}
