// IcadCusKeyboard.h : header file
//

#pragma once

#include "IcadAccelerator.h"
#include "IcadAccelEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CIcadCusKeyboard dialog

class CIcadCusKeyboard : public CPropertyPage
{
	DECLARE_DYNCREATE(CIcadCusKeyboard)
	friend class CIcadAccelEdit;

// Construction
public:
	CIcadCusKeyboard();
	~CIcadCusKeyboard();

//*** Member (uh huh-huh) variables
public:
	ICACCEL* m_pCurAccel;
	CIcadAccelEdit m_AccelEdit;

//*** Member (huh-huh-huh, huh-huh) functions
public:
	void ResetAccelList();
	void AccelToString(LPACCEL lpAccel, CString& strResult);
	void DeleteAccelListItem(int idxItem);
	void SaveCommandString();
	void OnExit();
	BOOL SaveAcceleratorsToFile(LPCTSTR pszFileName);
	BOOL ReadAcceleratorFile(LPCTSTR pszFileName);

// Dialog Data
	//{{AFX_DATA(CIcadCusKeyboard)
	enum { IDD = CUS_KEYBOARD };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CIcadCusKeyboard)
	public:
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CIcadCusKeyboard)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeDefKeysList();
	afx_msg void OnAddButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnAddCmdButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
