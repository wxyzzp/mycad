/* D:\DEV\PRJ\ICAD\ICADTEXTSCRN.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

/////////////////////////////////////////////////////////////////////

#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "IcadTextScrn.h"

IMPLEMENT_DYNAMIC(CTextScreen, CFrameWnd)

/////////////////////////////////////////////////////////////////////
// CTextScrn message map and member (huh-huh) functions

BEGIN_MESSAGE_MAP(CTextScreen, CFrameWnd)
	//{{AFX_MSG_MAP(CTextScreen)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_MOVE()
    ON_WM_CHAR()
    ON_WM_CLOSE()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
	ON_WM_DROPFILES()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTextScreen::CTextScreen()
{
	m_pPromptWnd=NULL;
	m_pbrBkgnd=NULL;
}

CTextScreen::~CTextScreen()
{
    delete m_pPromptWnd;
    delete m_pbrBkgnd;
}

int CTextScreen::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if(CFrameWnd::OnCreate(lpCreateStruct)==(-1))
       return -1;

	//*** Add drag and drop capability
	ModifyStyleEx(0,WS_EX_ACCEPTFILES);
    //*** Create the Prompt window
	m_pPromptWnd = new CIcadPromptWnd();
    m_pPromptWnd->Create(/*0,*/this);
    
	m_pbrBkgnd = new CBrush(GetSysColor(COLOR_WINDOW));

    return 0;
}

void CTextScreen::OnMove(int cx, int cy)
{
 	//*** Set the Icad variable.
	((CMainWindow*)m_pParent)->SetMovePos(this,1);
}

void CTextScreen::OnSize(UINT nType, int cx, int cy)
{
    CRect rectClient;
    GetClientRect(rectClient);
    m_pPromptWnd->OnSize(rectClient);
	//*** Set the Icad variables.
	((CMainWindow*)m_pParent)->SetSize(nType,this,1);
}

void CTextScreen::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    return;
}

void CTextScreen::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	//*** Make sure one of the child windows received the WM_RBUTTONDOWN.
	if(NULL==m_pPromptWnd)
	{
		return;
	}
	if(NULL!=m_pPromptWnd->m_pPromptWndList && 
		m_pPromptWnd->m_pPromptWndList->m_bGotRButtonDown)
	{
		((CMainWindow*)m_pParent)->TextScreenMenu(point);
		m_pPromptWnd->m_pPromptWndList->m_bGotRButtonDown=FALSE;
	}
	else if(NULL!=m_pPromptWnd->m_pPromptWndEdit && 
		m_pPromptWnd->m_pPromptWndEdit->m_bGotRButtonDown)
	{
		((CMainWindow*)m_pParent)->TextScreenMenu(point);
		m_pPromptWnd->m_pPromptWndEdit->m_bGotRButtonDown=FALSE;
	}
}

void CTextScreen::OnClose(void)
{
	if(((CMainWindow*)m_pParent)->m_nIsClosing)
	{
		ShowWindow(SW_HIDE,FALSE);
	}
	else
	{
		ShowWindow(SW_HIDE,TRUE);
	}
    return;
}

BOOL CTextScreen::OnEraseBkgnd(CDC* pDC)
{
    return 1;
}

void CTextScreen::OnPaint(void)
{
    PAINTSTRUCT ps;
    CDC *pDC;
	CRect rectUpdate;

    if(GetUpdateRect(rectUpdate)!=0)
	{
        pDC=BeginPaint(&ps);
        pDC->FillRect(rectUpdate,m_pbrBkgnd);
        EndPaint(&ps);
    }
    return;
}

BOOL CTextScreen::ShowWindow(int nCmdShow, BOOL bUpdateVars)
{
	BOOL bRet = CFrameWnd::ShowWindow(nCmdShow);
	WINDOWPLACEMENT wp;
	if(nCmdShow==SW_HIDE || nCmdShow==SW_SHOWMINIMIZED || nCmdShow==SW_MINIMIZE)
	{
		m_pParent->SetFocus();
	}
	if(bUpdateVars)
	{
		//*** Set the SCREENMODE variable.
		GetWindowPlacement(&wp);
		struct resbuf rb;
		rb.restype=SDS_RTSHORT;
		if(nCmdShow==SW_HIDE) {
			rb.resval.rint=1;
			SDS_setvar(NULL,DB_QSCREENMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		} else {
			rb.resval.rint=0;
			SDS_setvar(NULL,DB_QSCREENMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		}
		//*** Set the WNDLTEXT variable.
		if(nCmdShow==SW_HIDE)
		{
			rb.resval.rint=0;
		}
		else
		{
			rb.resval.rint=1;
			if(wp.showCmd==SW_SHOWMINIMIZED || wp.showCmd==SW_MINIMIZE)
			{
				rb.resval.rint=2;
			}
			else if(wp.showCmd==SW_SHOWMAXIMIZED || wp.showCmd==SW_MAXIMIZE)
			{
				rb.resval.rint=3;
			}
		}
		SDS_setvar(NULL,DB_QWNDLTEXT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	}
	//*** Update the menu also
	extern char *SDS_UpdateSetvar;
	SDS_UpdateSetvar="WNDLTEXT"/*DNT*/;
	((CMainWindow*)m_pParent)->IcadWndAction(ICAD_WNDACTION_UDCHKSTAT);
	return bRet;
}

void CTextScreen::OnDropFiles(HDROP hDropInfo) 
{
	//*** Call the main window's OnDropFiles function.
	((CMainWindow*)m_pParent)->OnDropFiles(hDropInfo);
}

void CTextScreen::OnSysColorChange() 
{
	CFrameWnd::OnSysColorChange();
	
	delete m_pbrBkgnd;
	m_pbrBkgnd = new CBrush(GetSysColor(COLOR_WINDOW));
	UpdateWindow();	
}


