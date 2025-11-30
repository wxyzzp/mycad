// ***************************************************
// SDS_APIS.CPP
// Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
// 
// Abstract
//
// Provides entry points for external threads to the engine.
//
//
#include "Icad.h" /*DNT*/
#include "sds.h" /*DNT*/
#include "sds_engine.h" /*DNT*/


long SDS_MANAGE_STATE::ziEntryCount=0;


// ------------------------------
// This API is very special and should call directly to 
// here for now because it throws an exception
// to cancel out of an ADS application
//
// This method SHOULD NEVER be called from the engine!!! Only from an external app.
//
//
void
sds_abort(const char *szAbortMsg)
	{
	SDSAPI_Entry();

    sds_printf("%s"/*DNT*/,szAbortMsg);	
    sds_exit(RTNORM);
    return;
	}

// -----------------------------
// This API is obsolete and does nothing
//
void		       
sds_abortintellicad(void)
	{
	SDSAPI_Entry();
	// NOP
	}


// -------------------------------
// This method is completely thread safe and
// can call directly to the engine
//
int				   
sds_agetcfg(const char *szSymbol, char  *szVariable)
	{
	SDSAPI_Entry();
	return sdsengine_agetcfg( szSymbol, szVariable );
	}


// ------------------------------------
// This method is completely thread safe and
// can call directly to the engine
//
int				   
sds_agetenv(const char *szSymbol, char  *szVariable)
	{
	SDSAPI_Entry();
	return sdsengine_agetenv( szSymbol, szVariable );
	}

// -----------------------------------
// This API is completely thread safe and
// can be called from any thread
//
// SPECIAL !!!
// Because of multiple dependencies, there is no sdsengine_ version
// of this function
//
// int	sds_alert(const char *szAlertMsg)



// -------------------------------
// This method is completely thread-safe and can be
// called from anywhere
//
double             
sds_angle(const sds_point ptStart, const sds_point ptEnd)
	{
	SDSAPI_Entry();
	return sdsengine_angle( ptStart, ptEnd );
	}



// ------------------------------
// This API SEEMS to be directly callable from outside the engine,
// though it does depend on global variables (read-only)
//
// TODO -- decide if this needs to be engine-only callable
//
int                
sds_angtof(const char *szAngle, int nUnitType, double *pdAngle)
	{
	SDSAPI_Entry();
	return sdsengine_angtof( szAngle, nUnitType, pdAngle );
	}


// --------------------------------
// All these angtos APIs SEEM to be callable from outside
// the engine.  They depend of SDS_CURDWG and other read-only globals,
// so they're not completely thread-safe
//
// TODO -- Change this to engine-only APIs???
//
int                
sds_angtos(double dAngle, int nUnitType, int nPrecision, char *szAngle)
	{
	SDSAPI_Entry();
	return sdsengine_angtos( dAngle, nUnitType, nPrecision, szAngle );
	}

int                
sds_angtos_end(double dAngle, int nUnitType, int nPrecision, char *szAngle)
	{
	SDSAPI_Entry();
	return sdsengine_angtos_end( dAngle, nUnitType, nPrecision, szAngle );
	}

int                
sds_angtos_convert(int ignoremode, double dAngle, int nUnitType, int nPrecision, char *szAngle)
	{
	SDSAPI_Entry();
	return sdsengine_angtos_convert( ignoremode, dAngle, nUnitType, nPrecision, szAngle );
	}

// EBATECH(CNBR) -[ Bugzilla#78443 DIMAZIN 2003/2/6
int                
sds_angtos_dim(int ignoremode, double dAngle, int nUnitType, int nPrecision, char *szAngle, int nDimazin, int nDimdsep )
//sds_angtos_dim(int ignoremode, double dAngle, int nUnitType, int nPrecision, char *szAngle)
	{
	SDSAPI_Entry();
	return sdsengine_angtos_dim( ignoremode, dAngle, nUnitType, nPrecision, szAngle, nDimazin, nDimdsep );
	}
// EBATECH(CNBR) ]-

int				   
sds_angtos_absolute(double dAngle, int nUnitType, int nPrecision, char *szAngle)
	{
	SDSAPI_Entry();
	return sdsengine_angtos_absolute( dAngle, nUnitType, nPrecision, szAngle );
	}


// -------------------------------------------
// !!! ENGINE CALL !!!
// 
// 
int				   
sds_arxload(const char *szARXProgram)
	{
	SDSAPI_Entry();
	// TODO!! Make engine call

	return sdsengine_arxload( szARXProgram );
	}

// -------------------------------------------
// !!! ENGINE CALL !!!
// 
// 
struct sds_resbuf *
sds_arxloaded(void)
	{
	SDSAPI_Entry();
	// TODO!! Make engine call

	return sdsengine_arxloaded();
	}

// -------------------------------------------
// !!! ENGINE CALL !!!
// 
// 
int				   
sds_arxunload(const char *szARXProgram)
	{
	SDSAPI_Entry();
	// TODO!! Make engine call

	return sdsengine_arxunload( szARXProgram );
	}


// ---------------------------------
// This API is completely thread-safe
//
int                
sds_asetcfg(const char *szSymbol, const char *szValue)
	{
	SDSAPI_Entry();
	return sdsengine_asetcfg( szSymbol, szValue );
	}


// ---------------------------------
// This API is completely thread-safe
//
int				   
sds_asetenv(const char *szSymbol, const char *szValue)
	{
	SDSAPI_Entry();
	return sdsengine_asetenv( szSymbol, szValue );
	}


// -----------------------------------------
// sds_buildlist !!!!
// This method is completely thread-safe and because
// of the complexities of varargs, there is not sdsengine_
// version
//
// struct sds_resbuf *sds_buildlist(int nRType, ...);


// -------------------------------------------
// This API SHOULD be completely thread-safe and callable from anywhere
//
// NOTE -- 9/14/98 Not thread safe now, but must be made so
// TODO -- see above
//
int 
sds_callinmainthread(int (*fnDragEnts)(void *),void *pUserData)
	{
	SDSAPI_Entry();
	return sdsengine_callinmainthread( fnDragEnts, pUserData );
	}


// ----------------------------------------------
// This method is completely thread-safe.  
// This method SHOULD NOT be called from the within IntelliCAD
// either the engine or the UI thread
//
// there is no sdsengine_ version of this
//
// NOTE -- in fact, developers should be discouraged from using this at all
//
void*
sds_calloc(size_t sizeHowMany, size_t sizeBytesEach)
	{
	SDSAPI_Entry();
    return calloc(sizeHowMany,sizeBytesEach);
	}

// ----------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_cmd(const struct sds_resbuf *prbCmdList)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_cmd( prbCmdList );
	}

// ------------------------------------------------
// This API calls
// sds_cmd() rather than sdsengine_cmd() so that it can be called
// from outside the engine
//
// !!!! Because of varargs, there is no sdsengine_ version of this
// function
//
// int sds_command(int nRType, ...);




// -------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_cvunit(double dOldNum, const char *szOldUnit, const char *szNewUnit, double *pdNewNum)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_cvunit( dOldNum, szOldUnit, szNewUnit, pdNewNum );
	}

// -------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_defun(const char *szFuncName, int nFuncCode)
	{
	SDSAPI_Entry();
	// TODO -- make engine call
	
	return sdsengine_defun( szFuncName, nFuncCode );
	}

// -------------------------------------------
// !!! ENGINE CALL !!!
//
int				   
sds_dictadd(const sds_name nmDict, const char *szAddThis, const sds_name nmNonGraph)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_dictadd( nmDict, szAddThis, nmNonGraph );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int				   
sds_dictdel(const sds_name nmDict, const char *szDelThis)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_dictdel( nmDict, szDelThis );
	}


// ------------------------------------------
// !!! ENGINE CALL !!!
//
struct sds_resbuf *
sds_dictnext(const sds_name nmDict, int swFirst)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_dictnext( nmDict, swFirst );
	}


// ------------------------------------------
// !!! ENGINE CALL !!!
//
int				   
sds_dictrename(const sds_name nmDict, const char *szOldName, const char *szNewName)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_dictrename( nmDict, szOldName, szNewName );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
struct sds_resbuf *
sds_dictsearch(const sds_name nmDict, const char *szFindThis, int swFirst)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_dictsearch( nmDict, szFindThis, swFirst );
	}


// ------------------------------------
// This API is completely thread and engine safe
//
double			   
sds_distance(const sds_point ptFrom, const sds_point ptTo)
	{
	SDSAPI_Entry();

	return sdsengine_distance( ptFrom, ptTo );
	}


// ------------------------------------------
// !!! ENGINE CALL !!!
//
struct sds_dobjll *
sds_dispobjs(const sds_name nmEntity,int nDispMode)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_dispobjs( nmEntity, nDispMode );
	}

// -----------------------------------
// This API seems to be safe to call from outside the engine
//
int                
sds_distof(const char *szDistance, int nUnitType, double *pdDistance)
	{
	SDSAPI_Entry();

	return sdsengine_distof( szDistance, nUnitType, pdDistance );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_draggen(const sds_name nmSelSet, const char *szDragMsg, int nCursor, int (*fnDragEnts)(sds_point ptCursorLoc, sds_matrix mxTransform), sds_point ptDestPoint )
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_draggen( nmSelSet, szDragMsg, nCursor, fnDragEnts, ptDestPoint );
	}


// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_entdel(const sds_name nmEntity)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_entdel( nmEntity );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
struct sds_resbuf *
sds_entget(const sds_name nmEntity)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_entget( nmEntity );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
struct sds_resbuf *
sds_entgetx(const sds_name nmEntity, const struct sds_resbuf *prbAppList)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_entgetx( nmEntity, prbAppList );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_entlast(sds_name nmLastEnt)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_entlast( nmLastEnt );
	}


// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_entmake(const struct sds_resbuf *prbEntList)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_entmake( prbEntList );
	}


// ------------------------------------------
// !!! ENGINE CALL !!!
//
int				   
sds_entmakex(const struct sds_resbuf *prbEntList, sds_name nmNewEnt)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_entmakex( prbEntList, nmNewEnt );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_entmod(const struct sds_resbuf *prbEntList)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_entmod( prbEntList );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
// This API is NOT thread safe and SHOULD NOT be called from outside the engine
//
int                
sds_entnext(const sds_name nmKnownEnt, sds_name nmNextEnt)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_entnext_withxref( nmKnownEnt, nmNextEnt );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_entsel(const char *szSelectMsg, sds_name nmEntity, sds_point ptSelected)
	{
	SDSAPI_Entry();

	return sdsengine_entsel( szSelectMsg, nmEntity, ptSelected );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_entupd(const sds_name nmEntity)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_entupd( nmEntity );
	}

// ------------------------------
// This API is very special and should call directly to 
// here for now because it throws an exception
// to cancel out of an ADS application
//
// This method SHOULD NOT be called from the engine!!! Only from an external app.
// 
// There is no sdsengine_ version
//
// void               sds_exit(int swAbnormalExit);


// -------------------------------------
// This API is thread and engine safe
// 
//
void               
sds_fail(const char *szFailMsg)
	{
	SDSAPI_Entry();

	sdsengine_fail( szFailMsg );
	}


// -------------------------------------
// This API is engine safe and can be called from any place
// 
//
// SPECIAL !!!
// Because of multiple dependencies, there is no sdsengine_ version
// of this function
//
// int sds_findfile(const char *szLookFor, char *szPathFound)


// ----------------------------------------------
// This method is completely thread-safe.  
// This method SHOULD NOT be called from the within IntelliCAD
// either the engine or the UI thread
//
// there is no sdsengine_ version of this
//
// NOTE -- in fact, developers should be discouraged from using this at all
//
void sds_free(void *pMemLoc) 
	{	
	SDSAPI_Entry();

    free(pMemLoc);
	}

// -------------------------------------
// This method is thread and engine safe
// NOTE -- it may not always be
//
void			   
sds_freedispobjs(struct sds_dobjll *pDispObjs)
	{
	SDSAPI_Entry();

	sdsengine_freedispobjs( pDispObjs );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_getangle(const sds_point ptStart, const char *szAngleMsg, double *pdRadians)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_getangle( ptStart, szAngleMsg, pdRadians );
	}

// ************************************************
// TODOTHREAD -- this will have to change enormously
//
// SPECIAL
//
// DO NOT CALL FROM ENGINE
//
//char* sds_getappname(void)

// -------------------------------------
//	!!! ENGINE CALL ???
//
// TODOTHREAD -- how do we handle this?
//
struct sds_resbuf *
sds_getargs(void)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_getargs();
	}


// -------------------------------------
// This API is thread and engine safe
// 
//
int				   
sds_getcfg(const char *szSymbol, char  *szVariable, int nLength)
	{
	SDSAPI_Entry();

	return sdsengine_getcfg( szSymbol, szVariable, nLength );
	}  



// -------------------------------------
// This API is thread and engine safe
// 
// TODOTHREAD -- is this always going to be safe?
//
int                
sds_getcname(const char *szOtherLang, char **pszEnglish)
	{
	SDSAPI_Entry();

	return sdsengine_getcname( szOtherLang, pszEnglish );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_getcorner(const sds_point ptStart, const char *szCornerMsg, sds_point ptOpposite)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	//EBATECH(FUTA) 2001-8-9 fix can't to use OSNAP
	//return sdsengine_getcorner( ptStart, szCornerMsg, ptOpposite );
	return sdsengine_getcorner( ptStart, szCornerMsg, ptOpposite , true);
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_getdist(const sds_point ptStart, const char *szDistMsg, double *pdDistance)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_getdist( ptStart, szDistMsg, pdDistance );
	}


// -------------------------------------
// This API is engine safe and can be called from anywhere
// 
struct sds_resbuf *
sds_getdoclist(void)
	{
	SDSAPI_Entry();

	return sdsengine_getdoclist();
	}


// -------------------------------------
// This API is engine safe and can be called from anywhere
// 
// TODOTHREAD -- does this need to be an engine call??
//
int                
sds_getfiled(const char *szTitle, const char *szDefaultPath, const char *szExtension, int bsOptions, struct sds_resbuf *prbFileName)
	{
	SDSAPI_Entry();

	return sdsengine_getfiled( szTitle, szDefaultPath, szExtension, bsOptions, prbFileName );
	}

//----------------------------------------
// SPECIAL
//
// TODOTHREAD -- this could be made thread local or something
// so that it doesn't use global
//
// THIS SHOULD NEVER BE CALLED BY THE ENGINE
//
int                
sds_getfuncode(void)
	{
	SDSAPI_Entry();
// TODOTHREAD -- turn on this assert
// ASSERT( !OnEngineThread() );

	return sdsengine_getfuncode();
	}


//----------------------------------------
// SPECIAL
//
// TODOTHREAD -- this could be made thread local or something
// so that it doesn't use globals
//
// engine call????
//
int                
sds_getinput(char *szEntry)
	{
	SDSAPI_Entry();

	return sdsengine_getinput( szEntry );
	}


// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_getint(const char *szIntMsg, int *pnInteger)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_getint( szIntMsg, pnInteger );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_getkword(const char *szKWordMsg, char *szKWord)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_getkword( szKWordMsg, szKWord );
	}



// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_getorient(const sds_point ptStart, const char *szOrientMsg, double *pdRadians)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_getorient( ptStart, szOrientMsg, pdRadians );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_getpoint(const sds_point ptReference, LPCTSTR szPointMsg, sds_point ptPoint)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_getpoint( ptReference, szPointMsg, ptPoint );
	}


// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_getreal(const char *szRealMsg, double *pdReal)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_getreal( szRealMsg, pdReal );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_getstring(int swSpaces, const char *szStringMsg, char *szString)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_getstring( swSpaces, szStringMsg, szString );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_getsym(const char *szSymbol, struct sds_resbuf **pprbSymbolInfo)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_getsym( szSymbol, pprbSymbolInfo );
	}



// -------------------------------------
// This API is engine safe and can be called from anywhere
// 
int                
sds_gettbpos(const char *pToolBarName, sds_point ptTbPos)
	{
	SDSAPI_Entry();

	return sdsengine_gettbpos( pToolBarName, ptTbPos );
	}


// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_getvar(const char *szSysVar, struct sds_resbuf *prbVarInfo)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_getvar( szSysVar, prbVarInfo );
	}


// -------------------------------------
// This API is engine safe and can be called from anywhere
// 
int                
sds_graphscr(void)
	{
	SDSAPI_Entry();

	return sdsengine_graphscr();
	}


// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_grclear(void)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_grclear();
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int		    	   
sds_grarc(const sds_point ptCenter, double dRadius, double dStartAngle, double dEndAngle, int nColor, int swHighlight)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_grarc( ptCenter, dRadius, dStartAngle, dEndAngle, nColor, swHighlight );
	}


// ------------------------------------------
// !!! ENGINE CALL !!!
//
int		    	   
sds_grfill(const sds_point *pptPoints, int nNumPoints, int nColor, int swHighlight)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_grfill( pptPoints, nNumPoints, nColor, swHighlight );
	}


// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_grdraw(const sds_point ptFrom, const sds_point ptTo, int nColor, int swHighlight)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_grdraw( ptFrom, ptTo, nColor, swHighlight );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_grread(int bsAllowed, int *pnInputType, struct sds_resbuf *prbInputValue)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_grread( bsAllowed, pnInputType, prbInputValue );
	}



// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_grtext(int nWhere, const char *szTextMsg, int swHighlight)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_grtext( nWhere, szTextMsg, swHighlight );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_grvecs(const struct sds_resbuf *prbVectList, sds_matrix mxDispTrans)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_grvecs( prbVectList, mxDispTrans );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_handent(const char *szEntHandle, sds_name nmEntity)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	db_objhandle newhan;
	newhan=szEntHandle;
	return sdsengine_handent( newhan, nmEntity );
	}

// -------------------------------------
// This API is thread and engine safe
// 
int                
sds_help(char *szHelpFile, char *szContextID, int nMapNumber)
	{
	SDSAPI_Entry();

	return sdsengine_help( szHelpFile, szContextID, nMapNumber );
	}

// *****************************************
// This is an SDS External API
//
// SPECIAL -- thread and engine safe
// 
// TODOTHREAD -- ???  Initialize thread state???
//
// THIS SHOULD NEVER BE CALLED FROM THE ENGINE
//
// There is no sdsengine_ version of this
//
int sds_init(int nARGC, char *nARGV[]) 
	{
	SDSAPI_Entry();

    // Need to do our init code,
    // Currently there is no init code needed!

    return(RTNORM);
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_initget(int bsAllowed, LPCTSTR szKeyWordList)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_initget( bsAllowed, szKeyWordList );
	}


// -------------------------------------
// This API is thread and engine safe
// 
int                
sds_inters(const sds_point ptFrom1, const sds_point ptTo1, const sds_point ptFrom2, const sds_point ptTo2, int swFinite, sds_point ptIntersection)
	{
	SDSAPI_Entry();

	return sdsengine_inters( ptFrom1, ptTo1, ptFrom2, ptTo2, swFinite, ptIntersection );
	}

// ---------------------------------
// !!! ENGINE CALL ???
//
// TODOTHREAD -- how do we handle this?
//
int                
sds_invoke( const struct sds_resbuf *prbArguments, struct sds_resbuf **pprbReturn )
	{
	SDSAPI_Entry();

	// TODO -- make engine call
	return sdsengine_invoke( prbArguments, pprbReturn );
	}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// SPECIAL 
//
// These APIs are so simple (just direct calls to C runtime) no
// need to have engine versions
//
// int sds_isalnum(int nASCIIValue)
// int sds_isalpha(int nASCIIValue);
// int sds_iscntrl(int nASCIIValue);
// int sds_isdigit(int nASCIIValue);
// int sds_isgraph(int nASCIIValue);
// int sds_islower(int nASCIIValue);
// int sds_isprint(int nASCIIValue);
// int sds_ispunct(int nASCIIValue);
// int sds_isspace(int nASCIIValue);
// int sds_isupper(int nASCIIValue);
// int sds_isxdigit(int nASCIIValue);

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Very SPECIAL
//
// THIS SHOULD NEVER BE CALLED BY THE ENGINE
//
// There is no sdsengine_ version of this
//
// int sds_link(int nRSMsg)
//-------------------------------------

// ------------------------------------------
// !!! ENGINE CALL !!!
//
struct sds_resbuf *
sds_loaded(void)
	{
	SDSAPI_Entry();
	// TODO -- make engine call

	return sdsengine_loaded();
	}



// ----------------------------------------------
// This is an SDS External API
//
// This method is completely thread-safe.  
// This method SHOULD NOT be called from the within IntelliCAD
// by either the engine or the UI thread
//
// there is no sdsengine_ version of this
//
// NOTE -- in fact, developers should be discouraged from using this at all
//
void *
sds_malloc(size_t sizeBytes) 
	{						
	SDSAPI_Entry();

    return(malloc(sizeBytes));
	}


// -------------------------------------
// This API is engine safe and can be called from anywhere
//
int                
sds_menucmd(const char *szPartToDisplay)
	{
	SDSAPI_Entry();

	return sdsengine_menucmd( szPartToDisplay );
	}


// -------------------------------------
// This API is engine safe and can be called from anywhere
//
int                
sds_menugroup(char *pMenuGroupName)
	{
	SDSAPI_Entry();

	return sdsengine_menugroup( pMenuGroupName );
	}


// ----------------------------------------------
// This is an SDS External API
//
// This method is completely thread-safe.  
// This method SHOULD NOT be called from the within IntelliCAD
// by either the engine or the UI thread
//
// there is no sdsengine_ version of this
//
// NOTE -- in fact, developers should be discouraged from using this at all
//
int sds_msize(void *pvBuffer) 
	{    
	SDSAPI_Entry();
	return(_msize(pvBuffer));
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int	               
sds_namedobjdict(sds_name nmDict)
	{
	SDSAPI_Entry();

	return sdsengine_namedobjdict( nmDict );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_nentsel(const char *szNEntMsg, sds_name nmEntity, sds_point ptEntPoint, sds_point ptECStoWCS[4], struct sds_resbuf **pprbNestBlkList)
	{
	SDSAPI_Entry();

	return sdsengine_nentsel( szNEntMsg, nmEntity, ptEntPoint, ptECStoWCS, pprbNestBlkList );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_nentselp(const char *szNEntMsg, sds_name nmEntity, sds_point ptEntPoint, int swUserPick, sds_matrix mxECStoWCS, struct sds_resbuf **pprbNestBlkList)
	{
	SDSAPI_Entry();

	return sdsengine_nentselp( szNEntMsg, nmEntity, ptEntPoint, swUserPick, mxECStoWCS, pprbNestBlkList );
	}



// ---------------------------------
// This API is completely thread-safe
//
// Because of dependencies, this API has no sdsengine_ version
//
// struct sds_resbuf *sds_newrb(int nTypeOrDXF)
// --------------------------------------------------


// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_osnap(const sds_point ptAperCtr, const char *szSnapModes, sds_point ptPoint)
	{
	SDSAPI_Entry();

	return sdsengine_osnap( ptAperCtr, szSnapModes, ptPoint );
	}


// ---------------------------------
// This API is completely thread-safe
//
void               
sds_polar(const sds_point ptPolarCtr, double dAngle, double dDistance, sds_point ptPoint)
	{
	SDSAPI_Entry();

	sdsengine_polar( ptPolarCtr, dAngle, dDistance, ptPoint );
	}


// ************************************************
// Because of varargs there is no sdsengine_ version
// of this.
//
// SPECIAL
//
// TODOTHREAD -- figure out if this is anywhere near thread safe
//
// int sds_printf(const char *szPrintThis, ...)
// ---------------------------------------------


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// SPECIAL
// 
// Not clear whether these are thread-safe or not
//
int                
sds_progresspercent(int iPercentDone)
	{
	SDSAPI_Entry();

	return sdsengine_progresspercent( iPercentDone );
	}

int				   
sds_progressstart(void)
	{
	SDSAPI_Entry();

	return sdsengine_progressstart();
	}

int                
sds_progressstop(void)
	{
	SDSAPI_Entry();

	return sdsengine_progressstop();
	}

// end of progress APIs
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!




// ---------------------------------
// This API is engine safe
//
int                
sds_prompt(const char *szPromptMsg)
	{
	SDSAPI_Entry();

	return sdsengine_prompt( szPromptMsg );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_putsym(const char *szSymbol, struct sds_resbuf *prbSymbolInfo)
	{
	SDSAPI_Entry();

	return sdsengine_putsym( szSymbol, prbSymbolInfo );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int				   
sds_readaliasfile(char *szAliasFile)
	{
	SDSAPI_Entry();

	return sdsengine_readaliasfile( szAliasFile );
	}


// ----------------------------------------------
// This is an SDS External API
//
// This method is completely thread-safe.  
// This method SHOULD NOT be called from the within IntelliCAD
// by either the engine or the UI thread
//
// there is no sdsengine_ version of this
//
// NOTE -- in fact, developers should be discouraged from using this at all
//
void *
sds_realloc(void *pOldMemLoc, size_t sizeBytes) 
	{	
	SDSAPI_Entry();

    return(realloc(pOldMemLoc,sizeBytes));
	}


// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_redraw(const sds_name nmEntity, int nHowToDraw)
	{
	SDSAPI_Entry();

	return sdsengine_redraw( nmEntity, nHowToDraw );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_regapp(const char *szApplication)
	{
	SDSAPI_Entry();

	return sdsengine_regapp( szApplication );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_regappx(const char *szApplication, int swSaveAsR12)
	{
	SDSAPI_Entry();

	return sdsengine_regappx( szApplication, swSaveAsR12 );
	}


// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_regfunc( int (*nFuncName)(void), int nFuncCode )
	{
	SDSAPI_Entry();

	return sdsengine_regfunc( nFuncName, nFuncCode );
	}

// ---------------------------------
// This API is completely thread-safe
//
// Because of dependencies, this API has no sdsengine_ version
//
// int sds_relrb(struct sds_resbuf *prbReleaseThis)
//------------------------------------------------


// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_retint(int nReturnInt)
	{
	SDSAPI_Entry();

	return sdsengine_retint( nReturnInt );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_retlist(const struct sds_resbuf *prbReturnList)
	{
	SDSAPI_Entry();

	return sdsengine_retlist( prbReturnList );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_retname(const sds_name nmReturnName, int nReturnType)
	{
	SDSAPI_Entry();

	return sdsengine_retname( nmReturnName, nReturnType );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_retnil(void)
	{
	SDSAPI_Entry();

	return sdsengine_retnil();
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_retpoint(const sds_point ptReturn3D)
	{
	SDSAPI_Entry();

	return sdsengine_retpoint( ptReturn3D );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_retreal(double dReturnReal)
	{
	SDSAPI_Entry();

	return sdsengine_retreal( dReturnReal );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_retstr(const char *szReturnString)
	{
	SDSAPI_Entry();

	return sdsengine_retstr( szReturnString );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_rett(void)
	{
	SDSAPI_Entry();

	return sdsengine_rett();
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_retval(const struct sds_resbuf *prbReturnValue)
	{
	SDSAPI_Entry();

	return sdsengine_retval( prbReturnValue );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_retvoid(void)
	{
	SDSAPI_Entry();

	return sdsengine_retvoid();
	}



// ---------------------------------
// This API is engine safe
//
int				   
sds_rp2pix(double dNumberX,double dNumberY,int *pPixelX,int *pPixelY)
	{
	SDSAPI_Entry();

	return sdsengine_rp2pix( dNumberX, dNumberY, pPixelX, pPixelY );
	}

// ---------------------------------
// This API is engine safe
//
int                
sds_rtos(double dNumber, int nUnitType, int nPrecision, char *szNumber)
	{
	SDSAPI_Entry();
	//EBATECH(CNBR) Bugzilla#78443 change value to read $DIMZIN
	return sdsengine_rtos( dNumber, nUnitType, nPrecision, szNumber, -1/*0*/ );
	}

// ---------------------------------
// This API is engine safe and callable from anywhere
//
int                
sds_sendmessage(char *szCommandMsg)
	{
	SDSAPI_Entry();

	return sdsengine_sendmessage( szCommandMsg );
	}

// ---------------------------------
// This is IntelliCAD specific (no AutoCAD version)
//
// TODOTHREAD -- this has to change dramatically
// 
int                
sds_setcallbackfunc( int (*cbfnptr)(int flag,void *arg1,void *arg2,void *arg3) )
	{
	SDSAPI_Entry();

	return sdsengine_setcallbackfunc( cbfnptr );
	}

// ---------------------------------
// This API is completely thread-safe
//
// NOTE -- currently this function doesn't do anything, so it may not
// always be thread safe
//
int                
sds_setfunhelp(char *szFunctionName, char *szHelpFile, char *szContextID, int nMapNumber)
	{
	SDSAPI_Entry();

	return sdsengine_setfunhelp( szFunctionName, szHelpFile, szContextID, nMapNumber );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_setvar(const char *szSysVar, const struct sds_resbuf *prbVarInfo)
	{
	SDSAPI_Entry();

	return sdsengine_setvar( szSysVar, prbVarInfo );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_setview(const struct sds_resbuf *prbViews, int nWhichVPort)
	{
	SDSAPI_Entry();

	return sdsengine_setview( prbViews, nWhichVPort );
	}

// ------------------------------------------
// !!! ENGINE CALL ???
//
int                
sds_snvalid(const char *szTableName, int swAllowPipe)
	{
	SDSAPI_Entry();

	return sdsengine_snvalid( szTableName, swAllowPipe );
	}

// ---------------------------------
// This API is engine safe
//
int                
sds_ssadd(const sds_name nmEntToAdd, const sds_name nmSelSet, sds_name nmNewSet)
	{
	SDSAPI_Entry();

	return sdsengine_ssadd( nmEntToAdd, nmSelSet, nmNewSet );
	}

// ---------------------------------
// This API is engine safe
//
int                
sds_ssdel(const sds_name nmEntToDel, const sds_name nmSelSet)
	{
	SDSAPI_Entry();

	return sdsengine_ssdel( nmEntToDel, nmSelSet );
	}

// ---------------------------------
// This API is fully thread safe
//
int                
sds_ssfree(sds_name nmSetToFree)
	{
	SDSAPI_Entry();

	return sdsengine_ssfree( nmSetToFree );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_ssget(const char *szSelMethod, const void *pFirstPoint, const void *pSecondPoint, 
		  const struct sds_resbuf *prbFilter, sds_name nmNewSet)
	{
	SDSAPI_Entry();

	return sdsengine_ssget( szSelMethod, pFirstPoint, pSecondPoint, prbFilter, 
							nmNewSet );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_ssgetfirst(struct sds_resbuf **pprbHaveGrips, struct sds_resbuf **pprbAreSelected)
	{
	SDSAPI_Entry();

	return sdsengine_ssgetfirst( pprbHaveGrips, pprbAreSelected );
	}

// ---------------------------------
// This API is fully thread safe
//
int                
sds_sslength(const sds_name nmSelSet, long *plNumberOfEnts)
	{
	SDSAPI_Entry();

	return sdsengine_sslength( nmSelSet, plNumberOfEnts );
	}

// ---------------------------------
// This API is fully thread safe
//
int                
sds_ssmemb(const sds_name nmEntity, const sds_name nmSelSet)
	{
	SDSAPI_Entry();

	return sdsengine_ssmemb( nmEntity, nmSelSet );
	}

// ---------------------------------
// This API is fully thread safe
//
int                
sds_ssname(const sds_name nmSelSet, long lSetIndex, sds_name nmEntity)
	{
	SDSAPI_Entry();

	return sdsengine_ssname( nmSelSet, lSetIndex, nmEntity );
	}

// ---------------------------------
// This API is fully thread safe
//
int                
sds_ssnamex(struct sds_resbuf **pprbEntName, const sds_name nmSelSet, const long iIndex)
	{
	SDSAPI_Entry();

	return sdsengine_ssnamex( pprbEntName, nmSelSet, iIndex );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_sssetfirst(const sds_name nmGiveGrips, const sds_name nmSelectThese)
	{
	SDSAPI_Entry();

	return sdsengine_sssetfirst( nmGiveGrips, nmSelectThese );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_swapscreen(void)
	{
	SDSAPI_Entry();

	return sdsengine_swapscreen();
	}


// -----------------------------------------
// This API is thread-safe because it is currently unimplemented
//
// TODO -- is this always going to be thread/engine safe
//
int                
sds_tablet (const struct sds_resbuf *prbGetOrSet, struct sds_resbuf **pprbCalibration)
	{
	SDSAPI_Entry();

	return sdsengine_tablet( prbGetOrSet, pprbCalibration );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
struct sds_resbuf *
sds_tblnext(const char *szTable, int swFirst)
	{
	SDSAPI_Entry();

	return sdsengine_tblnext( szTable, swFirst );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int	               
sds_tblobjname(const char *szTable, const char *szEntInTable, sds_name nmEntName)
	{
	SDSAPI_Entry();

	return sdsengine_tblobjname( szTable, szEntInTable, nmEntName );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
struct sds_resbuf *
sds_tblsearch(const char *szTable, const char *szFindThis, int swNextItem)
	{
	SDSAPI_Entry();

	return sdsengine_tblsearch( szTable, szFindThis, swNextItem );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_textbox(const struct sds_resbuf *prbTextEnt, sds_point ptCorner, sds_point ptOpposite)
	{
	SDSAPI_Entry();

	return sdsengine_textbox( prbTextEnt, ptCorner, ptOpposite );
	}

// -------------------------------------
// This API is engine safe and can be called from anywhere
// 
int                
sds_textpage(void)
	{
	SDSAPI_Entry();

	return sdsengine_textpage();
	}

// -------------------------------------
// This API is engine safe and can be called from anywhere
// 
int                
sds_textscr(void)
	{
	SDSAPI_Entry();

	return sdsengine_textscr();
	}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// SPECIAL 
//
// These APIs are so simple (just direct calls to C runtime) no
// need to have sdsengine_ versions
//
// int sds_tolower(int nASCIIValue);
// int sds_toupper(int nASCIIValue);
// --------------------------------------------



// -------------------------------------
// This API is engine safe and can be called from anywhere
//
// TODOTHREAD -- is this REALLY engine safe???
//
int                
sds_trans(const sds_point ptVectOrPtFrom, const struct sds_resbuf *prbCoordFrom, 
		  const struct sds_resbuf *prbCoordTo, int swVectOrDisp, sds_point ptVectOrPtTo)
	{
	SDSAPI_Entry();

	return sdsengine_trans( ptVectOrPtFrom, prbCoordFrom,
							prbCoordTo, swVectOrDisp, ptVectOrPtTo );
	}

// -------------------------------------
// This API is engine safe and can be called from anywhere
//
int				   
sds_ucs2rp(sds_point ptSour3D,sds_point ptDest3D)
	{
	SDSAPI_Entry();

	return sdsengine_ucs2rp( ptSour3D, ptDest3D );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_update(int nWhichVPort,const sds_point ptCorner1,const sds_point ptCorner2)
	{
	SDSAPI_Entry();

	return sdsengine_update( nWhichVPort, ptCorner1, ptCorner2 );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_undef(const char *szFuncName, int nFuncCode)
	{
	SDSAPI_Entry();

	return sdsengine_undef( szFuncName, nFuncCode );
	}


// -------------------------------------
// This API is engine safe and can be called from anywhere
//
// TODOTHREAD -- is it really engine safe?
//
int                
sds_usrbrk(void)
	{
	SDSAPI_Entry();

	return sdsengine_usrbrk();
	}


// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_vports(struct sds_resbuf **prbViewSpecs)
	{
	SDSAPI_Entry();

	return sdsengine_vports( prbViewSpecs );
	}

// -------------------------------------------
// This API is completely thread-safe.
// Because it is compiled into multiple modules, there 
// is no sdsengine_ version
//
// int sds_wcmatch(const char *szCompareThis, const char *szToThis)
//-------------------------------------------------


// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_xdroom(const sds_name nmEntity, long *plMemAvail)
	{
	SDSAPI_Entry();

	return sdsengine_xdroom( nmEntity, plMemAvail );
	}

// -------------------------------------
// This API is engine safe and can be called from anywhere
//
// It would be thread-safe if it didn't bring up an error dialog
//
int                
sds_xdsize(const struct sds_resbuf *prbEntData, long *plMemUsed)
	{
	SDSAPI_Entry();

	return sdsengine_xdsize( prbEntData, plMemUsed );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_xformss(const sds_name nmSetName, sds_matrix mxTransform)
	{
	SDSAPI_Entry();

	return sdsengine_xformss( nmSetName, mxTransform );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_xload(const char *szApplication)
	{
	SDSAPI_Entry();

	return sdsengine_xload( szApplication );
	}

// -----------------------------------------
// This API is completely thread-safe
//
// We should discourage developers from using this
//
int                
sds_xstrcase(char *szString)
	{
	SDSAPI_Entry();

	return sdsengine_xstrcase( szString );
	}

// -----------------------------------------
// This API is completely thread-safe
//
// We should discourage developers from using this
//
char *
sds_xstrsave(char *szSource, char **pszDest)
	{
	SDSAPI_Entry();

	return sdsengine_xstrsave( szSource, pszDest );
	}

// ------------------------------------------
// !!! ENGINE CALL !!!
//
int                
sds_xunload(const char *szApplication)
	{
	SDSAPI_Entry();

	return sdsengine_xunload( szApplication );
	}



// -----------------------------------------
// This API is completely thread-safe
//
// IntelliCAD only API
// 
// 
HDC *
sds_getviewhdc(void)
	{
	SDSAPI_Entry();

	return sdsengine_getviewhdc();
	}

// -----------------------------------------
// This API is completely thread-safe
//
// IntelliCAD only API
// 
// 
int 
sds_getrgbvalue(int nColor)
	{
	SDSAPI_Entry();

	return sdsengine_getrgbvalue( nColor );
	}

// -----------------------------------------
// This API is completely thread-safe
//
// IntelliCAD only API
// 
// 
HPALETTE 
sds_getpalette(void)
	{
	SDSAPI_Entry();

	return sdsengine_getpalette();
	}

// -----------------------------------------
// This API is completely thread-safe
//
// IntelliCAD only API
// 
// 
HWND 
sds_getviewhwnd(void)
	{
	SDSAPI_Entry();

	return sdsengine_getviewhwnd();
	}

// -----------------------------------------
// This API is completely thread-safe
//
// IntelliCAD only API
// 
// 
HWND sds_getmainhwnd(void)
	{
	SDSAPI_Entry();

	return sdsengine_getmainhwnd();
	}

int sds_getpreviewbmp(const char *szFileName, void** pHBITMAP, unsigned long* pAllocatedBytes, short* bIsBitmap)
{
	SDSAPI_Entry();

	return sdsengine_getpreviewbmp(szFileName, pHBITMAP, pAllocatedBytes, bIsBitmap);
}

int sds_bmpout(const char* pFile, int width, int height)
{
	SDSAPI_Entry();
	return sdsengine_bmpout(pFile, width, height);
}
