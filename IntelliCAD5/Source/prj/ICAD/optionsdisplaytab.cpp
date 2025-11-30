/* F:\ICADDEV\PRJ\ICAD\OPTIONSDISPLAYTAB.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// OptionsDisplayTab.cpp : implementation file
//

#include "icad.h"
#include "IcadApi.h"
#include "OptionsDisplayTab.h"
#include "optionsfuncts.h"
#include "Preferences.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OptionsDisplayTab property page

IMPLEMENT_DYNCREATE(OptionsDisplayTab, CPropertyPage)

OptionsDisplayTab::OptionsDisplayTab() : CPropertyPage(OptionsDisplayTab::IDD)
{
	//{{AFX_DATA_INIT(OptionsDisplayTab)
	m_bContinuousMotion = UserPreference::ContinuousMotionPreference;
	//}}AFX_DATA_INIT

	resbuf rb;

	sds_getvar("SCRLHIST", &rb);
	iScrlHist = rb.resval.rint;

	sds_getvar("BKGCOLOR", &rb);
	iBkgColor = rb.resval.rint;

	sds_getvar("PROMPTMENU", &rb);
	bPromptMenu = rb.resval.rint;
	
	sds_getvar("AUTOMENULOAD", &rb);
	bAutoMenuLoad = rb.resval.rint;

	bBkgFlag = false;
}

OptionsDisplayTab::~OptionsDisplayTab()
{
}

void OptionsDisplayTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OptionsDisplayTab)
	DDX_Text(pDX, OPTION_CMD_EDIT, iScrlHist);
	DDV_MinMaxInt(pDX, iScrlHist, 1, 500);
	DDX_Check(pDX, OPTION_PROMPTBOX, bPromptMenu);
	DDX_Check(pDX, OPTION_MENULOAD, bAutoMenuLoad);
	DDX_Check(pDX, OPTION_INERTIA, m_bContinuousMotion);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OptionsDisplayTab, CPropertyPage)
	//{{AFX_MSG_MAP(OptionsDisplayTab)
	ON_NOTIFY(UDN_DELTAPOS, OPTION_CMD_SPIN, OnCmdSpin)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(OPTION_SCREEN_COLOR, OnScreenColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OptionsDisplayTab message handlers

void OptionsDisplayTab::OnCmdSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

    UpdateData(TRUE);
    iScrlHist+=(-pNMUpDown->iDelta);

    if (iScrlHist < 1) iScrlHist = 1;
	if (iScrlHist > 500) iScrlHist = 500;

    UpdateData(FALSE);
	*pResult = 0;
}

void OptionsDisplayTab::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	switch(nIDCtl)
	{
	case OPTION_SCREEN_COLOR:
		{
			DrawColorButton(lpDrawItemStruct, this, OPTION_SCREEN_COLOR, iBkgColor, true);
		}
		break;

	default:
		CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
	}
}


void OptionsDisplayTab::OnScreenColor() 
{
    SDS_GetColorDialog(iBkgColor,&iBkgColor,1);

	//Update color swatch
	PaintColorSwatch(this, OPTION_SCREEN_COLOR, iBkgColor, true);

    bBkgFlag=true;
}

void OptionsDisplayTab::OnOK() 
{
	UserPreference::ContinuousMotionPreference = !!m_bContinuousMotion;

	resbuf rb;

	rb.restype = RTSHORT;

	rb.resval.rint = iScrlHist;
	sds_setvar("SCRLHIST", &rb);

	rb.resval.rint = iBkgColor;
	sds_setvar("BKGCOLOR", &rb);

	if (bPromptMenu)
		rb.resval.rint = 1;
	else 
		rb.resval.rint = 0;
	sds_setvar("PROMPTMENU", &rb);

	if (bAutoMenuLoad)
		rb.resval.rint = 1;
	else
		rb.resval.rint = 0;
	sds_setvar("AUTOMENULOAD", &rb);		

	CPropertyPage::OnOK();
}


