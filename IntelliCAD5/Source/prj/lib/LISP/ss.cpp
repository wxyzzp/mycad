/* C:\ICAD\PRJ\LIB\LISP\SS.CPP
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

// Modified PK 26/07/2000
extern struct resbuf *args_sssetfirst; // Reason : Fix to bug no. 58350
// Modified CyberAge AP 22/11/2000
int cmd_ssadd;	// Reason : To fix bug no. 58277
int lsp_sssetfirst(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL) { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTPICKS && lsp_argsll->restype!=RTNIL)
        { msgn=1; goto out; }
	if (lsp_argsll->rbnext && (lsp_argsll->rbnext->restype!=RTPICKS && lsp_argsll->rbnext->restype!=RTNIL))
        { msgn=1; goto out; }
    if (lsp_argsll->rbnext && lsp_argsll->rbnext->rbnext!=NULL) { msgn=9; goto out; }

	/*DG - 29.1.2002*/// Pass NULL for the 2nd arg if lsp_argsll->rbnext->restype == RTNIL too (as for the 1st one).
	if( sds_sssetfirst(lsp_argsll->restype == RTNIL ? NULL : lsp_argsll->resval.rlname,
					  !lsp_argsll->rbnext || lsp_argsll->rbnext->restype == RTNIL ? NULL : lsp_argsll->rbnext->resval.rlname)
		== RTNORM )
        {
        // Bug 1-61153 Fix.
        // Need to get the name of the argument(s) that were supplied when this
        // command was issued, so we can return it to LISP command line.
        struct sds_resbuf args[2];
        bool needArg0 = FALSE, needArg1 = FALSE;
        // First set up the resbuf chain that we will be passing back to LISP. 
        // There will be one, possibly two arguments supplied. One or more could be
        // nil too. Here we hook the two resbufs together ans set their types. Note
        // if no second arg is supplied, simply don't hook it to the first resbuf.
        args[0].restype = lsp_argsll->restype == RTPICKS ? LSP_RTSYMB : RTNIL;
        args[0].resval.rstring = NULL;
        args[1].rbnext  = NULL;
        args[1].resval.rstring = NULL;

        if (lsp_argsll->rbnext != NULL)
            {
            args[1].restype = lsp_argsll->rbnext->restype == RTPICKS ? LSP_RTSYMB : RTNIL;
            args[0].rbnext = &(args[1]);
            needArg0 = args[0].restype != RTNIL;
            needArg1 = args[1].restype != RTNIL;
            }
        else
            {
            args[0].rbnext = NULL;
            needArg0 = args[0].restype != RTNIL;
            }

        void *rKey;
        POSITION Pos;
        class commandAtomObj *atom=NULL;

        // Now if we need to get the symbol name for the selection set(s) passed in from
        // the command line, or program, We'll loop throught all the symbols in the 
        // commandAtomObjPtrList. We are only interested in Selection sets, and then only
        // if they match the rlname that was passed in. Once we have found an atomLink whos
        // rb.resval.rlname is the one we want, the symbol's name is in the atomLink's 
        // LName variable.
        if (needArg0 || needArg1)
            for(Pos = commandAtomObjPtrList->GetStartPosition(); Pos != NULL; ) 
                {
	            commandAtomObjPtrList->GetNextAssoc(Pos, rKey, (void*&)atom);
                if (atom->rb.restype == SDS_RTPICKS)
                    {
                    int len = strlen(atom->LName);
                    if (needArg0)
                        {
                        if (sds_name_equal(atom->rb.resval.rlname, lsp_argsll->resval.rlname))
                            {
                            args[0].resval.rstring= new char [len+1];
                            strncpy(args[0].resval.rstring,atom->LName,len);
                            strupr(args[0].resval.rstring);  /* Syms kept in caps */
                            needArg0 = FALSE;
                            }                    
                        }
                    if (needArg1)
                        {
                        if (sds_name_equal(atom->rb.resval.rlname, lsp_argsll->rbnext->resval.rlname))
                            {
                            args[1].resval.rstring= new char [len+1];
                            strncpy(args[1].resval.rstring,atom->LName,len);
                            strupr(args[1].resval.rstring);  /* Syms kept in caps */
                            needArg1 = FALSE;
                            }                    
                        }
                    }
                if (!needArg0 && !needArg1) // Get out as soon as all args are resolved.
                    break;
                }
        // Now return the list to LISP. Could be only one resbuf.
		// Modified CyberAge AP 11/01/2000 [
		// Reason: Fixed sssetfirst combined with ssget makes icad close down.
		// lsp_retlist(&(args[0]));
		if (lsp_argsll->restype == RTNIL) 
			lsp_retlist(args_sssetfirst);
		else
		{
			if(args[0].resval.rstring)
				lsp_retlist(&(args[0]));
		}

		// Clean up any string allocation.
		if (args[0].resval.rstring)
			IC_FREE(args[0].resval.rstring);
		if (args[1].resval.rstring)
			IC_FREE(args[1].resval.rstring);
		}
	else 
		lsp_retnil();

out:
	args_sssetfirst = NULL;
	// Modified CyberAge AP 11/01/2000 ]
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_ssgetfirst(void) {
    int msgn;
    struct resbuf *rbp[2],*res,*last;

    msgn=-1;
    rbp[0]=rbp[1]=NULL; res=last=NULL;

    if (lsp_argsll!=NULL) { msgn=9; goto out; }

    if (sds_ssgetfirst(rbp,rbp+1)!=RTNORM) rbp[0]=rbp[1]=NULL;

    /*
    **  TODO:
    **
    **  We need to verify exactly what ADS does with rbp[0] and rbp[1].
    **  I'm assuming (from the ACAD docs) that it allocates just
    **  two resbufs (one for rbp[0], one for rbp[1]) and that they are
    **  both RTPICKSs (if non-NULL).
    **
    **  Our sds_ssgetfirst() isn't finished yet, so, presently, this
    **  LISP one should always return (nil nil).
    */

	/*DG - 25.1.2002*/// Now sds_ssgetfirst works, but it's callers must free returned selsets.
	// It's for TO DO.

    /* Build the list (minus the first RTLB) that we're going to return: */

    if ((res=last=db_alloc_resbuf())==NULL)
        { msgn=7; goto out; }
    if (rbp[0]!=NULL && rbp[0]->restype==RTPICKS) {
        last->restype=RTPICKS;
        last->resval.rlname[0]=rbp[0]->resval.rlname[0];
        last->resval.rlname[1]=rbp[0]->resval.rlname[1];
    } else last->restype=RTNIL;

    if ((last->rbnext=db_alloc_resbuf())==NULL)
        { msgn=7; goto out; }
    last=last->rbnext;
    if (rbp[1]!=NULL && rbp[1]->restype==RTPICKS) {
        last->restype=RTPICKS;
        last->resval.rlname[0]=rbp[1]->resval.rlname[0];
        last->resval.rlname[1]=rbp[1]->resval.rlname[1];
    } else last->restype=RTNIL;

    if ((last->rbnext=db_alloc_resbuf())==NULL)
        { msgn=7; goto out; }
    last=last->rbnext;
    last->restype=RTLE;
    last->resval.rstring=NULL;

    lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value */
                                 /* (since we're not doing an sds_ret...). */

    /* Steal the result from res: */
    lsp_evres.restype=RTLB;
    lsp_evres.resval.rstring=NULL;
    lsp_evres.rbnext=res;
    res=NULL;

out:
    if (rbp[0]!=NULL) lsp_freesuprb(rbp[0]);
    if (rbp[1]!=NULL) lsp_freesuprb(rbp[1]);
    if (res!=NULL) lsp_freesuprb(res);
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_ssnamex(void) {
/*
**  TODO:
**
**  The sds one isn't finished (and probably won't be for some time --
**  it's horrendous, and will require us to save pick-method-history
**  in selection sets, somehow).
**
**  All we're doing in the LISP one for now is parameter-checking
**  (which is pretty wacky by itself) and returning nil.
**  Someday, we'll have to hook it up to the sds one (but we NEED
**  that one to write this one -- we can't very easily just
**  blindly write this one to interface with it: it's WAY too complex).
*/
    int msgn,ssidx;
    sds_real ar1;
    struct resbuf *argp[2];

    msgn=-1;
    ssidx=-1;  /* Default: show ALL members. */

    if ((argp[0]=lsp_argsll)     ==NULL) { msgn= 2; goto out; }
    if ( argp[0]->restype!=RTPICKS)      { msgn= 1; goto out; }
    if ((argp[1]=argp[0]->rbnext)!=NULL) {
        if (argp[1]->restype==RTLONG) {
            ssidx=argp[1]->resval.rlong;
        } else if (argp[1]->restype==RTSHORT) {
            ssidx=argp[1]->resval.rint;
        } else if (argp[1]->restype==RTREAL  ||
                   argp[1]->restype==RTANG   ||
                   argp[1]->restype==RTORINT) {

            /* ACAD treats <=-1.0 as ALL.  Otherwise, it takes the abs val */
            /* and discards the fractional part: */

            if (argp[1]->resval.rreal<=-1.0) ssidx=-1;
            else {
                modf(fabs(argp[1]->resval.rreal),&ar1);
                ssidx=(ar1<=INT_MAX) ? ((int)ar1) : -1;
            }
        } else                               { msgn=32; goto out; }
        if ( argp[1]->rbnext!=NULL)          { msgn= 9; goto out; }
    }

    if (ssidx<0) ssidx=-1;

    /* Now ssidx is -1 for ALL -- or the idx of an item in the ss. */
    /* (We still haven't checked the length of the ss, hoewever.) */

    /* For now ... */
    lsp_retnil();


out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_ssget(void) {
/*
**  Can't use lsp_convptlist() on lsp_argsll here to convert
**  from list-form to points because of what it would do an entmask
**  that has 2D points in it (which look like 3D points starting
**  with the DXF code as the X because they are lists of 3 numbers).
**  So, we break out the right args and convert them separately.
**
**  Note that a LISp entmask of nil is acceptable; the ADS entmask
**  should be NULL.
*/
    char *modestr;
    int msgn,type,fi1;
    sds_real *p2p;
    sds_point p1,p2;
    void *p1p;
    struct resbuf *entmask,*thisarg,*ptlist,*tlist,*tp1,*tp2;
    sds_name ss;

    msgn=-1;

    /* Init the sds_ssget args to NULL: */
    modestr=NULL; p1p=NULL; p2p=NULL; entmask=NULL;

    ptlist=tlist=NULL; type=0;


    if ((thisarg=lsp_argsll)!=NULL) {

                          /* DEAL WITH THE 1ST ARG: */

        if (thisarg->restype==RTSTR) {  /* Mode */

            modestr=thisarg->resval.rstring;
            if(*modestr=='_'/*DNT*/) ++modestr;
            strupr(modestr);
            if ((fi1=strlen(modestr))==1) {
                switch(*modestr) {
                case 'I'/*DNT*/:
                case 'L'/*DNT*/:
                case 'P'/*DNT*/:
                case 'X'/*DNT*/:
                    type=0;
                    break;
                case 'F'/*DNT*/:
                    type = 1;
                    break;
                case 'C'/*DNT*/:
                case 'W'/*DNT*/:
                case 'O'/*DNT*/:
                    type = 2;
                    break;
                default:
                    msgn = 29;
                    break;
                }
            } else if (fi1==2) {
                switch(modestr[1]) {
                case 'P'/*DNT*/:
                    if (*modestr!='C'/*DNT*/ && *modestr!='W'/*DNT*/ && *modestr!='O'/*DNT*/)
                        msgn = 29;
                    else
                        type = 1;
                    break;
                case 'C'/*DNT*/:
                    if (*modestr!='C'/*DNT*/ && *modestr!='O'/*DNT*/)
                        msgn = 29;
                    else 
                        type = 1;
                    break;
                case 'O'/*DNT*/:
                    if (*modestr!='P'/*DNT*/)
                        msgn = 29;
                    else
                        type = 0;
                    break;
                }
            } else 
                msgn=29;

            if (msgn>-1) goto out;

        } else if (thisarg->restype==RTLB || thisarg->restype==RTNIL) {

            /* If there's no mode string, it must be a point, an entmask, */
            /* or a point followed by an entmask. */

            if (thisarg->restype==RTLB && thisarg->rbnext==NULL)
                { msgn=5; goto out; }

            if (thisarg->restype==RTNIL ||
                thisarg->rbnext->restype==RTLB) {  /* Try for entmask */

                if ((fi1=lsp_convassoc(thisarg,&entmask,0))==-2)
                    { msgn=7; goto out; }
                if (fi1) { msgn=31; goto out; }

            } else {  /* Try for pt */

                /* Find end of list: */
                if ((tp1=lsp_argend(thisarg,NULL))==NULL) { msgn=5; goto out; }

                /* Chop, copy, repair: */
                tp2=tp1->rbnext; tp1->rbnext=NULL;
                if ((tlist==NULL &&
                     (tlist=db_alloc_resbuf())==NULL) ||
                    lsp_copysplrb(tlist,thisarg)) msgn=7;
                tp1->rbnext=tp2;
                if (msgn>-1) goto out;

                /* Try to convert to point-form: */
                lsp_convptlist(tlist,0);
                if (tlist->restype!=RT3DPOINT && tlist->restype!=RTPOINT)
                    { msgn=1; goto out; }  /* Not a pt */

                ic_ptcpy(p1,tlist->resval.rpoint);
                p1p=p1;

                /* We check later for a entmask after the pt. */

            }

        } else { msgn=1; goto out; }

        /* Step past 1st arg: */
        thisarg=lsp_nextarg(thisarg);

    }  /* End if lsp_argsll!=NULL */


    if (type) {  /* Needs 2 pts, or a list of pts */

        if (thisarg==NULL)          { msgn=2; goto out; }
        if (thisarg->restype!=RTLB) { msgn=1; goto out; }
        if (thisarg->rbnext==NULL)  { msgn=5; goto out; }

        if (type==1) {  /* Needs a list of pts */

            /* thisarg should point to a list of lists that looks like this: */
            /*     ((x y z) (x y z) ... (x y z)) */

            /* Find end of list of lists: */
            if ((tp1=lsp_argend(thisarg,NULL))==NULL) { msgn=5; goto out; }

            /* Chop, copy, repair: */
            tp2=tp1->rbnext; tp1->rbnext=NULL;
            if ((tlist==NULL &&
                 (tlist=db_alloc_resbuf())==NULL) ||
                lsp_copysplrb(tlist,thisarg)) msgn=7;
            tp1->rbnext=tp2;
            if (msgn>-1) goto out;

            /* Try to convert to point-form: */
            lsp_convptlist(tlist,0);

            /* Now tlist should look like this: (p1 p2 ... pn) */
            /* where the members are RTPOINTs or RT3DPOINTs */

            if (tlist->restype!=RTLB || tlist->rbnext==NULL ||
                tlist->rbnext->restype==RTLE) { msgn=30; goto out; }

            /* Find link just before the RTLE that ends the list of pts: */
            if (lsp_argend(tlist,&tp1)==NULL || tp1==NULL)
                { msgn=5; goto out; }

            /* Chop, copy, repair: */
            tp2=tp1->rbnext; tp1->rbnext=NULL;
            if ((ptlist==NULL &&
                 (ptlist=db_alloc_resbuf())==NULL) ||
                lsp_copysplrb(ptlist,tlist->rbnext)) msgn=7;
            tp1->rbnext=tp2;
            if (msgn>-1) goto out;

            /* Now -- check for at least 2 pts (ACAD page-faults with */
            /* only 1): */
            if ((ptlist->restype!=RT3DPOINT) &&
                (ptlist->rbnext==NULL || 
                (ptlist->rbnext->restype!=RT3DPOINT && 
                        ptlist->rbnext->restype!=RTPOINT &&
                                ptlist->rbnext->restype!=RTREAL)) )
                { msgn=30; goto out; }

            p1p=ptlist;

            /* Step past list of pts: */
            thisarg=lsp_nextarg(thisarg);

        } else if (type==2) {  /* Needs 2 pts */

            /* Find end of 2nd pt: */
            if ((tp1=lsp_nextarg(thisarg))==NULL) { msgn=2; goto out; }
            if ((tp1=lsp_argend(tp1,NULL))==NULL) { msgn=5; goto out; }

            /* Chop, copy, repair: */
            tp2=tp1->rbnext; tp1->rbnext=NULL;
            if ((tlist==NULL &&
                 (tlist=db_alloc_resbuf())==NULL) ||
                 lsp_copysplrb(tlist,thisarg)) msgn=7;
            tp1->rbnext=tp2;
            if (msgn>-1) goto out;

            /* DON'T REUSE tp1 YET!  (Used again at end of this block.) */

            /* Try to convert to point-form: */
            lsp_convptlist(tlist,0);

            /* Make sure they're both pts: */
            if ((tlist->restype!=RT3DPOINT && tlist->restype!=RTPOINT) ||
                 tlist->rbnext==NULL ||
                (tlist->rbnext->restype!=RT3DPOINT &&
                 tlist->rbnext->restype!=RTPOINT)) { msgn=28; goto out; }

            /* Take the pts: */
            ic_ptcpy(p1,tlist->resval.rpoint);         p1p=p1;
            ic_ptcpy(p2,tlist->rbnext->resval.rpoint); p2p=p2;

            /* Step past 2nd pt: */
            thisarg=tp1->rbnext;

        }

    }  /* End if type */


    /* If we don't already have an entmask and there's another arg, */
    /* try to get an entmask from it: */
    if (entmask==NULL && thisarg!=NULL) {
        if ((fi1=lsp_convassoc(thisarg,&entmask,0))==-2) { msgn=7; goto out; }
        if (fi1) { msgn=31; goto out; }
        thisarg=lsp_nextarg(thisarg);  /* Step past entmask */
    }

    /* If we have an entmask and there are more args, that's too many: */
    if (entmask!=NULL && thisarg!=NULL) { msgn=9; goto out; }

    if ((fi1=sds_ssget(modestr,p1p,p2p,entmask,ss))==RTNORM) {

        /* Add ss to the llist of valid selection sets: */
		/*DG - 25.1.2002*/// Add ss to lsp_ssll w/o freeing of lsp_ssll which is made in lsp_add2ssll.
		lsp_sslink*	pNewSSitem = new lsp_sslink ;
		if(!pNewSSitem)
		{
			sds_ssfree(ss);
			msgn = 7;
			goto out;
		}
		memset(pNewSSitem,0,sizeof(lsp_sslink));
        ic_encpy(pNewSSitem->ss, ss);
        pNewSSitem->next = lsp_ssll;
		lsp_ssll = pNewSSitem;
        //if (lsp_add2ssll(ss)) { sds_ssfree(ss); msgn=7; goto out; }

        lsp_retname(ss,RTPICKS);

    } else if (fi1==RTCAN) {
        msgn=18;
    } else {
        /* Probably nothing found; lsp_convassoc() */
        /* checks pretty well for bad entmasks. */

        lsp_retnil();
    }

    /* If we add anything here, do goto out's where necessary */
    /* in the if/else above. */


out:
    if (entmask!=NULL) { lsp_freesuprb(entmask); entmask=NULL; }
    if (ptlist!= NULL) { lsp_freesuprb(ptlist);  ptlist= NULL; }
    if (tlist!=  NULL) { lsp_freesuprb(tlist);   tlist=  NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_sslength(void) {
    int msgn;
    long fl1;

    msgn=-1; fl1=0L;

    if (lsp_argsll==NULL)             { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTPICKS) { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)     { msgn=9; goto out; }

    if (sds_sslength(lsp_argsll->resval.rlname,&fl1)!=RTNORM)
        { lsp_retnil(); goto out; }

    if (fl1>SHRT_MAX) lsp_retreal((sds_real)fl1);
    else              lsp_retint((int)fl1);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_ssname(void) {
    int msgn;
    long fl1;
    struct resbuf retrb;

    msgn=-1; fl1=0L;
    retrb.restype=RTENAME; retrb.rbnext=NULL;

    if (lsp_argsll==NULL || lsp_argsll->rbnext==NULL) { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTPICKS) { msgn=1; goto out; }
    if (lsp_argsll->rbnext->restype==RTLONG)
        fl1=lsp_argsll->rbnext->resval.rlong;
    else if (lsp_argsll->rbnext->restype==RTREAL ||
             lsp_argsll->rbnext->restype==RTANG  ||
             lsp_argsll->rbnext->restype==RTORINT)
                fl1=(long)lsp_argsll->rbnext->resval.rreal;
    else if (lsp_argsll->rbnext->restype==RTSHORT)
        fl1=(long)lsp_argsll->rbnext->resval.rint;
    else { msgn=32; goto out; }
    if (lsp_argsll->rbnext->rbnext!=NULL) { msgn=9; goto out; }

    if (sds_ssname(lsp_argsll->resval.rlname,fl1,retrb.resval.rlname)!=RTNORM)
        retrb.restype=RTNIL;

    if (lsp_retval(&retrb)!=RTNORM) msgn=7;

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_ssmemb(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL || lsp_argsll->rbnext==NULL) { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTENAME || lsp_argsll->rbnext->restype!=RTPICKS)
        { msgn=1; goto out; }
    if (lsp_argsll->rbnext->rbnext!=NULL) { msgn=9; goto out; }

    if (sds_ssmemb(lsp_argsll->resval.rlname,
        lsp_argsll->rbnext->resval.rlname)!=RTNORM) lsp_retnil();
    else if (lsp_retval(lsp_argsll)!=RTNORM) msgn=7;

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_ssadd(void) {
    int msgn;
    struct resbuf *argp[2],retrb;

    msgn=-1; argp[0]=argp[1]=NULL;
    retrb.restype=RTPICKS; retrb.rbnext=NULL;

    if ((argp[0]=lsp_argsll)!=NULL) {
        if (argp[0]->restype!=RTENAME) { msgn=1; goto out; }
        if ((argp[1]=argp[0]->rbnext)!=NULL) {
            if (argp[1]->restype!=RTPICKS) { msgn=1; goto out; }
            if (argp[1]->rbnext!=NULL) { msgn=9; goto out; }
        }
    }

	// Modified CyberAge AP 11/--/2000
	cmd_ssadd=1;
    if (sds_ssadd((argp[0]==NULL) ? NULL : argp[0]->resval.rlname,
        (argp[1]==NULL) ? NULL : argp[1]->resval.rlname,
        retrb.resval.rlname)!=RTNORM) {

        retrb.restype=RTNIL;  /* Shouldn't fail, but... */

    } else {  /* It worked. */
        /* If the ss arg is NOT present, we created a new ss; */
        /* add it to the llist of valid selection sets: */
        if(!argp[1])
		{
			/*DG - 25.1.2002*/// Add ss to lsp_ssll w/o freeing of lsp_ssll which is made in lsp_add2ssll.
			lsp_sslink*	pNewSSitem = new lsp_sslink ;
			if(!pNewSSitem)
			{
				sds_ssfree(retrb.resval.rlname);
				msgn = 7;
				goto out;
			}
			memset(pNewSSitem,0,sizeof(lsp_sslink));
			ic_encpy(pNewSSitem->ss, retrb.resval.rlname);
			pNewSSitem->next = lsp_ssll;
			lsp_ssll = pNewSSitem;
			//if(lsp_add2ssll(retrb.resval.rlname))
			//{ sds_ssfree(retrb.resval.rlname); msgn=7; goto out; }
		}
    }
	// Modified CyberAge AP 11/--/2000
	cmd_ssadd=0;

    if (lsp_retval(&retrb)!=RTNORM) msgn=7;

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_ssdel(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL || lsp_argsll->rbnext==NULL) { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTENAME || lsp_argsll->rbnext->restype!=RTPICKS)
        { msgn=1; goto out; }
    if (lsp_argsll->rbnext->rbnext!=NULL) { msgn=9; goto out; }

    if (sds_ssdel(lsp_argsll->resval.rlname,
        lsp_argsll->rbnext->resval.rlname)!=RTNORM) lsp_retnil();
    else if (lsp_retval(lsp_argsll->rbnext)!=RTNORM) msgn=7;

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

