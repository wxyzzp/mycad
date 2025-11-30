/* D:\ICADDEV\PRJ\ICAD\ICADCHILDWND.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * $Revision: 1.2.4.1 $ $Date: 2004/02/16 01:35:45 $
 *
 * Abstract
 *
 * Icad MDI child window class
 *
 */

#pragma once
#include "LayoutTabs.h"
#define _LAYOUTTABS_
class db_drawing;
class db_handitem;

class CIcadChildWnd : public CMDIChildWnd
{
protected:
    CIcadChildWnd();
    DECLARE_DYNCREATE(CIcadChildWnd)
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadIpFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

public:
    virtual ~CIcadChildWnd();
    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnToolTipTextW(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	void OnChildWndClose(void);
    CSplitterWnd m_wndSplitter;
	void UpdateLayoutTabs(db_drawing* pDrawing, BOOL bInit = FALSE);
	virtual void RecalcLayout(BOOL bNotify = TRUE);
protected:
    //*** These variables could be read from the config.
    int m_nMaxRows;  //*** Maximum number of rows for splitter window.
    int m_nMaxCols;  //*** Maximum number of columns for splitter window.
    CSize m_sizeMin; //*** Minimum size at which a splitter window-pane may be displayed.
    CRect m_WindowRect;
	db_drawing* m_pDrawingAtClose;
	db_handitem* m_pTabRecAtClose;

	CLayoutTabs m_layoutTabs;
	//{{AFX_MSG(CIcadChildWnd)
    afx_msg void OnMove(int cx, int cy);
	afx_msg void OnClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnChildActivate();
	//}}AFX_MSG
//4M Elias Item:50->
	afx_msg void OnSizing( UINT nSide, LPRECT lpRect );
//4M Elias Item:50<-
    void OnUpdatePrintPreview(CCmdUI* pCmdUI);
	void OnPreviewZoomIn();
	void OnPreviewZoomOut();
	void OnPreviewPrint();
	void OnPreviewSetup();
	void OnPreviewClose();
	void OnPreviewHelp();
	DECLARE_MESSAGE_MAP()

};

