#ifndef PLANE_H
#define PLANE_H

#include "Surface.h"
#include "point.h"
#include "vector.h"
#include "trnsform.h"


// A plane is defined by original and normal, and optional u-axis.
// Its domain is assumed to be (-INFINITY, INFINITY) in both U and
// V direction.
//
// Author: 	Stephen W. Hou
// Date: 	June 13, 2004
//

class GEO_CLASS CPlane : public CSurface
{
public:

    enum PLANETYPE
    {
        TYPE_Xy,
        TYPE_Yz,
        TYPE_Zx,
        TYPE_Arbitrary
    };

	CPlane();

    CPlane(const C3Vector &normal);

	CPlane(const C3Point & origin,
		   const C3Vector & normal,
		   const C3Vector & uAxis = *(C3Vector*)NULL);

    CPlane(const CPlane &plane) { *this = plane; }

    CPlane(PLANETYPE planeType);

    virtual ~CPlane() { if (m_pToEcsXform) delete m_pToEcsXform; }

	virtual CSurface* 	Clone() const;

	void				SetOrigin(const C3Point &origin);
	C3Point				GetOrigin() const { return m_Origin; }

	void				SetNormal(const C3Vector &normal);
	const C3Vector &	GetNormal() const { return m_Normal; }

	void 				SetUAxis(const C3Vector & uAxis);
	C3Vector			GetUAxis() const { return m_UAxis; }

	virtual SURFACETYPE Type() const { return TYPE_Plane; }

    // Transform a plane.
    //
	virtual RC 			Transform(
							CTransform & cTransform);   // In: The transformation

	virtual RC 			TransformCopy(
							CTransform & cTransform,    // In: The transformation
							PSurface & pOut) const;     // Out: the transformed surface

    // Evaluate a point on a plane.
    //
	virtual RC 			Evaluate(
							double rU,                	// In: U parameter value to evaluate at
							double rV,                	// In: V parameter value to evaluate at
							C3Point & cPoint) const;  	// Out: Point on the surface with that parameter

    // Find the nearest point on a plane.
    //
	virtual RC 			NearestPoint(
							C3Point ptTo,			 	// In: The point we want to get closest to
							C3Point & ptNearest,     	// Out: The nearest point on the surface
							double & rU,            	// Out: Parameter value of Nearest in U direction
							double & rV,            	// Out: Parameter value of Nearest in V direction
							double & rDist) const;		// Out: The distance from the point to the surface

    // Perform ray-hit test and return true with hit point on the plane if
    // the plane is hit.
    //
	virtual BOOL 		HitTest( 	// Return TRUE if the ray hit the surface
							const CRay & ray, 			// In: The point we want to test
							double & rAt) const;		// Out: The position on ray

    // Project a point to a plane along a sepcified direction and return a point
    // on the plane w.r.t. WCS or the plane coordinate system.
    //
	virtual RC 			ProjectPoint(
							const C3Point & pt, 			// In: The point we want to test
							const C3Vector & along,		    // In: Along the dirction
							C3Point & rPt,                  // Out: Projected point on the surface
                            bool uvCoordinates = true) const;	// Optional In: TRUE to return the point in the plane coordinate system

    // Project a vector to a plane along a sepcified direction and return a vector
    // on the plane w.r.t. WCS or the plane coordinate system.
    //
    virtual RC 			ProjectVector(
							const C3Vector & vec, 			// In: The vector we want to test
							const C3Vector & along,		    // In: Along the dirction
							C3Vector & rVec,                // Out: Projected point on the surface
                            bool uvCoordinates = true) const;	// Optional In: TRUE to return the vector in the plane coordinate system

    // Compute distance from a point to a plane.
    //
	virtual double		DistanceTo(const C3Point & point) const;

    // Compute the intersection of two planes and return an intersection line if succese 
    // and NULL otherwise. The caller is responsible to free the memory allocated for the
    // intersection line.
    //
	CLine*				IntersectWith(const CPlane &plane) const;   // In:  The second plane

    // Compute the intersection of three planes and return TRUE with the intersection
    // point if there is an intersection, and FALSE otherwise. 
    //
    BOOL				IntersectWith(const CPlane &plane1,         // In:  The second plane
                                      const CPlane &plane2,         // In:  The third plane
                                      C3Point &intersection) const; // Out: The intersection point

    // Mirror a point
    //
	C3Point				Mirror(const C3Point & point) const;

    // Test if two planes are parallel.
    //
	BOOL 				IsParallelTo(const CPlane & plane) const;

    // Test if two planes are perpendicular.
    //
	BOOL 				IsPerpendicularTo(const CPlane & plane) const;

    // Test if a point is on the plane
    //
	BOOL				IsOnPlane(const C3Point & point) const;

    PLANETYPE           PlaneType() const;

    // Return a transform to transform an entity from WCS to plane coordinate system.
    //
    CAffine             WcsToEcsTransform() const;

    // Assign operator
    //
    CPlane&             operator=(const CPlane &plane);

    // Negate a plane
    //
    CPlane              operator-() const;
    CPlane&             operator-();

    // Test if two surfaces are equal

	virtual BOOL		Equal(const CSurface & surface) const;

private:

	C3Point		m_Origin;
	C3Vector	m_Normal;
	C3Vector	m_UAxis;

    CAffine*    m_pToEcsXform;       // cache for world to plane transform
};


typedef CPlane* PPlane;



#endif // PLANE_H