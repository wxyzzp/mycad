/* C:\ICAD\PRJ\LIB\DB\DRAWING.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */


#include "db.h"
#include "drawing.h"
#ifndef	_IC_DYNARRAY_H
#include "ic_dynarray.h"
#endif
#include "handletable.h"
#include "fixuptable.h"
#include "failedfonttable.h"
#include "fixup1005.h"
#include "appid.h"
#include "dimstyletabrec.h"
#include "ltypetabrec.h"
#include "styletabrec.h"
#include "vporttabrec.h"
#include "vxtabrec.h"
#include "objects.h"
#include "DbLayout.h"


// I know I'm adding a global - YUCK!  The problem is that every so often we come across a
// weird ACAD drawing that has a reactor that points to an entity that that doesn't have a pointer
// back.  Drawings created with a beta version????	Anyway, a better option would be for the handitems
// to point to the drawing they are in then this could be in the drawing. That would be way too much
// risk to introduce at this point...
int g_nClosingDrawings = 0;

/**************************** db_drawing ****************************/

struct stockclassstru stockclass[DB_NUMSTOCKCLASSES]= {
	{500,"LWPOLYLINE","AcDbPolyline","\"AutoCAD\"",0,0,498 },
	{501,"IMAGE","AcDbRasterImage","ISM",127,0,498 },
	{502,"OLE2FRAME","AcDbOle2Frame","\"\"",0,0,498 },
	{503,"HATCH","AcDbHatch","\"ACAD_SEDONA\"",0,0,498 },
	{504,"DICTIONARYVAR","AcDbDictionaryVar","\"AutoCAD\"",0,0,499 },
	{505,"RASTERVARIABLES","AcDbRasterVariables","ISM",0,0,499 },
	{506,"IMAGEDEF","AcDbRasterImageDef","\"AutoCAD\"",0,0,499 },
	{507,"IMAGEDEF_REACTOR","AcDbRasterImageDefReactor","ISM",1,0,499 },
	{508,"IDBUFFER","AcDbIdBuffer","\"Acad\"",0,0,499 },
	{509,"SPATIAL_FILTER","AcDbSpatialFilter","\"Acad\"",0,0,499 },
	{510,"XRECORD","AcDbXrecord","\"<Built In>\"",0,0,499 },
	{511,"SORTENTSTABLE","AcDbSortentsTable","\"AutoCAD\"",0,0,499 },
	{512,"LAYER_INDEX","AcDbLayerIndex","\"Acad\"",0,0,499 },
	{513,"SPATIAL_INDEX","AcDbSpatialIndex","\"Acad\"",0,0,499 },
	{514,"OBJECT_PTR","CAseDLPNTableRecord","\"ASE-LPNTableRecord\"",1,0,499 },
	{515,"LAYOUT"/*DNT*/,"AcDbLayout","\"AutoCAD 2000\"",1,0,499 },
	{516,"PLOTSETTINGS"/*DNT*/,"AcDbPlotsettings","\"AutoCAD 2000\"",1,0,499 },
	{517,"ACDBDICTIONARYWDFLT"/*DNT*/,"AcDbDictionaryWithDefault","\"AutoCAD 2000\"",1,0,499 },
	{518,"ACDBPLACEHOLDER"/*DNT*/,"AcDbPlaceholder","\"AutoCAD 2000\"",1,0,499 },
	{519,"VBA_PROJECT"/*DNT*/,"AcDbVbaProject","\"AutoCAD 2000\"",1,0,499 },
	{520,"WIPEOUT"/*DNT*/,"AcDbWipeout","\"AutoCAD 2000\"",1,0,498 },
	{521,"WIPEOUTVARIABLES"/*DNT*/,"AcDbWipeoutVariables","\"AutoCAD 2000\"",1,0,499 },
	{522,"RTEXT"/*DNT*/,"RText", "RText|AutoCAD Express Tool|expresstools@autodesk.com",1,0,498 },
	{523,"ARCALIGNEDTEXT"/*DNT*/,"AcDbArcAlignedText","RText|AutoCAD Express Tool|expresstools@autodesk.com",1,0,498 },
	{524,"DIMASSOC"/*DNT*/,"AcDbDimAssoc","AcDbDimAssoc|Product Desc:     AcDim ARX App For Dimension|Company:          Autodesk, Inc.|WEB Address:      www.autodesk.com",1,0,499 } // EBATECH(CNBR) 2002/7/10 Bugzilla#78218 add new DIMASSOC object

	};

	/*F*/
 // EBATECH(CNBR) -[ 2002/9/19 Metric drawing from scratch
db_drawing::db_drawing(int measureinit, int pstylepolicy)
{
/*
**	Creates a blank drawing with the header defaults and
**	standard table records, objects, and classes.
*/
	db_handitem *stdmls,*thip1;
	db_dictionary *nod,*groupd,*mlsd;
	db_dictionaryrec *tdrp1;
	db_class *cls;
	int i;
	db_placeholder *normal;
	db_dictionarywdflt *plnd;
	char *varname;
	resbuf rb;
	memset(this,0,sizeof(*this));  /* Should be safe as long as there */
								   /* are no virtual functions in this class. */

	m_undoList = NULL;
	handleTable = new HandleTable();
	ASSERT( handleTable);
	fixupTable = new FixupTable();
	ASSERT( fixupTable);
	failedFontTable = new FailedFontTable();
	ASSERT( failedFontTable);

	m_fixup1005HandleMap = NULL;
	m_fixup1005HipTable = NULL;

	m_copyFlag = false;  // not in the middle of a copy

  /*
  **  Init the times.  (Note that it's okay to init usrstarttime
  **  whether or not USRTIMER is ON.  See TIME LOGIC in db_util.cpp.)
  */
	time(&sessionstarttime);
	set_usrstarttime();

	rel=2;
	table=new db_table[DB_NTABS];
	freeit=1;

  /* Init header with drawing sysvars -- but note that the standard */
  /* tables have not been set up yet, so the "current." members */
  /* won't get set yet: */
	db_makesvbuf(0,&header,this);

  // EBATECH(CNBR) -[ 2002/9/19 Metric drawing from scratch
	if( measureinit == DB_METRIC_MEASUREINIT )
	{
		for (i=0; db_getvaralt( i, db_measurement_metric, &varname, &rb ) == RTNORM ; i++)
		{
			db_setvar(varname, -1, &rb, this, NULL, NULL, 0 );
			if( rb.restype == RTSTR )
				delete []  rb.resval.rstring ;
		}
	}
	// Named Plotstyle drawing
	if( pstylepolicy == DB_NAMED_PSTYLEMODE )
	{
		for (i=0 ; db_getvaralt( i, db_pstylemode_named, &varname, &rb ) == RTNORM ; i++)
		{
			db_setvar(varname, -1, &rb, this, NULL, NULL, 0 );
			if( rb.restype == RTSTR )
				delete []  rb.resval.rstring ;
		}
	}

	st_cantfindtol=false;
	st_tolfont=NULL;

/*
  **  Init stockhand[], hard-coding the ones we don't use to ACAD's values,
  **  and setting the ones we DO use to "" (to be set by addhanditem()).
  **  THIS HAS TO BE DONE BEFORE ANY ADDHANDITEM() CALLS.
  **  HANDSEED must be higher than any of the hard-coded ones, to reserve
  **  them.
  */

	/* (Note there's no handle "4".) */
	stockhand[DB_SHI_TAB_BLKREC    ]="1";
	stockhand[DB_SHI_TAB_LAYER	   ]="2";
	stockhand[DB_SHI_TAB_STYLE	   ]="3";
	stockhand[DB_SHI_TAB_LTYPE	   ]="5";
	stockhand[DB_SHI_TAB_VIEW	   ]="6";
	stockhand[DB_SHI_TAB_UCS	   ]="7";
	stockhand[DB_SHI_TAB_VPORT	   ]="8";
	stockhand[DB_SHI_TAB_APPID	   ]="9";
	stockhand[DB_SHI_TAB_DIMSTYLE  ]="A";
	stockhand[DB_SHI_TAB_VX 	   ]="B";

	stockhand[DB_SHI_BLKREC_MSPACE ]="19";
	stockhand[DB_SHI_BLKREC_PSPACE ]="16";
	stockhand[DB_SHI_BLK_MSPACE    ]="1A";
	stockhand[DB_SHI_ENDBLK_MSPACE ]="1B";
	stockhand[DB_SHI_BLK_PSPACE    ]="17";
	stockhand[DB_SHI_ENDBLK_PSPACE ]="18";

	stockhand[DB_SHI_LTYPE_BB	   ].Nullify();
	stockhand[DB_SHI_LTYPE_BL	   ].Nullify();
	stockhand[DB_SHI_LTYPE_CONT    ].Nullify();
	stockhand[DB_SHI_LAYER_0	   ].Nullify();
	stockhand[DB_SHI_VPORT_STARACTIVE].Nullify();
	stockhand[DB_SHI_APPID_ACAD    ].Nullify();
	stockhand[DB_SHI_STYLE_STD	   ].Nullify();
	stockhand[DB_SHI_DIMSTYLE_STD  ].Nullify();
	stockhand[DB_SHI_DICT_NO	   ].Nullify();
	stockhand[DB_SHI_DICT_GROUP    ].Nullify();
	stockhand[DB_SHI_DICT_MLS	   ].Nullify();
	stockhand[DB_SHI_MLS_STD	   ].Nullify();
	stockhand[DB_SHI_PLACEHOLDER   ].Nullify();
	stockhand[DB_SHI_LAYOUTDIC	   ].Nullify();
	stockhand[DB_SHI_DICWDFLT	   ].Nullify();
	stockhand[DB_SHI_MSLAYOUT	   ].Nullify();
	stockhand[DB_SHI_PSLAYOUT	   ].Nullify();
	stockhand[DB_SHI_PLOTSETTINGS  ]="21";
	stockhand[DB_SHI_WIPEOUTVARIABLES  ]="22";


  /*
  **  Set up the table records, etc., required for a new drawing.
  **  NOTE ORDER: some tables refer to recs in other tables that must
  **  already exist.
  */

  /* LTYPE BYLAYER, BYBLOCK, and CONTINUOUS: */
	current.celtype=new db_ltypetabrec(db_str_bylayer,this);
	addhanditem(current.celtype);
	thip1=new db_ltypetabrec(db_str_byblock,this);
	addhanditem(thip1);
	thip1=new db_ltypetabrec(db_str_continuous,this);
	addhanditem(thip1);

  /* LAYER 0 */
	current.clayer=new db_layertabrec(db_str_0,this);
	addhanditem(current.clayer);

  /* STYLE STANDARD: */
	current.dimtxsty=current.textstyle=new db_styletabrec(db_str_standard,this);
	addhanditem(current.textstyle);
	current.textstyle->set_3(db_str_txt,this);

  /* DIMSTYLE STANDARD: */
	current.dimstyle=new db_dimstyletabrec(db_str_standard,this);
	addhanditem(current.dimstyle);

  /* APPID ACAD: */
	thip1=new db_appidtabrec(db_str_acad,this);
	addhanditem(thip1);

  /* VPORT *ACTIVE: */
	thip1=new db_vporttabrec(db_str_staractive,this);
	addhanditem(thip1);


  /*
  **  The DICTIONARYs and the STANDARD MLINESTYLE.	Tricky. Don't addhanditem
  **  until we've created them all.  The NOD must go first.  Don't add
  **  records or reactors until all have been added to the database
  **  (so they have handles).
  */

  /* Create MLINESTYLE STANDARD: */
	stdmls=new db_mlinestyle();
	stdmls->set_2(db_str_standard);
	stdmls->set_3(db_str_quotequote);
	// EBATECH(CNBR) 2002/6/20 Bugzilla:78214 AutoCAD 2002 Creates STANDARD
	// MLINESTYLE 1st 62(Fill Color) is 256(ByLayer).
	stdmls->set_62(DB_BLCOLOR);
	//stdmls->set_62(DB_BBCOLOR);
	// EBATECH(CNBR) ]-
	stdmls->set_71(2); // Num Of Segment Lines
	{
		char *lt[2];
		int clr[2];
		sds_real offset[2];

		lt[0]=lt[1]=db_str_bylayer;
		clr[0]=clr[1]=DB_BLCOLOR;
		offset[1]=-(offset[0]=0.5);

		stdmls->set_6(lt,2,this);
		stdmls->set_49(offset,2);
		stdmls->set_62(clr,2);
	}

  /* Create the three DICTIONARYs: */
	nod=new db_dictionary();
	groupd=new db_dictionary();
	mlsd=new db_dictionary();
	db_dictionary* pLayoutsDict = new db_dictionary();

	/*
	**	Now we can addhanditem the OBJECTs -- NOD first -- and set the
	**	DICTIONARY stockhands (which can't be done in addhanditem
	**	since a DICTIONARY doesn't have any indication WHICH
	**	DICTIONARY it is).
	*/
	addhanditem(nod);
	addhanditem(groupd);
	addhanditem(mlsd);
	addhanditem(stdmls);
	addhanditem(pLayoutsDict);
	stockhand[DB_SHI_DICT_NO]=nod->RetHandle();
	stockhand[DB_SHI_DICT_GROUP]=groupd->RetHandle();
	stockhand[DB_SHI_DICT_MLS]=mlsd->RetHandle();
	stockhand[DB_SHI_LAYOUTDIC] = pLayoutsDict->RetHandle();

  /* We can add the records to the DICTIONARYs (since they all */
  /* have handles now): */

	tdrp1=new db_dictionaryrec(db_str_standard,DB_SOFT_OWNER,
		stdmls->RetHandle(),stdmls);
	mlsd->addrec(tdrp1);

	tdrp1=new db_dictionaryrec("ACAD_GROUP",DB_SOFT_OWNER,
		groupd->RetHandle(),groupd);
	nod->addrec(tdrp1);
	tdrp1=new db_dictionaryrec("ACAD_MLINESTYLE",DB_SOFT_OWNER,
		mlsd->RetHandle(),mlsd);
	nod->addrec(tdrp1);

	/*D.G.*/// We should create the ACAD_LAYOUT entry because OpenDWG library creates
	// "LAYOUT" object(s) even if we doesn't.
	/*DG - 15.2.2002*/// Here is the required structure: NOD contains ACAD_LAYOUT record
	// for a special DICTIONARY which contain records for all LAYOUT objects.
	// Since we don't create these objects (the DICTIONARY of LAYOUTs) ourselves and let
	// ODT create them then we should add ACAD_LAYOUT record only into the saved file.
	// So, I comment out these lines for the future (when we will create layouts-related stuff).
	tdrp1 = new db_dictionaryrec("ACAD_LAYOUT", DB_SOFT_OWNER,
								 stockhand[DB_SHI_LAYOUTDIC], pLayoutsDict);	/*DG*/// of course, not NOD (see comments above)
	nod->addrec(tdrp1);

  /* We can add the persistent reactors now, too: */
	stdmls->addReactor(mlsd->RetHandle(),mlsd, ReactorItem::SOFT_POINTER);

	groupd->addReactor(nod->RetHandle(),nod, ReactorItem::SOFT_POINTER);

	mlsd->addReactor(nod->RetHandle(),nod, ReactorItem::SOFT_POINTER);

	pLayoutsDict->addReactor(nod->RetHandle(),nod, ReactorItem::SOFT_POINTER);

	// DP: create layouts for model and paper spaces
	addDefaultLayoutObject(pLayoutsDict, MODELSPACE_LAYOUT);
	addDefaultLayoutObject(pLayoutsDict, PAPERSPACE_LAYOUT1);
	addDefaultLayoutObject(pLayoutsDict, PAPERSPACE_LAYOUT2);

  /* Create ACAD_PLOTSTYLENAME dictionary and Default PLOTSTYLE */
	normal = new db_placeholder();
	plnd = new db_dictionarywdflt();

	addhanditem(normal);
	addhanditem(plnd);
	stockhand[DB_SHI_PLACEHOLDER]=normal->RetHandle();
	stockhand[DB_SHI_DICWDFLT]=plnd->RetHandle();

	tdrp1=new db_dictionaryrec(db_str_normal,DB_SOFT_OWNER,
		normal->RetHandle(),normal);
	plnd->addrec(tdrp1);
	tdrp1=new db_dictionaryrec("ACAD_PLOTSTYLENAME"/*DNT*/,DB_SOFT_OWNER,
		plnd->RetHandle(),plnd);
	nod->addrec(tdrp1);

	plnd->set_340((db_handitem*)normal);
	normal->addReactor(plnd->RetHandle(),plnd, ReactorItem::SOFT_POINTER);
	plnd->addReactor(nod->RetHandle(),nod, ReactorItem::SOFT_POINTER);

	(db_layertabrec*)(current.clayer)->set_390((db_handitem*)normal);

  /* Add the "classes" crap */

	for (i=0; i<sizeof(stockclass)/sizeof(stockclassstru); i++)
		{
		cls=new db_class;
		cls->set_data(stockclass[i].classnum,
					  stockclass[i].dxfrecname,
					  stockclass[i].cppname,
					  stockclass[i].appname,
					  "",
					  stockclass[i].vernum,
					  stockclass[i].wasproxy,
					  stockclass[i].isent);
		addclass(cls);
		}

	set_isDirty(false);

	m_loadedFromA2K=false;
	m_nFileVersion = IC_ACAD2004;

	dimfrac=0;
	dimldrblk[0]=0;
	dimtmove=0;
	endcaps=0;
	fingerprintguid[0]=0;
	joinstyle=0;
	pucsbase=NULL;
	pucsorgback[0]=pucsorgback[1]=pucsorgback[2]=0.0;
	pucsorgbottom[0]=pucsorgbottom[1]=pucsorgbottom[2]=0.0;
	pucsorgfront[0]=pucsorgfront[1]=pucsorgfront[2]=0.0;
	pucsorgleft[0]=pucsorgleft[1]=pucsorgleft[2]=0.0;
	pucsorgright[0]=pucsorgright[1]=pucsorgright[2]=0.0;
	pucsorgtop[0]=pucsorgtop[1]=pucsorgtop[2]=0.0;
	pucsorthoref=NULL;
	tducreate[0]=tducreate[1]=0L;
	tduupdate[0]=tduupdate[1]=0L;
	ucsbase=NULL;
	ucsorgback[0]=ucsorgback[1]=ucsorgback[2]=0.0;
	ucsorgbottom[0]=ucsorgbottom[1]=ucsorgbottom[2]=0.0;
	ucsorgfront[0]=ucsorgfront[1]=ucsorgfront[2]=0.0;
	ucsorgleft[0]=ucsorgleft[1]=ucsorgleft[2]=0.0;
	ucsorgright[0]=ucsorgright[1]=ucsorgright[2]=0.0;
	ucsorgtop[0]=ucsorgtop[1]=ucsorgtop[2]=0.0;
	ucsorthoref=NULL;
	versionguid[0]=0;
}

db_drawing::db_drawing(int mode)
{
/*
**	A special-purpose constructor.
**
**	mode==0 :
**
**		A do-nothing mode for when you specifically DON'T want the
**		object initialized. (Avoids an infinite recursion in
**		db_makesvbuf() when making dummydp.)
**
**	mode!=0 :
**
**		Makes a db_drawing object suitable for the drawing-readers.
**		The tables are blank (no standard records).
*/
	/* Zero the whole object; should be safe as long as there */
	/* are no virtual functions in this class. */
	memset(this,0,sizeof(*this));

	handleTable = new HandleTable();
	ASSERT( handleTable);
	fixupTable = new FixupTable();
	ASSERT( fixupTable);

	m_fixup1005HandleMap = NULL;
	m_fixup1005HipTable = NULL;

	m_copyFlag = false;  // not in the middle of a copy

	/*
	**	Init the times.  (Note that it's okay to init usrstarttime
	**	whether or not USRTIMER is ON.	See TIME LOGIC in db_util.cpp.)
	*/
	time(&sessionstarttime);
	set_usrstarttime();

	if (mode)
		{
		rel=2;
		table=new db_table[DB_NTABS];
		freeit=1;
		db_makesvbuf(0,&header,this);
		}

	st_cantfindtol=false;
	st_tolfont=NULL;

	set_isDirty(false);
	m_loadedFromA2K=false;
	m_nFileVersion = IC_ACAD2004;

	dimfrac=0;
	dimldrblk[0]=0;
	dimtmove=0;
	fingerprintguid[0]=0;
	endcaps=0;
	joinstyle=0;
	pucsbase=NULL;
	pucsorgback[0]=pucsorgback[1]=pucsorgback[2]=0.0;
	pucsorgbottom[0]=pucsorgbottom[1]=pucsorgbottom[2]=0.0;
	pucsorgfront[0]=pucsorgfront[1]=pucsorgfront[2]=0.0;
	pucsorgleft[0]=pucsorgleft[1]=pucsorgleft[2]=0.0;
	pucsorgright[0]=pucsorgright[1]=pucsorgright[2]=0.0;
	pucsorgtop[0]=pucsorgtop[1]=pucsorgtop[2]=0.0;
	pucsorthoref=NULL;
	tducreate[0]=tducreate[1]=0L;
	tduupdate[0]=tduupdate[1]=0L;
	ucsbase=NULL;
	ucsorgback[0]=ucsorgback[1]=ucsorgback[2]=0.0;
	ucsorgbottom[0]=ucsorgbottom[1]=ucsorgbottom[2]=0.0;
	ucsorgfront[0]=ucsorgfront[1]=ucsorgfront[2]=0.0;
	ucsorgleft[0]=ucsorgleft[1]=ucsorgleft[2]=0.0;
	ucsorgright[0]=ucsorgright[1]=ucsorgright[2]=0.0;
	ucsorgtop[0]=ucsorgtop[1]=ucsorgtop[2]=0.0;
	ucsorthoref=NULL;
	versionguid[0]=0;
}

db_drawing::~db_drawing(void)
{
	// Increment closing drawings counter.
	g_nClosingDrawings++;

	if (freeit)
		{
		struct db_fontlink *tp;

		delete [] pn; delete [] table;

		if (imagebufbmp!=NULL) delete [] imagebufbmp;  /* Allocated by MarComp */
		if (imagebufwmf!=NULL) delete [] imagebufwmf;  /* Allocated by MarComp */

		if (fptr!=NULL) fclose(fptr);
		db_handitem::delll(entll[0]);
		db_handitem::delll(objll[0]);
		db_handitem::delll(pendentmake[0]);

		/*DG - 24.1.2002*/// Set these for safety, because this db_drawing may be referenced somewhere
		// eg, in m_undoList.
		objll[0] = objll[1] = entll[0] = entll[1] = NULL;

		db_class::delll(classll[0]);

		while (fontll!=NULL)
			{
			tp=fontll->next;
			delete fontll;
			fontll=tp;
			}

		delete m_undoList;
		}

	delete handleTable;
	delete fixupTable;
	delete failedFontTable;

	ASSERT(NULL == m_fixup1005HandleMap);	// this should already be deleted and set to null through Delete1005FixupTables()
	delete m_fixup1005HandleMap;
	ASSERT(NULL == m_fixup1005HipTable);	// this should already be deleted and set to null through Delete1005FixupTables()
	delete m_fixup1005HipTable;

	// Done closing drawing so set decrement closing drawings counter.
	g_nClosingDrawings--;
}


/*F*/
int db_drawing::addhanditem(db_handitem *hip) 
{
/*
**	Adds hip to the drawing object.  The logic for complex entities
**	and block defs (using pendentmake) is in this function.
**	Various clean-ups are performed here (setting NULL table record
**	pointers to defaults, setting the -2 group for SEQEND, etc.).
**
**	When called with hip==NULL, terminates the creation of any
**	complex entity or block definition that's in progress
**	(similar to sds_entmake(NULL)).
**
**	dp->stockhand[] elements are assigned in here for the table
**	records and objects that have stockhands.
**
**	WARNING: When this function returns an error code, the caller
**		should NOT do anything further with the hip passed to this
**		function; it's probably been freed.  However, when this
**		function returns 0 (OK), it should be okay to use the hip
**		passed to this function.  (Just remember: IN ANY CASE,
**		it's now part of the drawing's database -- or has been deleted;
**		it is out of the caller's hands -- so don't free it.)
**
**	Returns:
**		 0 : OK
**		-1 : Not added, for some reason (dup or "" table rec name,
**			   complex entity creation problem, etc.).	Sets db_lasterror
**			   to the appropriate OL_ error code.
**		-2 : No memory.
**
*/
	char *fcp1,*fcp2;
	int hiptype,cancelcomplex,shi,rc,fi1;
	db_blocktabrec *btrhip;
	db_handitem *thip1;
	sds_real ar1;
	sds_point ap1;

	rc=cancelcomplex=0;

	if (hip==NULL)
	{
		cancelcomplex=1;
		goto out;
	}

	/* Don't zero db_lasterror until AFTER the cancel-complex-entity */
	/* line above.	The CALLER may be setting it and then calling */
	/* this function to cancel. */
	db_lasterror=0;

	hiptype=hip->ret_type();
	if (!db_is_validhitype(hiptype))
	{
		rc=-1;
		cancelcomplex=1;
		db_lasterror=OL_EMKTYPE;
		goto out;
	}

	hip->next=NULL;  /* A safety feature, in case hip->next is not */
					 /* already NULL (because of a copy constructor */
					 /* or something). */
	if (db_is_tabrectype(hiptype))
	{
		/* Set any table record pointers that haven't already been set */
		/* (due to the fact that this table record's void constructor */
		/* was called): */
		if (hiptype==DB_DIMSTYLETABREC)
		{
			hip->get_340(&thip1);
			if (thip1==NULL)
				hip->set_340(current.textstyle);
		}
		if (hiptype==DB_LAYERTABREC)
		{
			hip->get_6(&thip1);
			if (thip1==NULL)
				hip->set_6(table[DB_LTYPETAB].findRecord(db_str_continuous));
		}
		rc=table[db_hitypeinfo[hiptype].tabtype].addRecord(hip);
	}
	else 
		if (db_is_objecttype(hiptype))
		{
			if (objll[0]==NULL)
				objll[0]=hip;
			else
				objll[1]->next=hip;
			objll[1]=hip;
		}
		else
		{  /* Entity */

		/* Go ahead and add it to pendentmake.	If that turns out */
		/* to be an error, pendentmake will be freed. */
		if (pendentmake[0]==NULL)
			pendentmake[0]=hip;
		else
			pendentmake[1]->next=hip;
		pendentmake[1]=hip;

		/* Deal with complex entity and block def logic: */
		switch (hiptype)
			{
			case DB_BLOCK:
				if (blockdefpending())
					{
					cancelcomplex=1;
					db_lasterror=OL_EMKNESTBLK;
					}
				else if (complexinsertpending() || polylinepending())
					{
					cancelcomplex=1;
					db_lasterror=OL_EMKINVCOMP;
					}
				else
					{
					hip->get_2(&fcp1);
					if (fcp1==NULL || !*fcp1)  /* No block name. */
						{
						cancelcomplex=1;
						db_lasterror=OL_EMKNOBNM;
						}
					setblockdefpending();
					}
				break;

			case DB_ENDBLK:
				if (!somethingpending())
					{
					cancelcomplex=1;
					db_lasterror=OL_EMKBLOCK;
					}
				else if (!blockdefonlypending())
					{
					cancelcomplex=1;
					db_lasterror=OL_EMKINVCOMP;
					}
				else
					{
					/* Successfully gathered the block def entities in */
					/* pendentmake.  Find/make the table record and */
					/* give it the block def data. */
					if (pendentmake[0]==NULL ||
						pendentmake[0]->ret_type()!=DB_BLOCK)
						{
					/* Should have been caught earlier, but... */
						cancelcomplex=1;
						db_lasterror=OL_EMKBLOCK;
						break;
						}
					pendentmake[0]->get_2(&fcp1);
					if (fcp1==NULL || !*fcp1)
						{
						/* No block name.  Should have been caught in BLOCK. */
						cancelcomplex=1;
						db_lasterror=OL_EMKNOBNM;
						break;
						}

					/*D.G.*/// We should set db_tablerecord::xrefidx for blocks from xrefs when they are created
					// via sds (not only via xref command). We do it like as in the AddXrefPseudoBlock function
					// (so, maybe such code in it can be removed).
					// Note, OpenDWG library (at least till the version 2.006) writes DXF71 = 1 for
					// an according block entity if its blocktabrec db_tablerecord::xrefidx == -1.
					// DXF71 == 1 means "xref is unloaded".
					btrhip = (db_blocktabrec*)findtabrec(DB_BLOCKTAB, fcp1, 1);
					pendentmake[0]->get_70(&fi1);
					int		xrefidx = 0;
					if(fi1 >= 4 && (!btrhip || btrhip->ret_xrefidx() < 0))
						{
						for(db_handitem* pHandItem = tblnext(DB_BLOCKTABREC, 1); pHandItem; pHandItem = pHandItem->next)
							++xrefidx;
						}
					if(!btrhip)
						{
						/* Doesn't exist, so make it. */
						btrhip=new db_blocktabrec(fcp1,this);
						if ((rc=addhanditem(btrhip))!=0)  /* Recursion */
							{
							cancelcomplex=1;
							break;
							}
						}

					if(btrhip->ret_xrefidx() < 0)
						btrhip->set_xrefidx(xrefidx);

					/* Last created OR modified block tab rec. */
					lastblockdef=btrhip;

					/* Now btrhip points to the associated db_blocktabrec */
					/* (even if we had to make it). */

					/* Make the block tab rec's name and flags match those */
					/* in the BLOCK entity.  Use the SINGLE-parameter */
					/* set_2() so that we don't disturb an anonymous block */
					/* name at this point.	(The db_blocktabrec setters */
					/* may set the old db_block values, but so what? */
					/* We're going to delete it anyway.) */
					btrhip->set_2(fcp1);
					btrhip->set_70(fi1);

					/* Set the BLOCK's ptr to the tab rec: */
					((db_block *)pendentmake[0])->set_bthip(btrhip);

					/* Define (or redefine) the block def in the table record */
					/* (btrhip->entll) by stealing pendentmake: */
					btrhip->steal_blockredef(pendentmake[0],pendentmake[1]);
					pendentmake[0]=pendentmake[1]=NULL;

					setnothingpending();
					}
				break;

			case DB_POLYLINE:
				if (complexinsertpending() || polylinepending())
					{
					cancelcomplex=1;
					db_lasterror=OL_EMKINVCOMP;
					}
				else
					{
					setpolylinepending();
					complexmain=hip;
					}
				break;

			case DB_INSERT:
				if (complexinsertpending() || polylinepending())
				{
					cancelcomplex=1;
					db_lasterror=OL_EMKINVCOMP;
				}
				else 
				{
					fcp1=NULL;
					hip->get_2(&fcp1);
					if (fcp1==NULL || !*fcp1)
						{
						/* No block name was supplied. */
						/* (An invalid one should be caught when entmod */
						/* calls set_2(), setting db_lasterror; then */
						/* entmake would not call addhanditem.) */
						cancelcomplex=1;
						db_lasterror=OL_EMKMANDF;
						}
					else
						{
						/* Check for direct recursion -- a block def */
						/* with an INSERT of itself: */
						if (blockdefpending() && pendentmake[0]!=NULL &&
							pendentmake[0]->ret_type()==DB_BLOCK)
							{
							fcp2=NULL;
							pendentmake[0]->get_2(&fcp2);
							if (fcp2!=NULL && strisame(fcp1,fcp2))
								{
								cancelcomplex=1;
								db_lasterror=OL_EMKBLOCK;
								break;
								}
							}

						fi1=0; 
						hip->get_66(&fi1);
						if (fi1)
							{
							setcomplexinsertpending();
							complexmain=hip;
							}  /* Complex */
						}
					}
				break;

			case DB_VERTEX:
				if (polylinepending())
					{
					/* For 2D VERTEXs, set the Z to the value in the POLYLINE: */
					if (complexmain!=NULL)
						{
						complexmain->get_70(&fi1);
						if (!(fi1&0x58)) /* 2D */
							{
							complexmain->get_10(ap1);
							ar1=ap1[2];
							hip->get_10(ap1);
							ap1[2]=ar1;
							hip->set_10(ap1);
							}
						}
					}
				else
					{
					if (!complexinsertpending() && !polylinepending())
						{
						cancelcomplex=1;
						db_lasterror=OL_EMKTYPE;
						}
					else
						{
						cancelcomplex=1;
						db_lasterror=OL_EMKINVCOMP;
						}
					}
				break;

			case DB_ATTRIB:
				if (!complexinsertpending())
					{
					if (!polylinepending())
						{
						cancelcomplex=1;
						db_lasterror=OL_EMKTYPE;
						}
					else
						{
						cancelcomplex=1;
						db_lasterror=OL_EMKINVCOMP;
						}
					}
				break;

			case DB_SEQEND:
				if (complexinsertpending() || polylinepending())
					{
					/* Check for 0-vertex POLYLINE (1 vert is OK): */
					if (complexmain!=NULL &&
						complexmain->ret_type()==DB_POLYLINE)
						{

						for (thip1=complexmain,fi1=0; thip1!=
							NULL; thip1=thip1->next,fi1++);
						if (fi1<3)
							{
							cancelcomplex=1;
							db_lasterror=OL_EMKPLINE;
							}
						}

					/* Shut off POLYLINE/INSERT. */
					setcomplexinsertnotpending();
					setpolylinenotpending();
					}
				else
					{
					cancelcomplex=1;
					db_lasterror=OL_EMKTYPE;
					}
				break;

			default:  /* Plain old simple entity. */
				if (complexinsertpending() || polylinepending())
					{
					cancelcomplex=1;
					db_lasterror=OL_EMKINVCOMP;
					}
				break;

			}

		if (cancelcomplex)
			{
			rc=-1;
			goto out;
			}

		/*
		**	If pendflags indicates that nothing is in-progress, then
		**	steal pendentmake for the database, set the lastmainents,
		**	and increment nmainents.  (Block defs handled
		**	in ENDBLK above.)
		**
		**	This is also where we make a VX table record if we just
		**	added a VIEWPORT entity to entll.
		*/
		if (!somethingpending() && pendentmake[0]!=NULL && pendentmake[1]!=NULL)
			{
			if (entll[0]==NULL) 
				entll[0]	  =pendentmake[0];
			else				
				entll[1]->next=pendentmake[0];
			entll[1]=pendentmake[1];
			pendentmake[0]->get_67(&fi1);
			lastmainent[fi1!=0]=lastmainent[2]=pendentmake[0];
			if (pendentmake[0]->ret_invisible())	 // last cannot be invisible
				lastmainent[fi1!=0]=lastmainent[2]=NULL;
			pendentmake[0]=pendentmake[1]=NULL;
			nmainents++;
			isDirty=true;

			if (hiptype==DB_VIEWPORT)
				{
				db_viewport *pViewport = (db_viewport *)hip;
				db_vxtabrec *vxr;

				vxr=new db_vxtabrec;
				vxr->set_vpehip(pViewport);
				addhanditem(vxr);  /* Recursion */
				}
			}

		/* hip is still valid -- it just points inside pendentmake */
		/* or entll now. */

		/* If layer is still NULL, set it to clayer: */
		if (hip->ret_layerhip()==NULL) 
			hip->set_layerhip(current.clayer);

		/* If ltype is still NULL, set it to celtype (entities and layertabrecs): */
		if (hip->ret_ltypehip()==NULL) 
			hip->set_ltypehip(current.celtype);

		/* Set SEQEND's mainhip and any table record pointers that */
		/* haven't already been set (due to the fact that this entity's */
		/* void constructor was called): */
		switch (hiptype)
			{
			case DB_ATTDEF: 
			case DB_ATTRIB: 
			case DB_TEXT: 
			case DB_MTEXT:
				hip->get_7(&thip1);
				if (thip1==NULL) 
					hip->set_7(current.textstyle);
				break;
			case DB_DIMENSION: 
			case DB_LEADER: 
			case DB_TOLERANCE:
				hip->get_3(&thip1);
				if (thip1==NULL) 
					hip->set_3(current.dimstyle);
				break;
			case DB_VERTEX:
				if (((db_vertex *)hip)->retParentPolyline() == NULL)
					((db_vertex *)hip)->setParentPolyline( (db_polyline *)complexmain  );
				break;
			case DB_SEQEND:
				((db_seqend *)hip)->get_mainent(&thip1);
				if (thip1==NULL)
					((db_seqend *)hip)->set_mainent(complexmain);
				break;
			}

		}  /* End else it's an entity */

	/*
	**	If successfully added, assign the handle.
	**
	**	TODO: Right now, we waste handles on pending complex
	**	entities that are aborted.	Someday, we can fix it so
	**	that we only assign handles when the pending stuff
	**	is actually added to the database (by walking through
	**	pendentmake).  Watch out for table records when we fix the
	**	logic.
	*/
	if (!rc) 
		hip->assignhand(this);

	/*
	**	Check for certain tab recs and objects and set the appropriate
	**	dp->stockhand[] elements.  (Note that DICTIONARYs are missing here.
	**	The NOD, GROUP, and MLINESTYLE DICTIONARY stock handles
	**	are set in the db_drawing constructor.	(A DICTIONARY has
	**	no indication WHICH DICTIONARY it is!))
	*/
	if (!rc && (db_is_tabrectype(hiptype) || db_is_objecttype(hiptype)))
		{
		shi=-1;  /* stockhand[] idx */
		fcp1=NULL; 
		hip->get_2(&fcp1);
		switch (hiptype)
			{
			case DB_LTYPETABREC:
				if (hip->ret_byblock())    
					shi=DB_SHI_LTYPE_BB;
				if (hip->ret_bylayer())    
					shi=DB_SHI_LTYPE_BL;
				if (hip->ret_continuous()) 
					shi=DB_SHI_LTYPE_CONT;
				break;
			case DB_LAYERTABREC:
				if (fcp1!=NULL && *fcp1=='0' && !fcp1[1]) 
					shi=DB_SHI_LAYER_0;
				break;
			case DB_VPORTTABREC:
				if (fcp1!=NULL && strisame(fcp1,db_str_staractive))
					shi=DB_SHI_VPORT_STARACTIVE;
				break;
			case DB_APPIDTABREC:
				if (fcp1!=NULL && strisame(fcp1,db_str_acad))
					shi=DB_SHI_APPID_ACAD;
				break;
			case DB_STYLETABREC:
				if (fcp1!=NULL && strisame(fcp1,db_str_standard))
					shi=DB_SHI_STYLE_STD;
				break;
			case DB_DIMSTYLETABREC:
				if (fcp1!=NULL && strisame(fcp1,db_str_standard))
					shi=DB_SHI_DIMSTYLE_STD;
				break;
			case DB_MLINESTYLE:
				if (fcp1!=NULL && strisame(fcp1,db_str_standard))
					shi=DB_SHI_MLS_STD;
				break;
			}
		if (shi>-1) 
			stockhand[shi]=hip->RetHandle();
		}

	/*
	** Add an element to handleTable
	*/
	if (!rc && handleTable)
		handleTable->Add(hip);

out:
	if (cancelcomplex)
		{
		setnothingpending();
		db_handitem::delll(pendentmake[0]);
		pendentmake[0]=pendentmake[1]=NULL;
		complexmain=NULL;  /* May have pointed into pendentmake. */
		}

	return rc;
}


/*F*/
void db_drawing::getucs(
	sds_point org,
	sds_point xdir,
	sds_point ydir,
	sds_point zdir) {
/*
**	Reads UCSORG, UCSXDIR, and UCSYDIR from header (via
**	db_qgetvar()) and sets the appropriate pass parameters.
*/
	short inps,fi1,fi2;
	sds_real ar1;
	sds_point ucs[4];


	inps=inpspace();

	db_qgetvar((inps) ? DB_QP_UCSORG  : DB_QUCSORG ,header.buf,ucs[0],DB_3DPOINT,0);
	db_qgetvar((inps) ? DB_QP_UCSXDIR : DB_QUCSXDIR,header.buf,ucs[1],DB_3DPOINT,0);
	db_qgetvar((inps) ? DB_QP_UCSYDIR : DB_QUCSYDIR,header.buf,ucs[2],DB_3DPOINT,0);

	/* Get the unit UCS axes; use fi2 to signal when to default to WCS: */
	fi2=0;
	for (fi1=1; fi1<3 && !fi2; fi1++) {  /* Unitize the X- and Y-axes. */
		if (icadRealEqual((ar1=DB_VLEN(ucs[fi1])),0.0)) fi2=1;
		else if (!icadRealEqual(ar1,1.0)) { ar1=1.0/ar1; DB_VSCALE(ucs[1],ar1); }
	}

	/* Cross X with Y to get Z-axis: */
	if (!fi2) {
		ucs[3][0]=ucs[1][1]*ucs[2][2]-ucs[1][2]*ucs[2][1];
		ucs[3][1]=ucs[1][2]*ucs[2][0]-ucs[1][0]*ucs[2][2];
		ucs[3][2]=ucs[1][0]*ucs[2][1]-ucs[1][1]*ucs[2][0];
		if ((ar1=DB_VLEN(ucs[3]))<IC_ZRO) fi2=1;
		else if (ar1!=1.0) { ar1=1.0/ar1; DB_VSCALE(ucs[3],ar1); }
	}

	if (!fi2) {
		/* Cross Z with X to make SURE Y-axis is normal to the others: */
		ucs[2][0]=ucs[3][1]*ucs[1][2]-ucs[3][2]*ucs[1][1];
		ucs[2][1]=ucs[3][2]*ucs[1][0]-ucs[3][0]*ucs[1][2];
		ucs[2][2]=ucs[3][0]*ucs[1][1]-ucs[3][1]*ucs[1][0];
	} else {
		ucs[1][1]=ucs[1][2]=ucs[2][0]=ucs[2][2]=ucs[3][0]=ucs[3][1]=0.0;
		ucs[1][0]=ucs[2][1]=ucs[3][2]=1.0;
	}

	if (org !=NULL) { DB_PTCPY( org,ucs[0]); }
	if (xdir!=NULL) { DB_PTCPY(xdir,ucs[1]); }
	if (ydir!=NULL) { DB_PTCPY(ydir,ucs[2]); }
	if (zdir!=NULL) { DB_PTCPY(zdir,ucs[3]); }
}


/*F*/
int db_drawing::trans(
	sds_point  pt,
	resbuf	  *from,
	resbuf	  *to,
	int 	   disp,
	sds_point  result,
	sds_point *altucs) {
/*
**	No paper space yet (mode 3).  Works by transforming pt from the "from" system
**	to the WCS and then from the WCS to the "to" system.
**
**	NOTE THAT THIS FUNCTION GETS SYSTEM VARIABLES DIRECTLY FROM THE HEADER
**	BUFFER.  (Calling db_getvar() risks infinite recursion.
**	Also, we WANT TARGET and VIEWDIR in the WCS in this function --
**	not in the UCS as sds_getvar() returns them.)
**
**	altucs was added to make it possible to transform to and from
**	a UCS other than the current one (specifically, the UCS in which a
**	DIMENSION was created).  If NULL, the current UCS will be used
**	(from the system variables).  If non-NULL, it must point to an array
**	of at least 4 sds_points, set up like this:
**
**		altucs[0] : UCS origin (WCS)
**		altucs[1] : UCS X-axis vector (WCS)
**		altucs[2] : UCS Y-axis vector (WCS)
**		altucs[3] : UCS Z-axis vector (WCS)
**
**		This definition of the UCS will be used instead of the current one.
**
**		BECAUSE THE DCS IS DEFINED IN TERMS OF THE UCS, THIS FUNCTION
**		SHOULD NOT BE CALLED WITH AN ALTERNATIVE UCS (altucs) TO
**		DO TRANSFORMATIONS INVOLVING THE DCS.  (WCS AND ECS SHOULD BE OKAY.)
*/
	char *headbuf;
	short sidx,arsz,defucs,fi1;
	int errln,inps,rc;
	sds_real cap=0.015625;	/* 1/64; square polar cap. */
	sds_real ar1,ar2;
	sds_point wpt,ucs[4],dcs[4],ecs[4],ap1,ap2,*whichsys;
	resbuf *whichrbp;
	db_sysvarlink *found;


	rc=RTNORM; errln=0L;

	if (pt==NULL || from==NULL || to==NULL || result==NULL || header.buf==NULL)
		{ rc=RTERROR; errln=__LINE__; goto out; }

	inps=inpspace();
	headbuf=header.buf; arsz=sizeof(sds_real);

	/* Init the working point: */
	wpt[0]=pt[0]; wpt[1]=pt[1]; wpt[2]=pt[2];

	/* If "from" and "to" are obviously the same systems, done: */
	if (from->restype==RTSHORT && to->restype==RTSHORT &&
		from->resval.rint==to->resval.rint) goto out;

	ecs[0][0]=ecs[0][1]=ecs[0][2]=0.0;	/* ECS and WCS origins coincide. */

	/* Set the UCS and DCS systems to defaults: */
	memset(ucs,0,sizeof(ucs)); memset(dcs,0,sizeof(ucs));
	ucs[1][0]=ucs[2][1]=ucs[3][2]=dcs[1][0]=dcs[2][1]=dcs[3][2]=1.0;

	/* Set up the UCS system, if necessary -- which means even if DCS */
	/* is all that's needed (since defining the DCS system requires */
	/* the UCS system): */
	if ((from->restype==RTSHORT && from->resval.rint) ||
		(  to->restype==RTSHORT &&	 to->resval.rint)) {

		if (altucs==NULL) {  /* Get the current UCS. */

			found=db_oldsysvar+((inps) ? DB_QP_UCSORG : DB_QUCSORG);
			memcpy(ucs[0]  ,headbuf+found->bc		   ,arsz);
			memcpy(ucs[0]+1,headbuf+found->bc+arsz	   ,arsz);
			memcpy(ucs[0]+2,headbuf+found->bc+arsz+arsz,arsz);

			defucs=0;  /* Set if we have to use the default axes. */

			found=db_oldsysvar+((inps) ? DB_QP_UCSXDIR : DB_QUCSXDIR);
			memcpy(ucs[1]  ,headbuf+found->bc		   ,arsz);
			memcpy(ucs[1]+1,headbuf+found->bc+arsz	   ,arsz);
			memcpy(ucs[1]+2,headbuf+found->bc+arsz+arsz,arsz);

			found=db_oldsysvar+((inps) ? DB_QP_UCSYDIR : DB_QUCSYDIR);
			memcpy(ucs[2]  ,headbuf+found->bc		   ,arsz);
			memcpy(ucs[2]+1,headbuf+found->bc+arsz	   ,arsz);
			memcpy(ucs[2]+2,headbuf+found->bc+arsz+arsz,arsz);

			/* Make the X- and Y-vectors unit: */
			for (fi1=1; fi1<3; fi1++) {
				if (icadRealEqual((ar1=sqrt(ucs[fi1][0]*ucs[fi1][0]+
									ucs[fi1][1]*ucs[fi1][1]+
									ucs[fi1][2]*ucs[fi1][2])),0.0)) break;
				ucs[fi1][0]/=ar1; ucs[fi1][1]/=ar1; ucs[fi1][2]/=ar1;
			}
			if (fi1<3) defucs=1;  /* One of them was the zero vector. */

			if (!defucs) {
				/* Z = X x Y */
				ucs[3][0]=ucs[1][1]*ucs[2][2]-ucs[1][2]*ucs[2][1];
				ucs[3][1]=ucs[1][2]*ucs[2][0]-ucs[1][0]*ucs[2][2];
				ucs[3][2]=ucs[1][0]*ucs[2][1]-ucs[1][1]*ucs[2][0];
				if (icadRealEqual((ar1=sqrt(ucs[3][0]*ucs[3][0]+
							  ucs[3][1]*ucs[3][1]+
							  ucs[3][2]*ucs[3][2])),0.0)) {

					defucs=1;
				} else {
					ucs[3][0]/=ar1; ucs[3][1]/=ar1; ucs[3][2]/=ar1;
				}
			}

			if (!defucs) {
				/* Now make SURE that the X- and Y-vectors are */
				/* perpendicular by redefining the Y-vector */
				/* using Z x X: */
				ucs[2][0]=ucs[3][1]*ucs[1][2]-ucs[3][2]*ucs[1][1];
				ucs[2][1]=ucs[3][2]*ucs[1][0]-ucs[3][0]*ucs[1][2];
				ucs[2][2]=ucs[3][0]*ucs[1][1]-ucs[3][1]*ucs[1][0];
			} else {  /* Use default UCS axes. */
				ucs[1][0]=ucs[2][1]=ucs[3][2]=1.0;
				ucs[1][1]=ucs[1][2]=ucs[2][0]=ucs[2][2]=ucs[3][0]=ucs[3][1]=0.0;
			}

		} else {  /* Use altucs. */

			memcpy(ucs,altucs,4*sizeof(sds_point));

		}

	}


	/*
	**	Now set up the DCS system, if necessary.
	**
	**	DEFINE IT ACCORDING TO THE WCS SYSTEM SO THAT WCS CAN BE
	**	THE INTERMEDIATE TRANSFORMATION SYSTEM EVEN FOR DCS.
	**	This turns out to be easy, since TARGET and VIEWDIR
	**	are WCS in the header, and since the DCS Z-axis comes from
	**	crossing the WCS Z-axis with VIEWDIR.
	**
	**	(DO THIS AFTER SETTING UP THE UCS SYSTEM).
	*/
	if ((from->restype==RTSHORT && from->resval.rint==2) ||
		(  to->restype==RTSHORT &&	 to->resval.rint==2)) {

		/* Set the DCS origin (WCS): */
		found=db_oldsysvar+DB_QTARGET;
		memcpy(dcs[0]  ,headbuf+found->bc		   ,arsz);
		memcpy(dcs[0]+1,headbuf+found->bc+arsz	   ,arsz);
		memcpy(dcs[0]+2,headbuf+found->bc+arsz+arsz,arsz);

		/* Set the unit DCS Z-axis: */
		found=db_oldsysvar+DB_QVIEWDIR;
		memcpy(dcs[3]  ,headbuf+found->bc		   ,arsz);
		memcpy(dcs[3]+1,headbuf+found->bc+arsz	   ,arsz);
		memcpy(dcs[3]+2,headbuf+found->bc+arsz+arsz,arsz);
		if (icadRealEqual((ar1=sqrt(dcs[3][0]*dcs[3][0]+
					  dcs[3][1]*dcs[3][1]+
					  dcs[3][2]*dcs[3][2])),0.0)) {  /* Zero vector */

			dcs[3][0]=dcs[3][1]=0.0; dcs[3][2]=1.0;  /* Set default */
		} else {
			dcs[3][0]/=ar1; dcs[3][1]/=ar1; dcs[3][2]/=ar1;
		}

		/* Set the unit DCS X-axis (WCSZ x DCSZ): */
		dcs[1][0]=-dcs[3][1]; dcs[1][1]= dcs[3][0]; dcs[1][2]=0.0;
		if ((ar1=sqrt(dcs[1][0]*dcs[1][0]+
					  dcs[1][1]*dcs[1][1]+
					  dcs[1][2]*dcs[1][2]))<IC_ZRO) {  /* Use WCS +X or -X. */

			dcs[1][0]=(dcs[3][2]<0.0) ? -1.0 : 1.0;
			dcs[1][1]=dcs[1][2]=0.0;
		} else {
			dcs[1][0]/=ar1; dcs[1][1]/=ar1; dcs[1][2]/=ar1;
		}

		/* Set the unit DCS Y-axis: */
		dcs[2][0]=dcs[3][1]*dcs[1][2]-dcs[3][2]*dcs[1][1];
		dcs[2][1]=dcs[3][2]*dcs[1][0]-dcs[3][0]*dcs[1][2];
		dcs[2][2]=dcs[3][0]*dcs[1][1]-dcs[3][1]*dcs[1][0];

		/* Get VIEWTWIST: */
		found=db_oldsysvar+DB_QVIEWTWIST;
		memcpy(&ar1,headbuf+found->bc,arsz);
		ar2=sin(ar1); ar1=cos(ar1);

		/*
		** Rotate the DCS X- and Y-axes CW by VIEWTWIST about the DCS Z-axis
		** and convert them to WCS to redefine dcs[1] and dcs[2].
		** The rotated X-axis is (cos(VT),-sin(VT),0) in the DCS.
		** The rotated Y-axis is (sin(VT), cos(VT),0) in the DCS.
		*/
		ap1[0]=ar1*dcs[1][0]-ar2*dcs[2][0];  /* New dcs[1] */
		ap1[1]=ar1*dcs[1][1]-ar2*dcs[2][1];
		ap1[2]=ar1*dcs[1][2]-ar2*dcs[2][2];

		ap2[0]=ar2*dcs[1][0]+ar1*dcs[2][0];  /* New dcs[2] */
		ap2[1]=ar2*dcs[1][1]+ar1*dcs[2][1];
		ap2[2]=ar2*dcs[1][2]+ar1*dcs[2][2];

		dcs[1][0]=ap1[0]; dcs[1][1]=ap1[1]; dcs[1][2]=ap1[2];
		dcs[2][0]=ap2[0]; dcs[2][1]=ap2[1]; dcs[2][2]=ap2[2];
	}


	for (sidx=0; sidx<2; sidx++) {	/* 0: "from" -> WCS; 1: WCS -> "to" */

		whichrbp=(sidx) ? to : from;
		whichsys=NULL;

		switch (whichrbp->restype) {

			case RTSHORT:

				if		(whichrbp->resval.rint==1) whichsys=ucs;
				else if (whichrbp->resval.rint==2) whichsys=dcs;
				else continue;	/* Already WCS (or unknown). */
				break;

			case RTENAME: case RT3DPOINT:

				whichsys=ecs;

				if (whichrbp->restype==RT3DPOINT) {

					ecs[3][0]=whichrbp->resval.rpoint[0];
					ecs[3][1]=whichrbp->resval.rpoint[1];
					ecs[3][2]=whichrbp->resval.rpoint[2];

				} else {
					db_handitem* pHandItem = (db_handitem *)(whichrbp->resval.rlname[0]);

					if (pHandItem->ret_type() == DB_VERTEX)
						{
						pHandItem = ((db_vertex *)pHandItem)->ret_main();
						ASSERT(pHandItem);
						}

					if (pHandItem)
						pHandItem->get_210(ecs[3]);
				}  /* End else RTENAME */

				/* Unitize the Z-axis: */
				if (icadRealEqual((ar1=sqrt(ecs[3][0]*ecs[3][0]+
							  ecs[3][1]*ecs[3][1]+
							  ecs[3][2]*ecs[3][2])),0.0)) {

					ecs[3][0]=ecs[3][1]=0.0; ecs[3][2]=1.0;  /* Default */
				} else {
					ecs[3][0]/=ar1; ecs[3][1]/=ar1; ecs[3][2]/=ar1;
				}

				if (icadRealEqual(ecs[3][2],1.0)) continue;  /* WCS==ECS */

				if (ic_arbaxis(ecs[3],ecs[1],ecs[2],ecs[3])==-1) {
					rc=RTERROR;
					errln=__LINE__;
					goto out;
				}

				break;

		} /* End switch (whichrbp->restype) */

		if (whichsys==NULL) continue;

		if (sidx) {  /* Transform from WCS to whichsys: */

			if (!disp) {
				wpt[0]-=whichsys[0][0];
				wpt[1]-=whichsys[0][1];
				wpt[2]-=whichsys[0][2];
			}

			ap1[0]=wpt[0]*whichsys[1][0]+
				   wpt[1]*whichsys[1][1]+
				   wpt[2]*whichsys[1][2];
			ap1[1]=wpt[0]*whichsys[2][0]+
				   wpt[1]*whichsys[2][1]+
				   wpt[2]*whichsys[2][2];
			ap1[2]=wpt[0]*whichsys[3][0]+
				   wpt[1]*whichsys[3][1]+
				   wpt[2]*whichsys[3][2];

		} else {  /* Transform from whichsys to WCS: */

			ap1[0]=wpt[0]*whichsys[1][0]+
				   wpt[1]*whichsys[2][0]+
				   wpt[2]*whichsys[3][0];
			ap1[1]=wpt[0]*whichsys[1][1]+
				   wpt[1]*whichsys[2][1]+
				   wpt[2]*whichsys[3][1];
			ap1[2]=wpt[0]*whichsys[1][2]+
				   wpt[1]*whichsys[2][2]+
				   wpt[2]*whichsys[3][2];

			if (!disp) {
				ap1[0]+=whichsys[0][0];
				ap1[1]+=whichsys[0][1];
				ap1[2]+=whichsys[0][2];
			}

		}

		wpt[0]=ap1[0]; wpt[1]=ap1[1]; wpt[2]=ap1[2];

	} /* End for sidx */


out:
	if (rc==RTNORM && result!=NULL)
		{ result[0]=wpt[0]; result[1]=wpt[1]; result[2]=wpt[2]; }


	return rc;
}


/*F*/
int db_drawing::movept2elev(sds_point sour, sds_point dest) {
/*
**	Given sour, sets dest as the point on the line through sour
**	in direction VIEWDIR at the current ELEVATION.	For example,
**	when ELEVATION or VIEWCTR is changed, this function can be
**	used to convert VIEWCTR so that it is at the current ELEVATION
**	along the same VIEWDIR.
**
**	sour and dest can refer to the same point.
**
**	Returns:
**		 0 : OK
**		-1 : sour or dest is NULL
**		-2 : Can't getvar() ELEVATION or VIEWDIR; dest set to sour
**		-3 : VIEWDIR is parallel to the XY-plane; dest set to sour
*/
	short rc;
	sds_real ar1;
	sds_point vd,wdest;
	resbuf rb;


	rc=0; rb.rbnext=NULL;

	if (sour==NULL || dest==NULL) { rc=-1; goto out; }

	ic_ptcpy(wdest,sour);

	if (db_getvar(NULL,DB_QVIEWDIR,&rb,this,NULL,NULL)!=RTNORM) { rc=-2; goto out; }
	if (fabs(rb.resval.rpoint[2]) < IC_ZRO) { rc=-3; goto out; }
	ic_ptcpy(vd,rb.resval.rpoint);
	if (db_getvar(NULL,DB_QELEVATION,&rb,this,NULL,NULL)!=RTNORM) { rc=-2; goto out; }

	ar1=(rb.resval.rreal-sour[2])/vd[2];
	wdest[0]=sour[0]+ar1*vd[0];
	wdest[1]=sour[1]+ar1*vd[1];
	wdest[2]=rb.resval.rreal;

out:
	if (dest!=NULL) ic_ptcpy(dest,wdest);
	return rc;
}

// ******************************************************************
/*
**	Returns a pointer to the handitem of the last non-deleted main entity
**	of whichever type: 0:MSPACE; 1:PSPACE; 2:Database.
*/
DB_API db_handitem *db_drawing::entlast(int which)
	{
	int ps;
	db_handitem *thip1;

	if (which<0)
		{
		which=0;
		}
	else if (which>2)
		{
		which=2;
		}

	if (entll[0]==NULL)
		{
		lastmainent[0]=lastmainent[1]=lastmainent[2]=NULL;
		}
	else if (lastmainent[which]==NULL)
		{
		for (thip1=entll[0]; thip1!=NULL; thip1=thip1->next)
			{
			ps=thip1->ret_pspace();
			if (!db_is_subentitytype(thip1->ret_type()) &&
				!thip1->ret_deleted() &&
//				!thip1->ret_invisible() &&
				(which==2 || (which==0 && !ps) || (which==1 && ps)))
				{
				lastmainent[which]=thip1;
				}
			}
		}

	return lastmainent[which];
	}


/*F*/
db_handitem *db_drawing::tblsearch(int tabtype, const char* recname, int setnext) {
	db_handitem *hip=NULL;

	if (tabtype>-1 && tabtype<DB_NTABS) {
		hip=findtabrec(tabtype,recname,1);
		if (hip!=NULL && setnext) {
			//table[tabtype].m_pNextTabRec=hip; *code review* it's meanless. 2003/4/30 Ebatech(CNBR)
			table[tabtype].m_pNextTabRec=hip->next;
			while (table[tabtype].m_pNextTabRec!=NULL &&
				table[tabtype].m_pNextTabRec->ret_deleted())
					table[tabtype].m_pNextTabRec=table[tabtype].m_pNextTabRec->next;
		}
	}

	return hip;
}


/*F*/
db_handitem*
db_drawing::tblnext
(
 int	tabtype,
 int	rewind
)
{
//	Given a DB_ table type code (like DB_UCSTAB), this function returns a ptr
//	to the handitem of the m_pNextTabRec ('tblnext') record for that table --
//	or NULL if there are no records or the last record has been passed
//	(or if tabtype is not valid).
//
//	Pass non-0 "rewind" to start over for that table.

	db_handitem	*rhip = NULL, *thip1;

	if(tabtype > -1 && tabtype < DB_NTABS)
	{
		if(rewind)
			rhip = table[tabtype].m_pTabRecLL[0];
		else
			rhip = table[tabtype].m_pNextTabRec;

		while(rhip && rhip->ret_deleted())
			rhip = rhip->next;

		// rhip now points to the first non-deleted item

		// advance the pointer to the next non-deleted item after rhip
		//
		if(!rhip)
			thip1 = NULL;
		else
			thip1 = rhip->next;

		while(thip1 && thip1->ret_deleted())
			thip1 = thip1->next;

		// thip1 now points to the next non-deleted item

		/*D.G.*/// Removed CybAge's changes.
		// Reason: wrong behaviour, especially for deleted records.

		/* // Modified Cybage AP 19/12/2000 [
		// Reason: To fix Bug No. 66368.
		static db_handitem*	last = NULL;
		if(rewind)	// If rewind then forget the last history.
			last = NULL;

		if(!thip1 && !last)
			last = rhip;
		if(last && last->next)
		{
			table[tabtype].m_pNextTabRec = last->next->next;
			rhip = last->next;
			last = rhip;
		}
		else */
			table[tabtype].m_pNextTabRec = thip1;
		/* // Modified Cybage AP 19/12/2000 ] */
	}

	return rhip;
}


/*F*/
int db_drawing::delhanditem(
	db_handitem *prevhip,
	db_handitem *hip,
	int 		 mode,
	bool bDeleteObject /*= true*/) {	/*DG - 27.4.2002*/// Sometimes we don't want to destroy the object when mode == 3.
/*
**	Sets or clears the "deleted" flag for a handitem -- or truly
**	removes it from its llist.
**
**	WARNING: You can create incomplete complex entities in the
**	database with this function.
**
**	Either of the handitem ptrs can be NULL -- but not both.
**
**	prevhip : Ptr to the prev handitem, if known, when REMOVING the link.
**				NULL if not known or just modifying the flag.
**		hip : Ptr to the handitem to work on.
**
**	   mode : 0 : Flag as deleted
**			  1 : Flag as valid
**			  2 : Toggle the flag
**			  3 : Actually delete the link.  (For speed, supply prevhip,
**					if possible).
**
**	If the deleted link is table[?].m_pNextTabRec, table[?].m_pNextTabRec is NULLed.
**	api_tblnext() (or the db_ version) will have to find the first
**	non-deleted record.  (For speed, not done here.)
**
**	If the deleted link is a lastmainent, the appropriate lastmainents
**	are NULLed.  api_entlast() (or the db_ version) will have to search
**	for the last non-deleted main entity.  (For speed, not done here.)
**	Ditto if a main entity is undeleted.
**
**	Returns:
**		 0 : OK
**		-1 : Calling error (invalid handitem(s) or mode.
**		-2 : hip not found when searching for prevhip.
**		-3 : Attempt to remove a standard table record (like layer 0).
*/
/*D.G.*/// Deleting of objects added.

	char fs1[IC_ACADNMLEN];
	int hiptype,tabtype,itsatab,itsamain,olddeleted,rc,fi1;
	db_handitem *thip1;


	rc=itsatab=0; itsamain=1;


	if (prevhip==NULL && hip==NULL) goto out;
	if (mode<0 || mode>3 || (prevhip!=NULL && (hip=prevhip->next)==NULL))
		{ rc=-1; goto out; }

	hiptype=hip->ret_type();
	itsatab=db_is_tabrectype(hiptype);
	/*DG - 15.5.2002*/// If the entity is a part of a block then it's not "a main entity"
	// (block here is not Model_Space or Paper_Space).
	itsamain=(!itsatab && !db_is_subentitytype(hiptype)) && !hip->m_parentBlock;
	olddeleted=hip->ret_deleted();	/* Save old setting */

	// No can delete viewport 1 (paperspace viewport)
	//
	if ( hiptype == DB_VIEWPORT )
		{
		int iVportNumber;
		hip->get_69( &iVportNumber );
		if ( iVportNumber == 1 )
			{
			rc = -3;
			goto out;
			}
		}

	if (itsatab) {	/* No can delete certain table records. */
		hip->get_2(fs1,sizeof(fs1)-1);
		switch (hiptype) {
			case DB_APPIDTABREC:
				if (strisame(fs1,db_str_acad)) { rc=-3; goto out; }
				break;
			case DB_DIMSTYLETABREC:
				//Bugzilla No. 78070 ; 17-06-2002
				//if (strisame(fs1,db_str_standard)) { rc=-3; goto out; }
				break;
			case DB_LAYERTABREC:
				if (strisame(fs1,db_str_0)) { rc=-3; goto out; }
				break;
			case DB_LTYPETABREC:
				if (strisame(fs1,db_str_bylayer) ||
					strisame(fs1,db_str_byblock) ||
					strisame(fs1,db_str_continuous)) { rc=-3; goto out; }
				break;
			case DB_STYLETABREC:
				if (strisame(fs1,db_str_standard)) { rc=-3; goto out; }
				break;
			case DB_VPORTTABREC:
				if (strisame(fs1,db_str_staractive)) { rc=-3; goto out; }
				break;
		}
	}

	if (mode<3) {  /* Just mess with the "deleted" bit. */
		if		(mode==0) hip->set_deleted(1);
		else if (mode==1) hip->set_deleted(0);
		else if (mode==2) hip->set_deleted(!hip->ret_deleted());
		if (itsamain && olddeleted!=hip->ret_deleted()) {  /* A main ent has been toggled */
			if (olddeleted) nmainents++; else if (--nmainents<0) nmainents=0;
		}
		if (hip->ret_deleted()) {
			if (itsatab) {
				tabtype=db_hitypeinfo[hiptype].tabtype;
				if (hip==table[tabtype].m_pNextTabRec) table[tabtype].m_pNextTabRec=NULL;
			} else {
				if (hip==lastmainent[0]) lastmainent[0]=NULL;
				if (hip==lastmainent[1]) lastmainent[1]=NULL;
				if (hip==lastmainent[2]) lastmainent[2]=NULL;
			}
		} else if (itsamain) lastmainent[hip->ret_pspace()!=0]=lastmainent[2]=NULL;
	} else {  /* mode==3 -- actually delete from the llist. */
		if (itsatab) {
			tabtype=db_hitypeinfo[hiptype].tabtype;
			if (prevhip==NULL) {
				for (thip1=table[tabtype].m_pTabRecLL[0]; thip1!=NULL &&
					thip1!=hip; prevhip=thip1,thip1=thip1->next);
				if (thip1==NULL) { rc=-2; goto out; }
			}
			if (prevhip==NULL) table[tabtype].m_pTabRecLL[0]=hip->next;
			else					  prevhip->next=hip->next;
			if (hip->next==NULL) table[tabtype].m_pTabRecLL[1]=prevhip;
			if (hip==table[tabtype].m_pNextTabRec) table[tabtype].m_pNextTabRec=NULL;
		} else {
			if(db_is_objecttype(hiptype))	/*D.G.*/// Deleting of objects added.
			{
				if(!prevhip)
				{
					for(thip1 = objll[0]; thip1 && thip1 != hip; prevhip = thip1, thip1 = thip1->next);
					if(!thip1)
					{
						rc = -2;
						goto out;
					}
				}
				if(!prevhip)
					objll[0] = hip->next;
				else
					prevhip->next = hip->next;
				if(!hip->next)
					objll[1] = prevhip;

				delete hip;
				goto out;
			}
			else
			{
				if (prevhip==NULL) {
					for (thip1=entll[0]; thip1!=NULL &&
						thip1!=hip; prevhip=thip1,thip1=thip1->next);
					if (thip1==NULL) { rc=-2; goto out; }
				}
				if (prevhip==NULL) entll[0]=hip->next;
				else	   prevhip->next=hip->next;
				if (hip->next==NULL) entll[1]=prevhip;
				for (fi1=0; fi1<sizeof(lastmainent)/sizeof(lastmainent[0]); fi1++)
					if (hip==lastmainent[fi1] &&
						(lastmainent[fi1]=prevhip)!=NULL &&
						db_is_subentitytype(lastmainent[fi1]->ret_type()))
							lastmainent[fi1]=NULL;
			}
		}
		if (itsamain && !olddeleted && --nmainents<0) nmainents=0;
		/*DG - 27.4.2002*/// delete an according db_vxtabrec
		// TO DO: should we implement the same behaviour for other modes (0-2)?
		if(hip->ret_type() == DB_VIEWPORT)
		{
			db_handitem*	pVxtabrec = tblnext(DB_VXTAB, 1);
			for( ; pVxtabrec; pVxtabrec = tblnext(DB_VXTAB, 0))
				if(static_cast<db_vxtabrec*>(pVxtabrec)->ret_vpehip() == hip)
				{
					delhanditem(NULL, pVxtabrec, 3);
					break;
				}
		}
		if(bDeleteObject)
			delete hip;
	}

out:
	return rc;
}


/*F*/
db_handitem *db_drawing::handent(db_objhandle phand)
	{
	ASSERT( handleTable);
	ASSERT( phand!=NULL);

	if (phand!=NULL)
		{
		if ( handleTable )
			return handleTable->Find( phand);
		}
	// should never happen
	return NULL;
	}



/*F*/
DB_API int db_drawing::initucs2ent(
	sds_point *xfa,
	sds_point  extru,	/* See notes below about this one. */
	sds_real  *dangp,
	short	   mode) {
/*
**	This function and db_drawing::ucs2ent() are designed to transform
**	input pts (UCS) to the system native to a particular entity
**	(WCS or ECS) so that data the user enters in the UCS can be
**	converted into the correct values to store internally (in a
**	db_handitem or an elist for entmaking).
**
**	Sets up for transformation from UCS to the native systems for
**	entities (WCS or the entity-creation ECS).
**
**	mode:
**
**		0 : UCS to WCS.
**		1 : UCS to the entity-creation ECS.
**
**	xfa should be an array of at least 4 sds_points.  It is set up
**	for future calls to db_drawing::ucs2ent().	It is initialized in the
**	current function to define the UCS according to the WCS (mode==0)
**	or according to the entity-creation ECS (mode==1):
**
**		xfa[0] : Origin
**		xfa[1] : X-axis unit vector
**		xfa[2] : Y-axis unit vector
**		xfa[3] : Z-axis unit vector
**
**	Pass parameter extru is treated in one of the following three
**	ways, based on the value passed:
**
**	  NULL:
**
**		extru is ignored (not used, not set).  The ECS is defined
**		by the Z-axis of the current UCS.
**
**	  (0,0,0) :
**
**		extru is set to the unit Z-axis of the current UCS -- which is
**		the extrusion direction (210 group) for any entity created in
**		that UCS via one of the drawing commands (LINE, ARC, etc.).
**		The ECS is defined by this vector (the current UCS's Z-axis).
**
**	  NOT (0,0,0):
**
**		For mode 1, extru is used as the entity's extrusion (instead
**		of getting it from the current UCS's Z-axis). The ECS is
**		defined by this vector (as passed -- not by the current UCS's
**		Z-axis). Not used during mode 0.  Not altered in either mode.
**
**	dangp should be either the address of an sds_real or NULL.
**	If it is non-NULL AND mode==1, it is set to the angle offset that
**	needs to be added to a UCS angle or rotation to get the
**	equivalent in the entity-creation ECS.	(Note that this angle
**	is only meaningful when the UCS and ECS share the same Z-axis
**	direction -- which may not be the case when extru is passed
**	as non-(0,0,0).)
**
**	Notice that this function can be used to retrieve the current
**	UCS origin and unit axes according to the WCS (determined from
**	UCSORG, UCSXDIR, and UCSYDIR) when mode==0.
**
**	When an error occurs, xfa, extru, and *dangp are set to appropriate
**	default values.
**
**	DEFINITION: THE ENTITY-CREATION ECS:
**
**		The entity-creation ECS is the Entity Coordinate System whose
**		Z-axis is parallel to the UCS's Z-axis.  When planar entities
**		are created by the user (the ARC command, for example), this is
**		the ECS in which they are created.
**
**	Returns:
**		 0 : OK
**		-1 : Calling error (No header data)
**		-2 : System variable not found
**		-3 : Unexpected error in arbitrary axis determination
**			   (avoiding a divide-by-0 that should never be possible)
*/
	char *header;
	short arsz,defucsw,inps,setextru,rc,fi1,fi2;
	sds_real ar1;
	sds_real cap=0.015625;	/* 1/64; square polar cap. */
	sds_point ucsw[4],ucse[4],ecs[3];
	struct db_sysvarlink *found;


	rc=0;


	header=ret_headerbuf(); arsz=sizeof(sds_real); inps=inpspace();

	setextru=0;  /* Flags whether or not we need to set extru for the caller. */
	if (extru!=NULL) {
		/* See if we're going to USE it or SET it, and unitize it in the */
		/* former case: */
		if (icadRealEqual((ar1=sqrt(extru[0]*extru[0]+extru[1]*extru[1]+extru[2]*extru[2])),0.0)) {
			setextru=1;
		} else if (ar1!=1.0) {
			extru[0]/=ar1; extru[1]/=ar1; extru[2]/=ar1;
		}
	}


  /* Safe to goto out beyond here. */


	/* Get ucsw[] -- the UCS according to the WCS: */

	found=db_oldsysvar+((inps) ? DB_QP_UCSORG : DB_QUCSORG);
	memcpy(ucsw[0]	,header+found->bc		   ,arsz);
	memcpy(ucsw[0]+1,header+found->bc+arsz	   ,arsz);
	memcpy(ucsw[0]+2,header+found->bc+arsz+arsz,arsz);

	found=db_oldsysvar+((inps) ? DB_QP_UCSXDIR : DB_QUCSXDIR);
	memcpy(ucsw[1]	,header+found->bc		   ,arsz);
	memcpy(ucsw[1]+1,header+found->bc+arsz	   ,arsz);
	memcpy(ucsw[1]+2,header+found->bc+arsz+arsz,arsz);

	found=db_oldsysvar+((inps) ? DB_QP_UCSYDIR : DB_QUCSYDIR);
	memcpy(ucsw[2]	,header+found->bc		   ,arsz);
	memcpy(ucsw[2]+1,header+found->bc+arsz	   ,arsz);
	memcpy(ucsw[2]+2,header+found->bc+arsz+arsz,arsz);

	/* Make the X- and Y-vectors unit: */
	for (fi1=1; fi1<3; fi1++) {
		if (icadRealEqual((ar1=sqrt(ucsw[fi1][0]*ucsw[fi1][0]+
					  ucsw[fi1][1]*ucsw[fi1][1]+
					  ucsw[fi1][2]*ucsw[fi1][2])),0.0)) break;
		ucsw[fi1][0]/=ar1; ucsw[fi1][1]/=ar1; ucsw[fi1][2]/=ar1;
	}
	defucsw=(fi1<3);  /* One of them was the zero vector. */

	if (!defucsw) {
		/* Z = X x Y */
		ucsw[3][0]=ucsw[1][1]*ucsw[2][2]-ucsw[1][2]*ucsw[2][1];
		ucsw[3][1]=ucsw[1][2]*ucsw[2][0]-ucsw[1][0]*ucsw[2][2];
		ucsw[3][2]=ucsw[1][0]*ucsw[2][1]-ucsw[1][1]*ucsw[2][0];
		if (icadRealEqual((ar1=sqrt(ucsw[3][0]*ucsw[3][0]+
					  ucsw[3][1]*ucsw[3][1]+
					  ucsw[3][2]*ucsw[3][2])),0.0)) {

			defucsw=1;
		} else {
			ucsw[3][0]/=ar1; ucsw[3][1]/=ar1; ucsw[3][2]/=ar1;
		}
	}

	if (!defucsw) {
		/* Now make SURE that the X- and Y-vectors are */
		/* perpendicular by redefining the Y-vector */
		/* using Z x X: */
		ucsw[2][0]=ucsw[3][1]*ucsw[1][2]-ucsw[3][2]*ucsw[1][1];
		ucsw[2][1]=ucsw[3][2]*ucsw[1][0]-ucsw[3][0]*ucsw[1][2];
		ucsw[2][2]=ucsw[3][0]*ucsw[1][1]-ucsw[3][1]*ucsw[1][0];
	} else {  /* Use default UCS axes. */
		ucsw[1][0]=ucsw[2][1]=ucsw[3][2]=1.0;

				   ucsw[1][1]=ucsw[1][2]=
		ucsw[2][0]			 =ucsw[2][2]=
		ucsw[3][0]=ucsw[3][1]			=0.0;
	}


	if (!mode) goto out;  /* Got ucsw, and that's all we want. */


	/* Get the ECS ... */
	if (extru!=NULL && !setextru) {  /* ... from extru as passed. */
		ecs[2][0]=extru[0]; ecs[2][1]=extru[1]; ecs[2][2]=extru[2];
	} else {  /* ... from the Z-axis of the current UCS. */
		ecs[2][0]=ucsw[3][0]; ecs[2][1]=ucsw[3][1]; ecs[2][2]=ucsw[3][2];
	}

	if (icadRealEqual(ecs[2][2],1.0)) {  /* WCS==ECS */
		ecs[0][0]=ecs[1][1]=1.0;

				  ecs[0][1]=ecs[0][2]=
		ecs[1][0]		   =ecs[1][2]=
		ecs[2][0]=ecs[2][1] 		 =0.0;
	} else {
		if (ic_arbaxis(ecs[2],ecs[0],ecs[1],ecs[2])==-1) {
			rc=-3;
			goto out;
		}
	}


	/* Get ucse[] -- the UCS according to the ECS: */

	/* Get the ucse origin by projecting the WCS vector from the WCS */
	/* origin to the UCS origin onto the ECS axes: */
	for (fi1=0; fi1<3; fi1++)
		ucse[0][fi1]=ucsw[0][0]*ecs[fi1][0]+
					 ucsw[0][1]*ecs[fi1][1]+
					 ucsw[0][2]*ecs[fi1][2];

	/* Get the ucse axes by finding the projections of the ucsw axes */
	/* on the ecs axes: */
	for (fi1=1; fi1<4; fi1++)
		for (fi2=0; fi2<3; fi2++)
			ucse[fi1][fi2]=ucsw[fi1][0]*ecs[fi2][0]+
						   ucsw[fi1][1]*ecs[fi2][1]+
						   ucsw[fi1][2]*ecs[fi2][2];


out:
	if (rc) {
		if (xfa!=NULL) {
			xfa[1][0]=xfa[2][1]=xfa[3][2]=1.0;

			xfa[0][0]=xfa[0][1]=xfa[0][2]=
					  xfa[1][1]=xfa[1][2]=
			xfa[2][0]		   =xfa[2][2]=
			xfa[3][0]=xfa[3][1] 		 =0.0;
		}
		if (setextru) { extru[0]=extru[1]=0.0; extru[2]=1.0; }
		if (dangp!=NULL) *dangp=0.0;
	} else {
		if (xfa!=NULL) memcpy(xfa,(mode) ? ucse : ucsw,sizeof(ucsw));
		if (setextru) { extru[0]=ucsw[3][0]; extru[1]=ucsw[3][1]; extru[2]=ucsw[3][2]; }
		if (dangp!=NULL) {
			if (mode) {
				*dangp=atan2(ucse[1][1],ucse[1][0]);
				if (*dangp<0.0) *dangp+=IC_TWOPI;
			} else *dangp=0.0;
		}
	}

	return rc;
}


/*F*/
DB_API int db_drawing::ucs2ent(
	sds_point  sour,
	sds_point  dest,
	sds_point *xfa) {
/*
**	Transforms sour to dest given an array of 4 sds_points, xfa,
**	that defines the source system in terms of the destination system.
**
**	SET UP xfa FIRST BY CALLING db_drawing::initucs2ent() WITH THE CORRECT mode.
**
**	Although this function is rather generic, it was written
**	to transform UCS input pts to either WCS or ECS pts for
**	defining entities.	That means xfa needs to define the UCS
**	according to either the WCS or the entity-creation ECS (which is
**	the ECS whose Z-axis is parallel to the UCS's Z-axis).
**	See db_drawing::initucs2ent().
**
**	xfa is expected to defining the source system according to the
**	destination system as follows:
**
**		xfa[0] : Origin
**		xfa[1] : X-axis
**		xfa[2] : Y-axis
**		xfa[3] : Z-axis
**
**	sour and dest may refer to the same point in memory.
**
**	Returns:
**		 0 : OK
**		-1 : Calling error (something NULL).
*/
	sds_point sp;


	if (sour==NULL || dest==NULL || xfa==NULL || xfa[0]==NULL) return -1;

	sp[0]=sour[0]; sp[1]=sour[1]; sp[2]=sour[2];

	dest[0]=xfa[0][0]+sp[0]*xfa[1][0]+sp[1]*xfa[2][0]+sp[2]*xfa[3][0];
	dest[1]=xfa[0][1]+sp[0]*xfa[1][1]+sp[1]*xfa[2][1]+sp[2]*xfa[3][1];
	dest[2]=xfa[0][2]+sp[0]*xfa[1][2]+sp[1]*xfa[2][2]+sp[2]*xfa[3][2];

	return 0;
}
//Added Cybage AJK 29/10/2001 [
//Reason --Fix for bug no 77865 from Bugzilla
bool validateEnt(struct resbuf *elist,int hittype);
//Added Cybage AJK 29/10/2001 DD/MM/YYYY ]
int db_drawing::entmake(struct resbuf *elist, db_handitem **hipp) {
/*
**	Creates a new db_handitem based on the type (and table record name,
**	if it's a table record), calls db_handitem::entmod() with elist
**	to set all the values, and then adds it to the database
**	via db_drawing::addhanditem().
**
**	When hipp!=NULL and the new handitem is successfully added,
**	*hipp is pointed at the new handitem for the caller.
**	(It's a default arg (NULL); see the prototype.)
**
**	Returns db_drawing::addhanditem's return code (currently 0 for OK and -1
**	for failure).  Upon failure, the caller can consult db_ret_lasterror()
**	for the reason.
**
**	NOTE: Our initial idea was to have this function return a pointer
**	to the db_handitem created if it was successfully added to the
**	database.  This feature is not currently needed.  However,
**	if we DO add that capability someday, be aware of the "pending"
**	logic that is going on in db_drawing::addhanditem().
**	A db_handitem successfully added to pendentmake will never make
**	it to the actual database if something cancels the pending
**	complex entity (or block def).	For example, this function might
**	added a complex INSERT and several ATTRIBs successfully and return
**	their pointers to the caller -- but if something later cancels
**	the pending complex INSERT, the pending llist will be freed and
**	those pointers will be invalid (since they pointed into the
**	PENDING llist -- not entll).  So, be CAREFUL.
*/
	char *tabrecname;
	int itsatabrec,hitype,rc;
	sds_real ar1;
	struct resbuf *typelink,*recnamelink,*trbp1;
	db_handitem *newhip;
	db_objhandle oldhandle;


	if (FixingUp1005s()) {
		oldhandle.Nullify();
		struct resbuf *typerb=ic_ret_assoc(elist,0);
		struct resbuf *oldhanrb = NULL;

		if (typerb)					// this can be NULL, since elist can be NULL
			{
			if (strsame(typerb->resval.rstring,"DIMSTYLE"/*DNT*/))
				oldhanrb=ic_ret_assoc(elist,105);
			else oldhanrb=ic_ret_assoc(elist,5);
			}

		if (oldhanrb!=NULL)
			oldhandle=oldhanrb->resval.rstring;
	}

	rc=0; db_lasterror=0;

	if (hipp!=NULL) *hipp=NULL;

	/* If elist is NULL, cancel pending.  (Not an error.) */
	if (elist==NULL) { addhanditem(NULL); goto out; }

	/* Find the type (0) and tab rec name (2): */
	typelink=recnamelink=NULL; hitype=-1; tabrecname=NULL; itsatabrec=0;
	for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
		if (trbp1->restype==0) {
			typelink=trbp1;
			if (trbp1->resval.rstring!=NULL) {
				/*
				**	Note special-handling for "BLOCK", since it's
				**	the name of a tab rec AND an entity.  We interpret
				**	it as the entity and require "BLOCKTABREC"
				**	for the table record.
				*/
				if (strisame("BLOCKTABREC",trbp1->resval.rstring)) {
					hitype=DB_BLOCKTABREC;
				} else if (strisame("BLOCK",trbp1->resval.rstring)) {
					hitype=DB_BLOCK;
				} else if ((hitype=db_str2hitype(trbp1->resval.rstring))<0) {
					continue;
				}
				if (!(itsatabrec=db_is_tabrectype(hitype)) ||
					recnamelink!=NULL) break;
			}
		} else if (trbp1->restype==2) {
			recnamelink=trbp1;
			if ((tabrecname=trbp1->resval.rstring)==NULL) continue;
			if (typelink!=NULL) break;
		}
	}

	/* Check for errors concerning type and tabrec name (if applicable). */
	/* (Remember: STYLE recs for SHAPEs are named "".) */
	if (typelink==NULL)
		{
		rc=-1;
		db_lasterror=OL_EMMFTYPE;
		goto out;
		}

	if (hitype<0)
		{
		rc=-1;
		db_lasterror=OL_EMKTYPE;
		goto out;
		}
	if (itsatabrec && (recnamelink==NULL || tabrecname==NULL ||
		(hitype!=DB_STYLETABREC && !*tabrecname)))
			{
			rc=-1;
			db_lasterror=OL_EMKMANDF;
			goto out;
			}
	//Added Cybage AJK 29/10/2001 [
	//Reason-fix for bug no 77865 from bugzilla
	if(!validateEnt(elist,hitype))
	{
		rc=-1;
		goto out;
	}
	//Added Cybage AJK 29/10/2001 DD/MM/YYYY ]

	/* Make the appropriate object and point newhip at it: */
	if ((newhip=db_makehip(hitype,this,tabrecname))==NULL)
		{
		rc=-1;
		db_lasterror=OL_EMKTYPE;
		goto out;
		}

	/*
	**	If it's a VERTEX, set default widths from the POLYLINE.
	**	The entmod will  override with any widths present in elist.
	*/
	if (hitype==DB_VERTEX && complexmain!=NULL) {
		complexmain->get_40(&ar1); newhip->set_40(ar1);
		complexmain->get_41(&ar1); newhip->set_41(ar1);
	}

	// 7/99 now assign handle before entmod(was assigned in addhanditem).  This way entities with
	// reactors will be able to point the reactors back to themselves during entmake.
	newhip->assignhand(this);

	// while an entity is being copied we don't want new associations to be made.  The old
	// associations will have to be resolved separately.
	if (m_copyFlag)
		newhip->SetCreateAssocFlag(false);

	if (newhip->entmod(elist,this))  /* Sets db_lasterror. */
		{
		delete newhip;
		newhip=NULL;
		rc=-1;
		goto out;
	}

	// set assoc back to true
	if (m_copyFlag)
		newhip->SetCreateAssocFlag(true);

	rc=addhanditem(newhip);  /* Sets db_lasterror. */
	if (!rc && hipp!=NULL) *hipp=newhip;

out:
	if (rc && (somethingpending() || pendentmake[0]!=NULL))
		addhanditem(NULL);	/* Cancel pending (if we didn't already). */

	if (elist && !rc && FixingUp1005s())
		{
		if (newhip)
			{
			if (oldhandle!=NULL)  // if there was an OLD one, put it and the new one in the 1005 table, if any
				{
				if (oldhandle!=newhip->RetHandle())
					m_fixup1005HandleMap->Add(oldhandle,newhip->RetHandle());
				}
			if (newhip->Has1005Eed(this))	// if this item has 1005 data, add it to the fixup list
				m_fixup1005HipTable->Add(newhip);
			}
		}

	return rc;
}




/*F*/
void db_drawing::resetfonts(void) {
/*
**	Frees db_drawing::fontll and rebuilds it again by cycling through
**	the STYLE table records resetting the font and bigfont values
**	via db_setfontusingmap().  This should be performed whenever
**	the system variable FONTMAP is changed so that the new fontmap
**	will take effect upon the next regen.
*/
	char *fcp1;
	db_fontlink *tfp;
	db_styletabrec *srec;
	db_fontsubstmode fsm;

	/* Free fontll: */
	while (fontll!=NULL) { tfp=fontll->next; delete fontll; fontll=tfp; }

	for (srec=(db_styletabrec *)table[DB_STYLETAB].m_pTabRecLL[0]; srec!=
		NULL; srec=(db_styletabrec *)srec->next) {

		if (srec->ret_flags() && IC_SHAPEFILE_SHAPES) fsm=IC_NO_ALLOW_ICAD_FNT;
		else fsm=IC_ALLOW_ICAD_FNT;

			srec->set_font(NULL); fcp1=NULL; srec->get_3(&fcp1);
		if (fcp1!=NULL && *fcp1 && db_setfontusingmap(fcp1,NULL,&tfp,fsm,this)>=0)
			srec->set_font(tfp);
		srec->set_bigfont(NULL); fcp1=NULL; srec->get_4(&fcp1);
		if (fcp1!=NULL && *fcp1 && db_setfontusingmap(fcp1,NULL,&tfp,fsm,this)>=0)
			srec->set_bigfont(tfp);
	}
}


/*F*/
int db_drawing::dictadd(db_handitem *dicthip, char *recname, db_handitem *rechip, int HiRefType /* = DB_SOFT_OWNER*/) {
/*
**	Returns:
**		 0 : OK
**		-1 : NULL pass parameter (or "" recname)
**		-2 : dicthip isn't a DICTIONARY
**		-3 : rechip isn't an OBJECT
**		-4 : recname already exists in this dictionary
*/
	int rc;
	db_dictionary *dictp;
	db_dictionaryrec *newrec;

	rc=0;

	if (dicthip==NULL || recname==NULL ||
		!*recname || rechip==NULL)			   { rc=-1; goto out; }
	if (!db_is_objecttype(rechip->ret_type())) { rc=-3; goto out; }
	// Ebatech(cnbr) -[ 2002/9/2 Bugzilla78227
	//if (dicthip->ret_type()!=DB_DICTIONARY)	 { rc=-2; goto out; }
	if (dicthip->ret_type() == DB_DICTIONARY)
	{
	// Ebatech(cnbr) ]-
		dictp=(db_dictionary *)dicthip;
		// If this is the named object dictionary, and this record exists, remove it
		// and add the newer version. This is an artifact from the db_dictionaryrec
		// not being able to know when it has been deleted. I have tested this and
		// it seems harmless. This can happen when a programmatically created image is
		// undone. The image dictionary is correctly deleted, but findrec() below still
		// sees it and there are no flags to check in a db_dictionaryrec to see that
		// it has been deleted. MB
		if (dictp == namedobjdict())
		{
			if (dictp->findrec(recname,0,this)!=NULL)
			{
				dictdel(dictp, recname);
			}
		}
		else
		{
			if (dictp->findrec(recname,0,this)!=NULL)
			{
				rc=-4; goto out;
			}
		}
		newrec=new db_dictionaryrec(recname,
			(HiRefType > 0 && HiRefType < 5) ? HiRefType : DB_SOFT_OWNER,
			rechip->RetHandle(),
			rechip);
		dictp->addrec(newrec);
	// Ebatech(cnbr) -[ 2002/9/2 Bugzilla78227
	}
	else if (dicthip->ret_type() == DB_DICTIONARYWDFLT)
	{
		db_dictionarywdflt *dicwp = (db_dictionarywdflt *)dicthip;
		if (dicwp->findrec(recname,0,this)!=NULL)
		{
			rc=-4; goto out;
		}
		newrec=new db_dictionaryrec(recname, DB_SOFT_OWNER,
			rechip->RetHandle(),
			rechip);
		dicwp->addrec(newrec);
		if(rechip->ret_type() == DB_PLACEHOLDER ){
			((db_placeholder*)rechip)->set_ownerItem(dicthip);
		}
	}
	else { rc=-2; goto out; }
	// Ebatech(cnbr) ]-
out:
	return rc;
}


/*F*/
int db_drawing::dictdel(db_handitem *dicthip, char *recname) {
/*
**	Returns:
**		 0 : OK
**		-1 : NULL pass parameter (or "" recname)
**		-2 : recname not found
**		-3 : dicthip isn't a DICTIONARY
*/
	int rc;

	rc=0;
	if (dicthip==NULL)						{ rc=-1; goto out; }
	// Ebatech(cnbr) -[ 2002/9/2 Bugzilla78227
	//if (dicthip->ret_type()!=DB_DICTIONARY) { rc=-3; goto out; }
	//rc=((db_dictionary *)dicthip)->delrec(recname);
	if (dicthip->ret_type() == DB_DICTIONARY )
	{
	// Ebatech(cnbr) ]-
		rc=((db_dictionary *)dicthip)->delrec(recname);
	// Ebatech(cnbr) -[ 2002/9/2 Bugzilla78227
	}
	else if (dicthip->ret_type() == DB_DICTIONARYWDFLT )
	{
		db_dictionarywdflt *dictw = (db_dictionarywdflt *)dicthip;
		db_dictionaryrec *result = dictw->findrec(recname,0,this);
		db_handitem* ph;
		if( result ){
			ph = result->hiref->ret_hip(this);
			if( ph->ret_type() == DB_PLACEHOLDER ){
				((db_placeholder*)ph)->set_ownerItem(NULL);
			}
		}
		rc=dictw->delrec(recname);
	}
	else  { rc=-3; goto out; }
	// Ebatech(cnbr) ]-
out:
	return rc;
}


/*F*/
int db_drawing::dictrename(db_handitem *dicthip, char *oldname, char *newname) {
/*
**	Returns:
**		 0 : OK
**		-1 : NULL pass parameter (or "" name)
**		-2 : oldname not found
**		-3 : newname already exists
**		-4 : dicthip isn't a DICTIONARY
*/
	int rc;

	rc=0;
	if (dicthip==NULL)						{ rc=-1; goto out; }
	// Ebatech(cnbr) -[ 2002/9/2 Bugzilla78227
	//if (dicthip->ret_type()!=DB_DICTIONARY) { rc=-4; goto out; }
	//rc=((db_dictionary *)dicthip)->renamerec(oldname,newname,this);
	if (dicthip->ret_type() == DB_DICTIONARY)
	{
	// Ebatech(cnbr) ]-
		rc=((db_dictionary *)dicthip)->renamerec(oldname,newname,this);
	// Ebatech(cnbr) -[ 2002/9/2 Bugzilla78227
	}
	else if (dicthip->ret_type() == DB_DICTIONARYWDFLT )
	{
		rc=((db_dictionarywdflt *)dicthip)->renamerec(oldname,newname,this);
	}
	else { rc=-4; goto out; }
	// Ebatech(cnbr) ]-
out:
	return rc;
}


/*F*/
db_handitem *db_drawing::dictnext(db_handitem *dicthip, int rewind) {
	db_handitem *ret;
	db_dictionary *dictp;
	db_dictionaryrec *tdrp1;

	ret=NULL;

	// Ebatech(cnbr) -[ 2002/9/2 Bugzilla78227
	//if (dicthip==NULL || dicthip->ret_type()!=DB_DICTIONARY) goto out;
	if( dicthip == NULL ) goto out;
	if( dicthip->ret_type() == DB_DICTIONARY )
	{
	// Ebatech(cnbr) ]-
		dictp=(db_dictionary *)dicthip;
		if (rewind) {
			dictp->get_recllends(&tdrp1,NULL);
			dictp->set_acadnextrec(tdrp1);
		} else {
			tdrp1=dictp->ret_acadnextrec();
		}

		ret=(tdrp1==NULL || tdrp1->hiref==NULL) ? NULL : tdrp1->hiref->ret_hip(this);
		dictp->stepacadnextrec(this);

		while(ret != NULL) {
			if (ret->ret_deleted()) {
				tdrp1=dictp->ret_acadnextrec();
				ret=(tdrp1==NULL || tdrp1->hiref==NULL) ? NULL : tdrp1->hiref->ret_hip(this);
				dictp->stepacadnextrec(this);
			}else
				goto out;
		}
	// Ebatech(cnbr) -[ 2002/9/2 Bugzilla78227
	}
	else if( dicthip->ret_type() == DB_DICTIONARYWDFLT )
	{
		db_dictionarywdflt *dicwp;

		dicwp=(db_dictionarywdflt *)dicthip;
		if (rewind) {
			dicwp->get_recllends(&tdrp1,NULL);
			dicwp->set_acadnextrec(tdrp1);
		} else {
			tdrp1=dicwp->ret_acadnextrec();
		}

		ret=(tdrp1==NULL || tdrp1->hiref==NULL) ? NULL : tdrp1->hiref->ret_hip(this);
		dicwp->stepacadnextrec(this);

		while(ret != NULL) {
			if (ret->ret_deleted()) {
				tdrp1=dicwp->ret_acadnextrec();
				ret=(tdrp1==NULL || tdrp1->hiref==NULL) ? NULL : tdrp1->hiref->ret_hip(this);
				dicwp->stepacadnextrec(this);
			}else
				goto out;
		}
	}
	// Ebatech(cnbr) ]-
out:
	return ret;
}


/*F*/
db_handitem *db_drawing::icaddictnext(db_handitem *dicthip, int rewind) {
	db_handitem *ret;
	db_dictionary *dictp;
	db_dictionaryrec *tdrp1;

	ret=NULL;

	// Ebatech(cnbr) -[ 2002/9/2 Bugzilla78227
	//if (dicthip==NULL || dicthip->ret_type()!=DB_DICTIONARY) goto out;
	if( dicthip == NULL ) goto out;
	if( dicthip->ret_type() == DB_DICTIONARY )
	{
	// Ebatech(cnbr) ]-
		dictp=(db_dictionary *)dicthip;
		if (rewind) {
			dictp->get_recllends(&tdrp1,NULL);
			dictp->set_icadnextrec(tdrp1);
		} else tdrp1=dictp->ret_icadnextrec();
		ret=(tdrp1==NULL || tdrp1->hiref==NULL) ?
			NULL : tdrp1->hiref->ret_hip(this);
		dictp->stepicadnextrec();
	// Ebatech(cnbr) -[ 2002/9/2 Bugzilla78227
	}
	else if( dicthip->ret_type() == DB_DICTIONARYWDFLT )
	{
		db_dictionarywdflt *dicwp;

		dicwp=(db_dictionarywdflt *)dicthip;
		if (rewind) {
			dicwp->get_recllends(&tdrp1,NULL);
			dicwp->set_icadnextrec(tdrp1);
		} else tdrp1=dicwp->ret_icadnextrec();
		ret=(tdrp1==NULL || tdrp1->hiref==NULL) ?
			NULL : tdrp1->hiref->ret_hip(this);
		dicwp->stepicadnextrec();
	}
out:
	return ret;
}


/*F*/
db_handitem *db_drawing::dictsearch(db_handitem *dicthip, char *recname, int setnext) {
	db_handitem *ret;
	db_dictionary *dictp;
	db_dictionaryrec *tdrp1;

	ret=NULL;

	// Ebatech(cnbr) -[ 2002/9/2 Bugzilla78227
	//if (dicthip==NULL || dicthip->ret_type()!=DB_DICTIONARY) goto out;
	if( dicthip == NULL ) goto out;
	if( dicthip->ret_type() == DB_DICTIONARY ){
	// ebatech(cnbr) ]-
		dictp=(db_dictionary *)dicthip;
		tdrp1=dictp->findrec(recname,setnext,this);
	// Ebatech(cnbr) -[ 2002/9/2 Bugzilla78227
	}else if( dicthip->ret_type() == DB_DICTIONARYWDFLT ){
		db_dictionarywdflt *dicwp = (db_dictionarywdflt *)dicthip;
		tdrp1 = dicwp->findrec( recname, setnext, this);
	}else goto out;
	// ebatech(cnbr) ]-
	ret=(tdrp1==NULL || tdrp1->hiref==NULL) ?
		NULL : tdrp1->hiref->ret_hip(this);

out:
	// Should return NULL if the record is deleted.
	if (ret == NULL)
		return(ret);
	else
		return (!ret->ret_deleted() ? ret : NULL);
}


/*F*/
bool db_drawing::inpspace(void) {
	bool rc=false;
	short fsh1,fsh2;

	if (header.buf!=NULL) {
		memcpy(&fsh1,header.buf+db_oldsysvar[DB_QTILEMODE].bc,2);
		memcpy(&fsh2,header.buf+db_oldsysvar[DB_QCVPORT].bc  ,2);
		if (!fsh1 && fsh2==1) rc=true;
	}
	return rc;
}



// ***************************************
// PUBLIC METHOD -- class db_drawing
//
// GetCurrentViewport
// Note--These refer to MVIEW (floating) viewports
//
//
//
DB_API
db_viewport *
db_drawing::GetCurrentViewport( void )
	{
	db_viewport *pRetval = NULL;

	int cvport;
	struct resbuf rb;

	if(db_getvar(NULL,DB_QCVPORT,&rb,this,NULL,NULL) != RTNORM)
		{
		return NULL;
		}
	cvport = rb.resval.rint;

	pRetval = this->GetViewport( cvport );

	// If nothing matching cvport found, go with the first one.  If that isn't there,
	// no current viewport (this happens since we don't create the first viewport until
	// tilemode has been turned off the first time)
	//
	if ( pRetval == NULL )
		{
		db_handitem *hip = this->tblnext(DB_VXTAB,1);
		if ( (hip != NULL) &&
			(!hip->ret_deleted()))
			{
			pRetval = ((db_vxtabrec *)hip)->ret_vpehip();
			ASSERT( pRetval->ret_type() == DB_VIEWPORT );
			}
		}

	return pRetval;
	}

// ***************************************
// PUBLIC METHOD -- class db_drawing
//
// GetViewport
// Note--These refer to MVIEW (floating) viewports
//
//
//
DB_API
db_viewport *
db_drawing::GetViewport( int iNumberToFind )
	{
	db_viewport *pRetval = NULL;

	db_handitem *hip = this->tblnext(DB_VXTAB,1);
	while(hip!=NULL)
		{
		int iVportNumber;
		if(hip->ret_deleted())
			{
			continue;
			}
		db_viewport *pViewport = ((db_vxtabrec *)hip)->ret_vpehip();
		ASSERT( pViewport->ret_type() == DB_VIEWPORT );

		pViewport->get_69(&iVportNumber);
		if ( iVportNumber == iNumberToFind )
			{
			pRetval = pViewport;
			break;
			}

		hip=this->tblnext(DB_VXTAB,0);
		}

	return pRetval;
	}

void
db_drawing:: AddHandleToFixupTable(db_objhandle handle, void **theptr)
	{
	if (handle!=NULL)
		fixupTable->Add(handle,theptr);
	}

void
db_drawing:: AddHandleToTable(db_handitem *hip)
	{
	handleTable->Add(hip);
	}

bool
db_drawing:: Make1005FixupTables(void)
	{
	m_fixup1005HandleMap= new FixupHandleTable();
	m_fixup1005HipTable= new Fixup1005HipTable();
	if (m_fixup1005HandleMap && m_fixup1005HipTable)
		return true;
	delete m_fixup1005HandleMap;
	m_fixup1005HandleMap = NULL;
	delete m_fixup1005HipTable;
	m_fixup1005HipTable = NULL;
	return false;
	}

bool
db_drawing:: FixingUp1005s(void)
	{
	if (m_fixup1005HandleMap)
		return true;
	return false;
	}

void
db_drawing:: Apply1005Fixups(void)
	{
	if (m_fixup1005HipTable)
		m_fixup1005HipTable->ApplyFixups(this);
	}

void
db_drawing:: Delete1005FixupTables(void)
	{
	delete m_fixup1005HandleMap;
	m_fixup1005HandleMap = NULL;
	delete m_fixup1005HipTable;
	m_fixup1005HipTable = NULL;
	}

bool
db_drawing::GetNew1005Handle(
	const db_objhandle oldHandle,		// in
	db_objhandle* newHandle)			// out
	{
	return m_fixup1005HandleMap->GetNewHandle(oldHandle, newHandle);
	}

int db_drawing::check()
{
	// iterate tables
	for(int i = 0; i < DB_NTABS; ++i)
	{
		if(!table[i].check(this)) // currently it can't fail so just assert
			assert(false);
	}

	// iterate objects
	db_handitem* pElement;
	for(pElement = objll[0]; pElement; pElement = pElement->next)
	{
		if(!pElement->check(this)) // currently it can't fail so just assert
			assert(false);
	}

	if (!addDefLayoutDictAndRecordIfNotPresent())
	{
		assert(false);
	}

	// iterate entities
	for(pElement = entll[0]; pElement; pElement = pElement->next)
	{
		if(!pElement->check(this)) // currently it can't fail so just assert
			assert(false);
	}

	return 1;
}


/********************************************************************************
 * Author:	Sachin Dange.
 *
 * Purpose:	Ensure that the ACAD_LAYOUT dictionary and the underlying layout objects 
 *			are present. This check is particularly necessary in case when Layout Objects 
 *			are not added if the File Version is 13 OR 14 and File Type is DXF 
 *			(ASCII and BINARY), that has been saved in AutoCAD 2000 only.
 *
 * Returns:	int
 ********************************************************************************/
int db_drawing::addDefLayoutDictAndRecordIfNotPresent()
{
	// get the Named Object Dictionary (NOD).
	db_dictionary* pNamedObjDict = static_cast<db_dictionary*>(this->namedobjdict());
	if (pNamedObjDict == NULL)
		return 0;	// serious problem

	// check if ACAD_LAYOUT dictionary is present in the NOD.
	db_dictionary* pLayoutsDict = static_cast<db_dictionary*>(this->dictsearch(pNamedObjDict, "ACAD_LAYOUT"/*DNT*/, 0));
	db_dictionaryrec* pLayoutRec = NULL;
	int result = 0;

	// If the ACAD_LAYOUT dictionary is not present, then create one and add it to the 
	// main dictionary.
	if (pLayoutsDict == NULL)
	{
		pLayoutsDict = new db_dictionary();
		addhanditem(pLayoutsDict);

		db_objhandle hLayoutDictHandle = pLayoutsDict->RetHandle();
		stockhand[DB_SHI_LAYOUTDIC] = hLayoutDictHandle; 

		// Add the LAYOUT record.
		pLayoutRec = new db_dictionaryrec("ACAD_LAYOUT"/*DNT*/, DB_SOFT_OWNER, hLayoutDictHandle, pLayoutsDict);
		pNamedObjDict->addrec(pLayoutRec);
		pLayoutsDict->addReactor(pNamedObjDict->RetHandle(), pNamedObjDict, ReactorItem::SOFT_POINTER);

		// Add the "Model Space" Layout Object.
		result = addDefaultLayoutObject(pLayoutsDict, MODELSPACE_LAYOUT);
		
		// Add the "Default Paper Space" Layout Object.
		result = addDefaultLayoutObject(pLayoutsDict, PAPERSPACE_LAYOUT1);

		// Add the "Default Paper Space" Layout Object.
		result = addDefaultLayoutObject(pLayoutsDict, PAPERSPACE_LAYOUT2);
	}
	else
	{
		// At least 2 default layout objects ("Model" and "Layout1") should be present 
		// in the dictionary. If any of them are not present, add them to the 
		// ACAD_LAYOUT Dictionary.
		
		bool bModelLayoutPresent = false; 
		bool bPaperLayoutPresent = false; 

		pLayoutsDict->get_recllends(&pLayoutRec, NULL);
		while (pLayoutRec != NULL)
		{
			if (stricmp(pLayoutRec->name, MODELSPACE_LAYOUT) == 0)
				bModelLayoutPresent = true;
			else
				bPaperLayoutPresent = true;

			if (bModelLayoutPresent && bPaperLayoutPresent)
				return 1;

			pLayoutRec = pLayoutRec->next;
		}

		// Add the "Model" Space Layout Object.
		if (!bModelLayoutPresent)
			result = addDefaultLayoutObject(pLayoutsDict, MODELSPACE_LAYOUT);

		// Add the "Paper" Space Layout Object.
		if (!bPaperLayoutPresent)
		{
			result = addDefaultLayoutObject(pLayoutsDict, PAPERSPACE_LAYOUT1);
			result = addDefaultLayoutObject(pLayoutsDict, PAPERSPACE_LAYOUT2);
		}
	}

	return result;
}


/********************************************************************************
 * Author:	Sachin Dange.
 *
 * Purpose:	To add a default Layout Object (for Model Space or Paper Space), 
 *			depending on pLayoutName.
 *
 * Returns:	int
 ********************************************************************************/
int db_drawing::addDefaultLayoutObject(db_handitem* pLayoutsDictHandItem, const char* pLayoutName)
{
	db_dictionary* pLayoutsDict = static_cast<db_dictionary*>(pLayoutsDictHandItem);
	db_dictionaryrec* pDictRec = NULL;

	CDbLayout* phLayout = new CDbLayout();
	addhanditem(phLayout);
	
	db_objhandle hLayoutObjectHandle = phLayout->RetHandle();
	pDictRec = new db_dictionaryrec(pLayoutName, DB_SOFT_OWNER, hLayoutObjectHandle, phLayout);
	pLayoutsDict->addrec(pDictRec);

	phLayout->addReactor(pLayoutsDictHandItem->RetHandle(), pLayoutsDict, ReactorItem::SOFT_POINTER);

	// Modified By Sachin....Bugzilla::78897
	// Now Lineweights will be shown in Plot and Plot Previews for all Layouts.
	int nCurrentPlotLayoutFlag;
	phLayout->get_70(&nCurrentPlotLayoutFlag);
	phLayout->set_70(nCurrentPlotLayoutFlag | CDbPlotSettings::ePrintLineweights);
	// End Modification....Bugzilla::78897

	
	if (stricmp(pLayoutName, MODELSPACE_LAYOUT) == 0)
	{
		// Modified By Sachin....Bugzilla::78897
		//!!! phLayout->set_70(CDbPlotSettings::eModelType | CDbPlotSettings::ePrintLineweights);
		// End Modification.....Bugzilla::78897
		
		phLayout->setBlock(phLayout);
		stockhand[DB_SHI_MSLAYOUT] = hLayoutObjectHandle;
		return 1;
	}

	if (stricmp(pLayoutName, PAPERSPACE_LAYOUT1) == 0)
	{
		phLayout->setCurrent();
		stockhand[DB_SHI_PSLAYOUT] = hLayoutObjectHandle;
		return 1;
	}

	if (stricmp(pLayoutName, PAPERSPACE_LAYOUT2) == 0)
	{
		db_block* pBlock = new db_block(this);
		pBlock->set_2("*Paper_Space0");
		addhanditem(pBlock);
		addhanditem(new db_endblk(this));

		((db_blocktabrec*)pBlock->ret_bthip())->set_layoutobjhandle(hLayoutObjectHandle);

		phLayout->setBlock(pBlock->ret_bthip());
		return 1;
	}

	return 0;
}
//----------------------------------------------------------------




//Added Cybage AJK 29/10/2001 [
//Reason --Fix for bug no 77865 from Bugzilla
/*	This function has been added to validate entities passed to it.
*	This is an evolving function and more and more cases need to be added to it as and
*	when they are found.
*	The function is being called from sds_entmake and sds_entmod.
*	It is an important function and care should be taken while
1) returning false everytime.
2) checking optional DXF codes (e.g. DXF 62 is optional in layer)
*/
bool validateEnt(struct resbuf *elist,int hitype)
{
	//if hittype is not known then it defaults to 0 otherwise it has value greater than 0
	if(!hitype)//get hittype if it is not known
	{
		char *tabrecname = NULL;
		int itsatabrec	 = -1;
		struct resbuf *typelink = NULL;
		struct resbuf *recnamelink= NULL;
		struct resbuf *trbp1= NULL;
		typelink=recnamelink=NULL; hitype=-1; tabrecname=NULL; itsatabrec=0;
		for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext)
		{
			if (trbp1->restype==0)
			{
				typelink=trbp1;
				if (trbp1->resval.rstring!=NULL)
				{
					if (strisame("BLOCKTABREC",trbp1->resval.rstring)) {
						hitype=DB_BLOCKTABREC;
					} else if (strisame("BLOCK",trbp1->resval.rstring)) {
						hitype=DB_BLOCK;
					} else if ((hitype=db_str2hitype(trbp1->resval.rstring))<0) {
						continue;
					}
					if (!(itsatabrec=db_is_tabrectype(hitype)) ||
						recnamelink!=NULL) break;
				}
			} else if (trbp1->restype==2)
			{
				recnamelink=trbp1;
				if ((tabrecname=trbp1->resval.rstring)==NULL) continue;
				if (typelink!=NULL) break;
			}
		}
	}
	switch(hitype)
	{
	case DB_LAYERTABREC:
		if((ic_assoc(elist,62)==0) && (ic_rbassoc->resval.rint==DB_BBCOLOR|| ic_rbassoc->resval.rint>=DB_BLCOLOR || ic_rbassoc->resval.rint<=-DB_BLCOLOR))//layer can have -ve value when it is turned off
		{
			return false;
		}
		// Commented the following code as one could not entmod an Xref Layer in some cases. eg .
		// in RENAME command, where one can rename an xref block and hence its dependant symbols
		// like layers, dimstyle, style, blocks.
		/*
		//Added Cybage AJK 17/12/2001 [
		//Reason - Fix for bug no 77948 from Bugzilla
		if((ic_assoc(elist,2)==0))
		{

			if(!ic_isvalidname(ic_rbassoc->resval.rstring))
				return false;
		}
		//Added Cybage AJK 17/12/2001 DD/MM/YYYY ]
		*/
		break;
	case DB_SPLINE :
		if((ic_assoc(elist,71)==0) && (ic_rbassoc->resval.rint == 0 ))
		{
			return false;
		}
		break;
	//Bugzilla No 78476 28-04-2003 [
	case DB_LEADER :
		struct resbuf *trbp= NULL;
		bool zCapture = true;
		double zCurrPt, zFirstPt;
		zCurrPt = zFirstPt = 0.0;
		for (trbp=elist; trbp!=NULL; trbp=trbp->rbnext)
		{			
			if(ic_assoc(trbp,10)==0)
			{
				zCurrPt = ic_rbassoc->resval.rpoint[2];
				
				if(zCapture)
				{
					zFirstPt = zCurrPt;
					zCapture = false;
				}

				if(fabs(zFirstPt - zCurrPt) > IC_ZRO)
					ic_rbassoc->resval.rpoint[2] = zFirstPt;
			}
			else
				break;
		}
		return true;
		break;
	//Bugzilla No 78476 28-04-2003 ]
		//TODO
		//keep adding cases for various entities here
	}
	return true;
}
//Added Cybage AJK 29/10/2001 DD/MM/YYYY ]

// EBATECH(CNBR) -[ Bugzilla#78260
// Some system variables are moved from/to DICTIONARYVAR/HEADER section.
bool db_drawing::fixupDicVars(bool bSave, int version)
{
	db_handitem *dicvarp, *recvarp;
	db_dictionaryvar *sysvarp;
	struct resbuf rb;
	char	strbuf[256];

	if( !objll[0] )
		return false;

	dicvarp = dictsearch(objll[0], "ACDBVARIABLEDICTIONARY"/*DNT*/, 0);

	// DIMADEC : R14 in DICTIONARY / 2000 or later in HEADER
	if( bSave )	// PreSavingProcess
	{
		if( db_getvar(NULL, DB_QDIMADEC, &rb, this, NULL, NULL) == RTNORM)
		{
			// Add DICTIONARYVAR to the database.
			if( version == IC_ACAD14 && rb.resval.rint != atoi(db_oldsysvar[DB_QDIMADEC].defval))
			{
				if( !dicvarp )
				{
					dicvarp = new db_dictionary();
					ASSERT( dicvarp );
					addhanditem(dicvarp);
					dictadd(objll[0], "ACDBVARIABLEDICTIONARY", dicvarp );
				}
				if(( recvarp = dictsearch(dicvarp, "DIMADEC"/*DNT*/, 0)) == NULL )
				{
					recvarp = new db_dictionaryvar();
					ASSERT( recvarp );
					addhanditem(recvarp);
					dictadd(dicvarp, "DIMADEC", recvarp);
				}
				sysvarp =(db_dictionaryvar *)recvarp;
				itoa( rb.resval.rint, strbuf, 10 );
				sysvarp->set_1(strbuf);
			}
			// Remove DICTIONARYVAR from the database.
			else if( dicvarp && (recvarp = dictsearch(dicvarp, "DIMADEC"/*DNT*/, 0)) != NULL )
			{
				sysvarp =(db_dictionaryvar *)recvarp;
				dictdel(dicvarp, "DIMADEC"/*DNT*/);
				delhanditem(NULL, recvarp, 0);
			}
		}
	} else {	// PostLoading Process
		// set variable and remove from the database.
		if( dicvarp && (recvarp = dictsearch(dicvarp, "DIMADEC"/*DNT*/, 0)) != NULL )
		{
			sysvarp =(db_dictionaryvar *)recvarp;
			dictdel(dicvarp, "DIMADEC"/*DNT*/);
			if( version == IC_ACAD14 && !sysvarp->ret_deleted() && sysvarp->ret_1() != NULL )
			{
				rb.restype = RTSHORT;
				rb.resval.rint = atoi(sysvarp->ret_1());
				db_setvar(NULL, DB_QDIMADEC, &rb, this, NULL, NULL, 0 );
			}
			delhanditem(NULL, recvarp, 0);
		}
	}
	// TODO : You can add here some variables...
	return true;
}
// EBATECH(CNBR) ]-
