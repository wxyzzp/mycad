/* C:\ICAD\PRJ\LIB\DB\VXTABREC.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"
#include "vxtabrec.h"

/************************** db_vxtabrec **************************/

/*F*/
db_vxtabrec::db_vxtabrec(void) :
    db_tablerecord(DB_VXTABREC,db_str_quotequote) {

    vpehip=NULL; flag=0;
}
db_vxtabrec::db_vxtabrec(char *pname, db_drawing *dp) :
    db_tablerecord(DB_VXTABREC,pname) {

    vpehip=NULL; flag=0;
}
db_vxtabrec::db_vxtabrec(const db_vxtabrec &sour) :
    db_tablerecord(sour) {  /* Copy constructor */

    vpehip=sour.vpehip; flag=sour.flag;
}



