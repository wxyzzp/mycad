/* D:\ICADDEV\PRJ\ICAD\PRINTDIALOG.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Print dialog
 * 
 */ 

// PrintDialog.cpp : implementation file
//

#include "icad.h"
#include "IcadApp.h"
#include "PrintDialog.h"
#include "DbLayout.h"
#include "cmdsLayout.h"
#include "checkfp.h"
#include "resource.hm"
#include "AutoDLL.h"
#include "paths.h"
#include "IcadView.h"
#include "Preferences.h"

/////////////////////////////////////////////////////////////////////////////
// PrintDialog

IMPLEMENT_DYNAMIC(CPrintDialogSheet, CPropertySheet)

CPrintDialogSheet::CPrintDialogSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_prevTab=SCALE_VIEW;
	m_Printing = FALSE; 
	m_Preview = FALSE;
	m_PrintDirect = FALSE;
	m_properties =  NULL;
	m_units = PRN_OPT_INCHES;
	m_bWasCalledFromPrintPreview= FALSE;
	m_bPrintPreviewWasPressed = FALSE;

	m_bRestorySettingsFromRegistry = FALSE;

	m_plotMgr = CreatePlotManager();

	if (m_plotMgr)
	{
		m_advancedTab.SetPlotManager(m_plotMgr);
		m_scaleViewTab.SetPlotManager(m_plotMgr);

		HRESULT result = m_plotMgr->get_PlotProperties(&m_properties);

		if (SUCCEEDED(result))
		{
			m_penMapTab.SetPlotProperties(m_properties);
			m_scaleViewTab.SetPlotProperties(m_properties);
		}
	}

	AddPage(&m_scaleViewTab);
	AddPage(&m_penMapTab);
	AddPage(&m_advancedTab); 

}

CPrintDialogSheet::~CPrintDialogSheet()
{
	m_scaleViewTab.ResetViewList();
	if ( m_properties )
		m_properties->Release();

	if (m_plotMgr)
		m_plotMgr->Release();
}

BEGIN_MESSAGE_MAP(CPrintDialogSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CPrintDialogSheet)	
	ON_WM_HELPINFO()
	ON_BN_CLICKED(PRN_PREVIEW, OnPrintPreview)	
	//ON_BN_CLICKED(PRN_APPLY, SaveRegSettings)
	ON_BN_CLICKED(PRN_APPLY, OnApplyClick)
	ON_BN_CLICKED(PRN_SETUP, OnPrintSetup)
	ON_BN_CLICKED(PRINT_HELP, OnHelp)
	ON_BN_CLICKED(IDCANCEL, OnCancel)	
	ON_BN_CLICKED(IDOK, OnOK)	
	ON_BN_CLICKED(PRN_SETDEFAULT, OnSetDefaults)
	ON_MESSAGE(ICAD_WM_LOAD, OnLoadPlotSettings)
	ON_MESSAGE(ICAD_WM_SAVE, OnSavePlotSettings)
	ON_WM_CHAR()
	ON_CBN_SELCHANGE(PRN_LOADFROM, OnSelchangeLoadFrom)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintDialogSheet message handlers
BOOL CPrintDialogSheet::GetCurrentPrintInfo(LPICPRINTINFO pPrintInfo)
{
//*** This function fills in the values of the print info structure.
	if(NULL==pPrintInfo)
	{
		return FALSE;
	}
	sds_point ptLwrLeft,ptUprRight;
	m_scaleViewTab.GetPrintWindow(pPrintInfo->pView,ptLwrLeft,ptUprRight);
	ic_ptcpy(pPrintInfo->ptLwrLeft,ptLwrLeft);
	ic_ptcpy(pPrintInfo->ptUprRight,ptUprRight);
	CDC* pDC = pPrintInfo->pDC;
	if(NULL==pDC)
	{
		//*** No need to continue without a printer device context.
		return TRUE;
	}

	//*** Get the width and height of the print area in device units.
	pPrintInfo->dPrinterWidth  =(double)pDC->GetDeviceCaps(PHYSICALWIDTH);
	pPrintInfo->dHorzRes =(double)pDC->GetDeviceCaps(HORZRES);
	pPrintInfo->dPrinterHeight =(double)pDC->GetDeviceCaps(PHYSICALHEIGHT);
	pPrintInfo->dVertRes=(double)pDC->GetDeviceCaps(VERTRES);
	//*** Give ourselves a tiny buffer from the print area.
	pPrintInfo->dAdjustedWidth = pPrintInfo->dHorzRes;
	pPrintInfo->dAdjustedWidth -= ((pPrintInfo->dPrinterWidth -
		pPrintInfo->dHorzRes)/8.0);
	pPrintInfo->dAdjustedHeight = pPrintInfo->dVertRes;
	pPrintInfo->dAdjustedHeight -= ((pPrintInfo->dPrinterHeight -
		pPrintInfo->dVertRes)/8.0);
	//*** Get the header and footer adjustment values.
	double dPixPerInchY = (double)pDC->GetDeviceCaps(LOGPIXELSY);
	if(!m_advancedTab.m_Header.IsEmpty())
	{
		//*** If there is a Header, add 1/4" space for it.
		pPrintInfo->dHeaderAdjust=(dPixPerInchY/4.0);
	}
	if(!m_advancedTab.m_Footer.IsEmpty())
	{
		//*** If there is a Footer, add 1/4" space for it.
		pPrintInfo->dFooterAdjust=(dPixPerInchY/4.0);
	}

									// make sure driver hasn't reset it
	CheckFPControlWord();

	//*** Calculate the viewsize.
	if(m_scaleViewTab.m_FitMode>0)
	{
		//*** Fit the paper to the size of the page.
		double cx = fabs(ptUprRight[0]-ptLwrLeft[0]);
		double cy = fabs(ptUprRight[1]-ptLwrLeft[1]);
		if((cx/cy) > (pPrintInfo->dAdjustedWidth/pPrintInfo->dAdjustedHeight))
		{
			pPrintInfo->dViewSize = (cx * pPrintInfo->dAdjustedHeight /
				pPrintInfo->dAdjustedWidth);
		}
		else
		{
			double dHeightAdjustDU =
				(cy / pPrintInfo->dAdjustedHeight * (pPrintInfo->dVertRes -
				pPrintInfo->dAdjustedHeight));
			double dHeaderAdjustDU =
				(cy / pPrintInfo->dAdjustedHeight * pPrintInfo->dHeaderAdjust);
			double dFooterAdjustDU =
				(cy / pPrintInfo->dAdjustedHeight * pPrintInfo->dFooterAdjust);
			pPrintInfo->dViewSize=(cy+dHeaderAdjustDU+dFooterAdjustDU+dHeightAdjustDU);
		}
	}
	else
	{
		//*** Figure out the view size based on the user defined scale.
		double dInchesY = (pPrintInfo->dAdjustedHeight/dPixPerInchY);
		pPrintInfo->dViewSize=(m_scaleViewTab.m_drawUnitScale / m_scaleViewTab.m_printUnitScale * dInchesY);
		if(m_scaleViewTab.m_Scale!=PRN_OPT_INCHES)
		{
			pPrintInfo->dViewSize*=25.4; //*** Convert inches to millimeters.
		}
	}
	pPrintInfo->dPixPerDU = (pPrintInfo->dAdjustedHeight/pPrintInfo->dViewSize);
	return TRUE;
}


IIcadPlotProperties* 
CPrintDialogSheet:: GetPlotProperties()
{
	//Probably don't need this since called when settings have already been saved. I don't want to 
	//take it out at this late date though - just in case.
	OnSavePlotSettings();

	if ( !m_properties )
	{
		ASSERT(false);
		m_properties = CreatePlotProperties();
	}

	return m_properties;
}


IIcadPlotManager* CPrintDialogSheet::GetPlotManager()
{
	// Don't want to save plot mananger info here because this is called outside of the dialog where 
	// properties have already been saved.
	if (!m_plotMgr)
	{
		ASSERT(false);
		m_plotMgr = CreatePlotManager();
	}

	return m_plotMgr;
}

BOOL CPrintDialogSheet::SetDefaults(BOOL bDisplayWarning)
{
	if(bDisplayWarning)
	{
		UpdateData(TRUE);

		if(MessageBox(ResourceString(IDC_ICADPRINTDIA_DO_YOU_WANT_0, "Do you want to change back to the default Print settings?" ),
			ResourceString(IDC_ICADPRINTDIA_RESET_PRINT_1, "Reset Print Settings" ),MB_ICONQUESTION | MB_YESNO)==IDNO)
		{
			return FALSE;
		}
	}

	return TRUE;
}


void CPrintDialogSheet::DoEndDialog(int nResult)
{
	SaveRegSettings();

    UpdateData(TRUE);

	if(SDS_CMainWindow->GetIcadView())
	{
		SDS_CMainWindow->GetIcadView()->RestorePrintViewInfo();
	}

	CString str;
	sds_real rDist;
	if(RTNORM!=sds_distof(m_advancedTab.m_OriginX,-1,&rDist))
	{
		str.Format(ResourceString(IDC_ICADPRINTDIA__IS_NOT_A__16, "\"%s\" is not a valid X value.\nPlease enter a valid value before continuing." ),m_advancedTab.m_OriginX);
		MessageBox(str,ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),MB_OK | MB_ICONEXCLAMATION);
		//*** Set focus to the From X edit box
		CEdit* pEdit = (CEdit*)GetDlgItem(PRN_ORIGIN_X);
		if(pEdit!=NULL)
		{
			pEdit->SetFocus();
		}
		return;
	}
	if(RTNORM!=sds_distof(m_advancedTab.m_OriginY,-1,&rDist))
	{
		str.Format(ResourceString(IDC_ICADPRINTDIA__IS_NOT_A__17, "\"%s\" is not a valid Y value.\nPlease enter a valid value before continuing." ),m_advancedTab.m_OriginY);
		MessageBox(str,ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),MB_OK | MB_ICONEXCLAMATION);
		//*** Set focus to the From Y edit box
		CEdit* pEdit = (CEdit*)GetDlgItem(PRN_ORIGIN_Y);
		if(pEdit!=NULL)
		{
			pEdit->SetFocus();
		}
		return;
	}

	if(0 == m_scaleViewTab.m_FitMode)
	{
		int nUnit; //*** Uh huh-huh huh-huh huh-huh
		BOOL bWorked;
		for(nUnit=(-1),bWorked=FALSE; nUnit<=5; nUnit++)
		{
			if(RTNORM==sds_distof(m_scaleViewTab.m_DrawingUnits,nUnit,&m_scaleViewTab.m_drawUnitScale))
			{
				bWorked=TRUE;
				break;
			}
		}
		if(!bWorked)
		{
			str.Format(ResourceString(IDC_ICADPRINTDIA__IS_NOT_A__18, "\"%s\" is not a valid value for drawing units.\nPlease enter a valid value before continuing." ),m_scaleViewTab.m_DrawingUnits);
			MessageBox(str,ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),MB_OK | MB_ICONEXCLAMATION);
			//*** Set focus to the From X edit box
			CEdit* pEdit = (CEdit*)GetDlgItem(PRN_PLOTUNITS);
			if(pEdit!=NULL)
			{
				pEdit->SetFocus();
			}
			return;
		}
		for(nUnit=(-1),bWorked=FALSE; nUnit<=5; nUnit++)
		{
			if(RTNORM==sds_distof(m_scaleViewTab.m_PrintedUnits,nUnit,&m_scaleViewTab.m_printUnitScale))
			{
				bWorked=TRUE;
				break;
			}
		}
		if(!bWorked)
		{
			str.Format(ResourceString(IDC_ICADPRINTDIA__IS_NOT_A__12, "\"%s\" is not a valid value for printed units.\nPlease enter a valid value before continuing." ),m_scaleViewTab.m_PrintedUnits);
			MessageBox(str,ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),MB_OK | MB_ICONEXCLAMATION);
			//*** Set focus to the From Y edit box
			CEdit* pEdit = (CEdit*)GetDlgItem(PRN_PRINTUNITS);
			if(pEdit!=NULL)
			{
				pEdit->SetFocus();
			}
			return;
		}
	}

	if(PRN_OPT_WINDOW == m_scaleViewTab.m_View)
	{
		sds_point ptFrom,ptTo;
		//*** Convert the string from the editboxes into points.
		if(RTNORM!=sds_distof(m_scaleViewTab.m_winFromX,-1,&ptFrom[0]))
		{
			str.Format(ResourceString(IDC_ICADPRINTDIA__IS_NOT_A__16, "\"%s\" is not a valid X value.\nPlease enter a valid value before continuing." ),m_scaleViewTab.m_winFromX);
			MessageBox(str,ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),MB_OK | MB_ICONEXCLAMATION);
			//*** Set focus to the From X edit box
			CEdit* pEdit = (CEdit*)GetDlgItem(PRN_FROM_X);
			if(pEdit!=NULL)
			{
				pEdit->SetFocus();
			}
			return;
		}
		if(RTNORM!=sds_distof(m_scaleViewTab.m_winFromY,-1,&ptFrom[1]))
		{
			str.Format(ResourceString(IDC_ICADPRINTDIA__IS_NOT_A__17, "\"%s\" is not a valid Y value.\nPlease enter a valid value before continuing." ),m_scaleViewTab.m_winFromY);
			MessageBox(str,ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),MB_OK | MB_ICONEXCLAMATION);
			//*** Set focus to the From Y edit box
			CEdit* pEdit = (CEdit*)GetDlgItem(PRN_FROM_Y);
			if(pEdit!=NULL)
			{
				pEdit->SetFocus();
			}
			return;
		}
		if(RTNORM!=sds_distof(m_scaleViewTab.m_winToX,-1,&ptTo[0]))
		{
			str.Format(ResourceString(IDC_ICADPRINTDIA__IS_NOT_A__16, "\"%s\" is not a valid X value.\nPlease enter a valid value before continuing." ),m_scaleViewTab.m_winToX);
			MessageBox(str,ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),MB_OK | MB_ICONEXCLAMATION);
			//*** Set focus to the To X edit box
			CEdit* pEdit = (CEdit*)GetDlgItem(PRN_TO_X);
			if(pEdit!=NULL)
			{
				pEdit->SetFocus();
			}
			return;
		}
		if(RTNORM!=sds_distof(m_scaleViewTab.m_winToY,-1,&ptTo[1]))
		{
			str.Format(ResourceString(IDC_ICADPRINTDIA__IS_NOT_A__17, "\"%s\" is not a valid Y value.\nPlease enter a valid value before continuing." ),m_scaleViewTab.m_winToY);
			MessageBox(str,ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),MB_OK | MB_ICONEXCLAMATION);
			//*** Set focus to the To Y edit box
			CEdit* pEdit = (CEdit*)GetDlgItem(PRN_TO_Y);
			if(pEdit!=NULL)
			{
				pEdit->SetFocus();
			}
			return;
		}
		if(icadRealEqual(ptFrom[0],0.0) && icadRealEqual(ptFrom[1],0.0) &&
			icadRealEqual(ptTo[0],0.0) && icadRealEqual(ptTo[1],0.0))
		{
			MessageBox(	ResourceString(IDC_ICADPRINTDIA_PLEASE_ENT_19, "Please enter a windowed print area greater than zero." )
				,ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),MB_OK | MB_ICONEXCLAMATION);
			//*** Set focus to the From X edit box
			CEdit* pEdit = (CEdit*)GetDlgItem(PRN_FROM_X);
			if(pEdit!=NULL)
			{
				pEdit->SetFocus();
			}
			return;
		}
	}

    EndDialog(nResult);
}

void CPrintDialogSheet::SetUnits(int Units)
{
	m_units = Units;

	if (m_penMapTab)
		m_penMapTab.SetUnits(Units);
}

int CPrintDialogSheet::GetUnits()
{
	return m_units;
}


void CPrintDialogSheet::SaveRegSettings()
{
	bool bSaveToLayout = ((CButton*)GetDlgItem(PRN_SAVELAYOUT))->GetCheck() == 1 ? true : false;
	UserPreference::s_bSaveToLayout = bSaveToLayout;

	// if the tab hasn't been initialized then we don't have any info to save.
	if (m_advancedTab)
		m_advancedTab.SaveRegSettings(bSaveToLayout);

	if (m_scaleViewTab)
		m_scaleViewTab.SaveRegSettings(bSaveToLayout);

	if (m_penMapTab)
		m_penMapTab.SavePlotSettings();
}


BOOL CPrintDialogSheet::OnInitDialog()
{
	CPropertySheet::OnInitDialog();
	
	// Start moving controls on Print Property Sheet
	CFont *pFont = NULL;	
	RECT rcTab;
	RECT rc;
	CWnd  *pWnd = NULL;
	
	//Change the Font otherwise its BOLD one	
	pWnd	= GetDlgItem(ID_APPLY_NOW);
	ASSERT(pWnd);
	pFont = pWnd->GetFont();	
	ASSERT(pFont);	
	m_LayoutName.SetFont(pFont);
	m_Layout.SetFont(pFont);
	m_SaveLayout.SetFont(pFont);
	m_LayoutComboText.SetFont(pFont);
	m_LayoutCombo.SetFont(pFont);
	m_PrintPreview.SetFont(pFont);
	m_PrintSetup.SetFont(pFont);
	m_Reset.SetFont(pFont);
	m_Apply.SetFont(pFont);
	
	// resize the sheet
	GetWindowRect (&rc);
	ScreenToClient (&rc);
	rc.right += 50;
	rc.bottom += 50;
	//MoveWindow (&rc);
	
	// resize the CTabCtrl	
	CTabCtrl* pTab = GetTabControl ();
	ASSERT (pTab);
	pTab->GetWindowRect (&rc);
	ScreenToClient (&rc);
	rcTab = rc;
	rc.top += 50;     	
	rc.bottom += 50;
	pTab->MoveWindow (&rc);	
	
	// resize the page
	CPropertyPage* pPage = GetActivePage ();
	ASSERT (pPage);
	// store page size in m_PageRect
	pPage->GetWindowRect (&m_PageRect);
	ScreenToClient (&m_PageRect);
	m_PageRect.top += 50;     	
	m_PageRect.bottom += 50;
	pPage->MoveWindow (&m_PageRect);
	
		
	// Hide default Apply Button
	pWnd = GetDlgItem(ID_APPLY_NOW);
	pWnd->ShowWindow(SW_HIDE);

	// Get the correct button size by grabbing it from the apply button
	RECT applyRec;
	pWnd->GetWindowRect(&applyRec);
	ScreenToClient(&applyRec);

	// Move the apply button rec down 50 to match the change in tab size above
	applyRec.top += 50;     	
	applyRec.bottom += 50;

	pTab = GetTabControl ();
	ASSERT (pTab);
	pTab->GetWindowRect (&rcTab);	
	ScreenToClient(&rcTab);	
	rc = rcTab;
	rc.top   = rcTab.bottom + 7;
	rc.left= rcTab.left + 470;
	rc.bottom = rcTab.bottom + 30;

	// Compute the width of the tab and divide it by 6 (minus the width of the help button and 5 for the space between each button) 
	// to determine the width of each button at the bottom of the print dialog
	int tabWidth = 0, bSpace = 0, bHeight = 0, bWidth = 0;
	tabWidth = rcTab.right - rcTab.left;
	bHeight = applyRec.bottom - applyRec.top;
	bWidth = applyRec.right - applyRec.left;
	bSpace = (tabWidth - bHeight) / 6;
	applyRec.left = applyRec.right - (bSpace - 5);

	pWnd = GetDlgItem(IDCANCEL);    
	pWnd->MoveWindow(&applyRec, true);
	
	pWnd = GetDlgItem(IDOK);
	pWnd->SetWindowText(ResourceString(IDC_PRINT_DLG_PRINT,"&Print"));
	// Now move each of the other buttons the computed distance that divides the space evenly
	applyRec.left  -= bSpace;
	applyRec.right -= bSpace;
	pWnd->MoveWindow(&applyRec, true);
	

	pWnd = GetDlgItem(PRN_SETDEFAULT);
	applyRec.left  -= bSpace;
	applyRec.right -= bSpace;
	pWnd->MoveWindow(&applyRec, true);
	
	pWnd = GetDlgItem(PRN_SETUP);
	applyRec.left  -= bSpace;
	applyRec.right -= bSpace;
	pWnd->MoveWindow(&applyRec, true);
	
	pWnd = GetDlgItem(PRN_PREVIEW);        	
	applyRec.left  -= bSpace;
	applyRec.right -= bSpace;
	pWnd->MoveWindow(&applyRec, true);

	pWnd = GetDlgItem(PRN_APPLY);
	applyRec.left  -= bSpace;
	applyRec.right -= bSpace;
	pWnd->MoveWindow(&applyRec, true);

	pWnd->EnableWindow(); 
	
	CButton * pButton = (CButton * )GetDlgItem(PRINT_HELP);	 
	applyRec.left = rcTab.left;
	applyRec.right -= bSpace;
	pButton->MoveWindow(&applyRec, true);

	pButton->SetIcon( AfxGetApp()->LoadIcon(HELP_ICON) ); 

	CenterWindow();

	//cmd_setUpCTAB(); 2003/8/10 EBATECH(CNBR) Sync CTAB
	sds_name sdsCurLayoutName;
	if(!cmd_getCurrentLayout(sdsCurLayoutName))
	{
		ASSERT(FALSE);
		return FALSE;
	}
	((CEdit*)GetDlgItem(PRN_LAYOUT))->SetSel(0, -1);
	((CEdit*)GetDlgItem(PRN_LAYOUT))->ReplaceSel(((CDbLayout*)sdsCurLayoutName[0])->name(SDS_CURDWG));

	((CButton*)GetDlgItem(PRN_SAVELAYOUT))->SetCheck(UserPreference::s_bSaveToLayout ? 1 : 0);

	// initialize combo box
	m_LayoutCombo.ResetContent();
	m_LayoutCombo.AddString(ResourceString(IDS_PRN_USECURLAYOUT, "Current Layout")); // first item
	m_LayoutCombo.AddString(ResourceString(IDS_PRN_USEREGISTRY,"Previous Plot")); // second item
	m_LayoutCombo.SetCurSel(UserPreference::s_bUsePrintSettingsFromRegistry ? 1 : 0);


	m_bRestorySettingsFromRegistry = bool(UserPreference::s_bUsePrintSettingsFromRegistry)
		|| m_scaleViewTab.wasPicked() || m_advancedTab.wasPicked()
		|| m_bWasCalledFromPrintPreview && m_bPrintPreviewWasPressed;


	m_units = m_scaleViewTab.m_Scale; // set before m_penMapTab.Create()

	m_bWasCalledFromPrintPreview = m_bPrintPreviewWasPressed = FALSE;

	m_bRestorySettingsFromRegistry = bool(UserPreference::s_bUsePrintSettingsFromRegistry)
		|| m_scaleViewTab.wasPicked() || m_advancedTab.wasPicked()
		|| m_bWasCalledFromPrintPreview && m_bPrintPreviewWasPressed;

	m_scaleViewTab.SetPaperSize();	    
		
	//*** Disable the preview button if printing direct.
	::EnableWindow(::GetDlgItem(GetSafeHwnd(), PRN_PREVIEW), (!m_PrintDirect));
	/////////////////////
	
	SetActivePage(ADVANCED);
	SetActivePage(PENMAP);

	//m_prevTab = SCALE_VIEW;
	SetActivePage(m_prevTab);
	
	return TRUE;
	
}

void CPrintDialogSheet::OnOK() 
{
	//*** If printing (as opposed to setup) this gets freed in FilePrint in
	//*** CIcadView because it is too early to free it here.
	m_prevTab=SCALE_VIEW;
	if(!m_Printing)
	{
		m_scaleViewTab.RemovePrintEnts();
	}

    DoEndDialog(IDOK);
}

int CPrintDialogSheet::OnCreate(LPCREATESTRUCT lpCreateStruct ) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;

	
	//Increase window size
	RECT rc;
	GetWindowRect (&rc);
    ScreenToClient (&rc);    
    rc.bottom += 50;
    MoveWindow (&rc);



/*	m_LayoutName.Create(ResourceString(IDC_PRINT_DLG_LAYOUT_NAME, "Layout Name:"),WS_CHILD | WS_VISIBLE ,CRect(10,23,110,40), this,PRN_LAYOUTNAME);
		
	m_Layout.Create(WS_CHILD | ES_READONLY |WS_VISIBLE | WS_BORDER  ,CRect(100,20,180,40), this,PRN_LAYOUT);

	m_SaveLayout.Create(ResourceString(IDC_PRINT_DLG_SAVE_CHANGES, "Save Changes to Layout"),WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX ,CRect(190,20,365,40),this,PRN_SAVELAYOUT);
	m_SaveLayout.SetCheck(1);  	
	
	m_LayoutComboText.Create(ResourceString(IDC_PRINT_DLG_USE_SETTINGS, "Use Settings:"),WS_CHILD | WS_VISIBLE ,CRect(400,21,480,40), this,IDC_STATIC);
	m_LayoutCombo.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST,CRect(482,18,605,120), this,PRN_LOADFROM); */


	
	m_LayoutName.Create(ResourceString(IDC_PRINT_DLG_LAYOUT_NAME, "Layout Name:"),WS_CHILD | WS_VISIBLE ,CRect(10,13,110,30), this,PRN_LAYOUTNAME);
		
	m_Layout.Create(WS_CHILD | ES_READONLY |WS_VISIBLE | WS_BORDER  ,CRect(100,10,180,30), this,PRN_LAYOUT);

	m_SaveLayout.Create(ResourceString(IDC_PRINT_DLG_SAVE_CHANGES, "Save Changes to Layout"),WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX ,CRect(350,35,600,55),this,PRN_SAVELAYOUT);
	m_SaveLayout.SetCheck(1);  	
	
	m_LayoutComboText.Create(ResourceString(IDC_PRINT_DLG_USE_SETTINGS, "Use Settings:"),WS_CHILD | WS_VISIBLE ,CRect(350,11,430,30), this,IDC_STATIC);
	m_LayoutCombo.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST,CRect(432,8,550,80), this,PRN_LOADFROM);




	m_Help.Create(ResourceString(IDC_PRINT_DLG_HELP, "&Help"),WS_CHILD | WS_VISIBLE | BS_ICON,CRect(180,20,90,50), this,PRINT_HELP);

	m_Apply.Create(ResourceString(IDC_PRINT_DLG_APPLY, "Apply"),WS_CHILD | WS_VISIBLE | WS_TABSTOP,CRect(180,20,90,50), this,PRN_APPLY);
	
	m_PrintPreview.Create(ResourceString(IDC_PRINT_DLG_PRINT_PREVIEW, "Print Previe&w"),WS_CHILD | WS_VISIBLE | WS_TABSTOP,CRect(180,20,90,50), this,PRN_PREVIEW);
			
	m_PrintSetup.Create(ResourceString(IDC_PRINT_DLG_PRINT_SETUP, "Print &Setup"),WS_CHILD | WS_VISIBLE | WS_TABSTOP,CRect(180,20,90,50), this,PRN_SETUP);
				
	m_Reset.Create(ResourceString(IDC_PRINT_DLG_RESET, "&Reset"),WS_CHILD | WS_VISIBLE | WS_TABSTOP,CRect(180,20,90,50), this,PRN_SETDEFAULT);
	
	
	// return 0 to continue the creation of the window object.
	return 0;
}

void CPrintDialogSheet::OnCancel() 
{
		
	if (!m_scaleViewTab.m_bApplyWasPressed && m_scaleViewTab.m_bPreviewWasPressed)
	{
		sds_entmod(m_scaleViewTab.pLayout);
		sds_relrb(m_scaleViewTab.pLayout);
	}
	else
		sds_relrb(m_scaleViewTab.pLayout );
	
	m_scaleViewTab.m_bApplyWasPressed = false;
	m_scaleViewTab.m_bPreviewWasPressed  = false;

	m_prevTab=SCALE_VIEW;
	m_scaleViewTab.RemovePrintEnts();

    UpdateData(TRUE);
	
	if(SDS_CMainWindow->GetIcadView())
	{
		SDS_CMainWindow->GetIcadView()->RestorePrintViewInfo();
	}
    
	EndDialog(IDCANCEL);
}

BOOL CPrintDialogSheet::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();	
	return CPropertySheet::OnHelpInfo(pHelpInfo);
}

void CPrintDialogSheet::OnPrintPreview() 
{
    m_prevTab=GetPageIndex(GetActivePage());
	m_bPrintPreviewWasPressed = TRUE;
	m_scaleViewTab.m_bPreviewWasPressed = true;
	DoEndDialog(PRN_PREVIEW);	
}

void CPrintDialogSheet::OnPrintSetup() 
{
    UpdateData(TRUE);
	((CIcadApp*)AfxGetApp())->FilePrinterSetup();
	m_scaleViewTab.SetPaperSize();
}

void CPrintDialogSheet::OnHelp() 
{
	CommandRefHelp( m_hWnd, HLP_PRINT);
}

void CPrintDialogSheet::OnSetDefaults() 
{
	// User does not want to continue if SetDefaults returns FALSE
	if (!SetDefaults(TRUE))
		return;	
	m_advancedTab.SetDefaults();
	
	m_scaleViewTab.SetDefaults();
	
	m_penMapTab.SetDefaults();

	SaveRegSettings();
}


void CPrintDialogSheet::OnLoadPlotSettings()
{	
	// if the tab hasn't been initialized then the settings will be loaded on init
	if (m_advancedTab)
		m_advancedTab.LoadPlotSettings();

	if (m_scaleViewTab)
		m_scaleViewTab.LoadPlotSettings();

	if (m_penMapTab)
		m_penMapTab.LoadPlotSettings();
}


void CPrintDialogSheet::OnSavePlotSettings()
{
	// if the tab hasn't been initialized then we don't have any info to save.
	if (m_advancedTab)
		m_advancedTab.SavePlotSettings();

	if (m_scaleViewTab)
		m_scaleViewTab.SavePlotSettings();

	if (m_penMapTab)
		m_penMapTab.SavePlotSettings();
}

void CPrintDialogSheet::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    if (nChar==VK_ESCAPE)
	{
		CWnd *parent = GetParent();
		parent->SendMessage( WM_CHAR, nChar, 0);	
	}
	else
	{
		CPropertySheet::OnChar(nChar, nRepCnt, nFlags);
	}
}

void CPrintDialogSheet::OnSelchangeLoadFrom() 
{
	UpdateData(TRUE);

	int selIndex = m_LayoutCombo.GetCurSel();
	if(selIndex != CB_ERR)
	{
		UserPreference::s_bUsePrintSettingsFromRegistry = (selIndex == 1);
	    
		m_bRestorySettingsFromRegistry = bool(UserPreference::s_bUsePrintSettingsFromRegistry)
			|| m_scaleViewTab.wasPicked() || m_advancedTab.wasPicked()
			|| m_bWasCalledFromPrintPreview && m_bPrintPreviewWasPressed;

		if(m_scaleViewTab) 
			m_scaleViewTab.initSettings();

		m_units = m_scaleViewTab.m_Scale; // set before m_penMapTab.Create()

		if(m_advancedTab) 
			m_advancedTab.initSettings();

		if(m_penMapTab) 
			m_penMapTab.initSettings();

		m_scaleViewTab.SetPaperSize();
	}
}

/********************************************************************************
 * Author:	Sachin Dange.
 *
 * Purpose:	To set the flag when "Apply" button is pressed.
 *
 * Returns:	void
 ********************************************************************************/

void CPrintDialogSheet::OnApplyClick()
{
	m_scaleViewTab.m_bApplyWasPressed = true;
	SaveRegSettings();

}

