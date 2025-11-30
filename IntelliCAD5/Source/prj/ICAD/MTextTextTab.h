/* D:\ICADDEV\PRJ\ICAD\MTEXTTEXTTAB.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "IcadFontComboBox.h"	// Added by ClassView
#if !defined(AFX_MTEXTTEXTTAB_H__72621C91_0E07_11D3_BE92_00C04F990354__INCLUDED_)
#define AFX_MTEXTTEXTTAB_H__72621C91_0E07_11D3_BE92_00C04F990354__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MTextTextTab.h : header file
//

#include "IcadFontComboBox.h"
#include "MTextRichEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CMTextTextTab dialog

class CMTextTextTab: public CPropertyPage
{
	DECLARE_DYNCREATE(CMTextTextTab)

// Construction
public:
	CMTextTextTab();
	~CMTextTextTab();
	long m_begin, m_end;

// Dialog Data
	//{{AFX_DATA(CMTextTextTab)
	enum { IDD = MTEXT_TEXT };
	CIcadFontComboBox m_font;
	CButton	m_bold;
	CButton	m_undo;
	CButton	m_underline;
	CButton	m_italic;
	CComboBox m_fontsize;
	CComboBox m_zoomCBox;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMTextTextTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
public:
	CFont&	SelectedFont();
	// Bugzilla No. 78034; 23-12-2002 
	int GetTextHeightFromTwips(sds_real& textHeight, long twipsHeight);
	
	bool init(SDS_mTextData* pMTextData);
	CMTextRichEdit* GetRichEdit() { return (CMTextRichEdit*)GetParent()->GetDlgItem(IDC_RICHEDIT); }
	double GetZoom() { return m_prevZoom; } // m_prevZoom == curZoom

// Implementation
protected:
	bool changeZoom(CString& strZoom);
	int _getTextHeightFromTwips(sds_real* pTextHeight, long &twipsHeight);

protected:
	CFont	m_editFont;

	double	m_prevZoom;
	double  m_minZoom;
	static CString m_sLastZoom;    

	// Generated message map functions
	//{{AFX_MSG(CMTextTextTab)
	virtual BOOL OnInitDialog();
	afx_msg void OnBold();
	afx_msg void OnItalic();
	afx_msg void OnUnderline();
	afx_msg void OnSelendokFont();
	afx_msg void OnUndo();
	afx_msg void OnKillfocusFontsize();
	afx_msg void OnSelendokFontsize();
	afx_msg void OnSetfocusFontsize();
	afx_msg void OnKillfocusZoom();
	afx_msg void OnSelchangeZoom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MTEXTTEXTTAB_H__72621C91_0E07_11D3_BE92_00C04F990354__INCLUDED_)


