#if !defined(AFX_ICADCUSMENUADVDLG_H__D98FAC41_F871_11D0_90D4_0060974FCFCA__INCLUDED_)
#define AFX_ICADCUSMENUADVDLG_H__D98FAC41_F871_11D0_90D4_0060974FCFCA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// IcadCusMenuAdvDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIcadCusMenuAdvDlg dialog

class CIcadCusMenuAdvDlg : public CDialog
{
// Construction
public:
	CIcadCusMenuAdvDlg(CWnd* pParent, LPICMENUITEM lpMenuItem);   // standard constructor
	void EnableAllEnts(BOOL bEnable);

// Dialog Data
	//{{AFX_DATA(CIcadCusMenuAdvDlg)
	enum { IDD = CUS_MENU_ADVSETTINGS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadCusMenuAdvDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	LPICMENUITEM m_pMenuItem;

	// Generated message map functions
	//{{AFX_MSG(CIcadCusMenuAdvDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckAllEnts();
	virtual void OnOK();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICADCUSMENUADVDLG_H__D98FAC41_F871_11D0_90D4_0060974FCFCA__INCLUDED_)
