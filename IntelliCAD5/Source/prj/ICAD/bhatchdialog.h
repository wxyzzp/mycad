/* D:\ICADDEV\PRJ\ICAD\BHATCHDIALOG.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Bhatch/boundary dialog
 *
 */

#if !defined(AFX_BHATCHDIALOG_H__FD440443_4CD4_11D2_BA83_0060081C2430__INCLUDED_)
#define AFX_BHATCHDIALOG_H__FD440443_4CD4_11D2_BA83_0060081C2430__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "bhatchpatproptab.h"
#include "bhatchpatterntab.h"
#include "bhatchboundarytab.h"
#include "icaddialog.h"

const CString USER = "_USER";

class BHatchDialog : public IcadDialog
{
public:
	enum BoundaryEntity {POLYLINE, HATCH};
	enum Tab {BOUNDARY, PATTERN_PROP, PATTERN};

// Construction
	BHatchDialog(BoundaryEntity enttype, db_hatch *hatch = NULL, CWnd* pParent = NULL);   // standard constructor

    void RecalcPattern(
//4M Item:17->
// BHatch Edit
         sds_name * hatches=NULL
//<-4M Item:17
      );

// Dialog Data
	//{{AFX_DATA(BHatchDialog)
	enum { IDD = BHATCH_MAIN };
	CTabCtrl	m_TabCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BHatchDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void EnablePatternTab(BOOL enable);
    void RedrawHatch(
//4M Item:17->
// BHatch Edit
         sds_name * hatches=NULL
//<-4M Item:17
      );
    void UndrawHatch();

	// Hatch will need to be initialized to dialog box settings.
	void InitializeHatch();
	void UpdateHatch();
	void CopyHatchPropertiesAndPattern();

	BHatchBoundaryTab m_boundaryTab;
	BHatchPatPropTab m_patPropTab;
	BHatchPatternTab m_patternTab;

	//Pointer to the hatch being created/modified. This is NULL if bpoly using the dialog.
	db_hatch*	m_hatch;

	// Flag: "true" if current hatch pattern definition is copied.
	bool		m_bCopied;

protected:
	BoundaryEntity m_boundType;
	Tab m_prevTab;

private:
	// Original system variables.  Since hatch dialog is destroyed while still active (when picking points)
	// Need to save the original system variables so that they can be restored when the user cancels
	int m_origStyle;
	double m_origScale;
	double m_origSpace;
	double m_origAngle;
	int m_origCrossHatch;
	CString m_origName;

	// Generated message map functions
	//{{AFX_MSG(BHatchDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTab2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnHelp();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BHATCHDIALOG_H__FD440443_4CD4_11D2_BA83_0060081C2430__INCLUDED_)


