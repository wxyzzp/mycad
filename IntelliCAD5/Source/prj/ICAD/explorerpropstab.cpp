/* explorerpropstab.cpp
 * Copyright (C) 1997-1999 Visio Corporation. All Rights Reserved.
 *
 * Abstract
 *
 * Implements the tab for the Styles Properties in IntelliCAD Explorer.
 *
 */



// to do - the error checking on entered values needs to be made much more robust here,
//		   in the other properties tabs, and in IntelliCAD Explorer in general; currently,
//		   the code allows you to enter bogus values in several places



#include "icad.h"				/*DNT*/
#include "ExplorerPropsTab.h"	/*DNT*/
#include "IcadExplorer.h"		/*DNT*/
#include "resource.hm"			/*DNT*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExplorerPropsTab dialog construction/destruction

IMPLEMENT_DYNAMIC( CExplorerPropsTab, CDialog )

/******************************************************************************
*+ CExplorerPropsTab::CExplorerPropsTab - constructor
*/

CExplorerPropsTab::CExplorerPropsTab(
	CWnd* pParent) /*=NULL*/
	{

	m_inTextStyleProperties		= false;
	m_hasTrueTypeFont			= false;

	m_hasTrueTypeFontVaries		= false;
	m_backwardsVaries			= false;
	m_fontFilenameVaries		= false;
	m_fontFamilyVaries			= false;
	m_fontStyleVaries			= false;
	m_bigfontFilenameVaries		= false;	// EBATECH(CNBR)
	m_heightVaries				= false;
	m_styleNameVaries			= false;
	m_obliqueAngleVaries		= false;
	m_upsideDownVaries			= false;
	m_verticallyVaries			= false;
	m_widthVaries				= false;

	// let wizard initialize the values; don't edit these,
	// because the wizard may change them back
	//{{AFX_DATA_INIT(CExplorerPropsTab)
	m_backwards = FALSE;
	m_height = 0.0;
	m_styleName = _T("");
	m_obliqueAngle = 0.0;
	m_upsideDown = FALSE;
	m_vertically = FALSE;
	m_width = 0.0;
	//}}AFX_DATA_INIT

	// really initialize the members to what we want;
	// do this after the wizard initializes the values
	m_styleName		= _T("");
	m_fontFilename		= _T(""/*DNT*/);
	m_fontFamily		= _T(""/*DNT*/);
	m_bigfontFilename		= _T(""/*DNT*/); // EBATECH(CNBR)

	m_fontStyle = 34;						// default to value for Arial Regular

	m_newFontStyleIndex = m_newFontLangIndex = -1; // EBATECH(CNBR) Lang

	m_height		= 0.0;
	m_width			= 1.0;
	m_obliqueAngle	= 0.0;

	m_newBackwards = m_backwards	= FALSE;
	m_newUpsideDown = m_upsideDown	= FALSE;
	m_newVertically = m_vertically	= FALSE;
	}

/////////////////////////////////////////////////////////////////////////////
// CExplorerPropsTab implementation

/******************************************************************************
*+ CExplorerPropsTab::DoDataExchange - Initializes/retrieves dialog values
*
*  This function takes the values from the CExplorerPropsTab member variables
*  and sticks them in the Text Styles Properties tab and then gets the values
*  from the dialog and sticks them back into the CExplorerPropsTab member
*  variables.
*
*  This tab was implemented the way true property pages should be implemented,
*  so it depends on four other functions to get values from and restore values
*  to the main IntelliCAD Explorer code.  The functions to get the values are
*  CExplorerPropsTab::GetStyleValues() and CExplorerPropsTab::CompareStyleValues().
*  The functions that restore the values are CExplorerPropsTab::SavePropertyValues()
*  and CExplorerPropsTab::RestoreStyleValues().
*
*/

void CExplorerPropsTab::DoDataExchange(
	CDataExchange* pDX
	)
	{

	if (!m_dataptr)								// something's wrong with the data?
		return;

	if (m_inTextStyleProperties && !m_styleName.IsEmpty())
		{
		CDialog::DoDataExchange(pDX);

		DDXDlgStyleNameStatic(pDX);
		DDXDlgHeight(pDX);
		DDXDlgWidth(pDX);
		DDXDlgObliqueAngle(pDX);
		DDXDlgFontName(pDX);					// must do before DDXDlgFontStyle()
		DDXDlgFontStyle(pDX);					// must do after DDXDlgFontName()
		DDXDlgFontLang(pDX);					// must do after DDXDlgFontName() EBATECH(CNBR)
		DDXDlgBigfontName(pDX);					// EBATECH(CNBR)
		DDXDlgBackwards(pDX);
		DDXDlgUpsideDown(pDX);
		DDXDlgVertically(pDX);

		// EBATECH(CNBR) Add m_fontLangCtrl and m_bigfontNameCtrl
		//{{AFX_DATA_MAP(CExplorerPropsTab)		// must get handles to controls
		DDX_Control(pDX, EXP_STYLE_PREVIEW, m_previewCtrl);
		DDX_Control(pDX, EXP_STYLE_WIDTH, m_widthCtrl);
		DDX_Control(pDX, EXP_STYLE_VERTICALLY, m_verticallyCtrl);
		DDX_Control(pDX, EXP_STYLE_UPSIDE_DOWN, m_upsideDownCtrl);
		DDX_Control(pDX, EXP_STYLE_OBLIQUE, m_obliqueAngleCtrl);
		DDX_Control(pDX, EXP_STYLE_HEIGHT, m_heightCtrl);
		DDX_Control(pDX, EXP_STYLE_BACKWARDS, m_backwardsCtrl);
		DDX_Control(pDX, EXP_STYLE_FONT_NAME, m_fontNameCtrl);
		DDX_Control(pDX, EXP_STYLE_FONT_STYLE, m_fontStyleCtrl);
		DDX_Control(pDX, EXP_STYLE_FONT_LANGUAGE, m_fontLangCtrl);
		DDX_Control(pDX, EXP_STYLE_BIGFONT_NAME, m_bigfontNameCtrl);
	//}}AFX_DATA_MAP
		}
	else
		{
		if (!pDX->m_bSaveAndValidate)			// only do if initializing dialog
			IEXP_SubDlg::DoDataExchange(pDX);	// handle the other tabs
		}
	}

/******************************************************************************
*+ CExplorerPropsTab::DDXDlgStyleNameStatic - Initializes the style name
*
*  This function is used in the Text Styles Properties dialog in Drawing
*  Explorer, where we don't need to get the style name back from the dialog.
*  We only have to initialize the dialog with a style name or the number of
*  styles selected (if there are multiple styles selected in the main dialog).
*
*/

void CExplorerPropsTab::DDXDlgStyleNameStatic(	// Initializes the style name
	CDataExchange* pDX
	)
	{
	char tmpBuf[IC_ACADBUF];
	CString tmpString;

	if (!pDX->m_bSaveAndValidate)				// initializing
		{
		if (m_styleNameVaries)					// multiple styles are selected
			{
			sprintf(tmpBuf, ResourceString(IDC_ICADEXPLORER__D_TEXT_S_156, "%d Text styles"),
				m_Vindex);

			tmpString = tmpBuf;
			}
		else
			tmpString = m_styleName;

		DDX_Text(pDX, EXP_STYLE_NAME, tmpString);
		}
	}

/******************************************************************************
*+ CExplorerPropsTab::DDXDlgHeight - Initializes/gets the height
*
*/

void CExplorerPropsTab::DDXDlgHeight(
	CDataExchange* pDX
	)
	{
	char tmpBuf[IC_ACADBUF];

	if (!pDX->m_bSaveAndValidate)				// initializing
		{
		if (m_heightVaries)
			m_heightStr = ResourceString(IDC_IEXP_VARIES_S, "Varies");
		else
			{
			sprintf(tmpBuf, "%G"/*DNT*/, m_height);
			m_heightStr = tmpBuf;
			}

		DDX_Text(pDX, EXP_STYLE_HEIGHT, m_heightStr);
		}
	else										// retrieving
		DDX_Text(pDX, EXP_STYLE_HEIGHT, m_newHeightStr);
	}

/******************************************************************************
*+ CExplorerPropsTab::DDXDlgWidth - Initializes/gets the width factor
*
*/

void CExplorerPropsTab::DDXDlgWidth(
	CDataExchange* pDX
	)
	{
	char tmpBuf[IC_ACADBUF];

	if (!pDX->m_bSaveAndValidate)				// initializing
		{
		if (m_widthVaries)
			m_widthStr = ResourceString(IDC_IEXP_VARIES_S, "Varies");
		else
			{
			sprintf(tmpBuf, "%G"/*DNT*/, m_width);
			m_widthStr = tmpBuf;
			}

		DDX_Text(pDX, EXP_STYLE_WIDTH, m_widthStr);
		}
	else										// retrieving
		DDX_Text(pDX, EXP_STYLE_WIDTH, m_newWidthStr);
	}

/******************************************************************************
*+ CExplorerPropsTab::DDXDlgObliqueAngle - Initializes/gets the oblique angle
*
*/

void CExplorerPropsTab::DDXDlgObliqueAngle(
	CDataExchange* pDX
	)
	{
	char tmpBuf[IC_ACADBUF];

	if (!pDX->m_bSaveAndValidate)				// initializing
		{
		if (m_obliqueAngleVaries)
			m_obliqueAngleStr = ResourceString(IDC_IEXP_VARIES_S, "Varies");
		else
			{
			double angle = (180 / IC_PI) * m_obliqueAngle;
			ConvertObliqueAngle(angle);
			sprintf(tmpBuf, "%G"/*DNT*/, angle);
			m_obliqueAngleStr = tmpBuf;
			}

		DDX_Text(pDX, EXP_STYLE_OBLIQUE, m_obliqueAngleStr);
		}
	else										// retrieving
		{
		DDX_Text(pDX, EXP_STYLE_OBLIQUE, m_newObliqueAngleStr);
		}
	}

/******************************************************************************
*+ CExplorerPropsTab::DDXDlgFontName - Gets the font name
*
*/

void CExplorerPropsTab::DDXDlgFontName(
	CDataExchange* pDX
	)
	{
	if (pDX->m_bSaveAndValidate)				// only handle saving here
		{
		int index;
		char tmpStr[IC_ACADBUF];
		CString namectrl;
		DDX_CBIndex(pDX, EXP_STYLE_FONT_NAME, index);
		if (CB_ERR != m_fontNameCtrl.GetLBText(index, tmpStr))
			{
			DWORD fontType = m_fontNameCtrl.GetFontType(index);
			if (CB_ERR == fontType)
				{
//				m_newHasTrueTypeFont = false;
//				m_newFontName = "txt.shx";/*DNT*/	// failure - so set to something that should be valid
				m_fontNameCtrl.GetWindowText(namectrl);
				if (namectrl == ResourceString(IDC_IEXP_VARIES_S, "Varies"))
					{
					m_newHasTrueTypeFont = false;
					m_newFontName = "";
					}
				else
					{
					if (namectrl.Right(4) == ".shx" || namectrl.Right(4) == ".SHX" ||
						namectrl.Right(4) == ".shp" || namectrl.Right(4) == ".SHP")
						m_newHasTrueTypeFont = false;
					else
						m_newHasTrueTypeFont = true;
					m_newFontName = tmpStr;
					}
				}
			else
				{
				if ((SHX_FONTTYPE == fontType) || (SHP_FONTTYPE == fontType))
					m_newHasTrueTypeFont = false;
				else
					m_newHasTrueTypeFont = true;

				m_newFontName = tmpStr;
				}
			}
		else
			{
//			m_newHasTrueTypeFont = false;
//			m_newFontName = "txt.shx";/*DNT*/		// failure - so set to something that should be valid
//			if (namectrl == ResourceString(IDC_IEXP_VARIES_S, "Varies"))
				m_newFontName = "";
			}
		}
	else
		{
		if (m_fontFamilyVaries || m_hasTrueTypeFontVaries)
			m_fontFamily = ResourceString(IDC_IEXP_VARIES_S, "Varies");
		if (m_fontFilenameVaries || m_hasTrueTypeFontVaries)
			m_fontFilename = ResourceString(IDC_IEXP_VARIES_S, "Varies");
		}
	}

/******************************************************************************
*+ CExplorerPropsTab::DDXDlgFontStyle - Gets the font style
*
*/

void CExplorerPropsTab::DDXDlgFontStyle(
	CDataExchange* pDX
	)
	{

// to do - need to deal with m_newFontStyle as a long instead
//m_newFontStyleIndex = 0;
	if (pDX->m_bSaveAndValidate && m_newHasTrueTypeFont)// only handle saving here; and only for TTF
		{
//		char tmpStr[IC_ACADBUF];
		CString stylectrl;
		DDX_CBIndex(pDX, EXP_STYLE_FONT_STYLE, m_newFontStyleIndex);
		m_fontStyleCtrl.GetWindowText(stylectrl);
		if (stylectrl == ResourceString(IDC_IEXP_VARIES_S, "Varies"))
			{
			m_newFontStyle = -1;
			m_newFontStyleIndex = -1;
			}
//		if (CB_ERR != m_fontStyleCtrl.GetLBText(m_newFontStyleIndex, tmpStr))
//			m_newFontStyle = tmpStr;
//		else
//			m_newFontStyle = "Regular";					// failure - so set to something that should be valid
		}
	}

// EBATECH(CNBR) -[
/******************************************************************************
*+ CExplorerPropsTab::DDXDlgBigfontName - Gets the bigfont name
*
*/

void CExplorerPropsTab::DDXDlgBigfontName(
	CDataExchange* pDX
	)
{
	if (pDX->m_bSaveAndValidate){				// only handle saving here
		int index;
		char tmpStr[IC_ACADBUF];
		CString namectrl;
		DDX_CBIndex(pDX, EXP_STYLE_BIGFONT_NAME, index);
		if (CB_ERR != m_bigfontNameCtrl.GetLBText(index, tmpStr)){
			m_newBigfontName = CString(tmpStr);
			if (m_newBigfontName == ResourceString(IDC_IEXP_VARIES_S, "Varies")){
				m_newBigfontName = "";
			}
			if (m_newBigfontName ==  ResourceString(IDC_EXPSTYLE_NO_BIGFONT, "(none)")){
				m_newBigfontName = "";
			}
		}else{
			m_newBigfontName = "";
		}
	}else{
		if (m_bigfontFilenameVaries){
			m_bigfontFilename = ResourceString(IDC_IEXP_VARIES_S, "Varies");
		}
	}
}

/******************************************************************************
*+ CExplorerPropsTab::DDXDlgFontLang - Gets the font lang
*
*/

void CExplorerPropsTab::DDXDlgFontLang(
	CDataExchange* pDX
	)
{
	if (pDX->m_bSaveAndValidate && m_newHasTrueTypeFont){// only handle saving here; and only for TTF
		CString langctrl;
		DDX_CBIndex(pDX, EXP_STYLE_FONT_LANGUAGE, m_newFontLangIndex);
		m_fontLangCtrl.GetWindowText(langctrl);
		if (langctrl == ResourceString(IDC_IEXP_VARIES_S, "Varies")){
			m_newFontStyle = -1;
			m_newFontLangIndex = -1;
		}
	}
}
// EBATECH(CNBR) ]-

/******************************************************************************
*+ CExplorerPropsTab::DDXDlgBackwards - Initializes/gets the backwards value
*
*/

void CExplorerPropsTab::DDXDlgBackwards(
	CDataExchange* pDX
	)
	{

	if (!pDX->m_bSaveAndValidate)				// initializing
		{
		if (m_backwardsVaries)
			m_backwards = IEXP_VARIES_I;

		DDX_Check(pDX, EXP_STYLE_BACKWARDS, m_backwards);
		}
	else										// retrieving
		DDX_Check(pDX, EXP_STYLE_BACKWARDS, m_newBackwards);
	}

/******************************************************************************
*+ CExplorerPropsTab::DDXDlgUpsideDown - Initializes/gets the upside down value
*
*/

void CExplorerPropsTab::DDXDlgUpsideDown(
	CDataExchange* pDX
	)
	{

	if (!pDX->m_bSaveAndValidate)				// initializing
		{
		if (m_upsideDownVaries)
			m_upsideDown = IEXP_VARIES_I;

		DDX_Check(pDX, EXP_STYLE_UPSIDE_DOWN, m_upsideDown);
		}
	else										// retrieving
		DDX_Check(pDX, EXP_STYLE_UPSIDE_DOWN, m_newUpsideDown);
	}

/******************************************************************************
*+ CExplorerPropsTab::DDXDlgVertically - Initializes/gets the vertical value
*
*/

void CExplorerPropsTab::DDXDlgVertically(
	CDataExchange* pDX
	)
	{

	if (!pDX->m_bSaveAndValidate)				// initializing
		{
		if (m_verticallyVaries)
			m_vertically = IEXP_VARIES_I;

		DDX_Check(pDX, EXP_STYLE_VERTICALLY, m_vertically);
		}
	else										// retrieving
		DDX_Check(pDX, EXP_STYLE_VERTICALLY, m_newVertically);
	}

bool CExplorerPropsTab::ConvertObliqueAngle(double& value)
	{

	// make sure the angle is greater than -360 and
	// less than 360
	if ((value <= -360.0) || (value >= 360.0))
		ic_condenseAngle(value);

	// make sure the angle is from -85 to 85
	bool result = true;
	if (value >= 275.0)
		value -= 360.0;			// convert to -85 or higher
	else if (value <= -275)
		value += 360.0;			// convert to 85 or lower
	else if ((value < -85)	|| (value > 85))
		result = false;

	return result;
	}

/******************************************************************************
*+ CExplorerPropsTab::GetTextStyleValues - store style values for use elsewhere
*
*  Retrieves the values for a text style from a style table record and stores
*  them in the styleValues object.
*/

void CExplorerPropsTab::GetTextStyleValues(	// store style values for use elsewhere
	db_styletabrec *styleRec				// i - style record to get values from
	)
	{
	char *tmpStr, tmpBuf[IC_ACADBUF];
	sds_real tmpReal;
	int tmpInt;

	styleRec->get_2(&tmpStr);
	m_styleName = tmpStr;

	bool hasTrueTypeFont = styleRec->hasTrueTypeFont();
	// EBATECH(CNBR) -[ Forget to initialize
	m_weight = FW_NORMAL;
	m_italic = false;
	// EBATECH(CNBR) ]-
	if (hasTrueTypeFont)
		{
		m_hasTrueTypeFont = true;

		styleRec->getFontFamily(tmpBuf);
		m_fontFamily = tmpBuf;

		styleRec->getFontStyleNumber(&m_fontStyle);
		// EBATECH(CNBR) -[ Forget to initialize/ Move before.
		//m_weight = FW_NORMAL;
		//m_italic = false;
		// EBATECH(CNBR) ]-
		if ((0x3000000 & m_fontStyle) == 0x3000000) // Bold & italic
			{
			m_weight = FW_BOLD;
			m_italic = true;
			}
		else if (0x2000000 & m_fontStyle)			// Bold
			m_weight = FW_BOLD;
		else if (0x1000000 & m_fontStyle)			// Italic
			m_italic = true;
		}

	styleRec->get_3(tmpBuf, IC_ACADBUF - 1);
	if (!hasTrueTypeFont)					// sometimes shx fonts don't have an extension,
		ic_setext(tmpBuf, ".shx"/*DNT*/);	// we need it for finding the font in combo boxes

	m_fontFilename = tmpBuf;

	// EBATECH(CNBR)
	styleRec->get_4(tmpBuf, IC_ACADBUF - 1);
	if (!hasTrueTypeFont){					// sometimes shx fonts don't have an extension,
		if(strlen(tmpBuf)>0){				// Bigfont accept NULL string. 2001-06-27
			ic_setext(tmpBuf, ".shx"/*DNT*/);	// we need it for finding the font in combo boxes
		}
	}else{
		tmpBuf[0]='\0';						// Bigfont must set null if TrueType is selected.
	}
	m_bigfontFilename = tmpBuf;
	// EBATECH(CNBR) ]-

	styleRec->get_40(&tmpReal);
	m_height = tmpReal;

	styleRec->get_41(&tmpReal);
	m_width = tmpReal;

	styleRec->get_50(&tmpReal);
	m_obliqueAngle = tmpReal;

	styleRec->get_70(&tmpInt);
	m_vertically = (tmpInt & 4) ? TRUE : FALSE;

	styleRec->get_71(&tmpInt);
	m_backwards = (tmpInt & 2) ? TRUE : FALSE;
	m_upsideDown = (tmpInt & 4) ? TRUE : FALSE;
	}

/******************************************************************************
*+ CExplorerPropsTab::CompareTextStyleValues - compare style values and set varies flags
*
*  Compares the Text Style values from the specified table record agains those
*  stored previously.  If any values are different, the function sets the
*  appropriate varies flags.
*/

void CExplorerPropsTab::CompareTextStyleValues(	// compare style values and set varies flags
	db_styletabrec *styleRec					// i - style record to get values from
	)
	{
	char *tmpStr, tmpBuf[IC_ACADBUF];
	sds_real tmpReal;
	int tmpInt;

	styleRec->get_2(&tmpStr);
	if (m_styleName != tmpStr)
		m_styleNameVaries = true;

	styleRec->get_3(&tmpStr);
	if (m_fontFilename != tmpStr)
		m_fontFilenameVaries = true;

	if (m_hasTrueTypeFont != styleRec->hasTrueTypeFont())
		m_hasTrueTypeFontVaries = true;

	// if both are TrueType fonts, see if the font families and
	// font styles vary
	if (m_hasTrueTypeFont && styleRec->hasTrueTypeFont())
		{
		if (styleRec->getFontFamily(tmpBuf))
			{
			if (m_fontFamily.IsEmpty())	// can be empty if failed to get it
				m_fontFamilyVaries = true;
			else
				{
				if (m_fontFamily != tmpBuf)
					m_fontFamilyVaries = true;
				}
			}
		else
			{
			if (!m_fontFamily.IsEmpty()) // can be empty if failed to get it
				m_fontFamilyVaries = true;
			}

		long styleNumber;
		styleRec->getFontStyleNumber(&styleNumber);
		if (m_fontStyle != styleNumber)
			{
			m_fontStyleVaries = true;
			if ((0x3000000 & m_fontStyle) == 0x3000000)
				{
				if ((0x3000000 & styleNumber) == 0x3000000)
					m_fontStyleVaries = false;
				}
			else if (0x2000000 & m_fontStyle)
				{
				if (0x2000000 & styleNumber)
					m_fontStyleVaries = false;
				}
			else if (0x1000000 & m_fontStyle)
				{
				if (0x1000000 & styleNumber)
					m_fontStyleVaries = false;
				}
			else if (!((0x3000000 & styleNumber) == 0x3000000) &&
				!(0x2000000 & styleNumber) && !(0x1000000 & styleNumber))
				m_fontStyleVaries = false;
			}
		}

	if (m_fontFamilyVaries)
		m_fontFilenameVaries = true;		// should be true anyway

	// EBATECH(CNBR) -[ compare bigfont
	styleRec->get_4(&tmpStr);
	if (m_bigfontFilename != tmpStr)
		m_bigfontFilenameVaries = true;
	// EBATECH(CNBR) ]-

	styleRec->get_40(&tmpReal);
	if (m_height != tmpReal)
		m_heightVaries = true;

	styleRec->get_41(&tmpReal);
	if (m_width != tmpReal)
		m_widthVaries = true;

	styleRec->get_50(&tmpReal);
	if (m_obliqueAngle != tmpReal)
		m_obliqueAngleVaries = true;

	styleRec->get_70(&tmpInt);
	// VT. 4-4-02. Fix using bool(true) as int(1).
	if (m_vertically != ((tmpInt & 4) ? 1:0))
		m_verticallyVaries = true;

	styleRec->get_71(&tmpInt);
	if (m_backwards != ((tmpInt & 2) ? 1:0))
		m_backwardsVaries = true;

	if (m_upsideDown != ((tmpInt & 4) ? 1:0))
		m_upsideDownVaries = true;
	}

/******************************************************************************
*+ CExplorerPropsTab::SavePropertyValues - saves the values from the Properties tabs
*
*  Determines whether the Text Style Properties tab or another tab is being
*  used and saves the values appropriately.
*/

void CExplorerPropsTab::SavePropertyValues(	// saves the values from the Properties tabs
	void
	)
	{
	if (m_inTextStyleProperties)
		RestoreTextStyleValues();
	else
		SaveTabChanges();
	}

/******************************************************************************
*+ CExplorerPropsTab::GetTextStyleTableInfo - gets the font filename and style number
*
*  Gets the TrueType font filename (e.g., arial.ttf) and the style number (e.g., 33554466)
*  that are needed for a text style table record when the font is a TrueType font.
*/

void CExplorerPropsTab::GetTextStyleTableInfo(
	char *fontFilename,				// o - the font's filename (e.g., arial.ttf); can be NULL
	char *fontGlobalName,			// o - the font's familyname in English (e.g., Arial); can be NULL
	long *styleNumber)				// o - the style number (e.g., 33554466); can be NULL
{
	// EBATECH(CNBR)
	long styletemp = 34L;
	char fileName[IC_ACADBUF];
	char globalName[IC_ACADBUF];
	char localName[IC_ACADBUF];
	fileName[0] = globalName[0] = localName[0] = '\0';
	// get the style data from the style combo box
	//StyleItem *styleItem = (StyleItem *) m_fontStyleCtrl.GetItemDataPtr(m_newFontStyleIndex);
	//*styleNumber = CTTFInfoGetter::styleNumber(styleItem->weight, styleItem->italic, styleItem->pitchAndFamily, styleItem->charSet );
	//CTTFInfoGetter::get().getFontFileName(fontFilename, styleItem->fullName, *styleNumber);
	if( (int)m_fontStyleCtrl.GetItemDataPtr(m_newFontStyleIndex) != CB_ERR ){
		// get the style data from the style combo box
		StyleItem *styleItem = (StyleItem *) m_fontStyleCtrl.GetItemDataPtr(m_newFontStyleIndex);
		int charSet;

		if( m_fontLangCtrl.GetItemData(m_newFontLangIndex) != CB_ERR ){
			charSet = m_fontLangCtrl.GetItemData(m_newFontLangIndex);
		}else{
			charSet = styleItem->charSet;
		}
		styletemp = CTTFInfoGetter::styleNumByFontParams(styleItem->weight, styleItem->italic, styleItem->pitchAndFamily, charSet );

		if(!CTTFInfoGetter::get().getFontFileName(fileName, styleItem->fullName, styletemp))
		{
			strncpy(localName,(LPCTSTR)m_newFontName,IC_ACADBUF-1);
			if( CTTFInfoGetter::get().getGlobalStyleByLocalName( localName, styletemp, globalName )){
				CTTFInfoGetter::makeFontFileName(globalName, fileName);
			}else{
				strcpy(fileName, "Arial.ttf"/*DNT*/);
				styletemp = ACAD_DEFAULTSTYLENUMBER;
			}
		}
	}
	if( styleNumber ){ *styleNumber = styletemp; }
	if( fontFilename ){ strcpy( fontFilename, fileName ); }
	if( fontGlobalName ){ strcpy( fontGlobalName, globalName ); }
	// EBATECH(CNBR) ]-
}
/******************************************************************************
*+ CExplorerPropsTab::DrawFontPreview - draws a preview of the specified font
*
*  Draws a sample of the specified font according to the width factor, oblique
*  angle, backwards, upside down, vertically, font name, and font style
*  settings.
*
*/

void CExplorerPropsTab::DrawFontPreview(
	void)
	{
	CString cstmp;
	m_fontNameCtrl.GetLBText(m_fontNameCtrl.GetCurSel(), cstmp);
	if (cstmp != ResourceString(IDC_IEXP_VARIES_S, "Varies"))
		m_previewCtrl.m_fontType	= m_fontNameCtrl.GetFontType(m_fontNameCtrl.GetCurSel());
	else
		m_previewCtrl.m_fontType	= -1;

	int fontCount = m_fontNameCtrl.GetCount();
	if ((0 == fontCount) || (CB_ERR == fontCount))
		{
		m_previewCtrl.SetWindowText(""/*DNT*/);
		return;
		}

	m_previewCtrl.m_width			= 1.0;
	m_previewCtrl.m_obliqueAngle	= 0.0;
	m_previewCtrl.m_backwards		= FALSE;
	m_previewCtrl.m_upsideDown		= FALSE;
	m_previewCtrl.m_vertically		= FALSE;

	int index = m_fontNameCtrl.GetCurSel();
	if (CB_ERR == index)
		m_previewCtrl.m_fontItem = NULL;
	else
		{
		FontItem *fontItem = (FontItem *) m_fontNameCtrl.GetItemDataPtr(index);
		if (-1 == (int) fontItem)
			m_previewCtrl.m_fontItem = NULL;
		else
			{
			m_previewCtrl.m_fontItem = fontItem;
			if (!fontItem && cstmp != ResourceString(IDC_IEXP_VARIES_S, "Varies"))
				{
				fontItem = new FontItem;
				m_fontNameCtrl.GetWindowText(cstmp);
				strncpy(fontItem->fontName, cstmp.GetBuffer(0), 32);
				if (m_hasTrueTypeFont)
					fontItem->fontType = TRUETYPE_FONTTYPE;
				else if (m_fontFilename.Right(4) == ".shp" || m_fontFilename.Right(4) == ".SHP")
					fontItem->fontType = SHP_FONTTYPE;
				else
					fontItem->fontType = SHX_FONTTYPE;
				LOGFONT logFont;
				logFont.lfCharSet = DEFAULT_CHARSET;
				strcpy(logFont.lfFaceName, fontItem->fontName);
				logFont.lfPitchAndFamily = 0;

				// DP: TODO: REMOVE ENUMERATION
				//	numerate the fonts
				CClientDC dc(NULL);
				::EnumFontFamiliesEx(dc.GetSafeHdc(),
									 &logFont,
									 (FONTENUMPROC)CIcadFontComboBox::EnumFontFamExProc,
									 (LPARAM) this,
									 0);

				if (!logFont.lfHeight)
					logFont.lfHeight = 1;
				fontItem->logFont = logFont;
				m_previewCtrl.m_fontItem = fontItem;
				m_previewCtrl.m_fontType = fontItem->fontType;

				}
			}
		}

	m_previewCtrl.DrawStyleFontPreview();
	}


// EBATECH(CNBR) Add OnBigfontNameChanged and OnFontLangChanged
BEGIN_MESSAGE_MAP(CExplorerPropsTab, IEXP_SubDlg)
	ON_MESSAGE(CIcadFontComboBox::WM_FONT_NAME_CHANGED, OnFontNameChanged)
	ON_MESSAGE(CIcadFontComboBox::WM_BIGFONT_NAME_CHANGED, OnBigfontNameChanged)
	ON_MESSAGE(CIcadFontStyleComboBox::WM_FONT_STYLE_CHANGED, OnFontStyleChanged)
	ON_MESSAGE(CIcadFontLangComboBox::WM_FONT_LANG_CHANGED, OnFontLangChanged)
	//{{AFX_MSG_MAP(CExplorerPropsTab)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExplorerPropsTab message handlers

/******************************************************************************
*+ CExplorerPropsTab::OnFontNameChanged
*
*  to do - When IntelliCAD Explorer is redone, this should be able to be merged
*		   with the same function in explorerpropstab.cpp into a base class
*		   from which the IntelliCAD Explorer Styles Properties tab and the
*		   Style command's dialog can be derived.
*/

LONG CExplorerPropsTab::OnFontNameChanged(UINT wParam, LONG lParam)
	{
	// if we can't get the index of the current selection, something's wrong
	int index = m_fontNameCtrl.GetCurSel();
	if (CB_ERR == index)
		return 0L;

	// get the new font's type
	DWORD fontType = m_fontNameCtrl.GetFontType(m_fontNameCtrl.GetCurSel());

	// enable/disable the style combo box and the vertically check box
	if (((SHX_FONTTYPE == fontType) || (SHP_FONTTYPE == fontType) ||
		(!m_hasTrueTypeFont && (fontType == CB_ERR))) && !m_hasTrueTypeFontVaries)
		{
		m_newHasTrueTypeFont = false;
		m_verticallyCtrl.EnableWindow(TRUE);
//		m_fontStyleCtrl.DeleteStyleItems();		// delete dynamic memory before resetting contents
		//EBATECH(CNBR) -[ Add Some Controls
		m_fontStyleCtrl.SetCurSel(-1);
		m_fontStyleCtrl.EnableWindow(FALSE);	// no style for shx/shp fonts
		m_fontLangCtrl.SetCurSel(-1);
		m_fontLangCtrl.EnableWindow(FALSE);	// no style for shx/shp fonts
		m_bigfontNameCtrl.SetCurSel(-1);
		m_bigfontNameCtrl.EnableWindow(TRUE);	// no style for shx/shp fonts
		//EBATECH(CNBR) ]-
		}
	else
		{
		m_newHasTrueTypeFont = true;
		m_verticallyCtrl.EnableWindow(FALSE);
		m_fontStyleCtrl.EnableWindow(TRUE);	// can't generate TrueType fonts vertically
		//EBATECH(CNBR) -[ Add Some Controls
		m_fontLangCtrl.EnableWindow(TRUE);
		m_bigfontNameCtrl.SetCurSel(-1);
		m_bigfontNameCtrl.EnableWindow(FALSE);
		//EBATECH(CNBR) ]-
		CString fontName;
		m_fontNameCtrl.GetLBText(m_fontNameCtrl.GetCurSel(), fontName);
		if (fontType != CB_ERR)
			{
			m_fontStyleCtrl.FillTrueTypeStyles(fontName, m_fontStyle);
			m_fontLangCtrl.FillTrueTypeLangs(fontName, m_fontStyle);
			if (m_fontStyleVaries)		// style varies but not name - fill style box as usual
										// but add a "Varies" string.
				{
				m_fontStyleCtrl.AddString("Varies");
				m_fontStyleCtrl.SetCurSel(m_fontStyleCtrl.FindStringExact(-1, "Varies"));
				m_fontLangCtrl.AddString("Varies");
				m_fontLangCtrl.SetCurSel(m_fontStyleCtrl.FindStringExact(-1, "Varies"));
				m_previewCtrl.SetWindowText("");
				return 1L;
				}
			}
		else
			{
			m_fontStyleCtrl.ResetContent();
			m_fontLangCtrl.ResetContent();
			m_previewCtrl.SetWindowText("");
			if (m_fontStyleVaries || m_hasTrueTypeFontVaries)	// both vary - so "Varies" is the only selection
				{
				m_fontStyleCtrl.AddString("Varies");
				m_fontStyleCtrl.SetCurSel(0);
				m_fontLangCtrl.AddString("Varies");
				m_fontLangCtrl.SetCurSel(0);
				return 1L;
				}
			else				// name varies, not style, so similar style is the only choice.
				{
				m_fontStyleCtrl.FillTrueTypeStyles("Arial", m_fontStyle);
				m_fontLangCtrl.FillTrueTypeLangs("Arial", m_fontStyle);
				m_weight = FW_NORMAL;
				m_italic = false;
				if ((0x3000000 & m_fontStyle) == 0x3000000) // Bold & italic
					{
						m_fontStyleCtrl.DeleteString(2);	// we want the last string
						m_fontStyleCtrl.DeleteString(1);
						m_fontStyleCtrl.DeleteString(0);
						m_weight = FW_BOLD;
						m_italic = true;
					}
				else if (0x2000000 & m_fontStyle)			// Bold
					{
						m_fontStyleCtrl.DeleteString(3);	// bold is the third string
						m_fontStyleCtrl.DeleteString(1);
						m_fontStyleCtrl.DeleteString(0);
						m_weight = FW_BOLD;
					}
				else if (0x1000000 & m_fontStyle)			// Italic
					{
						m_fontStyleCtrl.DeleteString(3);	// keep the second string
						m_fontStyleCtrl.DeleteString(2);
						m_fontStyleCtrl.DeleteString(0);
						m_italic = true;
					}
				else										// Regular
					{
						m_fontStyleCtrl.DeleteString(3);	// only want the first string
						m_fontStyleCtrl.DeleteString(2);
						m_fontStyleCtrl.DeleteString(1);
					}
				m_fontStyleCtrl.SetCurSel(0);
				DrawFontPreview();
				return 1L;
				}
			}
		// Now take care of Bold/Italic/BoldItalic settings.
		// We cannot search for the name "bold/italic/bolditalic" because it changes from
		// program to program, so we must find the closest match of weight and italic.
		int weight[5];
		int italic[5];
		int final = -1;  //the index that is the closest match.
		bool test = false;	//we found a match
		for (int i = 0; m_fontStyleCtrl.GetCount() > i; i++)
			{
			weight[i] = ((StyleItem*) (m_fontStyleCtrl.GetItemDataPtr(i)))->weight;
			italic[i] = ((StyleItem*) (m_fontStyleCtrl.GetItemDataPtr(i)))->italic;
			}
		if ((0x3000000 & m_fontStyle) == 0x3000000) // Bold & italic
			{
			for (int j = 0; j < i; j++)
				{
				if (italic[j] == 255 && (final < 0 || weight[final] < weight[j])) // look for darkest font with
					{											 // italics we can find.
					final = j;
					test = true;
					}
				}
			if (!test)		// failed!	Ditch italics - find the darkest available font.
				{
				final = 0;
				for (j = 0; j < i; j++)
					{
					if (weight[final] < weight[j])
						final = j;
					}
				}
			}
		else if (0x2000000 & m_fontStyle)			// Bold
			{
			for (int j = 0; j < i; j++)
				{
				if (italic[j] == 0 && (final < 0 || weight[final] < weight[j]))
					{
					final = j;
					test = true;
					}
				}
			if (!test)		// failed!	Now accept italic fonts.
				{
				final = 0;
				for (j = 0; j < i; j++)
					{
					if (weight[final] < weight[j])
						final = j;
					}
				}
			}
		else if (0x1000000 & m_fontStyle)			// Italic
			{
			for (int j = 0; j < i; j++)
				{

				// conditions: italics, non-bold, and as close to FW_NORMAL as possible.
				if (italic[j] == 255 && weight[j] < 600 &&
					(final < 0 || abs(weight[j] - 400) < abs(weight[final] - 400)))
					{
					test = true;
					final = j;
					}
				}
			if (!test)	// failed!	Allow bold fonts.
				{
				for (j = 0; j < i; j++)
					{
					if (italic[j] == 255 &&
						(final < 0 || abs(weight[j] - 400) < abs(weight[final] - 400)))
						{
						test = true;
						final = j;
						}
					}
				if (!test)	// failed - find closest match to 400 weight.
					{
					final = 0;
					for (j = 0; j < i; j++)
						{
						if (abs(weight[j] - 400) < abs(weight[final] - 400))
							final = j;
						}
					}
				}
			}
		else														// Regular
			{
			for (int j = 0; j < i; j++)
				{
				if (italic[j] == 0 && weight[j] < 600 &&
					(final < 0 || abs(weight[j] - 400) < abs(weight[final] - 400)))
					{
					test = true;
					final = j;
					}
				if (!test)
					{
					if (italic[j] == 0 &&
						(final < 0 || abs(weight[j] - 400) < abs(weight[final] - 400)))
						{
						test = true;
						final = j;
						}
					}
				if (!test)
					{
					final = 0;
					if (abs(weight[j] - 400) < abs(weight[final] - 400))
						final = j;
					}
				}
			}
			{ // EBATECH(CNBR) -[ Set Lang
				int i,n,charSet;
				n=m_fontLangCtrl.GetCount();
				charSet=((m_fontStyle & 0x0000ff00)>>8);
				for(i = 0; i < n ; i++ ){
					if( charSet == m_fontLangCtrl.GetItemData(i)){
						m_fontLangCtrl.SetCurSel(i);
						break;
					}
				}
				if(i>=n){
					m_fontLangCtrl.SetCurSel(0);
				}
			}// EBATECH(CNBR)

		m_fontStyleCtrl.SetCurSel(final);
		long styleNumber;
		StyleItem* style = ((StyleItem*) m_fontStyleCtrl.GetItemDataPtr(final));

		// EBATECH(CNBR) -[ Lang
		int index = m_fontLangCtrl.GetCurSel();
		int charSet;
		if (CB_ERR == index){
			charSet = style->charSet;
		}else{
			charSet = m_fontLangCtrl.GetItemData(index);
			if (CB_ERR == charSet){
				charSet = style->charSet;
			}
		}// EBATECH(CNBR) ]-

		styleNumber = CTTFInfoGetter::styleNumByFontParams(style->weight, style->italic, style->pitchAndFamily, charSet);

		m_weight = FW_NORMAL;
		m_italic = false;
		if ((0x3000000 & styleNumber) == 0x3000000) // Bold & italic
			{
			m_weight = FW_BOLD;
			m_italic = true;
			}
		else if (0x2000000 & styleNumber)			// Bold
			m_weight = FW_BOLD;
		else if (0x1000000 & styleNumber)			// Italic
			m_italic = true;

		m_newFontStyle = styleNumber; // EBATECH(CNBR)

		}

	DrawFontPreview();
	return 1L;
	}

/******************************************************************************
*+ CExplorerPropsTab::OnFontStyleChanged
*
*  to do - When IntelliCAD Explorer is redone, this should be able to be merged
*		   with the same function in explorerpropstab.cpp into a base class
*		   from which the IntelliCAD Explorer Styles Properties tab and the
*		   Style command's dialog can be derived.
*/

LONG CExplorerPropsTab::OnFontStyleChanged(UINT wParam, LONG lParam)
	{
	int index = m_fontStyleCtrl.GetCurSel();
	if (CB_ERR == index)
		return 0L;

	long styleNumber;
	StyleItem *styleItem = (StyleItem *) m_fontStyleCtrl.GetItemDataPtr(index);	// get the style data from the style combo box
	if (!styleItem)
		{
		m_previewCtrl.SetWindowText("");
		return 1L;
		}

	// EBATECH(CNBR) -[
	index = m_fontLangCtrl.GetCurSel();
	if (CB_ERR == index)
		return 0L;

	int charSet = m_fontLangCtrl.GetItemData(index);	// get the style data from the style combo box
	styleNumber = CTTFInfoGetter::styleNumByFontParams(styleItem->weight, styleItem->italic, styleItem->pitchAndFamily, charSet );
	m_newFontStyle = styleNumber;
	// EBATECH(CNBR) ]-

	m_weight = FW_NORMAL;
	m_italic = false;
	if ((0x3000000 & styleNumber) == 0x3000000) // Bold & italic
		{
		m_weight = FW_BOLD;
		m_italic = true;
		}
	else if (0x2000000 & styleNumber)			// Bold
		m_weight = FW_BOLD;
	else if (0x1000000 & styleNumber)			// Italic
		m_italic = true;

	DrawFontPreview();
	return 1L;
	}

/******************************************************************************
*+ CExplorerPropsTab::OnFontLangChanged
*
*  to do - When IntelliCAD Explorer is redone, this should be able to be merged
*		   with the same function in explorerpropstab.cpp into a base class
*		   from which the IntelliCAD Explorer Styles Properties tab and the
*		   Style command's dialog can be derived.
*/

LONG CExplorerPropsTab::OnFontLangChanged(UINT wParam, LONG lParam)
	{
	int index = m_fontStyleCtrl.GetCurSel();
	if (CB_ERR == index)
		return 0L;

	long styleNumber;
	StyleItem *styleItem = (StyleItem *) m_fontStyleCtrl.GetItemDataPtr(index);	// get the style data from the style combo box

	if (!styleItem)
		{
		m_previewCtrl.SetWindowText("");
		return 1L;
		}

	index = m_fontLangCtrl.GetCurSel();
	if (CB_ERR == index)
		return 0L;

	int charSet = m_fontLangCtrl.GetItemData(index);	// get the style data from the style combo box
	styleNumber = CTTFInfoGetter::styleNumByFontParams(styleItem->weight, styleItem->italic, styleItem->pitchAndFamily, charSet );
	m_newFontStyle = styleNumber;

	DrawFontPreview();
	return 1L;
	}
// EBATECH(CNBR) 2001-06-14 -[ Add BIGFONT and LANGAGE LISTBOX CONTROL
/******************************************************************************
*+ CExplorerPropsTab::OnBigfontNameChanged
*
*  to do - When IntelliCAD Explorer is redone, this should be able to be merged
*		   with the same function in explorerpropstab.cpp into a base class
*		   from which the IntelliCAD Explorer Styles Properties tab and the
*		   Style command's dialog can be derived.
*/

LONG CExplorerPropsTab::OnBigfontNameChanged(UINT wParam, LONG lParam)
	{
	// if we can't get the index of the current selection, something's wrong
	int index = m_bigfontNameCtrl.GetCurSel();
	if (CB_ERR == index)
		return 0L;

	m_bigfontNameCtrl.GetLBText(index, m_newBigfontName);

	if( m_newBigfontName.Compare( ResourceString(IDC_EXPSTYLE_NO_BIGFONT, "(none)")) == 0){
		m_newBigfontName = "" /*DNT*/;
	}
	return 1L;
	}

// EBATECH(CNBR)
/******************************************************************************
*+ CExplorerPropsTab::OnInitDialog - initialize the values for the dialog
*/

BOOL CExplorerPropsTab::OnInitDialog()
	{

	if (m_inTextStyleProperties)
		{
		// if multiple styles are selected, the values may vary
		if (!GetTextStylesVariesInfo())
			return FALSE;

		CDialog::OnInitDialog();	// must do after getting values but before using font combo box

		// fill the font list with the desired fonts
		CString tmpFontName = m_hasTrueTypeFont ? m_fontFamily : m_fontFilename;
		m_fontNameCtrl.FillFontComboBox(GET_FONT_TTF + GET_FONT_SHX); // EBATECH(CNBR) remove GET_FONT_SHP
		if (tmpFontName != ResourceString(IDC_IEXP_VARIES_S, "Varies") && (m_fontNameCtrl.FindStringExact(-1, tmpFontName) != -1))
			m_fontNameCtrl.SetCurrentFont(tmpFontName);
		else
			{
			m_fontNameCtrl.AddString(tmpFontName);
			m_fontNameCtrl.SetCurSel(m_fontNameCtrl.FindStringExact(-1, tmpFontName));
			SendMessage(CIcadFontComboBox::WM_FONT_NAME_CHANGED);
			}

		m_weight = FW_NORMAL;
		m_italic = false;
		if ((0x3000000 & m_fontStyle) == 0x3000000) // Bold & italic
			{
			m_weight = FW_BOLD;
			m_italic = true;
			}
		else if (0x2000000 & m_fontStyle)			// Bold
			m_weight = FW_BOLD;
		else if (0x1000000 & m_fontStyle)			// Italic
			m_italic = true;

		// fill the bigfont list with the desired fonts
		m_bigfontNameCtrl.FillFontComboBox(GET_BIGFONT_SHX);
		if (m_bigfontFilename == "" || (
			m_bigfontFilename != ResourceString(IDC_IEXP_VARIES_S, "Varies") &&
			(m_bigfontNameCtrl.FindStringExact(-1, m_bigfontFilename) != -1))){
			m_bigfontNameCtrl.SetCurrentFont(m_bigfontFilename);
		}else{
			m_bigfontNameCtrl.AddString(m_bigfontFilename);
			m_bigfontNameCtrl.SetCurSel(m_bigfontNameCtrl.FindStringExact(-1, m_bigfontFilename));
			SendMessage(CIcadFontComboBox::WM_BIGFONT_NAME_CHANGED);
		}
		//EBATECH(CNBR) 2001-06-27 BUG
		//tmpFontName = "";
		//if(m_bigfontNameCtrl.FindStringExact(-1, tmpFontName) == CB_ERR )
		//	m_bigfontNameCtrl.InsertString(0, tmpFontName);

		return TRUE;
		}
	else
		return IEXP_SubDlg::OnInitDialog();
	}

void CExplorerPropsTab::OnPaint()
{
	IEXP_SubDlg::OnPaint();
	if (m_previewCtrl)
		{
		DrawFontPreview();
		CString cstext;
		m_fontStyleCtrl.GetWindowText(cstext);
		if (cstext == ResourceString(IDC_IEXP_VARIES_S, "Varies"))
			m_previewCtrl.SetWindowText("");
		}
}

#ifdef USE_THIS

/////////////////////////////////////////////////////////////////////////////
// CFontPropPage implementation

BEGIN_MESSAGE_MAP(CFontPropPage, CStockPropPage)
	//{{AFX_MSG_MAP(CFontPropPage)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(AFX_IDC_FONTPROP, OnSelchangeFontprop)
	ON_CBN_EDITUPDATE(AFX_IDC_FONTNAMES, OnEditupdateFontnames)
	ON_CBN_EDITUPDATE(AFX_IDC_FONTSIZES, OnEditupdateFontsizes)
	ON_CBN_SELCHANGE(AFX_IDC_FONTNAMES, OnSelchangeFontnames)
	ON_CBN_SELCHANGE(AFX_IDC_FONTSIZES, OnSelchangeFontsizes)
	ON_CBN_SELCHANGE(AFX_IDC_FONTSTYLES, OnSelchangeFontstyles)
	ON_CBN_EDITCHANGE(AFX_IDC_FONTSTYLES, OnEditchangeFontstyles)
	ON_BN_CLICKED(AFX_IDC_STRIKEOUT, OnStrikeout)
	ON_BN_CLICKED(AFX_IDC_UNDERLINE, OnUnderline)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CFontPropPage::CFontPropPage() :
	CStockPropPage(IDD, AFX_IDS_FONT_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CFontPropPage)
	//}}AFX_DATA_INIT
}

BOOL _AfxStringFromCy(CString& str, CY& cy)
{
	VARIANTARG varCy;
	VARIANTARG varBstr;
	AfxVariantInit(&varCy);
	AfxVariantInit(&varBstr);
	V_VT(&varCy) = VT_CY;
	V_CY(&varCy) = cy;
	if (FAILED(VariantChangeType(&varBstr, &varCy, 0, VT_BSTR)))
	{
		VariantClear(&varCy);
		VariantClear(&varBstr);
		return FALSE;
	}
	str = V_BSTR(&varBstr);
	VariantClear(&varCy);
	VariantClear(&varBstr);
	return TRUE;
}

BOOL _AfxCyFromString(CY& cy, LPCTSTR psz)
{
	USES_CONVERSION;

	VARIANTARG varBstr;
	VARIANTARG varCy;
	AfxVariantInit(&varBstr);
	AfxVariantInit(&varCy);
	V_VT(&varBstr) = VT_BSTR;
	V_BSTR(&varBstr) = SysAllocString(T2COLE(psz));
	if (FAILED(VariantChangeType(&varCy, &varBstr, 0, VT_CY)))
	{
		VariantClear(&varBstr);
		VariantClear(&varCy);
		return FALSE;
	}
	cy = V_CY(&varCy);
	VariantClear(&varBstr);
	VariantClear(&varCy);
	return TRUE;
}

void CFontPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CFontPropPage)
	DDX_Control(pDX, AFX_IDC_FONTPROP, m_FontProp);
	DDX_Control(pDX, AFX_IDC_SAMPLEBOX, m_SampleBox);
	DDX_Control(pDX, AFX_IDC_FONTSTYLES, m_FontStyles);
	DDX_Control(pDX, AFX_IDC_FONTSIZES, m_FontSizes);
	DDX_Control(pDX, AFX_IDC_FONTNAMES, m_FontNames);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		FONTOBJECT fobj;
		fobj.strName = m_FontNames.GetCurrentName();

		if (fobj.strName.IsEmpty())
			return;

		m_FontSizes.GetPointSize(fobj.cySize);

		if (m_nCurrentStyle & NTM_REGULAR)
			fobj.sWeight = FW_REGULAR;
		if (m_nCurrentStyle & NTM_BOLD)
		{
			fobj.sWeight = FW_BOLD;
			fobj.bBold = TRUE;
		}
		else
			fobj.bBold = FALSE;
		if (m_nCurrentStyle & NTM_ITALIC)
			fobj.bItalic = TRUE;
		else
			fobj.bItalic = FALSE;
		fobj.bUnderline = m_bUnderline;
		fobj.bStrikethrough = m_bStrikeOut;

		SetFontProps(pDX, fobj, m_strPropName);
	}
	else
	{
		FONTOBJECT fobj;
		MERGEOBJECT mobj;
		if (!m_strPropName.IsEmpty() && GetFontProps(pDX, &fobj, m_strPropName, &mobj))
		{
			if (fobj.bBold && fobj.bItalic)
				m_nCurrentStyle = NTM_BOLD | NTM_ITALIC;
			else if (fobj.bBold)
				m_nCurrentStyle = NTM_BOLD;
			else if (fobj.bItalic)
				m_nCurrentStyle = NTM_ITALIC;
			else
				m_nCurrentStyle = NTM_REGULAR;
			m_nActualStyle = m_nCurrentStyle;
			m_bUnderline = fobj.bUnderline;
			m_bStrikeOut = fobj.bStrikethrough;

			mobj.bNameOK = TRUE;
			mobj.bSizeOK = TRUE;
			mobj.bStyleOK = TRUE;
			mobj.bUnderlineOK = TRUE;
			mobj.bStrikethroughOK = TRUE;

			_AfxStringFromCy(m_strFontSize, fobj.cySize);
			SelectFontFromList(fobj.strName, &mobj);
		}
	}
}

BOOL CFontPropPage::SetFontProps(CDataExchange* pDX, FONTOBJECT fobj, LPCTSTR pszPropName)
{
	USES_CONVERSION;

	BOOL bStatus = FALSE;
	COleDispatchDriver PropDispDriver;

	// Set the properties for all the objects
	ASSERT_KINDOF(COlePropertyPage, pDX->m_pDlgWnd);
	COlePropertyPage* propDialog = (COlePropertyPage*)(pDX->m_pDlgWnd);

	ULONG nObjects;
	LPDISPATCH* ppDisp = GetObjectArray(&nObjects);

	for (ULONG i = 0; i < nObjects; i++)
	{
		DISPID dwDispID;

		// Get the Dispatch ID for the property and if successful set the value
		LPCOLESTR lpOleStr = T2COLE(pszPropName);
		if (SUCCEEDED(ppDisp[i]->GetIDsOfNames(IID_NULL, (LPOLESTR*)&lpOleStr,
			1, m_lcid, &dwDispID)))
		{
			LPDISPATCH pFontDisp = NULL;

			// Get property
			PropDispDriver.AttachDispatch(ppDisp[i], FALSE);
			TRY
				PropDispDriver.GetProperty(dwDispID, VT_DISPATCH, &pFontDisp);
			END_TRY
			PropDispDriver.DetachDispatch();

			if (pFontDisp == NULL)
				continue;

			// Get font interface
			IFont * pFont;
			HRESULT hresult = pFontDisp->QueryInterface(IID_IFont, (void**)&pFont);
			if (hresult == S_OK)
			{
				// Set font characteristics
				if (propDialog->GetControlStatus(AFX_IDC_FONTNAMES))
				{
					BSTR bstrName = fobj.strName.AllocSysString();
					pFont->put_Name(bstrName);
					SysFreeString(bstrName);
				}
				if (propDialog->GetControlStatus(AFX_IDC_FONTSIZES))
					pFont->put_Size(fobj.cySize);
				if (propDialog->GetControlStatus(AFX_IDC_FONTSTYLES))
				{
					pFont->put_Bold(fobj.bBold);
					pFont->put_Italic(fobj.bItalic);
					pFont->put_Weight(fobj.sWeight);
				}
				if (propDialog->GetControlStatus(AFX_IDC_UNDERLINE))
					pFont->put_Underline(fobj.bUnderline);
				if (propDialog->GetControlStatus(AFX_IDC_STRIKEOUT))
					pFont->put_Strikethrough(fobj.bStrikethrough);

				// Release the font interface
				RELEASE(pFont);
				bStatus = TRUE;
			}

			// Release the font dispatch interface
			RELEASE(pFontDisp);
		}
	}
	return bStatus;
}

BOOL CFontPropPage::GetFontProps(CDataExchange* /* pDX */,
	FONTOBJECT* pfobj, LPCTSTR pszPropName, MERGEOBJECT* pmobj)
{
	USES_CONVERSION;
	BOOL bStatus = FALSE;

	COleDispatchDriver PropDispDriver;

	ULONG nObjects;
	LPDISPATCH* ppDisp = GetObjectArray(&nObjects);

	for (ULONG i = 0; i < nObjects; i++)
	{
		DISPID dwDispID;

		// Get the Dispatch ID for the property and if successful get the value
		LPCOLESTR lpOleStr = T2COLE(pszPropName);
		if (SUCCEEDED(ppDisp[i]->GetIDsOfNames(IID_NULL, (LPOLESTR*)&lpOleStr,
			1, m_lcid, &dwDispID)))
		{
			LPDISPATCH pFontDisp;

			// Get property
			PropDispDriver.AttachDispatch(ppDisp[i], FALSE);
			PropDispDriver.GetProperty(dwDispID, VT_DISPATCH, &pFontDisp);
			PropDispDriver.DetachDispatch();

			if (pFontDisp == NULL)
				continue;

			// Get font interface
			IFont * pFont;
			HRESULT hresult = pFontDisp->QueryInterface(IID_IFont, (void**)&pFont);
			if (hresult == S_OK)
			{
				BOOL bTemp;

				// Set font characteristics
				OLECHAR *pszName;
				pFont->get_Name(&pszName);
				if (lstrcmp(OLE2CT(pszName), pfobj->strName) != 0 && i != 0)
					pmobj->bNameOK = FALSE;
				pfobj->strName = pszName;
				SysFreeString(pszName);

				CY cyTemp;
				pFont->get_Size(&cyTemp);
				if ((cyTemp.Lo != pfobj->cySize.Lo || cyTemp.Hi != pfobj->cySize.Hi) && i != 0)
					pmobj->bSizeOK = FALSE;
				pfobj->cySize = cyTemp;

				pFont->get_Bold(&bTemp);
				if (pfobj->bBold != bTemp && i != 0)
					pmobj->bStyleOK = FALSE;
				pfobj->bBold = bTemp;

				pFont->get_Italic(&bTemp);
				if (pfobj->bItalic != bTemp && i != 0)
					pmobj->bStyleOK = FALSE;
				pfobj->bItalic = bTemp;

				pFont->get_Underline(&bTemp);
				if (pfobj->bUnderline != bTemp && i != 0)
					pmobj->bUnderlineOK = FALSE;
				pfobj->bUnderline = bTemp;

				pFont->get_Strikethrough(&bTemp);
				if (pfobj->bStrikethrough != bTemp && i != 0)
					pmobj->bStrikethroughOK = FALSE;
				pfobj->bStrikethrough = bTemp;

				short sTemp;

				pFont->get_Weight(&sTemp);
				if (pfobj->sWeight != sTemp && i != 0)
					pmobj->bStyleOK = FALSE;
				pfobj->sWeight = sTemp;

				// Release the font interface
				RELEASE(pFont);
				bStatus = TRUE;
			}

			// Release font interface
			RELEASE(pFontDisp);
		}
	}
	return bStatus;
}

BOOL CFontPropPage::OnInitDialog()
{
	CStockPropPage::OnInitDialog();
	OnObjectsChanged();
	IgnoreApply(AFX_IDC_FONTPROP);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

AFX_STATIC_DATA int _afxTTDefaults[] = { 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48 };

void CFontPropPage::FillSizeList()
{
	// Clear the size list
	m_FontSizes.ResetContent();
	m_FontStyles.ResetContent();
	m_nStyles = 0L;

	// Fill with "real" sizes
	CString strFaceName;
	m_FontNames.GetLBText(m_FontNames.GetCurSel(), strFaceName);
	CClientDC dc(NULL);
	EnumFontFamilies(dc.GetSafeHdc(), (LPCTSTR) strFaceName, (FONTENUMPROC) CFontPropPage::EnumFontFamiliesCallBack2, (LPARAM) this);

	// Check if we have a font that is either a vector or Truettype font
	if (m_FontNames.GetFontType() != RASTER_FONTTYPE)
	{
		// Fill with "common" sizes
		for (int i = 0; i < _countof(_afxTTDefaults); i++)
			m_FontSizes.AddSize(_afxTTDefaults[i], 0);
	}

	// See what fonts are native
	BOOL	bRegular = (BOOL)(m_nStyles & NTM_REGULAR);
	BOOL	bBold = (BOOL)(m_nStyles & NTM_BOLD);
	BOOL	bItalic = (BOOL)(m_nStyles & NTM_ITALIC);
	BOOL	bBoldItalic = (BOOL)((m_nStyles & NTM_BOLD) &&
								 (m_nStyles & NTM_ITALIC));

	// Allow for "synthesized" italic && bold variants
	if (bRegular)
		bBold = bItalic = TRUE;
	if (bBold || bItalic)
		bBoldItalic = TRUE;

	// Fill the styles list box
	CString strStyle;

	int nEntry;
	if (bRegular)
	{
		strStyle.LoadString(AFX_IDS_REGULAR);
		nEntry = m_FontStyles.AddString(strStyle);
		m_FontStyles.SetItemData(nEntry, (DWORD)NTM_REGULAR);
	}
	if (bBold)
	{
		strStyle.LoadString(AFX_IDS_BOLD);
		nEntry = m_FontStyles.AddString(strStyle);
		m_FontStyles.SetItemData(nEntry, (DWORD)NTM_BOLD);
	}
	if (bItalic)
	{
		strStyle.LoadString(AFX_IDS_ITALIC);
		nEntry = m_FontStyles.AddString(strStyle);
		m_FontStyles.SetItemData(nEntry, (DWORD)NTM_ITALIC);
	}
	if (bBoldItalic)
	{
		strStyle.LoadString(AFX_IDS_BOLDITALIC);
		nEntry = m_FontStyles.AddString(strStyle);
		m_FontStyles.SetItemData(nEntry, (DWORD)NTM_ITALIC|NTM_BOLD);
	}

	// Set the point size
	if (m_FontSizes.FindString(-1, m_strFontSize) != CB_ERR)
	{
		nEntry = m_FontSizes.SelectString(-1, m_strFontSize);
		if (nEntry == CB_ERR)
			return;
	}
	else
	{
		// Point size is not in the list so just fill the edit box
		// and don't select anything from the list
		m_FontSizes.SetCurSel(-1);
		m_FontSizes.SetWindowText(m_strFontSize);
	}

	// Set the styles combo box selection
	BOOL bFound = FALSE;
	int nMaxEntries = m_FontStyles.GetCount();
	for (int nEntry3 = 0; nEntry3 < nMaxEntries; nEntry3++)
	{
		if (m_FontStyles.GetItemData(nEntry3) == m_nActualStyle)
		{
			m_FontStyles.SetCurSel(nEntry3);
			bFound = TRUE;
		}
	}

	if (!bFound)
	{
		m_FontStyles.SetCurSel(0);		// Set style to regular
		m_nCurrentStyle = NTM_REGULAR;
	}
	else
		m_nCurrentStyle = m_nActualStyle;

	// Redraw the sample
	UpdateSampleFont();
}

int CALLBACK CFontPropPage::EnumFontFamiliesCallBack2(
	ENUMLOGFONT* lpelf, NEWTEXTMETRIC* lpntm, int FontType, LPARAM lParam)
{
	CFontPropPage *pDlg = (CFontPropPage *)lParam;
	ASSERT(pDlg != NULL);

	if (FontType & TRUETYPE_FONTTYPE)
	{
		if (!(lpntm->ntmFlags & (NTM_BOLD | NTM_ITALIC)))
			pDlg->m_nStyles |= NTM_REGULAR;

		if (lpntm->ntmFlags & NTM_ITALIC)
			pDlg->m_nStyles |= NTM_ITALIC;

		if (lpntm->ntmFlags & NTM_BOLD)
			pDlg->m_nStyles |= NTM_BOLD;
	}
	else
	{
		if (FontType & RASTER_FONTTYPE)
		{
			int height = lpntm->tmHeight - lpntm->tmInternalLeading;
			pDlg->m_FontSizes.AddSize(MulDiv(height, 72, afxData.cyPixelsPerInch), height);
		}

		if (lpelf->elfLogFont.lfWeight >= FW_BOLD && lpelf->elfLogFont.lfItalic)
			pDlg->m_nStyles |= NTM_BOLD | NTM_ITALIC;
		else if (lpelf->elfLogFont.lfWeight >= FW_BOLD)
			pDlg->m_nStyles |= NTM_BOLD;
		else if (lpelf->elfLogFont.lfItalic)
			pDlg->m_nStyles |= NTM_ITALIC;
		else
			pDlg->m_nStyles |= NTM_REGULAR;
	}

	return 1;
}









/////////////////////////////////////////////////////////////////////////////
// CFontPropPage message handlers

void CFontPropPage::OnEditupdateFontnames()
{
	// When the users entry matches an entry in the list, select it
	CString str;
	m_FontNames.GetWindowText(str);
	int nEntry = m_FontNames.FindStringExact(-1, str);
	if (nEntry != CB_ERR)
	{
		m_FontNames.SetCurSel(nEntry);
		m_FontNames.SetEditSel(-1, -1);

		// Re-fill the size list
		FillSizeList();
	}
}

void CFontPropPage::OnEditupdateFontsizes()
{
	// when the users entry matches an entry in the list, select it
	m_FontSizes.GetWindowText(m_strFontSize);
	int nEntry = m_FontSizes.FindStringExact(-1, m_strFontSize);
	if (nEntry != CB_ERR)
	{
		m_FontSizes.SetCurSel(nEntry);
		m_FontSizes.SetEditSel(-1, -1);

		// Update the sample text
		UpdateSampleFont();
	}
}

void CFontPropPage::OnSelchangeFontnames()
{
	FillSizeList();
}

void CFontPropPage::UpdateSampleFont()
{
	ASSERT(m_FontNames.GetFontItem());

	LOGFONT lf = *m_FontNames.GetLogFont();
	m_FontSizes.UpdateLogFont( &lf );

	// Handle styles
	if (m_nCurrentStyle & NTM_BOLD)
		lf.lfWeight = FW_BOLD;
	else
		lf.lfWeight = FW_REGULAR;
	if (m_nCurrentStyle & NTM_ITALIC)
		lf.lfItalic = TRUE;
	else
		lf.lfItalic = FALSE;

	lf.lfStrikeOut = (unsigned char)m_bStrikeOut;
	lf.lfUnderline = (unsigned char)m_bUnderline;

	SampleFont.DeleteObject();
	SampleFont.CreateFontIndirect( &lf );

	CRect rcSample;
	m_SampleBox.GetWindowRect( &rcSample );
	ScreenToClient( &rcSample );

	InvalidateRect( rcSample );
	UpdateWindow();
}

void CFontPropPage::OnPaint()
{
	CPaintDC dc(this);
	CRect rcText;
	CFont *oldFont;
	CSize TextExtent;
	COLORREF crText;
	TEXTMETRIC tm;
	int bkMode, len, x, y;
	CString strSample;

	strSample.LoadString(AFX_IDS_SAMPLETEXT);

	// If there is no sample font abort
	if (!SampleFont.GetSafeHandle())
		return;

	// Get the bounding box
	m_SampleBox.GetWindowRect( &rcText );
	ScreenToClient( &rcText );

	// Select the new font and colors into the dc
	oldFont = dc.SelectObject( &SampleFont );
	crText = dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
	bkMode = dc.SetBkMode(TRANSPARENT);

	// Calculate the position of the text
	dc.GetTextMetrics( &tm );

	len = strSample.GetLength();
	TextExtent = dc.GetTextExtent(strSample, len);
	TextExtent.cy = tm.tmAscent - tm.tmInternalLeading;

	if ((TextExtent.cx >= (rcText.right - rcText.left)) ||
			(TextExtent.cx <= 0))
		x = rcText.left;
	else
		x = rcText.left + ((rcText.right - rcText.left) - TextExtent.cx) / 2;

	y = min(rcText.bottom,
		rcText.bottom - ((rcText.bottom - rcText.top) - TextExtent.cy) / 2);

	// Draw it
	dc.ExtTextOut(x, y - (tm.tmAscent), ETO_CLIPPED, &rcText,
		strSample, len, NULL);

	// Put the DC back the way it was
	dc.SetBkMode(bkMode);
	dc.SetTextColor(crText);

	if (oldFont)
		dc.SelectObject(oldFont);
}

void CFontPropPage::OnSelchangeFontsizes()
{
	int nEntry = m_FontSizes.GetCurSel();
	if (nEntry != CB_ERR)
	{
		m_FontSizes.GetLBText(nEntry, m_strFontSize);
		UpdateSampleFont();
	}
}
void CFontPropPage::OnSelchangeFontstyles()
{
	int nEntry = m_FontStyles.GetCurSel();
	m_nCurrentStyle = m_FontStyles.GetItemData(nEntry);
	m_nActualStyle = m_nCurrentStyle;

	// Update the sample font
	UpdateSampleFont();
}

void CFontPropPage::OnEditchangeFontstyles()
{
	// when the users entry matches an entry in the list, select it
	CString str;
	m_FontStyles.GetWindowText(str);
	int nEntry = m_FontStyles.FindStringExact(-1, str);
	if (nEntry != CB_ERR)
	{
		m_FontStyles.SetCurSel(nEntry);
		m_FontStyles.SetEditSel(-1, -1);

		// Update the sample text
		m_nCurrentStyle = m_FontStyles.GetItemData(nEntry);
		m_nActualStyle = m_nCurrentStyle;
		UpdateSampleFont();
	}
}

void CFontPropPage::SelectFontFromList(CString strFaceName, MERGEOBJECT* pmobj)
{
	// Set the effects buttons
	CButton* pStrikeOut = (CButton*) GetDlgItem(AFX_IDC_STRIKEOUT);
	if (!pmobj->bStrikethroughOK)
		pStrikeOut->SetCheck(2);
	else if (m_bStrikeOut)
		pStrikeOut->SetCheck(1);
	else
		pStrikeOut->SetCheck(0);

	CButton* pUnderline = (CButton*) GetDlgItem(AFX_IDC_UNDERLINE);
	if (!pmobj->bUnderlineOK)
		pStrikeOut->SetCheck(2);
	else if (m_bUnderline)
		pUnderline->SetCheck(1);
	else
		pUnderline->SetCheck(0);

	// Set the font facename
	if (pmobj->bNameOK)
	{
		int nEntry1 = m_FontNames.SelectString(-1, strFaceName);
		if (nEntry1 == CB_ERR)
			return;
	}

	// Fill the size list appropriately
	FillSizeList();

	// Set the styles combo box selection
	BOOL bFound = FALSE;
	int nMaxEntries = m_FontStyles.GetCount();
	for (int nEntry3 = 0; nEntry3 < nMaxEntries; nEntry3++)
	{
		if (m_FontStyles.GetItemData(nEntry3) == m_nActualStyle)
		{
			m_FontStyles.SetCurSel(nEntry3);
			bFound = TRUE;
		}
	}

	if (pmobj->bSizeOK)
	{
		if (!bFound)
		{
			m_FontStyles.SetCurSel(0);		// Set style to regular
			m_nCurrentStyle = NTM_REGULAR;
		}
		else
			m_nCurrentStyle = m_nActualStyle;
	}

	UpdateSampleFont();
}

void CFontPropPage::OnStrikeout()
{
	CButton* pStrikeOut = (CButton*) GetDlgItem(AFX_IDC_STRIKEOUT);
	if (pStrikeOut->GetCheck() == 1)
		m_bStrikeOut = TRUE;
	else
		m_bStrikeOut = FALSE;

	UpdateSampleFont();
}

void CFontPropPage::OnUnderline()
{
	CButton* pUnderline = (CButton*) GetDlgItem(AFX_IDC_UNDERLINE);
	if (pUnderline->GetCheck() == 1)
		m_bUnderline = TRUE;
	else
		m_bUnderline = FALSE;

	UpdateSampleFont();
}

void CFontPropPage::OnSelchangeFontprop()
{
	OnSelchangePropname(m_FontProp);
}

BOOL CFontPropPage::OnEditProperty(DISPID dispid)
{
	return CStockPropPage::OnEditProperty(dispid, m_FontProp);
}

void CFontPropPage::OnObjectsChanged()
{
	ULONG nObjects;
	if (GetObjectArray(&nObjects) != NULL && m_hWnd != NULL)
	{
		FillPropnameList(IID_IFontDisp, 1, m_FontProp);

		if ( m_FontProp.GetCount() > 0 )
			FillFacenameList();
		else
		{
			m_FontNames.EnableWindow(FALSE);
			m_FontSizes.EnableWindow(FALSE);
			m_FontStyles.EnableWindow(FALSE);
			GetDlgItem(AFX_IDC_STRIKEOUT)->EnableWindow(FALSE);
			GetDlgItem(AFX_IDC_UNDERLINE)->EnableWindow(FALSE);
		}
	}

	if (m_hWnd != NULL)
		OnSelchangeFontprop();
}

/////////////////////////////////////////////////////////////////////////////
// Class factory for Font property page

#ifdef _AFXDLL

#ifdef AFXCTL_FACT_SEG
#pragma code_seg(AFXCTL_FACT_SEG)
#endif

IMPLEMENT_OLECREATE_EX(CFontPropPage, "OCxx.CFontPropPage",
	0x0be35200,0x8f91,0x11ce,0x9d,0xe3,0x00,0xaa,0x00,0x4b,0xb8,0x51)

BOOL CFontPropPage::CFontPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, AFX_IDS_FONT_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}

#endif //_AFXDLL

/////////////////////////////////////////////////////////////////////////////
// Force any extra compiler-generated code into AFX_INIT_SEG

#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif

IMPLEMENT_DYNCREATE(CFontPropPage, CStockPropPage)

















/////////////////////////////////////////////////////////////////////////////
// CSizeComboBox

int CSizeComboBox::AddSize(int PointSize, LONG lfHeight)
{
	if (lfHeight == 0)
		lfHeight = MulDiv( -afxData.cyPixelsPerInch, PointSize, 72 );

	CString str;
	wsprintf(str.GetBuffer(16), _T("%d"), PointSize);
	str.ReleaseBuffer();

	int nMaxEntries = GetCount();
	int nEntry;

	// we use positive height values for non-truetype fonts, negitive for true type
	if (lfHeight > 0)
	{
		for (nEntry = 0; nEntry < nMaxEntries; nEntry++)
		{
			int iComp = (int)(lfHeight - GetHeight(nEntry));
			if (!iComp)
				return CB_ERR;
			if (iComp < 0)
				break;
		}
	}
	else
	{
		for (nEntry = 0; nEntry < nMaxEntries; nEntry++)
		{
			int iComp = (int)(lfHeight - GetHeight(nEntry));
			if (!iComp)
				return CB_ERR;
			if (iComp > 0)
				break;
		}
	}

	if (nEntry == nMaxEntries)
		nEntry = -1;
	nEntry = InsertString(nEntry, str);
	if (nEntry != CB_ERR)
		SetItemData(nEntry, (DWORD)lfHeight);

	return nEntry;
}

void CSizeComboBox::GetPointSize(CY& cy)
{
	TCHAR szText[20];
	GetWindowText(szText, 20);
	cy.Lo = 0;
	cy.Hi = 0;
	_AfxCyFromString(cy, szText);
}

LONG CSizeComboBox::GetHeight(int sel)
{
	if (sel == -1)
		sel = GetCurSel();

	if (sel == -1)
	{
		TCHAR szText[20];
		GetWindowText(szText, 20);
		sel = FindString( -1, szText);
		if (sel == CB_ERR)
		{
			CY cyTmp;
			cyTmp.Lo = 0;
			cyTmp.Hi = 0;
			_AfxCyFromString(cyTmp, szText);
			int PointSize = (int)((cyTmp.Lo + 5000) / 10000);
			if (PointSize != 0)
				return MulDiv(-afxData.cyPixelsPerInch, PointSize, 72);
			else
				sel = 0;
		}
	}

	return (LONG) GetItemData(sel);
}

void CSizeComboBox::UpdateLogFont( LPLOGFONT lpLF, int sel )
{
	ASSERT(lpLF);

	lpLF->lfHeight = (int)GetHeight(sel);
	lpLF->lfWidth = 0;
}

#endif // USE_THIS
