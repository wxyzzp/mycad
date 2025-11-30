/*	PolyLine.cpp - Implements the class CPolyLine.
 *	Copyright (C) 1998-1999 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * See PolyLine.h for the definition of the class; this implements its methods.
 *
 */

#include "Geometry.h"
#include "TheUser.h"
#include "Point.h"
#include "Line.h"
#include "Trnsform.h"
#include "Extents.h"
#include "Knots.h"
#include "CompCrvs.h"
#include "PolyLine.h"
#include "plane.h"

DB_SOURCE_NAME;

PCurve CPolyLine::Clone() const
	{
	RC rc = SUCCESS;
	PPolyLine p = new CPolyLine(*this, rc);
	if (!p)
		rc = DM_MEM;
	if (rc)
		{
		delete p;
		p = NULL;
		}
	return p;
	}
/************************************************************************/
// Linearization
RC CPolyLine::Linearize(
	double rResolution,			// In: Resolution - ignored here
	PAffine pTransform,			// In: Rendering transformation (NULL OK)
	CCache & cCache) const      // In/Out: The cache we render into
	{
	RC rc = SUCCESS;
	int i;
	C3Point P;
		
	// Generate the first point if the cache is empty
	if (!cCache.Started())
		{
		if (pTransform)
			pTransform->ApplyToPoint(Vertex(0), P);
		else
			P = Vertex(0);

		if (rc = cCache.Accept(P, 0))
				QUIT
		}

	// Geerate the rest of the points
	for (i = 1;  i < Size();  i++)
		{
		if (pTransform)
			pTransform->ApplyToPoint(Vertex(i), P);
		else 
			P = Vertex(i);

		// Generate the first point if the cache is empty
		if (rc = cCache.Accept(P, 0))
			QUIT
		}
exit:	
	RETURN_RC(CPolyLine::Linearize, rc);
	}
/**********************************************************************/
// Integrate a real function along the Polyline
RC CPolyLine::Integrate(
	CRealFunction &  cFunction, // In: The function
	double rFrom,               // In: Lower integration bound
	double rTo,                 // In: Upper integration bound
	double & rIntegral) const   // Out: The integral
    {
// Call CCurve::Integrate for every nontrivial span
	RC rc = SUCCESS;
	double a, b;
	rIntegral = 0.0;
	int i = 0;
	BOOL bDone = FALSE;

	// This implementation is no good for extended domain

	// Loop over spans and integrate	
	for (a = rFrom, i = (int)floor(rFrom) + 1;   
		 !bDone  &&  i < SegmentCount();
		 a = b, i++)
		{
		double r;
		b = i;
		bDone = b >= rTo;
		if (bDone)
			b = rTo;
			
		if (rc = CCurve::Integrate(cFunction, a, b, r))
			QUIT
		rIntegral += r;
		}
exit:
	RETURN_RC(CPolyLine::Integrate, rc);
	}
/************************************************************************/
// Localize a given parameter value within its segment
int CPolyLine::Localize(
	double &t)	const		 // In/out: Parameter value
    {
// Given a parameter value t, this method finds the highest i within
// the polyline segments so that t-i>=0, and adjust t to be relative to
// that segment
	int i = (int)floor(t);
	if (i < 0)
		i = 0;
	if (i >= Size() - 2)
		i = Size() - 2;	
	t -= i;
	return i;
    }
/************************************************************************/
// Evaluator - position only
RC CPolyLine::Evaluate(
	double rAt,				 // In: Parameter value
	C3Point & ptPoint) const // Out: Point on the curve there
    {
	int i = Localize(rAt);
	ptPoint = (1 - rAt) * Vertex(i) + rAt * Vertex(i+1);

	RETURN_RC(CPolyLine::Evaluate, SUCCESS);
    }
/************************************************************************/
// Evaluator - position and tangent vector
RC CPolyLine::Evaluate(
	double rAt,				   // In: Parameter value
	C3Point & ptPoint,		   // Out: Point on the polyline there
	C3Point & ptTangent) const // Out: Tangent vector at that point
    {
	int i = Localize(rAt);
	ptPoint = (1 - rAt) * Vertex(i) + rAt * Vertex(i+1);
	ptTangent = Vertex(i+1) - Vertex(i);
	RETURN_RC(CPolyLine::Evaluate, SUCCESS);
    }
/************************************************************************/
RC CPolyLine::Evaluate(
	double rAt,             // In: Where to evaluate
	int nDerivatives,       // In: NUmber of desired derivatives
	P3Point pValues) const  // Out: Position and derivatives, ROOM FOR 3
    {
	int i = Localize(rAt);
	pValues[0] = (1 - rAt) * Vertex(i) + rAt * Vertex(i+1);
	pValues[1] = Vertex(i+1) - Vertex(i);
	for (i = 2;  i < nDerivatives;  i++)
		pValues[i] = C3Point(0,0,0);
	RETURN_RC(CPolyLine::Evaluate, SUCCESS);
    }
/************************************************************************/
// Update bounding box in a given direction
RC CPolyLine::UpdateExtents(
	BOOL bTight,		 // In: Ignored here
	BOOL bIgnoreStart,	 // In: Ignore the startpoint, if =TRUE
	CExtents & cExtents, // In/out: The extents, updated here
	PAffine pTransform)  // Optional: Transformation (NULL OK)
	{
	for (int i=0;  i<Size();  i++)
		{
		C3Point P = Vertex(i);
		if (pTransform)
			pTransform->ApplyToPoint(P, P);
		cExtents.Update(P);
		}

	RETURN_RC(CPolyLine::UpdateExtents, SUCCESS);
	}
/*******************************************************************************/
// A translation to a given point
RC CPolyLine::Move(
	const C3Point & ptBy)	// In: The desired origin's new position
	{
	for (int i = 0;    i < Size();    i++) 
		Vertex(i) += ptBy;
	RETURN_RC(CPolyLine::Move, SUCCESS);
	}
/****************************************************************************/
// Breaks in continuity of the nth derivative
RC CPolyLine::Breaks(
	int iDeriv,                 // In: The required continuity
	PRealArray & pBreaks) const //Out: The breaks
/*
 * All curves are treated as periodic, so even if the curve is open,
 * its first break (at Start) will NOT be repeated at the end.  Thus, a
 * smooth open curve will show ONE break.
 */
	{
	RC rc;
	int i;
	pBreaks = new CRealArray(Size()-1, rc);
	if (!pBreaks)
		rc = DM_MEM;
	if (rc)
		QUIT
	
	for (i = 0;  i < Size() - 1;  i++)
		(*pBreaks)[i] = i;
exit:
	RETURN_RC(CPolyLine::Breaks, rc);
	}
/************************************************************************/
// Transform in place
RC CPolyLine::Transform(
	CTransform & cTransform)   // In: The transformation
	{
	RC rc;
	
	// A non-affine transform cannot be applied in place
	if (!cTransform.Affine())
		{
		rc = E_BadInput;
		QUIT
		}

	// Apply the transformation to the control points
	rc = cTransform.ApplyToPointArray(*this);
exit:
	RETURN_RC(CPolyLine::Transform, rc);
	}
/************************************************************************/
// Seeds for equations solving
RC CPolyLine::Seeds(
	CRealArray & arrSeeds,  // In/Out: Array of seeds
	double* pStart,			// In: Domain start (optional, NULL OK)
	double* pEnd) const 	// In: Domain end (optional, NULL OK)
    {
	RC rc = SUCCESS;

	for (int i = 0;  i < Size();  i++)
		if (rc = arrSeeds.Add(i))
			break;
	RETURN_RC(CPolyLine::Seeds, rc);
    }
/************************************************************************/
// Snap the end point to a given point
RC CPolyLine::SnapEnd(
	SIDE nWhichEnd,   	   // In: Start or end
	const C3Point & ptTo)  // In: The point to snap to 
	{
	if (nWhichEnd == LEFT)
		Set(0, ptTo);
	else
		Set(Size() - 1, ptTo);

	RETURN_RC(CPolyLine::SnapEnd, SUCCESS);
	}
/*******************************************************************************/
RC CPolyLine::NextCusp(  // Return E_NOMSG if there is no cusp
	double t,		// In: A paramater value
	double & rCusp)	// Out: Parameter value of the next cusp after t, or curve-end
	{
	RC rc;
	int n = (int)floor(t) + 1;

	if (n >= Size() - 1)
		{
		rCusp = Size() - 1;
		rc = E_NOMSG;
		}
	else
		{
		rCusp = n;
		rc = SUCCESS;
		}
	RETURN_RC(CPolyLine::NextCustp, rc);
	}
/*******************************************************************************/
// Return a modeling ready slice of the curve
RC CPolyLine::NextSlice(	// Return E_NOMSG when it's the last slice
	int & i,			// In/out: An index marker
	PCurve & pSlice)	// Out: The current slice
	{
	pSlice = NULL;
	C3Point U;

	// Construct a modeling line from this segment
	ASSERT(i >= 0  || i < SegmentCount());
	CLine seg(Vertex(i), Vertex(i+1));
	RC rc = ALLOC_FAIL(pSlice = new CUnboundedLine(&seg));
	i++;

	if (i == SegmentCount()  &&  !rc)
		{
		// It's the last slice
		rc = E_NOMSG;
		delete this;
		}

	RETURN_RC(CPolyLine::NextSlice, rc);
	}
/*******************************************************************************/
// Persist this polyline
RC CPolyLine::Persist(
	PStorage pStorage) const	// In/Out: The storage
	{
	RC rc = pStorage->AcceptPolyLine(this);
	RETURN_RC(CPolyLine::Persist, rc);
	}

#ifdef CURVE_SPLITTING
/*******************************************************************************/
// Open if it's a closed periodic polyline
RC CPolyLine::OpenUp()
	{
	RC rc = SUCCESS;

	if (!m_bPeriodic)
		goto exit;	// 'cause we're done

	// Add the first vertex as the last
	if (rc = Add(Vertex(0)))
		QUIT
	m_bPeriodic = FALSE;
exit:
	RETURN_RC(CPolyLine::OpenUp, rc);
	}
/*****************************************************************************/
// Split
RC CPolyLine::Split(
	double rAt,              // In: Paramter value to split at
	SIDE nSide,              // In: Which side to keep (possibly both)
	PCurve & pFirst,         // Out: The first half
	PCurve & pSecond) const  // Out: The second half

/* NULL pieces are considered legal. If you try to split a polyline at one of
 * its ends then the pieces will be the a copy of the original polyline and a
 * null piece, and the operation will be considered successful. The caller must
 * therefore be preapred for NULL pieces.  The ends, for this matter, are the
 * untrimmed domain ends.
*/
    {
	int n;
	RC rc = FAILURE;
	double t;
	C3Point P;
	PPolyLine p = NULL;
	pFirst = pSecond = NULL;

	if (!PullIntoDomain(rAt)  ||
		!(nSide == LEFT  || nSide == RIGHT || nSide == BOTH) )
		// Do nothing, the input is bad
		goto exit;
		
	// Get a fresh copy
	if (rc = ALLOC_FAIL(p = (PPolyLine)Clone()))
		QUIT

	// Insert in it n additional point if necessary
	n = (int)floor(rAt);
	t = rAt - n;
	P = (1 - t) * Vertex(n) + t * Vertex(n+1);
	if (P == Vertex(n+1))
		n++;
	else if (P != Vertex(n))
		p->Insert(P, ++n);
    	
		 	
    if (Period() != 0)	// Periodic case
    	{
    	// Shift the domain interval to the splitting point
    	if (rc = p->StartAt(n))
    		QUIT

		// Make the polyline nonperiodic
		if (rc = p->OpenUp())
			QUIT;
	/*
	  We now have our one nonperiodic plyline.  It is considered both
	  first and second, so 2 returned pointers will point to the same
	  polyline if nSide=BOTH.
	 */
		if (nSide != RIGHT)
			pFirst = p;
		if (nSide != LEFT)
			pSecond = p;
		p = NULL;
		}
	else
		rc = CCurve::Split(rAt,nSide, pFirst, pSecond);
exit:
	delete p;
	RETURN_RC(CPolyLine::Split, rc);
    }
#endif
/**********************************************************************/
LOCAL BOOL UpdateNearestPoint( // Return TRUE if Dist < Margin
	C3Point & P,		 // In: Candidate nearest point
	double t,			 // In: Candidate's parameter
	double d,			 // In: Candidate's distance
	C3Point & ptNearest, // In: Current nearest point, updated here 
	double & rAt,		 // In: Parameter value there, uupdated here
	double & rDist,		 // In: Current distance, updated here
	double & rMargin,	 // In: Distance considered 0
	double ignore,		 // In: Parameter value to ignore
	RC & rc)			 // Out: SUCCESS if candidate was accepted
	{
	if (d < rDist && !FEQUAL(t, ignore, FUZZ_DIST))
		{
		rc = SUCCESS;
		ptNearest = P;
		rAt = t;
		rDist = d;
		return rDist < rMargin;
		}
	return FALSE;
	}
/**********************************************************************/
// Nearest point on the polyline to a given point
RC CPolyLine::PullBack(
	C3Point ptTo,    // In: The point we want to get closest to
	C3Point & ptNearest,  // Out: The nearest point on the curve
	double & rAt,		  // Out: Parameter value of Nearest
	double & rDist,		  // Out: The distance from the point to the curve
	double rMargin,		  // In: Distance considered 0 (optional)
	double ignore) const  // Optional: Parameter value to ignore
    {
	RC rc = E_NOMSG;
	rDist = INFINITY;

	for (int i = 1;   i < Size();   i++)
		{
		C3Point P;
		double t, d;

		// Look for a nearest point on the edge
		CLine line(Vertex(i-1), Vertex(i));
		if (!line.PullBack(ptTo, P, t, d))
			{
			t += (i - 1);
			if (UpdateNearestPoint(P, t, d, ptNearest, rAt, 
								rDist, rMargin, ignore, rc))
				break;
			}

		// Try the vertex
		P = ptTo - Vertex(i);
		d = P.Norm();
		if (UpdateNearestPoint(P, i, d, ptNearest, rAt, 
						  rDist, rMargin, ignore, rc))
			break;
		}

	RETURN_RC(CPolyLline::PullBack, rc);
	}
//////////////////////////////////////////////////////////////////////////
//																		//
//							Ray interceptions							//

// Implementation of CInterceptor
CInterceptor::CInterceptor()
	{
	m_pLine = NULL;
	m_c = 0;
	m_rSpeed = m_c = m_rValue = 0;
	m_bFirst = TRUE;
	}

CInterceptor::CInterceptor(
	PLine pLine,		// In: The line to be intercepted
	RC & rc)			// Out: Return code
	{
	m_bFirst = TRUE;
	m_pLine = pLine;

	// Get the line as an equation V*X - c = 0
	m_V = m_pLine->Velocity();
	m_V.TurnLeft();
	m_c = m_pLine->Origin() * m_V;

	// Get the line's speed
	m_rSpeed = pLine->Velocity() * pLine->Velocity();
	if (m_rSpeed < FUZZ_GEN)
		rc = E_ZeroVector;
	else
		rc = SUCCESS;
	}
/*************************************************************************/
// Look for an intersection between the current point and a new one
BOOL CInterceptor::FindIntercept(
	C3Point & ptNew,	// In: A point
	double & rOnLine,	// Out: Intersection parameter on the line
	double & ratio)		// Out: Ratio of intersection on the segment
	{
	double rNewValue = m_V * ptNew - m_c;

	BOOL bFound = !m_bFirst &&	  // This is not the first point, and 
		m_rValue * rNewValue < 0; // an intersection found in this segment
	if (bFound)
		{
		// The intersection falls between the projections, in ratio 
		// inverse to the ratio between heights
		double rCurrentProj = m_P * m_pLine->Velocity();
		double rNextProj = ptNew * m_pLine->Velocity();
		double one_ratio = fabs(m_rValue);
		ratio = fabs(rNewValue);
		double s = one_ratio + ratio;
		if (FZERO(s, FUZZ_GEN))
			// Segment is parallel to the ray, take its midpoint
			ratio = one_ratio = 0.5;
		else
			{
			ratio /= s;
			one_ratio /= s;		// = 1 - ratio
			}

		// This is the parameter on the line
		ASSERT(m_rSpeed >= FUZZ_GEN);
		rOnLine = ratio * rCurrentProj +  one_ratio * rNextProj - 
			m_pLine->Origin() * m_pLine->Velocity();
		rOnLine /= m_rSpeed;
		}
	else
		rOnLine = ratio = 0;

	// Update
	m_bFirst = FALSE;
	m_P = ptNew;
	m_rValue = rNewValue;

	return bFound;
	}
////////////////////////////////////////////////////////////////////
// Base class for the 2 polyline interceptors

class CPolyInterceptor	:	public CInterceptor
	{
public:
	// Constructors/destructors
	CPolyInterceptor()
		{
		}

	virtual ~CPolyInterceptor()
		{
		}

	CPolyInterceptor(
		PPolyLine pPoly,	// In: The polyline
		PLine pLine,		// In: The line to be intercepted
		RC & rc)			// Out: Return code
		:CInterceptor(pLine, rc)
		{
		m_pPoly = pPoly;
		}

	// Methods
	virtual RC ProcessOneIntercept(
		double rOnLine,		// In: Intercept parameter on the line
		double ratio)=0;	// In: Ratio on polyline segment (see below)

	// Traverse the intercepts
	RC Traverse()
		{
		RC rc = SUCCESS;
		double t, ratio;

		for (int i = 0;	  !rc &&  i < m_pPoly->Size();   i++)
			if (FindIntercept(m_pPoly->Vertex(i), t, ratio))
				
				// Do what you have to do with this intercept
				if (rc = ProcessOneIntercept(t, ratio))
					break;

		RETURN_RC(Traverse, rc);
		}

	// Data
protected:
	PPolyLine m_pPoly;		// The polyline
	};
/***********************************************************************/
// Intercepts counter
class CPolyInterceptsCounter	:	public CPolyInterceptor, 
									public CRealArray
	{
public:
	// Constructors/destructors
	CPolyInterceptsCounter()
		{
		}

	virtual ~CPolyInterceptsCounter()
		{
		}

	CPolyInterceptsCounter(
		PPolyLine pPoly,	// In: The polyline
		PLine pLine,		// In: The line to be intercepted
		RC & rc)			// Out: Return code
		:CPolyInterceptor(pPoly, pLine, rc)
		{
		}

	// Methods
	RC ProcessOneIntercept(
		double rOnLine,	// In: Intercept parameter on the line
		double ratio)	// In: Ratio on polyline segment (see below)
// The intercept is (1-t)*A + t*B, where A and B are the endpoints of the
// segment where the interception is located
		{
		int n;
		RC rc = SUCCESS;

		if (FEQUAL(ratio, 0, FUZZ_GEN)  || FEQUAL(ratio, 1, FUZZ_GEN)) 
			{
			// Vertex intercepts invalidate the entire test
			rc = E_NOMSG;
			QUIT
			}

		if (rOnLine < FUZZ_GEN)
			// The intersection is on the wrong side of the ray
			goto exit;

		// Is it a new intersections?
		if (!Search(rOnLine, FUZZ_GEN, n))
			{
			// Yes it is, add it to the list, and increment the count
			if (rc = Insert(rOnLine, n+1))
				QUIT
			}
exit:
		if (rc == E_NOMSG)
			rc = SUCCESS;
		RETURN_RC(ProcessOneIntercept::ProcessOneIntercept, rc);
		}

	// No data
	};
/***********************************************************************/
// Nearest and farthest intercepts finder
class CNearestPolyIntercept		:	public CPolyInterceptor
	{
public:
	// Constructors/destructors
	CNearestPolyIntercept()
		{
		m_pNearFore = m_pFarFore = m_pNearBack = m_pFarBack = NULL;
		m_bIgnoreOrigin = FALSE;
		}

	virtual ~CNearestPolyIntercept()
		{
		}

	CNearestPolyIntercept(
		PPolyLine pPoly,		// In: The polyline
		PUnboundedLine pLine,	// In: A line 
		BOOL bIgnoreOrigin,		// In: Ignore the origin if TRUE
		double * pNearFore,		// In/out: Nearest forward intercept so far
		double * pFarFore,		// In/out: Farthest forward intercept so far
		double * pNearBack,		// In/out: Nearest backword intercept so far
		double * pFarBack,		// In/out: Farthest backword intercept so far
		RC & rc)				// Out: Return code
		:CPolyInterceptor(pPoly, pLine, rc)
		{
		m_pNearFore = pNearFore;
		m_pFarFore = pFarFore;
		m_pNearBack = pNearBack;
		m_pFarBack = pFarBack;
		m_bIgnoreOrigin = bIgnoreOrigin;
		}

	// Methods
	RC ProcessOneIntercept(
		double rOnLine,	// In: Intercept parameter on the line
		double ratio)	// In: Ignored here
		{
		UpdateNearAndFar(rOnLine, m_bIgnoreOrigin, m_pNearFore, 
						 m_pFarFore, m_pNearBack, m_pFarBack);
		RETURN_RC(CNearestPolyIntercept::ProcessOneIntercept, SUCCESS);
		}
	// Data
protected:
	double * m_pNearFore;	// The nearest forward intercept so far
	double * m_pFarFore;	// The farthest forward intercept so far
	double * m_pNearBack;	// The nearest backword intercept so far
	double * m_pFarBack;	// The farthest backword intercept so far
	BOOL m_bIgnoreOrigin;	// Ignore intercepts at the origin if TRUE
	};
/*******************************************************************************/
// Update the number of intercpts with a given ray
RC CPolyLine::UpdateIntercepts(
	PRay pRay,			// In: A ray 
	int	& nIntercepts)  // In/out: The number of intercepts
	{
	RC rc;
	CPolyInterceptsCounter interceptor(this, pRay, rc);
	if (rc)
		QUIT
	if (rc = interceptor.Traverse())
		QUIT
	nIntercepts += interceptor.Size();
exit:
	RETURN_RC(CPolyLine::UpdateIntercepts, rc);
	}
/*******************************************************************************/
// Update the nearest and farthest intercpts with a given line
RC CPolyLine::UpdateIntercepts(
	PUnboundedLine pLine, // In: A line 
	BOOL bIgnoreOrigin,	  // In: Ignore the origin if TRUE
	double * pNearFore,   // In/out: Forward intercept nearest to line's origin
	double * pFarFore,    // In/out: Forward intercept farthest from line's origin
	double * pNearBack,	  // In/out: Backward intercept nearest to line's origin
	double * pFarBack)    // In/out: Backward intercept farthest from line's origin
	{
	RC rc;
	CNearestPolyIntercept interceptor(this, pLine, bIgnoreOrigin, pNearFore, 
									  pFarFore, pNearBack, pFarBack, rc);
	if (!rc)
		rc = interceptor.Traverse();
	RETURN_RC(PolyLine::UpdateIntercepts, rc);
	}
//////////////////////////////////////////////////////////////////////////
//		Domain Manipulation
/*
 Our domain is always [i,i+1] for the ith segment, so setting the domain or
 one of its ends is undefined.  The problem with that is that the user may
 try to set the domain, say for linearizing a part of the curve, and then 
 reset it to the original domain.  The results will be incorrect. 
 That's whey SetDomain is NOT implemented, but we need ResetStart and
 ResetEnd for corner rounding when rendering.  For those we cheat, moving
 the line's start or end  POINT, leaving the domain at [0,1].  
 */
/*****************************************************************************/
// Reset the domain start
RC CPolyLine::ResetStart(
	double rNew)  // In: New curve start
	{
	// Redefine the first vertex
	int i;
	if (rNew > 0)
		{
		i = Localize(rNew);
		Vertex(i) = (1 - rNew) * Vertex(i) + rNew * Vertex(i+1);
		if (i > 0)
			Remove(0, i);
		}
	RETURN_RC(CPolyLine::::ResetStart, SUCCESS);
	}
/*****************************************************************************/
// Reset the domain end
RC CPolyLine::ResetEnd(
	double rNew)  // In: New curve start
	{
	// Redefine the last vertex
	if (rNew < SegmentCount())
		{
		int i = Localize(rNew);
		if (i < FUZZ_GEN)
			{
			i--;
			rNew = 1;
			}
		Vertex(i+1) = (1 - rNew) * Vertex(i) + rNew * Vertex(i+1);
		int n = i - Size() - 2;
		if (i < n)
			Remove(i+2, n);
		}

	RETURN_RC(CPolyLine::ResetEnd, SUCCESS); 
    }
/************************************************************************/
// Linearize a part of the curve
RC CPolyLine::LinearizePart(
	double rFrom,			// In: Where to start
	double rTo,				// In: Where to stop
	double rResolution,		// In: Chord-height toelrance
	PAffine pTransform,		// In: Rendering transformation (NULL OK)
	CCache & cCache) const	// In: The cache we render into
	{
	RC rc = SUCCESS;
	C3Point P;

	int i = Localize(rFrom);
	int n = Localize(rTo);

	// Generate the first point if the cache is empty
	if (!cCache.Started())
		{
		if (rFrom < FUZZ_GEN)
			P = Vertex(i);
		else
			P = (1 - rFrom) * Vertex(i) + rFrom * Vertex(i+1);
		if (pTransform)
			pTransform->ApplyToPoint(Vertex(0), P);
		if (rc = cCache.Accept(P, 0))
			QUIT
		}

	// Generate intermediate points
	for (i++;  i <= n;  i++)
		{
		if (pTransform)
			pTransform->ApplyToPoint(Vertex(i), P);
		else 
			P = Vertex(i);

		// Generate the first point if the cache is empty
		if (rc = cCache.Accept(P, 0))
			QUIT
		}

	// Last point
	if (rTo > 1 - FUZZ_GEN)
		P = Vertex(n+1);
	else
		P = (1 - rTo) * Vertex(n) + rTo * Vertex(n+1);
	if (pTransform)
		pTransform->ApplyToPoint(P, P);
	if (rc = cCache.Accept(P, 0))
		QUIT
exit:	
	RETURN_RC(CPolyLine::LinearizePart, rc);
	}
/************************************************************************/
// Reverse the orientation
RC CPolyLine::Reverse()
    {
	// Invoke the CPointArray method
	RC rc = ReverseOrder();
	RETURN_RC(CCurve::Reverse, rc);
    }
/*******************************************************************/
// How complex is this polyline?
int CPolyLine::Complexity() const
	{
	return Size();
	}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Check if the given point lies inside this polygon or on its boundary.
 *			Note, it suppose the polygon is closed (the 1st vertex is coincident
 *			to the last one) without intersections of edges.
 * Returns:	True if given point lies inside the polygon or on its boundary.
 ********************************************************************************/
bool
CPolyLine::IsPointInside
(
 const C3Point& PointToCheck
)const
{
// slip a horizontal ray from given point and count its intersections with polygon edges
	int		IntersectionsCount = 0,
			Vert1, Vert2,
			PointsCount = Size() - 1;
	double	tmp1, tmp2;

	for(Vert1 = 0; Vert1 < PointsCount; Vert1++)
	{
		Vert2 = (Vert1 + 1) % PointsCount;

		// checking of belonging to an edge
		if( fabs(PointToCheck.X() - m_ptEntries[Vert1].X()) <= FUZZ_GEN                        &&
			fabs(PointToCheck.X() - m_ptEntries[Vert2].X()) <= FUZZ_GEN                        &&
			PointToCheck.Y() >= min(m_ptEntries[Vert1].Y(), m_ptEntries[Vert2].Y()) - FUZZ_GEN &&
			PointToCheck.Y() <= max(m_ptEntries[Vert1].Y(), m_ptEntries[Vert2].Y()) + FUZZ_GEN )
			return true;	// the point lies on an vertical edge
		else if(fabs(PointToCheck.Y() - m_ptEntries[Vert1].Y()) <= FUZZ_GEN                   &&
			fabs(PointToCheck.Y() - m_ptEntries[Vert2].Y()) <= FUZZ_GEN                        &&
			PointToCheck.X() >= min(m_ptEntries[Vert1].X(), m_ptEntries[Vert2].X()) - FUZZ_GEN &&
			PointToCheck.X() <= max(m_ptEntries[Vert1].X(), m_ptEntries[Vert2].X()) + FUZZ_GEN )
			return true;	// the point lies on an horizontal edge
		else { // check if the point is lying on an arbitrary edge. Bug #78328 fix - Stephen Hou, 8/10/2003 
			double x21 = m_ptEntries[Vert2].X() - m_ptEntries[Vert1].X();
			double y21 = m_ptEntries[Vert2].Y() - m_ptEntries[Vert1].Y();
			double xx1 = PointToCheck.X() - m_ptEntries[Vert1].X();
			double yy1 = PointToCheck.Y() - m_ptEntries[Vert1].Y();
			if (fabs(yy1 / xx1 - y21 / x21) <= FUZZ_GEN) {  // point on the line of edge
				double tx = xx1 / x21;
				double ty = yy1 / y21;
				if (tx >= 0.0 && tx <= 1.0 && ty >= 0.0 && ty <= 1.0) 
					return true;					// the point lies on an arbitrary edge
			}
		}
		// for checking of strong belonging to the interior (not on an edge)
		if( (fabs(m_ptEntries[Vert1].Y() - m_ptEntries[Vert2].Y()) <= FUZZ_GEN)                      ||	// horizontal edge
			(m_ptEntries[Vert1].Y() > (tmp1 = PointToCheck.Y() + FUZZ_GEN) && m_ptEntries[Vert2].Y() > tmp1) ||	// the edge lies above the ray
			(m_ptEntries[Vert1].Y() < (tmp2 = PointToCheck.Y() - FUZZ_GEN) && m_ptEntries[Vert2].Y() < tmp2) )	// the edge lies below the ray
			continue; 

		if( (m_ptEntries[Vert1].Y() > m_ptEntries[Vert2].Y()             &&
			 fabs(PointToCheck.Y() - m_ptEntries[Vert1].Y()) <= FUZZ_GEN &&
			 PointToCheck.X() < m_ptEntries[Vert1].X())                    ||
			(m_ptEntries[Vert2].Y() > m_ptEntries[Vert1].Y()             &&
			 fabs(PointToCheck.Y() - m_ptEntries[Vert2].Y()) <= FUZZ_GEN &&
			 PointToCheck.X() < m_ptEntries[Vert2].X()) ) {

			// The intersection can be a local minimum or maximum point, which should not be
			// counted - Bug #78328 fix: Stephen Hou, 8/10/2003

			double yPrev, yNext;
			if (m_ptEntries[Vert1].Y() > m_ptEntries[Vert2].Y()) {
				yNext = m_ptEntries[Vert2].Y();
				yPrev = (Vert1 != 0) ? m_ptEntries[Vert1 - 1].Y() : m_ptEntries[PointsCount].Y();
			}
			else {
				yNext = (Vert2 != PointsCount) ? m_ptEntries[Vert2 + 1].Y() : m_ptEntries[0].Y();
				yPrev = m_ptEntries[Vert1].Y();
			}
			if (yPrev < (tmp1 = PointToCheck.Y() - FUZZ_GEN) && yNext < tmp1 ||	// a local max point
				yPrev > (tmp2 = PointToCheck.Y() + FUZZ_GEN) && yNext > tmp2)	// a local min point
				continue;		
			else
				IntersectionsCount++;																		
		}
		else {
			if(fabs(min(m_ptEntries[Vert1].Y(), m_ptEntries[Vert2].Y()) - PointToCheck.Y()) <= FUZZ_GEN)
				continue;																				// the ray goes thru the lower vertex of the edge
			else
				if( m_ptEntries[Vert1].X() +
					(PointToCheck.Y() - m_ptEntries[Vert1].Y()) / (m_ptEntries[Vert2].Y() - m_ptEntries[Vert1].Y()) *
					(m_ptEntries[Vert2].X() - m_ptEntries[Vert1].X())
					> PointToCheck.X())
					IntersectionsCount++;
		}												// the ray intersects the edge
	}
	return !!(IntersectionsCount & 1);
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Check if this polygon has clockwise orientation on the XY plane.
 *			Note, it suppose the polygon is closed (the 1st vertex is coincident
 *			to the last one) without intersections of edges.
 * Returns:	True in clockwise orientation case.
 ********************************************************************************/
bool
CPolyLine::IsClockwiseProjection() const
{
	int		Vert1 = Size() - 1, Vert2 = Size() - 1;
	double	DoubleArea = 0.0;

    for( ; Vert1--; --Vert2)
        DoubleArea += (m_ptEntries[Vert2].X() - m_ptEntries[Vert1].X()) * (m_ptEntries[Vert1].Y() + m_ptEntries[Vert2].Y());

	return DoubleArea > 0.0;
}


/*********************************************************************************
 *
 * Compute the length of a portion of a curve between two points.
 *
 * @Author: Stephen W. Hou
 * @Date:   April 8, 2004
 *********************************************************************************/
RC 
CPolyLine::GetLength(double rFrom,		// In: Lower integration bound
				     double rTo,        // In: Upper integration bound
				     double &rLength) const // Out: The length
{
	ASSERT(rFrom <= rTo);

	RC rc = SUCCESS;
	if (rFrom == rTo) 
		rLength = 0.0;
	else {
		double length;
		bool count = false;
		for (int i = 1; i < Size(); i++) {
			double from = rFrom - i + 1;
			if (from >= 0 && from <= 1.0) {
				CLine line(Vertex(i-1), Vertex(i));
				rc = line.GetLength(from, line.End(), rLength);
				if (rc)
					break;

				count = true;
			}
			double to = rTo - i + 1;
			if (to >= 0 && to <= 1.0) {
				CLine line(Vertex(i-1), Vertex(i));
				rc = line.GetLength(line.Start(), to, length);
				if (!rc) 
					rLength += length;

				break;
			}
			else {
				if (count) {
					CLine line(Vertex(i-1), Vertex(i));

					rc = line.Length(length);
					if (rc) 
						break;

					rLength += length;
				}
			}
		}
	}
	RETURN_RC(CPolyLline::GetLength, rc);
}


/*********************************************************************************
 *
 * Find a point on the curve by an arc length from the start of the 
 * curve.
 *
 * @Author: Stephen W. Hou
 * @Date:   April 8, 2004
 *********************************************************************************/
RC 
CPolyLine::FindPointByLength(double rLength,
		                     double &rParam,
						     C3Point &cPoint) const
{
	double curveLength;
	Length(curveLength);

    ASSERT(rLength >= 0.0 && rLength <= curveLength);

	RC rc;
	if (rLength == 0.0) {
		rParam = 0.0;
		rc = GetStartPoint(cPoint);
	}
	else if (rLength == curveLength) {
		rParam = 1.0;
		rc = GetEndPoint(cPoint);
	}
	else {
		double arcLength = rLength;
		for (int i = 1; i < Size(); i++) {
			CLine line(Vertex(i - 1), Vertex(i));
			
			double length;
			line.Length(length);
			if (arcLength > length) 
				arcLength -= length;
			else if (arcLength == length) 
				cPoint = Vertex(i);
			else {
				double param;
				rc = line.FindPointByLength(arcLength, param, cPoint);
				if (!rc) 
					rParam = double(i - 1) + param;
			}
		}	
    }

    return rc;
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
PCurve CPolyLine::ProjectTo(const CPlane & plane, const C3Vector & dir, bool get2dCurve) const
{
	C3Point point;
    CPolyLine* pPolyline = new CPolyLine;
    pPolyline->m_2dCurve = get2dCurve;
    C3Vector along;
    if (&dir == NULL) {
        CPlane::PLANETYPE type = plane.PlaneType();
        if (type== CPlane::TYPE_Xy) {
	        for (register i = 0; i < Size(); i++) {
                point = (*this)[i];
                point.SetZ(0.0);
		        pPolyline->Add(point);
	        }            
            return pPolyline;
        }
        else if (type == CPlane::TYPE_Yz) {
	        for (register i = 0; i < Size(); i++) {
                point = (*this)[i];
                if (get2dCurve) {
                    double u = point.Y();
                    double v = point.Z();
                    point.SetX(u);
                    point.SetY(v);
                    point.SetZ(0.0);
                }
                else
                    point.SetX(0.0);
		        pPolyline->Add(point);
	        }            
            return pPolyline;
        }
        else if (type == CPlane::TYPE_Zx) {
	        for (register i = 0; i < Size(); i++) {
                point = (*this)[i];
                if (get2dCurve) {
                    double u = point.Z();
                    double v = point.X();
                    point.SetX(u);
                    point.SetY(v);
                    point.SetZ(0.0);
                }
                else
                    point.SetY(0.0);
		        pPolyline->Add(point);
	        }            
            return pPolyline;
        }
        else 
            along = plane.GetNormal().Negate();
    }
    else
        along = dir;

	for (register i = 0; i < Size(); i++) {
		plane.ProjectPoint((*this)[i], along, point, get2dCurve);
		pPolyline->Add(point);
	}
	return pPolyline;
}


