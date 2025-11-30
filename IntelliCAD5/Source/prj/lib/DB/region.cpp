/* C:\ICAD\PRJ\LIB\DB\REGION.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"


/**************************** db_region *****************************/

    /* For now, 3DSOLID, BODY, AND REGION have identical but separate */
    /* class definitions and member functions (so that every entity */
    /* still has its own class). */

/*F*/
db_region::db_region(void) : db_entity(DB_REGION) {
    data=imdata=NULL; datasz=imdatasz=0; pt[0]=pt[1]=pt[2]=0.0;
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_region::db_region(db_drawing *dp) : db_entity(DB_REGION,dp) {
    data=imdata=NULL; datasz=imdatasz=0; pt[0]=pt[1]=pt[2]=0.0;
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_region::db_region(const db_region &sour) :
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
db_region::~db_region(void) {

    delete [] data; delete [] imdata;

    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}



