// File: GeCone.cpp
//

#include "GeCone.h"
#include "GeVector3d.h"

//////////////////////////////////////////////////
// CGeCone class implementation
//////////////////////////////////////////////////

CGeCone::CGeCone()
{
}

CGeCone::CGeCone(double cosAngle, double sinAngle, const CGePoint3d& baseOrigin, double baseRadius, const CGeVector3d& axisOfSymmetry)
{
}

CGeCone::CGeCone(double cosAngle, double sinAngle, const CGePoint3d& baseOrigin, double baseRadius, const CGeVector3d& axisOfSymmetry, const CGeVector3d& refVector, const CGeInterval& height, double startAngle, double endAngle)
{
}

CGeCone::CGeCone(const CGeCone& source)
{
}

double
CGeCone::baseRadius() const
{
	return 0.0;
}

CGePoint3d
CGeCone::baseCenter() const
{
	return CGePoint3d();
}

void
CGeCone::getAngles(double& startAngle, double& endAngle) const
{
}

double
CGeCone::halfAngle() const
{
	return 0.0;
}

void
CGeCone::getHalfAngle(double& cosAngle, double& sinAngle) const
{
}

void
CGeCone::getHeight(CGeInterval& height) const
{
}

double
CGeCone::heightAt(double u) const
{
	return 0.0;
}

CGeVector3d
CGeCone::axisOfSymmetry() const
{
	return CGeVector3d();
}

CGeVector3d
CGeCone::refAxis() const
{
	return CGeVector3d();
}

CGePoint3d
CGeCone::apex() const
{
	return CGePoint3d();
}

bool
CGeCone::isClosed(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCone::isOuterNormal() const
{
	return false;
}

CGeCone&
CGeCone::setBaseRadius(double radius)
{
	return *this;
}

CGeCone&
CGeCone::setAngles(double startAngle, double endAngle)
{
	return *this;
}

CGeCone&
CGeCone::setHeight(const CGeInterval& height)
{
	return *this;
}

CGeCone&
CGeCone::set(double cosAngle, double sinAngle, const CGePoint3d& baseCenter, double baseRadius, const CGeVector3d& axisOfSymmetry)
{
	return *this;
}

CGeCone&
CGeCone::set(double cosAngle, double sinAngle, const CGePoint3d& baseCenter, double baseRadius, const CGeVector3d& axisOfSymmetry, const CGeVector3d& refVector, const CGeInterval& height, double startAngle, double endAngle)
{
	return *this;
}

bool
CGeCone::intersectWith(const CGeLinearCurve3d& linearCurve, int& intNum, CGePoint3d& point1, CGePoint3d& point2, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGeCone&
CGeCone::operator = (const CGeCone& source)
{
	return *this;
}
