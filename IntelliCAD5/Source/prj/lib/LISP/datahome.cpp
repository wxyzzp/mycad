/* C:\ICAD\PRJ\LIB\LISP\DATAHOME.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * A home for the globals
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/
#include "commandatom.h" /*DNT*/
// Added Cybage AJK 15/10/2001 [
// Reason: Fix for Bug No: 77845 from BugZilla 
#include "bindtable.h"	
//added Cybage AJK 15/10/2001 DD/MM/YYYY ]
/*
**  The pending lisp expression (the incomplete fragment not yet
**  executed) and its associated mode.  The mode keeps track of whether
**  an unfinished quote or comment is currently open, so that
**  appropriate action can be taken when the next fragment is received
**  by lsp_lispev().  This is used by lsp_lispev() and must be saved
**  before any recursion calls to that function and restored afterward
**  (like in lsp_load()).
**
**  The freer is lsp_freepend().
*/
struct lsp_pendlink lsp_pend;

/*
**  The llists of operands for the current recursion's function to
**  act upon.  This llist is built by lsp_lispev1() for the function
**  it's going to call.  Therefore, any functions that it calls
**  that call lsp_lispev1() themselves (like lsp_setq and lsp_if and any
**  others that use RTLB lists to delay evaluation until they're
**  ready) MUST make private copies of lsp_argsll for themselves
**  before the recursion calls are made.
**
**  The freer is lsp_freesuprb().
*/
struct resbuf *lsp_argsll;  /* A SUPER RESBUF LLIST -- SEE NOTE ABOVE */


/*
**  lsp_lispev1() results (which MAY be a llist taking off from the
**  next pointer.  Results are placed here
**  by the sds_ret... functions (and by ic_lsp_eval1() itself
**  for its own internal use in building lsp_argsll).
**
**  MAKE SURE THAT ANYTHING THAT SETS A RESULT FREES THE OLD ONE FIRST.
**  MAINLY, THIS MEANS THE sds_ret... FUNCTIONS -- BUT THERE
**  ARE OTHERS (LIKE lsp_lispev1()) THAT DIRECTLY SET lsp_evres.
**
**  The freer is lsp_freesplrb().
*/
struct resbuf lsp_evres;  /* A SPECIAL SUPER RESBUF -- SEE NOTE ABOVE */

/*
**  The llist of all FILE pointers opened by our LISP (open) command
**  that are STILL OPEN.  The freer is in lsp_freeatomsll()
**  (but lsp_close() can remove single links).
*/
struct lsp_fileplink *lsp_filepll;

/*
**  The llist of all selection sets created by our LISP commands
**  that are STILL VALID (have not been freed).
**  The freer is in lsp_freeatomsll().
**  Unreferenced sel sets are freed in lsp_add2ssll() every time
**  a new ss is added.
*/
struct lsp_sslink *lsp_ssll;

/*
**  The following global var is for our sds_getfuncd().  It's the
**  fn id # of the last user-defined function (not predefined)
**  that lsp_lispev1() called (or even thought about calling -- it's
**  captured once the function name has been found, long before
**  the actual call).
*/
int lsp_lastufncd;


/*
**  This global function pointer was created because I needed it in
**  lsp_evalarg() for the recursive call to lsp_lispev1().  It's set
**  at the top of lsp_lispev1() itself, which should be okay since
**  different recursions will always keep the same value in it:
*/

/*
**  The chars disallowed in symbol names.
*/
char *lsp_badsymchars="().\'\";"/*DNT*/;
/*
**  The ic_escconv() conversion strings that determine which escape
**  sequences in strings are converted.  There are 2 of them: one for
**  converting escape sequences to the true chars, and one for converting
**  true chars to their escape sequence equivalents.  The latter
**  does NOT include quote: AutoLISP does not convert ["] to [\]["].
*/
char *lsp_esc2true="\\\\\"\"e\x1Bn\nr\rt\t"/*DNT*/;
char *lsp_true2esc="\\\\e\x1Bn\nr\rt\t"/*DNT*/;
/*
**  A special name for the (prin?) mystery symbol.  This is the
**  ASCII value of the 1-char name we use to identify it
**  (the happy-face).  (Who knows how AutoLISP does it.)
*/
char lsp_mysterysym=1;
sds_real lsp_dmin=1.0E-300;  /* These 2 are used to prevent math overflows. */
//Modified CyberAge AW 22/11/00
//Raeson:Bugfixed No.46232 & 46234
//sds_real lsp_dexp=300.0;	
sds_real lsp_dexp=309.0;	


/*
**  Note the casting of the long limits to longs!  Without the long cast,
**  the MIN was getting interpreted as an UNSIGNED long.  Godware bug?
**  This is why I decided to create these globals:
*/
sds_real lsp_rlongmin=(sds_real)((long)LONG_MIN);
sds_real lsp_rlongmax=(sds_real)((long)LONG_MAX);

int lsp_cmdhasmore;
int (* lsp_fnvarval)(char *varp, struct resbuf *rbp);
struct lsp_igetlink lsp_iget;
int lsp_errtrace;
unsigned char *lsp_primes;
long lsp_primessz;  /* The alloc'd length of the array. */
struct lsp_atfragslink *lsp_atfrags;
struct lsp_strlink *lsp_tracesyms;
int lsp_cb_recurs;

int lsp_allocsz=514;  /* For our dummy lsp_alloc() function. */



// ***********************
// Used to tell if PROTECTED lisp is being used
//
bool	g_lsp_bProtectedLisp = false;

char *lsp_version="LISP Release 1.0"/*DNT*/;

// Special mappings to hold commandAtomObj classes. The first one is used to
// hold one or more keys mapped to the a particular commandAtomObj class instance.
// For instance, the key 'LINE' and 'LINIE' (in German) would both be keys to
// the same commmandAtomObj that is the class instance holding information to execute
// the function cmd_line(). However 'PRINC' would only have one key pointing to
// it's commandAtomObj class instance, because there is no local (translated) call to
// this LISP atom.
// commandAtomObjPtrList on the other hand is guaranteed to only have one key per
// each commandAtomObj. This will be the list that is used to remove ALL of the
// commandAtomObj structures from memory when required, but specifically when
// exiting icad.
// commandAtomListStarted is just a flag.
//
// The freer is lsp_freeatomsll().
// The searcher is lsp_findatom().
//
// commandAtomObj defines the functions and variables and contains a SPECIAL
// super resbuf.
//

//<alm>: these pointers are initialized in costructors of following static objects,
//		 but never are destroyed, i don't know why
CMapStringToOb* commandAtomList;
CMapPtrToPtr* commandAtomObjPtrList;

template <typename _Map>
struct TLspInitMap
{
public:
	TLspInitMap(_Map*& p)
	{
		p = new _Map(100);
		UINT nHashSize = 113;
		BOOL bAllocNow = TRUE;
		p->InitHashTable(nHashSize, bAllocNow);
	}
};
static TLspInitMap<CMapStringToOb> s_initCommandAtomList(commandAtomList);
static TLspInitMap<CMapPtrToPtr> s_initCommandAtomObjPtrList(commandAtomObjPtrList);

//</alm>

int				commandAtomListStarted;
// Added Cybage AJK 15/10/2001 [
// Reason: Fix for Bug No: 77845 from BugZilla 
CBindStructure bindTable;
//Added Cybage AJK 15/10/2001 DD/MM/YYYY ]
// put a function in here to force it to link

int lsp_ver(void) {
    int msgn;

    msgn=-1;
    if (lsp_argsll!=NULL) { msgn=9; goto out; }
    if (lsp_retstr(lsp_version)!=RTNORM) { msgn=7; goto out; }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}


