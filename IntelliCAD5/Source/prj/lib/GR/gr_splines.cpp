/*	splines.CPP - Implements some Geo-IntelliCAD interface
 *	Copyright (C) 1997-98 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * This file knows about both Geo and sds
 */

// Intellicad stuff
#include "gr.h"
#include "icadlib.h"

#undef	X
#undef	Y
#include <windows.h>

#define CMD_ERR_OFFSHARPCURVE 933	// A duplicates cmds.h definition 
									// unfortunately not seen from here
// Geo stuff
#ifndef INTELLICAD
	#define INTELLICAD
#endif

#define THREE_D
#include "Geometry.h"
#include "TheUser.h"
#include "Point.h"
#include "Knots.h"		// Knots
#include "CtrlPts.h"	// Control points
#include "SqArray.h"	// Square matrix for linear equations soloving
#include "Curve.h"
#include "NURBS.h"
#include "CrvOfset.h"
#include "CrvPair.h"
#include "EllipArc.h"

// The exported functions
#include "splines.h"
#include "SplineUtils.h"	

DB_SOURCE_NAME;

//////////////////////////////////////////////////////////////////////////
//							sds point utilities							//
//////////////////////////////////////////////////////////////////////////
LOCAL void RptToSds(
	C3Point & P,	// I: Geo-style point
	sds_point q)	// O: sds-style point
	{
	q[0] = P.X();
	q[1] = P.Y();
	q[2] = P.Z();
	}

LOCAL void SdsToRpt(
	sds_point q,	// O: sds-style point
	C3Point & P)	// O: Geo-style point
	{
	P = C3Point(q[0], q[1], q[2]);
	}

//////////////////////////////////////////////////////////////////////////
//						Definition of sds_Cache							//
//////////////////////////////////////////////////////////////////////////
class sds_Cache	:	public CCache
	{
public:
	sds_Cache();

	virtual ~sds_Cache();

	sds_Cache(
		int * nverts_p, 
		sds_point ** vertex_pp, 
		RC & rc);

	RC MakeRoom();

	RC Accept(
		const C3Point & Point,   // In: The point to add
		double rDenominator);         // In: Parametner value (ignored here)

	RC Add(
		sds_point point);   // In: The point to add

	RC YieldPoints(
		int * nverts_p,			 // Out: Number of points 
		sds_point ** vertex_pp); // Out: Array of points

	BOOL Started()
		{
		return m_nCount > 0;
		}

protected:
	// Data
	int m_nAllocated;		// Number of points allocated
	int m_nCount;			// Number of points cached so far
	sds_point * m_pPoints;	// The points
	};

//////////////////////////////////////////////////////////////////////////
//						Implementation of sds_Cache						//
//////////////////////////////////////////////////////////////////////////

//Default constructor an destructor
sds_Cache::sds_Cache()
	{
	m_nAllocated = m_nCount = 0;
	m_pPoints = NULL;
	}
sds_Cache::~sds_Cache()
	{
	delete [] m_pPoints;
	}
/************************************************************************/
// Construct from an ICAD point array
RC sds_Cache::YieldPoints(
	int * nverts_p,			// Out: Number of points 
	sds_point ** vertex_pp) // Out: Array of points
	{
	RC rc = SUCCESS;

	// Count
	if (nverts_p)
		*nverts_p = m_nCount;
	else
		rc = E_BadInput;

	// Points
	if (!vertex_pp)
		rc = E_BadInput;
	else
		*vertex_pp = m_pPoints;

	// Reset
	m_pPoints = NULL;
	m_nCount = m_nAllocated = 0;

	RETURN_RC(sds_Cache::YieldPoints, rc);
	}
/************************************************************************/
// Make room for an additional point
RC sds_Cache::MakeRoom()
	{
	RC rc = SUCCESS;

	if (m_nAllocated <= m_nCount)
		{
		m_nAllocated += 20;
		if (m_pPoints)
		{
			sds_point *temp;
			temp = new sds_point[m_nAllocated];
			memcpy(temp, m_pPoints, sizeof(sds_point) * (m_nAllocated - 20));
			delete [] m_pPoints;
			m_pPoints = temp;
		}
		else
		{
			m_pPoints = new sds_point[m_nAllocated];
			memset(m_pPoints,0,sizeof(sds_point)*m_nAllocated);

		}
		if (rc = ALLOC_FAIL(m_pPoints))
			QUIT
		}
exit:
	RETURN_RC(sds_Cache::MakeRoom, rc);
	}
/************************************************************************/
// Accept a point
RC sds_Cache::Accept(
	const C3Point & Point,   // In: The point to add
	double rDenominator)          // In: Denominator
	{
	C3Point P = Point;
	RC rc = MakeRoom();
	if (rc)
		QUIT

	// Add the point
	RptToSds(P, m_pPoints[m_nCount++]);
exit:
	RETURN_RC(sds_Cache::Accept, rc);
	}
/************************************************************************/
// Accept a point
RC sds_Cache::Add(
	sds_point point)   // In: The point to add
	{
	RC rc = MakeRoom();
	if (rc)
		QUIT

	// Add the point
	ic_ptcpy(m_pPoints[m_nCount], point);
	m_nCount++;
exit:
	RETURN_RC(sds_Cache::Add, rc);
	}
//////////////////////////////////////////////////////////////////////////
//							Spline Constructions						//
//////////////////////////////////////////////////////////////////////////

// Construct a spline from sds representation
RC GetSplineFrom_sds(
    int         deg,		// In: Degree
    int         nknots,		// In: Number of knots 
    double		*knot,		// In: Knots
    int         ncpts,		// In: Number of control points
    sds_point  *cpt,		// In: Control points
    double		*wt,		// In: Weights (NULL OK)
    PSpline &	pSpline)	// Out: The spline
	{
	INT i;
	RC rc;
	PKnots pKnots = NULL;
	PControlPoints pControlPoints = NULL;
	PRealArray pWeights = NULL;
	pSpline = NULL;

	// Check input validity
	if (nknots <= ncpts || 
		deg < 1	||  deg > MAX_DEGREE  ||
		!knot	|| !cpt  ||  !wt)
		{
		rc = E_BadInput;
		QUIT
		}

	// Costruct the knots
	pKnots = new CKnots(deg, ncpts, rc);
	if (!pKnots)
		rc = DM_MEM;
	if (rc)
		QUIT

	// Construct the control points
	pControlPoints = new CControlPoints(ncpts, rc);
	if (!pControlPoints)
		rc = DM_MEM;
	if (rc)
		QUIT

	if (wt)
		{
		// Construct the weights
		pWeights = new CRealArray(ncpts, rc);
		if (!pWeights)
			rc = DM_MEM;
		if (rc)
			QUIT
		}

	// Copy the knots, weights and control points
	for (i = 0;  i < ncpts;  i++)
		{
		(*pKnots)[i] = knot[i];

		// Store the point in homogeneous form (multiplied by weight)
		C3Point P(cpt[i][0], cpt[i][1], cpt[i][2]);
		if (wt)
			{
			P *= wt[i];
			(*pWeights)[i] = wt[i];
			}

		pControlPoints->Set(i, P);
		}

	// Last knot
	pKnots->Set(ncpts, knot[ncpts]);

	// Construc the spline
	if (wt)
		pSpline = new CNURBS(pKnots, pControlPoints, pWeights, rc);
	else
		pSpline = new CSpline(pKnots, pControlPoints,rc);
	if (!pSpline)
		rc = DM_MEM;
	else
		{ // Now owned by the spline
		pKnots = NULL;
		pControlPoints = NULL;
		pWeights = NULL;
		}
exit:
	if (rc)
		{
		delete pKnots;
		delete pControlPoints;
		delete pWeights;
		delete pSpline;
		pSpline = NULL;
		}
	return rc;
	}
/************************************************************************/
// Construct interpolating an array points with given end-tangents
//LOCAL RC GetFittingSpline(
__declspec( dllexport) RC
GetFittingSpline(
	INT nPoints,			// In: Number of points, must be no less than 2
	sds_point * pPoints,	// In: Array points
	double rFittingTolerance,	// In: Fitting tolerance
    sds_point sdsStart,		// In/out: Starting tangent direction
    sds_point sdsEnd,		// In/out: Ending tangent direction
	BOOL bPeriodic,			// In: Make it periodic, if TRUE 
	PSpline & pCurve)		// Out: The interpolating spline
    {
/*
 * This constructor creates a spline that interpolates the given points.
 * The parameterization is the so-called arc-length parameterization:  The 
 * parameter value assigned to Pi is obtained from that of Pi-1 by adding the
 * distance between the 2 pointsto.  The interpolation equations would break 
 * with this parameterization if Pi = P-+1, so here we ignore Pi+1 if  
 * th edistance between Pi and Pi+1 ia less than rFittingTolerance.
 * The end-conditions depend on the input vectors sdsStart and sdsEnd: 
 * for a nonzeor vector, the derivative at that end will be equal to the given
 * vector. A zero tangent will trigger the so-called "Natural" end condition,
 * which means that the curvature of the spline at that end will be 0.
 *
 * If bPeriodic, then the start and end tangents are computed and returned here
 */
	RC rc;
	INT i, n;
	double r = 0;
	C3Point ptCurrent, ptPrevious, ptStart, ptEnd;
	PKnots pKnots = NULL;
	PControlPoints pControlPoints = NULL;
	PSpline pPeriodic = NULL;
	BOOL bGivenStart, bGivenEnd;

	if (rFittingTolerance < FUZZ_GEN)
		rFittingTolerance = FUZZ_GEN;
	
	// Analize the end tangents
	SdsToRpt(sdsStart, ptStart);
	bGivenStart = ptStart.Norm() > FUZZ_GEN;
	SdsToRpt(sdsEnd, ptEnd);
	bGivenEnd = ptEnd.Norm() > FUZZ_GEN;

	// Construct knots and control points
	pKnots = new CKnots(3, 1, rc);
	if (!pKnots)
		rc = DM_MEM;
	if (rc)
    	QUIT
	if (rc = ALLOC_FAIL(pControlPoints = new CControlPoints))
    	QUIT
	
	// Load the first knot and point
	pKnots->Set(0, r);
	SdsToRpt(pPoints[0], ptPrevious);
	if (rc = pControlPoints->Add(ptPrevious))
		QUIT

	// Load the remaining knots and points
	for (i = 1;  i < nPoints;  i++)
		{
		SdsToRpt(pPoints[i], ptCurrent);
		double dr = PointDistance(ptPrevious, ptCurrent);
		if (FZERO(dr, rFittingTolerance))	
			continue;  // Igonre it, it's a duplicate point
		if (rc = pKnots->Add((r += dr)))
			QUIT
		if (rc = pControlPoints->Add(ptCurrent))
			QUIT
		ptPrevious = ptCurrent;
		}

	// Must have at least 2 points
	if ((n = pControlPoints->Size()) < 2)
		{
		// Not enough points to interpolate
		rc = E_BadInput;
		QUIT
		}
	
	// Make the last knot an implied one
	r = pKnots->Last();
	pKnots->Remove(pKnots->Size()-1, 1);
	pKnots->SetInterval(r);

	if (bPeriodic && !bGivenStart) {
		// Use central difference to compute end conditions  - SWH 4/24/2004
		//
		C3Point dir1((*pControlPoints)[1] - (*pControlPoints)[0]);
		dir1.Unitize();

		C3Point dir2((*pControlPoints)[0] - (*pControlPoints)[pControlPoints->Size() - 2]);
		dir2.Unitize();

		ptStart = (dir1 + dir2) / 2.0;
		ptStart.Unitize();
		ptEnd = ptStart;

		bGivenStart = bGivenEnd = true;
		RptToSds(ptStart, sdsStart);
		RptToSds(ptStart, sdsEnd);
	}
	if (rc = pKnots->Insert(0.0, 0, 3))
		QUIT

	// Set up the interpolation equations
	{
	n += 2; // Add room for the 2 end conditions
	CBandedArray A(n,2,rc);
	if (rc)
		QUIT

	// End conditions
	if (bGivenStart)
		{
		// Prescribed tangent end condition
		ptStart *= (.333333333333333 * (pKnots->Knot(4) - pKnots->Knot(3)));
		A[1][0] = -1.0;
		}
	else
		{
		// Natural end condition
		double s = pKnots->Knot(4) - pKnots->Knot(3);
		double t = pKnots->Knot(5) - pKnots->Knot(3);
		r = s + t;
		A[1][0] = -t / r;
		A[1][2] = -s / r;
		ptStart = C3Point(0, 0, 0);
		}
	
	// In any case
	pControlPoints->Insert(ptStart, 1);

	if (bGivenEnd)
		{
		// Prescribed tangent end condition
		ptEnd *= (-.333333333333333 * (pKnots->Knot(n) - pKnots->Knot(n-1)));
		A[n-2][n-1] = -1.0;
		}
	else
		{
		// Natural end condition
		double s = pKnots->Knot(n) - pKnots->Knot(n-1);
		double t = pKnots->Knot(n) - pKnots->Knot(n-2);
		r = s + t;
		A[n-2][n-1] = -t / r;
		A[n-2][n-3] = -s / r;
		ptEnd = C3Point(0, 0, 0);
		}

	// In any case
	pControlPoints->Insert(ptEnd, n-2);
	A[0][0] = A[n-1][n-1] = A[1][1] = A[n-2][n-2] = 1.0;
	
	// The rest of the array is straight interpolation conditions
	for (i=2;  i<n-2;  i++)
		pKnots->EvaluateBasis (i+1, pKnots->Knot(i+2), &A[i][i-2]);

    // Solve the equations
    if (rc = A.LUFactorNoPivot())
    	QUIT
    if (rc = A.LUSolveNoPivotP(*pControlPoints))
		QUIT
	}

	// Construct the spline
	pCurve = new CSpline(pKnots, pControlPoints, rc);
	if (!pCurve)
		rc = DM_MEM;
	else
		{
		// Now owned by the spline
		pKnots = NULL;
		pControlPoints = NULL;  
		}
exit:
	if (rc)
		{
	    delete pCurve;
		pCurve = NULL;
		}
	delete pPeriodic;
	RETURN_RC(GetFittingSpline, rc);
	}
/************************************************************************/
// ICAD result from Visio RC
LOCAL int Result(	// Return ICAD result
	RC rc)	// In: Visio return code
	{
	switch (rc)
		{
		case SUCCESS:			return 0;
		case E_BadInput:		return -1;		
		case DM_MEM:			return -2;
		case E_BadOffset:		return CMD_ERR_OFFSHARPCURVE;
		default:				return -3;
		}
	}
/************************************************************************/
// Free and set to null
LOCAL void FreeAndNull(
	void ** p)
	{
	if (p)
		if (*p)
			{
			delete *p;
			*p = NULL;
			}
	}
/************************************************************************/
// Generate the rendering points
LOCAL RC GeneratePoints(
	PSpline 	pCurve,		// In: A spline
    double		resocoeff,	// In: ICAD-style resoulution coefficient 
    int        *nverts_p,	// Out: Number of points generated
    sds_point **vertex_pp)	// Out: The points
	{
	RC rc = SUCCESS;

	// Construct the receiving stream
	sds_Cache cPoints;

	// Compute the pixel-size resolution
	resocoeff = 2 * resocoeff * resocoeff;
	if (FZERO(resocoeff, FUZZ_GEN))
		{
		rc = E_BadInput;
		QUIT
		}
	resocoeff = 1 / resocoeff;

	// Tesseleate the curve
	if (rc = pCurve->Linearize(resocoeff, NULL, cPoints))
		QUIT
	rc = cPoints.YieldPoints(nverts_p, vertex_pp);
exit:
	if (rc)
		{
		// Clean up
		FreeAndNull((void**)vertex_pp);
		if (nverts_p)
			*nverts_p = 0;
		}
	return Result(rc);
	}
//////////////////////////////////////////////////////////////////////////
//																		//
//							Public Interface							//
//																		//
//////////////////////////////////////////////////////////////////////////

// Tesselate a NURBS curve
int gr_splinegen1(

  /* Given: */           /* DXF code */

    int         deg,     /*    71    */
    int         flags,   /*    70    */

    int         nknots,  /*    72    */
    double		*knot,   /*    40    */
    double		ktol,    /*    42    */

    int         ncpts,   /*    73    */
    sds_point  *cpt,     /*    10    */
    double		*wt,     /*    41    (NULL OK) */ 
    double	    ctol,    /*    43    */

    double		resocoeff,

  /* Results: */

    int        *nverts_p,
    sds_point **vertex_pp)
/*
**
**        deg : Degree.
**      flags : A bit-coded int, assigned as follows:
**
**                xxxxxRPC
**                76543210
**
**                C (bit 0, value 1) : Closed
**                P (bit 1, value 2) : Periodic
**                R (bit 2, value 4) : Rational (at least 1 non-1.0 wt?)
**
**     nknots : The number of knot values (knot).
**       knot : Array of knot values (nknots elements).
**       ktol : Knot tolerance
**
**      ncpts : The number of control points (cpt) and the number
**                of associated weights (wt).
**        cpt : Array of control points (ncpts elements).
**         wt : Array of weights for each control point (ncpts elements).
**       ctol : Control point tolerance.
**
**  resocoeff : See description in gr_splinegen2().
**
**   nverts_p : This function must set *nverts_p to the number of elements
**                allocated for the *vertex_pp array.  (nverts_p is the address
**                of the caller's int variable for the number of
**                vertices.)
**  vertex_pp : This function must allocate and assign an array of
**                points and assign the address of that array to *vertex_pp.
**                (vertex_pp is the address of the caller's pointer to an
**                sds_point.)
**
**  Returns:
**       0 : OK
**      -1 : Bad calling parameter(s)
**      -2 : Insufficient memory
**      -3 : Anything else
*/
	{
	RC rc = SUCCESS;

	// Construct the NURBS curve
	PSpline pCurve = NULL;
	if (rc = GetSplineFrom_sds(deg, nknots, knot, ncpts, cpt, wt, pCurve))
		QUIT

	// Tesseleate it
	if (vertex_pp)
		rc = GeneratePoints(pCurve, resocoeff, nverts_p, vertex_pp);
exit:
	delete pCurve;
	return Result(rc);
	}
/************************************************************************/
// Construct an interpolating NURBS curve, and tesselate it
int gr_splinegen2(

  /* Given: */              /* DXF code */

    int         deg,        /*    71    */

    int         nfpts,      /*    74    */
    sds_point  *fpt,        /*    11    */
    double		ftol,       /*    44    */

    sds_point   btandir,    /*    12    */
    sds_point   etandir,    /*    13    */
	BOOL bPeriodic,			/* 70, Bit 2 */

    double		resocoeff,

  /* Results: */

    int        *nverts_p,
    sds_point **vertex_pp,

    int        *ncpts_p,     /*    73    */
    sds_point  **cpt_pp,     /*    10    */

    int        *nknots_p,    /*    72    */
    double	   **knot_pp)	 /*    40    */
/*
**
**        deg : Degree.
**
**      nfpts : The number of fit points (fpt).
**        fpt : Array of fit points (nfpts elements).
**       ftol : Fit tolerance.
**
**    btandir : The beginning tangent direction vector.
**                The zero-vector means "unspecified".
**    etandir : The ending    tangent direction vector.
**                The zero-vector means "unspecified".
**
**	bPeriodic : =TRUE if the spline is to be periodic.  In that case,
**				  the tangent directions are ignored, and are rewritten
**				  by the tangents of the generated spline
**
**  resocoeff : Resolution coefficient.  Basically, the square root
**                of the number of pixels per drawing unit at the
**                current zoom (adjusted for the effect of scaling of
**                blocks containing the entity).  We use it in the
**                formula
**
**                  n = (int)(resocoeff*sqrt(d));
**
**                where d is the distance between two adjacent
**                definition points in the spline's native coordinate
**                system, and n is the number of points to generate
**                between them.
**
**                This or some similar formula should probably
**                be used; it's similar to ACAD's circle resolution
**                formula.
**
**   nverts_p : This function must set *nverts_p to the number of elements
**                allocated for the *vertex_pp array.  (nverts_p is the address
**                of the caller's int variable for the number of
**                vertices.)
**  vertex_pp : This function must allocate and assign an array of
**                points and assign the address of that array to *vertex_pp.
**                (vertex_pp is the address of the caller's pointer to an
**                sds_point.)
**
**    ncpts_p : If possible, this function should set *ncpts_p to the number
**                of elements  allocated for the *cpt_pp array.  (ncpts_p is
**                the address of the caller's int variable for the
**                number of control points.)
**     cpt_pp : If possible, this function should allocate and assign
**                an array of sds_points and assign the address of that
**                array to *cpt_pp.  (cpt_pp is the address of the caller's
**                pointer to an sds_point.)
**
**   nknots_p : If possible, this function should set *nknots_p to the number
**                of elements allocated for the *knot_pp array.  (nknots_p is
**                the address of the caller's int variable for the
**                number of knot values.)
**    knot_pp : If possible, this function should allocate and assign
**                an array of sds_reals and assign the address of that
**                array to *knot_pp.  (knot_pp is the address of the caller's
**                pointer to an sds_real.)
**
**  Returns:
**       0 : OK
**      -1 : Bad calling parameter(s)
**      -2 : Insufficient memory
**      -3 : Anything else
*/
	{
	RC rc = SUCCESS;
	INT i, n;
	PSpline pCurve = NULL;

	// Construct the interpolating NURBS curve
	if (rc = GetFittingSpline(nfpts, fpt, ftol, btandir, 
							  etandir, bPeriodic, pCurve))
		QUIT
			
	// Return the control points
	n = pCurve->PointCount();
	if (ncpts_p)
		*ncpts_p = n;
	if (cpt_pp)
		{
		if (rc = ALLOC_FAIL(			
			*cpt_pp = new sds_point [n]))
			QUIT
		for (i = 0;  i < n;  i++)
			RptToSds(pCurve->ControlPoint(i), (*cpt_pp)[i]);
		}

	// Return the knots
	n += 4;
	if (nknots_p)
		*nknots_p = n;
	if (knot_pp)
		{
		if (rc = ALLOC_FAIL(			
			*knot_pp = new double [n]))
			QUIT
		for (i = 0;  i < n;  i++)
			(*knot_pp)[i] = pCurve->Knot(i);
		}

	// Tesseleate the spline
	if (vertex_pp)
		rc = GeneratePoints(pCurve, resocoeff, nverts_p, vertex_pp);
exit:
	delete pCurve;

	if (rc)
		{
		// Clean up
		FreeAndNull((void**)cpt_pp);
		FreeAndNull((void**)knot_pp);
		if (ncpts_p)
			*ncpts_p = 0;
		if (nknots_p)
			*nknots_p = 0;
		}
	return Result(rc);
	}


/////////////////////////////////////////////////////////////////////////
// The following functions are used by the pedit command

// Generate the rendering points
LOCAL RC GetPoints(
	PSpline pCurve,			// I: The spline to generate points on
	int nPerSegment,		// I: Number of segments per span, must be >0
	long *no_of_vertices,	// O: Number of generated vertices
	sds_point **vertices)	// O: The generated vertices, allocated here
	{
	RC rc = SUCCESS;
	int i, j, nFirst, nSegments;
	C3Point P;
	double t, dt, tNext;

	// The number of points and the relevant spans depend on the spline type:  
	if (pCurve->Knot(1) != pCurve->Knot(0))	
		// Periodic spline: the domain starts at the first knot.
		{
		nFirst = 0;
		nSegments = pCurve->PointCount();
		}
	else
		{
		// Non-periodic spline: the domain start after degree+1 knots. 
		nFirst = pCurve->Degree();
		nSegments = pCurve->PointCount() - nFirst; 
		}

	// Compute the number of points
	j = nSegments * nPerSegment + 1;
	if(j > 32767)
		{
		rc = E_TooManyKnots;
		QUIT
		}

	// Allocate the array of points
	*vertices = new sds_point[j];
	memset(*vertices,0,sizeof(sds_point)*j);
	if (rc = ALLOC_FAIL(*vertices))
		QUIT
	*no_of_vertices = 0;

	// Get the points
	t = pCurve->Knot(nFirst);
	for (i = nFirst;    i < nFirst + nSegments;	    i++)
		{
		tNext = pCurve->Knot(i+1);
		dt = (tNext - t) / nPerSegment;
		for (j = 0;    j < nPerSegment;   j++)
			{
			if (rc = pCurve->Evaluate(t, P))
				QUIT

			// We know that this is not a rational spline, so ignore w
			RptToSds(P, (*vertices)[(*no_of_vertices)++]);
			// Move on;
			t += dt;
			}
		t = tNext;
		}
	// One last point at the end
	if (rc = pCurve->Evaluate(t, P))
		QUIT
	RptToSds(P, (*vertices)[(*no_of_vertices)++]);

	// If closed - don't repeat the first point
	if (pCurve->Closed())
		(*no_of_vertices)--;


exit:
	if (rc)
		{
		// Clean up
		FreeAndNull((void**)vertices);
		if (no_of_vertices)
			*no_of_vertices = 0;
		}
		
	return rc;
	}
#define EQUAL_POINTS(p, q) (p)[0]==(q)[0] && (p)[1]==(q)[1] && (p)[2]==(q)[2]
/************************************************************************/
// Create an interpolating spline, and generate points along it
GR_API int gr_interp_spline(
	long n,	  // Must be > 2   I: The number of interpolation points 
	sds_point *q,			// I: The interpolaion points		
	int nPerSegment,			// I: Number of segments per span, must be >0
	long *no_of_vertices,	// O: Number of generated vertices
	sds_point **vertices)	// O: The generated vertices, allocated here
	{
	int rc = SUCCESS;
	PSpline pCurve = NULL;
	sds_point zero = {0,0,0};

	// Construct the interpolating NURBS curve
	if (rc = GetFittingSpline(n, q, FUZZ_DIST, zero, zero,
							  icadPointEqual(q[0], q[n-1]), // Periodic if closed
							  pCurve))
		QUIT

	// Generate points along it
	rc = GetPoints(pCurve, nPerSegment, no_of_vertices, vertices);
exit:
	// Clean up
	delete pCurve;
	if (rc)
		{
		FreeAndNull((void**)vertices);
		if (no_of_vertices)
			*no_of_vertices = 0;
		}

	return Result(rc);
	}
/************************************************************************/
// Create a spline from given control points, and generate points along it
GR_API int gr_raw_spline(
	long n,	  // Must be > 2   I: The number of control points 
	sds_point *q,			// I: The control points		
	int nPerSegment,		// I: Number of segments per span, must be >0
	long *no_of_vertices,	// O: Number of generated vertices
	sds_point **vertices)	// O: The generated vertices, allocated here
	{
	// We are constructing a uniform, non-rational spline, cubic if there 
	// are more than 3 points, quadratic otherwise.  The spline will be 
	// periodic if the first and last control points are equal
	RC rc = SUCCESS;
	INT i, nDegree;
	PKnots pKnots = NULL;
	PControlPoints pControlPoints = NULL;
	PSpline pCurve = NULL;
	
	// If it's a closed polyoline we'll produce a periodic spline
	BOOL bPeriodic = icadPointEqual(q[0], q[n-1]);
	if (bPeriodic)	// Ignore the last point
		n--;

	// Check input validity
	if (n < 3 || nPerSegment < 1) 
		{
		rc = E_BadInput;
		QUIT
		}

	// Costruct the knots
	nDegree = MIN(n-1, 3);
	pKnots = new CKnots(nDegree, n, rc);
	if (!pKnots)
		rc = DM_MEM;
	if (rc)
		QUIT

	// Construct the control points
	pControlPoints = new CControlPoints(n, rc);
	if (!pControlPoints)
		rc = DM_MEM;
	if (rc)
		QUIT

	// Copy the control points
	for (i = 0;  i < n;  i++)
		{
		C3Point P(q[i][0], q[i][1], q[i][2]);
		pControlPoints->Set(i, P);
		}

	// Set up the knots
	if (bPeriodic)
		// Periodic spline, no multiple knots
		for (i = 0;		i <= n;		i++)
			pKnots->Set(i, i);
	else
		{
		// Non-periodic spline, start with a multiple knot
		for (i = 0;		i <= nDegree;		i++)
			(*pKnots)[i] = 0;
		for (i = nDegree + 1;		i <= n;		i++)
			pKnots->Set(i, i - nDegree);
		}


	// Construct the spline
	pCurve = new CSpline(pKnots, pControlPoints, rc);
	if (!pCurve)
		rc = DM_MEM;
	else
		{// Now owned by the spline
		pKnots = NULL;
		pControlPoints = NULL;  
		}
	if (rc)
		QUIT

	// Generate the points along it
	rc = GetPoints(pCurve, nPerSegment, no_of_vertices, vertices);
exit:
	delete pKnots;
	delete pControlPoints;
	delete pCurve;	

	return Result(rc);
	}
/////////////////////////////////////////////////////////////////////////
// The following functions are used by the offset command

// Copy the rendering attributes from one linked list to another 
GR_API RC CopyAttributes(
	sds_resbuf * rbFrom,	// In: The list to copy from
	sds_resbuf * rbTo)		// In: The list to copy To
	{
	RC rc = SUCCESS;
	sds_resbuf * rb;

	// Go to the end of the destination's list
	for(/* No initialling */;	rbTo->rbnext != NULL;   rbTo = rbTo->rbnext);

	// Look for rendering attributes 
	for(rb = rbFrom;	 rb != NULL;   rb = rb->rbnext)
		{
		// Clone and add to the list
		switch (rb->restype)
			{
			case 6:					// Line type
			case 8:					// Layer
				rbTo->rbnext = sds_buildlist(rb->restype, rb->resval.rstring, 0);
				break;

			case 39:				// Thickness
			case 48:				// Line type scale
				rbTo->rbnext = sds_buildlist(rb->restype, rb->resval.rreal, 0);
				break;

			case 62:				// Color
				rbTo->rbnext = sds_buildlist(rb->restype, rb->resval.rint, 0);
				break;

			case 370:				// LWeight EBATECH(CNBR) 24-03-2003 Bugzilla#78471
				rbTo->rbnext = sds_buildlist(rb->restype, rb->resval.rint, 0);
				break;

			default:
				// Irrelevant
				continue;
			}

		if (rc = ALLOC_FAIL(rbTo->rbnext))
			QUIT
		rbTo = rbTo->rbnext;
		rbTo->rbnext = NULL;
		}
exit:
	return rc;
	}

// Build the linked list for creating a spline entity 
GR_API RC BuildSplineList(
	PSpline pSpline,			// I: The spline
	sds_resbuf ** rbSpline)	// O: The linked lis containing the spline data
	{
	RC rc = SUCCESS;
	sds_resbuf * rb;
	short nFlags = 0;
	sds_point p;
	double r;
	int i;
	C3Point N;

	// Initialize
	*rbSpline = NULL;

	// Get the flags settings
	if (pSpline->Closed())
		nFlags |= 1;
	if (pSpline->Period() != 0)
		{
		// Periodic spline conversion not implemented yet
		rc = E_BadInput;
		QUIT
		}

	if (pSpline->Rational())
		nFlags |= 4;

	if (pSpline->GetNormal(N))
		nFlags |= 8;

	// Build the spline's header
	if (!(*rbSpline = sds_buildlist(
		RTDXF0,"SPLINE",
		70,nFlags,
		71,pSpline->Degree(),
		73, pSpline->PointCount() + pSpline->Degree() + 1,
		74, pSpline->PointCount(), 0)))
		{
		rc = FAILURE;
		QUIT
		}
	
	// Go to the end of the list
	for(rb = *rbSpline;	rb->rbnext != NULL;   rb = rb->rbnext);

	// Enter the normal, if planar
	if (pSpline->Planar())
		{
		RptToSds(N, p);
		N =C3Point(.57, .57, .57);
		rb->rbnext = sds_buildlist(210, p, 0);
		rb = rb->rbnext;
		rb->rbnext = NULL;
		}

	// Load the control points
	for (i = 0;  i < pSpline->PointCount();  i++)
		{
		RptToSds(pSpline->ControlPoint(i), p);
		rb->rbnext = sds_buildlist(10, p, 0);
		rb = rb->rbnext;
		}
		rb->rbnext = NULL;

	// Load the knots
	for (i = 0;  i < pSpline->PointCount() + pSpline->Degree() + 1;  i++)
		{
		r = pSpline->Knot(i);
		rb->rbnext = sds_buildlist(40, r, 0);
		rb = rb->rbnext;
		}
		rb->rbnext = NULL;
		
	// Load the weights
	if (pSpline->Rational())
		{
		for (i = 0;  i < pSpline->PointCount();  i++)
			{
			r = pSpline->Weight(i);
			rb->rbnext = sds_buildlist(41, r, 0);
			rb = rb->rbnext;
			}
			rb->rbnext = NULL;
		}
exit:
	return rc;
	}		
/************************************************************************/
// Build the data of an offset of a curve 
LOCAL RC BuildOffset(
	PCurve pCurve,		  // I: The curve to offset
	double rDistance,	  // I: The signed distance (left is positive)
	double rTolerance,	  // I: Approximation tolerance
	P3Point pNormal,	  // I: The upward normal to the view plane
	sds_resbuf ** rbList) // O: The linked list containing the offset data
	{
	RC rc = SUCCESS;
	PCurve pApprox = NULL;
	PNURBS pSpline = NULL;

// In the future, when CCurve has a Persist method, the following line
// should be replaced by pCurve->GetOffset.  The offset then may be
// a line or a circular arc, and then GetApproximation may return a
// a line or an arc rather than a spline.

	// Get the procedural offset
	COffsetCurve cOffset(pCurve, rDistance, pNormal);

	// See if this offset distance is admissable for this curve
	if (rc = pCurve->AdmitOffset(rDistance, rTolerance, pNormal))
		QUIT
		
	// Approximate it
	if (rc = cOffset.GetApproximation(rTolerance, pApprox))
		QUIT
	if (pApprox->Type() != TYPE_NURBS)
		{
		rc = E_BadInput;
		QUIT
		}
	pSpline = (PNURBS)pApprox;
	pSpline->SetPlanar(pCurve->Planar());

	// Make a spline entity in the darabase
	rc = BuildSplineList(pSpline, rbList);
exit:
	delete pApprox;
	return rc;
	}
/************************************************************************/
// Construct an elliptical arc from sds definition
LOCAL RC GetEllipArcFrom_resbuf(
	sds_resbuf * rbEllipse, //I: The linked list containing the ellipse data
	PEllipArc & pArc,		// O: The elliptical arc
	C3Point & ptNormal)		// O: The view plane upward normal
	{
	RC rc = SUCCESS;
	pArc = NULL;
	C3Point ptCenter, ptMajor, ptMinor;
	sds_point p;
	double rRatio=1, rFrom=0, rTo=TWOPI;
	ptNormal = C3Point(0, 0, 1); // default normal

	// Get the ellipse's data
	sds_resbuf * rb;
	for (rb = rbEllipse;	rb != NULL;		rb = rb->rbnext)
		switch (rb->restype)
			{
			case 10:	
				ic_ptcpy(p, rb->resval.rpoint);
				SdsToRpt(p, ptCenter);
				break;

			case 11:	
				ic_ptcpy(p, rb->resval.rpoint);
				SdsToRpt(p, ptMajor);
				break;

			case 40:	
				rRatio = rb->resval.rreal;
				break;

			case 41:	
				rFrom = rb->resval.rreal;
				break;

			case 42:	
				rTo = rb->resval.rreal;
				break;

			case 210:
				ic_ptcpy(p, rb->resval.rpoint);
				SdsToRpt(p, ptNormal);
				if (rc = ptNormal.Unitize())
					QUIT
				break;
			}

	// Construct the minor axis
	ptMinor = ptNormal ^ ptMajor;
	if (rc)
		QUIT
	ptMinor *= rRatio;

	// Construct the arc
	pArc = new CEllipArc(ptCenter, ptMajor, ptMinor, rFrom, rTo);
	if (!pArc)
		rc = DM_MEM;
exit:
	if (rc)
		{
		delete pArc;
		pArc = NULL;
		}
	
	return rc;
	}
/************************************************************************/
// Construct a spline from sds definition
GR_API RC GetSplineFrom_resbuf(
	sds_resbuf * rbSpline, //I: The linked list containing the spline data
	PSpline & pSpline,	   // O: The spline
	C3Point & ptNormal)	   // O: The view plane upward normal
	{
	RC rc = SUCCESS;
	pSpline = NULL;
	C3Point Q;
	sds_point p; 
	sds_point ptStartTangent = {0,0,0};
	sds_point ptEndTangent = {0,0,0};
	sds_point * pFit = NULL;
	sds_resbuf * rb;
	int nDegree, n;
	double r;
	short nFlags;
	ptNormal = C3Point(0, 0, 1); // default normal

	// 0 tangents mean no prescribed tangents
	ptStartTangent[0] = ptStartTangent[1] = ptStartTangent[1] =
	ptEndTangent[0] = ptEndTangent[1] =ptEndTangent[2] = 0;

	// Construct the building blocks
	PKnots pKnots = NULL;
	PControlPoints pControlPoints = NULL;
	PRealArray pWeights = NULL;
	sds_Cache cFitPoints;

	if (ALLOC_FAIL(pKnots = new CKnots))
		QUIT
	if (ALLOC_FAIL(pControlPoints = new CControlPoints))
		QUIT
	if (ALLOC_FAIL(pWeights = new CRealArray))
		QUIT

	for (rb = rbSpline;		rb != NULL;		rb = rb->rbnext)
		switch (rb->restype)
			{
			case 10:
				ic_ptcpy(p, rb->resval.rpoint);
				SdsToRpt(p, Q);
				if (rc = pControlPoints->Add(Q))
					QUIT
				break;

			case 11:
				ic_ptcpy(p, rb->resval.rpoint);
				if (rc = cFitPoints.Add(p))
					QUIT
				break;

			case 12:
				ic_ptcpy(ptStartTangent, rb->resval.rpoint);
				break;

			case 13:
				ic_ptcpy(ptEndTangent, rb->resval.rpoint);
				break;

			case 40:	
				pKnots->Add(rb->resval.rreal);
				break;

			case 41:
				if (pWeights->Add(rb->resval.rreal))
					QUIT
				break;

			case 44:
				r = rb->resval.rreal;	// Fitting tolerance
				break;

			case 70:	
				nFlags = rb->resval.rint;
				break;

			case 71:	
				nDegree = rb->resval.rint;
				break;

			case 210:
				ic_ptcpy(p, rb->resval.rpoint);
				SdsToRpt(p, ptNormal);
				if (rc = ptNormal.Unitize())
					QUIT
				break;
			}


	if ((n = pKnots->Size()) > 0)
		{
		// We have a Scenario 1 spline

		// Adjust to Geo's way of representating knot
		pKnots->SetDegree(nDegree);
		n = pControlPoints->Size();
		r = (*pKnots)[n] - (*pKnots)[0];
		pKnots->Remove(n, pKnots->Size() - n);
		pKnots->SetInterval(r);

		if (pWeights->Size() > 0)
			{
			// Adjust to Geo's way of representating control points
			n = MIN(pWeights->Size(), pControlPoints->Size());
			for (INT i = 0;  i < n;  i++)
				{
				C3Point P = (*pControlPoints)[i];
				P *= (*pWeights)[i];
				pControlPoints->Set(i, P);
				}
			pSpline = new CNURBS(pKnots, pControlPoints, pWeights, rc);
			}
		else
			{
			// It's a non-rational spline
			delete pWeights;
			pWeights = NULL;
			pSpline = new CSpline(pKnots, pControlPoints, rc);
			}

		// Construct the spline
		if (!pSpline)
			rc = DM_MEM;

		// Now owned by the spline
		pKnots = NULL;
		pControlPoints = NULL;
		pWeights = NULL;
		}
	else
		{
		// We have a Scenario 2 spline		
		if (rc = cFitPoints.YieldPoints(&n, &pFit))
			QUIT
		if (rc = GetFittingSpline(n, pFit, r, ptStartTangent, 
						ptEndTangent, nFlags & 2, pSpline))
			QUIT
		}

	if (pSpline)
		pSpline->SetPlanar(nFlags & 8);
exit:
	if (rc)
		{
		delete pSpline;
		pSpline = NULL;
		}
	delete pKnots;
	delete pControlPoints;
	delete pWeights;
	if (pFit)
		IC_FREE(pFit);
	
	return rc;
	}
/************************************************************************/
// Build the linked list of the offset of an ellipse
GR_API int gr_offset_ellipse(
	sds_resbuf * rbEllipse, // I: The linked list containing the ellipse data
	double rDistance,		// I: Offset distance, (<0 for "through-point")
	sds_point sdsPick,		// I: User's pick point, wcs
	int mode,				// I: 1 for "through point", 2 for "both sides"
	sds_resbuf ** rbOffset)	// O: The linked list containing the offset data
	{
	RC rc = SUCCESS;
	double r, d;
	PEllipArc pArc = NULL;
	C3Point ptPick;
	C3Point P, T, N;

	// Make a full ellipse
	if (rc = GetEllipArcFrom_resbuf(rbEllipse, pArc, N))
		QUIT

	// If we do only one side, we need decide which side of the arc to offset,
	// based on the pick point.  Also, if it's offset thru point then we need
	// the distance from the point to the arc.  For both purposes, we need the
	// FULL ellipse nearest to the pick.
	if (mode < 2)
		{
		double rStart = pArc->Start();
		double rEnd = pArc->End();
		pArc->SetDomain(0, TWOPI);

		SdsToRpt(sdsPick, ptPick);
		if (rc = pArc->PullBack(ptPick, P, r, d))
			QUIT

		if (mode == 1)	// It's a thru-point offset
			rDistance = d;	
		
		//To decide which side, we get the the tangent to the ellipse at
		// the nearest point, and look which side of that tangent the pick
		// point lies.  A positive offset is to the left
		if (rc = pArc->Evaluate(r, P, T))
			QUIT
		ptPick -= P;
		if (Determinant(N, T, ptPick) < 0) 
			// It's a right offset!
			rDistance = -rDistance;
		
		// Restore the arc's domain
		pArc->SetDomain(rStart, rEnd);
		} // end if

	// Compute the tolerance, based on a rough estimate of the arc's length
	r = MAX(pArc->MajorAxis().Norm(), pArc->MinorAxis().Norm());
	r *= fabs(pArc->End() - pArc->Start());// The length on a circular arc
	r *= .0001;

	// Now get the offset
	if (rc = BuildOffset(pArc, rDistance, r, &N, rbOffset))
		QUIT

	// Copy the rendering attributes
	rc = CopyAttributes(rbEllipse, *rbOffset);
exit:
	delete pArc;
	return Result(rc);
	}
/************************************************************************/
// Build the linked list of the offset of a spline
GR_API int gr_offset_spline(
	sds_resbuf * rbSpline,  // I: The linked list containing the spline data
	double rDistance,		// I: Offset distance, (<0 for "through-point")
	sds_point sdsPick,		// I: User's pick point, wcs
	int mode,				// I: 1 for "through point", 2 for "both sides"
	sds_resbuf ** rbOffset)	// O: The linked list containing the offset data
	{
	RC rc = SUCCESS;
	double r, d;
	PSpline pSpline = NULL;
	C3Point ptPick;
	C3Point P, T, N;

	// Create the spline
	if (rc = GetSplineFrom_resbuf(rbSpline, pSpline, N))
		QUIT

	// If we do only one side, we need decide which side of the arc to offset,
	// based on the pick point.  Also, if it's offset thru point then we need
	// the distance from the point to the arc.  
	if (mode < 2)
		{
		SdsToRpt(sdsPick, ptPick);
		if (rc = pSpline->PullBack(ptPick, P, r, d))
			QUIT

		if (mode == 1)	// It's a thru-point offset
			rDistance = d;	
		
		//To decide which side, we get the the tangent to the spline at
		// the nearest point, and look which side of that tangent the pick
		// point lies.  A positive offset is to the left
		if (rc = pSpline->Evaluate(r, P, T))
			QUIT
		ptPick -= P;
		if (Determinant(N, T, ptPick) < 0) 
			// It's a right offset!
			rDistance = -rDistance;
		} // end if

	// Compute a tolerance based on the spline's length
	if (rc = pSpline->Length(r))
		QUIT
	r *= .0001;

	// Now get the offset
	if (rc = BuildOffset(pSpline, rDistance, r, &N, rbOffset))
		QUIT

	// Copy the rendering attributes
	rc = CopyAttributes(rbSpline, *rbOffset);
exit:
	delete pSpline;
	return Result(rc);
	}
/////////////////////////////////////////////////////////////////////////
// The following functions are used for area and lenght calculations


// Set the flag according to the spline's being closed & planar or not
GR_API short gr_get_spline_circleflag(	// Return 1 if planar & closed, 2 otherwise
	sds_resbuf * elist)		// I: The spline data as a linked list
	{
	sds_resbuf * rb;
	short nFlags = 0;

	// Find the spline's flags word
	for (rb = elist;		rb != NULL;		rb = rb->rbnext)
		if (rb->restype == 70)
			{
			nFlags = rb->resval.rint;
			break;
			}

	// Analize it
	if ((nFlags & 8)  &&  (nFlags & 1)) 
		return 1;
	else
		return 2;	
	}

/************************************************************************/
// Set the flag according to the ellipse's being closed or not
short gr_get_ellipse_circleflag( // Return 1 if closed, 2 otherwise
	sds_resbuf * elist)	// I: The ellipse data as a linked list
	{
	sds_resbuf * rb;
	double a=0, b=0;

	// Find the ellipse's start & end angles
	for (rb = elist;		rb != NULL;		rb = rb->rbnext)
		switch (rb->restype)
			{
			case 41:	
				a = rb->resval.rreal;
				break;

			case 42:	
				b = rb->resval.rreal;
				break;
			}

	// Analize it
	a = fabs(b - a);
	if (FEQUAL(a, TWOPI, FUZZ_GEN))	// It's a closed ellipse
		return 1;
	else
		return 2;
	}

/************************************************************************/
// Compute the length and area of a curve
LOCAL RC GetCurveData(
	PCurve p,			// I: The curve
	short f,			// I: 1 if area is to be done, 2 otherwise
	double * length,	// I/O: The spline's length, updated here
	double * area)		// I/O: The spline's area, updated if planar& closed
	{
	/*DG - 11.12.2001*/// length and area may be NULL now. Old code commented out below.
	RC	rc = SUCCESS;

	if(length)
	{
		double	thisLength = 0.0;
		if(rc = p->Length(thisLength))
			QUIT
		(*length) += thisLength;
	}

	if(f == 1 && area)
	{
		double	thisArea = 0.0;
		rc = p->UpdateAreaIntegral(thisArea);
		(*area) += fabs(thisArea) / 2.0;
	}
	/*
	double r, a = 0;
	
	RC rc = p->Length(r);
	if (rc)
		QUIT
	(*length) += r;

	if (f == 1)
		rc = p->UpdateAreaIntegral(a);
	(*area) += fabs(a) / 2;		// a may be negative 
	*/
exit:
	return rc;
	}

/************************************************************************/
// Compute the length and area of a spline
GR_API int gr_get_spline_edata(
	sds_resbuf * elist,	// I: The spline data as a linked list
	double * length,	// I/O: The spline's length, updated here
	double * area)		// I/O: The spline's area, updated if planar& closed 
	{
	PCurve p = NULL;
	PSpline q = NULL;
	C3Point N;
	
	short f = gr_get_spline_circleflag(elist);
	RC rc = GetSplineFrom_resbuf(elist, q, N);
	p = q;
	if (!rc)
		rc = GetCurveData(p, f, length, area);

	delete p;
	return Result(rc);
	}
/************************************************************************/
// Compute the length and area of an ellipse
GR_API int gr_get_ellipse_edata(
	sds_resbuf * elist,	// I: The ellipse data as a linked list
	double * length,	// I/O: The ellipse's length, updated here
	double * area)		// I/O: The ellipse's area, updated if planar& closed 
	{
	PCurve p = NULL;
	PEllipArc q = NULL;
	C3Point N;
	
	short f = gr_get_ellipse_circleflag(elist);
	RC rc = GetEllipArcFrom_resbuf(elist, q, N);
	p = q;
	if (!rc)
		rc = GetCurveData(p, f, length, area);

	delete p;
	return Result(rc);
	}
/************************************************************************/
// Compute the end tangents of a fitiing spline
GR_API int gr_get_spline_tangents(

// In						  DXF code
    int         nfpts,      /*    74    */
    sds_point  *fpt,        /*    11    */
    double		ftol,       /*    44    */
	BOOL bPeriodic,			/* 70, Bit 2 */

// Out
    sds_point   btandir,    /*    12    */
    sds_point   etandir)    /*    13    */
	{
// This function will return the tangent vectors at the seam of a periodic 
// spline. If the spline is not periodic (bPeriodic = FALSE) then it's a no-op.
	if (bPeriodic)
		return gr_splinegen2(3, nfpts, fpt, ftol, btandir, etandir, bPeriodic, 
							 1, NULL, NULL, NULL, NULL, NULL, NULL);
	else
		return 0;
	}


