/* D:\ICADDEV\PRJ\ICAD\MTEXTPROPSHEET.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

// CMTextPropSheet.cpp : implementation file
//

#include "icad.h"
#include "IcadApi.h"
#include "MTextPropSheet.h"
#include "resource.hm"
#include "paths.h"
#include "truetypeutils.h"
#include "Preferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define HEIGHT_COEF 1.34
#define TWIPS_PER_INCH 1440.0

//Modified Cybage AW 30/04/2001 [
//Reason : A constant view object is maintained for the 
//text editor, so that it is not affected by zoom factors
#define MIDWIN_0				384
#define MIDWIN_1				142
#define CENTERPROJECTION_0		6.24334	
#define CENTERPROJECTION_1		4.5
#define PIXELHEIGHT				-0.0316901
#define PIXELWIDTH				0.0316901
#define VIEWSIZE				9.0
//Modified Cybage AW 30/04/2001 ]


/****************************************************************************
* Author:		Denis Petrov
* Description:	Converts height in units into height in twips 
*				(1 twip = 1 / 1440 inch)
****************************************************************************/
void ConvertTextHeightToTwips(sds_real realHeight, long& twipsHeight)
{
	ASSERT(realHeight >= 0.0);

	//Modified Cybage AW 30/04/2001[
	//Reason : The current Zoom factor is applied on the text editor. To remove
	//this bug the values needed for coordinate conversion are hard coded in the 
	//view object.
	//struct gr_view* pView = SDS_CURGRVW;
	struct gr_view* pView = new gr_view;

	pView->midwin[0] = MIDWIN_0;
	pView->midwin[1] = MIDWIN_1;

	sds_point centerProjection;
	centerProjection[0] = CENTERPROJECTION_0;
	centerProjection[1] = CENTERPROJECTION_1;
	centerProjection[2] = 0.0;
	pView->SetCenterProjection(centerProjection);
	
	pView->SetPixelHeight(PIXELHEIGHT);
	pView->SetPixelWidth(PIXELWIDTH);

	pView->viewsize = VIEWSIZE;
	//Modified Cybage AW 30/04/2001 ]

	int pixelsHeight, pixelsPerInch = 96;
	ConvertTextHeightToPix(realHeight, pView, pixelsHeight);
	

	HDC hdcDisplay = CreateDC("DISPLAY", NULL, NULL, NULL);
	if(hdcDisplay)
		pixelsPerInch = GetDeviceCaps(hdcDisplay, LOGPIXELSY);
	DeleteDC(hdcDisplay);
	twipsHeight = long(HEIGHT_COEF * TWIPS_PER_INCH * (double)pixelsHeight / (double)pixelsPerInch + 0.5);

	//Modified Cybage AW 30/04/2001
	delete pView;
}

/****************************************************************************
* Author:		Denis Petrov
* Description:	Converts height in twips into height in units
*				(1 twip = 1 / 1440 inch)
****************************************************************************/
void ConvertTwipsToTextHeight(sds_real& realHeight, long twipsHeight)
{
	ASSERT(twipsHeight >= 0);
	//Modified Cybage AW 30/04/2001[
	//Reason : The current Zoom factor is applied on the text editor. To remove
	//this bug the values needed for coordinate conversion are hard coded in the 
	//view object.
	//struct gr_view* pView = SDS_CURGRVW;
	struct gr_view* pView = new gr_view;

	pView->midwin[0] = MIDWIN_0;
	pView->midwin[1] = MIDWIN_1;

	sds_point centerProjection;
	centerProjection[0] = CENTERPROJECTION_0;
	centerProjection[1] = CENTERPROJECTION_1;
	centerProjection[2] = 0.0;
	pView->SetCenterProjection(centerProjection);
	
	pView->SetPixelHeight(PIXELHEIGHT);
	pView->SetPixelWidth(PIXELWIDTH);

	pView->viewsize = VIEWSIZE;
	//Modified Cybage AW 30/04/2001 ]

	int pixelsHeight, pixelsPerInch = 96;
	HDC hdcDisplay = CreateDC("DISPLAY", NULL, NULL, NULL);
	if(hdcDisplay)
		pixelsPerInch = GetDeviceCaps(hdcDisplay, LOGPIXELSY);
	DeleteDC(hdcDisplay);
	pixelsHeight = long((double)twipsHeight * (double)pixelsPerInch / (HEIGHT_COEF * TWIPS_PER_INCH) + 0.5);
	ConvertPixToTextHeight(realHeight, pView, pixelsHeight);

	//Modified Cybage AW 30/04/2001
	delete pView;
}

/////////////////////////////////////////////////////////////////////////////
// CMTextPropSheet

IMPLEMENT_DYNAMIC(CMTextPropSheet, CPropertySheet)

CMTextPropSheet::CMTextPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	AddPage( &m_textTab);
	AddPage( &m_propertiesTab);
}

CMTextPropSheet::CMTextPropSheet(UINT nIDCaption, SDS_mTextData* dataPtr, CWnd* pParentWnd, UINT iSelectPage)
:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	AddPage( &m_textTab);
	AddPage( &m_propertiesTab);
	SetActivePage(&m_textTab);
	m_pMTextData = dataPtr;
}

CMTextPropSheet::CMTextPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	ASSERT( !"Used");
}

CMTextPropSheet::~CMTextPropSheet()
{
}

BEGIN_MESSAGE_MAP(CMTextPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMTextPropSheet)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	// Bugzilla No. 78437; 30-01-2003
	ON_NOTIFY(EN_SELCHANGE,IDC_RICHEDIT,OnSelChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/****************************************************************************
* Author:		Denis Petrov
* Description:	In this realization we must know where dialog was called
*				(from property page or in mtext command handler). 
*				It's bad that we must get this information from air.
*				This function return TRUE if dialog was called after mtext 
*				command handler.
****************************************************************************/

// Bugzilla No. 78437; 30-01-2003 [
void CMTextPropSheet::OnSelChange( SELCHANGE * pNotifyStruct, LRESULT * result )
{
	if(m_bPasting == FALSE && (pNotifyStruct->chrg.cpMin == pNotifyStruct->chrg.cpMax))
	{
		if(m_nStartChar != m_nEndChar)   // Some text is selected
			m_nStartPaste = m_nStartChar;
		else
			m_nStartPaste = m_nEndPaste; // No text is selected

		m_nEndPaste = pNotifyStruct->chrg.cpMax;
	}
}
// Bugzilla No. 78437; 30-01-2003 ]

BOOL CMTextPropSheet::needCreation()
{
	return strcmp(m_pParentWnd->GetRuntimeClass()->m_lpszClassName, "CDialog");
}

/////////////////////////////////////////////////////////////////////////////
// CMTextPropSheet message handlers
#define EDIT_HEIGHT 120
#define CONTROLS_OFFSET 10
#define MIN_MTEXT_DIALOG_WIDTH 595
#define MIN_MTEXT_DIALOG_HEIGHT	285

BOOL CMTextPropSheet::OnInitDialog()
{
	// redesigned by DP
	// Skip IcadPropSheet since this handles button repositioning
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	// Bugzilla No. 78437; 30-01-2003 [
	m_bPasting = FALSE;
	m_nStartPaste = m_nEndPaste = 0;
	// Bugzilla No. 78437; 30-01-2003 ]
	// DP: it's serious
	// DP: see realization of GetParent() ;)
	m_pParentWnd = GetParent();
	
	CWnd* pButtonApply = GetDlgItem(ID_APPLY_NOW);
	pButtonApply->ShowWindow(SW_HIDE);
	
	CWnd* pButtonOk = GetDlgItem(IDOK);
	CWnd* pButtonCancel = GetDlgItem(IDCANCEL);
	//Save position info so we can move other buttons into position
	CRect okRect, cancelRect, tabRect;
	pButtonOk->GetWindowRect(okRect);
	pButtonCancel->GetWindowRect(cancelRect);
	
	GetTabControl()->GetWindowRect(tabRect);
	
	//Resize window for Rich Edit control
	CRect ppRect, editRect;
	GetWindowRect(ppRect);
	editRect.top = tabRect.bottom + CONTROLS_OFFSET;
	editRect.left = tabRect.left;
	editRect.right = tabRect.right;
	editRect.bottom = editRect.top + EDIT_HEIGHT;
	
	cancelRect.bottom -= cancelRect.top;
	cancelRect.top = editRect.bottom + CONTROLS_OFFSET;
	cancelRect.bottom += cancelRect.top;
	cancelRect.left -= cancelRect.right;
	cancelRect.right = tabRect.right;
	cancelRect.left += cancelRect.right;
	
	okRect.bottom -= okRect.top;
	okRect.top = editRect.bottom + CONTROLS_OFFSET;
	okRect.bottom += okRect.top;
	okRect.left -= okRect.right;
	okRect.right = cancelRect.left - CONTROLS_OFFSET;
	okRect.left += okRect.right;
	
	ppRect.bottom = cancelRect.bottom + CONTROLS_OFFSET;
	MoveWindow(ppRect);
	
	// Create Help button
	CRect helpRect;
	helpRect.top = cancelRect.top;
	helpRect.bottom = cancelRect.bottom;
	helpRect.left = tabRect.left;
	helpRect.right = tabRect.left + cancelRect.bottom - cancelRect.top;
	ScreenToClient(helpRect);
	
	m_help.Create("", WS_CHILD | WS_VISIBLE | BS_ICON, helpRect, this, IDHELP);
	m_help.SetIcon(AfxGetApp()->LoadIcon(HELP_ICON));
	
	//Move OK and Cancel buttons;
	ScreenToClient(okRect);
	ScreenToClient(cancelRect);
	pButtonOk->MoveWindow(okRect, TRUE);
	pButtonCancel->MoveWindow(cancelRect, TRUE);
	
	ScreenToClient(editRect);
	m_edit.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER |
		ES_NOHIDESEL | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN, 
		editRect, this, IDC_RICHEDIT);
	
	CFont& currentFont = m_textTab.SelectedFont();
	m_edit.SetFont(&currentFont);

	const double piInverted =  0.31830988618379067154;
	if(needCreation())
		m_pMTextData->rRotAngle *= (180.0 * piInverted);

	//Want OnInitDialog to be called for properties tab.
	SetActivePage(&m_propertiesTab);
	SetActivePage(&m_textTab);

	m_textTab.init(m_pMTextData);
	m_propertiesTab.init(m_pMTextData);
	m_edit.init(m_pMTextData);
	m_edit.SetFocus();
	
	// Add size to system menu
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->InsertMenu(
		 -1,MF_BYPOSITION|MF_STRING,SC_SIZE,"&Size");
	}
	// Add resizable frame
	::SetWindowLong(m_hWnd,GWL_STYLE,
		GetStyle()|WS_THICKFRAME);

	RECT rectMtext;
	CString str = UserPreference::s_MtextPosition;
	sscanf(str,"%d %d %d %d",&rectMtext.left, &rectMtext.top, &rectMtext.right, &rectMtext.bottom ); 
	SetWindowPos(NULL, rectMtext.left, rectMtext.top,
	rectMtext.right - rectMtext.left, rectMtext.bottom - rectMtext.top, SWP_SHOWWINDOW);

	return bResult;
}

void CMTextPropSheet::OnHelp()
{
	CommandRefHelp(m_hWnd, HLP_MTEXT);
}

BOOL CMTextPropSheet::DestroyWindow()
{
	TEXTRANGE range;
	range.chrg.cpMin = 0;
	range.chrg.cpMax = m_edit.GetTextLength();
	range.lpstrText = m_text.GetBufferSetLength(range.chrg.cpMax);
	m_edit.SendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&range);
	m_text.ReleaseBuffer(-1);
	return CPropertySheet::DestroyWindow();
}

void CMTextPropSheet::UpdateButtons()
{
	if(m_textTab.m_font.GetFontType() == TRUETYPE_FONTTYPE)
	{
		m_textTab.m_bold.EnableWindow();
		m_textTab.m_italic.EnableWindow();
	}
	else
	{
		if(m_textTab.m_bold.GetCheck() == false)
			m_textTab.m_bold.EnableWindow(FALSE);
		if(m_textTab.m_italic.GetCheck() == false)
			m_textTab.m_italic.EnableWindow(FALSE);
	}
}

#pragma warning(disable:4800)
void CMTextPropSheet::UpdateCharFormat()
{
	CHARFORMAT cf;
	m_edit.GetSelectionCharFormat(cf);
	CString curFont, curFontSize;
	curFont = GetFontCombo()->GetFontItem()->fontName;
	GetFontSizeCombo()->GetLBText(GetFontSizeCombo()->GetCurSel(), curFontSize);
	bool bFormatChanged = false;
	if(curFont.Compare(cf.szFaceName))
	{
		GetFontCombo()->SetCurrentFont(cf.szFaceName);
		bFormatChanged = true;
	}
	if(m_textTab.m_font.GetFontType() == TRUETYPE_FONTTYPE)
	{
		if(static_cast<bool>(m_textTab.m_bold.GetCheck()) != static_cast<bool>(cf.dwEffects & CFE_BOLD))
		{
			m_textTab.m_bold.SetCheck(1);
			bFormatChanged = true;
		}
		if(static_cast<bool>(m_textTab.m_italic.GetCheck()) != static_cast<bool>(cf.dwEffects & CFE_ITALIC))
		{
			m_textTab.m_italic.SetCheck(1);
			bFormatChanged = true;
		}
		if(static_cast<bool>(m_textTab.m_underline.GetCheck()) != static_cast<bool>(cf.dwEffects & CFE_UNDERLINE))
		{
			m_textTab.m_underline.SetCheck(1);
			bFormatChanged = true;
		}
	}
	UpdateButtons();
	if(false)//!bFormatChanged)
	{
		cf.dwMask = CFM_CHARSET | CFM_FACE | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;
		_tcscpy(cf.szFaceName, GetFontCombo()->GetFontItem()->fontName);
		//cf.bCharSet = GetFontCombo()->GetFontItem()->fontInfo()->lfCharSet;
		if(m_textTab.m_bold.GetCheck())
			cf.dwEffects |= CFE_BOLD;
		if(m_textTab.m_italic.GetCheck())
			cf.dwEffects |= CFE_ITALIC;
		if(m_textTab.m_underline.GetCheck())
			cf.dwEffects |= CFE_UNDERLINE;
		m_edit.SetSelectionCharFormat(cf);
	}
}

// Bugzilla No. 78437; 30-01-2003 [
void CMTextPropSheet::OnPaste()
{
	BOOL isLocked = m_edit.LockWindowUpdate();

	m_bPasting = TRUE;
	CPoint currPos = m_edit.GetCaretPos();

	CHARFORMAT cf;
	sds_real newsize;
	for(int i = m_nStartPaste + 1; i <= m_nEndPaste; i++)
	{
		m_edit.SetSel(i,i);
		m_edit.GetSelectionCharFormat(cf);

		int nIndex = m_textTab.GetTextHeightFromTwips(newsize, cf.yHeight);
		if(nIndex == -1 ) 
		{			
			ConvertTwipsToTextHeight(newsize, cf.yHeight);
			CString height;
			height.Format("%.4f", newsize);
			nIndex = GetFontSizeCombo()->AddString(height);
			GetFontSizeCombo()->SetItemData(nIndex,cf.yHeight);
		}
	}
	m_edit.SetCaretPos(currPos);
	m_bPasting = FALSE;

	m_edit.scaleText(m_nStartPaste, m_nEndPaste-1, GetZoom());

	if(isLocked)
		m_edit.UnlockWindowUpdate();
}
// Bugzilla No. 78437; 30-01-2003 ]

void CMTextPropSheet::UpdateChar()
{
	// Bugzilla No. 78437; 30-01-2003
	m_edit.GetSel(m_nStartChar,m_nEndChar);
	CHARFORMAT cf;
	m_edit.GetSelectionCharFormat(cf);
	m_textTab.m_bold.SetCheck(cf.dwEffects & CFE_BOLD);
	m_textTab.m_italic.SetCheck(cf.dwEffects & CFE_ITALIC);
	m_textTab.m_underline.SetCheck(cf.dwEffects & CFE_UNDERLINE);
	m_textTab.m_font.SetCurrentFont(cf.szFaceName);
	UpdateButtons();
	
	// Bugzilla No. 78034; 23-12-2002 [
	double newsize;
	int index = m_textTab.GetTextHeightFromTwips(newsize, cf.yHeight);
	if(index != -1 ) 
		m_textTab.m_fontsize.SetCurSel(index);
	// Bugzilla No. 78034; 23-12-2002 ]

	if(!m_textTab.m_undo.IsWindowEnabled() && m_edit.CanUndo())
		m_textTab.m_undo.EnableWindow(TRUE);
}

void CMTextPropSheet::OnDestroy()
{
	CPropertySheet::OnDestroy();
	
	CString text = "";
	m_edit.SetSel(0, 0);
	int index = 0, bold = 0, italic = 0, underline = 0, fontsize = 200;
	int textindex = 0, oldbold = 0, olditalic = 0, oldunderline = 0, oldfontsize = -1;
	CString fonttype = "Arial", oldfonttype = "", height = "";
	bool isSHX = 0;
	int pitch = 0, charset = 0; // EBATECH(CNBR)
	
	double defaultsize;
	struct resbuf rb;
    SDS_getvar(NULL,DB_QTEXTSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	
	m_propertiesTab.m_cstyle.GetTextStyle();
	defaultsize = m_propertiesTab.m_cstyle.m_tableInfo.GetHeight();
	if (defaultsize <= 0.0009)
		defaultsize = rb.resval.rreal;	//Default fontheight
	// Bugzilla No. 78394; 09-12-2002
	//m_pMTextData->rTextHeight = defaultsize;
	
	//DP: old realization commented
	//oldfontsize = (int) (defaultsize * 1000);
	ConvertTextHeightToPix(defaultsize, SDS_CURGRVW, oldfontsize);
	
	fonttype = m_propertiesTab.m_cstyle.m_tableInfo.GetFontFilename(false);
	if (fonttype.Right(4) == ".shx" || fonttype.Right(4) == ".SHX" ||
		fonttype.Right(4) == ".shp" || fonttype.Right(4) == ".SHP")
		isSHX = 1;
	if (!isSHX)
		oldfonttype = m_propertiesTab.m_cstyle.m_tableInfo.GetFontFamily();
	else
		oldfonttype = m_propertiesTab.m_cstyle.m_tableInfo.GetFontFilename(false);
	
	
	CHARFORMAT cf;
	while (index <= m_edit.GetTextLength())
	{
		m_edit.SetSel(index, index);
		m_edit.GetSelectionCharFormat(cf);
		bold = (cf.dwEffects & CFE_BOLD);
		italic = (cf.dwEffects & CFE_ITALIC);
		underline = (cf.dwEffects & CFE_UNDERLINE);
		fontsize = cf.yHeight;
		fonttype = cf.szFaceName;
		isSHX = 0;
		// EBATECH(CNBR) -[ Accept any language
		pitch = ( cf.bPitchAndFamily & 0x00ff); // EBATECH(CNBR)
		charset = ( cf.bCharSet & 0x00ff );	// EBATECH(CNBR)

		int i = m_textTab.m_font.FindStringExact(-1, fonttype);
		if(i == -1)
		{ // Not found set default
			pitch = ( FF_SWISS | VARIABLE_PITCH );
			charset = ANSI_CHARSET;
		}
		//if (i == -1)
		//	i = 34;
		//FontItem* pFontItem = (FontItem*)m_textTab.m_font.GetItemDataPtr(i);
		//if(pFontItem)
		//	pitch.Format(_T ("%d"), pFontItem->logFont.lfPitchAndFamily);
		
		double newsize;
		// Bugzilla No. 78034; 23-12-2002 
		m_textTab.GetTextHeightFromTwips(newsize, fontsize);

		height.Format(_T ("%.4lf"), newsize);
		while(height.Right(1) == '0')
			height.Delete(height.GetLength() - 1, 1);
		
		// Bugzilla No. 78394; 09-12-2002
		if(index == 0)
			m_propertiesTab.m_cstyle.m_tableInfo.SetHeight(atof(height));
			//m_pMTextData->rTextHeight = atof(height);
			
					
		if (fonttype.Right(4) == ".shx" || fonttype.Right(4) == ".SHX" ||
			fonttype.Right(4) == ".shp" || fonttype.Right(4) == ".SHP")
			isSHX = 1;
		
		//the formatting information for italic, bold, and fonttype are redisplayed if one is
		//changed.  The usage of the c value has not been determined yet.
		if (bold != oldbold || italic != olditalic || fonttype.CompareNoCase(oldfonttype) != 0)
		{
			CString format = "\\f";
			format.Insert(format.GetLength(), fonttype);
			if (!isSHX)
			{
				/* EBATECH(CNBR) Set various charaset and pitch.
				if (bold == 0)
					format.Insert(format.GetLength(), "|b0");
				else
					format.Insert(format.GetLength(), "|b1");
				if (italic == 0)
					format.Insert(format.GetLength(), "|i0|c0|p");
				else
					format.Insert(format.GetLength(), "|i1|c0|p");
				format.Insert(format.GetLength(), pitch);*/
				CString fontFormat;
				fontFormat.Format(
					"|b%d|i%d|c%d|p%d",
					(bold	 == 0 ? 0 : 1),
					(italic == 0 ? 0 : 1),
					charset,
					pitch);
				format.Insert(format.GetLength(),fontFormat);
				// EBATECH(CNBR) ]-
			}
			format.Insert(format.GetLength(), ";");
			m_text.Insert(textindex, format);
			textindex += format.GetLength();
		}
		
		if (fontsize != oldfontsize && index != 0)
		{
			CString format;
			/* 
			if (textindex >= 1)
			{
			if (m_text.GetAt(textindex-1) != ';')
			//insert a space between the text and the formatting.
			format = " \\H";
			else
			format = "\\H";
			}
			else
			format = "\\H";
			*/
			// DP: next line inserted instead above statement
			format = "\\H";
			format.Insert(format.GetLength(), height);
			format.Insert(format.GetLength(), ";");
			m_text.Insert(textindex, format);
			textindex += format.GetLength();
		}
		
		if (underline != oldunderline)
		{
			if (underline == 0)
				m_text.Insert(textindex, "\\l");
			else
				m_text.Insert(textindex, "\\L");
			textindex += 2;
		}
		
		//the index counter does not pick up the change in formatting until a space after it is
		//necessary.  Textindex must start at 0, however, to properly insert the preliminary
		//formatting information.
		if (index != 0)
			textindex++;
		index++;
		oldfonttype = fonttype;
		oldbold = bold;
		olditalic = italic;
		oldunderline = underline;
		oldfontsize = fontsize;
	}
		
	// checking for strings with formatting information but no text, and clearing the string.
	index = m_text.Find("\\f");
	int length = m_text.GetLength();
	if (index == 0)
	{
		index = m_text.Find(";");
		if (index != -1)
		{
			int nextindex = m_text.Find("\\H", index);
			if (nextindex - 1 == index)
			{
				index = m_text.Find(";", nextindex);
				if (index != -1)
				{
					nextindex = m_text.Find("\\L");
					if (nextindex - 1 == index)
					{
						index = nextindex + 2;
						if (index >= length)
							m_text.Delete(0, length);
					}
					else
					{
						index++;
						if (index >= length)
							m_text.Delete(0, length);
					}
				}
			}
			else
			{
				nextindex = m_text.Find("\\L");
				if (nextindex - 1 == index)
				{
					index = nextindex + 2;
					if (index >= length)
						m_text.Delete(0, length);
				}
				else
				{
					index++;
					if (index >= length)
						m_text.Delete(0, length);
				}
			}
		}
	}
	else
	{
		int nextindex = m_text.Find("\\H");
		if (nextindex - 1 == index)
		{
			index = m_text.Find(";");
			if (index != -1)
			{
				nextindex = m_text.Find("\\L");
				if (nextindex - 1 == index)
				{
					index = nextindex + 2;
					if (index >= length)
						m_text.Delete(0, length);
				}
				else
				{
					index++;
					if (index >= length)
						m_text.Delete(0, length);
				}
			}
		}
		else
		{
			nextindex = m_text.Find("\\L");
			if (nextindex - 1 == index)
			{
				index = nextindex + 2;
				if (index >= length)
					m_text.Delete(0, length);
			}
			else
			{
				index++;
				if (index >= length)
					m_text.Delete(0, length);
			}
		}
	}
	// Bugzilla No. 78394; 09-12-2002 [
	/*if(m_propertiesTab.m_hWnd)
	{
		m_propertiesTab.m_cwidth.GetWindowText(text);
		if (text == "(no wrap)")
		{
			//Modified Cybage AW 12-05-01
			//Reason : "no wrap" option in width edit box was not retained.
			//m_width = -1.0;
			m_pMTextData->rBoxWidth = 0.0;
		}
		else
			m_pMTextData->rBoxWidth = atof(text);
	}

	m_text.Replace("\r","\\P");
	m_text.Replace("\n","");
	
	m_pMTextData->rRotAngle = atof(m_propertiesTab.m_rotation);
	m_pMTextData->nAlignment = m_propertiesTab.m_justification + 1;
	strncpy(m_pMTextData->szTextStyle, m_propertiesTab.m_style.GetBuffer(0), 512);
	m_pMTextData->text = m_text;*/
	// Bugzilla No. 78394; 09-12-2002 ]

	GetWindowRect(m_mtextWinodwRect);

}

void CMTextPropSheet::OnSize(UINT nType, int cx, int cy) 
{	

	CPropertySheet::OnSize(nType, cx, cy);

	CRect propSheetRect;
	GetClientRect(propSheetRect);
	ClientToScreen(propSheetRect);

	
	CTabCtrl* tab = GetTabControl();
	if ( tab == NULL )	return;	
	CRect tabRect;
	tab->GetWindowRect(tabRect);
	
	CRect okRect;
	CWnd* pButtonOk = GetDlgItem(IDOK);
	if ( pButtonOk != NULL )
	{
		
		pButtonOk->GetWindowRect(okRect);
		okRect.top =	propSheetRect.bottom - CONTROLS_OFFSET - okRect.Height(); 	
		okRect.bottom = propSheetRect.bottom - CONTROLS_OFFSET;
		ScreenToClient(okRect);
		pButtonOk->MoveWindow(okRect, TRUE);
	}

	
	CWnd* pButtonCancel = GetDlgItem(IDCANCEL);
	if ( pButtonCancel != NULL )
	{
		CRect cancelRect;
		pButtonCancel->GetWindowRect(cancelRect);
		cancelRect.top =	propSheetRect.bottom - CONTROLS_OFFSET - cancelRect.Height(); 	
		cancelRect.bottom = propSheetRect.bottom - CONTROLS_OFFSET;
		ScreenToClient(cancelRect);
		pButtonCancel->MoveWindow(cancelRect, TRUE);
	}

	if ( m_help.m_hWnd != NULL )
	{
		CRect helpRect;	
		m_help.GetWindowRect(helpRect);
		helpRect.top =	propSheetRect.bottom - CONTROLS_OFFSET - helpRect.Height();
		helpRect.bottom = propSheetRect.bottom - CONTROLS_OFFSET;		
		ScreenToClient(helpRect);
		m_help.MoveWindow(helpRect, TRUE);
	}
	
	if ( m_edit.m_hWnd != NULL )
	{
		CRect editRect;
		editRect.top	= tabRect.bottom + CONTROLS_OFFSET;
		editRect.left	= tabRect.left;
		editRect.right	= propSheetRect.right - CONTROLS_OFFSET;
		editRect.bottom = propSheetRect.bottom - 2 * CONTROLS_OFFSET - okRect.Height() ; 
		ScreenToClient(editRect);	
		m_edit.MoveWindow(editRect);
	}
}

void CMTextPropSheet::OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI )
{
	lpMMI->ptMinTrackSize.x = MIN_MTEXT_DIALOG_WIDTH;
	lpMMI->ptMinTrackSize.y = MIN_MTEXT_DIALOG_HEIGHT;         
}
