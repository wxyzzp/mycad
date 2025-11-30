/* C:\ICAD\PRJ\LIB\DB\HANDITEM.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

/*
**  An item that has a handle (table record or entity).
**  Note that tables and block defs both have llists of these.
*/
#ifndef INC_HANDITEM_H
#define INC_HANDITEM_H
#pragma pack (push)
#pragma pack (1)

class DB_CLASS db_handitem {  /* HIDEF */

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    /* Constructors/Destructor: */
    db_handitem(void);
    db_handitem(int ptype);
    db_handitem(const db_handitem &sour);  /* Copy constructor */
    virtual ~db_handitem(void);

    db_handitem *newcopy(void);


    /* Getters: */

    int ret_type(void) { return (int)type; }  /* The numeric type */
    char *ret_eed(void) { return eed; }  /* WARNING: Gives access */

    virtual db_handitem *ret_ltypehip(void) { return NULL; }
    virtual db_handitem *ret_layerhip(void) { return NULL; }
    virtual int ret_deleted(void)   { return 0; }
    virtual int ret_invisible(void) { return 0; }
    virtual int ret_pspace(void)    { return 0; }
    virtual int ret_bylayer(void)    { return 0; }
    virtual int ret_byblock(void)    { return 0; }
    virtual int ret_continuous(void) { return 0; }
    virtual struct db_fontlink *ret_font(void) { return NULL; }
    virtual struct db_fontlink *ret_bigfont(void) { return NULL; }
    virtual int ret_looked4xref(void) { return 0; }
    virtual db_drawing *ret_xrefdp(void) { return NULL; }
    virtual int ret_scenario(void) { return 0; }
    virtual int ret_is3pt(void) { return 0; }

    virtual void *ret_disp(void) { return NULL; }
    virtual void  get_extent(sds_point ll, sds_point ur) { ll[0]=ll[1]=ll[2]=ur[0]=ur[1]=ur[2]=0.0; }

    virtual bool get_ptbyidx(sds_point p, int i) { return false; }

    virtual char     **retp_1(void)  { return NULL; }
    virtual sds_real  *retp_10(void) { return NULL; }
    virtual sds_real  *retp_10(int idx) { return NULL; }
    virtual sds_real  *retp_11(void) { return NULL; }
    virtual sds_real  *retp_11(int idx) { return NULL; }
    virtual sds_real  *retp_12(void) { return NULL; }
    virtual sds_real  *retp_13(void) { return NULL; }
    virtual sds_real  *retp_14(void) { return NULL; }
    virtual sds_real  *retp_15(void) { return NULL; }
    virtual sds_real  *retp_16(void) { return NULL; }
    virtual sds_real  *retp_40(void) { return NULL; }
    virtual sds_real  *retp_41(void) { return NULL; }
    virtual sds_real  *retp_42(void) { return NULL; }
    virtual sds_real  *retp_43(void) { return NULL; }
    virtual sds_real  *retp_50(void) { return NULL; }

    virtual bool get_0(char* p, int maxlen) {
        if (type<0 || type>=DB_NHITYPES) *p=0;
        else { strncpy(p,db_hitypeinfo[type].name,maxlen); p[maxlen]=0; }
        return true;
    }
    virtual bool get_1(char *p, int maxlen) { *p=0; return false; }
    virtual bool get_1(char **p) { *p=NULL; return false; }
    virtual bool get_2(char *p, int maxlen) { *p=0; return false; }
    virtual bool get_2(char **p) { *p=NULL; return false; }
    virtual bool get_2(db_handitem **p) { *p=NULL; return false; }
    virtual bool get_3(char *p, int maxlen) { *p=0; return false; }
    virtual bool get_3(char **p) { *p=NULL; return false; }
    virtual bool get_3(db_handitem **p) { *p=NULL; return false; }
    virtual bool get_4(char *p, int maxlen) { *p=0; return false; }
    virtual bool get_4(char **p) { *p=NULL; return false; }
    virtual bool get_6(char *p, int maxlen) { *p=0; return false; }
    virtual bool get_6(char **p) { *p=NULL; return false; }
    virtual bool get_6(db_handitem **p) { *p=NULL; return false; }
    virtual bool get_6(char **p, int n) { return false; }
    virtual bool get_7(char *p, int maxlen) { *p=0; return false; }
    virtual bool get_7(char **p) { *p=NULL; return false; }
    virtual bool get_7(db_handitem **p) { *p=NULL; return false; }
    virtual bool get_8(char *p, int maxlen) { *p=0; return false; }
    virtual bool get_8(char **p) { *p=NULL; return false; }
    virtual bool get_8(db_handitem **p) { *p=NULL; return false; }
    virtual bool get_10(sds_point p) { p[0]=p[1]=p[2]=0.0; return false; }
    virtual bool get_10(sds_point p, int idx) { p[0]=p[1]=p[2]=0.0; return false; }
    virtual bool get_10(sds_point *p, int n) { return false; }
    virtual bool get_11(sds_point p) { p[0]=p[1]=p[2]=0.0; return false; }
    virtual bool get_11(sds_point *p) { return false; }
    virtual bool get_11(sds_point *p, int n) { return false; }
    virtual bool get_12(sds_point p) { p[0]=p[1]=p[2]=0.0; return false; }
    virtual bool get_12(sds_point *p) { return false; }
    virtual bool get_12(sds_point *p, int n) { return false; }
    virtual bool get_13(sds_point p) { p[0]=p[1]=p[2]=0.0; return false; }
    virtual bool get_13(sds_point *p) { return false; }
    virtual bool get_13(sds_point *p, int n) { return false; }
    virtual bool get_14(sds_point p) { p[0]=p[1]=p[2]=0.0; return false; }
    virtual bool get_14(sds_point *p, int n) { return false; }
    virtual bool get_15(sds_point p) { p[0]=p[1]=p[2]=0.0; return false; }
    virtual bool get_16(sds_point p) { p[0]=p[1]=p[2]=0.0; return false; }
    virtual bool get_17(sds_point p) { p[0]=p[1]=p[2]=0.0; return false; }
    virtual bool get_18(sds_point p) { p[0]=p[1]=p[2]=0.0; return false; }
    virtual bool get_38(sds_real *p) { *p=0.0; return false; }
    virtual bool get_39(sds_real *p) { *p=0.0; return false; }
    virtual bool get_40(sds_real *p) { *p=0.0; return false; }
    virtual bool get_40(sds_real *p, int idx) { return false; }
    virtual bool get_40array(sds_real *p, int n) { return false; }
    virtual bool get_41(sds_real *p) { *p=0.0; return false; }
    virtual bool get_41(sds_real *p, int idx) { return false; }
    virtual bool get_41array(sds_real *p, int n) { return false; }
    virtual bool get_41(sds_real *p, int vidx, int eidx) { return false; }
    virtual bool get_42(sds_real *p) { *p=0.0; return false; }
    virtual bool get_42(sds_real *p, int idx) { return false; }
    virtual bool get_42array(sds_real *p, int n) { return false; }
    virtual bool get_42(sds_real *p, int vidx, int eidx) { return false; }
    virtual bool get_43(sds_real *p) { *p=0.0; return false; }
    virtual bool get_44(sds_real *p) { *p=0.0; return false; }
    virtual bool get_45(sds_real *p) { *p=0.0; return false; }
    virtual bool get_46(sds_real *p) { *p=0.0; return false; }
    virtual bool get_47(sds_real *p) { *p=0.0; return false; }
    virtual bool get_48(sds_real *p) { *p=1.0; return false; }
    virtual bool get_49(sds_real *p) { *p=0.0; return false; }
    virtual bool get_49(sds_real *p, int n) { return false; }
    virtual bool get_50(sds_real *p) { *p=0.0; return false; }
    virtual bool get_51(sds_real *p) { *p=0.0; return false; }
    virtual bool get_52(sds_real *p) { *p=0.0; return false; }
    virtual bool get_53(sds_real *p) { *p=0.0; return false; }
    virtual bool get_54(sds_real *p) { *p=0.0; return false; }
    virtual bool get_55(sds_real *p) { *p=0.0; return false; }
    virtual bool get_56(sds_real *p) { *p=0.0; return false; }
    virtual bool get_57(sds_real *p) { *p=0.0; return false; }
    virtual bool get_58(sds_real *p) { *p=0.0; return false; }
    virtual bool get_59(sds_real *p) { *p=0.0; return false; }
    virtual bool get_60(int *p) { *p=0; return false; }
    virtual bool get_62(int *p) { *p=DB_BLCOLOR; return false; }
    virtual bool get_62(int *p, int n) { return false; }
    virtual bool get_66(int *p) { *p=0; return false; }
    virtual bool get_67(int *p) { *p=0; return false; }
    virtual bool get_68(int *p) { *p=0; return false; }
    virtual bool get_69(int *p) { *p=0; return false; }
    virtual bool get_70(int *p) { *p=0; return false; }
    virtual bool get_71(int *p) { *p=0; return false; }
    virtual bool get_72(int *p) { *p=0; return false; }
    virtual bool get_73(int *p) { *p=0; return false; }
    virtual bool get_74(int *p) { *p=0; return false; }
    virtual bool get_74(int *p, int vidx, int eidx) { *p=0; return false; }
    virtual bool get_75(int *p) { *p=0; return false; }
    virtual bool get_75(int *p, int vidx, int eidx) { *p=0; return false; }
    virtual bool get_76(int *p) { *p=0; return false; }
    virtual bool get_77(int *p) { *p=0; return false; }
    virtual bool get_78(int *p) { *p=0; return false; }
    virtual bool get_79(int *p) { *p=0; return false; } // EBATECH(CNBR) Bugzilla#78443 2003/2/7 DIMENSION/DIMAZIN
    virtual bool get_90(int *p) { *p=0; return false; }
    virtual bool get_91(int *p) { *p=0; return false; }
    virtual bool get_92(int *p) { *p=0; return false; }
    virtual bool get_93(int *p) { *p=0; return false; }
    virtual bool get_94(int *p) { *p=0; return false; }
    virtual bool get_95(int *p) { *p=0; return false; }
    virtual bool get_96(int *p) { *p=0; return false; }
    virtual bool get_97(int *p) { *p=0; return false; }
    virtual bool get_98(int *p) { *p=0; return false; }
    virtual bool get_99(int *p) { *p=0; return false; }
    // EBATECH(CNBR) -[ 2002/6/19 VIEWPORT UCSORG/UCSXDIR/UCSYDIR
    virtual bool get_110(sds_point p) { p[0]=p[1]=p[2]=0.0; return false; }
    virtual bool get_111(sds_point p) { p[0]=p[1]=p[2]=0.0; return false; }
    virtual bool get_112(sds_point p) { p[0]=p[1]=p[2]=0.0; return false; }
    // EBATECH(CNBR) ]-
    virtual bool get_140(sds_real *p) { *p=0.0; return false; }
    virtual bool get_141(sds_real *p) { *p=0.0; return false; }
    virtual bool get_142(sds_real *p) { *p=0.0; return false; }
    virtual bool get_143(sds_real *p) { *p=0.0; return false; }
    virtual bool get_144(sds_real *p) { *p=0.0; return false; }
    virtual bool get_145(sds_real *p) { *p=0.0; return false; }
    virtual bool get_146(sds_real *p) { *p=0.0; return false; }
    virtual bool get_147(sds_real *p) { *p=0.0; return false; }
    // EBATECH(CNBR) -[ 2002/10/10 PLOTSETTINS PAPERIMAGEORIGIN X,Y
    virtual bool get_148(sds_real *p) { *p=0.0; return false; }
    virtual bool get_149(sds_real *p) { *p=0.0; return false; }
    // EBATECH(CNBR) ]-
    virtual bool get_170(int *p) { *p=0; return false; }
    virtual bool get_171(int *p) { *p=0; return false; }
    virtual bool get_172(int *p) { *p=0; return false; }
    virtual bool get_173(int *p) { *p=0; return false; }
    virtual bool get_174(int *p) { *p=0; return false; }
    virtual bool get_175(int *p) { *p=0; return false; }
    virtual bool get_176(int *p) { *p=0; return false; }
    virtual bool get_177(int *p) { *p=0; return false; }
    virtual bool get_178(int *p) { *p=0; return false; }
    virtual bool get_179(int *p) { *p=0; return false; } // EBATECH(CNBR) Bugzilla#78443 2003/2/7 DIMENSION/DIMADEC
    virtual bool get_210(sds_point p) { p[0]=0.0; p[1]=0.0; p[2]=1.0; return false; }
    virtual bool get_211(sds_point p) { p[0]=0.0; p[1]=0.0; p[2]=1.0; return false; }
    virtual bool get_212(sds_point p) { p[0]=0.0; p[1]=0.0; p[2]=1.0; return false; }
	//Bugzilla No. 78013 ; 01-04-2002
	virtual bool get_213(sds_point p) { p[0]=0.0; p[1]=0.0; p[2]=1.0; return false; }
    virtual bool get_270(int *p) { *p=0; return false; }
    virtual bool get_271(int *p) { *p=0; return false; }
    virtual bool get_272(int *p) { *p=0; return false; }
    virtual bool get_273(int *p) { *p=0; return false; }
    virtual bool get_274(int *p) { *p=0; return false; }
    virtual bool get_275(int *p) { *p=0; return false; }

    // EBATECH(watanabe)-[dimfrac
    virtual bool get_276(int *p) { *p=0; return false; }
    // ]-EBATECH(watanabe)

    // EBATECH(watanabe)-[dimlunit
    virtual bool get_277(int *p) { *p=0; return false; }
    // ]-EBATECH(watanabe)

    virtual bool get_278(int *p) { *p=0; return false; } // EBATECH(CNBR) 2003/3/31 DIMENSION/DIMDSEP

    // EBATECH(watanabe)-[dimtmove
    virtual bool get_279(int *p) { *p=0; return false; }
    // ]-EBATECH(watanabe)

    virtual bool get_280(int *p) { *p=0; return false; }
    virtual bool get_281(int *p) { *p=0; return false; }
    virtual bool get_282(int *p) { *p=0; return false; }
    virtual bool get_283(int *p) { *p=0; return false; }
    virtual bool get_284(int *p) { *p=0; return false; }
    virtual bool get_285(int *p) { *p=0; return false; }
    virtual bool get_286(int *p) { *p=0; return false; }
    virtual bool get_287(int *p) { *p=0; return false; }
    virtual bool get_288(int *p) { *p=0; return false; }
    virtual bool get_289(int *p) { *p=0; return false; }
    virtual bool get_290(int *p) { *p=0; return false; }
    virtual bool get_291(int *p) { *p=0; return false; }
    virtual bool get_292(int *p) { *p=0; return false; }
    virtual bool get_293(int *p) { *p=0; return false; }
    virtual bool get_294(int *p) { *p=0; return false; }
    virtual bool get_295(int *p) { *p=0; return false; }
    virtual bool get_296(int *p) { *p=0; return false; }
    virtual bool get_297(int *p) { *p=0; return false; }
    virtual bool get_298(int *p) { *p=0; return false; }
    virtual bool get_299(int *p) { *p=0; return false; }
    virtual bool get_300(char **p) { *p=NULL; return false; }
    virtual bool get_310(char **p) { *p=NULL; return false; }

    virtual bool get_340name(char *p, int maxlen) { *p=0; return false; }
    virtual bool get_340name(char **p) { *p=NULL; return false; }
    virtual bool get_340hand(char *p, int maxlen) { *p=0; return false; }
    virtual bool get_340hand(char **p) { *p=NULL; return false; }
	virtual bool get_340hand(db_objhandle *p) {p = NULL; return false;}
    virtual bool get_340(db_handitem **p) { *p=NULL; return false; }
    // EBATECH(watanabe)-[dimldrblk
    // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
    //virtual bool get_341(char *p, int maxlen) { *p=0; return false; }
    //virtual bool get_341(char **p) { *p=NULL; return false; }
    virtual bool get_341name(char *p, int maxlen) { *p=0; return false; }
    virtual bool get_341name(char **p) { *p=NULL; return false; }
    virtual bool get_341hand(char *p, int maxlen) { *p=0; return false; }
    virtual bool get_341hand(char **p) { *p=NULL; return false; }
    virtual bool get_341hand(db_objhandle *p) {p = NULL; return false;}
    // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
    virtual bool get_341(db_handitem **p) { *p=NULL; return false; }
    // ]-EBATECH(watanabe)
    // EBATECH(CNBR) -[ 2002/6/19 VIEWPORT NAMDUCS/BASEUCS
    virtual bool get_345(db_handitem **p) { *p=NULL; return false; }
    virtual bool get_346(db_handitem **p) { *p=NULL; return false; }
    // EBATECH(CNBR) ]-
    virtual bool get_360hand(char **p) { *p=NULL; return false; }
	virtual bool get_360hand(db_objhandle *p) {p = NULL; return false;}
    virtual bool get_360(db_handitem **p) { *p=NULL; return false; }
    virtual bool get_370(int *p) { *p=0; return false; }
    virtual bool get_371(int *p) { *p=0; return false; }
    virtual bool get_372(int *p) { *p=0; return false; }
    virtual bool get_373(int *p) { *p=0; return false; }
    virtual bool get_374(int *p) { *p=0; return false; }
    virtual bool get_375(int *p) { *p=0; return false; }
    virtual bool get_376(int *p) { *p=0; return false; }
    virtual bool get_377(int *p) { *p=0; return false; }
    virtual bool get_378(int *p) { *p=0; return false; }
    virtual bool get_379(int *p) { *p=0; return false; }
    virtual bool get_380(int *p) { *p=0; return false; }
    virtual bool get_381(int *p) { *p=0; return false; }
    virtual bool get_382(int *p) { *p=0; return false; }
    virtual bool get_383(int *p) { *p=0; return false; }
    virtual bool get_384(int *p) { *p=0; return false; }
    virtual bool get_385(int *p) { *p=0; return false; }
    virtual bool get_386(int *p) { *p=0; return false; }
    virtual bool get_387(int *p) { *p=0; return false; }
    virtual bool get_388(int *p) { *p=0; return false; }
    virtual bool get_389(int *p) { *p=0; return false; }
    virtual bool get_390(db_handitem **p) { *p=0; return false; }
    virtual bool get_391(db_handitem **p) { *p=0; return false; }
    virtual bool get_392(db_handitem **p) { *p=0; return false; }
    virtual bool get_393(db_handitem **p) { *p=0; return false; }
    virtual bool get_394(db_handitem **p) { *p=0; return false; }
    virtual bool get_395(db_handitem **p) { *p=0; return false; }
    virtual bool get_396(db_handitem **p) { *p=0; return false; }
    virtual bool get_397(db_handitem **p) { *p=0; return false; }
    virtual bool get_398(db_handitem **p) { *p=0; return false; }
    virtual bool get_399(db_handitem **p) { *p=0; return false; }
	virtual bool get_400(short *s) { *s=0; return false; }
	virtual bool get_401(short *s) { *s=0; return false; }
	virtual bool get_402(short *s) { *s=0; return false; }
	virtual bool get_403(short *s) { *s=0; return false; }
	virtual bool get_404(short *s) { *s=0; return false; }
	virtual bool get_405(short *s) { *s=0; return false; }
	virtual bool get_406(short *s) { *s=0; return false; }
	virtual bool get_407(short *s) { *s=0; return false; }
	virtual bool get_408(short *s) { *s=0; return false; }
	virtual bool get_409(short *s) { *s=0; return false; }
	virtual bool get_410(char *s) { *s=0; return false; }
	virtual bool get_410(char *s, db_drawing *dp) { *s=0; return false; }
	virtual bool get_411(char *s) { *s=0; return false; }
	virtual bool get_411(char *s, db_drawing *dp) { *s=0; return false; }
	virtual bool get_412(char *s) { *s=0; return false; }
	virtual bool get_412(char *s, db_drawing *dp) { *s=0; return false; }
	virtual bool get_413(char *s) { *s=0; return false; }
	virtual bool get_413(char *s, db_drawing *dp) { *s=0; return false; }
	virtual bool get_414(char *s) { *s=0; return false; }
	virtual bool get_414(char *s, db_drawing *dp) { *s=0; return false; }
	virtual bool get_415(char *s) { *s=0; return false; }
	virtual bool get_415(char *s, db_drawing *dp) { *s=0; return false; }
	virtual bool get_416(char *s) { *s=0; return false; }
	virtual bool get_416(char *s, db_drawing *dp) { *s=0; return false; }
	virtual bool get_417(char *s) { *s=0; return false; }
	virtual bool get_417(char *s, db_drawing *dp) { *s=0; return false; }
	virtual bool get_418(char *s) { *s=0; return false; }
	virtual bool get_418(char *s, db_drawing *dp) { *s=0; return false; }
	virtual bool get_419(char *s) { *s=0; return false; }
	virtual bool get_419(char *s, db_drawing *dp) { *s=0; return false; }

    /* Setters/stealers: */

    virtual void steal_blockredef(db_handitem *first, db_handitem *last) {}

    void set_type(int p) { type=(char)((p>-1 && p<DB_NHITYPES) ? p : 0); }

    virtual void set_ltypehip(db_handitem *p) {}
    virtual void set_layerhip(db_handitem *p) {}
    virtual void set_deleted(int p) {}
    virtual void set_tabrecidx(int p) {}
    virtual void set_disp(void *p) {}
    virtual void set_extent(sds_point ll, sds_point ur) {}
    virtual void set_looked4xref(int p) {}
    virtual void set_xrefdp(db_drawing *p) {}
    virtual void null_xrefdp(void) {}
    virtual void set_scenario(int p) {}

    virtual bool set_0(const char* p) {
        type=0;
        if (p==NULL) return false;
        for (type=0; type<DB_NHITYPES &&
            db_compareNames(p,db_hitypeinfo[type].name); type++);
        if (type>=DB_NHITYPES) { type=0; return false; }
        return true;
    }
    virtual bool set_1(char *p) { return false; }
    virtual bool set_2(char*) { return false; }
    virtual bool set_2(char *p, db_drawing *dp) { return false; }
    virtual bool set_2(db_handitem *p) { return false; }
    virtual bool set_3(char *p) { return false; }
    virtual bool set_3(char *p, db_drawing *dp) { return false; }
    virtual bool set_3(db_handitem *p) { return false; }
    virtual bool set_4(char *p) { return false; }
    virtual bool set_4(char *p, db_drawing *dp) { return false; }
    virtual bool set_6(char *p) { return false; }
    virtual bool set_6(char *p, db_drawing *dp) { return false; }
    virtual bool set_6(db_handitem *p) { return false; }
    virtual bool set_6(char **p, int n, db_drawing *dp) { return false; }
    virtual bool set_7(char *p) { return false; }
    virtual bool set_7(char *p, db_drawing *dp) { return false; }
    virtual bool set_7(db_handitem *p) { return false; }
    virtual bool set_8(char *p) { return false; }
    virtual bool set_8(char *p, db_drawing *dp) { return false; }
    virtual bool set_10(sds_point p) { return false; }
    virtual bool set_10(sds_point p, int idx) { return false; }
    virtual bool set_10(sds_point *p, int n) { return false; }
    virtual bool set_11(sds_point p) { return false; }
    virtual bool set_11(sds_point *p) { return false; }
    virtual bool set_11(sds_point *p, int n) { return false; }
    virtual bool set_12(sds_point p) { return false; }
    virtual bool set_12(sds_point *p) { return false; }
    virtual bool set_12(sds_point *p, int n) { return false; }
    virtual bool set_13(sds_point p) { return false; }
    virtual bool set_13(sds_point *p) { return false; }
    virtual bool set_13(sds_point *p, int n) { return false; }
    virtual bool set_14(sds_point p) { return false; }
    virtual bool set_14(sds_point *p, int n) { return false; }
    virtual bool set_15(sds_point p) { return false; }
    virtual bool set_16(sds_point p) { return false; }
    virtual bool set_17(sds_point p) { return false; }
    virtual bool set_18(sds_point p) { return false; }
    virtual bool set_38(sds_real p) { return false; }
    virtual bool set_39(sds_real p) { return false; }
    virtual bool set_40(sds_real p) { return false; }
    virtual bool set_40(sds_real p, int idx) { return false; }
    virtual bool set_40(sds_real *p, int n) { return false; }
    virtual bool set_41(sds_real p) { return false; }
    virtual bool set_41(sds_real p, int idx) { return false; }
    virtual bool set_41(sds_real *p, int n) { return false; }
    virtual bool set_41(sds_real *p, int vidx, int eidx) { return false; }
    virtual bool set_42(sds_real p) { return false; }
    virtual bool set_42(sds_real p, int idx) { return false; }
    virtual bool set_42(sds_real *p, int n) { return false; }
    virtual bool set_42(sds_real *p, int vidx, int eidx) { return false; }
    virtual bool set_43(sds_real p) { return false; }
    virtual bool set_44(sds_real p) { return false; }
    virtual bool set_45(sds_real p) { return false; }
    virtual bool set_46(sds_real p) { return false; }
    virtual bool set_47(sds_real p) { return false; }
    virtual bool set_48(sds_real p) { return false; }
    virtual bool set_49(sds_real p) { return false; }
    virtual bool set_49(sds_real *p, int n) { return false; }
    virtual bool set_50(sds_real p) { return false; }
    virtual bool set_51(sds_real p) { return false; }
    virtual bool set_52(sds_real p) { return false; }
    virtual bool set_53(sds_real p) { return false; }
    virtual bool set_54(sds_real p) { return false; }
    virtual bool set_55(sds_real p) { return false; }
    virtual bool set_56(sds_real p) { return false; }
    virtual bool set_57(sds_real p) { return false; }
    virtual bool set_58(sds_real p) { return false; }
    virtual bool set_60(int p) { return false; }
    virtual bool set_62(int p) { return false; }
    virtual bool set_62(int *p, int n) { return false; }
    virtual bool set_66(int p) { return false; }
    virtual bool set_67(int p) { return false; }
    virtual bool set_68(int p) { return false; }
    virtual bool set_69(int p) { return false; }
    virtual bool set_70(int p) { return false; }
    virtual bool set_71(int p) { return false; }
    virtual bool set_72(int p) { return false; }
    virtual bool set_73(int p) { return false; }
    virtual bool set_74(int p) { return false; }
    virtual bool set_74(int p, int vidx, int eidx) { return false; }
    virtual bool set_75(int p) { return false; }
    virtual bool set_75(int p, int vidx, int eidx) { return false; }
    virtual bool set_76(int p) { return false; }
    virtual bool set_77(int p) { return false; }
    virtual bool set_78(int p) { return false; }
    virtual bool set_79(int p) { return false; } // EBATECH(CNBR) 2002/6/19 ucs ortho view type(0-6)
    virtual bool set_90(int p) { return false; }
    virtual bool set_91(int p) { return false; }
    virtual bool set_92(int p) { return false; }
    virtual bool set_93(int p) { return false; }
    virtual bool set_94(int p) { return false; }
    virtual bool set_95(int p) { return false; }
    virtual bool set_96(int p) { return false; }
    virtual bool set_97(int p) { return false; }
    virtual bool set_98(int p) { return false; }
    virtual bool set_99(int p) { return false; }
    // EBATECH(CNBR) -[ 2002/6/19 VIEWPORT UCSORG/UCSXDIR/UCSYDIR
    virtual bool set_110(sds_point p) { return false; }
    virtual bool set_111(sds_point p) { return false; }
    virtual bool set_112(sds_point p) { return false; }
    // EBATECH(CNBR) ]-
    virtual bool set_140(sds_real p) { return false; }
    virtual bool set_141(sds_real p) { return false; }
    virtual bool set_142(sds_real p) { return false; }
    virtual bool set_143(sds_real p) { return false; }
    virtual bool set_144(sds_real p) { return false; }
    virtual bool set_145(sds_real p) { return false; }
    virtual bool set_146(sds_real p) { return false; }
    virtual bool set_147(sds_real p) { return false; }
    // EBATECH(CNBR) -[ 2002/10/10 PLOTSETTINS PAPERIMAGEORIGIN X,Y
    virtual bool set_148(sds_real p) { return false; }
    virtual bool set_149(sds_real p) { return false; }
    // EBATECH(CNBR) ]-
    virtual bool set_170(int p) { return false; }
    virtual bool set_171(int p) { return false; }
    virtual bool set_172(int p) { return false; }
    virtual bool set_173(int p) { return false; }
    virtual bool set_174(int p) { return false; }
    virtual bool set_175(int p) { return false; }
    virtual bool set_176(int p) { return false; }
    virtual bool set_177(int p) { return false; }
    virtual bool set_178(int p) { return false; }
    virtual bool set_179(int p) { return false; } // EBATECH(CNBR) 2003/2/7 DIMSTYLE/DIMADEC
    virtual bool set_210(sds_point p) { return false; }

	// init_210 is a fast version of set_210 that does not
	// normalize the vector.  Used when reading from file
	//
    virtual bool init_210(sds_point p) { return false; }
    virtual bool set_211(sds_point p) { return false; }
    virtual bool set_212(sds_point p) { return false; }
	//Bugzilla No. 78013 ; 01-04-2002
	virtual bool set_213(sds_point p) { return false; }
    virtual bool set_270(int p) { return false; }
    virtual bool set_271(int p) { return false; }
    virtual bool set_272(int p) { return false; }
    virtual bool set_273(int p) { return false; }
    virtual bool set_274(int p) { return false; }
    virtual bool set_275(int p) { return false; }
    virtual bool set_278(int p) { return false; } // EBATECH(CNBR) 2003/3/31 DIMENSION/DIMDSEP
    virtual bool set_280(int p) { return false; }
    virtual bool set_281(int p) { return false; }
    virtual bool set_282(int p) { return false; }
    virtual bool set_283(int p) { return false; }
    virtual bool set_284(int p) { return false; }
    virtual bool set_285(int p) { return false; }
    virtual bool set_286(int p) { return false; }
    virtual bool set_287(int p) { return false; }
    virtual bool set_288(int p) { return false; }
    virtual bool set_289(int p) { return false; }
    virtual bool set_290(int p) { return false; }
    virtual bool set_291(int p) { return false; }
    virtual bool set_292(int p) { return false; }
    virtual bool set_293(int p) { return false; }
    virtual bool set_294(int p) { return false; }
    virtual bool set_295(int p) { return false; }
    virtual bool set_296(int p) { return false; }
    virtual bool set_297(int p) { return false; }
    virtual bool set_298(int p) { return false; }
    virtual bool set_299(int p) { return false; }
    virtual bool set_300(char *p) { return false; }
    virtual bool set_310(char *p, int n) { return false; }

    virtual bool set_340byname(char *p, db_drawing *dp) { return false; }
    virtual bool set_340byhand(db_objhandle p, db_drawing *dp) { return false; }
    virtual bool set_340(db_handitem *p) { return false; }
    // EBATECH(watanabe)-[dimldrblk
    // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
    //virtual bool set_341(char *p) { return false; }
    //virtual bool set_341(char *p, db_drawing *dp) { return false; }
    virtual bool set_341byname(char *p, db_drawing *dp) { return false; }
    virtual bool set_341byhand(db_objhandle p, db_drawing *dp) { return false; }
    // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
    virtual bool set_341(db_handitem *p) { return false; }
    // ]-EBATECH(watanabe)
    // EBATECH(CNBR) -[ 2002/6/19 VIEWPORT NAMEDUCS/BASEUCS
    virtual bool set_345(db_handitem *p) { return false; }
    virtual bool set_346(db_handitem *p) { return false; }
    // EBATECH(CNBR) ]-
    virtual bool set_360byhand(db_objhandle p, db_drawing *dp) { return false; }
    virtual bool set_360(db_handitem *p) { return false; }
    virtual bool set_370(int p) { return false; }
    virtual bool set_371(int p) { return false; }
    virtual bool set_372(int p) { return false; }
    virtual bool set_373(int p) { return false; }
    virtual bool set_374(int p) { return false; }
    virtual bool set_375(int p) { return false; }
    virtual bool set_376(int p) { return false; }
    virtual bool set_377(int p) { return false; }
    virtual bool set_378(int p) { return false; }
    virtual bool set_379(int p) { return false; }
    virtual bool set_380(int p) { return false; }
    virtual bool set_381(int p) { return false; }
    virtual bool set_382(int p) { return false; }
    virtual bool set_383(int p) { return false; }
    virtual bool set_384(int p) { return false; }
    virtual bool set_385(int p) { return false; }
    virtual bool set_386(int p) { return false; }
    virtual bool set_387(int p) { return false; }
    virtual bool set_388(int p) { return false; }
    virtual bool set_389(int p) { return false; }
    virtual bool set_390(db_handitem *hip) { return false; }
    virtual bool set_391(db_handitem *hip) { return false; }
    virtual bool set_392(db_handitem *hip) { return false; }
    virtual bool set_393(db_handitem *hip) { return false; }
    virtual bool set_394(db_handitem *hip) { return false; }
    virtual bool set_395(db_handitem *hip) { return false; }
    virtual bool set_396(db_handitem *hip) { return false; }
    virtual bool set_397(db_handitem *hip) { return false; }
    virtual bool set_398(db_handitem *hip) { return false; }
    virtual bool set_399(db_handitem *hip) { return false; }

	virtual bool set_400(short s) { return false; }
	virtual bool set_401(short s) { return false; }
	virtual bool set_402(short s) { return false; }
	virtual bool set_403(short s) { return false; }
	virtual bool set_404(short s) { return false; }
	virtual bool set_405(short s) { return false; }
	virtual bool set_406(short s) { return false; }
	virtual bool set_407(short s) { return false; }
	virtual bool set_408(short s) { return false; }
	virtual bool set_409(short s) { return false; }
    virtual bool set_410(char *s) { return false; }
    virtual bool set_411(char *s) { return false; }
    virtual bool set_412(char *s) { return false; }
    virtual bool set_413(char *s) { return false; }
    virtual bool set_414(char *s) { return false; }
    virtual bool set_415(char *s) { return false; }
    virtual bool set_416(char *s) { return false; }
    virtual bool set_417(char *s) { return false; }
    virtual bool set_418(char *s) { return false; }
    virtual bool set_419(char *s) { return false; }
    virtual bool set_410(char *s,db_drawing *dp) { return false; }
    virtual bool set_411(char *s,db_drawing *dp) { return false; }
    virtual bool set_412(char *s,db_drawing *dp) { return false; }
    virtual bool set_413(char *s,db_drawing *dp) { return false; }
    virtual bool set_414(char *s,db_drawing *dp) { return false; }
    virtual bool set_415(char *s,db_drawing *dp) { return false; }
    virtual bool set_416(char *s,db_drawing *dp) { return false; }
    virtual bool set_417(char *s,db_drawing *dp) { return false; }
    virtual bool set_418(char *s,db_drawing *dp) { return false; }
    virtual bool set_419(char *s,db_drawing *dp) { return false; }


    /* Other member functions: */

    struct resbuf *entgetx(const struct resbuf *apps, db_drawing *dp);
	virtual int entgetcommon(  struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
    virtual int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
    virtual int entmod(struct resbuf *modll, db_drawing *dp);
	/* This virtual feeds entmodhi() for OBJECTs (which don't have */
	/* a tablerec/entity level): */
    virtual int entmod1common(struct resbuf **rblinkpp, db_drawing *dp) { return entmodhi(rblinkpp,dp); }
/*-------------------------------------------------------------------------*//**
Checks object for validity and fix errors if possible

@author Denis Petrov
@param pDrawing => pointer to the drawing
@return 1 if object is valid or successfully validated, 0 else or for error
*//*------------------------------------------------------------------------*/
	virtual int check(db_drawing* pDrawing) { return 1; /* no checking by default */ }

    static void delll(db_handitem* ll)
	{
        db_handitem* p;
        while(ll != NULL)
		{
			p = ll->next;
			delete ll;
			ll = p;
		}
	}

    virtual void addent(db_handitem *p, db_drawing *dp) {}  /* See db_blocktabrec */
    virtual void delent(db_handitem *p, db_drawing *dp) {}  /* See db_blocktabrec */

    struct resbuf *get_eed(const struct resbuf *apps, short *rcp);
    int            set_eed(struct resbuf *elist, db_drawing *dp);
	void		   clear_eed() { delete [] eed; eed = NULL; }	/*D.G.*/// I've added this method because of wrong setting new eed in dimensions which already have it.

    int entmodhi(struct resbuf **rblinkpp, db_drawing *dp);

	//Direct access into the reactors. It is useful when external items such as the load and save
	//functions need to access the reactors.
	ReactorSubject* getReactor();

	//addReactor with the fixuploc is only intended to be used on load when the handitems may not be all
	//loaded when they need to be resolved.
	virtual bool addReactor(db_objhandle &handle, db_drawing *drawing, ReactorItem::Type type);
	virtual bool addReactor(db_objhandle &handle, db_handitem *item, ReactorItem::Type type = ReactorItem::SOFT_POINTER);

	// need an attach method that allows duplicate associations.  Temporary entities sometimes need
	// to be created such as savelp in entmod.  This allows undo to have the proper association.
	bool addReactorAllowDups(db_objhandle &handle, db_handitem *item, ReactorItem::Type type);

	bool removeReactor(db_objhandle &removeHandle);

    void RemovePersistentReactors(void);

	//Call Notify on the reactor list
	virtual void CheckReactors(db_drawing *drawing) {m_reactors->Notify(drawing, handle);}

	//*********************************************************************************
	//Associated entity is required to implement ReactorUpdate, BreakAssociation (both)
	//to work properly  CheckAssociationInSet is needed if the association will sometimes be broken
	//when in a associated entities are not in set (ie hatch breaks assoc when hatch is rotated but
	//not boundary.  Leader does not break the assoc).
	//*********************************************************************************

	//Call to notify the handitem that a reactor has changed - some classes will override and implement such
	//as hatch and leader
	virtual bool ReactorUpdate (db_objhandle &reactor) {return true;}

	//Removes associations from hatch and cleans up associations from boundary object to hatch
	virtual void BreakAssociation(bool dispMessage = true) {}

	// breaks the association with the entity mentioned.  Up to the entity to decide whether it has any
	// additional effects.
	virtual void BreakAssociation(db_handitem *entity, bool dispMessage = true) {}

	//Basically, it checks if the associated entities are in the selection set. Some entities like hatch
	//will need to implement this function so it will break the assoc if the boundary is not in the selection
	//set.  Other entities like leader do not break the association on commands like rotate so they do not
	//need implement.
	virtual void CheckAssociationInSet(db_drawing *drawing, const sds_name selSet) {}

	// Gives the user the ability to temporarily turn off the ability to create associations.  This
	// is useful in entities that can be assoicated with multiple entities like hatch.  This is useful during
	// cmds like copy where you don't want to create new associations until the old associations are
	// fixed.
	virtual void SetCreateAssocFlag(bool flag) {m_createAssocFlag = flag;}

	virtual bool notifyDeleted(db_handitem* hip) { return false;}

    void del_xdict(void) {
        delete xdict;
        xdict=NULL;
    }

	db_hireflink *ret_xdict(void) { return xdict; }
    void set_xdict(int type, db_objhandle hand, db_handitem *hip) {
        delete xdict; xdict=new db_hireflink(type,hand,hip);
    }

	bool Has1005Eed(db_drawing *dp);


	// **********************************
	// Undo specific methods for eed
	//
public:
	// Are they the same without looking inside
	//
	bool equal_eed( db_handitem *pOther );

	// Make a copy of the eed data and return in an opaque format
	//
	void *dup_eed( void );

	// Restore the eed from a copy returned by dup_eed.  Eed already in the object
	// will be cleaned-up/free
	//
	bool restore_dup_eed( void *pdupeed );

	// Let this class deal with cleaning up
	//
	static bool free_dup_eed( void *pdupeed );


	// ******************************
	// All handle methods
	//
public:
    void assignhand(db_drawing *dp);

	// gets
    db_objhandle RetHandle(void) const
		{
		return handle;
		}

	db_objhandle * RetHandlePtr(void)
		{
		return &handle;
		}

	// sets
    void SetHandle( db_objhandle newhandle)
		{
		handle=newhandle;
		}

	// * end of handle methods

    /* Public data ( fix this somehow): */


	public:

    db_handitem *next;  /* For llists */

	// If this object is in block definition this pointer is set to appropriate db_block object /AG/
	// If this object is db_entity or db_blocktabrec (except for Xref) and belongs to Xref 
	//  then this pointer points to db_blocktabrec which keeps this Xref
	db_handitem* m_parentBlock;

	static char *db_preactmarker;	

	// Private Data
protected:
	ReactorSubject *m_reactors;	// List of reactors
	bool m_createAssocFlag;		// Allows certain functions to stop creating associations;
								// this is useful for functions like copy or wblock so the
								// associations can be fixed up before new associations are made.


private:
    db_hireflink *xdict;        /* Ownership ptr to extension dictionary. */
    char         *eed;          /* 1000+ (R12 internal, with R13 strings and */
                                /*   hip's in place of table idx's) */
    db_objhandle  handle;
    char          type;         /* 0 : The DB_type. */
	// Ebatech(cnbr) 2002/10/10
	//db_handitem	*m_OwenerId;	/* 330 : Owener Object Pointer */
};
#pragma pack(pop)
#endif

