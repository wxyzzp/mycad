/* D:\ICADDEV\PRJ\ICAD\BHATCHPATTERNTAB.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Pattern tab of bhatch/boundary dialog
 *
 */

#if !defined(AFX_BHATCHPATTERNTAB_H__FD440445_4CD4_11D2_BA83_0060081C2430__INCLUDED_)
#define AFX_BHATCHPATTERNTAB_H__FD440445_4CD4_11D2_BA83_0060081C2430__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

struct cmd_hpdefll;

/////////////////////////////////////////////////////////////////////////////
// BHatchPatternTab dialog

class BHatchPatternTab : public CDialog
{
// Construction
public:
	BHatchPatternTab(CWnd* pParent = NULL);   // standard constructor
	~BHatchPatternTab();
// Dialog Data
	//{{AFX_DATA(BHatchPatternTab)
	enum { IDD = BHATCH_PATTERN };
	CScrollBar	m_scrollBar;
	CListBox	m_nameList;
	CComboBox m_fileCombo;
	CString	m_patternName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BHatchPatternTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:
	void EnableControls(BOOL enable);

	//After the dialog is initialized and the pattern name is loaded this should be called to set
	//the file and the pattern data
	//returns RTERROR if failed
	int InitPatternData();

	cmd_hpdefll *m_patternDef;
	int m_numPatLines;


//4M Spiros Item:17->
	CString filepat;
//4M Spiros Item:17<-

// Implementation
protected:
	CString m_hatchFilePath;
	BOOL m_tabEnabled;

	// keep track of where in slides we are
	int m_curSlideLine;
	int m_numPatterns;
	int m_maxLine;   // m_maxLine indicates the maximum top of screen line.

	void InitPatList();
	void InitPatFile();
	void ClickSlide(int slideNum);
	void UpdatePattern();

	// Generated message map functions
	//{{AFX_MSG(BHatchPatternTab)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSelchangeName();
	afx_msg void OnSelchangeFile();
	afx_msg void OnSlide1();
	afx_msg void OnSlide10();
	afx_msg void OnSlide11();
	afx_msg void OnSlide12();
	afx_msg void OnSlide13();
	afx_msg void OnSlide14();
	afx_msg void OnSlide15();
	afx_msg void OnSlide2();
	afx_msg void OnSlide3();
	afx_msg void OnSlide4();
	afx_msg void OnSlide5();
	afx_msg void OnSlide6();
	afx_msg void OnSlide7();
	afx_msg void OnSlide8();
	afx_msg void OnSlide9();
	//}}AFX_MSG

	afx_msg void OnVScroll(UINT nCode, UINT nPos, CScrollBar *pScrollBar);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BHATCHPATTERNTAB_H__FD440445_4CD4_11D2_BA83_0060081C2430__INCLUDED_)


