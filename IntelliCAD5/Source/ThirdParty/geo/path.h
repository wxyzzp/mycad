/*	Path.h - Defines the class of CPath
 *	Copyright (C) 1994-97 Visio Corporation. All rights reserved.
 *
 *  ABSTRACT
 *  A path is a list of curves, connected head to tail
 *
 */
#ifndef PATH_H
#define PATH_H

#include "Array.h"	// for the base class

// These follow COMPROP.flags in sheets.h
const BYTE PATH_NOFILL		= 0x01;
const BYTE PATH_NOLINE		= 0x02;
const BYTE PATH_HIDDEN		= 0x04;
const BYTE PATH_NOSNAP		= 0x08;
const BYTE PATH_CLOSED		= 0x10;		// Unused in COMPROP
const BYTE PATH_HAS_GAPS    = 0x20;		// dnb - 1/25/98 - hidden segments in path

class GEO_CLASS CPath : public CGeoObjectArray
	{
// Construction destruction
public:
	CPath();

protected:
	CPath(
		PPath p);	// In/out: Another path, gutted here

public:
	CPath(
		CPath &p,	// In: A path to copy
		RC & rc);

	virtual ~CPath()
		{
		}

// Inline Methods
	PCurve Curve(
		int nIndex) const
		{
		return (PCurve)m_pEntries[nIndex];
		}

	BOOL Closed() const
		{
		return m_bProperties & PATH_CLOSED;
		}

	void SetClosed(
		BOOL bSetTo)
		{
		SetProperty (PATH_CLOSED, bSetTo);
		}

	virtual BOOL Fillable() const
		{
		return !(m_bProperties & PATH_NOFILL);
		}

	void SetFillable(
		BOOL bSetTo)
		{
		SetProperty (PATH_NOFILL, !bSetTo);
		}

	virtual BOOL ShowBorder() const
		{
		return !(m_bProperties & PATH_NOLINE);
		}

	void SetShowBorder(
		BOOL bSetTo)
		{
		SetProperty (PATH_NOLINE, !bSetTo);
		}

	virtual BOOL Hidden() const
		{
		return m_bProperties & PATH_HIDDEN;
		}

	void SetHidden(
		BOOL bSetTo)
		{
		SetProperty (PATH_HIDDEN, !bSetTo);
		}

	virtual BOOL Snappable() const
		{
		return !(m_bProperties & PATH_NOSNAP);
		}

	void SetSnappable(
		BOOL bSetTo)
		{
		SetProperty (PATH_NOSNAP, !bSetTo);
		}

	virtual BOOL HasGaps() const
		{
		return (m_bProperties & PATH_HAS_GAPS);
		}

	double GetStartTrim()
		{
		return m_rStart;
		}

	void SetStartTrim(
		double value)
		{
		m_rStart = value;
		}

	double GetEndTrim()
		{
		return m_rEnd;
		}

	void SetEndTrim(
		double value)
		{
		m_rEnd = value;
		}

// Methods

	RC Add(PCurve pSegment, BOOL bIsVisible = TRUE);  // Add a curve - derived from objectarray
	RC Linearize(
		double rResolution,		// In: Chord-height toelrance
		PAffine pTransform,		// In: Rendering transformation (NULL OK)
		PTransform pMapping,	// In: Non-affine mapping to apply first (NULL OK)
		PCache  pCache);		// In/out: The rendering cache

	RC UpdateExtents(
		BOOL bTight,				// In: Tight extents if =TRUE
		CExtents & cExtents,		// IN/Out: The extents, updated here
		PAffine pTransform=NULL)	// Optional: Transformation (NULL OK)
		const;

	RC Wireframe(
		CExtents & cExtents,		// IN/Out: The wireframe extents, updated here
		PAffine pTransform=NULL)	// Optional: Transformation (NULL OK)
		const;

	RC WrapUp();	// Return E_NOMSG if the path is empty

	RC Reverse();

	RC Length(
		double & rLength);  // Out: The total length

	void NearestPoint(
		C3Point ptTo,			 // In: The point to get near
		P3Point pNearest=NULL,	 // Out: Nearest point - optional
		double*	pDistance=NULL,	 // Out: The distance - optional
		PCurve* pOn=NULL,		 // Out: Curve the point is on - Optional
		double* pAt=NULL,		 // Out: Parameter there - optional
		int* pIxCurve=NULL) const; // Out: The index of the curve - optional

	PCurve Curve(
		int i);	  //In: The index of the desired curve

	RC Move(
		const C3Point & ptMove);

	RC Transform(
		PTransform pTransform);    // In: The desired transformation, NULL OK

	RC UpdateIntercepts(
		PRay pRay,					// In: A ray 
		int	& nIntercepts) const;   // In/out: The number of intercepts

	virtual RC UpdateIntercepts(
		PUnboundedLine pLine, // In: A line 
		BOOL bIgnoreOrigin,	  // In: Ignore the origin if TRUE
		double * pNearFore,   // In/out: Forward intercept nearest to line's origin
		double * pFarFore,    // In/out: Forward intercept farthest from line's origin
		double * pNearBack,	  // In/out: Backward intercept nearest to line's origin
		double * pFarBack);   // In/out: Backward intercept farthest from line's origin

	RC EndPoints(
		C3Point & ptStart,  // Out: Start point
		C3Point & ptEnd);	// Out: End point

	RC EndPointGeometry(
		int iWhichEnd,        // In: 0 for start point, 1 for end
		C3Point & ptPoint,    // Out: End point (NULL OK)
		C3Point & ptTangent,  // Out: Derivative (NULL OK)
		double & rCurvature); // Out: Curvature there (NULL OK)

	RC SplitAtCusps();

	BOOL HitTest(
		const C3Point& ptTo, // In: The point to get near
		double	rMargin);	 // Out: The distance considered a hit

	RC SnapEnds(
		const C3Point & rptStart, // In: Snapped start point
		const C3Point & rptEnd);  // In: Snapped end point

	int Complexity() const;

    CPath &     operator=(const CPath &path);

#ifdef DEBUG
	void Dump() const;
#endif

// Private methods
protected:
	void SetProperty(
		BYTE bits,	// The property bit mask
		BOOL on);	// In: The value to set these bits to to

// Data
protected:
	BYTE m_bProperties; // Properties bit mask
	double m_rStart;	// Where rendering starts on the first curve
	double m_rEnd;		// Where rendering ends on the last curve
    };

/////////////////////////////////////////////////////////////////////
// Function Prototypes

GEO_API RC PathDistance(
	PPath pFirst,   // In: First path
	PPath pSecond,	// In: Second path
	double& rDist,	// out: The distance
	double rMargin);	// Optional: distance margin

#endif
