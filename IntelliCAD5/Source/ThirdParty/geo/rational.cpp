/*	Rational.cpp - Rational spline function
 *	Copyright (C) 1996-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 * This is the implementation of a real-valued NURBS function.
 * See Rationa.h for its definition.
 *
 */
#include "Geometry.h"
#include "Array.h"		// For CRealArray
#include "knots.h"		// For CKnots
#include "TheUser.h"	// For CStorage
#include "Ctrlpts.h"	// For CControlPoints
#include "NURBS.h"		// For CNURBS
#include "Rational.h"

DB_SOURCE_NAME

// Default constructor destructor
CRationalFunction::CRationalFunction()
	{
	m_pCurve = NULL;
	}
CRationalFunction::~CRationalFunction()
	{
	delete m_pCurve;
	}
/*****************************************************************************/
// Construct a monotone linear rationl Hermite interpolatin NURBS function
CRationalFunction::CRationalFunction(
	const CRealArray & cX,	// In: Data nodes - must be strictly increasing
	const CRealArray & cY,	// In: Data values - must be monotone (see below)
	const CRealArray & cDY,	// In: The derivatives - must be monotone
	RC & rc)				// Out: Return code
	{
/*
 * See "Monotone Linear Rational Spline Interpolation" by R.D.Fuhr and M.Kallay,
 * CAGD 9(1992) p.313 for details.  It is assumed that the input data is 
 * stricly monotone in the sense that Yi+1 - Yi and DYi are either all positive
 * or all negative for all i.
 */
	int i, sign;
	double rWb;
	PKnots pKnots = NULL;
	PControlPoints pCoefficients = NULL;
	m_pCurve = NULL;

/*
	DUMP(cX);
	DUMP(cY);
	DUMP(cDY);
*/

	// input validity check
	if (cY.Size() < cX.Size()  ||   cDY.Size() < cX.Size()  || cX.Size() < 2)
		{
		rc = E_BadInput;
		QUIT
	    }
	
	// Allocate knots and coefficients
	pKnots = new CKnots(1, cX.Size()*2 - 1, rc);
	if (rc)
		QUIT
	pCoefficients = new CControlPoints(pKnots->Size(), rc);
	if (!pKnots  ||  !pCoefficients)
		rc = DM_MEM;
	if (rc)
		QUIT
	
	// input validity check
	if (FZERO(cDY[0], FUZZ_GEN))
		{
		rc = E_BadInput;
		QUIT
	    }
	
	if ( cDY[0] > 0 )  
		sign =  1;
	else                   
		sign = -1;
	
	// first knots and point
	rWb = 1.0;
	pKnots->Set(0, cX[0]);
	pKnots->Set(1, cX[0]);
	pCoefficients->Set(0, C3Point(cY[0], rWb));
	
	// loop data
	for (i = 1;   i < cX.Size();   i++)
		{
		double rYc;
		double rWa = rWb;
		double rWc;
		double rDeltaX = cX[i] - cX[i-1];
		double rDeltaY = cY[i] - cY[i-1];
		
		//  Check monotonicity of nodes
		if (sign*cDY[i] < FUZZ_GEN || sign*rDeltaY < FUZZ_GEN  ||	rDeltaX < FUZZ_GEN)
			{
			rc = E_BadInput;
			QUIT;
			}
		
		// knot, point and weight at cX[i]
		pKnots->Set(2*i+1, cX[i]);
		rWb = rWa * sqrt(cDY[i-1] / cDY[i]);
		pCoefficients->Set(2*i, C3Point(cY[i] * rWb, rWb));
		
		// Knot, coefficient and weight half-way between data nodes
		rWc = rDeltaX * (rWa * cDY[i-1] + rWb * cDY[i]) / (2.0 * rDeltaY);
		rYc =  rWc * (rWa * cY[i-1] + rWb * cY[i] ) / (rWa + rWb);
		pCoefficients->Set(2*i - 1, C3Point(rYc, rWc));
		pKnots->Set(2*i, (cX[i-1] + cX[i]) / 2);
		}
	
	// Create the curve
	m_pCurve = new CNURBS(pKnots, pCoefficients, rc);
	if (m_pCurve)
		{
		pKnots = NULL;					// Now owned by m_pCurve
		pCoefficients = NULL;			// Now owned by m_pCurve
		m_rStart = m_pCurve->Start();	
		m_rEnd = m_pCurve->End();
		}
	else
		rc = DM_MEM;
/*	
#ifdef DEBUG
	m_pCurve)->Dump();

	// Validate the result
	DB_MSGX("Does the function interpolate the data?",());
	for (i = 0;  i < cX.Size();  i++)
		{
		double y, dy;
		Evaluate(cX[i], y, dy);
		DB_MSGX("Value: expected %lf, actual %lf",(cY[i], y));
		DB_MSGX("Derivative: expected %lf, actual %lf",(cDY[i], dy));
		}
#endif
*/
exit:	
	if (rc)
		{
		delete pKnots;
		delete pCoefficients;
		delete m_pCurve;
		m_pCurve = NULL;
		}
	}
/*****************************************************************************/
// Clone
PRealFunction CRationalFunction::Clone() const
	{
	PRationalFunction p = new CRationalFunction;
	if (p)
		{
		*p = *this;
		p->m_pCurve = m_pCurve->Clone();
		if (!p->m_pCurve)
			{
			delete p;
			p = NULL;
			}
		}
	return p;
	}
/************************************************************************/
// Evaluate
RC CRationalFunction::Evaluate(
	double t,		// In: Parameter value to evaluate at
	double& v)	// Out: Function value
	{
	RC rc;
	C3Point P;

	// Trim to the domain
	if (t > End())
		t = End();
	if (t < Start())
		t = Start();
	
	// Get the homogeneous  coordinate and divide X by Y.
	if (rc = m_pCurve->Evaluate(t, P))
		QUIT
	if (rc = FZERO(P.Y(), FUZZ_GEN))
		QUIT
	v = P.X() / P.Y();
exit:
	RETURN_RC(CRationalFunction::Evaluate, rc);
    }

#ifdef DEBUG
/************************************************************************/
// Dump
void CRationalFunction::Dump()
	{
	DB_MSGX("NURBS Function",());
	m_pCurve->Dump();
    }
#endif
/************************************************************************/
// Evaluate with derivative
RC CRationalFunction::Evaluate(
	double t,	// In: Parameter value to evaluate at
	double& v,	// Out: Function value
	double& dv)	// Out: Function derivative
	{
	RC rc;
	C3Point P, T;
	
	// Get the homogeneous  coordinate and divide X by Y.
	if (rc = m_pCurve->Evaluate(t, P, T))
		QUIT
	if (FZERO(P.Y(), FUZZ_GEN))
		{
		rc = E_InputZero;
		QUIT
		}
	v = P.X() / P.Y();
	dv = (T.X() - T.Y() * v) / P.Y();
exit:
	RETURN_RC(CRationalFunction::Evaluate, rc);
    }
