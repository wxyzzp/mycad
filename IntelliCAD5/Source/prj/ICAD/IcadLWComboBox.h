#if !defined(AFX_ICADLWCOMBOBOX_H__3BC2E57E_7CFE_4130_9D3A_EAA18D3D8637__INCLUDED_)
#define AFX_ICADLWCOMBOBOX_H__3BC2E57E_7CFE_4130_9D3A_EAA18D3D8637__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IcadLWComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIcadNewLWComboBox window

class CIcadNewLWComboBox : public CComboBox
{
// Construction
public:
	CIcadNewLWComboBox();

// Attributes
public:
	int m_nDisplayScaleFactor;
	bool m_bDisplayValuesInMM;

// Operations
public:

	void AddLineWeight(int nLineWeight);
	int GetCurrentDefaultLWeight();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadNewLWComboBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIcadNewLWComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIcadNewLWComboBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICADLWCOMBOBOX_H__3BC2E57E_7CFE_4130_9D3A_EAA18D3D8637__INCLUDED_)
