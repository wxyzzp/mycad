/* C:\ICAD\PRJ\LIB\DB\BODY.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/**************************** db_body *****************************/

    /* For now, 3DSOLID, BODY, AND REGION have identical but separate */
    /* class definitions and member functions (so that every entity */
    /* still has its own class). */

/*F*/
db_body::db_body(void) : db_entity(DB_BODY) {
    data=imdata=NULL; datasz=imdatasz=0; pt[0]=pt[1]=pt[2]=0.0;
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_body::db_body(db_drawing *dp) : db_entity(DB_BODY,dp) {
    data=imdata=NULL; datasz=imdatasz=0; pt[0]=pt[1]=pt[2]=0.0;
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_body::db_body(const db_body &sour) :
    db_entity(sour) {  /* Copy constructor */

    datasz=sour.datasz; imdatasz=sour.imdatasz;
    data=NULL;
    if (datasz>0 && sour.data!=NULL) {
        data=new char[datasz];
        memcpy(data,sour.data,datasz);
    }
    imdata=NULL;
    if (imdatasz>0 && sour.imdata!=NULL) {
        imdata=new char[imdatasz];
        memcpy(imdata,sour.imdata,imdatasz);
    }
    DB_PTCPY(pt,sour.pt);

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
}
db_body::~db_body(void) {

    delete [] data; delete [] imdata;

    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}



