// File: GeCylinder.cpp
//

#include "GeCylinder.h"
#include "GeVector3d.h"

//////////////////////////////////////////////////
// CGeCylinder class implementation
//////////////////////////////////////////////////

CGeCylinder::CGeCylinder()
{
}

CGeCylinder::CGeCylinder(double radius, const CGePoint3d& origin, const CGeVector3d& axisOfSymmetry)
{
}

CGeCylinder::CGeCylinder(double radius, const CGePoint3d& origin, const CGeVector3d& axisOfSymmetry, const CGeVector3d& refVector, const CGeInterval& height, double startAngle, double endAngle)
{
}

CGeCylinder::CGeCylinder(const CGeCylinder& source)
{
}

double
CGeCylinder::radius() const
{
	return m_radius;
}

CGePoint3d
CGeCylinder::origin() const
{
	return CGePoint3d();
}

void
CGeCylinder::getAngles(double& startAngle, double& endAngle) const
{
}

void
CGeCylinder::getHeight(CGeInterval& height) const
{
}

double
CGeCylinder::heightAt(double u) const
{
	return 0.0;
}

CGeVector3d
CGeCylinder::axisOfSymmetry() const
{
	return CGeVector3d();
}

CGeVector3d
CGeCylinder::refAxis() const
{
	return CGeVector3d();
}

bool
CGeCylinder::isOuterNormal() const
{
	return false;
}

bool
CGeCylinder::isClosed(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGeCylinder&
CGeCylinder::setRadius(double radius)
{
	m_radius = radius;
	return *this;
}

CGeCylinder&
CGeCylinder::setAngles(double startAngle, double endAngle)
{
	return *this;
}

CGeCylinder&
CGeCylinder::setHeight(const CGeInterval& height)
{
	return *this;
}

CGeCylinder&
CGeCylinder::set(double radius, const CGePoint3d& origin, const CGeVector3d& axisOfSym)
{
	return *this;
}

CGeCylinder&
CGeCylinder::set(double radius, const CGePoint3d& origin, const CGeVector3d& axisOfSym, const CGeVector3d& refVector, const CGeInterval& height, double startAngle, double endAngle)
{
	return *this;
}

bool
CGeCylinder::intersectWith(const CGeLinearCurve3d& linearCurve, int& intNum, CGePoint3d& point1, CGePoint3d& point2, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGeCylinder&
CGeCylinder::operator = (const CGeCylinder& source)
{
	return *this;
}
