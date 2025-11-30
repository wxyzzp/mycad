/*	Line.cpp - Implements the class CLine.
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * See Line.h for the definition of the class.
 * This file implements its methods
 *
 */
#include "Geometry.h"
#include "Point.h"
#include "Trnsform.h"
#include "TheUser.h"
#include "Extents.h"
#include "CompCrvs.h"
#include "Line.h"
#include "plane.h"
#include "vector.h"

DB_SOURCE_NAME;

//////////////////////////////////////////////////////////////////
//           Line segment


//Default constructor an destructor
CLine::CLine()
    {
    }

CLine::~CLine()
    {
    }

PCurve CLine::Clone() const
	{
	return new CLine(*this);
	}
/*******************************************************************************/
// Constructor from 2 points
CLine::CLine(
	const C3Point & A,	// In: 2 endpoints
	const C3Point & B)
	:m_ptStart(A),
	 m_ptEnd(B)
    {
    }
/************************************************************************/
// Construct from a packed line
CLine::CLine(
	LPBYTE & pStorage)    // In/Out: The packed line
	// pStorage will be advanced to the next piece of data
	{
    m_ptStart.Unpack(pStorage);
    m_ptEnd.Unpack(pStorage);
    }
/************************************************************************/
// Construct from a CUnboundedLine
CLine::CLine(
	PCUnboundedLine p,	// In: A line segment
	RC & rc)			// Out: Return code
	{
	rc = p->GetStartPoint(m_ptStart);
	if (!rc)
		rc = p->GetEndPoint(m_ptEnd);
	}
/************************************************************************/
// Curve type
CURVETYPE CLine::Type() const
    {
	return TYPE_Line;
    }
/***********************************************************************/
// Evaluator - point only
RC CLine::Evaluate(
	double rAt,
	C3Point & ptPoint) const
    {
	ptPoint = m_ptStart;
	ptPoint += rAt * (m_ptEnd - ptPoint);
	RETURN_RC(CLine::Evaluate, SUCCESS);
    }
/************************************************************************/
// Evaluator - point and n derivatives
RC CLine::Evaluate
	(
	double rAt,               // In: Where to evaluate
	int nDerivatives,         // In: Number of desired derivatives
	PRealPoint pValues) const // Out: Position and derivatives
	{
	// Position
	pValues[0] = (1.0 - rAt) * m_ptStart + rAt * m_ptEnd;
	// dnb - 5/27/98
	// Do not normalize the tangent because the domain is now
	// 0,1. The difference vector IS the derivative.
	pValues[1] = m_ptEnd - m_ptStart;

    // All other derivatives
	for (int i=2;  i<= nDerivatives;  i++)
		pValues[i] = C3Point(0, 0, 0);

	RETURN_RC(CLine::Evaluate, SUCCESS);
	}
/************************************************************************/
// Evaluator - point, tangent and curvature, talking LPREALs
RC CLine::Evaluate(
	double rAt,                 // In: Where to evaluate
	C3Point & ptPoint,			// Out: Point there
	C3Point & ptTangent,		// Out: Unit tangent vector there
	double & rCurvature,	    // Out: Curvature there
	P3Point pUp) const			// In: Upward normal (optional, NULL OK)
    {
	ptTangent = m_ptEnd - m_ptStart;
	ptPoint = m_ptStart + rAt * ptTangent;
	RC rc = ptTangent.Unitize();
	rCurvature = 0;

 	RETURN_RC(CLine::Evaluate, rc);
    }
/************************************************************************/
// Evaluator - point and tangent vector
RC CLine::Evaluate(
	double rAt,                // In: Parameter value to evaluate at
	C3Point & ptPoint,		   // Out: Point on the line with that parameter
	C3Point & ptTangent) const // Out: Tangent vector at that point
    {
	ptTangent = m_ptEnd - m_ptStart;
	ptPoint = m_ptStart + rAt * ptTangent;
	RETURN_RC(CLine::Evaluate, SUCCESS);
    }
/************************************************************************/
// Update the extents
RC CLine::UpdateExtents(
	BOOL bTight,			// In: Tight extents if =TRUE
	BOOL bIgnoreStart,		// In: Ignore the startpoint, if =TRUE
	CExtents & cExtents,	// In/out: The extents, updated here
	PAffine pTransform)		// Optional: Transformation (NULL OK)
	{
	// Startpoint, if not otherwise indicated
	if (!bIgnoreStart)
		{
		cExtents.Update(m_ptStart, pTransform);
		}

	// In any case, endpoint
	cExtents.Update(m_ptEnd, pTransform);

	RETURN_RC(CLine::UpdateExtents, SUCCESS);
	}
/************************************************************************/
// Length
RC CLine::Length(
	double & rLength) const   // Out: The total length
    {
	C3Point D = m_ptEnd - m_ptStart;
	rLength = D.Norm();
	RETURN_RC(CLine::Length, SUCCESS);
    }
/************************************************************************/
// Add this line's contribution to the integral of |C, C', Up|
RC CLine::UpdateAreaIntegral(
	double & rSoFar,	// In/Out: The integral
	P3Point pUp) const  // In: Upward unit normal (NULL OK)
    {
	C3Point V = m_ptEnd - m_ptStart;
	CNormal N(pUp);
	rSoFar += Determinant(m_ptStart, V, N);

	RETURN_RC(CLine::UpdateAreaIntegral, SUCCESS);
    }
/****************************************************************************/
// Add a point to the rendering cache
LOCAL RC AddPoint(
	PAffine pTransform,	// In: Rendering transformation (NULL OK)
	C3Point P,			// In: A point
	double t,			// In: Parameter value
	CCache & cCache)	// In: The cache we render into
	{
	if (pTransform)
		pTransform->ApplyToPoint(P, P);
	RC rc = cCache.Accept(P, t);
	RETURN_RC(AddPoint, rc);
	}
/****************************************************************************/
//	Linearize the line
RC CLine::Linearize(
	double rResolution,			// In: Chord-height toelrance (ignoreed here)
	PAffine pTransform,			// In: Rendering transformation (NULL OK)
	CCache & cCache) const		// In: The cache we render into
	{
	RC rc;
	ASSERT(!pTransform || pTransform->Affine());

	if (!cCache.Started())
		// Do the start point
		if (rc = AddPoint(pTransform, m_ptStart, 0, cCache))
			QUIT

	// Do the end point
	rc = AddPoint(pTransform, m_ptEnd, 1, cCache);
exit:
	RETURN_RC(CLine::Linearize, rc);
	}
/****************************************************************************/
// Linearize a portion of the line
RC CLine::LinearizePart(
	double rFrom,			// In: Where to start
	double rTo,				// In: Where to stop
	double rResolution,		// In: Chord-height toelrance
	PAffine pTransform,		// In: Rendering transformation (NULL OK)
	CCache & cCache) const	// In: The cache we render into
	{
	C3Point P;
	RC rc = SUCCESS;
	ASSERT(!pTransform || pTransform->Affine());

	if (!cCache.Started())
		{
		Evaluate(rFrom, P);
		if (rc = AddPoint(pTransform, P, rFrom, cCache))
			QUIT
		}

	Evaluate(rTo, P);
	rc = AddPoint(pTransform, P, rTo, cCache);
exit:
	RETURN_RC(CLine::LinearizePart, rc);
	}
/************************************************************************/
// Find the unique intersection point of two lines, if exists
RC IntersectLines(
	const C3Point & A,     // In: Point on the first line
	const C3Point & U,     // In: Direction vector of the first line
	const C3Point & B,     // In: Point on the second line
	const C3Point & V,     // In: Direction vector of the second line
	C3Point & P)           // In: Intersection point, if exists and unique
	{
/* This is a low level utility that returns the intersection point of 2 lines.
   if they are either parallel or coincident it returns FAILURE.

   It works by solving the 2 equations with 2 unknowns s and t:

   	 A + sU = B + tV, or

   	 sU + tV = B - A.  The solution for s is:

	     det(B-A, U)
   	 s = -----------, and then P = A + sU.
   	      det(U, V)
 */
 	RC rc = E_NOMSG;
	double s = Determinant(U, V);

	if (!FZERO(s, FUZZ_GEN))
		{
		rc = SUCCESS;
		s = Determinant(B - A, V) / s;
		P = A + s * U;
		}

	RETURN_RC(IntersectLines, rc);
	}
/************************************************************************/
// Transform in place
RC CLine::Transform(
	CTransform & cTransform)    // In: The transformation
	{
	RC rc = SUCCESS;

	if (!cTransform.Affine())
		{
		rc = FAILURE;
		QUIT
		}

	// Apply the transformation to the defining points
	cTransform.ApplyToPoint(m_ptStart, m_ptStart);
	cTransform.ApplyToPoint(m_ptEnd, m_ptEnd);
exit:
	RETURN_RC(CLine::Transform, rc);
	}
/************************************************************************/
// Reverse
RC CLine::Reverse()
    {
	// Swap the start and end
	SWAP(m_ptStart, m_ptEnd, C3Point)
	RETURN_RC(CCurve::Reverse, SUCCESS);
    }
/************************************************************************/
// Seeds for equations solving
RC CLine::Seeds(
	CRealArray & arrSeeds,  // In/Out: Array of seeds
	double* pStart,			// In: Domain start (optional, NULL OK)
	double* pEnd) const		// In: Domain end (optional, NULL OK)
    {
	RC rc = arrSeeds.Add(0);
	RETURN_RC(CLine::Seeds, rc);
    }
/*******************************************************************************/
// Find all intersections an overlaps
RC CLine::SelfIntersect(
	CRealArray & cParameters, // In/Out: List of self intersection parameters
	double rTolerance)		  // Optional: Tolerance
	{
	// A Line can never intersect itself
	RETURN_RC(CLIne::SelfIntersect, SUCCESS);
	}
/*******************************************************************************/
// Pack the Line
RC CLine::Pack(
	LPBYTE & pStorage)  const // In/Out: The storage
	{
	// pStorage will be advanced to the next available storage

	// Pack the type
	CURVETYPE * pType = (CURVETYPE *)pStorage;
	*pType = TYPE_Line;
	pStorage += sizeof(CURVETYPE);

	// Pack the points
	m_ptStart.Pack(pStorage);
	m_ptEnd.Pack(pStorage);

	RETURN_RC(CLine::Pack, SUCCESS);
	}
/*******************************************************************************/
// Packing size
int CLine::PackingSize() const
	{
	return sizeof(CURVETYPE) + 2 * m_ptStart.PackingSize();
	}
/************************************************************************/
// Snap the end point to a given point
RC CLine::SnapEnd(
	SIDE nWhichEnd,  	  // In: Start or end
	const C3Point & ptTo) // In: The point to snap to
	{
	RC rc = SUCCESS;

	if (nWhichEnd == LEFT)
		m_ptStart = ptTo;
	else
		m_ptEnd = ptTo;

	RETURN_RC(CLine::SnapEnd, SUCCESS);
	}

/*******************************************************************************/
// A translation to a given point
RC CLine::Move(
	const C3Point & ptBy)	// In: The desired origin's new position
	{
	m_ptStart += ptBy;
	m_ptEnd += ptBy;
	RETURN_RC(CLine::Move, SUCCESS);
	}
/*******************************************************************************/
// Compute an offset
RC CLine::Offset(
	double rDistance,	// In: Offset signed distance (left if positive)
	PCurve & pOffset, 	// Out: The offset
	P3Point pUp) const	// In: Upward unit normal (Optional)
    {
	C3Point V = Velocity();
	double r = V.Norm();

	RC rc = ALLOC_FAIL(pOffset = Clone());
	if (rc)
		QUIT

	// Get the direction vector
	if (FZERO(r, FUZZ_DIST))
		{
		rc = E_ZeroVector;
		QUIT
		}
	r = rDistance / r;
	V *= r;
	V.TurnLeft(pUp);

	// Offset the line
	pOffset->Move(V);
exit:
	if (rc)
		{
		delete pOffset;
		pOffset = NULL;
		}
	RETURN_RC(CLine::Offset, rc);
	}
/*******************************************************************************/
// Return an approximation with one of the Visio curves
RC CLine::GetApproximation(
	double rTolerance,		// In: Approxiation tolerance
	PCurve & pApprox) 		// Out: The approximation
	{
	RC rc = ALLOC_FAIL(pApprox = Clone());
	RETURN_RC(CLine::GetApproximation, rc);
	}
/*******************************************************************************/
// Persist this line
RC CLine::Persist(
	PStorage pStorage) const	// In/Out: The storage
	{
	RC rc = pStorage->AcceptLine(this);
	RETURN_RC(CLine::Persist, rc);
	}
/*******************************************************************************/
// Construct a copy reparameterized to arc length
RC CLine::ArcLengthCopy(
	double rFrom,	// In: Domain start
	double rTo,		// In: Domain end
	PCurve & pCopy)	// Out: A constant speed copy
	{
	PRealFunction pMapping = NULL;
	pCopy = NULL;
	double r = Velocity().Norm() * (rTo - rFrom);	// segment's length

	RC rc = ALLOC_FAIL(
		pMapping = new CLinearFunction(0, r, rFrom, rTo));
	if (!rc)
		rc = ALLOC_FAIL(pCopy = new CMappedCurve(this, pMapping));

	RETURN_RC(CLine::ArcLengthCopy, rc);
	}
/*******************************************************************/
// Is the line horizontal or a part of the border of a horizontal region
BOOL CLine::HorizontalBorder(
	BOOL bVisible,	// In: TRUE if this path is visble
	double xMax)	// In: X extent
	{
/*
 *	A line is considered a horizontal border if it either horizontal, or
 *  if it is an invisible border of a filled region which is a rectangle
 *  that extends from 0 to MaxX, in which case the line may be the vertical
 *  boundary at one of the extremes.
 *  This is a 2D method, used for rendering custom line patterns.
 */
	C3Point V = m_ptEnd - m_ptStart;

	if(FZERO(V.Y(), FUZZ_GEN))
		// It's a horizontal line, which is fine
		return TRUE;

	if (bVisible)
		// It is visible and non-horizontal, it won't do
		return FALSE;

	if (!FZERO(V.X(), FUZZ_GEN))
		// It is not vertical
		return FALSE;

	// It's vertical, now is it at one of the extremes?
	if (!FZERO(m_ptStart.X(),FUZZ_GEN)	&&
		!FEQUAL(m_ptStart.X(), xMax, FUZZ_GEN))
		return FALSE;

	// Passed all the tests
	return TRUE;
	}

/*****************************************************************************/
// Check if this offset distance will not cusp & do ugly things
RC CLine::AdmitOffset( // Always returns SUCCESS
	double rDistance,		// In: The offset distance
	double rTolerance,		// In: The acceptable error
	P3Point pNormal)		// In: The Up direction (NULL OK)
	{
	return SUCCESS;
	}
/*****************************************************************************/
// Direction unit vector
C3Point CLine::Direction() const
	{
	C3Point V = m_ptEnd - m_ptStart;
	if (V.Unitize())
		return C3Point(1,0,0);
	else
		return V;
	}
/**********************************************************************/
// Nearest point on the line to a given point
RC CLine::PullBack(
	C3Point ptTo,		   // In: The point we want to get closest to
	C3Point & ptNearest,   // Out: The nearest point on the curve
	double & rAt,          // Out: Parameter value of Nearest
	double & rDist,		   // Out: The distance from the point to the curve
	double rMargin,		   // In: Ignored here
	double ignore) const   // Optional: Parameter value to ignore
    {
	RC rc = E_NOMSG;
	C3Point AB = Velocity();
	C3Point AP = ptTo - Origin();
	double d = AB * AB;
	if (FZERO(d, FUZZ_GEN))
		{
		// The line is actually a point, so
		rAt = 0;
		ptNearest = Origin();
		rc = SUCCESS;
		goto exit;
		}
/*
 Say the line is C(t) = (1-t)*A + t*B and the point is P.  The line from P
 to the nearest point is perpendicular to AB, so (C-P)*(B-A)=0;
 Expanding C(t) and elimnating t, we get t=(B-A)*(B-A)/((P-A)*(B-A)).
 */
	rAt = AB * AP / d;
	if (PullIntoDomain(rAt))
		{
		rc = SUCCESS;
		ptNearest = Origin() + rAt * AB;
		}
exit:
	if (!rc)
		if (FEQUAL(rAt, ignore, FUZZ_DIST))
			rc = E_NOMSG;
		else
			{
			AP = ptTo - ptNearest;
			rDist = AP.Norm();
			}

	RETURN_RC(CLine::PullBack, rc);
    }

/*******************************************************************************/
// Update the number of intercpts with a given ray
RC CLine::UpdateIntercepts(
	PRay pRay,			// In: A ray
	int	& nIntercepts)  // In/out: The number of intercepts
	{
 	RC rc = E_NOMSG;
	double r, s, t;
	C3Point V = Velocity();
	C3Point D = pRay->Origin() - Origin();

	r = Determinant(V, pRay->Velocity());
	if (FZERO(r, FUZZ_GEN))
		QUIT	// The ray has same direction as this line

	rc = SUCCESS;
	s = Determinant(D, V) / r;
	t = Determinant(D, pRay->Velocity()) / r;
	if (!pRay->PullIntoDomain(s)  ||   !PullIntoDomain(t))
		// The intersection is outside one of the domains
		goto exit;

	// Endpoints intercpts invalidate the entire test
	if (FEQUAL(t, Start(), FUZZ_GEN)  ||
		FEQUAL(t, End(), FUZZ_GEN))
		rc = E_NOMSG;
	else
		// This is a valid intercept, increment the count
		nIntercepts++;
exit:
	RETURN_RC(CLine::UpdateIntercepts, rc);
	}
/*******************************************************************************/
// Create polyline edge from this curve
RC CLine::GetPolylineEdges(
	P3Point pVertices,	// Out: The vertex point point
	double* pBulges,	// Out: The bulge
	int	& nEdges)		// Out: The number of resulting edges
	{
	// If the line is too short then 0 will be returned for nEdges
	pVertices[0] = m_ptStart;
	pBulges[0] = 0;

	if (FZERO(PointDistance(m_ptStart, m_ptEnd), FUZZ_DIST))
		// Degenerate line, create no edge
		nEdges = 0;
	else
		nEdges = 1;

	RETURN_RC(CLine::GetPolylineEdges, SUCCESS);
	}
/*******************************************************************************/
// Return a modeling ready slice of the curve
RC CLine::NextSlice(	// Return E_NOMSG when it's the last slice
	int & i,			// In/out: An index marker
	PCurve & pSlice)	// Out: The current slice
	{
	// For modeling a CUnboundedLine is preferable
	RC rc = ALLOC_FAIL(pSlice = new CUnboundedLine(this));
	i++;
	delete this;
	if (!rc)
		rc = E_NOMSG;
	return rc;
	}
//////////////////////////////////////////////////////////////////////////
//		Domain Manipulation
/*
 Our line's domain is always [0,1], so setting the domain or one of its
 ends is not defined.  The problem with that is that the user may try to
 set the domain, say for linearizing a part of the curve, and then reset
 it to the original domain.  The results will be incorrect on CLine.
 That's whey SetDomain is NOT implemented, but we need ResetStart and
 ResetEnd for corner rounding when rendering.  For those we cheat, moving
 the line's start or end  POINT, leaving the domain at [0,1].
 */
/************************************************************************/
// Reset the domain interval's start
RC CLine::ResetStart(
	double rNew)  // In: New curve start
	{
	C3Point P;

	Evaluate(rNew, P);
	m_ptStart = P;

	RETURN_RC(CLine::ResetStart, SUCCESS);
	}
/************************************************************************/
// Reset the domain interval's end
RC CLine::ResetEnd(
	double rNew)  // In: New curve end
	{
	C3Point P;

	Evaluate(rNew, P);
	m_ptEnd = P;

	RETURN_RC(CLine::ResetEnd, SUCCESS);
	}

/*****************************************************************************/
// Replace this line with its restriction a new domain
RC CLine::ReplaceWithRestriction(
	double rFrom,   // In: New curve start
	double rTo,     // In: New curve end
	PCurve & pOut)	// Out: The restricion to the new domain
	{
	RC rc = SUCCESS;
	C3Point A, B;

	if (rFrom > rTo)
		{
		rc = E_BadInput;
		QUIT;
		}

	Evaluate(rFrom, A);
	Evaluate(rTo, B);
	m_ptStart = A;
	m_ptEnd = B;
exit:
	if (rc)
		pOut = NULL;
	else
		pOut = this;
	RETURN_RC(CLine::ReplaceWithRestriction, rc);
    }
/*****************************************************************************/
// Coalesce this line with another, if possible
BOOL CLine::CoalescedWith(
	PCurve pCurve)		// In/Out: the curve we try to coalesce with
	{
	BOOL bCoalesced = FALSE;

	// A line can only be coalesced with another line.
	if (pCurve->Type() == TYPE_Line ||
		pCurve->Type() == TYPE_Ray ||
		pCurve->Type() == TYPE_UnboundedLine)
		{
		C3Point A, B, P, Q;
		if (GetStartPoint(A))
			goto exit;
		if (GetEndPoint(B))
			goto exit;

// For efficiency, assume without checking that the the endpoint of
// one line coicides with the startpoint of the other.
		if (pCurve->GetStartPoint(P))
			goto exit;
		ASSERT(FZERO(PointDistance(B, P), FUZZ_DIST));
		if (pCurve->GetEndPoint(Q))
			goto exit;
		// Are the lines collinear?
		if (!Collinear(A, B, Q, FUZZ_DIST))
			goto exit;

		// Do they go in the same direction
		if ((B - A) * (Q - P) < 0)
			goto exit;

		bCoalesced = TRUE;
		SetEndPoint(Q);
		}	// End if
exit:
	return bCoalesced;
	}

/****************************************************************************
 *
 *	Find the perpendicular line through a point. The function returns SUCCESS
 *  if the perpendicualar point is found and E_NOMSG otherwise. The optional
 *  reference point is not taken account for linear entity.
 *
 *	@Author: Stephen W. Hou
 *	@Date:   Sep 21, 2003
 */
RC CLine::PerpendicularThru(
	const C3Point & ptThru,		// In: A point not on curve
    const C3Point & ptFerence,  // In: The optional eference point to select perpendicular point
	C3Point & ptPerpPoint,		// Out: Perpendicular point on the curve
	double & rAt				// Out: Parameter value of the perpendicular point
	) const
{
    rAt = 0.0;
    C3Point origin = Origin();
    C3Point dir = Direction();
	if (!Collinear(origin, ptThru, origin + dir, FUZZ_DIST)) {
		// the parameter value of the nearest point on the line
		//
		rAt = - (dir * (origin - ptThru)) / (dir * dir);
		ptPerpPoint = origin + rAt * dir;
        return PullIntoDomain(rAt) ? SUCCESS : E_NOMSG;
	}
    else {
        double rDist;
        return PullBack(ptThru, ptPerpPoint, rAt, rDist);
    }

	return E_NOMSG;
}


/*******************************************************************
 *
 * Compute the length of a portion of a line.
 *
 * @Author: Stephen W. Hou
 * @Date:   April 8, 2004
 */
RC CLine::GetLength(double rFrom,		// In: Lower integration bound
				    double rTo,			// In: Upper integration bound
				    double &rLength) const // Out: The length
{
	ASSERT(rFrom =< rTo);

	C3Point ptStart, ptEnd;
	GetStartPoint(ptStart);
	GetEndPoint(ptEnd);

	double length;
	Length(length);

	if (!FZERO(length, FUZZ_GEN)) {
		C3Point disp = ptEnd - ptStart;
		C3Point ptFrom = ptStart + disp * rFrom / length;
		C3Point ptTo = ptStart + disp * rTo / length;
		disp = ptTo - ptFrom;
		rLength = disp.Norm();
		return SUCCESS;
	}
	else
		return FAILURE;
}


/*******************************************************************
 *
 * Find a point on the curve by an arc length from the start of the
 * curve.
 *
 * @Author: Stephen W. Hou
 * @Date:   April 8, 2004
 */
RC CLine::FindPointByLength(double rLength,
		                    double &rParam,
						    C3Point &cPoint) const
{
	double curveLength;
	Length(curveLength);

    if (rLength <= 0.0 || rLength >= curveLength)
        return FAILURE;

	C3Point ptStart, ptEnd;
	GetStartPoint(ptStart);
	GetEndPoint(ptEnd);

	rParam = rLength / curveLength;
	cPoint = ptStart + (ptEnd - ptStart) * rParam;
	return SUCCESS;
}


/*******************************************************************
 *
 * Get the points on the curve by subdviding the length of the curve
 * uniformly in the model space.
 *
 * @Author: Stephen W. Hou
 * @Date:   April 8, 2004
 */
RC CLine::GetPointsWithEqualArcLength(int iNumSegments,				// In:  The number of segments
		                              CPointArray &ptArray,			// Out: The point array
									  CRealArray &parArray) const	// Out: The optional parameter array
{
	return GetPointsWithEqualInterval(iNumSegments, ptArray, parArray);
}

/************************************************************************/
// Project a curve to a plane along a specified direction. If the direction
// is not specified, the curve will be projected perpendicularly to the
// plane(along the normal direction of plane). Function returns NULL when
// it fails and a point to the curve otherwise.
//
// Auther:	Stephen W. Hou
// Date:	6/14/2004
//
PCurve CLine::ProjectTo(const CPlane & plane, const C3Vector & dir, bool get2dCurve) const
{
	PCurve pCurve = NULL;
    C3Vector along;

    if (&dir == NULL) {
        CPlane::PLANETYPE type = plane.PlaneType();
        if (type== CPlane::TYPE_Xy) {
            if (get2dCurve) {
                pCurve = new CLine(C3Point(m_ptStart.X(), m_ptStart.Y(), 0.0),
                                 C3Point(m_ptEnd.X(), m_ptEnd.Y(), 0.0));
                pCurve->SetAs2DCurve(get2dCurve);
                return pCurve;
            }
            else
                return new CLine(C3Point(m_ptStart.X(), m_ptStart.Y(), 0.0),
                                 C3Point(m_ptEnd.X(), m_ptEnd.Y(), 0.0));
        }
        else if (type == CPlane::TYPE_Yz) {
            if (get2dCurve) {
                pCurve = new CLine(C3Point(m_ptStart.Y(), m_ptStart.Z(), 0.0),
                                 C3Point(m_ptEnd.Y(), m_ptEnd.Z(), 0.0));
                pCurve->SetAs2DCurve(get2dCurve);
                return pCurve;
            }
            else
                return new CLine(C3Point(0.0, m_ptStart.Y(), m_ptStart.Z()),
                                 C3Point(0.0, m_ptEnd.Y(), m_ptEnd.Z()));
        }
        else if (type == CPlane::TYPE_Zx) {
            if (get2dCurve) {
                pCurve = new CLine(C3Point(m_ptStart.Z(), m_ptStart.X(), 0.0),
                                 C3Point(m_ptEnd.Z(), m_ptEnd.X(), 0.0));
                pCurve->SetAs2DCurve(get2dCurve);
                return pCurve;
            }
            else
                return new CLine(C3Point(m_ptStart.X(), 0.0, m_ptStart.Z()),
                                 C3Point(m_ptEnd.X(), 0.0, m_ptEnd.Z()));
        }
        else
            along = plane.GetNormal().Negate();
    }
    else
        along = dir;

	C3Point start, end;
	if (plane.ProjectPoint(m_ptStart, along, start, get2dCurve) == SUCCESS &&
        plane.ProjectPoint(m_ptEnd, along, end, get2dCurve) == SUCCESS) {
		pCurve = new CLine(start, end);
        pCurve->SetAs2DCurve(get2dCurve);
    }
	return pCurve;
}

/**************************************************************************/
//
// Performs ray hitting test.
//
// Author:  Stephen W. Hou
// Date:    7/22/2004
//
BOOL CLine::HitTest(            // Return TRUE if the ray hits the cuvre
    const C3Point &ptThrough,   // In: Through point
    const C3Vector &hitDir,     // In: Hit direction
    double rTolerance,          // In: The tolerance considered a hit
    double & rAt)               // Out: Hit location on the curve within tolerance
{
    BOOL bHit = FALSE;

    CLine* pLine = (CLine*)Clone();
    // If the hit direction is different than Z direction, we transform
    // point and line both to the coordinate system defined by the project
    // dirction.
    //
    C3Point point;
    if (!FZERO(hitDir.X(), FUZZ_RAD) || !FZERO(hitDir.Y(), FUZZ_RAD)) {
        CAffine iTransf, transf(hitDir);
        iTransf.SetInverse(transf);
        iTransf.ApplyToPoint(ptThrough, point);
        pLine->Transform(iTransf);
    }
    else
        point = ptThrough;

    double rDist;
    C3Point ptOnLine;
    if (pLine->PullBack(point, ptOnLine, rAt, rDist) == SUCCESS) {
	    double rSqFuzz = rTolerance * rTolerance;
        double xDiff = ptOnLine.X() - point.X();
        double yDiff = ptOnLine.Y() - point.Y();
        bHit = (xDiff * xDiff + yDiff * yDiff <= rSqFuzz);
    }
    if (pLine)
        delete pLine;

    return bHit;
}

/**************************************************************************/
// Reutrn TRUE if the line is degenerated to a point and FALSE otherwise.
//
// Auther:	Stephen W. Hou
// Date:	6/14/2004
//
BOOL CLine::IsDegenerated() const
{
	return (FEQUAL(m_ptStart.X(), m_ptEnd.X(), FUZZ_DIST) &&
            FEQUAL(m_ptStart.Y(), m_ptEnd.Y(), FUZZ_DIST) &&
			FEQUAL(m_ptStart.Z(), m_ptEnd.Z(), FUZZ_DIST));
}

#ifdef DEBUG
/*******************************************************************************/
// Dump
void CLine::Dump() const
	{
	DB_MSGX("Line thru points:",());
	m_ptStart.Dump();
	m_ptEnd.Dump();
	}
#endif
/*******************************************************************/
// How complex is this spline?
int CLine::Complexity() const
	{
	return 1;
	}
//////////////////////////////////////////////////////////////////
//																//
//					Unbounded  Ray								//
//																//
//////////////////////////////////////////////////////////////////
// Constructor from a point and direction vector
CRay::CRay(
	const C3Point & ptPoint,      // In: a point
	const C3Point & vecDirection) // In: Direction vector
    {
/* The line that will be constructed is Point + t*Direction,
 * for t between 0 and INFINITY.
 */
	m_ptStart = ptPoint;
	m_ptEnd = vecDirection;
    }
/**********************************************************************/
// Clone
PCurve CRay::Clone() const
	{
	return new CRay(*this);
	}
/***********************************************************************/
// Evaluator - point only
RC CRay::Evaluate(
	double rAt,
	C3Point & ptPoint) const
    {
	RC rc = SUCCESS;
	if (fabs(rAt) >= INFINITY)
		rc = E_BadInput;
	else
		ptPoint = m_ptStart + rAt * m_ptEnd;
	RETURN_RC(CRay::Evaluate, rc);
    }
/************************************************************************/
// Evaluator - point and n derivatives
RC CRay::Evaluate
	(
	double rAt,               // In: Where to evaluate
	INT nDerivatives,         // In: NUmber of desired derivatives
	PRealPoint pValues) const // Out: Position and derivatives
	{
	RC rc = SUCCESS;
	int i;
	if (fabs(rAt) >= INFINITY)
		{
		rc = E_BadInput;
		QUIT
		}

	// Position
	pValues[0] = m_ptStart + rAt * m_ptEnd;

    // First derivative
	if (nDerivatives > 0)
		pValues[1] = m_ptEnd;

    // All other derivatives
	for (i=2;  i<= nDerivatives;  i++)
		pValues[i] = C3Point(0.0, 0.0);
exit:
	RETURN_RC(CRay::Evaluate, rc);
	}
/************************************************************************/
// Evaluator - point, tangent and curvature
RC CRay::Evaluate(
	double rAt,                   // In: Where to evaluate
	C3Point & ptPoint,			  // Out: Point there
	C3Point & vecTangent,		  // Out: Unit tangent vector there
	double & rCurvature) const    // Out: Curvature there
    {
	RC rc = SUCCESS;
	if (fabs(rAt) >= INFINITY)
		{
		rc = E_BadInput;
		QUIT
		}

	ptPoint = m_ptStart + rAt * m_ptEnd;
	vecTangent = m_ptEnd;
	rCurvature = 0;
exit:
 	RETURN_RC(CRay::Evaluate, rc);
    }
/************************************************************************/
// Evaluator - point and tangent vector
RC CRay::Evaluate(
	double rAt,                 // In: Parameter value to evaluate at
	C3Point & ptPoint,		    // Out: Point on the line with that parameter
	C3Point & vecTangent) const // Out: Tangent vector at that point
    {
	RC rc = SUCCESS;
	if (fabs(rAt) >= INFINITY)
		{
		rc = E_BadInput;
		QUIT
		}

	ptPoint = m_ptStart + rAt * m_ptEnd;
	vecTangent = m_ptEnd;
exit:
	RETURN_RC(CRay::Evaluate, rc);
    }
/************************************************************************/
// Transform in place
RC CRay::Transform(
	CTransform & cTransform)    // In: The transformation
	{
	RC rc = SUCCESS;

	if (!cTransform.Affine())
		{
		rc = FAILURE;
		QUIT
		}

	// Apply the transformation to the defining points
	cTransform.ApplyToPoint(m_ptStart, m_ptStart);
	cTransform.ApplyToVector(m_ptEnd, m_ptEnd);
exit:
	RETURN_RC(CRay::Transform, rc);
	}
/*******************************************************************************/
// A translation to a given point
RC CRay::Move(
	const C3Point & ptBy)	// In: The desired origin's new position
	{
	m_ptStart += ptBy;
	RETURN_RC(CRay::Move, SUCCESS);
	}

/****************************************************************************/
//	Linearize the ray
RC CRay::Linearize(
	double rResolution,			// In: Chord-height toelrance (ignoreed here)
	PAffine pTransform,			// In: Rendering transformation (NULL OK)
	CCache & cCache) const		// In: The cache we render into
	{
	C3Point P, V;
	RC rc;
	ASSERT(!pTransform || pTransform->Affine());

	if (!cCache.Started())
		// Do the start point
		if (rc = AddPoint(pTransform, m_ptStart, 0, cCache))
			QUIT

	// Do the end point
	if (pTransform)
		{
		pTransform->ApplyToPoint(m_ptStart, P);
		pTransform->ApplyToVector(m_ptEnd, V);
		}
	else
		{
		P = m_ptStart;
		V = m_ptEnd;
		}
	rc = cCache.Accept(P, V);
exit:
	RETURN_RC(CRay::Linearize, rc);
	}
/*****************************************************************************/
// Linearize a portion of the ray - not implemented
RC CRay::LinearizePart(
	double rFrom,			// In: Where to start
	double rTo,				// In: Where to stop
	double rResolution,		// In: Chord-height toelrance
	PAffine pTransform,		// In: Rendering transformation (NULL OK)
	CCache & cCache) const	// In: The cache we render into
	{
	RC rc;

	if (rTo < INFINITY)
		rc = CLine::LinearizePart(rFrom, rTo, rResolution,
								  pTransform, cCache);
	else
		rc = Linearize(rResolution, pTransform, cCache);

	RETURN_RC(CRay::LinearizePart, rc);
	}
/*****************************************************************************/
// Replace this curve with its restriction a new domain
RC CRay::ReplaceWithRestriction(
	double rFrom,   // In: New curve start
	double rTo,     // In: New curve end
	PCurve & pOut)	// Out: The restricion to the new domain
	{
	RC rc = SUCCESS;
	pOut = NULL;

	if (rTo >= INFINITY)
		{
		pOut = this;
		rc = ResetStart(rFrom);
		}
	else
		{
		if (rc = ALLOC_FAIL(pOut = new CLine(*this)))
			QUIT
		rc = pOut->SetDomain(rFrom, rTo);
		}
exit:
	if (rc)
		{
		if (pOut != this)
			delete pOut;
		pOut = NULL;
		}
	else if (pOut != this)
		delete this;

	RETURN_RC(CRay::ReplaceWithRestriction, rc);
    }
/************************************************************************/
// Update the extents
RC CRay::UpdateExtents(
	BOOL bTight,			// In: Tight extents if =TRUE
	BOOL bIgnoreStart,		// In: Ignore the startpoint, if =TRUE
	CExtents & cExtents,	// In/out: The extents, updated here
	PAffine pTransform)		// Optional: Transformation (NULL OK)
	{
	C3Point P, U;
	U = m_ptEnd * INFINITY;

	// Update
	if (!bIgnoreStart)
		cExtents.Update(P, pTransform);

	P = m_ptStart + U;
	cExtents.Update(P, pTransform);

	RETURN_RC(CRay::UpdateExtents, SUCCESS);
	}
/************************************************************************/
// Update the wireframe extents
RC CRay::Wireframe(
	BOOL bIgnoreStart,		// In: Ignore the startpoint, if =TRUE
	CExtents & cExtents,	// In/out: The extents, updated here
	PAffine pTransform)		// Optional: Transformation (NULL OK)
	{
	// For a ray and an infinite line, the wireframe is computed from the
	// point and the tip of the direction vector
	if (!bIgnoreStart)
		cExtents.Update(m_ptStart, pTransform);

	C3Point P = m_ptStart + m_ptEnd;
	cExtents.Update(P, pTransform);

	RETURN_RC(CRay::Wireframe, SUCCESS);
	}
/*******************************************************************************/
// Initiate a stored path with this curve
RC CRay::InitiatePath(
	PStorage pStorage) const	// In/Out: The Storage
	{
	// A ray cannot be persisted in Visio
	RETURN_RC(CRay::InitiatePath, FAILURE);
	}
/*******************************************************************************/
// Persist this ray
RC CRay::Persist(
	PStorage pStorage) const	// In/Out: The storage
	{
	// Not implemented
	RETURN_RC(CRay::Persist, FAILURE);
	}
/*****************************************************************************/
// Set the endpoint
void CRay::SetEndPoint(
	const C3Point & ptSetTo)	// In: The point to set to
	{
	m_ptEnd = ptSetTo - m_ptStart;
	}
/*****************************************************************************/
// Coalesce a ay with another is impossible
BOOL CRay::CoalescedWith(
	PCurve pCurve)		// In/Out: the curve we try to coalesce with
	{
	return FALSE;
	}

/************************************************************************/
// Project a curve to a plane along a specified direction. If the direction
// is not specified, the curve will be projected perpendicularly to the
// plane(along the normal direction of plane). Function returns NULL when
// it fails and a point to the curve otherwise.
//
// Auther:	Stephen W. Hou
// Date:	6/14/2004
//
PCurve CRay::ProjectTo(const CPlane & plane, const C3Vector & dir, bool get2dCurve) const
{
	CRay* pCurve = NULL;
	C3Vector along = (&dir) ? dir : plane.GetNormal();

	C3Point origin;
    C3Vector lineDir;
	if (plane.ProjectPoint(m_ptStart, along, origin, get2dCurve) == SUCCESS &&
        plane.ProjectVector(C3Vector(m_ptEnd), along, lineDir, get2dCurve) == SUCCESS) {
		pCurve = new CRay(origin, lineDir.AsPoint());
        pCurve->SetAs2DCurve(get2dCurve);
    }

	return pCurve;
}

//////////////////////////////////////////////////////////////////
//																//
//						Unbounded Line							//
//																//
//////////////////////////////////////////////////////////////////
// Construct from a CLine pointer
CUnboundedLine::CUnboundedLine(
	PCLine pLine)
	:m_rStart(0),
	m_rEnd(1)
	{
	m_ptStart = pLine->Origin();
	m_ptEnd = pLine->Velocity();
	}
/*****************************************************************************/
// Copy constructor
CUnboundedLine::CUnboundedLine(
	const CUnboundedLine & other)
	:m_rStart(other.m_rStart),
	m_rEnd(other.m_rEnd)
	{
	m_ptStart = other.m_ptStart;
	m_ptEnd = other.m_ptEnd;
	}
/*****************************************************************************/
// Clone
PCurve CUnboundedLine::Clone() const
	{
	return new CUnboundedLine(*this);
	}
/*****************************************************************************/
// Get the start point, if defined
RC CUnboundedLine::GetStartPoint(
	C3Point & P) const   // Out: Curve's start point
	{
	RC rc = E_NOMSG;

	if (m_rStart > -INFINITY)
		{
		P = m_ptStart + m_rStart * m_ptEnd;
		rc = SUCCESS;
		}

	RETURN_RC(CUnboundedLine::GetStartPoint, rc);
	}
/*****************************************************************************/
// Get the end point, if defined
RC CUnboundedLine::GetEndPoint(
	C3Point & P) const   // Out: Curve's end point
	{
	RC rc = E_NOMSG;

	if (m_rEnd < INFINITY)
		{
		P = m_ptStart + m_rEnd * m_ptEnd;
		rc = SUCCESS;
		}

	RETURN_RC(CUnboundedLine::GetEndPoint, rc);
	}
/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Get start & end line's points if defined
 * Returns:	SUCCESS if points are both defined; else E_NOMSG.
 ********************************************************************************/
RC
CUnboundedLine::GetEnds
(
 C3Point&	StartPt,	// Out
 C3Point&	EndPt		// Out
)const
{
	RC	rc = E_NOMSG;

	if(m_rStart > -INFINITY)
	{
		StartPt = m_ptStart + m_rStart * m_ptEnd;
		rc = SUCCESS;
	}

	if(m_rEnd < INFINITY)
	{
		EndPt = m_ptStart + m_rEnd * m_ptEnd;
		rc = SUCCESS;
	}
	else
		rc = E_NOMSG;

	RETURN_RC(CUnboundedLine::GetStartPoint, rc);
}

/*****************************************************************************/
// Reverse
RC CUnboundedLine::Reverse()
	{
	// Reverse the vector direction
	m_ptEnd = -m_ptEnd;

	// Swap the ends and change their signs
	double r = -m_rEnd;
	m_rEnd = -m_rStart;
	m_rStart = r;

	return SUCCESS;
	}
/*****************************************************************************/
// Linearize a portion of the ray - not implemented
RC CUnboundedLine::LinearizePart(
	double rFrom,			// In: Where to start
	double rTo,				// In: Where to stop
	double rResolution,		// In: Chord-height toelrance
	PAffine pTransform,		// In: Rendering transformation (NULL OK)
	CCache & cCache) const	// In: The cache we render into
	{
	RC rc;

	if (rFrom > -INFINITY  &&  rTo < INFINITY)
		rc = CLine::LinearizePart(rFrom, rTo, rResolution,
								  pTransform, cCache);
	else
		rc = Linearize(rResolution, pTransform, cCache);

	RETURN_RC(CUnboundedLine::LinearizePart, rc);
	}
/****************************************************************************/
//	Linearize the line
RC CUnboundedLine::Linearize(
	double rResolution,			// In: Chord-height toelrance (ignoreed here)
	PAffine pTransform,			// In: Rendering transformation (NULL OK)
	CCache & cCache) const		// In: The cache we render into
	{
	C3Point P, U;
	RC rc;

	ASSERT(!pTransform || pTransform->Affine());

	if (Bounded())
		{
		if (!cCache.Started())
			{
			// Do the start point
			Evaluate(m_rStart, P);
			if (rc = AddPoint(pTransform, P, 0, cCache))
				QUIT
			}
		Evaluate(m_rEnd, P);
		if (rc = AddPoint(pTransform, P, 0, cCache))
			QUIT
		}
	else
		{
		// This unbounded line should be alone in its path
		ASSERT(!cCache.Started());

		if (pTransform)
			{
			pTransform->ApplyToPoint(m_ptStart, P);
			pTransform->ApplyToVector(m_ptEnd, U);
			}
		else
			{
			P = m_ptStart;
			U = m_ptEnd;
			}

		// Do the 2 halves
		if (rc = cCache.Accept(P, U))
			QUIT
		U = -U;
		rc = cCache.Accept(P, U);
		}
exit:
	RETURN_RC(CUnboundedLine::Linearize, rc);
	}
/************************************************************************/
// Update the extents
RC CUnboundedLine::UpdateExtents(
	BOOL bTight,			// In: Tight extents if =TRUE
	BOOL bIgnoreStart,		// In: Ignore the startpoint, if =TRUE
	CExtents & cExtents,	// In/out: The extents, updated here
	PAffine pTransform)		// Optional: Transformation (NULL OK)
	{
	C3Point P, U;

	// Update
	P = m_ptStart + m_ptEnd * m_rEnd;
	cExtents.Update(P, pTransform);

	P = m_ptStart + m_ptEnd * m_rStart;
	cExtents.Update(P, pTransform);

	RETURN_RC(CUnboundedLine::UpdateExtents, SUCCESS);
	}
/*******************************************************************************/
// Initiate a stored path with this curve
RC CUnboundedLine::InitiatePath(
	PStorage pStorage) const	// In/Out: The Storage
	{
	RC rc = SUCCESS;
	if (Bounded())
		rc = pStorage->InitiateCurve(this);
	RETURN_RC(CUnboundedLine::InitiatePath, rc);
	}
/*******************************************************************************/
// Persist this line
RC CUnboundedLine::Persist(
	PStorage pStorage) const	// In/Out: The storage
	{
	RC rc;
	CLine L(this, rc);
	if (rc)
		rc = pStorage->AcceptUnboundedLine(this);
	else
		rc = pStorage->AcceptLine(&L);
	RETURN_RC(CUnboundedLine::Persist, rc);
	}
/*****************************************************************************/
// Length
RC CUnboundedLine::Length(
	double & rLength) const   // Out: The total length
	{
	RC rc;
	CLine L(this, rc);

	if (rc)
		rLength = INFINITY;
	else
		L.Length(rLength);

	RETURN_RC(CUnboundedLine::Length, rc);
	}
/************************************************************************/
// Add this line's contribution to the integral of |C, C', Up|
RC CUnboundedLine::UpdateAreaIntegral(
	double & rSoFar,	// In/Out: The integral
	P3Point pUp) const  // In: Upward unit normal (NULL OK)
    {
	RC rc;
	CLine L(this, rc);

	if (rc)
		rSoFar = INFINITY;
	else
		rc = L.UpdateAreaIntegral(rSoFar, pUp);

	RETURN_RC(CUnboundedLine::UpdateAreaIntegral, rc);
    }
/*****************************************************************************/
// Create polyline edge from this curve
RC CUnboundedLine::GetPolylineEdges(
	P3Point pVertices,	// Out: The vertex point point
	double* pBulges,	// Out: The bulge
	int	& nEdges)		// Out: The number of resulting edges
	{
	RC rc;
	CLine L(this, rc);

	if (rc)
		nEdges = 0;
	else
		L.GetPolylineEdges(pVertices, pBulges, nEdges);

	RETURN_RC(CUnboundedLine::GetPolylineEdges, rc);
	}
/************************************************************************/
// Set the domain
RC CUnboundedLine::SetDomain(
	double rFrom,  // In: New curve start
	double rTo)    // In: New curve end
	{
	RC rc = FAILURE;

	if (rFrom < rTo - FUZZ_GEN)
		{
		rc = SUCCESS;
		m_rStart = rFrom;
		m_rEnd = rTo;
		}

	RETURN_RC(CUnboundedLine::SetDomain, rc);
	}
/************************************************************************/
// Reset the domain interval's start
RC CUnboundedLine::ResetStart(
	double rNew)  // In: New curve start
	{
	RC rc = FAILURE;

	if (rNew <= m_rEnd)
		{
		rc = SUCCESS;
		m_rStart = rNew;
		}
	RETURN_RC(CUnboundedLine::ResetStart, rc);
	}
/************************************************************************/
// Reset the domain interval's end
RC CUnboundedLine::ResetEnd(
	double rNew)  // In: New curve end
	{
	RC rc = FAILURE;

	if (rNew >= m_rStart)
		{
		rc = SUCCESS;
		m_rEnd = rNew;
		}
	RETURN_RC(CUnboundedLine::ResetEnd, rc);
	}
/************************************************************************/
// Return a restriction to a new domain
RC CUnboundedLine::ReplaceWithRestriction(
	double from,   // In: New curve start
	double to,     // In: New curve end
	PCurve & pOut) // Out: The restricion to the new domain
	{
	RC rc = SetDomain(from, to);
	if (rc)
		pOut = NULL;
	else
		pOut = this;

	RETURN_RC(CUnboundedLine::ReplaceWithRestriction, rc);
	}

/************************************************************************/
// Project a curve to a plane along a specified direction. If the direction
// is not specified, the curve will be projected perpendicularly to the
// plane(along the normal direction of plane). Function returns NULL when
// it fails and a point to the curve otherwise.
//
// Auther:	Stephen W. Hou
// Date:	6/14/2004
//
PCurve CUnboundedLine::ProjectTo(const CPlane & plane, const C3Vector &dir, bool get2dCurve) const
{
	CUnboundedLine* pCurve = NULL;
	C3Vector along = (&dir) ? dir : plane.GetNormal();

	C3Point origin;
	C3Vector lineDir;
	if (plane.ProjectPoint(Origin(), along, origin, get2dCurve) == SUCCESS &&
		plane.ProjectVector(C3Vector(Direction()), along, lineDir, get2dCurve) == SUCCESS) {
		pCurve = new CUnboundedLine(origin, lineDir.AsPoint());
        pCurve->SetAs2DCurve(get2dCurve);
	}
	return pCurve;
}
/**************************************************************************/
// Reutrn TRUE if the line is degenerated to a point and FALSE otherwise.
//
// Auther:	Stephen W. Hou
// Date:	6/14/2004
//
BOOL CUnboundedLine::IsDegenerated() const
{
	return FEQUAL(m_rStart, m_rEnd, FUZZ_DIST);
}


//////////////////////////////////////////////////////////////////
//																//
//						Linear Gap								//
//																//
//////////////////////////////////////////////////////////////////
PCurve CGap::Clone() const
	{
	return new CGap(*this);
	}

//	Linearize the gap
RC CGap::Linearize(
	double rResolution,			// In: Chord-height toelrance (ignoreed here)
	PAffine pTransform,			// In: Rendering transformation (NULL OK)
	CCache & cCache) const		// In: The cache we render into
	{
	C3Point P;
	RC rc;

	if (!cCache.Started())
		// Do the start point
		if (rc = AddPoint(pTransform, m_ptStart, 0, cCache))
			QUIT

	// Do the end point
	if (pTransform)
		pTransform->ApplyToPoint(m_ptEnd, P);
	else
		P = m_ptEnd;
	rc = cCache.SkipTo(P);

exit:
	RETURN_RC(CGap::Linearize, rc);
	}
/****************************************************************************/
// Linearize a portion of the line
RC CGap::LinearizePart(
	double rFrom,			// In: Where to start
	double rTo,				// In: Where to stop
	double rResolution,		// In: Chord-height toelrance
	PAffine pTransform,		// In: Rendering transformation (NULL OK)
	CCache & cCache) const	// In: The cache we render into
	{
	C3Point P;
	RC rc = SUCCESS;
	ASSERT(!pTransform || pTransform->Affine());

	if (!cCache.Started())
		{
		Evaluate(rFrom, P);
		if (rc = AddPoint(pTransform, P, rFrom, cCache))
			QUIT
		}

	Evaluate(rTo, P);
	if (pTransform)
		pTransform->ApplyToPoint(P, P);
	rc = cCache.SkipTo(P);
exit:
	RETURN_RC(CGap::LinearizePart, rc);
	}
/************************************************************************/
// Length
RC CGap::Length(
	double & rLength) const   // Out: The total length
    {
	// A Gap has zero length
	rLength = 0;
	RETURN_RC(CGap::Length, SUCCESS);
    }
/*****************************************************************************/
// Coalesce this gap with another, if possible
BOOL CGap::CoalescedWith(
	PCurve pCurve)		// In/Out: the curve we try to coalesce with
	{
	if (pCurve->Type() == TYPE_Gap)
		return CLine::CoalescedWith(pCurve);
	else
		return FALSE;
	}
/*******************************************************************************/
// Persist this line
RC CGap::Persist(
	PStorage pStorage) const	// In/Out: The storage
	{
	RC rc = pStorage->AcceptGap(this);
	RETURN_RC(CGap::Persist, rc);
	}
