/* G:\ICADDEV\PRJ\LIB\CMDS\BHATCHINFO.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#ifndef BHATCHINFO_H
#define BHATCHINFO_H

#include "db.h"      // for DWORD
#include "objhandle.h" // for db_objhandle
#include <list>        // for std::list

typedef std::list<db_objhandle> stdObjHandleList;

class ObjHandleRecord
{
public:
	int i_Region;
	int j_Path;
	stdObjHandleList HandleList;
};

// typedef std::list<record *> stdRecordList; (previous implementation)

class stdObjHandleRecordList : public std::list<ObjHandleRecord *>
	{  // begin definition of stdObjHandleRecordList
	public:
		// constructor
		stdObjHandleRecordList();

		// destructor
		~stdObjHandleRecordList();

		// accessor
		void get_HandleList(int regionIndex, // In: index of region
			                int pathIndex, // In: index of path
							bool & found, // Out: true if we have found the list
							stdObjHandleList & list); // if found is true, this is the list

		
	}; //   end definition of stdObjHandleRecordList




#endif

