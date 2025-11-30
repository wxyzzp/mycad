// File: GeRay3d.h
//
// CGeRay3d
//

#pragma once


#include "GeLinearCurve3d.h"


class GE_API CGeRay3d : public CGeLinearCurve3d
{
public:
	CGeRay3d();
	CGeRay3d(const CGeRay3d& source);
	CGeRay3d(const CGePoint3d& point, const CGeVector3d& vector);
	CGeRay3d(const CGePoint3d& point1, const CGePoint3d& point2);

	CGeRay3d&	set			(const CGePoint3d& point, const CGeVector3d& vector);
	CGeRay3d&	set			(const CGePoint3d& point1, const CGePoint3d& point2);
	CGeRay3d&	operator =	(const CGeRay3d& source);
};
