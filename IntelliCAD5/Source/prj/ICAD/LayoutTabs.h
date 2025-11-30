#pragma once

#ifndef __LayoutTabsH__
#define __LayoutTabsH__

class db_drawing;

class CLayoutTabs: public CWnd 
{
protected:
	static LPCTSTR s_lpClassName;
	static BOOL s_bRegistered;

	class CLayoutTab 
	{
	private:
		CString	m_name;
		CRect m_rect;
		CRgn m_rgn;
		
	public:
		CLayoutTab(LPCTSTR lpszText): m_name(lpszText) { }
		
		int	ComputeRgn(CDC& dc, int x);
		int	Draw(CDC& dc, CFont& font, BOOL bSelected);
		BOOL HitTest(CPoint pt) { return m_rgn.PtInRegion(pt); }
		CRect GetRect() const { return m_rect; }
		void GetTrapezoid(const CRect& rc, CPoint* pts) const;
		LPCTSTR	GetText() const { return m_name; }
		void SetText(LPCTSTR lpszText) { m_name = lpszText; }
	};
	
	class CButtonEx: public CButton 
	{
	public:
		enum 
		{ 
			eFIRST = 1, 
			ePREV = 2, 
			eNEXT = 3, 
			eLAST = 4,
			eNBUTTONS = 4
		};
	protected:
		virtual void DrawItem(LPDRAWITEMSTRUCT lpDis);
	};

	CButtonEx m_buttons[CButtonEx::eNBUTTONS]; // first/prev/next/last buttons
	CPtrList m_tabs;
	int	m_iCurItem;
	int	m_cxDesired;
	int	m_iFirstTab;
	int m_iEditedTab;
	CFont m_fontToUse;

	void InvalidateTab(int iTab, BOOL bErase=TRUE);
	CLayoutTab* GetTab(int iPos);
	void RecomputeLayout();
	int	HitTest(CPoint pt);

public:
	CLayoutTabs();
	virtual ~CLayoutTabs();

	BOOL CreateFromStatic(UINT nID, CWnd* pParent);

	virtual BOOL Create(DWORD dwWndStyle, const RECT& rc, CWnd* pParent, UINT nID);

	void SetFont(CFont* pFont, BOOL bRedraw = TRUE);
	CFont* GetFont() const { return CFont::FromHandle((HFONT)m_fontToUse.GetSafeHandle()); }

	int	GetCurSel()	const { return m_iCurItem; }
	int	SetCurSel(int nItem);
	int	GetItemCount() const { return m_tabs.GetCount(); }

	LONG InsertItem(int nItem, TC_ITEM* pItem);
	BOOL GetItem(int nItem, TC_ITEM* pItem) const;
	BOOL DeleteItem(int nItem);
	BOOL DeleteAllItems();

	void UpdateLayoutTabs(db_drawing* pDrawing, BOOL bInit = FALSE);

protected:
	afx_msg int  OnCreate(LPCREATESTRUCT lpcs);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnFirstTab();
	afx_msg void OnNextTab();
	afx_msg void OnPrevTab();
	afx_msg void OnLastTab();
	afx_msg void OnPageSetup();
	afx_msg void OnLayoutNew();
	afx_msg void OnLayoutDelete();
	afx_msg void OnLayoutRename();
	DECLARE_DYNAMIC(CLayoutTabs);
	DECLARE_MESSAGE_MAP()
};

#endif