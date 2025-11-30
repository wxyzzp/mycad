/* D:\DEV\PRJ\ICAD\ICADNEWWIZ.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

//*********************************************************************
//*  IcadNewWiz.cpp                                                   *
//*  Copyright (C) 1994,1995, 1996, 1997 by Visio, Inc.               *
//*                                                                   *
//*********************************************************************

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadNewWiz.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "resource.hm"/*DNT*/
#include "paths.h"
#include "IcadView.h"

//Globals to this file
int iLunits,iAunits;




//This is for the first wizard page
BOOL CIcadNewWiz1::OnSetActive() {
	m_pToSheet->SetWizardButtons(PSWIZB_NEXT);
	return (TRUE);
}

BEGIN_MESSAGE_MAP(CIcadNewWiz1,CPropertyPage)
END_MESSAGE_MAP()

void CIcadNewWiz1::DoDataExchange(CDataExchange *pDX) {
    DDX_Radio(pDX,NEW_WIZ1_TMPL,iNewDwg);
}

LRESULT CIcadNewWiz1::OnWizardNext() {
    UpdateData(TRUE);
    if(iNewDwg) {
        return(IDD_WIZNEW3);
    }else{
        return(0);
    }
}

CIcadNewWiz1::CIcadNewWiz1():CPropertyPage(IDD_WIZNEW1) {
    iNewDwg=0;
}



//This is for the second wizard page
BOOL CIcadNewWiz2::OnSetActive() {
	m_pToSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	return (TRUE);
}

BEGIN_MESSAGE_MAP(CIcadNewWiz2,CPropertyPage)
    ON_COMMAND(NEW_WIZ2_BROWSE,OnBrowse)
END_MESSAGE_MAP()

void CIcadNewWiz2::OnBrowse() {
    CString szTitle=ResourceString(IDC_ICADNEWWIZ_OPEN_TEMPLATE_0, "Open Template" );
    CString szExt=ResourceString(DNT_ICADNEWWIZ_DWG_1, "dwt,dwg" );
    struct resbuf rb;

	// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
	// always go to the template path, even if the basefile is not in that directory
    if(sds_getfiled(szTitle,m_csTemplatePath,szExt,2,&rb)==RTNORM) {
        m_csDwgFile=rb.resval.rstring;
        UpdateData(FALSE);
        if(rb.resval.rstring) IC_FREE(rb.resval.rstring);
    }
}

void CIcadNewWiz2::DoDataExchange(CDataExchange *pDX) {
    DDX_Text(pDX,NEW_WIZ2_DRAWING,m_csDwgFile);
}

LRESULT CIcadNewWiz2::OnWizardBack() {
    return(0);
}

//4M Item:78->
// Command newwiz did not reeinit Lisp
short cmd_OpenNewDrawing(char *fname);
//<-4M Item:78
BOOL CIcadNewWiz2::OnWizardFinish() {
	CString csOpen;
	char szTmp[IC_ACADBUF],szOldBase[IC_ACADBUF];
    struct resbuf rb;

	((CEdit*)GetDlgItem(NEW_WIZ2_DRAWING))->GetLine(0, szTmp, IC_ACADBUF);
	m_csDwgFile = szTmp;

	if(sds_findfile(m_csDwgFile,szTmp)!=RTNORM) 
	{
		CString msg;
		msg.Format(ResourceString(IDC_ICADNEWWIZ_TEMPLATENOTFOUND, "\nTemplate drawing %s is not on file. Using defaults.\n"), m_csDwgFile), 
		sds_printf(msg);

		struct resbuf rb;

		cmd_OpenNewDrawing(NULL);

		UpdateData(TRUE);

		rb.restype=RTSTR;
		rb.resval.rstring=ic_colorstr(256,NULL);
		sds_setvar("CECOLOR"/*DNT*/,&rb);

		rb.resval.rstring="BYLAYER"/*DNT*/;
		sds_setvar("CELTYPE"/*DNT*/,&rb);

		rb.restype=RTSHORT;
		rb.resval.rint=2;
		sds_setvar("LUNITS"/*DNT*/,&rb);
		if(rb.resval.rint>=4){
			rb.resval.rint+=2;
		}
		sds_setvar("DIMUNIT"/*DNT*/,&rb);
		rb.resval.rint=0;
		sds_setvar("AUNITS"/*DNT*/,&rb);
		sds_setvar("DIMAUNIT"/*DNT*/,&rb);
		rb.resval.rint=0;
		sds_setvar("BLIPMODE"/*DNT*/,&rb);
		rb.resval.rint=0;
		sds_setvar("GRIDMODE"/*DNT*/,&rb);
		rb.resval.rint=1;
		sds_setvar("UCSICON"/*DNT*/,&rb);
		rb.resval.rint=0;
		sds_setvar("SNAPMODE"/*DNT*/,&rb);

		SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);

		goto bail;
	}

	sds_getvar("BASEFILE"/*DNT*/,&rb);
	strcpy(szOldBase,rb.resval.rstring);
    if(rb.resval.rstring) IC_FREE(rb.resval.rstring);

	rb.resval.rstring=szTmp;
	sds_setvar("BASEFILE"/*DNT*/,&rb);

//4M Item:78->
// Command newwiz did not reeinit Lisp
/*
	m_pMain->IcadWndAction(ICAD_WNDACTION_FILENEW);
*/
   cmd_OpenNewDrawing(NULL);
//<-4M Item:78
    rb.resval.rstring=szOldBase;
    sds_setvar("BASEFILE"/*DNT*/,&rb);
bail:

	return(CPropertyPage::OnWizardFinish());
}

CIcadNewWiz2::CIcadNewWiz2():CPropertyPage(IDD_WIZNEW2) {
    struct resbuf rb;

    sds_getvar("BASEFILE"/*DNT*/,&rb);
    m_csDwgFile=rb.resval.rstring;
    if(rb.resval.rstring) IC_FREE(rb.resval.rstring);


	// Get the path to the template files
	CMainWindow *mainWnd = SDS_CMainWindow;
	m_csTemplatePath = m_csDwgFile;
	if (mainWnd && mainWnd->m_paths && mainWnd->m_paths->m_templatesPath)
	{
		// There may be mulitple template paths so just grab the first one
		FilePath cFirstPath(mainWnd->m_paths->m_templatesPath);
		m_csTemplatePath.Format("%s%s", cFirstPath.GetFirstPath(), "\\");
	}
}



//This is for the third wizard page
BOOL CIcadNewWiz3::OnSetActive() {
	m_pToSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	return (TRUE);
}

BEGIN_MESSAGE_MAP(CIcadNewWiz3,CPropertyPage)
END_MESSAGE_MAP()

void CIcadNewWiz3::DoDataExchange(CDataExchange *pDX) {
}

LRESULT CIcadNewWiz3::OnWizardBack() {
    return(IDD_WIZNEW1);
}

LRESULT CIcadNewWiz3::OnWizardNext() {
    return(0);
}

CIcadNewWiz3::CIcadNewWiz3():CPropertyPage(IDD_WIZNEW3) {
}

//This is for the fourth wizard page
BOOL CIcadNewWiz4::OnSetActive() {
	m_pToSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	return (TRUE);
}

BEGIN_MESSAGE_MAP(CIcadNewWiz4,CPropertyPage)
END_MESSAGE_MAP()

void CIcadNewWiz4::DoDataExchange(CDataExchange *pDX) {
    DDX_Radio(pDX,NEW_WIZ4_SCI,iLunits);
}

LRESULT CIcadNewWiz4::OnWizardBack() {
    return(0);
}

LRESULT CIcadNewWiz4::OnWizardNext() {
    UpdateData(TRUE);
    return(0);
}

CIcadNewWiz4::CIcadNewWiz4():CPropertyPage(IDD_WIZNEW4) {
    iLunits=1;
}


//This is for the fifth wizard page
BOOL CIcadNewWiz5::OnSetActive() {
	m_pToSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	return (TRUE);
}

BEGIN_MESSAGE_MAP(CIcadNewWiz5,CPropertyPage)
END_MESSAGE_MAP()

void CIcadNewWiz5::DoDataExchange(CDataExchange *pDX) {
    DDX_Radio(pDX,NEW_WIZ5_DEG,iAunits);
}

LRESULT CIcadNewWiz5::OnWizardBack() {
    return(0);
}

LRESULT CIcadNewWiz5::OnWizardNext() {
    UpdateData(TRUE);
    return(0);
}

CIcadNewWiz5::CIcadNewWiz5():CPropertyPage(IDD_WIZNEW5) {
    iAunits=0;
}


//This is for the sixth wizard page

BOOL CIcadNewWiz6::OnSetActive() {
	m_pToSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	return(TRUE);
}

BEGIN_MESSAGE_MAP(CIcadNewWiz6,CPropertyPage)
    ON_COMMAND(NEW_WIZ6_COLOR,OnColor)
    ON_WM_PAINT()
END_MESSAGE_MAP()

void CIcadNewWiz6::OnColor() {
    SDS_GetColorDialog(iColor,&iColor,0);
    ::SendMessage(m_hWnd,WM_PAINT,0,0);
}

void CIcadNewWiz6::OnPaint() {
    CPropertyPage::OnPaint();
    PaintIt();
}

void CIcadNewWiz6::PaintIt() {
    HBITMAP hBitmap,hOldBitmap;
    HWND hItemWnd;
    HDC hDC,hCmpDC;
    CBrush brush;
    RECT rect;
   	HPALETTE savpal;

    hItemWnd=::GetDlgItem(m_hWnd,NEW_WIZ6_CUR_COLOR);
    hDC=::GetDC(hItemWnd);
    if(iColor==0) {
        hBitmap=::LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EC_COLOR_BMP));
        hCmpDC=::CreateCompatibleDC(hDC);
        ::GetClientRect(hItemWnd,&rect);
		hOldBitmap=(HBITMAP)::SelectObject(hCmpDC,hBitmap);
        ::StretchBlt(hDC,rect.left+1,rect.top+1,rect.right-1,rect.bottom-1,hCmpDC,0,0,71,21,SRCCOPY);
		::SelectObject(hCmpDC,hOldBitmap);
		::DeleteObject(hBitmap);
        ::DeleteDC(hCmpDC);
        ::ReleaseDC(hItemWnd,hDC);
    }else if(iColor==256) {
        hBitmap=::LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EC_COLOR_BMP256));
        hCmpDC=::CreateCompatibleDC(hDC);
        ::GetClientRect(hItemWnd,&rect);
		hOldBitmap=(HBITMAP)::SelectObject(hCmpDC,hBitmap);
        ::StretchBlt(hDC,rect.left+1,rect.top+1,rect.right-1,rect.bottom-1,hCmpDC,0,0,71,21,SRCCOPY);
		::SelectObject(hCmpDC,hOldBitmap);
		::DeleteObject(hBitmap);
        ::DeleteDC(hCmpDC);
        ::ReleaseDC(hItemWnd,hDC);
    }else if(iColor>0 && iColor<256){
       	savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
        brush.CreateSolidBrush(SDS_BrushColorFromACADColor(iColor));
        ::GetClientRect(hItemWnd,&rect);
        rect.left+=1;rect.top+=1;rect.right-=1;rect.bottom-=1;
        ::FillRect(hDC,&rect,(HBRUSH)brush);
        brush.DeleteObject();
//	    ::SelectPalette(hDC,savpal,TRUE);
        ::ReleaseDC(hItemWnd,hDC);
    }
}

void CIcadNewWiz6::DoDataExchange(CDataExchange *pDX) {
    DDX_Control(pDX,NEW_WIZ6_LINETYPE,m_CLinetype);
    DDX_Check(pDX,NEW_WIZ6_GRID,iGrid);
    DDX_Check(pDX,NEW_WIZ6_SNAP,iSnap);
    DDX_Check(pDX,NEW_WIZ6_UCS,iUcs);
    DDX_Check(pDX,NEW_WIZ6_BLIP,iBlip);
}

BOOL CIcadNewWiz6::OnInitDialog() {
    CPropertyPage::OnInitDialog();

    if(!DoOnce){
        m_CLinetype.AddString(ResourceString(IDC_ICADNEWWIZ_BYLAYER_3, "BYLAYER" ));
        m_CLinetype.AddString(ResourceString(IDC_ICADNEWWIZ_BYBLOCK_4, "BYBLOCK" ));
        m_CLinetype.AddString(ResourceString(IDC_ICADNEWWIZ_CONTINUOUS_5, "CONTINUOUS" ));
        m_CLinetype.SetCurSel(0);
    }
    return(TRUE);
}

LRESULT CIcadNewWiz6::OnWizardBack() {
    return(0);
}

BOOL CIcadNewWiz6::OnWizardFinish() {
    struct resbuf rb;

	cmd_OpenNewDrawing(NULL);

    UpdateData(TRUE);

    rb.restype=RTSTR;
    rb.resval.rstring=ic_colorstr(iColor,NULL);
    sds_setvar("CECOLOR"/*DNT*/,&rb);

    switch(m_CLinetype.GetCurSel()) {
        case 0:
            rb.resval.rstring="BYLAYER"/*DNT*/;
            break;
        case 1:
            rb.resval.rstring="BYBLOCK"/*DNT*/;
            break;
        case 2:
            rb.resval.rstring="CONTINUOUS"/*DNT*/;
            break;
    }
    sds_setvar("CELTYPE"/*DNT*/,&rb);

    rb.restype=RTSHORT;
    rb.resval.rint=iLunits+1;
    sds_setvar("LUNITS"/*DNT*/,&rb);
    if(rb.resval.rint>=4){
        rb.resval.rint+=2;
    }
    sds_setvar("DIMUNIT"/*DNT*/,&rb);
    rb.resval.rint=iAunits;
    sds_setvar("AUNITS"/*DNT*/,&rb);
    sds_setvar("DIMAUNIT"/*DNT*/,&rb);
    rb.resval.rint=iBlip;
    sds_setvar("BLIPMODE"/*DNT*/,&rb);
    rb.resval.rint=iGrid;
    sds_setvar("GRIDMODE"/*DNT*/,&rb);
    rb.resval.rint=iUcs;
    sds_setvar("UCSICON"/*DNT*/,&rb);
    rb.resval.rint=iSnap;
    sds_setvar("SNAPMODE"/*DNT*/,&rb);

    SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);

    return(CPropertyPage::OnWizardFinish());
}

CIcadNewWiz6::CIcadNewWiz6():CPropertyPage(IDD_WIZNEW6) {
    iBlip=iSnap=iGrid=DoOnce=0;
    iUcs=1;
    iColor=256;
}



//This is for the main wizard sheet
BOOL CIcadNewWizard::OnInitDialog() {
    m_NewWiz1.m_pToSheet=m_NewWiz2.m_pToSheet=m_NewWiz3.m_pToSheet=m_NewWiz4.m_pToSheet=m_NewWiz5.m_pToSheet=m_NewWiz6.m_pToSheet=this;
    return(CPropertySheet::OnInitDialog());
}

BEGIN_MESSAGE_MAP(CIcadNewWizard,CPropertySheet)
    ON_BN_CLICKED(IDHELP,OnHelp)
END_MESSAGE_MAP()

void CIcadNewWizard::OnHelp() {
	CommandRefHelp( m_hWnd, HLP_NEWWIZ);
}

CIcadNewWizard::CIcadNewWizard():CPropertySheet() {
    AddPage(&m_NewWiz1);
    AddPage(&m_NewWiz2);
    AddPage(&m_NewWiz3);
    AddPage(&m_NewWiz4);
    AddPage(&m_NewWiz5);
    AddPage(&m_NewWiz6);
}

//This is the function call from the main
void CMainWindow::NewDwgWizard(void) {
	CIcadNewWizard NewWizard;

    NewWizard.m_psh.dwFlags|=PSH_HASHELP|PSH_WIZARD ;
	NewWizard.m_NewWiz1.m_psp.dwFlags|=PSP_HASHELP;
	NewWizard.m_NewWiz2.m_psp.dwFlags|=PSP_HASHELP;
	NewWizard.m_NewWiz3.m_psp.dwFlags|=PSP_HASHELP;
	NewWizard.m_NewWiz4.m_psp.dwFlags|=PSP_HASHELP;
	NewWizard.m_NewWiz5.m_psp.dwFlags|=PSP_HASHELP;
	NewWizard.m_NewWiz6.m_psp.dwFlags|=PSP_HASHELP;

    NewWizard.m_NewWiz2.m_pMain=this;
    NewWizard.DoModal();

}





