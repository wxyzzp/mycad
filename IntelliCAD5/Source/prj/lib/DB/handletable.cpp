/* D:\ICADDEV\PRJ\LIB\DB\HANDLETABLE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Maintains a table of handles for looking up db_handitem.
 * Since lookup is infrequent, the table is sorted lazily for searching.
 * 
 */ 

#include "db.h"
#include "handletable.h"
#include <stdlib.h>

/*
	// ************************************
	//	HandleEntry maintains the association between handles and entities
	//	Can be constructed from an entity (adding) or char formatted handle (searching)

	//	Entries are sorted by qsort, so must not have object semantics prohibited by that

HandleEntry:: HandleEntry( db_handitem *_entity)
						: entity( _entity)
	{
	*this = HandleEntry( _entity->RetHandle(), entity);
	}


HandleEntry:: HandleEntry( const HandleEntry& other)
						: entity( other.entity)
	{
	handle=other.handle;
	}


HandleEntry:: HandleEntry( const db_objhandle& _handle, db_handitem *_entity)
						: entity( _entity)
	{
	handle=_handle;
	}
*/


	// ************************************
	// HandleTable adds entries to a dynamic array
	// Set granularity to expand by about 64K
	// When searching, the table is first sorted if necessary

//const int	cluster = 0x10000;	// 64K
//HandleTable:: HandleTable() : m_sorted( false), m_table( cluster/sizeof( HandleEntry))
//	{
//	}


void
HandleTable:: Add( db_handitem* entity)
	{
//	m_table.Add( entity);
//	m_sorted = false;
	db_objhandle currHandle;
	__int64 handleVal = entity->RetHandle().ToHugeInt();
    m_map.insert(Handle_MAP_val(handleVal,entity));

	}


	// Comaparison routine for two HandleEntry structures
	// used by qsort and bsearch
/*
static int	__cdecl
compare( const void *entry1, const void *entry2)
	{
	HandleEntry	*key1 = (HandleEntry *)entry1;
	HandleEntry	*key2 = (HandleEntry *)entry2;

	return ObjhandleCompare(&(key1->handle),&(key2->handle));
	}
*/

db_handitem*
HandleTable:: Find( const db_objhandle handle)
	{
/*								// first make sure table is sorted
	if ( !m_sorted )
		{
		qsort( m_table.GetArray(), m_table.Number(), sizeof( HandleEntry), compare);
		m_sorted = true;
		}

								// locate entry by binary search
	HandleEntry		key( handle);
	HandleEntry	*found = (HandleEntry *)bsearch( 
								&key, 
								m_table.GetArray(), 
								m_table.Number(), 
								sizeof( HandleEntry), 
								compare
								);
*/
	__int64 handleVal;
	handleVal = ((db_objhandle) handle).ToHugeInt();
	Handle_MAP::iterator iter;
	iter = m_map.find(handleVal);
	db_handitem *mapEntity;
	// watch out for pointers in the handle table that are no longer valid
	if (iter != m_map.end())
	{
		mapEntity = (*iter).second;
		if (CHECKSTRUCTPTR( mapEntity ) )
			return mapEntity;
	}
	return NULL;
	}



