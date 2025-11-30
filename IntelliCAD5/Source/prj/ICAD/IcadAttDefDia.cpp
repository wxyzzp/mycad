/* F:\BLD\PRJ\ICAD\ICADATTDEFDIA.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 *
 */

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "IcadDoc.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "resource.hm"/*DNT*/
#include "paths.h"
#include "IcadDialog.h"

class att_CAttDef : public IcadDialog {
    public:
        int iHidden,iConstant,iVerify,iPreset,iFlag72,iFlag74,iCurJust;
        double dInsPt[3],dHeight,dRotate;
        CString csTmp,csPrompt,csTag,csValue,csStyle;
        CComboBox m_CStyle,m_CJust;
        att_CAttDef();
        BOOL OnInitDialog();
        void ent_SpinMasterReal(HWND hWndItem,int increment);
        void ent_SpinMasterAngle(HWND hWndItem,int increment);
    protected:
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
        afx_msg void OnHelp();
        afx_msg void OnDefine();
        afx_msg void OnPick();
        afx_msg void OnHighPick();
        afx_msg void OnRotatePick();
		//Modified Cybage AW 10-09-01 [
		//Reason : Bug#77841 in BugZilla
		afx_msg void OnConstantSel();
		//Modified Cybage AW 10-09-01 ]
        afx_msg void OnSpinX(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinY(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinZ(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinHigh(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinRotate(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnKillFocus();
        virtual void OnOK();
        virtual void DoDataExchange(CDataExchange *pDX);
		void		 FillCombo();

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(att_CAttDef,CDialog)
    ON_EN_KILLFOCUS(ATT_ROTATE,OnKillFocus)
    ON_BN_CLICKED(ATT_PICK,OnPick)
    ON_BN_CLICKED(ATT_CREATE,OnDefine)
    ON_BN_CLICKED(ATT_HIGH_PICK,OnHighPick)
    ON_BN_CLICKED(ATT_ROTATE_PICK,OnRotatePick)
	//Modified Cybage AW 10-09-01 [
	//Reason : Bug#77841 in BugZilla
	ON_BN_CLICKED(ATT_CONSTANT,OnConstantSel)
	//Modified Cybage AW 10-09-01 ]
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATT_IPT_X,OnSpinX)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATT_IPT_Y,OnSpinY)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATT_IPT_Z,OnSpinZ)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATT_HEIGHT,OnSpinHigh)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_ATT_ROTATE,OnSpinRotate)
	ON_WM_HELPINFO()
    ON_BN_CLICKED(IDHELP,OnHelp)
END_MESSAGE_MAP()

BOOL att_CAttDef::OnHelpInfo(HELPINFO *pHel){
	OnHelp();
	return(true);
}

void att_CAttDef::OnHelp() {
	CommandRefHelp( m_hWnd, HLP_DDATTDEF);
}

void att_CAttDef::OnDefine() {
    struct resbuf *rbNewAtt=NULL,rbent,rbucs;
	sds_point extru,ptemp;
    double thickness;

	SDS_getvar(NULL,DB_QTHICKNESS,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	thickness=rbucs.resval.rreal;
    SDS_getvar(NULL,DB_QUCSXDIR,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SDS_getvar(NULL,DB_QUCSYDIR,&rbent,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_crossproduct(rbucs.resval.rpoint,rbent.resval.rpoint,extru);
	rbent.restype=RT3DPOINT;
	ic_ptcpy(rbent.resval.rpoint,extru);
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;

    if(!UpdateData(TRUE)) return;
    ::GetWindowText(::GetDlgItem(m_hWnd,ATT_STYLE),csStyle.GetBuffer(IC_ACADBUF),IC_ACADBUF-1);
    csStyle.ReleaseBuffer();
    switch(m_CJust.GetCurSel()) {
        case 0:
            iFlag72=IC_TEXT_JUST_LEFT;
            iFlag74=IC_TEXT_VALIGN_BASELINE;
            break;
        case 1:
            iFlag72=IC_TEXT_JUST_LEFT;
            iFlag74=IC_TEXT_VALIGN_BOTTOM;
            break;
        case 2:
            iFlag72=IC_TEXT_JUST_LEFT;
            iFlag74=IC_TEXT_VALIGN_MIDDLE;
            break;
        case 3:
            iFlag72=IC_TEXT_JUST_LEFT;
            iFlag74=IC_TEXT_VALIGN_TOP;
            break;
        case 4:
            iFlag72=IC_TEXT_JUST_CENTER;
            iFlag74=IC_TEXT_VALIGN_BASELINE;
            break;
        case 5:
            iFlag72=IC_TEXT_JUST_CENTER;
            iFlag74=IC_TEXT_VALIGN_BOTTOM;
            break;
        case 6:
            iFlag72=IC_TEXT_JUST_CENTER;
            iFlag74=IC_TEXT_VALIGN_MIDDLE;
            break;
        case 7:
            iFlag72=IC_TEXT_JUST_CENTER;
            iFlag74=IC_TEXT_VALIGN_TOP;
            break;
        case 8:
            iFlag72=IC_TEXT_JUST_RIGHT;
            iFlag74=IC_TEXT_VALIGN_BASELINE;
            break;
        case 9:
            iFlag72=IC_TEXT_JUST_RIGHT;
            iFlag74=IC_TEXT_VALIGN_BOTTOM;
            break;
        case 10:
            iFlag72=IC_TEXT_JUST_RIGHT;
            iFlag74=IC_TEXT_VALIGN_MIDDLE;
            break;
        case 11:
            iFlag72=IC_TEXT_JUST_RIGHT;
            iFlag74=IC_TEXT_VALIGN_TOP;
            break;
        case 12:
            iFlag72=IC_TEXT_JUST_ALIGNED;
            iFlag74=IC_TEXT_VALIGN_BASELINE;
            break;
        case 13:
            iFlag72=IC_TEXT_JUST_MIDDLE;
            iFlag74=IC_TEXT_VALIGN_BASELINE;
            break;
        case 14:
            iFlag72=IC_TEXT_JUST_FIT;
            iFlag74=IC_TEXT_VALIGN_BASELINE;
            break;
    }
    if(csTag.IsEmpty()) {
        MessageBox(ResourceString(IDC_ICADATTDEFDIA_THE_ATTRIB_1, "The Attribute must have a name." ),ResourceString(IDC_ICADATTDEFDIA_ATTRIBUTE__2, "Attribute Name" ),MB_ICONEXCLAMATION|MB_OK|MB_APPLMODAL);
    }else{
        sds_entmake(NULL);
		sds_trans(dInsPt,&rbucs,&rbent,0,ptemp);
		sds_point anglept,temppt2;

// MR fix bug 47282 -- compute a base angle for the text rotation.  This is basically the same code
// as is used in cmd_textdtext, which is where I got it.  We compute a rotation angle within the UCS
// which is a base angle applied to the entity

		anglept[0]=1.0; anglept[1]=anglept[2]=0.0;
		sds_trans(anglept,&rbucs,&rbent,1,temppt2);
		sds_real baseangle=atan2(temppt2[1],temppt2[0]);

		//Modified Cybage AW 14-01-02 (dd-mm-yy)[
		//Reason :  If current text style is set with "Print text backward" and 
		//"Print text upsidedown", attributes are not drawn with these two proprties 
		//when created through the ddattdef dialog box
		struct resbuf *rbTextStyle;
		int nGen = 0;
		rbTextStyle=sds_tblsearch("STYLE"/*DNT*/,csStyle,0);
		if(ic_assoc(rbTextStyle,-1) == 0)
		{
			if(ic_assoc(rbTextStyle,71) == 0)
				nGen = ic_rbassoc->resval.rint;
		}
		sds_relrb(rbTextStyle);
		rbTextStyle = NULL;
		//Modified Cybage AW 14-01-02 (dd-mm-yy)]
		if((rbNewAtt=sds_buildlist(RTDXF0,"ATTDEF"/*DNT*/,
                                       10,ptemp,
                                       40,dHeight,
                                        1,csValue,
                                        3,csPrompt,
                                        2,csTag,
                                       70,(iHidden*IC_ATTDEF_INVIS)+(iConstant*IC_ATTDEF_CONST)+(iVerify*IC_ATTDEF_VERIFY)+(iPreset*IC_ATTDEF_PRESET),
                                       50,dRotate+baseangle,
                                        7,csStyle,
                                       72,iFlag72,
                                       74,iFlag74,
                                       11,ptemp,
									   39,thickness,
                                      210,extru,
									  //Modified Cybage AW 14-01-02 (dd-mm-yy)
									  //Reason :  If current text style is set with "Print text backward" and 
									  //"Print text upsidedown", attributes are not drawn with these two proprties 
									  //when created through the ddattdef dialog box
									  //Include the text generation flag in the Attribute buildlist.
									  71, nGen,
                                        0))==NULL) {
  	        MessageBox(ResourceString(IDC_ICADATTDEFDIA_MORE_MEMOR_3, "More memory is needed to do this operation" ),NULL,MB_OK|MB_ICONSTOP);
        }
        sds_entmake(rbNewAtt);
        IC_RELRB(rbNewAtt);
    }
	sds_polar(dInsPt,(dRotate-(IC_PI/2.0)),(dHeight*1.666666),dInsPt);
	UpdateData(false);

	::SetFocus(::GetDlgItem(m_hWnd,ATT_TAG));
}

void att_CAttDef::OnKillFocus() {
    if(!UpdateData(TRUE)) return;
    UpdateData(FALSE);
}

void att_CAttDef::OnPick() {
    if(!UpdateData(TRUE)) return;
    iCurJust=m_CJust.GetCurSel();
    EndDialog(ATT_PICK);
}

void att_CAttDef::OnHighPick() {
    if(!UpdateData(TRUE)) return;
    iCurJust=m_CJust.GetCurSel();
    EndDialog(ATT_HIGH_PICK);
}

void att_CAttDef::OnRotatePick() {
    if(!UpdateData(TRUE)) return;
    iCurJust=m_CJust.GetCurSel();
    EndDialog(ATT_ROTATE_PICK);
}

void att_CAttDef::OnSpinX(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    if(!UpdateData(TRUE)) return;
    att_CAttDef::ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATT_IPT_X),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void att_CAttDef::OnSpinY(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    if(!UpdateData(TRUE)) return;
    att_CAttDef::ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATT_IPT_Y),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void att_CAttDef::OnSpinZ(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    if(!UpdateData(TRUE)) return;
    att_CAttDef::ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATT_IPT_Z),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void att_CAttDef::OnSpinHigh(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    if(!UpdateData(TRUE)) return;
    att_CAttDef::ent_SpinMasterReal(::GetDlgItem(m_hWnd,ATT_HEIGHT),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void att_CAttDef::OnSpinRotate(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    if(!UpdateData(TRUE)) return;
    att_CAttDef::ent_SpinMasterAngle(::GetDlgItem(m_hWnd,ATT_ROTATE),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void att_CAttDef::ent_SpinMasterReal(HWND hWndItem,int increment) {
    double dTmp;
    char szTmp[IC_ACADBUF];

    ::GetWindowText(hWndItem,szTmp,IC_ACADBUF-1);
    sds_distof(szTmp,-1,&dTmp);
    dTmp+=(-increment/10.0);
    sds_rtos(dTmp,-1,-1,szTmp);
    ::SetWindowText(hWndItem,szTmp);
}

void att_CAttDef::ent_SpinMasterAngle(HWND hWndItem,int increment) {
    double dTmp;
    char szTmp[IC_ACADBUF];

    ::GetWindowText(hWndItem,szTmp,IC_ACADBUF-1);
    sds_angtof(szTmp,-1,&dTmp);
    dTmp+=(IC_PI/180)*(-increment);
    sds_angtos(dTmp,-1,-1,szTmp);
    ::SetWindowText(hWndItem,szTmp);
}

//Added Cybage AW 10-09-01 [
//Reason : Bug#77841 in BugZilla
void att_CAttDef::OnConstantSel()	
{
	CButton *pConst = NULL;
	pConst = (CButton *)GetDlgItem(ATT_CONSTANT);
	if(pConst  && m_hWnd)
	{
		if(pConst->GetCheck() == 1)
		{
			HWND hwnd = ::GetDlgItem(m_hWnd,ATT_PRESET);
			if(hwnd)	::EnableWindow(hwnd,FALSE);
			hwnd =		::GetDlgItem(m_hWnd,ATT_VERIFY);
			if(hwnd)	::EnableWindow(hwnd,FALSE);
			hwnd =		::GetDlgItem(m_hWnd,ATT_PROMPT);
			if(hwnd)	::EnableWindow(hwnd,FALSE);
		}
		else
		{
			HWND hwnd = ::GetDlgItem(m_hWnd,ATT_PRESET);
			if(hwnd)	::EnableWindow(hwnd,TRUE);
			hwnd =		::GetDlgItem(m_hWnd,ATT_VERIFY);
			if(hwnd)	::EnableWindow(hwnd,TRUE);
			hwnd =		::GetDlgItem(m_hWnd,ATT_PROMPT);
			if(hwnd)	::EnableWindow(hwnd,TRUE);
		}
	}
}
//Added Cybage AW 10-09-01 ]

void att_CAttDef::OnOK() {

    if(!UpdateData(TRUE)) return;
    ::GetWindowText(::GetDlgItem(m_hWnd,ATT_STYLE),csStyle.GetBuffer(IC_ACADBUF),IC_ACADBUF-1);
    csStyle.ReleaseBuffer();
    switch(m_CJust.GetCurSel()) {
        case 0:
            iFlag72=IC_TEXT_JUST_LEFT;
            iFlag74=IC_TEXT_VALIGN_BASELINE;
            break;
        case 1:
            iFlag72=IC_TEXT_JUST_LEFT;
            iFlag74=IC_TEXT_VALIGN_BOTTOM;
            break;
        case 2:
            iFlag72=IC_TEXT_JUST_LEFT;
            iFlag74=IC_TEXT_VALIGN_MIDDLE;
            break;
        case 3:
            iFlag72=IC_TEXT_JUST_LEFT;
            iFlag74=IC_TEXT_VALIGN_TOP;
            break;
        case 4:
            iFlag72=IC_TEXT_JUST_CENTER;
            iFlag74=IC_TEXT_VALIGN_BASELINE;
            break;
        case 5:
            iFlag72=IC_TEXT_JUST_CENTER;
            iFlag74=IC_TEXT_VALIGN_BOTTOM;
            break;
        case 6:
            iFlag72=IC_TEXT_JUST_CENTER;
            iFlag74=IC_TEXT_VALIGN_MIDDLE;
            break;
        case 7:
            iFlag72=IC_TEXT_JUST_CENTER;
            iFlag74=IC_TEXT_VALIGN_TOP;
            break;
        case 8:
            iFlag72=IC_TEXT_JUST_RIGHT;
            iFlag74=IC_TEXT_VALIGN_BASELINE;
            break;
        case 9:
            iFlag72=IC_TEXT_JUST_RIGHT;
            iFlag74=IC_TEXT_VALIGN_BOTTOM;
            break;
        case 10:
            iFlag72=IC_TEXT_JUST_RIGHT;
            iFlag74=IC_TEXT_VALIGN_MIDDLE;
            break;
        case 11:
            iFlag72=IC_TEXT_JUST_RIGHT;
            iFlag74=IC_TEXT_VALIGN_TOP;
            break;
        case 12:
            iFlag72=IC_TEXT_JUST_ALIGNED;
            iFlag74=IC_TEXT_VALIGN_BASELINE;
            break;
        case 13:
            iFlag72=IC_TEXT_JUST_MIDDLE;
            iFlag74=IC_TEXT_VALIGN_BASELINE;
            break;
        case 14:
            iFlag72=IC_TEXT_JUST_FIT;
            iFlag74=IC_TEXT_VALIGN_BASELINE;
            break;
    }
    if(csTag.IsEmpty()) {
        ::MessageBox(NULL,ResourceString(IDC_ICADATTDEFDIA_THE_ATTRIB_1, "The Attribute must have a name." ),ResourceString(IDC_ICADATTDEFDIA_ATTRIBUTE__2, "Attribute Name" ),MB_ICONSTOP|MB_OK|MB_TASKMODAL);
    }else{
        CDialog::OnOK();
    }
}

BOOL att_CAttDef::OnInitDialog() {
    struct resbuf rb;
    CString csTmp;

	FillCombo();
    IcadDialog::OnInitDialog();  //This will call UpdateData(FALSE)

//Changed by Vitaly Spirin	SDS_StartTblRead(ResourceString(IDC_ICADATTDEFDIA_STYLE_4, "STYLE" ));
	SDS_StartTblRead("STYLE");
	while(SDS_TblReadHasMore()) {
    	csTmp=SDS_TblReadString();
        if(csTmp.Find('|')>0) continue;
        m_CStyle.AddString(csTmp);
    }

	SDS_getvar(NULL,DB_QTEXTSTYLE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    m_CStyle.SelectString(-1,rb.resval.rstring);

    m_CJust.SetCurSel(iCurJust);

	//Added Cybage AW 11-02-02
	//"validate" & "defined" Attribute flags check box remains enabled 
	//when "Fixed" attribute flag  is checked. Bug number 77841 from BugZilla.
	OnConstantSel();

    return(true);
}

void att_CAttDef::FillCombo()
{
	CComboBox *pCombo = NULL;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(ATT_JUST)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_ATT_JUST_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATT_JUST_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATT_JUST_3);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATT_JUST_4);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATT_JUST_5);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATT_JUST_6);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATT_JUST_7);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATT_JUST_8);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATT_JUST_9);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATT_JUST_10);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATT_JUST_11);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATT_JUST_12);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATT_JUST_13);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATT_JUST_14);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ATT_JUST_15);
	pCombo->AddString(comboOption);
}

void att_CAttDef::DoDataExchange(CDataExchange *pDX) {

    if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
        sds_rtos(dInsPt[0],-1,-1,csTmp.GetBuffer(IC_ACADBUF));
        DDX_Text(pDX,ATT_IPT_X,csTmp);
        sds_rtos(dInsPt[1],-1,-1,csTmp.GetBuffer(IC_ACADBUF));
        DDX_Text(pDX,ATT_IPT_Y,csTmp);
        sds_rtos(dInsPt[2],-1,-1,csTmp.GetBuffer(IC_ACADBUF));
        DDX_Text(pDX,ATT_IPT_Z,csTmp);
        sds_angtos(dRotate,-1,-1,csTmp.GetBuffer(IC_ACADBUF));
        DDX_Text(pDX,ATT_ROTATE,csTmp);
        sds_rtos(dHeight,-1,-1,csTmp.GetBuffer(IC_ACADBUF));
        DDX_Text(pDX,ATT_HEIGHT,csTmp);
    }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
        DDX_Text(pDX,ATT_IPT_X,csTmp);
        if(sds_distof(csTmp,-1,&dInsPt[0])!=RTNORM){
            MessageBox(ResourceString(IDC_ICADATTDEFDIA_YOU_HAVE_E_5, "You have entered an invalid point." ),ResourceString(CMD_ERR_INVALIDPOINT, "Invalid Point Entered" ),MB_ICONERROR|MB_APPLMODAL|MB_OK);
            pDX->Fail();
        }
        DDX_Text(pDX,ATT_IPT_Y,csTmp);
        if(sds_distof(csTmp,-1,&dInsPt[1])!=RTNORM){
            MessageBox(ResourceString(IDC_ICADATTDEFDIA_YOU_HAVE_E_5, "You have entered an invalid point." ),ResourceString(CMD_ERR_INVALIDPOINT, "Invalid Point Entered" ),MB_ICONERROR|MB_APPLMODAL|MB_OK);
            pDX->Fail();
        }
        DDX_Text(pDX,ATT_IPT_Z,csTmp);
        if(sds_distof(csTmp,-1,&dInsPt[2])!=RTNORM){
            MessageBox(ResourceString(IDC_ICADATTDEFDIA_YOU_HAVE_E_5, "You have entered an invalid point." ),ResourceString(CMD_ERR_INVALIDPOINT, "Invalid Point Entered" ),MB_ICONERROR|MB_APPLMODAL|MB_OK);
            pDX->Fail();
        }
        DDX_Text(pDX,ATT_ROTATE,csTmp);
        if(sds_angtof(csTmp,-1,&dRotate)!=RTNORM){
            MessageBox(ResourceString(IDC_ICADATTDEFDIA_YOU_HAVE_E_7, "You have entered an invalid rotation angle." ),ResourceString(IDC_ICADATTDEFDIA_INVALID_AN_8, "Invalid Angle Entered" ),MB_ICONERROR|MB_APPLMODAL|MB_OK);
            pDX->Fail();
        }
        DDX_Text(pDX,ATT_HEIGHT,csTmp);
        if(sds_distof(csTmp,-1,&dHeight)!=RTNORM){
            MessageBox(ResourceString(IDC_ICADATTDEFDIA_YOU_HAVE_E_9, "You have entered an invalid height." ),ResourceString(IDC_ICADATTDEFDIA_INVALID_H_10, "Invalid Height Entered" ),MB_ICONERROR|MB_APPLMODAL|MB_OK);
            pDX->Fail();
        }
    }
    DDX_Text(pDX,ATT_PROMPT,csPrompt);
    DDX_Text(pDX,ATT_TAG,csTag);
    DDX_Text(pDX,ATT_VALUE,csValue);
    DDX_Check(pDX,ATT_HIDDEN,iHidden);
    DDX_Check(pDX,ATT_CONSTANT,iConstant);
    DDX_Check(pDX,ATT_VERIFY,iVerify);
    DDX_Check(pDX,ATT_PRESET,iPreset);
    DDX_Control(pDX,ATT_STYLE,m_CStyle);
    DDX_Control(pDX,ATT_JUST,m_CJust);
}

att_CAttDef::att_CAttDef():IcadDialog(ATT_DEF) {
    struct resbuf rb;

	SDS_getvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    dInsPt[0]=rb.resval.rpoint[0];
    dInsPt[1]=rb.resval.rpoint[1];
    dInsPt[2]=rb.resval.rpoint[2];
    dRotate=0.0;
    SDS_getvar(NULL,DB_QTEXTSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    dHeight=rb.resval.rreal;
	SDS_getvar(NULL,DB_QAFLAGS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    // Set the checkboxes to initially use the drawing defaults.
    iHidden  = (IC_ATTDEF_INVIS==(rb.resval.rint & IC_ATTDEF_INVIS));
	iConstant= (IC_ATTDEF_CONST==(rb.resval.rint & IC_ATTDEF_CONST));
	iVerify  = (IC_ATTDEF_VERIFY==(rb.resval.rint & IC_ATTDEF_VERIFY));
	iPreset  = (IC_ATTDEF_PRESET==(rb.resval.rint & IC_ATTDEF_PRESET));
    //
    iFlag72=IC_TEXT_JUST_LEFT;
    iFlag74=IC_TEXT_VALIGN_BASELINE;
    iCurJust=0;

    csPrompt=ResourceString(IDC_ICADATTDEFDIA_PROMPT_11, "Prompt" );
    csTag=ResourceString(IDC_ICADATTDEFDIA_NAME_12, "Name" );
    csValue=ResourceString(IDC_ICADATTDEFDIA_VALUE_13, "Value" );
}

//***This is the main window
void CMainWindow::IcadAttDefDia(void) {
    double rTmp,thickness;
    int ret;
    struct resbuf rb,rbent,rbucs,*rbNewAttDef;
	sds_point extru,ptemp;

	SDS_getvar(NULL,DB_QTHICKNESS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	thickness=rb.resval.rreal;
    SDS_getvar(NULL,DB_QUCSXDIR,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SDS_getvar(NULL,DB_QUCSYDIR,&rbent,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	extru[0]=rbucs.resval.rpoint[1]*rbent.resval.rpoint[2]-rbent.resval.rpoint[1]*rbucs.resval.rpoint[2];
	extru[1]=rbucs.resval.rpoint[2]*rbent.resval.rpoint[0]-rbent.resval.rpoint[2]*rbucs.resval.rpoint[0];
	extru[2]=rbucs.resval.rpoint[0]*rbent.resval.rpoint[1]-rbent.resval.rpoint[0]*rbucs.resval.rpoint[1];
	rbent.restype=RT3DPOINT;
	ic_ptcpy(rbent.resval.rpoint,extru);
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;
    att_CAttDef m_CAttDef;
    do{
        ret=m_CAttDef.DoModal();
        if(ret==IDOK || ret==ATT_CREATE) {
            rbNewAttDef=NULL;
            sds_entmake(NULL);
			sds_trans(m_CAttDef.dInsPt,&rbucs,&rbent,0,ptemp);
			//Modified Cybage AW 14-01-02 (dd-mm-yy) [
			//Reason :  If current text style is set with "Print text backward" and 
			//"Print text upsidedown", attributes are not drawn with these two proprties 
			//when created through the ddattdef dialog box
			struct resbuf *rbTextStyle;
			int nGen = 0;
			rbTextStyle=sds_tblsearch("STYLE"/*DNT*/,m_CAttDef.csStyle,0);
			if(ic_assoc(rbTextStyle,-1) == 0)
			{
				if(ic_assoc(rbTextStyle,71) == 0)
					nGen = ic_rbassoc->resval.rint;
			}
			sds_relrb(rbTextStyle);
			rbTextStyle = NULL;
			//Modified Cybage AW 14-01-02 (dd-mm-yy)]
            if((rbNewAttDef=sds_buildlist(RTDXF0,"ATTDEF"/*DNT*/,
                                              10,ptemp,
                                              40,m_CAttDef.dHeight,
                                               1,m_CAttDef.csValue,
                                               3,m_CAttDef.csPrompt,
                                               2,m_CAttDef.csTag,
                                              70,(m_CAttDef.iHidden*1)+(m_CAttDef.iConstant*2)+(m_CAttDef.iVerify*4)+(m_CAttDef.iPreset*8),
                                              50,m_CAttDef.dRotate,
                                               7,m_CAttDef.csStyle,
                                              72,m_CAttDef.iFlag72,
                                              74,m_CAttDef.iFlag74,
                                              11,ptemp,
											  39,thickness,
                                             210,extru,
											 //Reason :  If current text style is set with "Print text backward" and 
											 //"Print text upsidedown", attributes are not drawn with these two proprties 
											 //when created through the ddattdef dialog box
											 //Include the text generation flag in the Attribute buildlist.
											 71, nGen,
                                               0))==NULL) {
  	        MessageBox(ResourceString(IDC_ICADATTDEFDIA_MORE_MEMOR_3, "More memory is needed to do this operation" ),NULL,MB_OK|MB_ICONSTOP);
            }
            sds_entmake(rbNewAttDef);
            IC_RELRB(rbNewAttDef);
            rb.restype=RT3DPOINT;
            rb.resval.rpoint[0]=m_CAttDef.dInsPt[0];
            rb.resval.rpoint[1]=m_CAttDef.dInsPt[1];
            rb.resval.rpoint[2]=m_CAttDef.dInsPt[2];
			SDS_setvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
            rb.restype=RTREAL;
            rb.resval.rreal=m_CAttDef.dHeight;
            sds_setvar("TEXTSIZE"/*DNT*/,&rb);
            if(ret==ATT_CREATE) {
                continue;
            }else{
                break;
            }
        }else if(ret==ATT_PICK) {
            if(internalGetpoint(NULL,ResourceString(IDC_ICADATTDEFDIA__NSELECT__15, "\nSelect insertion point: " ),ptemp)!=RTNORM) continue;
            ic_ptcpy(m_CAttDef.dInsPt,ptemp);
        }else if(ret==ATT_HIGH_PICK) {
            if(sds_getdist(NULL,ResourceString(IDC_ICADATTDEFDIA__NSELECT__16, "\nSelect text height: " ),&rTmp)!=RTNORM) continue;
            m_CAttDef.dHeight=rTmp;
        }else if(ret==ATT_ROTATE_PICK) {
            if(sds_getangle(NULL,ResourceString(IDC_ICADATTDEFDIA__NSELECT__17, "\nSelect text angle: " ),&rTmp)!=RTNORM) continue;
            m_CAttDef.dRotate=rTmp;
        }else{
            break;
        }
    }while(1);
}




