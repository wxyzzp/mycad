// File: GeEllipArc2d.h
//
// CGeEllipArc2d
//

#pragma once


#include "GeCurve2d.h"


class GE_API CGeCircArc2d;
class GE_API CGeLinearCurve2d;


class GE_API CGeEllipArc2d : public CGeCurve2d
{
public:
	CGeEllipArc2d();
	CGeEllipArc2d(const CGeEllipArc2d& source);
	CGeEllipArc2d(const CGeCircArc2d& sourceCircArc);
	CGeEllipArc2d(const CGePoint2d& center, const CGeVector2d& majorAxis, const CGeVector2d& minorAxis, double majorRadius, double minorRadius);
	CGeEllipArc2d(const CGePoint2d& center, const CGeVector2d& majorAxis, const CGeVector2d& minorAxis, double majorRadius, double minorRadius, double startAngle, double endAngle);

	bool			intersectWith		(const CGeLinearCurve2d& linearCurve, int& intNum, CGePoint2d& point1, CGePoint2d& point2, const CGeTolerance& = CGeContext::s_defTol) const;
	bool			isCircular			(const CGeTolerance& = CGeContext::s_defTol) const;
    bool			isClockWise			() const;
	bool			isInside			(const CGePoint2d& point, const CGeTolerance& = CGeContext::s_defTol) const;
	CGePoint2d		center				() const;
	double			minorRadius			() const;
	double			majorRadius			() const;
	CGeVector2d		minorAxis			() const;
	CGeVector2d		majorAxis			() const;
	CGeVector2d		normal				() const;
	double			startAng			() const;
	double			endAng				() const;
	CGePoint2d		startPoint			() const;
	CGePoint2d		endPoint			() const;
	CGeEllipArc2d&	setCenter			(const CGePoint2d& center);
	CGeEllipArc2d&	setMinorRadius		(double radius);
	CGeEllipArc2d&	setMajorRadius		(double radius);
	CGeEllipArc2d&	setAxes				(const CGeVector2d& majorAxis, const CGeVector2d& minorAxis);
	CGeEllipArc2d&	setAngles			(double startAngle, double endAngle);
	CGeEllipArc2d&	set					(const CGePoint2d& center, const CGeVector2d& majorAxis, const CGeVector2d& minorAxis, double majorRadius, double minorRadius);
	CGeEllipArc2d&	set					(const CGePoint2d& center, const CGeVector2d& majorAxis, const CGeVector2d& minorAxis, double majorRadius, double minorRadius, double startAngle, double endAngle);
	CGeEllipArc2d&	set					(const CGeCircArc2d& sourceCircArc);
	CGeEllipArc2d&	operator =			(const CGeEllipArc2d& source);
};
