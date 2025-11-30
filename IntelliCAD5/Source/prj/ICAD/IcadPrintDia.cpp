/* PRJ\ICAD\IcadPrintDia.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 *
 */

#include "Icad.h"/*DNT*/
#include "IcadApp.h"/*DNT*/
#include "IcadPrintDia.h"/*DNT*/
#include "resource.hm"/*DNT*/

#include "checkfp.h"/*DNT*/


//////////////////////////////////////////////////////////////////////
// prn_Print Constructor.

prn_Print::prn_Print() : CDialog(PRN_PRINT)
{
	SetDefaults(FALSE);
	m_pViewList = new CPtrList();
	sds_ssadd(NULL,NULL,m_ssPrintEnts);
	m_bPrinting=FALSE;
	m_bPreview=FALSE;
	m_bPrintDirect=FALSE;
}

prn_Print::~prn_Print()
{
	ResetViewList();
	delete m_pViewList;
	sds_ssfree(m_ssPrintEnts);
}

void prn_Print::DoDataExchange(CDataExchange *pDX)
{
    DDX_Radio(pDX,PRN_VIEW,m_iView);
    DDX_Radio(pDX,PRN_SELMODE,m_iSelMode);
    DDX_Radio(pDX,PRN_SCALE14,m_iScale);
    DDX_Check(pDX,PRN_CLIPWINDOW,m_iClipMode);
    DDX_Check(pDX,PRN_SCL_FIT,m_iFitMode);
    DDX_Check(pDX,PRN_MAPTOBLACK,m_iMapToBlack);
    DDX_Text(pDX,PRN_FROM_X,m_strFromX);
    DDX_Text(pDX,PRN_FROM_Y,m_strFromY);
    DDX_Text(pDX,PRN_TO_X,m_strToX);
    DDX_Text(pDX,PRN_TO_Y,m_strToY);
    DDX_Check(pDX,PRN_ORIGIN_CENTER,m_iCenter);
    DDX_Text(pDX,PRN_ORIGIN_X,m_strOriginX);
    DDX_Text(pDX,PRN_ORIGIN_Y,m_strOriginY);
    DDX_Text(pDX,PRN_PLOTUNITS,m_strDrawingUnits);
    DDX_Text(pDX,PRN_PRINTUNITS,m_strPrintedUnits);
    DDX_Text(pDX,PRN_LINEWIDTH,m_nLineWidth);
	DDX_CBString(pDX,PRN_VIEWLIST,m_strViewName);
	DDX_CBString(pDX,PRN_HEADER,m_strHeader);
	DDX_CBString(pDX,PRN_FOOTER,m_strFooter);
}

void prn_Print::SetDefaults(BOOL bDisplayWarning)
{
	if(bDisplayWarning)
	{
		UpdateData(TRUE);

		if(MessageBox(ResourceString(IDC_ICADPRINTDIA_DO_YOU_WANT_0, "Do you want to change back to the default Print settings?" ),
			ResourceString(IDC_ICADPRINTDIA_RESET_PRINT_1, "Reset Print Settings" ),MB_ICONQUESTION | MB_YESNO)==IDNO)
		{
			return;
		}
	}
    m_iView=PRN_OPT_CURRENTVIEW;
    m_iScale=PRN_OPT_INCHES;
    m_iFitMode=1;
    m_iMapToBlack=0;
    m_iClipMode=0;
    m_iCenter=0;
	m_nLineWidth=1;

	//*** Initialize these strings to 0,0.
	char fs1[IC_ACADBUF];
	if(sds_rtos(0.0,-1,-1,fs1)==RTNORM)
	{
		m_strFromX=fs1; m_strFromY=fs1;
		m_strToX=fs1; m_strToY=fs1;
		m_strOriginX=fs1; m_strOriginY=fs1;
	}
	//*** Initialize these strings to 1.
	if(sds_rtos(1.0,-1,-1,fs1)==RTNORM)
	{
		m_strDrawingUnits=fs1;
		m_strPrintedUnits=fs1;
	}
	m_strHeader.Empty();
	m_strFooter.Empty();
	m_dDrawingUnits=1.0;
	m_dPrintedUnits=1.0;
}

void prn_Print::SetPaperSize()
{
	CString strPaperSize;
	CDC dc;
	if(!((CIcadApp*)AfxGetApp())->GetPrinterDC(dc)) return;
	double nPixPerInchX = (double)dc.GetDeviceCaps(LOGPIXELSX);
	double nPixPerInchY = (double)dc.GetDeviceCaps(LOGPIXELSY);
	double dInchesX = (double)dc.GetDeviceCaps(PHYSICALWIDTH);
	dInchesX/=nPixPerInchX;
	double dInchesY = (double)dc.GetDeviceCaps(PHYSICALHEIGHT);
	dInchesY/=nPixPerInchY;
	if(m_iScale==PRN_OPT_INCHES)
	{
		strPaperSize.Format(ResourceString(IDC_ICADPRINTDIA___2F_X___2F_2, "%.2f x %.2f" ),dInchesX,dInchesY);
	}
	else
	{
		double dMMX=dInchesX*25.4;
		double dMMY=dInchesY*25.4;
		strPaperSize.Format(ResourceString(IDC_ICADPRINTDIA___2F_X___2F_2, "%.2f x %.2f" ),dMMX,dMMY);
	}
	::SetWindowText(::GetDlgItem(GetSafeHwnd(),PRN_SCL_PAPER),strPaperSize);
	UpdateFitSize(&dc);
}

void prn_Print::ResetViewList()
{
	struct sds_resbuf* pRb=NULL;

	if(!m_pViewList->IsEmpty())
	{
		POSITION pos = m_pViewList->GetHeadPosition();
		while(pos!=NULL)
		{
			pRb = (struct sds_resbuf*)m_pViewList->GetNext(pos);
			IC_RELRB(pRb);
		}
		m_pViewList->RemoveAll();
	}
}

BOOL prn_Print::OnInitDialog()
{
    BOOL ret=CDialog::OnInitDialog();

	//*** Disable the preview button if printing direct.
	::EnableWindow(::GetDlgItem(GetSafeHwnd(),PRN_PREVIEW),(!m_bPrintDirect));

	long ssct;
	if(sds_sslength(m_ssPrintEnts,&ssct)==RTNORM && ssct>0L)
	{
		::EnableWindow(::GetDlgItem(GetSafeHwnd(),PRN_SELMODES),TRUE);
	}
	else
	{
		::EnableWindow(::GetDlgItem(GetSafeHwnd(),PRN_SELMODES),FALSE);
	}
	if(m_pViewList->IsEmpty())
	{
		//*** If there aren't any views defined in the drawing gray out the
		//*** Saved view option.
		if(m_iView==PRN_OPT_SAVEDVIEW)
		{
			m_iView=PRN_OPT_CURRENTVIEW;
			UpdateData(FALSE);
		}
		::EnableWindow(::GetDlgItem(GetSafeHwnd(),PRN_VIEW4),FALSE);
		::EnableWindow(::GetDlgItem(GetSafeHwnd(),PRN_VIEWLIST),FALSE);
	}
	else
	{
		struct sds_resbuf* pRb;
		CComboBox* pViewListBox = (CComboBox*)GetDlgItem(PRN_VIEWLIST);
		if(pViewListBox!=NULL)
		{
			POSITION pos = m_pViewList->GetHeadPosition();
			while(pos!=NULL && pViewListBox!=NULL)
			{
				pRb = (struct sds_resbuf*)m_pViewList->GetNext(pos);
				if(ic_assoc(pRb,2)==0) pViewListBox->AddString(ic_rbassoc->resval.rstring);
			}
			if(m_strViewName.IsEmpty() ||
				LB_ERR==pViewListBox->SelectString(-1,m_strViewName))
			{
				pViewListBox->SetCurSel(0);
			}
		}
	}
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
	CIntelliCadPrefKey		setupKey( HKEY_CURRENT_USER, "Setup", FALSE);
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
	OnUnitsChanged();
    OnClick();
    OnCenter();
	OnFitToPage();
	UpdateFitSize();
    return(ret);
}

//////////////////////////////////////////////////////////////////////
// prn_Print Message map and member functions.

BEGIN_MESSAGE_MAP(prn_Print,CDialog)
	ON_BN_CLICKED(PRN_SCALE18,OnUnitsChanged)
	ON_BN_CLICKED(PRN_SCALE14,OnUnitsChanged)
	ON_BN_CLICKED(PRN_SCL_FIT,OnFitToPage)
	ON_BN_CLICKED(PRN_VIEW,OnClick)
	ON_BN_CLICKED(PRN_VIEW2,OnClick)
	ON_BN_CLICKED(PRN_VIEW3,OnClick)
	ON_BN_CLICKED(PRN_VIEW4,OnClick)
	ON_BN_CLICKED(PRN_VIEW5,OnClick)
	ON_BN_CLICKED(PRN_SELECT_AREA,OnSelectArea)
	ON_BN_CLICKED(PRN_ORIGIN_CENTER,OnCenter)
	ON_BN_CLICKED(PRN_SELECT_ORIGIN,OnSelectOrigin)
	ON_BN_CLICKED(PRN_PREVIEW,OnPrintPreview)
	ON_BN_CLICKED(IDOK,OnOk)
	ON_BN_CLICKED(IDCANCEL,OnCancel)
	ON_BN_CLICKED(PRN_SETUP,OnPrintSetup)
	ON_BN_CLICKED(PRN_DEFAULTS,OnSetDefaults)
	ON_CBN_SELCHANGE(PRN_HEADER,OnUpdateHeader)
	ON_CBN_SELCHANGE(PRN_FOOTER,OnUpdateFooter)
	ON_CBN_SELCHANGE(PRN_VIEWLIST,OnViewSel)
	ON_BN_CLICKED(IDHELP,OnHelp)
	ON_WM_HELPINFO()
    ON_NOTIFY(UDN_DELTAPOS,PRN_SPIN_LINEWIDTH,OnSpinLineWidth)
END_MESSAGE_MAP()

BOOL prn_Print::OnHelpInfo(HELPINFO *pHel){
	OnHelp();
	return(CWnd::OnHelpInfo(pHel));
}

void prn_Print::OnHelp() {
	CommandRefHelp( m_hWnd, HLP_PRINT);
}

void prn_Print::OnSelectArea()
{
    UpdateData(TRUE);
    EndDialog(PRN_SELECT_AREA);
}

void prn_Print::OnCenter()
{
	UpdateData(TRUE);
    if(m_iCenter==0)
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
}

void prn_Print::OnSelectOrigin()
{
    UpdateData(TRUE);
    EndDialog(PRN_SELECT_ORIGIN);
}

void prn_Print::OnPrintPreview()
{
    DoEndDialog(PRN_PREVIEW);
}

void prn_Print::OnOk()
{
	//*** If printing (as opposed to setup) this gets freed in FilePrint in
	//*** CIcadView because it is too early to free it here.
	if(!m_bPrinting)
	{
		sds_name ename;
		for(long ssct=0L; sds_ssname(m_ssPrintEnts,ssct,ename)==RTNORM; ssct++)
		{
			//*** Un-hilight the selected entities.
			sds_redraw(ename,IC_REDRAW_UNHILITE);
		}
		sds_ssfree(m_ssPrintEnts);
		sds_ssadd(NULL,NULL,m_ssPrintEnts);
	}
    DoEndDialog(IDOK);
}

void prn_Print::OnCancel()
{
	sds_name ename;
	for(long ssct=0L; sds_ssname(m_ssPrintEnts,ssct,ename)==RTNORM; ssct++)
	{
		//*** Un-hilight the selected entities.
		sds_redraw(ename,IC_REDRAW_UNHILITE);
	}
	sds_ssfree(m_ssPrintEnts);
	sds_ssadd(NULL,NULL,m_ssPrintEnts);
    UpdateData(TRUE);
	if(SDS_CMainWindow->GetIcadView())
	{
		SDS_CMainWindow->GetIcadView()->RestorePrintViewInfo();
	}
    EndDialog(IDCANCEL);
}

void prn_Print::DoEndDialog(int nResult)
{
    UpdateData(TRUE);
	if(SDS_CMainWindow->GetIcadView())
	{
		SDS_CMainWindow->GetIcadView()->RestorePrintViewInfo();
	}

	CString str;
	sds_real rDist;
	if(RTNORM!=sds_distof(m_strOriginX,-1,&rDist))
	{
		str.Format(ResourceString(IDC_ICADPRINTDIA__IS_NOT_A__16, "\"%s\" is not a valid X value.\nPlease enter a valid value before continuing." ),m_strOriginX);
		MessageBox(str,ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),MB_OK | MB_ICONEXCLAMATION);
		//*** Set focus to the From X edit box
		CEdit* pEdit = (CEdit*)GetDlgItem(PRN_ORIGIN_X);
		if(pEdit!=NULL)
		{
			pEdit->SetFocus();
		}
		return;
	}
	if(RTNORM!=sds_distof(m_strOriginY,-1,&rDist))
	{
		str.Format(ResourceString(IDC_ICADPRINTDIA__IS_NOT_A__17, "\"%s\" is not a valid Y value.\nPlease enter a valid value before continuing." ),m_strOriginY);
		MessageBox(str,ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),MB_OK | MB_ICONEXCLAMATION);
		//*** Set focus to the From Y edit box
		CEdit* pEdit = (CEdit*)GetDlgItem(PRN_ORIGIN_Y);
		if(pEdit!=NULL)
		{
			pEdit->SetFocus();
		}
		return;
	}

	if(0==m_iFitMode)
	{
		int nUnit; //*** Uh huh-huh huh-huh huh-huh
		BOOL bWorked;
		for(nUnit=(-1),bWorked=FALSE; nUnit<=5; nUnit++)
		{
			if(RTNORM==sds_distof(m_strDrawingUnits,nUnit,&m_dDrawingUnits))
			{
				bWorked=TRUE;
				break;
			}
		}
		if(!bWorked)
		{
			str.Format(ResourceString(IDC_ICADPRINTDIA__IS_NOT_A__18, "\"%s\" is not a valid value for drawing units.\nPlease enter a valid value before continuing." ),m_strDrawingUnits);
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
			if(RTNORM==sds_distof(m_strPrintedUnits,nUnit,&m_dPrintedUnits))
			{
				bWorked=TRUE;
				break;
			}
		}
		if(!bWorked)
		{
			str.Format(ResourceString(IDC_ICADPRINTDIA__IS_NOT_A__12, "\"%s\" is not a valid value for printed units.\nPlease enter a valid value before continuing." ),m_strPrintedUnits);
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

	if(PRN_OPT_WINDOW==m_iView)
	{
		sds_point ptFrom,ptTo;
		//*** Convert the string from the editboxes into points.
		if(RTNORM!=sds_distof(m_strFromX,-1,&ptFrom[0]))
		{
			str.Format(ResourceString(IDC_ICADPRINTDIA__IS_NOT_A__16, "\"%s\" is not a valid X value.\nPlease enter a valid value before continuing." ),m_strFromX);
			MessageBox(str,ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),MB_OK | MB_ICONEXCLAMATION);
			//*** Set focus to the From X edit box
			CEdit* pEdit = (CEdit*)GetDlgItem(PRN_FROM_X);
			if(pEdit!=NULL)
			{
				pEdit->SetFocus();
			}
			return;
		}
		if(RTNORM!=sds_distof(m_strFromY,-1,&ptFrom[1]))
		{
			str.Format(ResourceString(IDC_ICADPRINTDIA__IS_NOT_A__17, "\"%s\" is not a valid Y value.\nPlease enter a valid value before continuing." ),m_strFromY);
			MessageBox(str,ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),MB_OK | MB_ICONEXCLAMATION);
			//*** Set focus to the From Y edit box
			CEdit* pEdit = (CEdit*)GetDlgItem(PRN_FROM_Y);
			if(pEdit!=NULL)
			{
				pEdit->SetFocus();
			}
			return;
		}
		if(RTNORM!=sds_distof(m_strToX,-1,&ptTo[0]))
		{
			str.Format(ResourceString(IDC_ICADPRINTDIA__IS_NOT_A__16, "\"%s\" is not a valid X value.\nPlease enter a valid value before continuing." ),m_strToX);
			MessageBox(str,ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),MB_OK | MB_ICONEXCLAMATION);
			//*** Set focus to the To X edit box
			CEdit* pEdit = (CEdit*)GetDlgItem(PRN_TO_X);
			if(pEdit!=NULL)
			{
				pEdit->SetFocus();
			}
			return;
		}
		if(RTNORM!=sds_distof(m_strToY,-1,&ptTo[1]))
		{
			str.Format(ResourceString(IDC_ICADPRINTDIA__IS_NOT_A__17, "\"%s\" is not a valid Y value.\nPlease enter a valid value before continuing." ),m_strToY);
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

void prn_Print::OnPrintSetup()
{
    UpdateData(TRUE);
	((CIcadApp*)AfxGetApp())->FilePrinterSetup();
	SetPaperSize();
}

BOOL prn_Print::UpdateFitSize(CDC* pDC)
{
	if(m_iFitMode==0)
	{
		//*** Don't change the values if NOT fitmode.
		return FALSE;
	}
	//*** Fill the print info struct
	ICPRINTINFO pi;
	memset(&pi,0,sizeof(pi));
	CPrintDialog pd(TRUE);
	pd.GetDefaults();
	CDC* pDefaultDC = NULL;
	if(NULL==pDC)
	{
		pDefaultDC = new CDC();
		if(!((CIcadApp*)AfxGetApp())->GetPrinterDC(*pDefaultDC))
		{
			return FALSE;
		}
		pDC = pDefaultDC;
	}
	pi.pDC = pDC;
	if(!GetCurrentPrintInfo(&pi))
	{
		return FALSE;
	}
	//*** Convert device units to inches (Width only, for m_strPrintedUnits)
	int nPixPerInchX = pDC->GetDeviceCaps(LOGPIXELSX);
	double dPrinterWidthMmIn = (pi.dAdjustedWidth/((double)nPixPerInchX));
	if(m_iScale!=PRN_OPT_INCHES)
	{
		dPrinterWidthMmIn*=25.4; //*** Convert Inches to Millimeters.
	}
	char fs1[IC_ACADBUF];
	if(sds_rtos(dPrinterWidthMmIn,-1,-1,fs1)==RTNORM)
	{
		m_strPrintedUnits=fs1;
	}
	//*** Calculate the width of the printer in drawing units
	double dPrinterWidthDU = pi.dAdjustedWidth/pi.dPixPerDU;
	if(sds_rtos(dPrinterWidthDU,-1,-1,fs1)==RTNORM)
	{
		m_strDrawingUnits=fs1;
	}

    UpdateData(FALSE);
	if(NULL!=pDefaultDC)
	{
		delete pDefaultDC;
	}
	return TRUE;
}

BOOL prn_Print::GetCurrentPrintInfo(LPICPRINTINFO pPrintInfo)
{
//*** This function fills in the values of the print info structure.
	if(NULL==pPrintInfo)
	{
		return FALSE;
	}
	sds_point ptLwrLeft,ptUprRight;
	GetPrintWindow(pPrintInfo->pView,ptLwrLeft,ptUprRight);
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
	if(!m_strHeader.IsEmpty())
	{
		//*** If there is a Header, add 1/4" space for it.
		pPrintInfo->dHeaderAdjust=(dPixPerInchY/4.0);
	}
	if(!m_strFooter.IsEmpty())
	{
		//*** If there is a Footer, add 1/4" space for it.
		pPrintInfo->dFooterAdjust=(dPixPerInchY/4.0);
	}

									// make sure driver hasn't reset it
	CheckFPControlWord();

	//*** Calculate the viewsize.
	if(m_iFitMode>0)
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
		pPrintInfo->dViewSize=(m_dDrawingUnits/m_dPrintedUnits*dInchesY);
		if(m_iScale!=PRN_OPT_INCHES)
		{
			pPrintInfo->dViewSize*=25.4; //*** Convert inches to millimeters.
		}
	}
	pPrintInfo->dPixPerDU = (pPrintInfo->dAdjustedHeight/pPrintInfo->dViewSize);
	return TRUE;
}

struct sds_resbuf* prn_Print::GetCurViewRb()
{
	struct sds_resbuf* pRb = NULL;
	struct sds_resbuf* pRbTmp;
	POSITION pos = m_pViewList->GetHeadPosition();
	for(pRb=NULL; pos!=NULL; pRb=NULL)
	{
		pRbTmp = (struct sds_resbuf*)m_pViewList->GetNext(pos);
		if(ic_assoc(pRbTmp,2)!=0)
		{
			continue;
		}
		if(m_strViewName.Compare(ic_rbassoc->resval.rstring)==0)
		{
			pRb=pRbTmp;
			break;
		}
	}
	return pRb;
}

BOOL prn_Print::GetPrintWindow(CIcadView* pView, sds_point ptLwrLeft,
	sds_point ptUprRight)
{
	if(NULL==pView)
	{
		pView = SDS_CMainWindow->GetIcadView();
		CRect rect;
		pView->GetClientRect(rect);
		pView->m_sizePrintView=rect.Size();
	}

	struct sds_resbuf rb;
	sds_real rViewSize,rViewWidth;
	sds_point ptViewCtr;


	db_drawing *flp=NULL;
	CIcadDoc* pDoc = pView->GetDocument();
	if(NULL!=pDoc)
	{
		flp=pDoc->m_dbDrawing;
	}

	int tilemode,cvport;
	SDS_getvar(NULL,DB_QTILEMODE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
	tilemode=rb.resval.rint;
	SDS_getvar(NULL,DB_QCVPORT,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
	cvport=rb.resval.rint;

	switch(m_iView)
	{
		case PRN_OPT_CURRENTVIEW:
			{
				sds_point ptScreenSize;

				//*** Get the SCREENSIZE.
				ptScreenSize[0]=pView->m_sizePrintView.cx;
				ptScreenSize[1]=pView->m_sizePrintView.cy;
				ptScreenSize[2]=0.0;

				if(tilemode==0) {
					SDS_getvar(NULL,DB_QVIEWSIZE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
					rViewSize=rb.resval.rreal;
					SDS_getvar(NULL,DB_QVIEWCTR,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
					ic_ptcpy(ptViewCtr,rb.resval.rpoint);
				} else {
					//*** Get the current view size.
					pView->m_pVportTabHip->get_40(&rViewSize); // VIEWSIZE
					//*** Get the current view center.
					pView->m_pVportTabHip->get_12(ptViewCtr); // VIEWCTR
				}

				//*** Calculate the From point.
				rViewWidth=(rViewSize*(ptScreenSize[0]/ptScreenSize[1]));
				ptLwrLeft[0]=(ptViewCtr[0]-(rViewWidth/2.0));
				ptUprRight[0]=(ptViewCtr[0]+(rViewWidth/2.0));
				//*** Calculate the To point.
				ptLwrLeft[1]=(ptViewCtr[1]-(rViewSize/2.0));
				ptUprRight[1]=(ptViewCtr[1]+(rViewSize/2.0));

				//*** Make sure the Z gets zero'd before returning.
				ptLwrLeft[2]=0.0; ptUprRight[2]=0.0;
			}
			break;
		case PRN_OPT_SAVEDVIEW:
			{
				struct sds_resbuf* pRb;
				pRb = GetCurViewRb();
				if(NULL==pRb)
					break;

				//*** Get the height and width of the view.
				if(ic_assoc(pRb,40)==0) rViewSize=ic_rbassoc->resval.rreal;
				if(ic_assoc(pRb,41)==0) rViewWidth=ic_rbassoc->resval.rreal;
				//*** Get the center point of the view.
				if(ic_assoc(pRb,10)==0) ic_ptcpy(ptViewCtr,ic_rbassoc->resval.rpoint);
				//*** Calculate the From point.
				ptLwrLeft[0]=(ptViewCtr[0]-(rViewWidth/2.0));
				ptUprRight[0]=(ptViewCtr[0]+(rViewWidth/2.0));
				//*** Calculate the To point.
				ptLwrLeft[1]=(ptViewCtr[1]-(rViewSize/2.0));
				ptUprRight[1]=(ptViewCtr[1]+(rViewSize/2.0));

				//*** Make sure the Z gets zero'd before returning.
				ptLwrLeft[2]=0.0; ptUprRight[2]=0.0;

				pView->SavePrintViewInfo(pRb);
			}
			break;
		case PRN_OPT_EXTENTS:
			{
				int tilemode,cvport;
				sds_point ptTmpLwrLeft,ptTmpUprRight;

				//*** Make sure the Z gets initialized to zeros.
				ptLwrLeft[2]=0.0; ptUprRight[2]=0.0;
				ptTmpLwrLeft[2]=0.0; ptTmpUprRight[2]=0.0;

				SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				tilemode=rb.resval.rint;
				SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				cvport=rb.resval.rint;
				SDS_getvar(NULL,((tilemode==0 && cvport==1) ? DB_QP_EXTMIN : DB_QEXTMIN),&rb,flp,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(ptTmpLwrLeft,rb.resval.rpoint);
				SDS_getvar(NULL,((tilemode==0 && cvport==1) ? DB_QP_EXTMAX : DB_QEXTMAX),&rb,flp,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(ptTmpUprRight,rb.resval.rpoint);

				SDS_rpextents(ptTmpLwrLeft,ptTmpUprRight,ptLwrLeft,ptUprRight,NULL);

				//*** Expand the extents area by 5 percent.
				double dOffsetX = ((ptUprRight[0]-ptLwrLeft[0])*1.095);
				dOffsetX-=(ptUprRight[0]-ptLwrLeft[0]);
				double dOffsetY = ((ptUprRight[1]-ptLwrLeft[1])*1.095);
				dOffsetY-=(ptUprRight[1]-ptLwrLeft[1]);
				ptUprRight[0]+=dOffsetX;
				ptUprRight[1]+=dOffsetY;
				ptLwrLeft[0]-=dOffsetX;
				ptLwrLeft[1]-=dOffsetY;
			}
			break;
		case PRN_OPT_LIMITS:
			{
				int tilemode,cvport;

				SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				tilemode=rb.resval.rint;
				SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				cvport=rb.resval.rint;

				SDS_getvar(NULL,((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN),&rb,flp,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(ptLwrLeft,rb.resval.rpoint);
				SDS_getvar(NULL,((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX),&rb,flp,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(ptUprRight,rb.resval.rpoint);

				//*** Make sure the Z gets zero'd before calling gr_ucs2rp.
				ptLwrLeft[2]=0.0; ptUprRight[2]=0.0;

				gr_ucs2rp(ptLwrLeft,ptLwrLeft,pView->m_pCurDwgView);
				gr_ucs2rp(ptUprRight,ptUprRight,pView->m_pCurDwgView);
			}
			break;
		case PRN_OPT_WINDOW:
			{
				sds_point ptFrom,ptTo;

				//*** Convert the string from the editboxes into points.
				sds_distof(m_strFromX,-1,&ptFrom[0]);
				sds_distof(m_strFromY,-1,&ptFrom[1]);
				sds_distof(m_strToX,-1,&ptTo[0]);
				sds_distof(m_strToY,-1,&ptTo[1]);
				//*** Make sure the Z gets zero'd before calling gr_ucs2rp.
				ptTo[2]=0.0; ptFrom[2]=0.0;
				//*** Setup m_ptLwrLeft as lower left corner and m_ptUprRight as upper right corner.
				gr_ucs2rp(ptFrom,ptFrom,pView->m_pCurDwgView);
				gr_ucs2rp(ptTo,ptTo,pView->m_pCurDwgView);
				ptLwrLeft[0]=min(ptFrom[0],ptTo[0]);
				ptLwrLeft[1]=min(ptFrom[1],ptTo[1]);
				ptUprRight[0]=max(ptFrom[0],ptTo[0]);
				ptUprRight[1]=max(ptFrom[1],ptTo[1]);
				//*** Make sure the Z gets zero'd before we return.
				ptLwrLeft[2]=0.0; ptUprRight[2]=0.0;
			}
			break;
		default:
			return FALSE;
	}
	return TRUE;
}

void prn_Print::OnClick()
{
    UpdateData(TRUE);

	if(m_iView==PRN_OPT_EXTENTS) ::EnableWindow(::GetDlgItem(m_hWnd,PRN_CLIPWINDOW),FALSE);
	else ::EnableWindow(::GetDlgItem(m_hWnd,PRN_CLIPWINDOW),TRUE);

	if(m_iView==PRN_OPT_SAVEDVIEW) ::EnableWindow(::GetDlgItem(m_hWnd,PRN_VIEWLIST),TRUE);
	else ::EnableWindow(::GetDlgItem(m_hWnd,PRN_VIEWLIST),FALSE);

    if(m_iView==PRN_OPT_WINDOW)
	{
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_FROM_X),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_FROM_Y),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_TO_X),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_TO_Y),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_STATIC1),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_STATIC2),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_STATIC3),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_STATIC4),TRUE);
        /*if(!m_bPreview)*/ ::EnableWindow(::GetDlgItem(m_hWnd,PRN_SELECT_AREA),TRUE);
    }
	else
	{
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_FROM_X),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_FROM_Y),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_TO_X),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_TO_Y),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_STATIC1),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_STATIC2),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_STATIC3),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_STATIC4),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_SELECT_AREA),FALSE);
    }
	UpdateFitSize();
}

void prn_Print::OnFitToPage()
{
    UpdateData(TRUE);
    if(m_iFitMode==0)
	{
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_PLOTUNITS),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_PRINTUNITS),TRUE);
		//*** Initialize these strings to 1.
		char fs1[IC_ACADBUF];
		if(sds_rtos(m_dDrawingUnits,-1,-1,fs1)==RTNORM)
		{
			m_strDrawingUnits=fs1;
		}
		if(sds_rtos(m_dPrintedUnits,-1,-1,fs1)==RTNORM)
		{
			m_strPrintedUnits=fs1;
		}
		UpdateData(FALSE);
    }
	else
	{
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_PLOTUNITS),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,PRN_PRINTUNITS),FALSE);
		UpdateFitSize();
	}
}

void prn_Print::OnUnitsChanged()
{
    UpdateData(TRUE);
	CString strPrintUnit;
	if(m_iScale==PRN_OPT_INCHES)
	{
		strPrintUnit=_T(ResourceString(IDC_ICADPRINTDIA_PRINTED_IN_20, "Printed Inches" ));
	}
	else
	{
		strPrintUnit=_T(ResourceString(IDC_ICADPRINTDIA_PRINTED_MI_21, "Printed Millimeters" ));
	}
	::SetWindowText(::GetDlgItem(GetSafeHwnd(),PRN_TXT_PRN_UNITS),strPrintUnit);
	SetPaperSize();
}

void prn_Print::OnSetDefaults()
{
	SetDefaults(TRUE);
    UpdateData(FALSE);
    OnCenter();
    OnClick();
	OnFitToPage();
}

void prn_Print::OnViewSel()
{
    UpdateData(TRUE);
	UpdateFitSize();
}

void prn_Print::OnUpdateHeader()
{
	CComboBox* pHeaderListBox = (CComboBox*)GetDlgItem(PRN_HEADER);
	if(NULL==pHeaderListBox)
	{
		return;
	}
	CString strHeader;
	pHeaderListBox->GetLBText(pHeaderListBox->GetCurSel(),strHeader);
	if(strHeader.CompareNoCase(ResourceString(IDC_ICADPRINTDIA__NONE__3, "(none)" ))==0)
	{
		m_strHeader.Empty();
		pHeaderListBox->SetWindowText(ResourceString(IDC_ICADPRINTDIA___22, " " ));
		pHeaderListBox->SetEditSel(0,-1);
		pHeaderListBox->Clear();
	}
}

void prn_Print::OnUpdateFooter()
{
	CComboBox* pFooterListBox = (CComboBox*)GetDlgItem(PRN_FOOTER);
	if(NULL==pFooterListBox)
	{
		return;
	}
	CString strFooter;
	pFooterListBox->GetLBText(pFooterListBox->GetCurSel(),strFooter);
	if(strFooter.CompareNoCase(ResourceString(IDC_ICADPRINTDIA__NONE__3, "(none)" ))==0)
	{
		m_strFooter.Empty();
		pFooterListBox->SetWindowText(ResourceString(IDC_ICADPRINTDIA___22, " " ));
		pFooterListBox->SetEditSel(0,-1);
		pFooterListBox->Clear();
	}
}

void prn_Print::OnSpinLineWidth(NMHDR *pNotifyStruct,LRESULT *result)
{
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    *result=0;

    m_nLineWidth-=pNMUpDown->iDelta;
	if(m_nLineWidth<=0)
	{
		m_nLineWidth=1;
	}
	UpdateData(FALSE);
	return;
}

