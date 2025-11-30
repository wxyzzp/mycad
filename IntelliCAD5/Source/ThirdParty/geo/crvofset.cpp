/*	CrvOfset.CPP - Implements the class COffsetCurve.
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * See CrvOfset.h for the definition of the class.
 * This file implements its methods		
 *
 */
#include "Geometry.h"
#include "Point.h"
#include "CrvOfset.h"

DB_SOURCE_NAME;


//Constructors and destructor
COffsetCurve::COffsetCurve()
    {
    m_pCurve = NULL;
    m_rOffset = 0;
    }

COffsetCurve::COffsetCurve(
	PCCurve pCurve,  // In: The base curve to be offset
	double rOffset,  // In: The offset amount (to the left)
	P3Point pUp)	 // In: Upward unit vector (NULL OK)
/* The offset curve will be incorrect if pCurve is not planar, or
   if pUp is not perpendicular to the curve's plane.  If pUp is NULL, 
   the the direction of the Z axis (0,0,1) will be used by default
*/
    {
    m_pCurve = pCurve;
    m_rOffset = rOffset;
	m_ptUp = CNormal(pUp);

	m_rStart = pCurve->Start();
	m_rEnd = pCurve->End();
    }

	// Copy constructor
COffsetCurve::COffsetCurve(
	const COffsetCurve & other)	// Other curve to copy
		{
		m_pCurve = other.m_pCurve;	// We don't own the base curve
		m_rOffset = other.m_rOffset;
		m_ptUp = other.m_ptUp;
		m_rStart = other.m_rStart;
		m_rEnd = other.m_rEnd;
		}

COffsetCurve::~COffsetCurve()
    {
    // We don't own the base curve, so we don't delete it
    }

PCurve COffsetCurve::Clone() const
	{
	return new COffsetCurve(*this);
	}
/**********************************************************************/
// Reset domain start
RC COffsetCurve::ResetStart(
	double rNew)  // In: New curve start
	{
	RC rc = SUCCESS;
	if (rNew < m_rEnd)
		m_rStart = rNew;
	else
		rc = E_BadInput;
	RETURN_RC(COffsetCurve::ResetStart, rc);
	}
/**********************************************************************/
// Reset domain end
RC COffsetCurve::ResetEnd(
	double rNew)  // In: New curve end
	{
	RC rc = SUCCESS;
	if (rNew > m_rStart)
		m_rEnd = rNew;
	else
		rc = E_BadInput;
	RETURN_RC(COffsetCurve::ResetEnd, rc);
	}
/**********************************************************************/
// Set new domain bounds
RC COffsetCurve::SetDomain(
	double rFrom,  // In: New curve start
	double rTo)    // In: New curve end
	{
	RC rc = SUCCESS;

	if (rFrom > rTo)
		rc = E_BadInput;
	else
		{
		m_rStart = rFrom;
		m_rEnd = rTo;
		}

	RETURN_RC(COffsetCurve::SetDomain, rc);
	}
/**********************************************************************/
// Evaluator - point only
RC COffsetCurve::Evaluate(
	double rAt,              // In: parameter value to evaluate at
	C3Point & ptPoint) const // Out: Point on the curve 
    {
    C3Point N;
    double t;
    RC rc = SUCCESS;

    // Get a point and a left normal on the curve
    if (rc = m_pCurve->Evaluate(rAt, ptPoint, N))
    	goto exit;
    N.TurnLeft(&m_ptUp);
    
    // Unitize the normal
    t = N.Norm();
    if (t < FUZZ_GEN)
    	{
    	rc = E_ZeroVector;
    	goto exit;
		}
    
    // Compute the offset
    ptPoint = ptPoint + (m_rOffset / t) * N;
exit:    
	RETURN_RC(COffsetCurve::Evaluate, rc);
    }
/************************************************************************/
// Evaluator - point and tangent vector
RC COffsetCurve::Evaluate(
	double rAt,                // In: Parameter value to evaluate at
	C3Point & ptPoint,		   // Out: Point on the line with that parameter
	C3Point & ptTangent) const // Out: Tangent vector at that point
    {
/*
 * In the following ^2 stands for "squared", and B stands for the normal to 
 * the plane in which we do the offset
 * The offset curve is defied as:  C + rN, where N = (C'X B) / Speed,
 * and Speed = sqrt(C'*C').
 * Its derivative is therefore C' + rN'. 
 * Now N' = (C"X B) / Speed - (C'X B)Speed'/Speed^2 = 
 *          (C"X B - N * Speed') / Speed, 
 * and Speed' = C'*C" / Speed 
 */          
    C3Point ptVals[3];
    double rSpeed, rDSpeed;
    RC rc;

    // Get a point and first 2 derivatives on the base curve
    if (rc = m_pCurve->Evaluate(rAt, 2, ptVals))
    	goto exit;
    ptTangent = ptVals[1];  // Storing it for later
    
    // Get the speed and its derivative
    rSpeed = ptTangent.Norm();
    if (rSpeed < FUZZ_GEN)
		{
    	rc = E_ZeroVector;
    	goto exit;
    	}    
    ptVals[1] /= rSpeed;	
    rDSpeed = ptVals[2] * ptVals[1];	

    // Turn the derivatives left
    ptVals[1].TurnLeft(&m_ptUp);   // Now it's a left unit normal
    ptVals[2].TurnLeft(&m_ptUp);
                          	    
    // Compute the offset point
    ptPoint = ptVals[0] + m_rOffset * ptVals[1];
    

// The offset's first derivative = (x',y') + (Offset/Speed)((-y",x") - N*Speed')
    ptVals[1] *= rDSpeed;
    ptVals[2] -= ptVals[1];
    ptVals[2] *= (m_rOffset / rSpeed);
    ptTangent += ptVals[2];
exit:    
	RETURN_RC(COffsetCurve::Evaluate, rc);
    }
/************************************************************************/
// Seeds for equations solving
RC COffsetCurve::Seeds(
	CRealArray & arrSeeds,  // In/Out: Array of seeds
	double* pStart,			// In: Domain start (optional, NULL OK)
	double* pEnd) const		// In: Domain end (optional, NULL OK)
    {
	double a = Start();
	double b = End();

	if (pStart)
		a = *pStart;
	if (pEnd)
		b = *pEnd;

	// Get the base curve's seeds between the start & end of the offset
	RC rc = m_pCurve->Seeds(arrSeeds, &a, &b);

	RETURN_RC(COffsetCurve::Seeds, rc);
    }
/************************************************************************/
// Evaluator - point, tangent and curvature
RC COffsetCurve::Evaluate(
	double rAt,                 // In: Where to evaluate
	C3Point & ptPoint,			// Out: Point there
	C3Point & ptTangent,		// Out: Unit tangent vector there
	double & rCurvature,		// Out: Curvature there
	double * pSpeed,			// Out: Speed there (optional, NULL OK)
	P3Point pUp) const			// In: Upward normal (optional, NULL OK)
    {
	double r, rSpeed;
	RC rc = m_pCurve->Evaluate(rAt, ptPoint, ptTangent, 
							  rCurvature, &rSpeed, pUp);

	// Offset the point
	C3Point ptNormal(ptTangent);
	if (rc)
		{
		pSpeed = NULL;
		QUIT
		}
	ptNormal.TurnLeft(pUp);
	ptPoint = ptPoint + ptNormal * m_rOffset;

/* The radius of curvature and speed are adjusted from the base curve's
   speed and curvature by the offset distance, as follows:

  1/(offset's curvature) = 1/curvature - distance,
  offset's speed = speed * curvature / (offset curvature)

  so: 
  offset's curvature = curvature / (1 - distance * curvature)
  offset's speed = (base speed) * (1 - distance * curvature)
*/
	r = 1 - m_rOffset * rCurvature;	
	if (fabs(r) * INFINITY < fabs(rCurvature))
		{
		rSpeed = 0;
		if (r * rCurvature > 0)
			rCurvature = INFINITY;
		else
			rCurvature = -INFINITY;
		}
	else
		{
		rCurvature /= r;
		rSpeed *= r;
		if (rSpeed < 0)
			{
			ptTangent = -ptTangent;
			rSpeed = -rSpeed;
			}
		}
exit:
	if (pSpeed)
		*pSpeed = rSpeed;
 	RETURN_RC(COffsetCurve::Evaluate, rc);
    }
