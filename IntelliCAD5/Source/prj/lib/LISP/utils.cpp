/* C:\ICAD\PRJ\LIB\LISP\UTILS.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/
#include "icad.h"
#include "IcadMain.h"
#include "IcadTablet.h"/*DNT*/

int lsp_osnap(void) {
    int msgn;
    sds_point ap1;

    msgn=-1; ap1[0]=ap1[1]=ap1[2]=0.0;

    /* Convert pts from list-form to pt-form. */
    lsp_convptlist(lsp_argsll,0);

    if (lsp_argsll==NULL)                         { msgn=2; goto out; }
    if (lsp_argsll->restype!=RT3DPOINT &&
        lsp_argsll->restype!=RTPOINT)             { msgn=1; goto out; }
    if (lsp_argsll->rbnext==NULL)                 { msgn=2; goto out; }
    if (lsp_argsll->rbnext->restype!=RTSTR ||
        lsp_argsll->rbnext->resval.rstring==NULL) { msgn=1; goto out; }
    if (lsp_argsll->rbnext->rbnext!=NULL)         { msgn=9; goto out; }

    if (sds_osnap(lsp_argsll->resval.rpoint,
        lsp_argsll->rbnext->resval.rstring,ap1)==RTNORM) {

        if (lsp_retpoint(ap1)!=RTNORM) { msgn=7; goto out; } /* Always 3D */
    } else lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_polar(void) {
/*
**  If given pt is 2D, so is the result.
*/
    int msgn,type;
    sds_real ang,dist;
    struct resbuf *tp1;

    msgn=-1; type=RT3DPOINT;

    /* Convert pts from list-form to pt-form. */
    lsp_convptlist(lsp_argsll,0);

    if ((tp1=lsp_argsll)==NULL) { msgn=2; goto out; }
    if (tp1->restype!=RT3DPOINT && tp1->restype!=RTPOINT)
        { msgn=1; goto out; }
    type=tp1->restype;
    if ((tp1=tp1->rbnext)==NULL) { msgn=2; goto out; }
    if      (tp1->restype==RTLONG)  ang=(sds_real)tp1->resval.rlong;
    else if (tp1->restype==RTSHORT) ang=(sds_real)tp1->resval.rint;
    else if (tp1->restype==RTREAL || tp1->restype==RTANG ||
        tp1->restype==RTORINT) ang=tp1->resval.rreal;
    else { msgn=1; goto out; }
    if ((tp1=tp1->rbnext)==NULL) { msgn=2; goto out; }
    if      (tp1->restype==RTLONG)  dist=(sds_real)tp1->resval.rlong;
    else if (tp1->restype==RTSHORT) dist=(sds_real)tp1->resval.rint;
    else if (tp1->restype==RTREAL || tp1->restype==RTANG ||
        tp1->restype==RTORINT) dist=tp1->resval.rreal;
    else { msgn=1; goto out; }
    if (tp1->rbnext!=NULL) { msgn=9; goto out; }

    lsp_freesplrb(&lsp_evres);  /* Default result is nil. */

    sds_polar(lsp_argsll->resval.rpoint,ang,dist,lsp_evres.resval.rpoint);

    lsp_evres.restype=type;
    /* Convert to list-form (2D or 3D): */
    if (lsp_convptlist(&lsp_evres,1)) { msgn=7; goto out; }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_textbox(void) {
    int msgn,fi1;
    struct resbuf *elist,*res,*tp1,*tp2;

    msgn=-1; elist=res=NULL;

    lsp_freesplrb(&lsp_evres);  /* Default result is nil. */

    if (lsp_argsll==NULL)                  { msgn=2; goto out; }
    if (lsp_argsll->restype==RTNIL)                  goto out;   /* nil */
    if (lsp_argsll->restype!=RTLB)         { msgn=1; goto out; }
    if (lsp_argsll->rbnext==NULL)          { msgn=5; goto out; }
    if (lsp_argsll->rbnext->restype==RTLE)           goto out;   /* nil */
    if (lsp_nextarg(lsp_argsll)!=NULL)     { msgn=9; goto out; }

    /* Convert LISP assoc list to ADS form: */
    if ((fi1=lsp_convassoc(lsp_argsll,&elist,0))==-1) { msgn=47; goto out; }
    else if (fi1) { msgn=7; goto out; }

    /* Make the result list -- except for the RTLB -- */
    /* RT3DPOINT/RT3DPOINT/RTLE: */
    tp2=NULL;
    for (fi1=0; fi1<3; fi1++) {
        if ((tp1=db_alloc_resbuf())==NULL)
            { msgn=7; goto out; }
        tp1->restype=(fi1<2) ? RT3DPOINT : RTLE;
        if (tp2==NULL) res=tp1; else tp2->rbnext=tp1;
        tp2=tp1;
    }

    if (sds_textbox(elist,res->resval.rpoint,res->rbnext->resval.rpoint)!=
        RTNORM) goto out;  /* Take the nil; (textbox '((2 . "test"))) */
                           /* for example.  A valid assoc list fragment, */
                           /* but insufficient info according to ERRNO. */
                           /* (textbox '((1 . "test"))) works, however. */

    /* Tack it onto the result and convert pts to LISP-form: */
    lsp_evres.restype=RTLB;
    lsp_evres.resval.rstring=NULL;
    lsp_evres.rbnext=res; res=NULL;
    if (lsp_convptlist(&lsp_evres,1)) { msgn=7; goto out; }

out:
    if (elist!=NULL) { lsp_freesuprb(elist); elist=NULL; }
    if (res!=NULL)   { lsp_freesuprb(res);   res=  NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}


int lsp_vports(void) {
    int msgn;
    struct resbuf *res,*rtlelink,*tp1;

    msgn=-1; res=rtlelink=NULL;

    if (lsp_argsll!=NULL) { msgn=9; goto out; }

    if (sds_vports(&res)!=RTNORM ||	res==NULL ||
        (rtlelink=db_alloc_resbuf())==NULL)
            { msgn=7; goto out; }

    rtlelink->restype=RTLE;
    rtlelink->resval.rstring=NULL;

    lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value */
                                /* (since we're not doing an sds_ret...). */

    lsp_evres.restype=RTLB;
    lsp_evres.resval.rstring=NULL;
    lsp_evres.rbnext=res; res=NULL;
    tp1=lsp_evres.rbnext; while (tp1->rbnext!=NULL) tp1=tp1->rbnext;
    tp1->rbnext=rtlelink; rtlelink=NULL;

    /* Convert pts to list-form: */
    if (lsp_convptlist(&lsp_evres,1)) { msgn=7; goto out; }

out:
    if (res!=NULL)      { lsp_freesuprb(res);      res=     NULL; }
    if (rtlelink!=NULL) { lsp_freesuprb(rtlelink); rtlelink=NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}


int lsp_wcmatch(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL)                   { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR)         { msgn=1; goto out; }
    if (lsp_argsll->rbnext==NULL)           { msgn=2; goto out; }
    if (lsp_argsll->rbnext->restype!=RTSTR) { msgn=1; goto out; }
    if (lsp_argsll->rbnext->rbnext!=NULL)   { msgn=9; goto out; }

    if (sds_wcmatch(lsp_argsll->resval.rstring,
        lsp_argsll->rbnext->resval.rstring)==RTNORM) lsp_rett();
    else lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}


int lsp_acad_colordlg(void) { return lsp_invoker(0); }
int lsp_acad_helpdlg(void)  { return lsp_invoker(1); }
int lsp_acad_strlsort(void) { return lsp_invoker(2); }

int lsp_invoker(int what) {
    int msgn,makelist,invret,fi1;
    struct resbuf *newlink,*res,*tp1;

    char *toinvoke[]={
        "ACAD_COLORDLG"/*DNT*/,      /*  0 */
        "ACAD_HELPDLG"/*DNT*/,       /*  1 */
        "ACAD_STRLSORT"/*DNT*/       /*  2 */
    };


    msgn=-1; newlink=res=NULL;

    /*
    **  AutoLISP seems to scan the args and give bad arg type if it
    **  finds any symbols, instead of letting the ADS function report
    **  the error.  We simply remove the super resbuf args with
    **  lsp_demote() below.  But, to match their messages, we can check
    **  for symbols here:
    */
    for (tp1=lsp_argsll; tp1!=NULL &&
        tp1->restype!=LSP_RTSYMB; tp1=tp1->rbnext);
    if (tp1!=NULL) { msgn=1; goto out; }

    lsp_demote(&lsp_argsll,1);  /* Demote args to a normal resbuf llist. */

    /* Add the name of the function to invoke to the beginning: */
    if ((newlink=db_alloc_resbuf())==NULL ||
        (newlink->resval.rstring= new char [strlen(toinvoke[what])+1])==
            NULL) { msgn=7; goto out; }
    strcpy(newlink->resval.rstring,toinvoke[what]);
    newlink->restype=RTSTR; newlink->rbnext=lsp_argsll; lsp_argsll=newlink;
    newlink=NULL;

    if ((invret=sds_invoke(lsp_argsll,&res))!=RTNORM || res==NULL) {
        /*
        **  A nil result seems to be the proper response here, but if you
        **  ever need to check what's going on, here's the code to do it.
        **  If we need to, we can check what against invret and res to
        **  decide what to return in each case.
        **
        **  { ///
        **      char fs1[41];
        **
        **      ic_adscode(invret,fs1);
        **      sds_printf("\ninvret: |%s|; res: %s",fs1,
        **          (res==NULL) ? "NULL" : "non-NULL");
        **  }
        */

        lsp_retnil();

        goto out;
    }

    makelist=(what==2 && res!=NULL && res->restype==RTSTR);

    if (makelist) {
        /* ADS function should have put RTLB/RTLE around the result -- */
        /* but it didn't, so we'll have to: */

        if (res==NULL) { lsp_retnil(); goto out; }

        if ((newlink=db_alloc_resbuf())==NULL)
            { msgn=7; goto out; }
        newlink->restype=RTLB;
        newlink->resval.rstring=NULL;
        newlink->rbnext=res; res=newlink;
        newlink=NULL;

        tp1=res; while (tp1->rbnext!=NULL) tp1=tp1->rbnext;
        if ((tp1->rbnext=db_alloc_resbuf())==
            NULL) { msgn=7; goto out; }
        tp1->rbnext->restype=RTLE;
        tp1->rbnext->resval.rstring=NULL;
    }

    if (lsp_retval(res)!=RTNORM) { msgn=7; goto out; }

    /* Convert any RTSHORT results to RTLONGs: */
    for (tp1=&lsp_evres; tp1!=NULL; tp1=tp1->rbnext) {
        if (lsp_evres.restype!=RTSHORT) continue;
        /* Don't try direct intra-union assignment: */
        fi1=lsp_evres.resval.rint; lsp_evres.restype=RTLONG;
        lsp_evres.resval.rlong=(long)fi1;
    }

out:
    if (newlink!=NULL) { lsp_freesuprb(newlink); newlink=NULL; }
    if (res!=NULL)     { lsp_freesuprb(res);         res=NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_tablet(void) {
    int msgn, fi1;
    struct resbuf *res,*tp1,*tp2;

    msgn=-1; res=NULL;

    /* Convert pts from list-form to pt-form: */
    lsp_convptlist(lsp_argsll,0);

    if ((tp1=lsp_argsll)==NULL) { msgn=2; goto out; }
    if (tp1->restype==RTLONG) {
        /* Convert to RTSHORT (don't try intra-union assignment): */
        fi1=(int)tp1->resval.rlong;
        tp1->restype=RTSHORT; tp1->resval.rint=fi1;
    } else if (tp1->restype!=RTSHORT) { msgn=1; goto out; }
    if (tp1->resval.rint==1) {
        for (tp1=tp1->rbnext,fi1=0; tp1!=NULL && fi1<4; tp1=tp1->rbnext,fi1++) {
            if (tp1->restype==RTPOINT) {
                /* Set Z as 0.0 and change to 3D point: */
                tp1->resval.rpoint[2]=0.0; tp1->restype=RT3DPOINT;
            } else if (tp1->restype!=RT3DPOINT) { msgn=1; goto out; }
        }
        if (fi1<4) { msgn=2; goto out; }
    } else if (tp1->resval.rint==0) {
        tp1=tp1->rbnext;
    } else { msgn=53; goto out; }
    if (tp1!=NULL) { msgn=9; goto out; }

    lsp_freesplrb(&lsp_evres);  /* Default result is nil. */

    if (sds_tablet(lsp_argsll,&res)==RTNORM) {
        /* Steal res and surround with parentheses: */
        if ((tp2=db_alloc_resbuf())==
            NULL) { msgn=7; goto out; }
        tp2->restype=RTLE;
        tp2->resval.rstring=NULL;
        lsp_evres.restype=RTLB;
        lsp_evres.resval.rstring=NULL;
        lsp_evres.rbnext=res; res=NULL;
        tp1=&lsp_evres; while (tp1->rbnext!=NULL) tp1=tp1->rbnext;
        tp1->rbnext=tp2; tp2=NULL;
        /* Convert pts from pt-form to list-form: */
        if (lsp_convptlist(&lsp_evres,1)) { msgn=7; goto out; }
    }

out:
    if (res!=NULL)  { lsp_freesuprb(res);   res=NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_tablet_setScreenSize(void) 
	{
    int msgn;
	ASSERT (SDS_CMainWindow->m_pTablet);
	if (!SDS_CMainWindow->m_pTablet)
		{
		msgn = -1;
		goto out;
		}

	int nIndex[4], i;
    struct resbuf *argsll,*tp1;

    msgn=-1;

    argsll=lsp_argsll; lsp_argsll=NULL;

    if (argsll==NULL || argsll->rbnext==NULL) 
		{ msgn=2; goto out; }

    for (tp1=argsll, i=0; (tp1!=NULL || i < 4); tp1=lsp_nextarg(tp1), i++) 
		{
		if (tp1->restype==RTLONG) 
			nIndex[i] = (int)tp1->resval.rlong;
		else if (tp1->restype==RTSHORT) 
			nIndex[i] = (int)tp1->resval.rint;
		else
			{ msgn=1; goto out; }

		if ((nIndex[i] < 0) || (nIndex[i] > 26))
			{ msgn=10; goto out; }
		}
    sds_point pt;
	SDS_CMainWindow->m_pTablet->SetScreenSize (nIndex[0], nIndex[1], nIndex[2], nIndex[3], pt, pt);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
	}


int lsp_tablet_setButton(void) 
	{
    int msgn;
	ASSERT (SDS_CMainWindow->m_pTablet);
	if (!SDS_CMainWindow->m_pTablet)
		{
		msgn = -1;
		goto out;
		}

	int nButton;

    msgn=-1;

    if (lsp_argsll==NULL || lsp_argsll->rbnext==NULL) 
		{ msgn=2; goto out; }

    if (lsp_argsll->restype==RTLONG) 
        nButton = (int)lsp_argsll->resval.rlong;
    else if (lsp_argsll->restype==RTSHORT) 
        nButton = (int)lsp_argsll->resval.rint;
	else
		{ msgn=1; goto out; }

	ASSERT ((nButton >= 0) && (nButton <= 63));
	if ((nButton < 0) || (nButton > 63))
		{ msgn=10; goto out; }
    
	if (!lsp_argsll->rbnext)
		{ msgn=1; goto out; }

    if (lsp_argsll->rbnext->restype!=RTSTR) 
		{ msgn=1; goto out; }

	if (!lsp_argsll->rbnext->resval.rstring)
		{ msgn=1; goto out; }

	SDS_CMainWindow->m_pTablet->SetButton (nButton, lsp_argsll->rbnext->resval.rstring);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
	}


int lsp_tablet_setGridItem(void) 
	{
    int msgn;
	ASSERT (SDS_CMainWindow->m_pTablet);
	if (!SDS_CMainWindow->m_pTablet)
		{
		msgn = -1;
		goto out;
		}

	int nIndex[2], i;
    struct resbuf *argsll,*tp1;

    msgn=-1;

    argsll=lsp_argsll; lsp_argsll=NULL;
    if (argsll==NULL || argsll->rbnext==NULL) 
		{ msgn=2; goto out; }

    for (tp1=argsll, i=0; (tp1!=NULL && i < 2); tp1=lsp_nextarg(tp1), i++) 
		{
		if (tp1->restype==RTLONG) 
			nIndex[i] = (int)tp1->resval.rlong - 1; //decrement by 1 to get zero based index
		else if (tp1->restype==RTSHORT) 
			nIndex[i] = (int)tp1->resval.rint - 1; //decrement by 1 to get zero based index;
		else
			{ msgn=1; goto out; }

		ASSERT ((nIndex[i] >= 0) && (nIndex[i] <= 25));
		if ((nIndex[i] < 0) || (nIndex[i] > 25))
			{ msgn=10; goto out; }
		}

	if (!tp1)
		{ msgn=1; goto out; }

    if (tp1->restype!=RTSTR) 
		{ msgn=1; goto out; }

	if (!tp1->resval.rstring)
		{ msgn=1; goto out; }

	SDS_CMainWindow->m_pTablet->SetGridItem (nIndex[1], nIndex[0], tp1->resval.rstring);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
	}

int lsp_tablet_setGridSize(void) 
	{
    int msgn;
	ASSERT (SDS_CMainWindow->m_pTablet);
	if (!SDS_CMainWindow->m_pTablet)
		{
		msgn = -1;
		goto out;
		}

	int nIndex[2], i;
    struct resbuf *argsll,*tp1;

    msgn=-1;

    argsll=lsp_argsll; lsp_argsll=NULL;
    if (argsll==NULL || argsll->rbnext==NULL) 
		{ msgn=2; goto out; }

    for (tp1=argsll, i=0; (tp1!=NULL || i < 2); tp1=lsp_nextarg(tp1), i++) 
		{
		if (tp1->restype==RTLONG) 
			nIndex[i] = (int)tp1->resval.rlong;
		else if (tp1->restype==RTSHORT) 
			nIndex[i] = (int)tp1->resval.rint;
		else
			{ msgn=1; goto out; }

		//ASSERT ((nIndex[i] >= 0) && (nIndex[i] < 30));
		//if ((nIndex[i] < 0) || (nIndex[i] > 29))
		//	{ msgn=10; goto out; }
		}
	SDS_CMainWindow->m_pTablet->SetGridDimensions (nIndex[0], nIndex[1]);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
	}

int lsp_tablet_setDefaults(void) 
	{
    int msgn;
	ASSERT (SDS_CMainWindow->m_pTablet);
	if (!SDS_CMainWindow->m_pTablet)
		{
		msgn = -1;
		return -1;
		}

	SDS_CMainWindow->m_pTablet->SetDefaults(); 
	return 0;
	}

int lsp_tablet_getButton(void) 
	{
    int msgn;
	ASSERT (SDS_CMainWindow->m_pTablet);
	if (!SDS_CMainWindow->m_pTablet)
		{
		msgn = -1;
		goto out;
		}

	int nButton;

    msgn=-1;

    if (lsp_argsll==NULL) 
		{ msgn=2; goto out; }

    if (lsp_argsll->restype==RTLONG) 
        nButton = (int)lsp_argsll->resval.rlong;
    else if (lsp_argsll->restype==RTSHORT) 
        nButton = (int)lsp_argsll->resval.rint;
	else
		{ msgn=1; goto out; }

	ASSERT ((nButton >= 0) && (nButton <= 63));
	if ((nButton < 0) || (nButton > 63))
		{ msgn=10; goto out; }

    char Command[256];
	if (SDS_CMainWindow->m_pTablet->GetButton (nButton, Command))
        lsp_retstr(Command);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
	}


int lsp_tablet_getGridItem(void) 
	{
    int msgn;
	ASSERT (SDS_CMainWindow->m_pTablet);
	if (!SDS_CMainWindow->m_pTablet)
		{
		msgn = -1;
		goto out;
		}

	int nIndex[2], i;
    struct resbuf *argsll,*tp1;

    msgn=-1;

    argsll=lsp_argsll; lsp_argsll=NULL;
    if (argsll==NULL || argsll->rbnext==NULL) 
		{ msgn=2; goto out; }

    for (tp1=argsll, i=0; (tp1!=NULL && i < 2); tp1=lsp_nextarg(tp1), i++) 
		{
		if (tp1->restype==RTLONG) 
			nIndex[i] = (int)tp1->resval.rlong - 1; //decrement by 1 to get zero based index
		else if (tp1->restype==RTSHORT) 
			nIndex[i] = (int)tp1->resval.rint - 1; //decrement by 1 to get zero based index;
		else
			{ msgn=1; goto out; }

		ASSERT ((nIndex[i] >= 0) && (nIndex[i] <= 25));
		if ((nIndex[i] < 0) || (nIndex[i] > 25))
			{ msgn=10; goto out; }
		}

    char Command[256];
	if (SDS_CMainWindow->m_pTablet->GetGridItem (nIndex[1], nIndex[0], Command))
        lsp_retstr(Command);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
	}

int lsp_setview(void) {
    int msgn,vpid,fi1;
    struct resbuf *elist,*argp[2],*endp;

    msgn=-1; elist=NULL; vpid=0;

    if ((argp[0]=lsp_argsll)==NULL)            { msgn=2; goto out; }
    if (argp[0]->restype==RTNIL)               { msgn=41; goto out; }
    if (argp[0]->restype!=RTLB)                { msgn=1; goto out; }
    if ((endp=lsp_argend(argp[0],NULL))==NULL) { msgn=21; goto out; }
    if ((argp[1]=endp->rbnext)!=NULL) {
        if      (argp[1]->restype==RTLONG)  vpid=argp[1]->resval.rlong;
        else if (argp[1]->restype==RTSHORT) vpid=argp[1]->resval.rint;
        else { msgn=1; goto out; }
        if (lsp_nextarg(argp[1])!=NULL) { msgn=9; goto out; }

        /* Chop after argp[0] so it's all we have: */
        lsp_freesuprb(argp[1]); endp->rbnext=NULL;
    }

    /* Convert the resbuf llist from LISP to ADS format: */
    if ((fi1=lsp_convassoc(argp[0],&elist,0))==-2) { msgn=7; goto out; }
    if (fi1) { msgn=21; goto out; }

    if (sds_setview(elist,vpid)==RTNORM) {
        /* Steal lsp_argsll to set lsp_evres: */
        lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value. */
        /* Set 1st link to RTLB and link chain onto it: */
        lsp_evres.restype=RTLB; lsp_evres.rbnext=lsp_argsll->rbnext;
        lsp_evres.resval.rstring=NULL;   /* Clear RTLB/RTLE matching. */
        lsp_argsll->rbnext=NULL;  /* Important! */
        lsp_argsll->resval.rstring=NULL; /* Clear RTLB/RTLE matching. */
    } else {
        lsp_retnil();
    }

out:
    if (elist!=NULL) { lsp_freesuprb(elist); elist=NULL; }

    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_menucmd(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL)                 { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR ||
        lsp_argsll->resval.rstring==NULL) { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)         { msgn=9; goto out; }

    lsp_freesplrb(&lsp_evres);  /* Default result is nil. */

    if (sds_menucmd(lsp_argsll->resval.rstring)==RTKWORD) {
        if ((lsp_evres.resval.rstring= new char [512])==NULL)
            { msgn=7; goto out; }
        lsp_evres.restype=RTSTR;
        if (sds_getinput(lsp_evres.resval.rstring)!=RTNORM) {
            IC_FREE(lsp_evres.resval.rstring); lsp_evres.resval.rstring=NULL;
            lsp_evres.restype=RTNIL;
        }
    }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_menugroup(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL)           { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR) { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)   { msgn=9; goto out; }

    if (sds_menugroup(lsp_argsll->resval.rstring)==RTNORM)
        lsp_retstr(lsp_argsll->resval.rstring);
    else lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}



int lsp_regapp(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL)                 { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR ||
        lsp_argsll->resval.rstring==NULL) { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)         { msgn=9; goto out; }

    if (sds_regapp(lsp_argsll->resval.rstring)==RTNORM) {
        if (lsp_retstr(lsp_argsll->resval.rstring)!=RTNORM)
            { msgn=7; goto out; }
    } else lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}






int lsp_gettbpos(void) {
    int msgn;
	sds_point pt1;

    msgn=-1;

    if (lsp_argsll==NULL)           { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR || lsp_argsll->resval.rstring==NULL)
                                    { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)   { msgn=9; goto out; }

    if (sds_gettbpos(lsp_argsll->resval.rstring,pt1)!=RTNORM) {
		lsp_retnil();
		goto out;
	}

    if (lsp_retpoint(pt1)!=RTNORM) { msgn=7; goto out; }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}




