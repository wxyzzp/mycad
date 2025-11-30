// File: GeExternalCurve3d.h
//
// CGeExternalCurve3d
//

#pragma once


#include "GeCurve3d.h"


class GE_API CGeExternalCurve3d : public CGeCurve3d
{
public:
	CGeExternalCurve3d();
	CGeExternalCurve3d(const CGeExternalCurve3d& source);
	CGeExternalCurve3d(void* pCurveDef, CGe::EExternalEntityKind curveKind, bool bMakeCopy = true);

	bool						isLine				() const;
	bool						isRay				() const;
	bool						isLineSeg			() const;
	bool						isCircArc			() const;
	bool						isEllipArc			() const;
	bool						isNativeCurve		(CGeCurve3d*& pNativeCurve) const;
	bool						isNurbCurve			() const;
	bool						isDefined			() const;
	void						getExternalCurve	(void*& pCurveDef) const;
	CGe::EExternalEntityKind	externalCurveKind	() const;
	CGeExternalCurve3d&			set					(void* pCurveDef, CGe::EExternalEntityKind curveKind, bool bMakeCopy = true);
	bool						isOwnerOfCurve		() const;
	CGeExternalCurve3d&			setToOwnCurve		();
	CGeExternalCurve3d&			operator =			(const CGeExternalCurve3d& source);
};
