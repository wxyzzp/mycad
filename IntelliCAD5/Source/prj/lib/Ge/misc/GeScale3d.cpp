// File: GeScale3d.cpp
//

#include "GeScale3d.h"
#include "GeMatrix3d.h"

//////////////////////////////////////////////////
// CGeScale3d class implementation
//////////////////////////////////////////////////

const CGeScale3d CGeScale3d::s_identity;


CGeScale3d::CGeScale3d()
{
}

CGeScale3d::CGeScale3d(const CGeScale3d& source)
{
}

CGeScale3d::CGeScale3d(double factor)
{
}

CGeScale3d::CGeScale3d(double sx, double sy, double sz)
{
}

CGeScale3d
CGeScale3d::operator * (const CGeScale3d& scale) const
{
	return CGeScale3d();
}

CGeScale3d&
CGeScale3d::operator *= (const CGeScale3d& scale)
{
	return *this;
}

CGeScale3d&
CGeScale3d::preMultBy(const CGeScale3d& leftScale)
{
	return *this;
}

CGeScale3d&
CGeScale3d::postMultBy(const CGeScale3d& rightScale)
{
	return *this;
}

CGeScale3d&
CGeScale3d::setToProduct(const CGeScale3d& scale1, const CGeScale3d& scale2)
{
	return *this;
}

CGeScale3d
CGeScale3d::operator * (double factor) const
{
	return CGeScale3d();
}

CGeScale3d&
CGeScale3d::operator *= (double factor)
{
	return *this;
}

CGeScale3d&
CGeScale3d::setToProduct(const CGeScale3d& scale, double factor)
{
	return *this;
}

CGeScale3d
CGeScale3d::inverse() const
{
	return CGeScale3d();
}

CGeScale3d&
CGeScale3d::invert()
{
	return *this;
}

bool
CGeScale3d::isProportional(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeScale3d::operator == (const CGeScale3d& scale) const
{
	return false;
}

bool
CGeScale3d::operator != (const CGeScale3d& scale) const
{
	return false;
}

bool
CGeScale3d::isEqualTo(const CGeScale3d& scale, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

double
CGeScale3d::operator [] (unsigned int i) const
{
	return m_scales[i];
}

double&
CGeScale3d::operator [] (unsigned int i)
{
	return m_scales[i];
}

CGeScale3d&
CGeScale3d::set(double sx, double sy, double sz)
{
	return *this;
}

CGeScale3d::operator CGeMatrix3d() const
{
}

void
CGeScale3d::getMatrix(CGeMatrix3d& matrix) const
{
}

CGeScale3d&
CGeScale3d::extractScale(const CGeMatrix3d& matrix)
{
	return *this;
}

CGeScale3d&
CGeScale3d::removeScale(CGeMatrix3d& matrix)
{
	return *this;
}
