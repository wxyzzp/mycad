/* File  : <DevDir>\source\prj\icad\IcadPromptWnd.cpp
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 */

#include "Icad.h"/*DNT*/
#include "IcadCmdBar.h"/*DNT*/
#include "IcadPromptWnd.h"/*DNT*/
#include "IcadTextScrn.h"/*DNT*/
//#include "..\lib\gr\truetypeutils.h"/*DNT*/

#include "ItcLanguage.h" // EBATECH

/////////////////////////////////////////////////////////////////////
// CIcadPromptWnd functions
/////////////////////////////////////////////////////////////////////

CIcadPromptWnd::CIcadPromptWnd() : m_pPromptWndList(NULL), m_pPromptWndEdit(NULL)
{
}

CIcadPromptWnd::~CIcadPromptWnd()
{
    delete m_pPromptWndList;
	delete m_pPromptWndEdit;
}

BOOL
CIcadPromptWnd::Create(CWnd* pParentWnd)
{
    CRect	rectEmpty;
    rectEmpty.SetRectEmpty();

    // Create the list control
	m_pPromptWndList = new CIcadPromptWndList();
    m_pPromptWndList->Create(NULL, NULL, WS_CHILD | WS_VSCROLL|WS_BORDER, rectEmpty, pParentWnd, IDC_CMDPROMPTLIST);
	m_pPromptWndList->m_pParentWnd = pParentWnd;
	m_pPromptWndList->m_pPromptWnd = this;
    m_pPromptWndList->ShowWindow(SW_SHOW);
    m_pPromptWndList->UpdateWindow();

	// Create the edit control
	m_pPromptWndEdit = new CIcadPromptWndEdit();
	m_pPromptWndEdit->Create(NULL, NULL, WS_CHILD, rectEmpty, pParentWnd, IDC_CMDPROMPTEDIT);
	m_pPromptWndEdit->m_pParentWnd = pParentWnd;
	m_pPromptWndEdit->ShowWindow(SW_SHOW);
	m_pPromptWndEdit->UpdateWindow();

	return 0;
}

void
CIcadPromptWnd::OnSize(LPCRECT rectNew)
{
    CRect	rectClient(rectNew), rectEdit(rectNew), rectList(rectNew);

    // Set the new coordinates for the list-box
    rectList.bottom = rectClient.bottom - m_pPromptWndList->m_nFontHgt;
	int		nLinesPerPage = (rectList.bottom - rectList.top) / m_pPromptWndList->m_nFontHgt;

	if(nLinesPerPage > m_pPromptWndList->m_nMaxHistLines)
	{
		rectList.top = rectList.bottom - m_pPromptWndList->m_nFontHgt * m_pPromptWndList->m_nMaxHistLines;
		m_pPromptWndList->MoveWindow(rectList);
	}
	m_pPromptWndList->MoveWindow(rectList);

    // Set the new coordinates for the edit control
    rectEdit.top = rectClient.bottom - m_pPromptWndEdit->m_nFontHgt;
    m_pPromptWndEdit->MoveWindow(rectEdit);
}


/////////////////////////////////////////////////////////////////////
// List control functions
/////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CIcadPromptWndList, CWnd)

BEGIN_MESSAGE_MAP(CIcadPromptWndList, CWnd)
	//{{AFX_MSG_MAP(CIcadPromptWndList)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
	ON_WM_SYSCOLORCHANGE()
    ON_WM_VSCROLL()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CIcadPromptWndList::CIcadPromptWndList() :	m_pHistList(NULL), m_nMaxHistLines(256), m_pParentWnd(NULL),
											m_pPromptWnd(NULL), m_bGotRButtonDown(FALSE)
{
	m_pHistViewList = new CStringList();
}

CIcadPromptWndList::~CIcadPromptWndList()
{
	ASSERT(m_pMemDC);
	::DeleteObject(::SelectObject(m_pMemDC->GetSafeHdc(), m_hOldFont));
	::DeleteObject(::SelectObject(m_pMemDC->GetSafeHdc(), m_hOldBitmap));

	delete m_pHistViewList;
	delete m_pMemDC;
	delete m_pbrBkgnd;
}

afx_msg int
CIcadPromptWndList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if(CWnd::OnCreate(lpCreateStruct) == -1)
       return -1;

    // Initialize the height of the list.
    m_listHgt = lpCreateStruct->cy;

	// Set the variable for the maximum number of history lines.
	resbuf	rb;
	sds_getvar("SCRLHIST"/*DNT*/, &rb);
	m_nMaxHistLines = rb.resval.rint;

    // Create the off-screen (memory) device context
    CDC*	pDC = GetDC();
	m_pMemDC = new CDC();
    GetClientRect(m_rectClient);
    m_pMemDC->CreateCompatibleDC(pDC);

	// EBATECH-[2001.3.25 get CharSet,We want to use SHIFTJIS_CHARSET
	int nCharSet = GetTextCharset(pDC->m_hDC);

    // Initialize the font height.
	HFONT	hFont = ::CreateFont(
							// EBATECH-[set in ItcLanguage.h
							//	16,						//int nHeight, 0=Best Guess
								ICAD_PROMPTWND_TEXT_HEIGHT,
							// ]-EBATECH
								0,						//int nWidth, 0=Reasonable Aspect Ratio
								0,						//int nEscapement, 
								0,						//int nOrientation, 
								FW_NORMAL,				//int nWeight,
								0,						//BYTE bItalic, 
								0,						//BYTE bUnderline, 
								0,						//BYTE cStrikeOut, 
								nCharSet,				//BYTE nCharSet,
								OUT_RASTER_PRECIS,		//BYTE nOutPrecision, 
								CLIP_DEFAULT_PRECIS,	//BYTE nClipPrecision, 
								DEFAULT_QUALITY,		//BYTE nQuality, 
								DEFAULT_PITCH,			//BYTE nPitchAndFamily,	/*D.G.*/ Was FIXED_PITCH (we cannot use fixed for hieroglyphical strings)
								ResourceString(IDC_ICADPROMPTWND_COURIER_NE_1, "COURIER NEW")	//LPCTSTR lpszFacename 
								);

	m_hOldFont = (HFONT)::SelectObject(m_pMemDC->GetSafeHdc(), hFont);

    TEXTMETRIC	tm;
    m_pMemDC->GetTextMetrics(&tm);
    m_nFontHgt = (int)tm.tmHeight;
	m_pbrBkgnd = new CBrush(GetSysColor(COLOR_WINDOW));

    m_pMemDC->SetBkMode(OPAQUE);
    m_pMemDC->SetBkColor(GetSysColor(COLOR_WINDOW));
    m_pMemDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));

    // Save the original bitmap object in the DC so we can set it back in the destructor.
	HBITMAP	hBitmap = ::CreateCompatibleBitmap(pDC->GetSafeHdc(), 1, 1);
	m_hOldBitmap = (HBITMAP)::SelectObject(m_pMemDC->GetSafeHdc(), hBitmap);

    ReleaseDC(pDC);

	int	idx, width;
	for(idx = 32; idx < 127; ++idx)
	{
		if(!m_pMemDC->GetCharWidth(idx, idx, &width))
			continue;
		m_pCharWidth[idx - 32] = (char)width;
	}

    return 0;
}

void
CIcadPromptWndList::OnUpdateScrlHist(int nNewMaxHistLines)
{
// Pass 0 to this function to just update the size.

	if(nNewMaxHistLines)
		m_nMaxHistLines = nNewMaxHistLines;

	int	nListCt = m_pHistViewList->GetCount();

	// We need to remove the history lines that are greater than m_nMaxHistLines
	while(nListCt > m_nMaxHistLines)
	{
		m_pHistViewList->RemoveHead();
		--nListCt;
	}

	if(!m_pParentWnd)
		return;

	CRect	rectList;
	m_pParentWnd->GetClientRect(rectList);
	rectList.bottom -= m_pPromptWnd->m_pPromptWndEdit->m_nFontHgt;
	int		nLinesPerPage = rectList.Height() / m_nFontHgt;

	if(nLinesPerPage > m_nMaxHistLines)
	{
		rectList.top = rectList.bottom - m_nFontHgt * m_nMaxHistLines;
		MoveWindow(rectList);
	}
	else
		if(nLinesPerPage > m_nLinesPerPage)
			MoveWindow(rectList);
}

void
CIcadPromptWndList::AddString(CString& str)
{
/*D.G.*/// DBCS enabled.

	if(m_rectClient.IsRectNull() || !(m_rectClient.right - m_rectClient.left))
		return;

    // Add the string to the screen list.
	// This code does the wrapping of the lines if they would go off the screen.
	unsigned char	*pStr = new unsigned char[str.GetLength() + 1],
					*pChar1,*pChar2 = pStr, tmpChar;
	CString			subStr(str);
	int				lines = 0, charWidth, strWidth;
	POSITION		pos = NULL, posBeg = NULL;

	strcpy((char*)pStr, str.GetBuffer(0));

	for( ; *pChar2; ++lines)
	{
		strWidth = 0;
		pChar1 = pChar2;

		for(tmpChar = 0; *pChar2; pChar2 = _mbsinc(pChar2))
		{
			if(*pChar2 > 31 && *pChar2 < 127)
				charWidth = m_pCharWidth[(*pChar2) - 32];
			else
				if(!m_pMemDC->GetCharWidth(_mbsnextc(pChar2), _mbsnextc(pChar2), &charWidth))
					continue;

			strWidth += charWidth;
			if(strWidth <= m_rectClient.right)
				continue;

			pChar2 = _mbsdec(pChar1, pChar2);
			tmpChar = *pChar2;
			*pChar2 = '\0';
			break;
		}

		subStr = pChar1;
		pos = m_pHistViewList->AddTail(subStr);

		if(!lines)
			posBeg = pos;

		if(tmpChar)
			*pChar2 = tmpChar;
	}	// for(

	delete [] pStr;

	if(str.IsEmpty())
	{
		pos = m_pHistViewList->AddTail(ResourceString(IDC_ICADPROMPTWND__2, ""));
		lines = 1;
	}

	::ScrollDC(m_pMemDC->GetSafeHdc(), 0, -m_nFontHgt * lines, m_rectClient, NULL, NULL, NULL);
	
	if(!SDS_CMainWindow->m_bPrintHoldFill)
	{
		CRect	rectUpdate(m_rectClient);
		rectUpdate.top = rectUpdate.bottom - m_nFontHgt * lines;
		m_pMemDC->FillRect(rectUpdate, m_pbrBkgnd);
		
		pos = posBeg;
		for(int ct = 0; pos; ++ct)
			m_pMemDC->TextOut(0, m_listHgt - m_nFontHgt * (lines - ct), m_pHistViewList->GetNext(pos));

		InvalidateRect(m_rectClient);
	}
		
    int	listCt = m_pHistViewList->GetCount();

    // Delete the first string in the list if it is getting too large.
    if(listCt > m_nMaxHistLines)
	{
		m_pHistViewList->RemoveHead();
		--listCt;
    }

    // Don't update the scroll bar unless we know it is visible.
	// If it is not visible and we call SetScrollInfo,
	// I found it sends two OnSize messages. (BAD!)
    if(listCt > m_nLinesPerPage)
	{
        SCROLLINFO	si;
        si.fMask = SIF_RANGE | SIF_POS;
        si.nMin = 0;
        si.nMax = si.nPos = listCt - 1;
        SetScrollInfo(SB_VERT, &si, TRUE);
    }
}

afx_msg void
CIcadPromptWndList::OnSize(UINT nType, int cx, int cy)
{
/*D.G.*/// DBCS enabled.

	CBitmap*	pbmMem;
	BOOL		bDidWidthChange = FALSE;

    m_listHgt = cy;
    m_nLinesPerPage = cy / m_nFontHgt;

	if(m_rectClient.right - m_rectClient.left != cx)
		bDidWidthChange = TRUE;
    GetClientRect(m_rectClient);
    // Create a new bitmap that is the new size of the window
    CDC*	pDC = GetDC();
    pbmMem = new CBitmap();

	if(!cx)
		cx = 1;	// This was added because "delete SelectObject()" would
	if(!cy)
		cy = 1;	// cause an assertion failure when a bitmap created
				// with a width or height of 0 was selected into the DC.

    pbmMem->CreateCompatibleBitmap(pDC, cx, cy);
	CBitmap*	pbmOld = m_pMemDC->SelectObject(pbmMem);
	if(pbmOld)
		pbmOld->DeleteObject();
	pbmMem->Detach();
	delete pbmMem;
    ReleaseDC(pDC);

	if(!m_pHistList)
		return;

	if(bDidWidthChange)
	{
		// If the width changed we need to rebuild the history list to wrap the text at the new width.
		int	charWidth, strWidth;
		charWidth = m_pCharWidth['Z' - 32];
        if(cx < charWidth)
			return;

		m_pHistViewList->RemoveAll();

		POSITION		posMain = m_pHistList->GetHeadPosition();
		CString			str, subStr;
		unsigned char	*pChar1, *pChar2, tmpChar, *pStr;

		while(posMain)
		{
			str = m_pHistList->GetNext(posMain);
			if(str.IsEmpty())
			{
				m_pHistViewList->AddTail(""/*DNT*/);
				continue;
			}

			pStr = new unsigned char[str.GetLength() + 1];
			strcpy((char*)pStr, str.GetBuffer(0));
			pChar2 = pStr;

			while(*pChar2)
			{
				strWidth = 0;
				pChar1 = pChar2;
				for(tmpChar = 0; *pChar2; pChar2 = _mbsinc(pChar2))
				{
					if(*pChar2 > 31 && *pChar2 < 127)
						charWidth = m_pCharWidth[(*pChar2) - 32];
					else
						if(!m_pMemDC->GetCharWidth(_mbsnextc(pChar2), _mbsnextc(pChar2), &charWidth))
							continue;

					strWidth += charWidth;
					if(strWidth <= m_rectClient.right)
						continue;

					pChar2 = _mbsdec(pChar1, pChar2);
					tmpChar = *pChar2;
					*pChar2 = '\0';
					break;
				}

				str = pChar1;
				m_pHistViewList->AddTail(str);
				if(tmpChar)
					*pChar2 = tmpChar;
			}

			delete [] pStr;
		}	// while(posMain)
	}	// if(bDidWidthChange)

    // Fill the background with as much text as possible
    POSITION	pos;
    UINT		ct1;
    int			ctHgt;

    m_pMemDC->FillRect(m_rectClient, m_pbrBkgnd);
    ctHgt = m_listHgt - m_nFontHgt;
	if(!m_pHistViewList)
		return;
	pos = m_pHistViewList->GetTailPosition();

    for(ct1 = 2; pos && ctHgt > 0; ++ct1)
    {
        m_pMemDC->TextOut(0, ctHgt, m_pHistViewList->GetPrev(pos));
        ctHgt = m_listHgt - m_nFontHgt * ct1;
    }

    // Setup the scroll bar
    SCROLLINFO	si;
    si.fMask = SIF_ALL;
    si.nMin = 0;
    si.nMax = si.nPos = m_pHistViewList->GetCount() - 1;
    si.nPage = cy / m_nFontHgt;
    SetScrollInfo(SB_VERT, &si);

    return;
}

afx_msg BOOL
CIcadPromptWndList::OnEraseBkgnd(CDC* pDC)
{
    return 1;
}

afx_msg void
CIcadPromptWndList::OnPaint(void)
{
    CRect		rectUpdate;
    PAINTSTRUCT	ps;
    CDC*		pDC;

    if(GetUpdateRect(rectUpdate))
	{
        pDC = BeginPaint(&ps);
        pDC->BitBlt(rectUpdate.left, rectUpdate.top,
					rectUpdate.Width(), rectUpdate.Height(),
					m_pMemDC, rectUpdate.left, rectUpdate.top, SRCCOPY);
        EndPaint(&ps);
    }
    return;
}

afx_msg void
CIcadPromptWndList::OnSysColorChange()
{	
	delete m_pbrBkgnd;
	m_pbrBkgnd = new CBrush(GetSysColor(COLOR_WINDOW));

    m_pMemDC->SetBkMode(OPAQUE);
    m_pMemDC->SetBkColor(GetSysColor(COLOR_WINDOW));
    m_pMemDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));

	// Call OnSize so the device context gets updated
	CRect	rect;
	GetClientRect(&rect);
	OnSize(0, rect.right - rect.left, rect.bottom - rect.top);
}

afx_msg void
CIcadPromptWndList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    int	nListCount = m_pHistViewList->GetCount(),
		nCurScrollPos = GetScrollPos(SB_VERT);

    if(nSBCode == SB_LINEUP)
	{
        // Return if we are at the top
        if(nCurScrollPos <= 0)
			return;
        // Set the new value of the thumb position.
        --nCurScrollPos;
    }
	else if(nSBCode == SB_LINEDOWN)
	{
        // Return if we are at the bottom
        if(nCurScrollPos >= nListCount - m_nLinesPerPage)
			return;
        // Set the new value of the thumb position.
        ++nCurScrollPos;
    }
	else if(nSBCode == SB_PAGEUP)
	{
        // Set the new value of the thumb position.
        if((nCurScrollPos -= (m_nLinesPerPage-1)) < 0)
			nCurScrollPos = 0;
    }
	else if(nSBCode == SB_PAGEDOWN)
	{
        // Set the new value of the thumb position.
        if((nCurScrollPos += m_nLinesPerPage - 1) >= nListCount - m_nLinesPerPage)
			nCurScrollPos = nListCount - m_nLinesPerPage;
    }
	else if(nSBCode == SB_THUMBPOSITION || nSBCode == SB_THUMBTRACK)
	{
        // Set the new value of the thumb position.
        nCurScrollPos = nPos;
    }
	else
		return;

    // Update the screen
    m_pMemDC->FillRect(m_rectClient, m_pbrBkgnd);
    int			ctHgt = m_listHgt - m_nFontHgt * m_nLinesPerPage,
				ct1 = 0;
    POSITION	posCur = m_pHistViewList->FindIndex(nCurScrollPos);

    while(posCur && ct1 < m_nLinesPerPage)
    {
        m_pMemDC->TextOut(0, ctHgt, m_pHistViewList->GetNext(posCur));
        ctHgt += m_nFontHgt;
        ++ct1;
    }

    // Set the new position of the scroll thumb
    SetScrollPos(SB_VERT, nCurScrollPos, TRUE);
    InvalidateRect(m_rectClient);
}

void
CIcadPromptWndList::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_bGotRButtonDown = TRUE;	
	CWnd::OnRButtonDown(nFlags, point);
}


/////////////////////////////////////////////////////////////////////
// Edit control functions
/////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CIcadPromptWndEdit, CWnd)

BEGIN_MESSAGE_MAP(CIcadPromptWndEdit, CWnd)
	//{{AFX_MSG_MAP(CIcadPromptWndEdit)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CIcadPromptWndEdit::CIcadPromptWndEdit() : m_pCmdStr(NULL), m_bGotRButtonDown(FALSE)
{
}

CIcadPromptWndEdit::~CIcadPromptWndEdit()
{
	ASSERT(m_pMemDC);
	::DeleteObject(::SelectObject(m_pMemDC->GetSafeHdc(), m_hOldFont));
	::DeleteObject(::SelectObject(m_pMemDC->GetSafeHdc(), m_hOldBitmap));

	delete m_pMemDC;
	delete m_pbrBkgnd;
}

afx_msg int
CIcadPromptWndEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if(CWnd::OnCreate(lpCreateStruct) == -1)
       return -1;

	// Create the off-screen (memory) device context
    CDC*	pDC=GetDC();
	m_pMemDC = new CDC();
	m_pMemDC->CreateCompatibleDC(pDC);

	// EBATECH-[2001.3.25 get CharSet,We want to use SHIFTJIS_CHARSET
	int nCharSet = GetTextCharset(pDC->m_hDC);

	HFONT	hFont = ::CreateFont(
							// EBATECH-[set in ItcLanguage.h
							//	16,						//int nHeight, 0=Best Guess
								ICAD_PROMPTWND_TEXT_HEIGHT,
							// ]-EBATECH
								0,						//int nWidth, 0=Reasonable Aspect Ratio
								0,						//int nEscapement, 
								0,						//int nOrientation, 
								FW_BOLD,				//int nWeight,
								0,						//BYTE bItalic, 
								0,						//BYTE bUnderline, 
								0,						//BYTE cStrikeOut, 
								nCharSet,				//BYTE nCharSet,		/*D.G.*/ Was ANSI_CHARSET.
								OUT_RASTER_PRECIS,		//BYTE nOutPrecision, 
								CLIP_DEFAULT_PRECIS,	//BYTE nClipPrecision, 
								DEFAULT_QUALITY,		//BYTE nQuality, 
								DEFAULT_PITCH,			//BYTE nPitchAndFamily,	/*D.G.*/ Was FIXED_PITCH (we cannot use fixed for hieroglyphical strings)
								ResourceString(IDC_ICADPROMPTWND_COURIER_NE_1, "COURIER NEW")	//LPCTSTR lpszFacename
								);
	
	m_hOldFont = (HFONT)::SelectObject(m_pMemDC->GetSafeHdc(), hFont);

    TEXTMETRIC	tm;
    m_pMemDC->GetTextMetrics(&tm);
    m_nFontHgt = (int)tm.tmHeight;
	m_pbrBkgnd = new CBrush(GetSysColor(COLOR_WINDOW));
	m_pMemDC->SetBkMode(TRANSPARENT);
    m_pMemDC->SetBkMode(OPAQUE);
    m_pMemDC->SetBkColor(GetSysColor(COLOR_WINDOW));
    m_pMemDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));

    // Save the original bitmap object in the DC so we can set it back in the destructor.
	HBITMAP	hBitmap = ::CreateCompatibleBitmap(pDC->GetSafeHdc(), 1, 1);
	m_hOldBitmap = (HBITMAP)::SelectObject(m_pMemDC->GetSafeHdc(), hBitmap);

    ReleaseDC(pDC);

	int	idx, width;
	for(idx = 32; idx < 127; ++idx)
	{
		if(!m_pMemDC->GetCharWidth(idx, idx, &width))
			continue;
		m_pCharWidth[idx - 32] = (char)width;
	}

    return 0;
}

afx_msg void
CIcadPromptWndEdit::OnSize(UINT nType, int cx, int cy)
{
	CBitmap*	pbmMem;
	GetClientRect(m_rectClient);

    // Create a new bitmap that is the new size of the window
    CDC*	pDC = GetDC();
    pbmMem = new CBitmap();

	if(!cx)
		cx = 1;	// This was added because "delete SelectObject()" would
	if(!cy)
		cy = 1;	// cause an assertion failure when a bitmap created
				// with a width or height of 0 was selected into the DC.

    pbmMem->CreateCompatibleBitmap(pDC, cx, cy);
	CBitmap*	pbmOld = m_pMemDC->SelectObject(pbmMem);
	if(pbmOld)
		pbmOld->DeleteObject();
	pbmMem->Detach();
	delete pbmMem;
    ReleaseDC(pDC);

    // Redraw the text string.
	if(!m_pCmdStr)
		return;
	m_strPrev.Empty();
	UpdateText(*m_pCmdStr);
}

afx_msg BOOL
CIcadPromptWndEdit::OnEraseBkgnd(CDC* pDC)
{
    return 1;
}

afx_msg void
CIcadPromptWndEdit::OnPaint()
{
	CRect		rectUpdate;
	PAINTSTRUCT	ps;
	CDC*		pDC;

	if(GetUpdateRect(rectUpdate))
	{
		pDC = BeginPaint(&ps);
		pDC->BitBlt(rectUpdate.left, rectUpdate.top,
					rectUpdate.Width(), rectUpdate.Height(),
					m_pMemDC, rectUpdate.left, rectUpdate.top, SRCCOPY);
		EndPaint(&ps);
	}

	return;
}

afx_msg void
CIcadPromptWndEdit::OnSysColorChange()
{	
	delete m_pbrBkgnd;
	m_pbrBkgnd = new CBrush(GetSysColor(COLOR_WINDOW));
    m_pMemDC->SetBkMode(OPAQUE);
    m_pMemDC->SetBkColor(GetSysColor(COLOR_WINDOW));
    m_pMemDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));

	// Call OnSize so the device context gets updated
	CRect	rect;
	GetClientRect(&rect);
	OnSize(0, rect.right - rect.left, rect.bottom - rect.top);
}

void
CIcadPromptWndEdit::UpdateText(CString& str)
{
/*D.G.*/// DBCS enabled.

	if(m_strPrev == str || m_rectClient.right - m_rectClient.left <= 0)
		return;

	CString			subStr(str);
	CStringList*	pTextList = new CStringList();
	int				strWidth, charWidth;
	unsigned char	*pStr = new unsigned char[str.GetLength() + 1],
					*pChar1, *pChar2 = pStr, tmpChar;

	strcpy((char*)pStr, str.GetBuffer(0));

	if(!*pStr)
		pTextList->AddTail(""/*DNT*/);

	while(*pChar2)
	{
		strWidth = 0;
		pChar1 = pChar2;
		for(tmpChar = 0; *pChar2; pChar2 = _mbsinc(pChar2))
		{
			if(*pChar2 > 31 && *pChar2 < 127)
				charWidth = m_pCharWidth[*pChar2 - 32];
			else
				if(!m_pMemDC->GetCharWidth(_mbsnextc(pChar2), _mbsnextc(pChar2), &charWidth))
					continue;

			strWidth += charWidth;
			if(strWidth <= m_rectClient.right)
				continue;

			pChar2 = _mbsdec(pChar1, pChar2);
			tmpChar = *pChar2;
			*pChar2 = '\0';
			break;
		}

		subStr = pChar1;
		pTextList->AddTail(subStr);

		if(tmpChar)
			*pChar2 = tmpChar;
	}	// while(*pChar2)

	delete [] pStr;

	int		nLines = pTextList->GetCount();
	if(nLines != ((m_rectClient.bottom - m_rectClient.top) / m_nFontHgt))
	{
		// Resize the window to fit the number of lines of text.
		CRect	rectEdit;
		GetWindowRect(rectEdit);
		m_pParentWnd->ScreenToClient(rectEdit);
		rectEdit.top = rectEdit.bottom - nLines * m_nFontHgt;
		MoveWindow(rectEdit);

		// Move the prompt list window also.
		CIcadPromptWndList*	pPromptWndList;
		if(m_pParentWnd->IsKindOf(RUNTIME_CLASS(CIcadCmdBarBorderWnd)))
			 pPromptWndList = ((CIcadCmdBarBorderWnd*)m_pParentWnd)->m_pPromptWnd->m_pPromptWndList;
		else
			 pPromptWndList = ((CTextScreen*)m_pParentWnd)->m_pPromptWnd->m_pPromptWndList;

		CRect	rectList;
		pPromptWndList->GetWindowRect(rectList);
		m_pParentWnd->ScreenToClient(rectList);
		rectList.bottom = rectEdit.top;
		pPromptWndList->MoveWindow(rectList);
	}

	// Draw the text
	//if(!str.IsEmpty() && !SDS_CMainWindow->m_bPrintHoldFill)
	if(!SDS_CMainWindow->m_bPrintHoldFill)
	{
		m_pMemDC->FillRect(m_rectClient, m_pbrBkgnd);
		if(!str.IsEmpty())
		{
			POSITION	pos = pTextList->GetHeadPosition();
			for(int lineNum = 0; pos; ++lineNum)
				m_pMemDC->TextOut(0, m_nFontHgt * lineNum, pTextList->GetNext(pos));
		}
		InvalidateRect(m_rectClient);
	}

	delete pTextList;

	m_strPrev = str;
	return;
}

void
CIcadPromptWndEdit::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_bGotRButtonDown = TRUE;
	CWnd::OnRButtonDown(nFlags, point);
}
