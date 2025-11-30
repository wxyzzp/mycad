// File: GeOffsetCurve3d.cpp
//

#include "GeOffsetCurve3d.h"
#include "GeMatrix3d.h"

//////////////////////////////////////////////////
// CGeOffsetCurve3d class implementation
//////////////////////////////////////////////////

CGeOffsetCurve3d::CGeOffsetCurve3d()
{
}

CGeOffsetCurve3d::CGeOffsetCurve3d(const CGeOffsetCurve3d& source)
{
}

CGeOffsetCurve3d::CGeOffsetCurve3d(const CGeCurve3d& baseCurve, const CGeVector3d& planeNormal, double offsetDistance)
{
}

const CGeCurve3d*
CGeOffsetCurve3d::curve() const
{
	return m_pCurve;
}

double
CGeOffsetCurve3d::offsetDistance() const
{
	return m_offset;
}

bool
CGeOffsetCurve3d::paramDirection() const
{
	return false;
}

CGeMatrix3d
CGeOffsetCurve3d::transformation() const
{
	return CGeMatrix3d();
}

CGeOffsetCurve3d&
CGeOffsetCurve3d::setCurve(const CGeCurve3d& baseCurve)
{
	return *this;
}

CGeOffsetCurve3d&
CGeOffsetCurve3d::setOffsetDistance(double offsetDistance)
{
	m_offset = offsetDistance;
	return *this;
}

CGeVector3d
CGeOffsetCurve3d::normal() const
{
	return m_normal;
}

CGeOffsetCurve3d&
CGeOffsetCurve3d::setNormal(const CGeVector3d& planeNormal)
{
	m_normal = planeNormal;
	return *this;
}

CGeOffsetCurve3d&
CGeOffsetCurve3d::operator = (const CGeOffsetCurve3d& source)
{
	return *this;
}
