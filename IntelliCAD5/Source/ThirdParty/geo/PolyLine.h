/*	PolyLine.h - Defines the class CPolyLine.
 *	Copyright (C) 1998 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * CPolyLine is a polygonal curve.  It is parameterized so that each
 * segment spans an interval of unit length, starting at 0.  In other 
 * words, segment(i) occupies the interval [i, i+1], from i=0.  If
 * periodic, the last segment is assumed to go back the the first 
 * point. CPolyline is derived from CCurve and CPointArray
 *
 */

#include "Curve.h"		// For the base class
#include "PtArray.h"	// For the base class

#ifndef POLYLINE_H
#define POLYLINE_H


class C3Vector;
class CPlane;


//class CPolyLine	:	public CCurve, public CControlPoints
class GEO_CLASS CPolyLine	:	public CCurve, public CPointArray
	{
public:
	// Constructors/destructor
	CPolyLine()
		{
		}

	CPolyLine(
		const CPolyLine& cOther, // In: Another polyline to copy
		RC& rc)					 // Out: Return code
		:CPointArray(cOther, rc)
		{
		}

	CPolyLine(
		int count,	// In: Number of points
		RC& rc)		// Out: Return code
		:CPointArray(count, rc)
		{
		}

	virtual ~CPolyLine()
		{
		}

	// Inline methods
	virtual BOOL AffineInvariant() const
		{
		return TRUE;
		}

	C3Point& Vertex(
		int i) const	// In: Index of the desired vertex
		{
		return (*this)[i];
		}

	int SegmentCount() const
		{
		return Size()-1;
		}

	CURVETYPE Type() const
		{
		return TYPE_PolyLine;
		}

	int Complexity() const;

	double Start() const
		{
		return 0;
		}

	double End() const
		{
		return Size() - 1;
		}

	double Period() const
		{
		return 0;
		}

	// Other methods
	PCurve Clone() const;

	RC Linearize(
		double rResolution,			// In: Resolution - ignored here
		PAffine pTransform,			// In: Rendering transformation (NULL OK)
		CCache & cCache) const;     // In/Out: The cache we render into

	RC Integrate(
		CRealFunction &  cFunction, // In: The function
		double rFrom,               // In: Lower integration bound
		double rTo,                 // In: Upper integration bound
		double & rIntegral) const;  // Out: The integral

	RC Evaluate(
		double rAt,				 // In: Parameter value
		C3Point & ptPoint) const;// Out: Point on the curve there

	RC Evaluate(
		double rAt,				   // In: Parameter value
		C3Point & ptPoint,		   // Out: Point on the spline there
		C3Point & ptTangent) const;// Out: Tangent vector at that point

	RC Evaluate(
		double rAt,             // In: Where to evaluate
		int nDerivatives,       // In: NUmber of desired derivatives
		P3Point pValues) const ;// Out: Position and derivatives, ROOM FOR 3

	RC UpdateExtents(
		BOOL bTight,			// In: Ignored here
		BOOL bIgnoreStart,		// In: Ignore the startpoint, if =TRUE
		CExtents & cExtents,	// In/out: The extents, updated here
		PAffine pTransform);	// Optional: Transformation (NULL OK)

	RC Move(
		const C3Point & ptBy);	// In: The desired origin's new position

	RC Breaks(
		int iDeriv,                 // In: The required continuity
		PRealArray & pBreaks) const;//Out: The breaks

	RC Transform(
		CTransform & cTransform);   // In: The transformation

	RC Seeds(
		CRealArray & arrSeeds,  // In/Out: Array of seeds
		double* pStart,			// In: Domain start (optional, NULL OK)
		double* pEnd) const; 	// In: Domain end (optional, NULL OK)

	RC SnapEnd(
		SIDE nWhichEnd,   	   // In: Start or end
		const C3Point & ptTo); // In: The point to snap to 

	RC NextCusp(  // Return E_NOMSG if there is no cusp
		double t,		// In: A paramater value
		double & rCusp);// Out: Parameter value of the next cusp after t, or curve-end

	RC Persist(
		PStorage pStorage) const;	// In/Out: The storage

#ifdef CURVE_SPLITTING
	RC OpenUp();

	RC Split(
		double rAt,              // In: Paramter value to split at
		SIDE nSide,              // In: Which side to keep (possibly both)
		PCurve & pFirst,         // Out: The first half
		PCurve & pSecond) const; // Out: The second half
#endif

	RC PullBack(
		C3Point ptTo,		     // In: The point we want to get closest to
		C3Point & ptNearest,     // Out: The nearest point on the curve
		double & rAt,            // Out: Parameter value of Nearest
		double & rDist,		     // Out: The distance from the point to the curve
		double rMargin=FUZZ_GEN, // In: Distance considered 0 (optional)
		double ignore=INFINITY)  // In: Parameter value to ignore (optional)
						const;

	RC UpdateIntercepts(
		PRay pRay,			// In: A ray 
		int	& nIntercepts); // In/out: The number of intercepts

	RC UpdateIntercepts(
		PUnboundedLine pLine, // In: A line 
		BOOL bIgnoreOrigin,	  // In: Ignore the origin if TRUE
		double * pNearFore,   // In/out: Forward intercept nearest to line's origin
		double * pFarFore,    // In/out: Forward intercept farthest from line's origin
		double * pNearBack,	  // In/out: Backward intercept nearest to line's origin
		double * pFarBack);   // In/out: Backward intercept farthest from line's origin

	virtual RC NextSlice(	// Return E_NOMSG when it's the last slice
		int & i,			// In/out: An index marker
		PCurve & pSlice);	// Out: The current slice

	virtual RC ResetStart(
		double rNew);  // In: New curve start
	
	virtual RC ResetEnd(
		double rNew);  // In: New curve end

	virtual RC LinearizePart(
		double rFrom,			// In: Where to start
		double rTo,				// In: Where to stop
		double rResolution,		// In: Chord-height toelrance
		PAffine pTransform,		// In: Rendering transformation (NULL OK)
		CCache & cCache) const;	// In: The cache we render into

	virtual PCurve ProjectTo(const CPlane & toPlane,   
		                     const C3Vector & direction = *(C3Vector*)NULL,
                             bool get2dCurve = false
						    ) const;

	RC Reverse();

// The following methods, support the Visio private interface GeomExIf, 
// implemented in vertex.cpp outside Geo.  
#ifdef GEOMEXIF
	virtual INT VertexInit (BOOL FAR & bPolyStarted, LPVOID FAR *control);  // returns vertex count
	virtual LPVOID VertexItem (INT itemNo, LPVOID control); // returns curve vertex
#endif

	bool	IsPointInside(const C3Point& PointToCheck) const;

	bool	IsClockwiseProjection() const;

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


private:
	int Localize(
		double &t)	const;		 // In/out: Parameter value

// No data
	};

//////////////////////////////////////////////////////////////////////////
//																		//
//						Ray/Line Interceptions							//

// Base clase
class GEO_CLASS CInterceptor
	{
public:
	// Constructors/destructors
	CInterceptor();

	virtual ~CInterceptor()
		{
		}

	CInterceptor(
		PLine pLine,	// In: The line to be intercepted
		RC & rc);		// Out: Return code

	// Methods
	BOOL FindIntercept(
		C3Point & ptNew,	// In: A point
		double & rOnLine,	// Out: Intersection parameter on the line
		double & ratio);	// Out: Ratio of intersection on the segment

	// Data
protected:
	PLine m_pLine;		// The line being intercepted
	C3Point m_V;		// Coefficients of the line's equation
	double m_c;			// as V*X - c = 0
	C3Point m_P;		// Current point
	double m_rValue;	// Current value of P*V-c
	double m_rSpeed;	// Line's speed
	BOOL m_bFirst;		// =TRUE until the first point has been processed
	};
#endif


