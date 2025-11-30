/** FILENAME:     DCLDoc.cpp- Contains implementation of class CDCLDoc.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
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
#include "dcl.h"
#include "mainfrm.h"
#include "dcldoc.h"
#include "dcldialoglist.h"
#include "dclview.h"
#include "main.h"
#include "treenode.h"
#include "tree.h"
#include "fileinfo.h"
#include "sds.h"
#include "exceptionmanager.h"
#include "ScreenObject.h"
#include "errormessage.h"
extern Main g_Main;
class Tree;
const BOOL flag = 1;

#ifdef _DEBUG
const bool Debug = TRUE;
#else
const bool Debug = FALSE;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CDCLDoc, CDocument)

BEGIN_MESSAGE_MAP(CDCLDoc, CDocument)
	//{{AFX_MSG_MAP(CDCLDoc)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/************************************************************************************
/* + CDCLDoc::CDCLDoc 
*    Constructor
*
*
*
*
*/

CDCLDoc::CDCLDoc()
	{
	m_pTreeRootNode = NULL;
	m_strSelectedDlg = _T(" ");
	m_wszf2Open = _T(" ");
	m_hRoot = NULL;
	m_hParent = NULL;
	m_pTree = NULL;
	}


/************************************************************************************
/* + CDCLDoc::~CDCLDoc  
*     Destructor
*
*
*
*
*/

CDCLDoc::~CDCLDoc()
	{
	// if needed add the call to the init() function here
	g_Main.defaultAttrib.Exit();
	ASSERT( m_pTree );
	if ( m_pTree )
		m_pTree->Exit();
	}


/************************************************************************************
/* +  CDCLDoc::OnNewDocument
*
*
*
*
*
*/

BOOL CDCLDoc::OnNewDocument()
{
	if ( !CDocument::OnNewDocument() )
		return FALSE;
	return TRUE;
}



#ifdef _DEBUG
void CDCLDoc::AssertValid() const
	{
	CDocument::AssertValid();
	}

void CDCLDoc::Dump(CDumpContext& dc) const
	{
	CDocument::Dump(dc);
	}
#endif //_DEBUG


/************************************************************************************
/* + void CDCLDoc::OnFileOpen( )
*   First clean up if anything is there
*   Take the filename from the user 
*   CreateTree(filename) will create the tree to be shown
*   ShowDialogList() will show the dialog names in dcl file on the list view
*   ShowTree(dialogname) will show the tree for dialog
*/

void CDCLDoc::OnFileOpen() 
	{
	int nRes = 0;
	CString strTitle = _T("");
	TCHAR szFilter[] = _T(" DCL Files(*.dcl)|*.dcl|AllFiles(*.*) | *.*||");
	CFileDialog fOpenDialog(TRUE/*Open dialog box*/,_T(".dcl"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFilter,AfxGetMainWnd());
	if ( fOpenDialog.DoModal() == IDOK )
		{
		BeginWaitCursor();
		Init();
		m_wszf2Open = fOpenDialog.m_ofn.lpstrFile;
		CFrameWnd* pFrame = (CFrameWnd*)AfxGetMainWnd();
		ASSERT( pFrame );
		strTitle = DCL_TITLE;
		int istrlen = m_wszf2Open.GetLength();
		int ibslash = m_wszf2Open.ReverseFind(_T('\\'));
		if ( ibslash == -1 )
			{
			strTitle += m_wszf2Open; 
			}
		else
			{
			strTitle = strTitle + m_wszf2Open.Right(istrlen - ibslash);
			}
		pFrame->SetWindowText(strTitle);
		nRes = CreateTree(m_wszf2Open);
		if ( nRes == 0 )
			{
			 if ( g_Main.m_DCLParser.get_AuditLevel() == 0 )
				{
				ErrorMessage errMessageBox;
				errMessageBox.Message(ID_DCL_TEMP_ERROR);
				}
			}

		if ( nRes )
			{
			nRes = ShowDialogList();
			if ( nRes )
				{
				nRes = ShowTree(m_strSelectedDlg);
				}

			}
		EndWaitCursor();
		}
	}

/************************************************************************************
/* + CDCLDoc::CreateTree(CString& strFilename) 
*	 Create the tree in this call
*    Call the parser's DCLParserStartUp(filename) which is the entry point
*    to start parsing
*
*
*/
int CDCLDoc::CreateTree(                      // Create the tree
						CString& strFilename  // i : FileName to be parsed
						)
	{
	CString fileName = strFilename;
	int nRet = g_Main.m_DCLParser.LoadDCLFile(fileName.GetBuffer(fileName.GetLength()),&nFileID);
	if ( nRet == SDS_RTNORM )
		return 1;
	else
		return 0;
	}

/************************************************************************************
/* + CDCLDoc::ShowDialogList() 
*    To fill up the listview with the dialognames and also fill the m_strdlgArray
*    Every tree has a m_nType variable which is 0 for a dialog
*    Also set the m_strSelectedDlg variable to the first entry in the listview
*    so that the first dialog will be shown by default as the m_strSelectedDlg
*    is passed to ShowTree(dialogname).
*/

int CDCLDoc::ShowDialogList()
	{
	Tree * tree;
	int i = 0;
	CListCtrl& listCtl = m_pDialogList->GetListCtrl();
	 // Check if the list is empty and return o if it is empty.
	if (g_Main.m_DCLParser.m_ListOfDCLFileData.IsEmpty() != 0)
		return 0;
	// Get the position value that can be used to iterate the list from tail.
	POSITION pos = g_Main.m_DCLParser.m_ListOfDCLFileData.GetTailPosition();
	ASSERT ( pos );
	if ( pos )
		{
		for (  i = 0; i < g_Main.m_DCLParser.m_ListOfDCLFileData.GetCount(); i++ )
			{
			ASSERT( pos );
			FileInfo *fInfo = (FileInfo*)g_Main.m_DCLParser.m_ListOfDCLFileData.GetPrev(pos);
			ASSERT ( fInfo );
			if ( fInfo )
				{
				if ( fInfo->get_DCLFileID() == nFileID )
					{	
					POSITION pos1 = fInfo->m_RootNodes.GetHeadPosition();
					ASSERT ( pos1 );
					if ( pos1 )
						{
						for ( int nIndex = 0; nIndex < fInfo->m_RootNodes.GetCount(); nIndex++ )
							{
							ASSERT ( pos1 );
							tree = (Tree*)fInfo->m_RootNodes.GetNext(pos1);
							ASSERT( tree );
							if ( tree->m_nType == 0 )//check only for dialog
								{ 
								TreeNode * treeNode = tree->m_startTreeNode;
								try
									{
									m_strdlgArray.Add(treeNode->m_Tile.m_wszFunctionName);
									}
								catch(CMemoryException mExcp)
									{
									// This returned 0 will just suggest some error and flow will not call ShowTree(). 
									m_strdlgArray.RemoveAll();
									return 0;
									}
								}
							}
						}
					}
				}
			}
		}
	
			
	//now the StringArray is initialised
	for ( i = 0; i < m_strdlgArray.GetSize(); i++ )
		{
		LVITEM lv;
		lv.mask = LVIF_TEXT;
		lv.iItem = i;
		lv.iSubItem = 0;
		lv.pszText =  m_strdlgArray[i].GetBuffer(m_strdlgArray[i].GetLength());
		lv.cchTextMax = m_strdlgArray[i].GetLength();
		listCtl.InsertItem(&lv);
		}	
	if ( m_strdlgArray.GetSize() > 0 )
		{
		m_strSelectedDlg = m_strdlgArray[0];
		LVITEM lv;
		lv.mask = LVIF_STATE;
		lv.iItem = 0;
		lv.iSubItem = 0;
		lv.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
		lv.state = LVIS_SELECTED | LVIS_FOCUSED;
		BOOL bRes = listCtl.SetItem(&lv);
		listCtl.Update(0);
		}

	 return 1;
	}

/************************************************************************************
/* +  CDCLDoc::ShowTree(CString& strDialogName)
*     Display the tree.
*
*
*
*
*/

int CDCLDoc::ShowTree(                             //  Display the tree
					  CString& strDialogName       // i : name of the dialog to be displayed
					  )
	{
	CString strNowParent;
	CString strToInsert = _T(" ");
	BOOL bHasChildren;
	TreeNode *pOldNode = NULL,*TempOldNode = NULL;

	m_pTreeRootNode = GetRootNode(m_strSelectedDlg);
	ASSERT ( m_pTreeRootNode );
	if ( m_pTreeRootNode )
		{
		pOldNode = m_pTreeRootNode ;
		if ( m_pTreeView )
			{
			bHasChildren  =  pOldNode->HasChild();
			strToInsert = m_pTreeRootNode->m_Tile.m_wszFunctionName + _T(" { label : ")+ m_pTreeRootNode->m_Tile.m_ListOfAttributes.get_Label() + _T(" }");
			m_hParent = m_hRoot = m_pTreeView->SetRoot(strToInsert,bHasChildren);
			strToInsert = " ";
			if ( !m_hRoot )
				{
				return 0;
				}
			}
		if ( bHasChildren ) // this if statement has to be added in case there is only one node that of the dialog itself
			{
			while(1)
				{
					bHasChildren  = pOldNode->HasChild();
             		if (bHasChildren)
						{ 
						pOldNode = pOldNode->GetFirstChild();
	       				bHasChildren = pOldNode->HasChild();
						strNowParent = m_pTreeView->GetTreeCtrl().GetItemText(m_hParent);
						if (pOldNode->m_Tile.m_wszFunctionName.IsEmpty())
							strToInsert = pOldNode->m_Tile.m_wszTileName + _T(" { label : ")+ pOldNode->m_Tile.m_ListOfAttributes.get_Label() + _T(" }");
						else
							strToInsert = pOldNode->m_Tile.m_wszFunctionName + _T(" : ") + pOldNode->m_Tile.m_wszTileName + _T(" { label : ")+ pOldNode->m_Tile.m_ListOfAttributes.get_Label() + _T(" }");
					   	m_hParent = m_pTreeView->Insert( strToInsert,m_hParent,bHasChildren,pOldNode);
						strToInsert = " ";
						ASSERT( m_hParent );
						strNowParent = m_pTreeView->GetTreeCtrl().GetItemText(m_hParent);
    					}
					else
						{
		 				TempOldNode = pOldNode ;
						if (pOldNode->get_Parent() != NULL)
							{
							if ( m_hParent != m_hRoot )
								{
								m_hParent = m_pTreeView->GetTreeCtrl().GetParentItem(m_hParent); 
								strNowParent = m_pTreeView->GetTreeCtrl().GetItemText(m_hParent);
								}
								pOldNode = (pOldNode->get_Parent())->GetNextChild() ;
			   				}
						else
							{
							pOldNode = NULL;
							strNowParent = m_pTreeView->GetTreeCtrl().GetItemText(m_hParent);
							}

						if (pOldNode != NULL)
							{
							bHasChildren = pOldNode->HasChild();
							strNowParent = m_pTreeView->GetTreeCtrl().GetItemText(m_hParent);
							if (pOldNode->m_Tile.m_wszFunctionName.IsEmpty())
								strToInsert = pOldNode->m_Tile.m_wszTileName + _T(" { label : ")+ pOldNode->m_Tile.m_ListOfAttributes.get_Label() + _T(" }");
							else
								strToInsert = pOldNode->m_Tile.m_wszFunctionName + _T(" : ") + pOldNode->m_Tile.m_wszTileName + _T(" { label : ")+ pOldNode->m_Tile.m_ListOfAttributes.get_Label() + _T(" }");
							m_hParent = m_pTreeView->Insert( strToInsert,m_hParent,bHasChildren,pOldNode);
							strToInsert = " ";
							ASSERT( m_hParent );
							strNowParent = m_pTreeView->GetTreeCtrl().GetItemText(m_hParent);
		        			}		
						else
							{
							pOldNode = TempOldNode;
							pOldNode = pOldNode->get_Parent();
							strNowParent = m_pTreeView->GetTreeCtrl().GetItemText(m_hParent);
							if ( m_hParent != m_hRoot )
								{
								m_hParent = m_pTreeView->GetTreeCtrl().GetParentItem(m_hParent);
			   					strNowParent = m_pTreeView->GetTreeCtrl().GetItemText(m_hParent);			                    
								ASSERT( m_hParent );
								}
							while (1)
								{
				  				if ( pOldNode->get_Parent() )
									{
									strNowParent = m_pTreeView->GetTreeCtrl().GetItemText(m_hParent);
									TempOldNode = (pOldNode->get_Parent())->GetNextChild();
									}
								else
									{
									TempOldNode = NULL;
									}
								if (TempOldNode != NULL)
									{
									pOldNode = TempOldNode;
									bHasChildren = pOldNode->HasChild();
									strNowParent = m_pTreeView->GetTreeCtrl().GetItemText(m_hParent);
									if (pOldNode->m_Tile.m_wszFunctionName.IsEmpty())
										strToInsert = pOldNode->m_Tile.m_wszTileName + _T(" { label : ")+ pOldNode->m_Tile.m_ListOfAttributes.get_Label() + _T(" }");
									else
										strToInsert = pOldNode->m_Tile.m_wszFunctionName + _T(" : ") + pOldNode->m_Tile.m_wszTileName + _T(" { label : ")+ pOldNode->m_Tile.m_ListOfAttributes.get_Label() + _T(" }");
									m_hParent = m_pTreeView->Insert( strToInsert, m_hParent, bHasChildren,pOldNode );
									ASSERT( m_hParent );
									strNowParent = m_pTreeView->GetTreeCtrl().GetItemText(m_hParent);
									//DisplayTreeNodeDetails(pOldNode);		
									break;
									}
								else
									{
									if(pOldNode == m_pTreeRootNode)
										break;
                     					 
									pOldNode = pOldNode->get_Parent();
									if ( m_hParent != m_hRoot )
										{
										m_hParent = m_pTreeView->GetTreeCtrl().GetParentItem(m_hParent);
										ASSERT( m_hParent );
										strNowParent = m_pTreeView->GetTreeCtrl().GetItemText(m_hParent);
										}
				     				if(pOldNode == m_pTreeRootNode)
										break;
									}
								}

	           
							}

						if(pOldNode == m_pTreeRootNode)
							break;
						}
				}

			} // end of if (this will take care if there is only one node )
		if ( m_pTreeView )
			m_pTreeView->GetTreeCtrl().Expand(m_hRoot,TVE_EXPAND);
		return 1;
		}
	return 0;
	}

/************************************************************************************
/* +  CDCLDoc::GetRootNode(CString& dialogName)
*     Get the RootNode of the particular dialog
*
*
*
*
*/

TreeNode * CDCLDoc::GetRootNode(                    // Get the RootNode of the particular dialog
								CString& dialogName // i : name of the dialog
								)
	{
	TreeNode * node;
	Tree * pTree;
	CString strdlgName = dialogName ;
	pTree = g_Main.m_DCLParser.FindReferencedTree(strdlgName,nFileID);
	ASSERT( pTree );
	m_pTree = pTree;
	node = pTree->get_StartTreeNode();
	ASSERT( node );
	//int nResult = g_Main.m_DCLParser.FindReferencedNode(strdlgName,0/*dialog*/,&node);
	return node;
	}
/************************************************************************************
/* + CDCLDoc::Init()
*    First thing that is done when file open is called
*    clears all the entries for the previously opened file
*
*
*
*/
void CDCLDoc::Init()
	{
	if ( m_pTreeView )
		{
		if ( m_pTreeView->GetTreeCtrl().DeleteAllItems() )
			m_pTreeView->GetTreeCtrl().Invalidate(TRUE);
		}
	if ( m_pDialogList )
		{
		if ( m_pDialogList->GetListCtrl().DeleteAllItems() )
			m_pDialogList->GetListCtrl().Invalidate(TRUE);
		}
	m_hRoot = NULL ;
	m_hParent = NULL;
	m_strdlgArray.RemoveAll();
	m_strSelectedDlg = " ";
	m_wszf2Open = " ";
	if ( m_pTree )
		m_pTree->Exit();
	m_pTreeRootNode = NULL;
	m_MapName2Value.RemoveAll();
	SendMessage((HWND)AfxGetMainWnd(),WM_PAINT,0,0);
	}

/************************************************************************************
/* + int CDCLDoc::ShowDialog(CString strDlgName) 
*	This function is called when the user right clicks on the dialogname
*	and selects ShowDialog.Here after init() there are the algorithms 
*	to calculate width,height , then adjusting the width height and then 
*	calculating the positions.CreateOnScreenObjects then calls Create on each tile.
*
*/
int CDCLDoc::ShowDialog(						// Displays the dialog box.
						CString strDlgName		// i : String specifying the name of the dialog.
						)
	{
	Tree *pTree = g_Main.m_DCLParser.FindReferencedTree(strDlgName,nFileID);
	ASSERT( pTree );
	int nRes;
	nRes = pTree->Init();
	if ( nRes != SDS_RTERROR )
		{
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
		if ( flag )
			pTree->ReAdjust();
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
		CRect lRect;
		UINT nFlags = SWP_NOMOVE;
		lRect = pTree->get_StartTreeNode()->m_Tile.get_ScreenObject()->GetDisplayRect();
		pTree->get_StartTreeNode()->m_Tile.get_ScreenObject()->GetWindow()->MoveWindow(lRect);
		pTree->get_StartTreeNode()->m_Tile.get_ScreenObject()->GetWindow()->ShowWindow(SW_SHOW);
		}
	return 1;
	}
