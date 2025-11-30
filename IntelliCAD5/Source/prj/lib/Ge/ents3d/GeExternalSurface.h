// File: GeExternalSurface.h
//
// CGeExternalSurface
//

#pragma once


#include "GeSurface.h"


class GE_API CGeExternalSurface : public CGeSurface
{
public:
	CGeExternalSurface();
	CGeExternalSurface(void* pSurfaceDef, CGe::EExternalEntityKind surfaceKind, bool bMakeCopy = true);
	CGeExternalSurface(const CGeExternalSurface& source);

	void						getExternalSurface	(void*& pSurfaceDef) const;
	CGe::EExternalEntityKind	externalSurfaceKind	() const;
	bool						isPlane				() const;
	bool						isSphere			() const;
	bool						isCylinder			() const;
	bool						isCone				() const;
	bool						isTorus				() const;
	bool						isNurbSurface		() const;
	bool						isDefined			() const;
	bool						isNativeSurface		(CGeSurface*& pNativeSurface) const;
	CGeExternalSurface&			set					(void* pSurfaceDef, CGe::EExternalEntityKind surfaceKind, bool bMakeCopy = true);
	bool						isOwnerOfSurface	() const;
	CGeExternalSurface&			setToOwnSurface		();
	CGeExternalSurface&			operator =			(const CGeExternalSurface& source);
};
