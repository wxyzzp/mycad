/* C:\ICAD\PRJ\LIB\DB\VPORTTABREC.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#ifndef _INC_VPORTTABREC
#define _INC_VPORTTABREC

class DB_CLASS db_vporttabrec : public db_tablerecord {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_vporttabrec(void);
    db_vporttabrec(char *pname, db_drawing *dp);
    db_vporttabrec(const db_vporttabrec &sour);  /* Copy constructor */

    bool get_10(sds_point p) { p[0]=corner[0][0]; p[1]=corner[0][1]; p[2]=0.0;       return true; }
    bool get_11(sds_point p) { p[0]=corner[1][0]; p[1]=corner[1][1]; p[2]=0.0;       return true; }
    bool get_12(sds_point p) { p[0]=cent[0]     ; p[1]=cent[1]     ; p[2]=0.0;       return true; }
    bool get_13(sds_point p) { p[0]=snapbase[0];; p[1]=snapbase[1] ; p[2]=0.0;       return true; }
    bool get_14(sds_point p) { p[0]=snapspace[0]; p[1]=snapspace[1]; p[2]=0.0;       return true; }
    bool get_15(sds_point p) { p[0]=gridspace[0]; p[1]=gridspace[1]; p[2]=0.0;       return true; }
    bool get_16(sds_point p) { p[0]=dir[0]      ; p[1]=dir[1]      ; p[2]=dir[2];    return true; }
    bool get_17(sds_point p) { p[0]=target[0]   ; p[1]=target[1]   ; p[2]=target[2]; return true; }
    bool get_40(sds_real *p) { *p=ht     ; return true; }
    bool get_41(sds_real *p) { *p=asp    ; return true; }
    bool get_42(sds_real *p) { *p=lenslen; return true; }
    bool get_43(sds_real *p) { *p=clip[0]; return true; }
    bool get_44(sds_real *p) { *p=clip[1]; return true; }
    bool get_50(sds_real *p) { *p=snaprot; return true; }
    bool get_51(sds_real *p) { *p=twist  ; return true; }
    bool get_68(int *p) { *p=stat       ; return true; }
    bool get_69(int *p) { *p=id         ; return true; }
    bool get_71(int *p) { *p=mode       ; return true; }
    bool get_72(int *p) { *p=czoom      ; return true; }
    bool get_73(int *p) { *p=fzoom      ; return true; }
    bool get_74(int *p) { *p=ucsicon    ; return true; }
    bool get_75(int *p) { *p=snap       ; return true; }
    bool get_76(int *p) { *p=grid       ; return true; }
    bool get_77(int *p) { *p=snapstyl   ; return true; }
    bool get_78(int *p) { *p=snapisopair; return true; }

    bool set_10(sds_point p) {
		ASSERT(p[0]>=0.0 && p[0]<=1.0);
		ASSERT(p[1]>=0.0 && p[1]<=1.0);
        corner[0][0]=(p[0]<0.0) ? 0.0 : ((p[0]<1.0) ? p[0] : 1.0);
        corner[0][1]=(p[1]<0.0) ? 0.0 : ((p[1]<1.0) ? p[1] : 1.0);
        return true;
    }
    bool set_11(sds_point p) {
		ASSERT(p[0]>=0.0 && p[0]<=1.0);
		ASSERT(p[1]>=0.0 && p[1]<=1.0);
        corner[1][0]=(p[0]<0.0) ? 0.0 : ((p[0]<1.0) ? p[0] : 1.0);
        corner[1][1]=(p[1]<0.0) ? 0.0 : ((p[1]<1.0) ? p[1] : 1.0);
        return true;
    }
    bool set_12(sds_point p) { cent[0]     =p[0]; cent[1]     =p[1]; return true; }
    bool set_13(sds_point p) { snapbase[0] =p[0]; snapbase[1] =p[1]; return true; }
    bool set_14(sds_point p) { snapspace[0]=p[0]; snapspace[1]=p[1]; return true; }
    bool set_15(sds_point p) { gridspace[0]=p[0]; gridspace[1]=p[1]; return true; }
    bool set_16(sds_point p) { dir[0]   =p[0]; dir[1]   =p[1]; dir[2]   =p[2]; return true; }
    bool set_17(sds_point p) { target[0]=p[0]; target[1]=p[1]; target[2]=p[2]; return true; }
    bool set_40(sds_real p) { ht     =(p>0.0) ? p : ((p<0.0) ? -p : atof(db_oldsysvar[DB_QVIEWSIZE].defval)); return true; }
    bool set_41(sds_real p) { asp    =(p>0.0) ? p : ((p<0.0) ? -p : 1.5); return true; }
    bool set_42(sds_real p) { lenslen=(p>0.0) ? p : ((p<0.0) ? -p : atof(db_oldsysvar[DB_QLENSLENGTH].defval)); return true; }
    bool set_43(sds_real p) { clip[0]=p; return true; }
    bool set_44(sds_real p) { clip[1]=p; return true; }
    bool set_50(sds_real p) { snaprot=p; return true; }
    bool set_51(sds_real p) { twist  =p; return true; }
    bool set_68(int p) { stat=p; return true; }
    bool set_69(int p) { id=p; return true; }
    bool set_71(int p) { mode=p; return true; }
    bool set_72(int p) { czoom=((p<1) ? 1 : ((p<20000) ? p : 20000)); return true; }
    bool set_73(int p) { fzoom=(char)p; return true; }
    bool set_74(int p) { ucsicon=(char)((p>-1 && p<4) ? p : atoi(db_oldsysvar[DB_QUCSICON].defval)); return true; }
    bool set_75(int p) { snap=(p!=0); return true; }
    bool set_76(int p) { grid=(p!=0); return true; }
    bool set_77(int p) { snapstyl=(p!=0); return true; }
    bool set_78(int p) { snapisopair=(char)((p>-1 && p<3) ? p : atoi(db_oldsysvar[DB_QSNAPISOPAIR].defval)); return true; }

	bool get_original_viewport (db_handitem **vp);
	bool set_original_viewport (db_handitem *vp);

  private:

    sds_real  corner[2][2];  /* 10, 11 */
    sds_real  cent[2];       /* 12 */
    sds_real  snapbase[2];   /* 13 */
    sds_real  snapspace[2];  /* 14 */
    sds_real  gridspace[2];  /* 15 */
    sds_point dir;           /* 16 */
    sds_point target;        /* 17 */
    sds_real  ht;            /* 40 */
    sds_real  asp;           /* 41 */
    sds_real  lenslen;       /* 42 */
    sds_real  clip[2];       /* 43, 44 */
    sds_real  snaprot;       /* 50 */
    sds_real  twist;         /* 51 */
    int       stat;          /* 68 */
    int       id;            /* 69 */
    int       mode;          /* 71 */
    int       czoom;         /* 72 */
    char      fzoom;         /* 73 */
    char      ucsicon;       /* 74 */
    char      snap;          /* 75 */
    char      grid;          /* 76 */
    char      snapstyl;      /* 77 */
    char      snapisopair;   /* 78 */

	db_handitem *original_vport;	//When a vport configuration is made active, new db_viewports are created that are
									//copies of the original db_viewports.  The original viewports are untouched, but the
									//new ones are renamed to *ACTIVE.  These active viewports are the ones that are displayed
									//This pointer connects a *ACTIVE viewport to its original viewport.
};

#endif

