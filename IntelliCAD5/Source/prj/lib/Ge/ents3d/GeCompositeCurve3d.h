// File: GeCompositeCurve3d.h
//
// CGeCompositeCurve3d
//

#pragma once


#include "GeCurve3d.h"


class GE_API CGeCompositeCurve3d : public CGeCurve3d
{
public:
	CGeCompositeCurve3d();
	CGeCompositeCurve3d(const CGeVoidPointerArray& curveList);
	CGeCompositeCurve3d(const CGeVoidPointerArray& curveList, const CGeIntArray& isOwnerOfCurves);
	CGeCompositeCurve3d(const CGeCompositeCurve3d& source);

	void					getCurveList		(CGeVoidPointerArray& curveList) const;
	CGeCompositeCurve3d&	setCurveList		(const CGeVoidPointerArray& curveList);
	CGeCompositeCurve3d&	setCurveList		(const CGeVoidPointerArray& curveList, const CGeIntArray& isOwnerOfCurves);
	double					globalToLocalParam	(double param, int& segNum) const; 
	double					localToGlobalParam	(double param, int segNum) const; 
	CGeCompositeCurve3d&	operator =			(const CGeCompositeCurve3d& source);
};
