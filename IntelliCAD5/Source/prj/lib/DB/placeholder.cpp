/* C:\ICAD\PRJ\LIB\DB\PLACEHOLDER.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#include "db.h"
#include "objects.h"

/***************************** db_placeholder *****************************/

db_placeholder::db_placeholder(void) : db_handitem(DB_PLACEHOLDER) {
	deleted=0;
	//unused=0; // 2002/9/5 Ebatech(cnbr) No more need.
	ownerItem = 0;	// 2002/9/5 Ebatech(cnbr) for marcomp
}
db_placeholder::db_placeholder(const db_placeholder &sour) :
	db_handitem(sour) {  /* Copy constructor */

	deleted=sour.deleted;
	//unused=sour.unused;  // 2002/9/5 Ebatech(cnbr) No more need.
	ownerItem = sour.ownerItem;  // 2002/9/5 Ebatech(cnbr) for marcomp
}
db_placeholder::~db_placeholder(void) {  }
