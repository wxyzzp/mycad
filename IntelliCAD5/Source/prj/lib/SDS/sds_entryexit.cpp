/*	LIB\SDS\SDS_ENTRYEXIT.CPP 
 *	Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 *	Abstract
 *
 *  Entry Exit Functions 
 *  Lisp Entry/Exit 
 *
 */

#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "sds_engine.h" /*DNT*/
#include "sdsthreadexception.h" /*DNT*/
#include "sdsthread.h" /*DNT*/
#include "sdsapplication.h" /*DNT*/
#include "commandatom.h" /*DNT*/
#include "lisp.h"/*DNT*/
#include "cmds.h"/*DNT*/

extern SDSApplication *SDS_LoadedAppsLast;
extern int       SDS_FunctionID;
extern char      SDS_AppName[IC_ACADBUF];
extern HINSTANCE SDS_appinstance;
extern int       SDS_userbreak;

bool SDS_InsideXloadFunc;
bool SDS_AbortXload;

/****************************************************************************/
/*********************** Entry Exit Functions *******************************/
/****************************************************************************/

int sds_link(int nRSMsg) {
	SDSAPI_Entry();


	SDSApplication *pThisApp = SDSApplication::GetCurrentApplication();
	ASSERT( pThisApp != NULL );
	ASSERT( pThisApp->GetMainThread()->IsCurrent() );

	// runtime catch
	//
	if ( pThisApp == NULL )
		{
		pThisApp = SDSApplication::GetActiveApplication();
		if ( pThisApp == NULL )
			{
			return RTERROR;
			}
		}

	pThisApp->SetRQCode( 0 ); 
    if(pThisApp->IsAppLoaded()==0) 
		{ 
		pThisApp->MarkAppAsLoaded(); 
		return(SDS_RQXLOAD); 
		}

	if ( pThisApp->AppIsUnloading() )
		{
		ASSERT( pThisApp->GetMainThread()->IsCurrent() );
		if ( !pThisApp->GetMainThread()->IsCurrent() )
			{
			throw SDSThreadException( SDSThreadException::APPEXIT );
			}
		}

	SDSApplication *ptempApp2=SDS_GetLastThread();

	// Check to see if this was called from invoke()
	if(ptempApp2 == NULL) 
		{
		ptempApp2=SDS_LoadedAppsLast;
		}

	// Swap, we go away and somebody else runs, afterwards all variables may have changed
	//
	// ****************************************
	// THREADSWAP !!!!!!!!!!!!!!!!!
		{
		SDSApplication *pStartApp = ptempApp2;
		SDSApplication *pStopApp = pThisApp;

		SDS_ThreadSwap( pStartApp->GetMainThread(), pStopApp->GetMainThread() );
		}

	pThisApp = SDSApplication::GetCurrentApplication();
	ASSERT( pThisApp != NULL );
	ASSERT( pThisApp->GetMainThread()->IsCurrent() );



	if( pThisApp->GetRQCode() ) 
		{
		return( pThisApp->GetRQCode() );
		}

	// Otherwise, must be calling a function
	//
	return SDS_RQSUBR;
}

// *****************************************
// This is an SDS External API
//
// TODOTHREAD -- this is probably going to have to change substantially
//
int 
sdsengine_usrbrk(void) 
	{

	if(SDS_userbreak) 
		{
		--SDS_userbreak;
		return(SDS_userbreak+1);
		} 
	else 
		{
		return(SDS_userbreak);
		}
	}

/****************************************************************************/
/************************* Lisp Entry/Exit **********************************/
/****************************************************************************/

// *****************************************
// This is an SDS External API
//
// ------------------------------
// This API is very special and should call directly to 
// here for now because it throws an exception
// to cancel out of an ADS application
//
// This method SHOULD NEVER be called from the engine!!! Only from an external app.
//
//
void sds_exit(int swAbnormalExit) 
	{
	SDSAPI_Entry();

// TODOTHREAD -- turn this assert on
//	ASSERT( !OnEngineThread() );

	// TODO -- is this still right???
	//
	if(SDS_InsideXloadFunc) 
		{
		SDS_AbortXload=TRUE;
		return;
		}


#if !defined( NDEBUG )
	// Save state of globals before we unload the app
	//
    SDSApplication *pThisThread;

	// This is mainly just a sanity check.  Also useful for debugging
	//
	pThisThread = SDSApplication::GetActiveApplication();
	ASSERT( pThisThread->GetMainThread()->IsCurrent() );
#endif

	SDSApplication *ptempApp2=SDS_GetLastThread();
	
	if ( ptempApp2 == NULL )
		{
		ptempApp2 = SDS_LoadedAppsLast;
		}
	if ( ptempApp2 != NULL )
		{
		ptempApp2->GetMainThread()->Resume();
		}


	throw SDSThreadException( SDSThreadException::APPEXIT );

	// Shouldn't be able to get here because we threw an exception
	//
	ASSERT( false );

    return;
	}


// *****************************************
// This is an SDS External API
//
void sdsengine_fail(const char *szFailMsg) 
	{
    sds_printf("%s"/*DNT*/,szFailMsg);
    return;
	}

// *****************************************
// This is an SDS External API
//
struct resbuf *
sdsengine_loaded(void) 
	{

	ASSERT( OnEngineThread() );

	SDSApplication *ptempApp = NULL;
    struct resbuf *brb=NULL,*crb=NULL,*trb;
    
    if(SDSApplication::GetEngineApplication()==NULL) 
		{
		return(NULL);
		}

    for(ptempApp=SDSApplication::GetEngineApplication()->GetNextLink(); ptempApp!=NULL; ptempApp=ptempApp->GetNextLink()) 
		{
        trb=sds_buildlist(RTSTR,ptempApp->GetAppName(),0);
        if(brb) 
			{
			crb=crb->rbnext=trb;
			}
        else
			{
			brb=crb=trb;
			}
	    }
    return(brb);
	}

int
sds_xunload_application( SDSApplication *pThisApp )
	{
	struct resbuf rb;
	SDSApplication *pLastApp;
    char fname[IC_ACADBUF],fs1[IC_ACADBUF],*fcp1;

	ASSERT( OnEngineThread() || OnUIThread() );

	DWORD dwThreadID;
	if ( pThisApp->GetMainThread()->IsValid() )
		{
		dwThreadID = pThisApp->GetMainThread()->GetThreadID();
		}
	else
		{
		dwThreadID = ::GetCurrentThreadId();
		}

	if(pThisApp==NULL) 
		{
		rb.restype=RTSHORT;
		rb.resval.rint=OL_ENOTLOADED;
		SDS_setvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
        return(SDS_RTERROR);
	    }	

	// This basically means we've been re-entered
	//
	if ( pThisApp->AppIsUnloading() )
		{
		return (SDS_RTNORM);
		}
	pThisApp->SetAppIsUnloading();

	strcpy( fname, pThisApp->GetAppName() );

	// If we are the current thread, we obviously don't need to notify ourselves
	//
	if ( ( pThisApp->GetMainThread() != NULL ) &&
		 !pThisApp->GetMainThread()->IsCurrent() &&
		 !::OnUIThread() )
		{
		// Notify the app that it is being unloaded.
		pLastApp=SDS_LoadedAppsLast;
		SDS_LoadedAppsLast=SDSApplication::GetActiveApplication();
		pThisApp->SetRQCode( SDS_RQXUNLD );
	
		// ****************************************
		// THREADSWAP !!!!!!!!!!!!!!!!!
			{
			SDSApplication *pStartApp = pThisApp;
			SDSApplication *pStopApp = SDS_LoadedAppsLast;

			SDS_ThreadSwap( pStartApp->GetMainThread(),pStopApp->GetMainThread());
			}
	
		pThisApp->SetRQCode( 0 );
		SDS_LoadedAppsLast=pLastApp;
		}



	// Undefine all of the commands for the app we are unloading.
    class commandAtomObj  *tap1;
	
    while((tap1=lsp_findatom((const TCHAR *)&dwThreadID,3))!=NULL) 
		{
		lsp_undefatom(tap1->LName,0);
		}


	SDSApplication::RemoveLink( pThisApp );



	

    if((fcp1=strrchr(fname,'.'/*DNT*/))==NULL ||
        (fcp1!=NULL && strchr(fcp1,'\\'/*DNT*/)!=NULL)) 
		{
		strcat(fname,".dll"/*DNT*/);
		}

    strcpy(fs1,fname);
    //*** sds_findfile has a bug when a pathname is included with
    //*** filename and it strcat()s' the pathname onto the path of the
    //*** current directory.
    if(strchr(fs1,'\\'/*DNT*/)==NULL && strchr(fs1,':'/*DNT*/)==NULL) 
		{
        if(sds_findfile(fs1,fname)!=SDS_RTNORM) 
			{
			cmd_ErrorPrompt(CMD_ERR_FINDUNLOAD,0,fs1);		
            return(SDS_RTERROR);
	        }
	    }

	for(fcp1=fname; *fcp1!=0; fcp1++) 
		{
		if(*fcp1=='/'/*DNT*/) 
			{
			*fcp1='\\'/*DNT*/;
			}
		//EBATECH(R.Arayashiki) MBCS String [
		else if ((_MBC_LEAD ==_mbbtype((unsigned char)*fcp1,0)) &&
			  (_MBC_TRAIL==_mbbtype((unsigned char)*(fcp1+1),1)))
			{
			fcp1 ++;
			}
		//EBATECH ]
		}

    HANDLE hInstLib;
    if((hInstLib=GetModuleHandle(fname))==NULL) 
		{
		DWORD ec;
        ec=GetLastError();
		cmd_ErrorPrompt(CMD_ERR_UNLOADING,0,fname,(void *)ec);		
		rb.restype=RTSHORT;
		rb.resval.rint=OL_EREFUSE;
		SDS_setvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
        return(SDS_RTERROR);
	    }


	pThisApp->TerminateCommandThread();
	if ( !pThisApp->GetMainThread()->IsCurrent() )
		{
		pThisApp->TerminateMainThread();
		}


	delete pThisApp;
	pThisApp = NULL;

	// Just free the library
    FreeLibrary((HINSTANCE)hInstLib);

    return(SDS_RTNORM);
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_xunload(const char *szApplication) 
	{

	ASSERT( OnEngineThread() );

    char fname[IC_ACADBUF];
    SDSApplication *ptempApp;

    strcpy(fname, szApplication);
    _strlwr(fname);

	ChangeSlashToBackslashAndRemovePipe(fname);

    // To unload a .dll without having to enter it's whole path.
    // We will lower case the test name. Then copy off each member
    // of SDSApplication, which we will lower case, then use strstr
    // to locate the right .dll to unload.
    // Once we have found it, copy it's fully qualified path/name back
    // into fname and continue as if the user had type it in fully 
    // qualified.
    CString tmpAppName;
    for(ptempApp=SDSApplication::GetEngineApplication(); ptempApp !=NULL; ptempApp=ptempApp->GetNextLink()) 
		{
        tmpAppName = ptempApp->GetAppName();
        tmpAppName.MakeLower(); // To strstr with fname (already lower)
        if(strstr(tmpAppName,fname)!= NULL) 
			{
            // Now copy fully qualified file name back into fname.
            strcpy(fname,ptempApp->GetAppName());
            tmpAppName.Empty( );
            break;
	        }
        tmpAppName.Empty( );
		}
	if ( ptempApp == NULL )
		{
		return RTERROR;
		}
	ASSERT( ptempApp->GetMainThread()->IsValid() );


	return sds_xunload_application( ptempApp );
  	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_xload(const char *szApplication) 
	{

	ASSERT( OnEngineThread() );

    char fname[IC_ACADBUF],fs1[IC_ACADBUF],*fcp1;
    HINSTANCE hInstLib;
    FARPROC EntryFunc;
    DWORD ec;
    SDSApplication *ptempApp=NULL,*pLastApp;
	struct resbuf rb;

	SDS_InsideXloadFunc=TRUE;

    strcpy(fname,szApplication);

	ChangeSlashToBackslashAndRemovePipe(fname);

    if((fcp1=strrchr(fname,'.'/*DNT*/))==NULL ||
        (fcp1!=NULL && strchr(fcp1,'\\'/*DNT*/)!=NULL)) strcat(fname,SDS_SDSFILEEXT);

    strcpy(fs1,fname);
    //*** sds_findfile has a bug when a pathname is included with
    //*** filename and it strcat()s' the pathname onto the path of the
    //*** current directory.
    if(strchr(fs1,'\\'/*DNT*/)==NULL && strchr(fs1,':'/*DNT*/)==NULL) {
        if(sds_findfile(fs1,fname)!=SDS_RTNORM) {
			cmd_ErrorPrompt(CMD_ERR_CANTFINDAPP,0,fs1);		
			rb.restype=RTSHORT;
			rb.resval.rint=OL_EOPEN;
			SDS_setvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
			SDS_InsideXloadFunc=FALSE;
            return(SDS_RTERROR);
        }
    }

    if(GetModuleHandle(fname)!=NULL) {
//      Let lisp print these messages. Don't print them here.
//		char fs2[IC_ACADBUF];
//      sprintf(fs2,"Application \"%s.\" is already loaded.",fname);
//      sds_printf(fs2);
        rb.restype=RTSHORT;
        rb.resval.rint=OL_ELOADED;
		SDS_setvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		SDS_InsideXloadFunc=FALSE;
        return(SDS_RTERROR);
    }


	// Load a DLL.
	if((SDS_appinstance=hInstLib=LoadLibrary(fname))==NULL) {

		ec=GetLastError();
		switch(ec) {
			case ERROR_BAD_ENVIRONMENT:
			case ERROR_BAD_FORMAT:
			case ERROR_INVALID_ACCESS:
			case ERROR_INVALID_DATA:
			case ERROR_INVALID_DRIVE:
			case ERROR_WRITE_PROTECT:
			case ERROR_BAD_UNIT:
				cmd_ErrorPrompt(CMD_ERR_INVALIDDLL,0,fname);		
				break;
			case 0:
				cmd_ErrorPrompt(CMD_ERR_SYSNOMEM,0,fname);		
				break;
			case ERROR_ACCESS_DENIED:
				cmd_ErrorPrompt(CMD_ERR_ACCESS_DENIED,0,fname);		
				break;
			case ERROR_NOT_ENOUGH_MEMORY:
			case ERROR_OUTOFMEMORY:
				cmd_ErrorPrompt(CMD_ERR_OUTOFMEMORY,0,fname);		
				break;
			case ERROR_NOT_READY:
				cmd_ErrorPrompt(CMD_ERR_NOT_READY,0,fname);		
				break;
			case ERROR_PROC_NOT_FOUND:
				cmd_ErrorPrompt(CMD_ERR_PROC_NOTFOUND,0,fname);		
				break;
			case ERROR_DLL_NOT_FOUND:
			case ERROR_MOD_NOT_FOUND:
				cmd_ErrorPrompt(CMD_ERR_DLLNOTFOUND,0,fname);		
				break;
			default:
				cmd_ErrorPrompt(CMD_ERR_DLLLOADPROB,0,fname,(void *)ec);		
				break;
		}
        rb.restype=RTSHORT;
        rb.resval.rint=OL_EEXEC;
		SDS_setvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		SDS_InsideXloadFunc=FALSE;
		return(SDS_RTERROR);
	}

	GetModuleFileName(GetModuleHandle(fname),SDS_AppName,IC_ACADBUF-1);

	if((EntryFunc=GetProcAddress(hInstLib,"SDS_EntryPoint"/*DNT*/))==NULL) {
		cmd_ErrorPrompt(CMD_ERR_BADENTPOINT,0,fname);		
		FreeLibrary(hInstLib); 
		SDS_InsideXloadFunc=FALSE;
		return(SDS_RTERROR);
	}

	ASSERT( CHECKFUNCPTR( EntryFunc ) );
	if ( !CHECKFUNCPTR( EntryFunc ) )
		{
		cmd_ErrorPrompt(CMD_ERR_BADENTPOINT,0,fname);		
		FreeLibrary(hInstLib); 
		SDS_InsideXloadFunc=FALSE;
		return(SDS_RTERROR);
		}

    if ((ptempApp=new SDSApplication( fname ))==NULL) 
		{
		SDS_InsideXloadFunc=FALSE;
        return(SDS_RTERROR);
	    }

	ASSERT( SDSApplication::GetEngineApplication()->IsValid() );
	SDSApplication::GetEngineApplication()->AddLink( ptempApp );


	// ********************************
    // Create a command pipeline for this application.
	//
    ptempApp->StartXAppCommandThread();

	// ********************************
    // Start the application thread.
	//
    ptempApp->StartXAppMainThread( (LPTHREAD_START_ROUTINE)EntryFunc );




    // Find the current running thread.
    DWORD CurThread=GetCurrentThreadId();
    for(SDSApplication *pcurApp=SDSApplication::GetEngineApplication(); pcurApp!=NULL; pcurApp=pcurApp->GetNextLink()) 
		{
        if ( pcurApp->GetMainThread()->IsSame( CurThread ) ) 
			break;
	    }
    if(pcurApp==NULL) 
		{
		SDS_InsideXloadFunc=FALSE;
		return(SDS_RTERROR);
		}

	pLastApp=SDS_LoadedAppsLast;
	SDS_LoadedAppsLast=pcurApp;

	// ****************************************
	// THREADSWAP !!!!!!!!!!!!!!!!!
		{
		SDSApplication *pStartApp = ptempApp;
		SDSApplication *pStopApp = pcurApp;

		SDS_ThreadSwap( pStartApp->GetMainThread(),pStopApp->GetMainThread());
		}

	SDS_LoadedAppsLast=pLastApp;
	
	SDS_InsideXloadFunc=FALSE;
	
	if(SDS_AbortXload) 
		{
		SDS_AbortXload=FALSE;
		sdsengine_xunload(szApplication);
		}

    return(SDS_RTNORM);
	}






// *****************************************
// This is an SDS External API
//
int 
sdsengine_defun(const char *szFuncName, int nFuncCode) 
	{	

	ASSERT( OnEngineThread() );

    return(lsp_defun2(szFuncName, nFuncCode));
	}

// *****************************************
// This is an SDS External API
//
// TODOTHREAD -- remove dependency on global
// TODOTHREAD -- this could be made thread local or something
// so that it doesn't use global
//
// THIS SHOULD NEVER BE CALLED BY THE ENGINE
//
int 
sdsengine_getfuncode(void) 
	{

    return(SDS_FunctionID);
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_getsym(const char *szSymbol, struct sds_resbuf **pprbSymbolInfo) 
	{
	ASSERT( OnEngineThread() );

/*
**  Looks up szSymbol in the LISP atoms list and converts it from LISP
**  format to ADS format via lsp_convassoc() or lsp_demote().  Points
**  *pprbSymbolInfo at the allocated result.  (It's up to the caller
**  to free it.)
**
**  Returns:
**
**      RTNORM or RTERROR, but if you check rc in 'out' you can tell why:
**
**       0 : OK
**      -1 : Can't find it in the atoms list.
**      -2 : No memory.
*/
    int demote,rc,fi1;
    struct resbuf *res,*trbp1;
    class commandAtomObj  *tap1;


    rc=0; 
	res=NULL;

    if ((tap1=lsp_findatom((char *)szSymbol,0))==NULL) 
		{ 
        // This function returns RTNORM in AutoCAD if a symbol is nil.
        // Go figure. I'm trying to add a bit of safeguard by setting the
        // thing the pointer points to to NULL. Hopefully this will keep a 
        // developer from thinking he has something in the buffer based
        // on the return value alone.
        rc=0; 
        *pprbSymbolInfo = NULL;
        goto out; 
	    }

    demote=1;

    /* See if it looks like an elist: */
    for (trbp1=&tap1->rb,fi1=0; trbp1!=NULL && fi1<4; trbp1=trbp1->rbnext,fi1++) 
		{
        if (fi1<2) 
			{
            if (trbp1->restype!=RTLB) 
				{
				break;
				}
			} 
		else if (fi1<3) 
			{
            if ( trbp1->restype!=RTSHORT && 
				 trbp1->restype!=RTLONG ) 
				{
				break;
				}
			} 
		else if (fi1<4) 
			{
            if (trbp1->restype!=RTDOTE) 
				{
				break;
				}
	        }
		}


    if (fi1>3) 
		{  /* An elist */
        if ((fi1=lsp_convassoc(&tap1->rb,&res,0))==-2) 
			{ 
			rc=-2; 
			goto out; 
			}

        if (!fi1) 
			{
			demote=0;
			}
	    }


    if (demote) 
		{  
		/* NOT an elist.  Just demote it. */
        if (res!=NULL) 
			{ 
			sds_relrb(res); 
			res=NULL; 
			}


        /* Make first link for lsp_copysplrb(): */
        if ((res=db_alloc_resbuf())==NULL)
            { 
			rc=-2; 
			goto out; 
			}


        res->restype=RTNIL;
        lsp_copysplrb(res,&tap1->rb); 
		lsp_demote(&res,1);
	    }

out:
    if (rc && res!=NULL) 
		{ 
		sds_relrb(res); 
		res=NULL; 
		}

    if (pprbSymbolInfo!=NULL) 
		{ 
		*pprbSymbolInfo=res; 
		res=NULL; 
		}

    if (res!=NULL) 
		{ 
		sds_relrb(res); 
		res=NULL; 
		}

    return (rc) ? RTERROR : RTNORM;

	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_putsym(const char *szSymbol, struct sds_resbuf *prbSymbolInfo) 
	{

	ASSERT( OnEngineThread() );

    if(lsp_defatom(szSymbol,0,prbSymbolInfo,1)) 
		{ 
		return(RTERROR); 
		}

    return(RTNORM);
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_regfunc(int (*nFuncName)(void), int nFuncCode) 
	{	

	ASSERT( OnEngineThread() );

    return(lsp_regfunc(nFuncName, nFuncCode));
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_undef(const char *szFuncName, int nFuncCode) 
	{
	ASSERT( OnEngineThread() );


    return(lsp_undef(szFuncName, nFuncCode));
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_retint(int nReturnInt) 
	{

	ASSERT( OnEngineThread() );

    return(lsp_retint(nReturnInt));
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_retlist(const struct sds_resbuf *prbReturnList) 
	{

	ASSERT( OnEngineThread() );

	int ret=RTERROR;
	if(prbReturnList==NULL) 
		{
		return(RTERROR);
		}


   	struct resbuf *trb=NULL;
	if(lsp_convassoc((struct resbuf *)prbReturnList,&trb,1)) 
		{
		ret=lsp_retlist(prbReturnList);
		return(ret);
		}

    ret=lsp_retlist(trb);
	sds_relrb(trb);

	return(ret);
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_retname(const sds_name nmReturnName, int nReturnType) 
	{
	ASSERT( OnEngineThread() );

    return(lsp_retname((long *)nmReturnName,nReturnType));
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_retnil(void) 
	{
	ASSERT( OnEngineThread() );

    return(lsp_retnil());
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_retpoint(const sds_point ptReturn3D) 
	{
	ASSERT( OnEngineThread() );

    return(lsp_retpoint((double *)ptReturn3D));
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_retreal(double dReturnReal) 
	{
	ASSERT( OnEngineThread() );

    return(lsp_retreal(dReturnReal));
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_retstr(const char *szReturnString) 
	{
	ASSERT( OnEngineThread() );

    return(lsp_retstr(szReturnString));
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_rett(void) 
	{
	ASSERT( OnEngineThread() );

    return(lsp_rett());
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_retval(const struct sds_resbuf *prbReturnValue) 
	{
	ASSERT( OnEngineThread() );

    return(lsp_retval(prbReturnValue));
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_retvoid(void) 
	{
	ASSERT( OnEngineThread() );

    return(lsp_retvoid());
	}

