/* C:\ICAD\PRJ\LIB\LISP\GEOM.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/
//Bugzilla No. 78442 ; 18-03-2003
extern int ic_pt_equal(sds_point p1, sds_point p2);

int lsp_2ptarg(int op) {
    int msgn;
    sds_real *arp[2];
    struct resbuf retrb;

    msgn=-1; arp[0]=arp[1]=NULL;
    retrb.restype=RTREAL; retrb.resval.rreal=0.0; retrb.rbnext=NULL; /*Default*/

    lsp_convptlist(lsp_argsll,0);  /* Convert from lists to pts. */

    if (lsp_argsll==NULL || lsp_argsll->rbnext==NULL) { msgn=2; goto out; }
    if (lsp_argsll->rbnext->rbnext!=NULL)             { msgn=9; goto out; }

    if (lsp_argsll->restype==RTPOINT || lsp_argsll->restype==RT3DPOINT)
        arp[0]=lsp_argsll->resval.rpoint;
    if (lsp_argsll->rbnext->restype==RTPOINT ||
        lsp_argsll->rbnext->restype==RT3DPOINT)
            arp[1]=lsp_argsll->rbnext->resval.rpoint;
    if (arp[0]==NULL || arp[1]==NULL) { msgn=1; goto out; }

    switch (op) {
        case 0:  /* Distance */
            if (lsp_argsll->restype==RTPOINT ||
                lsp_argsll->rbnext->restype==RTPOINT)
                    arp[0][2]=arp[1][2]=0.0;
            retrb.resval.rreal=sds_distance(arp[0],arp[1]);
            break;
        case 1:  /* Angle */
            retrb.resval.rreal=sds_angle(arp[0],arp[1]);
            break;
    }

    if (lsp_retval(&retrb)!=RTNORM) { msgn=7; goto out; }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_distance(void) { return lsp_2ptarg(0); }
int lsp_angle(void)    { return lsp_2ptarg(1); }

int lsp_inters(void) {
/*
**  Note that a 3D pt results only when all 4 defining pts are 3D pts.
*/
    int msgn,its3d,fi1;
    int teston;
    sds_real *pt[4];
    struct resbuf *tp1;

    msgn=-1; teston=its3d=1;

    /* Convert pts from list-form to pt-form. */
    lsp_convptlist(lsp_argsll,0);

    for (tp1=lsp_argsll,fi1=0; tp1!=NULL && fi1<4; tp1=tp1->rbnext,fi1++) {
        if      (tp1->restype==RTPOINT) its3d=0;
        else if (tp1->restype!=RT3DPOINT) { msgn=1; goto out; }
        pt[fi1]=tp1->resval.rpoint;
    }
    if (fi1<4) { msgn=2; goto out; }
    if (tp1!=NULL) {
        if (tp1->restype==RTNIL) teston=0;
        if (lsp_nextarg(tp1)!=NULL) { msgn=9; goto out; }
    }

    /*
    **  sds_inters() always works in 3D; the LISP inters only does 3D
    **  intersections if ALL of the defining pts are 3D.  Therefore,
    **  if the LISP one is going to set a 2D result, trick the ADS
    **  function into doing it all in the same plane by setting
    **  the Z's to 0.0.
    */
    if (!its3d) pt[0][2]=pt[1][2]=pt[2][2]=pt[3][2]=0.0;

    lsp_freesplrb(&lsp_evres);  /* Default result is nil. */

	//Bugzilla No. 78442 ; 18-03-2003
	if (ic_pt_equal(pt[0],pt[1]) || ic_pt_equal(pt[2],pt[3]))
		goto out;

    if (sds_inters(pt[0],pt[1],pt[2],pt[3],teston,
        lsp_evres.resval.rpoint)==RTNORM) {

        lsp_evres.restype=(its3d) ? RT3DPOINT : RTPOINT;
        /* Convert to list-form (2D or 3D): */
        if (lsp_convptlist(&lsp_evres,1)) { msgn=7; goto out; }
    }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_trans(void) {
    int msgn,fi1;
    int disp;
    struct resbuf *from,*to,*ptlink,*tp1;

    msgn=-1;

    from=to=NULL; disp=0;

    lsp_freesplrb(&lsp_evres);  /* Default result is nil. */

    /* Convert pts from list-form to pt-form. */
    lsp_convptlist(lsp_argsll,0);

    if ((tp1=lsp_argsll)==NULL) { msgn=2; goto out; }
    if (tp1->restype!=RT3DPOINT &&
        tp1->restype!=RTPOINT)  { msgn=1; goto out; }
    ptlink=tp1;
    if ((tp1=tp1->rbnext)==NULL) { msgn=2; goto out; }
    from=tp1;
    if (from->restype==RTLONG) {  /* Convert to RTSHORT */
        fi1=(int)from->resval.rlong;  /* Don't try direct intraunion assignment. */
        from->restype=RTSHORT;
        from->resval.rint=fi1;
    } else if (from->restype!=RTSHORT && from->restype!=RTENAME &&
        from->restype!=RT3DPOINT) {

        msgn=48; goto out;
    }
    if ((tp1=tp1->rbnext)==NULL) { msgn=2; goto out; }
    to=tp1;
    if (to->restype==RTLONG) {  /* Convert to RTSHORT */
        fi1=(int)to->resval.rlong;  /* Don't try direct intraunion assignment. */
        to->restype=RTSHORT;
        to->resval.rint=fi1;
    } else if (to->restype!=RTSHORT && to->restype!=RTENAME &&
        to->restype!=RT3DPOINT) {

        msgn=48; goto out;
    }
    if ((tp1=tp1->rbnext)!=NULL) {
        if (tp1->restype!=RTNIL) disp=1;
        if (lsp_nextarg(tp1)!=NULL) { msgn=9; goto out; }
    }

    /* Set the appropriate Z for a 2D source pt based on "from" and disp. */
    /* (See the AutoLISP manual; not sure how to do the DCS and PSDCS.) */
    /* Note that lsp_convptlist() above sets a 2D pt's Z to the elevation. */
    if (ptlink->restype==RTPOINT) {  /* 2D given pt */
        if (disp) ptlink->resval.rpoint[2]=0.0;  /* Always 0.0 for disp. */
        else {  /* A pt -- not a displacement. */
            if (from->restype==RTSHORT) {
                if (from->resval.rint==0)
                    ptlink->resval.rpoint[2]=0.0; /* WCS */
            } else if (from->restype==RTENAME || from->restype==RT3DPOINT) {
                ptlink->resval.rpoint[2]=0.0;  /* ECS */
            }
        }
    }

    if (sds_trans(ptlink->resval.rpoint,from,to,disp,
        lsp_evres.resval.rpoint)!=RTNORM) { lsp_retnil(); goto out; }

    /* Type is 2D if source pt is 2D and the codes match: */
    lsp_evres.restype=(ptlink->restype==RTPOINT &&
         from->restype==to->restype &&
        (from->restype!=RTSHORT || from->resval.rint==to->resval.rint)) ?
            RTPOINT : RT3DPOINT;

    /* Convert pt to LISP-form: */
    if (lsp_convptlist(&lsp_evres,1)) { msgn=7; goto out; }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}


