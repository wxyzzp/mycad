/* F:\ICADDEV\PRJ\ICAD\OPTIONSDISPLAYTAB.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#if !defined(AFX_OPTIONSDISPLAYTAB_H__EA355C2C_EC44_11D2_9FBB_00C04F68E33B__INCLUDED_)
#define AFX_OPTIONSDISPLAYTAB_H__EA355C2C_EC44_11D2_9FBB_00C04F68E33B__INCLUDED_

#include "..\LIB\ICADLIB\Icadlib.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDisplayTab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// OptionsDisplayTab dialog

class OptionsDisplayTab : public CPropertyPage
{
	DECLARE_DYNCREATE(OptionsDisplayTab)

// Construction
public:
	OptionsDisplayTab();
	~OptionsDisplayTab();

// Dialog Data
	//{{AFX_DATA(OptionsDisplayTab)
	enum { IDD = OPTIONS_DISPLAY };
	int		iScrlHist;
	BOOL	bPromptMenu;
	BOOL	bAutoMenuLoad;
	BOOL	m_bContinuousMotion;
	//}}AFX_DATA

public:
	bool bBkgFlag;
	int iBkgColor;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(OptionsDisplayTab)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(OptionsDisplayTab)
	afx_msg void OnCmdSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnScreenColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDISPLAYTAB_H__EA355C2C_EC44_11D2_9FBB_00C04F68E33B__INCLUDED_)


