/*	ARRAY.CPP - Implements the classes CGeoObject, CGeoArray and CRealArray
 *	Copyright (C) 1994-97 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * See Array.h for the definition of the classes, this file implements their methods.
 * Some other general services are implemented here
 *
 *
 */

#include "Geometry.h"
#include "Array.h"
#ifdef DEBUG
#include <ostream.h>
#endif // DEBUG

DB_SOURCE_NAME;

/////////////////////////////////////////////////////////////////////////////
// Generic Array

// Constructor
 CGeoArray::CGeoArray()
    {
	m_nAllocated = 0;
	m_nSize = 0;
	m_nGrowBy = 10;
    }

//Destructor
 CGeoArray::~CGeoArray(){}
/***************************************************************************/
// Represent an integer n as n = q * Size + r
int  CGeoArray::Mod(
	int & nIndex) const  // In: An integer (see n above)
		                 // Out: r - An integer within the range
// Return q = (Output nIndex - Input nIndex) / Size
	{
	int k = 0;

	while (nIndex >= m_nSize)
		{
		nIndex -= m_nSize;
		k++;
		}
	while (nIndex < 0)
		{
		nIndex += m_nSize;
		k--;
		}

	return k;
	}
/////////////////////////////////////////////////////////////////////////////
// Array of double

// Default onstructor
 CRealArray::CRealArray() : m_bSorted(false), m_rEntries(NULL)
    {
    }

// Construct with a given number of entries initialized to 0
 CRealArray::CRealArray(
	int nSize, // In: The number of entries
	RC  & rc)  // Out: Return code
    : m_bSorted(false)
	{
	if (nSize > 0)
		{
		m_nSize = m_nAllocated = nSize;
		if (rc = ALLOC_FAIL(m_rEntries = new double[nSize]))
			QUIT
		memset(m_rEntries, 0, nSize * sizeof(double));
		}
	else
		m_rEntries = NULL;
	rc = SUCCESS;
exit:;
    }

//Destructor
 CRealArray::~CRealArray()
    {
		if (m_rEntries)
			delete [] m_rEntries;
    }
/***************************************************************************/
// Copy constructor
 CRealArray::CRealArray(
	const CRealArray & Other,  // In: The other array to copy
	RC & rc)				   // Out: Return code
    {
	rc = SUCCESS;

	// Copy the other's entries
	m_nSize = m_nAllocated = Other.m_nSize;
	if (m_nSize) {
		if (rc = ALLOC_FAIL(m_rEntries = new double[m_nSize]))
			QUIT
		memcpy (m_rEntries, Other.m_rEntries, m_nSize * sizeof(double));
		m_bSorted = Other.m_bSorted;
	}
	else {
		m_rEntries = NULL;
		m_bSorted = false;
	}
exit:;
    }

/**************************************************************************/
// Set up with given number of entries. NOTE the data will be lost
// if the array is shrunk.
//
// Auther:  Stephen W. Hou
// Date:	4/7/2003
//
void CRealArray::SetSize(int newSize)
{
	if (newSize != m_nSize) {
		if (newSize == 0) {
			if (m_rEntries)
				delete [] m_rEntries;
			m_rEntries = NULL;
			m_nAllocated = m_nSize = 0;
		}
		else {
			if (newSize <= m_nAllocated)
				m_nSize = newSize;
			else {
				m_nAllocated = newSize;
				double* pEntries = new double[newSize];
				if (m_rEntries) {
					memcpy(pEntries, m_rEntries, m_nSize * sizeof(double));
					delete [] m_rEntries;
				}
				m_rEntries = pEntries;
				m_nSize = newSize;
			}
		}
	}
}

/***************************************************************************/
/// Add a double entry at the end of the list
RC CRealArray::Add(
	double rEntry) // In: The new entry to be added
    {
	RC rc = MakeRoom(m_nSize, 1);
	if (!rc)
		m_rEntries[m_nSize-1] = rEntry;

	if (m_bSorted && m_rEntries[m_nSize-1] < m_rEntries[m_nSize-2])
		m_bSorted = false;

	RETURN_RC(CRealArray::Add, rc);
    }
/***************************************************************************/
// Maek room for inserting additional entries
RC CRealArray::MakeRoom(
	int   nAt,    // In: The index of the inserted
	int   nNew)   // In: The number of entries to make room for
    {
	RC rc = SUCCESS;
	int nNewSize = MAX(m_nSize, nAt) + nNew;

	ASSERT(nAt >= 0);
	ASSERT(nAt <= m_nSize);
	ASSERT(nNew >= 0);

	if (nNewSize > m_nAllocated)  //There's not enough room
	    {
		//Extend the array as required, but at least by m_nGrowBy
		m_nAllocated += MAX(nNewSize - m_nAllocated, m_nGrowBy);

		if (m_nSize > 0)
		    {
			double * p;
			if (rc = ALLOC_FAIL(p = new double[m_nAllocated]))
				QUIT
			memcpy(p, m_rEntries, m_nSize * sizeof(double));
			delete [] m_rEntries;
			m_rEntries = p;
			}
		else
			if (rc = ALLOC_FAIL(m_rEntries = new double[m_nAllocated]))
				QUIT
	    }
	// Shift entries to make room, if necessary
	if (nAt < m_nSize)
		memmove (m_rEntries+nAt+nNew, m_rEntries+nAt, (m_nSize-nAt) * sizeof(double));

    m_nSize = nNewSize;
exit:
    RETURN_RC(CRealArray::MakeRoom, rc);
    }
/***************************************************************************/
// Insert a double entry at a given index of the list
RC CRealArray::Insert(
	double  rEntry,  // In: The new entry to be inserted
	int   nAt)       // In: The index of the new entry
    {
	ASSERT(nAt >= 0  &&  nAt <= m_nSize);
	RC rc = MakeRoom(nAt, 1);

	//Enter the new entry
	if (!rc)
		m_rEntries[nAt] = rEntry;

	if (m_bSorted && nAt > 0 && m_rEntries[nAt-1] > rEntry)
		m_bSorted = false;
    RETURN_RC(CRealArray::Insert, rc);
    }

/***************************************************************************/
// Remove the entry at a given index of the list
int  CRealArray::Remove(
	int nAt)      // In: The index of the entry to remove
// Return the new size
    {
	ASSERT(nAt >= 0  &&  nAt < m_nSize);

	// Shift the remaining entries, if any
	int nTail = m_nSize - nAt - 1;
	if (nTail > 0)
		memcpy (m_rEntries + nAt, m_rEntries + nAt + 1, nTail * sizeof(double) );
	return --m_nSize;
    }

/*********************************************************************************/
/// Add a double in the correct place to a sorted list of REALs iff it doesn't exist
///
RC  CRealArray::AddSort(
	double rEntry,     // In: The new entry to be added
	double rTolerance, // In: Computational zero
	int & nAt)		   // Out: The index where this entry fits in the array
    {
	RC rc = SUCCESS;
	if (Size() == 0)  // This is the first entry, just add it
	    {
		rc = Add(rEntry);
		nAt = 0;
	    }
	else
		{
		if (!m_bSorted)
			Sort();

		if (!Search (rEntry, rTolerance, nAt))
			// The entry is not found in the list, so insert it
			rc = Insert(rEntry, ++nAt);
		}
    RETURN_RC(CRealArray::AddSort, rc);
    }
/***************************************************************************/
// Search a array for the location of a new entry. The function returns TRUE
// if the entry exists in the array within given tolerance and FLASE otherwise.
// NOTE: The third output parameter will returns either the last index i such as
// rEntry >= Entries[i] if the new entry is greater than one of entries or -1
// if the array is empty or the value of new entry is smaller than any one in
// the array.
//
BOOL  CRealArray::Search(
	double rEntry,     // In: The entry to be located
	double rTolerance, // In: Computational zero
	int  & nIndex	   // Out: The last index i such that Entry >= Entries[i]
) const
// Return TRUE if rEntry is equal to Entries[nIndex-1]
{
	nIndex = -1;
	if (m_nSize == 0)
		{
		// The list is empty
		return FALSE;
		}

	int nTop=m_nSize, nFloor=0, nMid=m_nSize - 1;

	// Calling Sort() here may not be ideal but I don't see any other better option
	// because the implementation after the following call is based on the assumption
	// the data in the array are sorted but in fact they may not. SWH, 9-26-2003
	//
	const_cast<CRealArray*>(this)->Sort();

	if (rEntry < m_rEntries[0] - rTolerance) // the new entry is the lowest
		return FALSE;

	do
		{
		if (rEntry >= m_rEntries[nMid] - rTolerance)
			nFloor = nMid;
		else
			nTop = nMid;

		nMid = (nFloor + nTop) / 2;

		} while (nMid > nFloor);

	nIndex = nTop - 1;

	return (fabs(rEntry - m_rEntries[nIndex]) <= rTolerance);  // the rTolerance could be zero so it should be <= rTolerance
}

/***************************************************************************/
// Binary search within a range in a sorted array for the location of an entry
int CRealArray::BinarySearch(
	double rEntry,   // In: The entry to be located
	int nFloor,      // In: The first index in the search range
	int nTop) const  // In: The last index in the search range
// Return the index i such that Entries[i] <= rEntry < Entries[i+1]
    {
    // never search in an empty range
	ASSERT(nFloor <= nTop);
	ASSERT(m_bSort);

	const_cast<CRealArray*>(this)->Sort();

	while (1)
	    {
		// Halve the index-range
		int nMid = (nFloor + nTop) / 2;
		if (nMid == nFloor)
			//Nothing to halve, the search is over
			return nFloor;

		// Choose one of the halves
		if (rEntry >= m_rEntries[nMid])
			nFloor = nMid;
		else
			nTop = nMid;
	    }
    }
/***************************************************************************/
// Copy a portion of another array and append it to this one
RC CRealArray::Copy(
	const CRealArray & Other, // In: The array to copy from
	int nFrom,        // In: The first index in the other array to copy
	int nNumber)      // In: The number of entries to copy
    {
	int nOldSize = m_nSize;

	// Make room for the new entries
	RC rc = MakeRoom (m_nSize, nNumber);
	// This has already updated the array size!

	// Apend
	if (!rc)
		memcpy (m_rEntries + nOldSize, Other.m_rEntries + nFrom,
	    							     nNumber * sizeof(double) );
	m_bSorted = Other.m_bSorted;
	RETURN_RC(CRealArray::Copy, rc);
    }
/***************************************************************************/
// Remove entries from an array
int CRealArray::Remove(	// return the new size
	int nFrom,      // In: The first index of the entries to remove
	int nNumber)    // In: The number of entries to remove
    {
	if (nNumber >= m_nSize)
		RemoveAll();
	else
		{
		memcpy ( m_rEntries + nFrom,
				 m_rEntries + nFrom + nNumber,
				 (m_nSize - nFrom - nNumber) * sizeof(double) );
		m_nSize -= nNumber;
		}
	return m_nSize;
    }
/***************************************************************************/
// Remove all entries
void CRealArray::RemoveAll()
    {
		if (m_rEntries)
			delete [] m_rEntries;
		m_nSize = m_nAllocated = 0;
		m_rEntries = NULL;
    }
/***************************************************************************/
// Sum up the entries
double CRealArray::Sum()
    {
	double rSum = 0;
	for (int i = 0;   i < m_nSize;  i++)
		rSum += m_rEntries[i];
	return rSum;
	}
/*****************************************************************************/
// Reverse the order
void CRealArray::Reverse()
	{
	int i;
	int nLast = Size();
	int nHalf = nLast / 2;
	nLast--;

	for (i=0;  i<nHalf;  i++)
		SWAP(m_rEntries[i], m_rEntries[nLast - i], double)

	m_bSorted = false;
	}
/*****************************************************************************/
// Reallocate to the exact size
RC CRealArray::ReallocateToSize()
	{
	RC rc = SUCCESS;
	double * pTemp = m_rEntries;

	if (m_nSize == m_nAllocated)
		goto exit;		// 'cause there's nothing to do

	if (m_nSize > 0)
		{
		if (rc = ALLOC_FAIL(m_rEntries = new double[m_nSize]))
			QUIT
		memcpy(m_rEntries, pTemp, m_nSize * sizeof(double));
		delete [] pTemp;;
		}
	else
		{
		delete [] m_rEntries;
		m_rEntries = NULL;
		}

	m_nAllocated = m_nSize;
exit:
	RETURN_RC(CRealArray::ReallocateToSize, rc);
	}

/*****************************************************************************/
// Assign operator
//
// Author:	Stephen W. Hou
// Date:	4/13/2003
CRealArray& CRealArray::operator=(const CRealArray &array)
{
	ASSERT(&array != NULL);

	if (&array != this) {
		if (m_rEntries != NULL)
			delete [] m_rEntries;

		m_nAllocated = m_nSize = array.m_nSize;
		m_nGrowBy = array.m_nGrowBy;
		if (m_nSize > 0) {
			m_rEntries = new double[m_nSize];
			memcpy(m_rEntries, array.m_rEntries, m_nSize * sizeof(double));
		}
		else
			m_rEntries = NULL;

		m_bSorted = array.m_bSorted;
	}
	return *this;
}


/*****************************************************************************/
//
// Sort array in ascending numerial order
//
// Author:	Stpehen W. Hou
// Date:	4/23/2003
//
void CRealArray::Sort()
{
	if (!m_bSorted) {
		if (m_nSize > 1) {
			if (m_nSize <= 10) { // Straight insert sort
				register i, j;
				for (i = 1; i < m_nSize; i++) {
					double a = m_rEntries[i];
					j = i - 1;
					while (j >= 0 && m_rEntries[j] > a) {
						m_rEntries[j + 1] = m_rEntries[j];
						j--;
					}
					m_rEntries[j + 1] = a;
				}
			}
			else if (m_nSize > 10 && m_nSize <= 40) { // Diminishing increase sort
				int inc = 1;
				// determine the starting increasement
				do {
					inc *= 3;
					inc++;
				} while (inc < m_nSize);

				// look over the partial sorts
				do {
					inc /= 3;
					int j;
					// outer loop of straight insertion
					for (register i = inc; i < m_nSize; i++) {
						double a = m_rEntries[i];
						j = i;
						// inner loop of straight insertion
						while (m_rEntries[j - inc] > a) {
							m_rEntries[j] = m_rEntries[j - inc];
							j -= inc;
							if (j <= inc)
								break;
						}
						m_rEntries[j] = a;
					}
				} while (inc > 1);
			}
			else { // Heapsort
				double a;
				register i, j;
				register l = m_nSize >> 1;
				register ir = m_nSize - 1;

				for (;;) {
					if (l > 0)
						a = m_rEntries[--l];
					else {
						a = m_rEntries[ir];
						m_rEntries[ir] = m_rEntries[0];
						if (--ir == 0) {
							m_rEntries[0] = a;
							break;
						}
					}
					i = l;
					j = (l << 1) + 1;
					while (j <= ir) {
						if (j < ir && m_rEntries[j] < m_rEntries[j + 1])
							j++;

						if (a < m_rEntries[j]) {
							m_rEntries[i] = m_rEntries[j];
							i = j;
							j <<= 1;
							j++;
						}
						else
							j = ir + 1;
					}
					m_rEntries[i] = a;
				}
			}
		}
		m_bSorted = true;
	}
}



#ifdef DEBUG
/***************************************************************************/
// Dump
 void CRealArray::Dump() const
 	{
	DB_MSGX("Size = %d\n", m_nSize);
		for (int i=0;  i<m_nSize;  i++)
			DB_MSGX("Entry[%d] = %lf\n",(i, m_rEntries[i]));
	}

ostream& operator<<(ostream& out, const CRealArray &array)
{
	out << "Size = " << array.m_nSize << endl;
	for (int i = 0;  i < array.m_nSize;  i++)
		out << "Entry[" << i << "] = " << array.m_rEntries[i] << endl;
	return out;
}

#endif

/////////////////////////////////////////////////////////////////////////////
// Object

// Constructor / destructor
CGeoObject::CGeoObject()
    {
    }

CGeoObject::~CGeoObject()
    {
	}
/*****************************************************************************/
#ifdef DEBUG
// Dump
void CGeoObject::Dump() const
	{
	DB_MSGX("I don't know how to dump\n", ());
	}
#endif // DEBUG

/////////////////////////////////////////////////////////////////////////////
// Object Array

// Constructor
CGeoObjectArray::CGeoObjectArray() : m_pEntries(NULL)
    {
    }

//Destructor
CGeoObjectArray::~CGeoObjectArray()
    {
	Purge();
	}

// Purge the contents
void CGeoObjectArray::Purge()
    {
	if (m_pEntries)
		{
		for (int i=0;  i<m_nSize;  i++)
			delete m_pEntries[i];
		delete [] m_pEntries;
		m_pEntries = NULL;
		}
	m_nSize = m_nAllocated = 0;
    }
/***********************************************************************************/
//The object with a given index
PGeoObject CGeoObjectArray::operator [] (int nIndex) const
    {
	if (nIndex >= 0		&&		nIndex < m_nSize)
		return m_pEntries[nIndex];
	else
		return (PGeoObject)NULL;
    }
/***********************************************************************************/
// Add an object at the end of the list
RC CGeoObjectArray::Add(
	PGeoObject pObject)  //In: The object to be added
    {
	RC rc = MakeRoom(m_nSize, 1);
	if (!rc)
		m_pEntries[m_nSize-1] = pObject;
	RETURN_RC(CGeoObjectArray::Add, rc);
    }
/***************************************************************************/
// Maek room for inserting additional entries
RC CGeoObjectArray::MakeRoom(
	int   nAt,    // In: The index of the inserted, must not be negative
	int   nNew)   // In: The number of entries to make room for
    {
	RC rc = SUCCESS;
	int nNewSize = MAX(m_nSize, nAt) + nNew;

	ASSERT(nAt >= 0);
	ASSERT(nNew >= 0);

	if (nNewSize > m_nAllocated)  //There's not enough room
	    {
		//Extend the array as required, but at least by m_nGrowBy
		m_nAllocated += MAX(nNewSize - m_nAllocated, m_nGrowBy);

		if (m_nSize > 0)
		    {
			PGeoObject* p;
			if (rc = ALLOC_FAIL(p = new PGeoObject[m_nAllocated]))
				QUIT
			memset(p, 0 , m_nAllocated*sizeof(PGeoObject));
			memcpy(p, m_pEntries, m_nSize * sizeof(PGeoObject));
			delete [] m_pEntries;
			m_pEntries = p;
			}
		else
			{
			if (rc = ALLOC_FAIL(m_pEntries = new PGeoObject[m_nAllocated]))
				QUIT
			memset(m_pEntries, 0 , m_nAllocated*sizeof(PGeoObject));
			}
	    }
	// Shift entries to make room, if necessary
	if (nAt < m_nSize)
		memmove (m_pEntries+nAt+nNew, m_pEntries+nAt, (m_nSize-nAt) * sizeof(PGeoObject));

    m_nSize = nNewSize;
exit:
    RETURN_RC(CGeoObjectArray::MakeRoom, rc);
    }
/***************************************************************************/
// Insert an object at a given index of the list
RC CGeoObjectArray::Insert(
	PGeoObject pObject,   // In: The new object to be inserted
	int   nAt)         // In: The index of the new entry
    {
	RC rc = FAILURE;
	ASSERT(nAt >= 0);
	ASSERT(nAt <= Size());

	if (nAt < 0)
		QUIT

	if (rc = MakeRoom(nAt, 1))
		QUIT

	//Enter the new entry
	m_pEntries[nAt] = pObject;
exit:
	RETURN_RC(CGeoObjectArray::Insert, rc);
    }
/***************************************************************************/
// Remove the entry at a given index of the list
int  CGeoObjectArray::Remove(	// Return the new size
	int   nAt)				// In: The index of the entry to remove
    {
	ASSERT(nAt >= 0  &&  nAt < m_nSize);

	// Shift the remaining entries, if any
	int nTail = m_nSize - nAt - 1;
	if (nTail > 0)
	memcpy (m_pEntries + nAt, m_pEntries + nAt + 1,
	        nTail * sizeof(PGeoObject) );
	return --m_nSize;
    }
/***************************************************************************/
// Remove entries
int CGeoObjectArray::Remove(
	int nFrom,      // In: The first index of the entries to remove
	int nNumber)    // In: The number of entries to remove
    {
	if (nNumber >= m_nSize)
		RemoveAll();
	else
		{
		memcpy (m_pEntries + nFrom,
				 m_pEntries + nFrom + nNumber,
				 (m_nSize - nFrom - nNumber) * sizeof(PGeoObject));
		m_nSize -= nNumber;
		}
	return m_nSize;
    }
/***************************************************************************/
// Remove all entries
void CGeoObjectArray::RemoveAll()
    {
	if (m_pEntries)
		delete [] m_pEntries;
	m_nSize = m_nAllocated = 0;
	m_pEntries = NULL;
    }
/***************************************************************************/
// Copy a portion of another array and append it to this one
RC CGeoObjectArray::Copy(
	const CGeoObjectArray  & Other, // In: The array to copy from
	int nFrom,        // In: The first index in the other array to copy
	int nNumber)      // In: The number of entries to copy
    {
	int nOldSize = m_nSize;

	// Make room for the new entries
	RC rc = MakeRoom (m_nSize, nNumber);  // This updates m_nSize to the new size

	// Apend
	if (!rc)
		memcpy (m_pEntries + nOldSize, Other.m_pEntries + nFrom,
	         						nNumber * sizeof(PGeoObject) );
	RETURN_RC(CGeoObjectArray::Copy, rc);
    }
/*****************************************************************************/
// Reverse the order
void CGeoObjectArray::Reverse()
	{
	int i;
	int nLast = Size();
	int nHalf = nLast / 2;
	nLast--;

	for (i=0;  i<nHalf;  i++)
		{
		PGeoObject p = m_pEntries[i];
		m_pEntries[i] = m_pEntries[nLast - i];
        m_pEntries[nLast - i] = p;
    	}
    }
/*****************************************************************************/
// Delete the object and null the pointer
void CGeoObjectArray::DeleteObject(
	int nIndex)   // The object be deleted
	{
	delete m_pEntries[nIndex];
	m_pEntries[nIndex] = NULL;
	}
/*****************************************************************************/
// Reallocate to the exact size
RC CGeoObjectArray::ReallocateToSize()
	{
	RC rc = SUCCESS;
	PGeoObject * pTemp = m_pEntries;

	if (m_nSize == m_nAllocated)
		goto exit;		// 'cause there's nothing to do

	if (m_nSize > 0)
		{
		if (rc = ALLOC_FAIL(m_pEntries = new PGeoObject[m_nSize]))
			QUIT
		memcpy(m_pEntries, pTemp, m_nSize * sizeof(PGeoObject));
		delete [] pTemp;;
		}
	else
		{
		delete [] m_pEntries;
		m_pEntries = NULL;
		}

	m_nAllocated = m_nSize;
exit:
	RETURN_RC(CGeoObjectArray::ReallocateToSize, rc);
	}
////////////////////////////////////////////////////////////////////////////////////////////
//			General Services for the Geometry Library
////////////////////////////////////////////////////////////////////////////////////////////

// Convert double to int
int RealToInt(
	double dbl
	){
	/* Add in the rounding threshold.
	 *
	 * NOTE: The MAXWORD bias used in the floor function
	 * below must not be combined with this line. If it
	 * is combined the effect of FUZZ_GEN will be lost.
	 */
	dbl += 0.5 + FUZZ_GEN;

	/* Truncate
	 *
	 * The UINT_MAX bias in the floor function will cause
	 * truncation (rounding toward minuse infinity) within
	 * the range of a short.
	 */
	dbl = floor(dbl + UINT_MAX) - UINT_MAX;

	/* Clip the result.
	 */
	return 	dbl > SHRT_MAX - 7 ? SHRT_MAX - 7 :
			dbl < SHRT_MIN + 7 ? SHRT_MIN + 7 :
			(int)dbl;
	}



/*****************************************************************************/
#ifdef DEBUG
// Dump
void CGeoObjectArray::Dump() const
	{
	for (int i=0;  i<m_nSize;  i++)
		m_pEntries[i]->Dump();
	}
#endif // DEBUG
