/*	TheUser.CPP - Implememnts CUser and CStorage  
 *	Copyright (C) 1997-98 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * This file has a static instance of CUser, and a pointer to the current user.  The
 * pointer is initialized to the static one, and a function is supplied to register
 * a different one.
 * CStorage is an almost-abstract class: Its methods are all virtual, but they are all 
 * stubbed so that their derived classes don't have to implement what they don't use
 * 	
 *
 */
#include "Geometry.h"		
#include "GeomPtrs.h"		
#include "TheUser.h"		

DB_SOURCE_NAME;

CUser cNoUser;				// Default do-nothing user
PUser pTheUser = &cNoUser;	// The address for user registration

void RegisterUser(
	PUser pUser)		// In: The user to be registeed
	{
	if (pUser)
		pTheUser =  pUser;
	}

PUser TheUser()
	{
	return pTheUser;
	}
/////////////////////////////////////////////////////////////////////////////////
// CStorage
// Initiate a path - do-nothing stub
RC CStorage::InitiateCurve(
	PCCurve pCurve)          // In: A curve
	{
	// This stub should have never been called
	RETURN_RC(CStorage::InitiateCurve, FAILURE);
	}
/*******************************************************************************/
// Initiate a path with a NURBS curve- do-nothing stub
RC CStorage::InitiateNURBS(
	PCNURBS pSpline)          // In: A spline
	{
	// This stub should have never been called
	RETURN_RC(CStorage::InitiateNURBS, FAILURE);
	}
/*******************************************************************************/
// Accept a line - do-nothing stub
RC CStorage::AcceptLine(
	PCLine pLine)          // In: A line
	{
	// This stub should have never been called
	RETURN_RC(CStorage::AcceptLine, FAILURE);
	}
/*******************************************************************************/
// Accept a line - do-nothing stub
RC CStorage::AcceptUnboundedLine(
	PCLine pLine)          // In: A line
	{
	// This stub should have never been called
	RETURN_RC(CStorage::AcceptUnboundedLine, FAILURE);
	}
/*******************************************************************************/
// Accept an elliptical arc - do-nothing stub
RC CStorage::AcceptEllipArc(
	PCEllipArc pArc)          // In: An elliptical arc
	{
	// This stub should have never been called
	RETURN_RC(CStorage::AcceptEllipArc, FAILURE);
	}
/*******************************************************************************/
// Accept an ellipse - do-nothing stub
RC CStorage::AcceptEllipse(
	PCEllipArc pArc)          // In: An elliptical arc
	{
	// This stub should have never been called
	RETURN_RC(CStorage::AcceptEllipse, FAILURE);
	}
/*******************************************************************************/
// Accept a polyline - do-nothing stub
RC CStorage::AcceptPolyLine(
	PCPolyLine pPoly)          // In: A polyline
	{
	// This stub should have never been called
	RETURN_RC(CStorage::AcceptPolyLine, FAILURE);
	}
/*******************************************************************************/
// Accept a NURBS - do-nothing stub
RC CStorage::AcceptNURBS(
	PCNURBS pNurbs)          // In: A NURBS curve
	{
	// This stub should have never been called
	RETURN_RC(CStorage::AcceptNURBS, FAILURE);
	}
/*******************************************************************************/
// Accept a gap - do-nothing stub
RC CStorage::AcceptGap(
	PCGap pGap)          // In: A gap
	{
	// This stub should have never been called
	RETURN_RC(CStorage::AcceptGap, FAILURE);
	}
/*******************************************************************************/
// Initiate a new shape
RC CStorage::NewShape()
	{
	// This stub should have never been called
	RETURN_RC(CStorage::NewShape, FAILURE);
	}
/*******************************************************************************/
// Add a path to the shape
RC CStorage::NewPath(
	BOOL bFillable)		// In: Make it fillable if TRUE
	{
	// This stub should have never been called
	RETURN_RC(CStorage::NewPath, FAILURE);
	}
/*******************************************************************************/
// Acept a new curve to the current path
RC CStorage::AcceptCurve(
	PCurve pCurve,	// In: The curve
	GeoID id)		// In: Reference ID
	{
	// This stub should have never been called
	RETURN_RC(CStorage::AcceptCurve, FAILURE);
	}
/*******************************************************************************/
// Wrap up the new shape
RC CStorage::WrapUpPath()
	{
	// This stub should have never been called
	RETURN_RC(CStorage::WrapUpPath, FAILURE);
	}

/*******************************************************************************/
// Wrap up the new shape
RC CStorage::WrapUpShape(
	GeoID idSource,	 // In: id of shape to inherit attributes from
	BOOL bSkipText,	 // In: =TRUE if text is to be ignored
	BOOL bSkipFill)  // In: =TRUE if the fill is to be ignored
	{
	// This stub should have never been called
	RETURN_RC(CStorage::WrapUpShape, FAILURE);
	}

/////////////////////////////////////////////////////////////////////////////////
// CCache
// Accept a point - do-nothing stub
RC CCache::Accept(
	const C3Point & Point,   // In: The point to add
	double t)				 // In: Parametner value (ignored here)
	{
	// This stub should have never been called
	RETURN_RC(CCache::Accept, FAILURE);
	}
/*******************************************************************************/
// Accept a curve - do-nothing stub
RC CCache::Accept(
	const PCurve pCurve)	// In: The curve to add
	{
	// This stub should have never been called
	RETURN_RC(CCache::Accept, FAILURE);
	}
/*******************************************************************************/
// Accept a ray - Default implementation
RC CCache::Accept(
	const C3Point & Base,		// In: The ray's Base
	const C3Point & Direction)	// In: The Ray direction
	{
// The default implemetation is to clip the ray to short integer limits.
	C3Point Limit;
	double a, b;

	// X clipping
	if (FZERO(Direction.X(), FUZZ_GEN))
		a = SHRT_MAX;
	else
		if (Direction.X() < 0.0)
			a = (SHRT_MIN - Base.X()) / Direction.X();
		else
			a = (SHRT_MAX - Base.X()) / Direction.X();

	// Y clipping
	if (FZERO(Direction.Y(), FUZZ_GEN))
		b = SHRT_MAX;
	else
		if (Direction.Y() < 0.0)
			b = (SHRT_MIN - Base.Y()) / Direction.Y();
		else
			b = (SHRT_MAX - Base.Y()) / Direction.Y();

	// Take the minimum of the two
	if (a > b)
		a = b;
	Limit = Base + a * Direction;

	RC rc = Accept(Limit, 0.0);
	RETURN_RC(CCache::Accept, rc);
	}
/*******************************************************************************/
// Skip to a new point
RC CCache::SkipTo(
	const C3Point & P)   // In: The point to add
	{
	// The default behavior is to draw a line to the new point
	RC rc = Accept(P, 0);

	RETURN_RC(CCache::Accept, rc);
	}
/*******************************************************************************/
// Wrap up - do-nothing stub
RC CCache::WrapUp()		
	{
	// This stub should have never been called
	RETURN_RC(CCache::WrapUp, FAILURE);
	}
/*******************************************************************************/
// Reset for a fersh statr - do-nothing stub
RC CCache::Reset()		
	{
	// This stub should have never been called
	RETURN_RC(CCache::Reset, FAILURE);
	}
/*******************************************************************************/
// Have we started? - do-nothing stub
BOOL CCache::Started()
	{
	return TRUE;
	}

