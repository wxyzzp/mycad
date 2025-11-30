//
// This file implements the member functions of the abstract surface.
//
// Author: 	Stephen W. Hou
// Date: 	June 13, 2004
//

#include "surface.h"
#include "point.h"
#include "vector.h"
#include "trnsform.h"

CSurface::~CSurface()
{
}


RC CSurface::Transform(CTransform & cTransform)
{
	return FAILURE;
}


RC CSurface::TransformCopy(CTransform & cTransform,    	// In: The transformation
						   PSurface & pOut) const       // Out: the transformed surface
{
	pOut = NULL;
	return FAILURE;
}


RC CSurface::Evaluate(double rU,                	// In: U parameter value to evaluate at
					  double rV,                	// In: V parameter value to evaluate at
					  C3Point & cPoint) const  		// Out: Point on the surface with that parameter
{
	return FAILURE;
}


RC CSurface::NearestPoint(C3Point ptTo,			 	// In: The point we want to get closest to
						  C3Point & ptNearest,     	// Out: The nearest point on the surface
						  double & rU,            	// Out: Parameter value of Nearest in U direction
						  double & rV,            	// Out: Parameter value of Nearest in V direction
						  double & rDist) const		// Out: The distance from the point to the surface
{
	return FAILURE;
}


BOOL CSurface::HitTest(const CRay & ray, double & rAt) const
{
	return FALSE;
}


RC CSurface::ProjectPoint(const C3Point &pt, 		// In: The point we want to test
						  const C3Vector & along,	// In: Along the dirction
						  C3Point & rPt,	        // Out: Projected point on the surface
                          bool uvCoordinates) const
{
	return FAILURE;
}


RC CSurface::ProjectVector(const C3Vector &vec, 	// In: The point we want to test
						  const C3Vector & along,	// In: Along the dirction
						  C3Vector & rVec,	        // Out: Projected point on the surface
                          bool uvCoordinates) const
{
	return FAILURE;
}


BOOL CSurface::Equal(const CSurface & surface) const
{
	return FALSE;
}


