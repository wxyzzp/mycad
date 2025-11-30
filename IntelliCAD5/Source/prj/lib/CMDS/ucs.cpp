/* F:\DEV\PRJ\LIB\CMDS\UCS.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * UCS command
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "IcadView.h"/*DNT*/
#include "IcadDoc.h"/*DNT*/
#include "IcadTextScrn.h"/*DNT*/
#include "UCSIcon.h"

#if defined(SDS_NEWDB)
	#define gra_displayobject gr_displayobject
	#define gra_view      gr_view
	#define dwg_entlink	  db_handitem
	#define dwg_filelink  db_drawing
	#define gra_rp2ucs    gr_rp2ucs
	#define gra_ucs2rp    gr_ucs2rp
	#define gra_rp2pix    gr_rp2pix
	#define gra_pix2rp    gr_pix2rp
	#define gra_twist4ucs gr_twist4ucs
	#define gra_initview  gr_initview
#endif

// PROTO
long  cmd_ucsorigin(void); 
long  cmd_ucszaxis(const sds_point pp1,const sds_point pp2);    
long  cmd_ucs3point(void);  
long  cmd_ucsxyz(short xyzmode); 
long  cmd_ucs_objectdef(void);
long  cmd_ucssave(short delete_mode);
int	  cmd_ucs_add2ll(void);
void  cmd_ucs_pt2str(sds_point pt, char *str);


short cmd_ucsicon(void)   { 

	//this cmd has tentative code for UCSICONPOS, which will control which
	//	corner the ucs icon is painted in. Unfortunately, it's currently a config var
	//	and not drawing-specific, so clicking betw dwgs will cause a clash.  I'll
	//	comment out location code until this is resolved  12/8/97
    if (cmd_iswindowopen() == RTERROR) return RTERROR;			

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char fs1[IC_ACADBUF],fs2[IC_ACADBUF];
    sds_point pt1={0.0,0.0,0.0},ptcrt={0.0,0.0,0.0};
    struct resbuf setgetrb;
    int allmode=0;
    RT	ret;
	int	startstate,startpos,newpos,newstate;

    setgetrb.resval.rstring=NULL;
    setgetrb.rbnext=NULL;

    if (SDS_getvar(NULL,DB_QUCSICONPOS,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { return(RTERROR); }
	startpos=newpos=setgetrb.resval.rint;
    if (SDS_getvar(NULL,DB_QUCSICON,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { return(RTERROR); }
    startstate=newstate=setgetrb.resval.rint;

    for ( ;; ) {
        if (allmode) {
            if (sds_initget(0, ResourceString(IDC_UCS_UCS_ICON_ON_ON_UCS_I_0, "UCS_icon_on|ON UCS_icon_off|OFf ~ Display_at_origin|ORigin Display_in_corner|Corner ~Location ~Noorigin ~_ON ~_OFf ~_ ~_ORigin ~_Corner ~_Location ~_Noorigin")) != RTNORM)
                return(-2);
        } else {
            if (sds_initget(0, ResourceString(IDC_UCS_UCS_ICON_ON_ON_UCS_I_1, "UCS_icon_on|ON UCS_icon_off|OFf ~ Display_in_all_views|All ~ Display_at_origin|ORigin Display_in_corner|Corner ~Location ~Noorigin ~_ON ~_OFf ~_ ~_All ~_ ~_ORigin ~_Corner ~_Location ~_Noorigin" )) != RTNORM)
                return(-2);
        }

       if (newstate&1)
           strncpy(fs2, ResourceString(IDC_UCS_ON_2, "ON") ,sizeof(fs2)-1);
       else 
           strncpy(fs2, ResourceString(IDC_UCS_OFF_3, "OFF")  ,sizeof(fs2)-1);

        if (allmode) {
            sprintf(fs1,ResourceString(IDC_UCS__NON_OFF_ORIGIN_CORN_4, "\nON/OFF/ORigin/Corner/<%s>: " ), fs2);
        } else {
            sprintf(fs1,ResourceString(IDC_UCS__NON_OFF_ALL_ORIGIN__5, "\nON/OFF/All/ORigin/Corner/<%s>: " ), fs2);
        }

        if ((ret=sds_getkword(fs1,fs2))==RTERROR) {
            return(ret);
        } else if (ret==RTCAN) {
            return(ret);
        } else if (ret==RTNONE) {
            break;
        }
       if (strisame("LOCATION"/*DNT*/  ,fs2)){
           sds_initget(0,ResourceString(IDC_UCS_INITGET_LOWER_LEFT_L_7, "Lower_Left|LL Lower_Right|LR Upper_Left|UL Upper_Right|UR ~_LL ~_LR ~_UL ~_UR" ) );
			switch(newpos){
               case 0: strncpy(fs1,ResourceString(IDC_UCS__NNON_ORIGIN_LOCATIO_8, "\nNon-origin location of UCS Icon: LL/LR/UL/UR/<LR>: " ) ,sizeof(fs1)-1);
               case 1: strncpy(fs1,ResourceString(IDC_UCS__NNON_ORIGIN_LOCATIO_9, "\nNon-origin location of UCS Icon: LL/LR/UL/UR/<LL>: " ) ,sizeof(fs1)-1);
               case 2: strncpy(fs1,ResourceString(IDC_UCS__NNON_ORIGIN_LOCATI_10, "\nNon-origin location of UCS Icon: LL/LR/UL/UR/<UR>: " ) ,sizeof(fs1)-1);
               case 3: strncpy(fs1,ResourceString(IDC_UCS__NNON_ORIGIN_LOCATI_11, "\nNon-origin location of UCS Icon: LL/LR/UL/UR/<UL>: " ) ,sizeof(fs1)-1);
			}
			if ((ret=sds_getkword(fs1,fs2))==RTERROR) {
				return(ret);
			} else if (ret==RTCAN) {
				return(ret);
			} else if (ret==RTNONE) {
				continue;
			}
                if (strisame(fs2,"LR"/*DNT*/  ))newpos=0;
           else if (strisame(fs2,"LL"/*DNT*/  ))newpos=1;
           else if (strisame(fs2,"UR"/*DNT*/  ))newpos=2;
			else newpos=3;
			continue;			
       } else if (strisame("ON"/*DNT*/  ,fs2)) {
            newstate|=1;
            break;
        } else if (strisame("OFF"/*DNT*/  ,fs2)) {
            newstate&=(~1);
            break;
        } else if (strisame("ALL"/*DNT*/  ,fs2)) {
            allmode=1; continue;
        } else if ((strisame("NOORIGIN"/*DNT*/  ,fs2)) || (strisame("CORNER"/*DNT*/  ,fs2))) {
            newstate&=(~2);
			newstate|=1;
            break;
        } else if (strisame("ORIGIN"/*DNT*/  ,fs2)) {
            newstate|=3;                              
            break;
        } 
		cmd_ErrorPrompt(CMD_ERR_TRYAGAIN,0);
        continue;
    }

	if (SDS_CURDWG==NULL || SDS_CURVIEW==NULL) return(RTERROR);
	if (newstate!=startstate){
		setgetrb.resval.rint=newstate;
		setgetrb.restype=RTSHORT;
		if (allmode) {

			CIcadView *pView;
			CIcadDoc  *pDoc=SDS_CURDOC;
			POSITION pos=pDoc->GetFirstViewPosition();
			while (pos!=NULL) {//paint 'em all out
				pView=(CIcadView *)pDoc->GetNextView(pos);
				ASSERT_KINDOF(CIcadView, pView);
				SDS_DrawUcsIcon(NULL,NULL,pView,1);
			}
			SDS_SetUndo(IC_UNDO_REDRAW_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
			sds_setvar("UCSICON"/*DNT*/,&setgetrb);
			SDS_SetUndo(IC_UNDO_REDRAW_ON_REDO,NULL,NULL,NULL,SDS_CURDWG);
			if (setgetrb.resval.rint&1){
				POSITION pos=pDoc->GetFirstViewPosition();
				while (pos!=NULL) {
					pView=(CIcadView *)pDoc->GetNextView(pos);//set the view
					ASSERT_KINDOF(CIcadView, pView);
					SDS_VarToTab(SDS_CURDWG,pView->m_pVportTabHip);//update the var
					SDS_DrawUcsIcon(NULL,NULL,pView,0);//redraw the icon
				}
			}
		} else {
			SDS_DrawUcsIcon(NULL,NULL,NULL,1);
			SDS_SetUndo(IC_UNDO_REDRAW_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
			sds_setvar("UCSICON"/*DNT*/,&setgetrb);
			SDS_SetUndo(IC_UNDO_REDRAW_ON_REDO,NULL,NULL,NULL,SDS_CURDWG);
			if (SDS_CURDWG && SDS_CURVIEW) SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
			if (setgetrb.resval.rint&1) SDS_DrawUcsIcon(NULL,NULL,NULL,0);
			//if they turned it off or moved it, repaint the screen
		}
	}
    return(RTNORM); 
}

//========================FREEING OF GLOBALS==============================================
void cmd_ucs_freell(CIcadDoc *pDoc){
	struct SDS_ucs_llist *temp;

	while(pDoc->m_pucs_llbeg!=NULL){
		temp=pDoc->m_pucs_llbeg;
		pDoc->m_pucs_llbeg=pDoc->m_pucs_llbeg->next;
		temp->prev=temp->next=NULL;
		if (temp->name!=NULL) IC_FREE(temp->name);
		delete temp;
	}
	pDoc->m_pucs_llcur=pDoc->m_pucs_llbeg=NULL;
}
//======================================================================
int cmd_ucs_add2ll(void){
	struct SDS_ucs_llist *cmd_ucs_temp;
	struct resbuf rb;

	//we need to add a new UCS to the llist so the user can get back
	//to it!

	if ((cmd_ucs_temp= new struct SDS_ucs_llist )==NULL){
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		return(RTERROR);
	}
	memset(cmd_ucs_temp,0,sizeof(struct SDS_ucs_llist));
	if (SDS_CURDOC->m_pucs_llbeg==NULL){
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
	SDS_getvar(NULL,DB_QUCSNAME,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (*rb.resval.rstring){
		if ((SDS_CURDOC->m_pucs_llcur->name= new char [strlen(rb.resval.rstring)+sizeof(char)] )==NULL) return(RTERROR);
		strcpy(SDS_CURDOC->m_pucs_llcur->name,rb.resval.rstring);
	}
	if (rb.resval.rstring!=NULL) IC_FREE(rb.resval.rstring);
	return(RTNORM);

}
//==============================================================================================
long cmd_ucsprev(void)   { 
	struct resbuf rb;
	int ucsfollow;

	if (SDS_CURDOC==NULL) return(RTERROR);
    ucsfollow=(SDS_getvar(NULL,DB_QUCSFOLLOW,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM) ? rb.resval.rint : 0;

	if (SDS_CURDOC->m_pucs_llcur==NULL){
		cmd_ErrorPrompt(CMD_ERR_NOUCSDEFS,0);
		return(RTERROR);
	}
	rb.restype=RT3DPOINT;
	ic_ptcpy(rb.resval.rpoint,SDS_CURDOC->m_pucs_llcur->org);
	if (sdsengine_setvar("UCSORG"/*DNT*/,&rb)!=RTNORM) return(RTERROR);
	ic_ptcpy(rb.resval.rpoint,SDS_CURDOC->m_pucs_llcur->xdir);
	if (sdsengine_setvar("UCSXDIR"/*DNT*/,&rb)!=RTNORM) return(RTERROR);
	ic_ptcpy(rb.resval.rpoint,SDS_CURDOC->m_pucs_llcur->ydir);
	if (sdsengine_setvar("UCSYDIR"/*DNT*/,&rb)!=RTNORM) return(RTERROR);
	rb.restype=RTSTR;
   if (NULL==SDS_CURDOC->m_pucs_llcur->name)rb.resval.rstring=""/*DNT*/ ;
	else rb.resval.rstring=SDS_CURDOC->m_pucs_llcur->name;
	//don't free llist until we set var
	if (sdsengine_setvar("UCSNAME"/*DNT*/,&rb)!=RTNORM) return(RTERROR);

	if (SDS_CURDOC->m_pucs_llcur->prev!=NULL){
		SDS_CURDOC->m_pucs_llcur=SDS_CURDOC->m_pucs_llcur->prev;
		SDS_CURDOC->m_pucs_llcur->next->prev=NULL;
		SDS_CURDOC->m_pucs_llcur->next->next=NULL;
		if (SDS_CURDOC->m_pucs_llcur->next->name!=NULL) IC_FREE(SDS_CURDOC->m_pucs_llcur->next->name);
		delete SDS_CURDOC->m_pucs_llcur->next;
		SDS_CURDOC->m_pucs_llcur->next=NULL;
	}else{
		cmd_ucs_freell(SDS_CURDOC);
	}
    return(RTNORM);
}

//==============================================================================================

short cmd_ucs(void) 
	{
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	short ret;
    if (SDS_CMainWindow->m_bExpInsertState) 
		{
		ret = cmd_ucsdef(1); 
		}
	else
		{
		ret = cmd_ucsdef(0);
		if (ret==RTNORM)
			{
			// Set the drawing modified flag.
			SDS_DBModified( IC_DBMOD_ENTITIES_MODIFIED);

			/*D.Gavrilov*/// We should set up SDS_CMainWindow->m_pDragVarsCur->xfa
			// if we are dragging and change UCS transparently.
			// This code was taken from SDS_dragobject and commented out for all
			// cases except of DB_LINE. It's TO DO: invesigate other cases and
			// uncomment according code.
			if(SDS_CMainWindow->m_pDragVarsCur != NULL)
				{
				int xfmode = 1;
				switch(SDS_CMainWindow->m_pDragVarsCur->mode)
					{
//					case -1:
					case -6:
						if(SDS_CMainWindow->m_pDragVarsCur->elp &&
   							(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_LINE /*||
							SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_POINT ||
							SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_XLINE ||
							SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_RAY ||
							SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_ELLIPSE ||
							SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_TOLERANCE ||
	   						SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_LEADER ||
							SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_MLINE ||
							SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_SPLINE ||
							SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_MTEXT ||
							SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_VIEWPORT ||
							SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_DIMENSION ||
							SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_3DFACE */)) 
								xfmode=0; 
/*						else
							if(SDS_CMainWindow->m_pDragVarsCur->elp && SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_POLYLINE)
								{
								SDS_CMainWindow->m_pDragVarsCur->elp->get_70(&xfmode);
								if(0==(xfmode&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)))
									xfmode=1;//2d poly
								else xfmode=0;//3d poly
								}*/
						break;
/*					case -7:
					case 2:
					case 18:
					case 27:
					case 30:
					case 36: //viewports ALWAYS wcs
					case 37:
					case 38:
						xfmode=0;
						break;*/
					}
				if(SDS_CMainWindow->m_pDragVarsCur->mode<0 && SDS_CMainWindow->m_pDragVarsCur->elp)
					{
					SDS_CMainWindow->m_pDragVarsCur->elp->get_210(SDS_CMainWindow->m_pDragVarsCur->extru);
					}
				else
					{
					SDS_CMainWindow->m_pDragVarsCur->extru[0] = 0.0;
					SDS_CMainWindow->m_pDragVarsCur->extru[1] = 0.0;
					SDS_CMainWindow->m_pDragVarsCur->extru[2] = 0.0;
					}
				// now let's set up xfa
				SDS_CURDWG->initucs2ent(SDS_CMainWindow->m_pDragVarsCur->xfa,
	  									SDS_CMainWindow->m_pDragVarsCur->extru,
										&SDS_CMainWindow->m_pDragVarsCur->dang, xfmode);
				} // if(SDS_CMainWindow->m_pDragVarsCur != NULL)

			}
		}
	return ret;
}


void cmd_DrawUcsIcon(sds_point vdir, const sds_real *sclfact,int unpaint) {
	if (SDS_CURDOC==NULL) return;
	POSITION pos=SDS_CURDOC->GetFirstViewPosition();
	while (pos!=NULL) {
		CIcadView *pView=(CIcadView *)SDS_CURDOC->GetNextView(pos);//set the view
		ASSERT_KINDOF(CIcadView, pView);
		SDS_InitThisView(pView);
		SDS_DrawUcsIcon(vdir,sclfact,pView,unpaint);//redraw the icon
	}
}

short cmd_ucsdef(int fromexplorer){

	//NOTE: This command will temporarily set OSMODE to 0 so that all selections
	//	(& those in subroutines) don't OSNAP to any entities.

	//NOTE: ELEVATION is kept as a height relative to the WCS.  Any changes to the
	//		UCS need to RESET the elevation to its old value so that internally it
	//		will be re-calculated according to the new UCS settings!

	//NOTE: f'n to save current settings to global llist of ucs's is done
	//		by subroutines.  can't do it here because they may not successfully
	//		change the ucs.  The current settings aren't in the list. 
	
	//		UCSXDIR, UCSYDIR, & UCSORG	are set by subroutines.  UCSNAME set here.
	//		Also, many subroutines paint out ucsicon.  this routine repaints it, 
	//			and it sets UCSWORLD

    if (cmd_iswindowopen() == RTERROR) return RTERROR;			

	sds_point p1,p2,lastpt; 
	struct resbuf rb,rbucs,rbwcs;
    char fs1[IC_ACADBUF],tempstr[150],*newucsname;
    short osmode=-1,worlducs;
    RT	ret,rc=(0L);
	sds_real elev2keep;
	int ucsfollow,ucs_do_redraw=1;
	CIcadView *pView=SDS_CURVIEW;
	struct gra_view *view=SDS_CURGRVW;
	bool needsRegen = false;


	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	ucsfollow=(SDS_getvar(NULL,DB_QUCSFOLLOW,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM) ? rb.resval.rint : 0;

    
	//get the current elevation, which we'll reset upon exiting
	SDS_getvar(NULL,DB_QELEVATION,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	elev2keep=rb.resval.rreal;

	promptagain:
	if (!fromexplorer){
       if (sds_initget(0,ResourceString(IDC_UCS_INITGET_LIST_UCSS___21, "List_UCSs|? ~ 3_point|3point Delete Entity Origin ~OBject Previous Restore Save View X Y Z Z_axis|ZAxis World ~_? ~_ ~_3point ~_Delete ~_Entity ~_Origin ~_OBject ~_Previous ~_Restore ~_Save ~_View ~_X ~_Y ~_Z ~_ZAxis ~_World" ) )!=RTNORM) return(-2);
       ret=sds_getkword(ResourceString(IDC_UCS__N__3POINT_DELETE_E_22, "\n?/3point/Delete/Entity/Origin/Previous/Restore/Save/View/X/Y/Z/ZAxis/<World>: " ) ,fs1);
	}else{
       if (sds_initget(0,ResourceString(IDC_UCS_INITGET_CURRENT___3_23, "Current ~ 3_point|3point Entity Origin ~OBject View X Y Z Z_axis|ZAxis ~_Current ~_ ~_3point ~_Entity ~_Origin ~_OBject ~_View ~_X ~_Y ~_Z ~_|ZAxis" ) )!=RTNORM) return(-2);
       ret=sds_getkword(ResourceString(IDC_UCS__NCURRENT_ENTITY_OR_24, "\nCurrent/Entity/Origin/View/X/Y/Z/ZAxis/<3point>: " ) ,fs1);
	}
	if (ret==RTERROR) {
        return(ret);
    } else if (ret==RTCAN) {
        return(ret);
    } else if (ret==RTNONE){
		ret=RTNORM;
		if (!fromexplorer)
           strncpy(fs1,"WORLD"/*DNT*/  ,sizeof(fs1)-1);
		else
           strncpy(fs1,"3POINT"/*DNT*/  ,sizeof(fs1)-1);

	}


	//NOTE: set osmode to 0 before getting any points....
	// Removed this code I don't know why we need ot do 
	//SDS_getvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	//osmode=rb.resval.rint;
	//rb.resval.rint=0;
	//sds_setvar("OSMODE",&rb);
	SDS_getvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(lastpt,rb.resval.rpoint);

    strupr(fs1);
    if (strisame("ORIGIN"/*DNT*/  ,fs1)) {
        if ((rc=cmd_ucsorigin())<(-1L)) {                                                
    	    //sds_printf("\nInternal Error.  Line - %d, File - %s",(-rc),__FILE__); 
            ret=RTERROR;                                                   
			goto exit;
        }    
		ucs_do_redraw=0;	//req'd a/c display objects based upon ucs.  if no redraw, we get ghosting after move
		newucsname=""/*DNT*/;
    }else if (strisame("ZAXIS"/*DNT*/  ,fs1)) {
		needsRegen = true;
		SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
		p1[0]=p1[1]=p1[2]=0.0;
		cmd_ucs_pt2str(p1,tempstr);
       sprintf(fs1,ResourceString(IDC_UCS__NORIGIN_POINT___S__30, "\nOrigin point <%s>: " ) ,tempstr);
		sds_initget(0,NULL);
		if ((ret=internalGetpoint(NULL,fs1,p1))==RTNONE){
			p1[0]=p1[1]=p1[2]=0.0;
		}else if (ret!=RTNORM){
			goto exit;
		}
		ic_ptcpy(rb.resval.rpoint,p1);
		rb.resval.rpoint[2]+=1.0;
		cmd_ucs_pt2str(rb.resval.rpoint,tempstr);
       sprintf(fs1,ResourceString(IDC_UCS__NPOINT_ON_POSITIVE_31, "\nPoint on positive Z-axis <%s>: " ) ,tempstr);
		sds_initget(0,NULL);
		rc=ret;
		cmd_DrawUcsIcon(NULL,NULL,1);
		if ((ret=internalGetpoint(p1,fs1,p2))==RTNONE || ret==RTNORM){
			if (ret==RTNONE && rc==RTNONE){
				cmd_DrawUcsIcon(NULL,NULL,0);
				return(RTNONE);
			}
			if (ret==RTNONE)ic_ptcpy(p2,rb.resval.rpoint);
		}else if (ret!=RTNORM){
			goto exit;
		}
		if ((rc=cmd_ucszaxis(p1,p2))<(-1L)) {                                                      
            //sds_printf("\nInternal Error.  Line - %d, File - %s",(-rc),__FILE__);               
            ret=RTERROR;
			goto exit;
        } 
		newucsname=""/*DNT*/;
    } else if (strisame("3POINT"/*DNT*/,fs1)) {
		needsRegen = true;
		SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
		cmd_DrawUcsIcon(NULL,NULL,1);
        if ((rc=cmd_ucs3point())<(-1L)) {                                               
            //sds_printf("\nInternal Error.  Line - %d, File - %s",(-rc),__FILE__);       
            ret=RTERROR;
			goto exit;
        }             
		newucsname=""/*DNT*/;
    } else if (sds_wcmatch(fs1,"OBJECT,ENTITY"/*DNT*/)==RTNORM) {
		needsRegen = true;
		SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
		if ((rc=cmd_ucs_objectdef())<(-1L)) {
            //sds_printf("\nInternal Error.  Line - %d, File - %s",(-rc),__FILE__);      
            ret=RTERROR;
			goto exit;
        }             
		newucsname=""/*DNT*/;
    } else if (strisame("VIEW"/*DNT*/,fs1)) {
		needsRegen = true;
		SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
		SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(p1,rb.resval.rpoint);
		cmd_DrawUcsIcon(NULL,NULL,1);
		if ((rc=cmd_ucszaxis(rb.resval.rpoint,NULL))<(-1L)){
            //sds_printf("\nInternal Error.  Line - %d, File - %s",(-rc),__FILE__);      
            ret=RTERROR;
			goto exit;
        }             
		newucsname=""/*DNT*/;
    } else if (strisame("X"/*DNT*/  ,fs1)) {
		needsRegen = true;
		SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
        if ((rc=cmd_ucsxyz(0))<(-1L)) {                                                       
            //sds_printf("\nInternal Error.  Line - %d, File - %s",(-rc),__FILE__);              
            ret=RTERROR;
			goto exit;
        }                                                                                    
		newucsname=""/*DNT*/;
    } else if (strisame("Y"/*DNT*/ ,fs1)) {
		needsRegen = true;
		SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
        if ((rc=cmd_ucsxyz(1))<(-1L)) {                                                           
            //sds_printf("\nInternal Error.  Line - %d, File - %s",(-rc),__FILE__);               
            ret=RTERROR;
			goto exit;
        }
		newucsname=""/*DNT*/;
    } else if (strisame("Z"/*DNT*/  ,fs1)) {
		needsRegen = true;
		SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
        if ((rc=cmd_ucsxyz(2))<(-1L)) {                                                           
            //sds_printf("\nInternal Error.  Line - %d, File - %s",(-rc),__FILE__);               
            ret=RTERROR;
			goto exit;
        }                                                                                     
		newucsname=""/*DNT*/;
    } else if (strisame("PREVIOUS"/*DNT*/  ,fs1)) {
		needsRegen = true;
		SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
		cmd_DrawUcsIcon(NULL,NULL,1);//paint it out
        if ((rc=cmd_ucsprev())<(-1L)) {                                                      
            //sds_printf("\nInternal Error.  Line - %d, File - %s",(-rc),__FILE__);          
            ret=RTERROR;
			goto exit;
        }                                                                                
		newucsname=NULL;	//ucsname handled by subroutine
    } else if (strisame("RESTORE"/*DNT*/  ,fs1)) {
		needsRegen = true;
		SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
        if ((rc=cmd_ucsrestore(NULL))<(-1L)) {                                                   
            //sds_printf("\nInternal Error.  Line - %d, File - %s",(-rc),__FILE__);          
            ret=RTERROR;
			goto exit;
        }                                                                                
		newucsname=NULL;	//ucsname handled by subroutine
    } else if (strisame("SAVE"/*DNT*/  ,fs1)) {
		ucs_do_redraw=0;
        if ((rc=cmd_ucssave(0))<(-1L)) { 
            //sds_printf("\nInternal Error.  Line - %d, File - %s",(-rc),__FILE__);               
            ret=RTERROR;
			goto exit;
        }                                                                                     
		newucsname=NULL;	//ucsname handled by subroutine
    } else if (strisame("DELETE"/*DNT*/  ,fs1)) {
		ucs_do_redraw=0;
        if ((rc=cmd_ucssave(1))<(-1L)) {                                                     
            //sds_printf("\nInternal Error.  Line - %d, File - %s",(-rc),__FILE__);               
            ret=RTERROR;
			goto exit;
        }                                                                                     
		newucsname=NULL;	//ucsname handled by subroutine
    } else if (strisame("?"/*DNT*/  ,fs1)) {
		ucs_do_redraw=0;
        if ((rc=cmd_ucslister())<(-1L)) {                                                        
            //sds_printf("\nInternal Error.  Line - %d, File - %s",(-rc),__FILE__);              
            ret=RTERROR;
			goto exit;
        }                                                                                    
		newucsname=NULL;
		ucs_do_redraw=1;	//if we're going back to top of loop, reset default to redraw on
		goto promptagain;	// AutoCAD returns you to the UCS prompt - so we do too, now. 
    } else if (strisame("WORLD"/*DNT*/  ,fs1)) {
		cmd_DrawUcsIcon(NULL,NULL,1);//paint it out
        if ((rc=cmd_ucsworld())<(-1L)) {                                                      
            //sds_printf("\nInternal Error.  Line - %d, File - %s",(-rc),__FILE__);            
            ret=RTERROR;
			goto exit;
        } 
       newucsname=""/*DNT*/ ;
    } else if (strisame("CURRENT"/*DNT*/  ,fs1)){
        //called from explorer ONLY, and explorer will save current settings so we don't have to do anything!
        ret=RTNORM;
        goto exit;
    }

	if ( needsRegen && ucsfollow )	// AG: I added checking of ucsfollow to exclude regen in some cases
		cmd_regenall();

	if (ucsfollow){
		/*
		rb.restype=RT3DPOINT;
		rb.resval.rpoint[0]=rb.resval.rpoint[1]=0.0;
		rb.resval.rpoint[2]=1.0;
		sds_setvar(ResourceString(IDC_UCS_VIEWDIR_42, "VIEWDIR" )DNT,&rb);
		gra_twist4ucs(SDS_CURDWG,NULL);
		cmd_zoom_extents(0);*/
		cmd_view_plan(1);
	}else{
		//gra_initview(SDS_CURDWG,&SDS_CURCFG,&pView->m_pCurDwgView,NULL);
		//sds_getvar("VIEWCTR",&rb);
		struct gra_view *view=SDS_CURGRVW;
		struct gr_viewvars viewvarsp;
		SDS_VarToTab(SDS_CURDWG,pView->m_pVportTabHip);
		SDS_ViewToVars2Use(SDS_CURVIEW,SDS_CURDWG,&viewvarsp);
		//ic_ptcpy(viewvarsp.viewctr,rb.resval.rpoint);
		if (SDS_CURDOC->m_RegenListView==pView) 
			SDS_CURDOC->m_RegenListView=NULL; 
		if (view) {
			gr_initview(SDS_CURDWG,&SDS_CURCFG,&view,&viewvarsp);
			SDS_BuildRegenList(SDS_CURDWG,view,pView,SDS_CURDOC);
		}
		if (ucs_do_redraw)cmd_DrawUcsIcon(NULL,NULL,0); 		
	}
	
	if (newucsname!=NULL){
		rb.resval.rstring=newucsname;
		rb.restype=RTSTR;
		sdsengine_setvar("UCSNAME"/*DNT*/,&rb);
	}

	exit:

	rb.restype=RTREAL;
	rb.resval.rreal=elev2keep;
	sds_setvar("ELEVATION"/*DNT*/,&rb);

	//set worlducs var
	worlducs=0;
	p1[2]=p1[1]=p1[0]=0.0;
	SDS_getvar(NULL,DB_QUCSORG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);	
	if (icadPointEqual(rb.resval.rpoint,p1)){
		SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		p1[0]=1.0;
		if (icadPointEqual(p1,rb.resval.rpoint)){
			p1[0]=0.0;
			p1[1]=1.0;
			SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if (icadPointEqual(p1,rb.resval.rpoint))
				worlducs=1;
		}
	}
	if (ret!=RTNORM && ucs_do_redraw) cmd_DrawUcsIcon(NULL,NULL,0);
	
	rb.restype=RTSHORT;
	rb.resval.rint=worlducs;
	sds_setvar("WORLDUCS"/*DNT*/,&rb);
	rb.restype=RT3DPOINT;
	ic_ptcpy(rb.resval.rpoint,lastpt);
	SDS_setvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
    return(ret); 
}

//==============================================================================================
long cmd_ucsorigin(void)   { 
	//note: this f'n does NOT update worlducs variable.  caller does.
    char fs1[IC_ACADBUF],fs2[150];
	sds_point p0;
    RT ret=0;
    long rc=0;
    struct resbuf rb,rb2;
	CIcadView *pView=SDS_CURVIEW;
	struct gra_view *view=SDS_CURGRVW;
    
	p0[0]=p0[1]=p0[2]=0.0;
	cmd_ucs_pt2str(p0,fs2);
	
	SDS_getvar(NULL,DB_QUCSORG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (icadPointEqual(rb.resval.rpoint,p0)){
       sprintf(fs1,ResourceString(IDC_UCS__NORIGIN_POINT___S__30, "\nOrigin point <%s>: " ) ,fs2);
	}else{
       sprintf(fs1,ResourceString(IDC_UCS__NWORLD_ORIGIN_POIN_43, "\nWorld/Origin point <%s>: " ) ,fs2);
       sds_initget(0,ResourceString(IDC_UCS_INITGET_WORLD_ORIGI_44, "World_origin|World ~_World" ) );
	}
    if ((ret=(sds_getpoint(NULL,fs1,p0)))==RTCAN) {rc=(-1L); goto bail;}
    if (ret==RTERROR) {rc=(-1L); goto bail;}
    if (ret==RTNONE)  {rc=(0); goto bail;}
	if (ret!=RTKWORD){
		rb.restype=rb2.restype=RTSHORT;
		rb.rbnext=rb2.rbnext=NULL;
		rb.resval.rint=0;
		rb2.resval.rint=1;
		sds_trans(p0,&rb2,&rb,0,p0);
	}//else leave p0 at world origin

	SDS_getvar(NULL,DB_QUCSORG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (icadPointEqual(rb.resval.rpoint,p0))return(0);

	rb.restype=RT3DPOINT;
	ic_ptcpy(rb.resval.rpoint,p0);
	cmd_DrawUcsIcon(NULL,NULL,1);
    if (sdsengine_setvar("UCSORG"/*DNT*/,&rb)!=RTNORM) { 
        rc=(-1L); 
        goto bail; 
    }
	gr_initview(SDS_CURDWG,&SDS_CURCFG,&view,NULL);

bail:
	cmd_DrawUcsIcon(NULL,NULL,0);
    return(rc);

}

//==============================================================================================
long cmd_ucszaxis(const sds_point pp1, const sds_point pp2)   { 
	//given 2 UCS pts pp1 and pp2 which define the new positive Z axis, this f'n 
	//will set the new X and Y axes. NOTE: The new x axis will always be in
	//the XY plane of the WCS
	//If pp2 is NULL, pp1 is assumed to be a Z direction vector to define the
	//new coordinate system with its origin unchanged (0,0,0) current UCS
	//NOTE: PP1 AND PP2 ARE IN UCS (not wcs)!!!!!(passed from internalGetpoint)
	sds_point p1,p2,p3,p4;
	struct resbuf rb,rbucs,rbwcs;
	long ret=RTNORM;
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.rbnext=rbwcs.rbnext=NULL;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	if (RTNORM!=(ret=cmd_ucs_add2ll()))return(ret);
	if (pp2!=NULL){
		sds_trans(pp1,&rbucs,&rbwcs,0,p1);
		sds_trans(pp2,&rbucs,&rbwcs,0,p2);
		ic_ptcpy(rb.resval.rpoint,p1);
		rb.restype=RT3DPOINT;
		if ((ret=sdsengine_setvar("UCSORG"/*DNT*/,&rb))!=RTNORM)return(ret);

		p2[0] -= p1[0];
		p2[1] -= p1[1];
		p2[2] -= p1[2];

	}else{ //pp1 represents a ucs z direction.
		sds_trans(pp1,&rbucs,&rbwcs,1,p2);  // transform the vector to wcs
	}

	sds_point temp;
	ic_arbaxis(p2,p3,p4,temp);

	ic_ptcpy(rb.resval.rpoint,p3);
	rb.restype=RT3DPOINT;rb.rbnext=NULL;
	if ((ret=sdsengine_setvar("UCSXDIR"/*DNT*/,&rb))!=RTNORM)return(ret);
	ic_ptcpy(rb.resval.rpoint,p4);
	if ((ret=sdsengine_setvar("UCSYDIR"/*DNT*/,&rb))!=RTNORM)return(ret);

#ifdef OUTFORNOW
		//p1 & p2 are starting & ending pts for defining Z vector
	if (icadRealEqual(p2[0],p1[0]) && icadRealEqual(p2[1],p1[1])){
		if (icadRealEqual(p2[2],p1[2]))return(RTERROR);  //coincident points
		//defining Z vector parallel to WCS's Z

		struct gra_view *view=SDS_CURGRVW;
		if ((p2[2]-p1[2])>0.0)
			p3[0]=1.0;
		else
			p3[0]=-1.0;
		p3[1]=p3[2]=0.0;
		rbwcs.resval.rint=2;//use resbuf as dcs
		sds_trans(p3,&rbucs,&rbwcs,1,p4);//transform to dcs as vector
		rbwcs.resval.rint=0;//back to wcs resbuf
		p4[0]=atan2(p4[1],p4[0]);  //angle to twist the UCS around Z
		//if the Z's go in opposite direction, 
		//	we're still okay 
		p3[0]=cos(-p4[0]);
		p3[1]=sin(-p4[0]);
		p3[2]=0.0;
		p2[0]=p2[1]=0.0;
		if (p2[2]>p1[2])p2[2]=1.0;
		else p2[2]=-1.0;

		ic_crossproduct(p2,p3,p4);
//		p4[0]=p2[1]*p3[2]-p3[1]*p2[2];
//		p4[1]=p2[2]*p3[0]-p3[2]*p2[0];
//		p4[2]=p2[0]*p3[1]-p3[0]*p2[1];

		ic_ptcpy(rb.resval.rpoint,p3);
		rb.restype=RT3DPOINT;rb.rbnext=NULL;
		if ((ret=sdsengine_setvar("UCSXDIR"/*DNT*/,&rb))!=RTNORM)return(ret);
		ic_ptcpy(rb.resval.rpoint,p4);
		if ((ret=sdsengine_setvar("UCSYDIR"/*DNT*/,&rb))!=RTNORM)return(ret);
		
		//sds_trans(p3,&rbucs,&rbwcs,1,p2);
		//p3[0]=-p1[1];
		//p3[1]=p1[0];
		//p3[2]=0.0;
		//sds_trans(p3,&rbucs,&rbwcs,1,p4);
		//rbucs.restype=RT3DPOINT;
		//ic_ptcpy(rbucs.resval.rpoint,p2);
		//if ((ret=sdsengine_setvar("UCSXDIR",&rbucs))!=RTNORM)return(ret);
		//ic_ptcpy(rbucs.resval.rpoint,p4);
		//sdsengine_setvar("UCSYDIR",&rbucs);

	}else{	//Z vector NOT parallel to WCS's Z
		//project vector into world XY plane and take its normal P3
		//project p1->p2 vector into world XY plane & go ccw
		//90degrees to get the new x axis

		p3[0]=p1[0]-(p2[1]-p1[1]);
		p3[1]=p1[1]+(p2[0]-p1[0]);
		p3[2]=p1[2];
		cmd_unit_vector(p1,p3,p4);//p4 is unit vector from p1 toward p3
		rb.restype=RT3DPOINT;
		ic_ptcpy(rb.resval.rpoint,p4);
		if ((ret=sdsengine_setvar("UCSXDIR"/*DNT*/,&rb))!=RTNORM)return(ret);
		//now if we cross the z w/the x, we get the y vector
		cmd_unit_vector(p1,p2,p3); //p3 is unit vector from p1 toward p2
		rb.resval.rpoint[0]=p3[1]*p4[2]-p3[2]*p4[1];
		rb.resval.rpoint[1]=p3[2]*p4[0]-p3[0]*p4[2];
		rb.resval.rpoint[2]=p3[0]*p4[1]-p3[1]*p4[0];
		ret=sdsengine_setvar("UCSYDIR"/*DNT*/,&rb);
	}
#endif

	return(ret);
}

//==============================================================================================
long cmd_ucs3point(void)   { 
	char fs1[IC_ACADBUF],fs2[150];
	sds_point p0,p1,p2,p3,p4,p5,dummy;
	struct resbuf rbwcs,rbucs;
	long ret=RTNORM;
	short drawit=0;

	dummy[0]=dummy[1]=dummy[2]=0.0;	
	rbucs.rbnext=rbwcs.rbnext=NULL;
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;
	
	cmd_ucs_pt2str(dummy,fs2);
   sprintf(fs1,ResourceString(IDC_UCS__NNEW_ORIGIN___S____45, "\nNew origin <%s>: " ) ,fs2);
	sds_initget(0,NULL);
	if ((ret=internalGetpoint(NULL,fs1,p0))==RTNONE){
		ic_ptcpy(p0,dummy);
	}else if (ret!=RTNORM) goto exit;
	sds_trans(p0,&rbucs,&rbwcs,0,p1);
	ic_ptcpy(dummy,p0);
	dummy[0]+=1.0;
	cmd_ucs_pt2str(dummy,fs2);
   sprintf(fs1,ResourceString(IDC_UCS__NPOINT_ON_POSITIVE_46, "\nPoint on positive X axis <%s>: " ) ,fs2);
	sds_initget(0,NULL);
	if ((ret=internalGetpoint(p0,fs1,p2))==RTNONE){
		ic_ptcpy(p2,dummy);
	}else if (ret!=RTNORM) goto exit;
	cmd_unit_vector(p0,p2,p3);
	ic_ptcpy(p5,p2);
	sds_grdraw(p0,p5,-1,1);
	drawit=1;
	sds_trans(p3,&rbucs,&rbwcs,1,p2);
	ic_ptcpy(dummy,p0);
	dummy[1]+=1.0;
	cmd_ucs_pt2str(dummy,fs2);
   sprintf(fs1,ResourceString(IDC_UCS__NPOINT_IN_X_Y_PLAN_47, "\nPoint in X-Y plane with positive Y value <%s>: " ) ,fs2);
	sds_initget(0,NULL);
	if ((ret=internalGetpoint(p0,fs1,p3))==RTNONE){
		ic_ptcpy(p3,dummy);
	}else if (ret!=RTNORM)goto exit;
	cmd_unit_vector(p0,p3,p4);
	sds_trans(p4,&rbucs,&rbwcs,1,p3);
	//now we have to calculate the actual Y direction
	//calculate new Z direction p0
	p4[0]=p2[1]*p3[2]-p2[2]*p3[1];
	p4[1]=p2[2]*p3[0]-p2[0]*p3[2];
	p4[2]=p2[0]*p3[1]-p2[1]*p3[0];
	//vectors weren't perpendicular, so unitize 
	dummy[0]=sqrt(p4[0]*p4[0]+p4[1]*p4[1]+p4[2]*p4[2]);
	p4[0]/=dummy[0];
	p4[1]/=dummy[0];
	p4[2]/=dummy[0];
	if (fabs(dummy[0])<CMD_FUZZ){ret=RTERROR;goto exit;}
	//cross new Z w/ new X to get new Y
	ic_crossproduct(p4,p2,p3);
	//we got all pts, so we can save current stuff & set all the variables
	if (RTNORM!=(ret=cmd_ucs_add2ll()))goto exit;
	//cmd_DrawUcsIcon(NULL,NULL,1);//paint it out
	rbucs.restype=RT3DPOINT;
	ic_ptcpy(rbucs.resval.rpoint,p1);
	if ((ret=sdsengine_setvar("UCSORG"/*DNT*/,&rbucs))!=RTNORM)goto exit;
	ic_ptcpy(rbucs.resval.rpoint,p2);
	if ((ret=sdsengine_setvar("UCSXDIR"/*DNT*/,&rbucs))!=RTNORM)goto exit;
	ic_ptcpy(rbucs.resval.rpoint,p3);
	if ((ret=sdsengine_setvar("UCSYDIR"/*DNT*/,&rbucs))!=RTNORM)goto exit;

	exit:
	//caller will repaint icon
	if (drawit)sds_grdraw(p0,p5,-1,1);
    return(ret);
}

void cmd_ucs_pt2str(sds_point pt, char *str){
	char fs2[50];

	sds_rtos(pt[0],-1,-1,fs2);
	strcpy(str,fs2);
   strcat(str,ResourceString(IDC_UCS___48, "," ) );
	sds_rtos(pt[1],-1,-1,fs2);
	strcat(str,fs2);
   strcat(str,ResourceString(IDC_UCS___48, "," ) );
	sds_rtos(pt[2],-1,-1,fs2);
	strcat(str,fs2);
}


//==============================================================================================
long cmd_ucsxyz(short xyzmode)   { 
	//if mode==0:rotate about X axis
	//if mode==1:rotate about Y axis
	//if mode==2:rotate about Z axis (default)
	//NOTE: paints OUT icon.  Caller must repaint it

	sds_real ang;
	struct resbuf rbucs,rbwcs;
	sds_point p1,p2,p3,p4;
	RT rc;
	
	sds_initget(0,NULL);
   if (xyzmode==0)rc=sds_getorient(NULL,ResourceString(IDC_UCS__NENTER_ANGLE_OF_RO_49, "\nEnter angle of rotation about X axis <0>: " ) ,&ang);
   else if (xyzmode==1)rc=sds_getorient(NULL,ResourceString(IDC_UCS__NENTER_ANGLE_OF_RO_50, "\nEnter angle of rotation about Y axis <0>: " ) ,&ang);
   else rc=sds_getorient(NULL,ResourceString(IDC_UCS__NENTER_ANGLE_OF_RO_51, "\nEnter angle of rotation about Z axis <0>: " ) ,&ang);
	cmd_DrawUcsIcon(NULL,NULL,1);//paint it out
	if (rc!=RTNORM) return(rc);
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbwcs.resval.rint=0;
	rbucs.resval.rint=1;
	if (RTNORM!=(rc=cmd_ucs_add2ll()))return(rc);
	if (xyzmode==0){  //X axis
		p1[0]=0.0;
		p1[1]=cos(ang);
		p1[2]=sin(ang);
		sds_trans(p1,&rbucs,&rbwcs,1,p2);
		rbucs.restype=RT3DPOINT;
		ic_ptcpy(rbucs.resval.rpoint,p2);
		rc=sdsengine_setvar("UCSYDIR"/*DNT*/,&rbucs);
	}else if (xyzmode==1){	//Y axis
		p1[0]=cos(ang);
		p1[1]=0.0;
		p1[2]=-sin(ang);
		sds_trans(p1,&rbucs,&rbwcs,1,p2);
		rbucs.restype=RT3DPOINT;
		ic_ptcpy(rbucs.resval.rpoint,p2);
		rc=sdsengine_setvar("UCSXDIR"/*DNT*/,&rbucs);
	}else{					//Z axis
		p1[0]=cos(ang);
		p1[1]=sin(ang);
		p1[2]=0.0;
		sds_trans(p1,&rbucs,&rbwcs,1,p2);
		p3[0]=-p1[1];
		p3[1]=p1[0];
		p3[2]=0.0;
		sds_trans(p3,&rbucs,&rbwcs,1,p4);
		rbucs.restype=RT3DPOINT;
		ic_ptcpy(rbucs.resval.rpoint,p2);
		if ((rc=sdsengine_setvar("UCSXDIR"/*DNT*/,&rbucs))==RTNORM){
			ic_ptcpy(rbucs.resval.rpoint,p4);
			rc=sdsengine_setvar("UCSYDIR"/*DNT*/,&rbucs);
		}
	}

	return(rc);
}

long cmd_ucsrestore(char* name)   { 
    char ucsname[IC_ACADBUF];
    struct resbuf *tmprb=NULL,rb;
    RT ret;
    short numlines=0,gotit=0;
    long rc=0L;
	//caller must repaint icon
	if (name!=NULL)strncpy(ucsname,name,sizeof(ucsname)-1);
    while(1) {
	    if (sds_usrbrk()) { rc=(-1L); goto bail; }  //check for break by user
	    //Get entered ucs to load                                             	   
 	   	sds_initget(128,NULL);
		if (name==NULL){
           //if ((ret=sds_getkword(ResourceString(IDC_UCS__NUCS_NAME_TO_RESTO_52, "\nUCS name to restore: " ) ,ucsname))==RTERROR) {
			if ((ret=sds_getstring(1, ResourceString(IDC_UCS__NUCS_NAME_TO_RESTO_52, "\nUCS name to restore: " ) ,ucsname))==RTERROR) {
 	   			rc=(-__LINE__); goto bail;                                             
 	   		} else if (ret==RTCAN) {                                                    
 	   			rc=(-1L); goto bail;                                                   
 	   		} else if (ret==RTNONE) {                                                  
 	   			rc=(0); goto bail;                                                     
 	   		}
		}	

		ic_trim(ucsname,"be");

		//check if there is such a ucs
       if ((tmprb=sds_tblsearch("UCS"/*DNT*/,ucsname,1))==NULL) {     //if nothing in the table
           sds_printf(ResourceString(IDC_UCS__NCAN_NOT_FIND_UCS__53, "\nCan not find UCS %s\n" ) ,ucsname);   
	    }else{
			if (RTNORM!=(rc=cmd_ucs_add2ll()))goto bail;
			rb.rbnext=NULL;
	        //*** Set properties of the ucs 
			cmd_DrawUcsIcon(NULL,NULL,1);//paint it out
			rb.restype=RTSTR;
      	    if (ic_assoc(tmprb,2)!=0) {rc=(-__LINE__); goto bail;}                   
			rb.resval.rstring=ic_rbassoc->resval.rstring;
      	    if (sdsengine_setvar("UCSNAME"/*DNT*/,&rb)!=RTNORM) {rc=(-__LINE__); goto bail;} 
      	    if (ic_assoc(tmprb,10)!=0) {rc=(-__LINE__); goto bail;}              
			rb.restype=RT3DPOINT;
			ic_ptcpy(rb.resval.rpoint,ic_rbassoc->resval.rpoint);
    	    if (sdsengine_setvar("UCSORG"/*DNT*/,&rb)!=RTNORM) {rc=(-__LINE__); goto bail;}                                            
            if (ic_assoc(tmprb,11)!=0) {rc=(-__LINE__); goto bail;}       
			ic_ptcpy(rb.resval.rpoint,ic_rbassoc->resval.rpoint);
            if (sdsengine_setvar("UCSXDIR"/*DNT*/,&rb)!=RTNORM) {rc=(-__LINE__); goto bail;}      
            if (ic_assoc(tmprb,12)!=0) {rc=(-__LINE__); goto bail;}                
 			ic_ptcpy(rb.resval.rpoint,ic_rbassoc->resval.rpoint);
            if (sdsengine_setvar("UCSYDIR"/*DNT*/,&rb)!=RTNORM) {rc=(-__LINE__); goto bail;}    
            break;
        }
		if (name!=NULL)break;
    }
bail:
    IC_RELRB(tmprb);
    return(rc);
}

//==============================================================================================
long cmd_ucssave(short delete_mode)   { 
	if (SDS_CURDOC==NULL) return(RTERROR);

	//if delete_mode is non-zero, this f'n DELETES a UCS - otherwise
	//it will save a UCS
	char fs1[IC_ACADBUF],fs2[IC_ACADBUF],fs3[IC_ACADBUF];
	long ret;
	struct resbuf rb,*rbp1;
	sds_point ucsxdir,ucsydir,ucsorg;
	short messerflag=IC_TBLMESSER_MAKE;
	struct SDS_ucs_llist *cmd_ucs_temp;
	int fi1,expert=0;		// Suppresses some prompts depending on the value of "EXPERT"
	rbp1=NULL;
	
    if (SDS_getvar(NULL,DB_QEXPERT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return RTERROR;
	expert=rb.resval.rint;

	while(1){
		sds_initget(128,NULL);
		if (delete_mode)
			//ret=sds_getkword(ResourceString(IDC_UCS__NUCS_NAME_S_TO_DEL_54, "\nUCS name(s to delete, or ? for list: " ) ,fs1);
			ret=sds_getstring(1, ResourceString(IDC_UCS__NUCS_NAME_S_TO_DEL_54, "\nUCS name(s to delete, or ? for list: " ) ,fs1);
		else
           //ret=sds_getkword(ResourceString(IDC_UCS__NENTER_NAME_FOR_SA_55, "\nEnter name for saving UCS, or ? for list: " ) ,fs1);
		   ret=sds_getstring(1, ResourceString(IDC_UCS__NENTER_NAME_FOR_SA_55, "\nEnter name for saving UCS, or ? for list: " ) ,fs1);
		if (ret==RTNONE)
           strncpy(fs1,"?"/*DNT*/,sizeof(fs1)-1);
		else if (ret!=RTNORM)
			return(ret);
       if (strisame(fs1,"?" /*DNT*/)){
			if (RTCAN==cmd_ucslister())return(RTCAN);
		}else if (0==delete_mode && !ic_isvalidname(fs1))
			cmd_ErrorPrompt(CMD_ERR_ANYNAME,0);
		else
			break;
	}
	ic_trim(fs1,"be");
	strupr(fs1);
	//fs1 has name of UCS to save/delete
	if (delete_mode){
		fi1=0;
		rbp1=sds_tblnext("UCS"/*DNT*/,1);
		while(rbp1!=NULL){
			ic_assoc(rbp1,2);
			if (RTNORM==sds_wcmatch(ic_rbassoc->resval.rstring,fs1)){
				//go ahead and delete the UCS
				if ((ret=SDS_tblmesser(rbp1,IC_TBLMESSER_DELETE,SDS_CURDWG))!=RTNORM){
					cmd_ErrorPrompt(CMD_ERR_DELETEUCS,0);
					ret=RTERROR;
				}
				fi1++;
			}
			sds_relrb(rbp1);
			rbp1=NULL;
			rbp1=sds_tblnext("UCS"/*DNT*/,0);
		}
		if (fi1>0){
			//if current name among those being deleted, reset current name to ""
			if (SDS_getvar(NULL,DB_QUCSNAME,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return RTERROR;
			if (RTNORM==sds_wcmatch(rb.resval.rstring,fs1)){
				rb.resval.rstring=""/*DNT*/;
				sdsengine_setvar("UCSNAME"/*DNT*/,&rb);
			}
			//if the deleted ucs is in the llist of ucs's to restore, we need to delete
			//	it's name from the llist, but keep the entry.
			for (cmd_ucs_temp=SDS_CURDOC->m_pucs_llbeg;cmd_ucs_temp!=NULL;cmd_ucs_temp=cmd_ucs_temp->next){
				if (cmd_ucs_temp->name!=NULL && RTNORM==sds_wcmatch(cmd_ucs_temp->name,fs1)){
					IC_FREE(cmd_ucs_temp->name);
					cmd_ucs_temp->name=NULL;
				}
			}
           sds_printf(ResourceString(IDC_UCS__N_I__S_DELETED__56, "\n%i %s deleted." ) ,fi1,(fi1>1)?ResourceString(IDC_UCS_UCSS_57, "UCSs" ) :ResourceString(IDC_UCS_UCS_58, "UCS" ) );
		}else{
			cmd_ErrorPrompt(CMD_ERR_FINDUCS,0);
			return(RTNORM);		 
		}
		return(ret);
	}
	//adding name to current settings doesn't change them, so we don't need to add
	//	an entry to ucs llist.  When we reset a var, name will be taken into account.
   if ((rbp1=sds_tblsearch("UCS"/*DNT*/,fs1,0))!=NULL){
		sds_relrb(rbp1);
		rbp1=NULL;
		messerflag=IC_TBLMESSER_MODIFY;
		if (expert<4) {	// Skips the questions and assumes "Yes" response if EXPERT is 4 or higher.
		    for (;;){
        	    sds_initget(128, ResourceString(IDC_UCSSAVE_INIT1, "Yes No ~_Yes ~_No"));
                sprintf(fs2,ResourceString(IDC_UCS_UCS__S_ALREADY_EXIS_59, "UCS %s already exists.  Redefine it? (Yes/No) <No>: " ) ,fs1);	// 1-57872
			    if ((ret=sds_getkword(fs2,fs3))==RTERROR){
                    return(ret);
                }else if (ret==RTCAN){
                    return(ret);
                }else if (ret==RTNONE){
				    return(RTNORM);
		        }else if (ret==RTNORM){
                    if (strisame(fs3, "YES"/*DNT*/)) {
                        break;
                    }else if (strisame(fs3, "NO"/*DNT*/)) {
                        return(RTNORM);
                    }else{
                        continue;
                    }
                }
            }
        }
	}
	//Now on to saving the UCS
	SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(ucsxdir,rb.resval.rpoint);
	SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(ucsydir,rb.resval.rpoint);
	SDS_getvar(NULL,DB_QUCSORG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(ucsorg,rb.resval.rpoint);
	if (rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
   if ((rbp1=sds_buildlist(RTDXF0,"UCS"/*DNT*/,2,fs1,10,ucsorg,11,ucsxdir,
			12,ucsydir,70,0,0))==NULL)return(RTERROR);
	if ((ret=SDS_tblmesser(rbp1,messerflag,SDS_CURDWG))!=RTNORM){
		cmd_ErrorPrompt(CMD_ERR_SAVEUCS,0);
		ret=RTERROR;
	}else{
		rb.restype=RTSTR;
		rb.resval.rstring=fs1;
		ret=sdsengine_setvar("UCSNAME"/*DNT*/,&rb);
	}
	if (rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
	return(ret);
}
//=================================================================================================
long cmd_ucslister(void)   { 
    char fs1[IC_ACADBUF],ucsstr[IC_ACADBUF];
    struct resbuf *tmprb=NULL;
    RT	ret;
	int	scrollmax,scrollcur,fi1;
    long rc=0L;
	sds_point vpt={0.0,0.0,0.0},ptemp,ptemp1;
	struct resbuf rb,ucs,wcs;

    wcs.restype=RTSHORT;
    wcs.resval.rint=0;
    ucs.restype=RTSHORT;
    ucs.resval.rint=1;
    
    if ((ret=sds_getstring(0,ResourceString(IDC_UCS__NNAME_S_OF_UCS_TO__60, "\nName(s of UCS to list <* for all>: " ) ,ucsstr))==RTERROR) {
        rc=(-__LINE__); goto bail;
    } else if (ret==RTCAN) {
        rc=(-1L); goto bail;
    }
	if (ucsstr[0]==0){
		ucsstr[0]='*';
		ucsstr[1]=0;
	}
    strupr(ucsstr);
    
	SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);	
	if (!CMD_CMDECHO_PRINTF(rb.resval.rint))return(RTNORM);
	sds_textscr();
	if (SDS_getvar(NULL,DB_QSCRLHIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR); 
	scrollmax=min(SDS_CMainWindow->m_pTextScrn->m_pPromptWnd->m_pPromptWndList->m_nLinesPerPage,rb.resval.rint);
	scrollmax-=3;
    scrollcur=0;

	SDS_getvar(NULL,DB_QUCSNAME,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (*rb.resval.rstring){
       cmd_prtstr(rb.resval.rstring,ResourceString(IDC_UCS_CURRENT_UCS_NAME__61, "Current UCS name:" ) );
		if ((ret=cmd_entlister_scrollchk(&scrollcur,scrollmax))!=RTNORM) return(ret);
	}else{
		fi1=0;
		SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (icadRealEqual(rb.resval.rpoint[0],1.0)){
			SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if (icadRealEqual(rb.resval.rpoint[1],1.0)){
				SDS_getvar(NULL,DB_QUCSORG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if (icadPointEqual(rb.resval.rpoint,vpt)){
					fi1=1;
				}
			}
		}
		if (fi1){
           cmd_prtstr(ResourceString(IDC_UCS_CURRENTLY_IN_WORLD__62, "Currently in World UCS." ) ,NULL);
		}else{
           cmd_prtstr(ResourceString(IDC_UCS_CURRENT_UCS_IS_UNNA_63, "Current UCS is unnamed." ) ,NULL);
		}
        if ((ret=cmd_entlister_scrollchk(&scrollcur,scrollmax))!=RTNORM) return(ret);
	}

    
    if ((tmprb=sds_tblnext("UCS"/*DNT*/,1))==NULL) {     //if nothing in the table
		cmd_ErrorPrompt(CMD_ERR_NOUCSDEFS,-1);
		rc=RTNONE;
    } 
    
    // Because calls to cmd_prtstr() call rtos() which uses DIMZIN, it is possible
    // that if DIMZIN is set to 4, for the UCS coordinates to have the zeros removed.
    // Here we'll save off the current DIMZIN, set it to 0(zero) then restore it after
    // we finish showing the UCS data. This is the fix for bug # 5733 which I moved from
    // sds_conversion.cpp to here when I fixed DIMZIM == 4.
    struct resbuf dimzinbuf;
    int oldDimZin;
    SDS_getvar(NULL,DB_QDIMZIN,&dimzinbuf,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    oldDimZin = dimzinbuf.resval.rint;
    if (oldDimZin != 0) {
        dimzinbuf.resval.rint = 0;
        sds_setvar("DIMZIN"/*DNT*/,&dimzinbuf);
    }
    
    while(tmprb!=NULL) {
        if (sds_usrbrk()) { rc=(-1L); goto bail; }
        //*** UCS names.
        if (ic_assoc(tmprb,2)==0) {
            if (sds_wcmatch(ic_rbassoc->resval.rstring,ucsstr)==RTNORM) {
                //   Get the name of the ucs
                strncpy(fs1,ic_rbassoc->resval.rstring,sizeof(fs1)-1);
                //*** Get properties of the ucs and print them
				sds_printf("\n"/*DNT*/);
               cmd_prtstr(fs1,ResourceString(IDC_UCS_NAME_OF_UCS__64, "Name of UCS:" ) );
				if ((ret=cmd_entlister_scrollchk(&scrollcur,scrollmax))!=RTNORM) return(ret);
              	
                if (ic_assoc(tmprb,10)!=0) {rc=(-__LINE__); goto bail;}
				sds_trans(ic_rbassoc->resval.rpoint,&wcs,&ucs,0,vpt);
               cmd_prtpt(vpt,ResourceString(IDC_UCS_ORIGIN_OF_UCS__65, "Origin of UCS:" ) );
				if ((ret=cmd_entlister_scrollchk(&scrollcur,scrollmax))!=RTNORM) return(ret);
                
                if (ic_assoc(tmprb,11)!=0) {rc=(-__LINE__); goto bail;}          
				ic_ptcpy(ptemp,ic_rbassoc->resval.rpoint);
				sds_trans(ic_rbassoc->resval.rpoint,&wcs,&ucs,1,vpt);
               cmd_prtpt(vpt,ResourceString(IDC_UCS_X_AXIS__66, "X axis:" ) );
				if ((ret=cmd_entlister_scrollchk(&scrollcur,scrollmax))!=RTNORM) return(ret);
                
                if (ic_assoc(tmprb,12)!=0) {rc=(-__LINE__); goto bail;}               
				ic_ptcpy(ptemp1,ic_rbassoc->resval.rpoint);
                sds_trans(ic_rbassoc->resval.rpoint,&wcs,&ucs,1,vpt);
               cmd_prtpt(vpt,ResourceString(IDC_UCS_Y_AXIS__67, "Y axis:" ) );
				if ((ret=cmd_entlister_scrollchk(&scrollcur,scrollmax))!=RTNORM) return(ret);
				
				ic_crossproduct(ptemp,ptemp1,ptemp);
                sds_trans(ptemp,&wcs,&ucs,1,ptemp);
               cmd_prtpt(ptemp,ResourceString(IDC_UCS_Z_AXIS__68, "Z axis:" ) );
				if ((ret=cmd_entlister_scrollchk(&scrollcur,scrollmax))!=RTNORM) return(ret);
            }
        }
        IC_RELRB(tmprb);
        //*** Do NOT check for NULL here.  NULL is checked in the while().
        tmprb=sds_tblnext("UCS"/*DNT*/,0);
    }

    bail:

    if (oldDimZin != 0) {
        dimzinbuf.resval.rint = oldDimZin;
        sds_setvar("DIMZIN"/*DNT*/,&dimzinbuf);
    }

    IC_RELRB(tmprb);
    return(rc);
}

//==============================================================================================
long cmd_ucsworld(void)   { 
    RT ret=0;
    long rc=0;
    struct resbuf rb;

	if (RTNORM!=cmd_ucs_add2ll()){ rc=(-1L); goto bail; } 

    //set the vars
    rb.restype=RTSTR;
    rb.resval.rstring= ""/*DNT*/ ;
    if (sdsengine_setvar("UCSNAME"/*DNT*/,&rb)!=RTNORM) { rc=(-1L); goto bail; }                                            
    //	IC_FREE(rb.resval.rstring);      a bad idea

    rb.restype=RT3DPOINT;
	rb.resval.rpoint[0]=rb.resval.rpoint[1]=rb.resval.rpoint[2]=0.0;
    if (sdsengine_setvar("UCSORG"/*DNT*/,&rb)!=RTNORM) { rc=(-1L); goto bail; }                                            
	rb.resval.rpoint[0]=1.0;
    if (sdsengine_setvar("UCSXDIR"/*DNT*/,&rb)!=RTNORM) { rc=(-1L); goto bail; }       
	rb.resval.rpoint[0]=0.0;
	rb.resval.rpoint[1]=1.0;    
    if (sdsengine_setvar("UCSYDIR"/*DNT*/,&rb)!=RTNORM) { rc=(-1L); goto bail; }
	rb.restype=RTSHORT;
	rb.resval.rint=1;
    if (sds_setvar("WORLDUCS"/*DNT*/,&rb)!=RTNORM) { rc=(-1L); goto bail; }
	

bail:
    return(rc);
}

long cmd_ucs_objectdef(void)
	{
	struct resbuf *rbp1,rbwcs,rbucs,rbent, rbTmp;			// 1-58078
	sds_name ent,ss1;
	sds_point p0,p1,p2,p3,p4,p5,p6,ucsxdir,ucsydir,ucsorg;
	RT	ret;
	char gotpoint;
	long fl1,fl2;
	struct gra_view *view=SDS_CURGRVW;

	if (view==NULL)
		return(RTERROR);
	//NOTE: paints OUT icon.  Caller must repaint it
	rbp1=NULL;
	rbwcs.restype=rbucs.restype=RTSHORT;
	rbwcs.rbnext=rbucs.rbnext=rbent.rbnext=NULL;
	rbwcs.resval.rint=0;
	rbucs.resval.rint=1;

	rbp1=NULL;
	//if ((ret=sds_entsel("\nSelect entity for UCS definition: ",ent,p0))!=RTNORM) return(ret);
	gotpoint=0;
    if ((ret=sds_pmtssget(ResourceString(IDC_UCS__NSELECT_ENTITY_FOR_69, "\nSelect entity for UCS definition: " ) ,""/*DNT*/ ,&rbp1,NULL,NULL,ss1,1))!=RTNORM) 
		return(ret);
	if ((ret=sds_sslength(ss1,&fl1))!=RTNORM) 
		{
		sds_ssfree(ss1);
		return(ret);
		}
	if (fl1==0L) 
		return(RTNONE);

	for (fl2=0L;fl2<fl1;fl2++)
		{
		sds_ssname(ss1,fl2,ent);
		sds_redraw(ent,IC_REDRAW_DRAW);
		}
	sds_ssname(ss1,0L,ent);
	if (rbp1 && RTSTR==rbp1->restype && strsame(rbp1->resval.rstring,ResourceString(IDC_UCS_S_70, "S" ) ) && rbp1->rbnext && RT3DPOINT==rbp1->rbnext->restype)
		{
		ic_ptcpy(p0,rbp1->rbnext->resval.rpoint);
		gotpoint=1;
		}
	sds_ssfree(ss1);
	if (rbp1)
		{
		sds_relrb(rbp1);
		rbp1 = NULL;
		}

	if ((rbp1=sds_entget(ent))==NULL)
		return(RTERROR);
	cmd_DrawUcsIcon(NULL,NULL,1);//paint it out
	rbent.restype=RT3DPOINT;
	if (ic_assoc(rbp1,210)!=0)
		{
		rbent.resval.rpoint[0]=0.0;
		rbent.resval.rpoint[1]=0.0;
		rbent.resval.rpoint[2]=1.0;
		}
	else
		ic_ptcpy(rbent.resval.rpoint,ic_rbassoc->resval.rpoint);

	ic_assoc(rbp1,0);
	if (strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_LINE)))
		{
		ic_assoc(rbp1,10);
		ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
		ic_assoc(rbp1,11);
		ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
		if (1==gotpoint)
			{
			sds_trans(p1,&rbwcs,&rbucs,0,p5);
			sds_trans(p2,&rbwcs,&rbucs,0,p6);
			gr_ucs2rp(p0,p3,view);
			gr_ucs2rp(p5,p5,view);
			gr_ucs2rp(p6,p6,view);
			}
		if (0==gotpoint || cmd_dist2d(p3,p5)<cmd_dist2d(p3,p6))
			{
			//y-axis is defined as p1->p2
			ic_ptcpy(ucsorg,p1);
			p2[2]=p1[2];
			cmd_unit_vector(p1,p2,ucsxdir);
			}
		else
			{
			//y-axis defined as p2->p1
			ic_ptcpy(ucsorg,p2);
			p1[2]=p2[2];
			cmd_unit_vector(p2,p1,ucsxdir);
			}
		ic_crossproduct(rbent.resval.rpoint,ucsxdir,ucsydir);
		}
	else if (strsame(ic_rbassoc->resval.rstring,"ARC"/*DNT*/))
		{
		if (1==gotpoint)
			gra_ucs2rp(p0,p1,view);
		ic_assoc(rbp1,10);
		ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
		//define new origin for ucs
		sds_trans(p2,&rbent,&rbwcs,0,ucsorg);
		ic_assoc(rbp1,40);
		p3[0]=ic_rbassoc->resval.rreal;
		ic_assoc(rbp1,50);
		p3[1]=ic_rbassoc->resval.rreal;
		ic_assoc(rbp1,51);
		p3[2]=ic_rbassoc->resval.rreal;
		//using sds_polar slow, but I don't want
		//4 reals on stack to do it faster
		sds_polar(p2,p3[1],p3[0],p4);//startpt in ecs
		sds_polar(p2,p3[2],p3[0],p5);//endpt in ecs
		sds_trans(p4,&rbent,&rbucs,0,p2);//startpt in ucs
		sds_trans(p5,&rbent,&rbucs,0,p6);//endpt in ucs
		gra_ucs2rp(p2,p4,view);
		gra_ucs2rp(p6,p5,view);
		if (1==gotpoint && cmd_dist2d(p1,p4)>cmd_dist2d(p1,p5))
			{
			//use end angle
			//get unit vectors in ECS for x axis
			p4[0]=cos(p3[2]);
			p4[1]=sin(p3[2]);
			p4[2]=0.0;
			}
		else
			{
			//use start angle	
			//get unit vectors in ECS for x axis
			p4[0]=cos(p3[1]);
			p4[1]=sin(p3[1]);
			p4[2]=0.0;
			}
		//get unit vectors for y axis in ECS
		
		sds_trans(p4,&rbent,&rbwcs,1,ucsxdir);
		ic_crossproduct(rbent.resval.rpoint,ucsxdir,ucsydir);
		}
	else if (strsame(ic_rbassoc->resval.rstring,"CIRCLE"/*DNT*/))
		{
		ic_assoc(rbp1,10);
		ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
		sds_trans(p1,&rbent,&rbwcs,0,ucsorg);
		if (1==gotpoint)
			{
			sds_trans(p0,&rbucs,&rbent,0,p2);
			p5[0]=sds_angle(p1,p2);
			}
		else
			{
			struct resbuf rbsnapang;
			SDS_getvar(NULL,DB_QSNAPANG,&rbsnapang,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			p5[0]=rbsnapang.resval.rreal;
			}
		p3[0]=cos(p5[0]);
		p3[1]=sin(p5[0]);
		p3[2]=0.0;
		sds_trans(p3,&rbent,&rbwcs,1,ucsxdir);
		ic_crossproduct(rbent.resval.rpoint,ucsxdir,ucsydir);
		}
	else if (strsame(ic_rbassoc->resval.rstring,"POINT"/*DNT*/))
		{
		ic_assoc(rbp1,10);
		ic_ptcpy(ucsorg,ic_rbassoc->resval.rpoint);
		//ucsxdir & ucsydir are unchanged
		SDS_getvar(NULL,DB_QUCSXDIR,&rbTmp,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);		// 1-58078 this and next 3 lines
		ic_ptcpy(ucsxdir,rbTmp.resval.rpoint);
		SDS_getvar(NULL,DB_QUCSYDIR,&rbTmp,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(ucsydir,rbTmp.resval.rpoint);
		}
	else if (strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/))
		{
		ic_assoc(rbp1,70);
		if (ic_rbassoc->resval.rint&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)) 
			goto badent;
		//get extrusion direction for later use
		ic_ptcpy(p5,rbent.resval.rpoint);

		FreeResbufIfNotNull(&rbp1);
		if (sds_entnext_noxref(ent,ent)!=RTNORM)
			return(RTERROR);
		if ((rbp1=sds_entget(ent))==NULL)
			return(RTERROR);
		ic_assoc(rbp1,0);
		if (!strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/))
			goto badent;
		ic_assoc(rbp1,10);
		ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
		FreeResbufIfNotNull(&rbp1);
		if (sds_entnext_noxref(ent,ent)!=RTNORM)
			return(RTERROR);
		if ((rbp1=sds_entget(ent))==NULL)
			return(RTERROR);
		ic_assoc(rbp1,0);
		if (!strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/))
			goto badent;
		ic_assoc(rbp1,10);
		ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
		cmd_unit_vector(p1,p2,p3);
		FreeResbufIfNotNull(&rbp1);
		if ((rbp1=sds_buildlist(RT3DPOINT,p5,0))==NULL)
			return(RTERROR);
		sds_trans(p3,&rbent,&rbwcs,1,ucsxdir);
		sds_trans(p1,&rbent,&rbwcs,0,ucsorg);
		ic_crossproduct(rbent.resval.rpoint,ucsxdir,ucsydir);
		}
	else if (strsame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/))
		{
		db_lwpolyline *lwpl=(db_lwpolyline *)ent[0];
		ic_ptcpy(p5,rbent.resval.rpoint);

		lwpl->get_10(p1,0);
		lwpl->get_10(p2,1);
		cmd_unit_vector(p1,p2,p3);
		FreeResbufIfNotNull(&rbp1);
		if ((rbp1=sds_buildlist(RT3DPOINT,p5,0))==NULL)
			return(RTERROR);
		sds_trans(p3,&rbent,&rbwcs,1,ucsxdir);
		sds_trans(p1,&rbent,&rbwcs,0,ucsorg);
		ic_crossproduct(rbent.resval.rpoint,ucsxdir,ucsydir);
		}
	else if (sds_wcmatch(ic_rbassoc->resval.rstring,"TRACE,SOLID"/*DNT*/)==RTNORM)
		{
		if (strsame(ic_rbassoc->resval.rstring,"TRACE"/*DNT*/))
			{
			//use midpts of 10-11 and 12-13 to define x-axis
			ic_assoc(rbp1,10);
			ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
			ic_assoc(rbp1,11);
			ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
			p3[0]=0.5*(p1[0]+p2[0]);
			p3[1]=0.5*(p1[1]+p2[1]);
			p3[2]=0.5*(p1[2]+p2[2]);
			ic_assoc(rbp1,12);
			ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
			ic_assoc(rbp1,13);
			ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
			p4[0]=0.5*(p1[0]+p2[0]);
			p4[1]=0.5*(p1[1]+p2[1]);
			p4[2]=0.5*(p1[2]+p2[2]);
			}
		else
			{
			ic_assoc(rbp1,10);
			ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
			ic_assoc(rbp1,11);
			ic_ptcpy(p4,ic_rbassoc->resval.rpoint);
			}
		cmd_unit_vector(p3,p4,p1);
		sds_trans(p1,&rbent,&rbwcs,1,ucsxdir);
		ic_crossproduct(rbent.resval.rpoint,ucsxdir,ucsydir);
		sds_trans(p3,&rbent,&rbwcs,0,ucsorg);
		}
	else if (strsame(ic_rbassoc->resval.rstring,"3DFACE"/*DNT*/))
		{
		ic_assoc(rbp1,10);
		ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
		ic_ptcpy(ucsorg,p1);
		ic_assoc(rbp1,11);
		ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
		if (ic_assoc(rbp1,13)!=0)ic_assoc(rbp1,12);
		ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
		cmd_unit_vector(p1,p2,ucsxdir);
		cmd_unit_vector(p1,p3,p4);
		//now cross ucsxdir w/p4 vector to get Z vector, then
		//cross Z w/X to get Y
		ic_crossproduct(ucsxdir,p4,p5);
		//now p5 is Z vector
		ic_crossproduct(p5,ucsxdir,ucsydir);
		}
	else if (sds_wcmatch(ic_rbassoc->resval.rstring,"SHAPE,TEXT,INSERT,ATTDEF"/*DNT*/)==RTNORM)
		{
		ic_assoc(rbp1,10);
		sds_trans(ic_rbassoc->resval.rpoint,&rbent,&rbwcs,0,ucsorg);
		ic_assoc(rbp1,50);
		p1[0]=cos(ic_rbassoc->resval.rreal);
		p1[1]=sin(ic_rbassoc->resval.rreal);
		p1[2]=0.0;
		sds_trans(p1,&rbent,&rbwcs,1,ucsxdir);
		ic_crossproduct(rbent.resval.rpoint,ucsxdir,ucsydir);
		}
	//BugZilla No. 78195; 10-06-2002 [
	else if(sds_wcmatch(ic_rbassoc->resval.rstring,"DIMENSION"/*DNT*/)==RTNORM)
		{
		ic_assoc(rbp1,11);
		sds_trans(ic_rbassoc->resval.rpoint,&rbent,&rbwcs,0,ucsorg);
				
		sds_point dimucs[4];
		db_initdimucs(dimucs,NULL,SDS_CURDWG,(db_handitem *)ent[0]) ;
		ic_ptcpy(ucsxdir,dimucs[1]);
		ic_ptcpy(ucsydir,dimucs[2]);
		}
	//BugZilla No. 78195; 10-06-2002 ]
	else
		{
		badent:
		FreeResbufIfNotNull(&rbp1);
		cmd_ErrorPrompt(CMD_ERR_CANTDEFUCS,0);
//		sds_printf("Cannot define coordinate system based upon selected entity.");
		return(RTERROR);
		}

	FreeResbufIfNotNull(&rbp1);

	if (RTNORM!=(ret=cmd_ucs_add2ll()))
		return(ret);
	rbwcs.restype=RT3DPOINT;
	ic_ptcpy(rbwcs.resval.rpoint,ucsorg);
	if ((ret=sdsengine_setvar("UCSORG"/*DNT*/,&rbwcs))!=RTNORM)
		return(ret);
	ic_ptcpy(rbwcs.resval.rpoint,ucsxdir);
	if ((ret=sdsengine_setvar("UCSXDIR"/*DNT*/,&rbwcs))!=RTNORM)
		return(ret);
	ic_ptcpy(rbwcs.resval.rpoint,ucsydir);
	if ((ret=sdsengine_setvar("UCSYDIR"/*DNT*/,&rbwcs))!=RTNORM)
		return(ret);

	return(ret);
}
	
