#if !defined(AFX_ICADLWLISTBOX_H__5BB72883_6E24_41AF_A2EC_3D61D2CD846A__INCLUDED_)
#define AFX_ICADLWLISTBOX_H__5BB72883_6E24_41AF_A2EC_3D61D2CD846A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IcadLWListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIcadLWListBox window

class CIcadLWListBox : public CListBox
{
// Construction
public:
	CIcadLWListBox();

// Attributes
public:
	int m_nDisplayScaleFactor;
	bool m_bDisplayValuesInMM;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadLWListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

	void AddLineWeight(int nLineWeight);
	void SetCurrentDefLWeight(int nDefLineWeight);

// Implementation
public:
	virtual ~CIcadLWListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIcadLWListBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICADLWLISTBOX_H__5BB72883_6E24_41AF_A2EC_3D61D2CD846A__INCLUDED_)
