/*	CrvOfset.h - Defines the class of offset curves
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 *
 * The offset by r of the curve C is the traject of normals of length r from points
 * on C :
 *                 Offset(t) = Curve(t) + r * Unit_Normal(t).
 *
 * There are 2 unit normals at a point, so we choose the one that is a left turn 
 * from the derivative (tangent vector), when looking down on the curve's plane. 
 * The "down" direction is the opposite of the vector m_ptUp.
 * To offset in the other direction take a negative value of r, or invret m_ptUp.
 * The offset curve uses the base curve, but it doesn't own it, that is, the offset
 * curve doesn't allocate and doesn't delete the base curve.  It's the 
 * programmer's responsibility to make sure that the base curve doesn't get
 * deleted from under the offset curve, and doesn't linger on unneccesarily.
 */
#ifndef CRVOFSET_H
#define CRVOFSET_H

#include "Curve.h"

class  GEO_CLASS COffsetCurve : public CCurve
{
// Construction destruction
public:
	COffsetCurve();

	COffsetCurve(
		PCCurve pCurve,		// In: The base curve to be offset
		double rOffset,		// In: The offset amount (to the left) 
		P3Point pUp=NULL);	// In: Upward unit vector (optional)

	COffsetCurve(
		const COffsetCurve & other);	// Other curve to copy

	virtual ~COffsetCurve();

	virtual PCurve Clone() const;

// Methods
public:
	double Start() const
		{
		return m_rStart;
		}

	double End() const
		{
		return m_rEnd;
		}

	double Period() const
		{
		return m_pCurve->Period();
		}

	BOOL Planar() const
		{
		return m_pCurve->Planar();
		}

	virtual RC ResetStart(
		double rNew);  // In: New curve start
	
	virtual RC ResetEnd(
		double rNew);  // In: New curve end

// Methods
	virtual RC SetDomain(
		double rFrom,  // In: New curve start
		double rTo);   // In: New curve end


	RC Evaluate (double, C3Point &) const;

	RC Evaluate (double, C3Point &, C3Point &) const;

	virtual RC Evaluate(
		double rAt,                 // In: Where to evaluate
		C3Point & ptPoint,			// Out: Point there
		C3Point & ptTangent,		// Out: Unit tangent vector there
		double & rCurvature,		// Out: Curvature there
		double * pSpeed=NULL,		// Out: Speed there (optional, NULL OK)
		P3Point pUp=NULL) const;	// In: Upward normal (optional, NULL OK)

	void SetOffset(
		double rAmount) // The mew offset amount (may be 0 or negaitve)
		{
		m_rOffset = rAmount;
		}

	RC Seeds(
		CRealArray & arrSeeds,		// In/Out: Array of seeds
		double* pStart=NULL,		// In: Domain start (optional, NULL OK)
		double* pEnd=NULL) const;	// In: Domain end (optional, NULL OK)

protected:
// Data
	PCCurve  m_pCurve;	 // The base curve
	double  m_rOffset;   // The offset distance (may be negative)
	CNormal	m_ptUp;		 // The upward unit vector
	double m_rStart;	 // Domain start
	double m_rEnd;		 // Domain end
};
#endif


