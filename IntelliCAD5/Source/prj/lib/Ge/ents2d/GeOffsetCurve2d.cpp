// File: GeOffsetCurve2d.cpp
//

#include "GeOffsetCurve2d.h"
#include "GeMatrix2d.h"

//////////////////////////////////////////////////
// CGeOffsetCurve2d class implementation
//////////////////////////////////////////////////

CGeOffsetCurve2d::CGeOffsetCurve2d()
{
}

CGeOffsetCurve2d::CGeOffsetCurve2d(const CGeOffsetCurve2d& source)
{
}

CGeOffsetCurve2d::CGeOffsetCurve2d(const CGeCurve2d& baseCurve, double offsetDistance)
{
}

const CGeCurve2d*
CGeOffsetCurve2d::curve() const
{
	return m_pCurve;
}

double
CGeOffsetCurve2d::offsetDistance() const
{
	return m_offset;
}

bool
CGeOffsetCurve2d::paramDirection() const
{
	return false;
}

CGeMatrix2d
CGeOffsetCurve2d::transformation() const
{
	return CGeMatrix2d();
}

CGeOffsetCurve2d&
CGeOffsetCurve2d::setCurve(const CGeCurve2d& baseCurve)
{
	return *this;
}

CGeOffsetCurve2d&
CGeOffsetCurve2d::setOffsetDistance(double offsetDistance)
{
	return *this;
}

CGeOffsetCurve2d&
CGeOffsetCurve2d::operator = (const CGeOffsetCurve2d& source)
{
	return *this;
}
