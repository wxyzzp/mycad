/* C:\ICAD\PRJ\LIB\DB\CIRCLE.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/************************** db_circle *****************************/

/*F*/
db_circle::db_circle(void) : db_entity_with_extrusion(DB_CIRCLE) {
    thick=cent[0]=cent[1]=cent[2]=0.0; rad=1.0;
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_circle::db_circle(db_drawing *dp) : db_entity_with_extrusion(DB_CIRCLE,dp) {
    char *bufp;

    bufp=(dp!=NULL) ? dp->ret_headerbuf() : NULL;

    db_qgetvar(DB_QTHICKNESS,bufp,&thick,DB_DOUBLE,0);
    cent[0]=cent[1]=cent[2]=0.0; rad=1.0;

    disp=NULL; memset(extent,0,sizeof(extent));
}
db_circle::db_circle(const db_circle &sour) :
    db_entity_with_extrusion(sour) {  /* Copy constructor */

    thick=sour.thick;

    DB_PTCPY(cent,sour.cent); rad=sour.rad;

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
}
db_circle::~db_circle(void) {

    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}

/*F*/
int db_circle::entgetspecific(
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

    /* 39 (trace) */
    if ((sublist[0]=sublist[1]=db_newrb(39,'R',&thick))==NULL) { rc=-1; goto out; }

    /* 10 (cent) */
    if ((sublist[1]->rbnext=db_newrb(10,'P',cent))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 40 (rad) */
    if ((sublist[1]->rbnext=db_newrb(40,'R',&rad))==NULL) { rc=-1; goto out; }
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



