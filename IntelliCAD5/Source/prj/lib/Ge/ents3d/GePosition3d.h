// File: GePosition3d.h
//
// CGePosition3d
//

#pragma once


#include "GePointEntity3d.h"


class GE_API CGePosition3d : public CGePointEntity3d
{
public:
	CGePosition3d();
	CGePosition3d(const CGePoint3d& point);
	CGePosition3d(double x, double y, double z);
	CGePosition3d(const CGePosition3d& source);

	CGePosition3d&	set			(const CGePoint3d& point);
	CGePosition3d&	set			(double x, double y, double z);
	CGePosition3d&	operator =	(const CGePosition3d& source);
};
