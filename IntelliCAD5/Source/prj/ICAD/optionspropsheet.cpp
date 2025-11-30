/* F:\ICADDEV\PRJ\ICAD\OPTIONSPROPSHEET.CPP  
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// OptionsPropSheet.cpp : implementation file
//

#include "icad.h"
#include "IcadApi.h"
#include "OptionsPropSheet.h"
#include "resource.hm"
#include "paths.h"
#include "configure.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int SDS_VarsToReg();


/////////////////////////////////////////////////////////////////////////////
// CMainWindow
void CMainWindow::IcadConfigDia(void) 
	{
    int ret;

	OptionsPropSheet optionsDlg;
	ret = optionsDlg.DoModal();

    if(ret==IDOK)
		{
        if(optionsDlg.m_xhairTab.bCurFlag && ! optionsDlg.m_displayTab.bBkgFlag) 
			{
			::ExecuteUIAction( ICAD_WNDACTION_REDRAWALLWIN );
	        }

		SDS_VarsToReg();
	    }
	}


/////////////////////////////////////////////////////////////////////////////
// OptionsPropSheet

IMPLEMENT_DYNAMIC(OptionsPropSheet, CPropertySheet)

OptionsPropSheet::OptionsPropSheet(CWnd* pParentWnd) : IcadPropertySheet( IDS_OPTIONSPROP, pParentWnd)
{
	AddPage(&m_generalTab);
	AddPage(&m_pathTab);
	AddPage(&m_displayTab);
	AddPage(&m_xhairTab);
	
#ifdef BUILD_WITH_FLYOVERSNAP_SUPPORT
	AddPage(&m_snappingTab);
#endif
}

OptionsPropSheet::~OptionsPropSheet()
{
}


BEGIN_MESSAGE_MAP(OptionsPropSheet, IcadPropertySheet)
	//{{AFX_MSG_MAP(OptionsPropSheet)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OptionsPropSheet message handlers

BOOL OptionsPropSheet::OnInitDialog() 
{
	BOOL bResult = IcadPropertySheet::OnInitDialog();
	IcadPropertySheet::AddHelpButton();

	//Make sure apply button is not visible
	CWnd *applyWnd = GetDlgItem(ID_APPLY_NOW);
	applyWnd->ShowWindow(FALSE);

	return bResult;
}

void OptionsPropSheet::OnHelp() 
{
	CommandRefHelp( m_hWnd, HLP_CONFIG);
}

BOOL OptionsPropSheet::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	
	return CPropertySheet::OnHelpInfo(pHelpInfo);
}

BOOL OptionsPropSheet::setActivePage( UINT nID )
{
	int idx = 0;
	switch( nID )
	{
	case OPTIONS_GENERAL:
		idx = 0;
		break;
	case OPTIONS_PATHS:
		idx = 1;
		break;
	case OPTIONS_DISPLAY:
		idx = 2;
		break;
	case OPTIONS_CROSSHAIRS:
		idx = 3;
		break;
	case OPTIONS_SNAPPING:
		idx = 4;
		break;
	default:
		break;
	}

	return SetActivePage( idx );
}
