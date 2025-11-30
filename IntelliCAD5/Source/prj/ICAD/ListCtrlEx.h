/* ICAD\LISTCTRLEX.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 *	Definition of CListCtrlEx class - an extended list control class that provides editable sub-items
 *
 */

#pragma once

#include "icad.h"
#include "IcadFontComboBox.h"/*DNT*/
#include "IcadFontStyleCombo.h"/*DNT*/
#include "IcadFontLangCombo.h"/*DNT*/
#include "IcadLWCombo.h"/*DNT*/


int GetComboBoxLongestStrWidth(CComboBox *comboBox);

const int STYLE_FONT_NAME_COLUMN = 4;

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx window

class CInPlaceFontNameCombo;
class CInPlaceFontStyleCombo;
class CInPlaceFontLangCombo;
class CInPlaceLWCombo;

class CListCtrlEx : public CListCtrl
{
// Construction
public:
	CListCtrlEx();

// Attributes
public:
	enum EHighlight {HIGHLIGHT_NORMAL, HIGHLIGHT_ALLCOLUMNS, HIGHLIGHT_ROW};
protected:
	int  m_nHighlight;		// Indicate type of selection highlighting
	CEdit *m_pEdit;

// Operations
public:
	int HitTestEx (CPoint &point, int *nCol);
	CEdit * EditSubLabel (int nItem, int nCol);

	CComboBox * ShowInPlaceList (int nItem, int nCol, CStringList &lstItems, int nSel);
	//EBATECH(CNBR) -[ Add Which for FontName remove fontStyle from StyleCombo
	//CInPlaceFontNameCombo * ShowInPlaceFontNameCombo(int item, int col, const char* fontName);
	//CInPlaceFontStyleCombo * ShowInPlaceFontStyleCombo(int item, int col, const char* fontStyle,
	//												   long stylenumber = 0);
	CInPlaceFontNameCombo * ShowInPlaceFontNameCombo(int item, int col, const char* fontName, int which);
	CInPlaceFontStyleCombo * ShowInPlaceFontStyleCombo(int item, int col, long stylenumber = 0);
	CInPlaceFontLangCombo * ShowInPlaceFontLangCombo(int item, int col, long stylenumber = 0);
	//EBATECH(CNBR) ]-
	CInPlaceLWCombo * ShowInPlaceLWCombo(int item, int col, int lwunits, int celweight, int bitcode );

	int ToggleSubLabel (int nItem, int nCol, CString option1, CString option2);
	void RepaintSelectedItems();
	int SetHighlightType(EHighlight hilite);
	void EndLabelEditing ();

	// Overridables
	virtual void DrawSubItem(int nItem, int nCol, LV_COLUMN& lvc, CRect& rcCol, CDC *pDC);

	// Overrides from CListCrtl
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlEx)
	//}}AFX_VIRTUAL
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

// Implementation
public:
	virtual ~CListCtrlEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlEx)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg void OnKillFocus(CWnd *pNewWnd);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit window

class CInPlaceEdit : public CEdit
{
// Construction
public:
	CInPlaceEdit(int iItem, int iSubItem, CString sInitText);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceEdit)
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	int m_iItem;
	int m_iSubItem;
	CString m_sInitText;
	BOOL m_bESC;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CInPlaceList window

class CInPlaceList : public CComboBox
{
// Construction
public:
	CInPlaceList(int iItem, int iSubItem, CStringList *plstItems, int nSel);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceList)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceList)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnCloseup();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	int 	m_iItem;
	int 	m_iSubItem;
	CStringList m_lstItems;
	int 	m_nSel;
	BOOL	m_bESC;				// To indicate whether ESC key was pressed
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CInPlaceFontNameCombo window

class CInPlaceFontNameCombo : public CIcadFontComboBox
	{
// Construction
public:
	CInPlaceFontNameCombo(int item, int subItem, const char *fontName, int which);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceFontNameCombo)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceFontNameCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceFontNameCombo)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnCloseup();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	CString	m_fontName;
	int 	m_item;
	int 	m_subItem;
//	int 	m_nSel;
	bool	m_ESC;				// indicates whether or not ESC key was pressed
	int		m_which;	// EBATECH(CNBR) Both standard font and bigfont
	};


// to do - consolidate all of the combo box code so that all of the little functions
//		   are not repeated several times


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CInPlaceFontStyleCombo window
// EBATECH(CNBR) remove fontStyle it is never used.

class CInPlaceFontStyleCombo : public CIcadFontStyleComboBox
	{
// Construction
public:
	//CInPlaceFontStyleCombo(int item, int subItem, const char *fontName, const char *fontStyle);
	CInPlaceFontStyleCombo(int item, int subItem, const char *fontName);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceFontStyleCombo)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceFontStyleCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceFontStyleCombo)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnCloseup();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	CString	m_fontName;
//	CString	m_fontStyle;	EBATECH(CNBR)
	int 	m_item;
	int 	m_subItem;
//	int 	m_nSel;
	bool	m_ESC;				// indicates whether or not ESC key was pressed
	};


/////////////////////////////////////////////////////////////////////////////
// EBATECH(CNBR) -[
/////////////////////////////////////////////////////////////////////////////
// CInPlaceFontLangCombo window

class CInPlaceFontLangCombo : public CIcadFontLangComboBox
	{
// Construction
public:
	CInPlaceFontLangCombo(int item, int subItem, const char *fontName );

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceFontLangCombo)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceFontLangCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceFontLangCombo)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnCloseup();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	CString	m_fontName;
	int 	m_item;
	int 	m_subItem;
//	int 	m_nSel;
	bool	m_ESC;				// indicates whether or not ESC key was pressed
	};


/////////////////////////////////////////////////////////////////////////////
// EBATECH(CNBR) ]-
/////////////////////////////////////////////////////////////////////////////
// CInPlaceLWCombo window

class CInPlaceLWCombo : public CIcadLWComboBox
	{
// Construction
public:
	CInPlaceLWCombo(int item, int subItem, int lwunits, int celweight, int bitcode );

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceLWCombo)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceLWCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceLWCombo)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnCloseup();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	int		m_lwunits;
	int		m_celweight;
	int		m_bitcode;
	bool	m_ESC;				// indicates whether or not ESC key was pressed
	int 	m_Item;
	int 	m_subItem;
	};


/////////////////////////////////////////////////////////////////////////////
// EBATECH(CNBR) ]-
