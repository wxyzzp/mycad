// File: GeOffsetSurface.h
//
// CGeOffsetSurface
//

#pragma once


#include "GeSurface.h"


class GE_API CGeOffsetSurface : public CGeSurface
{
public:
	CGeOffsetSurface();
	CGeOffsetSurface(CGeSurface* pBaseSurface, double offsetDist, bool bMakeCopy = true);
	CGeOffsetSurface(const CGeOffsetSurface& source);

	bool				isPlane					() const;
	bool				isBoundedPlane			() const;
	bool				isSphere				() const;
	bool				isCylinder				() const;
	bool				isCone					() const;
	bool				isTorus					() const;
	bool				getSurface				(CGeSurface*& pSurf) const;
	void				getConstructionSurface	(CGeSurface*& pBaseSurf) const;
	double				offsetDist				() const;
	CGeOffsetSurface&	set						(CGeSurface* pBaseSurf, double offsetDist, bool bMakeCopy = true);
	CGeOffsetSurface&	operator =  (const CGeOffsetSurface& offset);

private:
	CGeSurface*	m_pSurface;
	double		m_offset;
};
