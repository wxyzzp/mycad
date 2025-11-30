/*	Array2D.H - Defines the class of 2-dimensional real array
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 * ABSTRACT
 * Unlike 1 dimensionall arrays (CRealArray), 2 dimensional arrays are not geared for 
 * dynamically adding storage space for new entries, so there is no Add method.
 * The entries are stored in a contiguous array of doubles, and there's an array of
 * double pointers to the beginnings of the rows.  This allows addressing the (i,j)th
 * entry as A[i][j].
 *
 */
#ifndef ARRAY2D_H
#define ARRAY2D_H

#include "geometry.h"
#include "point.h"


template <class Type>
class C2DArray
{
// Construction & destruction
public:
	C2DArray() : m_pRows(NULL), m_rEntries(NULL), m_nRowCount(0), m_nColCount(0) {}
	C2DArray(const C2DArray &array);
	C2DArray(
		int nRows,    //In: Number of rows
		int nCols,    //In: Number of columns
		RC & rc); //Out: Return code

	virtual ~C2DArray() { delete [] m_rEntries; delete [] m_pRows; }

	C2DArray& operator=(const C2DArray &array);

// Attributes
public:
	int RowCount() const;
	int ColCount() const;

	Type* operator [](int nIndex) const;    //The row with a given index

// Data
protected:
	int m_nRowCount;
	int m_nColCount;
	Type ** m_pRows;
	Type * m_rEntries;
};

class GEO_CLASS CReal2DArray 
{
public:

	CReal2DArray() {}
    CReal2DArray(const CReal2DArray &array);
	CReal2DArray( int nRows,    //In: Number of rows
		          int nCols,    //In: Number of columns
		         RC & rc); //Out: Return code
	virtual ~CReal2DArray() { delete [] m_rEntries; delete [] m_pRows; }

	CReal2DArray& operator=(const CReal2DArray &array);

	int RowCount() const;
	int ColCount() const;

	double* operator [](int nIndex) const; 

protected:
	int m_nRowCount;
	int m_nColCount;
	double ** m_pRows;
	double * m_rEntries;
};


typedef C2DArray<C3Point> CPoint2DArray;

typedef CReal2DArray * PRral2DArray;
typedef CPoint2DArray * PPoint2DArray;


////////////////////////////////////////////////////////////////////////////////
//		Implementation of Inline Methods
// Number of rows
template <class Type>
inline int C2DArray<Type>::RowCount() const
	{
	return m_nRowCount;
	}

// Number of columns
template <class Type>
inline int C2DArray<Type>::ColCount() const
	{
	return m_nColCount;
	}

//The row with a given index
template <class Type>
inline Type* C2DArray<Type>::operator [](int nIndex) const
	{
	return m_pRows[nIndex];
	}

template <class Type>
inline C2DArray<Type>::C2DArray(const C2DArray<Type> &array)
{
	m_nRowCount = array.m_nRowCount;
	m_nColCount = array.m_nColCount;

	int nEntryCount = m_nRowCount * m_nColCount;
	if (nEntryCount) {
		m_pRows = new Type* [m_nRowCount];
		m_rEntries = new Type [nEntryCount];

		for (register i = 0;  i < m_nRowCount;  i++)
			m_pRows[i] = &m_rEntries[i * m_nColCount];

		for (i = 0; i < nEntryCount; i++) 
			m_rEntries[i] = array.m_rEntries[i];
	}
	else {
		m_pRows = NULL;
		m_rEntries = NULL;
	}
}

// Construct with a given size
template <class Type>
inline C2DArray<Type>::C2DArray(
	int nRows,    //In: Number of rows
	int nCols,    //In: Number of columns
	RC & rc)	  //Out: Return code
    {
	rc = SUCCESS;

	// Set the fields
	m_nRowCount = nRows;
	m_nColCount = nCols;

	//Allocate space
	int nEntryCount = nRows * nCols;
	m_pRows = new Type* [nRows];
	m_rEntries = new Type [nEntryCount];
	if (!m_pRows  ||  !m_rEntries)
		{
			// Allocation failed
		rc = DM_MEM;
		delete [] m_pRows;
		m_pRows = NULL;
		delete [] m_rEntries;
		m_rEntries = NULL;
		}
	else

		//Set the pointers	
		for (int i=0;  i<nRows;  i++)
			m_pRows[i] = &m_rEntries[i * nCols];
    }


template <class Type>
inline C2DArray<Type>& C2DArray<Type>::operator=(const C2DArray<Type> &array)
{
	ASSERT(&array != NULL);

	if (&array != this) {
		m_nRowCount = array.m_nRowCount;
		m_nColCount = array.m_nColCount;

		delete [] m_rEntries;
		delete [] m_pRows;

		int nEntryCount = m_nRowCount * m_nColCount;
		if (nEntryCount) {
			m_pRows = new Type* [m_nRowCount];
			m_rEntries = new Type [nEntryCount];

			for (register i = 0;  i < m_nRowCount;  i++)
				m_pRows[i] = &m_rEntries[i * m_nColCount];

			for (i = 0; i < nEntryCount; i++) 
				m_rEntries[i] = array.m_rEntries[i];
		}
		else {
			m_pRows = NULL;
			m_rEntries = NULL;
		}
	}
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
//		Implementation of Inline Methods
// Number of rows
inline int CReal2DArray::RowCount() const
	{
	return m_nRowCount;
	}

// Number of columns
inline int CReal2DArray::ColCount() const
	{
	return m_nColCount;
	}

//The row with a given index
inline double* CReal2DArray::operator [](int nIndex) const
	{
	return m_pRows[nIndex];
	}

inline CReal2DArray::CReal2DArray(const CReal2DArray &array)
{
	m_nRowCount = array.m_nRowCount;
	m_nColCount = array.m_nColCount;

	int nEntryCount = m_nRowCount * m_nColCount;
	if (nEntryCount) {
		m_pRows = new double* [m_nRowCount];
		m_rEntries = new double [nEntryCount];

		for (register i = 0;  i < m_nRowCount;  i++)
			m_pRows[i] = &m_rEntries[i * m_nColCount];

		for (i = 0; i < nEntryCount; i++) 
			m_rEntries[i] = array.m_rEntries[i];
	}
	else {
		m_pRows = NULL;
		m_rEntries = NULL;
	}
}

// Construct with a given size
inline CReal2DArray::CReal2DArray(
	int nRows,    //In: Number of rows
	int nCols,    //In: Number of columns
	RC & rc)	  //Out: Return code
    {
	rc = SUCCESS;

	// Set the fields
	m_nRowCount = nRows;
	m_nColCount = nCols;

	//Allocate space
	int nEntryCount = nRows * nCols;
	m_pRows = new double* [nRows];
	m_rEntries = new double [nEntryCount];
	if (!m_pRows  ||  !m_rEntries)
		{
			// Allocation failed
		rc = DM_MEM;
		delete [] m_pRows;
		m_pRows = NULL;
		delete [] m_rEntries;
		m_rEntries = NULL;
		}
	else

		//Set the pointers	
		for (int i=0;  i<nRows;  i++)
			m_pRows[i] = &m_rEntries[i * nCols];
    }


inline CReal2DArray& CReal2DArray::operator=(const CReal2DArray &array)
{
	ASSERT(&array != NULL);

	if (&array != this) {
		m_nRowCount = array.m_nRowCount;
		m_nColCount = array.m_nColCount;

		delete [] m_rEntries;
		delete [] m_pRows;

		int nEntryCount = m_nRowCount * m_nColCount;
		if (nEntryCount) {
			m_pRows = new double* [m_nRowCount];
			m_rEntries = new double [nEntryCount];

			for (register i = 0;  i < m_nRowCount;  i++)
				m_pRows[i] = &m_rEntries[i * m_nColCount];

			for (i = 0; i < nEntryCount; i++) 
				m_rEntries[i] = array.m_rEntries[i];
		}
		else {
			m_pRows = NULL;
			m_rEntries = NULL;
		}
	}
	return *this;
}

#endif

