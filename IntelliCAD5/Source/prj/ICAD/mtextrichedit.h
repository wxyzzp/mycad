#if !defined(AFX_MTEXTRICHEDIT_H__6109A20A_3485_11D3_BD7E_0060089608A4__INCLUDED_)
#define AFX_MTEXTRICHEDIT_H__6109A20A_3485_11D3_BD7E_0060089608A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MTextRichEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMTextRichEdit window

class CMTextRichEdit: public CRichEditCtrl
{
// Construction
public:
	CMTextRichEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMTextRichEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMTextRichEdit();
	bool init(SDS_mTextData* m_pMTextData);

	bool scaleText(double zoomCoeff);
	
	// beginIndex - index of first scalled letter (0-based)
	// endIndex - index of last scalled letter (0-based)
	// scaleText(1,1,x) - only second letter will be scalled
	bool scaleText(long beginIndex, long endIndex, double zoomCoeff);

	// redefined functions
	DWORD GetSelectionCharFormat( CHARFORMAT& cf ) const;
	BOOL SetSelectionCharFormat( CHARFORMAT& cf );
	DWORD GetDefaultCharFormat( CHARFORMAT& cf ) const;
	BOOL SetDefaultCharFormat( CHARFORMAT& cf );

	// Generated message map functions
protected:

	//{{AFX_MSG(CMTextRichEdit)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MTEXTRICHEDIT_H__6109A20A_3485_11D3_BD7E_0060089608A4__INCLUDED_)
