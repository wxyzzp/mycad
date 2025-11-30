/* C:\ICAD\PRJ\LIB\LISP\CONVERT.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/

int lsp_cvunit(void) {
    char *from_p,*to_p;
    int msgn;
    sds_real before,after;
    struct resbuf *tp1;

    msgn=-1; before=after=0.0;

    if ((tp1=lsp_argsll)==NULL)             { msgn=2; goto out; }
    if (tp1->restype==RTREAL || tp1->restype==RTANG || tp1->restype==RTORINT)
        before=tp1->resval.rreal;
    else if (tp1->restype==RTLONG)  before=(sds_real)tp1->resval.rlong;
    else if (tp1->restype==RTSHORT) before=(sds_real)tp1->resval.rint;
    else                                    { msgn=1; goto out; }
    if ((tp1=tp1->rbnext)==NULL)            { msgn=2; goto out; }
    if (tp1->restype!=RTSTR || (from_p=tp1->resval.rstring)==NULL)
                                            { msgn=1; goto out; }
    if ((tp1=tp1->rbnext)==NULL)            { msgn=2; goto out; }
    if (tp1->restype!=RTSTR || (to_p=tp1->resval.rstring)==NULL)
                                            { msgn=1; goto out; }
    if (tp1->rbnext!=NULL)                  { msgn=9; goto out; }

    if (sds_cvunit(before,from_p,to_p,&after)==RTNORM) lsp_retreal(after);
    else lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_float(void) {
    int msgn;
    sds_real ar1;

    msgn=-1;

    if (lsp_argsll==NULL) { msgn=2; goto out; }
    if (lsp_argsll->restype==RTLONG) ar1=(sds_real)lsp_argsll->resval.rlong;
    else if (lsp_argsll->restype==RTSHORT) ar1=(sds_real)lsp_argsll->resval.rint;
    else if (lsp_argsll->restype==RTREAL || lsp_argsll->restype==RTANG ||
        lsp_argsll->restype==RTORINT) ar1=lsp_argsll->resval.rreal;
    else { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL) { msgn=9; goto out; }

    lsp_retreal(ar1);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

void lsp_convlfcr(char *str) {
/*
**  Converts all \n\r pairs in string str to \r.  AutoLISP does
**  this conversion when writing to a file, regardless of mode (text or
**  binary).  (Note that AutoLISP does NOT require the pair to form
**  a line-terminator pair; if it DID, \r\n\r would not be converted
**  (since the \r\n is taken as a line terminator first).  But it IS
**  converted to \r\r.)
*/
    char *sp,*dp;

    sp=dp=str;
    while (*sp) {
        if (*sp=='\n'/*DNT*/ && *(sp+1)=='\r'/*DNT*/) sp++; /*Just skip lf*/
        *dp++=*sp++;
    }
    *dp=0;
}

int lsp_convassoc(struct resbuf *sour, struct resbuf **dest, int ads2lisp)
	{
/*
**  Converts DXF resbuf association llists between the LISP form
**  (a list of dotted pairs and DXF-point/vector-lists) and the ADS form
**  (a llist of resbufs using DXF codes).
**
**  Actually, the definition of an association list can be broader
**  than this, but THIS function is talking about DXF association lists,
**  such as those used by entget and entmake and filter lists for
**  ssget.
**
**  Variable ads2lisp tells which way to translate:
**          0 : LISP to ADS
**      non-0 : ADS to LISP
**
**  THE CALLER MUST FREE THE *dest LIST (IF THAT POINTER IS SET
**  TO NON-NULL, MEANING THAT IT POINTS AT ALLOCATED MEMORY).
**
**  Returns:
**       0 : OK
**      -1 : sour does not point to a llist of the proper form,
**             or dest is NULL.
**      -2 : No memory.
*/
    int rc,dxf,ineed,inapp,fi1;
    struct resbuf *newll,*pnewll,*thismem,*chain,*pchain,*newlink;
    struct resbuf *thisapp,*thisval;

    rc=0; newll=pnewll=chain=pchain=newlink=NULL;

    if (dest==NULL)
		{
		rc=-1;
		goto out;
		}
    else
		*dest=NULL;  /* Init */


    /* If sour is NULL, take NULL; not an error: */
    if (sour==NULL)
		goto out;

    if (ads2lisp)   /* ADS to LISP */
		{
        /* Start with RTLB: */
        if ((newll=db_alloc_resbuf())==NULL)
			{
			rc=-2;
			goto out;
			}
        newll->restype=RTLB;
        newll->resval.rstring=NULL;
        pnewll=newll;

        ineed=inapp=0;

        for (thismem=sour; thismem!=NULL; thismem=thismem->rbnext)
			{

            /* Free prev chain (precaution): */
            if (chain!=NULL)
				{
				lsp_freesuprb(chain);
				chain=pchain=NULL;
				}

            if (thismem->restype==-3)   /* EED sentinel */
				{
                if (ineed)
					{
					/*
                    ** This should NEVER happen.  More than one -3
                    ** sentinel?  That should be an error.  But if it
                    ** ever does, let's TRY to balance the parentheses.
                    ** (I'm not going to test this.) Add enough RTLE's
                    ** to close the current EED app and the EED list:
                    */
                    for (fi1=ineed+inapp; fi1>0; fi1--)
						{
                        if ((chain=db_alloc_resbuf())==NULL)
							{
							rc=-2;
							goto out;
							}
                        chain->restype=RTLE;
                        chain->resval.rstring=NULL;
                        if (newll==NULL)
							newll=chain;
						else
							pnewll->rbnext=chain;
                        pnewll=chain;
						chain=NULL;
						}
					}

                /* Start EED with RTLB and -3: */
                if ((chain=db_alloc_resbuf())==NULL)
					{
					rc=-2;
					goto out;
					}
                chain->restype=RTLB;
                chain->resval.rstring=NULL;
                if ((chain->rbnext=db_alloc_resbuf())==NULL)
                    {
					db_free_resbuf( chain );
					chain=NULL;
					rc=-2;
					goto out;
					}
                chain->rbnext->restype=RTLONG;
                chain->rbnext->resval.rlong=-3L;

                ineed=1;  /* Flag that we've started the EED. */
                inapp=0;  /* Just in case. */

				}
			else if (thismem->restype==1001)   /* EED app name */
				{
                /* DON'T check ineed.  I think you can have 1001's */
                /* in ssget filters without a -3 sentinel. */

                if (inapp)   /* End prev app with RTLE. */
                    {
					if ((chain=db_alloc_resbuf())==NULL)
						{
						rc=-2;
						goto out;
						}
                    chain->restype=RTLE;
                    chain->resval.rstring=NULL;
                    if (newll==NULL)
						newll=chain;
					else
						pnewll->rbnext=chain;
                    pnewll=chain;
					chain=NULL;
					}

                /* Start app with RTLB and the app name: */
                if ((chain=db_alloc_resbuf())==NULL)
					{
					rc=-2;
					goto out;
					}
                chain->restype=RTLB;
                chain->resval.rstring=NULL;
                if ((chain->rbnext=db_alloc_resbuf())==NULL)
					{
					db_free_resbuf(chain);
					chain=NULL;
					rc=-2;
					goto out;
					}
                if ((chain->rbnext->resval.rstring= new char [strlen(thismem->resval.rstring)+1])==NULL)
					{
                    db_free_resbuf(chain->rbnext);
					db_free_resbuf(chain);
					chain=NULL;
                    rc=-2;
					goto out;
					}
                chain->rbnext->restype=RTSTR;
                strcpy(chain->rbnext->resval.rstring,thismem->resval.rstring);

                if (ineed)
					inapp=1;  /* Flag that we're in the EED apps. */

				}
			else
				{  /* Simple type */
				// EBATECH(CNBR) -[ 2002/7/3 Bugzilla#78240 ACAD_PROXY_OBJECT cannot show entity list item310
                //if (thismem->restype<310 || thismem->restype>319)
				//	{
				//	if ((rc=lsp_1associtem(thismem,&chain,ads2lisp))!=0)
				//		goto out;
				//	}
					if ((rc=lsp_1associtem(thismem,&chain,ads2lisp))!=0)
						{
						goto out;
						}
				//EBATECH(CNBR) ]-
				}

            /* Add the chain to newll: */
            if (chain!=NULL)
				{
                if (newll==NULL)
					newll=chain;
				else
					pnewll->rbnext=chain;
                pnewll=chain;
                while (pnewll->rbnext!=NULL)
					pnewll=pnewll->rbnext;
                chain=NULL;
				}

			}  /* End for each source link */

        /* Add enough RTLE's to close the current EED app, the EED list, */
        /* and the assoc list itself: */
        if (chain!=NULL)
			{
			lsp_freesuprb(chain);
			chain=pchain=NULL;
			}
        for (fi1=ineed+inapp+1; fi1>0; fi1--)
			{
            if ((chain=db_alloc_resbuf())==NULL)
				{
				rc=-2;
				goto out;
				}
            chain->restype=RTLE;
            chain->resval.rstring=NULL;
            if (newll==NULL)
				newll=chain;
			else
				pnewll->rbnext=chain;
            pnewll=chain;
			chain=NULL;
			}

		}
	else {  /* LISP to ADS */

        /* If sour RTNIL, take NULL; not an error: */
        if (sour->restype==RTNIL && sour->rbnext==NULL) goto out;

        /* Make sure it's a list: */
        if (sour->restype!=RTLB) { rc=-1; goto out; }

        /* Walk the members: */
        for (thismem=sour->rbnext; thismem!=NULL &&
            thismem->restype!=RTLE; thismem=lsp_nextarg(thismem)) {

            /* Make sure member is a list, too, and that there's more: */
            if (thismem->restype!=RTLB || thismem->rbnext==NULL)
                { rc=-1; goto out; }

            /* Get the DXF code: */
            if (thismem->rbnext->restype==RTSHORT)
                dxf=thismem->rbnext->resval.rint;
            else if (thismem->rbnext->restype==RTLONG)
                dxf=(int)thismem->rbnext->resval.rlong;
            else { rc=-1; goto out; }
            if (dxf<-4 || dxf>1999) { rc=-1; goto out; }

            /* Free prev chain (precaution): */
            if (chain!=NULL) { lsp_freesuprb(chain); chain=pchain=NULL; }

            if (dxf==-3) {  /* EED sentinel; has embedded lists */

                /* Make the -3 link: */
                if ((newlink=db_alloc_resbuf())==NULL) { rc=-2; goto out; }
                newlink->restype=-3;
                if (chain==NULL) chain=newlink;
                else pchain->rbnext=newlink;
                pchain=newlink; newlink=NULL;

                /* Walk the apps: */
                for (thisapp=thismem->rbnext->rbnext; thisapp!=NULL &&
                    thisapp->restype!=RTLE; thisapp=lsp_nextarg(thisapp)) {

                    /* Make sure there's an appname: */
                    if (thisapp->rbnext==NULL ||
                        thisapp->rbnext->restype!=RTSTR ||
                        thisapp->rbnext->resval.rstring==NULL)
                            { rc=-1; goto out; }

                    /* Take appname as a 1001: */
                    if ((newlink=db_alloc_resbuf())==NULL) { rc=-2; goto out; }
                    newlink->restype=1001;
                    if ((newlink->resval.rstring= new char [ strlen(thisapp->rbnext->resval.rstring)+1])==NULL)
                            { rc=-2; goto out; }
                    strcpy(newlink->resval.rstring,
                        thisapp->rbnext->resval.rstring);
                    if (chain==NULL) chain=newlink;
                    else pchain->rbnext=newlink;
                    pchain=newlink; newlink=NULL;

                    /* Take the vals. */
                    for (thisval=thisapp->rbnext->rbnext; thisval!=NULL &&
                        thisval->restype!=RTLE; thisval=lsp_nextarg(thisval)) {

                        /* Shouldn't be any more -3's; use lsp_1associtem(): */
                        if ((rc=lsp_1associtem(thisval,&newlink,ads2lisp))!=0)
                            goto out;

                        if (chain==NULL) chain=newlink;
                        else pchain->rbnext=newlink;
                        pchain=newlink;
                        /* Precaution: */
                        while (pchain->rbnext!=NULL) pchain=pchain->rbnext;
                        newlink=NULL;

                    }

                }

            } else {  /* A simple association list member */

                if ((rc=lsp_1associtem(thismem,&chain,ads2lisp))!=0) goto out;

            }

            /* Add the chain to newll: */
            if (chain!=NULL) {
                if (newll==NULL) newll=chain; else pnewll->rbnext=chain;
                pnewll=chain;
                while (pnewll->rbnext!=NULL) pnewll=pnewll->rbnext;
                chain=NULL;
            }

        }  /* End for each member */

    }  /* End else LISP to ADS */


out:
    if (rc && newll!=NULL) { lsp_freesuprb(newll); newll=pnewll=NULL; }
    if (chain!=NULL) { lsp_freesuprb(chain); chain=pchain=NULL; }
    if (newlink!=NULL) { lsp_freesuprb(newlink); newlink=NULL; }

    if (dest!=NULL) *dest=newll;

    return rc;
}

int lsp_convmat(struct resbuf **rbpp, sds_matrix mat, int ads2lisp) {
/*
**  This function converts between the LISP and ADS forms of a
**  transformation matrix.  The LISP form is a resbuf llist representing
**  a list of 4 lists of 4 numbers (real or int; when we make one, it
**  gets RTREALs).  The ADS form is an sds_matrix (which is a 4x4 2D
**  array of sds_reals).
**
**  ads2lisp!=0 ==> ADS to LISP (CALLER MUST FREE *rbpp IF SUCCESSFUL)
**  ads2lisp==0 ==> LISP to ADS
**
**  Returns:
**       0 : OK
**      -1 : Source matrix is of improper form, or rbpp==NULL, or mat==NULL.
**      -2 : No memory (for ADS-to-LISP)
*/
    int rc,row,col;
    struct resbuf *tp1,*tp2,*res;

    rc=0; res=NULL;

    if (rbpp==NULL || mat==NULL) { rc=-1; goto out; }

    if (ads2lisp) {

        if ((res=db_alloc_resbuf())==NULL)
            { rc=-2; goto out; }
        tp1=res;
        tp1->restype=RTLB;
        tp1->resval.rstring=NULL;
        for (row=0; row<4; row++) {
            if ((tp1->rbnext=db_alloc_resbuf())==NULL) { rc=-2; goto out; }
            tp1=tp1->rbnext;
            tp1->restype=RTLB;
            tp1->resval.rstring=NULL;
            for (col=0; col<4; col++) {
                if ((tp1->rbnext=db_alloc_resbuf())==NULL) { rc=-2; goto out; }
                tp1=tp1->rbnext; tp1->restype=RTREAL;
                tp1->resval.rreal=mat[row][col];
            }
            if ((tp1->rbnext=db_alloc_resbuf())==NULL) { rc=-2; goto out; }
            tp1=tp1->rbnext;
            tp1->restype=RTLE;
            tp1->resval.rstring=NULL;
        }
        if ((tp1->rbnext=db_alloc_resbuf())==
            NULL) { rc=-2; goto out; }
        tp1=tp1->rbnext;
        tp1->restype=RTLE;
        tp1->resval.rstring=NULL;

        *rbpp=res; res=NULL;

    } else {

        tp1=*rbpp;

        /* For nil or (), set mat to identity matrix: */
        if ((tp1->restype==RTNIL && tp1->rbnext==NULL) ||
            (tp1->restype==RTLB &&
             tp1->rbnext!=NULL &&
             tp1->rbnext->restype==RTLE &&
             tp1->rbnext->rbnext==NULL)) {

            for (row=0; row<4; row++)
                for (col=0; col<4; col++)
                    mat[row][col]=(sds_real)(row==col);

        } else {  /* Try to read full matrix llist: */

            if (tp1->restype!=RTLB) { rc=-1; goto out; }
            /* Not lsp_nextarg() here: */
            if ((tp1=tp1->rbnext)==NULL) { rc=-1; goto out; }

            for (row=0; row<4; row++) {

                if (tp1->restype!=RTLB) { rc=-1; goto out; }
                /* Not lsp_nextarg() here: */
                if ((tp2=tp1->rbnext)==NULL) { rc=-1; goto out; }

                for (col=0; col<4; col++) {

                    if (tp2->restype==RTREAL ||
                        tp2->restype==RTANG ||
                        tp2->restype==RTORINT) {

                        mat[row][col]=tp2->resval.rreal;

                    } else if (tp2->restype==RTLONG) {
                        mat[row][col]=(sds_real)tp2->resval.rlong;
                    } else if (tp2->restype==RTSHORT) {
                        mat[row][col]=(sds_real)tp2->resval.rint;
                    } else { rc=-1; goto out; }

                    tp2=tp2->rbnext;  /* Don't need lsp_nextarg() here. */
                    if (tp2==NULL || (col>2 && tp2->restype!=RTLE))
                        { rc=-1; goto out; }
                }

                tp1=lsp_nextarg(tp1);
                if (tp1==NULL || (row>2 && tp1->restype!=RTLE))
                    { rc=-1; goto out; }

            }
        }
    }

out:
    if (res!=NULL) { lsp_freesuprb(res); res=NULL; }
    return rc;
}

int lsp_fix(void) {
    int msgn;
    sds_real ar1,ar2;

    msgn=-1;

    if (lsp_argsll==NULL)         { msgn=2; goto out; }
    if (lsp_argsll->rbnext!=NULL) { msgn=9; goto out; }

    if      (lsp_argsll->restype==RTSHORT)
        ar1=(sds_real)lsp_argsll->resval.rint;
    else if (lsp_argsll->restype==RTLONG)
        ar1=(sds_real)lsp_argsll->resval.rlong;
    else if (lsp_argsll->restype==RTREAL ||
             lsp_argsll->restype==RTANG  ||
             lsp_argsll->restype==RTORINT) ar1=lsp_argsll->resval.rreal;
    else { msgn=1; goto out; }

    modf(ar1,&ar2);

    if (ar2<lsp_rlongmin || ar2>lsp_rlongmax) lsp_retreal(ar2);
    else lsp_retint((int)ar2);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_rtos(void)   { return lsp_valtos(0); }
int lsp_angtos(void) { return lsp_valtos(1); }

int lsp_valtos(int doang) {
    char *tstr;
    int idx,um,prec,msgn,fi1;
    sds_real val;
    struct resbuf *tp1;

    msgn=-1; val=0.0; um=prec=-1; tstr=NULL;

    for (tp1=lsp_argsll,idx=0; tp1!=NULL; tp1=tp1->rbnext,idx++) {
        if (idx==0) {
            if      (tp1->restype==RTSHORT) val=(sds_real)tp1->resval.rint;
            else if (tp1->restype==RTLONG)  val=(sds_real)tp1->resval.rlong;
            else if (tp1->restype==RTREAL || tp1->restype==RTANG ||
                tp1->restype==RTORINT)      val=tp1->resval.rreal;
            else { msgn=1; goto out; }
        } else if (idx<3) {
            fi1=0;
            if      (tp1->restype==RTSHORT) fi1=       tp1->resval.rint;
            else if (tp1->restype==RTLONG)  fi1=(int)tp1->resval.rlong;
            else { msgn=1; goto out; }
            if (fi1<((int)(!doang && idx==1))) { msgn=10; goto out; }
            if (idx==1) um=fi1; else prec=fi1;
        } else { msgn=9; goto out; }
    }
    if (idx<1) { msgn=2; goto out; }

    if (um>(fi1=4+(!doang))) um=fi1;
    if ((tstr= new char [IC_ACADBUF] )==NULL) { msgn=7; goto out; }
	memset(tstr,0, sizeof(char)*IC_ACADBUF);
    if ((!doang &&   sds_rtos(val,um,prec,tstr)!=RTNORM) ||
        ( doang && sds_angtos(val,um,prec,tstr)!=RTNORM))
            { msgn=10; goto out; }
	// This is added to remove the degrees symbol from return value in lisp.
	if(doang && tstr[strlen(tstr)-1]=='\xb0'/*DNT*/)
		tstr[strlen(tstr)-1]=0;
    if (lsp_retstr(tstr)!=RTNORM) { msgn=7; goto out; }

out:
    if (tstr!=NULL) { IC_FREE(tstr); tstr=NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_atoi(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL)           { msgn=2; goto out; }
    if (lsp_argsll->rbnext!=NULL)   { msgn=9; goto out; }
    if (lsp_argsll->restype!=RTSTR) { msgn=1; goto out; }

    lsp_retint(atoi(lsp_argsll->resval.rstring));

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_itoa(void) {
    char fs1[21];
    int msgn;
    long fl1;

    msgn=-1;

    if (lsp_argsll==NULL) { msgn=2; goto out; }
    if (lsp_argsll->restype==RTLONG)       fl1=lsp_argsll->resval.rlong;
    else if (lsp_argsll->restype==RTSHORT) fl1=(long)lsp_argsll->resval.rint;
    else { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL) { msgn=9; goto out; }

    sprintf(fs1,"%ld"/*DNT*/,fl1);

    if (lsp_retstr(fs1)!=RTNORM) { msgn=7; goto out; }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_atof(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL)           { msgn=2; goto out; }
    if (lsp_argsll->rbnext!=NULL)   { msgn=9; goto out; }
    if (lsp_argsll->restype!=RTSTR) { msgn=1; goto out; }

    lsp_retreal(atof(lsp_argsll->resval.rstring));

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_angtof(void) { return lsp_tof(0); }
int lsp_distof(void) { return lsp_tof(1); }

int lsp_tof(int which) {
    int msgn,mode;
    sds_real res;

    msgn=-1; mode=-1; res=0.0;

    if (lsp_argsll==NULL) { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR ||
        lsp_argsll->resval.rstring==NULL) { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL) {
        if      (lsp_argsll->rbnext->restype==RTLONG)
            mode=(int)lsp_argsll->rbnext->resval.rlong;
        else if (lsp_argsll->rbnext->restype==RTSHORT)
            mode=lsp_argsll->rbnext->resval.rint;
        else { msgn=1; goto out; }
        if (mode<0+which || mode>4+which) { msgn=1; goto out; }
        if (lsp_argsll->rbnext->rbnext!=NULL) { msgn=9; goto out; }
    }

    if (which) {
        if (sds_distof(lsp_argsll->resval.rstring,mode,&res)==RTNORM)
            lsp_retreal(res);
        else lsp_retnil();
    } else {
        if (sds_angtof(lsp_argsll->resval.rstring,mode,&res)==RTNORM)
            lsp_retreal(res);
        else lsp_retnil();
    }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

