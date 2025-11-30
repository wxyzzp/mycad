/* C:\ICAD\PRJ\LIB\LISP\LIST.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/


int lsp_1list(int op) {
    int msgn;
    int fint1;
    struct resbuf *thismem,*nextmem;

    msgn=-1;

    if (lsp_argsll==NULL)                  { msgn=2;         goto out; }
    if (lsp_nextarg(lsp_argsll)!=NULL)     { msgn=9;         goto out; }
    if (lsp_argsll->restype==RTNIL)
        { if (op) lsp_retint(0); else lsp_retnil(); goto out; }
    if (lsp_argsll->restype!=RTLB)         { msgn=1;         goto out; }
    if (lsp_argsll->rbnext==NULL)          { msgn=5;         goto out; }
    if (lsp_argsll->rbnext->restype==RTLE)
        { if (op) lsp_retint(0); else lsp_retnil(); goto out; }

    for (thismem=lsp_argsll->rbnext,fint1=1L; thismem->restype!=
        RTDOTE && (nextmem=lsp_nextarg(thismem))!=NULL &&
        nextmem->restype!=RTLE; thismem=nextmem,fint1++);
    if (thismem->restype==RTDOTE) { msgn=21; goto out; }

    if (op) lsp_retint(fint1);
    else if (lsp_retval(thismem)!=RTNORM) msgn=7;

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_last(void)   { return lsp_1list(0); }
int lsp_length(void) { return lsp_1list(1); }

int lsp_car(void) { return lsp_simplecxr(0); }
int lsp_cdr(void) { return lsp_simplecxr(1); }

int lsp_simplecxr(int op) {
    int msgn;
    struct resbuf *back1,*tp1;

    msgn=-1;

    if (lsp_argsll==NULL)                  { msgn=2;       goto out; }
    if (lsp_argsll->restype==RTNIL)        { lsp_retnil(); goto out; }
    if (lsp_argsll->restype!=RTLB)         { msgn=1;       goto out; }
    if (lsp_nextarg(lsp_argsll)!=NULL)     { msgn=9;       goto out; }
    if (lsp_argsll->rbnext==NULL)          { msgn=5;       goto out; }
    if (lsp_argsll->rbnext->restype==RTLE) { lsp_retnil(); goto out; }

    if (op) {  /* cdr */

        /* Set a ptr to the second member of the list, and a ptr to */
        /* one link before it: */
        if ((back1=lsp_argend(lsp_argsll->rbnext,NULL))==NULL ||
            (tp1=back1->rbnext)==NULL) { msgn=5; goto out; }

        /* If there ISN'T a 2nd member (we hit the RTLE), return nil: */
        if (tp1->restype==RTLE) { lsp_retnil(); goto out; }

        if (tp1->restype==RTDOTE) {  /* 2nd member's a dot. */

            /* Step to 3rd arg and take it: */
            if ((tp1=lsp_nextarg(tp1))==NULL || tp1->restype==RTLE)
                { msgn=5; goto out; }
            if (lsp_retval(tp1)!=RTNORM) { msgn=7; goto out; }

        } else {  /* 2nd member is something other than a dot. */

            /* Steal everything (including the RTLE) beginning with */
            /* the 2nd member: */

            lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value. */

            /* Set 1st link to RTLB and link chain onto it: */
            lsp_evres.restype=RTLB; lsp_evres.rbnext=tp1;
            lsp_evres.resval.rstring=NULL;
            back1->rbnext=NULL;  /* Important! */

            /* Set the RTLB's rstring to RTNIL to avoid lsp_argend() problems: */
            lsp_argsll->resval.rstring=NULL;

        }

    } else {  /* car */

        /* Since sds_retval() only takes the first arg in a llist of them, */
        /* just pass it the llist starting at the first member of the list: */
        if (lsp_retval(lsp_argsll->rbnext)!=RTNORM) { msgn=7; goto out; }

    }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_caar(void)   { return lsp_complexcxr("AA"/*DNT*/);   }
int lsp_cadr(void)   { return lsp_complexcxr("AD"/*DNT*/);   }
int lsp_cdar(void)   { return lsp_complexcxr("DA"/*DNT*/);   }
int lsp_cddr(void)   { return lsp_complexcxr("DD"/*DNT*/);   }
int lsp_caaar(void)  { return lsp_complexcxr("AAA"/*DNT*/);  }
int lsp_caadr(void)  { return lsp_complexcxr("AAD"/*DNT*/);  }
int lsp_cadar(void)  { return lsp_complexcxr("ADA"/*DNT*/);  }
int lsp_caddr(void)  { return lsp_complexcxr("ADD"/*DNT*/);  }
int lsp_cdaar(void)  { return lsp_complexcxr("DAA"/*DNT*/);  }
int lsp_cdadr(void)  { return lsp_complexcxr("DAD"/*DNT*/);  }
int lsp_cddar(void)  { return lsp_complexcxr("DDA"/*DNT*/);  }
int lsp_cdddr(void)  { return lsp_complexcxr("DDD"/*DNT*/);  }
int lsp_caaaar(void) { return lsp_complexcxr("AAAA"/*DNT*/); }
int lsp_caaadr(void) { return lsp_complexcxr("AAAD"/*DNT*/); }
int lsp_caadar(void) { return lsp_complexcxr("AADA"/*DNT*/); }
int lsp_caaddr(void) { return lsp_complexcxr("AADD"/*DNT*/); }
int lsp_cadaar(void) { return lsp_complexcxr("ADAA"/*DNT*/); }
int lsp_cadadr(void) { return lsp_complexcxr("ADAD"/*DNT*/); }
int lsp_caddar(void) { return lsp_complexcxr("ADDA"/*DNT*/); }
int lsp_cadddr(void) { return lsp_complexcxr("ADDD"/*DNT*/); }
int lsp_cdaaar(void) { return lsp_complexcxr("DAAA"/*DNT*/); }
int lsp_cdaadr(void) { return lsp_complexcxr("DAAD"/*DNT*/); }
int lsp_cdadar(void) { return lsp_complexcxr("DADA"/*DNT*/); }
int lsp_cdaddr(void) { return lsp_complexcxr("DADD"/*DNT*/); }
int lsp_cddaar(void) { return lsp_complexcxr("DDAA"/*DNT*/); }
int lsp_cddadr(void) { return lsp_complexcxr("DDAD"/*DNT*/); }
int lsp_cdddar(void) { return lsp_complexcxr("DDDA"/*DNT*/); }
int lsp_cddddr(void) { return lsp_complexcxr("DDDD"/*DNT*/); }

int lsp_complexcxr(char *op) {
/*
**  This function uses the CAR and CDR functions to perform the complex
**  combinations of these (like CAADAR).  It manufactures an expression
**  to perform complex CAR/CDR combinations and then calls lsp_lispev1()
**  to evaluated it (which is indirect recursion).
**
**  op is expected to be a string of 'A's and/or 'D's -- the complex
**  function name without the C and the R (all caps).
**
**  Example:
**      If op points to AADA, the following expression is created
**      (as a llist of super resbufs) and given to lsp_lispev1() to
**      evaluate:
**
**          (CAR(CAR(CDR(CAR(QUOTE([lsp_argsll])))))
**
**      where [lsp_argsll] respresents a copy of the current lsp_argsll
**      links.  (This example is CAADAR.)  The QUOTE function is needed
**      so that the arg is not evaluated; we want to use arg in
**      lsp_argsll -- not what it evaluates to.  (lsp_argsll should have
**      only one arg in it (a nil or a list), which is fine with
**      the QUOTE function.)
**
*/
    char *thisop;
    int nops,msgn,fi1;
    struct resbuf *chain,*pchain,*tp1;

    msgn=-1; nops=0; chain=pchain=NULL;

    /* Check the args here, too.  (The QUOTE function's messages aren't */
    /* quite the same, or we could let it fall through.) */
    if (lsp_argsll==NULL)                  { msgn=2;       goto out; }
    if (lsp_argsll->restype==RTNIL)        { lsp_retnil(); goto out; }
    if (lsp_argsll->restype!=RTLB)         { msgn=1;       goto out; }
    if (lsp_nextarg(lsp_argsll)!=NULL)     { msgn=9;       goto out; }
    if (lsp_argsll->rbnext==NULL)          { msgn=5;       goto out; }
    if (lsp_argsll->rbnext->restype==RTLE) { lsp_retnil(); goto out; }

    /* Build the chain of function calls: */
    for (thisop=op; *thisop; thisop++) {
        if (*thisop!='A'/*DNT*/ && *thisop!='D'/*DNT*/) continue;

        /* Make the RTLB: */
        if ((tp1=db_alloc_resbuf())==NULL)
            { msgn=7; goto out; }
        tp1->restype=RTLB;
        tp1->resval.rstring=NULL;
        if (chain==NULL) chain=tp1; else pchain->rbnext=tp1;
        pchain=tp1;

        /* Make the CAR or CDR symbol link: */
        if ((tp1=db_alloc_resbuf())==NULL)
            { msgn=7; goto out; }
        if ((tp1->resval.rstring= new char [4])==NULL)
            { 
			db_free_resbuf(tp1); 
			msgn=7; 
			goto out; 
			}
        tp1->restype=LSP_RTSYMB;
        strcpy(tp1->resval.rstring,"CAR"/*DNT*/);
        if (*thisop=='D'/*DNT*/) tp1->resval.rstring[1]='D'/*DNT*/;
        pchain->rbnext=tp1; pchain=tp1;

        nops++;
    }

    if (nops<1 || pchain==NULL) { msgn=17; goto out; }  /* Our fault */

    /* Add the QUOTE function call: */
    if ((pchain->rbnext=db_alloc_resbuf())==NULL) { msgn=7; goto out; }
    pchain=pchain->rbnext;
    pchain->restype=RTLB;
    pchain->resval.rstring=NULL;
    if ((pchain->rbnext=db_alloc_resbuf())==
        NULL) { msgn=7; goto out; }
    pchain=pchain->rbnext;
    pchain->restype=LSP_RTSYMB;
    if ((pchain->resval.rstring= new char [6])==NULL)
        { msgn=7; goto out; }
    strcpy(pchain->resval.rstring,"QUOTE"/*DNT*/);

    if (lsp_argsll!=NULL) {
        /* Add a copy of lsp_argsll (make 1st link for lsp_copysplrb() */
        /* to use): */
        if ((pchain->rbnext=db_alloc_resbuf())==NULL) { msgn=7; goto out; }
        if (lsp_copysplrb(pchain->rbnext,lsp_argsll)) { msgn=7; goto out; }
        while (pchain->rbnext!=NULL) pchain=pchain->rbnext;  /* Step to end */
    }

    /* Add the RTLEs: */
    for (fi1=0; fi1<=nops; fi1++) {  /* Note <=; 1 extra for the QUOTE fn */
        if ((pchain->rbnext=db_alloc_resbuf())==NULL) { msgn=7; goto out; }
        pchain=pchain->rbnext;
        pchain->restype=RTLE;
        pchain->resval.rstring=NULL;
    }

    /* Call lsp_lispev1() to eval this manufactured expression: */
    if (lsp_lispev1(chain,NULL,lsp_fnvarval))
        { msgn=-2; goto out; }  /* Avoid another message */

out:
    if (chain!=NULL) { lsp_freesuprb(chain); chain=pchain=NULL; }
    if (msgn!=-1) {
        if (msgn>-1) lsp_fnerrmsg(msgn);
        return -1;
    }
    return 0;
}

int lsp_cons(void) {
    int msgn,fi1;
    struct resbuf *endp,*firstp,*lastp,*argp[2],*tp1,*tp2;

    msgn=-1;


    if ((argp[0]=lsp_argsll)==NULL || (argp[1]=lsp_nextarg(argp[0]))==NULL)
        { msgn=2; goto out; }
    if (lsp_nextarg(argp[1])!=NULL) { msgn=9; goto out; }

    /* WATCH OUT; for the sake of lsp_copysplrb(), we're not freeing */
    /* lsp_evres here. */

    /* Free contents of 1st link of lsp_evres and start it with RTLB: */
    if ((fi1=ic_resval(lsp_evres.restype))==RTSTR ||
        lsp_evres.restype==LSP_RTSYMB) {

        if (lsp_evres.resval.rstring!=NULL) IC_FREE(lsp_evres.resval.rstring);
    } else if (fi1==RTVOID && lsp_evres.resval.rbinary.buf!=NULL)
        IC_FREE(lsp_evres.resval.rbinary.buf);
    lsp_evres.restype=RTLB;
    lsp_evres.resval.rstring=NULL;  /* See lsp_argend(). */

    /* Make the 1st link for lsp_copysplrb() to use: */
    if (lsp_evres.rbnext==NULL &&
        (lsp_evres.rbnext=db_alloc_resbuf())==NULL) { msgn=7; goto out; }

    /* Find end of 1st arg, terminate temporarily, and copy: */
    if ((tp1=lsp_argend(argp[0],NULL))==NULL) { msgn=5; goto out; }  /* Should never. */
    tp2=tp1->rbnext; tp1->rbnext=NULL;
    fi1=lsp_copysplrb(lsp_evres.rbnext,argp[0]);
    tp1->rbnext=tp2;
    if (fi1) { msgn=7; goto out; }

    /* Now lsp_evres should be properly terminated again (after the */
    /* lsp_copysplrb()). */

    /* Find last link created: */
    endp=lsp_evres.rbnext; while (endp->rbnext!=NULL) endp=endp->rbnext;

    /* If 2nd arg isn't nil or a list, add a dot: */
    if (argp[1]->restype!=RTNIL && argp[1]->restype!=RTLB) {
        if ((endp->rbnext=db_alloc_resbuf())==NULL) { msgn=7; goto out; }
        endp=endp->rbnext;
        endp->restype=RTDOTE;
    }

    /* If 2nd arg isn't nil, add the element(s): */
    if (argp[1]->restype!=RTNIL) {

        firstp=lastp=argp[1];
        if (firstp->restype==RTLB) {

            /* If it's an empty list, take nothing: */
            if ((firstp=firstp->rbnext)!=NULL && firstp->restype==RTLE)
                firstp=NULL;

            lastp=NULL;
            if (firstp!=NULL && (lsp_argend(argp[1],&lastp)==NULL ||
                lastp==NULL)) { msgn=5; goto out; }

        }

        if (firstp!=NULL && lastp!=NULL) {

            /* Make the 1st link for lsp_copysplrb() to use: */
            if ((endp->rbnext=db_alloc_resbuf())==NULL) { msgn=7; goto out; }
            endp=endp->rbnext;

            /* Chop, copy, repair. */
            tp1=lastp->rbnext; lastp->rbnext=NULL;
            fi1=lsp_copysplrb(endp,firstp);
            lastp->rbnext=tp1;
            if (fi1) { msgn=7; goto out; }

            while (endp->rbnext!=NULL) endp=endp->rbnext;
        }

    }

    /* Add list-end: */
    if ((endp->rbnext=db_alloc_resbuf())==NULL) { msgn=7; goto out; }
    endp=endp->rbnext;
    endp->restype=RTLE;
    endp->resval.rstring=NULL;

out:
    if (msgn>-1)
        { lsp_freesplrb(&lsp_evres); lsp_fnerrmsg(msgn); return -1; }

    return 0;
}

int lsp_append(void) {
    int msgn;
    struct resbuf *thisarg,*chain,*pchain,*subchain,*back1,*savlink,*tp1;

    msgn=-1; chain=pchain=subchain=NULL;

    /* Each arg must be a list or nil.  Build a chain of all list members: */
    for (thisarg=lsp_argsll; thisarg!=NULL; thisarg=lsp_nextarg(thisarg)) {

        if (thisarg->restype==RTNIL) continue;  /* Nothing to add, but ok. */
        if (thisarg->restype!=RTLB) { msgn=1; goto out; }
        if (thisarg->rbnext==NULL)  { msgn=5; goto out; }
        if (thisarg->rbnext->restype==RTLE) continue;  /* () is nil */

        /* Get the link 1 before the RTLE: */
        if (lsp_argend(thisarg,&back1)==NULL || back1==NULL)
            { msgn=5; goto out; }

        /* Chop temporarily; don't leave until repaired at end of block: */
        {
            savlink=back1->rbnext; back1->rbnext=NULL;

            /*
            ** Check for dots.  No dotted pairs allowed at this level
            ** (although the MEMBERS THEMSELVES can be dotted pairs). That
            ** is, we can take members from ((1 . 2)(3 . 4)), but not from
            ** (1 . 2) because the former list is not a dotted pair itself.
            */
            for (tp1=thisarg->rbnext; tp1!=NULL &&
                tp1->restype!=RTDOTE; tp1=lsp_nextarg(tp1));
            if (tp1!=NULL) msgn=21;

            /* Make 1st link for lsp_copysplrb(): */
            if (msgn<0 && (subchain=db_alloc_resbuf())==NULL) msgn=7;

            /* Copy the members: */
            if (msgn<0 && lsp_copysplrb(subchain,thisarg->rbnext)) msgn=7;

            back1->rbnext=savlink;  /* Repair */
        }

        if (msgn>-1) goto out;  /* NOW we can leave. */

        /* Link in, set pchain to last link: */
        if (chain==NULL) chain=subchain; else pchain->rbnext=subchain;
        pchain=subchain; while (pchain->rbnext!=NULL) pchain=pchain->rbnext;
        subchain=NULL;  /* Important. */

    }

    if (chain==NULL) { lsp_retnil(); goto out; }  /* No members found. */

    /* Make the RTLE: */
    if ((pchain->rbnext=db_alloc_resbuf())==
        NULL) { msgn=7; goto out; }
    pchain=pchain->rbnext;
    pchain->restype=RTLE;
    pchain->resval.rstring=NULL;

    lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value */
                                 /* (since we're not doing an sds_ret...). */

    /* Start with RTLB, add the chain just created: */
    lsp_evres.restype=RTLB;
    lsp_evres.resval.rstring=NULL;
    lsp_evres.rbnext=chain;
    chain=NULL;  /* Important. */

out:
    if (chain!=NULL)    { lsp_freesuprb(chain);    chain=pchain=NULL; }
    if (subchain!=NULL) { lsp_freesuprb(subchain);     subchain=NULL; }

    if (msgn>-1)
        { lsp_freesplrb(&lsp_evres); lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_assoc(void) {
    int msgn;
    struct resbuf *argp[2],*thismem,*tp1;

    msgn=-1;

    if ((argp[0]=lsp_argsll)==NULL ||
        (argp[1]=lsp_nextarg(argp[0]))==NULL) { msgn=2;       goto out; }
    if (lsp_nextarg(argp[1])!=NULL)           { msgn=9;       goto out; }
    if (argp[1]->restype==RTNIL)              { lsp_retnil(); goto out; }
    if (argp[1]->restype!=RTLB)               { msgn=1;       goto out; }
    if (argp[1]->rbnext==NULL)                { msgn=5;       goto out; }
    if (argp[1]->rbnext->restype==RTLE)       { lsp_retnil(); goto out; }

    /* Check each member of the main list: */
    for (thismem=argp[1]->rbnext; thismem!=NULL &&
        thismem->restype!=RTLE; thismem=lsp_nextarg(thismem)) {

        /* Check for a list.  (AutoLISP does it this way, too.  It doesn't */
        /* give Bad assoc list until it walks into a non-list item.) */
        if (thismem->restype!=RTLB) { msgn=22; goto out; }
        if (thismem->rbnext==NULL)  { msgn= 5; goto out; }
        if (thismem->rbnext->restype==RTLE) { msgn=22; goto out; }  /* nil */

        /* Check 1st arg against 1st member of sublist: */
        if (!lsp_argcomp(thismem->rbnext,argp[0],0.0)) break;
    }

    if (thismem==NULL || thismem->restype==RTLE) lsp_retnil();
    else {
        if ((tp1=lsp_argend(thismem,NULL))==NULL) { msgn=5; goto out; }
        /* It's a list, so just steal it. */
        lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value. */
        lsp_evres.restype=RTLB; lsp_evres.rbnext=thismem->rbnext;
        lsp_evres.resval.rstring=NULL;
        thismem->rbnext=tp1->rbnext;  /* Important! */
        tp1->rbnext=NULL;
        thismem->resval.rstring=NULL;
    }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_reverse(void) {
    int msgn;
    struct resbuf *chain,*subchain,*savlink,*tp1;
    struct resbuf *thismem,*nextmem,*memend;

    msgn=-1; chain=subchain=savlink=thismem=nextmem=memend=NULL;

    if (lsp_argsll==NULL)                  { msgn=2;       goto out; }
    if (lsp_nextarg(lsp_argsll)!=NULL)     { msgn=9;       goto out; }
    if (lsp_argsll->restype==RTNIL)        { lsp_retnil(); goto out; }
    if (lsp_argsll->restype!=RTLB)         { msgn=1;       goto out; }
    if (lsp_argsll->rbnext==NULL)          { msgn=5;       goto out; }
    if (lsp_argsll->rbnext->restype==RTLE) { lsp_retnil(); goto out; }

    lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value */
                                /* (since we're not doing an sds_ret...). */

    /* Start the chain with RTLE (since we'll build it backwards): */
    if ((chain=db_alloc_resbuf())==NULL)
        { msgn=7; goto out; }
    chain->restype=RTLE;
    chain->resval.rstring=NULL;

    /* Walk the members and build the chain backwards: */
    for (thismem=lsp_argsll->rbnext; thismem!=NULL &&
        thismem->restype!=RTLE; thismem=nextmem) {

        /* No dots at this level: */
        if (thismem->restype==RTDOTE) { msgn=21; goto out; }

        /* Find the end of this member and beginning of the next: */
        if ((memend=lsp_argend(thismem,NULL))==NULL) { msgn=5; goto out; }
        nextmem=memend->rbnext;

        /* Chop out this member and take it; don't leave before */
        /* repairing at the end of this block: */
        {
            savlink=memend->rbnext; memend->rbnext=NULL;

            /* Make first link for lsp_copysplrb() and copy the member: */
            if ((subchain=db_alloc_resbuf())==
                NULL || lsp_copysplrb(subchain,thismem)) msgn=7;
            else {
                /* Build the chain in reverse order: */
                tp1=subchain; while (tp1->rbnext!=NULL) tp1=tp1->rbnext;
                tp1->rbnext=chain; chain=subchain;
                subchain=NULL;  /* Important */
            }

            memend->rbnext=savlink;  /* Repair */

            if (msgn>-1) goto out;  /* NOW we can leave. */
        }

    }

    /* Make lsp_evres the RTLB link and tack on the chain: */
    lsp_evres.restype=RTLB;
    lsp_evres.resval.rstring=NULL;
    lsp_evres.rbnext=chain;
    chain=NULL;  /* Important */

out:
    if (chain!=NULL) { lsp_freesuprb(chain); chain=NULL; }
    if (subchain!=NULL) { lsp_freesuprb(subchain); subchain=NULL; }

    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_listp(void) {
    int msgn;

    msgn=-1;
    if (lsp_argsll==NULL) { msgn=2; goto out; }
    if (lsp_argsll->restype==RTLB || lsp_argsll->restype==RTNIL) lsp_rett();
    else lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_member(void) {
    int msgn;
    struct resbuf *argp[2],*thismem,*prev;

    msgn=-1; 

    if ((argp[0]=lsp_argsll)==NULL ||
        (argp[1]=lsp_nextarg(argp[0]))==NULL) { msgn=2;       goto out; }
    if (lsp_nextarg(argp[1])!=NULL)           { msgn=9;       goto out; }
    if (argp[1]->restype==RTNIL)              { lsp_retnil(); goto out; }
    if (argp[1]->restype!=RTLB)               { msgn=1;       goto out; }
    if (argp[1]->rbnext==NULL)                { msgn=5;       goto out; }
    if (argp[1]->rbnext->restype==RTLE)       { lsp_retnil(); goto out; }

    /* Walk the members and look for a match: */
    for (prev=argp[1],thismem=prev->rbnext; thismem!=NULL; prev=
        lsp_argend(thismem,NULL),thismem=(prev==NULL ||
        prev->rbnext->restype==RTLE) ? NULL : prev->rbnext) {

        /* AutoLISP doesn't gripe until it hits the dot: */
        if (thismem->restype==RTDOTE) { msgn=21; goto out; }

        if (!lsp_argcomp(argp[0],thismem,0.0)) break;
    }
    if (thismem==NULL) lsp_retnil();
    else {  /* Steal links from thismem through the list's RTLE: */
        lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value. */
        /* Set 1st link to RTLB and link chain onto it: */
        lsp_evres.restype=RTLB; lsp_evres.rbnext=thismem;
        lsp_evres.resval.rstring=NULL;
        prev->rbnext=NULL;  /* Important! */

        /* Avoid lsp_argend() problems with the broken list: */
        argp[1]->resval.rstring=NULL;
    }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_nth(void) {
    int msgn;
    long idx;
    struct resbuf *argp[2],*thismem,*tp1;

    msgn=-1;

    if ((argp[0]=lsp_argsll)==NULL ||
        (argp[1]=lsp_nextarg(argp[0]))==NULL) { msgn= 2; goto out; }
    if (lsp_nextarg(argp[1])!=NULL)           { msgn= 9; goto out; }
    if (argp[0]->restype!=RTLONG &&
        argp[0]->restype!=RTSHORT)            { msgn= 1; goto out; }
    if (argp[1]->restype==RTNIL)              { msgn=23; goto out; }
    if (argp[1]->restype!=RTLB)               { msgn= 1; goto out; }
    if (argp[1]->rbnext==NULL)                { msgn= 5; goto out; }
    if (argp[1]->rbnext->restype==RTLE)       { msgn=23; goto out; } /* () */

    /* Get and check the index number: */
    idx=(argp[0]->restype==RTLONG) ?
        argp[0]->resval.rlong : (long)argp[0]->resval.rint;
    if (idx<0L) { msgn=23; goto out; }

    /* Walk to the right member (and watch for dots): */
    for (thismem=argp[1]->rbnext; idx>0L && thismem!=NULL &&
        thismem->restype!=RTLE && thismem->restype!=RTDOTE; thismem=
        lsp_nextarg(thismem),idx--);
    if (thismem==NULL || thismem->restype==RTLE) { lsp_retnil(); goto out; }
    if (thismem->restype==RTDOTE)                { msgn=23;      goto out; }

    /* Return the member: */
    if (thismem->restype==RTLB) {  /* If it's a list, steal it. */
        if ((tp1=lsp_argend(thismem,NULL))==NULL) { msgn=5; goto out; }
        lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value. */
        lsp_evres.restype=RTLB; lsp_evres.rbnext=thismem->rbnext;
        lsp_evres.resval.rstring=NULL;
        thismem->rbnext=tp1->rbnext;  /* Important! */
        tp1->rbnext=NULL;
        thismem->resval.rstring=NULL;
    } else if (lsp_retval(thismem)!=RTNORM) { msgn=7; goto out; }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_subst(void) {
    int msgn;
    struct resbuf *argp[3],*thismem,*newcopy,*end1,*savlink;
    struct resbuf *prevend,*thisend,*newend;


    msgn=-1; newcopy=end1=savlink=NULL;

    lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value */

    if ((argp[0]=lsp_argsll)==NULL || (argp[1]=lsp_nextarg(argp[0]))==NULL ||
        (argp[2]=lsp_nextarg(argp[1]))==NULL) { msgn=2;       goto out; }
    if (lsp_nextarg(argp[2])!=NULL)           { msgn=9;       goto out; }
    if (argp[2]->restype==RTNIL)                              goto out;
    if (argp[2]->restype!=RTLB)               { msgn=1;       goto out; }
    if (argp[2]->rbnext==NULL)                { msgn=5;       goto out; }
    if (argp[2]->rbnext->restype==RTLE)                       goto out;

    /* Break lsp_argsll temporarily after the 1st arg so that it can be */
    /* copied easily: */
    if ((end1=lsp_argend(argp[0],NULL))==NULL) { msgn=5; goto out; }
    savlink=end1->rbnext; end1->rbnext=NULL;

    /* Steal the original list from argp[2] for lsp_evres: */
    lsp_evres.restype=RTLB;
    lsp_evres.resval.rstring=NULL;
    lsp_evres.rbnext=argp[2]->rbnext;
    argp[2]->rbnext=NULL;
    argp[2]->resval.rstring=NULL;

    /* Now walk lsp_evres and substitute where necessary: */
    for (prevend=&lsp_evres,thismem=prevend->rbnext,
        thisend=NULL; thismem!=NULL &&
        thismem->restype!=RTLE; prevend=thisend,thismem=prevend->rbnext) {

        if ((thisend=lsp_argend(thismem,NULL))==NULL) { msgn= 5; goto out; }
        if (thismem->restype==RTDOTE)                 { msgn=21; goto out; }
        if (lsp_argcomp(argp[1],thismem,0.0)) continue;  /* No match */

        /* Make 1st link and copy argp[0]: */
        if (newcopy!=NULL) { lsp_freesuprb(newcopy); newcopy=NULL; }
        if ((newcopy=db_alloc_resbuf())==NULL ||
            lsp_copysplrb(newcopy,argp[0])) { msgn=7; goto out; }
        newend=newcopy; while (newend->rbnext!=NULL) newend=newend->rbnext;

        /* Link it in, free the old one, adjust pointers: */
        prevend->rbnext=newcopy; newend->rbnext=thisend->rbnext;
        thisend->rbnext=NULL; lsp_freesuprb(thismem);
        thismem=newcopy; thisend=newend;
        newcopy=NULL;  /* Important */
    }

out:
    if (savlink!=NULL && end1!=NULL) { end1->rbnext=savlink; savlink=NULL; }
    if (newcopy!=NULL) { lsp_freesuprb(newcopy); newcopy=NULL; }

    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_list(void) {
    int msgn;
    struct resbuf *tp1;

    msgn=-1;

    /* Steal lsp_argsll instead of using lsp_copysplrb() or lsp_retlist(): */

	// Modified CyberAge AP 10/18/2000 [
	extern int LSP_ConvertToPoint; // Reason: Fix for bug no. 57282 and CADR problem.
	if(LSP_ConvertToPoint != 2) // i.e. called by internalGetpoint(...)
		LSP_ConvertToPoint = 0; // Reason : Fix for bug no. 57282
	// Modified CyberAge AP 10/18/2000 ]
    lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value. */

    if (lsp_argsll==NULL) goto out;  /* Leaves it nil */

    /* Find the end: */
    tp1=lsp_argsll; while (tp1->rbnext!=NULL) tp1=tp1->rbnext;

    /* Make the RTLE link: */
    if ((tp1->rbnext=db_alloc_resbuf())==NULL)
        { msgn=7; goto out; }
    tp1->rbnext->restype=RTLE;
    tp1->rbnext->resval.rstring=NULL;

    /* Set 1st link to RTLB and link lsp_argsll onto it: */
    lsp_evres.restype=RTLB; lsp_evres.rbnext=lsp_argsll;
    lsp_evres.resval.rstring=NULL;
    lsp_argsll=NULL;  /* Important! */

out:  /* Looks stupid here, but preserve for uniformity. */
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

