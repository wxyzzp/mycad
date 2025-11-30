// File: GeCompositeCurve2d.h
//
// CGeCompositeCurve2d
//

#pragma once


#include "GeCurve2d.h"
#include "GeArrays.h"


class GE_API CGeCompositeCurve2d : public CGeCurve2d
{
public:
	CGeCompositeCurve2d();
	CGeCompositeCurve2d(const CGeVoidPointerArray& curveList);
	CGeCompositeCurve2d(const CGeVoidPointerArray& curveList, const CGeIntArray& isOwnerOfCurves);
	CGeCompositeCurve2d(const CGeCompositeCurve2d& source);

	void					getCurveList		(CGeVoidPointerArray& curveList) const;
	CGeCompositeCurve2d&	setCurveList		(const CGeVoidPointerArray& curveList);
	CGeCompositeCurve2d&	setCurveList		(const CGeVoidPointerArray& curveList, const CGeIntArray& isOwnerOfCurves);
	double					globalToLocalParam	(double param, int& segNum) const; 
	double					localToGlobalParam	(double param, int segNum) const; 
	CGeCompositeCurve2d&	operator =			(const CGeCompositeCurve2d& source);
};
