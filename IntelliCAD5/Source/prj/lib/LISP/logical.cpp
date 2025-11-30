/* C:\ICAD\PRJ\LIB\LISP\LOGICAL.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/

int lsp_tilde(void) {
    int msgn;
    int res;

    msgn=-1;

    if (lsp_argsll==NULL) { msgn=2; goto out; }
    if      (lsp_argsll->restype==RTLONG)
        res=(int)lsp_argsll->resval.rlong;
    else if (lsp_argsll->restype==RTSHORT)
        res=(int)lsp_argsll->resval.rint;
    else { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL) { msgn=9; goto out; }

    res=~res;

    lsp_retint(res);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}


int lsp_not(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL)              { msgn=2; goto out; }
    if (lsp_nextarg(lsp_argsll)!=NULL) { msgn=9; goto out; }

    if (lsp_argsll->restype==RTNIL) lsp_rett(); else lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_and(void) { return lsp_andor(1); }
int lsp_or(void)  { return lsp_andor(0); }

int lsp_andor(int mode) {
    int msgn;
    struct resbuf *argsll,*tp1;

    msgn=-1;

    /* This is a donteval function (see lsp_lispev1()). */
    /* Steal lsp_argsll for local use (and don't free it): */
    argsll=lsp_argsll; lsp_argsll=NULL;

    for (tp1=argsll; tp1!=NULL; tp1=lsp_nextarg(tp1)) {
        if ((msgn=lsp_evalarg(tp1,NULL))!=-1) goto out;
        if (mode==0 && lsp_evres.restype!=RTNIL) { lsp_rett();   goto out; }
        if (mode==1 && lsp_evres.restype==RTNIL) { lsp_retnil(); goto out; }
    }
    if (mode==0) lsp_retnil(); else lsp_rett();

out:
    if (msgn!=-1) {  /* May get -2 from lsp_evalarg(). */
        if (msgn>-1) lsp_fnerrmsg(msgn);
        return -1;
    }
    return 0;
}

int lsp_logand(void) { return lsp_andior(0); }
int lsp_logior(void) { return lsp_andior(1); }

int lsp_andior(int mode) {
    int msgn;
    int res,fint1;
    struct resbuf *thisarg;

    msgn=-1; res=0;

    /* (Don't need to use lsp_nextarg() since we check for integers.) */
    for (thisarg=lsp_argsll; thisarg!=NULL; thisarg=thisarg->rbnext) {

        if (thisarg->restype==RTREAL || thisarg->restype==RTANG ||
            thisarg->restype==RTORINT) { msgn=36; goto out; }
        if      (thisarg->restype==RTLONG)  fint1=(int)thisarg->resval.rlong;
        else if (thisarg->restype==RTSHORT) fint1=(int)thisarg->resval.rint;
        else { msgn=1; goto out; }

        if (thisarg==lsp_argsll) { res=fint1; continue; }

        if (mode) res|=fint1; else res&=fint1;

    }

    lsp_retint(res);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_boole(void) {
    int msgn,btype,b00,b01,b10,b11,bidx,nbits,fi1;
    int res,himask,fint1,fint2;
    struct resbuf *thisarg;

    msgn=-1; res=0;
    nbits=8*sizeof(int); himask=(1<<(nbits-1));

    if (lsp_argsll==NULL) { msgn=2; goto out; }
    if      (lsp_argsll->restype==RTLONG)  btype=(int)lsp_argsll->resval.rlong;
    else if (lsp_argsll->restype==RTSHORT) btype=(int)lsp_argsll->resval.rint;
    else { msgn=1; goto out; }
    if (btype<0 || btype>15) { msgn=37; goto out; }
    if (lsp_argsll->rbnext==NULL) { msgn=2; goto out; }

    /* Flag what we're looking for: */
    b00=((btype&8)!=0); b01=((btype&4)!=0);
    b10=((btype&2)!=0); b11=((btype&1)!=0);

    /* (Don't need to use lsp_nextarg() since we check for integers.) */
    for (thisarg=lsp_argsll->rbnext; thisarg!=NULL; thisarg=thisarg->rbnext) {

        /* Put next arg into fint1: */
        if      (thisarg->restype==RTLONG)  fint1=(int)thisarg->resval.rlong;
        else if (thisarg->restype==RTSHORT) fint1=(int)thisarg->resval.rint;
        else { msgn=1; goto out; }

        if (thisarg==lsp_argsll->rbnext) { res=fint1; continue; }

        /* The 1st operand is res; the 2nd is fint1. */

        fint2=0;  /* Result bits will be shifted in from the right. */
        for (bidx=0; bidx<nbits; bidx++) {  /* Each bit */

            /* Take the hi bits from the operands and form a number */
            /* from from 0-4 by placing them side by side: */
            fi1=(((res&himask)!=0)<<1)|((fint1&himask)!=0);

            /* See if we have a combo that should turn on a bit: */
            fi1=((fi1==0 && b00) || (fi1==1 && b01) || (fi1==2 && b10) ||
                 (fi1==3 && b11));

            fint2<<=1;   /* Shift 1 left to make room at the low bit: */
            fint2|=fi1;  /* Tack on the result bit. */

            res<<=1; fint1<<=1;  /* Shift the two operands 1 left. */

        }

        res=fint2;  /* Set the new result. */

    }

    lsp_retint(res);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}


