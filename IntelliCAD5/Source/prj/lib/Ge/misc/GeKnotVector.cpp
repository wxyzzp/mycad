// File: GeKnotVector.cpp
//

#include "GeKnotVector.h"

//////////////////////////////////////////////////
// CGeKnotVector class implementation
//////////////////////////////////////////////////

CGeKnotVector::CGeKnotVector(double eps /* = 1.e-9*/)
{
}

CGeKnotVector::CGeKnotVector(int size, int growSize, double eps /* = 1.e-9*/)
{
}

CGeKnotVector::CGeKnotVector(int size, const double pKnots [], double eps /* = 1.e-9*/)
{
}

CGeKnotVector::CGeKnotVector(int plusMult, const CGeKnotVector& source)
{
}

CGeKnotVector::CGeKnotVector(const CGeKnotVector& source)
{
}

CGeKnotVector::CGeKnotVector(const CGeDoubleArray& source, double eps /* = 1.e-9*/)
{
}

CGeKnotVector::~CGeKnotVector()
{
}

CGeKnotVector&
CGeKnotVector::operator = (const CGeKnotVector& source)
{
	return *this;
}

CGeKnotVector&
CGeKnotVector::operator = (const CGeDoubleArray& source)
{
	return *this;
}

double&
CGeKnotVector::operator [] (int i)
{
	return m_knots[i];
}

const double
CGeKnotVector::operator [] (int i) const
{
	return m_knots[i];
}

bool
CGeKnotVector::isEqualTo(const CGeKnotVector& other) const
{
	return false;
}

double
CGeKnotVector::startParam() const
{
	return 0.0;
}

double
CGeKnotVector::endParam() const
{
	return 0.0;
}

int
CGeKnotVector::multiplicityAt(int i) const
{
	return 0;
}

int
CGeKnotVector::numIntervals() const
{
	return 0;
}

int
CGeKnotVector::getInterval(int ord, double par, CGeInterval& interval) const
{
	return 0;
}

void
CGeKnotVector::getDistinctKnots(CGeDoubleArray& knots) const
{
}

bool
CGeKnotVector::contains(double param) const
{
	return false;
}

bool
CGeKnotVector::isOn(double knot) const
{
	return false;
}

CGeKnotVector&
CGeKnotVector::reverse()
{
	return *this;
}

CGeKnotVector&
CGeKnotVector::removeAt(int i)
{
	return *this;
}

CGeKnotVector&
CGeKnotVector::removeSubVector(int startIndex, int endIndex)
{
	return *this;
}

CGeKnotVector&
CGeKnotVector::insertAt(int i, double u, int multiplicity /* = 1*/)
{
	return *this;
}

CGeKnotVector&
CGeKnotVector::insert(double u)
{
	return *this;
}

int
CGeKnotVector::append(double val)
{
	return 0;
}

CGeKnotVector&
CGeKnotVector::append(CGeKnotVector& tail, double knotRatio /* = 0.0*/)
{
	return *this;
}

int
CGeKnotVector::split(double par, CGeKnotVector* pKnotVector1, int multLast, CGeKnotVector* pKnotVector2, int multFirst) const
{
	return 0;
}

CGeKnotVector&
CGeKnotVector::setRange(double lower, double upper)
{
	return *this;
}

double
CGeKnotVector::tolerance() const
{
	return m_tolerance;
}

CGeKnotVector&
CGeKnotVector::setTolerance(double tol)
{
	m_tolerance = tol;
	return *this;
}

int
CGeKnotVector::length() const
{
	return 0;
}

bool
CGeKnotVector::isEmpty() const
{
	return false;
}

int
CGeKnotVector::logicalLength() const
{
	return 0;
}

CGeKnotVector&
CGeKnotVector::setLogicalLength(int length)
{
	return *this;
}

int
CGeKnotVector::physicalLength() const
{
	return 0;
}

CGeKnotVector&
CGeKnotVector::setPhysicalLength(int length)
{
	return *this;
}

int
CGeKnotVector::growLength() const
{
	return 0;
}

CGeKnotVector&
CGeKnotVector::setGrowLength(int length)
{
	return *this;
}

const double*
CGeKnotVector::asArrayPtr() const
{
	return 0;
}

double*
CGeKnotVector::asArrayPtr()
{
	return 0;
}

CGeKnotVector&
CGeKnotVector::set(int size, const double pKnots [], double eps /* = 1.e-9*/)
{
	return *this;
}
