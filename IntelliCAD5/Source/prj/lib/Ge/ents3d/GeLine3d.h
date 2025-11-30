// File: GeLine3d.h
//
// CGeLine3d
//

#pragma once


#include "GeLinearCurve3d.h"


class GE_API CGeLine3d : public CGeLinearCurve3d
{
public:
	CGeLine3d();
	CGeLine3d(const CGeLine3d& source);
	CGeLine3d(const CGePoint3d& point, const CGeVector3d& vector);
	CGeLine3d(const CGePoint3d& point1, const CGePoint3d& point2);

	CGeLine3d&	set			(const CGePoint3d& point, const CGeVector3d& vector);
	CGeLine3d&	set			(const CGePoint3d& point1, const CGePoint3d& point2);
	CGeLine3d&	operator =	(const CGeLine3d& source);

	static const CGeLine3d	s_xAxis;
	static const CGeLine3d	s_yAxis;
	static const CGeLine3d	s_zAxis;
};

