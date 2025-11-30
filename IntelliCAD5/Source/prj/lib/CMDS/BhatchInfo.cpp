/* G:\ICADDEV\PRJ\LIB\CMDS\BHATCHINFO.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "cmds.h"
#include "Bhatchinfo.h"

// constructor
stdObjHandleRecordList::stdObjHandleRecordList()
	{
	}

// destructor
stdObjHandleRecordList::~stdObjHandleRecordList()
	{
	for (const_iterator i = begin(); i != end(); i++)
		{   // begin i-loop
		delete (*i);
		}   //   end i-loop
	}

// accessor


void stdObjHandleRecordList::get_HandleList
                           (int regionIndex, // In: index of region
			                int pathIndex, // In: index of path
							bool & found, // Out: true if we have found the list
							stdObjHandleList & list) // if found is true, this is the list
	{
	found = false;
    for (const_iterator k = begin(); k!=end(); k++)
		{
		if ( ((*k)->i_Region == regionIndex) && ((*k)->j_Path == pathIndex) )
			{
			found = true;
			list = (*k)->HandleList;
			}
		}
	}


