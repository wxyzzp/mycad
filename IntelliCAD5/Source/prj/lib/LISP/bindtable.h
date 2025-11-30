// File  : \source\prj\lib\LISP\bindtable.h
// Author: Mohit Mehta
// Reason: Bugzilla No: 78052 and Code optimized.
#include "afxtempl.h"

//  Forward declaration
class CRefCount;

//  CBindStructure class contains a map to maintain all the atoms 
//  that are binded to some other atom
    
class CBindStructure
{
private:
	// Map in which all the atoms are put for binding
	CMapStringToPtr bindMap;
 
public:
	// Destructor to release the map and the RefCount objects
	~CBindStructure();
	// Binds two atoms and stores them in the map
	bool bindAtoms(CString atom1,CString atom2);
	// Check if given atom exists in the map
	bool checkAtom(CString atom1,CRefCount *&value);
	// Removes an atom from the map
	bool removeAtom(CString atom);
	// Checks if the two atoms are binded
	bool isBinded(CString atom1,CString atom2);
};

//  CRefCount class maintains the binding of objects in the map 
//  by keeping a count of the atoms referring the same object
   
class CRefCount
{
private:
	// Counter to maintain the reference count
	long m_refcount;
public:
	// Constructor which initializes the reference count to 1
	CRefCount(){ m_refcount = 1; }
	// Returns the reference count for an object
	long getRefCnt(){ return m_refcount;}
	// Increments the reference count for an object
	void incRefCnt(){ ++m_refcount; }
	// Decrements the reference count for an object 
	// and removes the RefCount object if count becomes zero  
	void decRefCnt()
	{
		if(!(--m_refcount))
			delete this;
	}
};
