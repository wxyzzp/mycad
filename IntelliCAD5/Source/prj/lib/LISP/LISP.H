/* G:\ICADDEV\PRJ\LIB\LISP\LISP.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Definitions for the LISP subsystem.
 *
 */


#ifndef _LISP_H_
#define _LISP_H_

//#include "stdafx.h"
#include "sds.h"/*DNT*/

#undef	X
#undef	Y
#undef	Z
#undef	T

#include <afxwin.h>
#include <afxtempl.h>
#include "icadlib.h"/*DNT*/
#include "lispvars.h"
#include "db.h"/*DNT*/
#include <stdio.h>
#include "res\icadrc2.h"

#if defined(__cplusplus)
	extern "C"/*DNT*/
	{
#endif


		
// Defines

/*
**  The following is an aid in casting a pointer to a function
**  pointer that the lisp evaluator can use:
*/
#define LSP_FNPCAST (int(*)(void))

/*
**  Type codes needed in addition to the RTtypes in the union lsp_vallink
**  below.  (The RTtypes are in the 5000's.  Chances are that if
**  they add any new ones, they'll still be in the 5000's and won't
**  interfere with our 6000's.)
**
**  WHENEVER YOU ADD NEW ONES, CHECK THE lsp_free... FUNCTIONS
**  AND lsp_demote() (AND ANYTHING SIMILAR) IN LISP.C.  FIND ALL
**  OCCURRENCES OF A SIMILAR LSP_RTtype IN LISP.C TO SEE WHERE
**  IT WAS IMPORTANT TO USE IT.
*/
#define LSP_RTSUBR  6000  /* Type-code for an internal LISP function ptr */
#define LSP_RTSYMB  6001  /* Type-code for a symbol */
#define LSP_RTFILE  6002  /* Type-code for a FILE pointer */
#define LSP_RTXSUB  6003  /* Type-code for an ADS function pointer */


// Structures

/*
**  The pending lisp expression (the incomplete fragment not yet
**  executed) and its associated data.
*/
struct lsp_pendlink {
    char   *expr;    /* Left-over expression fragment to be completed. */
    size_t  exprsz;  /* Allocated size of expr. */
    size_t  exprlen; /* String length of expr. */
    int   pc;      /* Number of parentheses still open by the end of expr. */
    int   mode;    /* Mode in effect as of the end of expr: */
                     /*   0=Normal; 1=Quote open; 2=Comment open */
};

/*
**  This struct is for lsp_filepll -- the llist of the FILE pointers
**  opened by our lisp (open) function that are STILL OPEN.
*/
struct lsp_fileplink {
    FILE *fp;
    struct lsp_fileplink *next;
};

/*
**  This struct is for lsp_ssll -- the llist of the selection sets
**  created by our lisp functions that are STILL VALID (have not been freed).
*/
struct lsp_sslink {
    sds_name ss;
    struct lsp_sslink *next;
};

/*
**  Because the LISP getxxx routines need initget -- and that initget must
**  not conflict with ADS's initget usage -- it was necessary to have
**  the LISP initget SAVE the intended values and not actually call
**  the ADS initget until just before the LISP getxxx operation.
**  Here's a structure to hold the data until it's needed:
*/
struct lsp_igetlink {
    int valid;  /* 1 if ready to be used; 0 otherwise */
    int bits;     /* The initget bit codes */
    char *kwl;    /* The initget keyword list (TO BE ALLOC'D AND FREED) */
};

/*
**  For llists for strings.
**  It was created for lsp_tracesyms (the functions being traced),
**  but it can be used by anything that needs a llist of strings.
*/
struct lsp_strlink {
    char *str;
    struct lsp_strlink *next;
};

/*
**  This struct is for a llist of ptrs to the llist of atoms temporarily
**  removed from the atoms list during execution of list-functions.
**  See lsp_atfrags in lisp1.c for more info.
*/
struct lsp_atfragslink {
    class commandAtomObj *fragp;   /* A ptr to a llist of atoms saved */
                                   /* during execution of a list-function */
                                   /* in lsp_dolistfn(). */
    struct lsp_atfragslink *next;
};



int test_fnvv(char *varp, struct resbuf *rbp);

#if defined(DEBUG)
    void lsp_writesuprb(struct resbuf *suprb);
    int  lsp_test(void);
    int  lsp_chkatoms(void);
    int  lsp_chkss(void);
#endif

/* PROTOS */
void			   lsp_init_dialog(
						LPCTSTR wszPathOfBaseDCL ,					 // i:Path of IBase.dcl 
						BOOL bGenerateErrorLog  ,				     // i:Flag set for generation of DCL log.
						LPCTSTR wszPathForErrorLog 			     // i:Path for Error.log to be generated. )
						);
void               lsp_dia_callback(sds_callback_packet *cpkt);
void			   lsp_lispclean(void);
int                lsp_test_fnvv(char *varp, struct resbuf *rbp);
int                lsp_loadstart(char *pn, int startup,int (* fnvarval)(char *varp, struct resbuf *rbp));

int                lsp_lispev(const char *expr, struct resbuf **respp, int (* fnvarval)(char *varp, struct resbuf *rbp), int *pcp);
int                lsp_lispev1(struct resbuf *xll, struct resbuf **respp, int (* fnvarval)(char *varp, struct resbuf *rbp));
int                lsp_lispeval(char *expr, char **respp, int casesens, int (* fnvarval)(char *varp, char **valpp));

int                lsp_1associtem(struct resbuf *sour, struct resbuf **dest, int ads2lisp);
int                lsp_add2ssll(sds_name ss);
int					lsp_andor(int mode);
int                lsp_argcomp(struct resbuf *arg1p, struct resbuf *arg2p, sds_real fuzz);
struct resbuf     *lsp_argend(struct resbuf *argp, struct resbuf **back1pp);
void				lsp_checkatoms(void);
int					lsp_complexcxr(char *op);
int                lsp_convassoc(struct resbuf *sour, struct resbuf **dest, int ads2lisp);
int                lsp_convmat(struct resbuf **rbpp, sds_matrix mat, int ads2lisp);
void				lsp_convlfcr(char *str);
int                lsp_convptlist(struct resbuf *rblist, int mode);
int                lsp_copysplrb(struct resbuf *dest, const struct resbuf *sour);
int                lsp_defatom(const char *name, int id, struct resbuf *valp, int replace, bool LSP_SDS = true);
int                lsp_defpredefd(void);
int                lsp_defun2(const char *sname, int funcno);
int                lsp_demote(void *vp, int candel);
int                lsp_dolistfn(struct resbuf *lfn, struct resbuf *cexpr);
int                lsp_evalarg(struct resbuf *arg, int *ierrp);
int                lsp_evalsym(char *sym, struct resbuf *splrbp, int (* fnvarval)(char *varp, struct resbuf *rbp));
struct lsp_strlink  *lsp_findstrlink(char *str, struct lsp_strlink *sll, struct lsp_strlink **beforepp);
int                lsp_fixfnargs(struct resbuf *beg);
int                lsp_fixnillists(struct resbuf *rblist);
void                 lsp_fnerrmsg(int code);
int                lsp_fnfromarg(struct resbuf *arg, int (**fnpp)(void), struct resbuf **listfnpp, int *freeitp, int *ierrp, class commandAtomObj **alink);
void                 lsp_freeatomlink(class commandAtomObj *ap);
void                 lsp_freeatomsll(void);
void                 lsp_freeiget(void);
void                 lsp_freepend(void);
void                 lsp_freesplrb(struct resbuf *rbp);
void                 lsp_freestrll(struct lsp_strlink *sll);
void                 lsp_freesuprb(struct resbuf *rbp);
struct resbuf       *lsp_getargs(void);
int                  lsp_getfuncode(void);
int					lsp_loader(int mode);
int					lsp_minmax(int op);
struct resbuf       *lsp_nextarg(struct resbuf *currarg);
int                lsp_numstr2rb(char *numstr, double number, struct resbuf *rbp);
int                lsp_prepatomname(char *dest, char *sour, int maxlen, int upper);
int                lsp_prtrecur(struct resbuf *rbarg, FILE *fptr, int mode, int recur);
int                  lsp_regfunc(int (*fhdl) (void), int fcode);
int                lsp_setfnptr(int (*fhdl) (void), int fcode);
int					lsp_simplecxr(int op);
void               lsp_spaceit(int howmany);
struct             lsp_strlink *lsp_findstrlink(char *str, struct lsp_strlink *sll, struct lsp_strlink **beforepp);
int                lsp_undef(const char *sname, int funcno);
int                lsp_undefatom(char *name, int id);
int					lsp_valtos(int doang);
void               lsp_writeatoms(void);
int                lsp_xstr2xll(const char *str, struct resbuf **rbp);
void               lsp_dia_setvars(sds_callback_packet *cpkt,char *key);
void			   lsp_free_dialog_handles(void);

/* Protos of special internal lisp functions that lsp_lispev1() needs */
/* to detect in order to delay evaluation: */

int   lsp_if(void);
int   lsp_quote(void);
int   lsp_setq(void);
int   lsp_while(void);
int   lsp_defun(void);
int   lsp_cond(void);
int   lsp_foreach(void);
int   lsp_repeat(void);
int   lsp_lambda(void);
int   lsp_trace(void);
int   lsp_untrace(void);
int   lsp_and(void);
int   lsp_or(void);


/* Protos of functions that each handle several internal lisp functions: */

int lsp_1list(int op);
int lsp_equalstuff(int op);
int lsp_andior(int mode);
int lsp_tracem(int mode);
int lsp_tof(int which);
int lsp_entmaker(int mode);


int lsp_quote(void);  /* Proto needed by lsp_apply() and lsp_mapcar(). */

int lsp_retint(int ival);
int lsp_retlist(const struct resbuf *rbp);
int lsp_retname(const sds_name aname, int type);
int lsp_retnil(void);
int lsp_retpoint(const sds_point pt);
int lsp_retreal(sds_real rval);
int lsp_retstr(const char *s);
int lsp_rett(void);
int lsp_retval(const struct resbuf *rbuf);
int lsp_retvoid(void);

/* Protos of functions that each handle several internal lisp functions: */

int lsp_tblstuff(int mode);
int lsp_nentselx(int mode);
int lsp_invoker(int what);

// Modified CyberAge AP 10/18/2000
// Reason: To fix CADR problem.
//extern int LSP_ConvertToPoint;
/* Protos of the internal functions (for lsp_defpredefd()): */

int lsp_add(void);           int lsp_subtr(void);             int lsp_mult(void);
int lsp_divide(void);        int lsp_setq(void);              int lsp_clruser(void);
int lsp_gc(void);            int lsp_strcat(void);            int lsp_strlen(void);
int lsp_strcase(void);       int lsp_substr(void);            int lsp_trim(void);
int lsp_lpad(void);          int lsp_rpad(void);              int lsp_chr(void);
int lsp_compeq(void);        int lsp_compneq(void);           int lsp_complt(void);
int lsp_compgt(void);        int lsp_compgte(void);           int lsp_complte(void);
int lsp_and(void);           int lsp_or(void);                int lsp_if(void);
int lsp_rtos(void);          int lsp_angtos(void);            int lsp_not(void);
int lsp_fix(void);           int lsp_sin(void);               int lsp_cos(void);
int lsp_tan(void);           int lsp_atan(void);              int lsp_abs(void);
int lsp_sqrt(void);          int lsp_log(void);               int lsp_log10(void);
int lsp_exp(void);           int lsp_expt(void);              int lsp_zerop(void);
int lsp_minusp(void);        int lsp_ascii(void);             int lsp_atoi(void);
int lsp_atof(void);          int lsp_progn(void);             int lsp_load(void);
int lsp_princ(void);         int lsp_prin1(void);             int lsp_print(void);	int lsp_printem(int mode);
int lsp_open(void);          int lsp_close(void);             int lsp_prompt(void);
int lsp_ver(void);           int lsp_graphscr(void);          int lsp_textscr(void);
int lsp_textpage(void);      int lsp_null(void);              int lsp_terpri(void);
int lsp_min(void);           int lsp_max(void);               int lsp_read_line(void);
int lsp_read_char(void);     int lsp_write_line(void);        int lsp_write_char(void);
int lsp_getenv(void);        int lsp_getint(void);            int lsp_getreal(void);
int lsp_getkword(void);      int lsp_getpoint(void);          int lsp_getstring(void);
int lsp_initget(void);       int lsp_getfiled(void);          int lsp_quote(void);
int lsp_list(void);          int lsp_oneplus(void);           int lsp_oneminus(void);
int lsp_distance(void);      int lsp_angle(void);             int lsp_set(void);
int lsp_car(void);           int lsp_cdr(void);               int lsp_caar(void);
int lsp_cadr(void);          int lsp_cdar(void);              int lsp_cddr(void);
int lsp_caaar(void);         int lsp_caadr(void);             int lsp_cadar(void);
int lsp_caddr(void);         int lsp_cdaar(void);             int lsp_cdadr(void);
int lsp_cddar(void);         int lsp_cdddr(void);             int lsp_caaaar(void);
int lsp_caaadr(void);        int lsp_caadar(void);            int lsp_caaddr(void);
int lsp_cadaar(void);        int lsp_cadadr(void);            int lsp_caddar(void);
int lsp_cadddr(void);        int lsp_cdaaar(void);            int lsp_cdaadr(void);
int lsp_cdadar(void);        int lsp_cdaddr(void);            int lsp_cddaar(void);
int lsp_cddadr(void);        int lsp_cdddar(void);            int lsp_cddddr(void);
int lsp_cons(void);          int lsp_while(void);             int lsp_entsel(void);
int lsp_type(void);          int lsp_append(void);            int lsp_assoc(void);
int lsp_reverse(void);       int lsp_listp(void);             int lsp_member(void);
int lsp_nth(void);           int lsp_subst(void);             int lsp_atom(void);
int lsp_atoms_family(void);  int lsp_eval(void);              int lsp_last(void);
int lsp_length(void);        int lsp_eq(void);                int lsp_equal(void);
int lsp_defun(void);         int lsp_ssget(void);             int lsp_sslength(void);
int lsp_ssname(void);        int lsp_ssmemb(void);            int lsp_ssadd(void);
int lsp_ssdel(void);         int lsp_redraw(void);            int lsp_grclear(void);
int lsp_entdel(void);        int lsp_entupd(void);            int lsp_entlast(void);
int lsp_entnext(void);       int lsp_entget(void);            int lsp_entmake(void);
int lsp_entmakex(void);
int lsp_entmod(void);        int lsp_cond(void);              int lsp_foreach(void);
int lsp_repeat(void);        int lsp_lambda(void);            int lsp_apply(void);
int lsp_mapcar(void);        int lsp_logand(void);            int lsp_logior(void);
int lsp_boole(void);         int lsp_boundp(void);            int lsp_alert(void);
int lsp_angtof(void);        int lsp_mem(void);               int lsp_alloc(void);
int lsp_expand(void);        int lsp_tilde(void);             int lsp_ads(void);
int lsp_error(void);         int lsp_trace(void);             int lsp_untrace(void);
int lsp_cvunit(void);        int lsp_distof(void);            int lsp_exit(void);
int lsp_quit(void);          int lsp_findfile(void);          int lsp_float(void);
int lsp_getcorner(void);     int lsp_getangle(void);          int lsp_getorient(void);
int lsp_getdist(void);       int lsp_getvar(void);            int lsp_setvar(void);
int lsp_grdraw(void);        int lsp_grtext(void);            int lsp_grvecs(void);
int lsp_grread(void);        int lsp_itoa(void);              int lsp_handent(void);
int lsp_lsh(void);           int lsp_command(void);           int lsp_tblsearch(void);
int lsp_tblnext(void);       int lsp_numberp(void);           int lsp_rem(void);
int lsp_inters(void);        int lsp_nentsel(void);           int lsp_nentselp(void);
int lsp_menucmd(void);       int lsp_osnap(void);             int lsp_polar(void);
int lsp_read(void);          int lsp_regapp(void);            int lsp_textbox(void);
int lsp_trans(void);         int lsp_vports(void);            int lsp_vmon(void);
int lsp_wcmatch(void);       int lsp_xdroom(void);            int lsp_xdsize(void);
int lsp_xload(void);         int lsp_xunload(void);           int lsp_gcd(void);
int lsp_tablet(void);        int lsp_setenv(void);

int lsp_tablet_setScreenSize(void);
int lsp_tablet_setButton(void);
int lsp_tablet_setGridItem(void);
int lsp_tablet_setGridSize(void);
int lsp_tablet_setDefaults(void);
int lsp_tablet_getButton(void);
int lsp_tablet_getGridItem(void);

int lsp_acad_colordlg(void); int lsp_acad_helpdlg(void);      int lsp_acad_strlsort(void);

int lsp_action_tile(void);   int lsp_add_list(void);          int lsp_client_data_tile(void);
int lsp_dimx_tile(void);     int lsp_dimy_tile(void);         int lsp_done_dialog(void);
int lsp_end_image(void);     int lsp_end_list(void);          int lsp_fill_image(void);
int lsp_load_dialog(void);   int lsp_mode_tile(void);         int lsp_new_dialog(void);
int lsp_set_tile(void);      int lsp_slide_image(void);       int lsp_start_dialog(void);
int lsp_start_image(void);   int lsp_start_list(void);        int lsp_term_dialog(void);
int lsp_unload_dialog(void); int lsp_vector_image(void);      int lsp_get_attr(void);
int lsp_get_tile(void);

// New for r13+
int lsp_getcfg(void);        int lsp_setcfg(void);                   
int lsp_getcname(void);      int lsp_dictadd(void);           int lsp_dictremove(void);
int lsp_dictnext(void);      int lsp_dictrename(void);        int lsp_dictsearch(void);   
int lsp_autoarxload(void);   int lsp_autoload(void);          int lsp_autoxload(void);    
int lsp_arxload(void);       int lsp_arxunload(void);         int lsp_arx(void);          
int lsp_help(void);          int lsp_namedobjdict(void);      int lsp_setfunhelp(void);   
int lsp_setview(void);       int lsp_snvalid(void);           int lsp_ssgetfirst(void);   
int lsp_sssetfirst(void);    int lsp_startapp(void);          int lsp_tblobjname(void);
int lsp_menugroup(void);     int lsp_ssnamex(void);           int lsp_gettbpos(void);   
  
#if defined(__cplusplus)
	}
#endif

// NOTE :: Outside of extern "C" block!!
//
// dwThreadId parameter is optional.  If it is used, match the other kind of match AND
// same threadid.  For example, if mode == 2, look up by id AND threadid
//
class commandAtomObj *lsp_findatom(const TCHAR *key, int mode, DWORD dwThreadId = 0, bool LSP_SDS = true);

#endif // _LISP_H_




