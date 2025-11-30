// File: GeOffsetCurve3d.h
//
// CGeOffsetCurve3d
//

#pragma once


#include "GeCurve3d.h"
#include "GeVector3d.h"


class GE_API CGeOffsetCurve3d : public CGeCurve3d
{
public:
	CGeOffsetCurve3d();
	CGeOffsetCurve3d(const CGeOffsetCurve3d& source);
	CGeOffsetCurve3d(const CGeCurve3d& baseCurve, const CGeVector3d& planeNormal, double offsetDistance);

	const CGeCurve3d*	curve				() const;
	double				offsetDistance		() const;
	bool				paramDirection		() const;
	CGeMatrix3d			transformation		() const;
	CGeOffsetCurve3d&	setCurve			(const CGeCurve3d& baseCurve);
	CGeOffsetCurve3d&	setOffsetDistance	(double offsetDistance);
	CGeVector3d			normal				() const; 
	CGeOffsetCurve3d&	setNormal			(const CGeVector3d& planeNormal);
	CGeOffsetCurve3d&	operator =			(const CGeOffsetCurve3d& source);

private:
	CGeCurve3d*	m_pCurve;
	double		m_offset;
	CGeVector3d	m_normal;
};
