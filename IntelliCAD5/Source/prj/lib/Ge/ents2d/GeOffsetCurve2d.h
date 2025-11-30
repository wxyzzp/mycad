// File: GeOffsetCurve2d.h
//
// CGeOffsetCurve2d
//

#pragma once


#include "GeCurve2d.h"


class GE_API CGeOffsetCurve2d : public CGeCurve2d
{
public:
	CGeOffsetCurve2d();
	CGeOffsetCurve2d(const CGeOffsetCurve2d& source);
	CGeOffsetCurve2d(const CGeCurve2d& baseCurve, double offsetDistance);

	const CGeCurve2d*	curve				() const;
	double				offsetDistance		() const;
	bool				paramDirection		() const;
	CGeMatrix2d			transformation		() const;
	CGeOffsetCurve2d&	setCurve			(const CGeCurve2d& baseCurve);
	CGeOffsetCurve2d&	setOffsetDistance	(double offsetDistance);
	CGeOffsetCurve2d&	operator =			(const CGeOffsetCurve2d& source);

private:
	CGeCurve2d*	m_pCurve;
	double		m_offset;
};
