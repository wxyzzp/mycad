// File: GePlanarSurface.h
//
// CGePlanarSurface
//

#pragma once


#include "GeSurface.h"
#include "GeVector3d.h"


class GE_API CGeLinearCurve3d;


class GE_API CGePlanarSurface : public CGeSurface
{
public:
	bool				intersectWith			(const CGeLinearCurve3d& linearCurve, CGePoint3d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGePoint3d			closestPointToLinearEnt	(const CGeLinearCurve3d& linearCurve, CGePoint3d& pointOnCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGePoint3d			closestPointToPlanarEnt	(const CGePlanarSurface& planarSurf, CGePoint3d& pointOnOtherPlanarSurf, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isParallelTo			(const CGeLinearCurve3d& linearCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isParallelTo			(const CGePlanarSurface& planarSurf, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isPerpendicularTo		(const CGeLinearCurve3d& linearCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isPerpendicularTo		(const CGePlanarSurface& planarSurf, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isCoplanarTo			(const CGePlanarSurface& planarSurf, const CGeTolerance& tol = CGeContext::s_defTol) const;
	void				get						(CGePoint3d& origin, CGeVector3d& uVector, CGeVector3d& vVector) const;
	void				get						(CGePoint3d& point1, CGePoint3d& origin, CGePoint3d& point2) const;
	CGePoint3d			pointOnPlane			() const;
	CGeVector3d			normal					() const;
	void				getCoefficients			(double& a, double& b, double& c, double& d) const;
	void				getCoordSystem			(CGePoint3d& origin, CGeVector3d& axis1, CGeVector3d& axis2) const;
	CGePlanarSurface&	operator =				(const CGePlanarSurface& source);

protected:
	CGePlanarSurface();
	CGePlanarSurface(const CGePlanarSurface& source);

	CGeVector3d	m_normal;
};
