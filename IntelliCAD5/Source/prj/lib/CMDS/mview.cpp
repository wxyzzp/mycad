/* G:\ICADDEV\PRJ\LIB\CMDS\MVIEW.CPP
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
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "IcadDoc.h"
#include "cmdsViewport.h"

#define	X	SDS_X
#define	Y	SDS_Y

bool  cmd_DontClearVport;

short cmd_mview(void) { 

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_istransparent() == RTERROR) return RTERROR;
	
    struct resbuf rb;
	if(SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	if(rb.resval.rint==1) {
		cmd_ErrorPrompt(CMD_ERR_TILEMODE0,0);
		return(RTERROR);
	}

	bool wasinms=FALSE;
	if(SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	if(rb.resval.rint>1) {
		wasinms=TRUE;
		cmd_pspace();
	}

	short	  swFit=0;			// Fit to screen?  1=Yes, Fit.   0=No, specify rectangle.
    int       nHowMany=1;		// For remembering how many viewports the user wanted
    int       nOrientation=1;	// For remembering which way they are arranged
								// 1=Horiz, 2=Vert, 3=Above, 4=Below, 5=Left, 6=Right
    RT	      ret=RTNORM;		// Return value
    RT	      retval=RTNORM;	// Return value
	long      fl1; 
	char      szCmdLine[IC_ACADBUF];		// Command line prompts and responses
	sds_name  ss,ename;			// Currently selected entities
    sds_point ptUser1,			// Point the user enters first
			  ptUser2,			// Point the user enters second
			  ptBoundingLL,		// Lower-left corner of bounding rectangle for all viewports
			  ptBoundingUR,		// Upper-right corner of bounding rectangle for all viewports	
              ptViewLL,			// Lower-left corner of view being created
		      ptViewUR,			// Upper-right corner of view being created
			  ptWindowXY,		// Given by SDS_getvar w/ DB_QSCREENSIZE
			  ptViewCenter;		// Given by SDS_getvar w/ DB_QVIEWCTR
	sds_real  rViewHeight,		// Given by SDS_getvar w/ DB_QVIEWSIZE
		      rViewWidth;		// Figured using rViewHeight & ptViewCenter, and aspect ratio from ptWindowXY
	struct sds_resbuf rbGetInfo;// For holding information when getting the above 4 things for Fitting to the window
	struct sds_resbuf *rbb,*trb;
	
    sds_initget(SDS_RSG_NONULL,ResourceString(IDC_MVIEW_INITGET_ON_ON_OFF__0, "On|ON Off|OFf ~Hideprint ~Hideplot ~ Fit_to_view|Fit Create_2_viewports|2 Create_3_viewports|3 Create_4_viewports|4 ~Restore ~_ON ~_OFf ~_Hideprint ~_Hideplot ~_ ~_Fit ~_2 ~_3 ~_4 ~_Restore" ));
	if((ret=(sds_getpoint(NULL,ResourceString(IDC_MVIEW__NVIEWPORTS___ON_O_1, "\nViewports:  ON/OFF/Fit/2/3/4/<First corner>: " ),ptUser1))) == RTCAN) { retval=ret; goto out; }

	if(ret==RTNORM)
	{
		if((ret = (sdsengine_getcorner( ptUser1, ResourceString(IDC_MVIEW__NOPPOSITE_CORNER__2, "\nOpposite corner: " ), ptUser2, true ))) != RTNORM)
		{
			retval=ret;
			goto out;
		}
		cmd_createVIEWPORT(SDS_CURDWG, ptUser1, ptUser2);
		retval=RTNORM; goto out;
	}

    if(ret==RTKWORD){
        sds_getinput(szCmdLine);
		if (strisame(szCmdLine,"ON"/*DNT*/)) {
			if (RTNORM!=(ret=sds_pmtssget(ResourceString(IDC_MVIEW__NSELECT_VIEWPORTS_4, "\nSelect viewports to turn on: " ),NULL,NULL,NULL,NULL,ss,0))) { retval=ret; goto out; }
			fl1=0L;
			while(RTNORM==sds_ssname(ss,fl1,ename)) {
				for(rbb=trb=sds_entget(ename); trb!=NULL; trb=trb->rbnext) {
					if(trb->restype==68) trb->resval.rint=2;
				}
				sds_entmod(rbb);
				sds_relrb(rbb);
				++fl1;
			}
			sds_ssfree(ss);
			retval=RTNORM; goto out;
		} else if (strisame(szCmdLine,"OFF"/*DNT*/)) {
			if (RTNORM!=(ret=sds_pmtssget(ResourceString(IDC_MVIEW__NSELECT_VIEWPORTS_6, "\nSelect viewports to turn off: " ),NULL,NULL,NULL,NULL,ss,0))) { retval=ret; goto out; }
			fl1=0L;
			while(RTNORM==sds_ssname(ss,fl1,ename)) {
				for(rbb=trb=sds_entget(ename); trb!=NULL; trb=trb->rbnext) {
					if(trb->restype==68) {
						ic_assoc(rbb,69);
						if(SDS_CURDOC->m_nLastMsVport==ic_rbassoc->resval.rint) SDS_CURDOC->m_nLastMsVport=0;
						trb->resval.rint=0;
					}
				}
				sds_entmod(rbb);
				sds_relrb(rbb);
				++fl1;
			}
			sds_ssfree(ss);
			retval=RTNORM; goto out;
		} else if (strisame(szCmdLine,"FIT"/*DNT*/)) {
			nHowMany=1;
			swFit=1;			// No more prompting, just fits to the screen, below.
		} else if (strisame(szCmdLine,"2"/*DNT*/)) {
			nHowMany=2;
			if(sds_initget(0,ResourceString(IDC_MVIEW_INITGET_HORIZONTAL_8, "Horizontal Vertical ~_Horizontal ~_Vertical" ))!=RTNORM) { 
                retval=RTERROR; 
                goto out; 
            }
			ret=sds_getkword(ResourceString(IDC_MVIEW__NTWO_VIEWPORTS____9, "\nTwo viewports:  Horizontal/<Vertical>: " ),szCmdLine);
			if (ret==RTERROR || ret==RTCAN) { retval=ret; goto out; }			// Allow RTNONE to select default (vertical)
			if (strisame(szCmdLine,"HORIZONTAL"/*DNT*/)) { 
				nOrientation=1;
			} else if ((strisame(szCmdLine,"VERTICAL" ))||(ret==RTNONE)) {
				nOrientation=2;	
			}					// Goes on to the "Fit/First corner" prompt below
		} else if (strisame(szCmdLine,"3"/*DNT*/)) {
			nHowMany=3;
			if(sds_initget(0,ResourceString(IDC_MVIEW_INITGET_HORIZONTA_12, "Horizontal Vertical ~ Above Below ~ Left Right ~_Horizontal ~_Vertical ~_ ~_Above ~_Below ~_ ~_Left ~_Right" ))!=RTNORM) { 
                retval=RTERROR; 
                goto out; 
            }
			ret=sds_getkword(ResourceString(IDC_MVIEW__NTHREE_VIEWPORTS_13, "\nThree viewports:  Horizontal/Vertical/Above/Below/Left/<Right>: " ),szCmdLine);
			if(ret==RTERROR || ret==RTCAN) { retval=ret; goto out;}			// Allow RTNONE to select default (right)
			if (strisame(szCmdLine,"HORIZONTAL"/*DNT*/)) {
				nOrientation=1;
			} else if (strisame(szCmdLine,"VERTICAL"/*DNT*/)) {
				nOrientation=2;
			} else if (strisame(szCmdLine,"ABOVE"/*DNT*/)) {
				nOrientation=3;
			} else if (strisame(szCmdLine,"BELOW"/*DNT*/)) {
				nOrientation=4;
			} else if (strisame(szCmdLine,"LEFT"/*DNT*/)) {
				nOrientation=5;
			} else if ((strisame(szCmdLine,"RIGHT"/*DNT*/))||(ret==RTNONE)) {
				nOrientation=6;
			}					// Goes on to the "Fit/First corner" prompt below
		} else if (strisame(szCmdLine,"4"/*DNT*/)) {
			nHowMany=4;			// Goes on to the "Fit/First corner" prompt below
		} else if (strisame(szCmdLine,"RESTORE"/*DNT*/)) {

			cmd_ErrorPrompt(CMD_ERR_OPTIONNOTSUP,0); retval=RTNORM; goto out;
			
			if (sds_initget(SDS_RSG_NONULL,ResourceString(IDC_MVIEW_INITGET___TO_LIST_19, "?_to_list|? ~_?" ))!=RTNORM) { 
                retval=RTERROR; 
                goto out; 
            }
			if (RTNORM!=(ret=sds_getkword(ResourceString(IDC_MVIEW__NRESTORE_VIEWPOR_20, "\nRestore viewports:  ? to list/Name of stored viewports: " ),szCmdLine))) return(ret);
			// TODO - not done yet

			sds_printf(ResourceString(IDC_MVIEW__NRESTORING_VIEWP_21, "\nRestoring viewports is under construction." ));
			retval=RTNORM; goto out;
		} else if(strisame(szCmdLine,"HIDEPRINT"/*DNT*/) || strisame(szCmdLine,"HIDEPLOT"/*DNT*/)) {

			cmd_ErrorPrompt(CMD_ERR_OPTIONNOTSUP,0); retval=RTNORM; goto out;

			if(RTNORM!=(ret=sds_pmtssget(ResourceString(IDC_MVIEW__NSELECT_VIEWPORT_24, "\nSelect viewports to remove hidden lines when printing: " ) ,NULL,NULL,NULL,NULL,ss,0))) return(ret);
			// TODO use ss here
			
			sds_printf(ResourceString(IDC_MVIEW__NHIDDEN_LINE_REM_25, "\nHidden line removal is under construction." ));
			sds_ssfree(ss);
			retval=RTNORM; goto out; 
		}
	}

	//This finds the bounding rectangle for multiple viewports, given any two points the user picks...
	if (swFit!=1) {	     // This gets two points from the user and changes them into UR/LL points for the bounding rectangle
		if (sds_initget(RSG_NONULL,ResourceString(IDC_MVIEW_INITGET_FIT_TO_SC_26, "Fit_to_screen|Fit ~_Fit" ))!=RTNORM) { 
            retval=RTERROR; 
            goto out; 
        }
		if ((ret=(sds_getpoint(NULL,ResourceString(IDC_MVIEW__NFIT_TO_SCREEN___27, "\nFit to screen/<First corner of bounding rectangle>: " ),ptUser1))) == RTCAN) { retval=ret; goto out; }
		if (ret==RTNORM)
		{
			if ((ret = (sdsengine_getcorner( ptUser1, ResourceString(IDC_MVIEW__NOPPOSITE_CORNER__2, "\nOpposite corner: " ),ptUser2, true ))) != RTNORM)
			{ 
                retval=ret; 
                goto out; 
            }
			// TODO - create an error message if points do not define a rectangle (same X or Y).
			if (ptUser1[X] < ptUser2[X]) {		// Finds out which point is furthest left
				ptBoundingLL[X]=ptUser1[X];
				ptBoundingUR[X]=ptUser2[X];
			} else {
				ptBoundingLL[X]=ptUser2[X];
				ptBoundingUR[X]=ptUser1[X];
			}
			if (ptUser1[Y] > ptUser2[Y]) {		// Finds out which point is highest
				ptBoundingUR[Y]=ptUser1[Y];
				ptBoundingLL[Y]=ptUser2[Y];
			} else {
				ptBoundingUR[Y]=ptUser2[Y];
				ptBoundingLL[Y]=ptUser1[Y];
			}
		} else if (ret==RTKWORD) {	
			swFit=1;
		}
	}

	// This makes the bounding rectangle the same as the MDI Window
	if (swFit==1) {					
		// Gets the aspect ratio of the MDI Window (in pixels)
		SDS_getvar(NULL,DB_QSCREENSIZE,&rbGetInfo,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(ptWindowXY,rbGetInfo.resval.rpoint);
		// Gets the center and height of the MDI Window (in drawing units)
		SDS_getvar(NULL,DB_QVIEWSIZE,&rbGetInfo,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		rViewHeight=rbGetInfo.resval.rreal;
		SDS_getvar(NULL,DB_QVIEWCTR,&rbGetInfo,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(ptViewCenter,rbGetInfo.resval.rpoint);
		// Figures the Lower-Left corner of the MDI Window we're fitting
		rViewWidth=(rViewHeight*(ptWindowXY[X]/ptWindowXY[Y]));
		ptBoundingLL[X]=(ptViewCenter[X]-(rViewWidth/2));
		ptBoundingLL[Y]=(ptViewCenter[Y]-(rViewHeight/2));
		// Figures the Upper-Right corner of the MDI Window we're fitting
		ptBoundingUR[X]=(ptViewCenter[X]+(rViewWidth/2));
		ptBoundingUR[Y]=(ptViewCenter[Y]+(rViewHeight/2));
	}
		
	// Now we have the bounding rect, # of vports, and orientation.  Now make the Viewports.
	if (nHowMany==1) {
		cmd_DontClearVport=TRUE;
		cmd_createVIEWPORT(SDS_CURDWG, ptBoundingLL, ptBoundingUR);
		cmd_DontClearVport=FALSE;
		retval=RTNORM; goto out;
	} else if (nHowMany==2) {														// 2 Viewports...
		if (nOrientation==1) {														// ...Horizonally
			cmd_DontClearVport=TRUE;
			ptViewLL[X]=ptBoundingLL[X];											//    Upper Viewport
			ptViewLL[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			cmd_createVIEWPORT(SDS_CURDWG, ptViewLL, ptBoundingUR);
			ptViewUR[X]=ptBoundingUR[X];											//    Lower Viewport
			ptViewUR[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			cmd_createVIEWPORT(SDS_CURDWG, ptBoundingLL, ptViewUR);
			cmd_DontClearVport=FALSE;
			retval=RTNORM; goto out;
		} else {																	// ...Vertically
			cmd_DontClearVport=TRUE;
			ptViewUR[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Left Viewport
			ptViewUR[Y]=ptBoundingUR[Y];
			cmd_createVIEWPORT(SDS_CURDWG, ptBoundingLL, ptViewUR);
			ptViewLL[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Right Viewport
			ptViewLL[Y]=ptBoundingLL[Y];
			cmd_createVIEWPORT(SDS_CURDWG, ptViewLL, ptBoundingUR);
			cmd_DontClearVport=FALSE;
			retval=RTNORM; goto out;
		}
	} else if (nHowMany==3) {														// 3 Viewports...
		if (nOrientation==1) {														// ...Horizonally
			cmd_DontClearVport=TRUE;
			ptViewLL[X]=ptBoundingLL[X];											//    Upper Viewport
			ptViewLL[Y]=ptBoundingLL[Y]+2*(((ptBoundingUR[Y]-ptBoundingLL[Y])/3));
			cmd_createVIEWPORT(SDS_CURDWG, ptViewLL, ptBoundingUR);
			ptViewLL[X]=ptBoundingLL[X];											//    Middle Viewport
			ptViewLL[Y]=ptBoundingLL[Y]+((ptBoundingUR[Y]-ptBoundingLL[Y])/3);
			ptViewUR[X]=ptBoundingUR[X];											//    Middle Viewport
			ptViewUR[Y]=ptBoundingLL[Y]+2*(((ptBoundingUR[Y]-ptBoundingLL[Y])/3));
			cmd_createVIEWPORT(SDS_CURDWG, ptViewLL, ptViewUR);
			ptViewUR[X]=ptBoundingUR[X];											//    Lower Viewport
			ptViewUR[Y]=ptBoundingLL[Y]+((ptBoundingUR[Y]-ptBoundingLL[Y])/3);
			cmd_createVIEWPORT(SDS_CURDWG, ptBoundingLL, ptViewUR);
			cmd_DontClearVport=FALSE;
			retval=RTNORM; goto out;
		} else if (nOrientation==2) {												// ...Vertically
			cmd_DontClearVport=TRUE;
			ptViewUR[X]=ptBoundingLL[X]+((ptBoundingUR[X]-ptBoundingLL[X])/3);		//    Left Viewport
			ptViewUR[Y]=ptBoundingUR[Y];
			cmd_createVIEWPORT(SDS_CURDWG, ptBoundingLL, ptViewUR);
			ptViewLL[X]=ptBoundingLL[X]+((ptBoundingUR[X]-ptBoundingLL[X])/3);		//    Center Viewport
			ptViewLL[Y]=ptBoundingLL[Y];
			ptViewUR[X]=ptBoundingLL[X]+2*(((ptBoundingUR[X]-ptBoundingLL[X])/3));
			ptViewUR[Y]=ptBoundingUR[Y];
			cmd_createVIEWPORT(SDS_CURDWG, ptViewLL, ptBoundingUR);
			ptViewLL[X]=ptBoundingLL[X]+2*(((ptBoundingUR[X]-ptBoundingLL[X])/3));	//    Right Viewport
			ptViewLL[Y]=ptBoundingLL[Y];
			cmd_createVIEWPORT(SDS_CURDWG, ptViewLL, ptBoundingUR);
			cmd_DontClearVport=FALSE;
			retval=RTNORM; goto out;
		} else if (nOrientation==3) {												// ...Large Above 2 small ones
			cmd_DontClearVport=TRUE;
			ptViewLL[X]=ptBoundingLL[X];											//    Upper Viewport (large one)
			ptViewLL[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			cmd_createVIEWPORT(SDS_CURDWG, ptViewLL, ptBoundingUR);
			ptViewUR[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Lower-Left Viewport
			ptViewUR[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			cmd_createVIEWPORT(SDS_CURDWG, ptBoundingLL, ptViewUR);
			ptViewLL[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Lower-Right Viewport
			ptViewLL[Y]=ptBoundingLL[Y];
			ptViewUR[X]=ptBoundingUR[X];
			ptViewUR[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			cmd_createVIEWPORT(SDS_CURDWG, ptViewLL, ptViewUR);
			cmd_DontClearVport=FALSE;
			retval=RTNORM; goto out;
		} else if (nOrientation==4) {												// ...Large Below 2 small ones
			cmd_DontClearVport=TRUE;
			ptViewLL[X]=ptBoundingLL[X];											//    Upper-Left Viewport
			ptViewLL[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			ptViewUR[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);
			ptViewUR[Y]=ptBoundingUR[Y];
			cmd_createVIEWPORT(SDS_CURDWG, ptViewLL, ptViewUR);
			ptViewLL[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Upper-Right Viewport
			ptViewLL[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			cmd_createVIEWPORT(SDS_CURDWG, ptViewLL, ptBoundingUR);
			ptViewUR[X]=ptBoundingUR[X];											//    Lower Viewport (large one)
			ptViewUR[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			cmd_createVIEWPORT(SDS_CURDWG, ptBoundingLL, ptViewUR);
			cmd_DontClearVport=FALSE;
			retval=RTNORM; goto out;
		} else if (nOrientation==5) {												// ...Large to left of small ones
			cmd_DontClearVport=TRUE;
			ptViewUR[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Left Viewport (large one)
			ptViewUR[Y]=ptBoundingUR[Y];
			cmd_createVIEWPORT(SDS_CURDWG, ptBoundingLL, ptViewUR);
			ptViewLL[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Upper-Right Viewport
			ptViewLL[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			cmd_createVIEWPORT(SDS_CURDWG, ptViewLL, ptBoundingUR);
			ptViewLL[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Lower-Right Viewport
			ptViewLL[Y]=ptBoundingLL[Y];
			ptViewUR[X]=ptBoundingUR[X];
			ptViewUR[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			cmd_createVIEWPORT(SDS_CURDWG, ptViewLL, ptViewUR);
			cmd_DontClearVport=FALSE;
			retval=RTNORM; goto out;
		} else if (nOrientation==6) {												// ...Large to right of small ones
			cmd_DontClearVport=TRUE;
			ptViewLL[X]=ptBoundingLL[X];											//    Upper-Left Viewport
			ptViewLL[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			ptViewUR[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);
			ptViewUR[Y]=ptBoundingUR[Y];
			cmd_createVIEWPORT(SDS_CURDWG, ptViewLL, ptViewUR);
			ptViewUR[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Lower-Left Viewport
			ptViewUR[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			cmd_createVIEWPORT(SDS_CURDWG, ptBoundingLL, ptViewUR);
			ptViewLL[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Right Viewport (large one)
			ptViewLL[Y]=ptBoundingLL[Y];
			cmd_createVIEWPORT(SDS_CURDWG, ptViewLL, ptBoundingUR);
			cmd_DontClearVport=FALSE;
			retval=RTNORM; goto out;
		}
	} else if (nHowMany==4) {														// 4 Viewports...
			cmd_DontClearVport=TRUE;
			ptViewLL[X]=ptBoundingLL[X];											//    Upper-Left Viewport
			ptViewLL[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			ptViewUR[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);
			ptViewUR[Y]=ptBoundingUR[Y];
			cmd_createVIEWPORT(SDS_CURDWG, ptViewLL, ptViewUR);
			ptViewLL[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Upper-Right Viewport
			ptViewLL[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			cmd_createVIEWPORT(SDS_CURDWG, ptViewLL, ptBoundingUR);
			ptViewUR[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Lower-Left Viewport
			ptViewUR[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			cmd_createVIEWPORT(SDS_CURDWG, ptBoundingLL, ptViewUR);
			ptViewLL[X]=((ptBoundingUR[X]+ptBoundingLL[X])/2);						//    Lower-Right Viewport
			ptViewLL[Y]=ptBoundingLL[Y];
			ptViewUR[X]=ptBoundingUR[X];
			ptViewUR[Y]=((ptBoundingUR[Y]+ptBoundingLL[Y])/2);
			cmd_createVIEWPORT(SDS_CURDWG, ptViewLL, ptViewUR);
			cmd_DontClearVport=FALSE;
			retval=RTNORM; goto out;
	} 
out: ;
	if(wasinms) cmd_mspace();

    return(retval); 
}

int cmd_makewindowport(sds_point pt1,sds_point pt2,db_handitem *TabEnt) {

	// if TabEnt==-1 a new table entry is created in the drawing.

	CRect *rect=&SDS_CMainWindow->m_rectMDIWin;

	SDS_CMainWindow->m_pViewTabEntryHip=TabEnt;
//	if(TabEnt) SDS_TabToVar(SDS_CURDWG,TabEnt);

	SDS_CMainWindow->m_rectCreateView = new CRect(
		(int)((rect->right-rect->left)*pt1[0]),
		(int)((rect->bottom-rect->top)*(1.0-pt2[1])),
		(int)((rect->right-rect->left)*pt2[0]),
		(int)((rect->bottom-rect->top)*(1.0-pt1[1])));
	ExecuteUIAction( ICAD_WNDACTION_WOPEN ); 
	delete SDS_CMainWindow->m_rectCreateView;
	SDS_CMainWindow->m_rectCreateView=NULL;

	return(RTNORM);
}



