/* C:\ICAD\PRJ\LIB\DB\LINE.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/**************************** db_line *****************************/

/*F*/
db_line::db_line(void) : db_entity_with_extrusion(DB_LINE) {

    pt[0][0]=pt[0][1]=pt[0][2]=pt[1][1]=pt[1][2]=0.0; pt[1][0]=1.0;

    thick=0.0;
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_line::db_line(db_drawing *dp) : db_entity_with_extrusion(DB_LINE,dp) {
    char *bufp;

    bufp=(dp!=NULL) ? dp->ret_headerbuf() : NULL;

    pt[0][0]=pt[0][1]=pt[0][2]=pt[1][1]=pt[1][2]=0.0; pt[1][0]=1.0;

    db_qgetvar(DB_QTHICKNESS,bufp,&thick,DB_DOUBLE,0);

    disp=NULL; memset(extent,0,sizeof(extent));
}
db_line::db_line(const db_line &sour) :
    db_entity_with_extrusion(sour) {  /* Copy constructor */

    thick=sour.thick;

    DB_PTCPY(pt[0],sour.pt[0]);  DB_PTCPY(pt[1],sour.pt[1]);

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
}
db_line::~db_line(void) {

    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}

/*F*/
int db_line::entgetspecific(
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

    /* 39 (thick) */
    if ((sublist[0]=sublist[1]=db_newrb(39,'R',&thick))==NULL) { rc=-1; goto out; }

    /* 10 (pt[0]) */
    if ((sublist[1]->rbnext=db_newrb(10,'P',pt[0]))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 11 (pt[1]) */
    if ((sublist[1]->rbnext=db_newrb(11,'P',pt[1]))==NULL) { rc=-1; goto out; }
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

int db_line::check(db_drawing* pDrawing)
{
	if(!db_entity_with_extrusion::check(pDrawing))
		return 0;

	// DP: heuristic for particular problem
	if(fabs(pt[1][2] - pt[0][2]) > FLT_MAX)
		if(fabs(pt[1][2]) > fabs(pt[0][2]))
			pt[1][2] = pt[0][2];
		else
			pt[0][2] = pt[1][2];

	return 1;
}


