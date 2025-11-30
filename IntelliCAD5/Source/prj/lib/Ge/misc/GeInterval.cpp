// File: GeInterval.cpp
//

#include "GeInterval.h"

//////////////////////////////////////////////////
// CGeInterval class implementation
//////////////////////////////////////////////////

CGeInterval::CGeInterval(double tol /* = 1.e-12*/)
{
}

CGeInterval::CGeInterval(const CGeInterval& source)
{
}

CGeInterval::CGeInterval(double lower, double upper, double tol /* = 1.e-12*/)
{
}

CGeInterval::CGeInterval(bool bBoundedBelow, double bound, double tol /* = 1.e-12*/)
{
}

CGeInterval::~CGeInterval()
{
}

double
CGeInterval::lowerBound() const
{
	return m_min;
}

double
CGeInterval::upperBound() const
{
	return m_max;
}

double
CGeInterval::element() const
{
	return 0.0;
}

void
CGeInterval::getBounds(double& lower, double& upper) const
{
}

double
CGeInterval::length() const
{
	return 0.0;
}

double
CGeInterval::tolerance() const
{
	return m_tolerance;
}

CGeInterval&
CGeInterval::set(double lower, double upper)
{
	m_min = lower;
	m_max = upper;
	return *this;
}

CGeInterval&
CGeInterval::set(bool bBoundedBelow, double bound)
{
	return *this;
}

CGeInterval&
CGeInterval::set()
{
	return *this;
}

CGeInterval&
CGeInterval::setUpper(double upper)
{
	m_max = upper;
	return *this;
}

CGeInterval&
CGeInterval::setLower(double lower)
{
	m_min = lower;
	return *this;
}

CGeInterval&
CGeInterval::setTolerance(double tol)
{
	m_tolerance = tol;
	return *this;
}

void
CGeInterval::getMerge(const CGeInterval& otherInterval, CGeInterval& result) const
{
}

int
CGeInterval::subtract(const CGeInterval& otherInterval, CGeInterval& leftInterval, CGeInterval& rightInterval) const
{
	return 0;
}

bool
CGeInterval::intersectWith(const CGeInterval& otherInterval, CGeInterval& result) const
{
	return false;
}

bool
CGeInterval::isBounded() const
{
	return false;
}

bool
CGeInterval::isBoundedAbove() const
{
	return false;
}

bool
CGeInterval::isBoundedBelow() const
{
	return false;
}

bool
CGeInterval::isUnBounded() const
{
	return false;
}

bool
CGeInterval::isSingleton() const
{
	return false;
}

bool
CGeInterval::isDisjoint(const CGeInterval& otherInterval) const
{
	return false;
}

bool
CGeInterval::contains(const CGeInterval& otherInterval) const
{
	return false;
}

bool
CGeInterval::contains(double val) const
{
	return false;
}

bool
CGeInterval::isContinuousAtUpper(const CGeInterval& otherInterval) const
{
	return false;
}

bool
CGeInterval::isOverlapAtUpper(const CGeInterval& otherInterval, CGeInterval& overlap) const
{
	return false;
}

bool
CGeInterval::isEqualAtUpper(const CGeInterval& otherInterval) const
{
	return false;
}

bool
CGeInterval::isEqualAtUpper(double value) const
{
	return false;
}

bool
CGeInterval::isEqualAtLower(const CGeInterval& otherInterval) const
{
	return false;
}

bool
CGeInterval::isEqualAtLower(double value) const
{
	return false;
}

bool
CGeInterval::isPeriodicallyOn(double period, double& val)
{
	return false;
}

bool
CGeInterval::operator > (double val) const
{
	return false;
}

bool
CGeInterval::operator > (const CGeInterval& otherInterval) const
{
	return false;
}

bool
CGeInterval::operator >= (double val) const
{
	return false;
}

bool
CGeInterval::operator >= (const CGeInterval& otherInterval) const
{
	return false;
}

bool
CGeInterval::operator < (double val) const
{
	return false;
}

bool
CGeInterval::operator < (const CGeInterval& otherInterval) const
{
	return false;
}

bool
CGeInterval::operator <= (double val) const
{
	return false;
}

bool
CGeInterval::operator <= (const CGeInterval& otherInterval) const
{
	return false;
}

CGeInterval&
CGeInterval::operator = (const CGeInterval& source)
{
	return *this;
}
