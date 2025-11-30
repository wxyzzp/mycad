// File: GePointOnCurve3d.h
//
// CGePointOnCurve3d
//

#pragma once


#include "GePointEntity3d.h"
#include "GeCurve3d.h"


class GE_API CGePointOnCurve3d : public CGePointEntity3d
{
public:
	CGePointOnCurve3d();
	CGePointOnCurve3d(const CGeCurve3d& curve);
	CGePointOnCurve3d(const CGeCurve3d& curve, double param);
	CGePointOnCurve3d(const CGePointOnCurve3d& source);

	const CGeCurve3d*	curve          () const;
	double				parameter      () const;
	CGePoint3d			point          () const;
	CGePoint3d			point          (double param);
	CGePoint3d			point          (const CGeCurve3d& curve, double param);
	CGeVector3d			deriv          (int order) const;
	CGeVector3d			deriv          (int order, double param);
	CGeVector3d			deriv          (int order, const CGeCurve3d& curve, double param);
	bool				isSingular     (const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				curvature      (double& result);
	bool				curvature      (double param, double& result);
	CGePointOnCurve3d&	setCurve       (const CGeCurve3d& curve);
	CGePointOnCurve3d&	setParameter   (double param);
	CGePointOnCurve3d&	operator =     (const CGePointOnCurve3d& source);

private:
	CGeCurve3d*	m_pCurve;
	double		m_parameter;
};
