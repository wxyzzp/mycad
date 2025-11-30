// File: GeCurveBoundary
//
// CGeCurveBoundary
//

#pragma once


#include "GePch.h"


class GE_API CGeEntity3d;
class GE_API CGeCurve2d;
class GE_API CGePosition3d;

     
class GE_API CGeCurveBoundary
{
public:
	CGeCurveBoundary();
	CGeCurveBoundary(int numberOfCurves, const CGeEntity3d* const* curve3d, const CGeCurve2d* const* curve2d, bool* pbOrientation3d, bool* pbOrientation2d, bool bMakeCopy = true);
	CGeCurveBoundary(const CGeCurveBoundary& source);
	~CGeCurveBoundary();

	bool				isDegenerate	() const;
	bool				isDegenerate	(CGePosition3d& degenPoint, CGeCurve2d** paramCurve) const;
	int					numElements		() const;
	void				getContour		(int& n, CGeEntity3d*** curve3d, CGeCurve2d*** paramGeometry, bool** ppbOrientation3d, bool** ppbOrientation2d) const;
	CGeCurveBoundary&	set				(int numElements, const CGeEntity3d* const* curve3d, const CGeCurve2d* const* curve2d, bool* pbOrientation3d, bool* pbOrientation2d, bool bMakeCopy = true);
	bool				isOwnerOfCurves	() const;
	CGeCurveBoundary&	setToOwnCurves	();
	CGeCurveBoundary&	operator =		(const CGeCurveBoundary& source);

private:
	int		m_numElements;

/* original code:
protected:
	friend class AcGeImpCurveBoundary;

	AcGeImpCurveBoundary    *mpImpBnd;
	int                     mDelBnd;*/
};
