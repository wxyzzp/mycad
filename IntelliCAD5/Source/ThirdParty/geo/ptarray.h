/* PtArray.h - Defines the class of array points
 *	Copyright (C) 1994-1997 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 * The array knows its size (i.e. number of entries), the storage space allocated - 
 * which may be more than that occupied - and the size of chunks for additional allocation.
 *
 */
#ifndef PTARRAY_H
#define PTARRAY_H

#include "Array.h"
#include "Point.h"


#ifdef DEBUG
class ostream;
#endif // DEBUG

class  GEO_CLASS CPointArray : public CGeoArray
{
// Construction & destruction
public:
	CPointArray();

	CPointArray(
		const CPointArray & Other,  //In: The other array to copy
		RC & rc);					// Out: Return code

	CPointArray(
		LPBYTE & pStorage,  // In/Out: The storage
		RC & rc);			// Out: Return code

	CPointArray(
		int nSize,    //In: The number of points
		RC & rc);	  // Out: Return code

	virtual ~CPointArray();

// Attributes
public:
	C3Point& operator [](int) const; //The point with a given index

	C3Point& First() const; // The first point
	C3Point& Last() const;	//The last point
                                       
// Operations
public:
	void Set(const int, const C3Point&); // Set a point
	void SetX(const int, const double);          // Set the x of a point
	void SetY(const int, const double);          // Set the y of a point
	RC Add(const C3Point&);          // Add an entry at the end of the array
	RC MakeRoom(int,int);   // Make room for new entries
	RC Insert(C3Point&,int);   // Insert a new entry
	RC Copy(const CPointArray&, int, int); // Copy from another array
	void Remove(int, int);
	void RemoveTail(int);
	void RemoveAll(); // Remove all entries
	C3Point Pop();
	RC Pack(LPBYTE & pStorage) const;  // In/Out: The storage
	int PackingSize() const;
	virtual RC ReallocateToSize();
	RC ReverseOrder();
	RC setSize(int newSize);

	CPointArray& operator=(const CPointArray &ptArray);

#ifdef DEBUG
	void Dump();

	GEO_FRIEND ostream& operator<<(ostream &out, const CPointArray &ptArray);
#endif // DEBUG

// Data
protected:
	P3Point m_ptEntries;
};
////////////////////////////////////////////////////////////////////////////////
//		Implementation of Inline Methods
// Set the point with given index to given value
inline void CPointArray::Set(const int nIndex, const C3Point& ptPoint)
    {
	(*this)[nIndex] = ptPoint;
    }
/******************************************************************************/
// Set the x of the point with given index to given value
inline void CPointArray::SetX(const int nIndex, const double rT)
    {
	(*this)[nIndex].SetX(rT);
    }
// Set the x of the point with given index to given value
inline void CPointArray::SetY(const int nIndex, const double rT)
    {
	(*this)[nIndex].SetY(rT);
    }
/******************************************************************************/
//The point with a given index
inline C3Point& CPointArray::operator [](int nIndex) const
    {
	return m_ptEntries[nIndex];
    }
/******************************************************************************/
//The first point
inline C3Point& CPointArray::First() const
    {
	return m_ptEntries[0];
    }
/******************************************************************************/
//The last point
inline C3Point& CPointArray::Last() const
    {
	return m_ptEntries[m_nSize-1];
    }
/******************************************************************************/
// Return the last entry and remove it from the array
inline C3Point CPointArray::Pop()
    {
	return m_ptEntries[--m_nSize];
    }
#endif
