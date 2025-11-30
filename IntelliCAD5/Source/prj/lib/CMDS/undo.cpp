/* G:\ICADDEV\PRJ\LIB\CMDS\UNDO.CPP
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

	// Entity names are cached under various circumstances and contain pointers to entities
	// Sicne undo may result in entities being freed, such caches need to be cleared
static void
VoidEntityReferences()
	{
	CIcadDoc	*currentDoc = SDS_CURDOC;
	
	if ( currentDoc )
		{
		if ( currentDoc->m_pGpakatt )
			currentDoc->m_pGpakatt->lastent[0] = currentDoc->m_pGpakatt->lastent[1] = NULL; 
		if ( currentDoc->m_pGpaktxt )
			currentDoc->m_pGpaktxt->lastent[0] = currentDoc->m_pGpaktxt->lastent[1] = NULL; 
		}
	}


short cmd_u(void) { 				// Prompts (none, actually), are frozen, 5/2/96.
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;			
	if (cmd_istransparent() == RTERROR) return RTERROR;	

	struct resbuf undoctl;
	SDS_getvar(NULL,DB_QUNDOCTL,&undoctl,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(!(undoctl.resval.rint & IC_UNDOCTL_ON)) {
		cmd_ErrorPrompt(CMD_ERR_UNDODISABLE,0);
		return(RTNORM);
	}

	VoidEntityReferences();
	SDS_UndoGroup(SDS_CURDWG, 1);
	return(RTNORM); 
}

short cmd_redo(void) {				// Prompts (none, actually), are frozen, 5/2/96. 
	SDS_MultibleMode=0;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;			
	if (cmd_istransparent() == RTERROR) return RTERROR;	

	struct resbuf undoctl;
	SDS_getvar(NULL,DB_QUNDOCTL,&undoctl,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(!(undoctl.resval.rint & IC_UNDOCTL_ON)) {
		cmd_ErrorPrompt(CMD_ERR_UNDODISABLE,0);
		return(RTNORM);
	}

	SDS_RedoGroup(SDS_CURDWG);
	return(RTNORM); 
}

short cmd_undo(void) {

	if (cmd_iswindowopen() == RTERROR) return RTERROR;			
	if (cmd_istransparent() == RTERROR) return RTERROR;	

    CString pmt;
    char fs1[IC_ACADBUF];
	RT ret;
	struct resbuf undoctl;

	for(;;) {
		SDS_getvar(NULL,DB_QUNDOCTL,&undoctl,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

		if(!(undoctl.resval.rint & IC_UNDOCTL_ON)) goto ctrl;
		
		if(undoctl.resval.rint & IC_UNDOCTL_ONECOMMANDUNDO) { 
			if(sds_initget(0,ResourceString(IDC_UNDO_INITGET_CONTROL_ONE_0, "Control One ~1 ~_Control ~_One ~_1" ))!=RTNORM) 
                return(RTERROR);
			pmt=ResourceString(IDC_UNDO__NUNDO___CONTROL__O_1, "\nUndo:  Control/<One>: ");
		} else {
			if(sds_initget(SDS_RSG_OTHER,ResourceString(IDC_UNDO_INITGET_MARK_BACK_T_2, "Mark Back_to_mark|Back ~ Begin_set|BEgin End_set|End ~ Control Auto ~Group ~Display ~_Mark ~_Back ~_ ~_BEgin ~_End ~_ ~_Control ~_Auto ~_Group ~_Display" ))!=RTNORM) 
                return(RTERROR);
            pmt=ResourceString(IDC_UNDO__NUNDO___MARK_BACK__3, "\nUndo:  Mark/Back to mark/BEgin set/End set/Control/Auto/<Number of steps to undo>: ");
		}
    
		ret=sds_getkword(pmt,fs1);
		pmt.Empty();	/*DG 2.10.2001*/// The following code can throw an exception, so empty the string here.
		if(ret==RTERROR || ret==RTCAN) return(ret);

		VoidEntityReferences();
		if(ret==RTNONE) {
			SDS_UndoGroup(SDS_CURDWG, 1);
			return(RTNORM);
		}

		if(strisame("AUTO"/*DNT*/,fs1)) {
    		if(sds_initget(0,ResourceString(IDC_UNDO_INITGET_AUTO_ON_ON__5, "Auto_on|ON Auto_off|OFf ~_ON ~_OFf" ))!=RTNORM) 
                return(RTERROR);
			if(undoctl.resval.rint & IC_UNDOCTL_AUTO) {
				ret=sds_getkword(ResourceString(IDC_UNDO__NOFF__ON____6, "\nOFF/<ON>: " ),fs1);
			}else{
				ret=sds_getkword(ResourceString(IDC_UNDO__NON__OFF____7, "\nON/<OFF>: " ),fs1);
			}
			if(ret==RTERROR || ret==RTCAN || ret==RTNONE) return(ret);
			if(strisame("ON"/*DNT*/,fs1))  
                undoctl.resval.rint|=IC_UNDOCTL_AUTO;
			else if(strisame("OFF"/*DNT*/,fs1)) 
                    if(undoctl.resval.rint & IC_UNDOCTL_AUTO) 
                        undoctl.resval.rint &= ~IC_UNDOCTL_AUTO;
			sds_setvar("UNDOCTL"/*DNT*/,&undoctl);
		} else if(strisame("CONTROL"/*DNT*/,fs1)) {
			ctrl: ;
    		if(sds_initget(0,ResourceString(IDC_UNDO_INITGET_NONE_ONE_A_11, "None One All ~1 ~_None ~_One ~_All ~_1" ))!=RTNORM) 
                return(RTERROR);
			ret=sds_getkword(ResourceString(IDC_UNDO__NSET_UNDO_CONTROL_12, "\nSet Undo control:  None/One/<All>: " ),fs1);
			if(ret==RTERROR || ret==RTCAN) return(ret);
			if((strisame("ALL"/*DNT*/,fs1)) || ret==RTNONE)  {
				if(undoctl.resval.rint & IC_UNDOCTL_ONECOMMANDUNDO) undoctl.resval.rint &= ~IC_UNDOCTL_ONECOMMANDUNDO;	// If set to one step only, remove that limitation.
				undoctl.resval.rint|=IC_UNDOCTL_ON;								// Enable undo (whether it was on or not).
			} else if(strisame("NONE"/*DNT*/,fs1)) {
				if(undoctl.resval.rint & IC_UNDOCTL_ON) undoctl.resval.rint &= ~IC_UNDOCTL_ON;	// If undo was enabled, disable it.
			} else if(strisame("ONE"/*DNT*/,fs1) || strisame("1" /*DNT*/,fs1)) { 
				if(!undoctl.resval.rint & IC_UNDOCTL_ON) undoctl.resval.rint|=IC_UNDOCTL_ON;	// If undo was disabled, enable it.
				undoctl.resval.rint|=IC_UNDOCTL_ONECOMMANDUNDO;								// Set to undo one step only.
			}
			sds_setvar("UNDOCTL"/*DNT*/,&undoctl);
		} else if(strisame("BEGIN"/*DNT*/,fs1) || strisame("GROUP"/*DNT*/,fs1)) {
			SDS_SetUndo(IC_UNDO_GROUP_BEGIN,NULL,(void*)1,NULL,SDS_CURDWG);
		} else if(strisame("END"/*DNT*/,fs1)) {
			SDS_SetUndo(IC_UNDO_GROUP_END,NULL,(void*)1,NULL,SDS_CURDWG);
		} else if(strisame("MARK"/*DNT*/,fs1)) {
			SDS_SetUndo(IC_UNDO_MARK,NULL,NULL,NULL,SDS_CURDWG);
		} else if(strisame("DISPLAY"/*DNT*/,fs1)) {
			SDS_UndoDisplay(SDS_CURDWG);
		} else if(strisame("BACK"/*DNT*/,fs1)) {
			ret = SDS_UndoBack( SDS_CURDWG);
			if ( ret )
				return ret;
		} else if(strisame("ONE"/*DNT*/,fs1)) {
			SDS_UndoGroup(SDS_CURDWG, 1);
		// We have to do this special case because bit 128 is set on initget().
		} else if(strisame("BACK, BEGIN"/*DNT*/,fs1)) {
			sds_printf(ResourceString(IDC_UNDO____PLEASE_SPECIFY__26, "-- Please specify... %s --" ),fs1);  
			continue;
		} else if(atoi(fs1) > 0) {
				SDS_UndoGroup(SDS_CURDWG, atoi(fs1));
		} else {
			cmd_ErrorPrompt(CMD_ERR_KWORD,0);
			continue;	/*DG - 31.5.2002*/// We must continue prompting after the message above.
		}
		break;
	}

	return(RTNORM); 
} 



