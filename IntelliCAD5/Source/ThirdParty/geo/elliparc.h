/*	EllipArc.h - Defines the class of elliptical arcs
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 *
 * An elliptical arc is represented by a mapping of the type
 * P(t) = C + A cos(t) + B sin(t), where C is the center, and  A and B
 * are 2 perpendicular vectors. The vector A is called Major, and B
 * Minor, but it is nowhere assumed that |A|>|B|.
 * It is assumed, hoever that they are mutually perpendicular.
 * This class doesn't have a specific Length method.
 *
 */
#ifndef ELLIPARC_H
#define ELLIPARC_H

#include "curve.h"

class C3Vector;
class CPlane;
class CPolynomial;


class GEO_CLASS CEllipArc : public CCurve
{
// Construction destruction
public:
	CEllipArc();

// Construct from raw data
	CEllipArc(
		const C3Point & ptCenter, // In: Ellipse's center
		const C3Point & ptMajor,  // In: Major axis
		const C3Point & ptMinor,  // In: Minor axis
		double rStart,			   // In: Start angle
		double rEnd);			   // In: End angle

	// Construct from 3 points, angle and eccentricity
	CEllipArc(
		const C3Point & A,	// In: Arc's first endpoint
		const C3Point & B,	// In: An interior point on the arc
		const C3Point & C,	// In: Arc's last endpoint
		double rAngle,		// In: Angle between major axis and the X axis
		double rRatio,		// In: Major axis / minor axis
		RC &  rc);			// Out: Return code

	CEllipArc(
		const C3Point & ptCenter, // In: Ellipse's center
		double rMajor,			   // In: Length of major axis
		double rMinor,			   // In: Length of minor axis
		double rAngle,			   // In: Angle between major axis and the X axis
		RC &   rc);				   // Out: Return code

	// Construct a circular arc from 2 points and bow
	CEllipArc(
		const C3Point & A,	// In: Arc's first endpoint
		double rBow, 		// In: Arc's bow - see explanation below
		const C3Point & B,	// In: Arc's last endpoint
		RC & rc);           // Out: Return code

	CEllipArc(
		const C3Point & ptCenter,	// In: Center
		double rRadius,				// In: Radius
		RC & rc);					// Out: Return code

    CEllipArc(
        const C3Point &ptCenter,    // In: Center
        double rRadius,             // In: Radius
        const C3Vector &normal,     // In: Normal
        RC & rc);                   // Out: Return code

	//Constructor a circular arc from 2 points, a tangent and the radius
	CEllipArc(
		const C3Point & A, // In: Arc's first point
		const C3Point & U, // In: Tangent vector at A
		const C3Point & B, // In: Arc's last point
		double pRadius,    // In: Radius
		RC & rc);		   // Out: Return code

	CEllipArc(
		double rLeft,			  // In: Ellipse's minimal x
		double rTop,			  // In: Ellipse's maximal y
		double rRight,			  // In: Ellipse's maximal x
		double rBottom,			  // In: Ellipse's minimal y
		const C3Point & ptStart,  // In: Defines Arc's starting radius
		const C3Point & ptEnd,	  // In: Defines Arc's ending radius
		RC & rc);				  // Out: Return code

	CEllipArc(
		double rRadius, 		   // In: Radius
		const C3Point & ptCenter,  // In: Center
		double rStart,			   // In: Start angle, must be < end angle
		double rSweep,			   // In: Sweep angle
		RC & rc);				   // Out: Return code

	CEllipArc(
		double rRadius, 		   // In: Radius
		const C3Point & ptCenter,  // In: Center
        const C3Vector & normal,   // In: Normal
		double rStart,			   // In: Start angle, must be < end angle
		double rSweep,			   // In: Sweep angle
		RC & rc);				   // Out: Return code

	CEllipArc(
		int iWhich,					// In: Which quarter, according to quadrant
		const C3Point & ptCenter,	// In: Arc's center
		double rMajor,				// In: Length of major axis
		double rMinor);				// In: Length of minor axis

	CEllipArc(
		double rLeft,				// In: Ellipse's minimal x
		double rTop,				// In: Ellipse's maximal y
		double rRight,				// In: Ellipse's maximal x
		double rBottom);			// In: Ellipse's minimal y

	CEllipArc(
		const C3Point & A, // In: Arc's first endpoint
		const C3Point & B, // In: Arc's last endpoint
		double rBulge, 	   // In: Tan(1/4 arc's angle), negative if clockwise
		RC &   rc,         // Out: Return code
		P3Point pN=NULL);  // In: Normal (optional, NULL OK)

	virtual ~CEllipArc();

	virtual PCurve Clone() const;

// Inline methods
	virtual BOOL AffineInvariant() const
		{
		return TRUE;
		}

	double Start() const
		{
		return m_rStart;
		}

	double End() const
		{
		return m_rEnd;
		}

	double Period() const
		{
		return TWOPI;
		}

	BOOL Planar() const
		{
		return TRUE;
		}

	const C3Point & Center() const
		{
		return m_ptCenter;
		}

	const C3Point & MajorAxis() const
		{
		return m_ptMajor;
		}

	const C3Point & MinorAxis() const
		{
		return m_ptMinor;
		}


	// Methods
	CURVETYPE Type() const;

	int Complexity() const;

	BOOL RecalcAxes(	 // Return TRUE if actually recalculated
		double& rShift); // Out: The angle shift of the new major axis
	RC Length(
		double & rLength) const;   // Out: The total length

	double Radius() const;

	BOOL IsCircular(
		double & rRadius)  const; // Out: The length of the longer axis

	// Test if the input point is lying on the plane of the ellipse
	//
	BOOL IsOnPlane(const C3Point &pt) const; // In: a point

	RC Evaluate(
		double rAt,					// In: Parameter value to evaluate at
		C3Point & ptPoint) const;	// Out: Point on the arc at that parameter

	RC Evaluate(
		double rAt,				 // In: Where to evaluate
	C3Point & ptPoint,			 // Out: Point on the arc with that parameter
	C3Point & ptTangent) const;  // Out: Tangent vector at that point

	RC Evaluate(
		double rAt,                 // In: Where to evaluate
		int nDerivatives,           // In: Number of desired derivatives
		PRealPoint pValues) const;  // Out: Position and derivatives

	virtual RC LinearizePart(
		double rFrom,			// In: Where to start
		double rTo,				// In: Where to stop
		double rResolution,		// In: Chord-height toelrance
		PAffine pTransform,		// In: Rendering transformation (NULL OK)
		CCache & cCache) const;	// In: The cache we render into


	RC UpdateAreaIntegral(
		double & rSoFar,			// In/Out: The integral
		P3Point pUp=NULL) const;	// In: Upward unit normal (NULL OK)

	BOOL Closed() const;

	RC Transform(
		CTransform & cTransform);   // In: The transformation

	RC Reverse();

	RC UpdateExtents(
		BOOL bTight,				// In: Tight extents if =TRUE
		BOOL bIgnoreStart,			// In: Ignore the startpoint, if =TRUE
		CExtents & cExtents,		// In/out: The extents, updated here
		PAffine pTransform=NULL);	// Optional: Transformation (NULL OK)

	RC SelfIntersect(
		CRealArray & cParameters, // In/Out: List of self intersection parameters
		double rTolerance);		  // Optional: Tolerance

	RC AngleToPoint(
		const C3Point & P,      // In: The point
		double & rAngle) const;	// Out: The angle

	// Utilities
	RC CenterFrom3Pts(const C3Point &,
	                  const C3Point &,
	                  const C3Point &);

	RC GetAngles(const C3Point &,
	             const C3Point &,
	             const C3Point &);

	RC RayAngle(
		C3Point P, // In: A point on the ray
		double & rAngle);	 // Out: The parameter value

	void FlipMinorAxis();

	RC Move(
		const C3Point & ptBy);	// In: The desired origin's new position

	RC CubicKnots(
		double rTolerance,	// In: The acceptable error
		PKnots & pKnots);	// Out: Knots for a fitting spline within tolerance

	RC GetApproximation(
		double rTolerance,		// In: Approxiation tolerance
		PCurve & pApprox); 		// Out: The approximation

	RC Offset(
		double rDistance,			// In: Offset signed distance (left if positive)
		PCurve & pOffset, 			// Out: The approximation of the offset
		P3Point pUp=NULL) const; // In: Upward unit normal (optional)

	RC ArcLengthCopy(
		double rFrom,	// In: Domain start
		double rTo,		// In: Domain end
		PCurve & pCopy);	// Out: A constant speed copy

	virtual RC InitiatePath(
		PStorage pStorage) const;	// In/Out: The Storage

	virtual RC Persist(
		PStorage pStorage) const;	// In/Out: The customer

	RC AdmitOffset(
		double rDistance,	// In: The offset distance
		double rTolerance,	// In: The acceptable error
		P3Point pNormal);	// In: The Up direction (NULL OK)

	virtual RC SetDomain(
		double rFrom,  // In: New curve start
		double rTo);   // In: New curve end

	virtual RC ResetStart(
	double rNew);  // In: New curve start

	virtual RC ResetEnd(
	double rNew);  // In: New curve end

	RC TryAxisTip(
		double angle,		  // In: The angle of that axis
		const C3Point & axis, // In: The axis with that angle
		C3Point & ptNearest,  // Out: The nearest point on the curve
		double & rAt,		  // Out: Parameter value of Nearest
		double & rDist) const;// Out: The distance

	RC PullBack(
		C3Point ptTo,		     // In: The point we want to get closest to
		C3Point & ptNearest,     // Out: The nearest point on the curve
		double & rAt,            // Out: Parameter value of Nearest
		double & rDist,		     // Out: The distance from the point to the curve
		double rMargin=FUZZ_GEN, // In: Ignored here
		double ignore=INFINITY)  // In: Parameter value to ignore (optional)
						const;

	RC UpdateIntercepts(
		PRay pRay,		     // In: A ray
		int	& nIntercepts);  // In/out: The number of intercepts

	RC TangentThru(		// E_NOMSG if solution not found
		const C3Point& ptThru,	// In: The point on the tangent
		double rSeed,           // In: Parameter value of the initial guess
		double & rAt);			// Out: Parameter value of tangent

	virtual RC PerpendicularThru(	// E_NOMSG if solution not found
		const C3Point & ptThru,		// In: The point to find a perpendicular point on the curve
        const C3Point & ptReference,// In: The optional eference point to select perpendicular point
        C3Point & ptPerpPoint,		// Out: Perpendicular point on the curve
		double & rAt				// Out: Parameter value of the perpendicular point
		) const;

	virtual RC GetPolylineEdges(
		P3Point pVertices,	// Out: an array of 2 points
		double* pBulges,	// Out: an array of 2 bulges
		int	& nEdges);		// Out: The number of resulting edges

	virtual PCurve ProjectTo(const CPlane & toPlane,
		                     const C3Vector & direction = *(C3Vector*)NULL,
                             bool get2dCurve = false
						    ) const;

    virtual BOOL HitTest(           // Return TRUE if the ray hits the cuvre
        const C3Point &pt,          // In: Through point 
        const C3Vector &dir,        // In: The hit direction 
        double rTolerance,          // In: The tolerance considered a hit
        double &rAt);               // Out: Hit location on the curve within tolearnce

    C3Vector GetNormal() const;

    BOOL SwapAxes();

    BOOL CEllipArc::IsOnXYPlane() const;

    BOOL CEllipArc::IsCoplanarWith(const CEllipArc &ell) const;

#ifdef DEBUG
	void Dump() const;
#endif

// The following methods, support the Visio private interface GeomExIf,
// implemented in vertex.cpp outside Geo.
#ifdef GEOMEXIF
	virtual INT VertexInit (BOOL FAR & bPolyStarted, LPVOID FAR *control);  // returns vertex count
	virtual LPVOID VertexItem (INT itemNo, LPVOID control); // returns curve vertex

protected:
	// Special case for circular arcs
	INT CircularVertexInit (BOOL FAR & bPolyStarted, LPVOID FAR *control);
	LPVOID CircularVertexItem (INT itemNo, LPVOID control);
#endif

// Data
protected:

	double	m_rStart;	// Starting angle
	double	m_rEnd;		// End angle
	C3Point	m_ptCenter; // Ellipse's center
	C3Point	m_ptMajor;  // Major axis
	C3Point	m_ptMinor;	// Minor axis
};
////////////////////////////////////////////////////////////////////////
//		Definition of CEllipse
class GEO_CLASS CEllipse : public CEllipArc
{
// Construction destruction
public:
	CEllipse()
		{
		}

	CEllipse(
		const C3Point & C,  // In: Ellipse's center
		const C3Point & P,	// In: A point
		const C3Point & Q); // In: Another point

	CEllipse(
		const C3Point & ptCenter,       // In: Ellipse's center
		const C3Vector & major,	        // In: Major axis
		const C3Vector & minor)         // In: Minor axis
        : CEllipArc(ptCenter, major.AsPoint(), minor.AsPoint(), 0.0, TWOPI)
        {
        }

	CEllipse(
		const C3Point & ptCenter,	// In: Center
		double rRadius,				// In: Radius
		RC & rc)					// Out: Return code
		:CEllipArc(ptCenter, rRadius, rc)
		{
		}

    CEllipse(
        const C3Point &ptCenter,    // In: Center
        double rRadius,             // In: Radius
        const C3Vector &normal,     // In: Normal
        RC & rc)                   // Out: Return code
        :CEllipArc(ptCenter, rRadius, normal, rc)
        {
        }

	CEllipse(
		const CEllipArc & other)	// In: An ellipArc to copy
		:CEllipArc(other)
		{
		}

	// Mehtods
	virtual PCurve Clone() const;

#ifdef CURVE_SPLITTING
	RC Split(
		double rAt,              // In: Paramter value to split at
		SIDE nSide,              // In: Which side to keep (possibly both)
		PCurve & pFirst,         // Out: The first half
		PCurve & pSecond) const; // Out: The second half
#endif

	virtual RC ReplaceWithRestriction(
		double rFrom,   // In: New curve start
		double rTo,     // In: New curve end
		PCurve & pOut);	// Out: The restricion to the new domain

	virtual RC Persist(
		PStorage pStorage) const;	// In/Out: The customer

// No data
	};


#endif
