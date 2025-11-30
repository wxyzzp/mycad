/* C:\DEV\PRJ\ICAD\ICADAPI.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Support functions for linking Ramona to the CAD engine.
 *
 */

// ** Includes
#include "Icad.h"/*DNT*/
#include "IcadATL.h"
#include "vxtabrec.h"
#include "IcadView.h"
#include "IcadEntityRenderer.h"
#include "LicensedComponents.h"
#include "IcadHelpers.h"/*DNT*/
#include "fixup1005.h"
#include "Modeler.h"
#include "IntCmds.h"

namespace ICAD_AUTOMATION
{
#include "IcadTLB.h"
}
using namespace ICAD_AUTOMATION;
#include "IcadAPC.h"
#include "IcadApp.h"
#include "IcadChildWnd.h"
#include "IcadDoc.h"
#include "paths.h"
#include <eh.h>
#include "sdsthreadexception.h" /*DNT*/
#include "sdsthread.h" /*DNT*/
#include "sdsapplication.h" /*DNT*/
#include "threadcontroller.h" /* DNT */
#include "IcadRaster.h"/*DNT*/
#include "icadmenu.h"/*DNT*/
#include "lisp.h"/*DNT*/
#include "commandatom.h" /*DNT*/
#include "cmds.h"/*DNT*/
#include "icadtablet.h"/*DNT*/
#include "commandqueue.h" /*DNT*/
#include "IcadPrivate.h" /*DNT*/
#include "safecounter.h" /*DNT*/
#include "icadgridiconsnap.h"

#include "inputevent.h" /*DNT*/
#include "inputeventfilter.h" /*DNT*/
#include "inputeventqueue.h" /*DNT*/

#include "SyncObject.h"
#include "IcadCursor.h"
#include "Preferences.h"

#include "DbGroup.h"
#include "DbMText.h"

#include "cmdsGroup.h"
#include "cmdsViewport.h"

#ifdef _USE_DISP_OBJ_PTS_3D_
//#pragma message("_USE_DISP_OBJ_PTS_3D_ defined in icadapi.cpp")
#include "DoSelect.h"
#include "DoBase.h"
#endif

#if defined(USE_SMARTHEAP)
	#include "smrtheap.h"/*DNT*/
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// SystemMetrix(shiba)-[06.10.2001
//	 FIX: During command(zoom, regen etc) execution, moving mouse (wheel), crash.
bool			SDS_BuildRegenList_Doing	= false;
// ]-SystemMetrix(shiba) 06.10.2001

// extern int Snap_DrawInOneWindow;	//declared in icadview.cpp, used to show flyover snap in all views
// ICAD.EXE for example
//
char		   SDS_AppName[IC_ACADBUF];


// The main application window
//
// Bugzilla::78404;			Date: 09-02-04
//!!! HWND		   SDS_hwnd;
// This is now stored in "IcadSharedGlobals" class
// Thus the Global variable is removed.
// Bugzilla::78404;			Date: 09-02-04


// The instance.  Used by SDS_GetGlobals to pass environment info
// to sds apps
//
HINSTANCE	   SDS_appinstance;

// Has the program been initialized.
//
static int			  SDS_inited;

// How many levels of user break are we in.  Doesn't always work...
//
int 		   SDS_userbreak;

// ????
char		   SDS_cmdcharbuf[IC_ACADBUF];

// Only used in icadapi.cpp
//
static UINT 		  SDS_CurEventMask;

// ??? Something about executing multiple commands
//
int 		   SDS_MultibleMode;

// SDS_FunctionID is returned by sds_getfuncode to tell the SDS app what
// function has been called.  This SHOULD NOT BE A GLOBAL!	AAAAAAAAARRRRGGGG!
//
int 		   SDS_FunctionID;

// SDS_LispPCount is the "parentheses count".  The number of pending paren needed to close
// the expression
//
int 		   SDS_LispPCount;

// ?????
int 		   SDS_AtCmdLine;

// ?? Found this comment in cmds4.cpp:
// "This golobal var is so the command line can bypass the prompt for the variable itself."
//
char		  *SDS_SetVarByPass;

// ???
static char 		  SDS_LastPrompt[IC_ACADBUF];

// Set while in CIcadView::OnMouseMove, looped on in dragging code.  VERY SCARY! Depends on threading and
// side-effects
//
int 		   SDS_AtMouseMove;

// ????
int			   SDS_IsDragging;


// NEVER USED -- REMOVE???
//
int 		   SDS_ExternalQuery;
db_drawing *SDS_ExtdwgBeg,*SDS_ExtdwgCur;

// ???
int			   SDS_BoxModeCursor;

// ???
int 		   SDS_NoCharEcho;

// ???
int 		   SDS_AtCopyMoveCommand;

// ???
int			   SDS_UseExportSelSet;

// ???
int			   SDS_InsideGrread;


// Used to delay init'ing lisp until we get back to a thread that can do it
//
static int			   SDS_DelayLispInit;

// ???
int			   SDS_AutoSave;

// ???
int			   SDS_IgnoreMenuWQuote;

// ???
int 		   SDS_SkipLastOrthoSet;

// ???
int 		   SDS_IgnoreMenuStar;

// 	Used to defer drawing.	Bad.
//
bool		   SDS_DontBitBlt;

// Seems to be a HACK to fix memory leak??
//
static char 		 *SDS_FreeKList;


// Communicates that the current command is being done transparently
//
static bool    SDS_DoingCommandTrans;

// Are we executing from lisp
//
bool		   SDS_DoingLispCode;

// Some other kind of hack
//
bool		   SDS_ElevationChanged;
double		   SDS_CorrectElevation;

// Have ANY user callbacks been registered??
//
bool		   SDS_SetAUserCallback;

// ???
char		  *SDS_UndoCmdName;

// Keeps track of calls to 3rd party functions, because they are on a different thread
// and we may threadswap differently
// ????
LONG		   SDS_Inside3rdPartyFunc = 0;

// ????
bool		   SDS_LastCmdWasHatch;

// ????
static bool 		  SDS_ReleasedLinkFunc;

// Used to keep from adding the paperspace viewport to a selection set in ssadd
// HACK...
bool		   SDS_InsideSSwithX;

// ???
bool		   SDS_WasFromPipeLine;


extern SDSApplication *SDS_LoadedAppsLast;

// Modified PK 31/05/2000
// Reason: Fix for bug no. 57282
// 0 = Do not convert list in point form.
// 1 = Convert list in point form.
// 2 = Must convert list in point form. Called by internalGetpoint(...)
int LSP_ConvertToPoint = 1;

// HACK for thread swap
int			   SDS_AtThreadSwapCount;


//4M Spiros Item:27->
int 			GripRClickedFlag=0;
sds_point		SDS_GripRClicked={ 0,0,0 };
sds_name		SDS_EntBeingDragged={ 0L,0L };
bool			SDS_GripDragging=0;
bool			SDS_GripPlaced=0;
bool			SDS_AtDragMulti=0;
bool			SDS_DragNOElevation=false;
//4M Spiros Item:27<-

//4M Spiros Item:31->
CString 		MdMacro;
//4M Spiros Item:31<-

// queue of commands
//
struct SDS_CmdHist	  *SDS_CmdHistBeg, *SDS_CmdHistCur, *SDS_CmdHistAct;


struct resbuf *SDS_UserDocList;

// used for AutoSave
static CTime*	SDS_SaveTime;

// Modified PK 28/06/2000
// Reason : Modification in fix for bug 56908
struct SDS_dragvars *SDS_Tvars;

extern struct SafeAliasList  *SDS_AliasListBeg;

extern int SDS_AtEntGet;

extern int SDS_AtNodeDrag;
extern sds_name  SDS_NodeDragEnt;
extern struct gr_displayobject *SDS_NodeDragEntDispObj;

extern struct lsp_pendlink lsp_pend;
extern struct resbuf lsp_evres,*lsp_argsll; /* A "SUPER" RESBUF LLIST */
extern int	lsp_cmdhasmore;

// "true" if we are in the Explorer, so we shouldn't set "COMMAND_END" undo unit because of using GROUP-units in Explorer.
bool SDS_InExplorer = false;

// The window has called Explorer
HWND SDS_hWndExplorerCaller = NULL;

// ** Protos
void		   SDS_cmdline(HWND hWnd);
int 		   SDS_FreeCmdHist(void);
struct resbuf *SDS_EntListToVects(const struct resbuf *elist);
struct resbuf *SDS_ArcSegments(int mode, sds_real viewht,sds_real r0,sds_real r1,sds_real r2,sds_real r3,sds_real r4,sds_real elev, struct resbuf *ecsp, struct resbuf *destcsp);
int			   SDS_VarsToReg(void);
int			   SDS_RegToVars(void);
void		   SDS_2dptmaxmin(sds_point minpt, sds_point maxpt, sds_point testpt);
int 		   SDS_cubeUCStoWCS(const sds_point ucsextmin,const sds_point ucsextmax,sds_point wcsextmin, sds_point wcsextmax);
void		   SDS_FatalErrorFunc(unsigned int u, EXCEPTION_POINTERS* pExp);

// *************************************************
// These messages are used internally by IntelliCAD
//
static UINT GetEventMessageID( void )
	{
	static UINT uiMessageID = 0;
	if ( !uiMessageID )
		{
		uiMessageID = RegisterWindowMessage( "SDSEventMessage" /*DNT*/ );
		ASSERT( uiMessageID != 0 );
		}
	return uiMessageID;
	}



static UINT GetMouseMoveEventMessageID( void )
	{
	static int uiMessageID = 0;
	if ( !uiMessageID )
		{
		uiMessageID = RegisterWindowMessage( "SDSMouseMoveEventMessage" /*DNT*/ );
		ASSERT( uiMessageID != 0 );
		}
	return uiMessageID;
	}


// ** Functions

int SDS_CallUserCallbackFunc(int flag,void *arg1,void *arg2,void *arg3)
{
	if(SDS_SetAUserCallback==FALSE)
	{
		if((flag == SDS_CBENTMOD || flag == SDS_CBENTDEL || flag == SDS_CBENTUNDO ||
			flag == SDS_CBENTREDO) && SDS_CURDOC && SDS_CURDOC->m_cmdHelper.isCheckCallback())
		{
			SDS_CURDOC->m_cmdHelper.callbackChanged((db_handitem*)*(long*)arg1, flag == SDS_CBENTUNDO);
		}
		return(RTNORM);
	}

	int ret=RTNORM;
	// first external application is Beg->next
	//
	//for(SDSApplication *curApp=SDSApplication::GetEngineApplication()->GetNextLink(); curApp!=NULL; curApp=curApp->GetNextLink())
	for(SDSApplication *curApp=SDSApplication::GetEngineApplication(); curApp!=NULL; curApp=curApp->GetNextLink())
		{
		ret = curApp->CallCallBack( flag, arg1, arg2, arg3 );
		if(ret==RTERROR)
			break;
		}

	if((flag == SDS_CBENTMOD || flag == SDS_CBENTDEL && ret == RTNORM || flag == SDS_CBENTUNDO ||
		flag == SDS_CBENTREDO) && SDS_CURDOC && SDS_CURDOC->m_cmdHelper.isCheckCallback())
	{
		SDS_CURDOC->m_cmdHelper.callbackChanged((db_handitem*)*(long*)arg1, flag == SDS_CBENTUNDO);
	}
	return(ret);
}

// This is used to set CMDACTIVE
//
static LONG SDS_InInternalFunction = 0;

int SDS_GetVarSimple(short qidx, struct resbuf *res)
{
	return SDS_getvar(NULL, qidx, res, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
}

int SDS_GetPreviewBmp(const char* pFileName, void** pHBITMAP, unsigned long* pAllocatedBytes, short* bIsBitmap)
{
	ASSERT(pFileName != NULL);
	ASSERT(pHBITMAP != NULL);
	
	int retVal = 0;

	if (!SDS_CMainWindow)
		return retVal;

	bool bIsBmp;
	unsigned long bytes;

	retVal = SDS_CMainWindow->m_pFileIO.drw_getdwgbitmap(pFileName, *pHBITMAP, bytes, bIsBmp);
	*pAllocatedBytes = bytes;
	*bIsBitmap = (bIsBmp == true) ? 1 : 0;

	return retVal;
}

int SDS_getvar(const char *vname, short qidx, struct resbuf *res, db_drawing *flp, db_charbuflink *configsv, db_charbuflink *sessionsv)
	{

	// First handle specials that are not stored in the database
	//

	// CMDACTIVE !!!
	// See reference on this variable for more information
	//
	// Key is ADS developers need to know what is going on inside IntelliCAD
	// These checks are ad-hoc.  Current (8-31-98) don't check for Script or Dialog Box
	//
	if ( (qidx == DB_QCMDACTIVE) ||
		 ((qidx == -1) && strisame( vname, "CMDACTIVE"/*DNT*/ )) )
		{
		res->restype = RTSHORT;
		res->resval.rint = 0;

		if ( SDS_InInternalFunction > 0 )
			{
				res->resval.rint |= 0x01;		// ordinary command is active
			}
		if ( SDS_Inside3rdPartyFunc > 0 )
			{
				res->resval.rint |= 0x20;		// 32d; inside SDS command
			}
		if ( (SDS_DoingCommandTrans) && ( SDS_Inside3rdPartyFunc <= 0 )) // if its a 3rd party command, 
			{															 // don't look for tranperency.
				res->resval.rint |= 0x02;		// doing a transparent command too
			}
		if((GetScriptQueue()->IsNotEmpty()) && (GetActiveCommandQueue()->IsNotEmpty()))
			{
				res->resval.rint |= 0x04;		// inside script.
			}
		return RTNORM;
		}
	else
		{
		return(db_getvar(vname,qidx,res,flp,configsv,sessionsv));
		}
	}


	// Utility to set DBMOD variable, which is read-only via sds_setvar
	// flag is or'd into current value
int
SDS_DBModified( DWORD whichFlag)
	{
	sds_resbuf	rb;
	int			original;
	int			result = RTNORM;

	SDS_getvar(NULL,DB_QDBMOD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

								// save initial status for UI notification
	original = rb.resval.rint;
	if( !(rb.resval.rint & whichFlag) )
		{
		rb.resval.rint |= (whichFlag | IC_DBMOD_DATABASEVBL_MODIFIED);
		result = SDS_setvar(NULL,DB_QDBMOD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

								// notify UI if file changed
		if ( !(original & IC_DBMOD_DATABASEVBL_MODIFIED) )
			ExecuteUIAction( ICAD_WNDACTION_DBMODON );
		}

	return result;
	}


//#define RISE_SYSVAR_CHANGE_CALLBACK
int SDS_setvar(const char *vname, short qidx, const struct resbuf *sour, db_drawing *flp, db_charbuflink *configsv, db_charbuflink *sessionsv, short asis)
{
	/*D.G.*/// We save OSMODE in the registry and in drawing headers of R14 or earlier versions files.
	// DP: Now we save and load this variable together with others.
	//if((vname && !db_compareNames("OSMODE"/*DNT*/, vname)) || qidx == DB_QOSMODE)
	//	UserPreference::OSModePreference = sour->resval.rint;

//		// Modified CyberAge VSB 12/05/2001
//		// Reason: Implementation of Flyover Snapping.
//		//set the current value of "SNAPALLVIEWS" in Snap_DrawInOneWindow variable.[
//		if(((NULL!=SDS_CURVIEW ) && vname && !db_compareNames("SNAPALLVIEWS"/*DNT*/, vname)))
//		{
//			if ( sour->resval.rint )
//				Snap_DrawInOneWindow = 0;
//			else
//				Snap_DrawInOneWindow = 1;
//		}
//		// Modified CyberAge VSB 12/05/2001 ]

	// EBATECH(CNBR) 2002/12/1 -[ Rise call back function
#ifdef RISE_SYSVAR_CHANGE_CALLBACK
	int rc;
	if((rc = db_setvar(vname,qidx,sour,flp,configsv,sessionsv,asis)) == RTNORM )
	{
		struct db_sysvarlink *found;
		if((found = db_GetLastSetvar()) != NULL )
		{
			// NOTE: PICKSTYLE switches oftenly in CIcadView::OnToolHitTest.
			// So I prohibit to rise call back.
			if( vname == NULL && qidx == DB_QPICKSTYLE )
				;
			else
			SDS_CallUserCallbackFunc( SDS_CBVARCHANGE, (void *)found->name, NULL, NULL);
		}
	}
	return rc;
#else
	return(db_setvar(vname,qidx,sour,flp,configsv,sessionsv,asis));
#endif
	// EBATECH(CNBR) ]-
}

short SDS_tblmesser(struct resbuf *rbll, short mode, db_drawing *flp,bool duplicatesok) {
// Because of the way this function is written (accepting a
// resbuf rather than a hip for accessing the object, it will search by name
// and find the FIRST entry with that name.  Only.	This behavior in many cases
// will probably not be correct.  In particular, it is possible to have an entry
// in a list which is already deleted.	If that entry is re-placed in the list,
// and this function comes along to delete the second entry, it will actually
// undelete the first entry.  Therefore it would be wise, I think, to avoid
// using this function for deletion where possible.

// Another way to put it would be that this function ALWAYS assumes that
// it is OK to undelete a deleted item.  If that is not OK for what you're doing,
// don't use this function for deletion.

	if(rbll==NULL || flp==NULL) return(RTERROR);
	sds_name ename;
	struct resbuf *tp1;
	char *oldname=NULL,*tabname=NULL,*newname=NULL;
	db_handitem *hip,*newhip=NULL,*bhip,*ehip;
	db_handitem *savehip=NULL;
	struct db_BinChange *chglst;

	for (tp1=rbll; tp1!=NULL; tp1=tp1->rbnext) {
		if(tp1->restype==0) tabname=tp1->resval.rstring;
		if(tp1->restype==1) newname=tp1->resval.rstring;
		if(tp1->restype==2) oldname=tp1->resval.rstring;
	}
	if(tabname==NULL || oldname==NULL) return(RTERROR);
	int tabidx=db_str2hitype(tabname);
	tabidx=db_tabrec2tabfn(tabidx);
	if(!db_is_tabrectype(tabidx)) return(RTERROR);

	//the real bug in this code is the fact that "mode" is being used to decide whether
	//to find valid records.  For the IC_TBLMESSER_DELETE case - it finds deleted records
	//as well as undeleted ones.  In all other cases (make, modify and rename) it only
	//finds undeleted ones.  This causes the serious bug decribed by Matt in his notes above.
	//So, if you call SDS_tblmesser (DELETE), it finds the first record with that name.  If that
	//record, is deleted, it undeletes it; else, it deletes it.  This has caused many of the
	//bugs in Explorer.  I've changed it to only find valid records.  AM 11/4/98.
	//It turned out that Cmd_ReadAsBlock was depening on this behavior of IC_TBLMESSER_DELETE
	//undeleting a record - I fixed this in read.cpp.
//old code
//	if(mode)
//		hip=flp->findtabrec(tabidx,oldname,1);
//	else
//		hip=flp->findtabrec(tabidx,oldname,0);

	//new code
	hip=flp->findtabrec(tabidx,oldname,1);

	switch(mode) {
		case IC_TBLMESSER_DELETE:  // Delete
			if(hip==NULL) return(RTERROR);
			ename[0]=(long)hip;
			ename[1]=(long)flp;
			sds_entdel(ename);
			break;
		case IC_TBLMESSER_MAKE:  // Make
			if(hip!=NULL) return(RTERROR);

			flp->get_tabrecllends(tabidx,&bhip,&ehip);
			if(flp->entmake(rbll,&newhip)) return(RTERROR);

			/*D.G.*/// Call callback here if we haven't use SDS_entmake (where it's called)
			ename[0] = (long)newhip;
			ename[1] = (long)flp;
			SDS_CallUserCallbackFunc(SDS_CBENTMAKE,(void *)ename, NULL, NULL);

			// Set the Undo.
			SDS_SetUndo(IC_UNDO_NEW_ENT_TAB_CLS,(void *)ehip,(void *)newhip,(void *)tabidx,flp);

			// Set the drawing modified flag.
			SDS_DBModified( IC_DBMOD_ENTITIES_MODIFIED);
			break;
		case IC_TBLMESSER_MODIFY:	 // Modify
			if(hip==NULL) return(RTERROR);
			SDS_CopyEntLink(&savehip,hip);
			hip->entmod(rbll,flp);

			/*D.G.*/// Call callback here if we haven't use sdsengine_entmod (where it's called)
			ename[0] = (long)hip;
			ename[1] = (long)flp;
			SDS_CallUserCallbackFunc(SDS_CBENTMOD, (void *)ename, NULL, NULL);

			if(db_CompEntLinks(&chglst,savehip,hip)==RTNORM && chglst) {
				SDS_SetUndo(IC_UNDO_CHG_ENT_TAB_CLS,(void *)hip,(void *)chglst,NULL,flp);
			}
			delete savehip;

			// Set the drawing modified flag.
			SDS_DBModified( IC_DBMOD_ENTITIES_MODIFIED);
			break;
		case IC_TBLMESSER_RENAME:  // Rename
			if(newname==NULL || hip==NULL) return(RTERROR);
			// Check to see if this name exists.
			SDS_CopyEntLink(&savehip,hip);
			if(duplicatesok==false && flp->findtabrec(tabidx,newname,0)) return(RTERROR);
			hip->set_2(newname);

			/*D.G.*/// Call callback here if we haven't use sdsengine_entmod (where it's called)
			ename[0] = (long)hip;
			ename[1] = (long)flp;
			SDS_CallUserCallbackFunc(SDS_CBENTMOD, (void *)ename, NULL, NULL);

			if(db_CompEntLinks(&chglst,savehip,hip)==RTNORM && chglst) {
				SDS_SetUndo(IC_UNDO_CHG_ENT_TAB_CLS,(void *)hip,(void *)chglst,NULL,flp);
			}
			delete savehip;

			// Set the drawing modified flag.
			SDS_DBModified( IC_DBMOD_ENTITIES_MODIFIED);
			break;
	}
	return(RTNORM);
}

struct resbuf *SDS_tblgetter(const char *tabname, const char *recname, short swFirst, db_drawing *flp) {
	if(tabname==NULL || *tabname==0 || flp==NULL) return(NULL);
	int tabidx=db_tabrec2tabfn(db_str2hitype((char *)tabname));
	if(!db_is_tabrectype(tabidx)) return(NULL);
	// A.G. Create applist rebuf to get all EED as the part of tabrec entity data
	struct resbuf *retRb = NULL;
	struct resbuf *appRb = sds_buildlist( RTSTR, "*", RTNONE );
	if(recname==NULL || *recname==0) {
		db_handitem *hip=flp->tblnext(tabidx,swFirst);
		if( hip )
			retRb = hip->entgetx(/*NULL*/ appRb, flp);
	} else {
		db_handitem *hip=flp->findtabrec(tabidx,(char *)recname,1);
		if( hip )
			retRb = hip->entgetx(/*NULL*/ appRb, flp);
	}
	sds_relrb( appRb );	// Free appRb list
	return retRb;
}

struct resbuf *SDS_tblgetter2(const char *tabname, const char *recname, short swFirst, db_drawing *flp) {
// this version does NOT return the first one if "" is given as the name, it searches for ""
	if(tabname==NULL || *tabname==0 || flp==NULL) return(NULL);
	int tabidx=db_tabrec2tabfn(db_str2hitype((char *)tabname));
	if(!db_is_tabrectype(tabidx)) return(NULL);
	if(recname==NULL) {
		db_handitem *hip=flp->tblnext(tabidx,swFirst);
		if(hip==NULL) return(NULL);
		return(hip->entgetx(NULL,flp));
	} else {
		db_handitem *hip=flp->findtabrec(tabidx,(char *)recname,1);
		if(hip==NULL) return(NULL);
		return(hip->entgetx(NULL,flp));
	}
}
// **************************************************
// **************************************************
// **************************************************
// Progress bar functions.
//
extern int SDS_ProgressPos;

int SDS_ProgressStart(void)
	{
	if(!cmd_InsideDDInsert)
		{
		ExecuteUIAction( ICAD_WNDACTION_STARTPROG );
		}
	return(RTNORM);
	}

int SDS_ProgressStop(void)
	{
	if(!cmd_InsideDDInsert)
		{
		ExecuteUIAction( ICAD_WNDACTION_ENDPROG );
		}
	return(RTNORM);
	}

int SDS_ProgressPercent(int iPercentDone)
	{
	SDS_ProgressPos=iPercentDone;
	if(!cmd_InsideDDInsert)
		{
		ExecuteUIAction( ICAD_WNDACTION_UPDATEPROG );
		}
	return(RTNORM);
	}

// *********************************
// SPECIAL
//
// TODOTHREAD -- are these threadsafe or not???
//
// *****************************************
// This is an SDS External API
//
int
sdsengine_progressstart(void)
	{
	return SDS_ProgressStart();
	}

// *****************************************
// This is an SDS External API
//
int
sdsengine_progressstop(void)
	{
	return SDS_ProgressStop();
	}

// *****************************************
// This is an SDS External API
//
int
sdsengine_progresspercent( int percentDone)
	{
	return SDS_ProgressPercent( percentDone);
	}


// END OF PROGRESS BAR APIs
// **************************************************
// **************************************************
// **************************************************




db_handitem *SDS_TabReaderHip;
int SDS_TabReaderIdx;

int SDS_StartTblRead(const char *tabname) {
	if(SDS_CURDWG==NULL) return(RTERROR);
	SDS_TabReaderIdx=db_str2hitype((char *)tabname);
	if(!db_is_tabrectype(SDS_TabReaderIdx)) return(RTERROR);
	SDS_TabReaderHip=SDS_CURDWG->tblnext(SDS_TabReaderIdx,1);
	return(RTNORM);
}

int SDS_TblReadHasMore(void) {
	return(SDS_TabReaderHip ? TRUE : FALSE);
}

char *SDS_TblReadString(void) {
	if(SDS_CURDWG==NULL) return(NULL);
	if(SDS_TabReaderHip==NULL) return(NULL);
	char *fcp1;
	SDS_TabReaderHip->get_2(&fcp1);
	SDS_TabReaderHip=SDS_CURDWG->tblnext(SDS_TabReaderIdx,0);
	return(fcp1);
}

// **************************************************************************
// ****************** Command line functions ********************************
// **************************************************************************


void SDS_CancelAllPending(int NoMenu)
	{

	if ( GetCancelScriptQueue()->IsEmpty() )
		{
		GetCancelScriptQueue()->Copy( GetActiveCommandQueue() );
		}


	GetScriptQueue()->Flush();

	dlg_term_dialog();
	SDS_PromptMenu(0);

	SDS_FreeEventList(NoMenu);
	}

class SDS_Exception
{
private:
	unsigned int nSE;
public:
	SDS_Exception() {}
	SDS_Exception( unsigned int n ) : nSE( n ) {}
	~SDS_Exception() {}
	unsigned int getSeNumber() { return nSE; }
};

bool SDS_HadFatalError;

void SDS_FatalErrorFunc(unsigned int u, EXCEPTION_POINTERS* pExp ) {

	if(SDS_HadFatalError) return;
	SDS_HadFatalError=TRUE;

	// Eliminate the apology dialog
	/*
	LOADSTRINGS_2(IDC_ICADAPI_WE_APOLOGIZE___A_7)
	if(MessageBox(SDS_hwnd,	LOADEDSTRING, ResourceString(IDC_ICADAPI_FATAL_ERROR_14, "Fatal error" ), MB_YESNO|MB_ICONERROR) == IDYES) {
		SDS_AutoSave=-1;
		ExecuteUIAction( ICAD_WNDACTION_SAVEALL );
	}
	*/

	// Switched from "Run-time error" to exception so we can catch and proceed
//	AfxAbort();
	throw SDS_Exception();
}

/*----------------------------------------------------------------------------*/
BOOL SDS_OurWinExec
(
LPCTSTR	lpszImageName,
LPCTSTR lpszCommandLine,
WORD	wShowWindow
)
{
   STARTUPINFO	  si;
   PROCESS_INFORMATION	pi;
   BOOL 		  RetCode;

   CString		imageName( lpszImageName);
   CString		commandLine( lpszCommandLine);

   memset(&si,0,sizeof(STARTUPINFO));
   si.cb = sizeof(STARTUPINFO);
   si.lpTitle=const_cast<LPTSTR>((LPCTSTR)imageName);
   si.dwFlags=STARTF_USESHOWWINDOW;
   // EBATECH(CNBR) -[ add optional argument wShowWindow
   //si.wShowWindow=SW_SHOWNOACTIVATE;
   si.wShowWindow=wShowWindow;
   // EBATECH(CNBR) ]-

   memset(&pi,0,sizeof(PROCESS_INFORMATION));

   RetCode = CreateProcess (NULL, const_cast<LPTSTR>((LPCTSTR)commandLine),
						  NULL, NULL, FALSE,
						  (DWORD) NORMAL_PRIORITY_CLASS,
						  NULL, NULL,
						  &si, &pi) ;
   if( RetCode )
	{
		HANDLE hand = OpenProcess(SYNCHRONIZE,TRUE,pi.dwProcessId);
		if( hand )
		{
			while( TRUE )
			{
				if( sds_usrbrk() )
				{
					TerminateProcess(pi.hProcess,0);
					break;
				}

				DWORD res = ::WaitForSingleObject( hand, 500 );
				if( res == WAIT_OBJECT_0 )
					break;
			}

			// clean up
			CloseHandle( hand );
		}
	}

/*	HANDLE hand;
	while(NULL!=(hand=OpenProcess(SYNCHRONIZE,TRUE,pi.dwProcessId))) {
		CloseHandle(hand);
		Sleep(500);
		if(sds_usrbrk()) {
			TerminateProcess(pi.hProcess,0);
			break;
		}
	}
*/
	if(RetCode) {
		if(pi.hProcess && pi.hProcess != INVALID_HANDLE_VALUE) CloseHandle(pi.hProcess);
		if (pi.hThread && pi.hThread != INVALID_HANDLE_VALUE)  CloseHandle(pi.hThread);
	}

	return RetCode;
}

/*-------------------------------------------------------------------------*//**
Find command atom.

@author	Martin Waardenburg
@param	pCommandAtom		<=	command atom
@param	pCommandString		<=> command string
@param	pTransparent		 <= *pTransparent is set to 'true' if the command is transparent and pTransparent != NULL
@param	bCheckAliasPrompt	 => check alias prompt
@return 1 for success, 0 for error. Note that command atom can be NULL even if
		success has been returned.
*//*--------------------------------------------------------------------------*/
/*DG - 30.7.2002*/// Now this function has not the side effect - changing of global SDS_DoingCommandTrans.
// I added pTransparent param instead.
static int				findCommandAtom
(
class commandAtomObj	*&pCommandAtom,
TCHAR					*pCommandString,
bool*					pTransparent = NULL,
bool					bCheckAliasPrompt = false
)
{

	if(pTransparent)
	{
		// Allow for transparent commands even when the first char is not the '
		// as in _.'ddgrips or _'ddgrips, or .'ddgrips. Only looking in the first
		// three characters. Has to be one of them
		TCHAR *pLocation;

		if ((pLocation = _tcsstr(pCommandString, "'"/*DNT*/)) != NULL)
		{
			if (pLocation <= pCommandString + 2)
			{
				memcpy(pLocation, pLocation + 1, (size_t) strlen(pLocation) + 1);
				*pTransparent = true;
				//SDS_DoingCommandTrans = TRUE;
			}
			// I replaced this with the code above, but decided to hang onto this for a
			// while.
			//if(*cmd=='\'') SDS_DoingCommandTrans=TRUE;
			//for(fcp1=cmd; *fcp1=='\''; fcp1++) ;
		}
	}

	// Since AutoCAD allows leading underscores and dots for calling aliases, we should too.
	// However we can't. So if we are searching the alias list, strip off the underscore and or dot.
	// Use a temporary variable because we do NOT want to alter fcp1. It is used later if
	// we don't find the alias.
	char tmpcmd[IC_ACADBUF], *tmpcmdPtr;
	tmpcmdPtr = pCommandString;
	// EBATECH(R.Arayashiki) -[ for MBCS String
	//while (*tmpcmdPtr == '\''/*DNT*/ || *tmpcmdPtr == '_'/*DNT*/ || *tmpcmdPtr == '.'/*DNT*/)
	//	tmpcmdPtr++;
	while (*tmpcmdPtr == '\''/*DNT*/ || *tmpcmdPtr == '_'/*DNT*/ || *tmpcmdPtr == '.'/*DNT*/)
	{
		tmpcmdPtr++;
		if ((_MBC_LEAD ==_mbbtype((unsigned char)*tmpcmdPtr,0)) &&
			  (_MBC_TRAIL==_mbbtype((unsigned char)*(tmpcmdPtr+1),1)))
			tmpcmdPtr ++;
	}
	// ]-EBATECH
	_tcscpy(tmpcmd, tmpcmdPtr);

	// check alias list.
	struct SDS_AliasList *Atmp;

	for (Atmp = SDS_AliasListBeg; Atmp!=NULL; Atmp=Atmp->next)
	{
		if (strisame(Atmp->alias, tmpcmd)) // After stripping dots and/or underscore.
		{
			if (bCheckAliasPrompt && NULL != Atmp->prompt)
			{
				// Do the shell command.
				int pc = 0;
				TCHAR fs1[IC_ACADBUF];
				*pCommandString = 0;

				if (RTCAN == sds_getstring(Atmp->crmode, Atmp->prompt, pCommandString))
					return 0;

				if (0 == *pCommandString)
				{
					//EBATECH(CNBR)-[
					//strcpy(fs1, "command.com"/*DNT*/);
					//pc = WinExec(fs1, SW_SHOWDEFAULT);
					//pc = SDS_OurWinExec(ResourceString(IDC_ICADAPI_INTELLICAD_SHEL_34, "IntelliCAD Shell" ), "command.com");
					strcpy( pCommandString, "command.com");
					//]- EBATECH(CNBR)
				}
				//else	EBATECH(CNBR)
				{
					CFile tfil;

					if (!tfil.Open(SDS_SHELLFILE,CFile::modeCreate|CFile::modeWrite))
					{
						sds_printf(ResourceString(IDC_ICADAPI_ERROR_USING_SHE_31, "Error using shell command." ));
						return 0;
					}

					if (NULL != Atmp->GName && 0 != *Atmp->GName)
					{
						tfil.Write(Atmp->GName, strlen(Atmp->GName));
						tfil.Write(" "/*DNT*/, 1);
					}

					tfil.Write(pCommandString, strlen(pCommandString));
					//EBATECH(CNBR)-[
					//tfil.Write(ResourceString(IDC_ICADAPI__R_NEXIT_33, "\r\nEXIT" ),6);
					tfil.Write("\r\n",2);
					tfil.Close();
					//pc=WinExec(SDS_SHELLFILE,SW_SHOWDEFAULT);
					//pc=CreateProcess("IntelliCAD Shell",SDS_SHELLFILE,SW_SHOWDEFAULT);
					pc = SDS_OurWinExec(ResourceString(IDC_ICADAPI_INTELLICAD_SHEL_34, "IntelliCAD Shell" ), SDS_SHELLFILE, SW_SHOWDEFAULT);
					pc = pc ? 32 : GetLastError();
					//EBATECH(CNBR)]-
				}

				if (pc > 31 || pc == 0)
					return 0;

				sds_printf(ResourceString(IDC_ICADAPI__NERROR__RUNNIN_35, "\nERROR: Running <%s>. System reports: " ),fs1);

				switch (pc)
				{
					case 0:
						sds_printf(ResourceString(IDC_ICADAPI_OUT_OF_MEMORY__36, "Out of memory." ));
						break;
					case ERROR_BAD_FORMAT:
						sds_printf(ResourceString(IDC_ICADAPI_BAD_FILE_FORMAT_37, "Bad file format." ));
						break;
					case ERROR_FILE_NOT_FOUND:
						sds_printf(ResourceString(IDC_ICADAPI_FILE_NOT_FOUND__38, "File not found." ));
						break;
					case ERROR_PATH_NOT_FOUND:
						sds_printf(ResourceString(IDC_ICADAPI_PATH_NOT_FOUND__39, "Path not found." ));
						break;
					default:
						sds_printf(ResourceString(IDC_ICADAPI_UNKNOWN_PROBLEM_40, "Unknown problem." ));
						break;
				}

				return 0;
			}
			else
			{
				// Just setup the alias and let fall through.
				if (Atmp->LName != NULL && *(Atmp->LName) != 0)
					strcpy(pCommandString, Atmp->LName);
				else if (Atmp->GName != NULL && *(Atmp->GName) != 0)
					strcpy(pCommandString, Atmp->GName);

				break;
			}
		}
	}

	pCommandAtom = lsp_findatom(pCommandString, 4);

	// If the above call fails, there are a few reasons that are related to the
	// prepending of the (.)dot and/or (_) underscore. Underscores are not the
	// problem, it's the dot. In an ideal world, the dot would only be used if
	// the user knows that the command being issued has been undefined. In that
	// case, there is no problem. However, nothing prevents a user form issuing
	// a command with a dot that has not been undefined. Here we will attempt to
	// resolve the command anyway.

	if (pCommandAtom == NULL)
	{
		// Consider the situation where the user has put both a dot and
		// an underscore on the string. In this case we simply need to make sure
		// the dot is first.
		char *fs2Ptr = pCommandString;
		bool hasDot=FALSE, hasUscore = FALSE, hasTrans = FALSE;

		while (*fs2Ptr == '\''/*DNT*/ || *fs2Ptr == '_'/*DNT*/ || *fs2Ptr == '.'/*DNT*/)
		{
			if (pTransparent && *fs2Ptr == '\''/*DNT*/)
				*pTransparent = true;
				//SDS_DoingCommandTrans = TRUE;

			if (*fs2Ptr == '_'/*DNT*/)
				hasUscore = TRUE;

			if (*fs2Ptr == '.'/*DNT*/)
				hasDot = TRUE;

			//EBATECH(R.Arayashiki) -[ for MBCS Support
			if ((_MBC_LEAD ==_mbbtype((unsigned char)*fs2Ptr,0)) &&
				  (_MBC_TRAIL==_mbbtype((unsigned char)*(fs2Ptr+1),1)))
				fs2Ptr ++;
			// ]-EBATECH
			fs2Ptr++;
		}

		char tmpPrefix[4] = {0,0,0,0}, tmpBuf[IC_ACADBUF];

		if (hasDot)
			_tcscat(tmpPrefix, "."/*DNT*/);

		if (hasUscore)
			_tcscat(tmpPrefix, "_"/*DNT*/);

		sprintf(tmpBuf, "%s%s", tmpPrefix, fs2Ptr);
		_tcscpy(pCommandString, tmpBuf);
		pCommandAtom = lsp_findatom(pCommandString, 4);

		// Now we'll see if the first character is a dot. If so we'll advance
		// the pointer and try again. It may have been issued to a command that has
		// NOT been undefined.
		if (pCommandAtom == NULL && pCommandString[0] == '.'/*DNT*/)
		{
			pCommandAtom = lsp_findatom(pCommandString + sizeof(TCHAR) * 1,4);

			// If pCommandLink is found, then fix pCommandString accordingly.
			if (pCommandAtom != NULL)
			{
				TCHAR tmp[IC_ACADBUF];
				_tcscpy(tmp, pCommandString + sizeof(TCHAR) * 1);
				_tcscpy(pCommandString, tmp);
			}
		}
		// If we haven't found it by now, we aren't going to.
	}

	return 1;
}

/*-------------------------------------------------------------------------*//**
Find global command name (without underscore).

@author	Martin Waardenburg
@param	pGlobalName		<=	global command name, without underscore
@param	pCommandString	<=> command string

*//*--------------------------------------------------------------------------*/
static TCHAR	*findGlobalCommandName
(
TCHAR			*pCommandString
)
{
	TCHAR *pGlobalName = NULL;
	class commandAtomObj *pCommandAtom = NULL;

	if (findCommandAtom(pCommandAtom, pCommandString) && NULL != pCommandAtom)
	{
		pGlobalName = pCommandAtom->GName;

		if (NULL != pGlobalName && '_' == *pGlobalName)
			pGlobalName += sizeof(TCHAR);
	}

	return pGlobalName;
}



/********************************************************************************
 * Author:	Mohan Nayak.														*
 *																				*
 * Purpose:	Set the toolbar combo box values to current styles.					*
 *																				*
 * Returns:	void																*
 ********************************************************************************/
void SetToolBarToCurrentStyle()
{
	int nSel=0;
	resbuf rb;

	if (!SDS_CMainWindow)
		return;

	if (sds_getvar("CLAYER"/*DNT*/, &rb) == RTNORM)
	{
		if(SDS_CMainWindow->m_wndLayers.GetSafeHwnd())
		{
			nSel = SDS_CMainWindow->m_wndLayers.SelectString(0,rb.resval.rstring);
			SDS_CMainWindow->m_wndLayers.SetCurSel(nSel);
		}

		IC_FREE(rb.resval.rstring);
	}

	// Modified by Sachin Dange to solve the bugs Related To "Color Control" ....[
	
	if(SDS_CMainWindow->m_wndColor.GetSafeHwnd())
	{
		SDS_CMainWindow->m_wndColor.UpdateColorList();  

		// Commented Part is no longer needed now as this part is now present 
		// in newly added function "UpdateColorList" of CColorComboBox class.

		/*
		if (sds_getvar("CECOLOR"/*DNT* /, &rb) == RTNORM)
		{
			if (SDS_CMainWindow->m_wndColor.GetSafeHwnd())
			{
				int nColor = ic_colornum(rb.resval.rstring);
				CString str=ic_colorstr(nColor,NULL);
				nSel = SDS_CMainWindow->m_wndColor.FindString(-1,str);
				if(nSel!=-1)
					SDS_CMainWindow->m_wndColor.SetCurSel(nSel);
				else
				{
					str.Format("Color %s",rb.resval.rstring);
					nSel=SDS_CMainWindow->m_wndColor.FindString(0,str);
					if(nSel!=-1)
						SDS_CMainWindow->m_wndColor.SetCurSel(nSel);
				}
				
			}
			IC_FREE(rb.resval.rstring);
		}*/
	}
	// ....] Modifications end here.
	
	if (sds_getvar("CELTYPE"/*DNT*/, &rb) == RTNORM)
    {
        if (SDS_CMainWindow->m_wndLType.GetSafeHwnd() && 
		   (nSel = SDS_CMainWindow->m_wndLType.FindString(0, rb.resval.rstring)) != CB_ERR)
        {
			SDS_CMainWindow->m_wndLType.SetCurSel(nSel);
        }
		IC_FREE(rb.resval.rstring);
    }

	if (sds_getvar("CELWEIGHT"/*DNT*/, &rb) == RTNORM)
	{
	   if (SDS_CMainWindow->m_wndLWeight.GetSafeHwnd())
		  SDS_CMainWindow->m_wndLWeight.SetCurSel( db_lweight2index(rb.resval.rint) + 3 );
	   	
	}

	SDS_getvar(NULL,DB_QDIMSTYLE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (SDS_CMainWindow->m_wndDimstyle.GetSafeHwnd())
	{
		nSel = SDS_CMainWindow->m_wndDimstyle.SelectString(0,rb.resval.rstring);
		SDS_CMainWindow->m_wndDimstyle.SetCurSel(nSel);
	}
	IC_FREE(rb.resval.rstring);	
	
}

/*----------------------------------------------------------------------------*/
void SDS_cmdthread( void )
{
	char fs1[IC_ACADBUF],fs2[IC_ACADBUF];
	sds_point pt1,pt2;
	sds_point tpt1,tpt2;
	sds_name ss,ename;
	int ret,osmode,grips,hl,nonemode;
	long fl1,sslen;
	db_drawing *pUndoDp;
	struct resbuf rb;
	CIcadDoc *pDoc=SDS_CURDOC;

	if (SDSApplication::GetEngineApplication() == NULL)
		return;

#if !defined(DEBUG)
	try {
		_set_se_translator(SDS_FatalErrorFunc);
#endif

		//BugZilla No. 78098  ; 03-03-2003
		// DP: give to command thread increased priority so 
		//that it processes all his jobs/events at once
		::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_HIGHEST);


		// Only do the SDS_AutoLoad() for the main thread!
		// Don't call it for applications command lines.
		// And only load for the maincommand thread NOT the lisp command thread.
		//
		if (SDSApplication::GetEngineApplication()->GetNextLink()==NULL &&
			SDSApplication::GetActiveApplication()->GetMainThread()->IsCurrent() )
			SDS_AutoLoad();

		for (;;) {

			if (SDSApplication::GetEngineApplication() == NULL)
				break;

			// If the user breaks and we are running a script.
			// Save the rest of the command buffer so the resume command will work.
			if (GetActiveCommandQueue()->IsNotEmpty() && GetScriptQueue()->IsNotEmpty() && sds_usrbrk())
				SDS_CancelAllPending(1);

			if (SDS_CMainWindow->m_bExpInsertState)
				ExecuteUIAction( ICAD_WNDACTION_SHOWEXP );

			if (SDS_LispPCount && GetActiveCommandQueue()->IsEmpty() && GetMenuQueue()->IsEmpty())
				sprintf(fs1,"Missing: %d) > ",SDS_LispPCount);
			else {
				SDS_getvar(NULL,DB_QCMDLNTEXT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				sprintf(fs1,"\n%s "/*DNT*/,rb.resval.rstring);
				IC_FREE(rb.resval.rstring);
			}

			if (SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() )
				SDS_AtCmdLine=1;

			if (SDS_CURDWG) {
				SDS_getvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				osmode=rb.resval.rint;
				rb.resval.rint=0;
				SDS_setvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
			}

			sds_initget(SDS_RSG_OTHER|SDS_RSG_NOLIM,NULL);
			SDS_SetCursor(SDS_GetPickCursor());
			if(SDS_CURDWG) {
				rb.resval.rint=osmode;
				SDS_setvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
			}

			nonemode=0;

			if (GetActiveCommandQueue()->IsNotEmpty() &&
				SDSApplication::GetActiveApplication()->IsCommandRunning()) {
				SDS_CMainWindow->m_IgnoreLastCmd=TRUE;
			}

			if (GetActiveCommandQueue()->IsNotEmpty() || GetMenuQueue()->IsNotEmpty() || lsp_cmdhasmore)
				SDS_WasFromPipeLine=TRUE;
			else
				SDS_WasFromPipeLine=FALSE;

			ret=SDS_AskForPoint( NULL,fs1,pt1 );

			// If we are exiting, just wait...
			//
			if (IDS_IsIcadExiting()) {
				while( true ) {
					MSG msg;

					// process any and all windows messages
					//
					while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
						TranslateMessage( &msg );
						DispatchMessage( &msg );
					}
				}
			}

			SDS_SetCursor(IDC_ICAD_CROSS);
			if ( SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() )
				SDS_AtCmdLine=0;

			*fs1=0;

			switch(ret) {
			case RTNONE:
				if (SDS_CmdHistCur==NULL)
					strcpy(fs2,"?"/*DNT*/);
				else
					strncpy(fs2,SDS_CmdHistCur->cmd,sizeof(fs2)-1);

				nonemode=1;
				break;

			case RTCAN:
			case RTERROR:
				// 	SDS_CancelAllPending(0);
				lsp_freepend();
				SDS_LispPCount=0;
				if (GetActiveCommandQueue()->IsEmpty() && GetMenuQueue()->IsEmpty()) {
					if (SDS_CURDOC) {

						// If there are selected entities, free them.
						// This both unhighlights them removes the selection set.
//4M Item:25->
// Replacement of SDS_NodeList with SDS_NodeList_New
/*
						if (SDS_CURDOC->m_nodeList.m_beg != NULL) {
*/
						if (SDS_CURDOC->m_nodeList.GetNoEntities()>0) {
//<-4M Item:25
//4M Item:25->
// Replacement of SDS_NodeList with SDS_NodeList_new
/*
								struct SDS_NodeListDef *tmp = SDS_CURDOC->m_nodeList.m_beg;
								while(tmp)
									{
									// Unhilite this entity.
									sds_redraw(tmp->ename,IC_REDRAW_UNHILITE);
									tmp = tmp->next;
									}
*/
							SDS_CURDOC->m_nodeList.begin();
							sds_name selected_ename;
							while (SDS_CURDOC->m_nodeList.getEntityName(selected_ename)){
								sds_redraw(selected_ename,IC_REDRAW_UNHILITE);
							}
//<-4M Item:25
							// NULL the nodelist.
//							SDS_CURDOC->m_nodeList.removeAll();			/*D.G.*/// SDS_FreeNodeList does this.
							SDS_FreeNodeList(SDS_CURDOC);
							sds_ssfree(SDS_CURDOC->m_pGripSelection);	/*D.G.*/// Free selection set.
						}
						else {
							// Else if there are entities gripped, do a redraw
							// to ungrip them, Then kill the grip selection set.
							if (SDS_CURDOC->m_pGripSelection[0] != 0 && SDS_CURDOC->m_pGripSelection[1] != 0) {
								// Get rid of all the grips, redraw to undraw them,
								// then free the selection set.
								sds_redraw(NULL,4);
								sds_ssfree(SDS_CURDOC->m_pGripSelection);
							}
						}
					}

					if (NULL!=SDS_CURVIEW && NULL!=SDS_CURVIEW->m_pSelection)
						ExecuteUIAction( ICAD_WNDACTION_UNSELOLEITEMS );
					SetToolBarToCurrentStyle();
				}
				continue;

				// END OF case RTCAN:
				// **************

			case RTNORM:
				SDS_getvar(NULL,DB_QGRIPS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				grips=rb.resval.rint;
				if ((pDoc=SDS_CURDOC)==NULL)
					continue;


				if (grips) {
					gr_ucs2rp(pt1,pt2,SDS_CURGRVW);
					sds_point cen,p1,p2;
					double fr1;

					SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					fr1=rb.resval.rreal;
					SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					fr1/=rb.resval.rpoint[1];
					SDS_getvar(NULL,DB_QGRIPSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					fr1*=rb.resval.rint;

					sds_name entityName;
					sds_point gripCenPoint;
					pDoc->m_nodeList.begin();
//4M Spiros Item:27->
					int Fnd=0;
					sds_name Ename[500],tmpEnt;
					sds_point gripCen[500],gp1,gp2;
					GripRClickedFlag=0;
					SDS_EntBeingDragged[0]=0;
					SDS_GripDragging=0;

					double MinGripDistance=1E10; //Huge Distance
					sds_point GripCenter;
//4M Spiros Item:27<-
					bool bIsAcisObject = false;
					while ( pDoc->m_nodeList.getPair( entityName, gripCenPoint ) ) {
						gr_ucs2rp(gripCenPoint,cen,SDS_CURGRVW);
//4M Item:27->
						double dist=sds_distance(pt2,cen);
						if (dist<MinGripDistance){
							MinGripDistance=dist;
							ic_ptcpy(GripCenter,cen);
						}
//<-4M Item:27
						p1[0]=cen[0]-fr1; p1[1]=cen[1]+fr1; p1[2]=cen[2];
						p2[0]=cen[0]+fr1; p2[1]=cen[1]-fr1; p2[2]=cen[2];

						if (p1[0]<pt2[0] && p1[1]>pt2[1] && p2[0]>pt2[0] && p2[1]<pt2[1]) {
//4M Spiros Item:27->
/*
							sds_name e2grip;
							ic_encpy(e2grip,entityName);
							if (RTERROR!=SDS_CallUserCallbackFunc(SDS_CBGRIPEDITBEG,(void *)e2grip,(void *)gripCenPoint,NULL)) {
								SDS_SetUndo(IC_UNDO_COMMAND_BEGIN,(void *)"Grip edit",NULL,NULL,SDS_CURDWG);
											 SDS_EntNodeDrag(pDoc,e2grip,gripCenPoint);
								SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)"Grip edit",NULL,NULL,SDS_CURDWG);
											 SDS_CallUserCallbackFunc(SDS_CBGRIPEDITEND,(void *)e2grip,(void *)gripCenPoint,NULL);
							}
							goto out;
*/
							// Note: the same entity with the same picked point can be inserted
							// into array multiple times in the certain case(i.e. when the entity
							// is ACIS entity), while can cause the problems so we make sure they
							// don't exist in arrays before we add them. This cehck is added to
							// fix a bug.
							//
							bool bFound = false;
							db_handitem *elp=(db_handitem *)entityName[0];
							for (register k = 0; k < Fnd; k++) {
								if (Ename[k][0] == entityName[0]) {
									if (elp->ret_type() == DB_3DSOLID ||
										elp->ret_type() == DB_REGION ||
										elp->ret_type() == DB_BODY) {
										bIsAcisObject = true;
										bFound = true;
										break;
									}
									else {
										if (icadPointEqual(gripCen[k],gripCenPoint)) {
											bFound = true;
											break;
										}
									}
								}
							}
							if (!bFound) {
								ic_encpy(Ename[Fnd],entityName);
								ic_ptcpy(gripCen[Fnd],gripCenPoint);
								Fnd++;
							}
//<-4M Spiros Item:27
						}
					} // while
//4M Spiros Item:27->
					if (Fnd) {
						SDS_GripRClicked[0]=GripCenter[0];
						SDS_GripRClicked[1]=GripCenter[1];

						GripRClickedFlag=1;
						if (Fnd==1 && !bIsAcisObject) {
							if (RTERROR!=SDS_CallUserCallbackFunc(SDS_CBGRIPEDITBEG,(void *)Ename[0],(void *)gripCen[0],NULL)) {
								SDS_SetUndo(IC_UNDO_COMMAND_BEGIN,(void *)"Grip edit"/*DNT*/,NULL,NULL,SDS_CURDWG);
								SDS_EntBeingDragged[0]=Ename[0][0];
								SDS_EntBeingDragged[1]=Ename[0][1];
								SDS_GripDragging=1;
								SDS_EntNodeDrag(pDoc,Ename[0],gripCen[0]);
								SDS_GripDragging=0;
								SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)"Grip edit"/*DNT*/,NULL,NULL,SDS_CURDWG);
								SDS_CallUserCallbackFunc(SDS_CBGRIPEDITEND,(void *)Ename[0],(void *)gripCen[0],NULL);
							}
						}
						else {
							SDS_CallUserCallbackFunc(SDS_CBGRIPEDITBEG,(void *)Ename[0],(void *)gripCen[0],(void *)TRUE); //Autodsys 051304
							SDS_SetUndo(IC_UNDO_COMMAND_BEGIN,(void *)"Grip edit"/*DNT*/,NULL,NULL,SDS_CURDWG);
							sds_resbuf rbucs,rbwcs;
							sds_resbuf* rbp=NULL;
							rbucs.restype=rbwcs.restype=RTSHORT; rbucs.rbnext=rbwcs.rbnext=NULL;
							rbucs.resval.rint=1; rbwcs.resval.rint=0;
							sds_trans(pt1,&rbucs,&rbwcs,0,gp1);
							ic_ptcpy(gp2,gp1);
							gp2[0]+=0.000001;
							gp2[1]+=0.000001;

							CString ltp("DASHED2");

							// EBATECH(CNBR) -[ memory leak 08/06/2003
							if ((rbp = sds_tblsearch("LTYPE"/*DNT*/,ltp,0))==NULL)
							{
								ltp="DOT";
								if ((rbp = sds_tblsearch("LTYPE"/*DNT*/,ltp,0))==NULL)
								{
									ltp="BYLAYER";
								}
							}
							IC_RELRB( rbp );
							// EBATECH(CNBR) ]-


							struct resbuf undoctl;
    						SDS_getvar(NULL,DB_QUNDOCTL,&undoctl,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						int iundo = undoctl.resval.rint;
    						if (undoctl.resval.rint & IC_UNDOCTL_ON)
    							{
    							undoctl.resval.rint &= ~IC_UNDOCTL_ON;  // turn Undo off
    							sds_setvar("UNDOCTL",&undoctl);
    							}

							struct resbuf *list = sds_buildlist(RTDXF0,  "LINE", 10, gp1, 11, gp2, 8,"0",62,256, 6, ltp, RTNONE);
							sds_entmake(list);
							sds_relrb(list);
							sds_entlast(tmpEnt);

							SDS_GripPlaced=0;
							SDS_AtDragMulti=1;
							bool TookP=1;
							SDS_GripDragging=1;
							SDS_DragNOElevation=true; //we don't want to change the elevation due to UCS difference

							if (SDS_EntNodeDrag(pDoc,tmpEnt,gp2)!=RTNORM)
								TookP=0;

							SDS_GripDragging=0;
							SDS_GripPlaced=1;
							SDS_AtDragMulti=0;
							SDS_AddGripNodes(pDoc,tmpEnt,-1);
//4M Item:71->
							if (TookP)
//<-4M Item:71
								SDS_AddGripNodes(pDoc,tmpEnt,0);
//4M Item:28->
							SDS_DrawGripNodes(pDoc);
//<-4M Item:28
							sds_entdel(tmpEnt);

    							undoctl.resval.rint = iundo;   // return Undo settings
    							sds_setvar("UNDOCTL", &undoctl);

							if (TookP) for(int fi=0;fi<Fnd;fi++)
								SDS_EntNodeDrag(pDoc,Ename[fi],gripCen[fi]);

							SDS_DragNOElevation=false; //back to defaults...
							SDS_GripPlaced=0;

							SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)"Grip edit"/*DNT*/,NULL,NULL,SDS_CURDWG);
							SDS_CallUserCallbackFunc(SDS_CBGRIPEDITEND,(void *)Ename[0],(void *)gripCen[0],(void *)TRUE);//Autodsys 051304
						}

//BugZilla No. 78005; 09-12-2002
                     GripRClickedFlag=0;
						goto out;
					}
//<-4M Spiros Item:27
				} // end of if (grips)

				bool bOneEntityPicked;	// we start dragging a SS if this flag is "true"
				bOneEntityPicked = true;
				if (RTNORM!=(ret=sds_pmtssget(NULL,NULL,pt1,NULL,NULL,ss,0)) ||
					RTNORM!=(ret=sds_ssname(ss,0L,ename))) {

					bOneEntityPicked = false;
					SDS_SetCursor(IDC_ICAD_WINDOW);
					SDS_BoxModeCursor=1;
					sds_initget(SDS_RSG_NOVIEWCHG,NULL);
					ret=sds_getcorner(pt1,ResourceString(IDC_ICADSELECTDIA__NOPPOSIT_31, "\nOpposite corner: " ),pt2);
					SDS_BoxModeCursor=0;

					if (ret!=RTNORM)
						continue;

					SDS_SetCursor(IDC_ICAD_CROSS);
					gr_ucs2rp(pt1,tpt1,SDS_CURGRVW);
					gr_ucs2rp(pt2,tpt2,SDS_CURGRVW);

					if (RTNORM!=sds_pmtssget(NULL,(tpt2[0]>tpt1[0]) ? "WINDOW"/*DNT*/ : "CROSSING"/*DNT*/,pt1,pt2,NULL,ss,0))
						continue;
				} // end of if(RTNORM!=...

				sslen=fl1=0L;
				sds_sslength(ss,&sslen);
				SDS_getvar(NULL,DB_QPICKADD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if (sslen>0L && !rb.resval.rint && SDS_CURDOC) {
					if (grips)
						SDS_FreeNodeList(SDS_CURDOC);

					sds_ssfree(SDS_CURDOC->m_pGripSelection);
				}

				SDS_DontBitBlt=1;
				SDS_getvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				hl=rb.resval.rint;

				/*DG - 12.6.2003*/// We should redraw ents after their grips unpainting in SDS_DrawGripNodes.
				sds_name	ssToRedraw;
				ssToRedraw[0] = ssToRedraw[1] = 0L;
				/*DG - 25.3.2003*/// Temporarily disable sending callbacks for the SS change
				// and send one SDS_CB_SSMOD after the loop.
				// This is necessary because of performance problems which can occur
				// if SDS_CB_SSADD and/or SDS_CB_SSDEL notifications are sent many times.
				bool	bCallSSchangeCB;
				bCallSSchangeCB = pDoc->m_bCallSSchangeCB;
				pDoc->m_bCallSSchangeCB = false;
				while (pDoc && RTNORM==sds_ssname(ss,fl1,ename)) {
					if (sds_ssmemb(ename,pDoc->m_pGripSelection)==RTNORM) {
						if (bOneEntityPicked)
							SDS_StartDraggingSS(pt1);
						sds_ssdel(ename,pDoc->m_pGripSelection);
						if (grips) {
							//paint out grips and invalidate them in the llist
							SDS_AddGripNodes(pDoc,ename,0);
							SDS_AddGripNodes(pDoc,ename,-1);
	 						SDS_ssadd(ename, ssToRedraw, ssToRedraw);
						}
						else {
							if (hl)
								sds_redraw(ename,IC_REDRAW_DRAW);
						}
					}
					else {
 						SDS_ssadd(ename, pDoc->m_pGripSelection, pDoc->m_pGripSelection);
						if (grips)
							SDS_AddGripNodes(pDoc,ename,1);
						else {
							if (hl)
								sds_redraw(ename,IC_REDRAW_HILITE);
						}
//						if (bOneEntityPicked)			/*D.G.*/// Uncomment this for start dragging after just one click.
//							SDS_StartDraggingSS(pt1);
					}
					++fl1;
				} // end of while(pDoc && ...
				pDoc->m_bCallSSchangeCB = bCallSSchangeCB;
				if(bCallSSchangeCB)
					SDS_CallUserCallbackFunc(SDS_CBSSCHANGE, (void*)SDS_CB_SSMOD, NULL, NULL);

//4M Item:28->
				SDS_DrawGripNodes(pDoc);
//<-4M Item:28
				for(fl1 = 0L; sds_ssname(ssToRedraw, fl1, ename) == RTNORM; ++fl1)
					sds_redraw(ename, IC_REDRAW_DRAW);
				sds_ssfree(ssToRedraw);

				//Zhifeng.Cheng :Add Code to deal with Entity property toolbar
				{
					char *pLayer=NULL,*pLType=NULL,*pColor=NULL, *pLWeight=NULL,*pDimstyle=NULL,*pEname=NULL;
					int bIsLA  = 1, 
						bIsLT  = 1, 
						bIsCo  = 1, 
						bIsLW  = 1, 
						nColor = -1, 
						nSel   = -1, 
						nLWeight = -4,
						
						nCheckLType = 0,
					
						nStatus =1,
						nCheck = 0;


					resbuf rbLw; 

					for(fl1 = 0L; sds_ssname(pDoc->m_pGripSelection, fl1, ename) == RTNORM; ++fl1)
					{
						resbuf *rb = sds_entget(ename);
						if (rb == NULL)
							continue;

						ic_assoc(rb,0);
						nCheck = 1;
						pEname=strdup(ic_rbassoc->resval.rstring);
						if(strisame(pEname,"DIMENSION"/*DNT*/) || strisame(pEname,"LEADER"/*DNT*/))
						 {
							 if(nStatus)
							 {
								 ic_assoc(rb,3);
								 if(!pDimstyle)
									 pDimstyle = strdup(ic_rbassoc->resval.rstring);
								 else
								 {
									 if(!strisame(pDimstyle,ic_rbassoc->resval.rstring))
										 nStatus = 0;
								 }

							 }
						 }
						 else
							 nStatus = 0;

						ic_assoc(rb,8);
						if(!pLayer)
							pLayer=strdup(ic_rbassoc->resval.rstring);
						else
						{
							if(bIsLA && !strisame(pLayer,ic_rbassoc->resval.rstring))
								bIsLA=0;
						}
						
						nCheckLType = 1;
						ic_assoc(rb,6);
						if(!pLType)
							pLType=strdup(ic_rbassoc->resval.rstring);
						else
						{
							if(bIsLT && !strisame(pLType,ic_rbassoc->resval.rstring))
								bIsLT=0;
						}
						ic_assoc(rb,62);
						if(nColor==-1)
							nColor=ic_rbassoc->resval.rint;
						else
						{
							if(bIsCo && nColor!=ic_rbassoc->resval.rint)
								bIsCo=0;
						}

						ic_assoc(rb,370); 
						if(nLWeight == -4)
							nLWeight = ic_rbassoc->resval.rint;
						else
						{
							if(bIsLW && nLWeight != ic_rbassoc->resval.rint)
								bIsLW = 0;
						}

						IC_RELRB(rb);
						if(!bIsLA && !bIsLT && !bIsCo)	//如果三个均不一致,我们直接退出,不需要再循环
							break;
					}
					if (SDS_CMainWindow->m_wndLayers.GetSafeHwnd())
					{
						nSel=bIsLA ? SDS_CMainWindow->m_wndLayers.FindString(-1,pLayer):-1;
						SDS_CMainWindow->m_wndLayers.SetCurSel(nSel);
					}

					if (SDS_CMainWindow->m_wndLType.GetSafeHwnd())
					{
						if (!nCheckLType)
						{
							struct resbuf rbLType;
							SDS_getvar(NULL, DB_QCELTYPE, &rbLType, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
							CString strCLType(rbLType.resval.rstring);
							IC_FREE(rbLType.resval.rstring);
							// Select active linetype
							SDS_CMainWindow->m_wndLType.SetCurSel(SDS_CMainWindow->m_wndLType.FindString(-1, strCLType));
						}
						else
						{
							nSel=bIsLT ? SDS_CMainWindow->m_wndLType.FindString(-1,pLType):-1;
							SDS_CMainWindow->m_wndLType.SetCurSel(nSel);
						}

						SDS_CMainWindow->m_wndLType.UpdateWindow();
					}

					if (SDS_CMainWindow->m_wndColor.GetSafeHwnd())
					{
						if(nCheck)
						{
							CString str=ic_colorstr(nColor,NULL);
							nSel=bIsCo? SDS_CMainWindow->m_wndColor.FindString(-1,str):-1;
							if(nSel==-1 && bIsCo)
							{
								str.Format("Color %d",nColor);
								nSel=SDS_CMainWindow->m_wndColor.FindString(-1,str);
								if(nSel==-1)
								{//总是把选择颜色摆到最后一个
									int nCount=SDS_CMainWindow->m_wndColor.GetCount();
									nSel=SDS_CMainWindow->m_wndColor.InsertString(nCount-1,str);
									SDS_CMainWindow->m_wndColor.SetItemData(nSel,SDS_BrushColorFromACADColor(nColor));
								}
							} 
							SDS_CMainWindow->m_wndColor.SetCurSel(nSel);
							SDS_CMainWindow->m_wndColor.UpdateWindow();
						}
						else
							SDS_CMainWindow->m_wndColor.UpdateColorList(); 
						
					}
					if(nLWeight == -4)
					{
						sds_getvar("CELWEIGHT"/*DNT*/,&rbLw);
						nLWeight = rbLw.resval.rint;
					}

					if (SDS_CMainWindow->m_wndLWeight.GetSafeHwnd())
					{
						nSel = bIsLW ? SDS_CMainWindow->m_wndLWeight.FindLWeight(nLWeight) : -1;
						SDS_CMainWindow->m_wndLWeight.SetCurSel(nSel);
						SDS_CMainWindow->m_wndLWeight.UpdateWindow();
					}
					
					if (SDS_CMainWindow->m_wndDimstyle.GetSafeHwnd())
					{
						if(nStatus)
						{
							if(nCheck)
							nSel = SDS_CMainWindow->m_wndDimstyle.SelectString(0,pDimstyle);  
							else
							{
								resbuf rb;
								SDS_getvar(NULL,DB_QDIMSTYLE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
								nSel = SDS_CMainWindow->m_wndDimstyle.SelectString(0,rb.resval.rstring);
								IC_FREE(rb.resval.rstring);	
							}

						}
						else
							nSel = -1;

						SDS_CMainWindow->m_wndDimstyle.SetCurSel(nSel);
						SDS_CMainWindow->m_wndDimstyle.UpdateWindow(); 
					}

					free(pLayer);
					free(pLType);
					free(pDimstyle);
					free(pEname);

				}
				//Add End.
				SDS_BitBlt2Scr(1);
				SDS_DontBitBlt=0;

				sds_ssfree(ss);
				out: ;
				continue;

				// END OF case RTNORM:
				// ******************

			case RTKWORD:
				SDS_LastCmdWasHatch=FALSE;
				sds_getinput(fs1);
				break;
			} // end of switch()


			char	*pGlobalName = findGlobalCommandName(fs1);	/*D.G.*/// This function changes fs1 string
																// removing symbols . and '. And we will use this changed for further processing.
			char	globalName[100];	/*D.G.*/// We cannot use pGlobalName which directly points to the lisp atoms list after executing the command
											// because that list can be destroyed while executing the command,
											// so, we copy it to this array (which is static for performance).
											// Note, it's not static because of thread-safety.

			if(pGlobalName)
				strcpy(globalName, findGlobalCommandName(fs1));
			else
				globalName[0] = '\0';

			// TODO These command name ptrs should be static strings to save
			//		The allocation of memory.
			if(!SDS_InExplorer)		/*D.G.*/// We check it on command_end, so check it here too.
			{
				if(nonemode)
					SDS_SetUndo(IC_UNDO_COMMAND_BEGIN, (void*)fs2, NULL, NULL, pUndoDp = SDS_CURDWG);
				else
					SDS_SetUndo(IC_UNDO_COMMAND_BEGIN, (void*)(pGlobalName ? pGlobalName : fs1),	/*D.G.*/
								NULL, NULL, pUndoDp = SDS_CURDWG);
			}

			try
			{
				if (strnisame(fs1,"(command "/*DNT*/,9) &&
					strrchr(fs1,'('/*DNT*/)==fs1 &&
					SDSApplication::GetActiveApplication()==SDSApplication::GetEngineApplication())
				{
					// Here is an odd one.
					// We have to save the command buffer because we are doing (command),
					// Then the save command buffer has to be appended onto what the (command)
					// function has added to the buffer.  This is so the (command) gets
					// executed FIRST!	Yuck!!!
					IDS_GetThreadController()->SetDontThreadSwapCmd( true );

					// save the state of the command queue
					CommandQueue temporaryCommandBuffer;
					temporaryCommandBuffer.Copy( GetActiveCommandQueue() );
					GetActiveCommandQueue()->Flush();

					SDS_DoOneCommand(fs1,0);

					// Add the old command queue after anything entered by the command
					GetActiveCommandQueue()->Concat( &temporaryCommandBuffer );
				}
				else
				{
					if ((pDoc=SDS_CURDOC)!=NULL)
					{
						if (NULL != pGlobalName && strisame(pGlobalName,"SELGRIPS"/*DNT*/))
						{
//4M Item:25->
					 pDoc->m_nodeList.begin();
					 sds_name selected_ename;
					 while (pDoc->m_nodeList.getEntityName(selected_ename)){
								sds_redraw(selected_ename, IC_REDRAW_UNHILITE);
					 }
//<-4M Item:25

							SDS_FreeNodeList(pDoc);
							sds_ssfree(pDoc->m_pGripSelection);
						}
					}

					SDS_DoOneCommand(fs1,0);
				}

				SetToolBarToCurrentStyle();
			}
			// If exception raised, then we should add IC_UNDO_COMMAND_END bracket before leaving this block
			catch (SDSThreadException& theException)
			{
				//Modify by SMR for orthomode debugging. 2001/5/27
				if (SDS_IgnoreMenuWQuote == 1) SDS_IgnoreMenuWQuote = 0;
				//End of modify. SMr 2001/5/28

				if(!SDS_InExplorer)
					if (nonemode)
						SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)fs2,NULL,NULL,pUndoDp=SDS_CURDWG);
					else
						SDS_SetUndo(IC_UNDO_COMMAND_END, (void*)(*globalName ? globalName : fs1),	/*D.G.*/
									NULL, NULL, pUndoDp = SDS_CURDWG);

				throw theException;
			}
			// End of exception block

			IDS_GetThreadController()->SetDontThreadSwapCmd( false );

			if(nonemode)
				strcpy(fs1,fs2);

			if (*globalName && strisame(globalName,"HATCH"/*DNT*/) && cmd_HatchedOK)
				SDS_LastCmdWasHatch=TRUE;
			else
				SDS_LastCmdWasHatch=FALSE;

			if (*globalName &&
				(strisame(globalName,"CLOSE"/*DNT*/) ||
				strisame(globalName,"CLOSEALL"/*DNT*/) ||
				strisame(globalName,"WCLOSEALL"/*DNT*/) ||
				strisame(globalName,"WCLOSE"/*DNT*/) ||
				strisame(globalName,"OPEN"/*DNT*/)) &&
				SDS_CURDWG!=pUndoDp)
			{
				pUndoDp = NULL;
			}
			else
			{
				if (*globalName && strisame(globalName,"DXFIN"/*DNT*/) && SDS_CURDWG!=pUndoDp)
					pUndoDp=SDS_CURDWG;
				else if (SDS_CURDWG!=pUndoDp && !SDS_ISVALIDDRAWING(pUndoDp))
					pUndoDp=NULL;	// To avoid using pUndoDb in case our command changes current DOC
									// using COM interface. Example - IIcadDoc::close()
			}

			if(!SDS_InExplorer)
				SDS_SetUndo(IC_UNDO_COMMAND_END,(void*)(*globalName ? globalName : fs1),	/*D.G.*/
							NULL, NULL, pUndoDp);

			// AG: I removed the following block from the end of SDS_DoOneCommand() to allow
			// LISP thread to set IC_UNDO_COMMAND_END undo bracket during the processing of the same command

			// If we are back at the commad line and the applications command
			// line is still running swap back, to the MAIN command line.
			// Scary stuff!!!!!	 FIX FOR (defun c:xx () (command "arc" "2,2"))
			if( SDSApplication::GetActiveApplication()->GetCommandThread()->IsCurrent() &&
				SDSApplication::GetActiveApplication()->GetLispCommandDepth()==0 &&
				!IDS_GetThreadController()->GetInsideCmdFunction() &&
				!SDS_DoingLispCode && !SDS_ReleasedLinkFunc && !SDS_Inside3rdPartyFunc)
			{
				// ****************************************
				// THREADSWAP !!!!!!!!!!!!!!!!!
				{
				SDSApplication *pStartApp = SDSApplication::GetActiveApplication();
				SDSApplication *pStopApp = SDSApplication::GetActiveApplication();
				SDS_ThreadSwap(pStopApp->GetMainThread(),pStartApp->GetCommandThread());
				}
			}

			SDS_getvar(NULL,DB_QSAVETIME,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

			if((rb.resval.rint > 0) && SDSApplication::GetEngineApplication() != NULL &&
				(SDSApplication::GetEngineApplication()->GetMainThread()->IsSame( SDSApplication::GetActiveApplication()->GetMainThread() ) ) &&
				GetActiveCommandQueue()->IsEmpty() &&
				GetMenuQueue()->IsEmpty()
				&& !SDS_CMainWindow->m_bPrintPreview )//Bugzilla No 78121 06-05-2002
			{
				CTime ct(time(NULL));
				CTimeSpan ts(ct-(*SDS_SaveTime));
				int fi3;

				if (rb.resval.rint <= (fi3 = ts.GetTotalMinutes()))
				{
					sds_printf(ResourceString(IDC_ICADAPI_AUTOSAVE, "\nAuto saving open drawings..."));
					SDS_AutoSave=1;
					ExecuteUIAction( ICAD_WNDACTION_SAVEALL );
					SDS_AutoSave=0;
					delete SDS_SaveTime;
					SDS_SaveTime = new CTime(time(NULL));
				}
			}
		} // end of for(;;)

#if !defined(DEBUG)
	} // try
	catch(SDS_Exception e)
	{
		PostUIAction( ICAD_WNDACTION_EXIT,0L);
	}
#endif
} // end of SDS_cmdthread


/*----------------------------------------------------------------------------*/
short cmd_istransparent
(
void
)
	{

	//BugZilla No. 78050; 25-03-2002
	//if (SDSApplication::GetActiveApplication()->GetCommandDepth()>1 &&
	if ((SDSApplication::GetActiveApplication()->GetCommandDepth()>1 ||
		SDSApplication::GetActiveApplication()->GetLispCommandDepth() > 1) &&
		SDS_DoingCommandTrans)
		{
		cmd_ErrorPrompt(CMD_ERR_NOTRANSPARENT,0);
		return(RTERROR);
		}
	return(RTNORM);
	}


/*----------------------------------------------------------------------------*/

static void doLispCode(char* cmd, int pushlsp)
{
	if(cmd_iswindowopen() == RTERROR)
		return;

	static int		lisprec;

	resbuf			*targsll, *argres = NULL;
	lsp_pendlink	tpend;
	int				resetlsp = 0, lspret;

	// Save the current pending lisp and eval.
	if(!SDS_LispPCount && (lisprec || lsp_pend.expr))
	{
		targsll = lsp_argsll;
		lsp_argsll = NULL;
		tpend = lsp_pend;
		memset(&lsp_pend, 0, sizeof(lsp_pend));
		resetlsp = 1;
	}

	if(SDS_LispPCount && lsp_pend.expr && *lsp_pend.expr && lsp_pend.expr[strlen(lsp_pend.expr)-1] == '\n')
	{
		lsp_pend.expr[strlen(lsp_pend.expr)-1] = 0;
		--lsp_pend.exprlen;
	}

	// Modified CyberAge AP 10/18/2000 [
	if(LSP_ConvertToPoint != 2) // i.e. called by internalGetpoint(...)
		LSP_ConvertToPoint = 1; // Reason: Fix for bug no. 57282
	// Modified CyberAge AP 10/18/2000 ]
	SDS_DoingLispCode = 1;
	++lisprec;
	lspret = lsp_lispev(((*cmd == '!') ? cmd+1 : cmd), &argres, NULL, &SDS_LispPCount);
	--lisprec;

	SDS_DoingLispCode = 0;

	if(resetlsp)
	{
		if(!lspret && !SDS_LispPCount)
		{
			lsp_freesuprb(lsp_argsll);
			lsp_argsll = targsll;
			delete [] lsp_pend.expr;
			lsp_pend.expr = NULL;
			lsp_pend.exprsz = 0;
			lsp_pend = tpend;
		}
		else
		{
			delete [] tpend.expr;
			tpend.expr = NULL;
			tpend.exprsz = 0;
		}
	}

	if(!SDS_LispPCount && argres && !lspret)
	{
		// Make a copy of result and convert points.
		resbuf*	evres = sds_newrb(0);
		lsp_copysplrb(evres, argres);

		// Modified PK 31/05/2000 [
		if(LSP_ConvertToPoint || (pushlsp && pushlsp != -1)) // Reason: Fix for bug no. 57282
			lsp_convptlist(evres, 0);
		else
			LSP_ConvertToPoint = 1;
		// Modified PK 31/05/2000 ]

		if(!pushlsp && !g_lsp_bProtectedLisp)
			lsp_prtrecur(evres, NULL, 1, 0);

		// Push the lisp result onto the command stack.
		if(pushlsp && pushlsp != -1)
		{
			if(GetScriptQueue()->IsEmpty()) // NO script
			    GetActiveCommandQueue()->AddItem( evres );
			else
			// Modified ALM 20/06/2003 [ Fix for Bugzilla 78250
			GetActiveCommandQueue()->AddItemToFront( evres );
			// Modified ALM 20/06/2003 ]
		}
		else
			lsp_freesuprb(evres);
	}
	SDS_DoingLispCode = 0;
}


static void callInternalCommand(commandAtomObj* alink)
{
	/*DG - 31.3.2003*/// cmd_xxx functions return 'short', so using LSP_FNPCAST is not correct (although it works).
	//fnptr = LSP_FNPCAST(alink->rb.resval.rstring);
	short		(*fnptr)(void) = (short(*)())alink->rb.resval.rstring;
	CIcadDoc*	pDoc = SDS_CURDOC;

	do
	{
		// ONLY clear the selection set on internal commands
		// In fact probably only a subset of those, but this is better than
		// what we used to do, which is always clear it
		SDS_FreeNodeList(pDoc);
		{
			// This is used to set CMDACTIVE
			SafeRecursionCounter	theCounter(&SDS_InInternalFunction);

			// Pass the global command name to the callback function. Strip the '_'.
			char*	pGlobalNameWithoutUnderscore = alink->GName;

			if(pGlobalNameWithoutUnderscore && *pGlobalNameWithoutUnderscore == '_')
				pGlobalNameWithoutUnderscore += sizeof(TCHAR);

			// Check the user callback function.
			if(SDS_CallUserCallbackFunc(SDS_CBCMDBEGIN, (void*)pGlobalNameWithoutUnderscore, NULL, NULL) == RTNORM)
			{
//4M Spiros Item:27->
				if(GripRClickedFlag == 1)
					GripRClickedFlag = 2;
//4M Spiros Item:27<-
				SDS_UndoCmdName = alink->GName;
			
				(*fnptr)();
//4M Spiros Item:27->
				if(GripRClickedFlag)
					GripRClickedFlag = 0;
//4M Spiros Item:27<-

#if 0 				  
				// Without following changes Lisp command thread does not return control to 
				// the main engine thread after some commands called from lisp and which 
				// use dialog and input points or entities from drawing.
				// For example (command "group") in dialog \SelectEntitiesAndCreateGroup>  or 
				// 			(command "settings") in dialog \Display\Limits\Select>
				// FIX for: (defun c:xx () (setvar "cmddia" 1) (command "group") or (command "settings"))
				// 
				if( SDSApplication::GetActiveApplication()->GetCommandThread()->IsCurrent()
					&& SDS_DoingLispCode
					&& SDSApplication::GetActiveApplication()->GetCommandDepth() > 0
					&& SDSApplication::GetActiveApplication()->GetLispCommandDepth() > 0
					&& IDS_GetThreadController()->GetInsideCmdFunction() )
				{						  
					// THREADSWAP !!!!!!!!!!!!!!!!!
					SDSApplication*	pApp = SDSApplication::GetActiveApplication();
					SDS_ThreadSwap(pApp->GetMainThread(), pApp->GetCommandThread());
				}
#endif
				SDS_CallUserCallbackFunc(SDS_CBCMDEND, (void *) pGlobalNameWithoutUnderscore, NULL, NULL);
			}
		}

		// We redrew the selection set above, but don't free it until AFTER the command
		// so the command can actually operate on it.

		if( !!(pDoc = SDS_CURDOC) &&
			fnptr != cmd_selgrips && fnptr != cmd_pasteclip && fnptr != cmd_pastespec &&
			fnptr != cmd_rtpan && fnptr != cmd_rtzoom && fnptr != cmd_rtrotx &&
			fnptr != cmd_rtroty && fnptr != cmd_rtrotz && fnptr != cmd_rtrot )
			sds_ssfree(pDoc->m_pGripSelection);

		if(SDS_MultibleMode==2)
		{
			SDS_MultibleMode = 1;
			break;
		}
	}
	while(SDS_MultibleMode);
}


static int callSdsFunction(commandAtomObj* alink, int pushlsp, TCHAR* localCommandName)
{

	if(alink->HasFuncPtr())
	{
		// Call the callback type functions.
		SafeRecursionCounter	insideCounter(&SDS_Inside3rdPartyFunc);
		lsp_freesplrb(&lsp_evres);	// IMPORTANT: Free prev value.

		// Check the user callback function.
		if(SDS_CallUserCallbackFunc(SDS_CBCMDBEGIN, (void*)localCommandName, NULL, NULL) == RTNORM)
		{
			/*DG 2.10.2001*/// Remove IC_UNDO_COMMAND_BEGIN from undo for this sds function
			// (by pushing IC_UNDO_COMMAND_END) and push IC_UNDO_GROUP_BEGIN/IC_UNDO_GROUP_END brackets.
			// Reason: doing so we will push undo units which can appear in the function directly to the undo stack
			// but not collect them as children in the parent undo unit created by IC_UNDO_COMMAND_BEGIN.
			// Note: the real IC_UNDO_COMMAND_END which will be tried to push in SDS_cmdthread later
			// will be ignored using command_end_pending flag in newSDS_SetUndo.
			/*DG - 1.7.2003*/// Transparent commands don't create their undo unit, so no need to send the command_end bracket.
			if(!SDS_DoingCommandTrans)
				SDS_SetUndo(IC_UNDO_COMMAND_END, (void*)localCommandName, 0, 0, SDS_CURDWG);
			/*DG - 26.7.2002*/// acad doesn't create undo groups for sds funcs calls,
			// also this grouping makes using of undo marks inside sds funcs
			//SDS_SetUndo(IC_UNDO_GROUP_BEGIN, (void*)localCommandName, 0, 0, SDS_CURDWG);

			if(alink->IsVbaFunction())
			{
				SDS_CMainWindow->m_pvWndAction = (void*)alink;
				ExecuteUIAction(ICAD_WNDACTION_VBAEXECUTE);
			}
			else
				alink->CallFunction();

			/*DG - 26.7.2002*/// see comments just above again
			//SDS_SetUndo(IC_UNDO_GROUP_END, (void*)localCommandName, 0, 0, SDS_CURDWG);	/*DG 2.10.2001*/// See comments just above.

			SDS_CallUserCallbackFunc(SDS_CBCMDEND, (void*)localCommandName, NULL, NULL);
		}

		lsp_convptlist(&lsp_evres, 0);

		if(!pushlsp)
			lsp_prtrecur(&lsp_evres, NULL, 1, 0);

		// Make a copy of result
		resbuf*	evres = sds_newrb(0);
		lsp_copysplrb(evres, &lsp_evres);

		// Push the lisp result onto the command stack.
		if(pushlsp && pushlsp != -1)
			GetActiveCommandQueue()->AddItem(evres);
	}
	else
	{
		SDS_FunctionID = alink->id;
		SDSApplication*	ptempApp = SDSApplication::GetEngineApplication();

		// Find the thread this function belongs to.
		for( ;ptempApp; ptempApp = ptempApp->GetNextLink())
			if(ptempApp->GetMainThread()->IsSame(alink->ThreadID))
				break;

		// Better have found the thread.
		if(!ptempApp)
			return RTERROR;

		// We must be on the engine application.  If we are not it is probably because
		// the user-defined function is being called from sds_command, which is currently illegal (also
		// in AutoCAD).  See Bug 56944
		//
		if(!SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent())
			return RTERROR;

		ptempApp->GetCommandQueue()->Copy(SDSApplication::GetEngineApplication()->GetCommandQueue());
		SDSApplication::GetEngineApplication()->GetCommandQueue()->Flush();


		SDS_LoadedAppsLast = SDSApplication::GetEngineApplication();

		lsp_freesplrb(&lsp_evres);	// IMPORTANT: Free prev value.

		// Check the user callback function.
		if(SDS_CallUserCallbackFunc(SDS_CBCMDBEGIN, (void*)localCommandName, NULL, NULL) == RTNORM)
		{
			SafeRecursionCounter	insideCounter(&SDS_Inside3rdPartyFunc);

			SDS_ReleasedLinkFunc = TRUE;

			/*DG 2.10.2001*/// Remove IC_UNDO_COMMAND_BEGIN from undo for this sds function
			// (by pushing IC_UNDO_COMMAND_END) and push IC_UNDO_GROUP_BEGIN/IC_UNDO_GROUP_END brackets.
			// Reason: doing so we will push undo units which can appear in the function directly to the undo stack
			// but not collect them as children in the parent undo unit created by IC_UNDO_COMMAND_BEGIN.
			// Note: the real IC_UNDO_COMMAND_END which will be tried to push in SDS_cmdthread later
			// will be ignored using command_end_pending flag in newSDS_SetUndo.
			/*DG - 1.7.2003*/// Transparent commands don't create their undo unit, so no need to send the command_end bracket.
			if(!SDS_DoingCommandTrans)
				SDS_SetUndo(IC_UNDO_COMMAND_END, (void*)localCommandName, 0, 0, SDS_CURDWG);
			/*DG - 26.7.2002*/// acad doesn't create undo groups for sds funcs calls,
			// also this grouping makes using of undo marks inside sds funcs
			//SDS_SetUndo(IC_UNDO_GROUP_BEGIN, (void*)localCommandName, 0, 0, SDS_CURDWG);

				// ****************************************
				// THREADSWAP !!!!!!!!!!!!!!!!!
			{
				SDSApplication	*pStartApp = ptempApp,
								*pStopApp = SDSApplication::GetEngineApplication();
 				if(!SDS_ThreadSwap(pStartApp->GetMainThread(), pStopApp->GetMainThread()))
				{
					SDS_ReleasedLinkFunc = FALSE;
					return RTERROR;
				}
			}

			/*DG - 26.7.2002*/// see comments just above again
			//SDS_SetUndo(IC_UNDO_GROUP_END, (void*)localCommandName, 0, 0, SDS_CURDWG);	/*DG 2.10.2001*/// See comments just above.

			SDS_ReleasedLinkFunc = FALSE;
			SDS_CallUserCallbackFunc(SDS_CBCMDEND, (void*)localCommandName, NULL, NULL);
		}

		SDS_ReleasedLinkFunc = FALSE;

		// This is a special case to fix the problem with a call to sds_command() for open
		// or new.	We have to delay the the re-init of lisp.
		if(SDS_DelayLispInit && SDSApplication::GetEngineApplication() == SDSApplication::GetActiveApplication())
		{
			SDS_DelayLispInit = 0;
			SDS_InitLisp();
		}

		lsp_convptlist(&lsp_evres, 0);
		if(!pushlsp)
			lsp_prtrecur(&lsp_evres, NULL, 1, 0);

		// Make a copy of result
		resbuf*	evres = sds_newrb(0);
		lsp_copysplrb(evres, &lsp_evres);

		// Push the lisp result onto the command stack.
		if(pushlsp && pushlsp != -1)
			GetActiveCommandQueue()->AddItem(evres);
		else
			lsp_freesuprb(evres);

	}

	return RTNORM;
}


static void callLispFunction(TCHAR* localCommandName)
{
	ic_lpad(localCommandName, (short)(strlen(localCommandName) + 1));
	localCommandName[0] = '('/*DNT*/;
	strcat(localCommandName, ")"/*DNT*/);
	int	pc = 0;

	// Check the user callback function.
	if(SDS_CallUserCallbackFunc(SDS_CBCMDBEGIN, (void*)localCommandName, NULL, NULL) == RTNORM)
	{
		SDS_DoingLispCode = 1;

		/*DG 2.10.2001*/// Remove IC_UNDO_COMMAND_BEGIN from undo for this lisp function (by pushing IC_UNDO_COMMAND_END).
		// Reason: doing so we will push undo units which can appear in the function directly to the undo stack
		// but not collect them as children in the parent undo unit created by IC_UNDO_COMMAND_BEGIN.
		// Notes: 1. by using (localCommandName+1) satisfy checking in SDS_SetUndo which checks for '(';
		//		  2. the real IC_UNDO_COMMAND_END which will be tried to push in SDS_cmdthread later
		//			 will be ignored using command_end_pending flag in newSDS_SetUndo.
		/*DG - 1.7.2003*/// Transparent commands don't create their undo unit, so no need to send the command_end bracket.
		if(!SDS_DoingCommandTrans)
			SDS_SetUndo(IC_UNDO_COMMAND_END, (void*)(localCommandName+1), 0, 0, SDS_CURDWG);
		/*DG - 26.7.2002*/// acad doesn't create undo groups for lisp funcs calls,
		// also this grouping makes using of undo marks inside lisp funcs;
		// also I'm not sure if the stuff added by AG below is necessary now
		//SDS_SetUndo(IC_UNDO_GROUP_BEGIN, (void*)localCommandName, 0, 0, SDS_CURDWG);

		/*DG - 17.1.2002*/// We should use this current drawing in the following pushing of IC_UNDO_GROUP_END.
		db_drawing*	pOriginalDrw = SDS_CURDWG;
		resbuf*		evres = NULL;

		if(!lsp_lispev(localCommandName, &evres, NULL, &pc))
			lsp_prtrecur(evres, NULL, 2, 0);

		SDS_DoingLispCode = 0;

/////////////	AG: this is set here to be properly included into UNDO brackets
		// The runnig lisp program did not finish the command function
		// So swap the active command line to the lisp command line.
		// Scary stuff!!!!!
		if(SDSApplication::GetActiveApplication()->GetLispCommandDepth() > 0)
		{
			// THREADSWAP !!!!!!!!!!!!!!!!!
			SDSApplication*	pApp = SDSApplication::GetActiveApplication();
			SDS_ThreadSwap(pApp->GetCommandThread(), pApp->GetMainThread());
		}
/////////////  AG: this is set here to be properly included into UNDO brackets
		
		/*DG - 26.7.2002*/// see comments just above
		//	/*DG - 17.1.2002*/// Since lisp code could open a new drawing we should use the original one here.
		//	SDS_SetUndo(IC_UNDO_GROUP_END, (void*)localCommandName, NULL, NULL, pOriginalDrw /*SDS_CURDWG*/);
		SDS_DoingLispCode = 0;
		SDS_CallUserCallbackFunc(SDS_CBCMDEND, (void*)localCommandName, NULL, NULL);
	}
}


static int callSetvar(char* cmd, bool* resetelev)
{
	char*	fcp1 = cmd;
	for( ; *fcp1 == '_'/*DNT*/ || *fcp1 == '.'/*DNT*/ || *fcp1 == '\''/*DNT*/; ++fcp1)
		;

	// check setvar list.
	// We need to add the underscore back on.
	if(strisame(fcp1, "VERNUM"/*DNT*/))
		fcp1 = "_VERNUM"/*DNT*/;
	else if(strisame(fcp1, "LINFO"/*DNT*/))
			fcp1 = "_LINFO"/*DNT*/;
	else if(strisame(fcp1, "PKSER"/*DNT*/))
			fcp1 = "_PKSER"/*DNT*/;
	else if(strisame(fcp1, "SERVER"/*DNT*/))
			fcp1 = "_SERVER"/*DNT*/;
	else if(strisame(fcp1, "ELEVATION"/*DNT*/) || strisame(fcp1, "ELEV"/*DNT*/))
			resetelev = false;
	// Added Cybage VSB 08/08/2001
	// Reason: Fix for Bug No: 77822
	else if(strisame(fcp1, "VENDORNAME"/*DNT*/))
			fcp1 = "_VENDORNAME"/*DNT*/;
	// Added Cybage VSB 08/08/2001]

	resbuf	rb;
	if(sds_getvar(fcp1, &rb) != RTNORM)
	{
		for( ; *fcp1 == '_'/*DNT*/ || *fcp1 == '.'/*DNT*/; ++fcp1)
			;
	}

	if(rb.restype == RTSTR && rb.resval.rstring)
		IC_FREE(rb.resval.rstring);

	// Modified Cybage PP 09/02/2001
	// Reason: Fix for bug no. : 59500
	//if (sds_getvar(fcp1, &rb)==RTNORM)
	if(!strisame(fcp1, "LASTCMDANG" ) && sds_getvar(fcp1, &rb) == RTNORM)
	{
		if(rb.restype == RTSTR)
		{
			IC_FREE(rb.resval.rstring);
			rb.resval.rstring = NULL;
		}

		SDS_SetVarByPass = fcp1;
		SDS_getvar(NULL, DB_QCMDNAMES, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		TCHAR	fs3[IC_ACADBUF];
		sprintf(fs3, "%s'SETVAR"/*DNT*/, rb.resval.rstring);
		IC_FREE(rb.resval.rstring);
		rb.resval.rstring = fs3;
		SDS_setvar(NULL, DB_QCMDNAMES, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
		cmd_setvar();
		return RTNORM;
	}
	return RTERROR;
}

/********************************************************************************
 * Author:	Mohan Nayak.														*
 *																				*
 * Purpose:	Enable/Disable toolbar combo boxes.									*
 *																				*
 * Returns:	void																*
 ********************************************************************************/
void enableToolBar(bool bEnable)
{
	if(SDS_CMainWindow)
	{
		// Entity Property ToolBar
		if(SDS_CMainWindow->m_wndLayers.GetSafeHwnd())
			SDS_CMainWindow->m_wndLayers.EnableWindow(bEnable);
		if(SDS_CMainWindow->m_wndColor.GetSafeHwnd())
			SDS_CMainWindow->m_wndColor.EnableWindow(bEnable);
		if(SDS_CMainWindow->m_wndLType.GetSafeHwnd())
			SDS_CMainWindow->m_wndLType.EnableWindow(bEnable);
		if(SDS_CMainWindow->m_wndLWeight.GetSafeHwnd())
			SDS_CMainWindow->m_wndLWeight.EnableWindow(bEnable);
		if(SDS_CMainWindow->m_wndDimstyle.GetSafeHwnd())
			SDS_CMainWindow->m_wndDimstyle.EnableWindow(bEnable);
		
	}
	return;
}


/*----------------------------------------------------------------------------*/
int		SDS_DoOneCommand
(
 char*	argcmd,
 int	pushlsp
)
{
	int				ret = RTNORM, lastcmdmode = 0, add2hist = 1;
	bool			resetelev = false;
	double			tempelev;
	resbuf			rb;
	commandAtomObj*	alink = NULL;
	TCHAR			cmd[IC_ACADBUF];
	bool			bTransparent = false;	/*DG - 30.7.2002*/// This is 'true' if we are executing the command transparently.
	int				kwint = SDSApplication::GetActiveApplication()->GetInputControlBits();

	SDSApplication::GetActiveApplication()->ClearKeywords();
	SDS_FreeKList = NULL;

	// Only set this after the user has typed one command.
	// if we set this in sds_init() we see the message in startup.
	db_setfontsubstfn(SDS_FontSubsMsg);

	if(SDS_ElevationChanged)
	{
		SDS_ElevationChanged = FALSE;
		SDS_getvar(NULL, DB_QELEVATION, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		tempelev = rb.resval.rreal;
		rb.resval.rreal = SDS_CorrectElevation;
		SDS_setvar(NULL, DB_QELEVATION, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 1);
		resetelev = true;
	}

	SDS_FunctionID = 0;

	if(SDSApplication::GetActiveApplication()->GetMainThread()->IsCurrent())
		SDSApplication::GetActiveApplication()->IncrementCommandDepth();
	else
		SDSApplication::GetActiveApplication()->IncrementLispCommandDepth();

	strncpy(cmd, argcmd, sizeof(cmd) - 1);

	// Destroy the prompt menu.
	SDS_PromptMenu(-1);

	if(SDS_CMainWindow->m_IgnoreLastCmd)
		add2hist = 0;

	SDS_CMainWindow->m_IgnoreLastCmd = FALSE;
	cmd_InsideViewPorts = FALSE;

	// Do the just return command.
	if(!*cmd && SDS_CmdHistCur)
	{
		lastcmdmode = 1;
		strncpy(cmd, SDS_CmdHistCur->cmd, sizeof(cmd) - 1);
		SDS_getvar(NULL, DB_QCMDECHO, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);

		if(CMD_CMDECHO_PRINTF(rb.resval.rint))
			sds_printf("%s"/*DNT*/, cmd);
	}

	// Do the help command if there is no history and the user hits return.
	if(!*cmd && !SDS_CmdHistCur)
		strcpy(cmd, "?"/*DNT*/);

	// Do a menu change.
	if(*cmd == '$')
	{
		sds_menucmd(cmd);
		goto out;
	}

	if(*cmd == '!' || *cmd == '(' || SDS_LispPCount)
	{
		doLispCode(cmd, pushlsp);
		goto out;
	}

	if(!findCommandAtom(alink, cmd, &bTransparent, true))
		goto out;

	/*DG - 30.7.2002*/// Set SDS_DoingCommandTrans to 'true' if the command string contains '
	// and the command is executed really transparently.
	if( SDSApplication::GetActiveApplication()->GetCommandDepth() > 1 ||
		SDSApplication::GetActiveApplication()->GetLispCommandDepth() > 1 )
	{
		if(bTransparent)
			SDS_DoingCommandTrans = true;
	}
	else
		bTransparent = false;

	if(alink)
	{
		SDS_getvar(NULL, DB_QCMDNAMES, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		TCHAR	localCommandName[IC_ACADBUF];
		strcpy(localCommandName, alink->LName);

		// This will eventually set CMDNAMES to the command being executed.
		// Here we will substitute the localized name in for the command. So
		// The user won't see the underscored global name of the command.
		if(rb.restype == RTSTR)
		{
			TCHAR	fs3[IC_ACADBUF];
			//VS. put global name (if possible) into cmdnames
			if(alink->GName)
			{
				if(!*rb.resval.rstring)
					sprintf(fs3, "%s", alink->GName + sizeof(TCHAR));
				else
					sprintf(fs3, "%s'%s", rb.resval.rstring, alink->GName + sizeof(TCHAR));
			}
			else
			{
				if(!*rb.resval.rstring)
					sprintf(fs3, "%s", alink->LName);
				else
					sprintf(fs3, "%s'%s", rb.resval.rstring, alink->LName);
			}

			IC_FREE(rb.resval.rstring);
			rb.resval.rstring = fs3;
			SDS_setvar(NULL, DB_QCMDNAMES, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);

			SDS_getvar(NULL, DB_QCMDNAMES, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			if(!*rb.resval.rstring)
				sprintf(fs3, "%s", alink->LName);
			else
				sprintf(fs3, "%s'%s", rb.resval.rstring, alink->LName);

			IC_FREE(rb.resval.rstring);
			rb.resval.rstring = fs3;

			// Need to compare against local name from resource.
			if( strisame(ResourceString(CMDSTR_REDRAW, "REDRAW"), rb.resval.rstring) ||
				strisame(ResourceString(CMDSTR_REGEN, "REGEN"), rb.resval.rstring) ||
				strisame(ResourceString(CMDSTR_REDRAWALL, "REDRAWALL"), rb.resval.rstring) ||
				strisame(ResourceString(CMDSTR_REGENALL, "REGENALL"), rb.resval.rstring) )
				add2hist = 0;
		}

		switch(alink->rb.restype)
		{
		case LSP_RTSUBR :
			struct resbuf rb;
			SDS_getvar(NULL,DB_QWNDLMDI,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if (rb.resval.rint != 0 && (SDS_CMainWindow != NULL && SDS_CMainWindow->m_nOpenViews!=0)) 
				enableToolBar(false);  // if any drawing is open, gray them all ...

			callInternalCommand(alink);
			
			if (rb.resval.rint != 0 && (SDS_CMainWindow != NULL && SDS_CMainWindow->m_nOpenViews!=0)) 
				enableToolBar(true); 
			break;
		case LSP_RTXSUB :
			ret = callSdsFunction(alink, pushlsp, localCommandName);
			break;
		default :
			if(SDSApplication::GetActiveApplication()->GetLispCommandDepth() > 0)
				sds_printf(ResourceString(IDC_ICADAPI__NERROR__USER_D_42, "\nERROR: User defined Lisp functions cannot be transparent."));
			else
				callLispFunction(localCommandName);
		}
	}
	else
	{
		if(callSetvar(cmd, &resetelev) != RTNORM)
		{
			// If we made it to here its was not found.
			cmd_ErrorPrompt(CMD_ERR_UNRECOGCMD, 0);
			ret = RTERROR;
		}
	}

	out: ;

	if(resetelev)
	{
		rb.restype = RTREAL;
		rb.resval.rreal = tempelev;
		SDS_setvar(NULL, DB_QELEVATION, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 1);
		SDS_ElevationChanged = TRUE;
	}

	/*DG - 30.7.2002*/// Set SDS_DoingCommandTrans to 'false' only if we are processing a transparent command.
	// See bugzilla bug 78198.
	if(bTransparent)
		SDS_DoingCommandTrans = false;

	if(SDS_getvar(NULL, DB_QCMDNAMES, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) == RTNORM && rb.resval.rstring)
	{
		if(strrchr(rb.resval.rstring, '\''))
			*strrchr(rb.resval.rstring, '\'') = 0;
		else
			*rb.resval.rstring = 0;

		SDS_setvar(NULL, DB_QCMDNAMES, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
		IC_FREE(rb.resval.rstring);
	}

	// Get rid of Lisp arguments for sds_getargs().
	// TODO: For some reason if we execute the line at the command prompt:
	//	   (command "insert" "test" (getpoint) "" "" "")
	// If the block comes from outside the drawing the following line of
	// code hurls.	I can't figure out why.  May be a heap problem!
	lsp_freesuprb(lsp_argsll);	//Uncomment this line.
	lsp_argsll = NULL;			// commented out to fix bug 1-57582
	/*D.G.*/// Two lines above have been uncommented to fix bug BRX 1682 (change request).

	// Add command to the command history buffer.
	if( SDSApplication::GetActiveApplication()->GetCommandDepth() == 1 &&
		add2hist && !lastcmdmode && ret == RTNORM &&
		(!SDS_CmdHistCur ? 1 : !strsame(SDS_CmdHistCur->cmd, argcmd)) &&
		SDSApplication::GetEngineApplication() == SDSApplication::GetActiveApplication() && *argcmd != '(' &&
		!SDSApplication::GetActiveApplication()->IsCommandRunning() )
	{
		TCHAR*	pCommandName = findGlobalCommandName(cmd);

		if(!pCommandName)
			pCommandName = argcmd;

		// Fix for bug # 1-59153 Don't add pmthist to the previous command list SDS_CmdHistCur.
		if(!strisame(pCommandName, "PMTHIST"/*DNT*/))
		{			
			SDS_CmdHist*	Ctmp = new SDS_CmdHist;
			memset(Ctmp, 0, sizeof(SDS_CmdHist));
			if(Ctmp && (Ctmp->cmd = new char [strlen(argcmd)+1]))
			{
				strcpy(Ctmp->cmd, argcmd);

				if(SDS_CmdHistBeg)
				{
					Ctmp->prev = SDS_CmdHistCur;
					SDS_CmdHistCur = SDS_CmdHistCur->next = Ctmp;
				}
				else
					SDS_CmdHistBeg = SDS_CmdHistCur = Ctmp;

				SDS_CmdHistAct = SDS_CmdHistCur;
			}
		}
	}

	if(SDSApplication::GetActiveApplication()->GetMainThread()->IsCurrent())
	{
		ASSERT(SDSApplication::GetActiveApplication()->GetCommandDepth() > 0);
		if(SDSApplication::GetActiveApplication()->GetCommandDepth() > 0)
			SDSApplication::GetActiveApplication()->DecrementCommandDepth();
	}
	else
	{
		// There is code in the system that clears this to 0, so it could be 0 at this point
		ASSERT(SDSApplication::GetActiveApplication()->GetLispCommandDepth() >= 0);

		if(SDSApplication::GetActiveApplication()->GetLispCommandDepth() > 0)
			SDSApplication::GetActiveApplication()->DecrementLispCommandDepth();
	}

	SDSApplication::GetActiveApplication()->SetInputControlBits(kwint);
	// AG: the following block was moved to  SDS_cmdthread() to allow
	// LISP thread to set IC_UNDO_COMMAND_END undo bracket during the processing of the same command
/*
	// If we are back at the commad line and the applications command
	// line is still running swap back, to the MAIN command line.
	// Scary stuff!!!!!	 FIX FOR (defun c:xx () (command "arc" "2,2"))
	if( SDSApplication::GetActiveApplication()->GetCommandThread()->IsCurrent() &&
		SDSApplication::GetActiveApplication()->GetLispCommandDepth()==0 &&
		!IDS_GetThreadController()->GetInsideCmdFunction() &&
		!SDS_DoingLispCode && !SDS_ReleasedLinkFunc && !SDS_Inside3rdPartyFunc)
		{
			// ****************************************
			// THREADSWAP !!!!!!!!!!!!!!!!!
			{
			SDSApplication *pStartApp = SDSApplication::GetActiveApplication();
			SDSApplication *pStopApp = SDSApplication::GetActiveApplication();
			SDS_ThreadSwap(pStopApp->GetMainThread(),pStartApp->GetCommandThread());
			}
		}
*/
	return ret;
}


int SDS_FreeCmdHist(void) {
	struct SDS_CmdHist *tmp,*cur;

	for(cur=SDS_CmdHistBeg; cur!=NULL; ) {
		tmp=cur; cur=cur->next;
		IC_FREE(tmp->cmd);
		IC_FREE(tmp);
	}

	SDS_CmdHistBeg=SDS_CmdHistCur=SDS_CmdHistAct=NULL;

	return(RTNORM);
}

void
SDS_SetLastPrompt(const char* prompt)
{
/*D.G.*/// Enabled for DBCS.

	char*	fcp1;
	resbuf	rb;

	if(prompt)
	{
		rb.restype = RTSTR;
		rb.resval.rstring = (char*)prompt;
		SDS_setvar(NULL, DB_QLASTPROMPT, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
		if(fcp1 = _tcsrchr(prompt, '\n'))
			strncpy(SDS_LastPrompt, fcp1 + 1, IC_ACADBUF - 1);
		else
			strncpy(SDS_LastPrompt, prompt, IC_ACADBUF - 1);
	}

	return;
}

int SDS_SendMessage(int message, WPARAM wParam, LPARAM lParam)
{

	/*DG - 29.3.2002*/// I added writing data in scr file format to SDS_CMainWindow->m_fLogFile.

	static int LastCur;
	static char *PendingMnu;
	struct resbuf rb;
	int hadevent=0;
	long sslen;

	// this variable counts the number of characters that users enter at a command 
	// prompt each time so we can know maximum how many spaces we can step back when
	// the back space key is pressed. SWH.
	// 
	static int cmdCharEnteredCount = 0;

	InputEvent event;

	if(SDS_CMainWindow->m_bCustomize)
		{
		return RTNORM;
		}

	/*DG - 4.4.2002*/// We need some things for errors handling on file opening.
	CFileException	fileException;
	static bool		bFileAlertShown = false;

	SDS_getvar(NULL, DB_QLOGFILEMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	if(rb.resval.rint)
	{
		if(!SDS_CMainWindow->m_fLogFile.m_hFile)
		{
			SDS_getvar(NULL, DB_QLOGFILEMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			if(rb.resval.rint)
			{
				SDS_getvar(NULL, DB_QLOGFILENAME, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
				if(SDS_CMainWindow->m_fLogFile.Open(rb.resval.rstring, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate, &fileException))
				{
					// create a string w/o spaces so it's interpreted by the script player as a command,
					// namely a wrong one, so user will see the breakpoint of script execution
					char*	pStr = SDS_CMainWindow->m_fLogFile.SeekToEnd() ?
									pStr = "\r\n[___IntelliCAD___"/*DNT*/ :
									pStr = "[___IntelliCAD___"/*DNT*/;
					SDS_CMainWindow->m_fLogFile.Write(pStr, strlen(pStr));

					time_t	systime;
					tm*		loctime;
					time(&systime);
					loctime = localtime(&systime);
					pStr = asctime(loctime);
					for(char* pChar = pStr; *pChar; ++pChar)
						if(*pChar == ' '/*DNT*/)
							*pChar = '_'/*DNT*/;
					SDS_CMainWindow->m_fLogFile.Write(pStr, strlen(pStr) - 1);

					pStr = "___]\r\n"/*DNT*/;
					SDS_CMainWindow->m_fLogFile.Write(pStr, strlen(pStr));
				}
				else
				{
					/*DG - 4.4.2002*/// Show alert message and nullify the file handle (because CFile::Open changes the handle).
					if(!bFileAlertShown)
					{

						TCHAR	osErrorMessage[1024], ourErrorMessage[128];
						fileException.GetErrorMessage(osErrorMessage, 1024);
						sprintf(ourErrorMessage, ResourceString(IDC_ICADTOOLBARMAIN_CAN_T_OP_4, "Can't open file %s, error = %u\n"), rb.resval.rstring, fileException.m_cause);
						CString	errorMessage(ourErrorMessage);
						errorMessage += '(';
						errorMessage += osErrorMessage;
						errorMessage += ')';
						sds_alert((LPCTSTR)errorMessage);
						bFileAlertShown = true;
					}
					SDS_CMainWindow->m_fLogFile.m_hFile = 0;
				}
				IC_FREE(rb.resval.rstring);
			}
		}
	}
	else
	{
		if(SDS_CMainWindow->m_fLogFile.m_hFile)
		{
			SDS_CMainWindow->m_fLogFile.Close();
			SDS_CMainWindow->m_fLogFile.m_hFile = 0;
		}
		bFileAlertShown = false;
	}

	if( !SDS_CMainWindow->m_fMacroRecFile.m_hFile && !SDS_CMainWindow->m_fLogFile.m_hFile &&
		PendingMnu )
		{
		IC_FREE(PendingMnu);
		PendingMnu=NULL;
		}

	if( message==WM_LBUTTONDOWN ||
		message==WM_MOUSEMOVE)
		{

		// Change the cursor if we are in MS and we are outside the current VP.
		if( SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM &&
			rb.resval.rint==0 &&
			SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM &&
			rb.resval.rint>1 &&
			SDS_CURVIEW)
			{
			// EBATECH(CNBR) 2001-06-24 unsign short -> int
			CPoint point((int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam));

			if(SDS_CURVIEW->m_rectMspace.PtInRect(point))
				{
				int cur=SDS_CMainWindow->m_ViewCursorType;

				SDS_CMainWindow->m_ViewCursorType=0;
				SDS_SetCursor(cur);
				}
			else
				{
//				SDS_CURVIEW->m_idcCursor=0;

				if( message == WM_LBUTTONDOWN )
					{
					db_handitem *hip,*vphip;
					int x1,x2,y1,y2,fi1;
					CRect rect;

					for(hip=SDS_CURDWG->tblnext(DB_VXTABREC,1); hip!=NULL; hip=hip->next)
						{

						if( hip->ret_deleted() ||
							(vphip=((db_vxtabrec *)hip)->ret_vpehip())==NULL)
							{
							continue;
							}


						// Continue if its the PS VP
						vphip->get_69(&fi1);
						if(fi1==1)
							{
							continue;
							}


						// Continue if This VP is off.
						vphip->get_68(&fi1);
						if(fi1==0)
							{
							continue;
							}

						SDS_GetRectForVP( SDS_CURVIEW, vphip,&x1,&x2,&y1,&y2);
						rect.SetRect(x1,y1,x2,y2);

						if(rect.PtInRect(point))
							{
							if(vphip==SDS_CURVIEW->m_pVportTabHip)
								{
								goto out2;			// ---------- GOTO out2 below
								}

							if(!IDS_GetThreadController()->GetAtWaitLoop())
								{
								return(RTNONE);
								}
							cmd_MakeVpActive(SDS_CURVIEW,vphip,SDS_CURDWG);
							return(RTNONE);
							}

						}
					if(hip==NULL)
						{
						return(RTNONE);
						}
					}
				out2: ;								// ------------------ LABEL out2
				}
			} // end of change cursor code





		if(message==WM_LBUTTONDOWN)
			{
			if(!(SDS_CurEventMask&SDS_EVM_MOUSEBUTTON))
				{
				return(RTNONE);
				}

			event.m_type=SDS_EVM_MOUSEBUTTON;
			}
		else
			{
			event.m_type=SDS_EVM_MOUSEXYZ;
			}

		struct gr_view *view=SDS_CURGRVW;
		if(view==NULL)
			{
			goto out;			// ----------------- GOTO out below
			}
		// EBATECH(CNBR) 2001-06-24 add cast operator
		if(SDS_CURVIEW->m_bHaveMagneticGrip)
			ic_ptcpy(event.m_pt, SDS_CURVIEW->m_magneticGrip);
		else
		{
			gr_pix2rp(view, LOWORD(lParam), HIWORD(lParam), &event.m_pt[0], &event.m_pt[1]);
			gr_rp2ucs(event.m_pt,event.m_pt,view);
		}

		if( !SDS_AtEntGet &&
			!SDS_AtCmdLine)
			{
			SDS_SnapPt(event.m_pt,event.m_pt);
			//Bugzilla::78435; 14-04-03
			COsnapManagerIcad *pOsnapManager = ((CEngineApplication*)SDSApplication::GetEngineApplication())->getOsnapManager();
			pOsnapManager->readSnapPoint( event.m_pt );
			}

		if( event.GetType()==SDS_EVM_MOUSEBUTTON &&
			(SDS_CMainWindow->m_fMacroRecFile.m_hFile || SDS_CMainWindow->m_fLogFile.m_hFile) )
			{

			char cst[3][30];

			for(int fi1=0; fi1<3; fi1++)
				{
				sds_rtos(event.m_pt[fi1],2,-1,cst[fi1]);
				}

			SDS_getvar(NULL,DB_QELEVATION,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

			if(icadRealEqual(event.m_pt[2],rb.resval.rreal))
				{
				_snprintf(SDS_CMainWindow->m_strCoords,sizeof(SDS_CMainWindow->m_strCoords)-1,ResourceString(IDC_ICADAPI__S__S_R_N_58, "%s,%s\r\n" ),cst[0],cst[1]);
				}
			else
				{
				_snprintf(SDS_CMainWindow->m_strCoords,sizeof(SDS_CMainWindow->m_strCoords)-1,ResourceString(DNT_ICADAPI__S__S__S_R_N_59, "%s,%s,%s\r\n" ),cst[0],cst[1],cst[2]);
				}


			if(SDS_CMainWindow->m_fMacroRecFile.m_hFile) 
				SDS_CMainWindow->m_fMacroRecFile.Write(SDS_CMainWindow->m_strCoords,strlen(SDS_CMainWindow->m_strCoords));
			if(SDS_CMainWindow->m_fLogFile.m_hFile)
				SDS_CMainWindow->m_fLogFile.Write(SDS_CMainWindow->m_strCoords,strlen(SDS_CMainWindow->m_strCoords));

			if(PendingMnu)
				{
				if(*PendingMnu==0)
					{
					IC_FREE(PendingMnu);
					PendingMnu=NULL;
					}

				if(*PendingMnu=='\\')
					{

					//strcpy(PendingMnu,PendingMnu+1);
					for(int ct=0; PendingMnu[ct]!=0; PendingMnu[ct]=(PendingMnu+1)[ct],++ct);

					if(!strchr(PendingMnu,'\\'))
						{

						if(SDS_CMainWindow->m_fMacroRecFile.m_hFile)
							{
							SDS_CMainWindow->m_fMacroRecFile.Write(PendingMnu,strlen(PendingMnu));
							SDS_CMainWindow->m_fMacroRecFile.Write("\r\n"/*DNT*/,2);
							}
						if(SDS_CMainWindow->m_fLogFile.m_hFile)
							{
							SDS_CMainWindow->m_fLogFile.Write(PendingMnu,strlen(PendingMnu));
							SDS_CMainWindow->m_fLogFile.Write("\r\n"/*DNT*/,2);
							}

						IC_FREE(PendingMnu);
						PendingMnu=NULL;
						}
					}
				else
					{
					if(strchr(PendingMnu,'\\'))
						{

						char *fcp2=strchr(PendingMnu,'\\');
						*fcp2=0;

						if(SDS_CMainWindow->m_fMacroRecFile.m_hFile)
							{
							SDS_CMainWindow->m_fMacroRecFile.Write(PendingMnu,strlen(PendingMnu));
							SDS_CMainWindow->m_fMacroRecFile.Write("\r\n"/*DNT*/,2);
							}
						if(SDS_CMainWindow->m_fLogFile.m_hFile)
							{
							SDS_CMainWindow->m_fLogFile.Write(PendingMnu,strlen(PendingMnu));
							SDS_CMainWindow->m_fLogFile.Write("\r\n"/*DNT*/,2);
							}

						*fcp2='\\';

						for(int ct=0; PendingMnu[ct]!=0; PendingMnu[ct]=fcp2[ct],++ct);

						}
					else
						{
						if(SDS_CMainWindow->m_fMacroRecFile.m_hFile)
							{
							SDS_CMainWindow->m_fMacroRecFile.Write(PendingMnu,strlen(PendingMnu));
							SDS_CMainWindow->m_fMacroRecFile.Write("\r\n"/*DNT*/,2);
							}
						if(SDS_CMainWindow->m_fLogFile.m_hFile)
							{
							SDS_CMainWindow->m_fLogFile.Write(PendingMnu,strlen(PendingMnu));
							SDS_CMainWindow->m_fLogFile.Write("\r\n"/*DNT*/,2);
							}

						IC_FREE(PendingMnu);
						PendingMnu=NULL;
						}
					}
				}
			}
		hadevent=1;
		}
	else if(message==WM_CHAR)
		{

		if(!(SDS_CurEventMask&SDS_EVM_KEYCHAR))
			{
			return(RTNONE);
			}

	if(SDS_CURVIEW)
//4M Bugzilla 77987 19/02/02->
/*
		SDS_CURVIEW->m_RTMotion = NO;
*/
// We leave the behaviour for ZOOM_WHEEL intact
	  if (SDS_CURVIEW->GetRTMotion()==ZOOM_WHEEL)
		 SDS_CURVIEW->SetRTMotion(NO);
//<-4M 19/02/02


		hadevent=1;
		event.m_type=SDS_EVM_KEYCHAR;
		event.m_key=(char)wParam;

		if(event.m_key==SDS_ESCAPECHAR)
			{
			sds_printf(ResourceString(IDC_ICADAPI__N___CANCEL____60, "\nCancel" ));
			SDS_userbreak=1;
			if(SDS_CMainWindow->m_bExpInsertState)
			{
				SDS_CMainWindow->m_bExpInsertState=FALSE;
				if(SDS_CMainWindow->m_pExplorer)
					ExecuteUIAction( ICAD_WNDACTION_DELEXP );
			}

			if (SDS_CMainWindow->m_pTablet)
				{

				SDS_CMainWindow->m_pTablet->m_bCancelled = TRUE;
				SDS_CMainWindow->m_pTablet->m_bExit = FALSE;
				SDS_CMainWindow->m_pTablet->m_RawInputEvent.SetEvent();

				}

			} // if event.key == ESCAPECHAR
		else if(event.m_key==SDS_ENTERCHAR &&
				SDS_CMainWindow->m_pTablet &&
				SDS_CMainWindow->m_pTablet->m_pWinTabRaw->IsEnabled())
			{

			SDS_CMainWindow->m_pTablet->m_bExit = TRUE;
			SDS_CMainWindow->m_pTablet->m_bCancelled = FALSE;
			SDS_CMainWindow->m_pTablet->m_RawInputEvent.SetEvent();

			}

		if(SDS_CMainWindow->m_fMacroRecFile.m_hFile)
			{
			switch(event.m_key)
				{
				case 13:
					SDS_CMainWindow->m_fMacroRecFile.Write("\r\n"/*DNT*/,2);
					cmdCharEnteredCount = 0;   // always reset the count to zero after users complete typing.
					break;
				case '\\':
				    SDS_CMainWindow->m_fMacroRecFile.Write("//"/*DNT*/,1);
					cmdCharEnteredCount++;
					break;
				case SDS_ESCAPECHAR:
					if (cmdCharEnteredCount > 0) {
						// Remove incompleted command that users entered at the command 
						// prompt from the script file. SWH
						//
						long offset = (cmdCharEnteredCount <= SDS_CMainWindow->m_fMacroRecFile.GetPosition()) ?
									   cmdCharEnteredCount : SDS_CMainWindow->m_fMacroRecFile.GetPosition();
						SDS_CMainWindow->m_fMacroRecFile.Seek(-1 * offset,CFile::current);
						cmdCharEnteredCount = 0;;
					}
					SDS_CMainWindow->m_fMacroRecFile.Write("^C\r\n"/*DNT*/,4);
					cmdCharEnteredCount = 0;
					break;
				case '\b':
					// Normally we need not check the current position of the file pointer
					// but here we do checking in case an exception is thrown before 
					// cmdCharEnteredCount is reset. SWH
					// 
					if (cmdCharEnteredCount > 0 && SDS_CMainWindow->m_fMacroRecFile.GetPosition() > 0) {
						SDS_CMainWindow->m_fMacroRecFile.Seek(-1,CFile::current);
						cmdCharEnteredCount--;
					}
					break;
				default:
					SDS_CMainWindow->m_fMacroRecFile.Write(&event.m_key,1);
					cmdCharEnteredCount++;
					break;
				}
			}

		if(SDS_CMainWindow->m_fLogFile.m_hFile)
			{
			switch(event.m_key)
				{
				case 13:
					SDS_CMainWindow->m_fLogFile.Write("\r\n"/*DNT*/,2);
					break;
				case '\\':
				   SDS_CMainWindow->m_fLogFile.Write("//"/*DNT*/,1);
					break;
				case SDS_ESCAPECHAR:
					SDS_CMainWindow->m_fLogFile.Write("^C\r\n"/*DNT*/,4);
					break;
				case '\b':
					if (SDS_CMainWindow->m_fLogFile.GetPosition() > 0)
						SDS_CMainWindow->m_fLogFile.Seek(-1,CFile::current);
					break;
				default:
					SDS_CMainWindow->m_fLogFile.Write(&event.m_key,1);
					break;
				}
			}
		}
	else if(message==WM_RBUTTONDOWN)
		{

		if(SDS_InsideGrread)
			{

			hadevent=1;
			event.m_type=SDS_EVM_AUXMENU;
			event.m_key=(char)0;
			event.m_menustr=(char *)0;
			if(SDS_CMainWindow->m_fIsShiftDown)
				{
				event.m_menustr=(char *)((int)event.m_menustr+1000);
				}

			if(SDS_CMainWindow->m_fIsCtrlDown)
				{
				event.m_menustr=(char *)((int)event.m_menustr+2000);
				}

			struct gr_view *view=SDS_CURGRVW;

			if(view!=NULL)
				{
				// EBATECH(CNBR) 2001-06-24 unsign short -> int
				gr_pix2rp(view,(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam),&event.m_pt[0],&event.m_pt[1]);
				gr_rp2ucs(event.m_pt,event.m_pt,view);
				}

			}
		else
			{

			sslen=0L;
//4M Spiros Item:27->
/*
			if(SDS_CURDOC && SDS_AtCmdLine &&
*/
			if(SDS_CURDOC && (SDS_AtCmdLine || SDS_GripDragging) &&
//4M Spiros Item:27<-
				((sds_sslength(SDS_CURDOC->m_pGripSelection,&sslen)!=RTERROR && sslen>0) ||
//4M Item:25->
// Replacement of SDS_NodeList with SDS_NodeList_New
/*
				SDS_CURDOC->m_nodeList.m_beg))
*/
				(SDS_CURDOC->m_nodeList.GetNoEntities()>0)))
//<-4M Item:25
				{

				ExecuteUIAction( ICAD_WNDACTION_RCLICK );

				}
			else
				{

				hadevent=1;
				event.m_type=SDS_EVM_KEYCHAR;
				event.m_key=(char)13;

				if(SDS_CMainWindow->m_fMacroRecFile.m_hFile)
					{
						if (cmdCharEnteredCount > 0) {
							// remove incompleted command that users just entered at the command 
							// prompt from the script file. SWH
							//
							long offset = (cmdCharEnteredCount <= SDS_CMainWindow->m_fMacroRecFile.GetPosition()) ?
										   cmdCharEnteredCount : SDS_CMainWindow->m_fMacroRecFile.GetPosition();
							SDS_CMainWindow->m_fMacroRecFile.Seek(-1 * offset,CFile::current);
							cmdCharEnteredCount = 0;
						}
						SDS_CMainWindow->m_fMacroRecFile.Write("\r\n"/*DNT*/,2);
					}

				if(SDS_CMainWindow->m_fLogFile.m_hFile)
					SDS_CMainWindow->m_fLogFile.Write("\r\n"/*DNT*/,2);

				}
			}
		}
	else if(message==WM_MENUSELECT)
		{

		if(SDS_CURVIEW)
		{
//4M Bugzilla 77987 19/02/02->
/*
			if(SDS_CURVIEW->m_pRealTime)
				return(RTNONE);
			SDS_CURVIEW->m_RTMotion = NO;
*/
// We leave the behaviour for ZOOM_WHEEL intact
		 if (SDS_CURVIEW->GetRTMotion()==ZOOM_WHEEL){
			   if(SDS_CURVIEW->GetRealTime())
				   return(RTNONE);
   		   SDS_CURVIEW->SetRTMotion(NO);
		 }
		 else if (SDS_CURVIEW->GetRTMotion()!=NO){
			SDS_SendMessage(WM_CHAR,27,0);
		 }
//<-4M 19/02/02
		}

		if(SDS_CMainWindow->m_bPrintPreview)
			{

			//*** Shutdown print preview before continuing.
			CIcadChildWnd* pChild = (CIcadChildWnd*)SDS_CMainWindow->MDIGetActive();

			if(pChild!=NULL)
				{
				 CView* pView = pChild->GetActiveView();
				 if(pView->IsKindOf(RUNTIME_CLASS(CPreviewView)))
					 {
					pView->SendMessage(WM_COMMAND,AFX_ID_PREVIEW_CLOSE);
					 }
				}
			}

		if(!(SDS_CurEventMask&SDS_EVM_POPUPCOMMAND))
			{
			return(RTNONE);
			}

		hadevent=1;
		event.m_type=SDS_EVM_POPUPCOMMAND;
		event.m_menustr= new char [strlen((char *)lParam)+1];
		strcpy(event.m_menustr,(char *)lParam);

		// ***********************************************
		// Very special case for SCR files on startup.
		// Add the menu seletct to the buffer DIRECTLY!
		//
		if(IDS_GetThreadController()->GetWaitThreadID()==0)
			{
			SDS_ProcessInputEvent(&event);
			goto out;
			}

		if(SDS_CMainWindow->m_fMacroRecFile.m_hFile || SDS_CMainWindow->m_fLogFile.m_hFile)
			{

			char *fcp1;
			fcp1=event.m_menustr;
			while(fcp1[0]=='^' && toupper(fcp1[1])=='C')
				{
				fcp1+=2;
				}

			if(!strisame(fcp1,"STOPSCRIPT"/*DNT*/))
				{
				if(strchr(fcp1,'\\'))
					{

					PendingMnu= new char [strlen(fcp1)+1];
					strcpy(PendingMnu,fcp1);

					char *fcp2=strchr(PendingMnu,'\\');
					*fcp2=0;
					if(SDS_CMainWindow->m_fMacroRecFile.m_hFile)
						SDS_CMainWindow->m_fMacroRecFile.Write(PendingMnu,strlen(PendingMnu));
					if(SDS_CMainWindow->m_fLogFile.m_hFile)
						SDS_CMainWindow->m_fLogFile.Write(PendingMnu,strlen(PendingMnu));

					*fcp2='\\';
					for(int ct=0; PendingMnu[ct]!=0; PendingMnu[ct]=fcp2[ct],++ct);
					}
				else
					{
					if(SDS_CMainWindow->m_fMacroRecFile.m_hFile) {
						// When a menu is selected, we need to check if users have entered some characters
						// at the command prompt first. SWH
						//
						if (cmdCharEnteredCount > 0) {
							// we need to earse incompleted command that users entered at the command 
							// prompt from the script file before append the menu command to the script 
							// file. Otherwise, those invalid command will cause scripts to fail. SWH
							//
							long offset = (cmdCharEnteredCount <= SDS_CMainWindow->m_fMacroRecFile.GetPosition()) ?
										   cmdCharEnteredCount : SDS_CMainWindow->m_fMacroRecFile.GetPosition();
							SDS_CMainWindow->m_fMacroRecFile.Seek(-1 * offset,CFile::current);
							cmdCharEnteredCount = 0;;
						}
						SDS_CMainWindow->m_fMacroRecFile.Write(fcp1,strlen(fcp1));
					}
					if(SDS_CMainWindow->m_fLogFile.m_hFile)
						SDS_CMainWindow->m_fLogFile.Write(fcp1,strlen(fcp1));
					}
					if(SDS_CMainWindow->m_fMacroRecFile.m_hFile)
						SDS_CMainWindow->m_fMacroRecFile.Write("\r\n"/*DNT*/,2);
					if(SDS_CMainWindow->m_fLogFile.m_hFile)
						SDS_CMainWindow->m_fLogFile.Write("\r\n"/*DNT*/,2);
				}
			}
		}



	if(hadevent)
		{
		SDS_PostInputEvent( event );
		}


	out:		// ------------------ LABEL out

	return(0);

	}


// ******************************************************************






// ******************************************************************

InputEventQueueInterface *IDS_GetInputEventQueue();

bool SDS_PostInputEvent( const InputEvent &theEvent )
	{
	InputEvent *pevent = new InputEvent( theEvent );

	UINT uiMessageID = GetEventMessageID();

	// We want to separate out mouse moves so we can PeekMessage for them easily
	//	This is needed so we get the latest mouse move out of the queue instead of
	// just the first.	This helps us be more accurate when reporting the cursor
	// position
	//
	if (pevent->GetType() == SDS_EVM_MOUSEXYZ)
		{
		uiMessageID = GetMouseMoveEventMessageID();
		}


	return	IDS_GetInputEventQueue()->PostEvent( pevent );
	}


// ******************************************************************
int SDS_WaitLoop(void)
	{

	if ( IDS_IsIcadExiting() )
		{
		return 0;
		}
	InputEvent *pEvent = NULL;

	// let's enforce non-reentrancy
	//
	static SingleThreadSection theSection;
	SingleThreadGuard theGuard( &theSection );


	int retcode=0;
	CIcadView *pView=SDS_CURVIEW;

	if(!GetMenuQueue()->IsEmpty())
		{
		const char *szMenuToken = GetMenuQueue()->PeekString();
		if ( szMenuToken != NULL )
			{
			if ( ( szMenuToken[0] != '\\' ) &&
				 ( szMenuToken[0] != '(' ) &&
				 ( (strlen(szMenuToken) == 0) || (szMenuToken[strlen(szMenuToken)-1]!='\\') )
			   )
				{
				struct resbuf rb;
				SDS_getvar(NULL,DB_QMENUECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if(!(rb.resval.rint&1))
					{
					rb=*GetMenuQueue()->PeekItem();
					rb.rbnext=NULL;
					lsp_prtrecur(&rb,NULL,0,0);
					}
				}
			}

		if( (szMenuToken != NULL) &&
			(szMenuToken[0] == '\\') )
			{
			GetMenuQueue()->AdvanceNext();
			}
		else if( (szMenuToken != NULL) &&
  				 (strlen(szMenuToken) > 0) &&
				 (szMenuToken[strlen(szMenuToken)-1]=='\\') )
			{

			for(const char *fcp1=szMenuToken; *fcp1!='\\'; fcp1++)
				{
				SDS_SendMessage(WM_CHAR,*fcp1,0);
				}

			GetMenuQueue()->AdvanceNext();
			}
		else
			{
			struct resbuf *pNextItem = GetMenuQueue()->PopItem();

			GetMenuQueue()->Concat( GetActiveCommandQueue() );
			GetActiveCommandQueue()->Flush();

			if( pNextItem->restype==RTSTR &&
				*SDS_cmdcharbuf )
				{
				CString cs = SDS_cmdcharbuf;
				cs += pNextItem->resval.rstring;
				db_astrncpy( &pNextItem->resval.rstring, cs.GetBuffer(IC_ACADBUF), -1 );
				cs.ReleaseBuffer();
				*SDS_cmdcharbuf = 0;
				}

			GetActiveCommandQueue()->AddItem( pNextItem );

			szMenuToken = GetMenuQueue()->PeekString();

			if( !SDSApplication::GetActiveApplication()->AcceptParenOrCR() &&
 				 (GetActiveCommandQueue()->PeekString() != NULL) &&
				 (szMenuToken != NULL) &&
				 (szMenuToken[0] == ' ') &&
				 (szMenuToken[1] == 0 ) )
				{
				GetMenuQueue()->AdvanceNext();
				}
			retcode=1;
			goto out;
			}
		}



   	IDS_GetThreadController()->SetAtWaitLoop( true );

	// Turn the cursor on.
	if(!SDS_CMainWindow->m_bPrintPreview)
		{
		ExecuteUIAction( ICAD_WNDACTION_CURSORON );
		}


	IDS_GetThreadController()->SetWaitThreadID( GetCurrentThreadId() );


	pEvent = IDS_GetInputEventQueue()->WaitEvent();


	// MouseMove events are special.  Rather than get the first one in the queue, we need
	// to get the latest one in order to be closest to where the mouse is RIGHT NOW
	//
	if ( (pEvent != NULL) &&
		 (pEvent->GetType() == SDS_EVM_MOUSEXYZ) )
		{

		// We're going to search for any more mouse events in the queue
		//
		InputEventFilter theMouseFilter( SDS_EVM_MOUSEXYZ );

		InputEvent *pLatestMouseMove = IDS_GetInputEventQueue()->PeekEvent( &theMouseFilter );

		// keep getting them until we run out
		//
		while( pLatestMouseMove )
			{
			delete pEvent;
			pEvent = pLatestMouseMove;
			pLatestMouseMove = IDS_GetInputEventQueue()->PeekEvent( &theMouseFilter );
			}
		}


	if ( pEvent != NULL )
		{
		SDS_ProcessInputEvent( pEvent );
		delete pEvent;
		retcode = 1;
		}

	retcode=1;

	out: ;

	// Turn the cursor off.
	ExecuteUIAction( ICAD_WNDACTION_CURSOROFF );

   	IDS_GetThreadController()->SetAtWaitLoop( false );

	return(retcode);
}

int
SDS_ProcessInputEvent(InputEvent* pEvent)
{
/*D.G.*/// DBCS enabled
	resbuf*		pRb = NULL;
	sds_point	point = {0.0, 0.0, 0.0};
	int			eventType = pEvent->GetType();

	static bool	bLeadingByte = false;

	if(bLeadingByte && eventType != SDS_EVM_KEYCHAR)
	{
		ASSERT(0);	/*D.G.*/// Look at my comments below about an analogous ASSERT.
		bLeadingByte = false;
	}

	if(!SDSApplication::GetEngineApplication())
		return RTERROR;

	switch(eventType)
	{
		case SDS_EVM_AUXMENU:
			pRb = sds_newrb(SDS_RTDRAGPT);
			pEvent->GetPoint(pRb->resval.rpoint);
			GetActiveCommandQueue()->AddItemToFront(pRb);

			pRb = sds_newrb(SDS_EVM_AUXMENU);
			pRb->resval.rint = (int)pEvent->GetKey() + (int)pEvent->m_menustr;	// hack use of menustr to hold an integer
			GetActiveCommandQueue()->AddItemToFront(pRb);
			break;

		case SDS_EVM_MOUSEXYZ:
			if(!(SDS_CurEventMask & SDS_EVM_MOUSEXYZ))
				break;

			pRb = sds_newrb(SDS_RTDRAGPT);
			pEvent->GetPoint(pRb->resval.rpoint);
			GetActiveCommandQueue()->AddItem(pRb);
			break;

		case SDS_EVM_KEYCHAR:
		{
			if(!(SDS_CurEventMask & SDS_EVM_KEYCHAR))
				break;

			// This is a very special callback for text input dragging only.
			if(SDS_IsDragging && SDS_CMainWindow->m_pDragVarsCur && SDS_CMainWindow->m_pDragVarsCur->mode == -5)
				(*SDS_CMainWindow->m_pDragVarsCur->scnf)(point, SDS_CMainWindow->m_pDragVarsCur->matx);

			char	sym = pEvent->GetKey();
			int		length = strlen(SDS_cmdcharbuf);

			switch(sym)
			{
				case 11:	// Ctrl - K

					if(!SDS_CmdHistAct)
						break;

					strcpy(SDS_cmdcharbuf, SDS_CmdHistAct->cmd);
					sds_printf("\r%s%s                                                  "/*DNT*/, SDS_LastPrompt, SDS_cmdcharbuf);
					sds_printf("\r%s%s"/*DNT*/, SDS_LastPrompt, SDS_cmdcharbuf);

					if(!SDS_CmdHistAct->prev)
						break;

					if(SDS_CmdHistAct->prev)
						SDS_CmdHistAct = SDS_CmdHistAct->prev;
					break;

				case 12:	// Ctrl - L

					if(!SDS_CmdHistAct)
						break;

					if(SDS_CmdHistAct->next)
						SDS_CmdHistAct = SDS_CmdHistAct->next;

					strcpy(SDS_cmdcharbuf, SDS_CmdHistAct->cmd);
					sds_printf("\r%s%s                                                  "/*DNT*/, SDS_LastPrompt, SDS_cmdcharbuf);
					sds_printf("\r%s%s"/*DNT*/, SDS_LastPrompt, SDS_cmdcharbuf);
					break;

				case '\b':
					if(SDS_InsideGrread)
						goto defgoto;

					if(length)
					{
						*_mbsdec((unsigned char*)SDS_cmdcharbuf, (unsigned char*)(SDS_cmdcharbuf + length)) = '\0';
						if(!SDS_NoCharEcho)
							sds_printf("\b"/*DNT*/);
					}
					break;

				case '\r':
				case ' ' :
					if( sym == ' ' &&
						(SDSApplication::GetActiveApplication()->AcceptParenOrCR()	||
						 SDS_cmdcharbuf[0] == '('									||
						 SDS_InsideGrread											||
						 (SDS_IsDragging && SDS_CMainWindow->m_pDragVarsCur && SDS_CMainWindow->m_pDragVarsCur->mode == -5)) )
						goto defgoto;

					pRb = sds_buildlist(RTSTR, SDS_cmdcharbuf, 0);
					GetActiveCommandQueue()->AddItemToFront(pRb);

					*SDS_cmdcharbuf = 0;
					if(!SDS_NoCharEcho)
						sds_printf("\n"/*DNT*/);
					break;

				case SDS_ESCAPECHAR:
					++SDS_userbreak;
					*SDS_cmdcharbuf = 0;
					SDS_MultibleMode = SDS_LispPCount = 0;
					if(SDS_CMainWindow->m_bExpInsertState)
					{
						SDS_CMainWindow->m_bExpInsertState=FALSE;
						if(SDS_CMainWindow->m_pExplorer)
							ExecuteUIAction( ICAD_WNDACTION_DELEXP );
					}

					GetScriptQueue()->Flush();

					while(!GetMenuQueue()->IsEmpty())
						GetMenuQueue()->AdvanceNext();

					break;

				default:

			defgoto:

					if(length < (sizeof(SDS_cmdcharbuf) - 1) && length < SDS_TILE_STR_LIMIT &&
						//important, do not add tabs to empty buffer
						!(strlen(SDS_cmdcharbuf) == 0 && sym == 9)) 

					{
						/*D.G.*/// Explanation of the following ASSERT.
						// Suppose user pressed some DBCS key and then pressed ESC. So we have 3 WM_CHARs:
						// the 1st - a leading byte, the 2nd - a trailing byte and the 3rd - '\0x1b'.
						// The 1st and the 2nd are just put into the input events queue and then are taken from it
						// (in the Engine thread) in SDS_WaitLoop and processed in this (SDS_ProcessInputEvent)
						// function. But the 3rd is processed (in the UI thread) in SDS_SendMessage before putting
						// it into the queue. During this preprocessing in SDS_SendMessage the SDS_userbreak flag
						// is set. And this causes calling of SDS_FreeEventList in SDS_AskForPoint.
						// SDS_FreeEventList sets *SDS_cmdcharbuf = 0. This calling can happen at any time - even
						// when we have processed only the 1st but not the 2nd yet.
						// So, it's for TO DO. At least we shouldn't process the 2nd at this point.
						if(bLeadingByte && !length)
						{
							ASSERT(0);
							bLeadingByte = false;
							break;
						}

						SDS_cmdcharbuf[length] = sym;
						SDS_cmdcharbuf[length + 1] = 0;

						if(!SDS_NoCharEcho)
						{
							if(bLeadingByte)
							{
								ASSERT(_ismbbtrail((unsigned int)(unsigned char)sym));
								sds_printf("%s"/*DNT*/, SDS_cmdcharbuf + length - 1);
								bLeadingByte = false;
							}
							else
							{
								if(_ismbblead((unsigned int)(unsigned char)sym))
									bLeadingByte = true;
								else
									sds_printf("%c"/*DNT*/, sym);
							}
						}
					}
			}	// switch(sym)

			// This is a very special callback for text input dragging only.
			if(SDS_IsDragging && SDS_CMainWindow->m_pDragVarsCur && SDS_CMainWindow->m_pDragVarsCur->mode == -5)
				(*SDS_CMainWindow->m_pDragVarsCur->scnf)(point, SDS_CMainWindow->m_pDragVarsCur->matx);

			break;
		}	// case SDS_EVM_KEYCHAR

		case SDS_EVM_MOUSEBUTTON:
			pRb = sds_newrb(RT3DPOINT);
			pEvent->GetPoint(pRb->resval.rpoint);
			GetActiveCommandQueue()->AddItemToFront(pRb);
			break;

		case SDS_EVM_MENUCOMMAND :
		case SDS_EVM_POPUPCOMMAND:
			if(SDS_IgnoreMenuWQuote && pEvent->m_menustr[0] == '\'')
				break;

			pRb = SDS_MnuStrToRB(pEvent->m_menustr);
			IC_FREE(pEvent->m_menustr);
			pEvent->m_menustr = NULL;

			if(pRb)
			{
				GetMenuQueue()->AddItem(pRb);
				pRb = NULL;
			}
			break;

		case SDS_EVM_AUTOEVENT:
			pRb = SDS_MnuStrToRB(pEvent->m_menustr);
			IC_FREE(pEvent->m_menustr);
			pEvent->m_menustr = NULL;

			if(pRb)
				GetActiveCommandQueue()->AddItemToFront(pRb);
	}	// switch(eventType)

	return RTNORM;
}


int SDS_FreeEventList(int NoMenu)
	{
	SDS_MultibleMode=0;

#if 1//rk
	if (GetActiveCommandQueue())
	{
		GetActiveCommandQueue()->Flush();
	}
#else
	GetActiveCommandQueue()->Flush();
#endif


	if(!NoMenu)
		{
		GetMenuQueue()->Flush();
		}

	*SDS_cmdcharbuf=0;

	if(SDSApplication::GetActiveApplication()->IsCommandRunning())
		{

		SDSApplication::GetActiveApplication()->SetCommandRunning( false );
		SDSApplication::GetActiveApplication()->ClearLispCommandDepth();
		// Modified PK 28/06/2000 [
		// Reason: Modification in fix for bug no. 56908
		/*if (SDS_IsDragging)
			{
			SDS_IsDragging = 0;
			ExecuteUIAction( ICAD_WNDACTION_ENDDRAG );
			ExecuteUIAction( ICAD_WNDACTION_FREEVIEWRBS );
			free(SDS_CMainWindow->m_pDragVarsCur);
			SDS_CMainWindow->m_pDragVarsCur=SDS_Tvars;
			}*/
		// Modified PK 28/06/2000 ]

		if ( SDSApplication::GetActiveApplication()->GetCommandThread()->IsCurrent() )
			{
			throw SDSThreadException( SDSThreadException::RESTARTCOMMANDTHREAD );
			}
		}

	return(RTNORM);
	}


/**************************************************************************/
/************** Init and Exit Functions ***********************************/
/**************************************************************************/

void SDS_FontSubsMsg(char *looked4,char *found) {
	if(looked4==NULL || *looked4==0 || found==NULL || *found==0 || cmd_InsideNewCommand)
		return;

	sds_printf(ResourceString(IDC_ICADAPI__NSUBSTITUTING__64, "\nSubstituting font [%s] for font [%s].\n" ),found,looked4);
}

#if defined(DEBUG)
void SDS_SetvarDebug(char *msg,int fi1) {
	sds_alert(msg);
}
#endif

#if defined(USE_SMARTHEAP)
MEM_BOOL __stdcall SDS_SmartHeapErrorHandler(MEM_ERROR_INFO MEM_FAR *einfo) {
	SDS_FatalErrorFunc(0,NULL);
	return(FALSE);
}
#endif


int SDS_Init(HWND hwnd, LPARAM JustReg) {
	char fs1[IC_ACADBUF];

	if(JustReg) {

		#if defined(USE_SMARTHEAP)
			MemSetErrorHandler(SDS_SmartHeapErrorHandler);
		#endif

		// Create the config and the session buffer.
		db_makesvbuf(2,&SDS_CURSES,NULL);
		db_makesvbuf(1,&SDS_CURCFG,NULL);


		IcadSharedGlobals::SetGetVar(SDS_GetVarSimple);
		IcadSharedGlobals::SetPreviewBMFunc(SDS_GetPreviewBmp);

		short cursorSize = IcadCursor::AppCursor().GetCrossHairSize();
		sds_setvar("CURSORSIZE", AutoResbuf( cursorSize));

		SDS_CMainWindow->m_pFileIO.drw_setconfigbuf(&SDS_CURCFG);

		// Read all of the registry vars into memory.
		SDS_RegToVars();
		return(RTNORM);
	}

	// Set up the correct colors into the palette colors.
	SDS_CreateOurColorMap();


	// Bail if its already inited.
	if(SDS_inited) return(FALSE);

	// Store the Main UI Thread ID for later
	//
	IDS_GetThreadController()->SetMainThreadID( ::GetCurrentThreadId() );

//	  sds_printf(ResourceString(IDC_ICADAPI_WELCOME_TO_INTE_65, "Welcome to IntelliCAD 98" ));

#if defined(DEBUG)
//	db_seterrormessfnp(SDS_SetvarDebug);
#endif

	// Init lisp.
	SDS_InitLisp();

	// Set the current cursor type.
	SDS_SetCursor(IDC_ICAD_CROSS);

	// Set the global window handle.
	IcadSharedGlobals::SetMainHWND(hwnd);

	// Init the Dialog loader.
	CString	path = FileString::ModuleFile().GetDrive() + FileString::ModuleFile().GetDirectory();
	path += ResourceString( IDS_DCL_ERROR_LOG, "ERROR.LOG");
	lsp_init_dialog( "", TRUE, path);

	// Set up the time span for the autosave.
	SDS_SaveTime = new CTime(time(NULL));

	extern char *cmd_FontSavePath;
	if(RTNORM==sds_findfile(SDS_PROGRAMFILE,fs1)) {
		strcpy(&fs1[strlen(fs1)-strlen(SDS_PROGRAMFILE)],"Fonts\\"/*DNT*/);
		db_astrncpy(&cmd_FontSavePath,fs1,-1);
	}

	db_setfontmap(NULL); // Initialize to use the internal resource
	struct resbuf rb;
	if(RTNORM==SDS_getvar(NULL,DB_QFONTMAP,&rb,NULL,&SDS_CURCFG,&SDS_CURSES)) {
		if(RTNORM==sds_findfile(rb.resval.rstring,fs1)) db_setfontmap(fs1);
		IC_FREE(rb.resval.rstring);
	}

	// Fire up the command line.
	SDS_cmdline(hwnd);

	// Create a command pipeline for lisp.
	SDSApplication::GetEngineApplication()->StartLispCommandThread();

	SDS_inited=1;

	// Now everything is done resume the command line.
//	SDSApplication::GetEngineApplication()->GetMainThread()->Resume();

	//*** Set the location of the MDI window (maximized/minimized/normal)
	SDS_CMainWindow->m_nIsClosing=0;

	SDS_EventMask(SDS_EVM_KEYCHAR|SDS_EVM_MOUSEBUTTON|SDS_EVM_MENUCOMMAND|SDS_EVM_POPUPCOMMAND);

	return(TRUE);
}

void SDS_Run()
{
	SDSApplication::GetEngineApplication()->GetMainThread()->Resume();
}

int SDS_Exit( void ) {

	IDS_SignalIcadExiting();

	// Bail if its not inited.
	if(!SDS_inited) return(FALSE);



	// Close any open log files or script recording.
	cmd_stopmacro();
	if(SDS_CMainWindow->m_fLogFile.m_hFile) {;
		SDS_CMainWindow->m_fLogFile.Close();
		SDS_CMainWindow->m_fLogFile.m_hFile=0;
	}

	cmd_InitRender(0);

	// Save all vars to the registry.
	SDS_VarsToReg();

	// Destroy the prompt menu.
	SDS_PromptMenu(-1);

	// Free up anything created from the graphics stuff.
	gr_clean();

	// Free the Lisp memory.
	lsp_lispclean();

	// Free the Alias list
	SDS_FreeAlias();

	// Free memory allocated for internal commands array
	SDS_Unloadfuncs();


	// Free the Dialog loader.
//	dlg_Uninitialize(SDS_hwnd);

	// Free the command history list.
	SDS_FreeCmdHist();

	// Free the list of fonts loadedf from fontmap.
	db_freefontmap();

	// Free any script file buffers.
	GetScriptQueue()->Flush();
	GetCancelScriptQueue()->Flush();


	// Clean up the parlink structure created in cmds12.
	cmd_DeleteDimStructAndStrings();



	// This is synchronous
	//
	SDSApplication::UnloadAllExternalApps();
	// Now all external apps have been unloaded

	// Terminate the main engine threads
	//
	SDSApplication *pEngine;
	pEngine = SDSApplication::GetEngineApplication();
	SDSApplication::RemoveLink( pEngine );

	pEngine->TerminateMainThread();
	pEngine->TerminateCommandThread();

	// Give threads time to terminate. This fixes a crash on exit on Windows 2000.
	// AG: we should avoid such sleep waitings
	//Sleep(100);

	delete pEngine;

	// Flush the input queue
	//
	IDS_GetInputEventQueue()->Shutdown();

	extern char *cmd_DwgSavePath;
	extern char *cmd_MenuSavePath;
	extern char *cmd_MacroSavePath;
	extern char *cmd_DxfSavePath;
	extern char *cmd_FontSavePath;
	extern char *cmd_LinSavePath;
	extern char *cmd_SlideSavePath;
	extern char *cmd_BMPSavePath;
	extern char *cmd_InsertSavePath;

	delete cmd_DwgSavePath;
	delete cmd_MenuSavePath;
	delete cmd_MacroSavePath;
	delete cmd_DxfSavePath;
	delete cmd_FontSavePath;
	delete cmd_LinSavePath;
	delete cmd_SlideSavePath;
	delete cmd_BMPSavePath;
	delete cmd_InsertSavePath;

	if(cmd_access(SDS_SHELLFILE)) _unlink(SDS_SHELLFILE);

	delete SDS_SaveTime; SDS_SaveTime=NULL;
	if(SDS_FreeKList) { IC_FREE(SDS_FreeKList); SDS_FreeKList=NULL; }

	SDS_inited=0;

	return(TRUE);
}

int SDS_AutoLoad(void)
	{

	char   fs1[IC_ACADBUF];
	char   fs2[IC_ACADBUF];

	static int DidSdsFile = 0;
	struct resbuf rb;

	if(!DidSdsFile)
		{

		if(RTNORM==sds_findfile(SDS_LOADSDSFILE,fs1))
			{
			FILE *fptr;
			if ((fptr=fopen(fs1,"rt"/*DNT*/))==NULL)
				{
				return(RTERROR);
				}

			while (ic_nfgets(fs1,sizeof(fs1)-1,fptr))
				{
				ic_trim(fs1,"be"/*DNT*/);
				// EBATECH(CNBR) -[ 2002/8/12 Bugzilla#78269 ICAD.SDS ignores line comment.
				if(*fs1 == ';' ) continue;
				// EBATECH(CNBR) ]-
				sds_xload(fs1);
				}

			fclose(fptr);
			}

		DidSdsFile=1;
		}

	//Modified Cybage SBD 28/05/2001 DD/MM/YYYY
	//Reason : Mail from Ronald Prepchuk(ronp@intellicad.org) Dated 24/05/2001 DD/MM/YYYY.
	//Re:Remove entries related to XRM
#ifdef BUILD_WITH_XRM_SUPPORT
	// Load Xref Manager.
	if (RTNORM == sds_findfile("xrm.dll", fs1))
	{
		sds_xload(fs1);
	}
	//Modified Cybage SBD 28/05/2001 DD/MM/YYYY
	//Reason : Mail from Ronald Prepchuk(ronp@intellicad.org) Dated 24/05/2001 DD/MM/YYYY.
	//Re:Remove entries related to XRM
#endif

#ifdef BUILD_WITH_IMAGEMENU_SUPPORT
	// Load imagemenu application
	if (RTNORM == sds_findfile("imagemenu.dll", fs1))
	{
		sds_xload(fs1);
	}
	//Re:Remove entries related to XRM
#endif // BUILD_WITH_IMAGEMENU_SUPPORT


#ifdef BUILD_WITH_RASTER_SUPPORT
	if(CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::RASTER))
	{
		if (RTNORM == sds_findfile("sdsimage.dll", fs1))
		{
			sds_xload(fs1);
		}

		if (RTNORM == sds_findfile("hiisras.dll", fs1))
		{
			sds_xload(fs1);
		}

		/*
		if (RTNORM == sds_findfile("spenable.lsp", fs1))
		{
			if(0==access(fs1,0))
			{
				for(int fi1=0; fs1[fi1]!=0; fi1++)
				{
					if(fs1[fi1]=='\\')
					{
						fs1[fi1]='/';
					}
				}

				sprintf(fs2,"(progn (load \"%s\")(if (/= s::startup nil)(s::startup))(princ))"/*DNT* /,fs1);

				{
					// this string gets freed in SDS_ProcessInputEvent
					//
					char* pTemp = (char *)malloc(strlen((char *)fs2)+1);
					strcpy( pTemp, (char *)fs2);

					InputEvent	theEvent( SDS_EVM_POPUPCOMMAND, pTemp );
					SDS_ProcessInputEvent( &theEvent);
				}
			} // end of access
		}
		*/
	} // if(...IsAccessible...
#endif

	// Look for and load icad.lsp on startup.
	if(RTNORM==sds_findfile(SDS_AUTOLOADLISP,fs1))
		{

		// We don't have to give the path to load, it does its own findfile.
		if(0==access(fs1,0))
			{
			for(int fi1=0; fs1[fi1]!=0; fi1++)
				{
				if(fs1[fi1]=='\\')
					{
					fs1[fi1]='/';
					}
				// EBATECH(R.Arayashiki)
				else if ((_MBC_LEAD ==_mbbtype((unsigned char)fs1[fi1],0)) &&
					  (_MBC_TRAIL==_mbbtype((unsigned char)fs1[fi1+1],1)))
					{
					fi1 ++;
					}
				//]-EBATECH
				}

/*
		   sprintf(fs2,"(LOAD \"DNT%s\ResourceString(IDC_ICADAPI___43, ")" ),fs1);
			event.type=SDS_EVM_POPUPCOMMAND;
			event.menustr=(char *)malloc(strlen((char *)fs2)+1);
			strcpy(event.menustr,(char *)fs2);
			SDS_ProcessInputEvent(&event);
		   strcpy(fs2,"(if (/= s::startup niDNT(s::startup))" ));
			event.type=SDS_EVM_POPUPCOMMAND;
			event.menustr=(char *)malloc(strlen((char *)fs2)+1);
			strcpy(event.menustr,(char *)fs2);
			SDS_ProcessInputEvent(&event);
*/

			sprintf(fs2,"(progn (load \"%s\")(if (/= s::startup nil)(s::startup))(princ))"/*DNT*/,fs1);
				{

				// this string gets freed in SDS_ProcessInputEvent
				//
				char* pTemp = new char [strlen((char *)fs2)+1];
				strcpy( pTemp, (char *)fs2);

				InputEvent	theEvent( SDS_EVM_AUTOEVENT, pTemp );
				SDS_ProcessInputEvent( &theEvent);
				}
			} // end of access
		}

	// ***************************************
	//*** Load the mnl for the drawing's menu.
	//
	if(RTNORM==SDS_getvar(NULL,DB_QMENUNAME,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))
		{
		strcpy(fs1,rb.resval.rstring);
		IC_FREE(rb.resval.rstring); rb.resval.rstring=NULL;

		cmd_loadlispmnl(fs1);
		}

	// Load the mnl for the current menu
	CIcadMenu* pMenu = SDS_CMainWindow->GetCurrentMenu();
	if(NULL!=pMenu)
		{
		VERIFY( pMenu->LoadMNLFilesFromLoadedMenuList() );
		}

	return(RTNORM);
}

int SDS_InitLisp(void)
	{

	// This is a special case to fix the problem with a call to sds_command() for open
	// or new.	We have to delay the the re-init of lisp.
	//
	// Specifically, we should only run SDS_InitLisp from the main command thread.	The
	// main command thread is in SDSApplication::GetEngineApplication()
	//	There is of course an exception even to this, that being before any threads are
	// created, which is the case when SDSApplication::GetEngineApplication() == NULL
	//
	if ( (SDSApplication::GetEngineApplication() != NULL) &&
		 (!SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent()) )
		{
		SDS_DelayLispInit=1;
		return(RTNORM);
		}

	SDSApplication *ptempApp;

	// Free up the previous atoms list.
	lsp_freeatomsll();

	// Init the Lisp memory with a fake lisp call.
	lsp_lispev("(princ)"/*DNT*/,NULL,NULL,NULL);

	// Load all of the command line functions
	SDS_loadfuncs();

	// Find the thread this function belongs to.
	if(SDSApplication::GetEngineApplication())
		{
		for(ptempApp=SDSApplication::GetEngineApplication()->GetNextLink(); ptempApp!=NULL; ptempApp=ptempApp->GetNextLink())
			{
// AG. I commented this out to make possible to proceed with processing CommandQueue after loading new document
// running script is an example of such behaviour
/*			  ////////// TESTING
			ptempApp->GetCommandQueue()->Copy( SDSApplication::GetEngineApplication()->GetCommandQueue() );
			SDSApplication::GetEngineApplication()->GetCommandQueue()->Flush();
*/
			SDS_LoadedAppsLast=SDSApplication::GetEngineApplication();
			/////////

			ptempApp->SetRQCode( SDS_RQXLOAD );
			// ****************************************
			// THREADSWAP !!!!!!!!!!!!!!!!!
				{
				SDSApplication *pStartApp = ptempApp;
				SDSApplication *pStopApp = SDSApplication::GetEngineApplication();
				if(FALSE==SDS_ThreadSwap(pStartApp->GetMainThread(),pStopApp->GetMainThread()))
					{
					return(RTERROR);
					}
				}
			}
		}

	return(RTNORM);
	}

/**************************************************************************/
/****************** General Support Functions *****************************/
/**************************************************************************/

int SDS_GetGlobals(char *appname,HWND *hwnd,HINSTANCE *hInstance) {
	strncpy(appname,SDS_AppName,IC_ACADBUF-1);
	*hwnd = IcadSharedGlobals::GetMainHWND();
	*hInstance=SDS_appinstance;
	return(RTNORM);
}




int SDS_rpextents(const sds_point extmin,const sds_point extmax,
					sds_point viewll, sds_point viewur,gr_view *view2use){
	//this f'n tests all 8 pts defined by the cube corners extmin&extmax
	//in WCS COORDINATES and returns the upper right & lower left view's
	//points expressed in RP COORDINATES
	//NOTE: for speed purposes, all transformations done here rather than in
	//	2dptmaxmin
	//NOTE:  any calls to sds_trans MUST use the transformation defined in
	//	view2use, NOT the current view!!

	sds_point pchk[8],pucs,prp;
	struct gr_view *view;
	int i,gotit=0;

	if(view2use==NULL)view=SDS_CURGRVW;
	else view=view2use;

	if(view==NULL) return(RTERROR);
	ic_ptcpy(pchk[0],extmin);
	ic_ptcpy(pchk[1],extmin);
	ic_ptcpy(pchk[2],extmin);
	ic_ptcpy(pchk[3],extmin);
	pchk[1][0]=extmax[0];
	pchk[2][1]=extmax[1];
	pchk[3][2]=extmax[2];

	ic_ptcpy(pchk[4],extmax);
	ic_ptcpy(pchk[5],extmax);
	ic_ptcpy(pchk[6],extmax);
	ic_ptcpy(pchk[7],extmax);
	pchk[5][0]=extmin[0];
	pchk[6][1]=extmin[1];
	pchk[7][2]=extmin[2];

	for(i=0;i<8;i++){
		pchk[i][0]-=view->ucsorg[0];
		pchk[i][1]-=view->ucsorg[1];
		pchk[i][2]-=view->ucsorg[2];
		pucs[0]=pchk[i][0]*view->ucsaxis[0][0]+pchk[i][1]*view->ucsaxis[0][1]+pchk[i][2]*view->ucsaxis[0][2];
		pucs[1]=pchk[i][0]*view->ucsaxis[1][0]+pchk[i][1]*view->ucsaxis[1][1]+pchk[i][2]*view->ucsaxis[1][2];
		pucs[2]=pchk[i][0]*view->ucsaxis[2][0]+pchk[i][1]*view->ucsaxis[2][1]+pchk[i][2]*view->ucsaxis[2][2];
		//pucs is now pchk[i] transformed into ucs
		if(gr_ucs2rp(pucs,prp,view))continue;//perpective w/target behind camera
		if(gotit==0){	//init the lower and upper pts we're using
			ic_ptcpy(viewll,prp);
			ic_ptcpy(viewur,prp);
		}else{
			SDS_2dptmaxmin(viewll,viewur,prp);
		}
		gotit++;
	}
	if(gotit>1)
		return(RTNORM);
	else
		return(RTERROR);
}

void SDS_cube_ptcheck(sds_point curmin,sds_point curmax,sds_point tpt1){
	if(tpt1[0]<curmin[0])curmin[0]=tpt1[0];
	else if(tpt1[0]>curmax[0])curmax[0]=tpt1[0];
	if(tpt1[1]<curmin[1])curmin[1]=tpt1[1];
	else if(tpt1[1]>curmax[1])curmax[1]=tpt1[1];
	if(tpt1[2]<curmin[2])curmin[2]=tpt1[2];
	else if(tpt1[2]>curmax[2])curmax[2]=tpt1[2];
}

int SDS_cubeUCStoWCS(const sds_point ucsextmin,const sds_point ucsextmax,
					sds_point wcsextmin, sds_point wcsextmax){
	//this f'n tests all 8 pts defined by the cube corners ucsextmin&ucsextmax
	//in UCS COORDINATES and returns the extents converted to WCS

	sds_point curmax,curmin,tpt1,tpt2;
	struct resbuf rbucs,rbwcs;
	int ret;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.rbnext=rbwcs.rbnext=NULL;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	//check the two corners we got first
	if(RTNORM!=(ret=sds_trans(ucsextmin,&rbucs,&rbwcs,0,tpt1)))return(ret);
	if(RTNORM!=(ret=sds_trans(ucsextmax,&rbucs,&rbwcs,0,tpt2)))return(ret);
	curmin[0]=min(tpt1[0],tpt2[0]);
	curmax[0]=max(tpt1[0],tpt2[0]);
	curmin[1]=min(tpt1[1],tpt2[1]);
	curmax[1]=max(tpt1[1],tpt2[1]);
	curmin[2]=min(tpt1[2],tpt2[2]);
	curmax[2]=max(tpt1[2],tpt2[2]);
	ic_ptcpy(tpt1,ucsextmin);
	tpt1[0]=ucsextmax[0];
	if(RTNORM!=(ret=sds_trans(tpt1,&rbucs,&rbwcs,0,tpt2)))return(ret);
	SDS_cube_ptcheck(curmin,curmax,tpt2);
	tpt1[1]=ucsextmax[1];
	if(RTNORM!=(ret=sds_trans(tpt1,&rbucs,&rbwcs,0,tpt2)))return(ret);
	SDS_cube_ptcheck(curmin,curmax,tpt2);
	tpt1[0]=ucsextmin[0];
	if(RTNORM!=(ret=sds_trans(tpt1,&rbucs,&rbwcs,0,tpt2)))return(ret);
	SDS_cube_ptcheck(curmin,curmax,tpt2);
	tpt1[2]=ucsextmax[2];
	if(RTNORM!=(ret=sds_trans(tpt1,&rbucs,&rbwcs,0,tpt2)))return(ret);
	SDS_cube_ptcheck(curmin,curmax,tpt2);
	tpt1[1]=ucsextmin[1];
	if(RTNORM!=(ret=sds_trans(tpt1,&rbucs,&rbwcs,0,tpt2)))return(ret);
	SDS_cube_ptcheck(curmin,curmax,tpt2);
	tpt1[0]=ucsextmax[0];
	if(RTNORM!=(ret=sds_trans(tpt1,&rbucs,&rbwcs,0,tpt2)))return(ret);
	SDS_cube_ptcheck(curmin,curmax,tpt2);
	ic_ptcpy(wcsextmin,curmin);
	ic_ptcpy(wcsextmax,curmax);
	return(ret);
}


void SDS_2dptmaxmin(sds_point minpt, sds_point maxpt, sds_point testpt){
	minpt[0]=min(minpt[0],testpt[0]);
	minpt[1]=min(minpt[1],testpt[1]);
	maxpt[0]=max(maxpt[0],testpt[0]);
	maxpt[1]=max(maxpt[1],testpt[1]);
	return;
}


/**************************************************************************/
/***************** Registry Functions *************************************/
/**************************************************************************/

// EBATECH(CNBR) -[ Save each system variable
//int SDS_VarsToReg(void) {
//	PreferenceKey		configKey(HKEY_CURRENT_USER, "Config");
//	HKEY hKey = configKey;
//
//	if(hKey == NULL) {
//		return(RTERROR);
//	}
//
//	RegSetValueEx(hKey,"SV_SIZE"/*DNT*/,0,REG_DWORD ,(const unsigned char *)&SDS_CURCFG.size,sizeof(SDS_CURCFG.size));
//	RegSetValueEx(hKey,"SV_VARS"/*DNT*/,0,REG_BINARY,(const unsigned char *)SDS_CURCFG.buf,SDS_CURCFG.size);
//
//	RegCloseKey(hKey);
//	return(RTNORM);
//}
int SDS_VarsToReg(void) {
	PreferenceKey		configKey(HKEY_CURRENT_USER, "Config");
	HKEY hKey = configKey;
	struct db_sysvarlink *sysvar=db_Getsysvar();
	int total_num=db_Getnvars(),i;
	//double	*dPtr;
	DWORD	iData, len;
	CString	str;

	if(hKey == NULL) {
		return(RTERROR);
	}
	for( i = 0 ; i < total_num ; i++ ){	// Enum all old system variable
		if( sysvar[i].loc == 1 ){	// Save to Registry?
			switch (sysvar[i].type) {
			case RTSHORT:
				iData = (DWORD)(*(short*)(SDS_CURCFG.buf+sysvar[i].bc));
				RegSetValueEx(hKey, sysvar[i].name,0,REG_DWORD ,(LPBYTE)&iData,sizeof(DWORD));
				break;
			case RTLONG:
				RegSetValueEx(hKey, sysvar[i].name,0,REG_DWORD ,(LPBYTE)(SDS_CURCFG.buf+sysvar[i].bc),sizeof(DWORD));
				break;
			case RTSTR:
				len = strlen(SDS_CURCFG.buf+sysvar[i].bc);
				if (sysvar[i].nc < len ){
					 len = sysvar[i].nc;
				}
				RegSetValueEx(hKey, sysvar[i].name,0,REG_SZ ,(LPBYTE)(SDS_CURCFG.buf+sysvar[i].bc),len);
				break;
			/* if you save string instead binary, remove this comment. EBATECH(CNBR)
			case RTREAL:
				dPtr = (double*)(SDS_CURCFG.buf+sysvar[i].bc);
				str.Format("%lf", *dPtr );
				RegSetValueEx( hKey, sysvar[i].name,0,REG_SZ , (LPBYTE)((LPCTSTR)str), str.GetLength());
				break;
			case RTPOINT:
				dPtr = (double*)(SDS_CURCFG.buf+sysvar[i].bc);
				str.Format("%lf,%lf", *dPtr, *(dPtr+1) );
				RegSetValueEx( hKey, sysvar[i].name,0,REG_SZ , (LPBYTE)((LPCTSTR)str), str.GetLength());
				break;
			case RT3DPOINT:
				dPtr = (double*)(SDS_CURCFG.buf+sysvar[i].bc);
				str.Format("%lf,%lf,%lf", *dPtr, *(dPtr+1), *(dPtr+2));
				RegSetValueEx( hKey, sysvar[i].name,0,REG_SZ , (LPBYTE)((LPCTSTR)str), str.GetLength());
				break;
			*/
			case RTREAL:
			case RTPOINT:
			case RT3DPOINT:
				RegSetValueEx( hKey, sysvar[i].name,0,REG_BINARY , (LPBYTE)(SDS_CURCFG.buf+sysvar[i].bc), sysvar[i].nc );
			}
		}
	}
	//RegCloseKey(hKey);
	return(RTNORM);
}
// EBATECH(CNBR) ]-

// EBATECH(CNBR) -[
//#ifdef OUTFORNOW
//void StoreOldConfigVarsToNew(unsigned char *oldbuf)
//	{
//	db_sysvar *sv=IcadSharedGlobals::GetSysvarPtr();
//
//	ASSERT(sv!=NULL);
//	sv->SetAperture(*(short *)&oldbuf[0]);
//	sv->SetAuditctl(*(short *)&oldbuf[2]);
//	sv->SetAutomenuload(*(short *)&oldbuf[4]);
//	sv->SetBasefile((char *)&oldbuf[6]);
//	sv->SetBkgcolor(*(short *)&oldbuf[262]);
//	sv->SetBkgredraw(*(short *)&oldbuf[264]);
//	sv->SetCmddia(*(short *)&oldbuf[266]);
//	sv->SetCmdlntext((char *)&oldbuf[268]);
//	sv->SetColorx(*(short *)&oldbuf[319]);
//	sv->SetColory(*(short *)&oldbuf[321]);
//	sv->SetColorz(*(short *)&oldbuf[323]);
//	sv->SetDctcust((char *)&oldbuf[325]);
//	sv->SetDctmain((char *)&oldbuf[376]);
//	sv->SetDragopen(*(short *)&oldbuf[427]);
//	sv->SetDragp1(*(short *)&oldbuf[429]);
//	sv->SetDragp2(*(short *)&oldbuf[431]);
//	sv->SetExplevel(*(short *)&oldbuf[433]);
//	sv->SetExplstblk((char *)&oldbuf[435]);
//	sv->SetExplstds((char *)&oldbuf[460]);
//	sv->SetExplstlay((char *)&oldbuf[485]);
//	sv->SetExplstlt((char *)&oldbuf[510]);
//	sv->SetExplstts((char *)&oldbuf[535]);
//	sv->SetExplstucs((char *)&oldbuf[560]);
//	sv->SetExplstvw((char *)&oldbuf[585]);
//	sv->SetFflimit(*(short *)&oldbuf[610]);
//	sv->SetFiledia(*(short *)&oldbuf[612]);
//	sv->SetFontalt((char *)&oldbuf[614]);
//	sv->SetFontmap((char *)&oldbuf[665]);
//	sv->SetGripblock(*(short *)&oldbuf[716]);
//	sv->SetGripcolor(*(short *)&oldbuf[718]);
//	sv->SetGriphot(*(short *)&oldbuf[720]);
//	sv->SetGrips(*(short *)&oldbuf[722]);
//	sv->SetGripsize(*(short *)&oldbuf[724]);
//	sv->SetIsavepercent(*(short *)&oldbuf[726]);
//	sv->SetLoginname((char *)&oldbuf[728]);
//	sv->SetLongfname(*(short *)&oldbuf[779]);
//	sv->SetLwpolyout(*(short *)&oldbuf[781]);
//	sv->SetMakebak(*(short *)&oldbuf[783]);
//	sv->SetMaxsort(*(short *)&oldbuf[785]);
//	sv->SetMenuctl(*(short *)&oldbuf[787]);
//	sv->SetMtexted((char *)&oldbuf[789]);
//	sv->SetMwheeldn((char *)&oldbuf[840]);
//	sv->SetMwheelup((char *)&oldbuf[891]);
//	sv->SetNfilelist(*(short *)&oldbuf[942]);
//	sv->SetPickadd(*(short *)&oldbuf[944]);
//	sv->SetPickauto(*(short *)&oldbuf[946]);
//	sv->SetPickbox(*(short *)&oldbuf[948]);
//	sv->SetPickdrag(*(short *)&oldbuf[950]);
//	sv->SetPickfirst(*(short *)&oldbuf[952]);
//	sv->SetPlinetype(*(short *)&oldbuf[954]);
//	sv->SetPlotid((char *)&oldbuf[956]);
//	sv->SetPlotrotmode(*(short *)&oldbuf[1007]);
//	sv->SetPlotter(*(short *)&oldbuf[1009]);
//	sv->SetProjmode(*(short *)&oldbuf[1011]);
//	sv->SetPromptmenu(*(short *)&oldbuf[1013]);
//	sv->SetPsprolog((char *)&oldbuf[1015]);
//	sv->SetPsquality(*(short *)&oldbuf[1066]);
//	sv->SetQaflags(*(short *)&oldbuf[1068]);
//	sv->SetRasterpreview(*(short *)&oldbuf[1070]);
//	sv->SetSavefile((char *)&oldbuf[1072]);
//	sv->SetSavetime(*(short *)&oldbuf[1123]);
//	sv->SetScreenboxes(*(short *)&oldbuf[1125]);
//	sv->SetScreenmode(*(short *)&oldbuf[1127]);
//	sv->SetScreenpos((double *)&oldbuf[1129]);
//	sv->SetScreensize((double *)&oldbuf[1145]);
//	sv->SetScrlhist(*(short *)&oldbuf[1161]);
//	sv->SetSortents(*(short *)&oldbuf[1163]);
//	sv->SetSrchpath((char *)&oldbuf[1165]);
//	sv->SetTbcolor(*(short *)&oldbuf[2189]);
//	sv->SetTbsize(*(short *)&oldbuf[2191]);
//	sv->SetTempprefix((char *)&oldbuf[2193]);
//	sv->SetTextfill(*(short *)&oldbuf[2244]);
//	sv->SetTipofday(*(short *)&oldbuf[2246]);
//	sv->SetTooltips(*(short *)&oldbuf[2248]);
//	sv->SetTreemax(*(long *)&oldbuf[2250]);
//	sv->SetUcsiconpos(*(short *)&oldbuf[2254]);
//	sv->SetWndbtexp(*(short *)&oldbuf[2256]);
//	sv->SetWndlcmd(*(short *)&oldbuf[2258]);
//	sv->SetWndlmain(*(short *)&oldbuf[2260]);
//	sv->SetWndlmdi(*(short *)&oldbuf[2262]);
//	sv->SetWndlscrl(*(short *)&oldbuf[2264]);
//	sv->SetWndlstat(*(short *)&oldbuf[2266]);
//	sv->SetWndltexp(*(short *)&oldbuf[2268]);
//	sv->SetWndltext(*(short *)&oldbuf[2270]);
//	sv->SetWndpcmd((double *)&oldbuf[2272]);
//	sv->SetWndpform((double *)&oldbuf[2288]);
//	sv->SetWndpmain((double *)&oldbuf[2304]);
//	sv->SetWndppmenu((double *)&oldbuf[2320]);
//	sv->SetWndptexp((double *)&oldbuf[2336]);
//	sv->SetWndptext((double *)&oldbuf[2352]);
//	sv->SetWndscmd((double *)&oldbuf[2368]);
//	sv->SetWndsform((double *)&oldbuf[2384]);
//	sv->SetWndsmain((double *)&oldbuf[2400]);
//	sv->SetWndstexp((double *)&oldbuf[2416]);
//	sv->SetWndstext((double *)&oldbuf[2432]);
//	sv->SetXloadctl(*(short *)&oldbuf[2448]);
//	sv->SetXloadpath((char *)&oldbuf[2450]);
//	sv->SetXrefctl(*(short *)&oldbuf[3474]);
//	}
//#endif
//
//int SDS_RegToVars(void)
//{
//
//	PreferenceKey		configKey(HKEY_CURRENT_USER, "Config", FALSE);
//	HKEY hKey;
//	DWORD type;
//	long fl1;
//	int fi1;
//	ULONG fl2;
//	struct resbuf rb;
//
//	if (ERROR_SUCCESS!=RegOpenKeyEx(configKey,"",0,KEY_READ | KEY_WRITE,&hKey))
//		return(RTERROR);
//
//	fl2=sizeof(fl1);
//	if (ERROR_SUCCESS==RegQueryValueEx(hKey,"SV_SIZE"/*DNT*/,NULL,&type,  // old one exists
//		(unsigned char *)&fl1,&fl2) && fl1==SDS_CURCFG.size)
//		{
//		unsigned char oldsvbuf[4096];
//		fl2=4096;
//		RegQueryValueEx(hKey,"SV_VARS"/*DNT*/,NULL,&type,oldsvbuf,&fl2);
//#ifdef OUTFORNOW
//		StoreOldConfigVarsToNew(oldsvbuf);
// REMOVE THIS WHEN DONE WITH NEW SYSVAR STUFF
//#endif
//		memcpy(SDS_CURCFG.buf,oldsvbuf,SDS_CURCFG.size);
//		}
//
//	// Check for install vars.
//	rb.restype=RTSHORT;
//	fl2=sizeof(fi1);
//	if (ERROR_SUCCESS==RegQueryValueEx(hKey,"INSTALL_EXPLEVEL"/*DNT*/,NULL,&type,
//		(LPBYTE)&fi1,&fl2))
//		{
//		rb.resval.rint=fi1;
//		if(RTNORM==SDS_setvar(NULL,DB_QEXPLEVEL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0))
//			{
//			RegDeleteValue(hKey,"INSTALL_EXPLEVEL"/*DNT*/);
//			}
//		}
//
//	RegCloseKey(hKey);
//	return(RTNORM);
//}
int SDS_RegToVars(void)
{

	PreferenceKey		configKey(HKEY_CURRENT_USER, "Config");
	HKEY hKey = configKey;
	DWORD type;
	long fl1;
	int fi1;
	ULONG fl2;
	struct resbuf rb;
	bool done = false;

	if( hKey == NULL ){
		return(RTERROR);
	}
	// read combined system variables from registry
	fl2=sizeof(fl1);
	if (ERROR_SUCCESS==RegQueryValueEx(hKey,"SV_SIZE"/*DNT*/,NULL,&type,  // old one exists
		(unsigned char *)&fl1,&fl2) && fl1==SDS_CURCFG.size){
			char * oldsvbuf = new char [fl1];
			if( oldsvbuf ){
				fl2=fl1;
				if(ERROR_SUCCESS == RegQueryValueEx(hKey,"SV_VARS"/*DNT*/,NULL,&type,(LPBYTE)oldsvbuf,&fl2)){
					memcpy(SDS_CURCFG.buf,oldsvbuf,SDS_CURCFG.size);
					done = true;
				}
				delete [] ((LPVOID)oldsvbuf);
			}
	}
	// read each system variables from registry
	if(done == false){
		struct db_sysvarlink *sysvar=db_Getsysvar();
		int total_num=db_Getnvars(),i;
		double*	dPtr;
		double* dPtr2;
		char* cPtr;
		char	tmpstr[256];
		DWORD	iData;

		for( i = 0 ; i < total_num ; i++ ){	// Enum all old system variable
			if( sysvar[i].loc == 1 ){	// Save to Registry?
				switch (sysvar[i].type) {
				case RTSHORT:
					fl2 = sizeof(DWORD);
					if(ERROR_SUCCESS == RegQueryValueEx(hKey,sysvar[i].name,NULL,&type,(LPBYTE)&iData,&fl2)){
						*(short*)(SDS_CURCFG.buf+sysvar[i].bc) = iData;
					}else{
						*(short*)(SDS_CURCFG.buf+sysvar[i].bc) = atoi(sysvar[i].defval);
					}
					break;
				case RTLONG:
					fl2 = sizeof(DWORD);
					if(ERROR_SUCCESS == RegQueryValueEx(hKey,sysvar[i].name,NULL,&type,(LPBYTE)&iData,&fl2)){
						*(long*)(SDS_CURCFG.buf+sysvar[i].bc) = iData;
					}else{
						*(long*)(SDS_CURCFG.buf+sysvar[i].bc) = atoi(sysvar[i].defval);
					}
					break;
				case RTSTR:
					fl2 = sysvar[i].nc;
					if(ERROR_SUCCESS != RegQueryValueEx(hKey,sysvar[i].name,NULL,&type,
						(LPBYTE)(SDS_CURCFG.buf+sysvar[i].bc),&fl2)){
						strncpy(SDS_CURCFG.buf+sysvar[i].bc, sysvar[i].defval, sysvar[i].nc);
					}
					break;
				case RTREAL:
					fl2 = sizeof(tmpstr);
					dPtr = (double *)(SDS_CURCFG.buf+sysvar[i].bc);
					dPtr2 = (double *)tmpstr;
					if(ERROR_SUCCESS != RegQueryValueEx(hKey,sysvar[i].name,NULL,&type,(LPBYTE)tmpstr,&fl2)){
						strcpy(tmpstr,sysvar[i].defval);
						type=REG_SZ;
					}
					if(type==REG_SZ){
						*dPtr = atof(tmpstr);
					}else{
						*dPtr = *dPtr2;
					}
					break;
				case RTPOINT:
					fl2 = sizeof(tmpstr);
					dPtr = (double *)(SDS_CURCFG.buf+sysvar[i].bc);
					dPtr2 = (double *)tmpstr;
					if(ERROR_SUCCESS != RegQueryValueEx(hKey,sysvar[i].name,NULL,&type,(LPBYTE)tmpstr,&fl2)){
						strcpy(tmpstr,sysvar[i].defval);
						type=REG_SZ;
					}
					if(type==REG_SZ){
						*dPtr = atof(tmpstr);
						cPtr = strchr(tmpstr, ',');
						*(dPtr+1) = cPtr ? atof(cPtr+1) : 0.0;
					}else{
						*dPtr = *dPtr2;
						*(dPtr+1) = *(dPtr2+1);
					}
					break;
				case RT3DPOINT:
					fl2 = sizeof(tmpstr);
					dPtr = (double *)(SDS_CURCFG.buf+sysvar[i].bc);
					dPtr2 = (double *)tmpstr;
					if(ERROR_SUCCESS != RegQueryValueEx(hKey,sysvar[i].name,NULL,&type,(LPBYTE)tmpstr,&fl2)){
						strcpy(tmpstr,sysvar[i].defval);
						type=REG_SZ;
					}
					if(type==REG_SZ){
						*dPtr = atof(tmpstr);
						cPtr = strchr(tmpstr, ',');
						*(dPtr+1) = cPtr ? atof(cPtr+1) : 0.0;
						if( cPtr ){
							cPtr = strchr(cPtr+1, ',');
							*(dPtr+2) = cPtr ? atof(cPtr+1) : 0.0;
						}else{
							*(dPtr+2) = 0.0;
						}
					}else{
						*dPtr = *dPtr2;
						*(dPtr+1) = *(dPtr2+1);
						*(dPtr+2) = *(dPtr2+2);
					}
					break;
				}
			}
		}
	}
	// Delete combined variables
	if (ERROR_SUCCESS==RegQueryValueEx(hKey,"SV_SIZE"/*DNT*/,NULL,NULL,NULL,&fl2)){
		RegDeleteValue(hKey,"SV_SIZE"/*DNT*/);
	}
	if (ERROR_SUCCESS==RegQueryValueEx(hKey,"SV_VARS"/*DNT*/,NULL,NULL,NULL,&fl2)){
		RegDeleteValue(hKey,"SV_VARS"/*DNT*/);
	}

	// Check for install vars.
	rb.restype=RTSHORT;
	fl2=sizeof(fi1);
	if (ERROR_SUCCESS==RegQueryValueEx(hKey,"INSTALL_EXPLEVEL"/*DNT*/,NULL,&type,
		(LPBYTE)&fi1,&fl2))
		{
		rb.resval.rint=fi1;
		if(RTNORM==SDS_setvar(NULL,DB_QEXPLEVEL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0))
			{
			RegDeleteValue(hKey,"INSTALL_EXPLEVEL"/*DNT*/);
			}
		}

	//RegCloseKey(hKey);
	return(RTNORM);
}
/**************************************************************************/
/***************** Viewport functions *************************************/
/**************************************************************************/

int SDS_SetActiveVPbyNum(int VPnum) {
	struct resbuf rb;
	SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(!rb.resval.rint) return(RTERROR);

	CIcadView *pView;
	CIcadDoc  *pDoc=SDS_CURDOC;
	if(pDoc==NULL) return(RTERROR);
	int fi1;

	POSITION pos=pDoc->GetFirstViewPosition();
	while (pos!=NULL) {
		pView=(CIcadView *)pDoc->GetNextView(pos);
		ASSERT_KINDOF(CIcadView, pView);

		pView->m_pVportTabHip->get_69(&fi1);
		if(fi1==VPnum) {
			SDS_CMainWindow->m_wndViewClose=pView;
			ExecuteUIAction( ICAD_WNDACTION_CHGCVPORT );
			SDS_CMainWindow->m_wndViewClose=NULL;
			return(RTNORM);
		}
	}
	return(RTERROR);
}

int	SDS_ViewToVars2Use(CIcadView *pView,db_drawing *dp,struct gr_viewvars  *viewvarsp) {

	if(dp==NULL) dp=SDS_CURDWG;
	if(dp==NULL || pView==NULL || pView->m_pVportTabHip==NULL) return(RTERROR);

	struct resbuf rb;
	int fi1;

	if(pView->m_pVportTabHip->ret_type()==DB_VIEWPORT) {
		pView->m_pVportTabHip->get_17(viewvarsp->target);
		pView->m_pVportTabHip->get_16(viewvarsp->viewdir);
		pView->m_pVportTabHip->get_51(&viewvarsp->viewtwist);
		pView->m_pVportTabHip->get_45(&viewvarsp->viewsize);
		pView->m_pVportTabHip->get_12(viewvarsp->viewctr);
		pView->m_pVportTabHip->get_42(&viewvarsp->lenslength);
		int l;
		pView->m_pVportTabHip->get_90(&l);
		viewvarsp->viewmode=(int)(l & 31);	// cheap, but easy to write!
		pView->m_pVportTabHip->get_72(&viewvarsp->curvdispqual);
	} else {
		pView->m_pVportTabHip->get_12(viewvarsp->viewctr);
		pView->m_pVportTabHip->get_17(viewvarsp->target);
		pView->m_pVportTabHip->get_16(viewvarsp->viewdir);
		pView->m_pVportTabHip->get_51(&viewvarsp->viewtwist);
		pView->m_pVportTabHip->get_40(&viewvarsp->viewsize);
		pView->m_pVportTabHip->get_42(&viewvarsp->lenslength);
		int	i;
		pView->m_pVportTabHip->get_71(&i);
		viewvarsp->viewmode=shortCast(i);
		pView->m_pVportTabHip->get_72(&viewvarsp->curvdispqual);
	}
	viewvarsp->screensize[0]=pView->m_iWinX;
	viewvarsp->screensize[1]=pView->m_iWinY;
	viewvarsp->screensize[2]=0.0;

	SDS_getvar(NULL,DB_QATTMODE,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	viewvarsp->attmode=rb.resval.rint;
	SDS_getvar(NULL,DB_QSPLFRAME,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	viewvarsp->splframe=rb.resval.rint;
	SDS_getvar(NULL,DB_QPDMODE,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	viewvarsp->pdmode=rb.resval.rint;
	SDS_getvar(NULL,DB_QFILLMODE,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	viewvarsp->fillmode=rb.resval.rint;
	SDS_getvar(NULL,DB_QQTEXTMODE,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	viewvarsp->qtextmode=rb.resval.rint;
	SDS_getvar(NULL,DB_QLTSCLWBLK,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	viewvarsp->ltsclwblk=rb.resval.rint;
	// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
	SDS_getvar(NULL,DB_QLWDISPLAY,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	viewvarsp->lwdisplay=rb.resval.rint;
	SDS_getvar(NULL,DB_QLWDEFAULT,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	viewvarsp->lwdefault=rb.resval.rint;
	// EBATECH(CNBR) ]-
	if(pView->m_pVportTabHip->ret_type()==DB_VIEWPORT && pView->m_pVportTabHip->get_69(&fi1) && fi1==1) {
		SDS_getvar(NULL,DB_QP_ELEVATION,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
		viewvarsp->elevation=rb.resval.rreal;
		SDS_getvar(NULL,DB_QP_UCSORG,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(viewvarsp->ucsorg,rb.resval.rpoint);
		SDS_getvar(NULL,DB_QP_UCSXDIR,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(viewvarsp->ucsxdir,rb.resval.rpoint);
		SDS_getvar(NULL,DB_QP_UCSYDIR,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(viewvarsp->ucsydir,rb.resval.rpoint);
	} else {
		SDS_getvar(NULL,DB_QELEVATION,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
		viewvarsp->elevation=rb.resval.rreal;
		SDS_getvar(NULL,DB_QUCSORG,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(viewvarsp->ucsorg,rb.resval.rpoint);
		SDS_getvar(NULL,DB_QUCSXDIR,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(viewvarsp->ucsxdir,rb.resval.rpoint);
		SDS_getvar(NULL,DB_QUCSYDIR,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(viewvarsp->ucsydir,rb.resval.rpoint);
	}
	SDS_getvar(NULL,DB_QPDSIZE,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	viewvarsp->pdsize=rb.resval.rreal;
	SDS_getvar(NULL,DB_QLTSCALE,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	viewvarsp->ltscale=rb.resval.rreal;
	return(RTNORM);
}


static void
z_saveVarsToLastViewport( db_drawing*dp, db_handitem *hip )
	{
	struct resbuf rb;

 	// Save all the current setvars into the last viewport
	db_qgetvar(DB_QTARGET ,dp->ret_headerbuf(),rb.resval.rpoint ,DB_3DPOINT,0);
	hip->set_17(rb.resval.rpoint);
	db_qgetvar(DB_QVIEWDIR ,dp->ret_headerbuf(),rb.resval.rpoint ,DB_3DPOINT,0);
	hip->set_16(rb.resval.rpoint);
	db_qgetvar(DB_QVIEWCTR ,dp->ret_headerbuf(),rb.resval.rpoint ,DB_3DPOINT,0);
	hip->set_12(rb.resval.rpoint);
	SDS_getvar(NULL,DB_QSNAPBASE,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	hip->set_13(rb.resval.rpoint);
	SDS_getvar(NULL,DB_QSNAPUNIT,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	hip->set_14(rb.resval.rpoint);
	SDS_getvar(NULL,DB_QGRIDUNIT,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	hip->set_15(rb.resval.rpoint);
	SDS_getvar(NULL,DB_QVIEWTWIST,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	hip->set_51(rb.resval.rreal);
	SDS_getvar(NULL,DB_QVIEWSIZE,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	hip->set_40(rb.resval.rreal);
	SDS_getvar(NULL,DB_QLENSLENGTH,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	hip->set_42(rb.resval.rreal);
	SDS_getvar(NULL,DB_QFRONTZ,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	hip->set_43(rb.resval.rreal);
	SDS_getvar(NULL,DB_QBACKZ,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	hip->set_44(rb.resval.rreal);
	SDS_getvar(NULL,DB_QSNAPANG,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	hip->set_50(rb.resval.rreal);
	SDS_getvar(NULL,DB_QVIEWMODE,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	hip->set_71(rb.resval.rint);
	SDS_getvar(NULL,DB_QZOOMPERCENT,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	hip->set_72(rb.resval.rint);
	SDS_getvar(NULL,DB_QFASTZOOM,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	hip->set_73(rb.resval.rint);
	SDS_getvar(NULL,DB_QUCSICON,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	hip->set_74(rb.resval.rint);
	SDS_getvar(NULL,DB_QSNAPMODE,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	hip->set_75(rb.resval.rint);
	SDS_getvar(NULL,DB_QGRIDMODE,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	hip->set_76(rb.resval.rint);
	SDS_getvar(NULL,DB_QSNAPSTYL,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	hip->set_77(rb.resval.rint);
	SDS_getvar(NULL,DB_QSNAPISOPAIR,&rb,dp,&SDS_CURCFG,&SDS_CURSES);
	hip->set_78(rb.resval.rint);
	}




int SDS_VarToTab(db_drawing *dp,db_handitem *hip) {

	if(dp==NULL) dp=SDS_CURDWG;
	if(dp==NULL) return(RTERROR);

	if(hip==NULL) return(RTERROR);

	if(hip->ret_type()==DB_VIEWPORT)
		{
		db_viewport *pViewport = (db_viewport *)hip;

		SDS_regapp( "ACAD"/*DNT*/, dp );


		VERIFY( pViewport->SetViewInfoToDefault( dp ) );

		}
	else
		{
		z_saveVarsToLastViewport( dp, hip );
	 	}
	return(RTNORM);
}

int	SDS_TabToVar(db_drawing *dp,db_handitem *hip)
	{

	if (dp==NULL)
		dp=SDS_CURDWG;
	if (dp==NULL)
		return(RTERROR);

	if (hip==NULL)
		return(RTERROR);

	struct resbuf rb;
	int fi1;

	if (hip->ret_type()==DB_VIEWPORT)
		{
		db_viewport *vp=(db_viewport *)hip;

		rb.restype=RT3DPOINT;
		vp->get_17(rb.resval.rpoint);
  		SDS_setvar(NULL,DB_QTARGET,&rb,dp,&SDS_CURCFG,&SDS_CURSES,1);

		vp->get_16(rb.resval.rpoint);
		SDS_setvar(NULL,DB_QVIEWDIR,&rb,dp,&SDS_CURCFG,&SDS_CURSES,1);

		rb.restype=RTREAL;
		vp->get_51(&rb.resval.rreal);
		SDS_setvar(NULL,DB_QVIEWTWIST,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);

		vp->get_45(&rb.resval.rreal);
		SDS_setvar(NULL,DB_QVIEWSIZE,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);

		rb.restype=RT3DPOINT;
		vp->get_12(rb.resval.rpoint);
		SDS_setvar(NULL,DB_QVIEWCTR,&rb,dp,&SDS_CURCFG,&SDS_CURSES,1);

		rb.restype=RTREAL;
		vp->get_42(&rb.resval.rreal);
		SDS_setvar(NULL,DB_QLENSLENGTH,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);

		vp->get_43(&rb.resval.rreal);
		SDS_setvar(NULL,DB_QFRONTZ,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);

		vp->get_44(&rb.resval.rreal);
		SDS_setvar(NULL,DB_QBACKZ,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);

		rb.restype=RTSHORT;
		int status;
		vp->get_90(&status);
		rb.resval.rint=status & 31;
		SDS_setvar(NULL,DB_QVIEWMODE,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);

		int c;
		vp->get_72(&c);
		rb.resval.rint=c;
		SDS_setvar(NULL,DB_QZOOMPERCENT,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);

		if (status & IC_VPENT_STATUS_FASTZOOM)
			rb.resval.rint=1;
		else
			rb.resval.rint=0;
		SDS_setvar(NULL,DB_QFASTZOOM,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);

		if (status & IC_VPENT_STATUS_UCSICON)
			rb.resval.rint=1;
		else
			rb.resval.rint=0;
		SDS_setvar(NULL,DB_QUCSICON,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);

		if (status & IC_VPENT_STATUS_SNAPON)
			rb.resval.rint=1;
		else
			rb.resval.rint=0;
		SDS_setvar(NULL,DB_QSNAPMODE,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);

		if (status & IC_VPENT_STATUS_GRIDON)
			rb.resval.rint=1;
		else
			rb.resval.rint=0;
		SDS_setvar(NULL,DB_QGRIDMODE,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);

		if (status & IC_VPENT_STATUS_SNAPSTYLEISO)
			rb.resval.rint=1;
		else
			rb.resval.rint=0;
		SDS_setvar(NULL,DB_QSNAPSTYL,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);

		rb.resval.rint=0;
		if (status & IC_VPENT_STATUS_SNAPISOPAIRTOP)
			rb.resval.rint=1;
		else if (status & IC_VPENT_STATUS_SNAPISOPAIRRIGHT)
			rb.resval.rint=2;
		SDS_setvar(NULL,DB_QSNAPISOPAIR,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);

		rb.restype=RTREAL;
		vp->get_50(&rb.resval.rreal);
		SDS_setvar(NULL,DB_QSNAPANG,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);

		rb.restype=RTPOINT;
		vp->get_13(rb.resval.rpoint);
		SDS_setvar(NULL,DB_QSNAPBASE,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);

		vp->get_14(rb.resval.rpoint);
		SDS_setvar(NULL,DB_QSNAPUNIT,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);

		vp->get_15(rb.resval.rpoint);
		SDS_setvar(NULL,DB_QGRIDUNIT,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);
		}
	else
		{
		// Set all the current vars to the current viewport.
		rb.restype=RT3DPOINT;
		hip->get_12(rb.resval.rpoint);
 		SDS_setvar(NULL,DB_QVIEWCTR,&rb,dp,&SDS_CURCFG,&SDS_CURSES,1);
		hip->get_13(rb.resval.rpoint);
		SDS_setvar(NULL,DB_QSNAPBASE,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);
		hip->get_14(rb.resval.rpoint);
		SDS_setvar(NULL,DB_QSNAPUNIT,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);
		hip->get_15(rb.resval.rpoint);
		SDS_setvar(NULL,DB_QGRIDUNIT,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);
		rb.restype=RT3DPOINT;
		hip->get_17(rb.resval.rpoint);
  		SDS_setvar(NULL,DB_QTARGET,&rb,dp,&SDS_CURCFG,&SDS_CURSES,1);
		hip->get_16(rb.resval.rpoint);
		SDS_setvar(NULL,DB_QVIEWDIR,&rb,dp,&SDS_CURCFG,&SDS_CURSES,1);
		rb.restype=RTREAL;
		hip->get_51(&rb.resval.rreal);
		SDS_setvar(NULL,DB_QVIEWTWIST,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);
		hip->get_40(&rb.resval.rreal);
		SDS_setvar(NULL,DB_QVIEWSIZE,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);
		hip->get_42(&rb.resval.rreal);
		SDS_setvar(NULL,DB_QLENSLENGTH,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);
		hip->get_43(&rb.resval.rreal);
		SDS_setvar(NULL,DB_QFRONTZ,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);
		hip->get_44(&rb.resval.rreal);
		SDS_setvar(NULL,DB_QBACKZ,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);
		hip->get_50(&rb.resval.rreal);
		SDS_setvar(NULL,DB_QSNAPANG,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);
		rb.restype=RTSHORT;
		hip->get_69(&fi1);
		rb.resval.rint=fi1;
		SDS_setvar(NULL,DB_QCVPORT,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);
		hip->get_71(&fi1);
		rb.resval.rint=fi1;
		SDS_setvar(NULL,DB_QVIEWMODE,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);
		hip->get_72(&fi1);
		rb.resval.rint=fi1;
		SDS_setvar(NULL,DB_QZOOMPERCENT,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);
		hip->get_73(&fi1);
		rb.resval.rint=fi1;
		SDS_setvar(NULL,DB_QFASTZOOM,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);
		hip->get_74(&fi1);
		rb.resval.rint=fi1;
		SDS_setvar(NULL,DB_QUCSICON,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);
		hip->get_75(&fi1);
		rb.resval.rint=fi1;
		SDS_setvar(NULL,DB_QSNAPMODE,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);
		hip->get_76(&fi1);
		rb.resval.rint=fi1;
		SDS_setvar(NULL,DB_QGRIDMODE,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);
		hip->get_77(&fi1);
		rb.resval.rint=fi1;
		SDS_setvar(NULL,DB_QSNAPSTYL,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);
		hip->get_78(&fi1);
		rb.resval.rint=fi1;
		SDS_setvar(NULL,DB_QSNAPISOPAIR,&rb,dp,&SDS_CURCFG,&SDS_CURSES,0);
		}
	return(RTNORM);
	}

void SDS_OnFirstView(CIcadView *pView,int UseFirstView) {

	if(pView==NULL || pView->m_pViewsDoc==NULL) return;
	db_drawing *flp=pView->m_pViewsDoc->m_dbDrawing;
	if(flp==NULL) return;

	struct resbuf tilemode;
	SDS_getvar(NULL,DB_QTILEMODE,&tilemode,((CIcadDoc*)pView->GetDocument())->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);

	if(UseFirstView) {
		if(!tilemode.resval.rint) {
			if((int)flp->ret_ntabrecs(DB_VXTAB,0)>0) {
				struct resbuf cvport;
				SDS_getvar(NULL,DB_QCVPORT,&cvport,((CIcadDoc*)pView->GetDocument())->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
				if(cvport.resval.rint!=1) {
					pView->m_pViewsDoc->m_nCvportOnOpen=cvport.resval.rint;
					cvport.resval.rint=1;
					SDS_setvar(NULL,DB_QCVPORT,&cvport,((CIcadDoc*)pView->GetDocument())->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);
				}
				pView->m_pVportTabHip=flp->tblnext(DB_VXTAB,1);
				ASSERT(pView->m_pVportTabHip != NULL);
				pView->m_pVportTabHip=((db_vxtabrec *)pView->m_pVportTabHip)->ret_vpehip();
			} else goto makenew;
		} else {
			if((int)flp->ret_ntabrecs(DB_VPORTTAB,0)>0) {
			   pView->m_pVportTabHip=flp->tblsearch(DB_VPORTTAB,"*ACTIVE"/*DNT*/,1);
			} else goto makenew;
		}
	} else {
		if(SDS_CMainWindow->m_pViewTabEntryHip==NULL) {
			makenew: ;
			if(!tilemode.resval.rint)
				pView->m_pVportTabHip = cmd_createVIEWPORT(flp, NULL, NULL);
			else
				pView->m_pVportTabHip = cmd_createVPORT(flp);
		} else {
			pView->m_pVportTabHip=SDS_CMainWindow->m_pViewTabEntryHip;
			SDS_CMainWindow->m_pViewTabEntryHip=NULL;
		}
	}

	// Make sure all the setvars are current from the table. (only if in PS/MS)
	if(!tilemode.resval.rint) SDS_TabToVar(flp,pView->m_pVportTabHip);

	struct gr_viewvars viewvarsp;
	SDS_ViewToVars2Use(pView,flp,&viewvarsp);
	gr_initview(flp,&SDS_CURCFG,&pView->m_pCurDwgView,&viewvarsp);

	if(!cmd_InsideDxfInCommand) {
		extern char *SDS_UpdateSetvar;	/*D.Gavrilov*/// We'll use it in some of
		SDS_UpdateSetvar=NULL;			// the following calls of IcadWndAction
		// Update the status bar.
		SDS_CMainWindow->IcadWndAction(ICAD_WNDACTION_UDSTLAY);
		SDS_CMainWindow->IcadWndAction(ICAD_WNDACTION_UDSTLTP);
		SDS_CMainWindow->IcadWndAction(ICAD_WNDACTION_UDSTCOL);
		SDS_CMainWindow->IcadWndAction(ICAD_WNDACTION_UDSTSTY);		// EBATECH (Style)
		SDS_CMainWindow->IcadWndAction(ICAD_WNDACTION_UDSTDIM);		// EBATECH (DimStyle)
		SDS_CMainWindow->IcadWndAction(ICAD_WNDACTION_UDSTLW);		// EBATECH (LineWeight)
		SDS_CMainWindow->IcadWndAction(ICAD_WNDACTION_STATCHANGE);	// this call is also needed!
		// Also menuchecks and toolbars
		SDS_CMainWindow->IcadWndAction(ICAD_WNDACTION_UDCHKSTAT);
		SDS_CMainWindow->IcadWndAction(ICAD_WNDACTION_UDTBSTATE);
	}
}

int SDS_CmdLineSize(int mode) {

	switch(mode) {
		case SDS_TEXTPAGE:
			ExecuteUIAction( ICAD_WNDACTION_TEXTPAGE );
			break;
		case SDS_TEXTSCR:
			ExecuteUIAction( ICAD_WNDACTION_TEXTSCR );
			break;
		case SDS_GRAPHSCR:
			ExecuteUIAction( ICAD_WNDACTION_GRAPHSCR );
			break;
	}
	return(RTNORM);
}

int SDS_UpdateEntDisp(const sds_name ename, int pushext, gr_view *pViewData) 
{
	// pushext bit 0 (1) = push extmin/extmax
	// pushext bit 1 (2) = set elp->extents
	// pushext bit 2 (4) = reset extmin/extmax
	// pushext bit 3 (8) = Don't transform extmin/extmax into WCS
	if(ename == NULL || SDS_CURDOC == NULL)
		return RTERROR;

	struct gr_displayobject *pDOListBegin = NULL, *pDOListEnd = NULL;
	db_handitem *telp;
	db_handitem *pEntity = (db_handitem *)ename[0];
	db_drawing *pDrawing = (db_drawing*)ename[1];
	sds_point ext[2], extent[2];
	int pspace;

	BOOL bExtmaxUpdated = FALSE;
	BOOL bExtminUpdated = FALSE;

	extent[0][0] = extent[0][1] = extent[0][2]=0.0;
	extent[1][0] = extent[1][1] = extent[1][2]=0.0;

	if(pViewData == NULL && SDS_CURDOC->m_RegenListView)
		pViewData = SDS_CURDOC->m_RegenListView->m_pCurDwgView;
	if(pViewData == NULL)
		pViewData=SDS_CURGRVW;

	if(pEntity == NULL || pDrawing == NULL || pViewData == NULL)
		return RTERROR;

	if(pEntity->ret_type() == DB_SEQEND)
	{
		((db_seqend*)pEntity)->get_mainent(&pEntity);
		if(pEntity == NULL)
			return RTERROR;
	}
	if(pEntity->ret_disp() && pEntity->ret_type() != DB_ACAD_PROXY_ENTITY)
	{
		gr_freedisplayobjectll((struct gr_displayobject *)pEntity->ret_disp());
		pEntity->set_disp(NULL);
	}
	// Check for deleted objects.
	if(pEntity->ret_deleted())
		return RTNORM;
	// Don't update vertexes.
	if(pEntity->ret_type() == DB_VERTEX)
		return RTNORM;

	pEntity->get_67(&pspace);

	if(pEntity->ret_type() == DB_OLE2FRAME && pushext)
	{
		pEntity->get_extent(ext[0], ext[1]);
		goto jump;
	}

	telp=pEntity;
#ifdef _USE_DISP_OBJ_PTS_3D_
	if(SDS_getdispobjs(pDrawing, &telp, &pDOListBegin, &pDOListEnd, pushext ? ext : NULL, pDrawing, pViewData,0))
		return RTERROR;
#else
	if(SDS_getdispobjs(pDrawing, &telp, &pDOListBegin, &pDOListEnd, pushext ? ext : NULL, pDrawing, pViewData,1))
		return RTERROR;
#endif
	// Getting no disp objects is not a problem, some objects don't have display objects.
	if(pDOListBegin == NULL || pDOListEnd == NULL) 
	{
		if(pEntity->ret_type()==DB_VIEWPORT) 
		{
			// TODO Calc VP ext here!
			return RTNORM;
		} 
		else 
			return RTNORM;
	}
	pEntity->set_disp((void*)pDOListBegin);

	// Update the drawing extents.
	if(pushext && pDOListBegin)
	{
jump: ;
		struct resbuf extmin,extmax;
		int firstentext;
		if(pspace) 
		{
			SDS_getvar(NULL, DB_QP_EXTMIN, &extmin, pDrawing, &SDS_CURCFG, &SDS_CURSES);
			SDS_getvar(NULL, DB_QP_EXTMAX, &extmax, pDrawing, &SDS_CURCFG, &SDS_CURSES);
		}
		else
		{
			SDS_getvar(NULL, DB_QEXTMIN, &extmin, pDrawing, &SDS_CURCFG, &SDS_CURSES);
			SDS_getvar(NULL, DB_QEXTMAX, &extmax, pDrawing, &SDS_CURCFG, &SDS_CURSES);
		}
		firstentext = 1;
		if(pushext & 0x01) 
		{
			if(!(pushext & 0x08))
				SDS_cubeUCStoWCS(ext[0], ext[1], ext[0], ext[1]);
			if(pushext & 0x04 && firstentext) 
			{
				bExtmaxUpdated = TRUE;
				bExtminUpdated = TRUE;
				extmin.resval.rpoint[0] = ext[0][0];
				extmin.resval.rpoint[1] = ext[0][1];
				extmin.resval.rpoint[2] = ext[0][2];
				extmax.resval.rpoint[0] = ext[1][0];
				extmax.resval.rpoint[1] = ext[1][1];
				extmax.resval.rpoint[2] = ext[1][2];
			} 
			else 
			{
				if(extmin.resval.rpoint[0] > ext[0][0])
				{
					bExtminUpdated = TRUE;
					extmin.resval.rpoint[0] = ext[0][0];
				}
				if(extmin.resval.rpoint[1] > ext[0][1])
				{
					bExtminUpdated = TRUE;
					extmin.resval.rpoint[1] = ext[0][1];
				}
				if(extmin.resval.rpoint[2] > ext[0][2])
				{
					bExtminUpdated = TRUE;
					extmin.resval.rpoint[2] = ext[0][2];
				}
				if(extmax.resval.rpoint[0] < ext[1][0])
				{
					bExtmaxUpdated = TRUE;
					extmax.resval.rpoint[0] = ext[1][0];
				}
				if(extmax.resval.rpoint[1] < ext[1][1])
				{
					bExtmaxUpdated = TRUE;
					extmax.resval.rpoint[1] = ext[1][1];
				}
				if(extmax.resval.rpoint[2] < ext[1][2])
				{
					bExtmaxUpdated = TRUE;
					extmax.resval.rpoint[2] = ext[1][2];
				}
			}
		}
		if(pEntity->ret_type() == DB_OLE2FRAME)
			goto out;
		if(pushext & 0x02) 
		{
#ifdef _USE_DISP_OBJ_PTS_3D_
			CD3 minPoint, maxPoint;
			CDoBase::getBoundingBox(pDOListBegin, minPoint, maxPoint);
			memcpy(extent[0], (sds_real*)minPoint, 3 * sizeof(sds_real));
			memcpy(extent[1], (sds_real*)maxPoint, 3 * sizeof(sds_real));
#else
			int fi1, fi2, ndims, last;
			for(pDOListBegin = pDOListBegin; pDOListBegin != NULL; pDOListBegin = pDOListBegin->next) 
			{
				if(pDOListBegin->type & '\x01' || pDOListBegin->npts < 1 || pDOListBegin->chain == NULL)
					continue;
				ndims = ((pDOListBegin->type & 0x01) != 0) + 2;
				last = pDOListBegin->npts * ndims;
				for (fi1 = 0, fi2 = last; fi1 < fi2; fi1 += ndims) 
				{
					if(firstentext) 
					{
						extent[0][0] = extent[1][0] = pDOListBegin->chain[fi1];
						extent[0][1] = extent[1][1] = pDOListBegin->chain[fi1+1];
					} 
					else
					{
						if(extent[0][0] > pDOListBegin->chain[fi1])
							extent[0][0] = pDOListBegin->chain[fi1];
						else 
							if(extent[1][0] < pDOListBegin->chain[fi1])
								 extent[1][0] = pDOListBegin->chain[fi1];
						if(extent[0][1] > pDOListBegin->chain[fi1+1])
							extent[0][1] = pDOListBegin->chain[fi1+1];
						else 
							if (extent[1][1] < pDOListBegin->chain[fi1+1])
								 extent[1][1]=pDOListBegin->chain[fi1+1];
					}
					firstentext = 0;
				}
			}
#endif
			pEntity->set_extent(extent[0], extent[1]);
		}
		// Don't push the extents for ray's and xline's.
		if(pEntity->ret_type() == DB_RAY || pEntity->ret_type() == DB_XLINE)
			return RTNORM;

out: ;
		if(pspace) 
		{
			if(bExtminUpdated)
				SDS_setvar(NULL, DB_QP_EXTMIN, &extmin, pDrawing, &SDS_CURCFG, &SDS_CURSES, 0);
			if(bExtmaxUpdated)
				SDS_setvar(NULL, DB_QP_EXTMAX, &extmax, pDrawing, &SDS_CURCFG, &SDS_CURSES, 0);
		}
		else
		{
			if(bExtminUpdated)
				SDS_setvar(NULL, DB_QEXTMIN, &extmin, pDrawing, &SDS_CURCFG, &SDS_CURSES, 0);
			if(bExtmaxUpdated)
				SDS_setvar(NULL, DB_QEXTMAX, &extmax, pDrawing, &SDS_CURCFG, &SDS_CURSES, 0);
		}
		if(bExtminUpdated)
			SDS_setvar(NULL, DB_QVSMIN, &extmin, pDrawing, &SDS_CURCFG, &SDS_CURSES, 0);
		if (bExtmaxUpdated)
			SDS_setvar(NULL, DB_QVSMAX, &extmax, pDrawing, &SDS_CURCFG, &SDS_CURSES, 0);
	}
	return(RTNORM);
}



CBitmap *SDS_DrawDiskFont(int cx,int cy,char *fname,char *bfname) {
	sds_point pt0,pt1,pt2,pt3;
	struct sds_resbuf *elist,rb;
	db_drawing *flp=new db_drawing(DB_ENGLISH_MEASUREINIT, DB_BYCOL_PSTYLEMODE);

	rb.restype=RTREAL;
	rb.resval.rreal=10.0;
	SDS_setvar(NULL,DB_QVIEWSIZE,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0);
// ---------------------

	if ((elist=sds_buildlist(RTDXF0,"STYLE"/*DNT*/,2,"STANDARD"/*DNT*/,70,0,40,0.0,41,1.0,50,0.0,71,0,3,fname,4,bfname,0))==NULL) return(NULL);
   db_handitem *elp=flp->findtabrec(DB_STYLETAB,"STANDARD"/*DNT*/,0);
	elp=new db_text(flp);
	elp->entmod(elist,flp);
	flp->addhanditem(elp);

	elist=sds_buildlist(RTDXF0,"TEXT"/*DNT*/,2,"Sample Text"/*DNT*/,10,pt0,11,pt1,70,0,1,""/*DNT*/,13,pt2,14,pt3,50,0.0,0);
	elp=new db_text(flp);
	elp->entmod(elist,flp);
	sds_relrb(elist);

	CBitmap *bm=SDS_DrawBitmap(flp,NULL,NULL,NULL,cx,cy,TRUE);

	delete flp;
	return(bm);
}

CBitmap *SDS_DrawDiskHatch(int cx,int cy,char *HatchName) {
	struct sds_resbuf rb;
	db_drawing *flp=new db_drawing(DB_ENGLISH_MEASUREINIT, DB_BYCOL_PSTYLEMODE);
	rb.restype=RTREAL;
	rb.resval.rreal=10.0;
	SDS_setvar(NULL,DB_QVIEWSIZE,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0);
// ---------------------

	// TODO Make hatch object here.

	CBitmap *bm=SDS_DrawBitmap(flp,NULL,NULL,NULL,cx,cy,TRUE);

	delete flp;
	return(bm);
}




void SDS_SetRegenListFlag(CIcadView *pView,CIcadDoc *pDoc) {

	if(pView==NULL || pDoc==NULL) return;

	struct resbuf rb;

	SDS_getvar(NULL,DB_QTILEMODE,&rb,pDoc->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint==0 || pDoc->m_RegenListView==NULL) {
		// TODO Speed fix here !!!!
		pView->m_fUseRegenList=0;
	} else if(pDoc->m_RegenListView==pView) {
		pView->m_fUseRegenList=1;
	} else {
		struct gr_view *rview=pDoc->m_RegenListView->m_pCurDwgView;
		// EBATECH(shiba)-[ 2001-06-13
		//	FIX:gap of a mouse position and a drawing position on another VIEW
		//		by the execution of the plane rotation operation (Ctrl+MRButton).
//		if(pView->m_pCurDwgView->camera[0]==rview->camera[0] &&
		sds_point ctr1, ctr2;
		pView->m_pCurDwgView->GetCenterProjection(ctr1);
		rview->GetCenterProjection(ctr2);
		if(ctr1 == ctr2 &&
		   icadPointEqual(pView->m_pCurDwgView->camera,rview->camera) &&
			  icadPointEqual(pView->m_pCurDwgView->target,rview->target) &&
 				 icadPointEqual(pView->m_pCurDwgView->ucsaxis[0],rview->ucsaxis[0]) &&
					icadPointEqual(pView->m_pCurDwgView->ucsaxis[1],rview->ucsaxis[1]) &&
					   pView->m_pCurDwgView->mode==rview->mode) {
			pView->m_fUseRegenList=1;
		} else {
			pView->m_fUseRegenList=0;
		}
	}
}

int SDS_getdispobjs(db_drawing *topdp,		// the top level dp we are getting for
					db_handitem **p_elpp,
					struct gr_displayobject **p_begdopp,
					struct gr_displayobject **p_enddopp,
					sds_point *p_ucsextp,
					db_drawing *p_flp,
					struct gr_view *p_viewp,
					short p_mode,
					db_viewport *pViewportForVpLayerData /*= NULL*/,
					bool bMeshAcisEntities /*= false*/)				/*D.G.*/// 'true' to get mesh surface disp. objs for ACIS entities.
{
	int entType = (*p_elpp)->ret_type();
	CDC *dc = NULL;
	CIcadView *icadView = SDS_CMainWindow->GetIcadView();
	if (icadView)
	{
		ASSERT_KINDOF(CIcadView, icadView);
		dc = icadView->GetTargetDeviceCDC();
	}

	/*D.G.*/// Partial fix for the following problem: dimensions created via ODT are not displayed,
	// they cannot be processed in most cases at all. The reason - ODT doesn't create anonymous blocks
	// for them.
	// My work-around: call cmd_makedimension here. It doesn't work for dimensions in blocks.
	// Really we should make anonymous blocks (calling cmd_makedimension) in gr_getdispobjs
	// or even in DB on creating (loading) dimensions. - TO DO.

	db_handitem* pSavEntity = *p_elpp;
	int	rc = gr_getdispobjs(topdp,
								NULL,
								p_elpp,
								p_begdopp,
								p_enddopp,
								p_ucsextp,
								p_flp,
								p_viewp,
								p_mode,
								dc,
								pViewportForVpLayerData,
								bMeshAcisEntities);		/*D.G.*/
	if(rc)
		return rc;

	if(!*p_begdopp && pSavEntity->ret_type() == DB_DIMENSION)
	{
		/*DG - 13.9.2002*/// In some cases the previous gr_getdispobjs call doesn't generate dispobjs,
		// but it's ok (e.g., when the dimension is on a frozen layer). We should check the presence
		// of according (anonymous) block itself to satisfy the fix described above..
		db_handitem*	pBthip = NULL;
		pSavEntity->get_2(&pBthip);
		if(!pBthip)
		{
			*p_elpp = pSavEntity;

			// SystemMetrix(Hiro)-[FIX: When the explorer hides the DEFPOINTS layer, the shape of dimension is changed.
			//if(cmd_makedimension(*p_elpp, p_flp, 0) != RTNORM)
			///    return -3;
			int iRestore = SDS_AtNodeDrag;
			SDS_AtNodeDrag = 1;
			int irt = cmd_makedimension(*p_elpp, p_flp, 0);
			SDS_AtNodeDrag = iRestore;
			if (irt != RTNORM)
			  {
				return -3;
			  }
			// ]-SystemMetrix(Hiro) FIX: When the explorer hides the DEFPOINTS layer, the shape of dimension is changed.

			return gr_getdispobjs(topdp,
									NULL,
									p_elpp,
									p_begdopp,
									p_enddopp,
									p_ucsextp,
									p_flp,
									p_viewp,
									p_mode,
									dc,
									pViewportForVpLayerData,
									bMeshAcisEntities);		/*D.G.*/
		}
		else
		{
			return rc;
		}
	}
	else
		return rc;
}

int SDS_InitThisView(CIcadView *pView) {
	if(pView==NULL) return(RTERROR);
	struct resbuf tilemode;
	struct resbuf cvport;
	SDS_getvar(NULL,DB_QCVPORT,&cvport,pView->m_pViewsDoc->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
	SDS_getvar(NULL,DB_QTILEMODE,&tilemode,pView->m_pViewsDoc->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
	if(tilemode.resval.rint==0 && cvport.resval.rint>1 && pView->m_pVportTabHip->ret_type()==DB_VIEWPORT) {
		SDS_VPeedToView(pView->m_pViewsDoc->m_dbDrawing,pView->m_pVportTabHip,&pView->m_pCurDwgView,pView);
		SDS_SetClipInDC(pView,pView->m_pVportTabHip,FALSE);
	} else {
		struct gr_viewvars viewvarsp;
		SDS_ViewToVars2Use(pView,pView->m_pViewsDoc->m_dbDrawing,&viewvarsp);
		gr_initview(pView->m_pViewsDoc->m_dbDrawing,&SDS_CURCFG,&pView->m_pCurDwgView,&viewvarsp);
	}
	return(RTNORM);
}

int SDS_VPeedToView(db_drawing *flp,db_handitem *elp,struct gr_view **gViewVP,CIcadView *pView) {

	struct gr_viewvars vars2use;
	struct resbuf setgetrb;
	sds_point center;
	double height,width;
	db_viewport *vp=(db_viewport *)elp;
	vp->get_10(center);
	vp->get_40(&width);
	vp->get_41(&height);

	vp->get_17(vars2use.target);
	vp->get_16(vars2use.viewdir);
	vp->get_51(&vars2use.viewtwist);
	vp->get_45(&vars2use.viewsize);
	vp->get_12(vars2use.viewctr);
	int status;
	vp->get_90(&status);
	vars2use.viewmode = status & 31;
	vp->get_72(&vars2use.curvdispqual);
	vp->get_42(&vars2use.lenslength);

	//populate rest of struct w/stuff not found in the entity
	SDS_getvar(NULL,DB_QUCSORG,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(vars2use.ucsorg,setgetrb.resval.rpoint);
	SDS_getvar(NULL,DB_QUCSXDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(vars2use.ucsxdir,setgetrb.resval.rpoint);
	SDS_getvar(NULL,DB_QUCSYDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(vars2use.ucsydir,setgetrb.resval.rpoint);
	SDS_getvar(NULL,DB_QLTSCALE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	vars2use.ltscale=setgetrb.resval.rreal;
	SDS_getvar(NULL,DB_QATTMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	vars2use.attmode=setgetrb.resval.rint;
	SDS_getvar(NULL,DB_QSPLFRAME,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	vars2use.splframe=setgetrb.resval.rint;
	SDS_getvar(NULL,DB_QPDMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	vars2use.pdmode=setgetrb.resval.rint;
	SDS_getvar(NULL,DB_QPDSIZE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	vars2use.pdsize=setgetrb.resval.rreal;
	SDS_getvar(NULL,DB_QFILLMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	vars2use.fillmode=setgetrb.resval.rint;
	SDS_getvar(NULL,DB_QELEVATION,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	vars2use.elevation=setgetrb.resval.rreal;
	//EED only contains 2D viewctr, so add Z value
	vars2use.viewctr[2]=setgetrb.resval.rreal;
	SDS_getvar(NULL,DB_QQTEXTMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	vars2use.qtextmode=setgetrb.resval.rint;
	SDS_getvar(NULL,DB_QLTSCLWBLK,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	vars2use.ltsclwblk=setgetrb.resval.rint;
	SDS_getvar(NULL,DB_QLWDISPLAY,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	vars2use.lwdisplay=setgetrb.resval.rint;
	SDS_getvar(NULL,DB_QLWDEFAULT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	vars2use.lwdefault=setgetrb.resval.rint;

	// This is the X and Y in pixels
	vars2use.screensize[0]=pView->m_iWinX;
	vars2use.screensize[1]=pView->m_iWinY;

	// Get center point of the main view ps viewport entity.
	double mainsize;
	sds_point maincen;
	db_viewport *psvp;
	psvp=((db_vxtabrec *)(flp->tblnext(DB_VXTAB,1)))->ret_vpehip();
	psvp->get_45(&mainsize);
	psvp->get_12(maincen);

	sds_real msu_psu,ar1;

	if (vars2use.viewmode & IC_VPENT_STATUS_PERSPECTIVEMODE) {	/* Perspective */

		/* Create an "as is" gr_view based on the data we've collected: */
		gr_initview(flp,&SDS_CURCFG,gViewVP,&vars2use);

		/* Now doctor centproj, pixht, and pixwd in the view struct */
		/* so that the full screen image will draw correctly to place the VPE */
		/* image correctly in it's "box". */

		/* The factor of 0.5 below is totally empirical, and I don't think */
		/* it's correct.  It makes the TOWER drawing come out close: */
		msu_psu=fabs(0.5*35.0*(*gViewVP)->viewdist/vars2use.lenslength/width);


		/* Adjust pixel sizes. */
		/* Here's that empirical 0.5 factor again for the TOWER drawing.) */
		ar1=mainsize*vars2use.screensize[0]/vars2use.screensize[1]/width*0.5;

		(*gViewVP)->SetPixelHeight( (*gViewVP)->GetPixelHeight() * ar1 );
		(*gViewVP)->SetPixelWidth( (*gViewVP)->GetPixelWidth() * ar1 );

		/* Determine the new CenterProjection to make the VPE MS entities */
		/* draw in the proper place.  (Note viewctr is not used in perspective.) */
		sds_point pointTemp;
		(*gViewVP)->GetCenterProjection( pointTemp );
		pointTemp[0] += msu_psu*(maincen[0]-center[0]);
		pointTemp[1] += msu_psu*(maincen[1]-center[1]);
		(*gViewVP)->SetCenterProjection( pointTemp );


	} else {  /* Orthogonal */
		msu_psu=vars2use.viewsize/height;  /* MS units per PS unit. */

		/* Determine the new viewctr: */
		vars2use.viewctr[0]+=msu_psu*(maincen[0]-center[0]);
		vars2use.viewctr[1]+=msu_psu*(maincen[1]-center[1]);

		/*	Determine new viewsize (after using old one above): */
		vars2use.viewsize*=mainsize/height;

		/* If PSLTSCALE is on, adjust LTSCALE so that it looks like MSPACE */
		/* linework was drawn in PSPACE: */
		SDS_getvar(NULL,DB_QPSLTSCALE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (setgetrb.resval.rint) vars2use.ltscale*=msu_psu;

		/* Update the view: */
		gr_initview(flp,&SDS_CURCFG,gViewVP,&vars2use);
	}

	return(RTNORM);
}





int SDS_EventMask(UINT uint) {
	SDS_CurEventMask=uint;
	return(RTNORM);
}

int SDS_PrintOnStatus(int box, LPCTSTR text, int hl) {

	switch(box) {
		case -1:
			SendMessage(SDS_CMainWindow->m_StatusBar,SB_SETTEXT,(WPARAM)0,(LPARAM)text);
			break;
		case -2:
			strncpy(SDS_CMainWindow->m_strCoords,text,sizeof(SDS_CMainWindow->m_strCoords)-1);
			ExecuteUIAction( ICAD_WNDACTION_UDCOORDS );
			break;
	}

	return(RTNORM);
}



int SDS_BuildRegenList(db_drawing *flp,struct gr_view *view,CIcadView *pView,CIcadDoc *pDoc) {

	int FstPSPt=1,FstMSPt=1,uselim=1,type,tilemode,cvport;
	int GaveAcisMsg=0,GaveHatchMsg=0,GaveProxyMsg=0,GaveA2KEntMsg=0;
	sds_name ename;
	struct resbuf rb;
	db_handitem *telp;

	if (flp==NULL || view==NULL || pDoc==NULL || pView==NULL) return(RTERROR);

	SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	tilemode=rb.resval.rint;
	SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	cvport=rb.resval.rint;
	/*just ignore fastzoom*/
	//SDS_getvar(NULL,DB_QFASTZOOM,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	//if(rb.resval.rint==1/*0*/) return(RTNORM);	/*DG - 7.2.2002*/// misprint?

	if(0==flp->ret_nmainents()) return(RTNORM);

	// SystemMetrix(shiba)-[06.10.2001
	//	 FIX: During command(zoom, regen etc) execution, moving mouse (wheel), crash.
	//
	//	Attention!! Don't return without SDS_BuildRegenList_Doing:false
	//
	SDS_BuildRegenList_Doing	= true;
	// ]-SystemMetrix(shiba) 06.10.2001

	db_setfont(NULL,NULL,NULL,NULL);

	pView->m_dViewSizeAtLastRegen=view->viewsize;

	SDS_CallUserCallbackFunc(SDS_CBVIEWCHANGE,(void *)5,NULL,NULL);

	/*DG - 24.4.2003*/// SDS apps can change pDoc->m_RegenListView during callback processing
	// (e.g. by calling sds_setview which calls SDS_ZoomIt), so set it AFTER callback sending.
	pDoc->m_RegenListView=pView;

	// Build a new list from the whole drawing.
	ename[1]=(long)flp;
	int pspace,doprog=0;
	long nentsindwg=flp->ret_nmainents();
	long entcount=0L,savprog=0L,prog;

	if(flp->ret_nmainents()>3000) {
		doprog=1;
		SDS_ProgressStart();
	   sds_grtext(-2,ResourceString(IDC_ICADAPI_REGENERATING_D_106, "Regenerating drawing..." ),0);
	}
	for(telp=flp->entnext_noxref(NULL); telp!=NULL; telp=flp->entnext_noxref(telp)) {
		type=telp->ret_type();
		telp->get_67(&pspace);

		if(type==DB_VERTEX || type==DB_SEQEND || telp->ret_deleted()) continue;
		ename[0]=(long)telp;

		if(doprog) {
			entcount++;
			prog=(int)(((double)entcount/nentsindwg)*100);
			if(savprog+10<=prog) SDS_ProgressPercent(savprog=prog);
		}

		if(pDoc && pDoc->m_bDidFirstRegen==FALSE) {
#ifdef BUILD_WITH_ACIS_SUPPORT
			if( (type == DB_3DSOLID || type == DB_REGION || type == DB_BODY) &&
				!GaveAcisMsg && !CModeler::get()->isStarted() )	/*D.G.*/
			{
				cmd_ErrorPrompt(CMD_ERR_DWGHASACIS,0);
				GaveAcisMsg=1;
				// SystemMetrix(shiba)-[06.10.2001
				//	 FIX: During command(zoom, regen etc) execution, moving mouse (wheel), crash.
//				if(!IsWindow(pView->m_hWnd)) return(RTERROR);
				if(!IsWindow(pView->m_hWnd)){
					SDS_BuildRegenList_Doing	= false;
					return(RTERROR);
				}
				// ]-SystemMetrix(shiba) 06.10.2001

			}
#endif
//			if(type==DB_HATCH && GaveHatchMsg==0) {
//				cmd_ErrorPrompt(CMD_ERR_DWGHASHATCH,0);
//				GaveHatchMsg=1;
//				if(!IsWindow(pView->m_hWnd)) return(RTERROR);
//			}
			if(type==DB_ACAD_PROXY_ENTITY && GaveProxyMsg==0) {
				cmd_ErrorPrompt(CMD_ERR_DWGHASPROXY,0);
				GaveProxyMsg=1;
				// SystemMetrix(shiba)-[06.10.2001
				//	 FIX: During command(zoom, regen etc) execution, moving mouse (wheel), crash.
//				if(!IsWindow(pView->m_hWnd)) return(RTERROR);
				if(!IsWindow(pView->m_hWnd)){
					SDS_BuildRegenList_Doing	= false;
					return(RTERROR);
				}
				// ]-SystemMetrix(shiba) 06.10.2001
			}
			if((type==DB_RTEXT || type==DB_ARCALIGNEDTEXT || type==DB_WIPEOUT) && GaveA2KEntMsg==0) {
				cmd_ErrorPrompt(CMD_ERR_UNSUPPORTEDA2KENT,0);
				GaveA2KEntMsg=1;
				// SystemMetrix(shiba)-[06.10.2001
				//	 FIX: During command(zoom, regen etc) execution, moving mouse (wheel), crash.
//				if(!IsWindow(pView->m_hWnd)) return(RTERROR);
				if(!IsWindow(pView->m_hWnd)){
					SDS_BuildRegenList_Doing	= false;
					return(RTERROR);
				}
				// ]-SystemMetrix(shiba) 06.10.2001
			}
		}

		if(pspace) {
			SDS_UpdateEntDisp(ename,FstPSPt ? 15 : 11,view);
		} else {
			/*DG*///regen the entity in this case too. otherwise polygonal arcs may show in print/preview viewports 
			
			// **************************************************************************
			//*!!! Added by Mohan : To modify the following line as suggested by Denis.
			// This solves the "Zoom Extents Bug" reported by Dave.
			// It also solves the Lineweight related bug that Dave had reported in which 
			// the text used to disappear in the file "Lineweights.dwg" file.
			// Copy the modified and uncommented line from the BricsCAD code.
			// And Remove your earlier changes...The bug should be fixed.

			//*** This line was originally commented...
			//if(tilemode==0 && cvport==1) continue;
			
			//*** These two lines are from the BricsCAD code. We only do this check when we are in model space.  
			if(!tilemode && cvport == 1 && !pView->IsPrinting() && !SDS_CMainWindow->m_bPrintPreview && !(cmd_InsideRegen && cmd_InsideRedrawAll))
				continue;
			// **************************************************************************

			SDS_UpdateEntDisp(ename,FstMSPt ? 15 : 11,view);
		}

		if(pDoc && pDoc->m_bDidFirstRegen==FALSE) {
			if(type==DB_INSERT) {
				db_handitem *bthip;
				char *btxrefpn;
				int btflags;
				telp->get_2(&bthip);
				bthip->get_70(&btflags);
				bthip->get_3(&btxrefpn);
				if((btflags & IC_BLOCK_XREF) && btxrefpn && *btxrefpn && bthip->ret_xrefdp()==NULL) {
					sds_printf(ResourceString(IDC_ICADAPI__N___UNABLE_TO_107, "\n-- Unable to locate the Externally Referenced file %s --\n" ),btxrefpn);
				}
			}
		}

		if(!(type==DB_XLINE || type==DB_RAY) &&
			(telp->ret_disp() || (!telp->ret_disp() && type==DB_OLE2FRAME))) {
			if(pspace) FstPSPt=0; else FstMSPt=0;
			uselim=0;
		}
	}

	if(doprog) {
	   sds_grtext(-2,""/*DNT*/,0);
		SDS_ProgressStop();
	}

	if(pDoc) pDoc->m_bDidFirstRegen=TRUE;
	struct resbuf extmin,extmax;
	if(uselim) {
		SDS_getvar(NULL,DB_QLIMMIN,&extmin,flp,&SDS_CURCFG,&SDS_CURSES);
		SDS_getvar(NULL,DB_QLIMMAX,&extmax,flp,&SDS_CURCFG,&SDS_CURSES);
	} else {
		SDS_getvar(NULL,DB_QEXTMIN,&extmin,flp,&SDS_CURCFG,&SDS_CURSES);
		SDS_getvar(NULL,DB_QEXTMAX,&extmax,flp,&SDS_CURCFG,&SDS_CURSES);
	}
	SDS_cubeUCStoWCS(extmin.resval.rpoint,extmax.resval.rpoint,extmin.resval.rpoint,extmax.resval.rpoint);
	extmin.restype=RT3DPOINT;
	extmax.restype=RT3DPOINT;
	SDS_setvar(NULL,DB_QEXTMIN,&extmin,flp,&SDS_CURCFG,&SDS_CURSES,0);
	SDS_setvar(NULL,DB_QEXTMAX,&extmax,flp,&SDS_CURCFG,&SDS_CURSES,0);

	POSITION pos=pDoc->GetFirstViewPosition();
	CIcadView *pView2;
	struct gr_view *rview=pDoc->m_RegenListView ? pDoc->m_RegenListView->m_pCurDwgView : SDS_CURGRVW;
	while (pos!=NULL) {
		pView2=(CIcadView *)pDoc->GetNextView(pos);
#ifdef _USE_DISP_OBJ_PTS_3D_
		if(!pView2->IsKindOf(RUNTIME_CLASS(CIcadView)))
			continue;
#else
		ASSERT_KINDOF(CIcadView, pView2);
#endif

		// EBATECH(shiba)-[ 2001-06-13
		//	FIX:gap of a mouse position and a drawing position on another VIEW
		//		by the execution of the plane rotation operation (Ctrl+MRButton).
		sds_point ctr1, ctr2;
		pView->m_pCurDwgView->GetCenterProjection(ctr1);
		rview->GetCenterProjection(ctr2);
		if(!SDS_CMainWindow->m_bPrintPreview && (pView2==pView ||
			ctr1 == ctr2 &&
		// ]-EBATECH(shiba) 2001-06-13
		   icadPointEqual(pView2->m_pCurDwgView->camera,rview->camera) &&
			  icadPointEqual(pView2->m_pCurDwgView->target,rview->target) &&
 				 icadPointEqual(pView2->m_pCurDwgView->ucsaxis[0],rview->ucsaxis[0]) &&
					icadPointEqual(pView2->m_pCurDwgView->ucsaxis[1],rview->ucsaxis[1]) &&
					   pView2->m_pCurDwgView->mode==rview->mode)) {
			   pView2->m_fUseRegenList=1;
		} else pView2->m_fUseRegenList=0;
	}

	// SystemMetrix(shiba)-[06.10.2001
	//	 FIX: During command(zoom, regen etc) execution, moving mouse (wheel), crash.
	SDS_BuildRegenList_Doing	= false;
	// ]-SystemMetrix(shiba) 06.10.2001
	return(RTNORM);
}



static PickType get_PickType(const char * str)
	{
	PickType pictyp;
	if (str != NULL)
		{
		if (strisame(str, "WINDOW") || strisame(str, "W") || strisame(str, "_W"))
			pictyp = WINDOW_t;
		else if (strisame(str, "CROSSING") || strisame(str, "C") || strisame(str, "_C"))
			pictyp = CROSSING_t;
		else if (strisame(str, "OUTSIDE") || strisame(str, "O") || strisame(str, "_O"))
			pictyp = OUTSIDE_t;
		else if (strisame(str, "WCIRCLE") || strisame(str, "WC") || strisame(str, "_WC"))
			pictyp = WCIRCLE_t;
		else if (strisame(str, "OCIRCLE") || strisame(str, "OC") || strisame(str, "_OC"))
			pictyp = OCIRCLE_t;
		else if (strisame(str, "CCIRCLE") || strisame(str, "CC") || strisame(str, "_CC"))
			pictyp = CCIRCLE_t;
		else if (strisame(str, "POINT") || strisame(str, "PO") || strisame(str, "_PO"))
			pictyp = POINT_t;
		else if (strisame(str, "FENCE") || strisame(str, "F") || strisame(str, "_F"))
			pictyp = FENCE_t;
		else if (strisame(str, "WPOLYGON") || strisame(str, "WP") || strisame(str, "_WP"))
			pictyp = WPOLYGON_t;
		else if (strisame(str, "OPOLYGON") || strisame(str, "OP") || strisame(str, "_OP"))
			pictyp = OPOLYGON_t;
		else if (strisame(str, "CPOLYGON") || strisame(str, "CP") || strisame(str, "_CP"))
			pictyp = CPOLYGON_t;
		else if (strisame(str, "X") || strisame(str, "_X"))
			pictyp = X_t;
		else
			pictyp = OTHER_t;
		}
	else
		{
		pictyp = OTHER_t;
		}
	return pictyp;
	}

// Helper functions for SDS_PickObjects
bool SelectUsingWindow(PickType pictyp, // I: type of pick
					   CRectangle & Extents, //I:  extents of the object under consideration
					   CRectangle & Window) //I: user-supplied rectangular window
	{
	bool selected;

	switch (pictyp)
		{	// begin switch (pictyp)
		case WINDOW_t:
			selected = Window.contains(Extents);
			break;
		case CROSSING_t:
			selected = Window.intersects(Extents);
			break;
		case OUTSIDE_t:
			selected = Window.disjoint(Extents);
			break;
		default:
			selected = false;
		}	//	 end switch (pictyp)

	return selected;
	}


bool SelectUsingCircle(PickType pictyp, // I: type of pick
					   CRectangle & Extents, //I:  extents of the object under consideration
					   CCircle & Circle) //I: user-supplied circle
	{
	bool selected;

	switch (pictyp)
		{	// begin switch (pictyp)
		case WCIRCLE_t:
			selected = Circle.contains(Extents);
			break;
		case CCIRCLE_t:
			selected = Circle.intersects(Extents);
			break;
		case OCIRCLE_t:
			selected = Circle.disjoint(Extents);
			break;
		default:
			selected = false;
		}	//	 end switch (pictyp)

	return selected;
	}

bool SelectUsingWindow(PickType pictyp, // I: type of pick
					   CParallelogram & Extents, //I:  extents of the object under consideration
					   CRectangle & Window) //I: user-supplied rectangular window
	{
	bool selected;

	switch (pictyp)
		{	// begin switch (pictyp)
		case WINDOW_t:
			selected = Window.contains(Extents);
			break;
		case CROSSING_t:
			selected = Window.intersects(Extents);
			break;
		case OUTSIDE_t:
			selected = Window.disjoint(Extents);
			break;
		default:
			selected = false;
		}	//	 end switch (pictyp)

	return selected;
	}


bool SelectUsingCircle(PickType pictyp, // I: type of pick
					   CParallelogram & Extents, //I:  extents of the object under consideration
					   CCircle & Circle) //I: user-supplied circle
	{
	bool selected;

	switch (pictyp)
		{	// begin switch (pictyp)
		case WCIRCLE_t:
			selected = Circle.contains(Extents);
			break;
		case CCIRCLE_t:
			selected = Circle.intersects(Extents);
			break;
		case OCIRCLE_t:
			selected = Circle.disjoint(Extents);
			break;
		default:
			selected = false;
		}	//	 end switch (pictyp)

	return selected;
	}




bool SelectUsingFence(PickType pictyp, // I: type of pick
					  CParallelogram & Extents, // I: extents of the object under consideration
					  CFence & Fence) // I: user-supplied fence
	{
	bool selected;

	switch (pictyp)
		{	// begin switch(pictyp)
		case FENCE_t:
			selected = Fence.crosses(Extents);
			break;
		default:
			selected = false;
		}	//	 end switch(pictyp)

	return selected;
	}


bool SelectUsingPolygon(PickType pictyp, // I: type of pick
					   CParallelogram & Extents, //I:  extents of the object under consideration
					   CPolygon & Polygon) //I: user-supplied polygon
	{
	bool selected;

	switch (pictyp)
		{	// begin switch (pictyp)
		case WPOLYGON_t:
			selected = Polygon.contains(Extents);
			break;
		case CPOLYGON_t:
			selected = Polygon.intersects(Extents);
			break;
		case OPOLYGON_t:
			selected = Polygon.disjoint(Extents);
			break;
		default:
			selected = false;
		}	//	 end switch (pictyp)

	return selected;
	}

/*-------------------------------------------------------------------------*//**
PUBLIC FUNCTION, this is used by sds_ssget() among others to select objects.

@author ?
@param	str						 =>
@param	pt1						 =>
@param	pt2						 =>
@param	filter					 =>
@param	ss						<=
@param	mode					 =>
@param	flp						 =>
@param	bIncludeInvisible		 =>
@param	bSpecialImageFilter		 => HACK, parameter is special to get sds_update() to work
									for images. Over time this can be eliminated when
									sds_update() hooks into a better version of the graphics
									pipeline
@param	bIncludeLockedLayers	 =>
@param	bAddToPrevSelSet		 =>
@return ?
*//*--------------------------------------------------------------------------*/
int					SDS_PickObjects
(
const char			*str,
const void			*pt1,
const sds_point		pt2,
const struct resbuf	*filter,
sds_name			ss,
int					mode,
db_drawing			*flp,
bool				bIncludeInvisible,
bool				bSpecialImageFilter,
bool				bIncludeLockedLayers,
bool				bAddToPrevSelSet
)
{
	if(flp==NULL) return(RTNORM);

	sds_ssadd(NULL,NULL,ss);

	int filterprev=0,freedisp=FALSE;
	sds_real fr1;
	long fl1,sslen;
	sds_point p1,p3,npt1,npt2;
	sds_name ename;
	sds_point savext[2],extent[2];
	struct sds_resbuf rb,*trb;
	struct gr_displayobject *tdo1,*beg,*end;
	db_handitem *elp,*telp,*layhip;

	gr_displayobject *saveDop = NULL;
	bool bSaveDop = false;

	if(flp==NULL) return(RTERROR);

	struct gr_view *view=SDS_CURGRVW;
	if(view==NULL) return(RTERROR);

	int SelPsEntsOnly=0;
	SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int tilemode=rb.resval.rint;
	if(SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	int cvport=rb.resval.rint;
	if(!tilemode && rb.resval.rint==1) SelPsEntsOnly=1;

   	if(str) {

		// ----------------------------------------
		// Previous selection set (may include filter)
		//
		if(strisame(str, "PREVIOUS"/*DNT*/) || strisame(str, "_PREVIOUS"/*DNT*/) ||
		   strisame(str, "P"/*DNT*/) || strisame(str, "_P"/*DNT*/) )
			{
			// We'll return RTERROR unless we actually find a match
			//
			int ret = RTERROR;

			if(sds_sslength(SDS_CURDOC->m_pPrevSelection,&fl1)==RTERROR || fl1<=0L)
				{
				return(RTERROR);
				}

			sds_ssfree(ss);
			sslen=0L;
			sds_sslength(ss,&sslen);
			fl1--;
			while(RTNORM==sds_ssname(SDS_CURDOC->m_pPrevSelection,fl1,ename))
				{
				elp=(db_handitem *)ename[0];
				if(elp)
					{
					if(filter==NULL ||
						EntityMatchesFilterList(filter,(db_handitem *)ename[0],flp))
						{
						SDS_ssadd(ename, ss, ss);

						// If we actually add something to the selection set, set return to indicate
						// we found something
						//
						ret = RTNORM;
						}
					}
				fl1--;
				}
			return ret;
			}
		// End of previous selection set
		// ----------------------------------------
		if(strisame(str, "I"/*DNT*/) || strisame(str, "_I"/*DNT*/)) {
			/*DG - 18.12.2001*/// (ssget"I") must look at PICKFIRST.
			resbuf	pickfirstRb;
			SDS_getvar(NULL, DB_QPICKFIRST, &pickfirstRb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			if(pickfirstRb.resval.rint && SDS_CURDOC && RTNORM==sds_sslength(SDS_CURDOC->m_pGripSelection,&fl1) && fl1>0L) {
				SDS_sscpy(ss,SDS_CURDOC->m_pGripSelection);
				//set selection set of objects as previous set....
				if (bAddToPrevSelSet)
					{
					sds_ssfree(SDS_CURDOC->m_pPrevSelection);
					SDS_ssadd(NULL, NULL, SDS_CURDOC->m_pPrevSelection);
					/*DG - 18.12.2001*/// Misprint?
					//SDS_sscpy(ss,SDS_CURDOC->m_pPrevSelection);
					SDS_sscpy(SDS_CURDOC->m_pPrevSelection, ss);
					}

				return(RTNORM);
			}
			return(RTERROR);
		}

		if (strisame(str, "LAST"/*DNT*/) || strisame(str, "_LAST"/*DNT*/) ||
			strisame(str, "L"/*DNT*/) || strisame(str, "_L"/*DNT*/)) {
			if(SDS_CURDWG->entlast((tilemode==0 && cvport==1) ? 1 : 0)==NULL) return(RTERROR);
			ename[1]=(long)SDS_CURDWG;
			ename[0]=(long)SDS_CURDWG->entlast((tilemode==0 && cvport==1) ? 1 : 0);

			// Check for locked or frozen layers. - Allow selection of ents on Locked layers via Lisp for ACAD compatibility (Bug#6391) AM 6/1/98
			if (!SDS_DoingLispCode)
				{
				layhip=((db_handitem *)ename[0])->ret_layerhip();
				if(layhip)
					{
					int flags;
					if(layhip->get_70(&flags))
						{
						if (flags&IC_LAYER_FROZEN)
							{
							return(RTERROR);
							}
						if ((flags&IC_LAYER_LOCKED) &&
							!bIncludeLockedLayers)
							{
							return(RTERROR);
							}
						}
					}
				}

			if(RTNORM!=sds_ssadd(ename,NULL,ss)) return(RTERROR);
			// set selection set of objects as previous set....
			if (bAddToPrevSelSet)
				{
				sds_ssfree(SDS_CURDOC->m_pPrevSelection);
				SDS_ssadd(NULL, NULL, SDS_CURDOC->m_pPrevSelection);
				SDS_ssadd(ename, SDS_CURDOC->m_pPrevSelection, SDS_CURDOC->m_pPrevSelection);
				}

			return(RTNORM);
		}
	}

	ename[1]=(long)flp;

	struct gr_selectorlink *gs=NULL;

	// set selection set of objects as previous set....
	if (bAddToPrevSelSet)
		{
		sds_ssfree(SDS_CURDOC->m_pPrevSelection);
		SDS_ssadd(NULL, NULL, SDS_CURDOC->m_pPrevSelection);
		}

	if(!str) { // Single point.
		SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		fr1=rb.resval.rreal;
		SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		fr1/=rb.resval.rpoint[1];
		SDS_getvar(NULL,DB_QPICKBOX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint==0) rb.resval.rint=5;
		fr1*=rb.resval.rint;
		ic_ptcpy(npt1,((double *)pt1));
		gr_ucs2rp(npt1,npt1,view);
		p1[0]=npt1[0]-fr1; p1[1]=npt1[1]-fr1; p1[2]=0.0;
		p3[0]=npt1[0]+fr1; p3[1]=npt1[1]+fr1; p3[2]=0.0;
		trb=sds_buildlist(RT3DPOINT,p1,RT3DPOINT,p3,0);
		gs=gr_initselector('W'/*DNT*/,'C'/*DNT*/,trb);
		sds_relrb(trb);
	} else if(strisame(str, "WINDOW"/*DNT*/) || strisame(str, "W"/*DNT*/) || strisame(str, "_W"/*DNT*/)) {
		if(pt1==NULL || pt2==NULL) return(RTERROR);
		ic_ptcpy(npt1,((double *)pt1));
		gr_ucs2rp(npt1,npt1,view);
		ic_ptcpy(npt2,((double *)pt2));
		gr_ucs2rp(npt2,npt2,view);
		trb=sds_buildlist(RT3DPOINT,npt1,RT3DPOINT,npt2,0);
		gs=gr_initselector('W'/*DNT*/,'I'/*DNT*/,trb);
		sds_relrb(trb);
	} else if(strisame(str, "CROSSING"/*DNT*/) || strisame(str, "C"/*DNT*/) || strisame(str, "_C"/*DNT*/)) {
		if(pt1==NULL || pt2==NULL) return(RTERROR);
		ic_ptcpy(npt1,((double *)pt1));
		gr_ucs2rp(npt1,npt1,view);
		ic_ptcpy(npt2,((double *)pt2));
		gr_ucs2rp(npt2,npt2,view);
		trb=sds_buildlist(RT3DPOINT,npt1,RT3DPOINT,npt2,0);
		gs=gr_initselector('W'/*DNT*/,'C'/*DNT*/,trb);
		sds_relrb(trb);
	} else if(strisame(str, "OUTSIDE"/*DNT*/) || strisame(str, "O"/*DNT*/) || strisame(str, "_O"/*DNT*/)) {
		if(pt1==NULL || pt2==NULL) return(RTERROR);
		ic_ptcpy(npt1,((double *)pt1));
		gr_ucs2rp(npt1,npt1,view);
		ic_ptcpy(npt2,((double *)pt2));
		gr_ucs2rp(npt2,npt2,view);
		trb=sds_buildlist(RT3DPOINT,npt1,RT3DPOINT,npt2,0);
		gs=gr_initselector('W'/*DNT*/,'O'/*DNT*/,trb);
		sds_relrb(trb);
	} else if(strisame(str, "WCIRCLE"/*DNT*/) || strisame(str, "WC"/*DNT*/) || strisame(str, "_WC"/*DNT*/)) {
		if(pt1==NULL) return(RTERROR);
		trb=(struct resbuf *)pt1;
		if(trb->restype!=RT3DPOINT) return(RTERROR);
		gr_ucs2rp(trb->resval.rpoint,trb->resval.rpoint,view);
		gs=gr_initselector('C'/*DNT*/,'I'/*DNT*/,trb);
	} else if(strisame(str,"OCIRCLE"/*DNT*/ ) || strisame(str,"OC"/*DNT*/ ) || strisame(str,"_OC"/*DNT*/ )) {
		if(pt1==NULL) return(RTERROR);
		trb=(struct resbuf *)pt1;
		if(trb->restype!=RT3DPOINT) return(RTERROR);
		gr_ucs2rp(trb->resval.rpoint,trb->resval.rpoint,view);
		gs=gr_initselector('C'/*DNT*/,'O'/*DNT*/,trb);
	} else if(strisame(str,"CCIRCLE"/*DNT*/ ) || strisame(str,"CC"/*DNT*/ ) || strisame(str,"_CC"/*DNT*/ )) {
		if(pt1==NULL) return(RTERROR);
		trb=(struct resbuf *)pt1;
		if(trb->restype!=RT3DPOINT) return(RTERROR);
		gr_ucs2rp(trb->resval.rpoint,trb->resval.rpoint,view);
		gs=gr_initselector('C'/*DNT*/,'C'/*DNT*/,trb);
	} else if(strisame(str,"POINT"/*DNT*/ ) || strisame(str,"PO"/*DNT*/ ) || strisame(str,"_PO"/*DNT*/ )) {
		if(pt1==NULL) return(RTERROR);
		ic_ptcpy(npt1,((double *)pt1));
		gr_ucs2rp(npt1,npt1,view);
		trb=sds_buildlist(RT3DPOINT,npt1,0);
		gs=gr_initselector('P'/*DNT*/,'I'/*DNT*/,trb);
		sds_relrb(trb);
	} else if(strisame(str, "FENCE"/*DNT*/) || strisame(str, "F"/*DNT*/) || strisame(str, "_F"/*DNT*/)) {
		if(pt1==NULL) return(RTERROR);
		trb=(struct resbuf *)pt1;
		while(trb!=NULL && trb->restype==RT3DPOINT) {
			gr_ucs2rp(trb->resval.rpoint,trb->resval.rpoint,view);
			trb=trb->rbnext;
		}
		gs=gr_initselector('F'/*DNT*/,'C'/*DNT*/,(struct resbuf *)pt1);
	} else if(strisame(str,"WPOLYGON"/*DNT*/ ) || strisame(str,"WP"/*DNT*/ ) || strisame(str,"_WP"/*DNT*/ )) {
		if(pt1==NULL) return(RTERROR);
		trb=(struct resbuf *)pt1;
		while(trb!=NULL && trb->restype==RT3DPOINT) {
			gr_ucs2rp(trb->resval.rpoint,trb->resval.rpoint,view);
			trb=trb->rbnext;
		}
		gs=gr_initselector('G'/*DNT*/,'I'/*DNT*/,(struct resbuf *)pt1);
	} else if(strisame(str,"OPOLYGON"/*DNT*/ ) || strisame(str,"OP"/*DNT*/ ) || strisame(str,"_OP"/*DNT*/ )) {
		if(pt1==NULL) return(RTERROR);
		trb=(struct resbuf *)pt1;
		while(trb!=NULL && trb->restype==RT3DPOINT) {
			gr_ucs2rp(trb->resval.rpoint,trb->resval.rpoint,view);
			trb=trb->rbnext;
		}
		gs=gr_initselector('G'/*DNT*/,'O'/*DNT*/,(struct resbuf *)pt1);
	} else if(strisame(str,"CPOLYGON"/*DNT*/ ) || strisame(str,"CP"/*DNT*/ ) || strisame(str,"_CP"/*DNT*/ )) {
		if(pt1==NULL) return(RTERROR);
		trb=(struct resbuf *)pt1;
		while(trb!=NULL && trb->restype==RT3DPOINT) {
			gr_ucs2rp(trb->resval.rpoint,trb->resval.rpoint,view);
			trb=trb->rbnext;
		}
		gs=gr_initselector('G'/*DNT*/,'C'/*DNT*/,(struct resbuf *)pt1);
	}

	sslen=0L;
	sds_sslength(ss,&sslen);

	if(str && (strisame(str,"X"/*DNT*/ ) || strisame(str, "_X"/*DNT*/))) SDS_InsideSSwithX=TRUE;

	bool bSelectingWithFilter = (bool)SDS_InsideSSwithX;
	bool bSelectingAll = ( str && (strisame(str, "~ALL ~_ALL"/*DNT*/) || strisame(str, "ALL"/*DNT*/) ||
								   strisame(str, "A"/*DNT*/) || strisame(str, "_A"/*DNT*/)));

	// If we are dealing with paper space and paper space viewports
	// we need to take VPLAYER settings into account
	//
	db_viewport *pCurrentViewport = NULL;
	if ( tilemode == 0 )
		{
		pCurrentViewport = flp->GetCurrentViewport();
		}

	for(elp=flp->entnext_noxref(NULL); elp!=NULL; elp=flp->entnext_noxref(elp)) {
		// Check for deleted objects and end stuff.
		if(elp->ret_deleted() || (!bIncludeInvisible && elp->ret_invisible()) || elp->ret_type()==DB_SEQEND ||
			elp->ret_type()==DB_ENDBLK || elp->ret_type()==DB_VERTEX) continue;


		ename[0]=(long)elp;

		if(bSelectingAll)
			{
			int psflag;
			elp->get_67(&psflag);
			if(SelPsEntsOnly && !psflag) continue;
			if(!SelPsEntsOnly && psflag) continue;

			// Check for frozen or locked layer.
			layhip=elp->ret_layerhip();
			if(layhip)
				{
				int flags;
				if ( layhip->get_70(&flags) )
					{
					if (flags&IC_LAYER_FROZEN)
						{
						continue;
						}
					if (flags&IC_LAYER_LOCKED)
						{
						continue;
						}
					}
				}
			}

		if(bSelectingWithFilter ||
		   bSelectingAll) {

			if(filter && !EntityMatchesFilterList(filter,elp,flp)) continue;
			// Must do a special case to handle attributes.
			if(elp->ret_type()==DB_ATTRIB) {
				for(telp=elp; telp!=NULL && telp->ret_type()==DB_ATTRIB; telp=flp->entnext_noxref(telp));
				if(telp==NULL) continue;
				if(telp->ret_type()==DB_SEQEND) ((db_seqend *)telp)->get_mainent(&telp);
				ename[0]=(long)telp;
				if(elp->ret_deleted()) continue;
				SDS_ssadd(ename, ss, ss);
				if (bAddToPrevSelSet)
					SDS_ssadd(ename, SDS_CURDOC->m_pPrevSelection, SDS_CURDOC->m_pPrevSelection);

				continue;
			}

			SDS_ssadd(ename, ss, ss);
			if (bAddToPrevSelSet)
				SDS_ssadd(ename, SDS_CURDOC->m_pPrevSelection, SDS_CURDOC->m_pPrevSelection);

			continue;
		}

		int psflag;
		elp->get_67(&psflag);
		if(SelPsEntsOnly && !psflag) continue;
		if(!SelPsEntsOnly && psflag) continue;

		// Here's where we take VPLAYER settings into account
		//
		if ( pCurrentViewport != NULL )
			{
			db_layertabrec *pLayer = (db_layertabrec *)elp->ret_layerhip();
			if ( pLayer != NULL )
				{
				if ( pCurrentViewport->IsLayerVpLayerFrozen( pLayer ) )
					{
					continue;
					}
				}
			}

		if ( bSpecialImageFilter &&
			(elp->ret_type() == DB_IMAGE) )
			{

			// The only place bSpecialImageFilter should be called is from
			// sds_update() where we are getting a crossing ss
			//
			ASSERT(strisame(str, "CROSSING"/*DNT*/));

			// If it is an image and we are in this special
			// case for images, then if any part of the crossing rectangle
			// intersects the image, it is in the selection set
			//
			elp->get_extent(extent[0],extent[1]);
			if ( (npt1[0] >= extent[0][0]) &&
				 (npt1[0] <= extent[1][0]) &&
				 (npt1[1] >= extent[0][1]) &&
				 (npt1[1] <= extent[1][1]))
				{
				SDS_ssadd(ename, ss, ss);
				if (bAddToPrevSelSet)
					SDS_ssadd(ename, SDS_CURDOC->m_pPrevSelection, SDS_CURDOC->m_pPrevSelection);

				continue;
				}
			if ( (pt2[0] >= extent[0][0]) &&
				 (pt2[0] <= extent[1][0]) &&
				 (pt2[1] >= extent[0][1]) &&
				 (pt2[1] <= extent[1][1]))
				{
				SDS_ssadd(ename, ss, ss);
				if (bAddToPrevSelSet)
					SDS_ssadd(ename, SDS_CURDOC->m_pPrevSelection, SDS_CURDOC->m_pPrevSelection);

				continue;
				}
			}

		if(elp->ret_type()!=DB_ATTRIB && filter && !EntityMatchesFilterList(filter,elp,flp)) continue;

#ifdef _USE_DISP_OBJ_PTS_3D_
		if(/*SDS_CURVIEW->m_fUseRegenList &&*/ elp->ret_type()!=DB_XLINE && elp->ret_type()!=DB_RAY) {
			beg=(struct gr_displayobject *)elp->ret_disp();
			freedisp=FALSE;
#else
		if(SDS_CURVIEW->m_fUseRegenList && elp->ret_type()!=DB_XLINE && elp->ret_type()!=DB_RAY) {
			beg=(struct gr_displayobject *)elp->ret_disp();
			freedisp=FALSE;
#endif
		} else {
			telp=elp;
			if(SDS_getdispobjs(flp,&elp,&beg,&end,NULL,flp,view,1)) continue;
			freedisp=TRUE;
			elp=telp;
			extent[0][0]=extent[0][1]=extent[0][2]=0.0;
			extent[1][0]=extent[1][1]=extent[1][2]=0.0;
			elp->get_extent(savext[0],savext[1]);
			elp->set_extent(extent[0],extent[1]);
		}

		if(gs && elp) {

			tdo1=(struct gr_displayobject *)elp->ret_disp();
			// Modified CyberAge VSB 03/16/2001
			// Reason: Fix for Bug No. 70229 from Bugzilla [
			if ( tdo1 == NULL && beg != NULL && freedisp == TRUE )
				freedisp = FALSE;
			// ]
			elp->set_disp((void *)beg);

			// Check for locked of frozen layers.- Allow selection of ents on Locked layers via Lisp for ACAD compatibility (Bug#6391) AM 6/1/98
			if (!SDS_DoingLispCode)
				{
				layhip=elp->ret_layerhip();
				if(layhip)
					{
					int flags;
					if ( layhip->get_70(&flags) )
						{
						if (flags&IC_LAYER_FROZEN)
							{
							goto out;
							}
						if ( (flags&IC_LAYER_LOCKED) &&
							 !bIncludeLockedLayers )
							{
							goto out;
							}
						}
					}
				}

			// AG: set initial (unchanged) display object in case of grip editing
			if(1 == SDS_AtNodeDrag && (long)elp == SDS_NodeDragEnt[0] && SDS_NodeDragEntDispObj )
			{
				saveDop = beg;
				bSaveDop = true;
				elp->set_disp( (void*) SDS_NodeDragEntDispObj );
			}

			/*D.G.*/// Use a "Point Inside" selector for TTF mtext if there is not in "str".
			short	IsSelected;
			if( elp->ret_type() == DB_MTEXT && ((CDbMText*)elp)->ret_disp() &&
				/*((gr_displayobject*)((CDbMText*)elp)->ret_disp())->truetypeDisplayObject*/false &&
				!str )
			{
				gr_selectorlink*	gs = NULL;
				sds_point			pt;
				resbuf*				rb;
				ic_ptcpy(pt, (double*)pt1);
				gr_ucs2rp(pt, pt, view);
				rb = sds_buildlist(RT3DPOINT, pt, 0);
				gs = gr_initselector('P'/*DNT*/, 'I'/*DNT*/, rb);
				sds_relrb(rb);
				if(gs)
					IsSelected = gr_selected(elp, gs);
				else
				{
					// AG: reset current display object in case of grip editing
					if( bSaveDop )
					{
						elp->set_disp( (void*) saveDop );
						bSaveDop = false;
						saveDop = NULL;
					}

					goto out;
				}
			}
			else
#ifdef _USE_DISP_OBJ_PTS_3D_
				IsSelected = gr_selected2(elp, gs, view);
#else
				IsSelected = gr_selected(elp, gs);
#endif

			// AG: reset current display object in case of grip editing
			if( bSaveDop )
			{
				elp->set_disp( (void*) saveDop );
				bSaveDop = false;
				saveDop = NULL;
			}


			if(IsSelected) {
				// Must do a special case to handle attributes.
				if(elp->ret_type()==DB_ATTRIB) {
					for(telp=elp; telp!=NULL && telp->ret_type()==DB_ATTRIB; telp=flp->entnext_noxref(telp));
					if(telp==NULL) goto out;
					if(telp->ret_type()==DB_SEQEND) ((db_seqend *)telp)->get_mainent(&telp);
					ename[0]=(long)telp;
					// We have to check the filter here because we didn't
					// check it above because of the object selected was a DB_ATTRIB.
					if(filter && !EntityMatchesFilterList(filter,telp,flp)) goto out;
					if(!sslen && RTNORM==sds_ssmemb(ename,ss)) goto out;
				}
				if(!str) {
					// This is kinda bogus, but it works...
					// When selecting with a single point there
					// should only be one object in the SS.  The trickey
					// thing is ACAD seems to search through the DB from
					// last ent to first ent.  This trick below will work
					// but it may be a bit slow.
					sds_ssfree(ss);
					sds_ssadd(ename,NULL,ss);
				} else {
					//SDS_ssadd(ename,ss,ss);	//DP
					sds_ssadd(ename,ss,ss);
					if (bAddToPrevSelSet)
						SDS_ssadd(ename, SDS_CURDOC->m_pPrevSelection, SDS_CURDOC->m_pPrevSelection);

				}
#define _SUPPORT_GROUPS_
#ifdef _SUPPORT_GROUPS_
				// select all entities in groups to which this belongs
				sds_resbuf pickstyleValue;
				sds_getvar("PICKSTYLE", &pickstyleValue);
				if(pickstyleValue.resval.rint & 1)
				{
					sds_name entity;
					sds_name groups = {0, 0};
					entity[0] = (long)elp;
					entity[1] = (long)SDS_CURDWG;
					int nGroups = cmd_findGroup(groups, entity);
					if(nGroups != 0)
					{
						sds_name group;
						sds_name entities;
						long nEntities;
						int i, j;
						for(i = 0; i < nGroups; ++i)
						{
							sds_ssname(groups, i, group);
							if(((CDbGroup*)group[0])->isSelectable() &&
								!((CDbGroup*)group[0])->ret_deleted())
							{
								cmd_getSSFromGroup(entities, group);
								sds_sslength(entities, &nEntities);
								for(j = 0; j < nEntities; ++j)
								{
									sds_ssname(entities, j, entity);
									if(!((db_handitem*)entity[0])->ret_deleted())
										sds_ssadd(entity, ss, ss);
								}
								sds_ssfree(entities);
							}
						}
						sds_ssfree(groups);
					}
				}
#endif
			}
			out: ;
			if(freedisp) {
				elp->set_extent(savext[0],savext[1]);
				gr_freedisplayobjectll(elp->ret_disp());
				elp->set_disp((void *)tdo1);
			}
			// Following two strings were commented because clicking on overlapping entities should select the most recently created entity
			//if(!str && sds_sslength(ss,&fl1)==RTNORM && fl1 == 1)
			//	break;
			continue;
		}
	}

	SDS_InsideSSwithX=FALSE;

	if(gs) gr_freeselector(gs);

	if(sds_sslength(ss,&fl1)==RTERROR || fl1==0L) {
		sds_ssfree(ss);
		return(RTERROR);
	}

	return(RTNORM);
}

struct sds_resbuf *SDS_MnuStrToRB(char *mnustr) {
// *** This function converts the passed menu string(mnustr) into a
// *** linked-list of commands.
	char *fcp3=NULL,*fcp1,*fcp2,*fcp4,fc1;
	int inlsp,inquote;
	struct sds_resbuf *beg,*cur,*tmp;

	struct resbuf rb;
	int EatNextEnter=0;

	beg=cur=tmp=NULL;
	if(mnustr==NULL) return(NULL);
	db_astrncpy(&fcp3,mnustr,-1);

	if(*fcp3 && fcp3[strlen(fcp3)-1]==' '/*DNT*/) fcp3[strlen(fcp3)-1]=';'/*DNT*/;
	// *** Walk the string and build list of commands.

	if(!*fcp3) {
		delete fcp3;
	   return(sds_buildlist(RTSTR,""/*DNT*/,0));
	}
	if(*fcp3=='*'/*DNT*/ && !SDS_IgnoreMenuStar) SDS_MultibleMode=2;

	for(fcp1=(*fcp3=='*'/*DNT*/ && !SDS_IgnoreMenuStar)?  fcp3+1 : fcp3; *fcp1!=0; fcp1++) {
		switch(*fcp1) {
			case '^'/*DNT*/:
				fcp1++;
				switch(*fcp1) {
					case 'B': case 'b':
						if(SDS_getvar(NULL,DB_QSNAPMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) break;
						if(rb.resval.rint) rb.resval.rint=0; else rb.resval.rint=1;
						if(sds_setvar("SNAPMODE"/*DNT*/,&rb)!=RTNORM) break;
						break;
					case 'C': case 'c':

						// Don't break if nothing is waiting for input
						//
						if ( IDS_GetThreadController()->GetWaitThreadID() == 0 )
							break;

						{
						InputEvent theInputEvent( SDS_EVM_KEYCHAR, SDS_ESCAPECHAR );
						SDS_ProcessInputEvent(&theInputEvent);
 						}
						break;
					case 'D': case 'd':
						if(SDS_getvar(NULL,DB_QCOORDS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) break;
						if(rb.resval.rint) rb.resval.rint=0; else rb.resval.rint=1;
						if(sds_setvar("COORDS"/*DNT*/,&rb)!=RTNORM) break;
						break;
					case 'E': case 'e':
						if(SDS_getvar(NULL,DB_QSNAPISOPAIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) break;
						++rb.resval.rint;
						if(rb.resval.rint==3) rb.resval.rint=0;
						if(sds_setvar("SNAPISOPAIR"/*DNT*/,&rb)!=RTNORM) break;
						break;
					case 'G': case 'g':
						if(SDS_getvar(NULL,DB_QGRIDMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) break;
						if(rb.resval.rint) rb.resval.rint=0; else rb.resval.rint=1;
						if(sds_setvar("GRIDMODE"/*DNT*/,&rb)!=RTNORM) break;
						break;
					case 'H': case 'h':
						{
						InputEvent theInputEvent( SDS_EVM_KEYCHAR,'\b'/*DNT*/ );
						SDS_ProcessInputEvent( &theInputEvent );
						SDS_userbreak=0;
						}
						goto bail;
					case 'O': case 'o':
						if(SDS_getvar(NULL,DB_QORTHOMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) break;
						if(rb.resval.rint) rb.resval.rint=0; else rb.resval.rint=1;
						if(sds_setvar("ORTHOMODE"/*DNT*/,&rb)!=RTNORM) break;
						break;
					case 'P': case 'p':
						if(SDS_getvar(NULL,DB_QMENUECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) break;
						if(rb.resval.rint) rb.resval.rint=0; else rb.resval.rint=1;
						if(sds_setvar("MENUECHO"/*DNT*/,&rb)!=RTNORM) break;
						break;
					case 'Q': case 'q':
					case 'T': case 't':
						if(SDS_getvar(NULL,DB_QTABMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) break;
						if(rb.resval.rint) rb.resval.rint=0; else rb.resval.rint=1;
						if(sds_setvar("TABMODE"/*DNT*/,&rb)!=RTNORM) break;
						break;
					case 'V': case 'v':
					case 'M': case 'm':
						continue;
					case 'Z': case 'z':
						goto bail;
					default:
						cmd_ErrorPrompt(CMD_ERR_UNRECOGMENU,0);
						delete fcp3;
						return(NULL);
				}
				continue;
			case ';':
				if(EatNextEnter) {	EatNextEnter=0; continue; }
				tmp=sds_buildlist(RTSTR,""/*DNT*/,0);
				break;
			case ' ':
				if(EatNextEnter) {	EatNextEnter=0; continue; }
				tmp=sds_buildlist(RTSTR," "/*DNT*/,0);
				break;
			case '\\':
				tmp=sds_buildlist(RTSTR,PAUSE,0);
				break;
			case '(':
				inlsp=inquote=0;
				for(fcp2=fcp1; *fcp2!=0; fcp2++) {
					if(!inquote && *fcp2=='(') ++inlsp;
					if(!inquote && *fcp2==')') --inlsp;
					if(!inquote && *fcp2==';') break;
					if(!inlsp && *fcp2==' ') break;
					if(*fcp2=='\"') if(inquote) inquote=0; else inquote=1;
					if(!inlsp && *(fcp2+1)!='(') { fcp2++; break; }
				}
				fc1=(*fcp2);
				*fcp2=0;
				tmp=sds_buildlist(RTSTR,fcp1,0);
				if(fc1!=0) *fcp2=fc1;
				fcp1=fcp2;
 				// This line is for ^pLINE^p; commands.
				if(*fcp1=='^'/*DNT*/) EatNextEnter=1;
			   // *** We don't want the pointer to walk past the end of the string or the start
				// *** of another lisp expression.
				if(!*fcp1 || (*fcp1!=';'/*DNT*/ && *fcp1!=' ')) fcp1--;
				break;
			default:
				// *** I assume we have a command string here.
				// EBATECH(R.Arayashiki) -[ for MBCS String
				//for(fcp2=fcp1; *fcp2!=0 && *fcp2!=';'/*DNT*/ && *fcp2!=' '/*DNT*/ && *fcp2!='\\'/*DNT*/ && *fcp2!='^'/*DNT*/; fcp2++);
				for(fcp2=fcp1; *fcp2!=0 && *fcp2!=';'/*DNT*/ && *fcp2!=' '/*DNT*/ && *fcp2!='\\'/*DNT*/ && *fcp2!='^'/*DNT*/; fcp2++)
				{
					if ((_MBC_LEAD ==_mbbtype((unsigned char)*fcp2,0)) &&
					  (_MBC_TRAIL==_mbbtype((unsigned char)fcp2[1],1)))
					{
						++fcp2;
					}
				}
				// ]- EBATECH
				if(*fcp2=='\\') ++fcp2;
				if(fcp2[0]=='^'/*DNT*/ && (fcp2[1]=='Z'/*DNT*/ || fcp2[1]=='z'/*DNT*/ || fcp2[1]=='H'/*DNT*/ || fcp2[1]=='h'/*DNT*/)) {
					for(fcp4=fcp1; *fcp4!='^'; fcp4++) {
						InputEvent theInputEvent( SDS_EVM_KEYCHAR, *fcp4 );
						SDS_ProcessInputEvent( &theInputEvent );
					}
					if(fcp2[1]=='H' || fcp2[1]=='h') {
						InputEvent theInputEvent( SDS_EVM_KEYCHAR, '\b' /*DNT*/ );
						SDS_ProcessInputEvent( &theInputEvent );
					}
					SDS_userbreak=0;
					goto bail;
				}

				fc1=(*fcp2);
				*fcp2=0;

				if(fc1==' '/*DNT*/)
					tmp=sds_buildlist(RTSTR,fcp1,RTSTR," "/*DNT*/,0);
				else
					tmp=sds_buildlist(RTSTR,fcp1,0);

				if(fc1!=0) *fcp2=fc1;
				fcp1=fcp2;
				// This line is for ^pLINE^p; commands.
				if(*fcp1=='^'/*DNT*/) EatNextEnter=1;
				// *** We don't want the pointer to walk past the end of the string.
				if(!*fcp1 || *fcp1=='^'/*DNT*/ || *fcp1=='\\'/*DNT*/) fcp1--;
				break;
		}

		if(tmp==NULL)
		{
			cmd_ErrorPrompt(CMD_ERR_UNABLE,0);			// The user shouldn't need to care what went wrong internally.	OK?
 //			sds_printf("\n-- Error creating linked list. --");
			goto bail;
		}

		if(beg==NULL)
		{
			beg=cur=tmp;
		}
		else
		{
			cur=cur->rbnext=tmp;
		}
		while(cur->rbnext) cur=cur->rbnext;
	}

	delete fcp3;
	return(beg);

	bail:

	delete fcp3;
	IC_RELRB(beg);
	return(NULL);
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!
// Used to check we are on the engine thread
//
// TODO -- Turn this on!!!
//
bool
OnEngineThread( void )
	{

	// return (GetCurrentThreadId() == GetEngineThreadId() );

	return true;
	}

// ------------------------------
// Used to check we are on the UI thread
//
//
bool
OnUIThread( void )
	{
	return ( GetCurrentThreadId() == IDS_GetThreadController()->GetMainThreadID() );
	}


// ***********************************
//
//

LRESULT
ExecuteUIAction( int iAction, LPARAM lParam )
	{
	ASSERT( IsWindow( IcadSharedGlobals::GetMainHWND() ) );
	return SendMessage( IcadSharedGlobals::GetMainHWND(), WM_COMMAND, iAction, lParam );
	}



LRESULT
PostUIAction( int iAction, LPARAM lParam )
	{
	ASSERT( IsWindow( IcadSharedGlobals::GetMainHWND() ) );
	return PostMessage( IcadSharedGlobals::GetMainHWND(), WM_COMMAND, iAction, lParam );
	}

/////////////////////////////////////////////////////////////////////////////////
//4M Item:25->
// Replacement of SDS_NodeList with SDS_NodeList_New
/*
SDS_NodeList::~SDS_NodeList()
{
	removeAll();
}

void SDS_NodeList::removeAll()
{
	SDS_NodeListDef *cur, *tmp;
	for( cur = m_beg; cur != NULL; )
	{
		tmp = cur;
		cur = cur->next;
		tmp->ptArray.RemoveAll();
		delete tmp;
	}
	m_beg = m_end = NULL;
}

SDS_NodeListDef* SDS_NodeList::find( sds_name ename, SDS_NodeListDef *&prev, SDS_NodeListDef *&next )
{
	SDS_NodeListDef *cur;
	prev = next = NULL;
	for( cur = m_beg; cur != NULL; cur = cur->next )
	{
		if ( sds_name_equal(cur->ename, ename) )
		{
			next = cur->next;
			break;
		}
		prev = cur;
	}
	if( !cur )
		prev = NULL;
	return cur;
}

void SDS_NodeList::add( sds_name ename, sds_point pt )
{
	SDS_NodeListDef *cur, *prev, *next;
	if( cur = find( ename, prev, next ) )
	{
		cur->ptArray.Add( CNodeData( pt ) );
	}
	else
	{
		cur = new SDS_NodeListDef( ename );
		if( cur )
		{
			if( m_beg )
			{
				ASSERT( m_end );
				m_end->next = cur;
				m_end = cur;
			}
			else
			{
				ASSERT( !m_end );
				m_beg = m_end = cur;
			}
			cur->ptArray.Add( CNodeData( pt ) );
		}
	}
}

void SDS_NodeList::remove( sds_name ename )
{
	SDS_NodeListDef *cur, *prev, *next;

	if( cur = find( ename, prev, next ) )
	{
		if( prev && next )
			prev->next = next;
		else if( !prev && !next )
		{
			m_beg = m_end = NULL;
		}
		else if( !prev )
		{
			m_beg = next;
		}
		else if( !next )
		{
			prev->next = NULL;
			m_end = prev;
		}

		// delete found element with its array of points
		cur->ptArray.RemoveAll();
		delete cur;
	}
}

void SDS_NodeList::begin()
{
	m_cur = m_beg;
	m_ptIdx = 0;
}

BOOL SDS_NodeList::getPair( sds_name ename, sds_point pt )
{
	for( ; m_cur != NULL; m_cur = m_cur->next )
	{
		int numOfPoints = m_cur->ptArray.GetSize();
		while( m_ptIdx < numOfPoints )
		{
			ic_encpy(ename, m_cur->ename);
			ic_ptcpy(pt, (m_cur->ptArray.GetAt(m_ptIdx)).m_cen);
			m_ptIdx++;
			return TRUE;
		}
		m_ptIdx = 0;
	}
	return FALSE;
}
*/
#include "NodeList4M.cpp"
//<-4M Item:25



int cmd_translate_drag(sds_point dragpt, sds_matrix mt);

sds_name	SDS_DraggingSS = {0L, 0L};	// we store here the selection set which we are moving
										// (allocated in SDS_StartDraggingSS and cleared in SDS_EndDraggingSS)
bool		SDS_bDraggingSS = false;	// flags if we are moving a selection set
										// (note, we cannot use SDS_IsDragging for this)

/*-------------------------------------------------------------------------*//**
Function remove Acis objects from selection set (in fact pCurDoc->m_pGripSelection),
to disable dragging of Acis objects in build with limited support.
@author Alexey Malov
@param <=> selectin set
*//*--------------------------------------------------------------------------*/
static
void removeAcisObjectsFromSet(sds_name ss)
{
	sds_name e;
	for (long i = 0; RTNORM == sds_ssname(ss, i, e); i++)
	{
		db_handitem* item = reinterpret_cast<db_handitem*>(e[0]);
		int type = item->ret_type();
		if (type == DB_3DSOLID ||
			type == DB_BODY ||
			type == DB_REGION)
		{
			sds_ssdel(e, ss);
			i--;
		}
	}
}

void SDS_StartDraggingSS(sds_point Pt)
{
	CIcadDoc*	pCurDoc = SDS_CURDOC;
	resbuf		rb;
	int			ret;

	if(!pCurDoc)
		return;

	if((ret = SDS_getvar(NULL, DB_QDRAGMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES)) != RTNORM)
		return;

	//modified cybage AJK on 03/09/2001
	//reason - improper working of dragmode on [
	//bugzilla bug no 77837
	//if(!cmd_InsideSds_draggen && rb.resval.rint == 2)
	//Modified Cybage AW 03-12-01 [
	//Reason : Entity dragging is enabled when DRAGMODE = OFF
	//if(!cmd_InsideSds_draggen && rb.resval.rint != 0)//]Modified Cybage 03/09/2001
	if(!cmd_InsideSds_draggen && rb.resval.rint <= 2)//Modified Cybage AW 03-12-01 ]
	{
		extern cmd_drag_globalpacket	cmd_drag_gpak;
		extern sds_point				SDS_PointDisplay;
		extern int						SDS_PointDisplayMode;
		extern int						SDS_DidDraggenOnce;
		cmd_drag_globalpacket*	gpak = &cmd_drag_gpak;

		if((ret = sds_initget(SDS_RSG_OTHER|SDS_RSG_NOLIM|SDS_RSG_NOPMENU, NULL)) != RTNORM)
			return;

		SDS_Tvars = SDS_CMainWindow->m_pDragVarsCur;

		if((SDS_CMainWindow->m_pDragVarsCur = new SDS_dragvars ) == NULL)
			return;
		memset(SDS_CMainWindow->m_pDragVarsCur,0,sizeof(SDS_dragvars));

		// disallow dragging of Acis objects
#if defined(BUILD_WITH_LIMITED_ACIS_SUPPORT)
		removeAcisObjectsFromSet(pCurDoc->m_pGripSelection);
#else
		if(!CModeler::checkFullModeler()) 
			removeAcisObjectsFromSet(pCurDoc->m_pGripSelection);
#endif
		SDS_sscpy(SDS_DraggingSS, pCurDoc->m_pGripSelection);

		gpak->reference = 0;
		gpak->refval = 0.0;
		gpak->rband = 0;

		ic_ptcpy(gpak->pt1, Pt);
		ic_ptcpy(gpak->lastpt, gpak->pt1);

		ic_ptcpy(SDS_PointDisplay, gpak->pt1);

		SDS_AtCopyMoveCommand = SDS_CMainWindow->m_fIsCtrlDown ? 2 : 1;
		SDS_PointDisplayMode = 5;
		cmd_InsideMoveCopyCommand = TRUE;
		cmd_InsideSds_draggen = TRUE;

		SDS_CMainWindow->m_pDragVarsCur->breakout = 0;
		SDS_CMainWindow->m_pDragVarsCur->scnf = cmd_translate_drag;
		SDS_CMainWindow->m_pDragVarsCur->cursor = 0;
		ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->pt1, gpak->pt1);
		ic_encpy(SDS_CMainWindow->m_pDragVarsCur->nmSelSet, pCurDoc->m_pGripSelection);

		POSITION pos = pCurDoc->GetFirstViewPosition();
		while(pos)
		{
			CIcadView *pView = (CIcadView *)pCurDoc->GetNextView(pos);
			ASSERT_KINDOF(CIcadView, pView);

			SDS_CMainWindow->m_pDragVarsCur->vects = SDS_SSToVects(pCurDoc->m_pGripSelection, SDS_CURDWG, pView->m_pCurDwgView);
			pView->m_pDragVects = SDS_CMainWindow->m_pDragVarsCur->vects;
		}

		for(int i = 0; i < 4; i++)
			for(int j = 0; j < 4; j++)
				SDS_CMainWindow->m_pDragVarsCur->matx[i][j] = (i == j)? 1.0 : 0.0;

		SDS_IsDragging = 1;
		SDS_SetCursor(IDC_ICAD_CROSS);
		SDS_DidDraggenOnce = 0;
		SDS_bDraggingSS = true;
		SDS_EventMask(SDS_EVM_MOUSEXYZ|SDS_EVM_MOUSEBUTTON);
	}
}

void SDS_EndDraggingSS(sds_point Pt, bool IsCancel /*= false*/)
{
	/*DG - 9.7.2002*/// This function is called from many places but in most cases it mustn't do anything
	// because we are not dragging.
	if(!SDS_bDraggingSS)
		return;

	CIcadDoc*	pCurDoc = SDS_CURDOC;

	if(!pCurDoc)
		return;

	extern cmd_drag_globalpacket	cmd_drag_gpak;
	extern int						SDS_PointDisplayMode;
	extern int						SDS_DidDraggenOnce;

	CIcadView*	pCurView = SDS_CURVIEW;
	if(pCurView->m_CursorOn)
	{
		IcadCursor::AppCursor().DrawCursor(pCurView->m_LastCursorPos, pCurView->m_LastCursorType, pCurView);
		pCurView->m_CursorOn = FALSE;
	}

	SafeRecursionCounter theCounter( &SDS_InInternalFunction );	// need this for CMDACTIVE
																// (BA check it at the end of commands, for example)

	bool		bIsCopying = !!SDS_CMainWindow->m_fIsCtrlDown;

	SDS_IsDragging = 0;
	SDS_bDraggingSS = false;

	ExecuteUIAction(ICAD_WNDACTION_ENDDRAG);
	ExecuteUIAction(ICAD_WNDACTION_FREEVIEWRBS);

	IC_FREE(SDS_CMainWindow->m_pDragVarsCur);
	SDS_CMainWindow->m_pDragVarsCur = SDS_Tvars;

	cmd_InsideSds_draggen = FALSE;
	cmd_InsideMoveCopyCommand = FALSE;
	SDS_PointDisplayMode = 0;

	if((bIsCopying && SDS_AtCopyMoveCommand != 2) || (!bIsCopying && SDS_AtCopyMoveCommand != 1))
	{
		SDS_AtCopyMoveCommand = 0;
		goto out;
	}

	SDS_AtCopyMoveCommand = 0;

	if(!IsCancel && SDS_DidDraggenOnce)
	{
		cmd_drag_globalpacket*	gpak = &cmd_drag_gpak;
		sds_matrix	mtx;
		resbuf		rb;
		sds_point	pt;
		int			SnapStyle;

		ic_ptcpy(pt, Pt);

		if(SDS_getvar(NULL, DB_QORTHOMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) != RTNORM)
			goto out;
		if((rb.resval.rint && !SDS_CMainWindow->m_fIsShiftDown) || (!rb.resval.rint && SDS_CMainWindow->m_fIsShiftDown))
		{
			if(SDS_getvar(NULL, DB_QSNAPSTYL, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) != RTNORM)
				goto out;
			if(rb.resval.rint==0)
				SnapStyle = -1;
			else
			{
				if(SDS_getvar(NULL, DB_QSNAPISOPAIR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) != RTNORM)
					goto out;
				SnapStyle = rb.resval.rint;
			}
			if(SDS_getvar(NULL, DB_QSNAPANG, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) != RTNORM)
				goto out;
			if(!SDS_SkipLastOrthoSet && ic_ortho(gpak->pt1, rb.resval.rreal, pt, pt, SnapStyle) != 0)
				goto out;
		}

		int i, j;
		for(i = 0; i < 4; i++)
		{
			for(j = 0; j < 3; j++)
				mtx[i][j] = 0.0;

			if(i < 3)
				mtx[i][3] = pt[i] - gpak->pt1[i];
		}
		for(i = 0; i < 4; i++)
			mtx[i][i] = 1.0;


		SDS_SetUndo(IC_UNDO_COMMAND_BEGIN,
					bIsCopying ? (void *)"COPY"/*DNT*/ : (void *)"MOVE"/*DNT*/,
					NULL, NULL, SDS_CURDWG);

		SDS_FreeNodeList(pCurDoc);

		// notify BA and others (BA does some things after the end of a command: updates links, etc.)
		SDS_CallUserCallbackFunc(SDS_CBCMDBEGIN,
								 bIsCopying ? (void *)"COPY"/*DNT*/ : (void *)"MOVE"/*DNT*/,
								 NULL, NULL);

		if(bIsCopying)
		{
			resbuf			rbFilter, *elist;
			sds_name		NewSS, ename, CopiedEnt;
			db_handitem		*NewEntity, *OldEntity;
			ReactorFixups	reactorFixup;

			SDS_CallUserCallbackFunc(SDS_CBBEGINCLONE, (void *)SDS_DraggingSS, NULL, NULL);

			SDS_CURDWG->SetCopyingFlag(true);

			rbFilter.restype = RTSTR;
			rbFilter.resval.rstring = "*"/*DNT*/;
			rbFilter.rbnext = NULL;

			/*DG 3.10.2001*/// Entities in the ss must be ordered by handles when bit 1 of SORTENTS is set.
			resbuf	sortentsRb;
			// 2002/11/30 SORTENTS is dictionary var #SORTENTSISDICVAR#
			SDS_getvar("SORTENTS"/*DNT*/, 0, &sortentsRb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			//SDS_getvar(0, DB_QSORTENTS, &sortentsRb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			if(sortentsRb.resval.rint & 1)
				SDS_ssOrder(SDS_DraggingSS);

			for(NewSS[0] = NewSS[1] = 0L, i = 0;
				sds_ssname(SDS_DraggingSS, i, ename) == RTNORM;
				i++)
			{
				elist = sds_entgetx(ename, &rbFilter);

				if(ic_assoc(elist,66) == 0 && ic_rbassoc->resval.rint == 1 &&
				   ic_assoc(elist,0) == 0 &&
				   (strisame("INSERT"/*DNT*/, ic_rbassoc->resval.rstring) ||
					strisame("POLYLINE"/*DNT*/, ic_rbassoc->resval.rstring)))
				{
					while(!strisame("SEQEND"/*DNT*/, ic_rbassoc->resval.rstring))
					{
						SDS_simplexforment(ename, elist, mtx);

						if (sds_entmake(elist) != RTNORM)
							goto bail;

						IC_RELRB(elist);

						if(sds_entnext_noxref(ename, ename) != RTNORM)
							goto bail;

						elist = sds_entgetx(ename, &rbFilter);
						ic_assoc(elist, 0);
					}
				}

				SDS_simplexforment(ename, elist, mtx);

				if(IC_TYPE_FROM_ENAME(elist->resval.rlname) == DB_IMAGE)
					SDS_entmakeImageCopy(elist);
				else
				{
					// This is for dimensions.
					SDS_AtNodeDrag = 1;

					if(sds_entmake(elist) != RTNORM)
						goto bail;

					// This is for dimensions.
					SDS_AtNodeDrag = 0;
				}

				IC_RELRB(elist);

				OldEntity = (db_handitem*)ename[0];
				if(OldEntity->ret_type() == DB_SEQEND)
					((db_seqend *)OldEntity)->get_mainent(&OldEntity);

				sds_entlast(CopiedEnt);
				sds_ssadd(CopiedEnt, NewSS, NewSS);
				NewEntity = (db_handitem*)CopiedEnt[0];

				reactorFixup.AddHanditem(NewEntity);
				reactorFixup.AddHandleMap(OldEntity->RetHandle(), NewEntity->RetHandle());
			}	// for(...

			SDS_CURDWG->SetCopyingFlag(false);

			SDS_CallUserCallbackFunc(SDS_CBENDCLONE, (void*)SDS_DraggingSS, NewSS, (void*)0);

			reactorFixup.ApplyFixups(SDS_CURDWG);

			bail:
			sds_ssfree(NewSS);
		}	// if(bIsCopying)
		else
		{
			if(SDS_progxformss(SDS_DraggingSS, mtx, 0) != RTNORM)
				goto out;
			cmd_checkReactors(SDS_DraggingSS);
		}

		sds_entmake(NULL);

		// see comments about notifications above
		SDS_CallUserCallbackFunc(SDS_CBCMDEND,
								 bIsCopying ? (void *)"COPY"/*DNT*/ : (void *)"MOVE"/*DNT*/,
								 NULL, NULL);

		SDS_SetUndo(IC_UNDO_COMMAND_END,
					bIsCopying ? (void *)"COPY"/*DNT*/ : (void *)"MOVE"/*DNT*/,
					NULL, NULL, SDS_CURDWG);

		SDS_getvar(NULL, DB_QUNDOCTL, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		int Save_UNDOCTL = rb.resval.rint;
		rb.resval.rint &= ~IC_UNDOCTL_ON;
		SDS_setvar("UNDOCTL"/*DNT*/, -1, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);

		cmd_redrawall();

		rb.resval.rint = Save_UNDOCTL;
		SDS_setvar("UNDOCTL"/*DNT*/, -1, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);

		sds_ssfree(pCurDoc->m_pGripSelection);
		SDS_sscpy(pCurDoc->m_pGripSelection, SDS_DraggingSS);

		long		EntNum = 0L;
		sds_name	EntName;
		SDS_getvar(NULL, DB_QGRIPS, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		while(sds_ssname(pCurDoc->m_pGripSelection, EntNum++, EntName) == RTNORM)
		{
			sds_redraw(EntName, IC_REDRAW_HILITE);
			if(rb.resval.rint)
				SDS_AddGripNodes(pCurDoc, EntName, 1);
		}
//4M Item:28->
	 SDS_DrawGripNodes(pCurDoc);
//<-4M Item:28
	}

	out:
	sds_ssfree(SDS_DraggingSS);
	SDS_EventMask(SDS_EVM_KEYCHAR|SDS_EVM_MOUSEBUTTON|SDS_EVM_MENUCOMMAND|SDS_EVM_POPUPCOMMAND);
}


