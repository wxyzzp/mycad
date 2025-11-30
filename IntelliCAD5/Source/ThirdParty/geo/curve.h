/*	CURVE.H - Defines the class of curves
 *	Copyright (C) 1994-98 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 *
 * A curve is an abstract class.  It abstracts the properties and methods that
 * are common to line segments, arcs, splines, etc.  It has some pure virtual
 * methods (those with =0) such as Evaluate. These must be supplied for every
 * derived class, typically depending on details of the derived class. It
 * also supports some general methods, such as Length. The implementation of
 * those is base on other methods, requireing no type-specific knowledge.
 * In many cases, e.g. Length, there may be a more efficient way to implement
 * the method for a specific types.  The length of a line segment for example is
 * just the distance between its endpoints.  In that case a type-specific
 * method can be added to override the general method.
 *
 */
#ifndef CURVE_H
#define CURVE_H

#include "geometry.h"
#include "Array.h"		// for the base class
#include "Roots.h"		// for the base class of CSpeed
#include "Point.h"		// for C3Point

class CPlane;
class C3Vector;

// Curve types

enum CURVETYPE {
	TYPE_Line,
	TYPE_Ray,
	TYPE_UnboundedLine,
	TYPE_Arc,
	TYPE_Elliptical,
	TYPE_NURBS,
	TYPE_PolyLine,
	TYPE_Gap,
    TYPE_Transformed,
    TYPE_Composite,
    TYPE_Mapped,
	TYPE_Unknown};

// Sides
enum SIDE {
	LEFT=-1,
	RIGHT=1,
	BOTH=2};


class GEO_CLASS CCurve  :  public CGeoObject
{
// Construction destruction
public:
    CCurve() : m_2dCurve(false)
		{
		}

	virtual ~CCurve();

// Abstract methods - required in all subclasses
	virtual PCurve Clone() const=0;

	virtual double Start() const =0;
	virtual double End() const =0;
	virtual double Period() const =0;

	virtual RC Evaluate(
		double rAt,					  // In: Parameter value
		C3Point & cPoint) const =0;   // Out: Point on the curve there

// Other mehtods
	virtual CURVETYPE Type() const;

	virtual int Complexity() const;

	// verify if the input parameter value is in the domain of curve
	//
	virtual BOOL PullIntoDomain(
		double & rT) const;   //In/out: Parameter value, pulled into the domain

	virtual BOOL Planar() const
		{
#ifdef THREE_D
		return FALSE;
#else
		return TRUE;
#endif
		}

	virtual BOOL AffineInvariant() const
		{
		return FALSE;
		}

	virtual BOOL Rational() const
		{
		return FALSE;
		}

	virtual BOOL Degenerate() const
		{
		return End() < Start() + FUZZ_GEN;
		}

	virtual RC Evaluate(
		double rAt,                // In: Parameter value to evaluate at
		C3Point & cPoint,   // Out: Point on the line with that parameter
		C3Point & cTangent) const; // Out: Tangent vector at that point

	virtual RC Evaluate(
		double rAt,                 // In: Where to evaluate
		int nDerivatives,           // In: Number of desired derivatives
		P3Point pValues) const;     // Out: Position and derivatives

	virtual RC Evaluate(
		double rAt,                 // In: Where to evaluate
		C3Point & ptPoint,			// Out: Point there
		C3Point & ptTangent,		// Out: Unit tangent vector there
		double & rCurvature,		// Out: Curvature there
		double * pSpeed=NULL,		// Out: Speed there (optional, NULL OK)
		P3Point pUp=NULL) const;	// In: Upward normal (optional, NULL OK)

	virtual RC EvaluateHomogeneous(
		double rAt,						// In: Parameter value
		C3Point & ptPoint,				// Out: Nomerator of the point there
		double & rDenominator) const;	// Out: Denominator of the point there

	virtual RC SnapEnd(
		SIDE nWhichEnd,    			// In: Start of end
		const C3Point & ptTo);		// In: The point to snap to

	virtual RC UpdateExtents(
		BOOL bTight,				// In: Tight extents if =TRUE
		BOOL bIgnoreStart,			// In: Ignore the startpoint, if =TRUE
		CExtents & cExtents,		// In/out: The extents, updated here
		PAffine pTransform=NULL);	// Optional: Transformation (NULL OK)

	virtual RC Wireframe(
		BOOL bIgnoreStart,		// In: Ignore the startpoint, if =TRUE
		CExtents & cExtents,	// In/out: The extents, updated here
		PAffine pTransform);	// Optional: Transformation (NULL OK)

	virtual RC Length(
		double & rLength) const;	// Out: The total length

	virtual RC UpdateAreaIntegral(
		double & rSoFar,			// In/Out: The integral so far
		P3Point pUp=NULL) const;	// In: Upward unit vector (NULL OK)

	virtual RC PullBack(
		const C3Point & ptTo,   // In: The point we want to get closest to
		double rSeed,           // In: Parameter value of the initial guess
		C3Point & ptNearest,    // Out: The nearest point on the curve
		double & rAt,           // Out: Parameter value of Nearest
		double & rSqDist) const;// Out: The distance squared

	virtual RC PullBack(
		C3Point ptTo,		     // In: The point we want to get closest to
		C3Point & ptNearest,     // Out: The nearest point on the curve
		double & rAt,            // Out: Parameter value of Nearest
		double & rDist,		     // Out: The distance from the point to the curve
		double rMargin=FUZZ_GEN, // In: Distance considered 0 (optional)
		double ignore=INFINITY)  // Optional: Parameter value to ignore
						const;

	virtual RC NearestPoint(
		C3Point ptTo,			 // In: The point we want to get closest to
		C3Point & ptNearest,     // Out: The nearest point on the curve
		double & rAt,            // Out: Parameter value of Nearest
		double & rDist,			 // Out: The distance from the point to the curve
		BOOL bIgnoreStart=FALSE) const; // Optional: Ignore startpoint if TRUE

    virtual BOOL HitTest(           // Return TRUE if the ray hits the cuvre
        const C3Point &pt,          // In: Through point 
        const C3Vector &dir,        // In: The hit direction 
        double rTolerance,          // In: The tolerance considered a hit
        double &rAt);               // Out: Hit location on the curve within tolearnce

	virtual BOOL HitTest(       // Return TRUE if the point is on the cuvre
		const C3Point& ptTo,    // In: The point we want to test
		double rMargin);        // In: The distance considered a hit

	int SnapToNearest(const C3Point &, const CRealArray &, double);

	virtual RC Linearize(
		double rResolution,			// In: Chord-height tolerance
		PAffine pTransform,			// In: Rendering transformation (NULL OK)
		CCache & cCache) const;     // In: The cache we render into

	virtual RC LinearizePart(
		double rFrom,               // In: Where to start
		double rTo,                 // In: Where to stop
		double rResolution,			// In: Chord-height toelrance
		PAffine pTransform,			// In: Rendering transformation (NULL OK)
		CCache & cCache) const;		// In: The cache we render into

	virtual BOOL Closed() const;

	virtual RC Seeds(
		CRealArray & arrSeeds,		// In/Out: Array of seeds
		double* pStart=NULL,		// In: Domain start (optional, NULL OK)
		double* pEnd=NULL) const;	// In: Domain end (optional, NULL OK)

	virtual RC SetDomain(
		double rFrom,  // In: New curve start
		double rTo);   // In: New curve end

	virtual RC ResetStart(
		double rNew);  // In: New curve start

	virtual RC ResetEnd(
		double rNew);  // In: New curve end

	virtual RC ReplaceWithRestriction(
		double rFrom,   // In: New curve start
		double rTo,     // In: New curve end
		PCurve & pOut);	// Out: The restricion to the new domain

	virtual RC Breaks(
		int iDeriv,                     // In: The required continuity
		PRealArray & pBreaks) const;	//Out: The breaks

#ifdef CURVE_SPLITTING
	virtual RC Split(
		double rAt,              // In: Paramter value to split at
		SIDE nSide,              // In: Which side to keep (possibly both)
		PCurve & pFirst,         // Out: The first half
		PCurve & pSecond) const; // Out: The second half
#endif

	virtual RC ArcLengthCopy(
		double rFrom,	// In: Domain start
		double rTo,		// In: Domain end
		PCurve & pCopy);	// Out: A constant speed copy

	virtual RC NextCusp(
		double t,			// In: A paramater value
		double & rCusp);	// Out: Parameter value of the next cusp after t

	// Cubic approximation stuff
	virtual RC CubicKnots(
		double rTolerance,	// In: The acceptable error
		PKnots & pKnots);	// Out: Knots for a fitting spline within tolerance

	RC CubicNextKnot(
		double rTolerance,		// In: The acceptable error
		double rSignificant,	// In: Step size lower bound
		double & rCurrent,		// In/Out: The current knot
		P3Point P,				// In/Out: Working array of 8 points
		double & rStep,			// Out: Step size to the next knot
		BOOL & bLast);			// Out: This is the last step, if TRUE

	virtual RC CubicError(		// Return E_NOMSG if the error is more than toelrance
		double rCurrent,		// In: The starting parameter of this span
		double rSpan,			// In: The length of this span
		double rTolerance,		// In: The maximum permitted error
		P3Point P,				// In/Out: Working array of points
		double & rError);		// Out: Approximate error

	virtual RC Transform(
		CTransform & cTransform);   // In: The transformation

	virtual RC TransformCopy(
		CTransform & cTransform,     // In: The transformation
		PCurve & pOut) const;        // Out: the transformed curve

	virtual RC Reverse();

	virtual RC SelfIntersect(
		CRealArray & cParameters, // In/Out: List of self intersection parameters
		double rTolerance);		  // Optional: Tolerance

	RC RandomPoint(
		double & rAt,           // Out: Random paramter well within the domain
		C3Point & ptPoint,		// Out: The point on the curve there
		C3Point & ptTangent);	// Out : The tangent vector there

	virtual RC Integrate(
		CRealFunction  &  cFunction, // In: The function
		double rFrom,                // In: Lower integration bound
		double rTo,                  // In: Upper integration bound
		double & rIntegral) const;	 // Out: The ingeral

	virtual RC Pack(
		LPBYTE & pStorage) const;  // In/Out: The storage

	virtual int PackingSize() const;

	BOOL SnapParameter(
		double rSTo,			 // In: Parameter value to snap to
		double rTolerance,    	 // In: Geometric Tolerance
		double & rSnap) const;	 // In: Parameter value to snap

	RC SearchInList(
		const CRealArray & cTo,	// In: List of parameter values to snap to
		double r,	        	// In: Parameter value to snap
		double rTolerance,    	// In: Geometric Tolerance
		int & nLocation,		// Out: Location in the list
		int & nSnap) const;		// Out: enrty r can be snapped to, -1 if it can't

	virtual RC GetCurvature(
		double rAt,					// In: Parameter value of a point on the curve
		double & rCurvature,		// Out: The curvature there
		P3Point pUp=NULL) const; 	// In: Upward normal (optional, NULL OK)

	virtual RC Move(
		const C3Point & ptBy);	// In: The desired origin's new position

	virtual RC GetApproximation(
		double rTolerance,		// In: Approxiation tolerance
		PCurve & pApprox); 		// Out: The approximation

	virtual RC Offset(
		double rDistance,		 // In: Offset signed distance (left if positive)
		PCurve & pOffset, 		 // Out: The approximation of the offset
		P3Point pUp=NULL) const; // In: Upward unit vector (NULL OK)

	virtual RC InitiatePath(
		PStorage pStorage) const;	// In/Out: The customer

	// Persist this curve
	virtual RC Persist(
		PStorage pStorage) const;	// In/Out: The customer

	virtual BOOL HorizontalBorder(
		BOOL bVisible,	// In: TRUE if this path is visble
		double rMaxX);	// In: X extent

	virtual RC AdmitOffset(
		double rDistance,		// In: The offset distance
		double rTolerance,		// In: The acceptable error
		P3Point pNormal);		// In: The Up direction (NULL OK)

	virtual RC GetStartPoint(
		C3Point & P) const   // Out: Curve's start point
		{
		return Evaluate(Start(), P);
		}

	virtual RC GetEndPoint(
		C3Point & P) const  // Out: Curve's end point
		{
		return Evaluate(End(), P);
		}

	virtual BOOL Bounded() const
		{
		return Start() > -INFINITY  &&   End() < INFINITY;
		}

	// Find the tangent line through a point. The function returns SUCCESS 
	// if the perpendicualar point is found and E_NOMSG otherwise.
	//
	virtual RC TangentThru(		// E_NOMSG if solution not found
		const C3Point& ptThru,	// In: The point on the tangent
		double rSeed,           // In: Parameter value of the initial guess
		double & rAt);			// Out: Parameter value of tangent

	// Find the perpendicular line through a point. The function returns SUCCESS 
	// if the perpendicualar point is found and E_NOMSG otherwise.
	//
	virtual RC PerpendicularThru(	// E_NOMSG if solution not found
		const C3Point & ptThru,		// In: Through point not on curve
        const C3Point & ptReference,// In: The optional eference point to select perpendicular point
		C3Point & ptPerpPoint,		// Out: Perpendicular point on the curve
		double & rAt				// Out: Parameter value of the perpendicular point
		) const { return false; }

	virtual RC UpdateIntercepts(
		PRay pRay,			// In: A ray
		int	& nIntercepts); // In/out: The number of intercepts

	virtual RC UpdateIntercepts(
		PUnboundedLine pLine, // In: A line
		BOOL bIgnoreOrigin,	  // In: Ignore the origin if TRUE
		double * pNearFore,   // In/out: Forward intercept nearest to line's origin
		double * pFarFore,    // In/out: Forward intercept farthest from line's origin
		double * pNearBack,	  // In/out: Backward intercept nearest to line's origin
		double * pFarBack);   // In/out: Backward intercept farthest from line's origin

	virtual RC GetPolylineEdges(
		P3Point pVertices,	// Out: an array of 2 points
		double* pBulges,	// Out: an array of 2 bulges
		int	& nEdges);		// Out: The number of resulting edges

	virtual RC NextSlice(	// Return E_NOMSG when it's the last slice
		int & i,			// In/out: An index marker
		PCurve & pSlice);	// Out: The current slice

	virtual BOOL CoalescedWith(
		PCurve pCurve)		// In/Out: the curve we try to coalesce with
		{
		return FALSE;
		}

    // Project a curve along a specified direction onto a specified plane
    // and return the projection of the curve. If the direction is not 
    // specified, the curve will be projected aginst the normal direction
    // of the plane.
    //
	virtual PCurve ProjectTo(const CPlane & toPlane,
		                     const C3Vector & direction	 = *(C3Vector*)NULL, // projected direction
                             bool get2dCurve = true     // true: a 2d curve in plane coordinate system
						    ) const { return NULL; }

	// Get the length of the portion of curve between two points.
	//
	virtual RC GetLength(double rFrom,		// In: Lower integration bound
						 double rTo,        // In: Upper integration bound
						 double &rLength) const; // Out: The length


	// Get a point on the curve at a specified arc length.
	//
	virtual RC FindPointByLength(double rLength,		// In:  The arc length
		                         double &rParam,		// Out: Parameter value of the point 
								 C3Point &point) const;	// Out: The point 

	// Get the points on the curve in the bound by subdividing the interval 
	// of the curve uniformly.
	//
	virtual RC GetPointsWithEqualInterval(int iNumSegments,			// In:  The number of points
		                                  CPointArray &ptArray,		// Out: The point array
										  CRealArray &parArray = *(CRealArray*)NULL) const; // Out: The optional parameter array

	// Get the points on the curve in the bopund by subdividing the legnth 
	// of the curve uniformly.
	//
	virtual RC GetPointsWithEqualArcLength(int iNumSegments,	    // In:  The number of points	
		                                   CPointArray &ptArray,	// Out: The point array
										   CRealArray &parArray = *(CRealArray*)NULL) const; // Out: The optional parameter array

    void    SetAs2DCurve(bool is2D) { m_2dCurve = is2D; }
    bool    Is2DCurve() const { return m_2dCurve; }

#ifdef DEBUG
	virtual void Dump() const;
#endif

// The following methods, support the Visio private interface GeomExIf,
// implemented in vertex.cpp outside Geo.
#ifdef GEOMEXIF
	virtual INT VertexInit (BOOL FAR & bPolyStarted, LPVOID FAR *control);  // returns vertex count
	virtual LPVOID VertexItem (INT itemNo, LPVOID control); // returns curve vertex
#endif

protected:

    bool        m_2dCurve;
};

// Utilities
RC UnpackCurve(
	LPBYTE & pStorage,  // In/Out: The storage
	PCurve & pCurve);   // Out: the unpacked curve

void UpdateNearAndFar(
	double contender,	  // In: Contender for the nearest/furthest position
	BOOL bIgnoreOrigin,	  // In: Ignore the origin if TRUE
	double * pNearFore,   // In/out: Forward intercept nearest to line's origin
	double * pFarFore,    // In/out: Forward intercept farthest from line's origin
	double * pNearBack,	  // In/out: Backward intercept nearest to line's origin
	double * pFarBack);    // In/out: Backward intercept farthest from line's origin

//////////////////////////////////////////////////////////////////////////
//																		//
//				THE SPEED OF A CURVE AS A REAL FUNCTION					//
//																		//
//////////////////////////////////////////////////////////////////////////
class GEO_CLASS CSpeed  :  public CRealFunction
{
public:
// Construct/destroy
	CSpeed()
		{
		m_pCurve = NULL;
		}

	CSpeed(
		PCCurve pCurve);  // In: The curve

	~CSpeed()
		{
		}

	PRealFunction Clone() const
		{
		return new CSpeed(m_pCurve);
		}

// Methods
	RC Evaluate(
		double rAt,         // In: Parameter value to evaluate at
		double & rValue);   // Out: xdy - ydx on the curve

	RC EvaluateInverse(
		double rAt,			// In: Parameter value
		double & rInverse);	// Out: 1/speed there
// Data
protected:
	PCCurve m_pCurve;
};

#endif


