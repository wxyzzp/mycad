/* F:\DEV\PRJ\LIB\CMDS\SAVE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * SAVE command
 *      with all the others i.e saveas saveasr12 etc.
 */ 
// THIS FILE HAS BEEN GLOBALIZED!
#include "cmds.h"/*DNT*/
#include "CloneHelper.h"
#include "Icad.h"/*DNT*/
#include "IcadDoc.h"
#include "db.h"/*DNT*/
#include "commandqueue.h" /*DNT*/
#include "IcadApi.h"
#include "IcadApp.h"

#define ICAD_WNDACTION_GETDWFINFO    4210

// Globals
bool cmd_InsideSaveAsCmd;
int  cmd_BitmapFilterIndex = 24;

extern int SDS_SavingAsR12;
extern int SDS_LastFilterIndex;
extern int  SDS_UseExportSelSet;
extern sds_name SDS_EditPropsSS;
extern bool SDS_DontBitBlt;
extern char *cmd_DwgSavePath;

// Proto
void cmd_OnOpenOK(HWND hDlg);	
void GetValidExtension(char* szFilename, char* szExt, const int cmd_BitmapFilterIndex);
void SetLastFilterIndex(const char* szExtension, int& nIndex, const int cmd_BitmapFilterIndex);

short cmd_saveasr12(void) { 
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
				
    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	SDS_SavingAsR12=1;
    RT ret=cmd_SaveFunc(1,NULL);    
	SDS_SavingAsR12=0;
    return(ret);    
}  

short cmd_save(void) {
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
	
    if (cmd_iswindowopen() == RTERROR) return RTERROR;	
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    return(cmd_SaveFunc(1,NULL));    
}

short cmd_saveas(void) { 
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
	
    if (cmd_iswindowopen() == RTERROR) return RTERROR;			

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	cmd_InsideSaveAsCmd=TRUE;
    RT ret=cmd_SaveFunc(1,NULL);    
	cmd_InsideSaveAsCmd=FALSE;
	return(ret);
}

short cmd_qsave(void) {
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
	extern int SDS_AutoSave;

    if (cmd_iswindowopen() == RTERROR) return RTERROR;			
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	int savedVal = SDS_AutoSave;
	SDS_AutoSave = 99; // reserved for QSAVE.
	short retVal = cmd_SaveFunc(0,NULL);
	SDS_AutoSave = savedVal;
    return(retVal);    
}

short cmd_SaveFunc(short saveasmode, CIcadDoc* pDoc) 
	{
    char fname[IC_PATHSIZE+IC_FILESIZE];
    struct resbuf setgetrb;
    long rc=0L;

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
	
	if (NULL==pDoc)
		{
		pDoc=SDS_CURDOC;
		if (NULL==pDoc)
			{
			return RTERROR;
			}
		SDS_CMainWindow->m_pvWndAction=(void*)pDoc;
		ExecuteUIAction( ICAD_WNDACTION_ISDOCEMBEDDED );
		if (SDS_CMainWindow->m_bWndAction)
			{
			SDS_CMainWindow->m_pvWndAction=(void*)pDoc;
			PostUIAction( ICAD_WNDACTION_FILESAVE,0L);
			return RTNORM;
			}
		}
    //*** Get the drawing name
    if (SDS_getvar(NULL,DB_QSAVENAME,&setgetrb,pDoc->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) 
		{
		rc=(-__LINE__);
		goto bail; 
		}
    strncpy(fname,setgetrb.resval.rstring,sizeof(fname)-1);
    IC_FREE(setgetrb.resval.rstring);

	switch(saveasmode) 
		{
		case 0:
			if (SDS_getvar(NULL,DB_QDWGTITLED,&setgetrb,pDoc->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) 
				{
				rc=(-__LINE__);
				goto bail;
				}
			if (!setgetrb.resval.rint)
				{
				rc=cmd_SaveAs(pDoc->m_dbDrawing,fname,sizeof(fname)-1,0);
				}
			else
				{
				//*** ~| TODO - Figure out the correct extension to append to the
				//*** filename since we don't save it in DWGNAME
				CString	oldName = fname;
				ic_setext(fname,".dwg"/*DNT*/);

								// if the name changed a SaveAs is necessary
								// since the new name might exist
				if ( !strisame( oldName, fname) )
					rc=cmd_SaveAs(pDoc->m_dbDrawing,fname,sizeof(fname)-1,0);
				}
			break;
		case 1:
			rc=cmd_SaveAs(pDoc->m_dbDrawing,fname,sizeof(fname)-1,0);
			break;
		case 2:
            if (0L!=(rc=cmd_SaveAs(pDoc->m_dbDrawing,fname,sizeof(fname)-1,1)))
				goto bail;
			//*** TODO - One export type that we don't support, but r14 does, is
			//*** Block (.dwg).
			if (SDS_LastFilterIndex <= 23) //*** Release X.X Dwg/Dxf 2004 or less.
				{
				//*** Export all entities to a Dwg or Dxf file.
				sds_point pt1;
				sds_name ss1;
				SDS_getvar(NULL,DB_QINSBASE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(pt1,setgetrb.resval.rpoint);
				if (sds_pmtssget(NULL,"X"/*DNT*/,NULL,NULL,NULL,ss1,0)!=RTNORM)
					{
					rc=-1;
					goto bail;
					} 
				CCloneHelper ch(pDoc->m_nFileType, pDoc->m_nFileVersion);
				ch.setBasePoint(pt1);
				if(ch.wblock(ss1, fname) == 0)
					{
					sds_ssfree(ss1);
					rc=-1;
					goto bail;
					}
				sds_ssfree(ss1);
				return RTNORM;
				}

			if (SDS_LastFilterIndex == 24)//*** dwt
			{

				SDS_CMainWindow->m_pFileIO.drw_savefile(SDS_CURDWG,
							0L, 
							NULL, 
							const_cast<char*>(fname),
							pDoc->m_nFileType, 
							pDoc->m_nFileVersion, 
							0, 
							12, 
							1, 
							1, 
							1, 
							TRUE);

				return RTNORM;
			}

			if (SDS_LastFilterIndex == 28)//*** Svg
			{

				SDS_CMainWindow->m_pFileIO.drw_savefile(SDS_CURDWG,
							0L, 
							NULL, 
							const_cast<char*>(fname),
							4, 
							-1, 
							0, 
							12, 
							1, 
							1, 
							1, 
							TRUE);

				// Eventually I'll want to go through the code below for this,
				// to allow users to select which entities to export.
				// Currently the output is way off centered. 
				
				// When BUGZILLA 78784 is fixed this should be called instead of
				// the SDS_CMainWindow->m_pFileIO.drw_savefile() call above.
/*
				sds_name ss1;
				sds_point pt1;
				SDS_getvar(NULL,DB_QINSBASE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(pt1,setgetrb.resval.rpoint);

				if (RTNORM!=sds_pmtssget(ResourceString(IDC_SAVE__NSELECT_ENTITIES_T_0, "\nSelect entities to export: " ),NULL,NULL,
					NULL,NULL,ss1,0))
				{
					rc=-1;
					goto bail;
				}

				CCloneHelper ch(3, -1);	// 3 resolves to Svg.
				ch.setBasePoint(pt1);
				if(ch.wblock(ss1, fname) == 0)
				{
					sds_ssfree(ss1);
					rc=-1;
					goto bail;
				}

				long ssct;
				sds_name ename;
				for (ssct=0L; sds_ssname(ss1,ssct,ename)==RTNORM; ssct++)
				{
					sds_redraw(ename,IC_REDRAW_UNHILITE);
				}

				sds_ssfree(ss1);
*/
				return RTNORM;
			}

			if (SDS_LastFilterIndex == 29)//*** Dwf
			{
				RT ret;
				int expert;
				short filedia;
				struct resbuf getRb;
				char fs1[IC_ACADBUF];
				SDS_getvar(NULL,DB_QEXPERT,&getRb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				expert = getRb.resval.rint;
				SDS_getvar(NULL,DB_QFILEDIA,&getRb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				filedia = getRb.resval.rint;

				int fileVersionAndFormat = 12;	// v6.0 compressed default.

				// fileTypeAndVersion & 0x1	 == V4.2
				// fileTypeAndVersion & 0x2	 == V5.2
				// fileTypeAndVersion & 0x4	 == V6.0
				// fileTypeAndVersion & 0x8	 == compressed.
				// fileTypeAndVersion & 0x10 == uncompressed.
				// fileTypeAndVersion & 0x20 == ASCII.
				if (!filedia)
				{
					sds_initget(0,ResourceString(IDC_SAVE_DWF_VERSION, "Version_4.2|V4.2 Version_5.5|V5.5 Version_6.0|V6.0 ~_V4.2 ~_V5.5 ~_V6.0"));
					CString AskStr = ResourceString(IDC_SAVE_DWF_VERSION_PROMPT, "\nDWF file version V4.2/V5.5/V6.0 <V6.0>: "); 
					if((ret = sds_getkword(AskStr, fs1)) == RTCAN)
						return(RTCAN);

					if (strisame(fs1,"V4.2"))
						fileVersionAndFormat = 0x1;
					else if (strisame(fs1,"V5.5"))
						fileVersionAndFormat = 0x2;
					else
						fileVersionAndFormat = 0x4;


					sds_initget(0,ResourceString(IDC_SAVE_DWF_FORMAT, "Compressed_Binary|C Uncompressed_Binary|U Ascii|A ~_C ~_U ~_A"));
					AskStr = ResourceString(IDC_SAVE_DWF_FORMAT_PROMPT, "\nDWF file format Compressed/Uncompressed/Ascii <Compressed>: "); 
					if((ret = sds_getkword(AskStr, fs1)) == RTCAN)
						return(RTCAN);

					if (strisame(fs1,"A"))
						fileVersionAndFormat |= 0x20;
					else if (strisame(fs1,"U"))
						fileVersionAndFormat |= 0x10;
					else
						fileVersionAndFormat |= 0x8;

				}
				else
				{
					HMODULE   hmodule = GetModuleHandle(NULL);
					ASSERT(hmodule);
					if(hmodule)
					{
						typedef HWND (* getviewhwnd)() ;
						getviewhwnd pFun ;
						pFun = (getviewhwnd)GetProcAddress(hmodule,"sds_getmainhwnd");
						ASSERT(pFun);
						if(pFun)
						{
							if (SendMessage(pFun(),ICAD_WNDACTION_GETDWFINFO,
												(WPARAM)NULL,(LPARAM)&fileVersionAndFormat) != S_OK)
							{
								return(RTCAN); 
							}
						}
					}
				}
				SDS_CMainWindow->m_pFileIO.drw_savefile(SDS_CURDWG,
							0L, 
							NULL, 
							const_cast<char*>(fname),
							5, 
							fileVersionAndFormat, 
							0, 
							12, 
							1, 
							1, 
							1, 
							TRUE);

				// Eventually I'll want to go through the code below for this,
				// to allow users to select which entities to export.
				// Currently the output is way off centered. 
				
				// When BUGZILLA 78784 is fixed this should be called instead of
				// the SDS_CMainWindow->m_pFileIO.drw_savefile() call above.
/*
				sds_name ss1;
				sds_point pt1;
				SDS_getvar(NULL,DB_QINSBASE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(pt1,setgetrb.resval.rpoint);

				if (RTNORM!=sds_pmtssget(ResourceString(IDC_SAVE__NSELECT_ENTITIES_T_0, "\nSelect entities to export: " ),NULL,NULL,
					NULL,NULL,ss1,0))
				{
					rc=-1;
					goto bail;
				}

				CCloneHelper ch(4, -1);	// 4 resolves to DWF.
				ch.setBasePoint(pt1);
				if(ch.wblock(ss1, fname) == 0)
				{
					sds_ssfree(ss1);
					rc=-1;
					goto bail;
				}

				long ssct;
				sds_name ename;
				for (ssct=0L; sds_ssname(ss1,ssct,ename)==RTNORM; ssct++)
				{
					sds_redraw(ename,IC_REDRAW_UNHILITE);
				}

				sds_ssfree(ss1);

*/
				return RTNORM;
			}

			SDS_UseExportSelSet=1;
			if (RTNORM!=sds_pmtssget(ResourceString(IDC_SAVE__NSELECT_ENTITIES_T_0, "\nSelect entities to export: " ),NULL,NULL,
				NULL,NULL,SDS_EditPropsSS,0))
				{
				rc=-1;
				goto bail;
				}

			if (SDS_LastFilterIndex == 25) //*** Bmp
				{
				ExecuteUIAction( ICAD_WNDACTION_CREATEHBMP );
				HBITMAP hBmp = (HBITMAP)SDS_CMainWindow->m_pvWndAction;
				SDS_CMainWindow->m_pvWndAction=NULL;
				if (NULL!=hBmp)
					{
					char *pBuffer;
					long  bytes;
					//*** When this object is destructed, it should be deleting the hBmp;
					CBitmap bmp;
					bmp.Attach(hBmp);
					if (0==SDS_SaveBitmapToBuffer(&bmp,&pBuffer,&bytes,FALSE))
						{
						SDS_SaveBufferToFile(pBuffer,bytes,fname);
						IC_FREE(pBuffer);
						}
					}
				}
			else if (SDS_LastFilterIndex == 26) //*** Wmf
				{
				ExecuteUIAction( ICAD_WNDACTION_CREATEHWMF);
				ICADWMFINFO* pWmfInfo = (ICADWMFINFO*)SDS_CMainWindow->m_pvWndAction;
				SDS_CMainWindow->m_pvWndAction=NULL;
				if (NULL!=pWmfInfo)
					{
					SDS_WriteWMF(pWmfInfo,fname);
					::DeleteMetaFile(pWmfInfo->hMetaFile);
					delete pWmfInfo;
					}
				}
			else if (SDS_LastFilterIndex == 27)//*** Emf
				{
				ExecuteUIAction( ICAD_WNDACTION_CREATEHEMF );
				HENHMETAFILE hMetaFile = (HENHMETAFILE)SDS_CMainWindow->m_pvWndAction;
				SDS_CMainWindow->m_pvWndAction=NULL;
				if (NULL!=hMetaFile)
					{
					HENHMETAFILE hCopyMetaFile = ::CopyEnhMetaFile(hMetaFile,fname);
					::DeleteEnhMetaFile(hMetaFile);
					::DeleteEnhMetaFile(hCopyMetaFile);
					}
				}
			else
			{
				ASSERT(!"Big problem");
			}
			//*** Unhightlight the selection set.
			sds_name ename;
			long ssct;
			for (ssct=0L; sds_ssname(SDS_EditPropsSS,ssct,ename)==RTNORM; ssct++)
				sds_redraw(ename,IC_REDRAW_UNHILITE);
			sds_ssfree(SDS_EditPropsSS);
			SDS_UseExportSelSet=0;
			return(RTNORM);
		}

    bail:

    if (rc==0L)
		{
		//*** Save the new file name in the WndAction string and send the message.
		SDS_CMainWindow->m_strWndAction=fname;
		SDS_CMainWindow->m_pvWndAction=(void*)pDoc;
		ExecuteUIAction( ICAD_WNDACTION_FILESAVE );
		if (SDS_CMainWindow->m_bWndAction)
			{
			SDS_UseExportSelSet=0;
			sds_name ename;
			if (saveasmode==2) 
				{
				SDS_DontBitBlt=1;
				for (int ssct=0L; sds_ssname(SDS_EditPropsSS,ssct,ename)==RTNORM; ssct++) 
					{
					sds_redraw(ename,IC_REDRAW_DRAW);
					if (((ssct+1)%200)==0) 
						SDS_BitBlt2Scr(1);
					}
				SDS_DontBitBlt=0;
				SDS_BitBlt2Scr(1);
				}
			return(RTNORM); 
			}
		rc=(-1);
		}
    if (rc==(-1L)) 
		return(RTCAN); 
    if (rc<(-1L)) 
		CMD_INTERNAL_MSG(rc);

	SDS_UseExportSelSet=0;
    return(RTERROR);
	}

long cmd_SaveAs(dwg_filelink* pCurDwg, char* fname, int nStrLen,int exportmode)
	{
	char fs1[IC_ACADBUF],fs2[IC_ACADBUF],fs3[IC_ACADBUF];
    struct resbuf setgetrb;
	int expert;			// For Expert Level suppression of prompts.
    RT ret, ret2;
    short filedia;
	long rc=0L;
	
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
	cmd_bSaveWithPassWord = FALSE;

	CString filetypes;

	LOADFILETYPES_9(IDC_SAVE_STANDARD_DRAWING_AU_1)
	filetypes = LOADEDSTRING;
	filetypes=filetypes+","/*DNT*/;
	filetypes=filetypes+ResourceString(IDC_SAVE_DWT_FILE, "Drawing Template|dwt"); ;

	CString exportypes;
	exportypes=ResourceString(IDC_SAVE_BITMAP_FILE_BMP_WIN_2, "Bitmap file|bmp,Windows Meta File|wmf,Enhanced Windows Meta File|emf"); 
	exportypes=exportypes+","/*DNT*/;
	exportypes=exportypes+ResourceString(IDC_SAVE_SVG_FILE, "Scalable Vector Graphics|svg"); ;
	exportypes=exportypes+","/*DNT*/;
	exportypes=exportypes+ResourceString(IDC_SAVE_DWF_FILE, "Design Web Format|dwf"); ;


	if (SDS_getvar(NULL,DB_QEXPERT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) 
		{
		rc=(-__LINE__);
		goto bail; 
		}
	expert=setgetrb.resval.rint;

    if (SDS_getvar(NULL,DB_QFILEDIA,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) 
		{
		rc=(-__LINE__); 
		goto bail; 
		}
    filedia=setgetrb.resval.rint;

    for (;;) 
		{
        if ( filedia && 
			GetActiveCommandQueue()->IsEmpty() && 
			GetMenuQueue()->IsEmpty() && 
			!lsp_cmdhasmore) 
			{
			*fs1=0;
			//*** Don't worry about cmd_DwgSavePath if a path is included with fs1.
			if (NULL!=strrchr(fs1,'\\'/*DNT*/))
				{
				if (cmd_DwgSavePath) strcpy(fs1,cmd_DwgSavePath);
				strcat(fs1,fname);
				}
			else
				{
				strncpy(fs1,fname,sizeof(fs1)-1);
				}
 			if (exportmode)
				{
				CString cs;
				cs=filetypes;
				cs=cs+","/*DNT*/;
				cs=cs+exportypes;
				//*** Remove the extension
				ic_setext(fs1,""/*DNT*/);
				//*** Call getfiled without showing the preview because the preview
				//*** only works for DWGs.
				if (RTNORM!=SDS_GetFiled(ResourceString(IDC_SAVE_EXPORT_DRAWING_AS_3, "Export Drawing As" ),fs1,cs,513,&setgetrb,NULL,cmd_OnOpenOK))
					{
					rc=(-1L); goto bail;
					}
				if(setgetrb.restype!=RTSTR){ filedia=0; continue; } // EBATECH(CNBR) 2002/4/30 Enable TYPE
				}
			else 
				{
				if (RTNORM!=SDS_GetFiled(ResourceString(IDC_SAVE_SAVE_DRAWING_AS_4, "Save Drawing As" ),fs1,filetypes,513,&setgetrb,MAKEINTRESOURCE(IDD_OPENSAVE_PREVIEW),cmd_OnOpenOK)) 
					{
					rc=(-1L); 
					goto bail;
					}
				if(setgetrb.restype!=RTSTR){ filedia=0; continue; } // EBATECH(CNBR) 2002/4/30 Enable TYPE
				}
            if (exportmode==1)
				{
			    if (!cmd_access(setgetrb.resval.rstring))
					{
          	        cmd_ErrorPrompt(CMD_ERR_EXPORTFILE,0);
				    rc=(-1L); 
					goto bail;
					}
				}
            
			// I'm attempting to simplify this mess for when I have to move
			// 2004 to the top of the file type list. MHB
			switch(SDS_LastFilterIndex) 
			{
			    case 1: 
					SDS_CURDOC->m_nFileVersion = IC_ACAD2004; 
					SDS_CURDOC->m_nFileType = IC_DWG;
					break;
				case 2:  
					SDS_CURDOC->m_nFileVersion = IC_ACAD2004; 
					SDS_CURDOC->m_nFileType = IC_DXF;
					break;
				case 3:
					SDS_CURDOC->m_nFileVersion = IC_ACAD2004; 
					SDS_CURDOC->m_nFileType = IC_BDXF;
					break;
			    case 4:	 
					SDS_CURDOC->m_nFileVersion = IC_ACAD2000; 
					SDS_CURDOC->m_nFileType = IC_DWG; 
					break;
				case 5:  
					SDS_CURDOC->m_nFileVersion = IC_ACAD2000; 
					SDS_CURDOC->m_nFileType = IC_DXF;
					break;
				case 6:
					SDS_CURDOC->m_nFileVersion = IC_ACAD2000; 
					SDS_CURDOC->m_nFileType = IC_BDXF;
					break;
			    case 7:	 
					SDS_CURDOC->m_nFileVersion = IC_ACAD14; 
					SDS_CURDOC->m_nFileType = IC_DWG; 
					break;
				case 8:  
					SDS_CURDOC->m_nFileVersion = IC_ACAD14; 
					SDS_CURDOC->m_nFileType = IC_DXF;
					break;
				case 9:
					SDS_CURDOC->m_nFileVersion = IC_ACAD14; 
					SDS_CURDOC->m_nFileType = IC_BDXF;
					break;
			    case 10:	 
					SDS_CURDOC->m_nFileVersion = IC_ACAD13; 
					SDS_CURDOC->m_nFileType = IC_DWG; 
					break;
				case 11:  
					SDS_CURDOC->m_nFileVersion = IC_ACAD13; 
					SDS_CURDOC->m_nFileType = IC_DXF;
					break;
				case 12:
					SDS_CURDOC->m_nFileVersion = IC_ACAD13;
					SDS_CURDOC->m_nFileType = IC_BDXF;
					break;
			    case 13:	 
					SDS_CURDOC->m_nFileVersion = IC_ACAD11; 
					SDS_CURDOC->m_nFileType = IC_DWG;
					break;
				case 14:  
					SDS_CURDOC->m_nFileVersion = IC_ACAD11; 
					SDS_CURDOC->m_nFileType = IC_DXF;
					break;
				case 15:
					SDS_CURDOC->m_nFileVersion = IC_ACAD11; 
					SDS_CURDOC->m_nFileType = IC_BDXF;
					break;
				case 16:  
					SDS_CURDOC->m_nFileVersion = IC_ACAD10; 
					SDS_CURDOC->m_nFileType = IC_DXF;
					break;
				case 17:
					SDS_CURDOC->m_nFileVersion = IC_ACAD10; 
					SDS_CURDOC->m_nFileType = IC_BDXF;
					break;
				case 18:  
					SDS_CURDOC->m_nFileVersion = IC_ACAD9;
					SDS_CURDOC->m_nFileType = IC_DXF;
					break;
				case 19:
					SDS_CURDOC->m_nFileVersion = IC_ACAD9;
					SDS_CURDOC->m_nFileType = IC_BDXF;
					break;
				case 20:  
					SDS_CURDOC->m_nFileVersion = IC_ACAD26;
					SDS_CURDOC->m_nFileType = IC_DXF;
					break;
				case 21:
					SDS_CURDOC->m_nFileVersion = IC_ACAD26;
					SDS_CURDOC->m_nFileType = IC_BDXF;
					break;
				case 22:  
					SDS_CURDOC->m_nFileVersion = IC_ACAD25; 
					SDS_CURDOC->m_nFileType = IC_DXF;
					break;
				case 23:
					SDS_CURDOC->m_nFileVersion = IC_ACAD25; 
					SDS_CURDOC->m_nFileType = IC_BDXF;
				case 24:
					SDS_CURDOC->m_nFileVersion = IC_ACAD2004; 
					SDS_CURDOC->m_nFileType = IC_DWT;
					break;
				default:
					SDS_CURDOC->m_nFileVersion = IC_ACAD2004; 
					SDS_CURDOC->m_nFileType = IC_DWG;
					break;
			}

            if (setgetrb.restype==RTSHORT && setgetrb.resval.rint==1) filedia=0;
            strncpy(fname,setgetrb.resval.rstring,nStrLen);
            IC_FREE(setgetrb.resval.rstring);

			pCurDwg->init_FileVersion(SDS_CURDOC->m_nFileVersion);
			}
		else 
			{
			// If/When we add 2005, this will allow .bmp, .wmf, and .emf files to have
			// the correct extensions. This should alway be one more than the case value
			// above for IC_ACAD25 - IC_BDXF.
			cmd_BitmapFilterIndex = 25;
			SDS_CURDOC->m_nFileType = IC_DWG;

			if (SDS_SavingAsR12) 
				{
				SDS_CURDOC->m_nFileVersion=IC_ACAD11;
				}
			else 
				{
				if (cmd_InsideSaveAsCmd) 
					{
					char fs1[IC_ACADBUF];

					sds_initget(0,ResourceString(IDC_SAVE_FORMAT_VERSION_1, "Release_2.5|R2.5 Release_2.6|R2.6 Release_9|R9 Release_10|R10 Release_11/12|R12 ~R11 Release_13|R13 AutoCAD_LT_2|LT2 Release_14|R14 AutoCAD_LT_95|LT95 AutoCAD_2000|A2K AutoCAD_2004|A2004"));
					CString AskStr = ResourceString(IDC_SAVE__NFILE_VERSION_R2_5_6, "\nFile version R2.5/R2.6/R9/R10/R11/R12/R13/LT2/R14/LT95/A2K/A2004 <A2004>: "); 
					if((ret = sds_getkword(AskStr, fs1)) == RTCAN)
						return(RTCAN);


					if (ret == RTNONE)
					{
						SDS_CURDOC->m_nFileVersion=IC_ACAD2004;
						SDS_LastFilterIndex = 1;
					}

					bool bDXFOnly = false;
					if (ret==RTNORM) 
					{
						if (strsame("A2004"/*DNT*/,fs1))
						{
							SDS_CURDOC->m_nFileVersion=IC_ACAD2004;
							SDS_CURDOC->m_nFileType = IC_DWG; 
							SDS_LastFilterIndex = 1;
						}
						else if (strsame("A2K"/*DNT*/,fs1))
						{
							SDS_CURDOC->m_nFileVersion=IC_ACAD2000;
							SDS_CURDOC->m_nFileType = IC_DWG; 
							SDS_LastFilterIndex = 4;
						}
						else if (strsame("R14"/*DNT*/,fs1) || strsame("LT95"/*DNT*/,fs1)) 
						{
							SDS_CURDOC->m_nFileVersion=IC_ACAD14;
							SDS_CURDOC->m_nFileType = IC_DWG; 
							SDS_LastFilterIndex = 7;
						}
						else if (strsame("R13"/*DNT*/,fs1) || strsame("LT2"/*DNT*/,fs1)) 
						{
							SDS_CURDOC->m_nFileVersion=IC_ACAD13;
							SDS_CURDOC->m_nFileType = IC_DWG; 
							SDS_LastFilterIndex = 10;
						}
						else if (strsame("R12"/*DNT*/,fs1) || strsame("R11"/*DNT*/,fs1)) 
						{
							SDS_CURDOC->m_nFileVersion=IC_ACAD11;
							SDS_CURDOC->m_nFileType = IC_DWG; 
							SDS_LastFilterIndex = 13;
						}
						else if (strsame("R10"/*DNT*/,fs1)) 
						{
							SDS_CURDOC->m_nFileVersion=IC_ACAD10;
							SDS_CURDOC->m_nFileType = IC_DXF;
							SDS_LastFilterIndex = 16;
							bDXFOnly = true;
						}
						else if (strsame("R9"/*DNT*/,fs1)) 
						{
							SDS_CURDOC->m_nFileVersion=IC_ACAD9;
							SDS_CURDOC->m_nFileType = IC_DXF;
							SDS_LastFilterIndex = 18;
							bDXFOnly = true;
						}
						else if (strsame("R2.6"/*DNT*/,fs1)) 
						{
							SDS_CURDOC->m_nFileVersion=IC_ACAD26;
							SDS_CURDOC->m_nFileType = IC_DXF;
							SDS_LastFilterIndex = 20;
							bDXFOnly = true;
						}
						else if (strsame("R2.5"/*DNT*/,fs1)) 
						{
							SDS_CURDOC->m_nFileVersion=IC_ACAD25;
							SDS_CURDOC->m_nFileType = IC_DXF;
							SDS_LastFilterIndex = 22;
							bDXFOnly = true;
						}
						else if (strsame("T"/*DNT*/,fs1)) 
						{
							SDS_CURDOC->m_nFileVersion=IC_ACAD2004;
							SDS_CURDOC->m_nFileType = IC_DWT;
							SDS_LastFilterIndex = 23;
							bDXFOnly = true;
						}
					}
				
					// For versions Release 10 and less, DWGdirect will only export
					// ascii or binary .dxf files. If a user selects one of these earlier
					// versions or any DXF version, we'll get which .dxf type (asci vs binary) they want here.
					CString AskStr2;
					int subOption = 0;
					if (bDXFOnly == true)
					{
						sds_initget(0,ResourceString(IDC_SAVE_FILETYPE, "Ascii_DXF|DXF Binary_DXF|BDXF ~_A ~B ~a ~b"));
						AskStr2 = ResourceString(IDC_SAVE_FILETYPE_PROMPT, "\nFile version Ascii dxf/Binary dxf <Ascii dxf>: "); 
					
						if((ret2 = sds_getkword(AskStr2, fs3)) == RTCAN)
							return(RTCAN);

						subOption = 0;
						if (ret2 == RTNORM)
						{
							subOption = (strisame("A"/*DNT*/,fs3)) ? 0 : 1;
						}

						// Ok, here we have a situation where we can only save as .dxf either binary or ascii.
						// If fname has an extension of .dwg, we need to change it to .dxf here. If it doesn't
						// have an extension at all, it is probably 'Drawing1', and will be handled correctly.
						char* tmp = strstr(fname, ".dwg");
						if (tmp == NULL)
						{
							tmp = strstr(fname, ".DWG");
						}
						if (tmp != NULL)
						{
							strcpy(tmp, ".dxf");
						}
					}
					// Increment the type and filter index based on the suboption
					// selected.
					SDS_CURDOC->m_nFileType += subOption;
					SDS_LastFilterIndex += subOption;
				}
			}

            char fs5[IC_ACADBUF];
            sprintf(fs5,ResourceString(IDC_SAVE__NFILE_NAME___S____6, "\nFile name (extension defines export type <%s>: " ),fname);
            *fs1=0;
            if ((ret=sds_getstring(1,fs5,fs1))==RTERROR) 
				{
                rc=(-__LINE__);
				goto bail;
				}
			else if (ret==RTCAN) 
				{
                rc=(-1L);
				goto bail;
				}   
            if (*fs1==0) 
				strncpy(fs1,fname,sizeof(fs1)-1);

            if (*fs1=='~'/*DNT*/) 
				{
				filedia=1;
				continue; 
				}
			strcpy(fs2,fs1);
			
			char szExtension[IC_ACADBUF];
			GetValidExtension(fs2, szExtension, cmd_BitmapFilterIndex);
			ic_setext(fs2, szExtension);
			SetLastFilterIndex(szExtension, SDS_LastFilterIndex, cmd_BitmapFilterIndex);

            if (!access(fs2,0) && expert<2) 	// If the drawing exists...
                {
				if (sds_initget(0,ResourceString(IDC_SAVE_INITGET_YES_REPLACE_7, "Yes-Replace_it|Yes No-Cancel_Save|No ~_Yes ~_No" ))!=RTNORM) 
					{
                    rc=(-__LINE__); 
                    goto bail; 
					}
                if ((ret=sds_getkword(ResourceString(IDC_SAVE__NA_DRAWING_WITH_TH_8, "\nA drawing with this name already exists.\nDo you want to replace it? <N> " ),fs1))==RTERROR) 
					{
                    rc=(-__LINE__); 
					goto bail;
					}
				else if (ret==RTCAN) 
					{
                    rc=(-1L); 
					goto bail;
					}
				else if (ret==RTNONE) 
					{
                    rc=(-1L); 
					goto bail;
					}
				else if (ret==RTNORM) 
					{
                    if (strisame(fs1, "NO"/*DNT*/)) 
						{
						rc=(-1L); 
                        goto bail; 
						}
					}
				} 
            
            if (exportmode==1 && !cmd_access(fs2))
				{
                cmd_ErrorPrompt(CMD_ERR_EXPORTFILE,0);
			    rc=(-1L); 
				goto bail;
				}
			
            strncpy(fname,fs2,nStrLen);	// If it gets this far (no "bail"ing) copy it.
			}
        
		ChangeSlashToBackslashAndRemovePipe(fname);

		// It may happen that when filedia = 0, "fname" may not be fully qualified.
		// If "fname" does not contain the path, give the path-name of the current 
		// directory, to this fname so that it becomes fully qualified.
		if (filedia == 0)
		{
			char* pDestination = strrchr(fname,'\\'/*DNT*/);
			if (pDestination == NULL)
			{
				// Get Current Directory in which this Drawing File is to be stored
				char strCurrentPathName[IC_PATHSIZE + IC_FILESIZE];
				::GetCurrentDirectory(IC_PATHSIZE + IC_FILESIZE, strCurrentPathName);
				strcat(strCurrentPathName, "\\"/*DNT*/);
				strcat(strCurrentPathName, fname);
				strncpy(fname, strCurrentPathName, nStrLen);
			}
		}

#ifdef BUILD_WITH_VBA
		if (filedia == 0 && expert < 2) // Do not prompt the user if expert is set to 2 or greater.
		{
			char strFileNameWithVbiExt[IC_PATHSIZE + IC_FILESIZE];
			strcpy(strFileNameWithVbiExt, fname);
			ic_setext(strFileNameWithVbiExt, ".vbi"/*DNT*/);

			// Check if the VBI File exists in the specified location.
			if (!access(strFileNameWithVbiExt, 0))
			{
				char strMessage[IC_ACADBUF];
						
				sprintf (strMessage, 
						 ResourceString(IDC_SDS_GET__VBI_FILE_EXISTS__CMDLINE, "A file with the name <%s> already exists. Overwrite this File? <N>: "/*DNT*/),
						 strFileNameWithVbiExt
						);

				sds_initget(RSG_OTHER , ResourceString(IDC_SDS_GET__VBI_FILE_OVERWRITE, "Yes No ~_Yes ~_No"));
				if ((ret=sds_getkword(strMessage, fs1))==RTERROR)
				{
					rc=(-__LINE__); 
					goto bail;
				}
				else if (ret==RTCAN) 
				{
					rc=(-1L); 
					goto bail;
				}
				else if (ret==RTNONE) 
				{
					rc=(-1L); 
					goto bail;
				}
				else if (ret==RTNORM) 
				{
					if (strisame(fs1, "NO"/*DNT*/)) 
					{
						rc=(-1L); 
						goto bail; 
					}
				}
			}

		}
#endif

		if (strrchr(fname,'\\'/*DNT*/)) 
			{
			db_astrncpy(&cmd_DwgSavePath,fname,strlen(fname)+1);
			*(strrchr(cmd_DwgSavePath,'\\'/*DNT*/)+1)=0;
			}
	
        break;
		}

bail:

	return rc;
	}

void GetValidExtension(
	char* szFilename,				// i - the filename to check
	char* szExt,					// o - the appropriate extension, without the .
	const int cmd_BitmapFilterIndex)					
	{
	ASSERT(szFilename && szExt);

	char szExtension[IC_ACADBUF];
	char* pExtension;

	ic_getext(szFilename, szExtension);			// passes back a . on the extension
	if (strlen(szExtension) > 1)
	{
		// If there is more than a ., go past it.
		if ((NULL != (pExtension = strrchr(szExtension, '.'/*DNT*/))) && (strlen(pExtension) > 1))
			pExtension += 1;
		
		// If the extension isn't one of our possibilities, use dwg.
		strcpy(szExt, pExtension);
		if (strisame(pExtension, "dxf"/*DNT*/))
		{
			SDS_CURDOC->m_nFileType = IC_DXF;
		}
		else if (strisame(pExtension, "dwt"/*DNT*/))
		{
			SDS_CURDOC->m_nFileType = IC_DWT;
		}
		else if (strisame(pExtension, "bmp"/*DNT*/))
		{
			SDS_LastFilterIndex = cmd_BitmapFilterIndex;
			SDS_CURDOC->m_nFileType = -1;
		}
		else if (strisame(pExtension, "wmf"/*DNT*/))
		{
			SDS_LastFilterIndex = cmd_BitmapFilterIndex + 1;
			SDS_CURDOC->m_nFileType = -1;
		}
		else if (strisame(pExtension, "emf"/*DNT*/))
		{
			SDS_LastFilterIndex = cmd_BitmapFilterIndex + 2;
			SDS_CURDOC->m_nFileType = -1;
		}
		else if (strisame(pExtension, "svg"/*DNT*/))
		{
			SDS_LastFilterIndex = cmd_BitmapFilterIndex + 3;
			SDS_CURDOC->m_nFileType = -1;
		}
		else if (strisame(pExtension, "dwf"/*DNT*/))
		{
			SDS_LastFilterIndex = cmd_BitmapFilterIndex + 4;
			SDS_CURDOC->m_nFileType = -1;
		}
		else
		{
				strcpy(szExt, "dwg"/*DNT*/);
				SDS_LastFilterIndex = 1;
				SDS_CURDOC->m_nFileType = IC_DWG;
		}

		return;
	}

	if (SDS_CURDOC->m_nFileType == IC_DWG)
	{
		strcpy(szExt, "dwg"/*DNT*/);
		SDS_CURDOC->m_nFileType = IC_DWG;
	}
	else if (SDS_CURDOC->m_nFileType == IC_DXF || SDS_CURDOC->m_nFileType == IC_BDXF)
	{
		strcpy(szExt, "dxf"/*DNT*/);
		SDS_CURDOC->m_nFileType = (SDS_CURDOC->m_nFileType == IC_DXF) ? IC_DXF : IC_BDXF;
	}
	else if (SDS_CURDOC->m_nFileType == IC_DWT)
	{
		strcpy(szExt, "dwt"/*DNT*/);
		SDS_CURDOC->m_nFileType = IC_DWT;
	}
	else if (SDS_LastFilterIndex == cmd_BitmapFilterIndex)
	{
		strcpy(szExt, "bmp"/*DNT*/);
		SDS_CURDOC->m_nFileType = -1;
	}
	else if (SDS_LastFilterIndex == cmd_BitmapFilterIndex + 1)
	{
		strcpy(szExt, "wmf"/*DNT*/);
		SDS_CURDOC->m_nFileType = -1;
	}
	else if (SDS_LastFilterIndex == cmd_BitmapFilterIndex + 2)
	{
		strcpy(szExt, "emf"/*DNT*/);
		SDS_CURDOC->m_nFileType = -1;
	}
	else if (SDS_LastFilterIndex == cmd_BitmapFilterIndex + 3)
	{
		strcpy(szExt, "svg"/*DNT*/);
		SDS_CURDOC->m_nFileType = -1;
	}
	else if (SDS_LastFilterIndex == cmd_BitmapFilterIndex + 4)
	{
		strcpy(szExt, "dwf"/*DNT*/);
		SDS_CURDOC->m_nFileType = -1;
	}
	else
	{
		strcpy(szExt, "dwg"/*DNT*/);
		SDS_LastFilterIndex = 1;
		SDS_CURDOC->m_nFileType = IC_DWG;
	}

}

void SetLastFilterIndex(
	const char* szExtension,		// i - the extension to check, without the .
	int& nIndex,					// i/o - the index to set, which may already be valid
	const int cmd_BitmapFilterIndex)					
	{
	ASSERT(szExtension);
	
	// For dwg and dxf, there is more than one possibility; so we'll
	// default to 1 and 2, respectively, unless the index is already
	// within the valid ranges (1,4-10 and 2,11-24 respectively).
	if (strisame(szExtension, "dwg"/*DNT*/) || SDS_CURDOC->m_nFileType == IC_DWG)
	{
		nIndex = 1;
	}
	else if (strisame(szExtension, "dxf"/*DNT*/) || (SDS_CURDOC->m_nFileType == IC_DXF || SDS_CURDOC->m_nFileType == IC_BDXF))
	{
		nIndex = 2;
	}
	if (strisame(szExtension, "dwt"/*DNT*/) || SDS_CURDOC->m_nFileType == IC_DWT)
	{
		nIndex = 3;
	}

	else if (strisame(szExtension, "bmp"/*DNT*/))
		nIndex = cmd_BitmapFilterIndex;
	else if (strisame(szExtension, "wmf"/*DNT*/))
		nIndex = cmd_BitmapFilterIndex + 1;
	else if (strisame(szExtension, "emf"/*DNT*/))
		nIndex = cmd_BitmapFilterIndex + 2;

	// The filename handling needs to be more robust and completed.  In the process,
	// all of these constants need to be gotten rid of.  SDS_LastFilterIndex needs
	// to be removed and either constants or enums used for the various numeric
	// values.

	}

	
short cmd_export(void)    { 
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
	
    if (cmd_iswindowopen() == RTERROR) return RTERROR;			
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    return(cmd_SaveFunc(2,NULL));    
}  


