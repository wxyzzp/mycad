/* C:\ICAD\PRJ\LIB\DB\OBJECT_PTR.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"
#include "objects.h"

#include "OdaCommon.h"
#include "..\..\..\Licensed\DWGdirect\Include\ResBuf.h" // clash with icadlib\resbuf.h
#include "DbProxyEntity.h"

/***************************** db_object_ptr *****************************/

db_object_ptr::db_object_ptr(void) : db_handitem(DB_OBJECT_PTR) 
{
    deleted=0;
	m_pResbufChain = NULL;
	m_OdClass  = NULL;
}

db_object_ptr::db_object_ptr(const db_object_ptr &sour) :
    db_handitem(sour) 
{  /* Copy constructor */

	m_pResbufChain = NULL;
	m_OdClass  = NULL;

    deleted=sour.deleted;
}

db_object_ptr::~db_object_ptr(void) 
{ 
	sds_relrb(m_pResbufChain);
	OdDbObjectPtr pClass((OdDbObject*)m_OdClass, kOdRxObjAttach);
}


