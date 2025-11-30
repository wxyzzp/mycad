/*	2CurveSolver.h - Defines C2CurveSolver and its derived classes
 *	Copyright (C) 1998 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 * This file implemetats a general numerical solver for equations that
 * 2 curves, such as the distane between 2 curves.  Unlike CCurvePair,
 * this class is not meant to be a base class for pairs of special types
 * of curves such as CLineLine.  It is more similar in design to
 * CRealFunction: any specific problem, e.g. the distance between 2
 * curves, should be implemented as a subclass of C2CurveSolver, so that it
 * can be solved with the Solve method.  This method is best when there is
 * an exact solution for pairs of lines/arcs, because it should converge in
 * a single step on lines and arcs.
 * The derived class may or may not implement the correction for each
 * one of the pairs line-line, line-arc, or arc-arc.  If an overriding
 * method is not implemented, the base class stub will return an error,
 * and then the solver will report that there is no solution.  This may
 * be the case, for example, when looking for a common tangent to 2 lines.
 * The derived class may override the "IsSatisfied" query that tells the
 * solver to return the current solution.  The default stub returns FALSE,
 * which means that the convergence test is based strictly on the size of
 * the corrections.
 *
 */
#ifndef C2CURVESOLVER_H
#define C2CURVESOLVER_H

#include "geometry.h"

/////////////////////////////////////////////////////////////////////
// The base class
class GEO_CLASS C2CurveSolver
	{
	public:
	// Construction destruction

	void Initialize();

	C2CurveSolver();

	C2CurveSolver(
		PCurve pFirst,			 // In: First curve
		PCurve pSecond);		 // In: Second curve

	virtual ~C2CurveSolver()
		{
		}

// Inline methods
	double Parameter(
		int i)	// In: The index of the curve
		{
		return m_s[i];
		}

	C3Point Point(
		int i)	// In: The index of the curve
		{
		return m_P[i];
		}

	RC Fail()
		{
		m_ds[0] = m_ds[1] = 0;
		return E_NOMSG;
		}

// Semi-pure methods
	virtual RC CorrectPointLine(
		int i,	// In: The index of the curve that degenerates to a point
		int j);	// In: The index of the other curve

	virtual RC CorrectPointArc(
		int i,	// In: The index of the curve that degenerates to a point
		int j);	// In: The index of the other curve

	virtual RC CorrectLineLine();

	virtual RC CorrectLineArc(
		int i,	// In: The index of the curve that has 0 curvature
		int j);	// In: The index of the other curve

	virtual RC CorrectArcArc();

	virtual BOOL IsSatisfied()
		{
		return FALSE;
		}

// Methods
	RC Solve( // Return E_NOMSG if no solution found
		double rSeed1,	// In: Initial guess on first curve
		double rSeed2);	// In: Intial guess on second curve

	BOOL Singular();

	// Data
protected:
	PCurve m_pCurve[2];		// The 2 curves
	double m_s[2];			// Current parameter values
	double m_ds[2];			// Current correction
	C3Point m_P[2];			// Current points on the curves
	C3Point m_T[2];			// Current unit tangents there
	double m_speed[2];		// Current speeds there
	C3Point m_C[2];			// Current centers of curvature there
	double m_rad[2];		// Current radii of curvature there
	BOOL m_bSingular;		// =TRUE if singularity encountered
	};

typedef C2CurveSolver * P2CurveSolver;

/////////////////////////////////////////////////////////////////////
// The distance between curves
class GEO_CLASS CCurvesDistance		:		public C2CurveSolver
	{
	public:
	// Construction destruction
	CCurvesDistance();

	CCurvesDistance(
		PCurve pFirst,      // In: First curve
		PCurve pSecond,     // In: Second curve
		double rMargin);	// In: Distance that is considered 0

	virtual ~CCurvesDistance()
		{
		}

	// Methods
	virtual RC CorrectPointLine(
		int i,	// In: The index of the curve that degenerates to a point
		int j);	// In: The index of the other curve

	virtual RC CorrectPointArc(
		int i,	// In: The index of the curve that degenerates to a point
		int j);	// In: The index of the other curve

	RC CorrectLineLine();

	RC CorrectLineArc(
		int i,	// In: The index of the curve that has 0 curvature
		int j);	// In: The index of the other curve

	virtual RC CorrectArcArc();

	virtual BOOL IsSatisfied();

	double SqDist()
		{
		return m_rSqDist;
		}

	// Data
protected:
	double m_rMargin;	// Distance that is considered 0
	double m_rSqDist;	// Current squared distance
};

/////////////////////////////////////////////////////////////////////
// Find XY distance of curves.
//
// Author:  Stephen W. Hou
// Date:    7/23/2004
//
class GEO_CLASS CCurvesXYDistance	:	public CCurvesDistance
	{
	public:
	// Construction destruction
	CCurvesXYDistance();

	CCurvesXYDistance(
		PCurve pFirst,      // In: First curve
		PCurve pSecond,     // In: Second curve
		double rMargin);	// In: Distance on XY plane which is considered 0

	virtual BOOL IsSatisfied();
};

/////////////////////////////////////////////////////////////////////
// The common tangent of 2 curves
class GEO_CLASS CCommonTangent		:		public C2CurveSolver
	{
	public:
	CCommonTangent(
		PCurve pFirst,  // In: First curve
		PCurve pSecond) // In: Second curve
		:C2CurveSolver(pFirst, pSecond)
		{
		}

	virtual ~CCommonTangent()
		{
		}

	// Overriding methods
	RC CorrectArcArc();

	BOOL IsSatisfied();

	// No data
	};
/************************************************************************/
// Function prototypes
GEO_API RC Distance(
	PCurve pFirst,      // In: First curve
	PCurve pSecond,		// In: Second curve
	double& rDist,		// In/Out: The distance, updated here
	double& r1,			// Out: Parameter value on first curve
	double& r2,			// Out: Parameter value on second curve
	C3Point& P1,		// Out: Point on first curve
	C3Point& P2,		// Out: Point on second curve
	double rMargin);	// Optional: Distance margin

GEO_API RC CommonTangent(
	PCurve pFirst,	// In: First curve
	PCurve pSecond,	// In: Second curve
	double s1,		// In: Inital gess of Parameter first curve
	double s2,		// In: Inital gess of Parameter second curve
	double& t1,		// Out: Parameter of tangent point on first curve
	double& t2);	// Out: Parameter of tangent point on second curve

/////////////////////////////////////////////////////////////////////
// One curve solver  - the base class
class GEO_CLASS C1CurveSolver
	{
	public:
	// Construction destruction

	void Initialize();

	C1CurveSolver()
		{
		Initialize();
		}

	C1CurveSolver(
		PCCurve pCurve);		 // In: The curve

	virtual ~C1CurveSolver()
		{
		}

// Inline methods
	double Parameter()
		{
		return m_s;
		}

	C3Point Point()
		{
		return m_P;
		}

	RC Fail()
		{
		m_ds = 0;
		return E_NOMSG;
		}

// Semi-pure methods
	virtual RC CorrectLine();

	virtual RC CorrectArc();

	virtual BOOL IsSatisfied()
		{
		return FALSE;
		}

// Methods
	RC Solve( // Return E_NOMSG if no solution found
		double rSeed);			// In: Initial guess

	// Data
protected:
	PCCurve m_pCurve;	// The curve
	double m_s;			// Current parameter value
	double m_ds;		// Current correction
	C3Point m_P;		// Current point on the curve
	C3Point m_T;		// Current unit tangent there
	double m_speed;		// Current speed there
	C3Point m_C;		// Current center of curvature there
	double m_rad;		// Current radius of curvature there
	BOOL m_bSingular;	// =TRUE if singularity encountered
	};

typedef C1CurveSolver * P1CurveSolver;

/////////////////////////////////////////////////////////////////////
// The distance between a point and a curve
class GEO_CLASS CPullBack		:		public C1CurveSolver
	{
	public:
	// Construction destruction
	CPullBack();

	CPullBack(
		PCCurve pCurve,		// In: A curve
		C3Point ptTo,		// In: A point
		double rMargin=0);	// In: Distance considered 0

	virtual ~CPullBack()
		{
		}

	// Methods
	virtual RC CorrectLine();

	virtual RC CorrectArc();

	virtual BOOL IsSatisfied();

	double SqDistance()
		{
		return m_rSqDist;
		}

	// Data
protected:
	C3Point m_ptTo;		// The point we're trying to get near to
	double m_rMargin;	// Distance that is considered 0
	double m_rSqDist;	// Current squared distance
};


//////////////////////////////////////////////////////////////////////////
//																		
// Find the nearest point on the curvewithin tolerance in X-Y range.
//	
// Author:  Stephen W. Hou
// Date:    7/13/2004			                                                       
//
class GEO_CLASS CXYPullBack	:	public CPullBack
	{
	public:
	// Construction destruction
        CXYPullBack() {}

	CXYPullBack(
		PCCurve pCurve,		// In: A curve
		C3Point ptTo,		// In: A point
		double rMargin=0);	// In: Distance considered 0

	// Methods
	virtual RC CorrectLine();

	virtual BOOL IsSatisfied();
    };


#endif
