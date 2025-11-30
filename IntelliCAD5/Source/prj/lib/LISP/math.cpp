/* C:\ICAD\PRJ\LIB\LISP\MATH.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/

int lsp_rem(void) {
    int msgn,allint,first;
    sds_real res,this1;
    struct resbuf *tp1;

    msgn=-1; allint=first=1; res=0.0;

    for (tp1=lsp_argsll; tp1!=NULL; tp1=tp1->rbnext,first=0) {

        if      (tp1->restype==RTLONG)  this1=(sds_real)tp1->resval.rlong;
        else if (tp1->restype==RTSHORT) this1=(sds_real)tp1->resval.rint;
        else if (tp1->restype==RTREAL || tp1->restype==RTANG ||
            tp1->restype==RTORINT) { this1=tp1->resval.rreal; allint=0; }
        else { msgn=1; goto out; }

        if (first) { res=this1; continue; }
        if (icadRealEqual(this1,0.0)) { msgn=4; goto out; }
        res=fmod(res,this1);

    }

    if (allint) lsp_retint((int)res); else lsp_retreal(res);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_gcd(void) {
    int msgn;
    int fint1;
    long n1,n2;

    msgn=-1;

    if (lsp_argsll==NULL) { msgn=2; goto out; }
    if      (lsp_argsll->restype==RTLONG)  n1=      lsp_argsll->resval.rlong;
    else if (lsp_argsll->restype==RTSHORT) n1=(long)lsp_argsll->resval.rint;
    else { msgn=1; goto out; }
    if (lsp_argsll->rbnext==NULL) { msgn=2; goto out; }
    if (lsp_argsll->rbnext->restype==RTLONG)
        n2=lsp_argsll->rbnext->resval.rlong;
    else if (lsp_argsll->rbnext->restype==RTSHORT)
        n2=(long)lsp_argsll->rbnext->resval.rint;
    else { msgn=1; goto out; }
    if (n1<1L || n2<1L) { msgn=52; goto out; }
    /* More args are allowed, but ignored!  (This functions would */
    /* have been a natural for multiple args, but they blew it.) */

    /* Create the prime flags buffer, if it doesn't exist: */
    if (lsp_primes==NULL &&
        ((lsp_primessz=ic_mkprimes(46341L,&lsp_primes))<0L ||
        lsp_primes==NULL)) { msgn=7; goto out; }

    if ((fint1=ic_gcd(n1,n2,lsp_primes,lsp_primessz))<1) fint1=1;
        /* Should never fail at this point. */

    lsp_retint(fint1);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_min(void) { return lsp_minmax(0); }
int lsp_max(void) { return lsp_minmax(1); }

int lsp_minmax(int op) {
    int argidx,msgn;
    sds_real res,ar1;
    struct resbuf *tp1,retrb;

    res=ar1=0.0; msgn=-1;
    retrb.restype=RTLONG; retrb.resval.rlong=0L;  /* Default */
    retrb.rbnext=NULL;

    for (tp1=lsp_argsll,argidx=0; tp1!=NULL; tp1=lsp_nextarg(tp1),argidx++) {
        if (tp1->restype==RTSHORT) {
            ar1=(sds_real)tp1->resval.rint;
        } else if (tp1->restype==RTLONG) {
            ar1=(sds_real)tp1->resval.rlong;
        } else if (tp1->restype==RTREAL || tp1->restype==RTORINT || tp1->restype==RTANG) {
            retrb.restype=RTREAL; ar1=tp1->resval.rreal;
        } else { msgn=1; goto out; }
        if (!argidx) res=ar1;
        else if ((!op && res>ar1) || (op && res<ar1)) res=ar1;
    }

    if (retrb.restype==RTREAL) retrb.resval.rreal=res;
    else retrb.resval.rlong=(long)res;

    if (lsp_retval(&retrb)!=RTNORM) { msgn=7; goto out; }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_lsh(void) {
    int msgn,first;
    unsigned int res;  /* Neg signed ints shift in 1's when */
                       /* right-shifted. */
    int this1;
    struct resbuf *tp1;

    msgn=-1; res=0; first=1;

    for (tp1=lsp_argsll; tp1!=NULL; tp1=tp1->rbnext,first=0) {

        if      (tp1->restype==RTLONG)  this1=(int)tp1->resval.rlong;
        else if (tp1->restype==RTSHORT) this1=(int)tp1->resval.rint;
        else if (tp1->restype==RTREAL || tp1->restype==RTANG ||
            tp1->restype==RTORINT) { msgn=36; goto out; }
        else { msgn=1; goto out; }

        if (first) { res=(unsigned int)this1; continue; }

        this1%=sizeof(int)*8;  /* AutoLISP seems to wrap each arg */
                                /* on the number of bits in an int. */
        if (!this1) continue;
        if (this1<0) res>>=-this1; else res<<=this1;

    }

    lsp_retint((int)res);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}


int lsp_num1arg(int op) {
    int msgn;
    sds_real arg;
    struct resbuf retrb;

    msgn=-1;
    retrb.restype=RTREAL; retrb.resval.rreal=0.0;  /* Default to RTREAL */
    retrb.rbnext=NULL;

    if (lsp_argsll==NULL)         { msgn=2; goto out; }

    if      (lsp_argsll->restype==RTSHORT)
        arg=(sds_real)lsp_argsll->resval.rint;
    else if (lsp_argsll->restype==RTLONG)
        arg=(sds_real)lsp_argsll->resval.rlong;
    else if (lsp_argsll->restype==RTREAL ||
             lsp_argsll->restype==RTANG  ||
             lsp_argsll->restype==RTORINT) arg=lsp_argsll->resval.rreal;
    else { msgn=1; goto out; }

    if (lsp_argsll->rbnext!=NULL) { msgn=9; goto out; }

    /* Set correct restype and set rreal (even for int vals so far): */
    switch (op) {
        case 0:  /* COS */
            retrb.resval.rreal=cos(arg);
            break;
        case 1:  /* SIN */
            retrb.resval.rreal=sin(arg);
            break;
        case 2:  /* TAN */
            ic_normang(&arg,NULL);
            if (fabs(arg-IC_PI/2.0)<IC_ZRO || fabs(arg-1.5*IC_PI)<IC_ZRO)
                { msgn=11; goto out; }
            retrb.resval.rreal=tan(arg);

// The old way:
//          ic_normang(&arg,NULL);
//          retrb.resval.rreal=(fabs(arg-IC_PI/2.0)<IC_ZRO ||
//              fabs(arg-1.5*IC_PI)<IC_ZRO) ? 1.0E+20 : tan(arg);

            break;
        case 3:  /* ABS */
            if (lsp_argsll->restype==RTSHORT || lsp_argsll->restype==RTLONG)
                retrb.restype=RTLONG;
            retrb.resval.rreal=(arg<0.0) ? -arg : arg;
            break;
        case 4:  /* SQRT */
            if (arg<0.0) { msgn=11; goto out; }
            retrb.resval.rreal=sqrt(arg);
            break;
        case 5: case 6:  /* LOG; LOG10 */
            if (arg<=0.0) { msgn=11; goto out; }
            retrb.resval.rreal=(op==5) ? log(arg) : log10(arg);
            break;
        case 7:  /* EXP */
            if (log10(exp(1.0))*arg>lsp_dexp) { msgn=3; goto out; }
            retrb.resval.rreal=exp(arg);
            break;
        case 8:  /* ZEROP */
            retrb.restype=(icadRealEqual(arg,0.0)) ? RTT : RTNIL;
            break;
        case 9:  /* MINUSP */
            retrb.restype=(arg<0.0) ? RTT : RTNIL;
            break;
        case 10: case 11:
            if (lsp_argsll->restype==RTSHORT || lsp_argsll->restype==RTLONG)
                retrb.restype=RTLONG;
            retrb.resval.rreal=arg+((op==10) ? 1 : -1);
            break;
    }

    if (retrb.restype==RTLONG) {
        /* Here's some residual paranoia from Commodore days: */
        retrb.resval.rlong=(retrb.resval.rreal<0.0) ?
            ((long)(retrb.resval.rreal-IC_ZRO)) :
            ((long)(retrb.resval.rreal+IC_ZRO));
    }

    if (lsp_retval(&retrb)!=RTNORM) { msgn=7; goto out; }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_cos(void)      { return lsp_num1arg( 0); }
int lsp_sin(void)      { return lsp_num1arg( 1); }
int lsp_tan(void)      { return lsp_num1arg( 2); }
int lsp_abs(void)      { return lsp_num1arg( 3); }
int lsp_sqrt(void)     { return lsp_num1arg( 4); }
int lsp_log(void)      { return lsp_num1arg( 5); }
int lsp_log10(void)    { return lsp_num1arg( 6); }
int lsp_exp(void)      { return lsp_num1arg( 7); }
int lsp_zerop(void)    { return lsp_num1arg( 8); }
int lsp_minusp(void)   { return lsp_num1arg( 9); }
int lsp_oneplus(void)  { return lsp_num1arg(10); }
int lsp_oneminus(void) { return lsp_num1arg(11); }

int lsp_expt(void) {
/*
**  AutoLISP doesn't seem to restrict the number of args.
**  If there aren't any, the result is integer 0; if there's 1,
**  the result is that arg; for more than 2, the result is found
**  by raising the 1st arg to the product of the remaining args
**  (or repeatedly raising the result so far to the power
**  of the next arg).
**
**  If all args are integer, so is the result.
*/
    int rettype,msgn;
    int fint1;
    sds_real res,base,power,abs1,abs2,ar1,ar2;
    struct resbuf *tp1;

    res=base=0.0; power=1.0; msgn=-1; rettype=RTLONG;

    for (tp1=lsp_argsll; tp1!=NULL; tp1=tp1->rbnext) {
        if        (tp1->restype==RTSHORT) {
            ar1=(sds_real)(tp1->resval.rint);
        } else if (tp1->restype==RTLONG) {
            ar1=(sds_real)(tp1->resval.rlong);
        } else if (tp1->restype==RTREAL || tp1->restype==RTORINT || tp1->restype==RTANG) {
            rettype=RTREAL; ar1=tp1->resval.rreal;
        } else { msgn=1; goto out; }

        if (tp1==lsp_argsll) { base=ar1; continue; }

        if ((abs1=fabs(power))>0.0 && (abs2=fabs(ar1))>0.0) {
            if ((ar2=log10(abs1)+log10(abs2))>lsp_dexp) { msgn=3; goto out; }
            if (ar2>-lsp_dexp) power*=ar1; else power=0.0;
        } else power=0.0;
    }

    if ((icadRealEqual(base,0.0) && power<=0.0) || (base<0.0 && !icadRealEqual(modf(power,&ar1),0.0)))
        { msgn=11; goto out; }

    res=pow(base,power);

    if (rettype==RTREAL) lsp_retreal(res);
    else {
        /* Here's some residual paranoia from Commodore days: */
        fint1=(res<0.0) ? ((int)(res-IC_ZRO)) : ((int)(res+IC_ZRO));
        lsp_retint(fint1);
    }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_atan(void) {
    int idx,msgn;
    sds_real arg1,arg2;
    struct resbuf *tp1;

    msgn=-1; arg1=arg2=0.0;

    for (tp1=lsp_argsll,idx=0; tp1!=NULL; tp1=tp1->rbnext,idx++) {

        if      (tp1->restype==RTSHORT) arg2=(sds_real)tp1->resval.rint;
        else if (tp1->restype==RTLONG)  arg2=(sds_real)tp1->resval.rlong;
        else if (tp1->restype==RTREAL || tp1->restype==RTANG ||
            tp1->restype==RTORINT)      arg2=tp1->resval.rreal;
        else { msgn=1; goto out; }

        if (idx<1) arg1=arg2;

    }
    if (idx<1) { msgn=2; goto out; }
    if (idx>2) { msgn=9; goto out; }

    if (idx==1) lsp_retreal(atan(arg1));
    else        lsp_retreal(ic_atan2(arg1,arg2));

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_add(void) {
    int mtype,fmtype,msgn;
    int fint1;
    sds_real res;
    struct resbuf *tp1;

    res=0.0; msgn=-1; mtype=fmtype=1;

    for (tp1=lsp_argsll; tp1!=NULL; tp1=lsp_nextarg(tp1)) {
        if        (tp1->restype==RTSHORT) {
            mtype=1; res+=(sds_real)(tp1->resval.rint);
        } else if (tp1->restype==RTLONG) {
            mtype=2; res+=(sds_real)(tp1->resval.rlong);
        } else if (tp1->restype==RTREAL || tp1->restype==RTORINT || tp1->restype==RTANG) {
            mtype=3; res+=tp1->resval.rreal;
        } else { msgn=1; goto out; }
        if (fmtype<mtype) fmtype=mtype;
    }

    if (fmtype<3) { fint1=(int)res; lsp_retint(fint1); }
    else lsp_retreal(res);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_subtr(void) {
    int mtype,fmtype,nargs,msgn;
    int fint1;
    sds_real res,ar1;
    struct resbuf *tp1;

    res=0.0; msgn=-1; mtype=fmtype=1;

    for (tp1=lsp_argsll,nargs=0; tp1!=NULL; tp1=lsp_nextarg(tp1),nargs++) {
        if        (tp1->restype==RTSHORT) {
            mtype=1; ar1=(sds_real)(tp1->resval.rint);
        } else if (tp1->restype==RTLONG) {
            mtype=2; ar1=(sds_real)(tp1->resval.rlong);
        } else if (tp1->restype==RTREAL || tp1->restype==RTORINT || tp1->restype==RTANG) {
            mtype=3; ar1=tp1->resval.rreal;
        } else { msgn=1; goto out; }
        if (fmtype<mtype) fmtype=mtype;
        if (nargs) res-=ar1; else res=ar1;
    }

    if (nargs==1) res=-res;  /* (- 4) should give -4 */

    if (fmtype<3) { fint1=(int)res; lsp_retint(fint1); }
    else lsp_retreal(res);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_mult(void) {
	int mtype,fmtype,nargs,msgn;
	int fint1;
	//Modified SAU 04/05/00
	int iCheck;
	sds_real res,abs1,abs2,ar1,ar2;
	struct resbuf *tp1;
	res=1.0; msgn=-1; mtype=fmtype=1;iCheck=0;
	//Modified SAU 04/05/00   [
	//Raeson:Bugfixed No.46232	  
	for (tp1=lsp_argsll,nargs=0; tp1!=NULL; tp1=lsp_nextarg(tp1),nargs++) {
		if(tp1->restype==RTSHORT && (tp1->resval.rint==0))		iCheck=1;		
		else if(tp1->restype==RTLONG && (tp1->resval.rlong==0)) iCheck=1;
		else if((tp1->restype==RTREAL || tp1->restype==RTORINT || tp1->restype==RTANG) && 
			(tp1->resval.rreal==0))							iCheck=1; 
	}
	//Modified SAU 04/05/00   ]
	for (tp1=lsp_argsll,nargs=0;(tp1!=NULL && !iCheck); tp1=lsp_nextarg(tp1),nargs++) {
		if		  (tp1->restype==RTSHORT) {
			mtype=1; ar1=(sds_real)(tp1->resval.rint);
		} else if (tp1->restype==RTLONG) {
			mtype=2; ar1=(sds_real)(tp1->resval.rlong);
		} else if (tp1->restype==RTREAL || tp1->restype==RTORINT || tp1->restype==RTANG) {
			mtype=3; ar1=tp1->resval.rreal;
		} else { msgn=1; goto out; }
		if (fmtype<mtype) fmtype=mtype;
		if ((abs1=fabs(res))>0.0 && (abs2=fabs(ar1))>0.0) {
			if ((ar2=log10(abs1)+log10(abs2))>lsp_dexp) { msgn=3; goto out; }
			if (ar2>-lsp_dexp) res*=ar1; else res=0.0;
		} else res=0.0;
	}
	//Modified SAU 04/05/00 
	//if (nargs<1) res=0.0;
	if (nargs<1 || iCheck) res=0.0;
	if (fmtype<3) { fint1=(int)res; lsp_retint(fint1); }
	else lsp_retreal(res);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_divide(void) {
/*
**  With integer arithmetic, AutoLISP seems to wait until the end to
**  to see whether to truncate.  So do we.  (/ 100 3 3.33) gives
**  10.01 (not the 9.9099 you'd get by truncating the (/ 100 3) result
**  to 33 first).
**
**  I suspect that in pure integer division, truncating as you go
**  yields the same result as truncating the floating point result
**  at the end.  Not sure how to prove it, though.
*/
    int mtype,fmtype,nargs,msgn;
    int fint1;
    sds_real res,abs1,abs2,ar1,ar2;
    struct resbuf *tp1;

    res=0.0; msgn=-1; mtype=fmtype=1;

    for (tp1=lsp_argsll,nargs=0; tp1!=NULL; tp1=lsp_nextarg(tp1),nargs++) {
        if        (tp1->restype==RTSHORT) {
            mtype=1; ar1=(sds_real)(tp1->resval.rint);
        } else if (tp1->restype==RTLONG) {
            mtype=2; ar1=(sds_real)(tp1->resval.rlong);
        } else if (tp1->restype==RTREAL || tp1->restype==RTORINT || tp1->restype==RTANG) {
            mtype=3; ar1=tp1->resval.rreal;
        } else { msgn=1; goto out; }
        if (fmtype<mtype) fmtype=mtype;
        if (nargs) {
            if ((abs2=fabs(ar1))<lsp_dmin) { msgn=4; goto out; }
            if ((abs1=fabs(res))>0.0) {
                if ((ar2=log10(abs1)-log10(abs2))>lsp_dexp) { msgn=3; goto out; }
                if (ar2>-lsp_dexp) res/=ar1; else res=0.0;
            } else res=0.0;
        } else res=ar1;
    }

    if (fmtype<3) { fint1=(int)res; lsp_retint(fint1); }
    else lsp_retreal(res);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

