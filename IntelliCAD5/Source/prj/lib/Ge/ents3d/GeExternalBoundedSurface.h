// File: GeExternalBoundedSurface.h
//
// CGeExternalBoundedSurface
//

#pragma once


#include "GeSurface.h"


class GE_API CGeExternalSurface;
class GE_API CGeCurveBoundary;


class GE_API CGeExternalBoundedSurface : public CGeSurface
{
public:
	CGeExternalBoundedSurface();
	CGeExternalBoundedSurface(void* pSurfaceDef, CGe::EExternalEntityKind surfaceKind, bool bMakeCopy = true);
	CGeExternalBoundedSurface(const CGeExternalBoundedSurface& source);

	CGe::EExternalEntityKind	externalSurfaceKind	() const;
	bool						isDefined			() const;
	void						getExternalSurface	(void*& pSurfaceDef) const;
	void						getBaseSurface		(CGeSurface*& pSurfaceDef) const;
	void						getBaseSurface		(CGeExternalSurface& unboundedSurfaceDef) const;
	bool						isPlane				() const;
	bool						isSphere			() const;
	bool						isCylinder			() const;
	bool						isCone				() const;
	bool						isTorus				() const;
	bool						isNurbs				() const;
	bool						isExternalSurface	() const;
	int							numContours			() const;
	void						getContours			(int& numContours, CGeCurveBoundary*& pCurveBoundaries) const;
	CGeExternalBoundedSurface&	set					(void* pSurfaceDef, CGe::EExternalEntityKind surfaceKind, bool bMakeCopy = true);
	bool						isOwnerOfSurface	() const;
	CGeExternalBoundedSurface&	setToOwnSurface		();
	CGeExternalBoundedSurface&	operator =			(const CGeExternalBoundedSurface& source);
};
