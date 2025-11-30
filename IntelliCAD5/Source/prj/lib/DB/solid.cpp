/* C:\ICAD\PRJ\LIB\DB\SOLID.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/************************** db_solid *****************************/

/*F*/
db_solid::db_solid(void) : db_entity_with_extrusion(DB_SOLID) {

    memset(pt,0,sizeof(pt));

    thick=0.0; 
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_solid::db_solid(db_drawing *dp) : db_entity_with_extrusion(DB_SOLID,dp) {
    char *bufp;

    bufp=(dp!=NULL) ? dp->ret_headerbuf() : NULL;

    memset(pt,0,sizeof(pt));

    db_qgetvar(DB_QTHICKNESS,bufp,&thick,DB_DOUBLE,0);

    disp=NULL; memset(extent,0,sizeof(extent));
}
db_solid::db_solid(const db_solid &sour) :
    db_entity_with_extrusion(sour) {  /* Copy constructor */

    thick=sour.thick;

    DB_PTCPY(pt[0],sour.pt[0]); DB_PTCPY(pt[1],sour.pt[1]);
    DB_PTCPY(pt[2],sour.pt[2]); DB_PTCPY(pt[3],sour.pt[3]);

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
}
db_solid::~db_solid(void) {

    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}

/*F*/
int db_solid::entgetspecific(
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

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}



