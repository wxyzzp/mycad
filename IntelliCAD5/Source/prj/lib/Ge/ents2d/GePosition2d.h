// File: GePosition2d.h
//
// CGePosition2d
//

#pragma once


#include "GePointEntity2d.h"


class GE_API CGePosition2d : public CGePointEntity2d
{
public:
	CGePosition2d();
	CGePosition2d(const CGePoint2d& point);
	CGePosition2d(double x, double y);
	CGePosition2d(const CGePosition2d& source);

	CGePosition2d&	set			(const CGePoint2d& point);
	CGePosition2d&	set			(double x, double y);
	CGePosition2d&	operator =	(const CGePosition2d& source);
};
