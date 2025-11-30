/* D:\ICADDEV\PRJ\ICAD\BHATCHPATPROPTAB.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Pattern Properties tab of bhatch/boundary dialog
 * 
 */ 

#if !defined(AFX_BHATCHPATPROPTAB_H__FD440446_4CD4_11D2_BA83_0060081C2430__INCLUDED_)
#define AFX_BHATCHPATPROPTAB_H__FD440446_4CD4_11D2_BA83_0060081C2430__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BHatchPatPropTab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// BHatchPatPropTab dialog

class BHatchPatPropTab : public CDialog
{
// Construction
public:
	void EnableControls();
	enum PatternType {PREDEFINED, USER_DEFINED};
	enum PenWidth {PEN_NONE, PEN_18, PEN_25, PEN_35, PEN_50, PEN_70, PEN_100, PEN_140, PEN_200};

	BHatchPatPropTab(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(BHatchPatPropTab)
	enum { IDD = BHATCH_PAT_PROP };
	int		m_patType;
	double	m_spacing;
	double	m_scale;
	BOOL	m_crossHatch;
	BOOL	m_associative;
	double	m_angle;
	CString	m_isoPenWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BHatchPatPropTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(BHatchPatPropTab)
	afx_msg void OnSelchangePatType();
	virtual BOOL OnInitDialog();
	afx_msg void OnCrossHatch();
	afx_msg void OnAssociative();
	afx_msg void OnKillfocusAngle();
	afx_msg void OnKillfocusScale();
	afx_msg void OnKillfocusSpacing();
	afx_msg void OnSelchangeIsoPen();
	afx_msg void OnCopyHatch();
	//}}AFX_MSG

	void		 FillCombo();

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BHATCHPATPROPTAB_H__FD440446_4CD4_11D2_BA83_0060081C2430__INCLUDED_)


