/* F:\ICADDEV\PRJ\ICAD\OPTIONSGENERALTAB.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// OptionsGeneralTab.cpp : implementation file
//

#include "icad.h"
#include "IcadApp.h"
#include "OptionsGeneralTab.h"
#include "LicensedComponents.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// OptionsGeneralTab property page

IMPLEMENT_DYNCREATE(OptionsGeneralTab, CPropertyPage)

OptionsGeneralTab::OptionsGeneralTab() : CPropertyPage(OptionsGeneralTab::IDD)
{
	//{{AFX_DATA_INIT(OptionsGeneralTab)
	m_autoSaveTime = 0;
	m_expLevel = -1;
	m_autoSave = FALSE;
	m_saveExt = _T("");
	m_bVbaSecurity = FALSE;
	//}}AFX_DATA_INIT

	resbuf rb;

	sds_getvar("EXPLEVEL", &rb);
	m_expLevel = rb.resval.rint - 1;

	sds_getvar("SAVETIME", &rb);
	m_autoSaveTime = rb.resval.rint;	
	m_autoSave = m_autoSaveTime > 0;

	sds_getvar("SAVEFILE", &rb);
	m_saveExt = rb.resval.rstring;
	if (rb.resval.rstring)
		IC_FREE(rb.resval.rstring);

#ifdef BUILD_WITH_VBA
	if(CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::VBA))
	{
		::ShowWindow(::GetDlgItem(m_hWnd,IDC_STATIC_VBA),SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd,OPTION_VBASECURITY),SW_SHOW);
		::EnableWindow(::GetDlgItem(m_hWnd,OPTION_VBASECURITY),true);
		((CIcadApp*)AfxGetApp())->GetWorkspaceSecurity (&m_bVbaSecurity);
	}
#endif
}

OptionsGeneralTab::~OptionsGeneralTab()
{
}

void OptionsGeneralTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OptionsGeneralTab)
	DDX_Text(pDX, OPTION_SAVE_TIME, m_autoSaveTime);
	DDV_MinMaxInt(pDX, m_autoSaveTime, 0, 240);
	DDX_CBIndex(pDX, OPTION_EXPERIENCELEVEL, m_expLevel);
	DDX_Text(pDX, OPTION_SAVE_EXTENSION, m_saveExt);
	DDX_Check(pDX, OPTION_VBASECURITY, m_bVbaSecurity);
	//}}AFX_DATA_MAP

    if(m_autoSaveTime==0){
        ::EnableWindow(::GetDlgItem(m_hWnd,OPTION_SAVE_TIME),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,OPTION_SAVE_TIME_SPIN),false);
        m_autoSave = FALSE;
    }else{
        ::EnableWindow(::GetDlgItem(m_hWnd,OPTION_SAVE_TIME),true);
        ::EnableWindow(::GetDlgItem(m_hWnd,OPTION_SAVE_TIME_SPIN),true);
    }
	
	DDX_Check(pDX, OPTION_AUTOSAVE, m_autoSave);

#ifdef BUILD_WITH_VBA
	if(CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::VBA))
	{
		::ShowWindow(::GetDlgItem(m_hWnd,IDC_STATIC_VBA),SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd,OPTION_VBASECURITY),SW_SHOW);
		::EnableWindow(::GetDlgItem(m_hWnd,OPTION_VBASECURITY),true);
	}
#endif

}


BEGIN_MESSAGE_MAP(OptionsGeneralTab, CPropertyPage)
	//{{AFX_MSG_MAP(OptionsGeneralTab)
	ON_BN_CLICKED(OPTION_AUTOSAVE, OnAutosave)
	ON_NOTIFY(UDN_DELTAPOS, OPTION_SAVE_TIME_SPIN, OnAutoSaveSpin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OptionsGeneralTab message handlers

void OptionsGeneralTab::OnAutosave() 
{
    UpdateData(true);

    if(m_autoSave) {
        m_autoSaveTime=AUTOSAVETIME;
    }else{
        m_autoSaveTime=0;
    }
    
	UpdateData(false);	
}

void OptionsGeneralTab::OnAutoSaveSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

    UpdateData(TRUE);
    m_autoSaveTime += -pNMUpDown->iDelta;
    
	if (m_autoSaveTime < 0) m_autoSaveTime = 0;
	if (m_autoSaveTime > 240) m_autoSaveTime = 240;

    UpdateData(FALSE);
	
	*pResult = 0;
}

void OptionsGeneralTab::OnOK() 
{
	resbuf rb;
    rb.restype=RTSHORT;

    rb.resval.rint=m_autoSaveTime;
    sds_setvar("SAVETIME"/*DNT*/,&rb);

    sds_getvar("EXPLEVEL"/*DNT*/,&rb);
    if(rb.resval.rint!=m_expLevel+1){
        rb.resval.rint=m_expLevel+1;
        sds_setvar("EXPLEVEL"/*DNT*/,&rb);
    }

	rb.restype = RTSTR;

    rb.resval.rstring= new char [ m_saveExt.GetLength()+1];
    strcpy(rb.resval.rstring,(LPCTSTR)m_saveExt);
    sds_setvar("SAVEFILE"/*DNT*/,&rb);
    if(rb.resval.rstring) IC_FREE(rb.resval.rstring);

#ifdef BUILD_WITH_VBA
	if(CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::VBA))
		((CIcadApp*)AfxGetApp())->SetWorkspaceSecurity (m_bVbaSecurity);	
#endif

	CPropertyPage::OnOK();
}

BOOL OptionsGeneralTab::OnInitDialog()
	{
	FillCombo();
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void OptionsGeneralTab::FillCombo()
{
	CComboBox *pCombo = NULL;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(OPTION_EXPERIENCELEVEL)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_OPTION_EXPERIENCELEVEL_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_OPTION_EXPERIENCELEVEL_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_OPTION_EXPERIENCELEVEL_3);
	pCombo->AddString(comboOption);
}
