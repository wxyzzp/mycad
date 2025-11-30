/* G:\ICADDEV\PRJ\LIB\DB\DBDLL.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Private definitions within DB.DLL
 * 
 */ 

#ifndef	_DBDLL_H
#define	_DBDLL_H

#include "dbrc.h"

#ifndef RCID
    #ifdef RC_INVOKED
	#define RCID(id) id
	#else
	#define RCID(id) MAKEINTRESOURCE(id)
	#endif
#endif

	// binary data file inclusion... copied from Visio
#define	RT_BINDATA	RCID( 1000)

#ifndef	RC_INVOKED
#ifndef	STRICT
#define	STRICT
#endif
extern	HINSTANCE	ModuleInstance;
#endif

#endif	// _DBDLL_H

