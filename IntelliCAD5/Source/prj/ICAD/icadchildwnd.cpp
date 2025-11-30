/* D:\DEV\PRJ\ICAD\ICADCHILDWND.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

/////////////////////////////////////////////////////////////////////
// Icad MDI child window class

#include "Icad.h"/*DNT*/
#include "IcadChildWnd.h"/*DNT*/
#include "IcadApp.h"/*DNT*/
#include "IcadPrintDia.h"/*DNT*/
#include "resource.hm"/*DNT*/
#include "cmds.h"/*DNT*/
#include "PrintDialog.h"
#include "paths.h"
#include "IcadView.h"

extern int SDS_IsDragging;

IMPLEMENT_DYNCREATE(CIcadChildWnd, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CIcadChildWnd, CMDIChildWnd)
	//{{AFX_MSG_MAP(CIcadChildWnd)
    ON_WM_MOVE()
	ON_WM_CLOSE()
    ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_CHILDACTIVATE()
	//}}AFX_MSG_MAP
//4M Elias Item:50->
    ON_WM_SIZING()
//<-4M Elias Item:50
    ON_NOTIFY_EX(TTN_NEEDTEXT,0,OnToolTipText)
	ON_NOTIFY_EX(TTN_NEEDTEXTW,0,OnToolTipTextW)
	ON_UPDATE_COMMAND_UI(IDC_PRNPREV_SETUP, OnUpdatePrintPreview)
	ON_UPDATE_COMMAND_UI(IDC_PRNPREV_CLOSE, OnUpdatePrintPreview)
	ON_UPDATE_COMMAND_UI(IDC_PRNPREV_HELP, OnUpdatePrintPreview)
	ON_COMMAND(IDC_PRNPREV_ZOOMIN, OnPreviewZoomIn)
	ON_COMMAND(IDC_PRNPREV_ZOOMOUT, OnPreviewZoomOut)
	ON_COMMAND(IDC_PRNPREV_PRINT, OnPreviewPrint)
	ON_COMMAND(IDC_PRNPREV_SETUP, OnPreviewSetup)
	ON_COMMAND(IDC_PRNPREV_CLOSE, OnPreviewClose)
	ON_COMMAND(IDC_PRNPREV_HELP, OnPreviewHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////
// CIcadChildWnd construction/destruction

CIcadChildWnd::CIcadChildWnd()
{
    //*** Initialize these potential config variables
    m_nMaxRows=2;
    m_nMaxCols=2;
    m_sizeMin.cx=10;
    m_sizeMin.cy=10;
	m_pDrawingAtClose=NULL;
	m_pTabRecAtClose=NULL;
}

CIcadChildWnd::~CIcadChildWnd()
{
	struct resbuf rb;

	if(!SDS_CMainWindow->m_nIsClosing && SDS_CURDWG) {
		SDS_getvar(NULL,DB_QUNDOCTL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(m_pDrawingAtClose && m_pTabRecAtClose && (rb.resval.rint & IC_UNDOCTL_ON)) {
			extern int SDS_AtCmdLine;
			if(SDS_AtCmdLine) SDS_SetUndo(IC_UNDO_COMMAND_BEGIN,(void *)ResourceString(IDC_ICADCHILDWND_WINDOW_CLOS_0, "Window close" ),NULL,NULL,m_pDrawingAtClose);
			CRect *cr=new CRect(m_WindowRect);
			SDS_SetUndo(IC_UNDO_CICADVIEW_DELETE,(void *)this,(void *)m_pTabRecAtClose,(void *)cr,m_pDrawingAtClose);
			if(SDS_AtCmdLine) SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)ResourceString(IDC_ICADCHILDWND_WINDOW_CLOS_0, "Window close" ),NULL,NULL,m_pDrawingAtClose);
		}
	}
}

BOOL CIcadChildWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	CMainWindow* pMain = (CMainWindow*)AfxGetMainWnd();
	if(NULL!=pMain && !pMain->m_bUsingTempView)
	{
		ASSERT_VALID( pMain );

		if(pMain->m_rectCreateView) {
			cs.x=pMain->m_rectCreateView->left;
			cs.y=pMain->m_rectCreateView->top;
			cs.cx=pMain->m_rectCreateView->right-pMain->m_rectCreateView->left;
			cs.cy=pMain->m_rectCreateView->bottom-pMain->m_rectCreateView->top;
		}
		if(pMain->m_nOpenViews==0)
		{
//			struct sds_resbuf rb; *** Commented out to maximize every time the first window is opened.
//			SDS_getvar(NULL,DB_QWNDLMDI,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
//			if(rb.resval.rint==2)
//			{
				cs.style|=(WS_MAXIMIZE | WS_VISIBLE);
//			}
		}
	}
	return CMDIChildWnd::PreCreateWindow(cs);
}

BOOL CIcadChildWnd::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	ASSERT_VALID( this );
    //*** Create the splitter window.
#ifdef _LAYOUTTABS_	
	BOOL ret = m_wndSplitter.Create(this, 1, 2, m_sizeMin, pContext, WS_CHILD | WS_VISIBLE | SPLS_DYNAMIC_SPLIT);
	m_wndSplitter.SetScrollStyle(WS_HSCROLL | WS_VSCROLL);
	m_layoutTabs.Create(WS_VISIBLE | WS_CHILD | TCS_BOTTOM, 
		CRect(0, lpcs->cy - GetSystemMetrics(SM_CYHSCROLL), lpcs->cx, lpcs->cy), &m_wndSplitter, 100);
	CFont font;
	font.CreateStockObject(DEFAULT_GUI_FONT);
	m_layoutTabs.SetFont(&font, FALSE);
#else
	BOOL ret=m_wndSplitter.Create(this,m_nMaxRows,m_nMaxCols,m_sizeMin,pContext,WS_CHILD|WS_VISIBLE|SPLS_DYNAMIC_SPLIT);
#endif
    return ret;
}

BOOL CIcadChildWnd::OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
	//*** Returning false here calls OnToolTipText in CMainWindow.
	return FALSE;
}

BOOL CIcadChildWnd::OnToolTipTextW(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
	//*** This function is for Windows NT.
	//*** Returning false here calls OnToolTipText in CMainWindow.
	return FALSE;
}

void CIcadChildWnd::OnUpdatePrintPreview(CCmdUI* pCmdUI)
{
	ASSERT_VALID( this );
	//*** Enable it.
	pCmdUI->Enable();
}

void CIcadChildWnd::OnPreviewZoomIn()
{
	ASSERT_VALID( this );
	if(NULL==SDS_CMainWindow)
	{
		return;
	}
	//*** Do the zoom in.
	SDS_CMainWindow->SendMessage(WM_COMMAND,AFX_ID_PREVIEW_ZOOMIN,0);
}

void CIcadChildWnd::OnPreviewZoomOut()
{
	ASSERT_VALID( this );
	if(NULL==SDS_CMainWindow)
	{
		return;
	}
	//*** Do the zoom out.
	SDS_CMainWindow->SendMessage(WM_COMMAND,AFX_ID_PREVIEW_ZOOMOUT,0);
}

void CIcadChildWnd::OnPreviewPrint()
{
	if(NULL==SDS_CMainWindow)
	{
		return;
	}

	SDS_CMainWindow->m_pIcadPrintDlg->m_prevTab=CPrintDialogSheet::SCALE_VIEW;
	//*** Close the print preview.
	SDS_CMainWindow->SendMessage(WM_COMMAND,AFX_ID_PREVIEW_CLOSE,0);
	//*** Do the QPrint
	::ExecuteUIAction( ICAD_WNDACTION_CURSOROFF );
	::ExecuteUIAction( ICAD_WNDACTION_QPRINT );
}

void CIcadChildWnd::OnPreviewSetup()
{
	//*** Close the print preview.
	SDS_CMainWindow->SendMessage(WM_COMMAND,AFX_ID_PREVIEW_CLOSE,0);
//	OnPreviewClose();
	//*** Call up the page setup dialog.
	if(SDS_CMainWindow->m_pIcadPrintDlg != NULL) 
		SDS_CMainWindow->m_pIcadPrintDlg->m_bWasCalledFromPrintPreview = TRUE;
	SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)ResourceString(IDC_ICADCHILDWND__C_C_CPRINT_1, "^C^C^C_PRINT" ));
}

void CIcadChildWnd::OnPreviewClose()
{
	if(NULL==SDS_CMainWindow)
	{
		return;
	}
	SDS_CMainWindow->m_pIcadPrintDlg->m_prevTab=CPrintDialogSheet::SCALE_VIEW;
	SDS_CMainWindow->SendMessage(WM_COMMAND,AFX_ID_PREVIEW_CLOSE,0);

	//Modified Cybage AW 15/10/2001[
	//Bug no : 77870 from BugZilla (Crash during print preview )
	//Reason : Following commented lines are added in CIcadView::OnEndPrintPreview() function.
	/*
	if(NULL!=SDS_CMainWindow->m_pIcadPrintDlg)
	{
		sds_ssfree(SDS_CMainWindow->m_pIcadPrintDlg->m_scaleViewTab.m_ssPrintEnt);
		sds_ssadd(NULL,NULL,SDS_CMainWindow->m_pIcadPrintDlg->m_scaleViewTab.m_ssPrintEnt);
	}
	*/
	//Modified Cybage AW 15/10/2001]
}

void CIcadChildWnd::OnPreviewHelp()
{
	CommandRefHelp( m_hWnd, HLP_PPREVIEW);
}

void CIcadChildWnd::OnChildWndClose(void)
{
	ASSERT_VALID( this );
	OnClose();
}

afx_msg void CIcadChildWnd::OnClose()
{
	ASSERT_VALID( this );
	CIcadDoc* pDoc = (CIcadDoc*)GetActiveDocument();
	if(NULL!=pDoc)
	{
		ASSERT_VALID( pDoc );
		CView* pView=GetActiveView();
		COleClientItem* pActiveItem = pDoc->GetInPlaceActiveItem(pView);
		if(NULL!=pActiveItem)
		{
			sds_alert(ResourceString(IDC_ICADCHILDWND_PLEASE_SHUT_3, "Please shut down the in-place editing session before closing\nthis window." ));
			return;
		}
		if(pDoc->IsEmbedded() && ::InSendMessage())
		{
			//*** Post a WCLOSE and return from here to avoid
			//*** RPC_E_CANTCALLOUT_ININPUTSYNCCALL.  This will probably
			//*** throw off WCLOSEALL.
			::PostUIAction( ICAD_WNDACTION_WCLOSE, 0L );
			return;
		}
	}
	extern bool cmd_InsideOfCloseCommand;
	extern bool SDS_CurrentlyUndoing;
	if(SDS_IsDragging || (!cmd_InsideOfCloseCommand && !cmd_InsideViewPorts && !cmd_InsideOpenCommand && !cmd_InsideNewCommand && !SDS_CurrentlyUndoing && !cmd_InsideCreateNewViewports)) {
		SDS_SendMessage(WM_CHAR,27,0);
		SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C^C_WCLOSE"/*DNT*/);
        SDS_CMainWindow->m_IgnoreLastCmd=true;
		return;
	}

	if(GetActiveView() && ((CIcadView *)GetActiveView())->m_pViewsDoc) {
		m_pDrawingAtClose=((CIcadView *)GetActiveView())->m_pViewsDoc->m_dbDrawing;
	}
	if(GetActiveView()) {
		m_pTabRecAtClose=((CIcadView *)GetActiveView())->m_pVportTabHip;
	}

	CMDIChildWnd::OnClose();
}

// catch document activation
afx_msg void CIcadChildWnd::OnChildActivate()
	{
	CIcadDoc *current = SDS_CURDOC;
	CWnd::OnChildActivate();

	//Set the SDS_CURDOC variable so that the DocChangeCallback will be set correctly.
	SDS_CMainWindow->m_pCurDoc = (this->m_pViewActive) ? (CIcadDoc *)this->m_pViewActive->GetDocument() : SDS_CURDOC;
	if ( SDS_CURDOC != current )
		SDS_CMainWindow->DocChangeCallback( (CIcadView *)GetActiveView() );
	}

// 4M Elias Item:50->
afx_msg void CIcadChildWnd::OnSizing( UINT nSide, LPRECT pRect )
{
	ASSERT_VALID( this );
	CMDIChildWnd::OnSizing( nSide, pRect );

	int nHeight=(pRect->bottom)-(pRect->top);
	// make height not less than 40 pixels
	nHeight = max( nHeight, 40 );
	pRect->bottom = pRect->top + nHeight;
}
//<-4M Elias Item:50

afx_msg void CIcadChildWnd::OnSize(UINT nType, int cx, int cy)
{
	ASSERT_VALID( this );
	struct sds_resbuf rb;
	rb.restype=RTSHORT;

	CMDIChildWnd::OnSize(nType,cx,cy);
	if(nType==SIZE_MAXIMIZED && IsWindowVisible() &&
		SDS_CMainWindow->IsWindowVisible())
	{
		rb.resval.rint=2;
		SDS_setvar(NULL,DB_QWNDLMDI,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	}
	else if(nType==SIZE_RESTORED && IsWindowVisible() &&
		SDS_CMainWindow->IsWindowVisible())
	{
		rb.resval.rint=1;
		SDS_setvar(NULL,DB_QWNDLMDI,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	}

	GetWindowRect( &m_WindowRect);
	m_WindowRect.bottom=cy+m_WindowRect.top+29;
	m_WindowRect.right=cx+m_WindowRect.left+10;
}

afx_msg void CIcadChildWnd::OnMove(int x, int y)
{
	ASSERT_VALID( this );
	CMDIChildWnd::OnMove(x,y);
	((CIcadView *)GetActiveView())->CalcViewPoints(1);
	m_WindowRect.top=y-25;
	m_WindowRect.left=x-6;
}

int CIcadChildWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CIcadChildWnd::OnEraseBkgnd(CDC* pDC)
{
	// Override and just return so we don't see a white
	// backgroung flash when opening a new view.
	return(TRUE);
}

void CIcadChildWnd::UpdateLayoutTabs(db_drawing* pDrawing, BOOL bInit)
{
	ASSERT(GetActiveFrame() == NULL || this == GetActiveFrame());
	ASSERT(GetActiveDocument() == NULL || pDrawing == ((CIcadDoc*)GetActiveDocument())->m_dbDrawing);
	m_layoutTabs.UpdateLayoutTabs(pDrawing, bInit);
}

void CIcadChildWnd::RecalcLayout(BOOL bNotify /*= TRUE*/)
{
	CMDIChildWnd::RecalcLayout(bNotify);

#ifdef _LAYOUTTABS_	
	CIcadView* pView = ((CMainWindow*)GetMDIFrame())->GetIcadView();
	if(pView && pView->GetSafeHwnd() && pView->GetParent()->GetSafeHwnd() == m_wndSplitter.GetSafeHwnd())
	{
		// only child view should be resized
		ASSERT(pView == m_wndSplitter.GetPane(0, 0));
		RECT cr;
		GetClientRect(&cr);
		if(pView->m_showScrollBars)
		{
			pView->MoveWindow(0, 0, cr.right - GetSystemMetrics(SM_CXVSCROLL) - 2, cr.bottom - GetSystemMetrics(SM_CYHSCROLL) - 2);
			pView->GetScrollBarCtrl(SB_HORZ)->MoveWindow(cr.right / 2, cr.bottom - GetSystemMetrics(SM_CYHSCROLL) - 2,
				cr.right / 2 - GetSystemMetrics(SM_CXVSCROLL) - 2, GetSystemMetrics(SM_CYHSCROLL));
			if(m_layoutTabs.GetSafeHwnd())
				m_layoutTabs.MoveWindow(0, cr.bottom - GetSystemMetrics(SM_CYHSCROLL) - 2, cr.right / 2, GetSystemMetrics(SM_CYHSCROLL));
		}
		else
		{
			pView->MoveWindow(0, 0, cr.right, cr.bottom - GetSystemMetrics(SM_CYHSCROLL) - 2);
			if(m_layoutTabs.GetSafeHwnd())
				m_layoutTabs.MoveWindow(0, cr.bottom - GetSystemMetrics(SM_CYHSCROLL) - 2, cr.right, GetSystemMetrics(SM_CYHSCROLL));
		}
	}
#endif
}

