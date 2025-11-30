/* D:\DEV\PRJ\ICAD\ICADCONFIGDIA.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */


//*********************************************************************
//*  IcadConfigDia.cpp                                                *
//*  Copyright (C) 1994,1995, 1996, 1997 by Visio, Inc.               *
//*                                                                   *
//*********************************************************************

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "cmds.h"/*DNT*/
#include "resource.hm"/*DNT*/
#include "paths.h"

//this is the default auotsave time when the user enables autosave
#define AUTOSAVETIME 30
#define AUTOSAVENAME ".SV$"/*DNT*/

int SDS_VarsToReg(void);

//***This is the dialog class
class cfg_SetVarsCfg:public CDialog {
    public:
        bool    bCurFlag,
                bBkgFlag,
                bBeenThere;
        int     iExpLevel,
                iSaveTime,
                iBkgColor,
                iColorX,
                iColorY,
                iColorZ,
                iPromptMenu,
				iAutoMenuLoad,
                iSave,
                iScrlHist;
        CString csSearchPath,
                csBaseFile,
                csSaveFile;
        CComboBox m_CInterface;
        CScrollBar m_CScroll;
        cfg_SetVarsCfg();  //constructor
        BOOL OnInitDialog();
    protected:
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		afx_msg void OnHelp();
        afx_msg void OnPaint( );
        afx_msg void OnSaveClick( );
        afx_msg void OnSelChange();
        afx_msg void OnSpinSave(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinCom(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void DoDataExchange(CDataExchange *pDX);
        virtual void OnOK( );
        void cfg_Proto();
        void cfg_SetColor();
        void cfg_SetColorX();
        void cfg_SetColorY();
        void cfg_SetColorZ();
        void cfg_SetColorButton();

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(cfg_SetVarsCfg,CDialog)
    ON_NOTIFY(UDN_DELTAPOS,CG_SPINSAVE,OnSpinSave)
    ON_NOTIFY(UDN_DELTAPOS,CG_SPINCOM,OnSpinCom)
    ON_COMMAND(CG_PROTO2,cfg_Proto)
    ON_COMMAND(CG_COLOR2,cfg_SetColor)
    ON_COMMAND(CG_COLOR_BX,cfg_SetColorX)
    ON_COMMAND(CG_COLOR_BY,cfg_SetColorY)
    ON_COMMAND(CG_COLOR_BZ,cfg_SetColorZ)
    ON_CBN_SELCHANGE(CG_INTERFACE,OnSelChange)
    ON_WM_PAINT()
    ON_BN_CLICKED(CG_SAVE,OnSaveClick)
    ON_BN_CLICKED(IDHELP,OnHelp)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL cfg_SetVarsCfg::OnHelpInfo(HELPINFO *pHel){
	OnHelp();
	return(CWnd::OnHelpInfo(pHel));
}

void cfg_SetVarsCfg::OnHelp() {
	CommandRefHelp( m_hWnd, HLP_CONFIG);
}

void cfg_SetVarsCfg::OnSaveClick() {

    UpdateData(true);
    if(iSave) {
        iSaveTime=AUTOSAVETIME;
        csSaveFile=AUTOSAVENAME;
    }else{
        iSaveTime=0;
    }
    UpdateData(false);
}

void cfg_SetVarsCfg::OnSpinSave(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

    UpdateData(TRUE);
    iSaveTime+=(-pNMUpDown->iDelta)*5;
    if(iSaveTime<0) iSaveTime=0;
    UpdateData(FALSE);
    *result=0;
}

void cfg_SetVarsCfg::OnSpinCom(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    UpdateData(TRUE);
    iScrlHist+=(-pNMUpDown->iDelta);
    if(iScrlHist<1) iScrlHist=1;
    UpdateData(FALSE);
    *result=0;
}

void cfg_SetVarsCfg::OnSelChange() {
    iExpLevel=m_CInterface.GetCurSel();
}

void cfg_SetVarsCfg::cfg_SetColor() {
    SDS_GetColorDialog(iBkgColor,&iBkgColor,1);
    ::SendMessage(m_hWnd,WM_PAINT,0,0);
    bBkgFlag=true;
}

void cfg_SetVarsCfg::cfg_SetColorX() {
    SDS_GetColorDialog(iColorX,&iColorX,3);
    ::SendMessage(m_hWnd,WM_PAINT,0,0);
    bCurFlag=true;
}

void cfg_SetVarsCfg::cfg_SetColorY() {
    SDS_GetColorDialog(iColorY,&iColorY,3);
    ::SendMessage(m_hWnd,WM_PAINT,0,0);
    bCurFlag=true;
}

void cfg_SetVarsCfg::cfg_SetColorZ() {
    SDS_GetColorDialog(iColorZ,&iColorZ,3);
    ::SendMessage(m_hWnd,WM_PAINT,0,0);
    bCurFlag=true;
}

void cfg_SetVarsCfg::OnPaint() {
    CDialog::OnPaint();

    HWND hItemWnd;
    HDC hDC;
    CBrush brush;
    RECT rect;
	HPALETTE savpal;
    struct resbuf rb;
    SDS_getvar(NULL,DB_QBKGCOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

    //Background color
    hItemWnd=::GetDlgItem(m_hWnd,CG_COLOR);
    hDC=::GetDC(hItemWnd);
	savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
//    if(iBkgColor==256){
//        brush.CreateSolidBrush(RGB(0,0,0));
//    }else
    if(rb.resval.rint==iBkgColor){
        brush.CreateSolidBrush(SDS_BrushColorFromACADColor(256));
    }else{
        if(iBkgColor==256){
            brush.CreateSolidBrush(RGB(0,0,0));
        }else{
            brush.CreateSolidBrush(SDS_BrushColorFromACADColor(iBkgColor));
        }
    }
    ::GetClientRect(hItemWnd,&rect);
    ::FillRect(hDC,&rect,(HBRUSH)brush);
    brush.DeleteObject();
	::SelectPalette(hDC,savpal,TRUE);
    ::ReleaseDC(hItemWnd,hDC);

    //X axis color
    hItemWnd=::GetDlgItem(m_hWnd,CG_COLOR_X);
    hDC=::GetDC(hItemWnd);
	savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
    brush.CreateSolidBrush(SDS_BrushColorFromACADColor(iColorX));
    ::GetClientRect(hItemWnd,&rect);
    ::FillRect(hDC,&rect,(HBRUSH)brush);
    brush.DeleteObject();
	::SelectPalette(hDC,savpal,TRUE);
    ::ReleaseDC(hItemWnd,hDC);

    //Y axis color
    hItemWnd=::GetDlgItem(m_hWnd,CG_COLOR_Y);
    hDC=::GetDC(hItemWnd);
	savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
    brush.CreateSolidBrush((iColorY==256)?RGB(0,0,0):SDS_BrushColorFromACADColor(iColorY));
    ::GetClientRect(hItemWnd,&rect);
    ::FillRect(hDC,&rect,(HBRUSH)brush);
    brush.DeleteObject();
	::SelectPalette(hDC,savpal,TRUE);
    ::ReleaseDC(hItemWnd,hDC);

    //Z axis color
    hItemWnd=::GetDlgItem(m_hWnd,CG_COLOR_Z);
    hDC=::GetDC(hItemWnd);
	savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
    brush.CreateSolidBrush((iColorZ==256)?RGB(0,0,0):SDS_BrushColorFromACADColor(iColorZ));
    ::GetClientRect(hItemWnd,&rect);
    ::FillRect(hDC,&rect,(HBRUSH)brush);
    brush.DeleteObject();
	::SelectPalette(hDC,savpal,TRUE);
    ::ReleaseDC(hItemWnd,hDC);
}

BOOL cfg_SetVarsCfg::OnInitDialog() {
    if(iSaveTime) {
        iSave=1;
    }else{
        iSave=0;
    }

    BOOL ret=CDialog::OnInitDialog();

    if(SDS_CURDWG==NULL){
        ::EnableWindow(::GetDlgItem(m_hWnd,CG_COLOR_BX),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,CG_COLOR_BY),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,CG_COLOR_BZ),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,CG_COLOR2),false);
    }
//    if(iSaveTime) {
//        ::EnableWindow(::GetDlgItem(m_hWnd,CG_SAVEFILE1),true);
//        ::EnableWindow(::GetDlgItem(m_hWnd,CG_SAVETIME2),true);
//        ::EnableWindow(::GetDlgItem(m_hWnd,CG_SPINSAVE),true);
//    }else{
//        ::EnableWindow(::GetDlgItem(m_hWnd,CG_SAVEFILE1),false);
//        ::EnableWindow(::GetDlgItem(m_hWnd,CG_SAVETIME2),false);
///        ::EnableWindow(::GetDlgItem(m_hWnd,CG_SPINSAVE),false);
//    }

    m_CInterface.SetCurSel(iExpLevel);

    return(ret);
}


void cfg_SetVarsCfg::OnOK(){
    struct resbuf rb;

    CDialog::OnOK();

    rb.restype=RTSHORT;

    rb.resval.rint=iScrlHist;
    sds_setvar("SCRLHIST"/*DNT*/,&rb);
	rb.resval.rint=iBkgColor;
	sds_setvar("BKGCOLOR"/*DNT*/,&rb);

    rb.resval.rint=iColorX;
    sds_setvar("COLORX"/*DNT*/,&rb);
    rb.resval.rint=iColorY;
    sds_setvar("COLORY"/*DNT*/,&rb);
    rb.resval.rint=iColorZ;
    sds_setvar("COLORZ"/*DNT*/,&rb);
    rb.resval.rint=iSaveTime;
    sds_setvar("SAVETIME"/*DNT*/,&rb);
    rb.resval.rint=iPromptMenu;
    sds_setvar("PROMPTMENU"/*DNT*/,&rb);
    rb.resval.rint=iAutoMenuLoad;
    sds_setvar("AUTOMENULOAD"/*DNT*/,&rb);

    sds_getvar("EXPLEVEL"/*DNT*/,&rb);
    if(rb.resval.rint!=iExpLevel+1){
        rb.resval.rint=iExpLevel+1;
        sds_setvar("EXPLEVEL"/*DNT*/,&rb);
    }

    rb.restype=RTSTR;

    rb.resval.rstring= new char [csSearchPath.GetLength()+1];
    strcpy(rb.resval.rstring,(LPCTSTR)csSearchPath);
    for(int i=0;i<csSearchPath.GetLength();i++) {
        if(rb.resval.rstring[i]==',') rb.resval.rstring[i]=';';
    }
    sds_setvar("SRCHPATH"/*DNT*/,&rb);
    if(rb.resval.rstring) IC_FREE(rb.resval.rstring);

    rb.resval.rstring= new char [csBaseFile.GetLength()+1];
    strcpy(rb.resval.rstring,(LPCTSTR)csBaseFile);
    sds_setvar("BASEFILE"/*DNT*/,&rb);
    if(rb.resval.rstring) IC_FREE(rb.resval.rstring);

    rb.resval.rstring= new char [csSaveFile.GetLength()+1];
    strcpy(rb.resval.rstring,(LPCTSTR)csSaveFile);
    sds_setvar("SAVEFILE"/*DNT*/,&rb);
    if(rb.resval.rstring) IC_FREE(rb.resval.rstring);

	SDS_VarsToReg();
}

//***This is the DDX function
void cfg_SetVarsCfg::DoDataExchange(CDataExchange *pDX) {
    DDX_Text(pDX,CG_APPSEARCH1,csSearchPath);
    DDX_Text(pDX,CG_PROTO,csBaseFile);
    DDX_Text(pDX,CG_SAVEFILE1,csSaveFile);
//    DDV_MaxChars(pDX,csSaveFile,3);
    DDX_Control(pDX,CG_INTERFACE,m_CInterface);
    DDX_Check(pDX,CG_PMENU,iPromptMenu);
    DDX_Check(pDX,CG_AUTOMENU,iAutoMenuLoad);
    DDX_Text(pDX,CG_COMMAND,iScrlHist);
    DDV_MinMaxInt(pDX,iScrlHist,1,SHRT_MAX);
    DDX_Text(pDX,CG_SAVETIME2,iSaveTime);
    DDV_MinMaxInt(pDX,iSaveTime,0,SHRT_MAX);
    if(iSaveTime==0){
        ::EnableWindow(::GetDlgItem(m_hWnd,CG_SAVEFILE1),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,CG_SAVETIME2),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,CG_SPINSAVE),false);
        iSave=0;
    }else{
        ::EnableWindow(::GetDlgItem(m_hWnd,CG_SAVEFILE1),true);
        ::EnableWindow(::GetDlgItem(m_hWnd,CG_SAVETIME2),true);
        ::EnableWindow(::GetDlgItem(m_hWnd,CG_SPINSAVE),true);
    }
    DDX_Check(pDX,CG_SAVE,iSave);
}

void cfg_SetVarsCfg::cfg_Proto() {
    struct resbuf rb;
    char title[24],*def;
    int flags=2; 	// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2

    UpdateData(TRUE);
    strcpy(title,ResourceString(IDC_ICADCONFIGDIA_SPECIFY_P_14, "Specify Prototype Drawing" ));
	def= new char [csBaseFile.GetLength()+1];
    strcpy(def,(LPCTSTR)csBaseFile);

    if(sds_getfiled(title,def,ResourceString(IDC_ICADCONFIGDIA_STANDARD__15, "Standard Drawing File|dwg" ),flags,&rb)==RTNORM){
        csBaseFile=rb.resval.rstring;
        UpdateData(FALSE);  //initialize data

        if(rb.resval.rstring) IC_FREE(rb.resval.rstring);
        if(def) IC_FREE(def);
    }
}


//***This is the constructor
cfg_SetVarsCfg::cfg_SetVarsCfg():CDialog(CG_CONFIG) {
    struct resbuf rb;

    bBeenThere=false;
    sds_getvar("SRCHPATH"/*DNT*/,&rb);
    csSearchPath=rb.resval.rstring;
    if(rb.resval.rstring) IC_FREE(rb.resval.rstring);

    sds_getvar("BASEFILE"/*DNT*/,&rb);
    csBaseFile=rb.resval.rstring;
    if(rb.resval.rstring) IC_FREE(rb.resval.rstring);

    sds_getvar("SAVEFILE"/*DNT*/,&rb);
    csSaveFile=rb.resval.rstring;
    if(rb.resval.rstring) IC_FREE(rb.resval.rstring);

    sds_getvar("SCRLHIST"/*DNT*/,&rb); iScrlHist=rb.resval.rint;
    sds_getvar("EXPLEVEL"/*DNT*/,&rb); iExpLevel=rb.resval.rint-1;
    sds_getvar("BKGCOLOR"/*DNT*/,&rb); iBkgColor=rb.resval.rint;
    sds_getvar("COLORX"/*DNT*/,&rb); iColorX=rb.resval.rint;
    sds_getvar("COLORY"/*DNT*/,&rb); iColorY=rb.resval.rint;
    sds_getvar("COLORZ"/*DNT*/,&rb); iColorZ=rb.resval.rint;
    sds_getvar("PROMPTMENU"/*DNT*/,&rb); iPromptMenu=rb.resval.rint;
	sds_getvar("AUTOMENULOAD"/*DNT*/,&rb); iAutoMenuLoad=rb.resval.rint;
    sds_getvar("SAVETIME"/*DNT*/,&rb); iSaveTime=rb.resval.rint;
    bCurFlag=bBkgFlag=false;

}

#include "OptionsPropSheet.h"


