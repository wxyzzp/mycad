#ifndef __DialogGroupHLightH__
#define __DialogGroupHLightH__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*------------------------------------------------------------------------------*//**
@author Denis Petrov

@description Representation of dialog for highlighting entities in group.
*//*----------------------------------------------------------------------------*/
class CDialogGroupHLight : public CDialog
{
// Construction
	CDialogGroupHLight(CWnd* pParent = NULL);   // standard constructor
public:
	CDialogGroupHLight(CWnd* pParent, sds_name entities);
	virtual BOOL OnInitDialog();
// Dialog Data
	//{{AFX_DATA(CDialogGroupHLight)
	enum { IDD = IDD_GROUP_HILITE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogGroupHLight)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	void changeCurrentEntity();
// Implementation
protected:
	sds_name m_entities;
	sds_name m_currentEntity;
	long m_count;
	long m_index;

	CStatic* m_pOutWin;
	static CString s_outString;

	// Generated message map functions
	//{{AFX_MSG(CDialogGroupHLight)
	afx_msg void OnNext();
	afx_msg void OnPrevious();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
