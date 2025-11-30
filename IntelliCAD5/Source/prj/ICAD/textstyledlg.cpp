/* PRJ\ICAD\TEXTSTYLEDLG.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * Abstract
 * Implements the Style command's dialog.
 */

#include "icad.h"				/*DNT*/
#include "IcadApi.h"			/*DNT*/
#include "TextStyleDlg.h"		/*DNT*/
#include "InputEditBoxDlg.h"	/*DNT*/
#include "ValueValidator.h"		/*DNT*/
#include "Resbuf.h"				/*DNT*/
#include "paths.h"				/*DNT*/
#include "cmds.h"				/*DNT*/
#include "resource.hm"			/*DNT*/
#include "undo\UndoSaver.h"/*DNT*/


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



extern bool SDS_DontUpdateDisp;



IMPLEMENT_DYNAMIC( CTextStyleDlg, CDialog )


/////////////////////////////////////////////////////////////////////////////
// CTextStyleDlg construction / destruction
/******************************************************************************
*+ CTextStyleDlg::CTextStyleDlg - initializes member variables
*
*/

CTextStyleDlg::CTextStyleDlg(
	CWnd* pParent /*=NULL*/)
	: CDialog(CTextStyleDlg::IDD, pParent)
	{

	//{{AFX_DATA_INIT(CTextStyleDlg)
	//}}AFX_DATA_INIT

	// initialize the values to what we want;
	// do this after the wizard initializes the values
	m_dirtyFlag = DIRTY_NONE;
	m_bNeedRegen = false;	/*DG - 9.1.2002*/
	}



/////////////////////////////////////////////////////////////////////////////
// CTextStyleDlg implementation

/******************************************************************************
*+ CTextStyleDlg::DoDataExchange - initializes dialog controls and retrieves
*  dialog values
*
*/
/* EBATECH(CNBR) 2001-06-14 Add BIGFONT and LANGUAGE CONTROL */

void CTextStyleDlg::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextStyleDlg)
	DDX_Control(pDX, IDAPPLY, m_applyCtrl);
	DDX_Control(pDX, EXP_STYLE_PREVIEW, m_previewCtrl);
	DDX_Control(pDX, EXP_STYLE_FONT_STYLE, m_fontStyleCtrl);
	DDX_Control(pDX, EXP_STYLE_FONT_NAME, m_fontNameCtrl);
	DDX_Control(pDX, EXP_STYLE_VERTICALLY, m_verticallyCtrl);
	DDX_Control(pDX, EXP_STYLE_UPSIDE_DOWN, m_upsideDownCtrl);
	DDX_Control(pDX, EXP_STYLE_BACKWARDS, m_backwardsCtrl);
	DDX_Control(pDX, EXP_STYLE_OBLIQUE, m_obliqueAngleCtrl);
	DDX_Control(pDX, EXP_STYLE_WIDTH, m_widthCtrl);
	DDX_Control(pDX, EXP_STYLE_HEIGHT, m_heightCtrl);
	DDX_Control(pDX, TEXT_STYLE_DELETE, m_deleteCtrl);
	DDX_Control(pDX, TEXT_STYLE_RENAME, m_renameCtrl);
	DDX_Control(pDX, EXP_STYLE_NAME, m_styleNameCtrl);
	DDX_Control(pDX, EXP_STYLE_BIGFONT_NAME, m_bigfontNameCtrl);
	DDX_Control(pDX, EXP_STYLE_FONT_LANGUAGE, m_fontLangCtrl);
	//}}AFX_DATA_MAP
	}

/******************************************************************************
*+ CTextStyleDlg::FillStylesComboBox - fills the Current Style Name combo box
*  with the names of the text styles in the current drawing
*
*/

void CTextStyleDlg::FillStylesComboBox(
	const char *defaultStyle)		// i - the style to select initially;
	{								// NULL or a NULL string selects the first style in the list

	// get the STYLE table's index
	int tableIndex = db_tabrec2tabfn(db_str2hitype("STYLE"/*DNT*/));
	if (!db_is_tabrectype(tableIndex))
		return;

	// add the name of each style to the combo box
	char *styleName;
	db_styletabrec *styleRecord = (db_styletabrec *) (SDS_CURDWG->tblnext(tableIndex, TRUE));
	while (styleRecord)
		{
		// EBATECH(CNBR) -[ 2002/4/16 Remove Shapefont from list
		if((styleRecord->ret_flags() & IC_SHAPEFILE_SHAPES))
			styleName = NULL;
		else
			{
		// EBATECH(CNBR) ]-
			styleRecord->get_2(&styleName);
			if (((CString) styleName).Find("|") == -1)
				m_styleNameCtrl.AddString(styleName);
			}
		styleRecord = (db_styletabrec *) (SDS_CURDWG->tblnext(tableIndex, FALSE));
		}

	// if there is no default style name, set to the first style;
	// otherwise, set to the default style name
	if ((NULL == defaultStyle) || ('\0'/*DNT*/ == defaultStyle[0]))
		m_styleNameCtrl.SetCurSel(0);
	else
		{
		int comboIndex = m_styleNameCtrl.FindStringExact(-1, defaultStyle);
		if (CB_ERR == comboIndex)
			m_styleNameCtrl.SetCurSel(0);
		else
			m_styleNameCtrl.SetCurSel(comboIndex);
		}
	}

/******************************************************************************
*+ CTextStyleDlg::GetTextStyleValues - store style values for use elsewhere
*
*  Retrieves the values for a text style from a style table record and stores
*  them in the appropriate member variables.
*/

// to do - this function and the function with the same name in CExplorerPropsTab
//		   can be merged if we pass in the variables that will store the values,
//		   preferably in a structure; this will require modifying some code in
//		   IntelliCAD Explorer, but will reuse code and will ensure that we get the
//		   values in the same way in both IntelliCAD Explorer and the Style command

void CTextStyleDlg::GetTextStyleValues(		// store style values for use elsewhere
	db_styletabrec *styleRec)				// i - style record to get values from
	{
	char *tmpStr, tmpBuf[IC_ACADBUF];
	sds_real tmpReal;
	int tmpInt;

	styleRec->get_2(&tmpStr);
	m_tableInfo.SetStyleName(tmpStr);

	bool hasTrueTypeFont = styleRec->hasTrueTypeFont();
	m_tableInfo.SetHasTrueTypeFont(false);
	if (hasTrueTypeFont)
		{
		m_tableInfo.SetHasTrueTypeFont(true);

		styleRec->getFontFamily(tmpBuf);
		m_tableInfo.SetFontFamily(tmpBuf);

		long tmpLong;
		styleRec->getFontStyleNumber(&tmpLong);
		m_tableInfo.SetFontStyle(tmpLong);

		m_charSet = ((tmpLong & 0x0000ff00) >> 8); // EBATECH(CNBR)
		m_weight = FW_NORMAL;
		m_italic = false;
		if ((0x3000000 & m_tableInfo.GetFontStyle()) == 0x3000000) // Bold & italic
			{
			m_weight = FW_BOLD;
			m_italic = true;
			}
		else if (0x2000000 & m_tableInfo.GetFontStyle())			// Bold
			m_weight = FW_BOLD;
		else if (0x1000000 & m_tableInfo.GetFontStyle())			// Italic
			m_italic = true;
		}

	styleRec->get_3(tmpBuf, IC_ACADBUF - 1);
	if (!hasTrueTypeFont)					// sometimes shx fonts don't have an extension,
		ic_setext(tmpBuf, ".shx"/*DNT*/);	// we need it for finding the font in combo boxes
	else
		ic_setext(tmpBuf, ".ttf"/*DNT*/);	// we need it for finding the font in combo boxes

	m_tableInfo.SetFontFilename(tmpBuf);

	// EBATECH(CNBR) 2001-06-14 -[ Bigfont
	styleRec->get_4(tmpBuf, IC_ACADBUF - 1);
	m_tableInfo.SetBigFontFilename(tmpBuf);
	// EBATECH(CNBR) ]-

	styleRec->get_40(&tmpReal);
	m_tableInfo.SetHeight(tmpReal);

	styleRec->get_41(&tmpReal);
	m_tableInfo.SetWidth(tmpReal);

	styleRec->get_50(&tmpReal);
	m_tableInfo.SetObliqueAngle(tmpReal);

	styleRec->get_70(&tmpInt);
	m_tableInfo.SetVertically((tmpInt & 4) ? TRUE : FALSE);

	styleRec->get_71(&tmpInt);
	m_tableInfo.SetBackwards((tmpInt & 2) ? TRUE : FALSE);
	m_tableInfo.SetUpsideDown((tmpInt & 4) ? TRUE : FALSE);

	}

/******************************************************************************
*+ CTextStyleDlg::DisplayTextStyleValues - displays the values for the current
*  text style in the dialog controls
*
*/

void CTextStyleDlg::DisplayTextStyleValues(
	const char *styleName)
	{
	char tmpBuf[IC_ACADBUF];

	sprintf(tmpBuf, "%G"/*DNT*/, m_tableInfo.GetHeight());
	m_heightCtrl.SetWindowText(tmpBuf);

	sprintf(tmpBuf, "%G"/*DNT*/, m_tableInfo.GetWidth());
	m_widthCtrl.SetWindowText(tmpBuf);

	sprintf(tmpBuf, "%G"/*DNT*/, (180 / IC_PI) * m_tableInfo.GetObliqueAngle());
	m_obliqueAngleCtrl.SetWindowText(tmpBuf);

	m_backwardsCtrl.SetCheck(m_tableInfo.GetBackwards());
	m_upsideDownCtrl.SetCheck(m_tableInfo.GetUpsideDown());
	m_verticallyCtrl.SetCheck(m_tableInfo.GetVertically());

	CString tmpFontName = m_tableInfo.GetHasTrueTypeFont() ?
						  m_tableInfo.GetFontFamily() :
						  m_tableInfo.GetFontFilename(false);
	m_fontNameCtrl.SetCurrentFont(tmpFontName);

	SetApplyButton();

	if(m_tableInfo.GetHasTrueTypeFont())
		GetStyleSel();

	if(strisame(styleName, "STANDARD"/*DNT*/))
		{
		m_renameCtrl.EnableWindow(FALSE);
		m_deleteCtrl.EnableWindow(FALSE);
		}
	else
		{
		m_renameCtrl.EnableWindow(TRUE);
		m_deleteCtrl.EnableWindow(TRUE);
		}
	// EBATECH(CNBR) -[ 2001.06.14
	tmpFontName = m_tableInfo.GetBigFontFilename(false);
	if(tmpFontName.Compare("") == 0){
		tmpFontName = CString(ResourceString(IDC_EXPSTYLE_NO_BIGFONT,"(none)"/*DNT*/));
	}
	m_bigfontNameCtrl.SetCurrentFont(tmpFontName);
	// EBATECH(CNBR) ]-

	}

void CTextStyleDlg::GetStyleSel()
{
	// Now take care of Bold/Italic/BoldItalic settings.
	// We cannot search for the name "bold/italic/bolditalic" because it changes from
	// program to program, so we must find the closest match of weight and italic.
	int weight[5];
	int italic[5];
	int final = -1;  //the index that is the closest match.
	bool test = false;	//we found a match
	for (int i = 0; m_fontStyleCtrl.GetCount() > i; i++){
		weight[i] = ((StyleItem*) (m_fontStyleCtrl.GetItemDataPtr(i)))->weight;
		italic[i] = ((StyleItem*) (m_fontStyleCtrl.GetItemDataPtr(i)))->italic;
	}
	if ((0x3000000 & m_tableInfo.GetFontStyle()) == 0x3000000){ // Bold & italic
		for (int j = 0; j < i; j++){
			if (italic[j] == 255 && (final < 0 || weight[final] < weight[j])){ // look for darkest font with
				final = j;		// italics we can find.
				test = true;
			}
		}
		if (!test){		// failed!	Ditch italics - find the darkest available font.
			final = 0;
			for (j = 0; j < i; j++){
				if (weight[final] < weight[j]){
					final = j;
				}
			}
		}
	}else if (0x2000000 & m_tableInfo.GetFontStyle()){			// Bold
		for (int j = 0; j < i; j++){
			if (italic[j] == 0 && (final < 0 || weight[final] < weight[j])){
				final = j;
				test = true;
			}
		}
		if (!test){		// failed!	Now accept italic fonts.
			final = 0;
			for (j = 0; j < i; j++){
				if (weight[final] < weight[j]){
					final = j;
				}
			}
		}
	}else if (0x1000000 & m_tableInfo.GetFontStyle()){			// Italic
		for (int j = 0; j < i; j++){
			// conditions: italics, non-bold, and as close to FW_NORMAL as possible.
			if (italic[j] == 255 && weight[j] < 600 &&
				(final < 0 || abs(weight[j] - 400) < abs(weight[final] - 400))){
				test = true;
				final = j;
			}
		}
		if (!test){	// failed!	Allow bold fonts.
			for (j = 0; j < i; j++){
				if (italic[j] == 255 &&
					(final < 0 || abs(weight[j] - 400) < abs(weight[final] - 400))){
					test = true;
					final = j;
				}
			}
			if (!test){	// failed - find closest match to 400 weight.
				final = 0;
				for (j = 0; j < i; j++){
					if (abs(weight[j] - 400) < abs(weight[final] - 400)){
						final = j;
					}
				}
			}
		}
	}else{														// Regular
		for (int j = 0; j < i; j++){
			if (italic[j] == 0 && weight[j] < 600 &&
				(final < 0 || abs(weight[j] - 400) < abs(weight[final] - 400))){
				test = true;
				final = j;
			}
			if (!test){
				if (italic[j] == 0 &&
					(final < 0 || abs(weight[j] - 400) < abs(weight[final] - 400))){
					test = true;
					final = j;
				}
			}
			if (!test){
				final = 0;
				if (abs(weight[j] - 400) < abs(weight[final] - 400)){
					final = j;
				}
			}
		}
	}
	m_fontStyleCtrl.SetCurSel(final);
	// EBATECH(CNBR) -[ Set Language
	int charSet = (( m_tableInfo.GetFontStyle() & 0x0000ff00 ) >> 8 );
	final = false;
	for (i = 0; m_fontLangCtrl.GetCount() > i; i++){
		if( charSet == (int)m_fontLangCtrl.GetItemData(i)){
			m_fontLangCtrl.SetCurSel(i);
			final = true;
			break;
		}
	}
	if(final == false){
		m_fontLangCtrl.SetCurSel(0);
	}
	// EBATECH(CNBR) ]-
}

/******************************************************************************
*+ CTextStyleDlg::DrawFontPreview - draws a preview of the specified font
*
*  Draws a sample of the specified font according to the width factor, oblique
*  angle, backwards, upside down, vertically, font name, and font style
*  settings.
*
*/

void CTextStyleDlg::DrawFontPreview(
	void)
	{
	int fontCount;

	// only draw the preview if there are any fonts
	fontCount = m_fontNameCtrl.GetCount();
	if ((0 == fontCount) || (CB_ERR == fontCount))
		{
		m_previewCtrl.SetWindowText(""/*DNT*/);
		return;
		}

	m_previewCtrl.m_fontType		= m_fontNameCtrl.GetFontType(m_fontNameCtrl.GetCurSel());
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
			m_previewCtrl.m_fontItem = fontItem;
		}

	m_previewCtrl.DrawStyleFontPreview();
	}

/******************************************************************************
*+ CTextStyleDlg::UpdateDirtyFlag - updates m_dirtyFlag for the specified value
*
*/

void CTextStyleDlg::UpdateDirtyFlag(
	const sds_real origValue,
	const sds_real newValue,
	const int flagValue)
	{

	if (origValue != newValue)
		{
		if (!(m_dirtyFlag & flagValue))			// set to dirty if not already set
			m_dirtyFlag += flagValue;
		}
	else
		{
		if (m_dirtyFlag & flagValue)			// clear just this flag value if already set
			m_dirtyFlag -= flagValue;
		}
	}

/******************************************************************************
*+ CTextStyleDlg::UpdateDirtyFlag - updates m_dirtyFlag for the specified value
*
*/

void CTextStyleDlg::UpdateDirtyFlag(
	const CString& origValue,
	const CString& newValue,
	const int flagValue)
	{

	if (!strisame(origValue, newValue))
		{
		if (!(m_dirtyFlag & flagValue))			// set to dirty if not already set
			m_dirtyFlag += flagValue;
		}
	else
		{
		if (m_dirtyFlag & flagValue)			// clear just this flag value if already set
			m_dirtyFlag -= flagValue;
		}
	}

/******************************************************************************
*+ CTextStyleDlg::UpdateDirtyFlag - updates m_dirtyFlag for the specified value
*
*/

void CTextStyleDlg::UpdateDirtyFlag(
	const BOOL origValue,
	const BOOL newValue,
	const int flagValue)
	{

	if (origValue != newValue)
		{
		if (!(m_dirtyFlag & flagValue))			// set to dirty if not already set
			m_dirtyFlag += flagValue;
		}
	else
		{
		if (m_dirtyFlag & flagValue)			// clear just this flag value if already set
			m_dirtyFlag -= flagValue;
		}
	}

/******************************************************************************
*+ CTextStyleDlg::UpdateDirtyFlag - updates m_dirtyFlag for the specified value
*
*/

void CTextStyleDlg::UpdateDirtyFlag(
	const long origValue,
	const long newValue,
	const int flagValue)
	{

	if (origValue != newValue)
		{
		if (!(m_dirtyFlag & flagValue))			// set to dirty if not already set
			m_dirtyFlag += flagValue;
		}
	else
		{
		if (m_dirtyFlag & flagValue)			// clear just this flag value if already set
			m_dirtyFlag -= flagValue;
		}
	}

/******************************************************************************
*+ CTextStyleDlg::SetApplyButton - enables/disables the Apply button
*
*  Enables/disables the Apply button according to the value of m_dirtyFlag.
*
*/

inline void CTextStyleDlg::SetApplyButton(
	void)
	{
		ASSERT(m_dirtyFlag >= DIRTY_NONE);

		if (DIRTY_NONE == m_dirtyFlag)
			m_applyCtrl.EnableWindow(FALSE);
		else
			m_applyCtrl.EnableWindow(TRUE);
	}

/******************************************************************************
*+ CTextStyleDlg::ConvertObliqueAngle - converts an angle to be from -85 to 85
*
*  If the angle will not fit within this range, it is left as is.
*
*/

bool CTextStyleDlg::ConvertObliqueAngle(
	sds_real& value)
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
*+ CTextStyleDlg::PromptToSaveStyle - prompts the user to save the style changes
*
*/

bool CTextStyleDlg::PromptToSaveStyle(
	void)
	{
	if (IDYES == MessageBox(ResourceString(IDC_TEXTSTYLE_SAVE_CHANGES, "The current style has been modified.\nDo you wish to save the changes?"),
			   ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),
			   MB_ICONQUESTION | MB_YESNO))
		return true;
	else
		return false;
	}

/******************************************************************************
*+ CTextStyleDlg::SaveTextStyle - saves the style values from the dialog
*
*  Updates the values that have changed for the current text style.
*
*  If an error occurs during the process, the values that have changed up to that
*  point are modified but the remaining values are left unchanged.
*
*  NOTE:  This function was built from CExplorerPropsTab::RestoreTextStyleValues.
*		  When that function is redone (see note there), the code here and the
*		  code there should be able to be shared to avoid redundancy.
*
*/

void CTextStyleDlg::SaveTextStyle(
	void)
	{
	CTextStyleTableInfo dlgValues;

	GetStyleValuesFromDialog(dlgValues);
	if (dlgValues.GetFontStyle() < 0) // error
		dlgValues.SetFontStyle(m_tableInfo.GetFontStyle());
	RestoreTextStyleValues(m_tableInfo, dlgValues);
	/*DG - 9.1.2002*/// Need regen all.
	m_bNeedRegen = true;
	}

/******************************************************************************
*+ CTextStyleDlg::GetStyleValuesFromDialog - gets the style values from the dialog
*
*/

void CTextStyleDlg::GetStyleValuesFromDialog(
	CTextStyleTableInfo& values)
{
	CString dlgStrValue;

	values.SetDrawing(SDS_CURDWG);
	values.SetStyleName(m_tableInfo.GetStyleName());			// to do - use this value or get from combo box???????

	m_heightCtrl.GetWindowText(dlgStrValue);
	values.SetHeight(atof(dlgStrValue));

	m_widthCtrl.GetWindowText(dlgStrValue);
	values.SetWidth(atof(dlgStrValue));

	m_obliqueAngleCtrl.GetWindowText(dlgStrValue);
	values.SetObliqueAngle((IC_PI/180) * atof(dlgStrValue));	// always store as radians

	values.SetBackwards(m_backwardsCtrl.GetCheck());
	values.SetUpsideDown(m_upsideDownCtrl.GetCheck());
	values.SetVertically(m_verticallyCtrl.GetCheck());

	DWORD fontType = m_fontNameCtrl.GetFontType(m_fontNameCtrl.GetCurSel());
	ASSERT(CB_ERR != fontType);
	values.SetHasTrueTypeFont(false);
	if ((CB_ERR != fontType) && (SHX_FONTTYPE != fontType) && (SHP_FONTTYPE != fontType))
		values.SetHasTrueTypeFont(true);

	m_fontNameCtrl.GetLBText(m_fontNameCtrl.GetCurSel(), dlgStrValue);
	if (values.GetHasTrueTypeFont()){
		values.SetFontFamily(dlgStrValue);

		char filename[IC_ACADBUF];
		long styleNumber;
		StyleItem *styleItem = (StyleItem *) m_fontStyleCtrl.GetItemDataPtr(m_fontStyleCtrl.GetCurSel());	// get the style data from the style combo box
		int charSet = (int) m_fontLangCtrl.GetItemData(m_fontLangCtrl.GetCurSel());	// get the style data from the style combo box
		styleNumber = CTTFInfoGetter::styleNumByFontParams(styleItem->weight, styleItem->italic, styleItem->pitchAndFamily,	charSet);
		// EBATECH(CNBR) 2001/8/15 -[ Fix:when getFontFileName is error
		//CTTFInfoGetter::get().getFontFileName(filename, styleItem->fullName, styleNumber);
		if(!CTTFInfoGetter::get().getFontFileName(filename, styleItem->fullName, styleNumber))
		{
			char localName[IC_ACADBUF], globalName[IC_ACADBUF];
			strncpy(localName, styleItem->fullName,IC_ACADBUF-1);
			if( CTTFInfoGetter::get().getGlobalStyleByLocalName( localName, styleNumber, globalName )){
				CTTFInfoGetter::makeFontFileName(globalName, filename);
			}else{
				strcpy(filename, "Arial.ttf"/*DNT*/);
				styleNumber = ACAD_DEFAULTSTYLENUMBER;
			}
		}
		// EBATECH(CNBR) ]-
		if (styleItem)
			values.SetFontStyle(styleNumber);
		else	// error, so styleNumber is unreliable.
			values.SetFontStyle(m_tableInfo.GetFontStyle());
		values.SetFontFilename(filename);
		values.SetBigFontFilename(""/*DNT*/);
	}else{
		values.SetFontFilename(dlgStrValue);
		m_bigfontNameCtrl.GetLBText(m_bigfontNameCtrl.GetCurSel(), dlgStrValue);
		if(dlgStrValue.Compare(ResourceString(IDC_EXPSTYLE_NO_BIGFONT, "(none)")) == 0){
			dlgStrValue = "" /*DNT*/;
		}
		values.SetBigFontFilename(dlgStrValue);
	}
}

/******************************************************************************
*+ CTextStyleDlg::RestoreTextStyleValues - stores the values in the Style table
*
*  Updates the values that have changed for the current text style.
*
*  If an error occurs during the process, the values that have changed up to that
*  point are modified but the remaining values are left unchanged.
*
*  NOTE:  This function was built from CExplorerPropsTab::RestoreTextStyleValues.
*		  When that function is redone (see note there), the code here and the
*		  code there should be able to be shared to avoid redundancy.
*
*/

void CTextStyleDlg::RestoreTextStyleValues(
	CTextStyleTableInfo& originalValues,
	CTextStyleTableInfo& newValues)
	{
#ifndef NEW_STYLE_PROCESS	// EBATECH(CNBR) 2001.06.15 -[ Here is new update code
	// (1) remove drive and path if they doesn't need.
	bool fontPath, bigfontPath;
	char foundpn[IC_ACADBUF];

	fontPath = bigfontPath = false;
	if( newValues.GetHasTrueTypeFont() == false ){
		if( sds_findfile( (LPCTSTR)newValues.GetFontFilename(false), foundpn) != SDS_RTNORM ){
			fontPath = true;
		}
		if( sds_findfile( (LPCTSTR)newValues.GetBigFontFilename(false), foundpn) != SDS_RTNORM ){
			bigfontPath = true;
		}
	}
	// (2) set flags from original tabled.
	resbuf *orgRb, *rb;
	int flag70, flag71;
	orgRb = SDS_tblgetter("STYLE"/*DNT*/,
			originalValues.GetStyleName(),
			TRUE,
			originalValues.GetDrawing());
	ASSERT(orgRb);
	flag70 = (( rb = ic_ret_assoc(orgRb, 70)) != NULL ? rb->resval.rint : 0 );
	flag70 = (newValues.GetVertically() ? flag70 | 0x04 : flag70 & 0xFB );
	flag71 = (( rb = ic_ret_assoc(orgRb, 71)) != NULL ? rb->resval.rint : 0 );
	flag71 = (newValues.GetBackwards() ? flag71 | 0x02 : flag71 & 0xFD );
	flag71 = (newValues.GetUpsideDown() ? flag71 | 0x04 : flag71 & 0xFB );
	IC_RELRB(orgRb);
	// (3) create result buffer and update.
	resbuf *tmpRb;
	tmpRb = sds_buildlist(
		RTDXF0, "STYLE"/*DNT*/,
		 2, (LPCTSTR)newValues.GetStyleName(),
		70, flag70,
		40, newValues.GetHeight(),
		41, newValues.GetWidth(),
		50, newValues.GetObliqueAngle(),
		71, flag71,
		 3, (LPCTSTR)newValues.GetFontFilename(fontPath),
		 4, (LPCTSTR)newValues.GetBigFontFilename(bigfontPath),
		 NULL );
	ASSERT(tmpRb);

	if(SDS_tblmesser(tmpRb, IC_TBLMESSER_MODIFY, newValues.GetDrawing()) == SDS_RTNORM)
	{
		db_styletabrec*	pStyle = (db_styletabrec*)(newValues.GetDrawing()->findtabrec(DB_STYLETAB, (LPCTSTR)newValues.GetStyleName(), 1));
		if(pStyle)
		{
			/*DG - 10.1.2002*/// Save for undo the following settings too (along with those saved by SDS_tblmesser)
			CUndoSaver	undoSaver(pStyle, newValues.GetDrawing());

			if(newValues.GetHasTrueTypeFont())
			{
				pStyle->setFontFamily((LPCTSTR)newValues.GetFontFamily(), newValues.GetDrawing());
				pStyle->setFontStyleNumber((LPCTSTR)newValues.GetFontFamily(), newValues.GetFontStyle(), newValues.GetDrawing());
			}
			else
				pStyle->ensureNoTrueTypeEED(newValues.GetDrawing());

			undoSaver.saveToUndo();		/*DG - 10.1.2002*/
		}
	}

	IC_RELRB(tmpRb);
	originalValues = newValues;
#else						// EBATECH(CNBR) Below is deleted code
	int		toggleVal;
	resbuf	*rb,
			*flagrb = NULL,
			*tmpModTableInfo = NULL,
			*modTableInfo = NULL,
			*curTableInfo = NULL,
			*tmpModEEDInfo = NULL,
			*modEEDInfo = NULL;

	// get flags for later; compiler complains if these follow goto modifyTable
	bool newHasTrueTypeFont = newValues.GetHasTrueTypeFont();
	bool originalHasTrueTypeFont = originalValues.GetHasTrueTypeFont();

	// start the resbuf list for updating the table record
	if (!(tmpModTableInfo = BuildDataList(&modTableInfo, RTDXF0, "STYLE"/*DNT*/)) ||
		!BuildDataList(&tmpModTableInfo, 2, newValues.GetStyleName()))
			goto modifyTable;

	// update the table record for the values that have changed
	if (newValues.GetHeight() != originalValues.GetHeight())
		{
		if (!BuildDataList(&tmpModTableInfo, 40, newValues.GetHeight()))
			goto modifyTable;
		}

	if (newValues.GetWidth() != originalValues.GetWidth())
		{
		if (!BuildDataList(&tmpModTableInfo, 41, newValues.GetWidth()))
			goto modifyTable;
		}

	if (newValues.GetObliqueAngle() != originalValues.GetObliqueAngle())
		{
		if (!BuildDataList(&tmpModTableInfo, 50, newValues.GetObliqueAngle()))	// to do - make sure this is stored/retrieved in radians - (IC_PI/180) * newValues.GetObliqueAngle()
			goto modifyTable;
		}

// to do - both the font name and font style need to be handled better - when we had a TTF and now don't or vice versa or we just have a different font of the same type
//		   also have to handle the *varies* situation
//		   also have to get the TTF filename if we have a TTF to save in DXF 3;
//		   what we'd have in m_newFontName would be the family name that goes in EED

// to do - somewhere along the line, the font attached to the style has to be updated with the proper info

	if ((newHasTrueTypeFont && (newValues.GetFontFamily() != originalValues.GetFontFamily())) ||
		(!newHasTrueTypeFont && (newValues.GetFontFilename(true) != originalValues.GetFontFilename(true))))
		{
		// update the font filename - do BEFORE handling EED
		if (!BuildDataList(&tmpModTableInfo, 3, newValues.GetFontFilename(true)))
			goto modifyTable;

		// update the bigfont filename - do BEFORE handling EED
		if (newValues.GetBigFontFilename(true) != originalValues.GetBigFontFilename(true)){
			if (!BuildDataList(&tmpModTableInfo, 4, newValues.GetBigFontFilename(true)/*DNT*/))
				goto modifyTable;
		}

		// start the resbuf list for the EED
		// ALWAYS handle, whether for TTF or SHX/SHP
		if (!(tmpModEEDInfo = BuildDataList(&modEEDInfo, -3, NULL)) ||
			!BuildDataList(&tmpModEEDInfo, 1001, "ACAD"/*DNT*/))
			goto modifyTable;

		// if the new font is not a TrueType font, the EED specification is done -
		// the current ACAD EED will be deleted; if the new font is a TrueType font,
		// we'll update all of the information - family name, weight, italic, and
		// pitch and family
		if (newHasTrueTypeFont)
			{
			if (!BuildDataList(&tmpModEEDInfo, 1000, newValues.GetFontFamily()))
				goto modifyTable;

			if (!BuildDataList(&tmpModEEDInfo, 1071, newValues.GetFontStyle()))
				goto modifyTable;
			}
		}

	if ((newHasTrueTypeFont && !originalHasTrueTypeFont) ||		// new style has TTF, original doesn't
		(newHasTrueTypeFont &&									// both have TTF, but values differ
		 originalHasTrueTypeFont &&
		(newValues.GetFontStyle() != originalValues.GetFontStyle())))
		{
		// if the EED information was handled by the font name, it's all done;
		// otherwise, we need to handle it here if the style changed
		if (NULL == modEEDInfo)
			{
//to do - here and Explorer - make sure the filename gets handled properly if it's not a true type font
			if (!BuildDataList(&tmpModTableInfo, 3, newValues.GetFontFilename(true)))
				goto modifyTable;

			// update the bigfont filename - do BEFORE handling EED
			if (newValues.GetBigFontFilename(true) != originalValues.GetBigFontFilename(true)){
				if (!BuildDataList(&tmpModTableInfo, 4, newValues.GetBigFontFilename(true)/*DNT*/))
					goto modifyTable;
			}


			if (!(tmpModEEDInfo = BuildDataList(&modEEDInfo, -3, NULL)) ||
				!BuildDataList(&tmpModEEDInfo, 1001, "ACAD"/*DNT*/))
				goto modifyTable;

			if (!BuildDataList(&tmpModEEDInfo, 1000, newValues.GetFontFamily()))
				goto modifyTable;

			if (!BuildDataList(&tmpModEEDInfo, 1071, newValues.GetFontStyle()))
				goto modifyTable;
			}
		}

	// need the current style's table info for properly setting the toggles
	curTableInfo = SDS_tblgetter("STYLE"/*DNT*/,
								 originalValues.GetStyleName(),
								 TRUE,
								 originalValues.GetDrawing());
	ASSERT(curTableInfo);

	rb = ic_ret_assoc(curTableInfo, 71);
	if (newValues.GetBackwards() != originalValues.GetBackwards())
		{
		toggleVal = newValues.GetBackwards() ? rb->resval.rint | 0x02 : rb->resval.rint & 0xFD;
		if (!(flagrb = BuildDataList(&tmpModTableInfo, 71, toggleVal)))
			goto modifyTable;
		}
	else
		flagrb = NULL;

	if (newValues.GetUpsideDown() != originalValues.GetUpsideDown())
		{
		if (flagrb)
			{
			flagrb->resval.rint = newValues.GetUpsideDown() ? flagrb->resval.rint | 0x04 :
															  flagrb->resval.rint ^ 0x04;
			}
		else
			{
			toggleVal = newValues.GetUpsideDown() ? rb->resval.rint | 0x04 :
													rb->resval.rint ^ 0xFD;

			if (!BuildDataList(&tmpModTableInfo, 71, toggleVal))
				goto modifyTable;
			}
		}

	rb = ic_ret_assoc(curTableInfo, 70);
	if (newValues.GetVertically() != originalValues.GetVertically())
		{
		toggleVal = newValues.GetVertically() ? rb->resval.rint | 0x04 :
												rb->resval.rint ^ 0x04;

		if (!BuildDataList(&tmpModTableInfo, 70, toggleVal))
			goto modifyTable;
		}


modifyTable:

//	if (IEXP_RegenOnUndo == FALSE)			// to do - deal with UNDO here???????
//		{
//		SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO, NULL, NULL, NULL, SDS_CURDWG);
//		IEXP_RegenOnUndo = TRUE;
//		}

//	m_CdetailView->m_regenstate = TRUE;

	if (modTableInfo)
		{
		// add on the EED if this is a TrueType font
		if (modEEDInfo)
			appendRbLists(&modTableInfo, &modEEDInfo);

		SDS_tblmesser(modTableInfo, IC_TBLMESSER_MODIFY, newValues.GetDrawing());
		IC_RELRB(modTableInfo);
		}
	else
		{
		if (modEEDInfo)
			IC_RELRB(modEEDInfo);
		}

	if (curTableInfo)
		IC_RELRB(curTableInfo);

	originalValues = newValues;
#endif
	}

/******************************************************************************
*+ CTextStyleDlg::CreateNewTextStyle - creates a new text style
*
*/

void CTextStyleDlg::CreateNewTextStyle(
	CTextStyleTableInfo& values)				// i - the style's entity info
	{
#ifndef NEW_STYLE_PROCESS	// EBATECH(CNBR) 2001.06.15 -[ Here is new create code
	// (1) remove drive and path if they doesn't need.
	bool fontPath, bigfontPath;
	char foundpn[IC_ACADBUF];

	fontPath = bigfontPath = false;
	if( values.GetHasTrueTypeFont() == false ){
		if( sds_findfile( (LPCTSTR)values.GetFontFilename(false), foundpn) != SDS_RTNORM ){
			fontPath = true;
		}
		if( sds_findfile( (LPCTSTR)values.GetBigFontFilename(false), foundpn) != SDS_RTNORM ){
			bigfontPath = true;
		}
	}
	// (2) set flags from original tabled.
	int flag70, flag71;
	flag70 = 0;
	flag70 = (values.GetVertically() ? flag70 | 0x04 : flag70 & 0xFB );
	flag71 = 0;
	flag71 = (values.GetBackwards() ? flag71 | 0x02 : flag71 & 0xFD );
	flag71 = (values.GetUpsideDown() ? flag71 | 0x04 : flag71 & 0xFB );
	// (3) create result buffer and update.
	resbuf *tmpRb;
	tmpRb = sds_buildlist(
		RTDXF0, "STYLE"/*DNT*/,
		 2, (LPCTSTR)values.GetStyleName(),
		70, flag70,
		40, values.GetHeight(),
		41, values.GetWidth(),
		50, values.GetObliqueAngle(),
		71, flag71,
		 3, (LPCTSTR)values.GetFontFilename(fontPath),
		 4, (LPCTSTR)values.GetBigFontFilename(bigfontPath),
		 NULL );
	ASSERT(tmpRb);

	if( SDS_tblmesser(tmpRb, IC_TBLMESSER_MAKE, values.GetDrawing()) == SDS_RTNORM ){
		db_styletabrec *pStyle;
		pStyle = (db_styletabrec*)(values.GetDrawing()->findtabrec( DB_STYLETAB, (LPCTSTR)values.GetStyleName(), 1));
		if( pStyle ){
			if( values.GetHasTrueTypeFont()){
				pStyle->setFontFamily((LPCTSTR)values.GetFontFamily(), values.GetDrawing());
				pStyle->setFontStyleNumber((LPCTSTR)values.GetFontFamily(), values.GetFontStyle(), values.GetDrawing());
			}else{
				pStyle->ensureNoTrueTypeEED(values.GetDrawing());
			}
		}
	}
	IC_RELRB(tmpRb);
#else						// EBATECH(CNBR) Below is deleted code
	int		toggleVal;
	resbuf	*flagrb = NULL,
			*tmpModTableInfo = NULL,
			*modTableInfo = NULL,
			*tmpModEEDInfo = NULL,
			*modEEDInfo = NULL;

	// get flags for later; compiler complains if these follow goto modifyTable
	bool hasTrueTypeFont = values.GetHasTrueTypeFont();

	// start the resbuf list for creating the table record
	if (!(tmpModTableInfo = BuildDataList(&modTableInfo, RTDXF0, "STYLE"/*DNT*/)) ||
		!BuildDataList(&tmpModTableInfo, 2, values.GetStyleName()))
			goto modifyTable;

	// add the values for the table record
	if (!BuildDataList(&tmpModTableInfo, 40, values.GetHeight()))
		goto modifyTable;

	if (!BuildDataList(&tmpModTableInfo, 41, values.GetWidth()))
		goto modifyTable;

	if (!BuildDataList(&tmpModTableInfo, 50, values.GetObliqueAngle()))	// to do - make sure angle is stored/retrieved in radians - (IC_PI/180) * values.GetObliqueAngle()
		goto modifyTable;

// to do - both the font name and font style need to be handled better - when we had a TTF and now don't or vice versa or we just have a different font of the same type
//		   also have to handle the *varies* situation
//		   also have to get the TTF filename if we have a TTF to save in DXF 3;
//		   what we'd have in m_newFontName would be the family name that goes in EED

// to do - somewhere along the line, the font attached to the style has to be updated with the proper info

	if (hasTrueTypeFont)
		{
		// handle the font filename - do BEFORE handling EED
		if (!BuildDataList(&tmpModTableInfo, 3, values.GetFontFilename(true)))
			goto modifyTable;

		// handle the bigfont filename - do BEFORE handling EED
		if (!BuildDataList(&tmpModTableInfo, 4, values.GetBigFontFilename(true)))
			goto modifyTable;

		// start the resbuf list for the EED
		if (!(tmpModEEDInfo = BuildDataList(&modEEDInfo, -3, NULL)) ||
			!BuildDataList(&tmpModEEDInfo, 1001, "ACAD"/*DNT*/))
			goto modifyTable;

		// store the font family name, weight, italic, and pitch and family
		if (!BuildDataList(&tmpModEEDInfo, 1000, values.GetFontFamily()))
			goto modifyTable;

		if (!BuildDataList(&tmpModEEDInfo, 1071, values.GetFontStyle()))
			goto modifyTable;
		}
	else
		{
		if (!BuildDataList(&tmpModTableInfo, 3, values.GetFontFilename(true)))
			goto modifyTable;
		// handle the bigfont filename - do BEFORE handling EED
		if (!BuildDataList(&tmpModTableInfo, 4, values.GetBigFontFilename(true)))
			goto modifyTable;

		}

	toggleVal = values.GetBackwards() ? 0x02 : 0x00;
	if (!(flagrb = BuildDataList(&tmpModTableInfo, 71, toggleVal)))
		goto modifyTable;

	if (flagrb)
		{
		if (values.GetUpsideDown())
			flagrb->resval.rint += 0x04;
		}
	else
		{
		toggleVal = values.GetUpsideDown() ? 0x04 : 0x00;
		if (!BuildDataList(&tmpModTableInfo, 71, toggleVal))
			goto modifyTable;
		}

	toggleVal = values.GetVertically() ? 0x04 : 0x00;
	if (!BuildDataList(&tmpModTableInfo, 70, toggleVal))
		goto modifyTable;


modifyTable:

//	if (IEXP_RegenOnUndo == FALSE)			// to do - deal with UNDO here???????
//		{
//		SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO, NULL, NULL, NULL, SDS_CURDWG);
//		IEXP_RegenOnUndo = TRUE;
//		}

//	m_CdetailView->m_regenstate = TRUE;

	if (modTableInfo)
		{
		// add on the EED if this is a TrueType font
		if (modEEDInfo)
			appendRbLists(&modTableInfo, &modEEDInfo);

		SDS_tblmesser(modTableInfo, IC_TBLMESSER_MAKE, values.GetDrawing());
		IC_RELRB(modTableInfo);
		}
	else
		{
		if (modEEDInfo)
			IC_RELRB(modEEDInfo);
		}
#endif
	}

/******************************************************************************
*+ CTextStyleDlg::StyleOKToDelete - determines if a text style is OK to delete
*
*  The STANDARD text style and styles within XREFs cannot be deleted.
*
*/

bool CTextStyleDlg::StyleOKToDelete(
	const CString& styleName)					// i - name of the style to delete
	{
	char messageBuf[IC_ACADBUF];
	if (IEXP_STANDARD == styleName)				// can't delete STANDARD
		{
		sprintf(messageBuf, ResourceString(IDC_ICADEXPLORER_CANNOT_DEL_60, "Cannot delete text style %s."), styleName);
		AfxMessageBox(messageBuf);
		return false;
		}
	else if (strchr(styleName, IEXP_VER_BAR))	// can't delete XREF styles
		{
		AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_DEL_61, "Cannot delete an XREF text style."));
		return false;
		}

	return true;
	}

/******************************************************************************
*+ CTextStyleDlg::StyleOKToRename - determines if a text style is OK to rename
*
*  The STANDARD text style and styles within XREFs cannot be renamed.
*
*/

bool CTextStyleDlg::StyleOKToRename(
	const CString& styleName)					// i - name of the style to rename
	{
	if (IEXP_STANDARD == styleName)				// can't rename STANDARD
		{
		AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_REN_31, "Cannot rename Text style STANDARD. Specify a different Text style."));
		return false;
		}
	else if (strchr(styleName, IEXP_VER_BAR))	// can't rename XREF styles
		{
		AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_REN_33, "Cannot rename an XREF text style."));
		return false;
		}

	return true;
	}

/******************************************************************************
*+ CTextStyleDlg::GetTextStyleDeletePlan - finds out how the user wants to continue
*
*  Displays a dialog that lets the user indicate whether they want to delete
*  all of the entities using the specified text style, change entities using the
*  specified text style to use another text style, or cancel the deletion process.
*
*  Returns EXP_DEL_CONTINUE if the user wants to delete the entities using the
*		specified text style.
*
*  Returns EXP_DEL_CHANGE if the user wants to change the entities using the
*		specified text style to use another text style.
*
*  Returns EXP_DEL_CANCEL if the user want to cancel the deletion process.
*
*  WARNING - newTextStyle will contain a valid stylename only if the function
*			 returns EXP_DEL_CHANGE.
*
*/

int CTextStyleDlg::GetTextStyleDeletePlan(
	const CString& styleToDelete,				// i - the style to be deleted
	CString& newTextStyle)						// o - the new style to use for EXP_DEL_CHANGE
	{
	CTextStyle_IEXP_MessItem messageDlg;

	// set up the messages and flags to be used in the message dialog
	messageDlg.m_Continue = ResourceString(IDC_ICADEXPLORER_CHANGE____34, "Change...");
	messageDlg.m_typeflag = 0;
	messageDlg.m_txtStyleNewDlg.m_type = ResourceString(IDC_ICADEXPLORER_TEXT_STYLE_62, "Text style");
	messageDlg.m_txtStyleNewDlg.m_String = styleToDelete;

	char messageBuf[IC_ACADBUF];
	sprintf(messageBuf, ResourceString(IDC_ICADEXPLORER_YOU_ARE_AB_64, "You are about to delete the text style %s. This will affect entities currently using this text style."),
		styleToDelete);
	messageDlg.m_txtmes = messageBuf;

	sprintf(messageBuf, ResourceString(IDC_ICADEXPLORER_DELETES_AL_65, "Deletes all entities currently using text style %s."),
		styleToDelete);
	messageDlg.m_txtcnt = messageBuf;

	sprintf(messageBuf, ResourceString(IDC_ICADEXPLORER_GIVES_YOU__66, "Gives you a choice of text styles to assign to the entities now using %s, then deletes the old text style."),
		styleToDelete);
	messageDlg.m_txtchng = messageBuf;

	sprintf(messageBuf, ResourceString(IDC_ICADEXPLORER_CANCELS_DE_67, "Cancels deletion of the text style %s."),
		styleToDelete);
	messageDlg.m_txtcancel = messageBuf;
	//Bugzilla No. 78162 ; 20-05-2002 
	sprintf(messageBuf,ResourceString(IDC_ICADEXPLORER_DELETE__S_45, "Delete %s" ),styleToDelete);
	messageDlg.m_title=messageBuf;

	// run the dialog
	int retCode = messageDlg.DoModal();

	// get the new text style to use for EXP_DEL_CHANGE if the user wants to change styles
	// if an error occurs, m_String may be styleToDelete; this is identical to the behavior
	// in IntelliCAD Explorer
	newTextStyle = (EXP_DEL_CHANGE == retCode) ? messageDlg.m_txtStyleNewDlg.m_String : ""/*DNT*/;

	return retCode;
	}

/******************************************************************************
*+ CTextStyleDlg::DeleteTextStyle - deletes a text style from a drawing
*
*  NOTE:  Returns TEXT_STYLE_DELETE_SUCCEEDED on successful deletion of the style.
*
*		  Returns TEXT_STYLE_DELETE_FAILED_OK on a failure where things are still
*			   in a stable state - for example, we might have failed to get a
*			   necessary value before we did anything major.
*
*		  Returns TEXT_STYLE_DELETE_FAILED_BAD on a failure where things are in
*			   an unstable state - for example, we might have deleted the style
*			   from the table but then couldn't remove it from all of the entities.
*
*  NOTE:  This function was adapted from IEXP_CdetailView::RemoveItem(), which
*			   provides the same functionality for IntelliCAD Explorer.  Because of
*			   this, this function tries to maintain the same structure and flow
*			   as that function.  Both functions could be made more efficient
*			   (such as, there is no reason to change an entity's text style if
*			   the entity is going to be deleted anyway).  Since I didn't have
*			   time to modify RemoveItem(), I left this function mimicing that
*			   function.  This way, if someone has to modify one of the functions,
*			   the other function will look similar and should be easier to fix
*			   than if both functions looked very different.
*
*/

int CTextStyleDlg::DeleteTextStyle(
	db_drawing *drawing,
	const int mode,
	CString& oldStyle,
	CString& newStyle)
	{

	ASSERT(NULL != drawing);

	// delete the style from the table
	resbuf *styleRb = sds_buildlist(RTDXF0, IEXP_STYLE, 2, oldStyle, 0);
	if (NULL == styleRb)
		return TEXT_STYLE_DELETE_FAILED_OK;

	if (SDS_tblmesser(styleRb, IC_TBLMESSER_DELETE, drawing) != RTNORM)
		{
		sds_relrb(styleRb);
		return TEXT_STYLE_DELETE_FAILED_BAD;
		}

	sds_relrb(styleRb);

	// if the text style being deleted is the current dim text style,
	// change the current dim text style to STANDARD
	resbuf dimTxStyRb;
	sds_getvar("DIMTXSTY"/*DNT*/, &dimTxStyRb);

	if (strisame(dimTxStyRb.resval.rstring, oldStyle))
		{
		IC_FREE(dimTxStyRb.resval.rstring);
		dimTxStyRb.resval.rstring = (char *) IEXP_STANDARD;
		dimTxStyRb.restype = RTSTR;
		sds_setvar("DIMTXSTY"/*DNT*/, &dimTxStyRb);
		}

	// if the text style being deleted is used in a block,
	// change the style of that text to STANDARD
	resbuf *tempRb;
	sds_name ename;

	// update any dimension styles that use the deleted text style;
	// make those dimension styles use the STANDARD text style
	resbuf *standardRb = sds_tblsearch(IEXP_STYLE, IEXP_STANDARD, 0);
	if (NULL == standardRb)
		return TEXT_STYLE_DELETE_FAILED_BAD;

	resbuf *enameRb = ic_ret_assoc(standardRb, -1);
	if (NULL == enameRb)
		{
		IC_RELRB(standardRb);
		return TEXT_STYLE_DELETE_FAILED_BAD;
		}

	ic_encpy(ename, enameRb->resval.rlname);
	IC_RELRB(standardRb);

	resbuf *dimstyleRb = SDS_tblgetter(IEXP_DIMSTY, NULL, TRUE, drawing);
	while (dimstyleRb)
		{
		// get the DIMTXSTY resbuf - DXF code 340
		for (tempRb = dimstyleRb; tempRb && (tempRb->restype != 340); tempRb = tempRb->rbnext)
			;

		if (tempRb)
			{
			char *string;
			db_handitem *dimTextStyle = (db_handitem *)((long) tempRb->resval.rlname[0]);
			dimTextStyle->get_2(&string);
			if (strisame(oldStyle, string))
				{
				ic_encpy(tempRb->resval.rlname, ename);
				sds_entmod(dimstyleRb);
				}
			}

		IC_RELRB(dimstyleRb);
		dimstyleRb = SDS_tblgetter(IEXP_DIMSTY, NULL, FALSE, drawing);
		}

	// walk through all of the entities in the drawing and update the
	// old text style to the new text style if appropriate; also, if the
	// user wants all of the entities that use the old text style to be
	// deleted, do so
	char styleName[IC_ACADBUF];
	ename[1] = (long) drawing;
	db_handitem *entity = NULL;
	resbuf *entityRb = NULL;

	while (entity = drawing->entnext_noxref(entity))
		{
		entity->get_7(styleName, IC_ACADBUF - 1);		// get the text style
		if (!strisame(oldStyle, styleName))				// non-text entities will cause us to reloop
			continue;

		ename[0] = (long) entity;
		entityRb = sds_entget(ename);
		for (tempRb = entityRb; tempRb && (tempRb->restype != 7); tempRb = tempRb->rbnext)
			;

		if (tempRb)
			{
			tempRb->resval.rstring = ic_realloc_char(tempRb->resval.rstring, newStyle.GetLength() + 1);
			strcpy(tempRb->resval.rstring, newStyle);
			}

		if (EXP_DEL_CONTINUE == mode)
			sds_entdel(ename);
		else if (EXP_DEL_CHANGE == mode)
			sds_entmod(entityRb);

		IC_RELRB(entityRb);
		}

	// update the old text style to the new text style
	// in any entities nested in blocks
	SDS_DontUpdateDisp = TRUE;
	CleanupEntsInsideBlocks(IEXP_TSTAB_IDX, oldStyle, mode, newStyle, drawing);
	SDS_DontUpdateDisp = FALSE;

	return TEXT_STYLE_DELETE_SUCCEEDED;
	}




/////////////////////////////////////////////////////////////////////////////
// CTextStyleDlg message handlers
/* EBATECH(CNBR) 2001-06-14 Add OnBigfontNameChanged and OnFontLangChanged */

BEGIN_MESSAGE_MAP(CTextStyleDlg, CDialog)
	ON_MESSAGE(CIcadFontComboBox::WM_FONT_NAME_CHANGED, OnFontNameChanged)
	ON_MESSAGE(CIcadFontStyleComboBox::WM_FONT_STYLE_CHANGED, OnFontStyleChanged)
	ON_MESSAGE(CIcadFontComboBox::WM_BIGFONT_NAME_CHANGED, OnBigfontNameChanged)
	ON_MESSAGE(CIcadFontLangComboBox::WM_FONT_LANG_CHANGED, OnFontLangChanged)

	//{{AFX_MSG_MAP(CTextStyleDlg)
	ON_BN_CLICKED(TEXT_STYLE_NEW, OnStyleNew)
	ON_BN_CLICKED(TEXT_STYLE_RENAME, OnStyleRename)
	ON_BN_CLICKED(TEXT_STYLE_DELETE, OnStyleDelete)
	ON_BN_CLICKED(IDAPPLY, OnApply)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_CBN_SELCHANGE(EXP_STYLE_NAME, OnSelChangeStyleName)
	ON_EN_CHANGE(EXP_STYLE_HEIGHT, OnChangeStyleHeight)
	ON_EN_CHANGE(EXP_STYLE_WIDTH, OnChangeStyleWidth)
	ON_EN_CHANGE(EXP_STYLE_OBLIQUE, OnChangeStyleOblique)
	ON_EN_KILLFOCUS(EXP_STYLE_HEIGHT, OnKillFocusStyleHeight)
	ON_EN_KILLFOCUS(EXP_STYLE_WIDTH, OnKillFocusStyleWidth)
	ON_EN_KILLFOCUS(EXP_STYLE_OBLIQUE, OnKillFocusStyleOblique)
	ON_BN_CLICKED(EXP_STYLE_BACKWARDS, OnStyleBackwards)
	ON_BN_CLICKED(EXP_STYLE_UPSIDE_DOWN, OnStyleUpsideDown)
	ON_BN_CLICKED(EXP_STYLE_VERTICALLY, OnStyleVertically)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/******************************************************************************
*+ CTextStyleDlg::OnFontNameChanged
*
*  to do - When IntelliCAD Explorer is redone, this should be able to be merged
*		   with the same function in explorerpropstab.cpp into a base class
*		   from which the IntelliCAD Explorer Styles Properties tab and the
*		   Style command's dialog can be derived.
*/

LONG CTextStyleDlg::OnFontNameChanged(UINT wParam, LONG lParam)
	{
	// if we can't get the index of the current selection, something's wrong
	int index = m_fontNameCtrl.GetCurSel();
	if (CB_ERR == index)
		return 0L;

	// get the new font's type
	DWORD fontType = m_fontNameCtrl.GetFontType(index);
	if (CB_ERR == fontType)
		return 0L;

	// enable/disable the style combo box and the vertically check box
	if ((SHX_FONTTYPE == fontType) || (SHP_FONTTYPE == fontType))
		{
		m_verticallyCtrl.EnableWindow(TRUE);
		m_fontStyleCtrl.DeleteStyleItems();		// delete dynamic memory before resetting contents
		m_fontStyleCtrl.ResetContent();
		m_fontStyleCtrl.EnableWindow(FALSE);	// no style for shx/shp fonts
		// EBATECH(CNBR) -[ Add Contorol
		m_fontLangCtrl.ResetContent();
		m_fontLangCtrl.EnableWindow(FALSE);		// no style for shx/shp fonts
		m_bigfontNameCtrl.EnableWindow(TRUE);
		// EBATECH(CNBR) ]-
		}
	else	// TrueType font
		{
		m_verticallyCtrl.EnableWindow(FALSE);
		m_fontStyleCtrl.EnableWindow(TRUE);		// can't generate TrueType fonts vertically
		// EBATECH(CNBR) -[ Add Contorol
		m_fontLangCtrl.EnableWindow(TRUE);
		m_bigfontNameCtrl.EnableWindow(FALSE);
		// EBATECH(CNBR) ]-

		CString fontName;
		long stylenumber;
		stylenumber = m_tableInfo.GetFontStyle();
		m_charSet = ((stylenumber & 0x0000ff00)>>8);
		m_fontNameCtrl.GetLBText(m_fontNameCtrl.GetCurSel(), fontName);
		m_fontStyleCtrl.FillTrueTypeStyles(fontName, stylenumber);
		m_fontLangCtrl.FillTrueTypeLangs(fontName, stylenumber);	// EBATECH(CNBR)
		CString text;
		m_fontStyleCtrl.GetWindowText(text);
		if (m_fontStyleCtrl.GetCount() == 0 || text.IsEmpty())
			{
			StyleItem* styleItem = new StyleItem;
			m_fontStyleCtrl.ResetContent();
			if ((0x3000000 & m_tableInfo.GetFontStyle()) == 0x3000000) // Bold & italic
				{
				m_fontStyleCtrl.AddString("Bold Italic");
				styleItem->weight = FW_BOLD;
				styleItem->italic = true;
				m_weight = FW_BOLD;
				m_italic = true;
				}
			else if (0x2000000 & m_tableInfo.GetFontStyle())			// Bold
				{
				m_fontStyleCtrl.AddString("Bold");
				styleItem->weight = FW_BOLD;
				styleItem->italic = false;
				m_weight = FW_BOLD;
				m_italic = false;
				}
			else if (0x1000000 & m_tableInfo.GetFontStyle())			// Italic
				{
				m_fontStyleCtrl.AddString("Italic");
				styleItem->weight = FW_NORMAL;
				styleItem->italic = true;
				m_weight = FW_NORMAL;
				m_italic = true;
				}
			else
				{
				m_fontStyleCtrl.AddString("Regular");
				styleItem->weight = FW_NORMAL;
				styleItem->italic = false;
				m_weight = FW_NORMAL;
				m_italic = false;
				}
			m_fontStyleCtrl.SetItemDataPtr(0, styleItem);
			m_fontStyleCtrl.SetCurSel(0);
			DrawFontPreview();
			}
		}

	// update the dirty flag and Apply button for the font name
	CString dlgStrValue;
	m_fontNameCtrl.GetLBText(index, dlgStrValue);

	CString tmpStr = m_tableInfo.GetHasTrueTypeFont() ?
					 m_tableInfo.GetFontFamily() :
					 m_tableInfo.GetFontFilename(false);
	UpdateDirtyFlag(tmpStr, dlgStrValue, DIRTY_FONT_NAME);

	// if the current font is a shx/shp, we need to clear the dirty flag for the style;
	// otherwise, we need to set the dirty flag for the style
	if ((SHX_FONTTYPE == fontType) || (SHP_FONTTYPE == fontType))
		{
		UpdateDirtyFlag(0L, 0L, DIRTY_FONT_STYLE);
		SetApplyButton();
		DrawFontPreview();
		}
	else	// TrueType font
		{
		GetStyleSel();
		OnFontStyleChanged(0, 0L);				// will set Apply button and draw preview
		}

	return 1L;
	}

/******************************************************************************
*+ CTextStyleDlg::OnFontStyleChanged
*
*  to do - When IntelliCAD Explorer is redone, this should be able to be merged
*		   with the same function in explorerpropstab.cpp into a base class
*		   from which the IntelliCAD Explorer Styles Properties tab and the
*		   Style command's dialog can be derived.
*/

LONG CTextStyleDlg::OnFontStyleChanged(UINT wParam, LONG lParam)
	{
	// if we can't get the index of the current selection, something's wrong
	int index = m_fontStyleCtrl.GetCurSel();
	if (CB_ERR == index)
		return 0L;

	// rather than compare style names (e.g., Bold, Bold Italic),
	// we compare the style number of the form stored in EED
	long styleNumber;
	StyleItem *styleItem = (StyleItem *) m_fontStyleCtrl.GetItemDataPtr(index);	// get the style data from the style combo box
	if( (long)styleItem != 0 && (long)styleItem != CB_ERR )	// error otherwise
		{
		styleNumber = CTTFInfoGetter::styleNumByFontParams(styleItem->weight, styleItem->italic, styleItem->pitchAndFamily, m_charSet);

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
		UpdateDirtyFlag(m_tableInfo.GetFontStyle(), styleNumber, DIRTY_FONT_STYLE);
		}

	SetApplyButton();
	DrawFontPreview();

	return 1L;
	}

// EBATECH(CNBR) 2001-06-14 -[ Add BIGFONT and LANGAGE LISTBOX CONTROL
/******************************************************************************
*+ CTextStyleDlg::OnBigfontNameChanged
*
*  to do - When IntelliCAD Explorer is redone, this should be able to be merged
*		   with the same function in explorerpropstab.cpp into a base class
*		   from which the IntelliCAD Explorer Styles Properties tab and the
*		   Style command's dialog can be derived.
*/

LONG CTextStyleDlg::OnBigfontNameChanged(UINT wParam, LONG lParam)
	{
	// if we can't get the index of the current selection, something's wrong
	int index = m_bigfontNameCtrl.GetCurSel();
	if (CB_ERR == index)
		return 0L;

	CString bigfontName;
	m_bigfontNameCtrl.GetLBText(index, bigfontName);

	if( bigfontName.Compare( ResourceString(IDC_EXPSTYLE_NO_BIGFONT, "(none)")) == 0){
		bigfontName = "" /*DNT*/;
	}
	CString tmpStr = m_tableInfo.GetBigFontFilename(false);
	UpdateDirtyFlag(tmpStr, bigfontName, DIRTY_BIGFONT_NAME);
	SetApplyButton();

	return 1L;
	}

/******************************************************************************
*+ CTextStyleDlg::OnFontLangChanged
*
*  to do - When IntelliCAD Explorer is redone, this should be able to be merged
*		   with the same function in explorerpropstab.cpp into a base class
*		   from which the IntelliCAD Explorer Styles Properties tab and the
*		   Style command's dialog can be derived.
*/

LONG CTextStyleDlg::OnFontLangChanged(UINT wParam, LONG lParam)
	{
	// if we can't get the index of the current selection, something's wrong
	int index = m_fontLangCtrl.GetCurSel();
	if (CB_ERR == index)
		return 0L;

	if( (int)m_fontLangCtrl.GetItemDataPtr(index)  == CB_ERR)
		return 0L;
	m_charSet = (int)m_fontLangCtrl.GetItemDataPtr(index);

	// rather than compare style names (e.g., Bold, Bold Italic),
	// we compare the style number of the form stored in EED
	index = m_fontStyleCtrl.GetCurSel();
	if (CB_ERR == index)
		return 0L;

	index = (int)m_fontStyleCtrl.GetItemDataPtr(index);	// get the style data from the style combo box
	if( index == 0 || index == CB_ERR )	// error otherwise
		return 0L;

	StyleItem *styleItem = (StyleItem *)index;
	long styleNumber;

	styleNumber = CTTFInfoGetter::styleNumByFontParams(styleItem->weight, styleItem->italic, styleItem->pitchAndFamily, m_charSet);
	UpdateDirtyFlag(m_tableInfo.GetFontStyle(), styleNumber, DIRTY_FONT_LANGUAGE);

	SetApplyButton();
	DrawFontPreview();

	return 1L;
	}
// EBATECH(CNBR) ]-
/******************************************************************************
*+ CTextStyleDlg::OnInitDialog - initialize the values for the dialog
*
*/

BOOL CTextStyleDlg::OnInitDialog()
	{
	CDialog::OnInitDialog();

	// set the icon on the ?/Help button
	CButton *helpWnd = (CButton *) GetDlgItem(IDHELP);
	helpWnd->SetIcon(AfxGetApp()->LoadIcon(HELP_ICON));

	// fill the styles combo box
	resbuf rb;
	char defaultStyle[IC_ACADBUF] = { 0 };
	if (SDS_getvar(NULL, DB_QTEXTSTYLE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) == RTNORM)
		{
		strcpy(defaultStyle, rb.resval.rstring);
		IC_FREE(rb.resval.rstring);
		rb.resval.rstring = NULL;
		}

	// the current style combo box must be filled first, then the text font name combo box
	// must be filled, and then the current style's values can be displayed
	FillStylesComboBox(defaultStyle);
	// EBATECH(CNBR) 2001-05-14 -[ Separate Standard font and Bigfont Control
	//m_fontNameCtrl.FillFontComboBox(GET_FONT_TTF + GET_FONT_SHX + GET_FONT_SHP);
	m_fontNameCtrl.FillFontComboBox(GET_FONT_TTF + GET_FONT_SHX);
	m_bigfontNameCtrl.FillFontComboBox(GET_BIGFONT_SHX);
	// EBATECH(CNBR) 2001-05-14 ]-
	OnSelChangeStyleName();

	m_weight = FW_NORMAL;
	m_italic = false;
	if ((0x3000000 & m_tableInfo.GetFontStyle()) == 0x3000000) // Bold & italic
		{
		m_weight = FW_BOLD;
		m_italic = true;
		}
	else if (0x2000000 & m_tableInfo.GetFontStyle())			// Bold
		m_weight = FW_BOLD;
	else if (0x1000000 & m_tableInfo.GetFontStyle())			// Italic
		m_italic = true;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
	}

/******************************************************************************
*+ CTextStyleDlg::OnStyleNew - handler for when the New button is pressed
*
*/

void CTextStyleDlg::OnStyleNew(
	)
	{
	// get a new, unique name from the user
	CInputEditBoxDlg newNameDlg;
	newNameDlg.SetTitle(_T(ResourceString(IDC_TEXTSTYLE_NEW_TEXT_STYLE, "New Text Style")));
	newNameDlg.SetLabel(_T(ResourceString(IDC_TEXTSTYLE_STYLE_NAME, "Style name:")));

	CTableNameValidator validator;
	newNameDlg.SetValidator(&validator);

	int result = newNameDlg.DoModal();

	// if the name is good, create the style and add it to the list of styles
	if (IDOK == result)
		{
		CTextStyleTableInfo dlgValues;
		GetStyleValuesFromDialog(dlgValues);

		CString newName = newNameDlg.GetValue();
		newName.TrimLeft();
		newName.TrimRight(); 
		newName.MakeUpper();						// store name in uppercase like R14
		dlgValues.SetStyleName(newName);
		CreateNewTextStyle(dlgValues);

		// if the table entry really got made, add the style to the list
		// and redisplay its values from the table (getting the values from
		// the table will help ensure there were no problems creating the style)
		resbuf *newTableInfo = SDS_tblgetter("STYLE"/*DNT*/,
							   dlgValues.GetStyleName(),
							   TRUE,
							   dlgValues.GetDrawing());

		ASSERT(newTableInfo);
		if (newTableInfo)
			{
			IC_RELRB(newTableInfo);

			int comboIndex = m_styleNameCtrl.AddString(dlgValues.GetStyleName());
			if (comboIndex >= 0)
				m_styleNameCtrl.SetCurSel(comboIndex);
			else
				m_styleNameCtrl.SetCurSel(0);

			m_dirtyFlag = DIRTY_NONE;	// only set to DIRTY_NONE if we indeed did create the new style
			OnSelChangeStyleName();

			// mark the drawing as modified so that the changes are saved
			resbuf rb;
			SDS_getvar(NULL, DB_QDBMOD, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			rb.resval.rint |= IC_DBMOD_ENTITIES_MODIFIED;
			SDS_setvar(NULL, DB_QDBMOD, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
			}
		}
	}

/******************************************************************************
*+ CTextStyleDlg::OnStyleRename - handler for when the Rename button is pressed
*
*/

void CTextStyleDlg::OnStyleRename()
	{
	// get the style's original name so we know which style to change later
	CString originalStyleName;
	m_styleNameCtrl.GetLBText(m_styleNameCtrl.GetCurSel(), originalStyleName);

	// don't let the user rename STANDARD or styles from an XREF
	if (!StyleOKToRename(originalStyleName))
		return;

	// get a new, unique name from the user
	CInputEditBoxDlg newNameDlg;
	newNameDlg.SetTitle(_T(ResourceString(IDC_TEXTSTYLE_RENAME_TEXT_STYLE, "Rename Text Style")));
	newNameDlg.SetLabel(_T(ResourceString(IDC_TEXTSTYLE_STYLE_NAME, "Style name:")));

	newNameDlg.SetValue(originalStyleName);

	CTableNameValidator validator;
	newNameDlg.SetValidator(&validator);

	int result = newNameDlg.DoModal();

	// if the new name is good, rename the style in the drawing and in the list of styles
	if (IDOK == result)
		{
		// get the style table record under the original name and change the name
		//
		// NOTE!!!!!!!	Since we are dealing with the table record directly and not through
		//				resbufs, renaming a style is not undoable.	This was done here to
		//				be consistent with IntelliCAD Explorer.  Both should really use resbufs
		//				so that they are undoable, but there isn't time at this point.
		CString newStyleName = newNameDlg.GetValue();
		newStyleName.TrimLeft();
		newStyleName.TrimRight();		
		newStyleName.MakeUpper();				// store name in uppercase like R14
		db_handitem *dbhandItem = SDS_CURDWG->findtabrec(DB_STYLETAB, originalStyleName, 1);
		dbhandItem->set_2((char*)(LPCTSTR)newStyleName);	// place the new name in the actual style record
		m_tableInfo.SetStyleName(newStyleName);				// place the new name in the values stored for the dialog

		// change the name of the style in the combo box--have to delete old name
		// and add the new name
		int comboIndex = m_styleNameCtrl.FindStringExact(-1, originalStyleName);
		if (CB_ERR != comboIndex)
			m_styleNameCtrl.DeleteString(comboIndex);

		comboIndex = m_styleNameCtrl.AddString(newStyleName);
		if (comboIndex >= 0)
			m_styleNameCtrl.SetCurSel(comboIndex);
		else
			{
			m_styleNameCtrl.SetCurSel(0);		// on error, set to whatever is first in the style list
			m_dirtyFlag = DIRTY_NONE;			// clear any change flags
			OnSelChangeStyleName();				// display the first style's values
			}

		// mark the drawing as modified so that the changes are saved
		resbuf rb;
		SDS_getvar(NULL, DB_QDBMOD, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		rb.resval.rint |= IC_DBMOD_ENTITIES_MODIFIED;
		SDS_setvar(NULL, DB_QDBMOD, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
		}
	}

/******************************************************************************
*+ CTextStyleDlg::OnStyleDelete - handler for when the Delete button is pressed
*
*/

void CTextStyleDlg::OnStyleDelete()
	{
	// get the name of the style to delete
	CString oldTextStyle;
	m_styleNameCtrl.GetLBText(m_styleNameCtrl.GetCurSel(), oldTextStyle);

	// don't let the user delete STANDARD or styles from an XREF;
	if (!StyleOKToDelete(oldTextStyle))
		return;

	// find out whether the user wants to delete all entities having this style,
	// replace this style with another style, or cancel the delete
	CString newTextStyle = ""/*DNT*/;
	int planStatus = GetTextStyleDeletePlan(oldTextStyle, newTextStyle);

	if ((EXP_DEL_CONTINUE != planStatus) && (EXP_DEL_CHANGE != planStatus))
		return;

	// delete the style
	int deleteStatus = DeleteTextStyle(m_tableInfo.GetDrawing(),
									   planStatus,
									   oldTextStyle,
									   newTextStyle);

	// if the deletion failed but everything should still be stable,
	// return
	if (TEXT_STYLE_DELETE_FAILED_OK == deleteStatus)
		return;

	// if the current text style was deleted, set STANDARD
	// as the new current text style
	CString curTextStyle(IEXP_STANDARD);		// reset style combo box to STANDARD by default
	resbuf rb;
	if (sds_getvar("TEXTSTYLE"/*DNT*/, &rb) == RTNORM)
		{
		if (strisame(oldTextStyle, rb.resval.rstring))
			{
			IC_FREE(rb.resval.rstring);
			rb.resval.rstring = (char *) IEXP_STANDARD;
			rb.restype = RTSTR;
			sds_setvar("TEXTSTYLE"/*DNT*/, &rb);
			}
		else
			{
			curTextStyle = rb.resval.rstring;	// reset style combo box to current text style
			IC_FREE(rb.resval.rstring);
			}
		}

	// if the current dimension text style was deleted, set STANDARD
	// as the new current dimension text style
	if (sds_getvar("DIMTXSTY"/*DNT*/, &rb) == RTNORM)
		{
		if (strisame(oldTextStyle, rb.resval.rstring))
			{
			IC_FREE(rb.resval.rstring);
			rb.resval.rstring = (char *) IEXP_STANDARD;
			rb.restype = RTSTR;
			sds_setvar("DIMTXSTY"/*DNT*/, &rb);
			}
		else
			IC_FREE(rb.resval.rstring);
		}

	// delete the deleted style's name from the style combo box
	m_styleNameCtrl.DeleteString(m_styleNameCtrl.GetCurSel());

	// select the current text style in the style combo box and display its values
	int comboIndex = m_styleNameCtrl.FindStringExact(-1, curTextStyle);
	if (CB_ERR != comboIndex)
		m_styleNameCtrl.SetCurSel(comboIndex);
	else
		m_styleNameCtrl.SetCurSel(0);

	m_dirtyFlag = DIRTY_NONE;
	OnSelChangeStyleName();

	// make sure any changes display on the graphics screen
	cmd_regenall();
	}

/******************************************************************************
*+ CTextStyleDlg::OnApply - handler for when the Apply button is pressed
*
*/

void CTextStyleDlg::OnApply()
	{
	m_dirtyFlag = DIRTY_NONE;
	SaveTextStyle();
	SetApplyButton();
	}

/******************************************************************************
*+ CTextStyleDlg::OnOK - handler for when the OK button is pressed
*
*/

void CTextStyleDlg::OnOK()
	{
	// if something has changed, prompt the user and perhaps save the changes
	if (m_dirtyFlag)
		{
		m_dirtyFlag = DIRTY_NONE;

		if (PromptToSaveStyle())
			SaveTextStyle();
		}

	// make the text style selected in the combo box the current text style
	// for the drawing
	CString curTextStyle;
	m_styleNameCtrl.GetLBText(m_styleNameCtrl.GetCurSel(), curTextStyle);

	resbuf rb;
	rb.resval.rstring = (char *) curTextStyle.GetBuffer(curTextStyle.GetLength());
	rb.restype = RTSTR;
	sds_setvar("TEXTSTYLE"/*DNT*/, &rb);

	CDialog::OnOK();
	}

/******************************************************************************
*+ CTextStyleDlg::OnHelp - handler for when the ?/Help button is pressed
*
*/

void CTextStyleDlg::OnHelp()
	{

	CommandRefHelp(m_hWnd, HLP_TEXTSTYLE);
	}

/******************************************************************************
*+ CTextStyleDlg::OnSelChangeStyleName - handler for when a different style is
*  selected in the Current Style Name combo box
*
*/

void CTextStyleDlg::OnSelChangeStyleName()
	{
	// if the current style has been modified, save it if appropriate
	if (m_dirtyFlag)
		{
		m_dirtyFlag = DIRTY_NONE;

		if (PromptToSaveStyle())
			SaveTextStyle();			// m_tableInfo.m_styleName safely contains the old style name at this point
		}

	// get the new style's record from the STYLE table
	CString styleName;
	m_styleNameCtrl.GetLBText(m_styleNameCtrl.GetCurSel(), styleName);
	db_styletabrec *styleTabRec = (db_styletabrec *) SDS_CURDWG->findtabrec(DB_STYLETAB,
																			styleName,
																			1);

	// get the style's values from the record and display them
	GetTextStyleValues(styleTabRec);	// m_tableInfo.m_styleName gets the new style name here
	AddFontName();
	AddBigFontName();
	DisplayTextStyleValues(styleName);
	m_weight = FW_NORMAL;
	m_italic = false;
	if ((0x3000000 & m_tableInfo.GetFontStyle()) == 0x3000000)	// Bold & italic
		{
		m_weight = FW_BOLD;
		m_italic = true;
		}
	else if (0x2000000 & m_tableInfo.GetFontStyle())			// Bold
		m_weight = FW_BOLD;
	else if (0x1000000 & m_tableInfo.GetFontStyle())			// Italic
		m_italic = true;
	DrawFontPreview();
	}

void CTextStyleDlg::AddFontName()
	{
	CString fontFamily = m_tableInfo.GetFontFamily();
	CString fontFilename = m_tableInfo.GetFontFilename(false);
	if (fontFilename.Right(4) == ".shx" || fontFilename.Right(4) == ".SHX" ||
		fontFilename.Right(4) == ".shp" || fontFilename.Right(4) == ".SHP")
		{
		if (m_fontNameCtrl.FindStringExact(-1, fontFilename) != -1){
			m_fontNameCtrl.SetCurrentFont(fontFilename);
			m_fontStyleCtrl.DeleteStyleItems();
			m_fontStyleCtrl.ResetContent();
			m_fontLangCtrl.ResetContent(); // EBATECH(CNBR)
		}else{

			// add the shx font to the list.
			m_fontNameCtrl.AddString(fontFilename);
			FontItem* fontItem = NULL;
			fontItem = new FontItem;

			// EBATECH(CNBR) -[ Delete below, It needs only TrueType font.
			//// get all of the fonts in all of the character sets
			//LOGFONT logFont;
			//logFont.lfCharSet = DEFAULT_CHARSET;
			//strcpy(logFont.lfFaceName, fontFilename);
			//logFont.lfPitchAndFamily = 0;
			//
			//// DP: TODO: REMOVE ENUMERATION
			//// enumerate the fonts
			//CClientDC dc(NULL);
			//::EnumFontFamiliesEx(dc.GetSafeHdc(),
			//					 &logFont,
			//					 (FONTENUMPROC)CIcadFontComboBox::EnumFontFamExProc,
			//					 (LPARAM) this,
			//					 0);
			fontItem->fontType = SHX_FONTTYPE;
			if (fontFilename.Right(4) == ".shp" || fontFilename.Right(4) == ".SHP")
				fontItem->fontType = SHP_FONTTYPE;
			// EBATECH(CNBR) ]-

			strncpy(fontItem->fontName, fontFilename.GetBuffer(0), 32);

			// insert the data into the list and set it as the current font.
			int i = m_fontNameCtrl.FindStringExact(-1, fontFilename);
			m_fontNameCtrl.SetItemData(i, (DWORD) fontItem);
			m_fontNameCtrl.SetCurSel(i);
			m_fontStyleCtrl.DeleteStyleItems();
			m_fontStyleCtrl.ResetContent();
			m_fontLangCtrl.ResetContent(); // EBATECH(CNBR)
			}
		}
	else
		{
		if (m_fontNameCtrl.FindStringExact(-1, fontFamily) != -1){
			m_fontNameCtrl.SetCurrentFont(fontFamily);
			// EBATECH(CNBR) ]- Add
			FontItem* fontItem = (FontItem*)m_fontNameCtrl.GetItemData(m_fontNameCtrl.GetCurSel());
			if( (long)fontItem != 0 && (long)fontItem != CB_ERR ){
				LOGFONT* lfp = &fontItem->logFont;
				long stylenumber = CTTFInfoGetter::styleNumByFontParams(lfp->lfWeight, lfp->lfItalic, lfp->lfPitchAndFamily, lfp->lfCharSet );
				m_fontStyleCtrl.FillTrueTypeStyles(fontFamily,stylenumber);
				m_fontLangCtrl.FillTrueTypeLangs(fontFamily,stylenumber);
			}else{
				m_fontStyleCtrl.DeleteStyleItems();
				m_fontStyleCtrl.ResetContent();
				m_fontLangCtrl.ResetContent(); // EBATECH(CNBR)
			}
		}
		else
			{

			// add the truetype font to the list
			m_fontNameCtrl.AddString(fontFamily);
			FontItem* fontItem = NULL;
			fontItem = new FontItem;

			// get all of the fonts in all of the character sets
			LOGFONT logFont;
			logFont.lfCharSet = DEFAULT_CHARSET;
			strcpy(logFont.lfFaceName, fontFamily);
			logFont.lfPitchAndFamily = 0;

			// DP: TODO: REMOVE ENUMERATION
			// enumerate the fonts
			CClientDC dc(NULL);
			::EnumFontFamiliesEx(dc.GetSafeHdc(),
								 &logFont,
								 (FONTENUMPROC)CIcadFontComboBox::EnumFontFamExProc,
								 (LPARAM) this,
								 0);

			fontItem->logFont = logFont;
			fontItem->fontType = TRUETYPE_FONTTYPE;
			strncpy(fontItem->fontName, fontFamily.GetBuffer(0), 32);

			// insert the data into the list and set it as the current font.
			int i = m_fontNameCtrl.FindStringExact(-1, fontFamily);
			m_fontNameCtrl.SetItemData(i, (DWORD) fontItem);
			m_fontNameCtrl.SetCurSel(i);

			// EBATECH(CNBR) 2001-06-16 -[
			//m_fontStyleCtrl.ResetContent();
			long stylenumber = CTTFInfoGetter::styleNumByFontParams(logFont.lfWeight, logFont.lfItalic, logFont.lfPitchAndFamily, logFont.lfCharSet );
			m_fontLangCtrl.FillTrueTypeLangs(fontFamily,stylenumber);
			m_fontStyleCtrl.FillTrueTypeStyles(fontFamily,stylenumber);
			// EBATECH(CNBR) ]-

			if ((0x3000000 & m_tableInfo.GetFontStyle()) == 0x3000000) // Bold & italic
				m_fontStyleCtrl.AddString("Bold Italic");
			else if (0x2000000 & m_tableInfo.GetFontStyle())			// Bold
				m_fontStyleCtrl.AddString("Bold");
			else if (0x1000000 & m_tableInfo.GetFontStyle())			// Italic
				m_fontStyleCtrl.AddString("Italic");
			else
				m_fontStyleCtrl.AddString("Regular");
			m_fontStyleCtrl.SetCurSel(0);


			}
		}
	}

/******************************************************************************
*+ CTextStyleDlg::AddBigFontName
*
*/
void CTextStyleDlg::AddBigFontName()
{
	CString fontFilename = m_tableInfo.GetBigFontFilename(false);
	if( fontFilename.Compare(""/*DNT*/) == 0 ){
		fontFilename = CString(ResourceString(IDC_EXPSTYLE_NO_BIGFONT, "(none)"));
	}
	if (m_bigfontNameCtrl.FindStringExact(-1, fontFilename) != -1){
		m_bigfontNameCtrl.SetCurrentFont(fontFilename);
	}else{
		// add the shx font to the list.
		m_bigfontNameCtrl.AddString(fontFilename);
		int i = m_bigfontNameCtrl.FindStringExact(-1, fontFilename);
		if( i != CB_ERR ){
			m_bigfontNameCtrl.SetCurSel(i);
		}
	}
	if( m_tableInfo.GetHasTrueTypeFont() ){
		m_bigfontNameCtrl.EnableWindow(FALSE);
	}else{
		m_bigfontNameCtrl.EnableWindow(TRUE);
	}
}

/******************************************************************************
*+ CTextStyleDlg::OnChangeStyleHeight - adjusts the dirty flag and Apply button
*
*/

void CTextStyleDlg::OnChangeStyleHeight()
	{
	CString dlgStrValue;

	m_heightCtrl.GetWindowText(dlgStrValue);
	if (dlgStrValue.IsEmpty())
		return;

	sds_real dlgRealValue = atof(dlgStrValue);

	UpdateDirtyFlag(m_tableInfo.GetHeight(), dlgRealValue, DIRTY_HEIGHT);
	SetApplyButton();
	}

/******************************************************************************
*+ CTextStyleDlg::OnChangeStyleWidth - adjusts the dirty flag, Apply button, and preview
*
*/

void CTextStyleDlg::OnChangeStyleWidth()
	{
	CString dlgStrValue;

	m_widthCtrl.GetWindowText(dlgStrValue);
	if (dlgStrValue.IsEmpty())
		return;

	sds_real dlgRealValue = atof(dlgStrValue);

	UpdateDirtyFlag(m_tableInfo.GetWidth(), dlgRealValue, DIRTY_WIDTH);
	SetApplyButton();
	DrawFontPreview();
	}

/******************************************************************************
*+ CTextStyleDlg::OnChangeStyleOblique - adjusts the dirty flag, Apply button, and preview
*
*/

void CTextStyleDlg::OnChangeStyleOblique()
	{
	CString dlgStrValue;

	m_obliqueAngleCtrl.GetWindowText(dlgStrValue);
	if (dlgStrValue.IsEmpty())
		return;

	sds_real dlgRealValue = (IC_PI/180) * atof(dlgStrValue);	// need value in radians

	UpdateDirtyFlag(m_tableInfo.GetObliqueAngle(), dlgRealValue, DIRTY_OBLIQUE);
	SetApplyButton();
	DrawFontPreview();
	}

/******************************************************************************
*+ CTextStyleDlg::OnKillFocusStyleHeight - traps text height entry errors
*
*/

void CTextStyleDlg::OnKillFocusStyleHeight()
	{
	char dlgStrValue[IC_ACADBUF];

	// get the value from the dialog
	// if there is no value, stay in the edit box
	if (!m_heightCtrl.GetWindowText(dlgStrValue, IC_ACADBUF - 1))
		{
		m_heightCtrl.SetFocus();
		return;
		}

	// if the number contains invalid characters or is out of range,
	// display a message and select the invalid value
	sds_real dlgRealValue;
	bool goodValue = ic_strIsValidReal(dlgStrValue, false, false, &dlgRealValue);
	if (!goodValue || (dlgRealValue < MIN_TEXT_HEIGHT))
		{
		char msg[IC_ACADBUF];
		sprintf(msg, ResourceString(IDC_TEXTSTYLE_HEIGHT_INVALID, "The text height must be %g or greater."),
			MIN_TEXT_HEIGHT);
		MessageBox(msg, ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ), MB_ICONEXCLAMATION);
		m_heightCtrl.SetFocus();
		m_heightCtrl.SetSel(0, -1);
		return;
		}
	}

/******************************************************************************
*+ CTextStyleDlg::OnKillFocusStyleWidth - traps width factor entry errors
*
*/

void CTextStyleDlg::OnKillFocusStyleWidth()
	{
	char dlgStrValue[IC_ACADBUF];

	// get the value from the dialog
	// if there is no value, stay in the edit box
	if (!m_widthCtrl.GetWindowText(dlgStrValue, IC_ACADBUF - 1))
		{
		m_widthCtrl.SetFocus();
		return;
		}

	// if the number contains invalid characters or is out of range,
	// display a message and select the invalid value
	sds_real dlgRealValue;
	bool goodValue = ic_strIsValidReal(dlgStrValue, false, false, &dlgRealValue);
	if (!goodValue || (dlgRealValue < MIN_WIDTH_FACTOR) || (dlgRealValue > MAX_WIDTH_FACTOR))
		{
		char msg[IC_ACADBUF];
		sprintf(msg, ResourceString(IDC_TEXTSTYLE_WIDTH_INVALID, "The width factor must be from\n%g to %g."),
			MIN_WIDTH_FACTOR, MAX_WIDTH_FACTOR);
		MessageBox(msg, ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ), MB_ICONEXCLAMATION);
		m_widthCtrl.SetFocus();
		m_widthCtrl.SetSel(0, -1);
		return;
		}
	}

/******************************************************************************
*+ CTextStyleDlg::OnKillFocusStyleOblique - traps oblique angle entry errors
*
*/

void CTextStyleDlg::OnKillFocusStyleOblique()
	{
	char dlgStrValue[IC_ACADBUF];

	// get the value from the dialog
	// if there is no value, stay in the edit box
	if (!m_obliqueAngleCtrl.GetWindowText(dlgStrValue, IC_ACADBUF - 1))
		{
		m_obliqueAngleCtrl.SetFocus();
		return;
		}

	// make sure the angle is within the min and max values
	sds_real dlgRealValue;
	bool goodValue = ic_strIsValidReal(dlgStrValue, true, false, &dlgRealValue);
	if (goodValue)
		{
		ic_roundReal(dlgRealValue);			// round to whole degrees
		ic_condenseAngle(dlgRealValue);		// condense angles >= 360 to 0 to 359
		ConvertObliqueAngle(dlgRealValue);	// handle angles >= 275 and <= -275
		if ((dlgRealValue < MIN_OBLIQUE_ANGLE) || (dlgRealValue > MAX_OBLIQUE_ANGLE))
			goodValue = false;
		}

	// if the value is still good, display the rounded, condensed, converted value
	if (goodValue)
		{
		char tmpStr[IC_ACADBUF];
		sprintf(tmpStr, "%d", (int) dlgRealValue);
		m_obliqueAngleCtrl.SetWindowText(tmpStr);
		}
	else
		{
		char msg[IC_ACADBUF];
		sprintf(msg, ResourceString(IDC_TEXTSTYLE_ANGLE_INVALID, "The oblique angle must be from\n%d to %d."),
			MIN_OBLIQUE_ANGLE, MAX_OBLIQUE_ANGLE);
		MessageBox(msg, ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ), MB_ICONEXCLAMATION);
		m_obliqueAngleCtrl.SetFocus();
		m_obliqueAngleCtrl.SetSel(0, -1);
		}
	}

/******************************************************************************
*+ CTextStyleDlg::OnStyleBackwards - adjusts the dirty flag, Apply button, and preview
*
*/

void CTextStyleDlg::OnStyleBackwards()
	{
	BOOL checked = m_backwardsCtrl.GetCheck();
	UpdateDirtyFlag(m_tableInfo.GetBackwards(), checked, DIRTY_BACKWARDS);
	SetApplyButton();
	DrawFontPreview();
	}

/******************************************************************************
*+ CTextStyleDlg::OnStyleUpsideDown - adjusts the dirty flag, Apply button, and preview
*
*/

void CTextStyleDlg::OnStyleUpsideDown()
	{
	BOOL checked = m_upsideDownCtrl.GetCheck();
	UpdateDirtyFlag(m_tableInfo.GetUpsideDown(), checked, DIRTY_UPSIDE_DOWN);
	SetApplyButton();
	DrawFontPreview();
	}

/******************************************************************************
*+ CTextStyleDlg::OnStyleVertically - adjusts the dirty flag, Apply button, and preview
*
*/

void CTextStyleDlg::OnStyleVertically()
	{
	BOOL checked = m_verticallyCtrl.GetCheck();
	UpdateDirtyFlag(m_tableInfo.GetVertically(), checked, DIRTY_VERTICALLY);
	SetApplyButton();
	DrawFontPreview();
	}




/////////////////////////////////////////////////////////////////////////////
// CTextStyle_IEXP_Newdlg message handlers

BEGIN_MESSAGE_MAP(CTextStyle_IEXP_Newdlg, IEXP_Newdlg)
END_MESSAGE_MAP()


/******************************************************************************
*+ CTextStyle_IEXP_Newdlg::OnInitDialog - set up for the dialog to be displayed
*
*/

afx_msg BOOL CTextStyle_IEXP_Newdlg::OnInitDialog(
	)
	{
	CDialog::OnInitDialog();

	// size the list box and move it to the desired location
	RECT rect;
	long width, height;
	GetWindowRect(&rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
	rect.top = m_top;
	rect.left = m_left;
	rect.bottom = m_top + height;
	rect.right = m_left + width;
	MoveWindow(&rect);

	char messageBuf[IC_ACADBUF];
	sprintf(messageBuf, ResourceString(IDC_ICADEXPLORER_NEW__S_143, "New %s" ), m_type);
	SetDlgItemText(EXP_DEL_LISTTXT, messageBuf);

	// fill the list box with the names of all of the styles in the drawing
	// except for the name of the style being deleted
	m_ListWnd = GetDlgItem(EXP_DEL_LIST);

	resbuf *rb;
	rb = SDS_tblgetter("STYLE"/*DNT*/, NULL, TRUE, SDS_CURDWG);		// to do - right now, this uses the current drawing
	while (rb)														// a nice enhancement would be to handle any specified drawing
		{
		resbuf *nameRb = ic_ret_assoc(rb, 2);
		if (NULL == nameRb)
			return(FALSE);

		if (!strisame(m_String, nameRb->resval.rstring))
			((CListBox *) m_ListWnd)->AddString(nameRb->resval.rstring);

		sds_relrb(rb);
		rb = SDS_tblgetter("STYLE"/*DNT*/, NULL, FALSE, SDS_CURDWG);
		}

	return(TRUE);
	}




/////////////////////////////////////////////////////////////////////////////
// CTextStyle_IEXP_MessItem message handlers

BEGIN_MESSAGE_MAP(CTextStyle_IEXP_MessItem, IEXP_MessItem)
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(EXP_DEL_CHANGE, OnChangeStyle)
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()


/******************************************************************************
*+ CTextStyle_IEXP_MessItem::OnInitDialog - set up for the dialog to be displayed
*
*/

BOOL CTextStyle_IEXP_MessItem::OnInitDialog(
	)
	{
	CDialog::OnInitDialog();

	m_txtStyleNewDlg.m_Dlgflag = FALSE;
	m_txtStyleNewDlg.m_state = FALSE;

	SetWindowText(m_title);

	if (!m_Continue.IsEmpty())
		SetDlgItemText(EXP_DEL_CHANGE, m_Continue);

	return(TRUE);
	}

/******************************************************************************
*+ CTextStyle_IEXP_MessItem::OnActivate - kill the dialog if called for wrong reason
*
*/

afx_msg void CTextStyle_IEXP_MessItem::OnActivate(
	UINT nState,
	CWnd *pWndOther,
	BOOL bMinimized
	)
	{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	if ((WA_ACTIVE == nState) && m_txtStyleNewDlg.m_state)
		EndDialog(EXP_DEL_CHANGE);
	}

/******************************************************************************
*+ CTextStyle_IEXP_MessItem::OnChangeStyle - display a list of styles to substitute
*
*/

afx_msg void CTextStyle_IEXP_MessItem::OnChangeStyle(
	)
	{
	ASSERT(!m_typeflag);		// m_typeflag should always be 0 for this dialog

	if (m_typeflag)
		EndDialog(EXP_DEL_CHANGE);

	//Bugzilla No. 78163 ; 12-05-2003 
	// display a list of text styles to change to
	// directly below the Change button
	if ( !m_txtStyleNewDlg )
	{
	CWnd *styleListWnd = GetDlgItem(EXP_DEL_CHANGE);
	ASSERT(styleListWnd != NULL);
	RECT rect;
	styleListWnd->GetWindowRect(&rect);

	m_txtStyleNewDlg.m_top = rect.bottom;
	m_txtStyleNewDlg.m_left = rect.left;

	m_txtStyleNewDlg.Create(EXP_DEL_NEWLIST, this);

	m_txtStyleNewDlg.m_Dlgflag = TRUE;
	}

	m_txtStyleNewDlg.ShowWindow(SW_SHOWNORMAL);

		
	}

/******************************************************************************
*+ CTextStyle_IEXP_MessItem::OnWindowPosChanged - move the style list if the main dialog moves
*
*/

afx_msg void CTextStyle_IEXP_MessItem::OnWindowPosChanged(
	WINDOWPOS *lpwndpos)
	{
	RECT listRect, newDlgRect;
	CWnd *styleListWnd;

	if (!m_typeflag && m_txtStyleNewDlg.m_Dlgflag)
		{
		styleListWnd = GetDlgItem(EXP_DEL_CHANGE);
		styleListWnd->GetWindowRect(&listRect);
		m_txtStyleNewDlg.GetWindowRect(&newDlgRect);
		m_txtStyleNewDlg.MoveWindow(listRect.left,
									listRect.bottom,
									newDlgRect.right - newDlgRect.left,
									newDlgRect.bottom - newDlgRect.top);
		}
	}

void CTextStyleDlg::OnPaint()
{
	CDialog::OnPaint();
	DrawFontPreview();
}


////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

void CMainWindow::IcadTextStyleDia()
{
	CTextStyleDlg	textStyleDlg;

	textStyleDlg.DoModal();

	/*DG - 9.1.2002*/// Regen all if necessary.
	if(textStyleDlg.m_bNeedRegen)
		cmd_regenall();
}
