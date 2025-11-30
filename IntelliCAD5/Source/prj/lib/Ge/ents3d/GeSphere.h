// File: GeSphere.h
//
// CGeSphere
//

#pragma once


#include "GeSurface.h"


class GE_API CGeLinearCurve3d;


class GE_API CGeSphere : public CGeSurface
{
public:
	CGeSphere();
	CGeSphere(double radius, const CGePoint3d& center);
	CGeSphere(double radius, const CGePoint3d& center, const CGeVector3d& northAxis, const CGeVector3d& refAxis, double startAngleU, double endAngleU, double startAngleV, double endAngleV);
	CGeSphere(const CGeSphere& source);

	double		radius			() const;
	CGePoint3d	center			() const;
	void		getAnglesInU	(double& startAngle, double& endAngle) const;
	void		getAnglesInV	(double& startAngle, double& endAngle) const;
	CGeVector3d	northAxis		() const;
	CGeVector3d	refAxis			() const;
	CGePoint3d	northPole		() const;
	CGePoint3d	southPole		() const;
	bool		isOuterNormal	() const;
	bool		isClosed		(const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGeSphere&	setRadius		(double radius);
	CGeSphere&	setAnglesInU	(double startAngle, double endAngle);
	CGeSphere&	setAnglesInV	(double startAngle, double endAngle);
	CGeSphere&	set				(double radius, const CGePoint3d& center);
	CGeSphere&	set				(double radius, const CGePoint3d& center, const CGeVector3d& northAxis, const CGeVector3d& refAxis, double startAngleU, double endAngleU, double startAngleV, double endAngleV);
	bool		intersectWith	(const CGeLinearCurve3d& linearCurve, int& intNum, CGePoint3d& point1, CGePoint3d& point2, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGeSphere&	operator =		(const CGeSphere& source);

private:
	CGePoint3d	m_center;
	double		m_radius;
};
