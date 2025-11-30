/* F:\BLD\PRJ\ICAD\ICADDOC.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

//////////////////////////////////////////////////////////////////////
// Icad Document class


#include "Icad.h"/*DNT*/
#include "IcadApp.h"/*DNT*/
#include "IcadSrvrItem.h"/*DNT*/
#include "IcadCntrItem.h"/*DNT*/
#include "IcadMenu.h"/*DNT*/
#include "IcadMenuMain.h"/*DNT*/
#include "IcadDispID.h"
#include "IcadVbaWorkspace.h"
#include "cmds.h"/*DNT*/
#include "cmdsLayout.h"/*DNT*/
#include "cmdsViewport.h"/*DNT*/
#include "IcadOleRes.h"/*DNT*/
#include "IcadIpFrame.h"/*DNT*/
#include "IcadToolBarMain.h"/*DNT*/
#include "paths.h"
#include "vxtabrec.h"
#include "IcadView.h"
#include "viewtabrec.h"
#include "vporttabrec.h"
#include "Preferences.h"
#include "objects.h"/*DNT*/// EBATECH(CNBR) 2001.07.28 for OEM App.

#ifndef _COMAddinManager_H
#include "COMAddinManager.h"
#endif

#if defined(USE_SMARTHEAP)
	#include "smrtheap.h"/*DNT*/
#endif

#ifndef BUILD_WITH_VBA
HRESULT ApcFireEvent(IUnknown* punk, REFIID iid, DISPID dispid, DISPPARAMS* pDispParams /*= NULL*/, VARIANT* pVarResult /*= NULL*/);
#endif

extern BOOL cmd_bOpenReadOnly;
extern int SDS_AutoSave;
extern bool g_Vba6Installed;

IMPLEMENT_DYNCREATE(CIcadDoc, COleServerDoc)

BEGIN_MESSAGE_MAP(CIcadDoc, COleServerDoc)
	//{{AFX_MSG_MAP(CIcadDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, COleServerDoc::OnUpdatePasteMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, COleServerDoc::OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, COleServerDoc::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_OLE_EDIT_CONVERT, COleServerDoc::OnEditConvert)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, COleServerDoc::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, COleServerDoc::OnEditLinks)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_FIRST, COleServerDoc::OnUpdateObjectVerbMenu)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////
// CIcadDoc construction/destruction

CIcadDoc::CIcadDoc()
{
	// Use OLE compound files
	EnableCompoundFile();

	m_bCallSSchangeCB = true;
	m_pGripSelection[0]=0L;
	m_pPrevSelection[0]=0L;
	m_pOopsSelection[0]=0L;
	m_pGripSelection[1]=0L;
	m_pPrevSelection[1]=0L;
	m_pOopsSelection[1]=0L;

	// Add one time construction code here.
	m_dbDrawing=NULL;
	m_pIcadMenu=NULL;
	m_pucs_llbeg=m_pucs_llcur=NULL;
	m_RegenListView=NULL;

	m_bInIPResize= FALSE;
	m_sizeDoc.cx = 500;
	m_sizeDoc.cy = 300;
	m_ZoomNum = m_sizeDoc;
	m_ZoomDenom = m_sizeDoc;

	m_hVbaSdsEvent = NULL;
	m_hFile=NULL;
	//*** Reasons for a file being opened read-only.
	m_bOpenReadOnly=FALSE;
	m_bIsReadOnly=FALSE;
	m_bAlreadyOpen=FALSE;

	m_bSaveToStorage=FALSE;

	m_bDocIsClosing=FALSE;
	m_bDidFirstRegen=FALSE;
	m_nFileVersion=IC_ACAD2004;
	m_nFileType=IC_DWG;	
	m_pGpaktxt= new SDS_textdtext_globalpacket;
	memset(m_pGpaktxt,0, sizeof(SDS_textdtext_globalpacket));	
	m_pGpakatt= new SDS_textdtext_globalpacket;
	memset(m_pGpakatt, 0, sizeof(SDS_textdtext_globalpacket));

	m_pViewForRedraw=NULL;
	m_pHandItemForRedraw=NULL;
	m_pDCForRedraw=NULL;
	m_pGviewForRedraw=NULL;
	m_nOpenViews=0;
	m_nLastMsVport=0;
	m_nCvportOnOpen=-1;

	m_rRectang_ang=0.0;
	m_rRectang_thick=0.0;
	m_rRectang_width=0.0;
	m_rRectang_elev=0.0;
	m_rRectang_fill=0.0;
	m_rRectang_chama=0.0;
	m_rRectang_chamb=0.0;

	m_rRectang_mode=0;	//Bit-coded modes for cmd_rectang_mode
	m_DocPalette = NULL;	// EBATECH(CNBR) for OEM App.

	m_bDesignMode = FALSE;
	m_bVbaProjectSaved = FALSE;

	CComObject<CAutoDoc>::CreateInstance(&m_pAutoDoc);
	m_pAutoDoc->Init (this);
	m_pAutoDoc->AddRef();

	EnableAutomation();
	AfxOleLockApp();
}

CIcadDoc::~CIcadDoc()
{
	// Free the Grip nodes.
	sds_ssfree(m_pGripSelection);
	SDS_FreeNodeList(this);
	cmd_ucs_freell(this);
	sds_ssfree(m_pPrevSelection);
	// Free the oops selection set.
sds_ssfree(m_pOopsSelection);

	if(NULL!=m_dbDrawing)
	{
		//*** Clear the clipboard if it contains entities from this drawing
		CIcadApp* pApp = (CIcadApp*)AfxGetApp();
		if(NULL!=pApp && ::OpenClipboard(NULL))
		{
			HANDLE hData = ::GetClipboardData(pApp->m_uIcadClipFormat);
			if(NULL!=hData)
			{
				void* pData = GlobalLock(hData);
				if(NULL!=pData)
				{
					db_drawing* pDbDrawing = (db_drawing*)(*(long*)(((char*)pData)+(sizeof(long)*2)));
					long procid = GetCurrentProcessId();
					long clipprocid = ((long*)pData)[3];
					GlobalUnlock(hData);
					//ensure that we are clearing the clipboard only if it is our same process, and drawing.
					if ((procid == clipprocid) && (m_dbDrawing==pDbDrawing))
					{
//						::SetClipboardData(pApp->m_uIcadClipFormat,NULL);
						::EmptyClipboard();
					}
				}
			}
			::CloseClipboard();
		}
		//*** Delete the db_drawing.
		delete m_dbDrawing;
		m_dbDrawing=NULL;
	}
	delete m_pGpaktxt;  m_pGpaktxt=NULL;
	delete m_pGpakatt;  m_pGpakatt=NULL;
	// EBATECH(CNBR) ]- 2001.07.28 for OEM App.
	if( m_DocPalette ){
		delete [] m_DocPalette;
		m_DocPalette = NULL;
	}
	// EBATECH(CNBR) ]- 2001.07.28 for OEM App.

	SDS_CMainWindow->m_pCurDoc=NULL;

	if(SDS_CMainWindow->m_nOpenViews && !SDS_CMainWindow->m_nIsClosing &&
		NULL!=SDS_CMainWindow->m_pCurView &&
		::IsWindow(SDS_CMainWindow->m_pCurView->GetSafeHwnd()))
	{
		SDS_CMainWindow->m_pCurDoc=SDS_CMainWindow->m_pCurView->GetDocument();
	}

	AfxOleUnlockApp();

	UnlockDocument();

	if (m_hVbaSdsEvent)
		::CloseHandle (m_hVbaSdsEvent);
	m_hVbaSdsEvent = NULL;

	if(NULL!=m_pIcadMenu)
	{
		m_pIcadMenu->Release();
	}
	CMainWindow* pMain = SDS_CMainWindow;
	if(NULL!=pMain && pMain->m_nOpenViews==0 && pMain->m_nIsClosing==0)
	{
		//*** Update the menus and toolbars (after the release above).
		pMain->IcadWndAction(ICAD_WNDACTION_NEWMENUS); //*** Moved this to the destructor of CIcadDoc.
	}

#if defined(USE_SMARTHEAP)
	MEM_POOL_INFO info;
	MEM_POOL_STATUS status;
	status = MemPoolFirst(&info,1);
	while (status==MEM_POOL_OK) {
		MemPoolShrink(info.pool);
		status=MemPoolNext(&info,1);
	}
#endif

	m_pAutoDoc->Release();

}


void CIcadDoc::OnSetItemRects(LPCRECT lpPosRect, LPCRECT lpClipRect)
{
	// call base class to change the size of the window
	if (g_Vba6Installed)
		CApcDocument<CIcadDoc,COleServerDoc>::OnSetItemRects(lpPosRect, lpClipRect);
	else
		COleServerDoc::OnSetItemRects(lpPosRect, lpClipRect);

	// save our zoom factor
	GetZoomFactor(&m_ZoomNum,&m_ZoomDenom);
}

BOOL CIcadDoc::SaveModified(void)
{
	return SaveModified( TRUE );
}

BOOL CIcadDoc::SaveModified( BOOL bAskForSave )
{
	BOOL bResult = TRUE;
	//*** From COleServerDoc::SaveModified().
	if (m_lpClientSite != NULL)
	{
		if (m_pInPlaceFrame == NULL)
		{
			if (!IsModified())
				return TRUE;// ok to continue

			if( bAskForSave )
			{
				// get name/title of document
				CString name;
				if (m_strPathName.IsEmpty())
				{
					// get name based on caption
					name = m_strTitle;
					if (name.IsEmpty())
						VERIFY(name.LoadString(AFX_IDS_UNTITLED));
				}
				else
				{
					// get name based on file title of path name
					name = m_strPathName;
					if (afxData.bMarked4)
					{
						AfxGetFileTitle(m_strPathName, name.GetBuffer(_MAX_PATH), _MAX_PATH);
						name.ReleaseBuffer();
					}
				}

				CString prompt;
				AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, name);
				switch (AfxMessageBox(prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE))
				{
				case IDCANCEL:
					return FALSE;   // don't continue
				case IDNO:
					return TRUE;		// If not saving changes, revert the document
				case IDYES:
					break;				// If so, either Save or Update, as appropriate

				default:
					ASSERT(FALSE);
					break;
				}
			} //if( bAskForSave )

			UpdateModifiedFlag();   // check for modified items
			OnUpdateDocument();
		}
	}
	else
	{
		//*** From COleDocument::SaveModified().
		// determine if necessary to discard changes
		if (::InSendMessage())
		{
			POSITION pos = GetStartPosition();
			COleClientItem* pItem;
			while ((pItem = GetNextClientItem(pos)) != NULL)
			{
				ASSERT(pItem->m_lpObject != NULL);
				SCODE sc = pItem->m_lpObject->IsUpToDate();
				if (sc != OLE_E_NOTRUNNING && FAILED(sc))
				{
					// inside inter-app SendMessage limits the user's choices
					CString name = m_strPathName;
					if (name.IsEmpty())
						VERIFY(name.LoadString(AFX_IDS_UNTITLED));

					CString prompt;
					prompt.Format(ResourceString(IDC_ICADDOC_ACTIVEX_OBJECTS__0, "ActiveX objects cannot be saved while exiting IntelliCAD.\nDiscard all changes to %s?" ),name);
					return AfxMessageBox(prompt, MB_OKCANCEL|MB_DEFBUTTON2,
						AFX_IDP_ASK_TO_DISCARD) == IDOK;
				}
			}
		}
		// sometimes items change without a notification, so we have to
		//  update the document's modified flag before calling
		//  CDocument::SaveModified.
		//*** Commented this out because linked items were always setting the modified
		//*** flag when a drawing is opened.
//		UpdateModifiedFlag();
		bResult = CDocument::SaveModified();
		if(!bResult)//don't do anything when cancelled
			return bResult;
	}

	//this causes the metafile to be updated with any changes
	UpdateAllItems(NULL, NULL, NULL, DVASPECT_CONTENT);

	struct resbuf setgetrb;
	//*** Set DBMOD to zero to indicate that nothing has been changed in the drawing database.
	setgetrb.restype=RTSHORT;
	setgetrb.resval.rint=0;
	SDS_setvar(NULL,DB_QDBMOD,&setgetrb,m_dbDrawing,NULL,NULL,0);
	SetModifiedFlag(FALSE);

	return bResult;
}

BOOL CIcadDoc::IsModified(void)
{
	//The VBA APC code overrides IsModified and the APC project is set dirty on startup.
	if (g_Vba6Installed)
		return CApcDocument<CIcadDoc,COleServerDoc>::IsModified();
	else
		return COleServerDoc::IsModified();
}

void CIcadDoc::OnCloseDocument(void)
{
	m_bDocIsClosing=TRUE;
	if(!IsEmbedded())
	{
		//*** Update the most recent used file list in the menus
		CMainWindow* pMain = SDS_CMainWindow;
		if(NULL!=pMain && NULL!=pMain->m_pMenuMain)
		{
			BOOL bSetCurrent = TRUE;
			if(pMain->m_nIsClosing)
				bSetCurrent=FALSE;
			pMain->m_pMenuMain->UpdateMRU(bSetCurrent);
		}
	}

	// Fire the VBA Event - before the SDS event
	if (g_Vba6Installed)
	{
	FireEvent(DISPID_DOC_CLOSE);
	GetApp()->FireWorkspaceEvent (DISPID_WS_CLOSEDOC, this);
	}

	// If the file has not yet been saved, get the name from the Title (Drawing1) instead of
	// the pathname
	//
	CString strFileName( GetPathName() );
	if ( strFileName.IsEmpty() )
		{
		strFileName = GetTitle();
		}
		SDS_CallUserCallbackFunc(SDS_CBCLOSEDOC,(void *)(char *)(LPCTSTR)strFileName,NULL,NULL);


	// Don't cancel command when exiting the whole app
	//
	//	This fix to 48714 breaks all scripts with a close command. (56414)
	//	48714 is minor in comparison
	//	if( !SDS_CMainWindow->m_nIsClosing	)
	//		{
	//		SDS_SendMessage(WM_CHAR,27,0);
	//		}

	if (g_Vba6Installed)
	{
		CApcDocument<CIcadDoc,COleServerDoc>::OnCloseDocument();
	}
	else
	{
		COleServerDoc::OnCloseDocument();
	}
	return;
}

BOOL CIcadDoc::OnNewDocument(void)
{
	resbuf rb,plrb;
	bool convertpl;

	SDS_getvar(NULL,DB_QBASEFILE,&rb,m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
	SDS_getvar(NULL,DB_QPLINETYPE,&plrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (plrb.resval.rint==2)
		convertpl=true;
	else
		convertpl=false;

	char fname[IC_ACADBUF];
	BeginWaitCursor();
	// The case of using BASEFILE to create new drawing.
	if(
			(sds_findfile( rb.resval.rstring, fname) EQ RTNORM )
			|| SearchDrawingFile( rb.resval.rstring,fname)
			) {
		int fi1;
		if(IsEmbedded() && NULL!=m_dbDrawing)
		{
			delete m_dbDrawing;
			m_dbDrawing=NULL;
		}
		m_dbDrawing=SDS_CMainWindow->m_pFileIO.drw_openfile(NULL,fname,&fi1,convertpl,(!IsEmbedded()));
		if(m_dbDrawing==NULL)
		{
			EndWaitCursor();
			IC_FREE(rb.resval.rstring);
			return(0);
		}

		// opening the file puts the name of the prototype drawing into the pathname field
		// this clears it.
		//
		m_dbDrawing->ClearPathName();

		IC_FREE(rb.resval.rstring);
	} else {
	// Otherwise, Creating completry new drawing.
		IC_FREE(rb.resval.rstring);
		if(IsEmbedded() && NULL!=m_dbDrawing)
		{
			delete m_dbDrawing;
			m_dbDrawing=NULL;
		}
		int measureinit = DB_ENGLISH_MEASUREINIT;
		int pstylepolicy = DB_BYCOL_PSTYLEMODE;
		if( SDS_getvar(NULL,DB_QMEASUREINIT,&rb,NULL,&SDS_CURCFG,&SDS_CURSES) == RTNORM )
			measureinit = rb.resval.rint;
		if( SDS_getvar(NULL,DB_QPSTYLEPOLICY,&rb,NULL,&SDS_CURCFG,&SDS_CURSES) == RTNORM )
			pstylepolicy = rb.resval.rint;
		m_dbDrawing=new db_drawing(measureinit, pstylepolicy);
		//EBATECH(CNBR) ]-
		/*D.G.*/// Set OSMODE from registry.
		// DP: Now we save and load this variable together with others.
		/*
		rb.restype = RTSHORT;
		rb.resval.rint = UserPreference::OSModePreference;
		SDS_setvar(NULL, DB_QOSMODE, &rb, m_dbDrawing, &SDS_CURCFG, &SDS_CURSES, 0);
		*/
//		rb.restype=RTSTR;
//		rb.resval.rstring="21";
//		SDS_setvar(NULL,DB_QHANDSEED,&rb,m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);

	}

	createDocPalette();	// EBATECH(CNBR) 2001.07.28 for OEM App.

	if(m_dbDrawing->ret_imagebufbmp()) {		
		delete [] m_dbDrawing->ret_imagebufbmp();
		m_dbDrawing->set_imagebufbmp(NULL,0);
	}

	SDS_getvar(NULL,DB_QDATE,&rb,m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
	SDS_setvar(NULL,DB_QTDCREATE,&rb,m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);
	SDS_setvar(NULL,DB_QTDUPDATE,&rb,m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);
	TIME_ZONE_INFORMATION TimeZoneInformation;
	if( GetTimeZoneInformation(&TimeZoneInformation) != TIME_ZONE_ID_INVALID ){
		// Bias is in minutes. Time real 1 means one day.
		rb.resval.rreal += ((sds_real)TimeZoneInformation.Bias / 1440.0);
	}
	SDS_setvar(NULL,DB_QTDUUPDATE,&rb,m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);


	CIcadView *pView=NULL;
	POSITION pos=GetFirstViewPosition();
	if(NULL!=pos)
	{
		pView=(CIcadView *)GetNextView(pos);
		ASSERT_KINDOF(CIcadView, pView);
	}
	if(NULL!=pView)
	{
		if(!IsEmbedded())
		{
			SDS_OnFirstView(pView,1);
		}
		else
		{
			if(pView!=NULL && pView->m_pViewsDoc!=NULL)
			{
				db_drawing *flp=pView->m_pViewsDoc->m_dbDrawing;
				if(flp!=NULL)
				{
					struct resbuf tilemode;
					SDS_getvar(NULL,DB_QTILEMODE,&tilemode,((CIcadDoc*)pView->GetDocument())->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);

			//		if(UseFirstView) {
						if(!tilemode.resval.rint) {
							// Note: Don't set the value of the second parameter to 0 because 
							// VX table can contain the deleted records(marked as deleted) after a viewport
							// creation is UNDOne, Setting the second parameter to 0 will cause those records
							// to be counted. SWH, 30/9/2004
							if((int)flp->ret_ntabrecs(DB_VXTAB,1)>0) {
								struct resbuf cvport;
								SDS_getvar(NULL,DB_QCVPORT,&cvport,((CIcadDoc*)pView->GetDocument())->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
								if (cvport.resval.rint<=1 || cvport.resval.rint>flp->ret_ntabrecs(DB_VXTAB,1)) {
									pView->m_pVportTabHip=flp->tblnext(DB_VXTAB,1);
									cvport.resval.rint=1;
									SDS_setvar(NULL,DB_QCVPORT,&cvport,((CIcadDoc*)pView->GetDocument())->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);
								} else {
									int ct;
									for(ct=1,pView->m_pVportTabHip=flp->tblnext(DB_VXTAB,1); ct!=cvport.resval.rint; ct++,pView->m_pVportTabHip=flp->tblnext(DB_VXTAB,0));
								}
								pView->m_pVportTabHip=((db_vxtabrec *)pView->m_pVportTabHip)->ret_vpehip();
							}
							else
							{
								pView->m_pVportTabHip = cmd_createVIEWPORT(flp, NULL, NULL);
	//							goto makenew;
							}
						} else {
							if((int)flp->ret_ntabrecs(DB_VPORTTAB,0)>0) {
								pView->m_pVportTabHip=flp->tblsearch(DB_VPORTTAB,ResourceString(IDC_ICADDOC__ACTIVE_2, "*ACTIVE" ),1);
							}
							else
							{
								pView->m_pVportTabHip = cmd_createVPORT(flp);
	//							goto makenew;
							}
						}
			/*
					} else {
						if(SDS_CMainWindow->m_pViewTabEntryHip==NULL) {
							makenew: ;
							pView->m_pVportTabHip=SDS_MakeNewVportTab(-1,flp,NULL,NULL);
						} else {
							pView->m_pVportTabHip=SDS_CMainWindow->m_pViewTabEntryHip;
							SDS_CMainWindow->m_pViewTabEntryHip=NULL;
						}
					}
			*/
					// Make sure all the setvars are current from the table. (only if in PS/MS)
					if(!tilemode.resval.rint) SDS_TabToVar(flp,pView->m_pVportTabHip);

					struct gr_viewvars viewvarsp;
					SDS_ViewToVars2Use(pView,flp,&viewvarsp);
					gr_initview(flp,&SDS_CURCFG,&pView->m_pCurDwgView,&viewvarsp);
				}
			}
		}
	}

	//*** Set DWGTITLED to zero to indicate that the drawing is not named.
	rb.restype=RTSHORT;
	rb.resval.rint=0;
	SDS_setvar(NULL,DB_QDWGTITLED,&rb,m_dbDrawing,NULL,NULL,0);


	rb.restype=RTSTR;
	rb.resval.rstring=(char *)(LPCTSTR)GetTitle();
	SDS_setvar(NULL,DB_QDWGNAME,&rb,m_dbDrawing,NULL,NULL,0);

	cmd_setUpCTAB(); // EBATECH(CNBR) 2003/8/10 Sync CTAB

	if (g_Vba6Installed)
	{
 		if(!CApcDocument<CIcadDoc,COleServerDoc>::OnNewDocument())
		{
			delete m_dbDrawing;
			m_dbDrawing=NULL;
			EndWaitCursor();
			return 0;
		}
	}
	else
	{
 		if(!COleServerDoc::OnNewDocument())
		{
			delete m_dbDrawing;
			m_dbDrawing=NULL;
			EndWaitCursor();
			return 0;
		}
	} //endif g_Vba6Installed


	if (g_Vba6Installed)
	{
		// Name the Project
		CString caption;
		CString title = GetTitle();
		int n = title.Find('.');
		if (n==-1)
			caption = title;
		else
			caption = title.Left(n);
		VERIFY(SUCCEEDED(ApcProject->APC_PUT(Name)(CComBSTR(caption))));

		// initialize design mode //DO-DOC
		//VERIFY(SUCCEEDED(ApcProject->APC_PUT(Mode)(axModeDesign)));

		// Create the "ThisDocument" Project Item
		HRESULT hr = DefineProjectItem();
		if (FAILED(hr))
			return FALSE;

		// Add a Reference to the CommonProjects project
		CComQIPtr<SUMMIT::IActProjectStorage, &__uuidof(SUMMIT::IActProjectStorage)> spLibrary(GetApp()->m_spWorkspace);
		if (spLibrary)
			{
			hr = spLibrary->ReferenceToSelf(ApcProject);
			ASSERT(SUCCEEDED(hr));
			}

		ApcProject->APC_PUT(Dirty)(VARIANT_FALSE);


	} //endif g_Vba6Installed

	// Fire the IWorkspaceEvents::New(IIcadDoc*) event, but only if either SHIFT key was not pressed
	// when the current message (File | Open) was initiated.  Note that this is using
	// GetKeyState instead of GetAsyncKeyState as was used in CIcadApp::InitInstance.
	//
	SHORT shiftState = GetKeyState(VK_SHIFT) & ~0x0001; // evaluates to 1 if pressed
	bool fEnableStartupMacros = (shiftState == 0);
	if (fEnableStartupMacros)
		GetApp()->FireWorkspaceEvent (DISPID_WS_NEWDOC, this);

	// Call the NEWDOC callback with the name of the new drawing in the first parameter
	//
	SDS_CallUserCallbackFunc(SDS_CBNEWDOC,(void *)rb.resval.rstring,NULL,NULL);

	SDS_CMainWindow->DocChangeCallback( pView );

	EndWaitCursor();
	return 1;
}

BOOL CIcadDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	resbuf plrb;
	bool convertpl;

	SDS_getvar(NULL,DB_QPLINETYPE,&plrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (plrb.resval.rint==2)
		convertpl=true;
	else
		convertpl=false;

	if(NULL==lpszPathName)
	{
		return FALSE;
	}
	// Read the drawing into memory
	int fi1;
	BeginWaitCursor();
	if(IsEmbedded() && NULL!=m_dbDrawing)
	{
		delete m_dbDrawing;
		m_dbDrawing=NULL;
	}

	if(NULL==(m_dbDrawing=SDS_CMainWindow->m_pFileIO.drw_openfile(NULL,(char *)lpszPathName,&fi1,convertpl,(!IsEmbedded()), SDS_CMainWindow->m_bIsRecovering)))
	{
		EndWaitCursor();
		return FALSE;
	}

	m_nFileVersion = m_dbDrawing->ret_FileVersion();
	SDS_CURDOC->m_nFileVersion = m_nFileVersion;

	// Set DWGTITLED to nonzero to indicate that the drawing is named.
	struct resbuf rb;
	rb.restype=RTSHORT;
	rb.resval.rint=1;
	SDS_setvar(NULL,DB_QDWGTITLED,&rb,m_dbDrawing,NULL,NULL,0);

	createDocPalette();	// EBATECH(CNBR) 2001.07.28 for OEM App.

	CIcadView *pView=NULL;
	POSITION pos=GetFirstViewPosition();
	if(NULL!=pos)
	{
		pView=(CIcadView *)GetNextView(pos);
		ASSERT_KINDOF(CIcadView, pView);
	}
	if(NULL!=pView)
	{
		if(!IsEmbedded())
		{
			SDS_OnFirstView(pView,1);
		}
		else
		{
			if(pView!=NULL && pView->m_pViewsDoc!=NULL)
			{
				db_drawing *flp=pView->m_pViewsDoc->m_dbDrawing;
				if(flp!=NULL)
				{
					struct resbuf tilemode;
					SDS_getvar(NULL,DB_QTILEMODE,&tilemode,((CIcadDoc*)pView->GetDocument())->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);

			//		if(UseFirstView) {
						if(!tilemode.resval.rint) {
							// Note: Don't set the value of the second parameter to 0 because 
							// VX table can contain the deleted records(marked as deleted) after a viewport
							// creation is UNDOne, Setting the second parameter to 0 will cause those records
							// to be counted. SWH, 30/9/2004
							if((int)flp->ret_ntabrecs(DB_VXTAB,1)>0) {
								struct resbuf cvport;
								SDS_getvar(NULL,DB_QCVPORT,&cvport,((CIcadDoc*)pView->GetDocument())->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
								if(cvport.resval.rint!=1) {
									m_nCvportOnOpen=cvport.resval.rint;
									cvport.resval.rint=1;
									SDS_setvar(NULL,DB_QCVPORT,&cvport,((CIcadDoc*)pView->GetDocument())->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);
								}
								pView->m_pVportTabHip=flp->tblnext(DB_VXTAB,1);
								ASSERT(pView->m_pVportTabHip != NULL);
								pView->m_pVportTabHip=((db_vxtabrec *)pView->m_pVportTabHip)->ret_vpehip();
							}
							else
							{
								pView->m_pVportTabHip = cmd_createVIEWPORT(flp, NULL, NULL);
	//							goto makenew;
							}
						} else {
							if((int)flp->ret_ntabrecs(DB_VPORTTAB,0)>0) {
								pView->m_pVportTabHip=flp->tblsearch(DB_VPORTTAB,ResourceString(IDC_ICADDOC__ACTIVE_2, "*ACTIVE" ),1);
							}
							else
							{
								pView->m_pVportTabHip = cmd_createVPORT(flp);
	//							goto makenew;
							}
						}
			/*
					} else {
						if(SDS_CMainWindow->m_pViewTabEntryHip==NULL) {
							makenew: ;
							pView->m_pVportTabHip=SDS_MakeNewVportTab(-1,flp,NULL,NULL);
						} else {
							pView->m_pVportTabHip=SDS_CMainWindow->m_pViewTabEntryHip;
							SDS_CMainWindow->m_pViewTabEntryHip=NULL;
						}
					}
			*/
					// Make sure all the setvars are current from the table. (only if in PS/MS)
					if(!tilemode.resval.rint) SDS_TabToVar(flp,pView->m_pVportTabHip);

					struct gr_viewvars viewvarsp;
					SDS_ViewToVars2Use(pView,flp,&viewvarsp);
					gr_initview(flp,&SDS_CURCFG,&pView->m_pCurDwgView,&viewvarsp);
				}
			}
		}
	}

	if(IsEmbedded())
	{
		EndWaitCursor();
		return TRUE;
	}

	//*** Lock the document so other applications cannot access it.
	if(!LockDocument(lpszPathName))
	{
		EndWaitCursor();
		sds_alert(ResourceString(IDC_ICADDOC_ERROR__DOCUMENT__3, "ERROR: Document could not be locked." ));
		return FALSE;
	}

	if((NULL != pView) &&
		(this->m_nCvportOnOpen!=-1))
		{
		db_handitem *hip;
		int fi1;
		db_drawing *flp=pView->m_pViewsDoc->m_dbDrawing;

		for(hip=SDS_CURDWG->tblnext(DB_VXTAB,1); hip!=NULL; hip=SDS_CURDWG->tblnext(DB_VXTAB,0))
			{
			((db_vxtabrec *)hip)->ret_vpehip()->get_69(&fi1);
			if(fi1==this->m_nCvportOnOpen)
				{
				cmd_MakeVpActive(pView,((db_vxtabrec *)hip)->ret_vpehip(),flp);
				break;
				}
			}
		this->m_nCvportOnOpen=-1;
		}

	if(m_bAlreadyOpen || (m_bIsReadOnly && !m_bOpenReadOnly))
	{
		CString str;
		if(m_bAlreadyOpen)
		{
			str.Format(ResourceString(IDC_ICADDOC_THIS_FILE_IS_ALR_5, "%s...\nThis file is already in use.  Do you want to open as read-only?" ),lpszPathName);
		}
		else if(m_bIsReadOnly)
		{
			str.Format(ResourceString(IDC_ICADDOC_THIS_FILE_IS_REA_6, "%s...\nThis file is read-only.  Do you want to continue?" ),lpszPathName);
		}
		HWND hWnd = SDS_CMainWindow->GetSafeHwnd();
		if(IDYES!=::MessageBox(hWnd,str,AfxGetAppName(),MB_YESNO))
		{
			EndWaitCursor();
			UnlockDocument();
			return FALSE;
		}
	}

	//*** Try to create the menu specified in MENUNAME
	CMainWindow* pMain = SDS_CMainWindow;
	SDS_getvar(NULL,DB_QAUTOMENULOAD,&rb,m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint!=0 &&
		RTNORM==SDS_getvar(NULL,DB_QMENUNAME,&rb,m_dbDrawing,&SDS_CURCFG,&SDS_CURSES))
	{
		char fname[IC_ACADBUF];
		strncpy(fname,rb.resval.rstring,sizeof(fname)-1);
		delete [] rb.resval.rstring;
		if(NULL!=pMain && NULL!=pMain->m_pMenuMain && *fname!='.'/*DNT*/)
		{
			char* pFileName;
			if(NULL==(pFileName=strrchr(fname,'\\'/*DNT*/)))
			{
				pFileName=fname;
			}
			if(NULL==strrchr(pFileName,'.'/*DNT*/))
			{
				ic_setext(fname,"mnu"/*DNT*/);
			}
			if( SearchDrawingFile( fname, fname) )
			{
				//*** Check to see if a menu with this name has already
				//*** been loaded.
				if(NULL==(m_pIcadMenu=pMain->m_pMenuMain->IsLoaded(fname)))
				{
					if(pMain->m_pMenuMain->LoadMenu(fname))
					{
						m_pIcadMenu=pMain->m_pMenuMain->GetCurrentMenu();
					}
				}
				else
				{
					m_pIcadMenu->AddRef();
				}
			}
		}
	}
	cmd_setUpCTAB(); // EBATECH(CNBR) 2003/8/10

	if (g_Vba6Installed)
	{
		//Load the VBA project if any
		char *vbaName = NULL;
		if((vbaName = new char[strlen(lpszPathName)+1])==NULL)
			return FALSE;
		strcpy(vbaName, (LPCSTR)lpszPathName);
		ic_setext(vbaName,"vbi"/*DNT*/);
		if(0==access(vbaName,00))
 		{
			SetPathName (vbaName, FALSE);
			if(!CApcDocument<CIcadDoc,COleServerDoc>::OnOpenDocument(vbaName))
			{
				//failed to open VBA project
				ASSERT("Failed to open the VBA project");
			}
			SetPathName (lpszPathName, FALSE);
			m_bVbaProjectSaved = TRUE;
		}
		else
		{
 			if(!CApcDocument<CIcadDoc,COleServerDoc>::OnNewDocument())
			{
				delete m_dbDrawing;
				m_dbDrawing=NULL;
				EndWaitCursor();
				delete [] vbaName;
				return 0;
			}

			// initialize design mode //DO-DOC
			//VERIFY(SUCCEEDED(ApcProject->APC_PUT(Mode)(axModeDesign)));

			// Create the "ThisDocument" Project Item
			HRESULT hr = DefineProjectItem();

			if (FAILED(hr))
			{
				delete [] vbaName;
				return FALSE;
			}

			// Add a Reference to the CommonProjects project
			CComQIPtr<SUMMIT::IActProjectStorage, &__uuidof(SUMMIT::IActProjectStorage)> spLibrary(GetApp()->m_spWorkspace);
			if (spLibrary)
				{
				hr = spLibrary->ReferenceToSelf(ApcProject);
				ASSERT(SUCCEEDED(hr));
				}

			m_bVbaProjectSaved = FALSE;

			ApcProject->APC_PUT(Dirty)(VARIANT_FALSE);
		}
		delete [] vbaName;

	} //endif g_Vba6Installed

	// fire IIcadDocEvents::Open event, but only if either SHIFT key was not pressed
	// when the current message (File | Open) was initiated.  Note that this is using
	// GetKeyState instead of GetAsyncKeyState as was used in CIcadApp::InitInstance.
	//
	SHORT shiftState = GetKeyState(VK_SHIFT) & ~0x0001; // evaluates to 1 if pressed
	bool fEnableStartupMacros = (shiftState == 0);
	//
	// Note: Sometimes, depending on the implementation, it makes sense to defer
	// the Document_Open event until the first window draw (typically CView::OnDraw, or
	// CView::OnInitialUpdate).  Deferment is not required by this sample.
	// The deciding factor has to do with the state of the document at this point in
	// the call sequence.  Realize that VBA script written for this event will expect
	// the document to be fully initialized.  If there is additional initialization code
	// that takes place, the event should be deferred until then.
	//
	if (fEnableStartupMacros)
		GetApp()->FireWorkspaceEvent (DISPID_WS_OPENDOC, this);


	//this callback has been moved to CIcadApp::FileOpen. When opening a file with VBA macros at medium security
	//VBA brings up a dialog that uses the Active window as its parent.  The active window is still the old document
	//hence it gets a paint message and SDS_CURDOC gets switched back to the old document.  Moving this callback to
	//the CIcadApp::FileOpen ensures that the document is open and the window is activated (i.e. SDS_CURDOC is correct).
	//SDS_CallUserCallbackFunc(SDS_CBOPENDOC,(void *)(char *)lpszPathName,NULL,NULL);

	EndWaitCursor();
	return 1;
}

BOOL CIcadDoc::LockDocument(LPCTSTR lpszPathName)
{
	m_bOpenReadOnly=m_bIsReadOnly=m_bAlreadyOpen=FALSE;
	DWORD dwAccess=GENERIC_READ;
	DWORD dwShare=FILE_SHARE_READ;
	DWORD dwAttrib=GetFileAttributes(lpszPathName);
	if(dwAttrib & FILE_ATTRIBUTE_READONLY)
	{
		m_bIsReadOnly=TRUE;
		if(cmd_bOpenReadOnly) {
			m_bOpenReadOnly=TRUE;
			dwShare|=FILE_SHARE_WRITE;
		}
	}
	else if(cmd_bOpenReadOnly)
	{
		m_bOpenReadOnly=TRUE;
		dwShare|=FILE_SHARE_WRITE;
	}
	else if(!m_bIsReadOnly)
	{
		dwAccess|=GENERIC_WRITE;
	}
	//*** Open the file so another application cannot write to it.
	m_hFile = ::CreateFile(lpszPathName
		,dwAccess						//*** Read-write access.
		,dwShare						//*** Don't allow sharing.
		,NULL							//*** No security attributes.
		,OPEN_EXISTING					//*** We are opening an existing file.
		,0								//*** No flags
		,NULL);							//*** No template

	if(INVALID_HANDLE_VALUE==m_hFile)
	{
		m_bAlreadyOpen=TRUE;
	}
	else if(NULL==m_hFile)
	{
		return FALSE;
	}
	return TRUE;
}

void CIcadDoc::UnlockDocument()
{
	//*** Release the handle to the file.
	if(NULL!=m_hFile && INVALID_HANDLE_VALUE!=m_hFile)
	{
		if(!::CloseHandle(m_hFile))
		{
			DWORD dwErr = GetLastError();
			int i=0;
		}
		else
		{
			m_hFile=NULL;
		}
	}
}
//Bugzilla No 78255 05-08-2002
void CIcadDoc:: StoreOleClientItem(CObList*& plist )
{
	COleClientItem * pItem	= NULL;	
	POSITION pos = GetStartPosition();
	if ( pos )
	{
		plist = new CObList;
		while ((pItem = GetNextClientItem(pos)) != NULL)
		{	
			plist->AddHead( pItem );
			RemoveItem( pItem );
		}
	}
}
//Bugzilla No 78255 05-08-2002
void CIcadDoc:: ReStoreOleClientItem(CObList *& plist )
{
	if ( plist != NULL )
	{
		COleClientItem * pItem = NULL;
		for( POSITION pos = plist->GetHeadPosition(); pos != NULL; plist->GetNext( pos ))
		{
			pItem = (COleClientItem* )plist->GetAt(pos);
			if ( pItem != NULL )
				AddItem( pItem );
		}
		delete plist;
	}
}

BOOL CIcadDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	BOOL bRet=FALSE;
	char fext[10],*fname=NULL, *vbaName=NULL;
	POSITION pos;
	ic_getext((char*)lpszPathName,fext);

	CMainWindow* pMain = SDS_CMainWindow;

	UnlockDocument();

	BOOL bSaveAs = FALSE;

	// If the file has not yet been saved, get the name from the Title (Drawing1) instead of
	// the pathname
	//
	CString strOldFileName( GetPathName() );
	if ( strOldFileName.IsEmpty() )
		{
		strOldFileName = GetTitle();
		}
	CString strName(GetPathName());
	CString str(fext);


if(str.Compare("."/*DNT*/)==0)
	{
		//*** No extension, assume qsave.
		CString str1(lpszPathName);
		// +5 for the extension.
		if((fname = new char[str1.GetLength()+5])==NULL) goto out;
		lstrcpy(fname,str1);
		str=".dwg"/*DNT*/;
		ic_setext(fname,"dwg"/*DNT*/);
	}
	else
	{
		if((fname = new char[strlen(lpszPathName)+1])==NULL) goto out;
		strcpy(fname,lpszPathName);
	}

	if (SDS_AutoSave == 1 || SDS_AutoSave == 99)	// Prevent autosave and qsave from changing the drawing version.
	{
		m_nFileVersion = m_dbDrawing->ret_FileVersion();
		if (SDS_AutoSave == 99)	// Temporary flag to let me know we are qsave'g.
			SDS_AutoSave = 0;	// Reset it so we don't think it is an autodave.

	}

	/*DG - 14.5.2003*/// Don't show messages if EXPERT > 2.
	resbuf	expertRb;
	if(SDS_getvar(NULL, DB_QEXPERT, &expertRb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) != RTNORM)
		expertRb.resval.rint = 0;	// on error, cause message to be displayed
	if(expertRb.resval.rint <= 2)
	{
		if( m_dbDrawing->WasLoadedFromA2K() && m_nFileVersion < IC_ACAD2000 &&
			(!str.CompareNoCase(".dwg"/*DNT*/) || !str.CompareNoCase(".dxf"/*DNT*/) || !str.CompareNoCase(".dwt"/*DNT*/)) )
		{
			resbuf	filediaRb;
			if(SDS_getvar(NULL, DB_QFILEDIA, &filediaRb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) != RTNORM)
				filediaRb.resval.rint = 1;	// on error, cause message box to be displayed

			// EBATECH[2001/9/20 localize
			if(filediaRb.resval.rint)		// FILEDIA == 1 - On; display message in message box
			{
				if(MessageBox(GetActiveWindow(),
							  ResourceString(IDC_ICADSAVE_PRER2K_DLG, "Pre-2K file formats do not support some entities.\nThese entities will not be saved in the drawing.\nContinue?"),
							  ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD"),
							  MB_OKCANCEL | MB_SETFOREGROUND) != IDOK)
				{
					bRet = TRUE;
					goto out;
				}
			}
			else							// FILEDIA == 0 - Off; display message at command line
				sds_printf(ResourceString(IDC_ICADSAVE_PRER2K_CMD, "\nPre-2K file formats do not support some entities.\nThese entities will not be saved in the drawing."));
			// ]EBATECH
		}
		// If we are saving to a pre-R14 version, notify the user that some entities
		// will not be saved, and see if they want to continue.
		else
			if( m_nFileVersion < IC_ACAD14 &&
				(!str.CompareNoCase(".dwg"/*DNT*/) || !str.CompareNoCase(".dxf"/*DNT*/) || !str.CompareNoCase(".dwt"/*DNT*/)) )
			{
				resbuf	filediaRb;
				if(SDS_getvar(NULL, DB_QFILEDIA, &filediaRb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) != RTNORM)
					filediaRb.resval.rint = 1;	// on error, cause message box to be displayed

				if(filediaRb.resval.rint)		// FILEDIA == 1 - On; display message in message box
				{
					if(MessageBox(GetActiveWindow(),
								  ResourceString(IDC_ICADSAVE_PRER14_DLG, "Pre-R14 file formats do not support some entities.\nThese entities will not be saved in the drawing.\nContinue?"),
								  ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD"),
								  MB_OKCANCEL | MB_SETFOREGROUND) != IDOK)
					{
						bRet = TRUE;
						goto out;
					}
				}
				else							// FILEDIA == 0 - Off; display message at command line
					sds_printf(ResourceString(IDC_ICADSAVE_PRER14_CMD, "\nPre-R14 file formats do not support some entities.\nThese entities will not be saved in the drawing."));
			}
	}	// expertRb.resval.rint <= 2


	// Update all of the window positions before we save.
	if(!m_bSaveToStorage)
	{
		pos=GetFirstViewPosition();
		while (pos!=NULL) {
			CIcadView *pView=(CIcadView *)GetNextView(pos);
			ASSERT_KINDOF(CIcadView, pView);

			pView->CalcViewPoints(1);
		}
	}

	//*** If this is a save as, make sure we can save to that file.
	if(!m_bSaveToStorage && !SDS_AutoSave && (strName.IsEmpty() ||
		0!=strName.CompareNoCase(fname)))
	{
		if((-1)==access(fname,00))
		{
			m_bOpenReadOnly=m_bIsReadOnly=m_bAlreadyOpen=FALSE;
		}
		else
		{
			//*** If the file exists, this will setup all the read-only
			//*** variables for this new document name.
			LockDocument(fname);
			UnlockDocument();
		}

		bSaveAs=TRUE;
	}
	//*** Check for read-only.
	if(!m_bSaveToStorage && !SDS_AutoSave && (m_bOpenReadOnly || m_bIsReadOnly ||
		m_bAlreadyOpen))
	{
		if(bSaveAs && m_bAlreadyOpen)
		{
			strName.Format(ResourceString(IDC_ICADDOC_THIS_FILE_IS_AL_11, "%s...\nThis file is already in use.  Please save under a different name." ),lpszPathName);
		}
		else
		{
			strName.Format(ResourceString(IDC_ICADDOC_THIS_FILE_IS_RE_12, "%s...\nThis file is Read-Only.  Please save under a different name." ),lpszPathName);
		}
		sds_alert(strName);
		int nRet = cmd_saveas();
		if(NULL!=fname)
		{
			delete [] fname;
		}
		if(nRet!=RTNORM)
		{
			return FALSE;
		}
		return TRUE;
	}
	//*** Make sure the file is still there if not a saveas.
	if(!m_bSaveToStorage && !bSaveAs && !SDS_AutoSave)
	{
		if((-1)==access(fname,00))
		{
			strName.Format(ResourceString(IDC_ICADDOC_THIS_FILENAME_I_13, "%s...\nThis filename is no longer valid.  Please save under a different name." ),lpszPathName);
			sds_alert(strName);
			int nRet = cmd_saveas();
			if(NULL!=fname)
			{
				delete [] fname;
			}
			if(nRet!=RTNORM)
			{
				return FALSE;
			}
			return TRUE;
		}
	}
	//*** Save
	if((str.CompareNoCase(".dwg"/*DNT*/)==0 ||str.CompareNoCase(".dwt"/*DNT*/)==0) && NULL!=pMain)
	{
		struct resbuf rb,dbmod;
		SDS_getvar(NULL,DB_QDATE,&rb,m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
		SDS_setvar(NULL,DB_QTDUPDATE,&rb,m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);
		TIME_ZONE_INFORMATION TimeZoneInformation;
		if( GetTimeZoneInformation(&TimeZoneInformation) != TIME_ZONE_ID_INVALID ){
			// Bias is in minutes. Time real 1 means one day.
			rb.resval.rreal += ((sds_real)TimeZoneInformation.Bias / 1440.0);
		}
		SDS_setvar(NULL,DB_QTDUUPDATE,&rb,m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);
		SDS_getvar(NULL,DB_QSAVEFILE,&rb,m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);

		if( SDS_AutoSave == 10 )  // reserved for undo fixing
			ic_setext(fname,"tmp");
		else if(SDS_AutoSave && SDS_AutoSave!=99 && SDS_AutoSave!=2 && SDS_AutoSave!=-1) //*** I set it to 2 when creating the temp file name when sending mail, in that case don't change the extension.
		{
			// EBATECH(CNBR) -[ 2002/6/28 SAVEFILEPATH
			resbuf path;
			SDS_getvar(NULL, DB_QSAVEFILEPATH, &path, m_dbDrawing, &SDS_CURCFG, &SDS_CURSES);
			if( path.resval.rstring != NULL && strlen(path.resval.rstring) > 0 && _access(path.resval.rstring, 0) == 0 )
			{
				int len;
				char body[_MAX_FNAME];

				_tsplitpath( lpszPathName, NULL, NULL, body, NULL);
				delete [] fname;
				if((fname = new char[IC_ACADBUF])==NULL) goto out;
				_tcscpy(fname, path.resval.rstring );
				len = _tcslen(fname);
				if( len > 2 && strcmp( &fname[len-2], ":\\" ) != 0 ){
					_tcscat(fname, "\\");
				}
				_tcscat(fname, body );
				_tcscat(fname, rb.resval.rstring ); // ".SV$"
			}
			else
			{
				ic_setext(fname,rb.resval.rstring);
			}
			IC_FREE(path.resval.rstring);
			//ic_setext(fname,rb.resval.rstring);
			// EBATECH(CNBR) ]-
		}
		else if(SDS_AutoSave==-1) //*** I set it to 2 when creating the temp file name when sending mail, in that case don't change the extension.
		{
			ic_setext(fname,"$fe"/*DNT*/);
		}

		// Save a bitmap to the file.
		SDS_getvar(NULL,DB_QDBMOD,&dbmod,m_dbDrawing,NULL,NULL);
		if(dbmod.resval.rint || !m_dbDrawing->ret_imagebufbmp()) {
			char *pBuffer=NULL;
			long  bytes=0L;
			int bmWidth=180;
			int bmHeight=90;

			CBitmap *Bitmap=SDS_DrawBitmap(m_dbDrawing,NULL,NULL,NULL,bmWidth,bmHeight,TRUE);
			if(Bitmap) {
				if(!SDS_SaveBitmapToBuffer(Bitmap,&pBuffer,&bytes,TRUE)) {					
					if(m_dbDrawing->ret_imagebufbmp()) delete [] m_dbDrawing->ret_imagebufbmp();
					m_dbDrawing->set_imagebufbmp(pBuffer,bytes);
				}
				delete Bitmap;
			}
		}


		if (!SDS_AutoSave)	//Don't fire events if we are autosaving
		{
			FireEvent(DISPID_DOC_SAVE);
			GetApp()->FireWorkspaceEvent (DISPID_WS_SAVEDOC, this);
		}

		if (g_Vba6Installed)
		{
			if (!SDS_AutoSave)	//save the VBA project only if we are not in AutoSave....
			{
				if (ApcProject && SDS_CMainWindow->m_pCurDoc->m_bVbaProjectSaved)
				{
					BOOL bSameAsLoad = FALSE;
					bSameAsLoad = AfxComparePath(m_strPathName, fname);

					//if file has never been saved || saving to a new file
					if (strName.IsEmpty() || !bSameAsLoad)
						strName = fname;

					if((vbaName = new char[strName.GetLength()+1])==NULL) goto out;
					strcpy(vbaName, strName);
					ic_setext(vbaName,"vbi"/*DNT*/);

					//this is an MFC hack.  We use MFC to save the VBI file, but not the DWG file.  The DWG file is
					//saved in this function using drw_savefile.  To save the VBI file, MFC needs to know if it is saving
					//to the same file (m_bSameAsLoad).  It does this by comparing the string m_strPathName with the file
					//name passed in (vbaName).  Since the m_strPathName is set to the .dwg extension, we temporarily
					//change it to have the VBI extension.  This way MFC can determine ig the VBI file has been saved.
					CString oldName;
					if (m_bVbaProjectSaved || bSameAsLoad)
					{
						oldName = GetPathName();
						// A.G.
						// This hack is to enable saving VBI file if it's not existed.
						// We suppose there's no file named as _vbiIcadDummy.vbi and
						// use this dummy name
						CFileFind finder;
						if( finder.FindFile( vbaName ) )
							SetPathName (vbaName,FALSE);
						else
							SetPathName( "_vbiIcadDummy.vbi", FALSE );
					}

					//VARIANT_BOOL	IsDirty;					/*D.G.*/
					//ApcProject->APC_GET(Dirty)(&IsDirty);		// Save vbi-file only
					//if(IsDirty == VARIANT_TRUE)					// if we've made something in VB.
					
					//Store oleclient items and remove from oledoc. They are messing up with VBI
					//Bugzilla No 78255 05-08-2002 [
					CObList * pObList = NULL ;
					StoreOleClientItem( pObList );
					CApcDocument<CIcadDoc,COleServerDoc>::OnSaveDocument(vbaName);
					//Restore oleclient items back to oledoc
					ReStoreOleClientItem( pObList );
					//Bugzilla No 78255 05-08-2002 ]

					//now change the path name back to the DWG extension
					if (m_bVbaProjectSaved || bSameAsLoad)
					{
						if (oldName.IsEmpty())
							SetPathName (fname, FALSE);
						else
							SetPathName (oldName, FALSE);
					}
					m_bVbaProjectSaved = TRUE;
					delete [] vbaName;
				}
			}
		} //endif g_Vba6Installed


		// We comment the next line to avoid deadloop in App thread : Andy Graphov
		//SDS_BroadcastRQToSDSApps(SDS_RQSAVE);
		// First parameter is the new name we are going to save to
		// Second parameter is the current name of the file
		// Before 98b release of IntelliCAD, second parameter was always NULL
		//
		if( SDS_AutoSave != 10 )  // reserved for undo saving
			SDS_CallUserCallbackFunc(SDS_CBSAVEDOC,(void *)fname,(void *)(char *)strOldFileName.GetBuffer(0),NULL);

		SDS_getvar(NULL,DB_QRASTERPREVIEW,&dbmod,m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
		bool bSavePreview = dbmod.resval.rint == 1;

		SDS_getvar(NULL,DB_QMAKEBAK,&dbmod,m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
		/*D.G.*/// Encreased dxf dec. precision from 6 to 12.
		bRet=pMain->m_pFileIO.drw_savefile(m_dbDrawing,0L,NULL,fname,IC_DWG,m_nFileVersion,0, 12, 1,1,dbmod.resval.rint,(!m_bSaveToStorage), cmd_bSaveWithPassWord == TRUE, bSavePreview);
		IC_FREE(rb.resval.rstring);
		if(bRet && (m_bSaveToStorage || SDS_AutoSave==1))
		{
			bRet=TRUE;
			goto out;
		}
	}
	else if(str.CompareNoCase(".dxf"/*DNT*/)==0 && NULL!=pMain)
	{
		struct resbuf rb;
		SDS_getvar(NULL,DB_QMAKEBAK,&rb,m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);

		// We comment the next line to avoid deadloop in App thread : Andy Graphov
		//SDS_BroadcastRQToSDSApps(SDS_RQSAVE);

		SDS_CallUserCallbackFunc(SDS_CBSAVEDOC,(void *)fname,(void *)(char *)strOldFileName.GetBuffer(0),NULL);
		/*D.G.*/// Encreased dxf dec. precision from 6 to 12.
		bRet=pMain->m_pFileIO.drw_savefile(m_dbDrawing,0L,NULL,fname,m_nFileType,m_nFileVersion,0, 12, 1,1,rb.resval.rint,TRUE);
		if(bRet) goto out;
	}
	else if(str.CompareNoCase(".sld"/*DNT*/)==0 || str.CompareNoCase(".emf"/*DNT*/)==0 || str.CompareNoCase(".wmf"/*DNT*/)==0)
	{
		cmd_WriteSldofMf(fname);
		bRet=TRUE;
		goto out;
	}
	else if(str.CompareNoCase(".bmp"/*DNT*/)==0)
	{
		char *pBuffer;
		long  bytes;
		if(SDS_CURVIEW && !SDS_CURVIEW->SaveBitmapToBuffer(&pBuffer,&bytes,FALSE)) {
			SDS_SaveBufferToFile(pBuffer,bytes,fname);
			IC_FREE(pBuffer);
		}
		bRet=TRUE;
		goto out;
	}
	else
	{
		sds_alert(ResourceString(IDC_ICADDOC_AN_INVALID_FILE_20, "An invalid file extension was entered." ));
		bRet=FALSE;
		goto out;
	}

	if(!bRet)
	{
		if(strlen(fname)>255)
		{
			sds_alert(ResourceString(IDC_ICADDOC_THE_FILENAME_OR_21, "The filename or extension is too long.  The filename\nand extension cannot exceed 255 characters." ));
			goto out;
		}
		else
		{
			str.Format(ResourceString(IDC_ICADDOC_ERROR_SAVING_TO_23, "Error saving to file \"%s\".\nThe file may be corrupt." ),fname);
			sds_alert(str);
		}
	}
else
	{
		//Notify any clients that this document has been saved
		NotifySaved();

		struct resbuf setgetrb;
		//*** Set DBMOD to zero to indicate that nothing has been changed in the drawing database.
		setgetrb.restype=RTSHORT;
		setgetrb.resval.rint=0;
		SDS_setvar(NULL,DB_QDBMOD,&setgetrb,m_dbDrawing,NULL,NULL,0);
		SetModifiedFlag(FALSE);

		//*** Set DWGTITLED to nonzero to indicate that the drawing is named.
		setgetrb.resval.rint=1;
		SDS_setvar(NULL,DB_QDWGTITLED,&setgetrb,m_dbDrawing,NULL,NULL,0);

		CString str1(fname);
		int idxChar;

		//*** Append the extension and set the title and DWGNAME.
		str1=fname;
		idxChar=str1.ReverseFind('.'/*DNT*/);
		if(idxChar==(-1)) str1+=str;
		SetPathName(str1);

//*** Set DWGNAME system variable.
setgetrb.restype=RTSTR;
		if((setgetrb.resval.rstring = new char [strlen(fname)+1])==NULL)
		{
			delete [] fname;
			goto out;
		}
strcpy(setgetrb.resval.rstring,str1.GetBuffer(0));
SDS_setvar(NULL,DB_QDWGNAME,&setgetrb,m_dbDrawing,NULL,NULL,0);
IC_FREE(setgetrb.resval.rstring);

		//*** Set the title to just the filename (no path)
		idxChar=str1.ReverseFind('\\'/*DNT*/);
		if(idxChar!=(-1))
		{
			str1=str1.Right(str1.GetLength()-(idxChar+1));
		}
		if(str1.GetLength()>30)
		{
			//*** If the string is too long add ... to the display string.
			int nStrLen = str1.GetLength()+1;
			char* pszTmp = new char[nStrLen];
			memset(pszTmp,0,nStrLen);
			strcpy(pszTmp,str1);
			ic_dotstr(pszTmp,50,"e"/*DNT*/);
			str1=pszTmp;
			delete [] pszTmp;
		}
		SetTitle(str1);

		if (g_Vba6Installed)
		{
			if (ApcProject)
			{
				// rename the VBA Project
				CString caption;
				CString title = GetTitle();
				int n = title.Find('.');
				if (n==-1)
					caption = title;
				else
					caption = title.Left(n);
				VERIFY(SUCCEEDED(ApcProject->APC_PUT(Name)(CComBSTR(caption))));
				ApcProject->APC_PUT(Dirty)(VARIANT_FALSE);
			}
		}

		//*** Update the most recent used file list in the menus
		if(NULL!=pMain && NULL!=pMain->m_pMenuMain)
		{
			pMain->m_pMenuMain->UpdateMRU(TRUE);
		}

		bRet=TRUE;
	}

out:

	if(!SDS_AutoSave)
	{
		LockDocument(GetPathName());
	}
	if(NULL!=fname)
	{
		delete [] fname;
	}
	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// CIcadDoc server implementation

COleServerItem* CIcadDoc::OnGetEmbeddedItem()
{
	// OnGetEmbeddedItem is called by the framework to get the COleServerItem
	//  that is associated with the document.  It is only called when necessary.

	CIcadSrvrItem* pItem = new CIcadSrvrItem(this);
	ASSERT_VALID(pItem);
	//*** Set the pointer to the view.
	CIcadView *pView=NULL;
	POSITION pos=GetFirstViewPosition();
	if(NULL!=pos)
	{
		pView=(CIcadView *)GetNextView(pos);
		ASSERT_KINDOF(CIcadView, pView);
	}
	pItem->m_pView=pView;
	return pItem;
}

void CIcadDoc::OnOpenEmbedding(LPSTORAGE lpStorage)
{
	ASSERT_VALID(this);
	ASSERT(lpStorage != NULL);

	// save state
	BOOL bUserCtrl = AfxOleGetUserCtrl();


	TRY
	{
		// EBATECH(CNBR) -[ 2002/5/30 Use TEMP dir instead of C:\ //
		char szPathDir[IC_FILESIZE+IC_PATHSIZE];
		char szPathName[IC_FILESIZE+IC_PATHSIZE];
		//char szPathName[IC_FILESIZE+IC_PATHSIZE], *pszPathName;
		// EBATECH(CNBR) ]-
		HRESULT hr;
		LPSTREAM lpStream;
		void *buffer;
		int bufsize = 0;
		unsigned long size;
		STATSTG StatStg;

		// abort changes to current document
		DeleteContents();

		///Open the Contents stream
		// ToDo GH OLESTR Return on Macro
		// hr = lpStorage->OpenStream(OLESTR(OleResourceString(IDC_ICADDOC_CONTENTS_27, "Contents" )), NULL,
		hr = lpStorage->OpenStream(OLESTR("Contents"/*DNT*/), NULL,
							STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
							0, &lpStream );
		if (FAILED(hr)) AfxThrowMemoryException();

		//allocate memory for a buffer, and read the stream information into this buffer
		hr = lpStream->Stat (&StatStg, STATFLAG_DEFAULT);
			bufsize = StatStg.cbSize.LowPart;
		ASSERT (bufsize != 0);
		buffer = new BYTE[bufsize];
		hr = lpStream->Read( buffer, bufsize, &size);
		if (FAILED(hr) || (size == 0)) AfxThrowMemoryException();
		// EBATECH(CNBR) -[ 2002/5/30 Use TEMP dir instead of C:\ //
		//write this buffer into a temp file on disk
		hr = GetTempPath(sizeof(szPathDir),szPathDir);
		if( FAILED(hr) || !GetTempFileName(szPathDir,"tmp",0,szPathName))
		//if( ( pszPathName = _tempnam("c:\\tmp"/*DNT*/, "DWG"/*DNT*/ ) ) == NULL )
		{
			sds_alert(ResourceString(IDC_ICADDOC_CANNOT_CREATE_A_30, "Cannot create a unique filename." ));

			if (NULL != buffer)				
				delete [] buffer;

			return ;
		}
 		//strncpy(szPathName,pszPathName,sizeof(szPathName));
		//free(pszPathName);
		// EBATECH(CNBR) ]-
		ic_setext(szPathName,"dwg"/*DNT*/);
		FILE *fp = fopen (szPathName, "wb"/*DNT*/);
		fwrite (buffer, 1, bufsize, fp);
		fclose (fp);		
		delete [] buffer;
		lpStream->Release();

		// open document from the sub-storage
		if (!OnOpenDocument(szPathName))
			AfxThrowMemoryException();

		// now document is storage based
		//m_strPathName.Empty();
		m_bEmbedded = TRUE;
		unlink(szPathName);
	}
	CATCH_ALL(e)
	{
		// restore state
		AfxOleSetUserCtrl(bUserCtrl);
		THROW_LAST();
	}
	END_CATCH_ALL

	// restore state
	AfxOleSetUserCtrl(bUserCtrl);

	SetModifiedFlag(FALSE); // start off with unmodified
	SendInitialUpdate();
}

void CIcadDoc::OnSaveEmbedding(LPSTORAGE lpStorage)
{
	ASSERT_VALID(this);
	ASSERT(lpStorage != NULL);

	// save state
	BOOL bUserCtrl = AfxOleGetUserCtrl();

	TRY
	{
		///create a temporary file on disk and save the dwg to it
		char szPathName[IC_FILESIZE+IC_PATHSIZE], *pszPathName;
		if( ( pszPathName = _tempnam("c:\\tmp"/*DNT*/, "DWG"/*DNT*/ ) ) == NULL )
		{
			sds_alert(ResourceString(IDC_ICADDOC_CANNOT_CREATE_A_30, "Cannot create a unique filename." ));
			return;
		}
		strncpy(szPathName,pszPathName,sizeof(szPathName));		
		IC_FREE(pszPathName);
		ic_setext(szPathName,"dwg"/*DNT*/);

		//*** Set a flag so we know about it in OnSaveDocument()
		m_bSaveToStorage=TRUE;
		// save document to the file on disk
		if (!OnSaveDocument(szPathName))
		{
			m_bSaveToStorage=FALSE;
			AfxThrowMemoryException();
		}
		m_bSaveToStorage=FALSE;

		///read the data from the temp file into a buffer
		HRESULT hr;
		LPSTREAM lpStream;
		struct _stat buf;
		void *buffer;
		int bufsize = 0;
		if (!_stat( szPathName, &buf ))
			bufsize = buf.st_size;		
		buffer = new BYTE[bufsize];

		FILE *fp = fopen (szPathName, "rb"/*DNT*/);
		fread (buffer, 1, bufsize, fp);
		fclose (fp);
		unlink(szPathName);

		///create a stream in the storage, and write the buffer into it.
		//First try to open an existing stream
		hr = lpStorage->OpenStream(
						// ToDo GH OLESTR Macro Return Code
						// OLESTR(ResourceString(IDC_ICADDOC_CONTENTS_27, "Contents" )), NULL,
						OLESTR("Contents"/*DNT*/), NULL,
						STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
						0, &lpStream
						);
		//if no stream exists, create one
		if (( hr != S_OK ) || (lpStream == NULL))
		{
			// ToDo GH OLESTR Macro
			// hr = lpStorage->CreateStream( OLESTR(ResourceString(IDC_ICADDOC_CONTENTS_27, "Contents" )),
			hr = lpStorage->CreateStream( OLESTR("Contents"/*DNT*/),
				STGM_DIRECT | STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
				0, 0, &lpStream	);
		}
		if ( (hr == S_OK) && lpStream )
		{
			LARGE_INTEGER lint = {0,0};
			unsigned long size;
			hr = lpStream->Seek( lint, STREAM_SEEK_SET, NULL);
			if ( hr == NOERROR )
			{
				hr = lpStream->Write( buffer, bufsize, &size);
			}
			lpStream->Release();
		}
		IC_FREE(buffer);		
		if (FAILED(hr)) AfxThrowMemoryException();
	}
	CATCH_ALL(e)
	{
		// restore state
		AfxOleSetUserCtrl(bUserCtrl);
		THROW_LAST();
	}
	END_CATCH_ALL

	// restore state
	AfxOleSetUserCtrl(bUserCtrl);
}

void CIcadDoc::OnDeactivateUI(BOOL bUndoable)
{
	//*** Cancel any commands that may be in progress.
	SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C^C"/*DNT*/);
	//*** Save the position of the inplace toolbars before deactivating.
	if(NULL!=m_pInPlaceFrame)
	{
		((CIcadIpFrame*)m_pInPlaceFrame)->m_pTbMain->SaveToRegistry(TRUE);
	}

	if (g_Vba6Installed)
		CApcDocument<CIcadDoc,COleServerDoc>::OnDeactivateUI(bUndoable);
	else
		COleServerDoc::OnDeactivateUI(bUndoable);

	//currently ICAD does not set (or resets too early) the modified flag, so I'm faking it out here
	SetModifiedFlag (TRUE);

	if (IsModified())
	{
		//this causes any changes to be saved into the storage
		SaveModified();
		//this causes the metafile to be updated with any changes
//		UpdateAllItems(NULL, NULL, NULL, DVASPECT_CONTENT);
	}

}

/////////////////////////////////////////////////////////////////////////////
// CIcadDoc serialization

void CIcadDoc::Serialize(CArchive& ar)
{
	if (g_Vba6Installed)
	{
		if (ar.IsStoring())
		{
			ar << (DWORD) ApcProjectItem.ID();
		}
		else
		{
			HRESULT hr;
			DWORD nID;
			ar >> nID;

			if ((long)nID == -1)
				{
				hr = E_FAIL;
				AfxThrowOleException(hr);
				}

			// Declare the project item(s) to the ApcProject
			CComPtr<IIcadDocument> spDoc;
			hr = m_pAutoDoc->QueryInterface (&spDoc);

			if (FAILED(hr))
				AfxThrowOleException(hr);

			hr = ApcProjectItem.Register(ApcProject, spDoc, nID);
			if (FAILED(hr))
				AfxThrowOleException(hr);
		}

		// Calling the base class COleServerDoc enables serialization
		//  of the container document's COleClientItem objects.
		CApcDocument<CIcadDoc,COleServerDoc>::Serialize(ar);
	}
	else
		COleServerDoc::Serialize(ar);

}

HMENU CIcadDoc::GetDefaultMenu()
{
	CMainWindow* pMain = SDS_CMainWindow;
	if(NULL==pMain || NULL==pMain->m_pMenuMain)
	{
		return NULL;
	}
	if(NULL!=m_pIcadMenu)
	{
		pMain->m_pMenuMain->SetCurrentMenu(m_pIcadMenu);
		if(NULL==m_pIcadMenu->m_hMenu)
		{
			m_pIcadMenu->UpdateVisibility();
		}
		return m_pIcadMenu->m_hMenu;
	}
	CIcadMenu* pMenu = pMain->m_pMenuMain->GetDefaultMenu();
	if(NULL!=pMenu)
	{
		BOOL bIsEmbedded = IsEmbedded();
		if(pMenu->m_bIsEmbedMenu!=bIsEmbedded)
		{
			//*** Load or unload the server embedded menu
			pMenu->m_bIsEmbedMenu=bIsEmbedded;
			pMenu->UpdateVisibility();
		}
		pMain->m_pMenuMain->SetCurrentMenu(pMenu);
		return pMenu->m_hMenu;
	}
	return NULL;
}


HRESULT CIcadDoc::ApcProjectItem_Activate()
{
	POSITION pos = GetFirstViewPosition();
	if (pos)
	{
		CIcadView* pView;
		pView = (CIcadView*) GetNextView(pos);
		if (pView)
		{
			CFrameWnd* pFrame;
			pFrame = pView->GetParentFrame();
			if (pFrame)
				pFrame->BringWindowToTop();
		}
	}

	return S_OK;
}

BOOL CIcadDoc::CheckDesignMode()
{
	if (!g_Vba6Installed)
		return FALSE;

	#ifdef _DEBUG

		AxProjectMode mode;
		if (ApcProject->get_Mode(&mode) == S_OK)
			ASSERT(m_bDesignMode == (mode == axModeDesign));

	#endif

	return m_bDesignMode;
}

void CIcadDoc::FireEvent(int id)
{
	if (g_Vba6Installed)
	{
		CComPtr<IIcadDocument> spDoc;
		HRESULT hr = m_pAutoDoc->QueryInterface (&spDoc);
		ASSERT(SUCCEEDED(hr));
		if (spDoc)
		{
			hr = ApcFireEvent(spDoc, DIID__EIcadDocument, id, NULL, NULL);
			ASSERT(SUCCEEDED(hr));
		}
	}
	else
	{
		if (m_pAutoDoc)
		{
			HRESULT hr =
				GetApp()->comAddinManager ().fireDocumentEvent(id, m_pAutoDoc);
			ASSERT(SUCCEEDED(hr));
		}
	}
}


HRESULT CIcadDoc::DefineProjectItem()
{
	if (!g_Vba6Installed)
		return E_FAIL;

	HRESULT hr;

	CComPtr<IIcadDocument> spDoc;
	hr = m_pAutoDoc->QueryInterface (&spDoc);
	if (FAILED(hr))
		return hr;

	// Declare the project item(s) to the ApcProject

	CComBSTR Docname;
	Docname.LoadString(IDC_VBA_THISDOCUMENT);

	hr = ApcProjectItem.Define(ApcProject, spDoc, axTypeHostProjectItem, Docname);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void CIcadDoc::DeleteContents()
{
	if (g_Vba6Installed)
	{
		ApcProjectItem.Detach();

		CApcDocument<CIcadDoc,COleServerDoc>::DeleteContents();
	}
	else
		COleServerDoc::DeleteContents();

}

HRESULT CIcadDoc::ApcProject_Save()	//Copied from COleDocument::OnSaveDocument
{
	if (DoFileSave())
		return S_OK;
	else
		return E_FAIL;
}

HRESULT CIcadDoc::ApcProjectItem_View()
{
	//
	// First bring this PI Frame to the top
	//
	ApcProjectItem_Activate();

	//
	// Then, activate the top level window
	//
	GetApp()->GetMainWnd()->BringWindowToTop();

	return S_OK;

}

COleClientItem* CIcadDoc::CreateOleItem(CView* pView, db_handitem* pHandItem)
{
	//*** Invoke the standard Insert Object dialog box to obtain information
	//*** for new CIcadCntrItem object.
	BOOL bRet=TRUE;
	COleInsertDialog* pDlg = NULL;
	CIcadCntrItem* pItem = NULL;
	if(NULL==pHandItem)
	{
		pDlg = new COleInsertDialog();
		//exclude our own classid from the dialog
		pDlg->m_io.cClsidExclude = 1;
		pDlg->m_io.lpClsidExclude = (LPCLSID)&(CLSID_Document);

		if(pDlg->DoModal()!=IDOK)
		{
			delete pDlg;
			return NULL;
		}
		BeginWaitCursor();

		CLSID clsid;
		//if the inserted object is a dwg file, quit !  We don't support this yet.
		if (pDlg->GetSelectionType() == COleInsertDialog::createNewItem)
		{
			clsid = pDlg->GetClassID();
		}
		else
		{
			CString cFilename = pDlg->GetPathName();
			int len = cFilename.GetLength();
			LPWSTR wszFilename;
			wszFilename = new wchar_t [len+1];
			memset(wszFilename , 0, (len+1) * sizeof(wchar_t));
			mbstowcs (wszFilename, cFilename, len);
			GetClassFile (wszFilename, &clsid);
			IC_FREE(wszFilename);
		}
		if (IsEqualCLSID(clsid, CLSID_Document))
		{
			bRet=FALSE;
			goto out;
		}
	}

	//*** Create new item connected to this document.
	pItem = new CIcadCntrItem(this,pHandItem);
	if(NULL==pItem)
	{
		bRet=FALSE;
		goto out;
	}
	if(NULL!=pDlg)
	{
		// We have to do a special set undo here because this
		// WindAction is done witt a PostThreadMessage().  OUCH!
		SDS_SetUndo(IC_UNDO_COMMAND_BEGIN,(void *)"INSERTOBJ"/*DNT*/,NULL,NULL,SDS_CURDWG);
		//*** Initialize the item from the dialog data.
		if(!pDlg->CreateItem(pItem))
		{
			bRet=FALSE;
			SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)"INSERTOBJ"/*DNT*/,NULL,NULL,SDS_CURDWG);
			goto out;
		}
		//*** Create the entity and set the initial size of the object.
		pItem->UpdateHandItem((CIcadView*)pView,TRUE);
		pItem->UpdateFromServerExtent((CIcadView*)pView);
		//*** As an arbitrary user interface design, this sets the selection
		//*** to the last item inserted.
		((CIcadView*)pView)->SetSelection(pItem);
		//*** If item created from class list (not from file) then launch
		//*** the server to edit the item.
		if(pDlg->GetSelectionType()==COleInsertDialog::createNewItem)
		{
			pItem->DoVerb(OLEIVERB_SHOW,pView);
		}
		else if(NULL!=pItem->m_pHandItem)
		{
			//*** Redraw the entity
			sds_name ename;
			ename[0]=(long)pItem->m_pHandItem;
			ename[1]=(long)m_dbDrawing;
			sds_redraw(ename,IC_REDRAW_DRAW);
		}
		SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)"INSERTOBJ"/*DNT*/,NULL,NULL,SDS_CURDWG);
	}
	else
	{
		//*** Create an object from a handitem.
		if(!pItem->ReadAcadOLEData((CIcadView*)pView))
		{
			bRet=FALSE;
			goto out;
		}
	}

out:

	if(NULL!=pDlg)
	{
		EndWaitCursor();
		delete pDlg;
		pDlg=NULL;
	}
	if(FALSE==bRet)
	{
		if(NULL!=pItem)
		{
			pItem->Delete();
			pItem=NULL;
		}
		sds_resbuf expertrb;
		if (SDS_getvar(NULL,DB_QEXPERT,&expertrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
			if (expertrb.resval.rint < 2)
			{
		sds_alert(ResourceString(IDC_ICADDOC_FAILED_TO_CREAT_34, "Failed to create the item." ));
	}

	}
	return pItem;
}

BOOL CIcadDoc::DeleteOleItem(db_handitem* pHandItem)
{
	CIcadCntrItem* pItem = GetOleItem(pHandItem);
	if(NULL==pItem)
	{
		return TRUE;
	}
	//*** Don't delete the item if in-place active.
	CIcadView* pView;
	POSITION pos=GetFirstViewPosition();
	while(pos!=NULL)
	{
		pView=(CIcadView*)GetNextView(pos);
		ASSERT_KINDOF(CIcadView, pView);

		COleClientItem* pActiveItem = GetInPlaceActiveItem(pView);
		if(pActiveItem != NULL && pActiveItem==pItem)
		{
			//*** Avoid this happening at all costs.
			return FALSE;
		}
		pView->m_pSelection=NULL;
	}
	pItem->Release(OLECLOSE_NOSAVE);// release OLE object
	RemoveItem(pItem);  // disconnect from document
	pItem->InternalRelease();   // may 'delete pItem'
	return TRUE;
}

BOOL CIcadDoc::DrawOleItem()
{
	CIcadView* pView = (CIcadView*)m_pViewForRedraw;
	db_handitem* pHandItem = m_pHandItemForRedraw;
	CDC* pDC = m_pDCForRedraw;
	struct gr_view *pGrView = m_pGviewForRedraw;

	if(NULL==pView || NULL==pHandItem ||
		NULL==pDC)
	{
		return FALSE;
	}
	//*** Return if the data in the handitem is NULL (from entmake)
	char* pOleData;
	pHandItem->get_310(&pOleData);
	if(NULL==pOleData)
	{
		return FALSE;
	}
	BOOL bRet=FALSE;
	//*** Find the OLE item that contains pHandItem.
	CIcadCntrItem* pItem = GetOleItem(pHandItem);
	if(NULL==pItem)
	{
		if(NULL==(pItem=(CIcadCntrItem*)CreateOleItem(pView,
			pHandItem)))
		{
			return FALSE;
		}
	}
	pItem->Draw(pView,pDC,pGrView);
	return TRUE;
}

CIcadCntrItem* CIcadDoc::GetOleItem(db_handitem* pHandItem)
{
	//*** Find the OLE item that contains pHandItem.
	CIcadCntrItem* pItem = NULL;
	POSITION pos = GetStartPosition();
//	POSITION pos = m_docItemList.GetHeadPosition();
	while(pos!=NULL)
	{
		pItem = (CIcadCntrItem*)GetNextItem(pos);
		if(!pItem->IsKindOf(RUNTIME_CLASS(CIcadCntrItem)))
		{
			continue;
		}
		if(pHandItem==pItem->m_pHandItem)
		{
			return pItem;
		}
	}
	return NULL;
}

BOOL CIcadDoc::DoFileSave()
{
	if(RTNORM!=cmd_SaveFunc(0,this))
	{
		return FALSE;
	}
	return TRUE;
}

LPMONIKER CIcadDoc::GetMoniker(OLEGETMONIKER nAssign)
{
	USES_CONVERSION;
	ASSERT_VALID(this);

	//*** From COleServerDoc
	if (m_lpClientSite != NULL)
	{
		// get moniker from client site instead of from document
		LPMONIKER lpMoniker = NULL;
		m_lpClientSite->GetMoniker(nAssign, OLEWHICHMK_OBJFULL, &lpMoniker);
		return lpMoniker;
	}

	CString strMoniker;
	if(!m_strMoniker.IsEmpty())
	{
		strMoniker = m_strMoniker;
	}
	else if(!m_strPathName.IsEmpty())
	{
		strMoniker = m_strPathName;
	}
	else
	{
		strMoniker = ResourceString(IDC_ICADDOC_UNNAMED____35, "Unnamed - " );
		strMoniker+=GetTitle();
	}

	//*** Return file moniker.
	LPMONIKER lpMoniker;
	CreateFileMoniker(T2COLE(strMoniker), &lpMoniker);
	return lpMoniker;
}



// ********************
// HELPER METHOD
//
// GetCurrentViewport
//
db_viewport *
CIcadDoc::GetCurrentViewport( void )
	{

	return this->m_dbDrawing->GetCurrentViewport();
	}


/////////////////////////////////////////////////////////////////////////////
// Automation Helpers

HRESULT CIcadDoc::PurgeDrawing ()
	{
	purge_drawing (m_dbDrawing, "BATCH");
	return S_OK;
	}

HRESULT CIcadDoc::RegenAllViews ()
	{
	cmd_regenall();
	return S_OK;
	}

HRESULT CIcadDoc::RegenCurrentView ()
	{
	cmd_regen();
	return S_OK;
	}

HRESULT CIcadDoc::RedrawAllViews ()
	{
	cmd_redrawall();
	return S_OK;
	}

HRESULT CIcadDoc::RedrawCurrentView ()
	{
	cmd_redraw();
	return S_OK;
	}

void cmd_viewport_splitter (CIcadView *pView, int nHowMany, int nOrientation);	//from cmd_viewports.cpp

HRESULT CIcadDoc::SplitView (CIcadView *pView, int nHowMany, int nOrientation)
	{
	cmd_viewport_splitter (pView, nHowMany, nOrientation);
	return S_OK;
	}

HRESULT CIcadDoc::SetView (db_vporttabrec *pVPort, db_viewtabrec *pView)
	{
	//this function copies the view parameters from the view to the vport

	//first set the vport with all the current default sysvars
	if (SDS_VarToTab(m_dbDrawing, (db_handitem *)pVPort) != RTNORM)
		return E_FAIL;

	//now copy over all the relevant vars from the view to the vport
	sds_point dir;
	pView->get_11 (dir);
	pVPort->set_16 (dir);

	sds_point target;
	pView->get_12 (target);
	pVPort->set_17 (target);

	sds_point center;
	pView->get_10 (center);
	pVPort->set_12 (center);

	double height;
	pView->get_40 (&height);
	pVPort->set_40 (height);

	double width;
	pView->get_41 (&width);
	width = height / width;
	pVPort->set_41 (width);

	double lenslength;
	pView->get_42 (&lenslength);
	pVPort->set_42 (lenslength);

	double twistangle;
	pView->get_50 (&twistangle);
	pVPort->set_51 (twistangle);

	//set the appropriate sysvars
	if (SDS_TabToVar(m_dbDrawing, pVPort) != RTNORM)
		return E_FAIL;

	RegenCurrentView();

	return S_OK;
	}

bool cmd_viewport_save_configuration (char * vp_to_save, db_drawing *flp);
bool cmd_viewport_restore_configuration (char * vp_to_restore, db_drawing *flp);

HRESULT CIcadDoc::SaveViewportConfiguration (char * vp_name)
	{
	cmd_viewport_save_configuration (vp_name, m_dbDrawing);
	return S_OK;
	}

HRESULT CIcadDoc::RestoreViewportConfiguration (char * vp_name)
	{
	cmd_viewport_restore_configuration (vp_name, m_dbDrawing);
	return S_OK;
	}

HRESULT CIcadDoc::InsertBlock (char *blkname,
							   db_insert *ins,
							   sds_point inspt,
							   db_insert **newins,
							   bool explode)
{
	char blkfile[IC_ACADBUF], newblkname[IC_ACADBUF];
	struct resbuf setgetrb;
	sds_real curelev=0.0;
	int ret=0;
	SDS_getvar(NULL,DB_QELEVATION,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	curelev=setgetrb.resval.rreal;

	if (blkname == NULL)
		return E_FAIL;

	// takes blkname and returns block name, and path if any
	if (ProcessInsertBlockName(blkname, newblkname, blkfile)!=RTNORM)
		return E_FAIL;

	struct resbuf *tmprb;
	if ((tmprb = sds_tblsearch("BLOCK"/*DNT*/, newblkname, 0))!=NULL)
	{
								// it already exists

		sds_relrb(tmprb);
	}
	else
	{
								// create it from file

		// we need the extension. ProcesInsertBlockName is the best place to do this, but it doesn't.
		char* startext=strrchr(blkname,'.'/*DNT*/);
		if (startext){
			if(!strcmpi (startext,".dwg"))
				ic_setext(blkfile,".DWG");
			else if(!strcmpi (startext,".dxf"))
				ic_setext(blkfile,".DXF");
		}
		// find the file's true path using sds_findfile
		if (FindTheBlockFile(blkfile, blkfile)!=RTNORM)
			return E_FAIL;


		if (LoadTheBlock(newblkname, blkfile)!=RTNORM)
			return E_FAIL;
	}

	ins->set_2(newblkname, m_dbDrawing);

	//turn off the dialogs during insertion
	struct resbuf rb;
	SDS_getvar(NULL,DB_QATTDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int attdia = rb.resval.rint;
	rb.resval.rint = 0;
	SDS_setvar(NULL,DB_QATTDIA,&rb,m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);

	//turn off prompting for attributes during insertion
   	SDS_getvar(NULL,DB_QATTREQ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int attreq = rb.resval.rint;
	rb.resval.rint = 0;
	SDS_setvar(NULL,DB_QATTREQ,&rb,m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);

	ret = DoTheInsert(newblkname,explode,ins,false,inspt,curelev,false,false);

	if (newins)
	{
		sds_name newinsname;
		sds_entlast(newinsname);
		*newins = (db_insert *)newinsname[0];
	}

	//restore the system variables afer the insert process
	rb.resval.rint = attdia;
	SDS_setvar(NULL,DB_QATTDIA,&rb,m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);
	rb.resval.rint = attreq;
	SDS_setvar(NULL,DB_QATTREQ,&rb,m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);

	if (ret==RTNORM)
		return S_OK;
	else
		return E_FAIL;
}

short prepare_drawing_for_purge (db_drawing *dp);

HRESULT CIcadDoc::DeleteEntity (db_handitem *hip)
	{
	//Deletes the entity including complex entities
	BOOL bCanDelete = FALSE;

	if (hip->ret_deleted())
		return E_FAIL;

	switch (hip->ret_type())
		{
		//special handling is required for table items since they can be deleted only if not referenced
		case DB_LAYERTABREC:
		case DB_LTYPETABREC:
		case DB_STYLETABREC:
		case DB_UCSTABREC:
		case DB_VIEWTABREC:
		case DB_BLOCKTABREC:
		case DB_DIMSTYLETABREC:
			//prepare the drawing by marking all table items with its referenced flag
			if (prepare_drawing_for_purge (m_dbDrawing) != RTNORM)
				return E_FAIL;

			int	flag;
			hip->get_70(&flag);
			bCanDelete =  (flag & IC_REFERENCED) ? FALSE : TRUE;
			break;

		default:
			bCanDelete = TRUE;
			break;
		}

	if (!bCanDelete)
		return E_FAIL;

	sds_name ename;
	ename[1] = (long)m_dbDrawing;
	ename[0] = (long)hip;

	if (sds_entdel(ename) == RTNORM)
	{
		if(sds_ssmemb(ename, m_pGripSelection) == RTNORM)	/*D.G.*/// Delete it from
		{													// the GripSelection too.
			SDS_AddGripNodes(this, ename, -1);
			SDS_AddGripNodes(this, ename, 0);
			sds_ssdel(ename, m_pGripSelection);
//4M Item:28->
SDS_DrawGripNodes(this);
//<-4M Item:28
}
		return NOERROR;
	}
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::RedrawEntity (db_handitem *hip, redraw_mode mode)
	{
	sds_name ename;
	ename[1] = (long)m_dbDrawing;
	ename[0] = (long)hip;

	if (sds_redraw (ename, mode) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::UpdateEntity (db_handitem *hip)
	{
	sds_name ename;
	HRESULT ret;
	ename[1] = (long)m_dbDrawing;
	ename[0] = (long)hip;

	// DP: sds_entupd() doesn't update text justification,
	// DP: so use sds_entmod() in this case (change it later)
	if(hip->ret_type() == DB_TEXT)
	{
		struct sds_resbuf *entlist;

		entlist=sds_entget(ename);
		if( entlist )
		{
			if (sds_entmod(entlist) == RTNORM)
				ret = NOERROR;
			else
				ret = E_FAIL;
		}
		FreeResbufIfNotNull(&entlist); // EBATECH(CNBR) Bugzilla#78467 Not free here!
		return ret;
	}
	else
	{
	if (sds_entupd(ename) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}
	}

HRESULT CIcadDoc::XformEntity (db_handitem *hip, sds_matrix matrix)
	{
	sds_name ename;
	ename[1] = (long)m_dbDrawing;
	ename[0] = (long)hip;

	if (SDS_xforment (ename, NULL, matrix) == 0)
		return NOERROR;
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::GetActiveSelectionSet(long* pLength, sds_name ss)
{
	sdsengine_sslength(m_pGripSelection, pLength);
	if(!*pLength)
		return S_FALSE;

	sds_name_set(m_pGripSelection, ss);

	return NOERROR;
}

HRESULT CIcadDoc::LoadorSaveLinetype (char *Linetype, char *Filename, BOOL bSave)
	{
	if (bSave)	//save the linetype to the file
		{
		//TODO implement saving of linetypes to a file.  This will be called from Linetype.Save
		return NOERROR;
		}
	else		//load the linetype from the file
		{
		if (cmd_ltload(Linetype, Filename, 0) == RTNORM)
			return NOERROR;
		else
			return E_FAIL;
		}
	}

HRESULT CIcadDoc::MakePViewportActive (db_viewport * pViewport)
	{
	if (pViewport == NULL)
		return E_INVALIDARG;

	CIcadView *pView=NULL;
	POSITION pos=GetFirstViewPosition();
	if(NULL!=pos)
	{
		pView=(CIcadView *)GetNextView(pos);
		ASSERT_KINDOF(CIcadView, pView);
	}

	if (pView == NULL)
		return E_FAIL;

	cmd_MakeVpActive(pView, pViewport, m_dbDrawing);

	return NOERROR;
	}

HRESULT CIcadDoc::MSpaceOrPSpace (BOOL mspace)
	{
	short rc;

	if (mspace)
		rc = cmd_mspace();
	else
		rc = cmd_pspace();

	if (rc == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

/*
	Filetype is the same as SDS_LastFilterIndex
	24 = cmd_BitmapFilterIndex
	25 = WMF
	26 = EMF
*/
extern cmd_BitmapFilterIndex;

HRESULT CIcadDoc::ExportEntities (CString FileName, int FileType, sds_name sset)
	{
	if (FileType > cmd_BitmapFilterIndex+2 || FileType < cmd_BitmapFilterIndex)
		return E_INVALIDARG;

	if (FileName.IsEmpty())
		return E_INVALIDARG;

	long LenSelSet;

	//if the selection set is empty, put all entities into the selection set
	if ((RTNORM != sds_sslength(sset, &LenSelSet)) || (LenSelSet == 0) )
		{
		if (sds_pmtssget(NULL,"X"/*DNT*/,NULL,NULL,NULL,SDS_EditPropsSS,0)!=RTNORM)
			return E_INVALIDARG;
		}
	else
		SDS_sscpy(SDS_EditPropsSS, sset);

	char *fname = FileName.GetBuffer(0);

	ASSERT(cmd_BitmapFilterIndex == 24); // If this fires, you'll need 
										 // to change the case consts below.
	//the following code was copied from cmds\save.cpp cmd_SaveFunc()
	switch (FileType)
		{
		case 24: //*** Bmp
			{
			ExecuteUIAction( ICAD_WNDACTION_CREATEHBMP );
			HBITMAP hBmp = (HBITMAP)SDS_CMainWindow->m_pvWndAction;
			SDS_CMainWindow->m_pvWndAction=NULL;
			if(NULL!=hBmp)
				{
				char *pBuffer;
				long  bytes;
				//*** When this object is destructed, it should be deleting the hBmp;
				CBitmap bmp;
				bmp.Attach(hBmp);
				if(0==SDS_SaveBitmapToBuffer(&bmp,&pBuffer,&bytes,FALSE))
					{
					SDS_SaveBufferToFile(pBuffer,bytes,fname);					
					delete [] pBuffer;
					}
				}
			}
			break;
		case 25: //*** Wmf
			{
			ExecuteUIAction( ICAD_WNDACTION_CREATEHWMF);
			ICADWMFINFO* pWmfInfo = (ICADWMFINFO*)SDS_CMainWindow->m_pvWndAction;
			SDS_CMainWindow->m_pvWndAction=NULL;
			if(NULL!=pWmfInfo)
				{
				SDS_WriteWMF(pWmfInfo,fname);
				::DeleteMetaFile(pWmfInfo->hMetaFile);
				delete pWmfInfo;
				}
			}
			break;
		case 26: //*** Emf
			{
			ExecuteUIAction( ICAD_WNDACTION_CREATEHEMF );
			HENHMETAFILE hMetaFile = (HENHMETAFILE)SDS_CMainWindow->m_pvWndAction;
			SDS_CMainWindow->m_pvWndAction=NULL;
			if(NULL!=hMetaFile)
				{
				HENHMETAFILE hCopyMetaFile = ::CopyEnhMetaFile(hMetaFile,fname);
				::DeleteEnhMetaFile(hMetaFile);
				::DeleteEnhMetaFile(hCopyMetaFile);
				}
			}
			break;
		}

	sds_ssfree(SDS_EditPropsSS);
	return NOERROR;
	}

HRESULT CIcadDoc::setUndo( int mode, void *data0, void *data1, void *data2 )
{
	int ret = SDS_SetUndo( mode, data0, data1, data2, m_dbDrawing );
	if( ret == RTNORM )
		return NOERROR;
	else
		return E_FAIL;
}

// EBATECH(CNBR) -[ 2001.07.28 for OEM App.
BOOL	CIcadDoc::createDocPalette()
{
	int i;
	db_handitem *top, *obj;
	struct resbuf *begpp, *endpp, *ptr;
	const char isxfb_col_table[] =	"ISXFB_COL_TABLE";
	if( m_DocPalette ){
		return TRUE;
	}
	if( m_dbDrawing == NULL ){
		return FALSE;
	}
	if(( top = m_dbDrawing->namedobjdict()) == NULL ){
		return FALSE;
	}
	if(( obj = m_dbDrawing->dictsearch(top, (char*)isxfb_col_table, 0)) == NULL ){
		return FALSE;
	}
	if( ((db_xrecord*)obj)->entgetspecific(&begpp, &endpp, m_dbDrawing ) != 0 ){
		return FALSE;
	}
	if(( m_DocPalette = new RGBQUAD[257] ) == NULL ){
		IC_RELRB(begpp);
		return FALSE;
	}
	for( i = 0, ptr = begpp; ptr != NULL ; ptr = ptr->rbnext ){
		if( ptr->restype == 90 ){
			long col = ptr->resval.rlong;
			m_DocPalette[i].rgbRed = GetRValue(col);
			m_DocPalette[i].rgbGreen = GetGValue(col);
			m_DocPalette[i].rgbBlue = GetBValue(col);
			m_DocPalette[i].rgbReserved = 0;
			i++;
		}
	}
	for( ; i < 257 ; i++ ){
		m_DocPalette[i].rgbRed = 255;
		m_DocPalette[i].rgbGreen = 255;
		m_DocPalette[i].rgbBlue = 255;
		m_DocPalette[i].rgbReserved = 0;
	}
	IC_RELRB(begpp);
	return TRUE;
}
// EBATECH(CNBR) -[ 2002/4/24 Support SHAPE
HRESULT CIcadDoc::LoadShapeFile (const char *Filename)
	{
	if (cmd_loadshape((char*)Filename) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}
// EBATECH(CNBR) ]-

