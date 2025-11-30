/* D:\ICADDEV\PRJ\ICAD\ICADSTYLEPREVIEW.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Helper class for the IntelliCAD Explorer Styles Properties tab and the
 * Style command's dialog.  Displays a sample of the specified font
 * with the specified attributes.
 * 
 */ 

#if !defined _ICADSTYLEPREVIEW_H
#define _ICADSTYLEPREVIEW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



const int PREVIEW_FONT_SIZE = 10;
const int FONT_POINTS_PER_INCH = 72;



/////////////////////////////////////////////////////////////////////////////
// CIcadStylePreview window

struct FontItem;

class CIcadStylePreview : public CStatic
	{
	// Construction/Destruction
public:
	CIcadStylePreview();
	virtual ~CIcadStylePreview();

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadStylePreview)
	//}}AFX_VIRTUAL

	// Implementation
public:
	void DrawStyleFontPreview(void);
	void DrawCADFontPreview(void);
	void DrawTTFFontPreview(void);

	// Data
public:
	int m_fontType;
	sds_real m_width;
	sds_real m_obliqueAngle;

	int m_backwards;
	int m_upsideDown;
	int m_vertically;

	FontItem *m_fontItem;

	// Generated message map functions
protected:
	//{{AFX_MSG(CIcadStylePreview)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	CFont	m_previewFont;	// It's for CWnd::SetFont used in DrawXXXFontPreview methods.
							// We don't use some CFont variable locally in these methods
							// because SetFont doesn't want to have its CFont parameter
							// to be destroyed "too early" (thanks to Microsoft:)

	};

/////////////////////////////////////////////////////////////////////////////

#endif // _ICADSTYLEPREVIEW_H
