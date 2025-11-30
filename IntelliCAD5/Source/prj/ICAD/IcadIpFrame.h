// IcadIpFrame.h : interface of the CIcadIpFrame class
//
#pragma once


class CIcadToolBarMain;
class CIcadIpFrame;

class CIcadOleCntrFrameWnd : public COleCntrFrameWnd
{

public:
	CIcadOleCntrFrameWnd(CIcadIpFrame* pInPlaceFrame);

// Attributes
public:

// Operations
public:
	void EnableDocking(DWORD dwDockStyle);
	void DockControlBar(CControlBar* pBar, UINT nDockBarID = 0,
		LPCRECT lpRect = NULL);
	void DockControlBar(CControlBar* pBar, CDockBar* pDockBar,
		LPCRECT lpRect = NULL);

// Implementation
public:
	virtual ~CIcadOleCntrFrameWnd() {}

// Generated message map functions
protected:
	//{{AFX_MSG(CIcadOleCntrFrameWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

class CIcadIpFrame : public COleIPFrameWnd
{
	DECLARE_DYNCREATE(CIcadIpFrame)
public:
	CIcadIpFrame();

// Attributes
public:

// Operations
public:
	void IcadInPlaceMenuExec(UINT nID);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadIpFrame)
	public:
	virtual BOOL OnCreateControlBars(CFrameWnd* pWndFrame, CFrameWnd* pWndDoc);
	virtual void RepositionFrame(LPCRECT lpPosRect, LPCRECT lpClipRect);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIcadIpFrame();
	HMENU m_hIcadIpMenu;
	CIcadToolBarMain* m_pTbMain;
	BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	BOOL OnToolTipTextW(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	void OnAccelKey(UINT nId);

protected:
	CToolBar m_wndToolBar;
	COleResizeBar m_wndResizeBar;
	COleDropTarget m_dropTarget;
	virtual HMENU GetInPlaceMenu();

private:
	BOOL PreTranslateMessage(MSG* pMsg);

// Generated message map functions
protected:
	//{{AFX_MSG(CIcadIpFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnResizeChild(WPARAM wParam, LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////
