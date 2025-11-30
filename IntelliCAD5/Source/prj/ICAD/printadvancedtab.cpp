/* D:\ICADDEV\PRJ\ICAD\PRINTADVANCEDTAB.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 *
 * Advanced Tab on Print Dialog
 */

// PrintAdvancedTab.cpp : implementation file
//

#include "icad.h"
#include "PrintAdvancedTab.h"
#include "DbLayout.h"
#include "cmdsLayout.h"
#include "PlotProperties.h"
#include "PrintDialog.h"
#include "Preferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PrintAdvancedTab dialog


PrintAdvancedTab::PrintAdvancedTab(CWnd* pParent /*=NULL*/)
	: CPropertyPage(PrintAdvancedTab::IDD), m_plotMgr(NULL), m_bWasPicked(false)
{
	//{{AFX_DATA_INIT(PrintAdvancedTab)
	m_Center = FALSE;
	m_OriginX = _T("");
	m_OriginY = _T("");
	m_Header = _T("");
	m_Footer = _T("");
//4M Item:99->
// Prints upside down
	m_Rotate = FALSE;
//<-4M
// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
	m_UseLWeight = FALSE;
	m_ScaleLWeight = FALSE;
//EBATECH(CNBR)]-
    //}}AFX_DATA_INIT
}


void PrintAdvancedTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PrintAdvancedTab)
	DDX_Check(pDX, PRN_ORIGIN_CENTER, m_Center);
	DDX_Text(pDX, PRN_ORIGIN_X, m_OriginX);
	DDX_Text(pDX, PRN_ORIGIN_Y, m_OriginY);
	DDX_CBString(pDX, PRN_HEADER, m_Header);
	DDX_CBString(pDX, PRN_FOOTER, m_Footer);
//4M Item:99->
// Prints upside down
	DDX_Check(pDX, PRN_ROTATE_0, m_Rotate);
//<-4M
// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
	DDX_Check(pDX, PRN_USE_LWEIGHT, m_UseLWeight);
	DDX_Check(pDX, PRN_SCALE_LWEIGHT, m_ScaleLWeight);
//EBATECH(CNBR)]-
    //}}AFX_DATA_MAP
}

void PrintAdvancedTab::setOrigin(double x, double y)
{ 
	m_oX = x;
	m_oY = y;
	m_bWasPicked = true;
}

void PrintAdvancedTab::LoadPlotSettings()
{
	HRESULT result = -1;
	CComPtr<IIcadPlotProperties> plotProperties;
	if (m_plotMgr)
	{
		result = m_plotMgr->get_PlotProperties(&plotProperties);

		if (SUCCEEDED(result))
		{
			double originVal;
			char originStr[IC_ACADBUF];

			result = plotProperties->get_OriginX(&originVal);

			if (SUCCEEDED(result))
			{
				if(sds_rtos(originVal,-1,-1,originStr)==RTNORM)
					m_OriginX=originStr;
			}

			result = plotProperties->get_OriginY(&originVal);

			if (SUCCEEDED(result))
			{
				if(sds_rtos(originVal,-1,-1,originStr)==RTNORM)
					m_OriginY=originStr;
			}
//4M Item:99->
//Prints upside down
         m_Rotate=FALSE;
         short Rotate;
			result = plotProperties->get_Rotate(&Rotate);

			if (SUCCEEDED(result))
			{
            switch(Rotate){
            case 0  :m_Rotate=FALSE;break;
            case 90 :m_Rotate=FALSE;break;
            case 180:m_Rotate=TRUE;break;
            case 270:m_Rotate=TRUE;break;
            }

			}
//<-4M
			// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
			BOOL bVal;
			result = plotProperties->get_UseLWeight(&bVal);

			if (SUCCEEDED(result))
			{
				m_UseLWeight= ( bVal == TRUE ? 1 : 0);
			}

			result = plotProperties->get_ScaleLWeight(&bVal);

			if (SUCCEEDED(result))
			{
				m_ScaleLWeight= ( bVal == TRUE ? 1 : 0);
			}
			// EBATECH(CNBR) ]-

			UpdateData(FALSE);
		}

	}
}


void PrintAdvancedTab::SavePlotSettings()
{
	UpdateData(TRUE);
	HRESULT result = -1;
	CComPtr<IIcadPlotProperties> plotProperties;
	if (m_plotMgr)
	{
		result = m_plotMgr->get_PlotProperties(&plotProperties);

		if (SUCCEEDED(result))
		{
			double originVal;

			if(!m_OriginX.IsEmpty())
				sds_distof(m_OriginX, -1, &originVal);

			plotProperties->put_OriginX(originVal);

			if(!m_OriginY.IsEmpty())
				sds_distof(m_OriginY, -1, &originVal);

			plotProperties->put_OriginY(originVal);
//4M Item:99->
//Prints upside down
         if (m_Rotate)
            plotProperties->put_Rotate(180);
         else
            plotProperties->put_Rotate(0);
//<-4M
			// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
			BOOL bVal;
			bVal = ( m_UseLWeight != 0 ? TRUE : FALSE );
			plotProperties->put_UseLWeight(bVal);
			bVal = ( m_ScaleLWeight != 0 ? TRUE : FALSE );
			plotProperties->put_ScaleLWeight(bVal);
			// EBATECH(CNBR) ]-
        }
	}
}

void PrintAdvancedTab::LoadRegSettings(bool bFromLayout)
{
	// DP: load plot settings from current LAYOUT
	sds_name layout;
	sds_name_clear(layout);
	if(bFromLayout && cmd_getCurrentLayout(layout))
	{
		CDbLayout* pCurLayout = (CDbLayout*)layout[0];
		
		double x, y;
		pCurLayout->getPlotOrigin(x, y);
		char value[IC_ACADBUF];
		if(sds_rtos(x, -1, -1, value) == RTNORM)
			m_OriginX = value;
		if(sds_rtos(y, -1, -1, value) == RTNORM)
			m_OriginY = value;
		
		m_Rotate = FALSE;
		switch(pCurLayout->plotRotation())
		{
		case CDbLayout::e0:
			m_Rotate = FALSE;
			break;
		case CDbLayout::e90:
			m_Rotate = FALSE;
			break;
		case CDbLayout::e180:
			m_Rotate = TRUE;
			break;
		case CDbLayout::e270:
			m_Rotate = TRUE;
			break;
		default:
			ASSERT(FALSE);
		}
		
		int layoutFlag;
		pCurLayout->get_70(&layoutFlag);
		if(layoutFlag & CDbLayout::ePlotCentered)
			m_Center = 1;
		else
			m_Center = 0;
		// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
		if(layoutFlag & CDbLayout::eScaleLineweights )
			m_ScaleLWeight = 1;
		else
			m_ScaleLWeight = 0;
		if(layoutFlag & CDbLayout::ePrintLineweights )
			m_UseLWeight = 1;
		else
			m_UseLWeight = 0;
		if(layoutFlag & CDbLayout::eModelType )
		{
			m_ScaleLWeight = 0;
			::EnableWindow(::GetDlgItem(m_hWnd,PRN_SCALE_LWEIGHT),FALSE);
		}
		// EBATECH(CNBR) ]-
	}

	HRESULT result;
	if(m_plotMgr)
	{
		BSTR headerBstr;
		result = m_plotMgr->get_Header(&headerBstr);

		if (SUCCEEDED(result))
		{
			m_Header = headerBstr;
			SysFreeString(headerBstr);
		}

		BSTR footerBstr;
		result = m_plotMgr->get_Footer(&footerBstr);

		if (SUCCEEDED(result))
		{
			m_Footer = footerBstr;
			SysFreeString(footerBstr);
		}

		if(layout[0] && layout[1])
		{
			UpdateData(FALSE);
			return;	// get out here, else m_Center, m_OriginX/Y, m_Rotate variables will be overridden. 
		}
		result = m_plotMgr->get_CenterOnPage(&m_Center);
	}

	//LoadPlotSettings sets data in config file and calls update data for us
	LoadPlotSettings();
}

void PrintAdvancedTab::SaveRegSettings(bool bToLayout)
{
	//SavePlotSettings sets data in config file and calls update data for us
	SavePlotSettings();

	if (m_plotMgr)
	{
		BSTR bstr = m_Header.AllocSysString();
		m_plotMgr->put_Header(bstr);
		SysFreeString(bstr);
		
		bstr = m_Footer.AllocSysString();
		m_plotMgr->put_Footer(bstr);
		SysFreeString(bstr);
		
		m_plotMgr->put_CenterOnPage(m_Center);
	}

	if(bToLayout)
	{
		double x, y;
		if(!m_OriginX.IsEmpty())
			sds_distof(m_OriginX, -1, &x);
		else
			x = 0.0;
		if(!m_OriginY.IsEmpty())
			sds_distof(m_OriginY, -1, &y);
		else
			y = 0.0;

		sds_name layout;
		if(!cmd_getCurrentLayout(layout))
		{
			ASSERT(FALSE);
			return;
		}
		struct resbuf* pLayout = sds_entget(layout);
		struct resbuf* pCurPair = pLayout;
		while(pCurPair != NULL)
		{
			switch(pCurPair->restype)
			{
			case 70:
				if(m_Center)
					pCurPair->resval.rint |= CDbLayout::ePlotCentered;
				else
					pCurPair->resval.rint &= ~CDbLayout::ePlotCentered;
				// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
				if(m_ScaleLWeight)
					pCurPair->resval.rint |= CDbLayout::eScaleLineweights;
				else
					pCurPair->resval.rint &= ~CDbLayout::eScaleLineweights;
				if(m_UseLWeight)
					pCurPair->resval.rint |= CDbLayout::ePrintLineweights;
				else
					pCurPair->resval.rint &= ~CDbLayout::ePrintLineweights;
				// EBATECH(CNBR) ]-
				break;
			case 73:
				if(m_Rotate)
					pCurPair->resval.rint = (int)CDbLayout::e180;
				else
					pCurPair->resval.rint = (int)CDbLayout::e0;
				break;
			case 46:
				pCurPair->resval.rreal = x;
				break;
			case 47:
				pCurPair->resval.rreal = y;
				break;
			}
			pCurPair = pCurPair->rbnext;
		}
		sds_entmod(pLayout);
		sds_relrb(pLayout);
	}
}

BEGIN_MESSAGE_MAP(PrintAdvancedTab, CPropertyPage)
	//{{AFX_MSG_MAP(PrintAdvancedTab)
	ON_BN_CLICKED(PRN_ORIGIN_CENTER, OnOriginCenter)
	ON_BN_CLICKED(PRN_SELECT_ORIGIN, OnSelectOrigin)
	ON_BN_CLICKED(PRN_OPEN_CFG, OnOpenCfg)
	ON_BN_CLICKED(PRN_SAVE_CFG, OnSaveCfg)
	ON_CBN_SELCHANGE(PRN_HEADER, OnSelchangeHeader)
	ON_CBN_SELCHANGE(PRN_FOOTER, OnSelchangeFooter)
	ON_WM_CHAR()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PrintAdvancedTab message handlers

void PrintAdvancedTab::OnOriginCenter()
{
	UpdateData(TRUE);


	//::SetWindowText(::GetDlgItem(m_hWnd,PRN_ORIGIN_X),"0.0000"));
	//::SetWindowText(::GetDlgItem(m_hWnd,PRN_ORIGIN_Y),"0.0000"));
    if(m_Center==0)
	{
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_TXT_ORIGIN_X),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_ORIGIN_X),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_TXT_ORIGIN_Y),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_ORIGIN_Y),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_SELECT_ORIGIN),TRUE);
    }
	else
	{
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_TXT_ORIGIN_X),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_ORIGIN_X),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_TXT_ORIGIN_Y),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_ORIGIN_Y),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_SELECT_ORIGIN),FALSE);
	}
	//*** Initialize these strings to 0,0.
	char fs1[IC_ACADBUF];fs1[0]='\0';
	if(sds_rtos(0.0,-1,-1,&fs1[0])==RTNORM)
	{
		m_OriginX = fs1;
		m_OriginY = fs1;
	}
	UpdateData(FALSE);
}

void PrintAdvancedTab::OnSelectOrigin()
{
    UpdateData(TRUE);

	CPrintDialogSheet *parent = (CPrintDialogSheet*) GetParent();
	parent->SaveRegSettings();
	parent->EndDialog(PRN_SELECT_ORIGIN);
}


void PrintAdvancedTab::OnOpenCfg()
{
	CString filename;

	CString filters = ResourceString (IDC_ICADPRINT_CONFIGFILE, "Print Configuration Files (*.pcp)|*.pcp||");

	CFileDialog openDlg (TRUE, "pcp", NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, filters, this);

	if (openDlg.DoModal() == IDOK)
	{
		filename = openDlg.GetPathName();

		HRESULT result = -1;

		if (m_plotMgr)
		{
			result = m_plotMgr->ImportPlotProperties(filename.AllocSysString()); // SysFreeString is called inside
		}

		if (SUCCEEDED(result))
		{
			CPrintDialogSheet *parent = (CPrintDialogSheet*) GetParent();
			parent->OnLoadPlotSettings();
//			parent->SendMessage(ICAD_WM_LOAD, 0, 0);
		}
	}
}


void PrintAdvancedTab::OnSaveCfg()
{
	CString filename;

	CString filters = ResourceString(IDC_ICADPRINT_CONFIGFILE, "Print Configuration Files (*.pcp)|*.pcp||");

	CFileDialog saveDlg (FALSE, "pcp", NULL, OFN_OVERWRITEPROMPT| OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, filters, this);

	if (saveDlg.DoModal() == IDOK)
	{
		CPrintDialogSheet *parent = (CPrintDialogSheet*) GetParent();
		parent->OnSavePlotSettings();
//		parent->SendMessage(ICAD_WM_SAVE, 0, 0);

		filename = saveDlg.GetPathName();

		HRESULT result = -1;

		if (m_plotMgr)
		{
			result = m_plotMgr->ExportPlotProperties(filename.AllocSysString()); // SysFreeString is called inside
		}
	}
}


void PrintAdvancedTab::OnSelchangeHeader()
{
	CComboBox* pHeaderListBox = (CComboBox*)GetDlgItem(PRN_HEADER);
	if(NULL==pHeaderListBox)
	{
		return;
	}
	CString strHeader;


	// need to do to avoid asserts since list box shows " " when "(none)" is desired.
	int selection = pHeaderListBox->GetCurSel();
	if (selection == CB_ERR)
		selection = 0;

	pHeaderListBox->GetLBText(selection,strHeader);
	if(strHeader.CompareNoCase(ResourceString(IDC_ICADPRINTDIA__NONE__3, "(none)" ))==0)
	{
		m_Header.Empty();
		pHeaderListBox->SetWindowText(ResourceString(IDC_ICADPRINTDIA___22, " " ));
		pHeaderListBox->SetEditSel(0,-1);
		pHeaderListBox->Clear();
	}
}


void PrintAdvancedTab::OnSelchangeFooter()
{
	CComboBox* pFooterListBox = (CComboBox*)GetDlgItem(PRN_FOOTER);
	if(NULL==pFooterListBox)
	{
		return;
	}
	CString strFooter;

	// need to do to avoid asserts since list box shows " " when "(none)" is desired.
	int selection = pFooterListBox->GetCurSel();
	if (selection == CB_ERR)
		selection = 0;

	pFooterListBox->GetLBText(selection, strFooter);
	if(strFooter.CompareNoCase(ResourceString(IDC_ICADPRINTDIA__NONE__3, "(none)" ))==0)
	{
		m_Footer.Empty();
		pFooterListBox->SetWindowText(ResourceString(IDC_ICADPRINTDIA___22, " " ));
		pFooterListBox->SetEditSel(0,-1);
		pFooterListBox->Clear();
	}
}

BOOL PrintAdvancedTab::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	//*** Fill the Header and Footer list boxes
	CComboBox* pHeaderListBox = (CComboBox*)GetDlgItem(PRN_HEADER);
	CComboBox* pFooterListBox = (CComboBox*)GetDlgItem(PRN_FOOTER);
	pHeaderListBox->AddString(_T(ResourceString(IDC_ICADPRINTDIA__NONE__3, "(none)" )));
	pFooterListBox->AddString(_T(ResourceString(IDC_ICADPRINTDIA__NONE__3, "(none)" )));

	CString strOwner(ResourceString(IDC_ICADPRINTDIA_YOURNAME_4, "YourName" )),strCompany(ResourceString(IDC_ICADPRINTDIA_YOURCOMPANY_5, "YourCompany" ));
	DWORD dwRetVal;
	int nStrLen;
	_TCHAR *pszTmp;
	HGLOBAL hGlobal;

	CIntelliCadPrefKey		setupKey(HKEY_CURRENT_USER, "Setup", FALSE);
	HKEY hKey = setupKey;
	if(hKey != NULL)
	{
		if(RegQueryValueEx(hKey,"RegisteredOwner"/*DNT*/,NULL,&dwRetVal,NULL,(LPDWORD)&nStrLen)==ERROR_SUCCESS)
		{
			nStrLen+=sizeof(_TCHAR);
			hGlobal=GlobalAlloc(GPTR,nStrLen);
			pszTmp = (_TCHAR*)GlobalLock(hGlobal);
			RegQueryValueEx(hKey,"RegisteredOwner"/*DNT*/,NULL,&dwRetVal,(LPBYTE)pszTmp,(LPDWORD)&nStrLen);
			strOwner=pszTmp;
			GlobalFree(hGlobal);
		}
		if(RegQueryValueEx(hKey,"RegisteredOrganization"/*DNT*/,NULL,&dwRetVal,NULL,(LPDWORD)&nStrLen)==ERROR_SUCCESS)
		{
			nStrLen+=sizeof(_TCHAR);
			hGlobal=GlobalAlloc(GPTR,nStrLen);
			pszTmp = (_TCHAR*)GlobalLock(hGlobal);
			RegQueryValueEx(hKey,"RegisteredOrganization"/*DNT*/,NULL,&dwRetVal,(LPBYTE)pszTmp,(LPDWORD)&nStrLen);
			strCompany=pszTmp;
			GlobalFree(hGlobal);
		}
	}

	time_t aclock;
	time(&aclock);
	struct tm* newtime = localtime(&aclock);

	TCHAR szDate[128];
	GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,NULL,NULL,
		szDate,sizeof(szDate)/sizeof(TCHAR));

	CString strTmp;
	strTmp.Format("%s"/*DNT*/,strOwner);
	pHeaderListBox->AddString(strTmp);
	pFooterListBox->AddString(strTmp);

	strTmp.Format("%s"/*DNT*/,strCompany);
	pHeaderListBox->AddString(strTmp);
	pFooterListBox->AddString(strTmp);

	strTmp.Format("%s"/*DNT*/,szDate);
	pHeaderListBox->AddString(strTmp);
	pFooterListBox->AddString(strTmp);

	strTmp.Format("%s"/*DNT*/,asctime(newtime));
	int nIndex = strTmp.ReverseFind(_T('\n'));
	if(nIndex!=(-1)) strTmp.SetAt(nIndex,_T('\0'));
	pHeaderListBox->AddString(strTmp);
	pFooterListBox->AddString(strTmp);

	strTmp.Format(ResourceString(IDC_ICADPRINTDIA__S__S__S_6, "%s,%s,%s" ),strOwner,strCompany,szDate);
	pHeaderListBox->AddString(strTmp);
	pFooterListBox->AddString(strTmp);

	strTmp.Format(ResourceString(IDC_ICADPRINTDIA_PREPARED_BY_7, "Prepared by %s,%s Confidential,%s" ),strOwner,strCompany,szDate);
	pHeaderListBox->AddString(strTmp);
	pFooterListBox->AddString(strTmp);

	strTmp.Format(ResourceString(IDC_ICADPRINTDIA__S_CONFIDEN_8, "%s Confidential,%s" ),strCompany,szDate);
	pHeaderListBox->AddString(strTmp);
	pFooterListBox->AddString(strTmp);

	strTmp.Format(ResourceString(IDC_ICADPRINTDIA__S_CONFIDEN_9, "%s Confidential,%s, " ),strCompany,szDate);
	pHeaderListBox->AddString(strTmp);
	pFooterListBox->AddString(strTmp);

	strTmp.Format(ResourceString(IDC_ICADPRINTDIA_PREPARED_B_10, "Prepared by %s, ,%s" ),strOwner,szDate);
	pHeaderListBox->AddString(strTmp);
	pFooterListBox->AddString(strTmp);

	//*** Initialize controls
    OnOriginCenter();

	initSettings();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

bool PrintAdvancedTab::initSettings()
{
	// Restory previouse settings from REGISTRY if we returned to print dialog from print preview dialog.
	// (especially it is necessary if UserPreference::s_bSaveToLayout is off)
	CPrintDialogSheet* pPrintDlg = (CPrintDialogSheet*) GetParent();
	ASSERT(pPrintDlg);
	LoadRegSettings(!pPrintDlg->isRestorySettingsFromRegistry());

	if(m_bWasPicked)
	{
		char value[IC_ACADBUF];
		if(sds_rtos(m_oX, -1, -1, value) == RTNORM)
			m_OriginX = value;
		if(sds_rtos(m_oY, -1, -1, value) == RTNORM)
			m_OriginY = value;
		m_bWasPicked = false;
		UpdateData(FALSE);
	}
	return true;
}

void PrintAdvancedTab::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar==VK_ESCAPE)
	{
		CWnd *parent = GetParent();
		parent->SendMessage( WM_CHAR, nChar, 0);
	}
	else
	{
		CPropertyPage::OnChar(nChar, nRepCnt, nFlags);
	}
}


void PrintAdvancedTab::OnClose()
{
	CPropertyPage::OnClose();

	HWND hParentWnd;
	hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}


BOOL PrintAdvancedTab::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int wNotifyCode = HIWORD(wParam); // notification code
	int wID = LOWORD(wParam);         // item, control, or accelerator identifier
	HWND hwndCtl = (HWND) lParam;      // handle of control

	// don't know the #defined version of this message.
	if (wNotifyCode == 0)
	{
		// want to send escape codes to parent
		// wID 2 == ESC
		// wID 1 == Enter from Edit box
		if ((wID == 2) || (wID == 1))
			return GetParent()->SendMessage(WM_COMMAND, wParam, lParam);
	}

	if (wNotifyCode == 4)
	{
		;
	}

	return CPropertyPage::OnCommand(wParam, lParam);
}


void PrintAdvancedTab::SetDefaults()
{
    m_Center=0;
	m_UseLWeight=0;
	m_ScaleLWeight=0;

	//*** Initialize these strings to 0,0.
	char fs1[IC_ACADBUF];
	if(sds_rtos(0.0,-1,-1,fs1)==RTNORM)
	{
		m_OriginX = fs1;
		m_OriginY = fs1;
	}

	m_Header.Empty();
	m_Footer.Empty();
	//Bugzilla No. 78286 ; 23-09-2002 [
	if(m_Rotate)
	{
		m_Rotate=FALSE; 
		
		HRESULT result = -1;

		CComPtr<IIcadPlotProperties> plotProperties;

		if (m_plotMgr)
		{
			result = m_plotMgr->get_PlotProperties(&plotProperties);
			if (SUCCEEDED(result))
				plotProperties->put_Rotate(0);
		}
	}
	//Bugzilla No. 78286 ; 23-09-2002 ]
	UpdateData(FALSE);

	OnOriginCenter();
}
