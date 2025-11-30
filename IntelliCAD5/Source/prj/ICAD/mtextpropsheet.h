/* D:\ICADDEV\PRJ\ICAD\MTEXTPROPSHEET.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#if !defined(AFX_MTEXTPROPSHEET_H__72621C93_0E07_11D3_BE92_00C04F990354__INCLUDED_)
#define AFX_MTEXTPROPSHEET_H__72621C93_0E07_11D3_BE92_00C04F990354__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CMTextPropSheet.h : header file
//
#include "MTextTextTab.h"
#include "MTextPropertiesTab.h"
//#include "IcadDialog.h"
#include "MTextRichEdit.h"

void ConvertTextHeightToTwips(sds_real realHeight, long& twipsHeight);
void ConvertTwipsToTextHeight(sds_real& realHeight, long twipsHeight);
/////////////////////////////////////////////////////////////////////////////
// CMTextPropSheet
class CMTextPropSheet: public CPropertySheet//IcadPropertySheet
{
	DECLARE_DYNAMIC(CMTextPropSheet)

// Construction
public:
	CMTextPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMTextPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMTextPropSheet(UINT nIDCaption, SDS_mTextData* dataPtr, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	BOOL needCreation();
// Attributes
public:
	CMTextTextTab		m_textTab;
	CMTextPropertiesTab	m_propertiesTab;
	CMTextRichEdit		m_edit;
	CString				m_text;
	CRect				m_mtextWinodwRect;

	SDS_mTextData*		m_pMTextData;
	CButton	m_help;
	// Bugzilla No. 78437; 30-01-2003 [
	long m_nStartPaste;				// Starting position of the pasted text 
	long m_nEndPaste;				// Ending position of the pasted text
	long m_nStartChar;				// Starting position of the slected text
	long m_nEndChar;				// Ending position of the slected text
	BOOL m_bPasting;
	// Bugzilla No. 78437; 30-01-2003 ]
	
// Operations
public:
	void UpdateChar();
	// Bugzilla No. 78437; 30-01-2003
	void OnPaste();
	void UpdateButtons();
	void UpdateCharFormat();
	CIcadFontComboBox* GetFontCombo() { return &m_textTab.m_font; }
	CComboBox* GetFontSizeCombo() { return &m_textTab.m_fontsize; }

	double GetZoom() { return m_textTab.GetZoom(); }
	bool scaleText(double zoomCoeff) { return m_edit.scaleText(zoomCoeff); }
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMTextPropSheet)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMTextPropSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMTextPropSheet)
	afx_msg void OnHelp();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI );

	// Bugzilla No. 78437; 30-01-2003
	afx_msg void OnSelChange( SELCHANGE * pNotifyStruct, LRESULT * result );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MTEXTPROPSHEET_H__72621C93_0E07_11D3_BE92_00C04F990354__INCLUDED_)

