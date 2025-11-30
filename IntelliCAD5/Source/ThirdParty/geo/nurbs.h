/*	NURBS.h - Defines the class of NURBS curves
 *	Copyright (C) 1994-99 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 * A NURBS curve is a spline with a denmoninator.  The denominator is
 * a spline function with the same knots.  Its coefficients are called
 * weights. Consult a CAD textbook, e.g. Farin's "Curves and
 * Surfaces for Computer Aided Geometric Design", for details.
 * The curve is defined by a knot sequence (with a degree),control points,
 * and possibly weights.
 * For performance reasons, the control points are stored multiplied by
 * their weights.  This is sometimes called homogeneous representation.
 *
 */
#ifndef NURBS_H
#define NURBS_H

#include "knots.h"
#include "CtrlPts.h"
#include "Curve.h"
#include "Roots.h"

class C3Vector;
class CPlane;

#define CSpline CNURBS


#ifdef DEBUG
class ostream;
#endif // DEBUG

class GEO_CLASS CNURBS : public CCurve
{
	friend class CNURBSExtentDerivative;

// Construction destruction
public:
	CNURBS();

	CNURBS(
		PKnots pKnots,					// In: Knots
		PControlPoints pControlPoints,	// In: Controlpoints
		PRealArray pWeights,			// In: Weights
		RC & rc);						// Out: Return code

	CNURBS(
		PKnots pKnots,					// In: Knots
		PControlPoints pControlPoints,	// In: Controlpoints
		PRealArray pWeights,			// In: Weights
		double startParam,				// In: Start parameter
		double endParam,				// In: End parameter
		double periodic,				// In::Periodic
		RC & rc);						// Out: Return code

	CNURBS(
		const CNURBS & Other,    // In: The other spline to copy
		RC & rc);				 // Out: Return code

	CNURBS(
		LPBYTE & pStorage,  // In/Out: The packed spline
		RC & rc);			// Out: Return code

	CNURBS(
		int nDegree,       // In: Degree
		int nPoints,       // In: Number of control points
		RC & rcErr);	   // Out: Return code

	CNURBS(
		const CNURBS & Other,   // In: The other spline to copy
		int nNewDegree,          // In: The desired new degree
		RC & rcErr);             // Out: Return code

	CNURBS(
		const CNURBS & Other,  // In:  The spline to represent as pw-Bezier
		BEZTYPE beztype, // In: Gives this constructor a unique signature
		RC & rcErr);     // Out:  Return code

	CNURBS(
		CPointArray & arrPoints,   // In: Array of points
		CRealArray & arrNodes,     // In: Their parameter nodes
		BOOL bPeriodic,            // In: =TRUE if the spline is to be periodic
		RC & rcErr);               // Out: Return code

	CNURBS(
		CPointArray & arrPoints, // In: Array of points
		CRealArray & arrNodes,   // In: Their parameter nodes
		CRealArray & arrKnots,   // In: Spline knots
		BOOL bPeriodic,          // In: =TRUE if it's to be periodic
		RC & rc);                // Out: Return code

	RC LeastSquares(
		CPointArray & arrPoints,   // In: Array of points
		CRealArray & arrNodes);    // In: Their parameter nodes

	RC PeriodicLeastSquares(
		CPointArray & arrPoints,   // In: Array of points
		CRealArray & arrNodes);    // In: Their parameter nodes

	CNURBS(
		PKnots & pKnots,           // In: Knot sequence
		PControlPoints & pPoints,  // In: Control points
		RC & rc);				   // Out: Return code

	CNURBS(
		CCurve & Curve,		// In: The curve to be approximated, assumed C2
		double rTolerance,	// In: The acceptable error
		RC & rc);			// Out: Return code

 	virtual ~CNURBS();

// Methods
	void Purge();

	RC Sanitize();

	PCurve Clone() const;

	CURVETYPE Type() const
		{
		return 	TYPE_NURBS;
		}

	int Complexity() const;

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
		return m_rPeriod;
		}

	BOOL Planar() const
		{
		return m_bPlanar;
		}

	BOOL Closed() const;

	void SetPlanar(
		BOOL bValue)	// In: The value to set to
		{
		m_bPlanar = bValue;
		}

	virtual BOOL AffineInvariant() const
		{
		return TRUE;
		}

	BOOL Rational() const
		{
		return m_pWeights != NULL;
		}

	RC Breaks(
		int iDeriv,                  // In: The required continuity
		PRealArray & pBreaks) const; //Out: The breaks

	int Degree() const;

	int LastKnot() const;

	C3Point & ControlPoint(
		int nIndex) const; // Index of the desired control point

    double Knot(
		int nIndex) const;          // Index of desired knot

	double Weight(			// Return the weight of index i
		int i)	const;		// In: Index of the desired weight

	// Return number of control points
	//
	int PointCount() const;

	// Return number of knots
	//
	int KnotCount() const { return (m_pKnots) ? m_pKnots->KnotCount() : 0; }

	// Normalize parameter domain to [0, 1].
	//
	RC Normalized();

	// Rationalize curve
	//
	RC Rationalize();

	// Reparameterize spline to shift parameter domain to start at the
	// input value. The funciton doesn't change the size of parameter
	// domain.
	//
	RC  Reparameterized(
			double newStartParam);  // In: New start parameter

	// Reparameterize to scale the parameter domain.
	//
	RC Reparameterized(
		    double newStartParam,   // In: New start parameter
			double newEndParam);    // In: New end parameter

	RC SetWeight(
		int i,					// In: Index of the desired weight
		double w);				// In: value to set to

	RC EvaluateHomogeneous(
		double rAt,						// In: Parameter value
		C3Point & ptPoint,				// Out: Nomerator of the point there
		double & rDenominator) const;	// Out: Denominator of the point there

	RC Evaluate(
		double rAt,					// In: Parameter value
		C3Point & ptPoint) const;   // Out: Point on the spline there

	RC Evaluate(
		double rAt,					// In: Parameter value
		C3Point & ptPoint,		    // Out: Point on the spline there
		C3Point & ptTangent) const; // Out: Tangent vector at that point

	RC Evaluate(
		double rAt,                // In: Where to evaluate
		int nDerivatives,          // In: NUmber of desired derivatives
		P3Point pValues) const; // Out: Position and derivatives

	void SetControlPoint(
		int nIndex,                // In: The index of the knot to be set
		C3Point ptValue,		   // In: The value to set it to
		double* pWeight = NULL);   // In: Weight (optional)

	C3Point ControlPointGeom(
		int nIndex) const; // Index of the desired control point

	int RenderingNumber(
		int nSpan,				  // In: A span index
		double rTolerance) const; //In: The tolerated bow between curve and chord

	virtual RC LinearizePart(
		double rFrom,			// In: Where to start
		double rTo,				// In: Where to stop
		double rResolution,		// In: Chord-height toelrance
		PAffine pTransform,		// In: Rendering transformation (NULL OK)
		CCache & cCache) const;	// In: The cache we render into


	RC Approximate(
		const CCurve & Curve, // In: The mappping to approximate
		int nFirst,           // In: The index of the first missing control point
		int nMissing);        // In: The number of missing control point

	RC Seeds(
		CRealArray & arrSeeds,		// In/Out: Array of seeds
		double* pStart=NULL,		// In: Domain start (optional, NULL OK)
		double* pEnd=NULL) const;	// In: Domain end (optional, NULL OK)

	RC UpdateExtents(
		BOOL bTight,				// In: Tight extents if =TRUE
		BOOL bIgnoreStart,			// In: Ignore the startpoint, if =TRUE
		CExtents & cExtents,		// In/out: The extents, updated here
		PAffine pTransform=NULL);	// Optional: Transformation (NULL OK)

	RC UpdateDirectionExtents(
		const C3Point & vecDirection,  // In: The direction vector
		CExtents & cExtents,		   // In/out: The extents, updated here
		PAffine pTransform=NULL);	   // Optional: Transformation (NULL OK)

	void SetInterval();

	virtual RC InsertKnot(
	    double & rAt,			// In: Parameter value for the inserted knot
                        	// Out: An existing knot within rSnap, if there is one
		int nContinuity,    // In: The desired continuity at the knot
		int & nFirst,		// Out: The index of the first inserted knot
		int & nAdded);		// Out: The number of knots actually added

	RC InsertKnot(
	    double & rAt,		// In: Parameter value for the inserted knot
                        	// Out: An existing knot within rSnap, if there is one
		int nContinuity);   // In: The desired continuity at the knot

	RC Split(
		double rAt,              // In: Paramter value to split at
		SIDE nSide,              // In: Specify to keep left(first part), right(second part), or both
		PCurve & pFirst,         // Out: The first half
		PCurve & pSecond) const; // Out: The second half

	// Join a curve with this curve at a common end point to become a singal curve.
	// The input curve will be destroied if the operation is successful.
	//
	RC JoinWith(
		CNURBS* &pNurbs);		 // In: The curve to join to the end of this curve

	// Elevate the order
	//
	RC Elevate();

	// compute curve length
	double GetLength(
		double fromParam,		// In: Start parameter
		double toParam) const;	// In: End parameter

	double GetLength() const;

	RC SelfIntersect(
		CRealArray & cParameters, // In/Out: List of self intersection parameters
		double rTolerance);		  // Optional: Tolerance

	BOOL DisjointSpans(
		int  i,			// In: First span index
		int  j) const;  // In: Second span index

	RC Reverse();

	RC ReDoEnd(
		CRealArray & arrNodes,    // In: Parameter values for new data points
		CPointArray & arrPoints); // In: New interpolation points for the spline's end

	RC InvertNotAKnot(
		CRealArray & arrNodes);   // In: The parameter nodes for interpolation

	RC InvertPeriodic();

	virtual void SwapDataWith(
	    CNURBS & cOther);    // In: The other spline to swap data with

	void SetDegree(int);

	RC AddSpan(
		double rKnot,         // In: Parameter values for new knot
		C3Point & rptPoints); // In: New control point

	virtual void Initialize();


	void SetKnot(
		int nIndex,		 //In: The index of the knot to be set
		double rValue);  // In: The value to set it to

	RC Transform(
		CTransform & cTransform);   // In: The transformation

	RC Pack(
		LPBYTE & pStorage) const;  // In/Out: The storage

	int PackingSize() const;

	RC SnapClosed(
		double rRTolerance);

	RC SnapEnd(
		SIDE nWhichEnd,  		// In: Start of end
		const C3Point & rptTo); // In: The point to snap to

	RC SetDomain(
		double rFrom,  // In: New curve start
		double rTo);   // In: New curve end

	RC ResetStart(
	double rNew);  // In: New curve start

	RC ResetEnd(
	double rNew);  // In: New curve end

	RC Integrate(
		CRealFunction &  cFunction,		// In: The function
		double rFrom,                   // In: Lower integration bound
		double rTo,                     // In: Upper integration bound
		double & rIntegral) const;		// Out: The integral

	RC Move(
		const C3Point & rptBy);	// In: The desired origin's new position

	RC ArcLengthCopy(
		double rFrom,	// In: Domain start
		double rTo,		// In: Domain end
		PCurve & pCopy);	// Out: A constant speed copy

	RC NextCusp(
		double t,			// In: A paramater value
		double & rCusp);	// Out: Parameter value of the next cusp after t

	virtual RC InitiatePath(
		PStorage pStorage) const;	// In/Out: The customer

	virtual RC Persist(
		PStorage pStorage) const;	// In/Out: The customer

	RC GetApproximation(
		double rTolerance,		// In: Approxiation tolerance
		PCurve & pApprox); 		// Out: The approximation

	RC AdmitOffset(
		double rDistance,	// In: The offset distance
		double rTolerance,	// In: The acceptable error
		P3Point pNormal);	// In: The Up direction (NULL OK)

	BOOL GetNormal(// Return TRUE if the spline is planar
		C3Point & pNormal);		// Out: A unit normal if it is

	RC UpdateIntercepts(
		PRay pRay,			// In: A ray
		int	& nIntercepts); // In/out: The number of intercepts

	virtual RC UpdateIntercepts(
		PUnboundedLine pLine, // In: A line
		BOOL bIgnoreOrigin,	  // In: Ignore the origin if TRUE
		double * pNearFore,   // In/out: Forward intercept nearest to line's origin
		double * pFarFore,    // In/out: Forward intercept farthest from line's origin
		double * pNearBack,	  // In/out: Backward intercept nearest to line's origin
		double * pFarBack);   // In/out: Backward intercept farthest from line's origin

	BOOL HitTest( // Return TRUE if the point is on the spline
		const C3Point& ptTo, // In: The point we want to test
		double rMargin);	 // In: The distance considered a hit

	RC ReplaceWithRestriction(
		double rFrom,   // In: New curve start
		double rTo,     // In: New curve end
		PCurve & pOut);	// Out: The restricion to the new domain

	virtual RC NextSlice(	// Return E_NOMSG when it's the last slice
		int & i,			// In/out: An index marker
		PCurve & pSlice);	// Out: The current slice

	virtual RC PerpendicularThru(	// E_NOMSG if solution not found
		const C3Point & ptThru,		// In: Through point
        const C3Point & ptReference,// In: The optional eference point to select perpendicular point
		C3Point & ptPerpPoint,		// Out: Perpendicular point on the curve
		double & rAt				// Out: Parameter value of the perpendicular point
		) const;

	RC Subset(				// Return a sub curve
		double rFrom,		// In: Start parameter
		double rTo,			// In: End parameter
	    PCurve & pSubset	// Out: New curve
		) const;

	virtual PCurve ProjectTo(const CPlane & toPlane,   
		                     const C3Vector & direction = *(C3Vector*)NULL,
                             bool get2dCurve = false
						    ) const;

#ifdef DEBUG
	void Dump() const;

	GEO_FRIEND ostream&	operator<<(ostream &out, const CNURBS &spl);
#endif

// The following methods, support the Visio private interface GeomExIf,
// implemented in vertex.cpp outside Geo.
#ifdef GEOMEXIF
	virtual INT VertexInit (BOOL FAR & bPolyStarted, LPVOID FAR *control);  // returns vertex count
	virtual LPVOID VertexItem (INT itemNo, LPVOID control); // returns curve vertex
#endif

// Data
protected:
	double m_rStart;					// Domain start
	double m_rEnd;						// Domain end
	double m_rPeriod;					// Period
	BOOL m_bPlanar;						// Planar flag
	CKnots * m_pKnots;                  // Knots
	CControlPoints * m_pControlPoints;  // Control points
	PRealArray m_pWeights;				// Weights

/* The following overrides the = operator and the simple minded copy
   constructor as private methods.  The compiler will treat their useage
   by strangers as errors.  These methods are NOT implemented, so that
   friends and derived classes will get a linking error if they try to
   use them. */

private:
	CNURBS& operator = (CNURBS & Other);
	CNURBS(CNURBS & Other);
};

/////////////////////////////////////////////////////////////////////////
// Implementation of inline methods
/*******************************************************************/
// Degree
inline int CNURBS::Degree() const
	{
	return m_pKnots->Degree();
	}
/*******************************************************************/
// The index of the knot that closes the domain interval
inline int CNURBS::LastKnot() const
	{
	return m_pKnots->Size();
	}
/*******************************************************************/
// Set a knot
inline void CNURBS::SetKnot(
	int nIndex,     //In: The index of the knot to be set
	double rValue)  // In: The value to set it to
	{
	m_pKnots->Set(nIndex, rValue);
	}
/*******************************************************************/
// Set the degree
inline void CNURBS::SetDegree(
	int nDegree) //In: The desired degree
	{
	m_pKnots->SetDegree(nDegree);
	}
/*******************************************************************/
// The knot with given index
inline double CNURBS::Knot(
	int nIndex) const         // Index of desired knot
	{
	return m_pKnots->Knot(nIndex);
	}
/*******************************************************************/
// The number of distinct control points
inline int CNURBS::PointCount() const
	{
	return m_pControlPoints->Size();
	}
/*******************************************************************/
// Return the control point with a given index
inline C3Point & CNURBS::ControlPoint(
	int nIndex) const // Index of the desired control point
	{
	return (*m_pControlPoints)[nIndex];
	}
/*******************************************************************/
// Set the interval from knots
inline void CNURBS::SetInterval()
	{
	if (m_pKnots)
		m_pKnots->SetInterval(m_rStart, m_rEnd, m_rPeriod);
   	}
/*******************************************************************/
// Insert a knot - simple version
inline RC CNURBS::InsertKnot(
	double & rAt,		// In: Parameter value for the inserted knot
                        // Out: An existing knot within rSnap, if there is one
	int nContinuity)    // In: The desired continuity at the knot
	{
	int n,k;  // Dummy variables
	return InsertKnot(rAt, nContinuity, n, k);
	}
/*******************************************************************/
// Test if the curve is closed within tolerance
inline BOOL CNURBS::Closed() const
{
	if (Period() ||
		m_pControlPoints && m_pControlPoints->Size() > 1 &&
		(*m_pControlPoints)[0] == (*m_pControlPoints)[m_pControlPoints->Size() - 1])
		return 1;
	else
		return 0;
}

/*******************************************************************/
// Compute the length of the curve
inline 	double CNURBS::GetLength() const { return GetLength(m_rStart, m_rEnd); }


//////////////////////////////////////////////////////////////////////////
//	Function prototypes
GEO_API RC GetPeriodicSpline(
	PKnots pNodes,			// In: Knos as interpolation nodes
	PPointArray pPoints,	// In: Interpolation points (last one ignored)
	PNURBS & pSpline);		// Out: Interpolating spline

#endif
