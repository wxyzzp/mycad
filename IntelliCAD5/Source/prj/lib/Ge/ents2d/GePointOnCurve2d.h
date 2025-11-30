// File: GePointOnCurve2d.h
//
// CGePointOnCurve2d
//

#pragma once


#include "GePointEntity2d.h"
#include "GeCurve2d.h"


class GE_API CGePointOnCurve2d : public CGePointEntity2d
{
public:
	CGePointOnCurve2d();
	CGePointOnCurve2d(const CGeCurve2d& curve);
	CGePointOnCurve2d(const CGeCurve2d& curve, double param);
	CGePointOnCurve2d(const CGePointOnCurve2d& source);

	const CGeCurve2d*	curve		() const;
	double				parameter	() const;
	CGePoint2d			point		() const;
	CGePoint2d			point		(double param);
	CGePoint2d			point		(const CGeCurve2d& curve, double param);
	CGeVector2d			deriv		(int order) const;
	CGeVector2d			deriv		(int order, double param);
	CGeVector2d			deriv		(int order, const CGeCurve2d& curve, double param);
	bool				isSingular	(const CGeTolerance&  tol = CGeContext::s_defTol) const;
	bool				curvature	(double& result);
	bool				curvature	(double param, double& result);
	CGePointOnCurve2d&	setCurve	(const CGeCurve2d& curve);
	CGePointOnCurve2d&	setParameter(double param);
	CGePointOnCurve2d&	operator =	(const CGePointOnCurve2d& source);

private:
	CGeCurve2d*	m_pCurve;
	double		m_parameter;
};
