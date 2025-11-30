// File: GeTorus.h
//
// CGeTorus
//

#pragma once


#include "GeSurface.h"


class GE_API CGeLinearCurve3d;


class GE_API CGeTorus : public CGeSurface
{
public:
	CGeTorus();
	CGeTorus(double majorRadius, double minorRadius, const CGePoint3d& origin, const CGeVector3d& axisOfSymmetry);
	CGeTorus(double majorRadius, double minorRadius, const CGePoint3d& origin, const CGeVector3d& axisOfSymmetry, const CGeVector3d& refAxis, double startAngleU, double endAngleU, double startAngleV, double endAngleV);
	CGeTorus(const CGeTorus& source);

	double		majorRadius		() const;
	double		minorRadius		() const;
	void		getAnglesInU	(double& startAngle, double& endAngle) const;
	void		getAnglesInV	(double& startAngle, double& endAngle) const;
	CGePoint3d	center			() const;
	CGeVector3d	axisOfSymmetry	() const;
	CGeVector3d	refAxis			() const;
	bool		isOuterNormal	() const;
	CGeTorus&	setMajorRadius	(double radius);
	CGeTorus&	setMinorRadius	(double radius);
	CGeTorus&	setAnglesInU	(double startAngle, double endAngle);
	CGeTorus&	setAnglesInV	(double startAngle, double endAngle);
	CGeTorus&	set				(double majorRadius, double minorRadius, const CGePoint3d& center, const CGeVector3d& axisOfSymmetry);
	CGeTorus&	set				(double majorRadius, double minorRadius, const CGePoint3d&  center, const CGeVector3d& axisOfSymmetry, const CGeVector3d& refAxis, double startAngleU, double endAngleU, double startAngleV, double endAngleV);
	bool		intersectWith	(const CGeLinearCurve3d& linearCurve, int& intNum, CGePoint3d& point1, CGePoint3d& point2, CGePoint3d& point3, CGePoint3d& point4, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGeTorus&	operator =		(const CGeTorus& source);

private:
	CGePoint3d	m_center;
};
