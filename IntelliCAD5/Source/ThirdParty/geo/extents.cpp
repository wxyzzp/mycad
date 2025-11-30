/*	Extents.cpp - Implements the class CExtents
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * See Extents.h for the definition of the class.
 * This file implements its methods		
 *
 */
#include "Geometry.h"
#include <float.h>		// for DBL_MAX
#include "Point.h"
#include "Trnsform.h"	// For CTransform
#include "Extents.h"

DB_SOURCE_NAME;

/////////////////////////////////////////////////////////////////////////////////
// Utilities

// Check if two intervals [A,B] and [C,D] overlap
inline BOOL IntervalsOverlap(
	double rA,   // In: First interval's start
	double rB,   // In: First interval's end
	double rC,   // In: Second interval's start
	double rD)   // In: Second interval's end
	{
	return !( (rB < rC - FUZZ_DIST)   ||   (rA > rD + FUZZ_DIST) );
	}
/*************************************************************************/
// Implementation of CExtents
// Constructor/destructor
CExtents::CExtents()
	{
	Reset();
	}

// Construct with given initial setting
CExtents::CExtents(
	double rMinX,        // IN: Lowest x value 
	double rMaxX,        // IN: Highest x value
	double rMinY,        // IN: Lowest y value  
	double rMaxY)        // IN: Highest y value
	{
	m_rMinX = rMinX;
	m_rMaxX = rMaxX;
	m_rMinY = rMinY;
	m_rMaxY = rMaxY;
	}

CExtents::~CExtents()
	{
	}
/*************************************************************************/
// Reset for a new extents calculation
void CExtents::Reset()
	{
	m_rMinX = m_rMinY = DBL_MAX;
	m_rMaxX = m_rMaxY = -DBL_MAX;
	}
/*************************************************************************/
// Update with another box
void CExtents::Update(
	CExtents & cOther)	// In: Other extents
	{
	if (cOther.m_rMinX < m_rMinX)
		m_rMinX = cOther.m_rMinX;
	if (cOther.m_rMaxX > m_rMaxX)
		m_rMaxX = cOther.m_rMaxX;

	if (cOther.m_rMinY < m_rMinY)
		m_rMinY = cOther.m_rMinY;
	if (cOther.m_rMaxY > m_rMaxY)
		m_rMaxY = cOther.m_rMaxY;
	}
/*************************************************************************/
// Update the X extents with a point
void CExtents::UpdateX(
	const C3Point & P)	// In: A point
	{
	if (P.X() < m_rMinX)
		m_rMinX = P.X();
	if (P.X() > m_rMaxX)
		m_rMaxX = P.X();
	}
/*************************************************************************/
// Update the Y extents with a point
void CExtents::UpdateY(
	const C3Point & P)	// In: A point
	{
	if (P.Y() < m_rMinY)
		m_rMinY = P.Y();
	if (P.Y() > m_rMaxY)
		m_rMaxY = P.Y();
	}
/*************************************************************************/
// Update with a point
void CExtents::Update(
	const C3Point & P)	// In: A point
	{
	if (P.X() < m_rMinX)
		m_rMinX = P.X();
	if (P.X() > m_rMaxX)
		m_rMaxX = P.X();

	if (P.Y() < m_rMinY)
		m_rMinY = P.Y();
	if (P.Y() > m_rMaxY)
		m_rMaxY = P.Y();
	}
/*************************************************************************/
// Update with a transformed point
void CExtents::Update(
	const C3Point & P,		// In: A point
	PTransform pTransform)	// In: Transformation
	{
	C3Point Q;
	if (pTransform)
		pTransform->ApplyToPoint(P, Q);
	else
		Q = P;
	Update(Q);
	}
/*************************************************************************/
// Check if this box overlaps with another box
BOOL CExtents::Overlap(
	const CExtents & cOther) const
	{
	return IntervalsOverlap(m_rMinX, m_rMaxX, cOther.m_rMinX, cOther.m_rMaxX)
	   &&  IntervalsOverlap(m_rMinY, m_rMaxY, cOther.m_rMinY, cOther.m_rMaxY);
	}
/*************************************************************************/
// Get the extents along a given vector
void CExtents::GetMinMax(
	const C3Point& V,	// In: A vector
	double& rMin,		// Out: The minimimum of P*U within the extents
	double& rMax)		// Out: The maximimum of P*U within the extents
	{
	double r = V.X() * m_rMinX;
	double s = V.Y() * m_rMinY;
	rMin = rMax = r + s;

	double t = V.Y() * m_rMaxY;
	double u = r + t;
	if (u < rMin)
		rMin = u;
	else
		rMax = u;

	r = V.X() * m_rMaxX;
	u = r + s;
	if (u < rMin)
		rMin = u;
	if (u > rMax)
		rMax = u;

	u = r + t;
	if (u < rMin)
		rMin = u;
	if (u > rMax)
		rMax = u;
	}
/*************************************************************************/
// Check if this box contains a given point
BOOL CExtents::Contain(
	const C3Point & P) const
	{
	return P.X() > m_rMinX  &&  P.X() < m_rMaxX 
	&&	   P.Y() > m_rMinY  &&  P.Y() < m_rMaxY; 
	}
/*************************************************************************/
// Check if this box contains a given box (including edges)
BOOL CExtents::Contain(
	const CExtents & cOther) const
	{
	return	cOther.m_rMinX >= m_rMinX  &&  cOther.m_rMaxX <= m_rMaxX  &&
			cOther.m_rMinY >= m_rMinY  &&  cOther.m_rMaxY <= m_rMaxY; 
	}
/*************************************************************************/
// Check if this box overlaps with another box
void CExtents::Inflate(
	double r)	// In: Value to inflate by
	{
	m_rMinX -= r;
	m_rMaxX += r;
	m_rMinY -= r;
	m_rMaxY += r;
	}
/*************************************************************************/
// Containment test for a possibly rotated box
PointBoxPosition IsPointInBox(	// Point's position relative to the box
	const C3Point & P,	// In: The point to be tested
	P3Point Q,			// In: Box's 3 consecutive corners
	double tolerance)	// In: Tolerance for being on edge or vertex

	// If the point is within tolerance from an edge or vertex then 
	// OnBoxEdge or OnBoxVertex will be returned even if it's inside.
	{
	// Get a coordinate basis
	C3Point A = Q[0] - Q[1];	// Box edge vector
	double a = A.Norm();		// Its length
	BOOL aOn = a < FUZZ_GEN;

	C3Point B = Q[2] - Q[1];	// Other edge vector
	double b = B.Norm();		// Its length
	BOOL bOn = b < FUZZ_GEN;
	C3Point V = P - Q[1];		// Vector from the origin to P

	if (aOn && bOn)
		{
		// The box collapses to a point
		if (V.Norm() < tolerance)
			return OnBoxVertex;
		else
			return OutsideBox;
		}

	// So at least one vector is nonzero, make it A, and unitize it
	if (aOn)
		{
		SWAP(A, B, C3Point);
		SWAP(a, b, double);
		bOn = aOn;
		}
	A /= a;

	// Get the other basis vector
	if (bOn)
		{
		B = A;
		B.TurnLeft();
		}
	else
		B /= b;

	// Determine V's situation in the direction of A 
	double r = V * A;
	if (r < -tolerance || r > a + tolerance)
		return OutsideBox;
	aOn  = (r < tolerance  || r > a - tolerance);


	// Get P's situation in the direction of B
	r = V * B;			// P's position along edge E
	if (r < -tolerance || r > b + tolerance)
		return OutsideBox;
	bOn = (r < tolerance  || r > b - tolerance);

	// Look at both direction and determine position
	if (aOn)
		if (bOn) 
			return OnBoxVertex;
		else
			return OnBoxEdge;
	else
		if (bOn) 
			return OnBoxEdge;
		else
			return InsideBox;
	}
