// File: GeCurveCurveInt3d.h
//
// CGeCurveCurveInt3d
//

#pragma once


#include "GeCurve3d.h"
#include "GeVector3d.h"


class GE_API CGeCurveCurveInt3d : public CGeEntity3d
{
public:
	CGeCurveCurveInt3d();
	CGeCurveCurveInt3d(const CGeCurveCurveInt3d& source);
	CGeCurveCurveInt3d(const CGeCurve3d& curve1, const CGeCurve3d& curve2, const CGeVector3d& planeNormal = CGeVector3d::s_identity, const CGeTolerance& tol = CGeContext::s_defTol);
	CGeCurveCurveInt3d(const CGeCurve3d& curve1, const CGeCurve3d& curve2, const CGeInterval& range1, const CGeInterval& range2, const CGeVector3d& planeNormal = CGeVector3d::s_identity, const CGeTolerance& tol = CGeContext::s_defTol);

	const CGeCurve3d*	curve1				() const;
	const CGeCurve3d*	curve2				() const;
	void				getIntRanges		(CGeInterval& range1, CGeInterval& range2) const;
	CGeVector3d			planeNormal			() const;
	CGeTolerance		tolerance			() const;
	int					numIntPoints		() const;
	CGePoint3d			intPoint			(int intNum) const;
	void				getIntParams		(int intNum, double& param1, double& param2) const;
	void				getPointOnCurve1	(int intNum, CGePointOnCurve3d& pointOnCurve) const;
	void				getPointOnCurve2	(int intNum, CGePointOnCurve3d& pointOnCurve) const;
	void				getIntConfigs		(int intNum, CGe::EGeXConfig& config1to2, CGe::EGeXConfig& config2to1) const;
	bool				isTangential		(int intNum) const;
	bool				isTransversal		(int intNum) const;
	double				intPointTol			(int intNum) const;
	int					overlapCount		() const;
	bool				overlapDirection	() const;
	void				getOverlapRanges	(int overlapNum, CGeInterval& range1, CGeInterval& range2) const;
	void				changeCurveOrder	(); 
	CGeCurveCurveInt3d&	orderWrt1			();
	CGeCurveCurveInt3d&	orderWrt2			();
	CGeCurveCurveInt3d&	set					(const CGeCurve3d& curve1, const CGeCurve3d& curve2, const CGeVector3d& planeNormal = CGeVector3d::s_identity, const CGeTolerance& tol = CGeContext::s_defTol);
	CGeCurveCurveInt3d&	set					(const CGeCurve3d& curve1, const CGeCurve3d& curve2, const CGeInterval& range1, const CGeInterval& range2, const CGeVector3d& planeNormal = CGeVector3d::s_identity, const CGeTolerance& tol = CGeContext::s_defTol);
	CGeCurveCurveInt3d&	operator =			(const CGeCurveCurveInt3d& source);

private:
	CGeCurve3d		*m_pCurve1, *m_pCurve2;
	CGeVector3d		m_normal;
	CGeTolerance	m_tolerance;
	bool			m_bTangential;
	bool			m_bTransversal;
};
