// File: GeLinearCurve2d.h
//
// CGeLinearCurve2d
//

#pragma once


#include "GeCurve2d.h"


class GE_API CGeLinearCurve2d : public CGeCurve2d
{
public:
	virtual ~CGeLinearCurve2d();

	bool				intersectWith		(const CGeLinearCurve2d& linearCurve, CGePoint2d& intPoint, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				overlap				(const CGeLinearCurve2d& linearCurve, CGeLinearCurve2d*& overlapCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isParallelTo		(const CGeLinearCurve2d& linearCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isPerpendicularTo	(const CGeLinearCurve2d& linearCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isColinearTo		(const CGeLinearCurve2d& linearCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGePoint2d			pointOnLine			() const;
	CGeVector2d			direction			() const;
	void				getLine				(CGeLine2d& line) const;
	void				getPerpLine			(const CGePoint2d& point, CGeLine2d& line) const;
	CGeLinearCurve2d&	operator =			(const CGeLinearCurve2d& source);

protected:
	CGeLinearCurve2d();
	CGeLinearCurve2d(const CGeLinearCurve2d& source);
};
