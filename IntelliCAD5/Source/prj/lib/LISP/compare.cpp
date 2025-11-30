/* C:\ICAD\PRJ\LIB\LISP\COMPARE.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/
// Added Cybage AJK 15/10/2001 [
//  Reason: Fix for Bug No: 77845 from BugZilla 
#include "bindtable.h"	
extern CBindStructure bindTable;
extern  struct resbuf *args;
//Added Cybage AJK 15/10/2001 ]
int lsp_eq(void)    { return lsp_equalstuff(0); }
int lsp_equal(void) { return lsp_equalstuff(1); }

int lsp_equalstuff(int op) {
/*
**  This function performs the LISP eq and equal functions.
**
**  Equal is simple because we compare 2 args to see if they
**  EVALUATE to the same thing.
**
**  The rules for eq are a little weird.  From the documentation
**  for eq and equal in the AutoLISP manual, eq will call 2
**  atoms equal if they evaluate to the same thing -- but lists are
**  only equal if they are bound to the same thing.  For example,
**  for (setq a '(1 2) b '(1 2) c a), eq says that a and c
**  are equal, but not "a" and "b".  This implies a list of symbols
**  that point into a list of values -- with some symbols pointing
**  to the same values.  We don't have that kind of data structure.
**  Our atoms list has 3 copies of the list after the above
**  statement.
**
**  Therefore, for OUR eq, if the 2 args EVALUATE to the same thing,
**  we return RTT; otherwise, we return RTNIL.  (If something is REALLY
**  testing the binding for lists, it will always appear as if 2 lists
**  of equal value are bound to the same thing.)
*/
    int msgn;
    sds_real fuzz;
    struct resbuf *argp[3];

    msgn=-1; fuzz=0.0;

    if ((argp[0]=lsp_argsll)==NULL ||
        (argp[1]=lsp_nextarg(argp[0]))==NULL) { msgn=2; goto out; }
    if ((argp[2]=lsp_nextarg(argp[1]))!=NULL) {
        if (!op || lsp_nextarg(argp[2])!=NULL) { msgn=9; goto out; }
        if(argp[2]->restype==RTREAL || argp[2]->restype==RTORINT || argp[2]->restype==RTANG)
            fuzz=argp[2]->resval.rreal;
        else if (argp[2]->restype==RTLONG)  fuzz=(sds_real)argp[2]->resval.rlong;
        else if (argp[2]->restype==RTSHORT) fuzz=(sds_real)argp[2]->resval.rint;
        else { msgn=1; goto out; }
    }
	// Added Cybage AJK 15/10/2001 [
   //Reason: Fix for Bug No: 77845 from BugZilla 

    if(op==0)//its an eq call
	{
		if(!bindTable.isBinded (args->rbnext->rbnext->resval.rstring,args->rbnext->rbnext->rbnext->resval.rstring)&& argp[0]->restype==SDS_RTLB)//5016
		{
			lsp_retnil();
			goto out;
		}
	}
  //Added Cybage AJK 15/10/2001  DD/MM/YYYY ]
  if (lsp_argcomp(argp[0],argp[1],fuzz)) lsp_retnil(); else lsp_rett();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}


/*
**                      THE COMPARISON FUNCTIONS
**
**  These were hard to figure out at first.
**
**  Here are the apparent rules (empirically obtained from AutoLISP):
**
**      Unassigned symbol is identical to NIL.
**      NIL < Anything but NIL.
**      NIL can be compared with anything (but T can't).
**      Any single-argument comparison is always T (not too few arguments)
**      Integer and real numerics can be mixed in comparisons.
**      = and /= never generate bad arg type errors.
**      Other than numeric vs numeric and nil vs anything, type mismatches
**        generate bad arg type with range comparisons and generate
**        not equal with = and /=.
**      For all types other than numeric, string, and nil, even
**        when the types DO match, bad arg type results for range
**        comparisons when they are NOT equal.  The expected results
**        occur when they ARE equal:
**      Lists (RTLB) and enames (RTENAME) seem to follow eq logic
**        for LISTS (even for points). That is, given (setq a '(1 2 3) b
**        '(1 2 3) c a), only a and c are equal (because they point to
**        the same actual lists in AutoLISP).  We don't have that
**        concept, so ALL lists and ALL enames compare as not equal
**        for us.  Range comparisons for unequal lists and enames give
**        bad arg type. (Note that eq itself has no such problem with
**        enames; only the comparison operators do!)
*/


int lsp_compare(int op) {
/*
**  Compares the items in lsp_argsll using operator op and sets
**  lsp_evres to either T or NIL.
**
**  Operator op is bit-coded to specify the type of comparison:
**
**               NGEL
**      <  : 00000001 (1)
**      =  : 00000010 (2)
**      <= : 00000011 (3)
**      >  : 00000100 (4)
**      >= : 00000110 (6)
**      /= : 00001000 (8)
**
**  Note that = and /= (eqop==1) do not fail with bad arg type
**  when the types don't match like the range comparisons do.
**
**  This function walks through lsp_argsll and conpares all adjacent
**  pairs of args.  If all comparisons are true, the result is T.
**  When any pair fails, execution bails out of the loop and
**  the result is NIL.
**
**  /= is undefined for more than 2 args according to the AutoLISP
**  manual, so we just walk through the list as in all other cases.
**  The result is that if 2 adjacent args are equal, the /= result is NIL;
**  otherwise, T.  That's not what AutoLISP does, but I haven't
**  discovered what they're doing.  Whatever it is, it would probably
**  not fit within the pattern of this function.
**
**  See comment above this function for more details.
**
**  Returns:
**      -1 : Error occurred and message was printed
**       0 : Result was set in lsp_evres
*/
    int res,numeric1,numeric2,valcomp,loop1,eqop,msgn;
    int fint1;
    sds_real val1,val2;
    struct resbuf *arg1p,*arg2p;

    msgn=-1; val1=val2=0.0; numeric1=numeric2=0;
    res=1;  /* Default to T for 1-arg results */

    eqop=(op==2 || op==8);  /* = or /= */

    if (lsp_argsll==NULL) { msgn=2; goto out; }
    if (lsp_nextarg(lsp_argsll)==NULL) goto out;  /* 1 arg means T */

    arg1p=NULL; arg2p=lsp_argsll; loop1=1;
    for (;;) {

        if (!loop1) {
            arg1p=arg2p; numeric1=numeric2; val1=val2;
            if ((arg2p=lsp_nextarg(arg2p))==NULL) break;
        }

        if (arg2p->restype==RTREAL || arg2p->restype==RTANG ||
            arg2p->restype==RTORINT) {
            numeric2=1; val2=arg2p->resval.rreal;
        } else if (arg2p->restype==RTLONG) {
            numeric2=1; val2=(sds_real)arg2p->resval.rlong;
        } else if (arg2p->restype==RTSHORT) {
            numeric2=1; val2=(sds_real)arg2p->resval.rint;
        } else numeric2=0;

        if (loop1) { loop1=0; continue; }


        if (numeric1 && numeric2) {

            if      (val1<val2)  valcomp= 9;  /* LT and NE are true */
            else if (val1==val2) valcomp= 2;
            else                 valcomp=12;  /* GT and NE are true */
            res=((op&valcomp)!=0);

        } else if (arg1p->restype==RTNIL) {

            res=(
                    ((op==1 || op==8) && arg2p->restype!=RTNIL) ||
                    ((op==6 || op==2) && arg2p->restype==RTNIL) ||
                      op==3
                );

        } else if (arg2p->restype==RTNIL) {

            res=(
                    ((op==4 || op==8) && arg1p->restype!=RTNIL) ||
                    ((op==3 || op==2) && arg1p->restype==RTNIL) ||
                      op==6
                );

        } else if (arg1p->restype==arg2p->restype) {  /* Types match */

            if (arg1p->restype==RTSTR) {  /* Range-checking allowed */

                fint1=strcmp(arg1p->resval.rstring,arg2p->resval.rstring);
                if      (fint1<0) valcomp= 9;  /* LT and NE are true */
                else if (!fint1)  valcomp= 2;
                else              valcomp=12;  /* GT and NE are true */
                res=((op&valcomp)!=0);

            } else {

                /* The rest following the logic that unequal args  */
                /* give Bad arg type with range comparisons, */
                /* as described earlier.  (Lists and enames are */
                /* never equal; see the discussion above about */
                /* eq function logic.) */

                if (arg1p->restype==RTLB || arg1p->restype==RTENAME ||
                    lsp_argcomp(arg1p,arg2p,0.0)) {

                    if (!eqop) { msgn=1; goto out; }
                    res=(op==8);
                } else {
                    res=((op&2)!=0);
                }

            }

        } else {  /* Types do not match */

            if (!eqop) { msgn=1; goto out; }
            res=(op==8);

        }

        if (!res) break;
    }


out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    if (res) lsp_rett(); else lsp_retnil();
    return 0;
}

int lsp_compeq(void)  { return lsp_compare(2); }
int lsp_compneq(void) { return lsp_compare(8); }
int lsp_complt(void)  { return lsp_compare(1); }
int lsp_complte(void) { return lsp_compare(3); }
int lsp_compgt(void)  { return lsp_compare(4); }
int lsp_compgte(void) { return lsp_compare(6); }




