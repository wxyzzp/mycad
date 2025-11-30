// File: GeSplineCurveBase.cpp
//

#include "GeSplineCurveBase.h"

//////////////////////////////////////////////////
// CGeSplineCurveBase class implementation
//////////////////////////////////////////////////

CGeSplineCurveBase::~CGeSplineCurveBase()
{
}

bool
CGeSplineCurveBase::isRational() const
{
	return m_bRational;
}

int
CGeSplineCurveBase::degree() const
{
	return m_degree;
}

int
CGeSplineCurveBase::order() const
{
	return m_order;
}

int
CGeSplineCurveBase::numKnots() const
{
	return m_knots.length();
}

int
CGeSplineCurveBase::numControlPoints() const
{
	return 0;
}

int
CGeSplineCurveBase::continuityAtKnot(int i, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return 0;
}

double
CGeSplineCurveBase::startParam() const
{
	return m_knots.startParam();
}

double
CGeSplineCurveBase::endParam() const
{
	return m_knots.endParam();
}

bool
CGeSplineCurveBase::hasFitData() const
{
	return m_bHasFitData;
}

double
CGeSplineCurveBase::knotAt(int i) const
{
	return m_knots[i];
}

const CGeKnotVector&
CGeSplineCurveBase::knots() const
{
	return m_knots;
}
