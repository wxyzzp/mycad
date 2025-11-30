#ifndef SURFACE_H
#define SURFACE_H


// A surface is an abstract class.  It abstracts the properties and
// methods that are common to plane, cylinder, cone, sphere, torus,
// spline, and offset surfaces etc.
//
// Author: 	Stephen W. Hou
// Date: 	June 13, 2004
//

#include "array.h"



class C3Point;
class C3Vector;
class CRay;


enum SURFACETYPE
{
	TYPE_Plane,
	TYPE_Cylinder,
	TYPE_Cone,
	TYPE_Sphere,
	TYPE_Torus,
	TYPE_Spline,
	TYPE_Offset
};


class GEO_CLASS CSurface : public CGeoObject
{
public:

	CSurface() {}
	virtual ~CSurface();

    // Clone a surface
    //
	virtual CSurface* 	Clone() const = 0;

	virtual SURFACETYPE Type() const = 0;

    // Transform a surface.
    //
	virtual RC 			Transform(
							CTransform & cTransform);   // In: The transformation

	virtual RC 			TransformCopy(
							CTransform & cTransform,    // In: The transformation
							CSurface* & pOut) const;    // Out: the transformed surface

    // Evaluate a point on a surface.
    //
	virtual RC 			Evaluate(
							double rU,                	// In: U parameter value to evaluate at
							double rV,                	// In: V parameter value to evaluate at
							C3Point & cPoint) const;  	// Out: Point on the surface with that parameter

    // Find the nearest point on a surface.
    //
	virtual RC 			NearestPoint(
							C3Point ptTo,			 	// In: The point we want to get closest to
							C3Point & ptNearest,     	// Out: The nearest point on the surface
							double & rU,            	// Out: Parameter value of Nearest in U direction
							double & rV,            	// Out: Parameter value of Nearest in V direction
							double & rDist) const;		// Out: The distance from the point to the surface

    // Perform ray-hit test and return true with hit point on the surface if
    // the surface is hit.
    //
	virtual BOOL 		HitTest( 	// Return TRUE if the ray hit the surface
							const CRay & ray, 			// In: The point we want to test
							double & rAt) const;		// Out: The position on ray

    // Project a point to a surface along a sepcified direction and return a point
    // on the surface w.r.t. WCS or the surface's coordinate system.
    //
	virtual RC 			ProjectPoint( 	
							const C3Point & pt, 		// In: The point we want to test
							const C3Vector & along,		// In: Along the dirction
							C3Point & rPt,		        // Out: Projected point on the surface
                            bool uvCoordinates = false) const;	// In: TRUE to return the point in the surface coordinate system

    // Project a vector to a surface along a sepcified direction and return a vector
    // on the surface w.r.t. WCS or the surface's coordinate system.
    //
    virtual RC 			ProjectVector(
							const C3Vector & vec, 			// In: The vector we want to test
							const C3Vector & along,		    // In: Along the dirction
							C3Vector & rVec,                // Out: Projected point on the surface
                            bool uvCoordinates = false) const;	// In: TRUE to return the vector in the surface coordinate system

    // Test if two surfaces are equal
    //
    virtual BOOL        Equal(const CSurface &surface) const;

    // Compute distance from a point to a surface.
    //
	virtual double		DistanceTo(const C3Point & point) const { return -1.0; }
};


typedef CSurface* PSurface;


#endif // SURFACE_H