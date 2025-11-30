/* C:\ICAD\PRJ\LIB\LISP\XDATA.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/

int lsp_xdroom(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL)             { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTENAME) { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)     { msgn=9; goto out; }

    lsp_freesplrb(&lsp_evres);  /* Default result is nil. */

    if (sds_xdroom(lsp_argsll->resval.rlname,
        &lsp_evres.resval.rlong)==RTNORM) lsp_evres.restype=RTLONG;

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_xdsize(void) {
    int msgn,fi1;
    struct resbuf *adsassoc,*tp1;

    msgn=-1; adsassoc=NULL;

    if (lsp_argsll==NULL)                  { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTLB)         { msgn=1; goto out; }
    if (lsp_nextarg(lsp_argsll)!=NULL)     { msgn=9; goto out; }
    if (lsp_argsll->rbnext==NULL)          { msgn=5; goto out; }
    if (lsp_argsll->rbnext->restype==RTLE) { msgn=1; goto out; }

    /*
    **  LISP xdsize requires a list of the form
    **      ([-3] (APP1 EED-DOTTED-PAIRS)...(APPn EED-DOTTED-PAIRS))
    **  If we make SURE the -3 is there and add an extra set of
    **  parentheses around the whole thing to get
    **      ((-3 (APP1 EED-DOTTED-PAIRS)...(APPn EED-DOTTED-PAIRS)))
    **  we can call lsp_convassoc() to convert it to ADS form
    **  (since it will look like a DXF assoc list).  Then we can
    **  call sds_xdsize() with the converted list.
    */

    if (lsp_argsll->rbnext->restype==RTLB) {  /* -3 missing; add it */
        if ((tp1=db_alloc_resbuf())==NULL)
            { msgn=7; goto out; }
        tp1->restype=RTLONG; tp1->resval.rlong=-3L;
        tp1->rbnext=lsp_argsll->rbnext;
        lsp_argsll->rbnext=tp1;
    }

    /* Add the surrounding parentheses: */
    if ((tp1=db_alloc_resbuf())==NULL)
        { msgn=7; goto out; }
    tp1->restype=RTLB;
    tp1->resval.rstring=NULL;
    tp1->rbnext=lsp_argsll; lsp_argsll=tp1;
    tp1=lsp_argsll; while (tp1->rbnext!=NULL) tp1=tp1->rbnext;
    if ((tp1->rbnext=db_alloc_resbuf())==NULL)
        { msgn=7; goto out; }
    tp1->rbnext->restype=RTLE;
    tp1->rbnext->resval.rstring=NULL;

    if ((fi1=lsp_convassoc(lsp_argsll,&adsassoc,0))==-2)
        { msgn=7; goto out; }
    else if (fi1) { msgn=49; goto out; }

    lsp_freesplrb(&lsp_evres);  /* Default result is nil. */

    if (sds_xdsize(adsassoc,&lsp_evres.resval.rlong)==RTNORM)
        lsp_evres.restype=RTLONG;

out:
    if (adsassoc!=NULL) { lsp_freesuprb(adsassoc); adsassoc=NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

