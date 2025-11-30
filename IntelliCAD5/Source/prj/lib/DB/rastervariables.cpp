/* C:\ICAD\PRJ\LIB\DB\RASTERVARIABLES.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"
#include "objects.h"

/***************************** db_rastervariables *****************************/

db_rastervariables::db_rastervariables(void) : db_handitem(DB_RASTERVARIABLES) {
    deleted=0;
    dxf70=dxf71=dxf72=dxf90=0;
}
db_rastervariables::db_rastervariables(const db_rastervariables &sour) :
    db_handitem(sour) {  /* Copy constructor */

    deleted=sour.deleted;
    dxf70=sour.dxf70; dxf71=sour.dxf71; dxf72=sour.dxf72; dxf90=sour.dxf90;
}


