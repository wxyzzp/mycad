// File: GeSurface
//
// CGeSurface
//

#pragma once


#include "GeEntity3d.h"
#include "GeInterval.h"
#include "GeArrays.h"


class GE_API CGePointOnSurface;


class GE_API CGeSurface : public CGeEntity3d
{
public:
	CGePoint2d	paramOf				(const CGePoint3d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool		isOn				(const CGePoint3d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool		isOn				(const CGePoint3d& point, CGePoint2d& paramPoint, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGePoint3d	closestPointTo		(const CGePoint3d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;
	void		getClosestPointTo	(const CGePoint3d& point, CGePointOnSurface& result, const CGeTolerance& tol = CGeContext::s_defTol) const;
	double		distanceTo			(const CGePoint3d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool		isNormalReversed	() const;
	CGeSurface&	reverseNormal		();
	void		getEnvelope			(CGeInterval& intervalX, CGeInterval& intervalY) const;
	bool		isClosedInU			(const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool		isClosedInV			(const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGePoint3d	evalPoint			(const CGePoint2d& paramPoint) const;
	CGePoint3d	evalPoint			(const CGePoint2d& paramPoint, int derivOrder, CGeVector3dArray& derivatives) const;
	CGePoint3d	evalPoint			(const CGePoint2d& paramPoint, int derivOrder, CGeVector3dArray& derivatives, CGeVector3d& normal) const;
	CGeSurface&	operator =			(const CGeSurface& source);

protected:
	CGeSurface();
	CGeSurface(const CGeSurface& source);
};
