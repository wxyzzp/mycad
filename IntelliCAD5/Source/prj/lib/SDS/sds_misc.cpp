/* D:\ICADDEV\PRJ\LIB\SDS\SDS_MISC.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

//** Includes
#include "Icad.h"/*DNT*/
#include "sds_engine.h" /*DNT*/
#include "viewport.h"
#include "IcadView.h"
#include "Preferences.h"
#include "IcadHelp.h"
//#include "sdsapplication.h"/*DNT*/
//#include "threadcontroller.h"/*DNT*/
#include "lisp.h"/*DNT*/
#include "cmds.h"/*DNT*/
//#include <malloc.h>
#include "commandatom.h" /*DNT*/ 

//** Variables	 
//extern struct resbuf *lsp_argsll;  /* A "SUPER" RESBUF LLIST -- SEE NOTE ABOVE */
//extern struct lsp_pendlink lsp_pend;
//extern int    SDS_FunctionID;
//extern HWND   SDS_hwnd;
//extern bool	  SDS_SetAUserCallback;
//extern db_handitem *SDS_LastHandItem;	
extern struct resbuf *SDS_UserDocList;	

/****************************************************************************/
/************************* New RX Functions *********************************/
/****************************************************************************/

// *****************************************
// This is an SDS External API
//
int sdsengine_agetenv(const char *szSymbol, char  *szVariable) {

	if(strisame("ACAD"/*DNT*/,szSymbol) || strisame("ICAD"/*DNT*/,szSymbol)) {
		struct resbuf rb;
		if(RTNORM==sds_getvar("SRCHPATH"/*DNT*/,&rb)) {
			strcpy(szVariable,rb.resval.rstring);
			IC_FREE(rb.resval.rstring);
			return(RTNORM);
		}
	}
	if(NULL==(szVariable=getenv(szSymbol))) return(RTERROR);
    return(RTNORM);
}

// *****************************************
// This is an SDS External API
//
int sdsengine_asetenv(const char *szSymbol, const char *szValue) { 
	char fs1[IC_ACADBUF];
	sprintf(fs1,"%s=%s"/*DNT*/,szSymbol,szValue);
	if(_putenv(szSymbol)) return(RTERROR);
    return(RTNORM);
}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_getcfg(const char *szSymbol, char  *szVariable,int nLength) 
	{
	CIntelliCadPrefKey	userKey(HKEY_CURRENT_USER, "User", FALSE);
	HKEY hKey = userKey;
	DWORD type;
	ULONG fl2=nLength;

	if(hKey == NULL) 
		{
		return(RTERROR);
		}
	if(ERROR_SUCCESS!=RegQueryValueEx(hKey,szSymbol,NULL,
		&type,(unsigned char *)szVariable,&fl2)) 
		{
		return(RTERROR);
		}
    return(RTNORM);
	}

// *****************************************
// This is an SDS External API
//
int sdsengine_agetcfg(const char *szSymbol, char  *szVariable) 
	{
	CIntelliCadPrefKey userKey(HKEY_CURRENT_USER, "User", FALSE);
	HKEY hKey = userKey;
	DWORD type;
	ULONG fl2=IC_ACADBUF;

	if(hKey == NULL) 
		{
		return(RTERROR);
		}
	if(ERROR_SUCCESS!=RegQueryValueEx(hKey,szSymbol,NULL,
		&type,(unsigned char *)szVariable,&fl2)) 
		{
		return(RTERROR);
		}
    return(RTNORM);
	}

// *****************************************
// This is an SDS External API
//
int sdsengine_asetcfg(const char *szSymbol, const char *szValue) {
	CIntelliCadPrefKey userKey(HKEY_CURRENT_USER, "User");
	HKEY hKey;
	DWORD retval;

	EditString	registryClass;
	registryClass = ""/*DNT*/;
	if(ERROR_SUCCESS!=RegCreateKeyEx(userKey, "",
					0,registryClass,REG_OPTION_NON_VOLATILE,
					KEY_ALL_ACCESS,NULL,&hKey,&retval)) {
		return(RTERROR);
	} 

	/*D.G.*/// Use REG_SZ instead of REG_DWORD.
	RegSetValueEx(hKey,szSymbol,0, REG_SZ /*REG_DWORD*/ ,(const unsigned char *)szValue,strlen(szValue)+1);
	
	RegCloseKey(hKey);
	return(RTNORM);
}

// *****************************************
// This is an SDS External API
//
int sdsengine_arxload(const char *szARXProgram) 
	{    

	ASSERT( OnEngineThread() );

    return(sdsengine_xload(szARXProgram));
	}

// *****************************************
// This is an SDS External API
//
struct sds_resbuf *
sdsengine_arxloaded(void) 
	{
    return(sdsengine_loaded());
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_arxunload(const char *szARXProgram) 
	{    
    return(sds_xunload(szARXProgram));
	}

//	Replace \ with \\ for string escapes passed to WinHelp
//	Cloned from ArchT

static void
ktiEscapeSlashDelimStr(const char* in_str, int token, char* out_str)
	{
	int i, j;

	i = j = 0;
	while (in_str[i] != '\0') {
		if ((in_str[i] == token) || (in_str[i] == '\\')) {
			out_str[j++] = '\\';
			out_str[j++] = in_str[i++];
		}
		else
			out_str[j++] = in_str[i++];
	}
	out_str[j] = '\0';
	}


// *****************************************
// This is an SDS External API
//
int 
sdsengine_help(char *szHelpFile, char *szContextID, int nMapNumber)
	{
	if(szHelpFile==NULL || *szHelpFile==0) 
		return(RTERROR);

	char fs1[IC_ACADBUF];
	if( sds_findfile(szHelpFile,fs1) != RTNORM ) 
		return(RTERROR);



									// map to Windows help call
	int	ret;
	if ( !szContextID )				// if bogus context, just bring up contents
		ret = CMainWindow::IcadHelp( fs1,ICADHELP_CONTENTS,0);
	else if ( nMapNumber )			// map directly to Windows arguments
		ret = CMainWindow::IcadHelp( fs1, nMapNumber, (DWORD)szContextID);

									// Use Help macro to Jump to topic ID
									// "JumpID( "<filepath>", "<topicID")"
									// (slashes in path must be escaped, \\)
	else
		{
			char ext[IC_ACADNMLEN];
			ic_getext((char*)szHelpFile,ext);

			if(strisame(ext,".hlp")) // call for .hlp file
			{
		CString	command( "JumpID(\"");
		LPTSTR	doubleSlash = new char[ _tcslen( fs1) * 2];

		ktiEscapeSlashDelimStr( fs1, '\\', doubleSlash);
		command += doubleSlash;
		delete doubleSlash;

		command += "\",\"";
		command += szContextID;
		command += "\")";
				ret = CMainWindow::IcadHelp( fs1, HELP_COMMAND, (DWORD)(LPCTSTR)command);
			}
			else
				ret = CMainWindow::IcadHelp( fs1, ICADHELP_KEY, (DWORD)szContextID);
		}

    if(ret==0) 
		return(RTERROR);

	return(RTNORM);
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_setfunhelp(char *szFunctionName, char *szHelpFile, char *szContextID, int nMapNumber) 
{
	
	// Modified the Function By Sachin Dange as suggested By Denis 
	//  Bugzilla 78663 .............[
	if (!_tcsnicmp(szFunctionName, "C:"/*DNT*/, 2) == 0)
	{
		cmd_ErrorPrompt(IDC_LISP4_ERRMSG_10, -1);
		sds_printf(": \"%s\"", szFunctionName);
		return RTERROR;
	}

	class commandAtomObj * pCommandAtomObj = NULL;
	pCommandAtomObj = lsp_findatom(szFunctionName,4);
	if(pCommandAtomObj)
	{
		if(szHelpFile)
		{
			pCommandAtomObj->m_szHelpFile = new char[strlen(szHelpFile) + 1];
			strcpy(pCommandAtomObj->m_szHelpFile,szHelpFile);
		}
		if(szContextID)
		{
			pCommandAtomObj->m_szContextID = new char[strlen(szContextID) + 1];
			strcpy(pCommandAtomObj->m_szContextID,szContextID);
		}
		pCommandAtomObj->m_nMapNumber = nMapNumber;
	}

	// .................... ] Modifications end here 
	return(RTNORM);
		
}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_snvalid(const char *szTableName, int swAllowPipe) 
	{

	ASSERT( OnEngineThread() );

		//Modified SAU 17/05/00     [
		//Reason:(snvalid) function not working properly
		int ilen=strlen(szTableName);

		if(ilen>31) 
			return(OL_ESNVALID);

		if(db_str2hitype((char *)szTableName)==-1)
		{
			for(int i=0;szTableName[i]!='\0';i++ )
			{
				if(!isalnum(szTableName[i]) && (szTableName[i]!='$')
					&& (szTableName[i]!='_') && (szTableName[i]!='-'))

				if(swAllowPipe && (szTableName[i]=='|') && (szTableName[0]!='|') && (szTableName[ilen-1]!='|') )
					continue;
				else
					return(OL_ESNVALID);
			}
		}
		//Modified SAU 17/05/00     ] 

    return(RTNORM);
	}




// *****************************************
// This is an SDS External API
//
// WHY ???
//
char *
sdsengine_xstrsave(char *szSource, char **pszDest) 
	{
	db_astrncpy(pszDest,szSource,strlen(szSource)+1);
	return(*pszDest);
	}

// *****************************************
// This is an SDS External API
//
// WHY ???
//
int 
sdsengine_xstrcase(char *szString) 
	{
    _tcsupr(szString);
    return(RTNORM);
	}

// *****************************************
// This is an SDS External API
//
// 
int 
sdsengine_readaliasfile(char *szAliasFile) 
	{

	// this may be okay to call from outside the engine, particularly
	// from the UI thread
	//
	ASSERT( OnEngineThread() );

	return(SDS_ReadAlias(szAliasFile));
	}

// *****************************************
// This is an SDS External API
//
// TODOTHREAD -- eliminate global
//
struct sds_resbuf *
sdsengine_getdoclist(void) 
	{
	SDS_UserDocList=NULL;
	ExecuteUIAction( ICAD_WNDACTION_GETDOCLIST );
	return(SDS_UserDocList);
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_getrgbvalue(int nColor) 
	{
	if( nColor<1 || nColor>256 ) 
		{
		return(RGB(0,0,0));
		}

	return SDS_GetPaletteColorsRGB( nColor );
	}

// *****************************************
// This is an SDS External API
//
HPALETTE 
sdsengine_getpalette(void) 
	{
	return((HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle());
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_ucs2rp(sds_point sour,sds_point dest) 
	{
	if(!gr_ucs2rp(sour,dest,SDS_CURGRVW)) 
		{
		return(RTNORM);
		}

	return(RTERROR);
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_rp2pix(sds_real p_rx,sds_real p_ry,int *p_ixp,int *p_iyp) 
	{
	if( p_ixp==NULL || 
		p_iyp==NULL ) 
		{
		return(RTERROR);
		}

	gr_rp2pix(SDS_CURGRVW,p_rx,p_ry,p_ixp,p_iyp);

	return(RTNORM);
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_menugroup(char *pMenuGroupName) 
	{
	if ( NULL==pMenuGroupName || 
		*pMenuGroupName==0 ) 
		{
		return(RTERROR);
		}

	SDS_CMainWindow->m_strWndAction=pMenuGroupName;
	ExecuteUIAction( ICAD_WNDACTION_MENUGROUP );

	if(!SDS_CMainWindow->m_bWndAction) 
		{
		return(RTERROR);
		}

	return(RTNORM);
	}


// *****************************************
// This is an SDS External API
//
// -------------------------------------------
// This API SHOULD be completely thread-safe and callable from anywhere
//
// NOTE -- 9/14/98 Not thread safe now, but must be made so
// TODO -- see above
//
int sdsengine_callinmainthread(int (*fnUserFunc)(void *),void *pUserData) 
	{
	if(fnUserFunc==NULL) 
		{
		return(RTERROR);
		}

	struct UserMainThreadArgs args;

	args.SDS_UserMainThreadFunc=fnUserFunc;
	args.SDS_UserMainThreadFuncData=pUserData;

	ExecuteUIAction( ICAD_WNDACTION_USERCALL, (LPARAM)&args);

	return args.SDS_UserMainThreadFuncRet;
	}

CIcadView *SDS_pViewToInvalidate;
CRect *SDS_RectToInvalidate;

// *****************************************
// This is an SDS External API
//
int 
sdsengine_update(int nWhichVPort,const sds_point ptCorner1,const sds_point ptCorner2) 
	{

	ASSERT( OnEngineThread() );

	int retcode = RTERROR;
	if(SDS_CURVIEW == NULL) 
		{
		return(RTERROR);
		}

	if( RTERROR == SDS_SetActiveVPbyNum(nWhichVPort) ) 
		{
		return(RTERROR);
		}

	sds_name nmSelSet;

	// Note the use of the bSpecialImageFilter parameter
	// This is a temporary hack to get sds_update to work for images
	//
	if ( SDS_PickObjects( "C"/*DNT*/, ptCorner1, ptCorner2, NULL, nmSelSet, 0, SDS_CURDWG, true, true ) == RTNORM )
		{
		long iSSSize = 0;
		sds_sslength( nmSelSet, &iSSSize );

		retcode = RTNORM;

		int i;
		for( i = 0; i < iSSSize; i++ )
			{
			sds_name nmEntity;

			if ( sds_ssname( nmSelSet, (long)i, nmEntity ) != RTNORM )
				{
				retcode = RTERROR;
				break;
				}
			// redraw the entity
			// 0 means DON'T BitBlt to screen
			//
			SDS_RedrawEntity( nmEntity, IC_REDRAW_DRAW, NULL, NULL, 0 );
			

			sds_ssfree( nmEntity );
			}

		sds_ssfree( nmSelSet );

		// When everything is done, now we bit blit to the screen
		//
		SDS_BitBlt2Scr(1);
		}

	return retcode;
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_gettbpos(const char *pToolBarName, sds_point ptTbPos)
	{
	if(NULL==pToolBarName)
	{
		return RTERROR;
	}
	//*** Set the toolbar string in the CMainWindow class.
	SDS_CMainWindow->m_strWndAction=pToolBarName;
	SDS_CMainWindow->m_pvWndAction=ptTbPos;


	//*** Send the WndAction to find out if the toolbar name is valid.
	ExecuteUIAction( ICAD_WNDACTION_GETTBPOS );   

	if(SDS_CMainWindow->m_bWndAction==FALSE)
		{
 		return RTERROR;
		}


	return RTNORM;
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_setview(const struct sds_resbuf *prbViews, int nWhichVPort) {		 

	ASSERT( OnEngineThread() );

	if(prbViews==NULL) return(RTERROR);
	struct resbuf *trb,rb;
	int tilemode, cvport;
	
	SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	tilemode=rb.resval.rint;
	SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	cvport=rb.resval.rint;

	// If tilemode is 0, work on viewport in paperspace
	if( tilemode == 0 ) 
		{
		db_viewport *pViewport;
		if ( nWhichVPort == 0 )
			{
			pViewport = SDS_CURDWG->GetCurrentViewport();
			}
		else
			{
			pViewport = SDS_CURDWG->GetViewport( nWhichVPort );
			}
		if ( pViewport == NULL )
			{
			return RTERROR;
			}
		if ( pViewport->SetView( SDS_CURDWG, prbViews ) )
			{
			sds_name nmEntity;
			nmEntity[0] = (long)pViewport;
			nmEntity[1] = (long)SDS_CURDWG;

			SDS_ClearViewPort( (db_handitem *)pViewport, SDS_CURVIEW );
			SDS_RedrawEntity( nmEntity, IC_REDRAW_DRAW, NULL, NULL, 1 );
			return RTNORM;
			}
		else
			{
			return RTERROR;
			}
		}
	else
		{
		sds_point center,target;
		sds_real viewsize,viewsizeold;
		int vmode,cenflag;
		sds_point viewdir,viewdirold;
		sds_real twistorfrontz,vtwist,vtwistold;
		sds_real backz,frontz;
		
		sds_real lensLength;
		int newViewMode;

		struct gr_view *view;
	
		view=SDS_CURGRVW;
		if(view==NULL) 
			return(RTERROR);
	
		// If 0 is passed in, set the current viewport
		//
		if ( nWhichVPort != 0 )
			{
			if(RTERROR==SDS_SetActiveVPbyNum(nWhichVPort)) 
				return(RTERROR);
			}

		if(SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
		vmode=rb.resval.rint;
		if(SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
		ic_ptcpy(viewdirold,rb.resval.rpoint);
		ic_ptcpy(viewdir,rb.resval.rpoint);
		//NOTE: resbuf llist MUST contain 10 or 12 group as center in RP coords
		if(SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
		viewsize=rb.resval.rreal;
		viewsizeold=rb.resval.rreal;
		if(SDS_getvar(NULL,DB_QVIEWTWIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
		vtwist=rb.resval.rreal;
		vtwistold=rb.resval.rreal;
		if(SDS_getvar(NULL,DB_QBACKZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
		backz=rb.resval.rreal;
		if(SDS_getvar(NULL,DB_QFRONTZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
		frontz=rb.resval.rreal;

		cenflag=0;
		
		for(trb=(struct resbuf *)prbViews; trb!=NULL; trb=trb->rbnext) 
			{
			switch(trb->restype) 
				{
				case 40:
					viewsize=trb->resval.rreal;
					break;
				case 11:
					if(icadRealEqual(viewdir[0],0.0) && icadRealEqual(viewdir[1],0.0) && icadRealEqual(viewdir[2],0.0)) break; 
					ic_ptcpy(viewdir,trb->resval.rpoint);
					break;
				case 12:
					//caller MUST pass center OR target for viewing
					ic_ptcpy(target,trb->resval.rpoint);
					cenflag++;
					break;
				case 10:
					//caller MUST pass center OR target for viewing
					ic_ptcpy(center,trb->resval.rpoint);
					cenflag++;
					break;

				case 42: 
					lensLength = trb->resval.rreal;
					break;

				case 43:
					frontz=trb->resval.rreal;
					break;	
				case 44:
					backz=trb->resval.rreal;
					break;
				case 71:
					vmode=trb->resval.rint;
					newViewMode = vmode;
					break;
				case 50:
					vtwist=trb->resval.rreal;
					break;
					
				}
			}

		if(!cenflag)
			return(RTERROR);

		SDS_SetUndo(IC_UNDO_GROUP_BEGIN, (void*)"SETVIEW"/*DNT*/, NULL, NULL, SDS_CURDWG);	/*D.G.*/

		if(vmode&1)
			{
			vmode=1;		//tell zoomit we're in perspective projection & save for undo
			twistorfrontz=frontz;
			ic_ptcpy(center,target);
			}
		else
			{
			vmode=2;		//tell zoomit to use orthographic projection & save for undo
			twistorfrontz=vtwist;
			rb.restype=RT3DPOINT;
			ic_ptcpy(rb.resval.rpoint,viewdir);
			sdsengine_setvar("VIEWDIR"/*DNT*/,&rb);//set viewdir so that we can init the view vars for correct rp2ucs proj
			rb.restype=RTREAL;
			rb.resval.rreal=vtwist;
			sdsengine_setvar("VIEWTWIST"/*DNT*/,&rb);//set viewtwist so that we can init the view vars for correct rp2ucs proj
			rb.restype=RTREAL;
			rb.resval.rreal=viewsize;
			sdsengine_setvar("VIEWSIZE"/*DNT*/,&rb);//set viewsize so that we can init the view vars for correct rp2ucs proj

			rb.restype = RTREAL;
			rb.resval.rreal = lensLength;
			sdsengine_setvar("LENSLENGTH"/*DNT*/,&rb); 

			gr_initview(SDS_CURDWG,&SDS_CURCFG,&view,NULL);
			gr_rp2ucs(center,center,view);
			rb.restype=RT3DPOINT;
			ic_ptcpy(rb.resval.rpoint,viewdirold);
			sdsengine_setvar("VIEWDIR"/*DNT*/,&rb);
			rb.restype=RTREAL;
			rb.resval.rreal=vtwistold;
			sdsengine_setvar("VIEWTWIST"/*DNT*/,&rb);
			rb.restype=RTREAL;
			rb.resval.rreal=viewsizeold;
			sdsengine_setvar("VIEWSIZE"/*DNT*/,&rb);
			}
	
	
		int ret = SDS_ZoomIt(SDS_CURDWG, vmode, center, &viewsize, 
							 &newViewMode, viewdir, &twistorfrontz, &backz);

		SDS_SetUndo(IC_UNDO_GROUP_END, (void*)"SETVIEW"/*DNT*/, NULL, NULL, SDS_CURDWG);	/*D.G.*/

		/*D.G.*/// The following call of SDS_SaveDataForPreviousOrUndo was added to create an undo unit
		// for REDO command (we should zoom after changing sysvars).
		SDS_SaveDataForPreviousOrUndo(SDS_CURVIEW, SDS_CURDWG, tilemode, cvport);

		return ret;
		}

}

// *****************************************
// This is an SDS External API
//
HDC *
sdsengine_getviewhdc(void) 
	{ 
	if(SDS_CURVIEW==NULL) 
		{
		return(NULL);
		}

	// UGH UGH UGH!!!
	//
	// For whatever perverse reason, this function returns a POINTER to the HDC
	// rather than the HDC.  This allows the caller to muck with our internals among
	// other bad things.  I changed it to at least point to a static copy.  Note, this
	// means the contents may change unexpectedly, but it would before also...
	// I would eliminate this function altogether if I could without breaking someone's app
	//
	static HDC zHackStaticHDC = SDS_CURVIEW->GetFrameBufferDC();
    return(&zHackStaticHDC);
}

// *****************************************
// This is an SDS External API
//
HWND 
sdsengine_getviewhwnd(void) 
	{ 
	if(SDS_CURVIEW==NULL) 
		{
		return(NULL);
		}
    return(SDS_CURVIEW->m_hWnd);
	}

// *****************************************
// This is an SDS External API
//
HWND 
sdsengine_getmainhwnd(void) 
	{ 
	return(IcadSharedGlobals::GetMainHWND());
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_getpreviewbmp(const char* pFileName, void** pHBITMAP, unsigned long* pAllocatedBytes, short* bIsBitmap) 
	{ 
	return(IcadSharedGlobals::CallGetPreviewBM(pFileName, pHBITMAP, pAllocatedBytes, bIsBitmap));
	}


// *****************************************
// This is an SDS External API
//
int 
sdsengine_swapscreen(void) 
	{ 

	// This MAY be safe to call from outside the engine
	//
	ASSERT( OnEngineThread() );

	if(SDS_CURVIEW==NULL) 
		{
		return(RTERROR);
		}
	SDS_BitBlt2View(0,SDS_CURVIEW);

    return(RTNORM);
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_sendmessage(char *szCommandMsg) 
	{
	SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)szCommandMsg);
    return(RTNORM);
	}


#include "icaddoc.h"
int sds_drawLinePattern(HDC h_dc,RECT rect, struct sds_resbuf *lineParam){

	CDC *pDC=CDC::FromHandle(h_dc);

	db_drawing *p_dbDrawing=SDS_CMainWindow->m_pCurDoc->m_dbDrawing;

	double size = 0.0;
	sds_point p1={0.0,0.0,0.0},p2={rect.right-rect.left,0.0,0.0};

	db_line *p_line=new db_line(p_dbDrawing);

	p_line->set_10(p1);
	p_line->set_11(p2);

	if (!lineParam) return 0;
	for (sds_resbuf *prb=lineParam; prb; prb=prb->rbnext)
		switch (prb->restype){
		case 6:		/* Linetype name */
			p_line->set_6(prb->resval.rstring,p_dbDrawing);	//set linetype
			break;
		case 10:	/* Begin point */
			p_line->set_10(prb->resval.rpoint);
			break;
		case 11:	/* End point */
			p_line->set_11(prb->resval.rpoint);
			break;
		case 62:	/* Line color */
			p_line->set_62(prb->resval.rint);
			break;
	}

	CBitmap *p_bitmap = SDS_DrawBitmap(p_dbDrawing,NULL,NULL,p_line,
		rect.right-rect.left,rect.bottom-rect.top,FALSE);
	delete p_line;

	CDC memDC; 
	if(memDC.CreateCompatibleDC(pDC)){
		memDC.SelectObject(p_bitmap);
		pDC->BitBlt(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,&memDC,0,0,SRCAND);
	}
return 1; 
}


/*-------------------------------------------------------------------------*//**
Saves the current drawing's view as a BMP file.

@author	Dmitry Gavrilov
@param	pFile	=> bmp file name; if NULL or empty then the used is asked for it
@param	width	=> width of saved bmp; if 0 then full size of view is used (both width and height)
@param	height	=> height of saved bmp; if 0 then full size of view is used (both width and height)
@return SDS return codes: RTNORM in success, RTERROR in some failure, RTCAN when
 user cancels some input
*//*--------------------------------------------------------------------------*/
int sdsengine_bmpout(const char* pFile, int width /*= 0*/, int height /*= 0*/)
{
	if(!SDS_CURVIEW)
		return RTERROR;

	int		ret = RTNORM;

	if(!pFile || !*pFile)
	{
		char	fname[IC_ACADBUF];
		resbuf	rb;

		if(SDS_getvar(NULL, DB_QFILEDIA, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) != RTNORM)
			return RTERROR;

		if(!rb.resval.rint)
		{
			if((ret = sds_getstring(1, ResourceString(IDC_TODO__NBITMAP_TO_CREATE__2, "\nBitmap to create: "), fname)) != RTNORM)
				return ret;
			if(*fname == '~'/*DNT*/)
				rb.resval.rint = 1;
		}
		if(rb.resval.rint)
		{
			if((ret = SDS_GetFiled(ResourceString(IDC_TODO_SAVE_BITMAP_0, "Save Bitmap"), NULL,
								   ResourceString(IDC_TODO_BITMAP_FILE_BMP_1, "Bitmap File|bmp"), 1, &rb, NULL, NULL)) != RTNORM)
				return ret;
			if(rb.restype == RTSTR)
			{
				strcpy(fname, rb.resval.rstring);
				IC_FREE(rb.resval.rstring);
			}
			else
				return RTCAN;
		}

		if(!*fname)
			return RTERROR;

		ChangeSlashToBackslashAndRemovePipe(fname);

		ic_setext(fname, const_cast<char*>(ResourceString(DNT_ICADDOC__BMP_19, ".bmp")));

		pFile = fname;
	}

	char*	pBuffer = NULL;
	long	bytes = 0L;

	if(width && height)
	{
		CBitmap*	pBmp = SDS_DrawBitmap(SDS_CURDWG, NULL, NULL, NULL, width, height, TRUE);
		if(pBmp)
		{
			if(SDS_SaveBitmapToBuffer(pBmp, &pBuffer, &bytes, false))
				ret = RTERROR;
			delete pBmp;
		}
		else
			ret = RTERROR;
	}
	else
	{
		if(!SDS_CURVIEW || SDS_CURVIEW->SaveBitmapToBuffer(&pBuffer, &bytes, false))
				ret = RTERROR;
	}

	if(ret == RTNORM)
		ret = SDS_SaveBufferToFile(pBuffer, bytes, const_cast<char*>(pFile));

	IC_FREE(pBuffer);

	return ret;
}
