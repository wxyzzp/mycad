/* E:\INTELLICAD\PRJ\ICAD\ICADMAIN.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * CMainWindow implementation
 *
 */

#include "Icad.h"/*DNT*/
#include "IcadApp.h"

#include "IcadChildWnd.h"/*DNT*/
#include "IcadCmdBar.h"
#include "IcadTextScrn.h"
#include "IcadSplash.h"/*DNT*/
#include "IcadToolBarMain.h"/*DNT*/
#include "IcadRanges.h"/*DNT*/
#include "IcadApp.h"/*DNT*/
#include "IcadDispID.h"
#include "IcadCustom.h"/*DNT*/
#include "IcadPromptMenu.h"/*DNT*/
#include "IcadMenuMain.h"/*DNT*/
#include "IcadDockBar.h"/*DNT*/
#include "IcadCntrItem.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "IcadTablet.h"/*DNT*/
#include "PrintDialog.h"/*DNT*/
#include "Paths.h"
#include "IcadView.h"
#include "Preferences.h"
#include "ICADhelp.h"
#include "IcadDocTemplate.h"
#include "configure.h"
#include "IcadDef.h"
#include "win32misc.h"
#ifndef _COMAddinManager_H
#include "COMAddinManager.h"
#endif

#include "DialogPassword.h"
#include "DwfInfoDialog.h"

#include "ItcLanguage.h" // EBATECH

#include "realtime.h" /*DNT*/
#include "lispvars.h"/*DNT*/
#include "lisp.h"/*DNT*/
#include "commandatom.h" /*DNT*/
//4M Bugzilla:78056 Item:54->
#include "TextStyleTableInfo.h"
//<-4M Item:54
#include "inputevent.h" /*DNT*/
#include "resource.hm" /*DNT*/

#ifndef BUILD_WITH_VBA
HRESULT ApcFireEvent(IUnknown* punk, REFIID iid, DISPID dispid, DISPPARAMS* pDispParams /*= NULL*/, VARIANT* pVarResult /*= NULL*/);
#endif

CMainWindow *SDS_CMainWindow;
HINSTANCE	CMainWindow::m_hInstHTMLHelp = NULL;

//extern char *SDS_RegEntry;
extern int SDS_InsideGrread;
extern bool g_Vba6Installed;

/////////////////////////////////////////////////////////////////////
// CMainWindow message map and member (huh-huh) functions

BEGIN_MESSAGE_MAP(CMainWindow, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainWindow)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_CONTEXTMENU()
	ON_WM_CLOSE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_ACTIVATE()
	ON_WM_ACTIVATEAPP()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_MESSAGE(ICAD_WNDACTION_DOMODAL,OnDoModal)
	ON_WM_MOUSEWHEEL()
	ON_WM_COPYDATA()
	//}}AFX_MSG_MAP

	ON_MESSAGE(ICAD_WNDACTION_PRINTF, OnPrintf)
	ON_MESSAGE(ICAD_WNDACTION_COLORDIA, OnColorDialog)
   ON_MESSAGE(ICAD_WNDACTION_GETPASSWORD,OnGetPassWord)
   ON_MESSAGE(ICAD_WNDACTION_GETDWFINFO,OnGetDwfInfo)

    ON_MESSAGE(WM_DISPLAYCHANGE,OnDisplayChange)
	ON_WM_DROPFILES()

	ON_COMMAND(ID_FILE_PRINT,DoPrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT,DoPrintDirect)
	// Window Action for SDS Event
	ON_COMMAND_RANGE(ICAD_WNDACTION_BEG,ICAD_WNDACTION_END,IcadWndAction)
	// Menu & Toolbar command handlers
	ON_COMMAND_RANGE(TB_START_ID,TB_END_ID,IcadMenuExec)
	ON_COMMAND_RANGE(ICAD_MRUFILELIST_START,ICAD_MRUFILELIST_END,OnMRUPick)
	ON_COMMAND_RANGE(ICAD_CONTEXTMENU_START,ICAD_CONTEXTMENU_END,OnContextMenuPick)
	ON_COMMAND_RANGE(ICAD_TEXTSCREENMENU_START,ICAD_TEXTSCREENMENU_END,OnTextScreenMenuPick)
	ON_COMMAND_RANGE(ICAD_ACCELID_START,ICAD_ACCELID_END,OnAccelKey)
 	ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
	ON_NOTIFY_RANGE(TBN_QUERYDELETE,IDR_ICAD_TOOLBAR0,IDR_ICAD_TOOLBAR10,QueryDelete)
	ON_NOTIFY_RANGE(TBN_QUERYINSERT,IDR_ICAD_TOOLBAR0,IDR_ICAD_TOOLBAR10,QueryInsert)
	ON_NOTIFY_RANGE(TBN_BEGINDRAG,IDR_ICAD_TOOLBAR0,IDR_ICAD_TOOLBAR10,BeginDrag)
	ON_NOTIFY_RANGE(TBN_ENDDRAG,IDR_ICAD_TOOLBAR0,IDR_ICAD_TOOLBAR10,EndDrag)
	ON_NOTIFY_RANGE(TBN_BEGINADJUST,IDR_ICAD_TOOLBAR0,IDR_ICAD_TOOLBAR10,BeginAdjust)
	ON_NOTIFY_RANGE(TBN_ENDADJUST,IDR_ICAD_TOOLBAR0,IDR_ICAD_TOOLBAR10,ToolBarChange)
	ON_NOTIFY_RANGE(TBN_TOOLBARCHANGE,IDR_ICAD_TOOLBAR0,IDR_ICAD_TOOLBAR10,ToolBarChange)
  	ON_NOTIFY_EX(TTN_NEEDTEXT,0,OnToolTipText)
	ON_NOTIFY_EX(TTN_NEEDTEXTW,0,OnToolTipTextW)
END_MESSAGE_MAP()


CMainWindow::CMainWindow( bool showSplash)
{
	SDS_CMainWindow=this;
	m_nMainThreadID=GetCurrentThreadId();
	m_ExpReturnHwnd=NULL;
	m_IgnoreLastCmd=false;
	m_pCmdBar=NULL;
	m_pCmdStr=NULL;
	m_pHistList=NULL;
	m_ViewCursorType=IDC_ICAD_PICK5;
	m_pPalette=NULL;
	m_bAutoMenuEnable = FALSE;
	m_pPromptMenu=NULL;
	m_pCurFrame=NULL;
	m_pCurView=NULL;
	m_pCurDoc=NULL;
	m_fIsCtrlDown=0;
	m_fIsAltDown=0;
	m_fIsShiftDown=0;
	m_nMaxCoordsLen=1;
	m_bInOpenNew=0;
	m_nOpenViews=0;
	m_nIsClosing=2;
	m_CurSession.buf=NULL;
	m_CurSession.size=0L;
	m_CurSession.seekof0=0L;
	m_CurConfig.buf=NULL;
	m_CurConfig.size=0L;
	m_CurConfig.seekof0=0L;
	m_pDragVarsCur=NULL;
	m_fMacroRecFile.m_hFile=0;
	m_fLogFile.m_hFile=0;
	m_pIcadCustom=NULL;
	m_bDaoRunning=FALSE;
	m_bCustomize=FALSE;
	m_pAccelList = new CPtrList();
	m_bKeyboardModified=FALSE;
	m_bAliasModified=FALSE;
	m_rectCreateView=NULL;
	m_wndViewFocus=NULL;
	m_wndViewClose=NULL;
	m_pViewTabEntryHip=NULL;
	m_bUsingTempView=FALSE;
	m_pIcadPrintDlg=NULL;
	m_bPrintPreview=FALSE;
	m_piVSpell=NULL;
	m_bExplorerState=FALSE;
	m_bExpInsertState=FALSE;
	m_bSentChar=FALSE;
	m_pvWndAction=NULL;
	m_bAlreadyDestroyed=FALSE;
	m_pOpenOleList=NULL;
	m_bPrintHoldFill=FALSE;
	m_bMapToRGBColor=FALSE;
	m_bIsAppActive=TRUE;
	m_pTablet = NULL;
	m_paths = NULL;
	m_pExplorer = NULL;
	m_bIsRecovering = 0;

	CRect rectWnd( CFrameWnd::rectDefault);
	WNDCLASS wc;

	// Init SDS registry.
	SDS_Init(GetSafeHwnd(),1);

	memset(&wc,0,sizeof(wc));
	wc.lpfnWndProc=AfxWndProc;
	wc.hInstance=AfxGetInstanceHandle();
	wc.hIcon=LoadIcon(wc.hInstance,MAKEINTRESOURCE(AFX_IDI_STD_MDIFRAME));
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.lpszClassName="IntelliCADApplicationWindow"/*DNT*/;

	RegisterClass(&wc);
	//*** Create the main window
//	SDS_getvar(NULL,DB_Q_VERNUM,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	char fs1[100];

	sprintf(fs1,CIcadApp::m_sApplicationTitle);

//	free(rb.resval.rstring);

	DWORD dwStyle = WS_OVERLAPPEDWINDOW;
	struct sds_resbuf rb;
	SDS_getvar(NULL,DB_QWNDLMAIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint==2)
		{
		//*** Maximize the window before the toolbars are positioned so they don't
		//*** wrap to the non-maximized size.
		//BUG 1-56283, 1-56484, 55523: ICAD does not properly maximize itself &
		//Toolbars aligned in a row will be pushed into a single column on load.
		//Rather than modifying the style to WS_MAXIMIZE later, start with WS_MAXIMIZE so that toolbars
		//are loaded correctly.
		dwStyle |= WS_MAXIMIZE;
		}
	//*** Get the window position and size
	SDS_getvar(NULL,DB_QWNDPMAIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rectWnd.left=(long)rb.resval.rpoint[0];
	rectWnd.top=(long)rb.resval.rpoint[1];
	SDS_getvar(NULL,DB_QWNDSMAIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rectWnd.right=(rectWnd.left+(long)rb.resval.rpoint[0]);
	rectWnd.bottom=(rectWnd.top+(long)rb.resval.rpoint[1]);
	//*** Set the default height if a height is not given
	if(rectWnd.IsRectNull()) {
		CRect rectDeskTop;
		SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&rectDeskTop,0);
		// EBATECH(CNBR) -[ Use MACRO Initial Window size is locale dependnt.
		if( rectDeskTop.Width()>ICAD_RECT_DESKTOP_WIDTH &&
			rectDeskTop.Height()>ICAD_RECT_DESKTOP_HEIGHT)
		{
			rectWnd.left=0;
			rectWnd.right=ICAD_RECT_DESKTOP_WIDTH;
			rectWnd.top=0;
			rectWnd.bottom=ICAD_RECT_DESKTOP_HEIGHT;
		} else rectWnd=rectDeskTop;
		// EBATECH(CNBR) ]-
	}
	Create("IntelliCADApplicationWindow"/*DNT*/,fs1, dwStyle, rectWnd, NULL, MAKEINTRESOURCE(IDR_MAINFRAME));

	//*** Setup MRU File list stuff for registry.
	SDS_getvar(NULL,DB_QNFILELIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	((CIcadApp*)AfxGetApp())->InitMRUList(rb.resval.rint);

	SDS_getvar(NULL,DB_QWNDLMDI,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint==0) {
		m_nLastMDIState=2;
	} else {
		m_nLastMDIState=rb.resval.rint;
	}


	//*** Set up the menu saved in the registry.
	m_pMenuMain = new CIcadMenuMain((CIcadApp*)AfxGetApp(),this);
	if(m_pMenuMain!=NULL)
	{
		m_pMenuMain->LoadMenu(NULL);
	}

		// restore status bar settings
	m_StatusBar.UpdateVariablePanes(NULL);

	//intialize the tablet
	m_pTablet = new CIcadTablet();
	if (m_pTablet != NULL)
		{
		char err[80];
		if (!m_pTablet->Initialize(GetSafeHwnd(), err, sizeof(err)))
			{
			//error condition occurred, but remain silent for now
			//return RTERROR;
			delete m_pTablet;
			m_pTablet = NULL;
			}
		}

	//load paths
	m_paths = new SystemPaths;
	m_paths->LoadFromRegistry();
}

CMainWindow::~CMainWindow()
	{
	delete m_pHistList;		m_pHistList=NULL;
	delete m_pCmdStr;		m_pCmdStr=NULL;
	delete m_pCmdBar;		m_pCmdBar=NULL;
	delete m_pAccelList;	m_pAccelList=NULL;
	delete m_pIcadPrintDlg;	m_pIcadPrintDlg=NULL;
	delete m_pPalette;		m_pPalette=NULL;
	if (m_pTablet)
		{
		delete m_pTablet;
		m_pTablet=NULL;
		}

	if (m_paths)
		{
		delete m_paths;
		m_paths = NULL;
		}

	// Containers of commandAtomObjects.
	delete commandAtomList;
	delete commandAtomObjPtrList;
	m_pCurView=NULL;
	m_pCurDoc=NULL;
	m_pCurFrame=NULL;
	}


db_drawing *
CMainWindow::CurrentDrawing() const
	{
	if ( Debug && FALSE )	// disabled assertion
		{
		static CIcadApp*	lastApp = NULL;

		CIcadApp	*app = GetApp();
		if ( !lastApp )
			lastApp = app;

			// this assertion will catch mismanaged AFXMODULESTATE
		ASSERT( lastApp EQ app );
		}

	return m_pCurDoc->m_dbDrawing;
	}

bool CMainWindow::IsValidDrawing(db_drawing *pDrw) const
{
	bool bFound = false;
	if(!pDrw || !m_pCurDoc)
		return false;
	CIcadMultiDocTemplate *pDocTemplate = (CIcadMultiDocTemplate*)m_pCurDoc->GetDocTemplate();
	ASSERT( pDocTemplate );
	POSITION pos = pDocTemplate->GetFirstDocPosition();
	do
	{
		CIcadDoc *tmpDoc = (CIcadDoc*)pDocTemplate->GetNextDoc( pos );
		if( tmpDoc && (tmpDoc->m_dbDrawing == pDrw) )
		{
			bFound = true;
			break;
		}
	}
	while( pos );
	return bFound;
}

afx_msg int CMainWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// CG: This line was added by the Palette Support component
	m_pPalette = NULL;
	SDS_CreateOurColorMap();
	SetPalette(SDS_GetNewPalette());

	struct resbuf rb;
	rb.rbnext=NULL;

	if(CMDIFrameWnd::OnCreate(lpCreateStruct)==(-1))
	   return -1;

	bool showSplash = true;
	if ( showSplash )
  		CSplashWnd::ShowSplashScreen( this);

	//*** Add drag and drop capability
	ModifyStyleEx(0,WS_EX_ACCEPTFILES);
	//*** I am setting this variable for reference purposes
	CRect rectMainWnd;
	GetWindowRect(&rectMainWnd);
	//*** Create the History list object and the command string object
	m_pHistList = new CStringList();
	m_pCmdStr = new CString();
	m_pCmdStr->Empty();
	//*** Create the status bar
	m_StatusBar.Create(this);

	//*** Create the toolbars.
	//*** NOTE: EnableDocking for the frame window now gets called in LoadToolbars
	//***		to avoid an Assert in LoadBarState.  The Assert was caused by calling
	//***		EnableDocking for the frame window before creating the toolbars.
	BOOL bLoadReg = LoadToolbars();

	//*** Create the command bar.
	CRect rectCmdBar;
	rectCmdBar.SetRectEmpty();
	m_pCmdBar = new CCmdBar();
	m_pCmdBar->Create(this,&rectCmdBar);
	m_pCmdBar->m_pPromptWnd->m_pPromptWndList->m_pHistList=m_pHistList;
	m_pCmdBar->m_pPromptWnd->m_pPromptWndEdit->m_pCmdStr=m_pCmdStr;

	//*** Create the text screen but don't display unless specified in the registry.
	//*** Get the window position and size
	CRect rectTextScrn;
	SDS_getvar(NULL,DB_QWNDPTEXT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rectTextScrn.left=(long)rb.resval.rpoint[0];
	rectTextScrn.top=(long)rb.resval.rpoint[1];
	SDS_getvar(NULL,DB_QWNDSTEXT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rectTextScrn.right=(rectTextScrn.left+(long)rb.resval.rpoint[0]);
	rectTextScrn.bottom=(rectTextScrn.top+(long)rb.resval.rpoint[1]);
	//*** Set the default height if a height is not given
	if(rectTextScrn.IsRectNull())
	{
		GetWindowRect(rectTextScrn);
		rectTextScrn.OffsetRect(20,20);
	}
	//*** Create the text screen window
	m_pTextScrn = new CTextScreen();
	m_pTextScrn->m_pParent=this;
	// EBATECH
	//m_pTextScrn->Create(NULL,ResourceString(IDC_ICADMAIN_INTELLICAD_PROM_1, "IntelliCAD Prompt History" ),
	//	WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,rectTextScrn,this);
	CString csTitle;
	csTitle = ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" );
	csTitle = csTitle + " " + ResourceString(IDC_ICADMAIN_INTELLICAD_PROM_1, "Prompt History" );
	m_pTextScrn->Create(NULL,csTitle,WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,rectTextScrn,this);
	// ]-EBATECH
	m_pTextScrn->m_pPromptWnd->m_pPromptWndList->m_pHistList=m_pHistList;
	m_pTextScrn->m_pPromptWnd->m_pPromptWndEdit->m_pCmdStr=m_pCmdStr;
	
	CClientDC theMainDC( this );
	m_bMapToRGBColor = ( theMainDC.GetDeviceCaps( BITSPIXEL ) > 8 );
	
	// TECNOBIT(Todeschini) 2003.05.09 -[ Object proterties toolbar comboboxes
  CPtrList* pLst = ((CIcadToolBarMain*)m_pToolBarMain)->GetToolBarList();
  for(POSITION pos = pLst->GetHeadPosition(); pos != NULL; )
  {
    CIcadToolBar *pToolBar = (CIcadToolBar*)pLst->GetNext(pos);
    CIcadTBbutton* picButton;
    TBBUTTON		tbButton;
    for(int n = 0; n < pToolBar->GetToolBarCtrl().GetButtonCount(); n++) // Scan buttons of current toolbar
    {
      pToolBar->GetToolBarCtrl().GetButton(n, &tbButton);
      picButton = (CIcadTBbutton*)tbButton.dwData;
      // Replace buttons with comboboxes
	  // Added Proper IDs in "SetButtonInfo" function instead of "pToolBar->GetItemID(n)" By Mohan Nayak
      if(picButton && picButton->m_lflgVisibility & ICAD_MN_CTRL) 
      {
        CRect rect;
        switch(picButton->m_BMPIdSmColor)
        {
          case IDB_ICAD_LAYER_CTRL:
            pToolBar->SetButtonInfo(n,IDC_LAYER_COMBOBOX, TBBS_SEPARATOR, 200);
            pToolBar->GetItemRect(n, &rect);        
            if(!m_wndLayers.Create(rect, pToolBar))
            {
               TRACE0("Failed to create layer combo-box\n");
               return -1;
            }
            m_wndLayers.ShowWindow(SW_SHOW);
			m_wndLayers.AddToolTip();
            break;
          case IDB_ICAD_COLOR_CTRL:
            pToolBar->SetButtonInfo(n, IDC_COLOR_COMBOBOX, TBBS_SEPARATOR, 100);
            pToolBar->GetItemRect(n, &rect);        
            if(!m_wndColor.Create(rect, pToolBar))
            {
               TRACE0("Failed to create color combo-box\n");
               return -1;
            }
            m_wndColor.ShowWindow(SW_SHOW);
			m_wndColor.AddToolTip();
            break;
          case IDB_ICAD_LTYPE_CTRL:
            pToolBar->SetButtonInfo(n,IDC_LTYPE_COMBOBOX, TBBS_SEPARATOR, 200);
            pToolBar->GetItemRect(n, &rect);        
            if(!m_wndLType.Create(rect, pToolBar))
            {
               TRACE0("Failed to create color combo-box\n");
               return -1;
            }
            m_wndLType.ShowWindow(SW_SHOW);
			m_wndLType.AddToolTip();
            break;
		  case  IDB_ICAD_LWEIGHT_CTRL:
			pToolBar->SetButtonInfo(n, IDC_LWEIGHT_COMBOBOX, TBBS_SEPARATOR, 200);
            pToolBar->GetItemRect(n, &rect);        
            if(!m_wndLWeight.Create(rect, pToolBar)) 
            {
               TRACE0("Failed to create color combo-box\n");
               return -1;
            }

			// Assign the Display Scale Factor for m_wndLWeight Combo-box.
			m_wndLWeight.m_DisplayScaleFactor = ((CIcadApp*)AfxGetApp())->m_nLWDisplayScaleFactor;

            m_wndLWeight.ShowWindow(SW_SHOW);
			m_wndLWeight.AddToolTip();
            break;

		  case IDB_ICAD_CB_DIMSTYLE:
            pToolBar->SetButtonInfo(n, IDC_DIMSTYLE_COMBOBOX, TBBS_SEPARATOR, 100);
            pToolBar->GetItemRect(n, &rect);
			rect.bottom += 100 ;
            if(!m_wndDimstyle.Create(rect, pToolBar))
            {
               TRACE0("Failed to create color combo-box\n");
               return -1;
            }
            m_wndDimstyle.ShowWindow(SW_SHOW);
			m_wndDimstyle.AddToolTip(); 
			break;

          default:
            TRACE0("Not defined control\n");
        }
      }
    }
  }
  // ]- TECNOBIT


	if(bLoadReg)
	{
		SDS_getvar(NULL,DB_QWNDLMAIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint==2)
		{
			//*** Maximize the window before the toolbars are positioned so they don't
			//*** wrap to the non-maximized size.
			//BUG 1-56484, 55523: ICAD does not properly maximize itself.
			//Do NOT modify style to WS_MAXIMIZE, or ICAD will not maximize properly on WIN95 & 98.
			//ModifyStyle(0,WS_MAXIMIZE);
		}
		CString prefixProfileName = ResourceString(IDS_REGISTRY_PROFILES, "Profiles");
		CString profName;
		GetCurrentRegistryProfileName(profName.GetBuffer(IC_ACADBUF), IC_ACADBUF);
		profName.ReleaseBuffer();
		prefixProfileName += _T("\\") + profName;
	    	// LoadBarState() uses m_pszRegistryKey + m_pszProfileName (= "ITC\\IntelliCAD" and "")
		try
		{
			LoadBarState(prefixProfileName + _T("\\ControlBars\\ControlBar"/*DNT*/));
		}
		catch (...)
		{
		}
	}

	// Removed this is the old ODBC stuff.
	// SQLAllocEnv(&m_Env);

	// Needs Some Work yet!
	// Removed GPS stuff...
//	m_StartPoint[0]=0;
//	m_StartPoint[1]=0;
//	m_StartPoint[2]=0;
//	m_ConvStarted=FALSE;
//	m_DDEidInst=0;
	// Removed so we don't clash with DDE that is started from our applications.
	//StartDDEServer();

	m_StatusBar.UpdateCurLay();
	m_StatusBar.UpdateCurCol();
	m_StatusBar.UpdateCurLType();
	m_StatusBar.UpdateCurStyle(); // EBATECH
	m_StatusBar.UpdateCurDimSt(); // EBATECH
	m_StatusBar.UpdateCurLw();    // EBATECH(CNBR)

  // TECNOBIT(Todeschini) 2003.09.05 -[
	if (m_wndLayers.GetSafeHwnd())
		m_wndLayers.UpdateLayerList(); 
	
	if (m_wndLType.GetSafeHwnd())
		m_wndLType.UpdateLTypeList();
  // ]-

	//*** Load the accelerators.
	LoadAcceleratorTable();

	//*** Load the aliases.
	LoadAliases();

	//*** Set a default position for the Prompt Menu if needed.
	//*** Get the position of the prompt menu.
	SDS_getvar(NULL,DB_QWNDPPMENU,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(0.0==rb.resval.rpoint[0] && 0.0==rb.resval.rpoint[1])
	{
		//*** Make sure the registry is empty and the prompt menu wasnt positioned
		//*** at 0,0 on purpose.
		PreferenceKey		configKey(HKEY_CURRENT_USER, "Config", FALSE);
		HKEY hKey = configKey;
		BOOL bGetDefault=TRUE;
		if(hKey != NULL)
		{
			DWORD type;
			DWORD dwBuffer,dwBufferSize;
			dwBufferSize=sizeof(dwBuffer);
			if(ERROR_SUCCESS==RegQueryValueEx(hKey,ResourceString(IDC_ICADMAIN_SV_SIZE_2, "SV_SIZE" ),NULL,&type,(unsigned char*)&dwBuffer,&dwBufferSize) &&
				dwBuffer>0)
			{
				bGetDefault=FALSE;
			}
//			RegCloseKey(hKey);
		}
		if(bGetDefault)
		{
			CRect rectDeskTop;
			SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&rectDeskTop,0);
			rb.resval.rpoint[0]=(rectDeskTop.right - 175);
			rb.resval.rpoint[1]=(rectDeskTop.top + 140);
			SDS_setvar(NULL,DB_QWNDPPMENU,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		}
	}
	

    // Combo boxes should be created before "LoadBarState()" function ..
	//  Commented code is shifted above just before "LoadBarState" function
	 
/*   // TECNOBIT(Todeschini) 2003.05.09 -[ Object proterties toolbar comboboxes
  CPtrList* pLst = ((CIcadToolBarMain*)m_pToolBarMain)->GetToolBarList();
  for(POSITION pos = pLst->GetHeadPosition(); pos != NULL; )
  {
    CIcadToolBar *pToolBar = (CIcadToolBar*)pLst->GetNext(pos);
    CIcadTBbutton* picButton;
    TBBUTTON		tbButton;
    for(int n = 0; n < pToolBar->GetToolBarCtrl().GetButtonCount(); n++) // Scan buttons of current toolbar
    {
      pToolBar->GetToolBarCtrl().GetButton(n, &tbButton);
      picButton = (CIcadTBbutton*)tbButton.dwData;
      // Replace buttons with comboboxes
      if(picButton && picButton->m_lflgVisibility & ICAD_MN_CTRL) 
      {
        CRect rect;
        switch(picButton->m_BMPIdSmColor)
        {
          case IDB_ICAD_LAYER_CTRL:
            pToolBar->SetButtonInfo(n,IDC_LAYER_COMBOBOX, TBBS_SEPARATOR, 200);
            pToolBar->GetItemRect(n, &rect);        
            if(!m_wndLayers.Create(rect, pToolBar))
            {
               TRACE0("Failed to create layer combo-box\n");
               return -1;
            }
            m_wndLayers.ShowWindow(SW_SHOW);
			m_wndLayers.AddToolTip();
            break;
          case IDB_ICAD_COLOR_CTRL:
            pToolBar->SetButtonInfo(n, IDC_COLOR_COMBOBOX, TBBS_SEPARATOR, 100);
            pToolBar->GetItemRect(n, &rect);        
            if(!m_wndColor.Create(rect, pToolBar))
            {
               TRACE0("Failed to create color combo-box\n");
               return -1;
            }
            m_wndColor.ShowWindow(SW_SHOW);
			m_wndColor.AddToolTip();
            break;
          case IDB_ICAD_LTYPE_CTRL:
            pToolBar->SetButtonInfo(n,IDC_LTYPE_COMBOBOX, TBBS_SEPARATOR, 200);
            pToolBar->GetItemRect(n, &rect);        
            if(!m_wndLType.Create(rect, pToolBar))
            {
               TRACE0("Failed to create color combo-box\n");
               return -1;
            }
            m_wndLType.ShowWindow(SW_SHOW);
			m_wndLType.AddToolTip();
            break;
		  case  IDB_ICAD_LWEIGHT_CTRL:
			pToolBar->SetButtonInfo(n, IDC_LWEIGHT_COMBOBOX, TBBS_SEPARATOR, 200);
            pToolBar->GetItemRect(n, &rect);        
            if(!m_wndLWeight.Create(rect, pToolBar)) 
            {
               TRACE0("Failed to create color combo-box\n");
               return -1;
            }

			// Assign the Display Scale Factor for m_wndLWeight Combo-box.
			m_wndLWeight.m_DisplayScaleFactor = ((CIcadApp*)AfxGetApp())->m_nLWDisplayScaleFactor;

            m_wndLWeight.ShowWindow(SW_SHOW);
			m_wndLWeight.AddToolTip();
            break;

		  case IDB_ICAD_CB_DIMSTYLE:
            pToolBar->SetButtonInfo(n, pToolBar->GetItemID(n), TBBS_SEPARATOR, 100);
			pToolBar->GetItemRect(n, &rect);
			rect.bottom += 100 ;
            if(!m_wndDimstyle.Create(rect, pToolBar))
            {
               TRACE0("Failed to create color combo-box\n");
               return -1;
            }
            m_wndDimstyle.ShowWindow(SW_SHOW);
			m_wndDimstyle.AddToolTip(); 
			break;

          default:
            TRACE0("Not defined control\n");
        }
      }
    }
  }
*//// ]- TECNOBIT



	return 0;
}

BOOL CMainWindow::PreTranslateMessage(MSG* pMsg)
{
    // SystemMetrix(Maeda) 2001.08.24 -[ for sds callback func
    if (pMsg->message==WM_KEYDOWN || pMsg->message==WM_SYSKEYDOWN)  // Modify by SystemMetrix(Maeda) 2001.08.25
    {                                                               // WM_SYSKEYDOWN event (press for ALT or F10 key)
        if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBKEYDOWN, (void *)&pMsg->wParam, (void *)&pMsg->lParam, NULL))
        {
            return TRUE;
        }
    }
    if (pMsg->message==WM_KEYUP || pMsg->message==WM_SYSKEYUP)      // Modify by SystemMetrix(Maeda) 2001.08.25
    {                                                               // WM_SYSKEYUP event (up for ALT or F10 key)
        if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBKEYUP, (void *)&pMsg->wParam, (void *)&pMsg->lParam, NULL))
        {
            return TRUE;
        }
    }
    if (pMsg->message==WM_CHAR)
    {
        if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBCHAR, (void *)&pMsg->wParam, (void *)&pMsg->lParam, NULL))
        {
            return TRUE;
        }
    }
    // ]- SystemMetrix(Maeda) 2001.08.24

	m_bSentChar=FALSE;
	if(!m_bExplorerState && !m_bPrintPreview &&	!SDS_InsideGrread &&
		TranslateAccelerator(GetSafeHwnd(),m_hAccel,pMsg)==TRUE)
	{
		return TRUE;
	}
	if(pMsg->message==WM_CHAR && !m_bExplorerState && !m_bPrintPreview)
	{
//4M Bugzilla 77987 19/02/02->
/*
        if(m_pCurView && (TCHAR)pMsg->wParam == 27)
		{
			if(m_pCurView->m_pRealTime)
			{
				m_pCurView->KillTimer(RealTimeTimer_id);
				delete m_pCurView->m_pRealTime;
				m_pCurView->m_pRealTime = NULL;
			}
			m_pCurView->m_RTMotion = NO;
		}
*/
// We leave the behaviour for ZOOM_WHEEL intact
       if (m_pCurView && m_pCurView->GetRTMotion()==ZOOM_WHEEL){
         if(m_pCurView && (TCHAR)pMsg->wParam == 27 )
         {
            m_pCurView->DeleteRealTime();
            m_pCurView->SetRTMotion(NO);
         }
       }
//<-4M 19/02/02
        SDS_SendMessage(pMsg->message,pMsg->wParam,pMsg->lParam);
		m_bSentChar=TRUE;
	}
	else if(pMsg->message==WM_KEYUP)
	{
		switch(pMsg->wParam) {
			case 16: m_fIsShiftDown=0; break;
			case 17: m_fIsCtrlDown=0;  break;
			case 33:
			case 34:
			case 35:
			case 36:
			case 37:
			case 38:
			case 39:
			case 40:
			case 45:
			case 46:
				if(SDS_InsideGrread) SDS_SendMessage(WM_CHAR,pMsg->wParam,pMsg->lParam);
				break;
		}
	}
	else if(pMsg->message==WM_KEYDOWN)
	{
			 if(pMsg->wParam==16) m_fIsShiftDown=1;
		else if(pMsg->wParam==17) m_fIsCtrlDown=1;
	}
	else if (pMsg->message==WT_PACKET && m_pTablet)
	{

		m_pTablet->ProcessDigitizerInput (pMsg->wParam, pMsg->lParam);
	}

	return CMDIFrameWnd::PreTranslateMessage(pMsg);
}



HMENU CMainWindow::GetWindowMenuPopup(HMENU hMenuBar)
{
	CMenu mnu;
	mnu.Attach(hMenuBar);
	int nItems = mnu.GetMenuItemCount();
	if(nItems<=0)
	{
		return NULL;
	}
	CString str;
	while(nItems--)
	{
		mnu.GetMenuString(nItems,str,MF_BYPOSITION);
#ifdef BUILD_WITH_RASTER_SUPPORT
		if(str.CompareNoCase(ResourceString(ICADMENUS_20, "&Window" ))==0 ||
			str.CompareNoCase(ResourceString(IDC_ICADMAIN_WINDOW_9, "Window" ))==0)
#else
		if(str.CompareNoCase(ResourceString(ICADMENUS_19, "&Window" ))==0 ||
			str.CompareNoCase(ResourceString(IDC_ICADMAIN_WINDOW_9, "Window" ))==0)
#endif //BUILD_WITH_RASTER_SUPPORT
		{
			CMenu* pWinMenu = mnu.GetSubMenu(nItems);
			mnu.Detach();
			return pWinMenu->GetSafeHmenu();
		}
	}
	mnu.Detach();
	return NULL;
}

afx_msg void CMainWindow::OnMove(int cx, int cy)
{
	SetMovePos(this,0);
}

void CMainWindow::SetMovePos(CWnd* pWnd, int nMode)
//*** This function gets called from several locations to set the Icad variable:
//*** nMode: 0 - CMainWindow
//***		 1 - CTextScreen
//***		 2 - CCmdBar
{
	if(m_nIsClosing) return;
	//*** Set the Icad variable.
	struct resbuf rb;
	CRect rect;
	pWnd->GetWindowRect(rect);
	rb.restype=SDS_RTPOINT;
	rb.resval.rpoint[0]=(double)rect.left;
	rb.resval.rpoint[1]=(double)rect.top;
	//*** Set the setvar strings according to the mode.
	if(nMode==0)
		{
		SDS_setvar(NULL,DB_QWNDPMAIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		rb.resval.rpoint[0]=(double)rect.Width();
		rb.resval.rpoint[1]=(double)rect.Height();
		SDS_setvar(NULL,DB_QWNDSMAIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		}
	else if(nMode==1) SDS_setvar(NULL,DB_QWNDPTEXT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	else if(nMode==2) SDS_setvar(NULL,DB_QWNDPCMD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
}

afx_msg void CMainWindow::OnSize(UINT nType, int cx, int cy)
{
	SetSize(nType,this,0);
	if(nType==0 || nType==SIZE_MAXIMIZED)
	{
		if(m_pCmdBar!=NULL) m_pCmdBar->m_nDockedWidth=cx+4;
	}
	CMDIFrameWnd::OnSize(nType,cx,cy);
	return;
}

void CMainWindow::SetSize(UINT nType, CWnd* pWnd, int nMode)
//*** This function gets called from several locations to set Icad variables:
//*** nMode: 0 - CMainWindow
//***		 1 - CTextScreen
//***
{
	if(m_nIsClosing) return;
	//*** Set the setvar strings according to the mode.
	CString strLoc,strPos,strSize;
	if(nMode==0)
	{
		strLoc ="WNDLMAIN"/*DNT*/;
		strPos ="WNDPMAIN"/*DNT*/;
		strSize="WNDSMAIN"/*DNT*/;
	}
	else if(nMode==1)
	{
		strLoc ="WNDLTEXT"/*DNT*/;
		strPos ="WNDPTEXT"/*DNT*/;
		strSize="WNDSTEXT"/*DNT*/;
	}
	struct resbuf rb;
	if(nType==0)
	{
		CRect rect;
		pWnd->GetWindowRect(rect);
		rb.restype=SDS_RTPOINT;
		rb.resval.rpoint[0]=(double)(rect.right-rect.left);
		rb.resval.rpoint[1]=(double)(rect.bottom-rect.top);
		SDS_setvar(strSize.GetBuffer(0),-1,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		//*** Set location to normal.
		rb.restype=SDS_RTSHORT;
		if(nMode==0) rb.resval.rint=0;
		else rb.resval.rint=1;
		SDS_setvar(strLoc.GetBuffer(0),-1,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	}
	else if(nType==SIZE_MINIMIZED)
	{
		rb.restype=SDS_RTSHORT;
		if(nMode==0) rb.resval.rint=1;
		else rb.resval.rint=2;
		SDS_setvar(strLoc.GetBuffer(0),-1,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		//*** Set the position back to the restore position.
		WINDOWPLACEMENT wp;
		pWnd->GetWindowPlacement(&wp);
		rb.restype=SDS_RTPOINT;
		rb.resval.rpoint[0]=(double)wp.rcNormalPosition.left;
		rb.resval.rpoint[1]=(double)wp.rcNormalPosition.top;
		SDS_setvar(strPos.GetBuffer(0),-1,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		// Save the restored size too.
		rb.resval.rpoint[0]=(double)(wp.rcNormalPosition.right - wp.rcNormalPosition.left);
		rb.resval.rpoint[1]=(double)(wp.rcNormalPosition.bottom - wp.rcNormalPosition.top);
		SDS_setvar(strSize.GetBuffer(0),-1,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	}
	else if(nType==SIZE_MAXIMIZED)
	{
		rb.restype=SDS_RTSHORT;
		if(nMode==0) rb.resval.rint=2;
		else rb.resval.rint=3;
		SDS_setvar(strLoc.GetBuffer(0),-1,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		/*
		BUG: WNDPMAIN is not updated when ICAD is maximized.
		We should not set WNDPMAIN to the restore position here because OnMove() is called before OnSize(), when
		ICAD is being maximized or minimized. Inside OnMove(), we set WNDSMAIN and WNDPMAIN to
		the size and the position of the maximized window.
		*/
	}
}

void CMainWindow::OnContextMenu(CWnd* pWnd, CPoint pos)
{
	CRect rect;
	GetClientRect(rect);
	CPoint clientpt(pos);
	ScreenToClient(&clientpt);
	if(clientpt.x>rect.left && clientpt.x<rect.right && clientpt.y>rect.top && clientpt.y<rect.bottom)
	{
		ContextMenu(pos);
		return;
	}
	CMDIFrameWnd::OnContextMenu(pWnd,pos);
	return;
}

afx_msg void CMainWindow::OnClose(void)
{
	//*** Don't shut down if in the middle of any in-place active session.
	CIcadApp* pApp = (CIcadApp*)AfxGetApp();
	if(pApp->HasInPlaceActiveItem())
	{
		sds_alert(ResourceString(IDC_ICADMAIN_PLEASE_SHUT_DO_17, "Please shut down the in-place editing session before exiting IntelliCAD." ));
		return;
	}

	if (g_Vba6Installed)
	{
		// fire the IWorkspace::BeforeExit(VARIANT_BOOL) event and determine if the user wants to Exit
		CComVariant ret(false);
		HRESULT hr = ApcFireEvent(GetApp()->m_spWorkspace, DIID__IcadWorkspaceEvents, DISPID_WS_BEFOREEXIT, NULL, &ret);
		ASSERT(SUCCEEDED(hr));

		// A return value of 'true' means 'Cancel' the Exit
		const CComVariant vTrue(true);
		if (ret == vTrue)
			return;

		// Shut down APC for VBA
		// if fMacrosTerminated is set to FALSE by WmClose (passed by reference), then
		// you should not shut down, because VBA is not yet terminated.  APC will
		// repost the WM_CLOSE message on your behalf once VBA has unwound itself
		// off of the stack.
		//
		BOOL fMacrosTerminated = TRUE;
		GetApp()->ApcHost.WmClose(fMacrosTerminated);
		if (!fMacrosTerminated)
            		return;    
    }
    else
    {
	    HRESULT hr = GetApp()->comAddinManager ().fireWorkspaceBeforeExit(GetApp()->m_spWorkspace);
	    ASSERT(SUCCEEDED(hr));

    } //endif g_Vba6Installed

	/*
	While ICAD is running, WNDPMAIN and WNDSMAIN contain the current size and
	position of ICAD main window. These values are, however, used to initialize the window next time
	ICAD is running. Then, we maximize or minimize the window if necessary.
	So, save the size and the position of the restored window to WNDSMAIN and WNDPMAIN rather than those of
	maximized window.
	*/
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	struct sds_resbuf rb;
	rb.restype=SDS_RTPOINT;
	rb.resval.rpoint[0] = wp.rcNormalPosition.left;
	rb.resval.rpoint[1] = wp.rcNormalPosition.top;
	SDS_setvar(NULL,DB_QWNDPMAIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES, 0);

	rb.resval.rpoint[0] = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	rb.resval.rpoint[1] = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
	SDS_setvar(NULL,DB_QWNDSMAIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES, 0);

	rb.restype=SDS_RTSHORT;

	// Fix for BRX 1086.
	// If ICAD is closed when minimised, it then starts up in the minimised state.
	// Incorrect behavior occurs if an IntelliCAD session is Minimized, then closed (while minimized).
	// The next time the application is started, it is started in the minimized position.  This can be misleading.	If the
	// minimized application is not noticed, the user may try to start the application again (and again).  Each session will be
	// started minimized unless a session is maximized and closed while maximized.

	/*
	if(wp.showCmd == SW_SHOWNORMAL)
		rb.resval.rint=0;
	else if(wp.showCmd == SW_SHOWMINIMIZED)
		rb.resval.rint=1;
	else if(wp.showCmd == SW_SHOWMAXIMIZED)
		rb.resval.rint=2;
	*/

	// If the main window is closed minimized or maximized, then save the state as MAXIMIZE (rb.resval.rint = 2).
	// If closed at NORMAL, then save the state as NORMAL (rb.resval.rint = 0).
	rb.resval.rint = wp.showCmd == SW_SHOWNORMAL ? 0 : 2;
	// End of fix for BRX 1086.

	SDS_setvar(NULL,DB_QWNDLMAIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES, 0);

	SDS_SendMessage(WM_CHAR,27,0);
	SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C_EXIT"/*DNT*/);
	return;
}

afx_msg void CMainWindow::OnKillFocus( CWnd* pOldWnd )
{
	m_fHasFocus=0;
	CMDIFrameWnd::OnKillFocus(pOldWnd);
	return;
}

afx_msg void CMainWindow::OnSetFocus( CWnd* pNewWnd )
{
	m_fHasFocus=1;
	CMDIFrameWnd::OnSetFocus(pNewWnd);
	return;
}

BOOL CMainWindow::DestroyWindow()
{
	// AG: calling this method we invoke CActLibraryDocTemplate::CloseAllDocuments()
	// which is responsible for releasing of Workspace object
	// we have to call it here because it's the only place in case of InPlaceActivation
	GetApp()->CloseAllDocuments(FALSE);

	if (g_Vba6Installed)
	{
		//Notify APC of the WM_DESTROY
		GetApp()->ApcHost.WmDestroy();
	} //endif g_Vba6Installed

		// destroy help window
	if ( m_hInstHTMLHelp )
		{
		IcadHelp( NULL, ICADHELP_QUIT, 0);

		/*D.G.*/// I'd offer the following way but it doesn't work
		// (some systems crash (Windows 2000 are the most 'crashable') while Freeing the Library
		// because HTML Help runs in another thread than our main one).
		// So, comment it out and don't FreeLibrary (let it been freed by the system).
/*		FARPROC	entryDllCanUnloadNow = GetProcAddress(m_hInstHTMLHelp, "DllCanUnloadNow");
		if(entryDllCanUnloadNow)
		{
			while((*entryDllCanUnloadNow)() != S_OK)
				;
			AfxFreeLibrary(m_hInstHTMLHelp);
		}*/

		m_hInstHTMLHelp = NULL;
		}

	m_nIsClosing=1; //*** Set the closing flag, also set in CloseApp().
	//*** Save the toolbars to the registry.
	((CIcadToolBarMain*)m_pToolBarMain)->SaveToRegistry();
	//*** Delete the toolbars
	delete m_pToolBarMain;
	m_pToolBarMain=NULL;

	//	Removed Save Aliases/Accelerators to registry
	//	Initial save now performed when defaults loaded

	//*** Delete the ICACCEL structures from the accelerator list.
	DeleteAccelList();
	//*** Delete the accelerator table
	DestroyAcceleratorTable(m_hAccel);

	//*** Save the menu to the registry and delete the list.
	if(NULL!=m_pMenuMain)
	{
		CIcadMenu* pMenu = m_pMenuMain->GetDefaultMenu();
		if(NULL!=pMenu)
		{
			pMenu->SaveToReg();
		}
		delete m_pMenuMain;
		m_pMenuMain=NULL;
	}

	//Save the Tablet to the registry
	if (m_pTablet)
		{
		m_pTablet->SaveToRegistry();
		}

	if (m_paths)
		m_paths->SaveToRegistry();

	// *** Reset the search path (if one was passed with the /s"path;path").
	// *** Make sure this is done before SDS_Exit(), so we write the old path to the registry.
	if(!((CIcadApp*)AfxGetApp())->m_cmdInfo.m_strOverridePath.IsEmpty())
	{
		struct resbuf rb;
		rb.restype=RTSTR;
		rb.resval.rstring=(char *)(LPCTSTR)((CIcadApp*)AfxGetApp())->m_cmdInfo.m_strOverridePath;
		SDS_setvar(NULL,DB_QSRCHPATH,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	}

	//Unload the com addins first, so that they can undef any defined functions
	if (g_Vba6Installed)
	{
		GetApp()->ApcHostAddIns.Destroy();
	}

	GetApp()->comAddinManager ().unload ();

	//rom
	try
	{
	SDS_Exit();
	}
	catch(...)
	{
	}

	//*** Destroy the text window
	m_pTextScrn->DestroyWindow();

	//*** Make sure all our registry changes are flushed.
	RegFlushKey(HKEY_CURRENT_USER);

	//*** Delete the customize object.
	if(m_pIcadCustom!=NULL) delete m_pIcadCustom;
	m_bAlreadyDestroyed=TRUE; //*** Set this flag because sometimes OLE destroys the window for us.
	BOOL bRet = FALSE;
	try
	{
	bRet = CFrameWnd::DestroyWindow();
	}
	catch(...)
	{
	}
	return bRet;
}

BOOL CMainWindow::CloseApp(void)
{
//*** This function returns FALSE only when CloseAllDocuments() destroys CMainWindow (this).
	//*** Most of this code was copied from CFrameWnd::OnClose();
	if(m_bPrintPreview)
	{
		//*** Shut down print preview
		SendMessage(WM_COMMAND,AFX_ID_PREVIEW_CLOSE,0);
	}

	//*** Don't shut down if in the middle of any in-place active session.
	CIcadApp* pApp = (CIcadApp*)AfxGetApp();
	if(pApp->HasInPlaceActiveItem())
	{
		sds_alert(ResourceString(IDC_ICADMAIN_PLEASE_SHUT_DO_17, "Please shut down the in-place editing session before exiting IntelliCAD." ));
		return TRUE;
	}

	// Note: only queries the active document
	CDocument* pDocument = GetActiveDocument();
	if (pDocument != NULL && !pDocument->CanCloseFrame(this))
	{
		// document can't close right now -- don't close it
		return TRUE;
	}
	if (pApp->m_pMainWnd == this)
	{
		// attempt to save all documents
		if (pDocument == NULL && !pApp->SaveAllModified())
			return TRUE;	 // don't close it


		m_nIsClosing=1; //*** Set the closing flag.


		//*** Set the location of the MDI window
		BOOL bMaxFlg;
		CWnd* pMDIWnd = MDIGetActive(&bMaxFlg);
		struct resbuf rb;
		rb.rbnext=NULL;
		rb.restype=SDS_RTSHORT;
		if(pMDIWnd==NULL) {
			rb.resval.rint=0;
		} else {
			if(bMaxFlg==TRUE) rb.resval.rint=2;
			else rb.resval.rint=1;
		}
		SDS_setvar(NULL,DB_QWNDLMDI,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

		//*** Hide the application's windows before closing down.
		pApp->HideApplication();

		// close all documents first
		pApp->CloseAllDocuments(FALSE);

		// don't exit if there are outstanding component objects
		if(!AfxOleCanExitApp())
		{
			// take user out of control of the app
			AfxOleSetUserCtrl(FALSE);

			// don't destroy the main window and close down just yet
			//	(there are outstanding component (OLE) objects)
			m_nIsClosing=0;
			return TRUE;
		}

		//*** There are cases where destroying the documents may destroy the
		//*** main window of the application.  Return FALSE from here to tell the
		//*** calling function to return because "this" has been deleted.
		if(!afxContextIsDLL && pApp->m_pMainWnd==NULL)
		{
			return FALSE;
		}
	}

	// detect the case that this is the last frame on the document and
	// shut down with OnCloseDocument instead.
	if (pDocument != NULL && pDocument->m_bAutoDelete)
	{
		BOOL bOtherFrame = FALSE;
		POSITION pos = pDocument->GetFirstViewPosition();
		while (pos != NULL)
		{
			CView* pView = pDocument->GetNextView(pos);
			ASSERT_VALID(pView);
			if (pView->GetParentFrame() != this)
			{
				bOtherFrame = TRUE;
				break;
			}
		}
		if (!bOtherFrame)
		{
			pDocument->OnCloseDocument();
			return TRUE;
		}

		// allow the document to cleanup before the window is destroyed
		pDocument->PreCloseFrame(this);
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////
// Help
//
bool
CMainWindow::IcadHelp( LPCTSTR pszFile, UINT uCommand, DWORD dwData )
	{

		char ext[IC_ACADNMLEN];
		CString	command( "JumpID(\""/*DNT*/);
	//	LPTSTR	doubleSlash;

		// KLUGE alert.
		// I (mb) added (dwData <= HLP_TCOPTIONS) to prevent a crash when dwData == 200
		// DWF help id. This may need to be revisited.
//		if (dwData <= HLP_TCOPTIONS)
		// Modified the Function Sachin Dange to implement the suggestions of Denis
	
		if ((LPCTSTR)dwData && (dwData > HLP_TCOPTIONS) && _tcsnicmp((LPCTSTR)dwData, "C:"/*DNT*/, 2) == 0)
		{
			class commandAtomObj * pCommandAtomObj = NULL;
			struct resbuf *pHelpFunData = NULL;
			char functionName[IC_ACADBUF];
			strcpy((char *)functionName, (char *)dwData);	  // 'C:FUNCTION'
			strcpy((char *)functionName, &(functionName[2])); //  FUNCTION; skip 'C:'
			pCommandAtomObj = lsp_findatom(functionName, 4);
			if(pCommandAtomObj)
			{
				if(pCommandAtomObj->m_szHelpFile)
				{
					pszFile = pCommandAtomObj->m_szHelpFile ;
					strcpy((char *)dwData, (char *) pCommandAtomObj->m_szContextID);
					uCommand = pCommandAtomObj->m_nMapNumber;
				}
			}

		}

	ic_getext((char*)pszFile, ext);

	// check if the file is Windows Help (.hlp) file.
	if(strisame(ext,".hlp"/*DNT*/)) 
		return !!::WinHelp( SDS_CMainWindow->GetSafeHwnd(), pszFile, uCommand, dwData);

		// use the HTML help control if found, otherwise default to WinHelp
		#ifdef	BUILD_WITH_HTMLHELP
	// load the HTML help OCX and get a ptr to the HTMLHelp() function.
	if ( ordinalHTMLhelp && !m_hInstHTMLHelp )
		m_hInstHTMLHelp = AfxLoadLibrary( "HHCTRL.OCX" );	/*D.G.*/// Use AfxLoadLibrary instead of LoadLibrary.

	typedef HWND (WINAPI *PFNHTMLHELP)(HWND hWndCaller, LPCTSTR pszFile, UINT uCommand, DWORD dwData);
	PFNHTMLHELP pfnHelp = m_hInstHTMLHelp ? ( PFNHTMLHELP ) GetProcAddress( m_hInstHTMLHelp, ordinalHTMLhelp ) :
											NULL;
	if ( pfnHelp )
		{
		HH_AKLINK link;

		if ( uCommand EQ ICADHELP_PARTIALKEY || uCommand EQ ICADHELP_KEY)
			{
			link.cbStruct = 	sizeof(HH_AKLINK) ;
			link.fReserved =	FALSE ;
			link.pszKeywords =	(LPCTSTR)dwData ;
			link.pszUrl =		NULL ;
			link.pszMsgText =	NULL ;
			link.pszMsgTitle =	NULL ;
			link.pszWindow =	NULL ;
			link.fIndexOnFail = TRUE ;

			dwData = (DWORD)&link;
			}

		if(uCommand == ICADHELP_QUIT)		/*D.G.*/// They say the 2st two pars must be NULL in this case.
			return !!pfnHelp(NULL, NULL, uCommand, dwData);
		else
			return !!pfnHelp( SDS_CMainWindow->GetSafeHwnd(), pszFile, uCommand, dwData );
		}
	else
		#endif
		
				return false;

	}


/////////////////////////////////////////////////////////////////////
// Command line/Text screen functions
//
void CMainWindow::ShowCmdBars(void)
{
	SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"'_CMDBAR;T"/*DNT*/);
}

void CMainWindow::UpdateCmdBars(CCmdUI* pCmdUI){
	pCmdUI->SetCheck(m_pCmdBar->IsWindowVisible ());
}

int
CMainWindow::PrintOnCommand(char* pStr)
{
	if(!pStr)
		return RTNORM;
	if(!m_pCmdBar || m_nIsClosing == 1)
		return RTERROR;

	bool			bIsCmdVisible;
	unsigned char*	pChar = (unsigned char*)pStr;
	resbuf			rb;

	SDS_getvar(NULL, DB_QWNDLCMD, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);	// Values are 0-3:	0 is Hidden, 1 is Floating
	bIsCmdVisible = !!rb.resval.rint;

   	for( ; *pChar; pChar = _mbsinc(pChar))
	{
		switch(*pChar)
		{
		case '\n' :
			if(bIsCmdVisible)
				m_pCmdBar->m_pPromptWnd->m_pPromptWndEdit->UpdateText(CString(ResourceString(IDC_ICADMAIN__3, "")));
			else
				m_StatusBar.TextOut(ResourceString(IDC_ICADMAIN__3, ""));
			m_pTextScrn->m_pPromptWnd->m_pPromptWndEdit->UpdateText(CString(ResourceString(IDC_ICADMAIN__3, "")));
			AddToList(*m_pCmdStr);
			m_pCmdStr->Empty();
			break;

		case '\r' :
			m_pCmdStr->Empty();
			break;

		case '\b' :
			if(!m_pCmdStr->IsEmpty())
			{
				char*	pCmdStr = m_pCmdStr->GetBuffer(m_pCmdStr->GetLength());
				/*D.G.*/// DBCS enabled.
				*_mbsdec((unsigned char*)pCmdStr, (unsigned char*)(pCmdStr + m_pCmdStr->GetLength())) = '\0';
				m_pCmdStr->ReleaseBuffer();
			}
			break;

		case '\t' :
			*m_pCmdStr += ResourceString(IDC_ICADMAIN______20, "    ");
			break;

		default   :
			*m_pCmdStr += (char)*pChar;
			if(_ismbblead((unsigned int)*pChar))
				*m_pCmdStr += (char)*(pChar + 1);

		}	// switch(*pChar)
	}	// for( thru the string

	if(bIsCmdVisible)
		m_pCmdBar->m_pPromptWnd->m_pPromptWndEdit->UpdateText(*m_pCmdStr);
	else
		m_StatusBar.TextOut((LPCTSTR)*m_pCmdStr);

	m_pTextScrn->m_pPromptWnd->m_pPromptWndEdit->UpdateText(*m_pCmdStr);

	return RTNORM;
}

void CMainWindow::DoPrintDirect()
{
	//*** This function usually gets called only when Print is
	//*** specifies as a command line argument.
	CIcadApp* pApp = (CIcadApp*)AfxGetApp();
	if(NULL==pApp)
	{
		return;
	}
	if(!pApp->IcadPrintDlg(
		PRINTDLG_DISPLAY | PRINTDLG_PREVIEW | PRINTDLG_PRINTDIRECT |
		PRINTDLG_PRINTING))
	{
		return;
	}
	GetIcadView()->FilePrint(FALSE);
}

void CMainWindow::DoPrint()
{
	//*** This function usually gets called only when the Print button is
	//*** picked from the Print Preview toolbar.  And the function above.
	CIcadApp* pApp = (CIcadApp*)AfxGetApp();
	if(NULL==pApp)
	{
		return;
	}
	if(!pApp->IcadPrintDlg(PRINTDLG_DISPLAY | PRINTDLG_PREVIEW | PRINTDLG_PRINTING))
	{
		return;
	}
	GetIcadView()->FilePrint(FALSE);
}

void
CMainWindow::AddToList(CString& str)
{
	// Add the string to the linked list
	m_pHistList->AddTail(str);

	// Add the string to the list-box
	m_pCmdBar->m_pPromptWnd->m_pPromptWndList->AddString(str);
	m_pTextScrn->m_pPromptWnd->m_pPromptWndList->AddString(str);

	// Delete the first string in the list if it is getting too large.
	if(m_pHistList->GetCount() > m_pCmdBar->m_pPromptWnd->m_pPromptWndList->m_nMaxHistLines)
		m_pHistList->RemoveHead();

	return;
}

void CMainWindow::ShowStatusBar(void){
	struct resbuf rb;
	rb.restype=SDS_RTSHORT;
	if(FALSE==m_StatusBar.IsWindowVisible()) rb.resval.rint=1;
	else rb.resval.rint=0;
	sds_setvar("WNDLSTAT"/*DNT*/,&rb);
}

void CMainWindow::UpdateStatusBar(CCmdUI* pCmdUI){
	pCmdUI->SetCheck(m_StatusBar.IsWindowVisible ());
}

BOOL CMainWindow::IcadStartDao(void) {

	if(m_bDaoRunning) return(m_bDaoRunning);

	m_bDaoRunning=TRUE;
	try
	{
//		m_CdbWorkSpace.Append();
		m_CdbWorkSpace.Open();
	}
	catch( CDaoException* e )
	{
		 AfxMessageBox(
				e->m_pErrorInfo->m_strDescription,
				MB_ICONEXCLAMATION );
		m_bDaoRunning=FALSE;
		e->Delete();
	}
	return(m_bDaoRunning);
}

CIcadView* CMainWindow::GetIcadView(void)
{
//	CIcadChildWnd* pChild = (CIcadChildWnd*)MDIGetActive();
//	if(pChild!=NULL)
//	{
//		 return (CIcadView*)pChild->GetActiveView();
//	}
//	return NULL;
	return m_pCurView;
}

void CMainWindow::OnDropFiles(HDROP hDropInfo)
{
	UINT ct,nStrSize,nFiles;
	char* ps1;
	HDROP hDrop;
	::SetForegroundWindow(this->m_hWnd);//EBATECH(FUTA) 2001-02-01 ICAD Window activate

//	extern int SDS_AtCmdLine;
//	if(!SDS_AtCmdLine) return;

	if(NULL==hDropInfo)
	{
		OpenClipboard();
		if(NULL==(hDrop=(HDROP)GetClipboardData(CF_HDROP)))
		{
			sds_alert(ResourceString(IDC_ICADMAIN_ERROR__INVALID_22, "ERROR: Invalid file type." ));
			CloseClipboard();
			return;
		}
	}
	else hDrop=hDropInfo;

	nFiles=DragQueryFile(hDrop,0xFFFFFFFF,NULL,0);
	for(ct=0; ct<nFiles; ct++)
	{
		if((nStrSize=DragQueryFile(hDrop,ct,NULL,0))<=0) continue;
		if((ps1 = new char[++nStrSize])==NULL)
		{
			sds_alert(ResourceString(IDC_ICADMAIN_ERROR__INSUFFI_23, "ERROR: Insufficient memory." ));
			return;
		}
		DragQueryFile(hDrop,ct,ps1,nStrSize);

		char szPath[IC_ACADBUF];
		if(sds_findfile(ps1,szPath)!=RTNORM ||
			!OpenFileUnknown(szPath,TRUE))
		{
			CString str;
			str.Format(ResourceString(IDC_ICADMAIN_ERROR____24, "ERROR: \"%s\" is not a valid file." ),ps1);
			sds_alert(str);
		}
		delete ps1;
	}
	if(NULL==hDropInfo)
	{
		//*** Clean-up the clipboard
		EmptyClipboard();
		CloseClipboard();
	}
	if(NULL!=m_pOpenOleList)
	{
		//*** If files are dragged and dropped, we want to save them in a list
		//*** and create the ole objects last.
		POSITION pos = m_pOpenOleList->GetHeadPosition();
		CString strPath;
		while(NULL!=pos)
		{
			strPath = m_pOpenOleList->GetNext(pos);
			if(!OpenFileUnknown(strPath.GetBuffer(0),FALSE))
			{
				CString str;
				str.Format(ResourceString(IDC_ICADMAIN_ERROR____24, "ERROR: \"%s\" is not a valid file." ),strPath);
				sds_alert(str);
			}
		}
		delete m_pOpenOleList;
		m_pOpenOleList=NULL;
	}
}

BOOL CMainWindow::OpenFileUnknown(LPSTR pszFileName, BOOL bOleAware)
{
//*** Build a menu string based on the file type to open it.
	char* cp1;
	if(NULL==pszFileName)
	{
		return FALSE;
	}
	//*** If the file has no extension, use .dwg as the default.
	char fname[IC_PATHSIZE+IC_FILESIZE];
	strncpy(fname,pszFileName,sizeof(fname)-1);
	char* pFileNameOnly;
	if(NULL==(pFileNameOnly=strrchr(fname,'\\')))
	{
		pFileNameOnly=fname;
	}
	if(NULL==(cp1=strrchr(pFileNameOnly,'.'/*DNT*/)))
	{
		ic_setext(fname,"dwg"/*DNT*/);
		cp1=strrchr(fname,'.');
	}
	//*** Convert all the back-slashes to forward-slashes
	char* cp2;
	for(cp2=fname; *cp2!=0; cp2++)
	{
		if(*cp2=='\\'/*DNT*/) *cp2='/'/*DNT*/;
		// This is a fix to convert spaces to a char
		// that will go to the command line (|).
		if(*cp2==' '/*DNT*/)  *cp2='|'/*DNT*/;
		// EBATECH(R.Arayashiki)-[MBCS support
		if ((_MBC_LEAD ==_mbbtype((unsigned char)*cp2,0)) &&
			  (_MBC_TRAIL==_mbbtype((unsigned char)*(cp2+1),1)))
			cp2 ++;
		//]-EBATECH
	}

	int nStrSize = strlen(fname)+30;	// should use CString
	LPTSTR ps2=new char[nStrSize+1];
/*
	if(strisame(".BMP"DNT,cp1))
	{
		nStrSize+=14;
		ps2=new char[nStrSize+1];
		sprintf(ps2,ResourceString(IDC_ICADMAIN__C_COPENIMAGE__28, "^C^C_OPENIMAGE;%s" ),fname);
	}
*/
	if(stricmp(".DWG"/*DNT*/,cp1)==0)
	{
		struct resbuf rb;
		SDS_getvar(NULL,DB_QDRAGOPEN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(!rb.resval.rint) {
			sprintf(ps2,"^C^C^C_INSERT;%s"/*DNT*/,fname);
		} else {
			sprintf(ps2,"^C^C^C_OPEN;%s"/*DNT*/,fname);
		}
	} else if(strisame(".DXF"/*DNT*/,cp1))
	{
		sprintf(ps2,"^C^C^C_DXFIN;%s"/*DNT*/,fname);
	}
	else if(strisame(".MNU"/*DNT*/,cp1) || strisame(".ICM"/*DNT*/,cp1))
	{
		sprintf(ps2,"^C^C^C_MENU;%s"/*DNT*/,fname);
	}
	else if(strisame(".LSP"/*DNT*/,cp1))
	{
		sprintf(ps2,"^C^C^C(LOAD \"%s\")"/*DNT*/,fname);
	}
	else if(strisame(".DLL"/*DNT*/,cp1))
	{
		sprintf(ps2,"^C^C^C(XLOAD \"%s\")"/*DNT*/,fname);
	}
	else if(strisame(".SCR"/*DNT*/,cp1) || strisame(".MCR"/*DNT*/,cp1))
	{
		sprintf(ps2,"^C^C^C_SCRIPT;%s"/*DNT*/,fname);
	}
	else if(strisame(".SLD"/*DNT*/,cp1)/* || strisame(".WMF",cp1) || strisame(".EMF",cp1)*/)
	{
		sprintf(ps2,"^C^C^C_VSLIDE;%s"/*DNT*/,fname);
	}
	else
	{
		if(bOleAware)
		{
			//*** If files are dragged and dropped, we want to save them in a list
			//*** and create the ole objects last.	The calling funcion needs to know
			//*** what to do with this list.
			if(NULL==m_pOpenOleList)
			{
				m_pOpenOleList = new CStringList();
			}
			m_pOpenOleList->AddTail(fname);
			return TRUE;
		}
		sprintf(ps2,"^C^C^C_INSERTOBJ;%s"/*DNT*/,fname);
	}

	if(ps2!=NULL)
	{
		//*** sending as a menu string.
//		  char* t = "^C^CSCRIPT;d:/mb.scr";SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)t);
		SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)ps2);
		delete ps2;
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

void CMainWindow::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CMDIFrameWnd::OnActivate(nState,pWndOther,bMinimized);

	if (g_Vba6Installed)
	{
		//Activate VBA
		GetApp()->ApcHost.WmActivate(nState);
	} //endif g_Vba6Installed

}

void CMainWindow::OnEnable(BOOL bEnable)
{
	CMDIFrameWnd::OnEnable(bEnable);

	if (g_Vba6Installed)
	{
		//Enable VBA
		GetApp()->ApcHost.WmEnable(bEnable);
	} //endif g_Vba6Installed

}

void CMainWindow::OnActivateApp(BOOL bActive, HTASK hTask)
{
	if(NULL==m_pCurDoc || !m_pCurDoc->IsEmbedded() || bActive)
	{
		//*** This flag is handled in IcadIpFrame when we are in-place.
		m_bIsAppActive=bActive;
	}

	if ( m_pTablet && m_pTablet->m_pWinTabTab)
		m_pTablet->m_pWinTabTab->Overlap (bActive);
	if ( m_pTablet && m_pTablet->m_pWinTabSys)
		m_pTablet->m_pWinTabSys->Overlap (bActive);
	if ( m_pTablet && m_pTablet->m_pWinTabCal)
		m_pTablet->m_pWinTabCal->Overlap (bActive);

	CMDIFrameWnd::OnActivateApp(bActive,hTask);
}


	// this is just a debug hook
BOOL
CMainWindow:: OnCommand(WPARAM wParam, LPARAM lParam)
	{
	WORD	notifyCode = HIWORD( wParam);
	WORD	id = LOWORD( wParam);


		// trap window actions with associated data )
	switch ( id )
		{
		case ICAD_WNDACTION_USERCALL:
			ASSERT( !notifyCode );
			IcadWndAction( id, lParam);
			return TRUE;
		}

	return CMDIFrameWnd::OnCommand( wParam, lParam);
	}

					// MFC dialogs from SDS apps in MFC Extension DLLs can crash
					// because invoking the dialog uses MFC objects from different threads
					// (the main UI thread and the app thread) which is not safe.
					//
					// This message allows the app to invoke a dialog on the UI thread
afx_msg LRESULT CMainWindow::OnDoModal(WPARAM wParam, LPARAM lParam)
	{
	return ((CDialog *)wParam)->DoModal();
	}


afx_msg LRESULT CMainWindow::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{
	CClientDC theMainDC( this );
	m_bMapToRGBColor = ( theMainDC.GetDeviceCaps( BITSPIXEL ) > 8 );

	//*** Get the position of the prompt menu.
	struct resbuf rb;
	SDS_getvar(NULL,DB_QWNDPPMENU,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	//*** If the prompt menu's start point is out side the new resolution, move it.
	// EBATECH(CNBR) 2001-06-24 unsigned short -> int
	int cxScreen = (int)(short)LOWORD(lParam);
	int cyScreen = (int)(short)HIWORD(lParam);
	if(rb.resval.rpoint[0]>=cxScreen || rb.resval.rpoint[1]>=cyScreen)
	{
		rb.resval.rpoint[0]=0.0;
		rb.resval.rpoint[1]=0.0;
		sds_setvar("WNDPPMENU"/*DNT*/,&rb);
	}

	return CMDIFrameWnd::OnDisplayChange(wParam,lParam);
}

afx_msg LRESULT CMainWindow::OnGetPassWord(WPARAM wParam, LPARAM lParam)
{
	int loaded;
	char message[100];
	char caption[100];
	char caution[100] = {0};

	bool bSaving = *((char*)wParam) == '1';
	if (bSaving == true)
	{
		loaded = ::LoadString( ::GetModuleHandle( NULL), IDS_PASSWORD_SMESSAGE, message, 100);
		ASSERT(loaded);

		loaded = ::LoadString( ::GetModuleHandle( NULL), IDS_PASSWORD_CAUTION, caution, 100);
		ASSERT(loaded);

	}
	else
	{
		int loaded = ::LoadString( ::GetModuleHandle( NULL), IDS_PASSWORD_OMESSAGE, message, 100);
		ASSERT(loaded);
	}

	loaded = ::LoadString( ::GetModuleHandle( NULL), IDS_PASSWORD_CAPTION, caption, 100);
	ASSERT(loaded);

	CPasswordDlg pwdDlg(caption);

	pwdDlg.m_sMessage = message;
	pwdDlg.m_sCaution = caution;
	pwdDlg.m_sFileName = (char*)lParam;
	if (pwdDlg.DoModal()==IDOK)
	{
		_snprintf((char*)wParam, 100, pwdDlg.m_sPassword, "%s");
		pwdDlg.DestroyWindow();
		return S_OK;
	}
	pwdDlg.DestroyWindow();

	return E_FAIL;

}

afx_msg LRESULT CMainWindow::OnGetDwfInfo(WPARAM wParam, LPARAM lParam)
{
	int loaded;
	char caption[100];
	char versionCaption[100];
	char formatCaption[100];
	char version1[100];
	char version2[100];
	char version3[100];
	char format1[100];
	char format2[100];
	char format3[100];
	char layoutCaption[100];
	char layoutCurrent[100];
	char layoutAll[100];

	loaded = ::LoadString( ::GetModuleHandle( NULL), IDS_DWFINFO_CAPTION, caption, 100);
	ASSERT(loaded);

	loaded = ::LoadString( ::GetModuleHandle( NULL), IDS_DWFINFO_VERSION_CAUTION, versionCaption, 100);
	ASSERT(loaded);

	loaded = ::LoadString( ::GetModuleHandle( NULL), IDS_DWFINFO_FORMAT_CAUTION, formatCaption, 100);
	ASSERT(loaded);

	loaded = ::LoadString( ::GetModuleHandle( NULL), IDS_DWFINFO_VERSION1, version1, 100);
	ASSERT(loaded);

	loaded = ::LoadString( ::GetModuleHandle( NULL), IDS_DWFINFO_VERSION2, version2, 100);
	ASSERT(loaded);

	loaded = ::LoadString( ::GetModuleHandle( NULL), IDS_DWFINFO_VERSION3, version3, 100);
	ASSERT(loaded);

	loaded = ::LoadString( ::GetModuleHandle( NULL), IDS_DWFINFO_FORMAT1, format1, 100);
	ASSERT(loaded);

	loaded = ::LoadString( ::GetModuleHandle( NULL), IDS_DWFINFO_FORMAT2, format2, 100);
	ASSERT(loaded);

	loaded = ::LoadString( ::GetModuleHandle( NULL), IDS_DWFINFO_FORMAT3, format3, 100);
	ASSERT(loaded);

	loaded = ::LoadString( ::GetModuleHandle( NULL), IDS_DWFINFO_LAYOUT_GROUP, layoutCaption, 100);
	ASSERT(loaded);

	loaded = ::LoadString( ::GetModuleHandle( NULL), IDS_DWFINFO_LAYOUT_CURRENT, layoutCurrent, 100);
	ASSERT(loaded);

	loaded = ::LoadString( ::GetModuleHandle( NULL), IDS_DWFINFO_LAYOUT_ALL, layoutAll, 100);
	ASSERT(loaded);

	CDwfInfoDialog dwfDlg(caption, versionCaption, formatCaption, version1, version2, version3,
																  format1, format2, format3,
																  layoutCaption, layoutCurrent, layoutAll);

	if (dwfDlg.DoModal()==IDOK)
	{
		*((int*)lParam) = dwfDlg.m_iVersionFormat;
		dwfDlg.DestroyWindow();
		return S_OK;
	}
	dwfDlg.DestroyWindow();

	return E_FAIL;
}

const DWORD CMainWindow::dwDockBarMap[4][2] =
{
	{ AFX_IDW_DOCKBAR_TOP,		CBRS_TOP	},
	{ AFX_IDW_DOCKBAR_BOTTOM,	CBRS_BOTTOM },
	{ AFX_IDW_DOCKBAR_LEFT, 	CBRS_LEFT	},
	{ AFX_IDW_DOCKBAR_RIGHT,	CBRS_RIGHT	},
};

// dock bars will be created in the order specified by dwDockBarMap
// this also controls which gets priority during layout
// this order can be changed by calling EnableDocking repetitively
// with the exact order of priority
void CMainWindow::EnableDocking(DWORD dwDockStyle)
{
	// must be CBRS_ALIGN_XXX or CBRS_FLOAT_MULTI only
	ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);

	m_pFloatingFrameClass = RUNTIME_CLASS(CMiniDockFrameWnd);
	for (int i = 0; i < 4; i++)
	{
		if (dwDockBarMap[i][1] & dwDockStyle & CBRS_ALIGN_ANY)
		{
			CIcadDockBar* pDock = (CIcadDockBar*)GetControlBar(dwDockBarMap[i][0]);
			if (pDock == NULL)
			{
				//*** Create our own DockBar class.
				pDock = new CIcadDockBar;
				if (!pDock->Create(this,
					WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_CHILD|WS_VISIBLE |
						dwDockBarMap[i][1], dwDockBarMap[i][0]))
				{
					AfxThrowResourceException();
				}
			}
		}
	}
}

//*** Just call the base class, not copied from MFC source
void CMainWindow::DockControlBar(CControlBar* pBar, UINT nDockBarID, LPCRECT lpRect)
{
	CFrameWnd::DockControlBar(pBar,nDockBarID,lpRect);
}

void CMainWindow::DockControlBar(CControlBar* pBar, CDockBar* pDockBar, LPCRECT lpRect)
{
	ASSERT(pBar != NULL);
	// make sure CControlBar::EnableDocking has been called
	ASSERT(pBar->m_pDockContext != NULL);

	if (pDockBar == NULL)
	{
		for (int i = 0; i < 4; i++)
		{
			if ((dwDockBarMap[i][1] & CBRS_ALIGN_ANY) ==
				(pBar->m_dwStyle & CBRS_ALIGN_ANY))
			{
				pDockBar = (CDockBar*)GetControlBar(dwDockBarMap[i][0]);
				ASSERT(pDockBar != NULL);
				// assert fails when initial CBRS_ of bar does not
				// match available docking sites, as set by EnableDocking()
				break;
			}
		}
	}
	ASSERT(pDockBar != NULL);
	ASSERT(m_listControlBars.Find(pBar) != NULL);
	ASSERT(pBar->m_pDockSite == this);
	// if this assertion occurred it is because the parent of pBar was not initially
	// this CFrameWnd when pBar's OnCreate was called
	// i.e. this control bar should have been created with a different parent initially

	//*** Force this to call the DockControlBar() in CIcadDockBar.
	((CIcadDockBar*)pDockBar)->DockControlBar(pBar, lpRect);
	//***
}

//*** End - From MFC source WINFRM2.CPP 10/30/96 12:42 PM

void CMainWindow::PasteFromClipboard()
{
	BOOL bFound = FALSE;
	//*** Figure out if the clipboard has any text or entities in it.
	BOOL bHasText = FALSE;
	BOOL bHasEnts = FALSE;
	BOOL bHasAcad = FALSE;
	UINT cfAcad14 = ::RegisterClipboardFormat(_T("AutoCAD.r14"/*DNT*/));
	UINT cfAcad15 = ::RegisterClipboardFormat(_T("AutoCAD.r15"/*DNT*/));
	UINT cfAcad = 0;
	CIcadApp* pApp = (CIcadApp*)AfxGetApp();
	if(NULL==pApp || !OpenClipboard())
	{
		return;
	}
	UINT nFormat = EnumClipboardFormats(0);
	while(0!=nFormat)
	{
		if(CF_TEXT==nFormat/* || CF_UNICODETEXT==nFormat*/)
		{
			bHasText=TRUE;
		}
		else if(pApp->m_uIcadClipFormat==nFormat)
		{
			bHasEnts=TRUE;
		}
		else if ((cfAcad14 == nFormat) || (cfAcad15 == nFormat))
		{
			bHasAcad = TRUE;
			cfAcad = nFormat;
		}
		nFormat=EnumClipboardFormats(nFormat);
	}
	CloseClipboard();
	if(bHasEnts)
	{
		//*** Paste entities.
		bFound=PasteClipboardType(pApp->m_uIcadClipFormat);
		if (!bFound && bHasAcad)	//if this failed, try to paste as ACAD
			{
			bFound=PasteClipboardType(cfAcad14);
			if (!bFound)
				bFound=PasteClipboardType(cfAcad15);
			}
	}
	else if (bHasAcad)
	{
		//*** Paste native ACAD format.
		bFound=PasteClipboardType(cfAcad);
	}
	else
	{
		//*** Cursor position decides the priority of what to paste if there aren't
		//*** any entities.
		CPoint ptCursor;
		CRect rectTextScrn(0,0,0,0);
		CRect rectCmdBar(0,0,0,0);
		BOOL bCursorInCmdBar = FALSE;
		if(NULL!=m_pTextScrn && m_pTextScrn->IsWindowVisible())
		{
			m_pTextScrn->GetWindowRect(&rectTextScrn);
		}
		if(NULL!=m_pCmdBar && m_pCmdBar->IsWindowVisible())
		{
			m_pCmdBar->GetWindowRect(&rectCmdBar);
		}
		if(::GetCursorPos(&ptCursor) && (rectTextScrn.PtInRect(ptCursor) ||
			rectCmdBar.PtInRect(ptCursor)))
		{
			bCursorInCmdBar=TRUE;
		}

		if(!bCursorInCmdBar)
		{
			//*** First check for an OLE item.
			if(!(bFound=PasteClipboardType(0)))
			{
				//*** Paste text
				bFound=PasteClipboardType(CF_TEXT);
			}
		}
		else
		{
			//*** First check for text.
			if(!(bFound=PasteClipboardType(CF_TEXT)))
			{
				//*** Paste an OLE item.
				bFound=PasteClipboardType(0);
			}
		}
	}

	if(!bFound)
	{
		sds_alert(ResourceString(IDC_ICADMAIN_NOTHING_WAS_FO_49, "Clipboard data invalid or unavailable.  The Paste operation was not performed."));
	}
	return;
}

BOOL CMainWindow::PasteClipboardType(UINT cfType, COlePasteSpecialDialog* pPasteDlg)
{
	//*** Get the pointers to the app, doc and view.
	CIcadView* pView=GetIcadView();
	if(NULL==pView)
	{
		return FALSE;
	}
	CIcadChildWnd* pChild = (CIcadChildWnd*)MDIGetActive(NULL);
	if(NULL==pChild)
	{
		return FALSE;
	}
	CIcadDoc* pDoc = (CIcadDoc*)pChild->GetActiveDocument();
	if(NULL==pDoc)
	{
		return FALSE;
	}
	CIcadApp* pApp = (CIcadApp*)AfxGetApp();
	if(NULL==pApp)
	{
		return FALSE;
	}

	UINT cfAcad14 = ::RegisterClipboardFormat(_T("AutoCAD.r14"/*DNT*/));
	UINT cfAcad15 = ::RegisterClipboardFormat(_T("AutoCAD.r15"/*DNT*/));

	if(cfType==CF_TEXT)
	{
		if(!OpenClipboard())
		{
			return FALSE;
		}
		PasteClipboardText();
		CloseClipboard();
		return TRUE;
	}
	else if(cfType==pApp->m_uIcadClipFormat)
	{
		if(!OpenClipboard())
		{
			return FALSE;
		}
		HANDLE hData = GetClipboardData(cfType);
		if(NULL!=hData)
		{
			void* pData = GlobalLock(hData);
//			CMainWindow* pEntMain = (CMainWindow*)(((long*)pData)[0]);
//			if(NULL!=pData && pEntMain==this)
			BOOL bFoundDwg = FALSE;

			//Attempt this paste only if we are in the same process
			long procid = GetCurrentProcessId();
			if (procid == ((long*)pData)[3])
			{
				db_drawing* pDbDrawing = (db_drawing*)(((long*)pData)[2]);
				//*** Enumerate through all open drawings and make sure this drawing is open
				//*** in this session.
				if(NULL!=pData && NULL!=pDbDrawing)
				{
					CDocTemplate* pDocTemplate = pDoc->GetDocTemplate();
					POSITION pos = pDocTemplate->GetFirstDocPosition();
					CIcadDoc* pFromDoc;
					while(pos!=NULL)
					{
						pFromDoc = (CIcadDoc*)pDocTemplate->GetNextDoc(pos);
						if(NULL!=pFromDoc && NULL!=pFromDoc->m_dbDrawing &&
							pFromDoc->m_dbDrawing==pDbDrawing)
						{
							bFoundDwg=TRUE;
							break;
						}
					}
				}
			}
			if(bFoundDwg)
			{
				cmd_PasteItem(pData,pDoc->m_dbDrawing);
//				GlobalUnlock(hData);
//				CloseClipboard();
//				return TRUE;
			}
			else
			{
				//sds_alert("Entities may only be pasted into a drawing in the SAME session of IntelliCAD.");
			}
			GlobalUnlock(hData);
			CloseClipboard();
			return bFoundDwg;
		}
		CloseClipboard();
		return FALSE;
	}
	else if ((cfType==cfAcad14) || (cfType==cfAcad15))
	{
		if(!OpenClipboard())
		{
			return FALSE;
		}
		HANDLE hData = GetClipboardData(cfType);
		if(NULL!=hData)
		{
			//create a temporary file on disk and save the dwg to it
			char szPathName[IC_FILESIZE+IC_PATHSIZE], *pszPathName;
			BOOL bRemoveTempFile = FALSE;
			int bufsize = ::GlobalSize (hData);
			char *buffer = (char*)::GlobalLock(hData);
			CloseClipboard();

			// NOTE: The ACAD.r14 format can have two variations.  The global data may represent the actual
			// DWG in binary form, or it may represent the name of a dwg file on disk that contains the cut/copied
			// data.  We have to handle both cases.

			if (buffer[0]=='A' && buffer[1]=='C')	//we have a dwg binary format on the clipboard
				{
				if( ( pszPathName = _tempnam( ResourceString(IDC_ICADMAIN_C___TMP_50, "c:\\tmp" ), ResourceString(DNT_ICADMAIN_DWG_51, "DWG" ) ) ) == NULL )
					{
					sds_alert(ResourceString(IDC_ICADMAIN_CANNOT_CREATE__52, "Cannot create a unique filename." ));
					::GlobalUnlock(hData);
					return FALSE;
					}
				else
					{
					strncpy(szPathName,pszPathName,sizeof(szPathName));
					IC_FREE(pszPathName);
					ic_setext(szPathName,"dwg"/*DNT*/);
					FILE *fp = fopen (szPathName, "wb"/*DNT*/);
					if (fp)
						{
						fwrite (buffer, 1, bufsize, fp);
						fclose (fp);
						}
					}
					bRemoveTempFile = TRUE;
				}
			else	//we have the dwg file name on the clipboard
				{
					strncpy(szPathName,buffer,bufsize);
					if(RTERROR==sds_findfile(szPathName,buffer))
						{
						::GlobalUnlock(hData);
						return FALSE;
						}
				}
			::GlobalUnlock(hData);


			short ret = RTNORM;
			if (pPasteDlg)
			{
				//if called from Paste Special, let the user have full control of insert point, scale and rotation.
				m_strOpenFname = szPathName;
				SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C^C_DDINSERT"/*DNT*/);
			}
			else
			{
				//if called from Paste/Ctrl V, paste it in the middle of the window, at unit scale and no rotation.
				sds_point inspt, scale;
				struct gr_view *view = pView->m_pCurDwgView;
				struct resbuf setgetrb;
				sds_real curelev;

				if(view!=NULL)
					{
					view->GetCenterProjection(inspt);
					}
				else
					{
					inspt[0] = inspt[1] = inspt[2] = 0.0;
					}
				scale[0] = scale[1] = scale[2] = 1.0;

				db_insert ins;
				ins.set_10(inspt);
				ins.set_41(scale[0]);
				ins.set_42(scale[1]);
				ins.set_43(scale[2]);
				//get current elevation
				SDS_getvar(NULL,DB_QELEVATION,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				curelev=setgetrb.resval.rreal;

				char blkfile[IC_ACADBUF], blkname[IC_ACADBUF];
				if (ProcessInsertBlockName(szPathName,blkname,blkfile)!=RTNORM)	// takes blkname and returns block name, and path if any
					return FALSE;

				struct resbuf *tmprb;
				if ((tmprb = sds_tblsearch("BLOCK"/*DNT*/,blkname,0))==NULL) 		// it does not exist in the file
					{
					if (LoadTheBlock(blkname,blkfile)!=RTNORM)
						return FALSE;
					}
				else
					sds_relrb(tmprb);

				ret=DoTheInsert(blkname,false,&ins,false,inspt,curelev,false,false);
			}

			if (bRemoveTempFile && !pPasteDlg) //for paste special, cannot remove file
				unlink(szPathName);
			if (ret == RTNORM)
				return TRUE;
			else
				return FALSE;
		}
	}
	else
	{
		if(CIcadCntrItem::CanPaste())
		{
			//*** Create new item connected to this document.
			CIcadCntrItem* pItem = new CIcadCntrItem(pDoc,NULL);
			if(NULL==pItem)
			{
				return FALSE;
			}
			// We have to do a special set undo here because this
			// WindAction is done witt a PostThreadMessage().  OUCH!
			SDS_SetUndo(IC_UNDO_COMMAND_BEGIN,(void *)ResourceString(IDC_ICADMAIN_PASTESPEC_54, "PASTESPEC" ),NULL,NULL,SDS_CURDWG);
			if(NULL!=pPasteDlg)
			{
				if(!pPasteDlg->CreateItem(pItem))
				{
					delete pItem;
					return FALSE;
				}
				((COleClientItem*)pItem)->SetDrawAspect(pPasteDlg->GetDrawAspect());
			}
			else if(!pItem->CreateFromClipboard())
			{
				if(!pItem->CreateStaticFromClipboard())
				{
					delete pItem;
					return FALSE;
				}
			}

			//*** set the correct drawing aspect
			if(NULL==pPasteDlg && OpenClipboard())
			{
				HANDLE hData = GetClipboardData(::RegisterClipboardFormat(_T(ResourceString(IDC_ICADMAIN_OBJECT_DESCRIP_55, "Object Descriptor" ))));
				if(NULL!=hData)
				{
					LPOBJECTDESCRIPTOR pObjDesc = (LPOBJECTDESCRIPTOR)GlobalLock(hData);
					ASSERT(pObjDesc != NULL);
					((COleClientItem*)pItem)->SetDrawAspect((DVASPECT)pObjDesc->dwDrawAspect);
					GlobalUnlock(hData);
				}
				CloseClipboard();
			}

			SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)ResourceString(IDC_ICADMAIN_PASTESPEC_54, "PASTESPEC" ),NULL,NULL,SDS_CURDWG);

			pItem->UpdateHandItem(pView,TRUE);
			pItem->UpdateFromServerExtent(pView);
			pView->SetSelection(pItem);
			pItem->Draw(pView,pView->GetFrameBufferCDC());
			return TRUE;
		}
	}
	return FALSE;
}

void CMainWindow::PasteClipboardText()
{
//*** This function relies on the calling function to open and close the
//*** clipboard.
	HANDLE hData = GetClipboardData(CF_TEXT);
	if(NULL==hData)
	{
		return;
	}
	//*** Replace all "\r\n" with ";"
	CString strSend;
	strSend.Empty();
	CString strClip = (LPCTSTR)GlobalLock(hData);
	int nCount;
	if(strClip.IsEmpty())
	{
		GlobalUnlock(hData);
		return;
	}
	while((nCount=strClip.Find("\r\n"/*DNT*/))!=(-1))
	{
		if(!(strClip.GetAt(0)==';')) {
			strSend+=strClip.Left(nCount);
			strSend+=ResourceString(IDC_ICADMAIN___56, ";" );
		}
		strClip=strClip.Right(strClip.GetLength()-(nCount+2));
	}
	strSend+=strClip;
	nCount=strSend.GetLength();
	if(strSend[nCount-1]!=';' && strSend[nCount-1]!=')' && strSend[nCount-1]!='\\')
	{
		strSend+=ResourceString(IDC_ICADMAIN____57, "\\" );
	}
	//*** Send the message to the command line.
	SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)strSend.GetBuffer(0));
	//*** Cleanup
	GlobalUnlock(hData);
}

// EBATECH(CNBR) 2001-06-24 -[ OEM App.
/*
BOOL CMainWindow::OnQueryNewPalette()
{
	// CG: This function was added by the Palette Support component
	if (m_pPalette == NULL)
		return FALSE;

	// BLOCK
	{
		CClientDC dc(this);
		CPalette* pOldPalette = dc.SelectPalette(m_pPalette,
			GetCurrentMessage()->message == WM_PALETTECHANGED);
		UINT nChanged = dc.RealizePalette();

		extern RGBQUAD SDS_PaletteColors[256];
		for (int i=0; i<256; i++) {
			int rgb,fi1,fi2;
			fi1=RGB(SDS_PaletteColors[255].rgbRed,
					SDS_PaletteColors[255].rgbGreen,
					SDS_PaletteColors[255].rgbBlue);
			fi2=RGB(SDS_PaletteColors[i].rgbRed,
					SDS_PaletteColors[i].rgbGreen,
					SDS_PaletteColors[i].rgbBlue);
			rgb=fi1^fi2;
			rgb=m_pPalette->GetNearestPaletteIndex(rgb);
			SDS_PaletteColors[i].rgbReserved=rgb;
		}


		extern int SDS_XorColorIndex;
		if(SDS_PaletteColors[255].rgbRed==128 && SDS_PaletteColors[255].rgbGreen==128 && SDS_PaletteColors[255].rgbBlue==128) {
			SDS_XorColorIndex=6;
		} else {
			i=RGB(~SDS_PaletteColors[255].rgbRed,
				  ~SDS_PaletteColors[255].rgbGreen,
				  ~SDS_PaletteColors[255].rgbBlue);
			SDS_XorColorIndex=m_pPalette->GetNearestPaletteIndex(i);
		}

//		dc.SelectPalette(pOldPalette, TRUE);

		SDS_CallUserCallbackFunc(SDS_CBPALETTECHG,(void *)(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),NULL,NULL);

		if (nChanged == 0)
			return FALSE;
	}

	Invalidate();

	return TRUE;
}
*/
BOOL CMainWindow::OnQueryNewPalette()
{
	// CG: This function was added by the Palette Support component
	if (m_pPalette == NULL)
		return FALSE;

	// BLOCK
	{
		CClientDC dc(this);
		CPalette* pOldPalette = dc.SelectPalette(m_pPalette,
			GetCurrentMessage()->message == WM_PALETTECHANGED);
		UINT nChanged = dc.RealizePalette();
		RGBQUAD*	ary = SDS_CURPALLET;

		//extern RGBQUAD SDS_PaletteColors[256];
		for (int i=0; i<256; i++) {
			int rgb,fi1,fi2;
			fi1=RGB(ary[255].rgbRed,
					ary[255].rgbGreen,
					ary[255].rgbBlue);
			fi2=RGB(ary[i].rgbRed,
					ary[i].rgbGreen,
					ary[i].rgbBlue);
			rgb=fi1^fi2;
			rgb=m_pPalette->GetNearestPaletteIndex(rgb);
			ary[i].rgbReserved=rgb;
		}


		extern int SDS_XorColorIndex;
		if(ary[255].rgbRed==128 && ary[255].rgbGreen==128 && ary[255].rgbBlue==128) {
			SDS_XorColorIndex=6;
		} else {

			if(ary[255].rgbRed==255 && ary[255].rgbGreen==255 && ary[255].rgbBlue==255)
				i=RGB(192, 192, 192);	 // Xor'ing with black is a no-op, use gray instead
			else
				i=RGB(~ary[255].rgbRed,
					  ~ary[255].rgbGreen,
					  ~ary[255].rgbBlue);

			SDS_XorColorIndex=m_pPalette->GetNearestPaletteIndex(i);
		}

//		dc.SelectPalette(pOldPalette, TRUE);

		SDS_CallUserCallbackFunc(SDS_CBPALETTECHG,(void *)(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),NULL,NULL);

		if (nChanged == 0)
			return FALSE;
	}

	Invalidate();

	return TRUE;
}
// EBATECH(CNBR) ]-

void CMainWindow::OnPaletteChanged(CWnd* pFocusWnd)
{
	// CG: This function was added by the Palette Support component

	CClientDC theMainDC( this );
	m_bMapToRGBColor = ( theMainDC.GetDeviceCaps( BITSPIXEL ) > 8 );

	if (pFocusWnd == this || IsChild(pFocusWnd))
		return;

	OnQueryNewPalette();

	CIcadChildWnd* pChild = (CIcadChildWnd*)MDIGetActive(NULL);
	if(pChild==NULL) return;
	CIcadDoc* pDoc = (CIcadDoc*)pChild->GetActiveDocument();
	if(pDoc==NULL) return;
	CDocTemplate* pDocTemplate = pDoc->GetDocTemplate();
	POSITION pos = pDocTemplate->GetFirstDocPosition();
	while(pos!=NULL)
	{
		pDoc = (CIcadDoc*)pDocTemplate->GetNextDoc(pos);
		CIcadView *pView;
		if(pDoc) {
			POSITION pos=pDoc->GetFirstViewPosition();
			while (pos!=NULL) {
				pView=(CIcadView *)pDoc->GetNextView(pos);
				ASSERT_KINDOF(CIcadView, pView);  // ????

				pView->SendMessage(WM_PALETTECHANGED,(WPARAM)pFocusWnd->GetSafeHwnd());
			}
		}
	}
}

CPalette* CMainWindow::SetPalette(CPalette* pPalette)
{
	// CG: This function was added by the Palette Support component

	// Call this function when the palette changes.  It will
	// realize the palette in the foreground to cause the screen
	// to repaint correctly.  All calls to CDC::SelectPalette in
	// painting code should select palettes in the background.

	CPalette* pOldPalette = m_pPalette;
	m_pPalette = pPalette;
	OnQueryNewPalette();
	return pOldPalette;
}


CIcadMenu *
CMainWindow::GetCurrentMenu( void )
	{
	CIcadMenu *pRetval = NULL;
	if ( m_pMenuMain != NULL )
		{
		pRetval = m_pMenuMain->GetCurrentMenu();
		}

	return pRetval;
	}


void
CMainWindow::DocChangeCallback( CIcadView *pNewView )
	{
	ASSERT_VALID( pNewView );

	static CIcadView *pOldView = NULL;

	// Only send if window has actually changed
	//
	if ( (pOldView != pNewView) &&
		 (pOldView != NULL) )
		{
		CString strDrawingName = pNewView->m_pMain->m_pCurDoc->GetPathName();
		if ( strDrawingName.IsEmpty() )
			{
			strDrawingName = pNewView->m_pMain->m_pCurDoc->GetTitle();
			}

		HWND hOldWindow = NULL;

		// **************************************
		// pOldView may have been deleted by now
		//
		if ( CHECKSTRUCTPTR( pOldView ) )
			{
// removed VTABLE check since it failed in some compiles
//			if ( CHECKPTR( (void *)(*pOldView) ) )
			hOldWindow = pOldView->GetSafeHwnd();
			}

		SDS_CallUserCallbackFunc(SDS_CBDOCCHG,(void *)(char *)(LPCTSTR)strDrawingName,(void *)pNewView->GetSafeHwnd(),(void *)hOldWindow );
		}

	pOldView = pNewView;
	}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Send WM_MOUSEWHEEL message to SDS_CURVIEW.
 * Returns:	TRUE.
 ********************************************************************************/
BOOL
CMainWindow::OnMouseWheel
(
 UINT nFlags,
 short zDelta,
 CPoint pt
)
{
	// WM_MOUSEWHEEL message isn't sent from a frame window to its childs. So, send it.

	// Get just the latest wheel rotation.
	bool	bFoundNewerMouseWheel = false;
	MSG		msgLookAhead;
	HWND	hWnd = GetSafeHwnd();

	while(::PeekMessage(&msgLookAhead, hWnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE))
		bFoundNewerMouseWheel = true;	// Just get the latest mouse move

	if(bFoundNewerMouseWheel)
	{
		// EBATECH(CNBR) 2001-06-24 Add Cast operator
		nFlags = (UINT)LOWORD(msgLookAhead.wParam);
		zDelta = (short)HIWORD(msgLookAhead.wParam);
		pt.x = (int)(short)LOWORD(msgLookAhead.lParam);
		pt.y = (int)(short)HIWORD(msgLookAhead.lParam);
	}

	// Send a message.
	if(SDS_CURVIEW && GetCapture() != this)
		::SendMessage(SDS_CURVIEW->GetSafeHwnd(), WM_MOUSEWHEEL, MAKELONG(nFlags, zDelta), MAKELONG(pt.x, pt.y));

	return TRUE;
}



afx_msg BOOL CMainWindow::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	if(pCopyDataStruct->cbData>=IC_ACADBUF)
		return FALSE;

	int    charCount   = 0;

	while(((char*)(pCopyDataStruct->lpData))[charCount] !='\0' && charCount < pCopyDataStruct->cbData)
	{
		if(((char *)(pCopyDataStruct->lpData))[charCount]==13 || ((char *)(pCopyDataStruct->lpData))[charCount]==10)
			((char *)(pCopyDataStruct->lpData))[charCount]=' ';
		InputEvent Event(SDS_EVM_KEYCHAR,((char *)(pCopyDataStruct->lpData))[charCount++]);
		SDS_PostInputEvent( Event );
	}

    return TRUE;
}
