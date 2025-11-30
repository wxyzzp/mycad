/* D:\DEV\PRJ\ICAD\ICADZOOMVIEW.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

// *********************************************************************
// *  ICADZOOMVIEW.C                                                   *
// *  Copyright (C) 1994,1995, 1996, 1997 by Visio, Inc.               *
// *                                                                   *
// *********************************************************************

// ** Includes
#include "Icad.h"/*DNT*/
#include "IcadDoc.h"
#include "cmds.h"/*DNT*/
#include "sdsapplication.h" /*DNT*/
#include "commandqueue.h" /*DNT*/
#include "IcadView.h"

/**************************************************************************/
/****************** Zooming and View Functions ****************************/
/**************************************************************************/

#define SDS_ZOOMRATIO_MIN 2.75e-14 //NOTE:  Acad uses 2.98e-14
#define SDS_ZOOMRATIO_MAX 1.0e300

extern bool cmd_InsideZoomExtents;
extern int    lsp_cmdhasmore;
extern bool   SDS_WasFromPipeLine;

// ** Functions
int SDS_RedrawAfterZoom( CIcadView *pView, db_drawing *flp, int tilemode, int cvport, const sds_real *viewsize )
	{
	sds_name ename;

	// Make sure all the setvars are in the current table
	SDS_VarToTab(flp,pView->m_pVportTabHip);

	if( tilemode==0 && 
		(cvport > 1) && 
		pView->m_pVportTabHip->ret_type()==DB_VIEWPORT) 
		{
		SDS_VPeedToView(flp,pView->m_pVportTabHip,&pView->m_pCurDwgView,pView);
		ename[0]=(long)pView->m_pVportTabHip;
		ename[1]=(long)flp;

		SDS_ClearViewPort(pView->m_pVportTabHip,pView);
		sds_redraw(ename,IC_REDRAW_DRAW);
		SDS_SetClipInDC(pView,pView->m_pVportTabHip,FALSE);
		} 
	else 
		{

		struct gr_viewvars viewvarsp;
		SDS_ViewToVars2Use(pView,flp,&viewvarsp);
		gr_initview(flp,&SDS_CURCFG,&pView->m_pCurDwgView,&viewvarsp);

		switch(pView->m_iViewMode) 
			{
			case 2: 
				cmd_shade(); 
				break;
			case 1: 
				cmd_hide(); 
				break;
			default:
				if(viewsize!=NULL)
					{
					if( (tilemode ==1 && 
						 SDS_CURDOC->m_nOpenViews==1) && 
						 !cmd_InsideZoomExtents) 
						{
						if (!icadRealEqual(pView->m_dViewSizeAtLastRegen,0.0))
							{
							double ar1=(*viewsize)/pView->m_dViewSizeAtLastRegen;
							if(ar1<0.01 || ar1>100.0 || cmd_InsideZoomExtents) 
								{
								// DP: try to invoke regeneration implicit
								SDS_CURDOC->m_RegenListView = NULL;
								//if(RTCAN == SDS_BuildRegenList(flp, pView->m_pCurDwgView, pView, SDS_CURDOC))
								//	return(RTCAN);
								}	
							}
						}
					}
				// DP: use here explicit call
				SDS_RedrawDrawing(flp, NULL, NULL, 0x03);
				//sds_redraw(NULL, IC_REDRAW_DONTCARE);

				/*DG - 7.2.2002*/// This decision is not correct for all cases and can lead even to
				// crashes because m_pPrevSelection or ents in it are not valid (due to some operations
				// eg, undo); also this decision leads to highlighting of ents in cases when they
				// shouldn't be. So, commented out for better times...
//				/*DG 30.10.2001*/// Highlight the current selection (for transparent operations).
//				CIcadDoc*	pCurDoc = pView->GetDocument();
//				long		entNum = 0L;
//				sds_name	entName;
//				while(sds_ssname(pCurDoc->m_pPrevSelection, entNum++, entName) == RTNORM)
//					sds_redraw(entName, IC_REDRAW_HILITE);

				break;
			}
		}

	return RTNORM;
	}

//Add by IntelliKorea for solve zoom redo probelm 2001/5/1
int SDS_SaveDataForPreviousOrRedo( CIcadView *pView, db_drawing *flp, int tilemode, int cvport )
	{

	struct resbuf rb;
	sds_real curelev;
	SDS_getvar(NULL,DB_QELEVATION,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
	curelev=rb.resval.rreal;

	SDS_prevview *prevt = NULL;
	
	prevt= new struct SDS_prevview;
	memset(prevt,0,sizeof(struct SDS_prevview));

	if(prevt==NULL) 
		{
		return(RTERROR);
		}

	if( tilemode ==0 && 
		cvport==1) 
		{

	//save current settings for later use in call to previous
		prevt->prjelev=curelev;
		SDS_getvar(NULL,DB_QVIEWSIZE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		prevt->viewsize=rb.resval.rreal;
		SDS_getvar(NULL,DB_QVIEWDIR,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(prevt->viewdir,rb.resval.rpoint);
		SDS_getvar(NULL,DB_QVIEWMODE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		prevt->viewmode=rb.resval.rint;
		SDS_getvar(NULL,DB_QVIEWTWIST,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		prevt->twist=rb.resval.rreal;
		if((prevt->viewmode & (16+2))==18)
			{
			SDS_getvar(NULL,DB_QFRONTZ,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			prevt->frontz=rb.resval.rreal;
			}
		if((prevt->viewmode & 4)==4)
			{
			SDS_getvar(NULL,DB_QBACKZ,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			prevt->backz=rb.resval.rreal;
			}
		if((prevt->viewmode & 1)==0)
			{
			SDS_getvar(NULL,DB_QVIEWCTR,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			}
		else
			{
			SDS_getvar(NULL,DB_QTARGET,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			}
		ic_ptcpy(prevt->viewctr,rb.resval.rpoint);

		// Set the Undo.
		SDS_SetUndo(IC_UNDO_ZOOM,(void *)prevt,NULL,NULL,flp);
		delete prevt; prevt=NULL;

		} 
	else 
		{
		//save current settings for later use in call to previous
		prevt->prjelev=curelev;
		SDS_getvar(NULL,DB_QVIEWSIZE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		prevt->viewsize=rb.resval.rreal;
		SDS_getvar(NULL,DB_QVIEWDIR,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(prevt->viewdir,rb.resval.rpoint);
		SDS_getvar(NULL,DB_QVIEWMODE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		prevt->viewmode=rb.resval.rint;
		SDS_getvar(NULL,DB_QVIEWTWIST,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		prevt->twist=rb.resval.rreal;
		if((prevt->viewmode & (16+2))==18)
			{
			SDS_getvar(NULL,DB_QFRONTZ,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			prevt->frontz=rb.resval.rreal;
			}
		if((prevt->viewmode & 4)==4)
			{
			SDS_getvar(NULL,DB_QBACKZ,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			prevt->backz=rb.resval.rreal;
			}
		if((prevt->viewmode & 1)==0)
			{
			SDS_getvar(NULL,DB_QVIEWCTR,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			}
		else
			{
			SDS_getvar(NULL,DB_QTARGET,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			}
		ic_ptcpy(prevt->viewctr,rb.resval.rpoint);

		// Set the Undo.
		SDS_SetUndo(IC_UNDO_ZOOM,(void *)prevt,NULL,NULL,flp);
		delete prevt; prevt=NULL;
		}

	return RTNORM;
	}
//End of add. IntelliKorea 2001/5/1 for zoom redo problem.


int SDS_SaveDataForPreviousOrUndo( CIcadView *pView, db_drawing *flp, int tilemode, int cvport )
	{
	static const int maxviews = 25;
	//not zooming previous, so save current view as previous & adjust
	//requestd parameters
	//allocate new view save structure & check if we have more than maxviews saved views...

	struct resbuf rb;
	sds_real curelev;
	SDS_getvar(NULL,DB_QELEVATION,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
	curelev=rb.resval.rreal;

	SDS_prevview *prevt;
	
	prevt= new SDS_prevview;
	memset(prevt,0,sizeof(struct SDS_prevview));
	if(prevt==NULL) 
		{
		return(RTERROR);
		}

	if( tilemode ==0 && 
		cvport==1) 
		{

		if(pView->m_pZoomPsPrevB) 
			{
			prevt->prev=pView->m_pZoomPsPrevC;
			pView->m_pZoomPsPrevC=pView->m_pZoomPsPrevC->next=prevt;
			} 
		else 
			{
			pView->m_pZoomPsPrevB=pView->m_pZoomPsPrevC=prevt;
			}
		if(pView->m_iPrevPsViews<maxviews)
			{
			pView->m_iPrevPsViews++;
			}
		else if(pView->m_pZoomPsPrevB!=NULL)
			{	
			//drop off first view if too many
			prevt=pView->m_pZoomPsPrevB;
			pView->m_pZoomPsPrevB=pView->m_pZoomPsPrevB->next;
			pView->m_pZoomPsPrevB->prev=NULL;
			prevt->next=NULL;
			delete prevt;
			prevt=pView->m_pZoomPsPrevC;
			}

		//save current settings for later use in call to previous
		pView->m_pZoomPsPrevC->prjelev=curelev;
		SDS_getvar(NULL,DB_QVIEWSIZE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		pView->m_pZoomPsPrevC->viewsize=rb.resval.rreal;
		SDS_getvar(NULL,DB_QVIEWDIR,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(pView->m_pZoomPsPrevC->viewdir,rb.resval.rpoint);
		SDS_getvar(NULL,DB_QVIEWMODE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		pView->m_pZoomPsPrevC->viewmode=rb.resval.rint;
		SDS_getvar(NULL,DB_QVIEWTWIST,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		pView->m_pZoomPsPrevC->twist=rb.resval.rreal;
		if((pView->m_pZoomPsPrevC->viewmode & (16+2))==18)
			{
			SDS_getvar(NULL,DB_QFRONTZ,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			pView->m_pZoomPsPrevC->frontz=rb.resval.rreal;
			}
		if((pView->m_pZoomPsPrevC->viewmode & 4)==4)
			{
			SDS_getvar(NULL,DB_QBACKZ,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			pView->m_pZoomPsPrevC->backz=rb.resval.rreal;
			}
		if((pView->m_pZoomPsPrevC->viewmode & 1)==0)
			{
			SDS_getvar(NULL,DB_QVIEWCTR,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			}
		else
			{
			SDS_getvar(NULL,DB_QTARGET,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			}
		ic_ptcpy(pView->m_pZoomPsPrevC->viewctr,rb.resval.rpoint);

		// Set the Undo.
		SDS_SetUndo(IC_UNDO_ZOOM,(void *)pView->m_pZoomPsPrevC,NULL,NULL,flp);

		} 
	else 
		{
		if(pView->m_pZoomPrevB) 
			{
			prevt->prev=pView->m_pZoomPrevC;
			pView->m_pZoomPrevC=pView->m_pZoomPrevC->next=prevt;
			} 
		else 
			{
			pView->m_pZoomPrevB=pView->m_pZoomPrevC=prevt;
			}
		if(pView->m_iPrevViews<maxviews)
			{
			pView->m_iPrevViews++;
			}
		else if(pView->m_pZoomPrevB!=NULL)
			{	
			//drop off first view if too many
			prevt=pView->m_pZoomPrevB;
			pView->m_pZoomPrevB=pView->m_pZoomPrevB->next;
			pView->m_pZoomPrevB->prev=NULL;
			prevt->next=NULL;
			delete prevt;
			prevt=pView->m_pZoomPrevC;
			}

		//save current settings for later use in call to previous
		pView->m_pZoomPrevC->prjelev=curelev;
		SDS_getvar(NULL,DB_QVIEWSIZE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		pView->m_pZoomPrevC->viewsize=rb.resval.rreal;
		SDS_getvar(NULL,DB_QVIEWDIR,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(pView->m_pZoomPrevC->viewdir,rb.resval.rpoint);
		SDS_getvar(NULL,DB_QVIEWMODE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		pView->m_pZoomPrevC->viewmode=rb.resval.rint;
		SDS_getvar(NULL,DB_QVIEWTWIST,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		pView->m_pZoomPrevC->twist=rb.resval.rreal;
		if((pView->m_pZoomPrevC->viewmode & (16+2))==18)
			{
			SDS_getvar(NULL,DB_QFRONTZ,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			pView->m_pZoomPrevC->frontz=rb.resval.rreal;
			}
		if((pView->m_pZoomPrevC->viewmode & 4)==4)
			{
			SDS_getvar(NULL,DB_QBACKZ,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			pView->m_pZoomPrevC->backz=rb.resval.rreal;
			}
		if((pView->m_pZoomPrevC->viewmode & 1)==0)
			{
			SDS_getvar(NULL,DB_QVIEWCTR,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			}
		else
			{
			SDS_getvar(NULL,DB_QTARGET,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			}
		ic_ptcpy(pView->m_pZoomPrevC->viewctr,rb.resval.rpoint);

		// Set the Undo.
		SDS_SetUndo(IC_UNDO_ZOOM,(void *)pView->m_pZoomPrevC,NULL,NULL,flp);
		}

	return RTNORM;
	}

int SDS_ZoomIt(db_drawing *flp,
			    int mode, sds_point center, const sds_real *viewsize, const int *viewmode,
				 sds_point viewdir, const sds_real *twistorfrontz, const sds_real *backz) {
	//NOTES:    MODE==0:	Zoom Previous.  All parameters ignored
	//			MODE==1:	Center is TARGET for perspective view & twistorfrontz is frontz
	//			MODE==2:	Center is VIEWCTR for othographic view & twistorfrontz is viewtwist
	//			MODE==3:	Center is VIEWCTR for orthographic view & twistorfrontz is viewtwist AND
	//							we don't want to save view in structure for later use
	//			MODE==4:	center points to a SDS_prevview for undo
	//All parameters except mode which are NULL are ignored
	//NOTE: You can only zoom in so far.  This isn't an electron microscope.  If the ration of
	//			screensize[Y]/viewsize drops below SDS_ZOOMRATIO_MIN, you can't zoom in any further
	//			and you'll get an error msg telling you so.

	int doregen;
	sds_point oldcenter;
    sds_real curelev,vsize;
    struct resbuf rb,rb1;
	struct SDS_prevview *prevt;
	CIcadView *pView=SDS_CURVIEW;
	//static char sds_zoomit_minzooms; 2004/2/29 EBATECH(CNBR) no more used.
	struct SDS_prevview *Zundo=NULL;

	if(flp==NULL) flp=SDS_CURDWG;
	if(flp==NULL) return(RTERROR);
	if(pView==NULL) return(RTERROR);

	pView->m_bReScaleOnce=FALSE;
	doregen=0;
	SDS_getvar(NULL,DB_QELEVATION,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
	curelev=rb.resval.rreal;

    struct resbuf tilemode;
	if(SDS_getvar(NULL,DB_QTILEMODE,&tilemode,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
    struct resbuf cvport;
	if(SDS_getvar(NULL,DB_QCVPORT,&cvport,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);

	SDS_CallUserCallbackFunc(SDS_CBVIEWCHANGE,(void *)4,NULL,NULL);

	if(mode==4) {
		prevt=(struct SDS_prevview *)center;
		SDS_CURDOC->m_RegenListView = NULL; /*D.Gavrilov*/// We should regen all in subsequent call of SDS_RedrawDrawing.
		goto undozoom;
	}

    if(mode==0) {	//zoom previous & ignore all parameters
		if(tilemode.resval.rint==0 && cvport.resval.rint==1) {
			if(!pView->m_pZoomPsPrevB) {
				cmd_ErrorPrompt(CMD_ERR_NOPREVZOOM,0);
				pView->m_iPrevPsViews=0;
				return(RTERROR);
			}
			prevt=pView->m_pZoomPsPrevC;
		} else {
			if(!pView->m_pZoomPrevB) {
				cmd_ErrorPrompt(CMD_ERR_NOPREVZOOM,0);
				pView->m_iPrevViews=0;
				return(RTERROR);
			}
			prevt=pView->m_pZoomPrevC;
		}
		
		Zundo= new SDS_prevview;
		memset(Zundo,0,sizeof(SDS_prevview));

		if(tilemode.resval.rint==1 && SDS_CURDOC->m_nOpenViews==1) {
			double ar1=prevt->viewsize/pView->m_dViewSizeAtLastRegen;
			if(ar1<0.01 || ar1>100.0) {
				SDS_getvar(NULL,DB_QREGENMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if( !rb.resval.rint && 
					GetActiveCommandQueue()->IsEmpty() && 
					GetMenuQueue()->IsEmpty() && 
					!lsp_cmdhasmore && 
					!SDS_WasFromPipeLine) 
					{

					char fs1[IC_ACADBUF];
                    if(sds_initget(0,ResourceString(IDC_ICADZOOMVIEW_YES_NO_0, "Yes No ~_Yes ~_No" ))!=RTNORM) 
						{						
						delete Zundo;
						return(RTERROR);
						}
					if(RTCAN==sds_getkword(ResourceString(IDC_ICADZOOMVIEW__NABOUT_TO__1, "\nAbout to REGEN this drawing, OK to proceed? <Y>: " ),fs1)) 
						{
						delete Zundo;
						return(RTCAN);
						}
					if(strisame(fs1,"NO"/*DNT*/)) 
						{
						delete Zundo;
						return(RTCAN);
						}

					}
			}
		}

		undozoom: ;

		//set viewmode, viewsize, viewdir & projection elevation
		SDS_getvar(NULL,DB_QVIEWCTR,&rb1,flp,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(oldcenter,rb1.resval.rpoint);
		if(Zundo) ic_ptcpy(Zundo->viewctr,rb1.resval.rpoint);
		SDS_getvar(NULL,DB_QVIEWMODE,&rb1,flp,&SDS_CURCFG,&SDS_CURSES);
		if(Zundo) Zundo->viewmode=rb1.resval.rint;
		if(prevt->viewmode!=rb1.resval.rint){
			doregen=1;
			rb.restype=RTSHORT;
			rb.resval.rint=prevt->viewmode;
			if(sdsengine_setvar("VIEWMODE"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
			prevt->viewmode=rb1.resval.rint;
		}
		rb.restype=RTREAL;
		rb.resval.rreal=prevt->viewsize;
		SDS_getvar(NULL,DB_QVIEWSIZE,&rb1,flp,&SDS_CURCFG,&SDS_CURSES);
		if(Zundo) Zundo->viewsize=rb1.resval.rreal;
		if(sdsengine_setvar("VIEWSIZE"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
		prevt->viewsize=rb1.resval.rreal;
		SDS_getvar(NULL,DB_QVIEWDIR,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		if(Zundo) ic_ptcpy(Zundo->viewdir,rb.resval.rpoint);
		if(!icadPointEqual(rb.resval.rpoint, prevt->viewdir)){
			doregen=1;
			//rb.restype=RT3DPOINT;
			ic_ptcpy(rb.resval.rpoint,prevt->viewdir);
			SDS_getvar(NULL,DB_QVIEWDIR,&rb1,flp,&SDS_CURCFG,&SDS_CURSES);
			if(sdsengine_setvar("VIEWDIR"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
			ic_ptcpy(prevt->viewdir,rb1.resval.rpoint);
		}
		//NOTE:  you MUST set viewtwist BEFORE setting viewctr
		rb.restype=RTREAL;
		rb.resval.rreal=prevt->twist;
		SDS_getvar(NULL,DB_QVIEWTWIST,&rb1,flp,&SDS_CURCFG,&SDS_CURSES);
		if(Zundo) Zundo->twist=rb1.resval.rreal;
		if(sdsengine_setvar("VIEWTWIST"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
		prevt->twist=rb1.resval.rreal;
		//set viewctr or target
		if((prevt->viewmode&1)==1){//perspective view
			SDS_getvar(NULL,DB_QTARGET,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			if(Zundo) ic_ptcpy(Zundo->viewctr,rb1.resval.rpoint);
			if(!icadPointEqual(prevt->viewctr, rb.resval.rpoint)){
				doregen=1;
				ic_ptcpy(rb.resval.rpoint,prevt->viewctr);
				SDS_getvar(NULL,DB_QTARGET,&rb1,flp,&SDS_CURCFG,&SDS_CURSES);
				if(sds_setvar("TARGET"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
				ic_ptcpy(prevt->viewctr,rb1.resval.rpoint);
			}
			//in perspective view, we also might need to set the front & back clipping
			if((prevt->viewmode & (16+2))==18){
				rb.restype=RTREAL;
				rb.resval.rreal=prevt->frontz;
				SDS_getvar(NULL,DB_QFRONTZ,&rb1,flp,&SDS_CURCFG,&SDS_CURSES);
				if(Zundo) Zundo->frontz=rb1.resval.rreal;
				if(sds_setvar("FRONTZ"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
				prevt->frontz=rb1.resval.rreal;
			}
			if((prevt->viewmode & 4)==4){
				rb.restype=RTREAL;
				rb.resval.rreal=prevt->backz;
				SDS_getvar(NULL,DB_QBACKZ,&rb1,flp,&SDS_CURCFG,&SDS_CURSES);
				if(Zundo) Zundo->backz=rb1.resval.rreal;
				if(sds_setvar("BACKZ"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
				prevt->backz=rb1.resval.rreal;
			}
		}else{//orthographic view...
			ic_ptcpy(rb.resval.rpoint,prevt->viewctr);
			rb.restype=RT3DPOINT;
			if(sdsengine_setvar("VIEWCTR"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
			ic_ptcpy(prevt->viewctr,oldcenter);
		}
		//NOTE:  reset elevation temporarily for use in re-calc of projection values
		rb.restype=RTREAL;
		rb.resval.rreal=prevt->prjelev;
		SDS_getvar(NULL,DB_QELEVATION,&rb1,flp,&SDS_CURCFG,&SDS_CURSES);
		if(Zundo) Zundo->prjelev=rb1.resval.rreal;
		if(sds_setvar("ELEVATION"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
		prevt->prjelev=rb1.resval.rreal;

		// Set the Undo.
		if(Zundo) {
			SDS_SetUndo(IC_UNDO_ZOOM,(void *)Zundo,NULL,NULL,flp);
			delete Zundo; Zundo=NULL;
		}

		if(mode!=4) {
			//reset the viewing vars...
			if(tilemode.resval.rint==0 && cvport.resval.rint==1) {
				prevt=pView->m_pZoomPsPrevC;
				pView->m_pZoomPsPrevC=pView->m_pZoomPsPrevC->prev;
				if(pView->m_pZoomPsPrevC==NULL) pView->m_pZoomPsPrevB=NULL;
				else pView->m_pZoomPsPrevC->next=NULL;
				delete prevt;
				pView->m_iPrevPsViews--;
			} else {
				prevt=pView->m_pZoomPrevC;
				pView->m_pZoomPrevC=pView->m_pZoomPrevC->prev;
				if(pView->m_pZoomPrevC==NULL) pView->m_pZoomPrevB=NULL;
				else pView->m_pZoomPrevC->next=NULL;
				delete prevt;
				pView->m_iPrevViews--;
			}
		}else{
			goto initview;
		}
    } else {
		if(viewsize!=NULL){

			if((tilemode.resval.rint==1 && SDS_CURDOC->m_nOpenViews==1) && !cmd_InsideZoomExtents) {
				double ar1=(*viewsize)/pView->m_dViewSizeAtLastRegen;
				if(ar1<0.01 || ar1>100.0 || cmd_InsideZoomExtents) {
					SDS_getvar(NULL,DB_QREGENMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					if( !rb.resval.rint && 
						GetActiveCommandQueue()->IsEmpty() && 
						GetMenuQueue()->IsEmpty() && 
						!lsp_cmdhasmore && 
						!SDS_WasFromPipeLine) 
						{

						char fs1[IC_ACADBUF];
                        if(sds_initget(0,ResourceString(IDC_ICADZOOMVIEW_YES_NO_0, "Yes No ~_Yes ~_No" ))!=RTNORM) 
							{
							return(RTERROR);
							}
						if(RTCAN==sds_getkword(ResourceString(IDC_ICADZOOMVIEW__NABOUT_TO__1, "\nAbout to REGEN this drawing, OK to proceed? <Y>: " ),fs1)) 
							{
							return(RTCAN);
							}
						if(strisame(fs1,"NO"/*DNT*/)) 
							{
							return(RTCAN);
							}

						}
				}
			}

			//check to see that they're not zooming in too far...
			SDS_getvar(NULL,DB_QSCREENSIZE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
            // EBATECH(CNBR) 2004/2/29 -[ Disable flip view
			//if((*viewsize)/rb.resval.rpoint[1]<SDS_ZOOMRATIO_MIN)
			if(icadIsZero((*viewsize)/rb.resval.rpoint[1]))
            {
				vsize=rb.resval.rpoint[1]*SDS_ZOOMRATIO_MIN;
				cmd_ErrorPrompt(CMD_ERR_ZOOMIN,0);
				return(RTERROR);
//				sds_zoomit_minzooms++;
//				if(0==(sds_zoomit_minzooms%4))sds_printf("\nThis is not an electron microscope.");	// Some people have no sense of humor.
//				if(0==(sds_zoomit_minzooms%12))sds_printf(ResourceString(IDC_ICADZOOMVIEW__NOUR_SINC_12, "\nOur sincerest apologies - This is not an electron microscope.\nUnable to zoom in any closer." ));
			}
            else if((*viewsize)/rb.resval.rpoint[1]>SDS_ZOOMRATIO_MAX)
            {
				vsize=rb.resval.rpoint[1]*SDS_ZOOMRATIO_MAX;
				cmd_ErrorPrompt(CMD_ERR_ZOOMOUT,0);
				return(RTERROR);
//				sds_zoomit_minzooms++;
//				if(0==(sds_zoomit_minzooms%4))sds_printf("\nZooming must remain within known universe.");	// Ditto.
//				if(0==(sds_zoomit_minzooms%12))sds_printf(ResourceString(IDC_ICADZOOMVIEW__NUNFORTUN_13, "\nUnfortunately, zooming must remain within the known universe.\nUnable to zoom out any further." ));
			}
            else
            {
				vsize=*viewsize;
            }
           	// EBATECH(CNBR) ]- 2004/2/29
		}

		if(mode!=3)
			{
			//not a zoom previous....
			if ( SDS_SaveDataForPreviousOrUndo( pView, flp, tilemode.resval.rint, cvport.resval.rint ) == RTERROR )
				{
				return RTERROR;
				}
			}


  
		if(center==NULL)
			{
			//keep the center, because changing some of the other parameters
			//	will alter it, and we need to reset it later
			if(mode==1){
				SDS_getvar(NULL,DB_QTARGET,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			}else{
				SDS_getvar(NULL,DB_QVIEWCTR,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			}
			ic_ptcpy(oldcenter,rb.resval.rpoint);
		}

		//now set requested viewing parameters....

		if(viewsize!=NULL){
			rb.restype=RTREAL;
			rb.resval.rreal=fabs(vsize);
			if(sdsengine_setvar("VIEWSIZE"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
		}
		if(viewmode!=NULL){
			rb.restype=RTSHORT;
			rb.resval.rint=*viewmode;
			if ((rb.resval.rint==0)||(rb.resval.rint==1)){
 				if(sdsengine_setvar("VIEWMODE"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
 				doregen=1;
			}
		}
		if(viewdir!=NULL){
			rb.restype=RT3DPOINT;
			ic_ptcpy(rb.resval.rpoint,viewdir);
			if(sdsengine_setvar("VIEWDIR"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
			doregen=1;
		}
		if(twistorfrontz!=NULL){
			rb.restype=RTREAL;
			rb.resval.rreal=*twistorfrontz;
			if(mode!=1){
				if(sdsengine_setvar("VIEWTWIST"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
			}else{
				if(sds_setvar("FRONTZ"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
			}
		}
		if(backz!=NULL){
			rb.restype=RTREAL;
			rb.resval.rreal=*backz;
			if(sdsengine_setvar("BACKZ"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
		}
		if(center!=NULL){
			rb.restype=RT3DPOINT;
			ic_ptcpy(rb.resval.rpoint,center);
			if(mode!=1){
				if(sdsengine_setvar("VIEWCTR"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
			}else{
				if(sdsengine_setvar("TARGET"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
					doregen=1;
			}
		}
    }
	initview:
	//init view & exit
	//we may have reset the viewtwist & other stuff, so restore the center
	//  of view if no new one was specified
	if(center==NULL){
		rb.restype=RT3DPOINT;
		ic_ptcpy(rb.resval.rpoint,oldcenter);
		if(mode==2 || mode==3){
			if(sdsengine_setvar("VIEWCTR"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
		}else if (mode==1){
			if(sdsengine_setvar("TARGET"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
		}
	}
	if(doregen && SDS_CURDOC->m_RegenListView==pView)
		SDS_CURDOC->m_RegenListView=NULL;


	SDS_RedrawAfterZoom( pView, flp, tilemode.resval.rint, cvport.resval.rint, viewsize );

	//Modifed by IntelliKorea for zoom redo problem 2001/5/1
		if ( SDS_SaveDataForPreviousOrRedo( pView, flp, tilemode.resval.rint, cvport.resval.rint ) == RTERROR )
		{
			return RTERROR;
		}
	// SystemMetrix(shiba)-[06.10.2001
	// FIX: too slow and no need to redraw. Comment out.
	//SDS_RedrawAfterZoom( pView, flp, tilemode.resval.rint, cvport.resval.rint, viewsize);
	// ]-SystemMetrix(shiba) 06.10.2001
	//End of modifiacation. IntelliKorea 2001/5/1

	// Set the drawing modified flag.
	SDS_DBModified( IC_DBMOD_VIEW_MODIFIED);

	if(mode==0){
		//if zooming previous, remember to reset elevation back to current one
		//	note:  this may affect the viewctr var, but user will see what they wanted
		rb.restype=RTREAL;
		rb.resval.rreal=curelev;
		if(sdsengine_setvar("ELEVATION"/*DNT*/,&rb)!=RTNORM)return(RTERROR);
	}
	return(RTNORM);
}


