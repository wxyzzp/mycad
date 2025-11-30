// File: GeSurfaceSurfaceInt.h
//
// CGeSurfaceSurfaceInt
//

#pragma once


#include "GeSurface.h"


class GE_API CGeCurve2d;
class GE_API CGeCurve3d;


class GE_API CGeSurfaceSurfaceInt : public CGeEntity3d
{
public:
	CGeSurfaceSurfaceInt();
	CGeSurfaceSurfaceInt(const CGeSurface& surf1, const CGeSurface& surf2, const CGeTolerance& tol = CGeContext::s_defTol);
	CGeSurfaceSurfaceInt(const CGeSurfaceSurfaceInt& source);

	const CGeSurface*		surface1			() const;
	const CGeSurface*		surface2			() const;
	CGeTolerance			tolerance			() const;
	int						numResults			(CGe::EGeIntersectError& error) const;
	CGeCurve3d*				intCurve			(int intNum, bool bIsExternal, CGe::EGeIntersectError& error) const; 
	CGeCurve2d*				intParamCurve		(int num, bool bIsExternall, bool bIsFirst, CGe::EGeIntersectError& error) const;
	CGePoint3d				intPoint			(int intNum, CGe::EGeIntersectError& error) const;
	void					getIntPointParams	(int intNum, CGePoint2d& paramPoint1, CGePoint2d& paramPoint2, CGe::EGeIntersectError& error) const;
	void					getIntConfigs		(int intNum, CGe::EssiConfig& surf1Left,  CGe::EssiConfig& surf1Right, CGe::EssiConfig& surf2Left, CGe::EssiConfig& surf2Right, CGe::EssiType& intType, int& dim, CGe::EGeIntersectError& error) const;
	int						getDimension		(int intNum, CGe::EGeIntersectError& error) const;
	CGe::EssiType			getType				(int intNum, CGe::EGeIntersectError& error) const;
	CGeSurfaceSurfaceInt&	set					(const CGeSurface& surf1, const CGeSurface& surf2, const CGeTolerance& tol = CGeContext::s_defTol);
	CGeSurfaceSurfaceInt&	operator =			(const CGeSurfaceSurfaceInt& source);

private:
	CGeSurface		*m_pSurface1, *m_pSurface2;
	CGeTolerance	m_tolerance;
};
