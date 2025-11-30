// File: GeNurbCurve2d.h
//
// CGeNurbCurve2d
//

#pragma once


#include "GeSplineCurve2d.h"


class GE_API CGePolyline2d;
class GE_API CGeEllipArc2d;
class GE_API CGeLineSegment2d;


class GE_API CGeNurbCurve2d : public CGeSplineCurve2d
{
public:
	CGeNurbCurve2d();
	CGeNurbCurve2d(const CGeNurbCurve2d& source);
	CGeNurbCurve2d(int degree, const CGeKnotVector& knots, const CGePoint2dArray& controlPoints, bool bIsPeriodic = false);
	CGeNurbCurve2d(int degree, const CGeKnotVector& knots, const CGePoint2dArray& controlPoints, const CGeDoubleArray& weights, bool bIsPeriodic = false);
	CGeNurbCurve2d(int degree, const CGePolyline2d& fitPolyline, bool bIsPeriodic = false);
	CGeNurbCurve2d(const CGePoint2dArray& fitPoints, const CGeVector2d& startTangent, const CGeVector2d& endTangent, bool bStartTangentDefined = true, bool bEndTangentDefined = true, const CGeTolerance& fitTol = CGeContext::s_defTol);
	CGeNurbCurve2d(const CGePoint2dArray& fitPoints, const CGeTolerance& fitTol = CGeContext::s_defTol);
	CGeNurbCurve2d(const CGePoint2dArray& fitPoints, const CGeVector2dArray& fitTangents, const CGeTolerance& fitTol = CGeContext::s_defTol, bool bIsPeriodic = false);
	CGeNurbCurve2d(const CGeEllipArc2d&  ellipArc);
	CGeNurbCurve2d(const CGeLineSegment2d& lineSegment);

	int					numFitPoints				() const;
	bool				getFitPointAt				(int i, CGePoint2d& point) const;
	bool				getFitTolerance				(CGeTolerance& fitTol) const;
	bool				getFitTangents				(CGeVector2d& startTangent, CGeVector2d& endTangent) const;
	bool				getFitData					(CGePoint2dArray& fitPoints, CGeTolerance& fitTol, bool& tangentsExist, CGeVector2d& startTangent, CGeVector2d& endTangent) const;
	void				getDefinitionData			(int& degree, bool& bRational, bool& bPeriodic, CGeKnotVector& knots, CGePoint2dArray& controlPoints, CGeDoubleArray& weights) const;
	int					numWeights					() const;
	double				weightAt					(int idx) const;
	bool				evalMode					() const;       
	bool				getParamsOfC1Discontinuity	(CGeDoubleArray& params, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				getParamsOfG1Discontinuity	(CGeDoubleArray& params, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				setFitPointAt				(int i, const CGePoint2d& point);
	bool				addFitPointAt				(int i, const CGePoint2d& point);
	bool				deleteFitPointAt			(int i);
	bool				setFitTolerance				(const CGeTolerance& fitTol = CGeContext::s_defTol);
	bool				setFitTangents				(const CGeVector2d& startTangent, const CGeVector2d& endTangent);
	CGeNurbCurve2d&		setFitData					(const CGePoint2dArray& fitPoints, const CGeVector2d& startTangent, const CGeVector2d& endTangent, const CGeTolerance& fitTol = CGeContext::s_defTol);
	CGeNurbCurve2d&		setFitData					(const CGeKnotVector& fitKnots, const CGePoint2dArray& fitPoints, const CGeVector2d& startTangent, const CGeVector2d& endTangent, const CGeTolerance& fitTol = CGeContext::s_defTol, bool bIsPeriodic = false);
	CGeNurbCurve2d&		setFitData					(int degree, const CGePoint2dArray& fitPoints, const CGeTolerance& fitTol = CGeContext::s_defTol);
	bool				purgeFitData				();
	CGeNurbCurve2d&		addKnot						(double newKnot);
	CGeNurbCurve2d&		insertKnot					(double newKnot);
	CGeSplineCurve2d&	setWeightAt					(int i, double weight);
	CGeNurbCurve2d&		setEvalMode					(bool bEvalMode = false);
	CGeNurbCurve2d&		joinWith					(const CGeNurbCurve2d& curve);
	CGeNurbCurve2d&		hardTrimByParams			(double newStartParam, double newEndParam);
	CGeNurbCurve2d&		makeRational				(double weight = 1.0);
	CGeNurbCurve2d&		makeClosed					();
	CGeNurbCurve2d&		makePeriodic				();
	CGeNurbCurve2d&		makeNonPeriodic				();
	CGeNurbCurve2d&		makeOpen					();
	CGeNurbCurve2d&		elevateDegree				(int plusDegree);
	CGeNurbCurve2d&		operator =					(const CGeNurbCurve2d& source);
};
