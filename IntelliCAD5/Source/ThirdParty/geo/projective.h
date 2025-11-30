// The class of projective transformations from the plane to itself.

#ifndef PROJECTIVE_H
#define PROJECTIVE_H

#include "Geometry.h"
#include "Point.h"

#include "trnsform.h"

class GEO_CLASS CProjective // NOT a subclass of CTransform for this implementation
{
public:
	// constructors

	// default constructor
	CProjective();


	// use this only if you are confident of no divide by zero
	void ApplyToPoint(const C3Point ptIn,
		              C3Point & ptOut) const;


	// It is the responsibility of the user to make sure that this is
	// applied only to points having z coordinate equal to zero.
	void ApplyToPoint(const C3Point ptIn,  // In: The point to be transformed
		              C3Point & ptOut,     // Out:  The transformed point             
					  RC & rc) const;      // Out: SUCCESS if nonzero denominator
                                           //      FAILURE otherwise

	// This function requires that P[0] = (0,0,0)
	void BuildFrom4NormalizedPointPairs(C3Point P[4],
		                                C3Point Q[4],
										RC & rc);


// The usage of the following function is:
// If A, B and C are objects of type CProjective, then
// A = B.Of(C) evaluates as follows:  A(P) = B(C(P))
CProjective Of(CProjective & That);

	// may at some stage decide to make this private, but, for now...
public:
	Matrix3X3 m_Matrix; // this is the only data member
};

// The following is not a member function

GEO_API void Projective4Pt(C3Point P[4], // In:  Points in XY plane to be transformed by T
			       C3Point Q[4], // In:  Points in XY plane that should equal T(P[i]).
			       CProjective & T,  // Out: T is projective and satisfies T(P[i]) = Q[i]
			       RC & rc);     // Out:  SUCCESS if T was constructed
                                 //       FAILURE if T could not be constructed



#endif

