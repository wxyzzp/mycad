#if !defined _ICADFONTLANGCOMBOBOX_H
#define _ICADFONTLANGCOMBOBOX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IcadFontLangComboBox.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CIcadFontLangComboBox window

class CIcadFontLangComboBox : public CComboBox
{
// Construction
public:
	CIcadFontLangComboBox();
	virtual ~CIcadFontLangComboBox();

// Attributes
public:
	enum
		{
		WM_FONT_LANG_CHANGED = (WM_USER + 101)
		};

// Operations
public:
	void FillTrueTypeLangs(const char *fontName, const long fontStyle);
	static int CALLBACK EnumFontFamExProcLang(ENUMLOGFONTEX *enumLogFontEx,
		NEWTEXTMETRICEX *newTextMetricEx, int fontType, LPARAM lParam);
	int AddLang(const char *langName, const int charSet);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadFontLangComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:

// Member variables
protected:
	CString m_styleName;

	// Generated message map functions
protected:
	//{{AFX_MSG(CIcadFontLangComboBox)
	afx_msg void OnSelChange();
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


/////////////////////////////////////////////////////////////////////////////
// CIcadFontLangComboBox inline functions




#endif // _ICADFONTLANGCOMBOBOX_H
