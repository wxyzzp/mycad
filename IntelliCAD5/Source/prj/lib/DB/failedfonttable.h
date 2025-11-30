/* D:\ICADDEV\PRJ\LIB\DB\FAILEDFONTTABLE.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#ifndef _INC_FAILEDFONTTABLE
#define _INC_FAILEDFONTTABLE
#ifndef	_DB_H
#include "db.h"
#endif

#ifndef	_IC_DYNARRAY_H
#include "ic_dynarray.h"
#endif


struct FailedFontEntry
	{
					FailedFontEntry(void);
					FailedFontEntry( void **theptr);
					FailedFontEntry( db_objhandle handle, void **theptr=NULL);
	db_objhandle	handle;
	void			**failedfontloc;
	};


typedef ic_dynarray<FailedFontEntry>	FailedFontArray;

class FailedFontTable 
	{
public:
						FailedFontTable();
						~FailedFontTable();
	void				Add( db_objhandle handle, void **theptr);
	bool				Get(unsigned int nItem, FailedFontEntry *theentry);
private:
	FailedFontArray			m_table;
	};

#endif

