/*	TheUser.h - Defines the classes that represent the user of the geomety library
 *	Copyright (C) 1997 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 *
 * The "User" of geometry interacts with the library in 3 ways:
 *
 *	**	UI:  e.g. abort a lengthy operation such as curve/curve intersection. Represented 
 *		by the class CUser.  A default do-nothing user is initialized at compile time.
 *		It can be replaced by a derived class with a different behavior, by calling
 *		the function RegisterUser.  Te current user is returned by calling TheUser().
 *
 *	**	Persistent storage of entities, supported by the class CStorage. Its methods are
 *		stubbed to return an error.
 *
 *	**	CCache is an abstract class, used for curve and path linearization. It's the stream
 *		that linearization points are put into. 
 *
 *
 */
#ifndef THEUSER_H
#define THEUSER_H

#include "Point.h"
#include "Array.h"	 // for CGeoObject

/////////////////////////////////////////////////////////////////////////////
// The User, typically the user interface for the purpose of interruption	  
class GEO_CLASS CUser		:	public CGeoObject
{
public:
	CUser()
		{
		}

	virtual ~CUser()
		{
		}

	// Methods
	BOOL Canceled()
		{
		return FALSE;
		}

// No data
};

// Return the current user
PUser TheUser();

void RegisterUser(
	PUser pUser);		// In: The user to be registeed

////////////////////////////////////////////////////////////////////////////////////////
//  Rendering cache
class GEO_CLASS CCache  :  public CGeoObject
{

public:
	// Constructor and destructors
	CCache()
		{
		}

	virtual ~CCache()
		{
		}

// Methods
	virtual RC Accept(
		const C3Point & Point,   // In: The point to add
		double t);				 // In: Parametner value (ignored here)

	// dnb - 12/3/98
	// Added this new method to impose view dependent limits on infinite lines
	virtual RC Accept(
		const C3Point & Base,		// The base of infinite ray
		const C3Point & Direction); // The direction of the infinite ray

	virtual RC Accept(
		const PCurve pCurve);	// In: The curve to add

	virtual RC SkipTo(
		const C3Point & Point);   // In: The point to skip to

	virtual RC WrapUp();		

	virtual RC Reset();		

	virtual BOOL Started();

// No data
};

/////////////////////////////////////////////////////////////////////////////////////////////
//								A Generic Geometry Customer								   //
/////////////////////////////////////////////////////////////////////////////////////////////
class GEO_CLASS CStorage	:	public CGeoObject
	{
public:
	// Default constructor/destructor
	CStorage()
		{
		}

	virtual ~CStorage()
		{
		}

	// Methods
	virtual RC InitiateCurve(
		PCCurve pCurve);  // In: The path's first curve

	virtual RC InitiateNURBS(
		PCNURBS pSpline);	// In: A spline

	virtual RC AcceptLine(
		PCLine pLine);            // In: The line

	virtual RC AcceptUnboundedLine(
		PCLine pLine);            // In: The line

	virtual RC AcceptEllipArc(
		PCEllipArc	pArc);	// In: The elliptical arc

	virtual RC AcceptEllipse(
		PCEllipArc	pArc);	// In: The elliptical arc

	virtual RC AcceptNURBS(
		PCNURBS pNURBS);        // In: The NURBS

	virtual RC AcceptPolyLine(
		PCPolyLine pPolyLine);   // In: The polyline

	virtual RC AcceptGap(
		PCGap pGap);            // In: The gap

	virtual RC NewShape();

	virtual RC NewPath(
		BOOL bBoundary);	// In: Make it a boundary if TRUE

	virtual RC AcceptCurve(
		PCurve pCurve,		// In: The curve
		GeoID id=INV_ID);   // In: Reference ID

	virtual RC WrapUpPath();

	virtual RC WrapUpShape(
		GeoID idSource,	  // In: id of shape to inherit attributes from
		BOOL bSkipText,	  // In: =TRUE if text is to be ignored
		BOOL bSkipFill);  // In: =TRUE if the fill is to be ignored

	// No data
	};

#endif


	



	

