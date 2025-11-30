/* D:\ICADDEV\PRJ\LIB\DB\FAILEDFONTTABLE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * List of fonts that we have already failed to load.
 *
 * 
 */ 

#include "db.h"
#include "failedfonttable.h"
#include <stdlib.h>


	// ************************************
	//	FailedFontEntry gives a location for a FailedFont to be placed for a given objhandle


FailedFontEntry:: FailedFontEntry(void)
	{
	handle.Nullify();
	failedfontloc=NULL;
	}

FailedFontEntry:: FailedFontEntry( db_objhandle _handle, void **theptr)
						: failedfontloc( theptr)
	{
	handle=_handle;
	}



	// ************************************
	// FailedFontTable adds entries to a dynamic array
	// Set granularity to expand by about 64K
	// When searching, the table is first sorted if necessary

const int	cluster = 0x10000;	// 64K
FailedFontTable:: FailedFontTable() : m_table( cluster/sizeof( FailedFontEntry))
	{
	}


FailedFontTable::~FailedFontTable()
	{
	}

void
FailedFontTable:: Add( db_objhandle handle, void **theptr)
	{
	FailedFontEntry entry(handle,theptr);
	m_table.Add( entry);
	}


bool
FailedFontTable:: Get( unsigned int nItem, FailedFontEntry *theentry)
	{
	return m_table.Get( nItem, theentry);
	}



