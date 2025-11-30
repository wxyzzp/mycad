#if !defined(AFX_OPTIONSSNAPDLGPIC_H__F560DD56_A0D7_11D4_BBFB_00105A717134__INCLUDED_)
#define AFX_OPTIONSSNAPDLGPIC_H__F560DD56_A0D7_11D4_BBFB_00105A717134__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsSnapDlgPic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsSnapDlgPic window

/*-----------------------------------------------------------------------------
Control class for subclussing standard staic control in COptionsSnapDlg to draw
OsnapMarker box with current size and current color
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COptionsSnapDlgPic : public CStatic
{
// Construction
public:
	COptionsSnapDlgPic();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsSnapDlgPic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COptionsSnapDlgPic();

	// Generated message map functions
protected:
	//{{AFX_MSG(COptionsSnapDlgPic)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	int m_iSize;
	int m_iThickness;
	int m_iColor;
	void Draw( CDC *pDC = NULL );
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSSNAPDLGPIC_H__F560DD56_A0D7_11D4_BBFB_00105A717134__INCLUDED_)
