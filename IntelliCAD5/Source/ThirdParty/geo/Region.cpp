/*	Region.CPP - Implements the class CRegion
 *	Copyright (C) 1998 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * See Region.h for the definition of the class
 *
 */
// standard stuff
#include "Geometry.h"
#include "Point.h"
#include "Extents.h"
#include "Line.h"
#include "Path.h"
#include "TheUser.h"
#include "Extents.h"
#include "Boolean.h"
#include "Region.h"

DB_SOURCE_NAME;
// Copy constructor
CRegion::CRegion(
	CRegion & other,	// In: Other region to copy
	RC & rc)			// Out: Return code
	{
	rc = SUCCESS;
	PPath pPath = NULL;

	for (int i = 0;   !rc && i < other.Size();   i++)
		{
		pPath = new CPath(*other.Path(i), rc);
		if (!pPath)
			rc = DM_MEM;
		if (rc)
			break;
		rc = Add(pPath);
		}	
	}





/*******************************************************************/
// Get the total length 
RC CRegion::Length(
	double& rLength)  // Out: the total length
	{
	RC rc = SUCCESS;
	rLength = 0;

	// Loop on the paths
	for (int i=0;  i<Size();  i++)
		{
		double r;
		if (rc = Path(i)->Length(r))
			break;
		rLength += r;
		}
	RETURN_RC(CRegion::Length, rc);
	}
/*******************************************************************/
// Move the origin to a given point
RC CRegion::Move(
	const C3Point& vecMove)	// In: The vector to move by
	{
	RC rc = SUCCESS;

	for (int i=0;  i<Size();  i++)
		if (rc = Path(i)->Move(vecMove))
			break;
		
	RETURN_RC(CRegion::Move, rc);
	}
/*******************************************************************/
// Apply a given transformation IN PLACE
RC CRegion::Transform(
	PTransform pTransform)    // In: The desired transformation, NULL OK
	{
	RC rc = SUCCESS;
	int i;

	//Do nothing for NULL transformation
	if (!pTransform)
			goto exit;
	
	// Transform the paths
	for (i = 0;   i < Size();   i++)
		if (rc = Path(i)->Transform(pTransform))
			QUIT;
exit:
	RETURN_RC(CRegion::Transform, rc);
	}
/*******************************************************************/
// Get bounding box
RC CRegion::UpdateExtents(
	BOOL bTight,			  // In: Tight extents if =TRUE
	CExtents & cExtents,	  // In/out: The extents, updated here
	PAffine pTransform) const // Optional: Transformatiom (NULL OK)
	{
	RC rc = SUCCESS;

	for (int i = 0;   i < Size();   i++)
		{
		PPath pPath = Path(i);
		if (!pPath->Hidden())
			pPath->UpdateExtents(bTight, cExtents, pTransform); 
		}

	RETURN_RC(CRegion::UpdateExtents, rc);
	}		
/*******************************************************************/
// Test if a given point is contained in the region 
RC CRegion::TestIfContained(
	const C3Point& P,	// In: Point, in this region's coordinates
	int nAttempts,		// In: Number of attempts
	BOOL& bContained)	// Out: =TRUE if contained
	{
	RC rc = SUCCESS;
	int i, j;
	int nIntercepts = 0;
	bContained = FALSE;

	if (Size() == 0)
		goto exit;	// 'cause there's nothing to do

	rc = FAILURE;
	for (i = 1;   rc && i <= nAttempts;   i++)
		{
		nIntercepts = 0;
		// Cast a random ray
		double r = FRAND * TWOPI;   // A random number between 0 and 2Pi
		CRay ray(P, C3Point(sin(r), cos(r)));
		nIntercepts = 0;

		// Tally its intercepts with all the boundaries
		for (j = 0;   j < Size();   j++)
			rc = Path(j)->UpdateIntercepts(&ray, nIntercepts);
		}

	if (rc)	// All attempts failed
		QUIT

	// Interception was successful, see if we're inside
	bContained = ODD(nIntercepts);
exit:
	RETURN_RC(CRegion::TestIfContained, rc);
	}
/*******************************************************************/
// Test if a given point is in the region or on its boundary 
RC CRegion::HitTest(
	const C3Point& P,	// In: Point, in this region's coordinates
	double rTolerance,	// In: Tolerance
	int& iResult)		// Out: result = HIT_OUTSIDE, HIT_ON_BOUNDARY
						//		or HIT_INSIDE
	{
	int i;
	BOOL bContained = FALSE;

	if (rTolerance < FUZZ_DIST)
		rTolerance = FUZZ_DIST;

	// Iterate until successful
	srand(0);
	iResult = HIT_OUTSIDE;

	// First pass, see if the point is near a path
	for (i = 0;   i < Size();   i++)
		{
		double rDistance;
		PPath pPath = Path(i);
		if (pPath->Hidden())
			continue;

		pPath->NearestPoint(P, NULL, &rDistance);
		if (FZERO(rDistance, rTolerance))
			{
			iResult = HIT_ON_BOUNDARY;
			/* 
			 * We have establisehd that the point is near a path, but it 
			 may still be inside the region.
			 */
			break;
			}
		}

	// Second pass, see if the point is inside 
	if (!TestIfContained(P, 5, bContained)  && bContained)
		iResult = HIT_INSIDE;

	RETURN_RC(CRegion::HitTest, SUCCESS);
	}
/*******************************************************************/
// A quik test if a given point is contained in the region 
BOOL CRegion::QuickHitTest(
	const C3Point& P,	// In: Point, in this region's coordinates
	double rTolerance,	// In: Tolerance
	BOOL bBorderOnly)	// In: Ignore interior hits if TRUE
// This version is tailored for hot cursoring.  bBrderOnly is there for 
// the case fill pattern=none.
	{
	int i;
	BOOL bHit = FALSE;
	if (rTolerance < FUZZ_DIST)
		rTolerance = FUZZ_DIST;

	// First pass, see if the point is inside
	if (!bBorderOnly)
		if (TestIfContained(P, 4, bHit))
			bHit = FALSE;

	// Second pass, see if the point is near any path
	for (i = 0;   !bHit  &&  i < Size();   i++)
		{
		PPath pPath = Path(i);
		if (!pPath->Hidden())
			bHit = pPath->HitTest(P, rTolerance);
		}

	return bHit;
	}
/*******************************************************************/
// Test if the first region is included in the second
LOCAL RC TestIfIncluded(
	PRegion pFirst,		  // In: First region
	PRegion pSecond,	  // In: Second region
	BOOL& bContained,	  // Out: =TRUE if pFirst is contained in pSecond
	BOOL* pEmpty=NULL,	  // Out: =TRUE if pSecond is empty
	BOOL* pInterior=NULL) // Out: =TRUE if there's an interior point in common
	{
/*
  This test is only valid if we know that the boundaries of the two
  regions are disjoint.  In that case, it suffices to test one point
  of every path in the first region for inclusion in the second.  if
  pResult is entered, the caller is interested to know whether there is
  an interior point in in common. 
*/
	RC rc = SUCCESS;
	int i, j;
	C3Point P, T;
	double r;
	BOOL bInterior = FALSE;
	bContained = TRUE;
	BOOL empty = TRUE;


	for (i = 0;   bContained  &&  i < pFirst->Size();   i++)
		{
		PPath pPath = pFirst->Path(i);

		// Ignore empty paths
		if (pPath->Size() <= 0)
			continue;
		empty = FALSE;

		PCurve pCurve = pPath->Curve(0);

		rc = FAILURE;
		for (j = 1;  rc && j <= 5;  j++)
			{
			// Get a random point on this path
			if (rc = pCurve->RandomPoint(r, P, T))
				continue;

			// Test if the point is contained in the second region
			if (rc = pSecond->TestIfContained(P, 5, bContained))
				continue;
			if (!bContained)
				goto exit;
			if (pPath->Fillable())
				bInterior = TRUE;
			} // End of j-loop on 5 random point attempts
		}	// End of i-loop on paths
exit:
	if (empty)
		bContained = FALSE;
	if (pEmpty)
		*pEmpty = empty;
	if (pInterior)
		*pInterior = bInterior;
	RETURN_RC(TestIfIncluded, rc);
	}
/*******************************************************************/
// Test for overlap between 2 regions

/*
 To test for overlap we perform the Boolean Intersection and see
 if the result is empty or not.  The following class is derived from
 CStorage, it will be the receptor of new shapes.  Here we are not
 interested in the resulting shapes, we only need to know if there
 is at least one, so the NewShape method is geared to do nothing but
 return E_NOMSG.  This will stop the boolean, and the error message 
 will bubble up.  Thus, CBoolean::Operate returns E_NOMSG we'll
 know that NewShape has been called, hence the intersection is
 nonempty.
 */

class COverlapDetector	:	public CStorage
	{
	RC NewShape()
		{
		return E_NOMSG;
		}
	};

LOCAL RC SubmitRegion(
	PRegion pRegion,		// In: A region
	CBoolean& intersector)	// In: A Boolean operator
	{
	RC rc = SUCCESS;
	int i, j;
	PCurve pCurve = NULL;

	// Loop on paths
	for (i = 0;   i < pRegion->Size();   i++)
		{
		PPath pPath = pRegion->Path(i);
		if (rc = intersector.NewPathIn(pPath->Closed(),pPath->Fillable()))
			if (rc == E_NOMSG)
				{
				// This path is irrelevant, but it's OK
				rc = SUCCESS;
				continue;
				}
			else
				QUIT

		// Loop on curves
		for (j = 0;  j < pPath->Size();  j++)
			{
			if (rc = ALLOC_FAIL(pCurve = pPath->Curve(j)->Clone()))
				QUIT
			if (rc = intersector.AcceptCurve(pCurve))
				QUIT
			pCurve = NULL;	// Now owned by the intersector
			} // end for j

		if (rc = intersector.EndPathIn())
			QUIT
		} // End for i

	// Operate with this region
	rc = intersector.Operate();
exit:
	RETURN_RC(SubmitRegion, rc);
	}
/*******************************************************************/
// Test 2 regions for overlap
LOCAL RC TestForOverlap(
	PRegion pFirst,		// In: First region
	PRegion pSecond,	// In: Second region
	double rTolerance,	// In: Distance considered 0
	short& iResult)		// IN/out: SHAPES_OVERLAP if they do
						//		   SHAPES_TOUCH if they don't
	{
	RC rc;
	COverlapDetector detector;
	CBoolean intersector(INSIDE, rTolerance, &detector);

	// Feed the  shapes to the intersector
	if (rc = SubmitRegion(pFirst, intersector))
		QUIT
	if (rc = SubmitRegion(pSecond, intersector))
		QUIT

	// See if the intersection of the regions is nonempty
	if (rc = intersector.WrapUp())
		{
		if (rc == E_NOMSG)
			{
			// The intersector tried to start a new shape, which 
			// means that the intersection is nonempty
			iResult = SHAPES_OVERLAP |  SHAPES_INTERIOR;
			rc = SUCCESS;
			}
		}
	else
		iResult = SHAPES_TOUCH;
exit:
	RETURN_RC(TestForOverlap, rc);
	}
/*******************************************************************/
// Test the proximity relationship between 2 regions
RC TestProximity(
	PRegion pFirst,		// In: First region
	PRegion pSecond,	// In: Second region
	double rTolerance,	// In: Distance considered 0
	BOOL & bNear,		// Out: =TRUE if the shapes are within tolerance
	short * pDetails)	// Out: (Optional) see below below
// The result is a bitmask.  *pDatails & SHAPES_RELATION takes exactly one
// of the following (mutually exclusive) values: 
//		SHAPES_DISJOINT=0	if the shapes are disjoint,
//		SHAPES_TOUCH if the shapes touch each other within tolerance
//		SHAPES_OVERLAP if the shapes have some nonzero area in common, 
//		SHAPES_1_INSIDE_2 if the first contains the secone
//		SHAPES_2_INSIDE_1 if the second contains the first
// In addition, the SHAPE_INTERIOR bit indicates that the shapes have a
// point in common that's in the interior of both.  This excludes the
// SHAPES_TOUCH relation.
// If either one of the shapes is empty then the result is SHAPES_EMPTY, 
// and bNear=FALSE.
//
// pDetails=NULL indicate that the caller isn't interested in details.
// Calling it this way will make the functions run much faster.
	{
	RC rc = SUCCESS;
	int i, j;
	short iResult = 0;
	double rDistance;
	bNear = FALSE;
	BOOL bInterior = FALSE;
	if (rTolerance < FUZZ_DIST)
		rTolerance = FUZZ_DIST;
	
	// Find the distance between boundaries
	for (i = 0;   i < pFirst->Size();   i++)
		{
		for (j = 0;   j < pSecond->Size();   j++)
			{
			if (rc = PathDistance(pFirst->Path(i), pSecond->Path(j), 
								  rDistance, rTolerance))
				QUIT
			if (rDistance < rTolerance)
				{
				bNear = TRUE;
				break;
				}
			} // end for j
		if (bNear)
			break;
		}  // end for i

	// Further investigation
	if (bNear  &&  pDetails)
		// A boundary intersection was found, test for overlap
		rc = TestForOverlap(pFirst, pSecond, rTolerance, iResult);
	else
		{
		// Disjoint boundaries, still one may be included in the other
		BOOL bEmpty;
		srand(0);

		// See if the first shape is included in the second
		if (rc = TestIfIncluded(pFirst, pSecond, bNear, &bEmpty, &bInterior))
			QUIT
		if (bEmpty)
			iResult |= SHAPES_EMPTY;
		else
			if (bNear)
				{
				iResult = SHAPES_1_INSIDE_2;
				if (bInterior)
					iResult |= SHAPES_INTERIOR;
				goto exit;
				}

		// See if the second shape is included in the first
		if (rc = TestIfIncluded(pSecond, pFirst, bNear, &bEmpty, &bInterior))
			QUIT
		if (bEmpty)
			iResult |= SHAPES_EMPTY;
		else
			if (bNear)
				{
				iResult = SHAPES_2_INSIDE_1;
				if (bInterior)
					iResult |= SHAPES_INTERIOR;
				}
		}
exit:
	if (bInterior)
		iResult |= SHAPES_INTERIOR;
	if (pDetails)
		*pDetails = iResult;
	RETURN_RC(TestProximity, rc);
	}
/**************************************************************************/
// Snap a point to the geometry of a shape, if within tolerance
RC CRegion::NearestPoint(		// Return FAILURE if not found
	BOOL bIgnore,		 // In: Ignore nonsnappable paths if TRUE
	C3Point ptTo,		 // In: The point
	C3Point & ptNearest, // Out: nearest point 
	double*	pDistance,	 // Out: The distance - optional
	PCurve * pOn,		 // Out: curve it's on - optional
	double * pAt,		 // Out: Parameter on the curve  - optional
	int * pIxPath,		 // Out: The index of the path it's on - optional
	int * pIxCurve) const// Out: The index of the it's on - optional

	{
	int i, iPath = -1, iCurve = -1;
	C3Point P;
	double d, rDistance = INFINITY;
	double t, rAt = 0;
	PCurve p, pCurve = NULL;
	RC rc = E_NOMSG;

	if (pOn)
		*pOn = NULL;

	if (Size() <= 0) // The region is empty
		goto exit;

	for (i = 0;  i < Size();  i++)
		{
		int j;
		if (bIgnore && !Path(i)->Snappable())
			continue;
		Path(i)->NearestPoint(ptTo, &P, &d, &p, &t, &j);
		if (d < rDistance )
			{
			ptNearest = P;
			rAt = t;
			rDistance = d;
			pCurve = p;
			iPath = i;
			iCurve = j;
			}
		}
exit:
	if (pCurve)
		{
		if (pDistance)
			*pDistance = rDistance;
		if (pOn)
			*pOn = pCurve;
		if (pAt)
			*pAt = rAt;
		if (pIxPath)
			*pIxPath = iPath;
		if (pIxCurve)
			*pIxCurve = iCurve;
		rc = SUCCESS;
		}
	RETURN_RC(CRegion::NearestPoint, rc);
	}
/*******************************************************************/
// Get the distance between 2 regions
RC RegionsDistance(
	PRegion pFirst,		// In: First region
	PRegion pSecond,	// In: Second region
	double & rDistance)	// Out: The distance
	{
	RC rc = SUCCESS;
	int i, j;
	double d;
	rDistance = INFINITY;
	BOOL bIn;
	
	// Find the distance between boundaries
	for (i = 0;   i < pFirst->Size();   i++)
		{
		for (j = 0;   j < pSecond->Size();   j++)
			{
			if (rc = PathDistance(pFirst->Path(i), pSecond->Path(j), d, 0))
				QUIT
			if (d < rDistance)
				{
				rDistance = d;
				if (rDistance < FUZZ_DIST)
					goto exit;
				}
			} // end for j
		}  // end for i

	// Disjoint boundaries, still one may be included in the other
	srand(0);

	// See if the first shape is included in the second
	if (rc = TestIfIncluded(pFirst, pSecond, bIn))
		QUIT
	if (bIn)
		{
		rDistance = 0;
		goto exit;
		}

	// See if the second shape is included in the first
	if (rc = TestIfIncluded(pSecond, pFirst, bIn))
		QUIT
	if (bIn)
		rDistance = 0;
exit:
	RETURN_RC(RegionsDistance, rc);
	}

/*******************************************************************/
// How complex is this spline?
int CRegion::Complexity() const
	{
	int n = 0;
	for (int i = 0;  i < Size();  i++)
		n += Path(i)->Complexity();
	return n;
	}
#ifdef DEBUG
/*******************************************************************/
// Dump
void CRegion::Dump()
	{
	DB_MSGX("Region with %d paths",(Size()));
	for (int i=0;  i<Size();  i++)
		Path(i)->Dump();
	}
#endif
