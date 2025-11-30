/////////////////////////////////////////////////////////////////////
// CTextScreen class
#pragma once

#include "IcadPromptWnd.h"

class CTextScreen : public CFrameWnd
{
    DECLARE_DYNAMIC(CTextScreen)

public:
    CTextScreen();
    virtual ~CTextScreen();
    CWnd* m_pParent;
    CIcadPromptWnd* m_pPromptWnd;
    CBrush* m_pbrBkgnd;
	BOOL ShowWindow(int nCmdShow, BOOL bUpdateVars);

protected:
	//{{AFX_MSG(CTextScreen)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnMove(int cx, int cy);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnClose(void);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint(void);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSysColorChange();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
