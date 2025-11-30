/* PRJ\ICAD\INPUTEDITBOXDLG.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1.1.1 $ $Date: 2000/01/19 13:41:22 $ 
 * 
 * Abstract
 * 
 * Implements a generic edit box in a dialog that can be displayed to
 * obtain a value from the user.
 *
 * The function allows you to supply the dialog title, the edit box label,
 * and a validation object.
 *
 * Due to time constraints, the dialog class only deals with a CString;
 * but it could easily be extended to deal with ints, longs, sds_reals,
 * etc.
 * 
 */ 

#if !defined(_INPUTEDITBOXDLG)
#define _INPUTEDITBOXDLG

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CInputEditBoxDlg dialog
//
// A dialog that contains an edit box for obtaining single values from the
// user.

class CValueValidator;

class CInputEditBoxDlg : public CDialog
	{
public:
	// Construction
	CInputEditBoxDlg(CWnd* pParent = NULL);		// standard constructor

	// Attributes
	void SetTitle(const CString& title);
	void SetLabel(const CString& label);
	void SetValue(const CString& value);
	void SetValidator(CValueValidator *validator);

	CString& GetValue(void);

	// Dialog Data
	//{{AFX_DATA(CInputEditBoxDlg)
	enum { IDD = INPUT_EDIT_BOX };
	CStatic	m_labelCtrl;
	CEdit	m_valueCtrl;
	//}}AFX_DATA

	CString m_title;
	CString m_label;
	CString m_value;

	CValueValidator *m_validator;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputEditBoxDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(CInputEditBoxDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(_INPUTEDITBOXDLG)


