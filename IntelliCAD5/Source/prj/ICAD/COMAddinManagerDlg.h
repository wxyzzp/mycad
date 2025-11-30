
#ifndef _COMAddinManagerDlg_H
#define _COMAddinManagerDlg_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _COMAddinManager_H
#include "COMAddinManager.h"
#endif

class CCOMAddinManagerDlg : public CDialog
{
	CArray<CCOMAddinStatus,CCOMAddinStatus&> m_addins;
	
// Construction
public:
	CCOMAddinManagerDlg(CWnd* pParent = NULL);   // standard constructor
	~CCOMAddinManagerDlg(); 

protected:
	void updateDialog(int index);

	void outLoadBehaviorColumn(int index, const CCOMAddinStatus& aStatus);

	BOOL AddColumn(LPCTSTR strItem,int nItem,int nSubItem = -1, int nCx = -1,
				int nMask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 
				int nFmt = LVCFMT_LEFT);
	BOOL AddItem(int nItem,int nSubItem,LPCTSTR strItem,int nImageIndex = -1);
	
	// Dialog Data
	//{{AFX_DATA(UiAddinManagerDlg)
	enum { IDD = IDD_COMADDIN_MANAGER };
	CListCtrl	m_addinsListCtrl;
	CButton		m_checkBoxBtnLoad;
	CButton		m_checkBoxBtnStartup;
	CButton		m_checkBoxBtnCommand;
	CString		m_description;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UiAddinManagerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(UiAddinManagerDlg)
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckLoadBox();
	afx_msg void OnCheckStartupBox();
	afx_msg void OnCheckCommandBox();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //#define _COMAddinManagerDlg_H
