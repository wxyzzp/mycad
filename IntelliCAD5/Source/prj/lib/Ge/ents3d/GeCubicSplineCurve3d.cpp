// File: GeCubicSplineCurve3d.cpp
//

#include "GeCubicSplineCurve3d.h"
#include "GeVector3d.h"

//////////////////////////////////////////////////
// CGeCubicSplineCurve3d class implementation
//////////////////////////////////////////////////

CGeCubicSplineCurve3d::CGeCubicSplineCurve3d()
{
}

CGeCubicSplineCurve3d::CGeCubicSplineCurve3d(const CGeCubicSplineCurve3d& source)
{
}

CGeCubicSplineCurve3d::CGeCubicSplineCurve3d(const CGePoint3dArray& fitPoints, const CGeTolerance& tol /* = CGeContext::s_defTol*/)
{
}

CGeCubicSplineCurve3d::CGeCubicSplineCurve3d(const CGePoint3dArray& fitPoints, const CGeVector3d& startDeriv, const CGeVector3d& endDeriv, const CGeTolerance& tol /* = CGeContext::s_defTol*/)
{
}

CGeCubicSplineCurve3d::CGeCubicSplineCurve3d(const CGeCurve3d& curve, double epsilon /* = CGeContext::s_defTol.equalPoint()*/)
{
}

CGeCubicSplineCurve3d::CGeCubicSplineCurve3d(const CGeKnotVector& knots, const CGePoint3dArray& fitPnts, const CGeVector3dArray& firstDerivs, bool bIsPeriodic /* = false*/)
{
}

int
CGeCubicSplineCurve3d::numFitPoints() const
{
	return 0;
}

CGePoint3d
CGeCubicSplineCurve3d::fitPointAt(int i) const
{
	return CGePoint3d();
}

CGeCubicSplineCurve3d&
CGeCubicSplineCurve3d::setFitPointAt(int i, const CGePoint3d& point)
{
	return *this;
}

CGeVector3d
CGeCubicSplineCurve3d::firstDerivAt(int i) const
{
	return CGeVector3d();
}

CGeCubicSplineCurve3d&
CGeCubicSplineCurve3d::setFirstDerivAt(int i, const CGeVector3d& deriv)
{
	return *this;
}

CGeCubicSplineCurve3d&
CGeCubicSplineCurve3d::operator = (const CGeCubicSplineCurve3d& source)
{
	return *this;
}
