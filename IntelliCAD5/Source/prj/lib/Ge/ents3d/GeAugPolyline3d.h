// File: GeAugPolyline3d.h
//
// CGeAugPolyline3d
//

#pragma once


#include "GePolyline3d.h"


class GE_API CGeAugPolyline3d : public CGePolyline3d
{
public:
	CGeAugPolyline3d();
	CGeAugPolyline3d(const CGeAugPolyline3d& source);
	CGeAugPolyline3d(const CGeKnotVector& knots, const CGePoint3dArray& controlPoints, const CGeVector3dArray& vectors);
	CGeAugPolyline3d(const CGePoint3dArray& controlPoints, const CGeVector3dArray& vectors);
	CGeAugPolyline3d(const CGeCurve3d& curve, double fromParam, double toParam, double approxEps);

	CGePoint3d			getPoint	(int i) const;
	CGeAugPolyline3d&	setPoint	(int i, CGePoint3d point);
	void				getPoints	(CGePoint3dArray& points) const;
	CGeVector3d			getVector	(int i) const;
	CGeAugPolyline3d&	setVector	(int i, CGeVector3d vector);
	void				getD1Vectors(CGeVector3dArray& d1vectors) const;
	CGeVector3d			getD2Vector	(int i) const;
	CGeAugPolyline3d&	setD2Vector	(int i, CGeVector3d d2vector);
	void				getD2Vectors(CGeVector3dArray& d2vectors) const;
	double				approxTol	() const;
	CGeAugPolyline3d&	setApproxTol(double approxTol);
	CGeAugPolyline3d&	operator =	(const CGeAugPolyline3d& source);
};
