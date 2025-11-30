/* C:\ICAD\PRJ\LIB\LISP\TABLE.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/

// Modified CyberAge AP 09/04/2000
// Reason: Fix for bug no. 46742
void lsp_modify_adslist(struct resbuf **adslist, char *tbname); 

int lsp_tblobjname(void) {
    int msgn;
    struct resbuf *argp[2];
	sds_name ename;

    msgn=-1; 

    if ((argp[0]=lsp_argsll)==NULL)      { msgn=2; goto out; }
    if (argp[0]->restype!=RTSTR || argp[0]->resval.rstring==NULL)
                                         { msgn=1; goto out; }
    if ((argp[1]=argp[0]->rbnext)==NULL) { msgn=2; goto out; }
    if (argp[1]->restype!=RTSTR || argp[1]->resval.rstring==NULL)
                                         { msgn=1; goto out; }
    if (lsp_nextarg(argp[1])!=NULL)      { msgn=9; goto out; }

    if (sds_tblobjname(argp[0]->resval.rstring,argp[1]->resval.rstring,ename)!=RTNORM) {
		// Modified CyberAge AP 10/18/2000 [
		// If Symbol table entry not found in in the required table 
		// it should returns nil.
		lsp_retnil();
		// Modified CyberAge AP 10/18/2000 ]
		goto out;
	}

    if (lsp_retname(ename,RTENAME)!=RTNORM) { msgn=7; goto out; }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_tblsearch(void) { return lsp_tblstuff(0); }
int lsp_tblnext(void)   { return lsp_tblstuff(1); }

int lsp_tblstuff(int mode) {
    char *tabnm,*sym;
    int msgn,fi1;
    int flag;
    struct resbuf *adslist,*lisplist,*tp1;

    msgn=-1;
    tabnm=sym=NULL; flag=0; adslist=lisplist=NULL;

    if ((tp1=lsp_argsll)==NULL) { msgn=2; goto out; }
    if (tp1->restype!=RTSTR || tp1->resval.rstring==NULL)
        { msgn=1; goto out; }
    tabnm=tp1->resval.rstring;
    if ((tp1=tp1->rbnext)==NULL) {
        if (!mode) { msgn=2; goto out; }
    } else if (!mode) {
        if (tp1->restype!=RTSTR || tp1->resval.rstring==NULL)
            { msgn=1; goto out; }
        sym=tp1->resval.rstring;
        tp1=tp1->rbnext;
    }
    if (tp1!=NULL) {
        if (tp1->restype!=RTNIL) flag=1;
        if (tp1->rbnext!=NULL) { msgn=9; goto out; }
    }

    if ((adslist=(mode) ?
        sds_tblnext(tabnm,flag) : sds_tblsearch(tabnm,sym,flag))==NULL)
            { lsp_retnil(); goto out; }
	// Modified CyberAge AP 09/04/2000
	lsp_modify_adslist(&adslist,tabnm);	// Reason: Fix for bug no. 46742
    /* Convert ADS assoc list to LISP form: */
    if ((fi1=lsp_convassoc(adslist,&lisplist,1))==-1) { msgn=22; goto out; }
    else if (fi1) { msgn=7; goto out; }
    if (lisplist==NULL || lisplist->rbnext==NULL) { msgn=22; goto out; }

    lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value */
                                /* (since we're not doing an sds_ret...). */

    /* Steal lisplist for the result: */
    lsp_evres.restype=RTLB;
    lsp_evres.resval.rstring=NULL;
    lsp_evres.rbnext=lisplist->rbnext;
    lisplist->rbnext=NULL;  /* Important */
    lisplist->resval.rstring=NULL;

out:
    if (adslist!=NULL)  { lsp_freesuprb(adslist);  adslist= NULL; }
    if (lisplist!=NULL) { lsp_freesuprb(lisplist); lisplist=NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

// Modified CyberAge AP 09/04/2000[
// Reason: Fix for bug no. 46742.
// (tblnext) should not display unnessary information
void lsp_modify_adslist(struct resbuf **adslist, char *tbname) {
	struct resbuf *tmplist;

	if( (*adslist)->restype == -1)
	{
		struct resbuf *t = *adslist;
		(*adslist) = (*adslist)->rbnext;
		t->rbnext = NULL;
		sds_relrb(t);
	}
	tmplist = *adslist; 		
	if(strcmpi(tbname,"appid") == 0)
	{
		while(tmplist->rbnext != NULL)
		{
			if(tmplist->rbnext->restype == 5 || tmplist->rbnext->restype == 71)
			{
				struct resbuf *t = tmplist->rbnext;
				tmplist->rbnext = tmplist->rbnext->rbnext;
				t->rbnext = NULL;
				sds_relrb(t);
			}
			else
				tmplist = tmplist->rbnext;
		}
		return;
	}
	if(strcmpi(tbname,"block") == 0)
	{
		while(tmplist->rbnext != NULL)
		{
			if(tmplist->rbnext->restype == 5 || tmplist->rbnext->restype == 1 || tmplist->rbnext->restype == 3)
			{
				struct resbuf *t = tmplist->rbnext;
				tmplist->rbnext = tmplist->rbnext->rbnext;
				t->rbnext = NULL;
				sds_relrb(t);
			}
			else
				tmplist = tmplist->rbnext;
		}
		return;
	}
	if(strcmpi(tbname,"dimstyle") == 0)
	{
		while(tmplist->rbnext != NULL)
		{
			if(tmplist->rbnext->restype == 105)
			{
				struct resbuf *t = tmplist->rbnext;
				tmplist->rbnext = tmplist->rbnext->rbnext;
				t->rbnext = NULL;
				sds_relrb(t);
			}
			else
				tmplist = tmplist->rbnext;
		}
		return;
	}
	if(strcmpi(tbname,"layer") == 0 || strcmpi(tbname,"ltype") == 0 || strcmpi(tbname,"view") == 0 || strcmpi(tbname,"ucs") == 0 || strcmpi(tbname,"style") == 0)
	{
		while(tmplist->rbnext != NULL)
		{
			if(tmplist->rbnext->restype == 5)
			{
				struct resbuf *t = tmplist->rbnext;
				tmplist->rbnext = tmplist->rbnext->rbnext;
				t->rbnext = NULL;
				sds_relrb(t);
			}
			else
				tmplist = tmplist->rbnext;
		}
		return;
	}
	if(strcmpi(tbname,"vport") == 0)
	{
		while(tmplist->rbnext != NULL)
		{
			if(tmplist->rbnext->restype == 5 || tmplist->rbnext->restype == 68 || tmplist->rbnext->restype == 69)
			{
				struct resbuf *t = tmplist->rbnext;
				tmplist->rbnext = tmplist->rbnext->rbnext;
				t->rbnext = NULL;
				sds_relrb(t);
			}
			else
				tmplist = tmplist->rbnext;
		}
		return;
	}
}
// Modified CyberAge AP 09/04/2000]
