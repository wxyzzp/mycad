// File: GeCircArc3d.cpp
//

#include "GeCircArc3d.h"

//////////////////////////////////////////////////
// CGeCircArc3d class implementation
//////////////////////////////////////////////////

CGeCircArc3d::CGeCircArc3d()
{
}

CGeCircArc3d::CGeCircArc3d(const CGeCircArc3d& source)
{
}

CGeCircArc3d::CGeCircArc3d(const CGePoint3d& startPoint, const CGePoint3d& pointOnArc, const CGePoint3d& endPoint)
{
}

CGeCircArc3d::CGeCircArc3d(const CGePoint3d& center, const CGeVector3d& normal, double radius)
{
}

CGeCircArc3d::CGeCircArc3d(const CGePoint3d& center, const CGeVector3d& normal, const CGeVector3d& refVec, double radius, double startAngle, double endAngle)
{
}

CGePoint3d
CGeCircArc3d::closestPointToPlane(const CGePlanarSurface& planarSurface, CGePoint3d& pointOnPlane, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return CGePoint3d();
}

bool
CGeCircArc3d::intersectWith(const CGeLinearCurve3d& linearCurve, int& intNum, CGePoint3d& point1, CGePoint3d& point2, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCircArc3d::intersectWith(const CGeCircArc3d& arc, int& intNum, CGePoint3d& point1, CGePoint3d& point2, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCircArc3d::intersectWith(const CGePlanarSurface& planarSurface, int& intNum, CGePoint3d& point1, CGePoint3d& point2, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCircArc3d::projIntersectWith(const CGeLinearCurve3d& linearCurve, const CGeVector3d& projDir, int& numInt, CGePoint3d& pointOnArc1, CGePoint3d& pointOnArc2, CGePoint3d& pointOnLine1, CGePoint3d& pointOnLine2, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCircArc3d::tangent(const CGePoint3d& point, CGeLine3d& line, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCircArc3d::tangent(const CGePoint3d& point, CGeLine3d& line, const CGeTolerance& tol, CGeError& error) const
{
	return false;
}

bool
CGeCircArc3d::isInside(const CGePoint3d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGePoint3d
CGeCircArc3d::center() const
{
	return m_center;
}

CGeVector3d
CGeCircArc3d::refVec() const
{
	return m_refVector;
}

double
CGeCircArc3d::radius() const
{
	return m_radius;
}

double
CGeCircArc3d::startAng() const
{
	return m_startAngle;
}

double
CGeCircArc3d::endAng() const
{
	return m_endAngle;
}

CGePoint3d
CGeCircArc3d::startPoint() const
{
	return CGePoint3d();
}

CGePoint3d
CGeCircArc3d::endPoint() const
{
	return CGePoint3d();
}

CGeVector3d
CGeCircArc3d::normal() const
{
	return m_normal;
}

void
CGeCircArc3d::getPlane(CGePlane& plane) const
{
}

CGeCircArc3d&
CGeCircArc3d::setCenter(const CGePoint3d& center)
{
	m_center = center;
	return *this;
}

CGeCircArc3d&
CGeCircArc3d::setRadius(double radius)
{
	m_radius = radius;
	return *this;
}

CGeCircArc3d&
CGeCircArc3d::setAngles(double startAngle, double endAngle)
{
	m_startAngle = startAngle;
	m_endAngle = endAngle;
	return *this;
}

CGeCircArc3d&
CGeCircArc3d::setAxes(const CGeVector3d& normal, const CGeVector3d& refVec)
{
	m_normal = normal;
	m_refVector = refVec;
	return *this;
}

CGeCircArc3d&
CGeCircArc3d::set(const CGePoint3d& startPoint, const CGePoint3d& pointOnArc, const CGePoint3d& endPoint)
{
	return *this;
}

CGeCircArc3d&
CGeCircArc3d::set(const CGePoint3d& startPoint, const CGePoint3d& pointOnArc, const CGePoint3d& endPoint, CGeError& error)
{
	return *this;
}

CGeCircArc3d&
CGeCircArc3d::set(const CGeCurve3d& curve1, const CGeCurve3d& curve2, double radius, double& param1, double& param2, bool& bSuccess)
{
	return *this;
}

CGeCircArc3d&
CGeCircArc3d::set(const CGeCurve3d& curve1, const CGeCurve3d& curve2, const CGeCurve3d& curve3, double& param1, double& param2, double& param3, bool& bSuccess)
{
	return *this;
}

CGeCircArc3d&
CGeCircArc3d::set(const CGePoint3d& center, const CGeVector3d& normal, double radius)
{
	return *this;
}

CGeCircArc3d&
CGeCircArc3d::set(const CGePoint3d& center, const CGeVector3d& normal, const CGeVector3d& refVec, double radius, double startAngle, double endAngle)
{
	return *this;
}

CGeCircArc3d&
CGeCircArc3d:: operator = (const CGeCircArc3d& source)
{
	return *this;
}
