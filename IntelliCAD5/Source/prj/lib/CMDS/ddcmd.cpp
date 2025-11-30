/* G:\ICADDEV\PRJ\LIB\CMDS\DDCMD.CPP
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
sds_name SDS_AttEditName;
sds_name SDS_EditPropsSS;

// These functions call the table manager.

short cmd_ddattdef(void) {

	if(cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;
	
	ExecuteUIAction( ICAD_WNDACTION_ATTDEF );
    return(RTNORM);
}

short cmd_ddattext(void) {

	if(cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_ATTEXT );
    return(RTNORM);
}

short cmd_ddfilter(void) {	// This isn't DDSELECT anymore - it brings up the Filter dialog
							// which is still called ddselect.
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_DDSELECT );
    return(RTNORM);
}

short cmd_ddchprop(void)
	{

	if(cmd_iswindowopen() == RTERROR)
		{
		return RTERROR;
		}

	RT ret=RTNORM;

	if(RTNORM==(ret=sds_pmtssget(ResourceString(IDC_DDCMD__NSELECT_ENTITIES__0, "\nSelect entities to modify: " ),NULL,NULL,NULL,NULL,SDS_EditPropsSS,0)))
		{
//		if (RemoveHatchFromSS(SDS_EditPropsSS) == RTNIL) {
//			ret = RTNIL;
//		}

		ExecuteUIAction( ICAD_WNDACTION_CHPROP );
		}
    return(ret);
	}

short cmd_ddmodify(void) {

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	RT ret=RTNORM;

	if(RTNORM==(ret=sds_pmtssget(ResourceString(IDC_DDCMD__NSELECT_ENTITIES__0, "\nSelect entities to modify: " ),NULL,NULL,NULL,NULL,SDS_EditPropsSS,0))) {

//		if (RemoveHatchFromSS(SDS_EditPropsSS) == RTNIL) {
//			ret = RTNIL;
//		}

		ExecuteUIAction( ICAD_WNDACTION_MODIFY );
	}
    return(ret);
}

short cmd_ddinsert(void) {
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	cmd_InsideDDInsert=TRUE;
	ExecuteUIAction( ICAD_WNDACTION_INSERT );
	cmd_InsideDDInsert=FALSE;
    return(RTNORM);
}

short cmd_dducsp(void) {
	SDS_MultibleMode=0;
	struct resbuf rb;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(1&rb.resval.rint){
		cmd_ErrorPrompt(CMD_ERR_PERSPECTIVE,0);
		return(RTERROR);
	}

	ExecuteUIAction( ICAD_WNDACTION_UCSP );
    return(RTNORM);
}

short cmd_ddvpoint(void) {
	SDS_MultibleMode=0;
	struct resbuf rb;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_isinpspace() == RTERROR) return RTERROR;
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(1&rb.resval.rint){
		cmd_ErrorPrompt(CMD_ERR_PERSPECTIVE,0);
		return(RTERROR);
	}

	ExecuteUIAction( ICAD_WNDACTION_VPOINT );
    return(RTNORM);
}

short cmd_ddcolor(void) {

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	struct resbuf rb;
	RT	ret;
	int	col;
    char resstr[20];

	SDS_getvar(NULL,DB_QCECOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	col=ic_colornum(rb.resval.rstring);
	IC_FREE(rb.resval.rstring);
    ret=SDS_GetColorDialog(col,&col,0);
	if(ret!=RTNORM) return(ret);

	sprintf(resstr,"%d"/*DNT*/,col);
    rb.resval.rstring=resstr;

	//rb.resval.rstring=ic_colorstr(col,NULL);
	sds_setvar("CECOLOR"/*DNT*/,&rb);
    return(RTNORM);
}

short cmd_config(void)   {
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	SDS_MultibleMode=0;
	ExecuteUIAction( ICAD_WNDACTION_CONFIG );
	SDS_BroadcastRQToSDSApps(SDS_RQCFG);
	return(RTNORM);
}

short cmd_ddatte(void)   {

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	RT ret=RTNORM;
	long fl1,fl2;
	struct resbuf rb1,rb2;//,*pRb1;
	sds_name sset,etemp;

	//pRb1=NULL;
	//filter for INSERTs with ATTRIBUTES
	rb1.restype=0;
	rb2.restype=66;
	rb1.resval.rstring="INSERT"/*DNT*/;
	rb2.resval.rint=1;
	rb1.rbnext=&rb2;
	rb2.rbnext=NULL;

    for(;;){
		ret=sds_pmtssget(ResourceString(IDC_DDCMD__NSELECT_A_BLOCK_W_1, "\nSelect a block with attributes: " ),""/*DNT*/,NULL,NULL,&rb1,sset,1);
		if(RTCAN==ret)return(ret);
		if(ret!=RTNORM)	continue;
		sds_sslength(sset,&fl1);
		if(0L==fl1) {
			cmd_ErrorPrompt(CMD_ERR_ATTRIBUTE,0);
			continue;
		}
		else break;
	}
	//re-paint extra selected entities so user can see which one we're editing
	for(fl2=1L;fl2<=fl1 && RTNORM==sds_ssname(sset,fl2,etemp);fl2++){
		sds_redraw(etemp,IC_REDRAW_DRAW);
	}
	if(RTNORM==(ret=sds_ssname(sset,0L,SDS_AttEditName))){
		ExecuteUIAction( ICAD_WNDACTION_ATTE );
	}
	sds_redraw(SDS_AttEditName,IC_REDRAW_DRAW);
	sds_ssfree(sset);
	//if(pRb1!=NULL)sds_relrb(pRb1);
	return(ret);
}

short cmd_ddim(void) {
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_DIMVARS );
    return(RTNORM);
}

short cmd_ddlmodes(void) {
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_TBLMGR );
	return(RTNORM);
}

short cmd_ddrename(void) {
	SDS_MultibleMode=0;

	if(cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_TBLMGR );
    return(RTNORM);

}

short cmd_ddstyle(void) {
	SDS_MultibleMode=0;

	if(cmd_iswindowopen() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_TBLSTY );
    return(RTNORM);

}

short cmd_ddltype(void) {
	SDS_MultibleMode=0;

	if(cmd_iswindowopen() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_TBLLT );
    return(RTNORM);

}

short cmd_dducs(void) {
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_TBLUCS );
    return(RTNORM);
}

short cmd_ddview(void) {
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_TBLVIEW );
    return(RTNORM);
}

short cmd_ddblocks(void) {
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_TBLBLK );
    return(RTNORM);
}

short cmd_ddapp(void) {
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0);
    SDS_FreeEventList(0);

	//ExecuteUIAction( ICAD_WNDACTION_TBLAPP );
    return(RTNORM);
}


// All of these functions call the setconfig dialog
// they all need to set the correct prop sheet and dialog.

extern int svar_ChildDialog;

short cmd_ddsetvar(void) {
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_SETVAR );
    return(RTNORM);
}

short cmd_ddselect(void) {	// DDSELECT brings up the entity selection page of Drawing Settings
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	svar_ChildDialog=CI_ENTSEL;
	ExecuteUIAction( ICAD_WNDACTION_SETVAR );
    return(RTNORM);
}

short cmd_ddptype(void) {
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	svar_ChildDialog=EC_POINTS;
	ExecuteUIAction( ICAD_WNDACTION_SETVAR );
    return(RTNORM);
}

short cmd_ddunits(void)  {
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	svar_ChildDialog=AN_LINEAR;
	ExecuteUIAction( ICAD_WNDACTION_SETVAR );
	return(RTNORM);
}

short cmd_ddemodes(void) {
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	svar_ChildDialog=EC_ATTRIBUTE;
	ExecuteUIAction( ICAD_WNDACTION_SETVAR );
	return(RTNORM);
}

short cmd_ddgrips(void)  {
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	svar_ChildDialog=DV_NODES;
	ExecuteUIAction( ICAD_WNDACTION_SETVAR );
	return(RTNORM);
}

short cmd_ddrmodes(void) {
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	svar_ChildDialog=CI_GRID;
	ExecuteUIAction( ICAD_WNDACTION_SETVAR );
    return(RTNORM);
}

short cmd_ddosnap(void)  {
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	svar_ChildDialog=CI_SNAP;
	ExecuteUIAction( ICAD_WNDACTION_SETVAR );
    return(RTNORM);
}

short cmd_ddnew(void) {
	SDS_MultibleMode=0;
//4M Item:78->
// Command newwiz did not reeinit Lisp
/*
    ExecuteUIAction( ICAD_WNDACTION_DDNEW );
*/

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	struct resbuf rb;
	
	// Check for FileDia and Startup variables
	int iFileDia = 1;
	int iStartup = 1;

	if (SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
		iFileDia = rb.resval.rint;

	if (SDS_getvar(NULL,DB_QSTARTUP,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
		iStartup = rb.resval.rint;

		// If the FileDia or Startup variable are set then open the new drawing wizard, otherwise just open a blank new drawing
	if (iFileDia && iStartup)
		((CMainWindow*) SDS_CMainWindow)->NewDwgWizard();
	else
		cmd_new();


//<-4M Item:78
    return(RTNORM);
}



