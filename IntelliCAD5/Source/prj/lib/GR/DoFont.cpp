#include "gr.h"
#include "DoFont.h"
#include "styletabrec.h"
#include "dwgcodepage.h"
#include "glyph.h"
#include "TTFInfoGetter.h"

/********************************************************************************
 * Author:	Vitaly Spirin
 * Purpose:	Obtain the DWGCODEPAGE identifier for the system
 *			and convert it to the character-set value.
 * Returns:	DWGCODEPAGE character set.
 ********************************************************************************/
BYTE GetDWGCharSet(db_drawing* pDrw)
{
    UINT	dwg_code_page;

	dwg_code_page = get_dwgcodepage(pDrw);

	switch(dwg_code_page)
	{
		case  932   :   return SHIFTJIS_CHARSET;
		case  949   :	return HANGUL_CHARSET;
		case  936   :	return GB2312_CHARSET;
		case  950   :	return CHINESEBIG5_CHARSET;
		case  1253  :	return GREEK_CHARSET;
		case  1254  :	return TURKISH_CHARSET;
		case  1255  :	return HEBREW_CHARSET;
		case  1256  :	return ARABIC_CHARSET;
		case  1257  :	return BALTIC_CHARSET;
		case  1251  :	return RUSSIAN_CHARSET;
		case  874   :	return THAI_CHARSET;
		case  1250  :	return EASTEUROPE_CHARSET;
		default     :	return ANSI_CHARSET;
	}
}

CDoFont::CDoFont(): m_pBackup(NULL)
{
}

CDoFont::~CDoFont()
{
	if(m_pBackup != NULL)
	{
		m_pBackup->DeleteObject();
		delete m_pBackup;
	}
}

bool CDoFont::createTrueTypeFont(db_entity*	    pTextEntity,
								 db_drawing*	pDrw,
								 CDC*		    pDC)
{
	ASSERT(pTextEntity && pDC && pDrw);

	if(!pTextEntity || !pDC || !pDrw)
		return false;

	char fontFamily[IC_ACADBUF];
	db_styletabrec*	pStyleRec = NULL;
	pTextEntity->get_7((db_handitem**)&pStyleRec);
	ASSERT(pStyleRec);

	if((!pStyleRec->getFontFamily(fontFamily)) || ('\0' == fontFamily[0]))
		strcpy(fontFamily, "Arial"/*DNT*/);		// if error, default to Arial

	long styleNumber;
	if(!pStyleRec->getFontStyleNumber(&styleNumber))
		styleNumber = ACAD_DEFAULTSTYLENUMBER;						// if error, default to Arial's default values

	LOGFONT logfont;
	CTTFInfoGetter::fontParamsByStyleNum(logfont.lfWeight, logfont.lfItalic, logfont.lfPitchAndFamily, logfont.lfCharSet, styleNumber);

	logfont.lfHeight		= - 2048;	// TT fonts are designed usually for this height (or for 1024)
	logfont.lfWidth			= 0;
	logfont.lfEscapement	= 0;
	logfont.lfOrientation	= 0;
	logfont.lfUnderline		= 0;
	logfont.lfStrikeOut		= 0;
	//logfont.lfCharSet		= GetDWGCharSet(pDrw); //DEFAULT_CHARSET; //GetSystemCharSet();		/*D.G.*/ EBATECH(CNBR) get from styleNumber
	logfont.lfOutPrecision	= OUT_TT_ONLY_PRECIS;
	logfont.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	logfont.lfQuality		= DEFAULT_QUALITY;
	strcpy(logfont.lfFaceName, fontFamily);			// make sure FontFamily isn't too large for lfFaceName

	if(!CreateFontIndirect(&logfont))
		return false;

	if(m_pBackup)
	{
		m_pBackup->DeleteObject();
		delete m_pBackup;
		m_pBackup = NULL;
	}
	/*D.G.*/// If a physical font haven't our CharSet then use ANSI_CHARSET because we should use
	// the requested font family but not created by GDI font mapper.
	bool bRecreateFont = false;
	CFont* pTmpFont = pDC->SelectObject(this);
	CString createdFontFamily(fontFamily);
	pDC->GetTextFace(createdFontFamily);
	if(createdFontFamily.CompareNoCase(fontFamily) != 0)
	{
		// DP: problems with font, probably drawing charset is not supported by this font
		// DP: enumerate all fonts in given family
		if(CTTFInfoGetter::get().fillFontsInfo(fontFamily, DEFAULT_CHARSET, pDC->GetSafeHdc()))
		{
			// DP: use font proposed by system mapper as backup
			GetLogFont(&logfont);
			m_pBackup = new CDoFont;
			m_pBackup->CreateFontIndirect(&logfont);
			// DP: well, found at least one , use first (this is subject to possible change)
			// DP: the most likely case is SYMBOL font
			memcpy(&logfont, &CTTFInfoGetter::get().fontInfo(0).elfLogFont, sizeof(LOGFONT));
			bRecreateFont = true;
		}
		// else
		// DP: there are no fonts in given family here, use proposed by system
	}

	pDC->SelectObject(pTmpFont);
	if(bRecreateFont)
	{
		DeleteObject();
		if(!CreateFontIndirect(&logfont))
			return false;
	}
	return true;
}

bool CDoFont::createBackup(UINT code, CDC* pDC)
{
	if(m_pBackup != NULL)
	{
		m_pBackup->DeleteObject();
		delete m_pBackup;
		m_pBackup = NULL;
	}

	if(!CTTFInfoGetter::get().fillFontsInfo(NULL, DEFAULT_CHARSET, pDC->GetSafeHdc()))
		return false;

	TTF_table fontTable;
	m_pBackup = new CDoFont;
	m_pBackup->CreateFontIndirect(&CTTFInfoGetter::get().fontInfo(0).elfLogFont);
	CDoFont* pOriginal = (CDoFont*)pDC->SelectObject(m_pBackup);
	if(fontTable.InitCMAP(pDC->GetSafeHdc()) && fontTable.FindGlyph(code) != 0)
	{
		pDC->SelectObject(pOriginal);
		return true;
	}
	for(int i = 1; i < CTTFInfoGetter::get().fontCount(); ++i)
	{
		pDC->SelectObject(pOriginal);
		m_pBackup->DeleteObject();
		m_pBackup->CreateFontIndirect(&CTTFInfoGetter::get().fontInfo(i).elfLogFont);
		pDC->SelectObject(m_pBackup);
		if(fontTable.InitCMAP(pDC->GetSafeHdc()) && fontTable.FindGlyph(code) != 0)
		{
			pDC->SelectObject(pOriginal);
			return true;
		}
	}
	
	pDC->SelectObject(pOriginal);
	m_pBackup->DeleteObject();
	delete m_pBackup;
	m_pBackup = NULL;
	return false;
}

CDoFont* CDoFont::Factory
(
 db_entity*	    pTextEntity,	// =>
 db_drawing*	pDrw,           // =>
 CDC*		    pDC				// =>
)
{
	CDoFont * pNewFont = new CDoFont ;
	if(pNewFont->createTrueTypeFont(pTextEntity, pDrw, pDC))
		return pNewFont;

	delete pNewFont;
	return NULL;
}

