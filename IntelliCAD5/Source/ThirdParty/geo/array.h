/*	Array.H - Defines the abstract class of arrays and the derived classes of real
 *  and object arrays
 *	Copyright (C) 1998 Visio Corporation. All rights reserved.
 *
 *
 * ABSTRACT
 * Abstract arrays know their size (i.e. number of entries), the storage space allocated - 
 * which may be more than that occupied - and the size of chunks for additional allocation.
 *
 * An object array has a list of object pointers, typically owning these objects. 
 * A real array has a list of real entries.  In addition to the standard array methods, 
 * it has methods for searching in ordered arrays, and inserting it the correct place.
 *
 */
#ifndef ARRAY_H
#define ARRAY_H

#ifndef GEOMETRY_H
#include "Geometry.h"
#endif

// DUMP macro, defined only for debug
#ifdef DEBUG
#define DUMP(Object) (Object).Dump()
class ostream;
#else
#define DUMP(Object)
#endif


/////////////////////////////////////////////////////////////////////////////
// Root of all classes - CGeoObject	  

class GEO_CLASS CGeoObject
{
public:
	CGeoObject();
	virtual ~CGeoObject();

#ifdef DEBUG
	virtual void Dump() const;
#endif // DEBUG
};

/////////////////////////////////////////////////////////////////////////////
// Generic Array

class GEO_CLASS CGeoArray	:    public CGeoObject
    {
// Construction & destruction
public:
	 CGeoArray();
	virtual  ~CGeoArray();

// Methods
public:
	int  Size() const;

	void SetGrowBy(
		int val)	//In: How much to grow by
		{
		m_nGrowBy = val;
		}

	int  Mod(
		int & nIndex) const;

// Data
protected:
	int m_nSize;
	int m_nAllocated;
	int m_nGrowBy;
    };

////////////////////////////////////////////////////////////////////////////////
//		Implementation of Inline Methods

// Current number of entries
inline int  CGeoArray::Size() const
    {
	return m_nSize;
    }

////////////////////////////////////////////////////////////////////////////////
//	Array of double
class GEO_CLASS CRealArray : public CGeoArray
    {
// Construction & destruction
public:
	CRealArray();

	CRealArray(
		int arraySize,		// In: The number of entries
		RC & rc);			// Out: Return code

	virtual  ~CRealArray();

// Attributes
public:
	double    operator [](int nIndex) const;    //The entry with a given index
	double &  operator [](int idnex);

	double &  Last();  // The last entry
                                       
// Operations
public:
	 CRealArray(
		const CRealArray & Other,  // In: The other array to copy
		RC & rc);				   // Out: Return code

	// Set up with given number of entries. NOTE the data will be lost
	// if the array is shrunk.
	//
	void SetSize(int newArraySize);  
   
	// Append the new entry to array
	//
	RC Add(double rEntry); 


	// Remove an entry from the array
	//
	int  Remove(int rAt);   

	// Remove entries from the array starting at a specified location
	//
	virtual int  Remove(int from, int numOfEntries); 


	// Insert a new entry a sorted array if it deosn't exist
	//
	RC  AddSort(double rEntry, double rTolerance, int & rAt); 
	RC  AddSort(double rEntry);      // Simple minded version of the above

	// Search a array for the location of a new entry. The function returns TRUE 
	// if the entry exists in the array within given tolerance and FLASE otherwise. 
	// NOTE: The third output parameter will returns either the last index i such as 
	// rEntry >= Entries[i] if the new entry is greater than one of entries or -1 
	// if the array is empty or the value of new entry is smaller than any one in 
	// the array.
	//
    BOOL  Search(double rEntry, double rTolearnce, int & nIndex) const; 


	// Returns the location for a new entry in the sorted array. The function will
	// return -1 if the array is empty or the new entry is smaller than any one
	// in array.
	//
    int  Search(double rEntry) const;       // Simple minded version of the above


	// Binary search within a range in a sorted array for the location of an entry
	//
	int BinarySearch(double, int,int nTop) const; //Low level search utility


	// Make room for new entries
	//
	RC MakeRoom(int rAt, int nNumOfNewEntries); 


	// Insert an entry to the specified location in the array. After inserted, the
	// array will not be a sorted array.
	//
	RC Insert(
		double  rEntry,  // In: The new entry to be inserted
		int   nAt);      // In: The index of the new entry

	// Copy a portion of another array    
	//
	RC Copy(const CRealArray &, int, int);    
	
	// Remove all entries from the array
	//
	void RemoveAll();  
	
	// Retrurn the last entry and remove it from the array
	//
	double Pop();

	// Sum up the entries
	//
	double Sum();

	// Revese the order of entries in the array
	//
	void Reverse();

	// Sort array in ascending numerial order
	//
	virtual void Sort();

	// The function returns true if the array is sorted in ascending order
	// and false otherqwise.
	//
    void SetSorted(bool isSorted) { m_bSorted = isSorted; }
	bool IsSorted() const { return m_bSorted; }

	virtual RC ReallocateToSize();

	CRealArray& operator=(const CRealArray &array);

#ifdef DEBUG
	void Dump() const;

	GEO_FRIEND ostream& operator<<(ostream& out, const CRealArray &array);
#endif

// Data
protected:
	double* m_rEntries;
	bool m_bSorted; 
    };

typedef CRealArray * PRealArray;

////////////////////////////////////////////////////////////////////////////////
//		Implementation of Inline Methods
//The entry with a given index, available for read and write
inline double  CRealArray::operator [](int nIndex) const
    {
	return m_rEntries[nIndex];
    }

inline double& CRealArray::operator [](int nIndex)
    {
	return m_rEntries[nIndex];
    }

//The last entry
inline double & CRealArray::Last()
    {
	return m_rEntries[m_nSize-1];
    }

// Simple minded version of AddSort
inline RC  CRealArray::AddSort(
	double rEntry)     // In: The new entry to be added
    {
	int n;
	return AddSort(rEntry, FUZZ_DIST, n);
    }

// Same as the other search, but with 0 tolerance                                                   		
inline int  CRealArray::Search(
	double rEntry) const     // In: The entry to be located
    {
	int n;
	Search(rEntry, 0, n);
	return n;
    }


inline double CRealArray::Pop()
    {
	return m_rEntries[--m_nSize];
    }
/////////////////////////////////////////////////////////////////////////////
// Object Array

class GEO_CLASS CGeoObjectArray : public CGeoArray
{
private:

    CGeoObjectArray(const CGeoObjectArray &array) { *this = array; }
    CGeoObjectArray& operator=(const CGeoObjectArray &array) { ASSERT(0); return *this; }

// Construction & destruction
public:
	CGeoObjectArray();
	virtual ~CGeoObjectArray();

// Attributes
public:
	PGeoObject operator [](int nIndex) const; //The object with a given index
	PGeoObject Last() const;  		 //The last entry
                                        
// Operations
public:
	PGeoObject Pop();
	virtual void Purge();	  // Purge the array, deleteing all entries
	virtual RC Add(PGeoObject);  // Add an object at the end of the list

	RC MakeRoom(int, int); // Make room for new entries

	virtual RC Insert(   // Insert an object at a given index in the list
		PGeoObject pObject, // In: The new object to be inserted
		int   nAt);      // In: The index of the new entry

	int  Remove(
		int   nAt);      // In: The index of the entry to remove
	
	int Remove(
		int nFrom,      // In: The first index of the entries to remove
		int nNumber);   // In: The number of entries to remove

	void RemoveAll();

	RC Copy(
		const CGeoObjectArray & Other, // In: The array to copy from
		int nFrom,        // In: The first index in the other array to Copy
		int nNumber);     // In: The number of entries to append

	void Reverse();

	void DeleteObject(
		int nIndex);   // The object be deleted

	virtual RC ReallocateToSize();

#ifdef DEBUG
	void Dump() const;

#endif // DEBUG

// Data	
protected:
	PGeoObject * m_pEntries;
};
////////////////////////////////////////////////////////////////////////////////
//		Implementation of Inline Methods

// Get the last entry in the array
inline PGeoObject CGeoObjectArray::Last() const
    {
	return m_pEntries[m_nSize-1];
    }

// Retrurn the last entry and remove it from the array
inline PGeoObject CGeoObjectArray::Pop()
    {
	return m_pEntries[--m_nSize];
    }

#endif

