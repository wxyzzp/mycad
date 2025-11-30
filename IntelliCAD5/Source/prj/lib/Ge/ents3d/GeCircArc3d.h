// File: GeCircArc3d.h
//
// CGeCircArc3d
//

#pragma once


#include "GeCurve3d.h"
#include "GeVector3d.h"


class GE_API CGeLinearCurve3d;
class GE_API CGePlanarSurface;
class GE_API CGeError;


class GE_API CGeCircArc3d : public CGeCurve3d
{
public:
	CGeCircArc3d();
	CGeCircArc3d(const CGeCircArc3d& source);
	CGeCircArc3d(const CGePoint3d& startPoint, const CGePoint3d& pointOnArc, const CGePoint3d& endPoint);
	CGeCircArc3d(const CGePoint3d& center, const CGeVector3d& normal, double radius);
	CGeCircArc3d(const CGePoint3d& center, const CGeVector3d& normal, const CGeVector3d& refVec, double radius, double startAngle, double endAngle);

	CGePoint3d		closestPointToPlane	(const CGePlanarSurface& planarSurface, CGePoint3d& pointOnPlane, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			intersectWith		(const CGeLinearCurve3d& linearCurve, int& intNum, CGePoint3d& point1, CGePoint3d& point2, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			intersectWith		(const CGeCircArc3d& arc, int& intNum, CGePoint3d& point1, CGePoint3d& point2, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			intersectWith		(const CGePlanarSurface& planarSurface, int& intNum, CGePoint3d& point1, CGePoint3d& point2, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			projIntersectWith	(const CGeLinearCurve3d& linearCurve, const CGeVector3d& projDir, int& numInt, CGePoint3d& pointOnArc1, CGePoint3d& pointOnArc2, CGePoint3d& pointOnLine1, CGePoint3d& pointOnLine2, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			tangent				(const CGePoint3d& point, CGeLine3d& line, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			tangent				(const CGePoint3d& point, CGeLine3d& line, const CGeTolerance& tol, CGeError& error) const;
	bool			isInside			(const CGePoint3d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGePoint3d		center				() const;
	CGeVector3d		refVec				() const;
	double			radius				() const;
	double			startAng			() const;
	double			endAng				() const;
	CGePoint3d		startPoint			() const;
	CGePoint3d		endPoint			() const;
	CGeVector3d		normal				() const;
	void			getPlane			(CGePlane& plane) const;
	CGeCircArc3d&	setCenter			(const CGePoint3d& center);
	CGeCircArc3d&	setRadius			(double radius);
	CGeCircArc3d&	setAngles			(double startAngle, double endAngle);
	CGeCircArc3d&	setAxes				(const CGeVector3d& normal, const CGeVector3d& refVec);
	CGeCircArc3d&	set					(const CGePoint3d& startPoint, const CGePoint3d& pointOnArc, const CGePoint3d& endPoint);
	CGeCircArc3d&	set					(const CGePoint3d& startPoint, const CGePoint3d& pointOnArc, const CGePoint3d& endPoint, CGeError& error);
	CGeCircArc3d&	set					(const CGeCurve3d& curve1, const CGeCurve3d& curve2, double radius, double& param1, double& param2, bool& bSuccess);
	CGeCircArc3d&	set					(const CGeCurve3d& curve1, const CGeCurve3d& curve2, const CGeCurve3d& curve3, double& param1, double& param2, double& param3, bool& bSuccess);
	CGeCircArc3d&	set					(const CGePoint3d& center, const CGeVector3d& normal, double radius);
	CGeCircArc3d&	set					(const CGePoint3d& center, const CGeVector3d& normal, const CGeVector3d& refVec, double radius, double startAngle, double endAngle);
	CGeCircArc3d&	operator =			(const CGeCircArc3d& source);

private:
	CGePoint3d	m_center;
	CGeVector3d	m_refVector;
	CGeVector3d	m_normal;
	double		m_radius;
	double		m_startAngle, m_endAngle;
};
