// File: GeCircArc2d.h
//
// CGeCircArc2d
//

#pragma once


#include "GeCurve2d.h"
#include "GeVector2d.h"


class GE_API CGeLinearCurve2d;
class GE_API CGeError;


class GE_API CGeCircArc2d : public CGeCurve2d
{
public:
	CGeCircArc2d();
	CGeCircArc2d(const CGeCircArc2d& source);
	CGeCircArc2d(const CGePoint2d& center, double radius);
	CGeCircArc2d(const CGePoint2d& startPoint, const CGePoint2d& pointOnArc, const CGePoint2d& endPoint);
	CGeCircArc2d(const CGePoint2d& center, double radius, double startAngle, double endAngle, const CGeVector2d& refVec = CGeVector2d::s_xAxis, bool bIsClockWise = false);
	CGeCircArc2d(const CGePoint2d& startPoint, const CGePoint2d& endPoint, double bulge, bool bBulgeFlag = true);

	bool			intersectWith	(const CGeLinearCurve2d& linearCurve, int& intNum, CGePoint2d& point1, CGePoint2d& point2, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			intersectWith	(const CGeCircArc2d& arc, int& intNum, CGePoint2d& point1, CGePoint2d& point2, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			tangent			(const CGePoint2d& point, CGeLine2d& line, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			tangent			(const CGePoint2d& point, CGeLine2d& line, const CGeTolerance& tol, CGeError& error) const;
	bool			isInside		(const CGePoint2d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGePoint2d		center			() const;
	CGeVector2d		refVec			() const;
	double			radius			() const;
	double			startAng		() const;
	double			endAng			() const;
	CGePoint2d		startPoint		() const;
	CGePoint2d		endPoint		() const;
    bool			isClockWise		() const;
	CGeCircArc2d&	setCenter		(const CGePoint2d& center);
	CGeCircArc2d&	setRadius		(double radius);
	CGeCircArc2d&	setAngles		(double startAngle, double endAngle);
    CGeCircArc2d&	setRefVec		(const CGeVector2d& vector);
    CGeCircArc2d&	setToComplement	();
	CGeCircArc2d&	set				(const CGePoint2d& startPoint, const CGePoint2d& pointOnArc, const CGePoint2d& endPoint);
	CGeCircArc2d&	set				(const CGePoint2d& startPoint, const CGePoint2d& pointOnArc, const CGePoint2d& endPoint, CGeError& error);
	CGeCircArc2d&	set				(const CGeCurve2d& curve1, const CGeCurve2d& curve2, double radius, double& param1, double& param2, bool& bSuccess);
	CGeCircArc2d&	set				(const CGeCurve2d& curve1, const CGeCurve2d& curve2, const CGeCurve2d& curve3, double& param1, double& param2, double& param3, bool& bSuccess);
	CGeCircArc2d&	set				(const CGePoint2d& center, double radius);
	CGeCircArc2d&	set				(const CGePoint2d& center, double radius, double startAngle, double endAngle, const CGeVector2d& refVec = CGeVector2d::s_xAxis, bool bIsClockWise = false);
    CGeCircArc2d&	set				(const CGePoint2d& startPoint, const CGePoint2d& endPoint, double bulge, bool bBulgeFlag = true);
	CGeCircArc2d&	operator =		(const CGeCircArc2d& source);

private:
	CGePoint2d	m_center;
	CGeVector2d	m_refVector;
	double		m_radius;
	double		m_startAngle, m_endAngle;
	bool		m_bIsClockWise;
};
