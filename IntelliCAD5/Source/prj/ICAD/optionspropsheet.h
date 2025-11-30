/* F:\ICADDEV\PRJ\ICAD\OPTIONSPROPSHEET.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#if !defined(AFX_OPTIONSPROPSHEET_H__EA355C2F_EC44_11D2_9FBB_00C04F68E33B__INCLUDED_)
#define AFX_OPTIONSPROPSHEET_H__EA355C2F_EC44_11D2_9FBB_00C04F68E33B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsPropSheet.h : header file
//
#include "OptionsCrosshairsTab.h"
#include "OptionsDisplayTab.h"
#include "OptionsPathsTab.h"
#include "OptionsGeneralTab.h"
#include "OptionsSnappingTab.h"
#include "IcadDialog.h"


/////////////////////////////////////////////////////////////////////////////
// OptionsPropSheet

class OptionsPropSheet : public IcadPropertySheet
{
	DECLARE_DYNAMIC(OptionsPropSheet)

// Construction
public:
	OptionsPropSheet(CWnd* = NULL);
	virtual ~OptionsPropSheet();

// Operations
public:

// Attributes
public:
	OptionsSnappingTab m_snappingTab;
	OptionsCrosshairsTab m_xhairTab;
	OptionsDisplayTab m_displayTab;
	OptionsPathsTab m_pathTab;
	OptionsGeneralTab m_generalTab;
	
	CButton m_help;

	BOOL setActivePage( UINT nID );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OptionsPropSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(OptionsPropSheet)
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSPROPSHEET_H__EA355C2F_EC44_11D2_9FBB_00C04F68E33B__INCLUDED_)


