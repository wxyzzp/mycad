/** FILENAME:     DCLDialogList.cpp- Contains implementation of class CDCLDialogList.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract : Class represents the ListView to show all dialog names in the dcl file
*
*
*
*Audit Log 
*
*
*Audit Trail
*
*
*/


#include "stdafx.h"
#include "dcl.h"
#include "dcldialoglist.h"
#include "dclview.h"
#include "sds.h"
#include "errormessage.h"
class CDCLDoc;

#define DLGLIST_COLUMN _T("Dialogs")
#define FILE_DISPLAY _T("FileName")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CDCLDialogList, CListView)
/************************************************************************************
/* + CDCLDialogList::CDCLDialogList
*    Constructor
*
*
*
*
*/
CDCLDialogList::CDCLDialogList()
	{
	}
/************************************************************************************
/* + CDCLDialogList::~CDCLDialogList - 
*    Destructor
*
*
*
*
*/

CDCLDialogList::~CDCLDialogList()
	{
	}


BEGIN_MESSAGE_MAP(CDCLDialogList, CListView)
	//{{AFX_MSG_MAP(CDCLDialogList)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_RETURN, OnReturn)
	ON_WM_CHAR()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDMN_SHOW_DIALOG, OnShowDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDCLDialogList::OnDraw(CDC* pDC)
	{
	CDocument* pDoc = GetDocument();
	}

/////////////////////////////////////////////////////////////////////////////
// CDCLDialogList diagnostics

#ifdef _DEBUG
void CDCLDialogList::AssertValid() const
	{
	CListView::AssertValid();
	}

void CDCLDialogList::Dump(CDumpContext& dc) const
	{
	CListView::Dump(dc);
	}
#endif //_DEBUG

/************************************************************************************
/* + int CDCLDialogList::OnCreate(LPCREATESTRUCT lpCreateStruct - 
*    After creating the view,set the documents m_pDialogList member
*
*
*
*
*/

int CDCLDialogList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
	{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CDCLDoc* pDoc = (CDCLDoc*)GetDocument();
	ASSERT( pDoc );
	pDoc->m_pDialogList = this;
	return 0;
	}
/************************************************************************************
/* + CDCLDoc* CDCLDialogList::GetDocument() 
*   Get a pointer to the document object
*
*
*
*
*/

CDCLDoc* CDCLDialogList::GetDocument()
	{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDCLDoc)));
	return (CDCLDoc*)m_pDocument;
	}
/************************************************************************************
/* + CDCLDialogList::PreCreateWindow(CREATESTRUCT& cs)
*    Overloaded  to set the listview style
*
*
*
*
*/

BOOL CDCLDialogList::PreCreateWindow(CREATESTRUCT& cs) 
	{
	
	cs.style |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL ;	
	return CListView::PreCreateWindow(cs);
	}
/************************************************************************************
/* +  CDCLDialogList::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult )
*      On DblClicking on a dialog name 
*         1. Set the documents m_strSelectedDlg member
*         2. Delete all the items from the TreeView and Redraw
*         3. Show the new dialog tree
*
*/

void CDCLDialogList::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	ASSERT( pNMHDR );
	TreeChanged(pNMHDR);
	*pResult = 0;
	}
/************************************************************************************
/* +  CDCLDialogList::OnInitialUpdate
*     Add the only column "dialogs" to the ListControl
*
*
*
*
*/

void CDCLDialogList::OnInitialUpdate() 
	{
	CListView::OnInitialUpdate();
	CString strRootdlg = DLGLIST_COLUMN;
	CString strFileDisplay = FILE_DISPLAY;
	int iColumn = 0;
	RECT cRect;
	GetClientRect(&cRect);
	
	CListCtrl& listCtl = GetListCtrl();
	LV_COLUMN lvc;
	for ( int nCounter = 0; nCounter < 1; nCounter++ )
		{
		lvc.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvc.iSubItem=0;
		if ( nCounter == 0 )
			lvc.pszText = strRootdlg.GetBuffer(strRootdlg.GetLength());
		else
			lvc.pszText = strFileDisplay.GetBuffer(strFileDisplay.GetLength());
		lvc.cx = (cRect.right - cRect.left);
		lvc.fmt = LVCFMT_LEFT;
		iColumn = nCounter;
		listCtl.InsertColumn(iColumn,&lvc);
		}
	}



void CDCLDialogList::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
	}
/************************************************************************************
/* + void CDCLDialogList::OnReturn(NMHDR* pNMHDR, LRESULT* pResult - 
*	Show the tree corresponding to the dialog selected.Call TreeChanged
*	and pass the NMHDR as it is.
*
*
*
*/

void CDCLDialogList::OnReturn(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	// TODO: Add your control notification handler code here
	TreeChanged(pNMHDR);
	*pResult = 0;
	}

/************************************************************************************
/* + void CDCLDialogList::TreeChanged(NMHDR* pNMHDR)
*	This function actually shows the tree corresponding to the selected dialog.
*	
*
*
*
*/

void CDCLDialogList::TreeChanged(NMHDR* pNMHDR)
	{
	ASSERT( pNMHDR );
	int nRes = 0;
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if ( pNMListView->iItem != -1 )
		{
		CListCtrl& listCtl = GetListCtrl();
		GetDocument()->m_strSelectedDlg = listCtl.GetItemText(pNMListView->iItem,pNMListView->iSubItem);
		if ( GetDocument()->m_strSelectedDlg.IsEmpty() )
			{
			POSITION pos = GetDocument()->m_pDialogList->GetListCtrl().GetFirstSelectedItemPosition();
			ASSERT( pos );
			GetDocument()->m_strSelectedDlg = GetDocument()->m_strdlgArray[(int)pos - 1];
			}
		if ( GetDocument()->m_pTreeView->GetTreeCtrl().DeleteAllItems())
			{
			GetDocument()->m_pTreeView->Invalidate(TRUE);
			nRes = GetDocument()->ShowTree( GetDocument()->m_strSelectedDlg );
			if ( nRes == -1)
				{
				ErrorMessage errMessageBox;
				errMessageBox.Message(ID_DCL_SHOW_TREE_FAILED);
				}
			}
		}


	}
/************************************************************************************
/* + void CDCLDialogList::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
*	Check if the key pressed is the TAB key , if yes switch to the 
*	TreeView.
*
*
*
*/

void CDCLDialogList::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
	{
	// TODO: Add your message handler code here and/or call default
	CDCLDoc* pDoc = (CDCLDoc*)GetDocument();
	BOOL bRes = FALSE;
	ASSERT( pDoc );
	if ( pDoc )
		{
			if ( nChar == VK_TAB ) //TAB KEY
				{
				pDoc->m_pTreeView->GetTreeCtrl().SetFocus();
				bRes = pDoc->m_pTreeView->GetTreeCtrl().SelectItem(pDoc->m_hRoot);
				ASSERT( bRes );
				}
		}
	CListView::OnChar(nChar, nRepCnt, nFlags);
	}

/************************************************************************************
/* + void CDCLDialogList::OnContextMenu(CWnd* pWnd, CPoint point - 
*	On Right clicking show the ShowDialog menu and on the user selecting ShowDialog
*	show the dialog.
*
*
*
*/

void CDCLDialogList::OnContextMenu(CWnd* pWnd, CPoint point) 
	{
	// TODO: Add your message handler code here
	CDCLDoc* pDoc = (CDCLDoc*)GetDocument();
	ASSERT( pDoc );
	CListCtrl& ctlList = GetListCtrl();
	int nRes;
	UINT flags;
	ScreenToClient(&point);
	if ( (nRes = ctlList.HitTest(point,&flags)) != -1 &&
			flags & LVHT_ONITEM )
		{
		ClientToScreen(&point);
		CMenu menu;
		VERIFY(menu.LoadMenu(IDR_MENU1));
		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		CWnd* pWndPopupOwner = this;
		while (pWndPopupOwner->GetStyle() & WS_CHILD)
			pWndPopupOwner = pWndPopupOwner->GetParent();
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		pWndPopupOwner);
		}
	}
/************************************************************************************
/* + void CDCLDialogList::OnShowDialog( )
*	Function to actually start actually showing the dialog.
*	Get the selected dialog name in the listctrl and call documents
*	ShowDialog(strSelectedDlg) with the selected dialog as the parameter.
*
*
*/

void CDCLDialogList::OnShowDialog()								// Function to actually start actually showing the dialog.
	{
	// TODO: Add your command handler code here
	CListCtrl& ctlList = GetListCtrl();
	POSITION pos = ctlList.GetFirstSelectedItemPosition();
	ASSERT( pos );
	CDCLDoc* pDoc = (CDCLDoc*)GetDocument();
	ASSERT( pDoc );
	pDoc->m_strSelectedDlg = pDoc->m_strdlgArray[(int)pos - 1];
	int nRes = pDoc->ShowDialog(pDoc->m_strSelectedDlg);
	// if the dialog size exceeds the desktop window just exit.
	// In the actual dll we need not exit.
	if ( nRes == SDS_RTERROR )
		pDoc->Init();
	}
