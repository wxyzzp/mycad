// File: GeCurve3d.cpp
//

#include "GeCurve3d.h"
#include "GeBoundBlock3d.h"

//////////////////////////////////////////////////
// CGeCurve3d class implementation
//////////////////////////////////////////////////

CGeCurve3d::~CGeCurve3d()
{
}

void
CGeCurve3d::getInterval(CGeInterval& interval) const
{
}

void
CGeCurve3d::getInterval(CGeInterval& interval, CGePoint3d& start, CGePoint3d& end) const
{
}

CGeCurve3d&
CGeCurve3d::reverseParam()
{
	return *this;
}

CGeCurve3d&
CGeCurve3d::setInterval()
{
	return *this;
}

bool
CGeCurve3d::setInterval(const CGeInterval& interval)
{
	return false;
}

double
CGeCurve3d::distanceTo(const CGePoint3d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return 0.0;
}

double
CGeCurve3d::distanceTo(const CGeCurve3d& curve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return 0.0;
}

CGePoint3d
CGeCurve3d::closestPointTo(const CGePoint3d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return CGePoint3d();
}

CGePoint3d
CGeCurve3d::closestPointTo(const CGeCurve3d& curve, CGePoint3d& pointOnOtherCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return CGePoint3d();
}

void
CGeCurve3d::getClosestPointTo(const CGePoint3d& point, CGePointOnCurve3d& pointOnCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
}

void
CGeCurve3d::getClosestPointTo(const CGeCurve3d& curve, CGePointOnCurve3d& pointOnThisCurve, CGePointOnCurve3d& pointOnOtherCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
}

CGePoint3d
CGeCurve3d::projClosestPointTo(const CGePoint3d& point, const CGeVector3d& projDir, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return CGePoint3d();
}

CGePoint3d
CGeCurve3d::projClosestPointTo(const CGeCurve3d& curve, const CGeVector3d& projDir, CGePoint3d& pointOnOtherCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return CGePoint3d();
}

void
CGeCurve3d::getProjClosestPointTo(const CGePoint3d& point, const CGeVector3d& projDir, CGePointOnCurve3d& pointOnCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
}

void
CGeCurve3d::getProjClosestPointTo(const CGeCurve3d& curve, const CGeVector3d& projDir, CGePointOnCurve3d& pointOnThisCurve, CGePointOnCurve3d& pointOnOtherCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
}

bool
CGeCurve3d::getNormalPoint(const CGePoint3d& point, CGePointOnCurve3d& pointOnCurve, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGeBoundBlock3d
CGeCurve3d::boundBlock() const
{
	return CGeBoundBlock3d();
}

CGeBoundBlock3d
CGeCurve3d::boundBlock(const CGeInterval& range) const
{
	return CGeBoundBlock3d();
}

CGeBoundBlock3d
CGeCurve3d::orthoBoundBlock() const
{
	return CGeBoundBlock3d();
}

CGeBoundBlock3d
CGeCurve3d::orthoBoundBlock(const CGeInterval& range) const
{
	return CGeBoundBlock3d();
}

CGeEntity3d*
CGeCurve3d::project(const CGePlane& projPlane, const CGeVector3d& projDir, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return 0;
}

CGeEntity3d*
CGeCurve3d::orthoProject(const CGePlane& projPlane, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return 0;
}

bool
CGeCurve3d::isOn(const CGePoint3d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCurve3d::isOn(const CGePoint3d& point, double& param, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCurve3d::isOn(double param, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

double
CGeCurve3d::paramOf(const CGePoint3d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return 0.0;
}

void
CGeCurve3d::getTrimmedOffset(double distance, const CGeVector3d& planeNormal, CGeVoidPointerArray& offsetCurveList, CGe::EOffsetCrvExtType extensionType /* = CGe::eFillet*/, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
}

bool
CGeCurve3d::isClosed(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCurve3d::isPlanar(CGePlane& plane, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCurve3d::isLinear(CGeLine3d& line, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCurve3d::isCoplanarWith(const CGeCurve3d& curve, CGePlane& plane, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCurve3d::isPeriodic(double& period) const
{
	return false;
}

double
CGeCurve3d::length(double fromParam, double toParam, double tol /* = CGeContext::s_defTol.equalPoint()*/) const
{
	return 0.0;
}

double
CGeCurve3d::paramAtLength(double datumParam, double length, bool bPositiveParamDir /* = true*/, double tol /* = CGeContext::s_defTol.equalPoint()*/) const
{
	return 0.0;
}

bool
CGeCurve3d::area(double fromParam, double toParam, double& value, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCurve3d::isDegenerate(CGe::EEntityId& degenerateType, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeCurve3d::isDegenerate(CGeEntity3d*& pConvertedEntity, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

void
CGeCurve3d::getSplitCurves(double param, CGeCurve3d*& pCurve1, CGeCurve3d*& pCurve2) const
{
}

bool
CGeCurve3d::explode(CGeVoidPointerArray& explodedCurves, CGeIntArray& newExplodedCurves, const CGeInterval* pInterval /* = 0*/) const
{
	return false;
}

void
CGeCurve3d::getLocalClosestPoints(const CGePoint3d& point, CGePointOnCurve3d& approxPoint, const CGeInterval* pInterval /* = 0*/, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
}

void
CGeCurve3d::getLocalClosestPoints(const CGeCurve3d& otherCurve, CGePointOnCurve3d& approxPointOnThisCurve, CGePointOnCurve3d& approxPointOnOtherCurve, const CGeInterval* pInterval1 /* = 0*/, const CGeInterval* pInterval2 /* = 0*/, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
}

bool
CGeCurve3d::hasStartPoint(CGePoint3d& startPoint) const
{
	return false;
}

bool
CGeCurve3d::hasEndPoint(CGePoint3d& endPoint) const
{
	return false;
}

CGePoint3d
CGeCurve3d::evalPoint(double param) const
{
	return CGePoint3d();
}

CGePoint3d
CGeCurve3d::evalPoint(double param, int numDeriv, CGeVector3dArray& derivArray) const
{
	return CGePoint3d();
}

void
CGeCurve3d::getSamplePoints(double fromParam, double toParam, double approxEps, CGePoint3dArray& pointArray, CGeDoubleArray& paramArray) const
{
}

void
CGeCurve3d::getSamplePoints(int numSample, CGePoint3dArray& pointArray) const
{
}

CGeCurve3d&
CGeCurve3d::operator = (const CGeCurve3d& source)
{
	return *this;
}

CGeCurve3d::CGeCurve3d()
{
}

CGeCurve3d::CGeCurve3d(const CGeCurve3d& source)
{
}
