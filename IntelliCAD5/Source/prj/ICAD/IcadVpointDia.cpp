/* D:\DEV\PRJ\ICAD\ICADVPOINTDIA.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

//*********************************************************************
//*  IcadVpoint.cpp                                                   *
//*  Copyright (C) 1994,1995, 1996, 1997 by Visio, Inc.               *
//*                                                                   *
//*********************************************************************

//** Includes
#include "Icad.h"/*DNT*/
#include "resource.hm"/*DNT*/
#include "paths.h"
#include "IcadDialog.h"
#include "IcadView.h"

class vp_CVpoint:public IcadDialog {
    public:
        double dViewDir[3];
        vp_CVpoint();
        ~vp_CVpoint();
        HBITMAP m_hBitmap[14];
        BOOL OnInitDialog();
        CButton m_CButOct1,
                m_CButOct2,
                m_CButOct3,
                m_CButOct4,
                m_CButOct5,
                m_CButOct6,
                m_CButOct7,
                m_CButOct8,
                m_CButTop,
                m_CButBottom,
                m_CButLeft,
                m_CButRight,
                m_CButFront,
                m_CButBack;
    protected:
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
        afx_msg void OnHelp();
        afx_msg void OnOct1Clicked();
        afx_msg void OnOct2Clicked();
        afx_msg void OnOct3Clicked();
        afx_msg void OnOct4Clicked();
        afx_msg void OnOct5Clicked();
        afx_msg void OnOct6Clicked();
        afx_msg void OnOct7Clicked();
        afx_msg void OnOct8Clicked();
        afx_msg void OnTopClicked();
        afx_msg void OnBottomClicked();
        afx_msg void OnLeftClicked();
        afx_msg void OnRightClicked();
        afx_msg void OnFrontClicked();
        afx_msg void OnBackClicked();
        afx_msg void OnPrevClicked();
        virtual void DoDataExchange(CDataExchange *pDX);

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(vp_CVpoint,IcadDialog)
    ON_BN_CLICKED(VP_OCT1,OnOct1Clicked)
    ON_BN_CLICKED(VP_OCT2,OnOct2Clicked)
    ON_BN_CLICKED(VP_OCT3,OnOct3Clicked)
    ON_BN_CLICKED(VP_OCT4,OnOct4Clicked)
    ON_BN_CLICKED(VP_OCT5,OnOct5Clicked)
    ON_BN_CLICKED(VP_OCT6,OnOct6Clicked)
    ON_BN_CLICKED(VP_OCT7,OnOct7Clicked)
    ON_BN_CLICKED(VP_OCT8,OnOct8Clicked)
    ON_BN_CLICKED(VP_TOP,OnTopClicked)
    ON_BN_CLICKED(VP_BOTTOM,OnBottomClicked)
    ON_BN_CLICKED(VP_LEFT,OnLeftClicked)
    ON_BN_CLICKED(VP_RIGHT,OnRightClicked)
    ON_BN_CLICKED(VP_FRONT,OnFrontClicked)
    ON_BN_CLICKED(VP_BACK,OnBackClicked)
    ON_BN_CLICKED(VP_PREV,OnPrevClicked)
    ON_BN_CLICKED(IDHELP,OnHelp)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

vp_CVpoint::~vp_CVpoint() {
	//*** Free up the bitmap images.
	int i;
	for(i=0; i<14; i++)
	{
		DeleteObject(m_hBitmap[i]);
	}
}

BOOL vp_CVpoint::OnHelpInfo(HELPINFO *pHel){
	OnHelp();
	return(true);
}

void vp_CVpoint::OnHelp() {
	CommandRefHelp( m_hWnd, HLP_SETVPOINT);
}

void vp_CVpoint::OnOct1Clicked() {

    dViewDir[0]=1.0;
    dViewDir[1]=1.0;
    dViewDir[2]=1.0;
    EndDialog(IDOK);
}

void vp_CVpoint::OnOct2Clicked() {

    dViewDir[0]=-1.0;
    dViewDir[1]=1.0;
    dViewDir[2]=1.0;
    EndDialog(IDOK);
}

void vp_CVpoint::OnOct3Clicked() {

    dViewDir[0]=-1.0;
    dViewDir[1]=-1.0;
    dViewDir[2]=1.0;
    EndDialog(IDOK);
}

void vp_CVpoint::OnOct4Clicked() {

    dViewDir[0]=1.0;
    dViewDir[1]=-1.0;
    dViewDir[2]=1.0;
    EndDialog(IDOK);
}

void vp_CVpoint::OnOct5Clicked() {

    dViewDir[0]=1.0;
    dViewDir[1]=1.0;
    dViewDir[2]=-1.0;
    EndDialog(IDOK);
}

void vp_CVpoint::OnOct6Clicked() {

    dViewDir[0]=-1.0;
    dViewDir[1]=1.0;
    dViewDir[2]=-1.0;
    EndDialog(IDOK);
}

void vp_CVpoint::OnOct7Clicked() {

    dViewDir[0]=-1.0;
    dViewDir[1]=-1.0;
    dViewDir[2]=-1.0;
    EndDialog(IDOK);
}

void vp_CVpoint::OnOct8Clicked() {

    dViewDir[0]=1.0;
    dViewDir[1]=-1.0;
    dViewDir[2]=-1.0;
    EndDialog(IDOK);
}

void vp_CVpoint::OnTopClicked() {

    dViewDir[0]=0.0;
    dViewDir[1]=0.0;
    dViewDir[2]=1.0;
    EndDialog(IDOK);
}

void vp_CVpoint::OnBottomClicked() {

    dViewDir[0]=0.0;
    dViewDir[1]=0.0;
    dViewDir[2]=-1.0;
    EndDialog(IDOK);
}

void vp_CVpoint::OnLeftClicked() {

    dViewDir[0]=-1.0;
    dViewDir[1]=0.0;
    dViewDir[2]=0.0;
    EndDialog(IDOK);
}

void vp_CVpoint::OnRightClicked() {

    dViewDir[0]=1.0;
    dViewDir[1]=0.0;
    dViewDir[2]=0.0;
    EndDialog(IDOK);
}

void vp_CVpoint::OnFrontClicked() {

    dViewDir[0]=0.0;
    dViewDir[1]=-1.0;
    dViewDir[2]=0.0;
    EndDialog(IDOK);
}

void vp_CVpoint::OnBackClicked() {

    dViewDir[0]=0.0;
    dViewDir[1]=1.0;
    dViewDir[2]=0.0;
    EndDialog(IDOK);
}

void vp_CVpoint::OnPrevClicked() {
    EndDialog(VP_PREV);
}


vp_CVpoint::OnInitDialog() {
    BOOL ret=IcadDialog::OnInitDialog();

	m_hBitmap[0] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VP_OCT1),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButOct1.SetBitmap(m_hBitmap[0]);
	m_hBitmap[1] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VP_OCT2),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButOct2.SetBitmap(m_hBitmap[1]);
	m_hBitmap[2] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VP_OCT3),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButOct3.SetBitmap(m_hBitmap[2]);
	m_hBitmap[3] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VP_OCT4),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButOct4.SetBitmap(m_hBitmap[3]);
	m_hBitmap[4] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VP_OCT5),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButOct5.SetBitmap(m_hBitmap[4]);
	m_hBitmap[5] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VP_OCT6),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButOct6.SetBitmap(m_hBitmap[5]);
	m_hBitmap[6] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VP_OCT7),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButOct7.SetBitmap(m_hBitmap[6]);
	m_hBitmap[7] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VP_OCT8),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButOct8.SetBitmap(m_hBitmap[7]);
	m_hBitmap[8] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VP_TOP),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButTop.SetBitmap(m_hBitmap[8]);
	m_hBitmap[9] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VP_BOTTOM),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButBottom.SetBitmap(m_hBitmap[9]);
	m_hBitmap[10] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VP_LEFT),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButLeft.SetBitmap(m_hBitmap[10]);
	m_hBitmap[11] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VP_RIGHT),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButRight.SetBitmap(m_hBitmap[11]);
	m_hBitmap[12] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VP_FRONT),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButFront.SetBitmap(m_hBitmap[12]);
	m_hBitmap[13] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VP_BACK),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	m_CButBack.SetBitmap(m_hBitmap[13]);

    return(ret);
}

void vp_CVpoint::DoDataExchange(CDataExchange *pDX) {
    DDX_Control(pDX,VP_OCT1,m_CButOct1);
    DDX_Control(pDX,VP_OCT2,m_CButOct2);
    DDX_Control(pDX,VP_OCT3,m_CButOct3);
    DDX_Control(pDX,VP_OCT4,m_CButOct4);
    DDX_Control(pDX,VP_OCT5,m_CButOct5);
    DDX_Control(pDX,VP_OCT6,m_CButOct6);
    DDX_Control(pDX,VP_OCT7,m_CButOct7);
    DDX_Control(pDX,VP_OCT8,m_CButOct8);
    DDX_Control(pDX,VP_TOP,m_CButTop);
    DDX_Control(pDX,VP_BOTTOM,m_CButBottom);
    DDX_Control(pDX,VP_LEFT,m_CButLeft);
    DDX_Control(pDX,VP_RIGHT,m_CButRight);
    DDX_Control(pDX,VP_FRONT,m_CButFront);
    DDX_Control(pDX,VP_BACK,m_CButBack);
}


vp_CVpoint::vp_CVpoint():IcadDialog(VP_VPOINT) {
}

//***This is the main window
void CMainWindow::IcadVpointDia(void) {
    int ret,fi1;
	int tilemode,cvport;
	sds_real fr1,newvtwist,oldvtwist;
	sds_point pt0,pt1,pt2,pt3,pt4,savevdir;
	struct resbuf rb,rbucs,rbwcs;
	sds_name elast;
	struct gr_view newview, *pnewview;
	struct gr_viewvars viewvarsp;


    vp_CVpoint m_CVpoint;
    ret=m_CVpoint.DoModal();
    if(ret==IDOK) {



        if(sds_entlast(elast)==RTERROR)fi1=1;
		else fi1=0;
		rbucs.restype=rbwcs.restype=RTSHORT;
		rbucs.resval.rint=1;
		rbwcs.resval.rint=0;

		if(SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return;
		tilemode=rb.resval.rint;
		SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		cvport=rb.resval.rint;
		SDS_getvar(NULL,DB_QVIEWTWIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		oldvtwist=newvtwist=rb.resval.rreal;

		SDS_getvar(NULL,fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN) : DB_QEXTMIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(pt1,rb.resval.rpoint);
		SDS_getvar(NULL,fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX) : DB_QEXTMAX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(pt2,rb.resval.rpoint);
		if(icadRealEqual(pt1[0],pt2[0])){
			SDS_getvar(NULL,!fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN) : DB_QEXTMIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			pt1[0]=rb.resval.rpoint[0];
			SDS_getvar(NULL,!fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX) : DB_QEXTMAX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			pt2[0]=rb.resval.rpoint[0];
		}
		if(icadRealEqual(pt1[1],pt2[1])){
			SDS_getvar(NULL,!fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN) : DB_QEXTMIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			pt1[1]=rb.resval.rpoint[1];
			SDS_getvar(NULL,!fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX) : DB_QEXTMAX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			pt2[1]=rb.resval.rpoint[1];
		}
		pt1[0]=min(pt1[0],pt2[0]);
		pt1[1]=min(pt1[1],pt2[1]);
		pt2[0]=max(pt1[0],pt2[0]);
		pt2[1]=max(pt1[1],pt2[1]);
		if(icadRealEqual(pt1[0],pt2[0])||icadRealEqual(pt1[1],pt2[1]))return;
		if(fi1){
			SDS_getvar(NULL,DB_QELEVATION,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			pt1[2]=pt2[2]=rb.resval.rreal;
		}


		//we want to zoom extents for the new view in ONE STEP.
		//Change viewdir, then get a view structure, and  populate
		//it with the correct stuff for our new view
		pnewview=&newview;

		SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(savevdir,rb.resval.rpoint);

		ic_ptcpy(rb.resval.rpoint,m_CVpoint.dViewDir);
		SDS_setvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

		//populate the new struct

		SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
		SDS_ViewToVars2Use(SDS_CURVIEW,SDS_CURDWG,&viewvarsp);
		gr_initview(SDS_CURDWG,&SDS_CURCFG,&pnewview,&viewvarsp);
		//calc the new viewtwist we need to set
		for(fi1=0;fi1<2;fi1++){
			pt3[0]=pt3[1]=pt3[2]=0.0;
			if(!fi1)pt3[2]=1.0;
			else pt3[1]=1.0;
			sds_trans(pt3,&rbwcs,&rbucs,0,pt4);
			//pt4 is WCS Z axis in UCS coords, pt3 is UCS Z in UCS coords
			//put them into rp coords using new view.  Transform 0,0,0 as
			//well, so we can create the vector
			gr_ucs2rp(pt3,pt3,pnewview);
			pt0[0]=pt0[1]=pt0[2]=0.0;
			gr_ucs2rp(pt0,pt0,pnewview);
			pt3[0]-=pt0[0];
			pt3[1]-=pt0[1];
			pt3[2]-=pt0[2];
			gr_ucs2rp(pt4,pt4,pnewview);
			pt0[0]=pt0[1]=pt0[2]=0.0;
			sds_trans(pt0,&rbwcs,&rbucs,0,pt0);
			gr_ucs2rp(pt0,pt0,pnewview);
			pt4[0]-=pt0[0];
			pt4[1]-=pt0[1];
			pt4[2]-=pt0[2];
			if(fabs(pt4[0])<IC_ZRO && fabs(pt4[1])<IC_ZRO)continue;
			if(fabs(pt3[0])<IC_ZRO && fabs(pt3[1])<IC_ZRO){
				//if they're going back to plan view for where they were...
				fi1=-1;
			}
			break;
		}
		if(fi1!=-1){
			//normalize the XY portions of the vectors.  We know both aren't 0.0
			pt3[2]=atan2(pt3[1],pt3[0]);
			pt4[2]=atan2(pt4[1],pt4[0]);
			ic_normang(&pt3[2],&pt4[2]);
			newvtwist=pt4[2]-pt3[2];
		}
		if(newvtwist!=oldvtwist){
			rb.restype=RTREAL;
			rb.resval.rreal=newvtwist;
			SDS_setvar(NULL,DB_QVIEWTWIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
			SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
			SDS_ViewToVars2Use(SDS_CURVIEW,SDS_CURDWG,&viewvarsp);
			gr_initview(SDS_CURDWG,&SDS_CURCFG,&pnewview,&viewvarsp);
		}

		//now get extents according to that view
		SDS_rpextents(pt1,pt2,pt3,pt4,&newview);
		//pt3 & pt4 now represent screen coordinates
		//of dwg's extents.  Convert center of view
		//back to UCS using our new view also
		pt1[0]=(pt3[0]+pt4[0])/2.0;
		pt1[1]=(pt3[1]+pt4[1])/2.0;
		pt1[2]=0.0;
		gr_rp2ucs(pt1,pt2,pnewview);
		SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(icadRealEqual(pt4[1],pt3[1]) || (((pt4[0]-pt3[0])/(pt4[1]-pt3[1]))>(rb.resval.rpoint[0]/rb.resval.rpoint[1]))){
			fr1=((pt4[0]-pt3[0])*rb.resval.rpoint[1])/rb.resval.rpoint[0];
		}else{
			fr1=pt4[1]-pt3[1];
		}
		fr1*=1.02;
		//Now call zoomit to save all the stuff

		rb.restype=RT3DPOINT;
		ic_ptcpy(rb.resval.rpoint,savevdir);
		SDS_setvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);
		if(newvtwist!=oldvtwist){
			rb.restype=RTREAL;
			rb.resval.rreal=oldvtwist;
			SDS_setvar(NULL,DB_QVIEWTWIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		}

		SDS_ZoomIt(NULL,2,pt2,&fr1,NULL,m_CVpoint.dViewDir,&newvtwist,NULL);

		/*D.G.*/// The following call of SDS_SaveDataForPreviousOrUndo was added to create an undo unit
		// for REDO command (we should zoom after changing sysvars).
		SDS_SaveDataForPreviousOrUndo(m_pCurView, SDS_CURDWG, tilemode, cvport);

	}else if(ret==VP_PREV) {
        SDS_ZoomIt(NULL,0,NULL,NULL,NULL,NULL,NULL,NULL);
    }
}







