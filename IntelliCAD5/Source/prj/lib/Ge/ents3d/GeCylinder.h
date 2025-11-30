// File: GeCylinder.h
//
// CGeCylinder
//

#pragma once


#include "GeSurface.h"


class GE_API CGeLinearCurve3d;


class GE_API CGeCylinder : public CGeSurface
{
public:
	CGeCylinder();
	CGeCylinder(double radius, const CGePoint3d& origin, const CGeVector3d& axisOfSymmetry);
	CGeCylinder(double radius, const CGePoint3d& origin, const CGeVector3d& axisOfSymmetry, const CGeVector3d& refVector, const CGeInterval& height, double startAngle, double endAngle);
	CGeCylinder(const CGeCylinder& source);

	double			radius			() const;
	CGePoint3d		origin			() const;
	void			getAngles		(double& startAngle, double& endAngle) const;
	void			getHeight		(CGeInterval& height) const;
	double			heightAt		(double u) const;
	CGeVector3d		axisOfSymmetry	() const;
	CGeVector3d		refAxis			() const;
	bool			isOuterNormal	() const;
	bool			isClosed		(const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGeCylinder&	setRadius		(double radius);
	CGeCylinder&	setAngles		(double startAngle, double endAngle);
	CGeCylinder&	setHeight		(const CGeInterval& height);
	CGeCylinder&	set				(double radius, const CGePoint3d& origin, const CGeVector3d& axisOfSym);
	CGeCylinder&	set				(double radius, const CGePoint3d& origin, const CGeVector3d& axisOfSym, const CGeVector3d& refVector, const CGeInterval& height, double startAngle, double endAngle);
	bool			intersectWith	(const CGeLinearCurve3d& linearCurve, int& intNum, CGePoint3d& point1, CGePoint3d& point2, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGeCylinder&	operator =		(const CGeCylinder& source);

private:
	double	m_radius;
};
