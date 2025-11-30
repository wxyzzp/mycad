// File: GeCurveCurveInt3d.cpp
//

#include "GeCurveCurveInt3d.h"

//////////////////////////////////////////////////
// CGeCurveCurveInt3d class implementation
//////////////////////////////////////////////////

CGeCurveCurveInt3d::CGeCurveCurveInt3d()
{
}

CGeCurveCurveInt3d::CGeCurveCurveInt3d(const CGeCurveCurveInt3d& source)
{
}

CGeCurveCurveInt3d::CGeCurveCurveInt3d(const CGeCurve3d& curve1, const CGeCurve3d& curve2, const CGeVector3d& planeNormal /* = CGeVector3d::s_identity*/, const CGeTolerance& tol /* = ÑGeContext::s_defTol*/)
{
}

CGeCurveCurveInt3d::CGeCurveCurveInt3d(const CGeCurve3d& curve1, const CGeCurve3d& curve2, const CGeInterval& range1, const CGeInterval& range2, const CGeVector3d& planeNormal /* = CGeVector3d::s_identity*/, const CGeTolerance& tol /* = ÑGeContext::s_defTol*/)
{
}

const CGeCurve3d*
CGeCurveCurveInt3d::curve1() const
{
	return m_pCurve1;
}

const CGeCurve3d*
CGeCurveCurveInt3d::curve2() const
{
	return m_pCurve2;
}

void
CGeCurveCurveInt3d::getIntRanges(CGeInterval& range1, CGeInterval& range2) const
{
}

CGeVector3d
CGeCurveCurveInt3d::planeNormal() const
{
	return m_normal;
}

CGeTolerance
CGeCurveCurveInt3d::tolerance() const
{
	return m_tolerance;
}

int
CGeCurveCurveInt3d::numIntPoints() const
{
	return 0;
}

CGePoint3d
CGeCurveCurveInt3d::intPoint(int intNum) const
{
	return CGePoint3d();
}

void
CGeCurveCurveInt3d::getIntParams(int intNum, double& param1, double& param2) const
{
}

void
CGeCurveCurveInt3d::getPointOnCurve1(int intNum, CGePointOnCurve3d& pointOnCurve) const
{
}

void
CGeCurveCurveInt3d::getPointOnCurve2(int intNum, CGePointOnCurve3d& pointOnCurve) const
{
}

void
CGeCurveCurveInt3d::getIntConfigs(int intNum, CGe::EGeXConfig& config1to2, CGe::EGeXConfig& config2to1) const
{
}

bool
CGeCurveCurveInt3d::isTangential(int intNum) const
{
	return m_bTangential;
}

bool
CGeCurveCurveInt3d::isTransversal(int intNum) const
{
	return m_bTransversal;
}

double
CGeCurveCurveInt3d::intPointTol(int intNum) const
{
	return 0.0;
}

int
CGeCurveCurveInt3d::overlapCount() const
{
	return 0;
}

bool
CGeCurveCurveInt3d::overlapDirection() const
{
	return false;
}

void
CGeCurveCurveInt3d::getOverlapRanges(int overlapNum, CGeInterval& range1, CGeInterval& range2) const
{
}

void
CGeCurveCurveInt3d::changeCurveOrder()
{
}

CGeCurveCurveInt3d&
CGeCurveCurveInt3d::orderWrt1()
{
	return *this;
}

CGeCurveCurveInt3d&
CGeCurveCurveInt3d::orderWrt2()
{
	return *this;
}

CGeCurveCurveInt3d&
CGeCurveCurveInt3d::set(const CGeCurve3d& curve1, const CGeCurve3d& curve2, const CGeVector3d& planeNormal /* = CGeVector3d::s_identity*/, const CGeTolerance& tol /* = ÑGeContext::s_defTol*/)
{
	return *this;
}

CGeCurveCurveInt3d&
CGeCurveCurveInt3d::set(const CGeCurve3d& curve1, const CGeCurve3d& curve2, const CGeInterval& range1, const CGeInterval& range2, const CGeVector3d& planeNormal /* = CGeVector3d::s_identity*/, const CGeTolerance& tol /* = ÑGeContext::s_defTol*/)
{
	return *this;
}

CGeCurveCurveInt3d&
CGeCurveCurveInt3d::operator = (const CGeCurveCurveInt3d& source)
{
	return *this;
}
