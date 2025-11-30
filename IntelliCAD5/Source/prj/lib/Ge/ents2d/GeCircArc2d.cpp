// File: GeCircArc2d.cpp
//

#include "GeCircArc2d.h"

//////////////////////////////////////////////////
// CGeCircArc2d class implementation
//////////////////////////////////////////////////

CGeCircArc2d::CGeCircArc2d()
{
}

CGeCircArc2d::CGeCircArc2d(const CGeCircArc2d& source)
{
}

CGeCircArc2d::CGeCircArc2d(const CGePoint2d& center, double radius)
{
}

CGeCircArc2d::CGeCircArc2d(const CGePoint2d& startPoint, const CGePoint2d& pointOnArc, const CGePoint2d& endPoint)
{
}

CGeCircArc2d::CGeCircArc2d(const CGePoint2d& center, double radius, double startAngle, double endAngle, const CGeVector2d& refVec /* = CGeVector2d::s_xAxis*/, bool bIsClockWise /* = false*/)
{
}

CGeCircArc2d::CGeCircArc2d(const CGePoint2d& startPoint, const CGePoint2d& endPoint, double bulge, bool bBulgeFlag /* = true*/)
{
}

bool
CGeCircArc2d::intersectWith(const CGeLinearCurve2d& linearCurve, int& intNum, CGePoint2d& point1, CGePoint2d& point2, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCircArc2d::intersectWith(const CGeCircArc2d& arc, int& intNum, CGePoint2d& point1, CGePoint2d& point2, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCircArc2d::tangent(const CGePoint2d& point, CGeLine2d& line, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCircArc2d::tangent(const CGePoint2d& point, CGeLine2d& line, const CGeTolerance& tol, CGeError& error) const
{
	return false;
}

bool
CGeCircArc2d::isInside(const CGePoint2d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGePoint2d
CGeCircArc2d::center() const
{
	return m_center;
}

CGeVector2d
CGeCircArc2d::refVec() const
{
	return m_refVector;
}

double
CGeCircArc2d::radius() const
{
	return m_radius;
}

double
CGeCircArc2d::startAng() const
{
	return m_startAngle;
}

double
CGeCircArc2d::endAng() const
{
	return m_endAngle;
}

CGePoint2d
CGeCircArc2d::startPoint() const
{
	return CGePoint2d();
}

CGePoint2d
CGeCircArc2d::endPoint() const
{
	return CGePoint2d();
}

bool
CGeCircArc2d::isClockWise() const
{
	return m_bIsClockWise;
}

CGeCircArc2d&
CGeCircArc2d::setCenter(const CGePoint2d& center)
{
	m_center = center;
	return *this;
}

CGeCircArc2d&
CGeCircArc2d::setRadius(double radius)
{
	m_radius = radius;
	return *this;
}

CGeCircArc2d&
CGeCircArc2d::setAngles(double startAngle, double endAngle)
{
	m_startAngle = startAngle;
	m_endAngle = endAngle;
	return *this;
}

CGeCircArc2d&
CGeCircArc2d::setRefVec(const CGeVector2d& vector)
{
	m_refVector = vector;
	return *this;
}

CGeCircArc2d&
CGeCircArc2d::setToComplement()
{
	return *this;
}

CGeCircArc2d&
CGeCircArc2d::set(const CGePoint2d& startPoint, const CGePoint2d& pointOnArc, const CGePoint2d& endPoint)
{
	return *this;
}

CGeCircArc2d&
CGeCircArc2d::set(const CGePoint2d& startPoint, const CGePoint2d& pointOnArc, const CGePoint2d& endPoint, CGeError& error)
{
	return *this;
}

CGeCircArc2d&
CGeCircArc2d::set(const CGeCurve2d& curve1, const CGeCurve2d& curve2, double radius, double& param1, double& param2, bool& bSuccess)
{
	return *this;
}

CGeCircArc2d&
CGeCircArc2d::set(const CGeCurve2d& curve1, const CGeCurve2d& curve2, const CGeCurve2d& curve3, double& param1, double& param2, double& param3, bool& bSuccess)
{
	return *this;
}

CGeCircArc2d&
CGeCircArc2d::set(const CGePoint2d& center, double radius)
{
	return *this;
}

CGeCircArc2d&
CGeCircArc2d::set(const CGePoint2d& center, double radius, double startAngle, double endAngle, const CGeVector2d& refVec /* = CGeVector2d::s_xAxis*/, bool bIsClockWise /* = false*/)
{
	return *this;
}

CGeCircArc2d&
CGeCircArc2d::set(const CGePoint2d& startPoint, const CGePoint2d& endPoint, double bulge, bool bBulgeFlag /* = true*/)
{
	return *this;
}

CGeCircArc2d&
CGeCircArc2d::operator = (const CGeCircArc2d& source)
{
	return *this;
}
