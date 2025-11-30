/* PRJ\ICAD\TEXTSTYLEDLG.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 *
 * Abstract
 *
 * Implements the Style command's dialog.
 *
 */


#if !defined(_TEXTSTYLEDLG_H)
#define _TEXTSTYLEDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "IcadFontComboBox.h"	/*DNT*/
#include "icadfontstylecombo.h"	/*DNT*/
#include "IcadFontLangCombo.h"	/*DNT*/// EBATECH(CNBR)
#include "IcadStylePreview.h"	/*DNT*/
#include "TextStyleTableInfo.h"	/*DNT*/
#include "IcadExplorer.h"		/*DNT*/
#include "styletabrec.h"		/*DNT*/


const int DIRTY_NONE					= 0;
const int DIRTY_HEIGHT					= 1;
const int DIRTY_WIDTH					= 2;
const int DIRTY_OBLIQUE					= 4;
const int DIRTY_BACKWARDS				= 8;
const int DIRTY_UPSIDE_DOWN				= 16;
const int DIRTY_VERTICALLY				= 32;
const int DIRTY_FONT_NAME				= 64;
const int DIRTY_FONT_STYLE				= 128;
const int DIRTY_BIGFONT_NAME			= 256; // EBATECH(CNBR)
const int DIRTY_FONT_LANGUAGE			= 512; // EBATECH(CNBR)

const sds_real MIN_TEXT_HEIGHT			= 0.0;
const sds_real MIN_WIDTH_FACTOR			= 0.01;
const sds_real MAX_WIDTH_FACTOR			= 100.0;
const int MIN_OBLIQUE_ANGLE				= -85;
const int MAX_OBLIQUE_ANGLE				= 85;

const int TEXT_STYLE_DELETE_SUCCEEDED	= 1;
const int TEXT_STYLE_DELETE_FAILED_OK	= 0;
const int TEXT_STYLE_DELETE_FAILED_BAD	= -1;


/////////////////////////////////////////////////////////////////////////////
// CTextStyleDlg dialog
/* EBATECH(CNBR) 2001-06-14 Add m_charSet, m_bigfontNameCtrl, m_fontLangCtrl
	 and corresponging message functions */

class CTextStyleDlg : public CDialog
{
	DECLARE_DYNAMIC( CTextStyleDlg )
public:
	// Construction
	CTextStyleDlg(CWnd* pParent = NULL);   // standard constructor

	// Implementation
	void FillStylesComboBox(const char *defaultStyle);
	void GetTextStyleValues(db_styletabrec *styleRec);
	void DisplayTextStyleValues(const char *styleName);
	void DrawFontPreview(void);
	void UpdateDirtyFlag(const sds_real origValue, const sds_real newValue, const int flagValue);
	void UpdateDirtyFlag(const CString& origValue, const CString& newValue, const int flagValue);
	void UpdateDirtyFlag(const BOOL origValue, const BOOL newValue, const int flagValue);
	void UpdateDirtyFlag(const long origValue, const long newValue, const int flagValue);
	inline void SetApplyButton(void);
	bool ConvertObliqueAngle(sds_real& value);
	bool PromptToSaveStyle(void);
	void SaveTextStyle(void);
	void GetStyleValuesFromDialog(CTextStyleTableInfo& values);
	void RestoreTextStyleValues(CTextStyleTableInfo& originalValues, CTextStyleTableInfo& newValues);
	void CreateNewTextStyle(CTextStyleTableInfo& values);
	bool StyleOKToDelete(const CString& styleName);
	bool StyleOKToRename(const CString& styleName);
	int GetTextStyleDeletePlan(const CString& styleToDelete, CString& newTextStyle);
	int DeleteTextStyle(db_drawing *drawing, const int mode, CString& oldStyle, CString& newStyle);
	void AddFontName(void);
	void AddBigFontName(void);
	void GetStyleSel(void);

	// Dialog Data
	//{{AFX_DATA(CTextStyleDlg)
	enum { IDD = TEXT_STYLE };
	CButton					m_applyCtrl;
	CIcadStylePreview		m_previewCtrl;
	CIcadFontStyleComboBox	m_fontStyleCtrl;
	CIcadFontComboBox		m_fontNameCtrl;
	CButton					m_verticallyCtrl;
	CButton					m_upsideDownCtrl;
	CButton					m_backwardsCtrl;
	CEdit					m_obliqueAngleCtrl;
	CEdit					m_widthCtrl;
	CEdit					m_heightCtrl;
	CButton					m_deleteCtrl;
	CButton					m_renameCtrl;
	CComboBox				m_styleNameCtrl;
	CIcadFontComboBox		m_bigfontNameCtrl;
	CIcadFontLangComboBox	m_fontLangCtrl;
	//}}AFX_DATA

	int						m_dirtyFlag;
	int						m_weight;
	bool					m_italic;
	int						m_charSet;

	bool					m_bNeedRegen;	/*DG - 9.1.2002*/

	CTextStyleTableInfo		m_tableInfo;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextStyleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	afx_msg LONG OnFontNameChanged(UINT wParam, LONG lParam);
	afx_msg LONG OnFontStyleChanged(UINT wParam, LONG lParam);
	afx_msg LONG OnBigfontNameChanged(UINT wParam, LONG lParam);
	afx_msg LONG OnFontLangChanged(UINT wParam, LONG lParam);

	// Generated message map functions
	//{{AFX_MSG(CTextStyleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnStyleNew();
	afx_msg void OnStyleRename();
	afx_msg void OnStyleDelete();
	afx_msg void OnApply();
	virtual void OnOK();
	afx_msg void OnHelp();
	afx_msg void OnSelChangeStyleName();
	afx_msg void OnChangeStyleHeight();
	afx_msg void OnChangeStyleWidth();
	afx_msg void OnChangeStyleOblique();
	afx_msg void OnKillFocusStyleHeight();
	afx_msg void OnKillFocusStyleWidth();
	afx_msg void OnKillFocusStyleOblique();
	afx_msg void OnStyleBackwards();
	afx_msg void OnStyleUpsideDown();
	afx_msg void OnStyleVertically();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};




/////////////////////////////////////////////////////////////////////////////
// CTextStyle_IEXP_Newdlg dialog
//
// NOTE - Needed to inherit from IEXP_Newdlg so that we could override
//		  OnInitDialog() to avoid using IntelliCAD Explorer global variables.
//

class CTextStyle_IEXP_Newdlg : public IEXP_Newdlg
	{
	protected:
		afx_msg BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	};




/////////////////////////////////////////////////////////////////////////////
// CTextStyle_IEXP_MessItem dialog
//
// NOTE - Needed to inherit from IEXP_MessItem so that we could include a
//		  dialog inherited from IEXP_Newdlg and override that dialog's
//		  OnInitDialog() to avoid using IntelliCAD Explorer global variables.
//

class CTextStyle_IEXP_MessItem : public IEXP_MessItem
	{
	public:
		CTextStyle_IEXP_Newdlg m_txtStyleNewDlg;

	protected:
		BOOL OnInitDialog();
		afx_msg void OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized);
		afx_msg void OnChangeStyle();
		afx_msg void OnWindowPosChanged(WINDOWPOS *lpwndpos);

	DECLARE_MESSAGE_MAP()
	};




//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_TEXTSTYLEDLG_H)


