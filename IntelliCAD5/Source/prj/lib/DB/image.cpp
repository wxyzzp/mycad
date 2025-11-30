/* C:\ICAD\PRJ\LIB\DB\IMAGE.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/**************************** db_image *****************************/

/*F*/
db_image::db_image(void) : db_entity(DB_IMAGE) {
    pt[0]=pt[1]=pt[2]=0.0;
    grdata=NULL; grdatasz=0;
    pixelvect[0][0]=pixelvect[1][1]=0.001;
    pixelvect[0][1]=pixelvect[0][2]=pixelvect[1][0]=pixelvect[1][2]=0.0;
    imagesz[0]=imagesz[1]=100.0; clipvert=NULL;
    dispprops=7; boundarytype=1; classver=0; nclipverts=0;
    clipon=fade=0; brightness=contrast=50; defref=defreactref=NULL;
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_image::db_image(db_drawing *dp) : db_entity(DB_IMAGE,dp) {
    pt[0]=pt[1]=pt[2]=0.0;
    grdata=NULL; grdatasz=0;
    pixelvect[0][0]=pixelvect[1][1]=0.001;
    pixelvect[0][1]=pixelvect[0][2]=pixelvect[1][0]=pixelvect[1][2]=0.0;
    imagesz[0]=imagesz[1]=100.0; clipvert=NULL;
    dispprops=7; boundarytype=1; classver=0; nclipverts=0;
    clipon=fade=0; brightness=contrast=50; defref=defreactref=NULL;
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_image::db_image(const db_image &sour) :
    db_entity(sour) {  /* Copy constructor */

    DB_PTCPY(pt,sour.pt);
    DB_PTCPY(pixelvect[0],sour.pixelvect[0]);
    DB_PTCPY(pixelvect[1],sour.pixelvect[1]);

    imagesz[0]=sour.imagesz[0];  imagesz[1]  =sour.imagesz[1];
    dispprops =sour.dispprops;   boundarytype=sour.boundarytype;
    classver  =sour.classver;    nclipverts  =sour.nclipverts;
    clipon    =sour.clipon;      fade        =sour.fade;
    brightness=sour.brightness;  contrast    =sour.contrast;
    defref    =sour.defref;      defreactref =sour.defreactref;

    clipvert=NULL;
    if (nclipverts>0 && sour.clipvert!=NULL) {
        clipvert=new sds_real[nclipverts][2];
        memcpy(clipvert,sour.clipvert,nclipverts*sizeof(clipvert[0]));
    }

    grdatasz=sour.grdatasz;
    grdata=NULL;
    if (grdatasz>0 && sour.grdata!=NULL) {
        grdata=new char[grdatasz];
        memcpy(grdata,sour.grdata,grdatasz);
    }

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
}
db_image::~db_image(void) {
    delete [] clipvert; delete [] grdata;
    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}

/*F*/
int db_image::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) {
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    short fsh1;
    int rc,fi1;
    long fl1;
    sds_point ap1,*apa;
    sds_name en;
    struct resbuf *sublist[2];


    rc=0; sublist[0]=sublist[1]=NULL;
    apa=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    /* 90 (classver) */
    fl1=classver;
    if ((sublist[0]=sublist[1]=db_newrb(90,'L',&fl1))==NULL) { rc=-1; goto out; }

    /* 10 */
    if ((sublist[1]->rbnext=db_newrb(10,'P',pt))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 11 */
    if ((sublist[1]->rbnext=db_newrb(11,'P',pixelvect[0]))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 12 */
    if ((sublist[1]->rbnext=db_newrb(12,'P',pixelvect[1]))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 13 (Actually a 2D point) */
    ap1[0]=imagesz[0]; ap1[1]=imagesz[1]; ap1[2]=0.0;
    if ((sublist[1]->rbnext=db_newrb(13,'P',ap1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 340 */
    en[0]=en[1]=0;
    if (defref!=NULL) { en[0]=(long)defref; en[1]=(long)dp; }
    if ((sublist[1]->rbnext=db_newrb(340,'N',en))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 70 */
    fsh1=(short)dispprops;
    if ((sublist[1]->rbnext=db_newrb(70,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 280 */
    fsh1=clipon;
    if ((sublist[1]->rbnext=db_newrb(280,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 281 */
    fsh1=brightness;
    if ((sublist[1]->rbnext=db_newrb(281,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 282 */
    fsh1=contrast;
    if ((sublist[1]->rbnext=db_newrb(282,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 283 */
    fsh1=fade;
    if ((sublist[1]->rbnext=db_newrb(283,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 360 */
    en[0]=en[1]=0;
    if (defreactref!=NULL) { en[0]=(long)defreactref; en[1]=(long)dp; }
    if ((sublist[1]->rbnext=db_newrb(360,'N',en))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 71 */
    fsh1=(short)boundarytype;
    if ((sublist[1]->rbnext=db_newrb(71,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 91 */
    fl1=nclipverts;
    if ((sublist[1]->rbnext=db_newrb(91,'L',&fl1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* The array of 14's */
    if (nclipverts>0) {
        if ((apa= new sds_point [ nclipverts ] )==NULL)
            { rc=-1; goto out; }
		memset(apa,0,sizeof(sds_point)*nclipverts );
        get_14(apa,nclipverts);
        for (fi1=0; fi1<nclipverts; fi1++) {
            if ((sublist[1]->rbnext=db_newrb(14,'P',apa[fi1]))==NULL) { rc=-1; goto out; }
            sublist[1]=sublist[1]->rbnext;
        }
    }

out:
    if (apa!=NULL) delete [] apa;

    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}

/*F*/
int db_image::entmod(struct resbuf *modll, db_drawing *dp) {
/*
**  Entity/table-record/object  specific.  Uses the setters for range-correction.
**
**  Returns: See db_handitem::entmod().
*/
    int did14,pass,cnt,rc;
    struct resbuf *curlink,*trbp1;
    sds_point *parray;
    db_handitem *thiship,*thip1;


    rc=did14=0; thiship=(db_handitem *)this; parray=NULL;

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
            case  10: thiship->set_10(curlink->resval.rpoint); break;
            case  11: thiship->set_11(curlink->resval.rpoint); break;
            case  12: thiship->set_12(curlink->resval.rpoint); break;
            case  13: thiship->set_13(curlink->resval.rpoint); break;
            case  14:
                if (did14) break;
                thiship->set_14((sds_point *)NULL,0);
                did14=1;
                for (pass=0; pass<2; pass++) {  /* 0 : count; 1 : collect */
                    if (pass) {
                        if (cnt<1) break;
                        parray=new sds_point[cnt];
                    }
                    cnt=0;
                    for (trbp1=curlink; trbp1!=NULL; trbp1=trbp1->rbnext) {
                        if (trbp1->restype!=14) continue;
                        if (pass) DB_PTCPY(parray[cnt],trbp1->resval.rpoint);
                        cnt++;
                    }
                }
                thiship->set_14(parray,cnt);  /* Sets 91, too. */
                break;
            case  70: thiship->set_70( curlink->resval.rint); break;
            case  71: thiship->set_71( curlink->resval.rint); break;
            case  90: thiship->set_90( curlink->resval.rlong); break;
            case  91: break;  /* Set by case 14. */
            case 280: thiship->set_280(curlink->resval.rint); break;
            case 281: thiship->set_281(curlink->resval.rint); break;
            case 282: thiship->set_282(curlink->resval.rint); break;
            case 283: thiship->set_283(curlink->resval.rint); break;
            case 340:
                thip1=(db_handitem *)curlink->resval.rlname[0];
                thiship->set_340(thip1);
                break;
            case 360:
                thip1=(db_handitem *)curlink->resval.rlname[0];
                thiship->set_360(thip1);
                break;
            default:
                rc=thiship->entmod1common(&curlink,dp);
                break;
        }
    }

    /* For the array, clear it if it never appeared in modll: */
    if (!did14) thiship->set_14((sds_point *)NULL,0);

    if (db_lasterror) { rc=-6; goto out; }

    if (!rc && curlink!=NULL) rc=set_eed(curlink,dp);

out:
    delete [] parray;

    return rc;
}


