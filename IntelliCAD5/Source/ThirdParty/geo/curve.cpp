/*	CURVE.CPP - Implements the class CCurve
 *	Copyright (C) 1994-98 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * See CURVE.H for the definition of the class.
 * This file implements its methods		
 *
 *
 */

#include "Geometry.h"
#include <Float.h>
#include "Point.h"
#include "CompCrvs.h"		// For CMappedCurve
#include "Roots.h"			
#include "Rational.h"		// For CRationalFunction
#include "Knots.h"
#include "CtrlPts.h"
#include "TheUser.h"
#include "Curve.h"
#include "Line.h"
#include "NURBS.h"
#include "EllipArc.h"
#include "CrvOfset.h"
#include "CrvPair.h"
#include "2CurveSolver.h"

DB_SOURCE_NAME;

const int iIterations = 50;

//////////////////////////////////////////////////////////////////
//           Curve 
CCurve::~CCurve()
    {
    }
/************************************************************************/
// Curve type
CURVETYPE CCurve::Type() const
    {
	return TYPE_Unknown;
    }
//////////////////////////////////////////////////////////////////////////
//																		//
//						AREA CALCULATION								//
//																		//
//////////////////////////////////////////////////////////////////////////
// The determinant |C, C', Up| on a curve, used for area computation
class CCurl  :  public CRealFunction
{
public:
// Construct/destroy
	CCurl()
		{
		m_pCurve = NULL;
		}

	CCurl(
		PCCurve pCurve,			// In: The curve
		P3Point pUp=NULL)		// In: Upward unit normal (NULL OK)
		{
		m_ptUp = CNormal(pUp);
		m_pCurve = pCurve;
		m_rStart = pCurve->Start();
		m_rEnd = pCurve->End();
		}

	~CCurl()
		{
		}

	PRealFunction Clone() const
		{
		return new CCurl(*this);
		}

// Methods
	RC Evaluate(
		double rAt,      // In: Parameter value to evaluate at
		double & rValue) // Out: The determinant |C, C', Up| on the curve
		{
		C3Point P, D;
		RC rc = m_pCurve->Evaluate(rAt, P, D);
		if (!rc)
			rValue = Determinant(P, D, m_ptUp);
		RETURN_RC(CCurl::Evaluate, rc);
		}
// Data
protected:
	PCCurve m_pCurve;		// The curve
	CNormal m_ptUp;			// Defines the up direction
};


/************************************************************************/
//The integral of YdX
RC CCurve::UpdateAreaIntegral(
	double & rSoFar,		// In/Out: The integral 
	P3Point pUp) const		// In: Upward unit normal (NULL OK)
    {
	RC rc;
	double r;
	CCurl cCurl(this, pUp);

	rc = Integrate(cCurl, Start(), End(), r);
	if (!rc)
		rSoFar += r;

	RETURN_RC(CCurve::UpdateAreaIntegral, rc);
    }	
/************************************************************************/
// Evaluator - point, tangent and curvature
RC CCurve::Evaluate(
	double rAt,                 // In: Where to evaluate
	C3Point & ptPoint,			// Out: Point there
	C3Point & ptTangent,		// Out: Unit tangent vector there
	double & rCurvature,		// Out: Curvature there
	double * pSpeed,			// Out: Speed there (optional, NULL OK)
	P3Point pUp) const			// In: Upward normal (optional, NULL OK)
    {
	RC rc;
	double rInverse = 1;
	double rSpeed = 0;
	rCurvature = INFINITY;

	// Define an "up" direction
	CNormal N(pUp);

	// Get the point and derivatives
	C3Point P[3];
	if (rc = Evaluate(rAt, 2, P))
		QUIT

	// Return the point
	ptPoint = P[0];

	// Get the speed, and the unit tangent, if needed
	rSpeed = P[1].Norm();
	if (FZERO(rSpeed, FUZZ_GEN))
		{
		ptTangent = C3Point(0, 0);
		rCurvature = INFINITY;
		rc = E_ZeroVector;
		}
	else
		{
		rInverse = 1 / rSpeed;
		ptTangent = P[1] * rInverse;
		}

	if (rc)
		{
		// Failed to get a unit direction, try to get it from the second derivative
		rInverse = P[2].Norm();
		if (FZERO(rInverse, FUZZ_GEN))
			QUIT						// Give up!
		ptTangent = P[1] * rInverse;
		// ...but the curvature remains INFINITY
		rc = SUCCESS;
		}
	else
		// Compute the curvature
		rCurvature = Determinant(ptTangent, P[2], N) * rInverse * rInverse;
exit:
	if (!rc && pSpeed)
		*pSpeed = rSpeed;
 	RETURN_RC(CCurve::Evaluate, rc);
    }
/************************************************************************/
// Get the curvature only
RC CCurve::GetCurvature(
	double rAt,				// In: Parameter value of a point on the curve
	double & rCurvature,	// Out: The curvature there
	P3Point pUp) const 		// In: Upward normal (optional, NULL OK)
	{
	C3Point P, T;
	return Evaluate(rAt, P, T, rCurvature, NULL, pUp);
	}
/************************************************************************/
// Homogeneous evaluator
RC CCurve::EvaluateHomogeneous(
	double rAt,						// In: Parameter value 
	C3Point & rptPoint,				// Out: Nomerator of the point there
	double & rDenominator) const	// Out: Denominator of the point there
	{
	rDenominator = 1;
	RC rc = Evaluate(rAt, rptPoint);
	RETURN_RC(CCurve::EvaluateHomogeneous, rc);
	}
//////////////////////////////////////////////////////////////////////////
//																		//
//			LENGTH CALCULATION AND CONSTANT SPEED CURVES				//
//																		//
//////////////////////////////////////////////////////////////////////////
// Implementation of CSpeed

// Constructor
CSpeed::CSpeed(
	PCCurve pCurve)  // In: The curve
	{
	m_pCurve = pCurve;
	m_rStart = pCurve->Start();
	m_rEnd = pCurve->End();
	}

/**********************************************************************/
// Get a function value
RC CSpeed::Evaluate(
	double rAt,         // In: Parameter value to evaluate at
	double & rValue)    // Out: xdy - ydx on the curve
	{
	C3Point P, D;
	RC rc = m_pCurve->Evaluate(rAt, P, D);
	if (!rc)
		rValue = sqrt(D.X() * D.X() + D.Y() * D.Y() + D.Z() * D.Z());
	RETURN_RC(CSpeed::Evaluate, rc);
	}
/**********************************************************************/
// Get the inverse of the function value
RC CSpeed::EvaluateInverse(
	double rAt,			// In: Parameter value
	double & rInverse)	// Out: 1/speed there
	{
	RC rc;

	if (rc = Evaluate(rAt, rInverse))
		QUIT;
	if (FZERO(rInverse, FUZZ_GEN))
		rc = FAILURE;
	else
		rInverse = 1 / rInverse;
exit:
	RETURN_RC(CSpeed::EvaluateInverse, rc);
	}	
	
/**********************************************************************/
// Curve length
RC CCurve::Length(
	double & rLength) const   // Out: The total length
    {
	// Integrate the curve's speed on 10 subintervals
	CSpeed cSpeed(this);
	RC rc;
	double dt = (End() - Start()) * 0.1;
	double a = Start();
	
	rLength = 0;
	for (int i = 1;  i <= 10;  i++)
		{
		double r;
		double b = a + dt;
		if (rc = Integrate(cSpeed, a, b, r))
			QUIT
		rLength += r;
		a = b;
		}
exit:
	RETURN_RC(CCurve::Length, rc);
    }
/*******************************************************************************/
// Construct a copy reparameterized to arc length
RC CCurve::ArcLengthCopy(
	double rFrom,	// In: Domain start
	double rTo,		// In: Domain end
	PCurve & pCopy)		// Out: A constant speed copy
	{
/*
 * This function computes the procedural combination of this curve with a 
 * parameter mapping f so that the speed (=length of derivative vector) is  
 * equal to 1.  The mapping f is actually the inverse of the length function
 * It does so by approximating the function with a sample of 33 values.
 */
	RC rc, k, n;
	PRationalFunction pMapping = NULL;
	double dt = (rTo - rFrom) / 32;
	int i;
	CSpeed cSpeed(this);
	pCopy = NULL;
	
	// Get a sample in 3 arrays:
	CRealArray cT(33, rc);	// Equally spaced parameter values in the curve's domain
	CRealArray cS(33, k);	// Accummulated length at cT[i]
	CRealArray cDT(33, n);	// The derivative dt/ds at cT[i]
	if (k)
		rc = k;
	if (n)
		rc = n;
	if (rc)
		QUIT

	// First sample point
	cT[0] = rFrom;
	cS[0] = 0;

	// Get dt/ds = 1/speed
	if (rc = cSpeed.EvaluateInverse(cT[0], cDT[0]))
		QUIT;
	
	// Loop on the sample and load the data arrays	
	for (i = 1;   i <= 32;   i++)
		{
		double ds;

		cT[i] = cT[i-1] + dt;

		// Store the accummulated length at Ti in Si
		if (rc = Integrate(cSpeed, cT[i-1], cT[i], ds))
			QUIT
		cS[i] = cS[i-1] + ds;

		// Get dt/ds = 1/speed
		if (rc = cSpeed.EvaluateInverse(cT[i], cDT[i]))
			QUIT;
		}
	
	// Interpolate the data to construct the mapping
	pMapping = new CRationalFunction(cS, cT, cDT, rc);
	if (!pMapping)
		rc = DM_MEM;
	if (rc)
		QUIT
/*
#ifdef DEBUG
	// Validation
	if (rc)
		QUIT
	pMapping->Dump();
	DB_MSGX("Validate the mapping",());
	srand(0);
	for (i = 0;  i < 10;  i++)
		{
		double s, r;
		// Get the length s at a random parameter value t
		double t = Start() + FRAND * (End() - Start());
		s = 0;
		for (int j=0;  j<10;  j++)
			{
			double ds;
			double dt = (t - Start()) / 10;
			Integrate(cSpeed, Start() + j * dt, Start() + (j+1) * dt, ds);
			s += ds;
			}

		// The value of the mapping at s should be equal to t
		pMapping->Evaluate(s, r);
		DB_MSGX("t=%lf  round trip=%lf",(t, r));
		}
#endif
*/
	// Construct the reparameterized copy
	rc = ALLOC_FAIL(pCopy = new CMappedCurve(this, pMapping));
	pMapping = NULL;

exit:
	delete pMapping;
	if (rc)
		{
		delete pCopy;
		pCopy = NULL;
		}
	RETURN_RC(CCurve::ArcLengthCopy, rc);
	}
/**********************************************************************/
// Determine if a parameter value lies within the curve's domain
BOOL CCurve::PullIntoDomain(
	double & rT   //In/out: Parameter value, pulled into the domain
) const
// Return TRUE if rT lies within the domain within ZEROPARAM
    {
	BOOL bIn = FALSE;
	double t = rT;

	if (Period())
		// Periodic case
	    {
		// Quit if it's too far out - Bug 4894, 8/8/95, mxk
		if (!(bIn = fabs(rT - Start()) < 1000 * Period()))
			goto exit;
			 
		// Pull rT into the particualr domain representation
		while (t >= End() + FUZZ_GEN)
			t -= Period();
			
		while (bIn = t <= Start() - FUZZ_GEN)
			t += Period();
		}
		 
	// Snap it in, if close
	if (bIn = FEQUAL(t, Start(), FUZZ_GEN))
		t = Start();
	else if (bIn = FEQUAL(t, End(), FUZZ_GEN))
		t = End();

	// After all this pulling, is it in?
	if (bIn = (Start() <= t  &&  t <= End()))
		rT = t;
exit:
	return bIn;
    }
/****************************************************************************/
// Parameter values where the curve's continuity is less than required
RC CCurve::Breaks(
	int iDeriv,                     // In: The required continuity
	PRealArray & pBreaks) const		//Out: The breaks
	{
	RC rc = SUCCESS;
	pBreaks = new CRealArray;	
	if (!pBreaks)
		{
		rc = DM_MEM;
		goto exit;
		}
	if (Period == 0)
		{
		if (rc = pBreaks->Add(Start()))
			goto exit;
		rc = pBreaks->Add(End());
		}
exit:
	if (rc)                               
		{
		delete pBreaks;
		pBreaks = NULL;
		}
	RETURN_RC(CCurve::Breaks, rc);
	}
/**********************************************************************/
//Nearest point on the curve to a given point, with initial guess.
RC CCurve::PullBack(		  // E_NOMSG if didn't converge in the domain
	const C3Point & ptTo,	  // In: The point we want to get closest to
	double rSeed,             // In: Parameter value of the initial guess
	C3Point & ptNearest,      // Out: The nearest point on the curve
	double & rAt,             // Out: Parameter value of Nearest
	double & rSqDist) const   // Out: The distance squared
    {
/// This function will only work correctly on C2 curves!!
	CPullBack solver(this, ptTo);
	RC rc = solver.Solve(rSeed);
	if (!rc)
		{
		ptNearest = solver.Point();
		rAt = solver.Parameter();
		rSqDist = solver.SqDistance();
		}
	RETURN_RC(CCurve::PullBack, rc);
	}		
/**********************************************************************/
// Nearest interior point on the curve to a given point
RC CCurve::PullBack(
	C3Point ptTo,		 // In: The point we want to get closest to
	C3Point & ptNearest, // Out: The nearest point on the curve
	double & rAt,        // Out: Parameter value of Nearest
	double & rDist,		 // Out: The distance from the point to the curve
	double rMargin,		 // In: Distance considered 0 (optional)
	double ignore) const // Optional: Parameter value to ignore
    {
	RC rc;
	int i;
	double t, d;
	C3Point P,Q; 
	PRealArray pSeeds = NULL;

	double rSqFuzz = rMargin * rMargin;
	CPullBack solver(this, ptTo, rSqFuzz);
	rDist = DBL_MAX;

	// Try all cusps
	if (rc = Breaks(1, pSeeds))
		goto exit;

	for (i=0;  i<pSeeds->Size();  i++)
		{
		t = (*pSeeds)[i];
		if (rc = Evaluate(t, P))
			goto exit;
		Q = P - ptTo;
		d = Q * Q;
		if (d < rDist 	&&	!FEQUAL(t, ignore, FUZZ_DIST))
	    	{
			ptNearest = P;
			rAt = t;
			rDist = d;
	    	}
		if (rDist < rSqFuzz)
			// No need to go on, we're on the curve
			break;
    	}
	
	// Get a list of seeds
	pSeeds->RemoveAll();
	if (rc = Seeds(*pSeeds))
		goto exit;
	
	// Search with every seed
	for (i=0;  i<pSeeds->Size();  i++)
		{
		if (!solver.Solve((*pSeeds)[i]) 
			&&	solver.SqDistance() < rDist
			&&	!FEQUAL(solver.Parameter(), ignore, FUZZ_DIST))
	    	{
			ptNearest = solver.Point();
			rAt = solver.Parameter();
			rDist = solver.SqDistance();
	    	}
		if (rDist < rSqFuzz)
			// No need to go on, we're on the curve
			break;
    	}
exit:
	delete pSeeds;
	if (rc == E_NOMSG) // No local minimum was found, but that's OK
		rc = SUCCESS;
	rDist = sqrt(rDist);  
	RETURN_RC(CCurve::PullBack, rc);
    }		
/**********************************************************************/
// Nearest point on the curve to a given point
RC CCurve::NearestPoint(
	C3Point ptTo,			 // In: The point we want to get closest to
	C3Point & ptNearest,	 // Out: The nearest point on the curve
	double & rAt,			 // Out: Parameter value of Nearest
	double & rDist,			 // Out: The distance from the point to the curve
	BOOL bIgnoreStart) const // Optional: Ignore startpoint if TRUE
	{
	rDist = INFINITY;
	C3Point P;
	double d;
	RC rc = PullBack(ptTo, ptNearest, rAt, rDist);

	// if pullback didn't succeed, reset the min dist and check the endpoints
	if ( rc != SUCCESS )
		rDist = INFINITY;

	// Try the start point
	if (!bIgnoreStart && !GetStartPoint(P))
		{
		if ((d = PointDistance(P, ptTo)) < rDist)
			{
			rDist = d;
			ptNearest = P;
			rAt = Start();
			rc = SUCCESS;
			}
		}

	// Try the end point
	if (!GetEndPoint(P))
		{
		if ((d = PointDistance(P, ptTo)) < rDist)
			{
			rDist = d;
			ptNearest = P;
			rAt = End();
			rc = SUCCESS;
			}
		}
	RETURN_RC(CCurve::NearestPoint, rc);
	}
/**********************************************************************/
// Determine if a given point is on the curve (within tolerance)
BOOL CCurve::HitTest( // Return TRUE if the point is on the curve
	const C3Point& ptTo, // In: The point we want to test
	double rMargin)      // In: The distance considered a hit
	{
	BOOL bHit = FALSE;
	double t, rDistance;
	C3Point Q;

	if (!PullBack(ptTo, Q, t, rDistance, rMargin))
		bHit = rDistance <= rMargin;

	return bHit;
	}

/**********************************************************************/
// Determine if a ray hit the curve within tolerance. It returns TRUE
// with hit locations at the curve.
//
// Author:  Stephen W. Hou
// Date:    7/7/2004
//
BOOL CCurve::HitTest(           // Return TRUE if the ray hits the cuvre
    const C3Point &ptThrough,   // In: Through point 
    const C3Vector &hitDir,     // In: Hit direction
    double rTolerance,          // In: The tolerance considered a hit
    double & rAt)               // Out: Hit location on the curve within tolerance
{
    BOOL bHit = FALSE;

    CAffine iTransf, transf(hitDir);

    CCurve* pCopy = NULL;
    C3Point point;

    C3Vector dir = hitDir.Normalize();

    if (!FZERO(dir.X(), FUZZ_GEN) || !FZERO(dir.Y(), FUZZ_GEN) ||
        !FEQUAL(dir.Z(), 1.0, FUZZ_GEN)) {
        iTransf.SetInverse(transf);
        iTransf.ApplyToPoint(ptThrough, point);
        if (TransformCopy(iTransf, pCopy)) {
            if (pCopy)
                delete pCopy;
            return bHit;
        }
    }
    else {
        pCopy = Clone();
        point = ptThrough;
    }

	double rSqFuzz = rTolerance * rTolerance;

    CUnboundedLine ray(point, C3Point(0., 0., 1.));
    CCurvesXYDistance solver(pCopy, &ray, rSqFuzz);

	// Generate seeds on both curves
    CRealArray curveSeeds, raySeeds;
    if (pCopy->Seeds(curveSeeds) || ray.Seeds(raySeeds)) {
		delete pCopy;
        return FALSE;
    }

    double dis = INFINITY;
    for (register i = 0; i < curveSeeds.Size(); i++) {
        for (register j = 0; j <raySeeds.Size(); j++) {
	        if (solver.Solve(curveSeeds[i], raySeeds[j]) || solver.Singular() || 
                solver.SqDist() > rSqFuzz || solver.SqDist() > dis)
		        continue;
            
            double param = solver.Parameter(0);
            if (pCopy->PullIntoDomain(param)) {
                dis = solver.SqDist();
                rAt = param;
	            bHit = TRUE;
            }
        }
    }

    delete pCopy;
    return bHit;
}

/****************************************************************************/
// Is the curve closed
BOOL CCurve::Closed() const
// Return true if the curve ends meet
	{
	C3Point P,Q;
	if (GetStartPoint(P)	||  GetEndPoint(Q))
		return FALSE;
	else
		return (P == Q);
	}
/************************************************************************/
/* 
 *             APPROXIMATING WITH A CUBIC SPLINE
 *             ---------------------------------
 *
 * The following functions with names "CubicXyz" implement the approximation
 * of a curve by a cubic spline.  The main problem is determining a knot-set under
 * which the spline interpolator (CNURBS::Approximate) will produce a sufficiently
 * tight approximation. To do this we march along the curve domain, adapting the 
 * step-size to the error estimate between a cubic piece and the curve. This is done
 * in the function CubicKnots, which calls CubicError for the error estimate. The
 * former can be overridden by a more efficient versions in special cases, where
 * there's a fixed span-size that can be computed once for the entire curve, e.g.
 * for a straight line or a circular arc.
 */
/************************************************************************/
// Construct the knot sequence for the approximation with a cubic spline within tolerance
RC CCurve::CubicKnots(
	double rTolerance,	// In: The acceptable error
	PKnots & pKnots)	// Out: Knots for an approximating spline within tolerance
	{
	RC rc = SUCCESS;
	BOOL bLast = FALSE;
	pKnots = NULL;
	int i = 0;
	int j, n;
	C3Point P[9];
	P3Point Q = &P[3];
	int nMupltiplicity = 4;
	double rCurrent = 0;
	double rStep = End() - Start();
	double rSmallestStep = rStep / 10000;
	rTolerance *= .03;
		
	// Check input validity
	if (FZERO(rTolerance, FUZZ_GEN)  ||  rStep < FUZZ_GEN)
		{
		rc = E_InputZero;
		QUIT
		}

	pKnots = new CKnots(3, 0, rc);
	if (!pKnots)
		rc = DM_MEM;
	if (rc)
		QUIT
	
	// Determine first knot's multiplicity according to derivative continuity
	if (Evaluate(Start(), 2, P)  ||
		Evaluate(End(), 2, Q))
		// This curve doesn't support second derivative
		{
		if (rc = Evaluate(Start(), P[0]))
			QUIT
		if (rc = Evaluate(End(), Q[0]))
			QUIT
		n = 0;
		}
	else
		n = 2;
	// This also takes care of an initial P[0] for later
	
	for (i=1;  i<=n;  i++)
		{
		if (!(P[i] == Q[i]))
			break;
		nMupltiplicity--;
		}

	/*
	 * The curve is closed, but not tangent continuous.	A periodic curve
	 * would be mathematically correct, but may confuse the users, so
	 */
	if (nMupltiplicity == 3)
		nMupltiplicity++;

	// Iterate 10 times, relaxing tolerance if failed
	for (j = 1;		j <= 10;	j++)
		{
		// Add the first knot
		rCurrent = Start();
		rStep = End() - rCurrent;
		if (rc = Evaluate(rCurrent, P[0]))
			QUIT
		for (i=1;  i<=nMupltiplicity;  i++)
			pKnots->Add(rCurrent);

		// March on
		rStep = End() - rCurrent;
		for (i = nMupltiplicity;   i < MAX_SPLINE_SIZE && !rc;    i++)
			{
			bLast = FALSE;
			if ((rc = CubicNextKnot(rTolerance, rSmallestStep, rCurrent, 
									P, rStep, bLast))  ||  bLast)
				break;
			rc = pKnots->Add(rCurrent);
			}

		if (rc  ||  !bLast)
			{
			// Failed, relax the tolerance and try again
			rTolerance *= 4;
			pKnots->RemoveAll();
			if (rc = Evaluate(Start(), P[0]))
				QUIT
			}
		else
			break;
		}
	if (!rc  &&  !bLast)
		rc = FAILURE;
exit:
    if (rc)
		{
		if (pKnots)
    		delete pKnots;
		pKnots = NULL;
		}
	else
		{
		if (i > 4)	// There is more than one nontrivial span
			{
			/*
			 To avoid a very small last span, which may be bad downstream, we try to 
			 push back the previous knot so that the two last spans are equal.  However,
			 this is limited by the size of the last step.
			 */
			double t = (rCurrent + (*pKnots)[i-2]) / 2;
			double s = rCurrent - rStep;
			pKnots->Set(i - 1, MAX(t, s));
			}
		// Now set the last knot
		pKnots->Set(i, rCurrent);
		}
	RETURN_RC(CCurve::CubicKnots, rc);
	}
/************************************************************************/
RC CCurve::CubicNextKnot(
	double rTolerance,		// In: The acceptable error
	double rSignificant,	// In: Step size lower bound
	double & rCurrent,		// In/Out: The current knot
	P3Point P,				// In/Out: Working array of 8 points
	double & rStep,			// Out: Step size to the next knot
	BOOL & bLast)			// Out: This is the last step, if TRUE 
	{
	RC rc = SUCCESS;
	bLast = FALSE;
	double rCloseEnough = rTolerance * 0.7;
	double rTargetError = rTolerance * 0.85;
	double rPassed = rCurrent;
	double rFailed = INFINITY;
	double rNext = rCurrent + rStep;

	// Figure out the local span-size	
	for (int i = 1;  i <= 100;  i++)	// Guard against infinite loop
		{
		double rError;
        
		// Compute the error for this step-size
		if (rc = CubicError(rCurrent, rStep, rTolerance, P, rError))
			{
			if (rc == E_NOMSG)
				{
				rFailed = rCurrent + rStep;	// This step is too large
				rc = SUCCESS;
				}
			else
				break;					// Some real trouble
			}
		else
			{						
			// This step is acceptable
			rPassed = rCurrent + rStep;
			if (rError > rCloseEnough)	// Accept this step, we can't do much better
				break;
			}
			
		// Stopping criteria
		if (rPassed >= End()  ||  rFailed - rPassed < rSignificant)
			break;
			
		// Get the next guess
		if (rError < FUZZ_GEN)
			{
			rNext = End();
			rStep = End() - rCurrent;
			}
		else
			{
			/* The error is assumed to be proportional to the 4th power of the span
			   size, so the ratio of spans is the 4th root of the ratio of errors.
			   Since we don't 
			 */ 
			rStep *= sqrt(sqrt(rTargetError / rError));
			rNext = rCurrent + rStep;
            }
		/* To avoid infinite oscilations we clip the next guess to the interval between
		 * the last passed and the last failed values.  Moreover, if rNext is too closed
		 * to this interval's ends we replace it by the interval's midpoint, to avoid 
		 * creeping too slowly towards the solution
		 */
		if (rNext > rFailed - rSignificant || rNext < rPassed + rSignificant)
			{
			rNext = (rFailed + rPassed) / 2;
			rStep = rNext - rCurrent;
			}		
		}

	if (rPassed - rCurrent < FUZZ_GEN)
		rCurrent = rNext;	// We'll take it anyway	
	else
		{
		if (bLast = (rPassed >= End()))
            rPassed = End();

		// Make the step
		rCurrent = rPassed;
		}
	P[0] = P[8];
	RETURN_RC(CCurve::CubicNextKnot, rc);
	}	
/************************************************************************/
// The expected error when approximating this curve segment by a cubic
RC CCurve::CubicError(	// Return E_NOMSG if the error is more than toelrance
	double rCurrent,	// In: The starting parameter of this span
	double rSpan,		// In: The length of this span
	double rTolerance,	// In: The maximum permitted error
	P3Point P,			// In/Out: Working array of points
	double & rError)	// Out: Approximate error
	/* 
	 The cubic error is, up to a constant, an approximation of the maximum of I^4(X),
	 I^4(Y), where  I(f) stands for the integral of the 4th root of the 4th derivative
	 of f, and X and Y are the coordiante functions of the curve.   

	 The working array P is passed as an argument by the caller for performance reasons,
	 because P[0] may be reused several times during step-size computation, and then 
	 P[8] becomes P[0] for the next step.  It is the caller's responsibility to provide
	 P[0].
	 */
	{
	RC rc;
	C3Point Q;
	double dt = rSpan / 8;
	double t;
	int i;
	
	// Get 4 more values (P[0] is assumed to be there)
	// P[0] is assumed valid!
	for (i=2;  i<=8;  i+=2)
		if (rc = Evaluate(rCurrent + i * dt, P[i]))
			QUIT

	// First approximation: the  4th order difference. 
	Q = P[0] + 6 * P[4] + P[8] - 4 * (P[2] + P[6]);
	rError = Q.Norm();
	if (rError > rTolerance)
		{
		rc = E_NOMSG;
		goto exit;
		}
	
	// Second approximation: 16 times the maximum of the same on two halves of the interval.
	for (i=1;  i<=7;  i+=2)
		if (rc = Evaluate(rCurrent + i * dt, P[i]))
			QUIT

	// First half
	Q = P[0] + 6 * P[2] + P[4] - 4 * (P[1] + P[3]);
	t = 16 * Q.Norm();
	if (t > rError)
		rError = t;
	if (rError > rTolerance)
		{
		rc = E_NOMSG;
		goto exit;
		}

	// The error on the second half
	Q = P[4] + 6 * P[6] + P[8] - 4 * (P[5] + P[7]);
	t = Q.Norm();
	if (t > rError)
		rError = t;
	if (rError > rTolerance)
		rc = E_NOMSG;
exit:
	RETURN_RC(CCurve::CubicError, rc);
	}
/*******************************************************************/
/* 
 *					GENERAL CURVE LINEARIZATION
 *					---------------------------
 *
 *******************************************************************/
// The approximate squared error between a curve and its chord
LOCAL RC GetSquaredError(
	PCCurve pCurve,		// In: A curve
	double a,			// In: Chord's starting parameter
	C3Point & A,		// In: Chord's starting point
	double b,			// In: Chord's end parameter
	C3Point & B,		// In: Chord's endpoint
	double & rError2,	// In/Out: An estimate of the error squared
	double & c,			// Out: The sample parameter
	C3Point & C)		// Out: Sample point
    {
    C3Point AB = B - A;
    C3Point AC;
	double rThird = (b - a) * .3333;
	double r = 1 + FRAND;

	// Get a point at a random parameter
	c = a + r * rThird;
	RC rc = pCurve->Evaluate(c, C);
	if (rc)
		QUIT
 	
	// Compute the squared distance of C from the line AB
    AC = C - A;
    r = AC * AC;
	if (FZERO(r, FUZZ_GEN))
		r = AB * AB;
	else
		{
		double s = Determinant(AB, AC);
		r = 4 * s * s / r;
		}

	// Update the maximal error
	if (r > rError2)
		rError2 = r;
exit:
	RETURN_RC(GetSquaredError, rc);
    }
/****************************************************************************/
// Check if a given chord approximates a given curve within tolerance
LOCAL RC CheckChord(
	PCCurve pCurve,		// In: A curve
	double rTolerance2,	// In: Chord-height toelrance squared
	double a,			// In: Chord's starting parameter
	C3Point & A,		// In: Chord's starting point
	double b,			// In: Chord's end parameter
	C3Point & B,		// In: Chord's endpoint
	double & rMaxError2,// Out: Maximal error squared
	BOOL & bGood)		// Out: TRUE if the chord is a good approximation
	{
	RC rc;
	double c, d;
	C3Point C, D;

	bGood = FALSE;
	rMaxError2 = 0;

	// Preliminary test: a random point in the mid half of the interval
	if (rc = GetSquaredError(pCurve, a, A, b, B, rMaxError2, c, C))
		QUIT
	if (rMaxError2 > rTolerance2)	// No good
		goto exit;

	// Passed the prelimiary 
	// Test an additional point in the first half
	if (rc = GetSquaredError(pCurve, a, A, c, C, rMaxError2, d, D))
		QUIT
	if (rMaxError2 > rTolerance2)	// No good
		goto exit;

	// Test an additional point in the second half
	if (rc = GetSquaredError(pCurve, c, C, b, B, rMaxError2, d, D))
		QUIT
	if (rMaxError2 <= rTolerance2)	// Passsed all tests
		bGood = TRUE;
exit:
	RETURN_RC(CheckChord, rc);
	}
/****************************************************************************/
// Linearize the curve
RC CCurve::Linearize(
	double rResolution,		// In: Chord-height toelrance
	PAffine pTransform,		// In: Rendering transformation (NULL OK)
	CCache & cCache) const	// In: The cache we render into
	{
	RC rc = LinearizePart(Start(), End(), rResolution, pTransform, cCache);

	RETURN_RC(CCurve::Linearize, rc);
	}
/****************************************************************************/
// Linearize a portion of the curve
RC CCurve::LinearizePart(
	double rFrom,			// In: Where to start
	double rTo,				// In: Where to stop
	double rResolution,		// In: Chord-height toelrance
	PAffine pTransform,		// In: Rendering transformation (NULL OK)
	CCache & cCache) const	// In: The cache we render into
	{
	RC rc;
	double t = rFrom;
	double rStep = (rTo - t ) / 32;  // Initial guess
	C3Point ptCurrent, Q;
	int i,j;	// Iteration counters, to safeguard against infinite loops
	/*
	 * We aim for a step size that will result in a chord-height that is less than
	 * the device resolution, but not too much less.  In the interest of speed,
	 * we'll take anything less than the resotultion and over half the resolution.
	 * Our correction will shoot for somewhere in the middle, about .77*resolution.
	 * To avoid unnecessary square roots, we work with squares.
	 */
	double rTolerance2 = rResolution * rResolution;
	double rHalfTol2 = rTolerance2 / 4;
	double rTarget = rTolerance2 * .6;
	if (rTo < rFrom + FUZZ_DIST)
		goto exit;

	// Initialize random number generator for debug predictability
	srand(0);

	// Get the first point, but don't cache unless the cache is empty
	if (rc = Evaluate(t, ptCurrent))
		QUIT
	if (!cCache.Started())
		{
		if (pTransform)
			pTransform->ApplyToPoint(ptCurrent, Q);
		else
			Q = ptCurrent;
		if (rc = cCache.Accept(Q, rFrom))
			QUIT
		}
	
	// Loop until the end of the curve is reached
	for (i = 1;	i < 4096 && t < rTo;	i++)
		{
		// Initial guesses for the next step
		double rNext = t + rStep;
		double rBad = rTo;
		double rGood = t;
		C3Point ptNext;
		BOOL bFirstTime = TRUE;

		// Iterate, to find a good step
		for (j = 1;  j < 100;  j++)
			{
			BOOL bGood;
			double r;

			if (rc = Evaluate(rNext, ptNext))
				QUIT
			if (rc = CheckChord(this, rTolerance2, t, ptCurrent, 
							    rNext, ptNext, r, bGood))
				QUIT
			if (bGood)
				{
				rGood = rNext;
				if (r > rHalfTol2)   // This step is close enough
					{
					rNext = rGood;
					break;
					}
				}
			else
				rBad = MAX(rNext, t);

			// Step size correction
			if (r < FUZZ_GEN)	// We're close to computational error
				{
				if (bFirstTime) // Probably a straight line segment
					rNext = rBad;
				else
					rNext = (rGood + rBad) / 2;
				}
			else
				{
				/*
				 * In circular arcs the bow is inverse proportional to the square
				 * of the arc's angle.  We use that as an approximate correction
				 * for a general curve.  If this correction is not satisfactory - 
				 * it is either not substantial enough, or takes us out of the
				 * already established brackets, we fall back to a safe correction,
				 * which is the midpoint of the established interval.
				 */
				double s = sqrt(sqrt(rTarget / r));
				if (FZERO(s - 1, 0.2))
					// This correnction is not substantial, fall back to a safe one
					rNext = (rBad + rGood) / 2;
				else
					{
					rStep = rStep * s;
					rNext = t + rStep;
					}
				}

			if (rNext > rBad   ||	rNext < rGood)
				// This correction is no good, fall back to a safe one
				rNext = (rBad + rGood) / 2;

			rStep = rNext - t;
			bFirstTime = FALSE;

			if  (rBad < rGood + FUZZ_GEN)
				break;
			}

		// Clip to the domain
		if (rNext > rTo)
			{
			rStep = rTo - t;
			t = rTo;
			rc = Evaluate(t, ptCurrent);
			}
		else
			{
			t = rNext;
			ptCurrent = ptNext;
			}
		
		// Accept the point
//		DB_MSGX("t=%lf, P=(%lf, %lf)",(t, ptCurrent.X(), ptCurrent.Y()));
		if (pTransform)
			pTransform->ApplyToPoint(ptCurrent, Q);
		else
			Q = ptCurrent;
    	if (rc = cCache.Accept(Q, t))
			QUIT;
		}
exit:
	RETURN_RC(CCurve::LinearizePart, rc);
	}
/************************************************************************/
// Update bounding box - do nothing stub
RC CCurve::UpdateExtents(
	BOOL bTight,			// In: Tight extents if =TRUE
	BOOL bIgnoreStart,		// In: Ignore the startpoint, if =TRUE
	CExtents & cExtents,	// In/out: The extents, updated here
	PAffine pTransform)		// Optional: Transformation (NULL OK)
	{
	// This should have never been called
	RETURN_RC(CCurve::UpdateExtents, FAILURE);
	}
/************************************************************************/
// Update wireframe box 
RC CCurve::Wireframe(
	BOOL bIgnoreStart,		// In: Ignore the startpoint, if =TRUE
	CExtents & cExtents,	// In/out: The extents, updated here
	PAffine pTransform)		// Optional: Transformation (NULL OK)
	{
	// For all bounded curves it's the same as the tight extents
	RC rc = UpdateExtents(TRUE, bIgnoreStart, cExtents, pTransform); 
	RETURN_RC(CCurve::Wireframe, rc);
	}
/************************************************************************/
// Seeds for equations solving
RC CCurve::Seeds(
	CRealArray & arrSeeds,  // In/Out: Array of seeds
	double* pStart,			// In: Domain start (optional, NULL OK)
	double* pEnd) const	// In: Domain end (optional, NULL OK)
    {
	RC rc = SUCCESS;

	// Knowing nothing about the curve, take 5 seeds
	double a = Start();
	double b = End();

	if (pStart)
		a = *pStart;
	if (pEnd)
		b = *pEnd;

	double r = a;
	double dr = (b - a) * 0.25;

	for (int i = 0;  i <= 4;	i++)
		{
		if (rc = arrSeeds.Add(r))
			QUIT
		r += dr;
		}
exit:
	RETURN_RC(CCurve::Seeds, rc);
    }
/*******************************************************************************/
// Find all self intersections
RC CCurve::SelfIntersect(
	CRealArray & cParameters, // In/Out: List of self intersection parameters
	double rTolerance)		  // Optional: Tolerance
// Self overlaps are not supported
	{
	int i,j;
	CRealArray arrSeeds;
	double t1, t2;
	RC rc;

	// Generate seeds on both curves
	if (rc = Seeds(arrSeeds))
		goto exit;

	// Loop on seeds
	for (i=0;  i<arrSeeds.Size();  i++)
		{	
		// Give the impatient user a chance to abort
		if (TheUser()->Canceled())
			{
	       	rc = E_CANCEL;
	       	goto exit;
			}

		for (j=0;  j<arrSeeds.Size();  j++)
			{
			// Solve the equations for an intersection
			rc = Intersect(this, this, arrSeeds[i], arrSeeds[j], 
											t1, t2, rTolerance);
			if (rc)
				if (rc == E_NOMSG)       
					{				 // Potential overlap
					rc = SUCCESS;    // but that's OK
					continue;
					}
				else
					goto exit;

			// Add to the list
			if (!FEQUAL(t1, t2, FUZZ_GEN))
				{
				if (rc = cParameters.AddSort(t1))
					goto exit;
				if (rc = cParameters.AddSort(t2))
					goto exit;
				}
			}	 // for j
		} // for i
exit:
	RETURN_RC(CCurve::SelfIntersect, rc);
	}
/************************************************************************/
// Transform
RC CCurve::TransformCopy(
	CTransform & cTransform,	// In: The transformation
	PCurve & pOut) const		// Out: the transformed line
	{
	RC rc;
	if (AffineInvariant()  && cTransform.Affine())
		{
		if (rc = ALLOC_FAIL(pOut = Clone()))
			QUIT
		rc = pOut->Transform(cTransform);	// in place
		}
	else
		pOut = new CTransformedCurve(this, &cTransform, rc);
exit:
	if (rc)
		{
		delete pOut;
		pOut = NULL;
		}
	RETURN_RC(CCurve::TransformCopy, rc);
	}
//////////////////////////////////////////////////////////////////////////
// Stubs of methods that are to be implemented for some subclasses only
/************************************************************************/
// Transform in place - stub
RC CCurve::Transform(
	CTransform & cTransform)   // In: The transformation
	{
    // This function should have never been called
	RETURN_RC(CCurve::Transform, FAILURE);
	}
/************************************************************************/
// Reverse - stub
RC CCurve::Reverse()
    {
    // This function should have never been called
	RETURN_RC(CCurve::Reverse, FAILURE);
    }
/*******************************************************************************/
// A translation to a given point
RC CCurve::Move(
	const C3Point & ptBy)	// In: The desired origin's new position
	{
	//This is a stub, it should have never been called
	RETURN_RC(CCurve::Move, FAILURE);
	}
/*******************************************************************************/
// Pack - stub
RC CCurve::Pack(
	LPBYTE & pStorage)  const // In/Out: The storage
	{
	//This is a stub, it should have never been called
	RETURN_RC(CCurve::Pack, FAILURE);
	}
/*******************************************************************************/
// Packing Size - stub
int CCurve::PackingSize() const
	{
	//This is a stub, it should have never been called
	return 0;
	}
/*****************************************************************************/
// Check if this offset distance will not cusp & do ugly things
RC CCurve::AdmitOffset(		// Always returns FAILURE
	double rDistance,		// In: The offset distance
	double rTolerance,		// In: The acceptable error
	P3Point pNormal)		// In: The Up direction (NULL OK)
	{
	// Stub, should have never been called
	RETURN_RC(CCurve::AdmitOffset, FAILURE);
	}
#ifdef DEBUG
/************************************************************************/
// Dump - stub
void CCurve::Dump() const
    {
    // I invoke the Fifth Ammendment!
    }
#endif // DEBUG
/************************************************************************/
// Evaluator - point and n derivatives
RC CCurve::Evaluate(
	double rAt,            // In: Where to evaluate
	int nDerivatives,      // In: Number of desired derivatives
	P3Point pValues) const // Out: Position and derivatives
    {
    // This function should have never been called
	RETURN_RC(CCurve::Evaluate, FAILURE);
    }
/************************************************************************/
// Evaluator - point and tangent vector
RC CCurve::Evaluate(
	double rAt,          // In: Parameter value to evaluate at
	C3Point & ptPoint,   // Out: Point on the line with that parameter
	C3Point & ptTangent  // Out: Tangent vector at that point
) const
    {
	// This should have never been called
	RETURN_RC(CCurve::Evaluate, FAILURE);
    }
/************************************************************************/
// Snap the end point to a given point
RC CCurve::SnapEnd(
	SIDE nWhichEnd,  		// In: Start of end
	const C3Point & ptTo)	// In: The point to snap to 
	{
	// Do nothing stub
	RETURN_RC(CCurve::SnapEnd, SUCCESS);
	}

#ifdef CURVE_SPLITTING	
/*****************************************************************************/
// Split
RC CCurve::Split(
	double rAt,              // In: Paramter value to split at
	SIDE nSide,              // In: Which side to keep (possibly both)  
	PCurve & pFirst,         // Out: The first half
	PCurve & pSecond) const  // Out: The second half
/* NULL pieces are considered legal. If you try to split a curve at one of
 * its ends then the pieces will be the original curve and a null piece,
 * and the operation will be considered successful. The caller must therefore
 * be preapred for NULL pieces.
*/ 
    {
	RC rc = SUCCESS;
	double t = 100 * FUZZ_GEN;
	pFirst = pSecond = NULL;

	// Test input validity. (Side effect: pulling rAt into the domain)
	if (!PullIntoDomain(rAt)  ||
		!(nSide == LEFT  || nSide == RIGHT || nSide == BOTH))
		{
		// Do nothing, the input is bad
		rc = E_BadInput;
		goto exit;
		}
		     	
	// Trivial cases
	if (rAt < Start() + t && nSide != LEFT)	// Too close to the beginning
		{
		// Clone the curve and return as second
		rc = ALLOC_FAIL(pSecond = Clone());
		goto exit;
		}

	if (rAt > End() - t && nSide != RIGHT)  // Too close to the end
		{
		// Clone the curve and return as first
		rc = ALLOC_FAIL(pFirst = Clone());
		goto exit;
		}
		
	if (nSide != RIGHT)  // Do the first half
	    {
		// Clone the curve and reset the start
		if (rc = ALLOC_FAIL(pFirst = Clone()))
			QUIT
		pFirst->ResetEnd(rAt);
 	    }

    if (nSide != LEFT)  // Do the second half
        {	
		// Clone the curve and reset the start
		if (rc = ALLOC_FAIL(pSecond = Clone()))
			QUIT
		pSecond->ResetStart(rAt);
	    }
exit:
	if (rc)
		{
		delete pFirst;
		delete pSecond;
		pFirst = pSecond = NULL;
		}
	RETURN_RC(CCurve::Split, rc); 
    }
#endif

/*****************************************************************************/
// Set the curve's domain
RC CCurve::SetDomain(
	double rFrom,  // In: New curve start
	double rTo)    // In: New curve end
	{
	// This should have never been called
	RETURN_RC(CCurve::SetDomain, FAILURE); 
    }
/*****************************************************************************/
// Replace this curve with its restriction a new domain
RC CCurve::ReplaceWithRestriction(
	double rFrom,   // In: New curve start
	double rTo,     // In: New curve end
	PCurve & pOut)	// Out: The restricion to the new domain
	{
	RC rc = SetDomain(rFrom, rTo);
	if (rc)
		pOut = NULL;
	else
		pOut = this;
	RETURN_RC(CCurve::ReplaceWithRestriction, rc); 
    }
/*****************************************************************************/
// Reset the domain start
RC CCurve::ResetStart(
		double rNew)  // In: New curve start
	{
	// This should have never been called
	RETURN_RC(CCurve::ResetStart, FAILURE); 
    }
/*****************************************************************************/
// Reset the domain end
RC CCurve::ResetEnd(
		double rNew)  // In: New curve end
	{
	// This should have never been called
	RETURN_RC(CCurve::ResetEnd, FAILURE); 
    }
/****************************************************************************/
// Get a random parameter value with its point and direction on the curve 
RC CCurve::RandomPoint(
	double & rAt,         // Out: Random paramter well within the domain
	C3Point & ptPoint,    // Out: The point on the curve there
	C3Point & ptTangent)  // Out : The tangent vector there
	{
	RC rc; 

	// Keep the parameter within 0.1 from domain start and end
	rAt = 0.8 * (End() - Start());
	if (FZERO(rAt, FUZZ_GEN))
		{
		// The interval is too small
		rc = FAILURE;
		goto exit;
		}
	rAt = (double)(Start() + (0.1 + FRAND) * rAt);

	// Evaluate
	rc = Evaluate(rAt, ptPoint, ptTangent);
exit:
	RETURN_RC(CCurve::RandomPoint, rc);
	}
/**********************************************************************/
// Integrate a real function along the curve

// The weights for Gauss-Legendre integration with 6 sampling points
const double rGaussWeight[3] =
	    {0.085662246189585, 0.18038078652407, 0.233956967286345};

// The sampling points for the above, relative to the [0,1] interval
const double rGaussSample[3] = 
	    {0.033765242898424, 0.169395306766868, 0.380690406958402};

#define SAMPLE_SIZE 5

RC CCurve::Integrate(
	CRealFunction &  cFunction, // In: The function
	double rFrom,               // In: Lower integration bound
	double rTo,                 // In: Upper integration bound
	double & rIntegral) const	// Out: The ingeral
    {
/* Numerical integration, using Gauss-Legendre quadrature on 6 points.
 * See "Elementary Numerical Analysis" by Conte & de Boor, III Edition,
 * p. 316 for details and numerical constants.  Our constants are store
 * as globals.  Their values are taken from the book.  The weights have
 * been divided by 2, and the sampling points adjusted to the interval
 * [0,1].
 * The function must have an evaluator of values only (no derivatives)
 * with valid values between the integration limits
 */
	RC rc = SUCCESS;
	rIntegral = 0.0;
	double rD = (rTo - rFrom);

	for (int i=0;  i<3;  i++)
	    {
		double r,s;
		if (rc = cFunction.Evaluate(rFrom + rGaussSample[i] * rD, r))
			goto exit;
		if (rc = cFunction.Evaluate(rTo - rGaussSample[i] * rD, s))
			goto exit;
		r += s;
		
		// Add with the appropriate weight
		rIntegral += r * rGaussWeight[i];
	    }
exit:	
	rIntegral *= rD;
	RETURN_RC(CCurve::Integrate, rc);
    }
/*******************************************************************************/
// Return an approximation with one of the Visio curves
RC CCurve::GetApproximation(
	double rTolerance,	// In: Approxiation tolerance
	PCurve & pApprox) 	// Out: The approximation
	{
	RC rc;
	pApprox = new CNURBS(*this, rTolerance, rc);
	if (!pApprox)
		rc = DM_MEM;
	if (rc)
		{
		delete pApprox;
		pApprox = NULL;
		}
	RETURN_RC(CCurve::GetApproximation, rc);
	}
/*******************************************************************************/
// Construct a procedural offset
RC CCurve::Offset(
	double rDistance,	// In: Offset signed distance (left if positive)
	PCurve & pOffset, 	// Out: The approximation of the offset
	P3Point pUp) const	// In: Upward unit vector (NULL OK)
	{
	RC rc = ALLOC_FAIL(pOffset = new COffsetCurve(this, rDistance, pUp));
	RETURN_RC(CCurve::Offset, rc);
	}
/*******************************************************************************/
// Initiate a stored path with this curve
RC CCurve::InitiatePath(
	PStorage pStorage) const	// In/Out: The Storage
	{
	RC rc = pStorage->InitiateCurve(this);
	RETURN_RC(CCurve::InitiatePath, rc);
	}
/*******************************************************************************/
// Persist this curve - stub
RC CCurve::Persist(
	PStorage pStorage) const	// In/Out: The Storage
	{
	// Should have never been called
	RETURN_RC(CCurve::Persist, FAILURE);
	}
/*******************************************************************************/
// Unpack - Note, this is NOT a member function!
RC UnpackCurve(
	LPBYTE & pStorage,  // In/Out: The storage
	PCurve & pCurve)    // Out: the unpacked curve
	{
	RC rc = SUCCESS;
	pCurve = NULL;
	CURVETYPE * p = (CURVETYPE *)pStorage;

	// Unpack the header
	CURVETYPE crvType = *p;
	pStorage += sizeof (CURVETYPE);

	switch (crvType)
		{
		case TYPE_Line:
			pCurve = new CLine(pStorage);
			break;

		case TYPE_UnboundedLine:
			pCurve = new CUnboundedLine(pStorage);
			break;

		case TYPE_Ray:
			pCurve = new CRay(pStorage);
			break;

		case TYPE_NURBS:
			pCurve = new CNURBS(pStorage, rc);
			break;

		default:
			rc = FAILURE;
			goto exit;
		}
	if (!rc  &&  !pCurve)
		rc = DM_MEM;
	if (rc)
		goto exit;
exit:
	RETURN_RC(UnpackCurve, rc);
	}
/*******************************************************************************/
RC CCurve::NextCusp(// Return E_NOMSG if there is no cusp
	double t,		// In: A paramater value
	double & rCusp)	// Out: Curve-end, by default there is no cusp
	{
	rCusp = End();
	RETURN_RC(CCurve::NextCusp, E_NOMSG);
	}
/*******************************************************************************/
// Is the line horizontal or a part of the border of a horizontal region
BOOL CCurve::HorizontalBorder(
	BOOL bVisible,	// In: TRUE if this path is visble
	double rMaxX)	// In: X extent
	{
/*
 *  Only a line can be horizontal.  Well, a spline can be too, if it's
 *  actually a line, but this is too rare a case to warrant the code 
 */
	return FALSE;
	}
/**********************************************************************/
// Snap, if possible within geometric tolerance, to a given parameter value
BOOL CCurve::SnapParameter(	// Return TRUE if snapped
	double rTo,				// In: Parameter value to snap to
	double rTolerance,    	// In: Geometric Tolerance
	double& rSnap) const	// In: Parameter value to snap
	{
	C3Point ptPoint, ptTangent;
	BOOL bSnapped = FALSE;

	if (Evaluate((rSnap + rTo) / 2, ptPoint, ptTangent))
		goto exit;
	bSnapped = FZERO((rSnap - rTo) * ptTangent.Norm(), rTolerance);
	if (bSnapped)
		rSnap = rTo;
exit:
	return bSnapped;
	}
////////////////////////////////////////////////////////////////////
//
//		Fining the tangent to a curve from a given point

/*
 The line segemnt from a point P to a point C(t) on a curve C is
 tangent to the curve if the det(C-P, C') = 0 there.  To find the 
 parameter value t where this happens we define that determinant
 as a subclass of CRealFunction, and invoke its NewtonRaphson method
 to find where the function is 0.  The class is protected by protecting
 all its methods, allowing access only to its friend class CCurve
 */

class CAngleToTangent	:	public CRealFunction
	{
	friend class CCurve;
protected:
	// Constructor/destructor/clone
	CAngleToTangent()
		{
		m_pCurve = NULL;
		}

	CAngleToTangent(
		PCurve pCurve,			// In: A curve
		const C3Point& P)	// In: A point
		{
		m_pCurve = pCurve;
		m_P = P;
		m_rStart = pCurve->Start();
		m_rEnd = pCurve->End();
		}

	virtual ~CAngleToTangent()
		{
		}

	PRealFunction Clone() const
		{
		CAngleToTangent* p = new CAngleToTangent;
		if (p)
			{
			p->m_pCurve = m_pCurve;
			p->m_P = m_P;
			p->m_rStart = m_rStart;
			p->m_rEnd = m_rEnd;
			}
		return p;
		}

	/******************************************************************/
	// Period
	double Period()
		{
		return m_pCurve->Period();
		}
	/******************************************************************/
	// Evaluate
	RC Evaluate(
		double rAt,             // In: Parameter value to evaluate at
		double & rValue,        // Out: The requested coordiante there
		double & rDerivative)  // Out: The derivative there
		{
/*
 This evaluatator returns the value and derivative of the 
 function f(t) = det(C(t)-P, C(t)') for the curve.  
 The derivative is det(C', C') + det(C-P, C"), but the first term 
 always vanishes.
 */
		C3Point D;
		C3Point C[3];
		RC rc = m_pCurve->Evaluate(rAt, 2, C);
		if (rc)
			QUIT
		D = C[0] - m_P;
		rValue = Determinant(D, C[1]);
		rDerivative = Determinant(D, C[2]);
exit:
		RETURN_RC(CAngleToTangent::Evaluate, rc);
		}

// Data
protected:
	PCurve m_pCurve;		// The curve
	C3Point m_P;		// The point
	};
/**********************************************************************/
// A tangent to a curve thru point, with initial guess.
RC CCurve::TangentThru(		// E_NOMSG if solution not found
	const C3Point& ptThru,	// In: The point on the tangent
	double rSeed,           // In: Parameter value of the initial guess
	double & rAt)			// Out: Parameter value of tangent
    {
/*
 * This function will only work correctly on C2 curves!!
 * The algorithm is based on Newton-Raphson's solution of the equation
 * f(t) = 0, where f(t) = det((ptTo - C(t)), C'(t)).
 */
	CAngleToTangent f(this, ptThru);
	RC rc = f.NewtonRaphson(rSeed, rAt);
	RETURN_RC(CCurve::TangentThru, rc);
	}
/*******************************************************************************/
// Update the number of intercpts with a given ray
RC CCurve::UpdateIntercepts(
	PRay pRay,			// In: A ray 
	int	& nIntercepts)  // In/out: The number of intercepts
	{
	// Not implemented, there is no need for it now, every relevant type has
	// its own UpdateIntercept version.
	RETURN_RC(CCurve::UpdateIntercepts, FAILURE);
	}
/*******************************************************************************/
// Create polyline edge(s) from this curve
RC CCurve::GetPolylineEdges(
	P3Point pVertices,	// Out: an array of 2 points
	double* pBulges,	// Out: an array of 2 bulges
	int	& nEdges)		// Out: The number of resulting edges
	{
	// Only valid for lines and circular arcs
	RETURN_RC(CCurve::GetPolylineEdges, FAILURE);
	}
/*******************************************************************************/
// Return a modeling ready slice of the curve
RC CCurve::NextSlice(// Return E_NOMSG when it's the last slice
	int & i,			// In/out: An index marker
	PCurve & pSlice)	// Out: The current slice
	{
	// The general case: the slice is the entire curve
	pSlice = this;
	i++;
	return E_NOMSG;
	}
/*******************************************************************************/
// Update the nearest and farthest intercpts with a given line
RC CCurve::UpdateIntercepts(
	PUnboundedLine pLine, // In: A line 
	BOOL bIgnoreOrigin,	  // In: Ignore the origin if TRUE
	double * pNearFore,   // In/out: Forward intercept nearest to line's origin
	double * pFarFore,    // In/out: Forward intercept farthest from line's origin
	double * pNearBack,	  // In/out: Backward intercept nearest to line's origin
	double * pFarBack)    // In/out: Backward intercept farthest from line's origin
	{
	CRealArray list, dummy;
	int i;

	// Get all intersections of the ray with this curve
	RC rc = Intersect(pLine, this, list, dummy);
	if (rc)
		QUIT

	// Update the nearest and farthest
	for (i = 0;  i < list.Size();  i++)
		UpdateNearAndFar(list[i], bIgnoreOrigin, pNearFore, pFarFore, 
												 pNearBack, pFarBack);
exit:
	RETURN_RC(CCurve::UpdateIntercepts, rc);
	}
/*******************************************************************************/
int CCurve::Complexity() const
	{
	ASSERT(FALSE);	// Shouldn't have been called
	return 10000000;
	}
/*******************************************************************************/
// Update the nearest and farthest intercpts with a given intercept
void UpdateNearAndFar(
	double contender,	  // In: Contender for the nearest/furthest position 
	BOOL bIgnoreOrigin,	  // In: Ignore the origin if TRUE
	double * pNearFore,   // In/out: Forward intercept nearest to line's origin
	double * pFarFore,    // In/out: Forward intercept farthest from line's origin
	double * pNearBack,	  // In/out: Backward intercept nearest to line's origin
	double * pFarBack)    // In/out: Backward intercept farthest from line's origin
	{
	if (contender >= 0)
		{
		if (pNearFore  &&  contender < *pNearFore && 
			!(bIgnoreOrigin && FZERO(*pNearFore, FUZZ_GEN)))
			*pNearFore = contender;
		if (pFarFore  &&  contender > *pFarFore && 
			!(bIgnoreOrigin && FZERO(*pFarFore, FUZZ_GEN)))
			*pFarFore = contender;
		}
	if (contender <= 0)
		{
		if (pNearBack  &&  contender > *pNearBack && 
			!(bIgnoreOrigin && FZERO(*pNearBack, FUZZ_GEN)))
			*pNearBack = contender;
		if (pFarBack  &&  contender < *pFarBack && 
			!(bIgnoreOrigin && FZERO(*pFarBack, FUZZ_GEN)))
			*pFarBack = contender;
		}
	}


/*******************************************************************
 *
 * Compute the length of a portion of a curve between two points.
 *
 * @Author: Stephen W. Hou
 * @Date:   April 6, 2004
 */
RC CCurve::GetLength(double rFrom,		// In: Lower integration bound
				     double rTo,        // In: Upper integration bound
				     double &rLength) const // Out: The length
{
	ASSERT(rFrom <= rTo);
	if (rFrom == rTo) {
		rLength = 0.0;
		return SUCCESS;
	}

	CSpeed cSpeed(this);
	RC rc;
	double dt = (rTo - rFrom) * 0.1;
	double a = rFrom;
	
	rLength = 0;
	for (int i = 1;  i <= 10;  i++) {
		double r;
		double b = a + dt;
		if (rc = Integrate(cSpeed, a, b, r))
			QUIT
		rLength += r;
		a = b;
	}
exit:
	RETURN_RC(CCurve::GetLength, rc);
}


/*******************************************************************
 *
 * Find a point on the curve by an arc length from the start of the 
 * curve.
 *
 * @Author: Stephen W. Hou
 * @Date:   April 6, 2004
 */
RC CCurve::FindPointByLength(double rLength,
		                     double &rParam,
						     C3Point &cPoint) const
{
	double curveLength;
	Length(curveLength);
	if (FZERO(curveLength, FUZZ_GEN))
		return FAILURE;

    if (rLength < 0.0 || rLength > curveLength)
        return FAILURE;

	RC rc;
	if (rLength == 0.0) 
		rc = GetStartPoint(cPoint);
	else if (rLength == curveLength) 
		rc = GetEndPoint(cPoint);
	else if (rLength > curveLength) 
		rc = FAILURE;
	else {
		double ratio = rLength / curveLength;
		double a = 1.0 - ratio;
		rParam = a * Start() + ratio * End();

		for (register i = 0; i < iIterations; i++) {
			double rArcLength;
			if ((rc = GetLength(Start(), rParam, rArcLength)) != SUCCESS)
				break;

			double delta = rArcLength - rLength;

			C3Point cTangent;
			if ((rc = Evaluate(rParam, cPoint, cTangent)) != SUCCESS)
				break;

			if (fabs(delta) < FUZZ_DIST) {
				rc = SUCCESS;
				break;
			}

			double rValue = cTangent.Norm();
			if (!FZERO(rValue, FUZZ_RAD)) 
				rParam -= delta / rValue;
			else {
				rc = FAILURE;
				break;
			}
		}
	}

    return rc;
}


/*******************************************************************
 *
 * Get the points on the curve by subdividing the domain of the curve 
 * uniformly in the parameter space.
 *
 * @Author: Stephen W. Hou
 * @Date:   April 6, 2004
 */
RC CCurve::GetPointsWithEqualInterval(int iNumSegments,			// In:  The number of segments
		                              CPointArray &ptArray,		// Out: The point array
									  CRealArray &parArray) const	// Out: The optional parameter array
{
	RC rc = SUCCESS;

    ASSERT(iNumSegments > 0);
	ptArray.RemoveAll();
	if (&parArray)
		parArray.RemoveAll();

	if (iNumSegments == 1)
		return rc;

    double delta = (End() - Start()) / iNumSegments;

	C3Point cPoint;
    for (register i = 1; i < iNumSegments; i++) {
        double param = Start() + delta * i;
        if ((rc = Evaluate(param, cPoint)) == SUCCESS) {
			ptArray.Add(cPoint);
			if (&parArray)
				parArray.Add(param);
		}
		else {
			ptArray.RemoveAll();
			if (&parArray)
				parArray.RemoveAll();
			break;
		}
    }
	if (rc == SUCCESS && Closed()) {
		GetEndPoint(cPoint);
		ptArray.Add(cPoint);
		if (&parArray)
			parArray.Add(End());		
	}
	return rc;
}


/*******************************************************************
 *
 * Get the points on the curve by subdividing the length of the curve 
 * uniformly in the model space.
 *
 * @Author: Stephen W. Hou
 * @Date:   April 6, 2004
 */
RC CCurve::GetPointsWithEqualArcLength(int iNumSegments,			// In:  The number of segments	
		                               CPointArray &ptArray,		// Out: The point
									   CRealArray &parArray) const	// Out: The optional parameter array
{
	RC rc = SUCCESS;

	ptArray.RemoveAll();
	if (&parArray)
		parArray.RemoveAll();

	ASSERT(iNumSegments > 0);
    if (iNumSegments == 1)
	    return rc;

	double length;
	if (Length(length))
		return FAILURE;

    double delta = length / iNumSegments;

	double param, arcLength;
	C3Point point;
    for (register i = 1; i < iNumSegments; i++) {
        arcLength = delta * i;
        if ((rc = FindPointByLength(arcLength, param, point)) == SUCCESS) {
			ptArray.Add(point);
			if (&parArray)
				parArray.Add(param);
		}
		else {
			ptArray.RemoveAll();
			if (&parArray)
				parArray.RemoveAll();
			break;
		}
    }
	if (rc == SUCCESS && Closed()) {
		GetEndPoint(point);
		ptArray.Add(point);
		if (&parArray)
			parArray.Add(End());		
	}
	return rc;
}	