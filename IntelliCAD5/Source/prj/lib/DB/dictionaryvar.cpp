/* C:\ICAD\PRJ\LIB\DB\DICTIONARYVAR.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#include "db.h"
#include "objects.h"

/***************************** db_dictionaryvar **************************/

db_dictionaryvar::db_dictionaryvar(void) : db_handitem(DB_DICTIONARYVAR) {
    deleted=0; unkint=0; unkstr=NULL;
}
db_dictionaryvar::db_dictionaryvar(const db_dictionaryvar &sour) :
    db_handitem(sour) {  /* Copy constructor */

    deleted=sour.deleted;
    unkint=sour.unkint;
    unkstr=NULL; db_astrncpy(&unkstr,sour.unkstr,-1);
}
db_dictionaryvar::~db_dictionaryvar(void) {
	delete [] unkstr;
}
