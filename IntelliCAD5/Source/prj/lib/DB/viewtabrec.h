/* C:\ICAD\PRJ\LIB\DB\VIEWTABREC.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

class DB_CLASS db_viewtabrec : public db_tablerecord {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_viewtabrec(void);
    db_viewtabrec(char *pname, db_drawing *dp);
    db_viewtabrec(const db_viewtabrec &sour);  /* Copy constructor */

    bool get_10(sds_point p) { p[0]=cent[0]  ; p[1]=cent[1]  ; p[2]=0.0;       return true; }
    bool get_11(sds_point p) { p[0]=dir[0]   ; p[1]=dir[1]   ; p[2]=dir[2];    return true; }
    bool get_12(sds_point p) { p[0]=target[0]; p[1]=target[1]; p[2]=target[2]; return true; }
    bool get_40(sds_real *p) { *p=ht     ; return true; }
    bool get_41(sds_real *p) { *p=wd     ; return true; }
    bool get_42(sds_real *p) { *p=lenslen; return true; }
    bool get_43(sds_real *p) { *p=clip[0]; return true; }
    bool get_44(sds_real *p) { *p=clip[1]; return true; }
    bool get_50(sds_real *p) { *p=twist  ; return true; }
    bool get_71(int *p) { *p=mode; return true; }
    bool get_345(db_handitem **p) { *p = ucstabrec; return true; }

    bool set_10(sds_point p) { cent[0]  =p[0]; cent[1]  =p[1]; return true; }
    bool set_11(sds_point p) { dir[0]   =p[0]; dir[1]   =p[1]; dir[2]   =p[2]; return true; }
    bool set_12(sds_point p) { target[0]=p[0]; target[1]=p[1]; target[2]=p[2]; return true; }
    bool set_40(sds_real p) { ht     =(p>0.0) ? p : ((p<0.0) ? -p : atof(db_oldsysvar[DB_QVIEWSIZE].defval)); return true; }
    bool set_41(sds_real p) { wd     =(p>0.0) ? p : ((p<0.0) ? -p : 1.5*ht); return true; }
    bool set_42(sds_real p) { lenslen=(p>0.0) ? p : ((p<0.0) ? -p : atof(db_oldsysvar[DB_QLENSLENGTH].defval)); return true; }
    bool set_43(sds_real p) { clip[0]=p; return true; }
    bool set_44(sds_real p) { clip[1]=p; return true; }
    bool set_50(sds_real p) { twist  =p; return true; }
    bool set_71(int p) { mode=p; return true; }
	bool set_345(db_handitem* p) { ucstabrec = p; return true; }
  private:

	db_handitem* ucstabrec;	/* 345*/
    sds_real  cent[2];  /* 10 */
    sds_point dir;      /* 11 */
    sds_point target;   /* 12 */
    sds_real  ht;       /* 40 */
    sds_real  wd;       /* 41 */
    sds_real  lenslen;  /* 42 */
    sds_real  clip[2];  /* 43, 44 */
    sds_real  twist;    /* 50 */
    int       mode;     /* 71 */
    int       unk;  /* Unknown short in pre-R13 DWG; may flag non-plan view. */
};


