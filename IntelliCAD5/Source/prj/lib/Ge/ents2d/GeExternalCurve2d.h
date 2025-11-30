// File: GeExternalCurve2d.h
//
// CGeExternalCurve2d
//

#pragma once


#include "GeCurve2d.h"


class GE_API CGeNurbCurve2d;


class GE_API CGeExternalCurve2d : public CGeCurve2d
{
public:
	CGeExternalCurve2d();
	CGeExternalCurve2d(const CGeExternalCurve2d& source);
	CGeExternalCurve2d(void* pCurveDef, CGe::EExternalEntityKind curveKind, bool bMakeCopy = true);

	bool						isNurbCurve			() const;
    bool						isNurbCurve			(CGeNurbCurve2d& nurbCurve) const;
	bool						isDefined			() const;
	void						getExternalCurve	(void*& pCurveDef) const;
	CGe::EExternalEntityKind	externalCurveKind	() const;
	CGeExternalCurve2d&			set					(void* pCurveDef, CGe::EExternalEntityKind curveKind, bool bMakeCopy = true);
	bool						isOwnerOfCurve		() const;
	CGeExternalCurve2d&			setToOwnCurve		();
	CGeExternalCurve2d&			operator =			(const CGeExternalCurve2d& source);
};
