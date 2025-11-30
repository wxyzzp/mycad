/* D:\ICAD\PRJ.MAIN\LIB\DB\DB.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */


#pragma once

#pragma warning (disable : 4786)	/*DG - 31.1.2003*/// To avoid C4786 warnings.

#if !defined( _DB_H )
#define _DB_H

#if defined( DBDLL )
	#define	DB_CLASS __declspec( dllexport)
	#define	DB_DECLSPEC __declspec( dllexport)
#else
	#if defined( DBSTATIC )
		#define	DB_CLASS
		#define	DB_DECLSPEC
	#else
		#define	DB_CLASS __declspec( dllimport)
		#define	DB_DECLSPEC __declspec( dllimport)
	#endif
#endif

#include "../../ICAD/Configure.h"	// EBATECH(CNBR) 2002.01.14

#include "icadlib.h"

#include <time.h>
#include <limits.h>
#include <float.h>

#include <string>
#include <vector>
typedef std::string CDbString;

#ifndef	DB_API
	#define	DB_API extern "C" DB_CLASS
#endif

	#include "icadlib.h"
	#include "sds.h"

	// This is defined in SDS.H and conflicts with STL template definitions
	// It should only be used by SDS apps
#undef	T

class ProgressMeter
	{
public:
	virtual void Start()=0;
	virtual void Stop()=0;
	virtual void Percent( int percentDone)=0;
	};

DB_API SetAdviseProgressMeter( ProgressMeter *pMeter );
DB_API int db_progresspercent(int percentDone);
DB_API int db_progressstart();
DB_API int db_progressstop();

	// date conversion
DB_API void db_jul2greg(double dJul,short *pYear,short *pMonth,short *pDay,short *pDow,short *pHour,short *pMin,double *pSec);

/*
**                          DATABASE NOTES
**
**  ENTITY NAMES (sds_name)
**
**    The 2 longs must be cast as pointers:
**      The [0] refers to the handle-item (class db_handitem *).
**      The [1] refers to the drawing (class db_drawing *).
*/


/*********** TEMP STUFF FOR ENTITY DATA COMPARISON (UNDO) *****/

struct db_BinChange;


/******* END TEMP STUFF FOR ENTITY DATA COMPARISON (UNDO) *****/


#if	defined(DBDLL) && defined(DB_DEFVARS)
    #define DB_EXTERN
#else
	#define DB_EXTERN DB_EXPORT
#endif


/* Declare pointers to the gra freer functions: */
DB_API void (*db_grafreedofp)(void *);



/*
**  The sizes of the HEADER ([0]), CONFIG ([1]), and SESSION ([2])
**  buffers.  Set by makesvbuf() if any is <1.  These are the sizes
**  in bytes of the db_charbuflink->buf buffers for HEADER,
**  CONFIG, and SESSION system variables.
*/
DB_API int db_sysvarbufsz[3];


/*
**  An error code set by various user-callable functions to an
**  OL_E... error code when an error occurs.  db_ret_lasterror()
**  returns it to the caller.  Used in setting system variable ERRNO.
**  See sds.h for the error codes.
*/
DB_API int db_lasterror;

#include "../drw/odtver.h"	// 2002.05.30 EBATECH(CNBR) ODT Dependency
#include "dbdefines.h"
#include "charbuflink.h"
#include "sysvarlink.h"
#include "fontlink.h"
#include "extvars.h"

/* Forward declaration so that the db_handitem-based classes can refer to it: */

class db_drawing;
class db_handitem;
class db_viewport;

#include "dbprotos.h"

// careful -- these are order dependent
#include "objhandle.h"
#include "hireflink.h"
#include "reactor.h"
#include "handitem.h"
#include "tablerecord.h"
#include "table.h"
#include "class.h"
//#include "header.h"
#include "drawing.h"

//#include "appid.h"
#include "blocktabrec.h"
//#include "dimstyletabrec.h"
//#include "layertabrec.h"
//#include "ltypetabrec.h"
//#include "styletabrec.h"
//#include "ucstabrec.h"
//#include "viewtabrec.h"
//#include "vporttabrec.h"
#include "entity.h"
#include "entitywithextrusion.h"
//#include "viewport.h"
//#include "vxtabrec.h"
#include "entities.h"
//#include "objects.h"

class BHatchBuilderFactory;
// EBATECH(watanabe)-[update leader according to modifying annotation
class CUpdateLeaderFactory;
// ]-EBATECH(watanabe)

// ***********************************************************
// This class acts as a singleton containing data shared
// by DLLs and the ICAD executable
//
class DB_CLASS IcadSharedGlobals
	{
	public:
		static void SetIcadAppInstance( HINSTANCE hInstance );
		static HINSTANCE GetIcadAppInstance( void );
		static void SetIcadResourceInstance( HINSTANCE hInstance );
		static HINSTANCE GetIcadResourceInstance( void );
		static void SetHatchFactory (BHatchBuilderFactory *hatchFact);
		static BHatchBuilderFactory* GetHatchFactory();
		// EBATECH(watanabe)-[update leader according to modifying annotation
		static void SetLeaderFactory (CUpdateLeaderFactory*);
		static CUpdateLeaderFactory* GetLeaderFactory();
		// ]-EBATECH(watanabe)
		static void SetCmdMessage(int (*pfunct)(const char *,...));
		static void CallCmdMessage(const char *);  // message printed on command line
		static void SetGetVar(int (*pfunct)(short, struct resbuf*));
		static int CallGetVar(short qidx, struct resbuf* rb); // calls SDS_GetVarSimple()
		static void SetPreviewBMFunc(int (*pfunct)(const char*, void**, unsigned long*, short*));
		static int CallGetPreviewBM(const char* filename, void** phbitmap, unsigned long* pAllocatedBytes, short* bIsBitmap);
		static void SetMainHWND(HWND hWnd);
		static HWND GetMainHWND();

	private:
		static HINSTANCE zm_hAppInstance;

		static HWND zm_hMainWindow;

		static HINSTANCE zm_hResourceInstance;
		
		// Bugzilla::78404;			Date: 03-02-04
		// To remove this field since it is only used in "Icad\IcadColorDia.cpp"
		// and that too temporarily...
		//!!! static bool zm_bDialogIsModal;
		// Bugzilla::78404;			Date: 03-02-04
		
		static BHatchBuilderFactory *m_hatchFactory;
		// EBATECH(watanabe)-[update leader according to modifying annotation
		static CUpdateLeaderFactory *m_leaderFactory;
		// ]-EBATECH(watanabe)
		static int (*m_pMessageFunct) (const char*,...);
		static int (*m_pGetVarFunct) (short, struct resbuf*);
		static int (*m_pPreviewBMFunct) (const char*, void**, unsigned long*, short*);

	// ***********************
	// Trick to make sure this class is never instanced
	//
	private:
		IcadSharedGlobals();
		~IcadSharedGlobals();
	};


// helper functions

DB_API short LoadXref(db_drawing *p_topdp,db_blocktabrec *bthip,char *btxrefpn,int recurlevel);
DB_API short UpdateTopLevelTables(db_drawing *topdp, db_drawing *xrefdp,db_blocktabrec *xrefblkptr);

/*-------------------------------------------------------------------------*//**
	Function set db_handitem::m_parentBlock = bthip for every db_entity in xref 
	and for every not Xref block table records 
@param bthip => block table record which keeps xref
@return RTNORM - siccess or RTERROR if bthip is wrong
*//*--------------------------------------------------------------------------*/
DB_API short setParentBlockInXref(db_blocktabrec* bthip);

inline int __cdecl
db_compareHandleStrings(const char * dst, const char * src)
{
        int f,l;

            do {
                if ( ((f = (unsigned char)(*(dst++))) >= 'A'/*DNT*/) && (f <= 'Z'/*DNT*/) )
                    f -= ('A'/*DNT*/ - 'a'/*DNT*/);

                if ( ((l = (unsigned char)(*(src++))) >= 'A'/*DNT*/) && (l <= 'Z'/*DNT*/) )
                    l -= ('A'/*DNT*/ - 'a'/*DNT*/);
            } while ( f && (f == l) );

        return(f - l);
}

inline bool equalHandles( db_handitem *pHandItem1, db_handitem *pHandItem2 )
	{
	db_objhandle pHand1,pHand2;

	ASSERT( pHandItem1 != NULL );
	ASSERT( pHandItem2 != NULL );

	if (pHandItem1->RetHandle()==pHandItem2->RetHandle())
		return true;
	return false;
	}

// EBATECH(CNBR) -[ 2002/7/10 Bugzilla#78218 Add new DIMASSOC object
#define DB_NUMSTOCKCLASSES 25
//#define DB_NUMSTOCKCLASSES 24

struct stockclassstru
	{
	int classnum;
	char *dxfrecname;
	char *cppname;
	char *appname;
	int vernum,wasproxy,isent;
	} ;

extern struct stockclassstru stockclass[DB_NUMSTOCKCLASSES];

//Added Cybage AJK 29/10/2001 [
//Reason--Fix for bug no 77865 from Bugzilla
bool validateEnt(struct resbuf *elist,int hitype=0);
//Added Cybage AJK 29/10/2001 DD/MM/YYYY ]
#endif // !defined( _DB_H )



