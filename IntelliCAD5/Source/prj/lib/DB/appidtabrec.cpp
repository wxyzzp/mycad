/* C:\ICAD\PRJ\LIB\DB\APPIDTABREC.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

/************************** db_appidtabrec **************************/

#include "db.h"
#include "appid.h"

/*F*/
db_appidtabrec::db_appidtabrec(void) :
    db_tablerecord(DB_APPIDTABREC,db_str_quotequote) {

    xrefidx=0;
}
db_appidtabrec::db_appidtabrec(char *pname, db_drawing *dp) :
    db_tablerecord(DB_APPIDTABREC,pname) {

    xrefidx=0;
}
db_appidtabrec::db_appidtabrec(const db_appidtabrec &sour) :
    db_tablerecord(sour) {  /* Copy constructor */

    xrefidx=sour.xrefidx;
}


