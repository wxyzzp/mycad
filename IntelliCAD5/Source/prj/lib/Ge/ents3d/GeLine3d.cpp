// File: GeLine3d.cpp
//

#include "GeLine3d.h"

//////////////////////////////////////////////////
// CGeLine3d class implementation
//////////////////////////////////////////////////

const CGeLine3d	CGeLine3d::s_xAxis;
const CGeLine3d	CGeLine3d::s_yAxis;
const CGeLine3d	CGeLine3d::s_zAxis;


CGeLine3d::CGeLine3d()
{
}

CGeLine3d::CGeLine3d(const CGeLine3d& source)
{
}

CGeLine3d::CGeLine3d(const CGePoint3d& point, const CGeVector3d& vector)
{
}

CGeLine3d::CGeLine3d(const CGePoint3d& point1, const CGePoint3d& point2)
{
}

CGeLine3d&
CGeLine3d::set(const CGePoint3d& point, const CGeVector3d& vector)
{
	return *this;
}

CGeLine3d&
CGeLine3d::set(const CGePoint3d& point1, const CGePoint3d& point2)
{
	return *this;
}

CGeLine3d&
CGeLine3d::operator = (const CGeLine3d& source)
{
	return *this;
}
