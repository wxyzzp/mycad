/* C:\ICAD\PRJ\LIB\LISP\FUNCS.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/

int lsp_eval(void) {
    int msgn;
    struct resbuf *argsll;

    msgn=-1; argsll=NULL;

    if (lsp_argsll==NULL) { msgn=2; goto out; }

    /* Steal lsp_argsll for use as a local (because the recursion call to */
    /* lsp_lispev1() in lsp_evalarg() would destroy it): */
    argsll=lsp_argsll; lsp_argsll=NULL;

    if (lsp_nextarg(argsll)!=NULL) { msgn=9; goto out; }

    msgn=lsp_evalarg(argsll,NULL);

out:
    lsp_freesuprb(argsll); argsll=NULL;  /* The original lsp_argsll. */

    if (msgn!=-1) {  /* lsp_evalarg() may return -2. */
        if (msgn>-1) lsp_fnerrmsg(msgn);
        return -1;
    }
    return 0;
}

int lsp_defun(void) {
/*
**  Simple in concept, tricky in execution.  The defun function
**  basically defines an atom whose name comes from the 1st arg
**  and whose value is a list of all the remaining args.
**
**  The 2nd arg is the params/locals list; cleaning it up is a little
**  tricky.  All non-symbol members and duplicate symbol members
**  must be removed.  If a duplicate symbol is found in the args
**  part, that's an error.  See lsp_fixfnargs().  Also, we can't disturb
**  it in place; the args are part of xll in lsp_lispev1();
**  we have to make a copy.
*/
    int msgn;
    struct resbuf rtlblink,rtlelink,*parloc,*parlocend,*rest,*argsll,*tp1,*tp2;

    msgn=-1; parloc=NULL;
    rtlblink.rbnext=rtlelink.rbnext=NULL;
    rtlblink.resval.rstring=rtlelink.resval.rstring=NULL;
    rtlblink.restype=RTLB; rtlelink.restype=RTLE;


    /* This is a donteval function (see lsp_lispev1()). */
    /* Steal lsp_argsll for local use (and don't free it): */
    argsll=lsp_argsll; lsp_argsll=NULL;


    if (argsll==NULL)                           { msgn=2; goto out; }
    if (argsll->restype!=LSP_RTSYMB  ||
        argsll->resval.rstring==NULL ||
        argsll->resval.rstring[0]==0)           { msgn=1; goto out; }
    /* NOW we can check the rbnext: */
    if (argsll->rbnext==NULL)                   { msgn=2; goto out; }
    if (argsll->rbnext->restype!=RTLB &&
        argsll->rbnext->restype!=RTNIL)         { msgn=1; goto out; }
    if ((parlocend=
        lsp_argend(argsll->rbnext,NULL))==NULL) { msgn=5; goto out; }
    if ((rest=parlocend->rbnext)==NULL)         { msgn=2; goto out; }

    /*
    **  Check the params/locals list.  Remove non-symbols.  Gripe about
    **  dup args.  Remove local symbols that are dups of the arg
    **  symbols or other locals.  Compress to RTNIL if it becomes ().
    **  Note that we make a COPY first.  We don't dare disturb
    **  argsll because it points into xll in lsp_lispev1().
    */
    parlocend->rbnext=NULL;
    if ((parloc=db_alloc_resbuf())==NULL ||
        lsp_copysplrb(parloc,argsll->rbnext)) msgn=7;
    parlocend->rbnext=rest;
    if (msgn==-1) msgn=lsp_fixfnargs(parloc);
    if (msgn==-1 && parloc==NULL) msgn=5;
    if (msgn>-1) goto out;

    /*
    **  Now we need to prepare a value for lsp_defatom():
    **
    **      RTLB|parloc|rest|RTLE
    **
    **  Remember we can't disturb rest permanently (because it's
    **  part of xll).  We'll have to attach the RTLE, call
    **  lsp_defatom(), then remove the RTLE.
    */
    rtlblink.rbnext=parloc;
    tp1=parloc; while (tp1->rbnext!=NULL) tp1=tp1->rbnext;
    tp1->rbnext=rest;
    tp2=rest; while (tp2->rbnext!=NULL) tp2=tp2->rbnext;
    tp2->rbnext=&rtlelink;
    if (_tcsnicmp(argsll->resval.rstring, "C:"/*DNT*/, 2) == 0) {
        if (lsp_defatom(argsll->resval.rstring,-3,&rtlblink,1)) msgn=7;
    } else {
        if (lsp_defatom(argsll->resval.rstring,-2,&rtlblink,1)) msgn=7;
    }
    tp1->rbnext=tp2->rbnext=NULL;
    if (msgn>-1) goto out;

    /* Return the function name: */
    tp1=argsll->rbnext; argsll->rbnext=NULL;
    if (lsp_copysplrb(&lsp_evres,argsll)) msgn=7;
    argsll->rbnext=tp1;

out:
    if (parloc!=NULL) { lsp_freesuprb(parloc); parloc=NULL; }
    if (msgn>-1) { lsp_freesplrb(&lsp_evres); lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_progn(void) {

    if (lsp_argsll==NULL) lsp_retnil();

    /*
    **  lsp_lispev1() evaluates each arg as it builds lsp_argsll.
    **  So, progn itself is just a dummy function.
    **
    **  Counts on lsp_lispev1() leaving the last arg's value
    **  in lsp_evres.  While building lsp_argsll, lsp_lispev1()
    **  normally STEALS lists from lsp_evres.  There's code
    **  in lsp_lispev1() to make an exception in progn's case.
    */

    return 0;
}

int lsp_lambda(void) {
/*
**  Similar to defun (and even simpler).  All lambda does is set the
**  result to a list of all of the unevaluated args!  That is,
**  (lambda arg1 arg2 ...) results in (arg1 arg2 ...).
**  (It sort of does a CDR on itself to delete the lambda.)
**
**  Lambda is used to define a list-function for the list-executor,
**  (lsp_dolistfn()) so the first arg should be the params/locals list
**  -- but nothing insists on it and nothing cleans up a bad
**  params/locals list like defun does; that's up to the user.
**  Lambda just makes a list of the args.  That's all it does.
*/
    int setnil,msgn;
    struct resbuf *rtlelink,*argsll,*tp1;

    msgn=-1; rtlelink=NULL; setnil=0;

    /* This is a donteval function (see lsp_lispev1()). */
    /* Steal lsp_argsll for local use (and don't free it): */
    argsll=lsp_argsll; lsp_argsll=NULL;

    if (argsll==NULL) { setnil=1; goto out; }  /* No args means nil */

    /* Make the RTLE link now, so that if it fails, we have little */
    /* to undo: */
    if ((rtlelink=db_alloc_resbuf())==NULL)
        { msgn=7; setnil=1; goto out; }
    rtlelink->restype=RTLE;
    rtlelink->resval.rstring=NULL;

    lsp_evres.restype=RTLB;  /* Start list */
    lsp_evres.resval.rstring=NULL;

    /* Alloc the next link for lsp_copysplrb(): */
    if ((lsp_evres.rbnext=db_alloc_resbuf())==NULL) { msgn=7; setnil=1; goto out; }

    /* THIS time, we mean COPY -- don't steal it from argsll: */
    /* it's part of xll in lsp_lispev1(): */
    lsp_copysplrb(lsp_evres.rbnext,argsll);

    /* Find the end and add the RTLE link: */
    tp1=lsp_evres.rbnext; while (tp1->rbnext!=NULL) tp1=tp1->rbnext;
    tp1->rbnext=rtlelink;
    rtlelink=NULL;  /* Important */

out:
    if (setnil) lsp_freesplrb(&lsp_evres);  /* Free old result, set RTNIL. */
    if (rtlelink!=NULL)
		{
		db_free_resbuf(rtlelink); rtlelink=NULL;
		}
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_apply(void) {
/*
**  The APPLY function, take 3:
**
**  This function works as follows.
**
**  The args are pre-evaluated, as with normal internal LISP functions.
**  The required syntax is
**      (APPLY A B)
**  where A must eval to a symbol or list (SL) and B must eval to a list
**  or nil (L).
**
**  My first attempt was to create the expression
**      (SL L-elements)
**  and call lsp_lispev1() to evaluate it.  No good, because of a subtlety.
**
**  Consider (setq a 1 b 2 c 3)
**           (apply '+ (list a b c))     (Works)
**           (apply '+ '(a b c))         (Bad arg type)
**
**  My 1st method (calling the evaluator with (+ a b c) or (+ 1 2 3))
**  worked in both cases.
**
**  I thought AutoLISP was gathering the args in L and passes them
**  DIRECTLY to the function -- not going through the interpreter again.
**  Therefore, in the example, plus is being asked to add 3 symbols, not
**  3 numbers.  It's like trying (+ 'a 'b 'c).
**
**  Here's another, clearer example:
**      (setq c 3)
**      (apply '(lambda (x) (print x)) '(c)) prints the symbol C.
**      ((lambda (x) (print x)) c)           prints the integer 3.
**
**  For version 2, I therefore doctored lsp_argsll so that all
**  it had are the ELEMENTS of the 2nd arg's list, and then called the
**  internal LISP function or list-function just like lsp_lispev1() does.
**  That way, the args from the 2nd list avoid being evaluated.
**
**  STILL not quite right.  I finally tricked AutoLISP into telling
**  how IT handles APPLY.  The trick makes use of the fact that an
**  empty list-function, like ((X Y)) with only a parameters list,
**  returns the calling expression:
**
**      (SETQ L '((X Y)) A 1 B 2)  (l a b)
**
**  gives (L A B).
**
**  So, what does (APPLY 'L '(A B)) give?
**
**      (L (<Subr-QUOTE> A) (<Subr-QUOTE> B))
**
**  NOW we can see.  AutoLISP manufactures the above expression and
**  sends it to the evaluator.  We can do that, too.
**
**  For example, (APPLY '+ '(1 2 3)) is executed by manufacturing
**  and executing (+ (<Subr-QUOTE> 1) (<Subr-QUOTE> 2) (<Subr-QUOTE> 3)).
**
**  For some reason, (APPLY + '(1 2 3)) gives bad arg type in AutoLISP --
**  even though +-function-pointer should evaluate to itself (as in eval)
**  to make (<Subr-+> (<Subr-QUOTE> 1) (<Subr-QUOTE> 2) (<Subr-QUOTE> 3)).
**  But, to match AutoLISP's syntax, A must eval to a symbol or list
**  (even though a function pointer should work).
**
**  When A is a list, it can be used something like this:
**      (APPLY '(LAMBDA (X) (PRINT X)) '(5)) or
**      (APPLY ''((X) (PRINT X)) '(5))
**  both produce the list-function ((X) (PRINT X)) and execute it
**  with 5 as the single lsp_argsll link.
**
**  This logic explains why (APPLY 'LAMBDA '(1 2)) results in
**  ((<Subr-QUOTE> 1) (<Subr-QUOTE> 2)) instead of (1 2).
**  It's executing (LAMBDA (<Subr-QUOTE> 1) (<Subr-QUOTE> 2)) instead of
**  (LAMBDA 1 2).
*/
    int msgn,fi1;
    struct resbuf *argp[2],*end1,*expr,*last,*thismem,*xend,*tp1;

    msgn=-1; expr=NULL;

    /*
    **  Note that we don't use lsp_nextarg() to get the 2nd arg, since we
    **  need the end of the 1st arg (end1) anyway:
    */
    if ((argp[0]=lsp_argsll)==NULL)            { msgn=2; goto out; }
    if (argp[0]->restype!=LSP_RTSYMB &&
        argp[0]->restype!=RTLB)                { msgn=1; goto out; }
    if ((end1=lsp_argend(argp[0],NULL))==NULL) { msgn=5; goto out; }
    if ((argp[1]=end1->rbnext)==NULL)          { msgn=2; goto out; }
    if (argp[1]->restype!=RTLB &&
        argp[1]->restype!=RTNIL)               { msgn=1; goto out; }
    if (lsp_nextarg(argp[1])!=NULL)            { msgn=9; goto out; }


    /* Start with the RTLB and the 1st arg (the function): */

    if ((expr=db_alloc_resbuf())==NULL)
        { msgn=7; goto out; }
    expr->restype=RTLB;
    expr->resval.rstring=NULL;
    last=expr;
    if ((last->rbnext=db_alloc_resbuf())==NULL)
        { msgn=7; goto out; }
    last=last->rbnext;
    /* Chop, copy, repair: */
    end1->rbnext=NULL;
    fi1=lsp_copysplrb(last,argp[0]);
    end1->rbnext=argp[1];
    if (fi1) { msgn=7; goto out; }
    while (last->rbnext!=NULL) last=last->rbnext;


    /* Now take each member of the 2nd arg's list; add (<Subr-QUOTE> */
    /* before it and ) after it: */

    if (argp[1]->restype==RTLB && argp[1]->rbnext!=NULL &&
        argp[1]->rbnext->restype!=RTLE) {

        for (thismem=argp[1]->rbnext; thismem!=NULL &&
            thismem->restype!=RTLE; thismem=xend->rbnext) {

            if (thismem->restype==RTDOTE) { msgn=21; goto out; }

            /* Make the RTLB and LSP_RTSUBR links: */
            if ((last->rbnext=db_alloc_resbuf())==NULL) { msgn=7; goto out; }
            last=last->rbnext;
            last->restype=RTLB;
            last->resval.rstring=NULL;
            if ((last->rbnext=db_alloc_resbuf())==NULL) { msgn=7; goto out; }
            last=last->rbnext;
            last->restype=LSP_RTSUBR;
            last->resval.rstring=(char *)lsp_quote;

            /* Take a copy of this member: */
            if ((last->rbnext=db_alloc_resbuf())==NULL) { msgn=7; goto out; }
            last=last->rbnext;
            if ((xend=lsp_argend(thismem,NULL))==NULL) { msgn=5; goto out; }
            tp1=xend->rbnext; xend->rbnext=NULL;
            fi1=lsp_copysplrb(last,thismem);
            xend->rbnext=tp1;
            if (fi1) { msgn=7; goto out; }
            while (last->rbnext!=NULL) last=last->rbnext;

            /* Add the RTLE: */
            if ((last->rbnext=db_alloc_resbuf())==NULL) { msgn=7; goto out; }
            last=last->rbnext;
            last->restype=RTLE;
            last->resval.rstring=NULL;

        }
    }

    /* Add the final RTLE: */
    if ((last->rbnext=db_alloc_resbuf())==NULL) { msgn=7; goto out; }
    last=last->rbnext;
    last->restype=RTLE;
    last->resval.rstring=NULL;

    /* Evaluate.  (Recursion; use NULL for respp.): */
    if (lsp_lispev1(expr,NULL,lsp_fnvarval)) msgn=-2;


out:
    if (expr!=NULL) { lsp_freesuprb(expr); expr=NULL; }

    if (msgn!=-1) {  /* May get -2 from inner expression error. */
        if (msgn>-1) lsp_fnerrmsg(msgn);
        return -1;
    }
    return 0;
}

int lsp_mapcar(void) {
/*
**  This function is very similar to lsp_apply().  Basically, it
**  manufactures expressions to pass to lsp_lispev1() and collects the
**  results in a list.  Like lsp_apply(), it quotes each member
**  it uses from the args list(s).  See lsp_apply() for a detailed
**  discussion of the way the expressions to be evaluated are assembled.
**
**  One interesting thing with AutoLISP's mapcar is the following:
**
**      (SETQ L '((X Y)))
**      (MAPCAR 'L '(A B) '(C D)) gives
**
**          ((L (<Subr-QUOTE> B) (<Subr-QUOTE> D))
**           (L (<Subr-QUOTE> B) (<Subr-QUOTE> D)))
**
**      not the expected
**
**          ((L (<Subr-QUOTE> A) (<Subr-QUOTE> C))
**           (L (<Subr-QUOTE> B) (<Subr-QUOTE> D)))
**
**  I don't know why.  But OURS will give the expected result.
**
**  The args lists can differ in number of args -- but the first
**  one to run out stops the whole process.  That is, if the shortest
**  args list has n elements, the result list will have n elements.
**
**  The number of args lists allowed is controlled by the function
**  to be called; 1 member from each list is taken for every call, so
**  if there are x args lists, the function had better be able to handle
**  x args per call.
*/
    int msgn,nargps,pass,argidx,done,fi1;
    struct resbuf *expr,*fnend1,*fnend2,**argp,*thisarg,*exprrtle;
    struct resbuf *reslist,*preslist,*xend,*last,*argsll,*tp1;


    msgn=-1; argp=NULL; nargps=argidx=0;
    expr=exprrtle=reslist=preslist=argsll=NULL;

    if (lsp_argsll==NULL) { msgn=2; goto out; }

    /* Steal lsp_argsll for use as a local (because the recursion call to */
    /* lsp_lispev1() would destroy it): */
    argsll=lsp_argsll; lsp_argsll=NULL;

    if (argsll->restype!=LSP_RTSYMB &&
        argsll->restype!=RTLB)                  { msgn=1; goto out; }
    if ((fnend1=lsp_argend(argsll,NULL))==NULL) { msgn=5; goto out; }
    if (fnend1->rbnext==NULL)                   { msgn=2; goto out; }

    lsp_freesplrb(&lsp_evres);  /* Default result is nil. */


    /* Count the arg lists, then alloc the pointers and start them. */
    /* If any are nil or (), bail right now and set nil as the result: */
    for (pass=0; pass<2; pass++) {

        if (pass) {
            nargps=argidx;
            if ((argp= new struct resbuf * [nargps] )==NULL) { msgn=7; goto out; }
			memset(argp,0,sizeof(struct resbuf *)*nargps);
        }

        for (thisarg=fnend1->rbnext,argidx=0; thisarg!=NULL; thisarg=
            lsp_nextarg(thisarg),argidx++) {

            if (thisarg->restype==RTNIL)                     goto out;  /* nil */
            if (thisarg->restype!=RTLB)           { msgn= 1; goto out; }
            if (thisarg->rbnext==NULL)            { msgn= 5; goto out; }
            if (thisarg->rbnext->restype==RTLE)              goto out;  /* nil */
            if (thisarg->rbnext->restype==RTDOTE) { msgn=21; goto out; }

            if (pass) argp[argidx]=thisarg->rbnext;  /* Point at 1st member */
        }

    }


    /* Start expr with the part that never changes -- the RTLB and function: */

    if ((expr=db_alloc_resbuf())==NULL)
        { msgn=7; goto out; }
    last=expr;
    last->restype=RTLB;
    last->resval.rstring=NULL;
    if ((last->rbnext=db_alloc_resbuf())==NULL)
        { msgn=7; goto out; }
    last=last->rbnext;
    /* Chop, copy, repair: */
    tp1=fnend1->rbnext; fnend1->rbnext=NULL;
    fi1=lsp_copysplrb(last,argsll);
    fnend1->rbnext=tp1;
    if (fi1) { msgn=7; goto out; }
    while (last->rbnext!=NULL) last=last->rbnext;
    fnend2=last;  /* Last link of the function in expr (LSP_RTSYMB or RTLE). */


    /* Make the RTLE for the end of expr (and, eventually, the end */
    /* of the final result): */

    if ((exprrtle=db_alloc_resbuf())==NULL)
        { msgn=7; goto out; }
    exprrtle->restype=RTLE;
    exprrtle->resval.rstring=NULL;


    /* Build expr, evaluate it, and tack the results onto reslist, */
    /* until we run out of args in one of the lists: */

    done=0;
    do {  /* while (!done) */

        /* Free the args from last time, AND NULL THE OLD .rstring */
        /* POINTERS FOR lsp_argend(): */
        lsp_freesuprb(fnend2->rbnext);
        last=fnend2; last->rbnext=NULL;
        expr->resval.rstring=exprrtle->resval.rstring=NULL;

        /* Add a QUOTE of the current member from each list: */
        for (argidx=0; argidx<nargps; argidx++) {

            /* Make the RTLB and LSP_RTSUBR links: */
            if ((last->rbnext=db_alloc_resbuf())==NULL) { msgn=7; goto out; }
            last=last->rbnext;
            last->restype=RTLB;
            last->resval.rstring=NULL;
            if ((last->rbnext=db_alloc_resbuf())==NULL) { msgn=7; goto out; }
            last=last->rbnext;
            last->restype=LSP_RTSUBR;
            last->resval.rstring=(char *)lsp_quote;

            /* Take a copy of this member: */
            if ((last->rbnext=db_alloc_resbuf())==NULL) { msgn=7; goto out; }
            last=last->rbnext;
            if ((xend=lsp_argend(argp[argidx],NULL))==NULL)
                { msgn=5; goto out; }
            tp1=xend->rbnext; xend->rbnext=NULL;
            fi1=lsp_copysplrb(last,argp[argidx]);
            xend->rbnext=tp1;
            if (fi1) { msgn=7; goto out; }
            while (last->rbnext!=NULL) last=last->rbnext;

            /* Add an RTLE: */
            if ((last->rbnext=db_alloc_resbuf())==NULL) { msgn=7; goto out; }
            last=last->rbnext;
            last->restype=RTLE;
            last->resval.rstring=NULL;

            /* Step to next member and set done flag if there are no more */
            /* (but finish this for loop): */
            if ((argp[argidx]=xend->rbnext)==NULL ||
                 argp[argidx]->restype==RTLE) done=1;
            else if (argp[argidx]->restype==RTDOTE) { msgn=21; goto out; }

        }

        /* Add the prepared RTLE, evaluate, remove the RTLE: */
        last->rbnext=exprrtle;
        /* Evaluate.  (Recursion; use NULL for respp.): */
        fi1=lsp_lispev1(expr,NULL,lsp_fnvarval);
        last->rbnext=NULL;
        if (fi1) { msgn=-2; goto out; }

        /* Copy the result into the result list: */
        if ((tp1=db_alloc_resbuf())==NULL) { msgn=7; goto out; }
        if (lsp_copysplrb(tp1,&lsp_evres))
			{
			db_free_resbuf(tp1); msgn=7; goto out;
			}
        if (reslist==NULL) reslist=tp1; else preslist->rbnext=tp1;
        preslist=tp1;
        while (preslist->rbnext!=NULL) preslist=preslist->rbnext;

    } while (!done);


    /* Free the prev lsp_evres val and attach reslist as a list; */
    /* attach the prepared RTLE to it's final home: */

    if (reslist==NULL) { lsp_retnil(); goto out; }  /* Precaution */
    lsp_freesplrb(&lsp_evres);
    lsp_evres.restype=RTLB;
    lsp_evres.resval.rstring=NULL;
    lsp_evres.rbnext=reslist;
    preslist->rbnext=exprrtle;
    exprrtle->resval.rstring=NULL;
    reslist=exprrtle=NULL;  /* Important */


out:
    if (expr!=NULL)     { lsp_freesuprb(expr); expr=NULL; }
    if (exprrtle!=NULL) { lsp_freesuprb(exprrtle); exprrtle=NULL; }
    if (reslist!=NULL)  { lsp_freesuprb(reslist); reslist=preslist=NULL; }
    if (argp!=NULL)     { delete [] argp; argp=NULL; }
    lsp_freesuprb(argsll); argsll=NULL;  /* The original lsp_argsll. */

    if (msgn!=-1) {  /* May get -2 from inner expression error. */
        if (msgn>-1) lsp_fnerrmsg(msgn);
        return -1;
    }
    return 0;
}



