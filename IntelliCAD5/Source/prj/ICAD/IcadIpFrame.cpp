/* D:\DEV\PRJ\ICAD\ICADIPFRAME.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// IcadIpFrame.cpp : implementation of the CIcadIpFrame class
//

#include "Icad.h"/*DNT*/
#include "IcadDoc.h"
#include "IcadIpFrame.h"/*DNT*/
#include "IcadToolBarMain.h"/*DNT*/
#include "IcadMenu.h"/*DNT*/
#include "IcadMenuMain.h"/*DNT*/
#include "IcadDockBar.h"/*DNT*/
#include "IcadView.h"
#include "Preferences.h"
#include "IcadDef.h"
#include "win32misc.h"

#ifdef _DEBUG

	#if !defined(USE_SMARTHEAP)
	#define new DEBUG_NEW
	#endif

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIcadOleCntrFrameWnd

BEGIN_MESSAGE_MAP(CIcadOleCntrFrameWnd, COleCntrFrameWnd)
	//{{AFX_MSG_MAP(CIcadOleCntrFrameWnd)
	//}}AFX_MSG_MAP
	// Global help commands
END_MESSAGE_MAP()

CIcadOleCntrFrameWnd::CIcadOleCntrFrameWnd(CIcadIpFrame* pInPlaceFrame) :
	COleCntrFrameWnd(pInPlaceFrame)
{
}

//*** Explanations to similar code can be found in CMainWindow.
void CIcadOleCntrFrameWnd::EnableDocking(DWORD dwDockStyle)
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

void CIcadOleCntrFrameWnd::DockControlBar(CControlBar* pBar, UINT nDockBarID,
	LPCRECT lpRect)
{
	CDockBar* pDockBar = (nDockBarID == 0) ? NULL :
		(CDockBar*)GetControlBar(nDockBarID);
	DockControlBar(pBar, pDockBar, lpRect);
}

void CIcadOleCntrFrameWnd::DockControlBar(CControlBar* pBar, CDockBar* pDockBar,
	LPCRECT lpRect)
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

/////////////////////////////////////////////////////////////////////////////
// CIcadIpFrame

IMPLEMENT_DYNCREATE(CIcadIpFrame, COleIPFrameWnd)

BEGIN_MESSAGE_MAP(CIcadIpFrame, COleIPFrameWnd)
	//{{AFX_MSG_MAP(CIcadIpFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ACTIVATEAPP()
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND_RANGE(TB_START_ID, TB_END_ID,IcadInPlaceMenuExec)
	ON_COMMAND_RANGE(ICAD_ACCELID_START,ICAD_ACCELID_END,OnAccelKey)
	ON_COMMAND(ID_HELP_FINDER, COleIPFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, COleIPFrameWnd::OnHelp)
	ON_COMMAND(ID_DEFAULT_HELP, COleIPFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_CONTEXT_HELP, COleIPFrameWnd::OnContextHelp)
	ON_NOTIFY_EX(TTN_NEEDTEXT,0,OnToolTipText)
	ON_NOTIFY_EX(TTN_NEEDTEXTW,0,OnToolTipTextW)
	ON_MESSAGE(WM_SIZECHILD,OnResizeChild)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIcadIpFrame construction/destruction

CIcadIpFrame::CIcadIpFrame()
{
	m_hIcadIpMenu=NULL;
	m_pTbMain = new CIcadToolBarMain();
}

CIcadIpFrame::~CIcadIpFrame()
{
	if(m_hIcadIpMenu!=NULL)
	{
		::DestroyMenu(m_hIcadIpMenu);
		m_hIcadIpMenu=NULL;
	}
	if(NULL!=SDS_CMainWindow)
	{
		SDS_CMainWindow->m_bIsAppActive=FALSE;
	}
}

BOOL CIcadIpFrame::PreTranslateMessage(MSG* pMsg) 
{
	if(TranslateAccelerator(GetSafeHwnd(),SDS_CMainWindow->m_hAccel,pMsg)==TRUE)
	{
		return TRUE;
	}
	return COleIPFrameWnd::PreTranslateMessage(pMsg);
}

LRESULT CIcadIpFrame::OnResizeChild(WPARAM wParam, LPARAM lParam)
{
	CRect rSize((LPRECT )lParam);
	CIcadDoc* pDoc = (CIcadDoc*) GetActiveDocument();
	if(NULL!=pDoc)
	{
		// change our extents  
		pDoc->m_sizeDoc.cx = MulDiv(pDoc->m_ZoomDenom.cx, rSize.Width(),pDoc->m_ZoomNum.cx);
		pDoc->m_sizeDoc.cy = MulDiv(pDoc->m_ZoomDenom.cy, rSize.Height(),pDoc->m_ZoomNum.cy);

		// notify container that we have changed
		pDoc->m_bInIPResize = TRUE;
		pDoc->NotifyChanged();
		pDoc->m_bInIPResize = FALSE;
	}
	return COleIPFrameWnd::OnResizeChild(wParam,lParam);
}

void CIcadIpFrame::RepositionFrame(LPCRECT lpPosRect, LPCRECT lpClipRect)
{
	ASSERT(AfxIsValidAddress(lpPosRect, sizeof(RECT), FALSE));
	ASSERT(AfxIsValidAddress(lpClipRect, sizeof(RECT), FALSE));

	// gaurd against recursion
	if (m_bInsideRecalc)
		return;
	m_bInsideRecalc = TRUE;

	//*** Make sure the scroll bars are accounted for.
	CView* pView = GetActiveView();
	if(NULL!=pView)
	{
		if(!(pView->GetStyle() & (WS_HSCROLL | WS_VSCROLL)))
		{
			((CIcadView*)pView)->UpdateScrollBars();
		}
	}
	// remember the client area for later
	m_rectPos.CopyRect(lpPosRect);
	m_rectClip.CopyRect(lpClipRect);

	// recalc layout based on new position & clipping rectangles
	RecalcLayout();

	// remove recursion lockout
	m_bInsideRecalc = FALSE;
}

int CIcadIpFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleIPFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// CResizeBar implements in-place resizing.
	if (!m_wndResizeBar.Create(this))
	{
		TRACE0(ResourceString(IDC_ICADIPFRAME_FAILED_TO_CR_0, "Failed to create resize bar\n" ));
		return -1;      // fail to create
	}

	// By default, it is a good idea to register a drop-target that does
	//  nothing with your frame window.  This prevents drops from
	//  "falling through" to a container that supports drag-drop.
	m_dropTarget.Register(this);

	return 0;
}

// OnCreateControlBars is called by the framework to create control bars on the
//  container application's windows.  pWndFrame is the top level frame window of
//  the container and is always non-NULL.  pWndDoc is the doc level frame window
//  and will be NULL when the container is an SDI application.  A server
//  application can place MFC control bars on either window.
BOOL CIcadIpFrame::OnCreateControlBars(CFrameWnd* pWndFrame, CFrameWnd* pWndDoc)
{
	//*** Use Our COleCntrFrameWnd class instead for docking purposes.
	HWND hWnd;
	if(NULL!=m_pMainFrame)
	{
		hWnd = m_pMainFrame->Detach();
	}
	if(NULL!=hWnd)
	{
		delete m_pMainFrame;
		m_pMainFrame=NULL;
	}
	CIcadOleCntrFrameWnd* pCntrFrameWnd = new CIcadOleCntrFrameWnd(this);
	m_pMainFrame = pCntrFrameWnd;
	m_pMainFrame->Attach(hWnd);
	if(NULL!=m_pDocFrame)
	{
		HWND hWndDocument = m_pDocFrame->Detach();
		delete m_pDocFrame;
		m_pDocFrame=NULL;
		if(NULL!=hWndDocument)
		{
			m_pDocFrame = new CIcadOleCntrFrameWnd(this);
			m_pDocFrame->Attach(hWndDocument);
		}
	}

	ICTOOLBARCREATESTRUCT tbc;

	memset(&tbc,0,sizeof(ICTOOLBARCREATESTRUCT));
	//*** Send all messages to pCntrFrameWnd.
	tbc.pParentFrame=pCntrFrameWnd;
	tbc.bToolTips=TRUE;
	tbc.bLargeButtons=FALSE;
	tbc.bColorButtons=TRUE;
	tbc.lflgVisibility=ICAD_MN_SIP;
	//*** Create the toolbars
//	if(!m_pTbMain->CreateFromRegistry(&tbc)) //*** This causes too many potential LoadBarState() problems.
//	{
		m_pTbMain->CreateDefault(&tbc);
//	}

	pCntrFrameWnd->EnableDocking(CBRS_ALIGN_ANY);

	CString prefixProfileName = ResourceString(IDS_REGISTRY_PROFILES, "Profiles");
	prefixProfileName += _T("\\");
	CString profName;
	GetCurrentRegistryProfileName(profName.GetBuffer(IC_ACADBUF), IC_ACADBUF);
	profName.ReleaseBuffer();
	prefixProfileName += profName;
	PreferenceKey inPlaceBarsKey(HKEY_CURRENT_USER, _T("InPlaceBars"), FALSE);
	if(inPlaceBarsKey.m_hKey)
	{
		inPlaceBarsKey.Close();
		// LoadBarState() uses this name:  m_pszRegistryKey + m_pszProfileName (= "ITC\\IntelliCAD")
		pCntrFrameWnd->LoadBarState(prefixProfileName + _T("\\InPlaceBars\\ControlBar"/*DNT*/));
	}
	else
	{
		CPtrList* pToolBarList = m_pTbMain->GetToolBarList();
		if(pToolBarList==NULL) return FALSE;
		POSITION pos = pToolBarList->GetHeadPosition();
		CIcadToolBar* pToolBar;
		int ct;
		for(ct=0; pos!=NULL; ct++)
		{
			pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
			if(NULL==pToolBar)
			{
				continue;
			}
			pCntrFrameWnd->DockControlBar(pToolBar,AFX_IDW_DOCKBAR_TOP);
			if(!pToolBar->m_bEmpty)
			{
				//*** Hide the control bar if there are not any buttons associated with it.
				pCntrFrameWnd->ShowControlBar(pToolBar,TRUE,FALSE);
			}
		}
	}

	//*** Set the owner to this frame window, without this, some apps hang.
	CPtrList* pToolBarList = m_pTbMain->GetToolBarList();
	if(pToolBarList==NULL) return FALSE;
	POSITION pos = pToolBarList->GetHeadPosition();
	CIcadToolBar* pToolBar;
	int bVisible = 0;
	while(pos!=NULL)
	{
		pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
		if(NULL==pToolBar)
		{
			continue;
		}
		pToolBar->SetOwner(this);
		bVisible += pToolBar->IsWindowVisible();	//if it is visible, increment the counter
		bVisible -= pToolBar->IsFloating();			//but if it is a floating toolbar, it doesnt count, so decrement it
	}
	pToolBar = NULL;

	//if none of toolbars are visible, we are in a potential error situation.  This typically happens
	//when you are in-place active in Visio and kill Visio without properly deactivating it.
	if (bVisible <= 0)
	{
		pos = pToolBarList->GetHeadPosition();
		while(pos!=NULL)
		{
			pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
			if(NULL==pToolBar)
			{
				continue;
			}
			if(!pToolBar->m_bEmpty)
			{
				pToolBar->ShowWindow (SW_SHOW);
			}
		}
	}

	return TRUE;
}

BOOL CIcadIpFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	return COleIPFrameWnd::PreCreateWindow(cs);
}

void CIcadIpFrame::OnDestroy()
{
	// notify the container that the rectangle has changed!
	COleServerDoc* pDoc = (COleServerDoc*)GetActiveDocument();
	if (pDoc != NULL)
	{
		ASSERT_KINDOF(COleServerDoc, pDoc);

		// close and abort changes to the document
		pDoc->DisconnectViews();
		pDoc->OnCloseDocument();
	}
	//*** Free the toolbars before DestroyDockBars() is called.
	if(NULL!=m_pTbMain)
	{
		//*** Save the toolbars to the registry before destroying.
//		m_pTbMain->SaveToRegistry(TRUE);
		delete m_pTbMain;
		m_pTbMain=NULL;
	}
	// explicitly destroy all of the dock bars since this window
	// is actually in the container and will not be destroyed
	if (m_pMainFrame != NULL)
		m_pMainFrame->DestroyDockBars();
	if (m_pDocFrame != NULL)
		m_pDocFrame->DestroyDockBars();

	CFrameWnd::OnDestroy();
}

HMENU CIcadIpFrame::GetInPlaceMenu()
{
	if(m_hIcadIpMenu!=NULL)
	{
		::DestroyMenu(m_hIcadIpMenu);
	}
	CMainWindow* pMain = SDS_CMainWindow;
	if(NULL==pMain)
	{
		return NULL;
	}
	CIcadMenu* pMenu = pMain->m_pMenuMain->GetDefaultMenu();
	if(NULL!=pMenu)
	{
		m_hIcadIpMenu = pMenu->MenuCreator(ICAD_MN_SIP);
		return m_hIcadIpMenu;
	}
	return NULL;
}

void CIcadIpFrame::IcadInPlaceMenuExec(UINT nID)
{
	if(m_pTbMain->m_bCustomize==TRUE)
	{
		m_pTbMain->m_pCusTbDlg->OnTbButtonPick(nID-TB_START_ID+ICAD_CUSTOOL_BTNID_START);
		return;
	}
	LPARAM lParam = 0;
	if(NULL!=SDS_CMainWindow->m_pMenuMain)
	{
		CIcadMenu* pMenu = SDS_CMainWindow->m_pMenuMain->GetCurrentMenu();
		if(NULL!=pMenu)
		{
			lParam=(LPARAM)pMenu->GetCommandString(nID);
		}
	}
	SDS_SendMessage(WM_MENUSELECT,0,lParam);
}

void CIcadIpFrame::OnAccelKey(UINT nID)
{
	SDS_CMainWindow->OnAccelKey(nID);
}

BOOL CIcadIpFrame::OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
	//*** Returning false here calls OnToolTipText in CMainWindow.
	return FALSE;
}

BOOL CIcadIpFrame::OnToolTipTextW(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
	//*** This function is for Windows NT.
	//*** Returning false here calls OnToolTipText in CMainWindow.
	return FALSE;
}

void CIcadIpFrame::OnActivateApp(BOOL bActive, HTASK hTask) 
{
	if(NULL!=SDS_CMainWindow)
	{
		SDS_CMainWindow->m_bIsAppActive=bActive;
	}
	COleIPFrameWnd::OnActivateApp(bActive, hTask);
}


