/* D:\ICADDEV\PRJ\LIB\CMDS\MPSPACE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h" /*DNT*/
#include "IcadDoc.h"
#include "vxtabrec.h"
#include "IcadView.h"
#include "UCSIcon.h"
#include "DbLayout.h"
#include "cmdsViewport.h"

bool  cmd_InsideOnPaint;
bool  cmd_InsidePspaceCommand;
BOOL  cmd_InsideCreatePaperspace;

short cmd_pmspace(void)   { 
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_istransparent() == RTERROR) return RTERROR;
	ExecuteUIAction( ICAD_WNDACTION_PMSPACE );
    return(RTNORM); 
}


static bool 
redrawViewport( db_viewport *pMView, db_drawing *dp, redraw_mode mode )
	{
	sds_name ename;

	if ( (pMView == NULL) || 
		 (dp == NULL) ||
		 (pMView->ret_type() != DB_VIEWPORT) )
		{
		return false;
		}

	ename[0]=(long)pMView;
	ename[1]=(long)dp;
	SDS_DontDrawVPGuts=TRUE;

	// Weird trick here.  Since VP border needs to show when VP is active, EVEN is the layer is off
	// we need ot turn the layer on before we redraw, then set it back after it is drawn.
	db_handitem *laytab=NULL;
	int laycol;
	pMView->get_8(&laytab);
	if(laytab) 
		{
		laytab->get_62(&laycol);
		laytab->set_62(abs(laycol));
		}
	sds_redraw(ename, mode );
	if(laytab) 
		{
		laytab->set_62(laycol);
		}

	SDS_DontDrawVPGuts=FALSE;

	return true;
	}

void cmd_MakeVpActive(CIcadView *pView,db_handitem *vphip,db_drawing *dp) 
	{

	if(pView==NULL || vphip==NULL || dp==NULL) 
		{
		ASSERT( false ); // bad caller
		return;
		}

	int fi1,fi2,activevpleft;
	db_handitem *hip,*oldhip;
	struct resbuf rb;
	struct gr_viewvars viewvarsp;

	if(!cmd_InsideOnPaint || 
	   !cmd_InsidePspaceCommand) 
		{
		SDS_DrawUcsIcon(NULL,NULL,pView,1);
		}

	ExecuteUIAction( ICAD_WNDACTION_CURSOROFF ); 

static CIcadView *saveview = pView;
	oldhip=pView->m_pVportTabHip;  

	extern int SDS_AtCmdLine;
	if(SDS_AtCmdLine && 
		oldhip!=vphip && 
		!cmd_InsideOnPaint) 
		{
		SDS_SetUndo(IC_UNDO_COMMAND_BEGIN,(void *)"VP Change"/*DNT*/,NULL,NULL,dp);
		}
	SDS_SetUndo(IC_UNDO_PS_MS_VP_CHANGE,(void *)oldhip,NULL,NULL,dp);


	// Erase the image of the current viewport, we'll redraw everything further
	// down -- including the proper highlight for the current one
	//
	redrawViewport( (db_viewport *)pView->m_pVportTabHip, dp, IC_REDRAW_UNDRAW );

	if(pView->m_pVportTabHip->ret_type()==DB_VIEWPORT) 
		{
		SDS_VarToTab(dp,pView->m_pVportTabHip);
		}

	// Find this viewport hip.
	for(hip=dp->tblnext(DB_VXTABREC,1); hip!=NULL; hip=dp->tblnext(DB_VXTABREC,0)) 
		{
		if(vphip==((db_vxtabrec *)hip)->ret_vpehip()) 
			break;
		}
	// This probably is bad
	//
	ASSERT( hip != NULL );
	if(hip==NULL) 
		{
		return;
		}
	
	// Set this one as the current.
	fi1=1;
	vphip->set_68(fi1);	
	++fi1;
	/*DG - 27.4.2002*/// ... and setup dxf340 in the current paper space layout
	db_handitem*	pLayout = SDS_CURDWG->handent(SDS_CURDWG->ret_stockhand(DB_SHI_PSLAYOUT));
	if(pLayout)
		static_cast<CDbLayout*>(pLayout)->setLastViewport(vphip);
	
	rb.restype=RTSHORT;
	SDS_getvar(NULL,DB_QMAXACTVP,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	activevpleft=rb.resval.rint;

	// Scan through the rest and set the 68 order.
	for(hip=dp->tblnext(DB_VXTABREC,1); hip!=NULL; hip=dp->tblnext(DB_VXTABREC,0)) 
		{
		if(vphip==((db_vxtabrec *)hip)->ret_vpehip()) 
			{
			continue;
			}
		((db_vxtabrec *)hip)->ret_vpehip()->get_68(&fi2);
		if (activevpleft<1) 
			{
			fi1=-1;
			}
		if(fi2!=0) 
			{
			((db_vxtabrec *)hip)->ret_vpehip()->set_68(fi1);
			if (activevpleft) 
				{
				activevpleft--;
				++fi1;
				}
			}
		}

	vphip->get_69(&fi1);
	rb.restype=RTSHORT;
	rb.resval.rint=fi1;
	SDS_setvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

	pView->m_pVportTabHip=vphip;
	SDS_TabToVar(dp,pView->m_pVportTabHip);

	if(fi1==1) 
		{ // PS
		SDS_ViewToVars2Use(pView,dp,&viewvarsp);
		gr_initview(dp,&SDS_CURCFG,&pView->m_pCurDwgView,&viewvarsp);
		rb.restype=RTPOINT;
		ic_ptcpy( rb.resval.rpoint, viewvarsp.screensize );
		sdsengine_setvar("SCREENSIZE"/*DNT*/,&rb);
		} 
	else 
		{     // MS
		// Setup this MS rect in the view for the cursor changes.
		int x1,x2,y1,y2;
		SDS_GetRectForVP(pView,pView->m_pVportTabHip,&x1,&x2,&y1,&y2);
		pView->m_rectMspace.SetRect(x1,y1,x2,y2);
		SDS_VPeedToView(dp,pView->m_pVportTabHip,&pView->m_pCurDwgView,pView);
		rb.restype=RTPOINT;
		rb.resval.rpoint[0]=x2-x1;
		rb.resval.rpoint[1]=y2-y1;
		rb.resval.rpoint[2]=0.0;
		sdsengine_setvar("SCREENSIZE"/*DNT*/,&rb);
		}

	redrawViewport( (db_viewport *)pView->m_pVportTabHip, dp, IC_REDRAW_DRAW );
	redrawViewport( (db_viewport *)oldhip, dp, IC_REDRAW_DRAW );

	if(!cmd_InsideOnPaint || 
	   !cmd_InsidePspaceCommand) 
		{
		SDS_DrawUcsIcon(NULL,NULL,pView,0);
		}

	if(fi1==1) 
		{ // PS
		// Set up a clipping rect if this is not the PS viewport.
		SDS_SetClipInDC(pView,vphip,TRUE);
		} 
	else 
		{     // MS
		// Setup a clipping region for a viewport.
		SDS_SetClipInDC(pView,vphip,FALSE);
		}

	if(SDS_AtCmdLine && 
	   (oldhip!=vphip) && 
	   !cmd_InsideOnPaint) 
		{
		SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)"VP Change"/*DNT*/,NULL,NULL,dp);
		}

	SDS_UpdateSetvar=NULL;
	ExecuteUIAction( ICAD_WNDACTION_UDCHKSTAT );
	ExecuteUIAction( ICAD_WNDACTION_UDTBSTATE );
	ExecuteUIAction( ICAD_WNDACTION_STATCHANGE );

	}

bool cmd_SetActiveMView( CIcadView *pView, int vpnum, db_drawing *dp )
	{
	db_viewport *pViewport = NULL;
	db_vxtabrec *pVXTableRecord = NULL;
	// Find this viewport hip.
	for(pVXTableRecord=(db_vxtabrec *)dp->tblnext(DB_VXTABREC,1); pVXTableRecord!=NULL; pVXTableRecord=(db_vxtabrec *)dp->tblnext(DB_VXTABREC,0)) 
		{
		int iNum;
		pViewport = pVXTableRecord->ret_vpehip();
		ASSERT( pViewport->ret_type() == DB_VIEWPORT );
		pViewport->get_69( &iNum );
		if ( iNum == vpnum )
			break;
		pViewport = NULL;
		}
	// pViewport will be NULL if we didn't find a matching number
	//
	if ( pViewport == NULL )
		{
		return false;
		}

	cmd_MakeVpActive( pView, pViewport, dp );

	return true;
	}



short cmd_mspace(void)   { 
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_istransparent() == RTERROR) return RTERROR;	
	
    struct resbuf rb;
	if(SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	if(rb.resval.rint==1) {
		cmd_ErrorPrompt(CMD_ERR_TILEMODE0,0);
		return(RTERROR);
	}	

	CIcadView *pView=SDS_CURVIEW;
	if(pView==NULL) return(RTERROR);  
	db_drawing *flp=SDS_CURDWG;
	if(flp==NULL) return(NULL);

	int nrecs=flp->ret_ntabrecs(DB_VXTAB,0);
	if(nrecs<=1) {
		cmd_ErrorPrompt(CMD_ERR_NOVIEWPORTS,0);
		return(RTERROR);
	}

	if(SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	
	/*DG - 27.4.2002*/// we must set active viewport even if we are in MS because it can be different layout
											/*
	// If already in MS return.
	if(rb.resval.rint!=1) return(RTNORM);	*/

	// Check to make sure ALL VPs are not OFF.
	int nactive=0;
	db_handitem *hip=flp->tblnext(DB_VXTAB,1);
	while(hip=flp->tblnext(DB_VXTAB,0)) {
		int fi1;
		if(hip->ret_deleted()) continue;
		((db_vxtabrec *)hip)->ret_vpehip()->get_68(&fi1);
		if(fi1!=0) nactive++;
	}
	if(nactive==0) {
		SDS_CURDOC->m_nLastMsVport=0;
		cmd_ErrorPrompt(CMD_ERR_NOACTIVEVP,0);
		return(RTERROR);
	}

	// Find the MS VP to activate.
	hip=flp->tblnext(DB_VXTAB,1);
	while(hip=flp->tblnext(DB_VXTAB,0)) {
		int fi1;
		if(hip->ret_deleted()) continue;
		((db_vxtabrec *)hip)->ret_vpehip()->get_69(&fi1);
		if(SDS_CURDOC->m_nLastMsVport) {
			if(SDS_CURDOC->m_nLastMsVport==fi1) break;
		} else break;

	}
	if(hip==NULL) {
		hip=flp->tblnext(DB_VXTAB,1);
		hip=flp->tblnext(DB_VXTAB,0);
		if(hip==NULL) return(RTERROR);
	}

	hip=((db_vxtabrec *)hip)->ret_vpehip();

	cmd_MakeVpActive(pView,hip,SDS_CURDWG);
	
    return(RTNORM); 
}

short cmd_pspace(void)   { 
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;			
	if (cmd_istransparent() == RTERROR) return RTERROR;	
		
    struct resbuf rb;
	struct gr_viewvars viewvarsp;

	if(SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	if(rb.resval.rint==1) {
		cmd_ErrorPrompt(CMD_ERR_TILEMODE0,0);
		return(RTERROR);
	}
	CIcadView *pView=SDS_CURVIEW;
	if(pView==NULL) return(RTERROR);
	db_drawing *flp=SDS_CURDWG;
	if(flp==NULL || SDS_CURDOC==NULL) return(NULL);

	cmd_InsidePspaceCommand=TRUE;
	cmd_InsideCreatePaperspace = TRUE;

	// NOTE: Setting the second parameter to 0 will also count the records marked as 
	// deleted so we should never set it to 0 here.      SWH - 30/9/2004
	int nrecs = flp->ret_ntabrecs(DB_VXTAB,1);
	if(nrecs == 0)
	{
		POSITION pos = SDS_CURDOC->GetFirstViewPosition();
		if(pos != NULL) // change to ASSERT
		{
			pView = (CIcadView*)SDS_CURDOC->GetNextView(pos);
			ASSERT_KINDOF(CIcadView, pView);
			ASSERT(pos == NULL);

			// Make the paperspace viewport.
			pView->m_pVportTabHip = cmd_createVIEWPORT(flp, NULL, NULL);
			pView->m_pVportTabHip->set_68(1);
			pView->m_pVportTabHip->set_69(1);
			SDS_TabToVar(flp, pView->m_pVportTabHip);

			SDS_ViewToVars2Use(pView, flp, &viewvarsp);
			gr_initview(flp, &SDS_CURCFG, &pView->m_pCurDwgView, &viewvarsp);
		}
	} 
	pView = SDS_CURVIEW;

	if(SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		return(RTERROR);
	
	/*DG - 27.4.2002*/// we must set active viewport even if we are in PS because it can be different layout
							/*
	// unless already in PS return.
	if(rb.resval.rint!=1 )	*/
//	{
	if (SDS_CURDOC) 
		SDS_CURDOC->m_nLastMsVport=rb.resval.rint;

	db_handitem *hip=flp->tblnext(DB_VXTAB,1);
	ASSERT(hip != NULL); 
		
	hip=((db_vxtabrec *)hip)->ret_vpehip();
	cmd_MakeVpActive(pView,hip,SDS_CURDWG);
//	}

	cmd_InsidePspaceCommand=FALSE;
	cmd_InsideCreatePaperspace=FALSE;

    return(RTNORM); 
}

short cmd_isinpspace(void) {
	
	struct resbuf rb;

	SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int tilemode=rb.resval.rint;
	SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int cvport=rb.resval.rint;

	if(tilemode==0 && cvport==1) {
		cmd_ErrorPrompt(CMD_ERR_NOTINPSPACE,0);		
		return(RTERROR);							
	}
	
	return(RTNORM);
}



int cmd_GetNumberOfMViewPorts(void)
{
	db_drawing *flp = SDS_CURDWG;
	if(flp == NULL) return(NULL);

	int nrecs = flp->ret_ntabrecs(DB_VXTAB,0);
	
	return nrecs;
}
