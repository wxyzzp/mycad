// File: GeLinearCurve3d.h
//
// CGeLinearCurve3d
//

#pragma once


#include "GeCurve3d.h"


class GE_API CGeLinearCurve3d : public CGeCurve3d
{
public:
	virtual ~CGeLinearCurve3d();

	bool				intersectWith		(const CGeLinearCurve3d& linearCurve, CGePoint3d& intPoint, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				intersectWith		(const CGePlanarSurface& planarSurface, CGePoint3d& intPoint, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				projIntersectWith	(const CGeLinearCurve3d& linearCurve, const CGeVector3d& projDir, CGePoint3d& pointOnThisCurve, CGePoint3d& pointOnOtherCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				overlap				(const CGeLinearCurve3d& linearCurve, CGeLinearCurve3d*& overlapCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isParallelTo		(const CGeLinearCurve3d& linearCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isParallelTo		(const CGePlanarSurface& planarSurface, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isPerpendicularTo	(const CGeLinearCurve3d& linearCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isPerpendicularTo	(const CGePlanarSurface& planarSurface, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isColinearTo		(const CGeLinearCurve3d& linearCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGePoint3d			pointOnLine			() const;
	bool				isOn				(const CGePoint3d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isOn				(const CGePoint3d& point, double& param, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isOn				(double param, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isOn				(const CGePlane& plane, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGeVector3d			direction			() const;
	void				getLine				(CGeLine3d& line) const;
	void				getPerpPlane		(const CGePoint3d& point, CGePlane& plane) const;
	CGeLinearCurve3d&	operator =			(const CGeLinearCurve3d& source);

protected:
	CGeLinearCurve3d();
	CGeLinearCurve3d(const CGeLinearCurve3d& source);
};
