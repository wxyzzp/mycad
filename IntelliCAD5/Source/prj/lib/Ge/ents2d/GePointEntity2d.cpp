// File: GePointEntity2d.cpp
//

#include "GePointEntity2d.h"

//////////////////////////////////////////////////
// CGePointEntity2d class implementation
//////////////////////////////////////////////////

CGePointEntity2d::~CGePointEntity2d()
{
}

CGePoint2d
CGePointEntity2d::point2d() const
{
	return m_point;
}

CGePointEntity2d::operator CGePoint2d() const
{
}

CGePointEntity2d&
CGePointEntity2d::operator = (const CGePointEntity2d& source)
{
	return *this;
}

CGePointEntity2d::CGePointEntity2d()
{
}

CGePointEntity2d::CGePointEntity2d(const CGePointEntity2d& source)
{
}
