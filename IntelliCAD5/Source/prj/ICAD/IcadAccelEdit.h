// IcadAccelEdit.h : header file
//
#pragma once
/////////////////////////////////////////////////////////////////////////////
// IcadAccelEdit window

class CIcadAccelEdit : public CEdit
{
// Construction
public:
	CIcadAccelEdit();

// Attributes
public:
	CWnd* m_pParentDlg;
	char m_cKeyDownBits;
	int m_idxNewItem;

// Operations
public:
	void CreateString(UINT nChar);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadAccelEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIcadAccelEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIcadAccelEdit)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetfocus();
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
