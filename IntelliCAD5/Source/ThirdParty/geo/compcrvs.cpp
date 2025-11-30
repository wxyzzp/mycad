/*	CompCrvs.cpp - Implements the classes of composed curves
 *	Copyright (C) 1994-98 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * The classes defined here are of curves that are composed from:
 *		a curve and a transformation - CTransformedCurve
 *		a parameter mapping and a curve - CMappedCurve
 *		a chain of contiguous curves - CCompositeCurve
 *
 * This file implements their methods, see CompCrvs.h for the definitions		
 *
 */

#include "Geometry.h"
#include "Point.h"
#include "CompCrvs.h"

DB_SOURCE_NAME;

////////////////////////////////////////////////////////////////////////
//							Transformed Curve						  //
////////////////////////////////////////////////////////////////////////
// Default onstructor & destructor
CTransformedCurve::CTransformedCurve()
	{
	m_pCurve = NULL;
	m_pTransform = NULL;
	}
/*********************************************************************/
// Construct from a curve and a transformation
CTransformedCurve::CTransformedCurve(
	PCCurve		pCurve,		// In: The curve to be transformed
	PTransform	pTransform,	// In: The transformation
	RC & rc)				// Out: Return code
	{
	if (!pCurve)
		{
		m_pTransform = NULL;
		m_pCurve = NULL;
		rc = E_BadInput;
		}
	else
		{
		m_pCurve = pCurve;
		m_pTransform = pTransform;
		rc = SUCCESS;
		}
	}
/************************************************************************/
PCurve CTransformedCurve::Clone() const
	{
	RC rc;

	PTransformedCurve pClone = new CTransformedCurve(m_pCurve, m_pTransform, rc);
	if (rc)
		{
		delete pClone;
		pClone = NULL;
		}

	return pClone;
	}
/************************************************************************/
// Evaluator - point and n derivatives
RC CTransformedCurve::Evaluate(
	double rAt,					// In: Where to evaluate
	C3Point & ptPoint) const	// Out: Position there
    {
	RC rc;
	if (rc = m_pCurve->Evaluate(rAt, ptPoint))
		QUIT
	m_pTransform->ApplyToPoint(ptPoint, ptPoint);
exit:
	RETURN_RC(CTransformedCurve::Evaluate, rc);
    }
#ifdef DEBUG
/***********************************************************************/
// Dump
void CTransformedCurve::Dump() const
	{
	DB_MSGX("Base curve:",());
	m_pCurve->Dump();
	DB_MSGX("Transformation:",());
	m_pTransform->Dump();
	}
#endif
/////////////////////////////////////////////////////////////////////
//			      Implementation of CMappedCurve                  //
/////////////////////////////////////////////////////////////////////
//Constructors and destructor
CMappedCurve::CMappedCurve()
    {
    m_pCurve = NULL;
	m_pMapping = NULL;
    }

CMappedCurve::CMappedCurve(
	PCurve pCurve,			   // In: The base curve to be offset
	PRealFunction pMapping)    // In: The offset amount (to the left) 
    {
    m_pCurve = pCurve;
    m_pMapping = pMapping;
    }

// Copy constructor
CMappedCurve::CMappedCurve(
	const CMappedCurve & cOther) // In: The other curve to copy
    {
    m_pCurve = cOther.m_pCurve;
    m_pMapping = cOther.m_pMapping->Clone();
    }

CMappedCurve::~CMappedCurve()
    {
    // We don't own the base curve, so we don't delete it
	delete m_pMapping;
    }
/***********************************************************************/
// Clone
PCurve CMappedCurve::Clone() const
	{
	return new CMappedCurve(*this);
	}
/***********************************************************************/
// Evaluator - point only
RC CMappedCurve::Evaluate(
	double rAt,               // In: parameter value to evaluate at
	C3Point & ptPoint) const  // Out: Point on the curve 
    {
	double t;

	// Map the parameter
	RC rc = m_pMapping->Evaluate(rAt, t);

	// Get the point on the curve
    if (!rc)
		rc = m_pCurve->Evaluate(t, ptPoint);

	RETURN_RC(CMappedCurve::Evaluate, rc);
    }
/***********************************************************************/
// Evaluator - point and derivative
RC CMappedCurve::Evaluate(
	double rAt,					// In: parameter value to evaluate at
	C3Point & ptPoint,			// Out: Point at that parameter
	C3Point & ptTangent)  const // Out: Tangent vector at that point
    {
	double t, dt;
	RC rc;

	// Map the parameter
	if (rc = m_pMapping->Evaluate(rAt, t, dt))
		QUIT

	// Get the point on the curve, applying the chain rule
	rc = m_pCurve->Evaluate(t, ptPoint, ptTangent);
	ptTangent = ptTangent * dt;
exit:
	RETURN_RC(CMappedCurve::Evaluate, rc);
    }
#ifdef DEBUG
/***********************************************************************/
// Dump
void CMappedCurve::Dump() const
	{
	DB_MSGX("Base curve:",());
	m_pCurve->Dump();
	DB_MSGX("Mapping:",());
	m_pMapping->Dump();
	}
#endif

//////////////////////////////////////////////////////////////////////////////////
//						Implementation of CCompositeCurve						//
//////////////////////////////////////////////////////////////////////////////////
// Default constructor
CCompositeCurve::CCompositeCurve()
	{
	m_rStart = m_rEnd = 0;
	}
/**********************************************************************************/
// Construct from a path
CCompositeCurve::CCompositeCurve(
	PPath pPath,	// In: The path
	int  iFrom,		// In: The index of the first curve
	int  iTo,		// In: The index of the last curve
	RC & rc)		// Out: Return code
{
	rc = SUCCESS;
	m_rStart = m_rEnd = 0.0;

	//Loop on the curves
    PCurve pCurve = NULL;
    double a, b/*, lastKnot*/;
	for (int i = iFrom;	  i <= iTo;  i++) {
		pCurve = pPath->Curve(i)->Clone();
		a = (i == 0) ? pPath->GetStartTrim() : pCurve->Start();
		b = (pCurve == pPath->Last()) ? pPath->GetEndTrim() : pCurve->End();

        if (rc = Add(pCurve, a, b))
			QUIT
	}
    return;

exit:
    if (pCurve)
	    delete pCurve;
}

/**********************************************************************************/
// Copy constructor
CCompositeCurve::CCompositeCurve(
	const CCompositeCurve & cOther)
	:CCurve(cOther)
    {
    m_cCurves = cOther.m_cCurves;
    m_cKnots = cOther.m_cKnots;

	m_rStart = cOther.m_rStart;
	m_rEnd = cOther.m_rEnd;
	}
/**********************************************************************************/
// Destructor
CCompositeCurve::~CCompositeCurve()
{
}
/**********************************************************************************/
// Clone
PCurve CCompositeCurve::Clone() const
	{
	return new CCompositeCurve(*this);
	}
/**********************************************************************************/
// Get a point
RC CCompositeCurve::Evaluate(
	double rAt,					// In: Parameter value
	C3Point & ptPoint) const	// Out: A point on the base path
	{
	RC rc;

	// Find the cloest knot before the point. Note, the first knot in the array
    // is corresponding to the end point of the first segment.
    //
	int n = FindSpan(rAt);

	// Find the parameter value to evaluate at
    if (n >= 0) 
	    rAt -= m_cKnots[n];

    // Get the start point of the curve
    //
    rAt += m_cCurves.Curve(++n)->Start();

	// Get a point on the appropriate curve
	rc = m_cCurves.Curve(n)->Evaluate(rAt, ptPoint);

    RETURN_RC(CCompositeCurve::Evaluate, rc);
	}
/**********************************************************************************/
// Get a point and a tangent vector on the base path
RC CCompositeCurve::Evaluate(
	double rAt,					// In: Parameter value
	C3Point & ptPoint,			// Out: A point on the base path
	C3Point & ptTangent) const	// Out: Tangent vector there
	{
	RC rc;

	// Find the cloest knot before the point. Note, the first knot in the array
    // is corresponding to the end point of the first segment.
    //
	int n = FindSpan(rAt);

	// Find the parameter value to evaluate at
    if (n >= 0)
	    rAt -= m_cKnots[n];

    // Get the start point of the curve 
    rAt += m_cCurves.Curve(++n)->Start();

	// Get a point and a tangent on the appropriate curve
	rc = m_cCurves.Curve(n)->Evaluate(rAt, ptPoint, ptTangent);

    RETURN_RC(CCompositeCurve::Evaluate, rc);
	}
/**********************************************************************************/
// Add a curve
RC CCompositeCurve::Add(
	PCurve pCurve,	// In: The curve to add
	double rFrom,	// In: Domain start
	double rTo)		// In: Domain end
	{
	RC rc;

    PCurve pCopy = NULL;

    if (!pCurve->PullIntoDomain(rFrom) || !pCurve->PullIntoDomain(rTo))
        QUIT

    if (m_cCurves.Size())
	    m_cKnots.Add(m_rEnd);

    pCopy = pCurve->Clone();
    if (!pCopy)
        QUIT

    pCopy->SetDomain(rFrom, rTo);

	if (rc = m_cCurves.Add(pCopy))
		QUIT

	m_rEnd += pCopy->End() - pCopy->Start();

	pCopy = NULL;
exit:
    if (pCopy)
	    delete pCopy;
	RETURN_RC(CCompositeCurve::Add, rc);
	}

/**********************************************************************************/
// Make a copy of the curve and then transform it.
//
// Author:  Stephen W. Hou
// Date:    July 4, 2004
//
RC CCompositeCurve::TransformCopy(
	CTransform & cTransform,	    // In: The transformation
	PCurve & pOut) const		    // Out: the transformed line
	{
	RC rc;
	if (rc = ALLOC_FAIL(pOut = Clone()))
		return FAILURE;
	rc = pOut->Transform(cTransform);	// in place

	if (rc) {
		delete pOut;
		pOut = NULL;
	}
	RETURN_RC(CCompositeCurve::TransformCopy, rc);
	}

/************************************************************************/
// Transform the curve
//
// Author:  Stephen W. Hou
// Date:    July 4, 2004
//
RC CCompositeCurve::Transform(CTransform & cTransform)   // In: The transformation
{
    return m_cCurves.Transform(&cTransform);
}

/************************************************************************/
// Reverse the direction of the curve.
//
// Author:  Stephen W. Hou
// Date:    July 4, 2004
//

RC CCompositeCurve::Reverse()
{
    RC rc = m_cCurves.Reverse();
	RETURN_RC(CCompositeCurve::Reverse, rc);
}

/************************************************************************/
// Determine if a ray hit the curve within tolerance. It returns TRUE
// with hit locations at the curve.
//
// Author:  Stephen W. Hou
// Date:    July 13, 2004
//

BOOL CCompositeCurve::HitTest(  // Return TRUE if the ray hits the cuvre
    const C3Point &ptThrough,   // In: Through point 
    const C3Vector &hitDir,     // In: Hit direction
    double rTolerance,          // In: The tolerance considered a hit
    double & rAt)               // Out: Hit location on the curve within tolerance
{
    double rParam;
    CCurve* pCurve = NULL;
    for (register i = 0; i < m_cCurves.Size(); i++) {
        pCurve = m_cCurves.Curve(i);
        if (pCurve->HitTest(ptThrough, hitDir, rTolerance, rParam)) {
            double rLowBound, rUpBound;
            if (GetSegmentDomain(i, rLowBound, rUpBound) == SUCCESS) {
	            rAt = rLowBound + rParam;
                return TRUE;
            }
        }
    }
    return FALSE;
}

/***************************************************************************/
// Return the parameter range of a segment.
//
// Author:  Stephen W. Hou
// Date:    7/14/2004
// 
RC CCompositeCurve::GetSegmentDomain( 
                        int index,              // In:  The index of segment(start from 0)
                        double &lowBound,       // Out: The low bound of parameter ranger
                        double &upBound) const  // Out: The up bound of parameter ranger
{
    ASSERT(index >= 0 && index <= m_cKnots.Size()); 

    lowBound = (index == 0) ? m_rStart : m_cKnots[index - 1];
    upBound = (index < m_cKnots.Size()) ? m_cKnots[index] : m_rEnd;
    return SUCCESS;
}

/**********************************************************************************/
//
// Locate a given parameter value in a span and returns a number greater than
// zero if it is found and -1 otherwise.
//
// Author:  Stephen W. Hou
// Date:    7/20/2004
//
int CCompositeCurve::FindSpan(double rAt) const
{
    int span = -1;
    for (register i = 0; i < m_cKnots.Size(); i++) {
        if (m_cKnots[i] <= rAt) 
            span = i;
        else
            break;
    }
    return span;
}

/**********************************************************************************/
// Find a 

#ifdef DEBUG
/**********************************************************************************/
// Dump
void CCompositeCurve::Dump() const
	{
	DB_MSGX("CCompositeCurve",());
	DB_MSGX("Curves:",());
	m_cCurves.Dump();
	DB_MSGX("Knots:",());
	m_cKnots.Dump();
	}
#endif
