#if !defined(AFX_ICADSPELL_H__50759671_CAF1_11D0_90AE_0060974FCFCA__INCLUDED_)
#define AFX_ICADSPELL_H__50759671_CAF1_11D0_90AE_0060974FCFCA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// IcadSpell.h : header file
//
#include "VSpell.h"
#include "VSPEwrap.h"
#include "icaddialog.h"

//*** Spelling modes
#define SPL_SELSET	0

/////////////////////////////////////////////////////////////////////////////
// IcadSpellDlg dialog

class IcadSpellDlg : public IcadDialog
{
// Construction
public:
	IcadSpellDlg(CWnd* pParent=NULL);   // standard constructor
	void SpellSelSet(sds_name ssSpellCheck, CVSpell* pvSpell);
	void GetNextFromSelSet();

// Dialog Data
	//{{AFX_DATA(IcadSpellDlg)
	enum { IDD = IDD_SPELL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(IcadSpellDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    afx_msg void OnHelp();
	CVSpell* m_pvSpell;
	CString m_strSentence,m_strWord;
	CString m_strChangeTo;
	long m_ssct;
	sds_name m_ssSpellCheck;
	int m_nSpellMode;  //*** This can be removed if we see no need for it.
	int m_idxFirstChar,m_nCount,m_nStep;
	BOOL m_bSentenceChanged;

	// Generated message map functions
	//{{AFX_MSG(IcadSpellDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnIgnore();
	afx_msg void OnChange();
	afx_msg void OnSelChangeSuggestList();
	afx_msg void OnIgnoreAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICADSPELL_H__50759671_CAF1_11D0_90AE_0060974FCFCA__INCLUDED_)
