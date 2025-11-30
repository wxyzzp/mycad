/* Region.h - Defines the class of CRegion
 * Copyright (C) 1998 Visio Corporation. All rights reserved.
 *
 * ABSTRACT
 *
 * CRegion is a list of paths.  Some instances of CBoundary - they are
 * boundaries of region that have area, and may contain points.  Others,
 * even though they may be closed, are just strokes, which are boundaries
 * of nothing.
 *
 */
#ifndef REGION_H
#define REGION_H

#include "Array.h"	// For the base class

// Some caller relies on this order, do not change it!
#define HIT_FAILED		   -1
#define HIT_OUTSIDE			0
#define	HIT_ON_BOUNDARY		1 
#define HIT_INSIDE			2

// Some caller relies on this order, do not change it!
#define SHAPES_DISJOINT		0x0000
#define SHAPES_TOUCH		0x0001
#define SHAPES_OVERLAP		0x0002
#define SHAPES_1_INSIDE_2	0x0010
#define SHAPES_2_INSIDE_1	0x0020

// Masks
#define SHAPES_IN_COMMON	0x0032	// Overlap or containment
#define SHAPES_CONTAINMENT	0x0030
#define SHAPES_RELATION		0x0033	// Overlap, contain or touch

#define SHAPES_EMPTY		0X0200
#define SHAPES_INTERIOR		0X0100

// Definition
class GEO_CLASS CRegion : public CGeoObjectArray
	{
// Construction destruction
public:
	CRegion()
		{
		}

	CRegion(
		CRegion & other,	// In: Other region to copy
		RC & rc);			// Out: Return code

	virtual ~CRegion()
		{
		}

// Inline Methods
	PPath Path(
		int nIndex) const
		{
		return (PPath)m_pEntries[nIndex];
		}

// Methods
	RC Length(
		double& rLength);  // Out: the total length

	RC Move(
		const C3Point& vecMove);	// In: The vector to move by

	RC Transform(
		PTransform pTransform);    // In: The desired transformation, NULL OK

	RC UpdateExtents(
		BOOL bTight,			  // In: Tight extents if =TRUE
		CExtents & cExtents,	  // IN/Out: The extents, updated here
		PAffine pTransform) const;// Optional: Transformatiom (NULL OK)

	RC HitTest(
		const C3Point& P,	// In: Point, in this shape's local coordinates
		double rTolerance,	// In: Tolerance
		int& iResult);		// In: result = HIT_FAILED, HIT_OUTSIDE, 
						//				HIT_ON_BOUNDARY or HIT_INSIDE

	BOOL QuickHitTest(
		const C3Point& P,	// In: Point, in this region's coordinates
		double rTolerance,	// In: Tolerance
		BOOL bBorderOnly);	// In: Ignore interior hits if TRUE

	RC TestIfContained(
		const C3Point& P,	// In: Point, in this region's coordinates
		int nAttempts,		// In: Number of attempts
		BOOL& bContained);	// Out: =TRUE if contained

	RC NearestPoint(			// Return TRUE if snapped
		BOOL bIgnore,			// In: Ignore nonsnappable paths if TRUE
		C3Point ptTo,			// In: The point
		C3Point & ptNearest,	// Out: nearest point 
		double*	pDistance=NULL,	// Out: The distance - optional
		PCurve * pOn=NULL,		// Out: curve it's on - optional
		double * pAt=NULL,		// Out: Parameter on the curve  - optional
		int * ppIxPath=NULL,	// Out: The index of the path - optional
		int * pIxCurve=NULL)const; // Out: The index of the curve - optional

	int Complexity() const;

#ifdef DEBUG
	void Dump();
#endif

// No data
    };

typedef CRegion* PRegion;

/////////////////////////////////////////////////////////////////////////
// Some 2 Region functions
GEO_API RC TestProximity(
	PRegion pFirst,		// In: First region
	PRegion pSecond,	// In: Second region
	double rTolerance,	// In: Distance considered 0
	BOOL & bNear,		// Out: =TRUE if the shapes are within tolerance
	short * pDetails);	// Out: (Optional) one of the valaues listed below

GEO_API RC RegionsDistance(
	PRegion pFirst,		 // In: First region
	PRegion pSecond,	 // In: Second region
	double & rDistance); // Out: The distance

#endif


