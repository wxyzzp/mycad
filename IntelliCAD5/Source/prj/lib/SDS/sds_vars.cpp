/* LIB\SDS\SDS_VARS.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Get/Set System Vars
 *
 */ 

//** Includes
#include "Icad.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "cmdslayout.h"/*DNT*/
#include "IcadCursor.h"
#include "IcadView.h"
#include "icadgridiconsnap.h"

bool      cmd_InsideTilemodeCmd;
char     *SDS_UpdateSetvar;
db_handitem *SDS_LastHandItem;

/****************************************************************************/
/*********************** DWG R/W Functions **********************************/
/****************************************************************************/

// *****************************************
// This is an SDS External API
//
int 
sdsengine_getvar(const char *szSysVar, struct sds_resbuf *prbVarInfo) 
	{

	ASSERT( OnEngineThread() );

	if(SDS_CMainWindow==NULL) 
		{
		return(RTERROR);
		}
    prbVarInfo->resval.rstring=NULL;
	prbVarInfo->rbnext = NULL;

    int ret = SDS_getvar(szSysVar,-1,prbVarInfo,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

    if (ret != RTNORM)
        {
        if (*szSysVar == '_'/*DNT*/)
            ret = SDS_getvar(szSysVar+1,-1,prbVarInfo,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
        else
            {
            char tmpvar[IC_ACADBUF];
            sprintf(tmpvar, "%s%s", "_"/*DNT*/, szSysVar);
            ret = SDS_getvar(tmpvar,-1,prbVarInfo,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
            }
        }

    return(ret);
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_setvar(const char *szSysVar, const struct sds_resbuf *prbVarInfo, bool bCheckReadOnly ) 
	{

	ASSERT( OnEngineThread() );

    int ret=RTNORM;
	struct resbuf rb;
	struct resbuf tilemode;
	struct resbuf cvport;
	struct gr_view *view=SDS_CURGRVW;

// to accommodate the sleazy memcmp method below, where we check the buffer by the
// size of a point whether or NOT a point was returned, we memset it to all 0 here to
// satisfy purify.
	
	memset(rb.resval.rpoint,0,3*sizeof(double));
	
	// Check to see if we need to change the value of the setvar.
	if(RTNORM!=sds_getvar(szSysVar,&rb)) 
		return(RTERROR);


	// ******************************
	// reject read-only
	// This check turned off because it was breaking things in 98d 
	//
	if ( bCheckReadOnly )
		{
		db_sysvarlink	*found = db_findsysvar( szSysVar, 0,  SDS_CURDWG);
		// Added Cybage VSB 13/08/2001 [
		// Reason: Fix for Bug No: 77824 from BugZilla
		//if ( found->spl & IC_SYSVARFLAG_USERREADONLY )
		if (found == NULL)
        {
			if (*szSysVar == '_'/*DNT*/)            
				found = db_findsysvar( szSysVar+1, 0,  SDS_CURDWG);
			else
            {
				char tmpvar[IC_ACADBUF];
				sprintf(tmpvar, "%s%s", "_"/*DNT*/, szSysVar);            
				found = db_findsysvar( tmpvar, 0,  SDS_CURDWG);
            }
        }
		if ( found && found->spl & IC_SYSVARFLAG_USERREADONLY )
		// Added Cybage VSB 13/08/2001 DD/MM/YYYY ]
			{
			if ( rb.restype==RTSTR )
				{
				IC_FREE(rb.resval.rstring);
				}
			return OL_ERDONLY;
			}
		}

// this is a partial fix.  This code used to simply compare the rpoint
// fields regardless of the actual type (the assumption being, apparently,
// that the rpoint field was the largest field of interest).  This caused
// lots of Purify uninitialized memory errors.  I have cased out some of the
// more common ones to attempt to cut down on the Purify errors.  A comparison
// of rpoint is still done as the default case.

	if (rb.restype==prbVarInfo->restype) {
		if (rb.restype==RTREAL) {
			if (rb.resval.rreal==prbVarInfo->resval.rreal) return RTNORM;
		}
		else  if (rb.restype==RTSHORT) {
			if (rb.resval.rint==prbVarInfo->resval.rint) return RTNORM;
		}
		else if (rb.restype==RTANG) {
			if (rb.resval.rreal==prbVarInfo->resval.rreal) return RTNORM;
		}
		else if (rb.restype==RTSTR) {
			if (strsame(rb.resval.rstring,prbVarInfo->resval.rstring)) {
				IC_FREE(rb.resval.rstring);
				return(RTNORM);
			}
		}
		else {
			if (!memcmp(rb.resval.rpoint,prbVarInfo->resval.rpoint,sizeof(sds_point))) 
				return(RTNORM);
		}
	}
    ret=SDS_setvar(szSysVar,-1,prbVarInfo,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

    if(ret==RTNORM) {

		// Handle the special case setvars that need a screen change
		struct db_sysvarlink *found;
		found=db_GetLastSetvar();
		
		// Set the Undo.
        if(found) SDS_SetUndo(IC_UNDO_SETVAR_CHANGE,(void *)found->name,(void *)&rb,NULL,SDS_CURDWG);

		if(found) {

			SDS_UpdateSetvar=(char *)szSysVar;

			// Update the toolbars/menus if we change any setvars that are 0 or 1.
			if(strsame("0_1"/*DNT*/,found->range)) {
				ExecuteUIAction( ICAD_WNDACTION_UDCHKSTAT );
				ExecuteUIAction( ICAD_WNDACTION_UDTBSTATE );
			}
			if(found->spl & IC_SYSVARFLAG_REQUIRESSCREENCHANGE) {

				int svidx=found-db_Getsysvar();

				if(DB_QBKGCOLOR==svidx && rb.resval.rint!=prbVarInfo->resval.rint) { 
					ExecuteUIAction( ICAD_WNDACTION_NEWPALETTE );
					struct resbuf mdiwindow;
					SDS_getvar(NULL,DB_QWNDLMDI,&mdiwindow,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);				// when a drawing is open. 
					if (mdiwindow.resval.rint!=0) {
						ExecuteUIAction( ICAD_WNDACTION_REDRAWALLWIN );
					}
				} else if(DB_QEXPLEVEL==svidx && rb.resval.rint!=prbVarInfo->resval.rint) { 
					ExecuteUIAction( ICAD_WNDACTION_NEWMENUS );
				} else if(DB_QUNDOCTL==svidx && rb.resval.rint!=prbVarInfo->resval.rint) { 
					ExecuteUIAction( ICAD_WNDACTION_UDCHKSTAT );
					ExecuteUIAction( ICAD_WNDACTION_UDTBSTATE );
				} else if(DB_QOSMODE==svidx && rb.resval.rint!=prbVarInfo->resval.rint) { 
					ExecuteUIAction( ICAD_WNDACTION_UDCHKSTAT );
					ExecuteUIAction( ICAD_WNDACTION_UDTBSTATE );
					ExecuteUIAction( ICAD_WNDACTION_STATCHANGE );
				} else if(DB_QGRIDMODE==svidx && rb.resval.rint!=prbVarInfo->resval.rint) { 
					if(SDS_CURDWG && SDS_CURVIEW) SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
					SDS_DrawGrid(NULL,NULL);
					ExecuteUIAction( ICAD_WNDACTION_STATCHANGE );
				} else if ( svidx EQ DB_QCURSORSIZE ) {
					IcadCursor::AppCursor().SetCrossHairSize( prbVarInfo->resval.rint);
				} else if(DB_QORTHOMODE==svidx && rb.resval.rint!=prbVarInfo->resval.rint) { 
					ExecuteUIAction( ICAD_WNDACTION_STATCHANGE );
				} else if(DB_QTABMODE==svidx && rb.resval.rint!=prbVarInfo->resval.rint) { 
					ExecuteUIAction( ICAD_WNDACTION_STATCHANGE );
				} else if(DB_QTILEMODE==svidx && rb.resval.rint!=prbVarInfo->resval.rint) { 
					cmd_InsideTilemodeCmd=TRUE;
					if(prbVarInfo->resval.rint==0) {
						SDS_CMainWindow->m_wndViewClose=SDS_CURVIEW;
						ExecuteUIAction( ICAD_WNDACTION_WCLOSEALLBUT );
						ExecuteUIAction( ICAD_WNDACTION_MDIMAX );
						cmd_pspace();
					} else {
						cvport.restype=RTSHORT;
						cvport.resval.rint=2;
						SDS_setvar(NULL,DB_QCVPORT,&cvport,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
						ExecuteUIAction( ICAD_WNDACTION_RUNFIRSTVIEW );
						cmd_CreateNewViewports(SDS_CURDWG);
						if ( SDS_CURVIEW != NULL )
							{
							SDS_TabToVar(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
							SDS_SetClipInDC(SDS_CURVIEW,SDS_CURVIEW->m_pVportTabHip,TRUE);	
							}
					}
					cmd_setUpCTAB();	// 2003-08-10 EBATECH(CNBR) Sync CTAB
					cmd_regenall();
					
					ExecuteUIAction(ICAD_WNDACTION_STATCHANGE);

					cmd_InsideTilemodeCmd=FALSE;
				} else if(DB_QSNAPMODE==svidx && rb.resval.rint!=prbVarInfo->resval.rint) { 
					ExecuteUIAction( ICAD_WNDACTION_STATCHANGE );
				} else if(DB_QCECOLOR==svidx && rb.resval.rint!=prbVarInfo->resval.rint) { 
					ExecuteUIAction( ICAD_WNDACTION_UDSTCOL );
				} else if(DB_QCELTYPE==svidx) {  
					ExecuteUIAction( ICAD_WNDACTION_UDSTLTP );
				} else if(DB_QCLAYER==svidx) {  
					ExecuteUIAction( ICAD_WNDACTION_UDSTLAY );
				// EBATECH-[add events that change TEXTSTYLE/DIMSTYLE on statusbar.
				}else if(DB_QCELWEIGHT==svidx) {  
					ExecuteUIAction( ICAD_WNDACTION_UDSTLW );
				}else if(DB_QTEXTSTYLE==svidx) {  
					ExecuteUIAction( ICAD_WNDACTION_UDSTSTY );
				} else if(DB_QDIMSTYLE==svidx) {  
					ExecuteUIAction( ICAD_WNDACTION_UDSTDIM );
				// ]-EBATECH
/*				} else if(DB_QGPSPAUSED==svidx) { 
NO GPS				if(prbVarInfo->resval.rint) {
						ret=IcadGPSSend(GPS_PAUSE,NULL);
					} else {
						ret=IcadGPSSend(GPS_RESUME,NULL);
					}
				} else if(DB_QGPSMODE==svidx) { 
					if(prbVarInfo->resval.rint) {
						ret=IcadGPSSend(GPS_ON,NULL);
					} else {
						ret=IcadGPSSend(GPS_OFF,NULL);
					}
*/				} else if(DB_QSNAPISOPAIR==svidx){    
					if(SDS_CURDWG && SDS_CURVIEW) SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
				} else if(DB_QGRIDUNIT==svidx){    
					if(SDS_CURDWG && SDS_CURVIEW) SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
					// Don't redraw the grid here it messes up the cmd_grid func.
					/* SDS_DrawGrid(NULL,NULL,NULL); */
				} else if(DB_QZOOMPERCENT==svidx){    
					if(SDS_CURDWG && SDS_CURVIEW) SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
				} else if(DB_QFASTZOOM==svidx){    
					if(SDS_CURDWG && SDS_CURVIEW) SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
				} else if(DB_QSNAPUNIT==svidx){    
					if(SDS_CURDWG && SDS_CURVIEW) SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
				} else if(DB_QSNAPBASE==svidx){    
					if(SDS_CURDWG && SDS_CURVIEW) SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
				} else if(DB_QUCSICON==svidx){    
					if(SDS_CURDWG && SDS_CURVIEW) SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
				} else if(DB_QSNAPANG==svidx){    
					if(SDS_CURDWG && SDS_CURVIEW) SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
				} else if(DB_QSNAPSTYL==svidx){    
					if(SDS_CURDWG && SDS_CURVIEW) SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
					struct resbuf gridmode;
					SDS_getvar(NULL,DB_QGRIDMODE,&gridmode,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);				// when a drawing is open. 
					if (gridmode.resval.rint) cmd_redraw();
				} else if(DB_QSCRLHIST==svidx){    
					ExecuteUIAction( ICAD_WNDACTION_UDSCRLHIST,0L);
				} else if(DB_QTBCOLOR==svidx){    
					ExecuteUIAction( ICAD_WNDACTION_UDTBARS,0L);
				} else if(DB_QTBSIZE==svidx){    
					ExecuteUIAction( ICAD_WNDACTION_UDTBARS,0L);
				} else if(DB_QTOOLTIPS==svidx){    
					ExecuteUIAction( ICAD_WNDACTION_UDTBTIPS,0L);
//				} else if(DB_QVIEWCTR==svidx){    
					// TODO Update the view BEcarful that Zoomit dosen't call this.
//				} else if(DB_QVIEWDIR==svidx){    
					// TODO Update the view BEcarful that Zoomit dosen't call this.
				} else if(DB_QVIEWMODE==svidx) {   
					ExecuteUIAction( ICAD_WNDACTION_UDCHKSTAT,0L);
					ExecuteUIAction( ICAD_WNDACTION_UDTBSTATE,0L);
//				} else if(DB_QVIEWSIZE==svidx){    
					// TODO Update the view BEcarful that Zoomit dosen't call this.
//				} else if(DB_QVIEWTWIST==svidx){    
					// TODO Update the view BEcarful that Zoomit dosen't call this.
				} else if(DB_QWNDLCMD==svidx){    
					ExecuteUIAction( ICAD_WNDACTION_UDCMDBAR,0L);
				} else if(DB_QWNDLMAIN==svidx){    
					ExecuteUIAction( ICAD_WNDACTION_UDGRASCR,0L);
				} else if(DB_QWNDLMDI==svidx){    
					ExecuteUIAction( ICAD_WNDACTION_NEWMENUS,0L);
					ExecuteUIAction( ICAD_WNDACTION_UDMDIWND,0L);
				} else if(DB_QWNDLSTAT==svidx){    
					ExecuteUIAction( ICAD_WNDACTION_UDSTATBAR,0L);
				} else if(DB_QWNDLSCRL==svidx){    
					ExecuteUIAction( ICAD_WNDACTION_UDSCRLBAR,0L);
				} else if(DB_QWNDLTEXT==svidx){    
					ExecuteUIAction( ICAD_WNDACTION_UDTXTSCR,0L);
				} else if(DB_QWNDPCMD==svidx){    
					ExecuteUIAction( ICAD_WNDACTION_UDCMDBAR,0L);
				} else if(DB_QWNDPMAIN==svidx){    
					ExecuteUIAction( ICAD_WNDACTION_UDGRASCR,0L);
				} else if(DB_QWNDPTEXT==svidx){    
					ExecuteUIAction( ICAD_WNDACTION_UDTXTSCR,0L);
				} else if(DB_QWNDSCMD==svidx) {   
					ExecuteUIAction( ICAD_WNDACTION_UDCMDBAR,0L);
				} else if(DB_QWNDSMAIN==svidx){    
					ExecuteUIAction( ICAD_WNDACTION_UDGRASCR,0L);
				} else if(DB_QWNDSTEXT==svidx){    
					ExecuteUIAction( ICAD_WNDACTION_UDTXTSCR,0L);
				} else if((DB_QUSERI1==svidx && rb.resval.rint!=prbVarInfo->resval.rint) || 
				          (DB_QUSERI2==svidx && rb.resval.rint!=prbVarInfo->resval.rint) || 
				          (DB_QUSERI3==svidx && rb.resval.rint!=prbVarInfo->resval.rint) || 
				          (DB_QUSERI4==svidx && rb.resval.rint!=prbVarInfo->resval.rint) || 
				          (DB_QUSERI5==svidx && rb.resval.rint!=prbVarInfo->resval.rint)) { 
					ExecuteUIAction( ICAD_WNDACTION_UDCHKSTAT,0L);
					ExecuteUIAction( ICAD_WNDACTION_UDTBSTATE,0L);
				} else if(DB_QATTMODE==svidx){  
					ExecuteUIAction( ICAD_WNDACTION_STATCHANGE,0L);
					if(view && SDS_CURVIEW) {
						SDS_getvar(NULL,DB_QCVPORT,&cvport,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						SDS_getvar(NULL,DB_QTILEMODE,&tilemode,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						if(tilemode.resval.rint==0 && cvport.resval.rint>1 && SDS_CURVIEW->m_pVportTabHip->ret_type()==DB_VIEWPORT) {
							SDS_VPeedToView(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip,&SDS_CURVIEW->m_pCurDwgView,SDS_CURVIEW);
						} else {
							struct gr_viewvars viewvarsp;
							SDS_ViewToVars2Use(SDS_CURVIEW,SDS_CURDWG,&viewvarsp);
							gr_initview(SDS_CURDWG,&SDS_CURCFG,&SDS_CURVIEW->m_pCurDwgView,&viewvarsp);
						}
					}
					ExecuteUIAction( ICAD_WNDACTION_UDCHKSTAT,0L);
					ExecuteUIAction( ICAD_WNDACTION_UDTBSTATE,0L);
				// gr_initview() ONLY!!!
				} else if(DB_QELEVATION==svidx || DB_QPDSIZE==svidx || DB_QSPLFRAME==svidx || DB_QPDMODE==svidx){    
					if(view && SDS_CURVIEW) {
						SDS_getvar(NULL,DB_QCVPORT,&cvport,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						SDS_getvar(NULL,DB_QTILEMODE,&tilemode,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						if(tilemode.resval.rint==0 && cvport.resval.rint>1 && SDS_CURVIEW->m_pVportTabHip->ret_type()==DB_VIEWPORT) {
							SDS_VPeedToView(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip,&SDS_CURVIEW->m_pCurDwgView,SDS_CURVIEW);
						} else {
							struct gr_viewvars viewvarsp;
							SDS_ViewToVars2Use(SDS_CURVIEW,SDS_CURDWG,&viewvarsp);
							gr_initview(SDS_CURDWG,&SDS_CURCFG,&SDS_CURVIEW->m_pCurDwgView,&viewvarsp);
						}
					}
					if(svidx == DB_QSPLFRAME)
						cmd_regenall();
				} else if(DB_QCVPORT==svidx){
					SDS_getvar(NULL,DB_QTILEMODE,&tilemode,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

					// If we're in tilemode, SDS_SetActiveVPByNum handles things
					//
					if ( tilemode.resval.rint == 1 )
						{
						if(RTERROR==SDS_SetActiveVPbyNum(prbVarInfo->resval.rint)) 
							{
							cmd_ErrorPrompt(CMD_ERR_BADVPORTNUM,0,(void *)prbVarInfo->resval.rint);
							SDS_setvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
							} 
						}
					else
						{
						if ( !cmd_SetActiveMView( SDS_CURVIEW, prbVarInfo->resval.rint, SDS_CURDWG ) )
							{
							cmd_ErrorPrompt(CMD_ERR_BADVPORTNUM,0,(void *)prbVarInfo->resval.rint);
							SDS_setvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
							}
						}
				} else if(DB_QLTSCALE==svidx){    
					if(view && SDS_CURVIEW) {
						SDS_getvar(NULL,DB_QCVPORT,&cvport,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						SDS_getvar(NULL,DB_QTILEMODE,&tilemode,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						if(tilemode.resval.rint==0 && cvport.resval.rint>1 && SDS_CURVIEW->m_pVportTabHip->ret_type()==DB_VIEWPORT) {
							SDS_VPeedToView(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip,&SDS_CURVIEW->m_pCurDwgView,SDS_CURVIEW);
						} else {
							struct gr_viewvars viewvarsp;
							SDS_ViewToVars2Use(SDS_CURVIEW,SDS_CURDWG,&viewvarsp);
							gr_initview(SDS_CURDWG,&SDS_CURCFG,&SDS_CURVIEW->m_pCurDwgView,&viewvarsp);
						}
					}
					cmd_regenall();
				} else if(DB_QDBMOD==svidx){    
					if(prbVarInfo->resval.rint) {
						ExecuteUIAction( ICAD_WNDACTION_DBMODON,0L);
					} else {
						ExecuteUIAction( ICAD_WNDACTION_DBMODOFF,0L);
					}
					ExecuteUIAction( ICAD_WNDACTION_UDCHKSTAT,0L);
					ExecuteUIAction( ICAD_WNDACTION_UDTBSTATE,0L);
				} else if(DB_QFONTMAP==svidx){    
					if(prbVarInfo->resval.rstring && prbVarInfo->resval.rstring) {
						char fs1[IC_ACADBUF];
						if(RTNORM==sds_findfile(rb.resval.rstring,fs1)) {
							db_setfontmap(fs1);
							ExecuteUIAction( ICAD_WNDACTION_NEWFONTMAP,0L);
						}
					}
				} else if(DB_QDIMTOL==svidx){    
					if(prbVarInfo->resval.rint) {
						struct resbuf rb2;
						SDS_getvar(NULL,DB_QDIMLIM,&rb2,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						if(rb2.resval.rint) {
							rb2.resval.rint=0;
							SDS_setvar(NULL,DB_QDIMLIM,&rb2,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
						}
					}
				} else if(DB_QDIMLIM==svidx){    
					if(prbVarInfo->resval.rint) {
						struct resbuf rb2;
						SDS_getvar(NULL,DB_QDIMTOL,&rb2,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						if(rb2.resval.rint) {
							rb2.resval.rint=0;
							SDS_setvar(NULL,DB_QDIMTOL,&rb2,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
						}
					}
				} else if(DB_QNFILELIST==svidx) {
					ExecuteUIAction( ICAD_WNDACTION_SETMRUSIZE,0L);
					ExecuteUIAction( ICAD_WNDACTION_NEWMENUS,0L);
				// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
				} else if( DB_QLWDISPLAY==svidx || DB_QLWDEFAULT==svidx ){
					if(rb.resval.rint!=prbVarInfo->resval.rint){
						ExecuteUIAction( ICAD_WNDACTION_UDSTLW );
						ExecuteUIAction( ICAD_WNDACTION_STATCHANGE );
						cmd_regenall();
					}
				} else if(DB_QCELWEIGHT==svidx || DB_QLWUNITS==svidx ){
					if( rb.resval.rint!=prbVarInfo->resval.rint) { 
						ExecuteUIAction( ICAD_WNDACTION_UDSTLW );
					}
				} else if( DB_QCTAB==svidx ) {
					CLayoutManager lm(SDS_CURDWG);
					lm.setCurrentLayout(prbVarInfo->resval.rstring);
					cmd_regenall();
					ExecuteUIAction( ICAD_WNDACTION_STATCHANGE );
				}
			}
			//Added Cybage AJK 10/12/2001 [
			//Reason : Fix for bug no 77939 
			/* The second condtion in if has been added to cater for the variables 
			having 7th bit of spl set,it should be removed once implementation of
			saving them in drawing is given.*/
			if(found->loc ==0 && (found->spl&128)!=128) 
				SDS_DBModified(IC_DBMOD_DATABASEVBL_MODIFIED);
			//Added Cybage AJK 10/12/2001 DD/MM/YYYY ]
			// Notify apps after successful sysvar changing.
			SDS_CallUserCallbackFunc(SDS_CBVARCHANGE, static_cast<void*>(found->name), NULL, NULL);
		}
		if( rb.restype == RTSTR ) {
			IC_FREE(rb.resval.rstring);
		}
        return(RTNORM);
    }
    if(rb.restype==RTSTR ) {
		IC_FREE(rb.resval.rstring);
	}
    return(RTERROR);
}

