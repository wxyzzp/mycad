/* C:\ICAD\PRJ\LIB\DB\3DFACE.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 


#include "db.h"

/**************************** db_3dface *****************************/

/*F*/
db_3dface::db_3dface(void) : db_entity(DB_3DFACE) {
    memset(pt,0,sizeof(pt)); flags=0;
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_3dface::db_3dface(db_drawing *dp) : db_entity(DB_3DFACE,dp) {
    memset(pt,0,sizeof(pt)); flags=0;
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_3dface::db_3dface(const db_3dface &sour) :
    db_entity(sour) {  /* Copy constructor */

    DB_PTCPY(pt[0],sour.pt[0]); DB_PTCPY(pt[1],sour.pt[1]);
    DB_PTCPY(pt[2],sour.pt[2]); DB_PTCPY(pt[3],sour.pt[3]);
    flags=sour.flags;

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
}
db_3dface::~db_3dface(void) {

    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}

/*F*/
int db_3dface::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) {
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    short fsh1;
    int rc;
    struct resbuf *sublist[2];


    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    /* 10 */
    if ((sublist[0]=sublist[1]=db_newrb(10,'P',pt[0]))==NULL) { rc=-1; goto out; }

    /* 11 */
    if ((sublist[1]->rbnext=db_newrb(11,'P',pt[1]))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 12 */
    if ((sublist[1]->rbnext=db_newrb(12,'P',pt[2]))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 13 */
    if ((sublist[1]->rbnext=db_newrb(13,'P',pt[3]))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 70 (flags) */
    fsh1=(short)flags;
    if ((sublist[1]->rbnext=db_newrb(70,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}


