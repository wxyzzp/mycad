/* C:\ICAD\PRJ\LIB\LISP\LOOPNCOND.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/
#include "icadapi.h"
#include "commandatom.h" /*DNT*/

int lsp_if(void) {
    int msgn;
    struct resbuf *argp[3],*which,*argsll;

    msgn=-1; argp[0]=argp[1]=argp[2]=NULL;

    /* This is a donteval function (see lsp_lispev1()). */
    /* Steal lsp_argsll for local use (and don't free it): */
    argsll=lsp_argsll; lsp_argsll=NULL;

    if (argsll==NULL) { msgn=2; goto out; }

    if ((argp[0]=argsll)==NULL || (argp[1]=lsp_nextarg(argp[0]))==NULL)
        { msgn=2; goto out; }
    if ((argp[2]=lsp_nextarg(argp[1]))!=NULL && lsp_nextarg(argp[2])!=NULL)
        { msgn=9; goto out; }

    /* Evaluate the 1st arg: */
    if ((msgn=lsp_evalarg(argp[0],NULL))!=-1) goto out;

    which=(lsp_evres.restype==RTNIL) ? argp[2] : argp[1];

    if (which==NULL) lsp_retnil(); else msgn=lsp_evalarg(which,NULL);

out:
    if (msgn!=-1) {  /* May get -2 from lsp_evalarg(). */
        if (msgn>-1) lsp_fnerrmsg(msgn);
        return -1;
    }
    return 0;
}

int lsp_while(void) {
    int msgn;
    struct resbuf lastres,*argsll,*thisarg;

    msgn=-1;
    lastres.restype=RTNIL; lastres.rbnext=NULL;  /* Special super resbuf */
                                                 /* for last result. */

    /* This is a donteval function (see lsp_lispev1()). */
    /* Steal lsp_argsll for local use (and don't free it): */
    argsll=lsp_argsll; lsp_argsll=NULL;

    if (argsll==NULL) { msgn=2; goto out; }

    for (;;) {

        if (sds_usrbrk()) { 
			SDS_CancelAllPending(0);
			msgn=20; goto out; 
		}

                /* CHECK THE 1ST ARG FOR NIL/NON-NIL */

        if ((msgn=lsp_evalarg(argsll,NULL))!=-1) goto out;

        if (lsp_evres.restype==RTNIL) {  /* Done */

            /* Copy or steal the result from just before the test expr eval: */
            if (lastres.restype==RTLB) {
                lsp_freesplrb(&lsp_evres);
                lsp_evres.restype=RTLB;
                lsp_evres.resval.rstring=NULL;
                lsp_evres.rbnext=lastres.rbnext;
                lastres.rbnext=NULL;
                lastres.resval.rstring=NULL;
            } else {
                if (lsp_copysplrb(&lsp_evres,&lastres)) { msgn=7; goto out; }
            }

            break;
        }


          /* EVALUATE THE REMAINING ARGS (EVEN SYMBOLS AND LITERALS) */

        for (thisarg=lsp_nextarg(argsll); thisarg!=NULL; thisarg=
            lsp_nextarg(thisarg)) {

            if ((msgn=lsp_evalarg(thisarg,NULL))!=-1) goto out;
        }

        /* Copy or steal most recent result from lsp_evres: */
        if (lsp_evres.restype==RTLB) {
            lsp_freesplrb(&lastres);
            lastres.restype=RTLB;
            lastres.resval.rstring=NULL;
            lastres.rbnext=lsp_evres.rbnext;
            lsp_evres.rbnext=NULL;
            lsp_evres.resval.rstring=NULL;
        } else {
            if (lsp_copysplrb(&lastres,&lsp_evres)) { msgn=7; goto out; }
        }

    }

out:
    lsp_freesplrb(&lastres);

    if (msgn!=-1) {  /* lsp_evalarg() may return -2. */
        if (msgn>-1) lsp_fnerrmsg(msgn);
        return -1;
    }
    return 0;
}

int lsp_repeat(void) {
    int msgn;
    long howmany;
    struct resbuf *argsll,*thisarg,*top;

    msgn=-1; howmany=0L;

    /* This is a donteval function (see lsp_lispev1()). */
    /* Steal lsp_argsll for local use (and don't free it): */
    argsll=lsp_argsll; lsp_argsll=NULL;

    lsp_freesplrb(&lsp_evres);  /* Default result is nil. */

    if (argsll==NULL) { msgn=2; goto out; }

    /* Evaluate the 1st arg: */
    if ((msgn=lsp_evalarg(argsll,NULL))!=-1) goto out;

    if      (lsp_evres.restype==RTLONG)  howmany=lsp_evres.resval.rlong;
    else if (lsp_evres.restype==RTSHORT) howmany=(long)lsp_evres.resval.rint;
    else { msgn=1; goto out; }

    /* Find the top of the loop (next arg): */
    top=lsp_nextarg(argsll);

    if (howmany<1L || top==NULL) goto out;  /* nil */

    while (howmany>0L) {
        for (thisarg=top; thisarg!=NULL; thisarg=lsp_nextarg(thisarg))
            if ((msgn=lsp_evalarg(thisarg,NULL))!=-1) goto out;
        howmany--;
    }

out:
    if (msgn!=-1) {  /* May get -2 from lsp_evalarg(). */
        if (msgn>-1) lsp_fnerrmsg(msgn);
        return -1;
    }
    return 0;
}

int lsp_foreach(void) {
/*
**  This one's weird; it's a donteval function, but the 2nd arg
**  has to be evaluated prior to the looping.
*/
	// Modified PK 26/07/2000
	// int msgn,hashidx;
	int msgn,hashidx,atomdefined=0; // Reason: Fix to bug no. 57436 
    struct resbuf *argp[3],*argsll,*thisval,*thisarg,thelist;
    class commandAtomObj *savatom;

    msgn=-1; argp[0]=argp[1]=argp[2]=NULL; savatom=NULL; hashidx=-1;
    thelist.restype=RTLB; thelist.rbnext=NULL;  /* Spl resbuf for the list. */
    thelist.resval.rstring=NULL;

    /* This is a "donteval" function (see lsp_lispev1()). */
    /* Steal lsp_argsll for local use (and don't free it): */
    argsll=lsp_argsll; lsp_argsll=NULL;

    lsp_freesplrb(&lsp_evres);  /* Default result is nil. */

    if ((argp[0]=argsll)==NULL || (argp[1]=lsp_nextarg(argp[0]))==NULL)
        { msgn=2; goto out; }
    argp[2]=lsp_nextarg(argp[1]);

    /* Eval the 2nd arg (the list): */
    if ((msgn=lsp_evalarg(argp[1],NULL))!=-1) goto out;

    if (argp[0]->restype!=LSP_RTSYMB || argp[0]->resval.rstring==NULL ||
        !argp[0]->resval.rstring[0] ||
        (lsp_evres.restype!=RTLB && lsp_evres.restype!=RTNIL))
            { msgn=1; goto out; }

    if (lsp_evres.restype==RTNIL || lsp_evres.rbnext==NULL ||
        lsp_evres.rbnext->restype==RTLE || argp[2]==NULL) goto out; /* nil */

    /* Now we know we have an RTLB list in lsp_evres; steal it: */
    thelist.rbnext=lsp_evres.rbnext; lsp_evres.rbnext=NULL;
    lsp_evres.resval.rstring=NULL;

    /* See if the symbol already exists in the atoms list: */
    if ((savatom=lsp_findatom(argp[0]->resval.rstring,0))!=NULL) {
		commandAtomList->RemoveKey(argp[0]->resval.rstring);
		commandAtomObjPtrList->RemoveKey(savatom);
//        delete(savatom);
    }

    /* Step through each value: */
    for (thisval=thelist.rbnext; thisval!=NULL &&
        thisval->restype!=RTLE; thisval=lsp_nextarg(thisval)) {

        if (thisval->restype==RTDOTE) { msgn=38; goto out; }

        /*
        **  Define the loop var.  (The 1 means replace prev value).
        **  Don't get fancy here and try to just substitute the
        **  new value.  The prev loop may have REMOVED the atom
        **  by setq-ing it to nil or something.):
        */
        if (lsp_defatom(argp[0]->resval.rstring,0,thisval,1))
            { msgn=7; goto out; }
		// Modified PK 26/07/2000
		atomdefined = 1; // Reason: Fix to bug no. 57436 

        /* Now eval the expressions after the list of vals: */
        for (thisarg=argp[2]; thisarg!=NULL; thisarg=lsp_nextarg(thisarg))
            if ((msgn=lsp_evalarg(thisarg,NULL))!=-1) goto out;
    }

out:
    lsp_freesplrb(&thelist);

    /* Remove the loop-var from the atoms list: */
	// Modified PK 26/07/2000
	// lsp_undefatom(argp[0]->resval.rstring,0);
	if (atomdefined) // Reason: Fix to bug no. 57436 
		lsp_undefatom(argp[0]->resval.rstring,0);

    /* Put back the saved atom: */
    if (savatom!=NULL) {
		commandAtomList->SetAt((LPCTSTR)savatom->LName, (CObject*)savatom);
        commandAtomObjPtrList->SetAt((void *)savatom, (CObject*)savatom);
	}


    if (msgn!=-1) {  /* May get -2 from lsp_evalarg(). */
        if (msgn>-1) lsp_fnerrmsg(msgn);
        return -1;
    }

    return 0;
}

int lsp_cond(void) {
    int msgn,firstmem;
    struct resbuf *argsll,*thisarg,*thismem;

    msgn=-1;

    /* This is a donteval function (see lsp_lispev1()). */
    /* Steal lsp_argsll for local use (and don't free it): */
    argsll=lsp_argsll; lsp_argsll=NULL;

    lsp_freesplrb(&lsp_evres);  /* Default result is nil. */

    if (argsll==NULL) goto out;

    /* Step through the args (which should all be lists): */
    for (thisarg=argsll; thisarg!=NULL; thisarg=lsp_nextarg(thisarg)) {

        if (thisarg->restype==RTNIL) { msgn=2; goto out; } /* Yep; too few. */
        if (thisarg->restype!=RTLB)  { msgn=1; goto out; }

        /* Start to eval the members of this list: */
        firstmem=1;
		// Bugzilla::78440; 15-04-03;
		// Rollbacked the changes for bugzilla::57121. Bugzilla::57121 is invalid.
		for (thismem=thisarg->rbnext; thismem!=NULL &&		
            thismem->restype!=RTLE; thismem=lsp_nextarg(thismem)) {

            if (thismem->restype==RTDOTE) { msgn=24; goto out; }

            if ((msgn=lsp_evalarg(thismem,NULL))!=-1) goto out;
            if (firstmem) {
                if (lsp_evres.restype==RTNIL) break;  /* Nope; next list. */
                firstmem=0;  /* OK; eval the rest of the members. */
            }
        }

        if (!firstmem) break;  /* Done; this list worked */
    }

    /* Result of last thing eval'd should now be in lsp_evres. */

out:
    if (msgn!=-1) {  /* May get -2 from lsp_evalarg(). */
        if (msgn>-1) lsp_fnerrmsg(msgn);
        return -1;
    }
    return 0;
}


