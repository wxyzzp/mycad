/* C:\ICAD\PRJ\LIB\DB\TOLERANCE.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/************************** db_tolerance *****************************/

/*F*/
db_tolerance::db_tolerance(void) : db_entity_with_extrusion(DB_TOLERANCE) {

    primtext=NULL; dship=NULL;
    pt[0]=pt[1]=pt[2]=xdir[1]=xdir[2]=0.0; xdir[0]=1.0; textht=0.2;

    disp=NULL; memset(extent,0,sizeof(extent));
}
db_tolerance::db_tolerance(db_drawing *dp) : db_entity_with_extrusion(DB_TOLERANCE,dp) {
    primtext=NULL; pt[0]=pt[1]=pt[2]=xdir[1]=xdir[2]=0.0; xdir[0]=1.0;
    textht=0.2;
    dship=(dp!=NULL) ? dp->ret_currenttabrec(DB_QDIMSTYLE) : NULL;

    disp=NULL; memset(extent,0,sizeof(extent));
}
db_tolerance::db_tolerance(const db_tolerance &sour) :
    db_entity_with_extrusion(sour) {  /* Copy constructor */


    primtext=NULL; if (sour.primtext!=NULL) db_astrncpy(&primtext,sour.primtext,-1);

    DB_PTCPY(pt,sour.pt); DB_PTCPY(xdir,sour.xdir);

    dship=sour.dship; textht=sour.textht;

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
}
db_tolerance::~db_tolerance(void) {

    delete [] primtext; 

    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}

/*F*/
int db_tolerance::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) {
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    char *fcp1;
    int rc;
    struct resbuf *sublist[2];


    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    /* 3 (dimstyle) */
    fcp1=NULL;
    if (dship!=NULL) dship->get_2(&fcp1);
    if ((sublist[0]=sublist[1]=db_newrb(3,'S',
        (fcp1!=NULL) ? fcp1 : db_str_quotequote))==NULL) { rc=-1; goto out; }

    /* 10 (pt) */
    if ((sublist[1]->rbnext=db_newrb(10,'P',pt))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 1 (primtext) */
    if ((sublist[1]->rbnext=db_newrb(1,'S',
        (primtext!=NULL) ? primtext : db_str_quotequote))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	// Extrusion is now part of the base class, so get it from accessor
	//
	sds_point extru;
	get_210( extru );

    /* 210 (extru) */
    if ((sublist[1]->rbnext=db_newrb(210,'P',extru))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 11 (xdir) */
    if ((sublist[1]->rbnext=db_newrb(11,'P',xdir))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}



