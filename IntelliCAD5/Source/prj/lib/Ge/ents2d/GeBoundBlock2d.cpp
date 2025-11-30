// File: GeBoundBlock2d.cpp
//

#include "GeBoundBlock2d.h"

//////////////////////////////////////////////////
// CGeBoundBlock2d class implementation
//////////////////////////////////////////////////

CGeBoundBlock2d::CGeBoundBlock2d()
{
}

CGeBoundBlock2d::CGeBoundBlock2d(const CGeBoundBlock2d& source)
{
}

CGeBoundBlock2d::CGeBoundBlock2d(const CGePoint2d& point1, const CGePoint2d& point2)
{
}

CGeBoundBlock2d::CGeBoundBlock2d(const CGePoint2d& base, const CGeVector2d& dir1, const CGeVector2d& dir2)
{
}

void
CGeBoundBlock2d::getMinMaxPoints(CGePoint2d& point1, CGePoint2d& point2) const
{
}

void
CGeBoundBlock2d::get(const CGePoint2d& base, const CGeVector2d& dir1, const CGeVector2d& dir2) const
{
}

CGeBoundBlock2d&
CGeBoundBlock2d::set(const CGePoint2d& point1, const CGePoint2d& point2)
{
	return *this;
}

CGeBoundBlock2d&
CGeBoundBlock2d::set(const CGePoint2d& base, const CGeVector2d& dir1, const CGeVector2d& dir2)
{
	return *this;
}

CGeBoundBlock2d&
CGeBoundBlock2d::extend(const CGePoint2d& point)
{
	return *this;
}

CGeBoundBlock2d&
CGeBoundBlock2d::swell(double distance)
{
	return *this;
}

bool
CGeBoundBlock2d::contains(const CGePoint2d& point) const
{
	return false;
}

bool
CGeBoundBlock2d::isDisjoint(const CGeBoundBlock2d& block) const
{
	return false;
}

bool
CGeBoundBlock2d::isBox() const
{
	return false;
}

CGeBoundBlock2d&
CGeBoundBlock2d::setToBox(bool b)
{
	return *this;
}

CGeBoundBlock2d&
CGeBoundBlock2d::operator = (const CGeBoundBlock2d& source)
{
	return *this;
}
