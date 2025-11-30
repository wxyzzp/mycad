/* G:\ICADDEV\PRJ\LIB\CMDS\DXFINOUT.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!

#include "cmds.h"/*DNT*/
#include "cmdslayout.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "IcadDoc.h"

#include "sdsapplication.h" /*DNT*/
#include "commandqueue.h" /*DNT*/
#include "IcadView.h"

char *cmd_DxfSavePath;
bool  cmd_InsideDxfInCommand;
void  cmd_OnOpenOK(HWND hDlg);

short cmd_dxfin(void)    {     

	if (cmd_iswindowopen() == RTERROR) return RTERROR;	
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;
	
	if( SDS_CURDWG==NULL || 
		SDS_CURDWG->ret_isDirty()==true) 
		{
		cmd_ErrorPrompt(CMD_ERR_DXFNEWONLY,0);
		return(RTERROR);
	}

	resbuf plrb;
	bool convertpl;
	
	SDS_getvar(NULL,DB_QPLINETYPE,&plrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (plrb.resval.rint==2)
		convertpl=true;
	else
		convertpl=false;

    char fs1[IC_ACADBUF];//,fs2[IC_ACADBUF];
    struct resbuf rb; 
	int fi1;
	short filedia,ret=RTNORM;
	struct gr_view *view=SDS_CURGRVW;

    *fs1=0;
    if(SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
	filedia=rb.resval.rint;
    if( filedia && 
		GetActiveCommandQueue()->IsEmpty() && 
		GetMenuQueue()->IsEmpty() && 
		!lsp_cmdhasmore) {
        dia: ;
		if(SDS_GetFiled(ResourceString(IDC_DXFINOUT_LOAD_DXF_FILE_0, "Load DXF File" ),cmd_DxfSavePath,ResourceString(IDC_DXFINOUT_DRAWING_EXCHANG_1, "Drawing Exchange Format|dxf" ),4,&rb,MAKEINTRESOURCE(IDD_OPENSAVE_PREVIEW),cmd_OnOpenOK)!=RTNORM) return(RTCAN);
        if(rb.restype==RTSTR) {
            strcpy(fs1,rb.resval.rstring); IC_FREE(rb.resval.rstring);
        }
        else{ goto typeit; } // EBATECH(CNBR) 2002/4/30 Enable TYPE button
    } else {
		typeit:
        if(RTNORM!=sds_getstring(1,ResourceString(IDC_DXFINOUT__NDXF_FILE_TO_L_2, "\nDXF file to load: " ),fs1)) return(RTCAN);
        if(*fs1=='~'/*DNT*/) goto dia;
    }

	ChangeSlashToBackslashAndRemovePipe(fs1);

    if(!strchr(fs1,'.'/*DNT*/)) ic_setext(fs1,"dxf"/*DNT*/);

	if(strrchr(fs1,'\\'/*DNT*/)) {
		db_astrncpy(&cmd_DxfSavePath,fs1,strlen(fs1)+1);
		*(strrchr(cmd_DxfSavePath,'\\'/*DNT*/)+1)=0;
	}

    if(*fs1 && access(fs1,04)==0) {




		db_drawing *dp;
		if((dp=SDS_CMainWindow->m_pFileIO.drw_openfile(NULL,fs1,&fi1,convertpl))!=NULL) {
			delete SDS_CURDWG;
			SDS_CMainWindow->m_pCurDoc->m_dbDrawing=dp;
			SDS_CURVIEW->m_pVportTabHip=NULL;


			cmd_InsideDxfInCommand=TRUE;
			SDS_OnFirstView(SDS_CURVIEW,1);
			cmd_InsideDxfInCommand=FALSE;

			POSITION pos=SDS_CURDOC->GetFirstViewPosition();
    
			// We need to make sure all of the CIcadViews get the current vp tab hip.
			// BUG 6334 -- when this wasn't set, we GPF'd next time we redraw
			//
			while (pos!=NULL) 
				{
				CIcadView *pView=(CIcadView *)SDS_CURDOC->GetNextView(pos);
				ASSERT_KINDOF(CIcadView, pView);

				pView->m_pVportTabHip=SDS_CURVIEW->m_pVportTabHip;
				}

			cmd_InsideOpenCommand=TRUE;
			cmd_CreateNewViewports(SDS_CURDWG);

	// BUG 8421 
	// Removed this and moved to ICADDOC.CPP

			cmd_bOpenReadOnly=FALSE;
			cmd_InsideOpenCommand=FALSE;

			// TODO - well... it's really not an error... what to do with this...
			// Maybe make up a generic "Done." message.  Anyway, *shouldn't* it be successful?
			sds_printf(ResourceString(IDC_DXFINOUT__NINPUT_FROM_DX_3, "\nInput from DXF complete." ));		// TODO - Linda - Not an error
			cmd_setUpCTAB();	// 2003/08/10 EBATECH(CNBR) Sync CTAB
			cmd_regenall();

			ExecuteUIAction( ICAD_WNDACTION_UDSTLAY );
			ExecuteUIAction( ICAD_WNDACTION_UDSTLTP );
			ExecuteUIAction( ICAD_WNDACTION_UDSTCOL );
			ExecuteUIAction( ICAD_WNDACTION_UDSTSTY ); // EBATECH (Style)
			ExecuteUIAction( ICAD_WNDACTION_UDSTDIM ); // EBATECH (DimStyle)
			ExecuteUIAction( ICAD_WNDACTION_UDSTLW );  // EBATECH (Lineweight)

			// Also menuchecks and toolbars
			extern char *SDS_UpdateSetvar;
			SDS_UpdateSetvar=NULL;
			ExecuteUIAction( ICAD_WNDACTION_UDCHKSTAT );
			ExecuteUIAction( ICAD_WNDACTION_UDTBSTATE );
		}

    }

    return(ret); 
}

short cmd_dxfout(void){     

	if (cmd_iswindowopen() == RTERROR) return RTERROR;			

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char fs1[IC_ACADBUF],fs2[IC_ACADBUF],pmt[IC_ACADBUF];
    struct resbuf rb; 
	RT ret;
	int expert;
	sds_name sset,ent;
	long entcnt,fl1;
	db_handitem **elp=NULL;

	sset[0]=sset[1]=0L;
    *fs1=0;
	if(SDS_getvar(NULL,DB_QSAVENAME,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
	strncpy(fs2,rb.resval.rstring,sizeof(fs2)-1);
	IC_FREE(rb.resval.rstring);
	ic_setext(fs2,"dxf"/*DNT*/);
	if(SDS_getvar(NULL,DB_QEXPERT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
	expert=rb.resval.rint;
    if(SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);

    if( rb.resval.rint && 
		GetActiveCommandQueue()->IsEmpty() && 
		GetMenuQueue()->IsEmpty() && 
		!lsp_cmdhasmore) {
        dia2: ;
 		if(SDS_GetFiled(ResourceString(IDC_DXFINOUT_SELECT_DXF_FILE_4, "Select DXF file" ),fs2,ResourceString(IDC_DXFINOUT_DRAWING_EXCHANG_1, "Drawing Exchange Format|dxf" ),5,&rb,MAKEINTRESOURCE(IDD_OPENSAVE_PREVIEW),cmd_OnOpenOK)!=RTNORM) return(RTCAN);
        if(rb.restype==RTSTR) {
            strcpy(fs1,rb.resval.rstring); IC_FREE(rb.resval.rstring);
        }
 		else{ goto typeit2; }	// EBATECH(CNBR) 2002/4/30 Enable TYPE button
    } else {
		typeit2: ;
		sprintf(pmt,ResourceString(IDC_DXFINOUT__NDXF_FILE_TO_E_5, "\nDXF file to export <%s>: " ),fs2);
        if(RTNORM!=(ret=sds_getstring(1,pmt,fs1))) return(RTCAN);
		if(ret==RTNONE || *fs1==0) strcpy(fs1,fs2);
		
        if(*fs1=='~'/*DNT*/) goto dia2;
        if(!strchr(fs1,'.'/*DNT*/)) ic_setext(fs1,"dxf"/*DNT*/);
        if( !access(fs1,00) && 
			expert<2 && 
			GetActiveCommandQueue()->IsEmpty() && 
			GetMenuQueue()->IsEmpty() && 
			!lsp_cmdhasmore) 
			{
            if(sds_initget(0,ResourceString(IDC_DXFINOUT_INITGET_YES_REP_6, "Yes-Replace_it|Yes No-Cancel_Save|No ~_Yes ~_No" ))!=RTNORM) return(RTERROR); 
            if((ret=sds_getkword(ResourceString(IDC_DXFINOUT__NA_DRAWING_WIT_7, "\nA drawing with this name already exists. Replace it? <N> " ),fs2))!=RTNORM) return(ret);
            if(strisame(fs2, "NO"/*DNT*/)) 
                return(RTNORM);
			}
    }

	ic_setext(fs1,"dxf"/*DNT*/);
    if(*fs1) {
		ChangeSlashToBackslashAndRemovePipe(fs1);

		if(strrchr(fs1,'\\'/*DNT*/)) {
			db_astrncpy(&cmd_DxfSavePath,fs1,strlen(fs1)+1);
			*(strrchr(cmd_DxfSavePath,'\\'/*DNT*/)+1)=0;
		}
		int AsciOrBin=IC_DXF; // Asci
		int Version=IC_ACAD2004;	// AutoCAD 2004.
		int prec=6;
		for(;;){
			if(0L==sset[0]){
				sds_initget(0,ResourceString(IDC_DXFINOUT_INITGET_ENTITI_10, "Entities ~Objects Binary Version ~ ` ~_Entities ~_Objects ~_Binary ~_Version ~_" ));
				ret=sds_getint(ResourceString(IDC_DXFINOUT__NENTITIES_BIN_11, "\nEntities/Binary/Version/Enter decimal places of accuracy (0-16) <6>: " ),&prec);
			}else{
				sds_initget(0,NULL);
				ret=sds_getint(ResourceString(IDC_DXFINOUT__NENTER_DECIMA_12, "\nEnter decimal places of accuracy (0-16) <6>: " ),&prec);
			}
			if(ret==RTNORM){
				if(prec<0 || prec>16){
					cmd_ErrorPrompt(CMD_ERR_INT0TO16,0);
					continue;
				}else break;
			}else if(ret==RTNONE){
				prec=6;
				break;
			}else if(ret==RTKWORD){
				sds_getinput(fs2);
				if(strisame(fs2,"BINARY"/*DNT*/)) {
					AsciOrBin=IC_BDXF;  
					break;
				} else if(strisame(fs2,"VERSION"/*DNT*/)) {
					if (SDS_CURDWG->WasLoadedFromA2K())
						{
						sds_initget(0,"AutoCAD 2000|7"/*DNT*/);
						if(sds_getint(ResourceString(IDC_SAVE_FILE_VERSION_3__1, "\nFile version (7=(AutoCAD 2000) <7>:"),&Version)==RTCAN) return(-2);
						}
					else
						{
						//BugZilla No. 78248; 26-08-2002 [	
						//sds_initget(0,"rel_2.5|0 rel_2.6|1 rel_9|2 rel_10|3 rel_11/12|4 rel_13|5 rel_14|6"/*DNT*/);
						sds_initget(0,ResourceString(IDC_SAVE_FORMAT_VERSION_3, "Release_2.5|R2.5 Release_2.6|R2.6 Release_9|R9 Release_10|R10 Release_11/12|R12 ~R11 Release_13|R13 Release_14|R14 AutoCAD_2000|A2K AutoCAD_2004|A2004 ~0 ~1 ~2 ~3 ~4 ~5 ~6 ~7 ~8 ~_R2.5 ~R2.6 ~R9 ~R10 ~R12 ~R11 ~R13 ~R14 ~A2K ~A2004 ~R2.5 ~R2.6 ~R9 ~R10 ~R12 ~R13 ~R14 ~A2K ~A2004"));
						//if(sds_getint("\nFile version (0=(r2.5) 1=(r2.6) 2=(r9.0) 3=(r10) 4=(r11/12) 5=(r13) 6=(r14)) <6>:"/*DNT*/,&Version)==RTCAN) return(-2);
						CString AskStr = ResourceString(IDC_SAVE__NFILE_VERSION_R2_5_5, "\nFile version R2.5/R2.6/R9/R10/R11/R12/R13/R14/A2K/A2004 <A2004>: ");
						char fs4[IC_ACADBUF] = {0};
						if((ret = sds_getkword(AskStr, fs4)) == RTCAN)
							return(RTCAN);

						if (ret == RTNONE)
							Version = IC_ACAD2004;

						if (ret==RTNORM) 
							{
							if (strsame("A2004"/*DNT*/,fs4)) 
								Version=IC_ACAD2004;
							else if (strsame("A2K"/*DNT*/,fs4)) 
								Version=IC_ACAD2000;
							else if (strsame("R14"/*DNT*/,fs4)) 
								Version=IC_ACAD14;
							else if (strsame("R13"/*DNT*/,fs4))
								Version=IC_ACAD13;
							else if (strsame("R11"/*DNT*/,fs4) || strsame("R12"/*DNT*/,fs4))
								Version=IC_ACAD11;
							else if (strsame("R10"/*DNT*/,fs4))
								Version=IC_ACAD10;
							else if (strsame("R9"/*DNT*/,fs4))
								Version=IC_ACAD9;
							else if (strsame("R2.6"/*DNT*/,fs4))
								Version=IC_ACAD26;
							else if (strsame("R2.5"/*DNT*/,fs4))
								Version=IC_ACAD25;
							}
						//BugZilla No. 78248; 26-08-2002 ]
						}
					continue;
				} else if(strisame(fs2,"ENTITIES"/*DNT*/) || strisame(fs2,"OBJECTS"/*DNT*/)) {
					//Export only selected entities
					if(sset[0]!=0L)sds_ssfree(sset);
					if(elp!=NULL){delete elp;elp=NULL;}
					if(RTNORM!=(ret=sds_pmtssget(ResourceString(IDC_DXFINOUT__NSELECT_ENTIT_17, "\nSelect entities to export: " ),NULL,NULL,NULL,NULL,sset,0)))return(ret);
					if(RTNORM!=(ret=sds_sslength(sset,&entcnt)))return(ret);
					if(entcnt==0L)return(RTNORM);
					elp= new db_handitem * [entcnt];
					memset(elp,0,sizeof(db_handitem*)*entcnt);
					for(fl1=0L;sds_ssname(sset,fl1,ent)==RTNORM;fl1++){
						elp[fl1]=(db_handitem*)ent[0];
					}
					//NOTE:  We have only put sset objects into elp array.  DXF f'n will 
					//	have to sort out which are simple and which are complex ents to 
					//	write vertices, attribs, & seqend markers to dxf file.
					continue;
				}
			}else return(ret);	
		}
		SDS_SetUndo(IC_UNDO_NOP,NULL,NULL,NULL,SDS_CURDWG);
		if(SDS_getvar(NULL,DB_QMAKEBAK,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
		ret=SDS_CMainWindow->m_pFileIO.drw_savefile(SDS_CURDWG,entcnt,elp,fs1,AsciOrBin,Version,0,prec,1,1,rb.resval.rint,TRUE);
	}
	if(sset[0]!=0L) {
		long fl1=0L;
		while(RTNORM==sds_ssname(sset,fl1,ent)) {
			sds_redraw(ent,IC_REDRAW_DRAW);
			++fl1;
		}		
		sds_ssfree(sset);
	}
	if(elp!=NULL)delete elp;
    return(RTNORM); 
}



