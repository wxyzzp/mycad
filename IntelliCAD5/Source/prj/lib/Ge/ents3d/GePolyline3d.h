// File: GePolyline3d.h
//
// CGePolyline3d
//

#pragma once


#include "GeSplineCurve3d.h"


class GE_API CGePolyline3d : public CGeSplineCurve3d
{
public:
	CGePolyline3d();
	CGePolyline3d(const CGePolyline3d& source);
	CGePolyline3d(const CGePoint3dArray& points);
	CGePolyline3d(const CGeKnotVector& knots, const CGePoint3dArray& controlPoints);
	CGePolyline3d(const CGeCurve3d& curve, double approxEps);

	int					numFitPoints	() const;
	CGePoint3d			fitPointAt		(int i) const;
	CGeSplineCurve3d&	setFitPointAt	(int i, const CGePoint3d& point);
	CGePolyline3d&		operator =		(const CGePolyline3d& source);
};
