// IcadTBDlg.h : header file
//

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CTbListBox window

struct CTblItems{
	BOOL IsChecked;
	UINT Index;
	CTblItems *next;
};

class CTbListBox : public CCheckListBox
{
public:
	CTbListBox();
	~CTbListBox();
// Operations

// Implementation

};


/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CIcadTBDlg dialog
class CIcadToolBarMain;

class CIcadTBDlg : public CDialog
{
//*** Member variables
public:
	int m_idxCurSel;
	CIcadToolBarMain* m_pTbMain;

//*** Member functions
public:
	void UpdateTbName();

// Construction
public:
	CIcadTBDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CIcadTBDlg)
	enum { IDD = IDD_ICAD_TB };
	CButton	m_Color;
	CButton	m_LgBut;
	CButton	m_Tcheck;
	CTbListBox	m_TbList;
	//}}AFX_DATA
	BOOL m_HasTipChng,m_HasLgChng,m_HasColorChng;
	int m_ExpLevel;
	bool m_bExport;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadTBDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual  BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIcadTBDlg)
	afx_msg void OnCustomize();
	afx_msg void OnDelete();
	virtual void OnOK();
	afx_msg void OnIcadTbCheck3();
	afx_msg void OnIcadTbCheck1();
	afx_msg void OnIcadTbCheck2();
	virtual void OnCancel();
	afx_msg void OnSetfocusForCustomize();
	afx_msg void OnSelChangeTbList();
	//}}AFX_MSG
    afx_msg void OnHelp();
	void OnExport();

	DECLARE_MESSAGE_MAP()
};
