/* C:\ICAD\PRJ\LIB\DB\XLINE.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/**************************** db_xline *****************************/

/*F*/
db_xline::db_xline(void) : db_entity(DB_XLINE) {

    pt[0]=pt[1]=pt[2]=vect[1]=vect[2]=0.0; vect[0]=1.0;
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_xline::db_xline(db_drawing *dp) : db_entity(DB_XLINE,dp) {

    pt[0]=pt[1]=pt[2]=vect[1]=vect[2]=0.0; vect[0]=1.0;
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_xline::db_xline(const db_xline &sour) :
    db_entity(sour) {  /* Copy constructor */

    DB_PTCPY(pt,sour.pt); DB_PTCPY(vect,sour.vect);

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
}
db_xline::~db_xline(void) {

    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}

/*F*/
int db_xline::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) {
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    int rc;
    struct resbuf *sublist[2];


    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    /* 10 (pt) */
    if ((sublist[0]=sublist[1]=db_newrb(10,'P',pt))==NULL) { rc=-1; goto out; }

    /* 11 (vect) */
    if ((sublist[1]->rbnext=db_newrb(11,'P',vect))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}



