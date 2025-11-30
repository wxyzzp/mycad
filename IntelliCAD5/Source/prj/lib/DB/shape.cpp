/* C:\ICAD\PRJ\LIB\DB\SHAPE.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#include "db.h"
#include "styletabrec.h"

/************************** db_shape *****************************/

/*F*/
db_shape::db_shape(void) : db_entity_with_extrusion(DB_SHAPE) {
    shname=NULL; tship=NULL; shnum=0;
    pt[0]=pt[1]=pt[2]=rot=obl=0.0; sz=xscl=1.0;
    thick=0.0;
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_shape::db_shape(db_drawing *dp) : db_entity_with_extrusion(DB_SHAPE,dp) {
    char *bufp;

    bufp=(dp!=NULL) ? dp->ret_headerbuf() : NULL;

    shname=NULL; tship=NULL; shnum=0;
    pt[0]=pt[1]=pt[2]=rot=obl=0.0; sz=xscl=1.0;

    db_qgetvar(DB_QTHICKNESS,bufp,&thick,DB_DOUBLE,0);

    disp=NULL; memset(extent,0,sizeof(extent));
}
db_shape::db_shape(const db_shape &sour) :
    db_entity_with_extrusion(sour) {  /* Copy constructor */

    thick=sour.thick;

    shname=NULL; if (sour.shname!=NULL) db_astrncpy(&shname,sour.shname,-1);
    tship=sour.tship; shnum=sour.shnum;

    DB_PTCPY(pt,sour.pt);

    sz=sour.sz; xscl=sour.xscl; rot=sour.rot; obl=sour.obl;

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
}
db_shape::~db_shape(void) {
    delete [] shname;

    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}

/*F*/
int db_shape::entgetspecific(
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

    /* 10 (pt) */
    if ((sublist[1]->rbnext=db_newrb(10,'P',pt))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 40 (sz) */
    if ((sublist[1]->rbnext=db_newrb(40,'R',&sz))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 2 (shname) */
	if ((sublist[1]->rbnext=db_newrb(2,'S',
	    (shname!=NULL) ? shname : db_str_quotequote))==NULL) { rc=-1; goto out; }
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

bool db_shape::set_name(char* p){
	db_fontlink* pfont;
	int code;

	if( tship && p ){
		if( pfont = ((db_styletabrec*)tship)->ret_font() ){
			if(( code = db_findCodeByName(pfont,p)) != 0 ){
				shnum = code;
				db_astrncpy(&shname, p, -1);
	}	}	}
	return true;
}

bool db_shape::set_code(int p) {
	db_fontlink* pfont;
	char shbuf[256];

	if( tship ){
		if( pfont = ((db_styletabrec*)tship)->ret_font() ){
			if( db_findNameByCode( shbuf, p, pfont )){
				shnum=p;
				db_astrncpy(&shname, shbuf, -1);
	}	}	}
	return true;
}

bool db_shape::set_name(char *p, db_drawing *dp) {
	int  flag;
	db_fontlink* pfont;
	db_styletabrec*	tship1;

	if( dp ){
		for( tship1 = (db_styletabrec*)(dp->tblnext(DB_STYLETABREC,1));
			tship1 != NULL ; tship1 = (db_styletabrec*)(dp->tblnext(DB_STYLETABREC,0))){
			tship1->get_70(&flag);
			if(( flag & IC_SHAPEFILE_SHAPES ) && !( flag & IC_ENTRY_XREF_DEPENDENT )){
				pfont = tship1->ret_font();
				if(( flag = db_findCodeByName(pfont, p)) != 0 ){
					tship = tship1;
					shnum = flag;
					db_astrncpy(&shname, p, -1);
					return true;
				}
			}
		}
	}
	return false;
}
