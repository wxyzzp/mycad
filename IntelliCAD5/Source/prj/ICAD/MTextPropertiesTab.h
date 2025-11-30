/* D:\ICADDEV\PRJ\ICAD\CMTextPropertiesTab.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#if !defined(AFX_CMTextPropertiesTab_H__72621C92_0E07_11D3_BE92_00C04F990354__INCLUDED_)
#define AFX_CMTextPropertiesTab_H__72621C92_0E07_11D3_BE92_00C04F990354__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CMTextPropertiesTab.h : header file
//
#include "MTextStyleBox.h"
#include "MTextRichEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CMTextPropertiesTab dialog

class CMTextPropertiesTab : public CPropertyPage
{
	DECLARE_DYNCREATE(CMTextPropertiesTab)

// Construction
public:
	CMTextPropertiesTab();
	~CMTextPropertiesTab();

// Dialog Data
	//{{AFX_DATA(CMTextPropertiesTab)
	enum { IDD = MTEXT_PROPERTIES };
	CMTextStyleBox	m_cstyle;
	CComboBox	m_cwidth;
	CComboBox	m_crotate;
	CComboBox	m_cjust;
	CString	m_rotation;
	//Bugzilla No. 78396; 23-12-2002
    //CString	m_style;
	CString	m_width;
	int		m_justification;
	//}}AFX_DATA

	bool init(SDS_mTextData* pMTextData);
	void UpdateStyle();
	void UpdateJustification();


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMTextPropertiesTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMTextPropertiesTab)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokJustification();
	afx_msg void OnSelendokRotation();
	afx_msg void OnSelendokStyle();
	afx_msg void OnSelendokWidth();
	afx_msg void OnKillfocusRotation();
	afx_msg void OnKillfocusWidth();
	//}}AFX_MSG

	void FillCombo();
	CMTextRichEdit* GetRichEdit() { return (CMTextRichEdit*)GetParent()->GetDlgItem(IDC_RICHEDIT); }

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMTextPropertiesTab_H__72621C92_0E07_11D3_BE92_00C04F990354__INCLUDED_)


