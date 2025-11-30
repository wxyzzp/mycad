// CDwfInfoDialog.h : header file
//

#if !defined(AFX_DWFINFODIALOG_H__C37D2D61_3333_44CF_8B48_B0B79EE6F34D__INCLUDED_)
#define AFX_DWFINFODIALOG_H__C37D2D61_3333_44CF_8B48_B0B79EE6F34D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDwfInfoDialog dialog

class CDwfInfoDialog : public CDialog
{
// Construction
public:
	CDwfInfoDialog(	char* caption, 
					char* versionCaption,
					char* formatCaption,
					char* ver1,
					char* ver2,
					char* ver3,
					char* format1,
					char* format2,
					char* format3,
					char* layoutCaption,
					char* layoutCurrent,
					char* layoutAll,
					CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDwfInfoDialog)
	enum { IDD = IDD_DWFINFODLG_DIALOG };
	CString m_sCaption;
	CString m_sVersionGroupCaption;
	CString m_sFormatGroupCaption;
	CString m_sVersion1;
	CString m_sVersion2;
	CString m_sVersion3;
	CString m_sFormat1;
	CString m_sFormat2;
	CString m_sFormat3;
	int m_iVersionFormat;
	CString m_sLayoutCaption;
	CString m_sLayoutCurrent;
	CString m_sLayoutAll;

		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDwfInfoDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDwfInfoDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDwfVersionRadio1();
	afx_msg void OnDwfVersionRadio2();
	afx_msg void OnDwfVersionRadio3();
	afx_msg void OnDwfLayoutRadio1();
	afx_msg void OnDwfLayoutRadio2();
	afx_msg void OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CButton	m_help;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DWFINFODIALOG_H__C37D2D61_3333_44CF_8B48_B0B79EE6F34D__INCLUDED_)
