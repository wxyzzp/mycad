/* D:\ICADDEV\PRJ\LIB\DB\FIXUPTABLE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Maintains a table of fixup locations to be set using handent() at the
 * end of a DWG load.
 * 
 */ 

#include "db.h"
#include "fixuptable.h"
#include <stdlib.h>


	// ************************************
	//	FixupEntry gives a location for a fixup to be placed for a given objhandle


FixupEntry:: FixupEntry(void)
	{
	handle.Nullify();
	fixuploc=NULL;
	}

FixupEntry:: FixupEntry( db_objhandle _handle, void **theptr)
						: fixuploc( theptr)
	{
	handle=_handle;
	}



	// ************************************
	// FixupTable adds entries to a dynamic array
	// Set granularity to expand by about 64K
	// When searching, the table is first sorted if necessary

const int	cluster = 0x10000;	// 64K
FixupTable:: FixupTable() : m_table( cluster/sizeof( FixupEntry))
	{
	}


void
FixupTable:: Add( db_objhandle handle, void **theptr)
	{
	FixupEntry entry(handle,theptr);
	m_table.Add( entry);
	}


bool
FixupTable:: Get( unsigned int nItem, FixupEntry *theentry)
	{
	return m_table.Get( nItem, theentry);
	}

bool FixupTable:: ApplyFixups(db_drawing *dp)
	{
	unsigned int i,num;
	FixupEntry entry;
	db_handitem *hip;

	num=m_table.Number();
	for (i=0; i<num; i++)
		{
		Get(i,&entry);
		hip=dp->handent(entry.handle);
		*(entry.fixuploc)=hip;
		}
	return true;
	}


