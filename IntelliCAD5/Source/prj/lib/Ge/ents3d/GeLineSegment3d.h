// File: GeLineSegment3d.h
//
// CGeLineSegment3d
//

#pragma once


#include "GeLinearCurve3d.h"


class GE_API CGeLineSegment3d : public CGeLinearCurve3d
{
public:
	CGeLineSegment3d();
	CGeLineSegment3d(const CGeLineSegment3d& source);
	CGeLineSegment3d(const CGePoint3d& point, const CGeVector3d& vector);
	CGeLineSegment3d(const CGePoint3d& point1, const CGePoint3d& point2);

	CGePoint3d			baryComb	(double blendCoeff) const;
	CGePoint3d			startPoint	() const;
	CGePoint3d			midPoint	() const;
	CGePoint3d			endPoint	() const;
	double				length		() const;
	double				length		(double fromParam, double toParam, double tol = CGeContext::s_defTol.equalPoint()) const;
	CGeLineSegment3d&	set			(const CGePoint3d& point, const CGeVector3d& vector);
	CGeLineSegment3d&	set			(const CGePoint3d& point1, const CGePoint3d& point2);
	CGeLineSegment3d&	set			(const CGeCurve3d& curve1, const CGeCurve3d& curve2, double& param1, double& param2, bool& bSuccess);
	CGeLineSegment3d&	set			(const CGeCurve3d& curve, const CGePoint3d& point, double& param, bool& bSuccess);
	void				getBisector	(CGePlane& plane) const;
	CGeLineSegment3d&	operator =	(const CGeLineSegment3d& source);
};
