// File: GeRay2d.h
//
// CGeRay2d
//

#pragma once


#include "GeLinearCurve2d.h"


class GE_API CGeRay2d : public CGeLinearCurve2d
{
public:
	CGeRay2d();
	CGeRay2d(const CGeRay2d& source);
	CGeRay2d(const CGePoint2d& point, const CGeVector2d& vector);
	CGeRay2d(const CGePoint2d& point1, const CGePoint2d& point2);

	CGeRay2d&	set			(const CGePoint2d& point, const CGeVector2d& vector);
	CGeRay2d&	set			(const CGePoint2d& point1, const CGePoint2d& point2);
	CGeRay2d&	operator =	(const CGeRay2d& source);
};
