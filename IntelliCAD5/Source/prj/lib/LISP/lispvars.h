/* C:\ICAD\PRJ\LIB\LISP\LISPVARS.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
#ifndef _INC_LISPVARS
#define _INC_LISPVARS

//#include "stdafx.h"

extern struct lsp_pendlink lsp_pend;
extern struct resbuf *lsp_argsll;
extern struct resbuf lsp_evres;
extern struct lsp_fileplink *lsp_filepll;
extern struct lsp_sslink *lsp_ssll;
extern int (* lsp_fnvarval)(char *varp, struct resbuf *rbp);
extern struct lsp_igetlink lsp_iget;
extern int lsp_errtrace;
extern struct lsp_strlink *lsp_tracesyms;
extern unsigned char *lsp_primes;
extern long lsp_primessz;  /* The alloc'd length of the array. */
extern struct lsp_atfragslink *lsp_atfrags;
extern int lsp_cmdhasmore;
extern int lsp_cb_recurs;
extern int lsp_lastufncd;
extern char *lsp_badsymchars;
extern char *lsp_esc2true;
extern char *lsp_true2esc;
extern char lsp_mysterysym;
extern sds_real lsp_dmin;
extern sds_real lsp_dexp;
extern sds_real lsp_rlongmin;
extern sds_real lsp_rlongmax;
extern char *lsp_version;
extern CMapStringToOb *commandAtomList;
extern CMapPtrToPtr   *commandAtomObjPtrList;
extern int commandAtomListStarted;
extern int lsp_allocsz;

extern bool g_lsp_bProtectedLisp;

#endif


