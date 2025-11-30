/* G:\ICADDEV\PRJ\ICAD\ICADCMDBAR.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#pragma once

#include "IcadTextScrn.h"

	//	Window IDs for Command Bar parts
#define IDC_CMDBAR				1102
#define IDC_CMDPROMPTEDIT		1103
#define IDC_CMDPROMPTLIST		1104

/////////////////////////////////////////////////////////////////////
// Command bar class.
#include "sizecbar.h"

class CIcadCmdBarBorderWnd;

class CCmdBar : public CSizingControlBar
{
    DECLARE_DYNAMIC(CCmdBar)

public:
    CCmdBar();
    virtual ~CCmdBar();
	//*** Member variables
	CIcadCmdBarBorderWnd* m_pBorderWnd;
	CIcadPromptWnd* m_pPromptWnd;
	CSize m_dynSize;
	int m_nFloatWidth,m_nDockedWidth;
    CBrush* m_pbrBkgnd;
	CWnd* m_pParent;
	//*** Member functions
    BOOL Create(CWnd* pParentWnd, LPCRECT lpRect);
	void DoPaint(CDC* pDC);
	void UpdateCmdBar(void);
    CSize CalcDynamicLayout(int nLength, DWORD dwMode);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

protected:
	//*** Message handlers (huh-huh)
	//{{AFX_MSG(CCmdBar)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int cx, int cy);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysColorChange(void);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CIcadCmdBarBorderWnd window

class CIcadCmdBarBorderWnd : public CWnd
{
    DECLARE_DYNAMIC(CIcadCmdBarBorderWnd)

// Construction
public:
	CIcadCmdBarBorderWnd();

// Attributes
public:
	CIcadPromptWnd* m_pPromptWnd;
	CWnd* m_pParentWnd;

protected:
    CBrush* m_pbrBkgnd;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadCmdBarBorderWnd)
	public:
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIcadCmdBarBorderWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIcadCmdBarBorderWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSysColorChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


