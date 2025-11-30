/* D:\ICADDEV\PRJ\LIB\DB\HANDLETABLE.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#ifndef _INC_HANDLETABLE
#define _INC_HANDLETABLE
#ifndef	_DB_H
#include "db.h"
#endif

//#ifndef	_IC_DYNARRAY_H
//#include "ic_dynarray.h"
//#endif

/*
struct HandleEntry
	{
					HandleEntry( db_handitem *entity);
					HandleEntry( const HandleEntry&);
					HandleEntry( const db_objhandle&, db_handitem *entity=NULL);
	db_objhandle	handle;
	db_handitem		*entity;
	};


typedef	ic_dynarray< HandleEntry>	HandleArray;
*/

#include <MAP>
using namespace std;
typedef map<__int64 , db_handitem *> Handle_MAP;
typedef Handle_MAP::value_type Handle_MAP_val;


class HandleTable 
	{
public:
	HandleTable() {;}
	void				Add( db_handitem* entity);
	db_handitem*		Find( const db_objhandle handle);
	~HandleTable() {m_map.clear();}

private:
	//bool				m_sorted;
	//HandleArray			m_table;
	Handle_MAP			m_map;
	};

#endif

