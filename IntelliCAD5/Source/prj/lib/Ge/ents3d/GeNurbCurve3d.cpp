// File: GeNurbCurve3d.cpp
//

#include "GeNurbCurve3d.h"

//////////////////////////////////////////////////
// CGeNurbCurve3d class implementation
//////////////////////////////////////////////////

CGeNurbCurve3d::CGeNurbCurve3d()
{
}

CGeNurbCurve3d::CGeNurbCurve3d(const CGeNurbCurve3d& source)
{
}

CGeNurbCurve3d::CGeNurbCurve3d(int degree, const CGeKnotVector& knots, const CGePoint3dArray& controlPoints, bool bIsPeriodic /* = false*/)
{
}

CGeNurbCurve3d::CGeNurbCurve3d(int degree, const CGeKnotVector& knots, const CGePoint3dArray& controlPoints, const CGeDoubleArray& weights, bool bIsPeriodic /* = false*/)
{
}

CGeNurbCurve3d::CGeNurbCurve3d(int degree, const CGePolyline3d& fitPolyline, bool bIsPeriodic /* = false*/)
{
}

CGeNurbCurve3d::CGeNurbCurve3d(const CGePoint3dArray& fitPoints, const CGeVector3d& startTangent, const CGeVector3d& endTangent, bool bStartTangentDefined /* = true*/, bool bEndTangentDefined /* = true*/, const CGeTolerance& fitTol /* = CGeContext::s_defTol*/)
{
}

CGeNurbCurve3d::CGeNurbCurve3d(const CGePoint3dArray& fitPoints, const CGeTolerance& fitTol /* = CGeContext::s_defTol*/)
{
}

CGeNurbCurve3d::CGeNurbCurve3d(const CGePoint3dArray& fitPoints, const CGeVector3dArray& fitTangents, const CGeTolerance& fitTol /* = CGeContext::s_defTol*/, bool bIsPeriodic /* = false*/)
{
}

CGeNurbCurve3d::CGeNurbCurve3d(const CGeEllipArc3d& ellipArc)
{
}

CGeNurbCurve3d::CGeNurbCurve3d(const CGeLineSegment3d& lineSegment)
{
}

int
CGeNurbCurve3d::numFitPoints() const
{
	return 0;
}

bool
CGeNurbCurve3d::getFitPointAt(int i, CGePoint3d& point) const
{
	return false;
}

bool
CGeNurbCurve3d::getFitTolerance(CGeTolerance& fitTol) const
{
	return false;
}

bool
CGeNurbCurve3d::getFitTangents(CGeVector3d& startTangent, CGeVector3d& endTangent) const
{
	return false;
}

bool
CGeNurbCurve3d::getFitTangents(CGeVector3d& startTangent, CGeVector3d& endTangent, bool& bStartTangentDefined, bool& bEndTangentDefined) const
{
	return false;
}

bool
CGeNurbCurve3d::getFitData(CGePoint3dArray& fitPoints, CGeTolerance& fitTol, bool& bTangentsExist, CGeVector3d& startTangent, CGeVector3d& endTangent) const
{
	return false;
}

void
CGeNurbCurve3d::getDefinitionData(int& degree, bool& bRational, bool& bPeriodic, CGeKnotVector& knots, CGePoint3dArray& controlPoints, CGeDoubleArray& weights) const
{
}

int
CGeNurbCurve3d::numWeights() const
{
	return 0;
}

double
CGeNurbCurve3d::weightAt(int i) const
{
	return 0.0;
}

bool
CGeNurbCurve3d::evalMode() const
{
	return false;
}

bool
CGeNurbCurve3d::getParamsOfC1Discontinuity(CGeDoubleArray& params, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeNurbCurve3d::getParamsOfG1Discontinuity(CGeDoubleArray& params, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeNurbCurve3d::setFitPointAt(int i, const CGePoint3d& point)
{
	return false;
}

bool
CGeNurbCurve3d::addFitPointAt(int i, const CGePoint3d& point)
{
	return false;
}

bool
CGeNurbCurve3d::deleteFitPointAt(int i)
{
	return false;
}

bool
CGeNurbCurve3d::setFitTolerance(const CGeTolerance& fitTol /* = CGeContext::s_defTol*/)
{
	return false;
}

bool
CGeNurbCurve3d::setFitTangents(const CGeVector3d& startTangent, const CGeVector3d& endTangent)
{
	return false;
}

bool
CGeNurbCurve3d::setFitTangents(const CGeVector3d& startTangent, const CGeVector3d& endTangent, bool bStartTangentDefined, bool bEndTangentDefined) const
{
	return false;
}

CGeNurbCurve3d&
CGeNurbCurve3d::setFitData(const CGePoint3dArray& fitPoints, const CGeVector3d& startTangent, const CGeVector3d& endTangent, const CGeTolerance& fitTol /* = CGeContext::s_defTol*/)
{
	return *this;
}

CGeNurbCurve3d&
CGeNurbCurve3d::setFitData(const CGeKnotVector& fitKnots, const CGePoint3dArray& fitPoints, const CGeVector3d& startTangent, const CGeVector3d& endTangent, const CGeTolerance& fitTol /* = CGeContext::s_defTol*/, bool bIsPeriodic /* = false*/)
{
	return *this;
}

CGeNurbCurve3d&
CGeNurbCurve3d::setFitData(int degree, const CGePoint3dArray& fitPoints, const CGeTolerance& fitTol /* = CGeContext::s_defTol*/)
{
	return *this;
}

bool
CGeNurbCurve3d::purgeFitData()
{
	return false;
}

CGeNurbCurve3d&
CGeNurbCurve3d::addKnot(double newKnot)
{
	return *this;
}

CGeNurbCurve3d&
CGeNurbCurve3d::insertKnot(double newKnot)
{
	return *this;
}

CGeSplineCurve3d&
CGeNurbCurve3d::setWeightAt(int i, double weight)
{
	return *static_cast<CGeSplineCurve3d*>(this);
}

CGeNurbCurve3d&
CGeNurbCurve3d::setEvalMode(bool bEvalMode /* = false*/)
{
	return *this;
}

CGeNurbCurve3d&
CGeNurbCurve3d::joinWith(const CGeNurbCurve3d& curve)
{
	return *this;
}

CGeNurbCurve3d&
CGeNurbCurve3d::hardTrimByParams(double newStartParam, double newEndParam)
{
	return *this;
}

CGeNurbCurve3d&
CGeNurbCurve3d::makeRational(double weight /* = 1.0*/)
{
	return *this;
}

CGeNurbCurve3d&
CGeNurbCurve3d::makeClosed()
{
	return *this;
}

CGeNurbCurve3d&
CGeNurbCurve3d::makePeriodic()
{
	return *this;
}

CGeNurbCurve3d&
CGeNurbCurve3d::makeNonPeriodic()
{
	return *this;
}

CGeNurbCurve3d&
CGeNurbCurve3d::makeOpen()
{
	return *this;
}

CGeNurbCurve3d&
CGeNurbCurve3d::elevateDegree(int plusDegree)
{
	return *this;
}

CGeNurbCurve3d&
CGeNurbCurve3d::operator = (const CGeNurbCurve3d& source)
{
	return *this;
}
