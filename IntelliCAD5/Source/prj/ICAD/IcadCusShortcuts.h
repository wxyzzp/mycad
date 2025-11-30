// IcadCusShortcuts.h : header file
//
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CIcadCusShortcuts dialog

class CIcadCusShortcuts : public CPropertyPage
{
	DECLARE_DYNCREATE(CIcadCusShortcuts)

// Construction
public:
	CIcadCusShortcuts();
	~CIcadCusShortcuts();

//*** Member variables
public:
	int m_idxCurAlias;

//*** Member functions
public:
	BOOL SaveAliasString();
	void ResetAliasList();
	BOOL SaveAliasesToFile(LPCTSTR pszFileName);
	BOOL ReadAliasFile(LPCTSTR pszFileName, BOOL bAppend);
	BOOL OnExit();


// Dialog Data
	//{{AFX_DATA(CIcadCusShortcuts)
	enum { IDD = CUS_SHORTCUTS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CIcadCusShortcuts)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CIcadCusShortcuts)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeAliasesList();
	afx_msg void OnNewButton();
	afx_msg void OnAssignButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnKillFocusAliasEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
