/* D:\ICADDEV\PRJ\ICAD\ICADPROMPTMENU.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!


#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "IcadPromptMenu.h"/*DNT*/
#include "commandatom.h" /*DNT*/
#include "lisp.h"/*DNT*/
#include "IcadView.h"

/////////////////////////////////////////////////////////////////////
// CPromptMenu message map and member (huh-huh) functions

IMPLEMENT_DYNAMIC(CPromptMenu, CMiniFrameWnd)

BEGIN_MESSAGE_MAP(CPromptMenu, CMiniFrameWnd)
	//{{AFX_MSG_MAP(CPromptMenu)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_CHAR()
	ON_WM_MOVE()
	ON_WM_CLOSE()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CPromptMenu::CPromptMenu() : m_pTrackWnd( NULL)
{
	m_pList = new CPtrList();
}

CPromptMenu::~CPromptMenu()
{
	//*** Free all the structures in the pList
	POSITION pos = m_pList->GetHeadPosition();
	while(pos!=NULL)
	{
		LPPMITEM pItem = (LPPMITEM)m_pList->GetAt(pos);
		delete pItem->pstr;
		delete pItem->pcmd;
		delete m_pList->GetNext(pos);
	}
	delete m_pList;
	delete m_pTrackWnd;
}

BOOL CPromptMenu::CreatePromptMenu(CPoint point, CMainWindow* pMain)
{
	BOOL ret;
	//*** Create the mini-frame
	CRect rectEmpty(point.x,point.y,point.x+125,point.y+125);
	if((ret=CMiniFrameWnd::Create(NULL, "PromptMenu"/*DNT*/,
		WS_POPUP | WS_CAPTION | WS_SYSMENU | MFS_SYNCACTIVE,
		rectEmpty,
		AfxGetMainWnd() //*** Parent frame window may be CMainWindow or CIcadIpFrame
		))==0) return 0;

	m_pMain=pMain;
	CRect rectClient;
	GetClientRect(rectClient);
	m_nExtraX=125-rectClient.right;
	m_nExtraY=125-rectClient.bottom;
	return ret;
}

void CPromptMenu::AppendMenu(LPCTSTR pStr,LPCTSTR pCmd)
{
	//*** Add the string to the menu list.
	LPPMITEM pItem = new PMITEM;
	pItem->pstr = new CString(pStr);
	pItem->pcmd = new CString(pCmd);
	m_pList->AddTail(pItem);
}

BOOL CPromptMenu::TrackPromptMenu(void)
{
	char *tmpPtr = NULL;
	struct resbuf rb;
	rb.resval.rstring=NULL;
	SDS_getvar(NULL,DB_QCMDNAMES,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rstring==NULL)
		return 1;

	// Here, if there is a localized name for the menu caption, we will retrieve it and
	// use it if not, we will simply pass on rb.resval.rstring.
	class commandAtomObj *menuCaption;
	tmpPtr = strrchr(rb.resval.rstring,'\''/*DNT*/) ? strrchr(rb.resval.rstring,'\''/*DNT*/)+1 : rb.resval.rstring;
	ASSERT(tmpPtr != NULL);

	menuCaption = lsp_findatom(tmpPtr, 0);
	if (menuCaption != NULL && menuCaption->LName != NULL && strnicmp(menuCaption->LName,"C:",2))
		SetWindowText(menuCaption->LName);
	else
	{	/*D.G.*/// try to find a global underscored name...
		CString	globalName = "_";
		globalName += tmpPtr;
		menuCaption = lsp_findatom(globalName, 0);
		if (menuCaption != NULL && menuCaption->LName != NULL)
			SetWindowText(menuCaption->LName);
		else
		{
			struct resbuf rbLastPrompt;
			SDS_getvar(NULL,DB_QLASTPROMPT,&rbLastPrompt,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if (rbLastPrompt.resval.rstring)
			{
				char* pBegin = rbLastPrompt.resval.rstring;
				while(*pBegin && *pBegin == '\n')
					++pBegin;
				char* pEnd = strchr(pBegin, '\n');
				if(pEnd)
					*pEnd = '\0';
				SetWindowText(*pBegin ? pBegin : tmpPtr);		
				IC_FREE(rbLastPrompt.resval.rstring);
			}
			else
				SetWindowText(tmpPtr);
		}
	}

	IC_FREE(rb.resval.rstring);

	//*** Calculate the size of the frame window
	//Get width of title - title is in ALLCAPS so use max width
	CString title;
	GetWindowText(title);
	CSize textSize;
	CDC* pDC = GetDC();
	CFont* pOldFont = (CFont *)pDC->SelectStockObject(DEFAULT_GUI_FONT);
	TEXTMETRIC tm;
	pDC->GetOutputTextMetrics(&tm);
	textSize=pDC->GetTextExtent(title);
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);

	LONG lFontHeight=(tm.tmHeight+(tm.tmHeight/3));
	int nHeight = lFontHeight * m_pList->GetCount();

	//*** Calc width based on string length
	POSITION pos = m_pList->GetHeadPosition();
	LPPMITEM item;

	//Get width of title - title is in ALLCAPS so use max width
//	CString title;
//	GetWindowText(title);
//	int titleLength = title.GetLength() * tm.tmMaxCharWidth;
	int titleLength = textSize.cx*1.25+24;//(title.GetLength()+6) * tm.tmAveCharWidth;

	// Get maximum length of strings in list
	int numChars = 0;
	while (pos)
	{
		item = (LPPMITEM) m_pList->GetNext(pos);

		if (item->pstr->GetLength() > numChars)
			numChars = item->pstr->GetLength();
	}

	//Add extra spacing since we are using the average width
	//Maximum width leaves too much room for long items
	int maxLength = (numChars + 6) * tm.tmAveCharWidth;

	CRect rect;
	GetWindowRect(&rect);
	rect.right=(rect.left+max(titleLength, maxLength));
	rect.bottom=(rect.top+nHeight+m_nExtraY+tm.tmDescent);
	MoveWindow(rect);
	GetClientRect(rect);

	//*** Create the prompt menu track window
	m_pTrackWnd = new CPromptMenuWnd();
	m_pTrackWnd->m_pList=m_pList;
	m_pTrackWnd->m_lFontHeight=lFontHeight;
	m_pTrackWnd->m_lFontDescent=tm.tmDescent;
	m_pTrackWnd->Create(this,&rect);

	ShowWindow(SW_SHOW);
	UpdateWindow();

	return 1;
}

void CPromptMenu::Hide(void)
{
	if(m_pTrackWnd!=NULL)
	{
		ReleaseCapture();
	}
	ShowWindow(SW_HIDE);
}

afx_msg BOOL CPromptMenu::OnEraseBkgnd(CDC* pDC)
{
	return 1;
}

afx_msg void CPromptMenu::OnPaint(void)
{
	PAINTSTRUCT ps;
	CDC *pDC;
	pDC=BeginPaint(&ps);
	EndPaint(&ps);
	return;
}

afx_msg void CPromptMenu::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//AG: delegate tthis message to active view to be processed by OsnapManager
	if( m_pMain && m_pMain->m_pCurView && (nChar == 9 || nChar == 27) )
		m_pMain->m_pCurView->processOsnapChar( nChar );

	if(NULL!=m_pMain && m_pMain->m_bSentChar==FALSE)
	{
		SDS_SendMessage(WM_CHAR,nChar,nFlags);
	}
	return;
}

afx_msg void CPromptMenu::OnMove(int x, int y)
{
	CRect rect;
	GetWindowRect(rect);
	struct resbuf rb;
	rb.restype=RTPOINT;
	rb.resval.rpoint[0]=(double)rect.left;
	rb.resval.rpoint[1]=(double)rect.top;
	rb.resval.rpoint[2]=0.0;
	sds_setvar("WNDPPMENU"/*DNT*/,&rb);
}

afx_msg void CPromptMenu::OnClose(void)
{
	ShowWindow(SW_HIDE);
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Send WM_MOUSEWHEEL message to SDS_CURVIEW.
 * Returns:	TRUE.
 ********************************************************************************/
BOOL
CPromptMenu::OnMouseWheel
(
 UINT	nFlags,
 short	zDelta,
 CPoint	pt
)
{
	// WM_MOUSEWHEEL message can come here during transparent using mouse wheel.
	// So, send it to the current view.

	// Get just the latest wheel rotation.
	bool	bFoundNewerMouseWheel = false;
	MSG		msgLookAhead;
	HWND	hWnd = GetSafeHwnd();

	while(::PeekMessage(&msgLookAhead, hWnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE))
		bFoundNewerMouseWheel = true;	// Just get the latest mouse move

	if(bFoundNewerMouseWheel)
	{
		// EBATECH(CNBR) 2001-06-24 Add  CAST operator
		nFlags = (UINT)LOWORD(msgLookAhead.wParam);
		zDelta = (short)HIWORD(msgLookAhead.wParam);
		pt.x = (int)(short)LOWORD(msgLookAhead.lParam);
		pt.y = (int)(short)HIWORD(msgLookAhead.lParam);
	}

	// Send a message.
	if(SDS_CURVIEW)
		::SendMessage(SDS_CURVIEW->GetSafeHwnd(), WM_MOUSEWHEEL, MAKELONG(nFlags, zDelta), MAKELONG(pt.x, pt.y));

	return TRUE;
}


/////////////////////////////////////////////////////////////////////
// CPromptMenuWnd message map and member (huh-huh) functions

IMPLEMENT_DYNAMIC(CPromptMenuWnd, CWnd)

BEGIN_MESSAGE_MAP(CPromptMenuWnd, CWnd)
	//{{AFX_MSG_MAP(CPromptMenuWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CPromptMenuWnd::CPromptMenuWnd()
{
	m_pbrBkgnd = new CBrush(GetSysColor(COLOR_BTNFACE));
	m_pbrHilite = new CBrush(GetSysColor(COLOR_HIGHLIGHT));
	m_nPrevItem=-1;
	m_bGotLButtonDown=FALSE;
	m_bGotRButtonDown=FALSE;
}

CPromptMenuWnd::~CPromptMenuWnd()
{
	delete m_pbrBkgnd;
	delete m_pbrHilite;
}

BOOL CPromptMenuWnd::Create(CWnd* pParent, LPCRECT lpRect)
{
	BOOL ret;
	m_rectClient=*lpRect;

	//*** Set the rects for all items
	LPPMITEM pItem;
	POSITION pos = m_pList->GetHeadPosition();
	int ct;
	int nPosY=m_lFontDescent;
	for(ct=0; pos!=NULL; ct++)
	{
		pItem=(LPPMITEM)m_pList->GetNext(pos);
		pItem->rect=m_rectClient;
		pItem->rect.top=nPosY;
		if(pItem->pstr->Compare("~"/*DNT*/)==0)
		{
			nPosY+=(m_lFontHeight/2);
			pItem->rect.bottom=nPosY;
		}
		else
		{
			nPosY+=m_lFontHeight;
			pItem->rect.bottom=nPosY;
		}
	}
	m_rectClient.bottom=pItem->rect.bottom;

	//*** Resize the parent window
	CRect rect;
	pParent->GetWindowRect(rect);
	rect.bottom=(rect.top+((CPromptMenu*)pParent)->m_nExtraY+pItem->rect.bottom);
	pParent->MoveWindow(rect);

	//*** Create the window
	if((ret=CWnd::Create(NULL,
		NULL,
		WS_CHILD,
		m_rectClient,
		pParent,
		0))==0) return 0;

	ShowWindow(SW_SHOW);
	UpdateWindow();
	return ret;
}

//////////////////////////////////////////////////////////////////////
// Painting functions
//
afx_msg BOOL CPromptMenuWnd::OnEraseBkgnd(CDC* pDC)
{
	return 1;
}

afx_msg void CPromptMenuWnd::OnPaint(void)
{
	PAINTSTRUCT ps;
	CDC *pDC;
	pDC=BeginPaint(&ps);
	DrawMenu(pDC);
	EndPaint(&ps);
	return;
}

void CPromptMenuWnd::DrawMenu(CDC* pDC)
{
	pDC->FillRect(m_rectClient,m_pbrBkgnd);
	CFont* pOldFont = (CFont *) pDC->SelectStockObject(DEFAULT_GUI_FONT);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
	POSITION pos = m_pList->GetHeadPosition();
	while(pos!=NULL)
	{
		LPPMITEM pItem = (LPPMITEM)m_pList->GetNext(pos);
		if(pItem->pstr->Compare("~"/*DNT*/)==0)
		{
			DrawSpacer(pDC,pItem->rect);
		}
		else if(pItem->pstr->Compare("`"/*DNT*/)==0)
		{
			pDC->DrawText(ResourceString(IDC_ICADPROMPTMENU_DONE_5, "Done"/*DNT*/),pItem->rect,DT_VCENTER | DT_SINGLELINE | DT_LEFT);
//			pDC->TextOut(5,pItem->rect.top+m_lFontDescent,"Done");
		}
		else
		{
			pDC->DrawText(*pItem->pstr,pItem->rect,DT_VCENTER | DT_SINGLELINE | DT_LEFT);
//			pDC->TextOut(5,pItem->rect.top+m_lFontDescent,pItem->pstr);
		}
	}
	pDC->SelectObject(pOldFont);
}

void CPromptMenuWnd::DrawSpacer(CDC* pDC, LPCRECT lpRect)
{
	CPoint point;
	CRect rect(lpRect);
	CPen *pPen,*pOldPen;

	pDC->FillRect(rect,m_pbrBkgnd);

	pPen=new CPen(PS_SOLID,1,GetSysColor(COLOR_3DSHADOW));
	pOldPen=pDC->SelectObject(pPen);
	point.x=rect.left;
	point.y=(rect.top+((rect.bottom-rect.top)/2)-1);
	pDC->MoveTo(point);
	point.x=rect.right+1;
	pDC->LineTo(point);
	pDC->SelectObject(pOldPen);
	delete pPen;

	pPen=new CPen(PS_SOLID,1,GetSysColor(COLOR_3DHILIGHT));
	pOldPen=pDC->SelectObject(pPen);
	point.x=rect.left;
	point.y++;
	pDC->MoveTo(point);
	point.x=rect.right+1;
	pDC->LineTo(point);
	pDC->SelectObject(pOldPen);
	delete pPen;
}

//////////////////////////////////////////////////////////////////////
// Mouse move and highlighting functions
//
afx_msg void CPromptMenuWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags,point);
	if(point.x<m_rectClient.left || point.x>m_rectClient.right ||
		point.y<m_rectClient.top || point.y>m_rectClient.bottom)
	{
		ReleaseCapture();
		return;
	}
	if(point.y<=2) return;
	int nItem = CalcIndexFromPt(point);
	if(m_nPrevItem==nItem) return;
	if(m_nPrevItem>=0) DrawItem(m_nPrevItem,0);
	DrawItem(nItem,1);
	m_nPrevItem=nItem;
}

afx_msg BOOL CPromptMenuWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	m_prevCursor=(::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW)));
	SetCapture();
	return CWnd::OnSetCursor(pWnd,nHitTest,message);
}

void CPromptMenuWnd::OnCaptureChanged(CWnd *pWnd)
{
	::SetCursor(m_prevCursor);
	if(m_nPrevItem>=0)
	{
		DrawItem(m_nPrevItem,0);
		m_bGotLButtonDown=FALSE;
		m_bGotRButtonDown=FALSE;
		m_nPrevItem=(-1);
	}
	CWnd::OnCaptureChanged(pWnd);
}

void CPromptMenuWnd::DrawItem(int nIndex, int nHilite)
{
	LPPMITEM pItem = (LPPMITEM)m_pList->GetAt(m_pList->FindIndex(nIndex));
	if(pItem->pstr->Compare("~"/*DNT*/)==0) return;

	CRect rect(pItem->rect);
	CDC* pDC = GetDC();
	COLORREF crPrevTextColor;
	if(nHilite)
	{
		pDC->FillRect(rect,m_pbrHilite);
		crPrevTextColor=pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
	}
	else
	{
		pDC->FillRect(rect,m_pbrBkgnd);
		crPrevTextColor=pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
	}
	CFont* pOldFont = (CFont *) pDC->SelectStockObject(DEFAULT_GUI_FONT);
	COLORREF crPrevBkMode = pDC->SetBkMode(TRANSPARENT);

	int nPosY=(rect.top+m_lFontDescent);
	if(pItem->pstr->Compare("`"/*DNT*/)==0)
	{
		pDC->DrawText(ResourceString(IDC_ICADPROMPTMENU_DONE_5, "Done"/*DNT*/),pItem->rect,DT_VCENTER | DT_SINGLELINE | DT_LEFT);
	}
	else
	{
		pDC->DrawText(*pItem->pstr,pItem->rect,DT_VCENTER | DT_SINGLELINE | DT_LEFT);
	}

	pDC->SelectObject(pOldFont);
	pDC->SetBkMode(crPrevBkMode);
	pDC->SetTextColor(crPrevTextColor);
	ReleaseDC(pDC);
}

void CPromptMenuWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	//*** We trap this message and set this variable because sometimes we get
	//*** the LButtonUp from some another windows LButtonDown.
	m_bGotLButtonDown=TRUE;
	CWnd::OnLButtonDown(nFlags, point);
}

void CPromptMenuWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags,point);
	if(!m_bGotLButtonDown) return;
	//*** Reset the lbutton variable.
	m_bGotLButtonDown=FALSE;
	int nIndex=CalcIndexFromPt(point);
	LPPMITEM pItem = (LPPMITEM)m_pList->GetAt(m_pList->FindIndex(nIndex));
	if(pItem->pstr->CompareNoCase(ResourceString(IDC_ICADPROMPTMENU_CANCEL_6, "CANCEL" ))==0)
	{
		SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C"/*DNT*/);
	}
 	else if(pItem->pstr->CompareNoCase("`"/*DNT*/)==0)
 	{
 		SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)""/*DNT*/);
	}
	else if(pItem->pstr->CompareNoCase("~"/*DNT*/)==0)
	{
	}
	else
	{
		//Bugzilla No. 78107 ; 15-04-2002 [
		/*
		CString cmd = *pItem->pcmd;
		int index = cmd.Find(',');
		if(index > 0)
			cmd=cmd.Left(index); 

		SDS_SendMessage(WM_MENUSELECT, 0, (LPARAM) (LPCTSTR)cmd);
		*/
		///*
		//*** Send only the upper case characters
		char* pStr = new char[pItem->pcmd->GetLength()+1];
		if(pStr==NULL)
		{
			MessageBox(ResourceString(IDC_ICADPROMPTMENU_ERROR__IN_9, "ERROR: Insufficient memory." ),
				NULL,
				MB_OK | MB_ICONEXCLAMATION);
			return;
		}
		strcpy(pStr,pItem->pcmd->GetBuffer(0));
		char* cp1 = pStr;

		int fi1;
		BOOL foundupper=0;

		for (fi1 = 0; cp1[fi1]; ++fi1)
		{
			if((isupper((unsigned char) cp1[fi1]) && ',' != cp1[fi1]) ||
				isdigit((unsigned char) cp1[fi1]) ||	/*D.G.*/// isdigit added (for VIEWPORTS command, for example)
				'?' == cp1[fi1] || // Martin Waardenburg 01-09-2000: Added question mark for "List External References" option in xref command, which sends a question mark.
				'-' == cp1[fi1] || '+' == cp1[fi1] ||		/*D.G.*/// '+' & '-' for the SELECT command options. 
				 '.' == cp1[fi1])  // MHB '.' for R2.5 on saveas popup menu. This used to return R2 which means we've never
								   //     saved as R2.6 for the menu selection - R2 return defaulted to R2.5 output file type. 
				foundupper = 1;
			else
			{
				if (foundupper)
				{
					cp1[fi1] = 0;
					break;
				}

				cp1++;
				fi1--;
			}
		}

		SDS_SendMessage(WM_MENUSELECT, 0, (LPARAM) cp1);
		delete pStr;
		//*/
		//Bugzilla No. 78107 ; 15-04-2002 ]
	}
}

void CPromptMenuWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	//*** We trap this message and set this variable because sometimes we get
	//*** the RButtonUp from some another windows RButtonDown.
	m_bGotRButtonDown=TRUE;
	CWnd::OnRButtonDown(nFlags, point);
}

afx_msg void CPromptMenuWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonUp(nFlags,point);
	if(!m_bGotRButtonDown) return;
	m_bGotRButtonDown=FALSE;
	SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)""/*DNT*/);
}

int CPromptMenuWnd::CalcIndexFromPt(CPoint point)
{
	if(point.y<=0 || m_rectClient.bottom<=0) return 0;

	POSITION pos = m_pList->GetHeadPosition();
	int nIndex;
	for(nIndex=0; pos!=NULL; nIndex++)
	{
		LPPMITEM pItem = (LPPMITEM)m_pList->GetNext(pos);
		if(point.y>=pItem->rect.top && point.y<pItem->rect.bottom) break;
	}
	int ct=m_pList->GetCount();
	if(nIndex>=ct) nIndex=(ct-1);
	return nIndex;
}
