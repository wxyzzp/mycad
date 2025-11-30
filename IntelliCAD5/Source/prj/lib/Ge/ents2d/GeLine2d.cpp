// File: GeLine2d.cpp
//

#include "GeLine2d.h"

//////////////////////////////////////////////////
// CGeLine2d class implementation
//////////////////////////////////////////////////

const CGeLine2d	CGeLine2d::s_xAxis;
const CGeLine2d	CGeLine2d::s_yAxis;


CGeLine2d::CGeLine2d()
{
}

CGeLine2d::CGeLine2d(const CGeLine2d& source)
{
}

CGeLine2d::CGeLine2d(const CGePoint2d& point, const CGeVector2d& vector)
{
}

CGeLine2d::CGeLine2d(const CGePoint2d& point1, const CGePoint2d& point2)
{
}

CGeLine2d&
CGeLine2d::set(const CGePoint2d& point, const CGeVector2d& vector)
{
	return *this;
}

CGeLine2d&
CGeLine2d::set(const CGePoint2d& point1, const CGePoint2d& point2)
{
	return *this;
}

CGeLine2d&
CGeLine2d::operator = (const CGeLine2d& source)
{
	return *this;
}
