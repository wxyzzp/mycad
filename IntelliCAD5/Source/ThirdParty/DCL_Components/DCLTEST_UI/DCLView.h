/** FILENAME:     DCLView.h- Contains declarations of class CDCLView.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract  TreeView class to show the in memory Tree
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

#ifndef _DCLVIEW_H
#define _DCLVIEW_H

#if _MSC_VER > 1000
#pragma once
#endif 

#include "dcldoc.h"

class CDCLView : public CTreeView
{
protected: // create from serialization only
	CDCLView();
	DECLARE_DYNCREATE(CDCLView)

// Attributes
public:
	CDCLDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDCLView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	void ShowAttribDialog(NMHDR* pNMHDR);
//use this function to insert b'cos here the lparam is set
	HTREEITEM Insert(							// Insert an item in the treeview
					CString& strItem,           // i : Text to insert 
					HTREEITEM hParent,          // i : parent item
					int iChildren,              // i : Children
					TreeNode* pTreeNode			// i : a pointer to the treenode whose description is in the node this is set as LPARAM
					);  
	HTREEITEM SetRoot(							// set the root item
						CString& strItem,       // i : text of the root
						int iChildren = 0		// i : Children
						);       
	HTREEITEM Insert(							// Insert an item in the treeview
					CString& strItem,           // i : String to insert
					HTREEITEM hParent,			// i : parent
					int iChildren				// i : Children 
					); 
	HTREEITEM InsertItem(						// Insert an item in the treeview
						CString& strItem        // i : string to insert
						);
	virtual ~CDCLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDCLView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReturn(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DCLView.cpp
inline CDCLDoc* CDCLView::GetDocument()
   { return (CDCLDoc*)m_pDocument; }
#endif

#endif
