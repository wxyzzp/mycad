/* C:\ICAD\PRJ\LIB\LISP\INTERPRETER.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

// THIS FILE HAS BEEN GLOBALIZED!
#include "Icad.h"/*DNT*/
#include "lisp.h"/*DNT*/
#include "icadapi.h"
#include "commandatom.h" /*DNT*/
#include "paths.h"
#include "sdsthread.h" /*DNT*/
#include "sdsapplication.h"/*DNT*/
#include "IcadHelp.h"
#include "cmds.h"/*DNT*/
#include <vector>
// Added Cybage AJK 15/10/2001 [
//   Reason: Fix for Bug No: 77845 from BugZilla
struct resbuf *args;
//Added Cybage AJK 15/10/2001 DD/MM/YYYY ]
// Modified PK 26/07/2000
struct resbuf *args_sssetfirst; // Reason : Fix to bug no. 58350
/*
**                         IMPORTANT NOTES
**
**  The material in this file is divided up into several distinct zones.
**  Find ZONE: to find the beginning of each.  They are as follows:
**
**      THE DEFINITIONS AND PROTOTYPES
**      OUR ADS FUNCTIONS (UNDER DEVELOPMENT)
**      THE ic_ LISP FUNCTIONS
**      THE INTERNAL LISP FUNCTIONS (NOW IN LISP2.C)
**      FREERS AND FUNCTIONS THAT MUST BE AWARE OF ALL LSP_RTtypes
**      OTHER UTILITY FUNCTIONS FOR OUR LISP
**      FUNCTIONS USED FOR DEBUGGING
**
**  There is an lsp_lispev() and an lsp_lispev1().  The latter expects to
**  be given one complete expression in super resbuf llist form.  The
**  former will take fragments of expressions (in string form) with
**  comments; it will remove the comments and collect fragments until it
**  has one or more complete expressions to convert to llist form and
**  pass on to lsp_lispev1().  See the comments at the top of these
**  functions for the details. Both functions are recursive -- directly
**  and/or indirectly -- and can call each other, so watch out with
**  statics and globals in them.
**
**  A SUPER RESBUF:  In several places, a resbuf is needed that holds
**  more types than a true ADS resbuf.  These super resbufs use
**  NORMAL resbufs in some ABNORMAL ways; they can hold all the RTtypes
**  as well as the LSP_RTtypes defined in icadlib.h.  For example,
**  when .restype==LSP_RTSUBR, then the .resval.rstring holds a FUNCTION
**  pointer (not to be freed) instead of a string.
**  When .restype==LSP_RTSYMB, .resval.rstring points to an allocated
**  string (just like RTSTR) and DOES need freeing.  A super resbuf
**  should be freed with lsp_freesuprb() -- NOT sds_relrb().
**  Care should be taken in passing super resbufs to functions
**  expecting normal ones -- especially ones that might try to free them.
**
**  SPECIAL RESBUF LLISTS:
**  There are also at least 2 cases of a super resbuf whose 1st link
**  is DEFINED -- not allocated (lsp_evres and the .rb inside
**  class commandAtomObj).  To free these properly, the allocated
**  stuff in the 1st link must be freed -- and all links UNDERNEATH
**  it -- but not the 1st link itself.  lsp_freesplrb() does this.
**  A resbuf llist whose 1st link is considered defined -- not allocated --
**  is referred to as a SPECIAL resbuf or SPECIAL resbuf llist.
**
**  FREEING RESBUF LINKS AND LLISTS:
**  Use lsp_freesuprb() instead of sds_relrb() or IC_FREEIT.
**  lsp_freesuprb() handles everything sds_relrb() does -- plus the
**  super types (LSP_RTtypes).  (For special resbufs, use
**  lsp_freesplrb(), as mentioned above.  DON'T CONFUSE THEM!)
**
**  DON'T FREE OLD VALUES BEFORE A lsp_copysplrb() UNLESS YOU HAVE TO.
**  lsp_copysplrb() uses existing links where possible.  If the existing
**  chain is longer than the new one, the extra links are cut off and
**  freed. If the existing chain is shorter, new links are added to the
**  end. This technique saves pointless freeing and allocating of resbuf
**  structs. (Old VALUES get freed before the links are re-used, of
**  course.)
**
**
**  RTLB/RTLE MATCHING IN LIST VALUES (lsp_argend).
**
**    To speed up lsp_argend()'s search for the end of a list, we've
**    used the resval.rstring members in those links to point to each
**    other. If the pointers are not set up yet, lsp_argend() searches
**    link by link and sets them.
**
**    The pointers are considered not set if either of the following
**    is true:
**
**        .rstring==NULL
**        .rint==-1       (ic_buildlist() sets rint to -1 for the
**                        no-value restypes like RTLB and RTLE --
**                        just as sds_buildlist() does.)
**
**    If NEITHER of these conditions is true for the RTLB link, then its
**    .rstring is expected to point to the link BEFORE the RTLE (because
**    lsp_argend() needs to set a before-the-RTLE pointer for the
**    caller). If the indicated RTLE link is found and appears valid
**    (.rstring!=NULL && rint!=-1), then it should point to the RTLB. If
**    both pointers look valid, they are assumed to be correct;
**    otherwise, the links are walked to find the matches and to set the
**    .rstring pointers.  Valid pair-matching looks like this:
**
**
**          |---------- > -----|
**        RTLB XXXX XXXX ... XXXX RTLE
**
**          |---------- < ---------|
**
**    THEREFORE, WHENEVER AN RTLB OR RTLE IS CREATED, AND WHENEVER A LIST
**    IS BROKEN (STOLEN OR PARTIALLY RE-USED) -- WHENEVER ANYTHING HAPPENS
**    THAT AFFECTS THIS .rstring POINTER TECHNIQUE -- THE APPROPRIATE
**    .rstrings SHOULD BE SET TO NULL SO THAT THEY ARE CLEARLY UNSET.
**
**
**  APPLY AND MAPCAR:
**  See lsp_apply()'s comments for an explanation of how these two
**  functions generate expressions for evaluation.  They apply the
**  quote function to the args.  There's an interesting discussion
**  about how an empty list-function (params/locals only) can trick ACAD
**  into showing the expressions that ACAD generates for evaluation
**  for apply and mapcar.
**
**  ERROR MESSAGES:
**  The handling of error messages is fairly standardized in the
**  internal lisp functions (see lisp2.c), and the same method is
**  used in a few functions in the current module.  The approach is
**  to declare a local int msgn, set it to -1, and then, if an
**  error occurs, set it to a message index (>=0) and goto out,
**  where lsp_fnerrmsg(msgn) is called.
**
**  IN GENERAL, the lisp interpreter cannot handle DXF codes in resbufs
**  -- only RT and LSP_RT codes.  Anything else will cause problems.
**  (Association lists have DXF values in dotted pairs -- but they
**  are RTLONGs, just like any other integer values.)
**  (I've added calls to ic_resval() in lsp_copysplrb(), lsp_freesplrb(),
**  and lsp_freesuprb(), so the no-DXF warning may not apply any more.)
*/



               /* ZONE: THE DEFINITIONS AND PROTOTYPES */


extern int SDS_FunctionID;
extern SDSApplication *SDS_LoadedAppsLast;

char *lsp_can="^C"/*DNT*/;  /* The LISP cancel code (to clear lsp_pend). */





extern int lsp_InsideLispCallback;
extern DWORD lsp_Dialog_CB_ThreadID;
extern HANDLE lsp_Dialog_CB_Thread;


           /* ZONE: OUR ADS FUNCTIONS (UNDER DEVELOPMENT) */


/*
**                             IMPORTANT
**
**  The functions in this file are for the IntelliCADD lisp
**  interpreter.  The calls made to some sds_ functions are expecting
**  to see OUR ads functions -- not the real ones -- so our interpreter
**  WILL NOT WORK if the real ads lib is linked in.
**
**  So, for development, a number of sds_functions are defined
**  in this file -- with lsp_ as the prefix to avoid confusion
**  with the real functions.  When done testing, be sure to do the
**  following:
**
**      Delete the lsp_ protos from this file (or comment them out).
**
**      Rename lsp_ to sds_
**
**      Activate the commented out externs and #includes so that our ads
**      functions can still find the globals in THIS file once they are
**      returned to the OURADS directory in their own files, and
**      so they'll include icadlib.h.
**
**      Move the function definitions back into the OURADS directory
**      (1 per file).
**
**      See if everything still works in software that uses OUR
**      ads functions instead of the real ones.
*/

//<alm>: class is intended:
//1 for caching freed/allocated pointers
//2 to use in small functions, like lsp_copysplrb
class CCacheOfPointers
{
	std::vector<void*> m_pool;

public:
	~CCacheOfPointers()
	{
		std::vector<void*>::iterator i = m_pool.begin();
		for (; i != m_pool.end(); ++i)
			free(*i);
	}

	void free(void* p)
	{
		m_pool.push_back(p);
	}

	void* malloc(size_t n)
	{
		if (m_pool.empty())
		{
			return ::malloc(n);			
		}
		else
		{
			void *p = realloc(m_pool.back(), n);
			if (p != NULL)
				m_pool.pop_back();
			return p;
		}
	}
};
//</alm>

/********************************************************************/

int lsp_defun2(const char *sname, int funcno) {
/*
**  For defining user-defined ADS-type functions.
**  Note that this function does not set the function
**  pointer itself; sds_regfunc() does that.
*/
    struct resbuf rb;
    int err = RTNORM;

    memset(&rb,0,sizeof(rb)); rb.restype=LSP_RTXSUB;

    if (lsp_defatom((char *)sname,funcno,&rb,1))  err = RTERROR;

    return (funcno<0 || err == RTERROR) ?
        RTERROR : RTNORM;
}

/********************************************************************/

int lsp_undef(const char *sname, int funcno) {
    return (funcno<1 || lsp_undefatom((char *)sname,funcno)) ?
        RTERROR : RTNORM;
}

/********************************************************************/

int lsp_regfunc(int (*fhdl) (void), int fcode) {
    return (lsp_setfnptr(fhdl,(int)fcode)) ? RTERROR : RTNORM;
}

/********************************************************************/

struct resbuf *lsp_getargs(void) {

    /* Demote lsp_argsll to a normal resbuf first.  (The predefined */
    /* lisp functions should use lsp_argsll directly and not do */
    /* an sds_getargs().) */

    lsp_demote(&lsp_argsll,1);

    return lsp_argsll;
}

/********************************************************************/

int lsp_getfuncode(void) {
    return lsp_lastufncd;
}

/********************************************************************/

int lsp_retint(int ival) {
    lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value. */
    lsp_evres.restype=RTLONG; lsp_evres.resval.rlong=(long)ival;
    return RTNORM;
}

/********************************************************************/

int lsp_retlist(const struct resbuf *rbp) {
    int rc;
    struct resbuf *chain,*tp1;

    chain=NULL; rc=RTNORM;

    lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value. */

    if (rbp==NULL) goto out;  /* Leaves it nil */

    /* Make the first link for lsp_copysplrb() to use: */
    if ((chain=db_alloc_resbuf())==NULL ||
        lsp_copysplrb(chain,rbp)) { rc=RTERROR; goto out; }

    // This call is adding an additional set of parentheses around
    // entity result buffers. Here we are simply testing to see if
    // we are indeed in an entity properties resbuf. If so, we patch
    // things up and return without adding the extra set of parentheses.
    if (chain->restype == RTLB &&
                            chain->rbnext->restype == RTLB &&
                                 (chain->rbnext->rbnext->restype == 5010 &&
                                  chain->rbnext->rbnext->resval.rint == -1)) {
        tp1 = chain;
        // Just for a sanity check well make sure the last two resbufs are
        // closing parens. Looking ahead one buffer here.
        while(tp1->rbnext->rbnext != NULL)
            tp1 = tp1->rbnext;
        if (tp1->restype == RTLE && tp1->rbnext->restype == RTLE) {
            // Ok we have closure. This list is already in the correct form.
            rc = RTNORM;
            lsp_evres = *chain;
            chain = NULL;
            goto out;
        }
    }

    /* Find the end: */
    tp1=chain; while (tp1->rbnext!=NULL) tp1=tp1->rbnext;

    /* Make the RTLE link: */
    if ((tp1->rbnext=db_alloc_resbuf())==NULL)
        { rc=RTERROR; goto out; }
    tp1->rbnext->restype=RTLE;
    tp1->rbnext->resval.rstring=NULL;

    /* Set 1st link to RTLB and link chain onto it: */
    lsp_evres.restype=RTLB; lsp_evres.rbnext=chain;
    lsp_evres.resval.rstring=NULL;
    chain=NULL;  /* Important! */

out:
    if (chain!=NULL) { lsp_freesuprb(chain); chain=NULL; }
    return rc;
}

/********************************************************************/

int lsp_retname(const sds_name aname, int type) {
    lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value. */
    if (aname==NULL) return RTERROR;
    lsp_evres.restype=(type==RTPICKS) ? RTPICKS : RTENAME;
    ic_encpy(lsp_evres.resval.rlname,((long *)aname));
    return RTNORM;
}

/********************************************************************/

int lsp_retnil(void) {
    lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value. */
    /* (This also sets the the 1st links restype to RTNIL.) */

    return RTNORM;
}

/********************************************************************/

int lsp_retpoint(const sds_point pt) {
/*
**  Always sets the result to a 3D point.
**  If you want to set a 2D point, use sds_retval().
*/
    sds_real *arp;

    arp=(sds_real *)pt;  /* That const problem. */

    lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value. */
    if (arp==NULL) return RTERROR;

    lsp_evres.restype=RT3DPOINT;
    ic_ptcpy(lsp_evres.resval.rpoint,arp);

    if (lsp_convptlist(&lsp_evres,1)) return RTERROR;

    return RTNORM;
}

/********************************************************************/

int lsp_retreal(sds_real rval) {
    lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value. */
    lsp_evres.restype=RTREAL; lsp_evres.resval.rreal=rval;
    return RTNORM;
}

/********************************************************************/

int lsp_retstr(const char *s) {
    lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value. */
    if (s==NULL) return RTERROR;
    if ((lsp_evres.resval.rstring= new char [ strlen(s)+1])==NULL)
        return RTERROR;
    lsp_evres.restype=RTSTR;
    strcpy(lsp_evres.resval.rstring,s);
    return RTNORM;
}

/********************************************************************/

int lsp_rett(void) {
    lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value. */
    lsp_evres.restype=RTT;
    return RTNORM;
}

/********************************************************************/

int lsp_retval(const struct resbuf *rbuf) {
/*
**  Ours has to work with super resbufs (or else we'll have to rename
**  this one for our own internal use and create another sds_retval()
**  that only uses normal resbufs).  Should be okay; WE can call it
**  with LSP_RT... codes; anyone else won't know how anyway.
**
**  Note that this function makes SURE that it only takes ONE
**  arg by temporarily terminating the llist after the first arg.
**
**  An RTPOINT or RT3DPOINT is converted to list form.
**
**  An RTVOID is transformed into the mystery symbol.
**
**  The ACAD sds_retval() doesn't return lists: it returns nil when it sees
**  the RTLB.  OURS does lists (and MUST in our internal LISP functions).
*/
    int setnil,rc,fi1;
    struct resbuf *lrbuf,*tp1,*tp2;


    rc=RTNORM; setnil=0;

    lrbuf=(struct resbuf *)rbuf;  /* That const problem. */

    /* If there's nothing, or (), set to RTNIL: */
    if (lrbuf==NULL ||
        (lrbuf->restype==RTLB && lrbuf->rbnext!=NULL &&
        lrbuf->rbnext->restype==RTLE)) { setnil=1; goto out; }

    /* Temporarily chop to 1 arg: */
    if ((tp1=lsp_argend(lrbuf,NULL))==NULL) { setnil=1; goto out; }
        /* Unterminated list?  Should't happen by this time, */
        /* but save RTERROR for insufficient memory below. */
    tp2=tp1->rbnext; tp1->rbnext=NULL;

    fi1=lsp_copysplrb(&lsp_evres,lrbuf);  /* Copy to lsp_evres. */

    tp1->rbnext=tp2;  /* Repair */

    if (fi1 || ((lsp_evres.restype==RTPOINT ||
        lsp_evres.restype==RT3DPOINT) &&
        lsp_convptlist(&lsp_evres,1))) { rc=RTERROR; setnil=1; goto out; }

    if (lsp_evres.restype==RTVOID) {
        if ((lsp_evres.resval.rstring= new char [2] )!=NULL) {
            memset(lsp_evres.resval.rstring,0,sizeof(char)*2);
			lsp_evres.resval.rstring[0]=lsp_mysterysym;
            lsp_evres.restype=LSP_RTSYMB;
        } else lsp_evres.restype=RTNIL;  /* Can't alloc 2 bytes? */
    }

out:
    if (setnil) lsp_freesplrb(&lsp_evres);
    return rc;
}

/********************************************************************/

int lsp_retvoid(void) {
/*
**  Sets the result to the unprintable mystery symbol.
*/
    lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value. */
    if ((lsp_evres.resval.rstring= new char [2] )!=NULL) {
		memset(lsp_evres.resval.rstring,0,sizeof(char)*2);
        lsp_evres.resval.rstring[0]=lsp_mysterysym;
        lsp_evres.restype=LSP_RTSYMB;
    }
    return RTNORM;
}

/********************************************************************/



                   /* ZONE: THE ic_ LISP FUNCTIONS */



void lsp_lispclean(void) {
    struct lsp_atfragslink *tafp1;

	lsp_free_dialog_handles();
    lsp_freepend();
    lsp_freeatomsll();  /* Frees lsp_atomsll, lsp_filepll, and lsp_ssll */
    lsp_freesplrb(&lsp_evres);
    lsp_freesuprb(lsp_argsll); lsp_argsll=NULL;
    lsp_freeiget();
    lsp_freestrll(lsp_tracesyms); lsp_tracesyms=NULL;
    if (lsp_primes!=NULL)
        { delete lsp_primes; lsp_primes=NULL; lsp_primessz=0L; }
    /* This just shouldn't happen, but, you know.  Just free the llist */
    /* itself -- not the temp atoms list each link points to; that */
    /* is not something we should ever do with this llist: */
    while (lsp_atfrags!=NULL)
        { tafp1=lsp_atfrags->next; delete lsp_atfrags; lsp_atfrags=tafp1; }
}

int lsp_lispev(const char *expr, struct resbuf **respp,
    int (* fnvarval)(char *varp, struct resbuf *rbp), int *pcp) {
/*
**  After lsp_lispev1() was written for single, complete, uncommented
**  expressions, and I started thinking about the LOAD command,
**  I realized that we needed to be able to handle multiple
**  expressions per line and expressions that spanned many lines.
**  We also needed to strip out comments.  Rather than complicate
**  that already complex function, I wrote this one to feed it.
**  I later changed it to handle bare atoms, too -- not just
**  expressions in parentheses.
**
**  This function collects pieces of expressions and strips out
**  comments.  The collector is lsp_pend.expr.  expr is stripped of
**  comments and tacked onto this global allocated string (which gets
**  realloc'd before the concatenation), which is then scanned for
**  atoms and complete expressions.  Those complete atoms/expressions
**  are sent to lsp_lispev1().  Anything left over is moved to the
**  beginning of lsp_pend.expr and treated as part of the next
**  expression. A '\n' is added to the end of partial expressions to
**  separate the args from adjacent lines.  That is, if | represents the
**  beginning and ending of a line,
**
**      |(+ 2|
**      |5 11)|
**
**  should give result in (+ 2\n5 11), not (+ 25 11) -- 18 not 36.
**
**  If you want to see the pending expression and the quote
**  or comment status, look at global lsp_pend.
**
**  Sending this function string lsp_can will cancel a pending
**  expression, freeing lsp_pend.expr and setting the other members
**  to 0.
**
**  When *respp!=NULL, it is pointed to the result (lsp_evres) for the
**  caller.  This also triggers the demotion of lsp_evres from a super
**  resbuf to a normal one.  (See lsp_demote(); all the exotic super
**  resbuf types are changed into normal types.  Only the evaluator
**  itself needs super restypes.)  The caller should NOT try to free it
**  (it's a special resbuf whose first link is declared -- not
**  allocated).
**
**  ONLY THE TOP-MOST CALLS TO THIS FUNCTION (FOR FINAL RESULT)
**  SHOULD PASS AN ADDRESS IN respp.  ALL RECURSION CALLS SHOULD USE NULL.
**
**  Function pointer fnvarval is used for setting the value of special
**  variables (not in the atoms list).  See lsp_lispev1() for details.
**
**  *pcp is set to the parentheses count (if pcp!=NULL).  *pcp is
**  the number of close-parentheses required to complete a pending
**  expression.
**
**  NOTE: The function undergoes indirect recursion via lsp_load()
**  at least, so be careful about the usage of statics and globals.
**  (For example, lsp_load() saves lsp_pend before calling lsp_lispev()
**  and then restores it afterward.)
**
**  Returns:
**       0 : OK
**      <0 : The lsp_lispev1() return code
*/
    char openp,closep,qt,sq,scolon,bar,fc1,*currp,*start,*eptr,*fcp1,*fcp2;
    char *pastsq;
    int msgn,elen,pc,closed,rc;
    size_t chunk,fszt1;
    struct resbuf *xll;

    static int recurlev;

    recurlev++;  /* Recursion counter */

    rc=pc=closed=0; msgn=-1; xll=NULL;
    openp='('/*DNT*/; closep=')'/*DNT*/; qt='\"'/*DNT*/; sq='\''/*DNT*/; scolon=';'/*DNT*/; bar='|'/*DNT*/;

    chunk=10240;  /* Memory increment for the pending expression */
                  /* (so we don't have to realloc every time we */
                  /* tack on a new line). */


    /* Init result to nil if it's never been set: */
    if (lsp_evres.restype==0) lsp_evres.restype=RTNIL;

    if (expr==NULL) goto out;  /* Should never. */

    if (strisame(expr,lsp_can))  /* Cancel pending expression. */
        { lsp_freepend(); goto out; }

    /* Clean up a blank pending expression: */
    if (lsp_pend.expr!=NULL && !lsp_pend.mode) {
        fcp1=lsp_pend.expr; while (*fcp1 && isspace((unsigned char) *fcp1)) fcp1++;
        if (!*fcp1) {
            if (lsp_pend.exprsz!=chunk) lsp_freepend();
            else {
                *lsp_pend.expr=0; lsp_pend.exprlen=0;
                lsp_pend.pc=lsp_pend.mode=0;
            }
        }
    }
    if (lsp_pend.expr==NULL) {
        memset(&lsp_pend,0,sizeof(lsp_pend));
        if ((lsp_pend.expr= new char [chunk]) ==NULL)
            { rc=-101; goto out; }
        *lsp_pend.expr=0; lsp_pend.exprsz=chunk;
    }

          /***** DEAL WITH COMMENTS AND QUOTES IN expr: *****/

    /* If an imbedded comment has been started with a prev expr, */
    /* find the end and skip it by moving expr beyond it: */
    if (lsp_pend.mode==2 && *expr) {
        while (expr[1] && (*expr!=bar || expr[1]!=scolon)) expr++;
        expr++; if (*expr) { expr++; lsp_pend.mode=0; }
    }

    /* Figure out where to start looking for quotes and comments: */
    if (lsp_pend.mode==1) {
        /* Quote mode active; find end of quote: */
        start=ic_endquote(expr,-1);  /* NULL if never ended */
        if (start!=NULL) { start++; lsp_pend.mode=0; }
    } else start=(char *)expr;

    /*
    **  There are 3 possibilities for start at this point:
    **      NULL : The new fragment is entirely inside a quote
    **        that begins in the pending expression.
    **      Points to "" at the fragment's end: the frag is "",
    **        or completely inside an imbedded comment
    **        that begins in the pending expression .
    **      Points to a place in the fragment that is not inside
    **        a quote or comment that started in the pending
    **        expression.  In this case, we can walk from start
    **        looking for new quotes, comments, and parentheses.
    **
    **  Beginning at start, look for quotes and comments, and compress
    **  out all comments imbedded completely in this line.  Chop any
    **  non-imbedded comments and imbedded ones that continue.
    **  Track the parentheses for this new fragment:
    */
    if (start!=NULL) {
        currp=start;
        while (*currp) {
            if (*currp==qt) {
                /* Step to end quote: */
                if ((currp=ic_endquote(currp+1,-1))==NULL)
                    { lsp_pend.mode=1; break; }  /* Quote never ends */
                currp++;
			// EBATECH(Futagami)
			} else if ((_MBC_LEAD ==_mbbtype((unsigned char)*currp,0)) &&
						(_MBC_TRAIL==_mbbtype((unsigned char)*(currp+1),1)) ){
				currp+=2;
			// ]-EBATECH
            } else if (*currp==scolon) {
                *currp=0;  /* Chop -- unless compressed out below. */
                if (currp[1]==bar) {  /* Imbedded comment */
                    lsp_pend.mode=2; fcp1=currp+2;

                    /* Look for the end of it: */
                    if (!*fcp1) break;
                    while (fcp1[1] && (*fcp1!=bar || fcp1[1]!=scolon)) fcp1++;
                    if (!fcp1[1]) break;

                    /* Compress it out: */
                    fcp1+=2; fcp2=currp; while (*fcp1) *fcp2++=*fcp1++;
                    *fcp2=0; lsp_pend.mode=0;
                    /* DON'T inc currp; need to process this char position */
                    /* again after the compression. */
                } else break;  /* Comment to EOL */
            } else if (*currp==openp) {
                lsp_pend.pc++; currp++;
            } else if (*currp==closep) {
                lsp_pend.pc--; currp++;
                if (lsp_pend.pc<1) closed=1;
            } else currp++;
        }
    }

                /***** ADD expr TO lsp_pend.expr: *****/

    elen=strlen(expr);
    fszt1=lsp_pend.exprlen+elen+10;  /* +10 for a possible '\n' and safety margin. */
    if (fszt1>lsp_pend.exprsz) {
        fszt1=(fszt1/chunk+1)*chunk;
        if ((fcp1=ic_realloc_char(lsp_pend.expr,fszt1))==NULL)
            { rc=-101; goto out; }
        lsp_pend.expr=fcp1; lsp_pend.exprsz=fszt1;
    }

	// *******************
	// IMPORTANT OPTIMIZATION
	// This strcat was taking huge amounts of time in the case of
	// large pending expressions.  Each time through it had to walk
	// to the end of the string before adding new chars at the end.
	// Results was O( n^2 ) behavior where n is number of lines in
	// the expression.
	// Since we know the length, we now just jump to near the end
	// and do the strcat there.  This reduces load to O( n ) time.
	//
	// Example:  1M LISP file loaded in 5 min 20 secs. before , now 1.7 sec.
	//
	int iOffset;
	// For short strings, don't worry
	if ( lsp_pend.exprlen < 5 )
		{
		iOffset = 0;
		}
	else
		{
		// but for long ones, jump to near the end
		iOffset = lsp_pend.exprlen - 2;
		}
	strcat((lsp_pend.expr + iOffset),expr);

	lsp_pend.exprlen+=elen;


    /***                THE ATOM/EXPRESSION EXTRACTOR                     ***/
    /*** BREAK OUT AND PASS TO lsp_lispev1() ONE ATOM OR EXPR AT A TIME.  ***/
    /*** (MAY BE MORE THAN ONE COMPLETE EXPRESSION IN lsp_pend.expr now.) ***/

    expr=lsp_pend.expr;  /* Note we're reusing expr here */
    while (!rc) {

        /* Find beginning of next atom/expression: */
        while (isspace((unsigned char) *expr)) expr++;
        if (!*expr) break;
        if (*expr==closep) { rc=-102; goto out; }  /* Extra right paren */

        /* Find the 1st char after any leading single quotes and space: */
        for (pastsq=(char *)expr; *pastsq==sq || isspace((unsigned char) *pastsq); pastsq++);
        if (!*pastsq) break;

        if (*pastsq==openp) {  /* Expression */

            if (!closed) break;  /* No completed expressions found yet. */

            /* Find matching closep: */
            /* (This pc is just a helper to set eptr; don't confuse it */
            /* with the one for the whole pending expression: lsp_pend.pc.) */
            for (eptr=(char *)pastsq+1,pc=1; *eptr; eptr++) {
                if (*eptr==qt) {  /* Start of string literal */
                    /* Jump to ending quote (ignoring escaped quotes): */
                    if ((eptr=ic_endquote(eptr+1,-1))==NULL)
                        break;  /* Incomplete quote; save as pending. */
                    continue;
                }
                if      (*eptr==openp)  pc++;
                else if (*eptr==closep) pc--;
                if (pc<1) break;
            }
            if (pc>0 || eptr==NULL) break; /* Incomplete expr or quote; */
                                           /* save as pending. */
        } else {  /* Atom */

            /* Find last char of this atom: */
            eptr=(char *)pastsq;
            if (*eptr==qt) {  /* Start of string literal */
                /* Jump to ending quote (ignoring escaped quotes): */
                if ((eptr=ic_endquote(eptr+1,-1))==NULL)
                    break;  /* Incomplete quote; save as pending. */
            } else {
                while (eptr[1] && !isspace((unsigned char) eptr[1]) && eptr[1]!=openp &&
                    eptr[1]!=closep && eptr[1]!=qt && eptr[1]!=sq) eptr++;
            }

        }

        /* Convert this complete atom/expression to llist form: */
        eptr++;
        lsp_freesuprb(xll); xll=NULL;
        fc1=*eptr; *eptr=0; rc=lsp_xstr2xll(expr,&xll); *eptr=fc1;
        if (xll==NULL || rc) goto out;  /* rc set by lsp_xstr2xll() */

        /*
        **  Convert any ()'s to RTNILs (this seems to be the perfect
        **  place to do it; lsp_xstr2xll() and lsp_lispev1() are both
        **  highly recursive).  Also, since we JUST created xll
        **  and no lsp_argend() calls have occurred yet, deleting
        **  links won't interfere with our .rstring argend system:
        */
        lsp_fixnillists(xll);

		char *pOldPend = lsp_pend.expr;

        /* Evaluate this atom/expression: */
        rc=lsp_lispev1(xll,NULL,fnvarval);

		// If lsp_lispev1 has changed lsp_pend.expr, we should get out and move on
		//
		if ( pOldPend != lsp_pend.expr )
			{
			expr = NULL;
			break;
			}

        /* Step over expression just evaluated: */
		// This is a special if() because of threads.  We kept having a hurl do to
		// (command) or sds_command(0); which was calling lsp_freepend() from another
		// thread.
		if(lsp_pend.expr)
			expr=eptr;
		else
			expr=""/*DNT*/;
    }


    /***** COPY ANY REMAINDER TO THE BEGINNING OF lsp_pend.expr *****/
    /***** AND ADD A '\n' TO SEPARATE ARGS ON DIFFERENT LINES:  *****/

    if ((lsp_pend.expr!=NULL) &&
		(expr != NULL))
		{  /* Careful!  CLRUSER may have been called */
        lsp_pend.exprlen-=expr-lsp_pend.expr;
        if (lsp_pend.exprlen<0) lsp_pend.exprlen=0;
        fcp1=lsp_pend.expr;
        if (fcp1!=expr) while (*expr) *fcp1++=*expr++;
        lsp_pend.expr[lsp_pend.exprlen]=0;
        if (*lsp_pend.expr) {
            lsp_pend.expr[lsp_pend.exprlen++]='\n'/*DNT*/;
            lsp_pend.expr[lsp_pend.exprlen]=0;
        }
    }

out:
    lsp_freesuprb(xll); xll=NULL;

    if (rc<-100) {
        /* Errors in THIS fn (not reported by lsp_lispev1() or */
        /* lsp_xstr2xll()). */

        if      (rc==-101) { msgn= 7; rc=-3; }
        else if (rc==-102) { msgn=54; rc=-1; }

        if (msgn>-1) lsp_fnerrmsg(msgn);
    }

    /*
    ** Set the result pointer for the user and demote the final result.
    ** (This can happen in lsp_lispev1(), too, in case it's called directly.
    ** It's done here in case execution never gets to the lsp_lispev1()
    ** call.)
    */
    if (respp!=NULL) {
		// Don't demote return because this is used ONLY by the
		// command line to print the lisp result.
        //lsp_demote(&lsp_evres,0); /* Make it a normal resbuf. */
        *respp=&lsp_evres;
    }

    /* On any error, free the pending expression: */
    if (rc) lsp_freepend();

    if (pcp!=NULL) *pcp=lsp_pend.pc;

    if (--recurlev<1) {  /* Ending outer call */
        recurlev=0;
//        if (rc) sds_printf("*Cancel*\n"/*DNT*/);
    }

    return rc;
}

int lsp_lispev1(struct resbuf *xll, struct resbuf **respp,
    int (* fnvarval)(char *varp, struct resbuf *rbp)) {
/*
**  Evaluates a LISP atom, or a LISP expression of the form
**
**           (function arg1 arg2 ...),
**
**  where the args can also be expressions.  The result is stored
**  in global lsp_evres.
**
**  Expects one complete atom/expression. (See lsp_lispev() for more
**  versatility.)
**
**  The atom/expression, xll, is a llist of super resbufs. Types RTLB and
**  RTLE should surround a whole expression and all inner expressions.
**  Floating point values use RTREAL, integer values use RTLONG, string
**  values use RTSTR, and dotted pair dots use RTDOTE.  The only other
**  types that can be generated from the user's keyboard input are RTNIL
**  and LSP_RTSYMB (see lsp_xstr2xll()).  However, the internal LISP
**  EVAL function can call lsp_lispev1() with a llist containing
**  LSP_RTSUBR, LSP_RTXSUB, and LSP_RTFILE types.  These types are normally
**  retrieved as values from symbols (which are then added to lsp_argsll),
**  but there's no problem if they come in directly in xll as in the
**  case of the recursion call in lsp_eval().
**
**  To evaluate an atom, lsp_evalarg() is called.
**
**  To evaluate an expression, this function loads llist lsp_argsll with
**  the args and then calls the specified function which is expected to
**  use them and to set lsp_evres to the desired result.
**
**  Variable respp gives the caller a way to get its hands on the
**  result (lsp_evres) without doing an extern.  If *respp!=NULL,
**  it is pointed to that global.  It also serves as a flag to
**  force the demotion of the final result to a normal resbuf (from
**  a super resbuf).  See lsp_demote().  The caller should NOT try
**  to free the resbuf this is pointed to; it's a special resbuf
**  (with its first link declared -- not allocated).
**
**  ONLY THE TOP-MOST CALLS TO THIS FUNCTION (FOR FINAL RESULT)
**  SHOULD PASS AN ADDRESS IN respp.  ALL RECURSION CALLS SHOULD USE NULL.
**
**  This function is recursive (directly and indirectly through
**  lsp_setq()) and other functions that delay evaluation of some args.
**  ALL RECURSION CALLS TO THIS FUNCTION SHOULD USE NULL FOR respp SO
**  THAT NO DEMOTION OF THE RESULT OCCURS UNTIL THE FINAL RESULT IS
**  OBTAINED.
**
**  SETQ NOTES:
**    SETQ is a very special function, with the following interesting
**    features:
**
**      (1) It takes bare (unquoted) symbols for the var names.
**
**      (2) It can do this sort of thing: (setq a 5 b -9 c (* a b)) --
**            which requires setting a and b BEFORE evaluating (* a b)!
**            With all other functions, all operands are evaluated first
**            before handing over the simplified args to the function.
**
**    These make SETQ quite unique, and the current function has to
**    be aware of the fact it's dealing with setq (via splfn).
**
**    We handle the evaluation problem by leaving the expressions and
**    symbols unevaluated here and letting lsp_setq() call lsp_lispev1()
**    and lsp_evalsym() (via lsp_evalarg()) to evaluated them when it
**    gets to them.  (This is indirect recursion, of course.)
**    lsp_setq() MUST STEAL lsp_argsll FOR LOCAL USE (AND SET lsp_argll
**    TO NULL), SINCE THE RECURSION CALL TO lsp_lispev1() REUSES
**    THE GLOBAL ONE.
**
**
**  OTHER FUNCTIONS THAT DELAY EVALUATION:
**    SETQ is not the only function that needs to delay evaluation of
**    some args.  IF and WHILE are other examples -- any functions
**    that conditionally evaluate args or that expect to use the results
**    of earlier expressions in later ones.  These are all handled as in
**    SETQ by leaving the inner expressions as RTLB lists in lsp_argsll
**    and leaving the symbols as LSP_RTSYMBs and letting the executed
**    functions call lsp_evalarg() to evaluate them when they get to
**    them. (This is indirect recursion, of course, for lsp_lispev1().)
**    THE FUNCTIONS THAT DO THIS MUST STEAL lsp_argsll FOR LOCAL USE
**    (AND SET lsp_argsll TO NULL), SINCE THE RECURSION CALLS TO
**    lsp_lispev1() REUSE THE GLOBAL ONE. The current function
**    (lsp_lispev1()) has to detect these special cases and take the
**    lists and symbols in unevaluated form.  (See donteval.)
**    Look at lsp_if() for an example of how to write these kind of functions.
**
**
**  FUNCTION *fnvarval rules:
**
**    Function pointer fnvarval offers a way to get vals for special
**    variables (variables not in the atoms llist lsp_atomsll). If
**    non-NULL, the function pointed to by fnvarval must look at string
**    varp to see if it is a variable it recognizes.  If so, it must
**    assign a value in *rbp.  This means allocating rbp->resval.rstring
**    if the value is to be a string.  The function does NOT have to free
**    a previous string value in *rbp.
**
**    It should return the following:
**        0 : OK (varp recognized, *rbp set for the value)
**       -1 : varp not recognized (or varp==NULL or varp is "" or rbp==NULL)
**       -2 : Insufficient memory to alloc rbp->resval.rstring for a
**              string value.
**
**  Returns:
**       0 : ok
**      -1 : Syntax error
**      -2 : Function not found
**      -3 : No memory (various things)
**      -4 : Function valid, but returned an error
**      -5 : Function not valid
**      -6 : Thread calling problem
**      -7 : User cancel
*/
    int argidx,donteval,msgn,splfn;
    int innererr,freelistfn,traceit,recap,rc,ret;
    struct resbuf *argsll,*pargp,*trbp1,*trbp2;  /* argsll is a super resbuf llist */
    struct resbuf *clink,*matchlink,*prematchlink,*argend,*nextarg;

    int (*fnptr)(void);
    struct resbuf *listfn;
    SDSApplication *ptempApp;
    class commandAtomObj *alink;

    static int recurlev;  /* Recursion counter. */
	// Added Cybage AJK 15/10/2001 [
    // Reason: Fix for Bug No: 77845 from BugZilla
	args=xll;
	// Added Cybage AJK 15/10/2001 DD/MM/YYYY ]
	{  /* Initialization; don't goto out before end of this block. */

        /* lsp_errtrace MAY have been set by a call to lsp_fnerrmsg() */
        /* before the 1st call to the present function (by lsp_lispev(), */
        /* for example): */
        if (recurlev<1) recurlev=lsp_errtrace=0;

        recurlev++;

        rc=innererr=recap=0; argsll=pargp=NULL;
        fnptr=LSP_FNPCAST NULL;
        listfn=NULL; msgn=-1; splfn=freelistfn=0;

        /* lsp_setq() and others need fnvarval; see notes at top of file. */
        lsp_fnvarval=fnvarval;

        /*  Free the prev lsp_evres value; set to NIL: */
        lsp_freesplrb(&lsp_evres);

        /* If lsp_atomsll has not been started yet, START it (with the */
        /* predefined functions and constants): */
        if (!commandAtomListStarted) lsp_defpredefd();
    }

    if (sds_usrbrk()) {
		rc=-7;  goto out;
	}

    if (xll==NULL || (xll->restype==RTNIL && xll->rbnext==NULL))
        goto out;  /* Result is nil if no expression or a single RTNIL */

    clink=xll;  /* Init current link ptr. */

    /* Check for 1st RTLB: */
    if (clink->restype!=RTLB) {  /* Not an expr; try for sym or literal. */

        /* ATOMS ONLY -- AN EXPR WOULD CAUSE INFINITE RECURSION! */

        if (lsp_evalarg(xll,NULL)==7) rc=-3;
        goto out;
    }

    /* Find matching RTLE (and the link before it): */
    if ((matchlink=lsp_argend(clink,&prematchlink))==NULL)
        { rc=-1; goto out; }  /* Never closed 1st RTLB */

    /* Step to 1st item inside the parentheses (the function): */
    clink=clink->rbnext;  /* Don't use lsp_nextarg() here! */
    if (clink==matchlink) goto out;  /* () evals to NIL */

    /* Find the function pointer or list-function to execute: */
    if ((rc=lsp_fnfromarg(clink,&fnptr,&listfn,&freelistfn,&innererr,&alink))!=0)
        goto out;

    /* Find the end of the function specifier and the beginning of the */
    /* 1st arg: */
    argend=lsp_argend(clink,NULL);
    nextarg=(argend==NULL) ? NULL : argend->rbnext;

    /*
    **  Find the functions (by function ptr -- not name -- since the
    **  user may have made copies via setq) that delay evaluation of ALL
    **  args and that never modify lsp_argsll. We can steal the args
    **  from xll for lsp_argsll, if we're careful.  We can't let any of
    **  these functions free lsp_argsll or modify it, since it's part of
    **  xll.
    */
    donteval=(fnptr!=NULL &&
       (fnptr==lsp_setq    || fnptr==lsp_if      || fnptr==lsp_and     ||
        fnptr==lsp_or      || fnptr==lsp_while   || fnptr==lsp_quote   ||
        fnptr==lsp_foreach || fnptr==lsp_cond    || fnptr==lsp_repeat  ||
        fnptr==lsp_lambda  || fnptr==lsp_trace   || fnptr==lsp_untrace ||
        fnptr==lsp_defun   || fnptr==lsp_command));

    if (donteval) {

      /* Don't eval ANY of the args; just steal them for lsp_argsll */
      /* and then put them back: */

      /* Free the old lsp_argsll: */
      if (lsp_argsll!=NULL) { lsp_freesuprb(lsp_argsll); lsp_argsll=NULL; }

      /* The following if is NECESSARY; the logic doesn't work */
      /* if there aren't any args (like (fn)) -- it makes it look */
      /* like there's one arg (the RTLE): */
      if (nextarg!=matchlink) {
          /* Steal the args temporarily and remove the matchlink RTLE */
          /* (out will re-cap xll again with the RTLE): */
          lsp_argsll=nextarg; prematchlink->rbnext=NULL;
          recap=1;
      }

      /*
      **  Now it's ESSENTIAL that the function pointed to by fnptr
      **  steal lsp_argsll for local use, set lsp_argsll to NULL so
      **  it doesn't get freed, and not modify the llist.
      **  Freeing/modifying lsp_argsll will free/modify part of xll.
      **  Make SURE the code execution from here makes it into
      **  the fnptr function -- and that they steal lsp_argsll
      **  and never free it (since it's part of xll).
      */

    } else {

      /* Find args, eval when necessary, and build local argsll -- */
      /* then set lsp_argsll to it: */

      for (argidx=0; ; argidx++) {

        /* Next arg: */
        if ((clink=nextarg)==NULL) { rc=-1; goto out; }
        if (clink==matchlink) break;  /* No more args */

        /* Prepare for next loop NOW (so we can use these ptrs below): */
        argend=lsp_argend(clink,NULL);
        nextarg=(argend==NULL) ? NULL : argend->rbnext;

        /*
        **  Free the prev lsp_evres value.  This var is used to
        **  capture each arg; it's the natural choice, because
        **  it's the one set by the recursion calls that evaluate
        **  inner expressions.  (Freer sets it to RTNIL.)
        */
//        lsp_freesplrb(&lsp_evres);  Don't think we need or want this any more.

        /* Set lsp_evres for the current arg: */

		// Modified PK 26/07/2000
		if (fnptr == lsp_sssetfirst && args_sssetfirst == NULL) // Reason : Fix to bug no. 58350
			args_sssetfirst = clink;
        switch (clink->restype) {
          case RTLB:  /* Inner expression */

            /* Check the end (RTLE): */
            if (argend==NULL) { rc=-1; goto out; }

            /* Terminate it temporarily.  (DON'T LEAVE UNTIL FIXED.) */
            argend->rbnext=NULL;

            /* Evaluate.  (Recursion; use NULL for respp.): */
            rc=lsp_lispev1(clink,NULL,fnvarval);
            if (rc) innererr=1;  /* Don't goto out yet! */
            /* Recursion result is now in lsp_evres. */

            argend->rbnext=nextarg;  /* Reconnect */

            if (rc) goto out;  /* NOW we can leave: */

            break;

          case RTLE:    case RTDOTE:    case RTNONE: case RTVOID:
          case RTPOINT: case RT3DPOINT:
            rc=-1; goto out;
            /* Invalid types.  RTLE handled in RTLB above; the POINT ones */
            /* should be in list-form here. */

          default:

            if (clink->restype==LSP_RTSYMB) {  /* Find symbol value. */
              if (lsp_evalsym(clink->resval.rstring,&lsp_evres,
                fnvarval)==-3) { rc=-3; goto out; }
            } else {  /* Must be a literal. */
              /* Isolate this link and copy to lsp_evres: */
              trbp2=clink->rbnext; clink->rbnext=NULL;
              if (lsp_copysplrb(&lsp_evres,clink)) rc=-3;
              clink->rbnext=trbp2;
              if (rc) goto out;
            }
            break;

        }

        /* Add whatever's in lsp_evres to argsll.  (Make a new copy of */
        /* the 1st link -- don't chain lsp_evres in; we can steal the */
        /* rest if it's a list, however.): */
        if ((trbp1=db_alloc_resbuf())==NULL)
            { rc=-3; goto out; }
        if (lsp_evres.restype==RTLB &&
          (fnptr!=lsp_progn || (nextarg!=NULL && nextarg!=matchlink))) {
          /*
          **  This was an important one for speed; knocked about 30%
          **  off the time in a test run.
          **
          **  Added the progn-check is so that we never steal the last
          **  arg's result from lsp_evres when we're doing a progn; that
          **  way, progn's result is already in lsp_evres and lsp_progn()
          **  doesn't need to hunt it down and set it again.
          **
          **  NOTE: If we find anything else that counts on the last
          **  arg being left in lsp_evres, check for that function, too --
          **  or just make it so we never steal the LAST arg, period.
          */
          trbp1->restype=RTLB;
          trbp1->resval.rstring=NULL;
          trbp1->rbnext=lsp_evres.rbnext;
          lsp_evres.rbnext=NULL;
          lsp_evres.restype=RTNIL;
          lsp_evres.resval.rstring=NULL;
        } else {
          if (lsp_copysplrb(trbp1,&lsp_evres)) { db_free_resbuf(trbp1); rc=-3; goto out; }
        }

        /* Tack it on: */
        if (argsll==NULL) argsll=trbp1; else pargp->rbnext=trbp1;
        pargp=trbp1;
        /* May have added multiple links, so step to the last one: */
        while (pargp->rbnext!=NULL) pargp=pargp->rbnext;

      }  /* End for each arg */

      /* Now point lsp_argsll to it (since we're done with the recursion */
      /* that could have reused it): */

      if (lsp_argsll!=NULL) lsp_freesuprb(lsp_argsll);
      lsp_argsll=argsll;
      argsll=NULL;  /* Don't let out free it now that we've pointed */
                    /* lsp_argsll at it! */

    }  /* End else not a function that delays ALL evaluation. */


    /* OK -- CALL THE FUNCTION: */

	/* It's POSSIBLE that even if we set fnptr, the pointer we set */
    /* it to was NULL: Call an external function */
    if (fnptr==NULL && listfn==NULL) {

        SDS_FunctionID=alink->id;

        // Find the thread this function belongs to.
        for(ptempApp=SDSApplication::GetEngineApplication(); ptempApp!=NULL; ptempApp=ptempApp->GetNextLink())
			{
            if(ptempApp->GetMainThread()->IsSame( alink->ThreadID ))
				break;
	        }

        // Better have found the thread.
        if(ptempApp==NULL) { rc=-6; goto out; }

		SDS_LoadedAppsLast=SDSApplication::GetEngineApplication();
        // This is REALLY wierd!!  If we are in a lisp callback we don't want to suspend the
        // command thread.  If we do the message loop for the dialog will stop.  So insted lets
        // resume the application thread and let the command thread fall through.  This should
        // be OK because we are inside a callback. The message loop from the dialog should stop
        // the code from continuing.
		// ****************************************
		// THREADSWAP !!!!!!!!!!!!!!!!!
			{
			SDSApplication *pStartApp = ptempApp;
			SDSApplication *pStopApp = SDSApplication::GetEngineApplication();
			if(FALSE==SDS_ThreadSwap(pStartApp->GetMainThread(),pStopApp->GetMainThread()))
				{
					rc=-6;
					goto out;
				}
			}
    }

    /* Now either fnptr!=NULL (if it's an internal LISP function) */
    /* or listfn!=NULL (if it's a list-function). */

    if (fnptr!=NULL) {  /* Internal LISP function */

        //*** ~| TODO - Check the return codes.  If RSERR xunload.
        ret=((*fnptr)());
        if (ret && !(ret==RSERR || ret==RSRSLT || ret==RTNORM)) { rc=-4; goto out; }

    } else if (listfn!=NULL) {  /* A list function (defun'd, etc.) */

        /* See if we should trace it: */
        if ((traceit=(lsp_tracesyms!=NULL &&
            xll->rbnext->restype==LSP_RTSYMB &&
            lsp_findstrlink(xll->rbnext->resval.rstring,lsp_tracesyms,
            NULL)!=NULL))!=0) {

            /* Print "[indenting]Entering [symbol name]: [args]\n": */

            lsp_spaceit((int)(recurlev-1));  /* The indenting */
            sds_printf(ResourceString(IDC_INTERPRETER_ENTERING_S_0,"Entering %s:"),xll->rbnext->resval.rstring);
            /* Print the args in lsp_argsll: */
            for (trbp1=lsp_argsll; trbp1!=NULL; trbp1=lsp_nextarg(trbp1)) {
                sds_printf(" "/*DNT*/);
                lsp_prtrecur(trbp1,NULL,1,0);
            }
            sds_printf("\n"/*DNT*/);
        }

        /* Eval the list-function: */
        if (lsp_dolistfn(listfn,xll)) { rc=-4; goto out; }

        if (traceit) {

            /* Print "[indenting]Result: [result]\n" */

            lsp_spaceit((int)(recurlev-1));  /* The indenting */
            sds_printf(ResourceString(IDC_INTERPRETER_RESULT_1,"Result: "));
            lsp_prtrecur(&lsp_evres,NULL,1,0);  /* The result */
            sds_printf("\n"/*DNT*/);
        }

    }


out:
    if (argsll!=NULL) { lsp_freesuprb(argsll); argsll=pargp=NULL; }
    if (freelistfn && listfn!=NULL) lsp_freesuprb(listfn);
    listfn=NULL;

    /* If lsp_argsll was stolen from xll, put xll back together again */
    /* by re-capping it (reattaching the RTLE) -- */
    /* but DON'T set lsp_argsll to NULL, since the donteval */
    /* functions steal lsp_argsll for themselves, locally: */
    if (recap) prematchlink->rbnext=matchlink;

    if (rc) {
        if      (rc==-1) msgn=24;
        else if (rc==-2) msgn= 0;
        else if (rc==-3) msgn= 7;
        else if (rc==-5) msgn= 8;
        else if (rc==-7) msgn=20;
    }

    /* Print error for CURRENT recursion (don't repeat it if it */
    /* came from a deeper recursion): */
    if (msgn>-1 && !innererr) lsp_fnerrmsg(msgn);

    /*
    ** Set the result pointer for the user and demote the final result.
    ** (This can happen in lsp_lispev(), too, in case it never gets to the
    ** lsp_lispev1() call. It's done here in case lsp_lispev1() is called
    ** directly, not via lsp_lispev().  Shouldn't happen during the
    ** recursion calls -- only at the end of the first call, for the
    ** final result.)
    */
    if (respp!=NULL) {
        lsp_demote(&lsp_evres,0); /* Make it a normal resbuf. */
        *respp=&lsp_evres;
    }

    if (lsp_errtrace) {
        if (sds_usrbrk()) lsp_errtrace=0;
        else {
            lsp_prtrecur(xll,NULL,1,0);  /* Print the expr being eval'd */
            sds_printf("\n"/*DNT*/);
        }
    }

    if (--recurlev<1) {  /* Ending the outer call */
        recurlev=lsp_errtrace=0;
    }

    return rc;
}

int lsp_test_fnvv(char *varp, struct resbuf *rbp) {
/*
**  Sample fnvarval() for lsp_lispev1().
**  BE SURE TO SEE THE RULES IN THE COMMENTS IN lsp_lispev1().
*/

    if (varp==NULL || !*varp || rbp==NULL) return -1;

    if (strisame(varp,"str"/*DNT*/)) {
        if ((rbp->resval.rstring= new char [ 11] )==NULL) return -2;
        rbp->restype=RTSTR; strcpy(rbp->resval.rstring,"String Val"/*DNT*/);
    } else if (strisame(varp,"RL"/*DNT*/)) {
        rbp->restype=RTREAL; rbp->resval.rreal=-123.456;
    } else return -1;

    return 0;
}

int lsp_loadstart(char *pn, int startup,
    int (* fnvarval)(char *varp, struct resbuf *rbp)) {
/*
**  If pathname pn is not NULL or "", this function attempts to
**  LOAD it by calling lsp_lispev().
**
**  If startup!=0 and S::STARTUP is in the atoms list, lsp_lispev()
**  is called to evaluate it.
**
**  Returns:
**       0 : OK
**      -1 : No memory for temp string.
**      -2 : An error occurred during the LOAD (should have given a
**             standard LISP error message).
**      -3 : An error occurred during the S::STARTUP evaluation
**             (should have given a standard LISP error message).
*/
    char *afs1,*afs2;
    int rc;

    rc=0;
    if ((afs1= new char [ IC_ACADBUF] )==NULL) { rc=-1; goto out; }
    if ((afs2= new char [ IC_ACADBUF] )==NULL) { rc=-1; goto out; }

    if (pn!=NULL && *pn) {
        ic_escconv(pn,afs2,'\\'/*DNT*/,lsp_true2esc,1,1);
        sprintf(afs1,"(LOAD \"%s\")"/*DNT*/,afs2);
        if (lsp_lispev(afs1,NULL,fnvarval,NULL)) { rc=-2; goto out; }
    }

    if (startup && lsp_findatom("S::STARTUP"/*DNT*/,0)!=NULL &&
        lsp_lispev("(S::STARTUP)"/*DNT*/,NULL,fnvarval,NULL)) { rc=-3; goto out; }

out:
    if (afs1!=NULL) { IC_FREE(afs1); afs1=NULL; }
    if (afs2!=NULL) { IC_FREE(afs2); afs2=NULL; }
    return rc;
}

int lsp_copysplrb(struct resbuf *dest, const struct resbuf *sour) {
/*
**  Copies resbuf llist *sour to SPECIAL resbuf *dest THAT ALREADY
**  HAS ITS 1ST LINK ALLOCATED OR DEFINED.
**
**  Note that a caller can use this function to copy a regular
**  llist of resbufs (all links allocated) by Callocing the 1st
**  link and passing it's address to the current function as dest.
**
**  WORKS WITH SUPER RESBUFS (LSP_RTtypes).
**
**    This function used to free the old dest and allocate the new one.
**    But why do all that freeing and allocating if we can just
**    reuse the existing links (as far as they go)?
**
**    This function now does this.  It is careful to free old
**    allocated strings and binary chunks and copy the new ones --
**    and free any extra dest when sour is shorter.
**
**  Returns:
**       0 : OK
**      -1 : No memory
**      -2 : Either sour or dest is NULL (or both)
*/
    int rc,fi1;
    struct resbuf *sptr,*dptr,*pdptr,*trbp1;

	//CCacheOfPointers pool;


    rc=0;

    if (sour==NULL || dest==NULL) { rc=-2; goto out; }


    for (sptr=(struct resbuf *)sour,pdptr=NULL,dptr=dest; sptr!=NULL; sptr=
        sptr->rbnext,pdptr=dptr,dptr=dptr->rbnext) {

        if (dptr==NULL) {  /* Ran out of existing links; calloc another. */
            if ((dptr=db_alloc_resbuf())==NULL) { rc=-1; goto out; }
            pdptr->rbnext=dptr;  /* Link it onto the end via the prev ptr. */
        } else {  /* Free CONTENTS of existing link. */
#if 0 //<alm>
            if ((fi1=ic_resval(dptr->restype))==RTSTR ||
#else
            if ((fi1=dptr->restype)==RTSTR ||
#endif
                dptr->restype==LSP_RTSYMB) {

//                if (dptr->resval.rstring!=NULL) pool.free(dptr->resval.rstring);
                if (dptr->resval.rstring!=NULL) IC_FREE(dptr->resval.rstring);
            } else if (fi1==RTVOID && dptr->resval.rbinary.buf!=NULL)
//                pool.free(dptr->resval.rbinary.buf);
                IC_FREE(dptr->resval.rbinary.buf);
        }

        /* Copy the source link to the dest (but preserve linkage): */
        trbp1=dptr->rbnext;
        memcpy(dptr,sptr,sizeof(struct resbuf));
        dptr->rbnext=trbp1;

        /* If it's a string or binary chunk, alloc its own copy: */
#if 0//<alm>
        if ((fi1=ic_resval(sptr->restype))==RTSTR ||
#else
        if ((fi1=sptr->restype)==RTSTR ||
#endif
            sptr->restype==LSP_RTSYMB) {

            if (sptr->resval.rstring!=NULL) {
//                if ((dptr->resval.rstring=(char *)pool.malloc(
//                    strlen(sptr->resval.rstring)+1))==NULL) { rc=-1; goto out; }
                if ((dptr->resval.rstring= new char[strlen(sptr->resval.rstring)+1]) ==NULL) 
				{ 
					rc=-1; 
					goto out; 
				}
                strcpy(dptr->resval.rstring,sptr->resval.rstring);
            }
        } else if (fi1==RTVOID) {
            if (sptr->resval.rbinary.buf!=NULL) {
//                if ((dptr->resval.rbinary.buf=(char *)pool.malloc(
//                    sptr->resval.rbinary.clen))==NULL) { rc=-1; goto out; }
                if ((dptr->resval.rbinary.buf= new char[sptr->resval.rbinary.clen])==NULL) 
				{ 
					rc=-1; 
					goto out; 
				}
                memcpy(dptr->resval.rbinary.buf,sptr->resval.rbinary.buf,
                    sptr->resval.rbinary.clen);
            }
        } else if (dptr->restype==RTLB || dptr->restype==RTLE) {
            dptr->resval.rstring=NULL;  /* (See lsp_argend()). */
        }
    }

    /*
    **  pdptr now points to the last used link.  If there's anything
    **  beyond it, free it (dest was bigger than sour).  No matter what,
    **  set pdptr->rbnext to NULL, because it ends here:
    */
    if (pdptr->rbnext!=NULL) lsp_freesuprb(pdptr->rbnext);
    pdptr->rbnext=NULL;

out:
    return rc;
}

int lsp_demote(void *vp, int candel) {
/*
**  Demotes a super resbuf llist to a normal resbuf llist by deleting or
**  changing the super (LSP_RT...) links.  Lists of 2 or 3 numerics
**  are converted to RTPOINTs or RT3DOINTS in either mode (since the
**  first link is never deleted).  RTLONGs are converted to RTSHORTs.
**
**  This is a tricky function.  If it's given a llist that has been
**  totally allocated (like lsp_argsll), it can delete the super
**  links.  If it's given a special resbuf llist where the first
**  link has been defined instead of allocated (like lsp_evres),
**  it doesn't dare try to delete the first link.
**
**  Therefore, there are two modes:
**
**      candel==0:
**          vp is expected to be a resbuf pointer; no deletion
**          is allowed -- the super types are simply changed
**          to similar RTtypes.  (LSP_RTSYMBs are changed to RTSTRs,
**          and LSP_RTSUBRs, LSP_RTXSUBs, and LSP_RTFILEs are changed
**          to RTLONGs.)
**
**      candel!=0:
**          vp is expected to be the ADDRESS OF a resbuf pointer;
**          super links are deleted.  (If the first link is deleted,
**          the resbuf pointer is changed for the caller via its address.)
**
**  THIS FUNCTION SHOULD NOT BE CALLED AT ALL WITH A LLIST THAT HAS
**  LINKS OTHER THAN THE FIRST THAT ARE DEFINED AND THEREFORE NOT FREEABLE.
**
**  Returns:
**       0 : No super types encountered
**      -1 : Encountered and deleted or fixed super types (LSP_RTtypes)
*/
    int rc;
    long fl1;
    struct resbuf *before,*rbp,*tp1,*tp2;

    rc=0;

    rbp=(candel) ? (*((struct resbuf **)vp)) : ((struct resbuf * )vp);

    lsp_convptlist(rbp,0);  /* Won't del the first link */

    for (before=NULL,tp1=rbp; tp1!=NULL; before=tp1,tp1=tp2) {

        tp2=tp1->rbnext;

        if (tp1->restype==RTLONG && tp1->resval.rlong>=-32768 &&
            tp1->resval.rlong<=32767) {  /* Change to RTSHORT */

            /* Don't try direct intra-union assignment. */
            fl1=tp1->resval.rlong; tp1->restype=RTSHORT;
            tp1->resval.rint=(int)fl1;
        }

        if (tp1->restype==LSP_RTSUBR || tp1->restype==LSP_RTXSUB ||
            tp1->restype==LSP_RTFILE) {

            rc=-1;
            if (!candel) {
                /* Don't try direct intra-union assignment. */
                fl1=(long)tp1->resval.rstring; tp1->restype=RTLONG;
                tp1->resval.rlong=fl1;
            }
        } else if (tp1->restype==LSP_RTSYMB) {
            rc=-1; if (!candel)tp1->restype=RTSTR;
        } else continue;

        if (candel) {
            if (before==NULL) *((struct resbuf **)vp)=tp2;
            else before->rbnext=tp2;
            tp1->rbnext=NULL; lsp_freesuprb(tp1); tp1=before;
        }
    }

    return rc;
}

int lsp_defatom(const char *name, int id, struct resbuf *valp, int replace, bool LSP_SDS) 
{
	//  Adds or changes a commandAtomObj in commandAtomList.
	//         name : Var or function name.
	//           id : >0 for user-defined functions "ADS-type"
	//                -1 for predefined LISP fn or var.
	//                 0 for user-defined vars and copied (SETQ'd) fns.
	//         valp : A pointer to a resbuf containing the value (which
	//                  may be a super resbuf or a llist of them).
	//                  Only the FIRST value is taken; valp can point
	//                  to a llist of many values.
	//      replace : When non-0, this function looks for an existing item of the
	//                  same name and replaces it (which is what is usually
	//                  wanted.  When 0, the new item is just added
	//                  to the top of commandAtomObj without checking
	//                  for a duplicate entry; that's faster;
	//                  lsp_defpredefd() does this when building commandAtomObj
	//                  with the predefined stuff (where there are no
	//                  duplicate items).
	//
	//  Names must be unique: a function and a var cannot have the same name.
	//  Names are case-insensitive.
	//  The user can redefine even the predefined functions and vars.
	//  Unlike sds_defun(), this function can assign function id
	//    numbers < 1.  (See definition of id above.)
	//
	//  Care is taken to ALTER existing atoms whenever possible, and to
	//  take advantage of lsp_copysplrb()'s method of reusing old
	//  links (by not freeing the old value, if we don't have to).
	//
	//  Returns:
	//       0 : OK
	//      -1 : name==NULL, name is "", or val is an unterminated list
	//      -2 : No memory
	//

    int rc, fi1;
    TCHAR nm[IC_ACADNMLEN];
    struct resbuf *tp1,*tp2 = NULL;
    class commandAtomObj *link2mod;
	
    rc=0;

    if (lsp_prepatomname(nm, (char *)name, sizeof(nm)-1, 1)) { rc=-1; goto out; }

#ifdef ORIGINALCODE
	link2mod=((replace) ? lsp_findatom(nm, 0, 0, LSP_SDS) : NULL);
    if (link2mod==NULL) 
	{
        // Note: It is legal to setq a LISP primitive to some other
        // definition (essentially overloading it). It is however up
        // to the user to save off the old definition and restore it
        // when finished. It is possible to lose a LISP primitive during
        // a given drawing session if the user fails to save/restore it.
        if (link2mod != NULL)
		    delete(link2mod);

		link2mod = new commandAtomObj();

		if (link2mod == NULL) 
		{
			rc=-2;
			goto out;
		}
		
        commandAtomList->SetAt((LPCTSTR)nm, (CObject*)link2mod);
		commandAtomObjPtrList->SetAt((void*)link2mod, (void*&)link2mod);
    }
#endif

	// NEW VERSION
	// 11/13/98 -- my theory here is, if we're replacing, just delete the
	// old one from the list and delete what it was pointing to.  Then make a new
	// one and fill its fields.  This seems much simpler
	// to me than the existing.

	link2mod=NULL;
	if (replace) 
		link2mod=lsp_findatom(nm, 0, 0, LSP_SDS);

	if (link2mod!=NULL)  // if we have it already
	{  
        // Here someone is trying to defun an already defined internal command.
        // For instance (defun _load() (...)). We are going to prepend a "L:"
        // to this function name and leave the underscore on it. ID of -2 means
        // LISP is defuning it. If link2mod->id == -3 then the user is defuning
        // something that is an internal command i.e. (defun load() (..)).
        if ((id == -2 && *nm == '_'/*DNT*/) || ((link2mod->id == -3) && !strnisame(nm, "C:"/*DNT*/, 2)))
            {
            link2mod = NULL;
            TCHAR tname[IC_ACADNMLEN];
            sprintf(tname, "L:%s", nm);
			
			link2mod=lsp_findatom(tname, 0, 0, LSP_SDS);

            if (link2mod != NULL)
                {// If LISP is defun'g this id < 0 else sds is defun'g it and id is used.
                id = id < 0 ? link2mod->id : id;
                link2mod = NULL;
                }
            strcpy(nm, tname);
            }
        else
            {
		    delete(link2mod);
		    commandAtomList->RemoveKey(nm);
		    commandAtomObjPtrList->RemoveKey(link2mod);
            }
	}

	link2mod = new commandAtomObj();

	if (link2mod == NULL) 
	{
		rc=-2;
		goto out;
	}

	if (id == -3 || strnisame(nm, "C:"/*DNT*/, 2)) 
	{
		link2mod->parensReqd = FALSE;
	}

	commandAtomList->SetAt((LPCTSTR)nm, (CObject*)link2mod);
	commandAtomObjPtrList->SetAt((void*)link2mod, (void*&)link2mod);

	// end of NEW VERSION


    // Set the members correctly (and free certain old values):
    if (link2mod->LName==NULL) 
	{
        link2mod->LName = new char[(lstrlen(nm) + 1) * sizeof(TCHAR)];
        if (link2mod->LName == NULL) 
		{
			rc=-2;
			goto out;
		}

        strcpy(link2mod->LName,nm);
    }

    // If it's an INTERNAL command line command and the second resbuf is a
    // string we are just starting ICAD. In this case we need to use the
    // second buffer as the global name and then set valp->rbnext to NULL
    // and continue processing.
	if (id == -3 && valp->rbnext != NULL && valp->rbnext->restype == RTSTR) 
	{
		ASSERT(valp->rbnext->restype == RTSTR);
        link2mod->GName = new char[(lstrlen(valp->rbnext->resval.rstring) + 1) * sizeof(TCHAR)];
        if (link2mod->GName == NULL) 
		{
			rc=-2;
			goto out;
		}

        strcpy(link2mod->GName,valp->rbnext->resval.rstring);
		valp->rbnext = NULL; // Memory is cleaned up in SDS_ReginternalFunc().

		commandAtomList->SetAt((LPCTSTR)link2mod->GName, (CObject*)link2mod);

		valp->rbnext = NULL;  // to break on
	}

    link2mod->SetFuncPtr( NULL );
    link2mod->ThreadID=GetCurrentThreadId();
    link2mod->Thread=GetCurrentThread();

    if (valp==NULL) 
	{
        link2mod->id=0;
        lsp_freesplrb(&link2mod->rb);
        link2mod->rb.restype=RTNIL;
    }
	else 
	{
        if (valp->restype==LSP_RTXSUB || id<0) link2mod->id=id;

        /* Chop, copy, restore: */
        if ((tp1=lsp_argend(valp,NULL))==NULL) 
		{
            rc=-1;
            goto out;
        }

        if(tp1 != NULL) 
		{
            tp2=tp1->rbnext;
            tp1->rbnext=NULL;
        }

        lsp_freesplrb(&(link2mod->rb));
        fi1=lsp_copysplrb(&(link2mod->rb),valp);
        tp1->rbnext=tp2;

        if (fi1) 
		{
            rc=-2;
            goto out;
        }
    }

out:
    return rc;
}

            /* ZONE: OTHER UTILITY FUNCTIONS FOR OUR LISP */


int lsp_undefatom(char *name, int id) {
//  Deletes a commandAtomObj from commandAtomList.  If id>0 (like for a
//  user-defined function), then the name AND id number must match.
//	Otherwise, the name is sufficient.
//
//  Returns:
//       0 : Deleted it
//      -1 : Not found
//
    TCHAR nm[IC_ACADNMLEN];
    class commandAtomObj  *atom;

	if (lsp_prepatomname(nm,name,sizeof(nm)-1,1))
		return -1;

	if (commandAtomList->Lookup(nm, (CObject*&)atom) != 0) {
		if (id > 0 && atom->id != id)
			return -1;
	} else
		return -1;

	// Since we are removing this atom from the commandAtomList, remove it
	// form the commandAtomObjPtrList too.
	commandAtomObjPtrList->RemoveKey((void*)atom);
	commandAtomList->RemoveKey(atom->LName);
	if(atom->GName != NULL)	// Global names are NULL for LISP atoms and sds defuns.
		commandAtomList->RemoveKey(atom->GName);
	delete (atom);

    return 0;
}

int lsp_setfnptr(int (*fhdl) (void), int fcode) {
//  Hunts down the function with id number fcode in commandAtomList
//  and sets its function pointer to fhdl.
//
//  Returns 0 if successful, -1 otherwise (can't find fcode in lsp_atomsll).
//
    class commandAtomObj *atom;

	DWORD dwThreadId = ::GetCurrentThreadId();

    if (fhdl==NULL)
		return -1;

    if ((atom = lsp_findatom((const TCHAR *)&fcode, 2, dwThreadId))==NULL)
		return -1;
    atom->rb.resval.rstring=(char *)fhdl;
    atom->SetFuncPtr( fhdl );
    return 0;
}

int lsp_numstr2rb(char *numstr, double number, struct resbuf *rbp) {
/*
**  Looks at string numstr to determine the number it represents
**  and puts it into *rbp as an RTLONG or RTREAL.
**
**  The llist linkage is not disturbed.
**
**  Note that if the resbuf rbp points to appears to have a string
**  value, the string is freed.  The idea here is that whatever *rbp
**  had in it before is being over-written.
**
**  Returns:
**       0 : OK
**      -1 : One or both passed pointers are NULL
**      -2 : numstr doesn't appear to be numeric
*/
    char *fcp1;
    sds_real ar1 = number;

    if (numstr==NULL || rbp==NULL) return -1;

    /* Free prev string (if it was a string): */
    if ((rbp->restype==RTSTR || rbp->restype==LSP_RTSYMB) &&
        rbp->resval.rstring!=NULL)
            { IC_FREE(rbp->resval.rstring); rbp->resval.rstring=NULL; }

    rbp->restype=RTNIL;  /* Default type in case not set below. */

    /*  Check for an RTREAL by looking for '.', 'e', or 'E': */
    fcp1=numstr;
	while (*fcp1 &&
			*fcp1!='.'/*DNT*/ &&
			*fcp1!='E'/*DNT*/ &&
			*fcp1!='e'/*DNT*/ ) fcp1++;
    if (*fcp1) { rbp->restype=RTREAL; rbp->resval.rreal=ar1; }
    else {
        rbp->restype=RTLONG;
        rbp->resval.rlong=(ar1<0.0) ?  /* Commodore paranoia */
            ((long)(ar1-IC_ZRO)) : ((long)(ar1+IC_ZRO));
    }

    return 0;
}


int lsp_xstr2xll(const char *str, struct resbuf **rbp) {
/*
**  Converts a string LISP expression into the equivalent as a
**  llist of super resbufs.  Expects one complete atom or expression
**  (but it may contain inner expressions).
**
**  *rbp is pointed to an allocated llist (WHICH THE CALLER MUST
**  FREE WITH lsp_freesuprb()), or NULL if ran out of memory or
**  found a syntax error.
**
**  The following items in exstr are identified and converted:
**
**      exstr item                          llist code
**      -------------------------------   ----------
**      (                                 RTLB
**      )                                 RTLE
**      . (isolated with white space)     RTDOTE
**      literal string (in quotes)        RTSTR
**      literal real                      RTREAL
**      literal integer                   RTLONG
**      symbol (anything but the above)   LSP_RTSYMB (or RTNIL if the
**                                          symbol's name is nil)
**
**  In addition, it detects 1 or more single quotes (') and applies them
**  to the next item it encounters by putting that number of
**  [RTLB][LSP_SYMB QUOTE] pairs before the item and that number of
**  [RTLE] links after it.  (An inner expression is considered a single
**  item in this regard.)
**
**  THIS FUNCTION IS RECURSIVE.  Inner expressions are treated by
**  recursion calls when an INNER '(' is encountered.
**
**  Returns:
**       0 : OK
**      <0 : An lsp_lispev1() return code
*/
    char qt,sq,openp,closep,fc1,*fcp1,*exstr=NULL;
    int bidx,eidx,innererr,first1,nsq,noutersq,pc,listing,msgn,rc,fi1;
    struct resbuf *xll,*pxll,*newrb,*tp1;

    msgn=-1; xll=pxll=newrb=NULL; rc=innererr=0;
    qt='\"'/*DNT*/; sq='\''/*DNT*/; openp='('/*DNT*/; closep=')'/*DNT*/;

    if (str==NULL) goto out;

    if ((exstr= new char [strlen(str)+1])==NULL) {
        rc=-3; goto out;
    }
    strcpy(exstr,str);

    eidx=-1;
    listing=noutersq=0;
    for (first1=1; ; first1=0) {

        nsq=0;  /* Number of single quotes before next item */

        /* Step to next item and count single quotes: */
        for (bidx=eidx+1; isspace((unsigned char) exstr[bidx]) || exstr[bidx]==sq; bidx++)
            if (exstr[bidx]==sq) nsq++;

        if (first1) noutersq=nsq;

        if (!exstr[bidx]) {  /* Done */
            /* If list never terminated, or only single quotes were sent, */
            /* that's a syntax error: */
            if (listing || (first1 && noutersq)) { rc=-1; goto out; }
            break;
        }

        /* More than 1 atom or list is an error: */
        if (!first1 && !listing) { rc=-1; goto out; }

        /* Make new link for all items except a list: */
        if (newrb!=NULL) { lsp_freesuprb(newrb); newrb=NULL; }
        if ((newrb=db_alloc_resbuf())==NULL)
            { rc=-3; goto out; }
        newrb->restype=RTNONE;  /* Precaution */

        if (exstr[bidx]==openp) {  /* 1st '(' or start of inner list. */
            if (first1) {  /* The 1st '('; just make the RTLB link. */
                newrb->restype=RTLB; eidx=bidx;
                newrb->resval.rstring=NULL;
                listing=1;
            } else {  /* Do the inner expression by recursion: */

                /* Find end of this expr: */
                for (eidx=bidx+1,pc=1; exstr[eidx]; eidx++) {
                    if (exstr[eidx]==qt) {  /* Start of string literal */
                        /* Jump to ending quote (ignoring escaped quotes): */
                        if ((fcp1=ic_endquote(exstr+eidx+1,-1))==NULL)
                            { rc=-1; goto out; }
                        eidx=fcp1-exstr;
                    } else if (exstr[eidx]==openp)  pc++;
                    else   if (exstr[eidx]==closep) pc--;
                    if (pc<1) break;
                }
                if (pc>0) { rc=-1; goto out; }

                /* Step 1 beyond the ')' and isolate it: */
                eidx++; fc1=exstr[eidx]; exstr[eidx]=0;

                /* Free the newrb that was alloc'd for single link types: */
                db_free_resbuf(newrb);
				newrb=NULL;

                /* Do the recursion call: */
                rc=lsp_xstr2xll(exstr+bidx,&newrb);

                /* Repair and step back to the ')' */
                exstr[eidx]=fc1; eidx--;

                if (rc) { innererr=1; goto out; }  /* Don't repeat message */

                /* newrb now points to a llist from the recursion call. */

            }
        } else if (exstr[bidx]==closep) {
            /* Should be the last item; all others should have been */
            /* passed while finding the ends of inner expressions above. */

            if (nsq) { rc=-1; goto out; }  /* Single quote without  */
                                           /* an item following it. */

            newrb->restype=RTLE; eidx=bidx;
            newrb->resval.rstring=NULL;
            listing=0;

        } else if (exstr[bidx]=='.'/*DNT*/) {
            newrb->restype=RTDOTE; eidx=bidx;
        } else if (exstr[bidx]==qt) {  /* Literal string */
            /* Jump to ending quote (ignoring escaped quotes): */
            if ((fcp1=ic_endquote(exstr+bidx+1,-1))==NULL) { rc=-1; goto out; }
            eidx=fcp1-exstr;  /* Final quote */
            /* Take the value without the quotes: */
            if ((newrb->resval.rstring= new char [eidx-bidx])==NULL)
                { rc=-3; goto out; }
            newrb->restype=RTSTR;
            fc1=exstr[eidx]; exstr[eidx]=0;
            ic_escconv(exstr+bidx+1,newrb->resval.rstring,'\\'/*DNT*/,
                lsp_esc2true,1,0);
            exstr[eidx]=fc1;
        } else {  /* Symbol, literal numeric, or nil */
            /* Step 1 beyond last char of this item: */
            eidx=bidx+1;
            while (exstr[eidx] && !isspace((unsigned char) exstr[eidx]) && exstr[eidx]!=openp &&
                exstr[eidx]!=closep && exstr[eidx]!=qt && exstr[eidx]!=sq) eidx++;

            fc1=exstr[eidx]; exstr[eidx]=0;  /* DON'T LEAVE UNTIL FIXED */


			double dTemp;
		   //Modified SAU 24/04/00  [
		   //Reason:(Bug Fixed)In case of Invalid floating point value
		   //		IntelliCAD results in no error and a really weird results
           /*if (ic_isnumstr(exstr+bidx,&dTemp)) {   Numeric literal
                lsp_numstr2rb(exstr+bidx,dTemp, newrb);
		   }*/
		   int iTemp=errno;
		   errno=0;
            if (ic_isnumstr(exstr+bidx,&dTemp)) {  /* Numeric literal */
				if(errno==ERANGE) { rc=-5; goto out;}
                lsp_numstr2rb(exstr+bidx,dTemp, newrb);
           //Modified SAU 24/04/00 ]
            } else {  /* Symbol (or nil) */
                if (eidx-bidx==3 && strnisame(exstr+bidx,"nil"/*DNT*/,3)) {  /* nil */
                    newrb->restype=RTNIL;
                } else {  /* Symbol */
                    if ((newrb->resval.rstring= new char [ eidx-bidx+1])!=NULL) {
                        newrb->restype=LSP_RTSYMB;
                        strncpy(newrb->resval.rstring,exstr+bidx,eidx-bidx);
                        strupr(newrb->resval.rstring);  /* Syms kept in caps */
                    } else rc=-3;  /* Don't leave yet! */
                }
            }
		   //Modified SAU 24/04/00
		   errno=iTemp;
            exstr[eidx]=fc1;  /* Repair */
            if (rc) goto out;  /* NOW it's okay. */

            eidx--;  /* Set it back to last char of this item */
        }

        /* Add to xll any "(QUOTE"'s (RTLB/LSP_RTSYMB) that single quotes */
        /* call for: */
        for (fi1=0; fi1<nsq; fi1++) {
            /* RTLB: */
            if ((tp1=db_alloc_resbuf())==NULL)
                { rc=-3; goto out; }
            tp1->restype=RTLB;
            tp1->resval.rstring=NULL;
            if (xll==NULL) xll=tp1; else pxll->rbnext=tp1;
            pxll=tp1; tp1=NULL;

            /* LSP_RTSYMB QUOTE: */
            if ((tp1=db_alloc_resbuf())==NULL)
                { rc=-3; goto out; }
            if ((tp1->resval.rstring= new char [ 6 ])==NULL)
                {
				db_free_resbuf(tp1);
				tp1=NULL;
				rc=-3;
				goto out;
				}
            tp1->restype=LSP_RTSYMB;
            strcpy(tp1->resval.rstring,"QUOTE"/*DNT*/);
            if (xll==NULL) xll=tp1; else pxll->rbnext=tp1;
            pxll=tp1; tp1=NULL;
        }

        /* Link the current item in (which maybe a llist): */
        if (newrb!=NULL) {  /* Precaution because of recursion call. */
            if (xll==NULL) xll=newrb; else pxll->rbnext=newrb;
            pxll=newrb; newrb=NULL;
            while (pxll->rbnext!=NULL) pxll=pxll->rbnext;  /* Find last */
        }

        /* For LIST MEMBERS, add ")"'s (RTLE) that single quotes call for: */
        if (!first1 && listing) {
            for (fi1=0; fi1<nsq; fi1++) {
                if ((tp1=db_alloc_resbuf())==NULL)
                    { rc=-3; goto out; }
                tp1->restype=RTLE;
                tp1->resval.rstring=NULL;
                if (xll==NULL) xll=tp1; else pxll->rbnext=tp1;
                pxll=tp1; tp1=NULL;
            }
        }

    }

    /* Add the outer RTLE's to close any single quotes that came */
    /* BEFORE the atom or list being converted: */
    if (noutersq) {
        for (fi1=0; fi1<noutersq; fi1++) {
            if ((tp1=db_alloc_resbuf())==NULL)
                { rc=-3; goto out; }
            tp1->restype=RTLE;
            tp1->resval.rstring=NULL;
            if (xll==NULL) xll=tp1; else pxll->rbnext=tp1;
            pxll=tp1; tp1=NULL;
        }
    }


out:
    /* In case never linked into xll.  (May be a llist from a recur. call.) */
    if (newrb!=NULL) { lsp_freesuprb(newrb); newrb=NULL; }

    if (exstr!=NULL) IC_FREE(exstr);

    if (rc) {
        if      (rc==-1) msgn=5;
        else if (rc==-3) msgn=7;
		//Modified SAU 24/04/2000
		else if (rc==-5) msgn=54;
        if (msgn>-1 && !innererr) lsp_fnerrmsg(msgn);
        lsp_freesuprb(xll); xll=pxll=NULL;
    }

    if (rbp!=NULL) *rbp=xll; else { lsp_freesuprb(xll); xll=NULL; }

    return rc;
}

struct resbuf *lsp_argend(struct resbuf *argp, struct resbuf **back1pp) {
/*
**  Given a ptr to a arg in a super resbuf llist, this function returns
**  a ptr to the last link in the arg (the RTLE for a list, or the arg
**  itself for a non-list).
**
**  If back1pp!=NULL, *back1pp is pointed to the link one PRIOR to
**  the RTLE for a list.  (This is handy for extracting the CONTENTS of
**  a list, without the surrounding parentheses.)
**
**  Here's a breakdown of what happens:
**      Given argp              Returns      *back1pp
**      ----------------------  -----------  -----------------------
**      NULL                    NULL         NULL
**      Non-list arg            argp         NULL
**      Terminated   RTLB list  Ptr to RTLE  Ptr to link before RTLE
**      UNterminated RTLB list  NULL         Ptr to the last link
**
**  Here's a speed idea:
**
**    Since RTLB and RTLE have no values, we'll use the resval.rstrings
**    to point to each other so we don't have to walk the links every
**    time.  If the ptrs have not been set, we walk the links and then
**    set them.  Thereafter, we just use the ptrs.
**
**    There are a couple of complications:
**
**      ACAD's sds_buildlist() seems to set resval.rint to -1 for all
**      no-value types like RTLB and RTLE; so does ours
**      (sds_buildlist()). Therefore, we should interprete both
**      rstring==NULL and rint==-1 as not set indications.
**
**      The other problem is back1pp.  To solve that problem, we point
**      the RTLB's rstring at the link BEFORE the matching RTLE, and
**      then keep that in mind when using it.  (The rstring in
**      RTLE is still set to point to the RTLB for confirmation.)
**
**    See the notes at the top of the file for further clarification.
*/
    int pc;
    struct resbuf *prev,*rtlep;

    if (back1pp!=NULL) *back1pp=NULL;

    if (argp==NULL)
        return NULL;
    if (argp->restype!=RTLB) {
        return argp;
    }
    if (argp->resval.rint==-1 ||
        (prev=(struct resbuf *)argp->resval.rstring)==NULL ||
        (rtlep=prev->rbnext)==NULL ||
        rtlep->restype!=RTLE ||
        ((struct resbuf *)rtlep->resval.rstring)!=argp) {

        for (prev=argp,rtlep=prev->rbnext,pc=1; rtlep!=NULL; prev=rtlep,
            rtlep=rtlep->rbnext) {
#if 0 //<alm>
            if      (rtlep->restype==RTLB) pc++;
            else if (rtlep->restype==RTLE) pc--;
            if (pc<1) break;
#else
            if (rtlep->restype==RTLB)
				pc++;
            else if (rtlep->restype==RTLE)
			{
				if (pc == 1)
					break;
				pc--;
			}
#endif
        }
        if (rtlep==NULL || rtlep->restype!=RTLE) {
            argp->resval.rstring=NULL;
        } else {
             argp->resval.rstring=(char *)prev;
            rtlep->resval.rstring=(char *)argp;
        }
    }

    if (back1pp!=NULL) *back1pp=prev;

    return rtlep;
}

struct resbuf *lsp_nextarg(struct resbuf *currarg) {
/*
**  Given a ptr to an arg in a super resbuf llist of args, this
**  function returns a ptr to the next arg (or NULL if there isn't one).
**  (For a normal arg, it just steps to the next link; for a list,
**  it finds the end and steps one beyond.)
**
**  This is the arg-walker used by the internal LISP functions that deal
**  with lists or that don't screen all arg types.  (Just stepping
**  to rbnext is fine if you're sure that any lists encountered will
**  be detected as bad args.)
*/
    if (currarg!=NULL && currarg->restype==RTLB)
        currarg=lsp_argend(currarg,NULL);
    if (currarg!=NULL) currarg=currarg->rbnext;
    return currarg;
}

int lsp_evalsym(char *sym, struct resbuf *splrbp,
    int (* fnvarval)(char *varp, struct resbuf *rbp)) {
/*
**  Evaluates a symbol by looking it up in the atoms llist, or resorting
**  to *fnvarval, if not found in the atoms.  (See lsp_lispev1() for
**  info about fnvarval.)  Puts the result in *splrbp, which must
**  point to a special resbuf (1st link already allocated).  This function
**  frees any previous value in *splrbp (so don't give it an uninitialized
**  local or some llist other than of super resbufs).
**
**  Returns:
**       0 : OK (got it)
**      -1 : Invalid calling params (sym or splrbp set to NULL)
**      -2 : Not found (splrbp->restype set to RTNIL)
**      -3 : No memory
*/
    int rc,fi1;
    class commandAtomObj *atom;


    rc=0;

    if (sym==NULL || splrbp==NULL) { rc=-1; goto out; }

    /* Search atoms llist: */
    atom=lsp_findatom(sym,0);
    if (atom == NULL || (atom != NULL && !atom->parensReqd && !strnisame(sym, "C:"/*DNT*/,2))) {
        TCHAR tmp[500] = "L:"/*DNT*/;
        strcat(tmp, sym);
        atom=lsp_findatom(tmp,0);
    }
    // If either it requires parens, or it is a C: defund function.
    if (atom!=NULL && (atom->parensReqd || strnisame(sym, "C:"/*DNT*/,2))) {  /* Found it */
        if (lsp_copysplrb(splrbp,&atom->rb)==-1) { rc=-3; goto out; }
    } else if (fnvarval!=NULL) {
        lsp_freesplrb(splrbp);  /* Free old val and set to RTNIL */
        if ((fi1=(*fnvarval)(sym,splrbp))!=0)
            { rc=(fi1==-2) ? -3 : -2; goto out; }
    } else { rc=-2; goto out; }

out:
    if (rc) lsp_freesplrb(splrbp);  /* Make sure it's RTNIL on error */
                                    /* and when not found. */
    return rc;
}

int lsp_fixnillists(struct resbuf *rblist) {
/*
**  Searches a llist of resbufs and converts any back-to-back RTLB/RTLE
**  pairs to RTNIL.  (LISP always treats () as nil.)
**
**  Returns the number of conversions that were made.
*/
    int rc;
    struct resbuf *tp1,*tp2;

    rc=0;

    for (tp1=rblist; tp1!=NULL; tp1=tp1->rbnext) {
        if (tp1->restype!=RTLB || (tp2=tp1->rbnext)==NULL ||
            tp2->restype!=RTLE) continue;
        tp1->restype=RTNIL; tp1->rbnext=tp2->rbnext;
        db_free_resbuf(tp2);  /* No special freer; we KNOW it's just an RTLE. */
        rc++;
    }

    return rc;
}

int lsp_convptlist(struct resbuf *rblist, int mode) {
/*
**  Converts points in a resbuf llist between the RTPOINT/RT3DPOINT
**  form and the list form.  mode tells which way to convert.
**
**  mode==0 (list to point):
**      Converts all occurrences of [RTLB][COORDS][RTLE] to an RTPOINT
**      (if there are 2 coords) or RT3DPOINT (if there are 3).
**      [COORDS] must be 2 or 3 numeric links (integer values ok).
**
**  mode!=0 (point to list):
**      Converts all RTPOINT and RT3DPOINT links to [RTLB][COORDS][RTLE]
**      form, where [COORDS] represents 2 or 3 RTREAL links.
**
**  Works with special resbufs, too, because there's no way the first
**  link will be freed (but other links may).
**
**  For list-to-point, the Z for 2D points is the current elevation.
**
**  Returns:
**       0 : OK
**      -1 : No memory (mode 1 only, because links are added)
*/
    int ok,rc,fi1,fi2;
    sds_point ap1;
    struct resbuf rb,*clink,*chain,*tp1,*tp2;


    rc=0; chain=NULL;

    if (mode) {  /* Pt to list */

        for (clink=rblist; clink!=NULL; clink=clink->rbnext) {
            if (clink->restype!=RTPOINT && clink->restype!=RT3DPOINT)
                continue;

            /* Make the new chain (everything EXCEPT the RTLB): */
			ASSERT(chain == NULL); // in which case the following line is highly unnecessary
            if (chain!=NULL)
				{
				ASSERT( chain->rbnext == NULL );
				db_free_resbuf(chain); // Hmmm...wonder if we should free the rest of the list,
				chain=NULL;
				}
            if ((chain=db_alloc_resbuf())==NULL) { rc=-1; goto out; }
            chain->restype=RTREAL;
            chain->resval.rreal=clink->resval.rpoint[0];
            tp1=chain;
            for (fi1=1,fi2=3+(clink->restype==RT3DPOINT); fi1<fi2; fi1++) {
                if ((tp1->rbnext=db_alloc_resbuf())==NULL) { rc=-1; goto out; }
                tp1=tp1->rbnext;
                if (fi1<fi2-1) {
                    tp1->restype=RTREAL;
                    tp1->resval.rreal=clink->resval.rpoint[fi1];
                } else {
                    tp1->restype=RTLE;
                    tp1->resval.rstring=NULL;
                }
            }

            /* Link it in and change the point link into the RTLB: */
            clink->restype=RTLB;
            clink->resval.rstring=NULL;  /* (See lsp_argend().) */
            tp1->rbnext=clink->rbnext;
            clink->rbnext=chain;
            clink=tp1;  /* Jump to the RTLE */
            chain=NULL;  /* Important, now that it's linked in! */
        }

    } else {  /* List to pt */

        for (clink=rblist; clink!=NULL; clink=clink->rbnext) {
            if (clink->restype!=RTLB) continue;
            ok=0;
            for (tp1=clink->rbnext,fi1=0; tp1!=NULL; tp1=tp1->rbnext,fi1++) {
                if (fi1>3) break;
                if (tp1->restype==RTLE) {
                    if (fi1==2 || fi1==3) ok=1;  /* Only successful way out. */
                    break;
                }
                if (fi1>2) break;
                if (tp1->restype==RTREAL || tp1->restype==RTANG ||
                    tp1->restype==RTORINT) {

                    ap1[fi1]=tp1->resval.rreal;
                } else if (tp1->restype==RTLONG) {
                    ap1[fi1]=(sds_real)tp1->resval.rlong;
                } else if (tp1->restype==RTSHORT) {
                    ap1[fi1]=(sds_real)tp1->resval.rint;
                } else break;
            }
            if (ok) {
                /* ap1 is set; clink points to RTLB; */
                /* tp1 points to the RTLE; fi1 is the # of coords taken. */

                if (fi1==2) {
                    clink->restype=RTPOINT;
                    ap1[2]=(sds_getvar("ELEVATION"/*DNT*/,&rb)==RTNORM) ?
                        rb.resval.rreal : 0.0;
                } else clink->restype=RT3DPOINT;

                ic_ptcpy(clink->resval.rpoint,ap1);
                tp2=clink->rbnext; clink->rbnext=tp1->rbnext;
                tp1->rbnext=NULL; lsp_freesuprb(tp2);
            }
        }

    }

out:
    if (chain!=NULL) { db_free_resbuf(chain); chain=NULL; }

    return rc;
}

int lsp_argcomp(struct resbuf *arg1p, struct resbuf *arg2p, sds_real fuzz) {
/*
**  Given 2 super resbuf args (which may be lists), this function checks
**  them for EQUALITY of VALUE (no range-checking and no eq-style binding
**  checking).  Lists are compared member by member by calling this
**  function recursively.
**
**  If the arg ptrs point to llists, only the 1st arg in each is
**  checked.  (An RTLB...RTLE list counts as 1 arg.)
**
**  NOTE: NUMERIC ARGS ARE NOT TYPE-SENSITIVE; THAT IS, 3 MATCHES 3.0.
**
**  The fuzz parameter is used for numeric comparisons and specifies
**  the amount by which the 2 numbers may differ and still be considered
**  equal.  The abs val of fuzz is used (since AutoLISP's equal
**  function seems to treat a neg fuzz as if it were the positive).
**
**  Returns:
**       0 : The args are equal
**      -1 : The args are NOT equal
*/
    int numeric,aaidx,rc;
    sds_real ara[2];
    struct resbuf *aa[2],*tp1,*tp2;

    rc=0; aa[0]=arg1p; aa[1]=arg2p;
    if (fuzz<0.0) fuzz=-fuzz;

    if (aa[0]==NULL && aa[1]==NULL) return  0;
    if (aa[0]==NULL || aa[1]==NULL) return -1;
    /* See if BOTH are numeric: */
    for (aaidx=0; aaidx<2; aaidx++) {
        numeric=(aa[aaidx]->restype==RTREAL  || aa[aaidx]->restype==RTLONG  ||
                 aa[aaidx]->restype==RTANG   || aa[aaidx]->restype==RTORINT ||
                 aa[aaidx]->restype==RTSHORT);
        if (!numeric) break;
    }
    /* Types must match if non-numeric: */
    if (!numeric && aa[0]->restype!=aa[1]->restype) return -1;

    /* Check the vals: */
    if (numeric) {

        for (aaidx=0; aaidx<2; aaidx++) {
            if      (aa[aaidx]->restype==RTLONG)
                ara[aaidx]=(sds_real)aa[aaidx]->resval.rlong;
            else if (aa[aaidx]->restype==RTSHORT)
                ara[aaidx]=(sds_real)aa[aaidx]->resval.rint;
            else
                ara[aaidx]=aa[aaidx]->resval.rreal;
        }

        if (fabs(ara[1]-ara[0])>fuzz) rc=-1;

    } else {  /* Non-numeric */

        switch (aa[0]->restype) {
            case RTSTR: case LSP_RTSYMB:
                if (strcmp(aa[0]->resval.rstring,aa[1]->resval.rstring)) rc=-1;
                break;
            case RTENAME: case RTPICKS:
                if (aa[0]->resval.rlname[0]!=aa[1]->resval.rlname[0] ||
                    aa[0]->resval.rlname[1]!=aa[1]->resval.rlname[1]) rc=-1;
                break;
            case RTPOINT: case RT3DPOINT:

                /* Shouldn't occur (since pts should be in list */
                /* form here), but does no harm. */

                if (fabs(aa[1]->resval.rpoint[0]-
                         aa[0]->resval.rpoint[0])>fuzz ||
                    fabs(aa[1]->resval.rpoint[1]-
                         aa[0]->resval.rpoint[1])>fuzz) rc=-1;
                if (rc || aa[0]->restype==RTPOINT) break;
                if (fabs(aa[1]->resval.rpoint[2]-
                         aa[0]->resval.rpoint[2])>fuzz) rc=-1;
                break;
            case LSP_RTSUBR: case LSP_RTFILE: case LSP_RTXSUB:
                if (aa[0]->resval.rstring!=aa[1]->resval.rstring) rc=-1;
                break;
            case RTLB:
                /* Step through the members and check them via recursion: */
                for (tp1=aa[0]->rbnext,tp2=aa[1]->rbnext; tp1!=NULL &&
                    tp2!=NULL && tp1->restype!=RTLE && tp2->restype!=RTLE &&
                    !rc; tp1=lsp_nextarg(tp1),tp2=lsp_nextarg(tp2))
                        if (lsp_argcomp(tp1,tp2,fuzz)) rc=-1;
                if (!rc && (tp1->restype!=RTLE || tp2->restype!=RTLE)) rc=-1;  /* One's longer */
                break;
        }

    }

    return rc;
}

commandAtomObj *
lsp_findatom(const TCHAR *key, int mode, DWORD dwThreadId, bool LSP_SDS) {
//  Searches for an atom in commandAtomList using key.  Returns a
//  pointer to the link, or NULL if not found.
//
//  The mode determines what key points to (and therefore what
//  kind of search is performed:
//      0 : Atom name (string; key is a char *)
//      1 : Function ptr (stored in ->rb.resval.rstring in the atoms llist;
//            key is a char *).
//      2 : Function ID number (key is an int *)
//      3 : Undefined command name
//      4 : Thread ID (key is an DWORD *)
//      5 : _Internal Command i.e. _line, _circle etc.
//      6 : Call by localized (translated) command name.
//  To speed future access, the found link is moved to the top of the
//  llist.
//

/*D.G.*/// Enabled for DBCS.

	void *rKey;
	POSITION Pos;
    bool  atomFound = FALSE;
    class commandAtomObj *where=NULL;
    TCHAR tmp[SDS_TILE_STR_LIMIT+2], tmpCmd[SDS_TILE_STR_LIMIT];

	

    if (key != NULL) 
	{
		switch(mode) 
		{
        case 0:		// By name.
					ASSERT(strlen(key) < SDS_TILE_STR_LIMIT);
					strcpy(tmp, key);
					_tcsupr(tmp);
					atomFound = commandAtomList->Lookup((LPCTSTR)tmp, (CObject*&)where) != 0;
					
			break;
		case 1:		// By function pointer.
					for (Pos = commandAtomObjPtrList->GetStartPosition(); Pos != NULL; ) 
					{
				commandAtomObjPtrList->GetNextAssoc(Pos, rKey, (void*&)where);

				if ((where->rb.restype == LSP_RTSUBR || where->rb.restype == LSP_RTXSUB) &&
					 		 where->rb.resval.rstring == (char *)key)  
						{
					atomFound = TRUE;
				}
			}
			break;
		case 2:		// By SDS function id number.
					for (Pos = commandAtomObjPtrList->GetStartPosition(); Pos != NULL; )
				{
				commandAtomObjPtrList->GetNextAssoc(Pos, rKey, (void*&)where);

				if (where->rb.restype == LSP_RTXSUB && where->id == *((int *)key))
					{
							if ((dwThreadId == 0) || (where->ThreadID == dwThreadId))
						{
						atomFound = TRUE;
						break;
						}
					}
				}
			break;
		case 3:		// By threadID
					for (Pos = commandAtomObjPtrList->GetStartPosition(); Pos != NULL; ) 
					{
				commandAtomObjPtrList->GetNextAssoc(Pos, rKey, (void*&)where);
						// May not need this test.
						if (where->rb.restype == LSP_RTXSUB && where->ThreadID == *((DWORD *)key)) {
					atomFound = TRUE;
					break;
				}
			}
			break;
        case 4:		// By name as command, either internal or C: (LISP defun).
					ASSERT(strlen(key) <= SDS_TILE_STR_LIMIT);
					strncpy(tmpCmd, key, SDS_TILE_STR_LIMIT);
					_tcsupr(tmpCmd);

            // Here we have been called from the command line, without parenthises.
            // So we could be an internal command or a C: defund'd command as in
            // (defun c:junk() ......).
            // Note that it is legal to have two lisp defun's with the same name that
            // differ only in the fact that one used a C: and the other didn't like:
            // (defun junk()....). junk() and c:junk() are two completely different
            // functions.

            // If we get here, the command was typed in without parenthisis, so
            // we will tack on a c: to see if this has been defun'd either from the
            // command line or from an external app. If we find it with a C: then use it
            // if not look it up without the C: and return accordingly.

			TCHAR *tmpPtr = tmpCmd;

            atomFound = commandAtomList->Lookup((LPCTSTR)tmpPtr, (CObject*&)where) != 0;

            if (atomFound)
				{
				if (where->parensReqd)
					{
					// We have found a LISP function that requires parens. The user
					// however has entered it's name without parens. In this case we
					// need to check to see if there is a defun with a C: and the same
					// name,and where_parensReqd == 0;

					// Strip off everthing up to the first character or number.
					while(*tmpPtr == '\''/*DNT*/ || *tmpPtr == '_'/*DNT*/ || *tmpPtr == '.'/*DNT*/)
						++tmpPtr;

					sprintf(tmp, "C:%s"/*DNT*/, tmpPtr);
					atomFound = commandAtomList->Lookup(tmp, (CObject*&)where) != 0;
					}
				// Internal LISP command called without parenthisis.
				if (atomFound)
					atomFound = !where->parensReqd;
                break;
				}

            // If the user undefines an internal command and then defuns a new one with
            // the same name. consider: (defun c:block() (command "_.block" .....))
            // If we run across a '.' here, we will try to find it in the atom list.
            // if it exist, then we are really calling an internal command that has been
            // redefined, and we want the real internal (base) command.
			tmpPtr = tmpCmd;
			// Strip off everthing up to the first character or number.
			bool hasDot = FALSE;
			
					while (*tmpPtr == '\''/*DNT*/ || *tmpPtr == '_'/*DNT*/ || *tmpPtr == '.'/*DNT*/)
				{
						if (*tmpPtr++ == '.'/*DNT*/)
					hasDot = TRUE;
				}
					
			if (hasDot == TRUE)
                {
				sprintf(tmp, ".%s"/*DNT*/, tmpPtr);
				atomFound = commandAtomList->Lookup(tmp, (CObject*&)where) != 0;

				// Internal LISP command called without parenthisis.
				if (atomFound)
					atomFound = !where->parensReqd;
                break;
			    }

            tmpPtr = tmpCmd;
			// Strip off everthing up to the first character or number.
			while(*tmpPtr == '\''/*DNT*/ || *tmpPtr == '_'/*DNT*/ || *tmpPtr == '.'/*DNT*/)
				++tmpPtr;

            sprintf(tmp, "C:%s"/*DNT*/, tmpPtr);
            atomFound = commandAtomList->Lookup(tmp, (CObject*&)where) != 0;

            if (atomFound)
				{
				// Internal LISP command called without parenthisis.
				atomFound = !where->parensReqd;
                break;
				}
		}
    }

	return (atomFound) ? where : ((commandAtomObj *)NULL);
}
int lsp_fixfnargs(struct resbuf *beg) {
/*
**  This function cleans up an RTLB/RTLE list of super resbufs so that
**  it is suitable as the params/locals list for a user-defined function
**  (the 1st member of the function def list, or the 2nd arg passed to
**  defun).
**
**  All non-symbol members are removed.  All duplicate symbols are
**  removed.  (If there are dup args -- before the slash -- an error
**  code is returned but they are removed.)  Duplicate slashes are
**  removed.  (Note that if the slash is not separated from the args, it
**  is treated as part of an arg name.)
**
**  If the list is emptied (so that it becomes ()), it is changed
**  to RTNIL.
**
**  beg must point to the beginning RTLB and the chain must continue
**  at least through the ending RTLE.  (It can continue beyond that,
**  and this function will doctor it in place inside a llist.)
**
**  Returns a lsp_fnerrmsg() code (but doesn't call that function).
**  That means that -1 is OK and >-1 is a message code number.
*/
    int yank,inlocals,msgn;
    struct resbuf *before,*tp1,*tp2;

    msgn=-1;

    if (beg==NULL)          { msgn=1; goto out; }
    if (beg->restype==RTNIL)          goto out;
    if (beg->restype!=RTLB) { msgn=1; goto out; }

    /* Since this function MAY remove the link just before the RTLE */
    /* (if it's a duplicate symbol), don't take any chances: */
    /* Set the RTLB's .rstring to NULL to avoid lsp_argend() errors: */
    beg->resval.rstring=NULL;

    /*
    **  A while is used instead of a for to walk the members of
    **  the params/locals list because the deletion code does NOT want
    **  the member-stepping to occur:
    */
    inlocals=0;
    before=beg; tp1=before->rbnext;
    while (tp1!=NULL && tp1->restype!=RTLE) {

        yank=0;
        if (tp1->restype==LSP_RTSYMB) {  /* Look for duplicate */

            if (!inlocals && tp1->resval.rstring[0]=='/'/*DNT*/ &&
                tp1->resval.rstring[1]==0) {

                inlocals=1;
                before=tp1; tp1=lsp_nextarg(tp1);  /* Step */
                continue;
            }

            /* Look for dups in the previous symbols: */
            for (tp2=beg->rbnext; tp2!=NULL &&
                tp2!=tp1 &&
                stricmp(tp1->resval.rstring,
                tp2->resval.rstring); tp2=lsp_nextarg(tp2));

            if (tp2!=NULL && tp2!=tp1) {
                if (!inlocals) msgn=25;  /* Flag it, but yank it and cont. */
                yank=1;
            }

        } else yank=1;

        if (yank) {  /* Del a non-symbol or a dup symbol */
            if ((tp2=lsp_argend(tp1,NULL))==NULL) { msgn=5; goto out; }
            before->rbnext=tp2->rbnext; tp2->rbnext=NULL;
            lsp_freesuprb(tp1); tp1=before->rbnext;
            continue;  /* Don't step this time; before and tp1 are ok */
        }

        before=tp1; tp1=lsp_nextarg(tp1);  /* Step */

    }

    /* If it's now (), change it into an RTNIL: */
    if (beg->rbnext==NULL) { msgn=5; goto out; }
    if (beg->rbnext->restype==RTLE) {
        tp1=beg->rbnext;
        beg->rbnext=tp1->rbnext;
        tp1->rbnext=NULL; lsp_freesuprb(tp1);
        beg->restype=RTNIL;
    }

out:
    return msgn;
}

int lsp_dolistfn(struct resbuf *lfn, struct resbuf *cexpr) {
/*
**  Executes a list-function defined by a llist of super resbufs
**  (usually a list created by defun).
**
**  lfn is expected to point to the RTLB that starts the llist
**      that defines the function.
**  cexpr should point to the llist of super resbufs that was used
**      to CALL the function.  (lsp_lispev1() just passes the
**      pointer to the expression it's evaluating.)
**
**  The params/locals list (the 1st member of the function def list)
**  should be in good shape if defun was used to define the function:
**  all duplicate symbols and non-symbols will have been removed.
**  BUT -- if the function was defined via setq or set, the params/locals
**  list could be ANYTHING, so we'll have to watch for duplicates and
**  non-symbols here.
**
**  AutoLISP seems to do the following with args and locals:
**      It walks the params/locals list and the args in parallel
**          and gripes if the number of expected args
**          doesn't exactly match the number it gets.  (The duplicate
**          params/locals and non-symbols ones count at this point and
**          are paired with args.)
**      If a member is a non-symbol or has been seen before, it is skipped
**          (and so is the matching arg).
**      If the param/local is a valid symbol, its link is removed from the
**          atoms list and saved locally; if still in the
**          args part (before the 1st slash), the atom is created
**          again with the new value from the arg it's paired with.
**      When execution of the function ends, the args and locals
**          are deleted from the atoms list and the saved atoms
**          are reattached to it.
**
**  To do this, we make a temp chain of atoms -- one for each valid
**  param and local lisp var.  For a given param/local, if there is
**  already an atom of that name, we unlink that atom from commandAtomObj
**  and link it into the temp list.  If it wasn't present in the atoms
**  list, we MAKE an atom of that name with RTNIL as its value in the
**  temp list.  This way, every arg and local is represented in the temp
**  list -- and its atom link is saved if it existed in the atoms
**  list.  Then, to restore things when we end, we can walk the temp
**  list, delete the matching atoms from the atoms list (which were used
**  as params and locals), and relink into the atoms list the temp
**  atoms whose values are NOT RTNIL.  Since existing atoms are preserved
**  by simply changing which list they are linked into, the addresses
**  of the links don't change; this allows a defun to use its own
**  name as a parameter/local, like this:
**
**      (defun f (f) (print f))
**
**  which makes an atom called f with this value:
**
**      ((F) (PRINT F))
**
**  During execution, this list-function defined by the llist of resbufs
**  for atom f is still at the same address, even though the atom itself
**  is unlinked from the atoms list and linked into the save-list, and a
**  temporary atom named f has been added to the atoms list as a
**  function parameter variable.  In the end, the list-function atom is
**  linked back into the atoms list again (after the parameter atom f is
**  removed).
**
**  There is one complication.  lsp_add2ssll() needs to check ALL
**  EXISTING ATOMS for references to selection sets, so it needs to be
**  able to search the saved atoms.  This is accomplished by using
**  a global llist of pointers to these "savatoms" llists.  That
**  global llist is lsp_atfrags.  It is maintained by the current
**  function (lsp_dolistfn()) and searched (but not altered) by
**  lsp_add2ssll().  When this function saves a llist of atoms, it adds
**  a link to lsp_atfrags with a pointer to the saved atoms.
**  When this function puts the atoms back into commandAtomObj, it removes
**  that lsp_atfrags link.
*/
    CString retkey;
    POSITION  Pos;
    CObject *rValue;
    int msgn,inlocals,issymb,fragcount;
    struct resbuf *thispar,*thisarg, *tmpLink = NULL;
	struct resbuf thismem;
	memset( &thismem, 0, sizeof( struct resbuf ) );

    class commandAtomObj *tap1,*tap2;
    struct lsp_atfragslink *tafp1;
    CMapStringToOb *saveAtomsList = new CMapStringToOb(10);

	// Modified CyberAge PP 09-22-2000 [
	// Fix for bug reported by Peter Coburn related with calc.lsp
	struct resbuf *thismem1 = NULL ;
	// Modified CyberAge PP 09-22-2000 ]
    msgn=-1; fragcount=0;

    if (lfn==NULL || lfn->restype!=RTLB || lfn->rbnext==NULL)
        { msgn=5; goto out; }
    if (lfn->rbnext->restype!=RTLB &&
        lfn->rbnext->restype!=RTNIL) { msgn=26; goto out; }


    /* Walk the params/locals and the args in parallel, */
    /* save the current atoms, and define the temp atoms for the params: */

    if (lfn->rbnext->restype==RTNIL) {  /* No parameters */

        /* ANY args are to many: */
        if (lsp_argsll!=NULL) { msgn=27; goto out; }

    } else {  /* RTLB list of parameters */

        /*
        **  Walk params and args in parallel.
        **  CAREFUL: Reaching the end of the args does NOT stop the
        **  looping in the "for" statement itself.  Inside the loop,
        **  if we still need an arg and there isn't one -- THEN we bail.
        */
        inlocals=0;
        for (thispar=lfn->rbnext->rbnext,thisarg=lsp_argsll; thispar!=NULL &&
            thispar->restype!=RTLE; thispar=lsp_nextarg(thispar),
            thisarg=(!inlocals && thisarg!=NULL) ?
            lsp_nextarg(thisarg) : thisarg) {

            issymb=(thispar->restype==LSP_RTSYMB);

            // If it's a slash, flag that we're in locals from now on:
            if (issymb && thispar->resval.rstring[0]=='/'/*DNT*/ &&
                thispar->resval.rstring[1]==0) { inlocals=1; continue; }

            // If we're out of args but we're still in the params section,
            // then break out and let the test after the loop detect
            // that there are more params than args.  (Check this BEFORE
            // checking that the param is a symbol; the param/arg pairing
            // doesn't care if the param is not a symbol.):
            if (thisarg==NULL && !inlocals) break;

            if (!issymb) continue; // Skip non-symbols

            // Have we seen this param before (i.e., is it already
            // in savatoms)?
            if (saveAtomsList->Lookup(thispar->resval.rstring, (CObject*&)tap1))
                continue;

            // Nope; this param is new.  See if it's in the atoms list:
            if ((tap2=lsp_findatom(thispar->resval.rstring,0))!=NULL) {

                // Yep.  MOVE this atom to savatoms:
                // First remove it from commandAtomList.
                commandAtomList->RemoveKey(tap2->LName);
                if (tap2->GName != NULL)
                    commandAtomList->RemoveKey(tap2->GName);

            } else {
				// Nope; make an RTNIL atom for it in savatoms.
				// Additionally flag with rb.resval.rint == RTNIL

                tap2 = new commandAtomObj();
                if (tap2 == NULL){ msgn=7; goto out; }
                tap2->LName = new char[strlen(thispar->resval.rstring)+1];

                if (tap2->LName == NULL) {
					delete(tap2);
					tap2=NULL;
					msgn=7;
					goto out;
				}

                strcpy(tap2->LName,thispar->resval.rstring);
                tap2->rb.restype = RTNIL;
				tap2->rb.resval.rint = RTNIL;
            }

            // Add the existing or new atom to savatoms:
            saveAtomsList->SetAt(tap2->LName,tap2);

            // If we're doing params before the slash, create this
            // param as an atom, using the arg as the value:
            if (!inlocals) {

                // Define the atom.  (Last 0 means don't worry about
                // replacing an existing atom; if there WAS one,
                // we moved it to savatoms above.):
                if (lsp_defatom(thispar->resval.rstring,0,thisarg,0))
                    { msgn=7; goto out; }

            }

        }  // End for each param and arg (in parallel)

        // See if the number of params and args matched:
        if ((thispar!=NULL && thispar->restype!=RTLE) ||
            thisarg!=NULL) { msgn=27; goto out; }

    }  // End else there's a list of params.


    // Get 1st member after the params/locals list:
	// Modified CyberAge PP 09-22-2000 [
	// Fix for bug reported by Peter Coburn related with calc.lsp
//	thismem = db_alloc_resbuf() ;
	if((thismem1 = lsp_nextarg(lfn->rbnext))==NULL) { msgn=5; goto out; }
	lsp_copysplrb( &thismem, thismem1 ) ;
	// Modified CyberAge PP 09-22-2000 ]

    // If there AREN'T any, set the result to the calling-expression:
    if( thismem.restype==RTLE )
	{
        if(cexpr!=NULL && lsp_retval(cexpr)!=RTNORM)
			msgn=7;
		goto out;
    }

    // If we moved any atoms to savatoms, make a new link in lsp_atfrags
    // with a pointer to where savatoms points:
    for (Pos = saveAtomsList->GetStartPosition(); Pos != NULL; ) {
        saveAtomsList->GetNextAssoc(Pos, retkey, rValue);

        if ((tafp1= new struct lsp_atfragslink )==NULL) { msgn=7; goto out; }
		memset(tafp1,0,sizeof(struct lsp_atfragslink));

        tafp1->fragp=(commandAtomObj*)rValue;
        tafp1->next=lsp_atfrags; lsp_atfrags=tafp1;

		// For each atom increment the count
		// We need this later to know how many to take off
		//
        fragcount++;
    }
    /*
    **  Evaluate the remaining members of the list-function.
    **  Interesting to note that this time we don't need to save a copy
    **  of lsp_argsll because we're not executing args from it this time;
    **  we're executing args from a list stored in the atoms list.
    */
    tmpLink = &thismem;
	for ( ; tmpLink != NULL && tmpLink->restype != RTLE; tmpLink = lsp_nextarg( tmpLink ) )
	{

        if( (msgn = lsp_evalarg( tmpLink, NULL )) != -1 )
			goto out;
    }


out:
	// Modified CyberAge PP 09-22-2000 [
	// Fix for bug reported by Peter Coburn related with calc.lsp
	lsp_freesplrb( &thismem ) ;
	// Modified CyberAge PP 09-22-2000 ]

    for (Pos = saveAtomsList->GetStartPosition(); Pos != NULL; ) {
        saveAtomsList->GetNextAssoc(Pos, retkey, (CObject*&)tap1);

        // Move the saved atoms back into lsp_atomsll and free the RTNIL
        //members of savatoms as we go:
        lsp_undefatom(tap1->LName, 0);
        saveAtomsList->RemoveKey(tap1->LName);

        // If this atom has value RTNIL, just free it; otherwise,
        // it came from the atoms list, so move it back:
		// If type is RTNIL and value has been set to RTNIL too, this is a sign
		// this is a new variable
		//
        if ( (tap1->rb.restype==RTNIL) &&
			 (tap1->rb.resval.rint == RTNIL))
			{
            delete(tap1);
            tap1=NULL;
        } else {
            commandAtomList->SetAt(tap1->LName, tap1);
            if (tap1->GName != NULL)
                commandAtomList->SetAt(tap1->GName, tap1);
            commandAtomObjPtrList->SetAt((void*)tap1, (void*&)tap1);
        }
    }
    ASSERT(saveAtomsList->GetCount() == 0);
    ASSERT(saveAtomsList->IsEmpty());
    delete(saveAtomsList);

    // If we added a link to lsp_atfrags, remove it now (since the
    // atoms list has just been repaired): */
    while ( (fragcount > 0) &&
			lsp_atfrags!=NULL)
		{
        tafp1=lsp_atfrags->next;
        delete lsp_atfrags;
        lsp_atfrags=tafp1;

		// Now decrement the count for each fragment removed, until 0
		//
		fragcount--;
	    }

    if (msgn!=-1) {  /// lsp_evalarg() may have returned -2.
        if (msgn>-1) lsp_fnerrmsg(msgn);
        return -1;
    }
    return 0;
}

int lsp_evalarg(struct resbuf *arg, int *ierrp) {
/*
**  THIS FUNCTION IS NORMALLY USED BY INTERNAL LISP FUNCTIONS THAT
**  OPERATE VIA DELAYED ARG EVALUATION -- FUNCTIONS LIKE SETQ AND
**  IF THAT DON'T WANT THEIR ARGS EVALUATED BEFORE HAND LIKE
**  MOST FUNCTIONS.  A CALL TO lsp_evalarg() CAN THEREFORE BE AN
**  INDIRECT RECURSION CALL TO lsp_lispev1().
**
**  This function evaluates a single arg -- list, symbol, or literal --
**  and places the result in lsp_evres.
**
**  Returns a lsp_fnerrmsg() code (but doesn't call that function).
**  That means that -1 is OK and >-1 is a message code number.
**  RETURNS -2 IF THE CALL TO lsp_lispev1() RETURNED AN ERROR VALUE;
**  THIS ALLOWS THE CALLER TO AVOID DUPLICATE ERROR MESSAGES.
**  If lsp_lispev1() failed AND ierrp!=NULL, *ierrp is set to the
**  return value from lsp_lispev1() for the caller.
**  The codes returned here are the following:
**      -1 : OK
**      -2 : lsp_lispev1() returned an error when evaluating a list
**             (*ierrp is set to the return val if ierrp!=NULL)
**       5 : Improper syntax (unterminated list)
**       7 : No memory
*/
    int msgn,fi1;
    struct resbuf *xend,*tp1;

    msgn=-1;

    if (ierrp!=NULL) *ierrp=0;

    if (arg==NULL) {  /* Shouldn't happen, but don't call it an error. */
        lsp_freesplrb(&lsp_evres);  /* Free and set to RTNIL. */
        goto out;
    }

    /* The 3 cases below all call functions that free the old val 1st. */

    if (arg->restype==RTLB) {  /* Inner expression to eval. */

        /* Find end of expr: */
        if ((xend=lsp_argend(arg,NULL))==NULL) { msgn=5; goto out; }

        tp1=xend->rbnext; xend->rbnext=NULL;  /* Chop temporarily */

        /* Evaluate.  (Recursion; use NULL for respp.): */
        fi1=lsp_lispev1(arg,NULL,lsp_fnvarval);

        xend->rbnext=tp1;  /* Repair */

        if (fi1) {
            if (ierrp!=NULL) *ierrp=fi1;
            msgn=-2; goto out;  /* Avoid another message */
        }

        /* Val of inner expr now in lsp_evres. */

    } else if (arg->restype==LSP_RTSYMB) {  /* Symbol to eval. */

        if (lsp_evalsym(arg->resval.rstring,&lsp_evres,
           lsp_fnvarval)==-3) { msgn=7; goto out; }

    } else {  /* A literal value; copy to lsp_evres. */

        tp1=arg->rbnext; arg->rbnext=NULL;
        fi1=lsp_copysplrb(&lsp_evres,arg);
        arg->rbnext=tp1;
        if (fi1) { msgn=7; goto out; }

    }

out:
    return msgn;
}

int lsp_add2ssll(sds_name ss) {
/*
**  Adds ss to the global llist of valid selection sets (lsp_ssll).
**
**  Frees and deletes unreferenced sel sets in lsp_ssll in the following
**  manner:  Scans the atoms list -- AND THE ATOMS SAVED BY
**  lsp_dolistfn() CALLS -- for all sel sets referenced ANYWHERE
**  (even in lists) and builds a llist of them (without duplicates).
**  Scans lsp_ssll and frees all sel sets NOT found in the new llist --
**  and frees lsp_ssll in the process.  Points lsp_ssll to the new,
**  clean list.
**
**  You can clean up lsp_ssll without adding anything new by calling
**  lsp_add2ssll(NULL).
**
**  Returns:
**       0 : OK
**      -1 : No memory
*/
    int rc;
    struct lsp_sslink *refd,*tssp1,*tssp2;
    class  commandAtomObj *tap1, *whichll;
    struct resbuf *trbp1;
    struct lsp_atfragslink *tafp1;

    rc=0; refd=NULL; whichll=NULL; tafp1=NULL;


    /*
    **  Scan the atoms list AND THE FRAGMENTS SAVED DURING lsp_dolistfn()
    **  CALLS, and make a llist of all sel sets referenced anywhere.
    **  (hashidx is used to select the proper atoms llist.
    **  For hashidx<LSP_HASHSZ, lsp_atomsll[hashidx] is searched;
    **  for hashidx>=LSP_HASHSZ, lsp_atfrags is used.
    */
    CString retkey;
    POSITION  Pos;
    for (Pos = commandAtomList->GetStartPosition(); Pos != NULL; ) {
        commandAtomList->GetNextAssoc(Pos, retkey, (CObject*&)tap1);

        for (trbp1=&tap1->rb; trbp1!=NULL; trbp1=trbp1->rbnext) {
            if (trbp1->restype!=RTPICKS)
                continue;

            /* Seen this one before in refd? */
            for (tssp1=refd; tssp1!=NULL &&
                (tssp1->ss[0]!=trbp1->resval.rlname[0] ||
                 tssp1->ss[1]!=trbp1->resval.rlname[1]); tssp1=tssp1->next);
            if (tssp1!=NULL) continue;  /* Yep. */

            /* Nope.  Add it to refd: */
            if ((tssp1= new struct lsp_sslink )==NULL) { rc=-1; goto out; }
			memset(tssp1,0,sizeof(struct lsp_sslink));
            ic_encpy(tssp1->ss,trbp1->resval.rlname);
            tssp1->next=refd; refd=tssp1;
        }

    }

    tafp1= lsp_atfrags;
    while(tafp1 != NULL) {

        whichll=tafp1->fragp;
        for (trbp1=(struct sds_resbuf*)&(whichll->rb); trbp1!=NULL; trbp1=trbp1->rbnext) {
            if (trbp1->restype!=RTPICKS) continue;

            /* Seen this one before in refd? */
            for (tssp1=refd; tssp1!=NULL &&
                (tssp1->ss[0]!=trbp1->resval.rlname[0] ||
                 tssp1->ss[1]!=trbp1->resval.rlname[1]); tssp1=tssp1->next);
            if (tssp1!=NULL) continue;  /* Yep. */

            /* Nope.  Add it to refd: */
            if ((tssp1= new struct lsp_sslink )==NULL) { rc=-1; goto out; }
			memset(tssp1,0,sizeof(struct lsp_sslink));
            ic_encpy(tssp1->ss,trbp1->resval.rlname);
            tssp1->next=refd; refd=tssp1;
        }
        tafp1 = tafp1->next;
    }

    /* Free lsp_ssll and sds_ssfree() all sel sets not found in refd */
    /* in the process: */
    while (lsp_ssll!=NULL) {
        tssp1=lsp_ssll->next;
        for (tssp2=refd; tssp2!=NULL &&
            (tssp2->ss[0]!=lsp_ssll->ss[0] ||
             tssp2->ss[1]!=lsp_ssll->ss[1]); tssp2=tssp2->next);
        if (tssp2==NULL) sds_ssfree(lsp_ssll->ss);
        delete lsp_ssll; lsp_ssll=tssp1;
    }

    /* refd becomes the new lsp_ssll: */
    lsp_ssll=refd; refd=NULL;


    if (ss!=NULL) {
        /* Make the new link and tack it onto the top of lsp_ssll: */
        if ((tssp1= new struct lsp_sslink )==NULL) { rc=-1; goto out; }
		memset(tssp1,0,sizeof(struct lsp_sslink));
        ic_encpy(tssp1->ss,ss);
        tssp1->next=lsp_ssll; lsp_ssll=tssp1;
    }

out:
    while (refd!=NULL) { tssp1=refd->next; delete refd; refd=tssp1; }
    return rc;
}



int lsp_1associtem(struct resbuf *sour, struct resbuf **dest, int ads2lisp) {
/*
**  THIS FUNCTION IS AN EXTENSION OF lsp_convassoc().
**
**  Converts 1 DXF association list item between the LISP form
**  (a dotted pair or DXF-point/vector-list) and the ADS form
**  (a single resbuf with a DXF code).
**
**  NOTE: COMPLEX ITEMS (LIKE DXF -3 IN LISP EED, AND -3 AND 1001 IN
**  ADS EED) ARE NOT HANDLED BY THIS FUNCTION (-1 IS RETURNED).
**  lsp_convassoc() processes those itself, calling this function to do
**  the EED values when it gets to them.
**
**  Variable ads2lisp tells which way to translate:
**          0 : LISP to ADS
**      non-0 : ADS to LISP
**
**  THE CALLER MUST FREE THE *dest LINK OR LIST (IF THAT POINTER IS SET
**  TO NON-NULL, MEANING THAT IT POINTS AT ALLOCATED MEMORY).
**
**  Returns:
**       0 : OK
**      -1 : sour does not point to an item of the proper form,
**             or dest is NULL.
**      -2 : No memory.
*/
    char fc1;
    int rc,dxf,rv,bclen,fi1,fi2;
    struct resbuf *chain,*pchain,*tlist,*thislink,*xend,*tp1;


    chain=pchain=tlist=NULL; rc=0;

    if (dest==NULL) { rc=-1; goto out; }
    else *dest=NULL;  /* Init */


    if (sour==NULL) goto out;  /* Not an error */

    if (ads2lisp) {  /* ADS to LISP */

        /* Check for valid DXF code: */
        dxf=sour->restype;
        if (dxf<-5 || dxf>1999) { rc=-1; goto out; }
        if ((rv=ic_resval(dxf))==RTERROR) { rc=-1; goto out; }

        /* Free old chain (precaution) and make the first link: */
        if (chain!=NULL) { lsp_freesuprb(chain); chain=pchain=NULL; }

        /* Start with RTLB link: */
        if ((chain=db_alloc_resbuf())==
            NULL) { rc=-2; goto out; }
        chain->restype=RTLB;
        chain->resval.rstring=NULL;
        pchain=chain;

        /* Add the DXF code: */
        if ((pchain->rbnext=db_alloc_resbuf())==NULL) { rc=-2; goto out; }
        pchain=pchain->rbnext;
        pchain->restype=RTLONG;
        pchain->resval.rlong=(long)dxf;

        /* Add a dot (RTDOTE) if this type requires one: */
        if (dxf!=-3 && rv!=RT3DPOINT) {
            if ((pchain->rbnext=db_alloc_resbuf())==NULL) { rc=-2; goto out; }
            pchain=pchain->rbnext;
            pchain->restype=RTDOTE;
        }


                     /* DEAL WITH EACH DXF TYPE: */

        /* Make the 1st value-link (only points will need more): */
        if ((pchain->rbnext=db_alloc_resbuf())==NULL) { rc=-2; goto out; }
        pchain=pchain->rbnext;
        pchain->restype=RTNONE;  /* So we can tell if it gets set below */

        /* EED sentinel and app names are handled in lsp_convassoc(): */
        if (dxf==-3 || dxf==1001) { rc=-1; goto out; }

        switch (rv) {

            case RTREAL:

                pchain->resval.rreal=sour->resval.rreal;
                pchain->restype=RTREAL;

                break;

            case RT3DPOINT:

                /* Put the X in the link that already exists: */
                pchain->resval.rreal=sour->resval.rpoint[0];
                pchain->restype=RTREAL;

                /* Make the Y and Z links: */
                for (fi1=1; fi1<3; fi1++) {
                    if ((pchain->rbnext=db_alloc_resbuf())==NULL) { rc=-2; goto out; }
                    pchain=pchain->rbnext;
                    pchain->resval.rreal=sour->resval.rpoint[fi1];
                    pchain->restype=RTREAL;
                }

                break;

            case RTSHORT:

                /* Short (but WE use RTLONG for integer vals) */

                pchain->resval.rlong=(long)sour->resval.rint;
                pchain->restype=RTLONG;

                break;

            case RTSTR:

                if (sour->resval.rstring == NULL)
				{
					pchain->resval.rstring = NULL;
				}
				else
				{
					if ((pchain->resval.rstring= new char [ strlen(sour->resval.rstring)+1])==NULL) { rc=-2; goto out; }
					strcpy(pchain->resval.rstring,sour->resval.rstring);
				}
				pchain->restype=RTSTR;

                break;

            case RTENAME:

                ic_encpy(pchain->resval.rlname,sour->resval.rlname);
                pchain->restype=RTENAME;

                break;

            case RTVOID:  /* Binary chunk */
			case RTBINARY: // EBATECH(CNBR) 2002/7/3 Bugzilla78240 ACA_PROXY_OBJECT cannot show entity list

                /* Make a string of the hex char pairs: */

                if ((bclen=sour->resval.rbinary.clen)<0) bclen=0;
                else if (bclen>127) bclen=127;

                if ((pchain->resval.rstring= new char [2*bclen+1])==NULL)
                    { rc=-2; goto out; }
                pchain->restype=RTSTR;

                for (fi1=fi2=0; fi1<bclen; fi1++,fi2+=2) {
                    fc1=(char)(((unsigned char)(sour->resval.rbinary.buf[fi1]&
                        0xF0))>>4);
                    pchain->resval.rstring[fi2]=fc1+((fc1>9) ? 'A'/*DNT*/-10 : '0'/*DNT*/);
                    fc1=(sour->resval.rbinary.buf[fi1]&0x0F);
                    pchain->resval.rstring[fi2+1]=fc1+((fc1>9) ? 'A'/*DNT*/-10 : '0'/*DNT*/);
                }
                pchain->resval.rstring[fi2]=0;

                break;

            case RTLONG:

                pchain->resval.rreal=(sds_real)sour->resval.rlong;
                pchain->restype=RTREAL;

                break;

            default:  /* Unrecognized */

                rc=-1; goto out;

        }

        if (pchain->restype==RTNONE) { rc=-1; goto out; }

        /* End with RTLE link: */
        if ((pchain->rbnext=db_alloc_resbuf())==NULL) { rc=-2; goto out; }
        pchain=pchain->rbnext;
        pchain->restype=RTLE;
        pchain->resval.rstring=NULL;
        pchain=chain;

    } else {  /* LISP to ADS */

        /* In this block, chain will be 1 link. */

        thislink=sour;

        /* Make sure this member is a list: */
        if (thislink->restype!=RTLB) { rc=-1; goto out; }

        /* Find end of this member: */
        if ((xend=lsp_argend(thislink,NULL))==NULL) { rc=-1; goto out; }

        /* Step to DXF code link and get the code: */
        if ((thislink=thislink->rbnext)==NULL) { rc=-1; goto out; }
        if      (thislink->restype==RTSHORT)
            dxf=thislink->resval.rint;
        else if (thislink->restype==RTLONG)
            dxf=(int)thislink->resval.rlong;
        else { rc=-1; goto out; }
        if (dxf<-5 || dxf>1999) { rc=-1; goto out; }
        if ((rv=ic_resval(dxf))==RTERROR) { rc=-1; goto out; }

        if (dxf==-3) { rc=-1; goto out; }  /* EED sentinel not allowed here */

        /* Step past the DXF code: */
        if ((thislink=thislink->rbnext)==NULL) { rc=-1; goto out; }

        /* If there should be a dot, check for it and step over it: */
        if (dxf!=-3 && rv!=RT3DPOINT) {
            if (thislink->restype!=RTDOTE ||
                (thislink=thislink->rbnext)==NULL) { rc=-1; goto out; }
        }

        /* Free old chain (precaution) and make the first link: */
        if (chain!=NULL) { lsp_freesuprb(chain); chain=pchain=NULL; }
        if ((chain=db_alloc_resbuf())==
            NULL) { rc=-2; goto out; }
        chain->restype=RTNONE;  /* So we can tell if it gets set below */
        pchain=chain;  /* Good form; not used presently (since only 1 link). */


                     /* DEAL WITH EACH DXF TYPE: */

        switch (rv) {

            case RTSTR:

                if (thislink->restype!=RTSTR ||
                    thislink->resval.rstring==NULL) { rc=-1; goto out; }
                if ((chain->resval.rstring= new char [ strlen(thislink->resval.rstring)+1] )==NULL)
                        { rc=-2; goto out; }
                strcpy(chain->resval.rstring,thislink->resval.rstring);
                chain->restype=dxf;

                break;

            case RTENAME:

                if (thislink->restype!=RTENAME &&
                    thislink->restype!=RTPICKS) { rc=-1; goto out; }
                ic_encpy(chain->resval.rlname,thislink->resval.rlname);
                chain->restype=dxf;

                break;

            case RTVOID:  /* Binary chunk */
			case RTBINARY: // EBATECH(CNBR) 2002/7/3 Bugzilla78240 ACA_PROXY_OBJECT cannot show entity list

                if (thislink->restype!=RTSTR ||
                    thislink->resval.rstring==NULL) { rc=-1; goto out; }

                /* It's a string of hex char pairs. */

                if ((bclen=strlen(thislink->resval.rstring)/2)>127) bclen=127;

                if ((chain->resval.rbinary.buf= new char [bclen])==NULL)
                    { rc=-2; goto out; }
                chain->resval.rbinary.clen=bclen;

                for (fi1=fi2=0; fi1<bclen; fi1++,fi2+=2) {
                    fc1=thislink->resval.rstring[fi2];
                    fc1=toupper(fc1);
                    if (!isxdigit((unsigned char) fc1)) fc1='0'/*DNT*/;  /* Precaution */
                    chain->resval.rbinary.buf[fi1]=
                        (fc1-((fc1>='0'/*DNT*/ && fc1<='9'/*DNT*/) ? '0'/*DNT*/ : 'A'/*DNT*/-10))<<4;
                    fc1=thislink->resval.rstring[fi2+1];
                    fc1=toupper(fc1);
                    if (!isxdigit((unsigned char) fc1)) fc1='0'/*DNT*/;  /* Precaution */
                    chain->resval.rbinary.buf[fi1]+=
                        fc1-((fc1>='0'/*DNT*/ && fc1<='9'/*DNT*/) ? '0'/*DNT*/ : 'A'/*DNT*/-10);
                }

                chain->restype=dxf;

                break;

            case RT3DPOINT:

                /* Build a temp list for lsp_convptlist() to convert: */
                if (tlist!=NULL) { lsp_freesuprb(tlist); tlist=NULL; }
                if ((tlist=db_alloc_resbuf())==
                    NULL) { rc=-2; goto out; }
                tlist->restype=RTLB;
                tlist->resval.rstring=NULL;
                if ((tlist->rbnext=db_alloc_resbuf())==NULL) { rc=-2; goto out; }
                tp1=xend->rbnext; xend->rbnext=NULL;
                fi1=lsp_copysplrb(tlist->rbnext,thislink);
                xend->rbnext=tp1;
                if (fi1) { rc=-2; goto out; }
                lsp_convptlist(tlist,0);
                if (tlist->restype!=RT3DPOINT && tlist->restype!=RTPOINT)
                    { rc=-1; goto out; }
                ic_ptcpy(chain->resval.rpoint,tlist->resval.rpoint);
                chain->restype=dxf;

                break;

            case RTREAL:

                /* Accept ints, too, but make reals out of them. */

                if (thislink->restype==RTREAL ||
                    thislink->restype==RTANG ||
                    thislink->restype==RTORINT)
                        chain->resval.rreal=thislink->resval.rreal;
                else if (thislink->restype==RTLONG)
                    chain->resval.rreal=(sds_real)thislink->resval.rlong;
                else if (thislink->restype==RTSHORT)
                    chain->resval.rreal=(sds_real)thislink->resval.rint;
                else { rc=-1; goto out; }

                chain->restype=dxf;

                break;

            case RTLONG:

                /* Accept any numeric type. */

                if (thislink->restype==RTREAL ||
                    thislink->restype==RTANG ||
                    thislink->restype==RTORINT)
                        chain->resval.rlong=(long)thislink->resval.rreal;
                else if (thislink->restype==RTLONG)
                    chain->resval.rlong=thislink->resval.rlong;
                else if (thislink->restype==RTSHORT)
                    chain->resval.rlong=(long)thislink->resval.rint;
                else { rc=-1; goto out; }

                chain->restype=dxf;

                break;

            case RTSHORT:

                /* Accept any numeric type. */

                if (thislink->restype==RTREAL ||
                    thislink->restype==RTANG ||
                    thislink->restype==RTORINT)
                        chain->resval.rint=(int)thislink->resval.rreal;
                else if (thislink->restype==RTLONG)
                    chain->resval.rint=(int)thislink->resval.rlong;
                else if (thislink->restype==RTSHORT)
                    chain->resval.rint=thislink->resval.rint;
                else { rc=-1; goto out; }

                chain->restype=dxf;

                break;

            default:  /* Unrecognized */

                rc=-1; goto out;

        }

        if (chain->restype==RTNONE) { rc=-1; goto out; }

    }  /* End else LISP to ADS */


out:
    if (rc && chain!=NULL) { lsp_freesuprb(chain); chain=pchain=NULL; }
    if (tlist!=NULL) { lsp_freesuprb(tlist); tlist=NULL; }

    if (dest!=NULL) *dest=chain;

    return rc;
}

int lsp_fnfromarg(struct resbuf *arg, int (**fnpp)(void),
    struct resbuf **listfnpp, int *freeitp, int *ierrp, class commandAtomObj **alink) {
/*
**  THIS FUNCTION IS AN EXTENSION OF lsp_lispev1().
**
**  Given a LISP arg (which may be a list), this function finds
**  the function to execute by evaluating arg.  The result may be either
**  a function pointer (LSP_RTSUBR or LSP_RTXSUB) or a list (RTLB).
**
**  Normally, I would just call lsp_evalarg(arg) and pick up the
**  result in lsp_evres and save a copy (because lsp_evres soon gets
**  reused).  But the result may be a long list that will take time and
**  memory to copy.  So, let's only copy when we have to.  There are 3
**  valid possibilities:
**
**      (1) The arg is a function pointer (LSP_RTSUBR or LSP_RTXSUB).
**          (The eval function makes this case possible.)
**          Save it in *fnpp.
**
**      (2) The arg is a symbol (LSP_RTSYMB).
**          FIND it in the atoms list (don't evaluate it).
**          If its val is a function pointer, set *fnpp to it.
**          If its val is a list, POINT *listfnpp at it (and flag that
**          we don't dare free it with *freeitp=0).
**
**      (3) The arg is a list (RTLB).
**          We have to evaluate it (which puts it in lsp_evres).
**          If its val is a function pointer, set *fnpp to it.
**          If its val is a list, MAKE A COPY and point *listfnpp at it
**          (and flag that we DO have to free it with *freeitp=1).
**
**  A successful call will set either *fnpp or *listfnpp BUT THE CALLER
**  SHOULD CHECK BOTH FOR NULL EVEN UPON SUCCESS.
**
**  Here are the parameters:
**
**      arg : resbuf llist containing the arg to evaluate to determine
**            the function pointer or list-function.
**
**      fnpp : Pointer to an internal LISP function pointer;
**             *fnpp is set if arg evals to a function pointer.
**
**      listfnpp : Pointer to a resbuf pointer llist that defines
**                 the list-function; *lsitfnpp is set if arg evals
**                 to a list-function.  (MAY NEED FREEING.  See freeitp.)
**
**      freeitp : Pointer to a int flag that tells whether or not
**                *listfnpp should be freed by the caller in the end.
**                (It WILL if we alloc it here; it WON'T if we simply
**                point it at a value in the atoms list.)
**
**      ierrp : Pointer to a int flag that is set to 1 if
**              the recursion call to lsp_lispev1() returned the error
**              that lsp_fnfromarg() returns (meaning an inner expression
**              error occurred).
**
**      alink : Pointer to an atomlink. This is a return pointer used by
**              the functions that use the thread infomation that is in
**              the atom link struct.
**
**  This function returns a lsp_lispev1() return code -- or -100 if
**  there is a calling error (if any of the args are NULL).
*/
    int freeit,ierr,rc;
    int (*fnp)(void);
    struct resbuf *listfnp,*trbp1,*trbp2;
    class commandAtomObj *tap1;
    TCHAR tmp[SDS_TILE_STR_LIMIT];


    rc=freeit=ierr=0; fnp=LSP_FNPCAST NULL; listfnp=NULL;

    if (arg==NULL || fnpp==NULL || listfnpp==NULL || freeitp==NULL ||
        ierrp==NULL) { rc=-100; goto out; }

    /* Init the stuff for the caller: */
    *fnpp=LSP_FNPCAST NULL; *listfnpp=NULL; *freeitp=0;
    *ierrp=0;

    /* Use locals, then set the vals for the caller all at once */
    /* when we get to the bottom without any errors. */


    if (arg->restype==LSP_RTSUBR || arg->restype==LSP_RTXSUB) {
        fnp=LSP_FNPCAST (arg->resval.rstring);
    } else if (arg->restype==LSP_RTSYMB) {
        bool isLisp = FALSE;        // Will be true id it's a (defun C:??()....) type w/ C:
        tap1=lsp_findatom(arg->resval.rstring,0);
        if (tap1 == NULL)
            {
            // Give it one more try looking for internal version that may/may not have
            // been re-defun'd.
            sprintf(tmp, "L:%s"/*DNT*/, arg->resval.rstring);
            tap1 = lsp_findatom(LPCTSTR(tmp),0);
            if (tap1 == NULL) {rc=-2;goto out;}
            }

        if (tap1 != NULL)
            isLisp = _tcsnicmp(arg->resval.rstring, "C:"/*DNT*/, 2) == 0;

        // Matched an internal command line command, but are looking for a LISP expression.
        // Add L: to it and try again.
        if (tap1->id == -3 && !isLisp) {    // One of those ENGLISH only dual LISP and command line functions.
                                            // like LOAD and (LOAD ...);
            sprintf(tmp, "L:%s"/*DNT*/, arg->resval.rstring);
            tap1 = lsp_findatom(LPCTSTR(tmp),0);

            if (tap1 == NULL) {
                rc=-2;
                goto out;
            }
        }

        if (tap1->rb.restype==LSP_RTSUBR || tap1->rb.restype==LSP_RTXSUB) {
            fnp=LSP_FNPCAST (tap1->rb.resval.rstring);
            *alink=tap1;
            if (tap1->id>0)
                lsp_lastufncd=tap1->id;  /* Save id # of last */
                                         /* user-def'd fn called */
        } else if (tap1->rb.restype==RTLB) {

            /*
            **  Here's the old way (that had trouble with recursion
            **  once we started stealing lsp_argsll from xll in
            **  lsp_lispev1()); it used the function def straight
            **  from the atoms llist.  Trouble is, an earlier
            **  recursion still has it cut up into pieces at this point:
            **
            **      listfnp=&tap1->rb; freeit=0;
            **
            **  Until we figure a better way, we'll have to make a
            **  disposable copy:
            */

            if (listfnp!=NULL) { lsp_freesuprb(listfnp); listfnp=NULL; }
            if ((listfnp=db_alloc_resbuf())==NULL ||
                lsp_copysplrb(listfnp,&tap1->rb)) { rc=-3; goto out; }
            freeit=1;

        } else { rc=-5; goto out; }
    } else if (arg->restype==RTLB) {

        /* Find the end (RTLE): */
        if ((trbp1=lsp_argend(arg,NULL))==NULL) { rc=-1; goto out; }

        /* Chop, eval, repair: */
        trbp2=trbp1->rbnext; trbp1->rbnext=NULL;
        /* Evaluate.  (Recursion; use NULL for respp.): */
        rc=lsp_lispev1(arg,NULL,lsp_fnvarval);
        trbp1->rbnext=trbp2;
        if (rc) { ierr=1; goto out; }

        /* The result is now in lsp_evres. */
        /* Save it, since lsp_evres will be reused by the caller: */
        if (lsp_evres.restype==LSP_RTSUBR || lsp_evres.restype==LSP_RTXSUB) {
            fnp=LSP_FNPCAST (lsp_evres.resval.rstring);
        } else if (lsp_evres.restype==RTLB) {

            freeit=1;

            /* Make the copy: */
            if ((listfnp=db_alloc_resbuf())==NULL ||
                lsp_copysplrb(listfnp,&lsp_evres)) { rc=-3; goto out; }
        } else { rc=-5; goto out; }

        /*  Free lsp_evres again; set to NIL: */
        lsp_freesplrb(&lsp_evres);

    } else if (arg->restype==RTNIL) {
        rc=-2; goto out;
    } else { rc=-5; goto out; }

    /* Success.  Set all the values for the caller (except *ierrp */
    /* which must be set even upon error): */
    *fnpp=fnp; *listfnpp=listfnp; *freeitp=freeit;
    listfnp=NULL; freeit=0;  /* It's been handed to the caller; */
                             /* don't let "out" free it now. */

out:
    if (ierrp!=NULL) *ierrp=ierr;

    /* If listfnp got alloc'd but wasn't handed to caller, */
    /* something went wrong, so free it (if freeit allows it): */
    if (freeit && listfnp!=NULL) { lsp_freesuprb(listfnp); listfnp=NULL; }

    return rc;
}

struct lsp_strlink *lsp_findstrlink(char *str, struct lsp_strlink *sll,
    struct lsp_strlink **beforepp) {
/*
**  Searches llist sll for str and returns a pointer to the link
**  where it is found.
**
**  When beforepp!=NULL, *beforepp is set to the link just before
**  the link that contains str -- or to the last link if it is never
**  found.  If str was found in the first link, *beforepp will be NULL.
*/
    struct lsp_strlink *bef;

    if (beforepp!=NULL) *beforepp=NULL;
    if (str==NULL) return NULL;

    for (bef=NULL; sll!=NULL && stricmp(sll->str,str); bef=sll,sll=sll->next);

    if (beforepp!=NULL) *beforepp=bef;
    return sll;
}

void lsp_spaceit(int howmany) {
/*
**  sds_printf() has a limit to how big a string it can print per call.
**  The TRACE indenting can result in LOTS of spaces to print,
**  so we have to break it up into safe increments (or do a for
**  loop that prints 1 space at a time, which would be slow).
**
**  This function prints howmany spaces in 100-space increments.
*/
    char *buf;

    buf=NULL;

    if (howmany<=0) goto out;
    if ((buf= new char [101])==NULL) goto out;  /* Oh, well. */
    memset(buf,' '/*DNT*/,100); buf[100]=0;
    while (howmany>100) { sds_printf(buf); howmany-=100; }
    if (howmany>0) { buf[howmany]=0; sds_printf(buf); buf[howmany]=' '/*DNT*/; }

out:
    if (buf!=NULL) { IC_FREE(buf); buf=NULL; }
}

int
lsp_prepatomname
(char*	dest,
 char*	sour,
 int	maxlen,
 int	upper
)
{
//  Copies sour to dest while compressing out spaces.  The maximum
//  length of dest is maxlen.  If upper!=0, the chars are toupper'd
//  in dest.
//
//  Returns:
//       0 : OK
//      -1 : sour and/or dest is NULL, or dest ends up as "".

/*D.G.*/// Enabled for DBCS.

    if(!sour || !dest)
		return -1;

	int		nDestLen = 0;
	char	*pSour = sour, *pDest = dest;

	for( ;nDestLen < maxlen; )
	{
		while(*pSour && isspace((unsigned char)*pSour))
			++pSour;
		if(!*pSour)
			break;

		*pDest++ = *pSour++;
		++nDestLen;
		if(nDestLen == maxlen)
		{
			if(_tclen(pSour-1) == 2)
				*pDest-- = '\0';
			break;
		}
		if(_tclen(pSour-1) == 2)
		{
			*pDest++ = *pSour++;
			++nDestLen;
		}
	}

	*pDest = '\0';

	if(upper)
		_tcsupr(dest);

    return -(int)(!*dest);
}



int lsp_command(void) {
    int msgn,ok,fi1;
	//Modified Cybage AW 30-07-01
	//Reason : In LISP program LONG coordinates are treated as short ints.
	//BugNo : 77816 from Bugzilla
    //long fl1;
    struct resbuf *argsll,*forcmd,*last,*thisarg,*newlinks,*tp1;

    msgn=-1;
	argsll=forcmd=last=newlinks=NULL;


    /* This is a donteval function (see lsp_lispev1()). */
    /* Steal lsp_argsll for local use (and don't free it): */
    argsll=lsp_argsll;
	lsp_argsll=NULL;

    /* Deal with a "cancel" right now: */
    if (argsll==NULL)
		{
		sds_cmd(NULL);
		goto out;
		}

    /*
    **  Process the args individually and call sds_cmd() when
    **  necessary. Here's the problem.  Function calls like (entlast)
    **  and (osnap...) expect the command chain to have been fully
    **  processed to that point.  To handle this, every time we see a
    **  function call, we need to send whatever we've collected so far
    **  to sds_cmd(), and THEN evaluate the function and use it to
    **  begin a new chain of resbufs for the next sds_cmd() call.
    */
    for (thisarg=argsll; /* No test */; thisarg=lsp_nextarg(thisarg)) {

        if ((thisarg==NULL || thisarg->restype==RTLB || thisarg->restype==RTNIL) && forcmd!=NULL) {

            /* Check for disallowed codes and change RTLONGs to RTSHORTs */
            /* (sds_cmd() can hang with super resbuf codes, for example): */
            for (tp1=forcmd,ok=1; tp1!=NULL && ok; tp1=tp1->rbnext) {
                switch (tp1->restype) {
                    case RTSHORT: case RTREAL:  case RTANG: case RTORINT: case RTSTR:
                    case RTENAME: case RTPICKS: case RTLB:  case RTLE:
						break; /* These are okay as they are (no RTpts yet). */
					case RTNIL:
                        tp1->restype=RTCAN;
						//if(tp1==forcmd) sds_cmd(NULL);
						//else ok=0;
                        break;
                    case RTLONG:  /* Convert to RTSHORT for sds_cmd(). */
						//Modified Cybage AW 30-07-01 [
						//Reason : In LISP program LONG coordinates are treated as short ints.
						//BugNo : 77816 from Bugzilla
                        //fl1=tp1->resval.rlong; /* Don't try intra-union assignment. */
                        //tp1->restype=RTSHORT;
                        //tp1->resval.rint=(int)fl1;
						//Modified Cybage AW 30-07-01 ]
                        break;
                    default: ok=0; break;
                }
            }
            if (!ok)
				{
				msgn=41;
				goto out;
				}

            /* Convert pts from list-form to pt-form: */
            lsp_convptlist(forcmd,0);

            lsp_cmdhasmore=(thisarg!=NULL);

            /* Send whatever we have so far to sds_cmd(). */
            if ((fi1=sds_cmd(forcmd))!=RTNORM)
				{
				if ( fi1 == RTCAN )
					{
					msgn = 18;
					}
				else
					{
					msgn = 41;
					}
                goto out;
	            }

            lsp_freesuprb(forcmd);
			forcmd=last=NULL;  /* Start over. */
        }

        if (thisarg==NULL) break;  /* Done */

        /* Eval current arg: */
        if ((msgn=lsp_evalarg(thisarg,NULL))!=-1)
			{
			goto out;
			}

        /* Add the result to forcmd: */
        if (newlinks!=NULL) { lsp_freesuprb(newlinks); newlinks=NULL; }
        if ((newlinks=db_alloc_resbuf())==NULL ||
            lsp_copysplrb(newlinks,&lsp_evres))
			{
			msgn=7;
			goto out;
			}
        if (forcmd==NULL)
			{
			forcmd=newlinks;
			}
		else
			{
			last->rbnext=newlinks;
			}
        last=newlinks;
		while (last->rbnext!=NULL)
			{
			last=last->rbnext;
			}
        newlinks=NULL;
    }

    lsp_retnil();

out:
    lsp_cmdhasmore=0;  /* For safety */
    if (forcmd  !=NULL) lsp_freesuprb(forcmd);
    if (newlinks!=NULL) lsp_freesuprb(newlinks);
    if (msgn!=-1) {  /* May get -2 from lsp_evalarg(). */
        if (msgn>-1) lsp_fnerrmsg(msgn);
        return -1;
    }
    return 0;
}


void lsp_fnerrmsg(int code) {
/*
**  Error messages for the lisp function we're trying to evaluate.
*/
    class commandAtomObj *erratom;
    CString msg;
    static int recur;

    /* Guard against infinite recursion (it must be an error in the */
    /* *ERROR* function itself): */
    if (recur) return;  /* Don't goto out this time! */

    recur=1;

    /* Look for *ERROR* set to a list-function: */
    if ((erratom=lsp_findatom("*ERROR*"/*DNT*/,0))!=NULL &&
         erratom->rb.restype==RTLB &&
         erratom->rb.rbnext!=NULL &&
        (erratom->rb.rbnext->restype==RTLB ||
         erratom->rb.rbnext->restype==RTNIL)) {

        if (code<0 || code>= IDC_LISP4_ERRMSG_END - IDC_LISP4_ERRMSG_START) goto out;
        msg.LoadString(IDC_LISP4_ERRMSG_START + code);

        /* Fix lsp_argsll so it has the message: */
        lsp_freesuprb(lsp_argsll);  /* Should be okay to free it now. */
        if ((lsp_argsll=db_alloc_resbuf())==
            NULL) goto out;  /* Oh, well. */
        lsp_argsll->restype=RTSTR;
        if ((lsp_argsll->resval.rstring= new char [ msg.GetLength() + 1])==NULL) goto out;
        strcpy(lsp_argsll->resval.rstring,LPCTSTR(msg));

        /* Call the list-function: */
        lsp_dolistfn(&erratom->rb,NULL);

    } else {

		if ( !g_lsp_bProtectedLisp )
			{
	        lsp_errtrace=1;
			}
        if (code>-1 && code<IDC_LISP4_ERRMSG_END - IDC_LISP4_ERRMSG_START) {
            msg.LoadString(IDC_LISP4_ERRMSG_START + code);
            CString formatStr;
            formatStr.LoadString(IDC_LISP4_ERROR);
            char tmpFormatStr[255];
            strcpy(tmpFormatStr, LPCTSTR(formatStr));
            sds_printf(tmpFormatStr,LPCTSTR(msg));
        }
    }

	if(lsp_cb_recurs) {
		lsp_term_dialog();
		lsp_cb_recurs=0;
	}

out:
    recur=0;
}



//TODO: function unfinished and needs work
int lsp_setfunhelp(void) 
{
	
	if (lsp_argsll->resval.rstring && (!_tcsnicmp(lsp_argsll->resval.rstring, "C:"/*DNT*/, 2) == 0))
	{
		cmd_ErrorPrompt(IDC_LISP4_ERRMSG_10, -1);
		sds_printf(": \"%s\"", lsp_argsll->resval.rstring);
		return RTERROR;
	}

	/// Modified the Function By Sachin Dange 
	// To solve the relative cases of Bugzilla 78663 .............[

	class commandAtomObj * pCommandAtomObj = NULL;
	struct resbuf *file=NULL;
	struct resbuf *topic=NULL;
	struct resbuf *command=NULL;
	pCommandAtomObj = lsp_findatom(lsp_argsll->resval.rstring,4);
	if(pCommandAtomObj)
	{
		file = lsp_argsll->rbnext;
		if (file)
			topic = file->rbnext;
		if (topic)
			command = topic->rbnext;
		if(file)
		{
			pCommandAtomObj->m_szHelpFile = new char[strlen(file->resval.rstring) + 1];
			strcpy(pCommandAtomObj->m_szHelpFile,file->resval.rstring);
		}
		if(topic)
		{
			pCommandAtomObj->m_szContextID = new char[strlen(topic->resval.rstring) + 1];
			strcpy(pCommandAtomObj->m_szContextID,topic->resval.rstring);
		}

		if (command)
		{
			if (command->restype EQ RTSTR)
			{
				if(strisame( command->resval.rstring,"HELP_CONTENTS"))
					pCommandAtomObj->m_nMapNumber = HELP_CONTENTS;
				else if(strisame( command->resval.rstring,"HELP_FINDER"))
					pCommandAtomObj->m_nMapNumber = HELP_FINDER;
				else if(strisame( command->resval.rstring,"HELP_HELPONHELP"))
					pCommandAtomObj->m_nMapNumber = HELP_HELPONHELP;
				else if(strisame( command->resval.rstring,"HELP_PARTIALKEY"))
					pCommandAtomObj->m_nMapNumber = HELP_PARTIALKEY;
			}

			else
			{
				ASSERT( command->restype EQ RTSHORT );
				pCommandAtomObj->m_nMapNumber = command->resval.rint;
			}
		}
	}
	// .....................] Modifications end here.

	if ( lsp_evres.resval.rstring)
		_tcscpy((char *)lsp_evres.resval.rstring, (char *)lsp_argsll->resval.rstring);
	else
	{
		cmd_ErrorPrompt(IDC_LISP4_ERRMSG_1, -1);
		return RTERROR;
	}
	return RTNORM;
}

int lsp_help(void)
{
	char hlp[IC_ACADBUF];
	char empty[]="";
	struct resbuf *file=NULL;
	struct resbuf *topic=NULL;
	struct resbuf *command=NULL;

								// locate file/topic/command arguments
	file = lsp_argsll;
	if ( file )
		topic = file->rbnext;
	if ( topic )
		command = topic->rbnext;

								// error: too many arguments
	if ( command && command->rbnext )
		{
		lsp_fnerrmsg( 1);
		return -1;
		}

								// check argument types
	if (
			(!file || file->restype EQ RTSTR )
			&& (!topic || topic->restype EQ RTSTR)
			&& (
				!command
				|| command->restype EQ RTSTR
				|| command->restype EQ RTSHORT
				)
			)
		{
								// use default helpfile if blank
		LPCTSTR	filename = "";
		if ( file )
			filename = file->resval.rstring;

								// use default command if not specified
		int		commandValue = 0;
		LPTSTR	topicString = "";
		if ( topic )
			topicString = topic->resval.rstring;

								// if no topic, change default command
		if strisame( topicString, "" )
			commandValue = ICADHELP_PARTIALKEY;

								// interpret string if specified
		if ( command )
			{
			if ( command->restype EQ RTSTR )
				{
				if(strisame( command->resval.rstring,"HELP_CONTENTS"))
					commandValue = ICADHELP_CONTENTS;
				else if(strisame( command->resval.rstring,"HELP_FINDER"))
					commandValue = ICADHELP_FINDER;
				else if(strisame( command->resval.rstring,"HELP_HELPONHELP"))
					commandValue = ICADHELP_HELPONHELP;
				else if(strisame( command->resval.rstring,"HELP_PARTIALKEY"))
					commandValue = ICADHELP_PARTIALKEY;
				}

			else
				{
				ASSERT( command->restype EQ RTSHORT );
				commandValue = command->resval.rint;
				}
			}

								// locate help file and issue command
		if(
				strisame ( filename, "") && SearchHelpFile( SDS_HLPFILE, hlp)
				|| SearchHelpFile( filename, hlp)
				)
			if ( sds_help( hlp, topicString, commandValue) EQ RTNORM )
				{
				lsp_retstr( filename);
				return 0;
				}
		}

								// invalid arguments
    lsp_retnil();
	return 0;
	}

int lsp_exit(void) { lsp_fnerrmsg(39); return -1; }
int lsp_quit(void) { lsp_fnerrmsg(39); return -1; }


