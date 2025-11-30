/* C:\ICAD\PRJ\LIB\DB\STYLETABREC.H
 * Copyright (C) 1988-1999 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#ifndef	_STYLETABREC_H
#define	_STYLETABREC_H

class DB_CLASS db_styletabrec : public db_tablerecord {

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

	db_styletabrec(void);
	db_styletabrec(char *pname, db_drawing *dp);
	db_styletabrec(const db_styletabrec &sour);  /* Copy constructor */
   ~db_styletabrec(void);

	db_fontlink *ret_font(void)    { return font;	 }
	db_fontlink *ret_bigfont(void) { return bigfont; }
	void		 set_font(db_fontlink *p)	 {	  font=p; }
	void		 set_bigfont(db_fontlink *p) { bigfont=p; }

	bool get_3(char **p) { *p=fontpn; return true; }
	bool get_3(char *p, int maxlen) {
		if (fontpn==NULL) *p=0;
		else { strncpy(p,fontpn,maxlen); p[maxlen]=0; }
		return true;
	}
	bool get_4(char **p) { *p=bigfontpn; return true; }
	bool get_4(char *p, int maxlen) {
		if (bigfontpn==NULL) *p=0;
		else { strncpy(p,bigfontpn,maxlen); p[maxlen]=0; }
		return true;
	}
	bool get_40(sds_real *p) { *p=fixedht; return true; }
	bool get_41(sds_real *p) { *p=xscl; return true; }
	bool get_42(sds_real *p) { *p=lastht; return true; }
	bool get_50(sds_real *p) { *p=obl; return true; }
	bool get_71(int *p) { *p=gen; return true; }

	double ret_fixedht(void) { return fixedht; }
	double ret_xscl(void) { return xscl; }
	double ret_lastht(void) { return lastht; }
	double ret_obl(void) { return obl; }
	int ret_gen(void) { return gen; }
	char *retp_fontpn(void) { return fontpn; }
	char *retp_bigfontpn(void) { return bigfontpn; }

	bool set_3(char *p, db_drawing *dp) {
		/*D.G.*/// Exclude the path from the font file name.
		char*	pFileName = strrchr(p, (int)'\\');
		if(pFileName)
			++pFileName;
		else
			pFileName = p;
		db_astrncpy(&fontpn, pFileName/*p*/, DB_PATHNAMESZ);  // was copying db_str_quotequote
		font=NULL;
		if (pFileName/*p*/ && *pFileName/*p*/ && dp!=NULL) {
			db_fontlink *tfp;
			db_fontsubstmode fsm;

			if (this->ret_flags() & IC_SHAPEFILE_SHAPES) 
			{	// Keep full path for shape files (as opposed to font files)
				fsm=IC_NO_ALLOW_ICAD_FNT;
				pFileName = p;
			}
			else 
				fsm=IC_ALLOW_ICAD_FNT;

			if (db_setfontusingmap(pFileName/*p*/, NULL, &tfp, fsm, dp) >= 0)
				{ font=tfp; db_astrncpy(&fontpn, pFileName/*p*/, DB_PATHNAMESZ); }
		}
		return true;
	}
	bool set_4(char *p, db_drawing *dp) {
		db_astrncpy(&bigfontpn,p,DB_PATHNAMESZ);  // was copying db_str_quotequote
		bigfont=NULL;
		if (p!=NULL && *p && dp!=NULL) {
			db_fontlink *tfp;
			db_fontsubstmode fsm;
			// EBATECH(CNBR) -[ force IC_NO_ALLOW_ICAD_FNT
			//if (this->ret_flags() & IC_SHAPEFILE_SHAPES) fsm=IC_NO_ALLOW_ICAD_FNT;
			//else fsm=IC_ALLOW_ICAD_FNT;
			fsm=IC_NO_ALLOW_ICAD_FNT;
			// EBATECH(CNBR) ]-
			if (db_setfontusingmap(bigfontpn,NULL,&tfp,fsm,dp)>=0)
				{ bigfont=tfp; db_astrncpy(&bigfontpn, p,DB_PATHNAMESZ); }
		}
		return true;
	}
	bool set_40(sds_real p) { fixedht=(p>=0.0) ? p : -p; return true; }
	bool set_41(sds_real p)
		{ xscl=(p>0.0) ? p : ((p<0.0) ? -p : 1.0); return true; }
	bool set_42(sds_real p)
		{ lastht=(p>0.0) ? p : ((p<0.0) ? -p : 0.2); return true; }
	bool set_50(sds_real p) { obl=p; return true; }
	bool set_71(int p) { gen=(char)p; return true; }

	db_handitem *ret_xrefblkhip(void) { return(xrefblkhip); }
	void set_xrefblkhip(db_handitem *p) { xrefblkhip=p; }

    virtual int entmod(struct resbuf *modll, db_drawing *dp);

	// added the following for TrueType font support
	bool hasTrueTypeFont(void);
	bool getFontFamily(char *family);
	bool setFontFamily(const char *family, db_drawing *drawing);
	bool getFontStyleNumber(long *number);
	bool setFontStyleNumber(const char *family, const long styleNumber, db_drawing *drawing);
	bool isObliqued(void);
	bool isVertical(void);
	bool isBackwards(void);
	bool isUpsideDown(void);
	bool addNewTrueTypeEED(const char * family, const long styleNumber, db_drawing *drawing);
	bool ensureNoTrueTypeEED(db_drawing *drawing);

	bool setFontFamilyEED(void); // EBATECH(CNBR)

  private:

	char	 *fontpn;	  /*  3 */
	char	 *bigfontpn;  /*  4 */
	sds_real  fixedht;	  /* 40 */
	sds_real  xscl; 	  /* 41 */
	sds_real  lastht;	  /* 42 */
	sds_real  obl;		  /* 50 */
	char	  gen;		  /* 71 */

	char	*localName;   // EBATECH(CNBR)

	/* DON'T FREE either of these font ptrs.  (They should point */
	/* to one of the links in fontll in a struct dwg_filelink.	*/
	/* NULL if not set yet.): */
	struct db_fontlink *font;
	struct db_fontlink *bigfont;
	db_handitem		 *xrefblkhip;  /* pointer to blockheader for xref from which this came, if any */
};

#endif // _STYLETABREC_H


