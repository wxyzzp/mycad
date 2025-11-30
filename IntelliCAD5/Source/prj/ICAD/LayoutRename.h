#if !defined(AFX_LAYOUTRENAME_H__2690B192_F714_4755_9023_BCF48D92B3AB__INCLUDED_)
#define AFX_LAYOUTRENAME_H__2690B192_F714_4755_9023_BCF48D92B3AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LayoutRename.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLayoutRename dialog

class CLayoutRename : public CDialog
{
// Construction
public:
	CLayoutRename(CWnd* pParent = NULL, LPCTSTR pLayoutName = _T("")); // standard constructor
	LPCTSTR layoutName() { m_layoutName.TrimLeft(); m_layoutName.TrimRight(); return m_layoutName; }

// Dialog Data
	//{{AFX_DATA(CLayoutRename)
	enum { IDD = IDD_LAYOUT_RENAME };
	CString	m_layoutName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayoutRename)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLayoutRename)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYOUTRENAME_H__2690B192_F714_4755_9023_BCF48D92B3AB__INCLUDED_)
