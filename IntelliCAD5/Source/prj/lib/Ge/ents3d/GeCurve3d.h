// File: GeCurve3d.h
//
// CGeCurve3d
//

#pragma once


#include "GeEntity3d.h"
#include "GeArrays.h"


class GE_API CGeInterval;
class GE_API CGePointOnCurve3d;
class GE_API CGeBoundBlock3d;
class GE_API CGeLine3d;


class GE_API CGeCurve3d : public CGeEntity3d
{
public:
	virtual ~CGeCurve3d();

	void			getInterval				(CGeInterval& interval) const;
	void			getInterval				(CGeInterval& interval, CGePoint3d& start, CGePoint3d& end) const;
	CGeCurve3d&		reverseParam			();
	CGeCurve3d&		setInterval				();
	bool			setInterval				(const CGeInterval& interval);
	double			distanceTo				(const CGePoint3d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;
	double			distanceTo				(const CGeCurve3d& curve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGePoint3d		closestPointTo			(const CGePoint3d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGePoint3d		closestPointTo			(const CGeCurve3d& curve, CGePoint3d& pointOnOtherCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	void			getClosestPointTo		(const CGePoint3d& point, CGePointOnCurve3d& pointOnCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	void			getClosestPointTo		(const CGeCurve3d& curve, CGePointOnCurve3d& pointOnThisCurve, CGePointOnCurve3d& pointOnOtherCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGePoint3d		projClosestPointTo		(const CGePoint3d& point, const CGeVector3d& projDir, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGePoint3d		projClosestPointTo		(const CGeCurve3d& curve, const CGeVector3d& projDir, CGePoint3d& pointOnOtherCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	void			getProjClosestPointTo	(const CGePoint3d& point, const CGeVector3d& projDir, CGePointOnCurve3d& pointOnCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	void			getProjClosestPointTo	(const CGeCurve3d& curve, const CGeVector3d& projDir, CGePointOnCurve3d& pointOnThisCurve, CGePointOnCurve3d& pointOnOtherCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			getNormalPoint			(const CGePoint3d& point, CGePointOnCurve3d& pointOnCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGeBoundBlock3d	boundBlock				() const;
	CGeBoundBlock3d	boundBlock				(const CGeInterval& range) const;
	CGeBoundBlock3d	orthoBoundBlock			() const;
	CGeBoundBlock3d	orthoBoundBlock			(const CGeInterval& range) const;
	CGeEntity3d*	project					(const CGePlane& projPlane, const CGeVector3d& projDir, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGeEntity3d*	orthoProject			(const CGePlane& projPlane, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isOn					(const CGePoint3d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isOn					(const CGePoint3d& point, double& param, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isOn					(double param, const CGeTolerance& tol = CGeContext::s_defTol) const;
	double			paramOf					(const CGePoint3d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;
	void			getTrimmedOffset		(double distance, const CGeVector3d& planeNormal, CGeVoidPointerArray& offsetCurveList, CGe::EOffsetCrvExtType extensionType = CGe::eFillet, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isClosed				(const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isPlanar				(CGePlane& plane, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isLinear				(CGeLine3d& line, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isCoplanarWith			(const CGeCurve3d& curve, CGePlane& plane, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isPeriodic				(double& period) const;
	double			length					(double fromParam, double toParam, double tol = CGeContext::s_defTol.equalPoint()) const;
	double			paramAtLength			(double datumParam, double length, bool bPositiveParamDir = true, double tol = CGeContext::s_defTol.equalPoint()) const;
	bool			area					(double fromParam, double toParam, double& value, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isDegenerate			(CGe::EEntityId& degenerateType, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isDegenerate			(CGeEntity3d*& pConvertedEntity, const CGeTolerance& tol = CGeContext::s_defTol) const;
	void			getSplitCurves			(double param, CGeCurve3d*& pCurve1, CGeCurve3d*& pCurve2) const;
	bool			explode					(CGeVoidPointerArray& explodedCurves, CGeIntArray& newExplodedCurves, const CGeInterval* pInterval = 0) const;
	void			getLocalClosestPoints	(const CGePoint3d& point, CGePointOnCurve3d& approxPoint, const CGeInterval* pInterval = 0, const CGeTolerance& tol = CGeContext::s_defTol) const;
	void			getLocalClosestPoints	(const CGeCurve3d& otherCurve, CGePointOnCurve3d& approxPointOnThisCurve, CGePointOnCurve3d& approxPointOnOtherCurve, const CGeInterval* pInterval1 = 0, const CGeInterval* pInterval2 = 0, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			hasStartPoint			(CGePoint3d& startPoint) const;
	bool			hasEndPoint				(CGePoint3d& endPoint) const;
	CGePoint3d		evalPoint				(double param) const;
	CGePoint3d		evalPoint				(double param, int numDeriv, CGeVector3dArray& derivArray) const;
	void			getSamplePoints			(double fromParam, double toParam, double approxEps, CGePoint3dArray& pointArray, CGeDoubleArray& paramArray) const;
	void			getSamplePoints			(int numSample, CGePoint3dArray& pointArray) const;
	CGeCurve3d&		operator =				(const CGeCurve3d& source);

protected:
	CGeCurve3d();
	CGeCurve3d(const CGeCurve3d& source);
};
