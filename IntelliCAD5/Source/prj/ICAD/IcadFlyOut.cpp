/* File  : <DevDir>\source\prj\icad\IcadFlyOut.cpp
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 */ 

#include "Icad.h"/*DNT*/
#include "IcadApi.h"/*DNT*/
#include "IcadToolBarMain.h"/*DNT*/
#include "IcadCustom.h"/*DNT*/
#include "IcadDoc.h"/*DNT*/

#ifdef _DEBUG

 #ifndef USE_SMARTHEAP
  #define new DEBUG_NEW
 #endif

#endif // _DEBUG


/////////////////////////////////////////////////////////////////////////////
// CIcadPopUp
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CIcadPopUp, CMiniFrameWnd)

BEGIN_MESSAGE_MAP(CIcadPopUp, CMiniFrameWnd)
	//{{AFX_MSG_MAP(CIcadPopUp)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIcadPopUp message handlers

BOOL
CIcadPopUp::OnEraseBkgnd
(
 CDC*	pDC
)
{
	CRect	rectClient;
	GetClientRect(rectClient);

	CBrush*	pbrBkgnd = new CBrush(GetSysColor(COLOR_BTNFACE));
	CPen	*pHilitePen = new CPen(PS_SOLID,0,GetSysColor(COLOR_BTNHILIGHT)),
			*pShadowPen = new CPen(PS_SOLID,0,GetSysColor(COLOR_BTNSHADOW)),
			*pOldPen = (CPen*)pDC->SelectObject(pHilitePen);

	pDC->FillRect(rectClient, pbrBkgnd);
	pDC->MoveTo(rectClient.left, rectClient.bottom - 1);
	pDC->LineTo(rectClient.left, rectClient.top);
	pDC->LineTo(rectClient.right - 1, rectClient.top);
	pDC->SelectObject(pShadowPen);

	pDC->LineTo(rectClient.right - 1, rectClient.bottom - 1);
	pDC->LineTo(rectClient.left, rectClient.bottom - 1);
	pDC->SelectObject(pOldPen);
	delete pHilitePen;
	delete pShadowPen;
	delete pbrBkgnd;

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CIcadFlyOut
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CIcadFlyOut, CToolBarCtrl)

BEGIN_MESSAGE_MAP(CIcadFlyOut, CToolBarCtrl)
	//{{AFX_MSG_MAP(CIcadFlyOut)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CAPTURECHANGED()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	/*D.Gavrilov*///OnRButtonDown added
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipText)
	ON_NOTIFY_EX(TTN_NEEDTEXTW, 0, OnToolTipTextW)
END_MESSAGE_MAP()

CIcadFlyOut::CIcadFlyOut() : m_pPopup(NULL), m_hcurToolBar(NULL), m_hcurToolBarDel(NULL), m_hcurFlyOutH(NULL),
							m_hcurFlyOutHR(NULL), m_hcurFlyOutV(NULL), m_hcurFlyOutVR(NULL), m_pToolTipCtrl(NULL),
							m_hbmImageWell(NULL), m_pTbMain(NULL), m_pLastButtonPick(NULL), m_pNestedFlyOut(NULL)
{
}

CIcadFlyOut::~CIcadFlyOut()
{
	delete m_pToolTipCtrl;

	if(m_hbmImageWell)
		::DeleteObject(m_hbmImageWell);
}


// CIcadFlyOut message handlers

BOOL
CIcadFlyOut::Create
(
 const RECT&	rect,
 CIcadTBbutton*	pibParent,
 BOOL			bReverseImages
)
{
	// Set the variable that tells whether we are running under Windows95 or NT 3.5.
	OSVERSIONINFO	osVer;
	osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osVer);
	if(osVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS || (osVer.dwPlatformId == VER_PLATFORM_WIN32_NT && osVer.dwMajorVersion >= 4))
		// Windows 95 or NT 4.0
		m_bWin95 = TRUE;
	else
		m_bWin95 = FALSE;

	CRect	rectWnd(rect);

	// Create the popup window that the toolbar will be a child of.
#ifdef ICAD_OLDTOOLBARS
	m_pPopup = new CMiniFrameWnd();
#else
	rectWnd.InflateRect(1, 1);
	m_pPopup = new CIcadPopUp();
#endif

	m_pPopup->Create(NULL, NULL, WS_POPUP | WS_VISIBLE | MFS_SYNCACTIVE, rectWnd, m_pTbMain->m_pParentFrame);

	// Create the toolbar.
	CRect	rectToolBar;
	m_pPopup->GetClientRect(rectToolBar);
#ifdef ICAD_OLDTOOLBARS
	if(!m_bWin95)
		rectToolBar.right += 2;
	rectToolBar.top -= 2;
#else
	rectToolBar.DeflateRect(1, 1);
#endif
	BOOL	ret;
	DWORD	dwStyle = WS_CHILD | WS_VISIBLE |	CCS_NODIVIDER | CCS_NOHILITE | CCS_NORESIZE | TBSTYLE_WRAPABLE;

	if(!(ret = CToolBarCtrl::Create(dwStyle, rectToolBar, m_pPopup, 0)))
		return ret;

#ifndef ICAD_OLDTOOLBARS
	ModifyStyle(0, TBSTYLE_FLAT);
#endif

	// Create the buttons - if we are a fly out - load button in top of flyout
	LoadFlyOutButtons(pibParent, bReverseImages);

	// Load the tooltips
	AddToolTips();

	m_bImagesReversed = bReverseImages;
	m_idPrevButton = -1;

	m_pTbLostCapture = SetCapture();

	m_pPopup->RedrawWindow(rectWnd, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

	RedrawWindow(rectToolBar, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

	return ret;
}

BOOL
CIcadFlyOut::PreTranslateMessage
(
 MSG*	pMsg
)
{
	if(pMsg->message == WM_MOUSEMOVE || pMsg->message == WM_LBUTTONUP)
		m_pToolTipCtrl->RelayEvent(pMsg);

	return CToolBarCtrl::PreTranslateMessage(pMsg);
}

void
CIcadFlyOut::LoadFlyOutButtons
(
 CIcadTBbutton*	pibParent,
 BOOL			bReverseImages
)
{
	// Set the sizes for the buttons for this toolbar.
	if(m_pTbMain->m_bLargeButtons)
	{
		SetBitmapSize(CSize(ICAD_LG_IMGX, ICAD_LG_IMGY));
		SetButtonSize(CSize(ICAD_LG_BUTTONX, ICAD_LG_BUTTONY));
	}
	else
	{
		SetBitmapSize(CSize(ICAD_SM_IMGX, ICAD_SM_IMGY));
		SetButtonSize(CSize(ICAD_SM_BUTTONX, ICAD_SM_BUTTONY));
	}

	HINSTANCE	hResDLL = NULL;
	char*		pszAltPath = NULL;

	if(!m_strMnuFileName.IsEmpty())
	{
		pszAltPath = m_strMnuFileName.GetBuffer(0);

		// Look for a resource dll and load it.
		char*	pszResDLL = new char[m_strMnuFileName.GetLength() + 5];	// +5 just in case an extension isn't specified.
		strcpy(pszResDLL, m_strMnuFileName);
		ic_setext(pszResDLL, "dll"/*DNT*/);
		hResDLL = LoadLibrary(pszResDLL);
		delete [] pszResDLL;
	}

	// Loop through once to get a count of all buttons for this toolbar and to set
	// set a pointer to the first visible flyout button.
	CIcadTBbutton	*pibCur, *pibFirstVisible = NULL;
	int				nButtons;
	long			lflgVisibility = m_pTbMain->m_lflgVisibility;

	if(IsNested())
		pibCur = pibParent->m_pibCurFlyOut;
	else
		pibCur = pibParent->m_pibFlyOut;

	for(nButtons = 0; pibCur; pibCur = pibCur->m_pibFlyOut)
	{
		if(!pibFirstVisible && ((lflgVisibility & pibCur->m_lflgVisibility) == lflgVisibility))
			pibFirstVisible = pibCur;
		++nButtons;
	}

/*D.Gavrilov*/// don't make this extra place
//	++nButtons;	// Add one for extra place at top

	// Allocate the array of button structures.
	HANDLE procHeap = GetProcessHeap();
	LPTBBUTTON	pTbButton = (LPTBBUTTON)HeapAlloc(procHeap, HEAP_ZERO_MEMORY, nButtons * sizeof(TBBUTTON));

	// Fill the top button - !!!!!!!!!!!! will need to replace pibParent->m_pibFlyOut with last used button
	if(!m_pLastButtonPick)
		m_pLastButtonPick = pibParent->m_pibCurFlyOut;

	int	idxParent = bReverseImages ? nButtons - 1 : 0;	/*D.Gavrilov*/// for true reversing

	// Top spot is special - it needs to have the button directly (instead of flyout) since it won't bring up the fly out
	if(m_pLastButtonPick->m_bChgParentBtnImg && m_pLastButtonPick->m_pibCurFlyOut)
	{
		pTbButton[idxParent].iBitmap = m_pTbMain->AddIcadBitmap(m_pLastButtonPick->m_pibCurFlyOut, 1, NULL, NULL, pszAltPath, hResDLL);
		pTbButton[idxParent].dwData = (DWORD)m_pLastButtonPick->m_pibCurFlyOut;
	}
	else
	{
		pTbButton[idxParent].iBitmap = m_pTbMain->AddIcadBitmap(m_pLastButtonPick, 0, NULL, NULL, pszAltPath, hResDLL);
		pTbButton[idxParent].dwData = (DWORD)m_pLastButtonPick;
	}

	pTbButton[idxParent].idCommand = ICAD_TBID_START;
	pTbButton[idxParent].fsState = TBSTATE_ENABLED;
	if((lflgVisibility & m_pLastButtonPick->m_lflgVisibility) != lflgVisibility)
		pTbButton[idxParent].fsState |= TBSTATE_HIDDEN;
	pTbButton[idxParent].fsStyle = TBSTYLE_BUTTON;
	pTbButton[idxParent].iString = 0;

	// Fill the array of button structures.
	int	ct= bReverseImages ? nButtons - 2 : 1;		/*D.Gavrilov*/// for true reversing

	if(IsNested())
		pibCur = pibParent->m_pibCurFlyOut;
	else
		pibCur = pibParent->m_pibFlyOut;

	for( ; pibCur; pibCur = pibCur->m_pibFlyOut)
	{
		if(pibCur == m_pLastButtonPick)	/*D.Gavrilov*/// don't make this extra place
			continue;

		if(pibCur->m_bChgParentBtnImg && pibCur->m_pibCurFlyOut)
			pTbButton[ct].iBitmap = m_pTbMain->AddIcadBitmap(pibCur->m_pibCurFlyOut, 1, NULL, NULL, pszAltPath, hResDLL);
		else
			pTbButton[ct].iBitmap = m_pTbMain->AddIcadBitmap(pibCur, 0, NULL, NULL, pszAltPath, hResDLL);
		
		pTbButton[ct].dwData = (DWORD)pibCur;

		pTbButton[ct].idCommand = ct + ICAD_TBID_START;
		if(bReverseImages)	/*D.Gavrilov*/// for true reversing
			++pTbButton[ct].idCommand;

		pTbButton[ct].fsState = TBSTATE_ENABLED;

		if((lflgVisibility & pibCur->m_lflgVisibility) != lflgVisibility)
			pTbButton[ct].fsState |= TBSTATE_HIDDEN;
		pTbButton[ct].fsStyle = TBSTYLE_BUTTON;
		pTbButton[ct].iString = 0;

		if(bReverseImages)
			--ct;
		else
			++ct;
	}

	CBitmap*	pBmp = m_pTbMain->CreateToolBarBitmap();
	if(pBmp)
	{
		SetBitmap((HBITMAP)pBmp->Detach());
		delete pBmp;
	}

	AddButtons(nButtons, pTbButton);

	HeapFree(procHeap,0,pTbButton);
}

int
CIcadFlyOut::SetBitmap
(
 HBITMAP	hbmImageWell
)
{
	if(!hbmImageWell)
		return -1;

	// From here down, from MFC source BARTOOL.CPP
	// need complete bitmap size to determine number of images
	BITMAP	bitmap;
	VERIFY(::GetObject(hbmImageWell, sizeof(BITMAP), &bitmap));

	// Set the sizes for the buttons for this toolbar.
	CSize	sizeImage(ICAD_SM_IMGX, ICAD_SM_IMGY);

	if(m_pTbMain->m_bLargeButtons)
	{
		sizeImage.cx = ICAD_LG_IMGX;
		sizeImage.cy = ICAD_LG_IMGY;
	}

	// add the bitmap to the common control toolbar
	BOOL	bResult;
	if(!m_hbmImageWell)
	{
		TBADDBITMAP	addBitmap;
		addBitmap.hInst = NULL;	// makes TBADDBITMAP::nID behave a HBITMAP
		addBitmap.nID = (UINT)hbmImageWell;
		bResult =  (BOOL)DefWindowProc(TB_ADDBITMAP, bitmap.bmWidth / sizeImage.cx, (LPARAM)&addBitmap);
	}
	else
	{
		TBREPLACEBITMAP	replaceBitmap;
		replaceBitmap.hInstOld = NULL;
		replaceBitmap.nIDOld = (UINT)m_hbmImageWell;
		replaceBitmap.hInstNew = NULL;
		replaceBitmap.nIDNew = (UINT)hbmImageWell;
		replaceBitmap.nButtons = bitmap.bmWidth / sizeImage.cx;
		bResult = (BOOL)DefWindowProc(TB_REPLACEBITMAP, 0, (LPARAM)&replaceBitmap);
	}

	// remove old bitmap, if present
	if(bResult)
	{
		if(m_hbmImageWell)
			::DeleteObject(m_hbmImageWell);
		m_hbmImageWell = hbmImageWell;
	}

	return bResult;
}

void
CIcadFlyOut::OnLButtonDown
(
 UINT	nFlags,
 CPoint	point
)
{
	if(!m_pTbMain->m_bCustomize)
	{
		CToolBarCtrl::OnLButtonDown(nFlags, point);
		return;
	}

	// Get the button located at the point the left button was pressed at.
	if( (m_idxCurDragButton = m_pTbMain->GetButtonFromPt(this, point, &m_CurDragButton)) != -1 && 
		!(m_CurDragButton.fsStyle & TBSTYLE_SEP) )
	{
		m_pTbMain->OnCustButtonDown(NULL, this, NULL, m_idxCurDragButton);
		PressButton(m_CurDragButton.idCommand);

		// Display the information for this button in the customize dialog.
		m_pTbMain->DisplayCustData((CIcadTBbutton*)m_CurDragButton.dwData);
		// Load the customize cursors
		if(!m_hcurToolBar)
		{
			m_hcurToolBar=AfxGetApp()->LoadCursor(IDC_CUSTTOOLBAR);
			m_hcurToolBarDel=AfxGetApp()->LoadCursor(IDC_CUSTTOOLBARDEL);
			m_hcurFlyOutH=AfxGetApp()->LoadCursor(IDC_CUSTFLYOUTH);
			m_hcurFlyOutHR=AfxGetApp()->LoadCursor(IDC_CUSTFLYOUTHR);
			m_hcurFlyOutV=AfxGetApp()->LoadCursor(IDC_CUSTFLYOUTV);
			m_hcurFlyOutVR=AfxGetApp()->LoadCursor(IDC_CUSTFLYOUTVR);
		}

		m_prevCursor=(::SetCursor(m_hcurToolBar));
		// Make sure this window has capture.  This fixed a bug that would not allow
		// the user to drag a button outside of the client area once the mouse had
		// gone outside the client area without the mouse button pushed down.
		SetCapture();
	}
}

void
CIcadFlyOut::OnMouseMove
(
 UINT	nFlags,
 CPoint	point
)
{
	// Is the customize dialog is displayed?
	if(m_pTbMain->m_bCustomize)
	{
		CRect	rect;
		GetClientRect(rect);
		if(!(nFlags & MK_LBUTTON))
		{
			// If the left mouse button is NOT down, the point is outside the client
			// area, and there is not a button pressed down in this flyout; destroy
			// the flyout.
			if(!rect.PtInRect(point))
			{
				m_pTbMain->DrawInsertMark(NULL, NULL, point);	// Erase the insert mark.
				ReleaseCapture();
				return;
			}
		}
		else
		{
			// If the left mouse button IS down we must be dragging a button.
			if(m_pTbLostCapture)
			{
				// We are dragging a button from the customize dialog.
				// If the point is NOT inside the client area of the flyout destroy the
				// flyout and return.
				if(!rect.PtInRect(point))
				{
					// I need to setup this local variable because ReleaseCapture() calls
					// OnReleaseCapture() which deletes the "this" pointer, making
					// m_pTbLostCapture invalid.
					CWnd*	pTbLostCapture = m_pTbLostCapture; 
					m_pTbMain->DrawInsertMark(NULL, NULL, point);	// Erase the insert mark.
					ReleaseCapture();
					pTbLostCapture->SetCapture();
				}
				else
					m_pTbMain->DrawInsertMark(NULL, this, point);

				return;
			}

			// We are dragging a button from the flyout.
			// Return if the point is inside the client area of the flyout.
			if(rect.PtInRect(point))
			{
				// Make sure this cursor is set here because it may have previously been
				// set to a drag-flyout cursor.
				::SetCursor(m_hcurToolBar);
				m_pTbMain->DrawInsertMark(NULL, this, point);
				return;
			}

			// Get the client area of the owner toolbar
			CRect	rectTbOwner;
//			m_pTbOwner->GetWindowRect(rectTbOwner);	/*D.Gavrilov*///we must obtain CLIENT
													//area of the toolbar
			m_pTbOwner->GetClientRect(rectTbOwner);
			m_pTbOwner->ClientToScreen(rectTbOwner);
			ScreenToClient(rectTbOwner);

			// Get the index of the first visible button in the flyout.
			int	idxFirstButton;
			if(m_bImagesReversed)
				idxFirstButton = m_pTbMain->GetLastVisibleButtonIndex(this);
			else
				idxFirstButton = m_pTbMain->GetFirstVisibleButtonIndex(this);

			// Return if the point is not inside the client area of the owner
			// toolbar or if we are not dragging the first button in the flyout.
			if(!rectTbOwner.PtInRect(point) || m_idxCurDragButton != idxFirstButton)
			{
				::SetCursor(m_hcurToolBarDel);
				m_pTbMain->DrawInsertMark(NULL, NULL, point);
				return;
			}

			// Change the cursor to a drag flyout cursor if we are dragging the first
			// flyout button in the client area of the owner toolbar.
			if(rect.Width()>rect.Height())	// Horizontal flyout
			{
				if(m_bImagesReversed)
					::SetCursor(m_hcurFlyOutHR);
				else
					::SetCursor(m_hcurFlyOutH);
			}
			else	// Vertical flyout.
			{
				if(m_bImagesReversed)
					::SetCursor(m_hcurFlyOutVR);
				else
					::SetCursor(m_hcurFlyOutV);
			}

			CIcadToolBar*	pTbOwner = (CIcadToolBar*)m_pTbOwner;
			CPoint			ptTbOwner(point);
			ClientToScreen(&ptTbOwner);
			pTbOwner->ScreenToClient(&ptTbOwner);
			m_pTbMain->DrawInsertMark(pTbOwner, NULL, ptTbOwner);

			return;
		}
	}

	TBBUTTON	tbButton;
	int			idxButton = m_pTbMain->GetButtonFromPt(this, point, &tbButton);
	if(idxButton == -1)
	{
		if(m_idPrevButton != -1)
		{
			if(!m_pTbMain->m_bCustomize)
				PressButton(m_idPrevButton, FALSE);
			m_pTbMain->SetCurToolTipPtr(NULL);
			m_idPrevButton = -1;
		}

		if(IsNested())
		{
			ReleaseCapture();
			((CIcadToolBar*)m_pTbOwner)->m_pFlyOut->SetCapture();
		}

		if(m_pNestedFlyOut)
		{
			//convert point to screen coords,
			ClientToScreen(&point);
			//convert it to client coords of the nested flyout
			m_pNestedFlyOut->ScreenToClient(&point);

			//now check if the point is in the nested flyout, if so, switch Captures
			idxButton = m_pTbMain->GetButtonFromPt(m_pNestedFlyOut, point, &tbButton);
			if(idxButton != -1)
			{
				ReleaseCapture();
				m_pNestedFlyOut->SetCapture();
			}
		}
	}
	else if(m_idPrevButton != tbButton.idCommand)
	{
		if(!m_pTbMain->m_bCustomize)
		{
			if(m_idPrevButton != -1)
				PressButton(m_idPrevButton, FALSE);

			if((tbButton.fsState & TBSTATE_ENABLED))
				PressButton(tbButton.idCommand);

			m_idPrevButton = tbButton.idCommand;
		}

		CIcadTBbutton*	picButton = (CIcadTBbutton*)tbButton.dwData;
		m_pTbMain->SetCurToolTipPtr(picButton);
		CWnd*			pWnd = m_pTbMain->GetHelpStringOutputWnd();
		if(pWnd)
			pWnd->SetWindowText(picButton->m_helpString);

		if(m_pNestedFlyOut)
			m_pNestedFlyOut->DestroyFlyOut();

		//loadflyout
		if(picButton->m_pibCurFlyOut)
		{
			CRect	rectButton;
			GetItemRect(idxButton, &rectButton);

			// Make sure more than one button is visible for this flyout.
			CIcadTBbutton*	picCur;
			int				nButtons;
			long			lflgVisibility = m_pTbMain->m_lflgVisibility;

			for(picCur = picButton->m_pibCurFlyOut, nButtons = 0; picCur; picCur = picCur->m_pibFlyOut)
			{
				if((lflgVisibility & picCur->m_lflgVisibility) == lflgVisibility)
					++nButtons;
			}

			if(nButtons <= 1)
				return;

			// Figure out the size of the flyout.
			CRect	rectFlyOut(rectButton), rectToolBar, rectDeskTop;

			GetWindowRect(rectToolBar);
			SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&rectDeskTop, 0);
			ScreenToClient(rectDeskTop);
			BOOL	bReverseImages = FALSE;
			int		nBtnSize;

			if(rectToolBar.Height() < rectToolBar.Width())	// Create a vertical flyout
			{
				if(m_pTbMain->m_bLargeButtons)
					nBtnSize = ICAD_LG_BUTTONY;
				else
					nBtnSize = ICAD_SM_BUTTONY;

				if(nBtnSize * nButtons >= rectDeskTop.bottom)
				{
					rectFlyOut.InflateRect(0, nBtnSize * (nButtons - 1), 0, 0);	/*D.Gavrilov*///" - 1" for removing extra place
					bReverseImages = TRUE;
				}
				else
				{
					rectFlyOut.InflateRect(0, 0, 0, nBtnSize * (nButtons - 1));	/*D.Gavrilov*///" - 1" for removing extra place
					bReverseImages = FALSE;
				}
			}
			else	// Create a horizontal flyout.
			{
				if(m_pTbMain->m_bLargeButtons)
					nBtnSize = ICAD_LG_BUTTONX;
				else
					nBtnSize = ICAD_SM_BUTTONX;

				if(nBtnSize * nButtons >= rectDeskTop.right)
				{
					rectFlyOut.InflateRect(nBtnSize * (nButtons - 1), 0, 0, 0);	/*D.Gavrilov*///" - 1" for removing extra place
					bReverseImages = TRUE;
				}
				else
				{
					rectFlyOut.InflateRect(0, 0, nBtnSize * (nButtons - 1), 0);	/*D.Gavrilov*///" - 1" for removing extra place
					bReverseImages = FALSE;
				}
			}

			// Create the flyout.
			ClientToScreen(rectFlyOut);
			m_pNestedFlyOut = new CIcadFlyOut();
			m_pNestedFlyOut->m_idxParentButton = m_idxParentButton ;
			m_pNestedFlyOut->m_pTbOwner = this->m_pTbOwner;
			m_pNestedFlyOut->m_pTbMain = m_pTbMain;
			m_pNestedFlyOut->m_strMnuFileName = m_strMnuFileName;
			m_pNestedFlyOut->Create(rectFlyOut, picButton, bReverseImages);
		}
	}

	if(m_pTbMain->m_bCustomize && (GetStyle() & TBSTYLE_FLAT))
		CToolBarCtrl::OnMouseMove(nFlags, point);
}

void
CIcadFlyOut::OnLButtonUp
(
 UINT	nFlags,
 CPoint	point
)
{
	CToolBarCtrl*	pTbCtrlOwner = &(((CToolBar*)m_pTbOwner)->GetToolBarCtrl());
	if(m_pTbMain->m_bCustomize)
	{
		if(!m_pTbLostCapture)
		{
			// If we are dragging a button from the flyout off of the flyout...
			// Get the client area of the owner toolbar
			CRect	rectTbOwner, rectFlyOut;
//			m_pTbOwner->GetWindowRect(rectTbOwner);	/*D.Gavrilov*///we must obtain CLIENT
													//area of the toolbar (as it MoveDraggedButton does)
			m_pTbOwner->GetClientRect(rectTbOwner);
			m_pTbOwner->ClientToScreen(rectTbOwner);

			ScreenToClient(rectTbOwner);
			GetClientRect(rectFlyOut);

			// Get the index of the first visible button in the flyout.
			int	idxFirstButton;
			if(m_bImagesReversed)
				idxFirstButton = m_pTbMain->GetLastVisibleButtonIndex(this);
			else
				idxFirstButton = m_pTbMain->GetFirstVisibleButtonIndex(this);

			// If the point is not inside the client area of the flyout but IS inside
			// the client area of the owner toolbar and we are dragging the first 
			// button in the flyout then move the parent button in the owner toolbar.
			if(!rectFlyOut.PtInRect(point) && rectTbOwner.PtInRect(point) && m_idxCurDragButton == idxFirstButton)
			{
				// Convert the drop point from client coords of the flyout to client
				// coords of the owner toolbar.
				CPoint	ptOwner(point);
				ClientToScreen(&ptOwner);
				m_pTbOwner->ScreenToClient(&ptOwner);

				// Get the button info before moving the button.
				TBBUTTON	tbButton;
				pTbCtrlOwner->GetButton(m_idxParentButton, &tbButton);

				// Move the button in the owner toolbar.
				int	idxMovedButton = m_pTbMain->MoveDraggedButton(((CIcadToolBar*)m_pTbOwner), NULL, ptOwner, m_idxParentButton);

				// Set the current button down variables.
				m_pTbMain->m_idxCurButtonDown = idxMovedButton;
				m_pTbMain->m_pToolBarCurBtnDown = ((CIcadToolBar*)m_pTbOwner);
				m_pTbMain->m_pFlyOutCurBtnDown = NULL;
				((CToolBar*)m_pTbOwner)->GetParentFrame()->RecalcLayout();
				m_pTbMain->DrawInsertMark(NULL, NULL, point);
				ReleaseCapture();
				return;
			}
			else
			{
				int	idxMovedButton = m_pTbMain->MoveDraggedButton(NULL, this, point, m_idxCurDragButton);
				if(idxMovedButton == -1)
				{
					// We are deleting the flyout button.
					// Set the customize variables in main to invalid.
					m_pTbMain->m_idxCurButtonDown = -1;
					m_pTbMain->m_pToolBarCurBtnDown = NULL;
					m_pTbMain->m_pFlyOutCurBtnDown = NULL;

					// Clear and disable the editboxes in the customize toolbar dialog.
					m_pTbMain->m_pCusTbDlg->ResetEditboxes();

					// Update the order of the linked list in the parent button.
					OnButtonOrderChanged();
					UpdateSize();
				}
				else
				{
					// Update the customize variables in main.
					m_pTbMain->m_idxCurButtonDown = idxMovedButton;
					m_pTbMain->m_pToolBarCurBtnDown = NULL;
					m_pTbMain->m_pFlyOutCurBtnDown = this;
					// Update the order of the linked list in the parent button.
					OnButtonOrderChanged();
				}
			}

			if(m_prevCursor)
				::SetCursor(m_prevCursor);
			m_prevCursor = NULL;
			m_hcurToolBar = NULL;
		}
		else
		{
			// If the customize dialog is up and m_pTbLostCapture!=NULL then 
			// m_pTbLostCapture points to the CIcadToolBarCtrl class that the
			// button was dragged from.  Set the cursor back to what is was
			// before OnLButtonDown in CIcadToolBarCtrl.

			::SetCursor(((CIcadToolBarCtrl*)m_pTbLostCapture)->m_prevCursor);
			((CIcadToolBarCtrl*)m_pTbLostCapture)->m_prevCursor = NULL;

			CPoint	ptScrn(point);
			ClientToScreen(&ptScrn);

			int	idxNewButton = m_pTbMain->AddDraggedButton((CIcadToolBarCtrl*)m_pTbLostCapture, NULL, this, ptScrn);

			// Add the new flyout bitmap to the owner toolbar.
			TBBUTTON		tbButtonNew;
			GetButton(idxNewButton, &tbButtonNew);
			CIcadTBbutton*	pibNew = (CIcadTBbutton*)tbButtonNew.dwData;
			pibNew->m_idxFlyOutImage = m_pTbMain->AddIcadBitmap(pibNew, 1, (CIcadToolBar*)m_pTbOwner);

			// Set the ID of the current pressed button.
			m_pTbMain->m_idxCurButtonDown = idxNewButton;
			m_pTbMain->m_pToolBarCurBtnDown = NULL;
			m_pTbMain->m_pFlyOutCurBtnDown = this;

			// Enable the Advanced... button.
			CButton*	pAdvButton = (CButton*)m_pTbMain->m_pCusTbDlg->GetDlgItem(CUS_TOOLBAR_BTNADV);
			pAdvButton->EnableWindow();
			// Enable the editboxes
			CEdit	*pEditToolTip = (CEdit*)m_pTbMain->m_pCusTbDlg->GetDlgItem(CUS_TOOLBAR_EDITTOOLTIP),
					*pEditHelpStr = (CEdit*)m_pTbMain->m_pCusTbDlg->GetDlgItem(CUS_TOOLBAR_EDITHELPSTR),
					*pEditCommand = (CEdit*)m_pTbMain->m_pCusTbDlg->GetDlgItem(CUS_TOOLBAR_EDITCOMMAND);

			if(!pEditToolTip->IsWindowEnabled())
			{
				pEditToolTip->EnableWindow();
				pEditHelpStr->EnableWindow();
				pEditCommand->EnableWindow();
			}

			// Update the linked list of the parent button.
			OnButtonOrderChanged();
			UpdateSize();
			m_pTbLostCapture = NULL;
		}

		m_pTbMain->DrawInsertMark(NULL, NULL, point);

		return;
	}

	TBBUTTON		tbButton;
	int				idxButton = m_pTbMain->GetButtonFromPt(this, point, &tbButton);
	CIcadTBbutton*	picButton = (CIcadTBbutton*)tbButton.dwData;

	if(idxButton != -1)
	{
		if(!(tbButton.fsState & TBSTATE_ENABLED))
		{
			ReleaseCapture();
			return;
		}

		if(picButton->m_bChgParentBtnImg && picButton->m_pibCurFlyOut)
			SDS_SendMessage(WM_MENUSELECT, 0, (LPARAM)picButton->m_pibCurFlyOut->m_command.GetBuffer(0));
		else
			SDS_SendMessage(WM_MENUSELECT, 0, (LPARAM)picButton->m_command.GetBuffer(0));
	}

	// Set the correct image in the owner toolbars button (the parent button)
	TBBUTTON		tbButtonParent;
	pTbCtrlOwner->GetButton(m_idxParentButton, &tbButtonParent);

	CIcadTBbutton*	pibParent = (CIcadTBbutton*)tbButtonParent.dwData;

	if(idxButton == -1 || pibParent->m_pibCurFlyOut == picButton || !pibParent->m_bChgParentBtnImg)
		pTbCtrlOwner->PressButton(tbButtonParent.idCommand, FALSE);
	else
	{
		CIcadToolBar*	pTbOwner = (CIcadToolBar*)m_pTbOwner;
		int				idxParentButton = m_idxParentButton;

		pibParent->m_pibCurFlyOut = picButton;
		m_pLastButtonPick = picButton;

		// The call to DeleteButton() seems to delete "this" object (IE 4.0), which
		// is why the pTbOwner and idxParentButton variables are redefined.
		pTbCtrlOwner->DeleteButton(m_idxParentButton);
		tbButton.iBitmap = picButton->m_idxFlyOutImage;
		tbButton.idCommand = tbButtonParent.idCommand;
		tbButton.fsState = TBSTATE_ENABLED;
		tbButton.dwData = tbButtonParent.dwData;
		pTbCtrlOwner->InsertButton(idxParentButton, &tbButton);

		// Redraw the toolbar
		pTbOwner->UpdateLayout();
	}

	ReleaseCapture();

	if(IsNested() && idxButton != -1)
		//if I have a parent flyout, and one of my buttons was pressed, kill my parent.
		//killing my parent will take care of killing me too.
		((CIcadToolBar*)m_pTbOwner)->m_pFlyOut->DestroyFlyOut();
	else
		//just kill myself
		DestroyFlyOut();
}

void
CIcadFlyOut::OnButtonOrderChanged()
{
// This function gets called to update the order of the linked list in the parent
// button when a button gets added, moved, or deleted from the flyout.

	TBBUTTON		tbButtonParent, tbButtonFlyOut;
	CToolBarCtrl*	pTbCtrlOwner = &(((CToolBar*)m_pTbOwner)->GetToolBarCtrl());

	pTbCtrlOwner->GetButton(m_idxParentButton, &tbButtonParent);
	CIcadTBbutton	*pibParent, *pibCur;

	pibParent = (CIcadTBbutton*)tbButtonParent.dwData;
	pibCur = pibParent;
	int		nButtons = GetButtonCount(),
			nVisibleBtns = m_pTbMain->GetVisibleButtonCount(this),
			ct;

	for(ct = 0; ct < nButtons && pibCur; ++ct)
	{
		if(!m_bImagesReversed)
			GetButton(ct, &tbButtonFlyOut);
		else
			GetButton( nButtons - 1 - ct, &tbButtonFlyOut);

		pibCur->m_pibFlyOut = (CIcadTBbutton*)tbButtonFlyOut.dwData;

		if(pibCur == pibParent && (pibCur->m_pibCurFlyOut != pibCur->m_pibFlyOut ||	nVisibleBtns <= 1))
		{
			pibCur->m_pibCurFlyOut = pibCur->m_pibFlyOut;
			// Change the image of the parent button;
			pTbCtrlOwner->DeleteButton(m_idxParentButton);

			if(nVisibleBtns < 2)
			{
				// If all the visible buttons have been deleted from the flyout, change the
				// image in the parent button to reflect that it is no longer a flyout.
				if(pibParent->m_idxFlyOutImage == -1)
				{
					tbButtonParent.iBitmap = m_pTbMain->AddIcadBitmap(pibParent->m_pibCurFlyOut, 0, (CIcadToolBar*)m_pTbOwner);
					tbButtonParent.dwData = (DWORD)pibParent->m_pibCurFlyOut;
					pibCur = pibParent->m_pibCurFlyOut;
					pibParent->m_pibFlyOut = NULL;

					m_pTbMain->FreeIcadButtonData(pibParent);

					pibParent = pibCur;
					pibParent->m_pibFlyOut = NULL;
					pibParent->m_pibCurFlyOut = NULL;
				}
				else
					tbButtonParent.iBitmap = pibParent->m_idxFlyOutImage;
			}
			else
				tbButtonParent.iBitmap = pibParent->m_pibCurFlyOut->m_idxFlyOutImage;

			pTbCtrlOwner->InsertButton(m_idxParentButton, &tbButtonParent);
		}
		pibCur = pibCur->m_pibFlyOut;

	}	// for(

	if(pibCur)
		pibCur->m_pibFlyOut = NULL;
}

void
CIcadFlyOut::AddToolTips()
{
	CRect	rect;
	int		ct;

	if(m_pToolTipCtrl)
		delete m_pToolTipCtrl;
	m_pToolTipCtrl = new CToolTipCtrl();
	m_pToolTipCtrl->Create(this);

	int			nButtons = GetButtonCount();
	TBBUTTON	tbButton;

	for(ct = 0; ct < nButtons; ++ct)
	{
		GetButton(ct, &tbButton);
		if(tbButton.fsState & TBSTATE_HIDDEN)
			continue;

		GetItemRect(ct, rect);
		m_pToolTipCtrl->AddTool(this, LPSTR_TEXTCALLBACK, rect, tbButton.idCommand);
	}

	return;
}

void
CIcadFlyOut::UpdateSize()
{
	// Resize the Parent popup window
	CRect	rectPopup;
	m_pPopup->GetWindowRect(rectPopup);

	int	nButtons = m_pTbMain->GetVisibleButtonCount(this),
		nBtnSize;

	if(rectPopup.Height() < rectPopup.Width())	// Horizontal flyout
	{
		if(m_pTbMain->m_bLargeButtons)
			nBtnSize = ICAD_LG_BUTTONX;
		else
			nBtnSize = ICAD_SM_BUTTONX;

		if(m_bImagesReversed)
		{
			rectPopup.left = rectPopup.right - nBtnSize * nButtons;
#ifndef ICAD_OLDTOOLBARS
				rectPopup.left += 2;
#endif
		}
		else
		{
			rectPopup.right = rectPopup.left + nBtnSize * nButtons;
#ifndef ICAD_OLDTOOLBARS
				rectPopup.right += 2;
#endif
		}
	}
	else	// Vertical flyout.
	{
		if(m_pTbMain->m_bLargeButtons)
			nBtnSize = ICAD_LG_BUTTONY;
		else
			nBtnSize = ICAD_SM_BUTTONY;

		if(m_bImagesReversed)
		{
			rectPopup.top = rectPopup.bottom - nBtnSize * nButtons;
#ifndef ICAD_OLDTOOLBARS
				rectPopup.top += 2;
#endif
		}
		else
		{
			rectPopup.bottom = rectPopup.top + nBtnSize * nButtons;
#ifndef ICAD_OLDTOOLBARS
				rectPopup.bottom += 2;
#endif
		}
	}

	m_pPopup->MoveWindow(rectPopup);

	// Resize the toolbar.
	CRect	rectToolBar;
	m_pPopup->GetClientRect(rectToolBar);

#ifdef ICAD_OLDTOOLBARS
	rectToolBar.top -= 2;
#else
	rectToolBar.DeflateRect(1, 1);
#endif

	MoveWindow(rectToolBar);
}

void
CIcadFlyOut::ReleaseCapture()
{
	::ReleaseCapture();
	if(m_bWin95)
		return;
	// Windows NT 3.5 doesn't call the OnCaptureChanged function.
	DestroyFlyOut();
}

void
CIcadFlyOut::OnCaptureChanged
(
 CWnd*	pWnd
)
{
	if(pWnd == this)	/*D.Gavrilov*///don't take mouse from the flyout itself
		return;

	//if we are customizing, CaptureChange messages must destroy the flyout, because that is the only
	//indication that the mouse has moved on to the next button/flyout.
	if(m_pTbMain->m_bCustomize)
		DestroyFlyOut();
	else
		CToolBarCtrl::OnCaptureChanged(pWnd);
}

void
CIcadFlyOut::DestroyFlyOut()
{
	// If there is a button pressed down in this flyout don't destroy it because it
	// the user may want to customize it.  The flyout gets destroyed in
	// OnCustButtonDown() of the next ToolBarCtrl to press a button in.
	if(m_pTbMain->m_pFlyOutCurBtnDown == this)
		return;

	//if there is a nested flyout, delete it.
	if(m_pNestedFlyOut)
	{
		m_pNestedFlyOut->DestroyFlyOut();
		m_pNestedFlyOut = NULL;
	}

	// Frame windows get destroyed not deleted.
	if(m_pPopup)
		m_pPopup->DestroyWindow();

	// set the owner's pointer to NULL.
	if(IsNested())
		((CIcadToolBar*)m_pTbOwner)->m_pFlyOut->m_pNestedFlyOut = NULL;
	else
		((CIcadToolBar*)m_pTbOwner)->m_pFlyOut=NULL;

	delete this;
}

BOOL
CIcadFlyOut::OnToolTipText
(
 UINT		nID,
 NMHDR*		pNMHDR,
 LRESULT*	pResult
)
{
	return m_pTbMain->OnToolTipText(nID, pNMHDR, pResult);
}

BOOL
CIcadFlyOut::OnToolTipTextW
(
 UINT		nID,
 NMHDR*		pNMHDR,
 LRESULT*	pResult
)
{
	// This function is for Windows NT UNICODE tooltips.
	return m_pTbMain->OnToolTipTextW(nID, pNMHDR, pResult);
}

BOOL
CIcadFlyOut::IsNested()
{
	if(((CIcadToolBar*)m_pTbOwner)->m_pFlyOut && ((CIcadToolBar*)m_pTbOwner)->m_pFlyOut->m_pNestedFlyOut == this)
		return TRUE;
	else
		return FALSE;
}

/*D.Gavrilov*/// show a context menu if need
void
CIcadFlyOut::OnRButtonDown
(
 UINT	nFlags,
 CPoint	point
)
{
	if(!m_pTbMain->m_bCustomize)
	{
		CToolBarCtrl::OnRButtonDown(nFlags, point);
		return;
	}

	if(SDS_CURDOC && SDS_CURDOC->IsInPlaceActive())
	{
		CToolBarCtrl::OnRButtonDown(nFlags, point);
		return;
	}

	if(SDS_CMainWindow)
	{
		CPoint	ptMain(point);
		ClientToScreen(&ptMain);
		SDS_CMainWindow->ContextMenu(ptMain);
	}

	if(nFlags & MK_LBUTTON)
	{
		m_pTbMain->DrawInsertMark(NULL, NULL, point);	// Erase the insert mark.
		ReleaseCapture();
	}
}
