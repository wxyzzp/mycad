/*	CrvPair.cpp - Implements the class of CCurvePair and its subclasses
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * See CrvPair.h for the definition of the class.
 * This file contains the implemetation of varoius 2 curve methods, such as
 * intersection and corner-rounding, in a way that allows overriding them for
 * special cases where it would be more effiecient (e.g. lin-line intersections).
 * The curves will be asked to identify their types strictly in one place: the
 * function GetCurvePair.  The function returns a subclass of the appropriate
 * type, for which special case methods may have been implemented.
 */
#include "Geometry.h"
#include "GeomPtrs.h"
#include "vector.h"
#include "TheUser.h"
#include "Line.h"
#include "EllipArc.h"		// For corner rounding
#include "CrvOfset.h"		// For corner rounding
#include "Roots.h"			// For the CRealFunction
#include "2CurveSolver.h"	// For CCurveDistance
#include "CrvPair.h"
#include "polynomial.h"
#include "trnsform.h"
#include "Plane.h"

DB_SOURCE_NAME

LOCAL RC AddIntersection(
	double rOnFirst,      // In: Parameter value on the first curve
	double rOnSecond,     // In: Parameter value on the second curve
	BOOL bSwap,           // In: Swap before adding, if ==TRUE
	CRealArray & cList1,  // In/Out: List of parameter values on first curve
	CRealArray & cList2); // In/Out: List of parameter values on second curve

/*******************************************************************************/
// Constructors/destructor
CCurvePair::CCurvePair()
	{
	m_pFirst = m_pSecond = NULL;
	m_bSwap = FALSE;
	}

CCurvePair::CCurvePair(
	PCurve pFirst,      // In: First curve
	PCurve pSecond,     // In: Second curve
	BOOL bSwap) 		// In: =TRUE if the returned results are to be swapped
	{
	m_pFirst = pFirst;
	m_pSecond = pSecond;
	m_bSwap = bSwap;
	}

CCurvePair::~CCurvePair()
	{
	}

/*******************************************************************************/
// Identify the types of 2 curves and package them as a curve-pair
RC GetCurvePair(
	PCurve pFirst,     // In: First curve
	PCurve pSecond,    // In: Second curve
	BOOL bCommute,     // In: =TRUE if this is done for a cummutative operation
	PCurvePair & pPair) // Out: The pair
	{
/*
 * Swapping is done to cut in half the number of combinations, so that we can use
 * a LineArc method, for example, where the input is ArcLine.  This is only valid
 * if the operation is cummutative.
 * An operation X is considered commutative if instead of A X B we're allowed
 * to do B X A, and then swap the output, if necessary.  Intersection, for
 * example, is commutative.  Corner-rounding, on the other hand, is not, because
 * it assumes that the end of A is the beginning of B, and that is no longer
 * true for the pair B A.
 * So, when an operation is not commutative and swapping would be called for,
 * a generic CurvePaire will be returned, which means that the operation will
 * be performed without taking advantage of any special case method.
 */
	RC rc = SUCCESS;
	pPair = NULL;
	CURVETYPE typeFirst = pFirst->Type();
	CURVETYPE typeSecond = pSecond->Type();
	BOOL bSwap = (typeFirst > typeSecond);

	// Swap if necessary and permitted
	if (bSwap)
		if (bCommute)
			{
			SWAP(pFirst, pSecond, PCurve)
			SWAP(typeFirst, typeSecond, CURVETYPE)
			}
		else  // Swapping needed, but not allowed
			{
			pPair = new CCurvePair(pFirst, pSecond, FALSE);
			goto exit;
			}

	// To be enhanced as more special-case methods get implemented
	switch (pFirst->Type())
		{
		case TYPE_Line:
		case TYPE_UnboundedLine:
		case TYPE_Ray:
			switch (pSecond->Type())
				{
				case TYPE_Line:
				case TYPE_UnboundedLine:
				case TYPE_Ray:
					pPair = new CLineLine(pFirst, pSecond, bSwap);	    break;

				case TYPE_Elliptical:
					pPair = new CLineEllipArc(pFirst, pSecond, bSwap);  break;

				default:
					pPair = new CCurvePair(pFirst, pSecond, bSwap);		break;
				}
			break;

		case TYPE_Elliptical:
			switch (pSecond->Type())
				{
				case TYPE_Elliptical:
					pPair = new CEllipArcEllipArc(pFirst, pSecond, bSwap);
					break;

				default:
					pPair = new CCurvePair(pFirst, pSecond, bSwap);
					break;
				}
			break;
		default:
			pPair = new CCurvePair(pFirst, pSecond, bSwap);
			break;
		}
exit:
	if (!pPair)
		rc = DM_MEM;
	RETURN_RC(GetCurvePair, rc);
	}

/*******************************************************************************/
// Round a corner between two curves with a circular arc of a given radius
RC RoundCorner(
	PCurve pFirst,      // In: First curve
	double rLimit1,     // In: How much of the first curve may be trimmed away
	PCurve pSecond,		// In: Second curve
	double rLimit2,     // In: How much of the second curve may be trimmed away
	double rRadius,     // In: Radius of rounding arc
	PEllipArc & pArc)   // Out: The arc that rounds the corner
/*
 * TrimLimit 1 and 2 must be between 0 and 1.  They indicate a relative portion
 * of the curve's parameter domain that may be trimmed when rounding corner.
 * It is assumed without checking that the end of the first curve coincides
 * with the beginning of the second curve.  A new circular arc is created.
 * Both curves are trimmed to match its endpoints
 */
	{
 	C3Point A, B, U, V;
	double rContact1, rContact2;
	BOOL bTrimmed = FALSE;
	PCurvePair pPair = NULL;

	RC rc = E_NOMSG;
	if (pFirst->Type() == TYPE_Gap  ||  pSecond->Type() == TYPE_Gap)
		goto exit;

	// Get a CurvePair
	if (rc = GetCurvePair(pFirst, pSecond, FALSE, pPair))
		QUIT

	// Get the possibly adjusted radius, and the contact points
	if (rc = pPair->RoundCorner(rLimit1, rLimit2, rRadius, rContact1, rContact2))
	   	QUIT
	bTrimmed = TRUE;

 	// Get conatct points and tangents on the curves
 	if (rc = pFirst->Evaluate(rContact1, A, U))
 		QUIT
 	if (rc = pSecond->Evaluate(rContact2, B))
 		QUIT

 	// Construct the arc
 	pArc = new CEllipArc(A, U, B, rRadius, rc);
	if (!pArc)
		rc = DM_MEM;
 	if (rc)
 		QUIT

	// Trim the curves
	if (rc = pFirst->ResetEnd(rContact1))
		QUIT
	rc = pSecond->ResetStart(rContact2);
exit:
 	if (rc)
 		{
 		// Undo
		if (bTrimmed)
			{
			pFirst->ResetEnd (pFirst->End());
			pSecond->ResetStart (pSecond->Start());
			}
 		delete pArc;
 		pArc = NULL;
 		}
	delete pPair;
	RETURN_RC(RoundCorner, rc);
	}
/*******************************************************************************/
// Get the radius & contact points on the curves for a corner-rounding arc
RC CCurvePair::RoundCorner(
	double rLimit1,   // In: How much of the first curve may be trimmed away
	double rLimit2,   // In: How much of the second curve may be trimmed away
	double & rRadius, // In/Out: Radius of rounding arc
	double & s,       // Out: Parameter value of contact point on curve 1
	double & t)       // Out: Parameter value of contact point on curve 2
 	{
/*
 * The previous version iterated, looking for a smaller radius when rounding fails.
 * That was too time consuming in some cases.  mxk, 9/1/95
 * Restored a simplified version, iterating 3 times.  mxk, 10/6/95
 */
	double r = 0;
	double rThisMid, rOtherMid;
 	RC rc = FAILURE;
	COffsetCurve ThisOffset(m_pFirst, 0);
	COffsetCurve OtherOffset(m_pSecond, 0);
	CCurvePair cOffsets(&ThisOffset, &OtherOffset, FALSE);
	int i, nDirection;
 	s = t = 0;

	// Validate input
	if (rLimit1 <= FUZZ_DIST  ||  rLimit1 > 1.0  ||
		rLimit2 <= FUZZ_DIST  ||  rLimit2 > 1.0)
		QUIT

	// Determine if we need to offset to the left or to the right
	if (rc = RelativeDirection(*m_pFirst, *m_pSecond, m_pFirst->End(),
							   m_pSecond->Start(), nDirection))
		QUIT
 	r = rRadius * nDirection;

 	// Get the midpoint of the parameter domain
 	rThisMid = m_pFirst->End() - rLimit1 * (m_pFirst->End() - m_pFirst->Start());
 	rOtherMid = m_pSecond->Start() +
 			    rLimit2 * (m_pSecond->End() - m_pSecond->Start());

	for (i = 1;    i <= 3;    i++)
		{
		rc = SUCCESS;

		// Set the offset to r
		ThisOffset.SetOffset(r);
		OtherOffset.SetOffset(r);

		// Intersect them
		if (!cOffsets.IntersectSeed(m_pFirst->End(),  m_pSecond->Start(), s, t)
		 		  &&  s >= rThisMid  &&   s <= m_pFirst->End()
		 		  &&  t >= m_pSecond->Start()   &&   t <= rOtherMid)
			break;
	 	// Intersection failed, try again with midpoints as seeds
	 	if (!cOffsets.IntersectSeed(rThisMid,  rOtherMid, s, t)
		 		  &&  s >= rThisMid  &&   s <= m_pFirst->End()
		 		  &&  t >= m_pSecond->Start()   &&   t <= rOtherMid)
			break;

		// No intersection, halve the radius and try again
		rc = E_NOMSG;
		r /= 2;
		}
exit:
	if (!rc)
		rRadius = fabs(r);
	RETURN_RC(CCurvePair::RoundCorner, rc);
	}
//////////////////////////////////////////////////////////////////////////////////
//																				//
//						CURVE-CURVE INTERSECTION								//
// This is a strictly 2D algorithm, intersecting curves in the XY plane (Z=0).	//
// To intersect curves in any other plane, you have to transform them to the	//
// XY plane first, before you call this method, otherwise the results will be 	//
// incorrect!																	//
//////////////////////////////////////////////////////////////////////////////////

// Get the intersection of an endpoint on one curve with the other's interior
LOCAL RC GetEndIntersections(
	PCurve pFirst,          // In: First curve
	PCurve pSecond,		    // In: Second curve
	CRealArray & cOnThis,   // In/Out: Intersection parameters on this curve
	CRealArray & cOnOther,  // In/Out: Intersection parameters on other curve
	double rTolerance)		// Optional: Tolerance
	{
	RC rc = SUCCESS;
	CRealPoint P;
	double dist, at;

	if (!pFirst->GetStartPoint(P))
		if (!pSecond->PullBack(P, P, at, dist))
			if (dist < rTolerance)
				if (rc = AddIntersection(pFirst->Start(), at, FALSE,
										cOnThis, cOnOther))
					QUIT

	if (!pFirst->GetEndPoint(P))
		if (!pSecond->PullBack(P, P, at, dist))
			if (dist < rTolerance)
				if (rc = AddIntersection(pFirst->End(), at, FALSE,
												cOnThis, cOnOther))
					QUIT
exit:
	RETURN_RC(GetEndIntersections, rc);
	}
/**********************************************************************************/
// Interface: find all the intersections between two curves
RC Intersect(
	PCurve pFirst,          // In: First curve
	PCurve pSecond,		    // In: Second curve
	CRealArray & cOnThis,   // In/Out: Intersection parameters on this curve
	CRealArray & cOnOther,  // In/Out: Intersection parameters on other curve
	double rTolerance)		// Optional: Tolerance
	{

	// Get the intersections of curve interiors
	PCurvePair pPair = NULL;
	RC rc = GetCurvePair(pFirst, pSecond, TRUE, pPair);
	if (rc)
		QUIT
	if (rc = pPair->Intersect(cOnThis, cOnOther, rTolerance))
		QUIT

	// Get endpoint intersections with interior points
	if (rc = GetEndIntersections(pFirst, pSecond, cOnThis,
								 cOnOther, rTolerance))
		QUIT
	rc = GetEndIntersections(pSecond, pFirst, cOnOther,
								 cOnThis, rTolerance);
exit:
	delete pPair;
	RETURN_RC(Intersect, rc);
	}
/*******************************************************************************/
// Interface: find one intersection, given a seed
RC Intersect(
	PCurve pFirst,		// In: First curve
	PCurve pSecond,		// In: Second curve
	double rThisSeed,	// In: Initial guess on this curve
	double rOtherSeed,	// In: Intial guess on the other curve
	double & rOnThis,	// Out: Parameter value on this curve
	double & rOnOther,	// Out: Paramter value on the other curve
	double rTolerance)	// Optional: Point distance tolerance
	{
	// This is the iterarive numerical algorithm, not to be overriden
	CCurvePair cPair(pFirst, pSecond, FALSE);
	if (rTolerance < FUZZ_GEN)
		rTolerance = FUZZ_GEN;
	RC rc = cPair.IntersectSeed(rThisSeed, rOtherSeed, rOnThis,
						 rOnOther, rTolerance);
	RETURN_RC(Intersect, rc);
	}
/**********************************************************************************/
// Interface: find all the intersections between two curves in an arbitrary
// direction.
//
// Author:	Stephen W. Hou
// Date:	06/14/2004
//
RC ProjIntersect(
	PCurve pFirst,           // In: First curve
	PCurve pSecond,		     // In: Second curve
	const C3Vector &projDir, // In: Projected direction
	CRealArray & cOnThis,    // In/Out: Intersection parameters on this curve
	CRealArray & cOnOther,   // In/Out: Intersection parameters on other curve
	double rTolerance)	// Optional: Point distance tolerance
{
	// Get the intersections of curve interiors
	PCurvePair pPair = NULL;
	RC rc = GetCurvePair(pFirst, pSecond, TRUE, pPair);
	if (rc)
		QUIT
	if (rc = pPair->ProjIntersect(projDir, cOnThis, cOnOther, rTolerance))
		QUIT

	// Get endpoint intersections with interior points
	if (rc = GetEndIntersections(pFirst, pSecond, cOnThis,
								 cOnOther, rTolerance))
		QUIT
	rc = GetEndIntersections(pSecond, pFirst, cOnOther,
								 cOnThis, rTolerance);
exit:
	delete pPair;
	RETURN_RC(ProjIntersect, rc);
}
/**********************************************************************************/
RC AreDuplicates(
	PCurve pThis,			// In: A curve
	PCurve pOther,			// In: A potential duplicate
	double rTolerance,		// In: Tolerance
	BOOL & bDuplicate) // Out: =TRUE if Other duplicates this path
	{
// This function is caled after we already know that the endpoints coincide
	PCurvePair pPair;
	RC rc = GetCurvePair(pThis, pOther, TRUE, pPair);
	if (!rc  &&  pPair)
		rc = pPair->AreDuplicates(rTolerance, bDuplicate);
	delete pPair;
	RETURN_RC(AreDuplicates, rc);
	}
/////////////////////////////////////////////////////////////////////
//		Utilities
// Add an intersection record to the list
LOCAL RC AddIntersection(
	double rOnFirst,       // In: Parameter value on the first curve
	double rOnSecond,      // In: Parameter value on the second curve
	BOOL bSwap,          // In: Swap before adding, if ==TRUE
	CRealArray & cList1, // In/Out: List of parameter values on first curve
	CRealArray & cList2) // In/Out: List of parameter values on second curve
/*
 * It is assumed, without checking here, that both parameter values
 * are between curve start and curve end if period = 0, or between
 * curve and curve start + period if period > 0.
 */
{
	RC rc = SUCCESS;
	int n;

	// Swap, if necessary
	if (bSwap)
		SWAP(rOnFirst, rOnSecond, double)

	// If it's a new intersection --
	if (!cList1.Search(rOnFirst, FUZZ_DIST/*FUZZ_GEN*/, n)	&&
		!cList2.Search(rOnSecond, FUZZ_DIST/*FUZZ_GEN*/, n)) {
		// -- add it to the lists
		if (rc = cList1.Add(rOnFirst))
			QUIT
		rc = cList2.Add(rOnSecond);
	}
exit:
	RETURN_RC(AddIntersection, rc);
}

/**********************************************************************************/
RC CCurvePair::AreDuplicates(
	double rTolerance,		// In: Tolerance
	BOOL & bDuplicate)   // Out: =TRUE if Other duplicates this path
// This function is called after we already know that the ends coincide
	{
	RC rc =SUCCESS;
	double s, rDist;
	C3Point P, Q, T, V;

	bDuplicate = FALSE;

	for (int i = 1;  i <= 3;  i++)
		{
		if (m_pSecond->RandomPoint(s, P, T))
			goto exit;
		if (rc = m_pFirst->NearestPoint(P, P, s, rDist))
			QUIT
		if (rDist > rTolerance)
			goto exit;
		}

	// Passed the test
	bDuplicate = TRUE;
exit:
	RETURN_RC(CCurvePair::AreDuplicates, rc);
	}
/****************************************************************************/
// Find one intersection point from a given pair of seeds
RC CCurvePair::IntersectSeed(
	double rThisSeed,	 // In: Initial guess on this curve
	double rOtherSeed,	 // In: Intial guess on the other curve
	double & rOnThis,	 // Out: Parameter value on this curve
	double & rOnOther,	 // Out: Paramter value on the other curve
	double rFuzz)		 // Optional: Point distance tolerance
	{
	// Find a local minimum of the distance between the curve interiors
	CCurvesDistance solver(m_pFirst, m_pSecond, rFuzz/*0*/);
	RC rc = solver.Solve(rThisSeed, rOtherSeed);
	if (rc)
		QUIT

	// Until the result passes some tests
	rc = E_NOMSG;
	if (solver.Singular())
		QUIT

	// See if this constitutes an intersection
	if (solver.SqDist() > rFuzz * rFuzz)
		QUIT
	rOnThis = solver.Parameter(0);
	rOnOther = solver.Parameter(1);
	if (m_pFirst->PullIntoDomain(rOnThis) &&
 		m_pSecond->PullIntoDomain(rOnOther))
	rc = SUCCESS;
exit:
 	RETURN_RC(CCurvePair::IntersectSeed, rc);
 	}
/*******************************************************************************/
// Find all intersections and overlaps
RC CCurvePair::Intersect(
	CRealArray & cOnThis,  // In/Out: Intersection parameters on this curve
	CRealArray & cOnOther, // In/Out: Intersection parameters on other curve
	double rTolerance)	   // Optional: Tolerance
{
	RC rc = SUCCESS;
	int i,j;
	CRealArray arrSeeds1, arrSeeds2;
	double s, t;

	// Initial arrays because they may not be empty
	cOnThis.RemoveAll();
	cOnOther.RemoveAll();

	// Generate seeds on both curves
	if (rc = m_pFirst->Seeds(arrSeeds1))
		QUIT
	if (rc = m_pSecond->Seeds(arrSeeds2))
		QUIT

	// Loop on seeds
    for (i=0;  i<arrSeeds1.Size();  i++) {
        for (j=0;  j<arrSeeds2.Size();  j++) {
			// Find an intersection from this pair of seeds
            if (!IntersectSeed(arrSeeds1[i], arrSeeds2[j], s, t, rTolerance)) {
                C3Point point1, point2;
                m_pFirst->Evaluate(s, point1);
                m_pSecond->Evaluate(t, point2);
                if (point1.DistanceTo(point2) <= rTolerance) {
				    // An intersection was found, add it to the list
				    if (rc = AddIntersection(s, t, m_bSwap, cOnThis, cOnOther))
					    QUIT
                }
            }
        }
    }
exit:
	RETURN_RC(CCurvePair::Intersect, rc);
}
/*******************************************************************************/
// Find all intersections viewed in the projected direction
//
// Author: Stephen W. Hou
// Date:   6/21/2004
//
RC CCurvePair::ProjIntersect(
	const C3Vector &projDir, // In: Projected direction
	CRealArray & cOnThis,    // In/Out: Intersection parameters on this curve
	CRealArray & cOnOther,   // In/Out: Intersection parameters on other curve
	double rTolerance)		 // Optional In: Point distance tolerance
{
	RC rc = FAILURE;
	cOnThis.RemoveAll();
	cOnOther.RemoveAll();

	CPlane plane(C3Point(), projDir);

	PCurve pCurve1 = m_pFirst->ProjectTo(plane);
	PCurve pCurve2 = m_pSecond->ProjectTo(plane);
	if (pCurve1 && pCurve2) {
		CRealArray cOnCurve1, cOnCurve2;
			CRealArray cOnProjCrv2d, cOnModofier2d, cOnModifiedCurve, cOnViewingLine;

        rc = ::Intersect(pCurve1, pCurve2, cOnCurve1, cOnCurve2, FUZZ_DIST);
		if (cOnCurve1.Size() > 0) {
			double rCurvature;
			C3Point ptPoint, ptTangent;
			for (register i = 0; i < cOnCurve1.Size(); i++) {
				pCurve1->Evaluate(cOnCurve1[i], ptPoint, ptTangent, rCurvature);

				CUnboundedLine viewingLine(ptPoint, plane.GetNormal().AsPoint());
                if (::Intersect(m_pFirst, &viewingLine, cOnThis, cOnViewingLine, FUZZ_DIST) == SUCCESS ||
                    ::Intersect(m_pSecond, &viewingLine, cOnOther, cOnViewingLine, FUZZ_DIST) != SUCCESS)
                    rc =  SUCCESS;
                else
                    rc =  FAILURE;
			}
        }
    }
    if (pCurve1)
        delete pCurve1;

    if (pCurve2)
	    delete pCurve2;

	return rc;
}
/////////////////////////////////////////////////////////////////////////////////
// Line-line curve-pair

// Constructors/destructor
CLineLine::CLineLine()
	{
	}

CLineLine::CLineLine(
	PCurve pFirst,      // In: First curve
	PCurve pSecond,     // In: Second curve
	BOOL bSwap) 		// In: =TRUE if the returned results are to be swapped
	// Call the right base class constructor
	: CCurvePair(pFirst, pSecond, bSwap)
	{
	}

CLineLine::~CLineLine()
	{
	}

/*******************************************************************************/
// Get the direction vector and distance between segment's points
LOCAL RC DirectionAndDistance( // Return E_ZeroVector if |V| = 0
	PLine pLine,		// In: The line
	C3Point& V,			// Out: Unitized differenc vector
	double& rLength)	// Out: |V|
	{
	RC rc;
	C3Point P;

	if (rc = pLine->GetStartPoint(P))
		QUIT
	if (rc = pLine->GetEndPoint(V))
		QUIT
	V -= P;

	rLength = V.Norm();
	if (FZERO(rLength, FUZZ_DIST))
		rc = E_NOMSG;	// Can happen with lines of zero length
	else
		V /= rLength;
exit:
	RETURN_RC(DirectionAndDistance, rc);
	}
/*******************************************************************************/
// Get the radius and contact points on the lines for a corner-rounding arc
RC CLineLine::RoundCorner(
	double rLimit1,     // In: How much of the first line may be trimmed away
	double rLimit2,     // In: How much of the second line may be trimmed away
	double & rRadius,   // In/Out: Radius of rounding arc
	double & rContact1, // Out: Parameter value of contact point on line 1
	double & rContact2) // Out: Parameter value of contact point on line 2
	{
	RC rc;
/*
 * The contact points of the arc between 2 lines are Radius*cot(1/2 angle) away
 * from their intersection.  But cot(a/2) = sqrt((1+cos(a)) / (1 - cos(a))),
   and cos(a) = -U*V, where U and V are the lines' direction unit vectors.
 */
	// Initialize
	rContact1 = rContact2 = 0;

	// Compute cot(angle/2)
	double rLength1, rLength2, rCot, r, s;
	C3Point V1, V2;
	if (rc = DirectionAndDistance(Line1(), V1, rLength1))
		QUIT
	if (rc = DirectionAndDistance(Line2(), V2, rLength2))
		QUIT

	rc = E_NOMSG;	// Presumed guilty until proven otherwise
	r = V1 * V2;
	s = 1.0 + r;

	if (s < FUZZ_GEN)
		goto exit;
	rCot = sqrt((MAX(1.0 - r, 0)) / s);

	// Compute the setback from the corner
	r = rRadius * rCot;
	s = MIN(rLimit1 * rLength1, rLimit2 * rLength2);
	if (s < r)
		{
		// We have to reduce the radius
		r = s;
		rRadius = r / rCot;
		}

	// Get the contact values
	rContact1 = Line1()->End() - r / rLength1;
	rContact2 = Line2()->Start() + r / rLength2;
	rc = SUCCESS;
exit:
	RETURN_RC(CLineLine::RoundCorner, rc);
	}
/*******************************************************************************/
// Find all intersections and overlaps
RC CLineLine::Intersect(
	CRealArray & cList1,  // In/Out: Intersection parameters on first curve
	CRealArray & cList2,  // In/Out: Intersection parameters on second curve
	double rTolerance)	  // Optional: Tolerance - ignored here
	{
	C3Point U, V, AB;

	// Rule out all singulrities
	RC rc = SUCCESS;

	// Initial arrays because they may not be empty
	cList1.RemoveAll();
	cList2.RemoveAll();

	double r, s, t, u, v, dotp;
	u = Line1()->Velocity().Norm();
	v = Line2()->Velocity().Norm();
	if (FZERO(u, FUZZ_GEN)  ||  FZERO(v, FUZZ_GEN))
		// At least one of the lines degenerates to a point
		goto exit;

	U = Line1()->Velocity() / u;
	V = Line2()->Velocity() / v;
    dotp = U * V;

    if (FEQUAL(fabs(dotp), 1.0, FUZZ_GEN)) // Changed by SWH - 7/8/2004
		// The lines are either parallel or coincidident
		goto exit;
    else if (FZERO(dotp, FUZZ_GEN)) {
        // The lines are perpendicular. The problem become to fine the closest point on
        // another line.  Stephen W. Hou - 7/8/2004
        double rDist;
        C3Point ptNearest;
        Line1()->PullBack(Line2()->Origin(), ptNearest, s, rDist);
        if (!FZERO(V.X(), FUZZ_GEN))
            t = (ptNearest.X() - Line2()->Origin().X()) / Line2()->Velocity().X();
        else if (!FZERO(V.Y(), FUZZ_GEN))
            t = (ptNearest.Y() - Line2()->Origin().Y()) / Line2()->Velocity().Y();
        else
            t = (ptNearest.Z() - Line2()->Origin().Z()) / Line2()->Velocity().Z();
    }
    else {

	    /*
	     * Suppose the lines are A + sU and B + tV.
	     * Then solve the 2 equations with 2 unknowns s and t:
   	     * A + sU = B + tV, or  sU - tV = B - A.  The solution is:
   	     *
	     *		 det(B-A, V)			  det(U, B-A)
   	     *	 s = -----------,		t = - -----------,
   	     *        det(U, V)				   det(U, V)
	     *
	     * For checking singularity I initially need the determinant of the
	     * normalized U/u and V/v, where u=|U| and v=|V|.  So I worked the
	     * rest of the calculations with that determinant in the denominator,
	     * and with these normalized vectors in the numerator.  This requires
	     * dividing these expressions by v and u, respectively
	     */
        // If the third vector is not supplied, Determinant() will use Z axis
        // as default vector to compute the value of determinat. In the case
        // z axis and U and V vectors are coplanar, the value of r will be zero.
        // which led to a singular result. To ensure this will not happen,
        // we have to specify the third vector and make sure it is perpendicular
        // to the first two vectors when we call Determinant().
        //
        // Stephen W. Hou - 7/16/2004
        //
	    // r = Determinant(U, V);
	    // AB = Line2()->Origin() - Line1()->Origin();
	    // s = Determinant(AB, V) / (r * u);
	    // t = - Determinant(U, AB) / (r * v);
        C3Vector uVector(U), vVector(V);
        C3Vector N(uVector ^ vVector);
        N.Normalized();
	     r = Determinant(U, V, N.AsPoint());
	     AB = Line2()->Origin() - Line1()->Origin();
	     s = Determinant(AB, V, N.AsPoint()) / (r * u);
	     t = - Determinant(U, AB, N.AsPoint()) / (r * v);
    }

	if (m_pFirst->PullIntoDomain(s)  &&  m_pSecond->PullIntoDomain(t))
		rc = AddIntersection(s, t, m_bSwap, cList1, cList2);

exit:
	RETURN_RC(CLineLine::Intersect, rc);
	}
/**********************************************************************************/
RC CLineLine::AreDuplicates(
	double rTolerance,		// In: Tolerance
	BOOL & bDuplicate)   // Out: =TRUE if Other duplicates this path
// This function is called after we already know that the ends coincide
	{
	// Line segments whose ends coinicide are duplicates
	bDuplicate = TRUE;
	RETURN_RC(CLineLine::AreDuplicates, SUCCESS);
	}
/////////////////////////////////////////////////////////////////////////////////
// Line-elliptical arc

// Constructors/destructor
CLineEllipArc::CLineEllipArc()
	{
	}

CLineEllipArc::CLineEllipArc(
	PCurve pFirst,      // In: First curve
	PCurve pSecond,     // In: Second curve
	BOOL bSwap) 		// In: =TRUE if the returned results are to be swapped
	// Call the right base class constructor
	: CCurvePair(pFirst, pSecond, bSwap)
	{
	}

CLineEllipArc::~CLineEllipArc()
	{
	}
/*******************************************************************************/
// Find all intersections and overlaps
RC CLineEllipArc::Intersect(
	CRealArray & cList1,  // In/Out: Intersection parameters on first curve
	CRealArray & cList2,  // In/Out: Intersection parameters on second curve
	double rTolerance)	  // Optional: Tolerance
/*
 * The arc's parametric representation is C + A cos(t) + B sin(t).
 * Write a (nonparametric) equation U*X = d for the line.  We need to
 * solve
 * 				U*(C + A cos(t) + B sin(t)) = d,
 * or:
 * 				U*A cos(t) + U*B sin(t) + U*C-d = 0;
 * but
 *				d = U*P,
 *
 * where P is any point on the line, so the equation is:
 *
  * 			U*A cos(t) + U*B sin(t) + U*(C-P) = 0;
*
 */
	{
	RC rc = SUCCESS;
	int i, nRoots;
	double r[2];

	cList1.RemoveAll();
	cList2.RemoveAll();

	// Get an equation of the form U*X = r for the line
	C3Point U = Line()->Velocity();

    // The equation U*A cos(t) + U*B sin(t) + U*(C-P) = 0 has solution iff
    // the line is not parallel to the central axis(normal) of ellipse or
    // is not perpendicular to the direction pointing from start point of
    // the line to the center of the ellipse. SWH - 7/6/2004
    //
    C3Vector normal(Arc()->MajorAxis() ^ Arc()->MinorAxis());
    normal.Normalized();

    C3Vector dir(U);
    dir.Normalized();

    double cosa = normal * dir;
    if (FEQUAL(fabs(cosa), 1.0, FUZZ_RAD)) {
        // The line is parallel to the normal of ellipse. In this case, we project the 
        // start point of the line to the plane of the ellipse and then find the closest 
        // point on the plane within the tolerance zone as an intersection point. 
        // SWH - 7/2/2004
        //
        double c = U * (Arc()->Center() - Line()->Origin());

        C3Point ptOnPlane;
        if (FZERO(c, FUZZ_DIST)) {
            // The start point of line is on the plane of the ellipse
            //
            ptOnPlane = Line()->Origin();
        }
        else {
            CPlane plane(Arc()->Center(), normal, C3Vector(Arc()->MajorAxis()));
            if ((rc = plane.ProjectPoint(Line()->Origin(), U, ptOnPlane, false)) != SUCCESS)
                return rc;
        }
        C3Point ptNearest;
        double rAtEllipse, rAtLine, rDis;
        if (Arc()->NearestPoint(ptOnPlane, ptNearest, rAtEllipse, rDis) != SUCCESS || rDis > rTolerance ||
            Line()->PullBack(ptOnPlane, ptNearest, rAtLine, rDis) || rDis > rTolerance)
            return FAILURE;

        if (rc = AddIntersection(rAtLine, rAtEllipse, m_bSwap, cList1, cList2))
		    QUIT
    }
    else {

	    U.TurnLeft();
	    double c = U * (Arc()->Center() - Line()->Origin());

	    if (rc = SolveTrigEquation1(U*Arc()->MajorAxis(), U*Arc()->MinorAxis(),
							        c, nRoots, r[0], r[1]))
		    QUIT

	    for (i = 0;  i < nRoots;  i++) {
		    double rAt, rDistance;

		    if (!Arc()->PullIntoDomain(r[i]))
			    // We are outside the arc's domain, so skip the rest
			    continue;

		    // Get the parameter value on the line
		    if (rc = Arc()->Evaluate(r[i], U))
			    QUIT
		    if (Line()->PullBack(U, U, rAt, rDistance))
			    // We are outside the line's domain, so skip the rest
			    continue;

		    // Record the intersection
		    if (rDistance < rTolerance)
			    if (rc = AddIntersection(rAt, r[i], m_bSwap, cList1, cList2))
				    QUIT
	    }
    }

exit:
	RETURN_RC(CLineEllipArc::Intersect, rc);
	}
/**********************************************************************************/
RC CLineEllipArc::AreDuplicates(
	double rTolerance,		// In: Tolerance
	BOOL & bDuplicate)   // Out: =TRUE if Other duplicates this path
	{
	// A line and an arc cannot coincide
	bDuplicate = FALSE;
	RETURN_RC(CLineEllipArc::AreDuplicates, SUCCESS);
	}
/////////////////////////////////////////////////////////////////////////////////
// Elliptical arc-elliptical arc

// Constructors/destructor
CEllipArcEllipArc::CEllipArcEllipArc()
	{
	}

CEllipArcEllipArc::CEllipArcEllipArc(
	PCurve pFirst,      // In: First curve
	PCurve pSecond,     // In: Second curve
	BOOL bSwap) 		// In: =TRUE if the returned results are to be swapped
	// Call the right base class constructor
	: CCurvePair(pFirst, pSecond, bSwap)
	{
	}

CEllipArcEllipArc::~CEllipArcEllipArc()
	{
	}

/******************************************************************************************/
//
// Compute the intersections of two 2D ellipses whole axes are parallel to the x and y axis
// of the coordinate system.
//
// Author:  Stephen W. Hou
// Date:    12/15/2003
//
vector<C3Point>
compute2DEllipseEllipseIntersections(double cx1,  // x coordinate of the center of the first ellipse
									 double cy1,  // y coordinate of the center of the first ellipse
									 double rx1,  // the length of first axis of the first ellipse
									 double ry1,  // the length of second axis of the first ellipse
									 double cx2,  // x coordinate of the center of the second ellipse
									 double cy2,  // y coordinate of the center of the second ellipse
									 double rx2,  // the length of first axis of the second ellipse
									 double ry2   // the length of second axis of the second ellipse
									)
{
	// convert ellipses to polynormial representation A*X^2 + B*X*Y + C*Y^2 + D*X + E*Y + F = 0
	//
	double A1 = ry1 * ry1;
	double B1 = 0.0;
	double C1 = rx1 * rx1;
	double D1 = -2.0 * ry1 * ry1 * cx1;
	double E1 = -2.0 * rx1 * rx1 * cy1;
	double F1 = ry1 * ry1 * cx1 * cx1 + rx1 * rx1 * cy1 * cy1 - rx1 * rx1 * ry1 * ry1;

    double A2 = ry2 * ry2;
	double B2 = 0.0;
	double C2 = rx2 * rx2;
	double D2 = -2.0 * ry2 * ry2 * cx2;
	double E2 = -2.0 * rx2 * rx2 * cy2;
    double F2 = ry2 * ry2 * cx2 * cx2 + rx2 * rx2 * cy2 * cy2 - rx2 * rx2 * ry2 * ry2;

	// construct a polynomial is as a Bezout determinant
	//
    double AB = A1 * B2 - A2 * B1;
    double AC = A1 * C2 - A2 * C1;
    double AD = A1 * D2 - A2 * D1;
    double AE = A1 * E2 - A2 * E1;
    double AF = A1 * F2 - A2 * F1;
    double BC = B1 * C2 - B2 * C1;
    double BE = B1 * E2 - B2 * E1;
    double BF = B1 * F2 - B2 * F1;
    double CD = C1 * D2 - C2 * D1;
    double DE = D1 * E2 - D2 * E1;
    double DF = D1 * F2 - D2 * F1;
    double BFpDE = BF + DE;
    double BEmCD = BE - CD;

    CPolynomial yPolynomial(AD * DF - AF * AF,
		                    AB * DF + AD * BFpDE - 2.0 * AE * AF,
					        AB * BFpDE + AD * BEmCD - AE * AE - 2.0 * AC * AF,
							AB * BEmCD + AD * BC - 2.0 * AC * AE,
						    AB * BC - AC * AC);

	// solve the polynormial equation
	//
	vector<double> yRoots;
    yPolynomial.getRoots(yRoots);

    double epsilon = 1e-3;
    double norm0   = (A1 * A1 + 2.0 * B1 * B1 + C1 * C1) * epsilon;
    double norm1   = (A2 * A2 + 2.0 * B2 * B2 + C2 * C2) * epsilon;

	vector<C3Point> intersections;
    for (register j = 0; j < yRoots.size(); j++) {
        CPolynomial xPolynomial(F1 + yRoots[j] * (E1 + yRoots[j] * C1) ,D1 + yRoots[j] * B1, A1);

		vector<double> xRoots;
        xPolynomial.getRoots(xRoots);

        for (register i = 0; i < xRoots.size(); i++) {
            double test = (A1 * xRoots[i] + B1 * yRoots[j] + D1) * xRoots[i] +
						  (C1 * yRoots[j] + E1) * yRoots[j] + F1;

            if (fabs(test) < norm0) {
                test = (A2 * xRoots[i] + B2 * yRoots[j] + D2) * xRoots[i] +
					   (C2 * yRoots[j] + E2) * yRoots[j] + F2;

                if (fabs(test) < norm1) {
					bool found = false;
					C3Point pt(xRoots[i], yRoots[j], 0.0);
					for (register k = 0; k < intersections.size(); k++) {
						if (pt == intersections[k]) {
							found = true;
							break;
						}
					}
					if (!found)
						intersections.push_back(pt);
				}
            }
        }
    }
	return intersections;
}



/*******************************************************************************/
// Find all intersections and overlaps
RC CEllipArcEllipArc::Intersect(
	CRealArray & cList1,  // In/Out: Intersection parameters on first curve
	CRealArray & cList2,  // In/Out: Intersection parameters on second curve
	double rTolerance)		// Optional: Tolerance
{
	RC rc = SUCCESS;
	double r, s, d;
	int i, n;
	double t[2];
	BOOL bSaved = m_bSwap;

	// Initial arrays because they may not be empty
	cList1.RemoveAll();
	cList2.RemoveAll();

    CEllipArc curve1(*First()), curve2(*Second());
	BOOL bCurve1IsCircle = curve1.IsCircular(r);
	BOOL bCurve2IsCircle = curve2.IsCircular(s);
	if (!bCurve1IsCircle || !bCurve2IsCircle) { // One of curves is ellipse
		if (!bCurve1IsCircle && !bCurve2IsCircle) {		// both are ellipse
            if (curve1.MajorAxis().Norm() < curve1.MinorAxis().Norm())
                curve1.SwapAxes();

            if (curve2.MajorAxis().Norm() < curve2.MinorAxis().Norm())
                curve2.SwapAxes();

            if (!curve1.Is2DCurve() || !curve1.Is2DCurve()) {

                C3Point ptCenter = curve1.Center();
                C3Point ptMajor = curve1.MajorAxis();
                C3Point ptMinor = curve1.MinorAxis();

			    CAffine tr(&ptCenter, &ptMajor, &ptMinor, NULL), iTransf;
			    iTransf.SetInverse(tr);

                CEllipArc* pCrv2Copy = new CEllipArc(curve2);
                pCrv2Copy->Transform(iTransf);
                if (pCrv2Copy->IsOnXYPlane()) {
                    CEllipArc* pCrv1Copy = new CEllipArc(curve1);
                    pCrv1Copy->Transform(iTransf);

                    CEllipArcEllipArc crvPair(pCrv1Copy, pCrv2Copy, m_bSwap);
                    rc = crvPair.CCurvePair::Intersect(cList1, cList2, rTolerance);

                    delete pCrv1Copy;
                    pCrv1Copy = NULL;
                }
                else
			        rc = CCurvePair::Intersect(cList1, cList2, rTolerance);

                delete pCrv2Copy;
                pCrv2Copy = NULL;
            }
            else
                rc = CCurvePair::Intersect(cList1, cList2, rTolerance);

			m_bSwap = bSaved;
		}
		else { // one of curves is circle
			BOOL isClosed = FALSE;
			double param, sParam, eParam, dis;
			CAffine *pTransf = NULL, iTransf;
			C3Point xAxis(1.0, 0.0, 0.0), dMaj, dMin;
			C3Point ptCenter, ptMajor, ptMinor, ptStart, ptEnd, point, ptNearest;
			if (!bCurve1IsCircle) {	// the first curve is ellipse and second one is cicle
                if (curve1.MajorAxis().Norm() < curve1.MinorAxis().Norm())
                    curve1.SwapAxes();

				dMaj = curve1.MajorAxis() / curve1.MajorAxis().Norm();
				if (!FEQUAL(dMaj * xAxis, 1.0, FUZZ_DIST)) {
					ptCenter = curve1.Center();
					dMin = curve1.MinorAxis() / curve1.MinorAxis().Norm();

					pTransf = new CAffine(&ptCenter, &dMaj, &dMin, NULL);
					iTransf.SetInverse(*pTransf);

					curve1.Transform(iTransf);
					curve2.Transform(iTransf);
				}
				isClosed = curve2.Closed();
				if (!isClosed) {
					curve2.GetStartPoint(ptStart);
					curve2.GetEndPoint(ptEnd);
				}
				CEllipArc circle(curve2.Center(), C3Point(s, 0.0, 0.0), C3Point(0.0, s, 0.0), 0, TWOPI);
				if (!isClosed) {
					circle.PullBack(ptStart, ptNearest, sParam, dis);
					circle.PullBack(ptEnd, ptNearest, eParam, dis);
					circle.SetDomain(sParam, (eParam > sParam) ? eParam : eParam + TWOPI);
				}
				curve2 = circle;
			}
			else { // the first one is circle and the second one is ellipse
                if (curve2.MajorAxis().Norm() < curve2.MinorAxis().Norm())
                    curve2.SwapAxes();

				dMaj = curve2.MajorAxis() / curve2.MajorAxis().Norm();
				if (!FEQUAL(dMaj * xAxis, 1.0, FUZZ_DIST)) {
					ptCenter = curve2.Center();
					dMin = curve2.MinorAxis() / curve2.MinorAxis().Norm();

					pTransf = new CAffine(&ptCenter, &dMaj, &dMin, NULL);
					iTransf.SetInverse(*pTransf);

					curve1.Transform(iTransf);
					curve2.Transform(iTransf);
				}
				isClosed = curve1.Closed();
				if (!isClosed) {
					curve1.GetStartPoint(ptStart);
					curve1.GetEndPoint(ptEnd);
				}
				CEllipArc circle(curve1.Center(), C3Point(r, 0.0, 0.0), C3Point(0.0, r, 0.0), 0, TWOPI);
				if (!isClosed) {
					circle.PullBack(ptStart, ptNearest, sParam, dis);
					circle.PullBack(ptEnd, ptNearest, eParam, dis);
					circle.SetDomain(sParam, (eParam > sParam) ? eParam : eParam + TWOPI);
				}
				curve1 = circle;
			}
			vector<C3Point> intersections =
							compute2DEllipseEllipseIntersections(curve1.Center().X(),
												curve1.Center().Y(), fabs(curve1.MajorAxis().X()),
												fabs(curve1.MinorAxis().Y()), curve2.Center().X(),
												curve2.Center().Y(), fabs(curve2.MajorAxis().X()),
												fabs(curve2.MinorAxis().Y()));

			curve1 = *First();
			curve2 = *Second();

			// Transform intersection points back to the model space and then
            // convert find corresponding parameter space points.
			//
			for (register i = 0; i < intersections.size(); i++) {
				if (pTransf)
					pTransf->ApplyToPoint(intersections[i], point);
				else
					point = intersections[i];

				curve1.PullBack(point, ptNearest, param, dis);
				if (param >= curve1.Start() && param <= curve1.End())
					cList1.Add(param);

				curve2.PullBack(point, ptNearest, param, dis);
				if (param >= curve2.Start() && param <= curve2.End())
					cList2.Add(param);
			}
			cList1.Sort();
			cList2.Sort();

			if (pTransf) {
				delete pTransf;
				pTransf = NULL;
			}
		}
	}
	else { // both are circle
		PEllipArc pSmaller = First();
		PEllipArc pBigger = Second();
		// The arcs are circular, Use standard trigonometry
		d = PointDistance(Second()->Center(), First()->Center());
		if (FZERO(d, FUZZ_DIST))  // The centers coincide
			QUIT

	/*
	 * The intersection points lie on both sides of the line segment connecting
	 * the 2 centers, at equal angles a.  This angle is between the sides of lengths
	 * d and r in a triangle whose 3rd side is of length s. The cosine theorem
	 * says s^2 = r^2 + d^2 - 2rdcos a, so a = acos((r^2+d^2-s^2)/2rd).  Here we
	 * reuse r for the angle a.
	 *
	 * This algoritm may get in trouble if r >> s, because then one angle is ~0,
	 * giving us inaccurate information about the other angle.  For that reason
	 * we'll do our computations with r as the lesser radius.
	 */
		if (r > s)
			{
			m_bSwap = !m_bSwap;
			SWAP(r, s, double);
			SWAP(pSmaller, pBigger, PEllipArc);
			}

		// There is not any intersection becuse circles do not touch each other - SWH, 9/12/2003
		//
		if ((d - r - s) > FUZZ_DIST || (s - d - r) > FUZZ_DIST)
			QUIT

		t[0] = r * r + d * d - s * s;
		t[1] = 2 * r * d;
		if (fabs(t[0]) > t[1]) // There is no intersection
			goto exit;
		r = t[0] / t[1];
		if (fabs(r) > 1 - FUZZ_GEN)
			{
			// The circles touch at 1 point
			n = 1;
			if (r > 0)
				r = 0;
			else
				r = PI;
			}
		else
			{
			// The circles intersect at 2 points
			r = acos(r);
			n = 2;
			}

		if (rc = pSmaller->AngleToPoint(pBigger->Center(), s))
			QUIT

		t[0] = s - r;
		t[1] = s + r;

		// Check if the intersection of the circles are within the arcs
		for (i = 0;  i < n;  i++)
			{
			C3Point P;
			if (!pSmaller->PullIntoDomain(t[i]))
				// Forget it, this parameter is outside the arc
				continue;
			// Get the point, and see if it's on the second arc
			pSmaller->Evaluate(t[i], P);
			if (rc = pBigger->AngleToPoint(P, s))
				QUIT
			if (pBigger->PullIntoDomain(s))	// We have an intersection!
				if (rc = AddIntersection(t[i], s, m_bSwap, cList1, cList2))
					QUIT
			}
exit:
		// Restore the Swap flag
		m_bSwap = bSaved;
	}

	RETURN_RC(CEllipArcEllipArc::Intersect, rc);
	}

/****************************************************************************/
// Find the relative direction ordering (left or right) at an intersection
RC RelativeDirection(
	CCurve & This,		// In: One curve
	CCurve & Other,		// In: The other curve
	double rOnThis,     // In: Parameter value on this curve
	double rOnOther,    // In: Parameter value on the other curve
	int & nSide)		// Out: 1 for left, -1 for right
/*
 * This is a low-level utility, it assumes without checking that the curves
 * intersect at the given parameter values. It determines whether Other emerges
 * out of the intersection to the left or to the right of this curve, when
 * looking in the curves' flow direction
 */
	{
	RC rc = FAILURE;
	double r = 0;

		// Try the first derivative
 	C3Point A, B, U, V;
	if (rc = This.Evaluate(rOnThis,A, U))
		goto exit;
	if (rc = Other.Evaluate(rOnOther, B, V))
		goto exit;
	r = Determinant(U, V);
	if (FZERO(r, FUZZ_GEN)) // No good, the tangents are parallel.
		{
		/* Go by the curvatures. If U and V point in the same direction then
		 * turning left means going out at a positive curvature.  If they
		 * point at opposite direction then we have to compare curvatures
		 */
		if (rc = Other.Evaluate(rOnOther, A, U, r))
			goto exit;
		if (U * V < 0)
			{
			double s;
			if (rc = This.Evaluate(rOnThis, B, V, s))
				goto exit;
			r = s - r;
			}
		}
exit:
	 // What have we got?
	 if (rc)
	 	nSide = 0;
	 else
	 	if (r>0)
	 		nSide = 1;
	 	else
	 		nSide = -1;

	 RETURN_RC(RelativeDirection, rc);
	 }

