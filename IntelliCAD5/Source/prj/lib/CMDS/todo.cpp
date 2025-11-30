/* D:\ICADDEV\PRJ\LIB\CMDS\TODO.CPP
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
#include "IcadDoc.h"
#include "sdsapplication.h" /*DNT*/
#include <time.h>
#include <sys/timeb.h>
#include "commandatom.h" /*DNT*/
#include "lisp.h"/*DNT*/
#include "IcadRaster.h"/*DNT*/
#include "commandqueue.h" /*DNT*/
#include "paths.h"
#include "IcadView.h"
#include "IcadHelp.h"
#include "configure.h"
#include "IcadApp.h"
#include "LicensedComponents.h"
//Bugzilla No. 78017 ; date : 18-03-2002
#include "dimensions.h"/*DNT*/
#include "cmdsDimVars.h"/*DNT*/
#include "cmdsViewport.h"

bool  cmd_InsideRedrawAll = false;
bool  cmd_InsideRegen = false;
bool  cmd_InsideDDInsert;
BOOL  cmd_InsideCreateNewViewports;
bool  cmd_InsideWtile;
bool  cmd_InsideRedrawVPpaint;

//** Global SavePaths for sds_getfiled()
char *cmd_FontSavePath;
char *cmd_BMPSavePath;
char *cmd_InsertSavePath;

BOOL cmd_bOpenReadOnly;
BOOL cmd_bSaveWithPassWord;

//** Protos
short cmd_OpenMenu(BOOL bAppend);
short cmd_hide_and_shade(short hidemode,int dragmode);
short cmd_OpenNewDrawing(char *fname);
short cmd_SetOsmode(short mode);

// All of these commands need to free the event list because sds_command is now pushing
// things into the command stack and if they are not at least eaten the command stack will
// get screwed up!!

// TODO ACIS Functions.
//short cmd_extrude(void)	{ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
//short cmd_region(void)	{ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
//short cmd_boundary(void)	{ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
//short cmd_interfere(void) { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
//short cmd_massprop(void)	{ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_matlib(void)	  { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
//short cmd_revolve(void)	{ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_rpref(void)	  { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
//short cmd_section(void)	{ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_showmat(void)   { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
//short cmd_slice(void) 	{ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_soldraw(void)   { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_solprof(void)   { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_solview(void)   { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }

// TODO Images in and out funtions.
short cmd_dxbin(void)	  { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_saveimg(void)   { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_stlout(void)	  { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_tiffin(void)	  { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_wmfopts(void)   { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_wmfout(void)	  
{ 
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	return(cmd_mslide()); 
}
short cmd_gifin(void)	  { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_pcxin(void)	  { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }


// TODO Other misc. functions not supported yet.
short cmd_arx(void) 	  { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
//short cmd_filter(void)	{ return(cmd_ddfilter()); }
short cmd_filter(void)	  { return(cmd_select()); }
//EBATECH(CNBR) 2002/4/24 Move HATCHEDIT command to /Source/lib/CMDS/bhatch.cpp
//short cmd_hatchedit(void) { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_mvsetup(void)   { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_convert(void)   { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_edge(void)	  { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
//EBATECH(CNBR) 2002/4/24 Move SHAPE and LOAD commands to /Source/lib/CMDS/shape.cpp
//short cmd_load(void)	  { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }

// TODO Clipboard and OLE functions.
short cmd_copylink(void)  { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }

// TODO Image functions.
short cmd_imageadjust(void)   { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_imageattach(void)   { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_imageclip(void)	  { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_imageframe(void)	  { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
short cmd_imagequality(void)  { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }

// TODO Misc. commands.
short cmd_igesin(void)	 { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0); return(RTNORM); }
short cmd_igesout(void)  { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0); return(RTNORM); }
short cmd_treestat(void) { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0); return(RTNORM); }
short cmd_psdrag(void)	 { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0); return(RTNORM); }
short cmd_psfill(void)	 { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0); return(RTNORM); }
short cmd_psin(void)	 { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0); return(RTNORM); }
short cmd_psout(void)	 { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0); return(RTNORM); }
short cmd_filmroll(void) { cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0); return(RTNORM); }


short cmd_recover(void)  
{ 
	int bSaveToFile;
	struct sds_resbuf rb;
	SDS_getvar(NULL,DB_QAUDITCTL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	bSaveToFile = rb.resval.rint;

	short retVal;
	SDS_CMainWindow->m_bIsRecovering = (bSaveToFile == 1) ? 0x03 : 0x01;
    retVal = cmd_open();    
	SDS_CMainWindow->m_bIsRecovering = 0;

	return retVal;
}

// This used to be a command. It has been changed to an alias
// and removed from cmds.h too.
//short cmd_import(void)	{
//	  cmd_open();
//	  return(RTNORM);
//}

//this function will remove one link in a resbuf
void cmd_remove_rblink(struct resbuf **pRbbll,int rbtype){
	//the linked list must have at least two links;
	struct resbuf *pRb,*pRbLast;

	//if only one do nothing
	if((*pRbbll)->rbnext==NULL) return;
	//go to the rbtype
	for(pRbLast=pRb=(*pRbbll);pRb!=NULL&&pRb->restype!=rbtype;pRbLast=pRb,pRb=pRb->rbnext);
	if(pRbLast==(*pRbbll)){ //if the first link
		(*pRbbll)=pRb->rbnext;
		pRbLast->rbnext=NULL;
		IC_RELRB(pRbLast);
	}else{					  //else
		pRbLast->rbnext=pRb->rbnext;
		pRb->rbnext=NULL;
	}
}

short cmd_bmpout(void)	  {
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	char					  fs1[IC_ACADBUF];
	struct resbuf			  rb;

	if(SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);

	if( rb.resval.rint &&
		GetActiveCommandQueue()->IsEmpty() &&
		GetMenuQueue()->IsEmpty() &&
		!lsp_cmdhasmore) {
		dia: ;
		if(RTNORM!=sds_getfiled(ResourceString(IDC_TODO_SAVE_BITMAP_0, "Save Bitmap" ),cmd_BMPSavePath,ResourceString(IDC_TODO_BITMAP_FILE_BMP_1, "Bitmap File|bmp" ),5,&rb)) return(RTCAN);
		if(rb.restype==RTSTR) {
			strcpy(fs1,rb.resval.rstring); IC_FREE(rb.resval.rstring);
		}
		else { goto typeit; } // EBATECH(CNBR) 2002/4/30 Enable TYPE button
	} else {
		typeit:;
		if(RTNORM!=sds_getstring(1,ResourceString(IDC_TODO__NBITMAP_TO_CREATE__2, "\nBitmap to create: " ),fs1)) return(RTCAN);
		if(*fs1=='~'/*DNT*/) goto dia;
	}

	if(!*fs1) return(RTNONE);

	ChangeSlashToBackslashAndRemovePipe(fs1);

	if(strrchr(fs1,'\\'/*DNT*/)) {
		db_astrncpy(&cmd_BMPSavePath,fs1,strlen(fs1)+1);
		*(strrchr(cmd_BMPSavePath,'\\'/*DNT*/)+1)=0;
	}

	if(NULL==strrchr((strrchr(fs1,'\\'/*DNT*/) ? strrchr(fs1,'\\'/*DNT*/)+1 : fs1) ,'.'/*DNT*/))
		ic_setext(fs1,"bmp"/*DNT*/);

	char *pBuffer;
	long  bytes;
	if(SDS_CURVIEW && !SDS_CURVIEW->SaveBitmapToBuffer(&pBuffer,&bytes,FALSE)) {
		SDS_SaveBufferToFile(pBuffer,bytes,fs1);
		IC_FREE(pBuffer);
	}

	return(RTNORM);
}

//* Functions.
short cmd_SetOsmode(short mode) {	// Mode is passed by one of the 12 functions below. Each sets a different snap mode
	SDS_MultibleMode=0;
	struct resbuf rb;				// For getting, holding, and setting the system variable OSMODE

	sds_getvar("OSMODE"/*DNT*/,&rb);
//4M Elias Item:77->
	// clear osnap off flag
	rb.resval.rint &= (~IC_OSMODE_OFF);
//4M Elias Item:77<-
	if (mode==0) {							// If the user is setting Entity Snaps to None...
		rb.resval.rint=0;					//	 this kills off any bits that were set.
	} else {
		rb.resval.rint ^= mode;				// Toggles the "mode" bit.	If off, turns Quick off if it's the only other bit set

		if (rb.resval.rint == IC_OSMODE_QUICK)			//	and if Quick is the only other bit set,
			rb.resval.rint &= (~IC_OSMODE_QUICK);		//	turns it off.

		if (mode==IC_OSMODE_PLAN) {					// If user selects 2D...
			rb.resval.rint &= (~IC_OSMODE_INT);		//		makes sure 3D is off
			rb.resval.rint &= (~IC_OSMODE_APP);		//		makes sure Visible is off
		} else if (mode==IC_OSMODE_INT) {				// If user selects 3D...
			rb.resval.rint &= (~IC_OSMODE_APP);		//		makes sure Visible is off
			rb.resval.rint &= (~IC_OSMODE_PLAN);		//		makes sure 2D is off
		} else if (mode==IC_OSMODE_APP) {			// If user selects Visible...
			rb.resval.rint &= (~IC_OSMODE_INT);		//		makes sure 3D is off
			rb.resval.rint &= (~IC_OSMODE_PLAN);		//		makes sure 2D is off
		}
	}
	return(sds_setvar("OSMODE"/*DNT*/,&rb));
}


short cmd_none(void) {			 return(cmd_SetOsmode(IC_OSMODE_NONE));   }
short cmd_endpoint(void) {		 return(cmd_SetOsmode(IC_OSMODE_END));	  }
short cmd_midpoint(void) {		 return(cmd_SetOsmode(IC_OSMODE_MID));	  }
short cmd_center(void) {		 return(cmd_SetOsmode(IC_OSMODE_CENTER)); }
short cmd_node(void) {			 return(cmd_SetOsmode(IC_OSMODE_NODE));   }
short cmd_quadrant(void) {		 return(cmd_SetOsmode(IC_OSMODE_QUADRANT));   }
short cmd_insertion(void) { 	 return(cmd_SetOsmode(IC_OSMODE_INS));	  }
short cmd_intersection(void) {	 return(cmd_SetOsmode(IC_OSMODE_INT));	  }
short cmd_3dintersection(void) { return(cmd_SetOsmode(IC_OSMODE_INT));	  }
short cmd_2dintersection(void) { return(cmd_SetOsmode(IC_OSMODE_PLAN));   }
short cmd_planviewint(void) {	 return(cmd_SetOsmode(IC_OSMODE_PLAN));   }
short cmd_visible(void) {		 return(cmd_SetOsmode(IC_OSMODE_APP));	  }
short cmd_perpendicular(void) {  return(cmd_SetOsmode(IC_OSMODE_PERP));   }
short cmd_tangent(void) {		 return(cmd_SetOsmode(IC_OSMODE_TAN));	  }
short cmd_nearest(void) {		 return(cmd_SetOsmode(IC_OSMODE_NEAR));   }
short cmd_quick(void) { 		 return(cmd_SetOsmode(IC_OSMODE_QUICK));  }

short cmd_copyhist(void)  {
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_COPYHIST );
	return(RTNORM);
}

short cmd_pastespec(void)
{
	//*** Ole WndActions need to be "Post"ed instead of "Send"ed
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	PostUIAction( ICAD_WNDACTION_PASTESPEC,0L);
	return(RTNORM);
}

short cmd_olelinks(void)
{
	//*** Ole WndActions need to be "Post"ed instead of "Send"ed
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	PostUIAction( ICAD_WNDACTION_OLELINKS,0L);
	return(RTNORM);
}

short cmd_logfileoff(void){
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	SDS_MultibleMode=0;
	struct resbuf rb;
	rb.restype=RTSHORT;
	rb.resval.rint=0;
	SDS_setvar(NULL,DB_QLOGFILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	return(RTNORM);
}

short cmd_logfileon(void) {
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	SDS_MultibleMode=0;
	struct resbuf rb;
	rb.restype=RTSHORT;
	rb.resval.rint=1;
	SDS_setvar(NULL,DB_QLOGFILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	return(RTNORM);
}

short cmd_flipscr(void){
	struct resbuf rb1;

	SDS_MultibleMode=0;
	if(SDS_getvar(NULL,DB_QWNDLTEXT,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);

	// 0 = Hidden, 1 = Shown normal, 2 = Shown minimized, 3 = Shown maximized
	if(rb1.resval.rint==0 || rb1.resval.rint==2)	// If the text screen is hidden or minimized..
		return(sds_textscr());				//	  bring up the text screen.
	else 											// Otherwise...
		return(sds_graphscr());				//	  switch to the graphics screen.
}

short cmd_syswindows(void) {
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	RT ret;
	char fs1[IC_ACADBUF];

	if(sds_initget(0,ResourceString(IDC_TODO_INITGET_CASCADE_TIL_3, "Cascade Tile_horizontally|Horizontal Tile_vertically|Vertical Arrange_icons|Arrange ~_Cascade ~_Horizontal ~_Vertical ~_Arrange" ))!=RTNORM)
		return(-2);
	if((ret=sds_getkword(ResourceString(IDC_TODO__NARRANGE_MDI_WINDO_4, "\nArrange MDI windows:  Horizontally/Vertically/Arrange icons/<Cascade>: " ),fs1))==RTERROR) {
		return(ret);
	} else if(ret==RTCAN) {
		return(ret);
	} else if(ret==RTNONE) {
		return(cmd_wcascade());
	}

	if(strisame("CASCADE"/*DNT*/,fs1)) {
		return(cmd_wcascade());
	} if(strisame("HORIZONTAL"/*DNT*/,fs1)) {
		return(cmd_whtile());
	} else if(strisame("VERTICAL"/*DNT*/,fs1)) {
		return(cmd_wvtile());
	} else if(strisame("ARRANGE"/*DNT*/,fs1)) {
		return(cmd_wiarange());
	}
	return(RTERROR);
}



short cmd_gps(void)  {				// Commands are frozen, 5/2/96.

	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;
	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0);
	SDS_FreeEventList(0);
	return(RTNORM);
/*	  char fs1[IC_ACADBUF];
	short ret;
	short oldmode	= 0;
	short oldpaused = 0;
	struct resbuf rb;

 	SDS_getvar(NULL,DB_QGPSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	oldmode=rb.resval.rint;

	SDS_getvar(NULL,DB_QGPSPAUSED,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	oldpaused=rb.resval.rint;

	LOAD_COMMAND_OPTIONS_2(IDC_TODO_INITGET_GPS_ON_ON_G_9)
	if(sds_initget(0, LOADEDSTRING) != RTNORM)
		return(RTERROR);
	if (oldmode==0)(ret=sds_getkword(ResourceString(IDC_TODO__NGPS___ON_OFF_TOG_10, "\nGPS:  ON/OFF/Toggle/Live/PAuse/PLay/PRoperties/PToggle/Resume/Stop/<OFF>: " ),fs1));
		else	   (ret=sds_getkword(ResourceString(IDC_TODO__NGPS___ON_OFF_TOG_11, "\nGPS:  ON/OFF/Toggle/Live/PAuse/PLay/PRoperties/PToggle/Resume/Stop/<ON>: " ), fs1));
	if (ret == RTERROR) {
		return(ret);
	} else if(ret==RTCAN) {
		return(ret);
	} else if(ret==RTNONE) {
		return(ret);
	}

	ret=RTNORM;
	if(strsame(ResourceString(IDC_TODO_ON_12, "ON" ),fs1)) {
		rb.restype=RTSHORT;
		rb.resval.rint=1;
		sds_setvar(ResourceString(IDC_TODO_GPSMODE_13, "GPSMODE" )DNT,&rb);
	} if(strsame(ResourceString(IDC_TODO_PLAY_14, "PLay" ),fs1)) {
		*fs1=0;
		if(SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
		if( rb.resval.rint &&
		   GetActiveCommandQueue()->IsEmpty() && GetMenuQueue()->IsEmpty() && !lsp_cmdhasmore) {
			dia: ;
			if(RTNORM!=sds_getfiled(ResourceString(IDC_TODO_SELECT_GPS_FILE_TO_15, "Select GPS file to play" ),ResourceString(IDC_TODO__16, "" ),ResourceString(IDC_TODO_GPS_17, "gps" ),4,&rb)) return(RTCAN);
			if(rb.restype==RTSTR) {
				strcpy(fs1,rb.resval.rstring); free(rb.resval.rstring);
			}
			else { goto typeit; } // EBATECH(CNBR) 2002/4/30 Enable TYPE button
		} else {
			typeit: ;
			if(RTNORM!=sds_getstring(1,ResourceString(IDC_TODO__NGPS_FILE_TO_PLAY_18, "\nGPS file to play: " ),fs1)) return(RTCAN);
			if(*fs1=='~') goto dia;
		}
		ret=IcadGPSSend(GPS_PLAY,fs1);
		if(ret==RTNORM) {
			rb.restype=RTSTR;
			rb.resval.rstring=fs1;
			sds_setvar(ResourceString(IDC_TODO_GPSPLAYED_19, "GPSPLAYED" )DNT,&rb);
		}
	} else if(strsame(ResourceString(IDC_TODO_PAUSE_20, "PAuse" ),fs1)) {
		rb.restype=RTSHORT;
		rb.resval.rint=1;
		sds_setvar(ResourceString(IDC_TODO_GPSPAUSED_21, "GPSPAUSED" )DNT,&rb);
	} else if(strsame(ResourceString(IDC_TODO_RESUME_22, "Resume" ),fs1)) {
		rb.restype=RTSHORT;
		rb.resval.rint=0;
		sds_setvar(ResourceString(IDC_TODO_GPSPAUSED_21, "GPSPAUSED" )DNT,&rb);
	} else if(strsame(ResourceString(IDC_TODO_PTOGGLE_23, "PToggle" ),fs1)) {
		rb.restype=RTSHORT;
		if (oldpaused==0) rb.resval.rint=1;
			else rb.resval.rint=0;
		sds_setvar(ResourceString(IDC_TODO_GPSPAUSED_21, "GPSPAUSED" )DNT,&rb);
	} else if(strsame(ResourceString(IDC_TODO_STOP_24, "Stop" ),fs1)) {
		ret=IcadGPSSend(GPS_STOP,NULL);
		rb.restype=RTSTR;
		rb.resval.rstring=ResourceString(IDC_TODO__16, "" );
		sds_setvar(ResourceString(IDC_TODO_GPSPLAYED_19, "GPSPLAYED" )DNT,&rb);
	} else if(stricmp(ResourceString(IDC_TODO_OFF_25, "OFf" ),fs1)==0) {
		rb.restype=RTSHORT;
		rb.resval.rint=0;
		sds_setvar(ResourceString(IDC_TODO_GPSMODE_13, "GPSMODE" )DNT,&rb);
	} else if(strsame(ResourceString(IDC_TODO_TOGGLE_26, "Toggle" ),fs1)) {
		rb.restype=RTSHORT;
		if (oldmode==0) rb.resval.rint=1;
			else rb.resval.rint=0;
		sds_setvar(ResourceString(IDC_TODO_GPSMODE_13, "GPSMODE" )DNT,&rb);
	} else if(strsame(ResourceString(IDC_TODO_PROPERTIES_27, "PRoperties" ),fs1)) {
		ret=IcadGPSSend(GPS_PROP,NULL);
	}

	if(ret!=RTNORM) cmd_ErrorPrompt(CMD_ERR_GPS,0);

	return(ret); */
}

short cmd_CopyCutclip(int cutmode)	{
	RT ret;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if(RTNORM==(ret=sds_pmtssget(cutmode ? ResourceString(IDC_TODO__NSELECT_ENTITIES__28, "\nSelect entities to cut to the clipboard: " ):ResourceString(IDC_TODO__NSELECT_ENTITIES__29, "\nSelect entities to copy to the clipboard: " ),NULL,NULL,NULL,NULL,SDS_EditPropsSS,0)))
	{
		SDS_sscpy(SDS_CURDOC->m_pGripSelection,SDS_EditPropsSS);
		ExecuteUIAction( ICAD_WNDACTION_COPYCLIP );
		long fl1=0L;
		sds_name ename;
		while(RTNORM==sds_ssname(SDS_EditPropsSS,fl1,ename)) {
			if(cutmode) sds_entdel(ename);
			else   		sds_redraw(ename,IC_REDRAW_DRAW);
			++fl1;
		}
		sds_ssfree(SDS_CURDOC->m_pGripSelection);
	}
	sds_ssfree(SDS_EditPropsSS);
	return(RTNORM);
}

short cmd_vba(void)  {

	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::VBA))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		return(RTNORM);
	}

	SDS_MultibleMode=0;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	PostUIAction( ICAD_WNDACTION_VBA,0L);
	return(RTNORM);
}

short cmd_appload(void)  {
	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	SDS_MultibleMode=0;

	ExecuteUIAction( ICAD_WNDACTION_APPLOAD );
	return(RTNORM);
}

short cmd_spell(void)  {

#ifndef BUILD_WITH_WORD_SPELLCHECK
	#ifndef BUILD_WITH_INSO_SPELLCHECK
	{ cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); return(RTNORM); }
	#endif //BUILD_WITH_INSO_SPELLCHECK
#endif //BUILD_WITH_WORD_SPELLCHECK

	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::SPELLCHECK))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		return(RTNORM);
	}

	RT ret=RTNORM;

	if(RTNORM==(ret=sds_pmtssget(ResourceString(IDC_TODO__NSELECT_ENTITIES__30, "\nSelect entities to check spelling: " ),NULL,NULL,NULL,NULL,SDS_EditPropsSS,0)))
	{
		//*** NOTE: This call has to be a PostMessage, not a SendMessage.  This fixed a
		//*** problem I was having with CoCreateInstance() failing.  See Article ID: Q131056
		//*** in the Microsoft Knowledge Base.
		PostUIAction( ICAD_WNDACTION_SPELL,0L);
	}
	return(ret);
}

short cmd_pasteclip(void)  {
	SDS_MultibleMode=0;
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	// AG. These comments seem to be outdated, so I replaced PostUIAction() with ExecuteUIAction()
	//*** Ole WndActions need to be "Post"ed instead of "Send"ed
	//	PostUIAction( ICAD_WNDACTION_EXPASTE,0L);
	ExecuteUIAction( ICAD_WNDACTION_EXPASTE,0L );
	return(RTNORM);
}

short cmd_copyclip(void)  {
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	return(cmd_CopyCutclip(0));
}

short cmd_cutclip(void)  {
	SDS_MultibleMode=0;
	CIcadDoc* pDoc = SDS_CURDOC;
	if(RTERROR==cmd_iswindowopen() || NULL==pDoc)
	{
		return RTERROR;
	}

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	//*** Write the data to the clipboard.
	return(cmd_CopyCutclip(1));
}

// These commands are special commands because they are invoked and
// they all require special handling.


short cmd_colordlg(void) {

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	COLORREF crColor=0;
	int retcol,mode=0;
	struct resbuf *ab=NULL;

	ab=sds_getargs();
	if(ab!=NULL) {
		if(ab->restype==RTSHORT){
			//Modified SAU 18/04/00  [
			//Reason:(Bug Fixed)acad_colordlg -9) should fail but doesn't
			if(ab->resval.rint<0 || ab->resval.rint>256 ){
				sds_retnil();
				cmd_ErrorPrompt(CMD_ERR_BADARGUMENT,0);
				return(RTERROR);
			}
			//Modified SAU 18/04/00  ]
			crColor=ab->resval.rint;
			if(ab->rbnext && ab->rbnext->restype==RTNIL){
				//Modified SAU 31/05/2000
				//mode=1;
				mode=6;
				if(crColor==0||crColor==256){
					sds_retnil();
					cmd_ErrorPrompt(CMD_ERR_BADARGUMENT,0);
					return(RTERROR);
				}
			}else{
				mode=0;
			}
		}else{
			sds_retnil();
			cmd_ErrorPrompt(CMD_ERR_BADARGUMENT,0);
			return(RTERROR);
		}
	}else{
		sds_retnil();
		cmd_ErrorPrompt(CMD_ERR_BADARGUMENT,0);
		return(RTERROR);
	}

	IC_RELRB(ab);

	if(SDS_GetColorDialog(crColor,&retcol,mode)==RTNORM) {
		sds_retint(retcol);
		return(RTNORM);
	} else {
		sds_retnil();
		return(RTERROR);
	}
}

short cmd_helpdlg(void)  {
	SDS_MultibleMode=0;
	char fs1[IC_ACADBUF];
	struct resbuf *ab;

	ab=sds_getargs();
	if(ab==NULL || ab->restype!=RTSTR) {
		sds_retnil();
		return(RTERROR);
	}
	if(sds_findfile(ab->resval.rstring,fs1)!=RTNORM) {
		cmd_ErrorPrompt(CMD_ERR_FINDHELP, 0, (void*)ab->resval.rstring);	/*DG - 30.5.2002*/// Added searched file name to the message.
		sds_retnil();
		return(RTERROR);
	}
	CMainWindow::IcadHelp( fs1,ICADHELP_CONTENTS,0);
	sds_rett();
	return(RTNORM);
}

// Normal command functions below.
short cmd_compile(void)  {

	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM);
	/* =======================================================
	  NOTE : Below code never execute feature reserved
	========================================================*/
	char fs1[IC_ACADBUF],fs2[IC_ACADBUF];
	struct resbuf rb;

	*fs1=0;
	if(SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);

	if( rb.resval.rint &&
		GetActiveCommandQueue()->IsEmpty() &&
		GetMenuQueue()->IsEmpty() &&
		!lsp_cmdhasmore) {
		dia: ;
		if(RTNORM!=sds_getfiled(ResourceString(IDC_TODO_FONT_FILE_34, "Font File" ),cmd_FontSavePath,ResourceString(IDC_TODO_FONT_FILE_SHP_35, "Font File|shp" ),4,&rb)) return(RTCAN);
		if(rb.restype==RTSTR) {
			strcpy(fs1,rb.resval.rstring); IC_FREE(rb.resval.rstring);
		}
		else { goto typeit; } // EBATECH(CNBR) 2002/4/30 Enable TYPE button
	} else {
		typeit: ;
		if(RTNORM!=sds_getstring(1,ResourceString(IDC_TODO__NFONT_FILENAME___36, "\nFont filename: " ),fs1)) return(RTCAN);
		if(*fs1=='~'/*DNT*/) goto dia;
	}

	if(*fs1) {
		ChangeSlashToBackslashAndRemovePipe(fs1);

		if(strrchr(fs1,'\\'/*DNT*/)) {
			db_astrncpy(&cmd_FontSavePath,fs1,strlen(fs1)+1);
			*(strrchr(cmd_FontSavePath,'\\'/*DNT*/)+1)=0;
		}

		strncpy(fs2,fs1,sizeof(fs2)-1);
		ic_setext(fs2,"fsh"/*DNT*/);
		// TODO compile font file here.
	}

	return(RTNORM);
}

short cmd_about(void) {
	ExecuteUIAction( ICAD_WNDACTION_ABOUTDIA );
	return(RTNORM);
}

short cmd_viewctl(void) {
	SDS_MultibleMode=0;
	struct resbuf rb;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_isinpspace() == RTERROR) return RTERROR;
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	if(cmd_dview_target==0){
		//if not called from dview command to move camera and target, then
		//	perspective view is not allowed
		SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(1&rb.resval.rint){
			cmd_ErrorPrompt(CMD_ERR_PERSPECTIVE,0);
			return(RTERROR);
		}
	}

	ExecuteUIAction( ICAD_WNDACTION_VIEWCTL );
	return(RTNORM);
}

short cmd_customize(void) {
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	SDS_MultibleMode=0;
	ExecuteUIAction( ICAD_WNDACTION_CUSTOMIZE );
	return(RTNORM);
}

short cmd_help(void) {
	SDS_MultibleMode=0;
	char fs1[IC_ACADBUF],*fcp1;
	struct resbuf rb;

	if ( !SearchHelpFile( SDS_HLPFILE, fs1) ) {
		cmd_ErrorPrompt(CMD_ERR_FINDHELP, 0, (void*)SDS_HLPFILE);	/*DG - 30.5.2002*/// Added searched file name to the message.
		return(RTERROR);
	}

	SDS_getvar(NULL,DB_QCMDNAMES,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rstring==NULL) {
		CMainWindow::IcadHelp( fs1,ICADHELP_FINDER,0);
		return(RTNORM);
	}
	fcp1=rb.resval.rstring;
	unsigned int globalHelpStrLen = strlen("'HELP"/*DNT*/);

	if(strlen(fcp1)>globalHelpStrLen &&
	   strisame(fcp1+strlen(fcp1)-globalHelpStrLen,"'HELP"/*DNT*/))
		*(fcp1+strlen(fcp1)-globalHelpStrLen)=0;
	if(strrchr(fcp1,'\''/*DNT*/)) fcp1=strrchr(fcp1,'\''/*DNT*/)+1;
	if(strrchr(fcp1,'_'/*DNT*/)) fcp1=strrchr(fcp1,'_'/*DNT*/)+1;
//	  Need to find out if more than one key can point to the same block of
//	  help text. This will ultimately send the english version of the command to
//	  help. Will I need to look up the localized name before calling help???
//	  HELP!!

	if(strisame(fcp1,ResourceString(IDC_TODO_HELP_37, "HELP" )) ||
	   strisame(fcp1,"?"/*DNT*/) || strisame(fcp1,"_?"/*DNT*/) || *fcp1==0) {
		CMainWindow::IcadHelp( fs1, ICADHELP_FINDER,0);
	} else if (_tcsnicmp(fcp1, "C:"/*DNT*/, 2) == 0){ 
		CMainWindow::IcadHelp( fs1,ICADHELP_KEY,(DWORD)fcp1);
	} else {
		CMainWindow::IcadHelp( fs1,ICADHELP_KEY,(DWORD)fcp1);
	}
	IC_FREE(rb.resval.rstring);
	return(RTNORM);
}

short cmd_undefine(void) {

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	class commandAtomObj *atom;
	TCHAR nm[IC_ACADBUF],name[IC_ACADBUF];

	if(RTNORM!=sds_getstring(0,ResourceString(IDC_TODO__NCOMMAND_TO_UNDEF_39, "\nCommand to undefine: " ),name))
		return(RTCAN);

	if (lsp_prepatomname(nm,name,sizeof(nm)-1,1))
		return -1;

	RT retVal = commandAtomList->Lookup(nm, (CObject*&)atom);

	// atom->id == -1 --- Predefined LISP primitive.
	// AutoCAD won't let one undefine a LISP primitive. We could very easily, but is it a
	// feature ???
	if (!retVal || atom->id == -1) {
		cmd_ErrorPrompt(CMD_ERR_UNRECOGCMD,0);
		return(RTERROR);
	}

	// Now remove them as their 'defined' name and rename them using
	// the (.) prepended 'undefined' name as in 'LINE' - defined, and
	// '.LINE' - undefined. Do this with the local and global name.
	// That way a user can undefine/redefine a command using either the
	// global name i.e. (LINE) or the local name i.e. (LINIE). Feature!
	CString undefinedAtomName = "."/*DNT*/;
	if ( atom->LName != NULL )
		{
		undefinedAtomName += atom->LName;
		commandAtomList->RemoveKey(atom->LName);
		commandAtomList->SetAt(undefinedAtomName, (CObject*&)atom);
		}

	undefinedAtomName = "."/*DNT*/;
	if ( atom->GName != NULL )
		{
		undefinedAtomName += atom->GName;
		commandAtomList->RemoveKey(atom->GName);
		commandAtomList->SetAt(undefinedAtomName, (CObject*&)atom);
		}


	return(RTNORM);
}

short cmd_redefine(void) {

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	TCHAR nm[IC_ACADBUF],name[IC_ACADBUF];
	class commandAtomObj *atom;

	if(RTNORM!=sds_getstring(0,ResourceString(IDC_TODO__NCOMMAND_TO_REDEF_40, "\nCommand to redefine: " ),name))
		return(RTCAN);

	if (lsp_prepatomname(nm,name,sizeof(nm)-1,1))
		return -1;

	CString undefinedAtomName = "."/*DNT*/;
	undefinedAtomName += nm;
	if (!commandAtomList->Lookup(undefinedAtomName, (CObject*&)atom)) {
		sds_printf(ResourceString(IDC_TODO_NOTUDEFINED_80,"\nThis command has not been undefined, or is invalid."));
		return(RTERROR);
	}

	// Now remove them as their 'undefined' name and rename them using
	// their local and global name as in'LINE' - defined, and
	// '.LINE' - undefined, redefine them.
	// That way a user can undefine/redefine a command using either the
	// global name i.e. (LINE) or the local name i.e. (LINIE). Feature!
	if ( atom->LName != NULL )
		{
		undefinedAtomName = "."/*DNT*/;
		undefinedAtomName += atom->LName;
		commandAtomList->RemoveKey(undefinedAtomName);
		commandAtomList->SetAt(atom->LName, (CObject*&)atom);
		}

	if ( atom->GName != NULL )
		{
		undefinedAtomName = "."/*DNT*/;
		undefinedAtomName += atom->GName;
		commandAtomList->RemoveKey(undefinedAtomName);
		commandAtomList->SetAt(atom->GName, (CObject*&)atom);
		}


	return(RTNORM);
}

short cmd_files(void) {
	SDS_MultibleMode=0;
	WinExec("explorer.exe"/*DNT*/,SW_SHOWDEFAULT);
	return(RTNORM);
}

short cmd_cal(void) {
	SDS_MultibleMode=0;
	WinExec("calc.exe"/*DNT*/,SW_SHOWDEFAULT);
	return(RTNORM);
}

short cmd_select(void) {

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	sds_name ss,ename;
	RT ret;

	ret=sds_pmtssget(ResourceString(IDC_TODO__NSELECT_ENTITIES__41, "\nSelect entities to include in set: " ),NULL,NULL,NULL,NULL,ss,0);

	struct resbuf rb;
	SDS_getvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint) {
		long fl1=0L;
		while(RTNORM==sds_ssname(ss,fl1,ename)) {
			sds_redraw(ename,IC_REDRAW_DRAW);
			++fl1;
		}
	}
	sds_ssfree(ss);
	return(ret);
}

short cmd_selgrips(void) {

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	RT ret;
	sds_name ss,ename;

	ret=sds_pmtssget(ResourceString(IDC_TODO__NSELECT_ENTITIES__42, "\nSelect entities to display grips: " ),NULL,NULL,NULL,NULL,ss,0);
	if(ret!=RTNORM || SDS_CURDOC==NULL) return(ret);
	long fl1=0L;
	while(RTNORM==sds_ssname(ss,fl1,ename)) {
		SDS_ssadd(ename, SDS_CURDOC->m_pGripSelection, SDS_CURDOC->m_pGripSelection);
		SDS_AddGripNodes(SDS_CURDOC,ename,1);
		++fl1;
	}
//4M Item:28->
   SDS_DrawGripNodes(SDS_CURDOC);
//<-4M Item:28
	sds_ssfree(ss);
	return(RTNORM);
}

short cmd_point(void) {

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	char fs1[IC_ACADBUF];
	int multipt=0;
	RT	ret=RTNORM;
	sds_real dang;
	sds_point pt,pt2;
	struct resbuf *elist,rbucs,rbwcs,rbecs;

	SDS_getvar(NULL,DB_QUCSXDIR,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SDS_getvar(NULL,DB_QUCSYDIR,&rbwcs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rbecs.resval.rpoint[0]=(rbucs.resval.rpoint[1]*rbwcs.resval.rpoint[2])-(rbwcs.resval.rpoint[1]*rbucs.resval.rpoint[2]);
	rbecs.resval.rpoint[1]=(rbucs.resval.rpoint[2]*rbwcs.resval.rpoint[0])-(rbwcs.resval.rpoint[2]*rbucs.resval.rpoint[0]);
	rbecs.resval.rpoint[2]=(rbucs.resval.rpoint[0]*rbwcs.resval.rpoint[1])-(rbwcs.resval.rpoint[0]*rbucs.resval.rpoint[1]);

	rbecs.restype=RT3DPOINT;
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;rbwcs.resval.rint=0;

	pt[0]=1.0;pt[1]=pt[2]=0.0;
	sds_trans(pt,&rbecs,&rbucs,1,pt);
	if(fabs(pt[1])>CMD_FUZZ || fabs(pt[0])>CMD_FUZZ)dang=atan2(pt[1],pt[0]);
	else dang=0.0;


	for( ;; ) {
		if(multipt)	{
			if(sds_initget(0,ResourceString(IDC_TODO_INITGET_POINT_SETT_43, "Point_Settings...|Settings ~ ` ~_Settings ~_" ))!=RTNORM)
				return(RTERROR);
			ret=sds_getpoint(NULL,ResourceString(IDC_TODO__NSETTINGS__LOCATI_44, "\nSettings/<Location of point>: " ),pt);
		} else {
			if(sds_initget(0,ResourceString(IDC_TODO_INITGET_POINT_SETT_45, "Point_Settings...|Settings ~ Multiple_points|Multiple ~ ` ~_Settings ~_ ~_Multiple ~_" ))!=RTNORM)
				return(RTERROR);
			ret=sds_getpoint(NULL,ResourceString(IDC_TODO__NSETTINGS_MULTIPL_46, "\nSettings/Multiple/<Location of point>: " ),pt);
		}

		if(ret==RTNORM) {
			/* Transform data from UCS to the entity's native system: */
			sds_trans(pt,&rbucs,&rbwcs,0,pt2);
			elist=sds_buildlist(RTDXF0,"POINT"/*DNT*/,10,pt2,50,dang,210,rbecs.resval.rpoint,0);
			ret=sds_entmake(elist);
			sds_relrb(elist);
			if(ret!=RTNORM)return(ret);
		} else if(ret==RTKWORD) {
			sds_getinput(fs1);
			if (strisame("MULTIPLE"/*DNT*/,fs1)) {
				multipt=1; continue;
			} else if (strisame("SETTINGS"/*DNT*/,fs1)) {
				cmd_ddptype(); continue;
			}
		} else break;

		if(!multipt) break;
	}
	return(ret);
}

short cmd_quit(void)	 {
	if (cmd_istransparent() == RTERROR) return RTERROR;
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	SDS_MultibleMode=0;
	if( GetActiveCommandQueue()->IsNotEmpty() ||
		GetMenuQueue()->IsNotEmpty() ||
		lsp_cmdhasmore) {
		RT ret;
		struct resbuf rb;
		char fs1[IC_ACADBUF];
		SDS_getvar(NULL,DB_QDBMOD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint) {
			if(sds_initget(0,ResourceString(IDC_TODO_INITGET_YES_NO_49, "Yes No ~_Yes ~_No" ))!=RTNORM)
				return(RTERROR);
			if(RTCAN==(ret=sds_getkword(ResourceString(IDC_TODO__NDISREGARD_CHANGE_50, "\nDisregard changes to drawing file? <N>: " ),fs1))) return(ret);
			if(ret==RTNORM){
				if(strisame("YES"/*DNT*/,fs1)) {
					SDS_CURDOC->SetModifiedFlag(FALSE);

					const char *szMenuToken = GetMenuQueue()->PeekString();
					const char *szCommandToken = GetActiveCommandQueue()->PeekString();

					if( (( szCommandToken != NULL ) &&
						 strsame( szCommandToken,"2"/*DNT*/))
					||
						( (szMenuToken != NULL) &&
						  strsame(szMenuToken,"2"/*DNT*/))
					  )
						{

						GetActiveCommandQueue()->AdvanceNext();

						// This is to deal with spaces in script file and menus
						if( GetActiveCommandQueue()->IsNotEmpty() &&
						   GetActiveCommandQueue()->PeekItem()->restype==RTSTR &&
						   GetActiveCommandQueue()->PeekItem()->resval.rstring &&
						   GetActiveCommandQueue()->PeekItem()->resval.rstring[0]==' '/*DNT*/ &&
						   GetActiveCommandQueue()->PeekItem()->resval.rstring[1]==0)
								{
								GetActiveCommandQueue()->AdvanceNext();
								}
						}


					if( GetActiveCommandQueue()->IsNotEmpty() ||
						GetMenuQueue()->IsNotEmpty() ||
						lsp_cmdhasmore)
						{
						return(cmd_open());
						}

//					SDS_BroadcastRQToSDSApps(SDS_RQQUIT); // AG. see comments at the bottom of the function
//					PostUIAction( ICAD_WNDACTION_EXIT,0L);
					ExecuteUIAction( ICAD_WNDACTION_EXIT,0L);
					return(RTNORM);
				} else if(strisame("NO"/*DNT*/,fs1)) {
					SDS_CMainWindow->m_pvWndAction=(void*)SDS_CURDOC;
					SDS_CURDOC->SetModifiedFlag(TRUE);

					sds_resbuf setgetrb;
					int filediaval;
					sds_getvar("FILEDIA"/*DNT*/, &setgetrb);
					filediaval = setgetrb.resval.rint;
					setgetrb.resval.rint = 0;  // Turn it off.
					sds_setvar("FILEDIA"/*DNT*/,&setgetrb);
					cmd_SaveFunc(0, SDS_CURDOC);
					if (filediaval != 0) {
						setgetrb.resval.rint = filediaval; // reset it to what it was.
						sds_setvar("FILEDIA"/*DNT*/,&setgetrb);
					}

					const char *szMenuToken = GetMenuQueue()->PeekString();
					const char *szCommandToken = GetActiveCommandQueue()->PeekString();

					if( ( ( szCommandToken != NULL ) &&
						 strsame( szCommandToken,"2"/*DNT*/))
					||
						( (szMenuToken != NULL) &&
						strsame(szMenuToken,"2"/*DNT*/))
					  )
						{

						GetActiveCommandQueue()->AdvanceNext();

						// This is to deal with spaces in script file and menus
						if( GetActiveCommandQueue()->IsNotEmpty() &&
							GetActiveCommandQueue()->PeekItem()->restype==RTSTR &&
							GetActiveCommandQueue()->PeekItem()->resval.rstring &&
							GetActiveCommandQueue()->PeekItem()->resval.rstring[0]==' '/*DNT*/ &&
							GetActiveCommandQueue()->PeekItem()->resval.rstring[1]==0)
							{
							GetActiveCommandQueue()->AdvanceNext();
							}
						}

					if( GetActiveCommandQueue()->IsNotEmpty() ||
						GetMenuQueue()->IsNotEmpty() ||
						lsp_cmdhasmore)
						{
						return(cmd_open());
						}

//					SDS_BroadcastRQToSDSApps(SDS_RQQUIT);  // AG. see comments at the bottom of the function
//					PostUIAction( ICAD_WNDACTION_EXIT,0L);
					ExecuteUIAction( ICAD_WNDACTION_EXIT,0L);
					return(RTNORM);
				}
			}
		}
	}

//	AG. This is commented out to prevent from sending SDS_RQQUIT if we canceled it while replying
//	to the SAVE dialog for modified files
//	SDS_BroadcastRQToSDSApps(SDS_RQQUIT);
//	PostUIAction( ICAD_WNDACTION_EXIT,0L);

	// AG: use PostUIAction instead ExecuteUIAction to avoid RPC errors while
	// processing OLE actions in embedded documents
	PostUIAction( ICAD_WNDACTION_EXIT,0L);


	return(RTNORM);
}

short cmd_exit(void)  {
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	return cmd_quit();
}

short cmd_end(void) 	 {
/*	Removed at the request of users, who would type "end" trying to set the endpoint snap, and close the program
	instead.  In case you ever think of restoring this, here's why it was a problem:  If you start a drawing, and
	make change, it asks to if you want to save or cancel.	Cool.  If you open a drawing, and make no changes, it
	just exits, which is annoying, but OK.	Where it's a pain is if you've opened a drawing, messed it up making
	changes, then try to set the endpoint snap - it happily saves your messed up version for you, then exits.
*/

	cmd_ErrorPrompt(CMD_ERR_DONTUSEEND,0);
	SDS_FreeEventList(0);
	return(RTNORM);
}

int cmd_CreateNewViewports(db_drawing *flp) {

	if(flp==NULL) return(RTERROR);

	struct resbuf tilemode;
	if(SDS_getvar(NULL,DB_QTILEMODE,&tilemode,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	if(tilemode.resval.rint==0) return(RTNORM);

	cmd_InsideCreateNewViewports=TRUE;

	// Create all of the viewports for this drawing.
	// Count to see how many open ones there are.
	int OpenVPs=0;
	char *fcp1;
	sds_point ll,ur;
	db_handitem *hip;
	for (hip=flp->tblnext(DB_VPORTTABREC,1); hip!=NULL; hip=hip->next) {
		hip->get_2(&fcp1);
		if(hip->ret_deleted() || !strisame(fcp1,"*ACTIVE"/*DNT*/)) continue;
		++OpenVPs;
	}
	if(OpenVPs>1) {
		SDS_CMainWindow->m_wndViewClose=SDS_CURVIEW;
		ASSERT( SDS_CMainWindow->m_wndViewClose != NULL );
		ExecuteUIAction( ICAD_WNDACTION_MDIRESTORE );

		if(cmd_InsideOpenCommand || cmd_InsideNewCommand)
			{
			ExecuteUIAction( ICAD_WNDACTION_CALCVIEWPTS );
			}

		for (hip=flp->tblnext(DB_VPORTTABREC,1); hip!=NULL; hip=hip->next)
			{
			hip->get_2(&fcp1);
			if(hip->ret_deleted() || !strisame(fcp1,"*ACTIVE"/*DNT*/))
				continue;
			hip->get_10(ll); hip->get_11(ur);
			cmd_makewindowport(ll,ur,hip);
			if((--OpenVPs)==0)
				break;
			}
		ASSERT( SDS_CMainWindow->m_wndViewClose != NULL );
		if ( SDS_CMainWindow->m_wndViewClose != NULL )
			{
			SDS_CMainWindow->m_wndViewClose->m_bDontDelInTab=TRUE;
			ExecuteUIAction( ICAD_WNDACTION_CLOSEVIEW );
			}
	}

	cmd_InsideCreateNewViewports=FALSE;
	return(RTNORM);
}


short cmd_xbind(void)	   {cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
/*
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	char fs1[IC_ACADBUF];				// Unless they have to be changed for this to
	RT ret;							// actually work, then just let docs know about it.

	if(sds_initget(0,ResourceString(IDC_TODO_INITGET_BLOCKS_BLO_53, "Blocks|Block Dimension_styles|Dimstyle Layers|LAyer Linetypes|LType Text_styles|Style" ))!=RTNORM) return(-2);
	if((ret=sds_getkword(ResourceString(IDC_TODO__NXBIND___BLOCK_DI_54, "\nXBind:  Block/Dimstyle/LAyer/LType/Style: " ),fs1))==RTERROR) {
		return(ret);
	} else if(ret==RTCAN) {
		return(ret);
	} else if(ret==RTNONE) {
		return(ret);
	}

	if(strsame(ResourceString(IDC_TODO_BLOCK_55, "Block" ),fs1)) {
		if(RTNORM!=sds_getstring(1,ResourceString(IDC_TODO__NBLOCK_S__TO_BIND_56, "\nBlock(s) to bind: " ),fs1) || *fs1) return(ret);
		// TODO.
	} if(strsame(ResourceString(IDC_TODO_DIMSTYLE_57, "Dimstyle" ),fs1)) {
		if(RTNORM!=sds_getstring(1,ResourceString(IDC_TODO__NDIMENSION_STYLE__58, "\nDimension style(s) to bind: " ),fs1) || *fs1) return(ret);
		// TODO.
	} else if(strsame(ResourceString(IDC_TODO_LAYER_59, "LAyer" ),fs1)) {
		if(RTNORM!=sds_getstring(1,ResourceString(IDC_TODO__NLAYER_S__TO_BIND_60, "\nLayer(s) to bind: " ),fs1) || *fs1) return(ret);
		// TODO.
	} else if(strsame(ResourceString(IDC_TODO_LTYPE_61, "LType" ),fs1)) {
		if(RTNORM!=sds_getstring(1,ResourceString(IDC_TODO__NLINETYPES_S__TO__62, "\nLinetypes(s) to bind: " ),fs1) || *fs1) return(ret);
		// TODO.
	} else if(strsame(ResourceString(IDC_TODO_STYLE_63, "Style" ),fs1)) {
		if(RTNORM!=sds_getstring(1,ResourceString(IDC_TODO__NTEXT_STYLE_S__TO_64, "\nText style(s) to bind: " ),fs1) || *fs1) return(ret);
		// TODO.
	}
	return(RTNORM);
}
*/



short cmd_reinit(void){

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	SDS_MultibleMode=0;
	RT ret;

	ret = MessageBox(IcadSharedGlobals::GetMainHWND(), ResourceString(IDC_TODO______RE_LOAD_ICAD__65, "     Re-Load Icad.pgp?    " ), ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ), MB_OKCANCEL);
	
	if(ret==IDOK){
		return(SDS_ReadAlias("Icad.pgp"/*DNT*/));
	}
	return(RTCAN);
}

short cmd_readaliases(void) {

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	TCHAR name[IC_ACADBUF];

	if(RTNORM!=sds_getstring(1,"\nAlias file name: " ,name))
		return(RTCAN);

	int len = strlen (name);
	if (len == 0)
		return RTERROR;

	int rval = SDS_ReadAlias(name);

	/*
	if (rval == RTNORM)
	{
		MessageBox(SDS_hwnd,
				   "ReadAlias succesful", "IntelliCAD", MB_OKCANCEL);
	}
	else
	{
		MessageBox(SDS_hwnd,
				   "ReadAlias unsuccesful", "IntelliCAD", MB_OKCANCEL);
	}
	*/

	return rval;
}

short cmd_readaccelerators(void) {

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	TCHAR name[IC_ACADBUF];

	if(RTNORM!=sds_getstring(1,"\nAccelerator file name: " ,name))
		return(RTCAN);

	int len = strlen (name);
	if (len == 0)
		return RTERROR;

	int rval = SDS_ReadAccelerators(name);

	/*
	if (rval == RTNORM)
	{
		MessageBox(SDS_hwnd,
				   "ReadAccelerators succesful", "IntelliCAD", MB_OKCANCEL);
	}
	else
	{
		MessageBox(SDS_hwnd,
				   "ReadAccelerators unsuccesful", "IntelliCAD", MB_OKCANCEL);
	}
	*/

	return rval;
}

short cmd_vbaload(void) {
	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::VBA))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		return(RTNORM);
	}
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;
	PostUIAction( ICAD_WNDACTION_VBALOAD,0L);
	return(RTNORM);
}

short cmd_vbaunload(void) {
	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::VBA))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		return(RTNORM);
	}

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	PostUIAction( ICAD_WNDACTION_VBAUNLOAD,0L);
	return(RTNORM);
}

short cmd_vbarun(void) {
	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::VBA))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		return(RTNORM);
	}

	SDS_MultibleMode=0;
	SDS_CMainWindow->m_pvWndAction=NULL;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	PostUIAction( ICAD_WNDACTION_VBARUN,0L);
	return(RTNORM);
}

short cmd_dashvbarun(void) {
	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::VBA))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		return(RTNORM);
	}

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	TCHAR name[IC_ACADBUF];

	if(RTNORM!=sds_getstring(0,ResourceString(IDC_VBACMD_DASHVBARUN, "\nMacro name: " ),name))
		return(RTCAN);

	int len = strlen (name);
	if (len == 0)
		return RTERROR;

	char *newname = new char [len+1];
	strcpy (newname, name);

	SDS_CMainWindow->m_pvWndAction=(void*)newname;

	SDS_MultibleMode=0;
	PostUIAction( ICAD_WNDACTION_VBARUN,0L);
	return(RTNORM);
}

short cmd_addinrun(void) {

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	TCHAR name[IC_ACADBUF];

	if(RTNORM!=sds_getstring(0,ResourceString(IDC_VBACMD_ADDINRUN, "\nAdd-In Function name: " ),name))
		return(RTCAN);

	int len = strlen (name);
	if (len == 0)
		return RTERROR;

	char *newname = new char [len+1];
	strcpy (newname, name);

	SDS_CMainWindow->m_pvWndAction=(void*)newname;

	SDS_MultibleMode=0;
	PostUIAction( ICAD_WNDACTION_ADDINRUN,0L);
	return(RTNORM);
}

short cmd_vbasecurity(void) {
	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::VBA))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		return(RTNORM);
	}

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	PostUIAction( ICAD_WNDACTION_SECURITY,0L);
	return(RTNORM);
}

short cmd_plot(void) {
	SDS_MultibleMode=0;
	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_PRINT );
	return(RTNORM);
}

char *SDS_FormToRun;

short cmd_runform(void) {
//	char fs1[IC_ACADBUF];
//	RT ret;

	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0);
	SDS_FreeEventList(0);

//	  ret=sds_getstring(1,ResourceString(IDC_TODO__NFORM_TO_RUN___67, "\nForm to run: " ),fs1);
//	  SDS_FormToRun=fs1;
//	if(ret!=RTNORM) return(ret);

//	ExecuteUIAction( ICAD_WNDACTION_RUNFORM );
	return(RTNORM);
}

short cmd_editxdata(void) {

	if(cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	RT ret=RTNORM;
	sds_point pt;

	if(RTNORM==(ret=sds_entsel(ResourceString(IDC_TODO__NSELECT_ENTITY_TO_68, "\nSelect entity to create or edit Entity Data: " ),SDS_AttEditName,pt))) {
		ExecuteUIAction( ICAD_WNDACTION_EDITXDATA );
	}
	return(ret);
}

short cmd_setlayer(void)   {

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	RT ret=RTNORM;
	sds_name ename;
	sds_point pt;
	struct resbuf setgetrb;

	if(sds_initget(SDS_RSG_OTHER,NULL) != RTNORM) return(RTERROR);
	if(RTNORM!=(ret=sds_entsel(ResourceString(IDC_TODO__NSELECT_ENTITY_TO_69, "\nSelect entity to set the current layer: " ),ename,pt))) return(ret);
	struct sds_resbuf*elist=sds_entget(ename);
   	if(ic_assoc(elist,8)!=0) {
		sds_relrb(elist);
		return(RTERROR);
	}
	//*** Set the name of the curent layer.
	setgetrb.restype=RTSTR;
	setgetrb.resval.rstring=ic_rbassoc->resval.rstring;
	sds_setvar("CLAYER"/*DNT*/,&setgetrb);

	sds_relrb(elist);
	return(RTNORM);
}

//Bugzilla No. 78017 ; date : 18-03-2002 [
// This function first removes the eed of the destination entity (if any) and then
// copies the eed from the source entity (if any).
short MatchEEd(struct resbuf **elist,sds_name cpyename)
{
	short ret				= RTNORM;

	struct resbuf *temp		= NULL;
	struct resbuf *rbbeg	= NULL;
	struct resbuf *rbprv	= NULL;

	//Delete ACAD EED of the destination entity
	for(temp=*elist;temp!=NULL ;temp=temp->rbnext)
	{
		if ( temp->restype == AD_XD_APPOBJHANDLE && strisame(temp->resval.rstring,DIM_ACADAPP) )
			break;
	}
	// If ACAD application exists.
	if (temp !=NULL) 
	{		
		
		for(rbbeg=temp,temp=temp->rbnext;temp!=NULL;temp=temp->rbnext) 
		{
			if (temp->restype==AD_XD_APPOBJHANDLE)
			{
				rbprv->rbnext=NULL;
				sds_relrb(rbbeg->rbnext);
				rbbeg->rbnext=temp;
				rbbeg=rbbeg->rbnext;
			}
			else
				rbprv=temp;
		}
		if (rbbeg && rbbeg->rbnext) 
		{ 
			sds_relrb(rbbeg->rbnext); 
			rbbeg->rbnext=NULL;		
		}
	}


	//copy EED from source to destination entity
	struct resbuf appnam,* apprb	= NULL;
	struct resbuf *hold				= NULL;
	struct resbuf *rb				= NULL;
	
	appnam.restype = RTSTR;
	appnam.resval.rstring = DIM_ACADAPP;
	appnam.rbnext = NULL;
	
	if ((apprb=sds_entgetx(cpyename,&appnam))!=NULL) 
	{
		for(rb=apprb; rb!=NULL; rb=rb->rbnext) 
		{
			if (rb->restype==-3) 
			{
				break;
			}
		}
		if (rb!=NULL) 
		{
			hold=rb->rbnext;			
			for(temp=*elist; temp!=NULL; temp=temp->rbnext) 
			{
				if (temp->restype==-3) 
				{
					break;
				}
			}
			if (temp!=NULL) 
			{
				temp=temp->rbnext;
				sds_relrb(temp->rbnext);
			} 
			else 
			{	//app data doesn't exist on target entity, make new -3 and 1001 markers
				for(temp=*elist; temp->rbnext!=NULL; temp=temp->rbnext);
				temp->rbnext=sds_newrb(-3);
				temp=temp->rbnext;
				temp->rbnext=sds_newrb(AD_XD_APPOBJHANDLE);
				temp=temp->rbnext;
				temp->resval.rstring= new char [strlen(hold->resval.rstring)+1]; // It seems like sizeof is returning 1 less then the string length.
				strncpy(temp->resval.rstring,hold->resval.rstring,strlen(hold->resval.rstring)+1);
			}
			if(temp->restype == AD_XD_APPOBJHANDLE )
				temp->rbnext=hold->rbnext;
			else
				temp->rbnext=hold->rbnext->rbnext;			
			rb->rbnext=NULL;
		}
		IC_RELRB(apprb);
	}	
	return ret;
}
//Bugzilla No. 78017 ; date : 18-03-2002 ]
short cmd_matchprop(void)	{

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	RT ret=RTNORM;
	sds_name cpyename,ename,ss;
	sds_point pt;

	
	sds_real thickness=0;


	if(RTNORM!=(ret=sds_entsel(ResourceString(IDC_TODO__NSELECT_ENTITY_TO_70, "\nSelect entity to copy properties from: " ),cpyename,pt))) return(ret);
	sds_redraw(cpyename,IC_REDRAW_HILITE);

	struct sds_resbuf*elist=sds_entget(cpyename);	// Layer
	//Bugzilla No. 78048 ; date : 18-03-2002 [	

	//if(ic_assoc(elist,0)!=0) {						//entity name
	//	sds_relrb(elist);
	//	return(RTERROR);
	//}
	//char *entname=ic_rbassoc->resval.rstring;
	
	//Bugzilla No. 78048 ; date : 18-03-2002 ]

   	if(ic_assoc(elist,8)!=0) {
		sds_relrb(elist);
		return(RTERROR);
	}
	char *clayer=ic_rbassoc->resval.rstring;

   	if(ic_assoc(elist,62)!=0) {						// Color
		sds_relrb(elist);
		return(RTERROR);
	}
	int color=ic_rbassoc->resval.rint;

	if(ic_assoc(elist,370)!=0) {						// Lineweight
		sds_relrb(elist);
		return(RTERROR);
	}
	int iLineWeight=ic_rbassoc->resval.rint;

   	if(ic_assoc(elist,6)!=0) {						// Linetype
		sds_relrb(elist);
		return(RTERROR);
	}
	char *ltype=ic_rbassoc->resval.rstring;

   	if(ic_assoc(elist,48)!=0) {						// Linetype Scale Multiplier (default is 1)
		sds_relrb(elist);
		return(RTERROR);
	}
	sds_real ltscale=ic_rbassoc->resval.rreal;

	//Bugzilla No. 78048 ; date : 18-03-2002 [	
	bool bApplyThickness = true;	
	if(ic_assoc(elist,39)!=0) {		// Thickness  (default is 0)
		bApplyThickness = false;	
	}
	else
	{
		thickness=ic_rbassoc->resval.rreal;
	}
	//Bugzilla No. 78048 ; date : 18-03-2002 ]
	

//4M Spiros Item:146->
/*
   The code below copies the following DXF codes of a TEXT or an ATTDEF to another TEXT or ATTDEF entity via
   "matchprop" command:
   40 : Height
   41 : Relative X-scale factor
   50 : Rotation angle
   51 : Oblique angle
	7 : Text Style
   71 : Generation Flags

   The following DXF codes are not copied from one entity to another for compatibility reasons with AutoCAD
   70 : Attribute Flags (is found in ATTDEFs)
   72 : Horizontal Alignment(is found both in TEXTs and ATTDEFs)
   73 : Vertical Alignment (is found in TEXTs)
   74 : Vertical Alignment (is found in ATTDEFs)

   ATTRIBs do not participate in "matchprop" command because they are not main entities but are dependent on
   their INSERTs. The same occurs in AutoCAD.
*/
	sds_real height,xscale,rotation,oblique;
	char* textstyle;
	int generationflags;
   int FromType=((db_handitem *)cpyename[0])->ret_type();
	if (( FromType==DB_TEXT) || (FromType==DB_ATTDEF) )
	{
	   if(ic_assoc(elist,40)!=0) {	sds_relrb(elist); return(RTERROR); }	// Height
		height=ic_rbassoc->resval.rreal;
	   if(ic_assoc(elist,41)!=0) {	sds_relrb(elist); return(RTERROR); }	// Relative X-scale factor	(default is 0)
		xscale=ic_rbassoc->resval.rreal;
	   if(ic_assoc(elist,50)!=0) {	sds_relrb(elist); return(RTERROR); }	// Rotation angle			(default is 0)
		rotation=ic_rbassoc->resval.rreal;
		if(ic_assoc(elist,51)!=0) {	sds_relrb(elist); return(RTERROR); }	// Oblique angle			(default is 0)
		oblique=ic_rbassoc->resval.rreal;
   	if(ic_assoc(elist, 7)!=0) {	sds_relrb(elist); return(RTERROR); }	// Text-style				(default is STANDARD)
		textstyle=ic_rbassoc->resval.rstring;
   	if(ic_assoc(elist,71)!=0) {	sds_relrb(elist); return(RTERROR); }	// Generation flags			(default is 0)
		generationflags=ic_rbassoc->resval.rint;
	}

	//Bugzilla No. 78083  ; 08-04-2002 [	
	int splineformat = 0;
	int arrowheadflag = 0;
	if ( FromType ==DB_LEADER )
	{
		if(ic_assoc(elist,72)!=0) {	sds_relrb(elist); return(RTERROR); }	
		splineformat=ic_rbassoc->resval.rint;
		if(ic_assoc(elist,71)!=0) {	sds_relrb(elist); return(RTERROR); }	
		arrowheadflag=ic_rbassoc->resval.rint;
	}
	//Bugzilla No.78083  ; 08-04-2002 ]

	//Bugzilla No. 78049 ; date : 18-03-2002 [
	char* dimstyle = NULL;
	bool bMatchEED = false;
	if (( FromType==DB_DIMENSION) || (FromType==DB_LEADER) || ( FromType==DB_TOLERANCE) )
	{
		if(ic_assoc(elist, 3)!=0) {	sds_relrb(elist); return(RTERROR); }	// DimensionStyle
		dimstyle=ic_rbassoc->resval.rstring;
		bMatchEED = true;
	}
	//Bugzilla No. 78049 ; date : 18-03-2002 ]

//4M Spiros Item:146<-
	ret=sds_pmtssget(ResourceString(IDC_TODO__NSELECT_ENTITIES__71, "\nSelect entities to copy properties to: " ),NULL,NULL,NULL,NULL,ss,0);
	if(ret!=RTNORM) {
		sds_redraw(cpyename,IC_REDRAW_DRAW);
		return(ret);
	}

	struct resbuf *rbb,*trb;
	long fl1=0L;
	//Bugzilla No. 78083  ; 08-04-2002 [	
	int ToType = 0;
	bool TranferTextAttributes = false;
	bool TranferLeaderAttributes = false;
	bool TranferDimensionAttributes = false;
	struct resbuf appnam;
	appnam.restype = RTSTR;
	appnam.resval.rstring = DIM_ACADAPP;
	appnam.rbnext = NULL;
	//Bugzilla No.78083  ; 08-04-2002  ]
	while(RTNORM==sds_ssname(ss,fl1,ename)) {
//4M Spiros Item:146->
		ToType=((db_handitem *)ename[0])->ret_type();
		TranferTextAttributes=((FromType==DB_TEXT)||(FromType==DB_ATTDEF)) &&
								   ((ToType==DB_TEXT)||(ToType==DB_ATTDEF));

		//Bugzilla No.78083  ; 08-04-2002 [	
		TranferLeaderAttributes = (FromType==DB_LEADER) && (ToType==DB_LEADER);

		TranferDimensionAttributes = (( FromType==DB_DIMENSION) || (FromType==DB_LEADER) || ( FromType==DB_TOLERANCE) ) &&
									 (( ToType==DB_DIMENSION) || (ToType==DB_LEADER) || ( ToType==DB_TOLERANCE) );
		//Bugzilla No.78083  ; 08-04-2002  ]
//<-4M Spiros Item:146
		//Bugzilla No. 78017 ; date : 18-03-2002 [
		for(rbb=trb=sds_entgetx(ename,&appnam); trb!=NULL; trb=trb->rbnext) {
		//Bugzilla No. 78017 ; date : 18-03-2002 ]
			
			switch(trb->restype) {
				//Bugzilla No. 78049 ; date : 18-03-2002 [
				case 3:		// DimStyle
					//Bugzilla No. 78083  ; 08-04-2002 
					if ( TranferDimensionAttributes && dimstyle != NULL ) 
						db_astrncpy(&trb->resval.rstring,dimstyle,strlen(dimstyle)+1);
					break;
				//Bugzilla No. 78049 ; date : 18-03-2002 ]
				case 8:		// Layer
					db_astrncpy(&trb->resval.rstring,clayer,strlen(clayer)+1);
					break;
				case 62:	// Color
					trb->resval.rint=color;
					break;
				case 370:	// Lineweight
					trb->resval.rint=iLineWeight;
					break;
				case 6:		// Linetype
					db_astrncpy(&trb->resval.rstring,ltype,strlen(ltype)+1);
					break;
				case 48:	// Linetype Scale Multiplier
					trb->resval.rreal=ltscale;
					break;
				case 39:	// Thickness
					//Bugzilla No. 78048 ; date : 18-03-2002 
					if ( bApplyThickness ) trb->resval.rreal=thickness;
					break;
//4M Spiros Item:146->
//...Text entity cases
				case 40:
					if (TranferTextAttributes) trb->resval.rreal=height;
			   break;	// Height
				case 41:
					if (TranferTextAttributes) trb->resval.rreal=xscale;
			   break;	// Relative X-scale factor
				case 50:
					if (TranferTextAttributes) trb->resval.rreal=rotation;
			   break;	// Rotation angle
				case 51:
					if (TranferTextAttributes) trb->resval.rreal=oblique;
			   break;	// Oblique angle
				case  7:
					if (TranferTextAttributes) db_astrncpy(&trb->resval.rstring,textstyle,strlen(textstyle)+1);
			   break;	// Text-style
				case 71:
					if (TranferTextAttributes) trb->resval.rint=generationflags;
					//Bugzilla No.78083  ; 08-04-2002 
					if (TranferLeaderAttributes) trb->resval.rint= arrowheadflag;
			   break;	// Generation flags
				//Bugzilla No.78083  ; 08-04-2002 [
				case 72:
					if (TranferLeaderAttributes) trb->resval.rint= splineformat;
			   break;	// Format for spline entity.
			   //Bugzilla No.78083	; 08-04-2002 ]
//4M Spiros Item:146<-
			}
		}
		
		
		//Bugzilla No. 78017 ; date : 18-03-2002 
		if ( bMatchEED && (( ToType==DB_DIMENSION) || (ToType==DB_LEADER) || ( ToType==DB_TOLERANCE)) )
			ret = MatchEEd(&rbb,cpyename);
		
		

		sds_entmod(rbb);
		sds_relrb(rbb);
		++fl1;
	}

	sds_redraw(cpyename,IC_REDRAW_DRAW);
	sds_relrb(elist);
	sds_ssfree(ss);
	return(RTNORM);
}

short cmd_expdimsty(void) {
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_TBLDIMSTY );
	return(RTNORM);
}

short cmd_defxlink(void) {
	SDS_MultibleMode=0;

	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0);
	SDS_FreeEventList(0);

//	  if (cmd_iswindowopen() == RTERROR) return RTERROR;

//	ExecuteUIAction( ICAD_WNDACTION_DEFXLINK );
	return(RTNORM);
}

short cmd_multiple(void) {
	SDS_MultibleMode=2;
	return(RTNORM);
}

short cmd_viewres(void)  {

	//*** TODO This command is NOT FINISHED, It just prompts for input.
	// Currently there is no setvars to store the answers into!!!

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	char fs1[IC_ACADBUF],pmt[IC_ACADBUF];
	RT	ret;
	int	fi1;
	struct resbuf setgetrb;

	if(SDS_getvar(NULL,DB_QFASTZOOM,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { return(RTERROR); }
	if(sds_initget(0,ResourceString(IDC_TODO_INITGET_YES__FAST__72, "Yes._Fast_zooms|Yes No._Regular_zooms|No ~_Yes ~_No" ))!=RTNORM)
		return(RTERROR);
	sprintf(pmt,ResourceString(IDC_TODO__NDO_YOU_WANT_FAST_73, "\nDo you want fast zooms? <%s>: " ),setgetrb.resval.rint ? ResourceString(IDC_TODO_YES_51, "Yes" ):ResourceString(IDC_TODO_NO_52, "No" ));
	if((ret=sds_getkword(pmt,fs1))!=RTNORM && ret!=RTNONE) return(ret);
	if(ret!=RTNONE) {
		if(strisame(fs1,"YES"/*DNT*/))
			setgetrb.resval.rint=1;
		if(strisame(fs1,"NO"/*DNT*/))
			setgetrb.resval.rint=0;
		sds_setvar("FASTZOOM"/*DNT*/,&setgetrb);
	}

	while(1) {
		if(SDS_getvar(NULL,DB_QZOOMPERCENT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { return(RTERROR); }
		sprintf(pmt,ResourceString(IDC_TODO__NENTER_CURVE_DISP_74, "\nEnter curve display quality (range 1-20000) <%ld>: " ),setgetrb.resval.rint);
		if(sds_initget(SDS_RSG_NONEG|SDS_RSG_NOZERO,""/*DNT*/)!=RTNORM)
			return(RTERROR);
		if((ret=sds_getint(pmt,&fi1))!=RTNORM)
			return(ret);
		if(fi1>20000 || fi1<1) {
			cmd_ErrorPrompt(CMD_ERR_ZOOMPERCENT,0);
			continue;
		}
		break;
	}
	setgetrb.resval.rint=fi1;
	sds_setvar("ZOOMPERCENT"/*DNT*/,&setgetrb);

	return(cmd_regen());
}

short cmd_qprint(void) {
#ifdef CRIPPLE_TESTDRIVE
	if (CIcadApp::IsTestDrive())
	{
		MessageBox(IcadSharedGlobals::GetMainHWND(),
				   ResourceString(IDS_TESTDRIVE_WARNING, "Sorry Testdrive software cannot save files.  Please register to receive the full version.",),
				   ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD"),
				   MB_OK | MB_ICONEXCLAMATION);

		return RTERROR;
	}
#endif

	SDS_MultibleMode=0;
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	// Turn the cursor off.
	ExecuteUIAction( ICAD_WNDACTION_CURSOROFF );
	ExecuteUIAction( ICAD_WNDACTION_QPRINT );
	return(RTNORM);
}

short cmd_print(void) {
#ifdef CRIPPLE_TESTDRIVE
	if (CIcadApp::IsTestDrive())
	{
		MessageBox(IcadSharedGlobals::GetMainHWND(),
				   ResourceString(IDS_TESTDRIVE_WARNING, "Sorry Testdrive software cannot save files.  Please register to receive the full version.",),
				   ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD"),
				   MB_OK | MB_ICONEXCLAMATION);

		return RTERROR;
	}
#endif

	SDS_MultibleMode=0;
	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	// Turn the cursor off.
	ExecuteUIAction( ICAD_WNDACTION_CURSOROFF );
	ExecuteUIAction( ICAD_WNDACTION_PRINT );
	return(RTNORM);
}

short cmd_ppreview(void) {
	SDS_MultibleMode=0;
	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	// Turn the cursor off.
	ExecuteUIAction( ICAD_WNDACTION_CURSOROFF );
	ExecuteUIAction( ICAD_WNDACTION_PPREVIEW );
	return(RTNORM);
}

short cmd_psetup(void) {
	SDS_MultibleMode=0;
	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_PSETUP );
	return(RTNORM);
}

short cmd_wcascade(void) {
	SDS_MultibleMode=0;
	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	cmd_InsideWtile=TRUE;
	ExecuteUIAction( ICAD_WNDACTION_WCASCADE );
	cmd_InsideWtile=FALSE;
	return(RTNORM);
}

short cmd_whtile(void) {
	SDS_MultibleMode=0;
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	cmd_InsideWtile=TRUE;
	ExecuteUIAction( ICAD_WNDACTION_WHTILE );
	cmd_InsideWtile=FALSE;
	return(RTNORM);
}

short cmd_wiarange(void) {
	SDS_MultibleMode=0;
	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_WIARANGE );
	return(RTNORM);
}

short cmd_wvtile(void) {
	SDS_MultibleMode=0;
	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	cmd_InsideWtile=TRUE;
	ExecuteUIAction( ICAD_WNDACTION_WVTILE );
	cmd_InsideWtile=FALSE;
	return(RTNORM);
}

short cmd_wopen(void) {
	SDS_MultibleMode=0;
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	struct resbuf rb;
	if(SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	if(rb.resval.rint==0) {
		cmd_ErrorPrompt(CMD_ERR_TILEMODE1,0);
		return(RTERROR);
	}

	ExecuteUIAction( ICAD_WNDACTION_WOPEN );
	return(RTNORM);
}

short cmd_wsplit(void) {

	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0);
	SDS_FreeEventList(0);

//	if (cmd_iswindowopen() == RTERROR) return RTERROR;

//	  struct resbuf rb;
//	if(SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
//	if(rb.resval.rint==0) {
//		cmd_ErrorPrompt(CMD_ERR_TILEMODE1,0);
//		return(RTERROR);
//	}

//	ExecuteUIAction( ICAD_WNDACTION_WSPLIT );
	return(RTNORM);
}

short cmd_tipofday(void) {
	ExecuteUIAction( ICAD_WNDACTION_TIPOFDAY );
	return(RTNORM);
}

short cmd_query(void) {
	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0);
	SDS_FreeEventList(0);
	return(RTNORM);

//
//	if (cmd_iswindowopen() == RTERROR) return RTERROR;
//
//	sds_name ss,ename;
//	RT ret;
//
//	if(SDS_ExtdwgBeg==NULL) {
//		cmd_ErrorPrompt(CMD_ERR_NOEXTDWGS,0);
//		return(RTERROR);
//	}
//
//	SDS_ExternalQuery=1;
//	ret=sds_pmtssget("\nSelect entities to query: ",NULL,NULL,NULL,NULL,ss,0);
//
//	if(ret!=RTNORM) return(ret);
//
//	long fl1=0L;
//	while(RTNORM==sds_ssname(ss,fl1,ename)) {
//		sds_redraw(ename,IC_REDRAW_DRAW);
//		++fl1;
//	}
//	return(RTNORM);
}

short cmd_extopen(void) {

	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0);
	SDS_FreeEventList(0);

#if defined(SDS_NEWDB)
#else
	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_istransparent() == RTERROR) return RTERROR;

	char fs1[IC_ACADBUF];
	struct resbuf rb;
	db_drawing *flp;
	static char *SavePath;

	resbuf plrb;
	bool convertpl;

	SDS_getvar(NULL,DB_QPLINETYPE,&plrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (plrb.resval.rint==2)
		convertpl=true;
	else
		convertpl=false;

	*fs1=0;

	if(SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
	if( rb.resval.rint &&
		GetActiveCommandQueue()->IsEmpty() &&
		GetMenuQueue()->IsEmpty() &&
		!lsp_cmdhasmore) {
		dia: ;
		if(RTNORM!=sds_getfiled(ResourceString(IDC_TODO_OPEN_EXTERNAL_DRAW_75, "Open External Drawings" ),SavePath,ResourceString(IDC_TODO_STANDARD_DRAWING_F_76, "Standard Drawing File|dwg" ),4,&rb)) return(RTCAN);
		if(rb.restype==RTSTR) {
			strcpy(fs1,rb.resval.rstring); IC_FREE(rb.resval.rstring);
		}
		else { goto typeit; } // EBATECH(CNBR) 2002/4/30 Enable TYPE button
	} else {
		typeit: ;
		if(RTNORM!=sds_getstring(1,ResourceString(IDC_TODO__NEXTERNAL_DRAWING_77, "\nExternal drawings to open: " ),fs1)) return(RTCAN);
		if(*fs1=='~'/*DNT*/) goto dia;
		ic_setext(fs1,"DWG"/*DNT*/);
	}

	if(!*fs1) return(RTNONE);

	ChangeSlashToBackslashAndRemovePipe(fs1);

	if(strrchr(fs1,'\\'/*DNT*/)) {
		db_astrncpy(&SavePath,fs1,strlen(fs1)+1);
		*(strrchr(SavePath,'\\'/*DNT*/)+1)=0;
	}

	sds_printf(ResourceString(IDC_TODO__NOPENING_EXTERNAL_78, "\nOpening external drawing..." ));

	int fi1;
	if((flp=SDS_CMainWindow->m_pFileIO.drw_openfile(NULL,fs1,&fi1,convertpl))==NULL) {
		cmd_ErrorPrompt(CMD_ERR_OPENDWG,0);
		return(RTERROR);
	}

	rb.restype=0;
	SDS_getvar(NULL,DB_QVIEWCTR,&rb,SDS_CURDWG,NULL,NULL);
	SDS_setvar(NULL,DB_QVIEWCTR,&rb,flp,NULL,NULL,0);
	SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,NULL,NULL);
	SDS_setvar(NULL,DB_QVIEWSIZE,&rb,flp,NULL,NULL,0);
	SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,NULL,NULL);
	SDS_setvar(NULL,DB_QVIEWMODE,&rb,flp,NULL,NULL,0);
	SDS_getvar(NULL,DB_QVIEWDIR ,&rb,SDS_CURDWG,NULL,NULL);
	SDS_setvar(NULL,DB_QVIEWDIR,&rb,flp,NULL,NULL,0);

	SDS_BuildRegenList(flp,NULL,NULL,NULL);

/*D*/	if(SDS_ExtdwgBeg==NULL) {
/*D*/		SDS_ExtdwgBeg=SDS_ExtdwgCur=flp;
/*D*/	} else {
/*D*/		SDS_ExtdwgCur=SDS_ExtdwgCur->next=flp;
/*D*/	}
#endif

	return(RTNORM);
}

short cmd_openimage(void) {

	if (cmd_iswindowopen() == RTERROR) return RTERROR;


 cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);	return(RTNORM);
	return(RTNORM);
}

short cmd_insobject(void)		// We really are talking about "objects" here.	Don't change to "entities".
{

	if (cmd_iswindowopen() == RTERROR)
		{
		return RTERROR;
		}

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	//*** Insert OLE object.
	SDS_CMainWindow->m_strWndAction.Empty();
	if( GetActiveCommandQueue()->IsNotEmpty() ||
		GetMenuQueue()->IsNotEmpty() ||
		lsp_cmdhasmore)
		{

		char fs1[IC_ACADBUF];
		if(RTNORM!=sds_getstring(1,ResourceString(IDC_TODO__NFILE_TO_INSERT_A_79, "\nFile to insert as OLE object: " ),fs1))
			{
			return(RTCAN);
			}

		ChangeSlashToBackslashAndRemovePipe(fs1);

		SDS_CMainWindow->m_strWndAction=fs1;
		}

	PostUIAction( ICAD_WNDACTION_INSOBJ,0L);
	return(RTNORM);
	}


short cmd_iswindowopen(void) {						// *****************************************
													// 0 means no MDI window (drawing) is open.
	struct resbuf mdiwindow;						// Lets the user know if they are trying to
													// use a command that is only available
	SDS_getvar(NULL,DB_QWNDLMDI,&mdiwindow,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);				// when a drawing is open.
	if (mdiwindow.resval.rint==0 && (NULL!=SDS_CMainWindow &&
		SDS_CMainWindow->m_nOpenViews<=0)) {		// *****************************************
		cmd_ErrorPrompt(CMD_ERR_NODWGWINDOW,0);
		return(RTERROR);
	}
	return(RTNORM);
}

bool cmd_cmdecho_printf( int ce )
	{
	return (((GetActiveCommandQueue()->IsNotEmpty() || GetMenuQueue()->IsNotEmpty() || lsp_cmdhasmore) && ce) ||
			((GetActiveCommandQueue()->IsEmpty() && GetMenuQueue()->IsEmpty() && !lsp_cmdhasmore)));
	}


short
cmd_dwfout(void)
{
	RT ret = RTNORM;
	char fname[IC_ACADBUF];
	char prompt [IC_ACADBUF];
    sprintf(prompt,ResourceString(IDC_LAYOUT_TEMPLATEFILE2, "Enter file name: " ),fname);
    if ((ret=sds_getstring(1,prompt,fname))==RTERROR) 
	{
        ret==RTCAN;
	}
	else if (ret==RTCAN) 
	{
        ret==RTCAN;
	}   

	ic_setext(fname, ".dwf");

	// Check if the file exists in the specified location.
	if (!access(fname, 0))
	{
		char strMessage[IC_ACADBUF];
				
		sprintf (strMessage, 
				 ResourceString(IDC_SDS_GET__VBI_FILE_EXISTS__CMDLINE, "A file with the name <%s> already exists. Overwrite this File? <N>: "/*DNT*/),
				 fname
				);

		sds_initget(RSG_OTHER , ResourceString(IDC_SDS_GET__VBI_FILE_OVERWRITE, "Yes No ~_Yes ~_No"));
		if ((ret=sds_getkword(strMessage, prompt))==RTERROR)
		{
			return RTCAN;
		}
		else if (ret==RTCAN || ret==RTNONE) 
		{
			return RTCAN;
		}
		else if (ret==RTNORM) 
		{
			if (strisame(prompt, "NO"/*DNT*/)) 
			{
				return RTCAN;
			}
		}
	}

	int fileVersionAndFormat = 0x40;;
	fileVersionAndFormat |= 0x20;
	SDS_CMainWindow->m_pFileIO.drw_savefile(SDS_CURDWG,
				0L, 
				NULL, 
				const_cast<char*>(fname),
				5, 
				fileVersionAndFormat, 
				0, 
				12, 
				1, 
				1, 
				1, 
				TRUE);

	return ret;
	}

extern char *cmd_SlideSavePath;	// UGH!
extern int	SDS_UseExportSelSet;	// UGH!

#include "vmldrawdevice.h"	/*DNT*/

short
cmd_vmlout(void)
	{

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	char					  fs1[IC_ACADBUF];
	struct resbuf			  rb;

	if(SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);

	if( rb.resval.rint &&
		GetActiveCommandQueue()->IsEmpty() &&
		GetMenuQueue()->IsEmpty() &&
		!lsp_cmdhasmore) {
		dia: ;
		if(RTNORM!=sds_getfiled( "Create HTML file" ,cmd_SlideSavePath, "HTML File|htm"  ,5,&rb)) return(RTCAN);
		if(rb.restype==RTSTR) {
			strcpy(fs1,rb.resval.rstring); IC_FREE(rb.resval.rstring);
		}
		else { goto typeit; } // EBATECH(CNBR) 2002/4/30 Enable TYPE button
	} else {
		typeit: ;
		if (GetActiveCommandQueue()->IsNotEmpty())
			{
			SDS_UseExportSelSet=TRUE;
			}
		if(RTNORM!=sds_getstring(1, "\nHTML file to create: "  ,fs1)) return(RTCAN);
		if(*fs1=='~') goto dia;
	}

	if(!*fs1) return(RTNONE);

	ChangeSlashToBackslashAndRemovePipe(fs1);

	if(strrchr(fs1,'\\')) {
		db_astrncpy(&cmd_SlideSavePath,fs1,strlen(fs1)+1);
		*(strrchr(cmd_SlideSavePath,'\\')+1)=0;
	}

	if(NULL==strrchr((strrchr(fs1,'\\'/*DNT*/) ? strrchr(fs1,'\\'/*DNT*/)+1 : fs1) ,'.'/*DNT*/))
		ic_setext(fs1,"htm"/*DNT*/);

	VMLDrawDevice* pOutputDevice = new VMLDrawDevice(fs1);
	if(pOutputDevice != NULL)
	{
		CDrawDevice* pInitialDD = SDS_CURVIEW->m_pFrameBufferDrawDevice;
		SDS_CURVIEW->m_pFrameBufferDrawDevice = pOutputDevice;

		SDS_RedrawDrawing(SDS_CURDWG, SDS_CURVIEW, SDS_CURDOC, 1);

		delete pOutputDevice;
		SDS_CURVIEW->m_pFrameBufferDrawDevice = pInitialDD;
	}

	return( RTNORM );
}

//Modified Cybage SBD 06/04/2001 DD/MM/YYYY [
//Reason : Mail from Surya Sarda(suryas@cadopia.com) Dated 03/04/2001 DD/MM/YYYY.
short cmd_onweb(void){

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	int ret=(int)ShellExecute(NULL,"open",ResourceString(IDC_WEB_VISIO,"http://www.intellicad.org"),NULL,NULL,SW_NORMAL);

	if(ret>32 )
		return( RTNORM );
	else
		return(RTERROR);
}
//Modified Cybage SBD 06/04/2001 DD/MM/YYYY ]

//Bugzilla::78503; 28-04-03; [
short cmd_ieHelp(void){
	
	char szwinHelp[]= "winhlp32.exe";
	char szieHelpfile[]= "spimenab.hlp";
	char szPath[IC_ACADBUF] = {0};
	char fs1[IC_ACADBUF] = {0};

	if (cmd_istransparent() == RTERROR) return RTERROR;
	
	if(sds_findfile(szwinHelp,szPath)!=RTNORM) {
		cmd_ErrorPrompt(CMD_ERR_FINDHELP, 0, (void*)szwinHelp);	
		sds_retnil();
		return(RTERROR);
	}

	strcat(szPath," ");
	
	if(sds_findfile(szieHelpfile,fs1)!=RTNORM) {
		cmd_ErrorPrompt(CMD_ERR_FINDHELP, 0, (void*)szieHelpfile);	
		sds_retnil();
		return(RTERROR);
	}

	strcat(szPath,fs1);

	int ret=(int)ShellExecute(NULL,"open",fs1,NULL,NULL,SW_NORMAL);
	
	if(ret>32 )
		return( RTNORM );
	else
		return(RTERROR);

}
//Bugzilla::78503; 28-04-03; ]

short cmd_audit(void)	 
{ 
	int ret = RTERROR;
	bool fixErrors = true;
	char answer [IC_ACADBUF];

	int bSaveToFile;
	struct sds_resbuf rb;
	SDS_getvar(NULL,DB_QAUDITCTL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	bSaveToFile = rb.resval.rint;

	if(sds_initget(0,ResourceString(IDC_TODO_INITGET_YES_NO_49, "Yes No ~_Yes ~_No" ))!=RTNORM)
		return (RTERROR);
	if(RTCAN==(ret=sds_getkword(ResourceString(IDC_TODO_AUDIT_PROMPT, "\nDisregard changes to drawing file? <N>: " ),answer))) 
		return (RTCAN);

	if (ret == RTNONE || strisame(answer, "NO"))
		fixErrors = false;

	drw_readwrite *pAuditHandler = new drw_readwrite;
	ASSERT(pAuditHandler != NULL);

	db_drawing* fixedDWG = pAuditHandler->drw_auditfile(SDS_CURDWG, fixErrors, (bSaveToFile == 1) ? 0x03 : 0x01);

	if (fixedDWG != SDS_CURDWG)	// Errors were detected.
	{
		// Swap the main windows point to the current DWG to
		// this new drawing.
		db_drawing* deleteDwg = SDS_CMainWindow->m_pCurDoc->m_dbDrawing;
		SDS_CMainWindow->m_pCurDoc->m_dbDrawing = fixedDWG;
		// Now delete the one it used to point to.
		// Note CURCFG and CURSES stay the same.

		// delete the old view so we can replace it.
		delete SDS_CMainWindow->m_pCurView->m_pCurDwgView;
		SDS_CMainWindow->m_pCurView->m_pCurDwgView = NULL;

		// Now generate a new view to replace the one we deleted.
		gr_initview(fixedDWG,&SDS_CURCFG,&SDS_CMainWindow->m_pCurView->m_pCurDwgView,NULL);

		// Now we can delete the old drawing.
		delete deleteDwg;


		// DO NOT DELETE - set to NULL, it was deleted above when
		// m_pCurDwgView was deleted.
		SDS_CMainWindow->m_pCurView->m_pVportTabHip = NULL;

		// Use the active viewport or create a new one.
		if((int)fixedDWG->ret_ntabrecs(DB_VPORTTAB,0)>0) {
			SDS_CMainWindow->m_pCurView->m_pVportTabHip = fixedDWG->tblsearch(DB_VPORTTAB,ResourceString(IDC_ICADDOC__ACTIVE_2, "*ACTIVE" ),1);
		}
		else
		{
			SDS_CMainWindow->m_pCurView->m_pVportTabHip = cmd_createVPORT(fixedDWG);
		}
		SDS_VarToTab(SDS_CURDWG,SDS_CMainWindow->m_pCurView->m_pVportTabHip);

		SDS_CMainWindow->IsValidDrawing(fixedDWG);

		// Regenerate the new drawing.
		cmd_regen();
	}

	delete pAuditHandler;

	return(RTNORM); 
}

