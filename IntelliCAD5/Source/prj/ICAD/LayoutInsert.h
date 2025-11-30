#if !defined(AFX_LAYOUTINSERT_H__A41026A7_F1F6_408B_A31F_777D47B39936__INCLUDED_)
#define AFX_LAYOUTINSERT_H__A41026A7_F1F6_408B_A31F_777D47B39936__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LayoutInsert.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLayoutInsert dialog

class CLayoutInsert : public CDialog
{
// Construction
public:
	CLayoutInsert(CWnd* pParent, db_drawing* pDestDrawing, db_drawing* pSourceDrawing);

// Dialog Data
	//{{AFX_DATA(CLayoutInsert)
	enum { IDD = IDD_LAYOUT_INSERT };
	CListBox	m_layoutList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayoutInsert)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	db_drawing* m_pDrawingDest;
	db_drawing* m_pDrawingSource;
	int m_initialWidth;
	int m_initialHeight;

	void placeControls(int cx, int cy);

	CLayoutInsert(CWnd* pParent = NULL);   // standard constructor
	// Generated message map functions
	//{{AFX_MSG(CLayoutInsert)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* pMMI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYOUTINSERT_H__A41026A7_F1F6_408B_A31F_777D47B39936__INCLUDED_)
