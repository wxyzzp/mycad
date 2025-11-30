// File  : \source\prj\lib\LISP\bindtable.cpp
// Author: Mohit Mehta
// Reason: Bugzilla No: 78052 and Code optimized.
# include "bindtable.h"

CBindStructure::~CBindStructure()
{
	CRefCount * value;
	POSITION pos;
	CString key;
	for(pos=bindMap.GetStartPosition();pos!=NULL; )
	{
		bindMap.GetNextAssoc(pos,key,(void*&)value);
		if(value)
			value->decRefCnt();
	}
	bindMap.RemoveAll ();
}

bool CBindStructure::bindAtoms(CString atom1,CString atom2)
{
	if(isBinded(atom1,atom2))				// If they are already binded then do nothing		
		return true;
	CRefCount *value1=NULL, *value2=NULL;
	bool exists1 = checkAtom(atom1,value1);
	bool exists2 = checkAtom(atom2,value2);
	if(exists1 && exists2)					// Both exist 
	{	
		if(value1)
			value1->decRefCnt();
		bindMap.SetAt(atom1,value2);		// Bind atom2 to atom1
		value2->incRefCnt();				// Increment the reference count
		return true;
	}
	if(!exists1 && !exists2)				// Neither of the two atoms exist
	{	
		value2 = new CRefCount;
		bindMap.SetAt(atom2,value2);		// Put atom2 in map
		bindMap.SetAt(atom1,value2);		// Put atom1 in map
		value2->incRefCnt();				// Increment the reference count
		return true;
	}
	if(!exists2)							// Atom2 is not there
	{
		value2 = new CRefCount;
		bindMap.SetAt(atom2,value2);		// Put atom2 in map
		bindMap.SetAt(atom1,value2);		// Bind atom2 to atom1
		value2->incRefCnt();				// Increment the reference count
		if(value1)
			value1->decRefCnt();			// Decrement the reference count for atom1
		return true;
	}
	// Atom1 is not there
	bindMap.SetAt(atom1,value2);			// Put atom1 in map and bind it to atom2
	value2->incRefCnt();					// Increment the reference count
	return true;
}

bool CBindStructure::checkAtom(CString atom,CRefCount *&value)
{
	if(bindMap.Lookup(atom,(void*&)value))	
		return true;						// Found
	else	
		return false;						// Not found
}

bool CBindStructure::removeAtom(CString atom)
{
	CRefCount * value=NULL;
	if(bindMap.Lookup(atom,(void*&)value))  // Find if the atom is there in the map
	{
		if(value)
			value->decRefCnt();				// Decrement the reference count for this atom
		if(!bindMap.RemoveKey(atom))
			return false;					// If removekey fails to remove the key return false 
	}
	return true;							// If key is not there in the map or has been removed return true
}

bool CBindStructure::isBinded(CString atom1,CString atom2)
{
	CRefCount *value1=NULL, *value2=NULL;
	// Check if both the atoms exist in the map
	if(bindMap.Lookup(atom1,(void*&)value1) && bindMap.Lookup(atom2,(void*&)value2))  
	{
		if(value1 == value2) // Check if they are pointing to the same RefCount object 
			return true;
	} 
	// Either atom1 or atom2 or both are not in the map. 
	// Else both exist but are not pointing to same RefCount object 	
	return false;			 
}




