// File: GeEllipArc3d.h
//
// CGeEllipArc3d
//

#pragma once


#include "GeCurve3d.h"


class GE_API CGeCircArc3d;
class GE_API CGeLinearCurve3d;


class GE_API CGeEllipArc3d : public CGeCurve3d
{
public:
	CGeEllipArc3d();
	CGeEllipArc3d(const CGeEllipArc3d& source);
	CGeEllipArc3d(const CGeCircArc3d& sourceCircArc);
	CGeEllipArc3d(const CGePoint3d& center, const CGeVector3d& majorAxis, const CGeVector3d& minorAxis, double majorRadius, double minorRadius);
	CGeEllipArc3d(const CGePoint3d& center, const CGeVector3d& majorAxis, const CGeVector3d& minorAxis, double majorRadius, double minorRadius, double startAngle, double endAngle);

	bool			intersectWith		(const CGeLinearCurve3d& linearCurve, int& intNum, CGePoint3d& point1, CGePoint3d& point2, const CGeTolerance& = CGeContext::s_defTol) const;
	bool			intersectWith		(const CGePlanarSurface& planarSurface, int& intNum, CGePoint3d& point1, CGePoint3d& point2, const CGeTolerance& = CGeContext::s_defTol) const;
	bool			projIntersectWith	(const CGeLinearCurve3d& linearCurve, const CGeVector3d& projDir, int &numInt, CGePoint3d& pointOnArc1, CGePoint3d& pointOnArc2, CGePoint3d& pointOnLine1, CGePoint3d& pointOnLine2, const CGeTolerance& = CGeContext::s_defTol) const;
	CGePoint3d		closestPointToPlane	(const CGePlanarSurface& planarSurface, CGePoint3d& pointOnPlane, const CGeTolerance& = CGeContext::s_defTol) const;
	bool			isCircular			(const CGeTolerance& = CGeContext::s_defTol) const;
	bool			isInside			(const CGePoint3d& point, const CGeTolerance& = CGeContext::s_defTol) const;
	CGePoint3d		center				() const;
	double			minorRadius			() const;
	double			majorRadius			() const;
	CGeVector3d		minorAxis			() const;
	CGeVector3d		majorAxis			() const;
	CGeVector3d		normal				() const;
	void			getPlane			(CGePlane& plane) const;
	double			startAng			() const;
	double			endAng				() const;
	CGePoint3d		startPoint			() const;
	CGePoint3d		endPoint			() const;
	CGeEllipArc3d&	setCenter			(const CGePoint3d& center);
	CGeEllipArc3d&	setMinorRadius		(double radius);
	CGeEllipArc3d&	setMajorRadius		(double radius);
	CGeEllipArc3d&	setAxes				(const CGeVector3d& majorAxis, const CGeVector3d& minorAxis);
	CGeEllipArc3d&	setAngles			(double startAngle, double endAngle);
	CGeEllipArc3d&	set					(const CGePoint3d& center, const CGeVector3d& majorAxis, const CGeVector3d& minorAxis, double majorRadius, double minorRadius);
	CGeEllipArc3d&	set					(const CGePoint3d& center, const CGeVector3d& majorAxis, const CGeVector3d& minorAxis, double majorRadius, double minorRadius, double startAngle, double endAngle);
	CGeEllipArc3d&	set					(const CGeCircArc3d& sourceCircArc);
	CGeEllipArc3d&	operator =			(const CGeEllipArc3d& source);
};
