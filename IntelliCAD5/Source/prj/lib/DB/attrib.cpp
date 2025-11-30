/* C:\ICAD\PRJ\LIB\DB\ATTRIB.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/************************** db_attrib *****************************/

/*F*/
db_attrib::db_attrib(void) : db_entity_with_extrusion(DB_ATTRIB) {

    val=tag=NULL; tship=NULL;
    ipt[0]=ipt[1]=ipt[2]=apt[0]=apt[1]=apt[2]=0.0;
    thick=rot=obl=0.0; xscl=1.0;
    db_qgetvar(DB_QTEXTSIZE,NULL,&ht,DB_DOUBLE,0);
    flags=gen=hjust=fldlen=vjust=0; 
    disp=NULL;
	memset(extent,0,sizeof(extent));
}
db_attrib::db_attrib(db_drawing *dp) : db_entity_with_extrusion(DB_ATTRIB,dp) {

    char *bufp;

    bufp=(dp!=NULL) ? dp->ret_headerbuf() : NULL;

    val=tag=NULL;
    tship=(dp!=NULL) ? dp->ret_currenttabrec(DB_QTEXTSTYLE) : NULL;
    ipt[0]=ipt[1]=ipt[2]=apt[0]=apt[1]=apt[2]=0.0;
    rot=obl=0.0; xscl=1.0;
    db_qgetvar(DB_QTHICKNESS,bufp,&thick,DB_DOUBLE,0);
    db_qgetvar(DB_QTEXTSIZE ,bufp,&ht,DB_DOUBLE,0);
    flags=gen=hjust=fldlen=vjust=0;

    disp=NULL;
	memset(extent,0,sizeof(extent));
}
db_attrib::db_attrib(const db_attrib &sour) :
    db_entity_with_extrusion(sour) {  /* Copy constructor */

    thick=sour.thick;

    val=NULL; if (sour.val!=NULL) db_astrncpy(&val,sour.val,-1);
    tag=NULL; if (sour.tag!=NULL) db_astrncpy(&tag,sour.tag,-1);

    DB_PTCPY(ipt,sour.ipt); DB_PTCPY(apt,sour.apt);
    tship =sour.tship ; ht   =sour.ht   ; xscl=sour.xscl; rot  =sour.rot  ;
    obl   =sour.obl   ; flags=sour.flags; gen =sour.gen ; hjust=sour.hjust;
    fldlen=sour.fldlen; vjust=sour.vjust;

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
}

db_attrib::~db_attrib(void) {

    delete [] val; delete [] tag;

    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}

db_handitem *db_attrib::ret_main(void) {
    db_handitem *thip1,*rethip;

    rethip=NULL;
    for (thip1=this; thip1!=NULL && thip1->ret_type()==DB_ATTRIB; thip1=thip1->next);
    if (thip1!=NULL && thip1->ret_type()==DB_SEQEND)
        ((db_seqend *)thip1)->get_mainent(&rethip);
    return rethip;
}

/*F*/
int db_attrib::entgetspecific(
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

	if(db_handitem* pMain = ret_main())
    {
		sds_name name;
		name[0] = (long)pMain;
		name[1] = (long)dp;
		if ((sublist[0]=sublist[1]=db_newrb(330, 'N', name))==NULL) { rc=-1; goto out; }
		/* 39 (thick) */
		if ((sublist[1]->rbnext=db_newrb(39,'R',&thick))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	else
	{
		/* 39 (thick) */
		if ((sublist[0]=sublist[1]=db_newrb(39,'R',&thick))==NULL) { rc=-1; goto out; }
	}

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

    /* 7 (tship). VT: Possible use null pointer. Fix. 21-3-02 */
    if(!tship) { rc = -1; goto out; }
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

    /* 2 (tag) */
    if ((sublist[1]->rbnext=db_newrb(2,'S',
        (tag!=NULL) ? tag : db_str_quotequote))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 70 (flags) */
    fsh1=(short)flags;
    if ((sublist[1]->rbnext=db_newrb(70,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 73 (fldlen) */
    fsh1=(short)fldlen;
    if ((sublist[1]->rbnext=db_newrb(73,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 74 (vjust) */
    fsh1=(short)vjust;
    if ((sublist[1]->rbnext=db_newrb(74,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}



