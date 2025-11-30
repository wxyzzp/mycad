// File: GeCurve2d.cpp
//

#include "GeCurve2d.h"
#include "GePoint2d.h"
#include "GeBoundBlock2d.h"

//////////////////////////////////////////////////
// CGeCurve2d class implementation
//////////////////////////////////////////////////

CGeCurve2d::~CGeCurve2d()
{
}

void
CGeCurve2d::getInterval(CGeInterval& interval) const
{
}

void
CGeCurve2d::getInterval(CGeInterval& interval, CGePoint2d& start, CGePoint2d& end) const
{
}

CGeCurve2d&
CGeCurve2d::reverseParam()
{
	return *this;
}

CGeCurve2d&
CGeCurve2d::setInterval()
{
	return *this;
}

bool
CGeCurve2d::setInterval(const CGeInterval& interval)
{
	return false;
}

double
CGeCurve2d::distanceTo(const CGePoint2d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return 0.0;
}

double
CGeCurve2d::distanceTo(const CGeCurve2d& curve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return 0.0;
}

CGePoint2d
CGeCurve2d::closestPointTo(const CGePoint2d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return CGePoint2d();
}

CGePoint2d
CGeCurve2d::closestPointTo(const CGeCurve2d& curve, CGePoint2d& pointOnOtherCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return CGePoint2d();
}

void
CGeCurve2d::getClosestPointTo(const CGePoint2d& point, CGePointOnCurve2d& pointOnCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
}

void
CGeCurve2d::getClosestPointTo(const CGeCurve2d& curve, CGePointOnCurve2d& pointOnThisCurve, CGePointOnCurve2d& pointOnOtherCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
}

bool
CGeCurve2d::getNormalPoint(const CGePoint2d& point, CGePointOnCurve2d& pointOnCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGeBoundBlock2d
CGeCurve2d::boundBlock() const
{
	return CGeBoundBlock2d();
}

CGeBoundBlock2d
CGeCurve2d::boundBlock(const CGeInterval& range) const
{
	return CGeBoundBlock2d();
}

CGeBoundBlock2d
CGeCurve2d::orthoBoundBlock() const
{
	return CGeBoundBlock2d();
}

CGeBoundBlock2d
CGeCurve2d::orthoBoundBlock(const CGeInterval& range) const
{
	return CGeBoundBlock2d();
}

bool
CGeCurve2d::isOn(const CGePoint2d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCurve2d::isOn(const CGePoint2d& point, double& param, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCurve2d::isOn(double param, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

double
CGeCurve2d::paramOf(const CGePoint2d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return 0.0;
}

bool
CGeCurve2d::isClosed(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCurve2d::isLinear(CGeLine2d& line, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCurve2d::isPeriodic(double& period) const
{
	return false;
}

double
CGeCurve2d::length(double fromParam, double toParam, double tol /* = CGeContext::s_defTol.equalPoint()*/) const
{
	return 0.0;
}

double
CGeCurve2d::paramAtLength(double datumParam, double length, bool bPositiveParamDir /* = true*/, double tol /* = CGeContext::s_defTol.equalPoint()*/) const
{
	return 0.0;
}

bool
CGeCurve2d::area(double fromParam, double toParam, double& value, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCurve2d::isDegenerate(CGe::EEntityId& degenerateType, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCurve2d::isDegenerate(CGeEntity2d*& pConvertedEntity, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

void
CGeCurve2d::getSplitCurves(double param, CGeCurve2d*& pCurve1, CGeCurve2d*& pCurve2) const
{
}

bool
CGeCurve2d::explode(CGeVoidPointerArray& explodedCurves, CGeIntArray& newExplodedCurves, const CGeInterval* pInterval /* = 0*/) const
{
	return false;
}

void
CGeCurve2d::getLocalClosestPoints(const CGePoint2d& point, CGePointOnCurve2d& approxPoint, const CGeInterval* pInterval /* = 0*/, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
}

void
CGeCurve2d::getLocalClosestPoints(const CGeCurve2d& otherCurve, CGePointOnCurve2d& approxPointOnThisCurve, CGePointOnCurve2d& approxPointOnOtherCurve, const CGeInterval* pInterval1 /* = 0*/, const CGeInterval* pInterval2 /* = 0*/, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
}

bool
CGeCurve2d::hasStartPoint(CGePoint2d& startPoint) const
{
	return false;
}

bool
CGeCurve2d::hasEndPoint(CGePoint2d& endPoint) const
{
	return false;
}

CGePoint2d
CGeCurve2d::evalPoint(double param) const
{
	return CGePoint2d();
}

CGePoint2d
CGeCurve2d::evalPoint(double param, int numDeriv, CGeVector2dArray& derivArray) const
{
	return CGePoint2d();
}

void
CGeCurve2d::getSamplePoints(double fromParam, double toParam, double approxEps, CGePoint2dArray& pointArray, CGeDoubleArray& paramArray) const
{
}

void
CGeCurve2d::getSamplePoints(int numSample, CGePoint2dArray& pointArray) const
{
}

void
CGeCurve2d::getTrimmedOffset(double distance, CGeVoidPointerArray& offsetCurveList, CGe::EOffsetCrvExtType extensionType /* = CGe::eFillet*/, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
}

CGeCurve2d&
CGeCurve2d::operator = (const CGeCurve2d& source)
{
	return *this;
}

CGeCurve2d::CGeCurve2d()
{
}

CGeCurve2d::CGeCurve2d(const CGeCurve2d& source)
{
}
