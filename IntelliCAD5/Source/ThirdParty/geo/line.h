/*	Line.h - The calss of straight line segments
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 *
 * CLine is defined by 2 points, m_ptStart and m_ptEnd. Its domain is
 * assumed to be [0,1].  Resetting its domain will therefore fail. It
 * is the most compact and efficient way to represet a line segment for
 * rendering and hit testing, but it is awkward for modeling because
 * the parameterization changes when you trim it.
 *
 * CRay is is defined by a point m_ptStart and a direction vector
 * stored as m_ptEnd.  It mapping is = m_ptStart + t * m_ptEnd, for
 * 0<t<infinity.  Its endpoint, length, etc. are undefined.  Its domain
 * is always [0,INFINITY].
 *
 * CUnboundedLine may actually be bounded.  Its domain may vary.
 * (The naming is due to  * history, CLine should be CLineSegment,
 * and CUnboundedLine should be CLine). It can model an infinite line,
 * but it is also a better representation of line segments for modeling
 * operations (e.g. Booleans and offset)
 *
 */

#ifndef LINE_H
#define LINE_H

#include "Curve.h"

class GEO_CLASS CLine : public CCurve
{
// Construction destruction
public:
	CLine();

	CLine(
		const C3Point & A,	// In: 2 endpoints
		const C3Point & B);

	CLine(
		LPBYTE & pStorage);    // In/Out: The packed line

	CLine(
		PCUnboundedLine p,	// In: A line segment
		RC & rc);			// Out: Return code

	virtual ~CLine();
	virtual PCurve Clone() const;

// Inline methods
	virtual BOOL AffineInvariant() const
		{
		return TRUE;
		}

	virtual BOOL Degenerate() const
		{
		return PointDistance(m_ptStart, m_ptEnd) < FUZZ_GEN;
		}

    virtual void SetOrigin(const C3Point &origin)
        {
        m_ptStart = origin;
        }
	virtual const C3Point & Origin() const
		{
		return m_ptStart;
		}

	virtual C3Point Direction() const;

	virtual double Start() const
		{
		return 0;
		}

	virtual double End() const
		{
		return 1;
		}

	double Period() const
		{
		return 0;
		}

	virtual RC GetStartPoint(
		C3Point & P) const   // Out: Curve's start point
		{
		P = m_ptStart;
		return SUCCESS;
		}

	virtual RC GetEndPoint(
		C3Point & P) const   // Out: Curve's end point
		{
		P = m_ptEnd;
		return SUCCESS;
		}

	/********************************************************************************
	 * Author:	Dmitry Gavrilov.
	 * Purpose:	Get start & end line's points.
	 * Returns:	SUCCESS.
	 ********************************************************************************/
	virtual RC GetEnds(
		C3Point& StartPt, C3Point& EndPt) const   // Out: Curve's start & end points
		{
		StartPt = m_ptStart;
		EndPt = m_ptEnd;
		return SUCCESS;
		}

	virtual C3Point Velocity() const
		{
		return m_ptEnd - m_ptStart;
		}

	/********************************************************************************
	 * Author:	Dmitry Gavrilov.
	 * Purpose:	Set start line's point.
	 * Returns:	SUCCESS.
	 ********************************************************************************/
	virtual void SetStartPoint(
		const C3Point & ptSetTo)	// In: The point to set to
		{
		m_ptStart = ptSetTo;
		}

	virtual void SetEndPoint(
		const C3Point & ptSetTo)	// In: The point to set to
		{
		m_ptEnd = ptSetTo;
		}

// Methods
/*
 Our line's domain is always [0,1], so setting the domain not defined.
 The problem with that is that the user may try to set the domain, say
 for linearizing a part of the curve, and then reset it to the original
 domain.  The results will be incorrect on CLine.  That's whey
 SetDomain is NOT implemented.
*/
	virtual RC SetDomain(
		double rFrom,  // In: New curve start
		double rTo)    // In: New curve end
		{
		return FAILURE;
		}

	virtual RC ResetStart(
		double rNew);  // In: New curve start

	virtual RC ResetEnd(
		double rNew);  // In: New curve end

	virtual RC ReplaceWithRestriction(
		double rFrom,   // In: New curve start
		double rTo,     // In: New curve end
		PCurve & pOut);	// Out: The restricion to the new domain

	virtual CURVETYPE Type() const;

	int Complexity() const;

	virtual RC Evaluate (double, C3Point &) const;

	virtual RC Evaluate (double, C3Point &, C3Point &) const;

	virtual RC Evaluate(
		double rAt,					// In: Where to evaluate
		int nDerivatives,			// In: Number of desired derivatives
		P3Point pValues) const;		// Out: Position and derivatives

 	virtual RC Evaluate(
		double rAt,                 // In: Where to evaluate
		C3Point & ptPoint,			// Out: Point there
		C3Point & ptTangent,		// Out: Unit tangent vector there
		double & rCurvature,		// Out: Curvature there
		P3Point pUp=NULL) const;	// In: Upward normal (optional, NULL OK)

	virtual RC Length(
		double & rLength) const;   // Out: The total length

	virtual RC UpdateAreaIntegral(
		double & rSoFar,			// In/Out: The integral
		P3Point pUp=NULL) const;	// In: Upward unit normal (NULL OK)

	virtual RC Linearize(
		double rResolution,			// In: Chord-height toelrance
		PAffine pTransform,			// In: Rendering transformation (NULL OK)
		CCache & cCache) const;     // In: The cache we render into

	virtual RC LinearizePart(
		double rFrom,			// In: Where to start
		double rTo,				// In: Where to stop
		double rResolution,		// In: Chord-height toelrance
		PAffine pTransform,		// In: Rendering transformation (NULL OK)
		CCache & cCache) const;	// In: The cache we render into

	virtual RC Transform(
		CTransform & cTransform);   // In: The transformation

	virtual RC Reverse();

	virtual RC Seeds(
		CRealArray & arrSeeds,		// In/Out: Array of seeds
		double* pStart=NULL,		// In: Domain start (optional, NULL OK)
		double* pEnd=NULL) const;	// In: Domain end (optional, NULL OK)

	virtual RC SelfIntersect(
		CRealArray & cParameters,  // In/Out: Self intersection parameters
		double rTolerance);		   // Optional: Tolerance

	virtual RC UpdateExtents(
		BOOL bTight,				// In: Tight extents if =TRUE
		BOOL bIgnoreStart,			// In: Ignore the startpoint, if =TRUE
		CExtents & cExtents,		// In/out: The extents, updated here
		PAffine pTransform=NULL);	// Optional: Transformation (NULL OK)

	virtual RC Pack(
		LPBYTE & pStorage) const;  // In/Out: The storage

	virtual int PackingSize() const;

	virtual RC SnapEnd(
		SIDE nWhichEnd,    			   // In: Start of end
		const C3Point & ptTo); // In: The point to snap to

	virtual RC Move(
		const C3Point & ptBy);	// In: The desired origin's new position

	virtual RC GetApproximation(
		double rTolerance,		// In: Approxiation tolerance
		PCurve & pApprox); 		// Out: The approximation

	virtual RC Offset(
		double rDistance,			// In: Offset signed distance (left if positive)
		PCurve & pOffset, 			// Out: The offset
		P3Point pUp=NULL) const;	// In: Upward unit normal (NULL OK)

	virtual RC ArcLengthCopy(
		double rFrom,		// In: Domain start
		double rTo,			// In: Domain end
		PCurve & pCopy);	// Out: A constant speed copy

	virtual RC Persist(
		PStorage pStorage) const;	// In/Out: The customer

	virtual BOOL HorizontalBorder(
		BOOL bVisible,	// In: TRUE if this path is visble
		double rMaxX);	// In: X extent

	virtual RC AdmitOffset(
		double rDistance,	// In: The offset distance
		double rTolerance,	// In: The acceptable error
		P3Point pNormal);		// In: The Up direction (NULL OK)

	virtual RC PullBack(
		C3Point ptTo,		     // In: The point we want to get closest to
		C3Point & ptNearest,     // Out: The nearest point on the curve
		double & rAt,            // Out: Parameter value of Nearest
		double & rDist,		     // Out: The distance from the point to the curve
		double rMargin=FUZZ_GEN, // In: Distance considered 0 (optional)
		double ignore=INFINITY)  // In: Parameter value to ignore (optional)
						const;

	virtual RC PerpendicularThru(	// E_NOMSG if solution not found
		const C3Point & ptThru,		// In: Through point not on curve
        const C3Point & ptReference,// In: The optional eference point to select perpendicular point
		C3Point & ptPerpPoint,		// Out: Perpendicular point on the curve
		double & rAt				// Out: Parameter value of the perpendicular point
		) const;

	virtual RC UpdateIntercepts(
		PRay pRay,			// In: A ray
		int	& nIntercepts); // In/out: The number of intercepts

	virtual RC GetPolylineEdges(
		P3Point pVertices,	// Out: an array of 2 points
		double* pBulges,	// Out: an array of 2 bulges
		int	& nEdges);		// Out: The number of resulting edges

	virtual RC NextSlice(	// Return E_NOMSG when it's the last slice
		int & i,			// In/out: An index marker
		PCurve & pSlice);	// Out: The current slice

	virtual BOOL CoalescedWith(
		PCurve pCurve);		// In/Out: the curve we try to coalesce with

	virtual RC GetLength(double rFrom,		// In: Lower integration bound
						 double rTo,        // In: Upper integration bound
						 double &rLength) const; // Out: The length

	virtual RC FindPointByLength(double rLength,		// In:  The arc length
		                         double &rParam,		// Out: Parameter value of the point
								 C3Point &point) const;	// Out: The point

	virtual RC GetPointsWithEqualArcLength(int iNumSegments,				// In:  The number of points
		                                   CPointArray &ptArray,		// Out: The point array
										   CRealArray &parArray = *(CRealArray*)NULL) const; // Out: The optional parameter array

    // Project a curve along a specified direction onto a specified plane
    // and return the projection of the curve. If the direction is not
    // specified, the curve will be projected aginst the normal direction
    // of the plane.
    //
    virtual PCurve ProjectTo(const CPlane & toPlane,
		                     const C3Vector & direction = *(C3Vector*)NULL,
                             bool get2dCurve = false     // true: a 2d curve in plane coordinate system
						    ) const;

    virtual BOOL ProjectToXYPlane() 
        {   
            m_ptStart.SetZ(0.0); 
            m_ptEnd.SetZ(0.0); 
            return TRUE; 
        }

	virtual BOOL IsDegenerated() const;

    virtual BOOL HitTest(           // Return TRUE if the ray hits the cuvre
        const C3Point &pt,          // In: Through point 
        const C3Vector &dir,        // In: The hit direction 
        double rTolerance,          // In: The tolerance considered a hit
        double &rAt);               // Out: Hit location on the curve within tolearnce


#ifdef DEBUG
	void Dump() const;
#endif

// The following methods, support the Visio private interface GeomExIf,
// implemented in vertex.cpp outside Geo.
#ifdef GEOMEXIF
	virtual INT VertexInit (BOOL FAR & bPolyStarted, LPVOID FAR *control);  // returns vertex count
	virtual LPVOID VertexItem (INT itemNo, LPVOID control); // returns curve vertex
#endif


// Data
protected:
	C3Point m_ptStart;      // Start point
	C3Point m_ptEnd;		// End point
};
//////////////////////////////////////////////////////////////////
//																//
//						Infinite Ray							//
//																//
//////////////////////////////////////////////////////////////////
class GEO_CLASS CRay : public CLine
{
// Construction destruction
public:
	CRay()
		{
		}

	virtual ~CRay()
		{
		}

	CRay(
		const C3Point & ptPoint,       // In: a point
		const C3Point & vecDirection); // In: Direction vector

	CRay(LPBYTE & pStorage) : CLine(pStorage) {}   // In/Out: The packed line

    void SetDirection(const C3Point &dir)
        {
        m_ptEnd = dir;
        }
// Overriding methods
	virtual PCurve Clone() const;

	virtual double End() const
		{
		return INFINITY;
		}

	virtual C3Point Direction() const
		{
		return m_ptEnd;
		}

	virtual C3Point Velocity() const
		{
		return m_ptEnd;
		}

	virtual BOOL Degenerate() const
		{
		return FALSE;
		}

	virtual RC Evaluate(
		double rAt,
		C3Point & ptPoint) const;

	virtual RC Evaluate(
		double rAt,                  // In: Parameter value to evaluate at
		C3Point & ptPoint,		     // Out: Point on the line with that parameter
		C3Point & vecTangent) const; // Out: Tangent vector at that point

	virtual RC Evaluate(
		double rAt,                // In: Where to evaluate
		int nDerivatives,          // In: Number of desired derivatives
		PRealPoint pValues) const;  // Out: Position and derivatives

 	virtual RC Evaluate(
		double rAt,                  // In: Where to evaluate
		C3Point & ptPoint,			 // Out: End point
		C3Point & vecTangent,		 // Out: Unit tangent vector there
		double & rCurvature) const;  // Out: Curvature there

	virtual RC Linearize(
		double rResolution,		// In: Chord-height toelrance
		PAffine pTransform,		// In: Rendering transformation (NULL OK)
		CCache & cCache) const;	// In: The cache we render into

	virtual RC LinearizePart(
		double rFrom,			// In: Where to start
		double rTo,				// In: Where to stop
		double rResolution,		// In: Chord-height toelrance
		PAffine pTransform,		// In: Rendering transformation (NULL OK)
		CCache & cCache) const;	// In: The cache we render into

	virtual RC UpdateExtents(
		BOOL bTight,				// In: Tight extents if =TRUE
		BOOL bIgnoreStart,			// In: Ignore the startpoint, if =TRUE
		CExtents & cExtents,		// In/out: The extents, updated here
		PAffine pTransform=NULL);	// Optional: Transformation (NULL OK)

	virtual RC Wireframe(
		BOOL bIgnoreStart,		// In: Ignore the startpoint, if =TRUE
		CExtents & cExtents,	// In/out: The extents, updated here
		PAffine pTransform);	// Optional: Transformation (NULL OK)

	virtual RC Transform(
		CTransform & cTransform);   // In: The transformation

	virtual RC Move(
		const C3Point & ptBy);	// In: The desired origin's new position

	virtual RC GetEndPoint(
		C3Point & P) const   // Out: Curve's end point
		{
		return E_NOMSG;	// A ray has no endpoint
		}

	/********************************************************************************
	 * Author:	Dmitry Gavrilov.
	 * Purpose:	Get start & end ray's points.
	 * Returns:	E_NOMSG.
	 ********************************************************************************/
	virtual RC GetEnds(
		C3Point& StartPt, C3Point& EndPt) const   // Out: Ray's start & end points
		{
		StartPt = m_ptStart;
		return E_NOMSG;	// A ray has no endpoint
		}

	virtual BOOL PullIntoDomain(
		double & rT) const //In/out: Parameter value, pulled into the domain
        {
		return (rT >= 0.0) ? TRUE : FALSE;
		}
	virtual CURVETYPE Type() const { return TYPE_Ray; }

#ifdef CURVE_SPLITTING
	virtual RC Split(
		double rAt,              // In: Paramter value to split at
		SIDE nSide,              // In: Which side to keep (possibly both)
		PCurve & pFirst,         // Out: The first half
		PCurve & pSecond) const; // Out: The second half
#endif

	virtual RC ReplaceWithRestriction(
		double rFrom,   // In: New curve start
		double rTo,     // In: New curve end
		PCurve & pOut);	// Out: The restricion to the new domain

	virtual RC Length(
		double & rLength) const   // Out: The total length
		{
		// The length of a ray is not defined
		return FAILURE;
		}

	virtual RC UpdateAreaIntegral(
		double & rSoFar,			// In/Out: The integral
		P3Point pUp=NULL) const		// In: Upward unit normal (NULL OK)
		{
		// The area bounded by a ray is not defined
		return FAILURE;
		}

	virtual RC Reverse()
		{
		// A ray cannot be reversed
		return FAILURE;
		}

	virtual RC GetPolylineEdges(
		P3Point pVertices,	// Out: an array of 2 points
		double* pBulges,	// Out: an array of 2 bulges
		int	& nEdges)		// Out: The number of resulting edges
		{
		return FAILURE;
		}

	virtual RC InitiatePath(
		PStorage pStorage) const;	// In/Out: The Storage

	virtual RC Persist(
		PStorage pStorage) const;	// In/Out: The customer

	virtual RC NextSlice(	// Return E_NOMSG when it's the last slice
		int & i,			// In/out: An index marker
		PCurve & pSlice)	// Out: The current slice
		{
		return CCurve::NextSlice(i, pSlice);
		}

	virtual void SetEndPoint(
		const C3Point & ptSetTo);	// In: The point to set to

	virtual BOOL CoalescedWith(
		PCurve pCurve);		// In/Out: the curve we try to coalesce with

	virtual RC GetPointsWithEqualInterval(int iNumSegments,			// In:  The number of points
		                                  CPointArray &ptArray,		// Out: The point array
										  CRealArray &parArray = *(CRealArray*)NULL) const // Out: The optional parameter array
		{
			return FAILURE;
		}

	virtual RC GetPointsWithEqualArcLength(int iNumSegments,			// In:  The number of points
		                                   CPointArray &ptArray,		// Out: The point array
										   CRealArray &parArray = *(CRealArray*)NULL) const // Out: The optional parameter array
		{
			return FAILURE;
		}

	virtual PCurve ProjectTo(const CPlane & toPlane,  
		                     const C3Vector & direction = *(C3Vector*)NULL,
                             bool get2dCurve = false     // true: a 2d curve in plane coordinate system
						    ) const;

	virtual BOOL IsDegenerated() const { return FALSE; }

// No data, the direction vector is stored in m_ptEnd
	};
//////////////////////////////////////////////////////////////////
//																//
//						Unbounded Line							//
//																//
//////////////////////////////////////////////////////////////////
class GEO_CLASS CUnboundedLine : public CRay
{
// Construction destruction
public:
	CUnboundedLine()
		{
		m_rStart = m_rEnd = 0;
		}

	CUnboundedLine(
		const C3Point & ptPoint,      // In: a point
		const C3Point & vecDirection) // In: Direction vector
		:CRay(ptPoint, vecDirection)
		{
		m_rStart = -INFINITY;
		m_rEnd = INFINITY;
		}

	CUnboundedLine(LPBYTE & pStorage) : CRay(pStorage) {}   // In/Out: The packed line

	CUnboundedLine(
		PCLine pLine);

	CUnboundedLine(
		const CUnboundedLine & other);

	virtual ~CUnboundedLine()
		{
		}

// Overriding methods
	virtual PCurve Clone() const;

	virtual double Start() const
		{
		return m_rStart;
		}

	virtual double End() const
		{
		return m_rEnd;
		}

	virtual BOOL Degenerate() const
		{
		return End() < Start() + FUZZ_GEN;
		}

	virtual RC ResetStart(
		double rNew);  // In: New curve start

	virtual RC ResetEnd(
		double rNew);  // In: New curve start

	virtual RC SetDomain(
		double rFrom,  // In: New curve start
		double rTo);   // In: New curve end

	virtual RC ReplaceWithRestriction(
		double rFrom,   // In: New curve start
		double rTo,     // In: New curve end
		PCurve & pOut);	// Out: The restricion to the new domain

	virtual RC GetStartPoint(
		C3Point & P) const;   // Out: Curve's start point

	virtual RC GetEndPoint(
		C3Point & P) const;   // Out: Curve's start point

	virtual RC GetEnds(
		C3Point& StartPt, C3Point& EndPt) const;   // Out: Lines's start & end points

	virtual CURVETYPE Type() const
	    {
		return TYPE_UnboundedLine;
		}

	virtual BOOL PullIntoDomain(
		double & rT) const   	 //In/out: Parameter value, pulled into the domain
		{
		return CCurve::PullIntoDomain(rT);
		}

#ifdef CURVE_SPLITTING
	virtual RC Split(
		double rAt,              // In: Paramter value to split at
		SIDE nSide,              // In: Which side to keep (possibly both)
		PCurve & pFirst,         // Out: The first half
		PCurve & pSecond) const; // Out: The second half
#endif

	virtual RC Length(
		double & rLength) const;   // Out: The total length

	virtual RC UpdateAreaIntegral(
		double & rSoFar,			// In/Out: The integral
		P3Point pUp=NULL) const;	// In: Upward unit normal (NULL OK)

	virtual RC GetPolylineEdges(
		P3Point pVertices,	// Out: an array of 2 points
		double* pBulges,	// Out: an array of 2 bulges
		int	& nEdges);		// Out: The number of resulting edges

	virtual RC Reverse();

	virtual RC Linearize(
		double rResolution,		// In: Chord-height toelrance
		PAffine pTransform,		// In: Rendering transformation (NULL OK)
		CCache & cCache) const;	// In: The cache we render into

	virtual RC LinearizePart(
		double rFrom,			// In: Where to start
		double rTo,				// In: Where to stop
		double rResolution,		// In: Chord-height toelrance
		PAffine pTransform,		// In: Rendering transformation (NULL OK)
		CCache & cCache) const;	// In: The cache we render into

	virtual RC UpdateExtents(
		BOOL bTight,				// In: Tight extents if =TRUE
		BOOL bIgnoreStart,			// In: Ignore the startpoint, if =TRUE
		CExtents & cExtents,		// In/out: The extents, updated here
		PAffine pTransform=NULL);	// Optional: Transformation (NULL OK)

	virtual RC InitiatePath(
		PStorage pStorage) const;	// In/Out: The Storage

	virtual RC Persist(
		PStorage pStorage) const;	// In/Out: The customer

	virtual PCurve ProjectTo(const CPlane & toPlane,   
		                     const C3Vector & direction = *(C3Vector*)NULL,
                             bool get2dCurve = false     // true: a 2d curve in plane coordinate system
						    ) const;

	virtual BOOL IsDegenerated() const;

	// Data
	// The direction vector is stored in m_ptEnd
protected:
	double m_rStart;	// Domain start
	double m_rEnd;		// Domain end
	};
//////////////////////////////////////////////////////////////////
//																//
//					Gap (invisible line)						//
//																//
//////////////////////////////////////////////////////////////////
class GEO_CLASS CGap : public CLine
{

public:
// Construction destruction
	CGap()
		{
		}

	CGap(
		const C3Point & A,	// In: 2 endpoints
		const C3Point & B)
		:CLine(A, B)
		{
		}

	virtual ~CGap()
		{
		}

	virtual PCurve Clone() const;

	CURVETYPE Type() const
		{
		return TYPE_Gap;
		}

	virtual RC Linearize(
		double rResolution,			// In: Chord-height toelrance
		PAffine pTransform,			// In: Rendering transformation (NULL OK)
		CCache & cCache) const;     // In: The cache we render into

	virtual RC LinearizePart(
		double rFrom,			// In: Where to start
		double rTo,				// In: Where to stop
		double rResolution,		// In: Chord-height toelrance
		PAffine pTransform,		// In: Rendering transformation (NULL OK)
		CCache & cCache) const;	// In: The cache we render into

	virtual RC Length(
		double & rLength) const;   // Out: The total length

	virtual RC PullBack(
		C3Point ptTo,		     // In: The point we want to get closest to
		C3Point & ptNearest,     // Out: The nearest point on the curve
		double & rAt,            // Out: Parameter value of Nearest
		double & rDist,		     // Out: The distance from the point to the curve
		double rMargin=FUZZ_GEN, // In: Ignored here
		double ignore=INFINITY)  // Optional: Parameter value to ignore
						const
		{
		// There is no interior point in the gap
		return E_NOMSG;
		}

	virtual BOOL CoalescedWith(
		PCurve pCurve);		// In/Out: the curve we try to coalesce with

	virtual RC Persist(
		PStorage pStorage) const;	// In/Out: The customer

// No data
	};

///////////////////////////////////////////////////////////////////
//		Utilities
GEO_API RC IntersectLines(
	const C3Point & A,   // In: Point on the first line
	const C3Point & U,   // In: Direction vector of the first line
	const C3Point & B,   // In: Point on the second line
	const C3Point & V,   // In: Direction vector of the second line
	C3Point & P);        // In: Intersection point, if exists and unique

#endif
