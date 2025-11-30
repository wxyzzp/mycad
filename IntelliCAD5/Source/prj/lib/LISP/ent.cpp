/* C:\ICAD\PRJ\LIB\LISP\ENT.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/

int lsp_entdel(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL)             { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTENAME) { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)     { msgn=9; goto out; }

    if (sds_entdel(lsp_argsll->resval.rlname)!=RTNORM) lsp_retnil();
    else if (lsp_retval(lsp_argsll)!=RTNORM) msgn=7;

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_entupd(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL)             { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTENAME) { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)     { msgn=9; goto out; }

    if (sds_entupd(lsp_argsll->resval.rlname)!=RTNORM) lsp_retnil();
    else if (lsp_retval(lsp_argsll)!=RTNORM) msgn=7;

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_entlast(void) {
    int msgn;
    struct resbuf retrb;

    msgn=-1; retrb.restype=RTENAME; retrb.rbnext=NULL;

    if (lsp_argsll!=NULL) { msgn=9; goto out; }

    if (sds_entlast(retrb.resval.rlname)!=RTNORM) retrb.restype=RTNIL;
    if (lsp_retval(&retrb)!=RTNORM) msgn=7;

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_entnext(void) {
    int msgn;
    struct resbuf retrb;

    msgn=-1; retrb.restype=RTENAME; retrb.rbnext=NULL;

    if (lsp_argsll!=NULL) {
        if (lsp_argsll->restype!=RTENAME) { msgn=1; goto out; }
        if (lsp_argsll->rbnext!=NULL)     { msgn=9; goto out; }
    }

    if (sds_entnext_noxref((lsp_argsll==NULL) ? NULL : lsp_argsll->resval.rlname,
        retrb.resval.rlname)!=RTNORM) retrb.restype=RTNIL;
    if (lsp_retval(&retrb)!=RTNORM) msgn=7;

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_entget(void) 
	{
    int msgn;
    struct resbuf *apll,*newlink,*elist,*assocll,*tp1;

    msgn=-1;
	apll=newlink=elist=assocll=NULL;

    if (lsp_argsll==NULL)
		{
		msgn=2;
		goto out; 
		}
    if (lsp_argsll->restype!=RTENAME)
		{
		msgn=1; 
		goto out; 
		}
    if (lsp_argsll->rbnext!=NULL) 
		{
        if (lsp_argsll->rbnext->restype!=RTLB &&
            lsp_argsll->rbnext->restype!=RTNIL)
			{
			msgn=1;
			goto out; 
			}
        if (lsp_nextarg(lsp_argsll->rbnext)!=NULL)
			{
			msgn=9; 
			goto out; 
			}

        /* Make the apps llist: */
        if (lsp_argsll->rbnext->restype==RTLB) 
			{
            for (tp1=lsp_argsll->rbnext->rbnext; tp1!=NULL &&
                tp1->restype!=RTLE; tp1=lsp_nextarg(tp1)) 
				{

                if (tp1->restype!=RTSTR) 
					{
					msgn=33; 
					goto out; 
					}

                if ((newlink=db_alloc_resbuf())==NULL || 
					(newlink->resval.rstring= new char [strlen(tp1->resval.rstring)+1])==NULL) 
					{
					msgn=7; 
					goto out; 
					}
                newlink->restype=RTSTR;
                strcpy(newlink->resval.rstring,tp1->resval.rstring);

                newlink->rbnext=apll;
				apll=newlink;
                newlink=NULL;
				}
			}
		}

    elist=sds_entgetx(lsp_argsll->resval.rlname,apll);

    /* Convert to a LISP association list: */
    if (lsp_convassoc(elist,&assocll,1)==-2) 
		{
		msgn=7; 
		goto out; 
		}

    /* If it's NULL, set result to nil: */
    if (assocll==NULL || assocll->restype!=RTLB) 
		{
		lsp_retnil(); 
		goto out; 
		}

    lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value */
                                 /* (since we're not doing an sds_ret...). */

    /* Steal the result from assocll: */
    lsp_evres.restype=RTLB;
    lsp_evres.resval.rstring=NULL;
    lsp_evres.rbnext=assocll->rbnext;
    assocll->rbnext=NULL;  /* Now assocll is just the RTLB link */
                           /* (soon to be freed). */
    assocll->resval.rstring=NULL;

out:
    if (apll!=NULL)    { lsp_freesuprb(apll);    apll=   NULL; }
    if (newlink!=NULL) { lsp_freesuprb(newlink); newlink=NULL; }
    if (elist!=NULL)   { lsp_freesuprb(elist);   elist=  NULL; }
    if (assocll!=NULL) { lsp_freesuprb(assocll); assocll=NULL; }

    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_entmake(void)  { return lsp_entmaker(0); }
int lsp_entmakex(void) { return lsp_entmaker(1); }

int lsp_entmaker(int mode) {  /* 0 : entmake;  1 : entmakex */
    char *afs1;
    int msgn,fi1;
    sds_name ename;
    struct resbuf *elist;

    msgn=-1; elist=NULL; afs1=NULL; ename[0]=ename[1]=0;

    /* If no args, cancel complex entity: */
    if (lsp_argsll==NULL) { sds_entmake(NULL); lsp_retnil(); goto out; }
    if (lsp_argsll->restype!=RTLB && lsp_argsll->restype!=RTNIL)
        { msgn=1; goto out; }
    if (lsp_nextarg(lsp_argsll)!=NULL) { msgn=9; goto out; }
    if (lsp_argsll->restype==RTNIL)  /* Same as no args. */
        { sds_entmake(NULL); lsp_retnil(); goto out; }

    if ((fi1=lsp_convassoc(lsp_argsll,&elist,0))==-2) { msgn=7; goto out; }
    if (fi1) { msgn=34; goto out; }

    if ((fi1=(mode) ? sds_entmakex(elist,ename) : sds_entmake(elist))==RTNORM) {
        if (mode) {
            lsp_retname(ename,RTENAME);
        } else {
            /* Steal lsp_argsll to set lsp_evres: */
            lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value. */
            /* Set 1st link to RTLB and link chain onto it: */
            lsp_evres.restype=RTLB; lsp_evres.rbnext=lsp_argsll->rbnext;
            lsp_evres.resval.rstring=NULL;
            lsp_argsll->rbnext=NULL;  /* Important! */
            lsp_argsll->resval.rstring=NULL;
        }
    } else if (fi1==RTKWORD) {
        if ((afs1= new char [ 512 ])==NULL) { msgn=7; goto out; }
        if (sds_getinput(afs1)==RTNORM) {
            if (lsp_retstr(afs1)!=RTNORM) { msgn=7; goto out; }
        } else lsp_retnil();  /* Should never */
    } else {  /* Must be RTREJ */
        lsp_retnil();
    }

out:
    if (elist!=NULL) { lsp_freesuprb(elist); elist=NULL; }
    if (afs1!=NULL) { IC_FREE(afs1); afs1=NULL; }

    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_entmod(void) {
    int msgn,fi1;
    struct resbuf *elist;

    msgn=-1; elist=NULL;

    if (lsp_argsll==NULL) { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTLB && lsp_argsll->restype!=RTNIL)
        { msgn=1; goto out; }
    if (lsp_nextarg(lsp_argsll)!=NULL) { msgn=9; goto out; }
    if (lsp_argsll->restype==RTNIL) { lsp_retnil(); goto out; }

    if ((fi1=lsp_convassoc(lsp_argsll,&elist,0))==-2) { msgn=7; goto out; }
    if (fi1) { msgn=35; goto out; }

    if ((fi1=sds_entmod(elist))==RTNORM) {
        /* Steal lsp_argsll to set lsp_evres: */
        lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value. */
        /* Set 1st link to RTLB and link chain onto it: */
        lsp_evres.restype=RTLB; lsp_evres.rbnext=lsp_argsll->rbnext;
        lsp_evres.resval.rstring=NULL;
        lsp_argsll->rbnext=NULL;  /* Important! */
        lsp_argsll->resval.rstring=NULL;
    } else lsp_retnil();

out:
    if (elist!=NULL) { lsp_freesuprb(elist); elist=NULL; }

    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}


int lsp_handent(void) {
    int msgn;
    struct resbuf retrb;

    msgn=-1;
    retrb.restype=RTENAME; retrb.rbnext=NULL;

    if (lsp_argsll==NULL)           { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR) { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)   { msgn=9; goto out; }

    if (sds_handent(lsp_argsll->resval.rstring,retrb.resval.rlname)==RTNORM) {
        if (lsp_retval(&retrb)!=RTNORM) { msgn=7; goto out; }
    } else lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_nentsel(void)  { return lsp_nentselx(0); }
int lsp_nentselp(void) { return lsp_nentselx(1); }

int lsp_nentselx(int mode) {
    char *pmpt;
    int msgn,usept,adsret,fi1,fi2;
    sds_name ename;
    sds_point pt,matpt[4];
    sds_matrix mat;
    struct resbuf *blkll,*res,*lispmat,*last,*tp1;

    msgn=-1; pmpt=NULL; blkll=res=lispmat=NULL; usept=adsret=0;
    ename[0]=ename[1]=0L; pt[0]=pt[1]=pt[2]=0.0;
    if (mode) {  /* Init the 4x4 matrix to identity. */
        for (fi1=0; fi1<4; fi1++)
            for (fi2=0; fi2<4; fi2++)
                mat[fi1][fi2]=(sds_real)(fi1==fi2);
    } else {  /* Init the 4x3 matrix to identity. */
        for (fi1=0; fi1<4; fi1++)
            for (fi2=0; fi2<3; fi2++)
                matpt[fi1][fi2]=(sds_real)(fi1==fi2);
    }

    /* Convert pts from list-form to pt-form. */
    lsp_convptlist(lsp_argsll,0);

    if ((tp1=lsp_argsll)!=NULL) {
        if (mode) {  /* nentselp is picky about its arg messages. */
            if (tp1->restype==RTSTR) {
                pmpt=tp1->resval.rstring;
                if ((tp1=tp1->rbnext)!=NULL) {
                    if (tp1->restype==RT3DPOINT || tp1->restype==RTPOINT) {
                        ic_ptcpy(pt,tp1->resval.rpoint); usept=1;
                    } else { msgn=1; goto out; }
                    if (tp1->rbnext!=NULL) { msgn=9; goto out; }
                }
            } else if (tp1->restype==RT3DPOINT || tp1->restype==RTPOINT) {
                ic_ptcpy(pt,tp1->resval.rpoint); usept=1;
                if (tp1->rbnext!=NULL) { msgn=1; goto out; }
            } else { msgn=1; goto out; }
        } else {
            if (tp1->restype!=RTSTR) { msgn=1; goto out; }
            if (tp1->rbnext!=NULL)   { msgn=9; goto out; }
            pmpt=tp1->resval.rstring;
        }
    }

    lsp_freesplrb(&lsp_evres);  /* Default result is nil. */

    /* OK.  NOW we can do the sds_initget() (since we're ready to */
    /* call the ads function). */
    if (lsp_iget.valid) sds_initget(lsp_iget.bits,lsp_iget.kwl);

    adsret=(mode) ?
        sds_nentselp(pmpt,ename,pt,usept,mat,&blkll) :
        sds_nentsel(pmpt,ename,pt,matpt,&blkll);

    if (adsret==RTCAN) {
        msgn=18; goto out;
    } else if (adsret==RTKWORD) {
        if ((lsp_evres.resval.rstring= new char [ 512 ])==NULL)
            { msgn=7; goto out; }
        lsp_evres.restype=RTSTR;
        if (sds_getinput(lsp_evres.resval.rstring)!=RTNORM) {
            IC_FREE(lsp_evres.resval.rstring); lsp_evres.resval.rstring=NULL;
            lsp_evres.restype=RTNIL;
        }
        goto out;
    } else if (adsret!=RTNORM) {
        goto out;  /* Missed?  Take the nil. */
    }

    /* Got a non-keyword result if made it to here. */


    /* BUILD A LLIST OF THE RESULT ITEMS -- EXCEPT FOR THE BEGINNING RTLB. */

    /* The ename: */
    if ((res=db_alloc_resbuf())==NULL)
        { msgn=7; goto out; }
    last=res;
    last->restype=RTENAME;
    ic_encpy(last->resval.rlname,ename);

    /* The point: */
    if ((last->rbnext=db_alloc_resbuf())==
        NULL) { msgn=7; goto out; }
    last=last->rbnext;
    last->restype=RT3DPOINT;
    ic_ptcpy(last->resval.rpoint,pt);

    if (blkll!=NULL) {  /* Must be a nested entity. */

        if (mode) {  /* The 4x4 matrix. */

            /* Convert mat to list-form: */
            if ((fi1=lsp_convmat(&lispmat,mat,1))==-1)
                { msgn=21; goto out; }  /* Should never */
            else if (fi1) { msgn=7; goto out; }
            last->rbnext=lispmat;
            lispmat=NULL;  /* Important */
            while (last->rbnext!=NULL) last=last->rbnext;  /* Find new end */

        } else {

            /* The 4x3 matrix (just make a list of the pts and convert */
            /* them to list-form later): */

            if ((last->rbnext=db_alloc_resbuf())==
                NULL) { msgn=7; goto out; }
            last=last->rbnext;
            last->restype=RTLB;
            last->resval.rstring=NULL;
            for (fi1=0; fi1<4; fi1++) {
                if ((last->rbnext=db_alloc_resbuf())==NULL) { msgn=7; goto out; }
                last=last->rbnext;
                last->restype=RT3DPOINT;
                ic_ptcpy(last->resval.rpoint,matpt[fi1]);
            }
            if ((last->rbnext=db_alloc_resbuf())==
                NULL) { msgn=7; goto out; }
            last=last->rbnext;
            last->restype=RTLE;
            last->resval.rstring=NULL;
        }

        /* The list of block def enames: */
        if ((last->rbnext=db_alloc_resbuf())==
            NULL) { msgn=7; goto out; }
        last=last->rbnext;
        last->restype=RTLB;
        last->resval.rstring=NULL;
        last->rbnext=blkll;  /* Steal blkll */
        blkll=NULL;  /* Important */
        /* Find the end again -- and convert the -1's to RTENAMEs */
        /* as we go; for some reason, ADS retrieves them as -1's: */
        while (last->rbnext!=NULL) {
            last=last->rbnext;
            if (last->restype==-1)
                last->restype=RTENAME;  /* Yep -- AFTER stepping. */
        }
        if ((last->rbnext=db_alloc_resbuf())==
            NULL) { msgn=7; goto out; }
        last=last->rbnext;
        last->restype=RTLE;
        last->resval.rstring=NULL;

    }

    /* The final RTLE: */
    if ((last->rbnext=db_alloc_resbuf())==
        NULL) { msgn=7; goto out; }
    last=last->rbnext;
    last->restype=RTLE;
    last->resval.rstring=NULL;

    /* Convert the pt and the 4x3 matrix pts to list form: */
    if (lsp_convptlist(res,1)) { msgn=7; goto out; }


    /* ADD IT TO lsp_evres (lsp_retnil() freed the old lsp_evres earlier): */

    lsp_evres.restype=RTLB; lsp_evres.rbnext=res;
    lsp_evres.resval.rstring=NULL;
    res=NULL;  /* Important */

out:
    if (blkll!=NULL)   { lsp_freesuprb(blkll);   blkll=  NULL; }
    if (res!=NULL)     { lsp_freesuprb(res);     res=    NULL; }
    if (lispmat!=NULL) { lsp_freesuprb(lispmat); lispmat=NULL; }
    lsp_freeiget();  /* Clear the initget settings. */
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

