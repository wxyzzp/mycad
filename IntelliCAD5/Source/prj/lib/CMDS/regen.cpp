/* D:\ICADDEV\PRJ\LIB\CMDS\REGEN.CPP
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
#include "IcadView.h"
#include "IcadDoc.h"
#include "HideAndShade.h"

short cmd_redraw(void)
{
	SDS_MultibleMode=0;
	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	
    struct resbuf tilemode;
	if(SDS_getvar(NULL,DB_QTILEMODE,&tilemode,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
    struct resbuf cvport;
	if(SDS_getvar(NULL,DB_QCVPORT,&cvport,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);

	SDS_SetUndo(IC_UNDO_REDRAW_ON_REDO,NULL,NULL,NULL,SDS_CURDWG);

	if(tilemode.resval.rint==0 && cvport.resval.rint>1 && SDS_CURVIEW->m_pVportTabHip->ret_type()==DB_VIEWPORT)
	{
		sds_name ename;
		ename[0]=(long)SDS_CURVIEW->m_pVportTabHip;
		ename[1]=(long)SDS_CURDWG;
		SDS_ClearViewPort(SDS_CURVIEW->m_pVportTabHip,SDS_CURVIEW);
		sds_redraw(ename,IC_REDRAW_DRAW);
		SDS_SetClipInDC(SDS_CURVIEW,SDS_CURVIEW->m_pVportTabHip,FALSE);
		return(RTNORM);	
	}
	else
		return sds_redraw(NULL,IC_REDRAW_DONTCARE);
}

short cmd_redrawall(void) { 
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;			

	SDS_SetUndo(IC_UNDO_REDRAW_ON_REDO,NULL,NULL,NULL,SDS_CURDWG);

	CIcadView *pView;
	CIcadDoc  *pDoc=SDS_CURDOC;
	POSITION pos=pDoc->GetFirstViewPosition();
	
    struct resbuf tilemode;
	if(SDS_getvar(NULL,DB_QTILEMODE,&tilemode,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
    struct resbuf cvport;
	if(SDS_getvar(NULL,DB_QCVPORT,&cvport,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);

	cmd_InsideRedrawAll=TRUE;
	while (pos!=NULL)
	{
		pView=(CIcadView *)pDoc->GetNextView(pos);
		ASSERT_KINDOF(CIcadView, pView);
		if(tilemode.resval.rint==0 && cvport.resval.rint>1 && pView->m_pVportTabHip->ret_type()==DB_VIEWPORT)
		{
			cmd_pspace();
			SDS_RedrawDrawing(NULL,pView,pDoc,1);
			cmd_mspace();
		}
		else
			SDS_RedrawDrawing(NULL,pView,pDoc,1);
	}
	cmd_InsideRedrawAll=FALSE;

    return(RTNORM); 
}


short cmd_regen(void) { 
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;	

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	if(SDS_CURVIEW==NULL || SDS_CURGRVW==NULL) return(RTERROR);
    
	struct resbuf tilemode;
	if(SDS_getvar(NULL,DB_QTILEMODE,&tilemode,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
    struct resbuf cvport;
	if(SDS_getvar(NULL,DB_QCVPORT,&cvport,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);

	SDS_SetUndo(IC_UNDO_REGEN_ON_REDO,NULL,NULL,NULL,SDS_CURDWG);
	cmd_InsideRegen = true;
	SDS_CURDOC->m_RegenListView=NULL;
	if(tilemode.resval.rint==0 && cvport.resval.rint>1 && SDS_CURVIEW->m_pVportTabHip->ret_type()==DB_VIEWPORT) {
		SDS_VPeedToView(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip,&SDS_CURVIEW->m_pCurDwgView,SDS_CURVIEW);
		sds_name ename;
		ename[0]=(long)SDS_CURVIEW->m_pVportTabHip;
		ename[1]=(long)SDS_CURDWG;
		SDS_ClearViewPort(SDS_CURVIEW->m_pVportTabHip,SDS_CURVIEW);
		sds_redraw(ename,IC_REDRAW_DRAW);
		SDS_SetClipInDC(SDS_CURVIEW,SDS_CURVIEW->m_pVportTabHip,FALSE);
		if(SDS_CURDOC->m_RegenListView == NULL)
			SDS_CURDOC->m_RegenListView = SDS_CURVIEW;
	} else {
		struct gr_view *view=SDS_CURGRVW;
		struct gr_viewvars viewvarsp;
		SDS_ViewToVars2Use(SDS_CURVIEW,SDS_CURDWG,&viewvarsp);
		if(view) gr_initview(SDS_CURDWG,&SDS_CURCFG,&view,&viewvarsp);
		sds_redraw(NULL,IC_REDRAW_DONTCARE);
	}
	cmd_InsideRegen = false;

    return(RTNORM); 
}

short cmd_regenall(void) { 
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;			

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	if(SDS_CURVIEW==NULL || SDS_CURGRVW==NULL) return(RTERROR);

	struct gr_viewvars viewvarsp;
	CIcadView *pView;
	CIcadDoc  *pDoc=SDS_CURDOC;

	POSITION pos=pDoc->GetFirstViewPosition();

    struct resbuf tilemode;
	if(SDS_getvar(NULL,DB_QTILEMODE,&tilemode,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
    struct resbuf cvport;
	if(SDS_getvar(NULL,DB_QCVPORT,&cvport,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);

	SDS_SetUndo(IC_UNDO_REGEN_ON_REDO,NULL,NULL,NULL,SDS_CURDWG);

	cmd_InsideRegen = cmd_InsideRedrawAll = true;
	while (pos!=NULL) {
		pView=(CIcadView *)pDoc->GetNextView(pos);
		ASSERT_KINDOF(CIcadView, pView);
		if(tilemode.resval.rint==0 && cvport.resval.rint>1 && pView->m_pVportTabHip->ret_type()==DB_VIEWPORT) {
			cmd_pspace();
			pView->m_fUseRegenList=0;
			pDoc->m_RegenListView=NULL;
			SDS_SetClipInDC(pView,pView->m_pVportTabHip,TRUE);
			SDS_RedrawDrawing(NULL,pView,pDoc,1);
			cmd_mspace();
		} else {
			SDS_ViewToVars2Use(pView,SDS_CURDWG,&viewvarsp);
			gr_initview(SDS_CURDWG,&SDS_CURCFG,&pView->m_pCurDwgView,&viewvarsp);	
			pView->m_fUseRegenList=0;
			pDoc->m_RegenListView=NULL;
			SDS_RedrawDrawing(NULL,pView,pDoc,1);
		}
	}
	cmd_InsideRegen = cmd_InsideRedrawAll = false;
	return(RTNORM); 
}


short cmd_regenall_lweight()
{
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;			
	
	if(SDS_CURVIEW==NULL || SDS_CURGRVW==NULL) return(RTERROR);

	struct gr_viewvars viewvarsp;
	CIcadView *pView;
	CIcadDoc  *pDoc=SDS_CURDOC;

	POSITION pos=pDoc->GetFirstViewPosition();

    struct resbuf tilemode;
	if(SDS_getvar(NULL,DB_QTILEMODE,&tilemode,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
    struct resbuf cvport;
	if(SDS_getvar(NULL,DB_QCVPORT,&cvport,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);

	SDS_SetUndo(IC_UNDO_REGEN_ON_REDO,NULL,NULL,NULL,SDS_CURDWG);

	cmd_InsideRegen = cmd_InsideRedrawAll = true;
	while (pos!=NULL) {
		pView=(CIcadView *)pDoc->GetNextView(pos);
		ASSERT_KINDOF(CIcadView, pView);
		if(tilemode.resval.rint==0 && cvport.resval.rint>1 && pView->m_pVportTabHip->ret_type()==DB_VIEWPORT) {
			cmd_pspace();
			pView->m_fUseRegenList=0;
			pDoc->m_RegenListView=NULL;
			SDS_SetClipInDC(pView,pView->m_pVportTabHip,TRUE);
			SDS_RedrawDrawing(NULL,pView,pDoc,1);
			cmd_mspace();
		} else {
			SDS_ViewToVars2Use(pView,SDS_CURDWG,&viewvarsp);
			gr_initview(SDS_CURDWG,&SDS_CURCFG,&pView->m_pCurDwgView,&viewvarsp);	
			pView->m_fUseRegenList=0;
			pDoc->m_RegenListView=NULL;
			SDS_RedrawDrawing(NULL,pView,pDoc,1);
		}
	}
	cmd_InsideRegen = cmd_InsideRedrawAll = false;
	return(RTNORM); 

}
