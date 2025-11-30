/* D:\ICADDEV\PRJ\LIB\CMDS\VIEWPORTS.CPP
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
#include "IcadDoc.h"
#include "IcadChildWnd.h"
#include "IcadView.h"
#include "vporttabrec.h"

#define	X	SDS_X
#define	Y	SDS_Y

BOOL  cmd_InsideViewPorts,cmd_CalcChildRect;
bool  cmd_DontCalcViewSize;

bool cmd_viewport_save_configuration (char * vp_to_save, db_drawing *flp)
	{
	cmd_InsideViewPorts=TRUE;

	db_handitem *hip;
    RT	      ret=RTNORM;		// Return value
	//set all vports with this name to deleted, since we will be recreating them
	for (hip=flp->tblnext(DB_VPORTTABREC,1); hip!=NULL; hip=hip->next) 
		{
		if(hip->ret_deleted()) continue;
					
		char *fcp1;
		hip->get_2(&fcp1);
		if(strisame(fcp1,vp_to_save)) 
			hip->set_deleted(1);
				
		}

	sds_point ll,ur,snapsp,gridsp,snapbase,target,targ2cam,center;
	double snaprot,height,twist,fclip,bclip,asp,lenslen;
	int flags,czoom,fzoom,ucsicon,snapstyle,isopair,id,snap,grid,viewmd;
	for (hip=flp->tblnext(DB_VPORTTABREC,1); hip!=NULL; hip=hip->next) 
		{
		char *fcp1;
		hip->get_2(&fcp1);
		if(hip->ret_deleted() || !strisame(fcp1,"*ACTIVE"/*DNT*/))
			continue;
		hip->get_70(&flags);
		hip->get_10(ll);
		hip->get_11(ur);
		hip->get_17(target);   
		hip->get_16(targ2cam);
		hip->get_51(&twist);
		hip->get_40(&height);
		hip->get_12(center);
		hip->get_41(&asp);      			  
		hip->get_42(&lenslen);
		hip->get_43(&fclip);
		hip->get_44(&bclip);
		hip->get_69(&id);
		hip->get_71(&viewmd);   
		hip->get_72(&czoom);
		hip->get_73(&fzoom);
		hip->get_74(&ucsicon);
		hip->get_75(&snap);
		hip->get_76(&grid);     
		hip->get_77(&snapstyle);
		hip->get_78(&isopair);
		hip->get_50(&snaprot);
		hip->get_13(snapbase); 
		hip->get_14(snapsp);
		hip->get_15(gridsp);    
		struct resbuf *trb=sds_buildlist(RTDXF0,"VPORT"/*DNT*/,
						  70,flags,2,vp_to_save,10,ll,11,ur,17,target,   
						  16,targ2cam,51,twist,40,height,12,center,41,asp,      			  
						  42,lenslen,43,fclip,44,bclip,69,id,71,viewmd,   
						  72,czoom,73,fzoom,74,ucsicon,75,snap,76,grid,     
						  77,snapstyle,78,isopair,50,snaprot,13,snapbase, 
						  14,snapsp,15,gridsp,0);    
		ret=sds_entmake(trb);

		db_handitem *orighip;
		orighip = (db_handitem *) SDS_LastHandItem; //nmLastEnt[0];
		((db_vporttabrec *)hip)->set_original_viewport (orighip);
		
		sds_relrb(trb);	
		}	
	return true;
	}

bool cmd_viewport_restore_configuration (char * vp_to_restore, db_drawing *flp)
	{
	cmd_InsideViewPorts=TRUE;
	// Count how many we have to restore.
	int count=0;
	db_handitem *hip=NULL;
	db_handitem *orighip=NULL;
	for (hip=flp->tblnext(DB_VPORTTABREC,1); hip!=NULL; hip=hip->next) {
		if(hip->ret_deleted()) continue;
		char *fcp1;
		hip->get_2(&fcp1);
		if(strisame(fcp1,vp_to_restore)) count++;
	}
	if(count<=0) {
		cmd_ErrorPrompt(CMD_ERR_NOVPCONFIG,0);
		return false;
	}

	CIcadView *pView=SDS_CURVIEW;
	if(!pView || SDS_CMainWindow->m_pCurFrame->IsZoomed()) {
		SDS_SetUndo(IC_UNDO_MDI_RESTORE,NULL,NULL,NULL,SDS_CURDWG);
		ExecuteUIAction( ICAD_WNDACTION_MDIRESTORE );
	}

	SDS_CMainWindow->m_wndViewClose=pView;
	ExecuteUIAction( ICAD_WNDACTION_WCLOSEALLBUT );

	for (hip=flp->tblnext(DB_VPORTTABREC,1); hip!=NULL; hip=hip->next) {
		if(hip->ret_deleted()) continue;
		char *fcp1;
		hip->get_2(&fcp1);
		if(strisame(fcp1,"*ACTIVE"/*DNT*/)) {
			hip->set_deleted(1);
			continue;
		} 
	}

	for (hip=flp->tblnext(DB_VPORTTABREC,1); count>0 && hip!=NULL; hip=hip->next) {
		if(hip->ret_deleted()) continue;
		char *fcp1;
		sds_point ll,ur;
		hip->get_2(&fcp1);
		if(strisame(fcp1,vp_to_restore)) {
			hip->get_10(ll);
			hip->get_11(ur);
			struct resbuf *trb=hip->entgetx(NULL,flp);
			SDS_entmake(trb,flp);
			sds_relrb(trb);
			hip->set_2("*ACTIVE"/*DNT*/);
			cmd_makewindowport(ll,ur,hip);
			count--;

			//sds_entlast (nmLastEnt);
			orighip = (db_handitem *) SDS_LastHandItem; //nmLastEnt[0];
			((db_vporttabrec *)hip)->set_original_viewport (orighip);
		}
	}

	SDS_CMainWindow->m_wndViewClose=pView;
	ExecuteUIAction( ICAD_WNDACTION_CLOSEVIEW );

	cmd_DontCalcViewSize=FALSE;
	return true;
}

//This is a helper function that actually splits a given view
void cmd_viewport_splitter (CIcadView *pView, int nHowMany, int nOrientation)
	{			   
	sds_point ptViewLL, ptViewUR, ptBoundingLL, ptBoundingUR;
	
	cmd_InsideViewPorts=TRUE;
	cmd_CalcChildRect=TRUE;
	if(!pView || SDS_CMainWindow->m_pCurFrame->IsZoomed() || SDS_CURDOC->m_nOpenViews==1) 
		{
		SDS_SetUndo(IC_UNDO_MDI_RESTORE,NULL,NULL,NULL,SDS_CURDWG);
		ExecuteUIAction( ICAD_WNDACTION_MDIRESTORE );
		ptBoundingLL[0]=0.0;  ptBoundingLL[1]=0.0;  ptBoundingLL[2]=0.0;
		ptBoundingUR[0]=1.0;  ptBoundingUR[1]=1.0;  ptBoundingUR[2]=0.0;
		}
	else 
		{
		ASSERT( pView!=NULL );
		ExecuteUIAction( ICAD_WNDACTION_CALCVIEWPTS );
		// Make sure that pView->m_pVportTabHip is set, this has failed
		if (pView->m_pVportTabHip!=NULL)
			{
			pView->m_pVportTabHip->get_10(ptBoundingLL);
			pView->m_pVportTabHip->get_11(ptBoundingUR);
			}
		else
			{
			// Throw an assert, it's not clear why this would be NULL
			ASSERT( pView->m_pVportTabHip==NULL );

			ptBoundingLL[0]=0.0;  ptBoundingLL[1]=0.0;  ptBoundingLL[2]=0.0;
			ptBoundingUR[0]=1.0;  ptBoundingUR[1]=1.0;  ptBoundingUR[2]=0.0;
			}
		}
	cmd_CalcChildRect=FALSE;
	
	switch(nHowMany) 
		{
		case 2:
			if (nOrientation==1) 
				{														// ...Horizonally
				ptViewLL[X]=ptBoundingLL[X];											//    Upper Viewport
				ptViewLL[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
				cmd_makewindowport(ptViewLL,ptBoundingUR,NULL);
				cmd_DontCalcViewSize=TRUE;
				ptViewUR[X]=ptBoundingUR[X];											//    Lower Viewport
				ptViewUR[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
				cmd_makewindowport(ptBoundingLL,ptViewUR,NULL);
				} 
			else 
				{																	// ...Vertically
				ptViewUR[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Left Viewport
				ptViewUR[Y]=ptBoundingUR[Y];
				cmd_makewindowport(ptBoundingLL,ptViewUR,NULL);
				cmd_DontCalcViewSize=TRUE;
				ptViewLL[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Right Viewport
				ptViewLL[Y]=ptBoundingLL[Y];
				cmd_makewindowport(ptViewLL,ptBoundingUR,NULL);
				}
			break;
		case 3:
			if (nOrientation==1) 
				{														// ...Horizonally
				ptViewLL[X]=ptBoundingLL[X];											//    Upper Viewport
				ptViewLL[Y]=ptBoundingLL[Y]+2*(((ptBoundingUR[Y]-ptBoundingLL[Y])/3));
				cmd_makewindowport(ptViewLL,ptBoundingUR,NULL);
				cmd_DontCalcViewSize=TRUE;
				ptViewLL[X]=ptBoundingLL[X];											//    Middle Viewport
				ptViewLL[Y]=ptBoundingLL[Y]+((ptBoundingUR[Y]-ptBoundingLL[Y])/3);
				ptViewUR[X]=ptBoundingUR[X];											//    Middle Viewport
				ptViewUR[Y]=ptBoundingLL[Y]+2*(((ptBoundingUR[Y]-ptBoundingLL[Y])/3));
				cmd_makewindowport(ptViewLL,ptViewUR,NULL);
				ptViewUR[X]=ptBoundingUR[X];											//    Lower Viewport
				ptViewUR[Y]=ptBoundingLL[Y]+((ptBoundingUR[Y]-ptBoundingLL[Y])/3);
				cmd_makewindowport(ptBoundingLL,ptViewUR,NULL);
				} 
			else if (nOrientation==2) 
				{												// ...Vertically
				ptViewUR[X]=ptBoundingLL[X]+((ptBoundingUR[X]-ptBoundingLL[X])/3);		//    Left Viewport
				ptViewUR[Y]=ptBoundingUR[Y];
				cmd_makewindowport(ptBoundingLL,ptViewUR,NULL);
				cmd_DontCalcViewSize=TRUE;
				ptViewLL[X]=ptBoundingLL[X]+((ptBoundingUR[X]-ptBoundingLL[X])/3);		//    Center Viewport
				ptViewLL[Y]=ptBoundingLL[Y];
				ptViewUR[X]=ptBoundingLL[X]+2*(((ptBoundingUR[X]-ptBoundingLL[X])/3));
				ptViewUR[Y]=ptBoundingUR[Y];
				cmd_makewindowport(ptViewLL,ptViewUR,NULL);
				ptViewLL[X]=ptBoundingLL[X]+2*(((ptBoundingUR[X]-ptBoundingLL[X])/3));	//    Right Viewport
				ptViewLL[Y]=ptBoundingLL[Y];
				cmd_makewindowport(ptViewLL,ptBoundingUR,NULL);
				} 
			else if (nOrientation==3) 
				{												// ...Large Above 2 small ones
				ptViewLL[X]=ptBoundingLL[X];											//    Upper Viewport (large one)
				ptViewLL[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
				cmd_makewindowport(ptViewLL,ptBoundingUR,NULL);
				ptViewUR[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Lower-Left Viewport
				ptViewUR[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
				cmd_makewindowport(ptBoundingLL,ptViewUR,NULL);
				cmd_DontCalcViewSize=TRUE;
				ptViewLL[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Lower-Right Viewport
				ptViewLL[Y]=ptBoundingLL[Y];
				ptViewUR[X]=ptBoundingUR[X];
				ptViewUR[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
				cmd_makewindowport(ptViewLL,ptViewUR,NULL);
				}
			else if (nOrientation==4) 
				{												// ...Large Below 2 small ones
				ptViewLL[X]=ptBoundingLL[X];											//    Upper-Left Viewport
				ptViewLL[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
				ptViewUR[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);
				ptViewUR[Y]=ptBoundingUR[Y];
				cmd_makewindowport(ptViewLL,ptViewUR,NULL);
				ptViewLL[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Upper-Right Viewport
				ptViewLL[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
				cmd_DontCalcViewSize=TRUE;
				cmd_makewindowport(ptViewLL,ptBoundingUR,NULL);
				cmd_DontCalcViewSize=FALSE;
				ptViewUR[X]=ptBoundingUR[X];											//    Lower Viewport (large one)
				ptViewUR[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
				cmd_makewindowport(ptBoundingLL,ptViewUR,NULL);
				} 
			else if (nOrientation==5) 
				{												// ...Large to left of small ones
				ptViewUR[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Left Viewport (large one)
				ptViewUR[Y]=ptBoundingUR[Y];
				cmd_makewindowport(ptBoundingLL,ptViewUR,NULL);
				ptViewLL[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Upper-Right Viewport
				ptViewLL[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
				cmd_makewindowport(ptViewLL,ptBoundingUR,NULL);
				cmd_DontCalcViewSize=TRUE;
				ptViewLL[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Lower-Right Viewport
				ptViewLL[Y]=ptBoundingLL[Y];
				ptViewUR[X]=ptBoundingUR[X];
				ptViewUR[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
				cmd_makewindowport(ptViewLL,ptViewUR,NULL);
				} 
			else if (nOrientation==6) 
				{												// ...Large to right of small ones
				ptViewLL[X]=ptBoundingLL[X];											//    Upper-Left Viewport
				ptViewLL[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
				ptViewUR[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);
				ptViewUR[Y]=ptBoundingUR[Y];
				cmd_makewindowport(ptViewLL,ptViewUR,NULL);
				ptViewUR[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Lower-Left Viewport
				ptViewUR[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
				cmd_DontCalcViewSize=TRUE;
				cmd_makewindowport(ptBoundingLL,ptViewUR,NULL);
				cmd_DontCalcViewSize=FALSE;
				ptViewLL[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Right Viewport (large one)
				ptViewLL[Y]=ptBoundingLL[Y];
				cmd_makewindowport(ptViewLL,ptBoundingUR,NULL);
				}
			break;
		case 4:
			ptViewLL[X]=ptBoundingLL[X];											//    Upper-Left Viewport
			ptViewLL[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			ptViewUR[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);
			ptViewUR[Y]=ptBoundingUR[Y];
			cmd_makewindowport(ptViewLL,ptViewUR,NULL);
			cmd_DontCalcViewSize=TRUE;
			ptViewLL[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Upper-Right Viewport
			ptViewLL[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			cmd_makewindowport(ptViewLL,ptBoundingUR,NULL);
			ptViewUR[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Lower-Left Viewport
			ptViewUR[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			cmd_makewindowport(ptBoundingLL,ptViewUR,NULL);
			ptViewLL[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Lower-Right Viewport
			ptViewLL[Y]=ptBoundingLL[Y];
			ptViewUR[X]=ptBoundingUR[X];
			ptViewUR[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			cmd_makewindowport(ptViewLL,ptViewUR,NULL);
			break;
	}
	// Now delete the original viewport.
	if(pView) 
		{
		SDS_CMainWindow->m_wndViewClose=pView;
		ExecuteUIAction( ICAD_WNDACTION_CLOSEVIEW );
		}
	cmd_DontCalcViewSize=FALSE;
}

void FindCorrespondingActiveVport (db_handitem *findhip, db_drawing *flp, db_handitem **activevp)
	{
	db_handitem *hip = NULL;
	for (hip=flp->tblnext(DB_VPORTTABREC,1); hip!=NULL; hip=hip->next) 
		{
		if(hip->ret_deleted()) 
			continue;

		char *fcp1;
		db_handitem *orighip = NULL;

		hip->get_2(&fcp1);
		if(strisame(fcp1,"*ACTIVE")) 
			{
			((db_vporttabrec *)hip)->get_original_viewport (&orighip);
			if (findhip == orighip)
				{
				*activevp = orighip;
				break;
				}
			}
		}

	*activevp = NULL;
	}

void ClearAllActiveVports (db_drawing *flp)
	{
	char *fcp1;
	db_handitem *hip = NULL;
	for (hip=flp->tblnext(DB_VPORTTABREC,1); hip!=NULL; hip=hip->next) 
		{
		if(hip->ret_deleted()) 
			continue;

		hip->get_2(&fcp1);
		if(strisame(fcp1,"*ACTIVE")) 
			((db_vporttabrec *)hip)->set_original_viewport (NULL);
		}
	}

short cmd_viewports(void) { 

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_istransparent() == RTERROR) return RTERROR;
	
    struct resbuf rb;
	if(SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	if(rb.resval.rint==0) {
		cmd_ErrorPrompt(CMD_ERR_TILEMODE1,0);
		return(RTERROR);
	}

    int       nHowMany=1;		// For remembering how many viewports the user wanted
    int       nOrientation=1;	// For remembering which way they are arranged
								// 1=Horiz, 2=Vert, 3=Above, 4=Below, 5=Left, 6=Right
    RT	      ret=RTNORM;		// Return value
	char      szCmdLine[IC_ACADBUF];		// Command line prompts and responses
	sds_point ptBoundingLL,ptBoundingUR;

	CIcadView *pView=SDS_CURVIEW;
	if(pView==NULL) return(RTERROR);

	db_drawing *flp=SDS_CURDWG;
	if(flp==NULL) return(RTERROR);

	cmd_InsideViewPorts=TRUE;

	sds_initget(0,ResourceString(IDC_VIEWPORTS_INITGET_LIST_S_0, "List_saved_viewports|? ~ Save Restore Delete Join Single|SIngle ~ Create_2_viewports|2 Create_3_viewports|3 Create_4_viewports|4 ~_? ~_ ~_Save ~_Restore ~_Delete ~_Join ~_SIngle ~_ ~_2 ~_3 ~_4" ));
	if((ret=(sds_getkword(ResourceString(IDC_VIEWPORTS__NVIEWPORTS____1, "\nViewports:  ? to list/Save/Restore/Delete/SIngle/Join/2/3/4/<3> : " ),szCmdLine))) == RTERROR) 
        goto out;
    if (RTCAN==ret) 
        goto out;
	if (strisame(szCmdLine,"SAVE"/*DNT*/)) {
		char fs1[IC_ACADBUF];
		char fs2[IC_ACADBUF];
		while(1) {
			RT ret=sds_getstring(1,ResourceString(IDC_VIEWPORTS__NVIEWPORT_CON_2, "\nViewport configuration name to save: " ),fs1);
			if(ret!=RTNORM) goto out;

			//if there are spaces in the start and in the end of viewport name, remove them.
			ic_trim((LPTSTR)fs1, " "/*DNT*/);

			if(strisame(fs1, "*ACTIVE"/*DNT*/)) 
				goto out;

			if ( !strlen(fs1) || !ic_isvalidname(fs1))
			{
				cmd_ErrorPrompt(CMD_ERR_INVALIDNAME,-1);
				continue;
			}

			if(flp->findtabrec(DB_VPORTTABREC,fs1,1)) {
				if(sds_initget(0,ResourceString(IDC_VIEWPORTS_INITGET_YES_NO_3, "Yes No ~_Yes ~_No" ))!=RTNORM) 
                    return(RTERROR);
				if(RTCAN==(ret=sds_getkword(ResourceString(IDC_VIEWPORTS__NA_VIEWPORT_C_4, "\nA viewport configuration with that name exists, Overwrite? (Yes/No)? <No>: " ),fs2))) 
                    return(ret);
				if((ret!=RTNORM) || strisame("NO"/*DNT*/,fs2)) 
                    continue;

				break;
			} else break;
		}

		cmd_viewport_save_configuration (fs1, flp);

		goto out;
	} else if (strisame("RESTORE"/*DNT*/,szCmdLine)) {
		char fs1[IC_ACADBUF];
		RT ret=sds_getstring(1,ResourceString(IDC_VIEWPORTS__NVIEWPORT_CON_5, "\nViewport configuration name to restore: " ),fs1);
		if(ret!=RTNORM) goto out;

		cmd_viewport_restore_configuration (fs1, flp);

		goto out;
	} else if (strisame("DELETE"/*DNT*/,szCmdLine)) {
		char fs1[IC_ACADBUF];
		RT ret=sds_getstring(1,ResourceString(IDC_VIEWPORTS__NVIEWPORT_CON_6, "\nViewport configuration name to delete: " ),fs1);
		if(ret!=RTNORM) goto out;
		
		// Count how many we have to restore.
		int count=0;
		db_handitem *hip=NULL;
		for (hip=flp->tblnext(DB_VPORTTABREC,1); hip!=NULL; hip=hip->next) {
			if(hip->ret_deleted()) continue;
			char *fcp1;
			hip->get_2(&fcp1);
			if(strisame(fcp1,fs1)) {
				sds_name ename;
				ename[0]=(long)hip;
				ename[1]=(long)SDS_CURDWG;
				sds_entdel(ename);
				count++;

				db_handitem *activevp = NULL;
				FindCorrespondingActiveVport (hip, flp, &activevp);
				if (activevp)
					((db_vporttabrec *)activevp)->set_original_viewport (NULL);
			}
		}
		if(count<=0) {
			cmd_ErrorPrompt(CMD_ERR_NOVPCONFIG,0);
			goto out;
		}
		goto out;

	} else if (strsame(szCmdLine,"?"/*DNT*/) || strsame(szCmdLine,"_?"/*DNT*/)) {
		sds_textscr();
		sds_printf(ResourceString(IDC_VIEWPORTS__NVIEWPORT_NAM_7, "\nViewport Name                      LL Corner              UR Corner" ));
		sds_printf(ResourceString(IDC_VIEWPORTS__N_____________8, "\n-------------------------------------------------------------------------" ));
		for (db_handitem *hip=flp->tblnext(DB_VPORTTABREC,1); hip!=NULL; hip=flp->tblnext(DB_VPORTTABREC,0)) {
			if(hip->ret_deleted()) continue;
			char *fcp1;
			sds_point ll,ur;
			hip->get_2(&fcp1);
			hip->get_10(ll);
			hip->get_11(ur);
			sds_printf(ResourceString(IDC_VIEWPORTS__N__31S___F__F_9, "\n%-31s  %f,%f   %f,%f" ),fcp1,ll[0],ll[1],ur[0],ur[1]);
		}
		goto out;
	} else if (strisame(szCmdLine,"SINGLE"/*DNT*/)) {
		ptBoundingLL[0]=0.0;  ptBoundingLL[1]=0.0;  ptBoundingLL[2]=0.0;
		ptBoundingUR[0]=1.0;  ptBoundingUR[1]=1.0;  ptBoundingUR[2]=0.0; 
		cmd_makewindowport(ptBoundingLL,ptBoundingUR,NULL);
		SDS_CMainWindow->m_wndViewClose=SDS_CURVIEW;
		ExecuteUIAction( ICAD_WNDACTION_WCLOSEALLBUT );
		ClearAllActiveVports(flp);
		goto out;
	} else if (strisame("JOIN"/*DNT*/,szCmdLine)) {
		sds_point pt;
		ret=internalGetpoint(NULL,ResourceString(IDC_VIEWPORTS__NSELECT_INSI_10, "\nSelect inside dominant viewport <Current>:" ),pt);
		CIcadView *pView1=SDS_CURVIEW;
		while(1) 
			{
			ret=internalGetpoint(NULL,ResourceString(IDC_VIEWPORTS__NSELECT_INSI_11, "\nSelect inside viewport to join:" ),pt);
			if (ret!=RTNORM)
				goto out;
			CIcadView *pView2=SDS_CURVIEW;
			if(pView1!=pView2)
				{
				CRect rect1, rect2, crect;
				CWnd *pView1Par = (pView1->GetOwner())->GetOwner();
				CWnd *pView2Par = (pView2->GetOwner())->GetOwner();
				pView1Par->GetWindowRect (&rect1);
				pView2Par->GetWindowRect (&rect2);

				if ((rect1.bottom == rect2.bottom) && (rect1.top == rect2.top) ||
					(rect1.left == rect2.left) && (rect1.right == rect2.right)	)
					{
					crect.UnionRect (rect1, rect2);
					}
				else
					{
					sds_printf (ResourceString(IDC_VIEWPORTS_JOIN_ERROR,"\nThe selected viewports do not form a rectangle."));
					continue;
					}

				(pView1Par->GetOwner())->ScreenToClient(crect);
				pView1Par->MoveWindow(crect.left, crect.top, crect.Width(), crect.Height());

				ExecuteUIAction( ICAD_WNDACTION_WCLOSE );
				SDS_CMainWindow->m_pCurView=pView1;
				ClearAllActiveVports(flp);
				goto out;
				}
			else
				continue;
			}
	} else if (strisame("2"/*DNT*/,szCmdLine)) {
		nHowMany=2;
		if(sds_initget(0,ResourceString(IDC_VIEWPORTS_INITGET_HORIZ_12, "Horizontal Vertical ~_Horizontal ~_Vertical" ))!=RTNORM) { 
			ret=RTERROR; 
			goto out; 
		}
		ret=sds_getkword(ResourceString(IDC_VIEWPORTS__NTWO_VIEWPOR_13, "\nTwo viewports:  Horizontal/<Vertical>: " ),szCmdLine);
		if (ret==RTERROR || ret==RTCAN) 
            goto out;			// Allow RTNONE to select default (vertical)
        strupr(szCmdLine);
		if (strisame("HORIZONTAL"/*DNT*/,szCmdLine)){ 
			nOrientation=1;
		} else if ((strisame("VERTICAL"/*DNT*/,szCmdLine)) || (ret==RTNONE)) {
			nOrientation=2;	
		}					
	} else if ((strsame(szCmdLine,"3"/*DNT*/)) || (RTNONE==ret)) {
		nHowMany=3;
		if(sds_initget(0,ResourceString(IDC_VIEWPORTS_INITGET_HORIZ_14, "Horizontal Vertical ~ Above Below ~ Left Right ~_Horizontal ~_Vertical ~_ ~_Above ~_Below ~_ ~_Left ~_Right" ))!=RTNORM) { 
            ret=RTERROR; 
            goto out; 
        }
		ret=sds_getkword(ResourceString(IDC_VIEWPORTS__NTHREE_VIEWP_15, "\nThree viewports:  Horizontal/Vertical/Above/Below/Left/<Right>: " ),szCmdLine);
        strupr(szCmdLine);

		if(ret==RTERROR || ret==RTCAN) 
            goto out;			// Allow RTNONE to select default (right)
		if (strisame("HORIZONTAL"/*DNT*/,szCmdLine)) {
			nOrientation=1;
		} else if (strisame("VERTICAL"/*DNT*/,szCmdLine)) {
			nOrientation=2;
		} else if (strisame("ABOVE"/*DNT*/,szCmdLine)) {
			nOrientation=3;
		} else if (strisame("BELOW"/*DNT*/,szCmdLine)) {
			nOrientation=4;
		} else if (strisame("LEFT"/*DNT*/,szCmdLine)) {
			nOrientation=5;
		} else if ((strisame("RIGHT"/*DNT*/,szCmdLine))||(ret==RTNONE)) {
			nOrientation=6;
		}				
	} else if (strisame("4"/*DNT*/,szCmdLine)) {
		nHowMany=4;		
	}

	pView=SDS_CURVIEW;
	if(pView==NULL) return(RTERROR);

	//The code in cmd_viewportsplitter used to be inline here.  Separated the UI from the actual work for Automation AM 6/9/99
	cmd_viewport_splitter (pView, nHowMany, nOrientation);

	out: ;
	cmd_DontCalcViewSize=FALSE;
    return(ret); 
}



