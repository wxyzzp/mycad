// File: GePointEntity3d.cpp
//

#include "GePointEntity3d.h"

//////////////////////////////////////////////////
// CGePointEntity3d class implementation
//////////////////////////////////////////////////

CGePointEntity3d::~CGePointEntity3d()
{
}

CGePoint3d
CGePointEntity3d::point3d() const
{
	return m_point;
}

CGePointEntity3d::operator CGePoint3d() const
{
}

CGePointEntity3d&
CGePointEntity3d::operator = (const CGePointEntity3d& source)
{
	return *this;
}

CGePointEntity3d::CGePointEntity3d()
{
}

CGePointEntity3d::CGePointEntity3d(const CGePointEntity3d& source)
{
}
