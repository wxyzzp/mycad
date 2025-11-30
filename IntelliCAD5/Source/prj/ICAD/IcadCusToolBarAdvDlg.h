// File  : <DevDir>\source\prj\icad\IcadCusToolBarAdvDlg.h

#if !defined(AFX_ICADCUSTOOLBARADVDLG_H__D98FAC42_F871_11D0_90D4_0060974FCFCA__INCLUDED_)
#define AFX_ICADCUSTOOLBARADVDLG_H__D98FAC42_F871_11D0_90D4_0060974FCFCA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


// CIcadCusToolBarAdvDlg dialog

class CIcadCusToolBarAdvDlg : public CDialog
{
public:
	CIcadCusToolBarAdvDlg(CWnd* pParent);   // standard constructor
	BOOL BrowseBitmaps(int nEditBoxID);

// Dialog Data
	//{{AFX_DATA(CIcadCusToolBarAdvDlg)
	enum { IDD = CUS_TOOLBAR_ADVSETTINGS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadCusToolBarAdvDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CIcadTBbutton*		m_picButton;
	BOOL				m_bFlyOut;
	CIcadToolBarMain*	m_pTbMain;

	void InitBitmapCombo(int nID);
	BOOL UpdateBmpFromCombo(int nComboID);
	void UpdateTbButton();

	// Generated message map functions
	//{{AFX_MSG(CIcadCusToolBarAdvDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBrowseLgBw();
	afx_msg void OnBrowseLgColor();
	afx_msg void OnBrowseSmBw();
	afx_msg void OnBrowseSmColor();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICADCUSTOOLBARADVDLG_H__D98FAC42_F871_11D0_90D4_0060974FCFCA__INCLUDED_)
