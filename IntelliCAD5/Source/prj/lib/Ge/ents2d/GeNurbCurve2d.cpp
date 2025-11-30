// File: GeNurbCurve2d.cpp
//

#include "GeNurbCurve2d.h"

//////////////////////////////////////////////////
// CGeNurbCurve2d class implementation
//////////////////////////////////////////////////

CGeNurbCurve2d::CGeNurbCurve2d()
{
}

CGeNurbCurve2d::CGeNurbCurve2d(const CGeNurbCurve2d& source)
{
}

CGeNurbCurve2d::CGeNurbCurve2d(int degree, const CGeKnotVector& knots, const CGePoint2dArray& controlPoints, bool bIsPeriodic /* = false*/)
{
}

CGeNurbCurve2d::CGeNurbCurve2d(int degree, const CGeKnotVector& knots, const CGePoint2dArray& controlPoints, const CGeDoubleArray& weights, bool bIsPeriodic /* = false*/)
{
}

CGeNurbCurve2d::CGeNurbCurve2d(int degree, const CGePolyline2d& fitPolyline, bool bIsPeriodic /* = false*/)
{
}

CGeNurbCurve2d::CGeNurbCurve2d(const CGePoint2dArray& fitPoints, const CGeVector2d& startTangent, const CGeVector2d& endTangent, bool bStartTangentDefined /* = true*/, bool bEndTangentDefined /* = true*/, const CGeTolerance& fitTol /* = CGeContext::s_defTol*/)
{
}

CGeNurbCurve2d::CGeNurbCurve2d(const CGePoint2dArray& fitPoints, const CGeTolerance& fitTol /* = CGeContext::s_defTol*/)
{
}

CGeNurbCurve2d::CGeNurbCurve2d(const CGePoint2dArray& fitPoints, const CGeVector2dArray& fitTangents, const CGeTolerance& fitTol /* = CGeContext::s_defTol*/, bool bIsPeriodic /* = false*/)
{
}

CGeNurbCurve2d::CGeNurbCurve2d(const CGeEllipArc2d& ellipArc)
{
}

CGeNurbCurve2d::CGeNurbCurve2d(const CGeLineSegment2d& lineSegment)
{
}

int
CGeNurbCurve2d::numFitPoints() const
{
	return 0;
}

bool
CGeNurbCurve2d::getFitPointAt(int i, CGePoint2d& point) const
{
	return false;
}

bool
CGeNurbCurve2d::getFitTolerance(CGeTolerance& fitTol) const
{
	return false;
}

bool
CGeNurbCurve2d::getFitTangents(CGeVector2d& startTangent, CGeVector2d& endTangent) const
{
	return false;
}

bool
CGeNurbCurve2d::getFitData(CGePoint2dArray& fitPoints, CGeTolerance& fitTol, bool& tangentsExist, CGeVector2d& startTangent, CGeVector2d& endTangent) const
{
	return false;
}

void
CGeNurbCurve2d::getDefinitionData(int& degree, bool& bRational, bool& bPeriodic, CGeKnotVector& knots, CGePoint2dArray& controlPoints, CGeDoubleArray& weights) const
{
}

int
CGeNurbCurve2d::numWeights() const
{
	return 0;
}

double
CGeNurbCurve2d::weightAt(int idx) const
{
	return 0.0;
}

bool
CGeNurbCurve2d::evalMode() const
{
	return false;
}

bool
CGeNurbCurve2d::getParamsOfC1Discontinuity(CGeDoubleArray& params, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeNurbCurve2d::getParamsOfG1Discontinuity(CGeDoubleArray& params, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeNurbCurve2d::setFitPointAt(int i, const CGePoint2d& point)
{
	return false;
}

bool
CGeNurbCurve2d::addFitPointAt(int i, const CGePoint2d& point)
{
	return false;
}

bool
CGeNurbCurve2d::deleteFitPointAt(int i)
{
	return false;
}

bool
CGeNurbCurve2d::setFitTolerance(const CGeTolerance& fitTol /* = CGeContext::s_defTol*/)
{
	return false;
}

bool
CGeNurbCurve2d::setFitTangents(const CGeVector2d& startTangent, const CGeVector2d& endTangent)
{
	return false;
}

CGeNurbCurve2d&
CGeNurbCurve2d::setFitData(const CGePoint2dArray& fitPoints, const CGeVector2d& startTangent, const CGeVector2d& endTangent, const CGeTolerance& fitTol /* = CGeContext::s_defTol*/)
{
	return *this;
}

CGeNurbCurve2d&
CGeNurbCurve2d::setFitData(const CGeKnotVector& fitKnots, const CGePoint2dArray& fitPoints, const CGeVector2d& startTangent, const CGeVector2d& endTangent, const CGeTolerance& fitTol /* = CGeContext::s_defTol*/, bool bIsPeriodic /* = false*/)
{
	return *this;
}

CGeNurbCurve2d&
CGeNurbCurve2d::setFitData(int degree, const CGePoint2dArray& fitPoints, const CGeTolerance& fitTol /* = CGeContext::s_defTol*/)
{
	return *this;
}

bool
CGeNurbCurve2d::purgeFitData()
{
	return false;
}

CGeNurbCurve2d&
CGeNurbCurve2d::addKnot(double newKnot)
{
	return *this;
}

CGeNurbCurve2d&
CGeNurbCurve2d::insertKnot(double newKnot)
{
	return *this;
}

CGeSplineCurve2d&
CGeNurbCurve2d::setWeightAt(int i, double weight)
{
	return *static_cast<CGeSplineCurve2d*>(this);
}

CGeNurbCurve2d&
CGeNurbCurve2d::setEvalMode(bool bEvalMode /* = false*/)
{
	return *this;
}

CGeNurbCurve2d&
CGeNurbCurve2d::joinWith(const CGeNurbCurve2d& curve)
{
	return *this;
}

CGeNurbCurve2d&
CGeNurbCurve2d::hardTrimByParams(double newStartParam, double newEndParam)
{
	return *this;
}

CGeNurbCurve2d&
CGeNurbCurve2d::makeRational(double weight /* = 1.0*/)
{
	return *this;
}

CGeNurbCurve2d&
CGeNurbCurve2d::makeClosed()
{
	return *this;
}

CGeNurbCurve2d&
CGeNurbCurve2d::makePeriodic()
{
	return *this;
}

CGeNurbCurve2d&
CGeNurbCurve2d::makeNonPeriodic()
{
	return *this;
}

CGeNurbCurve2d&
CGeNurbCurve2d::makeOpen()
{
	return *this;
}

CGeNurbCurve2d&
CGeNurbCurve2d::elevateDegree(int plusDegree)
{
	return *this;
}

CGeNurbCurve2d&
CGeNurbCurve2d::operator = (const CGeNurbCurve2d& source)
{
	return *this;
}
