/* C:\ICAD\PRJ\LIB\LISP\GR.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/

int lsp_grdraw(void) {
    int msgn,color,hilite;
    sds_real *arp1,*arp2;
    struct resbuf *tp1;

    msgn=-1; hilite=0; color=7; arp1=arp2=NULL;

    lsp_convptlist(lsp_argsll,0);  /* Convert list-form to pt-form. */

    if ((tp1=lsp_argsll)==NULL)  { msgn=2; goto out; }
    if (tp1->restype!=RT3DPOINT &&
        tp1->restype!=RTPOINT)   { msgn=1; goto out; }
    arp1=tp1->resval.rpoint;
    if ((tp1=tp1->rbnext)==NULL) { msgn=2; goto out; }
    if (tp1->restype!=RT3DPOINT &&
        tp1->restype!=RTPOINT)   { msgn=1; goto out; }
    arp2=tp1->resval.rpoint;
    if ((tp1=tp1->rbnext)==NULL) { msgn=2; goto out; }
    if (tp1->restype==RTLONG) color=(int)tp1->resval.rlong;
    else if (tp1->restype==RTSHORT) color=(int)tp1->resval.rlong;
    else { msgn=1; goto out; }
    if ((tp1=tp1->rbnext)!=NULL) {
        if (tp1->restype==RTLONG) hilite=(int)tp1->resval.rlong;
        else if (tp1->restype==RTSHORT) hilite=(int)tp1->resval.rlong;
        else { msgn=1; goto out; }
        if (tp1->rbnext!=NULL)   { msgn=9; goto out; }
    }

    sds_grdraw(arp1,arp2,color,hilite);

    lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_grtext(void) {
    char *textp;
    int msgn,box,hl;
    struct resbuf *tp1;

    msgn=-1; box=-10; textp=NULL; hl=-1;

    if ((tp1=lsp_argsll)!=NULL) {

        if (tp1->restype==RTLONG)       box=(int)tp1->resval.rlong;
        else if (tp1->restype==RTSHORT) box=tp1->resval.rint;
        else { msgn=1; goto out; }

        if ((tp1=tp1->rbnext)==NULL) { msgn=2; goto out; }
        if (tp1->restype!=RTSTR)     { msgn=1; goto out; }
        textp=tp1->resval.rstring;

        if ((tp1=tp1->rbnext)!=NULL) {
            if (tp1->restype==RTLONG)       hl=(int)tp1->resval.rlong;
            else if (tp1->restype==RTSHORT) hl=tp1->resval.rint;
            else { msgn=1; goto out; }
            if (hl<0) hl=-hl;  /* A subtle difference between LISP and ADS. */
                               /* LISP can only put side menu text if hl is */
                               /* MISSING; ADS requires a neg hl.  */
                               /* A neg hl in LISP is the same as positive. */
            if (tp1->rbnext!=NULL) { msgn=9; goto out; }
        }

    }

    if (sds_grtext(box,textp,hl)!=RTNORM || textp==NULL || box<-2)
        lsp_retnil();
    else if (lsp_retstr(textp)!=RTNORM) msgn=7;

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_grvecs(void) {
/*
**  NOTE: The AutoLISP and ADS versions of grvecs use the matrix
**  differently.  After much difficulty, I finally discovered that
**  the LISP version seems to be using the matrix differently -- similar
**  to the nentsel method of using ROW points instead of COLUMN points.
**  It seems that if you transpose the upper left 3x3 elements and
**  send that to sds_grvecs() you get the same results as calling
**  the LISP grvecs without transposition.  The result is that if you
**  call the LISP grvecs with a matrix built to scale/rotate/translate
**  in sds_grvecs(), you get what you would expect if the rotation
**  direction were reversed and the order were changed to
**  rotate/scale/translate.
**
**  So, we're converting the list-form of the matrix to the
**  sds_matrix form and then transposing the upper left 3x3 elements in
**  this function.  Seems to work (with XY_plane rotations anyway).
*/
    int msgn,fi1,fi2;
    long fl1;
    sds_real ar1;
    struct resbuf *matarg,*thismem,*before,*tp1;
    sds_matrix mat;

    msgn=-1;
    for (fi1=0; fi1<4; fi1++)
        for (fi2=0; fi2<4; fi2++)
            mat[fi1][fi2]=(sds_real)(fi1==fi2);

    lsp_freesplrb(&lsp_evres);  /* Default result is nil. */

    if (lsp_argsll==NULL)          { msgn=2; goto out; }
    if (lsp_argsll->restype==RTNIL)          goto out;  /* nil */
    if (lsp_argsll->restype!=RTLB) { msgn=1; goto out; }
    if (lsp_argsll->rbnext==NULL)  { msgn=5; goto out; }
    if (lsp_argsll->rbnext->restype==RTLE)   goto out;  /* nil */

    lsp_convptlist(lsp_argsll->rbnext,0);

    /* Get link just before the RTLE that ends the vectors list: */
    if (lsp_argend(lsp_argsll,&before)==NULL || before==NULL)
        { msgn=5; goto out; }

	//I dont think we need this following block of code that calculates "before"  AM
    /* Chop out just the MEMBERS of the vectors list, */
    /* convert to point-form, reconnect.  (If you convert lsp_argsll, */
    /* the valid vectors list '(1 2 3) (only colors) gets converted */
    /* to an RT3DPOINT.): */
    tp1=before->rbnext; before->rbnext=NULL;
    before=lsp_argsll->rbnext;  /* Find last link of members again. */
    while (before->rbnext!=NULL) before=before->rbnext;
    before->rbnext=tp1;
    /* before is now correct again (used later). */

    /* Check the vectors for bad types and an odd # of pts between */
    /* color changes, and change RTLONG colors to RTSHORTs for */
    /* sds_grvecs: */
    fi1=0;  /* Flags an odd # of args. */
    for (thismem=lsp_argsll->rbnext; thismem!=NULL &&
        thismem->restype!=RTLE; thismem=lsp_nextarg(thismem)) {

        if (thismem->restype==RT3DPOINT || thismem->restype==RTPOINT) {
            fi1=!fi1;
        } else if (thismem->restype==RTLONG || thismem->restype==RTSHORT) {
            if (fi1) { msgn=43; goto out; }
            if (thismem->restype==RTLONG) {
                /* Convert to RTSHORT. */
                /* Don't try intra-union assignment. */

                fl1=thismem->resval.rlong;
                thismem->restype=RTSHORT;
                thismem->resval.rint=(int)fl1;
            }
        } else { msgn=42; goto out; }
    }
    if (fi1) { msgn=43; goto out; }

    /* Check for matrix: */
    if ((matarg=lsp_nextarg(lsp_argsll))!=NULL) {

        if (lsp_nextarg(matarg)!=NULL) { msgn=9; goto out; }

        /* nil and () are okay; just set nil (not sure WHAT AutoLISP */
        /* is doing; not consistent; they may have a bug). */

        if (matarg->restype==RTNIL)          goto out;  /* nil */
        if (matarg->restype!=RTLB) { msgn=1; goto out; }
        if (matarg->rbnext==NULL)  { msgn=5; goto out; }
        if (matarg->rbnext->restype==RTLE)   goto out;  /* nil */

        /* Convert matrix from LISP form to ADS form: */
        if (lsp_convmat(&matarg,mat,0)) { msgn=44; goto out; }

        /* Transpose the top left 3x3 part (see note above): */
        ar1=mat[0][1]; mat[0][1]=mat[1][0]; mat[1][0]=ar1;
        ar1=mat[0][2]; mat[0][2]=mat[2][0]; mat[2][0]=ar1;
        ar1=mat[1][2]; mat[1][2]=mat[2][1]; mat[2][1]=ar1;

    }

    /* Chop out the vectors llist from the converted LISP list, */
    /* use it, repair it: */
    tp1=before->rbnext; before->rbnext=NULL;
    sds_grvecs(lsp_argsll->rbnext,mat); /* Possible errors trapped earlier. */
    before->rbnext=tp1;

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_grread(void) {
/*
**  The AutoLISP one is quirky.  Ours is only an approximation.
**
**  The AutoLISP parameters are given as
**
**      (grread [track][allkeys [curtype]])
**
**  It doesn't seem to care what type allkeys is; it'll take an
**  internal function or a string, for example.  But allkeys is documented
**  to be a bit-coded integer, so what do the non-numeric types mean?
**  OURS requires a number.
**
**  What happens if you give it 2 args?  Are they track and allkeys,
**  or allkeys and curtype?  AutoLISP Answer: sometimes one way,
**  sometimes the other.  (grread nil 2) sometimes traps F-keys and
**  uses a normal cursor, sometimes uses a pickbox cursor.  MUST be a
**  bug.  OURS always assumes they are track and allkeys (since the 2
**  being taken as curtype in the above example seems to be the rare
**  case).
**
**  (grread t 0 0) and (grread nil 1 0) seem to do the same thing --
**  immediately return (5 (X Y Z)).  WE do that by setting bit 0
**  of the ADS track arg to 1 if the LISP track arg is non-nil
**  OR bit 0 of allkeys is set.
**
**  When allkeys has bit 1 set, moving the cursor over the pulldowns
**  returns type 13's that tell what pulldown you're hovering over;
**  that's not documented.
*/
    int msgn;
    int lisptrack,adstrack,type;
    long fl1;
    struct resbuf *adsres,*tp1;

    msgn=-1; lisptrack=0; adsres=NULL;

    adstrack=0;  /* The ADS variable -- not the LISP one. */
    type=0;      /* Default to normal cursor. */

    /* Create the resbuf for sds_grread() to fill: */
    if ((adsres=db_alloc_resbuf())==NULL)
        { msgn=7; goto out; }
    adsres->restype=RTLONG; adsres->resval.rlong=0L;  /* Safe default */

    if ((tp1=lsp_argsll)!=NULL) {
        if (tp1->restype!=RTNIL) lisptrack=1;
        if ((tp1=lsp_nextarg(tp1))!=NULL) {
            if (tp1->restype==RTLONG) {
                adstrack=(int)tp1->resval.rlong;
            } else if (tp1->restype==RTSHORT) {
                adstrack=(int)tp1->resval.rint;
            } else if (tp1->restype==RTREAL || tp1->restype==RTANG ||
                tp1->restype==RTORINT) {

                adstrack=(int)tp1->resval.rreal;
            } else {
                msgn=1; goto out;
            }
            if ((tp1=lsp_nextarg(tp1))!=NULL) {
                if (tp1->restype==RTLONG) type=(int)tp1->resval.rlong;
                else if (tp1->restype==RTSHORT) type=(int)tp1->resval.rint;
                else { msgn=1; goto out; }
                if (lsp_nextarg(tp1)!=NULL) { msgn=9; goto out; }
            }
        }
    }
    adstrack|=lisptrack;

    if (sds_grread(adstrack,&type,adsres)!=RTNORM)
        { msgn=(adstrack&8) ? 45 : 20; goto out; }

    if (adsres->restype==RTSHORT) {  /* Keep integers as longs */
        fl1=(long)adsres->resval.rint;  /* Don't try intra-union assignment. */
        adsres->restype=RTLONG;
        adsres->resval.rlong=fl1;
    }

    /* Convert pts to list-form: */
    if (lsp_convptlist(adsres,1)) { msgn=7; goto out; }

    /* Set the result to a list containing type and adsres: */
    lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value */
                                /* (since we're not doing an sds_ret...). */
    lsp_evres.restype=RTLB;
    lsp_evres.resval.rstring=NULL;
    tp1=&lsp_evres;
    if ((tp1->rbnext=db_alloc_resbuf())==NULL)
        { msgn=7; goto out; }
    tp1=tp1->rbnext;
    tp1->restype=RTLONG;
    tp1->resval.rlong=(long)type;
    tp1->rbnext=adsres;
    adsres=NULL;  /* Important */
    while (tp1->rbnext!=NULL) tp1=tp1->rbnext;
    if ((tp1->rbnext=db_alloc_resbuf())==NULL)
        { msgn=7; goto out; }
    tp1=tp1->rbnext;
    tp1->restype=RTLE;
    tp1->resval.rstring=NULL;

out:
    if (adsres!=NULL) { lsp_freesuprb(adsres); adsres=NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_redraw(void) {
    int msgn,mode;

    msgn=-1;
    mode=IC_REDRAW_DRAW;  /* Default is redraw entity */

    if (lsp_argsll!=NULL) {
        if (lsp_argsll->restype!=RTENAME) { msgn=1; goto out; }
        if (lsp_argsll->rbnext!=NULL) {
            if (lsp_argsll->rbnext->restype==RTLONG)
                mode=(int)lsp_argsll->rbnext->resval.rlong;
            else if (lsp_argsll->rbnext->restype==RTSHORT)
                mode=lsp_argsll->rbnext->resval.rint;
            else { msgn=1; goto out; }
            if (lsp_argsll->rbnext->rbnext!=NULL) { msgn=9; goto out; }
        }
    }

    /* LISP does not draw with mode==0, ADS does: */
    if (mode) sds_redraw((lsp_argsll==NULL) ?
        NULL : lsp_argsll->resval.rlname,mode);

    lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_grclear(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll!=NULL) { msgn=9; goto out; }
    sds_grclear(); lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}


