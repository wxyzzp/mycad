/* G:\ICADDEV\PRJ\LIB\CMDS\MENU.CPP
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
#include "sdsapplication.h" /*DNT*/
#include "lisp.h"/*DNT*/
#include "commandqueue.h" /*DNT*/
#include "inputevent.h" /*DNT*/

char *cmd_MenuSavePath;

short cmd_menuunload(void)
{
	struct sds_resbuf rb;
	BOOL bForceFileDia=FALSE;
	char fs1[IC_ACADBUF];

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	#if defined(SDS_ICADMFC)
		char *ext="icm,mnu"/*DNT*/;
	#else
		char *ext="mnu"/*DNT*/;
	#endif

    if(SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);

	*fs1=0;
    if( rb.resval.rint && 
		GetActiveCommandQueue()->IsEmpty() && 
		GetMenuQueue()->IsEmpty() && 
		!lsp_cmdhasmore)
	{
		dia:
		if(!bForceFileDia)
		{
			ExecuteUIAction( ICAD_WNDACTION_CUSTOMIZE );
			return(RTNORM);
		}
		else
		{
			if(RTNORM!=sds_getfiled(ResourceString(IDC_MENU_UNLOAD_MENU_0, "Unload Menu" ),cmd_MenuSavePath,ext,4,&rb))
			{
				return(RTCAN);
			}
			if(rb.restype==RTSTR)
			{
				strcpy(fs1,rb.resval.rstring); IC_FREE(rb.resval.rstring);
			}
			else { goto typeit; }
		}
	}
	else
	{
		typeit: ;
	    if(RTNORM!=sds_getstring(1,ResourceString(IDC_MENU__NMENU_GROUP_NAME_T_1, "\nMenu group name to unload: " ),fs1))
		{
			return(RTCAN);
		}
		if(*fs1=='~')
		{
			bForceFileDia=TRUE;
			goto dia;
		}
/*
		if(*fs1!=0 && (fs1[0]=='.' && fs1[1]==0))
		{
			return RTNORM;
		}
		char* pFileName;
		if(NULL==(pFileName=strrchr(fs1,'\\')))
		{
			pFileName=fs1;
		}
		if(NULL==strrchr(pFileName,'.'))
		{
			ic_setext(fs1,"mnu");
		}
//*
		if(sds_findfile(fs1,fs1)==RTNORM)
		{
			break;
		}
		sds_printf("\nUnable to find file \" )%s\"." ),fs1);
		return RTERROR;
*/
	}
	if(*fs1==0)
	{
		return RTERROR;
	}
	ChangeSlashToBackslashAndRemovePipe(fs1);
/*
	if(strrchr(fs1,'\\')) {
		db_astrncpy(&cmd_MenuSavePath,fs1,strlen(fs1)+1);
		*(strrchr(cmd_MenuSavePath,'\\')+1)=0;
	}
*/
	SDS_CMainWindow->m_strWndAction=fs1;
	ExecuteUIAction( ICAD_WNDACTION_UNLOADMENU );
	if(!SDS_CMainWindow->m_bWndAction)
	{
//		cmd_ErrorPrompt(CMD_ERR_CANTFINDMENU,0,fs1);
	}

	return RTNORM;
}

short cmd_OpenMenu(BOOL bAppend)
{
    char fs1[IC_ACADBUF];

    struct resbuf rb;
	BOOL bForceFileDia=FALSE;
	CString ext=ResourceString(IDC_MENU_AUTOCAD_MENU_FILE_M_5, "AutoCAD Menu File|mnu,IntelliCAD Menu File|icm,Add-on Menu File|mns,XML ToolBar File|xml" );

	*fs1=0;
    if(SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);

    if( rb.resval.rint && 
		GetActiveCommandQueue()->IsEmpty() && 
		GetMenuQueue()->IsEmpty() && 
		!lsp_cmdhasmore) {
        dia:
		if(bAppend && !bForceFileDia)
		{
			ExecuteUIAction( ICAD_WNDACTION_CUSTOMIZE );
			return(RTNORM);
		}
		else
		{
			if(RTNORM!=sds_getfiled(ResourceString(IDC_MENU_OPEN_MENU_6, "Open Menu" ),cmd_MenuSavePath,ext,4,&rb)) return(RTCAN);
			if(rb.restype==RTSTR) {
				strcpy(fs1,rb.resval.rstring); IC_FREE(rb.resval.rstring);
			}
			else { goto typeit; } // EBATECH(CNBR) 2002/4/30 Enable TYPE button
		}

		ChangeSlashToBackslashAndRemovePipe(fs1);
    } else {
		typeit: ;
		for(;;)
		{
	        if(RTNORM!=sds_getstring(1,ResourceString(IDC_MENU__NMENU_NAME__OR___F_7, "\nMenu name, or . for none: " ),fs1)) return(RTCAN);
		    if(*fs1=='~'/*DNT*/)
			{
				bForceFileDia=TRUE;
				goto dia;
			}
			if(*fs1!=0 && (fs1[0]=='.'/*DNT*/ && fs1[1]==0)) return RTNORM;

			ChangeSlashToBackslashAndRemovePipe(fs1);

			//*** Verify the extension.
			char ext[IC_FILESIZE];
			char* pFileName;
			if(NULL==(pFileName=strrchr(fs1,'\\'/*DNT*/)))
			{
				pFileName=fs1;
			}
			if(NULL==strrchr(pFileName,'.'/*DNT*/))
			{
				ic_setext(fs1,"mnu"/*DNT*/);
			}
			ic_getext(fs1,ext);
			//*** Accept mns and icm also.
			// IntelliJapan(Maeda) 03.04.2004 -[ xml toolbar file support for menuload command.
			if(!*ext || (!strisame(".MNU"/*DNT*/,ext) && !strisame(".MNS"/*DNT*/,ext) &&
				           !strisame(".ICM"/*DNT*/,ext) && !strisame(".XML"/*DNT*/,ext)))
			// ]- IntelliJapan(Maeda) 03.04.2004
			{
				ic_setext(fs1,"mnu"/*DNT*/);
			}
			//*** Verify the file exists.
			if(sds_findfile(fs1,fs1)==RTNORM) break;
			cmd_ErrorPrompt(CMD_ERR_CANTFINDFILE,0,fs1);
		}
    }

    if(*fs1!=0)
	{
		if(strrchr(fs1,'\\'/*DNT*/)) {
			db_astrncpy(&cmd_MenuSavePath,fs1,strlen(fs1)+1);
			*(strrchr(cmd_MenuSavePath,'\\'/*DNT*/)+1)=0;
		}

		SDS_CMainWindow->m_strWndAction=fs1;
		SDS_CMainWindow->m_bWndAction=bAppend;
		ExecuteUIAction( ICAD_WNDACTION_LOADMENU );

		if(bAppend==FALSE) {
			struct resbuf vb;
			vb.restype=RTSTR;
			vb.resval.rstring=fs1;
			sds_setvar("MENUNAME"/*DNT*/,&vb);
		}

		//*** Load the mnl.
		cmd_loadlispmnl(fs1);
    }

    return(RTNORM);
}

short cmd_menuload(void) 
{ 
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	return(cmd_OpenMenu(TRUE)); 
}
short cmd_menu(void) 
{ 
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	return(cmd_OpenMenu(FALSE)); 
}

short cmd_loadlispmnl(const char *menuname) {
	//*** Load the mnl.
    if(menuname==NULL || *menuname==0) return(RTERROR);

	char fs2[IC_ACADBUF];
	char *fname= new char [strlen(menuname)+5]; // +5 for ext.
	strcpy(fname,menuname);

	ic_setext(fname,"mnl"/*DNT*/);
	if(access(fname,04)==0) {
		extern struct lsp_pendlink lsp_pend;
		extern struct resbuf lsp_evres,*lsp_argsll; /* A "SUPER" RESBUF LLIST */
		struct sds_resbuf *targsll;
		struct lsp_pendlink tpend;

		// Save the current pending lisp and eval.
		targsll=lsp_argsll; lsp_argsll=NULL;
		tpend=lsp_pend;        
		memset(&lsp_pend,0,sizeof(lsp_pend));
    
		//EBATECH(R.Arayashiki)-[ MBCS String
		for(int fi1=0; fname[fi1]!=0; fi1++)
			if(fname[fi1]=='\\'/*DNT*/) fname[fi1]='/'/*DNT*/;
			else if ((_MBC_LEAD ==_mbbtype((unsigned char)fname[fi1],0)) &&
				  (_MBC_TRAIL==_mbbtype((unsigned char)fname[fi1+1],1)))
			{
				fi1 ++;
			}
			// EBATECH ]
		sprintf(fs2,"(LOAD \"%s\")"/*DNT*/,fname);

			{
			// ---------------------------------
			// this string will get free in SDS_ProcessInputEvent
			// TODO -- fix this...
			//
			char *pTemp = new char [strlen((char *)fs2)+1];
			strcpy( pTemp, (char *)fs2 );

			InputEvent theEvent( SDS_EVM_POPUPCOMMAND, pTemp );

	        SDS_ProcessInputEvent( &theEvent);
			}

		//strcpy(fs2,"(if (/= s::startup nil)(s::startup))"/*DNT*/);  //old code - changed for Bug 1-49010 - see new code below
		strcpy(fs2,"(if (/= s::startup nil)(s::startup)(princ) )"/*DNT*/);
			{
			// ---------------------------------
			// this string will get free in SDS_ProcessInputEvent
			//
			char *pTemp = new char [strlen((char *)fs2)+1];
			strcpy( pTemp, (char *)fs2 );

			InputEvent theEvent( SDS_EVM_POPUPCOMMAND, pTemp );

	        SDS_ProcessInputEvent( &theEvent);
			}

    
		lsp_argsll=targsll; 
		lsp_pend=tpend;        
	}
	if(NULL!=fname)
	{
		IC_FREE(fname);
		fname=NULL;
	}
	return(RTNORM);
}

short cmd_toolbar(void)
{
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char fs1[IC_ACADBUF],fs2[IC_ACADBUF];
	int ret;

	for(;;)
	{
		*fs1=0;
        sds_initget(SDS_RSG_OTHER, ResourceString(IDC_TOOLBAR_INIT1, "~All ~? ~_All ~_?"));
        ret=sds_getstring(1,ResourceString(IDC_MENU__NENTER_TOOLBAR_NAM_9, "\nEnter Toolbar name(s) to show or hide, ALL, or <?>: " ),fs1);
        
        if (ret == SDS_RTCAN)
            return SDS_RTCAN;

		ic_trim(fs1,"be");

		if (ret == RTNONE || strisame(fs1, "?"/*DNT*/) )// If user doesn't know what to type...
		{												// display dialog for them.
			//*** Send the WndAction to display the ViewToolbars dialog.
			ExecuteUIAction( ICAD_WNDACTION_TBCONFIG );
			return RTNORM;
		} 
		//*** Set the toolbar string in the CMainWindow class.
		SDS_CMainWindow->m_strWndAction=fs1;
		if(!strisame("ALL"/*DNT*/,fs1))                       // If user typed something other than "ALL"...
		{                                               // check to see if the name is valid.
			//*** Send the WndAction to find out if the toolbar name(s) are valid.
			ExecuteUIAction( ICAD_WNDACTION_ISVALIDTBNAME );   
			if(SDS_CMainWindow->m_bWndAction==FALSE)
			{                                           // If name is not OK...        
                cmd_ErrorPrompt(CMD_ERR_NOTBFOUND,0);   // display message, and
				continue;                               // ask for another toolbar name
			}
		}
		break;                                          // Once they get it right...
	}

	if(strisame("ALL"/*DNT*/,fs1)) {
		sds_initget(0,ResourceString(IDC_MENU_INITGET_SHOW_HIDE_11, "Show Hide ~_Show ~_Hide" ));
	} else {
		sds_initget(0,ResourceString(IDC_MENU_INITGET_SHOW_HIDE__12, "Show Hide ~Left ~Right ~Top ~Bottom ~Float ~Move ~_Show ~_Hide ~_Left ~_Right ~_Top ~_Bottom ~_Float ~_Move" ));
	}
	if(sds_getkword(ResourceString(IDC_MENU__NSHOW_OR_HIDE___13, "\nShow or Hide: " ),fs2)!=RTNORM) return(RTCAN);

    SDS_CMainWindow->m_strWndAction=fs1;
	if(strisame("SHOW"/*DNT*/, fs2))
	{
		//*** Send the WndAction to show all the toolbars.
		ExecuteUIAction( ICAD_WNDACTION_SHOWTOOLBARS );
	}
	else if(strisame("HIDE"/*DNT*/, fs2))// Hide toolbar(s)
	{
		//*** Send the WndAction to hide all the toolbars.
		ExecuteUIAction( ICAD_WNDACTION_HIDETOOLBARS );
	}
	else if(strisame("MOVE"/*DNT*/, fs2) || strisame("FLOAT"/*DNT*/, fs2))
	{
		sds_point pt;
		SDS_CMainWindow->m_pvWndAction=NULL;
		if(strisame("MOVE"/*DNT*/, fs2))
		{
			//*** TODO - Get the point containing the new position for the toolbar.
			sds_initget(SDS_RSG_NOLIM | SDS_RSG_NONEG | SDS_RSG_NONULL,NULL);
			int nRet = internalGetpoint(NULL, ResourceString(IDC_MENU__NNEW_TOOLBAR_LOCA_17, "\nNew toolbar location: " ),pt);
			if(nRet==RTNONE)
			{
				int i=0;
			}
			else if(nRet!=RTNORM)
			{
				return RTERROR;
			}
			SDS_CMainWindow->m_pvWndAction=pt;
		}
		//*** Send the WndAction to hide all the toolbars.
		ExecuteUIAction( ICAD_WNDACTION_MOVETOOLBAR );
		if(!SDS_CMainWindow->m_bWndAction)
			return RTERROR;
	}

    return RTNORM;
}

short cmd_tbconfig(void) {
	SDS_MultibleMode=0;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_TBCONFIG );
    
    return(RTNORM); 
}



