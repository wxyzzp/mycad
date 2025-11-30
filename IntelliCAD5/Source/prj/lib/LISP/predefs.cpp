/* C:\ICAD\PRJ\LIB\LISP\PREDEFS.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/

int lsp_defpredefd(void) {
/*
**  Starts lsp_atomsll with the predefined vars and functions.
**
**  Returns:
**             0 : OK
**           -10 : The lsp_atomsll llist already exists
**     Otherwise : The lsp_defatom() return code
*/
    int rc;
    struct resbuf rb;


    rc=0;

    memset(&rb,0,sizeof(rb));

    if (commandAtomListStarted) { rc=-10; goto out; }


    /* Vars: */

    rb.restype=RTT;                                  if ((rc=lsp_defatom("T"/*DNT*/,                 -1,&rb,0))!=0) goto out;
    rb.restype=RTREAL; rb.resval.rreal=IC_PI;        if ((rc=lsp_defatom("PI"/*DNT*/,                -1,&rb,0))!=0) goto out;
    rb.restype=RTSTR;  rb.resval.rstring="\\"/*DNT*/;if ((rc=lsp_defatom("PAUSE"/*DNT*/,             -1,&rb,0))!=0) goto out;


    /* The "external" (invoked) functions (note the 32K ID #'s */
    /* used for now): */

//    rb.restype=LSP_RTXSUB;

//    rb.resval.rstring=(char *)lsp_acad_colordlg;     if ((rc=lsp_defatom("ACAD_COLORDLG"/*DNT*/,  32000,&rb,0))!=0) goto out;
//    rb.resval.rstring=(char *)lsp_acad_helpdlg;      if ((rc=lsp_defatom("ACAD_HELDLG"/*DNT*/,    32001,&rb,0))!=0) goto out;
//    rb.resval.rstring=(char *)lsp_acad_helpdlg;      if ((rc=lsp_defatom("ACAD_HELPDLG"/*DNT*/,   32002,&rb,0))!=0) goto out;
//    rb.resval.rstring=(char *)lsp_acad_strlsort;     if ((rc=lsp_defatom("ACAD_STRLSORT"/*DNT*/,  32003,&rb,0))!=0) goto out;


    /* The internal LISP functions: */

    rb.restype=LSP_RTSUBR;

  #if defined(DEBUG)
    rb.resval.rstring=(char *)lsp_test;              if ((rc=lsp_defatom("TEST"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_chkatoms;          if ((rc=lsp_defatom("CHKATOMS"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_chkss;             if ((rc=lsp_defatom("CHKSS"/*DNT*/,             -1,&rb,0))!=0) goto out;
  #endif

    rb.resval.rstring=(char *)lsp_add;               if ((rc=lsp_defatom("+"/*DNT*/,                 -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_subtr;             if ((rc=lsp_defatom("-"/*DNT*/,                 -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_mult;              if ((rc=lsp_defatom("*"/*DNT*/,                 -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_divide;            if ((rc=lsp_defatom("/"/*DNT*/,                 -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_setq;              if ((rc=lsp_defatom("SETQ"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_clruser;           if ((rc=lsp_defatom("CLRUSER"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_gc;                if ((rc=lsp_defatom("GC"/*DNT*/,                -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_strcat;            if ((rc=lsp_defatom("STRCAT"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_strlen;            if ((rc=lsp_defatom("STRLEN"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_strcase;           if ((rc=lsp_defatom("STRCASE"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_strcase;           if ((rc=lsp_defatom("XSTRCASE"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_substr;            if ((rc=lsp_defatom("SUBSTR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_trim;              if ((rc=lsp_defatom("L:TRIM"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_lpad;              if ((rc=lsp_defatom("LPAD"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_rpad;              if ((rc=lsp_defatom("RPAD"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_chr;               if ((rc=lsp_defatom("CHR"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_compeq;            if ((rc=lsp_defatom("="/*DNT*/,                 -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_compneq;           if ((rc=lsp_defatom("/="/*DNT*/,                -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_complt;            if ((rc=lsp_defatom("<"/*DNT*/,                 -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_compgt;            if ((rc=lsp_defatom(">"/*DNT*/,                 -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_compgte;           if ((rc=lsp_defatom(">="/*DNT*/,                -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_complte;           if ((rc=lsp_defatom("<="/*DNT*/,                -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_and;               if ((rc=lsp_defatom("AND"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_or;                if ((rc=lsp_defatom("OR"/*DNT*/,                -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_if;                if ((rc=lsp_defatom("IF"/*DNT*/,                -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_rtos;              if ((rc=lsp_defatom("RTOS"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_angtos;            if ((rc=lsp_defatom("ANGTOS"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_not;               if ((rc=lsp_defatom("NOT"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_fix;               if ((rc=lsp_defatom("FIX"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_sin;               if ((rc=lsp_defatom("SIN"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cos;               if ((rc=lsp_defatom("COS"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_tan;               if ((rc=lsp_defatom("TAN"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_atan;              if ((rc=lsp_defatom("ATAN"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_abs;               if ((rc=lsp_defatom("ABS"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_sqrt;              if ((rc=lsp_defatom("SQRT"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_log;               if ((rc=lsp_defatom("LOG"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_log10;             if ((rc=lsp_defatom("LOG10"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_exp;               if ((rc=lsp_defatom("EXP"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_expt;              if ((rc=lsp_defatom("EXPT"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_zerop;             if ((rc=lsp_defatom("ZEROP"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_minusp;            if ((rc=lsp_defatom("MINUSP"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_ascii;             if ((rc=lsp_defatom("ASCII"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_atoi;              if ((rc=lsp_defatom("ATOI"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_atof;              if ((rc=lsp_defatom("ATOF"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_progn;             if ((rc=lsp_defatom("PROGN"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_load;              if ((rc=lsp_defatom("L:LOAD"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_princ;             if ((rc=lsp_defatom("PRINC"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_prin1;             if ((rc=lsp_defatom("PRIN1"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_print;             if ((rc=lsp_defatom("L:PRINT"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_open;              if ((rc=lsp_defatom("L:OPEN"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_close;             if ((rc=lsp_defatom("L:CLOSE"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_prompt;            if ((rc=lsp_defatom("PROMPT"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_ver;               if ((rc=lsp_defatom("VER"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_graphscr;          if ((rc=lsp_defatom("L:GRAPHSCR"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_textscr;           if ((rc=lsp_defatom("L:TEXTSCR"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_textpage;          if ((rc=lsp_defatom("TEXTPAGE"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_null;              if ((rc=lsp_defatom("NULL"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_terpri;            if ((rc=lsp_defatom("TERPRI"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_min;               if ((rc=lsp_defatom("MIN"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_max;               if ((rc=lsp_defatom("MAX"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_read_line;         if ((rc=lsp_defatom("READ-LINE"/*DNT*/,         -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_read_char;         if ((rc=lsp_defatom("READ-CHAR"/*DNT*/,         -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_write_line;        if ((rc=lsp_defatom("WRITE-LINE"/*DNT*/,        -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_write_char;        if ((rc=lsp_defatom("WRITE-CHAR"/*DNT*/,        -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_getenv;            if ((rc=lsp_defatom("GETENV"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_setenv;            if ((rc=lsp_defatom("SETENV"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_getint;            if ((rc=lsp_defatom("GETINT"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_getreal;           if ((rc=lsp_defatom("GETREAL"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_getkword;          if ((rc=lsp_defatom("GETKWORD"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_getpoint;          if ((rc=lsp_defatom("GETPOINT"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_getstring;         if ((rc=lsp_defatom("GETSTRING"/*DNT*/,         -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_initget;           if ((rc=lsp_defatom("INITGET"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_getfiled;          if ((rc=lsp_defatom("GETFILED"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_quote;             if ((rc=lsp_defatom("QUOTE"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_list;              if ((rc=lsp_defatom("L:LIST"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_oneplus;           if ((rc=lsp_defatom("1+"/*DNT*/,                -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_oneminus;          if ((rc=lsp_defatom("1-"/*DNT*/,                -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_distance;          if ((rc=lsp_defatom("DISTANCE"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_angle;             if ((rc=lsp_defatom("ANGLE"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_set;               if ((rc=lsp_defatom("SET"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_car;               if ((rc=lsp_defatom("CAR"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cdr;               if ((rc=lsp_defatom("CDR"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_caar;              if ((rc=lsp_defatom("CAAR"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cadr;              if ((rc=lsp_defatom("CADR"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cdar;              if ((rc=lsp_defatom("CDAR"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cddr;              if ((rc=lsp_defatom("CDDR"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_caaar;             if ((rc=lsp_defatom("CAAAR"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_caadr;             if ((rc=lsp_defatom("CAADR"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cadar;             if ((rc=lsp_defatom("CADAR"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_caddr;             if ((rc=lsp_defatom("CADDR"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cdaar;             if ((rc=lsp_defatom("CDAAR"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cdadr;             if ((rc=lsp_defatom("CDADR"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cddar;             if ((rc=lsp_defatom("CDDAR"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cdddr;             if ((rc=lsp_defatom("CDDDR"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_caaaar;            if ((rc=lsp_defatom("CAAAAR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_caaadr;            if ((rc=lsp_defatom("CAAADR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_caadar;            if ((rc=lsp_defatom("CAADAR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_caaddr;            if ((rc=lsp_defatom("CAADDR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cadaar;            if ((rc=lsp_defatom("CADAAR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cadadr;            if ((rc=lsp_defatom("CADADR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_caddar;            if ((rc=lsp_defatom("CADDAR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cadddr;            if ((rc=lsp_defatom("CADDDR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cdaaar;            if ((rc=lsp_defatom("CDAAAR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cdaadr;            if ((rc=lsp_defatom("CDAADR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cdadar;            if ((rc=lsp_defatom("CDADAR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cdaddr;            if ((rc=lsp_defatom("CDADDR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cddaar;            if ((rc=lsp_defatom("CDDAAR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cddadr;            if ((rc=lsp_defatom("CDDADR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cdddar;            if ((rc=lsp_defatom("CDDDAR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cddddr;            if ((rc=lsp_defatom("CDDDDR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cons;              if ((rc=lsp_defatom("CONS"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_while;             if ((rc=lsp_defatom("WHILE"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_entsel;            if ((rc=lsp_defatom("ENTSEL"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_type;              if ((rc=lsp_defatom("TYPE"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_append;            if ((rc=lsp_defatom("APPEND"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_assoc;             if ((rc=lsp_defatom("ASSOC"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_reverse;           if ((rc=lsp_defatom("REVERSE"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_listp;             if ((rc=lsp_defatom("LISTP"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_member;            if ((rc=lsp_defatom("MEMBER"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_nth;               if ((rc=lsp_defatom("NTH"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_subst;             if ((rc=lsp_defatom("SUBST"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_atom;              if ((rc=lsp_defatom("ATOM"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_atoms_family;      if ((rc=lsp_defatom("ATOMS-FAMILY"/*DNT*/,      -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_eval;              if ((rc=lsp_defatom("EVAL"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_last;              if ((rc=lsp_defatom("LAST"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_length;            if ((rc=lsp_defatom("LENGTH"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_eq;                if ((rc=lsp_defatom("EQ"/*DNT*/,                -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_equal;             if ((rc=lsp_defatom("EQUAL"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_defun;             if ((rc=lsp_defatom("DEFUN"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_ssget;             if ((rc=lsp_defatom("SSGET"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_sslength;          if ((rc=lsp_defatom("SSLENGTH"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_ssname;            if ((rc=lsp_defatom("SSNAME"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_ssmemb;            if ((rc=lsp_defatom("SSMEMB"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_ssadd;             if ((rc=lsp_defatom("SSADD"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_ssdel;             if ((rc=lsp_defatom("SSDEL"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_redraw;            if ((rc=lsp_defatom("L:REDRAW"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_grclear;           if ((rc=lsp_defatom("GRCLEAR"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_entdel;            if ((rc=lsp_defatom("ENTDEL"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_entupd;            if ((rc=lsp_defatom("ENTUPD"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_entlast;           if ((rc=lsp_defatom("ENTLAST"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_entnext;           if ((rc=lsp_defatom("ENTNEXT"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_entget;            if ((rc=lsp_defatom("ENTGET"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_entmake;           if ((rc=lsp_defatom("ENTMAKE"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_entmakex;          if ((rc=lsp_defatom("ENTMAKEX"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_entmod;            if ((rc=lsp_defatom("ENTMOD"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cond;              if ((rc=lsp_defatom("COND"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_foreach;           if ((rc=lsp_defatom("FOREACH"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_repeat;            if ((rc=lsp_defatom("REPEAT"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_lambda;            if ((rc=lsp_defatom("LAMBDA"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_apply;             if ((rc=lsp_defatom("APPLY"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_mapcar;            if ((rc=lsp_defatom("MAPCAR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_logand;            if ((rc=lsp_defatom("LOGAND"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_logior;            if ((rc=lsp_defatom("LOGIOR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_boole;             if ((rc=lsp_defatom("BOOLE"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_boundp;            if ((rc=lsp_defatom("BOUNDP"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_alert;             if ((rc=lsp_defatom("ALERT"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_angtof;            if ((rc=lsp_defatom("ANGTOF"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_mem;               if ((rc=lsp_defatom("MEM"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_alloc;             if ((rc=lsp_defatom("ALLOC"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_expand;            if ((rc=lsp_defatom("EXPAND"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_tilde;             if ((rc=lsp_defatom("~"/*DNT*/,                 -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_ads;               if ((rc=lsp_defatom("ADS"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_error;             if ((rc=lsp_defatom("*ERROR*"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_trace;             if ((rc=lsp_defatom("L:TRACE"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_untrace;           if ((rc=lsp_defatom("UNTRACE"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_cvunit;            if ((rc=lsp_defatom("CVUNIT"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_distof;            if ((rc=lsp_defatom("DISTOF"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_exit;              if ((rc=lsp_defatom("L:EXIT"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_quit;              if ((rc=lsp_defatom("L:QUIT"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_findfile;          if ((rc=lsp_defatom("FINDFILE"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_float;             if ((rc=lsp_defatom("FLOAT"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_getcorner;         if ((rc=lsp_defatom("GETCORNER"/*DNT*/,         -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_getangle;          if ((rc=lsp_defatom("GETANGLE"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_getorient;         if ((rc=lsp_defatom("GETORIENT"/*DNT*/,         -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_getdist;           if ((rc=lsp_defatom("GETDIST"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_getvar;            if ((rc=lsp_defatom("GETVAR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_setvar;            if ((rc=lsp_defatom("L:SETVAR"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_grdraw;            if ((rc=lsp_defatom("GRDRAW"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_grtext;            if ((rc=lsp_defatom("GRTEXT"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_grvecs;            if ((rc=lsp_defatom("GRVECS"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_grread;            if ((rc=lsp_defatom("GRREAD"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_itoa;              if ((rc=lsp_defatom("ITOA"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_handent;           if ((rc=lsp_defatom("HANDENT"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_lsh;               if ((rc=lsp_defatom("LSH"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_command;           if ((rc=lsp_defatom("COMMAND"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_tblsearch;         if ((rc=lsp_defatom("TBLSEARCH"/*DNT*/,         -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_tblnext;           if ((rc=lsp_defatom("TBLNEXT"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_numberp;           if ((rc=lsp_defatom("NUMBERP"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_rem;               if ((rc=lsp_defatom("REM"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_inters;            if ((rc=lsp_defatom("INTERS"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_nentsel;           if ((rc=lsp_defatom("NENTSEL"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_nentselp;          if ((rc=lsp_defatom("NENTSELP"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_menucmd;           if ((rc=lsp_defatom("MENUCMD"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_menugroup;         if ((rc=lsp_defatom("MENUGROUP"/*DNT*/,         -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_osnap;             if ((rc=lsp_defatom("OSNAP"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_polar;             if ((rc=lsp_defatom("POLAR"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_read;              if ((rc=lsp_defatom("READ"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_regapp;            if ((rc=lsp_defatom("REGAPP"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_textbox;           if ((rc=lsp_defatom("TEXTBOX"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_trans;             if ((rc=lsp_defatom("TRANS"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_vports;            if ((rc=lsp_defatom("L:VPORTS"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_vmon;              if ((rc=lsp_defatom("VMON"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_wcmatch;           if ((rc=lsp_defatom("WCMATCH"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_xdroom;            if ((rc=lsp_defatom("XDROOM"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_xdsize;            if ((rc=lsp_defatom("XDSIZE"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_xload;             if ((rc=lsp_defatom("XLOAD"/*DNT*/,             -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_xunload;           if ((rc=lsp_defatom("XUNLOAD"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_gcd;               if ((rc=lsp_defatom("GCD"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_tablet;            if ((rc=lsp_defatom("L:TABLET"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_gettbpos;          if ((rc=lsp_defatom("GETTBPOS"/*DNT*/,          -1,&rb,0))!=0) goto out;

    rb.resval.rstring=(char *)lsp_action_tile;       if ((rc=lsp_defatom("ACTION_TILE"/*DNT*/,       -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_add_list;          if ((rc=lsp_defatom("ADD_LIST"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_client_data_tile;  if ((rc=lsp_defatom("CLIENT_DATA_TILE"/*DNT*/,  -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_dimx_tile;         if ((rc=lsp_defatom("DIMX_TILE"/*DNT*/,         -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_dimy_tile;         if ((rc=lsp_defatom("DIMY_TILE"/*DNT*/,         -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_done_dialog;       if ((rc=lsp_defatom("DONE_DIALOG"/*DNT*/,       -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_end_image;         if ((rc=lsp_defatom("END_IMAGE"/*DNT*/,         -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_end_list;          if ((rc=lsp_defatom("END_LIST"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_fill_image;        if ((rc=lsp_defatom("FILL_IMAGE"/*DNT*/,        -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_load_dialog;       if ((rc=lsp_defatom("LOAD_DIALOG"/*DNT*/,       -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_mode_tile;         if ((rc=lsp_defatom("MODE_TILE"/*DNT*/,         -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_new_dialog;        if ((rc=lsp_defatom("NEW_DIALOG"/*DNT*/,        -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_set_tile;          if ((rc=lsp_defatom("SET_TILE"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_slide_image;       if ((rc=lsp_defatom("SLIDE_IMAGE"/*DNT*/,       -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_start_dialog;      if ((rc=lsp_defatom("START_DIALOG"/*DNT*/,      -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_start_image;       if ((rc=lsp_defatom("START_IMAGE"/*DNT*/,       -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_start_list;        if ((rc=lsp_defatom("START_LIST"/*DNT*/,        -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_term_dialog;       if ((rc=lsp_defatom("TERM_DIALOG"/*DNT*/,       -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_unload_dialog;     if ((rc=lsp_defatom("UNLOAD_DIALOG"/*DNT*/,     -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_vector_image;      if ((rc=lsp_defatom("VECTOR_IMAGE"/*DNT*/,      -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_get_attr;          if ((rc=lsp_defatom("GET_ATTR"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_get_tile;          if ((rc=lsp_defatom("GET_TILE"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_getcfg;            if ((rc=lsp_defatom("GETCFG"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_setcfg;            if ((rc=lsp_defatom("SETCFG"/*DNT*/,            -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_getcname;          if ((rc=lsp_defatom("GETCNAME"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_dictadd;           if ((rc=lsp_defatom("DICTADD"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_dictremove;        if ((rc=lsp_defatom("DICTREMOVE"/*DNT*/,        -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_dictnext;          if ((rc=lsp_defatom("DICTNEXT"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_dictrename;        if ((rc=lsp_defatom("DICTRENAME"/*DNT*/,        -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_dictsearch;        if ((rc=lsp_defatom("DICTSEARCH"/*DNT*/,        -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_autoarxload;       if ((rc=lsp_defatom("AUTOARXLOAD"/*DNT*/,       -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_autoload;          if ((rc=lsp_defatom("AUTOLOAD"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_autoxload;         if ((rc=lsp_defatom("AUTOXLOAD"/*DNT*/,         -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_arxload;           if ((rc=lsp_defatom("ARXLOAD"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_arxunload;         if ((rc=lsp_defatom("ARXUNLOAD"/*DNT*/,         -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_arx;               if ((rc=lsp_defatom("L:ARX"/*DNT*/,               -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_help;              if ((rc=lsp_defatom("L:HELP"/*DNT*/,              -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_namedobjdict;      if ((rc=lsp_defatom("NAMEDOBJDICT"/*DNT*/,      -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_setfunhelp;        if ((rc=lsp_defatom("SETFUNHELP"/*DNT*/,        -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_setview;           if ((rc=lsp_defatom("SETVIEW"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_snvalid;           if ((rc=lsp_defatom("SNVALID"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_ssgetfirst;        if ((rc=lsp_defatom("SSGETFIRST"/*DNT*/,        -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_sssetfirst;        if ((rc=lsp_defatom("SSSETFIRST"/*DNT*/,        -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_ssnamex;           if ((rc=lsp_defatom("SSNAMEX"/*DNT*/,           -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_startapp;          if ((rc=lsp_defatom("STARTAPP"/*DNT*/,          -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_tblobjname;        if ((rc=lsp_defatom("TBLOBJNAME"/*DNT*/,        -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_tablet_setScreenSize;if ((rc=lsp_defatom("TABLET_SETSCREENSIZE"/*DNT*/,-1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_tablet_setButton;  if ((rc=lsp_defatom("TABLET_SETBUTTON"/*DNT*/,  -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_tablet_setGridItem;if ((rc=lsp_defatom("TABLET_SETGRIDITEM"/*DNT*/,-1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_tablet_setGridSize;if ((rc=lsp_defatom("TABLET_SETGRIDSIZE"/*DNT*/,-1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_tablet_setDefaults;if ((rc=lsp_defatom("TABLET_SETDEFAULTS"/*DNT*/,-1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_tablet_getButton;  if ((rc=lsp_defatom("TABLET_GETBUTTON"/*DNT*/,  -1,&rb,0))!=0) goto out;
    rb.resval.rstring=(char *)lsp_tablet_getGridItem;if ((rc=lsp_defatom("TABLET_GETGRIDITEM"/*DNT*/,-1,&rb,0))!=0) goto out;

out:
    if (rc) lsp_freeatomsll(); else commandAtomListStarted=1;
    return rc;
}


