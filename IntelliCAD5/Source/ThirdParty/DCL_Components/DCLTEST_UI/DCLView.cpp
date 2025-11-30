/** FILENAME:     DCLView.cpp- Contains implementation of class CDCLView.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract : Implementation of the TreeView
*            Take care of a single node .
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
#include "dcldoc.h"
#include "dcldialoglist.h"
#include "dclview.h"
#include "treenode.h"
#include "tile.h"
#include "tileattributes.h"
#include "ctrllistattributes.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CDCLView, CTreeView)

BEGIN_MESSAGE_MAP(CDCLView, CTreeView)
	//{{AFX_MSG_MAP(CDCLView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(NM_RETURN, OnReturn)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/************************************************************************************
/* + CDCLView::CDCLView 
*   Constructor
*
*
*
*
*/
CDCLView::CDCLView()
	{
	// TODO: add construction code here
	}



/************************************************************************************
/* + CDCLView::~CDCLView
*    Destructor
*
*
*
*
*/
CDCLView::~CDCLView()
	{

	}



BOOL CDCLView::PreCreateWindow(CREATESTRUCT& cs)
	{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return CTreeView::PreCreateWindow(cs);
	}	


void CDCLView::OnDraw(CDC* pDC)
	{
	CDCLDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	}

void CDCLView::OnInitialUpdate()
	{
	CTreeView::OnInitialUpdate();
	// TODO: You may populate your TreeView with items by directly accessing
	//  its tree control through a call to GetTreeCtrl().
	}

#ifdef _DEBUG
void CDCLView::AssertValid() const
	{
	CTreeView::AssertValid();
	}

void CDCLView::Dump(CDumpContext& dc) const
	{
	CTreeView::Dump(dc);
	}

CDCLDoc* CDCLView::GetDocument() // non-debug version is inline
	{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDCLDoc)));
	return (CDCLDoc*)m_pDocument;
	}
#endif //_DEBUG


/************************************************************************************
/* + CDCLView::OnCreate(LPCREATESTRUCT lpCreateStruct )
*    Set the proper styles and set the documents m_pTreeView member
*
*
*
*
*/
int CDCLView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
	{
	lpCreateStruct->style |= TVS_HASLINES | TVS_HASBUTTONS | TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS | TVS_SINGLEEXPAND ;
	if ( CTreeView::OnCreate(lpCreateStruct) == -1 )
		return -1;
		
	CDCLDoc* pDoc = (CDCLDoc*)GetDocument();
	ASSERT( pDoc );
	pDoc->m_pTreeView = this;
	return 0;
	}

/************************************************************************************
/* + HTREEITEM CDCLView::InsertItem(CString & strItem)
*    // Insert a string item in the treeview
*
*
*
*
*/

HTREEITEM CDCLView::InsertItem(                  // Insert an item in the treeview
							   CString & strItem // i : text to be inserted
							   )
	{
	CString strInsert = strItem;
	TVINSERTSTRUCT insert;
	insert.item.mask = TVIF_TEXT | TVIF_CHILDREN | TVIF_PARAM;
	insert.item.pszText = (TCHAR*)strInsert.GetBuffer(strInsert.GetLength());
	insert.item.cchTextMax = strInsert.GetLength();
	insert.item.cChildren = 1;
	CTreeCtrl& treeCtl = GetTreeCtrl();
	return treeCtl.InsertItem(&insert);
	}


/************************************************************************************
/* + CDCLView::Insert(CString & strItem, HTREEITEM hParent, int iChildre - 
*    Insert a treeview item but this doesn't set the lParam
*
*
*
*
*/

HTREEITEM CDCLView::Insert(						// Insert an item in the treeview
						   CString & strItem,   // i : String to insert
						   HTREEITEM hParent,   // i : parent
						   int iChildren        // i : Children
						   )
	{
	CString strInsert = strItem;
	TV_INSERTSTRUCT insert;
	insert.hParent = hParent;
	insert.item.mask = TVIF_TEXT | TVIF_CHILDREN ;
	insert.item.pszText = (TCHAR * )strInsert.GetBuffer(strInsert.GetLength());
	insert.item.cchTextMax = strInsert.GetLength();
	if ( iChildren > 0 )
		insert.item.cChildren = 1;
	else 
		insert.item.cChildren = 0;

	CTreeCtrl& treeCtl = GetTreeCtrl();
	return treeCtl.InsertItem(&insert);
	}

/************************************************************************************
/* + CDCLView::SetRoot(CString & strItem, int iChildren) 
*    Set the root item
*
*
*
*
*/

HTREEITEM CDCLView::SetRoot(                    // set the root item
							CString & strItem,  // i : text of the root
							int iChildren       // i : Children
							)
	{
	CString strInsert = strItem;
	TVINSERTSTRUCT insert;
	insert.hParent = TVI_ROOT;
	insert.item.mask = TVIF_TEXT | TVIF_CHILDREN | TVIF_PARAM;
	insert.item.pszText = (TCHAR*)strInsert.GetBuffer(strInsert.GetLength());
	insert.item.cchTextMax = strInsert.GetLength();
	ASSERT( GetDocument()->m_pTreeRootNode );
	insert.item.lParam = (long)GetDocument()->m_pTreeRootNode;
	if ( iChildren > 0 )
		insert.item.cChildren = 1;
	else 
		insert.item.cChildren = 0;
	CTreeCtrl& treeCtl = GetTreeCtrl();
	return treeCtl.InsertItem(&insert);
	}


/************************************************************************************
/* + CDCLView::Insert(CString& strItem, HTREEITEM hParent, int iChildren, TreeNode* pTreeNode) 
*    Insert a treeview item.Use this b'cos this sets the lParam
*     
*
*
*
*/

HTREEITEM CDCLView::Insert(						// Insert an item in the treeview
						   CString& strItem,	// i : Text to insert 
						   HTREEITEM hParent,	// i : parent item
						   int iChildren,		// i : Children
						   TreeNode* pTreeNode	// i : a pointer to the Treenode this viewnode refers to.This is set as LPARAM
						   )
	{
	ASSERT( pTreeNode );
	CString strInsert = strItem;
	TV_INSERTSTRUCT insert;
	insert.hParent = hParent;
	insert.item.mask = TVIF_TEXT | TVIF_CHILDREN | TVIF_PARAM ;
	insert.item.pszText = (TCHAR* )strInsert.GetBuffer(strInsert.GetLength());
	insert.item.cchTextMax = strInsert.GetLength();
	insert.item.lParam = (long)pTreeNode;
	if ( iChildren > 0 )
		insert.item.cChildren = 1;
	else 
		insert.item.cChildren = 0;
	CTreeCtrl& treeCtl = GetTreeCtrl();
	return treeCtl.InsertItem(&insert);
	}

/************************************************************************************
/* + CDCLView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult )
*   To show all the attributes of the node when the user doubleclicks on a treeview item
*   First clear the documents m_MapName2Value,
*   Fill all the attributes of the current node in the map,
*   Use this map to initialise the dialog
*
*/

void CDCLView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	ShowAttribDialog(pNMHDR);
	*pResult = 0;
	}



/************************************************************************************
/* + void CDCLView::OnReturn(NMHDR* pNMHDR, LRESULT* pResult - 
*  When te user hits enter key show the attribute dialog
*
*
*
*
*/

void CDCLView::OnReturn(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	ShowAttribDialog(pNMHDR);
	*pResult = 0;
	}


/************************************************************************************
/* + void CDCLView::ShowAttribDialog(NMHDR * pNMHDR) 
*   This function actually shows the attrib dialog
*   It is called from OnReturn() and OnDblClk().
*
*
*
*/

void CDCLView::ShowAttribDialog(NMHDR * pNMHDR)
	{
	//first clear the documents attributemap
	CDCLDoc* pDoc = (CDCLDoc*)GetDocument();
	ASSERT( pDoc );
	if ( pDoc )
		pDoc->m_MapName2Value.RemoveAll();
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	TreeNode* pTreeNode;
	HTREEITEM hTreeItem;
	LPNMHDR lpnmh = (LPNMHDR) pNMHDR;
	NM_TREEVIEW* pNMListView = (NM_TREEVIEW*)pNMHDR;
	CTreeCtrl& ctlTree = (CTreeCtrl&) GetTreeCtrl();    
	hTreeItem = ctlTree.GetSelectedItem();
	ASSERT( hTreeItem );		
	CString text = ctlTree.GetItemText(hTreeItem);
	if ( hTreeItem  != NULL )
		{
		DWORD param = ctlTree.GetItemData(hTreeItem);
		pTreeNode = (TreeNode*)param;
		ASSERT( pTreeNode );
		pDoc->m_ptrCurrentNode = pTreeNode;
		ASSERT( pDoc->m_ptrCurrentNode );
		//loop that checks the attributes
		for ( int iCount = 0; iCount < pTreeNode->m_Tile.m_ListOfAttributes.m_ListAttributes.GetSize(); iCount++ )
			{
			TileAttributes * pTileAttrib = (TileAttributes *)pTreeNode->m_Tile.m_ListOfAttributes.m_ListAttributes[iCount];
			CString type = pTileAttrib->get_AttributeType();
			CString name = pTileAttrib->get_AttributeName();
			CString value = pTileAttrib->get_AttributeValue();
			GetDocument()->m_MapName2Value.SetAt(name,value);
			}
		CCtrlListAttributes dlg;
		dlg.DoModal();
		}
	}
/************************************************************************************
/* + void CDCLView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags - 
*   Take care of the TAB Key press.If it is TAB key Select the 
*   first item in Listctrl
*
*
*
*/

void CDCLView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
	{
	// TODO: Add your message handler code here and/or call default
	CDCLDoc* pDoc = (CDCLDoc*)GetDocument();
	BOOL bRes = FALSE;
	ASSERT( pDoc );
	if ( pDoc )
		{
			if ( nChar == 9 ) //TAB KEY
				{
				pDoc->m_pDialogList->GetListCtrl().SetFocus();
				LVITEM lv;
				lv.mask = LVIF_STATE;
				lv.iItem = 0;
				lv.iSubItem = 0;
				lv.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
				lv.state = LVIS_SELECTED | LVIS_FOCUSED;
				bRes = pDoc->m_pDialogList->GetListCtrl().SetItem(&lv);
				ASSERT( bRes );
				}
		}
	CTreeView::OnChar(nChar, nRepCnt, nFlags);
	}
