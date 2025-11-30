/* E:\INTELLICAD\PRJ\ICAD\ICADINSERTDIA.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "cmds.h"/*DNT*/
#include "float.h"/*DNT*/
#include "resource.hm"/*DNT*/
#include "paths.h"
#include "IcadDialog.h"

//Globals
extern sds_name SDS_AttEditName;
void  cmd_OnOpenOK(HWND hDlg);


class ins_CInsert:public IcadDialog {
    public:
        int iExplode,iFile,iCols,iRows,iCurSel,iCoord;
        char szBlockName[IC_ACADBUF],szFileName[IC_ACADBUF],szCurDwg[IC_ACADBUF];
        double dInsPt[3],dScale[3],dColSpace,dRowSpace,dRotate;
        CString csTmp,csCurrentFile,csCurrentBlock;
        CComboBox m_CBlocks;
        ins_CInsert(char *name);
        BOOL OnInitDialog();
		//Modified Cybage SBD	04/03/2002 DD/MM/YYYY 
		//Reason : Fix for Bug No. 77983 from Bugzilla
        //void ins_SpinMasterReal(HWND hItemWnd,int increment);
		void ins_SpinMasterReal(HWND hItemWnd,int increment,int nUnitType=-1);
        void ins_SpinMasterInt(HWND hItemWnd,int increment);
        void ins_SpinMasterAngle(HWND hItemWnd,int increment);
        void ins_ReadInBlocks();
        void ins_ReadInFiles();

		bool redefineregen;
    protected:
        virtual void OnOK( );
        virtual void OnCancel( );
        virtual void OnHelp();
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
        afx_msg void ins_OnPick();
        afx_msg void ins_OnFileBlk();
        afx_msg void ins_OnBrowse();
        afx_msg void ins_OnCoord();
        afx_msg void OnSpinInsX(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinInsY(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinInsZ(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinSfX(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinSfY(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinSfZ(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinColSpace(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinRowSpace(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinRotate(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinCols(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinRows(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void DoDataExchange(CDataExchange *pDX);

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(ins_CInsert,CDialog)
    ON_BN_CLICKED(INS_PICK,ins_OnPick)
    ON_BN_CLICKED(INS_FILE_OR_BLK,ins_OnFileBlk)
    ON_BN_CLICKED(INS_FILE_OR_BLK1,ins_OnFileBlk)
    ON_BN_CLICKED(INS_BROWSE,ins_OnBrowse)
    ON_BN_CLICKED(INS_COORD,ins_OnCoord)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_INS_IPT_X,OnSpinInsX)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_INS_IPT_Y,OnSpinInsY)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_INS_IPT_Z,OnSpinInsZ)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_INS_SF_X,OnSpinSfX)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_INS_SF_Y,OnSpinSfY)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_INS_SF_Z,OnSpinSfZ)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_INS_COLSPACE,OnSpinColSpace)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_INS_ROWSPACE,OnSpinRowSpace)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_INS_ROTATION,OnSpinRotate)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_INS_COLUMNS,OnSpinCols)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_INS_ROWS,OnSpinRows)
	ON_WM_HELPINFO()
    ON_BN_CLICKED(IDHELP,OnHelp)
END_MESSAGE_MAP()

BOOL ins_CInsert::OnHelpInfo(HELPINFO *pHel){
	OnHelp();
	return(true);
}

void ins_CInsert::OnHelp() {
	CommandRefHelp( m_hWnd, HLP_DDINSERT);
}

void ins_CInsert::ins_OnBrowse() {
    struct resbuf rb;
    char dwgname[IC_FILESIZE+IC_PATHSIZE];//,szTest[IC_FILESIZE+IC_PATHSIZE],*fcp1;

    ::GetWindowText(::GetDlgItem(m_hWnd,INS_FROM_FILE),dwgname,sizeof(dwgname)-1);
    do{
		// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
		if(RTNORM!=SDS_GetFiled(ResourceString(IDC_ICADINSERTDIA_INSERT_BLO_1, "Insert Block" ),dwgname,ResourceString(IDC_ICADINSERTDIA_STANDARD_D_2, "Standard Drawing File|dwg" ),512+2,&rb,MAKEINTRESOURCE(IDD_OPENSAVE_PREVIEW),cmd_OnOpenOK)) {
            return;
        }
		
        strcpy(dwgname,rb.resval.rstring);
        if(rb.resval.rstring) IC_FREE(rb.resval.rstring);
        //*** Check path
		ChangeSlashToBackslashAndRemovePipe(dwgname);
		// DP: remarked because this is checked later
		/*
        //remove path
        strcpy(szTest,dwgname);
        if((fcp1=strrchr(szTest,'\\'))!=NULL) {
            *fcp1=0;
            strncpy(szTest,fcp1+1,sizeof(szTest)-1);
        }
        //remove the extension
        if((fcp1=strrchr(szTest,'.'))!=NULL) {
            *fcp1=0;
        }
		
        if(_tcsclen(szTest) >= IC_ACADNMLEN){	//D.G.
            cmd_ErrorPrompt(CMD_ERR_BLKNAMETOOLONG,1);
            continue;
        }*/
        if(strisame(szCurDwg,dwgname)){
            cmd_ErrorPrompt(CMD_ERR_DUPFILE,1);
            continue;
        }
        break;
    }while(1);
    csCurrentFile=dwgname;
    UpdateData(false);
}

void ins_CInsert::OnOK() {
    char dwgname[IC_ACADBUF],*fcp1;

    if(!UpdateData(true)) return;
    if(iFile){
        if(csCurrentBlock.IsEmpty()){
            cmd_ErrorPrompt(CMD_ERR_FINDBLOCK,1);
            return;
        }
        strncpy(szBlockName,csCurrentBlock,csCurrentBlock.GetLength()+1);
    }else{
        if(csCurrentFile.GetLength()==0){
            cmd_ErrorPrompt(CMD_ERR_FINDBLOCK,1);
            return;
        }
        if(RTERROR==sds_findfile(csCurrentFile,dwgname)) {
			if (csCurrentFile.Find(".") == -1)
				{
				csCurrentFile += ".dwg";
				}
			if(RTERROR==sds_findfile(csCurrentFile,dwgname)) 
				{
				cmd_ErrorPrompt(CMD_ERR_FINDBLOCK,1);
				return;
				}
        }
		ChangeSlashToBackslashAndRemovePipe(dwgname);
        if((fcp1=strrchr(dwgname,'\\'))!=NULL) {
            *fcp1=0;
            strncpy(dwgname,fcp1+1,sizeof(dwgname)-1);
        }
        //remove the extention
        if((fcp1=strrchr(dwgname,'.'))!=NULL) {
            *fcp1=0;
        }
        //can not insert dwg into itself
        if(strisame(szCurDwg,dwgname)){
            cmd_ErrorPrompt(CMD_ERR_DUPFILE,1);
            return;
        }
		struct resbuf *tmprb=NULL, setgetrb;
		if((tmprb=sds_tblsearch("BLOCK"/*DNT*/,dwgname,0))!=NULL) 
			{
			IC_RELRB(tmprb);

			int expert = 0;	
			if (SDS_getvar(NULL,DB_QEXPERT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
				expert=setgetrb.resval.rint;
			if (expert < 2)
				{
				char warning[IC_ACADBUF];
				char caption[IC_ACADBUF];
				_snprintf(warning,sizeof(warning),	ResourceString(IDC_BLOCK__NTHE_BLOCK__S_ARE_1, "\nThe block %s already exists. Do you want to redefine it? <N> " ),dwgname);
				_snprintf(caption,sizeof(caption),	ResourceString(IDC_INSERT_BLOCK_ALREADY_EXI_8,"Block already exists") );
				if (this->MessageBox(warning, caption, MB_YESNO) == IDNO)
					{
					return;
					}
				}
			redefineregen = TRUE;
			}
			
        strcpy(dwgname,csCurrentFile);
		//fix the name so that command line will accept it.
        for(fcp1=dwgname;*fcp1!=0;fcp1++){
			if(*fcp1=='\\') *fcp1='/';
		    if(*fcp1==' ')  *fcp1='|';
			// EBATECH(R.Arayashiki)
			if ((_MBC_LEAD ==_mbbtype((unsigned char)*fcp1,0)) &&
				  (_MBC_TRAIL==_mbbtype((unsigned char)*(fcp1+1),1)))
				fcp1 ++;
			//]-EBATECH
        }
        strcpy(szFileName,dwgname);
    }
    CDialog::OnOK();
}

void ins_CInsert::OnCancel() {
    CDialog::OnCancel();
}

void ins_CInsert::ins_OnCoord() {

    if(!UpdateData(true)) return;
    if(iCoord==0) {
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_IPT_X),true);
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_IPT_Y),true);
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_IPT_Z),true);
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_SF_X),true);
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_SF_Y),true);
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_SF_Z),true);
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_ROTATION),true);
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_PICK),true);
        ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_INS_IPT_X),true);
        ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_INS_IPT_Y),true);
        ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_INS_IPT_Z),true);
        ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_INS_SF_X),true);
        ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_INS_SF_Y),true);
        ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_INS_SF_Z),true);
        ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_INS_ROTATION),true);
    }else{
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_IPT_X),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_IPT_Y),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_IPT_Z),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_SF_X),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_SF_Y),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_SF_Z),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_ROTATION),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_PICK),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_INS_IPT_X),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_INS_IPT_Y),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_INS_IPT_Z),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_INS_SF_X),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_INS_SF_Y),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_INS_SF_Z),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,UPDN_INS_ROTATION),false);
    }
}

void ins_CInsert::ins_OnFileBlk() {
    if(!UpdateData(true)) return;
    if(iFile==0) {
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_FROM_FILE),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_BROWSE),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_BLOCK),FALSE);
		::SetFocus(::GetDlgItem(m_hWnd,INS_FROM_FILE));
    }else{
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_FROM_FILE),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_BROWSE),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,INS_BLOCK),TRUE);
    }
}

void ins_CInsert::OnSpinInsX(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ins_SpinMasterReal(::GetDlgItem(m_hWnd,INS_IPT_X),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ins_CInsert::OnSpinInsY(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ins_SpinMasterReal(::GetDlgItem(m_hWnd,INS_IPT_Y),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ins_CInsert::OnSpinInsZ(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ins_SpinMasterReal(::GetDlgItem(m_hWnd,INS_IPT_Z),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ins_CInsert::OnSpinSfX(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    //Modified Cybage SBD	04/03/2002 DD/MM/YYYY 
	//Reason : Fix for Bug No. 77983 from Bugzilla
	//ins_SpinMasterReal(::GetDlgItem(m_hWnd,INS_SF_X),pNMUpDown->iDelta);
	ins_SpinMasterReal(::GetDlgItem(m_hWnd,INS_SF_X),pNMUpDown->iDelta,2);
    UpdateData(TRUE);
    *result=0;
}

void ins_CInsert::OnSpinSfY(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
	//Modified Cybage SBD	04/03/2002 DD/MM/YYYY 
	//Reason : Fix for Bug No. 77983 from Bugzilla
    //ins_SpinMasterReal(::GetDlgItem(m_hWnd,INS_SF_Y),pNMUpDown->iDelta);
	ins_SpinMasterReal(::GetDlgItem(m_hWnd,INS_SF_Y),pNMUpDown->iDelta,2);
    UpdateData(TRUE);
    *result=0;
}

void ins_CInsert::OnSpinSfZ(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
	//Modified Cybage SBD	04/03/2002 DD/MM/YYYY 
	//Reason : Fix for Bug No. 77983 from Bugzilla
    //ins_SpinMasterReal(::GetDlgItem(m_hWnd,INS_SF_Z),pNMUpDown->iDelta);
	ins_SpinMasterReal(::GetDlgItem(m_hWnd,INS_SF_Z),pNMUpDown->iDelta,2);
    UpdateData(TRUE);
    *result=0;
}

void ins_CInsert::OnSpinColSpace(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ins_SpinMasterReal(::GetDlgItem(m_hWnd,INS_COLSPACE),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ins_CInsert::OnSpinRowSpace(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ins_SpinMasterReal(::GetDlgItem(m_hWnd,INS_ROWSPACE),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ins_CInsert::OnSpinRotate(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ins_SpinMasterAngle(::GetDlgItem(m_hWnd,INS_ROTATION),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ins_CInsert::OnSpinCols(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ins_SpinMasterInt(::GetDlgItem(m_hWnd,INS_COLUMNS),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ins_CInsert::OnSpinRows(NMHDR *pNotifyStruct,LRESULT *result) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    ins_SpinMasterInt(::GetDlgItem(m_hWnd,INS_ROWS),pNMUpDown->iDelta);
    UpdateData(TRUE);
    *result=0;
}

void ins_CInsert::ins_SpinMasterAngle(HWND hWndItem,int increment) {
    double dTmp;
    char szTmp[IC_ACADBUF];

    ::GetWindowText(hWndItem,szTmp,IC_ACADBUF-1);
    sds_angtof(szTmp,-1,&dTmp);
    dTmp+=(IC_PI/180)*(-increment);
    sds_angtos(dTmp,-1,-1,szTmp);
    ::SetWindowText(hWndItem,szTmp);
}

//Modified Cybage SBD	04/03/2002 DD/MM/YYYY 
//Reason : Fix for Bug No. 77983 from Bugzilla
//void ins_CInsert::ins_SpinMasterReal(HWND hWndItem,int increment) {
void ins_CInsert::ins_SpinMasterReal(HWND hWndItem,int increment,int nUnitType /* = -1 */) {
    double dTmp;
    char szTmp[IC_ACADBUF];

    ::GetWindowText(hWndItem,szTmp,IC_ACADBUF-1);
    sds_distof(szTmp,-1,&dTmp);
    dTmp+=(-increment);
	//Modified Cybage SBD	04/03/2002 DD/MM/YYYY 
	//Reason : Fix for Bug No. 77983 from Bugzilla
	//sds_rtos(dTmp,-1,-1,szTmp);
	sds_rtos(dTmp,nUnitType,-1,szTmp);
    ::SetWindowText(hWndItem,szTmp);
}

void ins_CInsert::ins_SpinMasterInt(HWND hWndItem,int increment) {
    int iTmp;
    char szTmp[IC_ACADBUF];

    ::GetWindowText(hWndItem,szTmp,IC_ACADBUF-1);
    iTmp=atoi(szTmp);
    iTmp+=(-increment);
    if(iTmp<1) iTmp=1;
    itoa(iTmp,szTmp,10);
    ::SetWindowText(hWndItem,szTmp);
}

void ins_CInsert::ins_OnPick() {
    struct resbuf rb;

    if(!UpdateData(true)) return;
    strncpy(szBlockName,csCurrentBlock,csCurrentBlock.GetLength()+1);
    rb.restype=RTSTR;
    rb.resval.rstring=szBlockName;
    sds_setvar("INSNAME"/*DNT*/,&rb);
    EndDialog(INS_PICK);
}

BOOL ins_CInsert::OnInitDialog() {
    IcadDialog::OnInitDialog();
	redefineregen = FALSE;
    if (szFileName[0] != '\0')
		{
		ins_OnFileBlk();
		}
	else
		{
		ins_ReadInBlocks();
		ins_OnFileBlk();
		}
    ins_OnCoord();
    return(true);
}

void ins_CInsert::ins_ReadInBlocks() {
    struct resbuf rb;
    char *pszTmp=NULL;

    ///This will read in the blocks
	SDS_StartTblRead("BLOCK"/*DNT*/);
	while(SDS_TblReadHasMore()) {
        pszTmp=SDS_TblReadString();
        if(*pszTmp!='*' && strchr(pszTmp,'|')==NULL){
            m_CBlocks.AddString(pszTmp);
        }
    }
    sds_getvar("INSNAME"/*DNT*/,&rb);
    if(*rb.resval.rstring) {
        m_CBlocks.SelectString(-1,rb.resval.rstring);
    }else{
        m_CBlocks.SetCurSel(0);
    }
    if(rb.resval.rstring) IC_FREE(rb.resval.rstring);
}

//***This is the DDX function
void ins_CInsert::DoDataExchange(CDataExchange *pDX) {
    char szTmp[IC_ACADBUF];

    if(!pDX->m_bSaveAndValidate){  //Moving data to the dialog
        sds_rtos(dInsPt[0],-1,-1,szTmp);
        csTmp=szTmp;
        DDX_Text(pDX,INS_IPT_X,csTmp);
        sds_rtos(dInsPt[1],-1,-1,szTmp);
        csTmp=szTmp;
        DDX_Text(pDX,INS_IPT_Y,csTmp);
        sds_rtos(dInsPt[2],-1,-1,szTmp);
        csTmp=szTmp;
        DDX_Text(pDX,INS_IPT_Z,csTmp);
		//Modified Cybage SBD	04/03/2002 DD/MM/YYYY 
		//Reason : Fix for Bug No. 77983 from Bugzilla
        //sds_rtos(dScale[0],-1,-1,szTmp);
		sds_rtos(dScale[0],2,-1,szTmp);
        csTmp=szTmp;
        DDX_Text(pDX,INS_SF_X,csTmp);
		//Modified Cybage SBD	04/03/2002 DD/MM/YYYY 
		//Reason : Fix for Bug No. 77983 from Bugzilla
        //sds_rtos(dScale[1],-1,-1,szTmp);
		sds_rtos(dScale[1],2,-1,szTmp);
        csTmp=szTmp;
        DDX_Text(pDX,INS_SF_Y,csTmp);
		//Modified Cybage SBD	04/03/2002 DD/MM/YYYY 
		//Reason : Fix for Bug No. 77983 from Bugzilla
        //sds_rtos(dScale[2],-1,-1,szTmp);
		sds_rtos(dScale[2],2,-1,szTmp);
        csTmp=szTmp;
        DDX_Text(pDX,INS_SF_Z,csTmp);
        sds_rtos(dColSpace,-1,-1,szTmp);
        csTmp=szTmp;
        DDX_Text(pDX,INS_COLSPACE,csTmp);
        sds_rtos(dRowSpace,-1,-1,szTmp);
        csTmp=szTmp;
        DDX_Text(pDX,INS_ROWSPACE,csTmp);
        sds_angtos(dRotate,-1,-1,szTmp);
        csTmp=szTmp;
        DDX_Text(pDX,INS_ROTATION,csTmp);
    }else if(pDX->m_bSaveAndValidate){ //Retrieving data from the dialog
        dInsPt[0]=dInsPt[0]=dInsPt[0]=dScale[1]=dScale[1]=dScale[1]=dColSpace=dRowSpace=dRotate=0.0;
        DDX_Text(pDX,INS_IPT_X,csTmp);
        sds_distof(csTmp,-1,&dInsPt[0]);
        DDX_Text(pDX,INS_IPT_Y,csTmp);
        sds_distof(csTmp,-1,&dInsPt[1]);
        DDX_Text(pDX,INS_IPT_Z,csTmp);
        sds_distof(csTmp,-1,&dInsPt[2]);
        DDX_Text(pDX,INS_SF_X,csTmp);
        sds_distof(csTmp,-1,&dScale[0]);
        DDX_Text(pDX,INS_SF_Y,csTmp);
        sds_distof(csTmp,-1,&dScale[1]);
        DDX_Text(pDX,INS_SF_Z,csTmp);
        sds_distof(csTmp,-1,&dScale[2]);
        DDX_Text(pDX,INS_COLSPACE,csTmp);
        sds_distof(csTmp,-1,&dColSpace);
        DDX_Text(pDX,INS_ROWSPACE,csTmp);
        sds_distof(csTmp,-1,&dRowSpace);
        DDX_Text(pDX,INS_ROTATION,csTmp);
        sds_angtof(csTmp,-1,&dRotate);
    }
    DDX_Text(pDX,INS_COLUMNS,iCols);
    DDV_MinMaxInt(pDX,iCols,1,SHRT_MAX);	// Ebatech(CNBR) Bugzilla#78539 SHRT_MIN to 1
    DDX_Text(pDX,INS_ROWS,iRows);
    DDV_MinMaxInt(pDX,iRows,1,SHRT_MAX);
    DDX_Text(pDX,INS_BLOCK,csCurrentBlock);
    DDX_Text(pDX,INS_FROM_FILE,csCurrentFile);
    DDX_Check(pDX,INS_COORD,iCoord);
    DDX_Check(pDX,INS_EXPLODE,iExplode);
    DDX_Control(pDX,INS_BLOCK,m_CBlocks);
    DDX_Radio(pDX,INS_FILE_OR_BLK,iFile);
}

ins_CInsert::ins_CInsert(char *name):IcadDialog(INS_INSERT) {
    struct resbuf rb;
    char *fcp1=NULL;

    iCurSel=0;
    iFile=1;
    iCols=iRows=1;
    iExplode=0;
    iCoord=1;
    dRotate=0.0;
    dColSpace=dRowSpace=1.0;
	SDS_getvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
        dInsPt[0]=rb.resval.rpoint[0];
        dInsPt[1]=rb.resval.rpoint[1];
        dInsPt[2]=rb.resval.rpoint[2];
    dScale[0]=dScale[1]=dScale[2]=1.0;
	SDS_getvar(NULL,DB_QDWGNAME,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    strncpy(szCurDwg,rb.resval.rstring,strlen(rb.resval.rstring)+1);
    IC_FREE(rb.resval.rstring);
	ChangeSlashToBackslashAndRemovePipe(szCurDwg);
    if((fcp1=strrchr(szCurDwg,'\\'))!=NULL) {
        *fcp1=0;
        strncpy(szCurDwg,fcp1+1,sizeof(szCurDwg)-1);
    }
	if (name)
	{
		iFile = 0;
		strcpy (szFileName, name);
	}
	else
		szFileName[0] = '\0';
}

//***This is the main window
void CMainWindow::IcadInsertDia(char *name) 
	{
    int ret;
    sds_point ptemp;
	char *nameptr;
	bool fromdisk,explodeflag,printit,givetblsloadedmsg;
	sds_point extru;
	sds_real rotang,rotbase;
	db_insert *ins = NULL;
	char blkname[IC_ACADBUF],blkfile[IC_ACADBUF];
	bool minsert=false,ask4onescl=false;
	sds_real curelev; // need to look at
	sds_point retinspt;
	bool multiIns=false;
	struct resbuf rb;
	bool dragmode;

    ins_CInsert m_CInsert(name);
    for(;;)
		{
        ret=m_CInsert.DoModal();
        if(ret==INS_PICK) 
			{
            if(internalGetpoint(NULL,ResourceString(IDC_ICADINSERTDIA__NSELECT_I_4, "\nSelect insertion point: " ),ptemp)!=RTNORM) continue;
            ic_ptcpy(m_CInsert.dInsPt,ptemp);
            sds_printf("\n"/*DNT*/);
			}
		else if(ret==IDOK) 
			{
			if (m_CInsert.iFile)
				nameptr=m_CInsert.szBlockName;
			else
				nameptr=m_CInsert.szFileName;
			
			fromdisk=false;
			if (ProcessInsertBlockName(nameptr,blkname,blkfile)!=RTNORM)
				{
				cmd_ErrorPrompt(CMD_ERR_BLOCKNAME,0);
				return;
				}

			if (blkfile[0])
				{
				if (FindTheBlockFile(blkfile,blkfile)!=RTNORM)
					{
					cmd_ErrorPrompt(CMD_ERR_CANTFINDFILE,0,blkfile);
					return;
					}
				if (LoadTheBlock(blkname,blkfile)!=RTNORM)
					{
					sds_printf("Error loading block file %s.\n",blkfile);
					return;
					}
				fromdisk=true;
				}

			if (m_CInsert.iExplode)
				{
				explodeflag=true;
				ask4onescl=true;
				}
			else
				explodeflag=false;

			printit=PrintMode();
			givetblsloadedmsg = (printit && fromdisk);

			if (m_CInsert.iCoord)
				{
				if (GetExtrusionAndRotation(extru,&rotang,&rotbase)!=RTNORM)
					{
					cmd_ErrorPrompt(CMD_ERR_UNABLE,0);
					return;
					}
				
				if (MakeDragParlink(blkname,extru,rotang,&ins)!=RTNORM)  // creates ins
					{
					cmd_ErrorPrompt(CMD_ERR_UNABLE,0);
					return;
					}
				
				ins->set_210(extru);
				
				SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if (rb.resval.rint)
					dragmode=true;
				else
					dragmode=false;

				if (GetInsertParameters(ins,minsert,ask4onescl,dragmode,givetblsloadedmsg,rotbase,&curelev,rotang,retinspt,&multiIns)!=RTNORM)
					{
// error msg?
					return;
					}
				//Added Cybage AJK 25-02-2002 [
				//Reason: Fix for bug no 77938 from Bugzilla
				ins->set_44(m_CInsert.dRowSpace);
				ins->set_45(m_CInsert.dColSpace);
				ins->set_70(m_CInsert.iCols);
				ins->set_71(m_CInsert.iRows);
				//Added Cybage AJK 25-02-2002 DD/MM/YYYY ]
				}
			else
				{
				//Modified Cybage SBD 14/01/2002 DD/MM/YYYY [
				//Reason : Fix for Bug No. 77953 from Bugzilla 
				struct resbuf rbent,rbucs;
			     
				rotang=m_CInsert.dRotate;
				if (GetExtrusionAndRotation(extru,&rotang,&rotbase)!=RTNORM)
				{
					cmd_ErrorPrompt(CMD_ERR_UNABLE,0);
					return;
				}
				rbent.restype=RT3DPOINT;
				ic_ptcpy(rbent.resval.rpoint,extru);
				rbucs.restype=RTSHORT;
				rbucs.resval.rint=1;
				sds_trans(m_CInsert.dInsPt,&rbucs,&rbent,0,m_CInsert.dInsPt);
				//Modified Cybage SBD 14/01/2002 DD/MM/YYYY ]

				ins=new db_insert();
				ins->set_10(m_CInsert.dInsPt);
				ins->set_41(m_CInsert.dScale[0]);
				ins->set_42(m_CInsert.dScale[1]);
				ins->set_43(m_CInsert.dScale[2]);
				ins->set_44(m_CInsert.dRowSpace);
				ins->set_45(m_CInsert.dColSpace);
				//Modified Cybage SBD 14/01/2002 DD/MM/YYYY 
				//Reason : Fix for Bug No. 77953 from Bugzilla 
				//ins->set_50(m_CInsert.dRotate);
				ins->set_50(rotang);
				ins->set_70(m_CInsert.iCols);
				ins->set_71(m_CInsert.iRows);

				ic_ptcpy(retinspt, m_CInsert.dInsPt);

				}
            break;
			}
		else
			return;
	    }
		if (DoTheInsert(blkname,explodeflag,ins,fromdisk,retinspt,curelev,multiIns, m_CInsert.redefineregen)!=RTNORM)
			cmd_ErrorPrompt(CMD_ERR_UNABLE,0);
		// DP: AFAIK this is temporary object and must be deleted
		if(ins)
			delete ins;
	}



