/* D:\ICADDEV\PRJ\ICAD\PRINTADVANCEDTAB.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Advanced tab on print dialog
 *
 */

#if !defined(AFX_PRINTGENERALTAB_H__CE54B895_6D27_11D2_BA8F_0060081C2430__INCLUDED_)
#define AFX_PRINTGENERALTAB_H__CE54B895_6D27_11D2_BA8F_0060081C2430__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PrintAdvancedTab.h : header file
//
namespace ICAD_AUTOMATION
{
struct IIcadPlotManager;
}
using namespace ICAD_AUTOMATION;


/////////////////////////////////////////////////////////////////////////////
// PrintAdvancedTab dialog

class PrintAdvancedTab : public CPropertyPage
{
// Construction
public:
	void LoadPlotSettings();
	void SavePlotSettings();
	void LoadRegSettings(bool bFromLayout = true);
	void SaveRegSettings(bool bToLayout = true);

	bool initSettings();
	bool wasPicked() const { return m_bWasPicked; }
	void setOrigin(double x, double y);
	void SetDefaults();
	PrintAdvancedTab(CWnd* pParent = NULL);   // standard constructor

	void SetPlotManager(IIcadPlotManager *plotMgr) {m_plotMgr = plotMgr;}


// Dialog Data
	//{{AFX_DATA(PrintAdvancedTab)
	enum { IDD = PRINT_ADVANCED };
	int	m_Center;
	CString	m_OriginX;
	CString	m_OriginY;
	CString	m_Header;
	CString	m_Footer;
//4M Item:99->
// Prints upside down
	int		m_Rotate;
//<-4M
// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
	int		m_UseLWeight;
	int		m_ScaleLWeight;
// EBATECH(CNBR) ]-
    //}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PrintAdvancedTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	double m_oX, m_oY;
	bool m_bWasPicked;
	IIcadPlotManager *m_plotMgr;
	// Generated message map functions
	//{{AFX_MSG(PrintAdvancedTab)
	afx_msg void OnOriginCenter();
	afx_msg void OnSelectOrigin();
	afx_msg void OnOpenCfg();
	afx_msg void OnSaveCfg();
	afx_msg void OnSelchangeHeader();
	afx_msg void OnSelchangeFooter();
	virtual BOOL OnInitDialog();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnClose();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTGENERALTAB_H__CE54B895_6D27_11D2_BA8F_0060081C2430__INCLUDED_)


