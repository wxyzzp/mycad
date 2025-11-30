/* \ICAD\ICADWNDACTION.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Contains the message loop for the Icad commands
 *
 */


#include "Icad.h"/*DNT*/
#include "sdsapplication.h" /*DNT*/
#include "sdsthread.h"
#include "sdsthreadjob.h"
#include "drawdevice.h" /*DNT*/
#include "IcadApp.h"/*DNT*/
#include "IcadCmdBar.h"
#include "IcadChildWnd.h"
#include "IcadTextScrn.h"
#include "IcadToolbarMain.h"/*DNT*/
#include "IcadExplorer.h"/*DNT*/
#include "IcadMenu.h"/*DNT*/
#include "IcadTipDlg.h"/*DNT*/
#include "IcadPromptMenu.h"/*DNT*/
#include "IcadRanges.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "cmdsInet.h"
#include "cmdsGroup.h"
#include "IcadCntrItem.h"/*DNT*/
#include "IcadMenuMain.h"/*DNT*/
#include "IcadOleLinksDialog.h"/*DNT*/
#include <eh.h>
#include "IcadDispID.h"
#ifndef _COMAddinManager_H
#include "COMAddinManager.h"
#endif
#include "icadgridiconsnap.h"

#include "SyncObject.h"
#include "IcadView.h"
#include "OsnapManagerImp.h"

#include "LayoutInsert.h"
#include "DialogHyperlink.h"
#include "Preferences.h"

extern struct SDS_mTextData  SDS_mTextDataPtr;
extern int	  SDS_IsDragging;
extern int	  SDS_AtCopyMoveCommand;
extern int *(*lw_render_wireframe)(void* data);
extern int *(*lw_render_hidden)(void* data);
extern int *(*lw_render_preview)(void* data);
extern int *(*lw_render_full)(void* data);
extern int *(*lw_edit_mtl)(void* data);
extern int *(*lw_edit_lights)(void* data);
extern int *(*lw_edit_bg)(void* data);
extern int *(*lw_options)(void* data);
//4M Spiros Item:31->
extern CString MdMacro;
//4M Spiros Item:31<-
extern int *(*qr_render)(void* data);
extern int *(*qr_options)(void* data);
extern int *(*qr_terminate)(void* data);
extern int *(*lw_terminate)(void* data);


#include "lisp.h"/*DNT*/
#include "CommandAtom.h"

#ifndef BUILD_WITH_VBA
HRESULT ApcFireEvent(IUnknown* punk, REFIID iid, DISPID dispid, DISPPARAMS* pDispParams /*= NULL*/, VARIANT* pVarResult /*= NULL*/)
	{
	return NOERROR;
	}
#endif

extern sds_name	SDS_EditPropsSS;
extern bool g_Vba6Installed;
extern bool cmd_InsideOfCloseCommand;
// EBATECH(watanabe)-[changed
extern double*	ptTolerance;
// ]-EBATECH(watanabe)
struct SDS_dragvars *SDS_SaveTvars;


void SDS_FatalErrorFunc(unsigned int u, EXCEPTION_POINTERS* pExp);

class SDS_Exception_Wndaction
{
private:
	unsigned int nSE;
public:
	SDS_Exception_Wndaction() {}
	SDS_Exception_Wndaction( unsigned int n ) : nSE( n ) {}
	~SDS_Exception_Wndaction() {}
	unsigned int getSeNumber() { return nSE; }
};


LRESULT
CMainWindow::OnColorDialog(WPARAM wParam,LPARAM lParam)
	{
	if (lParam)
		{
		WndActionEvent* pEvent = (WndActionEvent*) lParam;
		// EBATECH(CNBR) Add CAST operator
		int nRetVal,
			nReturnCol = (int)(short)LOWORD(wParam);
		int nRet = IcadColorDia((int)(short)LOWORD(wParam),&nReturnCol,(short)HIWORD(wParam));

		if (nRet==IDOK)
			nRetVal = RTNORM;
		else if (nRet==IDCANCEL)
			nRetVal = RTCAN;
		else
			nRetVal = RTERROR;

		//Make sure you set the return value before SetEvent
		pEvent->SetReturnVal(MAKELRESULT(nReturnCol, nRetVal));

		::SetEvent((HANDLE)(*pEvent));

		return TRUE;
		}
	else
		return FALSE;
	}

LRESULT
CMainWindow::OnPrintf(WPARAM wParam,LPARAM lParam)
{
	char *strToPrint = (char*)lParam;
	if( !CHECKSTR( strToPrint ) )
	{
		ASSERT( FALSE );
		return FALSE;
	}

	/*DG - 29.3.2002*/// Since now we don't write log file in the command line history format;
	// instead we write script-like log (in SDS_SendMessage). So the code below is commented out.
	/*
	resbuf rb;

	//DG - 4.4.2002/ We need some things for errors handling on file opening.
	CFileException	fileException;
	static bool		bFileAlertShown = false;

	SDS_getvar(NULL,DB_QLOGFILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint)
	{
		if(SDS_CMainWindow->m_fLogFile.m_hFile==0)
		{
			SDS_getvar(NULL,DB_QLOGFILENAME,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if(SDS_CMainWindow->m_fLogFile.Open(rb.resval.rstring,CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate, &fileException))
			{
				SDS_CMainWindow->m_fLogFile.SeekToEnd();
				struct tm *newtime;
				time_t aclock;
				time( &aclock ); // Get time in seconds
				newtime = localtime( &aclock );  // Convert time to struct
				char *fcp1="\r\n[ IntelliCAD - "/DNT/;
				SDS_CMainWindow->m_fLogFile.Write(fcp1,strlen(fcp1));
				fcp1=asctime(newtime);
				SDS_CMainWindow->m_fLogFile.Write(fcp1,strlen(fcp1)-1);
				fcp1=" ]\r\n"/DNT/;
				SDS_CMainWindow->m_fLogFile.Write(fcp1,strlen(fcp1));
			}
			else
				{
					//DG - 4.4.2002/ Show alert message and nullify the file handle (because CFile::Open changes the handle).
					if(!bFileAlertShown)
					{

						TCHAR	osErrorMessage[1024], ourErrorMessage[128];
						fileException.GetErrorMessage(osErrorMessage, 1024);
						sprintf(ourErrorMessage, ResourceString(IDC_ICADTOOLBARMAIN_CAN_T_OP_4, "Can't open file %s, error = %u\n"), rb.resval.rstring, fileException.m_cause);
						CString	errorMessage(ourErrorMessage);
						errorMessage += '(';
						errorMessage += osErrorMessage;
						errorMessage += ')';
						sds_alert((LPCTSTR)errorMessage);
						bFileAlertShown = true;
					}
					SDS_CMainWindow->m_fLogFile.m_hFile = 0;
				}
			free(rb.resval.rstring);
		}
		if(SDS_CMainWindow->m_fLogFile.m_hFile!=0)
		{
//			char *fcp1=m_strPrintfWndAction.GetBuffer(0);
			char *fcp1 = strToPrint;
			for(int ct=0; ct<(int)strlen(fcp1); ++ct)
			{
				// Deal with the backspace.
				if(*(fcp1+ct)=='\b'/DNT/)
				{
					SDS_CMainWindow->m_fLogFile.Seek(-1,CFile::current);
					continue;
				}
				if(*(fcp1+ct)=='\n'/DNT/)
					SDS_CMainWindow->m_fLogFile.Write("\r\n"/DNT/,2);
				else
					SDS_CMainWindow->m_fLogFile.Write(fcp1+ct,1);
			}
		}
	}
	else
	{
		if(SDS_CMainWindow->m_fLogFile.m_hFile!=0)
		{
			SDS_CMainWindow->m_fLogFile.Close();
		}
		SDS_CMainWindow->m_fLogFile.m_hFile=0;
		bFileAlertShown = false;
	}
	*/

	PrintOnCommand( strToPrint );
	return TRUE;
}

void CMainWindow::IcadWndAction(UINT nId)
	{
	IcadWndAction( nId, 0);
	}


void CMainWindow::IcadWndAction(UINT nId, LPARAM lParam)
{
	BOOL bShowToolbars=TRUE; //*** Used in ICAD_WNDACTION_SHOWTOOLBARS and ICAD_WNDACTION_HIDETOOLBARS

	ASSERT_VALID( this );

	// Moved the SAVE stuff to its own switch so I could add the try - catch crap.
	switch(nId)
	{
 		case ICAD_WNDACTION_FILESAVE:	//*** File - Save
			{
#ifdef CRIPPLE_TESTDRIVE
				if (CIcadApp::IsTestDrive())
				{
					GetApp()->m_pMainWnd->MessageBox(
						ResourceString(IDS_TESTDRIVE_WARNING, "Sorry Testdrive software cannot save files.  Please register to receive the full version.",),
						ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD"),
						MB_OK | MB_ICONEXCLAMATION);
					break;
				}
#endif

				CIcadDoc* pIcadDoc;

				if(NULL==m_pvWndAction)
				{
					break;
				}
				pIcadDoc=(CIcadDoc*)m_pvWndAction;
				m_pvWndAction=NULL;
/*
					//*** Should not happen, but just in case, point to current doc.
					CIcadChildWnd* pChild;
					if((pChild=(CIcadChildWnd*)MDIGetActive(NULL))==NULL)
					{
						break;
					}
					if((pDoc=(CIcadDoc*)(pChild->GetActiveDocument()))==NULL)
					{
						break;
					}
*/
				if(pIcadDoc->IsEmbedded())
				{
					m_bWndAction=pIcadDoc->SaveModified( FALSE );
				}
				else
				{
					m_bWndAction=pIcadDoc->OnSaveDocument(m_strWndAction);
				}
			}
			break;
 		case ICAD_WNDACTION_SAVEALL:   //*** SaveAll
			{

#ifdef CRIPPLE_TESTDRIVE
				if (CIcadApp::IsTestDrive())
				{
					GetApp()->m_pMainWnd->MessageBox(
						ResourceString(IDS_TESTDRIVE_WARNING, "Sorry Testdrive software cannot save files.  Please register to receive the full version.",),
						ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD"),
						MB_OK | MB_ICONEXCLAMATION);
					break;
				}
#endif

				extern int SDS_AutoSave;
				CIcadChildWnd* pChild = (CIcadChildWnd*)MDIGetActive(NULL);
				if(pChild==NULL) break;
				CIcadDoc* pDoc = (CIcadDoc*)pChild->GetActiveDocument();
				if(pDoc==NULL) break;
				CDocTemplate* pDocTemplate = pDoc->GetDocTemplate();
				POSITION pos = pDocTemplate->GetFirstDocPosition();
				CString strDwg;
				while(pos!=NULL)
				{
					pDoc = (CIcadDoc*)pDocTemplate->GetNextDoc(pos);
					strDwg=pDoc->GetPathName();
					if(strDwg.IsEmpty())
					{
						char fname[IC_PATHSIZE+IC_FILESIZE];
						strncpy(fname,pDoc->GetTitle(),sizeof(fname)-1);
						if(!SDS_AutoSave)
						{
							if(cmd_SaveAs(pDoc->m_dbDrawing,fname,sizeof(fname)-1,0)!=0)
							{
								continue;
							}
						}
						strDwg=fname;
					}
					struct resbuf dbmod;
					SDS_getvar(NULL,DB_QDBMOD,&dbmod,pDoc->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
					if(SDS_AutoSave==1 && !dbmod.resval.rint) continue;
					pDoc->OnSaveDocument(strDwg);
					if(SDS_AutoSave==-1) {
						dbmod.resval.rint=0;
						SDS_setvar(NULL,DB_QDBMOD,&dbmod,pDoc->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);
					}
				}

			}
			break;
		case ICAD_WNDACTION_EXIT:	   //*** File - Exit
			if(qr_terminate)			/*D.G.*///	It's for good style.
				(*qr_terminate)(NULL);	//		  /
			if(lw_terminate)			//		/
				(*lw_terminate)(NULL);	//	  /

			if(!CloseApp())
			{
				//*** If CloseApp() returned FALSE, "this" has been destroyed (because
				//*** an embedded OLE doc was closed), so break;
				break;
			}
			//*** If the following flag doesn't get set, then the user canceled
			//*** the close - be sure to check in cmd_end and cmd_exit.
			if(!m_nIsClosing)
			{
				break;
			}

/////////////////////////////////////////////////////////////////////////////////////////////////////
			// Send RQQUIT to all apps from here insteda of cmd_quit() function
			// This should be sent using SendJob to make it syncronous
			// SDS_BroadcastRQToSDSApps doesn't let you do so 'cause it'll call ::SendNotifyMessage
			// which will not wait till the completion unless it's the same thread
			ASSERT( SDSApplication::GetEngineApplication() != NULL );
			ASSERT( SDSApplication::GetEngineApplication()->GetMainThread() != NULL );
			SDSThread *pMainThread = SDSApplication::GetEngineApplication()->GetMainThread();
			BroadcastRQJob	*pJob = new BroadcastRQJob(SDS_RQQUIT);
			pMainThread->SendJob( pJob );
/////////////////////////////////////////////////////////////////////////////////////////////////////

			// then destroy the window
			if(!m_bAlreadyDestroyed)
			{
				DestroyWindow();
			}
			break;
	}

#if !defined(DEBUG)
	try
	{
	_set_se_translator(SDS_FatalErrorFunc);
#endif

	switch(nId)
	{
//		case ICAD_WNDACTION_GPS_PROP:
//			{
//				CGPSDlg temp(IDD_GPS);
//				temp.DoModal();
//			}
		case ICAD_WNDACTION_GRAPHSCR:
			//*** Destroys the text screen.
			m_pTextScrn->ShowWindow(SW_HIDE,TRUE);
			break;
		case ICAD_WNDACTION_TEXTPAGE:
			{
				//*** Displays and clears the text screen.
				CIcadPromptWndList* pPromptWndList = m_pTextScrn->m_pPromptWnd->m_pPromptWndList;
				pPromptWndList->m_pHistViewList->RemoveAll();
				m_pHistList->RemoveAll();
				CRect rect;
				pPromptWndList->GetClientRect(rect);
				pPromptWndList->m_pMemDC->FillRect(rect,pPromptWndList->m_pbrBkgnd);
				pPromptWndList->InvalidateRect(rect);
				//*** Continue to the next case to show the window
//				m_pTextScrn->ShowWindow(SW_SHOW,TRUE);
//				m_pTextScrn->UpdateWindow();
			}
//			break;
		case ICAD_WNDACTION_TEXTSCR:
			{
				//*** Displays the text screen.
				int nShow = SW_SHOW;
				WINDOWPLACEMENT wp;
				m_pTextScrn->GetWindowPlacement(&wp);
				if(wp.showCmd==SW_SHOWMINIMIZED || wp.showCmd==SW_MINIMIZE)
				{
					//*** If the window is minimized, restore it.
					nShow=SW_RESTORE;
				}
				m_pTextScrn->ShowWindow(nShow,TRUE);
				m_pTextScrn->UpdateWindow();
			}
			break;
		case ICAD_WNDACTION_ATTE:
			{
				extern sds_name SDS_AttEditName;
				IcadAtteDia(SDS_AttEditName);
			}
			break;
		case ICAD_WNDACTION_CONFIG:
			IcadConfigDia();
			break;
		case ICAD_WNDACTION_TBLMGR:
			IcadExplorer(IEXP_LATAB_IDX);
			break;
 		case ICAD_WNDACTION_FILENEW:	//*** File - New
			SDS_CMainWindow->m_bInOpenNew=1;
			((CIcadApp*)AfxGetApp())->FileNew();
			SDS_CMainWindow->m_bInOpenNew=0;
			break;
 		case ICAD_WNDACTION_FILEOPEN:	//*** File - Open
			SDS_CMainWindow->m_bInOpenNew=1;
			((CIcadApp*)AfxGetApp())->FileOpen(m_strOpenFname);
			SDS_CMainWindow->m_bInOpenNew=0;
			m_strOpenFname.Empty();
			break;
 		case ICAD_WNDACTION_FILECLOSE:	//*** File - Close
			{
				CIcadChildWnd* pChild;
				if((pChild=(CIcadChildWnd*)MDIGetActive(NULL))==NULL)
					break;

				CIcadDoc* pDoc;
				if((pDoc=(CIcadDoc*)(pChild->GetActiveDocument()))==NULL)
					break;

				cmd_InsideOfCloseCommand = TRUE;
				if(pDoc->SaveModified())
				{
					pDoc->OnCloseDocument();
				}
				cmd_InsideOfCloseCommand = FALSE;
			}
			break;
 		case ICAD_WNDACTION_ISDOCEMBEDDED:
			{
				//*** Its stupid that we need a WndAction for this, but...
				CIcadDoc* pDoc = (CIcadDoc*)m_pvWndAction;
				if(NULL==pDoc)
				{
					break;
				}
				m_pvWndAction=NULL;
				m_bWndAction=pDoc->IsEmbedded();
			}
			break;
		case ICAD_WNDACTION_PRINT:	   //*** File - Print
			{
#ifdef CRIPPLE_TESTDRIVE
				if (CIcadApp::IsTestDrive())
				{
					GetApp()->m_pMainWnd->MessageBox(
						ResourceString(IDS_TESTDRIVE_WARNING, "Sorry Testdrive software cannot print files.  Please register to receive the full version.",),
						ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD"),
						MB_OK | MB_ICONEXCLAMATION);
					break;
				}
#endif

				CIcadView* pView = GetIcadView();
				if(NULL!=pView)
				{
					pView->FilePrint(TRUE);
				}
			}
			break;
		case ICAD_WNDACTION_QPRINT:
			{
#ifdef CRIPPLE_TESTDRIVE
				if (CIcadApp::IsTestDrive())
				{
					GetApp()->m_pMainWnd->MessageBox(
						ResourceString(IDS_TESTDRIVE_WARNING, "Sorry Testdrive software cannot print files.  Please register to receive the full version.",),
						ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD"),
						MB_OK | MB_ICONEXCLAMATION);
					break;
				}
#endif
				CIcadView* pView = GetIcadView();
				if(NULL!=pView)
				{
					pView->FilePrint(FALSE);
				}
			}
			break;
		case ICAD_WNDACTION_PPREVIEW:  //*** File - Print Preview
			if(!m_bPrintPreview) //*** Don't allow re-entry.
			{
				CIcadView* pView = GetIcadView();
				if(NULL!=pView)
				{
					pView->FilePrintPreview();
				}
			}
			break;
		case ICAD_WNDACTION_PSETUP:    //*** File - Print Setup
			((CIcadApp*)AfxGetApp())->FilePrinterSetup();
			break;
		case ICAD_WNDACTION_WOPEN:	   //*** Window - New Window
			OnWindowNew();
			break;
 		case ICAD_WNDACTION_WSPLIT:    //*** Window - Split
			if(GetIcadView()==NULL) break;
			((CIcadChildWnd*)MDIGetActive(NULL))->m_wndSplitter.DoKeyboardSplit();
			break;
		case ICAD_WNDACTION_WCLOSE:		//*** Window - Hide
			{
				if(GetIcadView()==NULL)
					break;
				cmd_InsideOfCloseCommand = TRUE;
				((CIcadChildWnd*)MDIGetActive(NULL))->OnChildWndClose();
				cmd_InsideOfCloseCommand = FALSE;
			}
			break;
		case ICAD_WNDACTION_WCASCADE:  //*** Window - Cascade
			{
				if(GetIcadView()==NULL) break;
				cmd_InsideViewPorts=TRUE;
				MDICascade();
			}
			break;
		case ICAD_WNDACTION_WHTILE:    //*** Window - Tile Horizontally
			{
				CIcadView* pView = SDS_CURVIEW;
				if(GetIcadView()==NULL) break;
				MDITile(MDITILE_HORIZONTAL);
				//if(pView) ((CIcadChildWnd*)pView->GetParentFrame())->SetFocus();	Bug 46371
			}
			break;
		case ICAD_WNDACTION_WIARANGE:	 //*** Window - Arange icons
			if(GetIcadView()==NULL) break;
			MDIIconArrange();
			break;
		case ICAD_WNDACTION_WVTILE:    //*** Window - Tile Vertically
			{
				CIcadView* pView = SDS_CURVIEW;
				if(GetIcadView()==NULL) break;
				MDITile(MDITILE_VERTICAL);
				//if(pView) ((CIcadChildWnd*)pView->GetParentFrame())->SetFocus(); Bug 46371
			}
			break;
 		case ICAD_WNDACTION_WCLOSEALL: //*** Window - Close All
			{
				if(GetIcadView()==NULL)
					break;

				cmd_InsideOfCloseCommand = TRUE;
				CIcadChildWnd* pChild = (CIcadChildWnd*)MDIGetActive(NULL);
				CIcadChildWnd* pNextChild = NULL;
				while(((CMainWindow*)pChild)!=this) {
					if(pChild==NULL) break;
					pChild->OnChildWndClose();
					pNextChild=(CIcadChildWnd*)MDIGetActive(NULL);
					//*** If the following check succeeds, the user picked the Cancel button,
					//*** or tried to close an embedded document (in which case, we have a
					//*** small bug that we may need to work around, some day).
					if(pChild==pNextChild) break;
					pChild=pNextChild;
				}
				cmd_InsideOfCloseCommand = FALSE;
			}
			break;
 		case ICAD_WNDACTION_WINDOWS:   //*** Window - Windows
			// TODO Add dialog call here.
			break;
		case ICAD_WNDACTION_OSNAP:
			IcadSetvarsDia();
			break;
		case ICAD_WNDACTION_DIMVARS:
			IcadDimDia();
			struct resbuf rb1;
			SDS_getvar(NULL,DB_QDIASTAT,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if(rb1.resval.rint == 1)//Autodsys 072704
			{	/*D.Gavrilov*/// And now we must regenerate existing dimensions.
				// We'll do it like as in ICAD_WNDACTION_MODIFY case without
				// calling EntProp dialog.
				struct resbuf* rb_filter = NULL;
				rb_filter = sds_buildlist(RTDXF0, "DIMENSION,TOLERANCE" /*DNT*/, 0);
				if( rb_filter )
				{
					SDS_PickObjects("ALL"/*DNT*/,
									NULL,		//pt1
									NULL,		//pt2
									rb_filter,	//filter
									SDS_EditPropsSS,	//ss
									1,			//mode
									SDS_CURDWG,	//drawing
									true,		//bIncludeInvisible
									false,		//bSpecialImageFilter
									true		//bIncludeLockedLayers
									);
					sds_relrb( rb_filter );
				}
			}
			IcadEntProps(false);
			break;
		case ICAD_WNDACTION_XREF:
			IcadXrefDia();
			break;
		case ICAD_WNDACTION_INSERT:
			if (m_strOpenFname.IsEmpty())
				IcadInsertDia();
			else
				IcadInsertDia(m_strOpenFname.GetBuffer(0));
			m_strOpenFname.Empty();
			break;
		case ICAD_WNDACTION_BPOLY:
			IcadBoundaryPolyDia();
			break;
		case ICAD_WNDACTION_BHATCH:
			IcadBoundaryHatchDia(SDS_EditPropsSS);
			break;
		case ICAD_WNDACTION_TEXT_STYLE:
			IcadTextStyleDia();
			break;
		

		case ICAD_WNDACTION_ATTDEF:
			IcadAttDefDia();
			break;
		case ICAD_WNDACTION_INSNAME:
			IcadNameDia(0);
			break;
		case ICAD_WNDACTION_XREFDUP:
			IcadNameDia(1);
			break;
		case ICAD_WNDACTION_INSDUP:
			IcadNameDia(2);
			break;
		case ICAD_WNDACTION_INSCURDWG:
			IcadNameDia(3);
			break;
		case ICAD_WNDACTION_SETVAR:
			IcadSetvarsDia();
			break;
		case ICAD_WNDACTION_UDCOORDS:
			{
				TEXTMETRIC tm;
				HGDIOBJ hOldFont=NULL;
				CClientDC dcScreen(NULL);
				CString strTmp(m_strCoords);
				if(m_StatusBar.m_hFont!=NULL) hOldFont=dcScreen.SelectObject(m_StatusBar.m_hFont);
				dcScreen.GetTextMetrics(&tm);

				m_nMaxCoordsLen=strlen(m_strCoords);
				if(m_nMaxCoordsLen<20) m_nMaxCoordsLen=20;
				
				m_StatusBar.SetCoordsPaneWidth( (tm.tmAveCharWidth * m_nMaxCoordsLen) + 60);  //*** Width of the COORDS pane
				
				if(hOldFont!=NULL) dcScreen.SelectObject(hOldFont);
				//*** Resize the tooltips.
				m_StatusBar.ResizeToolTips();
			}

			m_StatusBar.SetCoordsText(m_strCoords);
			break;

		case ICAD_WNDACTION_UDSTLAY:
			m_StatusBar.UpdateCurLay();
			if(m_wndLayers.GetSafeHwnd())
				m_wndLayers.UpdateLayerList(); // TECNOBIT(Todeschini) 2003.09.05
			break;
		case ICAD_WNDACTION_UDSTLTP:
			m_StatusBar.UpdateCurLType();
			if(m_wndLType.GetSafeHwnd())
				m_wndLType.UpdateLTypeList(); // TECNOBIT(Todeschini) 2003.15.05
			break;
		case ICAD_WNDACTION_UDSTCOL:
			m_StatusBar.UpdateCurCol();
			if(m_wndLayers.GetSafeHwnd())
				m_wndLayers.Invalidate(); // TECNOBIT(Todeschini) 2003.12.05
			break;
		// EBATECH-[add events on StatusBar.
		case ICAD_WNDACTION_UDSTSTY:
			m_StatusBar.UpdateCurStyle();
            break;
		case ICAD_WNDACTION_UDSTDIM:
			m_StatusBar.UpdateCurDimSt();
			if (m_wndDimstyle.GetSafeHwnd())
				m_wndDimstyle.UpdateDimStyleList();
            break;
		// ]-EBATECH
		// EBATECH(CNBR) -[ Lineweight
		case ICAD_WNDACTION_UDSTLW:
			m_StatusBar.UpdateCurLw();
			if(m_wndLWeight.GetSafeHwnd())
				m_wndLWeight.UpdateLWeightList(); 
			break;
		// EBATECH(CNBR) ]-
		case ICAD_WNDACTION_TBCONFIG:
			IcadToolbars();
			break;
 		case ICAD_WNDACTION_UDSBARS:
			if(SDS_CURVIEW==NULL) break;
			SDS_CURVIEW->UpdateScrollBars();
			break;
 		case ICAD_WNDACTION_TIPOFDAY:
			{
				CTipDlg TipDlg;
				TipDlg.DoModal();
			}
			break;
 		case ICAD_WNDACTION_RCLICK:
			{
				struct sds_resbuf rb;
				long lflgEntVis=0L;

				//*** Check for entities in selection set and adjust
				//*** the entity visibility flag accordingly.
				long fl1=0L;
				if(SDS_CURDOC && RTNORM==sds_sslength(SDS_CURDOC->m_pGripSelection,&fl1) && fl1>0L) {
					if(fl1>1L) lflgEntVis|=ICAD_MN_MUL;
					sds_name ename;
					fl1=0L;
					while(RTNORM==sds_ssname(SDS_CURDOC->m_pGripSelection,fl1,ename)) {
						switch(IC_TYPE_FROM_ENAME(ename)) {
							case DB_POINT:	   lflgEntVis|=ICAD_MN_PNT;  break;
							case DB_LINE:	   lflgEntVis|=ICAD_MN_LIN;  break;
							case DB_RAY:	   lflgEntVis|=ICAD_MN_RAY;  break;
							case DB_XLINE:	   lflgEntVis|=ICAD_MN_XLN;  break;
							case DB_ARC:	   lflgEntVis|=ICAD_MN_ARC;  break;
							case DB_CIRCLE:    lflgEntVis|=ICAD_MN_CIR;  break;
							case DB_ELLIPSE:   lflgEntVis|=ICAD_MN_ELL;  break;
							case DB_SHAPE:	   lflgEntVis|=ICAD_MN_SHP;  break;
							case DB_TRACE:	   lflgEntVis|=ICAD_MN_TRC;  break;
							case DB_SOLID:	   lflgEntVis|=ICAD_MN_SLD;  break;
							case DB_POLYLINE:  lflgEntVis|=ICAD_MN_PLN;  break;
							case DB_TEXT:	   lflgEntVis|=ICAD_MN_TXT;  break;
							case DB_INSERT:    lflgEntVis|=ICAD_MN_INS;  break;
							case DB_ATTDEF:    lflgEntVis|=ICAD_MN_ATD;  break;
							case DB_DIMENSION: lflgEntVis|=ICAD_MN_DIM;  break;
							case DB_LEADER:    lflgEntVis|=ICAD_MN_LDR;  break;
							case DB_TOLERANCE: lflgEntVis|=ICAD_MN_TOL;  break;
							case DB_SPLINE:    lflgEntVis|=ICAD_MN_SPL;  break;
							case DB_MTEXT:	   lflgEntVis|=ICAD_MN_MTX;  break;
							case DB_MLINE:	   lflgEntVis|=ICAD_MN_MLN;  break;
							case DB_3DFACE:    lflgEntVis|=ICAD_MN_3DF;  break;
							case DB_IMAGE:	   lflgEntVis|=ICAD_MN_IMG;  break;
							case DB_VIEWPORT:  lflgEntVis|=ICAD_MN_VPT;  break;
							case DB_HATCH:	   lflgEntVis|=ICAD_MN_HAT;  break;
							case DB_LWPOLYLINE:lflgEntVis|=ICAD_MN_2DP;  break; /*D.Gavrilov*/// we shouldn't forget LWplines too
							case DB_3DSOLID:   lflgEntVis|=ICAD_MN_3DS;  break;	/*D.G.*/// ACIS
							case DB_BODY:	   lflgEntVis|=ICAD_MN_BDY;  break;	// entities
							case DB_REGION:    lflgEntVis|=ICAD_MN_RGN;  break;	// too
						}
						++fl1;
					}
				}

				long lflgVisibility=ICAD_MN_RCO;
				sds_getvar("EXPLEVEL"/*DNT*/,&rb);
				switch(rb.resval.rint)
				{
					case 1: lflgVisibility|=ICAD_MN_BEG; break;
					case 2: lflgVisibility|=ICAD_MN_INT; break;
					case 3: lflgVisibility|=ICAD_MN_EXP; break;
				}

				CIcadMenu* pIcadMenu = NULL;
				if(NULL!=m_pMenuMain)
				{
					pIcadMenu=m_pMenuMain->GetCurrentMenu();
				}
				if(NULL==pIcadMenu)
				{
					break;
				}
				int idxModifyMenu = pIcadMenu->m_idxModifyMenu;
				if(idxModifyMenu==(-1)) break;
				CMenu* pPopup = new CMenu();
				HMENU hSubMenu = pIcadMenu->SubMenuCreator(&idxModifyMenu,lflgVisibility,lflgEntVis);
				if(hSubMenu!=NULL)
				{
					pPopup->Attach(hSubMenu);

#ifndef _SUPPORT_HYPERLINKS_
					pPopup->RemoveMenu(pPopup->GetMenuItemCount() - 1, MF_BYPOSITION);
#else
					sds_name ename;
					if(UserPreference::s_bEnableHyperlinkMenu == false)
						pPopup->RemoveMenu(pPopup->GetMenuItemCount() - 1, MF_BYPOSITION);
					else
						if(fl1 != 1 ||
							sds_ssname(SDS_CURDOC->m_pGripSelection, 0, ename) != RTNORM ||
							cmd_getEntityHL(ename, NULL, NULL, NULL) == 0)
							pPopup->EnableMenuItem(pPopup->GetMenuItemCount() - 1, MF_BYPOSITION | MF_GRAYED);
#endif
					POINT pt;
					::GetCursorPos(&pt);
					pPopup->TrackPopupMenu(TPM_RIGHTBUTTON,pt.x,pt.y,this);
				}
				delete pPopup;
			}
			break;
 		case ICAD_WNDACTION_UDSCRLBAR:
			if ( m_pCurView )
				{
				struct resbuf rb;
				SDS_getvar(NULL,DB_QWNDLSCRL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if(rb.resval.rint==1)
					m_pCurView->ShowScrollBar( SB_BOTH, true);
				else
					m_pCurView->ShowScrollBar( SB_BOTH, false);
				}
//4M Spiros Item:31->
			if (MdMacro.GetLength())
				SDS_PrintOnStatus(-1,MdMacro,0);
			else
//4M Spiros Item:31<-
            SDS_PrintOnStatus(-1,ResourceString(IDC_ICADWNDACTION_READY_1, "Ready" ),0);
			break;
 		case ICAD_WNDACTION_UDSTATBAR:
			{
				struct resbuf rb;
				SDS_getvar(NULL,DB_QWNDLSTAT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if(rb.resval.rint==1) ShowControlBar((CControlBar *)&m_StatusBar,TRUE,FALSE);
				else ShowControlBar((CControlBar *)&m_StatusBar,FALSE,FALSE);
			}
//4M Spiros Item:31->
			if (MdMacro.GetLength())
				SDS_PrintOnStatus(-1,MdMacro,0);
			else
//4M Spiros Item:31<-
            SDS_PrintOnStatus(-1,ResourceString(IDC_ICADWNDACTION_READY_1, "Ready" ),0);
			break;
 		case ICAD_WNDACTION_UDCMDBAR:
			m_pCmdBar->UpdateCmdBar();
			break;
 		case ICAD_WNDACTION_UDTBARS:
			{
				sds_resbuf rb;
				BOOL bUpdate = FALSE;
				SDS_getvar(NULL,DB_QTBSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				CIcadToolBarMain* pTbMain = (CIcadToolBarMain*)m_pToolBarMain;
				if(rb.resval.rint==1 && !pTbMain->m_bLargeButtons)
				{
					pTbMain->m_bLargeButtons=TRUE;
					bUpdate=TRUE;
				}
				else if(rb.resval.rint==0 && pTbMain->m_bLargeButtons)
				{
					pTbMain->m_bLargeButtons=FALSE;
					bUpdate=TRUE;
				}
				SDS_getvar(NULL,DB_QTBCOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if(rb.resval.rint==1 && !pTbMain->m_bColorButtons)
				{
					pTbMain->m_bColorButtons=TRUE;
					bUpdate=TRUE;
				}
				else if(rb.resval.rint==0 && pTbMain->m_bColorButtons)
				{
					pTbMain->m_bColorButtons=FALSE;
					bUpdate=TRUE;
				}
				if(bUpdate)
				{
					//*** We don't want to update the bitmaps unnecessarily.
					pTbMain->UpdateBitmaps();
				}
			}
			break;
 		case ICAD_WNDACTION_UDCHKSTAT:
			{
				// Update the check marks and greying of the menu.
				extern char *SDS_UpdateSetvar;
				if(NULL==m_pMenuMain)
				{
					break;
				}
				CIcadMenu* pMenu = m_pMenuMain->GetCurrentMenu();
				if(NULL!=pMenu)
				{
					pMenu->UpdateCheckState(SDS_UpdateSetvar,NULL);
				}
			}
			break;
 		case ICAD_WNDACTION_UDGRASCR:
			{
				WINDOWPLACEMENT wp;
				GetWindowPlacement(&wp);
				struct sds_resbuf rb;
				SDS_getvar(NULL,DB_QWNDPMAIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				wp.rcNormalPosition.left=(long)rb.resval.rpoint[0];
				wp.rcNormalPosition.top=(long)rb.resval.rpoint[1];
				SDS_getvar(NULL,DB_QWNDSMAIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				wp.rcNormalPosition.right=wp.rcNormalPosition.left+(long)rb.resval.rpoint[0];
				wp.rcNormalPosition.bottom=wp.rcNormalPosition.top+(long)rb.resval.rpoint[1];

				SDS_getvar(NULL,DB_QWNDLMAIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if(rb.resval.rint==0) {
					wp.showCmd=SW_SHOWNORMAL;
				} else if(rb.resval.rint==1) {
					wp.showCmd=SW_SHOWMINIMIZED;
				} else if(rb.resval.rint==2) {
					wp.showCmd=SW_SHOWMAXIMIZED;
				}
				SetWindowPlacement(&wp);
			}
			break;
		case ICAD_WNDACTION_UDMDIWND:
			{
				if(GetIcadView()==NULL) break;

				ASSERT_VALID( GetIcadView() );

				struct resbuf rb;
				SDS_getvar(NULL,DB_QWNDLMDI,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if(rb.resval.rint==0)
				{
					CIcadChildWnd* pChild = (CIcadChildWnd*)MDIGetActive(NULL);
					while(((CMainWindow*)pChild)!=this)
					{
						if(NULL!=pChild)
						{
							ASSERT_VALID( pChild );
							pChild->MDIDestroy();
						}
						else
						{
						// If pChild == NULL, get out...
						//
						break;
						}
						pChild=(CIcadChildWnd*)MDIGetActive(NULL);
					}
				}
				else if(rb.resval.rint==1)
				{
					MDIRestore(MDIGetActive(NULL));
				}
				else if(rb.resval.rint==2)
				{
					MDIMaximize(MDIGetActive(NULL));
				}
			}
			break;
 		case ICAD_WNDACTION_UDTXTSCR:
			{
				//*** Update the position, size, and location of the text screen
				//*** from the Icad variables.
				struct resbuf rb;
				SDS_getvar(NULL,DB_QWNDPTEXT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				CRect rectTextScrn;
				rectTextScrn.left=(long)rb.resval.rpoint[0];
				rectTextScrn.top=(long)rb.resval.rpoint[1];
				SDS_getvar(NULL,DB_QWNDSTEXT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				rectTextScrn.right=(rectTextScrn.left+(long)rb.resval.rpoint[0]);
				rectTextScrn.bottom=(rectTextScrn.top+(long)rb.resval.rpoint[1]);
				SDS_getvar(NULL,DB_QWNDLTEXT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				int nLoc = rb.resval.rint;
				m_pTextScrn->MoveWindow(rectTextScrn);
				if(nLoc==0)
				{
					m_pTextScrn->ShowWindow(SW_HIDE,FALSE);
				}
				else if(nLoc==1)
				{
					m_pTextScrn->ShowWindow(SW_SHOWNORMAL,FALSE);
				}
				else if(nLoc==2)
				{
					m_pTextScrn->ShowWindow(SW_SHOWMINIMIZED,FALSE);
				}
				else if(nLoc==3)
				{
					m_pTextScrn->ShowWindow(SW_SHOWMAXIMIZED,FALSE);
				}
			}
			break;
 		case ICAD_WNDACTION_MODIFY:
			IcadEntProps();
			break;
 		case ICAD_WNDACTION_PROMPTM:
			{
				char *pStr=NULL,*cptr1,*cptr2,*cmd;
				pStr= new char [strlen(SDSApplication::GetActiveApplication()->GetKeywords())+1];
				strcpy(pStr,SDSApplication::GetActiveApplication()->GetKeywords());
				//*** Get the position of the prompt menu.
				struct resbuf rb;
				SDS_getvar(NULL,DB_QWNDPPMENU,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				CPoint point((int)rb.resval.rpoint[0],(int)rb.resval.rpoint[1]);
				//*** Create the prompt menu
				m_pPromptMenu = new CPromptMenu();
				m_pPromptMenu->CreatePromptMenu(point,this);
				for(cptr1=cptr2=pStr; *cptr2!=0; cptr2++)
				{
					if(*cptr2==' '/*DNT*/) {
						*cptr2=0;
						cmd=cptr1;
						if(strchr(cptr1,'|'/*DNT*/)) {
							cmd=strchr(cptr1,'|'/*DNT*/);
							*cmd=0; ++cmd;
							// EBATECH(Futagami) for MBCS String aware-[
							for(int fi1=0; cptr1[fi1]; fi1++){
								if ((_MBC_LEAD ==_mbbtype((unsigned char)cptr1[fi1],0)) &&
									(_MBC_TRAIL==_mbbtype((unsigned char)cptr1[fi1+1],1)) ){
									fi1++;
									continue;
								}
								if(cptr1[fi1]=='_'/*DNT*/) cptr1[fi1]=' '/*DNT*/;
							}
							//]-
						}
						if((*cptr1!='~'/*DNT*/ && *cptr1!=0) || (*cptr1=='~'/*DNT*/ && (cptr1+1)==cptr2))
							m_pPromptMenu->AppendMenu(cptr1,cmd);
						*cptr2=' '/*DNT*/;
						cptr1=cptr2+1;
					}
				}
				cmd=cptr1;
				if(strchr(cptr1,'|'/*DNT*/)) {
					cmd=strchr(cptr1,'|'/*DNT*/);
					*cmd=0; ++cmd;
					// EBATECH(Futagami) for MBCS String aware-[
					for(int fi1=0; cptr1[fi1]; fi1++){
						//for MBCS
						if ((_MBC_LEAD ==_mbbtype((unsigned char)cptr1[fi1],0)) &&
							(_MBC_TRAIL==_mbbtype((unsigned char)cptr1[fi1+1],1)) ){
							fi1++;
							continue;
						}
						if(cptr1[fi1]=='_'/*DNT*/) cptr1[fi1]=' '/*DNT*/;
					}
					// ]-
				}
				if((*cptr1!='~'/*DNT*/ && *cptr1!=0/*DNT*/) || (*cptr1=='~'/*DNT*/ && (cptr1+1)==cptr2)) {
					m_pPromptMenu->AppendMenu(cptr1,cmd);
				}
				//*** Add the "Cancel" menu item.
				if(!(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NOCANCEL))
					{
					m_pPromptMenu->AppendMenu("~"/*DNT*/,""/*DNT*/);
					m_pPromptMenu->AppendMenu(ResourceString(IDC_ICADWNDACTION_CANCEL_4, "Cancel" ),""/*DNT*/);
					}
				//*** Display the prompt menu
				m_pPromptMenu->TrackPromptMenu();
				IC_FREE(pStr);
			}
			break;
 		case ICAD_WNDACTION_DELPMTM:
			//*** Destroy the prompt menu
			if(m_pPromptMenu==NULL) break;
			m_pPromptMenu->DestroyWindow();
			m_pPromptMenu=NULL;
			break;
 		case ICAD_WNDACTION_HIDEPMTM:
			//*** Hide the prompt menu
			if(m_pPromptMenu==NULL) break;
			m_pPromptMenu->Hide();
			break;
 		case ICAD_WNDACTION_SHOWPMTM:
			//*** Show a hidden prompt menu
			if(m_pPromptMenu==NULL) break;
			m_pPromptMenu->ShowWindow(SW_SHOW);
			break;
 		case ICAD_WNDACTION_LOADMENU:
			{
				if(NULL==m_pMenuMain)
				{
					break;
				}
				CString strMnuFileName(m_strWndAction);
				m_strWndAction.Empty();
				BOOL bAppend = m_bWndAction;
				CIcadMenu* pIcadMenu = NULL;
				if(NULL!=(pIcadMenu=m_pMenuMain->GetCurrentMenu()))
				{
					if(pIcadMenu->LoadMenu(strMnuFileName,bAppend))
						pIcadMenu->SetMenu();
				}
				if(strMnuFileName.Find(".icm"/*DNT*/)==(-1))
				{
					((CIcadToolBarMain *)m_pToolBarMain)->CreateFromMnu(strMnuFileName,FALSE,bAppend);
				}
			}
			break;
 		case ICAD_WNDACTION_UNLOADMENU:
			{
				CString strMnuFileName(m_strWndAction);
				m_strWndAction.Empty();
				m_bWndAction=TRUE;
				if(NULL==m_pMenuMain)
				{
					break;
				}
				CIcadMenu* pIcadMenu = m_pMenuMain->GetCurrentMenu();
				if(NULL!=pIcadMenu)
				{
					if(!pIcadMenu->UnloadMnuFromMenuArray(strMnuFileName))
					{
						//*** If the menu has not been loaded "return" false.
						m_bWndAction=FALSE;
						break;
					}
					pIcadMenu->UpdateVisibility();
					pIcadMenu->SetMenu();
				}
				//*** Unload the toolbar
				if(NULL==m_pToolBarMain)
				{
					break;
				}
				BOOL bModified=FALSE;
				CPtrList* pToolBarList = ((CIcadToolBarMain*)m_pToolBarMain)->GetToolBarList();
				POSITION pos = pToolBarList->GetHeadPosition();
				POSITION posRemove = NULL;
				CString strGroupName;
				while(pos!=NULL)
				{
					CIcadToolBar* pToolBar = (CIcadToolBar*)pToolBarList->GetAt(pos);
					if(NULL==pToolBar)
					{
						pToolBarList->GetNext(pos);
						continue;
					}
					if(pToolBar->m_strGroupName.IsEmpty())
					{
						pToolBarList->GetNext(pos);
						continue;
					}
					strGroupName=pToolBar->m_strGroupName;
					int idxChar = strGroupName.ReverseFind('.'/*DNT*/);
					if(idxChar>(-1))
					{
						strGroupName=strGroupName.Left(idxChar);
					}
					if(strGroupName.CompareNoCase(strMnuFileName)==0)
					{
						posRemove=pos;
						pToolBarList->GetNext(pos);
						pToolBarList->RemoveAt(posRemove);
						delete pToolBar;
						bModified=TRUE;
					}
					else
					{
						pToolBarList->GetNext(pos);
					}
				}
				if(bModified)
				{
					((CIcadToolBarMain*)m_pToolBarMain)->SetModifiedFlag();
					RecalcLayout();
				}
			}
			break;
 		case ICAD_WNDACTION_NEWMENUS:
			{
				//*** Update the menu.
				CIcadMenu* pIcadMenu;
				if(NULL!=m_pMenuMain &&
					NULL!=(pIcadMenu=m_pMenuMain->GetCurrentMenu()))
				{
					pIcadMenu->UpdateVisibility();
					pIcadMenu->SetMenu();
				}

				((CIcadToolBarMain*)m_pToolBarMain)->OnVisibilityChanged();
			}
			break;
		case ICAD_WNDACTION_DBMODON:
			{

				CIcadDoc* pDoc = SDS_CURDOC;
				if(pDoc==NULL)
					{
					break;
					}
				pDoc->SetModifiedFlag(TRUE);
			}
			break;
		case ICAD_WNDACTION_DBMODOFF:
			{
				CIcadDoc* pDoc = SDS_CURDOC;

				if(pDoc==NULL)
					{
					break;
					}
				pDoc->SetModifiedFlag(FALSE);
			}
			break;
		case ICAD_WNDACTION_DEFXLINK:
			// TODO Define an x-link here.
			DisplayWizard();
			break;
		case ICAD_WNDACTION_DDNEW:
			{
				NewDwgWizard();
			}
			break;
		case ICAD_WNDACTION_INSOBJ:
			{
				CString strFileName(m_strWndAction);
				m_strWndAction.Empty();
				//*** Insert OLE object into the current document.
				CIcadView* pView = GetIcadView();
				if(pView==NULL) break;
				if(strFileName.IsEmpty())
				{
					pView->OnInsertObject();
				}
				else
				{
					pView->OnInsertObject(strFileName);
				}
			}
			break;
		case ICAD_WNDACTION_DDSELECT:
			// TODO I hate to use a global here.
			IcadSelDia(SDS_EditPropsSS);
			break;
		case ICAD_WNDACTION_OLELINKS:
			{
				CIcadChildWnd* pChild = (CIcadChildWnd*)MDIGetActive(NULL);
				if(NULL==pChild)
				{
					break;
				}
				CIcadDoc* pDoc = (CIcadDoc*)(pChild->GetActiveDocument());
				CIcadView* pView=GetIcadView();
				if(NULL==pDoc || NULL==pView)
				{
					break;
				}
				CIcadOleLinksDialog *pLinksDlg = NULL;
				pLinksDlg = new CIcadOleLinksDialog(pDoc, pView, 0, NULL);
				pLinksDlg->DoModal();
				delete pLinksDlg;
			}
			break;
		case ICAD_WNDACTION_OLEUNDOREDO:
			{
				//*** Deselect a selected OLE item.
				CIcadView* pView = GetIcadView();
				if(pView==NULL) break;
				pView->SetSelection(NULL);

				CIcadChildWnd* pChild = (CIcadChildWnd*)MDIGetActive(NULL);
				if(NULL==pChild) break;
				CIcadDoc* pDoc = (CIcadDoc*)(pChild->GetActiveDocument());
				if(NULL==pDoc || NULL==pDoc->m_pHandItemForRedraw) break;
				CIcadCntrItem* pItem = pDoc->GetOleItem(pDoc->m_pHandItemForRedraw);
				if(NULL==pItem) break;
				pItem->ResetStorage(pDoc->m_pHandItemForRedraw);
			}
			break;
		case ICAD_WNDACTION_UDOLEOBJ:
			{
				//*** The calling function must have setup the following variables
				//*** in the document class:
				//*** m_pViewForRedraw, m_pHandItemForRedraw, and
				//*** m_pDCForRedraw
				CIcadView* pView = SDS_CURVIEW;
				if(NULL!=pView && NULL!=pView->m_pViewsDoc)
				{
					pView->m_pViewsDoc->DrawOleItem();
					//*** Reset the variables to NULL
					pView->m_pViewsDoc->m_pViewForRedraw=NULL;
					pView->m_pViewsDoc->m_pHandItemForRedraw=NULL;
					pView->m_pViewsDoc->m_pDCForRedraw=NULL;
				}
			}
			break;

/* Moved to ON_MESSAGE handler
		case ICAD_WNDACTION_COLORDIA:
			{
				extern int SDS_ColDefCol,SDS_ColRetCol,SDS_ColMode,SDS_ColReturn;

				int ret=IcadColorDia(SDS_ColDefCol,&SDS_ColRetCol,SDS_ColMode);
				if (ret==IDOK)
					SDS_ColReturn=RTNORM;
				else if (ret==IDCANCEL)
					SDS_ColReturn=RTCAN;
				else
					SDS_ColReturn=RTERROR;

			}
			break;
*/
		case ICAD_WNDACTION_ABOUTDIA:
			IcadAbout();
			break;
		case ICAD_WNDACTION_CUSTOMIZE:
			IcadCustomDia();
			break;
		case ICAD_WNDACTION_RUNFORM:
			{
				extern char *SDS_FormToRun;
				IcadRunform(SDS_FormToRun);
			}
			break;
		case ICAD_WNDACTION_CURSOROFF:
			{
				CIcadView *pView=SDS_CURVIEW;
				struct gr_view *view=SDS_CURGRVW;

				if(view==NULL || pView==NULL) break;

				pView->CursorOn( false);
			}
			break;
		case ICAD_WNDACTION_CURSORON:
			{
				CIcadView *pView=SDS_CURVIEW;
				struct gr_view *view=SDS_CURGRVW;

				if(view==NULL || pView==NULL) break;

				pView->CursorOn( true);
			}
			break;
		case ICAD_WNDACTION_LASTDRAG:
			{
				struct resbuf rb;
				SDS_getvar(NULL,DB_QSNAPANG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				sds_real snapang=rb.resval.rreal;
				SDS_getvar(NULL,DB_QORTHOMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				int orthomode=rb.resval.rint;
				SDS_getvar(NULL,DB_QSNAPSTYL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				int snapstyl=rb.resval.rint;
				if(snapstyl==0)
					snapstyl=-1;
				else{
					SDS_getvar(NULL,DB_QSNAPISOPAIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					snapstyl=rb.resval.rint;
				}
				SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				SDS_dragone(SDS_CMainWindow->m_pDragVarsCur->pt3,1,rb.resval.rint,orthomode,snapang,snapstyl);
			}
			break;
		case ICAD_WNDACTION_CHPROP:
			IcadEntProps();
			break;
		case ICAD_WNDACTION_ATTEXT:
			IcadAttExtDia();
			break;
		case ICAD_WNDACTION_RENAMEAPP:
			IcadRenameApp();
			break;
		case ICAD_WNDACTION_UCSP:
			IcadUcspDia();
			break;
		case ICAD_WNDACTION_VPOINT:
			IcadVpointDia();
			break;
 		case ICAD_WNDACTION_UDTBSTATE:
			// TODO Call NEW function here.
			if(NULL!=m_pToolBarMain)
			{
				((CIcadToolBarMain*)m_pToolBarMain)->OnButtonStateChanged();
			}
			break;
 		case ICAD_WNDACTION_UDTBTIPS:
			{
				sds_resbuf rb;
				SDS_getvar(NULL,DB_QTOOLTIPS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				CIcadToolBarMain* pTbMain = (CIcadToolBarMain*)m_pToolBarMain;
				if(rb.resval.rint==0) pTbMain->m_bToolTips=FALSE;
				else pTbMain->m_bToolTips=TRUE;
				pTbMain->OnToolTipsChanged();
			}
			break;
		case ICAD_WNDACTION_TBLUCS:
			IcadExplorer(IEXP_CSTAB_IDX);
			break;
		case ICAD_WNDACTION_TBLVIEW:
			IcadExplorer(IEXP_VWTAB_IDX);
			break;
		case ICAD_WNDACTION_TBLSTY:
			IcadExplorer(IEXP_TSTAB_IDX);
			break;
		case ICAD_WNDACTION_TBLLT:
			IcadExplorer(IEXP_LTTAB_IDX);
			break;
		case ICAD_WNDACTION_TBLBLK:
			IcadExplorer(IEXP_BNTAB_IDX);
			break;
		case ICAD_WNDACTION_TBLDIMSTY:
			IcadExplorer(IEXP_DSTAB_IDX);
			break;
		case ICAD_WNDACTION_COPYCLIP:
			{
				CIcadView* pView=GetIcadView();
				if(pView==NULL) break;
				pView->CopyToClipboard();
			}
			break;
		case ICAD_WNDACTION_EXPASTE:
			// AG. These comments seem to be outdated, so I replaced PostUIAction() with ExecuteUIAction()
			// We have to do a special set undo here because this
			// WindAction is done with a PostThreadMessage().  OUCH!
//			SDS_SetUndo(IC_UNDO_COMMAND_BEGIN,(void *)"PASTECLIP"/*DNT*/,NULL,NULL,SDS_CURDWG);
			db_setfontsubstfn(NULL);
			PasteFromClipboard();
			db_setfontsubstfn(SDS_FontSubsMsg);
//			SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)"PASTECLIP"/*DNT*/,NULL,NULL,SDS_CURDWG);
			break;
		case ICAD_WNDACTION_ISVALIDTBNAME:
			{
				CString strName(m_strWndAction);
				m_strWndAction.Empty();
				CString strTbName;
				CPtrList* pToolBarList = ((CIcadToolBarMain*)m_pToolBarMain)->GetToolBarList();
				POSITION pos = pToolBarList->GetHeadPosition();
				m_bWndAction=FALSE;
				while(pos!=NULL)
				{
					CIcadToolBar* pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
					if(NULL==pToolBar)
					{
						continue;
					}
					if(pToolBar->m_bEmpty)
					{
						continue;
					}
					strName.MakeUpper();
					if(!pToolBar->m_strGroupName.IsEmpty())
					{
						//*** First try to match the group name.
						strTbName=pToolBar->m_strGroupName;
						strTbName.MakeUpper();
						if(RTNORM==sds_wcmatch(strTbName.GetBuffer(0),strName.GetBuffer(0)))
						{
							m_bWndAction=TRUE;
							break;
						}
					}
					pToolBar->GetWindowText(strTbName);
					strTbName.MakeUpper();
					if(RTNORM!=sds_wcmatch(strTbName.GetBuffer(0),strName.GetBuffer(0)))
					{
						continue;
					}
					m_bWndAction=TRUE;
					break;
				}
			}
			break;
		case ICAD_WNDACTION_HIDETOOLBARS:
			bShowToolbars=FALSE;
		case ICAD_WNDACTION_SHOWTOOLBARS:
			{
				CString strName(m_strWndAction);
				m_strWndAction.Empty();
				if(strName.CompareNoCase(ResourceString(IDC_ICADWNDACTION_ALL_7, "ALL" ))==0 ||
				   strName.CompareNoCase("ALL"/*DNT*/)==0)
				{
					CPtrList* pToolBarList = ((CIcadToolBarMain*)m_pToolBarMain)->GetToolBarList();
					POSITION pos = pToolBarList->GetHeadPosition();
					while(pos!=NULL)
					{
						CIcadToolBar* pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
						if(NULL==pToolBar || pToolBar->m_bEmpty)
						{
							continue;
						}
						ShowControlBar(pToolBar,bShowToolbars,FALSE);
					}
					break;
				}
				CString strTbName;
				CPtrList* pToolBarList = ((CIcadToolBarMain*)m_pToolBarMain)->GetToolBarList();
				POSITION pos = pToolBarList->GetHeadPosition();
				while(pos!=NULL)
				{
					CIcadToolBar* pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
					if(NULL==pToolBar)
					{
						continue;
					}
					if(pToolBar->m_bEmpty)
					{
						continue;
					}
					strName.MakeUpper();
					if(!pToolBar->m_strGroupName.IsEmpty())
					{
						//*** First try to match the group name.
						strTbName=pToolBar->m_strGroupName;
						strTbName.MakeUpper();
						if(RTNORM==sds_wcmatch(strTbName.GetBuffer(0),strName.GetBuffer(0)))
						{
							ShowControlBar(pToolBar,bShowToolbars,FALSE);
							continue;
						}
					}
					pToolBar->GetWindowText(strTbName);
					strTbName.MakeUpper();
					if(RTNORM!=sds_wcmatch(strTbName.GetBuffer(0),strName.GetBuffer(0)))
					{
						continue;
					}
					ShowControlBar(pToolBar,bShowToolbars,FALSE);
				}
			}
			break;
		case ICAD_WNDACTION_GETTBPOS:
			{
				if(NULL==m_pvWndAction)
				{
					m_bWndAction=FALSE;
					break;
				}
				CString strName(m_strWndAction);
				m_strWndAction.Empty();
				CString strTbName;
				CPtrList* pToolBarList = ((CIcadToolBarMain*)m_pToolBarMain)->GetToolBarList();
				POSITION pos = pToolBarList->GetHeadPosition();
				CIcadToolBar* pToolBar = NULL;
				while(pos!=NULL)
				{
					pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
					if(NULL==pToolBar)
					{
						continue;
					}
					if(pToolBar->m_bEmpty)
					{
						pToolBar=NULL;
						continue;
					}
					strName.MakeUpper();
					if(!pToolBar->m_strGroupName.IsEmpty())
					{
						//*** First try to match the group name.
						strTbName=pToolBar->m_strGroupName;
						strTbName.MakeUpper();
						if(RTNORM==sds_wcmatch(strTbName.GetBuffer(0),strName.GetBuffer(0)))
						{
							break;
						}
					}
					pToolBar->GetWindowText(strTbName);
					strTbName.MakeUpper();
					if(RTNORM==sds_wcmatch(strTbName.GetBuffer(0),strName.GetBuffer(0)))
					{
						break;
					}
					pToolBar=NULL;
				}
				if(NULL==pToolBar)
				{
					m_bWndAction=FALSE;
					break;
				}
				//*** Init the point to 0,0,0
				if(!pToolBar->IsFloating())
				{
					((sds_real*)m_pvWndAction)[0]=0.0;
					((sds_real*)m_pvWndAction)[1]=0.0;
					((sds_real*)m_pvWndAction)[2]=1.0;
					m_bWndAction=TRUE;
					break;
				}
				CRect rect;
				CFrameWnd* pParentFrame = pToolBar->GetParentFrame();
				if(NULL==pParentFrame)
				{
					m_bWndAction=FALSE;
					break;
				}
				pParentFrame->GetWindowRect(rect);
				((sds_real*)m_pvWndAction)[0]=(double)rect.left;
				((sds_real*)m_pvWndAction)[1]=(double)rect.top;
				((sds_real*)m_pvWndAction)[2]=0.0;
				if(!pToolBar->IsWindowVisible())
				{
					((sds_real*)m_pvWndAction)[2]=2.0;
				}
				m_bWndAction=TRUE;
			}
			break;
		case ICAD_WNDACTION_MOVETOOLBAR:
			{
				CPoint ptToolBar;
				if(NULL!=m_pvWndAction)
				{
					ptToolBar.x=(int)(((sds_real*)m_pvWndAction)[0]);
					ptToolBar.y=(int)(((sds_real*)m_pvWndAction)[1]);
				}
				CString strName(m_strWndAction);
				m_strWndAction.Empty();
				CString strTbName;
				CPtrList* pToolBarList = ((CIcadToolBarMain*)m_pToolBarMain)->GetToolBarList();
				POSITION pos = pToolBarList->GetHeadPosition();
				CIcadToolBar* pToolBar = NULL;
				BOOL bFound = FALSE;
				while(pos!=NULL)
				{
					pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
					if(NULL==pToolBar)
					{
						continue;
					}
					if(pToolBar->m_bEmpty)
					{
						pToolBar=NULL;
						continue;
					}
					strName.MakeUpper();
					if(!pToolBar->m_strGroupName.IsEmpty())
					{
						//*** First try to match the group name.
						strTbName=pToolBar->m_strGroupName;
						strTbName.MakeUpper();
						if(RTNORM==sds_wcmatch(strTbName.GetBuffer(0),strName.GetBuffer(0)))
						{
							bFound=TRUE;
						}
					}
					if(!bFound)
					{
						pToolBar->GetWindowText(strTbName);
						strTbName.MakeUpper();
						if(RTNORM!=sds_wcmatch(strTbName.GetBuffer(0),strName.GetBuffer(0)))
						{
							continue;
						}
					}
					bFound=FALSE;
					//*** Float the toolbar.
					if(NULL==m_pvWndAction)
					{
						CRect rect;
						if(!pToolBar->IsFloating())
						{
							pToolBar->GetWindowRect(rect);
						}
						else
						{
							CFrameWnd* pParentFrame = pToolBar->GetParentFrame();
							if(NULL==pParentFrame)
							{
								continue;
							}
							pParentFrame->GetWindowRect(rect);
						}
						ptToolBar=rect.TopLeft();
					}
					FloatControlBar(pToolBar,ptToolBar);
				}
				m_bWndAction=TRUE;
			}
			break;

		case ICAD_WNDACTION_VIEWCTL:
			IcadViewCtl();
			break;
		case ICAD_WNDACTION_VBA:	//Show the Visual Basic Environment.
			if (g_Vba6Installed)
				{
				if (GetApp()->m_bReadOnlyWorkspace)
					{
					GetApp()->m_pMainWnd->MessageBox(
							ResourceString(IDC_VBA_WARN_READONLY, "Warning! Another instance of IntelliCAD is running. The VBA Common Project is being opened Read Only.\n Any changes made to this project will not be saved." ),
							ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),
							MB_OK | MB_ICONEXCLAMATION);
					SDS_CMainWindow->m_pCurDoc->m_bVbaProjectSaved = FALSE;
					}
				else
					{
					SDS_CMainWindow->m_pCurDoc->m_bVbaProjectSaved = TRUE;
					}
				GetApp()->OnVbaIde();
				}
			else
				{
				GetApp()->m_pMainWnd->MessageBox(
						ResourceString(IDC_VBA_WARN_NOTINSTALLED, "To run Visual Basic, you will have to install VBA6.  See the readme for more details."),
						ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),
						MB_OK | MB_ICONEXCLAMATION);
				}
			break;
		case ICAD_WNDACTION_VBALOAD:	//Show the COM Addin Manager
		case ICAD_WNDACTION_VBAUNLOAD:
			if (g_Vba6Installed)
				{
				GetApp()->ApcHostAddIns->ShowAddInsDialog();
				}
			else
				{
				GetApp ()->comAddinManager ().showDialog ();
				}
			break;
		case ICAD_WNDACTION_VBAFIRE_REINIT:
			GetApp()->FireWorkspaceEvent (DISPID_WS_REINIT, NULL);
			break;
		case ICAD_WNDACTION_VBARUN:		//Show the VBA MacroDialog
			if (g_Vba6Installed)
				{
				CIcadChildWnd* pChild = (CIcadChildWnd*)MDIGetActive(NULL);
				if(pChild==NULL)
					break;

				ASSERT_VALID( pChild );

				CIcadDoc* pDoc = (CIcadDoc*)(pChild->GetActiveDocument());

				ASSERT_VALID( pDoc );

				ASSERT(pDoc->ApcRecorder == NULL);
				if (pDoc->ApcRecorder)
					break;

				if(NULL==m_pvWndAction)	//show the dialog
					{
					CString str1;
					str1.LoadString (IDC_VBA_RUNMACRO);
					pDoc->ApcRecorder.Run(this, str1);
					}
				else	//run the individual macro
					{
					CString str1((char *) m_pvWndAction);
					/*DG - 11.4.2003*/// Clear and nullify m_pvWndAction before executing the macro,
					// because it can be used BEFORE the macro finishes
					// (e.g., if the macro calls MessageBox and moreover we are running a script with macro calls).
					// See bug N 78490.
					delete [] ((char *) m_pvWndAction);
					m_pvWndAction = NULL;
					pDoc->ApcRecorder.RunMacro(str1.AllocSysString());
					}
				}
			else
				{
				GetApp()->m_pMainWnd->MessageBox(
						ResourceString(IDC_VBA_WARN_NOTINSTALLED, "To run Visual Basic, you will have to install VBA6.  See the readme for more details."),
						ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),
						MB_OK | MB_ICONEXCLAMATION);
				}
			break;
		case ICAD_WNDACTION_ADDINRUN:		//run the addin function
			if (g_Vba6Installed)
			{
				if(NULL==m_pvWndAction)
					break;

				CString strName((char *)m_pvWndAction);
				VARIANT addin;
				addin.vt = VT_BSTR;
				addin.bstrVal = strName.AllocSysString();

				// package parameter
				VARIANT varArg[1];
				varArg[0] = addin;

				DISPPARAMS parm;
				parm.rgvarg = varArg;
				parm.cArgs = 1;
				parm.rgdispidNamedArgs = NULL;
				parm.cNamedArgs = 0;

				HRESULT hr = ApcFireEvent(GetApp()->m_spWorkspace, DIID__IcadWorkspaceEvents, DISPID_WS_ADDINRUN, &parm, NULL);
				ASSERT(SUCCEEDED(hr));

				}
			else
            {
				HRESULT hr = GetApp()->comAddinManager ().fireAddinRun(
					GetApp()->m_spWorkspace,
					(const char*) m_pvWndAction);
				ASSERT(SUCCEEDED(hr));
            }
			delete [] ((char *) m_pvWndAction);
			break;
		case ICAD_WNDACTION_VBAEXECUTE:
			{
			class commandAtomObj *atom;
			atom = (commandAtomObj *) m_pvWndAction;
			if (atom && atom->HasFuncPtr())
				atom->CallFunction();
			}
			break;
		case ICAD_WNDACTION_SECURITY:
			if (g_Vba6Installed)
				{
				CIcadApp* pApp = (CIcadApp*)AfxGetApp();
				if(NULL==pApp)
					break;

				AxSecurityLevel axLevel;

				// show the security dialog
				ApcCallV(pApp->ApcHost->APC_RAW(ShowSecurityLevelDialog)(&axLevel));

				pApp->Encrypt(axLevel); // see comments below

				// write the value to the registry
				CRegKey reg;
				long nSuccess = reg.Open(HKEY_CURRENT_USER, pApp->m_regpathBase);
				if (ERROR_SUCCESS != reg.SetValue(static_cast<unsigned long>(axLevel), pApp->m_regpathSecurity))
					{
					GetApp()->m_pMainWnd->MessageBox(
							ResourceString(IDC_VBA_SECURITY_NOTSAVED, "IntelliCAD is unable to store the requested security value in registry\n"),
							ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),
							MB_OK | MB_ICONEXCLAMATION);
					}
				}
			else
				{
				GetApp()->m_pMainWnd->MessageBox(
						ResourceString(IDC_VBA_WARN_NOTINSTALLED, "To run Visual Basic, you will have to install VBA6.  See the readme for more details."),
						ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),
						MB_OK | MB_ICONEXCLAMATION);
				}
			break;
		case ICAD_WNDACTION_APPLOAD:
			IcadAppLoader();
			break;
		case ICAD_WNDACTION_SPELL:
			{
				// We have to do a special set undo here because this
				// WndAction is done witt a PostThreadMessage().  OUCH!
				//*** I moved this to IcadSpell because it was causing my portable
				//*** to hang.
//				SDS_SetUndo(IC_UNDO_COMMAND_BEGIN,(void *)"SPELL",NULL,NULL,SDS_CURDWG);
				SpellCheck(SDS_EditPropsSS);
				//*** Unhighlight selection set of entities.
				sds_name ename;
				long ssct;
				for(ssct=0L; sds_ssname(SDS_EditPropsSS,ssct,ename)==RTNORM; ssct++) sds_redraw(ename,IC_REDRAW_UNHILITE);
				sds_ssfree(SDS_EditPropsSS);
//				SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)"SPELL",NULL,NULL,SDS_CURDWG);
			}
			break;
		case ICAD_WNDACTION_MENUCMD:
			{
				extern char *SDS_MenuCmdStr;
				CIcadMenu* pIcadMenu = NULL;
				if(NULL!=m_pMenuMain)
				{
					pIcadMenu=m_pMenuMain->GetCurrentMenu();
				}
				if(NULL!=pIcadMenu)
				{
					char *tempVar;
					tempVar = new char [strlen(SDS_MenuCmdStr) + 1];
					sprintf(tempVar, SDS_MenuCmdStr);
					pIcadMenu->DoMenuRef(tempVar);
					IC_FREE(tempVar);
				}
			}
			break;
		case ICAD_WNDACTION_SETFOCUS:
			{
			if(m_wndViewFocus==NULL)
				break;
			m_wndViewFocus->SetFocus();
			CFrameWnd* pParentFrame = m_wndViewFocus->GetParentFrame();
			if ( pParentFrame != NULL )
				{
				pParentFrame->SetFocus();
				}
			m_wndViewFocus=NULL;
			}
			break;
		case ICAD_WNDACTION_MDIMIN:
			{
				WINDOWPLACEMENT wp;
				CIcadView *pView=SDS_CURVIEW;
				if ( pView != NULL )
					{
					ASSERT_VALID( pView );

					pView->GetParentFrame()->GetWindowPlacement(&wp);
					wp.showCmd=SW_SHOWMINIMIZED;
					pView->GetParentFrame()->SetWindowPlacement(&wp);
					}
			}
			break;
		case ICAD_WNDACTION_MDIMAX:
			{
				WINDOWPLACEMENT wp;
				CIcadView *pView=SDS_CURVIEW;
				if ( pView != NULL )
					{
					ASSERT_VALID( pView );

					pView->GetParentFrame()->GetWindowPlacement(&wp);
					wp.showCmd=SW_SHOWMAXIMIZED;
					pView->GetParentFrame()->SetWindowPlacement(&wp);
					}
			}
			break;
		case ICAD_WNDACTION_MDIRESTORE:
			{
				WINDOWPLACEMENT wp;
				CIcadView *pView=SDS_CURVIEW;
				if ( pView != NULL )
					{

					ASSERT_VALID( pView );

					pView->GetParentFrame()->GetWindowPlacement(&wp);
					wp.showCmd=SW_RESTORE;
					pView->GetParentFrame()->SetWindowPlacement(&wp);
					}
			}
			break;
		case ICAD_WNDACTION_CLOSEVIEW:
			{
			if( m_wndViewClose==NULL ||
				m_wndViewClose->m_hWnd==NULL)
				{
				break;
				}

			ASSERT_VALID( m_wndViewClose );
			CIcadChildWnd *pChild = (CIcadChildWnd *)m_wndViewClose->GetParentFrame();
			ASSERT_VALID( pChild );

			if ( pChild != NULL )
				{
				pChild->OnChildWndClose();
				}
			m_wndViewClose=NULL;
			}
			break;
		case ICAD_WNDACTION_EDITXDATA:
			{
				extern sds_name SDS_AttEditName;
				IcadXdataEdit(SDS_AttEditName);
			}
			break;
 		case ICAD_WNDACTION_WCLOSEALLBUT: //*** Window - Close All But active.
			{
				CIcadChildWnd* pChild = (CIcadChildWnd*)MDIGetActive(NULL);
				if(pChild==NULL)
					break;

				cmd_InsideOfCloseCommand = TRUE;
				ASSERT_VALID( pChild );

				CIcadDoc* pDoc = (CIcadDoc*)(pChild->GetActiveDocument());

				ASSERT_VALID( pDoc );

				POSITION pos=pDoc->GetFirstViewPosition();
				CIcadChildWnd* pCurFrame = m_pCurFrame;
				ASSERT_VALID( pCurFrame );
				while (pos!=NULL)
					{

					pChild=(CIcadChildWnd*)((CIcadView *)pDoc->GetNextView(pos))->GetParentFrame();
					ASSERT_VALID( pChild );
					if((pChild != NULL) &&
						(pChild!=pCurFrame))
						{
						pChild->OnChildWndClose();
						}
					}
				ASSERT_VALID( pCurFrame );


				CIcadView *pCurView = (CIcadView *)pCurFrame->GetActiveView();
				if ( pCurView != NULL )
					{
					ASSERT_VALID( pCurView );

					pCurView->m_pViewsDoc = m_pCurDoc = pCurView->GetDocument();
					m_pCurFrame = (CIcadChildWnd*)pCurView->GetParentFrame();
					m_pCurView = pCurView;
					pCurView->SetFocus();
					pCurView->SetNewView( FALSE );
					pCurFrame->SetFocus();
					}

				ASSERT_VALID( pCurFrame );
				cmd_InsideOfCloseCommand = FALSE;
			}
			break;
		case ICAD_WNDACTION_MLEDIT:
			IcadMLEditDia();
			break;
		case ICAD_WNDACTION_MTEXT:
			IcadMTextDia(&SDS_mTextDataPtr);
			break;
		// EBATECH(watanabe)-[get mtext info only
		case ICAD_WNDACTION_MTEXT2:
			IcadMTextDia(&SDS_mTextDataPtr, false);
			break;
		//]-EBATECH(watanabe)
		case ICAD_WNDACTION_TOLERANCE:
			IcadToleranceDia();
			break;
		// EBATECH(watanabe)-[add
		case ICAD_WNDACTION_TOLERANCE_EX:
			IcadToleranceDia(ptTolerance);
			break;
		// ]-EBATECH(watanabe)
		case ICAD_WNDACTION_STATCHANGE:
			{
				//*** Update the varibles on the status bar.
				extern char *SDS_UpdateSetvar;
				m_StatusBar.UpdateVariablePanes(SDS_UpdateSetvar);
#ifdef _LAYOUTTABS_
				if(m_pCurFrame == MDIGetActive())
					m_pCurFrame->UpdateLayoutTabs(m_pCurDoc->m_dbDrawing);
#endif
			}
			break;
		case ICAD_WNDACTION_UDSCRLHIST:
			{
				struct resbuf rb;
				SDS_getvar(NULL,DB_QSCRLHIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				int nMaxItems=rb.resval.rint;
				int nItems=m_pHistList->GetCount();
				//*** Delete the strings in the list if it is too large.
				while(nItems>nMaxItems)
				{
					m_pHistList->RemoveHead();
					nItems--;
				}
				//*** Force the text windows to update.
				m_pCmdBar->m_pPromptWnd->m_pPromptWndList->OnUpdateScrlHist(nMaxItems);
				m_pTextScrn->m_pPromptWnd->m_pPromptWndList->OnUpdateScrlHist(nMaxItems);
			}
			break;
		case ICAD_WNDACTION_STARTPROG:
			m_StatusBar.InitProgressBar();
			break;
		case ICAD_WNDACTION_UPDATEPROG:
			{
			   extern int SDS_ProgressPos;
			   m_StatusBar.UpdateProgressBar(SDS_ProgressPos);
			   break;
			}
		case ICAD_WNDACTION_ENDPROG:
			m_StatusBar.CloseProgressBar();
			break;
		case ICAD_WNDACTION_GROUP:
			IcadGroupDia();
			break;
		case ICAD_WNDACTION_HYPERLINK:
			{
				CDialogHyperlink dh(this, NULL);
				int result = RTNORM;
				long l;
				sds_name ss;
				sds_sslength(SDS_CURDOC->m_pGripSelection, &l);
				if(l > 0)
				{
					dh.setEntities(SDS_CURDOC->m_pGripSelection);
					cmd_redrawSS(SDS_CURDOC->m_pGripSelection, IC_REDRAW_HILITE);
				}
				else
				{
					result = sds_pmtssget("", NULL, NULL, NULL, NULL, ss, 1);
					dh.setEntities(ss);
					cmd_redrawSS(ss, IC_REDRAW_HILITE);
				}
				if(result != RTNORM)
					break;

				dh.DoModal();

				if(l > 0)
					cmd_redrawSS(SDS_CURDOC->m_pGripSelection, IC_REDRAW_UNHILITE);
				else
				{
					cmd_redrawSS(ss, IC_REDRAW_UNHILITE);
					sds_ssfree(ss);
				}
				break;
			}
		case ICAD_WNDACTION_PMSPACE:
			IcadPMSpaceDia();
			break;
		case ICAD_WNDACTION_SHOWEXP:
			if (m_pExplorer) m_pExplorer->ShowExpolorer();
			break;
		case ICAD_WNDACTION_DELEXP:
			if (m_pExplorer) {m_pExplorer->DestroyWindow();}
			break;

		case ICAD_WNDACTION_CALCVIEWPTS:
			if(SDS_CURVIEW==NULL) break;
			SDS_CURVIEW->CalcViewPoints(1);
			break;
		case ICAD_WNDACTION_DELOLEITEM:
			{
				//*** The db_handitem that gets passed to DeleteOleItem() needs
				//*** to be set to this void pointer.
				if(NULL==m_pvWndAction) break;
				CIcadChildWnd* pChild;
				if((pChild=(CIcadChildWnd*)MDIGetActive(NULL))==NULL) break;
				CIcadDoc* pDoc;
				if((pDoc=(CIcadDoc*)(pChild->GetActiveDocument()))==NULL) break;
				m_bWndAction=pDoc->DeleteOleItem((db_handitem*)m_pvWndAction);
			}
			break;
		case ICAD_WNDACTION_UNSELOLEITEMS:
			{
				//*** Deselect a selected OLE item.
				CIcadView* pView = GetIcadView();
				if(pView==NULL) break;
				pView->SetSelection(NULL);
			}
			break;
		case ICAD_WNDACTION_PASTESPEC:
			{
				// We have to do a special set undo here because this
				// WindAction is done witt a PostThreadMessage().  OUCH!
//				SDS_SetUndo(IC_UNDO_COMMAND_BEGIN,(void *)"PASTESPEC",NULL,NULL,SDS_CURDWG);
				COlePasteSpecialDialog pasteDlg;
				//*** Add the available formats to the dialog.
				//AddStandardFormats also adds the IntelliCAD Drawing format which we do not currently support -
				//i.e. ICAD into ICAD does not work.  So we do not AddStandardFormats, instead do it ourselves.
				//pasteDlg.AddStandardFormats();
				pasteDlg.AddFormat(CF_METAFILEPICT, TYMED_MFPICT, AFX_IDS_METAFILE_FORMAT,FALSE, FALSE);
				pasteDlg.AddFormat(CF_DIB, TYMED_HGLOBAL, AFX_IDS_DIB_FORMAT, FALSE, FALSE);
				pasteDlg.AddFormat(CF_BITMAP, TYMED_GDI, AFX_IDS_BITMAP_FORMAT, FALSE, FALSE);

				FORMATETC fmt;
				fmt.cfFormat=CF_TEXT;
				fmt.ptd=NULL;
				fmt.dwAspect=DVASPECT_CONTENT;
				fmt.lindex=-1;
				fmt.tymed=TYMED_HGLOBAL;

				EditString	format;
				EditString	result;
				format = ResourceString( IDC_ICADWNDACTION_SCRIPT_TE_10, "Script Text");
				result = ResourceString( IDC_ICADWNDACTION_COMMANDS__11, "commands in script format");
				pasteDlg.AddFormat(fmt,format,result,0);

				fmt.cfFormat=(((CIcadApp*)AfxGetApp())->m_uIcadClipFormat);
				format = ResourceString( IDC_ICADWNDACTION_INTELLICA_12, "IntelliCAD Entities");
				result = ResourceString( IDC_ICADWNDACTION_ENTITIES__13, "entities or table entries including layers, linetypes, etc");
				pasteDlg.AddFormat(fmt,format,result,0);

				fmt.cfFormat=::RegisterClipboardFormat(_T("AutoCAD.r14"/*DNT*/));
				format = ResourceString( IDC_ICADWNDACTION_INTELLICA_14, "IntelliCAD Block");
				result = ResourceString( IDC_ICADWNDACTION_A_BLOCK_15, "a block");
				pasteDlg.AddFormat(fmt,format,result,0);

				if(IDOK!=pasteDlg.DoModal())
				{
					break;
				}
				int idxPaste = pasteDlg.GetPasteIndex();
				if (!PasteClipboardType(pasteDlg.m_ps.arrPasteEntries[idxPaste].fmtetc.cfFormat,&pasteDlg))
					sds_alert("Clipboard data invalid or unavailable.  The Paste operation was not performed.");

//				SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)"PASTESPEC",NULL,NULL,SDS_CURDWG);
				// We need to print this line because the Substiting font message may have been printed
				// and since this command was done with a PostMessage() we may have to clean up the
				// command line.
				sds_printf("\n: "/*DNT*/);
			}
			break;
		case ICAD_WNDACTION_COPYHIST:
			{
				CString str;
				OpenClipboard();
				EmptyClipboard();

				POSITION pos = m_pHistList->GetHeadPosition();
				while(NULL!=pos)
				{
					str+=m_pHistList->GetNext(pos);
					str+="\r\n"/*DNT*/;
				}
				HANDLE hMem=GlobalAlloc(GHND | GMEM_DDESHARE,str.GetLength()+1);
				if(NULL!=hMem)
				{
 					LPSTR pszHist = (LPSTR)GlobalLock(hMem);
					if(NULL!=pszHist)
					{
						strcpy(pszHist,str);
						GlobalUnlock(hMem);
						SetClipboardData(CF_TEXT,hMem);
					}
				}
				CloseClipboard();
			}
			break;
 		case ICAD_WNDACTION_NEWFONTMAP:
			{
				CIcadChildWnd* pChild = (CIcadChildWnd*)MDIGetActive(NULL);
				if(pChild==NULL) break;
				CIcadDoc* pDoc = (CIcadDoc*)pChild->GetActiveDocument();
				if(pDoc==NULL) break;
				CDocTemplate* pDocTemplate = pDoc->GetDocTemplate();
				POSITION pos = pDocTemplate->GetFirstDocPosition();
				CString strDwg;
				while(pos!=NULL)
				{
					pDoc = (CIcadDoc*)pDocTemplate->GetNextDoc(pos);
					pDoc->m_dbDrawing->resetfonts();
				}
			}
			break;
 		case ICAD_WNDACTION_RUNFIRSTVIEW:
			SDS_OnFirstView(SDS_CURVIEW,1);
			break;
 		case ICAD_WNDACTION_NEWPALETTE:
			{
				CIcadChildWnd* pChild = (CIcadChildWnd*)MDIGetActive(NULL);
				if(pChild==NULL) break;
				CIcadDoc* pDoc = (CIcadDoc*)pChild->GetActiveDocument();
				if(pDoc==NULL) break;
				CDocTemplate* pDocTemplate = pDoc->GetDocTemplate();
				POSITION pos = pDocTemplate->GetFirstDocPosition();
				while(pos!=NULL)
				{
					pDoc = (CIcadDoc*)pDocTemplate->GetNextDoc(pos);
					CIcadView *pView;
					if(pDoc) {
						POSITION pos=pDoc->GetFirstViewPosition();
						while (pos!=NULL) {
							pView=(CIcadView *)pDoc->GetNextView(pos);
							ASSERT_KINDOF(CIcadView, pView);

							pView->m_LastCol=-100;
							pView->m_LastHl=-100;
							pView->m_FillLastCol=-100;
							pView->m_FillLastHl=-100;
							pView->GetFrameBufferDrawDevice()->selectPalette(NULL, TRUE);

						}
					}
				}
				delete m_pPalette;
				m_pPalette = NULL;
				SDS_CreateOurColorMap();
				SetPalette(SDS_GetNewPalette());
				pos = pDocTemplate->GetFirstDocPosition();
				while(pos!=NULL)
				{
					pDoc = (CIcadDoc*)pDocTemplate->GetNextDoc(pos);
					CIcadView *pView;
					if(pDoc) {
						POSITION pos=pDoc->GetFirstViewPosition();
						while (pos!=NULL) {
							pView=(CIcadView *)pDoc->GetNextView(pos);
							ASSERT_KINDOF(CIcadView, pView);

							pView->GetFrameBufferDrawDevice()->selectPalette(*m_pPalette, TRUE);
						}
					}
				}
			}
			break;
 		case ICAD_WNDACTION_DRAGON:
			if(SDS_IsDragging && SDS_CMainWindow->m_pDragVarsCur) {

				if(SDS_CMainWindow->m_pDragVarsCur->scnf && SDS_CMainWindow->m_pDragVarsCur->scnf==SDS_dragobj_cb) {
					int ret;
					if(RTERROR==(ret=(*SDS_CMainWindow->m_pDragVarsCur->scnf)(SDS_CMainWindow->m_pDragVarsCur->pt3,SDS_CMainWindow->m_pDragVarsCur->matx)))
					SDS_CMainWindow->m_pDragVarsCur->breakout=ret;
				}

				if(SDS_AtCopyMoveCommand) sds_grdraw(SDS_CMainWindow->m_pDragVarsCur->pt1,SDS_CMainWindow->m_pDragVarsCur->pt3,-1,0);
				SDS_DrawDragVectors(NULL);
			}
			break;
 		case ICAD_WNDACTION_DRAGOFF:
			if(SDS_CMainWindow->m_pDragVarsCur) {

				if(SDS_CMainWindow->m_pDragVarsCur->scnf && SDS_CMainWindow->m_pDragVarsCur->scnf==SDS_dragobj_cb) {
					int ret;
					if(RTERROR==(ret=(*SDS_CMainWindow->m_pDragVarsCur->scnf)(SDS_CMainWindow->m_pDragVarsCur->pt3,SDS_CMainWindow->m_pDragVarsCur->matx)))
					SDS_CMainWindow->m_pDragVarsCur->breakout=ret;
				}

				if(SDS_AtCopyMoveCommand) sds_grdraw(SDS_CMainWindow->m_pDragVarsCur->pt1,SDS_CMainWindow->m_pDragVarsCur->pt3,-1,0);
				SDS_DrawDragVectors(NULL);
			}
			break;
 		case ICAD_WNDACTION_LAYOUTTEMPL:
			{
				char fileName[MAX_PATH];
				strcpy(fileName, m_strWndAction);
				m_strWndAction.Empty();
				db_drawing* pDestDrawing = (db_drawing*)m_pvWndAction;
				m_pvWndAction = NULL;

				int result = 0;
				db_drawing* pTemplateDrawing = NULL;
				pTemplateDrawing = this->m_pFileIO.drw_openfile(NULL, fileName, &result, true);
				ASSERT(pTemplateDrawing != NULL);
				if(pTemplateDrawing != NULL)
				{
					CLayoutInsert layoutIns(this, pDestDrawing, pTemplateDrawing);
					layoutIns.DoModal();
					delete pTemplateDrawing;
				}
			}
			break;
 		case ICAD_WNDACTION_MENUGROUP:
			{
				CIcadMenu* pIcadMenu = NULL;
				if(NULL!=m_pMenuMain)
				{
					pIcadMenu=m_pMenuMain->GetCurrentMenu();
				}
				if(NULL==pIcadMenu)
				{
					break;
				}

				CString str=m_strWndAction;
				m_strWndAction.Empty();
				str.MakeUpper();

				m_bWndAction = pIcadMenu->IsMenuGroupLoaded( str );
			}
			break;
 		case ICAD_WNDACTION_GETDOCLIST:
			{
				extern struct resbuf *SDS_UserDocList;
				struct sds_resbuf *beg=NULL,*cur=NULL,*tmp;
				CIcadChildWnd* pChild = (CIcadChildWnd*)MDIGetActive(NULL);
				if(pChild==NULL) SDS_UserDocList=NULL;
				if (pChild!=NULL)
				{
					CIcadDoc* pDoc = (CIcadDoc*)pChild->GetActiveDocument();
					if(pDoc==NULL) SDS_UserDocList=NULL;
					if (pDoc!= NULL)
					{
						CDocTemplate* pDocTemplate = pDoc->GetDocTemplate();
						if (pDocTemplate != NULL)
						{
							POSITION pos = pDocTemplate->GetFirstDocPosition();
							CString strDwg;
							while(pos!=NULL)
							{
								pDoc = (CIcadDoc*)pDocTemplate->GetNextDoc(pos);
								tmp=sds_buildlist(RTSTR,pDoc->GetPathName(),0);
								if(beg==NULL) beg=cur=tmp;
								else cur=cur->rbnext=tmp;
							}
							SDS_UserDocList=beg;
						}
					}
				}
			}
			break;
 		case ICAD_WNDACTION_USERCALL:
			{
				UserMainThreadArgs *args = (UserMainThreadArgs *)lParam;
				args->SDS_UserMainThreadFuncRet=(*args->SDS_UserMainThreadFunc)(args->SDS_UserMainThreadFuncData);
			}
			break;
 		case ICAD_WNDACTION_UPDATE:
			{
				extern CIcadView *SDS_pViewToInvalidate;
				extern CRect *SDS_RectToInvalidate;
				if(SDS_pViewToInvalidate==NULL) break;
				SDS_pViewToInvalidate->InvalidateRect(SDS_RectToInvalidate);
			}
			break;
 		case ICAD_WNDACTION_RND_LIGHT:
			{

			if(lw_edit_lights)
				{

				(*lw_edit_lights)(NULL);

				}
			}
			break;
 		case ICAD_WNDACTION_RND_RENDERPRE:
			if(lw_render_preview)
				{
				struct resbuf rb;
				SDS_getvar(NULL, DB_QHIGHLIGHT, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
				int highlight = rb.resval.rint;

				rb.resval.rint = 0;
				SDS_setvar(NULL, DB_QHIGHLIGHT, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);

				// DP: render needs display objects with invisible edges
				int splframe = SDS_CURGRVW->splframe;
				SDS_CURGRVW->splframe = 1;

				(*lw_render_preview)(NULL);

				SDS_CURGRVW->splframe = splframe;

				rb.resval.rint = highlight;
				SDS_setvar(NULL, DB_QHIGHLIGHT, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
				}

			break;
 		case ICAD_WNDACTION_RND_RENDERWF:
			if(lw_render_wireframe)
				{
				struct resbuf rb;
				SDS_getvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				int highlight = rb.resval.rint;

				rb.resval.rint = 0;
				SDS_setvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

				// DP: render needs display objects with invisible edges
				int splframe = SDS_CURGRVW->splframe;
				SDS_CURGRVW->splframe = 1;

				(*lw_render_wireframe)(NULL);

				SDS_CURGRVW->splframe = splframe;

				rb.resval.rint = highlight;
				SDS_setvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				}
			break;
 		case ICAD_WNDACTION_RND_RENDERHD:
			if(lw_render_hidden)
				{
				struct resbuf rb;
				SDS_getvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				int highlight = rb.resval.rint;

				rb.resval.rint = 0;
				SDS_setvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

				// DP: render needs display objects with invisible edges
				int splframe = SDS_CURGRVW->splframe;
				SDS_CURGRVW->splframe = 1;

				(*lw_render_hidden)(NULL);

				SDS_CURGRVW->splframe = splframe;

				rb.resval.rint = highlight;
				SDS_setvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				}
			break;
 		case ICAD_WNDACTION_RND_RENDERFL:
			if(lw_render_full)
				{
				struct resbuf rb;
				SDS_getvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				int highlight = rb.resval.rint;

				rb.resval.rint = 0;
				SDS_setvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

				// DP: render needs display objects with invisible edges
				int splframe = SDS_CURGRVW->splframe;
				SDS_CURGRVW->splframe = 1;

				(*lw_render_full)(NULL);

				SDS_CURGRVW->splframe = splframe;

				rb.resval.rint = highlight;
				SDS_setvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				}
			break;
 		case ICAD_WNDACTION_RND_EDITMAT:
			if(lw_edit_mtl)
				{
				(*lw_edit_mtl)(NULL);
				}
			break;
 		case ICAD_WNDACTION_RND_EDITBG:
			if(lw_edit_bg)
				{
				(*lw_edit_bg)(NULL);
				}
			break;
 		case ICAD_WNDACTION_RND_ROPTIONS:
			if(lw_options)
				{
				(*lw_options)(NULL);
				}
			break;
 		case ICAD_WNDACTION_RND_QRENDER:
			if(qr_render)
				{
				struct resbuf rb;
				SDS_getvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				int highlight = rb.resval.rint;

				rb.resval.rint = 0;
				SDS_setvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

				(*qr_render)(NULL);


				rb.resval.rint = highlight;
				SDS_setvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				}
			break;
 		case ICAD_WNDACTION_RND_QROPTIONS:
			if(qr_options)
				{
				(*qr_options)(NULL);
				}
			break;
		case ICAD_WNDACTION_CHGCVPORT:
			if(m_wndViewClose==NULL || m_wndViewClose->m_hWnd==NULL)
				break;

			ASSERT_VALID( m_wndViewClose );

			m_wndViewClose->SetNewView(FALSE);
			((CIcadChildWnd*)m_wndViewClose->GetParentFrame())->SetFocus();

			ASSERT_VALID( m_wndViewClose );

			m_wndViewClose=NULL;
			break;
 		case ICAD_WNDACTION_ENDDRAG:
			extern int SDS_DidDraggenOnce;
			if(NULL==m_pDragVarsCur || SDS_DidDraggenOnce==0) break;
			if(m_pDragVarsCur->scnf) {
				// Pass NULL if an internal command... or drag line doesn't erase properly
				// User apps may not handle NULL however
				if ( cmd_InsideMoveCopyCommand || cmd_InsideRotateCommand || cmd_InsideScaleCommand || cmd_InsideMirrorCommand )
					(*m_pDragVarsCur->scnf)(m_pDragVarsCur->LastDrag, NULL);
				else
					(*m_pDragVarsCur->scnf)(m_pDragVarsCur->LastDrag, m_pDragVarsCur->matx);
			}
			if(m_pDragVarsCur->vects) {
				SDS_DrawDragVectors(NULL);
				ic_ptcpy(m_pDragVarsCur->pt3,m_pDragVarsCur->LastDrag);
			}
			break;
 		case ICAD_WNDACTION_FREEVIEWRBS:
			{
				if(NULL==m_pDragVarsCur->vects) break;

				CIcadChildWnd* pChild = (CIcadChildWnd*)MDIGetActive(NULL);
				if(pChild==NULL) break;
				CIcadDoc* pDoc = (CIcadDoc*)pChild->GetActiveDocument();
				if(pDoc==NULL) break;
				CDocTemplate* pDocTemplate = pDoc->GetDocTemplate();
				POSITION pos = pDocTemplate->GetFirstDocPosition();
				while(pos!=NULL)
				{
					pDoc = (CIcadDoc*)pDocTemplate->GetNextDoc(pos);
					CIcadView *pView;
					if(pDoc) {
						POSITION pos=pDoc->GetFirstViewPosition();
						while (pos!=NULL) {
							pView=(CIcadView *)pDoc->GetNextView(pos);
							ASSERT_KINDOF(CIcadView, pView);

							sds_relrb(pView->m_pDragVects);	 pView->m_pDragVects=NULL;
						}
					}
				}
				m_pDragVarsCur->vects=NULL;
			}
			break;
		case ICAD_WNDACTION_CREATEHBMP:
			{
				CIcadView* pView = GetIcadView();
				HBITMAP hBmp = NULL;
				m_pvWndAction=NULL;
				if(NULL==pView)
					break;

				if(pView->CreatePictureFromSS(SDS_EditPropsSS,&hBmp,NULL,NULL,NULL,TRUE))
				{
					//*** NOTE: Calling function must DeleteObject hBmp
					m_pvWndAction = (void*)hBmp;
				}
			}
			break;
		case ICAD_WNDACTION_CREATEHWMF:
			{
				m_pvWndAction=NULL;
				CIcadView* pView = GetIcadView();
				//*** NOTE: Calling function must delete pWmfInfo
				ICADWMFINFO* pWmfInfo = new ICADWMFINFO;
				if(NULL==pView || NULL==pWmfInfo)
					break;

				memset(pWmfInfo,0,sizeof(ICADWMFINFO));
				pView->CreatePictureFromSS(SDS_EditPropsSS,NULL,NULL,pWmfInfo,NULL,TRUE);
				m_pvWndAction = (void*)pWmfInfo;
			}
			break;
		case ICAD_WNDACTION_CREATEHEMF:
			{
				CIcadView* pView = GetIcadView();
				HENHMETAFILE hMetaFile = NULL;
				m_pvWndAction=NULL;
				if(NULL==pView)
					break;

				//*** NOTE: Calling function must call DeleteEnhMetaFile() to free hMetaFile.
				// Modified Cybage VSB 05/04/2001 (dd/mm/yy)[
				// Reason: Fix for the EMF bug reported Ben Thum email dated 02/04/2001
				//if(pView->CreatePictureFromSS(SDS_EditPropsSS,NULL,&hMetaFile,NULL,NULL,TRUE))
				// Modified Cybage VSB 05/04/2001  ]
				if(pView->CreatePictureFromSS(SDS_EditPropsSS,NULL,&hMetaFile,NULL,NULL,TRUE,TRUE))
				{
					m_pvWndAction = (void*)hMetaFile;
				}
			}
			break;
 		case ICAD_WNDACTION_SETMRUSIZE:
			{
				CIcadApp* pApp = (CIcadApp*)AfxGetApp();
				if(NULL==pApp)
					break;

				struct sds_resbuf rb;
				SDS_getvar(NULL,DB_QNFILELIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				pApp->SetMRUSize(rb.resval.rint);
			}
			break;
 		case ICAD_WNDACTION_REDRAWALLWIN:
			{
				CIcadChildWnd* pChild = (CIcadChildWnd*)MDIGetActive(NULL);
				if(pChild==NULL) break;
				CIcadDoc* pDoc = (CIcadDoc*)pChild->GetActiveDocument();
				if(pDoc==NULL) break;
				CDocTemplate* pDocTemplate = pDoc->GetDocTemplate();
				POSITION pos = pDocTemplate->GetFirstDocPosition();
				while(pos!=NULL)
				{
					pDoc = (CIcadDoc*)pDocTemplate->GetNextDoc(pos);
					CIcadView *pView;
					if(pDoc) {
						POSITION pos=pDoc->GetFirstViewPosition();
						while (pos!=NULL) {
							pView=(CIcadView *)pDoc->GetNextView(pos);
							ASSERT_KINDOF(CIcadView, pView);

							SDS_RedrawDrawing(((CIcadDoc*)pDoc)->m_dbDrawing,pView,(CIcadDoc*)pDoc,1);
						}
					}
				}
			}
			break;
 		case ICAD_WNDACTION_EXP_RETURN:
			{
				extern HWND SDS_hWndExplorerCaller;
				::SendMessage(SDS_hWndExplorerCaller, WM_COMMAND, ICAD_WNDACTION_EXP_RETURN, 0);
			}
			break;
		case ICAD_WNDACTION_RESETOSNAPMANAGER:
			{
				CIcadView* pView = GetIcadView();
				if( pView )
				{
					COsnapManagerIcad *pOsnapManager = ((CEngineApplication*)SDSApplication::GetEngineApplication())->getOsnapManager();
					pOsnapManager->reset( pView );
					pView->updateToolTip();
				}
			}
			break;
		case ICAD_WNDACTION_PROCESSOSNAPPOINT:
			{
				struct gr_view *grView = NULL;
				CIcadView* pView = GetIcadView();
				if( !pView || !IsWindow(pView->m_hWnd) )
					break;

				// reset OsnapManager first
				COsnapManagerIcad *pOsnapManager = ((CEngineApplication*)SDSApplication::GetEngineApplication())->getOsnapManager();
				pOsnapManager->reset( pView );
				pView->updateToolTip();

				// processPoint if cursor is over current view window
				grView = pView->m_pCurDwgView;
				if( grView )
				{
					POINT point;
					if( !::GetCursorPos( &point ) )
						break;

					CRect wndRect;
					pView->GetWindowRect( &wndRect );
					if( !wndRect.PtInRect( point ) )
						break;
					pView->ScreenToClient( &point );
					sds_point pt;
					gr_pix2rp( grView,point.x,point.y,&pt[0],&pt[1] );
					gr_rp2ucs(pt, pt, grView);
					SDS_SnapPt( pt, pt );
					gr_ucs2rp(pt, pt, grView);
					COsnapManagerIcad *pOsnapManager = ((CEngineApplication*)SDSApplication::GetEngineApplication())->getOsnapManager();
					pOsnapManager->processPoint( pView, pt );
				}
			}
			break;
		case ICAD_WNDACTION_LAYOUT:
			{
#ifdef _LAYOUTTABS_
				ASSERT(m_pCurFrame == MDIGetActive());
				m_pCurFrame->UpdateLayoutTabs(m_pCurDoc->m_dbDrawing, TRUE);
#endif
			}
			break;
}

#if !defined(DEBUG)
	} // try

	catch(SDS_Exception_Wndaction e)
	{
		// See notes in switch ICAD_WNDACTION_EXIT for whats going on here.
		if(!CloseApp())	return;
		if(!m_nIsClosing) return;
		if(!m_bAlreadyDestroyed) DestroyWindow();
		return;
	}
#endif

}


