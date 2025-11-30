// File: GeSplineCurve3d.h
//
// CGeSplineCurve3d
//

#pragma once


#include "GeCurve3d.h"
#include "GeSplineCurveBase.h"


class GE_API CGeSplineCurve3d : public CGeCurve3d, public CGeSplineCurveBase
{
public:
	virtual ~CGeSplineCurve3d();

    CGePoint3d			startPoint			() const;
    CGePoint3d			endPoint			() const;
    CGeSplineCurve3d&	setKnotAt			(int i, double value);
    CGePoint3d			controlPointAt		(int i) const;
    CGeSplineCurve3d&	setControlPointAt	(int i, const CGePoint3d& point);
    CGeSplineCurve3d&	operator =			(const CGeSplineCurve3d& source);

protected:
    CGeSplineCurve3d();
    CGeSplineCurve3d(const CGeSplineCurve3d& source);
};
