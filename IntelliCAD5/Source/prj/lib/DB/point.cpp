/* C:\ICAD\PRJ\LIB\DB\POINT.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/************************** db_point *****************************/

/*F*/
db_point::db_point(void) : db_entity_with_extrusion(DB_POINT) {

    pt[0]=pt[1]=pt[2]=rot=0.0;

    thick=0.0; 
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_point::db_point(db_drawing *dp) : db_entity_with_extrusion(DB_POINT,dp) {

    char *bufp;

    bufp=(dp!=NULL) ? dp->ret_headerbuf() : NULL;

    pt[0]=pt[1]=pt[2]=rot=0.0;

    db_qgetvar(DB_QTHICKNESS,bufp,&thick,DB_DOUBLE,0);

    disp=NULL; memset(extent,0,sizeof(extent));
}
db_point::db_point(const db_point &sour) :
    db_entity_with_extrusion(sour) {  /* Copy constructor */

    thick=sour.thick;

    DB_PTCPY(pt,sour.pt); rot=sour.rot;

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
}
db_point::~db_point(void) {


    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}

/*F*/
int db_point::entgetspecific(
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

    /* 39 (thick) */
    if ((sublist[1]->rbnext=db_newrb(39,'R',&thick))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	// Extrusion is now part of the base class, so get it from accessor
	//
	sds_point extru;
	get_210( extru );

    /* 210 (extru) */
    if ((sublist[1]->rbnext=db_newrb(210,'P',extru))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 50 (rot) */
    if ((sublist[1]->rbnext=db_newrb(50,'R',&rot))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}



