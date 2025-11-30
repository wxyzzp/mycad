// File: GeSplineCurve2d.h
//
// CGeSplineCurve2d
//

#pragma once


#include "GeCurve2d.h"
#include "GeSplineCurveBase.h"


class GE_API CGeSplineCurve2d : public CGeCurve2d, public CGeSplineCurveBase
{
public:
	virtual ~CGeSplineCurve2d();

    CGePoint2d			startPoint			() const;
    CGePoint2d			endPoint			() const;
    CGeSplineCurve2d&	setKnotAt			(int i, double value);
    CGePoint2d			controlPointAt		(int i) const;
    CGeSplineCurve2d&	setControlPointAt	(int i, const CGePoint2d& point);
    CGeSplineCurve2d&	operator =			(const CGeSplineCurve2d& source);

protected:
    CGeSplineCurve2d();
    CGeSplineCurve2d(const CGeSplineCurve2d& source);
};
