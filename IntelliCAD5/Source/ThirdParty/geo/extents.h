/*	Extents.h - Defines the classe 2D extents.
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 *
 * CExtents is a bounding rectangle in the XY plane
 *
 */
#ifndef EXTENTS_H
#define EXTENTS_H

class GEO_CLASS CExtents
{
public:
// Constructor/destructor
	CExtents();

	CExtents(
		double rMinX,        // IN: Lowest x value in the path
		double rMaxX,        // IN: Highest x value in the path
		double rMinY,        // IN: Lowest y value in the path
		double rMaxY);       // IN: Highest y value in the path

	virtual ~CExtents();

// Methods
	void Reset();

	void Update(
		CExtents & cOther);

	void Update(
		const C3Point & P);

	void Update(
		const C3Point & P,		// In: A point
		PTransform pTransform);	// In: Transformation

	void UpdateX(
		const C3Point & P);

	void UpdateY(
		const C3Point & P);

	BOOL Overlap(
		const CExtents & cOther) const;

	double & MinX()
		{
		return m_rMinX;
		}

	double & MaxX()
		{
		return m_rMaxX;
		}

	double & MinY()
		{
		return m_rMinY;
		}

	double & MaxY()
		{
		return m_rMaxY;
		}

	void GetMinMax(
		const C3Point& V,	// In: A vector
		double& rMin,		// Out: The minimimum of P*U within the extents
		double& rMax);		// Out: The maximimum of P*U within the extents

	BOOL Contain(
		const C3Point & P) const;

	BOOL Contain(						/*D.Gavrilov*/// added for hatches
		const CExtents & cOther) const;

	void Inflate(
		double r);	// In: Value to inflate by
// Data
protected:
	double m_rMinX;
	double m_rMaxX;
	double m_rMinY;
	double m_rMaxY;
};

typedef enum PointBoxPosition{
	OutsideBox=0,
	InsideBox,
	OnBoxEdge,
	OnBoxVertex};

GEO_API PointBoxPosition IsPointInBox(	// Point's position relative to the box
	const C3Point & P,	// In: The point to be tested
	P3Point Q,			// In: Box's 3 consecutive corners
	double tolerance);	// In: Tolerance for being on edge or vertex

#endif
