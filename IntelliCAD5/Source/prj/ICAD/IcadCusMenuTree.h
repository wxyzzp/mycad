#if !defined(AFX_ICADCUSMENUTREE_H__8CAF8C81_C589_11D0_90A6_0060974FCFCA__INCLUDED_)
#define AFX_ICADCUSMENUTREE_H__8CAF8C81_C589_11D0_90A6_0060974FCFCA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// IcadCusMenuTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIcadCusMenuTree window

class CIcadCusMenuTree : public CTreeCtrl
{
// Construction
public:
	CIcadCusMenuTree();
	CWnd* m_pParentDlg;
	char m_cKeyDownBits;
	HTREEITEM m_hCutItem;
	HTREEITEM m_hCopyItem;
	int m_nOrigImage;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadCusMenuTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIcadCusMenuTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIcadCusMenuTree)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICADCUSMENUTREE_H__8CAF8C81_C589_11D0_90A6_0060974FCFCA__INCLUDED_)
