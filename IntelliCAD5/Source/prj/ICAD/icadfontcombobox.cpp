/* D:\ICADDEV\PRJ\ICAD\ICADFONTCOMBOBOX.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 *
 * Abstract
 *
 * IcadFontComboBox.cpp : implementation file
 *
 */

#include "icad.h"/*DNT*/
#include "IcadFontComboBox.h"/*DNT*/
#include "paths.h"/*DNT*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIcadFontComboBox construction/destruction

/******************************************************************************
*+ CIcadFontComboBox::CIcadFontComboBox - constructor
*
*/
// EBATECH(CNBR) -[ Add which parameter.
CIcadFontComboBox::CIcadFontComboBox( int which )
: CComboBox()
	{
	m_getWhichFonts = which;

//	m_truetypeBitmap.LoadBitmap(AFX_IDB_TRUETYPE);
//	InitMaskFromBitmap(&m_truetypeBitmap, &m_maskBitmap);
	}
//CIcadFontComboBox::CIcadFontComboBox(): CComboBox()
//	{
//	m_getWhichFonts = GET_FONT_SHX;
//
////	m_truetypeBitmap.LoadBitmap(AFX_IDB_TRUETYPE);
////	InitMaskFromBitmap(&m_truetypeBitmap, &m_maskBitmap);
//	}
// EBATECH(CNBR) ]-
/******************************************************************************
*+ CIcadFontComboBox::~CIcadFontComboBox - destructor
*
*/

CIcadFontComboBox::~CIcadFontComboBox()
	{
	}


void CIcadFontComboBox::OnDestroy()
{
	// EBATECH(CNBR)
	//int count;
	//FontItem *fontItem;
	//
	// delete the fontItem attached to each entry
	//count = GetCount();
	//ASSERT(CB_ERR != count);
	//if (CB_ERR != count){
	//	for (int index = 0; index < count; index++){
	//		fontItem = (FontItem *) GetItemDataPtr(index);
	//		ASSERT(-1 != (int) fontItem);
	//		if (-1 != (int) fontItem){
	//			delete fontItem;
	//			fontItem = NULL;
	//		}
	//	}
	//}
	DeleteFontItems();
	CComboBox::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CIcadFontComboBox operations

/******************************************************************************
*+ CIcadFontComboBox::FillFontComboBox
*
*/

void CIcadFontComboBox::FillFontComboBox(
	const int bitCode)
{

	m_getWhichFonts = bitCode;

	// clear the list
	DeleteFontItems(); // EBATECH(CNBR)
	ResetContent();

	// add the appropriate fonts
	// EBATECH(CNBR) -[ add SHX and TTF
	if (m_getWhichFonts & GET_FONT_SHX)
		AddCADFonts("*.shx"/*DNT*/, SHX_FONTTYPE);
	//if (m_getWhichFonts & GET_FONT_SHP)
	//	AddCADFonts("*.shp"/*DNT*/, SHP_FONTTYPE);
	if (m_getWhichFonts & GET_BIGFONT_SHX)
		AddCADFonts("*.shx"/*DNT*/, SHX_BIGFONTTYPE);
	//if (m_getWhichFonts & GET_BIGFONT_SHP)
	//	AddCADFonts("*.shp"/*DNT*/, SHP_BIGFONTTYPE);
	if (m_getWhichFonts & GET_FONT_TTF)
		AddTrueTypeFonts();

	if( m_getWhichFonts == GET_BIGFONT_SHX)
		InsertString(0, ResourceString(IDC_EXPSTYLE_NO_BIGFONT, "(none)"));
	// EBATECH(CNBR) ]-
}

/******************************************************************************
*+ CIcadFontComboBox::AddTrueTypeFonts
*
*/

void CIcadFontComboBox::AddTrueTypeFonts(
	void)
	{

	// get all of the fonts in all of the character sets
	LOGFONT logFont;
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfFaceName[0] = '\0'/*DNT*/;
	logFont.lfPitchAndFamily = 0;

	// enumerate the fonts
	CClientDC dc(NULL);
	::EnumFontFamiliesEx(dc.GetSafeHdc(),
						 &logFont,
						 (FONTENUMPROC)CIcadFontComboBox::EnumFontFamExProc,
						 (LPARAM) this,
						 0);
	}

/******************************************************************************
*+ CIcadFontComboBox::EnumFontFamExProc
*
*/

int CALLBACK CIcadFontComboBox::EnumFontFamExProc(
	ENUMLOGFONTEX *enumLogFontEx,
	NEWTEXTMETRICEX *newTextMetricEx,
	int fontType,
	LPARAM lParam)
	{

	if (TRUETYPE_FONTTYPE & fontType)
		{
		((CIcadFontComboBox*)lParam)->AddFont(&enumLogFontEx->elfLogFont,
											  TRUETYPE_FONTTYPE,	//EBATECH(CNBR)
											  enumLogFontEx->elfLogFont.lfFaceName);
		}

	return 1;
	}

/******************************************************************************
*+ CIcadFontComboBox::AddCADFonts
*
*/

class FontCandidate : public AbstractFileProcessor
	{
public:
	FontCandidate( CIcadFontComboBox& combo, int fontType)
			: m_comboBox(combo), m_fontType( fontType) {}
	void ProcessFile( LPCTSTR, WIN32_FIND_DATA&);

protected:
	CIcadFontComboBox&	m_comboBox;
	int					m_fontType;
	int GetShxType( LPCTSTR pathname ); // EbaTech(CNBR)
	};

void
FontCandidate::ProcessFile( LPCTSTR path, WIN32_FIND_DATA& data)
{
	if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){	// skip directories
		// EBATECH(CNBR) -[ Check filetype
		//m_comboBox.AddFont(NULL, m_fontType, data.cFileName);
		CString fullName;
		fullName = CString(path) + CString(data.cFileName);
		if( m_fontType == GetShxType( (LPCTSTR)fullName )){
			m_comboBox.AddFont(NULL, m_fontType, data.cFileName);
		}
		// EBATECH(CNBR) ]-
	}
}

// EbaTech(CNBR) -[ GetShxType
int FontCandidate::GetShxType( LPCTSTR pathname )
{
	int rc = -1;
	HANDLE hfd;
	char lpBuff[32];
	DWORD nReadSize;
	if(( hfd = CreateFile( pathname,
		GENERIC_READ,
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL )) != INVALID_HANDLE_VALUE ){
		if( ReadFile( hfd, lpBuff, 32L, &nReadSize, NULL ) != 0 ){
			if(strnsame(lpBuff,"AutoCAD-86 shapes 1.0\r\n\x1A"/*DNT*/,23))
				//progesoft
				//if it return GET_FONT_SHX do not display in combobox nothing of old shx file
				//rc = GET_FONT_SHX;
				rc = SHX_FONTTYPE;
				//progesoft
			else if(strnsame(lpBuff,"AutoCAD-86 shapes 1.1\r\n\x1A"/*DNT*/,24))
				rc = SHX_FONTTYPE;
			else if(strnsame(lpBuff,"AutoCAD-86 unifont 1.0\r\n\x1A"/*DNT*/,25))
				rc = SHX_FONTTYPE;
			else if(strnsame(lpBuff,"AutoCAD-86 bigfont 1.0\r\n\x1A"/*DNT*/,25))
				rc = SHX_BIGFONTTYPE;
		}
		CloseHandle( hfd );
	}
	return rc;
}
// EbaTech(CNBR) ]-

void CIcadFontComboBox::AddCADFonts(
	const CString& fileSpec,
	const int fontType)
	{
		// will add files found in path
	FontCandidate	processor( *this, fontType);

	CMainWindow *mainWnd = (CMainWindow*) SDS_CMainWindow;// GetParentOwner();
	FilePath	fontPath( mainWnd->m_paths->m_fontPath);
	fontPath.FindFiles( processor, fileSpec);
	}

/******************************************************************************
*+ CIcadFontComboBox::AddFont
*
*/

int CIcadFontComboBox::AddFont(
	LOGFONT *logFont,			// i - will be NULL for shx and shp fonts
	DWORD fontType,				// i - always used
	char *fontName)				// i - always used
	{
	int index;
	FontItem *fontItem = NULL;

	// make sure the font isn't already in the combobox;
	// we want each font listed only once
	//Bugzilla No. 78415 ; 06-01-2003 [
	index = FindStringExact(-1, (LPCTSTR) fontName);
	//if (CB_ERR != FindStringExact(-1, (LPCTSTR) fontName))
	if (CB_ERR != index)
		return index;
	//Bugzilla No. 78415 ; 06-01-2003 ]

	// EbaTech(CNBR) -[ if we already have ???.shx then remove ???.ttf
	if (logFont){
		CString c( fontName );
		c += ".shx";
		if (CB_ERR != FindStringExact(-1, c))
			return CB_ERR;
	}
	// EBATECH(CNBR) ]-

	// set up the fontItem
	fontItem = new FontItem;
	ASSERT(fontItem);
	if (!fontItem)
		return CB_ERR;

	if (logFont)							// logFont will be NULL for shx and shp fonts
		fontItem->logFont = *logFont;

	fontItem->fontType = fontType;
	ASSERT(strlen(fontName) < LF_FACESIZE);
	strncpy(fontItem->fontName, fontName, LF_FACESIZE - 1);	// this has to be no more than LF_FACESIZE, including the NULL
	fontItem->fontName[LF_FACESIZE - 1] = '\0'/*DNT*/;

	// insert the font name and set the fontItem
	index = AddString((LPCTSTR) fontItem->fontName);
	if (index < 0)	// failed to add
		{
		delete fontItem;
		return CB_ERR;
		}
	else			// successfully added
		{
		if (CB_ERR == SetItemDataPtr(index, (void *) fontItem))
			{
			DeleteString(index);			// remove the font name if we can't store its data
			delete fontItem;
			return CB_ERR;
			}
		}

	return index;
	}

/******************************************************************************
*+ CIcadFontComboBox::SetCurrentFont
*
*  NOTE - In a dialog that handles multiple styles, the current font may be
*		  IDC_IEXP_VARIES_S.
*
*/

void CIcadFontComboBox::SetCurrentFont(
	const char* fontName)
	{
	CString tmpName = (strrchr(fontName, '\\')) ? strrchr(fontName, '\\') + 1 : fontName;	// make sure we just have the font name - no path
	m_fontName = tmpName;	// to do - do we want the full path here or just the font filename???????
	if(tmpName.Compare("") == 0){
		tmpName = ResourceString(IDC_EXPSTYLE_NO_BIGFONT, "(none)");
	}
	int index = FindStringExact(-1, tmpName);
	if (CB_ERR == index)
		{
			// to do - may have to find out if the current style's font is in the list; if it isn't, we probably
			//		   have to add it so that it can be displayed and selected - preview should say "Font not found"
		if (CB_ERR == SetCurSel(0))		// remove this when the above is done
			EnableWindow(FALSE);
		}
	else
		{
		if (CB_ERR == SetCurSel(index))
			EnableWindow(FALSE);
		}

	OnSelChange();
	}

/******************************************************************************
*+ CIcadFontComboBox::GetLogFont
*
*/

LPLOGFONT CIcadFontComboBox::GetLogFont(
	int selection)
	{
	return &GetFontItem(selection)->logFont;
	}

/******************************************************************************
*+ CIcadFontComboBox::GetFontType
*
*/

DWORD CIcadFontComboBox::GetFontType(
	int selection)
	{
	FontItem *fontItem;

	fontItem = GetFontItem(selection);
	if (!fontItem)
		return CB_ERR;
	if (-1 != (int) fontItem)					// -1 == failed to get FontItem
		return fontItem->fontType;
	else
		return CB_ERR;
	}

/******************************************************************************
*+ CIcadFontComboBox::GetFontItem
*
*/

FontItem * CIcadFontComboBox::GetFontItem(
	const int selection)
	{
	int tmpSelection = selection;

	if (-1 == tmpSelection)
		tmpSelection = GetCurSel();

	if ((-1 == tmpSelection) || (CB_ERR == tmpSelection))
		{
		CString str;
		GetWindowText(str);
		tmpSelection = FindString(-1, str);
		if (CB_ERR == tmpSelection)
			tmpSelection = 0;
		}

	ASSERT(-1 != (int) GetItemDataPtr(tmpSelection));
	return (FontItem *) GetItemDataPtr(tmpSelection);
	}

/******************************************************************************
*+ CIcadFontComboBox::HaveTrueTypeFont
*
*/

bool CIcadFontComboBox::HaveTrueTypeFont(
	void)
	{

	DWORD fontType = GetFontType(GetCurSel());
	if(TRUETYPE_FONTTYPE == fontType)
		return true;
	else
		return false;
//	if ((CB_ERR == fontType) ||
//		(SHX_BIGFONTTYPE == fontType) ||
//		(SHP_BIGFONTTYPE == fontType) ||
//		(SHX_FONTTYPE == fontType) ||
//		(SHP_FONTTYPE == fontType))
//		return false;
//	else
//		return true;
	}


// EBATECH(CNBR) -[ Delete each item proc.
/******************************************************************************
*+ CIcadFontComboBox::DeleteItem
*
*/
//
//void CIcadFontComboBox::DeleteItem(
//	LPDELETEITEMSTRUCT item)
//	{
//	FontItem *fontItem;
//
//	if (-1 == item->itemID)
//		return;
//
//	ASSERT(ODT_COMBOBOX == item->CtlType);
//
//	fontItem = GetFontItem(item->itemID);
//
//	// free the FontItem created in CIcadFontComboBox::AddFont()
//	if (-1 != (int) fontItem)
//		delete fontItem;
//	}
// EBATECH(CNBR) -[ Delete Clear proc.
void CIcadFontComboBox::DeleteFontItems(
	void)
{
	int count;
	FontItem *fontItem;

	// delete the fontItem attached to each entry
	count = GetCount();
	ASSERT(CB_ERR != count);
	if (CB_ERR != count){
		for (int index = 0; index < count; index++){
			fontItem = (FontItem *) GetItemDataPtr(index);
			ASSERT(-1 != (int) fontItem);
			if (-1 != (int) fontItem){
				delete fontItem;
				fontItem = NULL;
				SetItemDataPtr(index,NULL); // EBATECH(CNBR)
			}
		}
	}
}
// EBATECH(CNBR) ]-


BEGIN_MESSAGE_MAP(CIcadFontComboBox, CComboBox)
	//{{AFX_MSG_MAP(CIcadFontComboBox)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelChange)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIcadFontComboBox message handlers

void CIcadFontComboBox::OnSelChange()
{
	// update preview and other controls
	// EBATECH(CNBR) -[ for Bigfont control
	if( m_getWhichFonts != GET_BIGFONT_SHX ){
		GetParent()->SendMessage(WM_FONT_NAME_CHANGED);
	}else{
		GetParent()->SendMessage(WM_BIGFONT_NAME_CHANGED);
	}
	//GetParent()->SendMessage(WM_FONT_NAME_CHANGED);
	// EBAETCH(CNBR) ]-
}


