#include "icad.h"
#include "LayoutTabs.h"
#include "LayoutRename.h"
#include "cmdsLayout.h"

#define ID_LAYOUT_PAGESETUP             34050
#define ID_LAYOUT_NEW		            34051
#define ID_LAYOUT_DELETE	            34052
#define ID_LAYOUT_RENAME	            34053

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CXOFFSET = 8;		// defined pitch of trapezoid slant
const CXMARGIN = 2;		// left/right text margin
const CYMARGIN = 1;		// top/bottom text margin
const CYBORDER = 1;		// top border thickness
const CXBUTTON = GetSystemMetrics(SM_CXVSCROLL);
const CXBUTTONS = 4 * CXBUTTON;

// Compute the the points, rect and region for a tab.
// Input x is starting x pos.
int CLayoutTabs::CLayoutTab::ComputeRgn(CDC& dc, int x)
{
	m_rgn.DeleteObject();

	m_rect.SetRectEmpty();
	// calculate desired text rectangle
	dc.DrawText(m_name, &m_rect, DT_CALCRECT);
	m_rect.right += 2 * CXOFFSET + 3 * CXMARGIN;	  // add margins
	m_rect.bottom = m_rect.top + GetSystemMetrics(SM_CYHSCROLL);	// ht = scrollbar height
	m_rect += CPoint(x,0);							// shift right

	// create trapezoid region
	CPoint pts[4];
	GetTrapezoid(m_rect, pts);
	m_rgn.CreatePolygonRgn(pts, 4, WINDING);

	return m_rect.Width();
}

// Given the boundint rect, compute trapezoid region.
// Note that the right and bottom edges not included in rect or
// trapezoid; these are normal rules of geometry. 
void CLayoutTabs::CLayoutTab::GetTrapezoid(const CRect& rc, CPoint* pts) const
{
	pts[0] = rc.TopLeft();
	pts[1] = CPoint(rc.left + CXOFFSET,   rc.bottom);
	pts[2] = CPoint(rc.right- CXOFFSET-1, rc.bottom);
	pts[3] = CPoint(rc.right-1, rc.top);
}

// Draw tab in normal or highlighted state
int CLayoutTabs::CLayoutTab::Draw(CDC& dc, CFont& font, BOOL bSelected)
{
	COLORREF bgColor = GetSysColor(bSelected ? COLOR_WINDOW     : COLOR_3DFACE);
	COLORREF fgColor = GetSysColor(bSelected ? COLOR_WINDOWTEXT : COLOR_BTNTEXT);

	CBrush brush(bgColor);					 // background brush
	dc.SetBkColor(bgColor);					 // text background
	dc.SetTextColor(fgColor);				 // text color = fg color

	CPen blackPen(PS_SOLID, 1, RGB(0,0,0));	// black
	CPen shadowPen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));

	CPoint pts[4];
	CRect rc = m_rect;
	GetTrapezoid(rc, pts);
	CPen* pOldPen = dc.SelectObject(&blackPen);
	dc.FillRgn(&m_rgn, &brush);

	// Draw edges. This is requires two corrections:
	// 1) Trapezoid dimensions don't include the right and bottom edges,
	// so must use one pixel less on bottom (cybottom)
	// 2) the endpoint of LineTo is not included when drawing the line, so
	// must add one pixel (cytop)
	//
	pts[1].y--;			// correction #1: true bottom edge y-coord
	pts[2].y--;			// ...ditto
	pts[3].y--;			// correction #2:	extend final LineTo
	dc.MoveTo(pts[0]);						// upper left
	dc.LineTo(pts[1]);						// bottom left

	dc.SelectObject(&shadowPen);			// bottom line is shadow color
	dc.MoveTo(pts[1]);						// line is inside trapezoid bottom
	dc.LineTo(pts[2]);						// ...

	dc.SelectObject(&blackPen);			// upstroke is black
	dc.LineTo(pts[3]);						// y-1 to include endpoint
	if(!bSelected) 
	{
		// if not highlighted, upstroke has a 3D shadow, one pixel inside
		pts[2].x--;		// offset left one pixel
		pts[3].x--;		// ...ditto
		dc.SelectObject(&shadowPen);
		dc.MoveTo(pts[2]);
		dc.LineTo(pts[3]);
	}
	dc.SelectObject(pOldPen);

	// draw text
	rc.DeflateRect(CXOFFSET + CXMARGIN, CYMARGIN);
	CFont* pOldFont = dc.SelectObject(&font);
	dc.DrawText(m_name, &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	dc.SelectObject(pOldFont);

	return m_rect.right;
}

void CLayoutTabs::CButtonEx::DrawItem(LPDRAWITEMSTRUCT lpDis)
{
	DRAWITEMSTRUCT& dis = *lpDis;
	CDC& dc = *CDC::FromHandle(dis.hDC);
	CRect rc;
	GetClientRect(&rc);
	dc.FillSolidRect(&rc,GetSysColor(COLOR_3DFACE));
	BOOL bDown = dis.itemState & ODS_SELECTED;
	if (bDown) 
		rc += CPoint(1,1);

	CPen pen(PS_SOLID, 1, dis.itemState & ODS_DISABLED ? GetSysColor(COLOR_3DSHADOW) : RGB(0,0,0));
	CPen* pOldPen = dc.SelectObject(&pen);
	dc.MoveTo(rc.TopLeft());
	dc.LineTo(rc.right, rc.top);
	CRect rc2 = rc;
	for (int i = 0; i < 2; i++) 
	{
		dc.Draw3dRect(&rc2,	GetSysColor(bDown ? COLOR_3DFACE : COLOR_3DHIGHLIGHT), GetSysColor(COLOR_3DSHADOW));
		rc2.right--;
		rc2.bottom--;
	}

	//  this number needs to be odd so that will get 1 pixel at the end
	//  Paul had it set to 7 - Excel appears to have 9
	int arrowHeight = 9;  
	//  this number appears to be (arrowHeight / 2 + 1)
	//  Paul had this set to 4 - Excel appears to have 5
	int arrowWidth = 5;
	int cyMargin = (rc.Height() - arrowHeight) / 2;
	int cxMargin = (rc.Width() - arrowWidth) / 2;

	//  draw the arrows (left or right depending on button)
	int x = 0;
	int incr = 0;
	switch (dis.CtlID)
	{
	case eFIRST:
		x = rc.right - cxMargin - 1;
		incr = -1;
		break;
	case ePREV:
		x = rc.right - cxMargin - 2;
		incr = -1;
		break;
	case eNEXT:
		x = rc.left + cxMargin;
		incr = 1;
		break;
	case eLAST:
		x = rc.left + cxMargin - 1;
		incr = 1;
		break;
	}
	int y = rc.top + cyMargin;
	int h = arrowHeight;
	for (int j = 0; j < arrowWidth; j++) 
	{
		dc.MoveTo (x, y);
		dc.LineTo (x, y + h);
		h -= 2;
		x += incr;
		y++;
	}
	//  draw the vertical line for the first and last buttons
	if (dis.CtlID == eFIRST) 
	{
		//  the vertical line is before the arrow
		x--;
		y = rc.top + cyMargin;
		dc.MoveTo (x, y);
		dc.LineTo (x, y + arrowHeight);
	}
	if (dis.CtlID == eLAST) 
	{
		//  the vertical line is after the arrow
		x++;
		y = rc.top + cyMargin;
		dc.MoveTo (x, y);
		dc.LineTo (x, y + arrowHeight);
	}
	dc.SelectObject(pOldPen);
}

//////////////////////////////////////////////////////////////////
// CLayoutTabs
IMPLEMENT_DYNAMIC(CLayoutTabs, CWnd)
BEGIN_MESSAGE_MAP(CLayoutTabs, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_BN_CLICKED(CButtonEx::eFIRST, OnFirstTab)
	ON_BN_CLICKED(CButtonEx::ePREV, OnPrevTab)
	ON_BN_CLICKED(CButtonEx::eNEXT, OnNextTab)
	ON_BN_CLICKED(CButtonEx::eLAST, OnLastTab)
	ON_COMMAND(ID_LAYOUT_PAGESETUP, OnPageSetup)
	ON_COMMAND(ID_LAYOUT_NEW, OnLayoutNew)
	ON_COMMAND(ID_LAYOUT_DELETE, OnLayoutDelete)
	ON_COMMAND(ID_LAYOUT_RENAME, OnLayoutRename)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

LPCTSTR CLayoutTabs::s_lpClassName = _T("IntelliCADLayoutTabs");
BOOL CLayoutTabs::s_bRegistered = FALSE;

CLayoutTabs::CLayoutTabs()
{
	m_iCurItem =
	m_cxDesired =
	m_iFirstTab = 0;
	m_iEditedTab = -1;
}

CLayoutTabs::~CLayoutTabs()
{
	while (!m_tabs.IsEmpty())
		delete (CLayoutTab*)m_tabs.RemoveHead();
}

// Create folder tab control from static control.
// Destroys the static control. This is convenient for dialogs
BOOL CLayoutTabs::CreateFromStatic(UINT nID, CWnd* pParent)
{
	CStatic wndStatic;
	if (!wndStatic.SubclassDlgItem(nID, pParent))
		return FALSE;
	CRect rc;
	wndStatic.GetWindowRect(&rc);
	pParent->ScreenToClient(&rc);
	wndStatic.DestroyWindow();
	rc.bottom = rc.top + GetSystemMetrics(SM_CYHSCROLL);
	return Create(WS_CHILD|WS_VISIBLE, rc, pParent, nID);
}

BOOL CLayoutTabs::Create(DWORD dwStyle, const RECT& rc, CWnd* pParent, UINT nID)
{
	ASSERT(pParent);
	ASSERT(dwStyle & WS_CHILD);

	if(!s_bRegistered) 
	{
		WNDCLASS wc;
		memset(&wc, 0, sizeof(wc));
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS | CS_DBLCLKS;
		wc.lpfnWndProc = (WNDPROC)::DefWindowProc; // will get hooked by MFC
		wc.hInstance = AfxGetInstanceHandle();
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
		wc.lpszMenuName = NULL;
		wc.lpszClassName = s_lpClassName;
		if (!AfxRegisterClass(&wc)) 
		{
			TRACE("*** CLayoutTabs::AfxRegisterClass failed!\n");
			return FALSE;
		}
		s_bRegistered = TRUE;
	}
	if (!CWnd::CreateEx(0, s_lpClassName, NULL, dwStyle, rc, pParent, nID))
		return FALSE;

	CFont m_font;
	m_font.CreateStockObject(DEFAULT_GUI_FONT);
	return TRUE;
}

int CLayoutTabs::HitTest(CPoint pt)
{
	CRect rc;
	GetClientRect(&rc);
	rc.left += CXBUTTONS;
	if(rc.PtInRect(pt)) 
	{
		int xOrigin = CXBUTTONS - GetTab(m_iFirstTab)->GetRect().left;
		pt.x -= xOrigin;
		for(int i = 0; i < GetItemCount(); i++) 
		{
			if(GetTab(i)->HitTest(pt))
				return i;
		}
	}
	return -1;
}

void CLayoutTabs::UpdateLayoutTabs(db_drawing* pDrawing, BOOL bInit)
{
	char layoutName[IC_ACADBUF];
	TC_ITEM layoutTab;
	layoutTab.mask = TCIF_TEXT;
	layoutTab.pszText = layoutName;
	if(bInit)
	{
		DeleteAllItems();
		int index = 0;
		const char* pName;
		CLayoutManager layoutManager(pDrawing);
		layoutManager.resetIterator();
		while(layoutManager.next(pName, NULL, NULL, true))
		{
			strncpy(layoutName, pName, IC_ACADBUF - 1);
			InsertItem(index, &layoutTab);
			++index;
		}
	}
	struct resbuf curTab;
	sds_getvar("CTAB", &curTab);

	layoutTab.cchTextMax = IC_ACADBUF - 1;
	for(int i = 0; i < GetItemCount(); ++i)
	{
		GetItem(i, &layoutTab);
		if(stricmp(curTab.resval.rstring, layoutName) == 0)
			break;
	}
	SetCurSel(i);
	IC_FREE(curTab.resval.rstring);

	Invalidate();
}

int CLayoutTabs::SetCurSel(int nItem)
{
	if (nItem < 0 || nItem >= GetItemCount())
		return -1;
	if (nItem == m_iCurItem)
		return nItem;

	int iPrevItem = m_iCurItem;
	if(iPrevItem < GetItemCount())
		InvalidateTab(iPrevItem);
	m_iCurItem = nItem;
	InvalidateTab(m_iCurItem);
	return iPrevItem;
}

LONG CLayoutTabs::InsertItem(int nItem, TC_ITEM* pItem)
{
	ASSERT(nItem >= 0);
	ASSERT(pItem->mask = TCIF_TEXT);

	LONG index = nItem;
	if(nItem == 0)
		m_tabs.AddHead(new CLayoutTab(pItem->pszText));
	else
	{
		POSITION pos = m_tabs.FindIndex(nItem - 1);
		if(pos == NULL)
		{
			m_tabs.AddTail(new CLayoutTab(pItem->pszText));
			index = GetItemCount() - 1;
		}
		else
			m_tabs.InsertAfter(pos, new CLayoutTab(pItem->pszText));
	}
	RecomputeLayout();
	return index;
}

BOOL CLayoutTabs::GetItem(int nItem, TC_ITEM* pItem) const
{
	ASSERT(nItem >= 0);
	ASSERT(pItem->mask = TCIF_TEXT);

	POSITION pos = m_tabs.FindIndex(nItem);
	if(pos)
	{
		CLayoutTab* pTab = (CLayoutTab*)m_tabs.GetAt(pos);
		strncpy(pItem->pszText, pTab->GetText(), pItem->cchTextMax);
		return TRUE;
	}
	return FALSE;
}

BOOL CLayoutTabs::DeleteItem(int nItem)
{
	ASSERT(nItem >= 0);

	POSITION pos = m_tabs.FindIndex(nItem);
	if(pos) 
	{
		CLayoutTab* pTab = (CLayoutTab*)m_tabs.GetAt(pos);
		m_tabs.RemoveAt(pos);
		delete pTab;
		RecomputeLayout();
		return TRUE;
	}
	return FALSE;
}

BOOL CLayoutTabs::DeleteAllItems()
{
	while (!m_tabs.IsEmpty())
		delete (CLayoutTab*)m_tabs.RemoveHead();
	return TRUE;
}

void CLayoutTabs::SetFont(CFont* pFont, BOOL bRedraw)
{
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	m_fontToUse.CreateFontIndirect(&lf);
}

void CLayoutTabs::InvalidateTab(int iTab, BOOL bErase)
{
	CRect rc = GetTab(iTab)->GetRect();
	CLayoutTab* pTab = GetTab(m_iFirstTab);
	int xOrigin = 0;
	if(pTab)
		xOrigin = CXBUTTONS - pTab->GetRect().left;
	else
		xOrigin = CXBUTTONS;
	rc += CPoint(xOrigin, 0);
	InvalidateRect(rc, bErase);
}

CLayoutTabs::CLayoutTab* CLayoutTabs::GetTab(int iPos)
{
	POSITION pos = m_tabs.FindIndex(iPos);
	return pos ? static_cast<CLayoutTab*>(m_tabs.GetAt(pos)) : NULL;
}

void CLayoutTabs::RecomputeLayout()
{
	CClientDC dc(this);

	CFont selectedFont;
	LOGFONT lf;
	m_fontToUse.GetLogFont(&lf);
	lf.lfWeight |= FW_BOLD;
	selectedFont.CreateFontIndirect(&lf);

	CFont* pOldFont = NULL;
	pOldFont = dc.SelectObject(&selectedFont);
	int x = 0;
	CLayoutTab* pTab = NULL;
	for (int i = 0; i < GetItemCount(); i++) 
		x += GetTab(i)->ComputeRgn(dc, x) - CXOFFSET;
	dc.SelectObject(pOldFont);

	m_cxDesired = CXBUTTONS;
	if (pTab) 
	{
		CRect rc = pTab->GetRect();
		m_cxDesired += rc.right;
	}
}

void CLayoutTabs::OnPaint() 
{
	CPaintDC dc(this);

	CLayoutTab* pFirstTab = GetTab(m_iFirstTab);
	int xOrigin = 0;
	if(pFirstTab)
		xOrigin = CXBUTTONS - pFirstTab->GetRect().left;
	dc.SetViewportOrg(xOrigin, 0);

	CLayoutTab* pCurTab = NULL;
	for(int i = 0; i < GetItemCount(); i++) 
	{
		if (i == m_iCurItem) 
			pCurTab = GetTab(i);
		else
			GetTab(i)->Draw(dc, m_fontToUse, FALSE);
	}

	CFont selectedFont;
	LOGFONT lf;
	m_fontToUse.GetLogFont(&lf);
	lf.lfWeight |= FW_BOLD;
	selectedFont.CreateFontIndirect(&lf);

	if(pCurTab)
		pCurTab->Draw(dc, selectedFont, TRUE);

	// draw border: line along the top edge, excluding selected tab
	CRect rcCurTab(0, 0, 0, 0);
	if(pCurTab)
		rcCurTab = pCurTab->GetRect();

	CPen blackPen;
	blackPen.CreateStockObject(BLACK_PEN);
	CPen* pOldPen = dc.SelectObject(&blackPen);

	CRect rc;
	GetClientRect(&rc);
	rc.right -= xOrigin;
	dc.MoveTo(rcCurTab.right, rcCurTab.top);
	dc.LineTo(rc.right, rc.top);
	dc.MoveTo(rc.left, rc.top);
	dc.LineTo(rcCurTab.TopLeft());

	dc.SelectObject(pOldPen);
}

void CLayoutTabs::OnLButtonDown(UINT nFlags, CPoint pt)
{
	int iTab = HitTest(pt);
	if (iTab >= 0 && iTab != m_iCurItem) 
	{
		SetCurSel(iTab);
		
		// change current layout
		char layoutName[IC_ACADBUF];
		struct resbuf newTab;
		newTab.restype = RTSTR;
		newTab.resval.rstring = layoutName;
		
		TCITEM layoutTab;
		layoutTab.mask = TCIF_TEXT;
		layoutTab.pszText = layoutName;
		layoutTab.cchTextMax = IC_ACADBUF - 1;
		GetItem(GetCurSel(), &layoutTab);
		
		sds_setvar("CTAB", &newTab);
	}
}

void CLayoutTabs::OnRButtonDown(UINT nFlags, CPoint pt)
{
	int iSelTab;
	m_iEditedTab = HitTest(pt);
	if (m_iEditedTab >= 0) 
	{
		iSelTab = SetCurSel(m_iEditedTab);
		
		HMENU hmenu = CreatePopupMenu ();
		AppendMenu(hmenu, MF_ENABLED | MF_STRING, ID_LAYOUT_NEW, ResourceString(IDC_LAYOUT_OPTIONNEW, "New"));
		if(stricmp(GetTab(m_iEditedTab)->GetText(), _T("Model")) == 0)
		{
			AppendMenu(hmenu, MF_GRAYED | MF_DISABLED | MF_STRING, ID_LAYOUT_DELETE, ResourceString(IDC_LAYOUT_OPTIONDELETE, "Delete"));
			AppendMenu(hmenu, MF_GRAYED | MF_DISABLED | MF_STRING, ID_LAYOUT_RENAME, ResourceString(IDC_LAYOUT_OPTIONRENAME, "Rename"));
		}
		else
		{
			AppendMenu(hmenu, MF_ENABLED | MF_STRING, ID_LAYOUT_DELETE, ResourceString(IDC_LAYOUT_OPTIONDELETE, "Delete"));
			AppendMenu(hmenu, MF_ENABLED | MF_STRING, ID_LAYOUT_RENAME, ResourceString(IDC_LAYOUT_OPTIONRENAME, "Rename"));
		}
// Remove Page Setup until it can be supported
//		AppendMenu(hmenu, MF_SEPARATOR, NULL, "");
//		AppendMenu(hmenu, MF_GRAYED | MF_DISABLED | MF_STRING, ID_LAYOUT_PAGESETUP, ResourceString(IDC_LAYOUT_PAGESETUP, "Page Setup..."));
		
		// display menu next to the selected tab
		ClientToScreen(&pt);
		TrackPopupMenuEx(hmenu, TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd, NULL);
		
		DestroyMenu(hmenu);
		SetCurSel(iSelTab);
	}
}

int CLayoutTabs::OnCreate(LPCREATESTRUCT lpcs)
{
	if (CWnd::OnCreate(lpcs)!=0)
		return -1;

	CRect rc(0,0,0,0);
	for(int i = CButtonEx::eFIRST; i <= CButtonEx::eLAST; i++) 
		m_buttons[i - 1].Create(NULL, WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, rc, this, i);
	return 0;
}

void CLayoutTabs::OnSize(UINT nType, int cx, int cy)
{
	if(m_buttons[0].m_hWnd) 
	{
		int w = cy;
		CRect rc(0,0,w,cy);
		for (int i = CButtonEx::eFIRST; i <= CButtonEx::eLAST; i++) 
		{
			m_buttons[i-1].MoveWindow(&rc);
			rc += CPoint(w,0);
		}
	}
}

void CLayoutTabs::OnFirstTab()
{
	m_iFirstTab = 0;
	Invalidate();
}

void CLayoutTabs::OnNextTab()
{
	if(m_iFirstTab < m_tabs.GetCount() - 1) 
	{
		m_iFirstTab++;
		Invalidate();
	}
}

void CLayoutTabs::OnPrevTab()
{
	if (m_iFirstTab > 0) 
	{
		m_iFirstTab--;
		Invalidate();
	}
}

void CLayoutTabs::OnLastTab()
{
	m_iFirstTab = m_tabs.GetCount() - 1;
	Invalidate();
}

void CLayoutTabs::OnPageSetup()
{
	// TODO: currently not supported
}

void CLayoutTabs::OnLayoutNew()
{
	CLayoutManager layoutManager(SDS_CURDWG);
	/*
	char layoutName[IC_ACADBUF];
	int freeIndex = 1;
	sprintf(layoutName, "Layout%d", freeIndex);
	while(layoutManager.findLayoutNamed(layoutName))
	{
		++freeIndex;
		sprintf(layoutName, "Layout%d", freeIndex);
	}
	layoutManager.createLayout(layoutName);
	*/
	layoutManager.createLayout(layoutManager.defaultName());
	UpdateLayoutTabs(SDS_CURDWG, TRUE);
}

extern "C" short cmd_redraw();
void CLayoutTabs::OnLayoutDelete()
{
	if(m_iEditedTab < 0)
		return;

	if(MessageBox(ResourceString(IDC_LAYOUT_WARNDELETE, "The selected layout will be deleted.\nTo delete the selected layout, click OK\nTo cancel the deletion, click Cancel\n\nThe Model tab may not be deleted"), 
		ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD"), MB_ICONEXCLAMATION  | MB_OKCANCEL) == IDOK)
	{
		CLayoutManager layoutManager(SDS_CURDWG);
		layoutManager.deleteLayout(GetTab(m_iEditedTab)->GetText());
		UpdateLayoutTabs(SDS_CURDWG, TRUE);
		
		cmd_redraw();
	}
}

void CLayoutTabs::OnLayoutRename()
{
	if(m_iEditedTab < 0)
		return;

	CLayoutRename renameDialog(this, GetTab(m_iEditedTab)->GetText());
	INT_PTR result = renameDialog.DoModal();
	if(result == IDOK)
	{
		CLayoutManager layoutManager(SDS_CURDWG);
		if(!ic_isvalidname((char*)renameDialog.layoutName()))
		{
			MessageBox(ResourceString(IDC_LAYOUT_NOTVALIDNAME, "This is not valid name for layout"));
			return;
		}
		if(_tcsicmp(GetTab(m_iEditedTab)->GetText(), renameDialog.layoutName()) != 0 &&
			layoutManager.findLayoutNamed(renameDialog.layoutName()))
		{
			MessageBox(ResourceString(IDC_LAYOUT_ALREADYEXIST, "Layout with such name already exists"));
			return;
		}
		layoutManager.renameLayout(GetTab(m_iEditedTab)->GetText(), renameDialog.layoutName());
		UpdateLayoutTabs(SDS_CURDWG, TRUE);
	}
}
