// File: GeLine2d.h
//
// CGeLine2d
//

#pragma once


#include "GeLinearCurve2d.h"


class GE_API CGeLine2d : public CGeLinearCurve2d
{
public:
	CGeLine2d();
	CGeLine2d(const CGeLine2d& source);
	CGeLine2d(const CGePoint2d& point, const CGeVector2d& vector);
	CGeLine2d(const CGePoint2d& point1, const CGePoint2d& point2);

	CGeLine2d&	set			(const CGePoint2d& point, const CGeVector2d& vector);
	CGeLine2d&	set			(const CGePoint2d& point1, const CGePoint2d& point2);
	CGeLine2d&	operator =	(const CGeLine2d& source);

	static const CGeLine2d	s_xAxis;
	static const CGeLine2d	s_yAxis;
};
