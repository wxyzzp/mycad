// DialogPassword.h : header file
//

#if !defined(AFX_PASSWORDDLG_H__321170A8_5F99_4F87_9C8A_3409EA1C3108__INCLUDED_)
#define AFX_PASSWORDDLG_H__321170A8_5F99_4F87_9C8A_3409EA1C3108__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog

class CPasswordDlg : public CDialog
{
// Construction
public:
	CPasswordDlg(char* caption = NULL, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPasswordDlg)
	enum { IDD = IDD_PASSWORD_DIALOG };
	CString	m_sPassword;
	CString	m_sFileName;
	CString	m_sMessage;
	CString	m_sCaution;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPasswordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString	m_Caption;
	// Generated message map functions
	//{{AFX_MSG(CPasswordDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSWORDDLG_H__321170A8_5F99_4F87_9C8A_3409EA1C3108__INCLUDED_)
