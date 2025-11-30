/*	Knots.h - Defines the class of spline knots
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 * A knot list is a finite real array with periodic view that makes it
 * seem infinite.  If there are actually n points in the list then Knot[n+k]
 * is understood as Knot[k] + Interval, where Interval is a fixed real number.
 * This approach is applied to knots of nonperiodic curves as well.  For example,
 * the knots that would be viewd as (0,0,1,1) (of a straight line) are stored
 * here as {(0,0), Interval=1}.  This allows a unified approach to periodic and
 * nonperiodic splines.  The continuity at the beginning (and end) of the domain
 * is always Degree-Multiplicity, and a nonperiodic spline is a just periodic
 * one with continuity -1 at the beginning and end. This way the number of 
 * control-points is always equal to the number of knots.
 * The knot list is aware of its degree, and would refuse attempts to make it
 * invalid as a B-splne knot sequence (e.g. Knot[i] < Knot[i+1], or 
 * Multiplicity > Degree).
 *
 */
#ifndef KNOTS_H
#define KNOTS_H

#include "Array.h"
#ifdef DEBUG
class ostream;
#endif // DEBUG

class GEO_CLASS CKnots : public CRealArray
{
// Construction & destruction
public:
	CKnots();

	CKnots(
		const CKnots & Other,  // In: The other knots to copy
		RC & rc);			   // Out: Return code

	CKnots(
		LPBYTE & pStorage,  // In/Out: The storage
		RC & rc);			// Out: Return code

	CKnots(
		const CKnots & Other, // In: Knot sequence to copy from
		int nNewDegree,       // In: The desired new degree of the
		RC & rc);             // Out: Return code

	CKnots(
		const CKnots & Other, // In: Knot sequence to copy from
		BEZTYPE beztype, // In:  Make all interior knots of multiplicity degree
		RC & rc);        // Out:  Return code

	CKnots(
		const CRealArray & arrNodes,  // In: An array of nodes
		int nDegree,                  // In: The desired degree
		RC & rc);                     // Out: Return code
		
	CKnots(
		int nDegree, //In: The degree
		int nSize,   //In: The number of knots 
		RC & rc);    // Out: Return code
		
	CKnots(
		PKnots pOther,	// In: Another knot sequence
		double rFrom,   // In: New curve start
		double rTo,     // In: New curve end
		int nDegree,	// In: The new degree
		RC & rc);		// Out: return code

	virtual ~CKnots();
	
// Attributes
	int Degree() const;
	double Period() const;
	double Knot(int nIndex) const;    //The knot with a given index

	RC Breaks(
		double rFrom,                // In: Beginning of the interval of interest
		double rTo,                  // In: End of interval of interest
		int iDeriv,                  // In: the derivative whose breaks we seek
		PRealArray & pBreaks) const; // Out: the list of breaks

	double Interval() const;

// Operations
	RC FindSpan(
		double t,			// In: The parameter value to be located
		int & nSpan) const; // Out: Index of the greatest knot s so that s <= t;
		
    RC EvaluateBasis(int, double, double*, double*, double*);
    RC EvaluateBasis(int, double, double*);
	RC GetKnot(double &, int &, int &, int &);
	
	RC InsertKnot(
		double & rAt,      // In: Parameter value for the knot
		                   // Out: An existing knot within tolerance, if there' one
		int nContinuity,   // In: The desired continuity at the knot
		int & nFirst,      // Out: The index of the first inserted knot
		int & nAdded);     // Out: The number of knots actually added
		
	RC Insert(double, int, int);
	int  Remove(int, int);   // Remove entries
	RC Reverse(); // Reverse the direction
	RC EvaluateBasis(
		double rAt,       // In: The parameter value to be located
		double* rValues   // Out values relevant of basis fuctions
	); 
	void SetDegree(int);

    void Set(
		int nIndex,     // In: The index of the knot to be set
		double rValue); // In: The value to set it to

	RC PullToInterval(
		double & t,      // In: The parameter value
		                 // Out: A representative within the basic domain interval
		int & nShift) const; //Out: Number of Intervals away from the basic domain
 
	RC StartAt(
		int nIndex);  // In: The desired index of the first knot

	RC StartAt(
		double startValue); // In: The desired start value

	void SetInterval(
		double rValue);   // In: The value to set it to

	void GetDomainInterval(
		double & rStart,    // Out: Domain start
		double & rEne);     // Out: Domain end

	// Normalize parameter domain to [0, 1].
	//
	CKnots& Normalized();

	RC Stretch(
		double rFactor); // In: The factor to stretch the domain by

	// Reparameterize knots into the desired domain.
	//
	RC Stretch(
		double rStart,	 // In: The desired domain start
		double rEnd);    // In: The desired domain end;

	RC JoinWith(
		const CKnots &knots,	// In: The appended knot vector
		bool shareEnd = true);

	RC Sanitize();

	void SetCurrentSpan();

	RC Pack(
		LPBYTE & pStorage) const;  // In/Out: The storage

	int PackingSize() const;

	RC AddAsLast(
		double rKnot);  // In: The new knot

	void SetInterval(
		double & rStart,   // In: Domain start
		double & rEnd,     // In: Domain end
		double & rPeriod); // Out: Spline's period

	// Return all knots in the knot vector
	//
	CRealArray  GetAllKnots() const;

	// Arise the degree
	//
	RC Elevate();

	// Override base class function
	//
	virtual void Sort() { /* Knot vector should never need to be sorted! */ }

	// Return the size of knot vector. Note it may be different than 
	// the size of knots stored in the underlying CRealArray.
	// 
	int			KnotCount() const;

	int			Order() const { return (m_nDegree + 1); }

	CKnots& operator=(const CKnots &knots);


#ifdef DEBUG
	void Dump() const;
	friend ostream& operator<<(ostream& out, const CKnots &knots);
#endif

// Data
protected:
	int m_nDegree;
	mutable int m_nCurrentSpan;
	double m_rInterval;
};

////////////////////////////////////////////////////////////////////////////////
// Implementation of inline methods
// Degree
inline int CKnots::Degree() const
    {
	return m_nDegree;
    }
/****************************************************************************/
inline void CKnots::SetDegree(
	int nDegree) // In: The desired degree
    {
	m_nDegree = nDegree;
    }
/****************************************************************************/
// Basis evaluation with no derivatives
inline RC CKnots::EvaluateBasis(
	int nS,            // In: Index of left knot closest to rAt 
	double rAt,        // In: The parameter value to be located
	double* rValues)   // Out values relevant of basis fuctions
    {
	return EvaluateBasis(nS, rAt, rValues, NULL, NULL);
    }
/****************************************************************************/
inline double CKnots::Interval() const
    {
	return m_rInterval;
    }
/****************************************************************************/
inline void CKnots::SetInterval(
	double rValue)   // In: The value to set it to
    {
	m_rInterval = rValue;
    }
/*******************************************************************/
// Set the current span cache to a valid span
inline void CKnots::SetCurrentSpan()
	{
/* In order for the FindSpan algorithm to wrok efficiently, we cache the
 * most rectent span we worked in, as the first place to look when we need
 * to search again.  For the evaluator to work properly, it mus always be a
 * nontrivial span. This function will set it to the last span, because it is
 * the only span that is guaranteed to be nontrivial.
 */
 	m_nCurrentSpan = m_nSize - 1; 
	}
/*******************************************************************************/
// Return all knots in the knot vector.
// Author: Stephen W. Hou
// Date:	4/17/2003
//
inline int CKnots::KnotCount() const 
{
	return (Period()) ? Size() : (Size() + Order());
}
#endif
