/*	PtArray.cpp - Implements the class CPointArray 
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * See PtArray.h for the definition of the class.
 * This file implements its methods	
 *
 *
 */
#include "Geometry.h"
#include "PtArray.h"
#ifdef DEBUG
#include <ostream.h>
#endif // DEBUG


DB_SOURCE_NAME;

/**************************************************************/
// Constructor
CPointArray::CPointArray()
    {
	m_ptEntries = NULL;
    }
/**************************************************************/
// Copy constructor
CPointArray::CPointArray(
	const CPointArray & Other,  //In: The other array to copy
	RC & rc)					// Out: Return code
    {
	m_nSize = m_nAllocated = Other.m_nSize;

	// Copy the other's entries
	m_ptEntries = new C3Point[m_nSize];
	if (m_ptEntries)
		{
		memcpy (m_ptEntries, Other.m_ptEntries, m_nSize * sizeof(C3Point));
		rc = SUCCESS;
		}
	else
		rc = DM_MEM;
    }
/*******************************************************************************/
// Construct by unpacking
CPointArray::CPointArray(
	LPBYTE & pStorage,  // In/Out: The storage
	RC & rc)			// Out: Return code
	{
	int* pI = (int*)pStorage;
	m_nSize = m_nAllocated = *pI;
	m_nGrowBy = 10;
	pStorage += sizeof(int);

	m_ptEntries = new C3Point[m_nSize];
	if (m_ptEntries)
		{
		for (int i=0;  i < m_nSize;  i++)
			m_ptEntries[i].Unpack(pStorage);
		rc = SUCCESS;
		}
	else
		rc = DM_MEM;
	}
/**************************************************************/
// Construct with a given number of points, uninitialized
CPointArray::CPointArray(
	int nSize,  //In: The number of points
	RC & rc)	// Out: Return code
    {
	m_nSize = m_nAllocated = nSize;
	m_ptEntries = new C3Point[nSize];
	if (m_ptEntries)
		rc = SUCCESS;
	else
		rc = DM_MEM;
    }
/**************************************************************/
//Destructor
CPointArray::~CPointArray()
    {
	delete [] m_ptEntries;
    }
/***************************************************************************/
// Insert a C3Point entry at a given index of the list                                                 		
RC CPointArray::Insert(
	C3Point & ptPoint,  // In: The new entry to be inserted
	int nAt)            // In: The index of the new entry
    {
	// Make room for the new entry
	RC rc = MakeRoom (nAt, 1);

	//Enter the new entry
	if (!rc)       	
		m_ptEntries[nAt] = ptPoint;
	RETURN_RC(CPointArray::Insert, rc);     
    }
/***************************************************************************/
// Maek room for inserting additional entries                                                 		
RC CPointArray::MakeRoom(
	int   nAt,    // In: The index after which we need to insert entries
	int   nNew)   // In: The number of entries to make room for
    {
// This function allocates more space and shifts entries to make room for 
// addtional entries
	RC rc = SUCCESS;
	
	ASSERT(nAt >= 0);
	ASSERT(nAt <= m_nSize);
	ASSERT(nNew >= 0);

	if (m_nSize + nNew > m_nAllocated)  //There's not enough room
	    {
		//Extend the array
		P3Point pTemp;
		m_nAllocated += MAX(nNew, m_nGrowBy);

		pTemp = new C3Point[m_nAllocated];
		if (!pTemp)
			{
			rc = DM_MEM;
			goto exit;
			}

		if (nAt>0)
			memcpy (pTemp, m_ptEntries, nAt * sizeof(C3Point));
		if (nAt<m_nSize)
			memcpy (pTemp+nAt+nNew, m_ptEntries+nAt, (m_nSize - nAt) * sizeof(C3Point));
		if (m_nSize> 0)
			delete [] m_ptEntries;
		m_ptEntries = pTemp;
	    }
	else  // Shift entries to make room
		memmove (m_ptEntries+nAt+nNew, m_ptEntries+nAt, (m_nSize - nAt) * sizeof(C3Point));

    m_nSize += nNew;
exit:
	RETURN_RC(CPointArray::MakeRoom, rc);
    }
/***************************************************************************/
/// Add a double entry at the end of the list                                                 		
RC CPointArray::Add(
	const C3Point & ptEntry)  // In: The new entry to be added
    {
	RC rc = MakeRoom(m_nSize, 1);
	if (!rc)
		m_ptEntries[m_nSize-1] = ptEntry;
    RETURN_RC(CPointArray::Add, rc);
    }
/***************************************************************************/
// Append a portion of another array                                                		
RC CPointArray::Copy(
	const CPointArray & Other, // In: The array to copy from
	int nFrom,         // In: The first index of the entries to append
	int nNumber)       // In: The number of entries to append
    {
	int nOldSize = m_nSize;

	// Make room for the new entries
	RC rc = MakeRoom (m_nSize, nNumber);
    // This has already updated the array size!
        
	// Apend
	if (!rc)
		memcpy ( m_ptEntries + nOldSize, Other.m_ptEntries + nFrom, 
	          							nNumber * sizeof(C3Point) );
	RETURN_RC(CPointArray::Copy, rc); 
    }
/***************************************************************************/
// Remove entries from an array                                                		
void CPointArray::Remove(
	int nFrom,      // In: The first index of the tail
	int nNumber)    // In: The number of entries to remove
    {
	memcpy ( m_ptEntries + nFrom,  
	          m_ptEntries + nFrom + nNumber, 
	          (m_nSize - nFrom - nNumber) * sizeof(C3Point) ); 
	m_nSize -= nNumber;
    }
/***************************************************************************/
// Remove all entries                                               		
void CPointArray::RemoveAll()
    {
	delete [] m_ptEntries;
	m_ptEntries = NULL;
	m_nSize = m_nAllocated = 0;
    }
/*******************************************************************************/
// Pack
RC CPointArray::Pack(
	LPBYTE & pStorage) const// In/Out: The storage
	{
	int* pI = (int*) pStorage;
	*pI = m_nSize;
	pStorage += sizeof(int);

	for (int i=0;  i < m_nSize;  i++)
		m_ptEntries[i].Pack(pStorage);
		
	RETURN_RC(CPointArray::Pack, SUCCESS);
	}
/*******************************************************************************/
// Packing Size
int CPointArray::PackingSize() const
	{
	return sizeof(int) +  m_nSize * m_ptEntries->PackingSize();
	}
/*****************************************************************************/
// Reallocate to the exact size
RC CPointArray::ReallocateToSize()
	{
	RC rc = SUCCESS;
	P3Point pTemp = m_ptEntries;

	if (m_nSize == m_nAllocated)
		goto exit;		// 'cause there's nothing to do

	if (m_nSize > 0)
		{
		if (rc = ALLOC_FAIL(m_ptEntries = new C3Point[m_nSize]))
			QUIT
		memcpy(m_ptEntries, pTemp, m_nSize * sizeof(C3Point));
		delete [] pTemp;
		}
	else
		{
		delete [] m_ptEntries;
		m_ptEntries = NULL;
		}

	m_nAllocated = m_nSize;
exit:
	RETURN_RC(CPointArray::ReallocateToSize, rc);
	}
/*****************************************************************************/
// Reverse the order
RC CPointArray::ReverseOrder()
	{
	int i;
	int nLast = Size();
	int nHalf = nLast / 2;
	nLast--;
	
	// Reverse
	for (i=0;  i<nHalf;  i++)
		{
		C3Point P = m_ptEntries[i];
		m_ptEntries[i] = m_ptEntries[nLast - i];
        m_ptEntries[nLast - i] = P;
    	}
	RETURN_RC(CPointArray::ReverseOrder, SUCCESS);
    }
/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Set new size reallocating the array if necessary.
 *			Note, the previous data is lost.
 * Returns:	geo RC codes.
 ********************************************************************************/
RC
CPointArray::setSize(int newSize)
{
// I have added this method because of bad design of the class (performance).

	RC	rc = SUCCESS;

	if(newSize > m_nAllocated)
	{
		delete [] m_ptEntries;
		if(rc = ALLOC_FAIL(m_ptEntries = new C3Point[newSize]))
			QUIT
		m_nSize = m_nAllocated = newSize;
	}
	else
		m_nSize = newSize;

exit:
	RETURN_RC(CPointArray::setSize, rc);
}

/********************************************************************************
 * Assign operation
 *
 * Author:	Stephen W. Hou.
 * Date:	4/24/2003
 * Returns:	geo RC codes.
 ********************************************************************************/
CPointArray& CPointArray::operator=(const CPointArray &ptArray)
{
	if (this != &ptArray) {
		m_nSize = m_nAllocated = ptArray.m_nSize;
		if (m_nSize) {
			if (m_ptEntries)
				delete m_ptEntries;

			m_ptEntries = new C3Point[m_nSize];
			memcpy(m_ptEntries, ptArray.m_ptEntries, m_nSize * sizeof(C3Point));
		}
		else
			m_ptEntries = NULL;
	}
	return *this;
}
/*******************************************************************************/
#ifdef DEBUG
// Dump
void CPointArray::Dump()
	{
	DB_MSGX("Points array",());
	for (int i=0;  i<m_nSize;  i++)
		DB_MSGX("Point(%d)=(%lf, %lf)",(i, m_ptEntries[i].X(), m_ptEntries[i].Y()));
	}

ostream& operator<<(ostream &out, const CPointArray &ptArray)
{
	out << "Points array" << endl;
	for (int i = 0;  i < ptArray.m_nSize;  i++) {
		out << "Point[" << i << "] = ";
		out << ptArray.m_ptEntries[i];
	}
	return out;
}

#endif
