/* D:\ICADDEV\PRJ\ICAD\ICADPROMPTMENU.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

/////////////////////////////////////////////////////////////////////
// Icad Prompt Menu class

#ifndef _ICADPROMPTMNU_H_
#define _ICADPROMPTMNU_H_

typedef struct tagPMITEM
{
	CString* pstr;
	CString* pcmd;
	CRect rect;   
} PMITEM, *LPPMITEM;

class CPromptMenu;

class CPromptMenuWnd : public CWnd
{
    DECLARE_DYNAMIC(CPromptMenuWnd)

	friend class CPromptMenu;

public:
    CPromptMenuWnd();
    virtual ~CPromptMenuWnd();
	//*** Member variables
	CPtrList* m_pList;
	CBrush* m_pbrBkgnd;
	CBrush* m_pbrHilite;
	CRect m_rectClient;
	int m_nPrevItem;
	LONG m_lFontHeight;
	LONG m_lFontDescent;
	HCURSOR m_prevCursor;
	BOOL m_bWin95;
	//*** Member functions
	BOOL Create(CWnd* pParent, LPCRECT lpRect);
	void DrawMenu(CDC* pDC);
	void DrawItem(int nIndex, int nHilite);
	void DrawSpacer(CDC* pDC, LPCRECT lpRect);

protected:
	BOOL m_bGotLButtonDown;
	BOOL m_bGotRButtonDown;

	int CalcIndexFromPt(CPoint point);

	//*** Message handlers (huh-huh)
	//{{AFX_MSG(CPromptMenuWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint(void);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};


class CPromptMenu : public CMiniFrameWnd
{
    DECLARE_DYNAMIC(CPromptMenu)

public:
    CPromptMenu();
    virtual ~CPromptMenu();
protected:
	//*** Member variables
	CMainWindow* m_pMain;
	CPromptMenuWnd* m_pTrackWnd;
	CPtrList* m_pList;
public:
	int m_nExtraX,m_nExtraY;
	//*** Member functions
	BOOL CreatePromptMenu(CPoint point, CMainWindow* pMain);
	void AppendMenu(LPCTSTR pStr,LPCTSTR pCmd);
	BOOL TrackPromptMenu(void);
	void Hide(void);

protected:
	//*** Message handlers (huh-huh)
	//{{AFX_MSG(CPromptMenu)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint(void);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnClose(void);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif


