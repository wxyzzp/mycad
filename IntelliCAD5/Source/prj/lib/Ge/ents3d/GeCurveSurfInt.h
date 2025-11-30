// File: GeCurveSurfInt.h
//
// CGeCurveSurfInt
//

#pragma once


#include "GeCurve3d.h"
#include "GeSurface.h"


class GE_API CGeCurveSurfInt : public CGeEntity3d
{
public:
	CGeCurveSurfInt();
	CGeCurveSurfInt(const CGeCurve3d& curve, const CGeSurface& surf, const CGeTolerance& tol = CGeContext::s_defTol);
	CGeCurveSurfInt(const CGeCurveSurfInt& source);

	const CGeCurve3d*	curve				() const;
	const CGeSurface*	surface				() const;
	CGeTolerance		tolerance			() const;
	int					numIntPoints		(CGe::EGeIntersectError& error) const;
	CGePoint3d			intPoint			(int intNum, CGe::EGeIntersectError& error) const;
	void				getIntParams		(int intNum, double& param1, CGePoint2d& param2, CGe::EGeIntersectError& error) const;
	void				getPointOnCurve		(int intNum, CGePointOnCurve3d&, CGe::EGeIntersectError& error) const;
	void				getPointOnSurface	(int intNum, CGePointOnSurface&, CGe::EGeIntersectError& error) const;
	void				getIntConfigs		(int intNum, CGe::EcsiConfig& lower, CGe::EcsiConfig& higher, bool& bSmallAngle, CGe::EGeIntersectError& error) const;
	CGeCurveSurfInt&	set					(const CGeCurve3d& curve, const CGeSurface& surf, const CGeTolerance& tol = CGeContext::s_defTol);
	CGeCurveSurfInt&	operator =			(const CGeCurveSurfInt& source);

private:
	CGeCurve3d*		m_pCurve;
	CGeSurface*		m_pSurface;
	CGeTolerance	m_tolerance;
};
