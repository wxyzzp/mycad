// File: GeCurveCurveInt2d.cpp
//

#include "GeCurveCurveInt2d.h"

//////////////////////////////////////////////////
// CGeCurveCurveInt2d class implementation
//////////////////////////////////////////////////

CGeCurveCurveInt2d::CGeCurveCurveInt2d()
{
}

CGeCurveCurveInt2d::CGeCurveCurveInt2d(const CGeCurveCurveInt2d& source)
{
}

CGeCurveCurveInt2d::CGeCurveCurveInt2d(const CGeCurve2d& curve1, const CGeCurve2d& curve2, const  CGeTolerance& tol /* = ÑGeContext::s_defTol*/)
{
}

CGeCurveCurveInt2d::CGeCurveCurveInt2d(const CGeCurve2d& curve1, const CGeCurve2d& curve2, const CGeInterval& range1, const CGeInterval& range2, const  CGeTolerance& tol /* = ÑGeContext::s_defTol*/)
{
}

const CGeCurve2d*
CGeCurveCurveInt2d::curve1() const
{
	return m_pCurve1;
}

const CGeCurve2d*
CGeCurveCurveInt2d::curve2() const
{
	return m_pCurve1;
}

void
CGeCurveCurveInt2d::getIntRanges(CGeInterval& range1, CGeInterval& range2) const
{
}

CGeVector2d
CGeCurveCurveInt2d::planeNormal() const
{
	return m_normal;
}

CGeTolerance
CGeCurveCurveInt2d::tolerance() const
{
	return m_tolerance;
}

int
CGeCurveCurveInt2d::numIntPoints() const
{
	return 0;
}

CGePoint2d
CGeCurveCurveInt2d::intPoint(int intNum) const
{
	return CGePoint2d();
}

void
CGeCurveCurveInt2d::getIntParams(int intNum, double& param1, double& param2) const
{
}

void
CGeCurveCurveInt2d::getPointOnCurve1(int intNum, CGePointOnCurve2d& pointOnCurve) const
{
}

void
CGeCurveCurveInt2d::getPointOnCurve2(int intNum, CGePointOnCurve2d& pointOnCurve) const
{
}

void
CGeCurveCurveInt2d::getIntConfigs(int intNum, CGe::EGeXConfig& config1to2, CGe::EGeXConfig& config2to1) const
{
}

bool
CGeCurveCurveInt2d::isTangential(int intNum) const
{
	return m_bTangential;
}

bool
CGeCurveCurveInt2d::isTransversal(int intNum) const
{
	return m_bTransversal;
}

double
CGeCurveCurveInt2d::intPointTol(int intNum) const
{
	return 0.0;
}

int
CGeCurveCurveInt2d::overlapCount() const
{
	return 0;
}

bool
CGeCurveCurveInt2d::overlapDirection() const
{
	return false;
}

void
CGeCurveCurveInt2d::getOverlapRanges(int overlapNum, CGeInterval& range1, CGeInterval& range2) const
{
}

void
CGeCurveCurveInt2d::changeCurveOrder()
{
}

CGeCurveCurveInt2d&
CGeCurveCurveInt2d::orderWrt1()
{
	return *this;
}

CGeCurveCurveInt2d&
CGeCurveCurveInt2d::orderWrt2()
{
	return *this;
}

CGeCurveCurveInt2d&
CGeCurveCurveInt2d::set(const CGeCurve2d& curve1, const CGeCurve2d& curve2, const  CGeTolerance& tol /* = ÑGeContext::s_defTol*/)
{
	return *this;
}

CGeCurveCurveInt2d&
CGeCurveCurveInt2d::set(const CGeCurve2d& curve1, const CGeCurve2d& curve2, const CGeInterval& range1, const CGeInterval& range2, const  CGeTolerance& tol /* = ÑGeContext::s_defTol*/)
{
	return *this;
}

CGeCurveCurveInt2d&
CGeCurveCurveInt2d::operator = (const CGeCurveCurveInt2d& source)
{
	return *this;
}
