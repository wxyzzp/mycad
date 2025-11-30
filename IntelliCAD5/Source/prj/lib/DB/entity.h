/* C:\ICAD\PRJ\LIB\DB\ENTITY.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 *
 */

#ifndef _ENTITY_H_
#define _ENTITY_H_

#pragma pack (push)
#pragma pack (1)


class DB_CLASS db_entity : public db_handitem {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:
    db_entity(void);
    db_entity(int ptype, db_drawing *dp=NULL);

	/*D.G.*/
	virtual bool isAcisEntity()		{ return false; }

    db_handitem *ret_ltypehip(void) { return ltypehip; }
    db_handitem *ret_layerhip(void) { return layerhip; }
    int ret_deleted(void)   { return ((eflags&DB_DELETED)  !=0); }
// ret_ent_deleted does not descend as a virtual from handitem, and therefore will inline
	int ret_ent_deleted(void) { return ((eflags & DB_DELETED) !=0); }
    int ret_invisible(void) { return ((eflags&DB_INVISIBLE)!=0); }
    int ret_pspace(void)    { return ((eflags&DB_PSPACE)!=0); }
// ret_ent_pspace does not descend as a virtual from handitem, and therefore will inline
    int ret_ent_pspace(void)  { return ((eflags & DB_PSPACE)!=0); }
	sds_real ret_ltscale(void) { return ltscale; }
	int ret_color(void) { return color; }

    bool get_6(char *p, int maxlen)
        { if (ltypehip==NULL) *p=0; else ltypehip->get_2(p,maxlen); return true; }
    bool get_6(char **p) {
        if (ltypehip!=NULL) ltypehip->get_2(p); else *p=db_str_quotequote;
        return true;
    }
    bool get_6(db_handitem **p) { *p=ltypehip; return true; }
    bool get_8(char *p, int maxlen)
        { if (layerhip==NULL) *p=0; else layerhip->get_2(p,maxlen); return true; }
    bool get_8(char **p) {
        if (layerhip!=NULL) layerhip->get_2(p); else *p=db_str_quotequote;
        return true;
    }
    bool get_8(db_handitem **p) { *p=layerhip; return true; }
    bool get_48(sds_real *p) { *p=(ltscale>0.0) ? ltscale : 1.0; return true; }
    bool get_60(int *p) { *p=((eflags&DB_INVISIBLE)!=0); return true; }
    bool get_62(int *p) { *p=color; return true; }
    bool get_67(int *p) { *p=((eflags&DB_PSPACE)!=0); return true; }
	bool get_370(int *p) { *p=lineweight; return true; }
	bool get_380(int *p) { *p=plotstyletype; return true; }
	bool get_390(db_handitem **p) { *p=plotstylehip; return true; }
    void set_ltypehip(db_handitem *p) { ltypehip=p; }
    void set_layerhip(db_handitem *p) { layerhip=p; }
    void set_deleted(int p);

	bool db_entity::set_6(char *p, db_drawing *dp) {
		db_handitem *thip1;

	    if (p==NULL || !*p || dp==NULL ||
		    (thip1=dp->findtabrec(DB_LTYPETAB,p,1))==NULL)
			    { db_lasterror=OL_EMMLT; return false; }
	    ltypehip=thip1; return true;
	}

	bool set_8(char *p, db_drawing *dp);
    bool set_48(sds_real p) { ltscale=(p<=0.0) ? 1.0 : p; return true; }
    bool set_60(int p)
        { if (p) eflags|=DB_INVISIBLE; else eflags&=~DB_INVISIBLE; return true; }
    bool set_62(int p) { color=(p>-1 && p<257) ? p : DB_BLCOLOR; return true; }
    bool set_67(int p)
        { if (p) eflags|=DB_PSPACE; else eflags&=~DB_PSPACE; return true; }
	bool set_370(int p) 
		{ if( db_is_validweight(p, (DB_BLWMASK|DB_BBWMASK|DB_DFWMASK))){ lineweight = p; } return true; }

	bool set_380(int p) {
		if( p == DB_BL_PLSTYLE || p == DB_BB_PLSTYLE || p == DB_NM_PLSTYLE )
			plotstyletype = p;
		else
			plotstyletype = DB_BL_PLSTYLE;
		if( plotstyletype != DB_NM_PLSTYLE )
			plotstylehip = NULL;
		return true; 
	}
	bool set_390(db_handitem *p)
	{
		plotstylehip = p;
		if( p != NULL )
			plotstyletype = DB_NM_PLSTYLE;
		else if( plotstyletype == DB_NM_PLSTYLE )
			plotstyletype = DB_BL_PLSTYLE;
		return true;
	}

	//int entgetcommon(struct resbuf **begpp, struct resbuf **endpp);
	int entgetcommon(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
    int entmod1common(struct resbuf **rblinkpp, db_drawing *dp);

	//loaded when they need to be resolved.
	virtual bool addReactor(db_objhandle &handle, db_drawing *drawing, ReactorItem::Type type);

  protected:

    db_handitem *ltypehip;  /*  6 : Linetype */
    db_handitem *layerhip;  /*  8 : Layer */
    sds_real     ltscale;   /* 48 : Linetype scale */
    int          color;     /* 62 : Color */
    char         eflags;    /* Bit-coded entity flags (DOxxxxPI): */
                            /*   I : 60  : Invisible   */
                            /*   P : 67  : Paper space */
                            /*   O :     : draw-Order (drawn 1st or last) */
                            /*   D :     : Deleted     */
   int			 lineweight; /*370 : Lineweight */
   int			 plotstyletype; /* 380 : PlotStyleType */
   db_handitem	 *plotstylehip; /* 390 : PlotStyle */
};


#pragma pack(pop)


#endif // _ENTITY_H_
