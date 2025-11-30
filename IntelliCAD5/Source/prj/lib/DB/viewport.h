/* C:\ICAD\PRJ\LIB\DB\VIEWPORT.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#ifndef _INC_VIEWPORT
#define _INC_VIEWPORT

#include "layertabrec.h"


class DB_CLASS db_viewport : public db_entity
	{

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);



	// ****************************
	// public methods
	//
	public:
	// ctors and dtors
	//
	db_viewport(void);
	db_viewport(int numvpfrozlayers);
	db_viewport(db_drawing *dp);
	db_viewport(const db_viewport &sour);  /* Copy constructor */
	~db_viewport(void);

	// accessors (overrides of db_entity base class members)
	//
	void *ret_disp(void)
		{
		return disp;
		}
	void  set_disp(void *p)
		{
		disp=p;
		}
	void  get_extent(sds_point ll, sds_point ur)
		{
		DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]);
		}
	void  set_extent(sds_point ll, sds_point ur)
		{
		DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur);
		}

    sds_real *retp_10(void)
		{
		return cent;
		}

    bool get_10(sds_point p)
		{
		p[0]=cent[0]; p[1]=cent[1]; p[2]=cent[2];
		return true;
		}

    bool get_40(sds_real *p) { *p=wd; return true; }
    bool get_41(sds_real *p) { *p=ht; return true; }
    bool get_68(int *p) { *p=stat; return true; }
    bool get_69(int *p) { *p=id; return true; }
    bool get_70(int *p) { *p=flags; return true; }

    bool set_10(sds_point p) { cent[0]=p[0]; cent[1]=p[1]; cent[2]=p[2]; return true; }
    bool set_40(sds_real p) { wd=(p>0.0) ? p : ((p<0.0) ? -p : 1.0); return true; }
    bool set_41(sds_real p) { ht=(p>0.0) ? p : ((p<0.0) ? -p : 1.0); return true; }
    bool set_67(int p) { return true; }  /* Do nothing (leave it pspace). */
                                         /* (Don't let db_entity:: set_67() */
                                         /* get it's hands on it.) */
    bool set_68(int p) { stat=p; return true; }
    bool set_69(int p) { id=p; return true; }
    bool set_70(int p) { flags=p; return true; }

// new a2k variables
	bool	get_1(char *ssn) { strcpy(ssn,stylesheetname); return true; }
	bool	get_71(short *u) { *u=ucsvp; return true; }
	bool	get_74(short *u) { *u=ucsiconatucsorigin; return true; }
	bool	get_79(short *u) { *u=ucsorthoviewtype; return true; }
	bool	get_90(int *s) { *s=statusflags; return true; }
	bool	get_110(sds_point uo) { ic_ptcpy(uo,ucsorg); return true; }
	bool	get_111(sds_point ud) { ic_ptcpy(ud,ucsxdir); return true; }
	bool	get_112(sds_point ud) { ic_ptcpy(ud,ucsydir); return true; }
	bool	get_146(sds_real *e) { *e=ucselevation; return true; }
	bool	get_281(short *s) { *s=rendermode; return true; }
	bool	get_340(db_handitem **theclipbound) { *theclipbound=clipbound; return true; }
	db_handitem **retp_clipbound(void) { return &clipbound; } // for fixups
	bool	get_345(db_handitem **theucs) {*theucs=ucs; return true; }
	db_handitem **retp_ucs(void) { return &ucs; } // for fixups
	bool	get_346(db_handitem **thebaseucs) { *thebaseucs=baseucs; return true; }
	db_handitem **retp_baseucs(void) { return &baseucs; } // for fixups

// these are the variables that used to be in xdata
	bool	get_12(sds_point cen) { cen[0]=center[0]; cen[1]=center[1]; cen[2]=0.0; return true; }
	// EBATECH(CNBR) -[ 2002/6/19 They are 2D point. initialize Z value to zero.
	bool	get_13(sds_point	sb) { sb[0]=snapbase[0]; sb[1]=snapbase[1]; sb[2]=0.0; return true; }
	bool	get_14(sds_point	su) { su[0]=snapunit[0]; su[1]=snapunit[1]; su[2]=0.0; return true; }
	bool	get_15(sds_point	gu) { gu[0]=gridunit[0]; gu[1]=gridunit[1]; gu[2]=0.0; return true; }
	//bool	get_13(sds_point	sb) { sb[0]=snapbase[0]; sb[1]=snapbase[1]; sb[2]=0.0; return true; }
	//bool	get_14(sds_point	su) { su[0]=snapunit[0]; su[1]=snapunit[1]; su[2]=0.0; return true; }
	//bool	get_15(sds_point	gu) { gu[0]=gridunit[0]; gu[1]=gridunit[1]; gu[2]=0.0; return true; }
	// EBATECH(CNBR) ]-
	bool	get_16(sds_point d) { ic_ptcpy(d,dir); return true; }
	bool	get_17(sds_point t) { ic_ptcpy(t,target); return true; }
	bool	get_42(sds_real *r) { *r=lenslength; return true; }
	bool	get_43(sds_real *r) { *r=frontclip; return true; }
	bool	get_44(sds_real *r) { *r=backclip; return true; }
	bool	get_45(sds_real *h) { *h=size; return true; }
	bool	get_50(sds_real *sa) { *sa=snapangle; return true; }
	bool	get_51(sds_real *t) { *t=twist; return true; }
	bool	get_72(int *c) { *c=(int)circlezoom; return true; }


// new a2k variables
	bool	set_1(char *ssn) { strcpy(stylesheetname,ssn); return true; }
	bool	set_71(short u) { ucsvp=u; return true; }
	bool	set_74(short u) { ucsiconatucsorigin=u; return true; }
	bool	set_79(short u) { ucsorthoviewtype=u; return true; }
	bool	set_90(int s) { statusflags=s; return true; }
	bool	set_110(sds_point uo) { ic_ptcpy(ucsorg,uo); return true; }
	bool	set_111(sds_point ud) { ic_ptcpy(ucsxdir,ud); return true; }
	bool	set_112(sds_point ud) { ic_ptcpy(ucsydir,ud); return true; }
	bool	set_146(sds_real e) { ucselevation=e; return true; }
	bool	set_281(short s) { rendermode=s; return true; }
	bool	set_340(db_handitem *theclipbound);

	// when creating by drawing (instead of load) need to add association
	bool	set_340andAssociate(db_handitem *theclipbound);

	bool	set_345(db_handitem *theucs) {ucs=theucs; return true; }
	bool	set_346(db_handitem *thebaseucs) { baseucs=thebaseucs; return true; }

// these are the variables that used to be in xdata
	bool	set_12(sds_point cen) { center[0]=cen[0]; center[1]=cen[1]; center[2]=0.0; return true; }
	// EBATECH(CNBR) -[ 2002/6/19 They are 2D point. initialize Z value to zero.
	bool	set_13(sds_point	sb) { snapbase[0]=sb[0]; snapbase[1]=sb[1]; snapbase[2]=0.0; return true; }
	bool	set_14(sds_point	su) { snapunit[0]=su[0]; snapunit[1]=su[1]; snapunit[2]=0.0; return true; }
	bool	set_15(sds_point	gu) { gridunit[0]=gu[0]; gridunit[1]=gu[1]; gridunit[2]=0.0; return true; }
	//bool	set_13(sds_point	sb) { snapbase[0]=sb[0]; snapbase[1]=sb[1]; return true; }
	//bool	set_14(sds_point	su) { snapunit[0]=su[0]; snapunit[1]=su[1]; return true; }
	//bool	set_15(sds_point	gu) { gridunit[0]=gu[0]; gridunit[1]=gu[1]; return true; }
	// EBATECH(CNBR) ]-
	bool	set_16(sds_point d) { ic_ptcpy(dir,d); return true; }
	bool	set_17(sds_point t) { ic_ptcpy(target,t); return true; }
	bool	set_42(sds_real r) { lenslength=r; return true; }
	bool	set_43(sds_real r) { frontclip=r; return true; }
	bool	set_44(sds_real r) { backclip=r; return true; }
	bool	set_45(sds_real h) { size=h; return true; }
	// EBATECH(CNBR) -[ 2002/6/19 Needs Normarize
	bool	set_50(sds_real sa) { ic_normang(&sa,NULL); snapangle=sa; return true; }
	bool	set_51(sds_real t) { ic_normang(&t,NULL); twist=t; return true; }
	//bool	set_50(sds_real sa) { snapangle=sa; return true; }
	//bool	set_51(sds_real t) { twist=t; return true; }
	// EBATECH(CNBR) ]-
	// EBATECH(CNBR) -[ 2002/6/19 Check range (1-20000) default:100
	bool	set_72(int c) {
		circlezoom=(short)c;
		if( circlezoom < 1 || circlezoom > 20000 ) {
			circlezoom = 100;
		}
		return true;
	}
	//bool	set_72(int c) { circlezoom=(short)c; return true; }
	// EBATECH(CNBR) ]-

	int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
    int entmod(struct resbuf *modll, db_drawing *dp); // EBATECH(CNBR) 2002/6/19


	// These two methods set the view information in the viewport to
	// the default from the header variables of the drawing
	//
	bool CreateNewViewInfo( db_drawing *dp  );
	bool SetViewInfoToDefault( db_drawing *dp );

	// used by sds_setview--same interface
	bool SetView( db_drawing *dp, const struct resbuf *prbViewData );

	// This method lets us know if the passed in layer
	// is frozen by the VPLAYER command in this viewport
	//
	bool	IsLayerVpLayerFrozen( db_layertabrec *pLayer );
	int		GetVpLayerFrozenLayerCount( void );
	char*	GetVpLayerFrozenLayerName( int index );
	db_layertabrec *GetVpFrozenLayer(int index) { return m_pVpLayerData->GetFrozenLayer(index); }
	bool	VpLayerFreezeLayer( const char *szLayerName, db_drawing *pDrawing , bool *bChanged);
	bool	VpLayerThawLayer( const char *szLayerName, db_drawing *pDrawing , bool *bChanged);
	bool	VpLayerResetLayer( const char *szLayerName, db_drawing *pDrawing , bool *bChanged);
	bool	SetVpLayerToDefaults( db_drawing *pDrawing );
	bool	AddLayerToVpLayerData( db_layertabrec *pLayer );
	bool	RemoveLayerFromVpLayerData( db_layertabrec *pLayer );
	db_layertabrec **RetVpFrozenLayerPointer(int i) { return m_pVpLayerData->RetVpFrozenLayerPointerLoc(i); }
	void BreakAssociation(db_handitem * entity, bool dispMessage = true);

	void BreakAssociation(bool dispMessage = true);

	// *************************
	// Internal class definition
	//
	private:

	// class db_viewport::VpLayerData
	// keeps information about the layers frozen by VPLAYER command
	// for this viewport
	//
	class VpLayerData
		{
		// ctors & dtors
		public:
			VpLayerData( void );
			VpLayerData( int initialnumvpitems );
			~VpLayerData( void );

		// Operations
		public:
			bool	AddFrozenLayer( db_layertabrec* pNewLayerRecord );
			bool	RemoveFrozenLayer( db_layertabrec *pLayerToRemove );
			bool	IsLayerFrozen( db_layertabrec *pLayerRecord );
			int		GetFrozenLayerCount( void );
			db_layertabrec *GetFrozenLayer( int index );
			db_layertabrec **RetVpFrozenLayerPointerLoc( int index ) { return &(m_pFrozenLayers[index]); }
		// internal data
		//
		private:
			int m_iValidFrozenLayerCount;
			int m_iNumStorageLocs;
			db_layertabrec** m_pFrozenLayers;


		};


	private:

	private:
    sds_point cent;  /* 10 */
    sds_real  wd;    /* 40 */
    sds_real  ht;    /* 41 */
    int       stat;  /* 68 */
    int       id;    /* 69 */
    // EBATECH(CNBR) -[ 2002/6/19 DXF=70 is obsolute still exist in ODT.
    int       flags; /* 70 -- ACAD doesn't put it in entget, but we do. */
// new a2k variables
	db_handitem *clipbound;  // 340 clipping boundary -- currently unused
	long	statusflags;		// 90
	short	rendermode;			// 281
	char	stylesheetname[256];	// 1
	short	ucsvp;				// 71
	short	ucsiconatucsorigin;	// 74
	sds_point	ucsorg;			// 110
	sds_point	ucsxdir;		// 111
	sds_point	ucsydir;		// 112
	short		ucsorthoviewtype;	// 79
	sds_real	ucselevation;	// 146
	db_handitem *ucs;			// 345
	db_handitem *baseucs;		// 346
// these are the variables that used to be in xdata
// Since A2K, these variables store in general entity list. *EBATECH(CNBR)*
	sds_point	center;			// 12
	sds_point	target;			// 17
	sds_point	dir;			// 16
	sds_real	twist;			// 51
	sds_real	size;			// 45
	sds_real	lenslength;		// 42
	sds_real	frontclip;		// 43
	sds_real	backclip;		// 44
//	sds_real	viewmode;		in status
	short		circlezoom;		// 72
// 	short		fastzoom;  in status
//	short		ucsicon;  in status
	sds_point	snapunit;	// 14
	sds_point	gridunit;	// 15
//	short		snapstyle;		in status
//	short		snapisopair;    in status
	sds_real	snapangle;		// 50
	sds_point	snapbase;		// 13

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */

	db_viewport::VpLayerData*	m_pVpLayerData;	// 341(multiple)
};

#endif

