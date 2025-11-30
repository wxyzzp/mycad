/* D:\ICADDEV\PRJ\ICAD\PRINTDIALOG.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Print dialog
 * 
 */ 

#if !defined(AFX_PRINTDIALOG_H__CE54B892_6D27_11D2_BA8F_0060081C2430__INCLUDED_)
#define AFX_PRINTDIALOG_H__CE54B892_6D27_11D2_BA8F_0060081C2430__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "IcadRes.h"
#include "IcadPrintDia.h"
#include "PrintAdvancedTab.h"
#include "PrintPenMapTab.h"
#include "PrintScaleViewTab.h"
#include "IcadDialog.h"

namespace ICAD_AUTOMATION
{
struct IIcadPlotProperties;
struct IIcadPlotManager;
}
using namespace ICAD_AUTOMATION;

/////////////////////////////////////////////////////////////////////////////
// CPrintDialogSheet

class CPrintDialogSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CPrintDialogSheet)

// Construction
public:
	CPrintDialogSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	enum Tab {SCALE_VIEW, PENMAP, ADVANCED};
	int m_prevTab;

	PrintAdvancedTab m_advancedTab;
	PrintScaleViewTab m_scaleViewTab;

	BOOL m_Preview, m_Printing, m_PrintDirect;
	BOOL m_bWasCalledFromPrintPreview;
	BOOL m_bPrintPreviewWasPressed;

	CStatic m_LayoutName;
	CEdit m_Layout;
	CButton m_SaveLayout;
	CStatic m_LayoutComboText;
	CComboBox m_LayoutCombo;	
	RECT m_PageRect;
	CButton m_PrintPreview;
	CButton m_PrintSetup;
	CButton m_Reset;	
	CButton m_Help;
	CButton m_Apply;


// Operations
public:

	BOOL SetDefaults(BOOL bDisplayWarning);
	BOOL GetCurrentPrintInfo(LPICPRINTINFO pPrintInfo);
	void DoEndDialog(int nResult);
	virtual IIcadPlotProperties* GetPlotProperties();
	virtual IIcadPlotManager* GetPlotManager();

	void setOrigin(double x, double y) { m_advancedTab.setOrigin(x, y); }
	void setWindow(double x1, double y1, double x2, double y2) { m_scaleViewTab.setWindow(x1, y1, x2, y2); }

	bool isRestorySettingsFromRegistry() const { return m_bRestorySettingsFromRegistry; }

	void SaveRegSettings();
	void OnApplyClick();
	int GetUnits();
	void SetUnits(int Units);
	afx_msg void OnLoadPlotSettings();
	afx_msg void OnSavePlotSettings();


// Implementation
public:
	virtual ~CPrintDialogSheet();

	// Generated message map functions
protected:

	PrintPenMapTab m_penMapTab;

	
	IIcadPlotProperties	*m_properties;
	IIcadPlotManager *m_plotMgr;
	int m_units;

	bool m_bRestorySettingsFromRegistry;

	// Generated message map functions
	//{{AFX_MSG(CPrintDialogSheet)	
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnPrintPreview();
	afx_msg void OnPrintSetup();
	afx_msg void OnHelp();
	afx_msg void OnCancel();
	afx_msg void OnOK();
	afx_msg void OnSetDefaults();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSelchangeLoadFrom();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTDIALOGSHEET_H__1F52B7EA_38F4_492B_8B35_B261E040FA59__INCLUDED_)
