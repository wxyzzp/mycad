/* C:\DEV\PRJ\ICAD\ICADVIEWCTL.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */


#include "icad.h"/*DNT*/
#include "IcadDoc.h"
#include "cmds.h"/*DNT*/
#include <GL/gl.h>
#include <GL/glu.h>
#include "resource.hm"/*DNT*/
#include "paths.h"
#include "IcadDialog.h"
#include "IcadView.h"

extern char cmd_dview_target;	//*** For Dview command, 1=rotate target, 0= (default)rotate camera
extern sds_name cmd_dview_sset; //*** For Dview command, set of objects to be viewed.  If all objects, both longs==0L
static POINT ViewCtlPlacement={-9999,-9999};

void view_SubClassItems(CDialog *Cdlg);
LRESULT CALLBACK view_StaticProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
WNDPROC view_StaticWndProc;


class view_Ctrl:public IcadDialog {
    public:
        view_Ctrl();
        ~view_Ctrl();
        HBITMAP hBitmap[13];
        BOOL OnInitDialog();
        double dAngleFrom,dAngleIn;
		CString csAngleFrom,csAngleIn;
        double dAddFactor;
		int iUpdate;
        CString csAddFactor;
        void ScreenPlacement();
    protected:
        virtual void DoDataExchange(CDataExchange *pDX);
		virtual void OnCancel();
		virtual void OnOK();
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		afx_msg void OnHelp();
        afx_msg void OnUp();
        afx_msg void OnDown();
        afx_msg void OnLeft();
        afx_msg void OnRight();
        afx_msg void OnXNeg();
        afx_msg void OnXPos();
        afx_msg void OnYNeg();
        afx_msg void OnYPos();
        afx_msg void OnZNeg();
        afx_msg void OnZPos();
        afx_msg void OnUpdate();
        afx_msg void OnZoomin();
        afx_msg void OnZoomout();
		afx_msg void OnZoomExt();
        afx_msg void OnPreset();
        afx_msg void OnSpinInc(NMHDR *pNotifyStruct,LRESULT *result);
		void view_viewctrl(int neg, int axis);
		void view_viewpan(int dir);
		void view_viewextents(void);
    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(view_Ctrl,CDialog)
    ON_BN_CLICKED(VIEW_PAN_UP,OnUp)
    ON_BN_CLICKED(VIEW_PAN_DOWN,OnDown)
    ON_BN_CLICKED(VIEW_PAN_LEFT,OnLeft)
    ON_BN_CLICKED(VIEW_PAN_RIGHT,OnRight)
    ON_BN_CLICKED(VIEW_NEG_X,OnXNeg)
    ON_BN_CLICKED(VIEW_POS_X,OnXPos)
    ON_BN_CLICKED(VIEW_NEG_Y,OnYNeg)
    ON_BN_CLICKED(VIEW_POS_Y,OnYPos)
    ON_BN_CLICKED(VIEW_NEG_Z,OnZNeg)
    ON_BN_CLICKED(VIEW_POS_Z,OnZPos)
    ON_BN_CLICKED(VIEW_UPDATE_DWG,OnUpdate)
    ON_BN_CLICKED(VIEW_ZOOM_IN,OnZoomin)
    ON_BN_CLICKED(VIEW_ZOOM_OUT,OnZoomout)
    ON_BN_CLICKED(VIEW_ZOOM_EXT,OnZoomExt)
    ON_BN_CLICKED(VIEW_PRESET,OnPreset)
    ON_NOTIFY(UDN_DELTAPOS,UPDN_VIEW_INC,OnSpinInc)
    ON_BN_CLICKED(IDHELP,OnHelp)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL view_Ctrl::OnHelpInfo(HELPINFO *pHel){
	OnHelp();
	return(true);
}

void view_Ctrl::OnHelp() {
	CommandRefHelp( m_hWnd, HLP_VIEWCTL);
}

void view_Ctrl::DoDataExchange(CDataExchange *pDX) {
    DDX_Text(pDX,VIEW_INC,csAddFactor);
    DDX_Text(pDX,VIEW_ANGLE_IN,csAngleIn);
    DDX_Text(pDX,VIEW_ANGLE_FROM,csAngleFrom);
    DDX_Check(pDX,VIEW_UPDATE_DWG,iUpdate);
}

void view_Ctrl::OnSpinInc(NMHDR *pNotifyStruct,LRESULT *result) {
	struct resbuf setgetrb;
//	int auprec;
//	SDS_getvar(NULL,DB_QAUPREC,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
//	auprec=min(setgetrb.resval.rint,2);
	SDS_getvar(NULL,DB_QAUNITS,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(setgetrb.resval.rint==1 || setgetrb.resval.rint==4) setgetrb.resval.rint=0;
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
    UpdateData(TRUE);
	SDS_angtof_convert(1,(LPCTSTR)csAddFactor,-1,&dAddFactor);
	dAddFactor+=-pNMUpDown->iDelta*IC_PI/180.0;
	if(setgetrb.resval.rint==3)
		sds_angtos_absolute(dAddFactor,3,2,csAddFactor.GetBuffer(IC_ACADNMLEN));
	else
		sds_angtos_absolute(dAddFactor,setgetrb.resval.rint,0,csAddFactor.GetBuffer(IC_ACADNMLEN));

    UpdateData(FALSE);
    *result=0;
}

void view_Ctrl::OnOK(){
	//if we're done and we weren't updating the view as we went along, then do so
	cmd_dview_target=0;
    UpdateData(TRUE);

	SDS_CURDOC->m_RegenListView=NULL;
	SDS_InitThisView(SDS_CURVIEW);


    //save old value
    RECT rect;
    GetWindowRect(&rect);
    SDS_CMainWindow->ScreenToClient(&rect);
    ViewCtlPlacement.x=rect.left;
    ViewCtlPlacement.y=rect.top;

    CDialog::OnOK();

    switch(SDS_CURVIEW->m_iViewMode) {
		case 2:
			cmd_shade();
			break;
		case 1:
            cmd_hide();
            break;
		default:
			/*D.Gavrilov*/// The following call of SDS_SaveDataForPreviousOrUndo was added to create an undo unit
			// for REDO command (we should zoom after changing sysvars). It's a temporary decision and may not
			// work in some cases (nested commands, etc.) and it's not nice. Really non-invertable undo-groups
			// should be implemented.
		    struct resbuf tilemode, cvport;
			SDS_getvar(NULL, DB_QTILEMODE, &tilemode, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			SDS_getvar(NULL, DB_QCVPORT, &cvport, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			SDS_SaveDataForPreviousOrUndo(SDS_CURVIEW, SDS_CURDWG, tilemode.resval.rint, cvport.resval.rint);

            sds_redraw(NULL,IC_REDRAW_DONTCARE);
            break;
    }
}

void view_Ctrl::OnCancel(){
	CDialog::OnCancel();

    cmd_dview_target=0;
	SDS_ZoomIt(NULL,0,NULL,NULL,NULL,NULL,NULL,NULL);
}

void view_Ctrl::OnUpdate() {
    UpdateData(TRUE);
    if(iUpdate) {
        ::EnableWindow(::GetDlgItem(m_hWnd,VIEW_PAN_UP),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,VIEW_PAN_DOWN),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,VIEW_PAN_LEFT),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,VIEW_PAN_RIGHT),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,VIEW_ZOOM_IN),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,VIEW_ZOOM_OUT),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,VIEW_ZOOM_EXT),TRUE);
		//if the user turned updating on, make sure to update the view w/current data
		SDS_CURDOC->m_RegenListView=NULL;
		SDS_InitThisView(SDS_CURVIEW);

		switch(SDS_CURVIEW->m_iViewMode) {
			case 2: cmd_shade(); break;
			case 1: cmd_hide(); break;
			default: sds_redraw(NULL,IC_REDRAW_DONTCARE); break;
		}

    }else{
        ::EnableWindow(::GetDlgItem(m_hWnd,VIEW_PAN_UP),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,VIEW_PAN_DOWN),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,VIEW_PAN_LEFT),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,VIEW_PAN_RIGHT),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,VIEW_ZOOM_IN),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,VIEW_ZOOM_OUT),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,VIEW_ZOOM_EXT),FALSE);
    }
}

void view_Ctrl::ScreenPlacement(){
  //This is a next version project.
  //it works now but it needs a check to make sure
  //it is not off the screen.
//    SetWindowPos(&wndTop,ViewCtlPlacement.x,ViewCtlPlacement.y+38,0,0,SWP_SHOWWINDOW|SWP_NOSIZE);
    CRect rect;
    GetWindowRect(rect);
    SDS_CMainWindow->ScreenToClient(rect);
    rect+=ViewCtlPlacement;
    SDS_CMainWindow->ClientToScreen(rect);
    MoveWindow(rect);
}

BOOL view_Ctrl::OnInitDialog() {
    HWND hWnd;
    BOOL ret;

    view_SubClassItems(this);

    ret=IcadDialog::OnInitDialog();

//    if(ViewCtlPlacement.x!=-9999){
//        ScreenPlacement();
//    }

    hWnd=::GetDlgItem(m_hWnd,VIEW_PAN_UP);
	hBitmap[0] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VCTL_PAN_UP),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[0]);

	hWnd=::GetDlgItem(m_hWnd,VIEW_PAN_DOWN);
	hBitmap[1] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VCTL_PAN_DOWN),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[1]);

	hWnd=::GetDlgItem(m_hWnd,VIEW_PAN_LEFT);
	hBitmap[2] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VCTL_PAN_LEFT),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[2]);

	hWnd=::GetDlgItem(m_hWnd,VIEW_PAN_RIGHT);
	hBitmap[3] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VCTL_PAN_RIGHT),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[3]);

	hWnd=::GetDlgItem(m_hWnd,VIEW_NEG_X);
	hBitmap[4] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VCTL_X_NEG),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[4]);

	hWnd=::GetDlgItem(m_hWnd,VIEW_POS_X);
	hBitmap[5] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VCTL_X_POS),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[5]);

	hWnd=::GetDlgItem(m_hWnd,VIEW_NEG_Y);
	hBitmap[6] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VCTL_Y_NEG),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[6]);

	hWnd=::GetDlgItem(m_hWnd,VIEW_POS_Y);
	hBitmap[7] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VCTL_Y_POS),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[7]);

	hWnd=::GetDlgItem(m_hWnd,VIEW_NEG_Z);
	hBitmap[8] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VCTL_Z_NEG),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[8]);

	hWnd=::GetDlgItem(m_hWnd,VIEW_POS_Z);
	hBitmap[9] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(VCTL_Z_POS),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[9]);

	hWnd=::GetDlgItem(m_hWnd,VIEW_ZOOM_IN);
	hBitmap[10] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(IDB_ICAD_ZOOMIN),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[10]);

	hWnd=::GetDlgItem(m_hWnd,VIEW_ZOOM_OUT);
	hBitmap[11] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(IDB_ICAD_ZOOMOUT),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[11]);

	hWnd=::GetDlgItem(m_hWnd,VIEW_ZOOM_EXT);
	hBitmap[12] = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance
		,MAKEINTRESOURCE(IDB_ICAD_ZOOMEXT),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[12]);

	if(cmd_dview_target>0){
		hWnd=::GetDlgItem(m_hWnd,VIEW_PRESET);
		::EnableWindow(hWnd,false);
	}


	//call zoomit because we wanna save all present data in views llist
	SDS_ZoomIt(NULL,2,NULL,NULL,NULL,NULL,NULL,NULL);
	struct resbuf rb;

	if(2==cmd_dview_target)
		view_Ctrl::SetWindowText(ResourceString(IDC_ICADVIEWCTL_ROTATE_TARGE_1, "Rotate Target" ));
	else if(1==cmd_dview_target)
		view_Ctrl::SetWindowText(ResourceString(IDC_ICADVIEWCTL_ROTATE_CAMER_2, "Rotate Camera" ));
	else{	//default of 0, call from cmd_view where user hit Enter
		SDS_getvar(NULL,DB_QWORLDVIEW,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint==1)
			view_Ctrl::SetWindowText(ResourceString(IDC_ICADVIEWCTL_VIEW_CONTROL_3, "View Control - Relative to WCS" ));
		else
			view_Ctrl::SetWindowText(ResourceString(IDC_ICADVIEWCTL_VIEW_CONTROL_4, "View Control - Relative to UCS" ));
	}

    return(ret);
}

view_Ctrl::view_Ctrl():IcadDialog(VIEW_DIALOG) {
	struct resbuf setgetrb;
	int auprec,aunits;
	double fd1;
	sds_point viewdir;
	char angstr[IC_ACADNMLEN];

	iUpdate=1;
    SDS_getvar(NULL,DB_QAUPREC,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	auprec=max(setgetrb.resval.rint,2);
	SDS_getvar(NULL,DB_QAUNITS,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(setgetrb.resval.rint==1 || setgetrb.resval.rint==4)aunits=0;
	else aunits=setgetrb.resval.rint;
	if(0==cmd_dview_target){//if this is from vpoint
		if(setgetrb.resval.rint==3){
			sds_angtos_absolute(IC_PI/18.0,3,2,csAddFactor.GetBuffer(IC_ACADNMLEN));
		}else{
			sds_angtos_absolute(IC_PI/18.0,setgetrb.resval.rint,0,csAddFactor.GetBuffer(IC_ACADNMLEN));
		}
	}else{//from dview
		if(setgetrb.resval.rint==3){
			sds_angtos_absolute(0.1,3,2,csAddFactor.GetBuffer(IC_ACADNMLEN));
		}else{
			sds_angtos_absolute(IC_PI/30.0,setgetrb.resval.rint,0,csAddFactor.GetBuffer(IC_ACADNMLEN));
		}
	}
	SDS_getvar(NULL,DB_QVIEWDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(viewdir,setgetrb.resval.rpoint);
	dAngleIn=atan2(viewdir[1],viewdir[0]);
	sds_angtos_absolute(dAngleIn,aunits,auprec,csAngleIn.GetBuffer(IC_ACADNMLEN));
	fd1=sqrt(viewdir[0]*viewdir[0]+viewdir[1]*viewdir[1]);
	if(fd1>IC_ZRO){
		dAngleFrom=atan(viewdir[2]/fd1);
	}else if(viewdir[2]>=0.0)dAngleFrom=IC_PI/2.0;
	else dAngleFrom=-IC_PI/2.0;
	if(dAngleFrom>=0.0)
		sds_angtos_absolute(dAngleFrom,aunits,auprec,angstr);
	else{
		*angstr='-';
		sds_angtos_absolute(-dAngleFrom,aunits,auprec,angstr+1);
	}
	strncpy(csAngleFrom.GetBuffer(IC_ACADNMLEN),angstr,IC_ACADNMLEN-1);


}

view_Ctrl::~view_Ctrl() {
	for(int i=0;i<13;i++) DeleteObject(hBitmap[i]);
}

//**********************ROTATION ABOUT AN AXIS**************************
void view_Ctrl::view_viewctrl(int neg, int axis){
	//parameter 1: 0=rotation is positive, 1=rotate negative,
	//parameter 2: 0=X rotation, 1=Y rotation, 2=Z rotation

	UpdateData(TRUE);
	sds_resbuf rbdcs,rbucs;
	sds_point oldviewdir,newviewdir,viewctr,p1,p2;
	sds_real angsin,angcos,newviewtwist,viewdirlen;
	int aunits,auprec,viewmode;
	char angstr[IC_ACADNMLEN];
	CIcadView *pView=SDS_CURVIEW;

	rbdcs.rbnext=rbucs.rbnext=NULL;
	//if we're moving camera from DVIEW, then reverse what buttons do
	if(1==cmd_dview_target){
		if(neg)neg=0;
		else neg=1;
	}

	SDS_getvar(NULL,DB_QAUPREC,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	auprec=max(rbucs.resval.rint,2);
	SDS_getvar(NULL,DB_QAUNITS,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	aunits=rbucs.resval.rint;
	if(aunits==4 || aunits==1) aunits=0;

	SDS_angtof_convert(1,(LPCTSTR)csAddFactor,-1,&dAddFactor);
	if (icadRealEqual(dAddFactor,0.0))return;
	SDS_getvar(NULL,DB_QVIEWMODE,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if((viewmode=rbucs.resval.rint)&1){
		SDS_getvar(NULL,DB_QTARGET,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(viewctr,rbucs.resval.rpoint);
	}else{
		SDS_getvar(NULL,DB_QVIEWCTR,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(viewctr,rbucs.resval.rpoint);
	}

	if(axis==2)goto zrotation;

	if(neg){
		angcos=cos(-dAddFactor);
		angsin=sin(-dAddFactor);
	}else{
		angcos=cos(dAddFactor);
		angsin=sin(dAddFactor);
	}
	//*************calculate new viewdir in UCS********************
	//NOTE: If we're changing viewdir, we're changing the camera for
	//	an ortho or perspective projection.  For perspective move of
	//	camera, we'll calc the camera offset and then add its opposite
	//	to the target!

	rbucs.restype=rbdcs.restype=RTSHORT;

	rbucs.resval.rint=1;
	rbdcs.resval.rint=2;
	if(axis==0){
		oldviewdir[0]=0.0;
		oldviewdir[1]=angsin;
		oldviewdir[2]=angcos;
	}else if(axis==1){
		oldviewdir[0]=angsin;
		oldviewdir[1]=0.0;
		oldviewdir[2]=angcos;
	}
	sds_trans(oldviewdir,&rbdcs,&rbucs,1,newviewdir);
	if(fabs(newviewdir[0])<IC_ZRO)newviewdir[0]=0.0;
	if(fabs(newviewdir[1])<IC_ZRO)newviewdir[1]=0.0;
	if(fabs(newviewdir[2])<IC_ZRO)newviewdir[2]=0.0;
	SDS_getvar(NULL,DB_QVIEWDIR,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(oldviewdir,rbucs.resval.rpoint);
	viewdirlen=sqrt(oldviewdir[0]*oldviewdir[0]+
					oldviewdir[1]*oldviewdir[1]+
					oldviewdir[2]*oldviewdir[2]);
	newviewdir[0]*=viewdirlen;
	newviewdir[1]*=viewdirlen;
	newviewdir[2]*=viewdirlen;

	//*************calculate new view twist*****************
	p1[0]=p1[1]=0.0; p1[2]=1.0;
	//let rbucs actually represent WCS resbuf.
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=0;
	sds_trans(p1,&rbucs,&rbdcs,1,p2);
	if(axis==1){
		p1[1]=p2[1];
		p1[0]=p2[0]*angcos-p2[2]*angsin;
		p1[2]=p2[2]*angcos+p2[0]*angsin;
	}else if(axis==0){
		p1[0]=p2[0];
		p1[1]=p2[1]*angcos-p2[2]*angsin;
		p1[2]=p2[2]*angcos+p2[1]*angsin;
	}
	if(fabs(p1[0])<IC_ZRO && fabs(p1[1])<IC_ZRO){
		//if we'll now be looking parallel to the WCS Z axis, use Y axis for viewtwist
		SDS_getvar(NULL,DB_QUCSYDIR,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(p1,rbucs.resval.rpoint);
		//again, let rbucs represent wcs resbuf
		rbucs.restype=RTSHORT;rbucs.resval.rint=0;
		sds_trans(p1,&rbucs,&rbdcs,1,p2);
		if(axis==1){
			p1[1]=p2[1];
			p1[0]=p2[0]*angcos-p2[2]*angsin;
			p1[2]=p2[2]*angcos+p2[0]*angsin;
		}else if(axis==0){
			p1[0]=p2[0];
			p1[1]=p2[1]*angcos-p2[2]*angsin;
			p1[2]=p2[2]*angcos+p2[1]*angsin;
		}
	}
	newviewtwist=atan2(p1[1],p1[0]);
	if(newviewtwist<(IC_PI/2.0))newviewtwist+=IC_TWOPI;
	newviewtwist-=IC_PI/2.0;

	//**********************Reset the vars accordingly******************
	if(2==cmd_dview_target){
		//adjust target point
		viewctr[0]-=newviewdir[0]-oldviewdir[0];
		viewctr[1]-=newviewdir[1]-oldviewdir[1];
		viewctr[2]-=newviewdir[2]-oldviewdir[2];
		rbucs.restype=RT3DPOINT;
		ic_ptcpy(rbucs.resval.rpoint,viewctr);
		sds_setvar("TARGET"/*DNT*/,&rbucs);
		ic_ptcpy(newviewdir,oldviewdir);
	}else{
		rbucs.restype=RT3DPOINT;
		ic_ptcpy(rbucs.resval.rpoint,newviewdir);
		sdsengine_setvar("VIEWDIR"/*DNT*/,&rbucs);
	}
	rbucs.restype=RTREAL;
	rbucs.resval.rreal=newviewtwist;
	sdsengine_setvar("VIEWTWIST"/*DNT*/,&rbucs);

	//*******************update viewdir info on dialog*****************
	//use newviewtwist as dummy var
	newviewtwist=sqrt(newviewdir[0]*newviewdir[0]+newviewdir[1]*newviewdir[1]);
	if(newviewtwist>IC_ZRO){
		dAngleIn=atan2(newviewdir[1],newviewdir[0]);
		//use atan for angfrom and report angle betw -pi/2 and +pi/2
		dAngleFrom=atan(newviewdir[2]/newviewtwist);
	}else{
		dAngleIn=0.0;
		if(newviewdir[0]<0.0)dAngleFrom=-IC_PI/2.0;
		else dAngleFrom=IC_PI/2.0;
	}
	sds_angtos_absolute(dAngleIn,aunits,auprec,csAngleIn.GetBuffer(IC_ACADNMLEN));
	if(dAngleFrom>=0.0){
		sds_angtos_absolute(dAngleFrom,aunits,auprec,angstr);
	}else{
		*angstr='-';
		sds_angtos_absolute(-dAngleFrom,aunits,auprec,angstr+1);
	}
	strncpy(csAngleFrom.GetBuffer(IC_ACADNMLEN),angstr,IC_ACADNMLEN-1);

	//**************deal w/Z-axis rotation separately*********************
	if(axis==2){
		zrotation:
		SDS_getvar(NULL,DB_QVIEWTWIST,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(neg){
			rbucs.resval.rreal-=dAddFactor;
		}else{
			rbucs.resval.rreal+=dAddFactor;
		}
		ic_normang(&rbucs.resval.rreal,NULL);
		sdsengine_setvar("VIEWTWIST"/*DNT*/,&rbucs);
		rbucs.restype=RT3DPOINT;
		ic_ptcpy(rbucs.resval.rpoint,viewctr);
		sdsengine_setvar("VIEWCTR"/*DNT*/,&rbucs);
	}

	//******************re-init the view********************************
	if(iUpdate){
		SDS_VarToTab(SDS_CURDWG,pView->m_pVportTabHip);
		SDS_CURDOC->m_RegenListView=NULL;
		SDS_InitThisView(SDS_CURVIEW);

		if(cmd_dview_target>0 && cmd_dview_sset[0]>0L){
			sds_grclear();
			long fl1;
			sds_name etemp;
			for(fl1=0;RTNORM==sds_ssname(cmd_dview_sset,fl1,etemp);fl1++){
				sds_entupd(etemp);
				sds_redraw(etemp,IC_REDRAW_DRAW);
			}
		}else{
			switch(SDS_CURVIEW->m_iViewMode) {
				case 2:
//					SetWindowPos(NULL,0,0,0,0,SWP_HIDEWINDOW);
					cmd_shade();
//					SetWindowPos(NULL,0,0,0,0,SWP_SHOWWINDOW);
					break;
				case 1:
//					::EnableWindow(m_hWnd,FALSE);
//					::ShowWindow(m_hWnd,SW_HIDE);
                    cmd_hide();
//					::EnableWindow(m_hWnd,TRUE);
//					::ShowWindow(m_hWnd,SW_SHOW);
                    break;
				default: sds_redraw(NULL,IC_REDRAW_DONTCARE); break;
			}
		}
	}

	// Set the drawing modified flag.
	SDS_DBModified( IC_DBMOD_VIEW_MODIFIED);
    UpdateData(FALSE);

	HWND hItemWnd1=::GetDlgItem(m_hWnd,VIEW_PIC);
    ::SendMessage(hItemWnd1,WM_PAINT,0,0);
}

//*************************Zoom Extents***********************************
void view_Ctrl::OnZoomExt(void){
	struct resbuf rb;
	sds_point pt1,pt2,pt3,pt4;
	sds_name fd1;

    if(sds_entlast(fd1)==RTERROR) return;

	SDS_getvar(NULL,DB_QEXTMIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    ic_ptcpy(pt1,rb.resval.rpoint);
	SDS_getvar(NULL,DB_QEXTMAX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    ic_ptcpy(pt2,rb.resval.rpoint);
	pt1[0]=min(pt1[0],pt2[0]);
	pt1[1]=min(pt1[1],pt2[1]);
	pt1[2]=min(pt1[2],pt2[2]);
	pt2[0]=max(pt1[0],pt2[0]);
	pt2[1]=max(pt1[1],pt2[1]);
	pt2[2]=max(pt1[2],pt2[2]);

	SDS_rpextents(pt1,pt2,pt3,pt4,NULL);
	//pt3 & pt4 now represent screen coordinates
	//of dwg's extents.  Convert center of view
	//back to UCS
	pt1[0]=(pt3[0]+pt4[0])/2.0;
	pt1[1]=(pt3[1]+pt4[1])/2.0;
	struct gr_view *view=SDS_CURGRVW;
	gr_rp2ucs(pt1,pt2,view);
	SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(icadRealEqual(pt4[1],pt3[1]) || (((pt4[0]-pt3[0])/(pt4[1]-pt3[1]))>(rb.resval.rpoint[0]/rb.resval.rpoint[1]))){
		pt1[0]=((pt4[0]-pt3[0])*rb.resval.rpoint[1])/rb.resval.rpoint[0];
	}else{
		pt1[0]=pt4[1]-pt3[1];
	}
	SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint&1){
		SDS_ZoomIt(NULL,1,pt2,&pt1[0],NULL,NULL,NULL,NULL);
	}else{
		SDS_ZoomIt(NULL,3,pt2,&pt1[0],NULL,NULL,NULL,NULL);
	}
}

//***********************PANNING***********************************
void view_Ctrl::view_viewpan(int dir){
	//controls all panning in dialog
	//DIR is as follows:
	//1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
	struct resbuf rb,rbucs;
	sds_real viewsize;
	sds_point p1,p2;
	int viewmode;

	SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if((viewmode=rb.resval.rint)&1)
		SDS_getvar(NULL,DB_QTARGET,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	else
		SDS_getvar(NULL,DB_QVIEWCTR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(p1,rb.resval.rpoint);
	SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	viewsize=rb.resval.rreal;
	if(dir<3){
		SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rpoint[1]>0.0)viewsize*=(rb.resval.rpoint[0]/rb.resval.rpoint[1]);
	}
	rb.restype=rbucs.restype=RTSHORT;
	rb.resval.rint=2;
	rbucs.resval.rint=1;
	sds_trans(p1,&rbucs,&rb,0,p2);
	switch (dir) {
		case 1: p2[0]-=0.25*viewsize; break;
		case 2: p2[0]+=0.25*viewsize; break;
		case 3: p2[1]+=0.25*viewsize; break;
		case 4: p2[1]-=0.25*viewsize; break;
	}
	sds_trans(p2,&rb,&rbucs,0,p1);
	if(viewmode&1)
		SDS_ZoomIt(NULL,1,p1,NULL,NULL,NULL,NULL,NULL);
	else
		SDS_ZoomIt(NULL,3,p1,NULL,NULL,NULL,NULL,NULL);
}

//if the user clicks on PanUp
void view_Ctrl::OnUp() {
	view_viewpan(3);
}

//if the user clicks on PanDown
void view_Ctrl::OnDown() {
	view_viewpan(4);
}

//if the user clicks on PanLeft
void view_Ctrl::OnLeft() {
	view_viewpan(1);
}

//if the user clicks on PanRight
void view_Ctrl::OnRight() {
	view_viewpan(2);
}

//if the user clicks on XNeg
void view_Ctrl::OnXNeg() {
	view_viewctrl(0,0);
}

//if the user clicks on XPos
void view_Ctrl::OnXPos() {
	view_viewctrl(1,0);
}

//if the user clicks on YNeg
void view_Ctrl::OnYNeg() {
	view_viewctrl(0,1);
}

//if the user clicks on YPos
void view_Ctrl::OnYPos() {
	view_viewctrl(1,1);
}

//if the user clicks on ZNeg
void view_Ctrl::OnZNeg() {
	view_viewctrl(1,2);
}

//if the user clicks on ZPos
void view_Ctrl::OnZPos() {
	view_viewctrl(0,2);
}

//if the user clicks on ZoomIn
void view_Ctrl::OnZoomin() {
	struct resbuf rb;

	SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint&1){
		SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		sds_real vsize=rb.resval.rreal*0.8;
		SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		rb.resval.rpoint[0]*=0.8;
		rb.resval.rpoint[1]*=0.8;
		rb.resval.rpoint[2]*=0.8;
		SDS_ZoomIt(NULL,1,NULL,&vsize,NULL,rb.resval.rpoint,NULL,NULL);
	}else{
		SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		rb.resval.rreal*=0.8;
		SDS_ZoomIt(NULL,3,NULL,&rb.resval.rreal,NULL,NULL,NULL,NULL);
	}
}

//if the user clicks on ZoomOut
void view_Ctrl::OnZoomout() {
	struct resbuf rb;

	SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint&1){
		SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		sds_real vsize=rb.resval.rreal*1.25;
		SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		rb.resval.rpoint[0]*=1.25;
		rb.resval.rpoint[1]*=1.25;
		rb.resval.rpoint[2]*=1.25;
		SDS_ZoomIt(NULL,1,NULL,&vsize,NULL,rb.resval.rpoint,NULL,NULL);
	}else{
		SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		rb.resval.rreal*=1.25;
		SDS_ZoomIt(NULL,3,NULL,&rb.resval.rreal,NULL,NULL,NULL,NULL);
	}
}

//if the user clicks on Preset
void view_Ctrl::OnPreset() {
	struct resbuf rb;
	int aunits,auprec;
	sds_real fr1;
	char angstr[IC_ACADNMLEN];

	cmd_ddvpoint();
	SDS_CURDOC->m_RegenListView=NULL;
	SDS_InitThisView(SDS_CURVIEW);

	switch(SDS_CURVIEW->m_iViewMode) {
		case 2: cmd_shade(); break;
		case 1: cmd_hide(); break;
		default: sds_redraw(NULL,IC_REDRAW_DONTCARE); break;
	}

	//******************update data on screen
	SDS_getvar(NULL,DB_QAUPREC,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	auprec=max(rb.resval.rint,2);
	SDS_getvar(NULL,DB_QAUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	aunits=rb.resval.rint;
	if(aunits==4 || aunits==1) aunits=0;
	SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	fr1=sqrt(rb.resval.rpoint[0]*rb.resval.rpoint[0]+rb.resval.rpoint[1]*rb.resval.rpoint[1]);
	if(fr1>IC_ZRO){
		dAngleIn=atan2(rb.resval.rpoint[1],rb.resval.rpoint[0]);
		//use atan for angfrom and report angle betw -pi/2 and +pi/2
		dAngleFrom=atan(rb.resval.rpoint[2]/fr1);
	}else{
		dAngleIn=0.0;
		if(rb.resval.rpoint[0]<0.0)dAngleFrom=-IC_PI/2.0;
		else dAngleFrom=IC_PI/2.0;
	}
	sds_angtos_absolute(dAngleIn,aunits,auprec,csAngleIn.GetBuffer(IC_ACADNMLEN));
	if(dAngleFrom>=0.0){
		sds_angtos_absolute(dAngleFrom,aunits,auprec,angstr);
	}else{
		*angstr='-';
		sds_angtos_absolute(-dAngleFrom,aunits,auprec,angstr+1);
	}
	strncpy(csAngleFrom.GetBuffer(IC_ACADNMLEN),angstr,IC_ACADNMLEN-1);
    UpdateData(FALSE);
    ::SendMessage(::GetDlgItem(m_hWnd,VIEW_PIC),WM_PAINT,0,0);

	// Set the drawing modified flag.
	SDS_DBModified( IC_DBMOD_VIEW_MODIFIED);
}

void CMainWindow::IcadViewCtl(void) {
    view_Ctrl m_viewCtrl;
    m_viewCtrl.DoModal();
}

void view_SubClassItems(CDialog *Cdlg) {
    _TCHAR fs1[81];
    CWnd   *hCurItem;

    if((hCurItem=Cdlg->GetWindow(GW_CHILD))==NULL) return;
    do {
        GetClassName(hCurItem->m_hWnd,fs1,80);
        if ((_tcsicmp(fs1,ResourceString(IDC_ICADVIEWCTL_STATIC_10, "Static" ))==0) && (VIEW_PIC==GetWindowLong(hCurItem->m_hWnd,GWL_ID))) {
            view_StaticWndProc=(WNDPROC)SetWindowLong(hCurItem->m_hWnd,GWL_WNDPROC,(LONG)view_StaticProc);
        }
    }while((hCurItem=hCurItem->GetNextWindow(GW_HWNDNEXT))!=NULL);
    return;
}

LRESULT CALLBACK view_StaticProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {

    if(uMsg==WM_PAINT) {
		//*****************begin Open GL***********************
		HDC   ghDC;
		HGLRC ghRC;
	    RECT rect;
		int pixelformat,bkgcolor;
        sds_point viewdir;
        sds_real vtwist,fr1,fr2;
		short shadedge;
		long fl1;
		struct resbuf setgetrb;

		static PIXELFORMATDESCRIPTOR pfd=
		{
			sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
			1,                              // version number
			PFD_DRAW_TO_WINDOW |            // support window
			  PFD_SUPPORT_OPENGL |          // support OpenGL
			  PFD_DOUBLEBUFFER,             // double buffered
			PFD_TYPE_COLORINDEX,			// INDEX mode (temporary)
			//NOTE: for help on RGBA, search for OPENGL PALLETTE on MSDN CD
//			PFD_TYPE_RGBA,                  // RGBA type
			24,                             // 24-bit color depth
			0, 0, 0, 0, 0, 0,               // color bits ignored
			0,                              // no alpha buffer
			0,                              // shift bit ignored
			0,                              // no accumulation buffer
			0, 0, 0, 0,                     // accum bits ignored
			32,                             // 32-bit z-buffer
			0,                              // no stencil buffer
			0,                              // no auxiliary buffer
			PFD_MAIN_PLANE,                 // main layer
			0,                              // reserved
			0, 0, 0                         // layer masks ignored
		};

		ghDC = ::GetDC(hWnd);

		if((pixelformat = ChoosePixelFormat(ghDC, &pfd)) == 0 ) {
			cmd_ErrorPrompt(CMD_ERR_OPENGLCHOOSE,0);
			return(FALSE);
		}

		if (SetPixelFormat(ghDC, pixelformat, &pfd) == FALSE) {
			fl1=GetLastError();
			cmd_ErrorPrompt(CMD_ERR_OPENGLSET,0);
			::ReleaseDC(hWnd,ghDC);
			return(FALSE);
		}

		ghRC = wglCreateContext(ghDC);
		fl1=wglMakeCurrent(ghDC, ghRC);
		HPALETTE oldpal=::SelectPalette(ghDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);

		::GetClientRect(hWnd, &rect);

		SDS_getvar(NULL,DB_QBKGCOLOR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		bkgcolor=setgetrb.resval.rint;

		SDS_getvar(NULL,DB_QVIEWDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(viewdir,setgetrb.resval.rpoint);

		SDS_getvar(NULL,DB_QVIEWTWIST,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		vtwist=setgetrb.resval.rreal;
		SDS_getvar(NULL,DB_QSHADEDGE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		shadedge=setgetrb.resval.rint;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-2.0,2.0,-2.0,2.0,1.0,10.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glViewport(0,0,rect.right,rect.bottom);
		glClearIndex((GLfloat)255);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);
		//apply translations & rotations in reverse order!
		//fifth, twist the view
		if(!icadRealEqual(vtwist,0.0)){
			vtwist*=180.0/IC_PI;
			//if(fabs(viewdir[0])<IC_ZRO && fabs(viewdir[2])<IC_ZRO)vtwist*=180.0;
			glRotated(-vtwist,0,0,-1);
		}
		//fourth, offset the camera by the correct distance from the origin(target)
		glTranslated(0,0,-5);

		//third, rotate above/below X-Y plane
		for(fl1=0;fl1<3;fl1++){
			if(fabs(viewdir[fl1])<IC_ZRO)viewdir[fl1]=0.0;
		}
		fr2=sqrt(viewdir[0]*viewdir[0]+viewdir[1]*viewdir[1]);
		if (icadRealEqual(fr2,0.0)){
			if(viewdir[2]<0.0){
				glRotated(180.0,1,0,0);
			}
		}else{
			fr1=atan(viewdir[2]/fr2);
			fr1*=180.0/IC_PI;
			glRotated((90.0-fr1),-1,0,0);
		}
		//second, rotate around z-axis in XY plane
		if (icadRealEqual(viewdir[0],0.0)){
			if((viewdir[1]>0.0)||
			   (icadRealEqual(viewdir[1],0.0) && viewdir[2]<0.0))glRotated(180,0,0,1);
		}else{
			fr2=atan(viewdir[1]/viewdir[0]);
			fr2*=180.0/IC_PI;
			if(viewdir[0]>0.0){		//1st & 4th quadrant
				fr2+=90.0;
			}else {					//2nd & 3rd quadrant
				fr2-=90.0;
			}
			glRotated(fr2,0,0,-1);
		}
		//first, translate everything so that target (viewctr) is at origin

		glNewList(1, GL_COMPILE_AND_EXECUTE); //i don't think we need to start a display list

		//build the list of entities which comprise the
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		SDS_getvar(NULL,DB_QCOLORX,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		glIndexi(setgetrb.resval.rint-1);
		glBegin(GL_QUADS);
			glVertex3d(0,0,0);
			glVertex3d(1,0,0);
			glVertex3d(1,0,1);
			glVertex3d(0,0,1);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3d(0,1,0);
			glVertex3d(1,1,0);
			glVertex3d(1,1,1);
			glVertex3d(0,1,1);
		glEnd();
		SDS_getvar(NULL,DB_QCOLORY,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		glIndexi(setgetrb.resval.rint-1);
		glBegin(GL_QUADS);
			glVertex3d(0,0,0);
			glVertex3d(0,1,0);
			glVertex3d(1,1,0);
			glVertex3d(1,0,0);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3d(0,0,1);
			glVertex3d(0,1,1);
			glVertex3d(1,1,1);
			glVertex3d(1,0,1);
		glEnd();
		SDS_getvar(NULL,DB_QCOLORZ,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		glIndexi(setgetrb.resval.rint-1);
		glBegin(GL_QUADS);
			glVertex3d(0,0,0);
			glVertex3d(0,1,0);
			glVertex3d(0,1,1);
			glVertex3d(0,0,1);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3d(1,0,0);
			glVertex3d(1,1,0);
			glVertex3d(1,1,1);
			glVertex3d(1,0,1);
		glEnd();
		glIndexi(6);
		glLineWidth((float)2.0);
		glBegin(GL_LINES);
			glVertex3d(0,0,0);
			glVertex3d(1.9,0,0);
		glEnd();
		glBegin(GL_LINES);
			glVertex3d(0,0,0);
			glVertex3d(0,1.9,0);
		glEnd();
		glBegin(GL_LINES);
			glVertex3d(0,0,0);
			glVertex3d(0,0,1.9);
		glEnd();

		glEndList();
		glFinish();
		SwapBuffers(ghDC);
		wglMakeCurrent (NULL, NULL) ;
		wglDeleteContext (ghRC);

//		::SelectPalette(ghDC,oldpal,TRUE);
		::ReleaseDC(hWnd,ghDC);
    }
    return(CallWindowProc((WNDPROC)(long (__stdcall *)(void))view_StaticWndProc,hWnd,uMsg,wParam,lParam));
}




