/* C:\DEV\PRJ\ICAD\ICADUCSPDIA.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */


//** Includes
#include "Icad.h"/*DNT*/
#include "IcadDoc.h"
#include "cmds.h"/*DNT*/
#include "resource.hm"/*DNT*/
#include "paths.h"
#include "IcadDialog.h"
#include "IcadView.h"

static iRel=1;

class ucsp_CUcsp:public IcadDialog {
    public:
        bool bSetOrg;
        int iFollow;
        double dSqRoot2,dUcsXDir[3],dUcsYDir[3],dUcsXDirPrev[3],dUcsYDirPrev[3];
        ucsp_CUcsp();
        ~ucsp_CUcsp();
		HBITMAP m_hBitmap[6];
        BOOL OnInitDialog();
        CButton m_CButTop,
                m_CButBottom,
                m_CButLeft,
                m_CButRight,
                m_CButFront,
                m_CButBack;
    protected:
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
        afx_msg void OnHelp();
        afx_msg void OnWorldClicked();
        afx_msg void OnTopClicked();
        afx_msg void OnBottomClicked();
        afx_msg void OnLeftClicked();
        afx_msg void OnRightClicked();
        afx_msg void OnFrontClicked();
        afx_msg void OnBackClicked();
        afx_msg void OnPrevClicked();
        afx_msg void OnExpClicked();
        afx_msg void OnViewClicked();
        virtual void DoDataExchange(CDataExchange *pDX);

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(ucsp_CUcsp,CDialog)
    ON_BN_CLICKED(UCSP_TOP,OnTopClicked)
    ON_BN_CLICKED(UCSP_BOTTOM,OnBottomClicked)
    ON_BN_CLICKED(UCSP_LEFT,OnLeftClicked)
    ON_BN_CLICKED(UCSP_RIGHT,OnRightClicked)
    ON_BN_CLICKED(UCSP_FRONT,OnFrontClicked)
    ON_BN_CLICKED(UCSP_BACK,OnBackClicked)
    ON_BN_CLICKED(UCSP_PREV,OnPrevClicked)
    ON_BN_CLICKED(UCSP_WORLD,OnWorldClicked)
    ON_BN_CLICKED(UCSP_EXPLORER,OnExpClicked)
    ON_BN_CLICKED(UCSP_CURVIEW,OnViewClicked)
	ON_WM_HELPINFO()
    ON_BN_CLICKED(IDHELP,OnHelp)
END_MESSAGE_MAP()

BOOL ucsp_CUcsp::OnHelpInfo(HELPINFO *pHel){
	OnHelp();
	return(true);
}

void ucsp_CUcsp::OnHelp() {
	CommandRefHelp( m_hWnd, HLP_SETUCS);
}


void ucsp_CUcsp::OnExpClicked() {
    SDS_CMainWindow->IcadExplorer(3);
}

void ucsp_CUcsp::OnViewClicked() {
	struct resbuf rb,rbucs,rbwcs;
	struct gr_view *view=SDS_CURGRVW;
	sds_point p1,p2,p3;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;
	rbucs.rbnext=rbwcs.rbnext=NULL;

    UpdateData(TRUE);
	//this code generally copied from cmd_ucszaxis, but without all the
	//unnecessary error checking
	SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sds_trans(rb.resval.rpoint,&rbucs,&rbwcs,0,rb.resval.rpoint);
	if(icadRealEqual(rb.resval.rpoint[0],0.0) && icadRealEqual(rb.resval.rpoint[1],0.0)){
		//if view parallel to WCS Z axis...
		p1[0]=1.0;
		p1[1]=p1[2]=0.0;
		gr_ucs2rp(p1,p2,view);
		p1[0]=atan2(p2[1],p2[0]);  //angle to twist the UCS around Z
		//if the Z's go in opposite direction,
		//	we're still okay
		p3[0]=cos(-p1[0]);
		p3[1]=sin(-p1[0]);
		p3[2]=0.0;
        sds_trans(p3,&rbucs,&rbwcs,1,dUcsXDir);
		p1[0]=-p3[1];
		p1[1]=p3[0];
		p1[2]=0.0;
        sds_trans(p1,&rbucs,&rbwcs,1,dUcsYDir);
	}else{	//Z vector NOT parallel to WCS's Z
		//project vector into world XY plane and take its normal P1
		//project viewdir vector into world XY plane & go ccw
		//90degrees to get the new x axis
		p1[0]=-rb.resval.rpoint[1];
		p1[1]=rb.resval.rpoint[0];
		p1[2]=0.0;
		p3[0]=sqrt(p1[0]*p1[0]+p1[1]*p1[1]);
		p1[0]/=p3[0];
		p1[1]/=p3[0];
		ic_ptcpy(dUcsXDir,p1);
		p3[0]=sqrt(rb.resval.rpoint[0]*rb.resval.rpoint[0]+
				   rb.resval.rpoint[1]*rb.resval.rpoint[1]+
				   rb.resval.rpoint[2]*rb.resval.rpoint[2]);
		rb.resval.rpoint[0]/=p3[0];
		rb.resval.rpoint[1]/=p3[0];
		rb.resval.rpoint[2]/=p3[0];
		ic_crossproduct(rb.resval.rpoint,p1,dUcsYDir);
	}
    EndDialog(IDOK);
}

void ucsp_CUcsp::OnWorldClicked() {
    bSetOrg=true;
    dUcsXDir[0]=1.0;
    dUcsXDir[1]=0.0;
    dUcsXDir[2]=0.0;
    dUcsYDir[0]=0.0;
    dUcsYDir[1]=1.0;
    dUcsYDir[2]=0.0;
    EndDialog(IDOK);
}

void ucsp_CUcsp::OnTopClicked() {

    UpdateData(TRUE);
    if(iRel) { //Relative to WCS
        bSetOrg=true;
        dUcsXDir[0]=1.0;
        dUcsXDir[1]=0.0;
        dUcsXDir[2]=0.0;
        dUcsYDir[0]=0.0;
        dUcsYDir[1]=1.0;
        dUcsYDir[2]=0.0;
    }else{     //Relative to UCS
        dUcsXDir[0]=dUcsXDirPrev[0];
        dUcsXDir[1]=dUcsXDirPrev[1];
        dUcsXDir[2]=dUcsXDirPrev[2];
        dUcsYDir[0]=dUcsYDirPrev[0];
        dUcsYDir[1]=dUcsYDirPrev[1];
        dUcsYDir[2]=dUcsYDirPrev[2];
    }
    EndDialog(IDOK);
}

void ucsp_CUcsp::OnBottomClicked() {

    UpdateData(TRUE);
    if(iRel) { //Relative to WCS
        bSetOrg=true;
		dUcsXDir[0]=-1.0;
        dUcsXDir[1]=0.0;
        dUcsXDir[2]=0.0;
        dUcsYDir[0]=0.0;
		dUcsYDir[1]=1.0;
        dUcsYDir[2]=0.0;
    }else{     //Relative to UCS
        dUcsXDir[0]=dUcsXDirPrev[0];
        dUcsXDir[1]=dUcsXDirPrev[1];
        dUcsXDir[2]=dUcsXDirPrev[2];
        dUcsYDir[0]=dUcsYDirPrev[0];
        dUcsYDir[1]=-dUcsYDirPrev[1];
        dUcsYDir[2]=dUcsYDirPrev[2];
    }
    EndDialog(IDOK);
}

void ucsp_CUcsp::OnLeftClicked() {

    UpdateData(TRUE);
    if(iRel) { //Relative to WCS
        bSetOrg=true;
        dUcsXDir[0]=0.0;
        dUcsXDir[1]=-1.0;
        dUcsXDir[2]=0.0;
        dUcsYDir[0]=0.0;
        dUcsYDir[1]=0.0;
        dUcsYDir[2]=1.0;
    }else{     //Relative to UCS
        dUcsXDir[0]=-dUcsYDirPrev[0];
        dUcsXDir[1]=-dUcsYDirPrev[1];
        dUcsXDir[2]=-dUcsYDirPrev[2];
		ic_crossproduct(dUcsXDirPrev,dUcsYDirPrev,dUcsYDir);
    }
    EndDialog(IDOK);
}

void ucsp_CUcsp::OnRightClicked() {

    UpdateData(TRUE);
    if(iRel) { //Relative to WCS
        bSetOrg=true;
        dUcsXDir[0]=0.0;
        dUcsXDir[1]=1.0;
        dUcsXDir[2]=0.0;
        dUcsYDir[0]=0.0;
        dUcsYDir[1]=0.0;
        dUcsYDir[2]=1.0;
    }else{     //Relative to UCS
        dUcsXDir[0]=dUcsYDirPrev[0];
        dUcsXDir[1]=dUcsYDirPrev[1];
        dUcsXDir[2]=dUcsYDirPrev[2];
        ic_crossproduct(dUcsXDirPrev,dUcsYDirPrev,dUcsYDir);
    }
    EndDialog(IDOK);
}

void ucsp_CUcsp::OnFrontClicked() {

    UpdateData(TRUE);
    if(iRel) { //Relative to WCS
        bSetOrg=true;
        dUcsXDir[0]=1.0;
        dUcsXDir[1]=0.0;
        dUcsXDir[2]=0.0;
        dUcsYDir[0]=0.0;
        dUcsYDir[1]=0.0;
        dUcsYDir[2]=1.0;
    }else{     //Relative to UCS
        dUcsXDir[0]=dUcsXDirPrev[0];
        dUcsXDir[1]=dUcsXDirPrev[1];
        dUcsXDir[2]=dUcsXDirPrev[2];
		ic_crossproduct(dUcsXDirPrev,dUcsYDirPrev,dUcsYDir);
    }
    EndDialog(IDOK);
}

void ucsp_CUcsp::OnBackClicked() {

    UpdateData(TRUE);
    if(iRel) { //Relative to WCS
        bSetOrg=true;
        dUcsXDir[0]=-1.0;
        dUcsXDir[1]=0.0;
        dUcsXDir[2]=0.0;
        dUcsYDir[0]=0.0;
        dUcsYDir[1]=0.0;
        dUcsYDir[2]=1.0;
    }else{     //Relative to UCS
        dUcsXDir[0]=-dUcsXDirPrev[0];
        dUcsXDir[1]=-dUcsXDirPrev[1];
        dUcsXDir[2]=-dUcsXDirPrev[2];
		ic_crossproduct(dUcsXDirPrev,dUcsYDirPrev,dUcsYDir);
    }
    EndDialog(IDOK);
}

void ucsp_CUcsp::OnPrevClicked() {
    EndDialog(UCSP_PREV);
}

BOOL ucsp_CUcsp::OnInitDialog() {
    BOOL ret=IcadDialog::OnInitDialog();

	m_hBitmap[0] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(UCSP_TOP),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButTop.SetBitmap(m_hBitmap[0]);
	m_hBitmap[1] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(UCSP_BOTTOM),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButBottom.SetBitmap(m_hBitmap[1]);
	m_hBitmap[2] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(UCSP_LEFT),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButLeft.SetBitmap(m_hBitmap[2]);
	m_hBitmap[3] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(UCSP_RIGHT),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButRight.SetBitmap(m_hBitmap[3]);
	m_hBitmap[4] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(UCSP_FRONT),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButFront.SetBitmap(m_hBitmap[4]);
	m_hBitmap[5] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(UCSP_BACK),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButBack.SetBitmap(m_hBitmap[5]);

    return(ret);
}

void ucsp_CUcsp::DoDataExchange(CDataExchange *pDX) {
    DDX_Check(pDX,UCSP_PLAN,iFollow);
    DDX_Radio(pDX,ATT_REL,iRel);
    DDX_Control(pDX,UCSP_TOP,m_CButTop);
    DDX_Control(pDX,UCSP_BOTTOM,m_CButBottom);
    DDX_Control(pDX,UCSP_LEFT,m_CButLeft);
    DDX_Control(pDX,UCSP_RIGHT,m_CButRight);
    DDX_Control(pDX,UCSP_FRONT,m_CButFront);
    DDX_Control(pDX,UCSP_BACK,m_CButBack);
}

ucsp_CUcsp::ucsp_CUcsp():IcadDialog(UCSP_UCS) {
    struct resbuf rb;

	SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    dUcsXDirPrev[0]=rb.resval.rpoint[0];
    dUcsXDirPrev[1]=rb.resval.rpoint[1];
    dUcsXDirPrev[2]=rb.resval.rpoint[2];
	SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    dUcsYDirPrev[0]=rb.resval.rpoint[0];
    dUcsYDirPrev[1]=rb.resval.rpoint[1];
    dUcsYDirPrev[2]=rb.resval.rpoint[2];
	SDS_getvar(NULL,DB_QUCSFOLLOW,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    iFollow=rb.resval.rint;
    bSetOrg=false;
}

ucsp_CUcsp::~ucsp_CUcsp() {
	//*** Free up the bitmap images.
	int i;
	for(i=0; i<6; i++)
	{
		DeleteObject(m_hBitmap[i]);
	}
}

//***This is the main window
void CMainWindow::IcadUcspDia(void) {
    int ret;
    struct resbuf rb,rb2;
	sds_point vpoint;
	sds_real oldvtwist;
    struct SDS_ucs_llist *cmd_ucs_temp;

	CIcadView *pView=SDS_CURVIEW;
	SDS_getvar(NULL,DB_QVIEWTWIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	oldvtwist=rb.resval.rreal;

    ucsp_CUcsp m_CUcsp;
    ret=m_CUcsp.DoModal();
    if(ret==IDOK) {
   	    //we need to add a new UCS to the llist so the user can get back
	    //to it!
		SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
		rb.restype=RTSHORT;
		rb.resval.rint=m_CUcsp.iFollow;
		sds_setvar("UCSFOLLOW"/*DNT*/,&rb);	    
		if((cmd_ucs_temp= new struct SDS_ucs_llist )==NULL){
		    cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		    return;
	    }
		memset(cmd_ucs_temp,0,sizeof(struct SDS_ucs_llist));
	    if(SDS_CURDOC->m_pucs_llbeg==NULL){
		    SDS_CURDOC->m_pucs_llcur=SDS_CURDOC->m_pucs_llbeg=cmd_ucs_temp;
	    }else{
		    SDS_CURDOC->m_pucs_llcur->next=cmd_ucs_temp;
		    cmd_ucs_temp->prev=SDS_CURDOC->m_pucs_llcur;
		    SDS_CURDOC->m_pucs_llcur=cmd_ucs_temp;
	    }
		SDS_getvar(NULL,DB_QUCSORG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	    ic_ptcpy(SDS_CURDOC->m_pucs_llcur->org,rb.resval.rpoint);
		SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	    ic_ptcpy(SDS_CURDOC->m_pucs_llcur->xdir,rb.resval.rpoint);
		SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	    ic_ptcpy(SDS_CURDOC->m_pucs_llcur->ydir,rb.resval.rpoint);
	    //sds_getvar("VIEWDIR",&rb);
		//ic_ptcpy(vpoint,rb.resval.rpoint);
		//rb.restype=rb2.restype=RTSHORT;
		//rb.resval.rint=1;
		//rb2.resval.rint=0;
		//sds_trans(vpoint,&rb,&rb2,1,vpoint);//vpoint now in wcs

        //Now set the new ucs
        rb.resval.rpoint[0]=m_CUcsp.dUcsXDir[0];
        rb.resval.rpoint[1]=m_CUcsp.dUcsXDir[1];
        rb.resval.rpoint[2]=m_CUcsp.dUcsXDir[2];
        sdsengine_setvar("UCSXDIR"/*DNT*/,&rb);
        rb.resval.rpoint[0]=m_CUcsp.dUcsYDir[0];
        rb.resval.rpoint[1]=m_CUcsp.dUcsYDir[1];
        rb.resval.rpoint[2]=m_CUcsp.dUcsYDir[2];
        sdsengine_setvar("UCSYDIR"/*DNT*/,&rb);
        if(m_CUcsp.bSetOrg==true){
            rb.resval.rpoint[0]=0.0;
            rb.resval.rpoint[1]=0.0;
            rb.resval.rpoint[2]=0.0;
            sdsengine_setvar("UCSORG"/*DNT*/,&rb);
        }
        if(m_CUcsp.iFollow) {
			cmd_view_plan(1);
        }else{
    		gr_twist4ucs(SDS_CURDWG,&rb.resval.rreal);
			rb.restype=RTREAL;
			rb.resval.rreal+=oldvtwist;
			sdsengine_setvar("VIEWTWIST"/*DNT*/,&rb);
			rb.restype=rb2.restype=RTSHORT;
			rb.resval.rint=1;
			rb2.resval.rint=2;
			vpoint[0]=vpoint[1]=0.0;
			vpoint[2]=1.0;
			SDS_InitThisView(pView);
			sds_trans(vpoint,&rb2,&rb,1,vpoint);
			rb.restype=RT3DPOINT;
			ic_ptcpy(rb.resval.rpoint,vpoint);
			sdsengine_setvar("VIEWDIR"/*DNT*/,&rb);

			SDS_CURDOC->m_RegenListView=NULL;
			SDS_InitThisView(pView);
			sds_redraw(NULL,IC_REDRAW_DONTCARE);
			SDS_SetUndo(IC_UNDO_REGEN_ON_REDO,NULL,NULL,NULL,SDS_CURDWG);
		}
    }else if(ret==UCSP_PREV) {
        cmd_ucsprev();
        if(m_CUcsp.iFollow) {
            rb.restype=RT3DPOINT;
            rb.resval.rpoint[0]=0.0;
            rb.resval.rpoint[1]=0.0;
            rb.resval.rpoint[2]=1.0;
            sdsengine_setvar("VIEWDIR"/*DNT*/,&rb);
    		gr_twist4ucs(SDS_CURDWG,NULL);
            cmd_zoom_extents(0);
        }else{
            sds_redraw(NULL,IC_REDRAW_DONTCARE);
        }
    }
}


