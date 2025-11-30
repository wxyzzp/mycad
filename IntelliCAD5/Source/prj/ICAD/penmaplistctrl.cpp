/* D:\ICADDEV\PRJ\ICAD\PENMAPLISTCTRL.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// PenMapListCtrl.cpp : implementation file
//

#include "icad.h"
#include "IcadApi.h"
#include "PenMapListCtrl.h"
#include "PrintPenMapTab.h"

/////////////////////////////////////////////////////////////////////////////
// PenMapListCtrl

PenMapListCtrl::PenMapListCtrl(PrintPenMapTab *mapTab) : m_mapTab(mapTab)
{
}

PenMapListCtrl::~PenMapListCtrl()
{
}

BOOL PenMapListCtrl::ValidateData ()
{	
	CString width =  GetItemText(editRow, editColumn);

	if (width.IsEmpty())
		return FALSE;

	double lineWidthNum = atof(width);

	if ((!lineWidthNum) && (width[0] != '0'))
	{
		MessageBox(ResourceString(IDC_ICADPRINT_INVALIDWIDTH, "Please enter a numeric value."),
				   ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ), MB_OK | MB_ICONEXCLAMATION);

		SetItemText(editRow, editColumn, 0);


		return FALSE;
	}

	return TRUE;
}

BOOL PenMapListCtrl::IsSubclassedControl( )
{
	return TRUE;
}

BOOL PenMapListCtrl::PreCreateWindow(CREATESTRUCT &cs)
{
	cs.style &= ~LVS_TYPEMASK;
	cs.style |= LVS_SHOWSELALWAYS | LVS_REPORT | LVS_OWNERDRAWFIXED;
	cs.style |= LVS_EDITLABELS;

	return CListCtrlEx::PreCreateWindow(cs);
}

//Redefined
void PenMapListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
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
			rcHighlight.right = rcLabel.left + nExt + (offset * 5);		
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

	LV_COLUMN lvc;	
	lvc.mask = LVCF_FMT | LVCF_WIDTH;
	lvc.cx = 0;

	// Draw item label - Column 0
	
	CRect rcFirstCol = rcCol;
	rcFirstCol.right = rcFirstCol.left;
	// offset here is *2 what it is in color map so factor is 4 here and 8 in subitem
	rcFirstCol.right += offset*4;
	lvc.cx -= offset*4;

	GetColumn(0, &lvc);
	MapColor(nItem, 0, lvc, rcFirstCol, pDC);

	rcLabel.left += rcFirstCol.right + offset/2;
	rcLabel.right += rcFirstCol.right;
	rcLabel.right -= offset;

	pDC->DrawText(sLabel,-1,rcLabel,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP 
				| DT_VCENTER | DT_END_ELLIPSIS);	


	// Draw labels for remaining columns
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

void PenMapListCtrl::MapColor(int nItem, int nColumn, LV_COLUMN& lvc, CRect& rcCol,CDC *pDC)
{
	int offset = pDC->GetTextExtent(_T(" "), 1 ).cx;

	CRect rcColorBox = rcCol;
	rcColorBox.left += offset*2;
	rcColorBox.top += offset;
	rcColorBox.bottom -= offset;

	int Color;
	CString colorStr = GetItemText(nItem, nColumn);

	if (colorStr == ResourceString(IDC_COLOR_BLACK, "black"))
		Color = 256;

	else
		Color = ic_colornum(colorStr);

	//Draw the Filled rectangle
	CPalette *savpal = pDC->SelectPalette(SDS_CMainWindow->m_pPalette,TRUE);

	CBrush  brush;
	if(Color == 256)
		brush.CreateSolidBrush(RGB(0,0,0));
	else
		brush.CreateSolidBrush(SDS_BrushColorFromACADColor(Color));
	pDC->FillRect(&rcColorBox, &brush);
	brush.DeleteObject();
	brush.CreateSolidBrush(RGB(0,0,0));
	pDC->FrameRect(&rcColorBox, &brush);
	brush.DeleteObject();

	pDC->SelectPalette(savpal,TRUE);
}

void PenMapListCtrl::DrawSubItem(int nItem, int nColumn, LV_COLUMN& lvc, CRect& rcCol, CDC *pDC)
{	
	if (nColumn == 1) // Map color
	{
		int offset = pDC->GetTextExtent(_T(" "), 1 ).cx;

		rcCol.left = rcCol.right;
		rcCol.right += offset*8;
		lvc.cx -= offset*8;

		MapColor(nItem, nColumn, lvc, rcCol, pDC);

		//Draw the Text
		CListCtrlEx::DrawSubItem (nItem, nColumn, lvc, rcCol, pDC);

	}
	else
		CListCtrlEx::DrawSubItem(nItem, nColumn, lvc, rcCol, pDC);
}

CEdit* PenMapListCtrl::EditSubLabel(int row, int column)
{
	editColumn = column;
	editRow = row;

	return CListCtrlEx::EditSubLabel(row, column);
}

CEdit* PenMapListCtrl::EditInvalidSubLabel()
{
	SetItemState(editRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	return EditSubLabel(editRow, editColumn);
}

// sets the precision for a float value
void PenMapListCtrl::SetEditPrecision(int precision)
{

	CString width =  GetItemText(editRow, editColumn);

	if (width.IsEmpty())
		return;

	double lineWidthNum = atof(width);

	// set precision for edited field
	TCHAR widthStr[20];
	sprintf(widthStr,"%.*f", precision, lineWidthNum);
	width = widthStr;


	SetItemText(editRow, editColumn, width);
}

/********************************************************************************
 * Author:	Sachin Dange.
 *
 * Purpose:	Over-ridden to allocate memory for the User Defined data associated 
 *			with the List item.
 *
 * Returns:	int
 ********************************************************************************/
int PenMapListCtrl::InsertItem(int nItem, LPCTSTR lpszItem, int nImage)
{
	// First, call the Base class version, to insert the item.
	int index = CListCtrlEx::InsertItem(nItem, lpszItem, nImage);
	
	if (index >= 0)
	{
		// Now allocate the user-defined data and assign it to this item.
		SListItemData* pItemData = new SListItemData;
		pItemData->ActualWidth = 0.0;
		
		SetItemData(nItem, (DWORD)pItemData);
	}

	return index;
}

/********************************************************************************
 * Author:	Sachin Dange.
 *
 * Purpose:	Gets the Actual Width associated with the List item, pointed to by 
 *			the index nItem.
 *
 * Returns:	double.
 ********************************************************************************/
double PenMapListCtrl::GetActualWidth(int nItem)
{
	SListItemData *pItemData = NULL;
	pItemData = (SListItemData *)GetItemData(nItem);
	ASSERT (pItemData != NULL);

	return pItemData->ActualWidth;
}

/********************************************************************************
 * Author:	Sachin Dange.
 *
 * Purpose:	Sets the Actual Width to nWidth associated with the List item, pointed  
 *			to by the index nItem.
 *
 * Returns:	void.
 ********************************************************************************/
void PenMapListCtrl::SetActualWidth(int nItem, double nWidth)
{
	SListItemData *pItemData = NULL;
	pItemData = (SListItemData *)GetItemData(nItem);
	ASSERT (pItemData != NULL);

	pItemData->ActualWidth = nWidth;
}

/********************************************************************************
 * Author:	Sachin Dange.
 *
 * Purpose:	Over-ridden to free memory for the User Defined data associated 
 *			with all the the List items in order to prevent memory leaks.
 *
 * Returns:	BOOL
 ********************************************************************************/
BOOL PenMapListCtrl::DeleteAllItems()
{
	// First do the cleanup.
	int nCountOfItems = GetItemCount();
	if (nCountOfItems)
	{
		// Release the memory occupied by each ItemData for each item. 
		for (int listIndex = 0; listIndex < nCountOfItems; listIndex++)
		{
			SListItemData *pItemData;

			pItemData = (SListItemData *)GetItemData(listIndex);
			if (pItemData != NULL)
			{
				delete pItemData;
				pItemData = NULL;
			}
		}
	}

	// Now Call the Base class version.
	return CListCtrlEx::DeleteAllItems();
}


BEGIN_MESSAGE_MAP(PenMapListCtrl, CListCtrlEx)
	//{{AFX_MSG_MAP(PenMapListCtrl)
	ON_WM_KILLFOCUS()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void PenMapListCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	//	CListCtrlEx::OnKillFocus(pNewWnd);

	// override parent function and do nothing - want items to stay highlighted	
}

/********************************************************************************
 * Author:	Sachin Dange.
 *
 * Purpose:	Over-ridden to free memory for the User Defined data associated 
 *			with all the the List items in order to prevent memory leaks,
 *			just before this List Control is destroyed.
 *
 * Returns:	BOOL
 ********************************************************************************/
void PenMapListCtrl::OnDestroy()
{
	// Free the memory occupied by the User-defined data for each of the items
	// in the Pen Map List Control.
	DeleteAllItems();
	
	// Call the Base class version.
	CListCtrlEx::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// PenMapListCtrl message handlers

