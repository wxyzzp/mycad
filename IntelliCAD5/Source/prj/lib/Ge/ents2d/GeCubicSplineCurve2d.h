// File: GeCubicSplineCurve2d.h
//
// CGeCubicSplineCurve2d
//

#pragma once


#include "GeSplineCurve2d.h"


class GE_API CGeCubicSplineCurve2d : public CGeSplineCurve2d
{
public:
	CGeCubicSplineCurve2d();
	CGeCubicSplineCurve2d(const CGeCubicSplineCurve2d& source);
	CGeCubicSplineCurve2d(const CGePoint2dArray& fitPoints, const CGeTolerance& tol = CGeContext::s_defTol);
	CGeCubicSplineCurve2d(const CGePoint2dArray& fitPoints, const CGeVector2d& startDeriv, const CGeVector2d& endDeriv, const CGeTolerance& tol = CGeContext::s_defTol);
	CGeCubicSplineCurve2d(const CGeCurve2d& curve, double epsilon = CGeContext::s_defTol.equalPoint());
	CGeCubicSplineCurve2d(const CGeKnotVector& knots, const CGePoint2dArray& fitPoints, const CGeVector2dArray& firstDerivs, bool bIsPeriodic = false);

	int						numFitPoints	()        const;
	CGePoint2d				fitPointAt		(int i) const;
	CGeCubicSplineCurve2d&	setFitPointAt	(int i, const CGePoint2d& point);
	CGeVector2d				firstDerivAt	(int i) const;
	CGeCubicSplineCurve2d&	setFirstDerivAt	(int i, const CGeVector2d& deriv);
	CGeCubicSplineCurve2d&	operator =		(const CGeCubicSplineCurve2d& source);
};
