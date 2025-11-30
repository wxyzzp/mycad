/* C:\ICAD\PRJ\LIB\DB\SPLINE.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/************************** db_spline *****************************/

/*F*/
db_spline::db_spline(void) : db_entity_with_extrusion(DB_SPLINE), flags(0) 
{

    scenario=0;

    control=NULL; fpt=NULL;
    tandir[0][0]=tandir[0][1]=tandir[0][2]=
        tandir[1][0]=tandir[1][1]=tandir[1][2]=0.0;  /* (0,0,0) means undefined. */
    knot=NULL; ktol=ctol=1.0E-7; ftol=1.0E-10;
    deg=3; nknots=ncpts=nfpts=0;
	setIsRational(true);
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_spline::db_spline(db_drawing *dp) : db_entity_with_extrusion(DB_SPLINE,dp), flags(0) 
{
    scenario=0;

    control=NULL; fpt=NULL;
    tandir[0][0]=tandir[0][1]=tandir[0][2]=
        tandir[1][0]=tandir[1][1]=tandir[1][2]=0.0;  /* (0,0,0) means undefined. */
    knot=NULL; ktol=ctol=1.0E-7; ftol=1.0E-10;
    deg=3; nknots=ncpts=nfpts=0;
	setIsRational(true);
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_spline::db_spline(const db_spline &sour) :
    db_entity_with_extrusion(sour) {  /* Copy constructor */


    ncpts=sour.ncpts; control=NULL;
    if (ncpts>0 && sour.control!=NULL) {
        control=new db_splinecontrollink[ncpts];
        memcpy(control,sour.control,ncpts*sizeof(db_splinecontrollink));
    }
    nfpts=sour.nfpts; fpt=NULL;
    if (nfpts>0 && sour.fpt!=NULL) {
        fpt=new sds_point[nfpts];
        memcpy(fpt,sour.fpt,nfpts*sizeof(sds_point));
    }
    nknots=sour.nknots; knot=NULL;
    if (nknots>0 && sour.knot!=NULL) {
        knot=new sds_real[nknots];
        memcpy(knot,sour.knot,nknots*sizeof(sds_real));
    }

    DB_PTCPY(tandir[0],sour.tandir[0]); DB_PTCPY(tandir[1],sour.tandir[1]);

    scenario=sour.scenario; ktol=sour.ktol; ctol=sour.ctol; ftol=sour.ftol;
    flags   =sour.flags;     deg=sour.deg;

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
}
db_spline::~db_spline(void) {
    delete [] control; delete [] fpt; delete [] knot;

    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}

/*F*/
int db_spline::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) {
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    short fsh1;
    int do41s,rc,fi1;
    struct resbuf *sublist[2];


    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;


	// Extrusion is now part of the base class, so get it from accessor
	//
	sds_point extru;
	get_210( extru );

    /* 210 (extru) */
    if ((sublist[0] = sublist[1]=db_newrb(210,'P',extru))==NULL) { rc=-1; goto out; }

    /* 70 (flags) */
    fsh1=(short)flags;
    if ((sublist[1]->rbnext=db_newrb(70,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 71 (deg) */
    fsh1=(short)deg;
    if ((sublist[1]->rbnext=db_newrb(71,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 72 (nknots) */
    fsh1=(short)nknots;
    if ((sublist[1]->rbnext=db_newrb(72,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 73 (ncpts) */
    fsh1=(short)ncpts;
    if ((sublist[1]->rbnext=db_newrb(73,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 74 (nfpts) */
    fsh1=(short)nfpts;
    if ((sublist[1]->rbnext=db_newrb(74,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 42 (ktol) */
    if ((sublist[1]->rbnext=db_newrb(42,'R',&ktol))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 43 (ctol) */
    if ((sublist[1]->rbnext=db_newrb(43,'R',&ctol))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    if (scenario==2) {
        /* 44 (ftol) */
        if ((sublist[1]->rbnext=db_newrb(44,'R',&ftol))==NULL) { rc=-1; goto out; }
        sublist[1]=sublist[1]->rbnext;

    }

    /* 12 (tandir[0]) */
    if ((sublist[1]->rbnext=db_newrb(12,'P',tandir[0]))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 13 (tandir[0]) */
    if ((sublist[1]->rbnext=db_newrb(13,'P',tandir[1]))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    if (knot!=NULL) {
        for (fi1=0; fi1<nknots; fi1++) {
            /* 40 (knot[fi1]) */
            if ((sublist[1]->rbnext=db_newrb(40,'R',knot+fi1))==NULL) { rc=-1; goto out; }
            sublist[1]=sublist[1]->rbnext;
        }
    }

    if (control!=NULL) {

        /* See if ANY wt is non-1.0: */
        for (fi1=0; fi1<ncpts && fabs(control[fi1].wt-1.0)<IC_ZRO; fi1++);
        do41s=(fi1<ncpts);

        /*
        **  Oops.  For now, we're going to force the 41's (for the
        **  dialog editor).  If someday we want to go back to ACAD's
        **  logic (showing them only if at least one 41 is non-1.0),
        **  just delete this line:
        */
        do41s=1;

        for (fi1=0; fi1<ncpts; fi1++) {
            /* 10 (control[fi1].pt) */
            if ((sublist[1]->rbnext=db_newrb(10,'P',control[fi1].pt))==NULL) { rc=-1; goto out; }
            sublist[1]=sublist[1]->rbnext;

            if (do41s) {
                /* 41 (control[fi1].wt) */
                if ((sublist[1]->rbnext=db_newrb(41,'R',&control[fi1].wt))==NULL) { rc=-1; goto out; }
                sublist[1]=sublist[1]->rbnext;
            }
        }
    }

    if (fpt!=NULL) {
        for (fi1=0; fi1<nfpts; fi1++) {
            /* 11 (fpt[fi1]) */
            if ((sublist[1]->rbnext=db_newrb(11,'P',fpt[fi1]))==NULL) { rc=-1; goto out; }
            sublist[1]=sublist[1]->rbnext;

        }
    }

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}

/*F*/
int db_spline::entmod(struct resbuf *modll, db_drawing *dp) {
/*
**  Entity/table-record specific.  Uses the setters for range-correction.
**
**  Returns: See db_handitem::entmod().
*/
    int did40,did10,did11,cnt,pass,rc;
    sds_real  *rarray;
    sds_point *parray;
    struct resbuf *curlink,*trbp1;
    db_handitem *thiship;
	
	// Added Cybage VSB 29/10/2001 [
	// Reason: Fix for Bug No: 77897 from Bugzilla
	if(!validateEnt(modll,DB_SPLINE))	
		return -1;
	// Added Cybage VSB 29/10/2001 ]

    rc=0; thiship=(db_handitem *)this;
    rarray=NULL; parray=NULL;
    did10=did11=did40=0;

    db_lasterror=0;

    if (modll==NULL || dp==NULL) { rc=-1; goto out; }

    del_xdict();

    curlink=modll;

    /*
    **  If modll has been built properly, the first few groups
    **  are things we don't allow to be modified (ename, type, handle),
    **  so let's walk over these right now:
    */
    while (curlink!=NULL &&
        (curlink->restype==-1 ||
         curlink->restype== 0 ||
         curlink->restype== 5)) curlink=curlink->rbnext;

    /* Walk; stop at the end or the EED sentinel (-3): */
    for (; !rc && curlink!=NULL && curlink->restype!=-3; curlink=curlink->rbnext) {

        /* Call the correct setter: */
        switch (curlink->restype) {
            case  10:
                /* ALSO DOES THE 41'S!  Uses 1.0 for any 10's not followed */
                /* by a 41. */

                if (did10) break;
                delete [] rarray; rarray=NULL;
                delete [] parray; parray=NULL;
                cnt=0; did10=1;
                for (pass=0; pass<2; pass++) {  /* 0 : count; 1 : collect */
                    if (pass) {
                        if (cnt<1) break;
                        rarray=new sds_real[cnt];
                        parray=new sds_point[cnt];
                        cnt=0;
                    }
                    for (trbp1=curlink; trbp1!=NULL; trbp1=trbp1->rbnext) {
                        if (trbp1->restype!=10) continue;
                        if (pass) DB_PTCPY(parray[cnt],trbp1->resval.rpoint);
                        while (trbp1->rbnext!=NULL && trbp1->rbnext->restype==41)
                            trbp1=trbp1->rbnext;
                        if (pass) rarray[cnt]=(trbp1->restype==41) ?
                            trbp1->resval.rreal : 1.0;
                        cnt++;
                    }
                }
                if (parray==NULL) { cnt=0; delete [] rarray; rarray=NULL; }

                /* Order-dependent: 10, then 41: */
                thiship->set_10(parray,cnt);
                thiship->set_41(rarray,cnt);
                break;
            case  11:
                if (did11) break;
                delete [] parray; parray=NULL; cnt=0; did11=1;
                for (pass=0; pass<2; pass++) {  /* 0 : count; 1 : collect */
                    if (pass) {
                        if (cnt<1) break;
                        parray=new sds_point[cnt]; cnt=0;
                    }
                    for (trbp1=curlink; trbp1!=NULL; trbp1=trbp1->rbnext) {
                        if (trbp1->restype!=11) continue;
                        if (pass) DB_PTCPY(parray[cnt],trbp1->resval.rpoint);
                        cnt++;
                    }
                }
                thiship->set_11(parray,cnt);
                break;
            case  12: thiship->set_12(curlink->resval.rpoint); break;
            case  13: thiship->set_13(curlink->resval.rpoint); break;
            case  40:
                if (did40) break;
                delete [] rarray; rarray=NULL; cnt=0; did40=1;
                for (pass=0; pass<2; pass++) {  /* 0 : count; 1 : collect */
                    if (pass) {
                        if (cnt<1) break;
                        rarray=new sds_real[cnt]; cnt=0;
                    }
                    for (trbp1=curlink; trbp1!=NULL; trbp1=trbp1->rbnext) {
                        if (trbp1->restype!=40) continue;
                        if (pass) rarray[cnt]=trbp1->resval.rreal;
                        cnt++;
                    }
                }
                thiship->set_40(rarray,cnt);
                break;
            case  42: thiship->set_42(curlink->resval.rreal); break;
            case  43: thiship->set_43(curlink->resval.rreal); break;
            case  44: thiship->set_44(curlink->resval.rreal); break;
            case  70: thiship->set_70(curlink->resval.rint ); break;
            case  71: thiship->set_71(curlink->resval.rint ); break;
            case  72: break;  /* Set by 40; just avoid default case. */
            case  73: break;  /* Set by 10; just avoid default case. */
            case  74: break;  /* Set by 11; just avoid default case. */
            case 210: thiship->set_210(curlink->resval.rpoint); break;
            default:  /* The common entity or table record groups. */
                rc=thiship->entmod1common(&curlink,dp);
                break;
        }
    }

    if (db_lasterror) { rc=-6; goto out; }

    if (!rc) {
        /* For the arrays, clear them if they never appeared in modll: */
        if (!did10) thiship->set_10((sds_point *)NULL,0);
        if (!did11) thiship->set_11(NULL,0);
        if (!did40) thiship->set_40((sds_real*)NULL,0);

        /* Do the EED: */
        if (curlink!=NULL) rc=set_eed(curlink,dp);

        /* If there are 11's call it scenario 2; otherwise, 1: */
        scenario=1+(fpt!=NULL && nfpts>0);
    }

out:
    delete [] rarray;
    delete [] parray;

    return rc;
}



