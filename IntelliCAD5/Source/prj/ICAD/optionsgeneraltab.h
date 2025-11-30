/* F:\ICADDEV\PRJ\ICAD\OPTIONSGENERALTAB.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#if !defined(AFX_OPTIONSGENERALTAB_H__87E10C13_F0F4_11D2_9FBD_00C04F68E33B__INCLUDED_)
#define AFX_OPTIONSGENERALTAB_H__87E10C13_F0F4_11D2_9FBD_00C04F68E33B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsGeneralTab.h : header file
//

//this is the default auotsave time when the user enables autosave
#define AUTOSAVETIME 30

/////////////////////////////////////////////////////////////////////////////
// OptionsGeneralTab dialog

class OptionsGeneralTab : public CPropertyPage
{
	DECLARE_DYNCREATE(OptionsGeneralTab)

// Construction
public:
	OptionsGeneralTab();
	~OptionsGeneralTab();

// Dialog Data
	//{{AFX_DATA(OptionsGeneralTab)
	enum { IDD = OPTIONS_GENERAL };
	int		m_autoSaveTime;
	int		m_expLevel;
	BOOL	m_autoSave;
	CString	m_saveExt;
	BOOL	m_bVbaSecurity;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(OptionsGeneralTab)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(OptionsGeneralTab)
	virtual BOOL OnInitDialog();
	afx_msg void OnAutosave();
	afx_msg void OnAutoSaveSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

			void FillCombo();

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSGENERALTAB_H__87E10C13_F0F4_11D2_9FBD_00C04F68E33B__INCLUDED_)


