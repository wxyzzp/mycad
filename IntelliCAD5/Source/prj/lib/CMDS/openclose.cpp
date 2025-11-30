/* D:\ICADDEV\PRJ\LIB\CMDS\OPENCLOSE.CPP
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
#include "sdsthread.h" /*DNT*/
#include "sdsapplication.h" /*DNT*/
#include "commandqueue.h" /*DNT*/
#include "IcadView.h"
#include "IcadApp.h"
#include "IcadDispID.h"

char *cmd_DwgSavePath;
bool  cmd_InsideOfCloseCommand;
BOOL  cmd_InsideOpenCommand,cmd_InsideNewCommand;

short cmd_OpenNewDrawing(char *fname) {

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	// Start a new window.
	if(fname!=NULL)	{
		SDS_CMainWindow->m_strOpenFname=fname;
		ExecuteUIAction( ICAD_WNDACTION_FILEOPEN );
	} else {
		ExecuteUIAction( ICAD_WNDACTION_FILENEW );
	}

	// Don't re-init LISP if new is from an external app
	//
	if ( SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() )
		{
		// Re-init the Lisp because we opened a new drawing.
		SDS_InitLisp();
		}

	cmd_CreateNewViewports(SDS_CURDWG);
	SDS_AutoLoad();

	//Fire the reinitialize event for Addins to reinitialize themselves
	//GetApp()->FireWorkspaceEvent (DISPID_WS_REINIT, NULL);
	ExecuteUIAction( ICAD_WNDACTION_VBAFIRE_REINIT );

    return(RTNORM);
}

short cmd_new(void)
	{

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

    //*** Open a new drawing.
	cmd_InsideNewCommand=TRUE;

	struct resbuf rb;
	char fs1[IC_ACADBUF],fs2[IC_ACADBUF];
	char *setbf=NULL;

//    if( GetActiveCommandQueue()->IsNotEmpty() ||
//		GetMenuQueue()->IsNotEmpty() ||
//		lsp_cmdhasmore)
		{
			if(RTCAN==sds_getstring(1,ResourceString(IDC_OPENCLOSE__NTEMPLATE_FIL_0, "\nEnter template file name or [. (for none)]: " ),fs1))
			{
				return(RTCAN);
			}
			if( *fs1!=0 && *fs1!='.'/*DNT*/)
			{
				bool foundit = false;

				if (ic_stristr(fs1, ".dwg") || ic_stristr(fs1, ".dwt"))
				{
					if(RTNORM==sds_findfile(fs1,fs2))
					{
						SDS_getvar(NULL,DB_QBASEFILE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						setbf=rb.resval.rstring;
						rb.resval.rstring=fs2;
						SDS_setvar(NULL,DB_QBASEFILE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
						foundit = true;
					}
				}
				else if (ic_stristr(fs1, ".") == NULL)
				{
					ic_setext(fs1,"dwt"/*DNT*/);
					if(RTNORM==sds_findfile(fs1,fs2))
					{
						SDS_getvar(NULL,DB_QBASEFILE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						setbf=rb.resval.rstring;
						rb.resval.rstring=fs2;
						SDS_setvar(NULL,DB_QBASEFILE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
						foundit = true;
					}

					ic_setext(fs1,"dwg"/*DNT*/);
					if(foundit == false && RTNORM==sds_findfile(fs1,fs2))
					{
						SDS_getvar(NULL,DB_QBASEFILE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						setbf=rb.resval.rstring;
						rb.resval.rstring=fs2;
						SDS_setvar(NULL,DB_QBASEFILE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
						foundit = true;
					}
				}

				if (foundit == false)
				{
					CString msg;
					msg.Format(ResourceString(IDC_ICADNEWWIZ_TEMPLATENOTFOUND, "\nTemplate drawing %s is not on file. Using defaults."), fs1), 
					sds_printf(msg);
				}
			}
		}

    RT ret=cmd_OpenNewDrawing(NULL);

	if(setbf)
		{
		rb.resval.rstring=setbf;
		SDS_setvar(NULL,DB_QBASEFILE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		IC_FREE(setbf);
		}

	cmd_InsideNewCommand=FALSE;
    return(ret);
	}

void cmd_OnOpenOK(HWND hDlg)
{
	HWND hItem = GetDlgItem(hDlg,IDC_PREVIEW_CHKREADONLY);
	if(hItem==NULL) return;
	int nReadOnlyMode = (int)SendMessage(hItem,BM_GETCHECK,0,0L);
	if(IsWindowEnabled(hItem) && BST_CHECKED==nReadOnlyMode)
	{
		cmd_bOpenReadOnly=TRUE;
	}
	else
	{
		cmd_bOpenReadOnly=FALSE;
	}
}

short cmd_open(void) {
	SDS_MultibleMode=0;
    char fs1[IC_ACADBUF],fs2[IC_ACADBUF];
    struct resbuf rb;

	if (cmd_istransparent() == RTERROR) return RTERROR;

    *fs1=0;

	cmd_bOpenReadOnly=FALSE;
    if(SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
    if( rb.resval.rint &&
		GetActiveCommandQueue()->IsEmpty() &&
		GetMenuQueue()->IsEmpty() &&
		!lsp_cmdhasmore) {
        dia: ;
        // EBATECH(CNBR) -[ 2002/4/24 Allow multiple file selection
		CString fileTypes;
		fileTypes = ResourceString(IDC_OPENCLOSE_STANDARD_DRAWI_2, "Standard Drawing File|dwg,Drawing Exchange Format|dxf");
		fileTypes += ",";
		fileTypes += ResourceString(IDC_OPENCLOSE_DWF_FILE, "Design Web Format|dwf");
		fileTypes += ",";
		fileTypes += ResourceString(IDC_OPENCLOSE_DWT_FILE, "Drawing Template|dwt");
			 
		CString title = ResourceString(IDC_OPENCLOSE_OPEN_DRAWING_1, "Open Drawing" );
		if (SDS_CMainWindow->m_bIsRecovering == 1)
		{
			title = ResourceString(IDC_OPENCLOSE_RECOVER_DRAWING_1, "Recover Drawing" );
		}
		if(RTNORM!=SDS_GetFiled(title,cmd_DwgSavePath,fileTypes,32768+4,&rb,MAKEINTRESOURCE(IDD_OPENSAVE_PREVIEW),cmd_OnOpenOK)) 
			return(RTCAN);
		if(rb.restype==RTSHORT){  // EBATECH(CNBR) 2002/4/30 Enable TYPE button
			goto typeit; 
		}
        else if(rb.restype==RTSTR) {
            strcpy(fs1,rb.resval.rstring); IC_FREE(rb.resval.rstring);
        }
        else if(rb.restype==RTLB){
		    struct resbuf *rbnames=NULL;
			RT ret;
			for(ret=RTNORM, rbnames = &rb ; rbnames != NULL ; rbnames=rbnames->rbnext){
				if( rbnames->restype == RTSTR ){
					strcpy(fs1,rbnames->resval.rstring);

					cmd_InsideOpenCommand=TRUE;
					ret=cmd_OpenNewDrawing(fs1);
					cmd_bOpenReadOnly=FALSE;
					cmd_InsideOpenCommand=FALSE;

					if( ret != RTNORM ){
						break;
					}

					if(_tcsrchr(fs1,'\\'/*DNT*/)) {
						db_astrncpy(&cmd_DwgSavePath,fs1,strlen(fs1)+1);
						*(_tcsrchr(cmd_DwgSavePath,'\\'/*DNT*/)+1)=0;
					}
				}
			}
			if(rb.rbnext) sds_relrb(rb.rbnext);
			return(ret);
		}
		else return(RTCAN);
		// EBATECH(CNBR) ]-
    } else {
typeit: ;
		if ( GetActiveCommandQueue()->IsNotEmpty() ||
			 GetMenuQueue()->IsNotEmpty() ||
			 lsp_cmdhasmore)
			{
			SDS_WasFromPipeLine=TRUE;
			}
		else
			{
			SDS_WasFromPipeLine=FALSE;
			}

        if(RTNORM!=sds_getstring(1,ResourceString(IDC_OPENCLOSE__NDRAWING_NAME_3, "\nDrawing name: " ),fs1)) return(RTCAN);
        if(*fs1=='~'/*DNT*/) goto dia;

		char extension[15];
		ic_getext(fs1, extension);
		if (strsame(extension, "."/*DNT*/))
	        ic_setext(fs1,"DWG"/*DNT*/);

        strncpy(fs2,fs1,sizeof(fs2)-1);
        if(sds_findfile(fs2,fs1)!=RTNORM) {
			cmd_ErrorPrompt(CMD_ERR_CANTFINDDWG,0,fs1);
            return(RTERROR);
        }
    }

    if(!*fs1) return(RTNONE);

	char extension[15];
	ic_getext(fs1, extension);
	if (!strisame(extension, ".DWG") && !strisame(extension, ".DXF") && !strisame(extension, ".DWF") && !strisame(extension, ".DWT"))
	{
		CString msg = fs1;
		msg += "\n";
		msg += ResourceString(IDC_ICADDOC_AN_INVALID_FILE_20, "An invalid file extension was entered.");
		msg += "\n";
		msg += ResourceString(IDC_OPENCLOSE_OPEN_VERIFY, "Please verify the correct file name was given.");
		MessageBox(IcadSharedGlobals::GetMainHWND(), msg, ResourceString(IDC_OPENCLOSE_OPEN_DRAWING_1, "Open Drawing" ),MB_OK | MB_ICONEXCLAMATION);

		return(RTERROR);
	}

	ChangeSlashToBackslashAndRemovePipe(fs1);

	if(strrchr(fs1,'\\'/*DNT*/)) {
		db_astrncpy(&cmd_DwgSavePath,fs1,strlen(fs1)+1);
		*(strrchr(cmd_DwgSavePath,'\\'/*DNT*/)+1)=0;
	}

	cmd_InsideOpenCommand=TRUE;
	RT ret=cmd_OpenNewDrawing(fs1);

	// BUG 8421
	// Removed this and moved to ICADDOC.CPP

	cmd_bOpenReadOnly=FALSE;
	cmd_InsideOpenCommand=FALSE;
	return(ret);
}

short cmd_close(void) {
	SDS_MultibleMode=0;
	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_istransparent() == RTERROR) return RTERROR;

//	cmd_InsideOfCloseCommand=TRUE;
	if( GetActiveCommandQueue()->IsNotEmpty() ||
		GetMenuQueue()->IsNotEmpty() ||
		lsp_cmdhasmore) {
		RT ret;
		struct resbuf rb;
		char fs1[IC_ACADBUF];
		SDS_getvar(NULL,DB_QDBMOD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint) {
			if(sds_initget(0,ResourceString(IDC_OPENCLOSE_INITGET_YES_NO_4, "Yes No ~_Yes ~_No" ))!=RTNORM) {
//				cmd_InsideOfCloseCommand=FALSE;
				return(RTERROR);
			}
			if(RTCAN==(ret=sds_getkword(ResourceString(IDC_OPENCLOSE__NSAVE_CHANGES_5, "\nSave changes to drawing file? <Y>: " ),fs1))) {
//				cmd_InsideOfCloseCommand=FALSE;
				return(ret);
			}
			if((ret==RTNORM) || (ret==RTNONE)){
				if((strisame("NO"/*DNT*/,fs1)) && (ret==RTNORM)) {
					SDS_CURDOC->SetModifiedFlag(FALSE);
					ExecuteUIAction( ICAD_WNDACTION_FILECLOSE );
//					cmd_InsideOfCloseCommand=FALSE;
					return(RTNORM);
				} else {	// "Yes" or ENTER should hit this code.
					SDS_CMainWindow->m_pvWndAction=(void*)SDS_CURDOC;
					ExecuteUIAction( ICAD_WNDACTION_FILESAVE );
					ExecuteUIAction( ICAD_WNDACTION_FILECLOSE );
//					cmd_InsideOfCloseCommand=FALSE;
					return(RTNORM);
				}
			}
		}
	}
	ExecuteUIAction( ICAD_WNDACTION_FILECLOSE );
//	cmd_InsideOfCloseCommand=FALSE;
    return(RTNORM);
}

short cmd_saveall(void) {
	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	SDS_MultibleMode=0;
	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	ExecuteUIAction( ICAD_WNDACTION_SAVEALL );
	return(RTNORM);
}

short cmd_wmfin(void)     {

	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);
	return(RTNORM);
	/* =======================================================
	  NOTE : Below code never execute feature reserved
	========================================================*/
    char           fs1[IC_ACADBUF];
    struct resbuf  rb;
    CDC           *CDC;
    HMETAFILE      hMeta;
    CIcadView     *pView;
    RECT           rect;

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

    if (SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);

    if ( rb.resval.rint &&
		GetActiveCommandQueue()->IsEmpty() &&
		GetMenuQueue()->IsEmpty() &&
		!lsp_cmdhasmore) {
        dia: ;
        if (RTNORM!=sds_getfiled(ResourceString(IDC_OPENCLOSE_IMPORT_WMF_7, "Import WMF" ),cmd_DwgSavePath,ResourceString(IDC_OPENCLOSE_WINDOWS_METAFI_8, "Windows Metafile|wmf,Windows Clipboard|clp" ),0,&rb)) return(RTCAN);
        if (rb.restype==RTSTR) {
            strcpy(fs1,rb.resval.rstring); IC_FREE(rb.resval.rstring);
        }
        else{ goto typeit; }
    } else {
typeit:
        if(RTNORM!=sds_getstring(1,ResourceString(IDC_OPENCLOSE__NIMPORT_WMF___9, "\nImport WMF: " ),fs1)) return(RTCAN);
        if(*fs1=='~'/*DNT*/) goto dia;
    }

    if(!*fs1) return(RTNONE);

	ChangeSlashToBackslashAndRemovePipe(fs1);

	if (strrchr(fs1,'\\'/*DNT*/)) {
	    db_astrncpy(&cmd_DwgSavePath,fs1,strlen(fs1)+1);
		*(strrchr(cmd_DwgSavePath,'\\'/*DNT*/)+1)=0;
	}

    pView=SDS_CURVIEW;
    CDC=pView->GetDC();

    hMeta=GetMetaFile(fs1);
    pView->GetClientRect(&rect);
    PlayMetaFile(CDC->m_hDC,hMeta);
    pView->ReleaseDC(CDC);

    return(RTNORM);
}

short cmd_wclose(void) {
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

//	cmd_InsideOfCloseCommand=TRUE;
	ExecuteUIAction( ICAD_WNDACTION_WCLOSE );
//	cmd_InsideOfCloseCommand=FALSE;
    return(RTNORM);
}

short cmd_windows(void) {

    cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0);
    SDS_FreeEventList(0);

//	if (cmd_iswindowopen() == RTERROR) return RTERROR;
//	cmd_InsideOfCloseCommand=TRUE;
//	ExecuteUIAction( ICAD_WNDACTION_WINDOWS );
//	cmd_InsideOfCloseCommand=FALSE;
    return(RTNORM);
}

short cmd_wcloseall(void) {
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

//	cmd_InsideOfCloseCommand=TRUE;
	ExecuteUIAction( ICAD_WNDACTION_WCLOSEALL );
//	cmd_InsideOfCloseCommand=FALSE;
    return(RTNORM);
}




