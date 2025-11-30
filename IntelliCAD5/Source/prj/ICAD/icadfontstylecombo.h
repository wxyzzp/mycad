#if !defined _ICADFONTSTYLECOMBOBOX_H
#define _ICADFONTSTYLECOMBOBOX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IcadFontStyleComboBox.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CIcadFontStyleComboBox window

class CIcadFontStyleComboBox : public CComboBox
{
// Construction
public:
	CIcadFontStyleComboBox();
	virtual ~CIcadFontStyleComboBox();
	void DeleteStyleItems(void);

// Attributes
public:
	enum
		{
		WM_FONT_STYLE_CHANGED = (WM_USER + 100)
		};

// Operations
public:
	void FillTrueTypeStyles(const char *fontName, const long fontStyle);
	static int CALLBACK EnumFontFamExProcStyle(ENUMLOGFONTEX *enumLogFontEx,
		NEWTEXTMETRICEX *newTextMetricEx, int fontType, LPARAM lParam);
	int AddStyle(const char *styleName, const char *fullName,
		const int weight, const int italic, const int pitchAndFamily,
		const int charSet);	// EBATECH(CNBR) Add charSet

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadFontStyleComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
//	virtual void DeleteItem(LPDELETEITEMSTRUCT item);	// to do - maybe remove this

// Member variables
protected:
	CString m_styleName;

	// Generated message map functions
protected:
	//{{AFX_MSG(CIcadFontStyleComboBox)
	afx_msg void OnSelChange();
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


/////////////////////////////////////////////////////////////////////////////
// CIcadFontStyleComboBox inline functions




#endif // _ICADFONTSTYLECOMBOBOX_H
