// File: GeBoundBlock3d.cpp
//

#include "GeBoundBlock3d.h"

//////////////////////////////////////////////////
// CGeBoundBlock3d class implementation
//////////////////////////////////////////////////

CGeBoundBlock3d::CGeBoundBlock3d()
{
}

CGeBoundBlock3d::CGeBoundBlock3d(const CGePoint3d& point1, const CGePoint3d& point2)
{
}

CGeBoundBlock3d::CGeBoundBlock3d(const CGeBoundBlock3d& source)
{
}

void
CGeBoundBlock3d::getMinMaxPoints(CGePoint3d& point1, CGePoint3d& point2) const
{
}

void
CGeBoundBlock3d::get(const CGePoint3d& base, const CGeVector3d& dir1, const CGeVector3d& dir2, const CGeVector3d& dir3) const
{
}

CGeBoundBlock3d&
CGeBoundBlock3d::set(const CGePoint3d& point1, const CGePoint3d& point2)
{
	return *this;
}

CGeBoundBlock3d&
CGeBoundBlock3d::set(const CGePoint3d& base, const CGeVector3d& dir1, const CGeVector3d& dir2, const CGeVector3d& dir3)
{
	return *this;
}

CGeBoundBlock3d&
CGeBoundBlock3d::extend(const CGePoint3d& point)
{
	return *this;
}

CGeBoundBlock3d&
CGeBoundBlock3d::swell(double distance)
{
	return *this;
}

bool
CGeBoundBlock3d::contains(const CGePoint3d& point) const
{
	return false;
}

bool
CGeBoundBlock3d::isDisjoint(const CGeBoundBlock3d& block) const
{
	return false;
}

bool
CGeBoundBlock3d::isBox() const
{
	return false;
}

CGeBoundBlock3d&
CGeBoundBlock3d::setToBox(bool b)
{
	return *this;
}

CGeBoundBlock3d&
CGeBoundBlock3d::operator = (const CGeBoundBlock3d& source)
{
	return *this;
}
