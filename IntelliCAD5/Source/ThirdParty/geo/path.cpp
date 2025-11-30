/*	Path.cpp - Implements the class CPath
 *	Copyright (C) 1994-9 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * See Path.h for the definition of the class.
 *
 */
#include "Geometry.h"
#include "TheUser.h"
#include "Line.h"
#include "CompCrvs.h"		// For CTransformedCurve
#include "CrvPair.h"		// For intersection
#include "TheUser.h"		// For linearization
#include "Trnsform.h"		// For PTransform and PAffine
#include "CompCrvs.h"		// For CTransformedCurve
#include "2CurveSolver.h"	// For the distance between curves
#include "Path.h"

DB_SOURCE_NAME;

//Default constructor and destructor
CPath::CPath()
    {
    m_bProperties = 0;
	m_rStart = m_rEnd = 0;
    }
/*******************************************************************/
//  Copy constructor from another path
CPath::CPath(
	CPath &p,	// In: A path to copy
	RC & rc)
	{
	PCurve pCurve = NULL;
	rc = SUCCESS;
	m_bProperties = p.m_bProperties;
	m_rStart = p.m_rStart;
	m_rEnd = p.m_rEnd;

	for (int i = 0;   i < p.Size();   i++)
		{
		if (rc = ALLOC_FAIL(pCurve = p.Curve(i)->Clone()))
			QUIT
			
		// Add it to the shape geometry
		if (rc = Add(pCurve))
			QUIT
		pCurve = NULL;	// Now owned by the path
		}

exit:
	if (rc)
		delete pCurve;
	}
/*******************************************************************/
// Linearize
RC CPath::Linearize(
	double rResolution,		// In: Chord-height toelrance
	PAffine pTransform,		// In: Rendering transformation (NULL OK)
	PTransform pMapping,	// In: Non-affine mapping (NULL OK)
	PCache pCache)			// In/out: The rendering cache
	{
/* 
pMapping is a non-affine general transformation (e.g. custom line pattern
warping) that needs to be applied to the path before rendering. To apply 
it (if it's there, ie non NULL) we instantiate a CTransformedCurve for 
every curve in the path, and linearize that.  Otherwise we just linearize
the curves directly.

The situation is somewhat more complicated if the path has a line-end
markers (eg arrowheads). The linearized path then needs to be trimmed to 
accommondate setbacks at the ends.  The trimming parameter values should
have been computed and cached as m_rStart and m_rEnd when the path was 
constructed.
*/
	RC rc = SUCCESS;
	int i;
	double t;

	int n = Size() - 1;
	if (n < 0)
		goto exit;

	if (pMapping)
		// Linearize the procedurally mapped curves
		{
		CTransformedCurve C(Curve(0), pMapping, rc);
		if (rc)
			QUIT;

		// First curve needs to be trimeed for line-end marker
		t = n == 0 ? m_rEnd : t = C.End();
		if (rc = C.LinearizePart(m_rStart, t, rResolution, 
								 pTransform, *pCache))
			QUIT

		// All intermediate curves
		for (i = 1;  !rc && i < n;  i++)
			{ 
			C = CTransformedCurve(Curve(i), pMapping, rc);
			if (rc)
				QUIT;
			rc = C.Linearize(rResolution, pTransform, *pCache);
			}

		if (n == 0)
			// There's only one curve, and we've already linearized it
			goto exit;

		// Last curve needs to be trimeed for line-end marker
		C = CTransformedCurve(Curve(n), pMapping, rc);
		if (rc)
			QUIT;
		if (rc = C.LinearizePart(C.Start(), m_rEnd, rResolution, 
								 pTransform, *pCache))
			QUIT
		}
	else
		// Linearize the curves themselves
		{
		// First curve needs to be trimmed for line-end marker
		t = (n == 0) ? m_rEnd : t = Curve(0)->End();
		if (rc = Curve(0)->LinearizePart(m_rStart, t, rResolution, 
											  pTransform, *pCache))
			QUIT
		for (i = 1;  !rc && i < n;  i++)
			rc = Curve(i)->Linearize(rResolution, pTransform, *pCache);

		if (n > 0)
			// Last curve needs to be trimmed for line-end marker
			rc = Curve(n)->LinearizePart(Curve(n)->Start(), m_rEnd, 
								rResolution, pTransform, *pCache);
		}
exit:
		RETURN_RC(CPath::Linearize, rc);
	}
/*******************************************************************/
// Set path property bits
void CPath::SetProperty(
	BYTE bits,	// The property bit mask
	BOOL on)	// In: The value to set these bits to to
	{
	if (on)
		m_bProperties |= bits;
	else
		m_bProperties &= (~bits);
	}
/*******************************************************************/
// Update the extents with this path
RC CPath::UpdateExtents(
	BOOL bTight,			  // In: Tight extents if =TRUE
	CExtents & cExtents,	  // IN/Out: The extents, updated here
	PAffine pTransform) const // Optional: Transformatiom (NULL OK)
/*
 * update existing extents in coordinates defined by transformation
 * pTransform, assumed identity if NULL.
 */
	{
	RC rc = SUCCESS;
	int i;
	if (Size() <= 0)
		goto exit;
	if (rc = Curve(0)->UpdateExtents(bTight, FALSE, cExtents, pTransform))
		QUIT
	for (i = 1;   i < Size();  i++)
		if (rc = Curve(i)->UpdateExtents(bTight, TRUE, cExtents, pTransform))
			QUIT
exit:
	RETURN_RC(CPath::UpdateExtents, rc);
	}		
/*******************************************************************/
// Update the wireframe extents with this path
RC CPath::Wireframe(
	CExtents & cExtents,	  // IN/Out: The extents, updated here
	PAffine pTransform) const // Optional: Transformatiom (NULL OK)
/*
 * update existing extents in coordinates defined by transformation
 * pTransform, assumed identity if NULL.
 */
	{
	RC rc = SUCCESS;
	int i;
	if (Size() <= 0)
		goto exit;
	if (rc = Curve(0)->Wireframe(FALSE, cExtents, pTransform))
		QUIT
	for (i = 1;   i < Size();  i++)
		if (rc = Curve(i)->Wireframe(TRUE, cExtents, pTransform))
			QUIT
exit:
	RETURN_RC(CPath::Wireframe, rc);
	}		
/*******************************************************************/
// Wrap up a path incremental construction
RC CPath::WrapUp()	// Return E_NOMSG if the path is empty
	{
// This function should be called after path's incremental construction
	RC rc = E_NOMSG;
	C3Point P, Q;
	int n = Size() - 1;

	if (n < 0) // The path is empty
		{
		Purge();
		goto exit;
		}

	m_rStart = Curve(0)->Start();
	m_rEnd = Curve(n)->End();

	// Get start and endpoint and determine if closed
	if (Curve(0)->GetStartPoint(P) || Curve(n)->GetEndPoint(Q))
		SetClosed(FALSE);
	else
		SetClosed(PointDistance(P,Q) < FUZZ_DIST);

	rc = ReallocateToSize();
exit:	
	RETURN_RC(CPath::WrapUp, rc);
	}
/*******************************************************************/
// Add a (possibly invisible) curve
RC	CPath::Add(
	PCurve pSegment,	// In: The curve to add	
	BOOL bIsVisible)	// Optional: The segment is invisible if TRUE
	{
// The sole purpose of the Invisible flag is to mark the path as having
// gaps, to prompt separate fill and stroke passes when rendering.  
// For all other purposes it can be ignored.
	if (!bIsVisible)
		m_bProperties |= PATH_HAS_GAPS;

	return CGeoObjectArray::Add(pSegment);
	}

/*******************************************************************/
// Reverse the path 
RC	CPath::Reverse()
	{
	RC rc = SUCCESS;
	
	// Loop on the curve-list and reverse the curves
	for (int i = 0;   i < Size();   i++)
		if (rc = Curve(i)->Reverse())
			QUIT

	// Reverse the list
	CGeoObjectArray::Reverse();
exit:	
	RETURN_RC(CPath::Reverse, rc);
	}
/*******************************************************************/
// Length
RC CPath::Length(
	double & rLength)   // Out: The total length
    {
	RC rc = SUCCESS;
	
	rLength = 0;
	for (int i = 0;   i < Size();  i++)
		{
		double r;
		if (rc = Curve(i)->Length(r))
			break;
		rLength += r;
		}
	RETURN_RC(CPath::Length, rc);
	}
/*******************************************************************/
// Get the nearest point to a given point 
void CPath::NearestPoint(
	C3Point ptTo,		 // In: The point to get near
	P3Point pNearest,	 // Out: Nearest point - optional
	double*	pDistance,	 // Out: The distance - optional
	PCurve* pOn,		 // Out: Curve the point on - Optional
	double* pAt,		 // Out: Parameter there - optional
	int* pIxCurve) const // Out: The index of the curve - optional
	{
	C3Point P, ptNearest;
	double d, rDistance = INFINITY;
	double t, rAt = 0;
	int i, index = -1;

	// Loop on the curves, updating the nearest point
	for (i=0;  i<Size();  i++)
		{
		if (!Curve(i)->NearestPoint(ptTo, P, t, d, i==0))
			if (d < rDistance)
				{
				rDistance = d;
				ptNearest = P;
				index = i;
				rAt = t;
				}
		}

	// Output
	if (pDistance)
		*pDistance = rDistance;
	if (pNearest)
		*pNearest = ptNearest;
	if (pOn)
		if (index >= 0)
			*pOn = Curve(index);
		else
			*pOn = NULL;
	if (pIxCurve)
		*pIxCurve = index;
	if (pAt)
		*pAt = rAt;
	}
/*******************************************************************/
// Return the curve with a given index
PCurve CPath::Curve(
	int i)  //In: The index of the desired curve
	{
	if (i >= 0  &&  i < Size())
		return (PCurve)m_pEntries[i];
	else	
		return NULL;
	}
/*******************************************************************/
// Move the origin to a given point
RC CPath::Move(
	const C3Point & ptMove)
	{
	RC rc = SUCCESS;

	for (int i=0;  i<Size();  i++)
		if (rc = Curve(i)->Move(ptMove))
			break;
		
	RETURN_RC(CPath::Move, rc);
	}
/*******************************************************************/
// Apply a given transformation IN PLACE
RC CPath::Transform(
	PTransform pTransform)    // In: The desired transformation, NULL OK
	{
	RC rc = SUCCESS;
	int i;

	//Do nothing for NULL transformation
	if (!pTransform)
			QUIT
	
	// Replaced curves by their transformed copies
	for (i = 0;   i < Size();   i++)
		if (rc = Curve(i)->Transform(*pTransform))
			QUIT
exit:
	RETURN_RC(CPath::Transform, rc);
	}
/*******************************************************************/
// Tally the number of intersections of a ray with this path
RC CPath::UpdateIntercepts(
	PRay	pRay,				// In: A ray 
	int	& nIntercepts) const    // In/out: The number of intercepts
	{
	RC rc = SUCCESS;

	if (!Hidden()  &&   Fillable())
		for (int i = 0;   !rc && i < Size();   i++)
			rc = Curve(i)->UpdateIntercepts(pRay, nIntercepts);

	RETURN_RC(CPath::UpdateIntercepts, rc);
	}
/*******************************************************************/
// Update the nearest and furthest intercepts of a ray with this path
RC CPath::UpdateIntercepts(
	PUnboundedLine pLine, // In: A line 
	BOOL bIgnoreOrigin,	  // In: Ignore the origin if TRUE
	double * pNearFore,   // In/out: Forward intercept nearest to line's origin
	double * pFarFore,    // In/out: Forward intercept farthest from line's origin
	double * pNearBack,	  // In/out: Backward intercept nearest to line's origin
	double * pFarBack)    // In/out: Backward intercept farthest from line's origin
	{
	RC rc = SUCCESS;

	if (!Hidden())
		for (int i = 0;   !rc && i < Size();   i++)
			rc = Curve(i)->UpdateIntercepts(pLine, bIgnoreOrigin, pNearFore,
												pFarFore, pNearBack, pFarBack);
	RETURN_RC(CPath::UpdateIntercepts, rc);
	}
/*******************************************************************/
// Get the path's start and end points
RC CPath::EndPoints(
	C3Point & ptStart,  // Out: Start point
	C3Point & ptEnd)	// Out: End point
	{
	RC rc = FAILURE;
	int nLast = Size() - 1;
	if (nLast < 0)
		QUIT

	if (rc = Curve(0)->GetStartPoint(ptStart))
		QUIT
	rc = Curve(nLast)->GetEndPoint(ptEnd);
exit:
	RETURN_RC(CPath::EndPoints, rc);
	}
/*******************************************************************/
// Get the point, tangent and curvature at the path's start or end
RC CPath::EndPointGeometry(
	int iWhichEnd,        // In: 0 for start point, 1 for end
	C3Point & ptPoint,    // Out: End point
	C3Point & ptTangent,  // Out: Derivative
	double & rCurvature)  // Out: Curvature there 
	{
	RC rc = FAILURE;
	PCurve pCurve = NULL;
	double t=0;

	int nLast = Size() - 1;
	if (nLast < 0)
		QUIT

	// Get the curve and paremater value
	if (iWhichEnd == 0)  // They want the starting point
		{
		pCurve = Curve(0);
		t = pCurve->Start();
		}
	else				// They want the ending point
		{
		pCurve = Curve(nLast);
		t = pCurve->End();
		}
	// Get the geometry there
	rc = pCurve->Evaluate(t, ptPoint, ptTangent, rCurvature);

exit:
	RETURN_RC(CPath::EndPointGeometry, rc);
	}
/*******************************************************************/
// Split every curve at interior cusps.
RC CPath::SplitAtCusps()
	{
	RC rc = SUCCESS;
	int i;

	for (i = 0;  i < Size();  /* incremented inside the loop */)
		{
		int j = 0;  
		PCurve pCurve = Curve(i);
		PCurve pSlice = NULL;
		
		while (1)
			{
			if (rc = pCurve->NextSlice(j, pSlice))
				{
				if (rc == E_NOMSG)	// This is the last slice
					{
					rc = SUCCESS;
					m_pEntries[i++] = pSlice;
					break;
					}
				else				// Real trouble
					QUIT
				}
			else
				rc = Insert(pSlice, i++);
			}	// end while
		}	// end for
exit:
	RETURN_RC(CPath::CplitAtCusps, rc);
	}
/*******************************************************************/
// Get the nearest point to a given point 
BOOL CPath::HitTest(
	const C3Point& ptTo, // In: The point to get near
	double	rMargin)	 // Out: The distance considered a hit
	{
	int i;
	C3Point D;
	BOOL bHit = FALSE;
	double rSqMargin = rMargin * rMargin;

	if (Size() <= 0) // The path is empty
		goto exit;

	// Try the endpoints
	if (!Curve(0)->GetStartPoint(D))
		{
		D -= ptTo;
		if (bHit = D * D <= rSqMargin)
			goto exit;
		}

	// Hit test the curves
	for (i=0;  i<Size();  i++)
		{
		// Test the curve's endpoint
		if (!Curve(i)->GetEndPoint(D))
			{
			D -= ptTo;
			if (bHit = D * D < rSqMargin)
				break;
			}

		// Test for an interior hit
		if (bHit = Curve(i)->HitTest(ptTo, rMargin))
			break;
		}
exit:
	return bHit;
	}
/*******************************************************************/
// Snap the path ends to the given points
RC CPath::SnapEnds(
	const C3Point & rptStart, // In: Snapped start point
	const C3Point & rptEnd)	  // In: Snapped end point
	{
	RC rc = SUCCESS;
	INT nLast = Size() - 1;

	if (nLast < 0)
		{
		rc = E_BadInput;
		QUIT
		}

	// Snap the path's endpoints
	if (rc = Curve(0)->SnapEnd(LEFT, rptStart))
		QUIT
	if (rc = Curve(nLast)->SnapEnd(RIGHT, rptEnd))
		QUIT

	// Close it, if it is closed
	SetClosed(rptStart == rptEnd);

exit:
	RETURN_RC(CPath::SnapEnds, rc);
	}

/*******************************************************************/
// How complex is this spline?
int CPath::Complexity() const
	{
	int n = 0;
	for (int i = 0;  i < Size();  i++)
		n += Curve(i)->Complexity();
	return n;
	}
#ifdef DEBUG
/*******************************************************************/
// Dump
void CPath::Dump() const
	{
	if (Closed())
		{
		DB_MSGX("Closed ",())
		}
	else
		{
		DB_MSGX("Open ",())
		}

	if (Fillable())
		{
		DB_MSGX("boundary ",())
		}
	else
		{
		DB_MSGX("stroke ",())
		}

	DB_MSGX("with %d curves",(Size()));
	for (int i=0;  i<Size();  i++)
		Curve(i)->Dump();
	}
#endif
/////////////////////////////////////////////////////////////////////
// Functions
RC PathDistance(
	PPath pFirst,   // In: First path
	PPath pSecond,	// In: Second path
	double& rDist,	// Out: The distance
	double rMargin)	// Optional: distance margin
	{
	RC rc = SUCCESS;
	int i, j;
	C3Point P, Q;
	double s, t, d;
	rDist = INFINITY;
	
	if (!pFirst->Size()  ||  !pSecond->Size())
		goto exit;

	// The distance from the first path's start point
	if (!pFirst->Curve(0)->GetStartPoint(P))
		{
		pSecond->NearestPoint(P, NULL, &d);
		if (d < rDist)
			rDist = d;
		}

	// Loop over curves
	for (i = 0;  i < pFirst->Size();  i++)
		{
		PCurve pCurrent = pFirst->Curve(i);
		// The distance from the current curve's endpoint
		if (!pCurrent->GetEndPoint(P))
			{
			pSecond->NearestPoint(P, NULL, &d);
			if (d < rDist)
				rDist = d;
			}

		// The distance between curves
		for (j = 0;   j < pSecond->Size();	j++)
			{
			if (rc = Distance(pCurrent, pSecond->Curve(j),
								d, s, t, P, Q, rMargin))
				{
				if (rc == E_NOMSG)
					// Didn't converge, but that's OK
					rc = SUCCESS;
				else
					QUIT
				}
			else
				// Found a local minimum. update the global minimum
				if (d < rDist)
					rDist = d;
			}  // End for j
		}  // End for i
exit:
	RETURN_RC(PathDistance, rc);
	}

/*******************************************************************/
//  Copy constructor from another path
CPath& CPath::operator=(const CPath &p)
{
	PCurve pCurve = NULL;
	m_bProperties = p.m_bProperties;
	m_rStart = p.m_rStart;
	m_rEnd = p.m_rEnd;

    RemoveAll();
	for (int i = 0;   i < p.Size();   i++) {
		if (!(pCurve = p.Curve(i)->Clone()))
			break;
			
		// Add it to the shape geometry
        if (Add(pCurve)) 
			break;

		pCurve = NULL;	// Now owned by the path
	}

    if (pCurve) {
		delete pCurve;
        throw;
    }

    return *this;
}