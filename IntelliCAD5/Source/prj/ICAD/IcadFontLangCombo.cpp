// IcadFontLangComboBox.cpp : implementation file
//

#include "icad.h"
#include "IcadFontLangCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIcadFontLangComboBox construction/destruction

/******************************************************************************
*+ CIcadFontLangComboBox::CIcadFontLangComboBox - constructor
*
*/

CIcadFontLangComboBox::CIcadFontLangComboBox()
: CComboBox()
	{
	}

/******************************************************************************
*+ CIcadFontLangComboBox::~CIcadFontLangComboBox - destructor
*
*/

CIcadFontLangComboBox::~CIcadFontLangComboBox()
	{
	}

/////////////////////////////////////////////////////////////////////////////
// CIcadFontLangComboBox operations

/******************************************************************************
*+ CIcadFontLangComboBox::FillTrueTypeLangs
*
*/

void CIcadFontLangComboBox::FillTrueTypeLangs(
	const char *fontName,
	const long fontStyle)
	{
	LOGFONT logFont;

	// clear the current list
	ResetContent();

	ASSERT(fontName);
	if (NULL == fontName)
		strcpy(logFont.lfFaceName, "Arial"/*DNT*/);	// should be safe; ran into problems just returning
	else
		{
		// we had a problem with empty font names;
		// though they should never occur, we'll trap
		// for them

		CString tmpFontName = fontName;
		tmpFontName.TrimLeft();
		if (tmpFontName.IsEmpty())
			strcpy(logFont.lfFaceName, "Arial"/*DNT*/);	// should be safe; ran into problems just returning
		else
			strcpy(logFont.lfFaceName, fontName);
		}

	// enumerate all of the specified font's related fonts
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfPitchAndFamily = 0;

	CClientDC dc(NULL);
	::EnumFontFamiliesEx(dc.GetSafeHdc(),
						 &logFont,
						 (FONTENUMPROC)CIcadFontLangComboBox::EnumFontFamExProcLang,
						 (LPARAM) this,
						 0);

	int tmpInt = GetCount();
	int charSet = ((fontStyle & 0x0000ff00)>>8);

	for( int index = 0 ; index < tmpInt ; index++ ){
		if( charSet == (int)GetItemData(index)){
			if (CB_ERR == SetCurSel(index)){
				EnableWindow(TRUE);
				return;
			}
		}
	}
	// set to the current language

	if (CB_ERR == SetCurSel(0))
		EnableWindow(FALSE);

}

/******************************************************************************
*+ CIcadFontLangComboBox::EnumFontFamExProcLang
*
*/

int CALLBACK CIcadFontLangComboBox::EnumFontFamExProcLang(
	ENUMLOGFONTEX *enumLogFontEx,
	NEWTEXTMETRICEX *newTextMetricEx,
	int fontType,
	LPARAM lParam)
	{
	if (TRUETYPE_FONTTYPE & fontType)
		{
		((CIcadFontLangComboBox*)lParam)->AddLang((LPCTSTR) enumLogFontEx->elfScript,
			0x000000ff & enumLogFontEx->elfLogFont.lfCharSet);
		}

	return 1;
	}

/******************************************************************************
*+ CIcadFontLangComboBox::AddLang
*
*/

int CIcadFontLangComboBox::AddLang(
	const char *langName,
	const int charSet)
	{
	int index;

	ASSERT(langName);

	// make sure the style isn't already in the combobox;
	// we want each style listed only once
	if (CB_ERR != FindStringExact(-1, (LPCTSTR) langName))
		return CB_ERR;

	// insert the style name and set the styleItem
	index = AddString(langName);
	if (index < 0)	// failed to add
		{
		return CB_ERR;
		}
	else			// successfully added
		{
		if (CB_ERR == SetItemData(index, (DWORD)charSet))
			{
			DeleteString(index);	// remove the style name if we can't store its data
			return CB_ERR;
			}
		}

	return index;
	}




BEGIN_MESSAGE_MAP(CIcadFontLangComboBox, CComboBox)
	//{{AFX_MSG_MAP(CIcadFontLangComboBox)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelChange)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIcadFontLangComboBox message handlers

void CIcadFontLangComboBox::OnSelChange()
{
	// update preview and other controls
	GetParent()->SendMessage(WM_FONT_LANG_CHANGED);
}

void CIcadFontLangComboBox::OnDestroy()
{
	CComboBox::OnDestroy();
}
