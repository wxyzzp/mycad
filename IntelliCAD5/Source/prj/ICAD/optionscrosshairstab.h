/* F:\ICADDEV\PRJ\ICAD\OPTIONSCROSSHAIRSTAB.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#if !defined(AFX_OPTIONSCROSSHAIRSTAB_H__EA355C2B_EC44_11D2_9FBB_00C04F68E33B__INCLUDED_)
#define AFX_OPTIONSCROSSHAIRSTAB_H__EA355C2B_EC44_11D2_9FBB_00C04F68E33B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsCrosshairsTab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// OptionsCrosshairsTab dialog

class OptionsCrosshairsTab : public CPropertyPage
{
	DECLARE_DYNCREATE(OptionsCrosshairsTab)

// Construction
public:
	OptionsCrosshairsTab();
	~OptionsCrosshairsTab();

// Dialog Data
	//{{AFX_DATA(OptionsCrosshairsTab)
	enum { IDD = OPTIONS_CROSSHAIRS };
	int		m_crosshairsSize;
	BOOL	m_alwaysCrosshair;
	BOOL	m_EnableAttraction;
	int		m_AttractionDistance;
	//}}AFX_DATA
public:
	bool bCurFlag;
	int iColorX;
	int iColorY;
	int iColorZ;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(OptionsCrosshairsTab)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(OptionsCrosshairsTab)
	afx_msg void OnXAxis();
	afx_msg void OnYAxis();
	afx_msg void OnZAxis();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnDeltaposCrosshairsSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposAttractPointsSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAttraction();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSCROSSHAIRSTAB_H__EA355C2B_EC44_11D2_9FBB_00C04F68E33B__INCLUDED_)


