// File: GeSphere.cpp
//

#include "GeSphere.h"
#include "GeVector3d.h"

//////////////////////////////////////////////////
// CGeSphere class implementation
//////////////////////////////////////////////////

CGeSphere::CGeSphere()
{
}

CGeSphere::CGeSphere(double radius, const CGePoint3d& center)
{
}

CGeSphere::CGeSphere(double radius, const CGePoint3d& center, const CGeVector3d& northAxis, const CGeVector3d& refAxis, double startAngleU, double endAngleU, double startAngleV, double endAngleV)
{
}

CGeSphere::CGeSphere(const CGeSphere& source)
{
}

double
CGeSphere::radius() const
{
	return m_radius;
}

CGePoint3d
CGeSphere::center() const
{
	return m_center;
}

void
CGeSphere::getAnglesInU(double& startAngle, double& endAngle) const
{
}

void
CGeSphere::getAnglesInV(double& startAngle, double& endAngle) const
{
}

CGeVector3d
CGeSphere::northAxis() const
{
	return CGeVector3d();
}

CGeVector3d
CGeSphere::refAxis() const
{
	return CGeVector3d();
}

CGePoint3d
CGeSphere::northPole() const
{
	return CGePoint3d();
}

CGePoint3d
CGeSphere::southPole() const
{
	return CGePoint3d();
}

bool
CGeSphere::isOuterNormal() const
{
	return false;
}

bool
CGeSphere::isClosed(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGeSphere&
CGeSphere::setRadius(double radius)
{
	m_radius = radius;
	return *this;
}

CGeSphere&
CGeSphere::setAnglesInU(double startAngle, double endAngle)
{
	return *this;
}

CGeSphere&
CGeSphere::setAnglesInV(double startAngle, double endAngle)
{
	return *this;
}

CGeSphere&
CGeSphere::set(double radius, const CGePoint3d& center)
{
	m_radius = radius;
	m_center = center;
	return *this;
}

CGeSphere&
CGeSphere::set(double radius, const CGePoint3d& center, const CGeVector3d& northAxis, const CGeVector3d& refAxis, double startAngleU, double endAngleU, double startAngleV, double endAngleV)
{
	m_radius = radius;
	m_center = center;
	return *this;
}

bool
CGeSphere::intersectWith(const CGeLinearCurve3d& linearCurve, int& intNum, CGePoint3d& point1, CGePoint3d& point2, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGeSphere&
CGeSphere::operator = (const CGeSphere& source)
{
	return *this;
}
