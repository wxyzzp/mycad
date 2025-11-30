#if !defined(AFX_MTEXTSTYLEBOX_H__961089C6_4371_11D3_BD8F_0060089608A4__INCLUDED_)
#define AFX_MTEXTSTYLEBOX_H__961089C6_4371_11D3_BD8F_0060089608A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MTextStyleBox.h : header file
//

#include "TextStyleTableInfo.h"

/////////////////////////////////////////////////////////////////////////////
// CMTextStyleBox window
class CMTextStyleBox : public CComboBox
{
// Construction
public:
	CMTextStyleBox();

// Attributes
public:
	CTextStyleTableInfo		m_tableInfo;

// Operations
public:
	void FillStyleComboBox();
	void GetTextStyle();	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMTextStyleBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMTextStyleBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMTextStyleBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MTEXTSTYLEBOX_H__961089C6_4371_11D3_BD8F_0060089608A4__INCLUDED_)
