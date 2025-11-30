// File: GeNurbSurface.cpp
//

#include "GeNurbSurface.h"

//////////////////////////////////////////////////
// CGeNurbSurface class implementation
//////////////////////////////////////////////////

CGeNurbSurface::CGeNurbSurface()
{
}

CGeNurbSurface::CGeNurbSurface(int degreeU, int degreeV, int propsInU, int propsInV, int numControlPointsInU, int numControlPointsInV, const CGePoint3d controlPoints[], const double weights[], const CGeKnotVector& uKnots, const CGeKnotVector& vKnots, const CGeTolerance& tol /* = CGeContext::s_defTol*/)
{
}

CGeNurbSurface::CGeNurbSurface(const CGeNurbSurface& source)
{
}

bool
CGeNurbSurface::isRationalInU() const
{
	return false;
}

bool
CGeNurbSurface::isPeriodicInU(double& u) const
{
	return false;
}

bool
CGeNurbSurface::isRationalInV() const
{
	return false;
}

bool
CGeNurbSurface::isPeriodicInV(double& v) const
{
	return false;
}

int
CGeNurbSurface::singularityInU() const
{
	return 0;
}

int
CGeNurbSurface::singularityInV() const
{
	return 0;
}

int
CGeNurbSurface::degreeInU() const
{
	return 0;
}

int
CGeNurbSurface::numControlPointsInU() const
{
	return 0;
}

int
CGeNurbSurface::degreeInV() const
{
	return 0;
}

int
CGeNurbSurface::numControlPointsInV() const
{
	return 0;
}

void
CGeNurbSurface::getControlPoints(CGePoint3dArray& points) const
{
}

bool
CGeNurbSurface::getWeights(CGeDoubleArray& weights) const
{
	return false;
}

int
CGeNurbSurface::numKnotsInU() const
{
	return 0;
}

void
CGeNurbSurface::getUKnots(CGeKnotVector& uKnots) const
{
}

int
CGeNurbSurface::numKnotsInV() const
{
	return 0;
}

void
CGeNurbSurface::getVKnots(CGeKnotVector& vKnots) const
{
}

void
CGeNurbSurface::getDefinition(int& degreeU, int& degreeV, int& propsInU, int& propsInV, int& numControlPointsInU, int& numControlPointsInV, CGePoint3dArray& controlPoints, CGeDoubleArray& weights, CGeKnotVector& uKnots, CGeKnotVector& vKnots) const
{
}

CGeNurbSurface&
CGeNurbSurface::set(int degreeU, int degreeV, int propsInU, int propsInV, int numControlPointsInU, int numControlPointsInV, const CGePoint3d controlPoints[], const double weights[], const CGeKnotVector& uKnots, const CGeKnotVector& vKnots, const CGeTolerance& tol /* = CGeContext::s_defTol*/)
{
	return *this;
}

CGeNurbSurface&
CGeNurbSurface::operator = (const CGeNurbSurface& source)
{
	return *this;
}
