/*	CrvPair.h - Defines the classes of pairs of curves
 *	Copyright (C) 1995-1998 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 *
 * A curve-pair serves as a switch for all 2-curve methods, such as intesection.
 * Its subclasses (e.g. line-line) hold the keys to special case methods.  Thus,
 * before 2 curves intesect, for example, they identify themself and construct
 * the appropriate subclass, which then calls the appropriate special case
 * intersection, which is implemented as its version of the virtual intersection
 * method.
 *
 * CCurvePair and its subclasses are private classes, used internally for the
 * implementation of 2-curve methods.  The compiler will not let you use
 * them, even the destructors are protected.  If you find yourself trying
 * to use these classes directly then you are probably doing something
 * wrong.  Instaed, you should call one of their friend functions that does
 * what you are trying to do, only it does it right.
 *
 */
#ifndef CRVPAIR_H
#define CRVPAIR_H

#include "Curve.h"		// For members of CCurvePair

#define SINGULARITY 1
#define CONVERGED 2

// Forward declaration
class CCurvePair;
typedef CCurvePair* PCurvePair;

/////////////////////////////////////////////////////////////////////
// The class of curve-pairs
class GEO_CLASS CCurvePair
{
// CCurvePair is a private class, to be used only througn its friends
	GEO_FRIEND RC GetCurvePair(
		PCurve pFirst,     // In: First curve
		PCurve pSecond,    // In: Second curve
		BOOL bCommute,     // In: =TRUE if this is done for a cummutative operation
		PCurvePair & pPair);// Out: The pair

	GEO_FRIEND RC RoundCorner(
		PCurve pFirst,      // In: First curve
		double rLimit1,     // In: How much of the first curve may be trimmed away
		PCurve pSecond,		// In: Second curve
		double rLimit2,     // In: How much of the second curve may be trimmed away
		double rRadius,     // In: Radius of rounding arc
		PEllipArc & pArc);   // Out: The arc that rounds the corner

	GEO_FRIEND RC Intersect(
		PCurve pFirst,		// In: First curve
		PCurve pSecond,		// In: Second curve
		double rThisSeed,	// In: Initial guess on this curve
		double rOtherSeed,	// In: Intial guess on the other curve
		double & rOnThis,	// Out: Parameter value on this curve
		double & rOnOther,	// Out: Paramter value on the other curve
		double rSqTol=0);	// Optional: tolerance squared

	GEO_FRIEND RC Intersect(
		PCurve pFirst,				  // In: First curve
		PCurve pSecond,				  // In: Second curve
		CRealArray & cOnThis,		  // In/Out: Intersection parameters on this curve
		CRealArray & cOnOther,		  // In/Out: Intersection parameters on other curve
		double rTolerance = FUZZ_GEN);// Optional: Tolerance

	GEO_FRIEND RC ProjIntersect(
		PCurve pFirst,           // In: First curve
		PCurve pSecond,		     // In: Second curve
		const C3Vector &projDir, // In: Projected direction
		CRealArray & cOnThis,    // In/Out: Intersection parameters on this curve
		CRealArray & cOnOther,   // In/Out: Intersection parameters on other curve
		double rTolerance = FUZZ_GEN);	// Optional: Point distance tolerance

	GEO_FRIEND RC AreDuplicates(
		PCurve pThis,           // In: A curve
		PCurve pOther,          // In: A potential duplicate
		double rTolerance,		// In: Tolerance
		BOOL & bDuplicate);     // Out: =TRUE if Other duplicates this path

protected:
// Construction destruction
	CCurvePair();

	CCurvePair(
		PCurve pFirst,      // In: First curve
		PCurve pSecond,     // In: Second curve
		BOOL bSwap);        // In: =TRUE if the curves are to be swapped

	virtual ~CCurvePair();

// Methods
protected:
	BOOL Swapped()
		{
		return m_bSwap;
		}

	virtual RC RoundCorner(
		double rLimit1,      // In: How much of the first curve may be trimmed away
		double rLimit2,      // In: How much of the second curve may be trimmed away
		double & rRadius,    // In/Out: Radius of rounding arc
		double &rContact1,   // Out: Parameter value of contact point on curve 1
		double & rContact2); // Out: Parameter value of contact point on curve 2

	virtual RC Intersect(
		CRealArray & cOnThis,    // In/Out: Intersection parameters on this curve
		CRealArray & cOnOther,   // In/Out: Intersection parameters on other curve
		double rTolerance = FUZZ_GEN);	// Optional: Point distance tolerance

	virtual RC ProjIntersect(
		const C3Vector &projDir, // In: Projected direction
		CRealArray & cOnThis,    // In/Out: Intersection parameters on this curve
		CRealArray & cOnOther,   // In/Out: Intersection parameters on other curve
		double rTolerance = FUZZ_GEN);	// Optional: Point distance tolerance

	RC IntersectSeed(
		double rThisSeed,		 // In: Initial guess on this curve
		double rOtherSeed,		 // In: Intial guess on the other curve
		double & rOnThis,		 // Out: Parameter value on this curve
		double & rOnOther,		 // Out: Paramter value on the other curve
		double rFuzz=FUZZ_GEN);	 // Optional: tolerance

	RC AreDuplicates(
		double rTolerance,		// In: Tolerance
		BOOL & bDuplicate);  // Out: =TRUE if Other duplicates this path

	RC NearestPoints( // Return E_NOMSG if no minimum found
		double rSeed1,		// In: Initial guess on first curve
		double rSeed2,		// In: Intial guess on second curve
		double & rSqDist,	// Out: The square of the distance
		double & s,			// Out: Parameter value on first curve
		double & t,			// Out: Paramter value on second curve
		C3Point& P,			// Out: Point on first curve
		C3Point& Q,			// Out: Point on second curve
		double rMargin=0);	// Optional: Squared distance margin

	RC InteriorDistance(
		double& rSqDist,	// In/Out: The squared distance, updated here
		double& r1,			// Out: Parameter value on first curve
		double& r2,			// Out: Parameter value on second curve
		C3Point& P1,		// Out: Point on first curve
		C3Point& P2,		// Out: Point on second curve
		double rMargin=0);	// Optional: Squared distance margin

	RC EvaluateDistance(
		double s,	// In: Parameter value on first curve
		double t,	// In: Paramter value on second curve
		C3Point& P,	// Out: Point on first curve
		C3Point& Q,	// Out: Point on second curve
		double& f,	// Out: the value of f = the squared distance
		double* G,	// Out: 1/2(fs, ft) (=1/2 the gradient of f)
		double* H);	// Out: 1/2(fss, fst, ftt) (=the Hessian of f)

//Data
protected:
	PCurve m_pFirst;    // First curve
	PCurve m_pSecond;   // Second curve
	BOOL m_bSwap;       // = TRUE if returned results are to be swapped
};

/////////////////////////////////////////////////////////////////////
//
class GEO_CLASS CLineLine	 : public CCurvePair
{
// CCurvePair is a private class, to be used only througn its friends
GEO_FRIEND RC GetCurvePair(
	PCurve pFirst,     // In: First curve
	PCurve pSecond,    // In: Second curve
	BOOL bCommute,     // In: =TRUE if this is done for a cummutative operation
	PCurvePair & pPair);// Out: The pair

// Construction destruction
protected:
	CLineLine();
	virtual ~CLineLine();

	CLineLine(
		PCurve pFirst,    // In: First curve
		PCurve pSecond,   // In: Second curve
		BOOL bSwap);      // In: =TRUE if the returned results are to be swapped

// Inline methods
	PLine Line1()
		{
		return (PLine)m_pFirst;
		}

	PLine Line2()
		{
		return (PLine)m_pSecond;
		}

// Methods
	virtual RC RoundCorner(
		double rLimit1,      // In: How much of the first curve may be trimmed away
		double rLimit2,      // In: How much of the second curve may be trimmed away
		double & rRadius,    // In/Out: Radius of rounding arc
		double &rContact1,   // Out: Parameter value of contact point on line 1
		double & rContact2); // Out: Parameter value of contact point on line 2

	RC Intersect(
		CRealArray & cList1,  // In/Out: Intersection parameters on first curve
		CRealArray & cList2,  // In/Out: Intersection parameters on second curve
		double rTolerance = FUZZ_GEN);	// Optional: Point distance tolerance

	RC AreDuplicates(
		double rTolerance,		// In: Tolerance
		BOOL & bDuplicate);  // Out: =TRUE if Other duplicates this path

// No data
};

/////////////////////////////////////////////////////////////////////
//
class GEO_CLASS CLineEllipArc	 : public CCurvePair
{
// CCurvePair is a private class, to be used only througn its friends
GEO_FRIEND RC GetCurvePair(
	PCurve pFirst,     // In: First curve
	PCurve pSecond,    // In: Second curve
	BOOL bCommute,     // In: =TRUE if this is done for a cummutative operation
	PCurvePair & pPair);// Out: The pair

// Construction destruction
protected:
	CLineEllipArc();
	virtual ~CLineEllipArc();

	CLineEllipArc(
		PCurve pFirst,    // In: First curve
		PCurve pSecond,   // In: Second curve
		BOOL bSwap);      // In: =TRUE if the returned results are to be swapped

// Inline methods
	PLine Line() const
		{
		return (PLine)m_pFirst;
		}

	PEllipArc Arc() const
		{
		return (PEllipArc)m_pSecond;
		}

// Methods
	RC Intersect(
		CRealArray & cList1,  // In/Out: Intersection parameters on first curve
		CRealArray & cList2,  // In/Out: Intersection parameters on second curve
		double rTolerance = FUZZ_GEN);	// Optional: Point distance tolerance

	RC AreDuplicates(
		double rTolerance,		// In: Tolerance
		BOOL & bDuplicate);  // Out: =TRUE if Other duplicates this path

// No data
};
/////////////////////////////////////////////////////////////////////
//
class GEO_CLASS CEllipArcEllipArc	 : public CCurvePair
{
// CCurvePair is a private class, to be used only througn its friends
GEO_FRIEND RC GetCurvePair(
	PCurve pFirst,     // In: First curve
	PCurve pSecond,    // In: Second curve
	BOOL bCommute,     // In: =TRUE if this is done for a cummutative operation
	PCurvePair & pPair);// Out: The pair

// Construction destruction
protected:
	CEllipArcEllipArc();
	virtual ~CEllipArcEllipArc();

	CEllipArcEllipArc(
		PCurve pFirst,    // In: First curve
		PCurve pSecond,   // In: Second curve
		BOOL bSwap);      // In: =TRUE if the returned results are to be swapped

// Inline methods
	PEllipArc First() const
		{
		return (PEllipArc)m_pFirst;
		}

	PEllipArc Second() const
		{
		return (PEllipArc)m_pSecond;
		}

// Methods

	RC Intersect(
		CRealArray & cList1,		 // In/Out: Intersection parameters on first curve
		CRealArray & cList2,		 // In/Out: Intersection parameters on second curve
		double rTolerance = FUZZ_GEN); // Optional: Point distance tolerance

// No data
};
//////////////////////////////////////////////////////////////////////////////
// Function prototypes
// Interface: find all the intersections between two curves
GEO_API RC Intersect(
	PCurve pFirst,          // In: First curve
	PCurve pSecond,		    // In: Second curve
	CRealArray & cOnThis,   // In/Out: Intersection parameters on this curve
	CRealArray & cOnOther,  // In/Out: Intersection parameters on other curve
	double rTolerance);		// Optional: Tolerance

GEO_API RC ProjIntersect(
	PCurve pFirst,           // In: First curve
	PCurve pSecond,		     // In: Second curve
	const C3Vector &projDir, // In: Projected direction
	CRealArray & cOnThis,    // In/Out: Intersection parameters on this curve
	CRealArray & cOnOther,   // In/Out: Intersection parameters on other curve
	double rTolerance);		 // Optional: Point distance tolerance

// Relative direction of 2 curves
GEO_API RC RelativeDirection(
	CCurve & This,		// In: One curve
	CCurve & Other,		// In: The other curve
	double rOnThis,     // In: Parameter value on this curve
	double rOnOther,    // In: Parameter value on the other curve
	int & nSide);		// Out: 1 for left, -1 for right

#endif


