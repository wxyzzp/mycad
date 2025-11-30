// File: GeNurbCurve3d.h
//
// CGeNurbCurve3d
//

#pragma once


#include "GeSplineCurve3d.h"


class GE_API CGePolyline3d;
class GE_API CGeEllipArc3d;
class GE_API CGeLineSegment3d;


class GE_API CGeNurbCurve3d : public CGeSplineCurve3d
{
public:
	CGeNurbCurve3d();
	CGeNurbCurve3d(const CGeNurbCurve3d& source);
	CGeNurbCurve3d(int degree, const CGeKnotVector& knots, const CGePoint3dArray& controlPoints, bool bIsPeriodic = false);
	CGeNurbCurve3d(int degree, const CGeKnotVector& knots, const CGePoint3dArray& controlPoints, const CGeDoubleArray& weights, bool bIsPeriodic = false);
	CGeNurbCurve3d(int degree, const CGePolyline3d& fitPolyline, bool bIsPeriodic = false);
	CGeNurbCurve3d(const CGePoint3dArray& fitPoints, const CGeVector3d& startTangent, const CGeVector3d& endTangent, bool bStartTangentDefined = true, bool bEndTangentDefined = true, const CGeTolerance& fitTol = CGeContext::s_defTol);
	CGeNurbCurve3d(const CGePoint3dArray& fitPoints, const CGeTolerance& fitTol = CGeContext::s_defTol);
	CGeNurbCurve3d(const CGePoint3dArray& fitPoints, const CGeVector3dArray& fitTangents, const CGeTolerance& fitTol = CGeContext::s_defTol, bool bIsPeriodic = false);
	CGeNurbCurve3d(const CGeEllipArc3d&  ellipArc);
	CGeNurbCurve3d(const CGeLineSegment3d& lineSegment);

	int					numFitPoints				() const;
	bool				getFitPointAt				(int i, CGePoint3d& point) const;
	bool				getFitTolerance				(CGeTolerance& fitTol) const;
	bool				getFitTangents				(CGeVector3d& startTangent, CGeVector3d& endTangent) const;
	bool				getFitTangents				(CGeVector3d& startTangent, CGeVector3d& endTangent, bool& bStartTangentDefined, bool& bEndTangentDefined) const;
	bool				getFitData					(CGePoint3dArray& fitPoints, CGeTolerance& fitTol, bool& bTangentsExist, CGeVector3d& startTangent, CGeVector3d& endTangent) const;
	void				getDefinitionData			(int& degree, bool& bRational, bool& bPeriodic, CGeKnotVector& knots, CGePoint3dArray& controlPoints, CGeDoubleArray& weights) const;
	int					numWeights					() const;
	double				weightAt					(int i) const;
	bool				evalMode					() const;
	bool				getParamsOfC1Discontinuity	(CGeDoubleArray& params, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				getParamsOfG1Discontinuity	(CGeDoubleArray& params, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				setFitPointAt				(int i, const CGePoint3d& point);
	bool				addFitPointAt				(int i, const CGePoint3d& point);
	bool				deleteFitPointAt			(int i);
	bool				setFitTolerance				(const CGeTolerance& fitTol = CGeContext::s_defTol);
	bool				setFitTangents				(const CGeVector3d& startTangent, const CGeVector3d& endTangent);
	bool				setFitTangents				(const CGeVector3d& startTangent, const CGeVector3d& endTangent, bool bStartTangentDefined, bool bEndTangentDefined) const;
	CGeNurbCurve3d&		setFitData					(const CGePoint3dArray& fitPoints, const CGeVector3d& startTangent, const CGeVector3d& endTangent, const CGeTolerance& fitTol = CGeContext::s_defTol);
	CGeNurbCurve3d&		setFitData					(const CGeKnotVector& fitKnots, const CGePoint3dArray& fitPoints, const CGeVector3d& startTangent, const CGeVector3d& endTangent, const CGeTolerance& fitTol = CGeContext::s_defTol, bool bIsPeriodic = false);
	CGeNurbCurve3d&		setFitData					(int degree, const CGePoint3dArray& fitPoints, const CGeTolerance& fitTol = CGeContext::s_defTol);
	bool				purgeFitData				();
	CGeNurbCurve3d&		addKnot						(double newKnot);
	CGeNurbCurve3d&		insertKnot					(double newKnot);
	CGeSplineCurve3d&	setWeightAt					(int i, double weight);
	CGeNurbCurve3d&		setEvalMode					(bool bEvalMode = false);
	CGeNurbCurve3d&		joinWith					(const CGeNurbCurve3d& curve);
	CGeNurbCurve3d&		hardTrimByParams			(double newStartParam, double newEndParam);
	CGeNurbCurve3d&		makeRational				(double weight = 1.0);
	CGeNurbCurve3d&		makeClosed					();
	CGeNurbCurve3d&		makePeriodic				();
	CGeNurbCurve3d&		makeNonPeriodic				();
	CGeNurbCurve3d&		makeOpen					();
	CGeNurbCurve3d&		elevateDegree				(int plusDegree);
	CGeNurbCurve3d&		operator =					(const CGeNurbCurve3d& source);
};
