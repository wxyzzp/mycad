/* C:\ICAD\PRJ\LIB\LISP\STRING.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/

int lsp_ascii(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL)           { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR) { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)   { msgn=9; goto out; }

    lsp_retint((int)((unsigned char)(*lsp_argsll->resval.rstring)));

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_strcat(void) {
    char *tstr;
    int pass,msgn;
	int totlen;
    struct resbuf *tp1;

    msgn=-1; tstr=NULL; totlen=0;

    for (pass=0; pass<2; pass++) {
        if (pass && (tstr= new char [totlen+1])==NULL)
            { msgn=7; goto out; }
		// Modified PK 16/04/2000 [
		// Reason: Fix for bug no. 46434 
		else if (pass) // tstr != NULL
		{ strcpy(tstr, ""); } 
		// Modified 16/04/2000 ]
		totlen=0;
        for (tp1=lsp_argsll; tp1!=NULL; tp1=lsp_nextarg(tp1)) {
            if (tp1->restype!=RTSTR) { msgn=1; goto out; }
            if (pass) strcpy(tstr+totlen,tp1->resval.rstring);
            totlen+=strlen(tp1->resval.rstring);
        }
    }

    if (lsp_retstr(tstr)!=RTNORM) { msgn=7; goto out; }

out:
    if (tstr!=NULL) { IC_FREE(tstr); tstr=NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_strlen(void) {
    int msgn;
    int   totlen;
    struct resbuf *tp1;

    msgn=-1; totlen=0;

    for (tp1=lsp_argsll; tp1!=NULL; tp1=lsp_nextarg(tp1)) {
        if (tp1->restype!=RTSTR) { msgn=1; goto out; }
        totlen+=strlen(tp1->resval.rstring);
    }

    lsp_retint(totlen);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_strcase(void) {
    int msgn;
    struct resbuf *argp[2];

    msgn=-1;

    if ((argp[0]=lsp_argsll)==NULL) { msgn=2; goto out; }
    if (argp[0]->restype!=RTSTR)    { msgn=1; goto out; }
    if ((argp[1]=lsp_nextarg(argp[0]))!=NULL &&
        lsp_nextarg(argp[1])!=NULL) { msgn=9; goto out; }

    if (lsp_retstr(argp[0]->resval.rstring)==RTNORM) {
        if (argp[1]==NULL || argp[1]->restype==RTNIL)
             strupr(lsp_evres.resval.rstring);
        else strlwr(lsp_evres.resval.rstring);
    } else { msgn=7; goto out; }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_substr(void) {
    char *tstr;
    int msgn;
    int idx,start,len,slen;
    struct resbuf *tp1;

    msgn=-1; start=0; len=32000; slen=0; tstr=NULL;

    for (tp1=lsp_argsll,idx=0; tp1!=NULL; tp1=lsp_nextarg(tp1),idx++) {
        if (idx==0) {
            if (tp1->restype!=RTSTR) msgn=1;
            else slen=strlen(tp1->resval.rstring);
        } else if (idx<3) {
            if (tp1->restype!=RTLONG && tp1->restype!=RTSHORT) msgn=1;
            else if (idx==1) {
                start=(tp1->restype==RTLONG) ?
                    (int)tp1->resval.rlong : tp1->resval.rint;
                start--;  /* SUBSTR uses 1-based start pos; we want 0-based */
                if (start<0) msgn=10;
            } else {
                len=(tp1->restype==RTLONG) ?
                    (int)tp1->resval.rlong : tp1->resval.rint;
                if (len<0) msgn=10;
            }
        } else msgn=9;

        if (msgn>-1) goto out;
    }
    if (idx<2) { msgn=2; goto out; }

    if (start>slen) start=slen;
    if (len>slen-start) len=slen-start;
    if ((tstr= new char [len+1])==NULL) { msgn=7; goto out; }
    strncpy(tstr,lsp_argsll->resval.rstring+start,len);
    if (lsp_retstr(tstr)!=RTNORM) { msgn=7; goto out; }

out:
    if (tstr!=NULL) { IC_FREE(tstr); tstr=NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_trim(void) {
    char *be;
    int msgn;
    struct resbuf *argp[2];

    msgn=-1; be="be"/*DNT*/;

    if ((argp[0]=lsp_argsll)==NULL)           { msgn=2; goto out; }
    if (argp[0]->restype!=RTSTR) { msgn=1; goto out; }
    if ((argp[1]=lsp_nextarg(argp[0]))!=NULL) {
        if (argp[1]->restype!=RTSTR) { msgn=1; goto out; }
        be=argp[1]->resval.rstring;
    }
    ic_trim(argp[0]->resval.rstring,be);
    if (lsp_retstr(argp[0]->resval.rstring)!=RTNORM) { msgn=7; goto out; }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_rpad(void) {
    char *tstr;
    int msgn,want,fi1;

    msgn=-1; tstr=NULL;

    if (lsp_argsll==NULL || lsp_argsll->rbnext==NULL) { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR ||
        (lsp_argsll->rbnext->restype!=RTLONG &&
         lsp_argsll->rbnext->restype!=RTSHORT)) { msgn=1; goto out; }
    want=(lsp_argsll->rbnext->restype==RTLONG) ?
        (int)lsp_argsll->rbnext->resval.rlong :
               lsp_argsll->rbnext->resval.rint;
    if (want<0) { msgn=10; goto out; }

    if ((tstr= new char [want+1])==NULL) { msgn=7; goto out; }

    strncpy(tstr,lsp_argsll->resval.rstring,want);

    fi1=0; while (tstr[fi1]) fi1++;
    while (fi1<want) { tstr[fi1]=' '/*DNT*/; fi1++; }
    tstr[want]=0;

    if (lsp_retstr(tstr)!=RTNORM) { msgn=7; goto out; }

out:
    if (tstr!=NULL) { IC_FREE(tstr); tstr=NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_lpad(void) {
    char *tstr;
    int msgn,want,n2add,fi1;

    msgn=-1; tstr=NULL;

    if (lsp_argsll==NULL || lsp_argsll->rbnext==NULL) { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR ||
        (lsp_argsll->rbnext->restype!=RTLONG &&
         lsp_argsll->rbnext->restype!=RTSHORT)) { msgn=1; goto out; }
    want=(lsp_argsll->rbnext->restype==RTLONG) ?
        (int)lsp_argsll->rbnext->resval.rlong :
               lsp_argsll->rbnext->resval.rint;
    if (want<0) { msgn=10; goto out; }

    if ((tstr= new char [want+1])==NULL) { msgn=7; goto out; }

    strncpy(tstr,lsp_argsll->resval.rstring,want);

    fi1=0; while (tstr[fi1]) fi1++;
    if ((n2add=want-fi1)>0) {
        while (fi1>-1) { tstr[fi1+n2add]=tstr[fi1]; fi1--; }
        for (fi1=0; fi1<n2add; fi1++) tstr[fi1]=' '/*DNT*/;
    }
    tstr[want]=0;

    if (lsp_retstr(tstr)!=RTNORM) { msgn=7; goto out; }

out:
    if (tstr!=NULL) { IC_FREE(tstr); tstr=NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_chr(void) {
    char tstr[2];
    int msgn;

    msgn=-1; tstr[1]=0;

    if (lsp_argsll==NULL) { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTLONG && lsp_argsll->restype!=RTSHORT)
        { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL) { msgn=9; goto out; }

    *tstr=(lsp_argsll->restype==RTLONG) ?
        (char)lsp_argsll->resval.rlong : (char)lsp_argsll->resval.rint;

    if (lsp_retstr(tstr)!=RTNORM) { msgn=7; goto out; }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}


