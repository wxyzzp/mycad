#include "cmds.h"/*DNT*/
#include "cmdsWBlock.h"
#include "CloneHelper.h"
#include "commandqueue.h"/*DNT*/
#include "CmdQueryTools.h" //ucs2wcs(), wcs2ucs()
#include "IcadApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void cmd_OnOpenOK(HWND hDlg);
// Globals
char*	cmd_BatchtoDiskFname;
const char*	cmd_BatchtoDiskBname;
bool	cmd_GaveACISMsg;
bool	cmd_GaveProxyMsg;
bool	cmd_GaveA2KEntMsg;

extern char*	cmd_DwgSavePath;
////////////// Wblock stuff

short cmd_wblock2disk(char *fname, const char *bname) 
{
	cmd_BatchtoDiskFname=fname;
	cmd_BatchtoDiskBname=bname;
	RT ret=cmd_wblock();
	cmd_BatchtoDiskFname=NULL;
	cmd_BatchtoDiskBname=NULL;
	return(ret);
}

short cmd_wblock(void) 
{
//Autodsys 102603{{
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
//Autodsys 102603}}


	if (cmd_iswindowopen() == RTERROR) 
		return RTERROR;

	if (cmd_istransparent() == RTERROR) 
		return RTERROR;

	char *fcp1,fs1[IC_ACADBUF],fname[IC_PATHSIZE+IC_FILESIZE],blkname[IC_PATHSIZE+IC_FILESIZE];
	struct resbuf setgetrb,rbucs,rbwcs;
	sds_name ss1;
	sds_point pt1;
	int ret,blkcnt=0;
	int filedia;			// 0 uses the command line, 1 uses dialog boxes
	int expert;				// Suppresses some prompts based on value of "EXPERT"
	short selflg=0;
	long rc=0L;
	char filetype = IC_DWG;

//	SDS_CURDWG->StopAnonBlkNameGen();

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	if(SDS_getvar(NULL,DB_QEXPERT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) 
	{ 
		rc=(-__LINE__); 
		goto bail; 
	}
	expert=setgetrb.resval.rint;
	if(SDS_getvar(NULL,DB_QFILEDIA,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) 
	{ 
		rc=(-__LINE__); 
		goto bail; 
	}
	filedia=setgetrb.resval.rint;

	if(sds_ssadd(NULL,NULL,ss1)!=RTNORM) 
	{ 
		rc=(-__LINE__); 
		goto bail; 
	}

	if(cmd_BatchtoDiskFname) 
	{
		strncpy(fname,cmd_BatchtoDiskFname,sizeof(fname)-1);
		goto jump1;
	}

	if( filedia &&
		GetActiveCommandQueue()->IsEmpty() &&
		GetMenuQueue()->IsEmpty() &&
		!lsp_cmdhasmore) 
	{
dia: ;
		if(RTNORM!=SDS_GetFiled(ResourceString(IDC_BLOCK_SAVE_BLOCK_24, "Save Block" ),cmd_DwgSavePath,ResourceString(IDC_BLOCK_STANDARD_DRAWING__25, "Standard Drawing File|dwg,Drawing Exchange Format|dxf" ),517,&setgetrb,MAKEINTRESOURCE(IDD_OPENSAVE_PREVIEW),cmd_OnOpenOK)) 
		{
			rc=(-1L); 
			goto bail;
		}
		if(setgetrb.restype==RTSTR) 
		{
			strcpy(fname,setgetrb.resval.rstring); 
			IC_FREE(setgetrb.resval.rstring);
		}
		else 
		{ 
			goto typeit; 
		} // EBATECH(CNBR) 2002/4/30 Enable Type button
	} 
	else 
	{
typeit: ;
		if((ret=sds_getstring(1,ResourceString(IDC_BLOCK_FILE_NAME_FOR_NEW_26, "File name for new drawing/block: " ),fs1))==RTERROR) 
		{
			rc=(-__LINE__); 
			goto bail;
		} 
		else 
			if(ret==RTCAN || !*fs1) 
			{
				rc=(-1L); 
				goto bail;
			}
		if(*fs1=='~'/*DNT*/) 
			goto dia;

		// If no extension make it dwg.
		if(ic_chkext(fs1,"dxf"/*DNT*/) && ic_chkext(fs1,"dwg"/*DNT*/))
			ic_setext(fs1,"DWG"/*DNT*/);

		if(sds_findfile(fs1,fname)==RTNORM && expert<2) 
		{
			if(sds_initget(0,ResourceString(IDC_BLOCK_INITGET_YES_REPLA_27, "Yes-Replace_it|Yes No-Cancel_Save|No ~_Yes ~_No" ))!=RTNORM) 
			{
				rc=(-__LINE__);
				goto bail;
			}
			if((ret=sds_getkword(ResourceString(IDC_BLOCK__NA_DRAWING_WITH__28, "\nA drawing with this name already exists. Replace it? <N> " ),fs1))==RTERROR) 
			{
				rc=(-__LINE__); 
				goto bail;
			} 
			else 
				if(ret==RTCAN) 
				{
					rc=(-1L); 
					goto bail;
				} 
				else 
					if(ret==RTNONE) 
					{
						rc=0L; 
						goto bail;
					} 
					else 
						if(ret==RTNORM) 
						{
							if(strisame(fs1, "NO"/*DNT*/)) 
							{
								rc=0L;
								goto bail;
							}
						}
		} 
		else 
		{
			strncpy(fname,fs1,sizeof(fname)-1);
		}
	}
	//check if the file is ok to make
	if(!cmd_access(fname))
	{
		cmd_ErrorPrompt(CMD_ERR_SAVEFILE,0);
//		SDS_CURDWG->StartAnonBlkNameGen();
		return(RTERROR);
	}

	ChangeSlashToBackslashAndRemovePipe(fname);

	if(strrchr(fname,'\\'/*DNT*/)) 
	{
		db_astrncpy(&cmd_DwgSavePath,fname,strlen(fname)+1);
		*(strrchr(cmd_DwgSavePath,'\\'/*DNT*/)+1)=0;
	}
	if(0==ic_chkext(fname,"dxf"/*DNT*/))
		filetype = IC_DXF;
jump1: ;

	cmd_GaveACISMsg=FALSE;
	cmd_GaveProxyMsg=FALSE;
	cmd_GaveA2KEntMsg=FALSE;

	//*** Block name
	for(;;) 
	{
		if(cmd_BatchtoDiskBname) 
		{
			strncpy(fs1,cmd_BatchtoDiskBname,sizeof(fs1)-1);
		} 
		else 
		{
			ret=sds_initget(RSG_OTHER,ResourceString(IDC_BLOCK_INITGET_MULTIPLE__29, "Multiple_Blocks|MULTIPLE All_Entities|ALL Select_Entities|SELECT ~_MULTIPLE ~_ALL ~_SELECT" ));
			if((ret=sds_getkword(ResourceString(IDC_BLOCK__NENTER_TO_SELECT_30, "\nENTER to select entities/& to include multiple blocks/* for all entities/ <Block to save as WBlock>: " ),fs1))==RTERROR) 
			{
				rc=(-__LINE__); 
				goto bail;
			} 
			else 
				if(ret==RTCAN) 
				{
					rc=(-1L); 
					goto bail;
				}
		}
		sds_initget(0,NULL);	//bug in initget w/sds_getstring.popup menu persists.
		if(!*fs1 || strisame(fs1,"SELECT"/*DNT*/)) 
		{
			selflg=1;
			break;
		}
		else 
			if((*fs1)=='='/*DNT*/)
			{
				//EBATECH(FUTA) 2000-10-22 -[ for MBCS String
				//if((fcp1=strrchr(fname,IC_SLASH))==NULL)
				if((fcp1=(char *)_mbsrchr((const unsigned char *)fname,(unsigned int)IC_SLASH))==NULL)
					// ]- EBATECH(FUTA)
					//Modified SAU 31/05/2000
					//fcp1=fs1;
					fcp1=fname;
				else
					fcp1++;
				strncpy(fs1,fcp1,sizeof(blkname)-1);
				if((fcp1=strchr(fs1,'.'/*DNT*/))!=NULL)
					(*fcp1)=0;
			}
			else 
				if(strsame(fs1,"*"/*DNT*/) || strsame(fs1,"_*"/*DNT*/) || strisame(fs1,"ALL"/*DNT*/))
				{
					selflg=2;
					break;
				}
				else 
					if(strsame(fs1,"&"/*DNT*/) || strsame(fs1,"_&"/*DNT*/) || strisame(fs1,"MULTIPLE"/*DNT*/))
					{
						//save multiple blocks into the drawing...
						//note: if multiple blks written, translate them all to 0,0 so that
						//they're consistent.  otherwise, we don't know what to set INSBASE to
						ret=sds_getstring(0,ResourceString(IDC_BLOCK__NBLOCKS_TO_SAVE__32, "\nBlocks to save: " ),fs1);
						strupr(fs1);
						if(ret!=RTNORM)
						{
							rc=(-1L);
							goto bail;
						}

						CCloneHelper ch(IC_DWG, IC_ACAD2004);
						blkcnt = ch.wblock(fs1, true, fname);

						SDS_getvar(NULL,DB_QCMDECHO,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						if(CMD_CMDECHO_PRINTF(setgetrb.resval.rint))
						{
							if(blkcnt>0)
								sds_printf(ResourceString(IDC_BLOCK__N_I__S_WRITTEN_T_33, "\n%i %s written to file" ),blkcnt,(blkcnt==1)?ResourceString(IDC_BLOCK_BLOCK_34, "block" ):ResourceString(IDC_BLOCK_BLOCKS_35, "blocks" ));
							else
								sds_printf(ResourceString(IDC_BLOCK__NNO_MATCHING_BLO_36, "\nNo matching block names found." ));
						}
						break;
					}
					
		strncpy(blkname,fs1,sizeof(blkname)-1);

		CCloneHelper ch(filetype, IC_ACAD2004);
		if(!ch.wblock(blkname, false, fname))
		{
			sds_printf( ResourceString(IDC_BLOCK__N___BLOCK__S_NOT_37, "\n-- Block %s not found. --" ),blkname);
			continue;
		}
		break;
	}
	//*** Select entities if no block name was entered.
	//NOTE: if selflg==2, rather than just saving the dwg we'll ssget and
	//write it manually. This purges dwg database (but it's slow!)
	if(selflg>0) 
	{
		if(selflg==1)
		{
			//*** Get the insertion point
			if(sds_initget(RSG_NONULL,NULL)!=RTNORM) 
			{ 
				rc=(-__LINE__); 
				goto bail; 
			}
			if((ret=sds_getpoint(NULL,ResourceString(IDC_BLOCK__NINSERTION_POINT__5, "\nInsertion point for new block: " ),pt1))==RTERROR) 
			{
				rc=(-__LINE__); 
				goto bail;
			} 
			else 
				if(ret==RTCAN) 
				{
					rc=(-1L); 
					goto bail;
				} 
				else 
					if(ret==RTNORM) 
					{
						// 		  if(pt1[2]!=0.0) {
						// 			  sds_printf("\nNote:  Z insertion base is not zero.");
						// We need to tell the user a) why they would care, and b) what they should do about it.
						//			  }
						//*** Set LASTPOINT system variable.
						setgetrb.restype=RT3DPOINT;
						ic_ptcpy(setgetrb.resval.rpoint,pt1);
						SDS_setvar(NULL,DB_QLASTPOINT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					}
					//*** Get selection set of entities.
					if((ret=sds_pmtssget(ResourceString(IDC_BLOCK__NSELECT_ENTITIES_38, "\nSelect entities for wblock: " ),NULL,NULL,NULL,NULL,ss1,1))==RTERROR) 
					{
						rc=(-__LINE__); 
						goto bail;
					} 
					else 
						if(ret==RTCAN) 
						{
							rc=(-1L); 
							goto bail;
						}
		}
		else 
			if(selflg==2)
			{
				SDS_getvar(NULL,DB_QINSBASE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(pt1,setgetrb.resval.rpoint);
				if((ret=sds_pmtssget(NULL,"X"/*DNT*/,NULL,NULL,NULL,ss1,0))!=RTNORM) 
				{
					rc=(-__LINE__); 
					goto bail;
				}
			}

		CCloneHelper ch(filetype, IC_ACAD2004);
		ch.setBasePoint(pt1);
		rc = ch.wblock(ss1, fname);
		if(rc)
		{
			rc = 0;
			//*** Delete the entities
			if((ret=cmd_erase_and_oops(ss1,0))==RTERROR) 
			{
				rc=(-__LINE__); 
				goto bail;
			} 
			else 
				if(ret==RTCAN) 
				{
					rc=(-1L); 
					goto bail;
				}
		}
	}
bail:
	if(sds_ssfree(ss1)!=RTNORM) 
		rc=(-__LINE__);

//	SDS_CURDWG->StartAnonBlkNameGen();

	if(rc==0L) 
		return(RTNORM);
	if(rc==(-1L)) 
		return(RTCAN);
	if(rc<(-1L)) 
	{
		sds_printf("\nError on wblock\n");
		CMD_INTERNAL_MSG(rc);
	}
	return(RTERROR);
}
