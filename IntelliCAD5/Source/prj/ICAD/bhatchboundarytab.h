/* D:\ICADDEV\PRJ\ICAD\BHATCHBOUNDARYTAB.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *	
 * Abstract
 *	
 * Boundary tab of bhatch/boundary dialog
 *	
 */ 

#if !defined(AFX_BHATCHBOUNDARYTAB_H__FD440444_4CD4_11D2_BA83_0060081C2430__INCLUDED_)
#define AFX_BHATCHBOUNDARYTAB_H__FD440444_4CD4_11D2_BA83_0060081C2430__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "listcoll.h"

/////////////////////////////////////////////////////////////////////////////
// BHatchBoundaryTab dialog

 class BHatchBoundaryTab : public CDialog
{
 public:
	 enum island {NESTED_ISLAND, OUTER_ISLAND, IGNORE_ISLAND};
	 enum boundSet {ALL_OBJECTS, CURRENT_SET};

// Construction
	BHatchBoundaryTab(BOOL isHatch, CWnd* pParent = NULL);	 // standard constructor

	int m_islandRadio;

// Dialog Data
	//{{AFX_DATA(BHatchBoundaryTab)
	enum { IDD = BHATCH_BOUNDARY };
	int		m_boundarySetRadio;
	BOOL	m_retainBoundCheck;
	CString	m_boundaryTypeCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BHatchBoundaryTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:
	HanditemCollection m_boundarySet;
	BOOL m_pickPointsCalled;
	BOOL m_selectObjectsCalled; 
	BOOL m_callFromBhatch;
	

// Implementation
protected:
	BOOL m_isHatch;

	void UpdateIslandBmp (int newBmp);

	// Generated message map functions
	//{{AFX_MSG(BHatchBoundaryTab)
	virtual BOOL OnInitDialog();
	afx_msg void OnIslandIgnore();
	afx_msg void OnIslandNested();
	afx_msg void OnIslandOuter();
	afx_msg void OnPickPoints();
	afx_msg void OnChooseObjects();
	afx_msg void OnBoundarySetSelect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BHATCHBOUNDARYTAB_H__FD440444_4CD4_11D2_BA83_0060081C2430__INCLUDED_)


