/* G:\ICADDEV\PRJ\ICAD\ICADCMDBAR.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

/////////////////////////////////////////////////////////////////////
// K-POSS-A
// Icad Command Bar class

#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "IcadCmdBar.h"
#include "IcadToolBarMain.h"/*DNT*/ 

#define IDC_CMDBAR            1102

IMPLEMENT_DYNAMIC(CCmdBar, CSizingControlBar)

/////////////////////////////////////////////////////////////////////
// CCmdBar construction/destruction

CCmdBar::CCmdBar()
{
	m_nDockedWidth = NULL;
	m_dynSize =	CSize(0,0);
	m_pBorderWnd=NULL;
	m_pPromptWnd=NULL;
    m_szMinHorz = CSize(33, 32);
    m_szMinVert = CSize(33, 32);
    m_szMinFloat = CSize(37, 32);
    m_szHorz = CSize(200, 100);
    m_szVert = CSize(100, 200);
    m_szFloat = CSize(200, 200);
}

CCmdBar::~CCmdBar()
{
	m_pPromptWnd=NULL;
	if(NULL!=m_pBorderWnd)
	{
		delete m_pBorderWnd;
		m_pBorderWnd=NULL;
	}
	if(NULL!=m_pbrBkgnd)
	{
		delete m_pbrBkgnd;
		m_pbrBkgnd=NULL;
	}
}

BEGIN_MESSAGE_MAP(CCmdBar, CSizingControlBar)
	//{{AFX_MSG_MAP(CCmdBar)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_MOVE()
    ON_WM_CHAR()
    ON_WM_SYSCOLORCHANGE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CCmdBar::Create(CWnd* pParentWnd, LPCRECT lpRect)
{
	m_pParent=pParentWnd;
	CString csTitle;
	csTitle = ResourceString(AFX_IDS_APP_TITLE, "BtoCAD" );
	csTitle = csTitle + " " + ResourceString(IDC_ICADCMDBAR_INTELLICAD_CO_0, "Command Bar" );
 	if(!CSizingControlBar::Create(csTitle,pParentWnd,IDC_CMDBAR,0))
		return FALSE;
	ShowWindow(SW_SHOW);
	//*** Dock on top by default.
	((CMainWindow*)m_pParent)->EnableDocking(CBRS_ALIGN_ANY);
	((CMainWindow*)m_pParent)->DockControlBar(this,AFX_IDW_DOCKBAR_BOTTOM);
	//*** Resize the command bar to the size specified in Icad vars
	UpdateCmdBar();

    return TRUE;
}

int CCmdBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CSizingControlBar::OnCreate(lpCreateStruct)==(-1))
       return -1;
	//*** Set the style for the command bar
	SetSCBStyle(GetSCBStyle() | SCBS_SHOWEDGES | SCBS_SIZECHILD);
	SetBarStyle(GetBarStyle()|CBRS_SIZE_DYNAMIC|CBRS_TOOLTIPS | CBRS_FLYBY);
    EnableDocking(CBRS_ALIGN_BOTTOM);
    //*** Create the Prompt window
	m_pBorderWnd = new CIcadCmdBarBorderWnd();
    m_pBorderWnd->Create(this);
	m_pPromptWnd=m_pBorderWnd->m_pPromptWnd;
	m_pbrBkgnd = new CBrush(GetSysColor(COLOR_BTNFACE));
    
    return 0;
}

void CCmdBar::OnMove(int cx, int cy)
{
	//*** Set the Icad variable.
	((CMainWindow*)m_pParent)->SetMovePos(this,2);
}

void CCmdBar::OnSize(UINT nType, int cx, int cy)
{
    CRect rectClient;

	//*** Tell the children to resize
    GetClientRect(rectClient);
	rectClient.DeflateRect(2,1);
	m_pBorderWnd->MoveWindow(rectClient);

	return;
}

void CCmdBar::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    return;
}

void CCmdBar::UpdateCmdBar(void)
{
//*** This function updates the position, size, and location of the command bar
//*** from the icad variables.
	struct resbuf rb;
	CRect rectCmdBar;

	//*** Set the position and size of the command bar.
	sds_getvar("WNDPCMD"/*DNT*/,&rb);
	rectCmdBar.left=(long)rb.resval.rpoint[0];
	rectCmdBar.top=(long)rb.resval.rpoint[1];
	sds_getvar("WNDSCMD"/*DNT*/,&rb);
	rectCmdBar.right=(rectCmdBar.left+(long)rb.resval.rpoint[0]);
	rectCmdBar.bottom=(rectCmdBar.top+(long)rb.resval.rpoint[1]);
	//*** Set the default height if a height is not given
	if(rectCmdBar.IsRectNull()) 
	{
		//*** Create a temporary memory DC to get the height of the font in the CmdBar
		CDC* pDC=GetDC();
		CDC* pMemDC = new CDC();
		pMemDC->CreateCompatibleDC(pDC);
		TEXTMETRIC tm;
		pMemDC->SelectStockObject(ANSI_FIXED_FONT);
		pMemDC->GetTextMetrics(&tm);
		ReleaseDC(pDC);
		delete pMemDC;
		int nFontHgt=(int)tm.tmHeight;
		CRect rectMainWnd;
		m_pParent->GetWindowRect(&rectMainWnd);
		rectCmdBar=rectMainWnd;
		rectCmdBar.top=(rectCmdBar.bottom-(nFontHgt*4)-14);
		ScreenToClient(&rectCmdBar);
		MoveWindow(rectCmdBar);
		//*** Reset the new position and size to the Icad variables.
		struct resbuf rb;
		rb.restype=RTPOINT;
		rb.resval.rpoint[0]=(double)rectCmdBar.left;
		rb.resval.rpoint[1]=(double)rectCmdBar.top;
		SDS_setvar(NULL,DB_QWNDPCMD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		rb.resval.rpoint[0]=(double)(rectCmdBar.right-rectCmdBar.left);
		rb.resval.rpoint[1]=(double)(rectCmdBar.bottom-rectCmdBar.top);
		SDS_setvar(NULL,DB_QWNDSCMD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	} 
	//*** Show the control bar docked on top.
	sds_getvar("WNDLCMD"/*DNT*/,&rb);
	int nLocation = rb.resval.rint;
	switch(nLocation)
	{
		case 0: //*** Hide
			((CMainWindow*)m_pParent)->ShowControlBar((CControlBar *)this,FALSE,FALSE);
			break;
		case 1: //*** Floating
			{
				CControlBarInfo info;
				GetBarInfo(&info);
				CPoint ptCmdBar;
				if(info.m_ptMRUFloatPos.x!=CW_USEDEFAULT)
				{
					ptCmdBar=info.m_ptMRUFloatPos;
				}
				else
				{
					ptCmdBar=rectCmdBar.TopLeft();
				}
				((CMainWindow*)m_pParent)->FloatControlBar(this,ptCmdBar);
				((CMainWindow*)m_pParent)->ShowControlBar((CControlBar *)this,TRUE,FALSE);
			}
			break;
		case 2: //*** Upper
			((CMainWindow*)m_pParent)->DockControlBar(this,AFX_IDW_DOCKBAR_TOP);
			((CMainWindow*)m_pParent)->ShowControlBar((CControlBar *)this,TRUE,FALSE);
			break;
		case 3: //*** Lower
			((CMainWindow*)m_pParent)->DockControlBar(this,AFX_IDW_DOCKBAR_BOTTOM);
			((CMainWindow*)m_pParent)->ShowControlBar((CControlBar *)this,TRUE,FALSE);
			break;
		default:
			//*** Just in case.
			((CMainWindow*)m_pParent)->ShowControlBar((CControlBar *)this,TRUE,FALSE);
			break;
	}
	//*** Initialize the dynamic size member variables.
	m_dynSize.cx=m_nFloatWidth=rectCmdBar.right-rectCmdBar.left;
	m_dynSize.cy=rectCmdBar.bottom-rectCmdBar.top;
	return;
}

void CCmdBar::OnSysColorChange(void)
{	
	delete m_pbrBkgnd;
	m_pbrBkgnd = new CBrush(GetSysColor(COLOR_BTNFACE));
}

void CCmdBar::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CSizingControlBar::OnWindowPosChanged(lpwndpos);
	if(((CMainWindow*)m_pParent)->m_nIsClosing==1)
	{
		return;
	}
	struct resbuf rb;
	rb.restype=SDS_RTSHORT;
	if(lpwndpos->flags & SWP_HIDEWINDOW)
	{
		rb.resval.rint=0;
	}
	else
	{
		if(GetParentFrame()!=m_pParent)
		{
			rb.resval.rint=1;
		}
		else
		{
			if(m_dwStyle & CBRS_ALIGN_TOP)
			{
				rb.resval.rint=2;
			}
			else
			{
				rb.resval.rint=3;
			}
		}
	}
	SDS_setvar("WNDLCMD"/*DNT*/,-1,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
}

void CCmdBar::DoPaint(CDC* pDC)
{
	// paint inside client area
	CRect rect;
	GetClientRect(rect);
	pDC->FillRect(rect,m_pbrBkgnd);

#ifdef ICAD_OLDTOOLBARS
	DrawBorders(pDC,rect);
#else
	CRect rectDraw(rect);
	COLORREF clrHilite = ::GetSysColor(COLOR_BTNHILIGHT);
	COLORREF clrShadow = ::GetSysColor(COLOR_BTNSHADOW);
	COLORREF clrFace = ::GetSysColor(COLOR_BTNFACE);
	int cxBorder=1,cyBorder=1;
	int cxBorder2=2,cyBorder2=2;

	DWORD dwStyle = m_dwStyle;
	if(dwStyle & CBRS_BORDER_ANY)
	{
		// prepare for dark lines
		ASSERT(rectDraw.top == 0 && rectDraw.left == 0);
		COLORREF clr=clrHilite;

		// draw left and top
		if (dwStyle & CBRS_BORDER_LEFT)
			pDC->FillSolidRect(0, rectDraw.top, cxBorder, rectDraw.Height(), clr);
		if (dwStyle & CBRS_BORDER_TOP)
			pDC->FillSolidRect(0, 0, rectDraw.right, cyBorder, clr);

		clr = clrShadow;
		// draw right and bottom
		if (dwStyle & CBRS_BORDER_RIGHT)
			pDC->FillSolidRect(rectDraw.right, rectDraw.top, -cxBorder, rectDraw.Height(), clr);
		if (dwStyle & CBRS_BORDER_BOTTOM)
			pDC->FillSolidRect(0, rectDraw.bottom, rectDraw.right, -cyBorder, clr);
	}
#endif

}

CSize CCmdBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	CSize size=CSizingControlBar::CalcDynamicLayout(nLength,dwMode);
   //*** Set the dynamic size variable.
    if(dwMode==(LM_HORZ | LM_LENGTHY)) {
        m_dynSize.cy=nLength;
		//*** Set the Icad variable
		struct resbuf rb;
		sds_getvar("WNDSCMD"/*DNT*/,&rb);
		rb.resval.rpoint[1]=(double)nLength;
		SDS_setvar(NULL,DB_QWNDSCMD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		m_szFloat=m_dynSize;
        return size;
    } else if(dwMode==LM_HORZ) {
		m_dynSize.cx=m_nFloatWidth=nLength;
		//*** Set the Icad variable
		struct resbuf rb;
		sds_getvar("WNDSCMD"/*DNT*/,&rb);
		rb.resval.rpoint[0]=(double)nLength;
		SDS_setvar(NULL,DB_QWNDSCMD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		m_szFloat=m_dynSize;
        return size;
    } else if(dwMode==(LM_HORZ | LM_HORZDOCK)) {
		if(nLength==(-1)) {
			m_dynSize.cx=m_nDockedWidth;
		} else {
			m_dynSize.cx=m_nFloatWidth;
		}
		m_szFloat=m_dynSize;
        return size;
	}
	m_szFloat=m_dynSize;
	return size;
}

void CCmdBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	return;
}

void CCmdBar::OnContextMenu(CWnd* pWnd, CPoint point) 
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

/////////////////////////////////////////////////////////////////////////////
// CIcadCmdBarBorderWnd

IMPLEMENT_DYNAMIC(CIcadCmdBarBorderWnd, CWnd)

CIcadCmdBarBorderWnd::CIcadCmdBarBorderWnd()
{
	m_pParentWnd=NULL;
	m_pPromptWnd=NULL;
	m_pbrBkgnd=NULL;
}

CIcadCmdBarBorderWnd::~CIcadCmdBarBorderWnd()
{
	m_pParentWnd=NULL;
	if(NULL!=m_pPromptWnd)
	{
		delete m_pPromptWnd;
		m_pPromptWnd=NULL;
	}
	if(NULL!=m_pbrBkgnd)
	{
		delete m_pbrBkgnd;
		m_pbrBkgnd=NULL;
	}
}


BEGIN_MESSAGE_MAP(CIcadCmdBarBorderWnd, CWnd)
	//{{AFX_MSG_MAP(CIcadCmdBarBorderWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CIcadCmdBarBorderWnd message handlers

BOOL CIcadCmdBarBorderWnd::Create(CWnd* pParentWnd) 
{
	m_pParentWnd=pParentWnd;

    CRect rectEmpty;
    rectEmpty.SetRectEmpty();
	BOOL ret=CWnd::Create(NULL,NULL,
        WS_CHILD | WS_CLIPCHILDREN,
        rectEmpty,
        m_pParentWnd,
        0);
	if(ret==FALSE) return FALSE;
	ShowWindow(SW_SHOW);
	return ret;
}

int CIcadCmdBarBorderWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ModifyStyleEx(0,WS_EX_CLIENTEDGE);
    //*** Create the Prompt window
	m_pPromptWnd = new CIcadPromptWnd();
    m_pPromptWnd->Create(this);
	m_pbrBkgnd = new CBrush(GetSysColor(COLOR_WINDOW));
	return 0;
}

BOOL CIcadCmdBarBorderWnd::OnEraseBkgnd(CDC* pDC) 
{
	return 1;
}

void CIcadCmdBarBorderWnd::OnPaint() 
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

void CIcadCmdBarBorderWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	//*** Resize the prompt Windows.
	CRect rectClient;
	GetClientRect(rectClient);
    m_pPromptWnd->OnSize(rectClient);
	return;
}

void CIcadCmdBarBorderWnd::OnSysColorChange() 
{
	CWnd::OnSysColorChange();
	
	delete m_pbrBkgnd;
	m_pbrBkgnd = new CBrush(GetSysColor(COLOR_WINDOW));
	UpdateWindow();	
}



