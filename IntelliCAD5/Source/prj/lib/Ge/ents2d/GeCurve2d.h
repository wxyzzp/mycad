// File: GeCurve2d.h
//
// CGeCurve2d
//

#pragma once


#include "GeEntity2d.h"
#include "GeArrays.h"


class GE_API CGeInterval;
class GE_API CGePointOnCurve2d;
class GE_API CGeBoundBlock2d;


class GE_API CGeCurve2d : public CGeEntity2d
{
public:
	virtual ~CGeCurve2d();

	void			getInterval				(CGeInterval& interval) const;
	void			getInterval				(CGeInterval& interval, CGePoint2d& start, CGePoint2d& end) const;
	CGeCurve2d&		reverseParam			();
	CGeCurve2d&		setInterval				();
	bool			setInterval				(const CGeInterval& interval);
	double			distanceTo				(const CGePoint2d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;
	double			distanceTo				(const CGeCurve2d& curve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGePoint2d		closestPointTo			(const CGePoint2d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGePoint2d		closestPointTo			(const CGeCurve2d& curve, CGePoint2d& pointOnOtherCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	void			getClosestPointTo		(const CGePoint2d& point, CGePointOnCurve2d& pointOnCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	void			getClosestPointTo		(const CGeCurve2d& curve, CGePointOnCurve2d& pointOnThisCurve, CGePointOnCurve2d& pointOnOtherCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			getNormalPoint			(const CGePoint2d& point, CGePointOnCurve2d& pointOnCurve, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGeBoundBlock2d	boundBlock				() const;
	CGeBoundBlock2d	boundBlock				(const CGeInterval& range) const;
	CGeBoundBlock2d	orthoBoundBlock			() const;
	CGeBoundBlock2d	orthoBoundBlock			(const CGeInterval& range) const;
	bool			isOn					(const CGePoint2d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isOn					(const CGePoint2d& point, double& param, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isOn					(double param, const CGeTolerance& tol = CGeContext::s_defTol) const;
	double			paramOf					(const CGePoint2d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isClosed				(const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isLinear				(CGeLine2d& line, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isPeriodic				(double& period) const;
	double			length					(double fromParam, double toParam, double tol = CGeContext::s_defTol.equalPoint()) const;
	double			paramAtLength			(double datumParam, double length, bool bPositiveParamDir = true, double tol = CGeContext::s_defTol.equalPoint()) const;
	bool			area					(double fromParam, double toParam, double& value, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isDegenerate			(CGe::EEntityId& degenerateType, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isDegenerate			(CGeEntity2d*& pConvertedEntity, const CGeTolerance& tol = CGeContext::s_defTol) const;
	void			getSplitCurves			(double param, CGeCurve2d*& pCurve1, CGeCurve2d*& pCurve2) const;
	bool			explode					(CGeVoidPointerArray& explodedCurves, CGeIntArray& newExplodedCurves, const CGeInterval* pInterval = 0) const;
	void			getLocalClosestPoints	(const CGePoint2d& point, CGePointOnCurve2d& approxPoint, const CGeInterval* pInterval = 0, const CGeTolerance& tol = CGeContext::s_defTol) const;
	void			getLocalClosestPoints	(const CGeCurve2d& otherCurve, CGePointOnCurve2d& approxPointOnThisCurve, CGePointOnCurve2d& approxPointOnOtherCurve, const CGeInterval* pInterval1 = 0, const CGeInterval* pInterval2 = 0, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			hasStartPoint			(CGePoint2d& startPoint) const;
	bool			hasEndPoint				(CGePoint2d& endPoint) const;
	CGePoint2d		evalPoint				(double param) const;
	CGePoint2d		evalPoint				(double param, int numDeriv, CGeVector2dArray& derivArray) const;
	void			getSamplePoints			(double fromParam, double toParam, double approxEps, CGePoint2dArray& pointArray, CGeDoubleArray& paramArray) const;
	void			getSamplePoints			(int numSample, CGePoint2dArray& pointArray) const;
	void			getTrimmedOffset		(double distance, CGeVoidPointerArray& offsetCurveList, CGe::EOffsetCrvExtType extensionType = CGe::eFillet, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGeCurve2d&		operator =				(const CGeCurve2d& source);

protected:
	CGeCurve2d();
	CGeCurve2d(const CGeCurve2d& source);
};
