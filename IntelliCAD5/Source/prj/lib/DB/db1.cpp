/* C:\ICAD\PRJ\LIB\DB\DB1.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

// *********************************************************************
// *  DB1.CPP														   *
// *  Copyright (C) 1997, 1998 by Visio Corporation 				   *
// *********************************************************************

	// Moved these initializations from vars.h to here
	// DB_DEFVARS is used elsewhere, so is still defined
#define DB_DEFVARS 1

#include "db.h"
#include "BHatchFactory.h"

	char *db_str_0			="0"/*DNT*/;
	char *db_str_acad		="ACAD"/*DNT*/;
	char *db_str_byblock	="BYBLOCK"/*DNT*/;
	char *db_str_bylayer	="BYLAYER"/*DNT*/;
	char *db_str_continuous ="CONTINUOUS"/*DNT*/;
	char *db_str_invalid	="INVALID"/*DNT*/;
	char *db_str_msblockname="*MODEL_SPACE"/*DNT*/; // 2002/10/30 Ebatech(CNBR) Rename "*MSPACE_BLOCK"
	char *db_str_psblockname="*PAPER_SPACE"/*DNT*/; // 2002/10/30 Ebatech(CNBR) Rename "*PSPACE_BLOCK"
	char *db_str_quotequote =""/*DNT*/;
	char *db_str_standard	="STANDARD"/*DNT*/;
	char *db_str_staractive ="*ACTIVE"/*DNT*/;
	char *db_str_txt		="TXT"/*DNT*/;
	char *db_str_bycolor	="BYCOLOR"/*DNT*/;
	char *db_str_normal		="NORMAL"/*DNT*/;

#ifdef DWGCNVT
	char *db_icfontnm="DWG",*db_icfontfn="DWG.FNT";
#else
	char *db_icfontnm="ICAD",*db_icfontfn="ICAD.FNT";
#endif

	char *db_tolfontnm="GDT";
	char *db_ourtolfontnm="IC-GDT";




// *****************************************************************
// IcadSharedGlobals class
//
// These are variables that are shared between DLLs and the ICAD
// executable.	The IcadSharedGlobals class entirely consists of
// static methods and members, so it acts as a singleton.
//
HINSTANCE IcadSharedGlobals::zm_hAppInstance = NULL;

HWND IcadSharedGlobals::zm_hMainWindow = NULL;

HINSTANCE IcadSharedGlobals::zm_hResourceInstance = NULL;

BHatchBuilderFactory *IcadSharedGlobals::m_hatchFactory = NULL;
// EBATECH(watanabe)-[update leader according to modifying annotation
CUpdateLeaderFactory *IcadSharedGlobals::m_leaderFactory = NULL;
// ]-EBATECH(watanabe)
int (*IcadSharedGlobals::m_pMessageFunct) (const char*,...) = NULL;
int (*IcadSharedGlobals::m_pGetVarFunct) (short, struct resbuf*) = NULL;
int (*IcadSharedGlobals::m_pPreviewBMFunct) (const char*, void**, unsigned long*, short*) = NULL;

void IcadSharedGlobals::SetIcadAppInstance( HINSTANCE hInstance )
	{
	zm_hAppInstance = hInstance;
	}

HINSTANCE IcadSharedGlobals::GetIcadAppInstance( void )
	{

	ASSERT( zm_hAppInstance != NULL );
	return zm_hAppInstance;
	}


/********************************************************************************
 * Author:	Sachin Dange.
 *
 * Purpose:	Function to Set the Main Window Handle.
 *
 *
 * Returns:	void
 ********************************************************************************/
void IcadSharedGlobals::SetMainHWND(HWND hWnd)
{
	zm_hMainWindow = hWnd;
}


/********************************************************************************
 * Author:	Sachin Dange.
 *
 * Purpose:	Function to Get the Main Window Handle.
 *
 *
 * Returns:	HWND
 ********************************************************************************/
HWND IcadSharedGlobals::GetMainHWND()
{
	ASSERT(zm_hMainWindow != NULL);
	return zm_hMainWindow;
}


void IcadSharedGlobals::SetIcadResourceInstance( HINSTANCE hInstance )
	{
	zm_hResourceInstance = hInstance;
	}

HINSTANCE IcadSharedGlobals::GetIcadResourceInstance( void )
	{

	// This ASSERT gets triggered during startup, so it is off for now
	//
//	ASSERT( zm_hResourceInstance != NULL );
	return zm_hResourceInstance;
	}

void IcadSharedGlobals::SetHatchFactory (BHatchBuilderFactory *hatchFact)
{
	m_hatchFactory = hatchFact;
}

BHatchBuilderFactory* IcadSharedGlobals::GetHatchFactory()
{
	return m_hatchFactory;
}

// EBATECH(watanabe)-[update leader according to modifying annotation
void IcadSharedGlobals::SetLeaderFactory(CUpdateLeaderFactory* p)
{
	m_leaderFactory = p;
}

CUpdateLeaderFactory* IcadSharedGlobals::GetLeaderFactory()
{
	return m_leaderFactory;
}
// ]-EBATECH(watanabe)

void IcadSharedGlobals::SetCmdMessage(int (*pfunct)(const char *,...))
{
	m_pMessageFunct = pfunct;
}

void IcadSharedGlobals::CallCmdMessage(const char *message)
{
	if (m_pMessageFunct)
		(*m_pMessageFunct)(message);
}

void IcadSharedGlobals::SetGetVar(int (*pfunct)(short, struct resbuf*))
{
	m_pGetVarFunct = pfunct;
}

void IcadSharedGlobals::SetPreviewBMFunc(int (*pfunct)(const char*, void**, unsigned long*, short*))
{
	m_pPreviewBMFunct = pfunct;
}

int IcadSharedGlobals::CallGetVar(short qidx, struct resbuf* rb)
{
	if (m_pGetVarFunct)
		return (*m_pGetVarFunct)(qidx, rb);
	return RTERROR;
}

int IcadSharedGlobals::CallGetPreviewBM(const char* filename, void** phbitmap, unsigned long* pAllocatedBytes, short* bIsBitmap)
{
	if (m_pPreviewBMFunct)
		return (*m_pPreviewBMFunct)(filename, phbitmap, pAllocatedBytes, bIsBitmap);
	return RTERROR;
}


static ProgressMeter	*gpProgressMeter=NULL;
DB_API SetAdviseProgressMeter( ProgressMeter *pMeter )
	{
	gpProgressMeter = pMeter;
	}


void	(*db_grafreedofp)(void *)=NULL;
int		db_lasterror=0;
int		db_sysvarbufsz[3];

int
db_progressstart()
	{
	if ( gpProgressMeter )
		{
		gpProgressMeter->Start();
		return RTNORM;
		}

	return RTERROR;
	}

int
db_progressstop()
	{
	if ( gpProgressMeter )
		{
		gpProgressMeter->Stop();
		return RTNORM;
		}

	return RTERROR;
	}

int
db_progresspercent(int percentDone)
	{
	if ( gpProgressMeter )
		{
		gpProgressMeter->Percent( percentDone);
		return RTNORM;
		}

	return RTERROR;
	}


	struct db_handitemtypeinfo db_hitypeinfo[]= {	/* Indexed by  DB_ codes. */
		{ "APPID"			 , DB_TABLE    , DB_APPIDTAB	},	/*	0 */
		{ "BLOCK"			 , DB_TABLE    , DB_BLOCKTAB	},	/*	1 */
		{ "DIMSTYLE"		 , DB_TABLE    , DB_DIMSTYLETAB },	/*	2 */
		{ "LAYER"			 , DB_TABLE    , DB_LAYERTAB	},	/*	3 */
		{ "LTYPE"			 , DB_TABLE    , DB_LTYPETAB	},	/*	4 */
		{ "STYLE"			 , DB_TABLE    , DB_STYLETAB	},	/*	5 */
		{ "UCS" 			 , DB_TABLE    , DB_UCSTAB		},	/*	6 */
		{ "VIEW"			 , DB_TABLE    , DB_VIEWTAB 	},	/*	7 */
		{ "VPORT"			 , DB_TABLE    , DB_VPORTTAB	},	/*	8 */
		{ "VX"				 , DB_TABLE    , DB_VXTAB		},	/*	9 */
		{ "3DFACE"			 , DB_ENTITY   , 0				},	/* 10 */
		{ "3DSOLID" 		 , DB_ENTITY   , 0				},	/* 11 */
		{ "ARC" 			 , DB_ENTITY   , 0				},	/* 12 */
		{ "ATTDEF"			 , DB_ENTITY   , 0				},	/* 13 */
		{ "ATTRIB"			 , DB_ENTITY   , 0				},	/* 14 */
		{ "BLOCK"			 , DB_ENTITY   , 0				},	/* 15 */
		{ "BODY"			 , DB_ENTITY   , 0				},	/* 16 */
		{ "CIRCLE"			 , DB_ENTITY   , 0				},	/* 17 */
		{ "DIMENSION"		 , DB_ENTITY   , 0				},	/* 18 */
		{ "ELLIPSE" 		 , DB_ENTITY   , 0				},	/* 19 */
		{ "ENDBLK"			 , DB_ENTITY   , 0				},	/* 20 */
		{ "INSERT"			 , DB_ENTITY   , 0				},	/* 21 */
		{ "LEADER"			 , DB_ENTITY   , 0				},	/* 22 */
		{ "LINE"			 , DB_ENTITY   , 0				},	/* 23 */
		{ "MLINE"			 , DB_ENTITY   , 0				},	/* 24 */
		{ "MTEXT"			 , DB_ENTITY   , 0				},	/* 25 */
		{ "OLE2FRAME"		 , DB_ENTITY   , 0				},	/* 26 */
		{ "POINT"			 , DB_ENTITY   , 0				},	/* 27 */
		{ "POLYLINE"		 , DB_ENTITY   , 0				},	/* 28 */
		{ "RAY" 			 , DB_ENTITY   , 0				},	/* 29 */
		{ "REGION"			 , DB_ENTITY   , 0				},	/* 30 */
		{ "SEQEND"			 , DB_ENTITY   , 0				},	/* 31 */
		{ "SHAPE"			 , DB_ENTITY   , 0				},	/* 32 */
		{ "SOLID"			 , DB_ENTITY   , 0				},	/* 33 */
		{ "SPLINE"			 , DB_ENTITY   , 0				},	/* 34 */
		{ "TEXT"			 , DB_ENTITY   , 0				},	/* 35 */
		{ "TOLERANCE"		 , DB_ENTITY   , 0				},	/* 36 */
		{ "TRACE"			 , DB_ENTITY   , 0				},	/* 37 */
		{ "VERTEX"			 , DB_ENTITY   , 0				},	/* 38 */
		{ "VIEWPORT"		 , DB_ENTITY   , 0				},	/* 39 */
		{ "XLINE"			 , DB_ENTITY   , 0				},	/* 40 */
		{ "HATCH"			 , DB_ENTITY   , 0				},	/* 41 */
		{ "IMAGE"			 , DB_ENTITY   , 0				},	/* 42 */
		{ "LWPOLYLINE"		 , DB_ENTITY   , 0				},	/* 43 */
		{ "DICTIONARY"		 , DB_OBJECT   , 0				},	/* 44 */
		{ "DICTIONARYVAR"	 , DB_OBJECT   , 0				},	/* 45 */
		{ "GROUP"			 , DB_OBJECT   , 0				},	/* 46 */
		{ "IDBUFFER"		 , DB_OBJECT   , 0				},	/* 47 */
		{ "IMAGEDEF"		 , DB_OBJECT   , 0				},	/* 48 */
		{ "IMAGEDEF_REACTOR" , DB_OBJECT   , 0				},	/* 49 */
		{ "MLINESTYLE"		 , DB_OBJECT   , 0				},	/* 50 */
		{ "ACAD_PROXY_OBJECT", DB_OBJECT   , 0				},	/* 51 */
		{ "RASTERVARIABLES"  , DB_OBJECT   , 0				},	/* 52 */
		{ "SPATIAL_FILTER"	 , DB_OBJECT   , 0				},	/* 53 */
		{ "XRECORD" 		 , DB_OBJECT   , 0				},	/* 54 */
		{ "ACAD_PROXY_ENTITY", DB_ENTITY   , 0				},	/* 55 */
		{ "LAYER_INDEX" 	 , DB_OBJECT   , 0				},	/* 56 */
		{ "OBJECT_PTR"		 , DB_OBJECT   , 0				},	/* 57 */
		{ "SORTENTSTABLE"	 , DB_OBJECT   , 0				},	/* 58 */
		{ "SPATIAL_INDEX"	 , DB_OBJECT   , 0				},	/* 59 */
		{ "PLOTSETTINGS"	 , DB_OBJECT   , 0				},	/* 60 */
		{ "LAYOUT"			 , DB_OBJECT   , 0				},	/* 61 */
		{ "ACDBPLACEHOLDER"  , DB_OBJECT   , 0				},	/* 62 */  // Bugzilla 78230 / Old name : PLACEHOLDER
		{ "ACDBDICTIONARYWDFLT", DB_OBJECT , 0				},	/* 63 */  // Bugzilla 78230 / Old name : DICTIONARYWDFLT
		{ "WIPEOUTVARIABLES" , DB_OBJECT   , 0				},	/* 64 */
		{ "ARCALIGNEDTEXT"	 , DB_ENTITY   , 0				},	/* 65 */
		{ "RTEXT"			 , DB_ENTITY   , 0				},	/* 66 */
		{ "WIPEOUT" 		 , DB_ENTITY   , 0				},	/* 67 */
		{ "VBA_PROJECT" 	 , DB_OBJECT   , 0				},	/* 68 */
		{ "DIMASSOC"		 , DB_OBJECT   , 0				}	/* 69 */ // EBATECH(CNBR) 2002/7/10 Bugzilla#78218
	};
	// EBATECH(CNBR) 2002/6/29 Bugzilla 78230 Correct names of the elements
	// EBATECH(CNBR) 2002/7/10 Bugzilla 78218 Add new DIMASSOC object
	/* The the names of the elements marked "TODO" above haven't been */
	/* verified.  We haven't seen them in an ACAD entget yet. */


	/*
	**	This array converts a DB_...TAB type to a DB_...TABREC type.
	**	For example, db_tab2tabrec[DB_LTYPETAB] is DB_LTYPETABREC.
	**	Although the TAB and TABREC defines are CURRENTLY the same,
	**	that may change in the future.	See the note above
	**	their #defines.
	*/
	int db_tab2tabrec[]={
		DB_APPIDTABREC,
		DB_BLOCKTABREC,
		DB_DIMSTYLETABREC,
		DB_LAYERTABREC,
		DB_LTYPETABREC,
		DB_STYLETABREC,
		DB_UCSTABREC,
		DB_VIEWTABREC,
		DB_VPORTTABREC,
		DB_VXTABREC
	};


	/*
	**	All system variables that releases R10-R13 recognize
	**	except those mentioned below (plus a few undocumented ones).
	**
	**	WE'RE GOING TO PLAY WITH FIRE AGAIN WITH THE SORT:
	**	THIS ARRAY MUST BE MAINTAINED IN strcmp's SORT ORDER --
	**	WHICH SHOULD MATCH QEDIT'S.  THIS WILL ALLOW US TO SKIP THE qsort()
	**	AND USE OUR "QUICK FIND" METHOD (WHERE WE ASSIGN A
	**	DB_Q... #define TO EACH ARRAY ELEMENT.	WE HAD A PROBLEM
	**	WHEN OUR COMPARISON FUNCTION USED stricmp.	APPARENTLY, stricmp
	**	CONVERTS TO LOWER CASE BEFORE COMPARISON, MAKING UNDERBAR
	**	PRECEDE ALL ALPHABETIC CHARACTERS.	USING strcmp SHOULD
	**	AVOID THIS PROBLEM AND SORT AS QEDIT DOES.
	**
	**	WE MUST KEEP THE DB_Q... #defines CONSISTENT WITH THIS
	**	ARRAY, SORTED AS JUST DESCRIBED.  JUST SORT THIS ARRAY, THEN
	**	STEAL THE NAMES, ADD DB_Q TO EACH AND ASSIGN THE NUMBERS
	**	SEQUENTIALLY (USING THE EDITOR'S COLUMN FEATURES).
	**	(WE CAN'T PUT THEM HERE, INSIDE THIS "#if defined(DB_DEFVARS)"
	**	OR WE'D HAVE TO DUPLICATE THEM IN THE #else.)
	**
	**	LASTPT3D is omitted because it's the same as LASTPOINT
	**	(R10) -- and it's a session variable.  Having LASTPOINT and
	**	LASTPT3D maintains 2 separate records in the session buffer.
	**	We get around this in db_findsysvar() by changing var name
	**	"LASTPT3D" to "LASTPOINT".
	**
	**	The drawing vars are mapped according to their R10/R12 header
	**	slots; new R13 drawing vars are given slots beyond the end
	**	of the last R12 var (VISRETAIN).
	**
	**	NOTE:  °
	**
	**		We have a problem with some of the drawing system variables
	**		that we'll have to fix someday.  Some vars like ELEVATION,
	**		the limits, and the UCS settings are for the "current space"
	**		-- mspace and pspace have their own copies, and which one
	**		you have access to with getvar/setvar depends on which space
	**		is active.	Similar to these are the "current viewport"
	**		system vars (like snap and grid settings and the dview
	**		parameters) -- a separate set in each viewport, with
	**		getvar/setvar operating on the one that is currently active.
	**		The "space" ones actually have their own slots in an R12
	**		header, but only the MSPACE ones are mapped below.	The
	**		"viewport" ones are in the viewport table entries; the R12
	**		header has slots for the current values (not so in R13).
	**
	**		Someday, we'll have to mark them in spl as having
	**		usual form and deal with them in getvar and setvar.
	**		Currently, the R12 MSPACE slots are used, and each
	**		variable has only one setting.	The R13 viewport ones
	**		will use the defaults, since they aren't in the header
	**		any more.  All of this can be fixed someday (with some
	**		work) by using the "space" ones from the correct
	**		slots and using the viewport ones from the correct
	**		table entry.
	**
	**		For now, since they have slots in an R12 header, I've added
	**		the PSPACE variables by prefixing their names with "P_"
	**		(like P_ELEVATION).
	**
	**		These variables MUST be in alphabetical descending order
	**		or the binary search will FAIL - but usually it will just
	**		fail on the variable that is out of order.
	**
	**	KEY TO THE NOTE COMMENTS:
	**		A : New in R13. (bit 12 also set)
	**		I : Our own custom setvar.
	**		U : Undocumented
	**		2 : New in 2000 or Later -- Modified by EBATECH(CNBR) 2001/12/28
	**			Next Offset is 10400	- ODT 2.007/2.008
	**			Next Offset is 10402	- ODT 2.009
	**
	**  Type bit codes as specified in Icaddef.h
	**
	**	#define IC_SYSVARFLAG_UNUSUAL					0x0001	//   Bit 0 : Unusual form in buffer
	**	#define IC_SYSVARFLAG_SPECIALHANDLING			0x0002	//   Bit 1 : Special handling writing; not independent
	**	#define IC_SYSVARFLAG_UNDOCUMENTED				0x0004	//   Bit 2 : Undocumented 
	**	#define IC_SYSVARFLAG_SPECIALSOURCE				0x0008	//   Bit 3 : Special source (not stored in a buffer like DATE. Usually, truly read-only. Should be only certain session vars.)
	**	#define IC_SYSVARFLAG_SPECIALRANGE				0x0010	//   Bit 4 : Special range of allowed vals
	**	#define IC_SYSVARFLAG_DONTDXFOUT  				0x0020	//   Bit 5 : Exclude from DXFOUT 
	**	#define IC_SYSVARFLAG_REQUIRESSCREENCHANGE		0x0040	//   Bit 6 : Requires some sort of screen change
	**	#define IC_SYSVARFLAG_ACADSESSIONVARTOHDR 		0x0080	//   Bit 7 : An ACAD session var that we moved to the header (giving it a slot after the end of the true R12 header).  This makes it "per drawing" instead of "per session".
	**	#define IC_SYSVARFLAG_ANGLE  					0x0100	//   Bit 8 : Angle (it's "type" is RTREAL) 
	**	#define IC_SYSVARFLAG_TIMEDATE        			0x0200	//   Bit 9 : Time-date (it's "type" is RTREAL)
	**	#define IC_SYSVARFLAG_DONTSAVEINUNDOLIST 		0x0400	//   Bit 10: Don't save in undo list.
	**	#define IC_SYSVARFLAG_SPECIALVIEWVBL			0x0800	//   Bit 11: Special view variable (from VIEW table).
	**	#define IC_SYSVARFLAG_NEWR13VBL   				0x1000	//   Bit 12: New variable in R13 (exclude from R12 DXFout) 
	**	#define IC_SYSVARFLAG_FILEPATH					0x2000	//   Bit 13: A file pathname 
	**	#define IC_SYSVARFLAG_USERREADONLY				0x4000	//   Bit 14: User-read-only 
	**	#define IC_SYSVARFLAG_UNSUPPORTED				0x8000	//   Bit 15: A var that is currently unsupported.
	**	#define IC_SYSVARFLAG_PAPERSPACE			0x00010000	//   Bit 16: PaperSpace variant of another variable.
	**	#define IC_SYSVARFLAG_OBJDICTONARY			0x00020000	//   Bit 16: Variable is stored in an object dictionary.
	**/

	db_sysvarlink db_oldsysvar[]= {
//		{Varialbe Name	 , Type		,??,Bit Code, ??, ??, ??, ??}
/*2*/	{ "ACADLSPASDOC" , RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",		"0" },
		// EBATECH(CNBR) -[ 2002/6/15 ACADPREFIX is read-only
		{ "ACADPREFIX"	 , RTSTR	, 2, 0x4008,   -1,	 0, "", 		""	},
		//{ "ACADPREFIX"   , RTSTR	  , 2, 0x0008,	 -1,   0, "",		  ""  },
		{ "ACADVER" 	 , RTSTR	, 2, 0x4008,   -1,	 0, "", 		""	},
		{ "ACISOUTVER"	 , RTSHORT	, 0, 0x0000, 2871,	 2, "16_120",	"120" },
		{ "AFLAGS"		 , RTSHORT	, 0, 0x00A0, 1817,	 2, "0_15", 	"0" },
		{ "ANGBASE" 	 , RTREAL	, 0, 0x0100,  851,	 8, "", 		"0" },
		{ "ANGDIR"		 , RTSHORT	, 0, 0x0000,  859,	 2, "0_1",		"0" },
		// Modified Cybage PP --/--/2001
		{ "APBOX"		 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"0" },
		{ "APERTURE"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_50", 	"10" },
		// EBATECH(CNBR) 2002/12/2 Bugzilla#78395 AREA and PERIMETER are user read-only var.(0x00A0->0x40A0) AREA and PERIMTER accepts minus value.
		{ "AREA"		 , RTREAL	, 0, 0x40A0, 1819,	 8, "",		"0" },
		// EBATECH(CNBR) -[ 2002/6/15 since A2K this variable save/load from registry.
		{ "ATTDIA"		 , RTSHORT	, 1, 0x0000,  -1,	2, "0_1",	   "0" },
		//{ "ATTDIA"	   , RTSHORT  , 0, 0x0000,	987,   2, "0_1",	  "0" },
		{ "ATTMODE" 	 , RTSHORT	, 0, 0x0040,  402,	 2, "0_2",		"1" },
		// DP: In ACAD2000 this variable is saving in and loading from registry.
		// EBATEHC(CNBR) release offset and size
		{ "ATTREQ"		 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"1" },
		//{ "ATTREQ"	   , RTSHORT  , 1, 0x0000,	985,   2, "0_1",	  "1" },
		//{ "ATTREQ"	   , RTSHORT  , 0, 0x0000,	985,   2, "0_1",	  "1" },
		{ "AUDITCTL"	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",		"0" },
		{ "AUNITS"		 , RTSHORT	, 0, 0x0000,  394,	 2, "0_4",		"0" },
		{ "AUPREC"		 , RTSHORT	, 0, 0x0000,  396,	 2, "0_8",		"0" },
		{ "AUTOMENULOAD" , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"1" }, // ICAD only or Obsolute A2K
		// Modified Cybage PP --/--/2001
		{ "AUTOSNAP"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_63", 	"63" },
		{ "AXISMODE"	 , RTSHORT	, 0, 0x0000,  360,	 2, "0_1",		"0" }, // ICAD only or Obsolute A2K
		{ "AXISUNIT"	 , RTPOINT	, 0, 0x0000,  362,	16, "", 		"0,0,0" },	/*20*/// Undoc A2K(2DPoint)
/*U*/	{ "BACKGROUNDPLOT", RTSHORT	, 1, 0x1004,   -1,	 2, "0_3",		"2" },
		{ "BACKZ"		 , RTREAL	, 0, 0x4C20, 1139,	 8, "", 		"0" },
/*I*/	{ "BASEFILE"	 , RTSTR	, 1, 0x2020,   -1, 256, "", 		"icad.dwt" },
/*2*/	{ "BINDTYPE"	 , RTSHORT	, 2, 0x1000,   -1,	 2, "0_1",		"0" },
/*I*/	{ "BKGCOLOR"	 , RTSHORT	, 1, 0x0060,   -1,	 2, "0_256",	"256" },
/*U*/	{ "BKGREDRAW"	 , RTSHORT	, 1, 0x0024,   -1,	 2, "0_1",		"0" }, // Undoc and ICAD only
		// DP: In ACAD2000 this variable is saving in and loading from registry.
		// EBATEHC(CNBR) release offset and size
		{ "BLIPMODE"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"0" },
		//{ "BLIPMODE"	   , RTSHORT  , 1, 0x0000,	739,   2, "0_1",	  "0" },
		//{ "BLIPMODE"	   , RTSHORT  , 0, 0x0000,	739,   2, "0_1",	  "0" },
		{ "CDATE"		 , RTREAL	, 2, 0x4208,   -1,	 0, "", 		"" },
		{ "CECOLOR" 	 , RTSTR	, 0, 0x0051,  795,	 2, "", 		"BYLAYER" },
/*A*/	{ "CELTSCALE"	 , RTREAL	, 0, 0x1000, 1725,	 8, "1.0E-100_","1" },
		{ "CELTYPE" 	 , RTSTR	, 0, 0x0051,  797,	 2, "", 		"BYLAYER" },
		// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight
/*2*/	{ "CELWEIGHT"	 , RTSHORT	, 0, 0x1050, 4974,	 2, "-3_211",  "-1" },
/*2*/	{ "CEPSNTYPE"	 , RTSHORT	, 0, 0x5000,10384,	 2, "0_3",		"0" },	// Undoc A2K and ICAD
		// EBATECH(CNBR) -[ 2002/9/19 CHAMFERA,B,C Match initial value as ACAD.
		{ "CHAMFERA"	 , RTREAL	, 0, 0x0000,  989,	 8, "0_",		"0.5" }, // <- 0.0
		{ "CHAMFERB"	 , RTREAL	, 0, 0x0000,  997,	 8, "0_",		"0.5" }, // <- 0.0
/*A*/	{ "CHAMFERC"	 , RTREAL	, 0, 0x1000, 1733,	 8, "0_",		"1.0" }, // <- 0.0
		// EBATECH(CNBR) -[ 2002/6/15 Chamferd is angle. so no needs range value.
/*A*/	{ "CHAMFERD"	 , RTREAL	, 0, 0x1100, 1741,	 8, "","0" },
///*A*/   { "CHAMFERD"	   , RTREAL   , 0, 0x1100, 1741,   8, "0_3.14159265358979323846","0" },
/*A*/	{ "CHAMMODE"	 , RTSHORT	, 0, 0x10A0, 1827,	 2, "0_1",		"0" },
		{ "CIRCLERAD"	 , RTREAL	, 0, 0x00A0, 1829,	 8, "0_",		"0" },
		{ "CLAYER"		 , RTSTR	, 0, 0x0051,  330,	 2, "", 		"0" },
		{ "CMDACTIVE"	 , RTSHORT	, 0, 0x44A8, 1837,	 2, "", 		"0" },	// Read only 0-31
		{ "CMDDIA"		 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"1" },	// Undoc A2K
		{ "CMDECHO" 	 , RTSHORT	, 0, 0x00A0, 1839,	 2, "0_1",		"1" },
/*I*/	{ "CMDLNTEXT"	 , RTSTR	, 1, 0x2000,   -1,	51, "", 		":" },
		{ "CMDNAMES"	 , RTSTR	, 0, 0x44A0, 1841,	51, "", 		"" },
/*A*/	{ "CMLJUST" 	 , RTSHORT	, 0, 0x1000, 1749,	 2, "0_2",		"0" },
/*A*/	{ "CMLSCALE"	 , RTREAL	, 0, 0x1000, 1751,	 8, "", 		"1" },
/*A*/	{ "CMLSTYLE"	 , RTSTR	, 0, 0x1011, 1759,	 2, "", 		"STANDARD" },
/*I*/	{ "COLORX"		 , RTSHORT	, 1, 0x0060,   -1,	 2, "0_256",	"1" },
/*I*/	{ "COLORY"		 , RTSHORT	, 1, 0x0060,   -1,	 2, "0_256",	"3" },
/*I*/	{ "COLORZ"		 , RTSHORT	, 1, 0x0060,   -1,	 2, "0_256",	"5" },
/*2*/	{ "COMPASS" 	 , RTSHORT	, 2, 0x1000,   -1,	 2, "0_1",		"0" },
		// DP: In ACAD2000 this variable is saving in and loading from registry.
		// EBATECH(CNBR) Release offset and size
		{ "COORDS"		 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_2",		"1" },
		//{ "COORDS"	   , RTSHORT  , 1, 0x0000,	793,   2, "0_2",	  "1" },
		//{ "COORDS"	   , RTSHORT  , 0, 0x0000,	793,   2, "0_2",	  "1" },
		// EBATECH(CNBR) 2002/9/30 CPLOTSTYLE,CTAB,DEFLPLSTYLE and DEFPLSTYLE sets 0x10.
/*2*/	{ "CPLOTSTYLE"	 , RTSTR	, 0, 0x3011, 4976, IC_ACADBUF, "",	"BYCOLOR" },  // if drawing is bycolor, this is readonly
/*2*/	{ "CPROFILE"	 , RTSTR	, 1, 0x7000,   -1, IC_ACADBUF, "",	"Default" },
		// EBATECH(CNBR) 2003/8/10 CTAB 0x3010 -> 0x3050 (+ IC_SYSVARFLAG_REQUIRESCREENCHANGE)
/*2*/	{ "CTAB"		 , RTSTR	, 0, 0x3050, 7025, IC_ACADBUF, "",	"Model" },
// Add by SystemMetrix(Maeda) 2001.09.03 -[
/*I*/	{ "CTRLMOUSE"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"1" },
// ]- Add by SystemMetrix(Maeda) 2001.09.03
		{ "CURSORSIZE"	 , RTSHORT	, 1, 0x0040,   -1,	 2, "1_100",	"5" },
		{ "CVPORT"		 , RTSHORT	, 0, 0x00E0, 1892,	 2, "1_",		"2" },	/* Mine's always 2 */
/*U*/	{ "CYCLECURR"	 , RTSHORT	, 0, 0x4024, 2863,	 2, "", 		"5" },
/*U*/	{ "CYCLEKEEP"	 , RTSHORT	, 0, 0x4024, 2861,	 2, "", 		"1" },
		{ "DATE"		 , RTREAL	, 2, 0x4208,   -1,	 0, "", 		"" },
		{ "DBMOD"		 , RTSHORT	, 0, 0x40E0, 1894,	 2, "", 		"0" }, // Read only 0-31
/*A*/	{ "DCTCUST" 	 , RTSTR	, 1, 0x1010,   -1,	51, "", 		"" },
/*A*/	{ "DCTMAIN" 	 , RTSTR	, 1, 0x1010,   -1,	51, "", 		"" },
/*2*/	{ "DEFLPLSTYLE"  , RTSTR	, 1, 0x3011,   -1, IC_ACADBUF, "",	"NORMAL" },  // if drawing is bycolor, this is readonly
/*2*/	{ "DEFPLSTYLE"	 , RTSTR	, 1, 0x3011,   -1, IC_ACADBUF, "",	"BYLAYER" },  // if drawing is bycolor, this is readonly
/*A*/	{ "DELOBJ"		 , RTSHORT	, 0, 0x1000, 1761,	 2, "0_1",		"1" },
/*2*/	{ "DEMANDLOAD"	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_3",		"3" },

		//Modified Cybage AW 17-12-01 [
		//Reason : DIASTAT Variable functionality added (Read only variable)
		//Bug no. 54233 from BugZilla
		//{ "DIASTAT"	   , RTSHORT  , 0, 0x00A0, 1896,   2, "0_1",	  "1" },
		{ "DIASTAT" 	 , RTSHORT	, 0, 0x40A0, 1896,	 2, "0_1",		"1" },
		//Modified Cybage AW 17-12-01 ]
		// EBATECH(CNBR) -[ 2003/2/7 AutoCAD 2002 has DIMADEC in header.
/*2*/	{ "DIMADEC"		 , RTSHORT	, 0, 0x1000,10396,	 2, "-1_8",		"-1"},
		// EBATECH(CNBR) ]-
		{ "DIMALT"		 , RTSHORT	, 0, 0x8001,  929,	 1, "0_1",		"0" },
		{ "DIMALTD" 	 , RTSHORT	, 0, 0x8001,  930,	 1, "0_8",		"2" },
		{ "DIMALTF" 	 , RTREAL	, 0, 0x8000,  965,	 8, "1.0E-100_","25.4" },
/*2*/	{ "DIMALTRND"	 , RTREAL	, 0, 0x1000, 9074,	 8, "0_",		"0" },
/*A*/	{ "DIMALTTD"	 , RTSHORT	, 0, 0x9000, 1763,	 2, "0_8",		"2" },
/*A*/	{ "DIMALTTZ"	 , RTSHORT	, 0, 0x9000, 1765,	 2, "0_15", 	"0" },
/*A*/	{ "DIMALTU" 	 , RTSHORT	, 0, 0x9000, 1767,	 2, "1_8",		"2" },
/*A*/	{ "DIMALTZ" 	 , RTSHORT	, 0, 0x9000, 1769,	 2, "0_15", 	"0" },
		{ "DIMAPOST"	 , RTSTR	, 0, 0xA000,  949,	16, "", 		"" },
		{ "DIMASO"		 , RTSHORT	, 0, 0x0001,  931,	 1, "0_1",		"1" },
/*2*/	{ "DIMASSOC"	 , RTSHORT	, 0, 0x00020000,	0,	 2,	"0_2",		"2"	},
		{ "DIMASZ"		 , RTREAL	, 0, 0x0000,  427,	 8, "0_",		"0.18" },
		// EBATECH(watanabe)-[dimatfit
/*A*/	{ "DIMATFIT"	 , RTSHORT	, 0, 0x1000, 2917,	 2, "0_3",		"3" },
		// ]-EBATECH(watanabe)
/*A*/	{ "DIMAUNIT"	 , RTSHORT	, 0, 0x1000, 1771,	 2, "0_4",		"0" },
/*2*/	{ "DIMAZIN" 	 , RTSHORT	, 0, 0x1000, 9082,	 2, "0_3",		"0" },
		// EBATECH(cnbr) -[ 2002/6/15 these block name may or may not expand to IC_ACADBUF for long block name.
		{ "DIMBLK"		 , RTSTR	, 0, 0x0000,  758,	32, "", 		"" },
		{ "DIMBLK1" 	 , RTSTR	, 0, 0x0000, 1150,	32, "", 		"" },
		{ "DIMBLK2" 	 , RTSTR	, 0, 0x0000, 1183,	32, "", 		"" },
		// EBATECH(cnbr) ]-
		{ "DIMCEN"		 , RTREAL	, 0, 0x0000,  483,	 8, "", 		"0.09" },
		{ "DIMCLRD" 	 , RTSHORT	, 0, 0x0000, 1329,	 2, "0_256",	"0" },
		{ "DIMCLRE" 	 , RTSHORT	, 0, 0x0000, 1331,	 2, "0_256",	"0" },
		{ "DIMCLRT" 	 , RTSHORT	, 0, 0x0000, 1333,	 2, "0_256",	"0" },
/*A*/	{ "DIMDEC"		 , RTSHORT	, 0, 0x1000, 1773,	 2, "0_8",		"4" },
		{ "DIMDLE"		 , RTREAL	, 0, 0x0000,  750,	 8, "0_",		"0" },
		{ "DIMDLI"		 , RTREAL	, 0, 0x0000,  443,	 8, "0_",		"0.38" },
/*2*/	{ "DIMDSEP"		 , RTSTR	, 0, 0x1000,10398,	 2, "",         ""  },  // 2003/3/31 copy from ObjVar
		{ "DIMEXE"		 , RTREAL	, 0, 0x0000,  451,	 8, "0_",		"0.18" },
		{ "DIMEXO"		 , RTREAL	, 0, 0x0000,  435,	 8, "0_",		"0.0625" },
/*A*/	{ "DIMFIT"		 , RTSHORT	, 0, 0x1000, 1775,	 2, "0_5",		"3" },
		// EBATECH(watanabe)-[dimfrac
/*A*/	{ "DIMFRAC" 	 , RTSHORT	, 0, 0x1000, 2919,	 2, "0_2",		"0" },
		// ]-EBATECH(watanabe)
		{ "DIMGAP"		 , RTREAL	, 0, 0x0000, 1707,	 8, "", 		"0.09" },
/*A*/	{ "DIMJUST" 	 , RTSHORT	, 0, 0x1000, 1777,	 2, "0_4",		"0" },
		// EBATECH(watanabe)-[dimldrblk
		{ "DIMLDRBLK"	 , RTSTR	, 0, 0x0000, 2885,	32, "", 		""	},
		// ]-EBATECH(watanabe)
		{ "DIMLFAC" 	 , RTREAL	, 0, 0x0000,  973,	 8, "", 		"1" },
		{ "DIMLIM"		 , RTSHORT	, 0, 0x8041,  500,	 1, "0_1",		"0" },
		// EBATECH(watanabe)-[dimlunit
/*A*/	{ "DIMLUNIT"	 , RTSHORT	, 0, 0x1000, 2921,	 2, "1_6",		"2" },
		// ]-EBATECH(watanabe)
/*2*/	{ "DIMLWD"		 , RTSHORT	, 0, 0x1010, 9084,	 2, "-3_211",	"-2" },
/*2*/	{ "DIMLWE"		 , RTSHORT	, 0, 0x1010, 9086,	 2, "-3_211",	"-2" },
		{ "DIMPOST" 	 , RTSTR	, 0, 0x2000,  933,	16, "", 		"" },
		{ "DIMRND"		 , RTREAL	, 0, 0x0000,  742,	 8, "0_",		"0" },
		{ "DIMSAH"		 , RTSHORT	, 0, 0x0001, 1216,	 1, "0_1",		"0" },
		{ "DIMSCALE"	 , RTREAL	, 0, 0x0000,  419,	 8, "0_",		"1" },
/*A*/	{ "DIMSD1"		 , RTSHORT	, 0, 0x1000, 1779,	 2, "0_1",		"0" },
/*A*/	{ "DIMSD2"		 , RTSHORT	, 0, 0x1000, 1781,	 2, "0_1",		"0" },
		{ "DIMSE1"		 , RTSHORT	, 0, 0x0001,  503,	 1, "0_1",		"0" },
		{ "DIMSE2"		 , RTSHORT	, 0, 0x0001,  504,	 1, "0_1",		"0" },
		{ "DIMSHO"		 , RTSHORT	, 0, 0x0001,  932,	 1, "0_1",		"1" },
		{ "DIMSOXD" 	 , RTSHORT	, 0, 0x0001, 1218,	 1, "0_1",		"0" },
/*I*/	{ "DIMSS"		 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"0" },
/*I*/	{ "DIMSSPOS"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "1_5",		"2" },
		{ "DIMSTYLE"	 , RTSTR	, 0, 0x0051, 1324,	 2, "", 		"*UNNAMED" }, // EBATECH 0x0011->0x0051 (this is changed on StatusBar)
		{ "DIMTAD"		 , RTSHORT	, 0, 0x0001,  505,	 1, "0_3",		"0" },
/*A*/	{ "DIMTDEC" 	 , RTSHORT	, 0, 0x1000, 1783,	 2, "0_8",		"4" },
		{ "DIMTFAC" 	 , RTREAL	, 0, 0x8000, 1503,	 8, "1.0E-100_","1" },
		{ "DIMTIH"		 , RTSHORT	, 0, 0x0001,  501,	 1, "0_1",		"1" },
		{ "DIMTIX"		 , RTSHORT	, 0, 0x0001, 1217,	 1, "0_1",		"0" },
		{ "DIMTM"		 , RTREAL	, 0, 0x8000,  467,	 8, "", 		"0" },
		// EBATECH(watanabe)-[dimtmove
/*A*/	{ "DIMTMOVE"	 , RTSHORT	, 0, 0x1000, 2923,	 2, "0_2",		"0" },
		// ]-EBATECH(watanabe)
		{ "DIMTOFL" 	 , RTSHORT	, 0, 0x0001, 1149,	 1, "0_1",		"0" },
		{ "DIMTOH"		 , RTSHORT	, 0, 0x0001,  502,	 1, "0_1",		"1" },
		{ "DIMTOL"		 , RTSHORT	, 0, 0x8041,  499,	 1, "0_1",		"0" },
/*A*/	{ "DIMTOLJ" 	 , RTSHORT	, 0, 0x9000, 1785,	 2, "0_2",		"1" },
		{ "DIMTP"		 , RTREAL	, 0, 0x8000,  459,	 8, "", 		"0" },
		{ "DIMTSZ"		 , RTREAL	, 0, 0x0000,  491,	 8, "0_",		"0" },
		{ "DIMTVP"		 , RTREAL	, 0, 0x0000, 1219,	 8, "", 		"0" },
/*A*/	{ "DIMTXSTY"	 , RTSTR	, 0, 0x1011, 1787,	 2, "", 		"STANDARD" },
		{ "DIMTXT"		 , RTREAL	, 0, 0x0000,  475,	 8, "1.0E-100_","0.18" },
/*A*/	{ "DIMTZIN" 	 , RTSHORT	, 0, 0x1000, 1789,	 2, "0_15", 	"0" },
/*A*/	{ "DIMUNIT" 	 , RTSHORT	, 0, 0x1000, 1791,	 2, "1_8",		"2" },
/*A*/	{ "DIMUPT"		 , RTSHORT	, 0, 0x1000, 1793,	 2, "0_1",		"0" },
		{ "DIMZIN"		 , RTSHORT	, 0, 0x0001,  741,	 1, "0_15", 	"0" },
/*A*/	{ "DISPSILH"	 , RTSHORT	, 0, 0x1000, 1795,	 2, "0_1",		"0" },
		// Modified Cybage SBD 18/02/2002
		// Reason: Fix for Bug No. 62875 from Bugzilla
		//{ "DISTANCE"	   , RTREAL   , 0, 0x00A0, 1898,   8, "0_", 	  "0" },
		{ "DISTANCE"	 , RTREAL	, 0, 0x40A0, 1898,	 8, "0_",		"0" },
		{ "DONUTID" 	 , RTREAL	, 0, 0x00A0, 1906,	 8, "0_",		"0.5" },
		{ "DONUTOD" 	 , RTREAL	, 0, 0x00A0, 1914,	 8, "0_",		"1" },
		// DP: In ACAD2000 this variable is saving in and loading from registry.
		// EBATECH(CNBR) Release offset and size
		{ "DRAGMODE"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_2",		"2" },
		//{ "DRAGMODE"	   , RTSHORT  , 1, 0x0000,	304,   2, "0_2",	  "2" },
		//{ "DRAGMODE"	   , RTSHORT  , 0, 0x0000,	304,   2, "0_2",	  "2" },
		{ "DRAGOPEN"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"1" },
/*U*/	{ "DRAGP1"		 , RTSHORT	, 1, 0x1004,   -1,	 2, "0_32767",	"10" },
/*U*/	{ "DRAGP2"		 , RTSHORT	, 1, 0x1004,   -1,	 2, "0_32767",	"25" },
		// EBATECH(CNBR) 2002/6/15 Swap order
/*2*/	{ "DWGCHECK"	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",		 "0" },
		{ "DWGCODEPAGE"  , RTSTR	, 0, 0x4011, 1017,	 2, "", 		"ANSI_1252" },
		//{ "DWGCODEPAGE"  , RTSTR	  , 0, 0x4011, 1017,   2, "",		  "ANSI_1252" },
/*2*/	//{ "DWGCHECK"	   , RTSHORT  , 1, 0x1000,	 -1,   2, "0_1",	   "0" },
		// EBATECH(CNBR) ]-
		{ "DWGNAME" 	 , RTSTR	, 2, 0x0008,   -1,	 0, "", 		"" },  /* Very special.  Not in a buffer but CAN be set (it's flp->pn). */
		{ "DWGPREFIX"	 , RTSTR	, 2, 0x0008,   -1,	 0, "", 		"" },
		// EBATECH(CNBR) 2002/6/10 User Read only, 2002/9/19 Default=0
		{ "DWGTITLED"	 , RTSHORT	, 0, 0x40A0, 1922,	 2, "0_1",		"0" },
		//{ "DWGTITLED"    , RTSHORT  , 0, 0x00A0, 1922,   2, "0_1",	  "1" },
		// DP: In ACAD2000 this variable is saving in and loading from registry.
		// EBATECH(CNBR) Release offset and size
/*A*/	{ "EDGEMODE"	 , RTSHORT	, 1, 0x10A0,   -1,	 2, "0_1",		"0" },
/*A*/	//{ "EDGEMODE"	   , RTSHORT  , 1, 0x10A0, 1926,   2, "0_1",	  "0" },
/*A*/	//{ "EDGEMODE"	   , RTSHORT  , 0, 0x10A0, 1926,   2, "0_1",	  "0" },
		{ "ELEVATION"	 , RTREAL	, 0, 0x0043,  553,	 8, "", 		"0" },
		{ "ERRNO"		 , RTSHORT	, 2, 0x0000,   -1,	 2, "", 		"0" },
/*2*/	{ "EXEDIR"		 , RTSTR	, 2, 0x7000,   -1, 256, "", 		"" }, // Set value in startup process
		{ "EXPERT"		 , RTSHORT	, 2, 0x0000,   -1,	 2, "0_5",		 "0" },
/*I*/	{ "EXPLEVEL"	 , RTSHORT	, 1, 0x0060,   -1,	 2, "1_3",		"3" },
/*A*/	{ "EXPLMODE"	 , RTSHORT	, 0, 0x10A0, 1928,	 2, "0_1",		"1" },
/*I*/	{ "EXPLSTBLK"	 , RTSTR	, 1, 0x0020,   -1,	25, "", 		"" },
/*I*/	{ "EXPLSTDS"	 , RTSTR	, 1, 0x0020,   -1,	25, "", 		"" },
/*I*/	{ "EXPLSTLAY"	 , RTSTR	, 1, 0x0020,   -1,	25, "", 		"" },
/*I*/	{ "EXPLSTLT"	 , RTSTR	, 1, 0x0020,   -1,	25, "", 		"" },

		// Modified CyberAge VSB 03/09/2001
		// Reason: Fix for Bug No. 69131 from Bugzilla
/*I*/	//{ "EXPLSTTS"	   , RTSTR	  , 1, 0x0020,	 -1,  25, "",		  "" },
/*I*/	{ "EXPLSTTS"	 , RTSTR	, 1, 0x0020,   -1,	37, "", 		"" },
/*I*/	{ "EXPLSTUCS"	 , RTSTR	, 1, 0x0020,   -1,	25, "", 		"" },
/*I*/	{ "EXPLSTVW"	 , RTSTR	, 1, 0x0020,   -1,	25, "", 		"" },
		{ "EXTMAX"		 , RT3DPOINT, 0, 0x4400,  144,	24, "", 		"12.0,9.0,0.0" },
		{ "EXTMIN"		 , RT3DPOINT, 0, 0x4400,  120,	24, "", 		"0.0,0.0,0.0" },
/*2*/	{ "EXTNAMES"	 , RTSHORT	, 0, 0x5000, 9088,	 2, "0_1",		"1" }, // ODT2.007 forces adhd->extnames as 1.
/*2*/	{ "FACETRATIO"	 , RTSHORT	, 2, 0x1000,   -1,	 2, "0_1",		"0" },
/*A*/	{ "FACETRES"	 , RTREAL	, 0, 0x1020, 1797,	 8, "0.01_10",	"0.5" },
/*U*/	{ "FASTZOOM"	 , RTSHORT	, 0, 0x4064, 2859,	 2, "", 	  "0"/*"1"*/ }, /*DG - 7.2.2002*/// acad2000 doesn't use it, so turn it off for default.
/*U*/	{ "FFLIMIT" 	 , RTSHORT	, 1, 0x1004,   -1,	 2, "0_100",	"0" }, // Ebatech(cnbr) what does this used for?
/*U*/	{ "FIELDDISPLAY" , RTSHORT	, 1, 0x1004,   -1,	 2, "0_1",		"1" },
		{ "FILEDIA" 	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"1" },
		// EBATEHCH(CNBR) -[ 2002/9/19 FILLETRAD match to Acad
		{ "FILLETRAD"	 , RTREAL	, 0, 0x0000,  386,	 8, "0_",		"0.5" }, // <- 0.0
		{ "FILLMODE"	 , RTSHORT	, 0, 0x0000,  300,	 2, "0_1",		"1" },
/*U*/	{ "FITTYPE" 	 , RTSHORT	, 0, 0x0024, 2821,	 2, "2_3",		"2" }, // Ebatech(cnbr) ICAD used this value in PEDIT command, but ACAD doesn't. This is obsolute in A2K?
		{ "FLATLAND"	 , RTSHORT	, 0, 0x0000, 1290,	 2, "0_1",		"0" }, // Ebatech(cnbr) This header variable keep drawings, but nowhere in ICAD access it. This is obsolute in A2K?
/*A*/	{ "FONTALT" 	 , RTSTR	, 1, 0x3010,   -1,	51, "", 		"txt" },
/*A*/	{ "FONTMAP" 	 , RTSTR	, 1, 0x1050,   -1,	51, "", 		"icad.fmp" },
		{ "FRONTZ"		 , RTREAL	, 0, 0x4C20, 1131,	 8, "", 		"0" },
/*2*/	{ "FULLOPEN"	 , RTSHORT	, 2, 0x5000,   -1,	 2, "0_1",		"1" }, // ReadOnly for IntelliCAD
/*U*/	{ "GLOBCHECK"	 , RTSHORT	, 2, 0x1004,   -1,	 2, "0_3",		"0" }, // Ebatech(cnbr) This value is not used.
/*U*/	{ "GPSLASTPT"	 , RT3DPOINT, 0, 0x00A4, 1930,	24, "", 		""	}, // Ebatech(cnbr) GPS Related System variable.
/*U*/	{ "GPSMODE" 	 , RTSHORT	, 0, 0x00A4, 1954,	 2, "0_1",		"0" }, // Ebatech(cnbr) GPS Related System variable.
/*U*/	{ "GPSPAUSED"	 , RTSHORT	, 0, 0x00A4, 1956,	 2, "0_1",		"1" }, // Ebatech(cnbr) GPS Related System variable.
/*U*/	{ "GPSPLAYED"	 , RTSTR	, 0, 0x00A4, 1958, 256, "", 		""	}, // Ebatech(cnbr) GPS Related System variable.
		{ "GRIDMODE"	 , RTSHORT	, 0, 0x0860,  278,	 2, "0_1",		"0" },
		{ "GRIDUNIT"	 , RTPOINT	, 0, 0x0870,  280,	16, "", 		"0.5,0.5,0" },
		{ "GRIPBLOCK"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"0" },
		{ "GRIPCOLOR"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "1_255",	"3" },
		{ "GRIPHOT" 	 , RTSHORT	, 1, 0x0000,   -1,	 2, "1_255",	"1" },
/*U*/	{ "GRIPHOVER"	 , RTSHORT	, 1, 0x1004,   -1,	 2, "1_255",	"3" },
		{ "GRIPOBJLIMIT" , RTSHORT	, 1, 0x1000,   -1,	 2, "1_32767",	"100" },
		{ "GRIPS"		 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"1" },
/*U*/	{ "GRIPSIZE"	 , RTSHORT	, 1, 0x0004,   -1,	 2, "1_255",	"3" },
		{ "GRIPTIPS"     , RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",	"1" },
/*2*/	{ "HALOGAP"		 , RTSHORT	, 0, 0x00020000,	0,	 2,	"0_100",	"0"	},
		{ "HANDLES" 	 , RTSHORT	, 0, 0x4000, 1260,	 2, "0_1",		"1" },
/*U*/	{ "HANDSEED"	 , RTSTR	, 0, 0x4005, 1262,	 8, "", 		"25" },  /* Reserve some handles */
/*U*/	{ "HANDUDCOPY"	 , RTSHORT	, 2, 0x0024,   -1,	 2, "0_1",		"0" }, // Ebatech(cnbr) This value is not used.
/*2*/	{ "HIDEPRECISION", RTSHORT	, 2, 0x1000,   -1,	 2, "0_1",		"0" },
/*2*/	{ "HIDETEXT"	 , RTSHORT	, 0, 0x00020000,	0,	 2,	"0_1",		"1"	},
		{ "HIGHLIGHT"	 , RTSHORT	, 0, 0x00A0, 2214,	 2, "0_1",		"1" },
		{ "HPANG"		 , RTREAL	, 0, 0x01A0, 2216,	 8, "", 		"0" },
		{ "HPASSOC"		 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",		"1" },
/*A*/	{ "HPBOUND" 	 , RTSHORT	, 0, 0x10A0, 2224,	 2, "0_1",		"1" },
		{ "HPDOUBLE"	 , RTSHORT	, 0, 0x00A0, 2226,	 2, "0_1",		"0" },
		{ "HPDRAWORDER"	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_4",		"3" },
/*U*/	{ "HPGAPTOL"	 , RTSHORT	, 1, 0x1004,   -1,	 2, "0_5000",	"0" },
		{ "HPNAME"		 , RTSTR	, 0, 0x00A0, 2228,	51, "", 		"" },
		// EBATECH(CNBR) -[ 2002/6/15 HPSCALE & HPSPACE are grater than zero.
		{ "HPSCALE" 	 , RTREAL	, 0, 0x00A0, 2279,	 8, "1.0E-100_",  "1" },
		{ "HPSPACE" 	 , RTREAL	, 0, 0x00A0, 2287,	 8, "1.0E-100_",  "1" },
		//{ "HPSCALE"	   , RTREAL   , 0, 0x00A0, 2279,   8, "0_", 	  "1" },
		//{ "HPSPACE"	   , RTREAL   , 0, 0x00A0, 2287,   8, "0_", 	  "1" },
/*I*/	{ "HPSTYLE" 	 , RTSHORT	, 0, 0x0020, 2811,	 2, "0_2",		"0" },
		// EBATECH(watanabe)-[dimldrblk
		{ "HYPERLINKBASE", RTSTR	, 0, 0x0000, 2925, 256,"",	  ""  },
		// ]-EBATECH(watanabe)
		{ "ICADVER" 	 , RTSTR	, 2, 0x4008,   -1,	 0, "", 		""	},
/*2*/	{ "IMAGEHLT"	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",		"0" },
		{ "INDEXCTL"	 , RTSHORT	, 0, 0x00020000,	0,	 2,	"0_3",		"0" },
/*2*/	{ "INETLOCATION" , RTSTR	, 1, 0x3000,   -1, 256, "", 		"http://www.intellicad.org" },
		{ "INSBASE" 	 , RT3DPOINT, 0, 0x0001,   94,	24, "", 		"0,0,0" },
		{ "INSNAME" 	 , RTSTR	, 0, 0x00A0, 2295,	51, "", 		"" },
/*2*/	{ "INSUNITS"	 , RTSHORT	, 0, 0x1000, 9090,	 2, "0_20", 	"0" },
/*2*/	{ "INSUNITSDEFSOURCE", RTSHORT, 1,0x1000,  -1,	 2, "0_20", 	"0" },
/*2*/	{ "INSUNITSDEFTARGET", RTSHORT, 1,0x1000,  -1,	 2, "0_20", 	"0" },
/*U*/	{ "INTERSECTIONCOLOR", RTSHORT, 1, 0x1004, -1,	 2, "0_257",	"257" },
/*2*/	{ "ISAVEBAK"	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",		"1" }, // Ebatech(cnbr) Icad uses MAKEBAK.
		{ "ISAVEPERCENT" , RTSHORT	, 1, 0x0000,   -1,	 2, "0_100",	"50" }, // Ebatech(cnbr) Icad always save full.
/*U*/	{ "ISLASTCMDPT"  , RTSHORT	, 0, 0x0024, 2823,	 2, "0_1",		"0" },
/*A*/	{ "ISOLINES"	 , RTSHORT	, 0, 0x1020, 1805,	 2, "0_2047",	"4" },
		{ "LASTANGLE"	 , RTREAL	, 0, 0x01A0, 2346,	 8, "", 		"0" },
/*I*/	{ "LASTCMDANG"	 , RTREAL	, 0, 0x0020, 2849,	 8, "", 		"" },
/*U*/	{ "LASTCMDPT"	 , RT3DPOINT, 0, 0x0025, 2825,	24, "", 		"" },
		{ "LASTPOINT"	 , RT3DPOINT, 0, 0x00A1, 2354,	24, "", 		"0,0,0" },
		{ "LASTPROMPT"	 , RTSTR	, 2, 0x4000,   -1, 256, "", 		"" },
/*2*/	{ "LAYOUTREGENCTRL", RTSHORT , 1, 0x1000,   -1,	 2, "0_2",		"2" },
		{ "LENSLENGTH"	 , RTREAL	, 0, 0x4820, 1115,	 8, "", 		"50" },
		{ "LIMCHECK"	 , RTSHORT	, 0, 0x0000,  506,	 2, "0_1",		"0" },
		{ "LIMMAX"		 , RTPOINT	, 0, 0x0000,  184,	16, "", 		"12,9,0" },
		{ "LIMMIN"		 , RTPOINT	, 0, 0x0000,  168,	16, "", 		"0,0,0" },
/*2*/	{ "LISPINIT"	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",		"1" },
/*A*/	{ "LOCALE"		 , RTSTR	, 2, 0x1008,   -1,	 0, "", 		"en" },
/*U*/	{ "LOCALROOTPREFIX", RTSTR	, 1, 0x3004,   -1, 512, "", 		"pathname" },
		{ "LOGFILEMODE"  , RTSHORT	, 2, 0x0000,   -1,	 2, "0_1",		"0" },
		{ "LOGFILENAME"  , RTSTR	, 2, 0x2000,   -1, 256, "", 		"icad.log" },
/*2*/	{ "LOGFILEPATH"  , RTSTR	, 1, 0x3000,   -1, 256, "", 		"" },
		// EBATECH(CNBR) 06-01-2002 -[ LOGINNAME accept spaces.
		{ "LOGINNAME"	 , RTSTR	, 1, 0x2000,   -1,	51, "", 		"IntelliCAD User" },
		//{ "LOGINNAME"    , RTSTR	  , 1, 0x0000,	 -1,  51, "",		  "IntelliCAD User" },
		// EBATECH(CNBR) ]-
		{ "LONGFNAME"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"1" },
		{ "LTSCALE" 	 , RTREAL	, 0, 0x0040,  306,	 8, "1.0E-100_","1" },
/*I*/	{ "LTSCLWBLK"	 , RTSHORT	, 0, 0x0020, 1815,	 2, "0_1",		"0" },
		{ "LUNITS"		 , RTSHORT	, 0, 0x0000,  356,	 2, "1_5",		"2" },
		{ "LUPREC"		 , RTSHORT	, 0, 0x0000,  358,	 2, "0_8",		"4" },
		// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight
/*2*/	{ "LWDEFAULT"	 , RTSHORT	, 1, 0x1050,   -1,	 2, "0_211",	"9" },
/*2*/	{ "LWDISPLAY"	 , RTSHORT	, 0, 0x1040, 9092,	 2, "0_1",		"0" },
		{ "LWPOLYOUT"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"1" },
/*2*/	{ "LWUNITS" 	 , RTSHORT	, 1, 0x1040,   -1,	 2, "0_1",		"1" },
		// EBATECH(CNBR) ]-
/*I*/	{ "MACROREC"	 , RTSHORT	, 2, 0x0020,   -1,	 2, "0_1",		"0" },
		{ "MACROTRACE"	 , RTSHORT	, 2, 0x0000,   -1,	 2, "0_1",		"0" },
		{ "MAKEBAK" 	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"1" },
		// Ebatech(CNBR) 2002/10/20 Expand range 2-48 to 2-64, Change default 16 to 64.
		{ "MAXACTVP"	 , RTSHORT	, 0, 0x0000, 1705,	 2, "2_64", 	"64" },
		//{ "MAXACTVP"	 , RTSHORT	, 0, 0x0000, 1705,	 2, "2_48", 	"16" },
		// EBATECH(CNBR) ]-
/*A*/	{ "MAXOBJMEM"	 , RTLONG	, 2, 0x1004,   -1,	 4, "1_",		"2147483647" },
		{ "MAXSORT" 	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_32767",	"200" },
/*I*/	{ "MBUTTONPAN"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"1" },
/*2*/	{ "MEASUREINIT"  , RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",		"0" },
		{ "MEASUREMENT"  , RTSHORT	, 0, 0x0000, 2857,	 2, "0_1",		"0" },
		{ "MENUCTL" 	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"1" },
		{ "MENUECHO"	 , RTSHORT	, 0, 0x00A0, 2378,	 2, "0_15", 	"0" },
		{ "MENUNAME"	 , RTSTR	, 0, 0x2011,  404,	15, "", 		"icad" },  /* Part 1 */
		{ "MIRRTEXT"	 , RTSHORT	, 0, 0x0000, 1005,	 2, "0_1",		"1" },
		{ "MODEMACRO"	 , RTSTR	, 2, 0x2000,   -1,	51, "", 		"" },
		// EBATECH(CNBR) -[ 2002/6/10 MTEXTED accept spaces and enlarge size.
/*A*/	{ "MTEXTED" 	 , RTSTR	, 1, 0x3010,   -1, 256, "", 		"" },
/*U*/	{ "MTEXTFIXED"	 , RTSHORT	, 1, 0x1004,   -1,	 2, "0_1",		"0" },
/*U*/	{ "MTJIGSTRING"  , RTSTR	, 1, 0x3004,   -1, 256, "", 		"abc" },
		{ "MYDOCUMENTSPREFIX", RTSTR	, 1, 0x3000,   -1, 512, "", 		"pathname" },
///*A*/   { "MTEXTED"	   , RTSTR	  , 1, 0x1010,	 -1,  51, "",		  "" },
		// EBATECH(CNBR) ]-
/*I*/	{ "NFILELIST"	 , RTSHORT	, 1, 0x0060,   -1,	 2, "0_10", 	"4" },
/*2*/	{ "NOMUTT"		 , RTSHORT	, 2, 0x1000,   -1,	 2, "0_1",		"0" },
/*2*/	{ "OBSCUREDCOLOR", RTSHORT	, 0, 0x0002000,	0,	 2,	"0_256",	"0"	},	// in DXF REF OBSCUREDCOLOR
/*2*/	{ "OBSCUREDLTYPE", RTSHORT	, 0, 0x0002000,	0,	 2,	"0_11",		"0"	},	// in DXF REF OBSCUREDLTYPE
		{ "OFFSETDIST"	 , RTREAL	, 0, 0x00A0, 2380,	 8, "", 		"-1" },
/*2*/	{ "OFFSETGAPTYPE", RTSHORT	, 1, 0x1000,   -1,	 2, "0_2",		"0" },
/*2*/	{ "OLEHIDE" 	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_3",		"0" }, // ACAD is General\ShowProxyGraphics
/*2*/	{ "OLEQUALITY"	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_4",		"0" }, // ACAD is General\OLEQUALITY
/*2*/	{ "OLESTARTUP"	 , RTSHORT	, 0, 0x1000, 9094,	 2, "0_1",		"0" },
		{ "ORTHOMODE"	 , RTSHORT	, 0, 0x0040,  296,	 2, "0_1",		"0" },
 		// Modified by AG 9/11/2000
		// Acad declare 16383 as max value for OSMODE and 16384(0x4000) flag is used to turn all modes off
		// without clearing this value to NULL. This way we get 32767
		// DP: In ACAD2000 this variable is saving in and loading from registry.
		// EBATECH(CNBR) Release offset and size
 		{ "OSMODE"	   , RTSHORT	, 1, 0x0040,  -1,	 2, "0_32767",	"0" },
 		//{ "OSMODE"	   , RTSHORT  , 1, 0x0040,	400,   2, "0_32767",  "0" },
 		//{ "OSMODE"	   , RTSHORT  , 0, 0x0040,	400,   2, "0_32767",  "0" },
 		//{ "OSMODE"		 , RTSHORT	, 0, 0x0040,  400,	 2, "0_8191",  "0" }, // Reason : Fix for bug no. 46850
/*2*/	{ "OSNAPCOORD"	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_2",		"2" },
/*U*/	{ "OSNAPHATCH"	 , RTSHORT	, 1, 0x1004,   -1,	 2, "0_12",		"1" },
/*U*/	{ "PALETTEOPAQUE", RTSHORT	, 1, 0x1004,   -1,	 2, "0_3",		"0" },
/*2*/	{ "PAPERUPDATE"  , RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",		"0" },
		{ "PDMODE"		 , RTSHORT	, 0, 0x0040,  861,	 2, "0_100",	"0" },
		{ "PDSIZE"		 , RTREAL	, 0, 0x0040,  863,	 8, "", 		"0" },
		{ "PEDITACCEPT"	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",		"1" },
/*A*/	{ "PELLIPSE"	 , RTSHORT	, 0, 0x1020, 1807,	 2, "0_1",		"0" },
		// EBATECH(CNBR) 2002/12/2 Bugzilla#78395 AREA and PERIMETER are user read-only var.(0x00A0->0x40A0) AREA and PERIMETER accepts minus value.
		{ "PERIMETER"	 , RTREAL	, 0, 0x40A0, 2388,	 8, "",		"0" },
		{ "PFACEVMAX"	 , RTSHORT	, 2, 0x0000,   -1,	 2, "3_",		"4" },
/*U*/	{ "PHANDLE" 	 , RTLONG	, 2, 0x0004,   -1,	 4, "", 		"0" },
		{ "PICKADD" 	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"1" },
		{ "PICKAUTO"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"1" },
		{ "PICKBOX" 	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_32767",	"3" },
		{ "PICKDRAG"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"0" },
		{ "PICKFIRST"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"1" },
/*A*/	{ "PICKSTYLE"	 , RTSHORT	, 0, 0x1000, 1809,	 2, "0_3",		"1" },
		{ "PLATFORM"	 , RTSTR	, 2, 0x4008,   -1,	 0, "", 		"" },
		{ "PLINEGEN"	 , RTSHORT	, 0, 0x0000,  118,	 2, "0_1",		"0" },
		{ "PLINETYPE"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_2",		"2" },
		{ "PLINEWID"	 , RTREAL	, 0, 0x0000,  871,	 8, "0_",		"0" },
		{ "PLOTID"		 , RTSTR	, 1, 0x0000,   -1,	51, "", 		"" },
/*U*/	{ "PLOTOFFSET"	 , RTSHORT	, 1, 0x1004,   -1,	 2, "0_1",		"0" },
		// EBATECH(CNBR) 2002/6/15 Fixed Range. ICAD does not support this value.
/*A*/	{ "PLOTROTMODE"  , RTSHORT	, 1, 0x1000,   -1,	 2, "0_2",		"2" },
///*A*/   { "PLOTROTMODE"  , RTSHORT  , 1, 0x1000,	 -1,   2, "0_1",	  "1" },
		{ "PLOTTER" 	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_0",		"0" },
/*2*/	{ "PLQUIET" 	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",		"0" },
/*2*/	{ "POLARADDANG"  , RTSTR	, 1, 0x1000,   -1, IC_ACADBUF, "",	 "" },
/*2*/	{ "POLARANG"	 , RTREAL	, 1, 0x1000,   -1,	 8, "5_90",    "90" },
		// EBATECH(CNBR) 2002/6/15 Fixed Range (unlimited).
/*2*/	{ "POLARDIST"	 , RTREAL	, 1, 0x1000,   -1,	 8, "", 	  "0" },
///*2*/   { "POLARDIST"    , RTREAL   , 1, 0x1000,	 -1,   8, "0_", 	  "0" },
/*2*/	{ "POLARMODE"	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_15", 	"0" },
		{ "POLYSIDES"	 , RTSHORT	, 0, 0x00A0, 2396,	 2, "3_1024",	"4" },
		{ "POPUPS"		 , RTSHORT	, 2, 0x4000,   -1,	 2, "0_1",		"1" },
/*2*/	{ "PRODUCT" 	 , RTSTR	, 2, 0x5000,   -1,	51, "", 		"IntelliCAD" },//ReadOnly
		{ "PROGBAR" 	 , RTSHORT	, 2, 0x0000,   -1,	 2, "0_1",		"1" },
/*2*/	{ "PROGRAM" 	 , RTSTR	, 2, 0x5000,   -1,	51, "", 		"ICAD" },//ReadOnly
		{ "PROJECTNAME"	 , RTSTR	, 0, 0x000202000,	0,	32,	"",			"" }, // Accept spaces
/*A*/	{ "PROJMODE"	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_2",		"1" },
/*I*/	{ "PROMPTMENU"	 , RTSHORT	, 1, 0x0020,   -1,	 2, "0_1",		"1" },
		{ "PROXYGRAPHICS", RTSHORT	, 0, 0x0000, 2865,	 2, "0_1",		"1" },
/*2*/	{ "PROXYNOTICE"  , RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",		"1" },
/*2*/	{ "PROXYSHOW"	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_2",		"1" },
/*2*/	{ "PROXYWEBSEARCH" , RTSHORT, 1, 0x1000,   -1,	 2, "0_2",		"1" },
		{ "PSLTSCALE"	 , RTSHORT	, 0, 0x0000,  342,	 2, "0_1",		"1" },
		{ "PSPROLOG"	 , RTSTR	, 1, 0x0000,   -1,	51, "", 		"" },
		{ "PSQUALITY"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "-32768_32767","75" },
		// EBATECH(CNBR) 2002/9/30 Implement of PSTYLEMODE and PSTYLEPOLICY
/*2*/	{ "PSTYLEMODE"	 , RTSHORT, 0, 0x5000,	10386,	 2, "0_1",		"1" }, // User ReadOnly
/*2*/	{ "PSTYLEPOLICY" , RTSHORT, 1, 0x1000,	 -1,   2, "0_1",	  "1" }, // Default is bycolor
/*2*/	{ "PSVPSCALE"	 , RTREAL,	0, 0x1000,	10388,	 8, "0_",	   "0" },
// EBATECH(CNBR) 2002/9/19 PUCSBASE is string, Currently User cannot change.
/*2*/	{ "PUCSBASE"	 , RTSTR	, 1, 0x5000,   -1, 256, "", 		"" },
///*2*/	{ "PUCSBASE"	 , RTSHORT, 1, 0x1000,	 -1,   2, "0_2",	  "1" },
/*U*/	{ "P_ELEVATION"  , RTREAL	, 0, 0x10005, 1715,   8, "",		 "0" },
/*U*/	{ "P_EXTMAX"	 , RT3DPOINT, 0, 0x14004, 1615,  24, "",		 "-1E+20,-1E+20,-1E+20" },
/*U*/	{ "P_EXTMIN"	 , RT3DPOINT, 0, 0x14004, 1591,  24, "",		 "1E+20,1E+20,1E+20" },
/*U*/	{ "P_INSBASE"	 , RT3DPOINT, 0, 0x10005, 1671,  24, "",		 "0,0,0" },
/*U*/	{ "P_LIMCHECK"	 , RTSHORT	, 0, 0x10004, 2867,   2, "0_1", 	 "0" },
/*U*/	{ "P_LIMMAX"	 , RTPOINT	, 0, 0x10004, 1655,  16, "",		 "12,9,0" },
/*U*/	{ "P_LIMMIN"	 , RTPOINT	, 0, 0x10004, 1639,  16, "",		 "0,0,0" },
/*U*/	{ "P_UCSNAME"	 , RTSTR	, 0, 0x14015, 1583,   2, "",		 "" },
/*U*/	{ "P_UCSORG"	 , RT3DPOINT, 0, 0x14004, 1511,  24, "",		 "0,0,0" },
/*U*/	{ "P_UCSXDIR"	 , RT3DPOINT, 0, 0x14014, 1535,  24, "",		 "1,0,0" },
/*U*/	{ "P_UCSYDIR"	 , RT3DPOINT, 0, 0x14014, 1559,  24, "",		 "0,1,0" },
/*U*/	{ "QAFLAGS" 	 , RTSHORT	, 1, 0x0004,   -1,	 2, "0_32767",	"0" }, // Undoc ACAD & ICAD
		{ "QTEXTMODE"	 , RTSHORT	, 0, 0x0000,  302,	 2, "0_1",		"0" },
/*U*/	{ "RASTERDPI" 	 , RTSHORT	, 1, 0x0004,   -1,	 2, "100_32767",	"300" },
/*A*/	{ "RASTERPREVIEW", RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",		"1" },
		// Modified Cybage PP 05-12-2000 [
		// Fix for bug reported by Shoku Yanagawa. Ref.: Email dated 22th Nov.
/*2*/	{ "REFEDITNAME"  , RTSTR	, 2, 0x3000,   -1, 256, "", 	 "" },
		{ "REGENMODE"	 , RTSHORT	, 0, 0x0000,  298,	 2, "0_1",		"1" },
/*2*/	{ "REMEMBERFOLDERS", RTSHORT, 1, 0x1000,   -1,	 2, "0_1",		"1" },
/*U*/	{ "REPORTERROR"	 , RTSHORT	, 1, 0x1004,   -1,	 2, "0_1",		"1" },
		{ "RE_INIT" 	 , RTSHORT	, 2, 0x4000,   -1,	 2, "", 		"0" },
		// Modified Cybage PP 05-12-2000 ]
/*U*/	{ "ROAMABLEROOTPREFIX", RTSTR	, 1, 0x3004,   -1, 512, "", 		"pathname" },
/*2*/	{ "RTDISPLAY"	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",		"1" },
		{ "SAVEFILE"	 , RTSTR	, 1, 0x2000,   -1,	51, "", 		".SV$" },
		// EBATECH(CNBR) -[ 2002/6/28 Add SAVEFILEPATH accept NULL or existing path only.
/*2*/	{ "SAVEFILEPATH" , RTSTR	, 1, 0x2010,   -1, 256, "", 		"" },
		// EBATECH(CNBR) ]-
/*A*/	{ "SAVEIMAGES"	 , RTSHORT	, 0, 0x1004, 1813,	 2, "0_2",		"0" },
		{ "SAVENAME"	 , RTSTR	, 0, 0x60A8, 2398,	51, "", 		"" }, // Ebatech(cnbr) Store Filebody. Expand to 256?
		// Modified CyberAge PP 09-22-2000 [
		// Fix for bug reported by Peter Coburn. Ref.: Email dated 14th Sept.
		//{ "SAVETIME"	   , RTSHORT  , 1, 0x0040,	 -1,   2, "0_32767",  "60" },
		{ "SAVEROUNDTRIP"  , RTSHORT	, 1, 0x0000,  -1,	2, "0_1",	   "1" },
		{ "SAVETIME"	 , RTSHORT	, 1, 0x0040,   -1,	 2, "0_240",  "60" },
		// Modified CyberAge PP 09-22-2000 ]
		{ "SCREENBOXES"  , RTSHORT	, 1, 0x0000,   -1,	 2, "0_",		"26" },  /* ° MY system */
		{ "SCREENMODE"	 , RTSHORT	, 1, 0x4000,   -1,	 2, "0_2",		"1" },
/*U*/	{ "SCREENPOS"	 , RTPOINT	, 1, 0x4424,   -1,	16, "", 		"640,480,0" },	/* ° MY system */
		{ "SCREENSIZE"	 , RTPOINT	, 1, 0x4400,   -1,	16, "", 		"640,480,0" },	/* ° MY system */
/*I*/	{ "SCRLHIST"	 , RTSHORT	, 1, 0x0060,   -1,	 2, "0_",		"256" },
/*2*/	{ "SDI" 		 , RTSHORT	, 1, 0x5000,   -1,	 2, "0_1",		"0" },// ReadOnly
		{ "SHADEDGE"	 , RTSHORT	, 0, 0x0000, 1335,	 2, "0_3",		"3" },
		{ "SHADEDIF"	 , RTSHORT	, 0, 0x0000, 1337,	 2, "0_100",	"70" },
/*2*/	{ "SHORTCUTMENU" , RTSHORT	, 1, 0x1000,   -1,	 2, "0_15", 	"11" },
		{ "SHPNAME" 	 , RTSTR	, 0, 0x00A0, 2449,	51, "", 		"" }, // Ebatech(cnbr) Expand size to 256?
		{ "SIGWARN"		 , RTSHORT	, 2, 0x0000,   -1,	 2, "0_1",		"1" },
		// EBATECH(CNBR) -[ 2002/6/15 Fixed Range
		{ "SKETCHINC"	 , RTREAL	, 0, 0x0000,  378,	 8, "1.0E-100_",  "0.1" },
		//{ "SKETCHINC"    , RTREAL   , 0, 0x0000,	378,   8, "",		  "0.1" },
		{ "SKPOLY"		 , RTSHORT	, 0, 0x0000,  835,	 2, "0_1",		"0" },
// Modified CyberAge VSB 12/05/2001
// Reason: Implementation of Flyover Snapping.[
		{ "SNAPALLVIEWS" , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"1" },
// Modified CyberAge VSB 12/05/2001 ]
		{ "SNAPANG" 	 , RTREAL	, 0, 0x0960,  266,	 8, "", 		"0" },
		{ "SNAPBASE"	 , RTPOINT	, 0, 0x0860,  250,	16, "", 		"0,0,0" },
		// Modified Cybage PP --/--/2001
		{ "SNAPCOLOR"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_256",	"2" }, // Ebatech(cnbr) It does not use
		{ "SNAPISOPAIR"  , RTSHORT	, 0, 0x0860,  276,	 2, "0_2",		"0" },
		{ "SNAPMODE"	 , RTSHORT	, 0, 0x0860,  232,	 2, "0_1",		"0" },
		// Modified Cybage PP --/--/2001
		{ "SNAPSIZE"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "1_20",		"6" },
		{ "SNAPSTYL"	 , RTSHORT	, 0, 0x0860,  274,	 2, "0_1",		"0" },
		// Modified Cybage PP --/--/2001
		// Modified Cybage VSB 20/05/2001
		// Changed the limit
		//{ "SNAPTHICKNESS", RTSHORT  , 1, 0x0000,	 -1,   2, "1_4",		"2" },
		{ "SNAPTHICKNESS", RTSHORT	, 1, 0x0000,   -1,	 2, "1_5",		"2" },
/*2*/	{ "SNAPTYPE"	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",		"0" },
		{ "SNAPUNIT"	 , RTPOINT	, 0, 0x0870,  234,	16, "", 		"0.5,0.5,0" },
/*2*/	{ "SOLIDCHECK"	 , RTSHORT	, 2, 0x1000,   -1,	 2, "0_1",		"1" },
		//EBATECH(CNBR) -[ 2002/6/12 bad range -> 2002/11/30 Move to DICTIONARY VAR #SORTENTSISDICVAR#
		//{ "SORTENTS"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_127",	"96" },
		//{ "SORTENTS"	   , RTSHORT  , 1, 0x0000,	 -1,   2, "1_127",	  "96" },
		{ "SORTENTS"	 , RTSHORT	, 0, 0x00020000,	0,	 2,	"0_127",	"96" },
		{ "SPLFRAME"	 , RTSHORT	, 0, 0x0040,  983,	 2, "0_1",		"0" },
		{ "SPLINESEGS"	 , RTSHORT	, 0, 0x0000,  981,	 2, "-32768_32767","8" },
		{ "SPLINETYPE"	 , RTSHORT	, 0, 0x0000, 1292,	 2, "1_20", 	"5" }, // Ebatech(cnbr) It's okay or not?
/*I*/	{ "SRCHPATH"	 , RTSTR	, 1, 0x4020,   -1, IC_ACADBUF, "", 		"" },
/*U*/	{ "STANDARDSVIOLATION", RTSHORT	, 1, 0x1004,   -1,	 2, "0_2",		"2" },
		{ "STARTUP"		 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",		"1" },
/*2*/	{ "STARTUPTODAY" , RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",		"1" },
		{ "SURFTAB1"	 , RTSHORT	, 0, 0x0000, 1276,	 2, "2_32766",	"6" },
		{ "SURFTAB2"	 , RTSHORT	, 0, 0x0000, 1278,	 2, "2_32766",	"6" },
		{ "SURFTYPE"	 , RTSHORT	, 0, 0x0010, 1274,	 2, "", 		"6" },
		{ "SURFU"		 , RTSHORT	, 0, 0x0000, 1270,	 2, "2_200",	"6" },
		{ "SURFV"		 , RTSHORT	, 0, 0x0000, 1272,	 2, "2_200",	"6" },
		{ "SYSCODEPAGE"  , RTSTR	, 2, 0x4008,   -1,	 0, "", 		"ANSI_1252" },	/* Doc says in DWG; can't find. */
		{ "TABMODE" 	 , RTSHORT	, 2, 0x0040,   -1,	 2, "0_1",		"0" },
		{ "TARGET"		 , RT3DPOINT, 0, 0x0821, 1091,	24, "", 		"0,0,0" },
/*I*/	{ "TBCOLOR" 	 , RTSHORT	, 1, 0x0060,   -1,	 2, "0_1",		"1" },
/*I*/	{ "TBSIZE"		 , RTSHORT	, 1, 0x0060,   -1,	 2, "0_1",		"0" },
		{ "TDCREATE"	 , RTREAL	, 0, 0x4201,  799,	 8, "2440000.0_2470000.0", "2444240" },  /* 1-1-80 */
		{ "TDINDWG" 	 , RTREAL	, 0, 0x4201,  815,	 8, "", 		"0" },
		{ "TDUPDATE"	 , RTREAL	, 0, 0x4201,  807,	 8, "2440000.0_2470000.0", "2444240" },  /* 1-1-80 */
		{ "TDUSRTIMER"	 , RTREAL	, 0, 0x4201,  823,	 8, "", 		"0" },
/*2*/	{ "TDUUPDATE"	 , RTREAL	, 0, 0x5201, 9096,	 8, "0_",		"0" },
		{ "TEMPPREFIX"	 , RTSTR	, 1, 0x0000,   -1,	51, "", 		"" },
		//Modified Cybage SBD 10/11/2001 DD/MM/YYYY
		//Reason : Fix for Bug No. 77918 from Bugzilla
		//{ "TEXTANGLE"    , RTREAL   , 0, 0x0020, 2877,   8, "",		  "0.0" },
		{ "TEXTANGLE"	 , RTREAL	, 0, 0x0120, 2877,	 8, "", 		"0.0" },
		{ "TEXTEVAL"	 , RTSHORT	, 2, 0x0000,   -1,	 2, "0_1",		"0" },
/*A*/	{ "TEXTFILL"	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",		"1" },
/*A*/	{ "TEXTQLTY"	 , RTSHORT	, 0, 0x1020, 1811,	 2, "0_100",	"50" },
		// EBATECH(CNBR) -[ 2002/6/15 FIXED range
		{ "TEXTSIZE"	 , RTREAL	, 0, 0x0000,  314,	 8, "1.0E-100_",  "0.2" },
		//{ "TEXTSIZE"	   , RTREAL   , 0, 0x0000,	314,   8, "0_", 	  "0.2" },
		{ "TEXTSTYLE"	 , RTSTR	, 0, 0x0051,  398,	 2, "", 		"STANDARD" }, // EBATECH 0x0011->0x0051 (This is changed on StatusBar)
		{ "THICKNESS"	 , RTREAL	, 0, 0x0000,  561,	 8, "", 		"0" },
		{ "TILEMODE"	 , RTSHORT	, 0, 0x0060, 1585,	 2, "0_1",		"1" },
/*I*/	{ "TIPOFDAY"	 , RTSHORT	, 1, 0x0020,   -1,	 2, "0_1",		"1" },
/*A*/	{ "TOOLTIPS"	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_1",		"1" },
		{ "TRACEWID"	 , RTREAL	, 0, 0x0000,  322,	 8, "0_",		"0.05" },
/*2*/	{ "TRACKPATH"	 , RTSHORT	, 1, 0x1000,   -1,	 2, "0_3",		"0" },
/*U*/	{ "TRAYICONS"	 , RTSHORT	, 1, 0x1004,   -1,	 2, "0_1",		"1" },
/*U*/	{ "TRAYNOTIFY"	 , RTSHORT	, 1, 0x1004,   -1,	 2, "0_1",		"1" },
/*U*/	{ "TRAYTIMEOUT"	 , RTSHORT	, 1, 0x1004,   -1,	 2, "0_10",		"5" },
/*U*/	{ "TREEDEPTH"	 , RTSHORT	, 0, 0x0004,  344,	 2, "-32768_32767","3020" },
/*U*/	{ "TREEMAX" 	 , RTLONG	, 1, 0x0000,   -1,	 4, "", 		"10000000" },
		// DP: In ACAD2000 this variable is saving in and loading from registry.
/*A*/	{ "TRIMMODE"	 , RTSHORT	, 1, 0x10A0, 2500,	 2, "0_1",		"1" },
/*A*/	//{ "TRIMMODE"	   , RTSHORT  , 0, 0x10A0, 2500,   2, "0_1",	"1" },
/*2*/	{ "TSPACEFAC"	 , RTREAL	, 2, 0x1000,   -1,	 8, "0.25_4",	"1" },
/*2*/	{ "TSPACETYPE"	 , RTSHORT	, 2, 0x1000,   -1,	 2, "1_2",		"1" },
		// EBATECH(CNBR) -[ 2001/12/28
		// These variables are kept in 2002 DWG not 2002 DXF.
		// I cannot find them in both DXF HEADER SECTION and DICTIONARYVAR OBJECT.
		// Now I set them as Session variable. They may changed later.
		// 2002/5/30 ODT2009 Support these variable, so move to drawings.
#ifdef	ODTVER_2_009
/*2*/	{ "TSTACKALIGN"  , RTSHORT	, 0, 0x1020,10400,	 2, "0_2",	  "1" },
/*2*/	{ "TSTACKSIZE"	 , RTSHORT	, 0, 0x1020,10402,	 2, "25_125",  "70" },
#else
/*2*/	{ "TSTACKALIGN"  , RTSHORT	, 2, 0x1020,   -1,	 2, "0_2",	  "1" },
/*2*/	{ "TSTACKSIZE"	 , RTSHORT	, 2, 0x1020,   -1,	 2, "25_125",  "70" },
#endif
		// EBATECH(CNBR) ]-
/*2*/	{ "UCSAXISANG"	 , RTSHORT	, 1, 0x1000,   -1,	 2, "5_180",   "90" },
/*2*/	{ "UCSBASE" 	 , RTSTR	, 1, 0x1000,   -1, 256, "", 		"" },
		{ "UCSFOLLOW"	 , RTSHORT	, 0, 0x0021, 1147,	 2, "0_1",		"0" },
		{ "UCSICON" 	 , RTSHORT	, 0, 0x0860, 1294,	 2, "0_3",		"1" },
		{ "UCSICONPOS"	 , RTSHORT	, 1, 0x0020,   -1,	 2, "0_3",		"0" }, // Used by UCSICON command, Undoc ICAD only.
		{ "UCSNAME" 	 , RTSTR	, 0, 0x4011, 1296,	 2, "", 		"" },
		{ "UCSORG"		 , RT3DPOINT, 0, 0x4000, 1019,	24, "", 		"0,0,0" },
/*U*/	{ "UCSORTHO"	 , RTSHORT	, 0, 0x1004, 1147,	 2, "0_1",		"1" },
/*U*/	{ "UCSVIEW"		 , RTSHORT	, 0, 0x1004, 1147,	 2, "0_1",		"1" },
/*2*/	{ "UCSVP"		 , RTSHORT	, 2, 0x1000,   -1,	 2, "0_1",		"0" },
		{ "UCSXDIR" 	 , RT3DPOINT, 0, 0x4010, 1043,	24, "", 		"1,0,0" },
		{ "UCSYDIR" 	 , RT3DPOINT, 0, 0x4010, 1067,	24, "", 		"0,1,0" },
		{ "UNDOCTL" 	 , RTSHORT	, 0, 0x40E0, 2502,	 2, "", 		"5" },	/*D.G.*/// made read-only (+0x4000)
		{ "UNDOMARKS"	 , RTSHORT	, 0, 0x00A0, 2504,	 2, "0_",		"0" },
		{ "UNITMODE"	 , RTSHORT	, 0, 0x0000, 1341,	 2, "0_1",		"0" },
		{ "USERI1"		 , RTSHORT	, 0, 0x0040,  879,	 2, "-32768_32767","0" },
		{ "USERI2"		 , RTSHORT	, 0, 0x0040,  881,	 2, "-32768_32767","0" },
		{ "USERI3"		 , RTSHORT	, 0, 0x0040,  883,	 2, "-32768_32767","0" },
		{ "USERI4"		 , RTSHORT	, 0, 0x0040,  885,	 2, "-32768_32767","0" },
		{ "USERI5"		 , RTSHORT	, 0, 0x0040,  887,	 2, "-32768_32767","0" },
		{ "USERR1"		 , RTREAL	, 0, 0x0000,  889,	 8, "", 		"0" },
		{ "USERR2"		 , RTREAL	, 0, 0x0000,  897,	 8, "", 		"0" },
		{ "USERR3"		 , RTREAL	, 0, 0x0000,  905,	 8, "", 		"0" },
		{ "USERR4"		 , RTREAL	, 0, 0x0000,  913,	 8, "", 		"0" },
		{ "USERR5"		 , RTREAL	, 0, 0x0000,  921,	 8, "", 		"0" },
		// EBATECH(CNBR) -[ Expand size 51 to 256 2001/12/28
		{ "USERS1"		 , RTSTR	, 0, 0x20A0, 9104, 256, "", 		"" },
		{ "USERS2"		 , RTSTR	, 0, 0x20A0, 9360, 256, "", 		"" },
		{ "USERS3"		 , RTSTR	, 0, 0x20A0, 9616, 256, "", 		"" },
		{ "USERS4"		 , RTSTR	, 0, 0x20A0, 9872, 256, "", 		"" },
		{ "USERS5"		 , RTSTR	, 0, 0x20A0,10128, 256, "", 		"" },
		//{ "USERS1"	   , RTSTR	  , 0, 0x20A0, 2506,  51, "",		  "" },
		//{ "USERS2"	   , RTSTR	  , 0, 0x20A0, 2557,  51, "",		  "" },
		//{ "USERS3"	   , RTSTR	  , 0, 0x20A0, 2608,  51, "",		  "" },
		//{ "USERS4"	   , RTSTR	  , 0, 0x20A0, 2659,  51, "",		  "" },
		//{ "USERS5"	   , RTSTR	  , 0, 0x20A0, 2710,  51, "",		  "" },
		// EBATECH(CNBR) ]-
/*U*/	{ "USRTIMER"	 , RTSHORT	, 0, 0x0026,  831,	 2, "0_1",		"1" },	/* In R12/13 headers, but has no ACAD system var */
		{ "VIEWASPECT"	 , RTREAL	, 0, 0x0820,  348,	 8, "", 		"1.0" },
		{ "VIEWCTR" 	 , RT3DPOINT, 0, 0x4C63,  200,	24, "", 		"6.24334,4.5,0" },	/* ° MY system */
		{ "VIEWDIR" 	 , RT3DPOINT, 0, 0x4C71,  569,	24, "", 		"0,0,1" },
		{ "VIEWMODE"	 , RTSHORT	, 0, 0x4C60, 1147,	 2, "", 		"0" },
		{ "VIEWSIZE"	 , RTREAL	, 0, 0x4C60,  224,	 8, "", 		"9" },
		{ "VIEWTWIST"	 , RTREAL	, 0, 0x4D60, 1123,	 8, "", 		"0" },
		{ "VISRETAIN"	 , RTSHORT	, 0, 0x0000, 1723,	 2, "0_1",		"0" },	/* Last R12 header item */
		{ "VSMAX"		 , RT3DPOINT, 0, 0x00A2, 2761,	24, "", 		"10,10,0" },
		{ "VSMIN"		 , RT3DPOINT, 0, 0x00A2, 2785,	24, "", 		"0,0,0" },
/*2*/	{ "WHIPARC" 	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"0" },
/*2*/	{ "WHIPTHREAD"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_3",		"3" },
/*2*/	{ "WMFBKGND"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"1" },
/*2*/	{ "WMFFOREGND"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"0" },
/*I*/	{ "WNDBTEXP"	 , RTSHORT	, 1, 0x0460,   -1,	 2, "", 		"0" },
/*I*/	{ "WNDLCMD" 	 , RTSHORT	, 1, 0x0460,   -1,	 2, "0_3",		"0" },
/*I*/	{ "WNDLMAIN"	 , RTSHORT	, 1, 0x0460,   -1,	 2, "0_2",		"2" },
/*I*/	{ "WNDLMDI" 	 , RTSHORT	, 1, 0x4460,   -1,	 2, "0_2",		"2" },
/*U*/	{ "WNDLSCRL"	 , RTSHORT	, 1, 0x0464,   -1,	 2, "0_1",		"0" },
/*I*/	{ "WNDLSTAT"	 , RTSHORT	, 1, 0x0460,   -1,	 2, "0_1",		"1" },
/*I*/	{ "WNDLTEXP"	 , RTSHORT	, 1, 0x0460,   -1,	 2, "0_2",		"1" },
/*I*/	{ "WNDLTEXT"	 , RTSHORT	, 1, 0x0460,   -1,	 2, "0_3",		"0" },
/*I*/	{ "WNDPCMD" 	 , RTPOINT	, 1, 0x0460,   -1,	16, "", 		"0,0,0" },
/*U*/	{ "WNDPFORM"	 , RTPOINT	, 1, 0x0464,   -1,	16, "", 		"0,0,0" },
/*I*/	{ "WNDPMAIN"	 , RTPOINT	, 1, 0x0460,   -1,	16, "", 		"0,0,0" },
/*I*/	{ "WNDPPMENU"	 , RTPOINT	, 1, 0x0460,   -1,	16, "", 		"0,0,0" },
/*I*/	{ "WNDPTEXP"	 , RTPOINT	, 1, 0x0460,   -1,	16, "", 		"0,0,0" },
/*I*/	{ "WNDPTEXT"	 , RTPOINT	, 1, 0x0460,   -1,	16, "", 		"100,100,0" },
/*I*/	{ "WNDSCMD" 	 , RTPOINT	, 1, 0x0460,   -1,	16, "", 		"0,0,0" },
/*U*/	{ "WNDSFORM"	 , RTPOINT	, 1, 0x0464,   -1,	16, "", 		"0,0,0" },
/*I*/	{ "WNDSMAIN"	 , RTPOINT	, 1, 0x0460,   -1,	16, "", 		"0,0,0" },
/*I*/	{ "WNDSTEXP"	 , RTPOINT	, 1, 0x0460,   -1,	16, "", 		"0,0,0" },
/*I*/	{ "WNDSTEXT"	 , RTPOINT	, 1, 0x0460,   -1,	16, "", 		"500,400,0" },
		{ "WORLDUCS"	 , RTSHORT	, 0, 0x40A2, 2809,	 2, "0_1",		"1" },
		{ "WORLDVIEW"	 , RTSHORT	, 0, 0x0000, 1308,	 2, "0_1",		"1" },
/*U*/	{ "WRITESTAT"	 , RTSHORT	, 0, 0x40A4, 1924,	 2, "0_1",		"1" },
		{ "XCLIPFRAME"	 , RTSHORT	, 0, 0x00020000,	0,	 2,	"0_1",		"0"	},
/*2*/	{ "XEDIT"		 , RTSHORT	, 0, 0x0000,10388,	 2, "0_1",		"1" },
/*2*/	{ "XFADECTL"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_90", 	"50" },
		{ "XLOADCTL"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_2",		"1" },
		{ "XLOADPATH"	 , RTSTR	, 1, 0x0000,   -1,1024, "", 		"" },
		{ "XREFCTL" 	 , RTSHORT	, 1, 0x0000,   -1,	 2, "0_1",		"0" },
/*U*/	{ "XREFNOTIFY" 	 , RTSHORT	, 1, 0x1004,   -1,	 2, "0_2",		"2" },
/*U*/	{ "XREFTYPE" 	 , RTSHORT	, 1, 0x1004,   -1,	 1, "0_1",		"0" },
/*I*/	{ "ZOOMFACTOR"	 , RTSHORT	, 1, 0x0000,   -1,	 2, "3_100",	"10" }, // Undoc Icad, Same to A2k?
		{ "ZOOMPERCENT"  , RTSHORT	, 0, 0x0060, 2875,	 2, "1_20000",	"100"},
/*A*/	{ "_LINFO"		 , RTSTR	, 2, 0x100C,   -1,	 0, "", 		"" },
		{ "_PKSER"		 , RTSTR	, 2, 0x4008,   -1,	 0, "", 		"" },
/*U*/	{ "_SERVER" 	 , RTSHORT	, 2, 0x400C,   -1,	 0, "", 		"" },
// Added Cybage VSB 08/08/2001 [
// Reason: Fix for Bug No: 77822
// Added Cybage VSB 13/08/2001 [
// Reason: Spl. Flag 0x0008 changed to 0x4008. Variable listing did'nt show READ ONLY tag with variable value
		{ "_VENDORNAME"  , RTSTR	, 2, 0x4008,   -1,	60, "", 		"PLACE NAME OF ITC MEMBER HERE"},
// Added Cybage VSB 13/08/2001 ]
// Added Cybage VSB 08/08/2001 ]
		{ "_VERNUM" 	 , RTSTR	, 2, 0x4020,   -1,	11, "", 		"?" }
	};
	// EBATECH(CNBR) -[ 2002/9/19 Support Metric drawing.
	// The following tabel has default values for Metric drawing from scratch.
	// Note: MEASUREMENT and CPLOTSTYLE must be the 1st system variables in the list.
	db_sysvarlink db_measurement_metric[]= {
		{ "MEASUREMENT"  , RTSHORT	, 2, 0x0000, -1,	 2, "0_1",		"1" }, // 1 is Metric
		{ "CHAMFERA"	 , RTREAL	, 2, 0x0000, -1,	 8, "0_",		"10.0" },
		{ "CHAMFERB"	 , RTREAL	, 2, 0x0000, -1,	 8, "0_",		"10.0" },
		{ "CHAMFERC"	 , RTREAL	, 2, 0x1000, -1,	 8, "0_",		"20.0" },
		{ "CMLSCALE"	 , RTREAL	, 2, 0x1000, -1,	 8, "", 		"20.0" },
		{ "DIMALTD" 	 , RTSHORT	, 2, 0x8001, -1,	 1, "0_8",		"4" }, // A2K:3  LT98:4*
		{ "DIMALTF" 	 , RTREAL	, 2, 0x8000, -1,	 8, "1.0E-100_","0.0394" },
		{ "DIMALTTD"	 , RTSHORT	, 2, 0x9000, -1,	 2, "0_8",		"4" }, // A2K:3  LT98:4*
		{ "DIMALTU" 	 , RTSHORT	, 2, 0x9000, -1,	 2, "1_8",		"8" }, // A2K:2  LT98:8*
		{ "DIMASZ"		 , RTREAL	, 2, 0x0000, -1,	 8, "0_",		"2.5" },
		{ "DIMCEN"		 , RTREAL	, 2, 0x0000, -1,	 8, "", 		"2.5" },
		{ "DIMDEC"		 , RTSHORT	, 2, 0x1000, -1,	 2, "0_8",		"4" }, // A2K:2  LT98:4*
		{ "DIMDLI"		 , RTREAL	, 2, 0x0000, -1,	 8, "0_",		"3.75" },
		{ "DIMEXE"		 , RTREAL	, 2, 0x0000, -1,	 8, "0_",		"1.25" },
		{ "DIMEXO"		 , RTREAL	, 2, 0x0000, -1,	 8, "0_",		"0.625" },
		{ "DIMGAP"		 , RTREAL	, 2, 0x0000, -1,	 8, "", 		"0.625" },
		{ "DIMSTYLE"	 , RTSTR	, 2, 0x0051, -1,	 2, "", 		"*UNNAMED" }, // A2K:"ISO-25" LT98:"STANDARD"*
		{ "DIMTAD"		 , RTSHORT	, 2, 0x0001, -1,	 1, "0_3",		"1" },
		{ "DIMTDEC" 	 , RTSHORT	, 2, 0x1000, -1,	 2, "0_8",		"4" },	// A2K:2  LT98:4*
		{ "DIMTIH"		 , RTSHORT	, 2, 0x0001, -1,	 1, "0_1",		"0" },
		{ "DIMTOFL" 	 , RTSHORT	, 2, 0x0001, -1,	 1, "0_1",		"1" },
		{ "DIMTOH"		 , RTSHORT	, 2, 0x0001, -1,	 1, "0_1",		"0" },
		{ "DIMTOLJ" 	 , RTSHORT	, 2, 0x9000, -1,	 2, "0_2",		"1" },	// A2K:0  LT98:1*
		{ "DIMTXT"		 , RTREAL	, 2, 0x0000, -1,	 8, "1.0E-100_","2.5" },
		{ "DIMTZIN" 	 , RTSHORT	, 2, 0x1000, -1,	 2, "0_15", 	"0" },	// A2K:8  LT98:0*
		{ "DIMUNIT" 	 , RTSHORT	, 2, 0x1000, -1,	 2, "1_8",		"8" },	// A2K:2  LT98:8*
		{ "DIMZIN"		 , RTSHORT	, 2, 0x0001, -1,	 1, "0_15", 	"8" },
		{ "DONUTID" 	 , RTREAL	, 2, 0x00A0, -1,	 8, "0_",		"10.0" }, // A2K:0.5 LT98:10*
		{ "DONUTOD" 	 , RTREAL	, 2, 0x00A0, -1,	 8, "0_",		"20.0" }, // A2K:1.0 LT98:20*
		{ "EXTMAX"		 , RT3DPOINT, 2, 0x4400, -1,	24, "", 		"420.0,293.0,0.0" }, // A2K: infinite min
		{ "EXTMIN"		 , RT3DPOINT, 2, 0x4400, -1,	24, "", 		"0.0,0.0,0.0" },	// A2K: infinite max
		{ "FILLETRAD"	 , RTREAL	, 2, 0x0000, -1,	 8, "0_",		"10.0" },
		{ "GRIDUNIT"	 , RTPOINT	, 2, 0x0870, -1,	16, "", 		"10.0,10.0,0" },
		{ "HPNAME"		 , RTSTR	, 2, 0x00A0, -1,	51, "", 		"ANGLE" },
		{ "INSUNITS"	 , RTSHORT	, 2, 0x1000, -1,	 2, "0_20", 	"0" }, // Feature Reserved(Now same as English)
		{ "INSUNITSDEFSOURCE", RTSHORT, 2,0x1000,  -1,	 2, "0_20", 	"0" }, // Feature Reserved(Now same as English)
		{ "INSUNITSDEFTARGET", RTSHORT, 2,0x1000,  -1,	 2, "0_20", 	"0" }, // Feature Reserved(Now same as English)
		{ "LIMMAX"		 , RTPOINT	, 2, 0x0000, -1,	16, "", 		"420,297,0" },
		{ "LIMMIN"		 , RTPOINT	, 2, 0x0000, -1,	16, "", 		"0,0,0" },
		{ "P_EXTMAX"	 , RT3DPOINT, 2, 0x14004,-1,  24, "",		 "-1E+20,-1E+20,-1E+20" },
		{ "P_EXTMIN"	 , RT3DPOINT, 2, 0x14004, -1,  24, "",		 "1E+20,1E+20,1E+20" },
		{ "P_LIMMAX"	 , RTPOINT	, 2, 0x10004, -1,  16, "",		 "420,297,0" },
		{ "P_LIMMIN"	 , RTPOINT	, 2, 0x10004, -1,  16, "",		 "0,0,0" },
		{ "SKETCHINC"	 , RTREAL	, 2, 0x0000, -1,	 8, "1.0E-100_",  "1.0" },
		{ "SNAPUNIT"	 , RTPOINT	, 2, 0x0870, -1,	16, "", 		"10.0,10.0,0.0" },
		{ "TEXTSIZE"	 , RTREAL	, 2, 0x0000, -1,	 8, "1.0E-100_",  "2.5" },
		{ "TRACEWID"	 , RTREAL	, 2, 0x0000, -1,	 8, "0_",		"1.0" },
		{ "VIEWCTR" 	 , RT3DPOINT, 2, 0x4C63, -1,	24, "", 		"210.0,197.0,0" },	/* MY system */
		{ "VIEWSIZE"	 , RTREAL	, 2, 0x4C60, -1,	 8, "", 		"297.0" },
		{ "VSMAX"		 , RT3DPOINT, 2, 0x00A2, -1,	24, "", 		"1500,890,0" },
		{ "VSMIN"		 , RT3DPOINT, 2, 0x00A2, -1,	24, "", 		"-1000,-590,0" },
		{ NULL, 0, 0, 0, 0, 0, NULL, NULL }	// End Marker
	};
	// EBATECH(CNBR) ]-
	db_sysvarlink db_pstylemode_named[]= {
		{ "PSTYLEMODE"	 , RTSHORT	, 2, 0x5000, -1,	 2, "0_1",		"0" }, // 0 is Named Plotstyle
		{ "CPLOTSTYLE"	 , RTSTR	, 2, 0x3011, -1, IC_ACADBUF, "",	"BYLAYER" },
		{ NULL, 0, 0, 0, 0, 0, NULL, NULL }	// End Marker
	};

	int db_nvars=sizeof(db_oldsysvar)/sizeof(db_oldsysvar[0]);

