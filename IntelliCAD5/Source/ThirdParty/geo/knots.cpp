/*	Knots.CPP - Implements the class CKnots.
 *	Copyright (C) 1994-1997 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * See Knots.h for the definition of the class; this file implements its methods.
 * Knots methods are those spline methods that need no knowledge of anything else
 * but the knots.
 *
 */
#include "Geometry.h"
#include "knots.h"
#ifdef DEBUG
#include <ostream.h>
#endif // DEBUG
 
DB_SOURCE_NAME;

/***************************************************************************/
// Default constructor
CKnots::CKnots()
    {
	m_nCurrentSpan = m_nDegree = 0;
	m_rInterval = 0;
    }
/***************************************************************************/
// Copy constructor
CKnots::CKnots(
	const CKnots & Other,  // In: The other knots to copy
	RC & rc)			   // Out: Return code
// Use the real array copy constructor
: CRealArray(Other, rc)
    {
	m_nDegree = Other.m_nDegree;
	m_nCurrentSpan = Other.m_nCurrentSpan;
	m_rInterval = Other.m_rInterval;
    }
/*******************************************************************************/
// Construct from a packed knot sequence
CKnots::CKnots(
	LPBYTE & pStorage,  // In/Out: The storage
	RC & rc) 		    // Out: Return code
	{
	m_nCurrentSpan = 0;

	// Unpack the header data
	int* pI = (int*) pStorage;
	m_nSize = pI[0];
	m_nDegree = pI[1];
	pStorage += 2 * sizeof(int);

	double* pR = (double*)pStorage;
	m_rInterval = *pR;
	pStorage += sizeof(double);

	// Unpack the actual knots
	if (rc = ALLOC_FAIL(m_rEntries = new double[m_nSize]))
		QUIT
	memcpy (m_rEntries, pStorage, m_nSize * sizeof(double));
	pStorage += m_nSize * sizeof(double);

	// Set other header fields
	m_nAllocated = m_nSize;
	m_nGrowBy = 10;
	SetCurrentSpan();
exit:;
	}
/***************************************************************************/
// Construct from an array of nodes with a given degree
CKnots::CKnots(
	const CRealArray & arrNodes,  // In: An array of nodes
	int nDegree,                  // In: The desired degree
	RC & rc)                      // Out: Return code
/* This constructor assumes, WITHOUT CHECKING, that the nodes are strictly
   increasing. It does check, however that the degree and size are valid.
 */

// Use the real array copy constructor            
: CRealArray(arrNodes, rc)
    {
	m_nDegree = 0;
	m_nCurrentSpan = 0;
	m_rInterval = 0;
	if (rc)
		QUIT

	// Validity checks
    if (m_nSize < 2 || m_nSize > MAX_SPLINE_SIZE  ||
        nDegree < 1  ||  nDegree > MAX_DEGREE)
		{
		rc = E_BadSpline;
    	QUIT
		}

    // Set the degree and current span
	m_nDegree = nDegree;
	
	// Set the interval and forget the last knot
	m_rInterval = m_rEntries[--m_nSize] - m_rEntries[0];
	SetCurrentSpan();
	rc = SUCCESS;
exit:;	
    }
/***************************************************************************/
// Construct with a given number of knots and degree initialized to 0
CKnots::CKnots(
	int nDegree,     //In: The degree
	int nSize,       //In: The number of knots
	RC & rc)		 // Out: Return code
	:CRealArray(nSize, rc)
    {
		m_nDegree = nDegree;
		m_nCurrentSpan = 0;
		m_rInterval = 0;

	// Validity checks
	if (rc)
		QUIT
    if (nSize > MAX_SPLINE_SIZE  || nDegree < 1 || nDegree > MAX_DEGREE)
		rc = E_BadSpline;
    else
		rc = SUCCESS;
exit:;
    }
/*****************************************************************************/
// Construct as the restriction or extension of another knot sequence
CKnots::CKnots(
	PKnots pOther,	// In: Another knot sequence
	double rFrom,   // In: New curve start
	double rTo,     // In: New curve end
	int nDegree,	// In: The new degree
	RC & rc)		// Out: return code
	{
	rc = SUCCESS;
	int i, iFirst, iLast;

	m_nDegree = nDegree;
	m_nCurrentSpan = nDegree;
	m_rInterval = rTo - rFrom;

	if (nDegree < 1 ||  nDegree > MAX_DEGREE)
		{
		rc = E_BadInput;
		QUIT
		}

	
	// Start with a multiple knot
	for (i = 0;  i <= nDegree;  i++)
		if (rc = Add(rFrom))
			QUIT

	// Add intermediate knots
	if (rc = pOther->FindSpan(rFrom, iFirst))
		QUIT
	iFirst++;
	if (rc = pOther->FindSpan(rTo, iLast))
		QUIT
	while (FEQUAL(pOther->Knot(iLast), rTo, FUZZ_GEN))
		iLast--;
	for (i = iFirst;  i <= iLast;  i++)
		if (rc = Add(pOther->Knot(i)))
			QUIT

	rc = Sanitize();
exit:;
	}

/***************************************************************************/
// Destructor
CKnots::~CKnots()
    {
    }
/***************************************************************************/
// Evaluate all non-zero functions and their derivatives                                                  		
RC CKnots::EvaluateBasis(
	int nS,			  // In: Index of left knot closest to rAt 
	double rAt,       // In: The parameter value to be evaluated at
	double* rValues,  // Out values relevant of basis fuctions
	double* rFirst,   // Out First derivatves of these basis fuctions
	double* rSecond)  // Out Second derivatives of these basis fuctions
// Basis functions evaluated are returned in increasing order
    {
// For brevity
#define d m_nDegree

	int j,m;
	
	double* v[MAX_ORDER];
	double prWorkSpace[MAX_ORDER*3];

	// Copy 2d knots into a local buffer, to reduce the cost of calling Knot()
	double t[2*MAX_DEGREE];
	for (j=0, m=nS-d+1;   j < 2*d;  m++,j++)
		t[j] = Knot(m);
#define T(k) t[d-1+(k)]	// Thus T(k) stands for Knot(nS+k)
	
	// Set up Workspace as a virtual square array with MAX_ORDER rows and columns,
	// Since we only need 3 rows at a time, we let v roll over through its rows.
	memset (prWorkSpace, 0, 3 * MAX_ORDER * sizeof(double));
	for (m=0,j=0;  m<=d;  m++)
	    {
		v[m] = &prWorkSpace[j];
		if (j < MAX_ORDER * 2)
			j += MAX_ORDER;
		else
			j = 0;
	    }
	
	//The values of the degree 0 basis functions
	v[0][0] = 1.0;
	
    /* Using the notation of deBoor's "Practical Guied to Splines", p. 131,
       Bi,k is the k-1 degree basis function whose support begins at ti
       Using the index s for nSpan, the conversion table is:
       
       v[m][j] = Bs-m+j,m+1               Bi,k = v[k-1][i+k-s-1]
       m = k - 1                           k = m + 1
       j = j + k - s - 1                   i = s - m + 1
        .
       Using this table, the recursion formula on p. 131 is applied to compute  
       v[i][j] = B s,i-1(rAt). The first and last cases in the loop reflect
       the implicit fact that v[m][-1] = v[m][m+1] = 0.
     */
	for (m=1;  m<=d;  m++)
	    {
		// First
		v[m][0] = v[m-1][0] * ((T(1) - rAt) / (T(1) - T(-m+1)));

        // Intermediate
		for (j=1;  j<m;  j++)
		    {	
			v[m][j] = v[m-1][j-1] * ((rAt - T(-m+j)) / 
			               (T(j) - T(-m+j)))
					+ v[m-1][j] * ((T(j+1) - rAt) / 
					    (T(j+1) - T(-m+j+1)));
		    }
        // Last
		v[m][m] = v[m-1][m-1] * ((rAt - T(0)) / (T(m) - T(0)));
	    }
	
	// At this stage we have the values of the Degree, Degree-1 and Degree-2
	// basis functions stored in v[Degree], v[Degree-1] and v[Degree-2], 
	// respectively.  (They are actually stored in the Workspace in some
	// order that we don't care to know, but the v array points to them)
	// Now we have to rerutn what the caller asked for.
	
	// Function values
	if (rValues)
		memcpy (rValues, v[m_nDegree], (m_nDegree+1)*sizeof (double));
		
	// First derivatives are computed from the degree-1 basis functions
	// based on the recursion formula in Farin's "Curves and Surfaces for
	// Computer Aided Geometric Design", II Edition, p169 

	if (rFirst)
	    {
		rFirst[0] = - d * v[d-1][0] / (T(1) - T(-d+1));

		for (j=1;  j<d;  j++)
			rFirst[j] = d * ( v[d-1][j-1] / (T(j)   -   T(-d+j))
			                - v[d-1][j]   / (T(j+1) - T(-d+j+1)));

		rFirst[d] = d * v[d-1][d-1] / (T(d) - T(0));
	    }

	// Second derivatives
	if (rSecond)
	    {
		memset(rSecond, 0, (d+1) * sizeof(double));
		for (j=0;  j<=d;  j++)
		    {
			if (j > 0)
			    {
				if (j > 1)
					rSecond[j] += v[d-2][j-2] / 
					((T(j) - T(-d+j)) * (T(j-1) - T(-d+j)));

                if (j < d)
                    {
					double s = v[d-2][j-1] / (T(j) - T(-d+j+1)); 
					rSecond[j] -= s / (T(j) - T(-d+j));
					rSecond[j] -= s / (T(j+1) - T(-d+j+1));
				    }
			    }
            if (j < d - 1)
				rSecond[j] += v[d-2][j] /
				        ((T(j+1) - T(j-d+1)) * (T(j+1) - T(-d+j+2)));
			rSecond[j] *= d * (d - 1);
		    }
	    }
 	RETURN_RC(CKnots::EvaluateBasis, SUCCESS);
    }
#undef d
#undef T
/****************************************************************************/
// Basis evaluation with no derivatives when you don't know the span
RC CKnots::EvaluateBasis(
	double rAt,       // In: The parameter value to be located
	double* rValues)  // Out values relevant of basis fuctions
    {
	int nS;
	RC rc = FindSpan(rAt, nS);
	EvaluateBasis(nS, rAt, rValues, NULL, NULL);
	RETURN_RC(CKnots::EvaluateBasis, rc);
    }
/***************************************************************************/
// Locate a given parameter value in a span                                                   		
RC CKnots::FindSpan(
	double t,		   // In: The parameter value to be located
	int & nSpan) const // Out: Index of the greatest pseudoknot s so that s <= t;

// Find the span containing t, and set m_nCurrentSpan to the found span
    {
	RC rc = SUCCESS;
	int nTop, nFloor;
	int nShift = 0;

#ifdef DEBUG
	// Record the input state in case the assertion at the end fails
	double rInput = t;
	int nBefore = m_nCurrentSpan;
	ASSERT_ELSE(m_nSize > 0, "FindSpan called on an empty knot set")
		{
		// Quit in a big hurry; "goto exit" would cause an infinite 
		// loop at the assertion there
		rc = E_BadSpline;
		return E_BadSpline;
		}
#endif

	if (m_nSize <= 0)
		{
		rc = E_BadSpline;
		QUIT
		}

	// Locate modulu the base interval
	if (Period())
		if (rc = PullToInterval(t, nShift))
			QUIT
	
	// Your best bet is the most recently used span. If that fails try its neighbors.
	if (t >= m_rEntries[m_nCurrentSpan]) // Go up
	    {
		if (m_nCurrentSpan >= Size() - 1)   // We're in the last span or beyond
			goto exit;

		if (t < m_rEntries[m_nCurrentSpan+1])   // We're in the correct span
			goto exit;

		// Try the same at the neighboring span above
		m_nCurrentSpan++;
		if (m_nCurrentSpan >= Size() - 1)  // We're in the last span or beyond
			goto exit;
			
		if (t < m_rEntries[m_nCurrentSpan+1])   // We're in the correct span
			goto exit;

		// Not found, prepare for a binary search
		nFloor = m_nCurrentSpan + 1;
		nTop = Size() - 1;
	    }
	else // Go down
	    {
		if (m_nCurrentSpan <= 0)   // We're in the first span or below
			goto exit;
		m_nCurrentSpan--;  
		if (m_nCurrentSpan <= 0  ||
		  t >= m_rEntries[m_nCurrentSpan])
			goto exit;
		// Not found, prepare for a binary search
		nFloor = 0;
		nTop = m_nCurrentSpan;
	    }
	
	// Entry does not lie in the current span or near it, go for a full search
	m_nCurrentSpan = BinarySearch(t, nFloor, nTop);

	// We consider the first an last span as extending to +/- infinity
	if (t > m_rEntries[m_nSize-1])
		m_nCurrentSpan = m_nSize - 1;
	if (t < m_rEntries[0])
		m_nCurrentSpan = 0;
	
exit:
	// Go to the last ocurrance of multiple knot
	while (m_nCurrentSpan < m_nSize - 1  && 
	       m_rEntries[m_nCurrentSpan] >= m_rEntries[m_nCurrentSpan+1])
		m_nCurrentSpan++;

	// Are we in the right span?
	ASSERT_ELSE((m_nCurrentSpan == m_nDegree  ||  Knot(m_nCurrentSpan) <= t) &&
				(m_nCurrentSpan == m_nSize - 1  || 
				 t <= Knot(m_nCurrentSpan+1) + FUZZ_GEN),
			     "FindSpan located in the wrong span")
		{
		// Diagnostics, if the assertion failed
		DUMP(*this);
		DB_MSGX("t=%lf,  m_nCurrentSpan before=%d,  after=%d",
		                      (rInput, nBefore, m_nCurrentSpan));
		}
	nSpan = m_nCurrentSpan + nShift;
	RETURN_RC(CKnots::FindSpan, rc);
    }
/*****************************************************************************/
// Get all the occurrances of a given knot
RC CKnots::GetKnot(
    double & rAt,        // In: Parameter value for the knot
	                     // Out: Same, possibly snapped to a knot  within tolerance
	int & nFirst,        // Out: The index of the first occurrance of the knot
	int & nLast,         // Out: The index of the last occurrance of the knot
	int & nMultiplicity) // Out: The current multiplicity
/*
 * If multiplicity=0 then nFirst and nLast return the indices of the knots
 * immediately before and after rAt. Multiplicity = -1 if rAt is outside the range
 */
    {
    int nF, nL, nMul;
    double t = rAt;
	RC rc = SUCCESS;

	if (rc = FindSpan(t, nF))
		QUIT
	nMul = 0;

	// Try to snap to the nearest knot
	if (FEQUAL(t, Knot(nF), FUZZ_GEN))
	    {
		t = Knot(nF);
		nMul = 1;
	    }
	 
	if (FEQUAL(t, Knot(nF+1), FUZZ_GEN))
	    {
		t = Knot(++nF);
		nMul = 1;
	    }

	if (nMul)
		{
		// There's a knot there, need to compute multiplicity
		// Go back to the first occurrance of t
		while (FEQUAL(t, Knot(nF-1), FUZZ_GEN))
			nF--;

 		// Now go up to the last occurrance, counting multiplicity
 		nL = nF;
		while (FEQUAL(t, Knot(nL+1), FUZZ_GEN))
	    	{
			nL++;
			nMul++;
	    	}
	    }
	else
	    nL = nF + 1;

	nFirst = nF;
	nLast = nL;
	nMultiplicity = nMul;
	rAt = t;
exit:
	RETURN_RC(CKnots::GetKnot, rc);
    }
/*****************************************************************************/
// Insert a knot with desired multiplicity
RC CKnots::InsertKnot(
	double & rAt,      // In: Parameter value for the knot
	                   // Out: An existing knot within tolerance, if there' one
	int nContinuity,   // In: The desired continuity at the knot
	int & nFirst,      // Out: The index of the first inserted knot
	int & nAdded)      // Out: The number of knots actually added
    {
/* If there is an existing knot within rSnap from rAt then the insertion will
 * take place at the existing knot, repeating it up to the rquested multiplicity.
 * Thus, the number of knots acutally added depends on what was already there.
 * Knots inserted at an existing knot are considered inserted AFTER the last
 * preexisting occurrance.
 */

	int nInsert,nMultiplicity;
	RC rc = SUCCESS;

	// Get any existing knot there (within tolerance)
	GetKnot(rAt, nFirst, nInsert, nMultiplicity);

	// Determine how many additional knots are needed there
	nAdded = Degree() - nContinuity - nMultiplicity;
	if (nAdded < 1)  
		// There is nothing to add
		nAdded = 0;
	else
		{
		// Insert with the necessary multiplicity
		rc = Insert(rAt, nInsert, nAdded);
		nFirst = nInsert;
		}
	RETURN_RC(CKnots::InsertKnot, rc);
    }
/***************************************************************************/
// Insert a double entry at a given index repeatedly                                                 		
RC CKnots::Insert(
	double rEntry,  // In: The new entry
	int nAt,        // In: The index of the first new entry
	int nTimes)     // In: The number of times to insert
// Return the new size
    {
    // Validity check
    RC rc = E_BadSpline;
    int i;
	int nLast = nAt + nTimes;       	

	// Validity check
    if (nTimes < 0)
		QUIT
	if (Size() + nTimes > MAX_SPLINE_SIZE)
		{
		rc = E_TooManyKnots;
    	QUIT
		}
    	
	// Make room for the new entries
	if (rc = MakeRoom(nAt, nTimes))
		QUIT
	
	//Enter the new entry
	for (i=nAt;  i<nLast;  i++)
		m_rEntries[i] = rEntry;
	rc = SUCCESS;
exit:		                               
    RETURN_RC(CKnots::Insert, rc);
    }
/*****************************************************************************/
// Reverse for reversing the direction of the spline
RC CKnots::Reverse()
/* 
 * The reversed knot sequence will occupy the same interval.  The knots will be
 * redefined so that the sequence of their differences is reversed.
 */
	{
	int i;
	int nLast = m_nSize + m_nDegree;
	double rEnd = Knot(nLast);
	double * pNew = new double[m_nSize];
	RC rc = ALLOC_FAIL(pNew);
	if (rc)
		QUIT
	
	// Reverse the knots 
	for (i = 0;  i < m_nSize;  i++)
        pNew[i] = rEnd - Knot(nLast - i);
	delete [] m_rEntries;
	m_rEntries = pNew;
exit:
	RETURN_RC(CKnots::Reverse, SUCCESS);
    }
/*****************************************************************************/
// Construct from annother knot-sequence but with a different degree
CKnots::CKnots(
	const CKnots & Other, // In: Knot sequence to copy from
	int nNewDegree,       // In: The desired new degree of the
	RC & rc)			  // Out: Return code
	// Use this constructor without degree change to sanitize a knot sequence
	{
	double rCurrent;
	int i,j,nMultiplicity;
	
	int nDegree = Other.Degree();
	int nDegreeDifference = nNewDegree - Other.Degree();

	// Default initialization
	rc = SUCCESS;
	m_nDegree = m_nCurrentSpan = 0;
	m_rInterval = Other.m_rInterval;
	
	// Loop on Other's knots
	for (i=1;    i<=Other.Size();    i++ /*i is incremented inside the loop as well */)
		{
		// Get the multiplicity of the current knot, and copy it
		rCurrent = Other[i-1];
		nMultiplicity = 1;
		while (i < Other.Size() && FEQUAL(Other[i], rCurrent, FUZZ_GEN))
			{
			nMultiplicity++;
			i++;
			}
	
		// Sanity check
		if (nMultiplicity > nDegree &&   i > nDegree + 1) 
			// It's OK to have multiplicity=degree+1 only at the first knot
	    	{
	    	rc = FAILURE;
	    	QUIT
	    	}
	    	
		// Figure out the new multiplicity
		nMultiplicity += nDegreeDifference;
		if (nMultiplicity < 1)
			nMultiplicity = 1;	    	

		// Add the knots
		for (j=1;  j<=nMultiplicity;  j++)
			if (rc = Add(rCurrent))
				QUIT
		}
		
	m_nDegree = nNewDegree;
	
	// Make sure we didn't end up with too many knots
	if (Size() > MAX_SPLINE_SIZE)
		rc = E_TooManyKnots;
	SetCurrentSpan();
exit:
	if (rc)
		RemoveAll();
	}		
#undef t
/***************************************************************************/
// Construct from another knot sequence, but with all interior knots of multiplicity degree
// This will be used for the piecewise Bezier representation.
CKnots::CKnots(
		const CKnots & Other, // In: Knot sequence to copy from
		BEZTYPE beztype, // In:  Make all interior knots of multiplicity degree
		RC & rc)        // Out:  Return code
		{
		double rCurrent;
		int i,j,nMultiplicity;
		int nDegree = Other.Degree();

		// Default initialization
		rc = SUCCESS;
		m_nDegree = m_nCurrentSpan = 0;
		m_rInterval = Other.m_rInterval;

		// Loop on Other's knots
		bool AtFirstKnotValue = true;
		for (i=1; i<=Other.Size(); i++ /* note that i is also incremented inside loop*/ )
			{   // begin i-loop
			// Get the multiplicity of the current knot and copy it
			rCurrent = Other[i-1];
			nMultiplicity = 1;
			while (i < Other.Size() && FEQUAL(Other[i], rCurrent, FUZZ_GEN))
				{
				nMultiplicity++;
				i++;
				}

			// Sanity check
			if ( (nMultiplicity > nDegree) && (i > nDegree + 1) )
				{ // multiplicity must be <= degree with the exception of the first knot
				rc = FAILURE;
				QUIT;
				}

			// Determine the new multiplicity
			if (AtFirstKnotValue)
				{
				nMultiplicity = nDegree + 1;
				AtFirstKnotValue = false;
				}
			else
				nMultiplicity = nDegree;

			// Add the knots
			for (j = 1; j <=nMultiplicity; j++)
				if (rc = Add(rCurrent))
					QUIT
			}   //   end i-loop

		m_nDegree = nDegree;

		// Make sure we didn't wind up with too many knots
		if (Size() > MAX_SPLINE_SIZE)
			rc = E_TooManyKnots;

		// Finish up
		SetCurrentSpan();

exit:
			if (rc)
				RemoveAll();


		}
/***************************************************************************/
// Pull a parameter value into the base interval                                                   		
RC CKnots::PullToInterval(
	double & t,			// In: The parameter value
						// Out: A representative within the basic domain interval
	int & nShift) const //Out: The number of spans between input and output t
    {
	RC rc = SUCCESS;
	double s = m_rEntries[0] + m_rInterval;
	nShift = 0;
	
	while (t > s)
		{
		t -= m_rInterval;
		nShift++;
		}
	while (t < m_rEntries[0])
		{
		t += m_rInterval;
		nShift--;
		}
	
	// We shouldn't really wrap around more than once, but let's be generous
	if (abs(nShift) > 8)
		rc = FAILURE;
	else
		nShift *= m_nSize;
	
	RETURN_RC(CKnots::PullToInterval, rc);
    }
/***************************************************************************/
// Remove knot                                                		
int CKnots::Remove(	// Return the new number of entries
	int nFrom,      // In: The first index of the entries to remove
	int nNumber)    // In: The number of entries to remove
	{
	// nFrom can't be greater than m_nSize - 3
	
	// Locate a representing knot within the actual array
	Mod(nFrom);

	// The reomved knots may possibly spill over the end of the list 
	int nSpillOver = MAX(nFrom + nNumber - m_nSize, 0);
	
	// Remove the first lot
	CRealArray::Remove(nFrom, nNumber - nSpillOver);
	
	// Remove the spill-over from the top of the list
	if (nSpillOver > 0)
		CRealArray::Remove(0, nSpillOver);
		
	// Reset the current span
	SetCurrentSpan();
		
	return m_nSize - nNumber;
	}
/****************************************************************/
// Access the knot with a given index
double CKnots::Knot(
	int nIndex) const  // The index of the point to be accessed
	{
	// pull the index into the range
	int k = Mod(nIndex);
	return m_rEntries[nIndex] + k * m_rInterval;
	}
/****************************************************************************/
// Period
double CKnots::Period() const
	{
/* A knot sequence is considered periodic if the multiplicity of the first knot
 * is less than degree + 1.
 */
	if (m_nSize <= m_nDegree
/* There are less than degree+1 knot, which implies that
 * Knot(Degree) = Knot(0) + Interval, so  Knot(Degree) != Knot(0)
 */    
 	||  m_rEntries[m_nDegree] != m_rEntries[0])
		return m_rInterval;
	else
		return 0;
	}
/***************************************************************************/
// Change the knots representation and make it start at a given knot                                                		
RC CKnots::StartAt(
	int nIndex)  // In: The desired index of the first knot
	{
	int i;
	double* p;
	RC rc = SUCCESS;
	
	if (nIndex == 0) // There is nothing to do
		goto exit;
	
    // Allocate new entries and copy the knots into it
	if (rc = ALLOC_FAIL(p = new double[m_nAllocated]))
		QUIT
	for (i=0;  i<m_nSize;  i++)
		p[i] = Knot(nIndex + i);
		
	// Replace the old entries by the new ones
	delete [] m_rEntries;
	m_rEntries = p;
exit:	
	return rc;
	}
/****************************************************************************/
void CKnots::Set(
	int nIndex,    // In: The index of the knot to be set
	double rValue) // In: The value to set it to
	{
	ASSERT(0 <= nIndex  && nIndex <= m_nSize);

	if (nIndex == m_nSize)
		m_rInterval = rValue - m_rEntries[0];
	else
		m_rEntries[nIndex] = rValue;
	}
/****************************************************************************/
// Breaks in continuity of the nth derivative of a spline with these knots
RC CKnots::Breaks(
	double rFrom,                     // In: Beginning of the interval of interest
	double rTo,                       // In: End of interval of interest
	int iDeriv,                     // In: the derivative whose breaks we seek
	PRealArray & pBreaks) const // Out: the list of breaks
/* 
 * All curves are treated as periodic, so even if the curve is open, 
 * its first break (at Start) will NOT be repeated at the end.  Thus, a
 * smooth open curve will show ONE break.
 * If the rFrom or rTo are inside the domain interval then they are 
 * considered breaks.
 */
	{
	RC rc = SUCCESS;
	pBreaks = new CRealArray;
	int i;
	
	int iMultiplicity = 1;
	int iMaxMultiplicity = Degree() - iDeriv;
	if (!pBreaks)
		{
		rc = DM_MEM;
		QUIT
		}
	
	// Is From a break?
	if (rFrom > Knot(m_nDegree))
		if (rc = pBreaks->Add(rFrom))
			QUIT
		
	// Loop on knots
	for (i=1;  i<=Size();  i++)
		{
		if (Knot(i) < rFrom)
			continue;
		if (Knot(i) > rTo)
			break;
		if (Knot(i) == Knot(i-1))
			iMultiplicity++;
		else
			{
			if (iMultiplicity > iMaxMultiplicity)
				if (rc = pBreaks->Add(Knot(i-1)))
					QUIT
			iMultiplicity = 1;
			}			
        }
	// Is rTo a break?
	if (rTo > Knot(m_nDegree) + m_rInterval)
		rc = pBreaks->Add(rTo);
exit:
	if (rc)
		{
		delete pBreaks;
		pBreaks = NULL;
		}		
	RETURN_RC(CKnots::Breaks, rc);;
	}
/*******************************************************************/
// Tweak to make it an honest spline if possible
RC CKnots::Sanitize()
	{
	RC rc = E_BadSpline;
	int i, j;
		
	// Check the degree and reset the current pointer
	if (m_nDegree < 1   ||  m_nDegree > MAX_DEGREE

	// Check the interval
	|| m_rInterval < FUZZ_GEN
		
	// See that we have enough knots
	|| m_nSize < MIN(m_nDegree, 3))
		QUIT

	for (i = 0;    /*stopping criterion inside the loop*/;    i = j)
		{
		// Get the multiplicity while snapping close knots together
 		for (j=i+1;  j < Size() && FEQUAL(m_rEntries[j], m_rEntries[i],FUZZ_GEN);  j++)
			m_rEntries[j] = m_rEntries[i];
			
		// Check that the multiplicity is acceptable
		if (j > i + m_nDegree  &&   j > m_nDegree + 1) 
			// It's OK to have multiplicity=degree+1 only at the first knot
	    	QUIT
	
		// Check monotonicity
		if (j >= m_nSize)
			break;
		if (m_rEntries[j] < m_rEntries[i])
	    	QUIT
		}
		
	// Check if the last knot lies well within the interval
	if (Last() + FUZZ_GEN > m_rEntries[0] + m_rInterval
	
	// Make sure we didn't end up with too many knots
	|| Size() > MAX_SPLINE_SIZE)
	    QUIT
	
	// Got a clean bill of health, set the current span
	rc = SUCCESS;
	SetCurrentSpan();
exit:
	RETURN_RC(CKnots::Sanitize, rc);
	}
/****************************************************************************/
void CKnots::GetDomainInterval(
	double & rStart,    // Out: Domain start
	double & rEnd)      // Out: Domain end
    {
	rStart = Knot(m_nDegree);
	rEnd = rStart +m_rInterval;
    }
/*******************************************************************************/
// Pack
RC CKnots::Pack(
	LPBYTE & pStorage) const // In/Out: The storage
	{
	// Pack the header data
	int* pI = (int*) pStorage;
	pI[0] = m_nSize;
	pI[1] = m_nDegree;
	pStorage += 2 * sizeof(int);

	double* pR = (double*)pStorage;
	*pR = m_rInterval;
	pStorage += sizeof(double);

	// Pack the actual knots
	memcpy (pStorage, m_rEntries, m_nSize * sizeof(double));
	pStorage += m_nSize * sizeof(double);

	RETURN_RC(CKnots::Pack, SUCCESS);
	}
/*******************************************************************************/
// Packing Size
int CKnots::PackingSize() const
	{
	return 2 * sizeof(int) +  (m_nSize + 1) * sizeof(double);
	}
 /*******************************************************************************/
// Add a knot as the last knot
RC CKnots::AddAsLast(
	double rKnot)  // In: The new knot
	{
	RC rc = SUCCESS;
	double rNewInterval;

	if (!m_rEntries)
		{
		// It's the first knot, just add it
		rc = CRealArray::Add(rKnot);
		goto exit;
		}	
	
	rNewInterval = rKnot - m_rEntries[0];
	if (rNewInterval < m_rInterval)
		rc = E_BadSpline;
	else
		{
		// Add the currently implied last knot as an actual knot
		rc = Add(m_rEntries[0] + m_rInterval);

		// Add the new knot as an implied last knot, by reseting the domain
		m_rInterval = rNewInterval;
		}
exit:
	RETURN_RC(CKnots::AddAsLast, rc);
	}
/*******************************************************************/
// Set the curve's domain interval and period from knots
void CKnots::SetInterval(
	double & rStart,   // In: Domain start
	double & rEnd,     // In: Domain end
	double & rPeriod)  // Out: Spline's period
	{
	rStart = m_rEntries[0];
	rEnd = rStart + m_rInterval;

	// Periodic first knot's multiplicity is less than degree
	if (m_rEntries[m_nDegree] == m_rEntries[0])
		rPeriod = 0;
	else
		rPeriod = m_rInterval;
   	} 
#ifdef DEBUG
 /*******************************************************************************/
// Dump
void CKnots::Dump() const
	{
	DB_MSGX("Knots, Degree=%d, Interval=%lf\n", (m_nDegree, m_rInterval));
	for (int i=0;  i<m_nSize;  i++)
 		DB_MSGX("Knot(%d)=%lf\n", (i, m_rEntries[i]));
	}

ostream& operator<<(ostream& out, const CKnots &knots)
{
	out << "Knot Vector: " << endl;
	out << "Degree: " << knots.m_nDegree << "; ";
	out << "Interval " << knots.m_rInterval << "; ";
	out << "Is Periodic ? ";
	if (knots.Period()) {
		out << "Yes" << endl;
		out << *dynamic_cast<CRealArray*>(const_cast<CKnots*>(&knots));
	}
	else {
		out << "No" << endl;
		out << "Size = " << knots.KnotCount() << endl;
		for (int i = 0;  i < knots.m_nSize;  i++)
			out << "Entry[" << i << "] = " << knots.m_rEntries[i] << endl;

		int order = knots.Order();
		double lastKnot = knots.m_rEntries[0] + knots.m_rInterval;
		for (register j = 0; j < order; j++) 
			out << "Entry[" << ++i << "] = " << lastKnot << endl;
	}
	return out;
}

#endif // DEBUG

/*******************************************************************************
 *
 * Normalize domain to [0, 1]
 *
 * Author:	Stephen W. Hou
 * Date:	4/142003
 */
CKnots& CKnots::Normalized()
{
	register length = m_nSize;
	double min = m_rEntries[0];
	register double *p = &m_rEntries[length];
	while (length--) 
		*p = (*--p - min) / m_rInterval;

	m_rInterval = 1.0;
	return *this;
}


/*******************************************************************************
 * 
 * Change domain to start at the desired value
 *
 *  Author:	Stephen W. Hou
 *  Date:	4/18/2003
 */
RC CKnots::StartAt(double rStart) 
{
	register length = m_nSize;
	double shift = m_rEntries[0] - rStart;
	register double *p = &m_rEntries[length];
	while (length--) 
		*--p -= shift;

	return SUCCESS;
}


/*******************************************************************************
 * 
 * Change domain to start at the desired value
 *
 *  Author:	Stephen W. Hou
 *  Date:	4/18/2003
 */
RC CKnots::Stretch(double rStart, double rEnd) 
{
	RC rc = SUCCESS;
	double newInterval = rEnd - rStart;
	if (fabs(m_rInterval) > FUZZ_REAL && fabs(newInterval) > FUZZ_REAL) {
		double f = newInterval / m_rInterval;
		double b = rStart - f * m_rEntries[0];
		
		register length = m_nSize;
		register double *p = &m_rEntries[length];
		while (length--) 
		*p = (*--p) * f + b;
		m_rInterval = newInterval;
	}
	else
		rc = FAILURE;
	return rc;
}

/*******************************************************************************/
// Return all knots in the knot vector.
// Author:	Stephen W. Hou
// Date:	4/17/2003
//
CRealArray CKnots::GetAllKnots() const
{
	CRealArray knots = *dynamic_cast<CRealArray*>(const_cast<CKnots*>(this));
	if (Period() == 0.) {
		double lastDistintKnot = m_rEntries[0] + m_rInterval;
		int order = Order();
		for (register i = 0; i < order; i++)
			knots.Add(lastDistintKnot);
	}
	return knots;
}


/*******************************************************************************/
// Arise the degree
//
// Author:	Stephen W. Hou
// Date:	4/24/2003
//
RC CKnots::Elevate()
{
	RC rc = SUCCESS;
	if (m_rEntries) {
		int allocated = 2 * m_nSize - m_nDegree;
		double* pEntries = new double[allocated];

		register count = 0;
		for (register i = 0; i < m_nSize; i++) {
			pEntries[count++] = m_rEntries[i];
			if (m_rEntries[i] < m_rEntries[i + 1]) 
				pEntries[count++] = m_rEntries[i];
		}
		m_nSize = count;
		delete [] m_rEntries;
		m_rEntries = pEntries;
		m_nAllocated = allocated;
		m_nDegree++;
	}
	else
		rc = FAILURE;

	return rc;
}


/*******************************************************************************/
// Join the knot vector to the end of the this knot vector
//
// Author:	Stephen W. Hou
// Date:	4/24/2003
//	
RC CKnots::JoinWith(const CKnots &knots, bool shareEnd)
{
	int removed = (shareEnd) ? 1 : 0;
	m_nAllocated = m_nSize + knots.m_nSize - removed;
	double* pEntries = new double[m_nAllocated];
	memcpy(pEntries, m_rEntries, m_nSize * sizeof(double));
	memcpy(pEntries + m_nSize, knots.m_rEntries + removed, (knots.m_nSize - removed) * sizeof(double));
	m_nSize = m_nAllocated;
	m_rInterval += knots.m_rInterval;
	delete m_rEntries;
	m_rEntries = pEntries;
	return SUCCESS;
}


/*******************************************************************************/
// Assign operator
// Author:	Stephen W. Hou
// Date:	4/5/2003
//
CKnots& CKnots::operator=(const CKnots &knots)
{
	CRealArray::operator=(knots);

	m_nDegree = knots.m_nDegree;
	m_nCurrentSpan = knots.m_nCurrentSpan;
	m_rInterval = knots.m_rInterval;

	return *this;
}