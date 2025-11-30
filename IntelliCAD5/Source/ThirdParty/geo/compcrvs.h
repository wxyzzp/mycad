/*	CompCrvs.h - Defines the classes of composed curves
 *	Copyright (C) 1994-98 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 *
 *
 */
#ifndef COMPCRVS_H
#define COMPCRVS_H

#include "TrnsForm.h"
#include "Knots.h"
#include "Roots.h"			// for the class CRealFunction
#include "Path.h"			// for CPath
#include "Curve.h"			// for the base class

//////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
// The class of transformed curves
/*
 * The image of a curved undergoes an affine transformation can be computed as a new
 * curve of the same type, by properly applying the transformation to the curve's
 * defining geometry (e.g. a line's point and direction).  This is no longer possible
 * when the transfromation is not affine.  The transformed curve can only be defined
 * procedurally in that case: to evaluate on it, evaluate on the base curve, and apply
 * the transformation to the result.  The transformed curve owns neither the base curve
 * nor the transformation.  The programmer's must make sure that they are not deleted from
 * underneath it.
 */
class GEO_CLASS CTransformedCurve	:	public CCurve
	{
public:
// Construction& destruction
	CTransformedCurve();

	virtual ~CTransformedCurve()
		{
		}

	CTransformedCurve(
		PCCurve		pCurve,			// In: The curve to be transformed
		PTransform	pTransform,		// In: The transformation
		RC& rc);					// Out: Return code

	// Methods
	PCurve Clone() const;

	RC Evaluate(
		double rAt,					// In: Where to evaluate
		C3Point& rptPoint) const;	// Out: Position there

	double Start() const
		{
		return m_pCurve->Start();
		}

	double End() const
		{
		return m_pCurve->End();
		}

	double Period() const
		{
		return m_pCurve->Period();
		}

	BOOL Planar() const
		{
		return m_pCurve->Planar();
		}

    PCCurve    GetBaseCurve() const { return m_pCurve; }
	virtual CURVETYPE Type() const { return TYPE_Transformed; }

#ifdef DEBUG
	virtual void Dump() const;
#endif

// Data
protected:
	PCCurve		m_pCurve;        // The base curve
	PTransform m_pTransform;	 // The transoation
	};

typedef CTransformedCurve * PTransformedCurve;

////////////////////////////////////////////////////////////////////////////////////////
// The class of mapped curves
/*
 * CMapped curve is a composition C(f(t)) of a curve C with a parameter mapping f. The
 * mapped curve does not own the base curve. The programmer's must make sure that it
 * is not deleted from  underneath it.  It does own the mapping.
 */
class GEO_CLASS CMappedCurve : public CCurve
{
// Construction destruction
public:
	CMappedCurve();

	CMappedCurve(
		const CMappedCurve & cOther);	 // In: The curve to copy

	CMappedCurve(
		PCurve pCurve,			   // In: The base curve to be offset
		PRealFunction pMapping);   // In: The offset amount (to the left)

	virtual ~CMappedCurve();
	virtual PCurve Clone() const;

// Methods
public:
	double Start() const
		{
		return m_pMapping->Start();
		}

	double End() const
		{
		return m_pMapping->End();
		}

	// General composite curve ignores periodicity
	virtual double Period() const
		{
		return 0;
		}

	BOOL Planar() const
		{
		return m_pCurve->Planar();
		}

	RC Evaluate(
		double rAt,                 // In: parameter value to evaluate at
		C3Point & rptPoint) const;  // Out: Point on the curve

	RC Evaluate(
		double rAt,						// In: parameter value to evaluate at
		C3Point & rptPoint,				// Out: Point at that parameter
		C3Point & rptTangent) const;	// Out: Tangent vector at that point

    const PCurve    GetBaseCurve() const { return m_pCurve; }

	virtual CURVETYPE Type() const { return TYPE_Mapped; }

#ifdef DEBUG
	void Dump() const;
#endif

protected:
// Data
	PCurve			m_pCurve;    // The base curve
	PRealFunction   m_pMapping;  // The reparameterization mapping
};

//////////////////////////////////////////////////////////////////////////////////
//						Definition of CCompositeCurve							//
//////////////////////////////////////////////////////////////////////////////////
/*
 * A composite curve is a path, with a parameterization that makes it a curve.
 * A knot sequence assigns a knot span for every curve in the path.  To
 * evaluate at a parameter t, use CKnots methods to get to the right knot span,
 * convert t to the curve's original domain, and evaluate on the curve.
 */
class GEO_CLASS CCompositeCurve	:	public CCurve
	{
public:
	CCompositeCurve();

	CCompositeCurve(
		PPath pPath,	// In: The path
		int  iFrom,		// In: The index of the first curve
		int  iTo,		// In: The index of the last curve
		RC & rc);		// Out: Return code

	CCompositeCurve(
		const CCompositeCurve & cOther);

	virtual ~CCompositeCurve();

	PCurve Clone() const;

	virtual RC Transform(
		CTransform & cTransform);       // In: The transformation

	virtual RC TransformCopy(
		CTransform & cTransform,        // In: The transformation
		PCurve & pOut) const;           // Out: the transformed curve

	virtual RC Reverse();

	// Inline Methods
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
		return m_rEnd - m_rStart;
		}

	BOOL Planar() const
		{
		return FALSE;
		}

	// Methods
	RC Add(
		PCurve pCurve,	// In: The curve to add
		double rFrom,	// In: Domain start
		double rTo);	// In: Domain end

	RC Evaluate(
		double rAt,					// In: Parameter value
		C3Point & ptPoint) const;	// Out: A point on the base path

	RC Evaluate(
		double rAt,					// In: Parameter value
		C3Point & ptPoint,			// Out: A point on the base path
		C3Point & ptTangent) const;	// Out: Tangent vector there

    // Find a semgent index via a parametric point on the curve. If the point is 
    // located on the vertex of the curve, it will return the lowest index. The 
    // function will return -1 if the point is not on the curve.
    //
    int FindSegment(double rAt) const { return (FindSpan(rAt) + 1); }

    virtual BOOL HitTest(           // Return TRUE if the ray hits the cuvre
        const C3Point &pt,          // In: Through point
        const C3Vector &dir,        // In: The hit direction
        double rTolerance,          // In: The tolerance considered a hit
        double &rAt);               // Out: Hit location on the curve within tolearnce

    int             NumOfSegments() const { return m_cCurves.Size(); }

	virtual CURVETYPE Type() const { return TYPE_Composite; }

    const CCurve*   Segment(int index) const;

    RC              GetSegmentDomain(
                        int index,              // In:  The index of curve segment(start from 0)
                        double &lowBound,       // Out: The low bound of parameter ranger
                        double &upBound) const; // Out: The up bound of parameter ranger

#ifdef DEBUG
	void Dump() const;
#endif

private:

    // Find the point is after which knot.
    //
    int FindSpan(double rAt) const;

protected:
	//Data
	CPath m_cCurves;
	CRealArray m_cKnots;
	double m_rStart;
	double m_rEnd;
	};
typedef CCompositeCurve * PCompositeCurve;

inline const CCurve* CCompositeCurve::Segment(int i) const
{ ASSERT(i >= 0 && i < m_cCurves.Size()); return m_cCurves.Curve(i); }

#endif


