/* C:\ICAD\PRJ\LIB\DB\CLASS.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/***************************** db_class *****************************/

db_class::db_class(void) {
    memset(this,0,sizeof(*this));
}
db_class::db_class(
    int   pclassnum,
    char *pdxfrecname,
    char *pcppname,
    char *pappname,
    char *pdxf3,
    int   pvernum,
    char  pwasproxy,
    int   pitemclassid) {

    memset(this,0,sizeof(*this));
    set_data(
        pclassnum,
        pdxfrecname,
        pcppname,
        pappname,
        pdxf3,
        pvernum,
        pwasproxy,
        pitemclassid);
}
db_class::db_class(const db_class &sour) {  /* Copy constructor */
    next=NULL;
    classnum   =sour.classnum;
    vernum     =sour.vernum;
    wasproxy   =sour.wasproxy;
    itemclassid=sour.itemclassid;
    dxfrecname=NULL; if (sour.dxfrecname!=NULL) db_astrncpy(&dxfrecname,sour.dxfrecname,-1);
    cppname   =NULL; if (sour.cppname   !=NULL) db_astrncpy(&cppname   ,sour.cppname   ,-1);
    appname   =NULL; if (sour.appname   !=NULL) db_astrncpy(&appname   ,sour.appname   ,-1);
    dxf3      =NULL; if (sour.dxf3      !=NULL) db_astrncpy(&dxf3      ,sour.dxf3      ,-1);
}
db_class::~db_class(void) {
    delete [] dxfrecname; delete [] cppname; delete [] appname; delete [] dxf3;
}




