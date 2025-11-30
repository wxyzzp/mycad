// DCLParsrDlg.h : header file
//

#if !defined(AFX_DCLPARSRDLG_H__DBA36007_05C2_11D3_87D2_0080C8006398__INCLUDED_)
#define AFX_DCLPARSRDLG_H__DBA36007_05C2_11D3_87D2_0080C8006398__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDCLParsrDlg dialog

class CDCLParsrDlg : public CDialog
{
// Construction
public:
	CDCLParsrDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDCLParsrDlg)
	enum { IDD = IDD_DCLPARSR_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDCLParsrDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDCLParsrDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DCLPARSRDLG_H__DBA36007_05C2_11D3_87D2_0080C8006398__INCLUDED_)
