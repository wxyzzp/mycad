/* MAIL.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Supports the mail related commands
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h" /*DNT*/

#include "IcadMail.h"/*DNT*/
#include "IcadApi.h"
#include "IcadApp.h"

// **************
// GLOBALS
//
extern int	  SDS_AutoSave;


// **************************
// COMMAND FUNCTION
//
short cmd_mail(void) {
    char szFileName[IC_FILESIZE+IC_PATHSIZE],szPathName[IC_FILESIZE+IC_PATHSIZE],
		*pszPathName;
    short ret;

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

    if(cmd_iswindowopen() == RTERROR)
	{
		return RTERROR;
	}
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	MapiFileDesc md;
    struct resbuf rb;
	SDS_getvar(NULL,DB_QDWGNAME,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    strncpy(szFileName,rb.resval.rstring,sizeof(szFileName));
	IC_FREE(rb.resval.rstring);
    ic_setext(szFileName,"dwg"/*DNT*/);
	//*** Make sure a path isn't included in the file name.
	char* pszFileName = strrchr(szFileName,'\\'/*DNT*/);
	if(NULL==pszFileName)
	{
		pszFileName=szFileName;
	}
	else
	{
		pszFileName++;
	}
	//*** Set the path to a temporary name
	if((pszPathName = _tempnam("c:\\tmp"/*DNT*/,"tmpmail"/*DNT*/))==NULL)
	{
		sds_alert(ResourceString(IDC_MAIL_CANNOT_CREATE_A_UNI_0, "Cannot create a unique filename." ));
		return RTERROR;
	}
	strncpy(szPathName,pszPathName,sizeof(szPathName));
	IC_FREE(pszPathName);
	ic_setext(szPathName,"dwg"/*DNT*/);
	//*** Save the drawing to the temporary filename.
	SDS_CMainWindow->m_strWndAction=szPathName;
	int nAutoSave=SDS_AutoSave;
	SDS_AutoSave=2;
	SDS_CMainWindow->m_pvWndAction=(void*)SDS_CURDOC;
	ExecuteUIAction( ICAD_WNDACTION_FILESAVE );
	SDS_AutoSave=nAutoSave;
	if(!SDS_CMainWindow->m_bWndAction)
	{
		sds_alert(ResourceString(IDC_MAIL_CANNOT_SAVE_TO_UNIQ_1, "Cannot save to unique filename." ));
		return RTERROR;
	}

    md.ulReserved=0;
    md.flFlags=0;
    md.nPosition=0;
    md.lpszPathName=szPathName;
    md.lpszFileName=pszFileName;
    md.lpFileType=NULL;
	ret=Icad_Send("IntelliCAD Mail"/*DNT*/," "/*DNT*/,1,&md);

	//*** Erase the temporary drawing.
	unlink(szPathName);
	return(ret);
}

