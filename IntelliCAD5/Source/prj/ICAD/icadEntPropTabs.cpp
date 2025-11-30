// File  : <DevDir>\source\prj\icad\icadEntPropTabs.cpp


//Modified Cybage MM 24-09-2001 DD-MM-YYYY [
//Reason : Bug No.- 77847 from Bugzilla
#include "configure.h"
//Modified Cybage MM 24-09-2001 DD-MM-YYYY ]
#include "icadEntPropTabs.h"
//EBATECH(CNBR) 2002/4/24 for SHAPE
#include "styletabrec.h"

//Globals
extern double*			entprop_plextru;
extern cmd_elistll*	pVert;
extern int  num_of_splines;
extern int  num_of_leaders;
extern bool ent_bHitEnter;
extern bool bPlineIsMesh;
extern bool bPlineIs3D;
extern ent_DlgItem	ent_DlgItems[];
extern short ent_num_of_vars;

//////////////////////////////////////////////////////////////////////////////
// 3DFACE     ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_3DFACE_TAB,CDialog)
    ON_BN_CLICKED(FACE_PICK1,ent_OnPick1)
    ON_BN_CLICKED(FACE_PICK2,ent_OnPick2)
    ON_BN_CLICKED(FACE_PICK3,ent_OnPick3)
    ON_BN_CLICKED(FACE_PICK4,ent_OnPick4)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_FACE_P1X,OnSpinPt1x)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_FACE_P1Y,OnSpinPt1y)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_FACE_P1Z,OnSpinPt1z)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_FACE_P2X,OnSpinPt2x)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_FACE_P2Y,OnSpinPt2y)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_FACE_P2Z,OnSpinPt2z)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_FACE_P3X,OnSpinPt3x)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_FACE_P3Y,OnSpinPt3y)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_FACE_P3Z,OnSpinPt3z)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_FACE_P4X,OnSpinPt4x)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_FACE_P4Y,OnSpinPt4y)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_FACE_P4Z,OnSpinPt4z)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL
ent_3DFACE_TAB::OnHelpInfo
(
 HELPINFO*	pHel
)
{
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void
ent_3DFACE_TAB::OnOK()
{
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void
ent_3DFACE_TAB::OnCancel()
{
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void
ent_3DFACE_TAB::OnSpinPt1x
(
 NMHDR*		pNotifyStruct,
 LRESULT*	result
)
{
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,FACE_P1X),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void
ent_3DFACE_TAB::OnSpinPt1y
(
 NMHDR*		pNotifyStruct,
 LRESULT*	result
)
{
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,FACE_P1Y),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void
ent_3DFACE_TAB::OnSpinPt1z
(
 NMHDR*		pNotifyStruct,
 LRESULT*	result
)
{
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,FACE_P1Z),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void
ent_3DFACE_TAB::OnSpinPt2x
(
 NMHDR*		pNotifyStruct,
 LRESULT*	result
)
{
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,FACE_P2X),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void
ent_3DFACE_TAB::OnSpinPt2y
(
 NMHDR*		pNotifyStruct,
 LRESULT*	result
)
{
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,FACE_P2Y),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void
ent_3DFACE_TAB::OnSpinPt2z
(
 NMHDR*		pNotifyStruct,
 LRESULT*	result
)
{
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,FACE_P2Z),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void
ent_3DFACE_TAB::OnSpinPt3x
(
 NMHDR*		pNotifyStruct,
 LRESULT*	result
)
{
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,FACE_P3X),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void
ent_3DFACE_TAB::OnSpinPt3y
(
 NMHDR*		pNotifyStruct,
 LRESULT*	result
)
{
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,FACE_P3Y),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void
ent_3DFACE_TAB::OnSpinPt3z
(
 NMHDR*		pNotifyStruct,
 LRESULT*	result
)
{
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,FACE_P3Z),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void
ent_3DFACE_TAB::OnSpinPt4x
(
 NMHDR*		pNotifyStruct,
 LRESULT*	result
)
{
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,FACE_P4X),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void
ent_3DFACE_TAB::OnSpinPt4y
(
 NMHDR*		pNotifyStruct,
 LRESULT*	result
)
{
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,FACE_P4Y),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void
ent_3DFACE_TAB::OnSpinPt4z
(
 NMHDR*		pNotifyStruct,
 LRESULT*	result
)
{
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,FACE_P4Z),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

BOOL
ent_3DFACE_TAB::OnInitDialog()
{
    return(CDialog::OnInitDialog());
}

void
ent_3DFACE_TAB::ent_OnPick1()
{
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick1=1;
    ::SendMessage(hWnd,WM_CLOSE,FACE_PICK1,0);
}

void
ent_3DFACE_TAB::ent_OnPick2()
{
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick2=1;
    ::SendMessage(hWnd,WM_CLOSE,FACE_PICK2,0);
}

void
ent_3DFACE_TAB::ent_OnPick3()
{
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick3=1;
    ::SendMessage(hWnd,WM_CLOSE,FACE_PICK3,0);
}

void
ent_3DFACE_TAB::ent_OnPick4()
{
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick4=1;
    ::SendMessage(hWnd,WM_CLOSE,FACE_PICK4,0);
}

void
ent_3DFACE_TAB::DoDataExchange
(
 CDataExchange*	pDX
)
{
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_3DFACE) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end].EntTyp==DB_3DFACE) && (end<ent_num_of_vars);end++);
    end--;

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

ent_3DFACE_TAB::ent_3DFACE_TAB() : CDialog(ENT_3DFACE_TAB)
{
    ent_Pick1=ent_Pick2=ent_Pick3=ent_Pick4=0;
}


//////////////////////////////////////////////////////////////////////////////
// ARC        ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_ARC_TAB,CDialog)
    ON_BN_CLICKED(ARC_PICK2,ent_OnPick)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ARCRAD,OnSpinArcRad)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ARCSANG,OnSpinArcSAng)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ARCEANG,OnSpinArcEAng)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ARC_CPX,OnSpinArcCpx)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ARC_CPY,OnSpinArcCpy)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ARC_CPZ,OnSpinArcCpz)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL ent_ARC_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_ARC_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_ARC_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_ARC_TAB::OnSpinArcCpx(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ARC_CPX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ARC_TAB::OnSpinArcCpy(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ARC_CPY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ARC_TAB::OnSpinArcCpz(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ARC_CPZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ARC_TAB::OnSpinArcRad(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ARC_RADIUS),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ARC_TAB::OnSpinArcSAng(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterAngle(::GetDlgItem(m_hWnd,ARC_STARTANGLE),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ARC_TAB::OnSpinArcEAng(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterAngle(::GetDlgItem(m_hWnd,ARC_ENDANGLE),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

BOOL ent_ARC_TAB::OnInitDialog() {
    return(CDialog::OnInitDialog());
}

void ent_ARC_TAB::ent_OnPick() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick=1;
    ::SendMessage(hWnd,WM_CLOSE,ARC_PICK2,0);
}

void ent_ARC_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_ARC) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_ARC) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

ent_ARC_TAB::ent_ARC_TAB() : CDialog(ENT_ARC_TAB) {
    ent_Pick=0;
}


//////////////////////////////////////////////////////////////////////////////
// ATTDEF     ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_ATTDEF_TAB,CDialog)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTD_SPX,OnSpinAttdSpx)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTD_SPY,OnSpinAttdSpy)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTD_SPZ,OnSpinAttdSpz)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTD_APX,OnSpinAttdApx)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTD_APY,OnSpinAttdApy)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTD_APZ,OnSpinAttdApz)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTD_HEIGHT,OnSpinAttdHth)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTD_ROTATE,OnSpinAttdRot)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTD_THICK,OnSpinAttdTck)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTD_SCALE,OnSpinAttdScl)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTD_OBLIQUE,OnSpinAttdObl)
    ON_BN_CLICKED(ATTD_PICK1,ent_OnPick1)
    ON_BN_CLICKED(ATTD_PICK2,ent_OnPick2)
    ON_CBN_SELCHANGE(ATTD_JUSTIFY,OnSelChange)
    ON_CBN_SELCHANGE(ATTD_STYLE,OnStyleChange)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL ent_ATTDEF_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_ATTDEF_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_ATTDEF_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}


void ent_ATTDEF_TAB::OnSpinAttdSpx(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTD_SPX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTDEF_TAB::OnSpinAttdSpy(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTD_SPY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTDEF_TAB::OnSpinAttdSpz(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTD_SPZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTDEF_TAB::OnSpinAttdApx(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTD_APX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTDEF_TAB::OnSpinAttdApy(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTD_APY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTDEF_TAB::OnSpinAttdApz(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTD_APZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTDEF_TAB::OnSpinAttdHth(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTD_HEIGHT),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTDEF_TAB::OnSpinAttdRot(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterAngle(::GetDlgItem(m_hWnd,ATTD_ROTATE),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTDEF_TAB::OnSpinAttdTck(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTD_THICK),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTDEF_TAB::OnSpinAttdScl(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
	//Modified Cybage SBD	04/03/2002 DD/MM/YYYY
	//Reason : Fix for Bug No. 77983 from Bugzilla
	//ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTD_SCALE),pNMUpDown->iDelta);
	ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTD_SCALE),pNMUpDown->iDelta,2);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTDEF_TAB::OnSpinAttdObl(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterAngle(::GetDlgItem(m_hWnd,ATTD_OBLIQUE),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

BOOL ent_ATTDEF_TAB::OnInitDialog() {
	FillCombo();
    ent_StyleChoice();
    CDialog::OnInitDialog();
	// Bugzilla No. 76186; 11/11/2002 [
	//Current selection is  "Varies"...So add into ComboBox now
	if ( iTxtCurSel == 15)
	{
		CComboBox *pCombo = NULL;
		if (NULL != (pCombo = (CComboBox *) GetDlgItem(ATTD_JUSTIFY)))
		{
			int index = pCombo->AddString(ResourceString(IDC_ICADENTPROP_VARIES_18, "Varies" ));
			pCombo->SetCurSel(index); 

		}
	}
	// Bugzilla No. 76186; 11/11/2002 ]
    OnSelChange();
    return(TRUE);
}

void ent_ATTDEF_TAB::FillCombo()
{
	CComboBox *pCombo = NULL;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(ATTD_JUSTIFY)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_3);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_4);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_5);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_6);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_7);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_8);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_9);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_10);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_11);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_12);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_13);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_14);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_15);
	pCombo->AddString(comboOption);
	// Bugzilla No. 76186; 11/11/2002 
	// Add "Varies only if necessary"
	//comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_16);
	//pCombo->AddString(comboOption);
}

void ent_ATTDEF_TAB::ent_OnPick1() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick1=1;
    ::SendMessage(hWnd,WM_CLOSE,ATTD_PICK1,0);
}

void ent_ATTDEF_TAB::ent_OnPick2() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick2=1;
    ::SendMessage(hWnd,WM_CLOSE,ATTD_PICK2,0);
}

void ent_ATTDEF_TAB::ent_Yes10(){
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_SPX),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_SPY),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_SPZ),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTD_SPX),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTD_SPY),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTD_SPZ),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_PICK1),TRUE);
}

void ent_ATTDEF_TAB::ent_No10(){
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_SPX),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_SPY),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_SPZ),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTD_SPX),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTD_SPY),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTD_SPZ),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_PICK1),FALSE);
}

void ent_ATTDEF_TAB::ent_Yes11(){
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_APX),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_APY),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_APZ),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTD_APX),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTD_APY),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTD_APZ),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_PICK2),TRUE);
}

void ent_ATTDEF_TAB::ent_No11(){
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_APX),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_APY),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_APZ),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTD_APX),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTD_APY),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTD_APZ),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_PICK2),FALSE);
}

void ent_ATTDEF_TAB::ent_Yes40(){
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_HEIGHT),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTD_HEIGHT),TRUE);
}

void ent_ATTDEF_TAB::ent_No40(){
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_HEIGHT),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTD_HEIGHT),FALSE);
}

void ent_ATTDEF_TAB::ent_Yes50(){
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_ROTATE),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTD_ROTATE),TRUE);
}

void ent_ATTDEF_TAB::ent_No50(){
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTD_ROTATE),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTD_ROTATE),FALSE);
}

//Grey out the correct field.
void ent_ATTDEF_TAB::OnSelChange() {
    UpdateData(TRUE);
    if((iJust72&1 && iJust74&1) || iJust72&8 || iJust72&32){
        ent_Yes10();
    }else{
        ent_No10();
    }
    if(iJust72&1 && iJust74&1){
        ent_No11();
    }else{
        ent_Yes11();
    }
    if(iJust72&8){
        ent_No40();
    }else{
        ent_Yes40();
    }
    if(iJust72&8 || iJust72&32){
        ent_No50();
    }else{
        ent_Yes50();
    }
}

//Set current selection
void ent_ATTDEF_TAB::ent_SetSel() {
    switch(iJust72) {
        case 1:
            switch(iJust74) {
                case 1:
                    iTxtCurSel=0;
                    break;
                case 2:
                    iTxtCurSel=1;
                    break;
                case 4:
                    iTxtCurSel=2;
                    break;
                case 8:
                    iTxtCurSel=3;
                    break;
                default:
                    iTxtCurSel=15;
                    break;
            }
            break;
        case 2:
            switch(iJust74) {
                case 1:
                    iTxtCurSel=4;
                    break;
                case 2:
                    iTxtCurSel=5;
                    break;
                case 4:
                    iTxtCurSel=6;
                    break;
                case 8:
                    iTxtCurSel=7;
                    break;
                default:
                    iTxtCurSel=15;
                    break;
            }
            break;
        case 4:
            switch(iJust74) {
                case 1:
                    iTxtCurSel=8;
                    break;
                case 2:
                    iTxtCurSel=9;
                    break;
                case 4:
                    iTxtCurSel=10;
                    break;
                case 8:
                    iTxtCurSel=11;
                    break;
                default:
                    iTxtCurSel=15;
                    break;
            }
            break;
        case 8:
            iTxtCurSel=12;
            break;
        case 16:
            iTxtCurSel=13;
            break;
        case 32:
            iTxtCurSel=14;
            break;
        default:
            iTxtCurSel=15;
            break;
    }
}

//Fill members from the index of the combo box
void ent_ATTDEF_TAB::ent_SelChange() {
    switch(iTxtCurSel) {
        case 0:
            iJust72=1;
            iJust74=1;
            break;
        case 1:
            iJust72=1;
            iJust74=2;
            break;
        case 2:
            iJust72=1;
            iJust74=4;
            break;
        case 3:
            iJust72=1;
            iJust74=8;
            break;
        case 4:
            iJust72=2;
            iJust74=1;
            break;
        case 5:
            iJust72=2;
            iJust74=2;
            break;
        case 6:
            iJust72=2;
            iJust74=4;
            break;
        case 7:
            iJust72=2;
            iJust74=8;
            break;
        case 8:
            iJust72=4;
            iJust74=1;
            break;
        case 9:
            iJust72=4;
            iJust74=2;
            break;
        case 10:
            iJust72=4;
            iJust74=4;
            break;
        case 11:
            iJust72=4;
            iJust74=8;
            break;
        case 12:
            iJust72=8;
            iJust74=1;
            break;
        case 13:
            iJust72=16;
            iJust74=1;
            break;
        case 14:
            iJust72=32;
            iJust74=1;
            break;
        case 15:
            iJust72=63;
            iJust74=15;
            break;
    }
}

//This will read in the styles
//into the drop down list.
void ent_ATTDEF_TAB::ent_StyleChoice() {
    int i;
	CString csTmp;

	::SendMessage(::GetDlgItem(m_hWnd,ATTD_STYLE),CB_RESETCONTENT,0,0);
// Changed by Vitaly Spirin    SDS_StartTblRead(ResourceString(IDC_ICADENTPROP_STYLE_21, "STYLE" ));
    SDS_StartTblRead("STYLE");
	while(SDS_TblReadHasMore()) {
    	csTmp=SDS_TblReadString();
    	if(csTmp.GetLength() == 0) continue; // EBATECH(CNBR) 2002/4/24 STYPE for SHAPE has null NAME.
        if(csTmp.Find('|')>0) continue;
    	::SendMessage(::GetDlgItem(m_hWnd,ATTD_STYLE),CB_ADDSTRING,0,(LPARAM)(LPCTSTR)csTmp);
    }

    for(i=0;ent_DlgItems[i].ID!=ATTD_STYLE;i++);
    if(ent_DlgItems[i].DspValue!=NULL && strisame(ent_DlgItems[i].DspValue,ResourceString(IDC_ICADENTPROP_VARIES_18, "Varies" )))
        ::SendMessage(::GetDlgItem(m_hWnd,ATTD_STYLE),CB_ADDSTRING,0,(LPARAM)ResourceString(IDC_ICADENTPROP_VARIES_18, "Varies" ));
}

void ent_ATTDEF_TAB::OnStyleChange() {
    char szTmp[IC_ACADBUF];
    struct resbuf *pRb,*pRbb=NULL;

    ::GetWindowText(::GetDlgItem(m_hWnd,ATTD_STYLE),szTmp,sizeof(szTmp)-1);
    if((pRbb=sds_tblsearch("STYLE"/*DNT*/,szTmp,0))==NULL) return;
    for(pRb=pRbb;pRb!=NULL;pRb=pRb->rbnext){
        switch(pRb->restype){
            case 40:
                if(!icadRealEqual(pRb->resval.rreal,0.0)){
                    sds_rtos(pRb->resval.rreal,-1,-1,szTmp);
                    ::SetWindowText(::GetDlgItem(m_hWnd,ATTD_HEIGHT),szTmp);
                }
                break;
            case 41:
                sds_rtos(pRb->resval.rreal,-1,-1,szTmp);
                ::SetWindowText(::GetDlgItem(m_hWnd,ATTD_SCALE),szTmp);
                break;
            case 50:
                sds_angtos(pRb->resval.rreal,-1,-1,szTmp);
                ::SetWindowText(::GetDlgItem(m_hWnd,ATTD_OBLIQUE),szTmp);
                break;
            case 71:
                if(pRb->resval.rint & IC_TEXT_GEN_BACKWARD) {
                    ::SendMessage(::GetDlgItem(m_hWnd,ATTD_BACKWARD),BM_SETCHECK,BST_CHECKED,0);
                }else{
                    ::SendMessage(::GetDlgItem(m_hWnd,ATTD_BACKWARD),BM_SETCHECK,BST_UNCHECKED,0);
                }
                if(pRb->resval.rint & IC_TEXT_GEN_UPSIDEDOWN) {
                    ::SendMessage(::GetDlgItem(m_hWnd,ATTD_UPSIDEDOWN),BM_SETCHECK,BST_CHECKED,0);
                }else{
                    ::SendMessage(::GetDlgItem(m_hWnd,ATTD_UPSIDEDOWN),BM_SETCHECK,BST_UNCHECKED,0);
                }
                break;
        }
    }
    IC_RELRB(pRbb);
    UpdateData(true);
}

void ent_ATTDEF_TAB::DoDataExchange(CDataExchange *pDX) {
    int index,iLen,start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_ATTDEF) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_ATTDEF) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RTREAL:
            case RT3DPOINT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                } else { //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    DDV_MaxChars(pDX,csTmp,511);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                } else { //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    DDV_MaxChars(pDX,csTmp,511);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSHORT:
            case RTSTR:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    if(ent_DlgItems[i].ID==ATTD_STYLE) {
                        index=::SendMessage(::GetDlgItem(m_hWnd,ATTD_STYLE),CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)ent_DlgItems[i].DspValue);
                        DDX_CBIndex(pDX,ATTD_STYLE,index);
                    }else{
                        csTmp=ent_DlgItems[i].DspValue;
                        DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    }
                } else { //Retrieving data from the dialog
                    if(ent_DlgItems[i].ID==ATTD_STYLE) {
                        DDX_CBIndex(pDX,ATTD_STYLE,index);
                            iLen=::SendMessage(::GetDlgItem(m_hWnd,ATTD_STYLE),CB_GETLBTEXTLEN,(WPARAM)index,0);
                            ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,iLen+1);
                            ::SendMessage(::GetDlgItem(m_hWnd,ATTD_STYLE),CB_GETLBTEXT,(WPARAM)index,(LPARAM)ent_DlgItems[i].DspValue);
                    }else{
                        DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                        DDV_MaxChars(pDX,csTmp,511);
                        if(csTmp.IsEmpty()) {
                            if(ent_DlgItems[i].DspValue!=NULL){
                                IC_FREE(ent_DlgItems[i].DspValue);
                                ent_DlgItems[i].DspValue=NULL;
                            }
                        }else{
                            ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                            strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                        }
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                } else { //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
            case ENT_JUSTI:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    if(ent_DlgItems[i].DXFCode==72) {
                        iJust72=ent_DlgItems[i].CheckBit;
                    }else if(ent_DlgItems[i].DXFCode==74) {
                        iJust74=ent_DlgItems[i].CheckBit;
                        ent_SetSel();
                        DDX_CBIndex(pDX,ATTD_JUSTIFY,iTxtCurSel);
                    }
                } else { //Retrieving data from the dialog
                    DDX_CBIndex(pDX,ATTD_JUSTIFY,iTxtCurSel);
                    ent_SelChange();
                    if(ent_DlgItems[i].DXFCode==72) {
                        ent_DlgItems[i].CheckBit=iJust72;
                    }else if(ent_DlgItems[i].DXFCode==74) {
                        ent_DlgItems[i].CheckBit=iJust74;
                    }
                }
        }
    }

}

ent_ATTDEF_TAB::ent_ATTDEF_TAB():CDialog(ENT_ATTDEF_TAB) {
    ent_Pick1=ent_Pick2=0;
}


//////////////////////////////////////////////////////////////////////////////
// ATTRIB     ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_ATTRIB_TAB,CDialog)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTR_SPX,OnSpinAttrSpx)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTR_SPY,OnSpinAttrSpy)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTR_SPZ,OnSpinAttrSpz)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTR_APX,OnSpinAttrApx)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTR_APY,OnSpinAttrApy)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTR_APZ,OnSpinAttrApz)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTR_HEIGHT,OnSpinAttrHth)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTR_ROTATE,OnSpinAttrRot)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTR_THICK,OnSpinAttrTck)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTR_SCALE,OnSpinAttrScl)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATTR_OBLIQUE,OnSpinAttrObl)
    ON_BN_CLICKED(ATTR_PICK1,ent_OnPick1)
    ON_BN_CLICKED(ATTR_PICK2,ent_OnPick2)
    ON_CBN_SELCHANGE(ATTR_JUSTIFY,OnSelChange)
    ON_CBN_SELCHANGE(ATTR_STYLE,OnStyleChange)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL ent_ATTRIB_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_ATTRIB_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_ATTRIB_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}


void ent_ATTRIB_TAB::OnSpinAttrSpx(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTR_SPX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTRIB_TAB::OnSpinAttrSpy(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTR_SPY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTRIB_TAB::OnSpinAttrSpz(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTR_SPZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTRIB_TAB::OnSpinAttrApx(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTR_APX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTRIB_TAB::OnSpinAttrApy(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTR_APY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTRIB_TAB::OnSpinAttrApz(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTR_APZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTRIB_TAB::OnSpinAttrHth(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTR_HEIGHT),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTRIB_TAB::OnSpinAttrRot(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterAngle(::GetDlgItem(m_hWnd,ATTR_ROTATE),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTRIB_TAB::OnSpinAttrTck(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTR_THICK),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTRIB_TAB::OnSpinAttrScl(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
	//Modified Cybage SBD	04/03/2002 DD/MM/YYYY
	//Reason : Fix for Bug No. 77983 from Bugzilla
    //ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTR_SCALE),pNMUpDown->iDelta);
	ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATTR_SCALE),pNMUpDown->iDelta,2);
    UpdateData(TRUE);
    *result=0;
}

void ent_ATTRIB_TAB::OnSpinAttrObl(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterAngle(::GetDlgItem(m_hWnd,ATTR_OBLIQUE),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

BOOL ent_ATTRIB_TAB::OnInitDialog() {
	FillCombo();
    ent_StyleChoice();
    CDialog::OnInitDialog();
	// Bugzilla No. 76186; 11/11/2002 [
	//Current selection is  "Varies"...So add into ComboBox now
	if ( iTxtCurSel == 15)
	{
		CComboBox *pCombo = NULL;
		if (NULL != (pCombo = (CComboBox *) GetDlgItem(ATTR_JUSTIFY)))
		{
			int index = pCombo->AddString(ResourceString(IDC_ICADENTPROP_VARIES_18, "Varies" ));
			pCombo->SetCurSel(index); 

		}
	}
	// Bugzilla No. 76186; 11/11/2002 ]
    OnSelChange();
    return(TRUE);
}

void ent_ATTRIB_TAB::FillCombo()
{
	CComboBox *pCombo = NULL;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(ATTR_JUSTIFY)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_3);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_4);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_5);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_6);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_7);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_8);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_9);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_10);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_11);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_12);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_13);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_14);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_15);
	pCombo->AddString(comboOption);
	// Bugzilla No. 76186; 11/11/2002 
	// Add "Varies only if necessary"
	//comboOption.LoadString(IDC_COMBO_ATTD_JUSTIFY_16);
	//pCombo->AddString(comboOption);
}

void ent_ATTRIB_TAB::ent_OnPick1() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick1=1;
    ::SendMessage(hWnd,WM_CLOSE,ATTR_PICK1,0);
}

void ent_ATTRIB_TAB::ent_OnPick2() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick2=1;
    ::SendMessage(hWnd,WM_CLOSE,ATTR_PICK2,0);
}

void ent_ATTRIB_TAB::ent_Yes10(){
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_SPX),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_SPY),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_SPZ),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTR_SPX),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTR_SPY),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTR_SPZ),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_PICK1),TRUE);
}

void ent_ATTRIB_TAB::ent_No10(){
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_SPX),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_SPY),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_SPZ),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTR_SPX),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTR_SPY),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTR_SPZ),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_PICK1),FALSE);
}

void ent_ATTRIB_TAB::ent_Yes11(){
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_APX),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_APY),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_APZ),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTR_APX),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTR_APY),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTR_APZ),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_PICK2),TRUE);
}

void ent_ATTRIB_TAB::ent_No11(){
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_APX),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_APY),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_APZ),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTR_APX),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTR_APY),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTR_APZ),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_PICK2),FALSE);
}

void ent_ATTRIB_TAB::ent_Yes40(){
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_HEIGHT),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTR_HEIGHT),TRUE);
}

void ent_ATTRIB_TAB::ent_No40(){
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_HEIGHT),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTR_HEIGHT),FALSE);
}

void ent_ATTRIB_TAB::ent_Yes50(){
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_ROTATE),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTR_ROTATE),TRUE);
}

void ent_ATTRIB_TAB::ent_No50(){
    ::EnableWindow(::GetDlgItem(m_hWnd,ATTR_ROTATE),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_ATTR_ROTATE),FALSE);
}

//Grey out the correct field.
void ent_ATTRIB_TAB::OnSelChange() {
    UpdateData(TRUE);
    if((iJust72&1 && iJust74&1) || iJust72&8 || iJust72&32){
        ent_Yes10();
    }else{
        ent_No10();
    }
    if(iJust72&1 && iJust74&1){
        ent_No11();
    }else{
        ent_Yes11();
    }
    if(iJust72&8){
        ent_No40();
    }else{
        ent_Yes40();
    }
    if(iJust72&8 || iJust72&32){
        ent_No50();
    }else{
        ent_Yes50();
    }
}

//Set current selection
void ent_ATTRIB_TAB::ent_SetSel() {
    switch(iJust72) {
        case 1:
            switch(iJust74) {
                case 1:
                    iTxtCurSel=0;
                    break;
                case 2:
                    iTxtCurSel=1;
                    break;
                case 4:
                    iTxtCurSel=2;
                    break;
                case 8:
                    iTxtCurSel=3;
                    break;
                default:
                    iTxtCurSel=15;
                    break;
            }
            break;
        case 2:
            switch(iJust74) {
                case 1:
                    iTxtCurSel=4;
                    break;
                case 2:
                    iTxtCurSel=5;
                    break;
                case 4:
                    iTxtCurSel=6;
                    break;
                case 8:
                    iTxtCurSel=7;
                    break;
                default:
                    iTxtCurSel=15;
                    break;
            }
            break;
        case 4:
            switch(iJust74) {
                case 1:
                    iTxtCurSel=8;
                    break;
                case 2:
                    iTxtCurSel=9;
                    break;
                case 4:
                    iTxtCurSel=10;
                    break;
                case 8:
                    iTxtCurSel=11;
                    break;
                default:
                    iTxtCurSel=15;
                    break;
            }
            break;
        case 8:
            iTxtCurSel=12;
            break;
        case 16:
            iTxtCurSel=13;
            break;
        case 32:
            iTxtCurSel=14;
            break;
        default:
            iTxtCurSel=15;
            break;
    }
}

//Fill members from the index of the combo box
void ent_ATTRIB_TAB::ent_SelChange() {
    switch(iTxtCurSel) {
        case 0:
            iJust72=1;
            iJust74=1;
            break;
        case 1:
            iJust72=1;
            iJust74=2;
            break;
        case 2:
            iJust72=1;
            iJust74=4;
            break;
        case 3:
            iJust72=1;
            iJust74=8;
            break;
        case 4:
            iJust72=2;
            iJust74=1;
            break;
        case 5:
            iJust72=2;
            iJust74=2;
            break;
        case 6:
            iJust72=2;
            iJust74=4;
            break;
        case 7:
            iJust72=2;
            iJust74=8;
            break;
        case 8:
            iJust72=4;
            iJust74=1;
            break;
        case 9:
            iJust72=4;
            iJust74=2;
            break;
        case 10:
            iJust72=4;
            iJust74=4;
            break;
        case 11:
            iJust72=4;
            iJust74=8;
            break;
        case 12:
            iJust72=8;
            iJust74=1;
            break;
        case 13:
            iJust72=16;
            iJust74=1;
            break;
        case 14:
            iJust72=32;
            iJust74=1;
            break;
        case 15:
            iJust72=63;
            iJust74=15;
            break;
    }
}

//This will read in the styles
//into the drop down list.
void ent_ATTRIB_TAB::ent_StyleChoice() {
    int i;
	CString csTmp;

	::SendMessage(::GetDlgItem(m_hWnd,ATTR_STYLE),CB_RESETCONTENT,0,0);
//Changed by Vitaly Spirin    SDS_StartTblRead(ResourceString(IDC_ICADENTPROP_STYLE_21, "STYLE" ));
    SDS_StartTblRead("STYLE");
	while(SDS_TblReadHasMore()) {
    	csTmp=SDS_TblReadString();
    	if(csTmp.GetLength() == 0) continue; // EBATECH(CNBR) 2002/4/24 STYPE for SHAPE has null NAME.
        if(csTmp.Find('|')>0) continue;
    	::SendMessage(::GetDlgItem(m_hWnd,ATTR_STYLE),CB_ADDSTRING,0,(LPARAM)(LPCTSTR)csTmp);
    }

    for(i=0;ent_DlgItems[i].ID!=ATTR_STYLE;i++);
    if(ent_DlgItems[i].DspValue!=NULL && strisame(ent_DlgItems[i].DspValue,ResourceString(IDC_ICADENTPROP_VARIES_18, "Varies" )))
        ::SendMessage(::GetDlgItem(m_hWnd,ATTR_STYLE),CB_ADDSTRING,0,(LPARAM)ResourceString(IDC_ICADENTPROP_VARIES_18, "Varies" ));
}

void ent_ATTRIB_TAB::OnStyleChange() {
    char szTmp[IC_ACADBUF];
    struct resbuf *pRb,*pRbb=NULL;

    ::GetWindowText(::GetDlgItem(m_hWnd,ATTR_STYLE),szTmp,sizeof(szTmp)-1);
    if((pRbb=sds_tblsearch("STYLE"/*DNT*/,szTmp,0))==NULL) return;
    for(pRb=pRbb;pRb!=NULL;pRb=pRb->rbnext){
        switch(pRb->restype){
            case 40:
                if(!icadRealEqual(pRb->resval.rreal,0.0)){
                    sds_rtos(pRb->resval.rreal,-1,-1,szTmp);
                    ::SetWindowText(::GetDlgItem(m_hWnd,ATTR_HEIGHT),szTmp);
                }
                break;
            case 41:
                sds_rtos(pRb->resval.rreal,-1,-1,szTmp);
                ::SetWindowText(::GetDlgItem(m_hWnd,ATTR_SCALE),szTmp);
                break;
            case 50:
                sds_angtos(pRb->resval.rreal,-1,-1,szTmp);
                ::SetWindowText(::GetDlgItem(m_hWnd,ATTR_OBLIQUE),szTmp);
                break;
            case 71:
                if(pRb->resval.rint & IC_TEXT_GEN_BACKWARD) {
                    ::SendMessage(::GetDlgItem(m_hWnd,ATTR_BACKWARD),BM_SETCHECK,BST_CHECKED,0);
                }else{
                    ::SendMessage(::GetDlgItem(m_hWnd,ATTR_BACKWARD),BM_SETCHECK,BST_UNCHECKED,0);
                }
                if(pRb->resval.rint & IC_TEXT_GEN_UPSIDEDOWN) {
                    ::SendMessage(::GetDlgItem(m_hWnd,ATTR_UPSIDEDOWN),BM_SETCHECK,BST_CHECKED,0);
                }else{
                    ::SendMessage(::GetDlgItem(m_hWnd,ATTR_UPSIDEDOWN),BM_SETCHECK,BST_UNCHECKED,0);
                }
                break;
        }
    }
    IC_RELRB(pRbb);
    UpdateData(true);
}

void ent_ATTRIB_TAB::DoDataExchange(CDataExchange *pDX) {
    int index,iLen,start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_ATTRIB) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_ATTRIB) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RTREAL:
            case RT3DPOINT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                } else { //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    DDV_MaxChars(pDX,csTmp,511);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                } else { //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    DDV_MaxChars(pDX,csTmp,511);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSHORT:
            case RTSTR:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    if(ent_DlgItems[i].ID==ATTR_STYLE) {
                        index=::SendMessage(::GetDlgItem(m_hWnd,ATTR_STYLE),CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)ent_DlgItems[i].DspValue);
                        DDX_CBIndex(pDX,ATTR_STYLE,index);
                    }else{
                        csTmp=ent_DlgItems[i].DspValue;
                        DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    }
                } else { //Retrieving data from the dialog
                    if(ent_DlgItems[i].ID==ATTR_STYLE) {
                        DDX_CBIndex(pDX,ATTR_STYLE,index);
                            iLen=::SendMessage(::GetDlgItem(m_hWnd,ATTR_STYLE),CB_GETLBTEXTLEN,(WPARAM)index,0);
                            ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,iLen+1);
                            ::SendMessage(::GetDlgItem(m_hWnd,ATTR_STYLE),CB_GETLBTEXT,(WPARAM)index,(LPARAM)ent_DlgItems[i].DspValue);
                    }else{
                        DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                        DDV_MaxChars(pDX,csTmp,511);
                        if(csTmp.IsEmpty()) {
                            if(ent_DlgItems[i].DspValue!=NULL){
                                IC_FREE(ent_DlgItems[i].DspValue);
                                ent_DlgItems[i].DspValue=NULL;
                            }
                        }else{
                            ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                            strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                        }
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                } else { //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
            case ENT_JUSTI:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    if(ent_DlgItems[i].DXFCode==72) {
                        iJust72=ent_DlgItems[i].CheckBit;
                    }else if(ent_DlgItems[i].DXFCode==74) {
                        iJust74=ent_DlgItems[i].CheckBit;
                        ent_SetSel();
                        DDX_CBIndex(pDX,ATTR_JUSTIFY,iTxtCurSel);
                    }
                } else { //Retrieving data from the dialog
                    DDX_CBIndex(pDX,ATTR_JUSTIFY,iTxtCurSel);
                    ent_SelChange();
                    if(ent_DlgItems[i].DXFCode==72) {
                        ent_DlgItems[i].CheckBit=iJust72;
                    }else if(ent_DlgItems[i].DXFCode==74) {
                        ent_DlgItems[i].CheckBit=iJust74;
                    }
                }
        }
    }

}

ent_ATTRIB_TAB::ent_ATTRIB_TAB():CDialog(ENT_ATTRIB_TAB) {
    ent_Pick1=ent_Pick2=0;
}


//////////////////////////////////////////////////////////////////////////////
// CIRCLE     ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_CIRCLE_TAB,CDialog)
    ON_BN_CLICKED(CIRCLE_PICK1,ent_OnPick)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_CIRCLERAD,OnSpinCircleRad)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_CIRCLE_CPX,OnSpinCircleCpx)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_CIRCLE_CPY,OnSpinCircleCpy)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_CIRCLE_CPZ,OnSpinCircleCpz)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL ent_CIRCLE_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_CIRCLE_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_CIRCLE_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}


void ent_CIRCLE_TAB::OnSpinCircleCpx(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,CIRCLE_CPX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_CIRCLE_TAB::OnSpinCircleCpy(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,CIRCLE_CPY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_CIRCLE_TAB::OnSpinCircleCpz(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,CIRCLE_CPZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_CIRCLE_TAB::OnSpinCircleRad(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,CIRCLE_RADIUS),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

BOOL ent_CIRCLE_TAB::OnInitDialog() {

    return(CDialog::OnInitDialog());
}

void ent_CIRCLE_TAB::ent_OnPick() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick=1;
    ::SendMessage(hWnd,WM_CLOSE,CIRCLE_PICK1,0);
}

void ent_CIRCLE_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_CIRCLE) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_CIRCLE) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

ent_CIRCLE_TAB::ent_CIRCLE_TAB() : CDialog(ENT_CIRCLE_TAB) {
    ent_Pick=0;
}


//////////////////////////////////////////////////////////////////////////////
// DIMENSION  ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_DIM_TAB,CDialog)
    ON_BN_CLICKED(DIM_PICK1,ent_OnPick)
    ON_BN_CLICKED(DIM_DIMSET,ent_DimSettings)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_IPX,OnSpinDimIpx)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_IPY,OnSpinDimIpy)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_IPZ,OnSpinDimIpz)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_ROTATION,OnSpinDimRot)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()


/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Disable the Dimension Settings button if we have more than one
 *			dimensions in the selection set.
 * Returns:	CDialog::OnInitDialog return values.
 ********************************************************************************/
BOOL
ent_DIM_TAB::OnInitDialog()
{
	long	ssLength;
    sds_sslength(SDS_EditPropsSS, &ssLength);
	if(ssLength > 1)
		GetDlgItem(DIM_DIMSET)->EnableWindow(FALSE);

	return CDialog::OnInitDialog();
}

BOOL ent_DIM_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_DIM_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_DIM_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_DIM_TAB::ent_DimSettings()
{
    SDS_CMainWindow->IcadDimDia(true);
	// show correct dimstyle
	ent_DlgItems[123].DspValue = ic_realloc_char(ent_DlgItems[123].DspValue, strlen(ent_DlgItems[123].pRbll->prbb->resval.rstring) + 1);
	strcpy(ent_DlgItems[123].DspValue, ent_DlgItems[123].pRbll->prbb->resval.rstring);
    UpdateData(FALSE);
}

void ent_DIM_TAB::OnSpinDimIpx(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,DIM_IPX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_DIM_TAB::OnSpinDimIpy(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,DIM_IPY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_DIM_TAB::OnSpinDimIpz(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,DIM_IPZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_DIM_TAB::OnSpinDimRot(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,DIM_ROTATION),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_DIM_TAB::ent_OnPick() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick=1;
    ::SendMessage(hWnd,WM_CLOSE,0,0);
}

void ent_DIM_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_DIMENSION) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_DIMENSION) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
			//Bugzilla No. 7219 ; 07-10-2002 [
			case DIM_MEASURE:
				if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
				}
				break;			
			//Bugzilla No. 7219 ; 07-10-2002 ]
            case RT3DPOINT:
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    if(csTmp.GetLength()==0) csTmp=ResourceString(IDC_ICADENTPROP____22, "<>" );
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

ent_DIM_TAB::ent_DIM_TAB() : CDialog(ENT_DIM_TAB) {
    ent_Pick=0;
}


//////////////////////////////////////////////////////////////////////////////
// ELLIPSE    ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_ELP_TAB,CDialog)
    ON_BN_CLICKED(ELP_PICK1,ent_OnPick1)
    ON_BN_CLICKED(ELP_PICK2,ent_OnPick2)
	ON_WM_HELPINFO()
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ELP_STX,OnSpin10)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ELP_STY,OnSpin20)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ELP_STZ,OnSpin30)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ELP_EPX,OnSpin11)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ELP_EPY,OnSpin21)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ELP_EPZ,OnSpin31)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ELP_RATIO,OnSpin40)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ELP_START,OnSpin41)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ELP_END,OnSpin42)
END_MESSAGE_MAP()

void ent_ELP_TAB::OnSpin10(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ELP_STX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ELP_TAB::OnSpin20(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ELP_STY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ELP_TAB::OnSpin30(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ELP_STZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ELP_TAB::OnSpin11(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ELP_EPX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ELP_TAB::OnSpin21(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ELP_EPY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ELP_TAB::OnSpin31(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ELP_EPZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ELP_TAB::OnSpin40(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,ELP_RATIO),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ELP_TAB::OnSpin41(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterAngle(::GetDlgItem(m_hWnd,ELP_START),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_ELP_TAB::OnSpin42(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterEndAngle(::GetDlgItem(m_hWnd,ELP_END),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

BOOL ent_ELP_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_ELP_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_ELP_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL ent_ELP_TAB::OnInitDialog() {

    return(CDialog::OnInitDialog());
}

void ent_ELP_TAB::ent_OnPick1() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick1=1;
    ::SendMessage(hWnd,WM_CLOSE,ELP_PICK1,0);
}

void ent_ELP_TAB::ent_OnPick2() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick2=1;
    ::SendMessage(hWnd,WM_CLOSE,ELP_PICK2,0);
}

void ent_ELP_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_ELLIPSE) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_ELLIPSE) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
            case RTANG_END:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

ent_ELP_TAB::ent_ELP_TAB() : CDialog(ENT_ELLIPSE_TAB) {
    ent_Pick1=ent_Pick2=0;
}


//////////////////////////////////////////////////////////////////////////////
// HATCH      ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_HATCH_TAB,CDialog)
    ON_BN_CLICKED(HATCH_HATCHEDIT,OnHatchEditButton)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()


BOOL ent_HATCH_TAB::OnInitDialog() {

    return(CDialog::OnInitDialog());
}

BOOL ent_HATCH_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_HATCH_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_HATCH_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_HATCH_TAB::OnHatchEditButton()
{
//4M Item:17->
// BHatch edit
	CDialog *parent = (CDialog*) GetParent();
	parent->EndDialog(HATCH_HATCHEDIT);
//<-4M Item:17
}

void ent_HATCH_TAB::DoDataExchange(CDataExchange *pDX)
{
    int start,end,i;
	CString csTmp;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_HATCH) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_HATCH) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RTSTR:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
        }
    }
}


//Modified Cybage MM 24-09-2001 DD-MM-YYYY [
//Reason : Bug No.- 77847 from Bugzilla
//////////////////////////////////////////////////////////////////////////////
// IMAGE      ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_IMAGE_TAB,CDialog)
    ON_BN_CLICKED(IMAGE_PICK1,ent_OnPick)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_IMAGE_OPX,OnSpinImageOPx)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_IMAGE_OPY,OnSpinImageOPy)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_IMAGE_OPZ,OnSpinImageOPz)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_IMAGE_BRIGHTNESS,OnSpinImageBrightness)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_IMAGE_CONTRAST,OnSpinImageContrast)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_IMAGE_FADE,OnSpinImageFade)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL ent_IMAGE_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_IMAGE_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_IMAGE_TAB::OnOK(){
	#ifdef BUILD_WITH_RASTER_SUPPORT
		validationFailure = FALSE;
		BOOL success = FALSE;
		char * val= new char[MAX_PATH];
		int brightness = ::GetDlgItemInt(m_hWnd,IMAGE_BRIGHTNESS,&success,TRUE);
		if(!success){
			::GetDlgItemText(m_hWnd,IMAGE_BRIGHTNESS,val,100);
			if(strisame(val,ResourceString(IDC_ICADENTPROP_VARIES_18, "Varies" ))) brightness=0;
			else brightness = -1;
		}
		if(!validate(brightness,IMAGE_BRIGHTNESS)){
			delete [] val;
			return;
		}
		int contrast = ::GetDlgItemInt(m_hWnd,IMAGE_CONTRAST,&success,TRUE);
		if(!success){
			::GetDlgItemText(m_hWnd,IMAGE_CONTRAST,val,100);
			if(strisame(val,ResourceString(IDC_ICADENTPROP_VARIES_18, "Varies" ))) contrast=0;
			else contrast = -1;
		}
		if(!validate(contrast,IMAGE_CONTRAST)){
			delete [] val;
			return;
		}
		int fade = ::GetDlgItemInt(m_hWnd,IMAGE_FADE,&success,TRUE);
		if(!success){
			::GetDlgItemText(m_hWnd,IMAGE_FADE,val,100);
			if(strisame(val,ResourceString(IDC_ICADENTPROP_VARIES_18, "Varies" ))) fade=0;
			else fade = -1;
		}
		if(!validate(fade,IMAGE_FADE)){
			delete [] val;
			return;
		}
		delete [] val;
	#endif
	HWND hParentWnd;
	ent_bHitEnter=true;
	hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL ent_IMAGE_TAB:: validate(int value , int ID){
	if(value<0 || value>100){
		validationFailure=TRUE;
		AfxMessageBox(ResourceString(IDC_ICADENTPROP__71, "Please enter an integer between 0 and 100" ));
		::SetFocus(::GetDlgItem(m_hWnd,ID));
		CEdit * ptr = (CEdit *)GetDlgItem(ID);
		ptr->SetSel(0,-1);
		return FALSE;
	}
	else{
		validationFailure=FALSE;
		return TRUE;
	}
}

void ent_IMAGE_TAB::OnSpinImageOPx(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,IMAGE_OPX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_IMAGE_TAB::OnSpinImageOPy(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,IMAGE_OPY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_IMAGE_TAB::OnSpinImageOPz(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,IMAGE_OPZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_IMAGE_TAB::OnSpinImageBrightness(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterInt(::GetDlgItem(m_hWnd,IMAGE_BRIGHTNESS),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_IMAGE_TAB::OnSpinImageContrast(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterInt(::GetDlgItem(m_hWnd,IMAGE_CONTRAST),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_IMAGE_TAB::OnSpinImageFade(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterInt(::GetDlgItem(m_hWnd,IMAGE_FADE),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_IMAGE_TAB::ent_OnPick() {
    HWND hWnd;
    hWnd=::GetParent(m_hWnd);
    ent_Pick=1;
    ::SendMessage(hWnd,WM_CLOSE,IMAGE_PICK1,0);
}

BOOL ent_IMAGE_TAB::OnInitDialog() {
    return(CDialog::OnInitDialog());
}

void ent_IMAGE_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_IMAGE) && (start<ent_num_of_vars);start++);
    start+=4; //Skip the layer,linetype,linetypescale and color. Note that thickness is not applicable to Images.
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_IMAGE) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
					DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
					if(csTmp.IsEmpty()) {
						if(ent_DlgItems[i].DspValue!=NULL){
							IC_FREE(ent_DlgItems[i].DspValue);
							ent_DlgItems[i].DspValue=NULL;
						}
					}else{
						ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
						strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
					}
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

ent_IMAGE_TAB::ent_IMAGE_TAB() : CDialog(ENT_IMAGE_TAB){
	ent_Pick=0;
}
//Modified Cybage MM 24-09-2001 DD-MM-YYYY ]

//////////////////////////////////////////////////////////////////////////////
// INSERT     ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_INSERT_TAB,CDialog)
    ON_BN_CLICKED(INSERT_PICK1,ent_OnPick)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_COL,OnSpinCol)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ROW,OnSpinRow)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ROWSPACE,OnSpinRowSpace)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_COLSPACE,OnSpinColSpace)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_SFX,OnSpinSfx)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_SFY,OnSpinSfy)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_SFZ,OnSpinSfz)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_INSERT_IPX,OnSpinIpx)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_INSERT_IPY,OnSpinIpy)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_INSERT_IPZ,OnSpinIpz)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ROTATION,OnSpinRot)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL ent_INSERT_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_INSERT_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_INSERT_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_INSERT_TAB::OnSpinCol(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterInt(::GetDlgItem(m_hWnd,INSERT_COLUMNS),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_INSERT_TAB::OnSpinRow(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterInt(::GetDlgItem(m_hWnd,INSERT_ROWS),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_INSERT_TAB::OnSpinRowSpace(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,INSERT_ROWSPACE),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_INSERT_TAB::OnSpinColSpace(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,INSERT_COLSPACE),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_INSERT_TAB::OnSpinIpx(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,INSERT_IPX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_INSERT_TAB::OnSpinIpy(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,INSERT_IPY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_INSERT_TAB::OnSpinIpz(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,INSERT_IPZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_INSERT_TAB::OnSpinSfx(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
	//Modified Cybage SBD	04/03/2002 DD/MM/YYYY
	//Reason : Fix for Bug No. 77983 from Bugzilla
    //ent_SpinMasterReal(::GetDlgItem(m_hWnd,INSERT_SFX),pNMUpDown->iDelta);
	ent_SpinMasterReal(::GetDlgItem(m_hWnd,INSERT_SFX),pNMUpDown->iDelta,2);
    UpdateData(TRUE);
    *result=0;
}

void ent_INSERT_TAB::OnSpinSfy(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
	//Modified Cybage SBD	04/03/2002 DD/MM/YYYY
	//Reason : Fix for Bug No. 77983 from Bugzilla
    //ent_SpinMasterReal(::GetDlgItem(m_hWnd,INSERT_SFY),pNMUpDown->iDelta);
	ent_SpinMasterReal(::GetDlgItem(m_hWnd,INSERT_SFY),pNMUpDown->iDelta,2);
    UpdateData(TRUE);
    *result=0;
}

void ent_INSERT_TAB::OnSpinSfz(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
	//Modified Cybage SBD	04/03/2002 DD/MM/YYYY
	//Reason : Fix for Bug No. 77983 from Bugzilla
	//ent_SpinMasterReal(::GetDlgItem(m_hWnd,INSERT_SFZ),pNMUpDown->iDelta);
	ent_SpinMasterReal(::GetDlgItem(m_hWnd,INSERT_SFZ),pNMUpDown->iDelta,2);
    UpdateData(TRUE);
    *result=0;
}

void ent_INSERT_TAB::OnSpinRot(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterAngle(::GetDlgItem(m_hWnd,INSERT_ROTATION),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

BOOL ent_INSERT_TAB::OnInitDialog() {
    BOOL ret=CDialog::OnInitDialog();
    return(ret);
}

void ent_INSERT_TAB::ent_OnPick() {
    HWND hWnd;
    hWnd=::GetParent(m_hWnd);
    ent_Pick=1;
    ::SendMessage(hWnd,WM_CLOSE,INSERT_PICK1,0);
}

void ent_INSERT_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_INSERT) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_INSERT) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

ent_INSERT_TAB::ent_INSERT_TAB() : CDialog(ENT_INSERT_TAB) {
    ent_Pick=0;
}


//////////////////////////////////////////////////////////////////////////////
// LEADER     ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_LEADER_TAB,CDialog)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_LEADER_X,OnSpinX)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_LEADER_Y,OnSpinY)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_LEADER_Z,OnSpinZ)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_LEADER_VERT_NUM,OnSpinLed)
    ON_BN_CLICKED(LEADER_NEXT,OnLedNext)
    ON_BN_CLICKED(LEADER_PREV,OnLedPrev)
	// Bugzilla No. 78001; 18/03/2002
	ON_BN_CLICKED(LEADER_DIMSET,ent_DimSettings)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL ent_LEADER_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_LEADER_TAB::OnSpinX(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,LEADER_X),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_LEADER_TAB::OnSpinY(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,LEADER_Y),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_LEADER_TAB::OnSpinZ(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,LEADER_Z),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_LEADER_TAB::OnSpinLed(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    if(pNMUpDown->iDelta<0){
        OnLedNext();
    }else{
        OnLedPrev();
    }
    *result=0;
}

void ent_LEADER_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_LEADER_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

// Bugzilla No. 78001; 18/03/2002 [
void ent_LEADER_TAB::ent_DimSettings()
{
    SDS_CMainWindow->IcadDimDia(true);
}
// Bugzilla No. 78001; 18/03/2002 ]

BOOL ent_LEADER_TAB::OnInitDialog() {
	// Bugzilla No. 78001; 18/03/2002 [
	long	ssLength;
    sds_sslength(SDS_EditPropsSS, &ssLength);
	if(ssLength > 1)
		GetDlgItem(LEADER_DIMSET)->EnableWindow(FALSE);
	// Bugzilla No. 78001; 18/03/2002 ]
    CDialog::OnInitDialog();

    if(LedTot<=0){
        GreyLed();
    }else{
        LoadLed();
    }
    return(true);
}

void ent_LEADER_TAB::GreyLed(){
    ::EnableWindow(::GetDlgItem(m_hWnd,LEADER_NEXT),false);
    ::EnableWindow(::GetDlgItem(m_hWnd,LEADER_PREV),false);
    ::EnableWindow(::GetDlgItem(m_hWnd,LEADER_X),false);
    ::EnableWindow(::GetDlgItem(m_hWnd,LEADER_Y),false);
    ::EnableWindow(::GetDlgItem(m_hWnd,LEADER_Z),false);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_LEADER_X),false);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_LEADER_Y),false);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_LEADER_Z),false);
    ::EnableWindow(::GetDlgItem(m_hWnd,LEADER_PICK),false);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_LEADER_VERT_NUM),false);
}

void ent_LEADER_TAB::LoadLed(){
    char szTmp[IC_ACADBUF];
    double ptTmp[3];

    pLcur=pLedbb;
    sds_trans(pLcur->prbb->resval.rpoint,&wcs,&ucs,0,ptTmp);
    sds_rtos(ptTmp[0],-1,-1,szTmp);
    ::SetWindowText(::GetDlgItem(m_hWnd,LEADER_X),szTmp);
    sds_rtos(ptTmp[1],-1,-1,szTmp);
    ::SetWindowText(::GetDlgItem(m_hWnd,LEADER_Y),szTmp);
    sds_rtos(ptTmp[2],-1,-1,szTmp);
    ::SetWindowText(::GetDlgItem(m_hWnd,LEADER_Z),szTmp);

    itoa(LedCurCnt,szTmp,10);
    ::SetWindowText(::GetDlgItem(m_hWnd,LEADER_VERT_NUM),szTmp);
    itoa(LedTot,szTmp,10);
    ::SetWindowText(::GetDlgItem(m_hWnd,LEADER_TOT),szTmp);

    //if this is the last one disable the next button
    if(LedCurCnt==LedTot) ::EnableWindow(::GetDlgItem(m_hWnd,LEADER_NEXT),false);
    //if only one disable the prev button
    if(LedCurCnt==1) ::EnableWindow(::GetDlgItem(m_hWnd,LEADER_PREV),false);
}

void ent_LEADER_TAB::OnLedNext(){
    char szTest[IC_ACADBUF],szTmp[IC_ACADBUF];
    double ptTmp[3];

    if(LedCurCnt < LedTot) {
        //save the old value back
        sds_trans(pLcur->prbb->resval.rpoint,&wcs,&ucs,0,ptTmp);
        sds_rtos(ptTmp[0],-1,-1,szTest);
        ::GetWindowText(::GetDlgItem(m_hWnd,LEADER_X),szTmp,IC_ACADBUF-1);
        if(!strisame(szTest,szTmp)) {
            sds_distof(szTmp,-1,&ptTmp[0]);
        }
        sds_rtos(ptTmp[1],-1,-1,szTest);
        ::GetWindowText(::GetDlgItem(m_hWnd,LEADER_Y),szTmp,IC_ACADBUF-1);
        if(!strisame(szTest,szTmp)) {
            sds_distof(szTmp,-1,&ptTmp[1]);
        }
        sds_rtos(ptTmp[2],-1,-1,szTest);
        ::GetWindowText(::GetDlgItem(m_hWnd,LEADER_Z),szTmp,IC_ACADBUF-1);
        if(!strisame(szTest,szTmp)) {
            sds_distof(szTmp,-1,&ptTmp[2]);
        }
        sds_trans(ptTmp,&ucs,&wcs,0,pLcur->prbb->resval.rpoint);
        //get the next value
        LedCurCnt++;
        pLcur=pLcur->next;
        sds_trans(pLcur->prbb->resval.rpoint,&wcs,&ucs,0,ptTmp);
        sds_rtos(ptTmp[0],-1,-1,szTmp);
        ::SetWindowText(::GetDlgItem(m_hWnd,LEADER_X),szTmp);
        sds_rtos(ptTmp[1],-1,-1,szTmp);
        ::SetWindowText(::GetDlgItem(m_hWnd,LEADER_Y),szTmp);
        sds_rtos(ptTmp[2],-1,-1,szTmp);
        ::SetWindowText(::GetDlgItem(m_hWnd,LEADER_Z),szTmp);

        itoa(LedCurCnt,szTmp,10);
        ::SetWindowText(::GetDlgItem(m_hWnd,LEADER_VERT_NUM),szTmp);

        //if this is the last one disable the next button
        if(LedCurCnt==LedTot) {
            ::SetFocus(::GetDlgItem(m_hWnd,LEADER_PREV));
            ::EnableWindow(::GetDlgItem(m_hWnd,LEADER_NEXT),FALSE);
        }
        //if only one disable prev button
        if(LedCurCnt==1) {
            ::EnableWindow(::GetDlgItem(m_hWnd,LEADER_PREV),false);
        }else{
            ::EnableWindow(::GetDlgItem(m_hWnd,LEADER_PREV),true);
        }
    }
}
void ent_LEADER_TAB::OnLedPrev(){
    char szTest[IC_ACADBUF],szTmp[IC_ACADBUF];
    double ptTmp[3];

    if(LedCurCnt > 1) {
        //save the old value back
        sds_trans(pLcur->prbb->resval.rpoint,&wcs,&ucs,0,ptTmp);
        sds_rtos(ptTmp[0],-1,-1,szTest);
        ::GetWindowText(::GetDlgItem(m_hWnd,LEADER_X),szTmp,IC_ACADBUF-1);
        if(!strisame(szTest,szTmp)) {
            sds_distof(szTmp,-1,&ptTmp[0]);
        }
        sds_rtos(ptTmp[1],-1,-1,szTest);
        ::GetWindowText(::GetDlgItem(m_hWnd,LEADER_Y),szTmp,IC_ACADBUF-1);
        if(!strisame(szTest,szTmp)) {
            sds_distof(szTmp,-1,&ptTmp[1]);
        }
        sds_rtos(ptTmp[2],-1,-1,szTest);
        ::GetWindowText(::GetDlgItem(m_hWnd,LEADER_Z),szTmp,IC_ACADBUF-1);
        if(!strisame(szTest,szTmp)) {
            sds_distof(szTmp,-1,&ptTmp[2]);
        }
        sds_trans(ptTmp,&ucs,&wcs,0,pLcur->prbb->resval.rpoint);
        //get the next value
        LedCurCnt--;
        pLcur=pLcur->prev;
        sds_trans(pLcur->prbb->resval.rpoint,&wcs,&ucs,0,ptTmp);
        sds_rtos(ptTmp[0],-1,-1,szTmp);
        ::SetWindowText(::GetDlgItem(m_hWnd,LEADER_X),szTmp);
        sds_rtos(ptTmp[1],-1,-1,szTmp);
        ::SetWindowText(::GetDlgItem(m_hWnd,LEADER_Y),szTmp);
        sds_rtos(ptTmp[2],-1,-1,szTmp);
        ::SetWindowText(::GetDlgItem(m_hWnd,LEADER_Z),szTmp);

        itoa(LedCurCnt,szTmp,10);
        ::SetWindowText(::GetDlgItem(m_hWnd,LEADER_VERT_NUM),szTmp);

        //if this is the last one disable the next button
        if(LedCurCnt==1) {
            ::SetFocus(::GetDlgItem(m_hWnd,LEADER_NEXT));
            ::EnableWindow(::GetDlgItem(m_hWnd,LEADER_PREV),FALSE);
        }
        //if only one disable the prev button
        if(LedCurCnt==LedTot) {
            ::EnableWindow(::GetDlgItem(m_hWnd,LEADER_NEXT),false);
        }else{
            ::EnableWindow(::GetDlgItem(m_hWnd,LEADER_NEXT),true);
        }
    }
}

void ent_LEADER_TAB::Count(){
    int i;
    struct ent_EntCodes *pPv=NULL,*pTmp=NULL;

    for(i=0;(ent_DlgItems[i].ID!=LEADER_X) && (i<ent_num_of_vars);i++);
    if(i==ent_num_of_vars || ent_DlgItems[i].pRbll==NULL) {
        GreyLed();
    }else{
        pPv=NULL;
        pTmp=pLedbb=ent_DlgItems[i].pRbll;
        while(pTmp!=NULL){
            LedTot++;
            pTmp->prev=pPv;
            pPv=pTmp;
            pTmp=pTmp->next;
        }
    }
}

void ent_LEADER_TAB::DoDataExchange(CDataExchange *pDX) {
    int i;
    double ptTmp[3];
    char szTest[IC_ACADBUF],szTmp[IC_ACADBUF];

    for(i=ddxStart;i<=ddxEnd;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
                if(pDX->m_bSaveAndValidate && pLcur!=NULL){ //Retrieving data from the dialog
                    sds_trans(pLcur->prbb->resval.rpoint,&wcs,&ucs,0,ptTmp);
                    sds_rtos(ptTmp[0],-1,-1,szTest);
                    ::GetWindowText(::GetDlgItem(m_hWnd,LEADER_X),szTmp,IC_ACADBUF-1);
                    if(!strisame(szTest,szTmp)) {
                        sds_distof(szTmp,-1,&ptTmp[0]);
                    }
                    sds_rtos(ptTmp[1],-1,-1,szTest);
                    ::GetWindowText(::GetDlgItem(m_hWnd,LEADER_Y),szTmp,IC_ACADBUF-1);
                    if(!strisame(szTest,szTmp)) {
                        sds_distof(szTmp,-1,&ptTmp[1]);
                    }
                    sds_rtos(ptTmp[2],-1,-1,szTest);
                    ::GetWindowText(::GetDlgItem(m_hWnd,LEADER_Z),szTmp,IC_ACADBUF-1);
                    if(!strisame(szTest,szTmp)) {
                        sds_distof(szTmp,-1,&ptTmp[2]);
                    }
                    sds_trans(ptTmp,&ucs,&wcs,0,pLcur->prbb->resval.rpoint);
                }
                break;
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

ent_LEADER_TAB::ent_LEADER_TAB() : CDialog(ENT_LEADER_TAB) {
    pLedbb=pLcur=NULL;
    LedTot=0;
    if(num_of_leaders<=1) Count();
    LedCurCnt=1;

    for(ddxStart=0;(ent_DlgItems[ddxStart].EntTyp!=DB_LEADER) && (ddxStart<ent_num_of_vars);ddxStart++);
    ddxStart+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(ddxEnd=ddxStart;(ent_DlgItems[ddxEnd+1].EntTyp==DB_LEADER) && (ddxEnd<ent_num_of_vars);ddxEnd++);

    ucs.restype=wcs.restype=RTSHORT;
    ucs.resval.rint=1;
    wcs.resval.rint=0;
}


//////////////////////////////////////////////////////////////////////////////
// LINE       ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_LINE_TAB,CDialog)
    ON_BN_CLICKED(LINE_PICK1,ent_OnPick1)
    ON_BN_CLICKED(LINE_PICK2,ent_OnPick2)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_LINE_STX,OnSpinStx)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_LINE_STY,OnSpinSty)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_LINE_STZ,OnSpinStz)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_LINE_EDX,OnSpinEdx)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_LINE_EDY,OnSpinEdy)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_LINE_EDZ,OnSpinEdz)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL ent_LINE_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_LINE_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_LINE_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}


void ent_LINE_TAB::OnSpinStx(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,LINE_STX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_LINE_TAB::OnSpinSty(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,LINE_STY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_LINE_TAB::OnSpinStz(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,LINE_STZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_LINE_TAB::OnSpinEdx(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,LINE_EDX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_LINE_TAB::OnSpinEdy(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,LINE_EDY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_LINE_TAB::OnSpinEdz(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,LINE_EDZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

BOOL ent_LINE_TAB::OnInitDialog() {

    return(CDialog::OnInitDialog());
}

void ent_LINE_TAB::ent_OnPick1() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick1=1;
    ::SendMessage(hWnd,WM_CLOSE,0,0);
}

void ent_LINE_TAB::ent_OnPick2() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick2=1;
    ::SendMessage(hWnd,WM_CLOSE,0,0);
}

void ent_LINE_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_LINE) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_LINE) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

ent_LINE_TAB::ent_LINE_TAB() : CDialog(ENT_LINE_TAB) {
    ent_Pick1=ent_Pick2=0;
}


//////////////////////////////////////////////////////////////////////////////
// MLINE      ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_MLINE_TAB,CDialog)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL ent_MLINE_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_MLINE_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_MLINE_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL ent_MLINE_TAB::OnInitDialog() {
    return(CDialog::OnInitDialog());
}

void ent_MLINE_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_MLINE) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_MLINE) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

ent_MLINE_TAB::ent_MLINE_TAB() : CDialog(ENT_MLINE_TAB) {
}


//////////////////////////////////////////////////////////////////////////////
// MTEXT      ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_MTEXT_TAB,CDialog)
    ON_BN_CLICKED(MTEXT_PICK,ent_OnPick)
	ON_BN_CLICKED(ID_EDITMTEXT, OnEditMText)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_MTEXT_ORIGIN_X,OnSpinX)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_MTEXT_ORIGIN_Y,OnSpinY)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_MTEXT_ORIGIN_Z,OnSpinZ)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_MTEXT_HEIGHT,OnSpinHigh)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_MTEXT_WIDTH,OnSpinWid)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_MTEXT_ROTATION,OnSpinRot)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL ent_MTEXT_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

extern struct SDS_mTextData SDS_mTextDataPtr;

void ent_MTEXT_TAB::OnEditMText()
	{
    char szTmp[IC_ACADBUF];
	int rotation, justify;
	double width;
	CString style, text, strwidth, rotate;

	//getting all necessary info to sent to new mtext box.
	((CComboBox*) GetDlgItem(MTEXT_STYLE))->GetWindowText(style);
	((CEdit*) GetDlgItem(MTEXT_TEXT))->GetWindowText(text);
	((CEdit*) GetDlgItem(MTEXT_WIDTH))->GetWindowText(strwidth);
	((CEdit*) GetDlgItem(MTEXT_ROTATION))->GetWindowText(rotate);
	justify = ((CComboBox*) GetDlgItem(MTEXT_JUSTIFY))->GetCurSel();
	rotation = atoi(rotate);

    sds_distof(strwidth,-1,&width);	/*D.G.*/// use this instead of the following to satisfy all units
//	width = atof (strwidth);
	/* DP */
	((CEdit*) GetDlgItem(MTEXT_HEIGHT))->GetWindowText(strwidth);
	sds_distof(strwidth, -1, &SDS_mTextDataPtr.rTextHeight);

	//start up a new mtext dialog box
	cmd_oldmtext(text, style, width, rotation, justify);

	//get all changed info and insert in current dialog box.  If there were no changes (cancel was
	//selected) then these will be the same values we passed in.
    sds_rtos(SDS_mTextDataPtr.rBoxWidth, -1, -1, szTmp);	/*D.G.*/// use this instead of the following to satisfy all units
	strwidth = szTmp;
//	strwidth.Format("%.4lf", SDS_mTextDataPtr.rBoxWidth);
	rotate.Format("%.0lf", SDS_mTextDataPtr.rRotAngle);
	((CComboBox*) GetDlgItem(MTEXT_JUSTIFY))->SetCurSel(SDS_mTextDataPtr.nAlignment);
	((CEdit*) GetDlgItem(MTEXT_TEXT))->SetWindowText(SDS_mTextDataPtr.text);
	((CEdit*) GetDlgItem(MTEXT_WIDTH))->SetWindowText(strwidth);
	((CEdit*) GetDlgItem(MTEXT_ROTATION))->SetWindowText(rotate);
	((CComboBox*) GetDlgItem(MTEXT_STYLE))->SelectString(-1, SDS_mTextDataPtr.szTextStyle);
	/*D.G.*/// take the height too.
    sds_rtos(SDS_mTextDataPtr.rTextHeight, -1, -1, szTmp);
	((CEdit*) GetDlgItem(MTEXT_HEIGHT))->SetWindowText(szTmp);
	}

void ent_MTEXT_TAB::OnSpinX(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,MTEXT_ORIGIN_X),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_MTEXT_TAB::OnSpinY(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,MTEXT_ORIGIN_Y),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_MTEXT_TAB::OnSpinZ(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,MTEXT_ORIGIN_Z),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_MTEXT_TAB::OnSpinWid(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,MTEXT_WIDTH),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_MTEXT_TAB::OnSpinHigh(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,MTEXT_HEIGHT),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_MTEXT_TAB::OnSpinRot(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,MTEXT_ROTATION),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_MTEXT_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_MTEXT_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL ent_MTEXT_TAB::OnInitDialog() {
	FillCombo();
    ent_StyleChoice();
    CDialog::OnInitDialog();
    return(true);
}

void ent_MTEXT_TAB::ent_OnPick() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick=1;
    ::SendMessage(hWnd,WM_CLOSE,0,0);
}

void ent_MTEXT_TAB::ent_StyleChoice() {
    int i;
	CString csTmp;

//Changed by Vitaly Spirin    SDS_StartTblRead(ResourceString(IDC_ICADENTPROP_STYLE_21, "STYLE" ));
    SDS_StartTblRead("STYLE");
	while(SDS_TblReadHasMore()) {
    	csTmp=SDS_TblReadString();
    	if(csTmp.GetLength() == 0) continue; // EBATECH(CNBR) 2002/4/24 STYPE for SHAPE has null NAME.
        if(csTmp.Find('|')>0) continue;
    	::SendMessage(::GetDlgItem(m_hWnd,MTEXT_STYLE),CB_ADDSTRING,0,(LPARAM)(LPCTSTR)csTmp);
    }

    for(i=0;ent_DlgItems[i].ID!=MTEXT_STYLE;i++);
    if(ent_DlgItems[i].DspValue!=NULL && strisame(ent_DlgItems[i].DspValue,ResourceString(IDC_ICADENTPROP_VARIES_18, "Varies" )))
        ::SendMessage(::GetDlgItem(m_hWnd,MTEXT_STYLE),CB_ADDSTRING,0,(LPARAM)ResourceString(IDC_ICADENTPROP_VARIES_18, "Varies" ));
}

void ent_MTEXT_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i,index,iLen;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_MTEXT) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_MTEXT) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    if(ent_DlgItems[i].ID==MTEXT_STYLE) {
                        index=::SendMessage(::GetDlgItem(m_hWnd,MTEXT_STYLE),CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)ent_DlgItems[i].DspValue);
                        DDX_CBIndex(pDX,MTEXT_STYLE,index);
                    }else{
                        csTmp=ent_DlgItems[i].DspValue;
                        DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    }
                } else { //Retrieving data from the dialog
                    if(ent_DlgItems[i].ID==MTEXT_STYLE) {
                        DDX_CBIndex(pDX,MTEXT_STYLE,index);
                            iLen=::SendMessage(::GetDlgItem(m_hWnd,MTEXT_STYLE),CB_GETLBTEXTLEN,(WPARAM)index,0);
                            ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,iLen+1);
                            ::SendMessage(::GetDlgItem(m_hWnd,MTEXT_STYLE),CB_GETLBTEXT,(WPARAM)index,(LPARAM)ent_DlgItems[i].DspValue);
                    }else{
                        DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                        if(csTmp.IsEmpty()) {
                            if(ent_DlgItems[i].DspValue!=NULL){
                                IC_FREE(ent_DlgItems[i].DspValue);
                                ent_DlgItems[i].DspValue=NULL;
                            }
                        }else{
                            ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                            strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                        }
                    }
                }
                break;
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case ENT_COMBO:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    DDX_CBIndex(pDX,ent_DlgItems[i].ID,ent_DlgItems[i].CheckBit);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_CBIndex(pDX,ent_DlgItems[i].ID,ent_DlgItems[i].CheckBit);
                }
                break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
                break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
                break;
        }
    }
}

ent_MTEXT_TAB::ent_MTEXT_TAB() : CDialog(ENT_MTEXT_TAB) {
    ent_Pick=0;
}

void ent_MTEXT_TAB::FillCombo()
{
	CComboBox *pCombo = NULL;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(MTEXT_JUSTIFY)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	// Bugzilla No. 76186; 11/11/2002
	//  Add "Varies if necessary
	//comboOption.LoadString(IDC_COMBO_MTEXT_JUSTIFY_1);
	//pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_MTEXT_JUSTIFY_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_MTEXT_JUSTIFY_3);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_MTEXT_JUSTIFY_4);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_MTEXT_JUSTIFY_5);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_MTEXT_JUSTIFY_6);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_MTEXT_JUSTIFY_7);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_MTEXT_JUSTIFY_8);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_MTEXT_JUSTIFY_9);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_MTEXT_JUSTIFY_10);
	pCombo->AddString(comboOption);

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(MTEXT_DIRECTION)))
		return;

	pCombo->ResetContent();
	// Bugzilla No. 76186; 11/11/2002 [
	// Add "Varies" if necessay
	//comboOption.LoadString(IDC_COMBO_MTEXT_DIRECTION_1);
	//pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_MTEXT_DIRECTION_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_MTEXT_DIRECTION_3);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_MTEXT_DIRECTION_4);
	pCombo->AddString(comboOption);
	// EBATECH(CNBR) -[ Add 4(BT), 5(STYLE)
	comboOption.LoadString(IDC_COMBO_MTEXT_DIRECTION_5);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_MTEXT_DIRECTION_6);
	pCombo->AddString(comboOption);
	// ]- EBATECH(CNBR)

	// Bugzilla No. 76186; 11/11/2002 [
	// Check and Add "Varies"
	int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_MTEXT) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_MTEXT) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
			case ENT_COMBO:                
				{
					if ( ( MTEXT_DIRECTION == ent_DlgItems[i].ID && ent_DlgItems[i].CheckBit == 5 )||
						 ( MTEXT_JUSTIFY   == ent_DlgItems[i].ID && ent_DlgItems[i].CheckBit == 9 )
						)
					{
						CComboBox *pCombo = NULL;						
						if (NULL == (pCombo = (CComboBox *) GetDlgItem(ent_DlgItems[i].ID)))
							break;
						pCombo->AddString(ResourceString(IDC_ICADENTPROP_VARIES_18, "Varies" ));
					}
				}

		}
	}
	// Bugzilla No. 76186; 11/11/2002 ]
}


//////////////////////////////////////////////////////////////////////////////
// PLINE      ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_PLINE_TAB,CDialog)
    ON_BN_CLICKED(PVERT_PICK,ent_OnPick)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_PLINEELEV,OnSpinPlineElev)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_PVERT_VERT_NUM,OnVSpin)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_PVERT_X,OnSpinPlineX)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_PVERT_Y,OnSpinPlineY)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_PVERT_Z,OnSpinPlineZ)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_PVERT_ST_WIDTH,OnSpinPlineST)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_PVERT_ED_WIDTH,OnSpinPlineED)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_PVERT_BULGE,OnSpinPlineBulge)
    ON_BN_CLICKED(PVERT_NEXT,OnNext)
    ON_BN_CLICKED(PVERT_PREV,OnPrev)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL ent_PLINE_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_PLINE_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_PLINE_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL ent_PLINE_TAB::OnInitDialog() {
    BOOL ret;
    double temp[3];
    char szTmp[IC_ACADBUF];
    struct resbuf rb;

    ret=CDialog::OnInitDialog();

/*	if(bPlineIsMesh) ::EnableWindow(::GetDlgItem(m_hWnd,PLINE_PFLAG_1),FALSE);
    if(pVert!=NULL) {
        LoadVertex();
        ltoa(lVertNum,szTmp,10);
        ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_VERT_NUM),szTmp);
        ltoa(lVertTot,szTmp,10);
        ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_VERT_TOT),szTmp);
        DisableSomeThings();
    }else{
        DisableVertex();
    }
*/
   	SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,NULL,NULL);
   	ic_ptcpy(temp,rb.resval.rpoint);
    SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,NULL,NULL);
    ic_crossproduct(temp,rb.resval.rpoint,temp);

//	rbucs.restype=RTSHORT;
//	rbucs.resval.rint=1;
    if(NULL==entprop_plextru){
		rbent.resval.rpoint[0]=rbent.resval.rpoint[1];
        rbent.resval.rpoint[2]=1.0;
        if(!icadPointEqual(temp,rbent.resval.rpoint)){
            ::EnableWindow(::GetDlgItem(m_hWnd,PLINE_ELEV),FALSE);
        }
        rbent.restype=RTSHORT;
		rbent.resval.rint=0;//wcs
	}else{
		rbent.restype=RT3DPOINT;
		rbent.resval.rpoint[0]=*entprop_plextru;
		rbent.resval.rpoint[1]=*(entprop_plextru+1);
		rbent.resval.rpoint[2]=*(entprop_plextru+2);
        if(!icadPointEqual(temp,rbent.resval.rpoint)){
            ::EnableWindow(::GetDlgItem(m_hWnd,PLINE_ELEV),FALSE);
        }
	}
//
    if(bPlineIsMesh) ::EnableWindow(::GetDlgItem(m_hWnd,PLINE_PFLAG_1),FALSE);
    if(pVert!=NULL) {
        LoadVertex();
        ltoa(lVertNum,szTmp,10);
        ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_VERT_NUM),szTmp);
        ltoa(lVertTot,szTmp,10);
        ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_VERT_TOT),szTmp);
        DisableSomeThings();
    }else{
        DisableVertex();
    }
//
    if(bPlineIs3D||pVert==NULL) {
        ::EnableWindow(::GetDlgItem(m_hWnd,PLINE_ELEV),FALSE);
    }else{
        ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_Z),FALSE);
    }

    return(ret);
}

void ent_PLINE_TAB::DisableSomeThings(){
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_POLYLINE) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_POLYLINE) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        if(ent_DlgItems[i].ID==PLINE_PFLAG_8){
            if(ent_DlgItems[i].CheckBit!=0){
                ::EnableWindow(::GetDlgItem(m_hWnd,PLINE_ELEV),FALSE);
                ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_ST_WIDTH),FALSE);
                ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_ED_WIDTH),FALSE);
                ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_BULGE),FALSE);
            }
        }
        if(ent_DlgItems[i].ID==PLINE_MESH){
            if(ent_DlgItems[i].CheckBit!=0){
                ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_ST_WIDTH),FALSE);
                ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_ED_WIDTH),FALSE);
                ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_BULGE),FALSE);
            }
        }
    }
}

void ent_PLINE_TAB::OnSpinPlineElev(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,PLINE_ELEV),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_PLINE_TAB::OnSpinPlineX(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,PVERT_X),pNMUpDown->iDelta);
    *result=0;
}

void ent_PLINE_TAB::OnSpinPlineY(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,PVERT_Y),pNMUpDown->iDelta);
    *result=0;
}

void ent_PLINE_TAB::OnSpinPlineZ(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,PVERT_Z),pNMUpDown->iDelta);
    *result=0;
}

void ent_PLINE_TAB::OnSpinPlineST(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,PVERT_ST_WIDTH),pNMUpDown->iDelta);
    *result=0;
}

void ent_PLINE_TAB::OnSpinPlineED(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,PVERT_ED_WIDTH),pNMUpDown->iDelta);
    *result=0;
}

void ent_PLINE_TAB::OnSpinPlineBulge(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,PVERT_BULGE),pNMUpDown->iDelta);
    *result=0;
}

void ent_PLINE_TAB::OnVSpin(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

    if(pNMUpDown->iDelta < 0) {
            OnNextSub(-(pNMUpDown->iDelta));
    }else{
            OnPrevSub(pNMUpDown->iDelta);
    }
}

void ent_PLINE_TAB::OnNext() {
    OnNextSub(1);
}

void ent_PLINE_TAB::OnNextSub(int increment) {
    char szTmp[IC_ACADBUF],szTest[IC_ACADBUF];
    int i;
	sds_point ptemp,ptemp2;

    if(lVertNum < lVertTot) {
        //This will save the dialog entries for the verticies back to the linked list
        for(pRb=pCurV->entlst;pRb!=NULL;pRb=pRb->rbnext) {
            switch(pRb->restype) {
                case 10:
                    ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_X),szTmp,IC_ACADBUF-1);
					sds_trans(pRb->resval.rpoint,&rbent,&rbucs,0,ptemp);
                    sds_rtos(ptemp[0],-1,-1,szTest);
					ptemp2[0]=ptemp2[1]=ptemp2[2]=0.0;
                    if(!strisame(szTest,szTmp)) {
                        sds_distof(szTmp,-1,&ptemp2[0]);
						ptemp2[0]-=ptemp[0];
                    }
                    ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_Y),szTmp,IC_ACADBUF-1);
                    sds_rtos(ptemp[1],-1,-1,szTest);
                    if(!strisame(szTest,szTmp)) {
                        sds_distof(szTmp,-1,&ptemp2[1]);
						ptemp2[1]-=ptemp[1];
                    }
                    if(::IsWindowEnabled(::GetDlgItem(m_hWnd,PLINE_ELEV))){
                        ::GetWindowText(::GetDlgItem(m_hWnd,PLINE_ELEV),szTmp,IC_ACADBUF-1);
                    }else if(::IsWindowEnabled(::GetDlgItem(m_hWnd,PVERT_Z))){
                        ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_Z),szTmp,IC_ACADBUF-1);
                    }
                    if(::IsWindowEnabled(::GetDlgItem(m_hWnd,PLINE_ELEV))||::IsWindowEnabled(::GetDlgItem(m_hWnd,PVERT_Z))){
                        sds_rtos(ptemp[2],-1,-1,szTest);
                        if(!strisame(szTest,szTmp)) {
                            sds_distof(szTmp,-1,&ptemp2[2]);
						    ptemp2[2]-=ptemp[2];
                        }
					}
                    sds_trans(ptemp2,&rbucs,&rbent,1,ptemp);
					pRb->resval.rpoint[0]+=ptemp[0];
					pRb->resval.rpoint[1]+=ptemp[1];
					pRb->resval.rpoint[2]+=ptemp[2];
                    break;
                case 40:
                    ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_ST_WIDTH),szTmp,IC_ACADBUF-1);
                    sds_rtos(pRb->resval.rreal,-1,-1,szTest);
                    if(!strisame(szTest,szTmp)) {
                        sds_distof(szTmp,-1,&pRb->resval.rreal);
                    }
                    break;
                case 41:
                    ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_ED_WIDTH),szTmp,IC_ACADBUF-1);
                    sds_rtos(pRb->resval.rreal,-1,-1,szTest);
                    if(!strisame(szTest,szTmp)) {
                        sds_distof(szTmp,-1,&pRb->resval.rreal);
                    }
                    break;
                case 42:
                    ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_BULGE),szTmp,IC_ACADBUF-1);
                    sds_rtos(pRb->resval.rreal,-1,-1,szTest);
                    if(!strisame(szTest,szTmp)) {
                        sds_distof(szTmp,-1,&pRb->resval.rreal);
                    }
                    break;
            }
        }

        for(i=0;i<increment;i++) {
            //if this is the last vertex stop
            if(lVertNum == lVertTot) break;
            //increment the count
            lVertNum++;
            //point to the next values
            pCurV=pCurV->next;
        }
        //This will get data from the linked list and put it in the dialog for the verticies
        for(pRb=pCurV->entlst;pRb!=NULL;pRb=pRb->rbnext) {
            switch(pRb->restype) {
                case  5:
                    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_HANDLE),pRb->resval.rstring);
                    break;
                case 10:
					sds_trans(pRb->resval.rpoint,&rbent,&rbucs,0,ptemp);
                    sds_rtos(ptemp[0],-1,-1,szTmp);
                    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_X),szTmp);
                    sds_rtos(ptemp[1],-1,-1,szTmp);
                    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_Y),szTmp);
                    if(::IsWindowEnabled(::GetDlgItem(m_hWnd,PVERT_Z))){
                        sds_rtos(ptemp[2],-1,-1,szTmp);
                        ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_Z),szTmp);
                    }
                    break;
                case 40:
                    sds_rtos(pRb->resval.rreal,-1,-1,szTmp);
                    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_ST_WIDTH),szTmp);
                    break;
                case 41:
                    sds_rtos(pRb->resval.rreal,-1,-1,szTmp);
                    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_ED_WIDTH),szTmp);
                    break;
                case 42:
                    sds_rtos(pRb->resval.rreal,-1,-1,szTmp);
                    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_BULGE),szTmp);
                    break;
            }
        }

        //if this is the last one disable the next button
        if((lVertNum)==lVertTot) {
            ::SetFocus(::GetDlgItem(m_hWnd,PVERT_PREV));
            ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_NEXT),FALSE);
        }
        //always enable the prev button
        ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_PREV),TRUE);
        //display the current count;
        ltoa(lVertNum,szTmp,10);
        ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_VERT_NUM),szTmp);
    }
}

void ent_PLINE_TAB::OnPrev() {
    OnPrevSub(1);
}

void ent_PLINE_TAB::OnPrevSub(int increment) {
    char szTmp[IC_ACADBUF],szTest[IC_ACADBUF];
    int i;
	sds_point ptemp,ptemp2;

    if(lVertNum > 1) {
        //This will save the dialog entries for the verticies back to the linked list
        for(pRb=pCurV->entlst;pRb!=NULL;pRb=pRb->rbnext) {
            switch(pRb->restype) {
                case 10:
                    ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_X),szTmp,IC_ACADBUF-1);
					sds_trans(pRb->resval.rpoint,&rbent,&rbucs,0,ptemp);
                    sds_rtos(ptemp[0],-1,-1,szTest);
					ptemp2[0]=ptemp2[1]=ptemp2[2]=0.0;
                    if(!strisame(szTest,szTmp)) {
                        sds_distof(szTmp,-1,&ptemp2[0]);
						ptemp2[0]-=ptemp[0];
                    }
                    ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_Y),szTmp,IC_ACADBUF-1);
                    sds_rtos(ptemp[1],-1,-1,szTest);
                    if(!strisame(szTest,szTmp)) {
                        sds_distof(szTmp,-1,&ptemp2[1]);
						ptemp2[1]-=ptemp[1];
                    }
                    if(::IsWindowEnabled(::GetDlgItem(m_hWnd,PLINE_ELEV))){
                        ::GetWindowText(::GetDlgItem(m_hWnd,PLINE_ELEV),szTmp,IC_ACADBUF-1);
                    }else if(::IsWindowEnabled(::GetDlgItem(m_hWnd,PVERT_Z))){
                        ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_Z),szTmp,IC_ACADBUF-1);
                    }
                    if(::IsWindowEnabled(::GetDlgItem(m_hWnd,PLINE_ELEV))||::IsWindowEnabled(::GetDlgItem(m_hWnd,PVERT_Z))){
                        sds_rtos(ptemp[2],-1,-1,szTest);
                        if(!strisame(szTest,szTmp)) {
                            sds_distof(szTmp,-1,&ptemp2[2]);
						    ptemp2[2]-=ptemp[2];
                        }
					}
                    sds_trans(ptemp2,&rbucs,&rbent,1,ptemp);
					pRb->resval.rpoint[0]+=ptemp[0];
					pRb->resval.rpoint[1]+=ptemp[1];
					pRb->resval.rpoint[2]+=ptemp[2];
                    break;
                case 40:
                    ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_ST_WIDTH),szTmp,IC_ACADBUF-1);
                    sds_rtos(pRb->resval.rreal,-1,-1,szTest);
                    if(!strisame(szTest,szTmp)) {
                        sds_distof(szTmp,-1,&pRb->resval.rreal);
                    }
                    break;
                case 41:
                    ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_ED_WIDTH),szTmp,IC_ACADBUF-1);
                    sds_rtos(pRb->resval.rreal,-1,-1,szTest);
                    if(!strisame(szTest,szTmp)) {
                        sds_distof(szTmp,-1,&pRb->resval.rreal);
                    }
                    break;
                case 42:
                    ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_BULGE),szTmp,IC_ACADBUF-1);
                    sds_rtos(pRb->resval.rreal,-1,-1,szTest);
                    if(!strisame(szTest,szTmp)) {
                        sds_distof(szTmp,-1,&pRb->resval.rreal);
                    }
                    break;
            }
        }

        for(i=0;i<increment;i++) {
            //if this is the first one stop
            if(lVertNum == 1) break;
            //increment the count
            lVertNum--;
            //point the the previous values
            pCurV=pCurV->prev;
        }
        //This will get data from the linked list and put it in the dialog for the verticies
        for(pRb=pCurV->entlst;pRb!=NULL;pRb=pRb->rbnext) {
            switch(pRb->restype) {
                case  5:
                    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_HANDLE),pRb->resval.rstring);
                    break;
                case 10:
					sds_trans(pRb->resval.rpoint,&rbent,&rbucs,0,ptemp);
                    sds_rtos(ptemp[0],-1,-1,szTmp);
                    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_X),szTmp);
                    sds_rtos(ptemp[1],-1,-1,szTmp);
                    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_Y),szTmp);
                    if(::IsWindowEnabled(::GetDlgItem(m_hWnd,PVERT_Z))){
                        sds_rtos(ptemp[2],-1,-1,szTmp);
                        ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_Z),szTmp);
                    }
                    break;
                case 40:
                    sds_rtos(pRb->resval.rreal,-1,-1,szTmp);
                    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_ST_WIDTH),szTmp);
                    break;
                case 41:
                    sds_rtos(pRb->resval.rreal,-1,-1,szTmp);
                    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_ED_WIDTH),szTmp);
                    break;
                case 42:
                    sds_rtos(pRb->resval.rreal,-1,-1,szTmp);
                    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_BULGE),szTmp);
                    break;
            }
        }

        //if this is the first one disable the prev button
        if(lVertNum==1) {
            ::SetFocus(::GetDlgItem(m_hWnd,PVERT_NEXT));
            ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_PREV),FALSE);
        }
        //always enable the next button
        ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_NEXT),TRUE);
        //display the current count;
        ltoa(lVertNum,szTmp,10);
        ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_VERT_NUM),szTmp);
    }
}

void ent_PLINE_TAB::CountVertex() { //count verticies and set the pCurV->prev
    struct cmd_elistll *pCv=NULL,*pPv=NULL;
    int seqend=0;

    lVertTot=0;
    if(pCurV!=NULL) {
        pCv=pVert;
        while(pCv!=NULL && seqend==0) {
            for(pRb=pCv->entlst;pRb!=NULL;pRb=pRb->rbnext) {
                if(pRb->restype==-1) pRb=pRb->rbnext;
                if(pRb->restype==0) {
                    if(strsame("SEQEND"/*DNT*/,pRb->resval.rstring)) {
                        seqend=1;
                        lVertTot--;
                        break;
                    }else{
                        break;
                    }
                }
            }
            lVertTot++;
            pCv->prev=pPv;
            pPv=pCv;
            pCv=pCv->next;
        }
    }
}

void ent_PLINE_TAB::LoadVertex() {
    char szTmp[IC_ACADBUF];
	sds_point ptemp;

    for(pRb=pCurV->entlst;pRb!=NULL;pRb=pRb->rbnext) {
        switch(pRb->restype) {
            case  5:
                ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_HANDLE),pRb->resval.rstring);
                break;
            case 10:
				sds_trans(pRb->resval.rpoint,&rbent,&rbucs,0,ptemp);
                sds_rtos(ptemp[0],-1,-1,szTmp);
                ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_X),szTmp);
                sds_rtos(ptemp[1],-1,-1,szTmp);
                ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_Y),szTmp);
                if(::IsWindowEnabled(::GetDlgItem(m_hWnd,PVERT_Z))){
                    sds_rtos(ptemp[2],-1,-1,szTmp);
                    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_Z),szTmp);
                }
                break;
            case 40:
                sds_rtos(pRb->resval.rreal,-1,-1,szTmp);
                ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_ST_WIDTH),szTmp);
                break;
            case 41:
                sds_rtos(pRb->resval.rreal,-1,-1,szTmp);
                ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_ED_WIDTH),szTmp);
                break;
            case 42:
                sds_rtos(pRb->resval.rreal,-1,-1,szTmp);
                ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_BULGE),szTmp);
                break;
        }
    }
    if(lVertNum==1) {
        ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_PREV),FALSE);
    }else{
        ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_PREV),TRUE);
    }
    if(lVertNum==lVertTot) {
        ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_NEXT),FALSE);
    }else{
        ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_NEXT),TRUE);
    }
}

void ent_PLINE_TAB::DisableVertex() {

    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_HANDLE)," "/*DNT*/);
    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_VERT_NUM),"0"/*DNT*/);
    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_VERT_TOT),"0"/*DNT*/);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_HANDLE),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_VERT_NUM),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_VERT_TOT),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_PICK),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_X),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_Y),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_Z),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_ST_WIDTH),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_ED_WIDTH),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_BULGE),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_NEXT),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_PREV),FALSE);
}

void ent_PLINE_TAB::ent_OnPick() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick=1;
    ::SendMessage(hWnd,WM_CLOSE,0,0);
}

void ent_PLINE_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_POLYLINE) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_POLYLINE) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case ENT_CHECK:
                if(ent_DlgItems[i].ID!=PLINE_MESH&&ent_DlgItems[i].ID!=PLINE_PFLAG_8){
                    if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                        iTmp=ent_DlgItems[i].CheckBit;
                        DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                        DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                        ent_DlgItems[i].CheckBit=iTmp;
                    }
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

void ent_PLINE_TAB::GrabElev(){
    char szTmp[IC_ACADBUF],szTest[IC_ACADBUF];
	sds_point ptemp,ptemp2;
    struct cmd_elistll *pTmp;

    if(pVert&&(!bPlineIs3D)){
        //This will save the dialog entries for the verticies back to the linked list
        for(pTmp=pVert;pTmp!=NULL;pTmp=pTmp->next) {
            for(pRb=pTmp->entlst;pRb!=NULL&&pRb->restype!=10;pRb=pRb->rbnext);
		    if(pRb==NULL) continue;
            ptemp2[0]=ptemp2[1]=ptemp2[2]=0.0;
		    sds_trans(pRb->resval.rpoint,&rbent,&rbucs,0,ptemp);

            if(::IsWindowEnabled(::GetDlgItem(m_hWnd,PLINE_ELEV))){
                ::GetWindowText(::GetDlgItem(m_hWnd,PLINE_ELEV),szTmp,IC_ACADBUF-1);
            }else if(::IsWindowEnabled(::GetDlgItem(m_hWnd,PVERT_Z))){
                ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_Z),szTmp,IC_ACADBUF-1);
            }
            if(::IsWindowEnabled(::GetDlgItem(m_hWnd,PLINE_ELEV))||::IsWindowEnabled(::GetDlgItem(m_hWnd,PVERT_Z))){
                sds_rtos(pRb->resval.rpoint[2],-1,-1,szTest);
                if(!strisame(szTest,szTmp)) {
                    sds_distof(szTmp,-1,&ptemp2[2]);
				    ptemp2[2]-=ptemp[2];
                }
		    }

            sds_trans(ptemp2,&rbucs,&rbent,1,ptemp);
		    pRb->resval.rpoint[0]+=ptemp[0];
		    pRb->resval.rpoint[1]+=ptemp[1];
		    pRb->resval.rpoint[2]+=ptemp[2];
        }
    }
}

void ent_PLINE_TAB::GrabVert(){
    char szTmp[IC_ACADBUF],szTest[IC_ACADBUF];
	sds_point ptemp,ptemp2;

    if(pCurV){
        //This will save the dialog entries for the verticies back to the linked list
        for(pRb=pCurV->entlst;pRb!=NULL;pRb=pRb->rbnext) {
            switch(pRb->restype) {
                case 10:
					ptemp2[0]=ptemp2[1]=ptemp2[2]=0.0;

					sds_trans(pRb->resval.rpoint,&rbent,&rbucs,0,ptemp);

                    ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_X),szTmp,IC_ACADBUF-1);
                    sds_rtos(ptemp[0],-1,-1,szTest);
                    if(!strisame(szTest,szTmp)) {
                        sds_distof(szTmp,-1,&ptemp2[0]);
						ptemp2[0]-=ptemp[0];
                    }

                    ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_Y),szTmp,IC_ACADBUF-1);
                    sds_rtos(ptemp[1],-1,-1,szTest);
                    if(!strisame(szTest,szTmp)) {
                        sds_distof(szTmp,-1,&ptemp2[1]);
						ptemp2[1]-=ptemp[1];
                    }

                    if(::IsWindowEnabled(::GetDlgItem(m_hWnd,PLINE_ELEV))){
                        ::GetWindowText(::GetDlgItem(m_hWnd,PLINE_ELEV),szTmp,IC_ACADBUF-1);
                    }else if(::IsWindowEnabled(::GetDlgItem(m_hWnd,PVERT_Z))){
                        ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_Z),szTmp,IC_ACADBUF-1);
                    }
                    if(::IsWindowEnabled(::GetDlgItem(m_hWnd,PLINE_ELEV))||::IsWindowEnabled(::GetDlgItem(m_hWnd,PVERT_Z))){
                        sds_rtos(ptemp[2],-1,-1,szTest);
                        if(!strisame(szTest,szTmp)) {
                            sds_distof(szTmp,-1,&ptemp2[2]);
						    ptemp2[2]-=ptemp[2];
                        }
					}

                    sds_trans(ptemp2,&rbucs,&rbent,1,ptemp);
					pRb->resval.rpoint[0]+=ptemp[0];
					pRb->resval.rpoint[1]+=ptemp[1];
					pRb->resval.rpoint[2]+=ptemp[2];

                    break;
                case 40:
                    ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_ST_WIDTH),szTmp,IC_ACADBUF-1);
                    sds_rtos(pRb->resval.rreal,-1,-1,szTest);
                    if(!strisame(szTest,szTmp)) {
                        sds_distof(szTmp,-1,&pRb->resval.rreal);
                    }
                    break;
                case 41:
                    ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_ED_WIDTH),szTmp,IC_ACADBUF-1);
                    sds_rtos(pRb->resval.rreal,-1,-1,szTest);
                    if(!strisame(szTest,szTmp)) {
                        sds_distof(szTmp,-1,&pRb->resval.rreal);
                    }
                    break;
                case 42:
                    ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_BULGE),szTmp,IC_ACADBUF-1);
                    sds_rtos(pRb->resval.rreal,-1,-1,szTest);
                    if(!strisame(szTest,szTmp)) {
                        sds_distof(szTmp,-1,&pRb->resval.rreal);
                    }
                    break;
            }
        }
    }
}

ent_PLINE_TAB::ent_PLINE_TAB() : CDialog(ENT_PLINE_TAB) {
    lVertNum=1;
    pCurV=pVert; //pass the global to the locals
    pRb=NULL;
    ent_Pick=0;
    CountVertex();

	// initialize this first
    rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;

}


//////////////////////////////////////////////////////////////////////////////
// LWPLINE    ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_LWPLINE_TAB,CDialog)
    ON_BN_CLICKED(PVERT_PICK,ent_OnPick)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_PLINEELEV,OnSpinPlineElev)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_PVERT_VERT_NUM,OnVSpin)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_PVERT_X,OnSpinPlineX)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_PVERT_Y,OnSpinPlineY)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_PVERT_Z,OnSpinPlineZ)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_PVERT_ST_WIDTH,OnSpinPlineST)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_PVERT_ED_WIDTH,OnSpinPlineED)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_PVERT_BULGE,OnSpinPlineBulge)
    ON_BN_CLICKED(PVERT_NEXT,OnNext)
    ON_BN_CLICKED(PVERT_PREV,OnPrev)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()



ent_LWPLINE_TAB::ent_LWPLINE_TAB() : CDialog(ENT_LWPLINE_TAB)
	{
    lVertNum=0;
    pCurV=pVert; //pass the global to the locals
    pRb=NULL;
    ent_Pick=0;
	// Initialize this first
    rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;
	}

BOOL ent_LWPLINE_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_LWPLINE_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;

    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_LWPLINE_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void
ent_LWPLINE_TAB::AssertValid() const
	{
	ASSERT( GetDlgItem( PVERT_HANDLE_STATIC) );
	ASSERT( GetDlgItem( PVERT_HANDLE) );
	ASSERT( GetDlgItem( PVERT_Z) );
	ASSERT( GetDlgItem( PVERT_Z_STATIC) );
	ASSERT( GetDlgItem( UPDN_PVERT_Z) );
	ASSERT( GetDlgItem( PVERT_PREV) );
	}


BOOL ent_LWPLINE_TAB::OnInitDialog() {
    BOOL ret;
    double temp[3];
    char szTmp[IC_ACADBUF];
    struct resbuf rb;

    ret=CDialog::OnInitDialog();
	//Added Cybage AJK 17-09-2001 [
	//Reason BugFix for bug no 58807 from Bugzilla
	//improper working of close polyline checkbox for ddmodify and entprop
	long	ssLength;
	sds_sslength(SDS_EditPropsSS,&ssLength);

	if(ssLength > 1)
	{
		CButton *pBtn = (CButton *)GetDlgItem(PLINE_PFLAG_1);
		if(pBtn)
		{
			bool ballEntOpen=true;
			bool bnoEntOpen=true;
			sds_name entname;
			RT retval;
			for (int i = 0; i < ssLength; i++)
			{
				int nClosed = 0;
				retval=sds_ssname(SDS_EditPropsSS, i, entname);
				if(retval!=RTNORM) return false;
				((db_handitem*)(entname[0]))->get_70(&nClosed);
				if(nClosed)
					ballEntOpen=false;
				else
					bnoEntOpen=false;
			}

			if(!ballEntOpen)
			{
				if(bnoEntOpen)
				{
					pBtn->SetCheck(1);
				}
				else
				{
					pBtn->SetButtonStyle(BS_AUTO3STATE, TRUE);
					pBtn->SetCheck(2);
				}
			}
			else
				pBtn->SetCheck(0);
		}
	}
	//Added Cybage AJK 17-09-2001 ]

	// hide vertex handle for lightweight polyline
	GetDlgItem( PVERT_HANDLE_STATIC)->ShowWindow( SW_HIDE);
	GetDlgItem( PVERT_HANDLE)->ShowWindow( SW_HIDE);

	// hide unneeded Z controls
	GetDlgItem( PVERT_Z)->ShowWindow( SW_HIDE);
	GetDlgItem( PVERT_Z_STATIC)->ShowWindow( SW_HIDE);
	GetDlgItem( UPDN_PVERT_Z)->ShowWindow( SW_HIDE);

    if(bPlineIsMesh) ::EnableWindow(::GetDlgItem(m_hWnd,PLINE_PFLAG_1),FALSE);

   	SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,NULL,NULL);
   	ic_ptcpy(temp,rb.resval.rpoint);
    SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,NULL,NULL);
    ic_crossproduct(temp,rb.resval.rpoint,temp);

//	rbucs.restype=RTSHORT;
//	rbucs.resval.rint=1;
    if(NULL==entprop_plextru){
		rbent.resval.rpoint[0]=rbent.resval.rpoint[1];
        rbent.resval.rpoint[2]=1.0;
        if(!icadPointEqual(temp,rbent.resval.rpoint)){
            ::EnableWindow(::GetDlgItem(m_hWnd,PLINE_ELEV),FALSE);
        }
        rbent.restype=RTSHORT;
		rbent.resval.rint=0;//wcs
	}else{
		rbent.restype=RT3DPOINT;
		rbent.resval.rpoint[0]=*entprop_plextru;
		rbent.resval.rpoint[1]=*(entprop_plextru+1);
		rbent.resval.rpoint[2]=*(entprop_plextru+2);
        if(!icadPointEqual(temp,rbent.resval.rpoint)){
            ::EnableWindow(::GetDlgItem(m_hWnd,PLINE_ELEV),FALSE);
        }
	}

								// if vertex array is available
	if( pVert!=NULL )
		{
								// if not loaded yet, load and start at 1 (don't reload after select point)
		if ( lVertNum EQ 0 )
			{
			GetVertexArray();
			lVertNum = 1;
			}

        GetDlgItem( PVERT_PREV)->EnableWindow( FALSE);

        ltoa(lVertNum,szTmp,10);
        ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_VERT_NUM),szTmp);
        ltoa(lVertTot,szTmp,10);
        ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_VERT_TOT),szTmp);
		DisplayVertex();

        DisableSomeThings();
    }else{
        DisableVertex();
    }


    if(bPlineIs3D||pVert==NULL) {
        ::EnableWindow(::GetDlgItem(m_hWnd,PLINE_ELEV),FALSE);
    }else{
        ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_Z),FALSE);
    }

    return(ret);
}


void
ent_LWPLINE_TAB::GetVertexArray()
	{
	sds_resbuf	*rb;

	lVertTot = 0;
	for ( rb = pCurV->entlst; rb && rb->restype != 10; )
		rb = rb->rbnext;

	while ( rb && rb->restype EQ 10 )
		{
		vertexArray.push_back( LoadVertex( rb));
		lVertTot += 1;
		}
	}


void
ent_LWPLINE_TAB::SaveVertexArray()
	{

	// If multiple lwpolylines then user can't change vertex info so return
	if (!pCurV)
		return;

	sds_resbuf	*rb;

	for ( rb = pCurV->entlst; rb && rb->restype != 10; )
		rb = rb->rbnext;

	for ( int i=0; i < lVertTot; i++ )
		{
		PlineVertex&	vertex = vertexArray[i];

		ASSERT( rb->restype EQ 10 );
		if ( rb->restype EQ 10 )
			{
			rb->resval.rpoint[0] = vertex.vertex[0];
			rb->resval.rpoint[1] = vertex.vertex[1];
			rb->resval.rpoint[2] = vertex.vertex[2];
			sds_trans( rb->resval.rpoint, &rbucs, &rbent, 0, rb->resval.rpoint );
			}

		rb = rb->rbnext;
		ASSERT( rb->restype EQ 40 );
		if ( rb->restype EQ 40 )
			rb->resval.rreal = vertex.startWidth;

		rb = rb->rbnext;
		ASSERT( rb->restype EQ 41 );
		if ( rb->restype EQ 41 )
			rb->resval.rreal = vertex.endWidth;

		rb = rb->rbnext;
		ASSERT( rb->restype EQ 42 );
		if ( rb->restype EQ 42 )
			rb->resval.rreal = vertex.bulge;

		rb = rb->rbnext;
		}
	}


void
ent_LWPLINE_TAB::SelectVertex( sds_point point)
	{
	PlineVertex&	vertex = vertexArray[ lVertNum-1];

    vertex.vertex[0] = point[0];
	vertex.vertex[1] = point[1];
	}


void ent_LWPLINE_TAB::DisableSomeThings(){
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_POLYLINE) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_POLYLINE) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        if(ent_DlgItems[i].ID==PLINE_PFLAG_8){
            if(ent_DlgItems[i].CheckBit!=0){
                ::EnableWindow(::GetDlgItem(m_hWnd,PLINE_ELEV),FALSE);
                ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_ST_WIDTH),FALSE);
                ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_ED_WIDTH),FALSE);
                ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_BULGE),FALSE);
            }
        }
        if(ent_DlgItems[i].ID==PLINE_MESH){
            if(ent_DlgItems[i].CheckBit!=0){
                ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_ST_WIDTH),FALSE);
                ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_ED_WIDTH),FALSE);
                ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_BULGE),FALSE);
            }
        }
    }
}

void ent_LWPLINE_TAB::OnSpinPlineElev(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,PLINE_ELEV),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_LWPLINE_TAB::OnSpinPlineX(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,PVERT_X),pNMUpDown->iDelta);
    *result=0;
}

void ent_LWPLINE_TAB::OnSpinPlineY(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,PVERT_Y),pNMUpDown->iDelta);
    *result=0;
}

void ent_LWPLINE_TAB::OnSpinPlineZ(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,PVERT_Z),pNMUpDown->iDelta);
    *result=0;
}

void ent_LWPLINE_TAB::OnSpinPlineST(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,PVERT_ST_WIDTH),pNMUpDown->iDelta);
    *result=0;
}

void ent_LWPLINE_TAB::OnSpinPlineED(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,PVERT_ED_WIDTH),pNMUpDown->iDelta);
    *result=0;
}

void ent_LWPLINE_TAB::OnSpinPlineBulge(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,PVERT_BULGE),pNMUpDown->iDelta);
    *result=0;
}

void ent_LWPLINE_TAB::OnVSpin(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

    if(pNMUpDown->iDelta < 0) {
            OnNextSub(-(pNMUpDown->iDelta));
    }else{
            OnPrevSub(pNMUpDown->iDelta);
    }
}

void ent_LWPLINE_TAB::OnNext() {
    OnNextSub(1);
}


void
ent_LWPLINE_TAB::ChangeVertex( int i)
	{
	GetVertex();

	lVertNum = i;
	DisplayVertex();
	}


void
ent_LWPLINE_TAB::DisplayVertex()
	{
    char szTmp[IC_ACADBUF];

	PlineVertex&	vertex = vertexArray[ lVertNum-1];

    sds_rtos( vertex.vertex[0],-1,-1,szTmp);
	::SetWindowText(::GetDlgItem(m_hWnd,PVERT_X),szTmp);
    sds_rtos( vertex.vertex[1],-1,-1,szTmp);
	::SetWindowText(::GetDlgItem(m_hWnd,PVERT_Y),szTmp);
    sds_rtos( vertex.startWidth,-1,-1,szTmp);
	::SetWindowText(::GetDlgItem(m_hWnd,PVERT_ST_WIDTH),szTmp);
    sds_rtos( vertex.endWidth ,-1,-1,szTmp);
	::SetWindowText(::GetDlgItem(m_hWnd,PVERT_ED_WIDTH),szTmp);
    sds_rtos( vertex.bulge,-1,-1,szTmp);
    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_BULGE),szTmp);

    //display the current count;
    ltoa(lVertNum,szTmp,10);
    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_VERT_NUM),szTmp);
	}


void ent_LWPLINE_TAB::OnNextSub(int increment)
	{
    if( lVertNum < lVertTot )
		ChangeVertex( lVertNum+1);

    //if this is the last one disable the next button
    if((lVertNum)==lVertTot)
		{
        ::SetFocus(::GetDlgItem(m_hWnd,PVERT_PREV));
        ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_NEXT),FALSE);
		}

    //always enable the prev button
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_PREV),TRUE);
    }


void ent_LWPLINE_TAB::OnPrev() {
    OnPrevSub(1);
}

void ent_LWPLINE_TAB::OnPrevSub(int increment)
	{
    if( lVertNum > 1 )
		ChangeVertex( lVertNum-1);

    //if this is the last one disable the next button
    if(lVertNum==1 )
		{
        ::SetFocus(::GetDlgItem(m_hWnd,PVERT_NEXT));
        ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_PREV),FALSE);
		}

    //always enable the prev button
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_NEXT),TRUE);
    }


PlineVertex
ent_LWPLINE_TAB::LoadVertex( sds_resbuf *&rb)
	{
    PlineVertex	vertex;

	ASSERT( rb->restype EQ 10 );
	sds_trans( rb->resval.rpoint,&rbent,&rbucs,0, vertex.vertex);
	rb = rb->rbnext;

	ASSERT( rb->restype EQ 40 );
    vertex.startWidth = rb->resval.rreal;
	rb = rb->rbnext;

	ASSERT( rb->restype EQ 41 );
    vertex.endWidth = rb->resval.rreal;
	rb = rb->rbnext;

	ASSERT( rb->restype EQ 42 );
    vertex.bulge = rb->resval.rreal;
	rb = rb->rbnext;

	return vertex;
	}


void ent_LWPLINE_TAB::DisableVertex() {

    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_HANDLE)," "/*DNT*/);
    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_VERT_NUM),"0"/*DNT*/);
    ::SetWindowText(::GetDlgItem(m_hWnd,PVERT_VERT_TOT),"0"/*DNT*/);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_HANDLE),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_VERT_NUM),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_VERT_TOT),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_PICK),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_X),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_Y),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_Z),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_ST_WIDTH),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_ED_WIDTH),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_BULGE),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_NEXT),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,PVERT_PREV),FALSE);
}

void ent_LWPLINE_TAB::ent_OnPick() {
    HWND hWnd;

	SaveVertexArray( );

    hWnd=::GetParent(m_hWnd);
    ent_Pick=1;
    ::SendMessage(hWnd,WM_CLOSE,0,0);
}

void ent_LWPLINE_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_LWPOLYLINE) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_LWPOLYLINE) && (end<ent_num_of_vars);end++);

	if ( pDX->m_bSaveAndValidate )
		SaveVertexArray();

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case ENT_CHECK:
                if(ent_DlgItems[i].ID!=PLINE_MESH&&ent_DlgItems[i].ID!=PLINE_PFLAG_8){
                    if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                        iTmp=ent_DlgItems[i].CheckBit;
                        DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                        DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                        ent_DlgItems[i].CheckBit=iTmp;
                    }
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

void ent_LWPLINE_TAB::GrabElev(){
    char szTmp[IC_ACADBUF],szTest[IC_ACADBUF];
	sds_point ptemp,ptemp2;
    struct cmd_elistll *pTmp;
	struct resbuf *pRb38 = NULL; /*D.Gavrilov*/// We should save an elev in 38 group (lwpline's verts are 2D)!

    if(pVert&&(!bPlineIs3D)){
		for(pRb38 = pVert->entlst; pRb38 != NULL && pRb38->restype != 38; pRb38 = pRb38->rbnext)
			;
        //This will save the dialog entries for the verticies back to the linked list
        for(pTmp=pVert;pTmp!=NULL;pTmp=pTmp->next) {
            for(pRb=pTmp->entlst;pRb!=NULL&&pRb->restype!=10;pRb=pRb->rbnext);
		    if(pRb==NULL) continue;
            ptemp2[0]=ptemp2[1]=ptemp2[2]=0.0;
		    sds_trans(pRb->resval.rpoint,&rbent,&rbucs,0,ptemp);

            if(::IsWindowEnabled(::GetDlgItem(m_hWnd,PLINE_ELEV))){
                ::GetWindowText(::GetDlgItem(m_hWnd,PLINE_ELEV),szTmp,IC_ACADBUF-1);
            }else if(::IsWindowEnabled(::GetDlgItem(m_hWnd,PVERT_Z))){
                ::GetWindowText(::GetDlgItem(m_hWnd,PVERT_Z),szTmp,IC_ACADBUF-1);
            }
            if(::IsWindowEnabled(::GetDlgItem(m_hWnd,PLINE_ELEV))||::IsWindowEnabled(::GetDlgItem(m_hWnd,PVERT_Z))){
                sds_rtos(pRb->resval.rpoint[2],-1,-1,szTest);
                if(!strisame(szTest,szTmp)) {
                    sds_distof(szTmp,-1,&ptemp2[2]);
				    ptemp2[2]-=ptemp[2];
                }
		    }

            sds_trans(ptemp2,&rbucs,&rbent,1,ptemp);
		    pRb->resval.rpoint[0]+=ptemp[0];
		    pRb->resval.rpoint[1]+=ptemp[1];
		    pRb->resval.rpoint[2]+=ptemp[2];
			if(pRb38 != NULL)
				pRb38->resval.rreal = pRb->resval.rpoint[2];
        }
    }
}

void
ent_LWPLINE_TAB::GetVertex()
{
	// If multiple lwpolylines then user can't change vertex info so return
	if (!pCurV)
		return;

    PlineVertex&	oldVertex = vertexArray[ lVertNum-1];
	CString			dialogText;
    TCHAR			szTest[IC_ACADBUF];
	sds_point		ptemp; //, ptemp2;

//	sds_trans( oldVertex.vertex,&rbent,&rbucs,0,ptemp);
	ic_ptcpy( ptemp, oldVertex.vertex );

//	ptemp2[0]=ptemp2[1]=ptemp2[2]=0.0;

	GetDlgItemText( PVERT_X, dialogText);
    sds_rtos(ptemp[0],-1,-1,szTest);
	if ( !strisame( szTest, dialogText) )
		{
		sds_distof(dialogText,-1,&ptemp[0]);
//		ptemp2[0]-=ptemp[0];
		}

	GetDlgItemText( PVERT_Y, dialogText);
    sds_rtos(ptemp[1],-1,-1,szTest);
	if ( !strisame( szTest, dialogText) )
		{
		sds_distof(dialogText,-1,&ptemp[1]);
//		ptemp2[1]-=ptemp[1];
		}

//  sds_trans(ptemp2,&rbucs,&rbent,1,ptemp);
	oldVertex.vertex[0] = ptemp[0];
	oldVertex.vertex[1] = ptemp[1];

	GetDlgItemText( PVERT_ST_WIDTH, dialogText);
    sds_rtos(oldVertex.startWidth,-1,-1,szTest);
	if ( !strisame( szTest, dialogText) )
		sds_distof(dialogText,-1,&oldVertex.startWidth);

	GetDlgItemText( PVERT_ED_WIDTH, dialogText);
    sds_rtos(oldVertex.startWidth,-1,-1,szTest);
	if ( !strisame( szTest, dialogText) )
		sds_distof(dialogText,-1,&oldVertex.endWidth);

	GetDlgItemText( PVERT_BULGE, dialogText);
    sds_rtos(oldVertex.bulge,-1,-1,szTest);
	if ( !strisame( szTest, dialogText) )
		sds_distof(dialogText,-1,&oldVertex.bulge);
}


//////////////////////////////////////////////////////////////////////////////
// POINT      ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_POINT_TAB,CDialog)
    ON_BN_CLICKED(POINT_PICK1,ent_OnPick)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_POINTANG,OnSpinPointAng)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_POINT_CPX,OnSpinCpx)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_POINT_CPY,OnSpinCpy)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_POINT_CPZ,OnSpinCpz)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL ent_POINT_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_POINT_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_POINT_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_POINT_TAB::OnSpinCpx(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,POINT_CPX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_POINT_TAB::OnSpinCpy(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,POINT_CPY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_POINT_TAB::OnSpinCpz(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,POINT_CPZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_POINT_TAB::OnSpinPointAng(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterAngle(::GetDlgItem(m_hWnd,POINT_ANG),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

BOOL ent_POINT_TAB::OnInitDialog() {

    return(CDialog::OnInitDialog());
}

void ent_POINT_TAB::ent_OnPick() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick=1;
    ::SendMessage(hWnd,WM_CLOSE,POINT_PICK1,0);
}

void ent_POINT_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_POINT) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_POINT) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
						      IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

ent_POINT_TAB::ent_POINT_TAB() : CDialog(ENT_POINT_TAB) {
    ent_Pick=0;
}


//////////////////////////////////////////////////////////////////////////////
// RAY        ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_RAY_TAB,CDialog)
    ON_BN_CLICKED(RAY_PICK1,ent_OnPick1)
    ON_BN_CLICKED(RAY_PICK2,ent_OnPick2)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_RAY_STX,OnSpinStx)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_RAY_STY,OnSpinSty)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_RAY_STZ,OnSpinStz)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_RAY_EDX,OnSpinEdx)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_RAY_EDY,OnSpinEdy)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_RAY_EDZ,OnSpinEdz)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL ent_RAY_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_RAY_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_RAY_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_RAY_TAB::OnSpinStx(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,RAY_STX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_RAY_TAB::OnSpinSty(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,RAY_STY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_RAY_TAB::OnSpinStz(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,RAY_STZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_RAY_TAB::OnSpinEdx(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,RAY_EDX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_RAY_TAB::OnSpinEdy(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,RAY_EDY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_RAY_TAB::OnSpinEdz(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,RAY_EDZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

BOOL ent_RAY_TAB::OnInitDialog() {

    return(CDialog::OnInitDialog());
}

void ent_RAY_TAB::ent_OnPick1() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick1=1;
    ::SendMessage(hWnd,WM_CLOSE,RAY_PICK1,0);
}

void ent_RAY_TAB::ent_OnPick2() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick2=1;
    ::SendMessage(hWnd,WM_CLOSE,RAY_PICK2,0);
}

void ent_RAY_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_RAY) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_RAY) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

ent_RAY_TAB::ent_RAY_TAB() : CDialog(ENT_RAY_TAB) {
    ent_Pick1=ent_Pick2=0;
}


//////////////////////////////////////////////////////////////////////////////
// SHAPE       // EBATECH(CNBR) 2002/4/24 ************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_SHAPE_TAB, CDialog)
    ON_BN_CLICKED(SHAPE_PICK,ent_OnPick)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_SHAPE_HEIGHT,OnSpinHeight)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_SHAPE_ROTATION,OnSpinRotation)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_SHAPE_WIDTH,OnSpinWidth)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_SHAPE_OBLIQUE,OnSpinOblique)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_SHAPE_ORIGIN_X,OnSpinOriginX)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_SHAPE_ORIGIN_Y,OnSpinOriginY)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_SHAPE_ORIGIN_Z,OnSpinOriginZ)
END_MESSAGE_MAP()

void ent_SHAPE_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_SHAPE_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_SHAPE_TAB::OnSpinOriginX(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,SHAPE_ORIGIN_X),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_SHAPE_TAB::OnSpinOriginY(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,SHAPE_ORIGIN_Y),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_SHAPE_TAB::OnSpinOriginZ(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,SHAPE_ORIGIN_Z),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_SHAPE_TAB::OnSpinHeight(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,SHAPE_HEIGHT),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_SHAPE_TAB::OnSpinRotation(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterAngle(::GetDlgItem(m_hWnd,SHAPE_ROTATION),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_SHAPE_TAB::OnSpinWidth(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
	ent_SpinMasterReal(::GetDlgItem(m_hWnd,SHAPE_WIDTH),pNMUpDown->iDelta,2);
    UpdateData(TRUE);
    *result=0;
}

void ent_SHAPE_TAB::OnSpinOblique(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterAngle(::GetDlgItem(m_hWnd,SHAPE_OBLIQUE),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

BOOL ent_SHAPE_TAB::OnInitDialog() {
	FillCombo();
	ent_NameChoice();
    CDialog::OnInitDialog();
    return(TRUE);
}

void ent_SHAPE_TAB::ent_OnPick() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick=1;
    ::SendMessage(hWnd,WM_CLOSE,SHAPE_PICK,0);
}

BOOL ent_SHAPE_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_SHAPE_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i,index,iLen;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_SHAPE) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_SHAPE) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RTREAL:
            case RT3DPOINT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                } else { //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    DDV_MaxChars(pDX,csTmp,511);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                } else { //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    DDV_MaxChars(pDX,csTmp,511);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSHORT:
            case RTSTR:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    if(ent_DlgItems[i].ID==SHAPE_NAME) {
                        index=::SendMessage(::GetDlgItem(m_hWnd,SHAPE_NAME),CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)ent_DlgItems[i].DspValue);
                        DDX_CBIndex(pDX,SHAPE_NAME,index);
                    } else {
                    	csTmp=ent_DlgItems[i].DspValue;
                    	DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    }
                } else { //Retrieving data from the dialog
                    if(ent_DlgItems[i].ID==SHAPE_NAME) {
                        DDX_CBIndex(pDX,SHAPE_NAME,index);
                        iLen=::SendMessage(::GetDlgItem(m_hWnd,SHAPE_NAME),CB_GETLBTEXTLEN,(WPARAM)index,0);
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,iLen+1);
                        ::SendMessage(::GetDlgItem(m_hWnd,SHAPE_NAME),CB_GETLBTEXT,(WPARAM)index,(LPARAM)ent_DlgItems[i].DspValue);
                    } else {
	                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
	                    DDV_MaxChars(pDX,csTmp,511);
	                    if(csTmp.IsEmpty()) {
	                        if(ent_DlgItems[i].DspValue!=NULL){
	                            IC_FREE(ent_DlgItems[i].DspValue);
	                            ent_DlgItems[i].DspValue=NULL;
	                        }
	                    }else{
	                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
	                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
	                    }
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                } else { //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

ent_SHAPE_TAB::ent_SHAPE_TAB() : CDialog(ENT_SHAPE_TAB) {
    ent_Pick=0;
}

void ent_SHAPE_TAB::FillCombo()
{
	CComboBox *pCombo = NULL;
	db_styletabrec* pShapeStyle;
	db_fontlink* pShapeFile;
	char* pShapeName;
	int  flag;
	int	idx;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(SHAPE_NAME)))
		return;
	pCombo->ResetContent();

	for( pShapeStyle = (db_styletabrec*)((long)SDS_CURDWG->tblnext(DB_STYLETABREC,1));
		pShapeStyle != NULL ; pShapeStyle = (db_styletabrec*)((long)SDS_CURDWG->tblnext(DB_STYLETABREC,0)))
	{
		pShapeStyle->get_70(&flag);
		if(( flag & IC_SHAPEFILE_SHAPES ) && !( flag & IC_ENTRY_XREF_DEPENDENT ))
		{
			pShapeFile = pShapeStyle->ret_font();
			for(idx = 0 ; idx < pShapeFile->nchrs ; idx++ )
			{
				pShapeName = pShapeFile->chr[idx].symbolName;
				if( pShapeFile->chr[idx].code != 0 &&
					pShapeName != NULL &&
					strlen(pShapeName) > 0 &&
					pCombo->FindString(-1,pShapeName ) == CB_ERR )
				{
					pCombo->AddString(pShapeName);
				}
			}
		}
	}
}

void ent_SHAPE_TAB::ent_NameChoice() {
    int i;
	CString csTmp;

    for(i=0;ent_DlgItems[i].ID!=SHAPE_NAME;i++);
    if(ent_DlgItems[i].DspValue!=NULL && strisame(ent_DlgItems[i].DspValue,ResourceString(IDC_ICADENTPROP_VARIES_18, "Varies" )))
        ::SendMessage(::GetDlgItem(m_hWnd,SHAPE_NAME),CB_ADDSTRING,0,(LPARAM)ResourceString(IDC_ICADENTPROP_VARIES_18, "Varies" ));
}

// EBATECH(CNBR) ]-

//////////////////////////////////////////////////////////////////////////////
// SOLID      ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_SOLID_TAB,CDialog)
    ON_BN_CLICKED(SOLID_PICK1,ent_OnPick1)
    ON_BN_CLICKED(SOLID_PICK2,ent_OnPick2)
    ON_BN_CLICKED(SOLID_PICK3,ent_OnPick3)
    ON_BN_CLICKED(SOLID_PICK4,ent_OnPick4)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_SOLID_P1X,OnSpinPt1x)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_SOLID_P1Y,OnSpinPt1y)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_SOLID_P2X,OnSpinPt2x)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_SOLID_P2Y,OnSpinPt2y)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_SOLID_P3X,OnSpinPt3x)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_SOLID_P3Y,OnSpinPt3y)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_SOLID_P4X,OnSpinPt4x)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_SOLID_P4Y,OnSpinPt4y)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_SOLID_P4Z,OnSpinPt4z)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL ent_SOLID_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_SOLID_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_SOLID_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_SOLID_TAB::OnSpinPt1x(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,SOLID_P1X),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_SOLID_TAB::OnSpinPt1y(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,SOLID_P1Y),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_SOLID_TAB::OnSpinPt2x(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,SOLID_P2X),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_SOLID_TAB::OnSpinPt2y(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,SOLID_P2Y),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_SOLID_TAB::OnSpinPt3x(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,SOLID_P3X),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_SOLID_TAB::OnSpinPt3y(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,SOLID_P3Y),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_SOLID_TAB::OnSpinPt4x(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,SOLID_P4X),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_SOLID_TAB::OnSpinPt4y(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,SOLID_P4Y),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_SOLID_TAB::OnSpinPt4z(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,SOLID_P4Z),pNMUpDown->iDelta);
    CopyAllZs();
    UpdateData(TRUE);
    *result=0;
}

void ent_SOLID_TAB::CopyAllZs() {
    char szTmp[IC_ACADBUF];

    ::GetWindowText(::GetDlgItem(m_hWnd,SOLID_P4Z),szTmp,IC_ACADBUF-1);
    ::SetWindowText(::GetDlgItem(m_hWnd,SOLID_P1Z),szTmp);
    ::SetWindowText(::GetDlgItem(m_hWnd,SOLID_P2Z),szTmp);
    ::SetWindowText(::GetDlgItem(m_hWnd,SOLID_P3Z),szTmp);
}

BOOL ent_SOLID_TAB::OnInitDialog() {
    CDialog::OnInitDialog();
    CopyAllZs();
    return(true);
}

void ent_SOLID_TAB::ent_OnPick1() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick1=1;
    ::SendMessage(hWnd,WM_CLOSE,0,0);
}

void ent_SOLID_TAB::ent_OnPick2() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick2=1;
    ::SendMessage(hWnd,WM_CLOSE,0,0);
}

void ent_SOLID_TAB::ent_OnPick3() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick3=1;
    ::SendMessage(hWnd,WM_CLOSE,0,0);
}

void ent_SOLID_TAB::ent_OnPick4() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick4=1;
    ::SendMessage(hWnd,WM_CLOSE,0,0);
}

void ent_SOLID_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_SOLID) && (start<ent_num_of_vars);start++);
    start+=4;  //Skip the layer,linetype,linetypescale,thickness
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_SOLID) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
                if(ent_DlgItems[i].ID==SOLID_P4Z) CopyAllZs();
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

ent_SOLID_TAB::ent_SOLID_TAB() : CDialog(ENT_SOLID_TAB) {
    ent_Pick1=ent_Pick2=ent_Pick3=ent_Pick4=0;
}


//////////////////////////////////////////////////////////////////////////////
// SPLINE     ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_SPLINE_TAB,CDialog)
    ON_BN_CLICKED(SPLINE_CTL_NEXT,OnCtlNext)
    ON_BN_CLICKED(SPLINE_CTL_PREV,OnCtlPrev)
    ON_BN_CLICKED(SPLINE_FIT_NEXT,OnFitNext)
    ON_BN_CLICKED(SPLINE_FIT_PREV,OnFitPrev)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL ent_SPLINE_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_SPLINE_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_SPLINE_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL ent_SPLINE_TAB::OnInitDialog() {
    CDialog::OnInitDialog();

    if(CtlTot<=0){
        GreyCtl();
    }else{
        LoadCtl();
    }

    if(FitTot<=0){
        GreyFit();
    }else{
        LoadFit();
    }
    return(true);
}

void ent_SPLINE_TAB::GreyCtl(){
    ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_CTL_NEXT),false);
    ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_CTL_PREV),false);
    ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_CTL_X),false);
    ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_CTL_Y),false);
    ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_CTL_Z),false);
}

void ent_SPLINE_TAB::GreyFit(){
    ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_FIT_NEXT),false);
    ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_FIT_PREV),false);
    ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_FIT_X),false);
    ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_FIT_Y),false);
    ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_FIT_Z),false);
}

void ent_SPLINE_TAB::LoadCtl(){
    char szTmp[IC_ACADBUF];
    double ptTmp[3];

    pCcur=pCtlbb;
    sds_trans(pCcur->prbb->resval.rpoint,&wcs,&ucs,0,ptTmp);
    sds_rtos(ptTmp[0],-1,-1,szTmp);
    ::SetWindowText(::GetDlgItem(m_hWnd,SPLINE_CTL_X),szTmp);
    sds_rtos(ptTmp[1],-1,-1,szTmp);
    ::SetWindowText(::GetDlgItem(m_hWnd,SPLINE_CTL_Y),szTmp);
    sds_rtos(ptTmp[2],-1,-1,szTmp);
    ::SetWindowText(::GetDlgItem(m_hWnd,SPLINE_CTL_Z),szTmp);

    //if this is the last one disable the next button
    if(CtlCurCnt==CtlTot) ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_CTL_NEXT),false);
    //if only one disable the prev button
    if(CtlCurCnt==1) ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_CTL_PREV),false);
}

void ent_SPLINE_TAB::LoadFit(){
    char szTmp[IC_ACADBUF];
    double ptTmp[3];

    pFcur=pFitbb;
    sds_trans(pFcur->prbb->resval.rpoint,&wcs,&ucs,0,ptTmp);
    sds_rtos(ptTmp[0],-1,-1,szTmp);
    ::SetWindowText(::GetDlgItem(m_hWnd,SPLINE_FIT_X),szTmp);
    sds_rtos(ptTmp[1],-1,-1,szTmp);
    ::SetWindowText(::GetDlgItem(m_hWnd,SPLINE_FIT_Y),szTmp);
    sds_rtos(ptTmp[2],-1,-1,szTmp);
    ::SetWindowText(::GetDlgItem(m_hWnd,SPLINE_FIT_Z),szTmp);

    //if this is the last one disable the next button
    if(FitCurCnt==FitTot) ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_FIT_NEXT),false);
    //if only one disable the prev button
    if(FitCurCnt==1) ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_FIT_PREV),false);
}

void ent_SPLINE_TAB::OnCtlNext(){
    char szTest[IC_ACADBUF],szTmp[IC_ACADBUF];
    double ptTmp[3];

    if(CtlCurCnt < CtlTot) {
        //save the old value back
        sds_trans(pCcur->prbb->resval.rpoint,&wcs,&ucs,0,ptTmp);
        sds_rtos(ptTmp[0],-1,-1,szTest);
        ::GetWindowText(::GetDlgItem(m_hWnd,SPLINE_CTL_X),szTmp,IC_ACADBUF-1);
        if(!strisame(szTest,szTmp)) {
            sds_distof(szTmp,-1,&ptTmp[0]);
        }
        sds_rtos(ptTmp[1],-1,-1,szTest);
        ::GetWindowText(::GetDlgItem(m_hWnd,SPLINE_CTL_Y),szTmp,IC_ACADBUF-1);
        if(!strisame(szTest,szTmp)) {
            sds_distof(szTmp,-1,&ptTmp[1]);
        }
        sds_rtos(ptTmp[2],-1,-1,szTest);
        ::GetWindowText(::GetDlgItem(m_hWnd,SPLINE_CTL_Z),szTmp,IC_ACADBUF-1);
        if(!strisame(szTest,szTmp)) {
            sds_distof(szTmp,-1,&ptTmp[2]);
        }
        sds_trans(ptTmp,&ucs,&wcs,0,pCcur->prbb->resval.rpoint);
        //get the next value
        CtlCurCnt++;
        pCcur=pCcur->next;
        sds_trans(pCcur->prbb->resval.rpoint,&wcs,&ucs,0,ptTmp);
        sds_rtos(ptTmp[0],-1,-1,szTmp);
        ::SetWindowText(::GetDlgItem(m_hWnd,SPLINE_CTL_X),szTmp);
        sds_rtos(ptTmp[1],-1,-1,szTmp);
        ::SetWindowText(::GetDlgItem(m_hWnd,SPLINE_CTL_Y),szTmp);
        sds_rtos(ptTmp[2],-1,-1,szTmp);
        ::SetWindowText(::GetDlgItem(m_hWnd,SPLINE_CTL_Z),szTmp);

        //if this is the last one disable the next button
        if(CtlCurCnt==CtlTot) {
            ::SetFocus(::GetDlgItem(m_hWnd,SPLINE_CTL_PREV));
            ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_CTL_NEXT),FALSE);
        }
        //if only one disable prev button
        if(CtlCurCnt==1) {
            ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_CTL_PREV),false);
        }else{
            ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_CTL_PREV),true);
        }
    }
}

void ent_SPLINE_TAB::OnCtlPrev(){
    char szTest[IC_ACADBUF],szTmp[IC_ACADBUF];
    double ptTmp[3];

    if(CtlCurCnt > 1) {
        //save the old value back
        sds_trans(pCcur->prbb->resval.rpoint,&wcs,&ucs,0,ptTmp);
        sds_rtos(ptTmp[0],-1,-1,szTest);
        ::GetWindowText(::GetDlgItem(m_hWnd,SPLINE_CTL_X),szTmp,IC_ACADBUF-1);
        if(!strisame(szTest,szTmp)) {
            sds_distof(szTmp,-1,&ptTmp[0]);
        }
        sds_rtos(pCcur->prbb->resval.rpoint[1],-1,-1,szTest);
        ::GetWindowText(::GetDlgItem(m_hWnd,SPLINE_CTL_Y),szTmp,IC_ACADBUF-1);
        if(!strisame(szTest,szTmp)) {
            sds_distof(szTmp,-1,&ptTmp[1]);
        }
        sds_rtos(pCcur->prbb->resval.rpoint[2],-1,-1,szTest);
        ::GetWindowText(::GetDlgItem(m_hWnd,SPLINE_CTL_Z),szTmp,IC_ACADBUF-1);
        if(!strisame(szTest,szTmp)) {
            sds_distof(szTmp,-1,&ptTmp[2]);
        }
        sds_trans(ptTmp,&ucs,&wcs,0,pCcur->prbb->resval.rpoint);
        //get the next value
        CtlCurCnt--;
        pCcur=pCcur->prev;
        sds_trans(pCcur->prbb->resval.rpoint,&wcs,&ucs,0,ptTmp);
        sds_rtos(ptTmp[0],-1,-1,szTmp);
        ::SetWindowText(::GetDlgItem(m_hWnd,SPLINE_CTL_X),szTmp);
        sds_rtos(ptTmp[1],-1,-1,szTmp);
        ::SetWindowText(::GetDlgItem(m_hWnd,SPLINE_CTL_Y),szTmp);
        sds_rtos(ptTmp[2],-1,-1,szTmp);
        ::SetWindowText(::GetDlgItem(m_hWnd,SPLINE_CTL_Z),szTmp);

        //if this is the last one disable the next button
        if(CtlCurCnt==1) {
            ::SetFocus(::GetDlgItem(m_hWnd,SPLINE_CTL_NEXT));
            ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_CTL_PREV),FALSE);
        }
        //if only one disable the prev button
        if(CtlCurCnt==CtlTot) {
            ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_CTL_NEXT),false);
        }else{
            ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_CTL_NEXT),true);
        }
    }
}
void ent_SPLINE_TAB::OnFitNext(){
    char szTest[IC_ACADBUF],szTmp[IC_ACADBUF];
    double ptTmp[3];

    if(FitCurCnt < FitTot) {
        //save the old value back
        sds_trans(pFcur->prbb->resval.rpoint,&wcs,&ucs,0,ptTmp);
        sds_rtos(ptTmp[0],-1,-1,szTest);
        ::GetWindowText(::GetDlgItem(m_hWnd,SPLINE_FIT_X),szTmp,IC_ACADBUF-1);
        if(!strisame(szTest,szTmp)) {
            sds_distof(szTmp,-1,&ptTmp[0]);
        }
        sds_rtos(ptTmp[1],-1,-1,szTest);
        ::GetWindowText(::GetDlgItem(m_hWnd,SPLINE_FIT_Y),szTmp,IC_ACADBUF-1);
        if(!strisame(szTest,szTmp)) {
            sds_distof(szTmp,-1,&ptTmp[1]);
        }
        sds_rtos(ptTmp[2],-1,-1,szTest);
        ::GetWindowText(::GetDlgItem(m_hWnd,SPLINE_FIT_Z),szTmp,IC_ACADBUF-1);
        if(!strisame(szTest,szTmp)) {
            sds_distof(szTmp,-1,&ptTmp[2]);
        }
        sds_trans(ptTmp,&ucs,&wcs,0,pFcur->prbb->resval.rpoint);
        //get the next value
        FitCurCnt++;
        pFcur=pFcur->next;
        sds_trans(pFcur->prbb->resval.rpoint,&wcs,&ucs,0,ptTmp);
        sds_rtos(ptTmp[0],-1,-1,szTmp);
        ::SetWindowText(::GetDlgItem(m_hWnd,SPLINE_FIT_X),szTmp);
        sds_rtos(ptTmp[1],-1,-1,szTmp);
        ::SetWindowText(::GetDlgItem(m_hWnd,SPLINE_FIT_Y),szTmp);
        sds_rtos(ptTmp[2],-1,-1,szTmp);
        ::SetWindowText(::GetDlgItem(m_hWnd,SPLINE_FIT_Z),szTmp);

        //if this is the last one disable the next button
        if(FitCurCnt==FitTot) {
            ::SetFocus(::GetDlgItem(m_hWnd,SPLINE_FIT_PREV));
            ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_FIT_NEXT),FALSE);
        }
        //if only one disable the prev button
        if(FitCurCnt==1) {
            ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_FIT_PREV),false);
        }else{
            ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_FIT_PREV),true);
        }
    }
}
void ent_SPLINE_TAB::OnFitPrev(){
    char szTest[IC_ACADBUF],szTmp[IC_ACADBUF];
    double ptTmp[3];

    if(FitCurCnt > 1) {
        //save the old value back
        sds_trans(pFcur->prbb->resval.rpoint,&wcs,&ucs,0,ptTmp);
        sds_rtos(ptTmp[0],-1,-1,szTest);
        ::GetWindowText(::GetDlgItem(m_hWnd,SPLINE_FIT_X),szTmp,IC_ACADBUF-1);
        if(!strisame(szTest,szTmp)) {
            sds_distof(szTmp,-1,&ptTmp[0]);
        }
        sds_rtos(ptTmp[1],-1,-1,szTest);
        ::GetWindowText(::GetDlgItem(m_hWnd,SPLINE_FIT_Y),szTmp,IC_ACADBUF-1);
        if(!strisame(szTest,szTmp)) {
            sds_distof(szTmp,-1,&ptTmp[1]);
        }
        sds_rtos(ptTmp[2],-1,-1,szTest);
        ::GetWindowText(::GetDlgItem(m_hWnd,SPLINE_FIT_Z),szTmp,IC_ACADBUF-1);
        if(!strisame(szTest,szTmp)) {
            sds_distof(szTmp,-1,&ptTmp[2]);
        }
        sds_trans(ptTmp,&ucs,&wcs,0,pFcur->prbb->resval.rpoint);
        //get the next value
        FitCurCnt--;
        pFcur=pFcur->prev;
        sds_trans(pFcur->prbb->resval.rpoint,&wcs,&ucs,0,ptTmp);
        sds_rtos(ptTmp[0],-1,-1,szTmp);
        ::SetWindowText(::GetDlgItem(m_hWnd,SPLINE_FIT_X),szTmp);
        sds_rtos(ptTmp[1],-1,-1,szTmp);
        ::SetWindowText(::GetDlgItem(m_hWnd,SPLINE_FIT_Y),szTmp);
        sds_rtos(ptTmp[2],-1,-1,szTmp);
        ::SetWindowText(::GetDlgItem(m_hWnd,SPLINE_FIT_Z),szTmp);

        //if this is the last one disable the next button
        if(FitCurCnt==1) {
            ::SetFocus(::GetDlgItem(m_hWnd,SPLINE_FIT_NEXT));
            ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_FIT_PREV),FALSE);
        }
        //always enable the prev button
        if(FitCurCnt==FitTot) {
            ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_FIT_NEXT),false);
        }else{
            ::EnableWindow(::GetDlgItem(m_hWnd,SPLINE_FIT_NEXT),true);
        }
    }
}

void ent_SPLINE_TAB::Count(){
    int i;
    struct ent_EntCodes *pPv=NULL,*pTmp=NULL;

    //Read in 10 group
    CtlTot=0;
    for(i=0;(ent_DlgItems[i].ID!=SPLINE_CTL_X) && (i<ent_num_of_vars);i++);
    if(i==ent_num_of_vars || ent_DlgItems[i].pRbll==NULL) {
        GreyCtl();
    }else{
        pPv=NULL;
        pTmp=pCtlbb=ent_DlgItems[i].pRbll;
        while(pTmp!=NULL){
            CtlTot++;
            pTmp->prev=pPv;
            pPv=pTmp;
            pTmp=pTmp->next;
        }
    }
    //Read in 11 group
    FitTot=0;
    for(i=0;(ent_DlgItems[i].ID!=SPLINE_FIT_X) && (i<ent_num_of_vars);i++);
    if(i==ent_num_of_vars || ent_DlgItems[i].pRbll==NULL) {
        GreyFit();
    }else{
        pPv=NULL;
        pTmp=pFitbb=ent_DlgItems[i].pRbll;
        while(pTmp!=NULL){
            FitTot++;
            pTmp->prev=pPv;
            pPv=pTmp;
            pTmp=pTmp->next;
        }
    }
}

void ent_SPLINE_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i;
    double ptTmp[3];
    char szTest[IC_ACADBUF],szTmp[IC_ACADBUF];

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_SPLINE) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_SPLINE) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
                if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    if(pFcur!=NULL){
                        sds_trans(pFcur->prbb->resval.rpoint,&wcs,&ucs,0,ptTmp);
                        sds_rtos(ptTmp[0],-1,-1,szTest);
                        ::GetWindowText(::GetDlgItem(m_hWnd,SPLINE_FIT_X),szTmp,IC_ACADBUF-1);
                        if(!strisame(szTest,szTmp)) {
                            sds_distof(szTmp,-1,&ptTmp[0]);
                        }
                        sds_rtos(ptTmp[1],-1,-1,szTest);
                        ::GetWindowText(::GetDlgItem(m_hWnd,SPLINE_FIT_Y),szTmp,IC_ACADBUF-1);
                        if(!strisame(szTest,szTmp)) {
                            sds_distof(szTmp,-1,&ptTmp[1]);
                        }
                        sds_rtos(ptTmp[2],-1,-1,szTest);
                        ::GetWindowText(::GetDlgItem(m_hWnd,SPLINE_FIT_Z),szTmp,IC_ACADBUF-1);
                        if(!strisame(szTest,szTmp)) {
                            sds_distof(szTmp,-1,&ptTmp[2]);
                        }
                        sds_trans(ptTmp,&ucs,&wcs,0,pFcur->prbb->resval.rpoint);
                   }
                    if(pCcur!=NULL){
                        sds_trans(pCcur->prbb->resval.rpoint,&wcs,&ucs,0,ptTmp);
                        sds_rtos(ptTmp[0],-1,-1,szTest);
                        ::GetWindowText(::GetDlgItem(m_hWnd,SPLINE_CTL_X),szTmp,IC_ACADBUF-1);
                        if(!strisame(szTest,szTmp)) {
                            sds_distof(szTmp,-1,&ptTmp[0]);
                        }
                        sds_rtos(ptTmp[1],-1,-1,szTest);
                        ::GetWindowText(::GetDlgItem(m_hWnd,SPLINE_CTL_Y),szTmp,IC_ACADBUF-1);
                        if(!strisame(szTest,szTmp)) {
                            sds_distof(szTmp,-1,&ptTmp[1]);
                        }
                        sds_rtos(ptTmp[2],-1,-1,szTest);
                        ::GetWindowText(::GetDlgItem(m_hWnd,SPLINE_CTL_Z),szTmp,IC_ACADBUF-1);
                        if(!strisame(szTest,szTmp)) {
                            sds_distof(szTmp,-1,&ptTmp[2]);
                        }
                        sds_trans(ptTmp,&ucs,&wcs,0,pCcur->prbb->resval.rpoint);
                    }
                }
                break;
            case RTSTR:
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

ent_SPLINE_TAB::ent_SPLINE_TAB() : CDialog(ENT_SPLINE_TAB) {
    wcs.restype=ucs.restype=RTSHORT;
    wcs.resval.rint=0;
    ucs.resval.rint=1;
    pFitbb=pCtlbb=pFcur=pCcur=NULL;
    Count();
    CtlCurCnt=1;
    FitCurCnt=1;
    if(num_of_splines>1) {FitTot=CtlTot=0;}
}


//////////////////////////////////////////////////////////////////////////////
// TEXT       ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_TEXT_TAB, CDialog)
    ON_BN_CLICKED(TEXT_PICK,ent_OnPick)
    ON_BN_CLICKED(TEXT_PICK2,ent_OnPick2)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TEXTHTH,OnSpinTextHigh)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TEXTROT,OnSpinTextRot)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TEXTWIDTH,OnSpinTextWide)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TEXTQUE,OnSpinTextQue)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TEXT_ORIGIN_X,OnSpinOrgX)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TEXT_ORIGIN_Y,OnSpinOrgY)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TEXT_ORIGIN_Z,OnSpinOrgZ)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TEXT_ALIGN_X,OnSpinAliX)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TEXT_ALIGN_Y,OnSpinAliY)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TEXT_ALIGN_Z,OnSpinAliZ)
    ON_CBN_SELCHANGE(TEXT_JUSTIFY,OnJustChange)
    ON_CBN_SELCHANGE(TEXT_STYLE,OnStyleChange)
END_MESSAGE_MAP()

void ent_TEXT_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_TEXT_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_TEXT_TAB::OnSpinOrgX(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TEXT_ORIGIN_X),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TEXT_TAB::OnSpinOrgY(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TEXT_ORIGIN_Y),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TEXT_TAB::OnSpinOrgZ(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TEXT_ORIGIN_Z),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TEXT_TAB::OnSpinAliX(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TEXT_ALIGN_X),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TEXT_TAB::OnSpinAliY(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TEXT_ALIGN_Y),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TEXT_TAB::OnSpinAliZ(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TEXT_ALIGN_Z),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TEXT_TAB::OnSpinTextHigh(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TEXT_HEIGHT),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TEXT_TAB::OnSpinTextRot(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterAngle(::GetDlgItem(m_hWnd,TEXT_ROTATION),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TEXT_TAB::OnSpinTextWide(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
	//Modified Cybage SBD	04/03/2002 DD/MM/YYYY
	//Reason : Fix for Bug No. 77983 from Bugzilla
    //ent_SpinMasterReal(::GetDlgItem(m_hWnd,TEXT_WIDTH),pNMUpDown->iDelta);
	ent_SpinMasterReal(::GetDlgItem(m_hWnd,TEXT_WIDTH),pNMUpDown->iDelta,2);
    UpdateData(TRUE);
    *result=0;
}

void ent_TEXT_TAB::OnSpinTextQue(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterAngle(::GetDlgItem(m_hWnd,TEXT_OBLIQUE),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

BOOL ent_TEXT_TAB::OnInitDialog() {
	FillCombo();
    ent_StyleChoice();
    CDialog::OnInitDialog();
	// Bugzilla No. 76186; 11/11/2002 [
	//Current selection is  "Varies"...So add into ComboBox now
	if ( iTxtCurSel == 15)
	{
		CComboBox *pCombo = NULL;
		if (NULL != (pCombo = (CComboBox *) GetDlgItem(TEXT_JUSTIFY)))
		{
			int index = pCombo->AddString(ResourceString(IDC_ICADENTPROP_VARIES_18, "Varies" ));
			pCombo->SetCurSel(index); 

		}
	}
	// Bugzilla No. 76186; 11/11/2002 ]
    OnJustChange();
    return(TRUE);
}

void ent_TEXT_TAB::ent_OnPick() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick=1;
    ::SendMessage(hWnd,WM_CLOSE,TEXT_PICK,0);
}

void ent_TEXT_TAB::ent_OnPick2() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick2=1;
    ::SendMessage(hWnd,WM_CLOSE,TEXT_PICK2,0);
}

void ent_TEXT_TAB::ent_Yes10(){
    ::EnableWindow(::GetDlgItem(m_hWnd,TEXT_ORIGIN_X),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,TEXT_ORIGIN_Y),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,TEXT_ORIGIN_Z),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_TEXT_ORIGIN_X),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_TEXT_ORIGIN_Y),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_TEXT_ORIGIN_Z),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,TEXT_PICK),TRUE);
}

void ent_TEXT_TAB::ent_No10(){
    ::EnableWindow(::GetDlgItem(m_hWnd,TEXT_ORIGIN_X),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,TEXT_ORIGIN_Y),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,TEXT_ORIGIN_Z),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_TEXT_ORIGIN_X),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_TEXT_ORIGIN_Y),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_TEXT_ORIGIN_Z),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,TEXT_PICK),FALSE);
}

void ent_TEXT_TAB::ent_Yes11(){
    char szTmp[IC_ACADBUF];
    if(::EnableWindow(::GetDlgItem(m_hWnd,TEXT_ALIGN_X),TRUE)){
        ::GetWindowText(::GetDlgItem(m_hWnd,TEXT_ORIGIN_X),szTmp,IC_ACADBUF-1);
        ::SetWindowText(::GetDlgItem(m_hWnd,TEXT_ALIGN_X),szTmp);
    }
    if(::EnableWindow(::GetDlgItem(m_hWnd,TEXT_ALIGN_Y),TRUE)){
        ::GetWindowText(::GetDlgItem(m_hWnd,TEXT_ORIGIN_Y),szTmp,IC_ACADBUF-1);
        ::SetWindowText(::GetDlgItem(m_hWnd,TEXT_ALIGN_Y),szTmp);
    }
    if(::EnableWindow(::GetDlgItem(m_hWnd,TEXT_ALIGN_Z),TRUE)){
        ::GetWindowText(::GetDlgItem(m_hWnd,TEXT_ORIGIN_Z),szTmp,IC_ACADBUF-1);
        ::SetWindowText(::GetDlgItem(m_hWnd,TEXT_ALIGN_Z),szTmp);
    }
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_TEXT_ALIGN_X),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_TEXT_ALIGN_Y),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_TEXT_ALIGN_Z),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,TEXT_PICK2),TRUE);
}

void ent_TEXT_TAB::ent_No11(){
    ::EnableWindow(::GetDlgItem(m_hWnd,TEXT_ALIGN_X),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,TEXT_ALIGN_Y),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,TEXT_ALIGN_Z),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_TEXT_ALIGN_X),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_TEXT_ALIGN_Y),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_TEXT_ALIGN_Z),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,TEXT_PICK2),FALSE);
}

void ent_TEXT_TAB::ent_Yes40(){
    ::EnableWindow(::GetDlgItem(m_hWnd,TEXT_HEIGHT),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_TEXTHTH),TRUE);
}

void ent_TEXT_TAB::ent_No40(){
    ::EnableWindow(::GetDlgItem(m_hWnd,TEXT_HEIGHT),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_TEXTHTH),FALSE);
}

void ent_TEXT_TAB::ent_Yes50(){
    ::EnableWindow(::GetDlgItem(m_hWnd,TEXT_ROTATION),TRUE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_TEXTROT),TRUE);
}

void ent_TEXT_TAB::ent_No50(){
    ::EnableWindow(::GetDlgItem(m_hWnd,TEXT_ROTATION),FALSE);
    ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_TEXTROT),FALSE);
}

//Grey out the correct field.
void ent_TEXT_TAB::OnJustChange() {
    UpdateData(TRUE);

    if((iJust72&1 && iJust73&1) || iJust72&8 || iJust72&32){
        ent_Yes10();
    }else{
        ent_No10();
    }
    if(iJust72&1 && iJust73&1){
        ent_No11();
    }else{
        ent_Yes11();
    }
    if(iJust72&8){
        ent_No40();
    }else{
        ent_Yes40();
    }
    if(iJust72&8 || iJust72&32){
        ent_No50();
    }else{
        ent_Yes50();
    }
}

//Set current selection
void ent_TEXT_TAB::ent_SetSel() {
    switch(iJust72) {
        case 1:
            switch(iJust73) {
                case 1:
                    iTxtCurSel=0;
                    break;
                case 2:
                    iTxtCurSel=1;
                    break;
                case 4:
                    iTxtCurSel=2;
                    break;
                case 8:
                    iTxtCurSel=3;
                    break;
                default:
                    iTxtCurSel=15;
                    break;
            }
            break;
        case 2:
            switch(iJust73) {
                case 1:
                    iTxtCurSel=4;
                    break;
                case 2:
                    iTxtCurSel=5;
                    break;
                case 4:
                    iTxtCurSel=6;
                    break;
                case 8:
                    iTxtCurSel=7;
                    break;
                default:
                    iTxtCurSel=15;
                    break;
            }
            break;
        case 4:
            switch(iJust73) {
                case 1:
                    iTxtCurSel=8;
                    break;
                case 2:
                    iTxtCurSel=9;
                    break;
                case 4:
                    iTxtCurSel=10;
                    break;
                case 8:
                    iTxtCurSel=11;
                    break;
                default:
                    iTxtCurSel=15;
                    break;
            }
            break;
        case 8:
            iTxtCurSel=12;
            break;
        case 16:
            iTxtCurSel=13;
            break;
        case 32:
            iTxtCurSel=14;
            break;
        default:
            iTxtCurSel=15;
            break;
    }
}

//Fill members from the index of the combo box
void ent_TEXT_TAB::ent_SelChange() {
    switch(iTxtCurSel) {
        case 0:
            iJust72=1;
            iJust73=1;
            break;
        case 1:
            iJust72=1;
            iJust73=2;
            break;
        case 2:
            iJust72=1;
            iJust73=4;
            break;
        case 3:
            iJust72=1;
            iJust73=8;
            break;
        case 4:
            iJust72=2;
            iJust73=1;
            break;
        case 5:
            iJust72=2;
            iJust73=2;
            break;
        case 6:
            iJust72=2;
            iJust73=4;
            break;
        case 7:
            iJust72=2;
            iJust73=8;
            break;
        case 8:
            iJust72=4;
            iJust73=1;
            break;
        case 9:
            iJust72=4;
            iJust73=2;
            break;
        case 10:
            iJust72=4;
            iJust73=4;
            break;
        case 11:
            iJust72=4;
            iJust73=8;
            break;
        case 12:
            iJust72=8;
            iJust73=1;
            break;
        case 13:
            iJust72=16;
            iJust73=1;
            break;
        case 14:
            iJust72=32;
            iJust73=1;
            break;
        case 15:
            iJust72=63;
            iJust73=15;
            break;
    }
}

//This will read in the styles
//into the drop down list.
void ent_TEXT_TAB::ent_StyleChoice() {
    int i;
	CString csTmp;

	::SendMessage(::GetDlgItem(m_hWnd,TEXT_STYLE),CB_RESETCONTENT,0,0);
//Changed by Vitaly Spirin    SDS_StartTblRead(ResourceString(IDC_ICADENTPROP_STYLE_21, "STYLE" ));
    SDS_StartTblRead("STYLE");
	while(SDS_TblReadHasMore()) {
    	csTmp=SDS_TblReadString();
    	if(csTmp.GetLength() == 0) continue; // EBATECH(CNBR) 2002/4/24 STYPE for SHAPE has null NAME.
        if(csTmp.Find('|')>0) continue;
        ::SendMessage(::GetDlgItem(m_hWnd,TEXT_STYLE),CB_ADDSTRING,0,(LPARAM)(LPCTSTR)csTmp);
    }

    for(i=0;ent_DlgItems[i].ID!=TEXT_STYLE;i++);
    if(ent_DlgItems[i].DspValue!=NULL && strisame(ent_DlgItems[i].DspValue,ResourceString(IDC_ICADENTPROP_VARIES_18, "Varies" )))
        ::SendMessage(::GetDlgItem(m_hWnd,TEXT_STYLE),CB_ADDSTRING,0,(LPARAM)ResourceString(IDC_ICADENTPROP_VARIES_18, "Varies" ));
}

void ent_TEXT_TAB::OnStyleChange() {
    char szTmp[IC_ACADBUF];
    struct resbuf *pRb,*pRbb=NULL;

    ::GetWindowText(::GetDlgItem(m_hWnd,TEXT_STYLE),szTmp,sizeof(szTmp)-1);
    if((pRbb=sds_tblsearch("STYLE"/*DNT*/,szTmp,0))==NULL) return;
    for(pRb=pRbb;pRb!=NULL;pRb=pRb->rbnext){
        switch(pRb->restype){
            case 40:
                if(!icadRealEqual(pRb->resval.rreal,0.0)){
                    sds_rtos(pRb->resval.rreal,-1,-1,szTmp);
                    ::SetWindowText(::GetDlgItem(m_hWnd,TEXT_HEIGHT),szTmp);
                }
                break;
            case 41:
                sds_rtos(pRb->resval.rreal,-1,-1,szTmp);
                ::SetWindowText(::GetDlgItem(m_hWnd,TEXT_WIDTH),szTmp);
                break;
            case 50:
                sds_angtos(pRb->resval.rreal,-1,-1,szTmp);
                ::SetWindowText(::GetDlgItem(m_hWnd,TEXT_OBLIQUE),szTmp);
                break;
            case 71:
                if(pRb->resval.rint&2) {
                    ::SendMessage(::GetDlgItem(m_hWnd,TEXT_GENERATION1),BM_SETCHECK,BST_CHECKED,0);
                }else{
                    ::SendMessage(::GetDlgItem(m_hWnd,TEXT_GENERATION1),BM_SETCHECK,BST_UNCHECKED,0);
                }
                if(pRb->resval.rint&4) {
                    ::SendMessage(::GetDlgItem(m_hWnd,TEXT_GENERATION2),BM_SETCHECK,BST_CHECKED,0);
                }else{
                    ::SendMessage(::GetDlgItem(m_hWnd,TEXT_GENERATION2),BM_SETCHECK,BST_UNCHECKED,0);
                }
                break;
        }
    }
    IC_RELRB(pRbb);
    UpdateData(true);
}

void ent_TEXT_TAB::DoDataExchange(CDataExchange *pDX) {
    int index,iLen,start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_TEXT) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_TEXT) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RTREAL:
            case RT3DPOINT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                } else { //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    DDV_MaxChars(pDX,csTmp,511);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                } else { //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    DDV_MaxChars(pDX,csTmp,511);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSHORT:
            case RTSTR:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    if(ent_DlgItems[i].ID==TEXT_STYLE) {
                        index=::SendMessage(::GetDlgItem(m_hWnd,TEXT_STYLE),CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)ent_DlgItems[i].DspValue);
                        DDX_CBIndex(pDX,TEXT_STYLE,index);
                    }else{
                        csTmp=ent_DlgItems[i].DspValue;
                        DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    }
                } else { //Retrieving data from the dialog
                    if(ent_DlgItems[i].ID==TEXT_STYLE) {
                        DDX_CBIndex(pDX,TEXT_STYLE,index);
                            iLen=::SendMessage(::GetDlgItem(m_hWnd,TEXT_STYLE),CB_GETLBTEXTLEN,(WPARAM)index,0);
                            ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,iLen+1);
                            ::SendMessage(::GetDlgItem(m_hWnd,TEXT_STYLE),CB_GETLBTEXT,(WPARAM)index,(LPARAM)ent_DlgItems[i].DspValue);
                    }else{
                        DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                        DDV_MaxChars(pDX,csTmp,511);
                        if(csTmp.IsEmpty()) {
                            if(ent_DlgItems[i].DspValue!=NULL){
                                IC_FREE(ent_DlgItems[i].DspValue);
                                ent_DlgItems[i].DspValue=NULL;
                            }
                        }else{
                            ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                            strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                        }
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                } else { //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
            case ENT_JUSTI:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    if(ent_DlgItems[i].DXFCode==72) {
                        iJust72=ent_DlgItems[i].CheckBit;
                    }else if(ent_DlgItems[i].DXFCode==73) {
                        iJust73=ent_DlgItems[i].CheckBit;
                        ent_SetSel();
                        DDX_CBIndex(pDX,TEXT_JUSTIFY,iTxtCurSel);
                    }
                } else { //Retrieving data from the dialog
                    DDX_CBIndex(pDX,TEXT_JUSTIFY,iTxtCurSel);
                    ent_SelChange();
                    if(ent_DlgItems[i].DXFCode==72) {
                        ent_DlgItems[i].CheckBit=iJust72;
                    }else if(ent_DlgItems[i].DXFCode==73) {
                        ent_DlgItems[i].CheckBit=iJust73;
                    }
                }
        }
    }
}

ent_TEXT_TAB::ent_TEXT_TAB() : CDialog(ENT_TEXT_TAB) {
    ent_Pick=ent_Pick2=0;
}

void ent_TEXT_TAB::FillCombo()
{
	CComboBox *pCombo = NULL;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(TEXT_JUSTIFY)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_TEXT_JUSTIFY_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_TEXT_JUSTIFY_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_TEXT_JUSTIFY_3);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_TEXT_JUSTIFY_4);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_TEXT_JUSTIFY_5);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_TEXT_JUSTIFY_6);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_TEXT_JUSTIFY_7);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_TEXT_JUSTIFY_8);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_TEXT_JUSTIFY_9);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_TEXT_JUSTIFY_10);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_TEXT_JUSTIFY_11);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_TEXT_JUSTIFY_12);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_TEXT_JUSTIFY_13);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_TEXT_JUSTIFY_14);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_TEXT_JUSTIFY_15);
	pCombo->AddString(comboOption);
	// Bugzilla No. 76186; 11/11/2002 
	// Add "Varies only if necessary"
	//comboOption.LoadString(IDC_COMBO_TEXT_JUSTIFY_16);
	//pCombo->AddString(comboOption);
}


//////////////////////////////////////////////////////////////////////////////
// TOLERANCE  ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_TOLER_TAB,CDialog)
    ON_BN_CLICKED(TOLE_PICK1,ent_OnPick)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TOLE_STX,OnSpinX)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TOLE_STY,OnSpinY)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TOLE_STZ,OnSpinZ)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL ent_TOLER_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_TOLER_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_TOLER_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_TOLER_TAB::OnSpinX(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TOLE_STX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TOLER_TAB::OnSpinY(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TOLE_STY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TOLER_TAB::OnSpinZ(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TOLE_STZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

BOOL ent_TOLER_TAB::OnInitDialog() {
    return(CDialog::OnInitDialog());
}

void ent_TOLER_TAB::ent_OnPick() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick=1;
    ::SendMessage(hWnd,WM_CLOSE,0,0);
}

void ent_TOLER_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_TOLERANCE) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_TOLERANCE) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

ent_TOLER_TAB::ent_TOLER_TAB() : CDialog(ENT_TOLER_TAB) {
    ent_Pick=0;
}


//////////////////////////////////////////////////////////////////////////////
// TRACE      ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_TRACE_TAB,CDialog)
    ON_BN_CLICKED(TRACE_PICK1,ent_OnPick1)
    ON_BN_CLICKED(TRACE_PICK2,ent_OnPick2)
    ON_BN_CLICKED(TRACE_PICK3,ent_OnPick3)
    ON_BN_CLICKED(TRACE_PICK4,ent_OnPick4)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TRACE_P1X,OnSpinPt1x)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TRACE_P1Y,OnSpinPt1y)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TRACE_P1Z,OnSpinPt1z)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TRACE_P2X,OnSpinPt2x)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TRACE_P2Y,OnSpinPt2y)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TRACE_P2Z,OnSpinPt2z)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TRACE_P3X,OnSpinPt3x)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TRACE_P3Y,OnSpinPt3y)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TRACE_P3Z,OnSpinPt3z)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TRACE_P4X,OnSpinPt4x)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TRACE_P4Y,OnSpinPt4y)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_TRACE_P4Z,OnSpinPt4z)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL ent_TRACE_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_TRACE_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_TRACE_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_TRACE_TAB::OnSpinPt1x(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TRACE_P1X),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TRACE_TAB::OnSpinPt1y(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TRACE_P1Y),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TRACE_TAB::OnSpinPt1z(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TRACE_P1Z),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TRACE_TAB::OnSpinPt2x(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TRACE_P2X),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TRACE_TAB::OnSpinPt2y(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TRACE_P2Y),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TRACE_TAB::OnSpinPt2z(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TRACE_P2Z),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TRACE_TAB::OnSpinPt3x(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TRACE_P3X),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TRACE_TAB::OnSpinPt3y(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TRACE_P3Y),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TRACE_TAB::OnSpinPt3z(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TRACE_P3Z),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TRACE_TAB::OnSpinPt4x(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TRACE_P4X),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TRACE_TAB::OnSpinPt4y(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TRACE_P4Y),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_TRACE_TAB::OnSpinPt4z(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,TRACE_P4Z),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

BOOL ent_TRACE_TAB::OnInitDialog() {

    return(CDialog::OnInitDialog());
}

void ent_TRACE_TAB::ent_OnPick1() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick1=1;
    ::SendMessage(hWnd,WM_CLOSE,TRACE_PICK1,0);
}

void ent_TRACE_TAB::ent_OnPick2() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick2=1;
    ::SendMessage(hWnd,WM_CLOSE,TRACE_PICK2,0);
}

void ent_TRACE_TAB::ent_OnPick3() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick3=1;
    ::SendMessage(hWnd,WM_CLOSE,TRACE_PICK3,0);
}

void ent_TRACE_TAB::ent_OnPick4() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick4=1;
    ::SendMessage(hWnd,WM_CLOSE,TRACE_PICK4,0);
}

void ent_TRACE_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_TRACE) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_TRACE) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

ent_TRACE_TAB::ent_TRACE_TAB() : CDialog(ENT_TRACE_TAB) {
    ent_Pick1=ent_Pick2=ent_Pick3=ent_Pick4=0;
}


//////////////////////////////////////////////////////////////////////////////
// VIEWPORT   ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_VIEWP_TAB,CDialog)
    ON_BN_CLICKED(VIEWP_PICK1,ent_OnPick)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_VIEWP_CPX,OnSpinCpx)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_VIEWP_CPY,OnSpinCpy)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_VIEWP_CPZ,OnSpinCpz)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_VIEWP_HEIGHT,OnSpinHigh)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_VIEWP_WIDTH,OnSpinWide)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL ent_VIEWP_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_VIEWP_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_VIEWP_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}


void ent_VIEWP_TAB::OnSpinCpx(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,VIEWP_CPX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_VIEWP_TAB::OnSpinCpy(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,VIEWP_CPY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_VIEWP_TAB::OnSpinCpz(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,VIEWP_CPZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_VIEWP_TAB::OnSpinHigh(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,VIEWP_HEIGHT),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_VIEWP_TAB::OnSpinWide(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,VIEWP_WIDTH),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

BOOL ent_VIEWP_TAB::OnInitDialog() {
    return(CDialog::OnInitDialog());
}

void ent_VIEWP_TAB::ent_OnPick() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick=1;
    ::SendMessage(hWnd,WM_CLOSE,0,0);
}

void ent_VIEWP_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_VIEWPORT) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_VIEWPORT) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

ent_VIEWP_TAB::ent_VIEWP_TAB() : CDialog(ENT_VIEWP_TAB) {
    ent_Pick=0;
}


//////////////////////////////////////////////////////////////////////////////
// XLINE      ****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_XLINE_TAB,CDialog)
    ON_BN_CLICKED(XLINE_PICK1,ent_OnPick1)
    ON_BN_CLICKED(XLINE_PICK2,ent_OnPick2)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_XLINE_STX,OnSpinStx)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_XLINE_STY,OnSpinSty)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_XLINE_STZ,OnSpinStz)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_XLINE_EDX,OnSpinEdx)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_XLINE_EDY,OnSpinEdy)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_XLINE_EDZ,OnSpinEdz)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL ent_XLINE_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return(true);
}

void ent_XLINE_TAB::OnOK(){
    HWND hParentWnd;

    ent_bHitEnter=true;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_XLINE_TAB::OnCancel(){
    HWND hParentWnd;

    ent_bHitEnter=false;
    hParentWnd=::GetParent(m_hWnd);
    ::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void ent_XLINE_TAB::OnSpinStx(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,XLINE_STX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_XLINE_TAB::OnSpinSty(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,XLINE_STY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_XLINE_TAB::OnSpinStz(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,XLINE_STZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_XLINE_TAB::OnSpinEdx(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,XLINE_EDX),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_XLINE_TAB::OnSpinEdy(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,XLINE_EDY),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ent_XLINE_TAB::OnSpinEdz(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ent_SpinMasterReal(::GetDlgItem(m_hWnd,XLINE_EDZ),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

BOOL ent_XLINE_TAB::OnInitDialog() {

    return(CDialog::OnInitDialog());
}

void ent_XLINE_TAB::ent_OnPick1() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick1=1;
    ::SendMessage(hWnd,WM_CLOSE,XLINE_PICK1,0);
}

void ent_XLINE_TAB::ent_OnPick2() {
    HWND hWnd;

    hWnd=::GetParent(m_hWnd);
    ent_Pick2=1;
    ::SendMessage(hWnd,WM_CLOSE,XLINE_PICK2,0);
}

void ent_XLINE_TAB::DoDataExchange(CDataExchange *pDX) {
    int start,end,i;

    for(start=0;(ent_DlgItems[start].EntTyp!=DB_XLINE) && (start<ent_num_of_vars);start++);
    start+=5; //Skip the layer,linetype,linetypescale,thickness and color
    for(end=start;(ent_DlgItems[end+1].EntTyp==DB_XLINE) && (end<ent_num_of_vars);end++);

    for(i=start;i<=end;i++){
        switch(ent_DlgItems[i].ResType){
            case RT3DPOINT:
            case RTREAL:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTANG:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
                break;
            case RTSTR:
            case RTSHORT:
                if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
                    csTmp=ent_DlgItems[i].DspValue;
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Text(pDX,ent_DlgItems[i].ID,csTmp);
                    if(csTmp.IsEmpty()) {
                        if(ent_DlgItems[i].DspValue!=NULL){
                            IC_FREE(ent_DlgItems[i].DspValue);
                            ent_DlgItems[i].DspValue=NULL;
                        }
                    }else{
                        ent_DlgItems[i].DspValue=ic_realloc_char(ent_DlgItems[i].DspValue,csTmp.GetLength()+1);
                        strcpy(ent_DlgItems[i].DspValue,(LPCTSTR)csTmp);
                    }
                }
            break;
            case ENT_CHECK:
                if(!pDX->m_bSaveAndValidate){ //Moving data to the dialog
                    iTmp=ent_DlgItems[i].CheckBit;
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
                    DDX_Check(pDX,ent_DlgItems[i].ID,iTmp);
                    ent_DlgItems[i].CheckBit=iTmp;
                }
            break;
            case ENT_GREY:
                if(!pDX->m_bSaveAndValidate && ent_DlgItems[i].CheckBit){ //Moving data to the dialog
                    ::EnableWindow(::GetDlgItem(m_hWnd,ent_DlgItems[i].ID),TRUE);
                }
            break;
        }
    }
}

ent_XLINE_TAB::ent_XLINE_TAB() : CDialog(ENT_XLINE_TAB) {
    ent_Pick1=ent_Pick2=0;
}

//////////////////////////////////////////////////////////////////////////////
// 3DSOLID ENTITY*************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_3DSOLID_TAB, CDialog)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()


BOOL
ent_3DSOLID_TAB::OnInitDialog()
{
    return(CDialog::OnInitDialog());
}

BOOL
ent_3DSOLID_TAB::OnHelpInfo
(
 HELPINFO*	pHel
)
{
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return TRUE;
}

void
ent_3DSOLID_TAB::OnOK()
{
    HWND	hParentWnd = ::GetParent(m_hWnd);

    ent_bHitEnter = true;
    ::SendMessage(hParentWnd, WM_CLOSE, 0, 0);
}

void
ent_3DSOLID_TAB::OnCancel()
{
    HWND	hParentWnd = ::GetParent(m_hWnd);

    ent_bHitEnter = false;
    ::SendMessage(hParentWnd, WM_CLOSE, 0, 0);
}

void
ent_3DSOLID_TAB::DoDataExchange
(
 CDataExchange*	pDX
)
{
	//return if retrieving data from the dialog
	if (pDX->m_bSaveAndValidate)
		return;

	int	start, end, i;

	for(start = 0; (ent_DlgItems[start].EntTyp != DB_3DSOLID) && (start < ent_num_of_vars); ++start)
		;
//	start += 5;		//Skip the layer,linetype,linetypescale,thickness and color
	start += 6;		//Skip the layer,linetype,linetypescale,thickness, color, and lineweight - SWH
	for(end = start; (ent_DlgItems[end+1].EntTyp == DB_3DSOLID) && (end < ent_num_of_vars); ++end)
		;

	CString buf;
	for(i = start; i < end; ++i)
	{
		ent_DlgItem& item = ent_DlgItems[i];
		buf = item.DspValue;
		DDX_Text(pDX, item.ID, buf);
	}	// for
}

//////////////////////////////////////////////////////////////////////////////
// REGION ENTITY****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_REGION_TAB, CDialog)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()


BOOL
ent_REGION_TAB::OnInitDialog()
{
    return(CDialog::OnInitDialog());
}

BOOL
ent_REGION_TAB::OnHelpInfo
(
 HELPINFO*	pHel
)
{
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return TRUE;
}

void
ent_REGION_TAB::OnOK()
{
    HWND	hParentWnd = ::GetParent(m_hWnd);

    ent_bHitEnter = true;
    ::SendMessage(hParentWnd, WM_CLOSE, 0, 0);
}

void
ent_REGION_TAB::OnCancel()
{
    HWND	hParentWnd = ::GetParent(m_hWnd);

    ent_bHitEnter = false;
    ::SendMessage(hParentWnd, WM_CLOSE, 0, 0);
}

void
ent_REGION_TAB::DoDataExchange
(
 CDataExchange*	pDX
)
{
	//return if retrieving data from the dialog
	if (pDX->m_bSaveAndValidate)
		return;

	int	start, end, i;

	for(start = 0; (ent_DlgItems[start].EntTyp != DB_REGION) && (start < ent_num_of_vars); ++start)
		;
//	start += 5;		//Skip the layer,linetype,linetypescale,thickness and color
	start += 6;		//Skip the layer,linetype,linetypescale,thickness, color, and lineweight - SWH

	for(end = start; (ent_DlgItems[end+1].EntTyp == DB_REGION) && (end < ent_num_of_vars); ++end)
		;

	CString buf;
	for(i = start; i < end; ++i)
	{
		ent_DlgItem& item = ent_DlgItems[i];
		buf = item.DspValue;
		DDX_Text(pDX, item.ID, buf);
	}	// for
}

//////////////////////////////////////////////////////////////////////////////
// BODY ENTITY****************************************************************
//////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ent_BODY_TAB, CDialog)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()


BOOL
ent_BODY_TAB::OnInitDialog()
{
    return(CDialog::OnInitDialog());
}

BOOL
ent_BODY_TAB::OnHelpInfo
(
 HELPINFO*	pHel
)
{
	CommandRefHelp( m_hWnd, HLP_ENTPROP);
	return TRUE;
}

void
ent_BODY_TAB::OnOK()
{
    HWND	hParentWnd = ::GetParent(m_hWnd);

    ent_bHitEnter = true;
    ::SendMessage(hParentWnd, WM_CLOSE, 0, 0);
}

void
ent_BODY_TAB::OnCancel()
{
    HWND	hParentWnd = ::GetParent(m_hWnd);

    ent_bHitEnter = false;
    ::SendMessage(hParentWnd, WM_CLOSE, 0, 0);
}

void
ent_BODY_TAB::DoDataExchange
(
 CDataExchange*	pDX
)
{
	//return if retrieving data from the dialog
	if (pDX->m_bSaveAndValidate)
		return;

	int	start = 0;
	for(; (ent_DlgItems[start].EntTyp != DB_BODY) && (start < ent_num_of_vars); ++start);
//	start += 5;		//Skip the layer,linetype,linetypescale,thickness and color
	start += 6;		//Skip the layer,linetype,linetypescale,thickness, color, and lineweight - SWH

	int end = start;
	for(; (ent_DlgItems[end+1].EntTyp == DB_BODY) && (end < ent_num_of_vars); ++end);

	CString buf;
	for(int i = start; i < end; ++i)
	{
		ent_DlgItem& item = ent_DlgItems[i];
		buf = item.DspValue;
		DDX_Text(pDX, item.ID, buf);
	}	// for
}
