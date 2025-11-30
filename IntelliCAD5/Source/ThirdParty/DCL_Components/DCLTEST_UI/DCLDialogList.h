/** FILENAME:     DCLDialogList.h- Contains declarations of class CDCLDialogList.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract : View derived from ListView to show the dialog names 
*          in the left pane of the Frame window.The user will dblclick on 
*          a dialog to see its corresponding tree.
*
*Audit Log 
*
*
*Audit Trail
*
*
*/

#ifndef _DCLDIALOGLIST_H
#define _DCLDIALOGLIST_H

#if _MSC_VER > 1000
#pragma once
#endif 

#include "dcldoc.h"

class CDCLDialogList : public CListView
{
protected:
	CDCLDialogList();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDCLDialogList)

// Attributes
public:

// Operations
public:
	void TreeChanged(NMHDR *);
	CDCLDoc* GetDocument();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDCLDialogList)
	public:
	virtual void OnInitialUpdate();                 // add a column to Listview
	protected:
	virtual void OnDraw(CDC* pDC);                  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs); 
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDCLDialogList();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDCLDialogList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReturn(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnShowDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif 
