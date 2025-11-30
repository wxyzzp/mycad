// File: GePlane.h
//
// CGePlane
//

#pragma once


#include "GePlanarSurface.h"


class GE_API CGeLine3d;
class GE_API CGeBoundedPlane;
class GE_API CGeLineSegment3d;


class GE_API CGePlane : public CGePlanarSurface
{
public:
	CGePlane();
	CGePlane(const CGePlane& source);
	CGePlane(const CGePoint3d& origin, const CGeVector3d& normal);
	CGePlane(const CGePoint3d& pointU, const CGePoint3d& origin, const CGePoint3d& pointV);
	CGePlane(const CGePoint3d& origin, const CGeVector3d& uAxis, const CGeVector3d& vAxis);
	CGePlane(double a, double b, double c, double d);

	double		signedDistanceTo(const CGePoint3d& point) const;
	bool		intersectWith	(const CGeLinearCurve3d& linearCurve, CGePoint3d& resultPoint, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool		intersectWith	(const CGePlane& plane, CGeLine3d& resultLine, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool		intersectWith	(const CGeBoundedPlane& boundedPlane, CGeLineSegment3d& resultLineSegment, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGePlane&	set				(const CGePoint3d& origin, const CGeVector3d& normal);
	CGePlane&	set				(const CGePoint3d& pointU, const CGePoint3d& origin, const CGePoint3d& pointV);
	CGePlane&	set				(double a, double b, double c, double d);
	CGePlane&	set				(const CGePoint3d& origin, const CGeVector3d& uAxis, const CGeVector3d& vAxis);
	CGePlane&	operator =		(const CGePlane& source);

	static const CGePlane	s_xyPlane;
	static const CGePlane	s_yzPlane;
	static const CGePlane	s_zxPlane;
};
