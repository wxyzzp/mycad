/* ICAD\LISTCTRLEX.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 *	Implementation of CListCtrlEx class - an extended list control class that provides editable sub-items
 *
 */

#include "Icad.h"
#include "ListCtrlEx.h"/*DNT*/
#include "IcadExplorer.h"

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx

CListCtrlEx::CListCtrlEx()
{
	m_nHighlight = HIGHLIGHT_NORMAL;
	m_pEdit = NULL;
}

CListCtrlEx::~CListCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlEx)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_CHAR()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx message handlers

void CListCtrlEx::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( GetFocus() != this ) SetFocus();
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CListCtrlEx::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( GetFocus() != this ) SetFocus();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CListCtrlEx::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
 	LV_ITEM		 *plvItem = &pDispInfo->item;

	if (plvItem->pszText != NULL)
	{
		SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
	}
	*pResult = FALSE;
}

void CListCtrlEx::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar==VK_ESCAPE)
	{
		GetParent()->SendMessage( WM_CHAR, nChar, 0);
	}
	else
	{
		CListCtrl::OnChar(nChar, nRepCnt, nFlags);
	}
}

// HitTestEx	- Determine the row index and column index for a point
// Returns	- the row index or -1 if point is not over a row
// point	- point to be tested.// col		- to hold the column index
int CListCtrlEx::HitTestEx(CPoint &point, int *col)
{
	int colnum = 0;
	int row = HitTest( point, NULL );
	if( col ) *col = 0;

	// Make sure that the ListView is in LVS_REPORT
	if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
		return row;

	// Get the top and bottom row visible
	row = GetTopIndex();
	int bottom = row + GetCountPerPage();
	if( bottom > GetItemCount() )
		bottom = GetItemCount();

	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	// Loop through the visible rows
	for( ;row <=bottom;row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		GetItemRect( row, &rect, LVIR_BOUNDS );
		if( rect.PtInRect(point) )
		{
			// Now find the column
			for( colnum = 0; colnum < nColumnCount; colnum++ )
			{
				int colwidth = GetColumnWidth(colnum);
				if( point.x >= rect.left && point.x <= (rect.left + colwidth ) )
				{
					if( col ) *col = colnum;
						return row;
				}
				rect.left += colwidth;
			}
		}
	}
	return -1;
}


// EditSubLabel	- Start edit of a sub item label
// Returns		- Temporary pointer to the new edit control
// nItem		- The row index of the item to edit
// nCol			- The column of the sub item.

CEdit * CListCtrlEx::EditSubLabel( int nItem, int nCol )
{
	// The returned pointer should not be saved
	// Make sure that the item is visible
	if( !EnsureVisible( nItem, TRUE ) )
		return NULL;

	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if( nCol >= nColumnCount || GetColumnWidth(nCol) < 5 )
		return NULL;

	// Get the column offset
	int offset = 0;
	for( int i = 0; i < nCol; i++ )
		offset += GetColumnWidth( i );

	CRect rect;
	GetItemRect( nItem, &rect, LVIR_BOUNDS );

	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect( &rcClient );
	if( offset + rect.left < 0 || offset + rect.left > rcClient.right )
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll( size );
		rect.left -= size.cx;
	}

	// Get Column alignment
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	GetColumn( nCol, &lvcol );
	DWORD dwStyle ;
	if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwStyle = ES_LEFT;
	else if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
	else dwStyle = ES_CENTER;
	rect.left += offset+4;
	rect.right = rect.left + GetColumnWidth( nCol ) - 3 ;
	if( rect.right > rcClient.right)
		rect.right = rcClient.right;
	dwStyle |= WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;
	//CEdit *pEdit = new CInPlaceEdit(nItem, nCol, GetItemText( nItem, nCol ));
	m_pEdit = new CInPlaceEdit(nItem, nCol, GetItemText( nItem, nCol ));
	//resize rectangle for minor adjustments
	rect.top -= 2;
	rect.bottom += 1;
	m_pEdit->Create( dwStyle, rect, this, IDC_EXPCOL_EDITBOX );
	return m_pEdit;
}

void CListCtrlEx::EndLabelEditing()
{
	if (m_pEdit) //we are editing a subitem
	{
		// Send Notification to parent of ListView ctrl
		LV_DISPINFO dispinfo;
		dispinfo.hdr.hwndFrom = m_pEdit->GetParent()->m_hWnd;
		dispinfo.hdr.idFrom = m_pEdit->GetDlgCtrlID();
		dispinfo.hdr.code = LVN_ENDLABELEDIT;
		dispinfo.item.mask = LVIF_TEXT;
		dispinfo.item.iItem = -1;
		dispinfo.item.iSubItem = -1;
		dispinfo.item.pszText = NULL;
		dispinfo.item.cchTextMax = 0;
		if (dispinfo.item.cchTextMax == 0) dispinfo.item.pszText = NULL;
		m_pEdit->GetParent()->GetParent()->SendMessage( WM_NOTIFY, m_pEdit->GetParent()->GetDlgCtrlID(),
						(LPARAM)&dispinfo );
		m_pEdit->DestroyWindow();
		m_pEdit = NULL;
	}
	else
	{
		CEdit *pEdit;
		if (pEdit = GetEditControl())	//we are editing the main item
		{
			LV_DISPINFO dispinfo;
			dispinfo.hdr.hwndFrom = pEdit->GetParent()->m_hWnd;
			dispinfo.hdr.idFrom = pEdit->GetDlgCtrlID();
			dispinfo.hdr.code = LVN_ENDLABELEDIT;
			dispinfo.item.mask = LVIF_TEXT;
			dispinfo.item.iItem = -1;
			dispinfo.item.iSubItem = -1;
			dispinfo.item.pszText = NULL;
			dispinfo.item.cchTextMax = 0;
			if (dispinfo.item.cchTextMax == 0) dispinfo.item.pszText = NULL;
			pEdit->GetParent()->GetParent()->SendMessage( WM_NOTIFY, pEdit->GetParent()->GetDlgCtrlID(),
							(LPARAM)&dispinfo );
			pEdit->DestroyWindow();
		}
	}
}

// ShowInPlaceList	- Creates an in-place drop down list for any cell in the list view control
// Returns			- A temporary pointer to the combo-box control
// nItem			- The row index of the cell
// nCol				- The column index of the cell
// lstItems			- A list of strings to populate the control with
// nSel				- Index of the initial selection in the drop down list

CComboBox * CListCtrlEx::ShowInPlaceList( int nItem, int nCol, CStringList &lstItems, int nSel )
{
	// The returned pointer should not be saved
	// Make sure that the item is visible
	if( !EnsureVisible( nItem, TRUE ) )
		return NULL;

	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if( nCol >= nColumnCount || GetColumnWidth(nCol) < 10 )
		return NULL;

	// Get the column offset
	int offset = 0;
	for( int i = 0; i < nCol; i++ )
		offset += GetColumnWidth( i );

	CRect rect;
	GetItemRect( nItem, &rect, LVIR_BOUNDS );

	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect( &rcClient );
	if( offset + rect.left < 0 || offset + rect.left > rcClient.right )
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll( size );
		rect.left -= size.cx;
	}
	rect.left += offset+4;
	rect.right = rect.left + GetColumnWidth( nCol ) - 3 ;
	int height = rect.bottom-rect.top;
	rect.bottom += 5*height;
	if( rect.right > rcClient.right)
		rect.right = rcClient.right;
	DWORD dwStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL//|WS_HSCROLL
					|CBS_DROPDOWNLIST|CBS_DISABLENOSCROLL;
	CComboBox *pList = new CInPlaceList(nItem, nCol, &lstItems, nSel);
	pList->Create( dwStyle, rect, this, IDC_EXPCOL_COMBOBOX );
	pList->SetItemHeight( -1, height);
	pList->SetHorizontalExtent( GetColumnWidth( nCol ));
	return pList;
}

// ShowInPlaceFontNameCombo	- Creates an in-place drop down list for the font names in the list view control
// Returns					- A temporary pointer to the combo-box control; this SHOULD NOT be saved
// nItem					- The row index of the cell
// nCol						- The column index of the cell

// to do - condense the code for the various combo boxes - the heading/scrolling/etc. code can be
//		   placed in a function so it only appears once

CInPlaceFontNameCombo * CListCtrlEx::ShowInPlaceFontNameCombo(
	int item,
	int col,
	const char* fontName,
	int which)	// EBATECH(CNBR) Add which
	{
	// make sure that the item is visible
	if(!EnsureVisible(item, TRUE ))
		return NULL;

	// make sure that column is valid
	CHeaderCtrl *pHeader = (CHeaderCtrl*) GetDlgItem(0);
	int columnCount = pHeader->GetItemCount();
	if((col >= columnCount) || (GetColumnWidth(col) < 10))
		return NULL;

	// get the column offset
	int offset = 0;
	for(int i = 0; i < col; i++)
		offset += GetColumnWidth(i);

	CRect rect;
	GetItemRect(item, &rect, LVIR_BOUNDS);

	// now scroll if we need to expose the column
	CRect clientRect;
	GetClientRect(&clientRect);
	if((offset + rect.left < 0) || (offset + rect.left > clientRect.right))
		{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll(size);
		rect.left -= size.cx;
		}

	// create the combo box
	rect.left += offset + 4;
	rect.right = rect.left + GetColumnWidth(col) - 3;
	int height = rect.bottom - rect.top;
	rect.bottom += 5 * height;
	if(rect.right > clientRect.right)
		rect.right = clientRect.right;

	DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL
					| CBS_SORT | CBS_DROPDOWNLIST;

	CInPlaceFontNameCombo *combo = new CInPlaceFontNameCombo(item, col, fontName, which); //EBATECH(CNBR)
	combo->Create(dwStyle, rect, this, IDC_EXPCOL_COMBOBOX);
	combo->SetItemHeight(-1, height);
	combo->SetHorizontalExtent(GetColumnWidth(col));
	combo->SetDroppedWidth(GetComboBoxLongestStrWidth(combo));

	return combo;
	}

// ShowInPlaceFontStyleCombo- Creates an in-place drop down list for the font styles in the list view control
// Returns					- A temporary pointer to the combo-box control; this SHOULD NOT be saved
// nItem					- The row index of the cell
// nCol						- The column index of the cell

CInPlaceFontStyleCombo * CListCtrlEx::ShowInPlaceFontStyleCombo(
	int item,
	int col,
	//const char* fontStyle,	EBATECH(CNBR)
	long stylenumber)
	{
	// make sure that the item is visible
	if(!EnsureVisible(item, TRUE ))
		return NULL;

	// make sure that column is valid
	CHeaderCtrl *pHeader = (CHeaderCtrl*) GetDlgItem(0);
	int columnCount = pHeader->GetItemCount();
	if((col >= columnCount) || (GetColumnWidth(col) < 10))
		return NULL;

	// get the column offset
	int offset = 0;
	for(int i = 0; i < col; i++)
		offset += GetColumnWidth(i);

	CRect rect;
	GetItemRect(item, &rect, LVIR_BOUNDS);

	// now scroll if we need to expose the column
	CRect clientRect;
	GetClientRect(&clientRect);
	if((offset + rect.left < 0) || (offset + rect.left > clientRect.right))
		{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll(size);
		rect.left -= size.cx;
		}

	CString fontName = GetItemText(item, STYLE_FONT_NAME_COLUMN);

	// we had a problem with the font name being empty; though this
	// should never happen, we'll trap for it and return if necessary
	CString tmpFontName = fontName;
	tmpFontName.TrimLeft();
	if (tmpFontName.IsEmpty())
		return NULL;

	// if this isn't a TrueType font, return
	char tmpStr[IC_ACADBUF];
	strcpy(tmpStr, fontName);
	if ((0 == ic_chkext(tmpStr, "shx")) || (0 == ic_chkext(tmpStr, "shp")))
		return NULL;

	// create the combo box
	rect.left += offset + 4;
	rect.right = rect.left + GetColumnWidth(col) - 3;
	int height = rect.bottom - rect.top;
	rect.bottom += 5 * height;
	if(rect.right > clientRect.right)
		rect.right = clientRect.right;

	DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL
					| CBS_DROPDOWNLIST;
	// EBATECH(CNBR) -[
	//CInPlaceFontStyleCombo *combo = new CInPlaceFontStyleCombo(item, col, fontName, fontStyle);
	CInPlaceFontStyleCombo *combo = new CInPlaceFontStyleCombo(item, col, fontName);
	// EBATECH(CNBR) ]-
	combo->Create(dwStyle, rect, this, IDC_EXPCOL_COMBOBOX);
	combo->SetItemHeight(-1, height);
	combo->SetHorizontalExtent(GetColumnWidth(col));
	combo->SetDroppedWidth(GetComboBoxLongestStrWidth(combo));
	if (combo->GetCount() == 1)
		{
		CString cstext;
		combo->GetWindowText(cstext);
		if (cstext.IsEmpty())	// error
			{
			if ((0x3000000 & stylenumber) == 0x3000000)		// Bold Italics
				cstext = "Bold Italic";
			else if (0x2000000 & stylenumber)				// Bold
				cstext = "Bold";
			else if (0x1000000 & stylenumber)				// Italics
				cstext = "Italic";
			else
				cstext = "Regular";
			combo->DeleteString(0);
			combo->AddString(cstext);
			}
		}

	return combo;
	}

// EBATECH(CNBR) -[
// ShowInPlaceFontLangCombo- Creates an in-place drop down list for the font styles in the list view control
// Returns					- A temporary pointer to the combo-box control; this SHOULD NOT be saved
// nItem					- The row index of the cell
// nCol						- The column index of the cell

CInPlaceFontLangCombo * CListCtrlEx::ShowInPlaceFontLangCombo(
	int item,
	int col,
	//const char* fontStyle,	EBATECH(CNBR)
	long stylenumber)
	{
	// make sure that the item is visible
	if(!EnsureVisible(item, TRUE ))
		return NULL;

	// make sure that column is valid
	CHeaderCtrl *pHeader = (CHeaderCtrl*) GetDlgItem(0);
	int columnCount = pHeader->GetItemCount();
	if((col >= columnCount) || (GetColumnWidth(col) < 10))
		return NULL;

	// get the column offset
	int offset = 0;
	for(int i = 0; i < col; i++)
		offset += GetColumnWidth(i);

	CRect rect;
	GetItemRect(item, &rect, LVIR_BOUNDS);

	// now scroll if we need to expose the column
	CRect clientRect;
	GetClientRect(&clientRect);
	if((offset + rect.left < 0) || (offset + rect.left > clientRect.right))
		{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll(size);
		rect.left -= size.cx;
		}

	CString fontName = GetItemText(item, STYLE_FONT_NAME_COLUMN);

	// we had a problem with the font name being empty; though this
	// should never happen, we'll trap for it and return if necessary
	CString tmpFontName = fontName;
	tmpFontName.TrimLeft();
	if (tmpFontName.IsEmpty())
		return NULL;

	// if this isn't a TrueType font, return
	char tmpStr[IC_ACADBUF];
	strcpy(tmpStr, fontName);
	if ((0 == ic_chkext(tmpStr, "shx")) || (0 == ic_chkext(tmpStr, "shp")))
		return NULL;

	// create the combo box
	rect.left += offset + 4;
	rect.right = rect.left + GetColumnWidth(col) - 3;
	int height = rect.bottom - rect.top;
	rect.bottom += 5 * height;
	if(rect.right > clientRect.right)
		rect.right = clientRect.right;

	DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL
					| CBS_DROPDOWNLIST;
	// EBATECH(CNBR) -[
	//CInPlaceFontLangCombo *combo = new CInPlaceFontLangCombo(item, col, fontName, fontStyle);
	CInPlaceFontLangCombo *combo = new CInPlaceFontLangCombo(item, col, fontName);
	// EBATECH(CNBR) ]-
	combo->Create(dwStyle, rect, this, IDC_EXPCOL_COMBOBOX);
	combo->SetItemHeight(-1, height);
	combo->SetHorizontalExtent(GetColumnWidth(col));
	combo->SetDroppedWidth(GetComboBoxLongestStrWidth(combo));
	return combo;
	}
// EBATECH(CNBR) ]-

// EBATECH(CNBR) -[
// ShowInPlaceLWCombo		- Creates an in-place drop down list for the lineweight in the list view control
// Returns					- A temporary pointer to the combo-box control; this SHOULD NOT be saved
// nItem					- The row index of the cell
// nCol						- The column index of the cell

CInPlaceLWCombo * CListCtrlEx::ShowInPlaceLWCombo(int item, int col, int lwunits, int celweight, int bitcode )
	{
	// make sure that the item is visible
	if(!EnsureVisible(item, TRUE ))
		return NULL;

	// make sure that column is valid
	CHeaderCtrl *pHeader = (CHeaderCtrl*) GetDlgItem(0);
	int columnCount = pHeader->GetItemCount();
	if((col >= columnCount) || (GetColumnWidth(col) < 10))
		return NULL;

	// get the column offset
	int offset = 0;
	for(int i = 0; i < col; i++)
		offset += GetColumnWidth(i);

	CRect rect;
	GetItemRect(item, &rect, LVIR_BOUNDS);

	// now scroll if we need to expose the column
	CRect clientRect;
	GetClientRect(&clientRect);
	if((offset + rect.left < 0) || (offset + rect.left > clientRect.right))
		{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll(size);
		rect.left -= size.cx;
		}

	// create the combo box
	rect.left += offset + 4;
	rect.right = rect.left + GetColumnWidth(col) - 3;
	int height = rect.bottom - rect.top;
	rect.bottom += 5 * height;
	if(rect.right > clientRect.right)
		rect.right = clientRect.right;

	DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL
					| CBS_DROPDOWNLIST;
	// EBATECH(CNBR) -[
	CInPlaceLWCombo *combo = new CInPlaceLWCombo(item, col, lwunits, celweight, bitcode );
	// EBATECH(CNBR) ]-
	combo->Create(dwStyle, rect, this, IDC_EXPCOL_COMBOBOX);
	combo->SetItemHeight(-1, height);
	combo->SetHorizontalExtent(GetColumnWidth(col));
	combo->SetDroppedWidth(GetComboBoxLongestStrWidth(combo));
	return combo;
	}

// ToggleSubLabel	-Toggle the value of a sub item label
// Returns		- 1 if succeeded.
// nItem		- The row index of the item to edit
// nCol			- The column of the sub item.
// option1		- Toggle value #1
// option2		- Toggle value #2

int CListCtrlEx::ToggleSubLabel (int nItem, int nCol, CString option1, CString option2)
{
	// The returned pointer should not be saved
	// Make sure that the item is visible
	if( !EnsureVisible( nItem, TRUE ) )
		return NULL;

	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if( nCol >= nColumnCount || GetColumnWidth(nCol) < 5 )
		return NULL;

	// Get the column offset
	int offset = 0;	for( int i = 0; i < nCol; i++ )
		offset += GetColumnWidth( i );

	CRect rect;
	GetItemRect( nItem, &rect, LVIR_BOUNDS );

	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect( &rcClient );
	if( offset + rect.left < 0 || offset + rect.left > rcClient.right )
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll( size );
		rect.left -= size.cx;
	}

	// Get Column alignment
	CString curtext = GetItemText(nItem, nCol);
	CString str;
	if (curtext == option1)
	{
		SetItemText (nItem, nCol, option2);
		str = option2;
	}
	else if (curtext == option2)
	{
		SetItemText (nItem, nCol, option1);
		str = option1;
	}
	else //nothing in this column to start with, so do not toggle
		return 1;

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;
	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = nItem;
	dispinfo.item.iSubItem = nCol;
	dispinfo.item.pszText = LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();
	GetParent()->SendMessage( WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo );
	return 1;
}

void CListCtrlEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem);
	int nItem = lpDrawItemStruct->itemID;
	CImageList* pImageList;

	// Save dc state
	int nSavedDC = pDC->SaveDC();

	// Get item image and state info
	LV_ITEM lvi;
	lvi.mask = LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;		// get all state flags
	GetItem(&lvi);

	// Should the item be highlighted
	BOOL bHighlight =((lvi.state & LVIS_DROPHILITED)
				  || ((lvi.state & LVIS_SELECTED)
				  && ((GetFocus() == this) || ( GetStyle() & LVS_SHOWSELALWAYS))));

	// Get rectangles for drawing
	CRect rcBounds, rcLabel, rcIcon;
	GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
	GetItemRect(nItem, rcLabel, LVIR_LABEL);
	GetItemRect(nItem, rcIcon, LVIR_ICON);
	CRect rcCol( rcBounds );

	CString sLabel = GetItemText( nItem, 0 );

	// Labels are offset by a certain amount
	// This offset is related to the width of a space character
	int offset = pDC->GetTextExtent(_T(" "), 1 ).cx*2;

	CRect rcHighlight;
	CRect rcWnd;
	int nExt;
	switch( m_nHighlight )
	{
	case HIGHLIGHT_NORMAL:
		nExt = pDC->GetOutputTextExtent(sLabel).cx + offset;
		rcHighlight = rcLabel;
		if( rcLabel.left + nExt < rcLabel.right )
			rcHighlight.right = rcLabel.left + nExt;
		break;
	case HIGHLIGHT_ALLCOLUMNS:
		rcHighlight = rcBounds;
		rcHighlight.left = rcLabel.left;
		break;
	case HIGHLIGHT_ROW:
		GetClientRect(&rcWnd);
		rcHighlight = rcBounds;
		rcHighlight.left = rcLabel.left;
		rcHighlight.right = rcWnd.right;
		break;
	default:
		rcHighlight = rcLabel;
	}


	// Draw the background color
	if( bHighlight )
	{
		pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->FillRect(rcHighlight, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}
	else
		pDC->FillRect(rcHighlight, &CBrush(::GetSysColor(COLOR_WINDOW)));

	// Set clip region
	rcCol.right = rcCol.left + GetColumnWidth(0);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rcCol);
	pDC->SelectClipRgn(&rgn);
	rgn.DeleteObject();

	// Draw state icon
	if (lvi.state & LVIS_STATEIMAGEMASK)
	{
		int nImage = ((lvi.state & LVIS_STATEIMAGEMASK)>>12) - 1;
		pImageList = GetImageList(LVSIL_STATE);
		if (pImageList)
		{
			pImageList->Draw(pDC, nImage,
				CPoint(rcCol.left, rcCol.top), ILD_TRANSPARENT);
		}
	}

	// Draw normal and overlay icon
	pImageList = GetImageList(LVSIL_SMALL);
	if (pImageList)
	{
		UINT nOvlImageMask=lvi.state & LVIS_OVERLAYMASK;
		pImageList->Draw(pDC, lvi.iImage,
			CPoint(rcIcon.left, rcIcon.top),
			(bHighlight?ILD_BLEND50:0) | ILD_TRANSPARENT | nOvlImageMask );
	}

	// Draw item label - Column 0
	rcLabel.left += offset/2;
	rcLabel.right -= offset;

	pDC->DrawText(sLabel,-1,rcLabel,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP
				| DT_VCENTER | DT_END_ELLIPSIS);

	// Draw labels for remaining columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;
	if( m_nHighlight == 0 )		// Highlight only first column
	{
		pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
	}

	rcBounds.right = rcHighlight.right > rcBounds.right ? rcHighlight.right :rcBounds.right;
	rgn.CreateRectRgnIndirect(&rcBounds);
	pDC->SelectClipRgn(&rgn);

	for(int nColumn = 1; GetColumn(nColumn, &lvc); nColumn++)
	{
		DrawSubItem (nItem, nColumn, lvc, rcCol, pDC);
	}

	// Draw focus rectangle if item has focus
	if (lvi.state & LVIS_FOCUSED && (GetFocus() == this))
		pDC->DrawFocusRect(rcHighlight);

	// Restore dc
	pDC->RestoreDC( nSavedDC );
}


void CListCtrlEx::DrawSubItem(int nItem, int nColumn, LV_COLUMN& lvc, CRect& rcCol, CDC *pDC)
{
	CString sLabel;
	CRect rcLabel;
	int offset = pDC->GetTextExtent(_T(" "), 1 ).cx*2;

	rcCol.left = rcCol.right;
	rcCol.right += lvc.cx;

	// Draw the background if needed
	if( m_nHighlight == HIGHLIGHT_NORMAL )
		pDC->FillRect(rcCol, &CBrush(::GetSysColor(COLOR_WINDOW)));

	sLabel = GetItemText(nItem, nColumn);
	if (sLabel.GetLength() == 0)
		return;

	// Get the text justification
	UINT nJustify = DT_LEFT;
	switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
	{
	case LVCFMT_RIGHT:
		nJustify = DT_RIGHT;
		break;
	case LVCFMT_CENTER:
		nJustify = DT_CENTER;
		break;
	default:
		break;
	}

	rcLabel = rcCol;
	rcLabel.left += offset;
	rcLabel.right -= offset;

	pDC->DrawText(sLabel, -1, rcLabel, nJustify | DT_SINGLELINE |
				DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS);
}

void CListCtrlEx::RepaintSelectedItems()
{
	CRect rcBounds, rcLabel;

	// Invalidate focused item so it can repaint
	int nItem = GetNextItem(-1, LVNI_FOCUSED);

	if(nItem != -1)
	{
		GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
		GetItemRect(nItem, rcLabel, LVIR_LABEL);
		rcBounds.left = rcLabel.left;

		InvalidateRect(rcBounds, FALSE);
	}

	// Invalidate selected items depending on LVS_SHOWSELALWAYS
	if(!(GetStyle() & LVS_SHOWSELALWAYS))
	{
		for(nItem = GetNextItem(-1, LVNI_SELECTED);
			nItem != -1; nItem = GetNextItem(nItem, LVNI_SELECTED))
		{
			GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
			GetItemRect(nItem, rcLabel, LVIR_LABEL);
			rcBounds.left = rcLabel.left;

			InvalidateRect(rcBounds, FALSE);
		}
	}

	UpdateWindow();
}

void CListCtrlEx::OnPaint()
{
	// in full row select mode, we need to extend the clipping region
	// so we can paint a selection all the way to the right
	if (m_nHighlight == HIGHLIGHT_ROW &&
		(GetStyle() & LVS_TYPEMASK) == LVS_REPORT )
	{
		CRect rcBounds;
		GetItemRect(0, rcBounds, LVIR_BOUNDS);

		CRect rcClient;
		GetClientRect(&rcClient);
		if(rcBounds.right < rcClient.right)
		{
			CPaintDC dc(this);

			CRect rcClip;
			dc.GetClipBox(rcClip);

			rcClip.left = min(rcBounds.right-1, rcClip.left);
			rcClip.right = rcClient.right;

			InvalidateRect(rcClip, FALSE);
		}
	}

	CListCtrl::OnPaint();
}


void CListCtrlEx::OnKillFocus(CWnd* pNewWnd)
{
	CListCtrl::OnKillFocus(pNewWnd);

	if (m_nHighlight == HIGHLIGHT_ROW)
	{
		// check if we are losing focus to label edit box
		if(pNewWnd != NULL && pNewWnd->GetParent() == this)
			return;

		// repaint items that should change appearance
		if((GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
			RepaintSelectedItems();
	}
}

void CListCtrlEx::OnSetFocus(CWnd* pOldWnd)
{
	CListCtrl::OnSetFocus(pOldWnd);

	if (m_nHighlight == HIGHLIGHT_ROW)
	{
		// check if we are getting focus from label edit box
		if(pOldWnd!=NULL && pOldWnd->GetParent()==this)
			return;

		// repaint items that should change appearance
		if((GetStyle() & LVS_TYPEMASK)==LVS_REPORT)
			RepaintSelectedItems();
	}
	m_pEdit = NULL;
}

int CListCtrlEx::SetHighlightType(EHighlight hilite)
{
	int oldhilite = m_nHighlight;
	if( hilite <= HIGHLIGHT_ROW )
	{
		m_nHighlight = hilite;
		Invalidate();
	}
	return oldhilite;
}


/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit

CInPlaceEdit::CInPlaceEdit(int iItem, int iSubItem, CString sInitText)
{
	m_iItem = iItem;
	m_iSubItem = iSubItem;
	m_sInitText = sInitText;
	m_bESC = FALSE;
}

CInPlaceEdit::~CInPlaceEdit()
{
}


BEGIN_MESSAGE_MAP(CInPlaceEdit, CEdit)
	//{{AFX_MSG_MAP(CInPlaceEdit)
	ON_WM_NCDESTROY()
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit message handlers

void CInPlaceEdit::OnNcDestroy()
{
	CEdit::OnNcDestroy();

	delete this;
}

void CInPlaceEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if( nChar == VK_ESCAPE )
			m_bESC = TRUE;
		GetParent()->SetFocus();
		return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);

	// Resize edit control if needed

	// Get text extent
	CString str;
	GetWindowText( str );
	CWindowDC dc(this);
	CFont *pFont = GetParent()->GetFont();
	CFont *pFontDC = dc.SelectObject( pFont );
	CSize size = dc.GetTextExtent( str );
	dc.SelectObject( pFontDC );
	size.cx += 5;			   	// add some extra buffer

	// Get client rect
	CRect rect, parentrect;
	GetClientRect( &rect );
	GetParent()->GetClientRect( &parentrect );
	// Transform rect to parent coordinates
	ClientToScreen( &rect );
	GetParent()->ScreenToClient( &rect );

	// Check whether control needs to be resized
	// and whether there is space to grow
	if( size.cx > rect.Width() )
	{
		if( size.cx + rect.left < parentrect.right )
			rect.right = rect.left + size.cx;
		else
			rect.right = parentrect.right;
		MoveWindow( &rect );
	}
}

int CInPlaceEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);

	SetWindowText( m_sInitText );
	SetFocus();
	SetSel( 0, -1 );
	return 0;
}

void CInPlaceEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	CString str;
	GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;
	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = m_bESC ? LPTSTR((LPCTSTR)m_sInitText) : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();
	if (dispinfo.item.cchTextMax == 0) dispinfo.item.pszText = NULL;
	GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(),
					(LPARAM)&dispinfo );
	DestroyWindow();
}

BOOL CInPlaceEdit::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
	{
		if(pMsg->wParam == VK_RETURN
			|| pMsg->wParam == VK_DELETE
			|| pMsg->wParam == VK_ESCAPE
			|| GetKeyState( VK_CONTROL)	)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;				// DO NOT process further
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
// CInPlaceList

CInPlaceList::CInPlaceList(int iItem, int iSubItem, CStringList *plstItems, int nSel)
{
	m_iItem = iItem;
	m_iSubItem = iSubItem;

	m_lstItems.AddTail( plstItems );
	m_nSel = nSel;
	m_bESC = FALSE;
}

CInPlaceList::~CInPlaceList()
{
}


BEGIN_MESSAGE_MAP(CInPlaceList, CComboBox)
	//{{AFX_MSG_MAP(CInPlaceList)
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceList message handlers

void CInPlaceList::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if( nChar == VK_ESCAPE )
			m_bESC = TRUE;
		GetParent()->SetFocus();
		return;
	}

	CComboBox::OnChar(nChar, nRepCnt, nFlags);
}

int CInPlaceList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);
	for( POSITION pos = m_lstItems.GetHeadPosition(); pos != NULL; )
	{
		AddString( (LPCTSTR) (m_lstItems.GetNext( pos )) );
	}
	SetCurSel( m_nSel );
	SetFocus();
	return 0;
}

void CInPlaceList::OnKillFocus(CWnd* pNewWnd)
{
	CComboBox::OnKillFocus(pNewWnd);

	CString str;
	GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;
	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();
	GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(),
					(LPARAM)&dispinfo );

	PostMessage( WM_CLOSE );
}

void CInPlaceList::OnNcDestroy()
{
	CComboBox::OnNcDestroy();

	delete this;
}

void CInPlaceList::OnCloseup()
{
	GetParent()->SetFocus();
}

BOOL CInPlaceList::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
	{
		if(pMsg->wParam == VK_RETURN
			|| pMsg->wParam == VK_DELETE
			|| pMsg->wParam == VK_ESCAPE
			|| GetKeyState( VK_CONTROL)	)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;				// DO NOT process further
		}
	}

	return CComboBox::PreTranslateMessage(pMsg);
}


/////////////////////////////////////////////////////////////////////////////
// CInPlaceFontNameCombo

CInPlaceFontNameCombo::CInPlaceFontNameCombo(
	int item,
	int subItem,
	const char *fontName,
	int which)	// EBATECH(CNBR) Add which args.
	{
	m_item		= item;
	m_subItem	= subItem;
	m_fontName	= fontName;
	m_ESC		= false;
	m_which		= which;
	}

CInPlaceFontNameCombo::~CInPlaceFontNameCombo()
	{
	}


BEGIN_MESSAGE_MAP(CInPlaceFontNameCombo, CIcadFontComboBox)
	//{{AFX_MSG_MAP(CInPlaceFontNameCombo)
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceFontNameCombo message handlers

void CInPlaceFontNameCombo::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
		{
		if( nChar == VK_ESCAPE )
			m_ESC = TRUE;

		GetParent()->SetFocus();
		return;
		}

	CComboBox::OnChar(nChar, nRepCnt, nFlags);
	}

int CInPlaceFontNameCombo::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	// set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);
//	SetCurSel( m_nSel );
//	FillFontComboBox(GET_FONT_TTF + GET_FONT_SHX + GET_FONT_SHP);
	FillFontComboBox(m_which); // EBATECH(CNBR) Filter come from member
	if (FindStringExact(-1, m_fontName) == -1)
		AddString(m_fontName);
	this->SetCurrentFont(m_fontName);
	SetFocus();

	return 0;
	}

void CInPlaceFontNameCombo::OnKillFocus(CWnd* pNewWnd)
	{
	CComboBox::OnKillFocus(pNewWnd);

	CString str;
	GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;
	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_item;
	dispinfo.item.iSubItem = m_subItem;
	dispinfo.item.pszText = m_ESC ? NULL : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();
	GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(),
					(LPARAM)&dispinfo );

	PostMessage( WM_CLOSE );
	}

void CInPlaceFontNameCombo::OnNcDestroy()
	{
	CComboBox::OnNcDestroy();

	delete this;
	}

void CInPlaceFontNameCombo::OnCloseup()
	{
	GetParent()->SetFocus();
	}

BOOL CInPlaceFontNameCombo::PreTranslateMessage(MSG* pMsg)
	{
	if( pMsg->message == WM_KEYDOWN )
		{
		if(pMsg->wParam == VK_RETURN
			|| pMsg->wParam == VK_DELETE
			|| pMsg->wParam == VK_ESCAPE
			|| GetKeyState( VK_CONTROL)	)
			{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;				// DO NOT process further
			}
		}

	return CComboBox::PreTranslateMessage(pMsg);
	}


/////////////////////////////////////////////////////////////////////////////
// CInPlaceFontStyleCombo

CInPlaceFontStyleCombo::CInPlaceFontStyleCombo(
	int item,
	int subItem,
	const char *fontName)
	//, const char *fontStyle) EBATECH(CNBR)
	{
	m_fontName	= fontName;
	//m_fontStyle	= fontStyle;
	m_item		= item;
	m_subItem	= subItem;
	m_ESC		= false;
	}

CInPlaceFontStyleCombo::~CInPlaceFontStyleCombo()
{
	if (m_hWnd)
	{
		void *ptr = NULL;
		int idx = GetCurSel();
		if( idx != CB_ERR )
		{
			ptr = GetItemDataPtr( idx );
			if( ptr && !IsBadReadPtr(ptr, sizeof(StyleItem)) )
			{
				StyleItem *pItem = (StyleItem*) ptr;
				((IEXP_CdetailView*) GetParent()->GetParent())->m_fullName = pItem->fullName;
			}
			DeleteStyleItems();
		}
	}
}


BEGIN_MESSAGE_MAP(CInPlaceFontStyleCombo, CIcadFontStyleComboBox)
	//{{AFX_MSG_MAP(CInPlaceFontStyleCombo)
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceFontStyleCombo message handlers

void CInPlaceFontStyleCombo::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
		{
		if( nChar == VK_ESCAPE )
			m_ESC = TRUE;

		GetParent()->SetFocus();
		return;
		}

	CComboBox::OnChar(nChar, nRepCnt, nFlags);
	}

int CInPlaceFontStyleCombo::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	// we had a problem with empty font names;
	// though they should never occur, we'll trap
	// for them
	CString tmpFontName = m_fontName;
	tmpFontName.TrimLeft();
	if (tmpFontName.IsEmpty())
		return -1;

	// set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);
//	SetCurSel( m_nSel );
	FillTrueTypeStyles(m_fontName, 0L);
	SetFocus();

	return 0;
	}

void CInPlaceFontStyleCombo::OnKillFocus(CWnd* pNewWnd)
	{
	CComboBox::OnKillFocus(pNewWnd);

	CString str;
	GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;
	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_item;
	dispinfo.item.iSubItem = m_subItem;
	dispinfo.item.pszText = m_ESC ? NULL : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();
	GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(),
					(LPARAM)&dispinfo );

	PostMessage( WM_CLOSE );
	}

void CInPlaceFontStyleCombo::OnNcDestroy()
	{
	CComboBox::OnNcDestroy();

	delete this;
	}

void CInPlaceFontStyleCombo::OnCloseup()
	{
	GetParent()->SetFocus();
	}

BOOL CInPlaceFontStyleCombo::PreTranslateMessage(MSG* pMsg)
	{
	if( pMsg->message == WM_KEYDOWN )
		{
		if(pMsg->wParam == VK_RETURN
			|| pMsg->wParam == VK_DELETE
			|| pMsg->wParam == VK_ESCAPE
			|| GetKeyState( VK_CONTROL)	)
			{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;				// DO NOT process further
			}
		}

	return CComboBox::PreTranslateMessage(pMsg);
	}

// EBATECH(CNBR) -[
/////////////////////////////////////////////////////////////////////////////
// CInPlaceFontLangCombo

CInPlaceFontLangCombo::CInPlaceFontLangCombo(
	int item,
	int subItem,
	const char *fontName)
	{
	m_fontName	= fontName;
	m_item		= item;
	m_subItem	= subItem;
	m_ESC		= false;
	}

CInPlaceFontLangCombo::~CInPlaceFontLangCombo()
{
	if (m_hWnd)
	{
		int charSet = 0;
		int idx = GetCurSel();
		if( idx != CB_ERR )
		{
			charSet = GetItemData( idx );
			//To Do: Modify Stylenumber and Langname(if exist)
			//((IEXP_CdetailView*) GetParent()->GetParent())->m_fullName = pItem->fullName;
		}
	}
	// EBATECH(CNBR) CIcadFontLangComboBox has ExtraData not ExtraPointer.
	// So, Nothing to do here.
}


BEGIN_MESSAGE_MAP(CInPlaceFontLangCombo, CIcadFontLangComboBox)
	//{{AFX_MSG_MAP(CInPlaceFontLangCombo)
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceFontLangCombo message handlers

void CInPlaceFontLangCombo::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
		{
		if( nChar == VK_ESCAPE )
			m_ESC = TRUE;

		GetParent()->SetFocus();
		return;
		}

	CComboBox::OnChar(nChar, nRepCnt, nFlags);
	}

int CInPlaceFontLangCombo::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	// we had a problem with empty font names;
	// though they should never occur, we'll trap
	// for them
	CString tmpFontName = m_fontName;
	tmpFontName.TrimLeft();
	if (tmpFontName.IsEmpty())
		return -1;

	// set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);
//	SetCurSel( m_nSel );
	FillTrueTypeLangs(m_fontName, 0L);
	SetFocus();

	return 0;
	}

void CInPlaceFontLangCombo::OnKillFocus(CWnd* pNewWnd)
	{
	CComboBox::OnKillFocus(pNewWnd);

	CString str;
	GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;
	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_item;
	dispinfo.item.iSubItem = m_subItem;
	dispinfo.item.pszText = m_ESC ? NULL : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();
	GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(),
					(LPARAM)&dispinfo );

	PostMessage( WM_CLOSE );
	}

void CInPlaceFontLangCombo::OnNcDestroy()
	{
	CComboBox::OnNcDestroy();

	delete this;
	}

void CInPlaceFontLangCombo::OnCloseup()
	{
	GetParent()->SetFocus();
	}

BOOL CInPlaceFontLangCombo::PreTranslateMessage(MSG* pMsg)
	{
	if( pMsg->message == WM_KEYDOWN )
		{
		if(pMsg->wParam == VK_RETURN
			|| pMsg->wParam == VK_DELETE
			|| pMsg->wParam == VK_ESCAPE
			|| GetKeyState( VK_CONTROL)	)
			{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;				// DO NOT process further
			}
		}

	return CComboBox::PreTranslateMessage(pMsg);
	}

// EBATECH(CNBR) ]-

int GetComboBoxLongestStrWidth(
	CComboBox *comboBox)
	{
	// find the longest string in the combo box
	CString str;
	CSize	size;
	int 	width = 0;
	int		numItems;
	CDC 	*dc = comboBox->GetDC();

	numItems = comboBox->GetCount();
	for (int i = 0; i < numItems; i++)
		{
		comboBox->GetLBText(i, str);
		size = dc->GetTextExtent(str);

		if (size.cx > width)
			width = size.cx;
		}

	comboBox->ReleaseDC(dc);

	// adjust the width for the vertical scroll bar
	// and for the left and right borders
	width += ::GetSystemMetrics(SM_CXVSCROLL) + (2 * ::GetSystemMetrics(SM_CXEDGE));

	return width;
	}

/////////////////////////////////////////////////////////////////////////////
// CInPlaceLWCombo

CInPlaceLWCombo::CInPlaceLWCombo(int item, int subItem, int lwunits, int celweight, int bitcode )
	{
	m_Item		= item;
	m_subItem	= subItem;
	m_lwunits	= lwunits;
	m_celweight	= celweight;
	m_bitcode	= bitcode;
	m_ESC		= false;
	}

CInPlaceLWCombo::~CInPlaceLWCombo()
{
}


BEGIN_MESSAGE_MAP(CInPlaceLWCombo, CIcadLWComboBox)
	//{{AFX_MSG_MAP(CInPlaceLWCombo)
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceLWCombo message handlers

void CInPlaceLWCombo::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
		{
		if( nChar == VK_ESCAPE )
			m_ESC = TRUE;

		GetParent()->SetFocus();
		return;
		}

	CComboBox::OnChar(nChar, nRepCnt, nFlags);
	}

int CInPlaceLWCombo::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	FillLWeight(m_lwunits, m_celweight, m_bitcode);
	SetFocus();

	return 0;
	}

void CInPlaceLWCombo::OnKillFocus(CWnd* pNewWnd)
	{
	CComboBox::OnKillFocus(pNewWnd);

	CString str;
	GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;
	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_Item;
	dispinfo.item.iSubItem = m_subItem;
	dispinfo.item.pszText = m_ESC ? NULL : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();
	GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(),
					(LPARAM)&dispinfo );

	PostMessage( WM_CLOSE );
	}

void CInPlaceLWCombo::OnNcDestroy()
	{
	CComboBox::OnNcDestroy();

	delete this;
	}

void CInPlaceLWCombo::OnCloseup()
	{
	GetParent()->SetFocus();
	}

BOOL CInPlaceLWCombo::PreTranslateMessage(MSG* pMsg)
	{
	if( pMsg->message == WM_KEYDOWN )
		{
		if(pMsg->wParam == VK_RETURN
			|| pMsg->wParam == VK_DELETE
			|| pMsg->wParam == VK_ESCAPE
			|| GetKeyState( VK_CONTROL)	)
			{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;				// DO NOT process further
			}
		}

	return CComboBox::PreTranslateMessage(pMsg);
	}

// EBATECH(CNBR) ]-
