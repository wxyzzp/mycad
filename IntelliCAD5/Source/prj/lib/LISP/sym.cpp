/* C:\ICAD\PRJ\LIB\LISP\SYM.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/
#include "commandatom.h" /*DNT*/
// Added Cybage AJK 15/10/2001 [
//   Reason: Fix for Bug No: 77845 from BugZilla 
#include "bindtable.h"	
extern CBindStructure bindTable;
//added Cybage AJK 15/10/2001  DD/MM/YYYY ]

int lsp_snvalid(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL) { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR || (lsp_argsll->rbnext && lsp_argsll->rbnext->restype!=RTLONG))
        { msgn=1; goto out; }
    if (lsp_argsll->rbnext && lsp_argsll->rbnext->rbnext!=NULL) { msgn=9; goto out; }

    if (sds_snvalid(lsp_argsll->resval.rstring,
        (lsp_argsll->rbnext ? lsp_argsll->rbnext->resval.rint : 0))==RTNORM) lsp_rett();
    else lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_atom(void) {
    int msgn;

    msgn=-1;
    if (lsp_argsll==NULL) { msgn=2; goto out; }
    if (lsp_argsll->restype==RTLB) lsp_retnil(); else lsp_rett();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_atoms_family(void) {
    int msgn;
    long mode;
    struct resbuf *argp[2],*chain,*pchain,*thismem,*tp1;
    class commandAtomObj *tap1;

    msgn=-1; chain=pchain=NULL;

    if ((argp[0]=lsp_argsll)==NULL) { msgn=2; goto out; }
    if ((argp[1]=lsp_nextarg(argp[0]))!=NULL &&
        lsp_nextarg(argp[1])!=NULL) { msgn=9; goto out; }
    if      (argp[0]->restype==RTLONG)  mode=argp[0]->resval.rlong;
    else if (argp[0]->restype==RTSHORT) mode=(long)argp[0]->resval.rint;
    else { msgn=1; goto out; }
    if (mode<0L) { msgn=23; goto out; }

    lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value */
                                /* (since we're not doing an sds_ret...). */


    if (argp[1]==NULL) {  /* Take all */
        CString retkey;
        POSITION  Pos;

        for (Pos = commandAtomList->GetStartPosition(); Pos != NULL; ) {
            commandAtomList->GetNextAssoc(Pos, retkey, (CObject*&)tap1);

            /* Skip the mystery symbol in text mode: */
            if (tap1->LName[0]==lsp_mysterysym &&
                tap1->LName[1]==0 && mode) continue;

            if ((tp1=db_alloc_resbuf())==NULL) { msgn=7; goto out; }

            tp1->restype=(mode) ? RTSTR : LSP_RTSYMB;

            //Bugzilla No. 78104 ; 15-04-2002 
			if ((tp1->resval.rstring= new char [strlen(tap1->LName)+1])==NULL) {
				db_free_resbuf(tp1); msgn=7; goto out;
		    }

            strcpy(tp1->resval.rstring,tap1->LName);

            if (chain==NULL)
                chain=tp1;
            else
                pchain->rbnext=tp1;

            pchain=tp1;
        }

    } else {  /* Check for the items in the symbols list */

        if (argp[1]->restype==RTNIL)        { lsp_retnil(); goto out; }
        if (argp[1]->restype!=RTLB)         { msgn=1;       goto out; }
        if (argp[1]->rbnext==NULL)          { msgn=5;       goto out; }
        if (argp[1]->rbnext->restype==RTLE) { lsp_retnil(); goto out; }

        /* Walk the list members: */
        for (thismem=argp[1]->rbnext; thismem!=NULL &&
            thismem->restype!=RTLE; thismem=lsp_nextarg(thismem)) {

            if (thismem->restype==RTDOTE) { msgn=24; goto out; }
            if (thismem->restype!=RTNIL && (thismem->restype!=RTSTR ||
                thismem->resval.rstring==NULL)) { msgn=23; goto out; }

            if ((tp1=db_alloc_resbuf())==NULL)
                { msgn=7; goto out; }

            if ((tap1=(thismem->restype==RTSTR) ?
                lsp_findatom(thismem->resval.rstring,0) : NULL)==NULL)
                    tp1->restype=RTNIL;
            else {
                tp1->restype=(mode) ? RTSTR : LSP_RTSYMB;
                if ((tp1->resval.rstring= new char [ strlen(tap1->LName)+1] )==NULL)
                        {
						db_free_resbuf(tp1); msgn=7; goto out;
						}
                strcpy(tp1->resval.rstring,tap1->LName);
            }
            if (chain==NULL) chain=tp1; else pchain->rbnext=tp1;
            pchain=tp1;
        }
    }

    if (chain==NULL || pchain==NULL)
        { lsp_retnil(); goto out; }  /* Should never */

    /* Add RTLE: */
    if ((pchain->rbnext=db_alloc_resbuf())==NULL) { msgn=7; goto out; }
    pchain=pchain->rbnext;
    pchain->restype=RTLE;
    pchain->resval.rstring=NULL;

    /* Start with RTLB and tack on the chain: */
    lsp_evres.restype=RTLB; lsp_evres.rbnext=chain;
    lsp_evres.resval.rstring=NULL;
    chain=NULL;  /* Important */

out:
    if (chain!=NULL) {
        lsp_freesuprb(chain);
        chain=pchain=NULL;
    }

    if (msgn>-1) {
        lsp_fnerrmsg(msgn);
        return -1;
    }
    return 0;
}

int lsp_boundp(void) {
/*
**  The AutoLISP manual says, If atom is an undefined symbol,
**  it is automatically created and is bound to nil.
**  We have no such concept, and I'm not convinced that AutoLISP
**  does, either.  When a symbol is set to nil, it disappears from
**  the atoms list.  So, in what sense has it been created?
*/
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL) { msgn=2; goto out; }
    /* Too many is okay; it only looks at the first. */

    if (lsp_argsll->restype==LSP_RTSYMB &&
        lsp_findatom(lsp_argsll->resval.rstring,0)!=NULL)
            lsp_rett();
    else lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_null(void) {
    int msgn;

    msgn=-1;
    if (lsp_argsll==NULL) { msgn=2; goto out; }
    /* No check for too many this time. */
    if (lsp_argsll->restype==RTNIL) lsp_rett(); else lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_numberp(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL)              { msgn=2; goto out; }
    if (lsp_nextarg(lsp_argsll)!=NULL) { msgn=9; goto out; }

    if (lsp_argsll->restype==RTLONG  || lsp_argsll->restype==RTREAL  ||
        lsp_argsll->restype==RTANG   || lsp_argsll->restype==RTORINT ||
        lsp_argsll->restype==RTSHORT) lsp_rett();
    else lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_quote(void) {
/*
**  Most of the magic is done by lsp_lispev1(), which detects that
**  it's about to call this function and puts the unevaluated symbol
**  or list into lsp_argsll.
*/
    int msgn;
	struct resbuf *argsll;

    msgn=-1;
	//Modified CyberAge AP 10/18/2000 [
	extern int LSP_ConvertToPoint; // Reason: Fix for bug no. 57282 and CADR problem.
	if(LSP_ConvertToPoint != 2) // i.e. called by internalGetpoint(...)
		LSP_ConvertToPoint = 0; // Reason : Fix for bug no. 3372 and 59027
	// Modified CyberAge AP 10/18/2000 ]

    /* This is a donteval function (see lsp_lispev1()). */
    /* Steal lsp_argsll for local use (and don't free it): */
    argsll=lsp_argsll; lsp_argsll=NULL;

    if (argsll==NULL || lsp_nextarg(argsll)!=NULL)
        { msgn=16; goto out; }

    /* THIS time, we mean COPY -- don't steal it from argsll: */
    /* it's part of xll in lsp_lispev1(): */
    if (lsp_copysplrb(&lsp_evres,argsll)) { msgn=7; goto out; }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_read(void) {
/*
**  The strategy here is to build a string-form lisp expression,
**  consisting of the given string surrounded by parentheses, and
**  then convert it to the resbuf-form and extract the 1st
**  member of the list.  That is, we're using the same string-
**  to-resbuf-llist converter that the evaluator uses.
**
**  (Do NOT convert the string back to its esc char form before
**  calling lsp_xstr2xll().  I made that mistake when trying to
**  fix the nnnnx problem and broke expressions like
**  (read "\"abc\"").  It's fine WITHOUT the conversion.)
*/
    char *xstr,*fcp1,*fcp2;
    int msgn,toldem,fi1;
    struct resbuf *xll;

    msgn=-1; xstr=NULL; xll=NULL; toldem=0;

    if (lsp_argsll==NULL)                 { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR ||
        lsp_argsll->resval.rstring==NULL) { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)         { msgn=9; goto out; }

    /* Make a copy with room for the parentheses caps: */
    fi1=strlen(lsp_argsll->resval.rstring);
    if ((xstr= new char [fi1+3] )==NULL) { msgn=7; goto out; }
    strcpy(xstr+1,lsp_argsll->resval.rstring);

    /*
    **  We have a problem with reading nnnnx, where the n's are
    **  numeric chars and the x is an invalid symbol name char:
    **
    **      ( ) . ' " ;
    **
    **  I've added the following cheap fix specifically for this case.
    **  If we detect a number and it's followed by one of these
    **  chars, we terminate the string at that char.
    */
    for (fcp1=xstr+1; *fcp1 && isspace((unsigned char) *fcp1); fcp1++);  /* Skip white space. */
    if (*fcp1) {
        /* Use strtod() to see if we hit a number (and where it ends): */
        strtod(fcp1,&fcp2);

        /* If fcp1!=fcp2, it must have recognized a number, so check for an */
        /* illegal symbol name char after it and plant a 0 over it, */
        /* if found: */
        if (fcp2!=NULL && *fcp2 && fcp1!=fcp2 &&
            strchr(lsp_badsymchars,*fcp2)!=NULL) *fcp2=0;
    }

    // In AutoCAD (read) strips off everything after a '.  Probable AutoCAD Bug?
    if(strchr(xstr+1,'\''/*DNT*/) && *(xstr+1)!='('/*DNT*/) {
        *strchr(xstr+1,'\''/*DNT*/)=0;
    }

    fi1=strlen(xstr+1);  /* Find length again, since it may have been chopped. */

    /* Cap with parentheses: */
    *xstr='('/*DNT*/; xstr[fi1+1]=')'/*DNT*/; xstr[fi1+2]=0;

    /* Get the resbuf llist form: */
    if ((fi1=lsp_xstr2xll(xstr,&xll))!=0) {
        if      (fi1==-1) msgn=5;
        else if (fi1==-3) msgn=7;
        toldem=1;  /* lsp_xstr2xll() calls lsp_fnerrmsg() itself. */
    }

    /* Check for an invalid resbuf llist expression: */
    if (xll==NULL || xll->restype!=RTLB || xll->rbnext==NULL)
        { lsp_retnil(); goto out; }  /* Something's wrong */

    /* Handle a couple of special cases: */
    if (xll->rbnext->restype==RTDOTE) { msgn=46; goto out; }
    if (xll->rbnext->restype==RTLE)   { lsp_retnil(); goto out; }

	// We need to handle special cases for T and nil
	if (xll->rbnext->restype==LSP_RTSYMB && strisame(xll->rbnext->resval.rstring,"T"/*DNT*/)) {
		xll->rbnext->restype=RTT;
		if (lsp_retval(xll->rbnext)!=RTNORM) { msgn=7; goto out; }
		xll->rbnext->restype=LSP_RTSYMB;
		goto out;
	}
	if (xll->rbnext->restype==LSP_RTSYMB && strisame(xll->rbnext->resval.rstring,"nil"/*DNT*/)) {
		xll->rbnext->restype=RTNIL;
		if (lsp_retval(xll->rbnext)!=RTNORM) { msgn=7; goto out; }
		xll->rbnext->restype=LSP_RTSYMB;
		goto out;
	}

    /* Take the 1st member: */
    if (lsp_retval(xll->rbnext)!=RTNORM) { msgn=7; goto out; }

out:
    if (xstr!=NULL) { IC_FREE(xstr); xstr=NULL; }
    if (xll!=NULL) { lsp_freesuprb(xll); xll=NULL; }
    if (msgn>-1 && !toldem) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_set(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL)                      { msgn=2; goto out; }
    if (lsp_argsll->restype!=LSP_RTSYMB)       { msgn=1; goto out; }
    /* Now it's safe to use rbnext instead of lsp_nextarg(): */
    if (lsp_argsll->rbnext==NULL)              { msgn=2; goto out; }
    if (lsp_nextarg(lsp_argsll->rbnext)!=NULL) { msgn=9; goto out; }

    if (lsp_argsll->rbnext->restype==RTNIL) {  /* Setting to NIL; del from atoms. */
        lsp_undefatom(lsp_argsll->resval.rstring,-1);
    } else if (lsp_defatom(lsp_argsll->resval.rstring,0,lsp_argsll->rbnext,1)) {
        msgn=6; goto out;
    }

    if (lsp_retval(lsp_argsll->rbnext)!=RTNORM) { msgn=7; goto out; }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_setq(void) {
/*
**  This one is VERY different from the other internal functions.
**  It's args are processed in pairs -- sym,val,... -- from left to
**  right, and the vals are evaluated only when processing reaches
**  that point.  That allows expressions like this:
**  (setq a 5 b -9 c (* a b)).
**
**  This is quite different from most of the other functions, which
**  receive lsp_argsll with all values already evaluated to simplest
**  form (in lsp_lispev1()).  To avoid args like
**  (* a b) in the example above, lsp_lispev1() recognizes setq as a
**  special case and passes inner expressions to lsp_setq() as RTLB
**  lists in lsp_argsll -- instead of evaluating them on the spot.
**  Symbols are passed as RTSYMBs instead of evaluating them.  Then
**  lsp_setq() calls lsp_evalarg() which calls lsp_lispev1() to evaluate
**  expressions and lsp_evalsym() to evaluate symbols WHEN IT GETS TO THEM
**  -- so that any earlier assignments have already occurred by that
**  time.  (The lsp_lispev1() call is indirect recursion.)
**
**  In order to pull this trick off, lsp_setq() has to have its
**  own private copy of lsp_argsll, because the recursion call to
**  lsp_lispev1() destroys the original lsp_argsll.  lsp_setq() steals
**  lsp_argsll and uses it as a local (argsll), setting lsp_argsll to
**  NULL so that the evaluation calls don't free it.
**
**  When working on this function, keep in mind some unusual cases that
**  it must be able to handle:
**      (setq a 5 b (+ a -14.0) c (* a b))
**          Gives a=5; b=-9.0; c=-45.0.  Result: -45.0
**      (setq nil 5)
**          Bad arg
**      (setq 5 6)
**          Bad arg
**      (setq "abc" 6)
**          Bad arg
**      (setq (strcat "abc") 6)
**          Bad arg
**      (setq (quote a) 6)
**          Bad arg
**      (setq a 5) followed by (setq a 4 b a)
**          Sets a and b to 4; result: 4
**      (setq b '(+ 1 2)) followed by (setq a b)
**          Sets a and b to the list (+ 1 2)
**      (setq a nil)
**          Removes a from atoms llist
**
**  Also, make sure that this function can store all types of values.
*/
    int msgn;
    struct resbuf *slink,*vlink,*nextslink;
	struct resbuf argsll;
	memset( &argsll, 0, sizeof( struct resbuf ) );
    msgn=-1; slink=vlink=nextslink=NULL;

    /* This is a donteval function (see lsp_lispev1()). */
    /* Steal lsp_argsll for local use (and don't free it): */

	// Modified CyberAge PP 09-22-2000 [
	// Fix for bug reported by Peter Coburn related with calc.lsp
//	argsll = db_alloc_resbuf() ;
	lsp_copysplrb( &argsll, lsp_argsll );
	lsp_argsll=NULL ;
	// Modified CyberAge PP 09-22-2000 ]

    if( lsp_nextarg( &argsll ) == NULL )
	{
		msgn=2;
		goto out;
	}

    for (slink=&argsll; slink!=NULL; slink=lsp_nextarg(vlink)) {

        if ((vlink=lsp_nextarg(slink))==NULL) { msgn=2; goto out; }

        /* Make sure the var name is in an arg of type LSP_RTSYMB, */
        /* that it's not a pure number, and that it's not NIL: */

		double dTemp;
        if (slink->restype!=LSP_RTSYMB || ic_isnumstr(slink->resval.rstring,&dTemp) ||
            strisame(slink->resval.rstring,"NIL"/*DNT*/)) { msgn=1; goto out; }

        /* Put the value in lsp_evres: */
        if ((msgn=lsp_evalarg(vlink,NULL))!=-1) goto out;

        if (lsp_evres.restype==RTNIL) {  /* Setting to NIL; del from atoms. */
			// Check to see if it's an internal command.
			// Bugzilla No : 78441 ; 31-03-2003 
			class commandAtomObj *link2mod,*link2mod_L;  
			//
			link2mod = lsp_findatom(slink->resval.rstring,0);
			if (link2mod == NULL)
				continue;
			// This test needs to match the one in lsp_defatom()
			if (((link2mod->id == -3) && !strnisame(slink->resval.rstring, "C:"/*DNT*/,2))
						|| (link2mod->id == -2 && *slink->resval.rstring == '_'))
				{
				// Yes, this is an internal command i.e. LINE. So we are going to prepend
				// an L: which would have been prepended during the (setq) for this atom name.
				// Check out code in lsp_defatom() when a user tries to (setq) a preexisting command.
				// Prepending 'L:' indicates a LISP atom instead of a command atom.
				TCHAR tname[IC_ACADNMLEN];
				sprintf(tname, "L:%s", slink->resval.rstring);
				// Bugzilla No : 78441 ; 31-03-2003 
				link2mod_L =lsp_findatom(tname,0);
				if (link2mod_L != NULL && link2mod_L->parensReqd) // We found it!
					lsp_undefatom(tname,-1);
				// eg (defun _abc() (...)) twice. we will have to remove _abc and L:_abc both
				if (link2mod->id == -2 && *slink->resval.rstring == '_')
					lsp_undefatom(slink->resval.rstring,-1);
				
				}
			else
				lsp_undefatom(slink->resval.rstring,-1);
        } else if (lsp_defatom(slink->resval.rstring,0,&lsp_evres,1)) {
            msgn=6; goto out;
        }
	//	 Added Cybage AJK 15/10/2001 [
	  // Reason: Fix for Bug No: 77845 from BugZilla 
		//Modified Cybage AJK 24/12/2001 [
		//Reason - Fix for crash problem
		/*if(slink->restype==LSP_RTSYMB && vlink->restype==LSP_RTSYMB )
		{
			if(!bindTable.isBinded (slink->resval.rstring,vlink->resval.rstring))
			bindTable.bindAtoms (slink->resval.rstring,vlink->resval.rstring);
			else
				bindTable.removeAtom (slink->resval.rstring);
		}*/
		if(slink->restype==LSP_RTSYMB && vlink->restype==LSP_RTSYMB )
		{
			// Bugzilla No : 78054 ; 25-03-2002 [
			/*if(!bindTable.isBinded (slink->resval.rstring,vlink->resval.rstring))
			{
				CStringList *ptrAtom1=NULL;
				if(bindTable.checkAtom(slink->resval.rstring,ptrAtom1)) 
					bindTable.removeAtom (slink->resval.rstring);
				bindTable.bindAtoms (slink->resval.rstring,vlink->resval.rstring);
			}*/
			bindTable.bindAtoms (slink->resval.rstring,vlink->resval.rstring);
			// Bugzilla No : 78054 ; 25-03-2002 ]
		}
		//Modified Cybage AJK 24/12/2001 DD/MM/YYYY ]

				
		//added Cybage AJK 15/10/2001 DD/MM/YYYY ]
 
    }  /* End for each pair of args */

    /* Note that since we've use lsp_evres to hold the value that */
    /* gets stored, the last value is left there; we don't need to */
    /* do an sds_retval() to put it there. */

out:
	// Modified CyberAge PP 09-22-2000
	// Fix for bug reported by Peter Coburn related with calc.lsp
	lsp_freesplrb( &argsll ) ;
    if (msgn!=-1) {  /* May get -2 from lsp_evalarg() */
        if (msgn>-1) lsp_fnerrmsg(msgn);
        return -1;
    }
    return 0;
}

int lsp_type(void) {
/*
**  Note that T has type SYM.  Does that mean that T is a var that
**  holds a symbol whose value is T?  For now, it's still a var
**  whose type is RTT, not LSP_RTSYMB.  We just make a special
**  case out of RTT here so that it TYPEs as SYM.
*/
    int msgn;
    struct resbuf retrb;

    msgn=-1;
    retrb.restype=LSP_RTSYMB; retrb.resval.rstring=NULL; retrb.rbnext=NULL;

    if (lsp_argsll==NULL) { msgn=2; goto out; }
    /* No such thing as too many; just looks at the first. */

    switch (lsp_argsll->restype) {
        case RTREAL: case RTANG:
        case RTORINT:              retrb.resval.rstring="REAL"/*DNT*/;    break;
        case RTLONG: case RTSHORT: retrb.resval.rstring="INT"/*DNT*/;     break;
        case RTSTR:                retrb.resval.rstring="STR"/*DNT*/;     break;
        case RTENAME:              retrb.resval.rstring="ENAME"/*DNT*/;   break;
        case RTPICKS:              retrb.resval.rstring="PICKSET"/*DNT*/; break;
        case RTLB:                 retrb.resval.rstring="LIST"/*DNT*/;    break;
        case LSP_RTSYMB: case RTT: retrb.resval.rstring="SYM"/*DNT*/;     break;
        case LSP_RTSUBR:           retrb.resval.rstring="SUBR"/*DNT*/;    break;
        case LSP_RTXSUB:           retrb.resval.rstring="EXSUBR"/*DNT*/;  break;
        case LSP_RTFILE:           retrb.resval.rstring="FILE"/*DNT*/;    break;
        default: lsp_retnil(); goto out;  /* Catches nil and any errors. */
    }

    if (lsp_retval(&retrb)!=RTNORM) { msgn=7; goto out; }

out:
    /* Don't free retrb.resval.rstring!  It's not alloc'd. */

    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}


