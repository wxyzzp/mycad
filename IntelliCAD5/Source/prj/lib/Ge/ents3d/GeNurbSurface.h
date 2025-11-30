// File: GeNurbSurface.h
//
// CGeNurbSurface
//

#pragma once


#include "GeSurface.h"


class GE_API CGeKnotVector;


class GE_API CGeNurbSurface : public CGeSurface
{
public:
	CGeNurbSurface();
	CGeNurbSurface(int degreeU, int degreeV, int propsInU, int propsInV, int numControlPointsInU, int numControlPointsInV, const CGePoint3d controlPoints[], const double weights[], const CGeKnotVector& uKnots, const CGeKnotVector& vKnots, const CGeTolerance& tol = CGeContext::s_defTol);
	CGeNurbSurface(const CGeNurbSurface& source);

	bool			isRationalInU		() const;
	bool			isPeriodicInU		(double& u) const;
	bool			isRationalInV		() const;
	bool			isPeriodicInV		(double& v) const;
	int				singularityInU		() const;
	int				singularityInV		() const;
	int				degreeInU			() const;
	int				numControlPointsInU	() const;
	int				degreeInV			() const;
	int				numControlPointsInV	() const;
	void			getControlPoints	(CGePoint3dArray& points) const;
	bool			getWeights			(CGeDoubleArray& weights) const;
	int				numKnotsInU			() const;
	void			getUKnots			(CGeKnotVector& uKnots) const;
	int				numKnotsInV			() const;
	void			getVKnots			(CGeKnotVector& vKnots) const;
	void			getDefinition		(int& degreeU, int& degreeV, int& propsInU, int& propsInV, int& numControlPointsInU, int& numControlPointsInV, CGePoint3dArray& controlPoints, CGeDoubleArray& weights, CGeKnotVector& uKnots, CGeKnotVector& vKnots) const;
	CGeNurbSurface&	set					(int degreeU, int degreeV, int propsInU, int propsInV, int numControlPointsInU, int numControlPointsInV, const CGePoint3d controlPoints[], const double weights[], const CGeKnotVector& uKnots, const CGeKnotVector& vKnots, const CGeTolerance& tol = CGeContext::s_defTol);
	CGeNurbSurface&	operator =			(const CGeNurbSurface& source);
};
