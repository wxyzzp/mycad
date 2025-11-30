/* F:\ICADDEV\PRJ\ICAD\OPTIONSPATHSTAB.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#if !defined(AFX_OPTIONSPATHSTAB_H__EA355C2D_EC44_11D2_9FBB_00C04F68E33B__INCLUDED_)
#define AFX_OPTIONSPATHSTAB_H__EA355C2D_EC44_11D2_9FBB_00C04F68E33B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OptionPathListCtrl.h"

// OptionsPathsTab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// OptionsPathsTab dialog

class OptionsPathsTab : public CPropertyPage
{
	DECLARE_DYNCREATE(OptionsPathsTab)

// Construction
public:
	OptionsPathsTab();
	~OptionsPathsTab();

// Dialog Data
	//{{AFX_DATA(OptionsPathsTab)
	enum { IDD = OPTIONS_PATHS };
	OptionPathListCtrl	m_pathList;
	OptionPathListCtrl	m_fileList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(OptionsPathsTab)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//enums to store data position
	// EBATECH(CNBR) -[ 2002/6/20 Add SAVEFILEPATH
	enum Paths{DRAWINGS, FONTS, HELP, XREFS, MENUS, HATCH_PAT, BLOCKS, SAVEFILEPATH, TEMPLATES};
	//enum Paths{DRAWINGS, FONTS, HELP, XREFS, MENUS, HATCH_PAT, BLOCKS};
	enum Files{LOGFILE, TEMPLATE, ALT_FONT, FONT_MAP};

	void SaveListItemToSysVar(int item, int subitem, char *sysvar);

	// Generated message map functions
	//{{AFX_MSG(OptionsPathsTab)
	virtual BOOL OnInitDialog();
	afx_msg void OnPathBrowse();
	afx_msg void OnFileBrowse();
	//}}AFX_MSG

//	void ClickList(OptionPathListCtrl &listCtrl, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSPATHSTAB_H__EA355C2D_EC44_11D2_9FBB_00C04F68E33B__INCLUDED_)


