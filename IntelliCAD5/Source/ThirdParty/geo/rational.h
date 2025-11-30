/*	Rational.cpp - Defines the class of NURBS functions and its methods
 *	Copyright (C) 1996-98 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 *  The function is stored as a spline curve, whose control points store 
 *  the weights as their Y coordinates.
 *
 */
#ifndef RATIONAL_H
#define RATIONAL_H

#include "roots.h"	// For the base class CRealFunction

class GEO_CLASS CRationalFunction	:	public CRealFunction
	{
	public:
	
	// Constructors/destructor
	CRationalFunction();
	
	virtual ~CRationalFunction();

	CRationalFunction(
		const CRealArray & cX,	// In: Data nodes - must be strictly increasing
		const CRealArray & cY,	// In: Data values - must be monotone (see below)
		const CRealArray & cDY,	// In: The derivatives - must be monotone
		RC & rc);					// Out: Return code

	PRealFunction Clone() const;

	// Methods
	RC Evaluate(
		double t,       // In: Parameter value to evaluate at
		double & v);	// Out: Function value

#ifdef DEBUG
	void Dump();
#endif

	RC Evaluate(
		double t,		// In: Parameter value to evaluate at
		double& v,		// Out: Function value
		double& dv);	// Out: Function derivative

	// data
	protected:
	PCurve m_pCurve;
	};

typedef CRationalFunction * PRationalFunction;

#endif

