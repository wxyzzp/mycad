/* F:\DEV\PRJ\LIB\CMDS\SOUND.CPP
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
#include <mmsystem.h>

void cmd_PlaySound(int mode){
    //mode:
    //   0 = IcadStart sound
    //   1 = IcadWarning sound

#if	0	// disabled sounds
    char *SDS_RegEntry;
  	char szTmp[IC_ACADBUF];
    HKEY hKey;
	DWORD type;
	ULONG fl2;
	fl2=sizeof(szTmp);

    switch(mode){
        case 0:
            SDS_RegEntry="AppEvents\\Schemes\\Apps\\Icad\\IcadStart\\.current"/*DNT*/;
  	        if(ERROR_SUCCESS!=RegOpenKeyEx(HKEY_CURRENT_USER,SDS_RegEntry,0,KEY_READ,&hKey)) return;
            if(ERROR_SUCCESS!=RegQueryValueEx(hKey,""/*DNT*/,NULL,&type,(unsigned char *)szTmp,&fl2)) return;
	        if(strlen(szTmp)>0){
                PlaySound("IcadStart"/*DNT*/,NULL,SND_APPLICATION|SND_ASYNC);
            }
            break;
        case 1:
            SDS_RegEntry="AppEvents\\Schemes\\Apps\\Icad\\IcadWarning\\.current"/*DNT*/;
  	        if(ERROR_SUCCESS!=RegOpenKeyEx(HKEY_CURRENT_USER,SDS_RegEntry,0,KEY_READ,&hKey)) return;
            if(ERROR_SUCCESS!=RegQueryValueEx(hKey,""/*DNT*/,NULL,&type,(unsigned char *)szTmp,&fl2)) return;
	        if(strlen(szTmp)>0){
                PlaySound("IcadWarning"/*DNT*/,NULL,SND_APPLICATION|SND_ASYNC);
            }
            break;
    }
#endif
}



