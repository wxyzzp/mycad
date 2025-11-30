/* D:\ICADDEV\PRJ\ICAD\MTEXTTEXTTAB.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

// CMTextTextTab.cpp : implementation file
//

#include "icad.h"
#include "icadapi.h"
#include "MTextTextTab.h"
#include "MTextPropSheet.h"
#include "gr.h"
#include "DoFont.h"
#include <algorithm>
// Bugzilla No. 78034; 23-12-2002 
#define DEFAULT_TEXT_HEIGHT 0.2
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMTextTextTab property page

IMPLEMENT_DYNCREATE(CMTextTextTab, CPropertyPage)

CString CMTextTextTab::m_sLastZoom;

CMTextTextTab::CMTextTextTab() : CPropertyPage(CMTextTextTab::IDD)
{
	m_prevZoom = 1.;
	//{{AFX_DATA_INIT(CMTextTextTab)
	//}}AFX_DATA_INIT
}

CMTextTextTab::~CMTextTextTab()
{
}

void CMTextTextTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMTextTextTab)
	DDX_Control(pDX, IDC_FONT, m_font);
	DDX_Control(pDX, IDC_BOLD, m_bold);
	DDX_Control(pDX, IDC_UNDO, m_undo);
	DDX_Control(pDX, IDC_UNDERLINE, m_underline);
	DDX_Control(pDX, IDC_ITALIC, m_italic);
	DDX_Control(pDX, IDC_FONTSIZE, m_fontsize);
	DDX_Control(pDX, IDC_CBOX_ZOOM, m_zoomCBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMTextTextTab, CPropertyPage)
	//{{AFX_MSG_MAP(CMTextTextTab)
	ON_BN_CLICKED(IDC_BOLD, OnBold)
	ON_BN_CLICKED(IDC_ITALIC, OnItalic)
	ON_BN_CLICKED(IDC_UNDERLINE, OnUnderline)
	ON_CBN_SELENDOK(IDC_FONT, OnSelendokFont)
	ON_BN_CLICKED(IDC_UNDO, OnUndo)
	ON_CBN_KILLFOCUS(IDC_FONTSIZE, OnKillfocusFontsize)
	// Bugzilla No. 78034; 23-12-2002 
	//ON_CBN_SELENDOK(IDC_FONTSIZE, OnSelendokFontsize)
	ON_CBN_SETFOCUS(IDC_FONTSIZE, OnSetfocusFontsize)
	ON_CBN_KILLFOCUS(IDC_CBOX_ZOOM, OnKillfocusZoom)
	ON_CBN_SELCHANGE(IDC_CBOX_ZOOM, OnSelchangeZoom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMTextTextTab message handlers


BOOL CMTextTextTab::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	
	m_bold.SetIcon(AfxGetApp()->LoadIcon(IDI_TEXT_BOLD));
	m_italic.SetIcon(AfxGetApp()->LoadIcon(IDI_TEXT_ITALIC));
	m_underline.SetIcon(AfxGetApp()->LoadIcon(IDI_TEXT_UNDERLINE));
	m_undo.SetIcon(AfxGetApp()->LoadIcon(IDI_TEXT_UNDO));
	m_undo.EnableWindow(FALSE);
	
	m_font.FillFontComboBox(GET_FONT_SHX | GET_FONT_TTF);
	// Bugzilla No. 78034; 23-12-2002 [
	// 0.2 is added in combo box through through dialog resource
	// so capture the corresponding height in twips from here.
	long height;
	ConvertTextHeightToTwips(DEFAULT_TEXT_HEIGHT, height);
	// 0.2 is added at the index zero ('0').
	m_fontsize.SetItemData(0,height);
	// Bugzilla No. 78034; 23-12-2002 ]

	m_zoomCBox.LimitText(10);
	if(m_sLastZoom.IsEmpty())
		m_zoomCBox.SetCurSel(4); // "100%"
	else
		m_zoomCBox.SetWindowText(m_sLastZoom);
	CString text;
	m_zoomCBox.GetWindowText(text);
	m_minZoom = m_prevZoom = atof(text)/100.;
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool CMTextTextTab::init(SDS_mTextData* pMTextData)
{
	// DP: values of this tab inited from text style
	return true;
}

void CMTextTextTab::OnBold()
{
	GetRichEdit()->SetFocus();
	CHARFORMAT cf;
	GetRichEdit()->GetSelectionCharFormat(cf);
	if (!(cf.dwMask & CFM_BOLD) || !(cf.dwEffects & CFE_BOLD))
		cf.dwEffects = CFE_BOLD;
	else
		cf.dwEffects = 0;
	cf.dwMask = CFM_BOLD;
	GetRichEdit()->SetSelectionCharFormat(cf);
	((CMTextPropSheet*)GetParent())->UpdateChar();
}

void CMTextTextTab::OnItalic()
{
	GetRichEdit()->SetFocus();
	CHARFORMAT cf;
	GetRichEdit()->GetSelectionCharFormat(cf);
	if (!(cf.dwMask & CFM_ITALIC) || !(cf.dwEffects & CFE_ITALIC))
		cf.dwEffects = CFE_ITALIC;
	else
		cf.dwEffects = 0;
	cf.dwMask = CFM_ITALIC;
	GetRichEdit()->SetSelectionCharFormat(cf);
	((CMTextPropSheet*)GetParent())->UpdateChar();
}

void CMTextTextTab::OnUnderline()
{
	GetRichEdit()->SetFocus();
	CHARFORMAT cf;
	GetRichEdit()->GetSelectionCharFormat(cf);
	if (!(cf.dwMask & CFM_UNDERLINE) || !(cf.dwEffects & CFE_UNDERLINE))
		cf.dwEffects = CFE_UNDERLINE;
	else
		cf.dwEffects = 0;
	cf.dwMask = CFM_UNDERLINE;
	GetRichEdit()->SetSelectionCharFormat(cf);
	((CMTextPropSheet*)GetParent())->UpdateChar();
}

CFont& CMTextTextTab::SelectedFont()
{
	FontItem* pSelectedFont = m_font.GetFontItem();
	if(pSelectedFont->fontType != TRUETYPE_FONTTYPE)
		m_font.SetCurrentFont("Arial");
	
	m_editFont.Detach();
	m_editFont.CreatePointFont(100, pSelectedFont->fontName);
	return m_editFont;
}

void CMTextTextTab::OnSelendokFont()
{
	CHARFORMAT cf;
	GetRichEdit()->GetSelectionCharFormat(cf);
	cf.dwMask = CFM_FACE | CFM_BOLD | CFM_ITALIC | CFM_CHARSET;
	_tcscpy(cf.szFaceName, m_font.GetFontItem()->fontName);
	cf.bCharSet = GetSystemCharSet();				// Use system character set.
	//cf.bCharSet = m_font.GetFontItem()->fontInfo()->lfCharSet;
	
	if(m_font.GetFontType() == SHX_FONTTYPE || m_font.GetFontType() == SHP_FONTTYPE || m_font.GetFontType() == SHX_BIGFONTTYPE)
	{
		if (cf.dwEffects & CFE_BOLD)
		{
			cf.dwEffects ^= CFE_BOLD;
			m_bold.SetCheck(false);
		}
		if (cf.dwEffects & CFE_ITALIC)
		{
			cf.dwEffects ^= CFE_ITALIC;
			m_italic.SetCheck(false);
		}
	}
	GetRichEdit()->SetSelectionCharFormat(cf);
	GetRichEdit()->SetFocus();
	((CMTextPropSheet*)GetParent())->UpdateButtons();
}

void CMTextTextTab::OnUndo() 
{
	GetRichEdit()->Undo();
	GetRichEdit()->SetFocus();
}

// Bugzilla No. 78034; 23-12-2002 [
/*void CMTextTextTab::OnSelendokFontsize() 
{
	GetRichEdit()->SetFocus();
}*/
// Bugzilla No. 78034; 23-12-2002 ]

void CMTextTextTab::OnSetfocusFontsize() 
{
	GetRichEdit()->GetSel(m_begin, m_end);	
}

void CMTextTextTab::OnKillfocusFontsize() 
{
	CString text;
	m_fontsize.GetWindowText(text);
	char* pString = text.GetBuffer(0);
	if (!ic_strIsValidReal(pString, false, true, NULL)) 
	{
		MessageBox(ResourceString(IDC_MTEXTTEXTTABDIA_UNABLE, "Unable to use this value for this unit entry.  Please try another value."),
					ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD"),
					MB_OK | MB_SETFOREGROUND);
		m_fontsize.SetCurSel(0);
		m_fontsize.SetFocus();
		return;
	}
	text.ReleaseBuffer();

    double SizeFromComboBox = atof(text);
	CHARFORMAT cf;
	CString message;
	GetRichEdit()->SetSel(m_begin, m_end);
	GetRichEdit()->GetSelectionCharFormat(cf);
	if (m_end - m_begin < GetRichEdit()->GetTextLength()) 
	{
		if (SizeFromComboBox > (cf.yHeight / 125.0 + .0001))
		{
			message.Format(ResourceString(IDC_MTEXTTEXTTABDIA_TOO_LARGE, "This value is too large in relationship to the rest of the text.  The largest value you can enter is %.4lf.  You may change the text size without limits by selecting the entire text."), cf.yHeight/125.0);
			MessageBox( message,
				ResourceString(DNT_ICADAPP_INTELLICAD_1, "IntelliCAD"),
				MB_OK | MB_SETFOREGROUND);
			message.Format(_T ("%.4lf"), cf.yHeight / 125.0);
			m_fontsize.SetWindowText(message);
			m_fontsize.SetFocus();
			return;
		}
		if (SizeFromComboBox < (cf.yHeight / 4000.0 - .0001)) 
		{
			message.Format(ResourceString(IDC_MTEXTTEXTTABDIA_TOO_SMALL, "This value is too small in relationship to the rest of the text.  The smallest value you can enter is %.4lf.  You may change the text size without limits by selecting the entire text."), cf.yHeight/4000.0);
			MessageBox( message,
				ResourceString(DNT_ICADAPP_INTELLICAD_1, "IntelliCAD"),
				MB_OK | MB_SETFOREGROUND);
			message.Format(_T ("%.4lf"), cf.yHeight / 4000.0);
			m_fontsize.SetWindowText(message);
			m_fontsize.SetFocus();
			return;
		}
	}

	// Bugzilla No. 78034; 23-12-2002 [
	cf.dwMask = CFM_SIZE;
	//DP: old realization commented
 	//cf.yHeight = (int) ((size / 0.2) * 200 + 0.5); // .2 is default fontsize in the combo box.  200 is
	ConvertTextHeightToTwips(SizeFromComboBox, cf.yHeight); // the actual default fontsize, plus .5 to 
															// offset a small rounding error.
	message.Format (_T ("%.4lf"), SizeFromComboBox);
	if(m_fontsize.FindStringExact(-1, message) == CB_ERR)
	{
		int index = m_fontsize.AddString (message);
		m_fontsize.SetItemData(index,cf.yHeight);
	}
	// Bugzilla No. 78034; 23-12-2002 ]
	
	GetRichEdit()->SetSelectionCharFormat(cf);
}

// Bugzilla No. 78034; 23-12-2002 [
int CMTextTextTab::GetTextHeightFromTwips(sds_real& textHeight, long twipsHeight)
{
	int nCount = m_fontsize.GetCount();
	TCHAR array[50];

	do
	{
		if(m_fontsize.GetItemData(--nCount) == twipsHeight)
		{
			if(CB_ERR == m_fontsize.GetLBText(nCount,array))
				return -1;
			textHeight = _tcstod(array,NULL);
			return nCount;
		}
	}while(nCount);

	// find the nearest value
	long _twipsHeight = twipsHeight;
	return _getTextHeightFromTwips(&textHeight, _twipsHeight);
}
// Bugzilla No. 78034; 23-12-2002 ]

int CMTextTextTab::_getTextHeightFromTwips(sds_real* pTextHeight, long &twipsHeight)
{
	// find the nearest height by twipsHeight
	TCHAR array[50];
	long nCount = m_fontsize.GetCount();
	long minDiff = MAXLONG;
	long diff;
	int index = -1;
	do
	{
		diff = labs(m_fontsize.GetItemData(--nCount) - twipsHeight);
		if(diff < minDiff && CB_ERR != m_fontsize.GetLBText(nCount,array))
		{
			minDiff = diff;
			index = nCount;
			if(pTextHeight)
				*pTextHeight = _tcstod(array,NULL);
		}
	}while(nCount);

	double inaccuracy = 0.6/m_minZoom;
	if(index != -1 && minDiff > inaccuracy)
		index = -1;
	return index;
}


void CMTextTextTab::OnKillfocusZoom()
{
	UpdateData();

	CString text;
	m_zoomCBox.GetWindowText(text);
	changeZoom(text);
}

void CMTextTextTab::OnSelchangeZoom() 
{
	UpdateData();

	CString text;
	int index = m_zoomCBox.GetCurSel();
	if(index == CB_ERR )
		return;
	m_zoomCBox.GetLBText(index, text);

	if(changeZoom(text))
	{
		m_zoomCBox.SetCurSel(index); // set it before SetFocus()
		GetRichEdit()->SetFocus();
	}
}

// returns	true - if new zoom is correct
//			false if new zoom is incorrect, 'strZoom' has right value
bool CMTextTextTab::changeZoom(CString& strZoom)
{
	CString text = strZoom;
	text.Replace("%", "");

	char* pString = text.GetBuffer(0);
	if (!ic_strIsValidReal(pString, false, true, NULL)) 
	{
		strZoom = m_sLastZoom;
		MessageBox(ResourceString(IDC_MTEXTTEXTTABDIA_UNABLE, "Unable to use this value for this unit entry.  Please try another value."),
					ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD"), MB_OK | MB_SETFOREGROUND);
		m_zoomCBox.SetFocus();
		return false;
	}
	text.ReleaseBuffer();

	double zoom = atof(text)/100.;
	ASSERT(zoom >= 0.);
	if(fabs(zoom - m_prevZoom) < IC_ZRO) // m_prevZoom != zoom 
		return true;

	long minDiff = MAXLONG, minHeight = MAXLONG;	
	if(zoom < m_prevZoom)
	{
		// 
		long height, sub;
		for(int i = 0; i < m_fontsize.GetCount(); ++i)
		{
			height = m_fontsize.GetItemData(i);
			if(height < minHeight)
				minHeight = height;
			for(int j = i+1; j < m_fontsize.GetCount(); ++j)
			{
				sub = labs(height - m_fontsize.GetItemData(j));
				if(sub < minDiff && sub != 0)
					minDiff = sub;
			}
		}
	
		if(labs(zoom*minHeight) < 2 || labs(zoom*minDiff) < 2)
		{
			// find out real used heights
			minDiff = minHeight = MAXLONG;

			BOOL isLocked = GetRichEdit()->LockWindowUpdate();
			long begin, end;
			GetRichEdit()->GetSel(begin, end);

			std::vector<long> arrayUsedHeight;
			CHARFORMAT cf;
			for(int i = GetRichEdit()->GetTextLength(); i >= 0; --i)
			{
				GetRichEdit()->SetSel(i, i);
				GetRichEdit()->GetSelectionCharFormat(cf);
				height = cf.yHeight;
				if(arrayUsedHeight.end() == std::find(arrayUsedHeight.begin(), arrayUsedHeight.end(), height))
					arrayUsedHeight.push_back(height);
			}

			GetRichEdit()->SetSel(begin, end);	// restore selection
			if(isLocked)
				GetRichEdit()->UnlockWindowUpdate();
			
			// get exact heights
			for(i = 0; i < arrayUsedHeight.size(); ++i)
				_getTextHeightFromTwips(NULL, arrayUsedHeight[i]);

			for(i = 0; i < arrayUsedHeight.size(); ++i)
			{
				height = arrayUsedHeight[i];
				if(height < minHeight)
					minHeight = height;
				for(int j = i+1; j < arrayUsedHeight.size(); ++j)
				{
					sub = labs(height - arrayUsedHeight[j]);
					if(sub < minDiff && sub != 0)
						minDiff = sub;
				}
			}
		}
	}

	// zoom should not be very small. Because characters with different source height will be same height.
	const double maxZoom = 1000.;
	if(labs(zoom*minHeight) < 2 || labs(zoom*minDiff) < 2 || zoom > maxZoom)
	{
		MessageBox(ResourceString(IDC_MTEXTTEXTTABDIA_UNABLE, "Unable to use this value for this unit entry.  Please try another value."),
					ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD"), MB_OK | MB_SETFOREGROUND);

		double newZoom;
		if(zoom > maxZoom)
			newZoom = maxZoom;
		else
			newZoom = 3./(minHeight > minDiff ? minDiff : minHeight);
		
		CString str;
		str.Format("%.3f%%", newZoom*100.);
		strZoom = str;
		m_zoomCBox.SetWindowText(str);
		m_zoomCBox.SetFocus();
		return false;
	}

	((CMTextPropSheet*)GetParent())->scaleText(zoom/m_prevZoom);
	m_prevZoom = zoom;
	m_sLastZoom = strZoom;

	if(zoom < m_minZoom)
		m_minZoom = zoom;

	return true;
}
