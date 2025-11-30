/* C:\ICAD\PRJ\LIB\DB\ELLIPSE.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/************************** db_ellipse *****************************/

/*F*/
db_ellipse::db_ellipse(void) : db_entity_with_extrusion(DB_ELLIPSE) {

    cent[0]=cent[1]=cent[2]=smavect[1]=smavect[2]=0.0;
    smavect[0]=1.0; min_maj=0.5; param[0]=0.0; param[1]=IC_TWOPI;
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_ellipse::db_ellipse(db_drawing *dp) : db_entity_with_extrusion(DB_ELLIPSE,dp) {

    cent[0]=cent[1]=cent[2]=smavect[1]=smavect[2]=0.0;
    smavect[0]=1.0; min_maj=0.5; param[0]=0.0; param[1]=IC_TWOPI;

    disp=NULL; memset(extent,0,sizeof(extent));
}
db_ellipse::db_ellipse(const db_ellipse &sour) :
    db_entity_with_extrusion(sour) {  /* Copy constructor */


    DB_PTCPY(cent,sour.cent); DB_PTCPY(smavect,sour.smavect);
    min_maj=sour.min_maj; param[0]=sour.param[0]; param[1]=sour.param[1];

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
}
db_ellipse::~db_ellipse(void) {

    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}

/*F*/
int db_ellipse::entgetspecific(
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

    /* 10 (cent) */
    if ((sublist[0]=sublist[1]=db_newrb(10,'P',cent))==NULL) { rc=-1; goto out; }

    /* 11 (smavect) */
    if ((sublist[1]->rbnext=db_newrb(11,'P',smavect))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	// Extrusion is now part of the base class, so get it from accessor
	//
	sds_point extru;
	get_210( extru );

    /* 210 (extru) */
    if ((sublist[1]->rbnext=db_newrb(210,'P',extru))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 40 (min_maj) */
    if ((sublist[1]->rbnext=db_newrb(40,'R',&min_maj))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 41 (param[0]) */
    if ((sublist[1]->rbnext=db_newrb(41,'R',param))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 42 (param[1]) */
    if ((sublist[1]->rbnext=db_newrb(42,'R',param+1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}

/*F*/
void db_ellipse::acadize(void) {
/*
**  ACAD seems to REQUIRE the 11-group to be the major axis.  We don't.
**  So, when we write a DWG or DXF, it has to be their way.  For cases
**  where min_maj>1.0, this function converts the data by choosing the
**  axis 90 CW from the existing primary axis and adding 90 degrees
**  to the param[]s.
*/
    sds_real ar1;
    sds_point newsmavect,wextru,ap1;


    /* Rule out or fix some never-should-happens: */

    if (icadRealEqual(min_maj,0.0)) return;
    if (min_maj< 0.0) min_maj=-min_maj;
    if (min_maj<=1.0) return;

    /* Get working extru: */
	get_210( wextru );

    /* Cross smavect with wextru to get the new smavect: */
	ic_crossproduct(smavect,wextru,newsmavect);
    if (db_unitizevector(newsmavect)) return;  /* Should never. */

    /* Cross newsmavect with smavect to check wextru: */
	ic_crossproduct(newsmavect,smavect,ap1);
    if (db_unitizevector(ap1)) return;  /* Should never. */

    /* Is the new extru significantly different from the old? */
    /* If so, take the new one: */
    if (fabs(ap1[0]*wextru[0]+ap1[1]*wextru[1]+ap1[2]*wextru[2]-1.0)>IC_ZRO)
        { 
		DB_PTCPY(wextru,ap1); 
		set_210( wextru );
		}

    ar1=DB_VLEN(smavect)*min_maj;  /* New smavect length we need */

    /* Set the new smavect: */
    smavect[0]=newsmavect[0]*ar1;
    smavect[1]=newsmavect[1]*ar1;
    smavect[2]=newsmavect[2]*ar1;

    /* Set the new min_maj: */
    min_maj=1.0/min_maj;

    /* If partial ellipse, add 90 deg to the param[]s: */
    if (fabs(param[0])>IC_ZRO || fabs(param[1]-IC_TWOPI)>IC_ZRO) {
        ar1=0.5*IC_PI;
        param[0]+=ar1; ic_normang(param  ,NULL);
        param[1]+=ar1; ic_normang(param+1,NULL);
    }
}



