// File: GePointOnSurface.h
//
// CGePointOnSurface
//

#pragma once


#include "GePointEntity3d.h"
#include "GeSurface.h"
#include "GePoint2d.h"


class GE_API CGePointOnSurface : public CGePointEntity3d
{
public:
	CGePointOnSurface();
	CGePointOnSurface(const CGeSurface& surf);
	CGePointOnSurface(const CGeSurface& surf, const CGePoint2d& paramPoint);
	CGePointOnSurface(const CGePointOnSurface& source);

	const CGeSurface*	surface				() const;
	CGePoint2d			parameter			() const;
	CGePoint3d			point				() const;
	CGePoint3d			point				(const CGePoint2d& paramPoint);
	CGePoint3d			point				(const CGeSurface& surf, const CGePoint2d& paramPoint);
	CGeVector3d			normal				() const;
	CGeVector3d			normal				(const CGePoint2d& paramPoint);
	CGeVector3d			normal				(const CGeSurface& surf, const CGePoint2d& paramPoint);
	CGeVector3d			uDeriv				(int order) const;
	CGeVector3d			uDeriv				(int order, const CGePoint2d& paramPoint);
	CGeVector3d			uDeriv				(int order, const CGeSurface& surf, const CGePoint2d& paramPoint);
	CGeVector3d			vDeriv				(int order) const;
	CGeVector3d			vDeriv				(int order, const CGePoint2d& paramPoint);
	CGeVector3d			vDeriv				(int order, const CGeSurface& surf, const CGePoint2d& paramPoint);
	CGeVector3d			mixedPartial		() const;
	CGeVector3d			mixedPartial		(const CGePoint2d& paramPoint);
	CGeVector3d			mixedPartial		(const CGeSurface& surf, const CGePoint2d& paramPoint);
	CGeVector3d			tangentVector		(const CGeVector2d& vector) const;
	CGeVector3d			tangentVector		(const CGeVector2d& vector, const CGePoint2d& paramPoint);
	CGeVector3d			tangentVector		(const CGeVector2d& vector, const CGeSurface& surf, const CGePoint2d& paramPoint);
	CGeVector2d			inverseTangentVector(const CGeVector3d& vector) const;
	CGeVector2d			inverseTangentVector(const CGeVector3d& vector, const CGePoint2d& paramPoint);
	CGeVector2d			inverseTangentVector(const CGeVector3d& vector, const CGeSurface& surf, const CGePoint2d& paramPoint);
	CGePointOnSurface&	setSurface			(const CGeSurface& surf);
	CGePointOnSurface&	setParameter		(const CGePoint2d& paramPoint);
	CGePointOnSurface&	operator =			(const CGePointOnSurface& source);

private:
	CGeSurface*	m_pSurface;
	CGePoint2d	m_parameter;
};
