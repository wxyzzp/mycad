/* C:\ICAD\PRJ\LIB\DB\LAYERTABREC.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#ifndef _LAYERTABREC_H
#define _LAYERTABREC_H

class DB_CLASS db_layertabrec : public db_tablerecord {

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

	db_layertabrec(void);
	db_layertabrec(char *pname, db_drawing *dp);
	db_layertabrec(const db_layertabrec &sour);  /* Copy constructor */

	db_handitem *ret_ltypehip(void) { return ltypehip; }
	bool get_6(char **p) {
		if (ltypehip!=NULL) ltypehip->get_2(p); else *p=db_str_quotequote;
		return true;
	}
	bool get_6(char *p, int maxlen)
		{ if (ltypehip==NULL) *p=0; else ltypehip->get_2(p,maxlen); return true; }
	bool get_6(db_handitem **p) { *p=ltypehip; return true; }
	bool get_62(int *p) { *p=color; return true; }
	int ret_color(void) { return color; }
/*DG - 11.6.2002*/// dxf290 and dxf370 of layers are supported since dxf 2000.
	bool get_370(int *p) { *p=lineweight; return true; }
	bool get_290(int *p) { *p=plottable; return true; }
	int ret_plottable(void) { return plottable; }
	db_handitem *ret_plotstylehip(void) { return plotstylehip; }
	db_handitem **retp_plotstylehip(void) { return &plotstylehip; }
	bool get_390(db_handitem **p){ *p = plotstylehip; return true; }

	void set_ltypehip(db_handitem *p) { ltypehip=p; }
	db_handitem *ret_ltype(void) { return ltypehip; }

	bool db_layertabrec::set_6(char *p, db_drawing *dp) {
		db_handitem *thip1;

		if (p==NULL || !*p || dp==NULL ||
			(thip1=dp->findtabrec(DB_LTYPETAB,p,1))==NULL)
				{ db_lasterror=OL_ESNVALID; return false; }
		ltypehip=thip1; return true;
	}

	bool set_6(db_handitem *p) { ltypehip=p; return true; }
	bool set_62(int p) { color=(abs(p)<257) ? p : DB_BLCOLOR; return true; }

	bool isVpLayerFrozenByDefault( void );
	void setVpLayerFrozenByDefault( bool bFrozen );

	db_handitem *ret_xrefblkhip(void) { return(xrefblkhip); }
	void set_xrefblkhip(db_handitem *p) { xrefblkhip=p; }
/*DG - 11.6.2002*/// dxf290 and dxf370 of layers are supported since dxf 2000.
	bool set_370(int p) { if( db_is_validweight(p, DB_DFWMASK)){ lineweight = p; } return true; }
	bool set_2(char* p);
	bool set_290(int p);
	bool set_390(db_handitem *p){ plotstylehip =p; return true; }

	bool db_layertabrec::set_390(char *p, db_drawing *dp) {
		db_handitem *dicthip;
		db_handitem *thip1;
		db_objhandle hand;
		if( p != NULL && *p && dp !=NULL )
		{
			hand = dp->ret_stockhand( DB_SHI_DICWDFLT );
			if( hand != NULL )
			{
				if(( dicthip = dp->handent( hand )) != NULL && ( thip1 = dp->dictsearch( dicthip, p, 0 )) != NULL )
				{
					plotstylehip=thip1;
					return true;
				}
			}
		}
		db_lasterror = OL_ESNVALID;
		return false;
	}

  private:
	db_handitem *ltypehip;	/*	6  : Linetype */
	int			 color; 	/* 62  : Color */
	db_handitem *xrefblkhip;  /* pointer to blockheader for xref from which this came, if any */
/*DG - 11.6.2002*/// dxf290 and dxf370 of layers are supported since dxf 2000.
	int 		 lineweight; /* 370: Lineweight */
	int 		 plottable;  /* 290: 0=not plot 1=can plot */
	static char *defpoints_name;
   db_handitem	 *plotstylehip; /* 390 : PlotStyle */
};
#endif
