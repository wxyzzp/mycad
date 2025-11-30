/* D:\DEV\PRJ\ICAD\ICADMAIL.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "Icad.h"/*DNT*/
#include <mapi.h>

short Icad_Send(char *Subject, char *Message, int NumOfFiles, lpMapiFileDesc Files)
{
	lpMapiMessage MapiSpecs;
	ULONG rc;
	HWND DiaHndl;
	HINSTANCE MAPILib=NULL;
	LPMAPISENDMAIL fpMAPISendMail=NULL;
	
	MAPILib=LoadLibrary(TEXT("MAPI32.DLL"/*DNT*/));
	fpMAPISendMail=(LPMAPISENDMAIL)GetProcAddress(MAPILib,TEXT("MAPISendMail"/*DNT*/));
    DiaHndl=SDS_CMainWindow->GetSafeHwnd();
	if(MAPILib==NULL||fpMAPISendMail==NULL){
        ::MessageBox(DiaHndl,ResourceString(IDC_ICADMAIL_TO_USE_SEND_MAI_0, "To use Send Mail, you must have a MAPI-compliant mail program. Please install one and try again." ),ResourceString(IDC_ICADMAIL_SEND_MAIL_ERROR_1, "Send Mail Error" ),MB_ICONSTOP|MB_APPLMODAL|MB_OK);
        return(RTERROR);
    }
	
	MapiSpecs= new MapiMessage;
	memset(MapiSpecs,0,sizeof(MapiMessage));
	MapiSpecs->lpszSubject=Subject;
	MapiSpecs->lpszNoteText=Message;
    MapiSpecs->nFileCount=NumOfFiles;
	MapiSpecs->flFlags=MAPI_SENT;
	MapiSpecs->lpFiles=Files;
	rc=(*fpMAPISendMail)(0,(ULONG)DiaHndl,MapiSpecs,MAPI_DIALOG|MAPI_LOGON_UI|MAPI_NEW_SESSION,0);

	int k;
    switch(rc)
	{
        case MAPI_E_AMBIGUOUS_RECIPIENT: 
            k=0;
            break;
        case MAPI_E_ATTACHMENT_NOT_FOUND: 
            k=0;
            break;
        case MAPI_E_ATTACHMENT_OPEN_FAILURE: 
            k=0;
            break;
        case MAPI_E_BAD_RECIPTYPE: 
            k=0;
            break;
        case MAPI_E_FAILURE: 
            k=0;
            break;
        case MAPI_E_INSUFFICIENT_MEMORY: 
            k=0;
            break;
        case MAPI_E_INVALID_RECIPS: 
            k=0;
            break;
        case MAPI_E_LOGIN_FAILURE: 
            k=0;
            break;
        case MAPI_E_TEXT_TOO_LARGE: 
            k=0;
            break;
        case MAPI_E_TOO_MANY_FILES: 
            k=0;
            break;
        case MAPI_E_TOO_MANY_RECIPIENTS: 
            k=0;
            break;
        case MAPI_E_UNKNOWN_RECIPIENT: 
            k=0;
            break;
        case MAPI_E_USER_ABORT: 
            k=0;
            break;
        case SUCCESS_SUCCESS: 
            k=0;
            break;
    }

    if(rc==SUCCESS_SUCCESS || rc==MAPI_E_USER_ABORT)
	{
		rc=0;
	}

	if(NULL!=MapiSpecs)
	{
		delete MapiSpecs;
		MapiSpecs=NULL;
	}
	FreeLibrary(MAPILib);
	if(rc!=0)
	{
		return(RTERROR);
	}
	return(RTNORM);
}


