// File: GeTorus.cpp
//

#include "GeTorus.h"
#include "GeVector3d.h"

//////////////////////////////////////////////////
// CGeTorus class implementation
//////////////////////////////////////////////////

CGeTorus::CGeTorus()
{
}

CGeTorus::CGeTorus(double majorRadius, double minorRadius, const CGePoint3d& origin, const CGeVector3d& axisOfSymmetry)
{
}

CGeTorus::CGeTorus(double majorRadius, double minorRadius, const CGePoint3d& origin, const CGeVector3d& axisOfSymmetry, const CGeVector3d& refAxis, double startAngleU, double endAngleU, double startAngleV, double endAngleV)
{
}

CGeTorus::CGeTorus(const CGeTorus& source)
{
}

double
CGeTorus::majorRadius() const
{
	return 0.0;
}

double
CGeTorus::minorRadius() const
{
	return 0.0;
}

void
CGeTorus::getAnglesInU(double& startAngle, double& endAngle) const
{
}

void
CGeTorus::getAnglesInV(double& startAngle, double& endAngle) const
{
}

CGePoint3d
CGeTorus::center() const
{
	return m_center;
}

CGeVector3d
CGeTorus::axisOfSymmetry() const
{
	return CGeVector3d();
}

CGeVector3d
CGeTorus::refAxis() const
{
	return CGeVector3d();
}

bool
CGeTorus::isOuterNormal() const
{
	return false;
}

CGeTorus&
CGeTorus::setMajorRadius(double radius)
{
	return *this;
}

CGeTorus&
CGeTorus::setMinorRadius(double radius)
{
	return *this;
}

CGeTorus&
CGeTorus::setAnglesInU(double startAngle, double endAngle)
{
	return *this;
}

CGeTorus&
CGeTorus::setAnglesInV(double startAngle, double endAngle)
{
	return *this;
}

CGeTorus&
CGeTorus::set(double majorRadius, double minorRadius, const CGePoint3d& center, const CGeVector3d& axisOfSymmetry)
{
	m_center = center;
	return *this;
}

CGeTorus&
CGeTorus::set(double majorRadius, double minorRadius, const CGePoint3d& center, const CGeVector3d& axisOfSymmetry, const CGeVector3d& refAxis, double startAngleU, double endAngleU, double startAngleV, double endAngleV)
{
	m_center = center;
	return *this;
}

bool
CGeTorus::intersectWith(const CGeLinearCurve3d& linearCurve, int& intNum, CGePoint3d& point1, CGePoint3d& point2, CGePoint3d& point3, CGePoint3d& point4, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGeTorus&
CGeTorus::operator = (const CGeTorus& source)
{
	return *this;
}
