/* D:\ICADDEV\PRJ\ICAD\ICADSTYLEPREVIEW.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Helper class for the IntelliCAD Explorer Styles Properties tab and the
 * Style command's dialog.  Displays a sample of the specified font
 * with the specified attributes.
 * 
 */ 

#include "icad.h"				/*DNT*/
#include "IcadStylePreview.h"	/*DNT*/
#include "icadfontcombobox.h"	/*DNT*/
#include "TextStyleDlg.h"		/*DNT*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CIcadStylePreview construction/destruction

/******************************************************************************
*+ CIcadStylePreview::CIcadStylePreview - constructor
*
*/

CIcadStylePreview::CIcadStylePreview()
	{
	m_fontType		= CB_ERR;				// CB_ERR will cancel preview and blank preview control
	
	m_width			= 1.0;
	m_obliqueAngle	= 0.0;

	m_backwards		= FALSE;
	m_upsideDown	= FALSE;
	m_vertically	= FALSE;

	m_fontItem		= NULL;
	}

/******************************************************************************
*+ CIcadStylePreview::CIcadStylePreview - destructor
*
*/

CIcadStylePreview::~CIcadStylePreview()
	{
	}



/////////////////////////////////////////////////////////////////////////////
// CIcadStylePreview implementation

/******************************************************************************
*+ CIcadStylePreview::DrawStyleFontPreview
*
*/

void CIcadStylePreview::DrawStyleFontPreview(
	void)
	{

	if ((CB_ERR == m_fontType) || (NULL == m_fontItem))
		{
		SetWindowText(""/*DNT*/);				// on error, blank Preview window
		return;
		}

	if ((SHX_FONTTYPE == m_fontType) || (SHP_FONTTYPE == m_fontType))
		DrawCADFontPreview();
	else
		DrawTTFFontPreview();
	}

/******************************************************************************
*+ CIcadStylePreview::DrawCADFontPreview
*
*/
// TO DO. We don't support shp/shx fonts in the preview now (So, out a message with Arial style).
void CIcadStylePreview::DrawCADFontPreview(
	void)
	{
	ASSERT(m_fontItem);
	if (NULL == m_fontItem)
		{
		SetWindowText(""/*DNT*/);					// on error, blank Preview window
		return;
		}

	// use a "standard" height for each font
	// all the previews will be based on MS Sans Serif on the current device
	CClientDC dc(this);
	int fontHeight = -((dc.GetDeviceCaps(LOGPIXELSY) * PREVIEW_FONT_SIZE) / FONT_POINTS_PER_INCH);
	
	CFont tmpFont;
	tmpFont.CreateFont(fontHeight, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
					   OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
					   DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif"/*DNT*/);

	CFont *oldFont = dc.SelectObject(&tmpFont);
	TEXTMETRIC textMetric;
	dc.GetTextMetrics(&textMetric);
	fontHeight = textMetric.tmHeight + textMetric.tmExternalLeading;
	dc.SelectObject(oldFont);
	tmpFont.DeleteObject();

	fontHeight *= 3; fontHeight /= 4;	// remove it when we'll support shx/shp display
	// create the font actually used for the preview
	m_previewFont.Detach();
	if (!m_previewFont.CreateFont(-fontHeight,
								0,
								0,
								0,
								FW_THIN,
								0,
								0,
								0,
								DEFAULT_CHARSET,
								OUT_TT_PRECIS,
								CLIP_CHARACTER_PRECIS,
								DEFAULT_QUALITY,
								34,
								"Arial"))
		{
		SetWindowText(""/*DNT*/);		// on error, blank Preview window
		return;
		}

	// display the font's name
	SetFont(&m_previewFont);
	SetWindowText("SHX/SHP display not supported");
	}

/******************************************************************************
*+ CIcadStylePreview::DrawTTFFontPreview
*
*/

void CIcadStylePreview::DrawTTFFontPreview(
	void)
	{
	ASSERT(m_fontItem);
	if (NULL == m_fontItem)
		{
		SetWindowText(""/*DNT*/);					// on error, blank Preview window
		return;
		}

	// use a "standard" height for each font
	// all the previews will be based on MS Sans Serif on the current device
	CClientDC dc(this);
	int fontHeight = -((dc.GetDeviceCaps(LOGPIXELSY) * PREVIEW_FONT_SIZE) / FONT_POINTS_PER_INCH);
	
	CFont tmpFont;
	tmpFont.CreateFont(fontHeight, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
					   OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
					   DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif"/*DNT*/);

	CFont *oldFont = dc.SelectObject(&tmpFont);
	TEXTMETRIC textMetric;
	dc.GetTextMetrics(&textMetric);
	fontHeight = textMetric.tmHeight + textMetric.tmExternalLeading;
	dc.SelectObject(oldFont);
	tmpFont.DeleteObject();

	// create the font actually used for the preview
	int fontWidth;
	bool found = true;  // did we find the font?
	if (m_fontItem->logFont.lfHeight <= 0 || m_fontItem->logFont.lfHeight > 1000000 ||
		m_fontItem->logFont.lfWidth <= 0 || m_fontItem->logFont.lfWidth > 1000000)	// error
		{
		fontWidth = (int) (m_width * textMetric.tmAveCharWidth);
		found = false;
		}
	else
		fontWidth = (int) (m_width * (fontHeight * ((double) (m_fontItem->logFont.lfWidth) / (double) (m_fontItem->logFont.lfHeight))));
	// EBATECH(CNBR)
	//int weight = ((CTextStyleDlg*) GetParent())->m_weight;
	//bool italic = ((CTextStyleDlg*) GetParent())->m_italic;
	//if (weight > 1000)
	//	{
	//	weight = ((CExplorerPropsTab*) GetParent())->m_weight;
	//	italic = ((CExplorerPropsTab*) GetParent())->m_italic;
	//	}
	int weight = FW_NORMAL;
	bool italic = false;
	if( GetParent()->IsKindOf(RUNTIME_CLASS( CTextStyleDlg ))){
		weight = ((CTextStyleDlg*) GetParent())->m_weight;
		italic = ((CTextStyleDlg*) GetParent())->m_italic;
	}else if( GetParent()->IsKindOf(RUNTIME_CLASS( CExplorerPropsTab ))){
		weight = ((CExplorerPropsTab*) GetParent())->m_weight;
		italic = ((CExplorerPropsTab*) GetParent())->m_italic;
	}
	// EBATECH(CNBR)
	m_previewFont.Detach();
	if (!m_previewFont.CreateFont(-fontHeight,
								fontWidth,
								0,
								0,
								weight,
								italic,
								0,
								0,
								m_fontItem->logFont.lfCharSet,
								OUT_TT_PRECIS,
								CLIP_CHARACTER_PRECIS,
								DEFAULT_QUALITY,
								m_fontItem->logFont.lfPitchAndFamily,
								m_fontItem->fontName))
		{
		SetWindowText(""/*DNT*/);					// on error, blank Preview window
		return;
		}

	// display the font's name
	SetFont(&m_previewFont);
	if (found)
		SetWindowText(m_fontItem->fontName);
	else
		SetWindowText("Not on system");
	}



BEGIN_MESSAGE_MAP(CIcadStylePreview, CStatic)
	//{{AFX_MSG_MAP(CIcadStylePreview)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIcadStylePreview message handlers
