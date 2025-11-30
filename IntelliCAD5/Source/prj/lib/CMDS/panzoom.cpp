/* G:\ICADDEV\PRJ\LIB\CMDS\PANZOOM.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "sdsapplication.h" /*DNT*/
#include "commandqueue.h" /*DNT*/
#include "IcadView.h"
#include "Preferences.h"

bool  cmd_InsideZoomExtents;

//EBATECH(Futa) 2001-07-10 -[ Dynamic Zoom support
int	 cmd_zoom_dynamic(void);
int	 zd_zoommax(sds_point& minpt,sds_point& maxpt,sds_point& v_min,sds_point& v_max);
void zd_mark(sds_point pt,int flg1,sds_real dx1,sds_point minpt,sds_point maxpt);
void zd_drawrect(sds_point pt0,sds_point pt2,int hlight,sds_point minpt,sds_point maxpt);
// ]-

short cmd_zoom(void) {			

	if (cmd_iswindowopen() == RTERROR) return RTERROR;			

    sds_point pt1, pt2, pt3, pt4, pt5, cp,lastpoint;
    sds_real fr1,fr2,elev;
    char fs1[IC_ACADBUF];
	char cst[3][50];
    int loopflg,dwgempty,oldosnap;
    RT	ret;
	struct resbuf rb;
    sds_name elast;
	
    SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(1&rb.resval.rint){
		cmd_ErrorPrompt(CMD_ERR_PERSPECTIVE,0);
		return(RTERROR);
	}
	if(SDS_getvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	ic_ptcpy(lastpoint,rb.resval.rpoint);

	if(SDS_getvar(NULL,DB_QELEVATION,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	elev=rb.resval.rreal;

	if(SDS_getvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	oldosnap=rb.resval.rint;
	rb.resval.rint=0;
	sds_setvar("OSMODE"/*DNT*/,&rb);

    top: ;

	// Since the display object list is not thread safe, we need to temporarily 
	// disable the hyperline tooltip to prevent the main win thread(see 
	// CIcadView::OnToolHitTest()) performing tool hit test on the display object 
	// list when this command thread is updating the display object list. SWH
    //
	bool bEnableHyperlinkTooltip = UserPreference::s_bEnableHyperlinkTooltip;
	UserPreference::s_bEnableHyperlinkTooltip = false;

    sds_initget(SDS_RSG_OTHER|SDS_RSG_NOLIM|SDS_RSG_NOLIM,ResourceString(IDC_PANZOOM_INITGET_ZOOM_IN__0, "Zoom_in|In Zoom_out|Out ~ All Center ~Dynamic Extents Left Previous Right ~Total ~Vmax Window ~_In ~_Out ~_ ~_All ~_Center ~_Dynamic ~_Extents ~_Left ~_Previous ~_Right ~_Total ~_Vmax ~_Window"));
	
	SDS_SetCursor(IDC_ZOOM);

	if( GetActiveCommandQueue()->IsNotEmpty() || 
		GetMenuQueue()->IsNotEmpty() || 
		lsp_cmdhasmore) 
		{
		SDS_WasFromPipeLine=TRUE;
		}
	else
		{

		SDS_WasFromPipeLine=FALSE;
		}

	if((ret=(sds_getpoint(NULL,ResourceString(IDC_PANZOOM__NZOOM___IN_OUT__1, "\nZoom:  In/Out/All/Center/Extents/Left/Previous/Right/Window/<Scale (nX/nXP)>: " ),pt1))) == RTCAN) goto out;
	SDS_SetCursor(IDC_ICAD_CROSS);
    
    if(ret==RTNORM) goto win;
    if(sds_entlast(elast)==RTERROR) dwgempty=1;
	else dwgempty=0;


    if(ret==RTKWORD){
        sds_getinput(fs1);
		if(strisame(fs1,"VMAX"/*DNT*/))
            strncpy(fs1,"EXTENTS"/*DNT*/,sizeof(fs1)-1);
		if (strisame(fs1,"IN"/*DNT*/)) {
			SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			fr1=0.5*rb.resval.rreal;
			SDS_ZoomIt(NULL,2,NULL,&fr1,NULL,NULL,NULL,NULL);
		}else if (strisame(fs1,"OUT"/*DNT*/)) {
			SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			fr1=2.0*rb.resval.rreal;
			SDS_ZoomIt(NULL,2,NULL,&fr1,NULL,NULL,NULL,NULL);
		}else if(strisame(fs1,"All"/*DNT*/)) {
			pt1[0]=pt1[1]=0.0;pt1[2]=1.0;
			SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if(!icadPointEqual(rb.resval.rpoint,pt1))cmd_zoom_extents(0);
			else cmd_zoom_extents(1);
        } else if(strisame(fs1,"CENTER"/*DNT*/)) {
			SDS_SetCursor(IDC_ZOOM);
			SDS_getvar(NULL,DB_QVIEWCTR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			sds_rtos(rb.resval.rpoint[0],-1,-1,cst[0]);
			sds_rtos(rb.resval.rpoint[1],-1,-1,cst[1]);
			sds_rtos(rb.resval.rpoint[2],-1,-1,cst[2]);            
			sprintf(fs1,ResourceString(IDC_PANZOOM__NCENTER_OF_VIEW_7, "\nCenter of view <%s,%s,%s>: " ),cst[0],cst[1],cst[2]);
			sds_initget(SDS_RSG_NOLIM,NULL);
			ret=sds_getpoint(NULL,fs1,cp);
			if(ret==RTNONE) ic_ptcpy(cp,rb.resval.rpoint);
			else if(ret!=RTNORM) goto out;
			SDS_SetCursor(IDC_ICAD_CROSS);
            
            loopflg=1;
            do {            
                sds_initget(134|SDS_RSG_NOVIEWCHG,NULL);
				SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				sds_rtos(rb.resval.rreal,-1,-1,cst[0]);
				sprintf(fs1,ResourceString(IDC_PANZOOM__NENTER_ZOOM_FAC_8, "\nEnter zoom factor (nX/nXP), or view height <%s>: " ),cst[0]);
                ret=sds_getdist(cp,fs1,&fr1);
                switch(ret) {
                    case RTNONE:
                        fr1=rb.resval.rreal;
                        loopflg=0;
                        break;
                    case RTNORM:
                        loopflg=0;
                        break;
                    case RTKWORD:
                        sds_getinput(fs1);        
                        if(!*fs1) break;
#ifdef LOCALISE_ISSUE
						// The following lines of code are checking characters not strings..
                        // Germans don't translate the X nor the P in AutoCAD, so we won't either
#endif
						if(strlen(fs1)>=2 && toupper(fs1[strlen(fs1)-2])=='X'/*DNT*/ && toupper(fs1[strlen(fs1)-1])=='P'/*DNT*/) {
							fs1[strlen(fs1)-2]=0;
							SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
							if(rb.resval.rint==1) goto jump1;
                            if((sds_distof(fs1,-1,&fr2)==RTNORM)&&(fr2>0.0)){
								SDS_CURVIEW->m_pVportTabHip->get_41(&rb.resval.rreal);
								fr1=rb.resval.rreal/fr2;
								loopflg=0;
							}
                        } else if(toupper(fs1[strlen(fs1)-1])=='X'/*DNT*/) {
                            fs1[strlen(fs1)-1]=0;
							jump1: ;
                            if((sds_distof(fs1,-1,&fr2)==RTNORM)&&(fr2>0.0)){
								SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
								fr1=rb.resval.rreal/fr2;
								loopflg=0;
							}
                        }
                        break;
                    default:
                        goto outgood;
                }
                if(loopflg) cmd_ErrorPrompt(CMD_ERR_TRYAGAIN,0); 
            } while(loopflg);

            SDS_ZoomIt(NULL,2,cp,&fr1,NULL,NULL,NULL,NULL);
        } else if(strisame(fs1,"DYNAMIC"/*DNT*/)) {
			//EBATECH(Fuat) 2001-7-3 Dynamic Zoom
			//cmd_ErrorPrompt(CMD_ERR_NODYNZOOM,0);
			cmd_zoom_dynamic();
        } else if(strisame(fs1,"TOTAL"/*DNT*/)) {
			//TODO zoom outside extents for open external dwgs			
        } else if(strisame(fs1,"EXTENTS"/*DNT*/)) {
				cmd_zoom_extents(0);
        } else if(strisame(fs1,"LEFT"/*DNT*/)) {
			SDS_SetCursor(IDC_ZOOM);
			sds_initget(SDS_RSG_NOLIM,NULL);
            if((ret=sds_getpoint(NULL,ResourceString(IDC_PANZOOM__NLOWER_LEFT_CO_13, "\nLower left corner: " ),pt1)) != RTNORM) goto out;
			SDS_SetCursor(IDC_ICAD_CROSS);
            
            loopflg=1;
            do {            
                sds_initget(134|RSG_NOLIM|SDS_RSG_NOVIEWCHG,NULL);
                ret=sds_getdist(pt1,ResourceString(IDC_PANZOOM__NENTER_ZOOM_FA_14, "\nEnter zoom factor (nX/nXP), or view height: " ),&fr1);
                switch(ret) {
                    case RTNORM:
                        loopflg=0;
                        break;
					case RTSTR:
                    case RTKWORD:
                        sds_getinput(fs1);        
                        if(!*fs1) break;
       					if(strlen(fs1)>=2 && toupper(fs1[strlen(fs1)-2])=='X'/*DNT*/ && toupper(fs1[strlen(fs1)-1])=='P'/*DNT*/) {
							fs1[strlen(fs1)-2]=0;
							SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
							if(rb.resval.rint==1) goto jump2;
                            if((sds_distof(fs1,-1,&fr2)==RTNORM)&&(fr2>0.0)){
								SDS_CURVIEW->m_pVportTabHip->get_41(&rb.resval.rreal);
								fr1=rb.resval.rreal/fr2;
								loopflg=0;
							}
                        } else if(toupper(fs1[strlen(fs1)-1])=='X'/*DNT*/) {
                            fs1[strlen(fs1)-1]=0;
							jump2: ;
                            if((sds_distof(fs1,-1,&fr2)==RTNORM)&&(fr2>0.0)){
								SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
								fr1=rb.resval.rreal/fr2;
								loopflg=0;
							}
                        }
                        break;
                    default:
                        goto outgood;
                }
                if(loopflg) cmd_ErrorPrompt(CMD_ERR_TRYAGAIN,0);
            } while(loopflg);

            SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
            cp[0]=pt1[0]+(0.5*fr1*rb.resval.rpoint[0]/rb.resval.rpoint[1]);
			cp[1]=pt1[1]+0.5*fr1;
            SDS_ZoomIt(NULL,2,cp,&fr1,NULL,NULL,NULL,NULL);
        } else if(strisame(fs1,"RIGHT"/*DNT*/)) {
			SDS_SetCursor(IDC_ZOOM);
			sds_initget(SDS_RSG_NOLIM,NULL);
            if((ret=sds_getpoint(NULL,ResourceString(IDC_PANZOOM__NUPPER_RIGHT_C_16, "\nUpper right corner: " ),pt1)) != RTNORM) goto out;
			SDS_SetCursor(IDC_ICAD_CROSS);

            loopflg=1;
            do {            
                sds_initget(134|RSG_NOLIM|SDS_RSG_NOVIEWCHG,NULL);
                ret=sds_getdist(pt1,ResourceString(IDC_PANZOOM__NENTER_ZOOM_FA_14, "\nEnter zoom factor (nX/nXP), or view height: " ),&fr1);
                switch(ret) {
                    case RTNORM:
                        loopflg=0;
                        break;
                    case RTKWORD:
                        sds_getinput(fs1);        
                        if(!*fs1) break;
						if(strlen(fs1)>=2 && toupper(fs1[strlen(fs1)-2])=='X'/*DNT*/ && toupper(fs1[strlen(fs1)-1])=='P'/*DNT*/) {
							fs1[strlen(fs1)-2]=0;
							SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
							if(rb.resval.rint==1) goto jump3;
                            if((sds_distof(fs1,-1,&fr2)==RTNORM)&&(fr2>0.0)){
								SDS_CURVIEW->m_pVportTabHip->get_41(&rb.resval.rreal);
								fr1=rb.resval.rreal/fr2;
								loopflg=0;
							}
                        } else if(toupper(fs1[strlen(fs1)-1])=='X'/*DNT*/) {
                            fs1[strlen(fs1)-1]=0;
							jump3: ;
                            if((sds_distof(fs1,-1,&fr2)==RTNORM)&&(fr2>0.0)){
								SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
								fr1=rb.resval.rreal/fr2;
								loopflg=0;
							}
                        }
                        break;
                    default:
                        goto outgood;
                }
                if(loopflg) cmd_ErrorPrompt(CMD_ERR_TRYAGAIN,0); 
            } while(loopflg);

            SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
            cp[0]=pt1[0]-(0.5*fr1*rb.resval.rpoint[0]/rb.resval.rpoint[1]);
			cp[1]=pt1[1]-0.5*fr1;
            SDS_ZoomIt(NULL,2,cp,&fr1,NULL,NULL,NULL,NULL);
        } else if(strisame(fs1,"PREVIOUS"/*DNT*/)) {
            SDS_ZoomIt(NULL,0,NULL,NULL,NULL,NULL,NULL,NULL);
        } else if(strisame(fs1,"WINDOW"/*DNT*/)) {
			SDS_SetCursor(IDC_ZOOM);
			sds_initget(SDS_RSG_NOLIM,NULL);
            if((ret=(sds_getpoint(NULL,ResourceString(IDC_PANZOOM__NFIRST_CORNER__19, "\nFirst corner: " ),pt1))) != RTNORM) goto out;
            win: ;
			SDS_SetCursor(IDC_ZOOM);
			sds_initget(SDS_RSG_NOLIM|SDS_RSG_NOVIEWCHG,NULL);
			SDS_DrawInOneWindow=1;
            if((ret=(sds_getcorner(pt1,ResourceString(IDC_PANZOOM__NOPPOSITE_CORN_20, "\nOpposite corner: " ),pt2))) != RTNORM) {
				SDS_DrawInOneWindow=0;
				goto out;
			}
			SDS_DrawInOneWindow=0;
			SDS_SetCursor(IDC_ICAD_CROSS);
			struct gr_view *view=SDS_CURGRVW;
			gr_ucs2rp(pt1,pt3,view);
			gr_ucs2rp(pt2,pt4,view);
			pt5[0]=(pt3[0]+pt4[0])/2.0;
			pt5[1]=(pt3[1]+pt4[1])/2.0;
			gr_rp2ucs(pt5,cp,view);
			SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if((fabs(pt4[0]-pt3[0])/fabs(pt4[1]-pt3[1]))>(rb.resval.rpoint[0]/rb.resval.rpoint[1]))
				fr1=fabs(pt4[0]-pt3[0])*rb.resval.rpoint[1]/rb.resval.rpoint[0];
			else
				fr1=fabs(pt4[1]-pt3[1]);
			SDS_ZoomIt(NULL,2,cp,&fr1,NULL,NULL,NULL,NULL);
        } else {
            if(!*fs1) {  cmd_ErrorPrompt(CMD_ERR_TRYAGAIN,0);  goto top;  }
            if(strlen(fs1)>=2 && toupper(fs1[strlen(fs1)-2])=='X'/*DNT*/ && toupper(fs1[strlen(fs1)-1])=='P'/*DNT*/) {
                fs1[strlen(fs1)-2]=0;
				SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if(rb.resval.rint==1) goto jump4;
                if((sds_distof(fs1,-1,&fr2)==RTNORM)&&(fr2>0.0)){
					SDS_CURVIEW->m_pVportTabHip->get_41(&rb.resval.rreal);
					fr1=rb.resval.rreal/fr2;
					loopflg=0;
				} else {
					fr2=-1.0;
				}
            } else if(toupper(fs1[strlen(fs1)-1])=='X'/*DNT*/) {
                fs1[strlen(fs1)-1]=0;
				jump4: ;
                if((sds_distof(fs1,-1,&fr2)==RTNORM)&&(fr2>0.0)){
					SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					fr1=rb.resval.rreal/fr2;
				}else{
					fr2=-1.0;
				}
            } else {
                if(sds_distof(fs1,-1,&fr2)==RTNORM){
					int tilemode,cvport;
					if(SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
					tilemode=rb.resval.rint;
					if(SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
					cvport=rb.resval.rint;

					SDS_getvar(NULL,dwgempty ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN) : DB_QEXTMIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					ic_ptcpy(pt1,rb.resval.rpoint);
					SDS_getvar(NULL,dwgempty ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX) : DB_QEXTMAX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					ic_ptcpy(pt2,rb.resval.rpoint);
					if(pt1[1]==pt2[1]) {
						SDS_getvar(NULL,!dwgempty ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN) : DB_QEXTMIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						pt1[1]=rb.resval.rpoint[1];
						SDS_getvar(NULL,!dwgempty ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX) : DB_QEXTMAX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						pt2[1]=rb.resval.rpoint[1];
					}
					fr1=fabs((pt2[1]-pt1[1])/fr2);
				}else
					fr2=-1.0;
            } 
            if(fr2<=0.0) { cmd_ErrorPrompt(CMD_ERR_TRYAGAIN,0);  goto top; }
            SDS_ZoomIt(NULL,2,NULL,&fr1,NULL,NULL,NULL,NULL);
        }
    }
	outgood:
	rb.restype=RTSHORT;
	rb.resval.rint=oldosnap;
	sds_setvar("OSMODE"/*DNT*/,&rb);
	rb.restype=RT3DPOINT;
	ic_ptcpy(rb.resval.rpoint,lastpoint);;
	sds_setvar("LASTPOINT"/*DNT*/,&rb);

	// reset UserPreference::s_bEnableHyperlinkTooltip
	//
	UserPreference::s_bEnableHyperlinkTooltip = bEnableHyperlinkTooltip;

    return(RTNORM); 

	out: ;
	SDS_SetCursor(IDC_ICAD_CROSS);
	rb.restype=RTSHORT;
	rb.resval.rint=oldosnap;
	sds_setvar("OSMODE"/*DNT*/,&rb);
	rb.restype=RT3DPOINT;
	ic_ptcpy(rb.resval.rpoint,lastpoint);;
	sds_setvar("LASTPOINT"/*DNT*/,&rb);

	// reset UserPreference::s_bEnableHyperlinkTooltip
	//
	UserPreference::s_bEnableHyperlinkTooltip = bEnableHyperlinkTooltip;
	return(ret);
}

int cmd_zoom_extents(int ck_limits)
	{
	//zooms to extents for current view.  If ck_limits !=0, we'll use the limits vars(
	//	(limmin & limmax) in zooming extents.  This is only valid in plan view, because 
	//	limits are 2D. Other than a call from Zoom All in plan view, this should be 0
	struct resbuf rb;
	sds_real fr1,elev;
	sds_point pt1,pt2,pt3,pt4,pt5,cp;
	int dwgempty;
	sds_name dummy;
	struct gr_view *view=SDS_CURGRVW;

	if( SDS_CURDWG==NULL || 
		SDS_CURVIEW==NULL) 
		{
		return(RTERROR);
		}

	// Zoom extents builds a new regen list and resets extents.

	SDS_getvar(NULL,DB_QREGENMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	if( !rb.resval.rint && 
		GetActiveCommandQueue()->IsEmpty() && 
		GetMenuQueue()->IsEmpty() && 
		!lsp_cmdhasmore && 
		!SDS_WasFromPipeLine) 
		{
		char fs1[IC_ACADBUF];

		if(sds_initget(0,ResourceString(IDC_PANZOOM_INITGET_YES_NO_21, "Yes No ~_Yes ~_No" ))!=RTNORM) 
			{
			return(RTERROR);
			}

		if(RTCAN==sds_getkword(ResourceString(IDC_PANZOOM__NABOUT_TO_REGE_22, "\nAbout to REGEN this drawing, OK to proceed? <Y>: " ),fs1)) 
			{
			return(RTCAN); 
			}

		if(strisame("NO"/*DNT*/,fs1)) 
			{
			return(RTCAN);
			}
		}

	if(RTCAN==SDS_BuildRegenList(SDS_CURDWG,SDS_CURVIEW->m_pCurDwgView,SDS_CURVIEW,SDS_CURDOC)) 
		{
		return(RTCAN);
		}


	SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int tilemode=rb.resval.rint;
	SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int cvport=rb.resval.rint;

	if(tilemode==0 && cvport==1) 
		{
		if( SDS_CURDWG->ret_nmainents()>1L ) 
			{
			dwgempty=0;
			}
		else 
			{
			dwgempty=1;
			}
		} 
	else 
		{
		if(sds_entlast(dummy)==RTERROR) 
			{
			dwgempty=1;
			}
		else 
			{
			dwgempty=0;
			}
		}

	SDS_getvar(NULL,DB_QELEVATION,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	elev=rb.resval.rreal;
	
	SDS_getvar(NULL,
		(dwgempty ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN) : ((tilemode==0 && cvport==1) ? DB_QP_EXTMIN : DB_QEXTMIN)),
		  &rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

    ic_ptcpy(pt3,rb.resval.rpoint);

	SDS_getvar(NULL,
		(dwgempty ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX) : ((tilemode==0 && cvport==1) ? DB_QP_EXTMAX : DB_QEXTMAX)),
		&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

    ic_ptcpy(pt4,rb.resval.rpoint);

	pt1[0]=min(pt3[0],pt4[0]);
	pt1[1]=min(pt3[1],pt4[1]);
	pt1[2]=min(pt3[2],pt4[2]);
	pt2[0]=max(pt3[0],pt4[0]);
	pt2[1]=max(pt3[1],pt4[1]);
	pt2[2]=max(pt3[2],pt4[2]);

	if(!dwgempty && ck_limits)
		{
		SDS_getvar(NULL,((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN),&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(pt3,rb.resval.rpoint);							            
		SDS_getvar(NULL,((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX),&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(pt4,rb.resval.rpoint);
		//note: limits are 2d, so don't check Z's
		pt1[0]=min(pt1[0],pt3[0]);
		pt1[1]=min(pt1[1],pt3[1]);
		pt2[0]=max(pt2[0],pt4[0]);
		pt2[1]=max(pt2[1],pt4[1]);
		}
	
	if(dwgempty)
		{
		pt1[2]=pt2[2]=elev;
		}

	SDS_rpextents(pt1,pt2,pt3,pt4,NULL);
	//pt3 & pt4 now represent screen coordinates
	//of dwg's extents.  Convert center of view
	//back to UCS 
	pt5[0]=(pt3[0]+pt4[0])/2.0;
	pt5[1]=(pt3[1]+pt4[1])/2.0;
	pt5[2]=0.0;
	gr_rp2ucs(pt5,cp,view);
	SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(icadRealEqual(pt4[1],pt3[1]) || ((pt4[0]-pt3[0])/(pt4[1]-pt3[1]))>(rb.resval.rpoint[0]/rb.resval.rpoint[1]))
		{
		fr1=((pt4[0]-pt3[0])*rb.resval.rpoint[1])/rb.resval.rpoint[0];
		}
	else
		{
		fr1=pt4[1]-pt3[1];
		}

	fr1*=1.02;
	cmd_InsideZoomExtents=TRUE;
	SDS_ZoomIt(NULL,2,cp,&fr1,NULL,NULL,NULL,NULL);
	cmd_InsideZoomExtents=FALSE;
	return(RTNORM);

	}


short cmd_pan(void)     {

	if (cmd_iswindowopen() == RTERROR) return RTERROR;			

    sds_point pt1, pt2, cp;
    RT	ret=RTNORM;
    struct resbuf rb;
    struct resbuf rbsnap;
	char fs1[25];
	sds_real fr1;
	int oldosmode;

    SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(1&rb.resval.rint){
		cmd_ErrorPrompt(CMD_ERR_PERSPECTIVE,0);
		return(RTERROR);
	}
    SDS_getvar(NULL,DB_QVIEWCTR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    ic_ptcpy(cp,rb.resval.rpoint);
    SDS_getvar(NULL,DB_QOSMODE,&rbsnap,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    oldosmode=rbsnap.resval.rint;
	rbsnap.resval.rint=0;
	sds_setvar("OSMODE"/*DNT*/,&rbsnap);

	sds_initget(SDS_RSG_NOLIM,ResourceString(IDC_PANZOOM_INITGET_LEFT_RI_24, "Left Right ~ Up Down ~ Page_left|PGLeft Page_right|PGRight ~ Page_up|PGUp Page_down|PGDown ~_Left ~_Right ~_ ~_Up ~_Down ~_ ~_PGLeft ~_PGRight ~_ ~_PGUp ~_PGDown" ));
	SDS_SetCursor(IDC_ICAD_OPENHAND);
    if((ret=(internalGetpoint(NULL,ResourceString(IDC_PANZOOM__N_LEFT_RIGHT_U_25, "\n Left/Right/Up/Down/PGLeft/PGRight/PGUp/PGDown/<Pan base point>: " ),pt1))) == RTNORM){ 
		SDS_SetCursor(IDC_ICAD_GRABHAND);
		sds_initget(SDS_RSG_NOLIM|SDS_RSG_NOVIEWCHG,NULL);
		SDS_DrawInOneWindow=1;
		if((ret=(internalGetpoint(pt1,ResourceString(IDC_PANZOOM__NPAN_DISPLACEM_26, "\nPan displacement point: " ),pt2))) != RTNORM) {
			SDS_DrawInOneWindow=0;
			SDS_SetCursor(IDC_ICAD_CROSS);
			goto exit;
		}
		SDS_DrawInOneWindow=0;
		SDS_SetCursor(IDC_ICAD_CROSS);
		cp[0]+=(pt1[0]-pt2[0]);
		cp[1]+=(pt1[1]-pt2[1]);
		cp[2]+=(pt1[2]-pt2[2]);
	}else if(ret!=RTKWORD){
		SDS_SetCursor(IDC_ICAD_CROSS);
		goto exit;
	}else{  //kword
		gr_ucs2rp(cp,pt1,SDS_CURGRVW);
		ic_ptcpy(cp,pt1);
		SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(pt1,rb.resval.rpoint);
		if(pt1[1]<CMD_FUZZ){
			SDS_SetCursor(IDC_ICAD_CROSS);
			ret=RTERROR;
			goto exit;
		}
		SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		fr1=rb.resval.rreal;
		sds_getinput(fs1);
		if(strisame("UP"/*DNT*/,fs1)) {
			cp[1]+=0.05*fr1;
		}else if(strisame("DOWN"/*DNT*/,fs1)) {
			cp[1]-=0.05*fr1;
		}else if(strisame("PGUP"/*DNT*/,fs1)) {
			cp[1]+=fr1;
		}else if(strisame("PGDOWN"/*DNT*/,fs1)) {
			cp[1]-=fr1;
		}else if(strisame("LEFT"/*DNT*/,fs1)) {
			cp[0]-=0.05*fr1*pt1[0]/pt1[1];
		}else if(strisame("RIGHT"/*DNT*/,fs1)) {
			cp[0]+=0.05*fr1*pt1[0]/pt1[1];
		}else if(strisame("PGLEFT"/*DNT*/,fs1)) {
			cp[0]-=fr1*pt1[0]/pt1[1];
		}else if(strisame("PGRIGHT"/*DNT*/,fs1)) {
			cp[0]+=fr1*pt1[0]/pt1[1];
		}else{
			ret=RTERROR;
			goto exit;
		}
		ic_ptcpy(pt1,cp);
		gr_rp2ucs(pt1,cp,SDS_CURGRVW);
	}
    SDS_ZoomIt(NULL,2,cp,NULL,NULL,NULL,NULL,NULL);
	SDS_SetCursor(IDC_ICAD_CROSS);
	
	exit:
	rbsnap.resval.rint=oldosmode;
	sds_setvar("OSMODE"/*DNT*/,&rbsnap);
    return(ret); 
}

//EBATECH(Futa) 2001-07-10	Dynamic Zoom
int cmd_zoom_dynamic(void)
{
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	RT	ret=RTNORM;
	struct resbuf rb;
	//struct resbuf rbsnap;
	//int oldosmode;
	struct gr_view *view=SDS_CURGRVW;
	sds_real s_asp,dx1,dy1,vsz,ovsz,zm_sz,min_vs;
	sds_point vcp,scp,vcp2,ovcp;
	sds_point pt0,pt1,cp0;
	sds_point minpt,maxpt;
	sds_point v_max,v_min,p_pt0,p_pt1;
	int iInputType=1;
	int key;
	struct resbuf rbUserInput;
	sds_point opt,tp,vdir,vdir2;
	sds_real odx;
	
	//no  cursor
	SDS_SetCursor(IDC_EMPTY);
	
	SDS_getvar(NULL,DB_QVIEWCTR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(vcp,rb.resval.rpoint);
	ic_ptcpy(ovcp,vcp);
	//SDS_getvar(NULL,DB_QOSMODE,&rbsnap,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	//oldosmode = rbsnap.resval.rint;
	//rbsnap.resval.rint = 0;
	//sds_setvar("OSMODE"/*DNT*/,&rbsnap);

	SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	vsz = rb.resval.rreal;
	SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	s_asp = rb.resval.rpoint[1] / rb.resval.rpoint[0];
	SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(vdir,rb.resval.rpoint);

	dx1 = vsz / s_asp / 2.0;
	dy1 = dx1 * s_asp;
	zm_sz = vsz;
	ovsz = vsz;

	gr_ucs2rp(vcp,scp,view);

	pt0[0] = scp[0] + dx1;
	pt0[1] = scp[1] + dy1;
	pt1[0] = scp[0] - dx1;
	pt1[1] = scp[1] - dy1;
	pt0[2] = pt1[2] = 0.0;

	gr_rp2ucs(pt0,p_pt0,view);
	gr_rp2ucs(pt1,p_pt1,view);
	p_pt0[2] = p_pt1[2] = 0.0;

	// zoom max
	if(zd_zoommax(minpt,maxpt,v_min,v_max) != RTNORM){
		return(RTERROR);
	}

	SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	vsz = rb.resval.rreal;
	min_vs = vsz / 1000.0;

	gr_ucs2rp(vcp,scp,view);

	if ((scp[0] > maxpt[0]) || (scp[0] < minpt[0]) ||
		(scp[1] > maxpt[1]) || (scp[1] < minpt[1]) || zm_sz > vsz ){
		scp[0] = (maxpt[0] + minpt[0]) * 0.5;
		scp[1] = (maxpt[1] + minpt[1]) * 0.5;
		dx1 = vsz / s_asp / 2.0 * 0.96;
		dy1 = dx1 * s_asp;
		zm_sz = vsz * 0.96;
		pt0[0] = scp[0] + dx1;
		pt0[1] = scp[1] + dy1;
		pt1[0] = scp[0] - dx1;
		pt1[1] = scp[1] - dy1;
		gr_rp2ucs(scp,opt,view);
	}else{
		ic_ptcpy(opt,vcp);
	}
	
	// draw previous view-rect
	zd_drawrect(pt0 , pt1 , 1 , minpt , maxpt);
	
	odx = dx1;
	short fl = 1;
	sds_real dx2,dx3;
	zd_mark(opt,fl,dx1,minpt,maxpt);
	SDS_getvar(NULL,DB_QVIEWCTR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(vcp,rb.resval.rpoint);

	while(RTNORM==(ret=sds_grread((1+2+8),&iInputType,&rbUserInput))) {

		SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(vdir2,rb.resval.rpoint);
		SDS_getvar(NULL,DB_QVIEWCTR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(vcp2,rb.resval.rpoint);
		SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (( fabs(vsz - rb.resval.rreal) > IC_ZRO) || ( fabs(vcp2[0] - vcp[0]) > IC_ZRO) ||
			( fabs(vcp2[1] - vcp[1]) > IC_ZRO)){

			dx1 *= rb.resval.rreal/vsz;
			zm_sz *= rb.resval.rreal/vsz;
			vsz = rb.resval.rreal;
			ic_ptcpy(vcp,vcp2);
			SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			s_asp = rb.resval.rpoint[1] / rb.resval.rpoint[0];
			sds_real dx11,dy11;
			dx11 = vsz / s_asp / 2.0;
			dy11 = dx11 * s_asp;
			gr_ucs2rp(vcp,scp,view);
			maxpt[0] = scp[0] + dx11;
			maxpt[1] = scp[1] + dy11;
			minpt[0] = scp[0] - dx11;
			minpt[1] = scp[1] - dy11;

			sds_point tp10,tp11;
			gr_ucs2rp(v_max,tp10,view);
			gr_ucs2rp(v_min,tp11,view);
			zd_drawrect(tp10 , tp11 , 1 , minpt , maxpt);

			gr_ucs2rp(p_pt0,tp10,view);
			gr_ucs2rp(p_pt1,tp11,view);
			zd_drawrect(tp10 , tp11 , 1 , minpt , maxpt);
			zd_mark(opt,fl,dx1,minpt,maxpt);
			odx = dx1;

			continue;
		}
		if (fabs(vdir[0]-vdir2[0])>IC_ZRO || fabs(vdir[1]-vdir2[1])>IC_ZRO || 
			fabs(vdir[2]-vdir2[2])>IC_ZRO) {
			ret = RTCAN;
			sds_printf(ResourceString(IDC_ICADAPI__N___CANCEL____60, "\nCancel" ));
			goto exit;
		}
		if (iInputType || 2 || 11 || 3 || 5){
			ic_ptcpy(cp0,rbUserInput.resval.rpoint);
			gr_ucs2rp(cp0,tp,view);
			if ((tp[0] < minpt[0] || tp[0] > maxpt[0]) || (tp[1] < minpt[1] || tp[1] > maxpt[1])){
				continue;
			}
		}
		switch(iInputType){
		case 2:		// Keyboard Input.	(Character code)
			key = rbUserInput.resval.rint;
			if (key != 13 && key != 32) break;
		case 11:	// Right mouse click
			if (zm_sz < min_vs) zm_sz = min_vs;
			zd_mark(opt,fl,dx1,minpt,maxpt);
			if (tp[0] < minpt[0] + dx1)
				tp[0] = minpt[0] + dx1;
			if (tp[1] < minpt[1] + dx1 * s_asp)
				tp[1] = minpt[1] + dx1 * s_asp;
			if (tp[0] > maxpt[0] - dx1)
				tp[0] = maxpt[0] - dx1;
			if (tp[1] > maxpt[1] - dx1 * s_asp)
				tp[1] = maxpt[1] - dx1 * s_asp;
			gr_rp2ucs(tp,cp0,view);
			SDS_ZoomIt(NULL,2,cp0,&zm_sz,NULL,NULL,NULL,NULL);
			ret=RTNORM;
			goto exit;
			break;
		case 3:		// The user selected a point.  (3D point)
			zd_mark(opt,fl,dx1,minpt,maxpt);
			fl*=-1;
			if (tp[0] < minpt[0] + dx1)
				tp[0] = minpt[0] + dx1;
			if (tp[1] < minpt[1] + dx1 * s_asp)
				tp[1] = minpt[1] + dx1 * s_asp;
			if (tp[0] > maxpt[0] - dx1)
				tp[0] = maxpt[0] - dx1;
			if (tp[1] > maxpt[1] - dx1 * s_asp)
				tp[1] = maxpt[1] - dx1 * s_asp;
			gr_rp2ucs(tp,cp0,view);
			zd_mark(cp0,fl,dx1,minpt,maxpt);
			ic_ptcpy(opt,cp0);
			odx = dx1;
			if (fl < 0)
				dx3=tp[0]-dx1;
			break;
		case 5:		// Returns drag-mode coordinates. (3D point)
			if (fabs(opt[0]-cp0[0])>IC_ZRO || fabs(opt[1]-cp0[1])>IC_ZRO || fabs(dx1-odx)>IC_ZRO){
				if(fl < 0){
					dx2=tp[0];
					if (dx2 > dx3){
						dx1 = dx2 - dx3;
						if (dx2 > maxpt[0] - dx1){
							dx1 = (maxpt[0] - dx3) / 2.0;
							tp[0] = maxpt[0] - dx1;
						}
						if (tp[1] < minpt[1] + dx1 * s_asp)
							tp[1] = minpt[1] + dx1 * s_asp;
						if (tp[1] > maxpt[1] - dx1 * s_asp)
							tp[1] = maxpt[1] - dx1 * s_asp;
						if (fabs(odx) < IC_ZRO)
							zm_sz = dx1 * 2.0;
						else
							zm_sz *= (dx1/odx);
						
					}else{
						dx1 = 0.0;
						tp[0] = dx3;
					}
				}else{
					if (tp[0] < minpt[0] + dx1)
						tp[0] = minpt[0] + dx1;
					if (tp[1] < minpt[1] + dx1 * s_asp)
						tp[1] = minpt[1] + dx1 * s_asp;
					if (tp[0] > maxpt[0] - dx1)
						tp[0] = maxpt[0] - dx1;
					if (tp[1] > maxpt[1] - dx1 * s_asp)
						tp[1] = maxpt[1] - dx1 * s_asp;
				}
				gr_rp2ucs(tp,cp0,view);
				if(opt[0]!=cp0[0] || opt[1]!=cp0[1]){
					zd_mark(opt,fl,odx,minpt,maxpt);
					zd_mark(cp0,fl,dx1,minpt,maxpt);
					ic_ptcpy(opt,cp0);
					odx = dx1;
				}
			}
			
			break;
		}
	}

	exit:

	SDS_SetCursor(IDC_ICAD_CROSS);
	if (ret!=RTNORM){
		rb.restype=RT3DPOINT;
		ic_ptcpy(rb.resval.rpoint,vdir);
		sdsengine_setvar("VIEWDIR",&rb);
		//zoom previous
		SDS_ZoomIt(NULL,2,ovcp,&ovsz,NULL,NULL,NULL,NULL);
	}
	//rbsnap.resval.rint=oldosmode;
	//sds_setvar("OSMODE"/*DNT*/,&rbsnap);
	return(ret); 
}

//EBATECH(Futa) 2001-07-10	Dynamic Zoom
int zd_zoommax(sds_point& minpt,sds_point& maxpt,sds_point& v_min,sds_point& v_max){
	struct resbuf rb;
	sds_real fr1,elev;
	sds_point pt1,pt2,pt3,pt4,pt5,cp;
	int dwgempty;
	sds_name dummy;
	struct gr_view *view=SDS_CURGRVW;

	if( SDS_CURDWG==NULL || 
		SDS_CURVIEW==NULL) 
		{
		return(RTERROR);
		}

	if(RTCAN==SDS_BuildRegenList(SDS_CURDWG,SDS_CURVIEW->m_pCurDwgView,SDS_CURVIEW,SDS_CURDOC)) 
		{
		return(RTCAN);
		}

	SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int tilemode=rb.resval.rint;
	SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int cvport=rb.resval.rint;

	if(tilemode==0 && cvport==1) 
		{
		if( SDS_CURDWG->ret_nmainents()>1L ) 
			{
			dwgempty=0;
			}
		else 
			{
			dwgempty=1;
			}
		} 
	else 
		{
		if(sds_entlast(dummy)==RTERROR) 
			{
			dwgempty=1;
			}
		else 
			{
			dwgempty=0;
			}
		}

	SDS_getvar(NULL,DB_QELEVATION,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	elev=rb.resval.rreal;
	
	SDS_getvar(NULL,
		(dwgempty ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN) : ((tilemode==0 && cvport==1) ? DB_QP_EXTMIN : DB_QEXTMIN)),
		  &rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	ic_ptcpy(pt3,rb.resval.rpoint);

	SDS_getvar(NULL,
		(dwgempty ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX) : ((tilemode==0 && cvport==1) ? DB_QP_EXTMAX : DB_QEXTMAX)),
		  &rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	ic_ptcpy(pt4,rb.resval.rpoint);

	pt1[0]=min(pt3[0],pt4[0]);
	pt1[1]=min(pt3[1],pt4[1]);
	pt1[2]=min(pt3[2],pt4[2]);
	pt2[0]=max(pt3[0],pt4[0]);
	pt2[1]=max(pt3[1],pt4[1]);
	pt2[2]=max(pt3[2],pt4[2]);

	if(!dwgempty)
		{
		SDS_getvar(NULL,((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN),&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(pt3,rb.resval.rpoint);										
		SDS_getvar(NULL,((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX),&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(pt4,rb.resval.rpoint);
		//note: limits are 2d, so don't check Z's
		pt1[0]=min(pt1[0],pt3[0]);
		pt1[1]=min(pt1[1],pt3[1]);
		pt2[0]=max(pt2[0],pt4[0]);
		pt2[1]=max(pt2[1],pt4[1]);
		}
	
	if(dwgempty)
		{
		pt1[2]=pt2[2]=elev;
		}

	SDS_rpextents(pt1,pt2,pt3,pt4,NULL);
	//pt3 & pt4 now represent screen coordinates
	//of dwg's extents.
	//Convert center of view back to UCS 
	pt5[0]=(pt3[0]+pt4[0])/2.0;
	pt5[1]=(pt3[1]+pt4[1])/2.0;
	pt5[2]=0.0;
	gr_rp2ucs(pt5,cp,view);
	SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(icadRealEqual(pt4[1],pt3[1]) || ((pt4[0]-pt3[0])/(pt4[1]-pt3[1]))>(rb.resval.rpoint[0]/rb.resval.rpoint[1]))
		{
		fr1=((pt4[0]-pt3[0])*rb.resval.rpoint[1])/rb.resval.rpoint[0];
		}
	else
		{
		fr1=pt4[1]-pt3[1];
		}
	//

	sds_point cp2,vcp,scp;
	sds_point ppt0,ppt1,ppt2,ppt3,mpt[4];
	sds_real dx1,dy1,max_x,min_x,max_y,min_y;
	sds_real vsz,s_asp;
	sds_real dv1,dv2;
	struct resbuf rbpsdcs,rbdcs;
	rbpsdcs.restype=rbdcs.restype=RTSHORT;
	rbpsdcs.resval.rint = 3;
	rbdcs.resval.rint = 2;

	s_asp = rb.resval.rpoint[1] / rb.resval.rpoint[0];

	SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	vsz = rb.resval.rreal;

	dx1 = vsz / s_asp / 2.0;
	dy1 = dx1 * s_asp;

	SDS_getvar(NULL,DB_QVIEWCTR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(vcp,rb.resval.rpoint);

	dv1 = fr1 / vsz;

	gr_ucs2rp(cp,cp2,view);
	ppt0[0] = ppt3[0] = cp2[0] + dx1 * dv1;
	ppt0[1] = ppt1[1] = cp2[1] + dy1 * dv1;
	ppt1[0] = ppt2[0] = cp2[0] - dx1 * dv1;
	ppt2[1] = ppt3[1] = cp2[1] - dy1 * dv1;
	ppt0[2] = ppt1[2] = ppt2[2] = ppt3[2] = 0.0;

	dv2 = 21.0;

	if(dv1 > dv2){
		sds_point pt20,pt21,pt22,pt23;
		gr_ucs2rp(vcp,scp,view);

		pt20[0] = pt23[0] = scp[0] + dx1 * dv2;
		pt20[1] = pt21[1] = scp[1] + dy1 * dv2;
		pt21[0] = pt22[0] = scp[0] - dx1 * dv2;
		pt22[1] = pt23[1] = scp[1] - dy1 * dv2;
		pt20[2] = pt21[2] = pt22[2] = pt23[2] = 0.0;

		if (pt20[0] > ppt0[0])
			scp[0]-=(pt20[0]-ppt0[0]);
		if (pt20[1] > ppt0[1])
			scp[1]-=(pt20[1]-ppt0[1]);
		if (pt22[0] < ppt2[0])
			scp[0]+=(ppt2[0]-pt22[0]);
		if (pt22[1] < ppt2[1])
			scp[1]+=(ppt2[1]-pt22[1]);
		gr_rp2ucs(scp,cp,view);
		fr1 = vsz * dv2;
	}

	fr1*=1.05;
	SDS_ZoomIt(NULL,2,cp,&fr1,NULL,NULL,NULL,NULL);

	SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	vsz = rb.resval.rreal;
	dx1 = vsz / s_asp / 2.0;
	dy1 = dx1 * s_asp;

	gr_ucs2rp(cp,cp2,view);

	maxpt[0] = cp2[0] + dx1;
	maxpt[1] = cp2[1] + dy1;
	minpt[0] = cp2[0] - dx1;
	minpt[1] = cp2[1] - dy1;
	maxpt[2] = minpt[2] = 0.0;

	ic_ptcpy(mpt[0],pt4);
	ic_ptcpy(mpt[2],pt3);
	mpt[1][0] = mpt[2][0];
	mpt[1][1] = mpt[0][1];
	mpt[3][0] = mpt[0][0];
	mpt[3][1] = mpt[2][1];

	max_x = mpt[0][0] ; min_x = mpt[0][0];
	max_y = mpt[0][1] ; min_y = mpt[0][1];
	for(int i=1;i < 4;i++){
		if (max_x < mpt[i][0]) max_x = mpt[i][0];
		if (min_x > mpt[i][0]) min_x = mpt[i][0];
		if (max_y < mpt[i][1]) max_y = mpt[i][1];
		if (min_y > mpt[i][1]) min_y = mpt[i][1];
	}
	
	ppt0[0] = max_x;
	ppt1[0] = min_x;
	ppt0[1] = max_y;
	ppt1[1] = min_y;
	ppt0[2] = ppt1[2] = 0.0;
	gr_rp2ucs(ppt0,v_max,view);
	gr_rp2ucs(ppt1,v_min,view);

	zd_drawrect(ppt0 , ppt1 , 1 , minpt , maxpt);

	return(RTNORM);
}

//EBATECH(Futa) 2001-07-10	Dynamic Zoom
void zd_mark(sds_point pt,int flg1,sds_real dx1,sds_point minpt,sds_point maxpt){
	
	struct resbuf rb;
	sds_point pt0,pt1,pt2,pt3,cp;
	sds_real vsz,s_asp,dy1,ds1;
	struct gr_view *view=SDS_CURGRVW;

	SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	vsz = rb.resval.rreal;

	SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	s_asp = rb.resval.rpoint[1] / rb.resval.rpoint[0];
	dy1 = dx1 * s_asp;

	gr_ucs2rp(pt,cp,view);

	pt0[0] = cp[0] + dx1;
	pt0[1] = cp[1] + dy1;
	pt1[0] = cp[0] - dx1;
	pt1[1] = cp[1] - dy1;
	pt0[2] = pt1[2] = 0.0;

	zd_drawrect(pt0 , pt1 , 0 , minpt , maxpt);

	ds1 = vsz * 0.035;	// Mark Size
	if (flg1 > 0){
		pt0[0] = pt3[0] = cp[0] + ds1;
		pt0[1] = pt1[1] = cp[1] + ds1;
		pt1[0] = pt2[0] = cp[0] - ds1;
		pt2[1] = pt3[1] = cp[1] - ds1;
		pt0[2] = pt1[2] = pt2[2] = pt3[2] = 0.0;

		gr_rp2ucs(pt0,pt0,view);
		gr_rp2ucs(pt1,pt1,view);
		gr_rp2ucs(pt2,pt2,view);
		gr_rp2ucs(pt3,pt3,view);

		sds_grdraw(pt0,pt2,-1,0);
		sds_grdraw(pt1,pt3,-1,0);
	}else{
		pt0[0] = cp[0] + dx1;
		pt0[1] = pt2[1] = cp[1];
		pt2[0] = pt0[0] - ds1 * 2.12;
		pt1[0] = pt3[0] = pt0[0] - ds1;
		pt1[1] = cp[1] + ds1;
		pt3[1] = cp[1] - ds1;
		pt0[2] = pt1[2] = pt2[2] = pt3[2] = 0.0;

		gr_rp2ucs(pt0,pt0,view);
		gr_rp2ucs(pt1,pt1,view);
		gr_rp2ucs(pt2,pt2,view);
		gr_rp2ucs(pt3,pt3,view);

		sds_grdraw(pt0,pt1,-1,0);
		sds_grdraw(pt0,pt2,-1,0);
		sds_grdraw(pt0,pt3,-1,0);
	}
}

//EBATECH(Futa) 2001-07-10	Dynamic Zoom
void zd_drawrect(sds_point pt0 , sds_point pt2 , int hlight
					, sds_point minpt , sds_point maxpt) {
	struct gr_view *view=SDS_CURGRVW;
	sds_point pt1,pt3;
	short fp[4];

	pt1[0] = pt2[0];
	pt1[1] = pt0[1];
	pt3[0] = pt0[0];
	pt3[1] = pt2[1];
	pt0[2] = pt1[2] = pt2[2] = pt3[2] = 0.0;
	
	fp[0] = fp[1] = fp[2] = fp[3] = 1;
	if (pt0[0] > maxpt[0]){
		pt0[0] = pt3[0] = maxpt[0];
		fp[3] = 0;
	}
	if (pt0[1] > maxpt[1]){
		pt0[1] = pt1[1] = maxpt[1];
		fp[0] = 0;
	}
	if (pt2[0] < minpt[0]){
		pt1[0] = pt2[0] = minpt[0];
		fp[1] = 0;
	}
	if (pt2[1] < minpt[1]){
		pt2[1] = pt3[1] = minpt[1];
		fp[2] = 0;
	}
	gr_rp2ucs(pt0,pt0,view);
	gr_rp2ucs(pt1,pt1,view);
	gr_rp2ucs(pt2,pt2,view);
	gr_rp2ucs(pt3,pt3,view);

	if (fp[0])
		sds_grdraw(pt0,pt1,-1,hlight);
	if (fp[1])
		sds_grdraw(pt1,pt2,-1,hlight);
	if (fp[2])
		sds_grdraw(pt2,pt3,-1,hlight);
	if (fp[3])
		sds_grdraw(pt3,pt0,-1,hlight);

}
