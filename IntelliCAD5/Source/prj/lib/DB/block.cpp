/* C:\ICAD\PRJ\LIB\DB\BLOCK.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/**************************** db_block *****************************/

/*F*/
db_block::db_block(void) : db_entity(DB_BLOCK) {

    name=NULL; tflags=0; looked4xref=0;
    xrefpn=NULL; xrefdp=NULL; base[0]=base[1]=base[2]=0.0; bthip=NULL;
}
db_block::db_block(db_drawing *dp) : db_entity(DB_BLOCK,dp) {

    name=NULL; tflags=0; looked4xref=0;
    xrefpn=NULL; xrefdp=NULL; base[0]=base[1]=base[2]=0.0; bthip=NULL;
}
db_block::db_block(const db_block &sour) :
    db_entity(sour) {  /* Copy constructor */

    name  =NULL; if (sour.name  !=NULL) db_astrncpy(&name  ,sour.name  ,-1);
    xrefpn=NULL; if (sour.xrefpn!=NULL) db_astrncpy(&xrefpn,sour.xrefpn,-1);
    tflags=sour.tflags; looked4xref=sour.looked4xref;
    xrefdp=NULL;  /* Or we'll have a freeing problem. */
    DB_PTCPY(base,sour.base); bthip=sour.bthip;
}
db_block::~db_block(void) {
    delete [] name; delete [] xrefpn; delete (db_drawing *)xrefdp;
}


/*F*/
int db_block::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) {
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    char *fcp1;
    int fint1,rc;
    sds_point ap1;
    sds_name en;
    struct resbuf *sublist[2];


    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    /* Note that for BLOCK, ALL values come from the table record. */

    /* 70 (flags) */
    fint1=0;
    if (bthip!=NULL) bthip->get_70(&fint1);
    if ((sublist[0]=sublist[1]=db_newrb(70,'I',&fint1))==NULL) { rc=-1; goto out; }

    /* 10 (base) */
    ap1[0]=ap1[1]=ap1[2]=0.0;
    if (bthip!=NULL) bthip->get_10(ap1);
    if ((sublist[1]->rbnext=db_newrb(10,'P',ap1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* -2 (ename of 1st ent in def) */
    if (bthip!=NULL) ((db_blocktabrec *)bthip)->get_firstentinblock(en,dp); else en[0]=en[1]=0;
    if ((sublist[1]->rbnext=db_newrb(-2,'N',en))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 2 (Block name) */
    fcp1=NULL;
    if (bthip!=NULL) bthip->get_2(&fcp1);
    if ((sublist[1]->rbnext=db_newrb(2,'S',
        (fcp1!=NULL) ? fcp1 : db_str_quotequote))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 3 (xrefpn) */
    fcp1=NULL;
    if (bthip!=NULL) bthip->get_3(&fcp1);
    if ((sublist[1]->rbnext=db_newrb(3,'S',
        (fcp1!=NULL) ? fcp1 : db_str_quotequote))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}



