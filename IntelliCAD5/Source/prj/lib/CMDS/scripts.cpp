/* D:\ICADDEV\PRJ\LIB\CMDS\SCRIPTS.CPP
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
#include "sdsapplication.h" /*DNT*/
#include "commandqueue.h" /*DNT*/
#include "IcadApi.h"

char *cmd_MacroSavePath;



short cmd_resume(void)   
	{ 
	SDS_MultibleMode=0;

	if ( GetCancelScriptQueue()->IsNotEmpty() )
		{
		GetActiveCommandQueue()->Copy( GetCancelScriptQueue() );
		GetCancelScriptQueue()->Flush();
	    }
    return(RTNORM); 
	}




short cmd_rscript(void)  
	{
	SDS_MultibleMode=0;


    if( GetScriptQueue()->IsNotEmpty() ) 
		{
		GetActiveCommandQueue()->Copy( GetScriptQueue() );
	    }
    return(RTNORM); 
	}



short cmd_script(void)   
	{ 
    char fs1[IC_ACADBUF]; 
    struct resbuf rb,*trb; 
    FILE *fptr;

    *fs1=0;
    if(SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) 
		{
		return(RTERROR);
		}

    if( rb.resval.rint && 
		GetActiveCommandQueue()->IsEmpty() && 
		GetMenuQueue()->IsEmpty() && 
		!lsp_cmdhasmore) 
		{

		// NOTE LABEL dia
        dia: ;   // ------------- LABEL


        if(RTNORM!=sds_getfiled(ResourceString(IDC_SCRIPTS_RUN_SCRIPT_0, "Run Script" ),cmd_MacroSavePath,ResourceString(IDC_SCRIPTS_COMMAND_LINE_SCR_1, "Command Line Script|scr" ),4,&rb)) 
			{
			return(RTCAN);
			}

        if(rb.restype==RTSTR) 
			{
            strcpy(fs1,rb.resval.rstring); IC_FREE(rb.resval.rstring);
	        }
	    else { goto typeit; } // EBATECH(CNBR) 2002/4/30 Enable TYPE button
		} 
	else 
		{
		// NOTE LABEL typeit
		typeit: ; // ------------- LABEL
        if(RTNORM!=sds_getstring(1,ResourceString(IDC_SCRIPTS__NSCRIPT_TO_RUN__2, "\nScript to run: " ),fs1)) 
			{
			return(RTCAN);
			}
        if(*fs1=='~'/*DNT*/) 
			{
			goto dia;			// ------- dia label is above
			}
	    }

    if(!*fs1) 
		{
		return(RTNONE);
		}

	if(!strrchr(fs1,'.'/*DNT*/)) 
		{
		ic_setext(fs1,"scr"/*DNT*/);
		}
	
	ChangeSlashToBackslashAndRemovePipe(fs1);
	
	if(strrchr(fs1,'\\'/*DNT*/)) 
		{
		db_astrncpy(&cmd_MacroSavePath,fs1,strlen(fs1)+1);
		*(strrchr(cmd_MacroSavePath,'\\'/*DNT*/)+1)=0;
		}

    if( sds_findfile(fs1,fs1)!=RTNORM || 
		(fptr=fopen(fs1,"rb"/*DNT*/))==NULL ) 
		{
		sds_printf(ResourceString(IDC_SCRIPTS__NUNABLE_TO_FIND_3, "\nUnable to find file \"%s\"." ),fs1);
		return(RTERROR);
		}

	GetScriptQueue()->Flush();
	GetCancelScriptQueue()->Flush();
	
	SDS_IgnoreMenuStar=1;

	while (ic_nfgets(fs1,sizeof(fs1)-1,fptr)) 
		{
		char* cp1;
		for(cp1=fs1; *cp1!=0; cp1++) 
			{
			if(*cp1=='\\'/*DNT*/) 
				{
				*cp1='/'/*DNT*/;
				}
			//EBATECH(R.Arayashiki)-[ for MBCS String
			else if ((_MBC_LEAD ==_mbbtype((unsigned char)*cp1,0)) &&
				  (_MBC_TRAIL==_mbbtype((unsigned char)*(cp1+1),1)))
				{
				cp1 ++;
				}
			// ]-EBATECH
			}
    
		if(*fs1==';'/*DNT*/) 
			continue;

    	trb=SDS_MnuStrToRB((char *)fs1); 
		
		if ( trb != NULL )
			{
			resbuf *pTemp = trb;

			// find the end of the list
			//
			while( pTemp->rbnext != NULL )
				{
				pTemp = pTemp->rbnext;
				}

			// if last token is a space, add a NULL string to the list
			//
			if ( ( pTemp->restype == RTSTR ) &&
				 ( pTemp->resval.rstring ) &&
				 ( pTemp->resval.rstring[0] == ' '/*DNT*/) &&
				 ( pTemp->resval.rstring[1] == 0) )
				{
				pTemp->rbnext = sds_buildlist( RTSTR, ""/*DNT*/,0);
				}

			GetActiveCommandQueue()->AddItem( trb );
			}
		}
	fclose(fptr);
	SDS_IgnoreMenuStar=0;

	if ( GetActiveCommandQueue()->IsNotEmpty() )
		{
		GetScriptQueue()->Copy( GetActiveCommandQueue() );
		}

	return(RTNORM); 
	}





// ugh....
//
static int cmd_iAppendMode=0;



void cmd_AppendHook(HWND hDlg)
{
	HWND hAppend = GetDlgItem(hDlg,IDC_MACRO_APPEND);
	if(hAppend==NULL) return;
	cmd_iAppendMode = (int)SendMessage(hAppend,BM_GETCHECK,0,0L);
}

short cmd_recmacro(void)   
{ 	
    char fs1[IC_ACADBUF],append[IC_ACADBUF]; 
    struct resbuf rb; 
	RT ret;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	*append=0;  *fs1=0;
    if(SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);

    if( rb.resval.rint && 
		GetActiveCommandQueue()->IsEmpty() && 
		GetMenuQueue()->IsEmpty() && 
		!lsp_cmdhasmore) {
        dia: ;
        if(RTNORM!=SDS_GetFiled(ResourceString(IDC_SCRIPTS_RECORD_SCRIPT_5, "Record Script" ),cmd_MacroSavePath,ResourceString(IDC_SCRIPTS_COMMAND_LINE_SCR_1, "Command Line Script|scr" ) ,5,&rb,MAKEINTRESOURCE(IDD_MACRO_APPEND),cmd_AppendHook)) return(RTCAN);
        if(rb.restype==RTSTR) {
            strcpy(fs1,rb.resval.rstring); IC_FREE(rb.resval.rstring);
        }
        else if( rb.restype == RTSHORT ) { goto typeit; } // EBATECH(CNBR) 2002/4/30
		if(cmd_iAppendMode) *append='Y'/*DNT*/;
    } else {
		typeit: ;
        if(RTNORM!=sds_getstring(1,ResourceString(IDC_SCRIPTS__NSCRIPT_TO_RECO_6, "\nScript to record: " ),fs1)) return(RTCAN);
        if(*fs1=='~'/*DNT*/) 
			goto dia;
       if(sds_initget(0,ResourceString(IDC_SCRIPTS_INITGET_YES_NO_7, "Yes No ~_Yes ~_No" ))!=RTNORM) 
			return(RTERROR);
		if(RTCAN==(ret=sds_getkword( ResourceString(IDC_SCRIPTS__NAPPEND_TO_SCRI_8, "\nAppend to script? <N>: " ),append))) return(ret);
    }

    if(!*fs1) return(RTNONE);

	ChangeSlashToBackslashAndRemovePipe(fs1);

	if(strrchr(fs1,'\\'/*DNT*/)) {
		db_astrncpy(&cmd_MacroSavePath,fs1,strlen(fs1)+1);
		*(strrchr(cmd_MacroSavePath,'\\'/*DNT*/)+1)=0;
	}

	cmd_stopmacro();
	int flags=CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite |
		(toupper(*append)=='Y'/*DNT*/ ? (CFile::modeNoTruncate) : 0);
	if(!SDS_CMainWindow->m_fMacroRecFile.Open(fs1,flags)) {
		char fs2[IC_ACADBUF];
		SDS_CMainWindow->m_fMacroRecFile.m_hFile=0;
		sprintf(fs2,ResourceString(IDC_SCRIPTS_UNABLE_TO_OPEN_F_9, "Unable to open file.\n%s" ),fs1);
		sds_alert(fs2);
		return(RTERROR);
	}
	SDS_CMainWindow->m_fMacroRecFile.SeekToEnd();

	rb.restype=RTSHORT;
	rb.resval.rint=1;
	sds_setvar("MACROREC"/*DNT*/,&rb);

	return(RTNORM); 
}

short cmd_stopmacro(void) {
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	SDS_MultibleMode=0;
	struct resbuf rb;

	if(SDS_CMainWindow->m_fMacroRecFile.m_hFile==0) return(RTNORM);

	SDS_CMainWindow->m_fMacroRecFile.Close();
	SDS_CMainWindow->m_fMacroRecFile.m_hFile=0;

	rb.restype=RTSHORT;
	rb.resval.rint=0;
	sds_setvar("MACROREC"/*DNT*/,&rb);

	return(RTNORM); 
}

short cmd_delay(void) {
	int fi1;
	RT	ret;

	if (sds_initget(RSG_NONEG,NULL)!=RTNORM) return(RTERROR);
	if(RTNORM!=(ret=sds_getint(ResourceString(IDC_SCRIPTS__NMILLISECONDS__10, "\nMilliseconds to delay: " ),&fi1))) return(ret);

	while(1) {
		Sleep(250);
		fi1-=210;
		if(fi1<=0 || sds_usrbrk() ) break;
	}
	return(RTNORM); 
}



