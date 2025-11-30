/* explorerpropstab.h
 * Copyright (C) 1997-1999 Visio Corporation. All Rights Reserved.
 *
 * Abstract
 *
 * Implements the tab for the Styles Properties in IntelliCAD Explorer.
 *
 */

#if !defined(_EXPLORERTEXTSTYLEPROPS_H)
#define _EXPLORERTEXTSTYLEPROPS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExplorerTextStyleProps.h : header file
//

#include "ExplorerSubDlg.h"		/*DNT*/
#include "IcadFontComboBox.h"	/*DNT*/
#include "IcadFontStyleCombo.h"	/*DNT*/
#include "IcadFontLangCombo.h"	/*DNT*/
#include "IcadStylePreview.h"	/*DNT*/
#include "styletabrec.h"

#ifdef USE_THIS
// Stores all the information about a font
typedef struct tagFONTOBJECT
{
	CString strName;
	CY	cySize;
	BOOL bBold;
	BOOL bItalic;
	BOOL bUnderline;
	BOOL bStrikethrough;
	short sWeight;
} FONTOBJECT;

// Merge objects are used when trying to consolidate multiple font properties.
// If the characteristics of these multiple properties differ then this is
// represented in the merge object.
typedef struct tagMERGEOBJECT
{
	BOOL bNameOK;
	BOOL bSizeOK;
	BOOL bStyleOK;
	BOOL bUnderlineOK;
	BOOL bStrikethroughOK;
} MERGEOBJECT;

#endif // USE_THIS




// Why is this class derived from IEXP_SubDlg, and why is it used in IntelliCAD Explorer?
//	   All of the Properties tabs in IntelliCAD Explorer were instances of IEXP_SubDlg.
//	   Because of the similarity between the Text Styles Properties tab and the dialog
//	   used in the Style command, I want to reuse as much code as I could.	At this
//	   point, using both IEXP_SubDlg and this new class would be quite a hassle; so
//	   it was cleaner to derive this class from IEXP_SubDlg and use it for all of the
//	   tabs.  Basically, this gives us compatability with the old code without making
//	   major modifications and allows us to add some new functionality.  The ideal
//	   approach would be to rewrite IntelliCAD Explorer to use tabs in a more normal manner
//	   (i.e., where each tab is its own dialog class with its own member variables
//	   and where the division of labor between the tabs and their container are more in
//	   line with normal property pages).
//
// What is the m_inTextStyleProperties flag for?
//	   To avoid making too many modifications to the existing code, this flag was added
//	   to let each instance know whether it should use its own functions or IEXP_SubDlg's
//	   functions (for example, DoDataExchange()).  If an instance is created in Drawing
//	   Explorer for any Properties tab other than for the Text Styles Properties tab,
//	   the IEXP_SubDlg functions should be used; otherwise, this class's functions
//	   should be used.
//
//


/////////////////////////////////////////////////////////////////////////////
// CExplorerPropsTab dialog

class CExplorerPropsTab : public IEXP_SubDlg
{
	DECLARE_DYNAMIC( CExplorerPropsTab )
public:
	// Construction
	CExplorerPropsTab(CWnd* pParent = NULL);				// standard constructor

	// Implementation
	bool GetTextStylesVariesInfo(void);						// see if values vary if multiple styles are selected
	void GetTextStyleValues(db_styletabrec *styleRec);		// get values from a style record
	void CompareTextStyleValues(db_styletabrec *styleRec);	// compare values from a style record and stored values
	void RestoreTextStyleValues(void);						// restore values to main view's list
	void SavePropertyValues(void);							// wrapper for saving values when done with Properties
	void GetTextStyleTableInfo(char *fontFilename, char *fontGlobalName, long *styleNumber); // EBATECH(CNBR) Add GlobalName

	void DrawFontPreview(void);
	bool ConvertObliqueAngle(double& value);

	void DDXDlgStyleNameStatic(CDataExchange* pDX);
	void DDXDlgHeight(CDataExchange* pDX);
	void DDXDlgWidth(CDataExchange* pDX);
	void DDXDlgObliqueAngle(CDataExchange* pDX);
	void DDXDlgFontName(CDataExchange* pDX);
	void DDXDlgFontStyle(CDataExchange* pDX);
	void DDXDlgFontLang(CDataExchange* pDX);	// EBATECH(CNBR)
	void DDXDlgBigfontName(CDataExchange* pDX); // EBATECH(CNBR)
	void DDXDlgBackwards(CDataExchange* pDX);
	void DDXDlgUpsideDown(CDataExchange* pDX);
	void DDXDlgVertically(CDataExchange* pDX);

	// friends shared with the Style command's dialog
	friend void InitCommonStyleValues(void);

	// Dialog Data
	bool m_inTextStyleProperties;							// in the Text Style Properties tab?
	bool m_hasTrueTypeFont;

	bool m_hasTrueTypeFontVaries;
	bool m_backwardsVaries;
	bool m_fontFilenameVaries;
	bool m_fontFamilyVaries;
	bool m_fontStyleVaries;
	bool m_bigfontFilenameVaries;	// EBATECH(CNBR)
	bool m_heightVaries;
	bool m_styleNameVaries;
	bool m_obliqueAngleVaries;
	bool m_upsideDownVaries;
	bool m_verticallyVaries;
	bool m_widthVaries;
	int  m_weight;
	bool m_italic;

	CString m_fontFilename;
	CString m_fontFamily;
	CString m_bigfontFilename;		// EBATECH(CNBR)

	long m_fontStyle;

	// text values used for doubles in dialog
	CString m_heightStr;
	CString m_widthStr;
	CString m_obliqueAngleStr;

	// new values from dialog; needed for finding out what changed and
	// updating values in the table record(s)
	bool	m_newHasTrueTypeFont;
	CString m_newHeightStr;
	CString m_newWidthStr;
	CString m_newObliqueAngleStr;
	CString m_newFontName;
	CString m_newBigfontName;		// EBATECH(CNBR)
	long	m_newFontStyle;
	int		m_newFontStyleIndex;
	int		m_newFontLangIndex;		// EBATECH(CNBR)
	int		m_newBackwards;
	int		m_newUpsideDown;
	int		m_newVertically;

	// actual values used in dialog except for doubles; see strings above
	// EBATECH(CNBR) Add CIcadFontComboBox::m_bigfontNameCtrl & CIcadFontLangComboBox::m_fontLangCtrl.
	//{{AFX_DATA(CExplorerPropsTab)
	enum { IDD = EXP_STYLE };
	CIcadStylePreview		m_previewCtrl;
	CEdit					m_widthCtrl;
	CButton					m_verticallyCtrl;
	CButton					m_upsideDownCtrl;
	CEdit					m_obliqueAngleCtrl;
	CEdit					m_heightCtrl;
	CButton					m_backwardsCtrl;
	CIcadFontComboBox		m_fontNameCtrl;
	CIcadFontStyleComboBox	m_fontStyleCtrl;
	CIcadFontLangComboBox	m_fontLangCtrl;
	CIcadFontComboBox		m_bigfontNameCtrl;
	int						m_backwards;
	double					m_height;
	CString					m_styleName;
	double					m_obliqueAngle;
	int						m_upsideDown;
	int						m_vertically;
	double					m_width;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExplorerPropsTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	afx_msg LONG OnFontNameChanged(UINT wParam, LONG lParam);
	afx_msg LONG OnFontStyleChanged(UINT wParam, LONG lParam);
	afx_msg LONG OnFontLangChanged(UINT wParam, LONG lParam); // EBATECH(CNBR)
	afx_msg LONG OnBigfontNameChanged(UINT wParam, LONG lParam);

	// Generated message map functions
	//{{AFX_MSG(CExplorerPropsTab)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_EXPLORERTEXTSTYLEPROPS_H)
