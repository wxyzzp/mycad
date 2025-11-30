/* C:\ICAD\PRJ\LIB\LISP\GETTERS.CPP
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
#include "IcadApi.h"
#include "IcadDoc.h"

// Modified PK 12/07/2000 
extern int SDS_CalledFromgetfiled; // Reason : Fix for bug no. 47174

int lsp_getter(int mode) {
/*
**  Handles all the get... functions that allow keyword entry
**  (except nentsel and nentselp).
*/
    const char *pmpt;
	std::string sPrompt;
    int adsret,argidx,msgn,fi1;
    int fint1;
    sds_real ar1,*arp1;
    sds_point ap1;
    sds_name ename;
    struct resbuf retrb,*tp1,lastpoint,lastangle;

	SDS_getvar(NULL,DB_QLASTPOINT,&lastpoint,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SDS_getvar(NULL,DB_QLASTANGLE,&lastangle,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

    msgn=-1; pmpt=NULL; retrb.restype=RTNIL; retrb.rbnext=NULL;
    adsret=RTNONE; arp1=NULL;

    /* Check args and get what we need: */
    switch (mode) {

        case 0: case 1: case 2: case 4: /* int/real/kword/entsel */

            if (lsp_argsll!=NULL) {
                if (lsp_argsll->restype!=RTSTR) { msgn=1; goto out; }
                if (lsp_argsll->rbnext!=NULL)   { msgn=9; goto out; }
                pmpt=lsp_argsll->resval.rstring;
            }
            break;

        case 3: case 5: case 6: case 7: case 8:
            /* point/corner/angle/orient/dist */

            /* Convert any point-lists to point form: */
            lsp_convptlist(lsp_argsll,0);

            /* The argument logic is the same as for lsp_initget(); */
            /* see that function for a description of the logic. */

            for (tp1=lsp_argsll,argidx=0; tp1!=NULL; tp1=tp1->rbnext,
                argidx++) {

                if (argidx>1) { msgn=1; goto out; }
                if (tp1->restype==RTPOINT || tp1->restype==RT3DPOINT) {
                    if (arp1!=NULL) { msgn=1; goto out; }
                    arp1=tp1->resval.rpoint;
                } else if (tp1->restype==RTSTR) {
                    if (pmpt!=NULL) { msgn=1; goto out; }
                    pmpt=tp1->resval.rstring;
                } else { msgn=1; goto out; }
            }

            if (mode==5 && arp1==NULL) { msgn=40; goto out; }

            break;

    }

	//<alm: copy prompt to local variable, to protect from freeing of lsp_argsll>
	if (pmpt)
	{
		sPrompt.assign(pmpt);
		pmpt = sPrompt.c_str();
	}
	//</alm>

    /* OK.  NOW we can do the sds_initget() (since we're ready to */
    /* call the sds_getxxx() functions next). */
    if (lsp_iget.valid) sds_initget(lsp_iget.bits,lsp_iget.kwl);

    /* Get what we need from the appropriate ads function: */
    switch (mode) {
        case 0:  /* int  */
            if ((adsret=sds_getint(pmpt,&fint1))==RTNORM)
                { retrb.restype=RTLONG; retrb.resval.rlong=(long)fint1; }
            break;
        case 1:  /* real */
            if ((adsret=sds_getreal(pmpt,&ar1))==RTNORM)
                { retrb.restype=RTREAL; retrb.resval.rreal=ar1; }
            break;
        case 3:  /* point */
            if ((adsret=sds_getpoint(arp1,pmpt,ap1))==RTNORM)
                { retrb.restype=RT3DPOINT; ic_ptcpy(retrb.resval.rpoint,ap1); }
            break;
        case 4:  /* entsel */
            if ((adsret=sds_entsel(pmpt,ename,ap1))==RTNORM) {
                /* Make a list of the ename and pt: */
                tp1=&retrb;
                tp1->restype=RTLB;
                tp1->resval.rstring=NULL;  /* (See lsp_argend().) */
                for (fi1=-2; fi1<5; fi1++) {
                    if ((tp1->rbnext=db_alloc_resbuf())==NULL) { msgn=7; goto out; }
                    tp1=tp1->rbnext;
                    if (fi1==-2) {
                        tp1->restype=RTENAME;
                        ic_encpy(tp1->resval.rlname,ename);
                    } else if (fi1==-1) {
                        tp1->restype=RTLB;
                        tp1->resval.rstring=NULL;
                    } else if (fi1<3) {
                        tp1->restype=RTREAL;
                        tp1->resval.rreal=ap1[fi1];
                    } else {
                        tp1->restype=RTLE;
                        tp1->resval.rstring=NULL;
                    }
                }
            }
            break;
        case 5:  /* corner */
            if ((adsret=sds_getcorner(arp1,pmpt,ap1))==RTNORM)
                { retrb.restype=RT3DPOINT; ic_ptcpy(retrb.resval.rpoint,ap1); }
            break;
        case 6:  /* angle */
            if ((adsret=sds_getangle(arp1,pmpt,&ar1))==RTNORM)
                { retrb.restype=RTANG; retrb.resval.rreal=ar1; }
            break;
        case 7:  /* orient */
            if ((adsret=sds_getorient(arp1,pmpt,&ar1))==RTNORM)
                { retrb.restype=RTORINT; retrb.resval.rreal=ar1; }
            break;
        case 8:  /* dist */
            if ((adsret=sds_getdist(arp1,pmpt,&ar1))==RTNORM)
                { retrb.restype=RTREAL; retrb.resval.rreal=ar1; }
            break;
    }

    if      (adsret==RTCAN)   msgn=18;
    /* else if (adsret==RTERROR) msgn=19; Don't think we need; take nil. */
    if (msgn>-1) goto out;

    if (adsret==RTKWORD || mode==2) {
        retrb.restype=RTSTR;
        if ((retrb.resval.rstring= new char [ 512])==NULL)
            { msgn=7; goto out; }
        if ((mode==2 && sds_getkword(pmpt,retrb.resval.rstring)!=RTNORM) ||
            (mode!=2 && sds_getinput(retrb.resval.rstring)!=RTNORM)) {

            IC_FREE(retrb.resval.rstring); retrb.resval.rstring=NULL;
            retrb.restype=RTNIL;
        }
    }

    if (lsp_retval(&retrb)!=RTNORM) { msgn=7; goto out; }

out:
	SDS_setvar(NULL,DB_QLASTPOINT,&lastpoint,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	SDS_setvar(NULL,DB_QLASTANGLE,&lastangle,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

    lsp_freesplrb(&retrb);
    lsp_freeiget();  /* Clear the initget settings. */
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_getint(void)    { return lsp_getter(0); }
int lsp_getreal(void)   { return lsp_getter(1); }
int lsp_getkword(void)  { return lsp_getter(2); }
int lsp_getpoint(void)  { return lsp_getter(3); }
int lsp_entsel(void)    { return lsp_getter(4); }
int lsp_getcorner(void) { return lsp_getter(5); }
int lsp_getangle(void)  { return lsp_getter(6); }
int lsp_getorient(void) { return lsp_getter(7); }
int lsp_getdist(void)   { return lsp_getter(8); }

int lsp_getstring(void) {
/*
**  The AutoLISP getstring is weird.  It has no errors associated
**  with the number or types of args.  It seems to scan the args
**  and select the 1st string as the prompt and check all the others
**  for something non-nil to set cronly.  Strange, but simple to do.
*/
    char *pmpt,*afs1;
    int cronly,msgn,fi1;
    struct resbuf *tp1;

    msgn=-1; pmpt=afs1=NULL; cronly=0;

    /* sds_getstring() will set up to 132 chars: */
    if ((afs1= new char [ 133 ])==NULL) { msgn=7; goto out; }

    for (tp1=lsp_argsll; tp1!=NULL; tp1=lsp_nextarg(tp1)) {
        if (pmpt==NULL && tp1->restype==RTSTR) pmpt=tp1->resval.rstring;
        else if (!cronly && tp1->restype!=RTNIL) cronly=1;
    }

    if ((fi1=sds_getstring(cronly,pmpt,afs1))==RTNORM) {
        if (lsp_retstr(afs1)!=RTNORM) msgn=7;
    } else if (fi1==RTCAN) {
        msgn=18;
    } else lsp_retnil();  /* Used to set msgn=19 here, but don't think */
                           /* that's right. */

    if (msgn>-1) goto out;  /* In case more code added below someday. */

out:
    if (afs1!=NULL) { IC_FREE(afs1); afs1=NULL; }
    lsp_freeiget();  /* I think this one has to clear the initget settings */
                     /* even though it never uses them. */
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_initget(void) {
/*
**  Another strange one.  The AutoLISP one seems to accept the 2 args
**  in EITHER order.  If there's only 1, it decides which by the type.
**  Bad arg type occurs under several conditions: either arg is
**  something other than an integer or a string; both args are the
**  same type; there are more than 2 args.  (There's no Too many args
**  error -- just Bad arg type.)
**
**  Note that we only save the bits and keyword list in a global struct.
**  The sds_initget() call doesn't happen until just before an
**  sds_getxxx() in the appropriate lsp_getxxx() function.  This is to
**  avoid conflicts with ADS.
*/
    char *kwl;
    int argidx,gotbits,msgn;
    int bits;
    struct resbuf *tp1;

    msgn=-1; bits=gotbits=0; kwl=NULL;

    for (tp1=lsp_argsll,argidx=0; tp1!=NULL; tp1=tp1->rbnext,argidx++) {
        if (argidx>1) { msgn=1; goto out; }
        if (tp1->restype==RTLONG || tp1->restype==RTSHORT) {
            if (gotbits) { msgn=1; goto out; }
            bits=(tp1->restype==RTLONG) ?
                (int)tp1->resval.rlong : (int)tp1->resval.rint;
            gotbits=1;
        } else if (tp1->restype==RTSTR) {
            if (kwl!=NULL) { msgn=1; goto out; }
            kwl=tp1->resval.rstring;
        } else { msgn=1; goto out; }
    }

    /* Free old initget settings and set the new ones: */
    lsp_freeiget();
    if (kwl!=NULL) {
        if ((lsp_iget.kwl= new char [strlen(kwl)+1])==NULL)
            { msgn=7; goto out; }
        strcpy(lsp_iget.kwl,kwl);
    }
    lsp_iget.bits=bits;
    lsp_iget.valid=1;

    lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_getfiled(void) {
    char *sp[3],*qq;
    int argidx,msgn;
    int flags;
    long fl1;
    struct resbuf retrb,*tp1;

    msgn=-1; sp[0]=sp[1]=sp[2]=NULL; flags=0;
    memset(&retrb,0,sizeof(retrb)); retrb.restype=RTNIL;
    qq=""/*DNT*/;  /* Safety measure */

    for (tp1=lsp_argsll,argidx=0; tp1!=NULL; tp1=tp1->rbnext,argidx++) {
        if (argidx>3) { msgn=9; goto out; }
        if ((argidx<3 && tp1->restype!=RTSTR) ||
            (argidx>2 && tp1->restype!=RTSHORT && tp1->restype!=RTLONG))
                { msgn=1; goto out; }
        if (argidx<3) sp[argidx]=(tp1->resval.rstring==NULL) ?
            qq : tp1->resval.rstring;
        else flags=(tp1->restype==RTLONG) ?
            (int)tp1->resval.rlong : (int)tp1->resval.rint;
    }
    if (argidx<4) { msgn=2; goto out; }

	// Modified PK 12/07/2000 [
	SDS_CalledFromgetfiled = 1; // Reason : Fix for bug no. 47174
	if( sds_getfiled(sp[0],sp[1],sp[2],flags,&retrb) == RTNORM) {
		if (retrb.restype==RTSHORT) {  /* Deal with longs, as usual */
			fl1=retrb.resval.rint;
			retrb.restype=RTLONG; retrb.resval.rlong=fl1;
		}
	} else retrb.restype=RTNIL;  /* Make SURE */
	SDS_CalledFromgetfiled = 0;
	// Modified PK 12/07/2000 ]

    if (lsp_retval(&retrb)!=RTNORM) { msgn=7; goto out; }

out:
    if (retrb.restype==RTSTR && retrb.resval.rstring!=NULL)
        { IC_FREE(retrb.resval.rstring); retrb.resval.rstring=NULL; }
    // EBATECH(CNBR) -[ Free unused buffer.
    if ( retrb.restype==RTLB && retrb.rbnext != NULL )
        { sds_relrb(retrb.rbnext);}
    // EBATECH(CNBR) ]-
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}
