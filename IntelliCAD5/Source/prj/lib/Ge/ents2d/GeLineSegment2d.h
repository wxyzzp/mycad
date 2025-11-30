// File: GeLineSegment2d.h
//
// CGeLineSegment2d
//

#pragma once


#include "GeLinearCurve2d.h"


class GE_API CGeLineSegment2d : public CGeLinearCurve2d
{
public:
	CGeLineSegment2d();
	CGeLineSegment2d(const CGeLineSegment2d& source);
	CGeLineSegment2d(const CGePoint2d& point, const CGeVector2d& vector);
	CGeLineSegment2d(const CGePoint2d& point1, const CGePoint2d& point2);

	CGePoint2d			baryComb	(double blendCoeff) const;
	CGePoint2d			startPoint	() const;
	CGePoint2d			midPoint	() const;
	CGePoint2d			endPoint	() const;
	double				length		() const;
	double				length		(double fromParam, double toParam, double tol = CGeContext::s_defTol.equalPoint()) const;
	CGeLineSegment2d&	set			(const CGePoint2d& point, const CGeVector2d& vector);
	CGeLineSegment2d&	set			(const CGePoint2d& point1, const CGePoint2d& point2);
	CGeLineSegment2d&	set			(const CGeCurve2d& curve1, const CGeCurve2d& curve2, double& param1, double& param2, bool& bSuccess);
	CGeLineSegment2d&	set			(const CGeCurve2d& curve, const CGePoint2d& point, double& param, bool& bSuccess);
	void				getBisector	(CGeLine2d& line) const;
	CGeLineSegment2d&	operator =	(const CGeLineSegment2d& source);
};
