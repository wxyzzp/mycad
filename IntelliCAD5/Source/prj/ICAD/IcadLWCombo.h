#if !defined _ICADLWCOMBOBOX_H
#define _ICADLWCOMBOBOX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IcadLWComboBox.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CIcadLWComboBox window

class CIcadLWComboBox : public CComboBox
{
// Construction
public:
	CIcadLWComboBox();
	virtual ~CIcadLWComboBox();

// Attributes
public:
	enum
		{
		WM_LWEIGHT_CHANGED = (WM_USER + 201)
		};

// Operations
public:
	void FillLWeight(int lwunits, int celweight, int bitcode );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadLWComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:

// Member variables
protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(CIcadLWComboBox)
	afx_msg void OnSelChange();
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


/////////////////////////////////////////////////////////////////////////////
// CIcadLWComboBox inline functions




#endif // _ICADLWCOMBOBOX_H
