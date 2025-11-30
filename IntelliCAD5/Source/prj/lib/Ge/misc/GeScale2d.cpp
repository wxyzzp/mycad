// File: GeScale2d.cpp
//

#include "GeScale2d.h"
#include "GeMatrix2d.h"
#include "GeScale3d.h"

//////////////////////////////////////////////////
// CGeScale2d class implementation
//////////////////////////////////////////////////

const CGeScale2d CGeScale2d::s_identity;


CGeScale2d::CGeScale2d()
{
}

CGeScale2d::CGeScale2d(const CGeScale2d& source)
{
}

CGeScale2d::CGeScale2d(double factor)
{
}

CGeScale2d::CGeScale2d(double sx, double sy)
{
}

CGeScale2d
CGeScale2d::operator * (const CGeScale2d& scale) const
{
	return CGeScale2d();
}

CGeScale2d&
CGeScale2d::operator *= (const CGeScale2d& scale)
{
	return *this;
}

CGeScale2d&
CGeScale2d::preMultBy(const CGeScale2d& leftScale)
{
	return *this;
}

CGeScale2d&
CGeScale2d::postMultBy(const CGeScale2d& rightScale)
{
	return *this;
}

CGeScale2d&
CGeScale2d::setToProduct(const CGeScale2d& scale1, const CGeScale2d& scale2)
{
	return *this;
}

CGeScale2d
CGeScale2d::operator * (double factor) const
{
	return CGeScale2d();
}

CGeScale2d&
CGeScale2d::operator *= (double factor)
{
	return *this;
}

CGeScale2d&
CGeScale2d::setToProduct(const CGeScale2d& scale, double factor)
{
	return *this;
}

CGeScale2d
CGeScale2d::inverse() const
{
	return CGeScale2d();
}

CGeScale2d&
CGeScale2d::invert()
{
	return *this;
}

bool
CGeScale2d::isProportional(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeScale2d::operator == (const CGeScale2d& scale) const
{
	return false;
}

bool
CGeScale2d::operator != (const CGeScale2d& scale) const
{
	return false;
}

bool
CGeScale2d::isEqualTo(const CGeScale2d& scale, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

double
CGeScale2d::operator [] (unsigned int i) const
{
	return m_scales[i];
}

double&
CGeScale2d::operator [] (unsigned int i)
{
	return m_scales[i];
}

CGeScale2d&
CGeScale2d::set(double sx, double sy)
{
	return *this;
}

CGeScale2d::operator CGeMatrix2d() const
{
}

void
CGeScale2d::getMatrix(CGeMatrix2d& matrix) const
{
}

CGeScale2d&
CGeScale2d::extractScale(const CGeMatrix2d& matrix)
{
	return *this;
}

CGeScale2d&
CGeScale2d::removeScale(CGeMatrix2d& matrix)
{
	return *this;
}

CGeScale2d::operator CGeScale3d() const
{
}
