/* C:\ICAD\PRJ\LIB\DB\WIPEOUTVARIABLES.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#include "db.h"
#include "objects.h"

/***************************** db_wipeoutvariables *****************************/

db_wipeoutvariables::db_wipeoutvariables(void) : db_handitem(DB_WIPEOUTVARIABLES) {
	deleted=0;	// Ebatech(cnbr) 2002/10/10
	dxf70=0;
}
db_wipeoutvariables::db_wipeoutvariables(const db_wipeoutvariables &sour) :
	db_handitem(sour) {  /* Copy constructor */
	deleted=sour.deleted;	// Ebatech(cnbr) 2002/10/10
	dxf70=sour.dxf70;

}
db_wipeoutvariables::~db_wipeoutvariables(void) {  }



