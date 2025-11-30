// File: GeBoundedPlane.h
//
// CGeBoundedPlane
//

#pragma once


#include "GePlanarSurface.h"


class GE_API CGeLineSegment3d;


class GE_API CGeBoundedPlane : public CGePlanarSurface
{
public:
	CGeBoundedPlane();
	CGeBoundedPlane(const CGeBoundedPlane& source);
	CGeBoundedPlane(const CGePoint3d& origin, const CGeVector3d& uVector, const CGeVector3d& vVector);
	CGeBoundedPlane(const CGePoint3d& point1, const CGePoint3d& origin, const CGePoint3d& point2);

	bool				intersectWith	(const CGeLinearCurve3d& linearCurve, CGePoint3d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				intersectWith	(const CGePlane& plane, CGeLineSegment3d& results, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				intersectWith	(const CGeBoundedPlane& plane, CGeLineSegment3d& result, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGeBoundedPlane&	set				(const CGePoint3d& origin, const CGeVector3d& uVector, const CGeVector3d& vVector);
	CGeBoundedPlane&	set				(const CGePoint3d& point1, const CGePoint3d& origin, const CGePoint3d& point2);
	CGeBoundedPlane&	operator =		(const CGeBoundedPlane& source);
};
