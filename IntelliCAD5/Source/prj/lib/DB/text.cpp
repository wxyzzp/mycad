/* C:\ICAD\PRJ\LIB\DB\TEXT.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/************************** db_text *****************************/

/*F*/
db_text::db_text(void) : db_entity_with_extrusion(DB_TEXT) {

    val=NULL; tship=NULL;
    ipt[0]=ipt[1]=ipt[2]=apt[0]=apt[1]=apt[2]=0.0;
    thick=rot=obl=0.0; xscl=1.0;
    db_qgetvar(DB_QTEXTSIZE,NULL,&ht,DB_DOUBLE,0);
    gen=hjust=vjust=0; 
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_text::db_text(db_drawing *dp) : db_entity_with_extrusion(DB_TEXT,dp) {
    char *bufp;

    bufp=(dp!=NULL) ? dp->ret_headerbuf() : NULL;

    val=NULL;
    tship=(dp!=NULL) ? dp->ret_currenttabrec(DB_QTEXTSTYLE) : NULL;
    ipt[0]=ipt[1]=ipt[2]=apt[0]=apt[1]=apt[2]=0.0;
    rot=obl=0.0; xscl=1.0;

	thick = 0.0;
   
    db_qgetvar(DB_QTEXTSIZE ,bufp,&ht,DB_DOUBLE,0);
    
    gen=hjust=vjust=0;

    disp=NULL; memset(extent,0,sizeof(extent));
}
db_text::db_text(const db_text &sour) :
    db_entity_with_extrusion(sour) {  /* Copy constructor */

    thick=sour.thick;

    val=NULL; if (sour.val!=NULL) db_astrncpy(&val,sour.val,-1);

    tship=sour.tship;
    DB_PTCPY(ipt,sour.ipt);
    DB_PTCPY(apt,sour.apt);
    ht   =sour.ht;
    xscl =sour.xscl;
    rot  =sour.rot;
    obl  =sour.obl;
    gen  =sour.gen;
    hjust=sour.hjust;
    vjust=sour.vjust;

    disp=NULL;  /* Don't copy display objects. */
    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
}
db_text::~db_text(void) {

    delete [] val; 

    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}

/*F*/
int db_text::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) {
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    char *fcp1;
    short fsh1;
    int rc;
    struct resbuf *sublist[2];


    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    /* 39 (thick) */
    if ((sublist[0]=sublist[1]=db_newrb(39,'R',&thick))==NULL) { rc=-1; goto out; }

    /* 10 (ipt) */
    if ((sublist[1]->rbnext=db_newrb(10,'P',ipt))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 40 (ht) */
    if ((sublist[1]->rbnext=db_newrb(40,'R',&ht))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 1 (val) */
    if ((sublist[1]->rbnext=db_newrb(1,'S',
        (val!=NULL) ? val : db_str_quotequote))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 50 (rot) */
    if ((sublist[1]->rbnext=db_newrb(50,'R',&rot))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 41 (xscl) */
    if ((sublist[1]->rbnext=db_newrb(41,'R',&xscl))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 51 (obl) */
    if ((sublist[1]->rbnext=db_newrb(51,'R',&obl))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 7 (tship) */
	extern char* db_str_standard;
	if(!tship)
		fcp1 = db_str_standard;
	else
		tship->get_2(&fcp1);
    if ((sublist[1]->rbnext=db_newrb(7,'S',
        (fcp1!=NULL) ? fcp1 : db_str_quotequote))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 71 (gen) */
    fsh1=(short)gen;
    if ((sublist[1]->rbnext=db_newrb(71,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 72 (hjust) */
    fsh1=(short)hjust;
    if ((sublist[1]->rbnext=db_newrb(72,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 11 (apt) */
    if ((sublist[1]->rbnext=db_newrb(11,'P',apt))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	// Extrusion is now part of the base class, so get it from accessor
	//
	sds_point extru;
	get_210( extru );

    /* 210 (extru) */
    if ((sublist[1]->rbnext=db_newrb(210,'P',extru))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 73 (vjust) */
    fsh1=(short)vjust;
    if ((sublist[1]->rbnext=db_newrb(73,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}




