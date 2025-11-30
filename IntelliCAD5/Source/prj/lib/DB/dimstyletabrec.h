/* C:\ICAD\PRJ\LIB\DB\DIMSTYLETABREC.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#ifndef _INC_DIMSTYLETABREC
#define _INC_DIMSTYLETABREC

class DB_CLASS db_dimstyletabrec : public db_tablerecord {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_dimstyletabrec(void);
    db_dimstyletabrec(char *pname, db_drawing *dp);
    db_dimstyletabrec(const db_dimstyletabrec &sour);  /* Copy constructor */
   ~db_dimstyletabrec(void);

	/*** GET or RET ***/

    bool get_3(char **p) { *p=post; return true; }
    bool get_3(char *p, int maxlen) {
        if (post==NULL) *p=0;
        else { strncpy(p,post,maxlen); p[maxlen]=0; }
        return true;
    }
    bool get_4(char **p) { *p=apost; return true; }
    bool get_4(char *p, int maxlen) {
        if (apost==NULL) *p=0;
        else { strncpy(p,apost,maxlen); p[maxlen]=0; }
        return true;
    }
    bool get_5(char **p) { *p=blk; return true; }
    bool get_5(char *p, int maxlen) {
        if (blk==NULL) *p=0;
        else { strncpy(p,blk,maxlen); p[maxlen]=0; }
        return true;
    }
    bool get_6(char **p) { *p=blk1; return true; }
    bool get_6(char *p, int maxlen) {
        if (blk1==NULL) *p=0;
        else { strncpy(p,blk1,maxlen); p[maxlen]=0; }
        return true;
    }
    bool get_7(char **p) { *p=blk2; return true; }
    bool get_7(char *p, int maxlen) {
        if (blk2==NULL) *p=0;
        else { strncpy(p,blk2,maxlen); p[maxlen]=0; }
        return true;
    }

    // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
    //// EBATECH(watanabe)-[dimldrblk
    //bool get_341(char **p) { *p=ldrblk; return true; }
    //bool get_341(char *p, int maxlen) {
    //    if (ldrblk==NULL) *p=0;
    //    else { strncpy(p,ldrblk,maxlen); p[maxlen]=0; }
    //    return true;
    //}
    bool get_341name(char **p)
      {
        if (ldrblk)
          {
            ldrblk->get_2(p);
          }
        else
          {
            *p = db_str_quotequote;
          }
        return true;
      }

    bool get_341name(char *p, int maxlen)
      {
        if (ldrblk)
          {
            ldrblk->get_2(p, maxlen);
          }
        else
          {
            strncpy(p, db_str_quotequote, maxlen);
          }
        return true;
      }

    bool get_341(db_handitem **p)
      {
        *p = ldrblk;
        return true;
      }
    //// ]-EBATECH(watanabe)
    // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED

    bool get_40(sds_real *p) { *p=scale; return true; }
    bool get_41(sds_real *p) { *p=asz  ; return true; }
    bool get_42(sds_real *p) { *p=exo  ; return true; }
    bool get_43(sds_real *p) { *p=dli  ; return true; }
    bool get_44(sds_real *p) { *p=exe  ; return true; }
    bool get_45(sds_real *p) { *p=rnd  ; return true; }
    bool get_46(sds_real *p) { *p=dle  ; return true; }
    bool get_47(sds_real *p) { *p=tp   ; return true; }
    bool get_48(sds_real *p) { *p=tm   ; return true; }

    bool get_71(int *p) { *p=tol; return true; }
    bool get_72(int *p) { *p=lim; return true; }
    bool get_73(int *p) { *p=tih; return true; }
    bool get_74(int *p) { *p=toh; return true; }
    bool get_75(int *p) { *p=se1; return true; }
    bool get_76(int *p) { *p=se2; return true; }
    bool get_77(int *p) { *p=tad; return true; }
    bool get_78(int *p) { *p=zin; return true; }
	bool get_79(int *p) { *p=azin; return true; } // EBATECH(CNBR) Bugzilla#78443 2003/2/7

    bool get_140(sds_real *p) { *p=txt ; return true; }
    bool get_141(sds_real *p) { *p=cen ; return true; }
    bool get_142(sds_real *p) { *p=tsz ; return true; }
    bool get_143(sds_real *p) { *p=altf; return true; }
    bool get_144(sds_real *p) { *p=lfac; return true; }
    bool get_145(sds_real *p) { *p=tvp ; return true; }
    bool get_146(sds_real *p) { *p=tfac; return true; }
    bool get_147(sds_real *p) { *p=gap ; return true; }
	bool get_148(sds_real *p) { *p=altrnd; return true; } // EBATECH(CNBR) 2003/3/31

    bool get_170(int *p) { *p=alt ; return true; }
    bool get_171(int *p) { *p=altd; return true; }
    bool get_172(int *p) { *p=tofl; return true; }
    bool get_173(int *p) { *p=sah ; return true; }
    bool get_174(int *p) { *p=tix ; return true; }
    bool get_175(int *p) { *p=soxd; return true; }

    bool get_176(int *p) { *p=clrd; return true; }
    bool get_177(int *p) { *p=clre; return true; }
    bool get_178(int *p) { *p=clrt; return true; }
	bool get_179(int *p) { *p=adec; return true; } // EBATECH(CNBR) Bugzilla#78443 2003/2/7

    bool get_270(int *p) { *p=unit ; return true; }
    bool get_271(int *p) { *p=dec  ; return true; }
    bool get_272(int *p) { *p=tdec ; return true; }
    bool get_273(int *p) { *p=altu ; return true; }
    bool get_274(int *p) { *p=alttd; return true; }
    bool get_275(int *p) { *p=aunit; return true; }

    // EBATECH(watanabe)-[dimfrac
    bool get_276(int *p) { *p=frac ; return true; }
    // ]-EBATECH(watanabe)

    // EBATECH(watanabe)-[dimlunit
    bool get_277(int *p) { *p=lunit; return true; }
    // ]-EBATECH(watanabe)

    bool get_278(int *p) { *p=( dsep & 0x000000ff); return true; } // EBATECH(CNBR) 2003/3/31

    // EBATECH(watanabe)-[dimtmove
    bool get_279(int *p) { *p=tmove; return true; }
    // ]-EBATECH(watanabe)

    bool get_280(int *p) { *p=just ; return true; }
    bool get_281(int *p) { *p=sd1  ; return true; }
    bool get_282(int *p) { *p=sd2  ; return true; }
    bool get_283(int *p) { *p=tolj ; return true; }
    bool get_284(int *p) { *p=tzin ; return true; }
    bool get_285(int *p) { *p=altz ; return true; }
    bool get_286(int *p) { *p=alttz; return true; }
    bool get_287(int *p) { *p=fit  ; return true; }
    bool get_288(int *p) { *p=upt  ; return true; }

    // EBATECH(watanabe)-[dimatfit
    bool get_289(int *p) { *p=atfit; return true; }
    // ]-EBATECH(watanabe)

    bool get_340name(char **p)
    {
		if (txsty!=NULL) {
			txsty->get_2(p);
		}
		else {
			*p=db_str_quotequote;
		}
		return true;
	}
    bool get_340name(char *p, int maxlen)
        { if (txsty==NULL) *p=0; else txsty->get_2(p,maxlen); return true; }
    bool get_340(db_handitem **p) { *p=txsty; return true; }
	db_handitem  *ret_txsty(void) { return txsty; }

	// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight
	bool get_371(int *p) { *p=lwd; return true; }
	bool get_372(int *p) { *p=lwe; return true; }
	// EBATECH(CNBR) ]-

	/*** SET ***/

    bool set_3(char *p) {
        db_astrncpy(&post,(p==NULL) ? db_str_quotequote : p,
            db_oldsysvar[DB_QDIMPOST].nc-1);
        return true;
    }
    bool set_4(char *p) {
        db_astrncpy(&apost,(p==NULL) ? db_str_quotequote : p,
            db_oldsysvar[DB_QDIMAPOST].nc-1);
        return true;
    }
    bool set_5(char *p) {
        db_astrncpy(&blk,(p==NULL) ? db_str_quotequote : p,IC_ACADNMLEN-1);
        return true;
    }
    bool set_6(char *p) {
        db_astrncpy(&blk1,(p==NULL) ? db_str_quotequote : p,IC_ACADNMLEN-1);
        return true;
    }
    bool set_7(char *p) {
        db_astrncpy(&blk2,(p==NULL) ? db_str_quotequote : p,IC_ACADNMLEN-1);
        return true;
    }

    // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
    //// EBATECH(watanabe)-[dimldrblk
    //bool set_341(char *p) {
    //    db_astrncpy(&ldrblk,(p==NULL) ? db_str_quotequote : p,IC_ACADNMLEN-1);
    //    return true;
    //}
    bool set_341byname(char *p, db_drawing *dp)
      {
        if (dp == NULL)
          {
            // invalid drawing
            db_lasterror = OL_ESNVALID;
            return false;
          }

        if (p)
          {
            if (*p)
              {
                db_handitem *thip1 = dp->findtabrec(DB_BLOCKTAB, p, 1);
                if (thip1)
                  {
                    ldrblk = thip1;
                    return true;
                  }
                else
                  {
                    // invalid block name
                    db_lasterror = OL_ESNVALID;
                    return false;
                  }
              }
          }

        // arrow type is closed-filled
        ldrblk = NULL;
        return true;
      }

    bool set_341byhand(db_objhandle p, db_drawing *dp)
      {
        db_handitem* thip1 = dp->handent(p);
        if (thip1 == NULL)
          {
            // invalid arrow type
            db_lasterror = OL_EHANDLE;
            return false;
          }
        ldrblk = thip1;
        return true;
      }

    bool set_341(db_handitem *p)
      {
        ldrblk = p;
        return true;
      }
    //// ]-EBATECH(watanabe)
    // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED

    bool set_40(sds_real p) { scale=(p>=0.0) ? p : -p; return true; }  /* ACAD allows 0.0. */
    bool set_41(sds_real p) { asz  =(p>=0.0) ? p : -p; return true; }
    bool set_42(sds_real p) { exo  =(p>=0.0) ? p : -p; return true; }
    bool set_43(sds_real p) { dli  =(p>=0.0) ? p : -p; return true; }
    bool set_44(sds_real p) { exe  =(p>=0.0) ? p : -p; return true; }
    bool set_45(sds_real p) { rnd  =(p>=0.0) ? p : -p; return true; }
    bool set_46(sds_real p) { dle  =(p>=0.0) ? p : -p; return true; }
    bool set_47(sds_real p) { tp   =p;                 return true; }
    bool set_48(sds_real p) { tm   =p;                 return true; }

    bool set_71(int p) { tol=(p!=0); return true; }
    bool set_72(int p) { lim=(p!=0); return true; }
    bool set_73(int p) { tih=(p!=0); return true; }
    bool set_74(int p) { toh=(p!=0); return true; }
    bool set_75(int p) { se1=(p!=0); return true; }
    bool set_76(int p) { se2=(p!=0); return true; }
    bool set_77(int p) { tad=charCast((p<0) ? 0 : ((p< 3) ? (p) :  3)); return true; }
    bool set_78(int p) { zin=charCast((p<0) ? 0 : ((p<15) ? (char)(p) : 15)); return true; }
    bool set_79(int p) { azin=charCast((p<0) ? 0 : ((p<4) ? p : 3)); return true; } // EBATECH(CNBR) Bugzilla#78443 2003/2/7

    bool set_140(sds_real p) { txt=(p>0.0) ? p : ((p<0.0) ? -p : atof(db_oldsysvar[DB_QDIMTXT].defval)); return true; }
    bool set_141(sds_real p) { cen=p; return true; }
    bool set_142(sds_real p) { tsz=(p>=0.0) ? p : -p; return true; }
    bool set_143(sds_real p) { altf=(p>0.0) ? p : ((p<0.0) ? -p : atof(db_oldsysvar[DB_QDIMALTF].defval)); return true; }
    bool set_144(sds_real p) { lfac=(!icadRealEqual(p,0.0)) ? p : atof(db_oldsysvar[DB_QDIMLFAC].defval); return true; }
    bool set_145(sds_real p) { tvp=p; return true; }
    bool set_146(sds_real p) { tfac=(p>0.0) ? p : ((p<0.0) ? -p : atof(db_oldsysvar[DB_QDIMTFAC].defval)); return true; }
    bool set_147(sds_real p) { gap=p; return true; }
    bool set_148(sds_real p) { altrnd  =(p>=0.0) ? p : -p; return true; } // EBATECH(CNBR) 2003/3/31

    bool set_170(int p) { alt =(p!=0); return true; }
    bool set_171(int p) { altd=charCast((p<0) ? 0 : ((p<8) ? p : 8)); return true; }
    bool set_172(int p) { tofl=charCast((p!=0)); return true; }
    bool set_173(int p) { sah =charCast((p!=0)); return true; }
    bool set_174(int p) { tix =charCast((p!=0)); return true; }
    bool set_175(int p) { soxd=charCast((p!=0)); return true; }

    bool set_176(int p) { clrd=(p<0) ? 0 : ((p<256) ? p : 256); return true; }
    bool set_177(int p) { clre=(p<0) ? 0 : ((p<256) ? p : 256); return true; }
    bool set_178(int p) { clrt=(p<0) ? 0 : ((p<256) ? p : 256); return true; }
    bool set_179(int p) { adec=charCast((p<-1) ? 0 : ((p<9) ? p : 8)); return true; } // EBATECH(CNBR) Bugzilla#78443 2003/2/7

	//Modified Cybage SBD 16/03/2001 DD/MM/YYYY  
	//Reason : Fix for Bug No.77759 from Bugzilla 
	//bool set_270(int p) { unit =charCast((p<1) ? 1 : ((p<7) ? p : 7)); return true; }

    // EBATECH(watanabe)-[dimunit -> dimlunit, dimfrac
	//bool set_270(int p) { unit =charCast((p<1) ? 1 : ((p<8) ? p : 8)); return true; }
	bool set_270(int p) {
		unit =charCast((p<1) ? 1 : ((p<8) ? p : 8));
        lunit = unit;
        if (unit >= 6)
          {
            lunit -= 2;
          }
        switch (unit)
          {
            case 4:
            case 5:
            case 6:
            case 7:
                frac = 0;
                if ((unit == 6) ||
                    (unit == 7))
                  {
                    frac = 2;
                  }
                break;
          }
		return true;
	}
    // ]-EBATECH(watanabe)
    bool set_271(int p) { dec  =charCast((p<0) ? 0 : ((p<8) ? p : 8)); return true; }
    bool set_272(int p) { tdec =charCast((p<0) ? 0 : ((p<8) ? p : 8)); return true; }
    bool set_273(int p) { altu =charCast((p<1) ? 1 : ((p<7) ? p : 7)); return true; }
    bool set_274(int p) { alttd=charCast((p<0) ? 0 : ((p<8) ? p : 8)); return true; }
    bool set_275(int p) { aunit=charCast((p<0) ? 0 : ((p<4) ? p : 4)); return true; }

    // EBATECH(watanabe)-[dimfrac
    bool set_276(int p) {
        frac =charCast((p<0) ? 0 : ((p<2) ? p : 2));
        unit =lunit;
        switch (lunit)
          {
            case 4:
            case 5:
                if (frac == 2)
                  {
                    unit += 2;
                  }
                break;
            case 6:
                unit += 2;
                break;
        }
        return true;
    }
    // ]-EBATECH(watanabe)

    // EBATECH(watanabe)-[dimlunit
    bool set_277(int p) {
        lunit=charCast((p<1) ? 1 : ((p<6) ? p : 6));
        unit =lunit;
        switch (lunit)
          {
            case 4:
            case 5:
                if (frac == 2)
                  {
                    unit += 2;
                  }
                break;
            case 6:
                unit += 2;
                break;
          }
        return true;
    }
    // ]-EBATECH(watanabe)

    bool set_278(int p) { dsep=charCast(p); return true; } // EBATECH(CNBR) 2003/3/31

    // EBATECH(watanabe)-[dimtmove
    bool set_279(int p) {
        tmove=charCast((p<0) ? 0 : ((p<2) ? p : 2));
        fit = atfit;
        if (tmove == 1)
          {
            fit = 4;
          }
        else if (tmove == 2)
          {
            fit = 5;
          }
        return true;
    }
    // ]-EBATECH(watanabe)
    bool set_280(int p) { just =charCast((p<0) ? 0 : ((p<4) ? p : 4)); return true; }
    bool set_281(int p) { sd1  =(p!=0); return true; }
    bool set_282(int p) { sd2  =(p!=0); return true; }
    bool set_283(int p) { tolj =charCast((p<0) ? 0 : ((p<2) ? p : 2)); return true; }
    bool set_284(int p) { tzin =charCast((p<0) ? 0 : ((p<15) ? p : 15)); return true; }
    bool set_285(int p) { altz =charCast((p<0) ? 0 : ((p<15) ? p : 15)); return true; }
    bool set_286(int p) { alttz=charCast((p<0) ? 0 : ((p<15) ? p : 15)); return true; }

    // EBATECH(watanabe)-[dimfit -> dimatfit, dimtmove
    //bool set_287(int p) { fit  =charCast((p<0) ? 0 : ((p<5) ? p : 5)); return true; }
    bool set_287(int p) {
        fit  =charCast((p<0) ? 0 : ((p<5) ? p : 5));
        atfit = fit;
        tmove = 0;
        if (fit > 3)
          {
            atfit = 3;
            tmove = fit - 3;
          }
        return true;
    }
    // ]-EBATECH(watanabe)

    bool set_288(int p) { upt  =charCast((p!=0)); return true; }

    // EBATECH(watanabe)-[dimatfit
    bool set_289(int p) {
        atfit=charCast((p<0) ? 0 : ((p<3) ? p : 3));
        fit = atfit;
        if (tmove == 1)
          {
            fit = 4;
          }
        else if (tmove == 2)
          {
            fit = 5;
          }
        return true;
    }
    // ]-EBATECH(watanabe)

	bool set_340byname(char *p, db_drawing *dp) {
		db_handitem *thip1;

	    if (p==NULL || !*p || dp==NULL ||
		    (thip1=dp->findtabrec(DB_STYLETAB,p,1))==NULL)
			    { db_lasterror=OL_ESNVALID; return false; }
	    txsty=thip1; return true;
	}

	bool set_340byhand(db_objhandle p, db_drawing *dp) {
	    db_handitem *thip1;

	    if ((thip1=dp->handent(p))==NULL)
		    { db_lasterror=OL_EHANDLE; return false; }
	    txsty=thip1; return true;
	}

    bool set_340(db_handitem *p) { txsty=p; return true; }

	// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight
	bool set_371(int p) { lwd=p; return true; }
	bool set_372(int p) { lwe=p; return true; }
	// EBATECH(CNBR) ]-

  private:

    char              *post;   /*   3 */
    char              *apost;  /*   4 */
    char              *blk;    /*   5 */
    char              *blk1;   /*   6 */
    char              *blk2;   /*   7 */

    // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
    db_handitem       *ldrblk; /* 341 */
    // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED

    sds_real           scale;  /*  40 */
    sds_real           asz;    /*  41 */
    sds_real           exo;    /*  42 */
    sds_real           dli;    /*  43 */
    sds_real           exe;    /*  44 */
    sds_real           rnd;    /*  45 */
    sds_real           dle;    /*  46 */
    sds_real           tp;     /*  47 */
    sds_real           tm;     /*  48 */

    char               tol;    /*  71 */
    char               lim;    /*  72 */
    char               tih;    /*  73 */
    char               toh;    /*  74 */
    char               se1;    /*  75 */
    char               se2;    /*  76 */
    char               tad;    /*  77 */
    char               zin;    /*  78 */
	char               azin;   /*  79 */// EBATECH(CNBR) Bugzilla#78443 2003/2/7
	
    sds_real           txt;    /* 140 */
    sds_real           cen;    /* 141 */
    sds_real           tsz;    /* 142 */
    sds_real           altf;   /* 143 */
    sds_real           lfac;   /* 144 */
    sds_real           tvp;    /* 145 */
    sds_real           tfac;   /* 146 */
    sds_real           gap;    /* 147 */
	sds_real           altrnd; /* 148 */// EBATECH(CNBR) 2003/3/31 

    char               alt;    /* 170 */
    char               altd;   /* 171 */
    char               tofl;   /* 172 */
    char               sah;    /* 173 */
    char               tix;    /* 174 */
    char               soxd;   /* 175 */

    int                clrd;   /* 176 */
    int                clre;   /* 177 */
    int                clrt;   /* 178 */
	char               adec;   /* 179 */// EBATECH(CNBR) Bugzilla#78443 2003/2/7

    char               unit;   /* 270 */
    char               dec;    /* 271 */
    char               tdec;   /* 272 */
    char               altu;   /* 273 */
    char               alttd;  /* 274 */
    char               aunit;  /* 275 */

    // EBATECH(watanabe)-[dimfrac
    char               frac;   /* 276 */
    // ]-EBATECH(watanabe)

    // EBATECH(watanabe)-[dimlunit
    char               lunit;  /* 277 */
    // ]-EBATECH(watanabe)

	char               dsep;   /* 278 */// EBATECH(CNBR) 2003/3/31

    // EBATECH(watanabe)-[dimtmove
    char               tmove;  /* 279 */
    // ]-EBATECH(watanabe)

    char               just;   /* 280 */
    char               sd1;    /* 281 */
    char               sd2;    /* 282 */
    char               tolj;   /* 283 */
    char               tzin;   /* 284 */
    char               altz;   /* 285 */
    char               alttz;  /* 286 */
    char               fit;    /* 287 */
    char               upt;    /* 288 */

    // EBATECH(watanabe)-[dimatfit
    char               atfit;  /* 289 */
    // ]-EBATECH(watanabe)

    db_handitem       *txsty;  /* 340 */
	// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight
	int               lwd;     /* 371 */
	int               lwe;     /* 372 */
	// EBATECH(CNBR) ]-
};

#endif

