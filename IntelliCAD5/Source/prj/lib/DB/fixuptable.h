/* D:\ICADDEV\PRJ\LIB\DB\FIXUPTABLE.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#ifndef _INC_FIXUPTABLE
#define _INC_FIXUPTABLE
#ifndef	_DB_H
#include "db.h"
#endif

#ifndef	_IC_DYNARRAY_H
#include "ic_dynarray.h"
#endif



struct FixupEntry
	{
					FixupEntry(void);
					FixupEntry( void **theptr);
					FixupEntry( db_objhandle handle, void **theptr=NULL);
	db_objhandle	handle;
	void			**fixuploc;
	};


typedef	ic_dynarray< FixupEntry>	FixupArray;

class FixupTable 
	{
public:
						FixupTable();
	void				Add( db_objhandle handle, void **theptr);
	bool				Get(unsigned int nItem, FixupEntry *theentry);
	bool				ApplyFixups(db_drawing *dp);
private:
	FixupArray			m_table;
	};

#endif

