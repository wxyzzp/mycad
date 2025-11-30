/////////////////////////////////////////////////////////////////////
// CIcadPromptWnd class
#pragma once

#ifndef _ICADPROMPTWND_H
#define _ICADPROMPTWND_H

class CIcadPromptWnd;

class CIcadPromptWndList : public CWnd
{
    DECLARE_DYNAMIC(CIcadPromptWndList)

public:
    CIcadPromptWndList();
    virtual ~CIcadPromptWndList();
    void AddString(CString& str);
	void OnUpdateScrlHist(int nNewMaxHistLines);

	CWnd* m_pParentWnd;
	CIcadPromptWnd* m_pPromptWnd;
    int m_nFontHgt;
    int m_listHgt;
    int m_nLinesPerPage;
	char m_pCharWidth[95];
    //*** Off screen device context members
    CRect m_rectClient;
    CDC* m_pMemDC;
    CBrush* m_pbrBkgnd;
	CStringList* m_pHistList;
	CStringList* m_pHistViewList;
	int m_nMaxHistLines;
	BOOL m_bGotRButtonDown;

	HBITMAP m_hOldBitmap;
	HFONT m_hOldFont;

protected:
	//{{AFX_MSG(CIcadPromptWndList)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint(void);
	afx_msg void OnSysColorChange(void);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


class CIcadPromptWndEdit : public CWnd
{
    DECLARE_DYNAMIC(CIcadPromptWndEdit)

public:
    CIcadPromptWndEdit();
    virtual ~CIcadPromptWndEdit();
	void UpdateText(CString& str);
    int m_nFontHgt;
	CRect m_rectClient;
	CWnd* m_pParentWnd;
	CString m_strPrev;
	char m_pCharWidth[95];
    //*** Off screen device context members
	CDC* m_pMemDC;
    CBrush* m_pbrBkgnd;
    //CBitmap* m_pbmOld;
    //CFont* m_pOldFont;
	CString* m_pCmdStr;
	BOOL m_bGotRButtonDown;

	HBITMAP m_hOldBitmap;
	HFONT m_hOldFont;

protected:
	//{{AFX_MSG(CIcadPromptWndEdit)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint(void);
	afx_msg void OnSysColorChange(void);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


class CIcadPromptWnd
{

public:
    CIcadPromptWnd();
    virtual ~CIcadPromptWnd();
	BOOL Create(CWnd* pParentWnd);
	void OnSize(LPCRECT rectNew);

    CIcadPromptWndList* m_pPromptWndList;
    CIcadPromptWndEdit* m_pPromptWndEdit;

protected:

};


#endif // _ICADPROMPTWND_H
