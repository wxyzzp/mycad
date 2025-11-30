// File: GePolyline2d.h
//
// CGePolyline2d
//

#pragma once


#include "GeSplineCurve2d.h"


class GE_API CGePolyline2d : public CGeSplineCurve2d
{
public:
	CGePolyline2d();
	CGePolyline2d(const CGePolyline2d& source);
	CGePolyline2d(const CGePoint2dArray& points);
	CGePolyline2d(const CGeKnotVector& knots, const CGePoint2dArray& points);
	CGePolyline2d(const CGeCurve2d& curve, double approxEps);

	int					numFitPoints	() const;
	CGePoint2d			fitPointAt		(int i) const;
	CGeSplineCurve2d&	setFitPointAt	(int i, const CGePoint2d& point);
	CGePolyline2d&		operator =		(const CGePolyline2d& source);
};
