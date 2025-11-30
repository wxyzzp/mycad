/* BOOLEAN.CPP - Implements the classes CBoolean, CFragment, CCombine, CSubtract,
 * BooleanPath, CPathList and CBooleanCurve.  The carry out the Boolean operations.
 * Copyright (C) 1994-98 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * See BOOLEAN.H for the definition of the classes.
 *
 * CBoolean support Union and Intersect.  For the other boolean operations it has
 * derived classes: CSubtract, CFragment, and CCombine.
 * Algorithms have much in common, but there are some differences.
 *
 * HOW TO OPERATE
 *	Instantiate the appropriate class, depending on the operation
 *	For every operand (shape)
 *		{
 *		For every path in the shape
 *			{
 *			Call NewPathIn
 *			For every curve in the path
 *				Call AcceptCurve
 *			Call EndPathIn
 *			}
 *		Call Operate
 *		}
 *	Call WrapUp
 *
 * You have to supply a pointer to the client whow will receive the
 * resulting shapes.  The client must be derived from CStorage.  Its
 * NewShape, NewPath, AcceptCurve, WrapUpPath and WrapUpShape methods
 * will be invokedby the CBoolean with the results of the operation.
 *
 * Some attributes are passed backwords and forword.  The operation care
 * little about the IDs, they are maintained for the client to help
 * resulting shapes refer to their progenitors. Others are determined by
 * the operation. For example, text is to be skipped in Fragment, and
 * fill in Trim and Join.  These attributes can be obtained by the client
 * by casting the returned pointer, and querying it. AddPath returns a
 * pointer to a CBooleanPath, which can be queried for its ID, and its
 * Fillable attributes.  WrapUpShape returns a pointer to a CBoolGenes
 * , which can be queried for ID, SkipText and SkipFill. For all
 * operations except Trim, this ID is the one set by the client by
 * invoking SetID - the primary selection in Visio. In Trim the ID in
 * the genes is the ID of the shape that this shape was split from,
 * as passed by the Operate call. Path IDs matter nothing to Visio,
 * but IntelliCAD needs them for BHatch entities to refer to the
 * entities they are asssociated with.
 *
 */

// Standard includes
#include "Geometry.h"
#include <float.h>     // for DBL_MAX
#include "TheUser.h"
#include "Extents.h"
#include "Trnsform.h"
#include "Point.h"
#include "Curve.h"
#include "Line.h"
#include "CrvPair.h"
#include "Path.h"
#include "Boolean.h"

// Internal constants
#define OVERLAP_SAME 1		  // The values 1 and -1 are important,
#define OVERLAP_OPPOSITE -1	  // the algorithm relies on them

#define NO_SPLIT		0
#define START_SPLIT		1
#define END_SPLIT		2
#define INTERIOR_SPLIT	3
#define SPLIT_OPEN		4

#define ITERATIONS_LIMIT 50000

DB_SOURCE_NAME;

///////////////////////////////////////////////////////////////
// Utilities

// The direction of a curve at a given point relative to a given direction
LOCAL RC RelativeDirection(
	PCurve pCurve,
	double rAt,						// In: Parameter value on the curve
	const C3Point & ptRelativeTo,	// In: The given direction
	int & nSide)					// Out: 1 for left, -1 for right
	{
	C3Point ptP, ptTangent;
	double t;
	RC rc;

	if (rc = pCurve->Evaluate(rAt, ptP, ptTangent))
		goto exit;
	t = Determinant(ptTangent, ptRelativeTo);

	if (FZERO(t, FUZZ_GEN))   // Cannot safely determine direction
		{
		rc = E_NOMSG;
		goto exit;
		}

	if (t > 0)
		nSide = 1;
	else
		nSide = -1;
exit:
	RETURN_RC(RelativeDirection, rc);
	}
/**********************************************************************/
// Snap, if possible within geometric tolerance, to a given parameter list
LOCAL void SearchInList(
	PCurve pCurve,			 // In: A curve
	const CRealArray & cTo,	 // In: List of parameter values to snap to
	double r,	        	 // In: Parameter value to snap
	double rTolerance,    	 // In: Geometric Tolerance
	int& nLocation,			 // Out: Location in the list
	int& nSnap)				 // Out: enrty r can be snapped to, -1 if it can't
	{
// nLocation is the index of r if it were inserted or found in the list
	nSnap = -1;
	nLocation = 0;

	if (!cTo.Size())
		return;

	// Search for the location in the list
	nLocation = cTo.Search(r) + 1;

	// Try to snap it
	if (nLocation > 0)
		{
		if (pCurve->SnapParameter(cTo[nLocation - 1], rTolerance, r))
			// Snapped down
			nSnap = (--nLocation);
		}
	else if (nLocation < cTo.Size())
		if (pCurve->SnapParameter(cTo[nLocation], rTolerance, r))
			// Snapped up
			nSnap = nLocation;
	}
/*******************************************************************************/
// Get the necessary 3 tangent directions needed for the following 2 methods
LOCAL RC Get3Directions(
	PCurve c,			// In: A curve
	PCurve c1,			// In: An existing valid left turn
	PCurve c2,			// In: A candidate to replace it
	C3Point& ptThis,	// Out: Unit tangent at the end of this curve
	C3Point& ptT1,		// Out: Unit tangent at the start of c1
	C3Point& ptT2) 		// Out: Unit tangent at the start of c2
	{
	RC rc = SUCCESS;
	C3Point P;

	// Get all 3 tangents
	if (rc = c->Evaluate(c->End(), P, ptThis))
		QUIT
	if (rc = P.Unitize())
		goto exit;	// No fuss
	if (rc = c1->Evaluate(c1->Start(), P, ptT1))
		QUIT
	if (rc = ptT1.Unitize())
		goto exit;	// No fuss
	if (rc = c2->Evaluate(c2->Start(), P, ptT2))
		QUIT
	rc = ptT2.Unitize();
exit:
	return rc;
	}
/*******************************************************************************/
// Choose the leftmost turn between the current and the contender
BOOL SharperLeftTurn( // Return TRUE if the candidate is a sharper left turn
	PCurve pCurve,		 // In: A curve
	PCurve pCurrent,     // In: An existing valid left turn
	PCurve pContender)   // In: A contender to replace it
	{
	RC rc = SUCCESS;
	BOOL bReplace = FALSE;
	C3Point ptThis, ptT1, ptT2;
	double rAngle1, rAngle2, rCurvature1, rCurvature2;

	// Get all 3 tangent directions
	if (rc = Get3Directions(pCurve, pCurrent, pContender, ptThis, ptT1, ptT2))
		QUIT

	// Get the angles between this direction and the candidate's directions
	if (rc = Angle(ptThis, ptT1, rAngle1))
		QUIT
	if (FEQUAL(rAngle1, -PI, FUZZ_DEG))
		rAngle1 = PI;
	if (rc = Angle(ptThis, ptT2, rAngle2))
		QUIT
 	if (FEQUAL(rAngle2, -PI, FUZZ_DEG))
		rAngle2 = PI;

	// compare angles
	if (!FEQUAL(rAngle1, rAngle2, FUZZ_DEG))
		bReplace = (rAngle2 > rAngle1);
	else
		{
		// Tangent directions coincide, compare curvatures
		if (rc = pCurrent->GetCurvature(pCurrent->Start(), rCurvature1))
			QUIT
		if (rc = pContender->GetCurvature(pContender->Start(), rCurvature2))
			QUIT
		bReplace = (rCurvature2 > rCurvature1);
		}

	if (bReplace  &&  FEQUAL(rAngle2, PI, FUZZ_DEG))
		{
		// Careful, is the candidate really turning left?
		if (rc = pCurve->GetCurvature(pCurve->End(), rCurvature1))
			QUIT
		if (rc = pContender->GetCurvature(pContender->Start(), rCurvature2))
			QUIT
		// If not, the candidate unacceptible
		bReplace = (rCurvature2 < -rCurvature1);
		}
exit:
	return bReplace;
	}
/*******************************************************************************/
// Choose the least turning curve
LOCAL RC LeastTurn(
	PCurve pCurve,		// In: A curve
	PCurve pCurrent,	// In: An existing valid left turn
	PCurve pCandidate,	// In: A candidate to replace it
	BOOL & bReplace)	// Out: =TRUE if the choice is the second
	{
	RC rc = SUCCESS;
	bReplace = FALSE;
	C3Point ptThis, ptT1, ptT2;
	double rCos1, rCos2;

	// Get all 3 tangent directions
	if (Get3Directions(pCurve, pCurrent, pCandidate, ptThis, ptT1, ptT2))
		goto exit;

	// Get the cosines of the angles
	if (rc = CosAngle(ptThis, ptT1, rCos1))
		QUIT
	if (rc = CosAngle(ptThis, ptT2, rCos2))
		QUIT

	// compare angles
	if (!FEQUAL(rCos1, rCos2, FUZZ_DEG))
		bReplace = (rCos2 > rCos1);
	else
		{
		// Tangent directions coincide, prefer the closest curvature
		double rThisCurvature, rCurvature1, rCurvature2;

		if (rc = pCurve->GetCurvature(pCurve->End(), rThisCurvature))
			QUIT
		if (rc = pCurrent->GetCurvature(pCurrent->Start(), rCurvature1))
			QUIT
		if (rc = pCandidate->GetCurvature(pCandidate->Start(), rCurvature2))
			QUIT
		bReplace = (fabs(rCurvature2 - rThisCurvature) <
		            fabs(rCurvature1 - rThisCurvature));
		}
exit:
	RETURN_RC(LeastTurn, rc);
	}
/////////////////////////////////////////////////////////////////////////////////
// Implementation of CBooleanCurve
// Constructor/destructor
CBooleanCurve::CBooleanCurve()
	{
	m_pCurve = NULL;
	m_id = INV_ID;
	}

CBooleanCurve::CBooleanCurve(
	PCurve pCurve,		// In: The curve
	double rTolerance,	// In: Tolerance
	GeoID id)			// In: Reference ID
	{
	m_pCurve = pCurve;
	m_pCurve->UpdateExtents(FALSE, FALSE, m_cExtents);
	m_cExtents.Inflate(rTolerance);
	m_id = id;
	}

CBooleanCurve::~CBooleanCurve()
	{
	delete m_pCurve;
	}
/**********************************************************************************/
// Update the list of intersections
void CBooleanCurve::UpdateIntersections()
	{
	/*
	 * Splitting a closed curve may modify its domain, to make it start at
	 * the split.  After that, the intersections need to be pulled into
	 * the new domain interval.
	 */
	double rInterval = End() - Start();
	for (int i = 0;   i < m_cIntersections.Size();   i++)
		{
		while (m_cIntersections[i] < Start())
			m_cIntersections[i] += rInterval;
		while (m_cIntersections[i] > End())
			m_cIntersections[i] -= rInterval;
		}
	}
/*****************************************************************************/
// Split
RC CBooleanCurve::GetNewCurve(
	double rAt,     // In: Paramter value to split at
	PCurve & pNew,	// Out: The new curve segment
	int& nStatus,	// Out: Status
	BOOL& bClosed)	// In/Out: =TRUE if this is a one-curve closed path
    {
	RC rc = SUCCESS;
	double t = 100 * FUZZ_GEN;
	pNew = NULL;

	if (bClosed)// A closed curve being split the first time
		{
		nStatus = SPLIT_OPEN;
		double r = rAt + End() - Start();
		m_pCurve->SetDomain(rAt, r);
		}
	else if (rAt < Start() + t)	// Too close to the beginning
		nStatus = START_SPLIT;
	else if (rAt > End() - t)  // Too close to the end
		nStatus = END_SPLIT;
	else	// It's a true split
	    {
		nStatus = INTERIOR_SPLIT;
		// Clone the curve and split the domain
		if (rc = ALLOC_FAIL(pNew = m_pCurve->Clone()))
			QUIT
		if (rc = m_pCurve->ResetEnd(rAt))
			QUIT
		if (rc = pNew->ResetStart(rAt))
			QUIT
 	    }
	bClosed = FALSE;
exit:
	RETURN_RC(CBooleanCurve::GetNewCurve, rc);
    }

/**********************************************************************************/
// Split this path at a given curve
RC CBooleanCurve::Split(
	int nCurveIndex,      // In: Index of this curve in the path
	int nLast,            // In: Index of the path's last curve
	double rTolerance,	  // In: Point-distance tolerance
	int & nSplitType,	  // Out: Type of split, if any
	PBooleanCurve & pNew, // Out: The new curve splinter
	BOOL & bClosedPath)   // In/Out: FALSE if this is a closed path
	{
	RC rc = SUCCESS;
	PCurve pCurve = NULL;
	nSplitType = NO_SPLIT;
	BOOL bClosedCurve = bClosedPath && nLast == 0 && m_pCurve->Closed();

/*
 Loop over the list of intersections, moving backwords in the curve
 domain, and breaking if there is a split. or if we reached the
 domain start.
 */

	while (m_cIntersections.Size())
		{
		nSplitType = NO_SPLIT;
		double rAt = m_cIntersections.Pop();

		// Get a new curve and see the status
		if (rc = GetNewCurve(rAt, pCurve, nSplitType, bClosedCurve))
			QUIT

		switch (nSplitType)
			{
			case SPLIT_OPEN: // Closed curve split the first time
				UpdateIntersections();
				bClosedPath = FALSE;
				nSplitType = NO_SPLIT;
				continue;

			case END_SPLIT:
				if (nCurveIndex == nLast)
					{
					// No splitting at the end of a path
					nSplitType = NO_SPLIT;
					continue;
					}
				else
					// Split between this curve and the next one
					goto exit;

			case START_SPLIT:
				if (nCurveIndex == 0)
					// No splitting at the beginning of a path
					nSplitType = NO_SPLIT;
				// Otherwise
					// split between this curve and the previous one
				goto exit;

			case INTERIOR_SPLIT:  // This curve really split
				// Create a new BooleanCurve record
				pNew = new CBooleanCurve(pCurve, rTolerance, m_id);
				rc = ALLOC_FAIL(pNew);
				goto exit;
			} // End switch
		} // End while
exit:
	RETURN_RC(CBooleanCurve::Split, rc);
	}
/**********************************************************************************/
// Cast a ray in a good direction from a safe point on the curve
RC CBooleanCurve::CastRay(
	PRay& pRay)       // Out: The ray
	{
	double t;
	C3Point ptPoint, ptDirection;
	RC rc;
	pRay = NULL;

	// Get a random point on the curve
	if (rc = m_pCurve->RandomPoint(t, ptPoint, ptDirection))
		QUIT

	//Cast a ray to the right of the curve's direction
	ptDirection.RandomTurnLeft();
	rc = ALLOC_FAIL(pRay = new CRay(ptPoint, ptDirection));
exit:
	if (rc)
		{
		delete pRay;
		pRay = NULL;
		}
	RETURN_RC(CBooleanCurve::CastRay, rc);
	}
#if defined DEBUG
/**********************************************************************************/
// Dump
void CBooleanCurve::Dump()
	{
	C3Point P;
	DB_MSGX("BooleanCurve %p", (this));
	m_pCurve->Dump();
	DB_MSGX("Endpoints:", (Start(), End()));
	m_pCurve->GetStartPoint(P);
	P.Dump();
	m_pCurve->GetEndPoint(P);
	P.Dump();
	DB_MSGX("Intersections:",());
	m_cIntersections.Dump();
	}
#endif
/////////////////////////////////////////////////////////////////////////////////
// Implementation of CBooleanPath
// Constructors/destructor
CBooleanPath::CBooleanPath()
	{
	m_nStatus = UNKNOWN;
	m_pTwin = m_pNext = m_pPrevious = NULL;
	m_pEndJunction = m_pStartJunction = NULL;
	m_bFillable = FALSE;
	m_bClosed = FALSE;
	m_id = INV_ID;
	}
/**********************************************************************************/
// Construct from a Visio path
CBooleanPath::CBooleanPath(
	BOOL bClosed,	// In: It's going to be a closed path if TRUE
	BOOL bFillable,	// In: Make it fillable if TRUE
	GeoID id)		// In: Reference ID
	{
	m_nStatus = UNKNOWN;
	m_pTwin = m_pNext = m_pPrevious = NULL;
	m_pEndJunction = m_pStartJunction = NULL;
	m_bClosed = bClosed;
	m_bFillable = bFillable;
	m_id = id;
	}
/**********************************************************************************/
// Copy constructor
CBooleanPath::CBooleanPath(
	PBooleanPath pOther)  // In: The other path this was carved from
	{
	m_nStatus = UNKNOWN;
	m_pTwin = m_pNext = m_pPrevious = NULL;
	m_pEndJunction = m_pStartJunction = NULL;
	m_bClosed = pOther->m_bClosed;
	m_bFillable = pOther->m_bFillable;
	m_id = pOther->m_id;
	}

CBooleanPath::~CBooleanPath()
	{
	Unlink();
	}
/**********************************************************************************/
// Accept a curve into the current path
RC CBooleanPath::AcceptCurve(
	PCurve pCurve,		// In: The curve
	double rTolerance,	// In: Point-distance tolerance
	GeoID id)			// In: Reference ID
	{
	PBooleanCurve p = NULL;
	RC rc = SUCCESS;

	// Avoid degenerate curves
	if (pCurve->Degenerate())
		goto exit;

	// Add it to the list
	if (rc = ALLOC_FAIL(p = new CBooleanCurve(pCurve, rTolerance, id)))
		QUIT
	if (rc = Add(p))
		QUIT
	m_cExtents.Update(p->m_cExtents);
	p = NULL;
exit:
	delete p;
	RETURN_RC(CBooleanPath::AcceptCurve, rc);
	}
/**********************************************************************************/
// Get the intersection of this path with the other path
RC CBooleanPath::GetIntersections(
	PBooleanPath pOtherPath,  // In: Other list of curves
	double rTolerance)        // In: Geometric tolerance
// Store intersection parameter values with the curves
	{
	RC rc = SUCCESS;
	int i,j,k,kFrom;
	int nThisLast = Size() - 1;
	int nOtherLast = pOtherPath->Size() - 1;

	// Loop on the list to get all intersections
	for (j = 0;   j <= nThisLast;   j++)
		{
		PBooleanCurve pThis = Curve(j);

		if (pOtherPath == this)
			{
			CRealArray rOnThis;
			// Get the self-intersections of this curve
			if (rc = pThis->m_pCurve->SelfIntersect(rOnThis, rTolerance))
				QUIT
			for (i=0;  i<rOnThis.Size();  i++)
				{
				int nAt, nSnap;
				double s = rOnThis[i];

				// Snap to endpoints
				pThis->m_pCurve->SnapParameter(pThis->Start(), rTolerance, s);
				pThis->m_pCurve->SnapParameter(pThis->End(), rTolerance, s);

				SearchInList(pThis->m_pCurve,pThis->m_cIntersections,
							 s, rTolerance, nAt, nSnap);
				if (nSnap < 0) // s was not found in the list
					if (rc = pThis->m_cIntersections.Insert(s, nAt))
						QUIT
				} // for i
			// and continue from this curve onwards in the list
			kFrom = j + 1;
			}
		else
			// Start from the very beginning of the other list
			kFrom = 0;

		// Mutual intersections
		for (k = kFrom;  k <= nOtherLast;  k++)
			{
			PBooleanCurve pOther = pOtherPath->Curve(k);
			CRealArray rOnThis, rOnOther;
			if (!pThis->m_cExtents.Overlap(pOther->m_cExtents))
				continue;
			if (rc = Intersect(pThis->m_pCurve, pOther->m_pCurve,
			                   rOnThis, rOnOther, rTolerance))
				QUIT

			// Record the intersections
			for (i=0;  i<rOnThis.Size();  i++)
				{
				int nAt, nSnap;
				double s = rOnThis[i];
				double t = rOnOther[i];

				// Snap to endpoints
				pThis->m_pCurve->SnapParameter(pThis->Start(), rTolerance, s);
				pThis->m_pCurve->SnapParameter(pThis->End(), rTolerance, s);
				pOther->m_pCurve->SnapParameter(pOther->Start(), rTolerance, t);
				pOther->m_pCurve->SnapParameter(pOther->End(), rTolerance, t);

				// Avoid insignificant intersections
				if (pOtherPath == this  &&
					k == j + 1  &&
					s == pThis->End() &&
					t == pOther->Start())
				    // This curve's tail with the head of the next one in the path
					continue;

				SearchInList(pThis->m_pCurve,pThis->m_cIntersections,
							 s, rTolerance, nAt, nSnap);
				if (nSnap < 0) // s was not found in the list
					if (rc = pThis->m_cIntersections.Insert(s, nAt))
						QUIT
				SearchInList(pOther->m_pCurve, pOther->m_cIntersections,
							 t, rTolerance, nAt, nSnap);
				if (nSnap < 0) // t was not found in the list
					if (rc = pOther->m_cIntersections.Insert(t, nAt))
						QUIT
				} // for i
			} // for k
		} // for j
exit:
	RETURN_RC(CBooleanPath::GetIntersections, rc);
	}
/**********************************************************************************/
// Split a path's curve-list at the first curve intersection
RC CBooleanPath::Split(
	double rTolerance,		  // In: Tolerance
	PBooleanPath  & pNewPath) // Out: The new splinter
	{
	PBooleanCurve pNewCurve;
	RC rc = E_NOMSG;
	pNewPath = NULL;
	int j;
	int nSplit = NO_SPLIT;

	// Loop over the curves until one splits
	for (j = Size() - 1;    j >= 0;   j--)
		{
		PBooleanCurve pCurrent = Curve(j);

		// We split only once
		if (rc = pCurrent->Split(j, Size() - 1, rTolerance, nSplit,
											 pNewCurve, m_bClosed))
			break;

		if (nSplit == NO_SPLIT)
			continue;

		// Start a new path with the new curve splinter, if there is one
		if (rc = ALLOC_FAIL(pNewPath = new CBooleanPath(this)))
			QUIT
		if (nSplit == INTERIOR_SPLIT)
			{
			ASSERT (pNewCurve);		   // There has to be a new curve
			if (rc = pNewPath->Add(pNewCurve))
				QUIT
			}

		// Move the tail of the curve list to the new path
		int nTail = m_cCurves.Size() - j - 1;
		int nCutAt = j + 1;
		if (nSplit == START_SPLIT)
			{
			nTail++;
			nCutAt--;
			}
		if (nTail)
			{
			if (rc = pNewPath->m_cCurves.Copy(m_cCurves, nCutAt, nTail))
				QUIT
			m_cCurves.Remove(nCutAt, nTail);
			}

		if (m_bClosed)
			{
			// The original path was closed, just split it open
			if (nCutAt)
				{
				// Append the head of this path to the new path
				if (rc = pNewPath->m_cCurves.Copy(m_cCurves, 0, nCutAt))
					QUIT
				m_cCurves.RemoveAll();
				}

			// Let this path assume the new path, and then forget it
			rc = m_cCurves.Copy(pNewPath->m_cCurves, 0, pNewPath->Size());
			pNewPath->m_cCurves.RemoveAll();
			delete pNewPath;
			pNewPath = NULL;
			if (rc)
				QUIT;
			}
		else
			m_bFillable = FALSE;

		// In any case, it's no longer closed
		m_bClosed = FALSE;

		// Update the extents of the old and the new path
		if (pNewPath)
			pNewPath->UpdateExtents(rTolerance);
		UpdateExtents(rTolerance);
		break;
		}
	if (nSplit == NO_SPLIT)
		rc = E_NOMSG;
exit:
	RETURN_RC(CBooleanPath::Split, rc);
	}
/**********************************************************************************/
// Orient a path w.r.t. the shape it belongs to
RC CBooleanPath::Orient(
	CPathList& cShape)  //In: List of all paths in this shape
	{
/*
 * Orientation is computed by casting a ray from one of the path's curves to the
 * right, and checking the parity of the number of its intersections with the other
 * paths of this shape.
 */
	PRay pRay = NULL;
	RC rc = FAILURE;
	int nIntercepts = 0;
	int nOverlaps = 0;
	int i,j;

	// Try from all curves
	for (j = 0;   rc  &&  j < Size();  j++)
		{
		nIntercepts = 0;
		nOverlaps = 0;
		PBooleanCurve pCurve = Curve(j);
		// Cast a ray to the left
		if (rc = pCurve->CastRay(pRay))
			continue;

		// Loop on shape's paths
		for (i = 0;   i < cShape.Size();    i++)
			{
			int n;
			PBooleanPath pPath = cShape.Path(i);
			// Get the intersections of the current path with the ray
			if (rc = pPath->Intercept(pRay, TRUE, nIntercepts, n))
				break;
			if (pPath == this)
			/*
			 * This is not an overlap, it's just the calling path going through
			 * the ray's origin, which is not surprising
			 */
				n = 0;

			if (n)
				// The other path overlappping this one is redundant
				cShape.Path(i)->m_nStatus = REDUNDANT;
			nOverlaps += n;
			}
		delete pRay;
		pRay = NULL;
		}
	// Redundant path, representing an even number of overlapping paths
	if (ODD(nOverlaps))
		m_nStatus = REDUNDANT;
	else
		if (EVEN(nIntercepts))
			// Inside's on the right, this paths goes in the wrong direction
			Reverse();

	RETURN_RC(CBooleanPath::Orient, rc);
	}
/**********************************************************************************/
// Classify this path's position relative to the other shape
RC CBooleanPath::Classify(
	CPathList  & cShape) //In: Other shape's list of paths
	{
	PBooleanCurve pCurrent;
	PRay pRay;
	RC rc = FAILURE;
	int i;
	int nIntercepts = 0;

	// Try from all curves, until successful
	for (i=0;  rc && i<Size();  i++)
		{
		pCurrent = Curve(i);
		if (!(rc = pCurrent->CastRay(pRay)))
			rc = cShape.UpdateInOrOut(pRay, m_nStatus, nIntercepts);
		delete pRay;
		}

	if (rc)	   // All attempts to classify have failed
		QUIT

	if (m_nStatus) // The status has already been set as OVERLAP_XXX
		goto exit;

	if ODD(nIntercepts)
		m_nStatus = INSIDE;
	else
		m_nStatus = OUTSIDE;
exit:
	RETURN_RC(CBooleanPath::Classify, rc);
	}
/**********************************************************************************/
// Update number of intercepts of this path by a ray
RC CBooleanPath::Intercept(
	PRay pRay,              // In: The ray
	BOOL bCountOverlaps,    // In: = TRUE if overlaps are to be counted
	int  & nIntercepts,     // In/Out: Number of intercepts so far
	int  & nOverlaps)		// Out: Overlaps indicator
/*
 * This method is used in 2 modes, one to determine the orientation of a path
 * within its shape, and the other to test containment within another shape.
 * In the first mode it counts ALL the overlaps, in the second it quits after
 * any overlap is encountered, but then it indicates the direction of the
 * overlapping path relative to the ray direction.  It returns -1 if the
 * curve crosses to the right, and 1 otherwise.  If the direction is too close
 * to the ray's direction the operation is considered a FAILURE.
 *
 * An overlap is a path that goes through the ray's origin.
 */
	{
	RC rc = SUCCESS;
	int i,j;
	nOverlaps = 0;
	double rAt;

	for (j=0;  j<Size();  j++)
		{
		CRealArray cOnRay, cOnCurve;
		PCurve pCurve = Curve(j)->m_pCurve;

		if (rc = Intersect(pRay, pCurve, cOnRay, cOnCurve))
			break;

		// Analyze the curve intersection points
		for (i = 0;    i < cOnCurve.Size();    i++)
			{
			rAt = cOnCurve[i];
			if (FEQUAL(rAt, pCurve->Start(), FUZZ_GEN)  ||
			    FEQUAL(rAt, pCurve->End(), FUZZ_GEN))
				{
				// Vertex intercepts are not acceptable
				rc = E_NOMSG;
				goto exit;
				}
			}

		// Analyze the ray intercepts
		for (i = 0;    i < cOnRay.Size();    i++)
			{
			rAt = cOnRay[i];
			if (FZERO(rAt, FUZZ_GEN))  // It's an overlap
				if (bCountOverlaps)
					nOverlaps++;
				else
					{ // Determine crossing-direction and quit
					rc = RelativeDirection(pCurve, rAt, pRay->Direction(), nOverlaps);
					break;
					}
			else					  // It isn't an overlap
				nIntercepts++;
			}
		}
exit:
	RETURN_RC(CBooleanPath::Intercept, rc);
	}
/**********************************************************************************/
// Reverse the path
RC CBooleanPath::Reverse()
	{
	RC rc = SUCCESS;
	// Loop on the curve-list and reverse the curves
	for (int i = 0;   i < Size();   i++)
		if (rc = Curve(i)->m_pCurve->Reverse())
			break;

	// Reverse the list
	m_cCurves.Reverse();

	RETURN_RC(CBooleanPath::Reverse, rc);
	}
/**********************************************************************************/
// Is the path bounded?
BOOL CBooleanPath::Bounded()
	{
	// Loop on the curve-list and check
	for (int i = 0;   i < Size();   i++)
		if (!Curve(i)->m_pCurve->Bounded())
			return FALSE;
	return TRUE;
	}
/**********************************************************************************/
// Update the (loose) extents of all the curves in the list and the path's extents
RC CBooleanPath::UpdateExtents(
	double rTolerance)		  // In: Tolerance
	{
	RC rc = SUCCESS;

	m_cExtents.Reset();
	for (int i=0;  i<Size();  i++)
		m_cExtents.Update(Curve(i)->m_cExtents);
	m_cExtents.Inflate(rTolerance);

	RETURN_RC(CBooleanPath::UpdateExtents, rc);
	}
/**********************************************************************************/
// Get the path's endpoints
RC CBooleanPath::GetEndPoints(
	C3Point& ptStart,    // Out: The path's first point
	C3Point& ptEnd)      // Out: The path's last point
	{
	RC rc = SUCCESS;
	PCurve pCurve;

	pCurve = Curve(0)->m_pCurve;
	if (rc = pCurve->GetStartPoint(ptStart))
		QUIT
	pCurve = LastCurve();
	rc = pCurve->GetEndPoint(ptEnd);
exit:
	RETURN_RC(CBooleanPath::GetEndPoints, rc);
	}
/**********************************************************************************/
RC CBooleanPath::Append(
	PBooleanPath pNext) // In/Out: The path to append
	{
	//The operation will gut the appended path
	RC rc = m_cCurves.Copy(pNext->m_cCurves, 0, pNext->m_cCurves.Size());
	pNext->m_cCurves.RemoveAll();
	m_pEndJunction = pNext->m_pEndJunction;
	ASSERT(!m_pNext);
	m_pNext = pNext->m_pNext;
	if (m_pNext)
		{
		ASSERT(!m_pNext->m_pPrevious);
		m_pNext->m_pPrevious = this;
		}
	RETURN_RC(CBooleanPath::Append,rc);
	}
/**********************************************************************************/
// Add all the curves in the list to a Visio path
RC CBooleanPath::AddCurvesToPath(
	BOOL bTrim,			// In: Trim the curves if TRUE
	PStorage pClient)	// In/Out: The client of the operation
	{
	RC rc = SUCCESS;

	// Loop over the list
	for (int i = 0;   i < Size();   i++)
		{
		PCurve p = Curve(i)->m_pCurve;
		if (bTrim)
			if (rc = p->ReplaceWithRestriction(p->Start(), p->End(), p))
				QUIT
		if (rc = pClient->AcceptCurve(p, Curve(i)->ID()))
			QUIT
		Curve(i)->m_pCurve = NULL; // Now owned by the client
		}

	rc = pClient->WrapUpPath();
exit:
	RETURN_RC(CBooleanPath::AddCurvesToPath, rc);
	}
#if defined DEBUG
/**********************************************************************************/
// Dump
void CBooleanPath::Dump()
	{
	if (Size())
		{
		C3Point P,Q;
		DB_MSGX("BooleanPath with endpoints:",());
		GetEndPoints(P,Q);
		P.Dump();
		Q.Dump();
		}
	if (m_pEndJunction)
		{
		DB_MSGX("m_pEndJunction=%p", (m_pEndJunction));
		m_pEndJunction->Point().Dump();
		}
	for (int i=0;  i<m_cCurves.Size();  i++)
		Curve(i)->Dump();
	DB_MSGX("Status=%d",(m_nStatus));
	}
#endif
/////////////////////////////////////////////////////////////////////////////////
// Implementation of CPathList
// Constructor/destructor
CPathList::CPathList()
	{
	m_ID = INV_ID;
	}
CPathList::~CPathList()
	{
	}
/**********************************************************************************/
// Get the intersections of this shape with the other shape
RC CPathList::GetIntersections(
	CPathList  * pOtherShape,  // In: Other shape's path-list
	double rTolerance)         // In: Geometric tolerance
// The intersections parameter values will be stored with the curves.
	{
	RC rc = SUCCESS;
	int i, j, nFrom;

	// Loop on the list of paths to get all intersections
	for (i=0;  i<Size(); i++)
		{
		PBooleanPath pThis = Path(i);

		if (pOtherShape == this) // it's the same shape
			// Start from this path onwards in the list
			nFrom = i;
		else
			// Start from the beginning of the other list
			nFrom = 0;

		// Loop on the paths of the other shape
		for (j=nFrom;  j<pOtherShape->Size();  j++)
			{
			PBooleanPath pOther = pOtherShape->Path(j);
			if (pThis->m_cExtents.Overlap(pOther->m_cExtents))
				if (rc = pThis->GetIntersections(pOther, rTolerance))
					QUIT
			}
		}
exit:
	RETURN_RC(CPathList::GetIntersections, rc);
	}
/**********************************************************************************/
// Split the paths-list at all path intersections
RC CPathList::SplitAtIntersections(
	double rTolerance)		  // In: Tolerance
	{
	RC rc = SUCCESS;
	int i;

	// Loop on the list of paths to split at intersections
	for (i = Size() - 1;     !rc  && i >= 0;     i--)
		{
		PBooleanPath pCurrent = Path(i);
		while (!rc)
			{
			PBooleanPath pNew = NULL;
			if (rc = pCurrent->Split(rTolerance, pNew))
				break;
			if (pNew)
				rc = Add(pNew);
			}
		if (rc == E_NOMSG)
			rc = SUCCESS;
		}

	RETURN_RC(CPathList::SplitAtIntersections, rc);
	}
/**********************************************************************************/
// Update the the number of intersections with a ray
RC CPathList::UpdateInOrOut(
	PRay pRay,                     // In: The ray
	int  & nStatus,                // Out: overlap indicator
	int  & nIntercepts)            // In/Out: = Number of ray intercepts
	{
	RC rc = SUCCESS;
	int i;

	nIntercepts = 0;
	for (i = 0;  i < Size();  i++)
		{
		PBooleanPath pCurrent = Path(i);

		if (rc = pCurrent->Intercept(pRay, FALSE, nIntercepts, nStatus))
			break;

		if (nStatus)  // an overlap has been encountered
			{
 			pCurrent->m_nStatus = REDUNDANT;  // One ocurrance is redundant
/*
 * If we have organized the paths corectly, this shape has no overlapping paths.
 * We can therefore expect no more than one path to overlap the calling path, so:
 */
 			break;
			}
		}
	RETURN_RC(CPathList::UpdateInOrOut, rc);
	}
 /**********************************************************************************/
// Orient the paths of a shape, merging overlapping ones
RC CPathList::Orient()
/*
 * Every path will be oriented so that the shape's interior will always be on the
 * left when going along the path. If there's an even nubers of overlapping copies
 * of the same path then they will all be later removed.
 */
	{
	int i,j;
	RC rc = SUCCESS;

	// Loop on the paths in the list and orient each one
	for (i = 0;  i < Size();   i++)
		{
		// Iterate until succeeding to orient the current path
		for (j = 1;   j<=50;   j++)
			if (!(rc = Path(i)->Orient(*this)))
				break;   // Succeeded!

		if (rc)	      // All iterations failed
			QUIT
		}

	// Remove redundant paths
	for (i = 0;  i < Size();   i++)
		if (Path(i)->m_nStatus == REDUNDANT)
			Remove(i--);
exit:
	RETURN_RC(CPathList::Orient, rc);
	}
/**********************************************************************************/
// Classify all this shape's paths' positions relative to the other shape
RC CPathList::Classify(
	CPathList  & cShape)	 // In: Other shape
// The status will be recorded with the paths
	{
	RC rc = SUCCESS;

	// Loop on the paths
	for (int i = 0;  i < Size();  i++)
		{
		PBooleanPath pCurrent = Path(i);
		if (pCurrent->Status())	 // This path's status has alreadey been determined
			continue;

		// Iterate for success
		for (int j = 1;    j <= 50;   j++)
			if (!(rc = pCurrent->Classify(cShape)))
				break;
		}
	RETURN_RC(CPathList::Classify, rc);
	}
/**********************************************************************************/
// Delete the paths that go away during the Boolean operation
RC CPathList::UnionOrIntersect(
	int nAdmit)    // In: Admissable type - INSIDE for intersect, OUTSIDE for union
	{
 	for (int i = 0;  i < Size();   i++)
		{
		PBooleanPath pCurrent = Path(i);
		if (pCurrent->m_nStatus != nAdmit && pCurrent->m_nStatus != OVERLAP_SAME)
			{
			delete pCurrent;
			Remove(i--);
			}
		}
	RETURN_RC(CPathList::UnionOrIntersect, SUCCESS);
	}
/*******************************************************************/
// Get the total area of a shape bounded by this path-list
RC CPathList::GetArea(
	double  & rArea)	  // Out: the total area
/*
 * This function assumes that this path list is the output from
 * CBoolean::Organize, otherwise the result is meaningless
 */
	{
	RC rc = SUCCESS;
	rArea = 0.0;
	int i, j;

	// Loop on curves of organized paths to sum up the area integral
	for (i = 0;    i < Size();    i++)
		{
		PBooleanPath pPath = Path(i);
		for (j = 0;   j < pPath->Size();   j++)
			if (rc = pPath->Curve(j)->m_pCurve->UpdateAreaIntegral(rArea))
				QUIT
		}
	rArea /= 2;

	// Barring approximation errors, the area should be positive
	if (rArea < 0)
		rArea = 0;
	rc = SUCCESS;
exit:
	RETURN_RC(CPathList::GetArea, rc);
	}
/**********************************************************************************/
// Reset the status of all paths to UNKNOWN
void CPathList::Reset()
	{
	for (int i=0;  i<Size();  i++)
		{
		Path(i)->m_nStatus = UNKNOWN;
		}
	}
/**********************************************************************************/
// Test the containment of a point in the shape defined by this list
BOOL CPathList::HitTest(
	C3Point & P)	// In: A point
	{
	int i, j;
	int nIntercepts = 0;
	int nOnBoundary = 0;
	if (Size() == 0)
		return FALSE;

	RC rc = FAILURE;
	// Make 4 attempts
	for (i = 1;   rc && i <= 4;   i++)
		{
		// Cast a random ray
		double r = FRAND * TWOPI;   // A random number between 0 and 2Pi
		CRay ray(P, C3Point(sin(r), cos(r)));
		nIntercepts = 0;

		// Tally its intercepts with all the boundaries
		for (j = 0;   j < Size();   j++)
			rc = Path(j)->Intercept(&ray, TRUE, nIntercepts, nOnBoundary);
		}

	if (rc  ||  nOnBoundary)
		return FALSE;

	// Interception was successful, see if we're inside
	return ODD(nIntercepts);
	}
#if defined DEBUG
/**********************************************************************************/
// Dump
void CPathList::Dump()
	{
	DB_MSGX("Path list, m_nSize=%d", (m_nSize));
	for (int i=0;  i<Size();  i++)
		{
		DB_MSGX("Path %d:    %p",(i, Path(i)));
		Path(i)->Dump();
		}
	}
#endif
/////////////////////////////////////////////////////////////////////////////////
// Implementation of CJunction
// Constructor/destructor
CJunction::CJunction()
	{
	}

CJunction::CJunction(
	const C3Point  & cPoint)	  // In: The junction point
	{
	m_cPoint = cPoint;
	}

CJunction::~CJunction()
	{
	}
/**********************************************************************************/
// Remove a path from this junction's path list
RC CJunction::Add(
	PBooleanPath pPath)  // In: The path to be added
	{
	RC rc = m_cPaths.Add(pPath);
	if (!rc)
		pPath->SetStartJunction(this);
	RETURN_RC(CJunction::Add, rc);
	}
/**********************************************************************************/
// Remove a path from this junction's path list
void CJunction::Remove(
	PBooleanPath p)	// In: The path to remove
	{
	int i;

	// For debug purposes, hopefully elimited by the optimizer
	BOOL bFound = FALSE;

	// Find p in the list and remove it
	for (i = 0;  i < m_cPaths.Size();   i++)
		if (Path(i) == p)
			{
			bFound = TRUE;
			m_cPaths.Remove(i);
			break;
			}

	// Something is wrong if p wasn't found at this junction
	ASSERT (bFound);

	p->Unlink();
	}
/**********************************************************************************/
// Find the next best path that will take us forward
BOOL CJunction::Forward(// Return TRUE if this closes a path
	PBooleanPath & pPathIn,		// In/out: The path coming in, NULL OK
	PBooleanPath & pPathOut)	// Out: The next path, NULL if none found
// If pPathOut is rlready linked backwards then we are closing a path, so
// report that as the return value.  In that case, pPathIn will be replaced
// with the path that pPathOut was linked to
	{
	BOOL bClosed = FALSE;
	pPathOut = NULL;
	int i;

	ASSERT(!pPathIn  ||  pPathIn->Size() > 0);

	// Find any acceptable candidate
	if (!pPathIn)
		{
		// We came from nowhere, so any path will do
		pPathOut = Path(0);
		goto exit;
		}

	for (i = 0;   i < Size();   i++)
		{
		if (Path(i) != pPathIn->Twin())
			{
			pPathOut = Path(i);
			break;
			}
		}
	if (!pPathOut)  // No valid path out found
	  goto exit;

	// Excluding path-in's twin, see if there is a sharper left turn
	for (i++/* carry on after the last i */;   i < Size();   i++)
		{
		PBooleanPath pPath = Path(i);
		if (pPath == pPathIn->Twin())	// It's our twin
			continue;

		// Choose between the existing path out and this new one
		if (SharperLeftTurn(pPathIn->LastCurve(),
							pPathOut->FirstCurve(),
							pPath->FirstCurve()))
			pPathOut = pPath;
		}
exit:
	if (pPathOut)  // We're moving forward!
		{
		PBooleanPath pPrevious = pPathOut->Previous();
		pPathOut->LinkTo(pPathIn);
		if (pPrevious)
			{
			bClosed = TRUE;
			pPathIn = pPrevious;
			}
		}
	return bClosed;
	}
/**********************************************************************************/
// Backtrack from a dead-end
PJunction CJunction::BackTrack( // Return back-junction or NULL
	PBooleanPath& pPath)	// In: The current path, NULL OK
	{
	PBooleanPath pPrevious = NULL;
	PJunction pBack = NULL;
	if (!pPath)
		goto exit;

	// Clean-out this junction
	m_cPaths.Purge();

	// Backtrack
	if (pPath)
		{
		pPrevious = pPath->Previous();

		// Remove the path from its junction
		pBack = pPath->StartJunction();
		pBack->Remove(pPath);
		delete pPath;
		}
exit:
	// Output
	pPath = pPrevious;
	return pBack;
	}
/**********************************************************************************/
// Extend from this junction - a CJoin specific method
RC CJunction::Extend(
	PBooleanPath pPath)	 //In: The path to be extended
	{
	RC rc = E_NOMSG;
//	Dump();
	PBooleanPath pExtension = NULL;
	PBooleanPath pContender = NULL;

	for (int i=0;   i<Size();   i++)
		if (Path(i)->Available()  &&  Path(i) != pPath->Twin())
			{
			BOOL bReplace;

			// We have a contender for the extension
			pContender = Path(i);
			if (!pExtension)
				// This is the first candidate, accept it
				pExtension = pContender;
			else
				// Choose between current extension and contender, ignoring errors
				LeastTurn(pPath->LastCurve(), pExtension->FirstCurve(),
						  pContender->FirstCurve(), bReplace);
			if (bReplace)
				pExtension = pContender;
			}

	if (pExtension)
		rc = pPath->Extend(pExtension);

	RETURN_RC(CJunction::Extend, rc);
}
/**********************************************************************************/
// Yield a joined paths, and add them to the path-list - a CJoin specific method
RC CJunction::YieldPaths(
	CPathList  & cList) // In/Out: The list of joined paths
	{
	RC rc=SUCCESS;

	// Loop over the path-list
	while (Size())
		{
		PBooleanPath pPath = Pop();
		if (pPath->Good())
			{
			// It's a good path, add it to the list and strike out its twin
			if (pPath->Twin())
				pPath->Twin()->StrikeOut();
			if (rc = cList.Add(pPath))
				{
				delete pPath;
				QUIT
				}
			pPath->StrikeOut();
			}
		else
			// It's a duplicate, delete it
			delete pPath;
		}
exit:
	RETURN_RC(CJunction::YieldPaths, rc);
	}

#if defined DEBUG
/**********************************************************************************/
// Dump
void CJunction::Dump()
	{
	DB_MSGX("Junction at", ());
	m_cPoint.Dump();
	m_cPaths.Dump();
	}
#endif

/////////////////////////////////////////////////////////////////////////////////
// Implementation of CBoolean
/*
 * ALGORITHM FOR UNION AND INTERSECT
 * ---------------------------------
 * Organize the paths in a list, and within every path, organize the curves
 * in a list. The curves are all transformed to page coordinates.  The path and
 * curve records listed are specific (CBooleanPath, CBooleanCurve) because they
 * contain additional information needed for Boolean operations.
 *
 * Split the paths at all intersections, including self-intersections.
 * Overlapping paths are OK, but not partially overlapping.
 *
 * Within each operands, orient the paths so that the interior of the shape will be
 * on your left as you go along the path. The orientation is done by setting a flag
 * at the path record.
 * Overlapping paths are left with one representative, if the number of ocurrances
 * is odd. Orientation is determined by casting a ray to the right from an interior
 * point, and counting intersections.
 *
 * Classify the status of every path in one operand with respect to the other shape,
 * in a way that will be used to determine what to do with it during the operation:
 *   INSIDE, OUTSIDE - depending on whether it lies inside or outside the other shape
 *   OVERLAP_SAME if it overlaps a path with the same direction in the other shape
 *   OVERLAP_OPPOSITE if it overlaps a path in the opposite direction.
 *
 * Select the paths that will survive the operation.  Selection criteria will vary
 * between operations.  For Union keep only OUTSIDE and OVERLAP_SAME paths.  For
 * Intersect keep INSIDE and OVERLAP_SAME.
 *
 * Reverse the paths if necessary, and compute their starting and ending points.
 *
 * Join all paths whenever possible.  Paths can only join head to tail.  Whenever
 * there is more than one candidate to be joined to a path, take the one that turns
 * leftmost.
 *
 * The result is stored in the Stock list, ready to operate again with another
 * shape.
 *
 * Construct a shape from the list of paths.
 *
 */
// Constructor/destructor

CBoolean::CBoolean()
	{
	srand(0);	// Predictable randomness is easier to debug
	m_pReceiving = &m_cStock;
	m_pCurrentPath = NULL;
	m_nAdmissable = 0;
	m_rTolerance = 0;
	m_id = INV_ID;
	m_pClient = NULL;
	m_bEmpty = FALSE;
	}

CBoolean::CBoolean(
	int nAdmissable,     // In: The type of paths that will stay after the operation
	double rTolerance,   // In: Point-distance tolerance
	PStorage pClient)	 // In: The client of this operation
	{
	srand(0);	// Predictable randomness is easier to debug
	m_pReceiving = &m_cStock;
	m_pCurrentPath = NULL;
	m_nAdmissable = nAdmissable;
	m_rTolerance = rTolerance;
	m_id = INV_ID;
	m_pClient = pClient;
	m_bEmpty = FALSE;
	}

CBoolean::~CBoolean()
	{
	delete m_pCurrentPath;
	}
/**********************************************************************************/
// Start accepting a new path
RC CBoolean::NewPathIn(
	BOOL bClosed,	// In: It's going to be a closed path if TRUE
	BOOL bFillable,	// In: Make it fillable if TRUE
	GeoID id)		// In: Reference ID
	{
	RC rc = E_NOMSG;
	// Union, Intersect and Subtract deal strictly with closed fillable paths
	if (bClosed && bFillable)
		rc = ALLOC_FAIL(m_pCurrentPath =
			new CBooleanPath(bClosed, bFillable, id));

	RETURN_RC(CBoolean::NewPathIn, rc);
	}
/**********************************************************************************/
// Accept a curve into the current path
RC CBoolean::AcceptCurve(
	PCurve pCurve,	// In: The curve
	GeoID id)		// In: Reference ID
	{
	RC rc = SUCCESS;
	PCurve p = NULL;
	RC done = SUCCESS;
	int iSlice = 0;


	if (!m_pCurrentPath)
		{
		rc = E_BadInput;
		QUIT
		}

	// Slice the curve to modeling-ready pieces
	while (!rc  && !done)
		{
		done = pCurve->NextSlice(iSlice, p);
		if (done)
			if (done != E_NOMSG)
				{
				rc = done;
				break;
				}
		rc = m_pCurrentPath->AcceptCurve(p, m_rTolerance, id);
		}
exit:
	RETURN_RC(CBoolean::AccepCurve, rc);
	}
/**********************************************************************************/
// Wrap up the current input path
RC CBoolean::EndPathIn()
	{
	RC rc = SUCCESS;

	if (m_pCurrentPath)
		{
		m_pCurrentPath->Update(m_cExtents);
		if (m_pCurrentPath->Size() > 0)
			rc = m_pReceiving->Add(m_pCurrentPath);
		}

	m_pCurrentPath = NULL;

	RETURN_RC(CBoolean::EndPathIn, rc);
	}
/**********************************************************************************/
// Organize a path list
RC CBoolean::Organize(
	GeoID id)	// In: Reference ID
	{
	RC rc;

	m_pReceiving->SetID(id);

	// Get all self intersctions
	if (rc = m_pReceiving->GetIntersections(m_pReceiving, RealisticTolerance()))
		QUIT

	// Split at intersections
	if (rc = m_pReceiving->SplitAtIntersections(m_rTolerance))
		QUIT

	// Orient the the paths (if it makes sense)
	rc = m_pReceiving->Orient();
exit:
	RETURN_RC(CBoolean::Organize, rc);
	}
/*****************************************************************************/
// Return a realistic tolerance
double CBoolean::RealisticTolerance()
	{
	/*
	 * A looser tolerance is returned if the current tolerance m_rTolerance
	 * is, relative to the numbers we are working with, too small for the
	 * limited accuracy of compter arithmetic.
	 */
	double r = MAX(m_cExtents.MaxX(), m_cExtents.MaxY());
	double s = MAX(-m_cExtents.MinX(), -m_cExtents.MinY());
	if (s > r)
		r = s;
	if (r < 1)
		r = 1;
	r *= FUZZ_GEN;
	if (m_rTolerance < r)
		return r;
	else
		return m_rTolerance;
	}
/**********************************************************************************/
RC CBoolean::Process(
	BOOL bFull)                  // In: Full processing if TRUE
	{
	RC rc;

	// Get all mutual intersctions
	if (rc = m_cStock.GetIntersections(&m_cTool, m_rTolerance))
		QUIT

	// Split at intersections
	if (rc = m_cStock.SplitAtIntersections(m_rTolerance))
		QUIT
	if (rc = m_cTool.SplitAtIntersections(m_rTolerance))
		QUIT

	if (bFull)
	{
		// Classify the paths of each shape w.r.t the other shape
		if (rc = m_cStock.Classify(m_cTool))
			QUIT
		rc = m_cTool.Classify(m_cStock);
	}
exit:
	RETURN_RC(CBoolean::Process, rc);
	}
/**********************************************************************************/
// Return E_NOMSG if we already know it's an empty intersection
BOOL CBoolean::EmptyIntersection()
	{
	if (!m_bEmpty)
		m_bEmpty = m_nAdmissable == INSIDE    // The operation is Intersect
			    && m_pReceiving->Size() == 0;  // and the result is empty
	if (m_bEmpty)
		m_cStock.Purge();
	return m_bEmpty;
	}
/**********************************************************************************/
// Delete the paths that go away during the Union operation
RC CBoolean::Operate(
	GeoID id)			// In: Reference ID
// This version is used for union and intersection only
	{
	RC rc = SUCCESS;

	if (EmptyIntersection())
		goto exit;

	if (m_pReceiving == &m_cStock)
		{
		// It's the first operand, just organize it
		rc = Organize(id);
		if (m_cStock.Size())
			m_pReceiving = &m_cTool;
		}
	else
		{
		// Organize it, and then process with Stock
		if (rc = Organize(id))
			QUIT

		// Process the existing operand with the new operand
		if (rc = Process(TRUE))
			QUIT

		// Remove the unneeded paths
		if (rc = m_cStock.UnionOrIntersect(m_nAdmissable))
			QUIT
		if (rc = m_cTool.UnionOrIntersect(m_nAdmissable))
			QUIT

		// Move everything to the Stock list
		rc = m_cStock.Copy(m_cTool, 0, m_cTool.Size());
		m_cTool.RemoveAll();
		m_cStock.Reset();
		}
exit:
	RETURN_RC(CBoolean::Operate, rc);
	}
/**********************************************************************************/
// Organize the paths into a graph
RC CBoolean::ConstructGraph()
	{
// This function removes all the paths away from m_cStock
	RC rc = SUCCESS;
	C3Point ptStart, ptEnd;
	PJunction pJunction = NULL;
	PBooleanPath pPath = NULL;

	while (!rc   &&   m_cStock.Size())
		{
		// Get the next nontrivial path
		pPath = m_cStock.Pop();
		if (pPath->Size() == 0 ||
			pPath->GetEndPoints(ptStart, ptEnd) != SUCCESS)
			{
			delete pPath;
			pPath = NULL;
			continue;
			}

		// Get the junctions and hook up to them
		rc = DM_MEM;
		pJunction = GetJunction(ptEnd);
		if (!pJunction)
			goto exit;
		pPath->SetEndJunction(pJunction);

		pJunction = GetJunction(ptStart);
		if (!pJunction)
			goto exit;
		if (rc = AddPathToJunction(pPath, pJunction))
			QUIT
		pPath = NULL;
		}
exit:
	delete pPath;	// Cleanup, in case of failure to add to the junction
	RETURN_RC(CBoolean::ConstructGraph, rc);
	}
/**********************************************************************************/
// Get a junction that matches a given point
PJunction CBoolean::GetJunction(
	const C3Point& P)   // In: The point
	{
	PJunction pJunction = NULL;

	// Search for an existing matching junction
	for (int i = 0;   i < m_cJunctions.Size();   i++)
		if (Junction(i)->Point().Equal(P, m_rTolerance))
			{
			pJunction = Junction(i);
			goto exit;
			}
	// Match not found, create a new junction and add it to the list
	pJunction = new CJunction(P);
	if (!pJunction)
		goto exit;
	if (m_cJunctions.Add(pJunction))
		{
		delete pJunction;
		pJunction = NULL;
		}
exit:
	// In any case
	return pJunction;
	}
/**********************************************************************************/
// Add this path to its junction
RC CBoolean::AddPathToJunction(
	PBooleanPath pPath,   // In: The path
	PJunction pJunction)  // In: The junction it is about to join
	{
/* This (rather trivial) function is just a stub for its overriding, more
 * substantial, CFragment counterpart.
 */
	RC rc = pJunction->Add(pPath);
	RETURN_RC(CBooelan::AddPathToJunction, rc);
	}
/**********************************************************************************/
// Tell the client to start a fresh path
RC CBoolean::NewPathOut(
	PBooleanPath pSource)	// In: Boolean path to create from
	{
	// Union and intersect always create region boundaries
	RC rc = m_pClient->NewPath(TRUE);
	RETURN_RC(CBooelan::NewPathOut, rc);
	}
/**********************************************************************************/
// Add all the paths in the list to a Visio shape
RC CBoolean::CreateShape(
	GeoID id,		  // In: Reference ID
	BOOL bSkipText,	  // In: =TRUE if text is to be ignored
	BOOL bSkipFill,	  // In: =TRUE if the shape's fill pattern is to be ignored
	CPathList& cList, // In/Out: The path list, emptied here
	PStorage pClient) // In/Out: The client who receives the shape
	{
	RC rc = SUCCESS;
	PPath pPath;
	int i;

	// No point in creating an empty shape
	if (cList.Size() <= 0)
		goto exit;

	// Start a new shape
	if (rc = pClient->NewShape())
		QUIT

	// Loop over the list
	for (i=0;  i<cList.Size();  i++)
		{
		pPath = NULL;
		PBooleanPath pCurrent = cList.Path(i);
		if (pCurrent->Size() < 1) // The path was empty
			continue;

		// Build a path and hand it over to the client
		pCurrent->WrapUp(m_rTolerance);
		if (rc = NewPathOut(pCurrent))
			QUIT
		if (rc = pCurrent->AddCurvesToPath(Trim(), m_pClient))
			QUIT

		// We no longer need the BooleanPath record
		cList.DeleteObject(i);
		}
	rc = pClient->WrapUpShape(m_id, bSkipText, bSkipFill);
exit:
	// We no longer need this list
	if (!rc) // We are confident that the paths have all been deleted
		cList.RemoveAll();
	RETURN_RC(CBoolean::CreateShape, rc);
	}
/**********************************************************************************/
// Wrap up
RC CBoolean::WrapUp()
	{
	RC rc=SUCCESS;
	if (!m_cStock.Size())
		goto exit;

	// Asemble the paths
	ConstructGraph();
	rc = Assemble();
	if (rc  ||  !m_cStock.Size())
		QUIT

	// Create a Shape from it
	rc = CreateShape(m_id, FALSE, FALSE, m_cStock, m_pClient);
exit:
	RETURN_RC(CBoolean::WrapUp, rc);
	}
/*******************************************************************/
// Get the total area of a shape
RC CBoolean::GetArea(
	double& rArea)	  // Out: the total area
	{
	RC rc;

	// Split and orient the paths
	if (rc = Organize())
		QUIT

	// Get the area
	rc = m_cStock.GetArea(rArea);
exit:
	RETURN_RC(CBoolean::GetArea, rc);
	}
/////////////////////////////////////////////////////////////////////////////////
// Implementation of CSubtract
/*
 * ALGORITHM FOR SUBTRACT
 * ----------------------
 * Differs only slightly from the Union and Intersect above:
 * The Tool's orientation needs to be reversed.
 * The types of paths that get to stay are OUTSIDE and OVERLAP_OPPOSITE in the
 * Stock, and INSIDE in the Tool.
 *
 */
// Constructor/destructor
CSubtract::CSubtract(
	double rTolerance,   // In: Point-distance tolerance
	PStorage pClient)	 // In: The client of this operation
	{
	srand(0);	// Predictable randomness is easier to debug
	m_rTolerance = rTolerance;
	m_pClient = pClient;
	// Everything else is set by the CBoolean default constructor
	}

CSubtract::~CSubtract()
	{
	}
/**********************************************************************************/
// Delete the paths that go away during the operation
RC CSubtract::Operate(
	GeoID id)			// In: Reference ID
	{
	RC rc = SUCCESS;
	int i;

	if (m_pReceiving == &m_cStock)
		{
		// This is the first operand - just organize it
		rc = Organize(id);
		m_pReceiving = &m_cTool;
		goto exit;
		}

	if (m_cStock.Size() == 0)
		// It's not the first shape, and we already have an empty result
		goto exit;

	// Organize the shape, and process it
	if (rc = Organize(id))
		QUIT

	// Process the existing operand with the new operand
	if (rc = Process(TRUE))
		QUIT

	// Remove the unneeded paths from Stock
 	for (i = 0;  i < m_cStock.Size();   i++)
		{
		PBooleanPath pCurrent = m_cStock.Path(i);
		if (pCurrent->Status() != OUTSIDE  &&
		    pCurrent->Status() != OVERLAP_OPPOSITE)
			{
			delete pCurrent;
			m_cStock.Remove(i--);
			}
		}

	// Remove the unneeded paths from Tool
 	for (i = 0;  i < m_cTool.Size();   i++)
		{
		PBooleanPath pCurrent = m_cTool.Path(i);
		if (pCurrent->Status() != INSIDE)
			{
			delete pCurrent;
			m_cTool.Remove(i--);
			}
		}

	// Reverse the orientation of the Tool's paths
 	for (i = 0;  i < m_cTool.Size();   i++)
		m_cTool.Path(i)->Reverse();

	// Move everything to the Stock list
	if (rc = m_cStock.Copy(m_cTool, 0, m_cTool.Size()))
		QUIT
	m_cTool.RemoveAll();
	m_cStock.Reset();
exit:
	RETURN_RC(CSubtract::Operate, rc);
	}
/////////////////////////////////////////////////////////////////////////////////
// Implementation of CFragment
/*
 * THE ALGORITHM FOR FRAGMENT
 * --------------------------
 * Organize the input paths, the same way as in other boolean operations, except
 * for orienting them (which is meaningless here).
 *
 * Remove every path that duplicates another path (regardless of direction)
 *
 * Prepare a graph with junctions, where paths starts and ends.  Every path is
 * owned by its start-junction, and points to its end-junction.
 *
 * Create a twin, i.e. a copy with reveresed orientation, for every path, and add
 * it to the graph.  Every path points to its twin.
 *
 * Assemble paths from the graph, looping over the list of junctions. When reaching
 * a junction, look for the path emanating with the sharpest left turn.  The path's
 * twin is not acceptable.  If there is nowhere to go, delete the path and its twin,
 * and backtrack to the junction we came from.  If there is nowhere to backtrack to
 * then quit, and proceed with the next junction in the list. If we reach a junction
 * that has already been visited, assemble a closed path, and move it to Stock list.
 *
 * Mover every counterclockwise path away, and make it the first path in a new
 * path-list. It will be the outer boundary of a shape.
 *
 *
 * For every hole - a clockwise path - find the closest boundary that contains it.
 * If found, add the hole to the shape of that boundary.  If not found, delete the
 * hole.
 *
 * Construct a shape from every list of paths.
 */
 // Constructor/destructor
CFragment::CFragment()
	{
	}

CFragment::CFragment(
	double rTolerance,  // In: Point distance tolerance
	PStorage pClient)	// In: The client of this operation
	{
	srand(0);	// Predictable randomness is easier to debug
	m_rTolerance = rTolerance;
	m_pClient = pClient;
	// Everything else is set by the CBoolean default constructor
	}

CFragment::~CFragment()
	{
	}
/**********************************************************************************/
// Start accepting a new path
RC CFragment::NewPathIn(
	BOOL bClosed,	// In: It's going to be a closed path if TRUE
	BOOL bFillable,	// In: Make it fillable if TRUE
	GeoID id)		// In: Reference ID
	{
	ASSERT (m_pCurrentPath == NULL);
	RC rc = ALLOC_FAIL(m_pCurrentPath =
			new CBooleanPath(bClosed, bFillable, id));
	RETURN_RC(CFragment::NewPathIn, rc);
	}
/**********************************************************************************/
// Organize a path list
RC CFragment::Organize(
	GeoID id)			// In: Reference ID
	{
	// Get all self intersctions
	RC rc = m_pReceiving->GetIntersections(m_pReceiving, RealisticTolerance());
	if (rc)
		QUIT

	m_pReceiving->SetID(id);

	// Split at intersections
	rc = m_pReceiving->SplitAtIntersections(m_rTolerance);
exit:
	RETURN_RC(CFragment::Organize, rc);
	}
/**********************************************************************************/
// Delete the paths that go away during the operation
RC CFragment::Operate(
	GeoID id)	// In: Reference ID
	{
	RC rc = SUCCESS;

	if (rc = Organize(id))
		QUIT
	// After the first operand --
	m_pReceiving = &m_cTool;

	// Process the existing operand with the new operand
	if (rc = Process(FALSE))
		QUIT

	// Move everything to the Stock list
	rc = m_cStock.Copy(m_cTool, 0, m_cTool.Size());
	m_cTool.RemoveAll();
exit:
	RETURN_RC(CFragment::Operate, rc);
	}
/**********************************************************************************/
// Add a path to the junction it is emanating from
RC CFragment::AddPathToJunction(
	PBooleanPath pPath,			// In: The path
	PJunction pJunction)	    // In: The junction it is about to join
/*
 * The path is a candidate for joining a junction. This function will
 * add it if it is not a duplicate of an existing path.
 */
	{
	RC rc = SUCCESS;
	BOOL bDuplicate = FALSE;
	int i;

	// First test all the paths emanating from Start
	for (i = 0;   i < pJunction->Size();   i++)
		{
		PBooleanPath pOther = pJunction->Path(i);
		if (pOther->EndJunction() == pPath->EndJunction())
			// same end, test for duplicate
			if (rc = pPath->TestForDuplicate(pOther, m_rTolerance, bDuplicate))
				QUIT
		}

	// Now test all the paths coming from the end-junction
	for (i = 0;   i < pPath->EndJunction()->Size();   i++)
		{
		PBooleanPath pOther = pPath->EndJunction()->Path(i);
		if (pOther->EndJunction() == pJunction)
			// Potential duplicate-opposite
			if (rc = pPath->TestForDuplicate(pOther, m_rTolerance, bDuplicate))
				QUIT
		}

	// Add it, if it isn't a duplicate, otherwise discard it
	if (bDuplicate)
		delete pPath;
	else
		rc = pJunction->Add(pPath);
exit:
	RETURN_RC(CFragment::AddPathToJunction, rc);
	}
/**********************************************************************************/
// Wrap up
RC CFragment::WrapUp()
	{
	RC rc=SUCCESS;

	if (!m_cStock.Size())
		goto exit;

	// Organize the paths into a graph
	if (rc = ConstructGraph())
		QUIT

	// Create a twin in the opposite direction for every path
	if (rc = CreateTwins())
		QUIT

	// Assemble into a list of closed paths, back into m_cStock;
	if (rc = Assemble())
		QUIT

	// Sort out into shapes with holes
	if (rc = SortOut())
		QUIT

	// Create the resulting ShapeSheets
	rc = ProduceResults();
exit:
	RETURN_RC(CFragment::WrapUp, rc);
	}
/**********************************************************************************/
// Produce the final result
RC CFragment::ProduceResults()
	{
	RC rc=SUCCESS;

	// Create the resulting ShapeSheets
	for (int i=0;   !rc && i<m_cResults.Size();   i++)
		{
		if (rc = CreateShape(m_id, TRUE, FALSE, *Shape(i), m_pClient))
			break;

		// Give the impatient user a chance to bail out
		if (TheUser()->Canceled())
			rc = E_CANCEL;
		}

	RETURN_RC(CFragment::ProduceResults, rc);
	}

/**********************************************************************************/
// Create a duplicate in the opposite direction for every path
RC CFragment::CreateTwins()
	{
	RC rc = SUCCESS;

	for (int i = 0;   i < m_cJunctions.Size();  i++)
		{
		PJunction pJunction = Junction(i);
		for (int j = 0;  j < pJunction->Size();  j++)
			if (rc = pJunction->Path(j)->CreateTwin(m_rTolerance))
				QUIT
		}
exit:
	RETURN_RC(CFragment::CreateTwins, rc);
	}
/**********************************************************************************/
// Assemble into closed paths
RC CBoolean::Assemble()
	{
	RC rc = SUCCESS;

		// Loop on junctions
	for (int i = 0;   i < m_cJunctions.Size();   i++)
		{
		PJunction pJunction = Junction(i);
		PBooleanPath pPath=NULL;
		PBooleanPath pNextPath = NULL;

		for (						int j=0;
			pJunction != NULL		&& j < ITERATIONS_LIMIT;
									j++)
			{
			BOOL bClosed = pJunction->Forward(pPath, pNextPath);
			if (pNextPath)
				{
				// Did we close a path?
				if (bClosed)
					pNextPath->AddClosedPath(m_cStock, pPath);
				else
					{
					pPath = pNextPath;
					pJunction = pPath->EndJunction();
					}
				}
			else
				{
				// We hit a dead end
				pJunction = pJunction->BackTrack(pPath);
				}
			}
		}
	RETURN_RC(CBoolean::Assemble, rc);
	}

/**********************************************************************************/
/* Albert.Isaev.ITC[31.7.2002/19:17:16] */
//Calls Purge methods for m_cStock and m_cTool
void CBoolean::Purge()
{
	m_cStock.Purge();
	m_cTool.Purge();
	return;
}

/**********************************************************************************/
// Sort out into shapes outer boundaries with holes in them
RC CFragment::SortOut()
	{
	RC rc = SUCCESS;
	int i;

	// First pass: move out all counterclockwise paths
	for (i = m_cStock.Size() - 1;   i >= 0;   i--)
		{
		int nCounterClockwise;
		PPathList pShape = NULL;
		PBooleanPath pPath = m_cStock.Path(i);

		// Get the path's orientation
		if (rc = pPath->Orientation(nCounterClockwise))
			QUIT

		switch(nCounterClockwise)
			{
			case -1:
				// It's a clockwise path, leave it for now
				break;

			case 1:
				{
				// It's the outer boundary - start a new shape with it
				if (rc = ALLOC_FAIL(pShape = new CPathList))
					QUIT
				if (rc = pShape->Add(pPath))
					QUIT
				if (rc = m_cResults.Add(pShape))
					QUIT
				// This path is now owned by the shape
				m_cStock.Remove(i);
				break;
				}

			default:
				// Probably an insignificant sliver, it failed to be oriented
				delete pPath;
				m_cStock.Remove(i);
			}  // End of switch
		}   // End of "for i" loop

	// Second pass: place every hole in a shape, or delete it
	while (m_cStock.Size() > 0)
		Locate(m_cStock.Pop());	// We don't break for error here
exit:
	RETURN_RC(CFragment::SortOut, rc);
	}
/**********************************************************************************/
// Find a shape that contains a given hole
RC CFragment::Locate(
	PBooleanPath pPath)  // In: The hole
// Return E_NOMSG if no container found for this hole
	{
	RC rc = FAILURE;
	int i,j,k;
	int nContainer = -1;

	// Iterate until successful
	for (i = 1;   rc && i < 50;   i++)
	{
		// Try from all curves
		for (j = 0;   rc && j < pPath->Size();  j++)
			{
			double rNearest = DBL_MAX;
			nContainer = -1;
			PBooleanCurve pCurve = pPath->Curve(j);
			PRay pRay = NULL;

			// Cast a ray to the left
			if (rc = pCurve->CastRay(pRay))
				continue;

			// Loop on shapes boundaries
			for (k = 0;   k < m_cResults.Size();    k++)
				{
				BOOL bReplace;
				PBooleanPath pBoundary = ShapeBoundary(k);
                // 2004/1/8 -[ EBATECH(CNBR) add null check!
                if( !pBoundary )
                    continue;
                // ]-
				if (rc = pBoundary->Locate(pRay, rNearest, bReplace))
					break;
				if (bReplace)
					nContainer = k;
				}
			delete pRay;
			}
		}
/*
 Holes with no container will always be there.  if we think of shapes as islands
 and holes as lakes, the outer boundaries of shapes border the ocean.
 */
	if (rc  ||  nContainer < 0)  // Container not found
		rc = m_cOcean.Add(pPath);
	else
		// We have a container for this hole
		rc = Shape(nContainer)->Add(pPath);

	RETURN_RC(CFragment::Locate, rc);
	}
/////////////////////////////////////////////////////////////////////////////////
// Some Fragment-specific methods of CBooleanPath
/**********************************************************************************/
RC CBooleanPath::TestForDuplicate(
	PBooleanPath pOther,	// In: Potential duplicate
	double rTolerance,		// In: Tolerance
	BOOL  & bDuplicate)		// Out: =TRUE if Other duplicates this path
	{
	RC rc =SUCCESS;
	bDuplicate = FALSE;

// By now multiple-curve duplicate paths should have been split at curve-ends
	if (Size() <= 1  &&  pOther->Size() <= 1)
		rc = AreDuplicates(FirstCurve(), pOther->FirstCurve(),
										rTolerance, bDuplicate);

	RETURN_RC(CBooleanPath::IsDuplicate, rc);
	}
/**********************************************************************************/
RC CBooleanPath::CreateTwin(
	double rTolerance) // In: Point-distance tolerance
	{
	int i;
	RC rc = SUCCESS;
	PCurve pCurve = NULL;
	PBooleanCurve pBCurve = NULL;

	// Create a new empty path
	if (m_pTwin)
		// This path already has a twin
		goto exit;

	if (rc = ALLOC_FAIL(m_pTwin = new CBooleanPath(this)))
		QUIT

	// Load it with the reversed list of reversed curves
	for (i = Size() - 1;   i >= 0;   i--)
		{
		if (rc = ALLOC_FAIL(pCurve = Curve(i)->m_pCurve->Clone()))
			QUIT
		pCurve->Reverse();

		#ifdef DEBUG
			{
			C3Point P,Q;
			Curve(i)->m_pCurve->GetStartPoint(P);
			pCurve->GetEndPoint(Q);
			ASSERT_ELSE(PointDistance(P, Q) < FUZZ_DIST, "Inorrect curve reverse")
				{
				Curve(i)->m_pCurve->Dump();
				pCurve->Dump();
				}

			Curve(i)->m_pCurve->GetEndPoint(P);
			pCurve->GetStartPoint(Q);
			ASSERT_ELSE(PointDistance(P, Q) < FUZZ_DIST, "Inorrect curve reverse")
				{
				Curve(i)->m_pCurve->Dump();
				pCurve->Dump();
				}
			}
		#endif

		pBCurve = new CBooleanCurve(pCurve, rTolerance, m_id);
		pCurve = NULL;		// Now owned by pBCurve
		if (rc = m_pTwin->Add(pBCurve))
			QUIT
		pBCurve = NULL;
		}

	// Hook it up
	m_pEndJunction->Add(m_pTwin);
	m_pTwin->m_pEndJunction = m_pStartJunction;
	m_pTwin->m_pStartJunction = m_pEndJunction;
	m_pTwin->m_pTwin = this;
exit:
	// Cleanup in case of failures
	delete pCurve;
	delete pBCurve;
	if (rc)
		{
		delete m_pTwin;
		m_pTwin = NULL;
		}
	RETURN_RC(CBooleanPath::CreateTwin, rc);
	}
/**********************************************************************************/
RC CBooleanPath::Orientation(
	int& nCounterClockwise)  // Out: = 1 if this path goes counterclockwise
							 //       -1 if it goes clockwise
							 //        0 if it cannot be determined
	{
	RC rc = FAILURE;

	// The path goes counterclockwise if its area is positive
	double rArea = 0;
	for (int i = 0;   i < Size();   i++)
		if (rc = Curve(i)->m_pCurve->UpdateAreaIntegral(rArea))
			QUIT

	if (FZERO(rArea, FUZZ_GEN))
		nCounterClockwise = 0;
	else if (rArea > 0)
		nCounterClockwise = 1;
	else
		nCounterClockwise = -1;

exit:
	RETURN_RC(CBooleanPath::Orientation, rc);
	}
/**********************************************************************************/
// See if this path is a container, and if it's closer than the current one
RC CBooleanPath::Locate(
	PRay pRay,           // In: The ray
	double  & rNearest,  // In/Out: Ray-parameter of the nearest intercept so far
	BOOL  & bCloser)	 // Out: =TRUE if this is a closer container
/*
 * This path is a container if the ray intercepts it at an odd number of points,
 * with no overlaps. This method checks it is a container,  and, if so, if its
 * nearest intercept is closer than the previous one.
 */
	{
	RC rc = SUCCESS;
	bCloser = FALSE;
	int i,j;
	int nIntercepts = 0;
	double rAt;
	C3Point P, T;
	double rClosestIntercept = DBL_MAX;

	for (j=0;  j<Size();  j++)
		{
		CRealArray cOnRay, cOnCurve;
		PCurve pCurve = Curve(j)->m_pCurve;

		if (rc = Intersect(pRay, pCurve, cOnRay, cOnCurve))
			break;

		// Analyze the curve intersection points
		for (i = 0;    i < cOnCurve.Size();    i++)
			{
			rAt = cOnCurve[i];
			if (FEQUAL(rAt, pCurve->Start(), FUZZ_GEN)  ||
			    FEQUAL(rAt, pCurve->End(), FUZZ_GEN))
				{
				// Vertex intercepts are not acceptable
				rc = E_NOMSG;
				goto exit;
				}

			// Eliminate grazing intercepts
			if (rc = pCurve->Evaluate(rAt, P, T))
				QUIT
			if (FZERO(Determinant(T, pRay->Direction()), FUZZ_DIST))
				{
				rc = E_NOMSG;
				goto exit;
				}
			}

		// Analyze the ray intercepts
		if (cOnRay.Size() > 0)
			{
			rAt = cOnRay[0];
			if (FZERO(rAt, FUZZ_GEN))  // Overlap, this can't be a container
				goto exit;

			if (rAt < rClosestIntercept)
				rClosestIntercept = rAt;
			nIntercepts += cOnRay.Size();
			}
		}

	if (!ODD(nIntercepts))  // This is not a container
		goto exit;
	if (rClosestIntercept >= rNearest) // No closer than the previous container
		goto exit;

	// Passed the hurdles, this is a better container, replace the previous one
	rNearest = rClosestIntercept;
	bCloser = TRUE;
exit:
	RETURN_RC(CBooleanPath::Locate, rc);
	}
/**********************************************************************************/
// Put together a closed path and add it to the path-list
RC CBooleanPath::AddClosedPath(
	CPathList  & cList,	  // In/Out: The list of output closed paths
	PBooleanPath & pLead) // In/out: set to NULL if used in the closed path
// This path starts a chain of paths that closes.  Here we pick them up and
// append them to a single closed path. pLead is the path that led us here.
// It may be in the chain, and then we'll set it to NULL.
	{
	RC rc = SUCCESS;
	int i;

	// The closed path will start with this one
	PBooleanPath pPath = Next();
	m_pStartJunction->Remove(this);
	if (this == pLead)
		pLead = NULL;

	// Traverse the loop to assemble a closed path

	for (i = 0;  i < ITERATIONS_LIMIT;  i++) // Infinite loop avoidance
		{
		PBooleanPath pNext;
		if (rc  ||  !pPath  ||  pPath == this)
			break;
		pNext = pPath->Next();

		if (pPath == pLead)
			pLead = NULL;
		pPath->m_pStartJunction->Remove(pPath);
		rc = Append(pPath);
		delete pPath;
		if (rc)
			QUIT
		pPath = pNext;
		}
	SetFillable(TRUE);
	rc = cList.Add(this);
exit:
	RETURN_RC(CBooleanPath::AddClosedPath, rc);
	}
/**********************************************************************************/
// Return TRUE if this path closes the input path
BOOL CBooleanPath::Closes(
	PBooleanPath pPath)	// In: A path
	{
	BOOL bFound = FALSE;
	PBooleanPath p = pPath;

	// Is this path anywhere along the chain of paths starting at pPath?
	do{
		p = p->Next();
		bFound = (this == p);
		}
	while (p && p != pPath  && !bFound);

	return bFound;
	}
/**********************************************************************************/
// Unlink this path from its twin, previous and next paths and its junctions
void CBooleanPath::Unlink()
	{
	if (m_pPrevious)
		{
		m_pPrevious->m_pNext = NULL;
		m_pPrevious = NULL;
		}

	if (m_pNext)
		{
		m_pNext->m_pPrevious = NULL;
		m_pNext = NULL;
		}

	if (m_pTwin)
		{
		m_pTwin->m_pTwin = NULL;
		m_pTwin = NULL;
		}
	}
/**********************************************************************************/
// Link this path backward to another path
void CBooleanPath::LinkTo(
	PBooleanPath p)	// The path to link to (NULL OK)
	{
	m_pPrevious = p;
	if (p)
		{
		p->m_pNext = this;
		}
	}
/**********************************************************************************/
// Wrap up
void CBooleanPath::WrapUp(
	double rTolerance)		// In: Gap-closing tolerance
	{
	if (Size() < 1)
		return;
	C3Point P, Q;
	if (!GetEndPoints(P, Q))
		if (PointDistance(P, Q) < rTolerance)
			m_bClosed = TRUE;
	}
////////////////////////////////////////////////////////////////////////////////////
//  Some Join specific methods
/**********************************************************************************/
// Start a new path
BOOL CBooleanPath::PathHead()
	{
	BOOL bHead = FALSE;
	//DB_MSGX("Entering PathHead",());
	//Dump();
	switch (m_nStatus)
		{
		case UNKNOWN:  // A new path-head (this path and its twin)
			m_nStatus =	m_pTwin->m_nStatus = HEAD;
		case HEAD:   // This is already a path-head
			bHead = TRUE;
		}
	//DB_MSGX("Upon exit status=%d",(m_nStatus));
	return bHead;
}
/**********************************************************************************/
// Extend a path
RC CBooleanPath::Extend(
	PBooleanPath pExtension)	 //In: The extension
	{
	RC rc = E_NOMSG;

	//DB_MSGX("Entering Path::Extend",());
	//Dump();

	if (pExtension == this)
		{
		m_nStatus = m_pTwin->m_nStatus = CLOSED;
		m_bFillable = m_pTwin->m_bFillable = TRUE;
		}
	else
		{
		// Update status
		pExtension->m_nStatus = REDUNDANT;
		pExtension->m_pTwin->m_nStatus = HEAD;
		m_pTwin->m_nStatus = REDUNDANT;

		// Append the extension
		if (rc = Append(pExtension))
			QUIT
		rc = pExtension->m_pTwin->Append(m_pTwin);

		// Update twinning
		m_pTwin = pExtension->m_pTwin;
		m_pTwin->m_pTwin = this;
		}
	//DB_MSGX("Extending to:",());
	//m_pEndJunction->Dump();
	//DB_MSGX("Status=%d",(m_nStatus));
exit:
	RETURN_RC(CBooleanPath::Extend, rc);
	}
/**********************************************************************************/
// Mark a path as no longer in use
void CBooleanPath::StrikeOut()
	{
	m_nStatus = REDUNDANT;
	if (m_pTwin)
		m_pTwin->m_nStatus = REDUNDANT;
	}
/////////////////////////////////////////////////////////////////////////////////
// Implementation of CCombine
/*
 * ALGORITHM FOR COMBINE
 * ----------------------
 * Make a list of all the paths and cretae the resulting shape from the list
 */
// Constructor/destructor
CCombine::CCombine()
	{
	}

CCombine::CCombine(
	PStorage pClient)	 // In: The client of this operation
	{
	m_pClient = pClient;
	// Everything else is set by the CBoolean default constructor
	}

CCombine::~CCombine()
	{
	}
/**********************************************************************************/
// Start accepting a new path
RC CCombine::NewPathIn(
	BOOL bClosed,	// In: It's going to be a closed path if TRUE
	BOOL bFillable,	// In: Make it fillable if TRUE
	GeoID id)		// In: Reference ID
	{
	RC rc = ALLOC_FAIL(m_pCurrentPath =
			new CBooleanPath(bClosed, bFillable, id));
	RETURN_RC(CCombine::NewPathIn, rc);
	}
/**********************************************************************************/
// Accept a curve into the current path
RC CCombine::AcceptCurve(
	PCurve pCurve,	// In: The curve
	GeoID id)		// In: Reference ID
	{
	RC rc = SUCCESS;

	if (m_pCurrentPath)
		rc = m_pCurrentPath->AcceptCurve(pCurve, m_rTolerance, id);
	else
		rc = E_BadInput;

	RETURN_RC(CCombine::AccepCurve, rc);
	}
/**********************************************************************************/
// Operate - there's nothing to do
RC CCombine::Operate(
	GeoID id)	// In: Reference ID
	{
	RETURN_RC(CCombine::Operate, SUCCESS);
	}
/**********************************************************************************/
// Tell the client to start a fresh path
RC CCombine::NewPathOut(
	PBooleanPath pSource)	// In: Boolean path to create from
	{
	RC rc = m_pClient->NewPath(pSource->Fillable());
	RETURN_RC(CCombine::NewPathOut, rc);
	}
/**********************************************************************************/
// Wrap up
RC CCombine::WrapUp()
	{
	RC rc = CreateShape(m_id, FALSE, FALSE, m_cStock, m_pClient);
	RETURN_RC(CCombine::WrapUp, rc);
	}
/////////////////////////////////////////////////////////////////////////////////
// Implementation of CTrim
/*
 * ALGORITHM FOR TRIM
 * -------------------
 * For every shape, make a list of all its paths
 * Split it and all the other shapes at all intersections
 * Cretae a shape from every path in the list, keeping the attributes (except text)
 * of the shape it came from.
 */
// Constructor/destructor
CTrim::CTrim(
	double rTolerance,  // In: Point distance tolerance
	PStorage pClient)	// In: The client of this operation
	{
	srand(0);	// Predictable randomness is easier to debug
	m_rTolerance = rTolerance;
	m_pClient = pClient;
	m_pReceiving = NULL;
	// Everything else is set by the CBoolean default constructor
	}

CTrim::~CTrim()
	{
	delete m_pReceiving;
	}
/**********************************************************************************/
// Start accepting a new path
RC CTrim::NewPathIn(
	BOOL bClosed,	// In: It's going to be a closed path if TRUE
	BOOL bFillable,	// In: Make it fillable if TRUE
	GeoID id)		// In: Reference ID
	{
	RC rc = SUCCESS;

	// If there is a current path, add it to the receiving list
	if (!m_pReceiving)
		if (rc = ALLOC_FAIL(m_pReceiving = new CPathList))
			QUIT
	if (m_pCurrentPath)
		if (m_pReceiving->Add(m_pCurrentPath))
			QUIT

	// Start a new path
	rc = ALLOC_FAIL(m_pCurrentPath = new CBooleanPath(bClosed, bFillable, id));
		QUIT
exit:
	RETURN_RC(CTrim::NewPathIn, rc);
	}
/**********************************************************************************/
// Delete the paths that go away during the operation
RC CTrim::Operate(
	GeoID id)			// In: Reference ID
	{
	RC rc = SUCCESS;
	int i;

	if (rc = Organize(id))
		QUIT

	for (i=0;   i<m_cResults.Size();  i++)
		{
		// Get all mutual intersctions
		if (rc = Shape(i)->GetIntersections(m_pReceiving, m_rTolerance))
			QUIT

		// Split at intersections
		if (rc = Shape(i)->SplitAtIntersections(m_rTolerance))
			QUIT
		}
	if (rc = m_pReceiving->SplitAtIntersections(m_rTolerance))
		QUIT
	if (rc = m_cResults.Add(m_pReceiving))
		delete m_pReceiving;
	m_pReceiving = NULL;
exit:
	RETURN_RC(CTrim::Operate, rc);
	}
/**********************************************************************************/
// Tell the client to start a fresh path
RC CTrim::NewPathOut(
	PBooleanPath pSource)	// In: Boolean path to create from
	{
	// Trim results are typically split, so they are not fillable
	RC rc = m_pClient->NewPath(FALSE);
	RETURN_RC(CCombine::NewPathOut, rc);
	}
/**********************************************************************************/
// Wrap up
RC CTrim::WrapUp()
	{
	/*
	 * This function creates a separate shape for every piece in the list
	 * of results, with the attributes (other than text) of the path is used to
	 * belong to. Since CreateShape is a CPathList method, the piece is temporaily
	 * added to cList, and then removed.
	 */
	RC rc = SUCCESS;
	CPathList cList;
	PPathList pPieces = NULL;
	PBooleanPath pPath = NULL;

	// Create the resulting ShapeSheets
	while (!rc  &&  m_cResults.Size())
		{
		pPieces = Pop();
		while (!rc   &&   pPieces->Size())
			{
			pPath = pPieces->Pop();
			if (!pPath->Bounded())
				delete pPath;
			else
				if (rc = cList.Add(pPath))
					QUIT
			pPath = NULL;
			rc = CreateShape(pPieces->ID(), TRUE, FALSE, cList, m_pClient);
			cList.RemoveAll();
			}
		delete pPieces;
		pPieces = NULL;
		}
exit:
	// Clenaup, in case of failure
	delete pPieces;
	delete pPath;
	RETURN_RC(CTrim::WrapUp, rc);
	}
/////////////////////////////////////////////////////////////////////////////////
// Implementation of CJoin
/*
 * ALGORITHM FOR JOIN
 * -------------------
 * For every shape, add all open paths to the Stock path-list and closed to Tool
 * Construct a graph of open paths and their junctions
 * Cretae a twin for every open path
 * Assemble by joining paths at junctions, retaining twin relationships
 * For every path in the graph, delete its twin, and move the path to the
 *      Tool path-list
 * Create a shape, and add all the paths in Tool to it
 */
// Constructor/destructor
CJoin::CJoin(
	double rTolerance,  // In: Point distance tolerance
	PStorage pClient)	// In: The client of this operation
	:CFragment(rTolerance, pClient)
	{
	}

CJoin::~CJoin()
	{
	}
/**********************************************************************************/
// Start accepting a new path
RC CJoin::NewPathIn(
	BOOL bClosed,	// In: It's going to be a closed path if TRUE
	BOOL bFillable,	// In: Make it fillable if TRUE
	GeoID id)		// In: Reference ID
	{
	RC rc = ALLOC_FAIL(m_pCurrentPath =
			new CBooleanPath(bClosed, bFillable, id));
	RETURN_RC(CJoin::NewPathIn, rc);
	}
/**********************************************************************************/
// Accept a curve into the current path
RC CJoin::AcceptCurve(
	PCurve pCurve,	// In: The curve
	GeoID id)		// In: Reference ID
	{
	RC rc = SUCCESS;

	if (m_pCurrentPath)
		rc = m_pCurrentPath->AcceptCurve(pCurve, m_rTolerance, id);
	else
		rc = E_BadInput;

	RETURN_RC(CJoin::AccepCurve, rc);
	}
/**********************************************************************************/
// Operate on a path - nothing to do
RC CJoin::Operate(
	GeoID id)			// In: Reference ID
	{
	RETURN_RC(CJoin::Operate, SUCCESS);
	}
/**********************************************************************************/
// Assemble into paths
RC CJoin::Assemble()
	{
	int i,j;
	RC rc = SUCCESS;

	// First pass: extend paths at all junctions
	for (i = 0;   i < m_cJunctions.Size();   i++)
		{
		PJunction pStart = Junction(i);
		for (j = 0;   j < pStart->Size();   j++)
			{
			// Start a new path
			PBooleanPath pPath = pStart->Path(j);
			if (!pPath->PathHead())
				// This is not a path-head
				continue;

			// Extend it as long as you can
			while (!rc)
				{
				PJunction pStep = pPath->EndJunction();
				rc = pStep->Extend(pPath);
				}
			if (rc)
				{
				if (rc == E_NOMSG)	  // Reset
					rc = SUCCESS;
				}
			else
				goto exit;
			}
		}
	// Second pass: yield the paths
	for (i = 0;   i < m_cJunctions.Size();   i++)
		Junction(i)->YieldPaths(m_cTool);
	m_cStock.RemoveAll();
exit:
	RETURN_RC(CJoin::Assemble, rc);
	}
/**********************************************************************************/
// Tell the client to start a fresh path
RC CJoin::NewPathOut(
	PBooleanPath pSource)	// In: Boolean path to create from
	{
	// Join will fill closed paths
	RC rc = m_pClient->NewPath(pSource->Closed());
	RETURN_RC(CJoin::NewPathOut, rc);
	}
/**********************************************************************************/
// Wrap up
RC CJoin::WrapUp()
	{
	RC rc = SUCCESS;
	CPathList cList;

	// Organize the paths into a graph
	if (rc = ConstructGraph())
		QUIT

	// Create a twin in the opposite direction for every path
	if (rc = CreateTwins())
		QUIT

	// Assemble by joining paths, back into m_cTool
	rc = Assemble();
	if (rc  ||  !m_cTool.Size())
		goto exit;

	// Cretae a ShapeSheet from it
	rc = CreateShape(m_id, FALSE, TRUE, m_cTool, m_pClient);
exit:
	RETURN_RC(CJoin::WrapUp, rc);
	}
/////////////////////////////////////////////////////////////////////////////////
// Implementation of CDrawRegion
/*
  ALGORITHM FOR DrawRegion
  ------------------------
  DrawRegion performs a Fragement operation, and then picks from the esulting
  shapes the one that contains a given point.  If a point was not specifies then
  it picks the Ocean.
 */

CDrawRegion::CDrawRegion()
	{
	m_pPickPoint = NULL;
	}

CDrawRegion::CDrawRegion(
	double rTolerance,	// In: Point distance tolerance
	PStorage pClient,	// In: The client of this operation
	P3Point pPick)		// In: Pick point (optional)
	:CFragment(rTolerance, pClient)
	{
	m_pPickPoint = pPick;
	}
/**********************************************************************************/
// Produce the final result
RC CDrawRegion::ProduceResults()
	{
	RC rc = SUCCESS;
	BOOL bFound = FALSE;

	// Create a shape from the region containing the point
	if (m_pPickPoint)
		for (int i = 0;   !bFound && i < m_cResults.Size();   i++)
			if (bFound = Shape(i)->HitTest(*m_pPickPoint))
				rc = CreateShape(m_id, TRUE, FALSE, *Shape(i), m_pClient);

	// No region containing the point was found, pick the ocean
	if (!bFound)
		rc = CreateShape(m_id, TRUE, FALSE, m_cOcean, m_pClient);

	RETURN_RC(CDrawRegion::ProduceResults, rc);
	}
