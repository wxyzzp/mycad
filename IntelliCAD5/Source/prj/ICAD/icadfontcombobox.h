/* D:\ICADDEV\PRJ\ICAD\ICADFONTCOMBOBOX.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * $Revision: 1.3 $ $Date: 2001/07/24 14:59:03 $
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#if !defined _ICADFONTCOMBOBOX_H
#define _ICADFONTCOMBOBOX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TTFInfoGetter.h"
// IcadFontComboBox.h : header file
//

const int GET_FONT_TTF = 1;						// bit flags for type of fonts to list in combo box
const int GET_FONT_SHX = 2;
const int GET_FONT_SHP = 4;
const int GET_BIGFONT_SHX = 8;	// EBATECH(CNBR)

//WARNING!!!!!!! - Windows uses RASTER_FONTTYPE, DEVICE_FONTTYPE, and TRUETYPE_FONTTYPE to indicate the
//				   type of a font.	These are 0x0001, 0x002, and 0x004, respectively.  If Windows adds
//				   more types or changes their values, we may need to change the values below to ensure
//				   that we are using unique values for the CAD font types we support.
const int SHX_FONTTYPE = -16000;
const int SHP_FONTTYPE = -16001;
const int SHX_BIGFONTTYPE = -16002;	// EBATECH(CNBR)


struct FontItem
	{
	TCHAR fontName[LF_FACESIZE];
	DWORD fontType;
	LOGFONT logFont;	// WARNING!!!!!!! - always check fontType before trusting logFont
	};					// logFont will not be valid for shx and shp fonts

/////////////////////////////////////////////////////////////////////////////
// CIcadFontComboBox

class CIcadFontComboBox : public CComboBox
{
// Construction
public:
	// EBAECH(CNBR) -[ Add which parameter
	CIcadFontComboBox( int which = GET_FONT_SHX );
	//CIcadFontComboBox();
	// EBATECH(CNBR) ]-
	virtual ~CIcadFontComboBox();

// Attributes
public:
	enum
		{
		WM_FONT_NAME_CHANGED = (WM_USER + 1),
		WM_BIGFONT_NAME_CHANGED = (WM_USER + 2) // EBATECH(CNBR)
		};

// Operations
public:
	void FillFontComboBox(const int bitCode);
	void AddTrueTypeFonts(void);
	static int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX *enumLogFontEx,
		NEWTEXTMETRICEX *newTextMetricEx, int fontType, LPARAM lParam);
	void AddCADFonts(const CString& fileSpec, const int fontType);
	int AddFont(LOGFONT *logFont, DWORD fontType, char *fontName);
	void SetCurrentFont(const char* fontName);

	LPLOGFONT GetLogFont(int selection = -1);
	DWORD GetFontType(int selection = -1);
	FontItem * GetFontItem(const int selection = -1);
	bool HaveTrueTypeFont(void);
	void DeleteFontItems(void); // EBATECH(CNBR)

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadFontComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	//virtual void DeleteItem(LPDELETEITEMSTRUCT item);	// to do - maybe remove this

// Member variables
protected:
	int m_getWhichFonts;
	CString m_fontName;

// Generated message map functions
protected:
	//{{AFX_MSG(CIcadFontComboBox)
	afx_msg void OnSelChange();
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


/////////////////////////////////////////////////////////////////////////////
// CIcadFontComboBox inline functions


#endif // _ICADFONTCOMBOBOX_H



