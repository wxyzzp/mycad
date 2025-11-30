/*	Roots.cpp - The file is concerned with functions and equations
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 * 
 * This file contains:
 *		The implementation of the class CRealFunction, with a 
 *		Newton-Raphson solver.
 *
 *		The implementation of its derived class CLinearFunction
 *
 *		Math utilities for finding some special nonlinear equations
 *      with one unknown, including a quadratic equation.
 *
 * See Roots.h for the class definitions.
 *
 */
#include "Geometry.h"
#include <Float.h>
#include "Roots.h"

DB_SOURCE_NAME;
////////////////////////////////////////////////////////////////////////
// Class CREalFunction
// Constructor and destructors
CRealFunction::CRealFunction()
	{
	m_rStart = m_rEnd = 0;
	m_nIterations = 20;
	m_rTolerance = FUZZ_GEN;
	m_rAccuracy = FUZZ_REAL;
	}

CRealFunction::~CRealFunction()
	{
	}

/************************************************************************/
// Evaluate - stub
RC CRealFunction::Evaluate(
	double t,    // In: Parameter value to evaluate at
	double & v)  // Out: Function value
	{
	// Stub - should have never been called
	RETURN_RC(CRealFunction::Evaluate, FAILURE);
    }
/************************************************************************/
// Evaluate function and first derivative - stub
RC CRealFunction::Evaluate(
	double t,    // In: Parameter value to evaluate at
	double & v,  // Out: Function value
	double & d)  // Out: Derivative
	{
	// Stub - should have never been called
	RETURN_RC(CRealFunction::Evaluate, FAILURE);
    }
/****************************************************************************/
// Newton Raphson solver with a seed
RC CRealFunction::NewtonRaphson(
	double rSeed,    // In: Initial guess
	double & rRoot)  // Out: The root
	{
	int i;
	RC rc = SUCCESS;
	double rFunction = DBL_MAX;
	double rDerivative = 1.0;
	double rCorrection;
	BOOL bHitStart = FALSE;
	BOOL bHitEnd = FALSE;


	// Iterations loop 
	rRoot = rSeed;
	for (i=1;  i<= m_nIterations;  i++)
		{
		if (rc = Evaluate (rRoot, rFunction, rDerivative))
			break;

		// Quit if the function or the derivative vanish
		if (FZERO(rFunction, m_rAccuracy)  ||  FZERO(rDerivative, FUZZ_GEN))
			break;

		// Newton-Raphson correction
		rCorrection = -rFunction / rDerivative;

		// Converged?
		if (FZERO(rCorrection, m_rAccuracy))
			break;

		// Make the correction, bracket it, and try again
		rRoot += rCorrection; 

		// Clip to the domain
		if (Clip(rRoot, bHitStart, bHitEnd))
			break;
	 	}
	if (!rc  &&  !FZERO(rFunction, m_rTolerance))
		// We failed to converge
		rc = E_NOMSG;
	
	RETURN_RC(CRealFunction::NewtonRaphson, rc);
	} 
#ifdef DEBUG
/***********************************************************************/
// Dump
void CRealFunction::Dump() const
	{
	DB_MSGX("Start=%lf, End=%lf",(m_rStart, m_rEnd));
	}
#endif
/****************************************************************************/
// Solve the quadratic equation  ax^2 + 2bx + c = 0
RC SolveQuadratic( // Return E_NOMSG if there is no real root
	double a,    // In: Quadratic coeffeicnt
	double b,    // In: Half the linear coefficent
	double c,    // In: The constant term
	double & r1, // Out: The root with smaller absolute value 
	double & r2) // Out: The root with greater absolute value
	{
/*
 * Bug 5019: Modified the test for small a to make it relative to the numerator,
 * rather than absolute.  For that, I moved it after the numerator is computed.
 * Bug 5019, 7/31/95, mxk
 */
    RC rc = E_NOMSG;  // Presumed guilty
    double d;

	// The discriminant
	d = b * b - a * c;
	if (d < 0)     // No real roots
		goto exit;
	d = sqrt(d);
	
	if (b > 0)
		d = -b - d;
	else
		d = -b + d;

    // How small is a?
    if (fabs(a) < d * FUZZ_REAL)
		{
		// How small is b?
		if (fabs(b) < fabs(c) * FUZZ_REAL)	
			// There is no equation
		    goto exit;

		// It's a linear equation
		r1 = r2 = - c / (2 * b);
		rc = SUCCESS;
		goto exit;
		}
		
	// General case - true quadratic
	r2 = d / a;
	
	// Use the fact that r1 * r2 = c / a
	r1 = c / d;
	rc = SUCCESS;
exit:
#ifdef DEBUG
	if (!rc)
		{
		// Validity checks
		ASSERT_ELSE(FZERO(a * r1*r1 + 2 * b * r1 + c, FUZZ_GEN), "r1 is no solution")
			{
			DB_MSGX("a=%lf, b=%lf, c=%lf, r1=%lf, value=%lf",
					(a, b, c, r1, a * r1*r1 + 2*b*r1 + c));
			}
		ASSERT_ELSE(FZERO(a * r1*r1 + 2 * b * r1 + c, FUZZ_GEN), "r2 is no solution")
			{
			DB_MSGX("a=%lf, b=%lf, c=%lf, r2=%lf, value=%lf",
					(a, b, c, r2, a * r2*r2 + 2*b*r2 + c));
			}
		ASSERT_ELSE(fabs(r1) <= fabs(r2), "r1 > r2")
			{
			DB_MSGX("a=%lf, b=%lf, c=%lf, r1=%lf, r2=%lf",(a, b, c, r1,r2));
			}
		}
#endif

	RETURN_RC(SolveQuadratic, rc);
	} 
/****************************************************************************/
// Solve the trigonetric equation  a cos(x) + b sin(a) + c = 0
RC SolveTrigEquation1( // Return E_NOMSG if there is no real root
	double a,    // In: Coeffeicnt of cos(x)
	double b,    // In: Coefficient of sin(x)
	double c,    // In: The constant term
	int & n,	 // Out: The number of roots - 0, 1 or 2
	double & r1, // Out: The smaller root
	double & r2) // Out: The greater root
	{
/*
 * The expression a cos(x) + b sin(x) can be written as r cos(x-t), where
 * r = sqrt(a^2 + b^2), and t = atan2(b,a). The equation can therefore be
 * written as cos(x-t) = -c/r so the 2 solutions are x = t +- acos(-c/r).
 */
    RC rc = FAILURE;
	n = 0;
 	double r = _hypot(a, b);
 	if (FZERO(r, FUZZ_GEN))  // Singular case, meaningless for us
 		goto exit;
 		
 	rc = SUCCESS;
 	r1 = atan2(b, a);   // = t in the explanation above
 	r = -c / r;
 	
 	if (fabs(r) > 1 + FUZZ_GEN)	// There is no root
 		goto exit;
 		
 	n = 1;
 	if (r > 1 - FUZZ_GEN)		// There is only one root: r1 = t
		goto exit;
		
 	if (r < -1 + FUZZ_GEN)		// There is only one root: r1 = t + PI
		{
		r1 += PI;
		goto exit;
		}
		
	n = 2;
	r = acos(r);				
	r2 = r1 + r;				// = t + acos(-c/r)
	r1 -= r;					// = t - acos(-c/r)
exit:
#ifdef DEBUG
	// Validity checks - relative accuracy of 1e-6
	r = MAX(fabs(a), fabs(b));
	r = MAX(r, fabs(c)) * .000001;
	if (n == 1)
		{
		ASSERT_ELSE(FZERO(a*cos(r1) + b*sin(r1) + c, r), "Invalid r1")
			{
			DB_MSGX("a=%lf, b=%lf, c=%lf, r1=%lf, value=%lf",
					(a, b, c, r1, a*cos(r1) + b*sin(r1) + c));
			}
		}
	
	if (n == 2)
		{
		ASSERT_ELSE(FZERO(a*cos(r1) + b*sin(r1) + c, r), "Invalid r2")
			{
			DB_MSGX("a=%lf, b=%lf, c=%lf, r2=%lf, value=%lf",
					(a, b, c, r2, a*cos(r2) + b*sin(r2) + c));
			}
		}
#endif
	RETURN_RC(SolveTrigEquation1, rc);
	} 
/**********************************************************************/
// Clip an iterating variable to the search domain
BOOL CRealFunction::Clip(	// =TRUE if this is a repeat offense
	double & t,				// In/Out: Tested number, clipped to the domain
	BOOL far & bHitStart, 	// In/Out: Was t beyond Start?
	BOOL far & bHitEnd)  	// In/Out: Was t beyond End?
/*
 * This function will clip t to the domain [rSart, rEnd] if it lies 
 * outside it, and this is not a repeat offense.  If it is a repeat offense
 * of then it will just return TRUE, and not change t.
 * Information on first offense is passed in and out through the arguments
 * bHitStart and bHitEnd. A an offense is only considered repeat if it is
 * the same side (Start/End).
 */
	{
	// Clip to Start
	if (t < m_rStart - FUZZ_GEN)
		{
		if (Period())
			{
			while (t < m_rStart)
				t += Period();
			return FALSE;
			}

		if (bHitStart)
			return TRUE;
		else
			{
			bHitStart = TRUE;
			t = m_rStart;
			}
		}
	else
		bHitStart = FALSE;

	// Clip to End
	if (t > m_rEnd + FUZZ_GEN)
		{
		if (Period())
			{
			while (t > m_rEnd)
				t -= Period();
			return FALSE;
			}

		if (bHitEnd)
			return TRUE;
		else
			{
			bHitEnd = TRUE;
			t = m_rEnd;
			}
		}
	else
		bHitEnd = FALSE;

	return FALSE;  // This is not a repeat offense
	}
	
/********************************************************************
 *					Class CLinearFunction							*
 ********************************************************************/

CLinearFunction::CLinearFunction() // default is linear (0,1) identity function
	{
	CRealFunction::CRealFunction();

	m_rStart = m_valStart = 0;
	m_rEnd = m_valEnd = 1.0;

	}

CLinearFunction::~CLinearFunction()
	{
	CRealFunction::~CRealFunction();
	}

// build from domain and range
CLinearFunction::CLinearFunction(	
	double rDomainStart,			// In: domain start
	double rDomainEnd,				// In: domain end
	double rRangeStart,				// In: range start
	double rRangeEnd)				// In: range end
	{
	CRealFunction::CRealFunction();

	m_rEnd = rDomainEnd;
	m_rStart = rDomainStart;
	m_valStart = rRangeStart;
	m_valEnd = rRangeEnd;
	}
/****************************************************************************/
// Evaluate  function value only
RC CLinearFunction::Evaluate(
	double rAt,        // In: Parameter value to evaluate at
	double & rValue)   // Out: The requested coordiante there
	{
	RC err = SUCCESS;
	double r = m_rEnd - m_rStart;
	
	if (FZERO(r, FUZZ_GEN))
		{
		rValue = m_valStart;
		err = FAILURE;
		}
	else // avoid dividing by zero. This would be degenerate case.
		rValue = ((rAt - m_rStart) * m_valEnd + (m_rEnd - rAt) * m_valStart) / r;

	RETURN_RC(CLinearFunction::Evaluate, err);
	}
/****************************************************************************/
// Evaluate with derivative
RC CLinearFunction::Evaluate(
	double rAt,             // In: Parameter value to evaluate at
	double & rValue,        // Out: The requested coordiante there
	double & rDerivative)   // Out: The derivative there
	{
	RC err = SUCCESS;
	double r = m_rEnd - m_rStart;
	
	if (FZERO(r, FUZZ_GEN))
		{
		rValue = m_valStart;
		rDerivative = 0.0; 
		err = FAILURE;
		}
	else
		{
		rValue = ((rAt - m_rStart) * m_valEnd + (m_rEnd - rAt) * m_valStart) / r;
		rDerivative = (m_valEnd - m_valStart) / r;
		}

	RETURN_RC(CLinearFunction::Evaluate, err);
	}
#ifdef DEBUG
/***********************************************************************/
// Dump
void CLinearFunction::Dump() const
	{
	CRealFunction::Dump();
	DB_MSGX("valStart=%lf, valEnd=%lf",(m_valStart, m_valEnd));
	}
#endif

