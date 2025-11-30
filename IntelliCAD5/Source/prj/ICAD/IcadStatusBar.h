/////////////////////////////////////////////////////////////////////
// Icad status bar class
#pragma once

class CMainWindow;

class CIcadStatusBar : public CStatusBar
{
    DECLARE_DYNCREATE(CIcadStatusBar)

public:
    CIcadStatusBar();
    virtual ~CIcadStatusBar();
	void UpdateCurLay(void);
	void UpdateCurCol(void);
	void UpdateCurLType(void);
	void UpdateCurStyle(void);
	void UpdateCurDimSt(void);
	void UpdateCurLw(void);

	int GetPaneFromPoint(CPoint point);
	HFONT m_hFont;
	CMenu* m_pLayerMenu;
	CMenu* m_pLtypeMenu;
	CMenu* m_pStyleMenu;
	CMenu* m_pDimstMenu;

	CToolTipCtrl m_ToolTipCtrl;
	CProgressCtrl* m_pProgress;

	void UpdateVariablePanes(LPCTSTR pszVariable);
	void AddToolTips(void);
	void ResizeToolTips(void);

	BOOL InitProgressBar(void);
	BOOL UpdateProgressBar(int nPercentage);
	BOOL CloseProgressBar(void);

	BOOL TextOut(LPCTSTR pszText);

	void SetCoordsPaneWidth(int nWidth);
	void SetCoordsText(LPCTSTR pszText);

protected:
	CMainWindow* m_pMain;

	// Inner Structure definition for storing Indicator Information.
	struct SPaneInfo
	{
		UINT uPaneValue;
		BOOL bShowPane;
	};

	SPaneInfo* m_pPaneArray;
	int m_numPanes;
	int m_numVisiblePanes;
	
	void SetVisibleIndicators ();
	void ShowHideVariablePanes(UINT nMenuID);

	CString GetRegistryRoot();
	int LoadSettingsFromRegistry();
	int SaveSettingsToRegistry();
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void MenuPick(UINT nId);

	// Generated message map functions
	//{{AFX_MSG(CIcadStatusBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpcs);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
