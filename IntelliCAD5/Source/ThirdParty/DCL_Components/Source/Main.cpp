/** FILENAME:     Main.cpp- Contains implementation of class CMain.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract : Global main class
*
*
*
*Audit Log
*
*
*Audit Trail
*
*
*/


#include "stdafx.h"
#include "constants.h"
#include "main.h"
#include "dclprivate.h"
#include "sds.h"
#include "tileattributes.h"
#include "fileinfo.h"
#include "tree.h"
#include "treenode.h"
#include "tile.h"
#include "screenobject.h"
#include "exceptionmanager.h"
#include "Dialog.h"
#include "imagebutton.h"
#include "listbox.h"




#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


extern Main g_Main;
//Bugzilla No. 54233 ; 22-04-2002 [
typedef int (*SET_VAR_FNPTR)(const char *sym, struct sds_resbuf *szPathFound);
void RetFunctionAddr(SET_VAR_FNPTR *fnptr)
{
	HINSTANCE hInstLib=NULL;
	hInstLib=GetModuleHandle("icad.exe"/*DNT*/);
	if ( hInstLib != NULL )
		*fnptr=(SET_VAR_FNPTR)GetProcAddress(hInstLib,"sds_setvar"/*DNT*/);
}
void SetDiaStatVariable(int nMessage)
{
	static SET_VAR_FNPTR SetVarfnptr = NULL;
	struct resbuf rb;
	rb.restype = RTSHORT;
	if ( nMessage == SDS_DLGOK)
		rb.resval.rint = 1;
	else 
		rb.resval.rint = 0;
	if(!SetVarfnptr)
		RetFunctionAddr(&SetVarfnptr);
	if(SetVarfnptr)	
		(*SetVarfnptr)("DIASTAT"/*DNT*/,&rb);		
}
//Bugzilla No. 54233 ; 22-04-2002 ]
/************************************************************************************
/* + Main::Main -
*	Constructor
*
*
*
*
*/

Main::Main()
	{
	//m_nObjectsCreated = 0;
	lpSubClassProc = NULL;
	}

/************************************************************************************
/* + Main::~Main -
*	Destructor
*
*
*
*
*/
Main::~Main()
	{
	CMapStringToPtr* m_keytoObject = NULL;
	CString key;
	POSITION position = m_hdlgtoDialog.GetStartPosition();
	while ( position )
		{
		// find the dialog tree
			m_hdlgtoDialog.GetNextAssoc(position,key,(CObject*&)m_keytoObject);
			if ( m_keytoObject )
				{
				if ( m_keytoObject->IsEmpty() == FALSE)
					{
					m_keytoObject->RemoveAll(); // clean the map
					m_hdlgtoDialog.SetAt(key,NULL);
					}
					delete  m_keytoObject; // delete the map
				}
		}
	}

/************************************************************************************
/* + int Main::dlg_mode_tile(sds_hdlg hDialog, LPTSTR szControlKey, int iTileStat -
*	Sets a tile's mode.
*
*
*
*
*/

int Main::dlg_mode_tile(					// Sets a tile's mode.
						sds_hdlg hDialog,	// i : Handle to the dialog.
						LPTSTR szControlKey,// i : Key specifies the tile.
						int iTileState		// i : Mode to set(enable,disable,setfocus,setsel,flip).
						)
	{
	try
	{
		ScreenObject * pScreenObject;
		int RetVal;
		pScreenObject = GetDialogControl(hDialog,szControlKey); // function gets the screen object of the key
		if(pScreenObject != NULL)
			{
			// call the function of the screenobject.
			RetVal =pScreenObject->ModeTile (iTileState);
			return RetVal;
			}
		}
	catch(...)
		{
		TRACE ("failed in dlg_mode_tile in g_Main");
		return SDS_RTERROR;
		}
	return SDS_RTERROR;
	}


/************************************************************************************
/* + int Main::dlg_set_tile(sds_hdlg hDialog, LPTSTR szControlKey, LPTSTR szControlInf -
*	Sets the value of the tile.
*
*
*
*
*/
int Main::dlg_set_tile(						// Sets the value of the tile.
					   sds_hdlg hDialog,	// i : Handle to the dialog.
					   LPTSTR szControlKey,	// i : Key specifies the tile.
					   LPTSTR szControlInfo	// i : Value to assign.
					   )
	{
	try
	{
		ScreenObject * pScreenObject;
		int RetVal;
		// function gets the screen object of the key

/*		if ( _tcscmp(szControlKey,_T("radgroup") ) )
			{
			_tcscpy(szControlKey,_T("rad_1"));
			} */

		pScreenObject = GetDialogControl(hDialog,szControlKey);
		if(pScreenObject != NULL)
			{
			// call the function in the ScreenObject.
			RetVal =pScreenObject->SetTile (szControlKey,szControlInfo);

			if ( RetVal == SDS_RTNORM )
				{
                if ( pScreenObject->m_attriblist->IsRadioGroup() == TRUE )
					{

					pScreenObject = GetDialogControl(hDialog,szControlInfo);
					if(pScreenObject != NULL)
						{
                        RetVal =pScreenObject->SetTile (szControlInfo,_T("1"));
						}
					}
				}


			return RetVal;
			}
		}
	catch(...)
		{
		TRACE ("failed in dlg_set_tile in g_Main");
		return SDS_RTERROR;
		}
	//Modified Cybage SBD 30/03/2001 DD/MM/YYYY
	//Reason : Fix for Bug No. 77761 from Bugzilla
	//return SDS_RTNORM;
	return SDS_RTERROR;
	}

/************************************************************************************
/* + int Main::dlg_start_image(sds_hdlg hDialog, LPTSTR szControlKe -
*	 Starts creation of an image in the specified tile.
*
*
*
*
*
*/

int Main::dlg_start_image(						// Starts creation of an image in the specified tile.
						  sds_hdlg hDialog,		// i : Handle to the dialog.
						  LPTSTR szControlKey	// i : Key specifies the tile.
						  )
	{
	try{
		ScreenObject * pScreenObject;
		// function gets the screen object of the key
		pScreenObject = GetDialogControl(hDialog,szControlKey);
		if(pScreenObject != NULL)
		{
			// save the screen object in the member m_pImageButton to be used in vector or slide image
			m_pImageButton = dynamic_cast<ImageButton*>(pScreenObject);
			if ( m_pImageButton )
			{
				m_pImageButton->Start();
    			
    			return	SDS_RTNORM;
			}
		}
	}
	catch(...)
		{
		TRACE ("failed in dlg_start_image in g_Main");
		return SDS_RTERROR;
		}
	return SDS_RTERROR;
	}


/************************************************************************************
/* + int Main::Init()
*	 To Initialize the parser.
*
*
*
*
*/

int Main::Init(												// To Initialize the parser.
			   CString wszPathOfBaseDCL ,					// Path of IBase.dcl
			   BOOL bGenerateErrorLog  ,					// Flag set for generation of DCL log.
			   CString wszPathForErrorLog,  					// Path for Error.log to be generated.
			   void func( sds_callback_packet*)
			   )
	{
	int nReturn = SDS_RTERROR;

	// Initialize Parser
	try
		{
		nReturn = m_DCLParser.Init(wszPathOfBaseDCL,bGenerateErrorLog,wszPathForErrorLog);

		// Initialize Default Attributes
		nReturn = defaultAttrib.Init();
		// code to get INtelliCad's window handle
		#ifndef _TESTEXE
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
				 // this is an SDS function which returns the handle of Icad stored in
				// g_Main.hParentHwnd used by all the dialoges as the parent window
				g_Main.hParentHwnd = pFun();
				}
			}
		#endif
		}

	catch(CExceptionManager *e)
		{
		e->Delete();
		TRACE("An error ocured while initializing the DCL dll");
		return SDS_RTERROR;
		}


	return nReturn;
	}

/****************************************************************************************************
/* + int Main::Exit -
*	CleanUp and free all the resources.
*
*
*
*
*/

int Main::Exit()								// CleanUp and free all the resources
	{
	defaultAttrib.Exit();
	return SDS_RTNORM;
	}

/************************************************************************************************************
/* + int Main::dlg_fill_image(int nULCornerX, int nULCornery, int nWidthLess1, int nHeightLess1, int nColo -
*	 Draws a filled rectangle in the currently active image.
*
*
*
*
*/

int Main::dlg_fill_image(					// Draws a filled rectangle in the currently active image.
						 int nULCornerX,	// i : X coordinate of Upper Left Corner.
						 int nULCornery,	// i : Y coordinate of Upper Left Corner.
						 int nWidthLess1,	// i : Width relative from the Upper Left Corner.
						 int nHeightLess1,	// i : Height relative from the Upper Left Corner.
						 int nColor			// i : Specifies the color.
						 )
	{

	try
	{
		if( m_pImageButton  != NULL)
    		{
    		// call the function from screenobject whwre the m_pImageButton 
    		// object is initialized in start image
    		m_pImageButton->DrawFillImage(nULCornerX,
    									  nULCornery,
    									  nWidthLess1,	
    									  nHeightLess1,	
    									  nColor);	
    						
    		return	SDS_RTNORM;									
    		}
    	}
	catch(...)
		{
		TRACE ("failed in dlg_fill_image in g_Main");
		return SDS_RTERROR;
		}
	return SDS_RTERROR;
	}


/************************************************************************************
/* + int Main::dlg_slide_image(int nULCornerX, int nULCornery, int nWidthLess1, int nHeightLess1, LPTSTR szSnapshotNam -
*	 Draws a slide in the currently acive image.
*
*
*
*
*/

int Main::dlg_slide_image(					// Draws a slide in the currently acive image.
						  int nULCornerX,	// i : X coordinate of Upper Left Corner.
						  int nULCornery,	// i : Y coordinate of Upper Left Corner.
						  int nWidthLess1,	// i : Width relative from the Upper Left Corner.
						  int nHeightLess1,	// i : Height relative from the Upper Left Corner.
						  LPTSTR szSnapshotName	// i : Slide name or the library.
						  )
	{
	try
		{
		if( m_pImageButton  != NULL)
			{
			// call the function from screenobject where the m_pImageButton
			// object is initialized in start image
			m_pImageButton->DrawSlideImage(nULCornerX,
																			nULCornery,
																			nWidthLess1,
																			nHeightLess1,
																			szSnapshotName);

			return	SDS_RTNORM;
			}
		}
	catch(...)
		{
		TRACE ("failed in dlg_fill_image in g_Main");
		return SDS_RTERROR;
		}


	return SDS_RTNORM;
	}


/************************************************************************************
/* + int Main::dlg_vector_image(int nXVectStart, int nYVectStart, int nXVectEnd, int nYVectEnd, int nColo -
*	 Draws a vector in the active image.
*
*
*
*
*/

int Main::dlg_vector_image(						// Draws a vector in the active image.
							int nXVectStart,	// i : X coordinate of point1.
							int nYVectStart,	// i : Y coordinate of point1.
							int nXVectEnd,		// i : X coordinate of point2.
							int nYVectEnd,		// i : Y coordinate of point2.
							int nColor			// i : specifies the color.
							)
	{
	try
		{
		if( m_pImageButton  != NULL)
    	{
    		// call the function from screenobject where the m_pImageButton 
    		// object is initialized in start image
    		m_pImageButton->DrawVectorImage(nXVectStart,
																			nYVectStart,
																			nXVectEnd,
																			nYVectEnd,
																			nColor	);

			return	SDS_RTNORM;
			}
		}
	catch(...)
		{
		TRACE ("failed in dlg_vector_image in g_Main");
		return SDS_RTERROR;
		}
	return SDS_RTERROR;

	}

/************************************************************************************
/* + int Main::dlg_end_image
*	 Ends creation of the currently active image.
*
*
*
*
*/

int Main::dlg_end_image()						// Ends creation of the currently active image.
	{
	try
    {
		// the member m_pImageButton is initialized to null here indicating end 
    	// of operation on the selected image in start image.
    	if( m_pImageButton  != NULL)
    	{
    		m_pImageButton->UpdateWindow(); 
    		m_pImageButton = NULL;
    		return	SDS_RTNORM;
    	}
    }
	catch(...)
		{
		TRACE ("failed in dlg_end_image in g_Main");
		return SDS_RTERROR;
		}

	return SDS_RTERROR;
	}
#ifdef __4M_RENDER__
//Elias Add:1->
/************************************************************************************
/* + int Main::dlg_create_preview_image
*	  Creates a bitmap preview of the specified object type.
*
*
*
*
*/

int Main::dlg_create_preview_image(
									  sds_hdlg hDialog,		 // i : Handle to the dialog.
									  LPTSTR szControlKey,	 // i : Key specifies the tile.
									  const void * pPreview  // [in]  The preview definition structure
									)
	{
	try
		{
		ScreenObject * pScreenObject;
		// function gets the screen object of the key
		pScreenObject = GetDialogControl(hDialog,szControlKey);
		if( pScreenObject != NULL)
			{
			// call the function from screenobject where the m_pScrObjForImage
			// object is initialized in start image
			(static_cast<ImageButton*>(pScreenObject))->CreatePreviewBitmap( pPreview );
            (static_cast<ImageButton*>(pScreenObject))->UpdateWindow();

			return	SDS_RTNORM;
			}
		}
	catch(...)
		{
		TRACE ("failed in dlg_create_preview_image in g_Main");
		return SDS_RTERROR;
		}

	return SDS_RTERROR;
	}
//Elias Add:1<-
#endif //__4M_RENDER__
/************************************************************************************
/************************************************************************************
/* + int Main::dlg_new_dialog(LPTSTR szDialogName, int nDlgFileID, VOID *pfnDefCallBack, sds_hdlg hDialo -
*	 Start handling a new dialog.
*
*
*
*
*/


int Main::dlg_new_dialog(						// Start handling a new dialog.
						 LPTSTR szDialogName,	// i : DialogName.
						 int nDlgFileID,		// i : Id of the dcl file.
						 VOID *pfnDefCallBack,	// i : Callback to set if any.
						 sds_hdlg* hDialog		// o : DialogBox handle.
						 )
	{

	try{
		// finds the tree for the given dialog
		Tree *pTree = g_Main.m_DCLParser.FindReferencedTree(szDialogName,nDlgFileID);
		// The following code was added for the case when the same dialog is invoked
		// again.Instead of keeping the same Tree pointer in the m_hdlgToTree map
		// the tree is cloned and the new pointer is added to the map.
		// the Tree is destroyed when start_dialog returns.
		// Start
    		POSITION posStart = g_Main.m_hdlgtoTree.GetStartPosition();
    		for ( int iIndex = 0; iIndex < g_Main.m_hdlgtoTree.GetCount(); iIndex++ )
    			{
    			Tree* pTempTree = NULL;
    			CString key;
    			g_Main.m_hdlgtoTree.GetNextAssoc(posStart,key,(CObject*&)pTempTree);
				/*DG - 11.2.2003*/// Added comparing of pDialogAddress'es.
				// Reason: m_DCLParser can have a tree which isn't present in m_hdlgtoTree,
				// because Tree::CreateTreeNodeScreenObj can replace a map entry with the same key
				// (it calls g_Main.m_hdlgtoTree.SetAt with a key for a new Dialog, but this key
				// can be already present in m_hdlgtoTree! because a Dialog can be allocated at
				// the same place in memory as a previous one, especially in release mode).
    			if(pTree == pTempTree || pTree->pDialogAddress == pTempTree->pDialogAddress)
    				{
    				TreeNode* pNewNode = new TreeNode;
    				ASSERT(pNewNode);
    				Tree* pCloneTree = new Tree(pNewNode,pTree->get_TreeType());
    				pCloneTree->get_StartTreeNode()->CloneTree(pTree->get_StartTreeNode(),pTree->get_TreeType());
    				pTree = pCloneTree;
    
    				m_DCLParser.AddClonedTree(pTree,nDlgFileID);
    				
    				}
    			}
		// End
	int nRes;
	if(pTree)
		{
		try
			{
		nRes = pTree->Init();
		if ( nRes != SDS_RTERROR )
			{
			 // pankaj 9 june
			_sds_hdlg* handle = new _sds_hdlg;
			int p;
			p = atoi((LPCTSTR)pTree->pDialogAddress);
			handle->p = p;
			*hDialog = handle;
			// Initialise the DIALOG::handle and retrieve it to fill in CALLBACK_PACKET::dialog.
			((Dialog*)pTree->get_StartTreeNode()->m_Tile.get_ScreenObject())->handle = handle;
        	m_ArrDlgHandles.Add(p);

		/////////////////////////////////////////
			nRes = pTree->CalcWidthHeight();
			if ( nRes == SDS_RTERROR )
				{
				return SDS_RTERROR;
				}
			nRes = pTree->AdjustWidthHeight();
			if ( nRes == SDS_RTERROR )
				{
				return SDS_RTERROR;
				}
			nRes = pTree->ReAdjust();
			if ( nRes == SDS_RTERROR )
				{
				return SDS_RTERROR;
				}
			nRes = pTree->CalcPosition();
			if ( nRes == SDS_RTERROR )
				{
				return SDS_RTERROR;
				}
			#ifdef _DEBUG
			pTree->PrintDisplayRects();
			#endif
			nRes = pTree->CreateOnScreenObjects();
			if ( nRes == SDS_RTERROR )
				{
				return SDS_RTERROR;
				}
			(static_cast<Dialog *>(pTree->get_StartTreeNode ()->m_Tile.get_ScreenObject()) )->m_DefCallBack = CLIENTFUNC(pfnDefCallBack);
			CRect lRect;
			HWND DskTop = g_Main.hParentHwnd;
			RECT rectDskTop;
			//2000-10-27 EBATECH(FUTA) Fix: wrong dialog box location[
			//::GetClientRect(DskTop,&rectDskTop);
			::GetWindowRect(DskTop,&rectDskTop);
			//2000-10-27 EBATECH(FUTA)]
			UINT nFlags = SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE;
			lRect = pTree->get_StartTreeNode()->m_Tile.get_ScreenObject()->GetDisplayRect();
			int nWidth = lRect.Width();
			int nHeight = lRect.Height();
			int nLeft = ((rectDskTop.left + rectDskTop.right)/2 - (nWidth/2));
			int nTop = ((rectDskTop.top + rectDskTop.bottom)/2 - (nHeight/2));
			if (nLeft < 0)nLeft = 0;
			if (nTop < 0)nTop = 0;
			lRect.OffsetRect(CPoint(nLeft,nTop));
			pTree->get_StartTreeNode()->m_Tile.get_ScreenObject()->GetWindow()->MoveWindow(lRect);
			pTree->get_StartTreeNode()->m_Tile.get_ScreenObject()->GetWindow()->ShowWindow(SW_SHOW);
			}
			}
		catch(...)
			{

			TRACE(_T("Exception in new_dialog()"));
			return SDS_RTERROR;
			}

		}
		else
		{
			return SDS_RTERROR;
		}
		}
	catch(...)
		{
		TRACE ("failed in dlg_new_dialog in g_Main");
		return SDS_RTERROR;
		}
	return SDS_RTNORM;
	}


/************************************************************************************
/* + int Main::dlg_load_dialog(LPTSTR wszFiletoLoad, int *nDlgFileI -
*	 Loads the specified dcl file.
*
*
*
*
*/

int Main::dlg_load_dialog(						// Loads the specified dcl file.
						  LPTSTR wszFiletoLoad,	// i : String that specifies the file.
						  int *nDlgFileID		// o : FileID
						  )
	{
    int nReturn = SDS_RTERROR;
    // Call the method of Parser to load the DCL File and start parsing.
	try
		{
		nReturn = m_DCLParser.LoadDCLFile(wszFiletoLoad,nDlgFileID);
		}
	catch(CExceptionManager *e)
		{
		e->Delete();
		TRACE("An error occured during loading of DCL Files\n");
		return SDS_RTERROR;
		}

	return nReturn;
	}

/************************************************************************************
/* + int Main::dlg_unload_dialog(int nDlgFileI -
*	 Unloads the specified file.
*
*
*
*
*/

int Main::dlg_unload_dialog(				// Unloads the specified file.
							int nDlgFileId	// i : ID of the file.
							)
	{
	try
		{

		// call dlg_term dialog to cleenup screenobjects inthe dialog
		//g_Main.dlg_term_dialog();


		// call UnLoadDCLFile  to clean the tree
		m_DCLParser.UnLoadDCLFile(nDlgFileId);
		return SDS_RTNORM;
		}
	catch(CExceptionManager *e)
		{
		e->Delete();
		TRACE("An error occured while un loading the DCL File\n");
		return SDS_RTERROR;
		}

	}

/************************************************************************************
/* + int Main::dlg_start_dialog(sds_hdlg hDialog, int *nMessage)
*	 Starts a dialogbox.
*
*
*
*
*/

int Main::dlg_start_dialog(					// Starts a dialogbox.
						   sds_hdlg hDialog,// i : Handle to the dialog.
						   int *nMessage	// o : Status(OK,Cancel).
						   )
	{


	Tree * pTree = NULL;
	TCHAR string [DIALOGADDRESS];
	try
		{
		ScreenObject * pScreenObject = NULL;
		Tree* pPreviuos = NULL;
		CWnd* pParent;
		int RetVal;
		int actuanval = hDialog->p;
		int val = actuanval;
		_itot(val,string,SUBDIALOGADDRESS);

		TRACE(" The Dialog Address String is : %s \n", string );

		// get the tree of the dialog which state is to be modified
		g_Main.m_hdlgtoTree.Lookup (string,(CObject * &)pTree);
		if(pTree)
			{
			// get the parent (ICad ) and disable the parent window
			// Disable the previuos window and enable this.
			// When this is closed enable the previous one.

			pParent = (static_cast<Dialog *>(pTree->get_StartTreeNode ()->m_Tile .get_ScreenObject ()))->GetParent ();
			Tree* pPreviousTree = NULL;
			TCHAR prevString [DIALOGADDRESS];
			int nSize = m_ArrDlgHandles.GetSize();
			CWnd* pIcadWnd = CWnd::FromHandle(g_Main.hParentHwnd);
			ASSERT(pIcadWnd);
			DWORD iCadStyle = pIcadWnd->GetStyle();
			::SetWindowLong(g_Main.hParentHwnd,GWL_STYLE,iCadStyle|WS_DISABLED);
			if ( (nSize - 2) >= 0 )
				{
				int pValue = m_ArrDlgHandles.GetAt(nSize-2);
				_itot(pValue,prevString,SUBDIALOGADDRESS);
				g_Main.m_hdlgtoTree.Lookup ((LPCTSTR)prevString,(CObject * &)pPreviousTree);
				ASSERT( pPreviousTree );
				CWnd* pWnd = CWnd::FromHandle(pPreviousTree->hDialog);
				DWORD style = pWnd->GetStyle();
				style = ::SetWindowLong(pPreviousTree->hDialog,GWL_STYLE,style|WS_DISABLED);
				}

			HWND hParent;
			//////////////////
			// it initial_focus attribute is set for the dialog
			// the find the screenobject given the key of the toile
			// and set the focus to that control
			CString nameoftile = pTree->get_StartTreeNode ()->m_Tile .get_ScreenObject ()->m_attriblist->get_initialfocus ();
			ScreenObject * pScreenObject = NULL;
			CMapStringToPtr * m_keytoObject;

			g_Main.m_hdlgtoDialog .Lookup (string,(CObject * &)m_keytoObject);
			if(m_keytoObject != NULL)
			{
			m_keytoObject->Lookup (nameoftile ,(void * &)pScreenObject );
			if(pScreenObject != NULL )
				{
				pScreenObject->InitFocus ();
				}
			}
			//get the dialog window and enable it
			// on the chile call rumModal to make the Dialogbox Modal
			(static_cast<Dialog *>(pTree->get_StartTreeNode ()->m_Tile .get_ScreenObject ()))->m_IsModal = TRUE;
			(static_cast<Dialog *>(pTree->get_StartTreeNode ()->m_Tile .get_ScreenObject ()))->EnableWindow ();
			//Modified SAU 12/07/200          [
			//REGRESSION:  is_cancel attribute is ignored
			if(pTree->GetCancelScrObj()!=NULL)
			(static_cast<Dialog *>(pTree->get_StartTreeNode ()->m_Tile .get_ScreenObject ()))->Set_DiaCancelobj(pTree->GetCancelScrObj());
			//Modified SAU 12/07/200          ]
			Dialog* pDialog = (Dialog*)pTree->get_StartTreeNode()->m_Tile.get_ScreenObject();
			ASSERT(pDialog);
			pDialog->set_ModalResult(0);
			RetVal = (static_cast<Dialog *>(pTree->get_StartTreeNode ()->m_Tile .get_ScreenObject ()))->RunModalLoop (MLF_NOIDLEMSG);
			if ( pDialog->get_ModalResult() > 0 )
				*nMessage = RetVal;
			else
				{
				int nRes = RemoveHandle(hDialog->p);
				ASSERT(nRes);
				*nMessage = 0;
				}
			TRACE("afTER RUN MODAL LOOP1\n");
			nSize = m_ArrDlgHandles.GetSize();
			if ( (nSize - 1) >= 0 )
				{
				Tree* pActiveTree = NULL;
				int pValue = m_ArrDlgHandles.GetAt(nSize-1);
				_itot(pValue,prevString,SUBDIALOGADDRESS);
				g_Main.m_hdlgtoTree.Lookup ((LPCTSTR)prevString,(CObject * &)pActiveTree);
				ASSERT( pActiveTree );
				hParent = pActiveTree->hDialog;
				}
			else
				{
				hParent = g_Main.hParentHwnd;
				}
			BOOL bRes = ::EnableWindow(hParent,true);
			::SetFocus(hParent);
			TRACE("afTER RUN MODAL LOOP2\n");
			bRes = ::ShowWindow(hParent,SW_SHOW);
			(static_cast<Dialog*>(pTree->get_StartTreeNode()->m_Tile.get_ScreenObject()))->DestroyWindow ();
			ASSERT(pTree);
			if ( pTree )
				{
				pTree->Exit();
 				//g_Main.m_hdlgtoTree.RemoveKey(string);
				// delete pTree;
				}

			TRACE("afTER RUN MODAL LOOP3\n");
			return SDS_RTNORM;
			}
		}
	catch (...)
		{

		// This code segment was added to handle condition where there is an unload_dailog() ,
		//  before start dialog.


		DWORD str;
		str = ::GetLastError();
		TRACE("Start Dialog failed");
		return SDS_RTERROR;
		}
	return SDS_RTNORM;
	}

/************************************************************************************
/* + int Main::dlg_done_dialog(sds_hdlg hDialog, int nMessage() -
*	 Terminates the current dialogbox.
*
*
*
*
*/

int Main::dlg_done_dialog(					// Terminates the current dialogbox
						  sds_hdlg hDialog,	 // i : Handle to the dialog.
						  int nMessage		 // i : Status(eg OK,Cancel).
						  )
	{

	ScreenObject * pScreenObject = NULL;
	Tree * pTree = NULL;
	CMapStringToPtr * m_keytoObject;
	int actuanval = hDialog->p;
	int val = actuanval;
	char str [DIALOGADDRESS];
	CString string;
	itoa(val,str,SUBDIALOGADDRESS);
	string = str;
	CWnd *pWnd = NULL;
	try
		{
			g_Main.m_hdlgtoTree.Lookup (string,(CObject * &)pTree);
			ASSERT(pTree);
			Dialog* pDialog = static_cast<Dialog*>(pTree->get_StartTreeNode()->m_Tile.get_ScreenObject());
			ASSERT(pDialog);
		//Modified SAU 12/07/200          [
		//REGRESSION:  is_cancel attribute is ignored
		switch(nMessage)
			{
			case SDS_DLGOK :
				if ( pDialog->m_hWnd != NULL )
					{
					pDialog->set_ModalResult(1);
					pDialog->EndModalLoop(SDS_DLGOK);
					}

				break;
			case SDS_DLGCANCEL :

				if ( pDialog->m_hWnd != NULL )
					{
					pDialog->set_ModalResult(1);
					pDialog->EndModalLoop(SDS_DLGCANCEL);
					}

				break;
			case SDS_DLGALLDONE :

				if ( pDialog->m_hWnd != NULL )
					{
					pDialog->set_ModalResult(1);
					pDialog->EndModalLoop(SDS_DLGALLDONE);
					}
				break;
			default :
				if (nMessage >= SDS_DLGSTATUS)
				{

					if ( pDialog->m_hWnd != NULL )
					{
						pDialog->set_ModalResult(1);
						pDialog->EndModalLoop(nMessage);
					}

				}
				break;
			}
        //Modified SAU 12/07/200          ]
		//Bugzilla No. 54233 ; 22-04-2002 
		SetDiaStatVariable(nMessage);
		if(g_Main.m_hdlgtoDialog.Lookup (string,(CObject * &)m_keytoObject))
			{
		// find the dialog tree
			if ( m_keytoObject )
				{
				if ( m_keytoObject->IsEmpty() == FALSE)
					{
					m_keytoObject->RemoveAll(); // clean the map
					m_hdlgtoDialog.SetAt(string,NULL);
					}

				int nRes = RemoveHandle(hDialog->p);
				ASSERT(nRes);
				//Modified SAU 12/07/200
				//Reason:dismissing a sub-dialog without issuing a (done_dialog) kills the main dialog
				hDialog->p=0;
				delete  m_keytoObject; // delete the map
				}
			}
		else
			{
			TRACE("dialog not found");
			}


   // This code is added to take care of clean up for cases when unload_dialog is called
   // before start_dialog.
   // In this case this code gets executed through unload_dialog->term_dialog->done_dialog.

/*	ASSERT(pTree);
	if ( pTree )
		{
		(static_cast<Dialog*>(pTree->get_StartTreeNode()->m_Tile.get_ScreenObject()))->DestroyWindow ();
		pTree->Exit();
		} */
		}
	catch(...)
		{
		TRACE ("FAILED IN DONE DIALOG");
		return SDS_RTERROR;
		}
	return SDS_RTNORM;
	}

/************************************************************************************
/* + int Main::dlg_new_positioned_dialog(LPTSTR szDialogName, int nDlgFileID, void *pfnDef_CallBack, int nXPos, int nYPos, sds_hdlg *hDialo -
*	 Initializes a dialog box at a specified screen position.
*
*
*
*
*/

int Main::dlg_new_positioned_dialog(						// Initializes a dialog box at a specified screen position.
									LPTSTR szDialogName,	// i : DialogName.
									int nDlgFileID,			// i : FileID.
									void *pfnDef_CallBack,	// i : Callback to set.
									int nXPos,				// i : X coordinate of the Left Corner.
									int nYPos,				// i : Y coordinate of the Left Corner.
									sds_hdlg *hDialog		// o : Handle to the dialog.
									)
	{
	try
		{
		Tree *pTree = g_Main.m_DCLParser.FindReferencedTree(szDialogName,nDlgFileID);
		ASSERT( pTree );
		// The following code was added for the case when the same dialog is invoked
		// again.Instead of keeping the same Tree pointer in the m_hdlgToTree map
		// the tree is cloned and the new pointer is added to the map.
		// the Tree is destroyed when start_dialog returns.
		// Start
		POSITION posStart = g_Main.m_hdlgtoTree.GetStartPosition();
		for ( int iIndex = 0; iIndex < g_Main.m_hdlgtoTree.GetCount(); iIndex++ )
			{
			Tree* pTempTree = NULL;
			CString key;
			g_Main.m_hdlgtoTree.GetNextAssoc(posStart,key,(CObject*&)pTempTree);
			if ( pTree == pTempTree )
				{
				TreeNode* pNewNode = new TreeNode;
				ASSERT(pNewNode);
				Tree* pCloneTree = new Tree(pNewNode,pTree->get_TreeType());
				pCloneTree->get_StartTreeNode()->CloneTree(pTree->get_StartTreeNode(),pTree->get_TreeType());
				pTree = pCloneTree;
				m_DCLParser.AddClonedTree(pTree,nDlgFileID);
				}
			}
		// End
		int nRes;
		nRes = pTree->Init();
		if ( nRes != SDS_RTERROR )
			{
			 // pankaj 9 june
			int p;
			_sds_hdlg* handle = new _sds_hdlg;
			p = atoi((LPCTSTR)pTree->pDialogAddress);
			handle->p = p;
			*hDialog = handle;
			((Dialog*)pTree->get_StartTreeNode()->m_Tile.get_ScreenObject())->handle = handle;
		    m_ArrDlgHandles.Add(p);
		/////////////////////////////////////////
			nRes = pTree->CalcWidthHeight();
			if ( nRes == SDS_RTERROR )
				{
				return SDS_RTERROR;
				}
			nRes = pTree->AdjustWidthHeight();
			if ( nRes == SDS_RTERROR )
				{
				}
			nRes = pTree->ReAdjust();
			if ( nRes == SDS_RTERROR )
				{
				return SDS_RTERROR;
				}
			nRes = pTree->CalcPosition();
			if ( nRes == SDS_RTERROR )
				{
				return SDS_RTERROR;
				}
			#ifdef _DEBUG
			pTree->PrintDisplayRects();
			#endif

			nRes = pTree->CreateOnScreenObjects();
			if ( nRes == SDS_RTERROR )
				{
				return SDS_RTERROR;
				}
			(static_cast<Dialog *>(pTree->get_StartTreeNode ()->m_Tile.get_ScreenObject()) )->m_DefCallBack = reinterpret_cast <CLIENTFUNC> (pfnDef_CallBack);
			CRect lRect;
			UINT nFlags = SWP_NOMOVE;
			lRect = pTree->get_StartTreeNode()->m_Tile.get_ScreenObject()->GetDisplayRect();
			POINT point;
			// if the given position is invalid then dialog will come in center
			// else will come in the specified position
			if (nXPos >0 && nYPos >0 && nXPos <640 && nYPos <480 )
				{
				point.x = nXPos;
				point.y = nYPos;
				lRect.OffsetRect (point);
				}
			else
				{
				HWND DskTop = g_Main.hParentHwnd;
				RECT rectDskTop;
				//2000-10-27 EBATECH(FUTA) Fix: wrong dialog box location[
				//::GetClientRect(DskTop,&rectDskTop);
				::GetWindowRect(DskTop,&rectDskTop);
				//2000-10-27 EBATECH(FUTA)]
				UINT nFlags = SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE;
				lRect = pTree->get_StartTreeNode()->m_Tile.get_ScreenObject()->GetDisplayRect();
				int nWidth = lRect.Width();
				int nHeight = lRect.Height();
				int nLeft = ((rectDskTop.left + rectDskTop.right)/2 - (nWidth/2));
				int nTop = ((rectDskTop.top + rectDskTop.bottom)/2 - (nHeight/2));
				if (nLeft < 0)nLeft = 0;
				if (nTop < 0)nTop = 0;
				lRect.OffsetRect(CPoint(nLeft,nTop));
				}

			pTree->get_StartTreeNode()->m_Tile.get_ScreenObject()->GetWindow()->MoveWindow(lRect);
			pTree->get_StartTreeNode()->m_Tile.get_ScreenObject()->GetWindow()->ShowWindow(SW_SHOW);
			}
		}
	catch(...)
		{
		TRACE ("failed in dlg_new_positioned_dialog in g_Main");
		return SDS_RTERROR;
		}
	return SDS_RTNORM;
	}


/************************************************************************************
/* + int Main::dlg_done_positioned_dialog(sds_hdlg hDialog, int *nXPos, int *nYPos, int nMessag -
*	 Terminates the current dialogbox and returns the x,y coordinates.
*
*
*
*
*/

int Main::dlg_done_positioned_dialog(						// Terminates the current dialogbox and returns the x,y coordinates.
									 sds_hdlg hDialog,		// i : Handle to the dialog.
									 int *nXPos,			// o : X coordinate of the Left Corner.
									 int *nYPos,			// o : Y coordinate of the Left Corner.
									 int nMessage			// o : Status
									 )
	{

	ScreenObject * pScreenObject = NULL;
	Tree * pTree =  NULL ;
	CMapStringToPtr * m_keytoObject;
	int actuanval = hDialog->p;
	int val = actuanval;

	char str [DIALOGADDRESS];
	CString string;
	itoa(val,str,SUBDIALOGADDRESS);
	string = str;
	CWnd *pWnd = NULL;
	RECT rect;
	Dialog* pDialog = NULL;

	try
		{
		if(g_Main.m_hdlgtoDialog.Lookup (string,(CObject * &)m_keytoObject))
			{
			g_Main.m_hdlgtoTree.Lookup (string,(CObject * &)pTree);
			// gets the position of the dialog on the screen when this function is called
			(static_cast<Dialog *>(pTree->get_StartTreeNode ()->m_Tile.get_ScreenObject ()))->GetWindowRect (&rect);
			*nXPos = rect.left ;
			*nYPos = rect.top ;
			pDialog = static_cast<Dialog *>(pTree->get_StartTreeNode ()->m_Tile.get_ScreenObject());
			ASSERT( pDialog );
			//Modified SAU 12/07/200          [
			//REGRESSION:  is_cancel attribute is ignored
			switch(nMessage)
			{
			case SDS_DLGOK :
				if ( pDialog->m_hWnd )
					{
					pDialog->set_ModalResult(1);
					pDialog->EndModalLoop(SDS_DLGOK);
					}
				break;
			case SDS_DLGCANCEL :

				if ( pDialog->m_hWnd )
					{
					pDialog->set_ModalResult(1);
					pDialog->EndModalLoop(SDS_DLGCANCEL);
					}

				break;
			case SDS_DLGALLDONE :

				if ( pDialog->m_hWnd )
					{
					pDialog->set_ModalResult(1);
					pDialog->EndModalLoop(SDS_DLGALLDONE);
					}

				break;
			default :
				if (nMessage >= SDS_DLGSTATUS)
					{

					if ( pDialog->m_hWnd )
						{
						pDialog->set_ModalResult(1);
						pDialog->EndModalLoop(nMessage);
						}

					}
				break;
			}
			//Modified SAU 12/07/200               ]
			//Bugzilla No. 54233 ; 22-04-2002 
			SetDiaStatVariable(nMessage);

			if ( m_keytoObject->IsEmpty() == FALSE )
				{
				m_keytoObject->RemoveAll(); // cleans the map
				m_hdlgtoDialog.SetAt(string,NULL);
				//Modified SAU 12/07/200  [
				//Reason:dismissing a sub-dialog without issuing a (done_dialog) kills the main dialog
				int nRes = RemoveHandle(hDialog->p);
				ASSERT(nRes);
				hDialog->p=0;
				//Modified SAU 12/07/200  ]
				delete m_keytoObject; // deletes the map
				}

			}
		else
			{
			TRACE("dialog not found");
			}
		}
	catch(...)
		{
		TRACE ("FAILED IN DONE DIALOG");
		return SDS_RTERROR;
		}
	return SDS_RTNORM;
	}

/************************************************************************************
/* + int Main::dlg_start_list(sds_hdlg hDialog, TCHAR *wszControlKey, int nChangeAddNew, int nWhichIte -
*	 Starts processing the specified list box.
*
*
*
*
*/

int Main::dlg_start_list(									// Starts processing the specified list box.
						 sds_hdlg hDialog,					// i : Handle to the dialog.
						 TCHAR *wszControlKey,				// i : Key specifies the listbox.
						 int nChangeAddNew,					// i : Specifies the operation to be performed(change,append,new).
						 int nWhichItem						// i : Index indicates the list item to change.
						 )
	{
	try
		{
		ScreenObject * pScreenObject;
		// get the screenobject of the key
		pScreenObject = GetDialogControl(hDialog,wszControlKey);
		if(pScreenObject != NULL)
			{
			// sets the member  m_pSCRObjForList to indecate the startlist operation
			m_pSCRObjForList = pScreenObject;
			pScreenObject->ListStarted = 1;
			// nChangeAddNew specifies the type of operation to perform in add list
			// nWhichItem specifies the data of operation fro details see ADD list function in listBox class
			static_cast<ListBox*>(m_pSCRObjForList)->nChangeAddNew = nChangeAddNew;
			static_cast<ListBox*>(m_pSCRObjForList)->nWhichItem  = nWhichItem;
			return	SDS_RTNORM;
			}
		}
	catch(...)
		{
		TRACE ("failed in dlg_start_list in g_Main");
		return SDS_RTERROR;
		}
	return SDS_RTERROR;
	}

/************************************************************************************
/* + int Main::dlg_add_list(LPTSTR wszAddToLis )
*	 Adds the specified string to the current list.
*
*
*
*
*/

int Main::dlg_add_list(								// Adds the specified string to the current list.
					   LPTSTR wszAddToList			// i : String to be added.
					   )
	{
	try
		{
		int retval;
		if( m_pSCRObjForList  != NULL)
			{
			// call the function from screenobject where the m_pSCRObjForList
			// object is initialized in start list
			retval = m_pSCRObjForList->AddList (wszAddToList);
			return	retval;
			}
		}
	catch(...)
		{
		TRACE ("failed in dlg_add_list in g_Main");
		return SDS_RTERROR;
		}

	return SDS_RTERROR;
	}

/************************************************************************************
/* + int Main::dlg_end_list()
*	 Ends processing the currently active list.
*
*
*
*
*/

int Main::dlg_end_list()							// Ends processing the currently active list.
	{
	try
		{
		if( m_pSCRObjForList  != NULL)
			{
			// the member m_pSCRObjForList is initialized to null here indicating end
			// of operation on the selected list in start list.
			// alog with nChangeAddNew and nWhichItem set to -1
			m_pSCRObjForList->SetTile("","");	//2000-12-1 EBATECH(FUTA)
			static_cast<ListBox*>(m_pSCRObjForList)->nChangeAddNew = -1;
			static_cast<ListBox*>(m_pSCRObjForList)->nWhichItem  = -1;
			m_pSCRObjForList = NULL;
			return	SDS_RTNORM;
			}
		}
	catch(...)
		{
		TRACE ("failed in dlg_end_list in g_Main");
		return SDS_RTERROR;
		}

	return SDS_RTERROR;
	}

/************************************************************************************
/* + int Main::dlg_get_attr(sds_hdlg hDialog, LPTSTR wszControlKey, LPTSTR wszAttribInfo, int lInfoMaxLength, LPTSTR wszAttribNam -
*	 Retrieves the attributes value.
*
*
*
*
*/

int Main::dlg_get_attr(								// Retrieves the attributes value.
					   sds_hdlg hDialog,			// i : Handle to the dialog.
					   LPTSTR wszControlKey,		// i : Key specifies the tile.
					   LPTSTR wszAttribInfo,		// o : Attributes initial value as in the dcl description.
					   int lInfoMaxLength,			// o : Length of the value.
					   LPTSTR wszAttribName			// i : Name of the attribute.
					   )
	{
	try
		{
		ScreenObject * pScreenObject;
		CString	AttribString;
		// function gets the screen object of the key
		pScreenObject = GetDialogControl(hDialog,wszControlKey);
		if(pScreenObject != NULL)
			{
			if(lInfoMaxLength>0)
				{
				//copies the string in wszAttribInfo parameter of length given by lInfoMaxLength
				pScreenObject->GetAttribute (wszAttribName,AttribString,lInfoMaxLength);
				_tcsncpy (wszAttribInfo,AttribString,lInfoMaxLength);
				wszAttribInfo[lInfoMaxLength-1] = '\0';
				return SDS_RTNORM;
				}
			}
		}
	catch(...)
		{
		TRACE ("failed in dlg_get_attr in g_Main");
		return SDS_RTERROR;
		}
	return SDS_RTERROR;
	}

/************************************************************************************
/* + int Main::dlg_get_tile(sds_hdlg hDialog, LPTSTR wszControlKey, LPTSTR wszControlInfo, int lInfoMaxLengt -
*	 Retrieves the value of the tile.
*
*
*
*
*/

int Main::dlg_get_tile(								// Retrieves the value of the tile.
					   sds_hdlg hDialog,			// i : Handle to the dialog.
					   LPTSTR wszControlKey,		// i : Key specifies the tile.
					   LPTSTR wszControlInfo,		// o : Value of the tile.
					   int lInfoMaxLength			// i : Length allocated to value.
					   )
	{
	try
		{
		ScreenObject * pScreenObject;
		CString	valueString;
		// function gets the screen object of the key
		pScreenObject = GetDialogControl(hDialog,wszControlKey);
		if(pScreenObject != NULL)
			{
			//copies the string in wszAttribInfo parameter of length given by lInfoMaxLength
			if(lInfoMaxLength>0)
				{
				// Modified CyberAge AP 09/22/2000 [
				// Reason: If get_tile is called for radio_row it should return the
				//		   value of the currently selected radio_button.
				TreeNode * pTreeNode = pScreenObject->m_attriblist->m_pTile->m_pTreeNode;
				CString AttribType = pTreeNode->m_Tile.get_RenderableTileName();
				if( AttribType == "radio_row" || AttribType == "radio_column" ||
					AttribType == "boxed_radio_row" || AttribType == "boxed_radio_column")
					{
					int iChildCount = pTreeNode->GetChildCount();
					for(int i=0;i<iChildCount;i++)
						{
						ScreenObject * pTempScrObj = (pTreeNode->GetChild(i))->m_Tile.get_ScreenObject();
						pTempScrObj->GetTile(valueString,lInfoMaxLength);
						if( valueString == "1")
							{
							valueString = pTempScrObj->m_attriblist->get_Key();
							break;
							}
						}
					}
				else
					pScreenObject->GetTile(valueString ,lInfoMaxLength);
				// Modified CyberAge AP 09/22/2000 ]
				_tcsncpy(wszControlInfo,valueString,lInfoMaxLength);
				wszControlInfo[lInfoMaxLength-1] ='\0';
				return SDS_RTNORM;
				}
			}
		}
	catch(...)
		{
		TRACE ("failed in dlg_get_tile in g_Main");
		return SDS_RTERROR;
		}
	return SDS_RTERROR;
	}

/************************************************************************************
/* + int Main::dlg_action_tile(sds_hdlg hDialog, LPTSTR wszControlKey, void *pfnControlFun -
*	 Assigns an action to evaluate when the user selects the specified tile.
*
*
*
*
*/

int Main::dlg_action_tile(							// Assigns an action to evaluate when the user selects the specified tile.
						  sds_hdlg hDialog,			// i : Handle to the dialog.
						  LPTSTR wszControlKey,		// i : Name of the tile that triggers the action.
						  void *pfnControlFunc		// i : pointer to the function that should be called when the tile is selected.
						  )
	{
	try
		{
		ScreenObject * pScreenObject;
		CString	valueString;
		// function gets the screen object of the key
		pScreenObject = GetDialogControl(hDialog,wszControlKey);
		if(pScreenObject != NULL)
			{
			// sets the function pointer of the screenobject
//MB 22-12-1999 missing return
			return pScreenObject->ActionTile (reinterpret_cast <CLIENTFUNC>(pfnControlFunc));
			}
		}
	catch(...)
		{
		TRACE ("failed in dlg_action_tile in g_Main");
		return SDS_RTERROR;
		}
	return SDS_RTERROR;
	}

/************************************************************************************
/* + int Main::dlg_client_data_tile(sds_hdlg hDialog, LPTSTR wszControlKey, void *pApplicationDat -
*	 Associates application-managed data with the specified tile.
*
*
*
*
*/

int Main::dlg_client_data_tile(						// Associates application-managed data with the specified tile.
							   sds_hdlg hDialog,	// i : Handle to the dialog.
							   LPTSTR wszControlKey, // i : Key specifies the tile.
							   void *pApplicationData // i : data.
							   )
	{
	try
		{
		ScreenObject * pScreenObject;
		int RetVal;
		// function gets the screen object of the key
		pScreenObject = GetDialogControl(hDialog,wszControlKey);
		if(pScreenObject != NULL)
			{
			// sets the clientdata of the screen object
			RetVal = pScreenObject->set_ClientDataTile (pApplicationData);
			return RetVal;
			}
		}
	catch(...)
		{
		TRACE ("failed in dlg_client_data_tile in g_Main");
		return SDS_RTERROR;
		}
	return SDS_RTERROR;

	}

/************************************************************************************
/* + int Main::dlg_dimension_tile(sds_hdlg hDialog, LPTSTR wszControlKey, int nWidthLess1, int nHeightLess -
*		Retrieve the dimensions of a tile.
*
*
*
*
*/

int Main::dlg_dimension_tile(						//	Retrieve the dimensions of a tile.
							 sds_hdlg hDialog,		// i : Handle to the dialog.
							 LPTSTR wszControlKey,	// i : Specifies the tile.
							 short* nWidthLess1,		// o : Width of the tile.
							 short* nHeightLess1		// o : Height of the tile.
							 )
	{
	try
		{
		ScreenObject * pScreenObject;
		int RetVal;
		// function gets the screen object of the key
		pScreenObject = GetDialogControl(hDialog,wszControlKey);
		TRACE ( "\n\nControlKey ----->%s",wszControlKey);
		if(pScreenObject != NULL)
			{
			// call the function of the screenobject
			RetVal =pScreenObject->DimensionsTile ((int*) nWidthLess1 ,(int*)nHeightLess1);
			TRACE( "  X=%d,Y=%d ",*nWidthLess1,*nHeightLess1);
			return RetVal;
			}
		}
	catch(...)
		{
		TRACE ("failed in dlg_dimension_tile in g_Main");
		return SDS_RTERROR;
		}
	return SDS_RTERROR;
	}

/************************************************************************************
/* + int Main::dlg_term_dialog()
*	 Terminates all the current dialog boxes as if user has cancelled them.
*
*
*
*
*/

int Main::dlg_term_dialog()						// Terminates all the current dialog boxes as if user has cancelled them.
	{
	int index = m_ArrDlgHandles.GetSize();
	try
		{
			for ( int iLoop = (index-1) ; iLoop >= 0 ; iLoop-- )
			{
			int pStoredValue = m_ArrDlgHandles.GetAt(iLoop);
			_sds_hdlg hDialog;
			hDialog.p = pStoredValue;
			dlg_done_dialog(&hDialog,SDS_DLGALLDONE);
			}
		}
	catch(...)
		{
		TRACE(_T("Term dialog failed..."));
		return SDS_RTERROR;
		}
	return SDS_RTNORM;
	}

/************************************************************************************
/* + int Main::dlg_get_attr_string(sds_hdlg hControl, LPTSTR wszAttribName, LPTSTR wszAttribInfo, int lInfoMaxLengt -
*	 Get the value of the specified attribute using the control handle.
*
*
*
*
*/

int Main::dlg_get_attr_string(					// Get the value of the specified attribute using the control handle.
							  sds_htile hControl,	// i : Handle of the control.
							  LPTSTR wszAttribName,	// i : Name of the attribute.
							  LPTSTR wszAttribInfo,	// o : Value of the tile.
							  int lInfoMaxLength	// o : Length allocated to value.
							  )
	{
	try
		{
		int result;
		ScreenObject * pScreenObject;
		CString	AttribString;

		pScreenObject = hControl->p;
		if(pScreenObject != NULL)
			{
			if(lInfoMaxLength>0)
				{
				// call method of screen object
				if ( pScreenObject->m_nActionSetThruParent == 0 )
					{
					result = pScreenObject->GetAttribute (wszAttribName,AttribString,lInfoMaxLength);
					}
				else
					{
					 AttribString = pScreenObject->m_attriblist->GetParentKey();
					 result = SDS_RTNORM;
					}

				if ( result == SDS_RTNORM )
					{
				_tcsncpy (wszAttribInfo,AttribString,lInfoMaxLength);
            //  _tcsncpy (wszAttribInfo,"buttons",lInfoMaxLength);
				wszAttribInfo[lInfoMaxLength-1] = '\0';
					}

				return result;
				}
			}
		}
	catch(...)
		{
		TRACE ("failed in dlg_get_attr_string in g_Main");
		return SDS_RTERROR;
		}
	return SDS_RTERROR;
	}

/************************************************************************************
/* + int dlg_get_list_string(sds_hdlg hDialog,char *szControlKey,char *szListString,int nStringLength,int nWhichItem)
*
*
*		// Function to get a string from list given the index
*
*
*/

int Main::dlg_get_list_string(
								sds_hdlg hDialog,				// i : Handle to the Dialog
								char *szControlKey,				// i : Key specifies the tile.
								char *szListString,				// o : pointer to hold the string
								int nStringLength,				// i : length of requsted string
								int nWhichItem)				// i : index in the listbox
	{

	try
		{
		ScreenObject * pScreenObject;
		CString	AttribString;
		// function gets the screen object of the key
		pScreenObject = GetDialogControl(hDialog,szControlKey);
		if(pScreenObject != NULL)
			{
			if(nStringLength>0)
				{
				int	stringLength;

				// call function of screenobject (listbox) to get the string
				pScreenObject->GetListString (AttribString , &stringLength ,nWhichItem);
				_tcsncpy (szListString,AttribString,nStringLength);
				szListString[nStringLength-1] = '\0';
				return SDS_RTNORM;
				}
			}
		}
	catch(...)
		{
		TRACE ("failed in dlg_get_list_string in g_Main");
		return SDS_RTERROR;
		}
	return SDS_RTERROR;
	}


/************************************************************************************
/* + ScreenObject * Main::GetDialogControl(sds_hdlg hDialog, LPTSTR szControlKe -
*
*	function to search for control given the key and the dialog handle
*
*
*
*/

ScreenObject * Main::GetDialogControl(						// function to search for control given the key and the dialog handle
									  sds_hdlg hDialog,		// Dialog handle
									  LPTSTR szControlKey)  // key of the control
{
	ScreenObject * pScreenObject = NULL;
	try
	{

	CMapStringToPtr * m_keytoObject;
	int actuanval = hDialog->p;
	int val = actuanval;
	char string [DIALOGADDRESS];
	itoa(val,string,SUBDIALOGADDRESS);
	// look for the map of key to screenobject for a given dialog handle
	if(g_Main.m_hdlgtoDialog.Lookup (string,(CObject * &)m_keytoObject))
		{
		// look for the screenobject for the given key
		if(!m_keytoObject->Lookup (szControlKey ,(void * &)pScreenObject ))
			{
			TRACE("Key not found in dialog");
			}
		}
	else
		{
		TRACE("dialog not found");

		}
	}
	catch(...)
		{
		TRACE ("failed in GetDialogControl in g_Main");
		return NULL;
		}
	return pScreenObject;

}
/************************************************************************************
/* + int Main::RemoveHandle(int* pValue)
*	Removes an entry from m_ArrDlgHandles.
*
*
*
*
*/

int Main::RemoveHandle(
					   int pValue		// i : Value specifying what is to be removed
					   )
	{
	// Loop through all the array,find index of the input and call RemoveAt member.
	int index = 0;
	try
		{
		for ( index = 0;index < m_ArrDlgHandles.GetSize();index++ )
			{
			int  pStoredValue = m_ArrDlgHandles.GetAt(index);
			if ( pStoredValue == pValue )
				m_ArrDlgHandles.RemoveAt(index);
			}
		}
	catch(...)
		{
		TRACE(_T("RemoveHandle failed..."));
		return SDS_RTERROR;
		}
	return SDS_RTNORM;
	}


