/* D:\DEV\PRJ\ICAD\ICADCUSMENUADVDLG.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// IcadCusMenuAdvDlg.cpp : implementation file
//

#include "Icad.h"/*DNT*/
#include "IcadCusMenu.h"/*DNT*/
#include "IcadToolbarMain.h"/*DNT*/
#include "IcadCusMenuAdvDlg.h"/*DNT*/
#include "paths.h"				/*DNT*/
#include "resource.hm"			/*DNT*/

#ifdef _DEBUG

	#if !defined(USE_SMARTHEAP)
	#define new DEBUG_NEW
	#endif

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIcadCusMenuAdvDlg dialog


CIcadCusMenuAdvDlg::CIcadCusMenuAdvDlg(CWnd* pParent, LPICMENUITEM lpMenuItem)
	: CDialog(CIcadCusMenuAdvDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIcadCusMenuAdvDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pMenuItem=lpMenuItem;
}


void CIcadCusMenuAdvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIcadCusMenuAdvDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIcadCusMenuAdvDlg, CDialog)
	//{{AFX_MSG_MAP(CIcadCusMenuAdvDlg)
	ON_BN_CLICKED(CUS_MENU_ADV_CHKALLENTS, OnCheckAllEnts)	
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIcadCusMenuAdvDlg message handlers

BOOL CIcadCusMenuAdvDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Modified CyberAge VSB 26/11/2001 [
	// Bug No 77925 from BugZilla
	// Desc :set the icon on the ?/Help button
	CButton *helpWnd = (CButton *) GetDlgItem(IDHELP);
	helpWnd->SetIcon(AfxGetApp()->LoadIcon(HELP_ICON));
	// Modified CyberAge VSB 26/11/2001 ]

	if(NULL==m_pMenuItem) return TRUE;

	CButton* pCheckBox;
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKBEGINNER);
	if(m_pMenuItem->lflgVisibility & ICAD_MN_BEG) pCheckBox->SetCheck(1);
	else pCheckBox->SetCheck(0);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKINTERMEDIATE);
	if(m_pMenuItem->lflgVisibility & ICAD_MN_INT) pCheckBox->SetCheck(1);
	else pCheckBox->SetCheck(0);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKADVANCED);
	if(m_pMenuItem->lflgVisibility & ICAD_MN_EXP) pCheckBox->SetCheck(1);
	else pCheckBox->SetCheck(0);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKMDIONE);
	if(m_pMenuItem->lflgVisibility & ICAD_MN_OPN) pCheckBox->SetCheck(1);
	else pCheckBox->SetCheck(0);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKMDINONE);
	if(m_pMenuItem->lflgVisibility & ICAD_MN_CLS) pCheckBox->SetCheck(1);
	else pCheckBox->SetCheck(0);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKEMBEDDED);
	if(m_pMenuItem->lflgVisibility & ICAD_MN_SEM) pCheckBox->SetCheck(1);
	else pCheckBox->SetCheck(0);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKINPLACE);
	if(m_pMenuItem->lflgVisibility & ICAD_MN_SIP) pCheckBox->SetCheck(1);
	else pCheckBox->SetCheck(0);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKCONTAINER);
	if(m_pMenuItem->lflgVisibility & ICAD_MN_CLI) pCheckBox->SetCheck(1);
	else pCheckBox->SetCheck(0);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKCONTEXTEXCLUSIVE);
	if(m_pMenuItem->lflgVisibility & ICAD_MN_RCO) pCheckBox->SetCheck(1);
	else pCheckBox->SetCheck(0);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKHIDE);
	if(m_pMenuItem->lflgVisibility & ICAD_MN_HIDE) pCheckBox->SetCheck(1);
	else pCheckBox->SetCheck(0);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKTEMPSNAP);
	if(m_pMenuItem->lflgVisibility & ICAD_MN_TOM) pCheckBox->SetCheck(1);
	else pCheckBox->SetCheck(0);

	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKALLENTS);
	if((m_pMenuItem->lflgEntVis & ICAD_MN_ENT)==ICAD_MN_ENT) //*** All Entities
	{
		pCheckBox->SetCheck(1);
		EnableAllEnts(FALSE);
	}
	else
	{
		pCheckBox->SetCheck(0); //*** Uncheck "All Entities".
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKPOINT);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_PNT) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKLINE);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_LIN) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKRAY);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_RAY) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKXLINE);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_XLN) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKARC);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_ARC) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKCIRCLE);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_CIR) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKELLIPSE);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_ELL) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKSHAPE);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_SHP) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKTRACE);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_TRC) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKSOLID);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_SLD) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHK3DFACE);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_3DF) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHK3DSOLID);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_3DS) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHK2DPOLY);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_2DP) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHK3DPOLY);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_3DP) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKPOLYFACEMESH);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_PFM) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHK3DMESH);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_3DM) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKTEXT);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_TXT) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKINSERT);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_INS) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKATTDEF);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_ATD) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKDIMENSION);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_DIM) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKLEADER);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_LDR) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKTOLERANCE);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_TOL) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKSPLINE);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_SPL) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKMTEXT);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_MTX) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKMLINE);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_MLN) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKGROUP);
		if(m_pMenuItem->lflgEntVis & ICAD_MN_GRP) pCheckBox->SetCheck(1);
		else pCheckBox->SetCheck(0);
	}
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKEXCLUDEMULT);
	if(m_pMenuItem->lflgEntVis & ICAD_MN_MUL) pCheckBox->SetCheck(1);
	else pCheckBox->SetCheck(0);

	if(!m_pMenuItem->chekVar.IsEmpty())
	{
		CEdit* pEdit = (CEdit*)GetDlgItem(CUS_MENU_ADV_EDITCHECKVAR);
		pEdit->SetWindowText(m_pMenuItem->chekVar);
	}
	if(!m_pMenuItem->grayVar.IsEmpty())
	{
		CEdit* pEdit = (CEdit*)GetDlgItem(CUS_MENU_ADV_EDITGRAYVAR);
		pEdit->SetWindowText(m_pMenuItem->grayVar);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CIcadCusMenuAdvDlg::EnableAllEnts(BOOL bEnable) 
{
	CButton* pCheckBox;

	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKPOINT);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKLINE);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKRAY);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKXLINE);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKARC);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKCIRCLE);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKELLIPSE);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKSHAPE);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKTRACE);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKSOLID);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHK3DFACE);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHK3DSOLID);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHK2DPOLY);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHK3DPOLY);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKPOLYFACEMESH);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHK3DMESH);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKTEXT);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKINSERT);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKATTDEF);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKDIMENSION);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKLEADER);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKTOLERANCE);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKSPLINE);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKMTEXT);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKMLINE);
	pCheckBox->EnableWindow(bEnable);
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKGROUP);
	pCheckBox->EnableWindow(bEnable);

	return;
}

void CIcadCusMenuAdvDlg::OnCheckAllEnts() 
{
	CButton* pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKALLENTS);
	if(pCheckBox->GetCheck()) EnableAllEnts(FALSE);
	else EnableAllEnts(TRUE);

	return;
}

void CIcadCusMenuAdvDlg::OnOK() 
{
	if(NULL==m_pMenuItem) return;

	CButton* pCheckBox;
	long	Old_lflgVisibility = m_pMenuItem->lflgVisibility,
			Old_lflgEntVis = m_pMenuItem->lflgEntVis;
	bool	NeedToUpdate = false;

	m_pMenuItem->lflgVisibility=0L;
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKBEGINNER);
	if(pCheckBox->GetCheck()) m_pMenuItem->lflgVisibility|=ICAD_MN_BEG;
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKINTERMEDIATE);
	if(pCheckBox->GetCheck()) m_pMenuItem->lflgVisibility|=ICAD_MN_INT;
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKADVANCED);
	if(pCheckBox->GetCheck()) m_pMenuItem->lflgVisibility|=ICAD_MN_EXP;
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKMDIONE);
	if(pCheckBox->GetCheck()) m_pMenuItem->lflgVisibility|=ICAD_MN_OPN;
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKMDINONE);
	if(pCheckBox->GetCheck()) m_pMenuItem->lflgVisibility|=ICAD_MN_CLS;
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKEMBEDDED);
	if(pCheckBox->GetCheck()) m_pMenuItem->lflgVisibility|=ICAD_MN_SEM;
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKINPLACE);
	if(pCheckBox->GetCheck()) m_pMenuItem->lflgVisibility|=ICAD_MN_SIP;
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKCONTAINER);
	if(pCheckBox->GetCheck()) m_pMenuItem->lflgVisibility|=ICAD_MN_CLI;
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKCONTEXTEXCLUSIVE);
	if(pCheckBox->GetCheck()) m_pMenuItem->lflgVisibility|=ICAD_MN_RCO;
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKHIDE);
	if(pCheckBox->GetCheck()) m_pMenuItem->lflgVisibility|=ICAD_MN_HIDE;
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKTEMPSNAP);
	if(pCheckBox->GetCheck()) m_pMenuItem->lflgVisibility|=ICAD_MN_TOM;

	m_pMenuItem->lflgEntVis=0L;
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKALLENTS);
	if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_ENT;
	else
	{
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKPOINT);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_PNT;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKLINE);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_LIN;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKRAY);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_RAY;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKXLINE);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_XLN;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKARC);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_ARC;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKCIRCLE);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_CIR;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKELLIPSE);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_ELL;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKSHAPE);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_SHP;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKTRACE);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_TRC;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKSOLID);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_SLD;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHK3DFACE);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_3DF;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHK3DSOLID);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_3DS;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHK2DPOLY);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_2DP;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHK3DPOLY);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_3DP;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKPOLYFACEMESH);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_PFM;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHK3DMESH);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_3DM;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKTEXT);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_TXT;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKINSERT);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_INS;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKATTDEF);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_ATD;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKDIMENSION);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_DIM;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKLEADER);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_LDR;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKTOLERANCE);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_TOL;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKSPLINE);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_SPL;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKMTEXT);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_MTX;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKMLINE);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_MLN;
		pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKGROUP);
		if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_GRP;
	}
	pCheckBox = (CButton*)GetDlgItem(CUS_MENU_ADV_CHKEXCLUDEMULT);
	if(pCheckBox->GetCheck()) m_pMenuItem->lflgEntVis|=ICAD_MN_MUL;

	if(Old_lflgVisibility != m_pMenuItem->lflgVisibility || 
	   Old_lflgEntVis != m_pMenuItem->lflgEntVis)
		NeedToUpdate = true;


	CEdit* pEdit;
	CString str;
	pEdit = (CEdit*)GetDlgItem(CUS_MENU_ADV_EDITCHECKVAR);
	pEdit->GetWindowText(str);
	str.TrimLeft(); str.TrimRight();
	if(!str.IsEmpty())
	{
		m_pMenuItem->chekVar = str;
		NeedToUpdate = true;
	}
	else	/*D.G.*/// we should clear chekVar in this case.
	{
		m_pMenuItem->chekVar.Empty();
		NeedToUpdate = true;
	}

	pEdit = (CEdit*)GetDlgItem(CUS_MENU_ADV_EDITGRAYVAR);
	pEdit->GetWindowText(str);
	str.TrimLeft(); str.TrimRight();
	if(!str.IsEmpty())
	{
		m_pMenuItem->grayVar = str;
		NeedToUpdate = true;
	}
	else	/*D.G.*/// we should clear grayVar in this case.
	{
		m_pMenuItem->grayVar.Empty();
		NeedToUpdate = true;
	}

	/*D.G.*/// It's for updating an item data from this dialog.
	if(NeedToUpdate)
	{
		CIcadCusMenu* pParentDialog = (CIcadCusMenu*)GetParent();
		if(!pParentDialog || !pParentDialog->m_pCurMenu)
			SDS_CMainWindow->GetCurrentMenu()->m_bMenuModified = TRUE;
		else
			pParentDialog->m_pCurMenu->m_bMenuModified = TRUE;
	}
	
	CDialog::OnOK();
}

// Modified CyberAge VSB 26/11/2001 [
// Bug No 77925 from BugZilla
void CIcadCusMenuAdvDlg::OnHelp()
	{

	CommandRefHelp(m_hWnd, HLP_MCOPTIONS);
	}
// Modified CyberAge VSB 26/11/2001 ]
