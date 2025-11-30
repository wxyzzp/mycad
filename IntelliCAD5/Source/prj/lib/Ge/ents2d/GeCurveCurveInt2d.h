// File: GeCurveCurveInt2d.h
//
// CGeCurveCurveInt2d
//

#pragma once


#include "GeCurve2d.h"
#include "GeVector2d.h"


class GE_API CGeInterval;


class GE_API CGeCurveCurveInt2d : public CGeEntity2d
{
public:
	CGeCurveCurveInt2d();
	CGeCurveCurveInt2d(const CGeCurveCurveInt2d& source);
	CGeCurveCurveInt2d(const CGeCurve2d& curve1, const CGeCurve2d& curve2, const CGeTolerance& tol = CGeContext::s_defTol);
	CGeCurveCurveInt2d(const CGeCurve2d& curve1, const CGeCurve2d& curve2, const CGeInterval& range1, const CGeInterval& range2, const CGeTolerance& tol = CGeContext::s_defTol);

	const CGeCurve2d*	curve1				() const;
	const CGeCurve2d*	curve2				() const;
	void				getIntRanges		(CGeInterval& range1, CGeInterval& range2) const;
	CGeVector2d			planeNormal			() const;
	CGeTolerance		tolerance			() const;
	int					numIntPoints		() const;
	CGePoint2d			intPoint			(int intNum) const;
	void				getIntParams		(int intNum, double& param1, double& param2) const;
	void				getPointOnCurve1	(int intNum, CGePointOnCurve2d& pointOnCurve) const;
	void				getPointOnCurve2	(int intNum, CGePointOnCurve2d& pointOnCurve) const;
	void				getIntConfigs		(int intNum, CGe::EGeXConfig& config1to2, CGe::EGeXConfig& config2to1) const;
	bool				isTangential		(int intNum) const;
	bool				isTransversal		(int intNum) const;
	double				intPointTol			(int intNum) const;
	int					overlapCount		() const;
	bool				overlapDirection	() const;
	void				getOverlapRanges	(int overlapNum, CGeInterval& range1, CGeInterval& range2) const;
	void				changeCurveOrder	(); 
	CGeCurveCurveInt2d&	orderWrt1			();
	CGeCurveCurveInt2d&	orderWrt2			();
	CGeCurveCurveInt2d& set					(const CGeCurve2d& curve1, const CGeCurve2d& curve2, const CGeTolerance& tol = CGeContext::s_defTol);
	CGeCurveCurveInt2d& set					(const CGeCurve2d& curve1, const CGeCurve2d& curve2, const CGeInterval& range1, const CGeInterval& range2, const CGeTolerance& tol = CGeContext::s_defTol);
	CGeCurveCurveInt2d&	operator =			(const CGeCurveCurveInt2d& source);

private:
	CGeCurve2d		*m_pCurve1, *m_pCurve2;
	CGeVector2d		m_normal;
	CGeTolerance	m_tolerance;
	bool			m_bTangential;
	bool			m_bTransversal;
};
