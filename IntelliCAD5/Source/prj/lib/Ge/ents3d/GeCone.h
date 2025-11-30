// File: GeCone.h
//
// CGeCone
//

#pragma once


#include "GeSurface.h"


class GE_API CGeLinearCurve3d;


class GE_API CGeCone : public CGeSurface
{
public:
	CGeCone();
	CGeCone(double cosAngle, double sinAngle, const CGePoint3d& baseOrigin, double baseRadius, const CGeVector3d& axisOfSymmetry);
	CGeCone(double cosAngle, double sinAngle, const CGePoint3d& baseOrigin, double baseRadius, const CGeVector3d& axisOfSymmetry, const CGeVector3d& refVector, const CGeInterval& height, double startAngle, double endAngle);
	CGeCone(const CGeCone& source);

	double		baseRadius		() const;
	CGePoint3d	baseCenter		() const;
	void		getAngles		(double& startAngle, double& endAngle) const;
	double		halfAngle		() const;
	void		getHalfAngle	(double& cosAngle, double& sinAngle) const;
	void		getHeight		(CGeInterval& height) const;
	double		heightAt		(double u) const;
	CGeVector3d	axisOfSymmetry	() const;
	CGeVector3d	refAxis			() const;
	CGePoint3d	apex			() const;
	bool		isClosed		(const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool		isOuterNormal	() const;
	CGeCone&	setBaseRadius	(double radius);
	CGeCone&	setAngles		(double startAngle, double endAngle);
	CGeCone&	setHeight		(const CGeInterval& height);
	CGeCone&	set				(double cosAngle, double sinAngle, const CGePoint3d& baseCenter, double baseRadius, const CGeVector3d& axisOfSymmetry);
	CGeCone&	set				(double cosAngle, double sinAngle, const CGePoint3d& baseCenter, double baseRadius, const CGeVector3d& axisOfSymmetry, const CGeVector3d& refVector, const CGeInterval& height, double startAngle, double endAngle);
	bool		intersectWith	(const CGeLinearCurve3d& linearCurve, int& intNum, CGePoint3d& point1, CGePoint3d& point2, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGeCone&	operator =		(const CGeCone& source);
};
