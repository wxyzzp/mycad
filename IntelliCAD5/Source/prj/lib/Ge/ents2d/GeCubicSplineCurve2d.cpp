// File: GeCubicSplineCurve2d.cpp
//

#include "GeCubicSplineCurve2d.h"
#include "GeVector2d.h"

//////////////////////////////////////////////////
// CGeCubicSplineCurve2d class implementation
//////////////////////////////////////////////////

CGeCubicSplineCurve2d::CGeCubicSplineCurve2d()
{
}

CGeCubicSplineCurve2d::CGeCubicSplineCurve2d(const CGeCubicSplineCurve2d& source)
{
}

CGeCubicSplineCurve2d::CGeCubicSplineCurve2d(const CGePoint2dArray& fitPoints, const CGeTolerance& tol /* = CGeContext::s_defTol*/)
{
}

CGeCubicSplineCurve2d::CGeCubicSplineCurve2d(const CGePoint2dArray& fitPoints, const CGeVector2d& startDeriv, const CGeVector2d& endDeriv, const CGeTolerance& tol /* = CGeContext::s_defTol*/)
{
}

CGeCubicSplineCurve2d::CGeCubicSplineCurve2d(const CGeCurve2d& curve, double epsilon /* = CGeContext::s_defTol.equalPoint()*/)
{
}

CGeCubicSplineCurve2d::CGeCubicSplineCurve2d(const CGeKnotVector& knots, const CGePoint2dArray& fitPoints, const CGeVector2dArray& firstDerivs, bool bIsPeriodic /* = false*/)
{
}

int
CGeCubicSplineCurve2d::numFitPoints() const
{
	return 0;
}

CGePoint2d
CGeCubicSplineCurve2d::fitPointAt(int i) const
{
	return CGePoint2d();
}

CGeCubicSplineCurve2d&
CGeCubicSplineCurve2d::setFitPointAt(int i, const CGePoint2d& point)
{
	return *this;
}

CGeVector2d
CGeCubicSplineCurve2d::firstDerivAt(int i) const
{
	return CGeVector2d();
}

CGeCubicSplineCurve2d&
CGeCubicSplineCurve2d::setFirstDerivAt(int i, const CGeVector2d& deriv)
{
	return *this;
}

CGeCubicSplineCurve2d&
CGeCubicSplineCurve2d::operator = (const CGeCubicSplineCurve2d& source)
{
	return *this;
}
