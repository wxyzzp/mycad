/* C:\ICAD\PRJ\LIB\LISP\MEM.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/

int lsp_expand(void) {
    int msgn;
    int fint1;

    msgn=-1;
    fint1=1;  /* Default */

    if (lsp_argsll!=NULL) {
        if      (lsp_argsll->restype==RTLONG)
            fint1=(int)lsp_argsll->resval.rlong;
        else if (lsp_argsll->restype==RTSHORT)
            fint1=(int)lsp_argsll->resval.rint;
        else { msgn=1; goto out; }

        /* It takes <=0, too (but seems to do nothing). */

        if (lsp_argsll->rbnext!=NULL) { msgn=9; goto out; }
    }

    lsp_retint(fint1);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_alloc(void) {
    int msgn,oldsz;

    msgn=-1; oldsz=lsp_allocsz;

    if (lsp_argsll==NULL) { msgn=2; goto out; }
    if      (lsp_argsll->restype==RTLONG)
        lsp_allocsz=(int)lsp_argsll->resval.rlong;
    else if (lsp_argsll->restype==RTSHORT)
        lsp_allocsz=lsp_argsll->resval.rint;
    else { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL) { msgn=9; goto out; }

    /* There's an apparent limit of 5460; negs are set to it, too: */
    if (lsp_allocsz<0 || lsp_allocsz>5460) lsp_allocsz=5460;

    lsp_retint(oldsz);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_clruser(void) {  /* Garbage collection */
    lsp_lispclean();  /* The next call to lsp_lispev1() will set the */
                     /* predefined fns and vars again. */
    lsp_retnil(); return 0;
}

int lsp_gc(void) { return 0; }

int lsp_vmon(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll!=NULL) { msgn=9; goto out; }
    lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_mem(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll!=NULL) { msgn=9; goto out; }

    sds_printf("\nNodes:       -\nFree nodes:  -\nSegments:    -"/*DNT*/
        "\nAllocate:    %d\nCollections: -\n"/*DNT*/,lsp_allocsz);

    lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

