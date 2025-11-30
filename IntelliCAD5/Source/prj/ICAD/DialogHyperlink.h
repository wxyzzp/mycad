#ifndef __DialogHyperlinkH__
#define __DialogHyperlinkH__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*------------------------------------------------------------------------------*//**
@author Denis Petrov

@description Dialog for input of hyperlinks.
*//*----------------------------------------------------------------------------*/
class CDialogHyperlink : public CDialog
{
	enum EAction
	{
		eACTION_MODIFY,
		eACTION_REMOVE
	};
// Construction
	CDialogHyperlink(CWnd* pParent = NULL);   // standard constructor
public:
	CDialogHyperlink(CWnd* pParent, sds_name entities, bool bApplyLater = false);
	virtual ~CDialogHyperlink();

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	int setEntities(sds_name entities);
	const CString& hyperlink() const { return m_link; }
	const CString& description() const { return m_desc; }
	const CString& location() const { return m_loc; }

	bool isApplyLater() const { return m_bApplyLater; }
	void setApplyLater(bool bApplyLater) { m_bApplyLater = bApplyLater; }
	void OnApply();
	void modify();
	void remove();
// Dialog Data
	//{{AFX_DATA(CDialogHyperlink)
	enum { IDD = IDD_HYPERLINK };
	CString	m_link;
	CString	m_loc;
	CString	m_desc;
	int m_rel;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogHyperlink)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bApplyLater;
	EAction m_action;
	sds_name m_entities;

	// Generated message map functions
	//{{AFX_MSG(CDialogHyperlink)
	afx_msg void OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnHelp();
	afx_msg void OnBrowse();
	afx_msg void OnRemove();
	afx_msg void OnRelative();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
