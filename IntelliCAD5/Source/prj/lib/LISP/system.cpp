/* C:\ICAD\PRJ\LIB\LISP\SYSTEM.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/
#include "sds_engine.h"


int lsp_getcname(void) {
    int msgn;
    char *cp1=NULL;

    msgn=-1;

    if (lsp_argsll==NULL)           { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR || lsp_argsll->resval.rstring==NULL)
                                    { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)   { msgn=9; goto out; }

    if (sds_getcname(lsp_argsll->resval.rstring,&cp1)!=RTNORM) {
		lsp_retnil();
		goto out;
	}

    if (lsp_retstr(cp1)!=RTNORM) { msgn=7; goto out; }

out:
	// Modified Cybage PP 22/11/2000 [
	// Reason: Fix for bug no. 47103
	//if(cp1) free(cp1);
	if(cp1) delete cp1 ;
	// Modified cybage PP 22/11/2000 ]

    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_getcfg(void) {
    int msgn;
    char fs1[IC_ACADBUF];

    msgn=-1;

    if (lsp_argsll==NULL)           { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR || lsp_argsll->resval.rstring==NULL)
                                    { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)   { msgn=9; goto out; }

    if (sds_agetcfg(lsp_argsll->resval.rstring,fs1)!=RTNORM) {
		lsp_retnil();
		goto out;
	}

    if (lsp_retstr(fs1)!=RTNORM) { msgn=7; goto out; }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_setvar(void) {
    int msgn;
    long fl1;
    struct resbuf rb,*argp[2];

    msgn=-1; rb.rbnext=NULL; rb.restype=RTNIL;

    if ((argp[0]=lsp_argsll)==NULL)      { msgn=2; goto out; }
    if (argp[0]->restype!=RTSTR || argp[0]->resval.rstring==NULL)
                                         { msgn=1; goto out; }
    if ((argp[1]=argp[0]->rbnext)==NULL) { msgn=2; goto out; }
    if (lsp_nextarg(argp[1])!=NULL)      { msgn=9; goto out; }

	// Modified Cybage PP 09/02/2001 [
	// Reason: Fix for bug no. : 59500
	if( strisame( argp[0]->resval.rstring, "LASTCMDANG" ) ||
		strisame( argp[0]->resval.rstring, "_LASTCMDANG" ) )
	{
		msgn = 41 ;
		goto out ;
	}
	// Modified Cybage PP 09/02/2001 ]

    switch (argp[1]->restype) {
        case RTSTR:
            if (argp[1]->resval.rstring==NULL) { msgn=41; goto out; }
            rb.restype=RTSTR;
            if ((rb.resval.rstring= new char [ strlen(argp[1]->resval.rstring)+1] )==NULL) { msgn=7; goto out; }
            strcpy(rb.resval.rstring,argp[1]->resval.rstring);
            break;
        case RTREAL: case RTANG: case RTORINT:
            rb.restype=argp[1]->restype;
            rb.resval.rreal=argp[1]->resval.rreal;
            break;
        case RTLONG: case RTSHORT:
            fl1=(argp[1]->restype==RTLONG) ?
                argp[1]->resval.rlong : ((long)argp[1]->resval.rint);
            if (fl1<SHRT_MIN || fl1>SHRT_MAX)
                 { rb.restype=RTREAL;  rb.resval.rreal=(sds_real)fl1; }
            else { rb.restype=RTSHORT; rb.resval.rint=(int)fl1;     }
            break;
        case RTLB:  /* Possible pt */
            lsp_convptlist(argp[1],0);
            if (argp[1]->restype!=RT3DPOINT && argp[1]->restype!=RTPOINT)
                { msgn=41; goto out; }
            rb.restype=argp[1]->restype;
            ic_ptcpy(rb.resval.rpoint,argp[1]->resval.rpoint);
            break;
        default: msgn=41; goto out;
    }

	// *******************************************
	// true in last parameter makes this check for READONLY sysvars
	//
    if (sdsengine_setvar(argp[0]->resval.rstring,&rb, true )!=RTNORM) 
		{
		msgn=41;
		}
	else {
		//Modified SAU 31/05/2000			[
		//Reason:Setting the value of MODEMACRO with setvar does not work
		if(strisame(argp[0]->resval.rstring,"MODEMACRO"))
			sds_grtext(-1,((rb).resval).rstring,0);
		//Modified SAU 31/05/2000			]
		if (rb.restype==RTSHORT) {  /* Change to long to keep consistent. */
            fl1=(long)rb.resval.rint;  /* Don't try intra-union assignment. */
            rb.restype=RTLONG;
            rb.resval.rlong=fl1;
        }
        if (lsp_retval(&rb)!=RTNORM) { msgn=7; goto out; }
    }

out:
    if (rb.restype==RTSTR) {
        if (rb.resval.rstring!=NULL) IC_FREE(rb.resval.rstring);
        rb.resval.rstring=NULL; rb.restype=RTNIL;
    }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_setcfg(void) {
    int msgn;
    struct resbuf rb,*argp[2];

    msgn=-1; rb.rbnext=NULL; rb.restype=RTNIL;

    if ((argp[0]=lsp_argsll)==NULL)      { msgn=2; goto out; }
    if (argp[0]->restype!=RTSTR || argp[0]->resval.rstring==NULL)
                                         { msgn=1; goto out; }
    if ((argp[1]=argp[0]->rbnext)==NULL) { msgn=2; goto out; }
    if (argp[1]->restype!=RTSTR || argp[1]->resval.rstring==NULL)
                                         { msgn=1; goto out; }
    if (lsp_nextarg(argp[1])!=NULL)      { msgn=9; goto out; }

    if (sds_asetcfg(argp[0]->resval.rstring,argp[1]->resval.rstring)!=RTNORM) {
		lsp_retnil();
		goto out;
	}

    if (lsp_retstr(argp[1]->resval.rstring)!=RTNORM) { msgn=7; goto out; }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_getvar(void) {
    int msgn;
    long fl1;
    struct resbuf rb;

    msgn=-1; rb.rbnext=NULL; rb.restype=RTNIL;

    if (lsp_argsll==NULL)           { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR || lsp_argsll->resval.rstring==NULL)
                                    { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)   { msgn=9; goto out; }

	// Modified Cybage PP 09/02/2001 [
	// Reason: Fix for bug no. : 59500
	if( strisame( lsp_argsll->resval.rstring, "LASTCMDANG" ) ||
		strisame( lsp_argsll->resval.rstring, "_LASTCMDANG" ) )
	{
		lsp_retnil() ;
		goto out ;
	}
	// Modified Cybage PP 09/02/2001 ]

    if (sds_getvar(lsp_argsll->resval.rstring,&rb)!=RTNORM) lsp_retnil();
    else {
        if (rb.restype==RTSHORT) {  /* Change to long to keep consistent. */
            fl1=(long)rb.resval.rint;  /* Don't try intra-union assignment. */
            rb.restype=RTLONG;
            rb.resval.rlong=fl1;
        }
        if (lsp_retval(&rb)!=RTNORM) { msgn=7; goto out; }
    }

out:
    if (rb.restype==RTSTR) {        
		if (rb.resval.rstring!=NULL) IC_FREE(rb.resval.rstring);
        rb.resval.rstring=NULL; rb.restype=RTNIL;
    }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_flipper(int op) {
    int msgn;

    msgn=-1;
    if (lsp_argsll!=NULL) { msgn=9; goto out; }

    if      (op==0) sds_graphscr();
    else if (op==1) sds_textscr();
    else if (op==2) sds_textpage();

    lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_graphscr(void) { return lsp_flipper(0); }
int lsp_textscr(void)  { return lsp_flipper(1); }
int lsp_textpage(void) { return lsp_flipper(2); }



int lsp_getenv(void) {
    char fs1[41],*fcp1;
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL)           { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR) { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)   { msgn=9; goto out; }

    strncpy(fs1,lsp_argsll->resval.rstring,sizeof(fs1)-1);
    #if !defined(UNIX)  /* AutoLISP says case-sensitive for UNIX */
        strupr(fs1);
    #endif

	if(strisame("ACAD"/*DNT*/,fs1) || strisame("ICAD"/*DNT*/,fs1)) {
		struct resbuf rb;
		if(RTNORM!=sds_getvar("SRCHPATH"/*DNT*/,&rb)) {
			lsp_retnil();
		} else {
			lsp_retstr(rb.resval.rstring);
			IC_FREE(rb.resval.rstring);
		}
	} else {
		if ((fcp1=getenv(fs1))==NULL) lsp_retnil();
		else if (lsp_retstr(fcp1)!=RTNORM) { msgn=7; goto out; }
	}

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_setenv(void) {
    char *afs1,*fcp1,*fcp2;
    int msgn;

    msgn=-1; afs1=NULL;

    if (lsp_argsll==NULL)                         { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR ||
        lsp_argsll->resval.rstring==NULL)         { msgn=1; goto out; }
    if (lsp_argsll->rbnext==NULL)                 { msgn=2; goto out; }
    if (lsp_argsll->rbnext->restype!=RTSTR ||
        lsp_argsll->rbnext->resval.rstring==NULL) { msgn=1; goto out; }
    if (lsp_argsll->rbnext->rbnext!=NULL)         { msgn=9; goto out; }

    /* Alloc afs1 large enough for var=val: */
    if ((afs1= new char [strlen(lsp_argsll->resval.rstring)+
        strlen(lsp_argsll->rbnext->resval.rstring)+2])==NULL)
            { msgn=7; goto out; }
    /* Set afs1 to var: */
    fcp1=afs1; fcp2=lsp_argsll->resval.rstring;
    while (*fcp2) { *fcp1=*fcp2; fcp1++; fcp2++; }
    *fcp1=0;
    #if !defined(UNIX)  /* AutoLISP says case-sensitive for UNIX */
        strupr(afs1);
    #endif

    if (strisame("ACAD"/*DNT*/,afs1) || strisame("ICAD"/*DNT*/,afs1)) {
        if (RTNORM!=sds_setvar("SRCHPATH"/*DNT*/,lsp_argsll->rbnext)) lsp_retnil();
	} else {
        /* Complete var=val: */
        *fcp1='='/*DNT*/; fcp1++; fcp2=lsp_argsll->rbnext->resval.rstring;
        while (*fcp2) { *fcp1=*fcp2; fcp1++; fcp2++; }
        *fcp1=0;
        if (_putenv(afs1)) lsp_retnil();
        else if (lsp_retstr(lsp_argsll->rbnext->resval.rstring)!=RTNORM)
            { msgn=7; goto out; }
	}

out:
    if (afs1!=NULL) { IC_FREE(afs1); afs1=NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}




