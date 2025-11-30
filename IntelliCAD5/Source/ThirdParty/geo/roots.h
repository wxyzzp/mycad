/*	Roots.h - Definition of the class CRealFunction, its subclass CLinearFunction,
 *  and utilities for  finding roots of miscellaneous nonlinear equations.
 * Copyright (C) 1994-1997 Visio Corporation. All rights reserved.
 *
 *
 */
#ifndef ROOTS_H
#define ROOTS_H

#include "Array.h"

////////////////////////////////////////////////////////////////////////////
///                        General real function                         ///
////////////////////////////////////////////////////////////////////////////
class GEO_CLASS CRealFunction	   :     public CGeoObject
{
// Construction destruction
public:
	CRealFunction(); 
	virtual ~CRealFunction();
	virtual PRealFunction Clone() const=0;

// Inline methods
	double Start()
		{
		return m_rStart;
		}

	double End()
		{
		return m_rEnd;
		}

	virtual double Period()
		{
		return 0;
		}

// Methods
public:
	virtual RC Evaluate(
		double rAt,        // In: Parameter value to evaluate at
		double & rValue);  // Out: The requested coordiante there

	virtual RC Evaluate(
		double rAt,             // In: Parameter value to evaluate at
		double & rValue,        // Out: The requested coordiante there
		double & rDerivative);  // Out: The derivative there

	BOOL Clip(					// Return TRUE if this is a repeat offense
		double & t,				// In/Out: Tested number, clipped to the domain
		BOOL far & bHitStart, 	// In/Out: Was t beyond Start?
		BOOL far & bHitEnd);  	// In/Out: Was t beyond End?

	RC NewtonRaphson(
		double rSeed,     // In: Initial guess
		double & rRoot);  // Out: The root

#ifdef DEBUG
	virtual void Dump() const;
#endif

protected:
//Data 
	double m_rStart;     // Domain start
	double m_rEnd;       // Domain end
	double m_rTolerance; // t may be considered a root of f 
						 //          if |f(t)|<m_rTolerance
	double m_rAccuracy ; // Approximate accruacy of computation by this function 
	int m_nIterations;   // Limit on the number of iterations when solving
};

/********************************************************************
 *						Class CLinearFunction						*
 ********************************************************************/

// A linear function is of the form y = ax + b

class GEO_CLASS CLinearFunction : public CRealFunction
	{
	public :

	// Construction/destruction
	CLinearFunction(); // default is linear (0,1) identity function
	virtual ~CLinearFunction();
	CLinearFunction(const CLinearFunction& cOther)
		{
		m_valStart = cOther.m_valStart;
		m_valEnd = cOther.m_valEnd;
		}
	CLinearFunction(double, double, double, double); // build from domain and range
	PRealFunction Clone() const
		{
		return new CLinearFunction(*this);
		}

	//methods - virtual methods prescribed in CRealFunction base class
	virtual RC Evaluate(
		double rAt,        // In: Parameter value to evaluate at
		double & rValue);  // Out: The requested coordiante there

	virtual RC Evaluate(
		double rAt,             // In: Parameter value to evaluate at
		double & rValue,        // Out: The requested coordiante there
		double & rDerivative);  // Out: The derivative there

#ifdef DEBUG
	virtual void Dump() const;
#endif

	protected :
		double		m_valStart;
		double		m_valEnd;
	}; // end class CLinearFunction

////////////////////////////////////////////////////////////////////////////
///                  Miscellaneous  Equation solvers                     ///
////////////////////////////////////////////////////////////////////////////

GEO_API RC SolveQuadratic(
	double a,    // In: Quadratic coeffeicnt
	double b,    // In: Half the linear coefficent
	double c,    // In: The constant term
	double & r1, // Out: The root with smaller absolute value 
	double & r2);// Out: The root with greater absolute value
	
GEO_API RC SolveTrigEquation1(
	double a,    // In: Coeffeicnt of cos(x)
	double b,    // In: Coefficient of sin(x)
	double c,    // In: The constant term
	int & n,	 // Out: The number of roots - 0, 1 or 2
	double & r1, // Out: The smaller root
	double & r2);// Out: The greater root

GEO_API BOOL Clip(			  // =TRUE if this is a repeat offense
	double & t,       // In/Out: Tested number, clipped to the domain
	double rStart,    // In: Start of clipping domain
	BOOL & bHitStart, // In/Out: Was t beyond Start?
	double rEnd,      // In: End of clipping domain
	BOOL & bHitEnd);  // In/Out: Was t beyond End?

#endif

