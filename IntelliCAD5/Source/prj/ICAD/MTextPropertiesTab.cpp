/* D:\ICADDEV\PRJ\ICAD\CMTextPropertiesTab.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 *
 * Abstract
 *
 * CMTextPropertiesTab.cpp : implementation file
 *
 */

#include "icad.h"
#include "icadapi.h"
#include "MTextPropertiesTab.h"
#include "MTextTextTab.h"
#include "MTextPropSheet.h"
#include "gr.h"
#include "DoFont.h"
#include "truetypeutils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMTextPropertiesTab property page

IMPLEMENT_DYNCREATE(CMTextPropertiesTab, CPropertyPage)

CMTextPropertiesTab::CMTextPropertiesTab() : CPropertyPage(CMTextPropertiesTab::IDD)
{
	//{{AFX_DATA_INIT(CMTextPropertiesTab)
	m_rotation = _T("");
	//Bugzilla No. 78396; 23-12-2002
	//m_style = _T("");
	m_width = _T("");
	m_justification = 0;
	//}}AFX_DATA_INIT
	::PostMessage(m_hWnd, WM_INITDIALOG, 0, 0);
}

CMTextPropertiesTab::~CMTextPropertiesTab()
{
}

void CMTextPropertiesTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMTextPropertiesTab)
	DDX_Control(pDX, IDC_STYLE, m_cstyle);
	DDX_Control(pDX, IDC_WIDTH, m_cwidth);
	DDX_Control(pDX, IDC_ROTATION, m_crotate);
	DDX_Control(pDX, IDC_JUSTIFICATION, m_cjust);
	DDX_CBString(pDX, IDC_ROTATION, m_rotation);
	//Bugzilla No. 78396; 23-12-2002
	//DDX_CBString(pDX, IDC_STYLE, m_style);
	DDX_CBString(pDX, IDC_WIDTH, m_width);
	DDX_CBIndex(pDX, IDC_JUSTIFICATION, m_justification);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMTextPropertiesTab, CPropertyPage)
	//{{AFX_MSG_MAP(CMTextPropertiesTab)
	ON_CBN_SELENDOK(IDC_JUSTIFICATION, OnSelendokJustification)
	ON_CBN_SELENDOK(IDC_ROTATION, OnSelendokRotation)
	ON_CBN_SELENDOK(IDC_STYLE, OnSelendokStyle)
	ON_CBN_SELENDOK(IDC_WIDTH, OnSelendokWidth)
	ON_CBN_KILLFOCUS(IDC_ROTATION, OnKillfocusRotation)
	ON_CBN_KILLFOCUS(IDC_WIDTH, OnKillfocusWidth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CMTextPropertiesTab::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	FillCombo();
	m_cstyle.FillStyleComboBox();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool CMTextPropertiesTab::init(SDS_mTextData* pMTextData)
{
	CString string = pMTextData->szTextStyle;
	if(!string.IsEmpty())
	{
		int comboIndex = m_cstyle.FindStringExact(-1, string);
		if(CB_ERR == comboIndex)
			m_cstyle.SetCurSel(0);
		else
			m_cstyle.SetCurSel(comboIndex);
	}

	if(icadRealEqual(pMTextData->rBoxWidth,0.0))
	{
		int comboIndex = m_cwidth.FindStringExact(-1, _T("(no wrap)"));
		if(comboIndex != CB_ERR)
			m_cwidth.SetCurSel(comboIndex);
		m_width = _T("(no wrap)");
	}
	else
	{
		string.Format(_T ("%.4lf"), pMTextData->rBoxWidth);
		m_cwidth.SetCurSel(m_cwidth.AddString(string));
		m_width = string;
	}
	
	//Modified Cybage AW 12-05-01 [
	//For adding and selecting the string which is not in the list
	//of the combobox for selecting the text angle
	string.Format(_T ("%.0lf"), pMTextData->rRotAngle);
	int index = m_crotate.FindStringExact(-1, string);
	if(index != CB_ERR)
		m_crotate.SetCurSel(index);
	else
	{
		bool bFlag = false;
		for(int i = 0; i < m_crotate.GetCount(); i++)
		{
			CString str;
			m_crotate.GetLBText(i, str);
			if(atoi(string) > atoi(str))
				continue;
			m_crotate.InsertString(i, string);
			bFlag=true;
			break;
		}
		if(!bFlag)
			m_crotate.InsertString(m_crotate.GetCount(), string);
		
		index = i;
		m_crotate.SetCurSel(index);
	}
	//m_crotate.SetCurSel(0);
	//Modified Cybage AW 12-05-01 ]
	string.Format("%.0lf", pMTextData->rRotAngle);
	m_rotation = string;

	m_justification = pMTextData->nAlignment - 1;
	m_cjust.SetCurSel(m_justification);
	UpdateJustification();
	
	UpdateStyle();
	return true;
}

void CMTextPropertiesTab::FillCombo()
{
	CComboBox *pCombo = NULL;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(IDC_JUSTIFICATION)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_IDC_JUSTIFICATION_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_IDC_JUSTIFICATION_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_IDC_JUSTIFICATION_3);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_IDC_JUSTIFICATION_4);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_IDC_JUSTIFICATION_5);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_IDC_JUSTIFICATION_6);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_IDC_JUSTIFICATION_7);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_IDC_JUSTIFICATION_8);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_IDC_JUSTIFICATION_9);
	pCombo->AddString(comboOption);

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(IDC_WIDTH)))
		return;

	pCombo->ResetContent();
	comboOption.LoadString(IDC_COMBO_IDC_WIDTH_1);
	pCombo->AddString(comboOption);
}

void CMTextPropertiesTab::OnSelendokJustification()
{
	UpdateJustification();
}

void CMTextPropertiesTab::UpdateJustification()
{
	GetRichEdit()->SetSel(0, GetRichEdit()->GetTextLength());
	PARAFORMAT pf;
	GetRichEdit()->GetParaFormat(pf);
	pf.dwMask = PFM_ALIGNMENT;
	switch(m_cjust.GetCurSel())
	{
	case 0:
	case 3:
	case 6:
		pf.wAlignment = PFA_LEFT;
		break;
	case 1:
	case 4:
	case 7:
		pf.wAlignment = PFA_CENTER;
		break;
	case 2:
	case 5:
	case 8:
		pf.wAlignment = PFA_RIGHT;
		break;
	};
	GetRichEdit()->SetParaFormat(pf);
	GetRichEdit()->SetSel(GetRichEdit()->GetTextLength(), GetRichEdit()->GetTextLength());
	GetRichEdit()->SetFocus();
}

void CMTextPropertiesTab::OnSelendokRotation()
{
	GetRichEdit()->SetFocus();
}

void CMTextPropertiesTab::OnKillfocusRotation()
{
	CString text;
	m_crotate.GetWindowText(text);
	char* pString = text.GetBuffer(0);
	if (!ic_strIsValidReal(pString, true, true, NULL))
	{
		MessageBox(ResourceString(IDC_MTEXTTEXTTABDIA_UNABLE, "Unable to use this value for this unit entry.  Please try another value."),
					ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD"),
					MB_OK | MB_SETFOREGROUND);
		
		m_crotate.SetCurSel(0);
		m_crotate.SetFocus();
	}
	text.ReleaseBuffer();
}

void CMTextPropertiesTab::OnSelendokStyle()
{
	UpdateStyle();
}

// If a file refers to a fontname that is not listed on the user's computer, this function is
// called to temporarily add it to the fontname list.
void CMTextPropertiesTab::UpdateStyle()
{
	m_cstyle.GetTextStyle();
	CString fontName = m_cstyle.m_tableInfo.GetFontFilename(false);
	if(fontName.IsEmpty())
		fontName = m_cstyle.m_tableInfo.GetFontFamily();

	if(((CMTextPropSheet*)GetParent())->GetFontCombo()->FindStringExact(-1, fontName) != CB_ERR)
		((CMTextPropSheet*)GetParent())->GetFontCombo()->SetCurrentFont(fontName);
	else
	{
		int itemIndex;
		if(fontName.Right(4) == ".shx" || fontName.Right(4) == ".SHX")
			itemIndex = ((CMTextPropSheet*)GetParent())->GetFontCombo()->AddFont(NULL, SHX_FONTTYPE, (char*)(LPCTSTR)fontName);
		else
			if(fontName.Right(4) == ".shp" || fontName.Right(4) == ".SHP")
				itemIndex = ((CMTextPropSheet*)GetParent())->GetFontCombo()->AddFont(NULL, SHP_FONTTYPE, (char*)(LPCTSTR)fontName);
			else
			{
				LOGFONT logFont;
				logFont.lfCharSet = DEFAULT_CHARSET;
				strcpy(logFont.lfFaceName, m_cstyle.m_tableInfo.GetFontFamily());
				logFont.lfPitchAndFamily = 0;
				itemIndex = ((CMTextPropSheet*)GetParent())->GetFontCombo()->AddFont(&logFont, TRUETYPE_FONTTYPE, (char*)(LPCTSTR)m_cstyle.m_tableInfo.GetFontFamily());
			}
		((CMTextPropSheet*)GetParent())->GetFontCombo()->SetCurSel(itemIndex);
	}

	CHARFORMAT cf;
	GetRichEdit()->SetSel(0, GetRichEdit()->GetTextLength());
	GetRichEdit()->GetSelectionCharFormat(cf);
	_tcscpy(cf.szFaceName, ((CMTextPropSheet*)GetParent())->GetFontCombo()->GetFontItem()->fontName);
	cf.dwMask = CFM_FACE | CFM_SIZE | CFM_CHARSET;	/*D.G.*/
	cf.bCharSet = GetSystemCharSet();				// Use system character set.
	//cf.bCharSet = ((CMTextPropSheet*)GetParent())->GetFontCombo()->GetFontItem()->fontInfo()->lfCharSet;
	// DP: because of stupid RICHEDIT make some steps with charset
	// DP: at first enumerate all
	if(CTTFInfoGetter::get().fillFontsInfo(cf.szFaceName, DEFAULT_CHARSET, GetDC()->GetSafeHdc()))
	{
		// DP: try to find font with charset of drawing
		for(int i = CTTFInfoGetter::get().fontCount(); i--; )
			if(CTTFInfoGetter::get().fontInfo(i).elfLogFont.lfCharSet == GetDWGCharSet(SDS_CURDWG))
			{
				cf.bCharSet = GetDWGCharSet(SDS_CURDWG);
				break;
			}
	}
	
	struct resbuf rb;
    SDS_getvar(NULL,DB_QTEXTSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	double fontHeight = m_cstyle.m_tableInfo.GetHeight();
	if (fontHeight <= 0.0009)
		fontHeight = rb.resval.rreal;	//Default fontheight
	// Bugzilla No. 78034; 23-12-2002 
	//cf.yHeight = (int) ((fontHeight / 0.2) * 200 + 0.5); 
	ConvertTextHeightToTwips(fontHeight, cf.yHeight);

	CString string;
	string.Format (_T ("%.4lf"), fontHeight);
	int index = ((CMTextPropSheet*)GetParent())->GetFontSizeCombo()->FindStringExact(-1, string);
	// Bugzilla No. 78034; 23-12-2002 [
	if (index == CB_ERR)
	{
		index = ((CMTextPropSheet*)GetParent())->GetFontSizeCombo()->AddString(string);
		((CMTextPropSheet *)GetParent())->GetFontSizeCombo()->SetItemData(index,cf.yHeight);
	}
	// Bugzilla No. 78034; 23-12-2002 ]	
	
	((CMTextPropSheet*)GetParent())->GetFontSizeCombo()->SetCurSel(index);

	GetRichEdit()->SetSelectionCharFormat(cf);
	GetRichEdit()->SetDefaultCharFormat(cf);	//to prevent a change to Arial the first run through
	GetRichEdit()->SetSel(GetRichEdit()->GetTextLength(), GetRichEdit()->GetTextLength());
	
	((CMTextPropSheet*)GetParent())->UpdateButtons();
}

void CMTextPropertiesTab::OnSelendokWidth() 
{
	GetRichEdit()->SetFocus();
}

void CMTextPropertiesTab::OnKillfocusWidth() 
{
	CString text;
	m_cwidth.GetWindowText(text);
	char* pString = "";
	pString = text.GetBuffer(0);
	if (!ic_strIsValidReal(pString, false, false, NULL) && text != _T("(no wrap)"))
	{
		MessageBox(ResourceString(IDC_MTEXTTEXTTABDIA_UNABLE, "Unable to use this value for this unit entry.  Please try another value."),
					ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD"),
					MB_OK | MB_SETFOREGROUND);
		
		m_cwidth.SetCurSel(0);
		m_cwidth.SetFocus();
		return;
	}
	//Modified Cybage AW 12-05-01 [
	//Reason : "no wrap" option in width edit box was not retained.
	/*
	CString string;
	string.Format (_T ("%.4lf"), atof(text));
	m_cwidth.SetWindowText(string);
	if (m_cwidth.FindStringExact(-1, string) == CB_ERR)
	m_cwidth.AddString (string);
	*/
	if(text != "(no wrap)")
	{
		CString string;
		string.Format (_T ("%.4lf"), atof(text));
		m_cwidth.SetWindowText(string);
		if (m_cwidth.FindStringExact(-1, string) == CB_ERR)
			m_cwidth.AddString(string);
	}
	else
		m_cwidth.SetWindowText(text);
	//Modified Cybage AW 12-05-01 ]
}

/////////////////////////////////////////////////////////////////////////////
// CMTextPropertiesTab message handlers


